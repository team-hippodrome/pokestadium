#include "pikachu_reward.h"
#include "src/geo_render.h"
#include "src/model_animation.h"
#include "src/display_object_textures.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/ui_graphics.h"
#include "src/pokemon_stats.h"
#include "src/text_system.h"
#include "src/gb_save.h"
#include "src/jpeg_stream.h"
#include "src/audio_fade.h"
#include "src/audio_stored_fade.h"
#include "src/audio_sfx.h"
#include "src/cry.h"
#include "src/gfx_buffer.h"
#include "src/gfx_rect.h"
#include "src/DDC0.h"
#include "src/matrix.h"
#include "src/geo_layout.h"
#include "src/memory.h"
#include "src/stage_loader.h"
#include "src/util.h"

// Scene graph built from sSceneGeoLayout; its first child is the camera node.
static GraphNode* sSceneRootNode;
// Geo node built from D_8C201070 (fragment _798CD0) for the three follower models.
static GraphNode* sFollowerGeoNode;
// Battle model of the cartridge Pikachu, from Model_LoadForPokemon.
static unk_D_86002F30* sPikachuModel;
static char** sMoveNameTable;
static char** sPikachuSurfText; // text table 28: the Surfing Pikachu reward dialogue
// [0] = Pikachu model, [1..3] = follower models shown while a move is forgotten.
static DisplayObject sSceneObjects[4];
static unk_D_832025C8 sAdvanceIcon;
static RewardPikachuSource sTargetPikachu;

// Set when an ending starts; the Pikachu model is hidden once its current animation ends.
static s32 sHideLeadWhenAnimEnds = 0;
// Frame counter for the follower models; -1 = inactive (all hidden).
static s32 sFollowerTimer = -1;
// Last message shown in the main text box; scrolled out by PikachuReward_ScrollTextIn.
static char* sPreviousText = NULL;
// Geo layout for the reward scene (commands per GeoLayoutJumpTable in src/geo_layout.c):
// camera (0x0B, 320x240), an orthographic pass (0x0E) drawing the full-screen background quad
// display list 0x8C200060 (vertex colors #FF3232 at the top to #FF9B32 at the bottom), light and
// ambient light (0x14, 0x16), then a perspective pass (0x0F) with a model part (0x1F) whose
// callback is PikachuReward_SpinSunburstRays and whose child is the sunburst geo D_8C200920,
// plus a reference (0x0A) to D_800AC840, the display root used by ModelRenderer_AttachDisplayObject.
//@src/geo_layout.c
static u32 sSceneGeoLayout[] = {
    0x0C000000,  0x05000000, 0x0B00001E, 0x00000000, 0x014000F0, 0x00000320,
    0x00000000,  0x000005DC, 0x05000000, 0x0D000000, 0x05000000, 0x0E000002,
    0x05000000,  0x22010000, 0x8C200060, 0x06000000, 0x14000000, 0x002B0012,
    0xFFFFFF32,  0x16FFFFFF, 0x0F000003, 0x05000000, 0x1F000000, 0x00000000,
    0x00000000,  0x003CFF06, 0x00640064, 0x00640000, 0x08000000, PikachuReward_SpinSunburstRays,
    0x00000000,  0x05000000, 0x00000000, D_8C200920, 0x06000000, 0x0A000000,
    &D_800AC840, 0x06000000, 0x06000000, 0x06000000, 0x06000000, 0x01000000,
};

/**
 * Geo layout callback for the sunburst behind Pikachu (sSceneGeoLayout -> D_8C200920).
 * D_8C200920 in fragment _798CD0 draws six 142x800 quads at rotationZ steps of 0x2AAA
 * using a 32x32 I4 vertical-gradient texture tinted yellow (#FFFF00), i.e. radial light
 * rays. On callback stage 2 this spins them by -0x80 per frame about the Z axis and keeps
 * their center 70 units above the Pikachu model (sSceneObjects[0]).
 */
s32 PikachuReward_SpinSunburstRays(s32 callback_stage, DisplayObject* object) {
    if (callback_stage == 2) {
        object->rotation.z -= 0x80;
        object->position.x = sSceneObjects->position.x;
        object->position.y = sSceneObjects->position.y + 70.0f;
    }
    return 0;
}

/**
 * Returns 1 unless one of the Pokémon's known moves has an ID of 0xA6 (166) or higher,
 * i.e. outside the 165 Generation I moves (glitch moves). Scanning stops at the first
 * empty slot. Copy of EventBattle_HasLegalMoves.
 * //@src/fragments/event_battle/event_battle.c
 */
s32 PikachuReward_HasOnlyValidMoves(BattleMon* pokemon) {
    s32 i;
    s32 is_valid = 1;

    for (i = 0; i < 4; i++) {
        if (pokemon->moves[i] == 0) {
            break;
        }

        if (pokemon->moves[i] >= 0xA6) {
            is_valid = 0;
        }
    }

    return is_valid;
}

/**
 * Cup legality test for one Pokémon: species 1..151, only valid moves, the species'
 * D_8006FF00 rule mask shares a bit with `arg3`, and level is within arg1..arg2.
 * Same per-Pokémon test as EventBattle_CheckTeamLegality, whose rule table gives the
 * Prime Cup level 1..100 and mask 0x20; PikachuReward_SearchDeckForPikachu passes those
 * same Prime Cup values.
 * //@src/fragments/event_battle/event_battle.c
 */
s32 PikachuReward_MeetsCupRules(BattleMon* pokemon, s16 min_level, s16 max_level, s16 species_rule_mask) {
    s32 is_legal = 0;

    if ((pokemon->species.id > 0) && (pokemon->species.id < 0x98)) {
        if ((PikachuReward_HasOnlyValidMoves(pokemon) != 0) && (D_8006FF00[pokemon->species.id - 1].unk_0C & species_rule_mask)) {
            if ((pokemon->level >= min_level) && (max_level >= pokemon->level)) {
                is_legal = 1;
            }
        }
    }
    return is_legal;
}

/**
 * Opens the party (arg1 == 0) or PC box arg1 - 1 on the Game Boy cartridge in Transfer
 * Pak port arg0 and walks its entries into sTargetPikachu.pokemon. arg2 counts only
 * entries that pass Prime Cup rules (PikachuReward_MeetsCupRules); when that entry is
 * reached, returns its raw slot index if it is a Pikachu (species 25), otherwise -1.
 * The raw index is later used by PikachuReward_SaveToCartridge to write it back.
 */
s32 PikachuReward_SearchDeckForPikachu(s16 port, s16 box_index, s16 legal_index) {
    UNUSED s32 pad[22];
    s32 found_pikachu;
    s16 deck_slot;
    DeckHandle* deck;
    BattleMon* ptr = &sTargetPikachu.pokemon;

    found_pikachu = 0;
    deck_slot = 0;

    if (box_index == 0) {
        deck = Deck_Open(0x20, port, 0, 0);
    } else {
        deck = Deck_Open(0x21, port, box_index - 1, 0);
    }

    if (deck != NULL) {
        while (Deck_ReadEntries(&ptr->species.id, 1, deck) == 1) {
            if (PikachuReward_MeetsCupRules(ptr, 1, 0x64, 0x20) != 0) {
                if (!legal_index--) {
                    if (ptr->species.id == 0x19) {
                        found_pikachu = 1;
                    }
                    break;
                }
            }
            deck_slot++;
        }
        Deck_CloseAndFlush(deck);
    }

    if (found_pikachu != 0) {
        return deck_slot;
    }
    return -1;
}

/**
 * Finds the first Pikachu (species 25) among player 1's brought Pokémon and resolves where
 * it lives on the Game Boy cartridge: BattleMon.unk_52 bits 0x70 = Transfer Pak port,
 * bits 0xF = party (0) / PC box + 1, and unk_53 = index among Prime Cup-legal entries.
 * Loads that cartridge save and fills sTargetPikachu. Returns 1 when the Pikachu was
 * located on the cartridge, else 0 (the fragment then exits without showing anything).
 * BattlePrep_CheckPikachuNeedsSurf performs the same unk_52 range checks before setting
 * transitionFlags 0x1000, which makes game_state.c load this fragment.
 * //@src/fragments/battle_prep/battle_prep_394120.c
 */
s32 PikachuReward_FindPikachuOnCart(void) {
    s32 i;
    TeamRoster* roster;

    roster = gSessionContext.battleSides[0].tagTeamMember[0];

    for (i = 0; i < roster->numberOfPokemonBrought; i++) {
        if (roster->pokemonPicked[i].species.id == 0x19) {
            sTargetPikachu.controllerPort = (roster->pokemonPicked[i].unk_52 & 0x70) >> 4;
            sTargetPikachu.boxIndex = roster->pokemonPicked[i].unk_52 & 0xF;
            if ((sTargetPikachu.controllerPort < 4) && (sTargetPikachu.boxIndex < 0xD)) {
                if (GbSave_LoadPort(sTargetPikachu.controllerPort) != 0) {
                    sTargetPikachu.deckSlot = PikachuReward_SearchDeckForPikachu(sTargetPikachu.controllerPort, sTargetPikachu.boxIndex,
                                                                           roster->pokemonPicked[i].unk_53);
                    //!bug deckSlot is a u8, so the -1 "not found" result becomes 255 and this
                    // test always passes (the compiled code masks with 0xFF before bltz).
                    if (sTargetPikachu.deckSlot >= 0) {
                        return 1;
                    }
                }
            }
            return 0;
        }
    }

    return 0;
}

/**
 * Returns the number of filled move slots (0..4) of sTargetPikachu.pokemon, counting up to
 * the first empty slot.
 */
s32 PikachuReward_CountKnownMoves(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        if (sTargetPikachu.pokemon.moves[i] == 0) {
            break;
        }
    }
    return i;
}

/**
 * Per-frame animation upkeep for the Pikachu model (sSceneObjects[0]). Unless animation 10
 * or 5 is playing, triggers event track 10 every 60 frames (idle flourish). When a non-idle
 * animation finishes, either returns to idle animation 0 or, once an ending has started
 * (sHideLeadWhenAnimEnds), clears node flag bit 0 to hide the model.
 */
void PikachuReward_UpdateLeadPikachuAnimation(void) {
    static s32 sIdleEventTimer = 0; // frames since the last idle event track trigger

    if ((sSceneObjects[0].transformAnim.animationId != 0xA) && (sSceneObjects[0].transformAnim.animationId != 5)) {
        sIdleEventTimer++;
        if (sIdleEventTimer == 0x3C) {
            sIdleEventTimer = 0;
            ModelAnim_SetEventTrack(&sSceneObjects[0], -1);
            ModelAnim_SetEventTrack(&sSceneObjects[0], 0xA);
        }
    }

    if ((sSceneObjects[0].transformAnim.animationId != 0) && (ModelAnim_IsFinished(&sSceneObjects[0]) != 0)) {
        if (sHideLeadWhenAnimEnds != 0) {
            sSceneObjects[0].node.flags &= ~1;
        } else {
            ModelAnim_SetAnimation(&sSceneObjects[0], 0);
        }
    }
}

/**
 * Drives the three small models sSceneObjects[1..3] during the move-forgetting sequence
 * ("1, 2 and... Poof!"). sFollowerTimer counts frames from 0; the models are shown at
 * frames 0, 8 and 16 and hidden again at frames 70, 78 and 86. A timer of -1 keeps all
 * three hidden. The models come from D_8C201070; what they depict is unconfirmed.
 */
void PikachuReward_UpdateFollowerVisibility(void) {
    if (sFollowerTimer == -1) {
        sSceneObjects[1].node.flags &= ~1;
        sSceneObjects[2].node.flags &= ~1;
        sSceneObjects[3].node.flags &= ~1;
        return;
    }

    switch (sFollowerTimer) {
        case 0x0:
            sSceneObjects[1].node.flags |= 1;
            break;

        case 0x8:
            sSceneObjects[2].node.flags |= 1;
            break;

        case 0x10:
            sSceneObjects[3].node.flags |= 1;
            break;

        case 0x46:
            sSceneObjects[1].node.flags &= ~1;
            break;

        case 0x4E:
            sSceneObjects[2].node.flags &= ~1;
            break;

        case 0x56:
            sSceneObjects[3].node.flags &= ~1;
            break;
    }

    if (sFollowerTimer < 0x57) {
        sFollowerTimer++;
    }
}

/**
 * Draws one frame of the 3D scene: advances the frame counter, updates the Pikachu and
 * follower models, clears the color buffer and renders the scene graph sSceneRootNode.
 * Also used as the per-frame callback for BgStage_RunUntilCondition. Always returns 0.
 */
s32 PikachuReward_RenderFrame(u8 arg0) {
    GeoRender_AdvanceFrameCounter();
    PikachuReward_UpdateLeadPikachuAnimation();
    PikachuReward_UpdateFollowerVisibility();
    BgStage_DrawFrame();
    GfxImage_FillCurrent(&gDisplayListHead, 1);
    Geo_RenderRootNode(sSceneRootNode);
    return 0;
}

/**
 * Draws the "press A to continue" glyph (character 0x23 of the active font) at (arg0, arg1)
 * with a pulsing alpha: fades in over 16 frames, holds at 0xFF, fades out, then stays
 * hidden until the 61-frame cycle repeats. Shown by PikachuReward_ShowTextWaitForA.
 */
void PikachuReward_DrawBlinkingStarIcon(s16 x, s16 y) {
    s32 alpha;

    if (sAdvanceIcon.blinkTimer < 0x10) {
        alpha = sAdvanceIcon.blinkTimer * 0x10;
    } else if (sAdvanceIcon.blinkTimer < 0x23) {
        alpha = 0xFF;
    } else if (sAdvanceIcon.blinkTimer < 0x32) {
        alpha = (-sAdvanceIcon.blinkTimer * 0x10) + 0x320;
    } else {
        alpha = 0;
    }

    sAdvanceIcon.blinkTimer++;
    if (sAdvanceIcon.blinkTimer >= 0x3D) {
        sAdvanceIcon.blinkTimer = 0;
    }

    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, alpha);
    Font_DrawCharAt(x, y, 0x23);
}

/**
 * Draws a text box background at (arg0, arg1) with size arg2 x arg3 as a dark blue
 * (#000032) to blue (#0000C8) gradient panel.
 */
void PikachuReward_DrawTextBoxPanel(s16 x, s16 y, s16 width, s16 height) {
    Color_RGB8 dark_color = { 0x00, 0x00, 0x32 };
    Color_RGB8 light_color = { 0x00, 0x00, 0xC8 };

    Ui_DrawGradientPanel(x, y, width, height, &dark_color, &light_color);
}

/**
 * Draws the 24x14 RGBA16 menu cursor texture D_8C200090 (fragment _798CD0) at (arg0, arg1).
 * Used to mark the selected Yes/No answer and the selected move.
 */
void PikachuReward_DrawSelectionCursor(s16 x, s16 y) {
    gSPDisplayList(gDisplayListHead++, D_8006F4E0);

    Gfx_DrawTextureRgba16(x, y, 0x18, 0xE, Util_ConvertAddrToVirtAddr(D_8C200090), 0x18, 0x200000);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

/**
 * Shows message arg0 in the main text box for arg1 frames without waiting for input, then
 * records it as sPreviousText for the next PikachuReward_ScrollTextIn.
 */
void PikachuReward_ShowTextForFrames(char* text, s32 frames) {
    while (frames-- > 0) {
        Controller_PollInputs();
        PikachuReward_RenderFrame(0);
        PikachuReward_DrawTextBoxPanel(0x1E, 0x20, 0x104, 0x30);
        Font_BeginTranslucentTextRendering();
        Font_SetActive(2, 0);
        Font_SetLineHeight(0x10);
        Font_Printf(0x2E, 0x28, text);
        Font_EndTexturedTextRendering();
        BgStage_AdvanceFrame();
    }

    sPreviousText = text;
}

/**
 * Shows message arg0 in the main text box until A is pressed, then plays the menu scroll
 * sound and records the message as sPreviousText. When arg1 is nonzero, the blinking
 * continue glyph is drawn in the box's lower right corner.
 */
void PikachuReward_ShowTextWaitForA(char* text, s32 show_continue_icon) {
    sAdvanceIcon.blinkTimer = 0x10;

    do {
        Controller_PollInputs();
        PikachuReward_RenderFrame(0);
        PikachuReward_DrawTextBoxPanel(0x1E, 0x20, 0x104, 0x30);
        Font_BeginTranslucentTextRendering();
        Font_SetActive(2, 0);
        Font_SetLineHeight(0x10);
        Font_Printf(0x2E, 0x28, text);
        Font_SetActive(4, 0);
        if (show_continue_icon != 0) {
            PikachuReward_DrawBlinkingStarIcon(0x104, 0x38);
        }
        Font_EndTexturedTextRendering();
        BgStage_AdvanceFrame();
    } while (!BTN_IS_PRESSED(gPlayer1Controller, BTN_A));

    Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
    sPreviousText = text;
}

/**
 * Eight-frame transition in the main text box: the previous message (sPreviousText)
 * scrolls up out of a scissor window while the new message arg0 scrolls up into view.
 */
void PikachuReward_ScrollTextIn(s8* text) {
    s32 i;
    s32 x1 = 0x1E;
    s32 x2 = 0x122;
    s32 y1 = 0x28;
    s32 y2 = 0x48;

    for (i = 1; i < 9; i++) {
        Controller_PollInputs();
        PikachuReward_RenderFrame(0);
        PikachuReward_DrawTextBoxPanel(0x1E, 0x20, 0x104, 0x30);

        gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, x1, y1, x2, y2);

        Font_BeginTranslucentTextRendering();
        Font_SetActive(2, 0);
        Font_SetLineHeight(0x10);
        Font_Printf(0x2E, y1 - i * 4, sPreviousText);
        Font_Printf(0x2E, y2 - i * 4, text);
        Font_EndTexturedTextRendering();
        BgStage_AdvanceFrame();
    }

    sPreviousText = text;
}

/**
 * Shows question arg0 with a Yes/No menu (strings 16 and 17 of text table 28). D-pad up or
 * down toggles, A confirms, B selects No. Returns 1 for Yes and 0 for No.
 */
s32 PikachuReward_ConfirmYesNoPrompt(char* question) {
    s32 is_yes;
    s32 blue;
    s32 cursor;
    s32 is_waiting;

    is_waiting = 1;
    cursor = 0;

    while (is_waiting != 0) {
        Controller_PollInputs();
        if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DUP | BTN_DDOWN)) {
            Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
            cursor ^= 1;
        } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
            is_waiting = 0;
        } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_B)) {
            cursor = 1;
            is_waiting = 0;
        }

        is_yes = cursor == 0;
        PikachuReward_RenderFrame(0);
        PikachuReward_DrawTextBoxPanel(0x1E, 0x20, 0x104, 0x30);
        PikachuReward_DrawTextBoxPanel(0xD2, 0x54, 0x50, 0x2C);
        PikachuReward_DrawSelectionCursor(0xDA, (cursor * 0x10) + 0x5C);
        Font_BeginTranslucentTextRendering();
        Font_SetActive(2, 0);
        Font_SetLineHeight(0x10);
        Font_Printf(0x2E, 0x28, question);

        if (is_yes != 0) {
            blue = 0;
        } else {
            blue = 0xFF;
        }

        Gfx_SetEnvColor(0xFF, 0xFF, blue, 0xFF);
        Font_Printf(0xF2, 0x5C, Text_GetString(NULL, 0, sPikachuSurfText, 0x10));

        if (cursor == 1) {
            blue = 0;
        } else {
            blue = 0xFF;
        }

        Gfx_SetEnvColor(0xFF, 0xFF, blue, 0xFF);
        Font_Printf(0xF2, 0x6C, Text_GetString(NULL, 0, sPikachuSurfText, 0x11));
        Font_EndTexturedTextRendering();
        BgStage_AdvanceFrame();
    }

    if (is_yes != 0) {
        Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
    } else {
        Audio_PlaySoundEffectById(SFX_MENU_BACK);
    }
    return is_yes;
}

/**
 * Shows prompt arg0 ("Which move should be forgotten?") next to a list of the Pikachu's four
 * moves. HM moves Cut (15), Fly (19), Strength (70) and Flash (148) cannot be chosen and
 * play an error sound. Returns the chosen move slot 0..3, or -1 if B is pressed.
 */
s32 PikachuReward_SelectMoveToForget(char* prompt) {
    s32 blue;
    s32 cursor;
    s32 is_waiting;

    is_waiting = 1;
    cursor = 0;

    while (is_waiting != 0) {
        Controller_PollInputs();

        if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DUP)) {
            Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
            cursor--;
            if (cursor < 0) {
                cursor = 3;
            }
        }

        if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DDOWN)) {
            Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
            cursor++;
            if (cursor >= 4) {
                cursor = 0;
            }
        } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
            if ((sTargetPikachu.pokemon.moves[cursor] == 0xF) || (sTargetPikachu.pokemon.moves[cursor] == 0x13) ||
                (sTargetPikachu.pokemon.moves[cursor] == 0x46) || (sTargetPikachu.pokemon.moves[cursor] == 0x94)) {
                Audio_PlaySoundEffectById(SFX_MENU_ERROR);
            } else {
                is_waiting = 0;
                Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
            }
        } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_B)) {
            is_waiting = 0;
            cursor = -1;
            Audio_PlaySoundEffectById(SFX_MENU_BACK);
        }

        PikachuReward_RenderFrame(0);
        PikachuReward_DrawTextBoxPanel(0x1E, 0x20, 0x82, 0x30);
        Font_BeginTranslucentTextRendering();
        Font_SetActive(2, 0);
        Font_SetLineHeight(0x10);
        Font_Printf(0x2E, 0x28, prompt);
        Font_EndTexturedTextRendering();
        PikachuReward_DrawTextBoxPanel(0x96, 0x20, 0x8C, 0x4C);
        PikachuReward_DrawSelectionCursor(0x9E, (cursor * 0x10) + 0x28);
        Font_BeginTranslucentTextRendering();
        Font_SetActive(2, 0);
        Font_SetLineHeight(0x10);

        if (cursor == 0) {
            blue = 0;
        } else {
            blue = 0xFF;
        }

        Gfx_SetEnvColor(0xFF, 0xFF, blue, 0xFF);
        Font_Printf(0xB6, 0x28, Text_GetString(NULL, 0, sMoveNameTable, sTargetPikachu.pokemon.moves[0] - 1));

        if (cursor == 1) {
            blue = 0;
        } else {
            blue = 0xFF;
        }

        Gfx_SetEnvColor(0xFF, 0xFF, blue, 0xFF);
        Font_Printf(0xB6, 0x38, Text_GetString(NULL, 0, sMoveNameTable, sTargetPikachu.pokemon.moves[1] - 1));

        if (cursor == 2) {
            blue = 0;
        } else {
            blue = 0xFF;
        }

        Gfx_SetEnvColor(0xFF, 0xFF, blue, 0xFF);
        Font_Printf(0xB6, 0x48, Text_GetString(NULL, 0, sMoveNameTable, sTargetPikachu.pokemon.moves[2] - 1));

        if (cursor == 3) {
            blue = 0;
        } else {
            blue = 0xFF;
        }

        Gfx_SetEnvColor(0xFF, 0xFF, blue, 0xFF);
        Font_Printf(0xB6, 0x58, Text_GetString(NULL, 0, sMoveNameTable, sTargetPikachu.pokemon.moves[3] - 1));

        Font_EndTexturedTextRendering();
        BgStage_AdvanceFrame();
    }

    return cursor;
}

/**
 * Ending when the player refuses Surf: shows "PIKACHU did not learn SURF.", turns the Pikachu
 * model and plays animation 5, then fades the screen and music out.
 */
void PikachuReward_ShowDeclinedEnding(void) {
    PikachuReward_ScrollTextIn(Text_GetString(NULL, 0, sPikachuSurfText, 4));
    sSceneObjects->rotation.y = -0x800;
    ModelAnim_SetAnimation(sSceneObjects, 5);
    ModelAnim_SetEventTrack(sSceneObjects, 0);
    sHideLeadWhenAnimEnds = 1;
    PikachuReward_ShowTextForFrames(Text_GetString(NULL, 0, sPikachuSurfText, 4), 0x46);
    StageFade_Start(0x1E);
    Audio_StopMusic(0x3C);
    PikachuReward_ShowTextForFrames(Text_GetString(NULL, 0, sPikachuSurfText, 4), 0x1E);
}

/**
 * Ending after Surf has been saved: plays Pikachu animation 10, the Surf move sound
 * (PlayMoveSoundEffect with move 57 and species 25) and music track 0x4A, shows
 * "Take good care of PIKACHU!", then fades out.
 */
void PikachuReward_ShowSuccessEnding(void) {
    ModelAnim_SetAnimation(sSceneObjects, 0xA);
    ModelAnim_SetEventTrack(sSceneObjects, 7);
    PlayMoveSoundEffect(0, 0x39, 0x19, 0);
    sHideLeadWhenAnimEnds = 1;
    Audio_PlayMusicIfChanged(0x4A);
    PikachuReward_ScrollTextIn(Text_GetString(NULL, 0, sPikachuSurfText, 0xF));
    PikachuReward_ShowTextForFrames(Text_GetString(NULL, 0, sPikachuSurfText, 0xF), 0x98);
    StageFade_Start(0x1E);
    PikachuReward_ShowTextForFrames(Text_GetString(NULL, 0, sPikachuSurfText, 0xF), 0x1E);
}

/**
 * 120-frame intro: lowers the camera eye and target from height 800 to 320 while spinning the
 * Pikachu model about the Y axis by 0x200 per frame.
 */
void PikachuReward_PlayIntroCamera(void) {
    s32 i;
    CameraNode* camera = sSceneRootNode->children;

    sSceneObjects->rotation.y = 0x1000;

    for (i = 0; i < 120; i++) {
        Controller_PollInputs();
        camera->view.targetPosition.y = 0x320 - ((i * 0x1E0) / 120);
        camera->view.eyePosition.y = 0x320 - ((i * 0x1E0) / 120);
        sSceneObjects->rotation.y += 0x200;
        PikachuReward_RenderFrame(0);
        BgStage_AdvanceFrame();
    }
}

/**
 * Grows the text box open over 8 frames, shows the congratulation and "Special reward!
 * PIKACHU may now learn SURF!" messages, then asks whether Pikachu should learn Surf.
 * Answering No asks "Give up on PIKACHU learning SURF?"; confirming that runs the declined
 * ending. Returns 1 if the player wants Surf, 0 if they gave up.
 */
s32 PikachuReward_ShowIntroAndConfirm(void) {
    s16 box_width = 0x14;
    s16 box_height = 0x10;
    s32 i;

    ModelAnim_SetAnimation(sSceneObjects, 8);

    for (i = 0; i < 8; i++) {
        Controller_PollInputs();
        PikachuReward_RenderFrame(0);
        PikachuReward_DrawTextBoxPanel(0xA0 - (box_width / 2), 0x38 - (box_height / 2), box_width, box_height);
        BgStage_AdvanceFrame();
        box_width += 0x1E;
        box_height += 4;
    }

    PikachuReward_ShowTextWaitForA(Text_GetString(NULL, 0, sPikachuSurfText, 0), 1);
    PikachuReward_ScrollTextIn(Text_GetString(NULL, 0, sPikachuSurfText, 1));
    PikachuReward_ShowTextWaitForA(Text_GetString(NULL, 0, sPikachuSurfText, 1), 1);

    while (true) {
        PikachuReward_ScrollTextIn(Text_GetString(NULL, 0, sPikachuSurfText, 2));
        if (PikachuReward_ConfirmYesNoPrompt(Text_GetString(NULL, 0, sPikachuSurfText, 2)) != 0) {
            return 1;
        }

        PikachuReward_ScrollTextIn(Text_GetString(NULL, 0, sPikachuSurfText, 3));
        if (PikachuReward_ConfirmYesNoPrompt(Text_GetString(NULL, 0, sPikachuSurfText, 3)) != 0) {
            PikachuReward_ShowDeclinedEnding();
            return 0;
        }
    }
}

/**
 * Chooses the move slot that will receive Surf. With fewer than four moves, returns the first
 * empty slot. With four, asks the player to forget a move (strings 5..13 of text table 28),
 * plays the "1, 2 and... Poof!" sequence with the follower models and the Amnesia move sound,
 * and returns the forgotten slot. Returns -1 if the player gives up (declined ending shown).
 */
s32 PikachuReward_ResolveMoveSlot(void) {
    s32 forgotten_slot;
    char forgot_text[0x80];
    s32 move_count;

    move_count = PikachuReward_CountKnownMoves();
    if (move_count == 4) {
        while (true) {
            PikachuReward_ScrollTextIn(Text_GetString(NULL, 0, sPikachuSurfText, 5));
            PikachuReward_ShowTextWaitForA(Text_GetString(NULL, 0, sPikachuSurfText, 5), 1);
            PikachuReward_ScrollTextIn(Text_GetString(NULL, 0, sPikachuSurfText, 6));
            PikachuReward_ShowTextWaitForA(Text_GetString(NULL, 0, sPikachuSurfText, 6), 1);
            PikachuReward_ScrollTextIn(Text_GetString(NULL, 0, sPikachuSurfText, 7));

            if (PikachuReward_ConfirmYesNoPrompt(Text_GetString(NULL, 0, sPikachuSurfText, 7)) != 0) {
                PikachuReward_ScrollTextIn(Text_GetString(NULL, 0, sPikachuSurfText, 9));

                forgotten_slot = PikachuReward_SelectMoveToForget(Text_GetString(NULL, 0, sPikachuSurfText, 9));
                if (forgotten_slot >= 0) {
                    PikachuReward_ShowTextForFrames(Text_GetString(NULL, 0, sPikachuSurfText, 0xA), 0x1E);
                    ModelAnim_SetAnimation(sSceneObjects, 9);
                    sFollowerTimer = 0;
                    Effect_TriggerMoveVisual(0x85, 0x19, 0);
                    PikachuReward_ShowTextWaitForA(Text_GetString(NULL, 0, sPikachuSurfText, 0xB), 1);
                    Text_SetStringToken(0x1D, Text_GetString(NULL, 0, sMoveNameTable, sTargetPikachu.pokemon.moves[forgotten_slot] - 1));
                    Text_GetString(forgot_text, sizeof(forgot_text), sPikachuSurfText, 0xC);
                    Audio_StopMusic(0x3C);
                    PikachuReward_ScrollTextIn(forgot_text);
                    PikachuReward_ShowTextWaitForA(forgot_text, 1);
                    PikachuReward_ScrollTextIn(Text_GetString(NULL, 0, sPikachuSurfText, 0xD));
                    PikachuReward_ShowTextWaitForA(Text_GetString(NULL, 0, sPikachuSurfText, 0xD), 1);
                    sFollowerTimer = -1;
                    return forgotten_slot;
                }
            }

            PikachuReward_ScrollTextIn(Text_GetString(NULL, 0, sPikachuSurfText, 8));

            if (PikachuReward_ConfirmYesNoPrompt(Text_GetString(NULL, 0, sPikachuSurfText, 8)) != 0) {
                PikachuReward_ShowDeclinedEnding();
                return -1;
            }
        }
    }

    Audio_StopMusic(0x3C);
    return move_count;
}

/**
 * Draws one frame of the cartridge save status over "PIKACHU learned SURF!". status: 0 shows
 * "Saving...", 1 shows the reattach instructions for the GB Game Pak, 2 adds "Save Error",
 * 3 adds "The GB Game Pak or the data has been changed." (strings 18..21 of text table 28).
 * The VERSION_US_REV2 build uses a different, still-undecompiled version of this function
 * (its assembly also calls Font_MeasureTextExtent).
 */
#if defined(VERSION_US_REV2)
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/pikachu_reward/pikachu_reward/PikachuReward_ShowSaveStatusScreen.s")
#else
void PikachuReward_ShowSaveStatusScreen(s32 status) {
    PikachuReward_RenderFrame(0);
    PikachuReward_DrawTextBoxPanel(0x1E, 0x20, 0x104, 0x30);

    if (status == 0) {
        PikachuReward_DrawTextBoxPanel(0x4C, 0x64, 0xA8, 0x28);
    } else {
        PikachuReward_DrawTextBoxPanel(0x30, 0x54, 0xE0, 0x78);
    }

    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);
    Font_SetLineHeight(0x10);
    Font_Printf(0x2E, 0x28, Text_GetString(NULL, 0, sPikachuSurfText, 0xE));

    if (status == 0) {
        Font_BeginTranslucentTextRendering();
        Font_SetActive(4, 0);
        Font_Printf(0x5C, 0x70, Text_GetString(NULL, 0, sPikachuSurfText, 0x12));
        Font_EndTexturedTextRendering();
    } else {
        Font_SetLineHeight(0x10);
        Font_Printf(0x40, 0x84, Text_GetString(NULL, 0, sPikachuSurfText, 0x13));
        Gfx_SetEnvColor(0xFF, 0xFF, 0x64, 0xFF);

        if (status == 2) {
            Font_SetActive(4, 0);
            Font_Printf(0x4C, 0x64, Text_GetString(NULL, 0, sPikachuSurfText, 0x14));
        } else if (status == 3) {
            Font_SetLineHeight(0x10);
            Font_Printf(0x40, 0x5C, Text_GetString(NULL, 0, sPikachuSurfText, 0x15));
        }
    }

    Font_EndTexturedTextRendering();
    BgStage_AdvanceFrame();
}
#endif

/**
 * Writes sTargetPikachu.pokemon back to its party/box slot on the Game Boy cartridge and
 * commits the cartridge save. While the write fails, polls the Transfer Pak and shows the
 * matching error or reattach screen until the pak is ready, then retries.
 */
void PikachuReward_SaveToCartridge(void) {
    s32 status_screen;
    s32 pak_status;
    DeckHandle* deck;

    if (sTargetPikachu.boxIndex == 0) {
        deck = Deck_Open(0x20, sTargetPikachu.controllerPort, 0, 2);
    } else {
        deck = Deck_Open(0x21, sTargetPikachu.controllerPort, sTargetPikachu.boxIndex - 1, 2);
    }

    Deck_SetCursor(deck, sTargetPikachu.deckSlot);
    Deck_WriteEntries(&sTargetPikachu.pokemon, 1, deck);
    Deck_CloseAndFlush(deck);
    PikachuReward_ShowSaveStatusScreen(0);
    PikachuReward_ShowSaveStatusScreen(0);
    GbSave_ValidateBeforeWrite(sTargetPikachu.controllerPort);

    while (GbSave_RebuildAndWriteSave(sTargetPikachu.controllerPort) == 0) {
        status_screen = 2;
        do {
            pak_status = GbSave_PollCartRemoval(sTargetPikachu.controllerPort);
            if (pak_status == 1) {
                status_screen = 1;
            }

            if (pak_status == 3) {
                status_screen = 3;
            }

            PikachuReward_ShowSaveStatusScreen(status_screen);
        } while (pak_status != 2);

        PikachuReward_ShowSaveStatusScreen(0);
        PikachuReward_ShowSaveStatusScreen(0);
    }
    GbSave_RequirePakPresent(sTargetPikachu.controllerPort);
}

/**
 * Plays Pikachu's cry, shows "PIKACHU learned SURF!", clears the slots after the known moves,
 * puts Surf (move 57) with its base PP into move slot arg0, saves to the cartridge and runs
 * the success ending.
 */
void PikachuReward_TeachSurfToPikachu(s32 move_slot) {
    s32 i;

    Cry_Play(0x19, 0);
    PikachuReward_ScrollTextIn(Text_GetString(NULL, 0, sPikachuSurfText, 0xE));
    PikachuReward_ShowTextForFrames(Text_GetString(NULL, 0, sPikachuSurfText, 0xE), 1);

    for (i = PikachuReward_CountKnownMoves(); i < 4; i++) {
        sTargetPikachu.pokemon.moves[i] = 0;
        sTargetPikachu.pokemon.pp[i] = 0;
    }

    sTargetPikachu.pokemon.moves[move_slot] = 0x39;
    sTargetPikachu.pokemon.pp[move_slot] = gMoveData[0x38].pp;

    PikachuReward_SaveToCartridge();
    PikachuReward_ShowTextWaitForA(Text_GetString(NULL, 0, sPikachuSurfText, 0xE), 1);
    PikachuReward_ShowSuccessEnding();
}

/**
 * Top-level flow of the reward scene: starts music track 0x49, fades in, runs the intro camera,
 * asks the player, picks a move slot and teaches Surf when accepted.
 */
void PikachuReward_RunSequence(void) {
    s32 move_slot;

    Audio_PlayMusicIfChanged(0x49);
    StageLoader_RunFrames(2);
    StageFade_Start(0x28);
    BgStage_RunUntilCondition(0xA, PikachuReward_RenderFrame);
    PikachuReward_PlayIntroCamera();
    if (PikachuReward_ShowIntroAndConfirm() != 0) {
        move_slot = PikachuReward_ResolveMoveSlot();
        if (move_slot >= 0) {
            PikachuReward_TeachSurfToPikachu(move_slot);
        }
    }
    StageLoader_RunFrames(2);
}

/**
 * Initializes one of the three follower models from sFollowerGeoNode with its D_8C2011C4
 * animation at 1.5x scale, hidden, placed at (arg1, arg2) with rotationZ arg3.
 */
void PikachuReward_SetupFollowerModel(DisplayObject* object, s16 x, s16 y, s16 rotation_z) {
    Model_InitDisplayObject(object, 0, 0, sFollowerGeoNode);
    ModelAnim_BindTransformCurve(object, 0, D_8C2011C4, 0x10000);
    object->node.flags &= ~1;
    Vec3f_SetComponentsDuplicate(&object->scale, 1.5f, 1.5f, 1.5f);
    object->position.x = x;
    object->position.y = y;
    object->rotation.z = rotation_z;
}

/**
 * Builds the scene graph (camera, red-to-orange background quad, sunburst rays) from
 * sSceneGeoLayout and the follower geo layout D_8C201070, attaches the four display objects,
 * sets up the cartridge Pikachu's model at 16x scale in idle animation 0, and places the three
 * followers at y = 380 and x = -80, 0 and 80.
 */
void PikachuReward_SetupScene(void) {
    MemoryBlock* pool_block;
    s32 i;

    pool_block = MainPool_AllocState(main_pool_get_available(), 0);
    sSceneRootNode = process_geo_layout(pool_block, sSceneGeoLayout);
    sFollowerGeoNode = process_geo_layout(pool_block, D_8C201070);

    MainPool_FinalizeAllocation(pool_block);
    ModelRenderer_InitDisplayRoots();

    for (i = 0; i < 4; i++) {
        ModelRenderer_AttachDisplayObject(&sSceneObjects[i]);
    }

    Model_InitDisplayObject(&sSceneObjects[0], 0, 0, sPikachuModel->unk_08->unk_00[0]);
    Vec3f_SetComponentsDuplicate(&sSceneObjects[0].scale, 16.0f, 16.0f, 16.0f);
    ModelAnim_SetAnimation(&sSceneObjects[0], 0);
    sSceneObjects->node.renderFlags &= ~0x40;
    PikachuReward_SetupFollowerModel(&sSceneObjects[1], -0x50, 0x17C, 0x1800);
    PikachuReward_SetupFollowerModel(&sSceneObjects[2], 0, 0x17C, 0);
    PikachuReward_SetupFollowerModel(&sSceneObjects[3], 0x50, 0x17C, -0x1800);
}

/**
 * Entry point of the Surfing Pikachu reward fragment, loaded by game_state.c when
 * transitionFlags 0x1000 is set after a Stadium cup clear (set by
 * BattlePrep_CheckPikachuNeedsSurf for the Round 2 Prime Cup when a brought Pikachu lacks
 * Surf). If that Pikachu can be found on the Game Boy cartridge, loads the assets, move name
 * table (37) and reward text table (28) and runs the scene.
 * //@src/game_state.c
 */
s32 Stadium_BonusPikachuReward(UNUSED s32 arg0, UNUSED s32 arg1) {
    UNUSED s32 pad;
    StageContext* stage_context;

    main_pool_push_state('PIKA');

    if (PikachuReward_FindPikachuOnCart() != 0) {
        Gfx_InitDisplayListBuffers(0x10000, 0);
        stage_context = StageContext_Allocate(0, 1, 3, 1, 2, 1);
        Font_Init(6, 0);

        FRAGMENT_LOAD(FRAGMENT_PARTICLE_DATA_LIBRARY);
        Fragment_Load(FRAGMENT_ID(_798CD0), _798CD0_ROM_START, _798CD0_ROM_END);
        ASSET_LOAD(D_1000000, common_menu1_ui, 0);

        Text_InitStringTables();
        sMoveNameTable = Text_GetStringTable(0x25);
        sPikachuSurfText = Text_GetStringTable(0x1C);
        PokeIcon_OpenModelArchives();
        sPikachuModel = Model_LoadForPokemon(&sTargetPikachu.pokemon);
        PikachuReward_SetupScene();
        StageContext_Activate(stage_context);
        PikachuReward_RunSequence();
        StageContext_Deactivate();
        Font_Free();
        Gfx_FreeDisplayListBuffers();
    }

    main_pool_pop_state('PIKA');
    return 0;
}
