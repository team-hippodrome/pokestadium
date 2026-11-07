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

static BattlePrepRuleWindowLayout gBattlePrepRuleWindowLayouts[] = {
    {
        78,
        84,
        164,
        72,
        2,
        8481,
        0,
        -1,
        44,
    },
    {
        78,
        91,
        164,
        58,
        2,
        8481,
        0,
        -1,
        44,
    },
    {
        77,
        122,
        166,
        92,
        5,
        8481,
        0,
        -1,
        9362,
    },
    {
        77,
        142,
        166,
        52,
        3,
        8481,
        0,
        -1,
        146,
    },
    {
        77,
        144,
        166,
        52,
        3,
        8481,
        0,
        -1,
        146,
    },
    {
        77,
        149,
        166,
        38,
        2,
        8481,
        0,
        -1,
        18,
    },
    {
        60,
        40,
        200,
        64,
        1,
        8481,
        0,
        -1,
        2,
    },
    {
        60,
        40,
        200,
        64,
        1,
        8481,
        0,
        -1,
        2,
    },
    {
        68,
        32,
        184,
        104,
        1,
        8481,
        1,
        -1,
        2,
    },
    {
        60,
        38,
        200,
        164,
        2,
        26889,
        0,
        -1,
        13,
    },
};

/**
 * Draws the rule window's selection arrow (D_10031D0) beside the selected row, bobbing
 * horizontally on a 12-frame cycle.
 */
void BattlePrep_DrawCursorArrow(BattlePrepRuleWindow* arg0, s16 arg1, s16 arg2) {
    static s16 D_84B17604 = 0;

    if (D_84B17604 < 6) {
        arg1 = (arg1 + (D_84B17604 / 2));
    } else {
        arg1 = ((arg1 - (D_84B17604 / 2)) + 6);
    }

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    gDPLoadTextureBlock(gDisplayListHead++, D_10031D0, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 22, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(arg1, arg2 + (arg0->unk_04 * 0xE) + 1, 0x10, 0xB, 0, 0, 0x800, 0x800, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    D_84B17604 = (D_84B17604 + 1) % 12;
}

/**
 * Prints rule window option arg5 at row arg3: gray when disabled (arg4 == 0), yellow when
 * selected, otherwise white.
 */
void BattlePrep_DrawRuleOption(BattlePrepRuleWindow* arg0, s16 arg1, s16 arg2, s16 arg3, s32 arg4, s8* arg5) {
    if (arg4 == 0) {
        Gfx_SetEnvColor(0x64, 0x64, 0x64, 0xFF);
    } else if (arg3 == arg0->unk_04) {
        Gfx_SetEnvColor(0xFF, 0xFF, 0, 0xFF);
    } else {
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
    }
    Font_Printf(arg1, arg2 + (arg3 * 0xE), arg5);
}

/**
 * Rule window kinds 1 and 2: "Keep battling?" Continue / Save and Quit. Kind 1 (cups) also
 * shows "Continue = N".
 */
void BattlePrep_DrawKeepBattlingPrompt(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1) {
    BattlePrep_DrawCursorArrow(arg0, arg1->unk_00 + 0xC, arg1->unk_02 + 0x18);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);
    Font_Printf(arg1->unk_00 + 0x10, arg1->unk_02 + 6, BattlePrep_GetString(0x29));
    BattlePrep_DrawRuleOption(arg0, arg1->unk_00 + 0x22, arg1->unk_02 + 0x18, 0, 1, BattlePrep_GetString(0x2A));
    BattlePrep_DrawRuleOption(arg0, arg1->unk_00 + 0x22, arg1->unk_02 + 0x18, 1, 1, BattlePrep_GetString(0x2B));

    if (arg1->unk_06 == 0x48) {
        Font_SetActive(1, 0);
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
        Font_Printf(arg1->unk_00 + 0x4A, arg1->unk_02 + 0x36, "%s%d", BattlePrep_GetString(0x2C),
                    gSessionContext.continueCount);
    }

    Font_EndTexturedTextRendering();
}

/**
 * Rule window kind 3: cup loss menu (Continue / Save / Re-enter / From the start / Quit).
 */
void BattlePrep_DrawCupLossMenu(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1) {
    s32 sp2C;
    s32 sp28;

    sp2C = gSessionContext.continueCount > 0;
    sp28 = !!(gSessionContext.sessionFlags & 2);
    sp28 = !sp28;

    BattlePrep_DrawCursorArrow(arg0, arg1->unk_00 + 5, arg1->unk_02 + 6);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);
    BattlePrep_DrawRuleOption(arg0, arg1->unk_00 + 0x1A, arg1->unk_02 + 6, 0, sp2C, BattlePrep_GetString(0x2D));
    BattlePrep_DrawRuleOption(arg0, arg1->unk_00 + 0x1A, arg1->unk_02 + 6, 1, sp2C, BattlePrep_GetString(0x2E));
    BattlePrep_DrawRuleOption(arg0, arg1->unk_00 + 0x1A, arg1->unk_02 + 6, 2, sp28, BattlePrep_GetString(0x2F));
    BattlePrep_DrawRuleOption(arg0, arg1->unk_00 + 0x1A, arg1->unk_02 + 6, 3, 1, BattlePrep_GetString(0x30));
    BattlePrep_DrawRuleOption(arg0, arg1->unk_00 + 0x1A, arg1->unk_02 + 6, 4, 1, BattlePrep_GetString(0x31));
    Font_SetActive(1, 0);
    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
    Font_Printf(arg1->unk_00 + 0x4A, arg1->unk_02 + 0x4C, "%s%d", BattlePrep_GetString(0x2C),
                gSessionContext.continueCount);
    Font_EndTexturedTextRendering();
}

/**
 * Rule window kind 4: castle loss menu (Re-enter / Re-start at this GYM / Quit).
 */
void BattlePrep_DrawCastleLossMenu(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1) {
    s32 sp2C = !!(gSessionContext.sessionFlags & 2);

    sp2C = !sp2C;
    BattlePrep_DrawCursorArrow(arg0, arg1->unk_00 + 5, arg1->unk_02 + 6);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);
    BattlePrep_DrawRuleOption(arg0, arg1->unk_00 + 0x1A, arg1->unk_02 + 6, 0, sp2C, BattlePrep_GetString(0x2F));
    BattlePrep_DrawRuleOption(arg0, arg1->unk_00 + 0x1A, arg1->unk_02 + 6, 1, 1, BattlePrep_GetString(0x32));
    BattlePrep_DrawRuleOption(arg0, arg1->unk_00 + 0x1A, arg1->unk_02 + 6, 2, 1, BattlePrep_GetString(0x31));
    Font_EndTexturedTextRendering();
}

/**
 * Rule window kind 5: Free Battle menu (Try once more / Change team combination / Quit).
 */
void BattlePrep_DrawFreeBattleMenu(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1) {
    s32 sp2C = gSessionContext.battleSides[0].winCount < 0x63 && gSessionContext.battleSides[1].winCount < 0x63;

    BattlePrep_DrawCursorArrow(arg0, arg1->unk_00 + 5, arg1->unk_02 + 6);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);
    BattlePrep_DrawRuleOption(arg0, arg1->unk_00 + 0x1A, arg1->unk_02 + 6, 0, sp2C, BattlePrep_GetString(0x33));
    BattlePrep_DrawRuleOption(arg0, arg1->unk_00 + 0x1A, arg1->unk_02 + 6, 1, 1, BattlePrep_GetString(0x34));
    BattlePrep_DrawRuleOption(arg0, arg1->unk_00 + 0x1A, arg1->unk_02 + 6, 2, 1, BattlePrep_GetString(0x31));
    Font_EndTexturedTextRendering();
}

/**
 * Rule window kind 6: Mewtwo loss menu (Battle again / Quit).
 */
void BattlePrep_DrawMewtwoLossMenu(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1) {
    BattlePrep_DrawCursorArrow(arg0, arg1->unk_00 + 5, arg1->unk_02 + 6);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);
    BattlePrep_DrawRuleOption(arg0, arg1->unk_00 + 0x1A, arg1->unk_02 + 6, 0, 1, BattlePrep_GetString(0x35));
    BattlePrep_DrawRuleOption(arg0, arg1->unk_00 + 0x1A, arg1->unk_02 + 6, 1, 1, BattlePrep_GetString(0x31));
    Font_EndTexturedTextRendering();
}

/**
 * Rule window kind 7: cup name, ball class and BATTLE N label.
 */
void BattlePrep_DrawCupProgressHeader(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1) {
    char* temp_v0;

    Font_BeginTranslucentTextRendering();
    Font_EnableTwoCycleTexturing();
    Font_SetActive(4, 0);
    Gfx_SetPrimColor(0x32, 0xFF, 0x64, 0xFF);

    if ((gSessionContext.mode == 3) || (gSessionContext.mode == 6)) {
        temp_v0 = BattlePrep_GetString(gSessionContext.mode - 1);
        Font_Printf((arg1->unk_00 - (Font_MeasureTextExtent(4, 0, temp_v0) / 2)) + 0x64, arg1->unk_02 + 6, temp_v0);
        temp_v0 = BattlePrep_GetString(gSessionContext.modeSelection + 7);
        Font_Printf((arg1->unk_00 - (Font_MeasureTextExtent(4, 0, temp_v0) / 2)) + 0x64, arg1->unk_02 + 0x18, temp_v0);
        temp_v0 = BattlePrep_GetString(gSessionContext.roundOrDemoId + 0xA);
        Font_Printf((arg1->unk_00 - (Font_MeasureTextExtent(4, 0, temp_v0) / 2)) + 0x64, arg1->unk_02 + 0x2A, temp_v0);
    } else {
        temp_v0 = BattlePrep_GetString(gSessionContext.mode - 1);
        Font_Printf((arg1->unk_00 - (Font_MeasureTextExtent(4, 0, temp_v0) / 2)) + 0x64, arg1->unk_02 + 0xA, temp_v0);
        temp_v0 = BattlePrep_GetString(gSessionContext.roundOrDemoId + 0xA);
        Font_Printf((arg1->unk_00 - (Font_MeasureTextExtent(4, 0, temp_v0) / 2)) + 0x64, arg1->unk_02 + 0x26, temp_v0);
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
void BattlePrep_DrawCastleProgressHeader(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1) {
    s32 sp12C;
    char sp2C[0x100];

    Font_BeginTranslucentTextRendering();
    Font_EnableTwoCycleTexturing();
    Font_SetActive(4, 0);
    Gfx_SetPrimColor(0x32, 0xFF, 0x64, 0xFF);

    sprintf(sp2C, "%s %s", BattlePrep_GetString(gSessionContext.modeSelection + 0x13), BattlePrep_GetString(0x36));
    sp12C = Font_MeasureTextExtent(4, 0, sp2C);
    Font_Printf((arg1->unk_00 - (sp12C / 2)) + 0x64, arg1->unk_02 + 0xA, sp2C);

    if (gSessionContext.modeSelection < 9) {
        if (gSessionContext.modeSelection == 8) {
            sprintf(sp2C, "%s %d", BattlePrep_GetString(0x1B), gSessionContext.roundOrDemoId);
            sp12C = Font_MeasureTextExtent(4, 0, sp2C);
            Font_Printf((arg1->unk_00 - (sp12C / 2)) + 0x64, arg1->unk_02 + 0x26, sp2C);
        } else if (gSessionContext.roundOrDemoId < 4) {
            sprintf(&sp2C, "%s %d", BattlePrep_GetString(0x37), gSessionContext.roundOrDemoId);
            sp12C = Font_MeasureTextExtent(4, 0, sp2C);
            Font_Printf((arg1->unk_00 - (sp12C / 2)) + 0x64, arg1->unk_02 + 0x26, sp2C);
        } else {
            sp12C = Font_MeasureTextExtent(4, 0, BattlePrep_GetString(gSessionContext.modeSelection + 0x1D));
            Font_Printf((arg1->unk_00 - (sp12C / 2)) + 0x64, arg1->unk_02 + 0x26,
                        BattlePrep_GetString(gSessionContext.modeSelection + 0x1D));
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
        if (arg2->tagTeamMember[0]->isHuman & 2) {
            Gfx_DrawTextureRgba16(arg0 + 0x1A, arg1 + 3, 0x24, 0x1A, D_3025200, 0x24, 0);
            if (0) {}
        } else {
            Gfx_DrawTextureRgba16(arg0 + 0x1A, arg1 + 3, 0x24, 0x1A, D_84B17608[arg2->tagTeamMember[0]->controllerPort],
                                  0x24, 0);
        }

        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
        return;
    }

    if (arg2->tagTeamMember[0]->controllerPort < arg2->tagTeamMember[1]->controllerPort) {
        Gfx_DrawTextureRgba16(arg0 + 6, arg1 + 3, 0x24, 0x1A, D_84B17608[arg2->tagTeamMember[0]->controllerPort], 0x24, 0);
        Gfx_DrawTextureRgba16(arg0 + 0x30, arg1 + 3, 0x24, 0x1A, D_84B17608[arg2->tagTeamMember[1]->controllerPort], 0x24,
                              0);
    } else {
        Gfx_DrawTextureRgba16(arg0 + 6, arg1 + 3, 0x24, 0x1A, D_84B17608[arg2->tagTeamMember[1]->controllerPort], 0x24, 0);
        Gfx_DrawTextureRgba16(arg0 + 0x30, arg1 + 3, 0x24, 0x1A, D_84B17608[arg2->tagTeamMember[0]->controllerPort], 0x24,
                              0);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    Font_BeginTranslucentTextRendering();
    Font_SetActive(4, 0);
    Font_DrawCharAt(arg0 + 0x24, arg1 + 8, 0x26);
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
void BattlePrep_DrawWinRecord(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1) {
    BattlePrep_DrawBorderedRect(arg1->unk_00 + 8, arg1->unk_02 + 0x14, 0xA8, 0x20, 0, 0, 0, 0xFF);
    BattlePrep_DrawBorderedRect(arg1->unk_00 + 8, arg1->unk_02 + 0x44, 0xA8, 0x20, 0, 0, 0, 0xFF);
    BattlePrep_DrawPlayerTag(arg1->unk_00 + 8, arg1->unk_02 + 0x14, gSessionContext.battleSides);
    BattlePrep_DrawPlayerTag(arg1->unk_00 + 8, arg1->unk_02 + 0x44, &gSessionContext.battleSides[1]);
    Font_BeginTranslucentTextRendering();
    Font_EnableTwoCycleTexturing();
    Font_SetActive(4, 0);
    Gfx_SetPrimColor(0x32, 0xFF, 0x64, 0xFF);
    Font_Printf(arg1->unk_00 + 0x40, arg1->unk_02 + 2, BattlePrep_GetString(0x38));
    Font_DisableTwoCycleTexturing();
    Font_SetActive(0x10, 0);
    BattlePrep_SetWinCountColor(&gSessionContext.battleSides[0]);
    Font_Printf(arg1->unk_00 + 0x5C, arg1->unk_02 + 0x18, "%2d", gSessionContext.battleSides[0].winCount);
    BattlePrep_SetWinCountColor(&gSessionContext.battleSides[1]);
    Font_Printf(arg1->unk_00 + 0x5C, arg1->unk_02 + 0x48, "%2d", gSessionContext.battleSides[1].winCount);
    Font_SetActive(4, 0);
    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
    Font_Printf(arg1->unk_00 + 0x82, arg1->unk_02 + 0x1F, BattlePrep_GetString(0x39));
    Font_Printf(arg1->unk_00 + 0x82, arg1->unk_02 + 0x4F, BattlePrep_GetString(0x39));
    Font_EndTexturedTextRendering();
}

#if defined(VERSION_US_REV2)
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/battle_prep/battle_prep_3A1C30/BattlePrep_DrawOverwriteSavePrompt.s")
#else
/**
 * Rule window kind 10: overwrite-saved-battle-data prompt.
 */
void BattlePrep_DrawOverwriteSavePrompt(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1) {
    SessionContinueData sp50;
    UNUSED s32 pad;
    s32 sp48;
    s32 temp_v0;

    Session_LoadContinueRecord(&sp50, NULL);
    BattlePrep_DrawBorderedRect(arg1->unk_00 + 0x10, arg1->unk_02 + 0x14, 0xA8, 0x3C, 0, 0, 0, 0xFF);
    BattlePrep_DrawBorderedRect(arg1->unk_00 + 0x10, arg1->unk_02 + 0x60, 0xA8, 0x22, 0, 0, 0, 0xFF);
    BattlePrep_DrawCursorArrow(arg0, arg1->unk_00 + 0x14, arg1->unk_02 + 0x64);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);
    Font_Printf(arg1->unk_00 + 0x10, arg1->unk_02 + 6, BattlePrep_GetString(0x3A));
    Font_Printf(arg1->unk_00 + 0x10, arg1->unk_02 + 0x52, BattlePrep_GetString(0x3B));
    BattlePrep_DrawRuleOption(arg0, arg1->unk_00 + 0x2A, arg1->unk_02 + 0x64, 0, 1, BattlePrep_GetString(0x3C));
    BattlePrep_DrawRuleOption(arg0, arg1->unk_00 + 0x2A, arg1->unk_02 + 0x64, 1, 1, BattlePrep_GetString(0x3D));
    Gfx_SetEnvColor(0xFF, 0x64, 0x64, 0xFF);

    if (sp50.unk_00 == 7) {
        Font_Printf(arg1->unk_00 + 0x1E, arg1->unk_02 + 0x17, BattlePrep_GetString(sp50.unk_00 - 1));
        Font_Printf(arg1->unk_00 + 0x1E, arg1->unk_02 + 0x26, BattlePrep_GetString(sp50.unk_01 + 0x13));
        if (sp50.unk_01 < 8) {
            if (sp50.unk_02 < 4) {
                Font_Printf(arg1->unk_00 + 0x1E, arg1->unk_02 + 0x35, "%s %d", BattlePrep_GetString(0x37), sp50.unk_02);
            } else {
                Font_Printf(arg1->unk_00 + 0x1E, arg1->unk_02 + 0x35, BattlePrep_GetString(0x3E));
            }
        } else {
            Font_Printf(arg1->unk_00 + 0x1E, arg1->unk_02 + 0x35, "%s %d", BattlePrep_GetString(0x1B), sp50.unk_02);
        }
    } else if ((sp50.unk_00 == 3) || (sp50.unk_00 == 6)) {
        Font_Printf(arg1->unk_00 + 0x1E, arg1->unk_02 + 0x17, BattlePrep_GetString(sp50.unk_00 - 1));
        Font_Printf(arg1->unk_00 + 0x1E, arg1->unk_02 + 0x26, BattlePrep_GetString(sp50.unk_01 + 7));
        Font_Printf(arg1->unk_00 + 0x1E, arg1->unk_02 + 0x35, BattlePrep_GetString(sp50.unk_02 + 0xA));
    } else {
        Font_Printf(arg1->unk_00 + 0x1E, arg1->unk_02 + 0x17, BattlePrep_GetString(sp50.unk_00 - 1));
        Font_Printf(arg1->unk_00 + 0x1E, arg1->unk_02 + 0x26, BattlePrep_GetString(sp50.unk_02 + 0xA));
    }

    if (sp50.unk_00 != 7) {
        Font_SetActive(1, 0);
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
        Font_Printf(arg1->unk_00 + 0x64, arg1->unk_02 + 0x43, "%s %d", BattlePrep_GetString(0x2C), sp50.unk_04);
    }

    if (arg0->unk_04 == 0) {
        sp48 = (arg1->unk_04 - Font_MeasureTextExtent(1, 0, BattlePrep_GetString(0x3F))) / 2;
        Font_SetActive(1, 0);
        Gfx_SetEnvColor(0xFF, 0xFF, 0, 0xFF);
        Font_SetLineHeight(0xC);
        Font_Printf(arg1->unk_00 + sp48, arg1->unk_02 + 0x86, BattlePrep_GetString(0x3F));
    }

    Font_EndTexturedTextRendering();

    if (sp50.unk_03 == 1) {
        temp_v0 = Font_MeasureTextExtent(1, 0, BattlePrep_GetString(sp50.unk_00 - 1));

        gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

        Gfx_DrawTextureRgba16(arg1->unk_00 + temp_v0 + 0x20, arg1->unk_02 + 0x15, 0x10, 0x10, D_3027690, 0x10, 0);

        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
    }
}
#endif

/**
 * Draws the active rule window: grows the panel vertically while opening (unk_02 < 4), then draws
 * the full panel and the contents for window kind unk_01 (1-10).
 */
void BattlePrep_DrawRuleWindow(BattlePrepRuleWindow* arg0) {
    BattlePrepRuleWindowLayout* temp_s0;
    s16 temp_a1;
    s16 var_a3;

    if ((arg0->unk_00 != 0) && (arg0->unk_02 > 0)) {
        temp_s0 = &gBattlePrepRuleWindowLayouts[arg0->unk_01 - 1];

        if (arg0->unk_02 < 4) {
            var_a3 = (temp_s0->unk_06 * arg0->unk_02) / 4;
            if (var_a3 < 0x10) {
                var_a3 = 0x10;
            }

            temp_a1 = temp_s0->unk_02 + ((temp_s0->unk_06 - var_a3) / 2);
            if (temp_s0->unk_0C == 0) {
                Ui_DrawBorderedPanel(temp_s0->unk_00, temp_a1, temp_s0->unk_04, var_a3, temp_s0->unk_0A);
            } else {
                BattlePrep_DrawBorderedRect(temp_s0->unk_00, temp_a1, temp_s0->unk_04, var_a3, 0x1E, 0x1E, 0x82, 0xFF);
            }
        } else {
            if (temp_s0->unk_0C == 0) {
                Ui_DrawBorderedPanel(temp_s0->unk_00, temp_s0->unk_02, temp_s0->unk_04, temp_s0->unk_06,
                                     temp_s0->unk_0A);
            } else {
                BattlePrep_DrawBorderedRect(temp_s0->unk_00, temp_s0->unk_02, temp_s0->unk_04, temp_s0->unk_06, 0x1E,
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
void BattlePrep_RunQuitMenu(BattlePrepRuleWindow* arg0) {
    BattlePrepRuleWindowLayout* sp24;
    s16 sp22;
    s32 tmp;

    sp24 = &gBattlePrepRuleWindowLayouts[arg0->unk_01 - 1];
    sp22 = sp24->unk_08 - 1;

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
        tmp = sp24->unk_10 >> (arg0->unk_04 * 3);
        switch (tmp & 7) {
            case 1:
                Audio_PlaySoundEffectById(SFX_MENU_BACK);
                break;

            case 2:
                Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
                break;

            case 3:
                Audio_PlaySoundEffectById(0x1C);
                break;

            case 4:
                Audio_PlaySoundEffectById(0x20);
                break;

            case 5:
                Audio_PlaySoundEffectById(0x1D);
                break;

            case 6:
                Audio_PlaySoundEffectById(0x21);
                break;
        }
        arg0->unk_00 = 2;
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DUP)) {
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
        do {
            arg0->unk_04--;
            if (arg0->unk_04 < 0) {
                arg0->unk_04 = sp22;
            }
        } while (!(sp24->unk_0E & (1 << arg0->unk_04)));
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DDOWN)) {
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
        do {
            arg0->unk_04++;
            if (sp22 < arg0->unk_04) {
                arg0->unk_04 = 0;
            }
        } while (!(sp24->unk_0E & (1 << arg0->unk_04)));
    }
}

/**
 * State 1: increments the open step each frame until 4, then enters the input state 3.
 */
void BattlePrep_RuleWindowExpanding(BattlePrepRuleWindow* arg0) {
    arg0->unk_02++;
    if (arg0->unk_02 >= 5) {
        arg0->unk_02 = 4;
        arg0->unk_00 = 3;
    }
}

/**
 * State 2: decrements the open step each frame until 0, then enters the closed state 4.
 */
void BattlePrep_RuleWindowCollapsing(BattlePrepRuleWindow* arg0) {
    arg0->unk_02--;
    if (arg0->unk_02 < 0) {
        arg0->unk_02 = 0;
        arg0->unk_00 = 4;
    }
}

/**
 * Per-frame dispatcher for the rule window state machine (1 expanding, 2 collapsing, 3 input).
 */
void BattlePrep_UpdateRuleWindow(BattlePrepRuleWindow* arg0) {
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
void BattlePrep_InitRuleWindow(BattlePrepRuleWindow* arg0, s16 arg1) {
    BattlePrepRuleWindowLayout* ptr = &gBattlePrepRuleWindowLayouts[arg1 - 1];

    arg0->unk_01 = arg1;
    arg0->unk_02 = 0;
    arg0->unk_04 = 0;
    arg0->unk_00 = 1;

    ptr->unk_0E = -1;

    Audio_PlaySoundEffectById(SFX_PANEL_OPEN);

    if (arg1 - 1 == 2) {
        if (gSessionContext.continueCount == 0) {
            ptr->unk_0E &= ~3;
        }
        if (gSessionContext.sessionFlags & 2) {
            ptr->unk_0E &= ~4;
        }
    } else if (arg1 - 1 == 3) {
        if (gSessionContext.sessionFlags & 2) {
            ptr->unk_0E &= ~1;
        }
    } else if ((arg1 - 1 == 4) &&
               ((gSessionContext.battleSides[0].winCount >= 0x63) || (gSessionContext.battleSides[1].winCount >= 0x63))) {
        ptr->unk_0E &= ~1;
    }

    while (!(ptr->unk_0E & (1 << arg0->unk_04))) {
        arg0->unk_04++;
    }
}

/**
 * Immediately hides the rule window by clearing its state, kind and open step.
 */
void BattlePrep_CloseRuleWindow(BattlePrepRuleWindow* arg0) {
    arg0->unk_00 = 0;
    arg0->unk_01 = 0;
    arg0->unk_02 = 0;
}

/**
 * Returns the chosen row once the window has finished closing (state 4) and resets it to idle;
 * returns -1 otherwise.
 */
s32 BattlePrep_PollRuleWindow(BattlePrepRuleWindow* arg0) {
    if (arg0->unk_00 == 4) {
        arg0->unk_00 = 0;
        return arg0->unk_04;
    }
    return -1;
}
