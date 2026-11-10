#include "battle_prep.h"
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

    Gfx_DrawTexturedRectClipped(x, y + (text_window->selected_option * 14) + 1, 16, 11, 0, 0, 0x800, 0x800, 0);

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
    } else if (option_index == text_window->selected_option) {
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

    if (window_layout->unk_06 == 72) {
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
void BattlePrep_DrawFreeBattleMenu(BattlePrepTextWindow* arg0, BattlePrepTextWindowLayout* arg1) {
    s32 one_player_had_99_wins = gSessionContext.battleSides[0].winCount < 99 && gSessionContext.battleSides[1].winCount < 99;

    BattlePrep_DrawCursorArrow(arg0, arg1->x + 5, arg1->y + 6);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);
    BattlePrep_DrawRuleOption(arg0, arg1->x + 26, arg1->y + 6, 0, one_player_had_99_wins, BattlePrep_GetString(51));
    BattlePrep_DrawRuleOption(arg0, arg1->x + 26, arg1->y + 6, 1, 1, BattlePrep_GetString(52));
    BattlePrep_DrawRuleOption(arg0, arg1->x + 26, arg1->y + 6, 2, 1, BattlePrep_GetString(49));
    Font_EndTexturedTextRendering();
}

/**
 * Rule window kind 6: Mewtwo loss menu (Battle again / Quit).
 */
void BattlePrep_DrawMewtwoLossMenu(BattlePrepTextWindow* arg0, BattlePrepTextWindowLayout* arg1) {
    BattlePrep_DrawCursorArrow(arg0, arg1->x + 5, arg1->y + 6);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);
    BattlePrep_DrawRuleOption(arg0, arg1->x + 26, arg1->y + 6, 0, 1, BattlePrep_GetString(53));
    BattlePrep_DrawRuleOption(arg0, arg1->x + 26, arg1->y + 6, 1, 1, BattlePrep_GetString(49));
    Font_EndTexturedTextRendering();
}

/**
 * Rule window kind 7: cup name, ball class and BATTLE N label.
 */
void BattlePrep_DrawCupProgressHeader(BattlePrepTextWindow* arg0, BattlePrepTextWindowLayout* arg1) {
    char* temp_v0;

    Font_BeginTranslucentTextRendering();
    Font_EnableTwoCycleTexturing();
    Font_SetActive(4, 0);
    Gfx_SetPrimColor(0x32, 0xFF, 0x64, 0xFF);

    if ((gSessionContext.mode == 3) || (gSessionContext.mode == 6)) {
        temp_v0 = BattlePrep_GetString(gSessionContext.mode - 1);
        Font_Printf((arg1->x - (Font_MeasureTextExtent(4, 0, temp_v0) / 2)) + 100, arg1->y + 6, temp_v0);
        temp_v0 = BattlePrep_GetString(gSessionContext.modeSelection + 7);
        Font_Printf((arg1->x - (Font_MeasureTextExtent(4, 0, temp_v0) / 2)) + 100, arg1->y + 24, temp_v0);
        temp_v0 = BattlePrep_GetString(gSessionContext.roundOrDemoId + 10);
        Font_Printf((arg1->x - (Font_MeasureTextExtent(4, 0, temp_v0) / 2)) + 100, arg1->y + 42, temp_v0);
    } else {
        temp_v0 = BattlePrep_GetString(gSessionContext.mode - 1);
        Font_Printf((arg1->x - (Font_MeasureTextExtent(4, 0, temp_v0) / 2)) + 100, arg1->y + 10, temp_v0);
        temp_v0 = BattlePrep_GetString(gSessionContext.roundOrDemoId + 10);
        Font_Printf((arg1->x - (Font_MeasureTextExtent(4, 0, temp_v0) / 2)) + 100, arg1->y + 38, temp_v0);
    }

    Font_DisableTwoCycleTexturing();
    Font_EndTexturedTextRendering();
}

#if defined(VERSION_US_REV2)
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/battle_prep/battle_prep_3A1C30/BattlePrep_DrawCastleProgressHeader.s")
#else
/**
 * Rule window kind 8: gym name with BATTLE, TRAINER N / GYM LEADER / ELITE FOUR N.
 */
void BattlePrep_DrawCastleProgressHeader(BattlePrepTextWindow* arg0, BattlePrepTextWindowLayout* arg1) {
    s32 sp12C;
    char sp2C[256];

    Font_BeginTranslucentTextRendering();
    Font_EnableTwoCycleTexturing();
    Font_SetActive(4, 0);
    Gfx_SetPrimColor(0x32, 0xFF, 0x64, 0xFF);

    sprintf(sp2C, "%s %s", BattlePrep_GetString(gSessionContext.modeSelection + 19), BattlePrep_GetString(54));
    sp12C = Font_MeasureTextExtent(4, 0, sp2C);
    Font_Printf((arg1->x - (sp12C / 2)) + 100, arg1->y + 10, sp2C);

    if (gSessionContext.modeSelection < 9) {
        if (gSessionContext.modeSelection == 8) {
            sprintf(sp2C, "%s %d", BattlePrep_GetString(27), gSessionContext.roundOrDemoId);
            sp12C = Font_MeasureTextExtent(4, 0, sp2C);
            Font_Printf((arg1->x - (sp12C / 2)) + 100, arg1->y + 38, sp2C);
        } else if (gSessionContext.roundOrDemoId < 4) {
            sprintf(&sp2C, "%s %d", BattlePrep_GetString(55), gSessionContext.roundOrDemoId);
            sp12C = Font_MeasureTextExtent(4, 0, sp2C);
            Font_Printf((arg1->x - (sp12C / 2)) + 100, arg1->y + 38, sp2C);
        } else {
            sp12C = Font_MeasureTextExtent(4, 0, BattlePrep_GetString(gSessionContext.modeSelection + 29));
            Font_Printf((arg1->x - (sp12C / 2)) + 100, arg1->y + 38,
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
void BattlePrep_DrawPlayerTag(s16 arg0, s16 arg1, BattleSessionTeams* arg2) {
    static u8* D_84B17608[] = { D_3025950, D_30260A0, D_30267F0, D_3026F40 };

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    if (arg2->tagTeamSize == 1) {
        if (arg2->tagTeamMember[0]->isHuman & 0x2) {
            Gfx_DrawTextureRgba16(arg0 + 26, arg1 + 3, 36, 26, D_3025200, 36, 0);
            if (0) {}
        } else {
            Gfx_DrawTextureRgba16(arg0 + 26, arg1 + 3, 36, 26, D_84B17608[arg2->tagTeamMember[0]->controllerPort],
                                  36, 0);
        }

        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
        return;
    }

    if (arg2->tagTeamMember[0]->controllerPort < arg2->tagTeamMember[1]->controllerPort) {
        Gfx_DrawTextureRgba16(arg0 + 6, arg1 + 3, 36, 26, D_84B17608[arg2->tagTeamMember[0]->controllerPort], 36, 0);
        Gfx_DrawTextureRgba16(arg0 + 48, arg1 + 3, 36, 26, D_84B17608[arg2->tagTeamMember[1]->controllerPort], 36,
                              0);
    } else {
        Gfx_DrawTextureRgba16(arg0 + 6, arg1 + 3, 36, 26, D_84B17608[arg2->tagTeamMember[1]->controllerPort], 36, 0);
        Gfx_DrawTextureRgba16(arg0 + 48, arg1 + 3, 36, 26, D_84B17608[arg2->tagTeamMember[0]->controllerPort], 36,
                              0);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    Font_BeginTranslucentTextRendering();
    Font_SetActive(4, 0);
    Font_DrawCharAt(arg0 + 36, arg1 + 8, 38);
    Font_EndTexturedTextRendering();
}

/**
 * Sets the text color for a side's win count; the side that won this battle pulses yellow.
 */
void BattlePrep_SetWinCountColor(BattleSessionTeams* arg0) {
    static s16 D_84B17618 = 0;

    if (arg0->wonBattle == 1) {
        Gfx_SetEnvColor(0xFF, (SINS((s32)(SINS(D_84B17618) * 16384.0f)) * 100.0f) + 155.0f, 0x37, 0xFF);
        D_84B17618 += 0x400;
    } else {
        Gfx_SetEnvColor(0xFF, 0x37, 0x37, 0xFF);
    }
}

/**
 * Rule window kind 9: Free Battle "Record" with each side's win(s) count.
 */
void BattlePrep_DrawWinRecord(BattlePrepTextWindow* arg0, BattlePrepTextWindowLayout* arg1) {
    BattlePrep_DrawBorderedRect(arg1->x + 8, arg1->y + 20, 168, 0x20, 0, 0, 0, 0xFF);
    BattlePrep_DrawBorderedRect(arg1->x + 8, arg1->y + 68, 168, 0x20, 0, 0, 0, 0xFF);
    BattlePrep_DrawPlayerTag(arg1->x + 8, arg1->y + 20, gSessionContext.battleSides);
    BattlePrep_DrawPlayerTag(arg1->x + 8, arg1->y + 68, &gSessionContext.battleSides[1]);
    Font_BeginTranslucentTextRendering();
    Font_EnableTwoCycleTexturing();
    Font_SetActive(4, 0);
    Gfx_SetPrimColor(0x32, 0xFF, 0x64, 0xFF);
    Font_Printf(arg1->x + 0x40, arg1->y + 2, BattlePrep_GetString(56));
    Font_DisableTwoCycleTexturing();
    Font_SetActive(16, 0);
    BattlePrep_SetWinCountColor(&gSessionContext.battleSides[0]);
    Font_Printf(arg1->x + 92, arg1->y + 24, "%2d", gSessionContext.battleSides[0].winCount);
    BattlePrep_SetWinCountColor(&gSessionContext.battleSides[1]);
    Font_Printf(arg1->x + 92, arg1->y + 72, "%2d", gSessionContext.battleSides[1].winCount);
    Font_SetActive(4, 0);
    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
    Font_Printf(arg1->x + 130, arg1->y + 0x1F, BattlePrep_GetString(57));
    Font_Printf(arg1->x + 130, arg1->y + 79, BattlePrep_GetString(57));
    Font_EndTexturedTextRendering();
}

#if defined(VERSION_US_REV2)
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/battle_prep/battle_prep_3A1C30/BattlePrep_DrawOverwriteSavePrompt.s")
#else
/**
 * Rule window kind 10: overwrite-saved-battle-data prompt.
 */
void BattlePrep_DrawOverwriteSavePrompt(BattlePrepTextWindow* arg0, BattlePrepTextWindowLayout* arg1) {
    SessionContinueData sp50;
    UNUSED s32 pad;
    s32 sp48;
    s32 temp_v0;

    Session_LoadContinueRecord(&sp50, NULL);
    BattlePrep_DrawBorderedRect(arg1->x + 0x10, arg1->y + 20, 168, 60, 0, 0, 0, 0xFF);
    BattlePrep_DrawBorderedRect(arg1->x + 0x10, arg1->y + 96, 168, 34, 0, 0, 0, 0xFF);
    BattlePrep_DrawCursorArrow(arg0, arg1->x + 20, arg1->y + 100);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);
    Font_Printf(arg1->x + 0x10, arg1->y + 6, BattlePrep_GetString(58));
    Font_Printf(arg1->x + 0x10, arg1->y + 82, BattlePrep_GetString(59));
    BattlePrep_DrawRuleOption(arg0, arg1->x + 42, arg1->y + 100, 0, 1, BattlePrep_GetString(60));
    BattlePrep_DrawRuleOption(arg0, arg1->x + 42, arg1->y + 100, 1, 1, BattlePrep_GetString(61));
    Gfx_SetEnvColor(0xFF, 0x64, 0x64, 0xFF);

    if (sp50.unk_00 == 7) {
        Font_Printf(arg1->x + 30, arg1->y + 23, BattlePrep_GetString(sp50.unk_00 - 1));
        Font_Printf(arg1->x + 30, arg1->y + 38, BattlePrep_GetString(sp50.unk_01 + 19));
        if (sp50.unk_01 < 8) {
            if (sp50.unk_02 < 4) {
                Font_Printf(arg1->x + 30, arg1->y + 53, "%s %d", BattlePrep_GetString(55), sp50.unk_02);
            } else {
                Font_Printf(arg1->x + 30, arg1->y + 53, BattlePrep_GetString(62));
            }
        } else {
            Font_Printf(arg1->x + 30, arg1->y + 53, "%s %d", BattlePrep_GetString(27), sp50.unk_02);
        }
    } else if ((sp50.unk_00 == 3) || (sp50.unk_00 == 6)) {
        Font_Printf(arg1->x + 30, arg1->y + 23, BattlePrep_GetString(sp50.unk_00 - 1));
        Font_Printf(arg1->x + 30, arg1->y + 38, BattlePrep_GetString(sp50.unk_01 + 7));
        Font_Printf(arg1->x + 30, arg1->y + 53, BattlePrep_GetString(sp50.unk_02 + 10));
    } else {
        Font_Printf(arg1->x + 30, arg1->y + 23, BattlePrep_GetString(sp50.unk_00 - 1));
        Font_Printf(arg1->x + 30, arg1->y + 38, BattlePrep_GetString(sp50.unk_02 + 10));
    }

    if (sp50.unk_00 != 7) {
        Font_SetActive(1, 0);
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
        Font_Printf(arg1->x + 100, arg1->y + 67, "%s %d", BattlePrep_GetString(44), sp50.unk_04);
    }

    if (arg0->selected_option == 0) {
        sp48 = (arg1->unk_04 - Font_MeasureTextExtent(1, 0, BattlePrep_GetString(63))) / 2;
        Font_SetActive(1, 0);
        Gfx_SetEnvColor(0xFF, 0xFF, 0, 0xFF);
        Font_SetLineHeight(12);
        Font_Printf(arg1->x + sp48, arg1->y + 134, BattlePrep_GetString(63));
    }

    Font_EndTexturedTextRendering();

    if (sp50.unk_03 == 1) {
        temp_v0 = Font_MeasureTextExtent(1, 0, BattlePrep_GetString(sp50.unk_00 - 1));

        gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

        Gfx_DrawTextureRgba16(arg1->x + temp_v0 + 0x20, arg1->y + 0x15, 0x10, 0x10, D_3027690, 0x10, 0);

        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
    }
}
#endif

/**
 * Draws the active rule window: grows the panel vertically while opening (unk_02 < 4), then draws
 * the full panel and the contents for window kind unk_01 (1-10).
 */
void BattlePrep_DrawRuleWindow(BattlePrepTextWindow* arg0) {
    BattlePrepTextWindowLayout* temp_s0;
    s16 temp_a1;
    s16 var_a3;

    if ((arg0->unk_00 != 0) && (arg0->unk_02 > 0)) {
        temp_s0 = &gBattlePrepRuleWindowLayouts[arg0->unk_01 - 1];

        if (arg0->unk_02 < 4) {
            var_a3 = (temp_s0->unk_06 * arg0->unk_02) / 4;
            if (var_a3 < 0x10) {
                var_a3 = 0x10;
            }

            temp_a1 = temp_s0->y + ((temp_s0->unk_06 - var_a3) / 2);
            if (temp_s0->unk_0C == 0) {
                Ui_DrawBorderedPanel(temp_s0->x, temp_a1, temp_s0->unk_04, var_a3, temp_s0->unk_0A);
            } else {
                BattlePrep_DrawBorderedRect(temp_s0->x, temp_a1, temp_s0->unk_04, var_a3, 0x1E, 0x1E, 0x82, 0xFF);
            }
        } else {
            if (temp_s0->unk_0C == 0) {
                Ui_DrawBorderedPanel(temp_s0->x, temp_s0->y, temp_s0->unk_04, temp_s0->unk_06,
                                     temp_s0->unk_0A);
            } else {
                BattlePrep_DrawBorderedRect(temp_s0->x, temp_s0->y, temp_s0->unk_04, temp_s0->unk_06, 0x1E,
                                            0x1E, 0x82, 0xFF);
            }

            switch (arg0->unk_01) {
                case 1:
                    BattlePrep_DrawKeepBattlingPrompt(arg0, temp_s0);
                    break;

                case 2:
                    BattlePrep_DrawKeepBattlingPrompt(arg0, temp_s0);
                    break;

                case 3:
                    BattlePrep_DrawCupLossMenu(arg0, temp_s0);
                    break;

                case 4:
                    BattlePrep_DrawCastleLossMenu(arg0, temp_s0);
                    break;

                case 5:
                    BattlePrep_DrawFreeBattleMenu(arg0, temp_s0);
                    break;

                case 6:
                    BattlePrep_DrawMewtwoLossMenu(arg0, temp_s0);
                    break;

                case 7:
                    BattlePrep_DrawCupProgressHeader(arg0, temp_s0);
                    break;

                case 8:
                    BattlePrep_DrawCastleProgressHeader(arg0, temp_s0);
                    break;

                case 9:
                    BattlePrep_DrawWinRecord(arg0, temp_s0);
                    break;

                case 10:
                    BattlePrep_DrawOverwriteSavePrompt(arg0, temp_s0);
                    break;
            }
        }
    }
}

/**
 * Handles input for an open rule window: A plays the option's sound from the layout's unk_10
 * 3-bit table and starts closing; D-pad up/down moves the cursor, skipping disabled rows (unk_0E).
 */
void BattlePrep_RunQuitMenu(BattlePrepTextWindow* arg0) {
    BattlePrepTextWindowLayout* sp24;
    s16 sp22;
    s32 tmp;

    sp24 = &gBattlePrepRuleWindowLayouts[arg0->unk_01 - 1];
    sp22 = sp24->unk_08 - 1;

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
        tmp = sp24->unk_10 >> (arg0->selected_option * 3);
        switch (tmp & 0x7) {
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
        arg0->unk_00 = 2;
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DUP)) {
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
        do {
            arg0->selected_option--;
            if (arg0->selected_option < 0) {
                arg0->selected_option = sp22;
            }
        } while (!(sp24->unk_0E & (0x1 << arg0->selected_option)));
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DDOWN)) {
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
        do {
            arg0->selected_option++;
            if (sp22 < arg0->selected_option) {
                arg0->selected_option = 0;
            }
        } while (!(sp24->unk_0E & (0x1 << arg0->selected_option)));
    }
}

/**
 * State 1: increments the open step each frame until 4, then enters the input state 3.
 */
void BattlePrep_RuleWindowExpanding(BattlePrepTextWindow* arg0) {
    arg0->unk_02++;
    if (arg0->unk_02 >= 5) {
        arg0->unk_02 = 4;
        arg0->unk_00 = 3;
    }
}

/**
 * State 2: decrements the open step each frame until 0, then enters the closed state 4.
 */
void BattlePrep_RuleWindowCollapsing(BattlePrepTextWindow* arg0) {
    arg0->unk_02--;
    if (arg0->unk_02 < 0) {
        arg0->unk_02 = 0;
        arg0->unk_00 = 4;
    }
}

/**
 * Per-frame dispatcher for the rule window state machine (1 expanding, 2 collapsing, 3 input).
 */
void BattlePrep_UpdateRuleWindow(BattlePrepTextWindow* arg0) {
    switch (arg0->unk_00) {
        case 1:
            BattlePrep_RuleWindowExpanding(arg0);
            break;

        case 2:
            BattlePrep_RuleWindowCollapsing(arg0);
            break;

        case 3:
            BattlePrep_RunQuitMenu(arg0);
            break;
    }
}

/**
 * Opens rule window kind arg1 with SFX_PANEL_OPEN. Disables rows by session state (no continues
 * left, sessionFlags 0x2, or a win count of 99 in kind 5), then moves the cursor to the first enabled row.
 */
void BattlePrep_InitRuleWindow(BattlePrepTextWindow* arg0, s16 arg1) {
    BattlePrepTextWindowLayout* ptr = &gBattlePrepRuleWindowLayouts[arg1 - 1];

    arg0->unk_01 = arg1;
    arg0->unk_02 = 0;
    arg0->selected_option = 0;
    arg0->unk_00 = 1;

    ptr->unk_0E = -1;

    Audio_PlaySoundEffectById(SFX_PANEL_OPEN);

    if (arg1 - 1 == 2) {
        if (gSessionContext.continueCount == 0) {
            ptr->unk_0E &= ~0x3;
        }
        if (gSessionContext.sessionFlags & 0x2) {
            ptr->unk_0E &= ~0x4;
        }
    } else if (arg1 - 1 == 3) {
        if (gSessionContext.sessionFlags & 0x2) {
            ptr->unk_0E &= ~0x1;
        }
    } else if ((arg1 - 1 == 4) &&
               ((gSessionContext.battleSides[0].winCount >= 99) || (gSessionContext.battleSides[1].winCount >= 99))) {
        ptr->unk_0E &= ~0x1;
    }

    while (!(ptr->unk_0E & (0x1 << arg0->selected_option))) {
        arg0->selected_option++;
    }
}

/**
 * Immediately hides the rule window by clearing its state, kind and open step.
 */
void BattlePrep_CloseRuleWindow(BattlePrepTextWindow* arg0) {
    arg0->unk_00 = 0;
    arg0->unk_01 = 0;
    arg0->unk_02 = 0;
}

/**
 * Returns the chosen row once the window has finished closing (state 4) and resets it to idle;
 * returns -1 otherwise.
 */
s32 BattlePrep_PollRuleWindow(BattlePrepTextWindow* arg0) {
    if (arg0->unk_00 == 4) {
        arg0->unk_00 = 0;
        return arg0->selected_option;
    }
    return -1;
}
