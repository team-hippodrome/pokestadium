#include "battle_prep.h"
#include "src/geo_render.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/ui_graphics.h"
#include "src/pokemon_stats.h"
#include "src/save_data.h"
#include "src/game_state.h"
#include "src/session.h"
#include "src/text_system.h"
#include "src/audio_sfx.h"
#include "src/audio_commands_category2.h"
#include "src/gfx_buffer.h"
#include "src/gfx_rect.h"
#include "src/DDC0.h"
#include "src/matrix.h"
#include "src/memory.h"
#include "src/stage_loader.h"

s32 BattlePrep_StarfieldGeoCallback(s32, unk_D_86002F34_alt8*);

StarfieldStar sStarfield[400];
// Absorbs the one-past-the-end StarfieldStar.unk_00 write in BattlePrep_InitStarfield.
s32 pad_D_84B1A590[2];
BattlePrepState sBattlePrepState;
BattlePrepBanner sBanner;
OpponentLadder sOpponentLadder;
BattlePrepRoster sRoster;
BattlePrepTeamIcons sTeamIcons;
BattlePrepBadgeAward sBadgeAward;
ResultBanner sResultBanner;
ResultBanner sResultBanner2;
BattlePrepResultUi sResultUi;
ModeSettings sModeSettings; // Save_GetModeSettings copy for the current round; the clear-progress functions write it back
char** sBattlePrepText;

// 48x16 RGBA16 player tags indexed by controller port: "1P", "2P", "3P", "4P" (decoded from common_menu0_ui).
// The "COM" tag that follows them (D_3016900) is drawn directly.
static u8* sPlayerTagTextures[] = {
    D_3015100,
    D_3015700,
    D_3015D00,
    D_3016300,
};
// Geo layout of the whole battle_prep scene: a perspective camera (near/far set in BattlePrep_InitScene), a
// callback node that draws sStarfield with BattlePrep_StarfieldGeoCallback, then the display object layers.
static u32 sSceneGeoLayout[] = {
    0x0C000000,  0x05000000,  0x0B00002D, 0x00000000,  0x014000F0,
    0x00000000,  0xFEDF0000,  0x00000121, 0x05000000,  0x0D000000,
    0x05000000,  0x0F000000,  0x05000000, 0x20050000,  0x00000000,
    0x00000000,  0x00000000,  0x00000000, 0x08000000,  BattlePrep_StarfieldGeoCallback,
    sStarfield,  0x06000000,  0x0F000002, 0x05000000,  0x0A000000,
    &D_800AC840, 0x06000000,  0x06000000, 0x0D000001,  0x05000000,
    0x0F000002,  0x05000000,  0x0A000000, &D_800AC840, 0x06000000,
    0x06000000,  0x0D000002,  0x05000000, 0x0F000000,  0x05000000,
    0x0A000000,  &D_800AC840, 0x06000000, 0x06000000,  0x0D000003,
    0x05000000,  0x14000000,  0x002B0012, 0xFFFFFF32,  0x16FFFFFF,
    0x0F000003,  0x05000000,  0x0A000000, &D_800AC840, 0x06000000,
    0x06000000,  0x06000000,  0x06000000, 0x01000000,
};

/**
 * Records each Pokémon on the player's roster with VictoryPalace_SaveSpeciesRecord (Hall of Fame),
 * then commits save record 0x13. For modes 3/6 only when modeSelection is 3, for mode 7 only when 9.
 */
void BattlePrep_SaveVictoryPalaceRecords(void) {
    s16 remaining;
    ExtendedRosterInfo* roster_info;
    BattleMon* battle_mon;

    roster_info = gSessionContext.battleSides[0].tagTeamMember[0]->rosterInfo;
    remaining = roster_info->numberOfPokemonCanBePicked;
    battle_mon = roster_info->battleMon;

    if ((gSessionContext.mode == 3) || (gSessionContext.mode == 6)) {
        if (gSessionContext.modeSelection != 3) {
            return;
        }
    } else if ((gSessionContext.mode == 7) && (gSessionContext.modeSelection != 9)) {
        return;
    }

    while (remaining-- > 0) {
        VictoryPalace_SaveSpeciesRecord(battle_mon++, gSessionContext.mode, gSessionContext.inRound2);
    }

    Save_CommitTypedRecord(0x13, 0);
}

/**
 * Advances the GB Tower unlock tier on the first clear of mode 3 or 6 at modeSelection 3 (the Master Ball
 * division; clearedModes bit 0x8 / 0x40 not yet set): raises GameOptions.gbTowerUnlockTier (max 2) and sets
 * transitionFlags 0x8 (tier 1) or 0x10 (tier 2) so the next screen can announce the unlock.
 */
void BattlePrep_AdvanceGbTowerUnlock(void) {
    GameOptions options;
    s32 is_first_clear = 0;

    Save_GetOptions(&options);

    if ((gSessionContext.mode == 3) && (gSessionContext.modeSelection == 3) && !(sModeSettings.clearedModes & 8)) {
        is_first_clear = 1;
    }

    if ((gSessionContext.mode == 6) && (gSessionContext.modeSelection == 3) && !(sModeSettings.clearedModes & 0x40)) {
        is_first_clear = 1;
    }

    if ((is_first_clear != 0) && (options.gbTowerUnlockTier < 2)) {
        options.gbTowerUnlockTier++;
        Save_SetOptions(&options);
        if (options.gbTowerUnlockTier == 1) {
            gSessionContext.transitionFlags |= 8;
        } else {
            gSessionContext.transitionFlags |= 0x10;
        }
    }
}

/**
 * Marks the current mode as cleared: sets clearedModes bit (1 << mode) unless the player is in an earlier
 * selection of mode 3/6/7 (below the last division or the rival). Mewtwo (mode 8) sets bit 0x100. When
 * clearedModes reaches exactly 0xF8, unlocks Mewtwo and sets SESSION_FLAG_REVEAL_MEWTWO. Saves sModeSettings.
 */
void BattlePrep_MarkModeCleared(void) {
    if (((gSessionContext.mode != 3) || (gSessionContext.modeSelection >= 3)) &&
        ((gSessionContext.mode != 6) || (gSessionContext.modeSelection >= 3)) &&
        ((gSessionContext.mode != 7) || (gSessionContext.modeSelection >= 9))) {
        sModeSettings.clearedModes |= (1 << gSessionContext.mode);
        if (sModeSettings.clearedModes == 0xF8) {
            sModeSettings.mewtwoUnlocked = 1;
            gSessionContext.sessionFlags |= SESSION_FLAG_REVEAL_MEWTWO;
        }
        Save_SetModeSettings(&sModeSettings, gSessionContext.inRound2);
    }
}

/**
 * Records progress through a multi-selection mode: raises castleClears (mode 7), nintendoCupClears
 * (mode 3) or primeCupClears (mode 6) to modeSelection + 1 when the current selection is the furthest
 * cleared so far, setting transitionFlags 0x400. Saves sModeSettings.
 */
void BattlePrep_UpdateClearProgress(void) {
    if (gSessionContext.mode == 7) {
        if ((gSessionContext.modeSelection >= sModeSettings.castleClears) && (gSessionContext.modeSelection < 8)) {
            sModeSettings.castleClears = gSessionContext.modeSelection + 1;
            gSessionContext.transitionFlags |= 0x400;
        }
    } else if (gSessionContext.mode == 3) {
        if ((gSessionContext.modeSelection >= sModeSettings.nintendoCupClears) && (gSessionContext.modeSelection < 4)) {
            sModeSettings.nintendoCupClears = gSessionContext.modeSelection + 1;
            gSessionContext.transitionFlags |= 0x400;
        }
    } else if ((gSessionContext.mode == 6) && (gSessionContext.modeSelection >= sModeSettings.primeCupClears) &&
               (gSessionContext.modeSelection < 4)) {
        sModeSettings.primeCupClears = gSessionContext.modeSelection + 1;
        gSessionContext.transitionFlags |= 0x400;
    }
    Save_SetModeSettings(&sModeSettings, gSessionContext.inRound2);
}

/**
 * Returns string file_number from the battle_prep text table (table 0x1B).
 */
char* BattlePrep_GetString(s32 file_number) {
    return Text_GetString(NULL, 0, sBattlePrepText, file_number);
}

/**
 * Formats string string_id from the battle_prep text table into buffer (buffer_size bytes).
 */
char* BattlePrep_FormatString(char* buffer, s32 buffer_size, u32 string_id) {
    return Text_GetString(buffer, buffer_size, sBattlePrepText, string_id);
}

/**
 * Geo layout callback in sSceneGeoLayout: on event 5 (draw) draws the starfield stored in the node's
 * callbackArg (sStarfield).
 */
s32 BattlePrep_StarfieldGeoCallback(s32 event, unk_D_86002F34_alt8* geo_node) {
    if (event == 5) {
        BattlePrep_DrawStarfield(geo_node->node.callbackArg);
        GeoRender_ApplyMaterialState();
    }
}

/**
 * Clears a display object and resets its position to 0, scale to 1 and rotation to 0.
 */
void BattlePrep_ResetDisplayObject(DisplayObject* display_object) {
    ModelRenderer_ClearDisplayObject(display_object);
    Vec3f_SetComponentsDuplicate(&display_object->position, 0.0f, 0.0f, 0.0f);
    Vec3f_SetComponentsDuplicate(&display_object->scale, 1.0f, 1.0f, 1.0f);
    Vec3s_SetComponents(&display_object->rotation, 0, 0, 0);
}

/**
 * Resets all 128 display objects in sBattlePrepState.displayObjects.
 */
void BattlePrep_ResetAllDisplayObjects(void) {
    s32 i;

    for (i = 0; i < 128; i++) {
        BattlePrep_ResetDisplayObject(&sBattlePrepState.displayObjects[i]);
    }
}

/**
 * Prints text in font 2 centered on x = 32 (the middle of a 64-px-wide trainer portrait card) at row y.
 * side is unused.
 */
void BattlePrep_DrawPortraitCaption(s16 side, s16 y, char* text) {
    s32 text_width = Font_MeasureTextExtent(2, 0, text);

    Font_BeginTranslucentTextRendering();
    Font_SetLineHeight(0xC);
    Font_SetActive(2, 0);
    Font_Printf(0x20 - (text_width / 2), y, text);
    Font_EndTexturedTextRendering();
}

/**
 * Draws a rounded rectangle at (x, y) of size width x height tinted (red, green, blue, alpha): four
 * corner textures up to 8 px each, with fills for the edges and center.
 */
void BattlePrep_DrawBorderedRect(s16 x, s16 y, s16 width, s16 height, u8 red, u8 green, u8 blue, u8 alpha) {
    s16 inner_width;
    s16 inner_height;
    s16 corner_height;
    s16 corner_width;

    if (width >= 0x10) {
        corner_width = 8;
    } else {
        corner_width = width / 2;
    }
    if (height >= 0x10) {
        corner_height = 8;
    } else {
        corner_height = height / 2;
    }
    inner_width = width - (corner_width * 2);
    inner_height = height - (corner_height * 2);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetEnvColor(gDisplayListHead++, red, green, blue, alpha);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_3015080, G_IM_FMT_I, 16, 8, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(x, y, corner_width, corner_height, 0, 0, 0x2000 / corner_width, 0x2000 / corner_height, 0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_30150C0, G_IM_FMT_I, 16, 8, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped((x + width) - corner_width, y, corner_width, corner_height, 0, 0, 0x2000 / corner_width, 0x2000 / corner_height,
                                0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_3015000, G_IM_FMT_I, 16, 8, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(x, (y + height) - corner_height, corner_width, corner_height, 0, 0, 0x2000 / corner_width, 0x2000 / corner_height,
                                0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_3015040, G_IM_FMT_I, 16, 8, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped((x + width) - corner_width, (y + height) - corner_height, corner_width, corner_height, 0, 0, 0x2000 / corner_width,
                                0x2000 / corner_height, 0);

    gSPDisplayList(gDisplayListHead++, gUiTranslucentFillDL);

    if ((inner_width > 0) && (inner_height > 0)) {
        Gfx_FillRectRgba(x + corner_width, y, inner_width, corner_height, red, green, blue, alpha);
        Gfx_FillRectRgba(x, y + corner_width, width, inner_height, red, green, blue, alpha);
        Gfx_FillRectRgba(x + corner_width, y + corner_width + inner_height, inner_width, corner_height, red, green, blue, alpha);
    } else if (inner_width > 0) {
        Gfx_FillRectRgba(x + corner_width, y, inner_width, height, red, green, blue, alpha);
    } else if (inner_height > 0) {
        Gfx_FillRectRgba(x, y + corner_height, width, inner_height, red, green, blue, alpha);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

/**
 * Renders battle side `side`'s 64x96 trainer portrait card into image: the card frame, the trainer
 * portrait, then tags from sPlayerTagTextures. In modes 0 and 9 it draws the "COM" tag for a COM side or
 * one "1P"-"4P" tag per tag-team member in port order. Otherwise side 0 (and both sides in mode 10) get
 * a port tag plus the trainer name, and the opponent side gets only the opponent's name.
 * The roster and the team icon panels both use these images.
 */
void BattlePrep_RenderTrainerPortrait(DepthBuffer* image, s16 side, u8* portrait, u8* frame) {
    BattleSessionTeams* battle_side = &gSessionContext.battleSides[side];

    GfxImage_SetRenderTarget(&gDisplayListHead, image);

    gSPDisplayList(gDisplayListHead++, D_8006F4E0);

    Gfx_DrawTextureRgba16(0, 0, 0x40, 0x20, frame, 0x40, 0x200000);
    Gfx_DrawTextureRgba16(0, 0x20, 0x40, 0x20, frame + 0x1000, 0x40, 0x200000);
    Gfx_DrawTextureRgba16(0, 0x40, 0x40, 0x20, frame + 0x2000, 0x40, 0x200000);
    Gfx_DrawTextureRgba16(4, 0x24, 0x38, 0x1C, portrait + 0x208, 0x40, 0x200000);
    Gfx_DrawTextureRgba16(4, 0x40, 0x38, 0x1C, portrait + 0x1008, 0x40, 0x200000);

    if (gSessionContext.mode == 0 || gSessionContext.mode == 9) {
        if (battle_side->tagTeamMember[0]->isHuman & 2) {
            Gfx_DrawTextureRgba16(8, 4, 0x30, 0x10, &D_3016900, 0x30, 0x200000);
        } else if (battle_side->tagTeamSize == 1) {
            Gfx_DrawTextureRgba16(8, 4, 0x30, 0x10, sPlayerTagTextures[battle_side->tagTeamMember[0]->controllerPort], 0x30, 0x200000);
        } else {
            if (battle_side->tagTeamMember[0]->controllerPort < battle_side->tagTeamMember[1]->controllerPort) {
                Gfx_DrawTextureRgba16(8, 4, 0x30, 0x10, sPlayerTagTextures[battle_side->tagTeamMember[0]->controllerPort], 0x30,
                                      0x200000);
                Gfx_DrawTextureRgba16(8, 0x14, 0x30, 0x10, sPlayerTagTextures[battle_side->tagTeamMember[1]->controllerPort], 0x30,
                                      0x200000);
            } else {
                Gfx_DrawTextureRgba16(8, 4, 0x30, 0x10, sPlayerTagTextures[battle_side->tagTeamMember[1]->controllerPort], 0x30,
                                      0x200000);
                Gfx_DrawTextureRgba16(8, 0x14, 0x30, 0x10, sPlayerTagTextures[battle_side->tagTeamMember[0]->controllerPort], 0x30,
                                      0x200000);
            }
        }

        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
    } else if ((side == 0) || (gSessionContext.mode == 0xA)) {
        Gfx_DrawTextureRgba16(8, 4, 0x30, 0x10, sPlayerTagTextures[battle_side->tagTeamMember[0]->controllerPort], 0x30, 0x200000);

        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

        BattlePrep_DrawPortraitCaption(side, 0x14, battle_side->tagTeamMember[0]->rosterInfo->unk_014);
    } else {
        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

        BattlePrep_DrawPortraitCaption(side, 6, battle_side->tagTeamMember[0]->rosterInfo->unk_220);
    }
}

/**
 * One frame (frame < 16) of the team panels flying apart at the end of the result screen. Display
 * objects 0-15 are the team icon panels: 0-3 and 8-11 (top row) rise, 4-7 and 12-15 (bottom row) fall,
 * all spin about x and z and drift sideways, speeding up each frame.
 */
void BattlePrep_ScatterTeamPanelsStep(s16 frame) {
    static s16 sTeamPanelRotationZSpeeds[] = {
        0x0300, 0x0150, 0xFEB0, 0xFD00, 0xFD00, 0xFEB0, 0x0150, 0x0300,
    };
    static f32 sTeamPanelDriftX[] = {
        -3.0f, -2.0f, 2.0f, 3.0f, -3.0f, -2.0f, 2.0f, 3.0f,
    };

    s32 i;

    if (frame < 0x10) {
        for (i = 0; i < 4; i++) {
            sBattlePrepState.displayObjects[i + 0].position.y += 2.0f * frame * 0.5f;
            sBattlePrepState.displayObjects[i + 0].rotation.x += 0x300;
            sBattlePrepState.displayObjects[i + 8].position.y += 2.0f * frame * 0.5f;
            sBattlePrepState.displayObjects[i + 8].rotation.x += 0x300;
            sBattlePrepState.displayObjects[i + 4].rotation.x -= 0x300;
            // clang-format off
            sBattlePrepState.displayObjects[i + 4].position.y -= 2.0f * frame * 0.5f; sBattlePrepState.displayObjects[i + 12].rotation.x -= 0x300;
            // clang-format on
            sBattlePrepState.displayObjects[i + 12].position.y -= 2.0f * frame * 0.5f;
        }

        for (i = 0; i < 8; i++) {
            sBattlePrepState.displayObjects[i + 0].rotation.z += sTeamPanelRotationZSpeeds[i];
            sBattlePrepState.displayObjects[i + 8].rotation.z += sTeamPanelRotationZSpeeds[i];

            sBattlePrepState.displayObjects[i + 0].position.x += frame * sTeamPanelDriftX[i] * 0.5f;
            sBattlePrepState.displayObjects[i + 8].position.x += frame * sTeamPanelDriftX[i] * 0.5f;
        }
    }
}

/**
 * One frame (frame < 16) of fading out the team icon panels (display objects 0-15) and the WIN/LOSE
 * banners (28-31): lowers their lodFraction blend factor from 0xF0 toward 0.
 */
void BattlePrep_FadeOutResultPanelsStep(s16 frame) {
    s32 i;

    if (frame < 0x10) {
        for (i = 0; i < 16; i++) {
            sBattlePrepState.displayObjects[i].lodFraction = (-frame * 0x10) + 0xF0;
        }

        for (i = 28; i < 32; i++) {
            sBattlePrepState.displayObjects[i].lodFraction = (-frame * 0x10) + 0xF0;
        }
    }
}

/**
 * Draws one frame: background stage, geo root (starfield and display objects) and both rule windows.
 */
void BattlePrep_RenderFrame(void) {
    BgStage_DrawFrame();
    GfxImage_FillCurrent(&gDisplayListHead, 1);
    Geo_RenderRootNode(sBattlePrepState.rootNode);
    BattlePrep_DrawRuleWindow(&sResultUi.mainWindow);
    BattlePrep_DrawRuleWindow(&sResultUi.menuWindow);
    BgStage_AdvanceFrame();
}

/**
 * Advances one frame of the phase 0 versus intro.
 */
void BattlePrep_UpdateVersusIntroFrame(void) {
    Controller_PollInputs();
    BattlePrep_UpdateStarfield(sStarfield);
    BattlePrepRoster_Update(&sRoster);
    BattlePrep_RenderFrame();
}

/**
 * Phase 0: pre-battle VS roster for one-off battles (Free Battle, Event, Battle Now, Mewtwo).
 * Plays music 0x31.
 */
s32 BattlePrep_ShowVersusIntro(void) {
    s32 i;
    s32 roster_frames = 0;

    Team_LoadOpponentPreset();
    BattlePrepRoster_Init(&sRoster, sBattlePrepState.rootNode->children);
    BattlePrepRoster_Load(&sRoster, sBattlePrepState.portraitArchive);
    Audio_PlayMusicIfChanged(0x31);

    for (i = 0; i < 16; i++) {
        BattlePrep_UpdateVersusIntroFrame();
    }

    StageFade_StartFromOpaque(8);

    while (BattlePrepRoster_InitDisplayObjects(&sRoster, sBattlePrepState.displayObjects) == 0) {
        BattlePrep_UpdateVersusIntroFrame();
        roster_frames++;
    }

    while (roster_frames < 0x1E) {
        BattlePrep_UpdateVersusIntroFrame();
        roster_frames++;
    }

    for (i = 0; i < 90; i++) {
        BattlePrep_UpdateVersusIntroFrame();
    }

    Audio_StopMusic(0x20);
    StageContext_SetClearColor(1);
    StageFade_StartFromTransparent(0x10);

    for (i = 0; i < 18; i++) {
        BattlePrep_UpdateVersusIntroFrame();
    }

    if (gSessionContext.sessionFlags & 1) {
        gSessionContext.sessionFlags &= ~1;
        Save_ResetAndCommitTypedRecord(0x15, 0);
        Save_FlushBank(2);
    }

    return 4;
}

/**
 * Advances one frame of the phase 1 round intro (starfield, banner carousel, opponent ladder, roster).
 */
void BattlePrep_UpdateRoundIntroFrame(void) {
    Controller_PollInputs();
    BattlePrep_UpdateStarfield(sStarfield);
    BattlePrepBanner_Update(&sBanner);
    OpponentLadder_Update(&sOpponentLadder);
    BattlePrepRoster_Update(&sRoster);
    BattlePrep_RenderFrame();
}

/**
 * Initializes the banner, opponent ladder and roster for the phase 1 round intro.
 */
void BattlePrep_InitRoundIntroScene(void) {
    BattlePrepBanner_Init(&sBanner);
    OpponentLadder_Init(&sOpponentLadder, sBattlePrepState.trainerDataArchive, sBattlePrepState.portraitArchive);
    BattlePrepRoster_Init(&sRoster, sBattlePrepState.rootNode->children);
}

/**
 * Phase 1: cup or Gym Leader Castle round intro. Shows the battle_headers banner (cup or gym name,
 * BATTLE N / SEMI-FINAL / FINAL BATTLE label), the opponent ladder, then the roster. Plays music 0x33.
 */
s32 BattlePrep_ShowRoundIntro(void) {
    s32 i;
    s32 hold_frames;

    if (osTvType == OS_TV_PAL) {
        hold_frames = 0x4B;
    } else {
        hold_frames = 0x5A;
    }

    Team_LoadOpponentPreset();
    BattlePrep_InitRoundIntroScene();
    BattlePrepRoster_Load(&sRoster, sBattlePrepState.portraitArchive);

    for (i = 0; i < 16; i++) {
        BattlePrep_UpdateRoundIntroFrame();
    }

    StageFade_StartFromOpaque(8);

    for (i = 0; i < 20; i++) {
        BattlePrep_UpdateRoundIntroFrame();
    }

    BattlePrepBanner_StartRoundIntro(&sBanner, &sBattlePrepState.displayObjects[0x10]);
    OpponentLadder_Start(&sOpponentLadder, sBattlePrepState.displayObjects, 0);
    Audio_PlayMusicIfChanged(0x33);

    for (i = 0; i < hold_frames; i++) {
        BattlePrep_UpdateRoundIntroFrame();
    }

    BattlePrepBanner_StartSlideOut(&sBanner);
    sOpponentLadder.state = 2;

    for (i = 0; i < 16; i++) {
        BattlePrep_UpdateRoundIntroFrame();
    }

    while (BattlePrepRoster_InitDisplayObjects(&sRoster, sBattlePrepState.displayObjects) == 0) {
        BattlePrep_UpdateRoundIntroFrame();
    }

    for (i = 0; i < hold_frames; i++) {
        BattlePrep_UpdateRoundIntroFrame();
    }

    Audio_StopMusic(0x20);
    StageContext_SetClearColor(1);
    StageFade_StartFromTransparent(0x10);

    for (i = 0; i < 18; i++) {
        BattlePrep_UpdateRoundIntroFrame();
    }

    if (gSessionContext.sessionFlags & 1) {
        gSessionContext.sessionFlags &= ~1;
        Save_ResetAndCommitTypedRecord(0x15, 0);
        Save_FlushBank(2);
    }

    return 4;
}

/**
 * In round 2 mode 6 at modeSelection 3 (session flag 0x2 clear), if every roster Pokémon passes the
 * unk_52 limits and a brought Pikachu (species 25) lacks Surf (move 57), sets transitionFlags 0x1000.
 */
void BattlePrep_CheckPikachuNeedsSurf(void) {
    s32 i;
    s32 j;
    TeamRoster* trainer_roster;
    ExtendedRosterInfo* roster_info;

    if (!(gSessionContext.sessionFlags & 2) && (gSessionContext.inRound2 == 1) && (gSessionContext.mode == 6) &&
        (gSessionContext.modeSelection == 3)) {
        trainer_roster = gSessionContext.battleSides[0].tagTeamMember[0];
        roster_info = trainer_roster->rosterInfo;

        for (i = 0; i < roster_info->numberOfPokemonCanBePicked; i++) {
            s32 unk_52_high = (roster_info->battleMon[i].unk_52 & 0x70) >> 4;
            s32 unk_52_low = roster_info->battleMon[i].unk_52 & 0xF;

            if ((unk_52_high >= 4) || (unk_52_low >= 13)) {
                return;
            }
        }

        for (i = 0; i < trainer_roster->numberOfPokemonBrought; i++) {
            BattleMon* battle_mon = &trainer_roster->pokemonPicked[i];

            if (battle_mon->species.id == 0x19) {
                for (j = 0; j < 4; j++) {
                    if (battle_mon->moves[j] == 0) {
                        break;
                    } else if (battle_mon->moves[j] == 0x39) {
                        return;
                    }
                }
                gSessionContext.transitionFlags |= 0x1000;
                break;
            }
        }
    }
}

/**
 * Advances one frame of the result scene objects and rule windows, then renders.
 */
void BattlePrep_UpdateResultScene(void) {
    Controller_PollInputs();
    BattlePrep_UpdateStarfield(sStarfield);
    BattlePrepTeamIcons_Update(&sTeamIcons);
    OpponentLadder_Update(&sOpponentLadder);
    BattlePrepBadgeAward_Update(&sBadgeAward);
    BattlePrepResultBanner_Update(&sResultBanner);
    BattlePrepResultBanner_Update(&sResultBanner2);
    BattlePrepResultLabel_Update(&sResultUi.perfectLabel);
    BattlePrepResultLabel_Update(&sResultUi.extraContinueLabel);
    BattlePrep_UpdateRuleWindow(&sResultUi.mainWindow);
    BattlePrep_UpdateRuleWindow(&sResultUi.menuWindow);
    BattlePrep_RenderFrame();
}

/**
 * Initializes the result banner, team icons, badge grid and result labels for phase 2.
 */
void BattlePrep_InitResultScene(void) {
    s32 player_lost;

    BattlePrepTeamIcons_Init(&sTeamIcons);
    OpponentLadder_Init(&sOpponentLadder, sBattlePrepState.trainerDataArchive, sBattlePrepState.portraitArchive);
    BattlePrepBadgeAward_Init(&sBadgeAward);

    if (sBattlePrepState.isDraw != 0) {
        BattlePrepResultBanner_Init(&sResultBanner, 1, -1);
        BattlePrepResultBanner_Init(&sResultBanner2, 0, -1);
    } else {
        BattlePrepResultBanner_Init(&sResultBanner, 1, sBattlePrepState.playerWon);
        player_lost = sBattlePrepState.playerWon == 0;
        BattlePrepResultBanner_Init(&sResultBanner2, 0, player_lost);
    }

    BattlePrepResultLabel_Init(&sResultUi.perfectLabel, 0);
    BattlePrepResultLabel_Init(&sResultUi.extraContinueLabel, 1);
}

/**
 * Reveals both teams and the WIN/LOSE/DRAW banners after a battle. Plays music 0x32, or 0x45
 * when the player loses to a COM roster.
 */
void BattlePrep_RunResultReveal(void) {
    s32 i;

    BattlePrepTeamIcons_Load(&sTeamIcons, sBattlePrepState.portraitArchive);

    for (i = 0; i < 8; i++) {
        BattlePrep_UpdateResultScene();
    }

    if (gSessionContext.battleSides[1].tagTeamMember[0]->isHuman & 2) {
        if (gSessionContext.battleSides[0].wonBattle == 1) {
            Audio_PlayMusicIfChanged(0x32);
        } else {
            Audio_PlayMusicIfChanged(0x45);
        }
    } else {
        Audio_PlayMusicIfChanged(0x32);
    }

    StageFade_StartFromOpaque(8);

    while (BattlePrepTeamIcons_InitDisplayObjects(&sTeamIcons, sBattlePrepState.displayObjects) == 0) {
        BattlePrep_UpdateResultScene();
    }

    while (sTeamIcons.state != 0) {
        BattlePrep_UpdateResultScene();
    }

    BattlePrepResultBanner_Launch(&sResultBanner, &sBattlePrepState.displayObjects[0x1C], &sBattlePrepState.displayObjects[0x20]);
    BattlePrepResultBanner_Launch(&sResultBanner2, &sBattlePrepState.displayObjects[0x1E], &sBattlePrepState.displayObjects[0x20]);

    while ((sOpponentLadder.state != 0) || (sResultBanner2.state != 0)) {
        BattlePrep_UpdateResultScene();
    }
}

/**
 * Result reveal for a perfect cup win: also grows the PERFECT label (D_3028870) and, before the
 * final, the EXTRA CONTINUE label (D_30281B0).
 */
void BattlePrep_RunPerfectResultReveal(void) {
    s32 i;

    BattlePrepTeamIcons_Load(&sTeamIcons, sBattlePrepState.portraitArchive);

    for (i = 0; i < 8; i++) {
        BattlePrep_UpdateResultScene();
    }

    Audio_PlayMusicIfChanged(0x32);
    StageFade_StartFromOpaque(8);

    while (BattlePrepTeamIcons_InitDisplayObjects(&sTeamIcons, sBattlePrepState.displayObjects) == 0) {
        BattlePrep_UpdateResultScene();
    }

    for (i = 0; i < 50; i++) {
        BattlePrep_UpdateResultScene();
    }

    BattlePrepResultLabel_StartGrow(&sResultUi.perfectLabel, &sBattlePrepState.displayObjects[0x1A]);
    Audio_PlayCategory11SoundCommand(0x01100013, 0, 0);

    while (sTeamIcons.state != 0) {
        BattlePrep_UpdateResultScene();
    }

    BattlePrepResultLabel_StartShrink(&sResultUi.perfectLabel);
    BattlePrepResultBanner_Launch(&sResultBanner, &sBattlePrepState.displayObjects[0x1C], &sBattlePrepState.displayObjects[0x20]);
    BattlePrepResultBanner_Launch(&sResultBanner2, &sBattlePrepState.displayObjects[0x1E], &sBattlePrepState.displayObjects[0x20]);

    if (gSessionContext.roundOrDemoId < 8) {
        for (i = 0; i < 16; i++) {
            BattlePrep_UpdateResultScene();
        }
        BattlePrepResultLabel_StartGrow(&sResultUi.extraContinueLabel, &sBattlePrepState.displayObjects[0x1B]);
        Audio_PlaySoundEffectById(SFX_TROPHY_APPEAR);
    }

    while ((sOpponentLadder.state != 0) || (sResultBanner2.state != 0)) {
        BattlePrep_UpdateResultScene();
    }
}

/**
 * Shows the opponent ladder after a cup or castle battle and, on a win, stamps the defeated
 * opponent's portrait. Stamps follow the round index (roundOrDemoId), not the opponents actually
 * fought: skipping rounds leaves stamped placeholder portraits in the skipped slots.
 */
void BattlePrep_RunOpponentLadderResult(void) {
    s32 i;
    s32 player_lost;

    BattlePrepResultBanner_StartExit(&sResultBanner2);
    player_lost = sBattlePrepState.playerWon == 0;
    OpponentLadder_Start(&sOpponentLadder, sBattlePrepState.displayObjects, player_lost + 1);

    while ((sOpponentLadder.state != 0) || (sResultBanner2.state != 0)) {
        BattlePrep_UpdateResultScene();
    }

    for (i = 0; i < 16; i++) {
        BattlePrep_UpdateResultScene();
    }
}

/**
 * Ends the result screen: waits for both result banners to finish leaving, removes every display object
 * above the team panels (16+), plays sound 0x01100017, scatters the team panels for 16 frames
 * (BattlePrep_ScatterTeamPanelsStep), removes them and shows 16 frames of bare starfield.
 */
void BattlePrep_ScatterTeamPanelsExit(void) {
    s32 i;
    s32 j;

    while ((sResultBanner.state != 0) || (sResultBanner2.state != 0)) {
        BattlePrep_UpdateResultScene();
    }

    for (i = 16; i < ARRAY_COUNT(sBattlePrepState.displayObjects); i++) {
        BattlePrep_ResetDisplayObject(&sBattlePrepState.displayObjects[i]);
    }

    Audio_PlayCategory11SoundCommand(0x01100017, 0, 0);

    for (j = 0; j < 16; j++) {
        Controller_PollInputs();
        BattlePrep_ScatterTeamPanelsStep(j);
        BattlePrep_UpdateStarfield(sStarfield);
        BattlePrep_RenderFrame();
    }

    for (i = 0; i < 16; i++) {
        BattlePrep_ResetDisplayObject(&sBattlePrepState.displayObjects[i]);
    }

    for (j = 0; j < 16; j++) {
        Controller_PollInputs();
        BattlePrep_UpdateStarfield(sStarfield);
        BattlePrep_RenderFrame();
    }
}

/**
 * Stops music and fades out the team panels and WIN/LOSE banners over 16 frames
 * (BattlePrep_FadeOutResultPanelsStep). Used when a cup loss spends a continue to retry the battle.
 */
void BattlePrep_FadeOutResultPanels(void) {
    s32 i;

    Audio_StopMusic(0x3C);

    for (i = 0; i < 16; i++) {
        Controller_PollInputs();
        BattlePrep_FadeOutResultPanelsStep(i);
        BattlePrep_UpdateStarfield(sStarfield);
        BattlePrep_RenderFrame();
    }
}

/**
 * Fades the whole screen out: stops music, starts a 16-frame stage fade from transparent to the clear
 * color, and runs 34 starfield frames while it completes.
 */
void BattlePrep_FadeOutScene(void) {
    s32 i;

    Audio_StopMusic(0x3C);
    StageContext_SetClearColor(1);
    StageFade_StartFromTransparent(0x10);

    for (i = 0; i < 18; i++) {
        Controller_PollInputs();
        BattlePrep_UpdateStarfield(sStarfield);
        BattlePrep_RenderFrame();
    }

    for (i = 0; i < 16; i++) {
        Controller_PollInputs();
        BattlePrep_UpdateStarfield(sStarfield);
        BattlePrep_RenderFrame();
    }
}

/**
 * "Keep battling?" Continue / Save and Quit prompt after a cup or castle win. Save and Quit
 * advances roundOrDemoId and writes continue data (after the overwrite prompt if a save exists).
 */
s32 BattlePrep_PromptKeepBattling(void) {
    s32 choice;
    s32 overwrite_choice;

    if (gSessionContext.mode == 7) {
        BattlePrep_InitRuleWindow(&sResultUi.mainWindow, 2);
    } else {
        BattlePrep_InitRuleWindow(&sResultUi.mainWindow, 1);
    }

    choice = BattlePrep_PollRuleWindow(&sResultUi.mainWindow);
    while (choice == -1) {
        BattlePrep_UpdateResultScene();
        choice = BattlePrep_PollRuleWindow(&sResultUi.mainWindow);
    }

    if (choice == 1) {
        if (Save_ContinueRecordExists() != 0) {
            BattlePrep_InitRuleWindow(&sResultUi.mainWindow, 0xA);

            overwrite_choice = BattlePrep_PollRuleWindow(&sResultUi.mainWindow);
            while (overwrite_choice == -1) {
                BattlePrep_UpdateResultScene();
                overwrite_choice = BattlePrep_PollRuleWindow(&sResultUi.mainWindow);
            }

            if (overwrite_choice == 1) {
                choice = 0;
            }
        }
        if (choice == 1) {
            gSessionContext.roundOrDemoId = gSessionContext.roundOrDemoId + 1;
            Session_SaveContinueData();
            Save_FlushBank(2);
        }
    }
    return choice;
}

/**
 * Cup loss menu Save option: confirms overwriting existing continue data, then spends a
 * continue and writes the continue data. Returns 0 if the player declines the overwrite.
 * Only BattlePrep_ShowCupResult calls this. Its Continue option spends a continue
 * directly, without saving. Continues are only spent after a cup loss.
 */
s32 BattlePrep_SpendContinueAndSave(void) {
    s32 saved = 1;
    s32 overwrite_choice;

    if (Save_ContinueRecordExists() != 0) {
        BattlePrep_InitRuleWindow(&sResultUi.mainWindow, 0xA);

        overwrite_choice = BattlePrep_PollRuleWindow(&sResultUi.mainWindow);
        while (overwrite_choice == -1) {
            BattlePrep_UpdateResultScene();
            overwrite_choice = BattlePrep_PollRuleWindow(&sResultUi.mainWindow);
        }

        if (overwrite_choice == 1) {
            saved = 0;
        }
    }

    if (saved != 0) {
        gSessionContext.continueCount--;
        Session_SaveContinueData();
        Save_FlushBank(2);
    }
    return saved;
}

/**
 * Free Battle (mode 0) result: adds wins to winCount, shows the win record and the
 * Try once more / Change team combination / Quit menu. Try once more (0x21) returns to
 * entry Pokemon selection; Change team combination (0x81) returns to the Free Battle rules
 * screen, where the controller ports assigned to each team can be changed; Quit (0x1)
 * returns to the area select. Once the winner's winCount reaches 99, Try once more is disabled
 * and the cursor starts on Change team combination.
 */
s32 BattlePrep_ShowFreeBattleResult(void) {
    s32 choice;

    if (gSessionContext.battleSides[0].wonBattle == 1) {
        gSessionContext.battleSides[0].winCount += 1;
    }

    if (gSessionContext.battleSides[1].wonBattle == 1) {
        gSessionContext.battleSides[1].winCount += 1;
    }

    BattlePrep_RunResultReveal();
    Audio_PlayCategory11SoundCommand(0x0110000F, 0, 0);
    BattlePrep_InitRuleWindow(&sResultUi.mainWindow, 9);
    BattlePrep_InitRuleWindow(&sResultUi.menuWindow, 5);

    choice = BattlePrep_PollRuleWindow(&sResultUi.menuWindow);
    while (choice == -1) {
        BattlePrep_UpdateResultScene();
        choice = BattlePrep_PollRuleWindow(&sResultUi.menuWindow);
    }

    switch (choice) {
        case 0:
            gSessionContext.transitionFlags |= 0x21;
            break;

        case 1:
            gSessionContext.transitionFlags |= 0x81;
            break;

        case 2:
            gSessionContext.transitionFlags |= 1;
            break;
    }

    BattlePrepResultBanner_StartExit(&sResultBanner2);
    BattlePrepResultBanner_StartExit(&sResultBanner);
    Audio_PlayCategory11SoundCommand(0x0110000D, 0, 0);
    BattlePrep_ScatterTeamPanelsExit();
    BattlePrep_FadeOutScene();

    return 4;
}

/**
 * Battle Now and Event (modes 9 and 10) result: shows the WIN/LOSE banner and waits for A.
 */
s32 BattlePrep_ShowSimpleResult(void) {
    BattlePrep_RunResultReveal();

    do {
        BattlePrep_UpdateResultScene();
    } while (!(BTN_IS_PRESSED(gPlayer1Controller, BTN_A)));

    BattlePrepResultBanner_StartExit(&sResultBanner2);
    BattlePrepResultBanner_StartExit(&sResultBanner);
    Audio_PlayCategory11SoundCommand(0x0110000D, 0, 0);
    BattlePrep_ScatterTeamPanelsExit();
    BattlePrep_FadeOutScene();

    return 4;
}

/**
 * Stadium cup (modes 1-6) result. On a win: ladder mark, badge grid, then Keep battling?
 * (or cup clear after the final, roundOrDemoId 8). On a loss: progress header plus the
 * Continue / Save / Re-enter / From the start / Quit menu. A cup final win hands off to the
 * cup clear ceremony (mode 17); observed transitionFlags 0x403 (Poke Ball) and 0x40B (Master
 * Ball, which also raises the GB Tower unlock tier).
 */
s32 BattlePrep_ShowCupResult(void) {
    UNUSED s32 pad[2];
    s32 next_phase = 4;
    s32 i;
    s32 menu_open;
    s32 choice;

    if (sBattlePrepState.perfectWin != 0) {
        BattlePrep_RunPerfectResultReveal();
    } else {
        BattlePrep_RunResultReveal();
    }

    BattlePrep_RunOpponentLadderResult();

    if (gSessionContext.battleSides[0].wonBattle == 1) {
        gSessionContext.continueCount += sBattlePrepState.perfectWin;
        if ((sBattlePrepState.perfectWin != 0) && (gSessionContext.roundOrDemoId < 8)) {
            BattlePrepResultLabel_StartShrink(&sResultUi.extraContinueLabel);
        }

        BattlePrepResultBanner_StartExit(&sResultBanner);
        BattlePrepBadgeAward_StartFlipIn(&sBadgeAward, sBattlePrepState.displayObjects);

        while (sBadgeAward.state != 3) {
            BattlePrep_UpdateResultScene();
        }

        if (gSessionContext.roundOrDemoId == 8) {
            for (i = 0; i < 120; i++) {
                BattlePrep_UpdateResultScene();
                if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
                    break;
                }
            }

            BattlePrep_SaveVictoryPalaceRecords();
            BattlePrep_AdvanceGbTowerUnlock();
            BattlePrep_MarkModeCleared();
            BattlePrep_UpdateClearProgress();
            Save_CommitTypedRecord(0x14, 0);
            Save_FlushBank(2);
            BattlePrep_ScatterTeamPanelsExit();
            BattlePrep_FadeOutScene();
            BattlePrep_CheckPikachuNeedsSurf();
            gSessionContext.transitionFlags |= 3;
        } else if (BattlePrep_PromptKeepBattling() == 1) {
            BattlePrep_ScatterTeamPanelsExit();
            BattlePrep_FadeOutScene();
            gSessionContext.transitionFlags |= 0x41;
        } else {
            BattlePrep_ScatterTeamPanelsExit();
            Audio_StopMusic(0x50);
            gSessionContext.roundOrDemoId++;
            next_phase = 1;
        }
    } else {
        menu_open = 1;
        while (menu_open != 0) {
            BattlePrep_InitRuleWindow(&sResultUi.mainWindow, 7);
            BattlePrep_InitRuleWindow(&sResultUi.menuWindow, 3);

            choice = BattlePrep_PollRuleWindow(&sResultUi.menuWindow);
            while (choice == -1) {
                BattlePrep_UpdateResultScene();
                choice = BattlePrep_PollRuleWindow(&sResultUi.menuWindow);
            }

            switch (choice) {
                case 0:
                    next_phase = 1;
                    gSessionContext.continueCount--;
                    menu_open = 0;
                    break;

                case 1:
                    if (BattlePrep_SpendContinueAndSave() != 0) {
                        menu_open = 0;
                        gSessionContext.transitionFlags |= 0x41;
                    }
                    break;

                case 2:
                    menu_open = 0;
                    gSessionContext.transitionFlags |= 0x21;
                    break;

                case 3:
                    menu_open = 0;
                    gSessionContext.transitionFlags |= 0x801;
                    break;

                case 4:
                    menu_open = 0;
                    gSessionContext.transitionFlags |= 1;
                    break;
            }
        }

        if (next_phase == 4) {
            BattlePrepResultBanner_StartExit(&sResultBanner);
            BattlePrep_ScatterTeamPanelsExit();
            BattlePrep_FadeOutScene();
        } else {
            BattlePrep_FadeOutResultPanels();
            BattlePrep_ResetAllDisplayObjects();
        }
    }

    return next_phase;
}

/**
 * Gym Leader Castle (mode 7) result. A win shows Keep battling? or, after a gym leader or the
 * rival, returns phase 3. A loss shows Re-enter / Re-start at this GYM / Quit. Observed
 * transitionFlags: 0x501 after the Elite Four clear and 0x201 after the rival clear.
 */
s32 BattlePrep_ShowCastleResult(void) {
    s32 unused;
    s32 i;
    s32 next_phase = 4;
    s32 choice;

    BattlePrep_RunResultReveal();
    BattlePrep_RunOpponentLadderResult();

    if (gSessionContext.battleSides[0].wonBattle == 1) {
        if ((gSessionContext.roundOrDemoId == 4) || (gSessionContext.modeSelection == 9)) {
            BattlePrep_MarkModeCleared();
            BattlePrep_UpdateClearProgress();
            BattlePrep_SaveVictoryPalaceRecords();
            Save_CommitTypedRecord(0x14, 0);
            Save_FlushBank(2);

            for (i = 0; i < 60; i++) {
                BattlePrep_UpdateResultScene();
                if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
                    break;
                }
            }

            BattlePrepResultBanner_StartExit(&sResultBanner);
            Audio_PlayCategory11SoundCommand(0x0110000D, 0, 0);
            BattlePrep_ScatterTeamPanelsExit();
            Audio_StopMusic(0x10);

            gSessionContext.transitionFlags |= 1;
            if (gSessionContext.modeSelection == 8) {
                gSessionContext.transitionFlags |= 0x400;
            }

            if (gSessionContext.modeSelection == 9) {
                gSessionContext.transitionFlags |= 0x200;
            } else {
                gSessionContext.transitionFlags |= 0x100;
            }
            next_phase = 3;
        } else if (BattlePrep_PromptKeepBattling() == 1) {
            BattlePrepResultBanner_StartExit(&sResultBanner);
            Audio_PlayCategory11SoundCommand(0x0110000D, 0, 0);
            BattlePrep_ScatterTeamPanelsExit();
            BattlePrep_FadeOutScene();
            gSessionContext.transitionFlags |= 0x41;
        } else {
            BattlePrepResultBanner_StartExit(&sResultBanner);
            Audio_PlayCategory11SoundCommand(0x0110000D, 0, 0);
            BattlePrep_ScatterTeamPanelsExit();
            Audio_StopMusic(0x50);
            gSessionContext.roundOrDemoId++;
            next_phase = 1;
        }
    } else {
        BattlePrep_InitRuleWindow(&sResultUi.mainWindow, 8);
        BattlePrep_InitRuleWindow(&sResultUi.menuWindow, 4);

        choice = BattlePrep_PollRuleWindow(&sResultUi.menuWindow);
        while (choice == -1) {
            BattlePrep_UpdateResultScene();
            choice = BattlePrep_PollRuleWindow(&sResultUi.menuWindow);
        }

        switch (choice) {
            case 0:
                gSessionContext.transitionFlags |= 0x21;
                break;

            case 1:
                gSessionContext.transitionFlags |= 0x801;
                break;

            case 2:
                gSessionContext.transitionFlags |= 1;
                break;
        }

        BattlePrepResultBanner_StartExit(&sResultBanner);
        BattlePrep_ScatterTeamPanelsExit();
        BattlePrep_FadeOutScene();

        if (gSessionContext.modeSelection == 9) {
            gSessionContext.modeSelection = 8;
        }
    }
    return next_phase;
}

/**
 * Mewtwo (mode 8) result. A loss offers Battle again / Quit; a win sets transitionFlags 0x5 (0x4 plus 0x1).
 */
s32 BattlePrep_ShowMewtwoResult(void) {
    s32 choice;
    u16 options_flags;

    BattlePrep_RunResultReveal();
    if (gSessionContext.battleSides[0].wonBattle == 1) {
        s32 mewtwo_beaten_before = !!(sModeSettings.clearedModes & 0x100);

        if ((gSessionContext.inRound2 == 0) & !mewtwo_beaten_before) {
            Save_GetOptionsField02(&options_flags);
            options_flags |= 4;
            Save_SetOptionsField02(&options_flags);
        }

        BattlePrep_MarkModeCleared();
        Save_CommitTypedRecord(0x14, 0);
        Save_FlushBank(2);
        BattlePrepResultBanner_StartExit(&sResultBanner2);
        BattlePrepResultBanner_StartExit(&sResultBanner);
        Audio_PlayCategory11SoundCommand(0x0110000D, 0, 0);
        BattlePrep_ScatterTeamPanelsExit();
        BattlePrep_FadeOutScene();
        gSessionContext.transitionFlags |= 5;
    } else {
        BattlePrep_InitRuleWindow(&sResultUi.menuWindow, 6);

        choice = BattlePrep_PollRuleWindow(&sResultUi.menuWindow);
        while (choice == -1) {
            BattlePrep_UpdateResultScene();
            choice = BattlePrep_PollRuleWindow(&sResultUi.menuWindow);
        }

        switch (choice) {
            case 0:
                gSessionContext.transitionFlags |= 0x21;
                break;

            case 1:
                gSessionContext.transitionFlags |= 1;
                break;
        }

        BattlePrepResultBanner_StartExit(&sResultBanner2);
        BattlePrepResultBanner_StartExit(&sResultBanner);
        Audio_PlayCategory11SoundCommand(0x0110000D, 0, 0);
        BattlePrep_ScatterTeamPanelsExit();
        BattlePrep_FadeOutScene();
    }
    return 4;
}

/**
 * Returns 1 when a cup mode (1-6) battle was won with all three picked Pokemon at nonzero HP.
 * The result grants an extra continue and shows the PERFECT label.
 */
u8 BattlePrep_IsPerfectCupWin(void) {
    s32 i;
    u8 is_perfect = 0;

    if ((gSessionContext.mode > 0) && (gSessionContext.mode < 7)) {
        if (gSessionContext.battleSides[0].wonBattle == 1) {
            is_perfect = 1;
            for (i = 0; i < 3; i++) {
                if (gSessionContext.battleSides[0].tagTeamMember[0]->pokemonPicked[i].hp == 0) {
                    is_perfect = 0;
                }
            }
        }
    }

    return is_perfect;
}

/**
 * Phase 2: post-battle result screen. Records draw/win state, then dispatches on
 * gSessionContext.mode to the per-mode result handler and returns the next phase.
 */
s32 BattlePrep_ShowBattleResult(void) {
    s32 next_phase;

    sBattlePrepState.perfectWin = BattlePrep_IsPerfectCupWin();
    BattlePrep_InitResultScene();

    switch (gSessionContext.mode) {
        case 0:
            next_phase = BattlePrep_ShowFreeBattleResult();
            break;

        case 1:
            next_phase = BattlePrep_ShowCupResult();
            break;

        case 2:
            next_phase = BattlePrep_ShowCupResult();
            break;

        case 3:
            next_phase = BattlePrep_ShowCupResult();
            break;

        case 4:
            next_phase = BattlePrep_ShowCupResult();
            break;

        case 5:
            next_phase = BattlePrep_ShowCupResult();
            break;

        case 6:
            next_phase = BattlePrep_ShowCupResult();
            break;

        case 7:
            next_phase = BattlePrep_ShowCastleResult();
            break;

        case 8:
            next_phase = BattlePrep_ShowMewtwoResult();
            break;

        case 9:
            next_phase = BattlePrep_ShowSimpleResult();
            break;

        case 10:
            next_phase = BattlePrep_ShowSimpleResult();
            break;
    }
    return next_phase;
}

/**
 * Phase 3: castle gym-clear outro, reached when BattlePrep_ShowCastleResult returns 3.
 * Plays music 0x3F.
 */
s32 BattlePrep_ShowCastleClearOutro(void) {
    s32 i;
    s32 hold_frames;

    if (gSessionContext.modeSelection == 9) {
        hold_frames = 0x3C;
    } else {
        hold_frames = 0x4B;
    }

    BattlePrepBanner_Init(&sBanner);
    BattlePrepBanner_StartClearPlaque(&sBanner, &sBattlePrepState.displayObjects[0x10]);
    Audio_PlayMusicIfChanged(0x3F);

    for (i = 0; i < hold_frames; i++) {
        Controller_PollInputs();
        BattlePrep_UpdateStarfield(sStarfield);
        BattlePrepBanner_Update(&sBanner);
        BattlePrep_RenderFrame();
    }

    StageContext_SetClearColor(1);
    StageFade_StartFromTransparent(0x10);

    for (i = 0; i < 18; i++) {
        Controller_PollInputs();
        BattlePrep_UpdateStarfield(sStarfield);
        BattlePrepBanner_Update(&sBanner);
        BattlePrep_RenderFrame();
    }

    return 4;
}

/**
 * Computes playerWon / isDraw from battleSides[].wonBattle, attaches the 128 display objects, builds
 * sSceneGeoLayout into rootNode (camera near 10, far 6000), initializes the starfield and closes both
 * rule windows.
 */
void BattlePrep_InitScene(void) {
    MemoryBlock* memory_block;
    CameraNode* camera;
    DisplayObject* display_object;
    s32 i;

    sBattlePrepState.playerWon = 0;
    sBattlePrepState.isDraw = 0;
    if (gSessionContext.battleSides[0].wonBattle == gSessionContext.battleSides[1].wonBattle) {
        sBattlePrepState.isDraw = 1;
    } else if (gSessionContext.battleSides[0].wonBattle == 1) {
        sBattlePrepState.playerWon = 1;
    }

    ModelRenderer_InitDisplayRoots();

    for (i = 0, display_object = &sBattlePrepState.displayObjects[0]; i < 128; display_object++, i++) {
        ModelRenderer_AttachDisplayObject(display_object);
        sBattlePrepState.displayObjects[i].effectSlot = i;
    }

    memory_block = MainPool_AllocState(main_pool_get_available(), 0);
    sBattlePrepState.rootNode = process_geo_layout(memory_block, &sSceneGeoLayout);
    MainPool_FinalizeAllocation(memory_block);

    camera = sBattlePrepState.rootNode->children;
    camera->perspective.nearPlane = 10.0f;
    camera->perspective.farPlane = 6000.0f;

    BattlePrep_InitStarfield(&sStarfield);
    BattlePrep_CloseRuleWindow(&sResultUi.mainWindow);
    BattlePrep_CloseRuleWindow(&sResultUi.menuWindow);
}

/**
 * Fragment entry. phase selects the first phase: 0 = versus intro, 1 = cup or castle round intro,
 * 2 = post-battle result, 3 = castle gym-clear outro. Loops while a phase returns another phase
 * (Continue after a cup or castle win returns 1 for the next round intro, Continue after a cup loss
 * returns 1 to retry the round, and a castle gym clear returns 3); 4 exits. Called from the mode states in
 * src/game_state.c around BATTLE_ENGINE.
 */
//@src/game_state.c
s32 BattlePrep_Main(s32 phase, UNUSED s32 unused_arg) {
    StageContext* stage_context;

    main_pool_push_state('EYEC');
    Save_EnsureBankLoaded(2);
    Save_GetModeSettings(&sModeSettings, gSessionContext.inRound2);
    Gfx_InitDisplayListBuffers(0x18000, 0);
    stage_context = StageContext_Allocate(0, 1, 3, 1, 2, 1);
    Font_Init(0x17, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_2000000, badge_ui, 0);
    ASSET_LOAD(D_3000000, common_menu0_ui, 0);

    Text_InitStringTables();
    sBattlePrepText = Text_GetStringTable(0x1B);
    PokeIcon_OpenModelArchive();

    sBattlePrepState.trainerDataArchive = BinArchive_Open(TRAINER_DATA_ROM_START, NULL, 1, 1);
    sBattlePrepState.portraitArchive = ASSET_LOAD2(battle_portraits, 1, 1);

    StageContext_Activate(stage_context);
    BattlePrep_InitScene();

    while (phase < 4) {
        main_pool_push_state('eye0');

        switch (phase) {
            case 0:
                phase = BattlePrep_ShowVersusIntro();
                break;

            case 1:
                phase = BattlePrep_ShowRoundIntro();
                break;

            case 2:
                phase = BattlePrep_ShowBattleResult();
                break;

            case 3:
                phase = BattlePrep_ShowCastleClearOutro();
                break;
        }

        main_pool_pop_state('eye0');
    }

    StageContext_Deactivate();
    Font_Free();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('EYEC');

    return 0;
}
