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
static void* sPartyGridTextureOffset;
static char** sMoveNameTable;
static char** sTypeNameTable;
static char** sTeamSelectText;
static char** sPokemonDetailText;
static s16 D_84803798;
static s16 sScrollingTextProgress;
static s32 D_8480379C;
static BattleOrderInputState sBattleOrderInputStates[4];
static SessionContext* sSessionContext;
static Controller sComController;
static s32 sComNextButtonPress;
static s32 sComSelectionTimer;
static s16 sCursorOscillationPhases[4];

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
static PokemonTypeString sPokemonTypeStrings[] = {
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
    Gfx_DrawTiledRgba16Image(sPartyGridTextureOffset);

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
        team_order_state->unk_01 = (battle_session_teams->tagTeamMember[0]->isHuman & 0x2) != 0;
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
    u8* texture;

    if (state->secondTagTeamPartnerPort == -1) {
        if (state->unk_01 != 0) {
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

// better name: BattleMenu_DrawBottomPokeBallIcon
s32 BattleMenu_DrawSidebarRowTypeIcon(s32 x, s32 y, s8 port, s8 ordinal_of_pokemon) {
    s32 sp34;
    s32 sp30;

    if (port != 0) {
        if (port == -1) {
            Gfx_DrawTextureRgba16(x, y, 32, 20, D_3005680, 32, 0x200000);
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
                Gfx_DrawTextureRgba16(x + 6, y, 24, 20, D_8480369C[sp34][sp30], 24, 0x200000);
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

void BattleMenu_ResetOrderSlotIcons(s32 side) {
    TeamOrderState* state = &sTeamOrderStates[side];
    s32 i;

    for (i = 0; i < 6; i++) {
        state->pokemonPendingPick[i] = -1;
    }
    state->numberPicked = 0;
}

// better name: BattleMenu_DrawPanelBorder
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

    if (input_state->summaryDisplayProgress > 0) {
        if (input_state->isHuman == 0) {
            Gfx_DrawRgba16ImageRegion(sPartyGridTextureOffset, 200, 129, 351, 83);
        } else {
            Gfx_DrawRgba16ImageRegion(sPartyGridTextureOffset, 80, 240, 351, 83);
        }
        input_state->summaryDisplayProgress -= 1;
    }
}

void BattleMenu_DrawCursorArrow(s16 x, s16 y, s16 oscillation_index) {
    if (sCursorOscillationPhases[oscillation_index] < 6) {
        x += sCursorOscillationPhases[oscillation_index];
    } else {
        x = (x - sCursorOscillationPhases[oscillation_index]) + 12;
    }
    Ui_DrawTextureMarker(x, y);
    sCursorOscillationPhases[oscillation_index] = (sCursorOscillationPhases[oscillation_index] + 1) % 12;
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
            BattleMenu_DrawCursorArrow(x + 134, y + 48, input_state->unk_0000);
        }

        Font_BeginTranslucentTextRendering();
        Font_SetActive(0x10, 0);
        Font_Printf(x + 31, y + 11, Text_GetString(NULL, 0, sTeamSelectText, 4));
        Font_Printf(x + 79, y + 48, Text_GetString(NULL, 0, sTeamSelectText, 5));
        Font_Printf(x + 175, y + 48, Text_GetString(NULL, 0, sTeamSelectText, 6));
        Font_EndTexturedTextRendering();
        input_state->summaryDisplayProgress = 2;
    } else {
        x = input_state->x;
        y = input_state->y;

        BattleMenu_DrawOrderSelectPanel_PartyGrid(input_state);
        Gfx_FillRectWithDisplayState(x + 5, y + 5, 461, 84, 0, 0, 0, 0x80);
        if (input_state->confirmCursorSelection == 0) {
            BattleMenu_DrawCursorArrow(x + 92, y + 60, input_state->unk_0000);
        } else {
            BattleMenu_DrawCursorArrow(x + 188, y + 60, input_state->unk_0000);
        }
        Font_BeginTranslucentTextRendering();
        Font_SetActive(0x10, 0);
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
        Font_Printf(x + 80, y + 20, Text_GetString(NULL, 0, sTeamSelectText, 4));
        Font_Printf(x + 128, y + 60, Text_GetString(NULL, 0, sTeamSelectText, 5));
        Font_Printf(x + 224, y + 60, Text_GetString(NULL, 0, sTeamSelectText, 6));
        Font_EndTexturedTextRendering();
    }
}

void BattleMenu_DrawOrderSelectPanel_LevelCapNotice(BattleOrderInputState* arg0) {
    char sp38[256];
    s32 sp34;
    s32 sp30;
    s32 sp2C;

    switch (sSessionContext->ruleCategory) {
        case 3:
            sp2C = 155;
            break;

        case 4:
            sp2C = 80;
            break;

        case 5:
            sp2C = 50;
            break;
    }

    if (arg0->tagTeamPartnerPort == -1) {
        // clang-format off
        if (arg0->isHuman == 0) {
            sp34 = 200; sp30 = 129;
        } else {
            sp34 = 80; sp30 = 240;
        }
        // clang-format on

        BattleMenu_DrawBorderedPanel(sp34, sp30, 351, 83, arg0->port);
        Font_BeginTranslucentTextRendering();
        Font_SetActive(0x10, 0);
        Gfx_SetEnvColor(0xFF, 0x63, 0x63, 0xFF);
        Font_SetLineHeight(37);
        Text_SetNumberToken(3, sp2C);
        Font_Printf(sp34 + 16, sp30 + 11, Text_GetString(sp38, sizeof(sp38), sTeamSelectText, 7));
        Font_EndTexturedTextRendering();
        arg0->summaryDisplayProgress = 2;
    } else {
        sp34 = arg0->x;
        sp30 = arg0->y;

        BattleMenu_DrawOrderSelectPanel_PartyGrid(arg0);
        Gfx_FillRectWithDisplayState(sp34 + 5, sp30 + 5, 461, 84, 0, 0, 0, 0x80);
        Font_BeginTranslucentTextRendering();
        Font_SetActive(0x10, 0);
        Gfx_SetEnvColor(0xFF, 0x63, 0x63, 0xFF);
        Font_SetLineHeight(40);
        Text_SetNumberToken(3, sp2C);
        Font_Printf(sp34 + 80, sp30 + 20, Text_GetString(NULL, 0, sTeamSelectText, 7));
        Font_EndTexturedTextRendering();
    }
}

void BattleMenu_DrawOrderSelectPanel_Ready(BattleOrderInputState* input_state) {
    s32 x;
    s32 y;
    s32 alpha;
    char* text;

    if (input_state->tagTeamPartnerPort == -1) {
        // clang-format off
        if (input_state->isHuman == 0) {
            x = 200; y = 129;
        } else {
            x = 80; y = 240;
        }
        // clang-format on
        input_state->summaryDisplayProgress = 0;
        BattleMenu_DrawOrderSelectPanel_PartyGrid(input_state);
        BattleMenu_DrawBorderedPanel(x, y, 351, 83, input_state->port);
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

        if (input_state->loadProgress < 16) {
            alpha = input_state->loadProgress * 16;
        } else if (input_state->loadProgress < 35) {
            alpha = 0xFF;
        } else if (input_state->loadProgress < 50) {
            alpha = (-input_state->loadProgress * 16) + 800;
        } else {
            alpha = 0;
        }

        input_state->loadProgress++;
        if (input_state->loadProgress >= 61) {
            input_state->loadProgress = 0;
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

void BattleMenu_DrawOrderSelectPanel_MonDetail(BattleOrderInputState* input_state) {
    s32 i;
    s16 x;
    s8 slot_to_summarize;
    PokemonTypeString* temp_v1;
    s32 idx;
    s32 y;
    BattleMon* pokemon_to_summarize;

    x = input_state->x;
    slot_to_summarize = input_state->summaryDisplaySlot;
    y = input_state->y;
    pokemon_to_summarize = &input_state->pokemon[slot_to_summarize];

    BattleMenu_DrawBorderedPanel(x, y, 471, 94, input_state->port);

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

    idx = PokemonType_ToDisplayIndex(pokemon_to_summarize->type1);
    Gfx_SetEnvColor(sPokemonTypeStrings[idx].color.r, sPokemonTypeStrings[idx].color.g, sPokemonTypeStrings[idx].color.b, 0xFF);
    Font_Printf(x + 122, y + 47,
                Text_GetString(NULL, 0, sTypeNameTable, PokemonType_ToDisplayIndex(pokemon_to_summarize->type1)));

    if (pokemon_to_summarize->type1 != pokemon_to_summarize->type2) {
        idx = PokemonType_ToDisplayIndex(pokemon_to_summarize->type2);
        Gfx_SetEnvColor(sPokemonTypeStrings[idx].color.r, sPokemonTypeStrings[idx].color.g, sPokemonTypeStrings[idx].color.b, 0xFF);
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
        temp_v1 = &sPokemonTypeStrings[gMoveDisplayInfo[pokemon_to_summarize->moves[i] - 1].type];
        Font_SetActive(4, 0);
        Gfx_SetEnvColor(temp_v1->color.r, temp_v1->color.g, temp_v1->color.b, 0xFF);
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
                              sPokemonTypeStrings[gMoveDisplayInfo[pokemon_to_summarize->moves[i] - 1].type].string, 20, 0);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    if (input_state->summaryDisplayProgress > 0) {
        if (input_state->isHuman == 0) {
            Gfx_DrawRgba16ImageRegion(sPartyGridTextureOffset, 200, 129, 351, 83);
        } else {
            Gfx_DrawRgba16ImageRegion(sPartyGridTextureOffset, 80, 240, 351, 83);
        }
        input_state->summaryDisplayProgress--;
    }
}

// Better name: BattleMenu_DrawTrainerLabel
void BattleMenu_DrawConfirmButtonLabel(BattleOrderInputState* input_state) {
    s16 x;
    s16 y;

    if (input_state->isHuman == 0) {
        // clang-format off
        x = 80; y = 129;
        // clang-format on
    } else {
        x = 433;
        y = 291;
    }

    BattleMenu_DrawBorderedPanel(x, y, 118, 32, input_state->port);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(8, 0);
    Font_Printf((x - (Font_MeasureTextExtent(8, 0, input_state->roster->trainerName) / 2)) + 59, y + 5,
                input_state->roster->trainerName);
    Font_EndTexturedTextRendering();
}

void BattleMenu_DrawOrderSelectPanel(BattleOrderInputState* input_state) {
    if ((D_84803798 != 0) || (input_state->confirmCursorMoving != 0)) {
        if ((D_84803798 != 0) && (input_state->tagTeamPartnerPort == -1)) {
            BattleMenu_DrawConfirmButtonLabel(input_state);
        }

        switch (input_state->doneSelecting) {
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

        if (input_state->confirmCursorMoving != 0) {
            input_state->confirmCursorMoving--;
        }
    } else if (input_state->doneSelecting == 3) {
        BattleMenu_DrawOrderSelectPanel_Ready(input_state);
    }
}

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

s32 BattleMenu_IsOrderSelectionComplete(BattleOrderInputState* input_state) {
    s8 number_of_picks_allowed;

    if ((sSessionContext->ruleCategory != RULE_CATEGORY_ANYTHING_GOES) && (sSessionContext->ruleCategory != RULE_CATEGORY_VS_MEWTWO)) {
        number_of_picks_allowed = 3;
    } else if (input_state->roster->isHuman & 0x2) {
        number_of_picks_allowed = input_state->rosterSize;
    } else {
        number_of_picks_allowed = input_state->rosterSize;
        if ((input_state->tagTeamPartnerPort != -1) && (number_of_picks_allowed >= 4)) {
            number_of_picks_allowed = 3;
        }
    }

    return input_state->numberOfPokemonPicked >= number_of_picks_allowed;
}

// better name: BattleMenu_SelectSlotOrShowSummary
void BattleMenu_SelectOrderSlot(BattleOrderInputState* input_state, s8 selected_slot, s32 show_summary) {
    if (selected_slot >= input_state->rosterSize) {
        Audio_PlaySoundEffectById(SFX_MENU_ERROR);
        return;
    }

    if (show_summary != 0) {
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
        input_state->doneSelecting = 4;
        input_state->confirmCursorMoving = 2;
        input_state->summaryDisplaySlot = selected_slot;
        return;
    }

    if (BattleMenu_IsSlotAlreadySelected(input_state, selected_slot) != 0) {
        Audio_PlaySoundEffectById(SFX_MENU_ERROR);
        return;
    }

    input_state->pickedSlots[input_state->numberOfPokemonPicked] = selected_slot;
    input_state->numberOfPokemonPicked++;
    BattleMenu_AppendOrderSlotIcon(input_state->isHuman, input_state->port);

    if (input_state->rButtonHeld != 0) {
        input_state->confirmCursorMoving = 2;
    }

    if (BattleMenu_PicksExceedLevelCap(input_state) != 0) {
        Audio_PlaySoundEffectById(SFX_MENU_ERROR);
        input_state->doneSelecting = 2;
        input_state->confirmCursorMoving = 2;
        return;
    }

    Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
    if (BattleMenu_IsOrderSelectionComplete(input_state) != 0) {
        input_state->doneSelecting = 1;
        input_state->confirmCursorSelection = 0;
        input_state->confirmCursorMoving = 2;
    }
}

// better name: BattleMenu_UndoSelections
void BattleMenu_UndoLastOrderSelection(BattleOrderInputState* order_state) {
    BattleOrderInputState* temp_v0;

    if (order_state->numberOfPokemonPicked > 0) {
        order_state->numberOfPokemonPicked = 0;

        BattleMenu_ResetOrderSlotIcons(order_state->isHuman);

        if (order_state->rButtonHeld != 0) {
            order_state->confirmCursorMoving = 2;
        }

        Audio_PlaySoundEffectById(SFX_MENU_BACK);

        if (order_state->tagTeamPartnerPort != -1) {
            temp_v0 = &sBattleOrderInputStates[order_state->tagTeamPartnerPort];
            temp_v0->doneSelecting = 0;
            temp_v0->numberOfPokemonPicked = 0;
            temp_v0->rButtonHeld = 0;
            temp_v0->confirmCursorMoving = 2;
        }
    }
}

void BattleMenu_HandleOrderSelectPanel_PartyInput(Controller* controller, BattleOrderInputState* input_state) {
    s32 r_held_down = BTN_IS_DOWN(controller, BTN_R);

    if (BTN_IS_PRESSED(controller, BTN_R)) {
        // clang-format off
        input_state->rButtonHeld = 1; input_state->confirmCursorMoving = 2;
        // clang-format on
    }

    if (controller->buttonHeld & 0x10) {
        input_state->rButtonHeld = 0;
        input_state->confirmCursorMoving = 2;
    }

    if (BTN_IS_PRESSED(controller, BTN_L)) {
        BattleMenu_UndoLastOrderSelection(input_state);
    } else if (BTN_IS_PRESSED(controller, BTN_B)) {
        BattleMenu_SelectOrderSlot(input_state, 0, r_held_down);
    } else if (BTN_IS_PRESSED(controller, BTN_CLEFT)) {
        BattleMenu_SelectOrderSlot(input_state, 1, r_held_down);
    } else if (BTN_IS_PRESSED(controller, BTN_CUP)) {
        BattleMenu_SelectOrderSlot(input_state, 2, r_held_down);
    } else if (BTN_IS_PRESSED(controller, BTN_A)) {
        BattleMenu_SelectOrderSlot(input_state, 3, r_held_down);
    } else if (BTN_IS_PRESSED(controller, BTN_CDOWN)) {
        BattleMenu_SelectOrderSlot(input_state, 4, r_held_down);
    } else if (BTN_IS_PRESSED(controller, BTN_CRIGHT)) {
        BattleMenu_SelectOrderSlot(input_state, 5, r_held_down);
    }
}

void BattleMenu_HandleOrderSelectPanel_ConfirmInput(Controller* controller, BattleOrderInputState* input_state) {
    BattleOrderInputState* temp_v0_2;

    if (BTN_IS_PRESSED(controller, BTN_R)) {
        input_state->rButtonHeld = 1;
        input_state->confirmCursorMoving = 2;
    }
    if (controller->buttonHeld & 0x10) {
        input_state->rButtonHeld = 0;
        input_state->confirmCursorMoving = 2;
    }

    if (BTN_IS_PRESSED(controller, BTN_DLEFT | BTN_DRIGHT)) {
        input_state->confirmCursorMoving = 2;
        input_state->confirmCursorSelection ^= 1;
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
    } else if (BTN_IS_PRESSED(controller, BTN_A)) {
        if (input_state->confirmCursorSelection == 0) {
            input_state->confirmCursorMoving = 2;
            input_state->doneSelecting = 3;
            input_state->loadProgress = 0;
            Audio_PlaySoundEffectById(9);
        } else {
            input_state->confirmCursorMoving = 2;
            input_state->doneSelecting = 0;
            input_state->numberOfPokemonPicked = 0;
            input_state->rButtonHeld = 0;
            BattleMenu_ResetOrderSlotIcons(input_state->isHuman);
            Audio_PlaySoundEffectById(SFX_MENU_BACK);
            if (input_state->tagTeamPartnerPort != -1) {
                temp_v0_2 = &sBattleOrderInputStates[input_state->tagTeamPartnerPort];
                temp_v0_2->doneSelecting = 0;
                temp_v0_2->numberOfPokemonPicked = 0;
                temp_v0_2->rButtonHeld = 0;
                temp_v0_2->confirmCursorMoving = 2;
            }
        }
    }
    input_state->confirmCursorMoving = 2;
}

void BattleMenu_HandleOrderSelectPanel_LevelCapNoticeInput(Controller* controller, BattleOrderInputState* input_state) {
    if (BTN_IS_PRESSED(controller, BTN_A | BTN_B)) {
        input_state->doneSelecting = 0;
        input_state->numberOfPokemonPicked = 0;
        input_state->rButtonHeld = 0;
        input_state->confirmCursorMoving = 2;
        BattleMenu_ResetOrderSlotIcons(input_state->isHuman);
        Audio_PlaySoundEffectById(SFX_MENU_BACK);
    }
}

void BattleMenu_HandleOrderSelectPanel_MonDetailInput(Controller* controller, BattleOrderInputState* input_state) {
    s32 is_valid_button = 0;

    switch (input_state->summaryDisplaySlot) {
        case 0:
            if (controller->buttonHeld & BTN_B) {
                is_valid_button = 1;
            }
            break;

        case 1:
            if (controller->buttonHeld & BTN_CLEFT) {
                is_valid_button = 1;
            }
            break;

        case 2:
            if (controller->buttonHeld & BTN_CUP) {
                is_valid_button = 1;
            }
            break;

        case 3:
            if (controller->buttonHeld & BTN_A) {
                is_valid_button = 1;
            }
            break;

        case 4:
            if (controller->buttonHeld & BTN_CDOWN) {
                is_valid_button = 1;
            }
            break;

        case 5:
            if (controller->buttonHeld & BTN_CRIGHT) {
                is_valid_button = 1;
            }
            break;
    }

    if (is_valid_button != 0) {
        if (!(controller->buttonDown & BTN_R)) {
            input_state->rButtonHeld = 0;
        }
        input_state->doneSelecting = 0;
        input_state->confirmCursorMoving = 2;
    }
}

s32 BattleMenu_UpdateOrderSelectPanel(s32 do_not_update, BattleOrderInputState* input_state) {
    Controller* controller;
    s32 done = 0;

    if (input_state->roster->isHuman & 0x2) {
        controller = &sComController;
    } else {
        controller = &gControllers[input_state->port];
    }

    if (do_not_update == 0) {
        switch (input_state->doneSelecting) {
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

void BattleMenu_RearrangeLeadPokemon(BattleSessionTeams* battle_session_teams, TeamOrderState* order_state) {
    TeamRoster* temp_v0;
    s32 tmp;

    if (battle_session_teams->tagTeamSize == 2) {
        tmp = ((order_state->pokemonPicked[0] >> 4) & 0xF) - 1;
        if (tmp == battle_session_teams->tagTeamMember[1]->controllerPort) {
            temp_v0 = battle_session_teams->tagTeamMember[0];
            battle_session_teams->tagTeamMember[0] = battle_session_teams->tagTeamMember[1];
            battle_session_teams->tagTeamMember[1] = temp_v0;
        }
    }
}

void BattleMenu_FillDefaultOrder(s8* pokemon_slots, s32 number_of_slots_filled) {
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

void BattleMenu_CommitOrderSelection(void) {
    s32 i;
    s32 var_s4;
    TeamRoster* temp_s1;
    BattleOrderInputState* var_s2;

    var_s2 = sBattleOrderInputStates;
    var_s4 = D_8480379C;

    while (var_s4-- > 0) {
        temp_s1 = var_s2->roster;
        if (temp_s1->isHuman & 0x2) {
            temp_s1->numberOfPokemonBrought = temp_s1->rosterInfo->numberOfPokemonCanBePicked;
            _bcopy(temp_s1->rosterInfo->battleMon, temp_s1->pokemonPicked, 504);
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

void BattleMenu_AddOrderSelectPanel(s16 x, s16 y, TeamRoster* roster, s32 is_human, s32 port) {
    s32 i;
    BattleOrderInputState* input_state;
    BattleMon* pokemon;

    input_state = &sBattleOrderInputStates[D_8480379C];
    input_state->unk_0000 = D_8480379C;
    input_state->port = roster->controllerPort;
    input_state->tagTeamPartnerPort = port;
    input_state->isHuman = is_human;
    input_state->doneSelecting = 0;
    input_state->numberOfPokemonPicked = 0;
    input_state->confirmCursorMoving = 2;
    input_state->summaryDisplayProgress = 0;
    input_state->rButtonHeld = 0;
    input_state->confirmCursorSelection = 0;
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

    D_8480379C++;
}

void BattleMenu_InitOrderSelectPanels(SessionContext* session_context) {
    BattleSessionTeams* session_teams = &session_context->battleSides[0];

    sSessionContext = session_context;
    D_8480379C = 0;
    D_84803798 = 2;
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
        BattleMenu_AddOrderSelectPanel(80, 33, session_teams->tagTeamMember[0], 0, D_8480379C + 1);
        BattleMenu_AddOrderSelectPanel(80, 129, session_teams->tagTeamMember[1], 0, D_8480379C - 1);
    } else {
        BattleMenu_AddOrderSelectPanel(80, 33, session_teams->tagTeamMember[1], 0, D_8480379C + 1);
        BattleMenu_AddOrderSelectPanel(80, 129, session_teams->tagTeamMember[0], 0, D_8480379C - 1);
    }

    session_teams++;
    if (session_teams->tagTeamSize == 1) {
        BattleMenu_AddOrderSelectPanel(80, 326, session_teams->tagTeamMember[0], 1, -1);
    } else if (session_teams->tagTeamMember[0]->controllerPort < session_teams->tagTeamMember[1]->controllerPort) {
        BattleMenu_AddOrderSelectPanel(80, 230, session_teams->tagTeamMember[0], 1, D_8480379C + 1);
        BattleMenu_AddOrderSelectPanel(80, 326, session_teams->tagTeamMember[1], 1, D_8480379C - 1);
    } else {
        BattleMenu_AddOrderSelectPanel(80, 230, session_teams->tagTeamMember[1], 1, D_8480379C + 1);
        BattleMenu_AddOrderSelectPanel(80, 326, session_teams->tagTeamMember[0], 1, D_8480379C - 1);
    }
}

#if defined(VERSION_US_REV2)
// asm-processor requires this import on one physical line.
// clang-format off
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/battle_order_select/battle_order_select/BattleMenu_RunOrderSelectScreen.s")
// clang-format on
#else
s32 BattleMenu_RunOrderSelectScreen(s32 arg0, s32 arg1) {
    StageContext* stage_context;

    main_pool_push_state('MSEL');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    stage_context = StageContext_Allocate(1, 1, 2, 0, 2, 1);
    Text_InitStringTables();
    Font_Init(60, 0);
    PokeIcon_OpenModelArchive();
    D_84803780 = ASSET_LOAD2(backgrounds, 1, 1);
    sPartyGridTextureOffset = BinArchive_GetFile(D_84803780, 5);

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
