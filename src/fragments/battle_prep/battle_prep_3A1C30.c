#include "battle_prep.h"
#include "game_state.h"
#include "src/geo_render.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/ui_graphics.h"
#include "src/save_data.h"
#include "src/audio_sfx.h"
#include "src/matrix.h"
#include "src/math_util.h"
#include "src/memmap.h"
#include "src/memory.h"

static BattlePrepTextWindowLayout gBattlePrepRuleWindowLayouts[] = {
    {
        78,
        84,
        164,
        72,
        2,
        0x2121,
        0,
        -1,
        0x2C,
    },
    {
        78,
        91,
        164,
        58,
        2,
        0x2121,
        0,
        -1,
        0x2C,
    },
    {
        77,
        122,
        166,
        92,
        5,
        0x2121,
        0,
        -1,
        0x2492,
    },
    {
        77,
        142,
        166,
        52,
        3,
        0x2121,
        0,
        -1,
        0x92,
    },
    {
        77,
        144,
        166,
        52,
        3,
        0x2121,
        0,
        -1,
        0x92,
    },
    {
        77,
        149,
        166,
        38,
        2,
        0x2121,
        0,
        -1,
        0x12,
    },
    {
        60,
        40,
        200,
        64,
        1,
        0x2121,
        0,
        -1,
        0x2,
    },
    {
        60,
        40,
        200,
        64,
        1,
        0x2121,
        0,
        -1,
        0x2,
    },
    {
        68,
        32,
        184,
        104,
        1,
        0x2121,
        1,
        -1,
        0x2,
    },
    {
        60,
        38,
        200,
        164,
        2,
        0x6909,
        0,
        -1,
        0xD,
    },
};

/**
 * Draws the rule window's selection arrow beside the selected row, bobbing
 * horizontally on a 12-frame cycle.
 */
void BattlePrep_DrawCursorArrow(BattlePrepTextWindow* text_window, s16 x, s16 y) {
    static s16 sCursorPosition = 0;

    if (sCursorPosition < 6) {
        x = (x + (sCursorPosition / 2));
    } else {
        x = ((x - (sCursorPosition / 2)) + 6);
    }

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    gDPLoadTextureBlock(gDisplayListHead++, gMenuCursor, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 22, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(x, y + (text_window->selectedOption * 14) + 1, 16, 11, 0, 0, 0x800, 0x800, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    sCursorPosition = (sCursorPosition + 1) % 12;
}

// Better name: BattlePrep_DrawContinueWindowOption
/**
 * Prints dialogue option for continue windows
 */
void BattlePrep_DrawRuleOption(BattlePrepTextWindow* text_window, s16 x, s16 y, s16 option_index, s32 is_selectable, s8* text) {
    if (is_selectable == 0) {
        Gfx_SetEnvColor(0x64, 0x64, 0x64, 0xFF); // #646464
    } else if (option_index == text_window->selectedOption) {
        Gfx_SetEnvColor(0xFF, 0xFF, 0, 0xFF); // #ffff00
    } else {
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #ffffff
    }
    Font_Printf(x, y + (option_index * 14), text);
}

/**
 * Texts window kinds 1 and 2: "Keep battling?" Continue / Save and Quit. Kind 1 (cups) also
 * shows "Continue = N".
 */
void BattlePrep_DrawKeepBattlingPrompt(BattlePrepTextWindow* text_window, BattlePrepTextWindowLayout* window_layout) {
    BattlePrep_DrawCursorArrow(text_window, window_layout->x + 12, window_layout->y + 24);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);
    Font_Printf(window_layout->x + 16, window_layout->y + 6, BattlePrep_GetString(41));
    BattlePrep_DrawRuleOption(text_window, window_layout->x + 34, window_layout->y + 24, 0, 1, BattlePrep_GetString(42));
    BattlePrep_DrawRuleOption(text_window, window_layout->x + 34, window_layout->y + 24, 1, 1, BattlePrep_GetString(43));

    if (window_layout->restingHeight == 72) {
        Font_SetActive(1, 0);
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #ffffff
        Font_Printf(window_layout->x + 74, window_layout->y + 54, "%s%d", BattlePrep_GetString(44),
                    gSessionContext.continueCount);
    }

    Font_EndTexturedTextRendering();
}

/**
 * Rule window kind 3: cup loss menu (Continue / Save / Re-enter / From the start / Quit).
 */
void BattlePrep_DrawCupLossMenu(BattlePrepTextWindow* text_window, BattlePrepTextWindowLayout* window_layout) {
    s32 has_continues;
    s32 on_first_trainer;

    has_continues = gSessionContext.continueCount > 0;
    // FAKE: triple negation
    on_first_trainer = !!(gSessionContext.sessionFlags & 0x2);
    on_first_trainer = !on_first_trainer;

    BattlePrep_DrawCursorArrow(text_window, window_layout->x + 5, window_layout->y + 6);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);
    BattlePrep_DrawRuleOption(text_window, window_layout->x + 26, window_layout->y + 6, 0, has_continues, BattlePrep_GetString(45));
    BattlePrep_DrawRuleOption(text_window, window_layout->x + 26, window_layout->y + 6, 1, has_continues, BattlePrep_GetString(46));
    BattlePrep_DrawRuleOption(text_window, window_layout->x + 26, window_layout->y + 6, 2, on_first_trainer, BattlePrep_GetString(47));
    BattlePrep_DrawRuleOption(text_window, window_layout->x + 26, window_layout->y + 6, 3, 1, BattlePrep_GetString(48));
    BattlePrep_DrawRuleOption(text_window, window_layout->x + 26, window_layout->y + 6, 4, 1, BattlePrep_GetString(49));
    Font_SetActive(1, 0);
    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #ffffff
    Font_Printf(window_layout->x + 74, window_layout->y + 76, "%s%d", BattlePrep_GetString(44),
                gSessionContext.continueCount);
    Font_EndTexturedTextRendering();
}

/**
 * Rule window kind 4: castle loss menu (Re-enter / Re-start at this GYM / Quit).
 */
void BattlePrep_DrawCastleLossMenu(BattlePrepTextWindow* text_window, BattlePrepTextWindowLayout* window_layout) {
    // FAKE: triple Negation
    // TODO: find out what sets this to true, it's not:
    // - being passed the first trainer
    // - being in the elite 4
    // - being in the champion
    s32 sp2C = !!(gSessionContext.sessionFlags & 0x2);

    sp2C = !sp2C;
    BattlePrep_DrawCursorArrow(text_window, window_layout->x + 5, window_layout->y + 6);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);
    BattlePrep_DrawRuleOption(text_window, window_layout->x + 26, window_layout->y + 6, 0, sp2C, BattlePrep_GetString(47));
    BattlePrep_DrawRuleOption(text_window, window_layout->x + 26, window_layout->y + 6, 1, 1, BattlePrep_GetString(50));
    BattlePrep_DrawRuleOption(text_window, window_layout->x + 26, window_layout->y + 6, 2, 1, BattlePrep_GetString(49));
    Font_EndTexturedTextRendering();
}

/**
 * Rule window kind 5: Free Battle menu (Try once more / Change team combination / Quit).
 */
void BattlePrep_DrawFreeBattleMenu(BattlePrepTextWindow* text_window, BattlePrepTextWindowLayout* window_layout) {
    s32 one_player_has_99_wins = gSessionContext.battleSides[0].winCount < 99 && gSessionContext.battleSides[1].winCount < 99;

    BattlePrep_DrawCursorArrow(text_window, window_layout->x + 5, window_layout->y + 6);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);
    BattlePrep_DrawRuleOption(text_window, window_layout->x + 26, window_layout->y + 6, 0, one_player_has_99_wins, BattlePrep_GetString(51));
    BattlePrep_DrawRuleOption(text_window, window_layout->x + 26, window_layout->y + 6, 1, 1, BattlePrep_GetString(52));
    BattlePrep_DrawRuleOption(text_window, window_layout->x + 26, window_layout->y + 6, 2, 1, BattlePrep_GetString(49));
    Font_EndTexturedTextRendering();
}

/**
 * Rule window kind 6: Mewtwo loss menu (Battle again / Quit).
 */
void BattlePrep_DrawMewtwoLossMenu(BattlePrepTextWindow* text_window, BattlePrepTextWindowLayout* window_layout) {
    BattlePrep_DrawCursorArrow(text_window, window_layout->x + 5, window_layout->y + 6);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);
    BattlePrep_DrawRuleOption(text_window, window_layout->x + 26, window_layout->y + 6, 0, 1, BattlePrep_GetString(53));
    BattlePrep_DrawRuleOption(text_window, window_layout->x + 26, window_layout->y + 6, 1, 1, BattlePrep_GetString(49));
    Font_EndTexturedTextRendering();
}

/**
 * Rule window kind 7: cup name, ball class and BATTLE N label.
 * Displays upon loading a suspending run from the pre-area select menu
 */
void BattlePrep_DrawCupProgressHeader(BattlePrepTextWindow* text_window, BattlePrepTextWindowLayout* window_layout) {
    char* string;

    Font_BeginTranslucentTextRendering();
    Font_EnableTwoCycleTexturing();
    Font_SetActive(4, 0);
    Gfx_SetPrimColor(0x32, 0xFF, 0x64, 0xFF); // #32ff64

    if ((gSessionContext.mode == RULE_CATEGORY_NINTENDO_CUP_99) || (gSessionContext.mode == RULE_CATEGORY_PRIME_CUP)) {
        string = BattlePrep_GetString(gSessionContext.mode - 1);
        Font_Printf((window_layout->x - (Font_MeasureTextExtent(4, 0, string) / 2)) + 100, window_layout->y + 6, string);
        string = BattlePrep_GetString(gSessionContext.modeSelection + 7);
        Font_Printf((window_layout->x - (Font_MeasureTextExtent(4, 0, string) / 2)) + 100, window_layout->y + 24, string);
        string = BattlePrep_GetString(gSessionContext.roundOrDemoId + 10);
        Font_Printf((window_layout->x - (Font_MeasureTextExtent(4, 0, string) / 2)) + 100, window_layout->y + 42, string);
    } else {
        string = BattlePrep_GetString(gSessionContext.mode - 1);
        Font_Printf((window_layout->x - (Font_MeasureTextExtent(4, 0, string) / 2)) + 100, window_layout->y + 10, string);
        string = BattlePrep_GetString(gSessionContext.roundOrDemoId + 10);
        Font_Printf((window_layout->x - (Font_MeasureTextExtent(4, 0, string) / 2)) + 100, window_layout->y + 38, string);
    }

    Font_DisableTwoCycleTexturing();
    Font_EndTexturedTextRendering();
}

#if defined(VERSION_US_REV2)
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/battle_prep/battle_prep_3A1C30/BattlePrep_DrawCastleProgressHeader.s")
#else
/**
 * Rule window kind 8: gym name with BATTLE, TRAINER N / GYM LEADER / ELITE FOUR N.
 * Displays upon loading a suspending run from the pre-area select menu
 */
void BattlePrep_DrawCastleProgressHeader(BattlePrepTextWindow* text_window, BattlePrepTextWindowLayout* window_layout) {
    s32 text_length;
    char buffer[256];

    Font_BeginTranslucentTextRendering();
    Font_EnableTwoCycleTexturing();
    Font_SetActive(4, 0);
    Gfx_SetPrimColor(0x32, 0xFF, 0x64, 0xFF); // #32ff64

    sprintf(buffer, "%s %s", BattlePrep_GetString(gSessionContext.modeSelection + 19), BattlePrep_GetString(54));
    text_length = Font_MeasureTextExtent(4, 0, buffer);
    Font_Printf((window_layout->x - (text_length / 2)) + 100, window_layout->y + 10, buffer);

    if (gSessionContext.modeSelection < SESSION_MODE_BATTLE_NOW) {
        // It is impossible to suspend the game on Vs Mewtwo so this never happens
        if (gSessionContext.modeSelection == SESSION_MODE_VS_MEWTWO) {
            sprintf(buffer, "%s %d", BattlePrep_GetString(27), gSessionContext.roundOrDemoId);
            text_length = Font_MeasureTextExtent(4, 0, buffer);
            Font_Printf((window_layout->x - (text_length / 2)) + 100, window_layout->y + 38, buffer);
        } else if (gSessionContext.roundOrDemoId < 4) {
            sprintf(&buffer, "%s %d", BattlePrep_GetString(55), gSessionContext.roundOrDemoId);
            text_length = Font_MeasureTextExtent(4, 0, buffer);
            Font_Printf((window_layout->x - (text_length / 2)) + 100, window_layout->y + 38, buffer);
        } else {
            text_length = Font_MeasureTextExtent(4, 0, BattlePrep_GetString(gSessionContext.modeSelection + 29));
            Font_Printf((window_layout->x - (text_length / 2)) + 100, window_layout->y + 38,
                        BattlePrep_GetString(gSessionContext.modeSelection + 29));
        }
    }

    Font_DisableTwoCycleTexturing();
    Font_EndTexturedTextRendering();
}
#endif

/**
 * Draws the COM (D_3025200) or 1P-4P controller tag for a battle side.
 */
void BattlePrep_DrawPlayerTag(s16 x, s16 y, BattleSessionTeams* session_teams) {
    static u8* sPortIcons[] = { D_3025950, D_30260A0, D_30267F0, D_3026F40 };

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    if (session_teams->tagTeamSize == 1) {
        if (session_teams->tagTeamMember[0]->isHuman & 0x2) {
            Gfx_DrawTextureRgba16(x + 26, y + 3, 36, 26, gComIconSmall, 36, 0);
            if (0) {}
        } else {
            Gfx_DrawTextureRgba16(x + 26, y + 3, 36, 26, sPortIcons[session_teams->tagTeamMember[0]->controllerPort],
                                  36, 0);
        }

        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
        return;
    }

    if (session_teams->tagTeamMember[0]->controllerPort < session_teams->tagTeamMember[1]->controllerPort) {
        Gfx_DrawTextureRgba16(x + 6, y + 3, 36, 26, sPortIcons[session_teams->tagTeamMember[0]->controllerPort], 36, 0);
        Gfx_DrawTextureRgba16(x + 48, y + 3, 36, 26, sPortIcons[session_teams->tagTeamMember[1]->controllerPort], 36,
                              0);
    } else {
        Gfx_DrawTextureRgba16(x + 6, y + 3, 36, 26, sPortIcons[session_teams->tagTeamMember[1]->controllerPort], 36, 0);
        Gfx_DrawTextureRgba16(x + 48, y + 3, 36, 26, sPortIcons[session_teams->tagTeamMember[0]->controllerPort], 36,
                              0);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    Font_BeginTranslucentTextRendering();
    Font_SetActive(4, 0);
    Font_DrawCharAt(x + 36, y + 8, 38);
    Font_EndTexturedTextRendering();
}

/**
 * Sets the text color for a side's win count; the side that won this battle pulses yellow.
 */
void BattlePrep_SetWinCountColor(BattleSessionTeams* arg0) {
    static s16 sWinCountHighlightPhase = 0;

    if (arg0->wonBattle == 1) {
        Gfx_SetEnvColor(0xFF, (SINS((s32)(SINS(sWinCountHighlightPhase) * 16384.0f)) * 100.0f) + 155.0f, 0x37, 0xFF);
        // Lowest -> middle -> highest: #ff3737 #ffaa37 #ffff37
        sWinCountHighlightPhase += 0x400;
    } else {
        Gfx_SetEnvColor(0xFF, 0x37, 0x37, 0xFF); // #ff3737
    }
}

/**
 * Rule window kind 9: Free Battle "Record" with each side's win(s) count.
 */
void BattlePrep_DrawWinRecord(BattlePrepTextWindow* text_window, BattlePrepTextWindowLayout* window_layout) {
    BattlePrep_DrawBorderedRect(window_layout->x + 8, window_layout->y + 20, 168, 0x20, 0, 0, 0, 0xFF);
    BattlePrep_DrawBorderedRect(window_layout->x + 8, window_layout->y + 68, 168, 0x20, 0, 0, 0, 0xFF);
    BattlePrep_DrawPlayerTag(window_layout->x + 8, window_layout->y + 20, gSessionContext.battleSides);
    BattlePrep_DrawPlayerTag(window_layout->x + 8, window_layout->y + 68, &gSessionContext.battleSides[1]);
    Font_BeginTranslucentTextRendering();
    Font_EnableTwoCycleTexturing();
    Font_SetActive(4, 0);
    Gfx_SetPrimColor(0x32, 0xFF, 0x64, 0xFF); // #32ff64
    // "Record" in green
    Font_Printf(window_layout->x + 64, window_layout->y + 2, BattlePrep_GetString(56));
    Font_DisableTwoCycleTexturing();
    Font_SetActive(16, 0);
    BattlePrep_SetWinCountColor(&gSessionContext.battleSides[0]);
    Font_Printf(window_layout->x + 92, window_layout->y + 24, "%2d", gSessionContext.battleSides[0].winCount);
    BattlePrep_SetWinCountColor(&gSessionContext.battleSides[1]);
    Font_Printf(window_layout->x + 92, window_layout->y + 72, "%2d", gSessionContext.battleSides[1].winCount);
    Font_SetActive(4, 0);
    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #ffffff
    Font_Printf(window_layout->x + 130, window_layout->y + 31, BattlePrep_GetString(57));
    Font_Printf(window_layout->x + 130, window_layout->y + 79, BattlePrep_GetString(57));
    Font_EndTexturedTextRendering();
}

#if defined(VERSION_US_REV2)
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/battle_prep/battle_prep_3A1C30/BattlePrep_DrawOverwriteSavePrompt.s")
#else
/**
 * Rule window kind 10: over write suspended battle data prompt
 */

// better name: BattlePrep_DrawOverwriteSuspendDataPrompt
void BattlePrep_DrawOverwriteSavePrompt(BattlePrepTextWindow* text_window, BattlePrepTextWindowLayout* window_layout) {
    SessionContinueData suspend_record;
    UNUSED s32 pad;
    s32 warning_length;
    s32 icon_offset_x;

    Session_LoadContinueRecord(&suspend_record, NULL);
    BattlePrep_DrawBorderedRect(window_layout->x + 16, window_layout->y + 20, 168, 60, 0, 0, 0, 0xFF);
    BattlePrep_DrawBorderedRect(window_layout->x + 16, window_layout->y + 96, 168, 34, 0, 0, 0, 0xFF);
    BattlePrep_DrawCursorArrow(text_window, window_layout->x + 20, window_layout->y + 100);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);
    Font_Printf(window_layout->x + 16, window_layout->y + 6, BattlePrep_GetString(58));
    Font_Printf(window_layout->x + 16, window_layout->y + 82, BattlePrep_GetString(59));
    BattlePrep_DrawRuleOption(text_window, window_layout->x + 42, window_layout->y + 100, 0, 1, BattlePrep_GetString(60));
    BattlePrep_DrawRuleOption(text_window, window_layout->x + 42, window_layout->y + 100, 1, 1, BattlePrep_GetString(61));
    Gfx_SetEnvColor(0xFF, 0x64, 0x64, 0xFF); // #ff6464

    if (suspend_record.sessionMode == SESSION_MODE_GYM_LEADER_CASTLE) {
        Font_Printf(window_layout->x + 30, window_layout->y + 23, BattlePrep_GetString(suspend_record.sessionMode - 1));
        Font_Printf(window_layout->x + 30, window_layout->y + 38, BattlePrep_GetString(suspend_record.cupOrGymIndex + 19));
        if (suspend_record.cupOrGymIndex < 8) {
            if (suspend_record.trainerIndex < 4) {
                Font_Printf(window_layout->x + 30, window_layout->y + 53, "%s %d", BattlePrep_GetString(55), suspend_record.trainerIndex);
            } else {
                Font_Printf(window_layout->x + 30, window_layout->y + 53, BattlePrep_GetString(62));
            }
        } else {
            Font_Printf(window_layout->x + 30, window_layout->y + 53, "%s %d", BattlePrep_GetString(27), suspend_record.trainerIndex);
        }
    } else if ((suspend_record.sessionMode == SESSION_MODE_NINTENDO_CUP_99) || (suspend_record.sessionMode == SESSION_MODE_PRIME_CUP)) {
        Font_Printf(window_layout->x + 30, window_layout->y + 23, BattlePrep_GetString(suspend_record.sessionMode - 1));
        Font_Printf(window_layout->x + 30, window_layout->y + 38, BattlePrep_GetString(suspend_record.cupOrGymIndex + 7));
        Font_Printf(window_layout->x + 30, window_layout->y + 53, BattlePrep_GetString(suspend_record.trainerIndex + 10));
    } else {
        Font_Printf(window_layout->x + 30, window_layout->y + 23, BattlePrep_GetString(suspend_record.sessionMode - 1));
        Font_Printf(window_layout->x + 30, window_layout->y + 38, BattlePrep_GetString(suspend_record.trainerIndex + 10));
    }

    if (suspend_record.sessionMode != SESSION_MODE_GYM_LEADER_CASTLE) {
        Font_SetActive(1, 0);
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #ffffff
        Font_Printf(window_layout->x + 100, window_layout->y + 67, "%s %d", BattlePrep_GetString(44), suspend_record.continueCount);
    }

    if (text_window->selectedOption == 0) {
        // "Previous saved data will be deleted" shown when you are hovering over "save"
        warning_length = (window_layout->width - Font_MeasureTextExtent(1, 0, BattlePrep_GetString(63))) / 2;
        Font_SetActive(1, 0);
        Gfx_SetEnvColor(0xFF, 0xFF, 0, 0xFF); // #ffff00
        Font_SetLineHeight(12);
        Font_Printf(window_layout->x + warning_length, window_layout->y + 134, BattlePrep_GetString(63));
    }

    Font_EndTexturedTextRendering();

    if (suspend_record.inRound2 == 1) {
        icon_offset_x = Font_MeasureTextExtent(1, 0, BattlePrep_GetString(suspend_record.sessionMode - 1));

        gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

        Gfx_DrawTextureRgba16(window_layout->x + icon_offset_x + 32, window_layout->y + 21, 16, 16, gSmallRound2Icon, 0x10, 0);

        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
    }
}
#endif

// Better name: BattlePrep_DrawTextWindow
/**
 * Draws the active text window: grows the panel vertically while opening, then draws
 * the full panel and the contents for window kind unk_01 (1-10).
 */
void BattlePrep_DrawRuleWindow(BattlePrepTextWindow* text_window) {
    BattlePrepTextWindowLayout* window_layout;
    s16 y;
    s16 height;

    if ((text_window->state != 0) && (text_window->openingProgress > 0)) {
        window_layout = &gBattlePrepRuleWindowLayouts[text_window->kind - 1];

        if (text_window->openingProgress < 4) {
            height = (window_layout->restingHeight * text_window->openingProgress) / 4;
            if (height < 16) {
                height = 16;
            }

            y = window_layout->y + ((window_layout->restingHeight - height) / 2);
            if (window_layout->isKind8 == 0) {
                Ui_DrawBorderedPanel(window_layout->x, y, window_layout->width, height, window_layout->color);
            } else {
                BattlePrep_DrawBorderedRect(window_layout->x, y, window_layout->width, height, 0x1E, 0x1E, 0x82, 0xFF); // #1e1e82
            }
        } else {
            if (window_layout->isKind8 == 0) {
                Ui_DrawBorderedPanel(window_layout->x, window_layout->y, window_layout->width, window_layout->restingHeight,
                                     window_layout->color);
            } else {
                BattlePrep_DrawBorderedRect(window_layout->x, window_layout->y, window_layout->width, window_layout->restingHeight, 0x1E,
                                            0x1E, 0x82, 0xFF); // #1e82ff
            }

            switch (text_window->kind) {
                case 1:
                    BattlePrep_DrawKeepBattlingPrompt(text_window, window_layout);
                    break;

                case 2:
                    BattlePrep_DrawKeepBattlingPrompt(text_window, window_layout);
                    break;

                case 3:
                    BattlePrep_DrawCupLossMenu(text_window, window_layout);
                    break;

                case 4:
                    BattlePrep_DrawCastleLossMenu(text_window, window_layout);
                    break;

                case 5:
                    BattlePrep_DrawFreeBattleMenu(text_window, window_layout);
                    break;

                case 6:
                    BattlePrep_DrawMewtwoLossMenu(text_window, window_layout);
                    break;

                case 7:
                    BattlePrep_DrawCupProgressHeader(text_window, window_layout);
                    break;

                case 8:
                    BattlePrep_DrawCastleProgressHeader(text_window, window_layout);
                    break;

                case 9:
                    BattlePrep_DrawWinRecord(text_window, window_layout);
                    break;

                case 10:
                    BattlePrep_DrawOverwriteSavePrompt(text_window, window_layout);
                    break;
            }
        }
    }
}

/**
 * Handles input for an open rule window: A plays the option's sound from the layout's unk_10
 * 3-bit table and starts closing; D-pad up/down moves the cursor, skipping disabled rows (unk_0E).
 */
// better name: BattlePrep_RunQuitMenu
void BattlePrep_RunQuitMenu(BattlePrepTextWindow* text_window) {
    BattlePrepTextWindowLayout* window_layout;
    s16 last_option_index;
    s32 sound_effect_switch;

    window_layout = &gBattlePrepRuleWindowLayouts[text_window->kind - 1];
    last_option_index = window_layout->optionCount - 1;

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
        sound_effect_switch = window_layout->unk_10 >> (text_window->selectedOption * 3);
        switch (sound_effect_switch & 0x7) {
            case 1:
                Audio_PlaySoundEffectById(SFX_MENU_BACK);
                break;

            case 2:
                Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
                break;

            case 3:
                Audio_PlaySoundEffectById(28);
                break;

            case 4:
                Audio_PlaySoundEffectById(32);
                break;

            case 5:
                Audio_PlaySoundEffectById(29);
                break;

            case 6:
                Audio_PlaySoundEffectById(33);
                break;
        }
        text_window->state = 2;
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DUP)) {
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
        do {
            text_window->selectedOption--;
            if (text_window->selectedOption < 0) {
                text_window->selectedOption = last_option_index;
            }
        } while (!(window_layout->disabledRows & (0x1 << text_window->selectedOption)));
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DDOWN)) {
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
        do {
            text_window->selectedOption++;
            if (last_option_index < text_window->selectedOption) {
                text_window->selectedOption = 0;
            }
        } while (!(window_layout->disabledRows & (0x1 << text_window->selectedOption)));
    }
}

/**
 * State 1: increments the open step each frame until 4, then enters the input state 3.
 */
// better name: BattlePrep_TextWindowExpanding
void BattlePrep_RuleWindowExpanding(BattlePrepTextWindow* text_window) {
    text_window->openingProgress++;
    if (text_window->openingProgress >= 5) {
        text_window->openingProgress = 4;
        text_window->state = 3;
    }
}

/**
 * State 2: decrements the open step each frame until 0, then enters the closed state 4.
 */
// better name: BattlePrep_TextWindowCollapsing
void BattlePrep_RuleWindowCollapsing(BattlePrepTextWindow* text_window) {
    text_window->openingProgress--;
    if (text_window->openingProgress < 0) {
        text_window->openingProgress = 0;
        text_window->state = 4;
    }
}

/**
 * Per-frame dispatcher for the rule window state machine (1 expanding, 2 collapsing, 3 input).
 */
void BattlePrep_UpdateRuleWindow(BattlePrepTextWindow* text_window) {
    switch (text_window->state) {
        case 1:
            BattlePrep_RuleWindowExpanding(text_window);
            break;

        case 2:
            BattlePrep_RuleWindowCollapsing(text_window);
            break;

        case 3:
            BattlePrep_RunQuitMenu(text_window);
            break;
    }
}

/**
 * Opens rule window kind arg1 with SFX_PANEL_OPEN. Disables rows by session state (no continues
 * left, sessionFlags 0x2, or a win count of 99 in kind 5), then moves the cursor to the first enabled row.
 */
void BattlePrep_InitRuleWindow(BattlePrepTextWindow* text_window, s16 layout_index) {
    BattlePrepTextWindowLayout* window_layout = &gBattlePrepRuleWindowLayouts[layout_index - 1];

    text_window->kind = layout_index;
    text_window->openingProgress = 0;
    text_window->selectedOption = 0;
    text_window->state = 1;

    window_layout->disabledRows = -1;

    Audio_PlaySoundEffectById(SFX_PANEL_OPEN);

    if (layout_index - 1 == 2) {
        if (gSessionContext.continueCount == 0) {
            window_layout->disabledRows &= ~0x3;
        }
        if (gSessionContext.sessionFlags & 0x2) {
            window_layout->disabledRows &= ~0x4;
        }
    } else if (layout_index - 1 == 3) {
        if (gSessionContext.sessionFlags & 0x2) {
            window_layout->disabledRows &= ~0x1;
        }
    } else if ((layout_index - 1 == 4) &&
               ((gSessionContext.battleSides[0].winCount >= 99) || (gSessionContext.battleSides[1].winCount >= 99))) {
        window_layout->disabledRows &= ~0x1;
    }

    while (!(window_layout->disabledRows & (0x1 << text_window->selectedOption))) {
        text_window->selectedOption++;
    }
}

/**
 * Immediately hides the rule window by clearing its state, kind and open step.
 */
void BattlePrep_CloseRuleWindow(BattlePrepTextWindow* text_window) {
    text_window->state = 0;
    text_window->kind = 0;
    text_window->openingProgress = 0;
}

/**
 * Returns the chosen row once the window has finished closing (state 4) and resets it to idle;
 * returns -1 otherwise.
 */
s32 BattlePrep_PollRuleWindow(BattlePrepTextWindow* text_window) {
    if (text_window->state == 4) {
        text_window->state = 0;
        return text_window->selectedOption;
    }
    return -1;
}
