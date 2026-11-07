#include "battle_order_select.h"
#include "game_state.h"
#include "src/geo_render.h"
#include "src/poke_icon.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/ui_graphics.h"
#include "src/status_icons.h"
#include "src/pokemon_stats.h"
#include "src/text_system.h"
#include "src/jpeg_stream.h"
#include "src/audio_sfx.h"
#include "src/gfx_buffer.h"
#include "src/DDC0.h"
#include "src/controller.h"
#include "src/memory.h"
#include "src/stage_loader.h"

static TeamOrderState sTeamOrderStates[2];
static BinArchive* sBackgroundArchive;
static void* sBackgroundImage;
static char** sMoveNameTable;
static char** sTypeNameTable;
static char** sTeamSelectText;
static char** sPokemonDetailText;
static s16 sFullRedrawFrames;
static s16 sScrollingTextProgress;
static s32 sOrderSelectPanelCount;
static BattleOrderInputState sBattleOrderInputStates[4];
static SessionContext* sSessionContext;
static Controller sComController;
static s32 sComNextButtonPress;
static s32 sComSelectionTimer;
static s16 sCursorOscillationPhases[4];

static u16 sBorderPortColors[] = { 0x1D, 0x341, 0x6947, 0x6A41 }; // #000073, #006B00, #6B2918, #6B4A00
static ComSelectionButtonPresses sComButtonPressesForSinglePokemon[] = {
    { 30, 0 },
    { 30, BTN_B },
    { 1, BTN_A },
    { 0, 0 },
};
static ComSelectionButtonPresses sComButtonPressesRosterSize2[] = {
    { 30, 0 }, { 10, BTN_B }, { 30, BTN_CLEFT }, { 1, BTN_A }, { 0, 0 },
};
static ComSelectionButtonPresses sComButtonPressesRosterSize3[] = {
    { 30, 0 }, { 10, BTN_B }, { 10, BTN_CLEFT }, { 30, BTN_CUP }, { 1, BTN_A }, { 0, 0 },
};
static ComSelectionButtonPresses sComButtonPressesRosterSize4[] = {
    { 30, 0 }, { 10, BTN_B }, { 10, BTN_CLEFT }, { 10, BTN_CUP }, { 30, BTN_A }, { 1, BTN_A }, { 0, 0 },
};
static ComSelectionButtonPresses sComButtonPressesRosterSize5[] = {
    { 30, 0 },     { 10, BTN_B },     { 10, BTN_CLEFT }, { 10, BTN_CUP },
    { 10, BTN_A }, { 30, BTN_CDOWN }, { 1, BTN_A },      { 0, 0 },
};
static ComSelectionButtonPresses sComButtonPressesRosterSize6[] = {
    { 30, 0 },     { 10, BTN_B },     { 10, BTN_CLEFT },  { 10, BTN_CUP },
    { 10, BTN_A }, { 10, BTN_CDOWN }, { 30, BTN_CRIGHT }, { 1, BTN_A },
};
static ComSelectionButtonPresses* sComButtonPressesByRosterSize[] = {
    NULL,
    sComButtonPressesForSinglePokemon,
    sComButtonPressesRosterSize2,
    sComButtonPressesRosterSize3,
    sComButtonPressesRosterSize4,
    sComButtonPressesRosterSize5,
    sComButtonPressesRosterSize6,
};
static PokemonTypeStyle sPokemonTypeStyles[] = {
    { { 0xF0, 0xF0, 0xF0 }, D_2017740 }, // #F0F0F0 
    { { 0xF0, 0x96, 0x32 }, D_2019040 }, // #F09632 
    { { 0x50, 0x96, 0xF0 }, D_20199A0 }, // #5096F0
    { { 0xC8, 0x96, 0xF0 }, D_2019680 }, // #C896F0 
    { { 0xF0, 0xC8, 0x78 }, D_2018D20 }, // #F0C878 
    { { 0x64, 0xB4, 0xB4 }, D_2018A00 }, // #64B4B4
    { { 0x50, 0x96, 0xF0 }, D_20199A0 }, // #5096F0 
    { { 0xC8, 0xF0, 0x50 }, D_2019360 }, // #C8F050 
    { { 0xDC, 0xC8, 0xF0 }, D_2019FE0 }, // #DCC8F0
    { { 0xF0, 0x64, 0x64 }, D_2017A60 }, // #F06464 
    { { 0x46, 0xB4, 0xF0 }, D_2017D80 }, // #46B4F0 
    { { 0x64, 0xC8, 0x64 }, D_20183C0 }, // #64C864
    { { 0xF0, 0xF0, 0x32 }, D_20186E0 }, // #F0F032 
    { { 0xF0, 0x8C, 0xB4 }, D_2019CC0 }, // #F08CB4 
    { { 0xB4, 0xF0, 0xF0 }, D_20180A0 }, // #B4F0F0
    { { 0x64, 0xDC, 0xC8 }, D_201A300 }, // #64DCC8
};
static Color_RGB8 sHelpTextGradientStart = { 0x00, 0x0A, 0x41 }; // #000A41
static Color_RGB8 sHelpTextGradientEnd = { 0x00, 0x00, 0xD2 };   // #0000D2
// FAKE: these split pointer tables are indexed across their declared bounds.
// Controller and partner-port lookups reach subsequent declarations, and the
// packed owner/frame lookup uses column 3 of D_8480369C, ending at D_848036D8.
// Declaration order is retained for matching.
static u8* sHumanPlayerLabelIcon[] = { gPlayer1LabelIcon, gPlayer2LabelIcon, D_3009E70 };
static u8* D_84803690[] = { D_300A5C0, D_300AD10 };
static u8* D_84803698[] = { D_300B9B8 };
static u8* D_8480369C[][3] = {
    {
        D_300C660,
        D_300D308,
        D_300DFB0,
    },
    {
        D_300EC58,
        D_3005B80,
        D_3005F40,
    },
    {
        D_3006300,
        D_30066C0,
        D_3006A80,
    },
    {
        D_3006E40,
        D_3007200,
        D_30075C0,
    },
    {
        D_3007980,
        D_3007D40,
        D_3008100,
    },
};
static u8* D_848036D8 = D_30084C0;

/*
    Draws the instructions for the L and R button at the bottom of the screen
    * (L) Button to cancel
    * (R) Button to check
*/
void BattleMenu_DrawBottomActionButtons(void) {
    Gfx_DrawTiledRgba16Image(sBackgroundImage);

    gSPDisplayList(gDisplayListHead++, D_8006F4E0);

    Gfx_DrawTextureRgba16(113, 424, 32, 24, gLButtonOrderSelect, 0x20, 0x200000);
    Gfx_DrawTextureRgba16(354, 424, 32, 24, gRButtonOrderSelect, 0x20, 0x200000);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    Font_BeginTranslucentTextRendering();
    Font_SetActive(8, 0);
    Font_Printf(90, 426, "*");
    Font_Printf(329, 426, "*");
    // (L) Button to cancel
    Font_Printf(145, 424, Text_GetString(NULL, 0, sTeamSelectText, 2));
    // (R) Button to Check
    Font_Printf(384, 424, Text_GetString(NULL, 0, sTeamSelectText, 3));
    Font_EndTexturedTextRendering();
}

/* Draws and advances the vertical help-text strip for the current rules. */
void BattleMenu_DrawScrollingHelpText(void) {
    s32 i;
    s32 char_index;
    s32 y;
    s8 pad[1];
    s8* scrolling_text;
    DepthBuffer* depth_buffer;
    Color_RGB8 help_text_gradient_start;
    Color_RGB8 help_text_gradient_end;

    y = 48 - (sScrollingTextProgress % 24);
    char_index = sScrollingTextProgress / 24;
    depth_buffer = StageContext_GetCurrentImage();
    help_text_gradient_start = sHelpTextGradientStart;
    help_text_gradient_end = sHelpTextGradientEnd;
    Ui_DrawGradientPanel(552, 32, 46, 390, &help_text_gradient_start, &help_text_gradient_end);

    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 563, 48, 587, 408);

    Font_BeginTranslucentTextRendering();
    Font_SetActive(0x10, 0);
    Gfx_SetEnvColor(0xA5, 0xDE, 0x8C, 0xFF); // #A5DE8C

    if ((sSessionContext->ruleCategory == RULE_CATEGORY_ANYTHING_GOES) ||
        (sSessionContext->ruleCategory == RULE_CATEGORY_VS_MEWTWO)) {
        scrolling_text = Text_GetString(NULL, 0, sTeamSelectText, 1);
    } else {
        scrolling_text = Text_GetString(NULL, 0, sTeamSelectText, 0);
    }

    for (i = 0; i < 16; i++) {
        Font_DrawCharAt(((46 - Font_GetCharWidth(0x10, scrolling_text[char_index])) / 2) + 552, y,
                        scrolling_text[char_index]);
        char_index += 1;
        if (strlen(scrolling_text) < char_index) {
            char_index = 0;
        }
        y += 24;
    }
    Font_EndTexturedTextRendering();

    gDPSetScissorFrac(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, depth_buffer->width * 4.0f,
                      depth_buffer->height * 4.0f);

    sScrollingTextProgress += 2;
    if (sScrollingTextProgress >= ((strlen(scrolling_text) * 24) + 24)) {
        sScrollingTextProgress = 0;
    }
}

/* Initializes a side's selection icons and controller-port labels. */
void BattleMenu_InitTeamOrderState(TeamOrderState* team_order_state, BattleSessionTeams* battle_session_teams,
                                   s32 rule_category) {
    s32 i;

    team_order_state->numberPicked = 0;
    team_order_state->isCom = 0;

    for (i = 0; i < 6; i++) {
        team_order_state->slotIconStates[i] = 0;
        team_order_state->slotIconDirections[i] = 1;
    }

    if (rule_category != RULE_CATEGORY_ANYTHING_GOES) {
        if (rule_category != RULE_CATEGORY_VS_MEWTWO) {
            // clang-format off
            for (i = 3; i < 6; i++) { team_order_state->slotIconStates[i] = -1; }
            // clang-format on
        }
    }

    if (battle_session_teams->tagTeamSize == 1) {
        team_order_state->firstTagTeamPartnerPort = battle_session_teams->tagTeamMember[0]->controllerPort;
        team_order_state->secondTagTeamPartnerPort = -1;
        team_order_state->isCom = (battle_session_teams->tagTeamMember[0]->isHuman & 0x2) != 0;
    } else if (battle_session_teams->tagTeamMember[0]->controllerPort <
               battle_session_teams->tagTeamMember[1]->controllerPort) {
        team_order_state->firstTagTeamPartnerPort = battle_session_teams->tagTeamMember[0]->controllerPort;
        team_order_state->secondTagTeamPartnerPort = battle_session_teams->tagTeamMember[1]->controllerPort;
    } else {
        team_order_state->firstTagTeamPartnerPort = battle_session_teams->tagTeamMember[1]->controllerPort;
        team_order_state->secondTagTeamPartnerPort = battle_session_teams->tagTeamMember[0]->controllerPort;
    }
}

/* Draws the side's COM, single-controller, or paired-controller label. */
void BattleMenu_DrawPlayerPortIcon(s32 x, s32 y, TeamOrderState* state) {
    u8* texture;

    if (state->secondTagTeamPartnerPort == -1) {
        if (state->isCom != 0) {
            Gfx_DrawTextureRgba16(x, y, 36, 26, gComPlayerLabelIcon, 36, 0x200000);
        } else {
            Gfx_DrawTextureRgba16(x, y, 36, 26, sHumanPlayerLabelIcon[state->firstTagTeamPartnerPort], 36, 0x200000);
        }
    } else {
        switch (state->firstTagTeamPartnerPort) {
            case 0:
                texture = D_84803690[state->secondTagTeamPartnerPort];
                break;

            case 1:
                texture = D_84803698[state->secondTagTeamPartnerPort];
                break;

            case 2:
                texture = D_8480369C[0][state->secondTagTeamPartnerPort];
                break;
        }
        Gfx_DrawTextureRgba16(x, y, 36, 45, texture, 36, 0x200000);
    }
}

/* Advances a packed selection-icon frame and draws its current texture. */
s32 BattleMenu_DrawOrderSlotIcon(s32 x, s32 y, s8 icon_state, s8 animation_direction) {
    s32 owner_index;
    s32 frame;

    if (icon_state != 0) {
        if (icon_state == -1) {
            Gfx_DrawTextureRgba16(x, y, 32, 20, D_3005680, 32, 0x200000);
        } else {
            owner_index = (icon_state >> 4) & 0xF;
            frame = icon_state & 0xF;

            if (animation_direction > 0) {
                if (frame < 3) {
                    frame++;
                }
            } else if (frame > 0) {
                frame--;
            }

            if (frame > 0) {
                Gfx_DrawTextureRgba16(x + 6, y, 24, 20, D_8480369C[owner_index][frame], 24, 0x200000);
                icon_state = (owner_index * 0x10) | frame;
            } else {
                icon_state = 0;
            }
        }
    }
    return icon_state;
}

/* Draws side 0's order sidebar and advances its selection icons. */
void BattleMenu_DrawTeamOrderSidebarTeam0(void) {
    TeamOrderState* state = &sTeamOrderStates[0];
    s32 i;
    s16 y;
    s32 poke_ball_y;

    if (state->secondTagTeamPartnerPort == -1) {
        y = 63;
    } else {
        y = 74;
    }

    gSPDisplayList(gDisplayListHead++, D_8006F4E0);

    Gfx_DrawTextureRgba16(45, y, 32, 12, gBattleOrderSideBarSlotTop, 32, 0x200000);
    Gfx_DrawTextureRgba16(45, y + 12, 32, 23, gBattleOrderSideBarSlot, 32, 0x200000);
    Gfx_DrawTextureRgba16(45, y + 35, 32, 23, gBattleOrderSideBarSlot, 32, 0x200000);
    Gfx_DrawTextureRgba16(45, y + 58, 32, 23, gBattleOrderSideBarSlot, 32, 0x200000);
    Gfx_DrawTextureRgba16(45, y + 81, 32, 23, gBattleOrderSideBarSlot, 32, 0x200000);
    Gfx_DrawTextureRgba16(37, y + 104, 40, 46, gBattleOrderSideBarFist, 40, 0x200000);

    if (state->secondTagTeamPartnerPort == -1) {
        BattleMenu_DrawPlayerPortIcon(45, 33, &sTeamOrderStates[0]);
    } else {
        BattleMenu_DrawPlayerPortIcon(45, 29, &sTeamOrderStates[0]);
    }

    poke_ball_y = y + 118;
    for (i = 0; i < 6; i++) {
        state->slotIconStates[i] = BattleMenu_DrawOrderSlotIcon(45, poke_ball_y - i * 23, state->slotIconStates[i],
                                                                    state->slotIconDirections[i]);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

/* Draws side 1's order sidebar and advances its selection icons. */
void BattleMenu_DrawTeamOrderSidebarTeam1(void) {
    TeamOrderState* state = &sTeamOrderStates[1];
    s32 i;
    s16 y;
    s32 poke_ball_y;

    if (state->secondTagTeamPartnerPort == -1) {
        y = 241;
    } else {
        y = 230;
    }

    gSPDisplayList(gDisplayListHead++, D_8006F4E0);

    Gfx_DrawTextureRgba16(45, y, 40, 44, D_3003F80, 40, 0x200000);
    Gfx_DrawTextureRgba16(45, y + 44, 32, 23, gBattleOrderSideBarSlot, 32, 0x200000);
    Gfx_DrawTextureRgba16(45, y + 67, 32, 23, gBattleOrderSideBarSlot, 32, 0x200000);
    Gfx_DrawTextureRgba16(45, y + 90, 32, 23, gBattleOrderSideBarSlot, 32, 0x200000);
    Gfx_DrawTextureRgba16(45, y + 113, 32, 23, gBattleOrderSideBarSlot, 32, 0x200000);
    Gfx_DrawTextureRgba16(45, y + 136, 32, 14, D_3004D40, 32, 0x200000);

    if (state->secondTagTeamPartnerPort == -1) {
        BattleMenu_DrawPlayerPortIcon(45, 393, &sTeamOrderStates[1]);
    } else {
        BattleMenu_DrawPlayerPortIcon(45, 380, &sTeamOrderStates[1]);
    }

    poke_ball_y = y + 12;
    for (i = 0; i < 6; i++) {
        state->slotIconStates[i] = BattleMenu_DrawOrderSlotIcon(45, poke_ball_y + i * 23, state->slotIconStates[i],
                                                                    state->slotIconDirections[i]);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

/* Appends a growing selection icon owned by the given controller port. */
void BattleMenu_AppendOrderSlotIcon(s32 side, s32 controller_port) {
    TeamOrderState* state = &sTeamOrderStates[side];

    state->slotIconStates[state->numberPicked] =
        ((controller_port + 1) << 4) | (state->slotIconStates[state->numberPicked] & 0xF);
    state->slotIconDirections[state->numberPicked] = 1;
    state->numberPicked++;
}

/* Starts shrinking all selection icons and clears the side's pick count. */
void BattleMenu_ResetOrderSlotIcons(s32 side) {
    TeamOrderState* state = &sTeamOrderStates[side];
    s32 i;

    for (i = 0; i < 6; i++) {
        state->slotIconDirections[i] = -1;
    }
    state->numberPicked = 0;
}

/* Draws a panel using the border color assigned to its controller port. */
void BattleMenu_DrawPanelBorder(s16 x, s16 y, s16 width, s16 height, s16 port) {
    Ui_DrawBorderedPanelStyleB(x, y, width, height, sBorderPortColors[port]);
}

/* Draws the six slot buttons, Pokemon icons, names, and levels. */
void BattleMenu_DrawOrderSelectPanel_PartyGrid(BattleOrderInputState* input_state) {
    s32 i;
    s16 x;
    s16 y;
    s32 pokemon_x;
    s32 pokemon_y;
    s32 is_slot_selected;

    x = input_state->x;
    y = input_state->y;

    BattleMenu_DrawPanelBorder(x, y, 471, 94, input_state->controllerPort);

    gSPDisplayList(gDisplayListHead++, D_8006F4E0);

    Gfx_DrawTextureRgba16(x + 7, y + 11, 26, 26, &D_30005B0, 28, 0x200000);
    Gfx_DrawTextureRgba16(x + 161, y + 11, 26, 26, &D_3001110, 28, 0x200000);
    Gfx_DrawTextureRgba16(x + 315, y + 11, 26, 26, &D_3001C70, 28, 0x200000);
    Gfx_DrawTextureRgba16(x + 7, y + 53, 26, 26, D_3000000, 28, 0x200000);
    Gfx_DrawTextureRgba16(x + 161, y + 53, 26, 26, &D_3000B60, 28, 0x200000);
    Gfx_DrawTextureRgba16(x + 315, y + 53, 26, 26, &D_30016C0, 28, 0x200000);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    Gfx_DrawTextureRgba16(x + 35, y + 6, 40, 40, input_state->firstPokemonIcon[0], 40, 0);
    Gfx_DrawTextureRgba16(x + 189, y + 6, 40, 40, input_state->otherPokemonIcon, 40, 0);
    Gfx_DrawTextureRgba16(x + 343, y + 6, 40, 40, &input_state->otherPokemonIcon[3200], 40, 0);
    Gfx_DrawTextureRgba16(x + 35, y + 48, 40, 40, &input_state->otherPokemonIcon[6400], 40, 0);
    Gfx_DrawTextureRgba16(x + 189, y + 48, 40, 40, &input_state->otherPokemonIcon[9600], 40, 0);
    Gfx_DrawTextureRgba16(x + 343, y + 48, 40, 40, &input_state->otherPokemonIcon[12800], 40, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    Font_BeginTranslucentTextRendering();
    Font_SetActive(4, 0);

    for (i = 0; i < input_state->rosterSize; i++) {
        is_slot_selected = BattleMenu_IsSlotAlreadySelected(input_state, i);
        if ((input_state->rButtonHeld != 0) && (is_slot_selected != 0)) {
            Gfx_SetEnvColor(0xFF, 0xFF, 0, 0xFF); // #FFFF00
        }
        pokemon_x = x + ((i % 3) * 154) + 76;
        pokemon_y = y + ((i / 3) * 42) + 8;
        Font_Printf(pokemon_x, pokemon_y, input_state->pokemon[i].nickname);
        if ((input_state->rButtonHeld != 0) && (is_slot_selected != 0)) {
            Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #FFFFFF
        }
        Font_Printf(pokemon_x, pokemon_y + 20, "%s%d", Text_GetString(NULL, 0, sPokemonDetailText, 21),
                    input_state->pokemon[i].level);
    }

    Font_EndTexturedTextRendering();

    if (input_state->overlayClearFrames > 0) {
        if (input_state->sideIndex == 0) {
            Gfx_DrawRgba16ImageRegion(sBackgroundImage, 200, 129, 351, 83);
        } else {
            Gfx_DrawRgba16ImageRegion(sBackgroundImage, 80, 240, 351, 83);
        }
        input_state->overlayClearFrames -= 1;
    }
}

/* Draws a horizontally oscillating marker and advances its per-panel phase. */
void BattleMenu_DrawCursorArrow(s16 x, s16 y, s16 oscillation_index) {
    if (sCursorOscillationPhases[oscillation_index] < 6) {
        x += sCursorOscillationPhases[oscillation_index];
    } else {
        x = (x - sCursorOscillationPhases[oscillation_index]) + 12;
    }
    Ui_DrawTextureMarker(x, y);
    sCursorOscillationPhases[oscillation_index] = (sCursorOscillationPhases[oscillation_index] + 1) % 12;
}

/* Draws the order-confirmation choices and the current choice marker. */
void BattleMenu_DrawOrderSelectPanel_ConfirmPrompt(BattleOrderInputState* input_state) {
    s32 sp38;
    s16 x;
    s16 y;

    if (input_state->partnerPanelIndex == -1) {
        if (input_state->sideIndex == 0) {
            x = 200;
            y = 129;
        } else {
            x = 80;
            y = 240;
        }

        BattleMenu_DrawOrderSelectPanel_PartyGrid(input_state);
        BattleMenu_DrawPanelBorder(x, y, 351, 83, input_state->controllerPort);

        if (input_state->confirmChoice == 0) {
            BattleMenu_DrawCursorArrow(x + 38, y + 48, input_state->panelIndex);
        } else {
            BattleMenu_DrawCursorArrow(x + 134, y + 48, input_state->panelIndex);
        }

        Font_BeginTranslucentTextRendering();
        Font_SetActive(0x10, 0);
        Font_Printf(x + 31, y + 11, Text_GetString(NULL, 0, sTeamSelectText, 4));
        Font_Printf(x + 79, y + 48, Text_GetString(NULL, 0, sTeamSelectText, 5));
        Font_Printf(x + 175, y + 48, Text_GetString(NULL, 0, sTeamSelectText, 6));
        Font_EndTexturedTextRendering();
        input_state->overlayClearFrames = 2;
    } else {
        x = input_state->x;
        y = input_state->y;

        BattleMenu_DrawOrderSelectPanel_PartyGrid(input_state);
        Gfx_FillRectWithDisplayState(x + 5, y + 5, 461, 84, 0, 0, 0, 0x80);
        if (input_state->confirmChoice == 0) {
            BattleMenu_DrawCursorArrow(x + 92, y + 60, input_state->panelIndex);
        } else {
            BattleMenu_DrawCursorArrow(x + 188, y + 60, input_state->panelIndex);
        }
        Font_BeginTranslucentTextRendering();
        Font_SetActive(0x10, 0);
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #FFFFFF
        Font_Printf(x + 80, y + 20, Text_GetString(NULL, 0, sTeamSelectText, 4));
        Font_Printf(x + 128, y + 60, Text_GetString(NULL, 0, sTeamSelectText, 5));
        Font_Printf(x + 224, y + 60, Text_GetString(NULL, 0, sTeamSelectText, 6));
        Font_EndTexturedTextRendering();
    }
}

/* Draws the current rules' total-level-limit warning. */
void BattleMenu_DrawOrderSelectPanel_LevelCapNotice(BattleOrderInputState* input_state) {
    char message_buffer[256];
    s32 x;
    s32 y;
    s32 level_cap;

    switch (sSessionContext->ruleCategory) {
        case 3:
            level_cap = 155;
            break;

        case 4:
            level_cap = 80;
            break;

        case 5:
            level_cap = 50;
            break;
    }

    if (input_state->partnerPanelIndex == -1) {
        // clang-format off
        if (input_state->sideIndex == 0) {
            x = 200; y = 129;
        } else {
            x = 80; y = 240;
        }
        // clang-format on

        BattleMenu_DrawPanelBorder(x, y, 351, 83, input_state->controllerPort);
        Font_BeginTranslucentTextRendering();
        Font_SetActive(0x10, 0);
        Gfx_SetEnvColor(0xFF, 0x63, 0x63, 0xFF); // #FF6363
        Font_SetLineHeight(37);
        Text_SetNumberToken(3, level_cap);
        Font_Printf(x + 16, y + 11, Text_GetString(message_buffer, sizeof(message_buffer), sTeamSelectText, 7));
        Font_EndTexturedTextRendering();
        input_state->overlayClearFrames = 2;
    } else {
        x = input_state->x;
        y = input_state->y;

        BattleMenu_DrawOrderSelectPanel_PartyGrid(input_state);
        Gfx_FillRectWithDisplayState(x + 5, y + 5, 461, 84, 0, 0, 0, 0x80);
        Font_BeginTranslucentTextRendering();
        Font_SetActive(0x10, 0);
        Gfx_SetEnvColor(0xFF, 0x63, 0x63, 0xFF); // #FF6363
        Font_SetLineHeight(40);
        Text_SetNumberToken(3, level_cap);
        Font_Printf(x + 80, y + 20, Text_GetString(NULL, 0, sTeamSelectText, 7));
        Font_EndTexturedTextRendering();
    }
}

/* Draws the ready message, pulsing its text alpha for tag-team panels. */
void BattleMenu_DrawOrderSelectPanel_Ready(BattleOrderInputState* input_state) {
    s32 x;
    s32 y;
    s32 alpha;
    char* text;

    if (input_state->partnerPanelIndex == -1) {
        // clang-format off
        if (input_state->sideIndex == 0) {
            x = 200; y = 129;
        } else {
            x = 80; y = 240;
        }
        // clang-format on
        input_state->overlayClearFrames = 0;
        BattleMenu_DrawOrderSelectPanel_PartyGrid(input_state);
        BattleMenu_DrawPanelBorder(x, y, 351, 83, input_state->controllerPort);
        Font_BeginTranslucentTextRendering();
        Font_SetActive(0x20, 0);
        Font_EnableTwoCycleTexturing();
        Gfx_SetPrimColor(0x1C, 0x74, 0xFF, 0xFF); // #1C74FF
        text = Text_GetString(NULL, 0, sTeamSelectText, 8);
        Font_Printf((x - (Font_MeasureTextExtent(0x20, 0, text) / 2)) + 175, y + 25, text);
        Font_DisableTwoCycleTexturing();
        Font_EndTexturedTextRendering();
    } else {
        x = input_state->x;
        y = input_state->y;

        if (input_state->readyPulseFrame < 16) {
            alpha = input_state->readyPulseFrame * 16;
        } else if (input_state->readyPulseFrame < 35) {
            alpha = 0xFF;
        } else if (input_state->readyPulseFrame < 50) {
            alpha = (-input_state->readyPulseFrame * 16) + 800;
        } else {
            alpha = 0;
        }

        input_state->readyPulseFrame++;
        if (input_state->readyPulseFrame >= 61) {
            input_state->readyPulseFrame = 0;
        }

        BattleMenu_DrawOrderSelectPanel_PartyGrid(input_state);
        Gfx_FillRectWithDisplayState(x + 5, y + 5, 461, 84, 0, 0, 0, 0x80);
        Font_BeginTranslucentTextRendering();
        Font_SetActive(0x20, 0);
        Font_EnableTwoCycleTexturing();
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, alpha);  // #FFFFFF
        Gfx_SetPrimColor(0x1C, 0x74, 0xFF, alpha); // #1C74FF
        text = Text_GetString(NULL, 0, sTeamSelectText, 8);
        Font_Printf((x - (Font_MeasureTextExtent(32, 0, text) / 2)) + 235, y + 37, text);
        Font_DisableTwoCycleTexturing();
        Font_EndTexturedTextRendering();
    }
}

/* Draws a Pokemon's stats, types, moves, and remaining PP. */
void BattleMenu_DrawOrderSelectPanel_MonDetail(BattleOrderInputState* input_state) {
    s32 i;
    s16 x;
    s8 slot_to_summarize;
    PokemonTypeStyle* move_type_style;
    s32 type_index;
    s32 y;
    BattleMon* pokemon_to_summarize;

    x = input_state->x;
    slot_to_summarize = input_state->summaryDisplaySlot;
    y = input_state->y;
    pokemon_to_summarize = &input_state->pokemon[slot_to_summarize];

    BattleMenu_DrawPanelBorder(x, y, 471, 94, input_state->controllerPort);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    Gfx_DrawTextureRgba16(x + 8, y + 7, 40, 40, &input_state->firstPokemonIcon[slot_to_summarize], 40, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    Font_BeginTranslucentTextRendering();
    Font_SetActive(4, 0);
    Font_Printf(x + 50, y + 7, input_state->pokemon[slot_to_summarize].nickname);
    Font_Printf(x + 50, y + 27, "%s", Text_GetString(NULL, 0, sPokemonDetailText, 8));
    Font_Printf(x + 190, y + 7, Text_GetString(NULL, 0, sTeamSelectText, 9));
    Font_Printf(x + 190, y + 27, Text_GetString(NULL, 0, sTeamSelectText, 10));
    Font_Printf(x + 190, y + 47, Text_GetString(NULL, 0, sTeamSelectText, 11));
    Font_Printf(x + 190, y + 67, Text_GetString(NULL, 0, sTeamSelectText, 12));
    Font_Printf(x + 50, y + 47, Text_GetString(NULL, 0, sTeamSelectText, 13));

    if (pokemon_to_summarize->type1 != pokemon_to_summarize->type2) {
        Font_Printf(x + 50, y + 67, Text_GetString(NULL, 0, sTeamSelectText, 14));
    }

    type_index = PokemonType_ToDisplayIndex(pokemon_to_summarize->type1);
    Gfx_SetEnvColor(sPokemonTypeStyles[type_index].color.r, sPokemonTypeStyles[type_index].color.g, sPokemonTypeStyles[type_index].color.b, 0xFF);
    Font_Printf(x + 122, y + 47,
                Text_GetString(NULL, 0, sTypeNameTable, PokemonType_ToDisplayIndex(pokemon_to_summarize->type1)));

    if (pokemon_to_summarize->type1 != pokemon_to_summarize->type2) {
        type_index = PokemonType_ToDisplayIndex(pokemon_to_summarize->type2);
        Gfx_SetEnvColor(sPokemonTypeStyles[type_index].color.r, sPokemonTypeStyles[type_index].color.g, sPokemonTypeStyles[type_index].color.b, 0xFF);
        Font_Printf(x + 122, y + 67,
                    Text_GetString(NULL, 0, sTypeNameTable, PokemonType_ToDisplayIndex(pokemon_to_summarize->type2)));
    }

    Font_SetActive(4, 0);
    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #FFFFFF
    Font_Printf(x + 140, y + 7, "%s%d", Text_GetString(NULL, 0, sPokemonDetailText, 21),
                input_state->pokemon[slot_to_summarize].level);
    Font_Printf(x + 82, y + 27, "%3d", pokemon_to_summarize->maxHp);
    Font_Printf(x + 250, y + 7, "%3d", pokemon_to_summarize->attack);
    Font_Printf(x + 250, y + 27, "%3d", pokemon_to_summarize->defense);
    Font_Printf(x + 250, y + 47, "%3d", pokemon_to_summarize->speed);
    Font_Printf(x + 250, y + 67, "%3d", pokemon_to_summarize->special);

    for (i = 0; i < 4; i++) {
        if (pokemon_to_summarize->moves[i] == 0) {
            break;
        }
        move_type_style = &sPokemonTypeStyles[gMoveDisplayInfo[pokemon_to_summarize->moves[i] - 1].type];
        Font_SetActive(4, 0);
        Gfx_SetEnvColor(move_type_style->color.r, move_type_style->color.g, move_type_style->color.b, 0xFF);
        Font_Printf(x + 332, (i * 20) + y + 7,
                    Text_GetString(NULL, 0, sMoveNameTable, pokemon_to_summarize->moves[i] - 1));
        Font_SetActive(4, 0);
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #FFFFFF
        Font_Printf(x + 435, (i * 20) + y + 7, "%2d", pokemon_to_summarize->pp[i] & 0x3F);
    }

    Font_EndTexturedTextRendering();

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    for (i = 0; i < 4; i++) {
        if (pokemon_to_summarize->moves[i] == 0) {
            break;
        }
        Gfx_DrawTextureRgba16(x + 310, (i * 20) + y + 7, 20, 20,
                              sPokemonTypeStyles[gMoveDisplayInfo[pokemon_to_summarize->moves[i] - 1].type].iconTexture, 20, 0);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    if (input_state->overlayClearFrames > 0) {
        if (input_state->sideIndex == 0) {
            Gfx_DrawRgba16ImageRegion(sBackgroundImage, 200, 129, 351, 83);
        } else {
            Gfx_DrawRgba16ImageRegion(sBackgroundImage, 80, 240, 351, 83);
        }
        input_state->overlayClearFrames--;
    }
}

/* Draws the trainer name in a controller-colored panel. */
void BattleMenu_DrawTrainerLabel(BattleOrderInputState* input_state) {
    s16 x;
    s16 y;

    if (input_state->sideIndex == 0) {
        // clang-format off
        x = 80; y = 129;
        // clang-format on
    } else {
        x = 433;
        y = 291;
    }

    BattleMenu_DrawPanelBorder(x, y, 118, 32, input_state->controllerPort);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(8, 0);
    Font_Printf((x - (Font_MeasureTextExtent(8, 0, input_state->roster->trainerName) / 2)) + 59, y + 5,
                input_state->roster->trainerName);
    Font_EndTexturedTextRendering();
}

/* Draws the current panel mode when needed and updates its redraw countdown. */
void BattleMenu_DrawOrderSelectPanel(BattleOrderInputState* input_state) {
    if ((sFullRedrawFrames != 0) || (input_state->redrawFrames != 0)) {
        if ((sFullRedrawFrames != 0) && (input_state->partnerPanelIndex == -1)) {
            BattleMenu_DrawTrainerLabel(input_state);
        }

        switch (input_state->panelMode) {
            case 0:
                BattleMenu_DrawOrderSelectPanel_PartyGrid(input_state);
                break;

            case 1:
                BattleMenu_DrawOrderSelectPanel_ConfirmPrompt(input_state);
                break;

            case 2:
                BattleMenu_DrawOrderSelectPanel_LevelCapNotice(input_state);
                break;

            case 3:
                BattleMenu_DrawOrderSelectPanel_Ready(input_state);
                break;

            case 4:
                BattleMenu_DrawOrderSelectPanel_MonDetail(input_state);
                break;
        }

        if (input_state->redrawFrames != 0) {
            input_state->redrawFrames--;
        }
    } else if (input_state->panelMode == 3) {
        BattleMenu_DrawOrderSelectPanel_Ready(input_state);
    }
}

/* Advances the scripted button presses used by the COM selection panel. */
void BattleMenu_UpdateComPanelAutoInput(void) {
    s32 i;
    s8 number_of_pokemon_to_pick;
    ComSelectionButtonPresses* com_button_presses;

    for (i = 0; i < 4; i++) {
        if ((sBattleOrderInputStates[i].roster != NULL) && (sBattleOrderInputStates[i].roster->isHuman & 0x2)) {
            break;
        }
    }

    if (i < 4) {
        if ((sSessionContext->ruleCategory != RULE_CATEGORY_ANYTHING_GOES) &&
            (sSessionContext->ruleCategory != RULE_CATEGORY_VS_MEWTWO)) {
            number_of_pokemon_to_pick = 3;
        } else {
            number_of_pokemon_to_pick = sBattleOrderInputStates[i].rosterSize;
        }
        sComController.buttonPressed = 0;
        if (sComSelectionTimer == 0) {
            com_button_presses = sComButtonPressesByRosterSize[number_of_pokemon_to_pick];

            sComSelectionTimer = com_button_presses[sComNextButtonPress].delay;
            if (sComSelectionTimer != 0) {
                sComController.buttonPressed = com_button_presses[sComNextButtonPress].button;
                sComNextButtonPress++;
            }
        }

        if (sComSelectionTimer > 0) {
            sComSelectionTimer--;
        }
    }
}

/* Tests whether a roster slot already occurs in the panel's picked order. */
s32 BattleMenu_IsSlotAlreadySelected(BattleOrderInputState* input_state, s8 slot) {
    s32 i;
    s8 pad;
    BattleOrderInputState* pad2;

    for (i = 0; i < input_state->numberOfPokemonPicked; i++) {
        if (slot == input_state->pickedSlots[i]) {
            return 1;
        }
    }

    return 0;
}

/* Tests picked level totals against the rules' intermediate and final limits. */
s32 BattleMenu_PicksExceedLevelCap(BattleOrderInputState* input_state) {
    s32 i;
    s32 level_sum = 0;

    if (!(input_state->roster->isHuman & 0x2)) {
        for (i = 0; i < input_state->numberOfPokemonPicked; i++) {
            level_sum += input_state->pokemon[input_state->pickedSlots[i]].level;
        }

        if (sSessionContext->ruleCategory == RULE_CATEGORY_NINTENDO_CUP_99) {
            if ((input_state->numberOfPokemonPicked == 2) && (level_sum >= 106)) {
                return 1;
            }

            if ((input_state->numberOfPokemonPicked == 3) && (level_sum >= 156)) {
                return 1;
            }
        } else if (sSessionContext->ruleCategory == RULE_CATEGORY_PETIT_CUP) {
            if ((input_state->numberOfPokemonPicked == 2) && (level_sum >= 56)) {
                return 1;
            }

            if ((input_state->numberOfPokemonPicked == 3) && (level_sum >= 81)) {
                return 1;
            }
        } else if (sSessionContext->ruleCategory == RULE_CATEGORY_PIKA_CUP) {
            if ((input_state->numberOfPokemonPicked == 2) && (level_sum >= 36)) {
                return 1;
            }

            if ((input_state->numberOfPokemonPicked == 3) && (level_sum >= 51)) {
                return 1;
            }
        }
    }

    return 0;
}

/* Tests whether the panel has picked its required number of Pokemon. */
s32 BattleMenu_IsOrderSelectionComplete(BattleOrderInputState* input_state) {
    s8 number_of_picks_allowed;

    if ((sSessionContext->ruleCategory != RULE_CATEGORY_ANYTHING_GOES) && (sSessionContext->ruleCategory != RULE_CATEGORY_VS_MEWTWO)) {
        number_of_picks_allowed = 3;
    } else if (input_state->roster->isHuman & 0x2) {
        number_of_picks_allowed = input_state->rosterSize;
    } else {
        number_of_picks_allowed = input_state->rosterSize;
        if ((input_state->partnerPanelIndex != -1) && (number_of_picks_allowed >= 4)) {
            number_of_picks_allowed = 3;
        }
    }

    return input_state->numberOfPokemonPicked >= number_of_picks_allowed;
}

/* Opens a slot's details or adds it to the picked order and checks the result. */
void BattleMenu_SelectSlotOrShowSummary(BattleOrderInputState* input_state, s8 selected_slot, s32 show_summary) {
    if (selected_slot >= input_state->rosterSize) {
        Audio_PlaySoundEffectById(SFX_MENU_ERROR);
        return;
    }

    if (show_summary != 0) {
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
        input_state->panelMode = 4;
        input_state->redrawFrames = 2;
        input_state->summaryDisplaySlot = selected_slot;
        return;
    }

    if (BattleMenu_IsSlotAlreadySelected(input_state, selected_slot) != 0) {
        Audio_PlaySoundEffectById(SFX_MENU_ERROR);
        return;
    }

    input_state->pickedSlots[input_state->numberOfPokemonPicked] = selected_slot;
    input_state->numberOfPokemonPicked++;
    BattleMenu_AppendOrderSlotIcon(input_state->sideIndex, input_state->controllerPort);

    if (input_state->rButtonHeld != 0) {
        input_state->redrawFrames = 2;
    }

    if (BattleMenu_PicksExceedLevelCap(input_state) != 0) {
        Audio_PlaySoundEffectById(SFX_MENU_ERROR);
        input_state->panelMode = 2;
        input_state->redrawFrames = 2;
        return;
    }

    Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
    if (BattleMenu_IsOrderSelectionComplete(input_state) != 0) {
        input_state->panelMode = 1;
        input_state->confirmChoice = 0;
        input_state->redrawFrames = 2;
    }
}

/* Clears a nonempty panel's picks, resetting the partner panel when present. */
void BattleMenu_UndoSelections(BattleOrderInputState* order_state) {
    BattleOrderInputState* partner_state;

    if (order_state->numberOfPokemonPicked > 0) {
        order_state->numberOfPokemonPicked = 0;

        BattleMenu_ResetOrderSlotIcons(order_state->sideIndex);

        if (order_state->rButtonHeld != 0) {
            order_state->redrawFrames = 2;
        }

        Audio_PlaySoundEffectById(SFX_MENU_BACK);

        if (order_state->partnerPanelIndex != -1) {
            partner_state = &sBattleOrderInputStates[order_state->partnerPanelIndex];
            partner_state->panelMode = 0;
            partner_state->numberOfPokemonPicked = 0;
            partner_state->rButtonHeld = 0;
            partner_state->redrawFrames = 2;
        }
    }
}

/* Maps slot buttons to picking or checking Pokemon and handles L-cancel. */
void BattleMenu_HandleOrderSelectPanel_PartyInput(Controller* controller, BattleOrderInputState* input_state) {
    s32 r_held_down = BTN_IS_DOWN(controller, BTN_R);

    if (BTN_IS_PRESSED(controller, BTN_R)) {
        // clang-format off
        input_state->rButtonHeld = 1; input_state->redrawFrames = 2;
        // clang-format on
    }

    if (controller->buttonReleased & 0x10) {
        input_state->rButtonHeld = 0;
        input_state->redrawFrames = 2;
    }

    if (BTN_IS_PRESSED(controller, BTN_L)) {
        BattleMenu_UndoSelections(input_state);
    } else if (BTN_IS_PRESSED(controller, BTN_B)) {
        BattleMenu_SelectSlotOrShowSummary(input_state, 0, r_held_down);
    } else if (BTN_IS_PRESSED(controller, BTN_CLEFT)) {
        BattleMenu_SelectSlotOrShowSummary(input_state, 1, r_held_down);
    } else if (BTN_IS_PRESSED(controller, BTN_CUP)) {
        BattleMenu_SelectSlotOrShowSummary(input_state, 2, r_held_down);
    } else if (BTN_IS_PRESSED(controller, BTN_A)) {
        BattleMenu_SelectSlotOrShowSummary(input_state, 3, r_held_down);
    } else if (BTN_IS_PRESSED(controller, BTN_CDOWN)) {
        BattleMenu_SelectSlotOrShowSummary(input_state, 4, r_held_down);
    } else if (BTN_IS_PRESSED(controller, BTN_CRIGHT)) {
        BattleMenu_SelectSlotOrShowSummary(input_state, 5, r_held_down);
    }
}

/* Handles confirmation choices, accepting the order or restarting selection. */
void BattleMenu_HandleOrderSelectPanel_ConfirmInput(Controller* controller, BattleOrderInputState* input_state) {
    BattleOrderInputState* partner_state;

    if (BTN_IS_PRESSED(controller, BTN_R)) {
        input_state->rButtonHeld = 1;
        input_state->redrawFrames = 2;
    }
    if (controller->buttonReleased & 0x10) {
        input_state->rButtonHeld = 0;
        input_state->redrawFrames = 2;
    }

    if (BTN_IS_PRESSED(controller, BTN_DLEFT | BTN_DRIGHT)) {
        input_state->redrawFrames = 2;
        input_state->confirmChoice ^= 1;
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
    } else if (BTN_IS_PRESSED(controller, BTN_A)) {
        if (input_state->confirmChoice == 0) {
            input_state->redrawFrames = 2;
            input_state->panelMode = 3;
            input_state->readyPulseFrame = 0;
            Audio_PlaySoundEffectById(9);
        } else {
            input_state->redrawFrames = 2;
            input_state->panelMode = 0;
            input_state->numberOfPokemonPicked = 0;
            input_state->rButtonHeld = 0;
            BattleMenu_ResetOrderSlotIcons(input_state->sideIndex);
            Audio_PlaySoundEffectById(SFX_MENU_BACK);
            if (input_state->partnerPanelIndex != -1) {
                partner_state = &sBattleOrderInputStates[input_state->partnerPanelIndex];
                partner_state->panelMode = 0;
                partner_state->numberOfPokemonPicked = 0;
                partner_state->rButtonHeld = 0;
                partner_state->redrawFrames = 2;
            }
        }
    }
    input_state->redrawFrames = 2;
}

/* Dismisses the level-limit warning and clears this panel's picks. */
void BattleMenu_HandleOrderSelectPanel_LevelCapNoticeInput(Controller* controller, BattleOrderInputState* input_state) {
    if (BTN_IS_PRESSED(controller, BTN_A | BTN_B)) {
        input_state->panelMode = 0;
        input_state->numberOfPokemonPicked = 0;
        input_state->rButtonHeld = 0;
        input_state->redrawFrames = 2;
        BattleMenu_ResetOrderSlotIcons(input_state->sideIndex);
        Audio_PlaySoundEffectById(SFX_MENU_BACK);
    }
}

/* Closes details when the corresponding slot button is released. */
void BattleMenu_HandleOrderSelectPanel_MonDetailInput(Controller* controller, BattleOrderInputState* input_state) {
    s32 is_valid_button = 0;

    switch (input_state->summaryDisplaySlot) {
        case 0:
            if (controller->buttonReleased & BTN_B) {
                is_valid_button = 1;
            }
            break;

        case 1:
            if (controller->buttonReleased & BTN_CLEFT) {
                is_valid_button = 1;
            }
            break;

        case 2:
            if (controller->buttonReleased & BTN_CUP) {
                is_valid_button = 1;
            }
            break;

        case 3:
            if (controller->buttonReleased & BTN_A) {
                is_valid_button = 1;
            }
            break;

        case 4:
            if (controller->buttonReleased & BTN_CDOWN) {
                is_valid_button = 1;
            }
            break;

        case 5:
            if (controller->buttonReleased & BTN_CRIGHT) {
                is_valid_button = 1;
            }
            break;
    }

    if (is_valid_button != 0) {
        if (!(controller->buttonDown & BTN_R)) {
            input_state->rButtonHeld = 0;
        }
        input_state->panelMode = 0;
        input_state->redrawFrames = 2;
    }
}

/* Dispatches panel input while enabled and reports whether the panel is ready. */
s32 BattleMenu_UpdateOrderSelectPanel(s32 fade_mode, BattleOrderInputState* input_state) {
    Controller* controller;
    s32 done = 0;

    if (input_state->roster->isHuman & 0x2) {
        controller = &sComController;
    } else {
        controller = &gControllers[input_state->controllerPort];
    }

    if (fade_mode == 0) {
        switch (input_state->panelMode) {
            case 0:
                BattleMenu_HandleOrderSelectPanel_PartyInput(controller, input_state);
                break;

            case 1:
                BattleMenu_HandleOrderSelectPanel_ConfirmInput(controller, input_state);
                break;

            case 2:
                BattleMenu_HandleOrderSelectPanel_LevelCapNoticeInput(controller, input_state);
                break;

            case 3:
                done = 1;
                break;

            case 4:
                BattleMenu_HandleOrderSelectPanel_MonDetailInput(controller, input_state);
                break;
        }
    }
    return done;
}

/* Updates and draws selection for one frame, requesting exit when all panels are ready.
 * BgStage_WaitForCondition supplies the fade mode; nonzero modes block selection input.
 */
//@src/stage_loader.c
s32 BattleMenu_UpdateOrderSelect(s32 fade_mode) {
    UNUSED s32 pad;
    s32 i;
    s32 ready_panel_count;
    s32 all_ready;

    ready_panel_count = 0;
    all_ready = 0;

    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
    BattleMenu_UpdateComPanelAutoInput();
    GeoRender_AdvanceFrameCounter();
    BgStage_DrawFrame();

    if (sFullRedrawFrames != 0) {
        BattleMenu_DrawBottomActionButtons();
    }

    for (i = 0; i < sOrderSelectPanelCount; i++) {
        ready_panel_count += BattleMenu_UpdateOrderSelectPanel(fade_mode, &sBattleOrderInputStates[i]);
        BattleMenu_DrawOrderSelectPanel(&sBattleOrderInputStates[i]);
    }

    BattleMenu_DrawTeamOrderSidebarTeam0();
    BattleMenu_DrawTeamOrderSidebarTeam1();
    BattleMenu_DrawScrollingHelpText();

    if (fade_mode == 0) {
        if (ready_panel_count == sOrderSelectPanelCount) {
            all_ready = 1;
            StageContext_SetClearColor(0xFFFF); // #FFFFFF
            Audio_StopMusic(0x10);
        }

        if (sFullRedrawFrames > 0) {
            sFullRedrawFrames -= 1;
        }
    } else {
        sFullRedrawFrames = 2;
    }
    return all_ready;
}

/* Moves the trainer owning the first selection into the lead tag-team position. */
void BattleMenu_SetLeadTagTeamMember(BattleSessionTeams* battle_session_teams, TeamOrderState* order_state) {
    TeamRoster* lead_roster;
    s32 first_controller_port;

    if (battle_session_teams->tagTeamSize == 2) {
        first_controller_port = ((order_state->slotIconStates[0] >> 4) & 0xF) - 1;
        if (first_controller_port == battle_session_teams->tagTeamMember[1]->controllerPort) {
            lead_roster = battle_session_teams->tagTeamMember[0];
            battle_session_teams->tagTeamMember[0] = battle_session_teams->tagTeamMember[1];
            battle_session_teams->tagTeamMember[1] = lead_roster;
        }
    }
}

/* Appends the unpicked roster slots in ascending order to fill six entries. */
void BattleMenu_AppendUnpickedSlots(s8* pokemon_slots, s32 number_of_slots_filled) {
    s32 i;
    s32 j;

    for (i = 0; i < 6; i++) {
        for (j = 0; j < number_of_slots_filled; j++) {
            if (i == pokemon_slots[j]) {
                break;
            }
        }

        if (j == number_of_slots_filled) {
            pokemon_slots[number_of_slots_filled] = i;
            number_of_slots_filled += 1;
        }
    }
}

/* Copies final orders into battle rosters and sets each tag team's lead trainer.
 * COM rosters keep their original order rather than using the scripted panel picks.
 */
void BattleMenu_CommitOrderSelection(void) {
    s32 i;
    s32 panels_remaining;
    TeamRoster* roster;
    BattleOrderInputState* input_state;

    input_state = sBattleOrderInputStates;
    panels_remaining = sOrderSelectPanelCount;

    while (panels_remaining-- > 0) {
        roster = input_state->roster;
        if (roster->isHuman & 0x2) {
            roster->numberOfPokemonBrought = roster->rosterInfo->numberOfPokemonCanBePicked;
            _bcopy(roster->rosterInfo->battleMon, roster->pokemonPicked, 504);
        } else {
            BattleMenu_AppendUnpickedSlots(input_state->pickedSlots, input_state->numberOfPokemonPicked);
            roster->numberOfPokemonBrought = input_state->numberOfPokemonPicked;

            for (i = 0; i < roster->rosterInfo->numberOfPokemonCanBePicked; i++) {
                roster->pokemonPicked[i] = input_state->pokemon[input_state->pickedSlots[i]];
            }
        }
        input_state++;
    }

    BattleMenu_SetLeadTagTeamMember(&sSessionContext->battleSides[0], &sTeamOrderStates[0]);
    BattleMenu_SetLeadTagTeamMember(&sSessionContext->battleSides[1], &sTeamOrderStates[1]);
}

/* Initializes one trainer's panel and loads its roster icons. */
void BattleMenu_AddOrderSelectPanel(s16 x, s16 y, TeamRoster* roster, s32 side_index, s32 partner_panel_index) {
    s32 i;
    BattleOrderInputState* input_state;
    BattleMon* pokemon;

    input_state = &sBattleOrderInputStates[sOrderSelectPanelCount];
    input_state->panelIndex = sOrderSelectPanelCount;
    input_state->controllerPort = roster->controllerPort;
    input_state->partnerPanelIndex = partner_panel_index;
    input_state->sideIndex = side_index;
    input_state->panelMode = 0;
    input_state->numberOfPokemonPicked = 0;
    input_state->redrawFrames = 2;
    input_state->overlayClearFrames = 0;
    input_state->rButtonHeld = 0;
    input_state->confirmChoice = 0;
    input_state->x = x;
    input_state->y = y;
    input_state->rosterSize = roster->rosterInfo->numberOfPokemonCanBePicked;
    input_state->pokemon = roster->rosterInfo->battleMon;
    input_state->roster = roster;

    for (i = 0; i < roster->rosterInfo->numberOfPokemonCanBePicked; i++) {
        pokemon = &roster->rosterInfo->battleMon[i];
        Pokemon_PrepareBattleMon(pokemon);
        pokemon->type1 = gSpeciesData[pokemon->species.id - 1].type1;
        pokemon->type2 = gSpeciesData[pokemon->species.id - 1].type2;
        PokeIcon_LoadModelTextureForMon(input_state->firstPokemonIcon[i], 0, pokemon);
    }

    sOrderSelectPanelCount++;
}

/* Resets selection state and creates controller-ordered panels for both sides. */
void BattleMenu_InitOrderSelectPanels(SessionContext* session_context) {
    BattleSessionTeams* session_teams = &session_context->battleSides[0];

    sSessionContext = session_context;
    sOrderSelectPanelCount = 0;
    sFullRedrawFrames = 2;
    sScrollingTextProgress = 0;
    sComNextButtonPress = 0;
    sComSelectionTimer = 0;
    sComController.buttonPressed = 0;

    BattleMenu_InitTeamOrderState(&sTeamOrderStates[0], &sSessionContext->battleSides[0],
                                  sSessionContext->ruleCategory);
    BattleMenu_InitTeamOrderState(&sTeamOrderStates[1], &sSessionContext->battleSides[1],
                                  sSessionContext->ruleCategory);

    if (session_teams->tagTeamSize == 1) {
        BattleMenu_AddOrderSelectPanel(80, 33, session_teams->tagTeamMember[0], 0, -1);
    } else if (session_teams->tagTeamMember[0]->controllerPort < session_teams->tagTeamMember[1]->controllerPort) {
        BattleMenu_AddOrderSelectPanel(80, 33, session_teams->tagTeamMember[0], 0, sOrderSelectPanelCount + 1);
        BattleMenu_AddOrderSelectPanel(80, 129, session_teams->tagTeamMember[1], 0, sOrderSelectPanelCount - 1);
    } else {
        BattleMenu_AddOrderSelectPanel(80, 33, session_teams->tagTeamMember[1], 0, sOrderSelectPanelCount + 1);
        BattleMenu_AddOrderSelectPanel(80, 129, session_teams->tagTeamMember[0], 0, sOrderSelectPanelCount - 1);
    }

    session_teams++;
    if (session_teams->tagTeamSize == 1) {
        BattleMenu_AddOrderSelectPanel(80, 326, session_teams->tagTeamMember[0], 1, -1);
    } else if (session_teams->tagTeamMember[0]->controllerPort < session_teams->tagTeamMember[1]->controllerPort) {
        BattleMenu_AddOrderSelectPanel(80, 230, session_teams->tagTeamMember[0], 1, sOrderSelectPanelCount + 1);
        BattleMenu_AddOrderSelectPanel(80, 326, session_teams->tagTeamMember[1], 1, sOrderSelectPanelCount - 1);
    } else {
        BattleMenu_AddOrderSelectPanel(80, 230, session_teams->tagTeamMember[1], 1, sOrderSelectPanelCount + 1);
        BattleMenu_AddOrderSelectPanel(80, 326, session_teams->tagTeamMember[0], 1, sOrderSelectPanelCount - 1);
    }
}

#if defined(VERSION_US_REV2)
// asm-processor requires this import on one physical line.
// clang-format off
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/battle_order_select/battle_order_select/BattleMenu_RunOrderSelectScreen.s")
// clang-format on
#else
/* Loads selection resources, runs the screen through its fades, and commits the orders. */
s32 BattleMenu_RunOrderSelectScreen(s32 arg0, s32 arg1) {
    StageContext* stage_context;

    main_pool_push_state('MSEL');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    stage_context = StageContext_Allocate(1, 1, 2, 0, 2, 1);
    Text_InitStringTables();
    Font_Init(60, 0);
    PokeIcon_OpenModelArchive();
    sBackgroundArchive = ASSET_LOAD2(backgrounds, 1, 1);
    sBackgroundImage = BinArchive_GetFile(sBackgroundArchive, 5);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_3000000, order_select_ui, 0);
    ASSET_LOAD(D_2000000, common_menu2_ui, 0);

    sMoveNameTable = Text_GetStringTable(37);
    sTypeNameTable = Text_GetStringTable(38);
    sTeamSelectText = Text_GetStringTable(18);
    sPokemonDetailText = Text_GetStringTable(6);

    BattleMenu_InitOrderSelectPanels(&gSessionContext);
    Audio_PlayMusicIfChanged(7);
    StageContext_Activate(stage_context);
    BgStage_WaitForCondition(BattleMenu_UpdateOrderSelect, 8, 8);
    StageContext_Deactivate();
    BattleMenu_CommitOrderSelection();
    Font_Free();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('MSEL');
    return 0;
}
#endif
