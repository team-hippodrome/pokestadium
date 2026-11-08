#include "free_battle_rules.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/ui_graphics.h"
#include "src/pokemon_stats.h"
#include "src/game_state.h"
#include "src/session.h"
#include "src/text_system.h"
#include "src/jpeg_stream.h"
#include "src/audio_sfx.h"
#include "src/DDC0.h"
#include "src/matrix.h"
#include "src/controller.h"
#include "src/memory.h"
#include "src/stage_loader.h"

typedef struct unk_D_84002D18 {
    /* 0x00 */ s16 leftSideTeam;
    /* 0x02 */ s16 rightSideTeam;
} TagTeamSelection; // size = 0x4

typedef struct unk_D_84002D28 {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
} unk_D_84002D28; // size >= 0xA

typedef struct unk_D_84002BE8 {
    /* 0x00 */ s16 state;
    /* 0x02 */ s16 defaultSide2Port; // -1 = not chosen, 0 = P2, 1 = P3
    /* 0x04 */ s16 tagTeamCursorPosition;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ u8* rulesetCursorPosition;
    /* 0x0C */ u8* unk_0C;
} FreeBattleSettingsState; // size = 0x10

#define FB_STATE_SELECT_LEFT_SIDE 1
#define FB_STATE_SELECT_RIGHT_SIDE 2
#define FB_STATE_SELECT_RULESET 3
#define FB_STATE_SELECT_COM_DIFFICULTY 4
#define FB_STATE_OPENING_COM_DIFFICULTY_WINDOW 5
#define FB_STATE_CLOSING_COM_DIFFICULTY_WINDOW 6
#define FB_STATE_CONFIRM 7
#define FB_STATE_EXIT 8

static char** sRuleSelectText;
static char** sPokemonDetailText;
#if defined(VERSION_US_REV2)
static s32 pad_D_84002D08_us_rev2;
#endif
static BinArchive* D_84002D08;
static u8* D_84002D0C;
static s32 pad_D_84002D10;
static s16 D_84002D14;
static s16 sFreeBattleRuleState;
#if defined(VERSION_US_REV2)
static s32 pad_D_84002D18_us_rev2;
#endif
static TagTeamSelection sTagTeamSelection;
static s32 pad_D_84002D1C;
static s16 sHoveredRuleset;
static unk_D_84002D28 D_84002D28;

static s16 D_84002BE0 = 2;
static s16 D_84002BE4 = 0;
static FreeBattleSettingsState sTagTeamOptions[11] = {
    {
        1,
        0,
        48,
        6,
        0x03008FD0,
        NULL,
    },
    {
        3,
        0,
        48,
        36,
        0x03008FD0,
        0x03009720,
    },
    {
        5,
        0,
        48,
        66,
        0x03008FD0,
        0x03009E70,
    },
    {
        9,
        0,
        48,
        96,
        0x03008FD0,
        0x0300A5C0,
    },
    {
        2,
        15,
        280,
        6,
        0x03009720,
        NULL,
    },
    {
        4,
        15,
        280,
        36,
        0x03009E70,
        NULL,
    },
    {
        8,
        15,
        280,
        66,
        0x0300A5C0,
        NULL,
    },
    {
        0,
        15,
        280,
        96,
        0x03008880,
        NULL,
    },
    {
        6,
        15,
        336,
        6,
        0x03009720,
        0x03009E70,
    },
    {
        0xA,
        15,
        336,
        36,
        0x03009720,
        0x0300A5C0,
    },
    {
        0xC,
        15,
        336,
        66,
        0x03009E70,
        0x0300A5C0,
    },
};
static s16 D_84002C98[] = { 0, 3, 4, 5, 6, 0 };

void FreeBattle_DrawHeaderImage(s16 x, s16 y) {
    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    Gfx_DrawTextureRgba16(x, y, 48, 24, gPokeBallIconTop, 48, 0);
    Gfx_DrawTextureRgba16(x, y + 24, 48, 24, gPokeBallIconBottom, 48, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

// Better name: FreeBattle_DrawVsIcon
void FreeBattle_DrawBannerImage(s16 x, s16 y) {
    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    Gfx_DrawTextureRgba16(x, y, 100, 20, gVsTop, 100, 0);
    Gfx_DrawTextureRgba16(x, y + 20, 100, 20, gVsMiddle, 100, 0);
    Gfx_DrawTextureRgba16(x, y + 40, 100, 16, gVsBottom, 100, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void FreeBattle_DrawTintedBar(s16 border_x, s16 y, s16 x, Color_RGBA8* env_color, Color_RGBA8* prim_color) {
    gSPDisplayList(gDisplayListHead++, D_8006F558);
    gDPSetEnvColor(gDisplayListHead++, env_color->r, env_color->g, env_color->b, 255);
    gDPSetPrimColor(gDisplayListHead++, 0, 0, prim_color->r, prim_color->g, prim_color->b, 255);

    Gfx_DrawTextureIa8(border_x, y, 16, 32, D_20003C0, 0x10, 0);
    Gfx_DrawTextureIa8((border_x + x) - 16, y, 16, 32, D_20005C0, 0x10, 0);
    Gfx_DrawTexturedRectClipped(border_x + 16, y, x - 32, 32, 0, 0, 0, 0x400, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void FreeBattle_DrawTitleBar(void) {
    Color_RGBA8 fill_color;
    Color_RGBA8 border_color;

    Color_SetRGB(&fill_color, 0x64, 0x64, 0xC8); // #6464C8
    Color_SetRGB(&border_color, 0x28, 0x28, 0x8C); // #28288C
    FreeBattle_DrawTintedBar(72, 40, 520, &fill_color, &border_color);
    FreeBattle_DrawHeaderImage(48, 32);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(0x10, 0);
    Gfx_SetEnvColor(0, 0, 0, 0xFF); // #000000
    Font_Printf(106, 44, Text_GetString(NULL, 0, sRuleSelectText, 0));
    Gfx_SetEnvColor(0xFF, 0xFF, 0x77, 0xFF); // #FFFF77
    Font_Printf(104, 42, Text_GetString(NULL, 0, sRuleSelectText, 0));
    Font_EndTexturedTextRendering();
}

void FreeBattle_DrawSelectionCorners(s16 left, s16 bottom, s16 right, s16 top) {
    static s16 D_84002CA4 = 0;

    s16 sp56 = SINS(D_84002CA4) * 2;
    UNUSED s32 pad[2];

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetEnvColor(gDisplayListHead++, 0xF0, 0xD4, 0x68, 255);

    Gfx_DrawTextureIa8(left + sp56, bottom + sp56, 0x10, 0x10, gBottomLeftSelectionCorner, 0x10, 0);
    Gfx_DrawTextureIa8(left + sp56, ((bottom + top) - sp56) - 0x10, 0x10, 0x10, gTopLeftSelectionCorner, 0x10, 0);
    Gfx_DrawTextureIa8(((left + right) - sp56) - 0x10, bottom + sp56, 0x10, 0x10, gBottomRightSelectionCorner, 0x10, 0);
    Gfx_DrawTextureIa8(((left + right) - sp56) - 0x10, ((bottom + top) - sp56) - 0x10, 0x10, 0x10, gTopRightSelectionCorner, 0x10, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    D_84002CA4 += 0x2000;
}

void FreeBattle_DrawPanelFrame(s16 left, s16 top, s16 right, s16 bottom) {
    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    Gfx_DrawTextureIa8(left - 3, top - 3, 8, 8, D_2000340, 8, 0);
    Gfx_DrawTextureIa8((left + right) - 5, top - 3, 8, 8, D_2000380, 8, 0);
    Gfx_DrawTexturedRectClipped(left + 4, top - 3, right - 8, 8, 0, 0, 0, 0x400, 0);
    Gfx_DrawTextureIa8(left - 3, (top + bottom) - 5, 8, 8, D_20002C0, 8, 0);
    Gfx_DrawTexturedRectClipped(left - 3, top + 4, 8, bottom - 8, 0, 0, 0x400, 0, 0);
    Gfx_DrawTextureIa8((left + right) - 5, (top + bottom) - 5, 8, 8, D_2000300, 8, 0);
    Gfx_DrawTexturedRectClipped(left + 4, (top + bottom) - 5, right - 8, 8, 0, 0, 0, 0x400, 0);
    Gfx_DrawTexturedRectClipped((left + right) - 5, top + 4, 8, bottom - 8, 0, 0, 0x400, 0, 0);

    gSPDisplayList(gDisplayListHead++, D_8006F470);

    Gfx_FillRectRgb(left + 4, top + 4, right - 8, bottom - 8, 0x3C, 0x3C, 0xA0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

// FreeBattle_DrawShadowForPortsWithNoPlayer
void FreeBattle_DrawSlicedFrame(s16 x, s16 y, s16 raw_width, s16 raw_height, u8 r, u8 g, u8 b, u8 alpha) {
    s16 width;
    s16 height;
    s16 height_adjustment;
    s16 width_adjustment;

    if (raw_width >= 32) {
        width_adjustment = 16;
    } else {
        width_adjustment = raw_width / 2;
    }

    if (raw_height >= 32) {
        height_adjustment = 16;
    } else {
        height_adjustment = raw_height / 2;
    }
    width = raw_width - (width_adjustment * 2);
    height = raw_height - (height_adjustment * 2);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetEnvColor(gDisplayListHead++, r, g, b, alpha);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_20288E0, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(x, y, width_adjustment, height_adjustment, 0, 0, 0x4000 / width_adjustment, 0x4000 / height_adjustment, 0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_2028960, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped((x + raw_width) - width_adjustment, y, width_adjustment, height_adjustment, 0, 0, 0x4000 / width_adjustment, 0x4000 / height_adjustment,
                                0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_20287E0, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(x, (y + raw_height) - height_adjustment, width_adjustment, height_adjustment, 0, 0, 0x4000 / width_adjustment, 0x4000 / height_adjustment,
                                0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_2028860, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped((x + raw_width) - width_adjustment, (y + raw_height) - height_adjustment, width_adjustment, height_adjustment, 0, 0, 0x4000 / width_adjustment,
                                0x4000 / height_adjustment, 0);

    gSPDisplayList(gDisplayListHead++, gUiTranslucentFillDL);

    if ((width > 0) && (height > 0)) {
        Gfx_FillRectRgba(x + width_adjustment, y, width, height_adjustment, r, g, b, alpha);
        Gfx_FillRectRgba(x, y + width_adjustment, raw_width, height, r, g, b, alpha);
        Gfx_FillRectRgba(x + width_adjustment, y + width_adjustment + height, width, height_adjustment, r, g, b, alpha);
    } else if (width > 0) {
        Gfx_FillRectRgba(x + width_adjustment, y, width, raw_height, r, g, b, alpha);
    } else if (height > 0) {
        Gfx_FillRectRgba(x, y + height_adjustment, raw_width, height, r, g, b, alpha);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

// better name: FreeBattle_IsTagTeamPossible
s32 FreeBattle_IsRuleAvailable(s16 option_index) {
    FreeBattleSettingsState* option = &sTagTeamOptions[option_index];
    UNUSED s16 t = option->state;
    s32 is_available = 0;

    if (option_index < 4) {
        if (sFreeBattleRuleState >= 2) {
            is_available = option_index == sTagTeamSelection.leftSideTeam;
        } else {
            is_available = option->state == (option->state & D_84002D14);
        }
    } else if (option_index < 11) {
        if (sFreeBattleRuleState >= 3) {
            is_available = option_index == (sTagTeamSelection.rightSideTeam + 4);
        } else {
            s16 tmp = (~sTagTeamOptions[sTagTeamSelection.leftSideTeam].state & D_84002D14);

            is_available = option->state == (option->state & tmp);
        }
    }
    return is_available;
}

// better name: FreeBattle_DrawAmpersand
void FreeBattle_DrawRuleMark(s16 x, s16 y, s16 option_index) {
    if (FreeBattle_IsRuleAvailable(option_index) != 0) {
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #FFFFFF
    } else {
        Gfx_SetEnvColor(0x64, 0x64, 0x64, 0x82); // #646464
    }
    Font_DrawCharAt(x, y, 38);
}

void FreeBattle_DrawRuleGrid(void) {
    s32 i;
    s16 temp_s2;
    s16 temp_s3;
    s32 player_icon_color_overlay;
    FreeBattleSettingsState* side_selection;
    s16 tmp1;
    s16 tmp2;

    FreeBattle_DrawSlicedFrame(100, 114, 432, 0x80, 0, 0, 0, 0xFF);
    FreeBattle_DrawBannerImage(266, 150);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    for (i = 0; i < 11; i++) {
        temp_s2 = sTagTeamOptions[i].tagTeamCursorPosition + 100;
        temp_s3 = sTagTeamOptions[i].unk_06 + 114;

        if (FreeBattle_IsRuleAvailable(i) != 0) {
            player_icon_color_overlay = 0xFF; // #FFFFFF
        } else {
            player_icon_color_overlay = 0x82; // #828282
        }

        gDPSetEnvColor(gDisplayListHead++, player_icon_color_overlay, player_icon_color_overlay, player_icon_color_overlay, player_icon_color_overlay);

        Gfx_DrawTextureRgba16(temp_s2, temp_s3, 36, 26, sTagTeamOptions[i].rulesetCursorPosition, 36, 0);
        if (sTagTeamOptions[i].unk_0C != NULL) {
            Gfx_DrawTextureRgba16(temp_s2 + 48, temp_s3, 36, 26, sTagTeamOptions[i].unk_0C, 36, 0);
        }
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    Font_BeginTranslucentTextRendering();
    Font_SetActive(0x10, 0);

    for (i = 0; i < 11; i++) {
        if (sTagTeamOptions[i].unk_0C != NULL) {
            FreeBattle_DrawRuleMark(sTagTeamOptions[i].tagTeamCursorPosition + 135, sTagTeamOptions[i].unk_06 + 115, i);
        }
    }

    Font_SetActive(0x10, 0);

    if ((sFreeBattleRuleState == 1) || (sFreeBattleRuleState == 2)) {
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #FFFFFF
    } else {
        Gfx_SetEnvColor(0x78, 0x78, 0xC8, 0xFF); // #7878C8
    }

    Font_Printf(100, 90, Text_GetString(NULL, 0, sRuleSelectText, 1));
    Font_EndTexturedTextRendering();

    if (sFreeBattleRuleState == FB_STATE_SELECT_LEFT_SIDE) {
        side_selection = &sTagTeamOptions[sTagTeamSelection.leftSideTeam];
        tmp1 = side_selection->tagTeamCursorPosition + 100;
        tmp2 = side_selection->unk_06 + 114;
        FreeBattle_DrawSelectionCorners(tmp1 - 4, tmp2 - 2, ((side_selection->unk_0C == NULL) ? 36 : 84) + 8, 30);
    } else if (sFreeBattleRuleState == FB_STATE_SELECT_RIGHT_SIDE) {
        side_selection = &sTagTeamOptions[sTagTeamSelection.rightSideTeam];
        tmp1 = side_selection[4].tagTeamCursorPosition + 100;
        tmp2 = side_selection[4].unk_06 + 114;
        FreeBattle_DrawSelectionCorners(tmp1 - 4, tmp2 - 2, ((side_selection[4].unk_0C == NULL) ? 36 : 84) + 8, 30);
    }
}

void FreeBattle_UpdateRuleRowSelect(void) {
    s32 left_side_team = sTagTeamSelection.leftSideTeam;

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
        Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
        sTagTeamSelection.rightSideTeam = 0;
        while (FreeBattle_IsRuleAvailable(sTagTeamSelection.rightSideTeam + 4) == 0) {
            sTagTeamSelection.rightSideTeam++;
        }
        sFreeBattleRuleState = FB_STATE_SELECT_RIGHT_SIDE;
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_B)) {
        Audio_PlaySoundEffectById(SFX_MENU_BACK);
        sFreeBattleRuleState = FB_STATE_EXIT;
    } else {
        do {
            if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DUP)) {
                sTagTeamSelection.leftSideTeam = (sTagTeamSelection.leftSideTeam + 3) % 4;
            } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DDOWN)) {
                sTagTeamSelection.leftSideTeam = (sTagTeamSelection.leftSideTeam + 1) % 4;
            }
        } while (FreeBattle_IsRuleAvailable(sTagTeamSelection.leftSideTeam) == 0);

        if (left_side_team != sTagTeamSelection.leftSideTeam) {
            Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
        }
    }
}

void FreeBattle_UpdateRuleOptionSelect(void) {
    s32 sp34 = sTagTeamSelection.rightSideTeam;
    s32 i;
    s32 var_a0;
    s32 var_s1;
    s32 var_s4;

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
        Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
        if (sTagTeamSelection.rightSideTeam == 3) {
            sFreeBattleRuleState = 5;
        } else {
            sHoveredRuleset = 0;
            sFreeBattleRuleState = 3;
        }
        return;
    }

    var_s4 = 0;
    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_B)) {
        Audio_PlaySoundEffectById(SFX_MENU_BACK);
        sTagTeamSelection.rightSideTeam = -1;
        sFreeBattleRuleState = 1;
        return;
    }

    var_a0 = sTagTeamSelection.rightSideTeam / 4;
    var_s1 = sTagTeamSelection.rightSideTeam % 4;

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DLEFT) && (var_a0 == 1)) {
        var_a0 = 0;
        var_s4 = 1;
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DRIGHT) && (var_a0 == 0)) {
        var_a0 = 1;
        var_s4 = 1;
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DUP)) {
        var_s1 = (var_s1 + 3) % 4;
        var_s4 = -1;
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DDOWN)) {
        var_s1 = (var_s1 + 1) % 4;
        var_s4 = 1;
    }

    if (var_s4 != 0) {
        for (i = 0; i < 4; i++) {
            if (FreeBattle_IsRuleAvailable(var_a0 * 4 + var_s1 + 4) != 0) {
                break;
            }
            var_s1 = (var_s1 + var_s4 + 4) % 4;
        }

        if (i != 4) {
            sTagTeamSelection.rightSideTeam = var_a0 * 4 + var_s1;
        }
    }

    if (sp34 != sTagTeamSelection.rightSideTeam) {
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
    }
}

void FreeBattle_ClearRuleSelection(void) {
    sTagTeamSelection.leftSideTeam = -1;
    sTagTeamSelection.rightSideTeam = -1;
}

s32 FreeBattle_SelectionHasSubrule(void) {
    s32 var_v1 = 0;

    if ((sTagTeamSelection.leftSideTeam != -1) && (sTagTeamOptions[sTagTeamSelection.leftSideTeam].unk_0C != 0)) {
        var_v1 = 1;
    }

    if ((sTagTeamSelection.rightSideTeam != -1) && (sTagTeamOptions[sTagTeamSelection.rightSideTeam + 4].unk_0C != 0)) {
        var_v1 = 1;
    }
    return var_v1;
}

// Better name: FreeBattle_DrawRulesetOptionText
void FreeBattle_DrawLevelOptionLine(s16 box_x, s16 box_y, s16 option_index, char* text) {
    if ((option_index != 0) && (FreeBattle_SelectionHasSubrule() != 0)) {
        Gfx_SetEnvColor(0x3C, 0x3C, 0x3C, 0xFF); // #3c3c3c
    } else if (option_index == sHoveredRuleset) {
        Gfx_SetEnvColor(0xFF, 0xFF, 0, 0xFF); // #ffff00
    } else {
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #ffffff
    }
    Font_Printf(box_x, box_y + (option_index * 24), text);
}

void func_84001AD8(void) {
}

// better name: FreeBattle_DrawRulesetSelectPanel
void FreeBattle_DrawLevelSelectPanel(void) {
    UNUSED s32 pad;
    char buffer[128];

    FreeBattle_DrawSlicedFrame(100, 268, 432, 132, 0, 0, 0, 0xFF);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(8, 0);
    FreeBattle_DrawLevelOptionLine(148, 276, 0, Text_GetString(NULL, 0, sRuleSelectText, 2));
    FreeBattle_DrawLevelOptionLine(148, 276, 1, Text_GetString(NULL, 0, sRuleSelectText, 3));
    FreeBattle_DrawLevelOptionLine(148, 276, 2, Text_GetString(NULL, 0, sRuleSelectText, 6));
    FreeBattle_DrawLevelOptionLine(148, 276, 3, Text_GetString(NULL, 0, sRuleSelectText, 7));
    FreeBattle_DrawLevelOptionLine(148, 276, 4, Text_GetString(NULL, 0, sRuleSelectText, 8));
    Font_SetActive(4, 0);

    sprintf(buffer, "%s1~100", Text_GetString(NULL, 0, sPokemonDetailText, 21));
    FreeBattle_DrawLevelOptionLine(430, 280, 0, buffer);
    sprintf(buffer, "%s50~55", Text_GetString(NULL, 0, sPokemonDetailText, 21));
    FreeBattle_DrawLevelOptionLine(430, 280, 1, buffer);
    sprintf(buffer, "%s25~30", Text_GetString(NULL, 0, sPokemonDetailText, 21));
    FreeBattle_DrawLevelOptionLine(430, 280, 2, buffer);
    sprintf(buffer, "%s15~20", Text_GetString(NULL, 0, sPokemonDetailText, 21));
    FreeBattle_DrawLevelOptionLine(430, 280, 3, buffer);
    sprintf(buffer, "%s1~100", Text_GetString(NULL, 0, sPokemonDetailText, 21));
    FreeBattle_DrawLevelOptionLine(430, 280, 4, buffer);

    Font_SetActive(0x10, 0);

    if (sFreeBattleRuleState == FB_STATE_SELECT_RULESET) {
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #ffffff
    } else {
        Gfx_SetEnvColor(0x78, 0x78, 0xC8, 0xFF); // #7878c8
    }

    Font_Printf(100, 244, Text_GetString(NULL, 0, sRuleSelectText, 9));
    Font_EndTexturedTextRendering();

    if (sFreeBattleRuleState == 3) {
        Ui_DrawAnimatedTextureMarker(108, (sHoveredRuleset * 24) + 276);
    }
}

// Better name: FreeBattle_UpdateRulesetSelection
void FreeBattle_UpdateLevelSelect(void) {
    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
        Audio_PlaySoundEffectById(SFX_FREE_BATTLE_RULE_CONFIRM);
        sFreeBattleRuleState = FB_STATE_CONFIRM;
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_B)) {
        Audio_PlaySoundEffectById(SFX_MENU_BACK);
        sHoveredRuleset = -1;
        sFreeBattleRuleState = FB_STATE_SELECT_RIGHT_SIDE;
    } else if (FreeBattle_SelectionHasSubrule() == 0) {
        if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DUP)) {
            Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
            sHoveredRuleset = (sHoveredRuleset + 4) % 5;
        } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DDOWN)) {
            Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
            sHoveredRuleset = (sHoveredRuleset + 1) % 5;
        }
    }
}

// Better name: FreeBattle_RulesetSelection
void FreeBattle_ClearLevelSelection(void) {
    sHoveredRuleset = -1;
}

void FreeBattle_DrawConfirmLine(s16 arg0, char* arg1) {
    if (arg0 == D_84002D28.unk_08) {
        Gfx_SetEnvColor(0xFF, 0xFF, 0, 0xFF); 
    } else {
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
    }
    Font_Printf(D_84002D28.unk_00 + 52, D_84002D28.unk_02 + (arg0 * 28) + 43, arg1);
}

#if defined(VERSION_US_REV2)
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/free_battle_rules/free_battle_rules/FreeBattle_DrawConfirmPanel.s")
#else
void FreeBattle_DrawConfirmPanel(void) {
    if (D_84002D28.unk_04 > 0) {
        s16 tmp = D_84002D28.unk_04 * 17;

        Ui_DrawBorderedPanel(D_84002D28.unk_00, D_84002D28.unk_02 + (s16)((136 - tmp) / 2), 152, tmp, 0x2121);
        if (D_84002D28.unk_04 == 8) {
            gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

            Gfx_DrawTextureRgba16(D_84002D28.unk_00 + 12, D_84002D28.unk_02 + 10, 36, 26, gComPlayerLabelIcon, 36, 0);

            gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

            Ui_DrawAnimatedTextureMarker(D_84002D28.unk_00 + 12,
                                         D_84002D28.unk_02 + (D_84002D28.unk_08 * 28) + 43);
            Font_BeginTranslucentTextRendering();
            Font_SetActive(0x10, 0);
            Font_Printf(D_84002D28.unk_00 + 48, D_84002D28.unk_02 + 11, Text_GetString(NULL, 0, sRuleSelectText, 10));
            FreeBattle_DrawConfirmLine(0, Text_GetString(NULL, 0, sRuleSelectText, 11));
            FreeBattle_DrawConfirmLine(1, Text_GetString(NULL, 0, sRuleSelectText, 12));
            FreeBattle_DrawConfirmLine(2, Text_GetString(NULL, 0, sRuleSelectText, 13));
            Font_EndTexturedTextRendering();
        }
    }
}
#endif

void FreeBattle_InitConfirmPanel(void) {
    D_84002D28.unk_00 = 400;
    D_84002D28.unk_02 = 100;
    D_84002D28.unk_04 = 0;
    D_84002D28.unk_08 = 1;
}

void FreeBattle_OpenConfirmPanel(void) {
    D_84002BE0 = 2;
    D_84002D28.unk_04++;
    if (D_84002D28.unk_04 == 8) {
        sFreeBattleRuleState = 4;
    }
}

void FreeBattle_CloseConfirmPanel(void) {
    D_84002BE0 = 2;
    D_84002D28.unk_04--;
    if (D_84002D28.unk_04 == 0) {
        sFreeBattleRuleState = D_84002D28.unk_06;
    }
}

void FreeBattle_UpdateConfirmDialog(void) {
    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
        D_84002BE4 = D_84002D28.unk_08;
        D_84002D28.unk_06 = 3;
        sHoveredRuleset = 0;
        sFreeBattleRuleState = 6;
        Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_B)) {
        D_84002D28.unk_06 = 2;
        sFreeBattleRuleState = 6;
        Audio_PlaySoundEffectById(SFX_MENU_BACK);
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DUP)) {
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
        D_84002D28.unk_08 = (D_84002D28.unk_08 + 2) % 3;
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DDOWN)) {
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
        D_84002D28.unk_08 = (D_84002D28.unk_08 + 1) % 3;
    }
}

void FreeBattle_PollInputs(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

void FreeBattle_DrawFrame(void) {
    BgStage_DrawFrame();
    if (D_84002BE0 > 0) {
        FreeBattle_DrawPanelFrame(76, 84, 488, 336);
        D_84002BE0 -= 1;
    }
    FreeBattle_DrawRuleGrid();
    FreeBattle_DrawLevelSelectPanel();
    FreeBattle_DrawConfirmPanel();
    BgStage_AdvanceFrame();
}

void FreeBattle_FadeIn(void) {
    s32 i;

    if (StageContext_GetFadeMode() == 1) {
        StageFade_StartFromOpaque(8);

        for (i = 0; i < 10; i++) {
            Controller_PollInputs();
            BgStage_DrawFrame();
            Gfx_DrawTiledRgba16Image(D_84002D0C);
            BgStage_AdvanceFrame();
        }
    }
}

void FreeBattle_FadeOut(void) {
    Audio_StopMusic(0x10);
    StageContext_SetClearColor(0xFFFF);
    StageFade_StartFromTransparent(8);

    while (StageContext_GetFadeMode() != 1) {
        Controller_PollInputs();
        BgStage_DrawFrame();
        Gfx_DrawTiledRgba16Image(D_84002D0C);
        BgStage_AdvanceFrame();
    }

    StageLoader_RunFrames(2);
}

void FreeBattle_SlideInPanel(void) {
    s32 i;
    s16 tmp;

    Audio_PlaySoundEffectById(SFX_PANEL_OPEN);

    for (i = 16; i < 368; i += 32) {
        FreeBattle_PollInputs();
        BgStage_DrawFrame();
        Gfx_DrawTiledRgba16Image(D_84002D0C);
        FreeBattle_DrawTitleBar();
        tmp = i;
        tmp = (336 - tmp) / 2;
        FreeBattle_DrawPanelFrame(76, tmp + 84, 488, i);
        BgStage_AdvanceFrame();
    }
}

void FreeBattle_SlideOutPanel(void) {
    s32 i;

    for (i = 9; i >= 0; i--) {
        FreeBattle_PollInputs();
        BgStage_DrawFrame();
        Gfx_DrawTiledRgba16Image(D_84002D0C);
        FreeBattle_DrawTitleBar();
        FreeBattle_DrawPanelFrame(76, (s16)((364 - (s16)(14 + i * 35)) / 2) + 84, 488, 14 + i * 35);
        BgStage_AdvanceFrame();
    }

    for (i = 0; i < 2; i++) {
        BgStage_DrawFrame();
        Gfx_DrawTiledRgba16Image(D_84002D0C);
        BgStage_AdvanceFrame();
    }
}

#if defined(VERSION_US_REV2)
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/free_battle_rules/free_battle_rules/FreeBattle_ApplyRuleSelection.s")
#else
void FreeBattle_ApplyRuleSelection(void) {
    char sp44[12];
    TeamRoster* temp_v0;
    s32 i;

    Session_SetMode(0, D_84002C98[sHoveredRuleset], 0);

    for (i = 0; i < 4; i++) {
        if (sTagTeamOptions[sTagTeamSelection.leftSideTeam].state & (1 << i)) {
            sprintf(sp44, "%dP", i + 1);
            Team_AddTrainer(0, Trainer_Create(i, 0, sp44, Text_GetPlayerLabel(1)));
        }
    }

    if (sTagTeamSelection.rightSideTeam == 3) {
        gSessionContext.aiDifficulty = D_84002BE4;
        temp_v0 = Trainer_Create(-1, 0, "COM", Text_GetPlayerLabel(3));
        Team_AddTrainer(1, temp_v0);
        if (sTagTeamSelection.leftSideTeam == 3) {
            temp_v0->controllerPort = 2;
        } else {
            temp_v0->controllerPort = 3;
        }
    } else {
        for (i = 0; i < 4; i++) {
            if (sTagTeamOptions[sTagTeamSelection.rightSideTeam + 4].state & (1 << i)) {
                sprintf(sp44, "%dP", i + 1);
                Team_AddTrainer(1, Trainer_Create(i, 0, sp44, Text_GetPlayerLabel(1)));
            }
        }
    }

    if (gSessionContext.battleSides[0].tagTeamSize == 2) {
        gSessionContext.battleSides[0].tagTeamMember[0]->rosterInfo->unk_003 = 0x35;
        gSessionContext.battleSides[0].tagTeamMember[1]->rosterInfo->unk_003 = 0x35;
    }

    if (gSessionContext.battleSides[1].tagTeamSize == 2) {
        gSessionContext.battleSides[1].tagTeamMember[0]->rosterInfo->unk_003 = 0x35;
        gSessionContext.battleSides[1].tagTeamMember[1]->rosterInfo->unk_003 = 0x35;
    }
}
#endif

s32 FreeBattle_RuleSelectLoop(void) {
    sFreeBattleRuleState = 0;
    Audio_PlayMusicIfChanged(45);
    FreeBattle_ClearRuleSelection();
    FreeBattle_ClearLevelSelection();
    FreeBattle_InitConfirmPanel();
    FreeBattle_FadeIn();
    FreeBattle_SlideInPanel();
    sFreeBattleRuleState = 1;
    sTagTeamSelection.leftSideTeam = 0;

    while ((sFreeBattleRuleState != 7) && (sFreeBattleRuleState != 8)) {
        FreeBattle_PollInputs();
        switch (sFreeBattleRuleState) {
            case 1:
                FreeBattle_UpdateRuleRowSelect();
                break;
            case 2:
                FreeBattle_UpdateRuleOptionSelect();
                break;
            case 3:
                FreeBattle_UpdateLevelSelect();
                break;
            case 4:
                FreeBattle_UpdateConfirmDialog();
                break;
            case 5:
                FreeBattle_OpenConfirmPanel();
                break;
            case 6:
                FreeBattle_CloseConfirmPanel();
                break;
        }
        FreeBattle_DrawFrame();
    }

    if (sFreeBattleRuleState == 8) {
        FreeBattle_SlideOutPanel();
        FreeBattle_FadeOut();
        return 0;
    }

    FreeBattle_ApplyRuleSelection();
    FreeBattle_SlideOutPanel();
    return 1;
}

#if defined(VERSION_US_REV2)
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/free_battle_rules/free_battle_rules/FreeBattle_RuleSelectMain.s")
#else
s32 FreeBattle_RuleSelectMain(s32 arg0, s32 arg1) {
    s32 sp1C;

    main_pool_push_state('RULE');

    Font_Init(28, 0);
    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_2000000, common_menu2_ui, 0);
    ASSET_LOAD(D_3000000, order_select_ui, 0);
    Text_InitStringTables();
    sRuleSelectText = Text_GetStringTable(15);
    sPokemonDetailText = Text_GetStringTable(6);
    D_84002D08 = ASSET_LOAD2(backgrounds, 1, 1);
    D_84002D0C = BinArchive_GetFile(D_84002D08, 6);
    D_84002D14 = Cont_GetControllerBits();
    StageLoader_UpdateSegments();
    sp1C = FreeBattle_RuleSelectLoop();
    StageLoader_WaitForRetrace();

    main_pool_pop_state('RULE');
    return sp1C;
}
#endif
