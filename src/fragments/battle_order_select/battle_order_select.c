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
static BinArchive* D_84803780;
static void* D_84803784;
static char** sMoveNameTable;
static char** sTypeNameTable;
static char** sTeamSelectText;
static char** sPokemonDetailText;
static s16 D_84803798;
static s16 sScrollingTextProgress;
static s32 D_8480379C;
static BattleOrderInputState sBattleOrderInputStates[4];
static SessionContext* sSessionContext;
static Controller D_84816428;
static s32 D_8481644C;
static s32 D_84816450;
static s16 D_84816458[4];

static u16 sBorderPortColors[] = { 0x1D, 0x341, 0x6947, 0x6A41 };
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
static unk_D_848035FC D_848035FC[] = {
    { { 0xF0, 0xF0, 0xF0 }, D_2017740 }, { { 0xF0, 0x96, 0x32 }, D_2019040 }, { { 0x50, 0x96, 0xF0 }, D_20199A0 },
    { { 0xC8, 0x96, 0xF0 }, D_2019680 }, { { 0xF0, 0xC8, 0x78 }, D_2018D20 }, { { 0x64, 0xB4, 0xB4 }, D_2018A00 },
    { { 0x50, 0x96, 0xF0 }, D_20199A0 }, { { 0xC8, 0xF0, 0x50 }, D_2019360 }, { { 0xDC, 0xC8, 0xF0 }, D_2019FE0 },
    { { 0xF0, 0x64, 0x64 }, D_2017A60 }, { { 0x46, 0xB4, 0xF0 }, D_2017D80 }, { { 0x64, 0xC8, 0x64 }, D_20183C0 },
    { { 0xF0, 0xF0, 0x32 }, D_20186E0 }, { { 0xF0, 0x8C, 0xB4 }, D_2019CC0 }, { { 0xB4, 0xF0, 0xF0 }, D_20180A0 },
    { { 0x64, 0xDC, 0xC8 }, D_201A300 },
};
static Color_RGB8 sHelpTextGradientStart = { 0x00, 0x0A, 0x41 }; // #000A41
static Color_RGB8 sHelpTextGradientEnd = { 0x00, 0x00, 0xD2 };   // #0000D2
static u8* sHumanPlayerLabelIcon[] = { sPlayer1LabelIcon, sPlayer2LabelIcon, D_3009E70 };
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
    Gfx_DrawTiledRgba16Image(D_84803784);

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

// better name: BattleMenu_DrawScrollingHelpText
void BattleMenu_DrawScrollingRulesText(void) {
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

void BattleMenu_InitTeamOrderState(TeamOrderState* team_order_state, BattleSessionTeams* battle_session_teams,
                                   s32 rule_category) {
    s32 i;

    team_order_state->numberPicked = 0;
    team_order_state->unk_01 = 0;

    for (i = 0; i < 6; i++) {
        team_order_state->pokemonPicked[i] = 0;
        team_order_state->pokemonPendingPick[i] = 1;
    }

    if (rule_category != RULE_CATEGORY_ANYTHING_GOES) {
        if (rule_category != RULE_CATEGORY_VS_MEWTWO) {
            // clang-format off
            for (i = 3; i < 6; i++) { team_order_state->pokemonPicked[i] = -1; }
            // clang-format on
        }
    }

    if (battle_session_teams->tagTeamSize == 1) {
        team_order_state->firstTagTeamPartnerPort = battle_session_teams->tagTeamMember[0]->controllerPort;
        team_order_state->secondTagTeamPartnerPort = -1;
        team_order_state->unk_01 = (battle_session_teams->tagTeamMember[0]->isHuman & 2) != 0;
    } else if (battle_session_teams->tagTeamMember[0]->controllerPort <
               battle_session_teams->tagTeamMember[1]->controllerPort) {
        team_order_state->firstTagTeamPartnerPort = battle_session_teams->tagTeamMember[0]->controllerPort;
        team_order_state->secondTagTeamPartnerPort = battle_session_teams->tagTeamMember[1]->controllerPort;
    } else {
        team_order_state->firstTagTeamPartnerPort = battle_session_teams->tagTeamMember[1]->controllerPort;
        team_order_state->secondTagTeamPartnerPort = battle_session_teams->tagTeamMember[0]->controllerPort;
    }
}

// better name: BattleMenu_DrawPlayerPortIcon
void BattleMenu_DrawSidebarLeadIcon(s32 x, s32 y, TeamOrderState* state) {
    u8* sp2C;

    if (state->secondTagTeamPartnerPort == -1) {
        if (state->unk_01 != 0) {
            Gfx_DrawTextureRgba16(x, y, 36, 26, gComPlayerLabelIcon, 0x24, 0x200000);
        } else {
            Gfx_DrawTextureRgba16(x, y, 36, 26, sHumanPlayerLabelIcon[state->firstTagTeamPartnerPort], 36, 0x200000);
        }
    } else {
        switch (state->firstTagTeamPartnerPort) {
            case 0:
                sp2C = D_84803690[state->secondTagTeamPartnerPort];
                break;

            case 1:
                sp2C = D_84803698[state->secondTagTeamPartnerPort];
                break;

            case 2:
                sp2C = D_8480369C[0][state->secondTagTeamPartnerPort];
                break;
        }
        Gfx_DrawTextureRgba16(x, y, 36, 45, sp2C, 36, 0x200000);
    }
}

// better name: BattleMenu_DrawBottomPokeBallIcon
s32 BattleMenu_DrawSidebarRowTypeIcon(s32 x, s32 y, s8 port, s8 ordinal_of_pokemon) {
    s32 sp34;
    s32 sp30;

    if (port != 0) {
        if (port == -1) {
            Gfx_DrawTextureRgba16(x, y, 0x20, 0x14, D_3005680, 0x20, 0x200000);
        } else {
            sp34 = (port >> 4) & 0xF;
            sp30 = port & 0xF;

            if (ordinal_of_pokemon > 0) {
                if (sp30 < 3) {
                    sp30++;
                }
            } else if (sp30 > 0) {
                sp30--;
            }

            if (sp30 > 0) {
                Gfx_DrawTextureRgba16(x + 6, y, 24, 20, D_8480369C[sp34][sp30], 0x18, 0x200000);
                port = (sp34 * 0x10) | sp30;
            } else {
                port = 0;
            }
        }
    }
    return port;
}

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
        BattleMenu_DrawSidebarLeadIcon(45, 33, &sTeamOrderStates[0]);
    } else {
        BattleMenu_DrawSidebarLeadIcon(45, 29, &sTeamOrderStates[0]);
    }

    poke_ball_y = y + 118;
    for (i = 0; i < 6; i++) {
        state->pokemonPicked[i] = BattleMenu_DrawSidebarRowTypeIcon(45, poke_ball_y - i * 23, state->pokemonPicked[i],
                                                                    state->pokemonPendingPick[i]);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

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
        BattleMenu_DrawSidebarLeadIcon(45, 393, &sTeamOrderStates[1]);
    } else {
        BattleMenu_DrawSidebarLeadIcon(45, 380, &sTeamOrderStates[1]);
    }

    poke_ball_y = y + 12;
    for (i = 0; i < 6; i++) {
        state->pokemonPicked[i] = BattleMenu_DrawSidebarRowTypeIcon(45, poke_ball_y + i * 23, state->pokemonPicked[i],
                                                                    state->pokemonPendingPick[i]);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

// Better name: BattleMenu_RegisterPokemonSelection
void BattleMenu_AppendOrderSlotIcon(s32 side, s32 pokemon_slot_index) {
    TeamOrderState* state = &sTeamOrderStates[side];

    state->pokemonPicked[state->numberPicked] =
        ((pokemon_slot_index + 1) << 4) | (state->pokemonPicked[state->numberPicked] & 0xF);
    state->pokemonPendingPick[state->numberPicked] = 1;
    state->numberPicked++;
}

void BattleMenu_ResetOrderSlotIcons(s32 arg0) {
    TeamOrderState* state = &sTeamOrderStates[arg0];
    s32 i;

    for (i = 0; i < 6; i++) {
        state->pokemonPendingPick[i] = -1;
    }
    state->numberPicked = 0;
}

void BattleMenu_DrawBorderedPanel(s16 x, s16 y, s16 width, s16 height, s16 port) {
    Ui_DrawBorderedPanelStyleB(x, y, width, height, sBorderPortColors[port]);
}

void BattleMenu_DrawOrderSelectPanel_PartyGrid(BattleOrderInputState* input_state) {
    s32 i;
    s16 x;
    s16 y;
    s32 pokemon_x;
    s32 pokemon_y;
    s32 is_slot_selected;

    x = input_state->x;
    y = input_state->y;

    BattleMenu_DrawBorderedPanel(x, y, 471, 94, input_state->port);

    gSPDisplayList(gDisplayListHead++, D_8006F4E0);

    Gfx_DrawTextureRgba16(x + 7, y + 11, 26, 26, &D_30005B0, 28, 0x200000);
    Gfx_DrawTextureRgba16(x + 161, y + 11, 26, 26, &D_3001110, 28, 0x200000);
    Gfx_DrawTextureRgba16(x + 315, y + 11, 26, 26, &D_3001C70, 28, 0x200000);
    Gfx_DrawTextureRgba16(x + 7, y + 53, 26, 26, D_3000000, 28, 0x200000);
    Gfx_DrawTextureRgba16(x + 161, y + 53, 26, 26, &D_3000B60, 28, 0x200000);
    Gfx_DrawTextureRgba16(x + 315, y + 53, 26, 26, &D_30016C0, 28, 0x200000);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    Gfx_DrawTextureRgba16(x + 35, y + 6, 40, 40, input_state->unk_0020[0], 40, 0);
    Gfx_DrawTextureRgba16(x + 189, y + 6, 40, 40, input_state->unk0CA0, 40, 0);
    Gfx_DrawTextureRgba16(x + 343, y + 6, 40, 40, &input_state->unk0CA0[3200], 40, 0);
    Gfx_DrawTextureRgba16(x + 35, y + 48, 40, 40, &input_state->unk0CA0[6400], 40, 0);
    Gfx_DrawTextureRgba16(x + 189, y + 48, 40, 40, &input_state->unk0CA0[9600], 40, 0);
    Gfx_DrawTextureRgba16(x + 343, y + 48, 40, 40, &input_state->unk0CA0[12800], 40, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    Font_BeginTranslucentTextRendering();
    Font_SetActive(4, 0);

    for (i = 0; i < input_state->rosterSize; i++) {
        is_slot_selected = BattleMenu_IsSlotAlreadySelected(input_state, i);
        if ((input_state->rButtonHeld != 0) && (is_slot_selected != 0)) {
            Gfx_SetEnvColor(0xFF, 0xFF, 0, 0xFF); // #FFFF00
        }
        pokemon_x = x + ((i % 3) * 0x9A) + 0x4C;
        pokemon_y = y + ((i / 3) * 0x2A) + 8;
        Font_Printf(pokemon_x, pokemon_y, input_state->pokemon[i].nickname);
        if ((input_state->rButtonHeld != 0) && (is_slot_selected != 0)) {
            Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #FFFFFF
        }
        Font_Printf(pokemon_x, pokemon_y + 20, "%s%d", Text_GetString(NULL, 0, sPokemonDetailText, 0x15),
                    input_state->pokemon[i].level);
    }

    Font_EndTexturedTextRendering();

    if (input_state->confirmCursorMovingSubstep > 0) {
        if (input_state->isHuman == 0) {
            Gfx_DrawRgba16ImageRegion(D_84803784, 0xC8, 0x81, 0x15F, 0x53);
        } else {
            Gfx_DrawRgba16ImageRegion(D_84803784, 0x50, 0xF0, 0x15F, 0x53);
        }
        input_state->confirmCursorMovingSubstep -= 1;
    }
}

void BattleMenu_DrawCursorArrow(s16 arg0, s16 arg1, s16 arg2) {
    if (D_84816458[arg2] < 6) {
        arg0 += D_84816458[arg2];
    } else {
        arg0 = (arg0 - D_84816458[arg2]) + 0xC;
    }
    Ui_DrawTextureMarker(arg0, arg1);
    D_84816458[arg2] = (D_84816458[arg2] + 1) % 12;
}

void BattleMenu_DrawOrderSelectPanel_ConfirmPrompt(BattleOrderInputState* input_state) {
    s32 sp38;
    s16 x;
    s16 y;

    if (input_state->tagTeamPartnerPort == -1) {
        if (input_state->isHuman == 0) {
            x = 200;
            y = 129;
        } else {
            x = 80;
            y = 240;
        }

        BattleMenu_DrawOrderSelectPanel_PartyGrid(input_state);
        BattleMenu_DrawBorderedPanel(x, y, 351, 83, input_state->port);

        if (input_state->confirmCursorSelection == 0) {
            BattleMenu_DrawCursorArrow(x + 38, y + 48, input_state->unk_0000);
        } else {
            BattleMenu_DrawCursorArrow(x + 0x86, y + 48, input_state->unk_0000);
        }

        Font_BeginTranslucentTextRendering();
        Font_SetActive(0x10, 0);
        Font_Printf(x + 0x1F, y + 0xB, Text_GetString(NULL, 0, sTeamSelectText, 4));
        Font_Printf(x + 0x4F, y + 0x30, Text_GetString(NULL, 0, sTeamSelectText, 5));
        Font_Printf(x + 0xAF, y + 0x30, Text_GetString(NULL, 0, sTeamSelectText, 6));
        Font_EndTexturedTextRendering();
        input_state->confirmCursorMovingSubstep = 2;
    } else {
        x = input_state->x;
        y = input_state->y;

        BattleMenu_DrawOrderSelectPanel_PartyGrid(input_state);
        Gfx_FillRectWithDisplayState(x + 5, y + 5, 0x1CD, 0x54, 0, 0, 0, 0x80);
        if (input_state->confirmCursorSelection == 0) {
            BattleMenu_DrawCursorArrow(x + 0x5C, y + 0x3C, input_state->unk_0000);
        } else {
            BattleMenu_DrawCursorArrow(x + 0xBC, y + 0x3C, input_state->unk_0000);
        }
        Font_BeginTranslucentTextRendering();
        Font_SetActive(0x10, 0);
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
        Font_Printf(x + 0x50, y + 0x14, Text_GetString(NULL, 0, sTeamSelectText, 4));
        Font_Printf(x + 0x80, y + 0x3C, Text_GetString(NULL, 0, sTeamSelectText, 5));
        Font_Printf(x + 0xE0, y + 0x3C, Text_GetString(NULL, 0, sTeamSelectText, 6));
        Font_EndTexturedTextRendering();
    }
}

void BattleMenu_DrawOrderSelectPanel_LevelCapNotice(BattleOrderInputState* arg0) {
    char sp38[0x100];
    s32 sp34;
    s32 sp30;
    s32 sp2C;

    switch (sSessionContext->ruleCategory) {
        case 3:
            sp2C = 0x9B;
            break;

        case 4:
            sp2C = 0x50;
            break;

        case 5:
            sp2C = 0x32;
            break;
    }

    if (arg0->tagTeamPartnerPort == -1) {
        // clang-format off
        if (arg0->isHuman == 0) {
            sp34 = 0xC8; sp30 = 0x81;
        } else {
            sp34 = 0x50; sp30 = 0xF0;
        }
        // clang-format on

        BattleMenu_DrawBorderedPanel(sp34, sp30, 0x15F, 0x53, arg0->port);
        Font_BeginTranslucentTextRendering();
        Font_SetActive(0x10, 0);
        Gfx_SetEnvColor(0xFF, 0x63, 0x63, 0xFF);
        Font_SetLineHeight(0x25);
        Text_SetNumberToken(3, sp2C);
        Font_Printf(sp34 + 0x10, sp30 + 0xB, Text_GetString(sp38, sizeof(sp38), sTeamSelectText, 7));
        Font_EndTexturedTextRendering();
        arg0->confirmCursorMovingSubstep = 2;
    } else {
        sp34 = arg0->x;
        sp30 = arg0->y;

        BattleMenu_DrawOrderSelectPanel_PartyGrid(arg0);
        Gfx_FillRectWithDisplayState(sp34 + 5, sp30 + 5, 0x1CD, 0x54, 0, 0, 0, 0x80);
        Font_BeginTranslucentTextRendering();
        Font_SetActive(0x10, 0);
        Gfx_SetEnvColor(0xFF, 0x63, 0x63, 0xFF);
        Font_SetLineHeight(0x28);
        Text_SetNumberToken(3, sp2C);
        Font_Printf(sp34 + 0x50, sp30 + 0x14, Text_GetString(NULL, 0, sTeamSelectText, 7));
        Font_EndTexturedTextRendering();
    }
}

void BattleMenu_DrawOrderSelectPanel_Ready(BattleOrderInputState* arg0) {
    s32 sp3C;
    s32 sp38;
    s32 sp34;
    char* temp_v0;

    if (arg0->tagTeamPartnerPort == -1) {
        // clang-format off
        if (arg0->isHuman == 0) {
            sp3C = 0xC8; sp38 = 0x81;
        } else {
            sp3C = 0x50; sp38 = 0xF0;
        }
        // clang-format on
        arg0->confirmCursorMovingSubstep = 0;
        BattleMenu_DrawOrderSelectPanel_PartyGrid(arg0);
        BattleMenu_DrawBorderedPanel(sp3C, sp38, 0x15F, 0x53, arg0->port);
        Font_BeginTranslucentTextRendering();
        Font_SetActive(0x20, 0);
        Font_EnableTwoCycleTexturing();
        Gfx_SetPrimColor(0x1C, 0x74, 0xFF, 0xFF);
        temp_v0 = Text_GetString(NULL, 0, sTeamSelectText, 8);
        Font_Printf((sp3C - (Font_MeasureTextExtent(0x20, 0, temp_v0) / 2)) + 0xAF, sp38 + 0x19, temp_v0);
        Font_DisableTwoCycleTexturing();
        Font_EndTexturedTextRendering();
    } else {
        sp3C = arg0->x;
        sp38 = arg0->y;

        if (arg0->unk_0008 < 0x10) {
            sp34 = arg0->unk_0008 * 0x10;
        } else if (arg0->unk_0008 < 0x23) {
            sp34 = 0xFF;
        } else if (arg0->unk_0008 < 0x32) {
            sp34 = (-arg0->unk_0008 * 0x10) + 0x320;
        } else {
            sp34 = 0;
        }

        arg0->unk_0008++;
        if (arg0->unk_0008 >= 0x3D) {
            arg0->unk_0008 = 0;
        }

        BattleMenu_DrawOrderSelectPanel_PartyGrid(arg0);
        Gfx_FillRectWithDisplayState(sp3C + 5, sp38 + 5, 0x1CD, 0x54, 0, 0, 0, 0x80);
        Font_BeginTranslucentTextRendering();
        Font_SetActive(0x20, 0);
        Font_EnableTwoCycleTexturing();
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, sp34);
        Gfx_SetPrimColor(0x1C, 0x74, 0xFF, sp34);
        temp_v0 = Text_GetString(NULL, 0, sTeamSelectText, 8);
        Font_Printf((sp3C - (Font_MeasureTextExtent(0x20, 0, temp_v0) / 2)) + 0xEB, sp38 + 0x25, temp_v0);
        Font_DisableTwoCycleTexturing();
        Font_EndTexturedTextRendering();
    }
}

void BattleMenu_DrawOrderSelectPanel_MonDetail(BattleOrderInputState* arg0) {
    s32 i;
    s16 temp_fp;
    s8 temp_s1;
    unk_D_848035FC* temp_v1;
    s32 idx;
    s32 sp7C;
    BattleMon* sp78;

    temp_fp = arg0->x;
    temp_s1 = arg0->summaryDisplaySlot;
    sp7C = arg0->y;
    sp78 = &arg0->pokemon[temp_s1];

    BattleMenu_DrawBorderedPanel(temp_fp, sp7C, 0x1D7, 0x5E, arg0->port);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    Gfx_DrawTextureRgba16(temp_fp + 8, sp7C + 7, 0x28, 0x28, &arg0->unk_0020[temp_s1], 0x28, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    Font_BeginTranslucentTextRendering();
    Font_SetActive(4, 0);
    Font_Printf(temp_fp + 0x32, sp7C + 7, arg0->pokemon[temp_s1].nickname);
    Font_Printf(temp_fp + 0x32, sp7C + 0x1B, "%s", Text_GetString(NULL, 0, sPokemonDetailText, 8));
    Font_Printf(temp_fp + 0xBE, sp7C + 7, Text_GetString(NULL, 0, sTeamSelectText, 9));
    Font_Printf(temp_fp + 0xBE, sp7C + 0x1B, Text_GetString(NULL, 0, sTeamSelectText, 0xA));
    Font_Printf(temp_fp + 0xBE, sp7C + 0x2F, Text_GetString(NULL, 0, sTeamSelectText, 0xB));
    Font_Printf(temp_fp + 0xBE, sp7C + 0x43, Text_GetString(NULL, 0, sTeamSelectText, 0xC));
    Font_Printf(temp_fp + 0x32, sp7C + 0x2F, Text_GetString(NULL, 0, sTeamSelectText, 0xD));

    if (sp78->unk_06 != sp78->unk_07) {
        Font_Printf(temp_fp + 0x32, sp7C + 0x43, Text_GetString(NULL, 0, sTeamSelectText, 0xE));
    }

    idx = PokemonType_ToDisplayIndex(sp78->unk_06);
    Gfx_SetEnvColor(D_848035FC[idx].unk_00.r, D_848035FC[idx].unk_00.g, D_848035FC[idx].unk_00.b, 0xFF);
    Font_Printf(temp_fp + 0x7A, sp7C + 0x2F,
                Text_GetString(NULL, 0, sTypeNameTable, PokemonType_ToDisplayIndex(sp78->unk_06)));

    if (sp78->unk_06 != sp78->unk_07) {
        idx = PokemonType_ToDisplayIndex(sp78->unk_07);
        Gfx_SetEnvColor(D_848035FC[idx].unk_00.r, D_848035FC[idx].unk_00.g, D_848035FC[idx].unk_00.b, 0xFF);
        Font_Printf(temp_fp + 0x7A, sp7C + 0x43,
                    Text_GetString(NULL, 0, sTypeNameTable, PokemonType_ToDisplayIndex(sp78->unk_07)));
    }

    Font_SetActive(4, 0);
    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
    Font_Printf(temp_fp + 0x8C, sp7C + 7, "%s%d", Text_GetString(NULL, 0, sPokemonDetailText, 0x15),
                arg0->pokemon[temp_s1].level);
    Font_Printf(temp_fp + 0x52, sp7C + 0x1B, "%3d", sp78->maxHp);
    Font_Printf(temp_fp + 0xFA, sp7C + 7, "%3d", sp78->unk_28);
    Font_Printf(temp_fp + 0xFA, sp7C + 0x1B, "%3d", sp78->unk_2A);
    Font_Printf(temp_fp + 0xFA, sp7C + 0x2F, "%3d", sp78->unk_2C);
    Font_Printf(temp_fp + 0xFA, sp7C + 0x43, "%3d", sp78->unk_2E);

    for (i = 0; i < 4; i++) {
        if (sp78->moves[i] == 0) {
            break;
        }
        temp_v1 = &D_848035FC[gMoveDisplayInfo[sp78->moves[i] - 1].unk_01];
        Font_SetActive(4, 0);
        Gfx_SetEnvColor(temp_v1->unk_00.r, temp_v1->unk_00.g, temp_v1->unk_00.b, 0xFF);
        Font_Printf(temp_fp + 0x14C, (i * 0x14) + sp7C + 7,
                    Text_GetString(NULL, 0, sMoveNameTable, sp78->moves[i] - 1));
        Font_SetActive(4, 0);
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
        Font_Printf(temp_fp + 0x1B3, (i * 0x14) + sp7C + 7, "%2d", sp78->pp[i] & 0x3F);
    }

    Font_EndTexturedTextRendering();

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    for (i = 0; i < 4; i++) {
        if (sp78->moves[i] == 0) {
            break;
        }
        Gfx_DrawTextureRgba16(temp_fp + 0x136, (i * 0x14) + sp7C + 7, 0x14, 0x14,
                              D_848035FC[gMoveDisplayInfo[sp78->moves[i] - 1].unk_01].unk_04, 0x14, 0);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    if (arg0->confirmCursorMovingSubstep > 0) {
        if (arg0->isHuman == 0) {
            Gfx_DrawRgba16ImageRegion(D_84803784, 0xC8, 0x81, 0x15F, 0x53);
        } else {
            Gfx_DrawRgba16ImageRegion(D_84803784, 0x50, 0xF0, 0x15F, 0x53);
        }
        arg0->confirmCursorMovingSubstep--;
    }
}

void BattleMenu_DrawConfirmButtonLabel(BattleOrderInputState* arg0) {
    s16 sp26;
    s16 sp24;

    if (arg0->isHuman == 0) {
        // clang-format off
        sp26 = 0x50; sp24 = 0x81;
        // clang-format on
    } else {
        sp26 = 0x1B1;
        sp24 = 0x123;
    }

    BattleMenu_DrawBorderedPanel(sp26, sp24, 0x76, 0x20, arg0->port);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(8, 0);
    Font_Printf((sp26 - (Font_MeasureTextExtent(8, 0, arg0->roster->trainerName) / 2)) + 0x3B, sp24 + 5,
                arg0->roster->trainerName);
    Font_EndTexturedTextRendering();
}

void BattleMenu_DrawOrderSelectPanel(BattleOrderInputState* arg0) {
    if ((D_84803798 != 0) || (arg0->confirmCursorMoving != 0)) {
        if ((D_84803798 != 0) && (arg0->tagTeamPartnerPort == -1)) {
            BattleMenu_DrawConfirmButtonLabel(arg0);
        }

        switch (arg0->doneSelecting) {
            case 0:
                BattleMenu_DrawOrderSelectPanel_PartyGrid(arg0);
                break;

            case 1:
                BattleMenu_DrawOrderSelectPanel_ConfirmPrompt(arg0);
                break;

            case 2:
                BattleMenu_DrawOrderSelectPanel_LevelCapNotice(arg0);
                break;

            case 3:
                BattleMenu_DrawOrderSelectPanel_Ready(arg0);
                break;

            case 4:
                BattleMenu_DrawOrderSelectPanel_MonDetail(arg0);
                break;
        }

        if (arg0->confirmCursorMoving != 0) {
            arg0->confirmCursorMoving--;
        }
    } else if (arg0->doneSelecting == 3) {
        BattleMenu_DrawOrderSelectPanel_Ready(arg0);
    }
}

void BattleMenu_UpdateComPanelAutoInput(void) {
    s32 i;
    s8 number_of_pokemon_to_pick;
    ComSelectionButtonPresses* com_button_presses;

    for (i = 0; i < 4; i++) {
        if ((sBattleOrderInputStates[i].roster != NULL) && (sBattleOrderInputStates[i].roster->isHuman & 2)) {
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
        D_84816428.buttonPressed = 0;
        if (D_84816450 == 0) {
            com_button_presses = sComButtonPressesByRosterSize[number_of_pokemon_to_pick];

            D_84816450 = com_button_presses[D_8481644C].delay;
            if (D_84816450 != 0) {
                D_84816428.buttonPressed = com_button_presses[D_8481644C].button;
                D_8481644C++;
            }
        }

        if (D_84816450 > 0) {
            D_84816450--;
        }
    }
}

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

s32 BattleMenu_PicksExceedLevelCap(BattleOrderInputState* input_state) {
    s32 i;
    s32 level_sum = 0;

    if (!(input_state->roster->isHuman & 2)) {
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

s32 BattleMenu_IsOrderSelectionComplete(BattleOrderInputState* arg0) {
    s8 var_v1;

    if ((sSessionContext->ruleCategory != 0) && (sSessionContext->ruleCategory != 8)) {
        var_v1 = 3;
    } else if (arg0->roster->isHuman & 2) {
        var_v1 = arg0->rosterSize;
    } else {
        var_v1 = arg0->rosterSize;
        if ((arg0->tagTeamPartnerPort != -1) && (var_v1 >= 4)) {
            var_v1 = 3;
        }
    }

    return arg0->numberOfPokemonPicked >= var_v1;
}

void BattleMenu_SelectOrderSlot(BattleOrderInputState* arg0, s8 arg1, s32 arg2) {
    if (arg1 >= arg0->rosterSize) {
        Audio_PlaySoundEffectById(SFX_MENU_ERROR);
        return;
    }

    if (arg2 != 0) {
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
        arg0->doneSelecting = 4;
        arg0->confirmCursorMoving = 2;
        arg0->summaryDisplaySlot = arg1;
        return;
    }

    if (BattleMenu_IsSlotAlreadySelected(arg0, arg1) != 0) {
        Audio_PlaySoundEffectById(SFX_MENU_ERROR);
        return;
    }

    arg0->pickedSlots[arg0->numberOfPokemonPicked] = arg1;
    arg0->numberOfPokemonPicked++;
    BattleMenu_AppendOrderSlotIcon(arg0->isHuman, arg0->port);

    if (arg0->rButtonHeld != 0) {
        arg0->confirmCursorMoving = 2;
    }

    if (BattleMenu_PicksExceedLevelCap(arg0) != 0) {
        Audio_PlaySoundEffectById(SFX_MENU_ERROR);
        arg0->doneSelecting = 2;
        arg0->confirmCursorMoving = 2;
        return;
    }

    Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
    if (BattleMenu_IsOrderSelectionComplete(arg0) != 0) {
        arg0->doneSelecting = 1;
        arg0->confirmCursorSelection = 0;
        arg0->confirmCursorMoving = 2;
    }
}

void BattleMenu_UndoLastOrderSelection(BattleOrderInputState* arg0) {
    BattleOrderInputState* temp_v0;

    if (arg0->numberOfPokemonPicked > 0) {
        arg0->numberOfPokemonPicked = 0;

        BattleMenu_ResetOrderSlotIcons(arg0->isHuman);

        if (arg0->rButtonHeld != 0) {
            arg0->confirmCursorMoving = 2;
        }

        Audio_PlaySoundEffectById(SFX_MENU_BACK);

        if (arg0->tagTeamPartnerPort != -1) {
            temp_v0 = &sBattleOrderInputStates[arg0->tagTeamPartnerPort];
            temp_v0->doneSelecting = 0;
            temp_v0->numberOfPokemonPicked = 0;
            temp_v0->rButtonHeld = 0;
            temp_v0->confirmCursorMoving = 2;
        }
    }
}

void BattleMenu_HandleOrderSelectPanel_PartyInput(Controller* arg0, BattleOrderInputState* arg1) {
    s32 temp_a2 = BTN_IS_DOWN(arg0, BTN_R);

    if (BTN_IS_PRESSED(arg0, BTN_R)) {
        // clang-format off
        arg1->rButtonHeld = 1; arg1->confirmCursorMoving = 2;
        // clang-format on
    }

    if (arg0->unk_0A & 0x10) {
        arg1->rButtonHeld = 0;
        arg1->confirmCursorMoving = 2;
    }

    if (BTN_IS_PRESSED(arg0, BTN_L)) {
        BattleMenu_UndoLastOrderSelection(arg1);
    } else if (BTN_IS_PRESSED(arg0, BTN_B)) {
        BattleMenu_SelectOrderSlot(arg1, 0, temp_a2);
    } else if (BTN_IS_PRESSED(arg0, BTN_CLEFT)) {
        BattleMenu_SelectOrderSlot(arg1, 1, temp_a2);
    } else if (BTN_IS_PRESSED(arg0, BTN_CUP)) {
        BattleMenu_SelectOrderSlot(arg1, 2, temp_a2);
    } else if (BTN_IS_PRESSED(arg0, BTN_A)) {
        BattleMenu_SelectOrderSlot(arg1, 3, temp_a2);
    } else if (BTN_IS_PRESSED(arg0, BTN_CDOWN)) {
        BattleMenu_SelectOrderSlot(arg1, 4, temp_a2);
    } else if (BTN_IS_PRESSED(arg0, BTN_CRIGHT)) {
        BattleMenu_SelectOrderSlot(arg1, 5, temp_a2);
    }
}

void BattleMenu_HandleOrderSelectPanel_ConfirmInput(Controller* arg0, BattleOrderInputState* arg1) {
    BattleOrderInputState* temp_v0_2;

    if (BTN_IS_PRESSED(arg0, BTN_R)) {
        arg1->rButtonHeld = 1;
        arg1->confirmCursorMoving = 2;
    }
    if (arg0->unk_0A & 0x10) {
        arg1->rButtonHeld = 0;
        arg1->confirmCursorMoving = 2;
    }

    if (BTN_IS_PRESSED(arg0, BTN_DLEFT | BTN_DRIGHT)) {
        arg1->confirmCursorMoving = 2;
        arg1->confirmCursorSelection ^= 1;
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
    } else if (BTN_IS_PRESSED(arg0, BTN_A)) {
        if (arg1->confirmCursorSelection == 0) {
            arg1->confirmCursorMoving = 2;
            arg1->doneSelecting = 3;
            arg1->unk_0008 = 0;
            Audio_PlaySoundEffectById(9);
        } else {
            arg1->confirmCursorMoving = 2;
            arg1->doneSelecting = 0;
            arg1->numberOfPokemonPicked = 0;
            arg1->rButtonHeld = 0;
            BattleMenu_ResetOrderSlotIcons(arg1->isHuman);
            Audio_PlaySoundEffectById(SFX_MENU_BACK);
            if (arg1->tagTeamPartnerPort != -1) {
                temp_v0_2 = &sBattleOrderInputStates[arg1->tagTeamPartnerPort];
                temp_v0_2->doneSelecting = 0;
                temp_v0_2->numberOfPokemonPicked = 0;
                temp_v0_2->rButtonHeld = 0;
                temp_v0_2->confirmCursorMoving = 2;
            }
        }
    }
    arg1->confirmCursorMoving = 2;
}

void BattleMenu_HandleOrderSelectPanel_LevelCapNoticeInput(Controller* arg0, BattleOrderInputState* arg1) {
    if (BTN_IS_PRESSED(arg0, BTN_A | BTN_B)) {
        arg1->doneSelecting = 0;
        arg1->numberOfPokemonPicked = 0;
        arg1->rButtonHeld = 0;
        arg1->confirmCursorMoving = 2;
        BattleMenu_ResetOrderSlotIcons(arg1->isHuman);
        Audio_PlaySoundEffectById(SFX_MENU_BACK);
    }
}

void BattleMenu_HandleOrderSelectPanel_MonDetailInput(Controller* arg0, BattleOrderInputState* arg1) {
    s32 var_v0 = 0;

    switch (arg1->summaryDisplaySlot) {
        case 0:
            if (arg0->unk_0A & 0x4000) {
                var_v0 = 1;
            }
            break;

        case 1:
            if (arg0->unk_0A & 2) {
                var_v0 = 1;
            }
            break;

        case 2:
            if (arg0->unk_0A & 8) {
                var_v0 = 1;
            }
            break;

        case 3:
            if (arg0->unk_0A & 0x8000) {
                var_v0 = 1;
            }
            break;

        case 4:
            if (arg0->unk_0A & 4) {
                var_v0 = 1;
            }
            break;

        case 5:
            if (arg0->unk_0A & 1) {
                var_v0 = 1;
            }
            break;
    }

    if (var_v0 != 0) {
        if (!(arg0->buttonDown & 0x10)) {
            arg1->rButtonHeld = 0;
        }
        arg1->doneSelecting = 0;
        arg1->confirmCursorMoving = 2;
    }
}

s32 BattleMenu_UpdateOrderSelectPanel(s32 arg0, BattleOrderInputState* arg1) {
    Controller* var_a0;
    s32 sp18 = 0;

    if (arg1->roster->isHuman & 2) {
        var_a0 = &D_84816428;
    } else {
        var_a0 = &gControllers[arg1->port];
    }

    if (arg0 == 0) {
        switch (arg1->doneSelecting) {
            case 0:
                BattleMenu_HandleOrderSelectPanel_PartyInput(var_a0, arg1);
                break;

            case 1:
                BattleMenu_HandleOrderSelectPanel_ConfirmInput(var_a0, arg1);
                break;

            case 2:
                BattleMenu_HandleOrderSelectPanel_LevelCapNoticeInput(var_a0, arg1);
                break;

            case 3:
                sp18 = 1;
                break;

            case 4:
                BattleMenu_HandleOrderSelectPanel_MonDetailInput(var_a0, arg1);
                break;
        }
    }
    return sp18;
}

s32 BattleMenu_UpdateOrderSelect(s32 arg0) {
    UNUSED s32 pad;
    s32 i;
    s32 var_s2;
    s32 sp30;

    var_s2 = 0;
    sp30 = 0;

    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
    BattleMenu_UpdateComPanelAutoInput();
    GeoRender_AdvanceFrameCounter();
    BgStage_DrawFrame();

    if (D_84803798 != 0) {
        BattleMenu_DrawBottomActionButtons();
    }

    for (i = 0; i < D_8480379C; i++) {
        var_s2 += BattleMenu_UpdateOrderSelectPanel(arg0, &sBattleOrderInputStates[i]);
        BattleMenu_DrawOrderSelectPanel(&sBattleOrderInputStates[i]);
    }

    BattleMenu_DrawTeamOrderSidebarTeam0();
    BattleMenu_DrawTeamOrderSidebarTeam1();
    BattleMenu_DrawScrollingRulesText();

    if (arg0 == 0) {
        if (var_s2 == D_8480379C) {
            sp30 = 1;
            StageContext_SetClearColor(0xFFFF);
            Audio_StopMusic(0x10);
        }

        if (D_84803798 > 0) {
            D_84803798 -= 1;
        }
    } else {
        D_84803798 = 2;
    }
    return sp30;
}

void BattleMenu_RearrangeLeadPokemon(BattleSessionTeams* arg0, TeamOrderState* arg1) {
    TeamRoster* temp_v0;
    s32 tmp;

    if (arg0->tagTeamSize == 2) {
        tmp = ((arg1->pokemonPicked[0] >> 4) & 0xF) - 1;
        if (tmp == arg0->tagTeamMember[1]->controllerPort) {
            temp_v0 = arg0->tagTeamMember[0];
            arg0->tagTeamMember[0] = arg0->tagTeamMember[1];
            arg0->tagTeamMember[1] = temp_v0;
        }
    }
}

void BattleMenu_FillDefaultOrder(s8* arg0, s32 arg1) {
    s32 i;
    s32 j;

    for (i = 0; i < 6; i++) {
        for (j = 0; j < arg1; j++) {
            if (i == arg0[j]) {
                break;
            }
        }

        if (j == arg1) {
            arg0[arg1] = i;
            arg1 += 1;
        }
    }
}

void BattleMenu_CommitOrderSelection(void) {
    s32 i;
    s32 var_s4;
    TeamRoster* temp_s1;
    BattleOrderInputState* var_s2;

    var_s2 = sBattleOrderInputStates;
    var_s4 = D_8480379C;

    while (var_s4-- > 0) {
        temp_s1 = var_s2->roster;
        if (temp_s1->isHuman & 2) {
            temp_s1->numberOfPokemonBrought = temp_s1->rosterInfo->numberOfPokemonCanBePicked;
            _bcopy(temp_s1->rosterInfo->unk_028, temp_s1->pokemonPicked, 0x1F8);
        } else {
            BattleMenu_FillDefaultOrder(var_s2->pickedSlots, var_s2->numberOfPokemonPicked);
            temp_s1->numberOfPokemonBrought = var_s2->numberOfPokemonPicked;

            for (i = 0; i < temp_s1->rosterInfo->numberOfPokemonCanBePicked; i++) {
                temp_s1->pokemonPicked[i] = var_s2->pokemon[var_s2->pickedSlots[i]];
            }
        }
        var_s2++;
    }

    BattleMenu_RearrangeLeadPokemon(&sSessionContext->battleSides[0], &sTeamOrderStates[0]);
    BattleMenu_RearrangeLeadPokemon(&sSessionContext->battleSides[1], &sTeamOrderStates[1]);
}

void BattleMenu_AddOrderSelectPanel(s16 arg0, s16 arg1, TeamRoster* arg2, s32 arg3, s32 arg4) {
    s32 i;
    BattleOrderInputState* temp_v1;
    BattleMon* temp_s0;

    temp_v1 = &sBattleOrderInputStates[D_8480379C];
    temp_v1->unk_0000 = D_8480379C;
    temp_v1->port = arg2->controllerPort;
    temp_v1->tagTeamPartnerPort = arg4;
    temp_v1->isHuman = arg3;
    temp_v1->doneSelecting = 0;
    temp_v1->numberOfPokemonPicked = 0;
    temp_v1->confirmCursorMoving = 2;
    temp_v1->confirmCursorMovingSubstep = 0;
    temp_v1->rButtonHeld = 0;
    temp_v1->confirmCursorSelection = 0;
    temp_v1->x = arg0;
    temp_v1->y = arg1;
    temp_v1->rosterSize = arg2->rosterInfo->numberOfPokemonCanBePicked;
    temp_v1->pokemon = arg2->rosterInfo->unk_028;
    temp_v1->roster = arg2;

    for (i = 0; i < arg2->rosterInfo->numberOfPokemonCanBePicked; i++) {
        temp_s0 = &arg2->rosterInfo->unk_028[i];
        Pokemon_PrepareBattleMon(temp_s0);
        temp_s0->unk_06 = gSpeciesData[temp_s0->unk_00.unk_00 - 1].type1;
        temp_s0->unk_07 = gSpeciesData[temp_s0->unk_00.unk_00 - 1].type2;
        PokeIcon_LoadModelTextureForMon(temp_v1->unk_0020[i], 0, temp_s0);
    }

    D_8480379C++;
}

void BattleMenu_InitOrderSelectPanels(SessionContext* arg0) {
    BattleSessionTeams* temp_s0 = &arg0->battleSides[0];

    sSessionContext = arg0;
    D_8480379C = 0;
    D_84803798 = 2;
    sScrollingTextProgress = 0;
    D_8481644C = 0;
    D_84816450 = 0;
    D_84816428.buttonPressed = 0;

    BattleMenu_InitTeamOrderState(&sTeamOrderStates[0], &sSessionContext->battleSides[0],
                                  sSessionContext->ruleCategory);
    BattleMenu_InitTeamOrderState(&sTeamOrderStates[1], &sSessionContext->battleSides[1],
                                  sSessionContext->ruleCategory);

    if (temp_s0->tagTeamSize == 1) {
        BattleMenu_AddOrderSelectPanel(0x50, 0x21, temp_s0->tagTeamMember[0], 0, -1);
    } else if (temp_s0->tagTeamMember[0]->controllerPort < temp_s0->tagTeamMember[1]->controllerPort) {
        BattleMenu_AddOrderSelectPanel(0x50, 0x21, temp_s0->tagTeamMember[0], 0, D_8480379C + 1);
        BattleMenu_AddOrderSelectPanel(0x50, 0x81, temp_s0->tagTeamMember[1], 0, D_8480379C - 1);
    } else {
        BattleMenu_AddOrderSelectPanel(0x50, 0x21, temp_s0->tagTeamMember[1], 0, D_8480379C + 1);
        BattleMenu_AddOrderSelectPanel(0x50, 0x81, temp_s0->tagTeamMember[0], 0, D_8480379C - 1);
    }

    temp_s0++;
    if (temp_s0->tagTeamSize == 1) {
        BattleMenu_AddOrderSelectPanel(0x50, 0x146, temp_s0->tagTeamMember[0], 1, -1);
    } else if (temp_s0->tagTeamMember[0]->controllerPort < temp_s0->tagTeamMember[1]->controllerPort) {
        BattleMenu_AddOrderSelectPanel(0x50, 0xE6, temp_s0->tagTeamMember[0], 1, D_8480379C + 1);
        BattleMenu_AddOrderSelectPanel(0x50, 0x146, temp_s0->tagTeamMember[1], 1, D_8480379C - 1);
    } else {
        BattleMenu_AddOrderSelectPanel(0x50, 0xE6, temp_s0->tagTeamMember[1], 1, D_8480379C + 1);
        BattleMenu_AddOrderSelectPanel(0x50, 0x146, temp_s0->tagTeamMember[0], 1, D_8480379C - 1);
    }
}

#if defined(VERSION_US_REV2)
// asm-processor requires this import on one physical line.
// clang-format off
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/battle_order_select/battle_order_select/BattleMenu_RunOrderSelectScreen.s")
// clang-format on
#else
s32 BattleMenu_RunOrderSelectScreen(s32 arg0, s32 arg1) {
    StageContext* sp24;

    main_pool_push_state('MSEL');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    sp24 = StageContext_Allocate(1, 1, 2, 0, 2, 1);
    Text_InitStringTables();
    Font_Init(0x3C, 0);
    PokeIcon_OpenModelArchive();
    D_84803780 = ASSET_LOAD2(backgrounds, 1, 1);
    D_84803784 = BinArchive_GetFile(D_84803780, 5);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_3000000, order_select_ui, 0);
    ASSET_LOAD(D_2000000, common_menu2_ui, 0);

    sMoveNameTable = Text_GetStringTable(0x25);
    sTypeNameTable = Text_GetStringTable(0x26);
    sTeamSelectText = Text_GetStringTable(0x12);
    sPokemonDetailText = Text_GetStringTable(6);

    BattleMenu_InitOrderSelectPanels(&gSessionContext);
    Audio_PlayMusicIfChanged(7);
    StageContext_Activate(sp24);
    BgStage_WaitForCondition(BattleMenu_UpdateOrderSelect, 8, 8);
    StageContext_Deactivate();
    BattleMenu_CommitOrderSelection();
    Font_Free();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('MSEL');
    return 0;
}
#endif
