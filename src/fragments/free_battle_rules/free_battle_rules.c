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

typedef struct TagTeamSelection {
    /* 0x00 */ s16 leftSideTeam;
    /* 0x02 */ s16 rightSideTeam;
} TagTeamSelection; // size = 0x4

typedef struct ComDifficultyWindow {
    /* 0x00 */ s16 x;
    /* 0x02 */ s16 y;
    /* 0x04 */ s16 openTicks;
    /* 0x06 */ s16 nextState;
    /* 0x08 */ s16 hoveredDifficulty;
} ComDifficultyWindow; // size = 0xA

typedef struct TagTeamOption {
    /* 0x00 */ s16 controllerMask;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 x;
    /* 0x06 */ s16 y;
    /* 0x08 */ u8* firstPlayerIcon;
    /* 0x0C */ u8* secondPlayerIcon;
} TagTeamOption; // size = 0x10

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
static s16 sConnectedControllers;
static s16 sSettingsState;
#if defined(VERSION_US_REV2)
static s32 pad_D_84002D18_us_rev2;
#endif
static TagTeamSelection sTagTeamSelection;
static s32 pad_D_84002D1C;
static s16 sHoveredRuleset;
static ComDifficultyWindow sComDifficultyWindow;

static s16 sPanelDrawBudget = 2;
static s16 sComDifficulty = 0;
static TagTeamOption sTagTeamOptions[11] = {
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
static s16 sRulesetCategories[] = { 0, 3, 4, 5, 6, 0 };

void FreeBattle_DrawPokeBallIcon(s16 x, s16 y) {
    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    Gfx_DrawTextureRgba16(x, y, 48, 24, gPokeBallIconTop, 48, 0);
    Gfx_DrawTextureRgba16(x, y + 24, 48, 24, gPokeBallIconBottom, 48, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void FreeBattle_DrawVsIcon(s16 x, s16 y) {
    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    Gfx_DrawTextureRgba16(x, y, 100, 20, gVsTop, 100, 0);
    Gfx_DrawTextureRgba16(x, y + 20, 100, 20, gVsMiddle, 100, 0);
    Gfx_DrawTextureRgba16(x, y + 40, 100, 16, gVsBottom, 100, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void FreeBattle_DrawTintedBar(s16 x, s16 y, s16 width, Color_RGBA8* env_color, Color_RGBA8* prim_color) {
    gSPDisplayList(gDisplayListHead++, D_8006F558);
    gDPSetEnvColor(gDisplayListHead++, env_color->r, env_color->g, env_color->b, 255);
    gDPSetPrimColor(gDisplayListHead++, 0, 0, prim_color->r, prim_color->g, prim_color->b, 255);

    Gfx_DrawTextureIa8(x, y, 16, 32, D_20003C0, 0x10, 0);
    Gfx_DrawTextureIa8((x + width) - 16, y, 16, 32, D_20005C0, 0x10, 0);
    Gfx_DrawTexturedRectClipped(x + 16, y, width - 32, 32, 0, 0, 0, 0x400, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void FreeBattle_DrawTitleBar(void) {
    Color_RGBA8 fill_color;
    Color_RGBA8 border_color;

    Color_SetRGB(&fill_color, 0x64, 0x64, 0xC8); // #6464C8
    Color_SetRGB(&border_color, 0x28, 0x28, 0x8C); // #28288C
    FreeBattle_DrawTintedBar(72, 40, 520, &fill_color, &border_color);
    FreeBattle_DrawPokeBallIcon(48, 32);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(0x10, 0);
    Gfx_SetEnvColor(0, 0, 0, 0xFF); // #000000
    Font_Printf(106, 44, Text_GetString(NULL, 0, sRuleSelectText, 0));
    Gfx_SetEnvColor(0xFF, 0xFF, 0x77, 0xFF); // #FFFF77
    Font_Printf(104, 42, Text_GetString(NULL, 0, sRuleSelectText, 0));
    Font_EndTexturedTextRendering();
}

void FreeBattle_DrawSelectionCorners(s16 x, s16 y, s16 width, s16 height) {
    static s16 D_84002CA4 = 0;

    s16 sp56 = SINS(D_84002CA4) * 2;
    UNUSED s32 pad[2];

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetEnvColor(gDisplayListHead++, 0xF0, 0xD4, 0x68, 255);

    Gfx_DrawTextureIa8(x + sp56, y + sp56, 0x10, 0x10, gBottomLeftSelectionCorner, 0x10, 0);
    Gfx_DrawTextureIa8(x + sp56, ((y + height) - sp56) - 0x10, 0x10, 0x10, gTopLeftSelectionCorner, 0x10, 0);
    Gfx_DrawTextureIa8(((x + width) - sp56) - 0x10, y + sp56, 0x10, 0x10, gBottomRightSelectionCorner, 0x10, 0);
    Gfx_DrawTextureIa8(((x + width) - sp56) - 0x10, ((y + height) - sp56) - 0x10, 0x10, 0x10, gTopRightSelectionCorner, 0x10, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    D_84002CA4 += 0x2000;
}

void FreeBattle_DrawPanelFrame(s16 x, s16 y, s16 width, s16 height) {
    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    Gfx_DrawTextureIa8(x - 3, y - 3, 8, 8, D_2000340, 8, 0);
    Gfx_DrawTextureIa8((x + width) - 5, y - 3, 8, 8, D_2000380, 8, 0);
    Gfx_DrawTexturedRectClipped(x + 4, y - 3, width - 8, 8, 0, 0, 0, 0x400, 0);
    Gfx_DrawTextureIa8(x - 3, (y + height) - 5, 8, 8, D_20002C0, 8, 0);
    Gfx_DrawTexturedRectClipped(x - 3, y + 4, 8, height - 8, 0, 0, 0x400, 0, 0);
    Gfx_DrawTextureIa8((x + width) - 5, (y + height) - 5, 8, 8, D_2000300, 8, 0);
    Gfx_DrawTexturedRectClipped(x + 4, (y + height) - 5, width - 8, 8, 0, 0, 0, 0x400, 0);
    Gfx_DrawTexturedRectClipped((x + width) - 5, y + 4, 8, height - 8, 0, 0, 0x400, 0, 0);

    gSPDisplayList(gDisplayListHead++, D_8006F470);

    Gfx_FillRectRgb(x + 4, y + 4, width - 8, height - 8, 0x3C, 0x3C, 0xA0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void FreeBattle_DrawRoundedPanel(s16 x, s16 y, s16 raw_width, s16 raw_height, u8 r, u8 g, u8 b, u8 alpha) {
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

// Once a side is confirmed, only its selected option remains enabled for drawing.
s32 FreeBattle_IsTagTeamOptionEnabled(s16 option_index) {
    TagTeamOption* option = &sTagTeamOptions[option_index];
    UNUSED s16 t = option->controllerMask;
    s32 is_available = 0;

    if (option_index < 4) {
        if (sSettingsState >= 2) {
            is_available = option_index == sTagTeamSelection.leftSideTeam;
        } else {
            is_available = option->controllerMask == (option->controllerMask & sConnectedControllers);
        }
    } else if (option_index < 11) {
        if (sSettingsState >= 3) {
            is_available = option_index == (sTagTeamSelection.rightSideTeam + 4);
        } else {
            s16 tmp = (~sTagTeamOptions[sTagTeamSelection.leftSideTeam].controllerMask & sConnectedControllers);

            is_available = option->controllerMask == (option->controllerMask & tmp);
        }
    }
    return is_available;
}

void FreeBattle_DrawTeamAmpersand(s16 x, s16 y, s16 option_index) {
    if (FreeBattle_IsTagTeamOptionEnabled(option_index) != 0) {
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #FFFFFF
    } else {
        Gfx_SetEnvColor(0x64, 0x64, 0x64, 0x82); // #646464
    }
    Font_DrawCharAt(x, y, 38);
}

// Upper window: choose the controller combination for each battle side.
void FreeBattle_DrawTagTeamWindow(void) {
    s32 i;
    s16 temp_s2;
    s16 temp_s3;
    s32 player_icon_color_overlay;
    TagTeamOption* side_selection;
    s16 tmp1;
    s16 tmp2;

    FreeBattle_DrawRoundedPanel(100, 114, 432, 0x80, 0, 0, 0, 0xFF);
    FreeBattle_DrawVsIcon(266, 150);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    for (i = 0; i < 11; i++) {
        temp_s2 = sTagTeamOptions[i].x + 100;
        temp_s3 = sTagTeamOptions[i].y + 114;

        if (FreeBattle_IsTagTeamOptionEnabled(i) != 0) {
            player_icon_color_overlay = 0xFF; // #FFFFFF
        } else {
            player_icon_color_overlay = 0x82; // #828282
        }

        gDPSetEnvColor(gDisplayListHead++, player_icon_color_overlay, player_icon_color_overlay, player_icon_color_overlay, player_icon_color_overlay);

        Gfx_DrawTextureRgba16(temp_s2, temp_s3, 36, 26, sTagTeamOptions[i].firstPlayerIcon, 36, 0);
        if (sTagTeamOptions[i].secondPlayerIcon != NULL) {
            Gfx_DrawTextureRgba16(temp_s2 + 48, temp_s3, 36, 26, sTagTeamOptions[i].secondPlayerIcon, 36, 0);
        }
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    Font_BeginTranslucentTextRendering();
    Font_SetActive(0x10, 0);

    for (i = 0; i < 11; i++) {
        if (sTagTeamOptions[i].secondPlayerIcon != NULL) {
            FreeBattle_DrawTeamAmpersand(sTagTeamOptions[i].x + 135, sTagTeamOptions[i].y + 115, i);
        }
    }

    Font_SetActive(0x10, 0);

    if ((sSettingsState == 1) || (sSettingsState == 2)) {
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #FFFFFF
    } else {
        Gfx_SetEnvColor(0x78, 0x78, 0xC8, 0xFF); // #7878C8
    }

    Font_Printf(100, 90, Text_GetString(NULL, 0, sRuleSelectText, 1));
    Font_EndTexturedTextRendering();

    if (sSettingsState == FB_STATE_SELECT_LEFT_SIDE) {
        side_selection = &sTagTeamOptions[sTagTeamSelection.leftSideTeam];
        tmp1 = side_selection->x + 100;
        tmp2 = side_selection->y + 114;
        FreeBattle_DrawSelectionCorners(tmp1 - 4, tmp2 - 2, ((side_selection->secondPlayerIcon == NULL) ? 36 : 84) + 8, 30);
    } else if (sSettingsState == FB_STATE_SELECT_RIGHT_SIDE) {
        side_selection = &sTagTeamOptions[sTagTeamSelection.rightSideTeam];
        tmp1 = side_selection[4].x + 100;
        tmp2 = side_selection[4].y + 114;
        FreeBattle_DrawSelectionCorners(tmp1 - 4, tmp2 - 2, ((side_selection[4].secondPlayerIcon == NULL) ? 36 : 84) + 8, 30);
    }
}

void FreeBattle_UpdateLeftTagTeam(void) {
    s32 left_side_team = sTagTeamSelection.leftSideTeam;

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
        Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
        sTagTeamSelection.rightSideTeam = 0;
        while (FreeBattle_IsTagTeamOptionEnabled(sTagTeamSelection.rightSideTeam + 4) == 0) {
            sTagTeamSelection.rightSideTeam++;
        }
        sSettingsState = FB_STATE_SELECT_RIGHT_SIDE;
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_B)) {
        Audio_PlaySoundEffectById(SFX_MENU_BACK);
        sSettingsState = FB_STATE_EXIT;
    } else {
        do {
            if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DUP)) {
                sTagTeamSelection.leftSideTeam = (sTagTeamSelection.leftSideTeam + 3) % 4;
            } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DDOWN)) {
                sTagTeamSelection.leftSideTeam = (sTagTeamSelection.leftSideTeam + 1) % 4;
            }
        } while (FreeBattle_IsTagTeamOptionEnabled(sTagTeamSelection.leftSideTeam) == 0);

        if (left_side_team != sTagTeamSelection.leftSideTeam) {
            Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
        }
    }
}

void FreeBattle_UpdateRightTagTeam(void) {
    s32 sp34 = sTagTeamSelection.rightSideTeam;
    s32 i;
    s32 var_a0;
    s32 var_s1;
    s32 var_s4;

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
        Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
        if (sTagTeamSelection.rightSideTeam == 3) {
            sSettingsState = 5;
        } else {
            sHoveredRuleset = 0;
            sSettingsState = 3;
        }
        return;
    }

    var_s4 = 0;
    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_B)) {
        Audio_PlaySoundEffectById(SFX_MENU_BACK);
        sTagTeamSelection.rightSideTeam = -1;
        sSettingsState = 1;
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
            if (FreeBattle_IsTagTeamOptionEnabled(var_a0 * 4 + var_s1 + 4) != 0) {
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

void FreeBattle_ClearTagTeamSelection(void) {
    sTagTeamSelection.leftSideTeam = -1;
    sTagTeamSelection.rightSideTeam = -1;
}

// A second player icon identifies a two-controller tag team on either side.
s32 FreeBattle_SelectionHasTagTeam(void) {
    s32 var_v1 = 0;

    if ((sTagTeamSelection.leftSideTeam != -1) && (sTagTeamOptions[sTagTeamSelection.leftSideTeam].secondPlayerIcon != 0)) {
        var_v1 = 1;
    }

    if ((sTagTeamSelection.rightSideTeam != -1) && (sTagTeamOptions[sTagTeamSelection.rightSideTeam + 4].secondPlayerIcon != 0)) {
        var_v1 = 1;
    }
    return var_v1;
}

void FreeBattle_DrawRulesetOptionText(s16 box_x, s16 box_y, s16 option_index, char* text) {
    if ((option_index != 0) && (FreeBattle_SelectionHasTagTeam() != 0)) {
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

// Lower window: cup rules and their corresponding Pokemon level ranges.
void FreeBattle_DrawRulesetWindow(void) {
    UNUSED s32 pad;
    char buffer[128];

    FreeBattle_DrawRoundedPanel(100, 268, 432, 132, 0, 0, 0, 0xFF);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(8, 0);
    FreeBattle_DrawRulesetOptionText(148, 276, 0, Text_GetString(NULL, 0, sRuleSelectText, 2));
    FreeBattle_DrawRulesetOptionText(148, 276, 1, Text_GetString(NULL, 0, sRuleSelectText, 3));
    FreeBattle_DrawRulesetOptionText(148, 276, 2, Text_GetString(NULL, 0, sRuleSelectText, 6));
    FreeBattle_DrawRulesetOptionText(148, 276, 3, Text_GetString(NULL, 0, sRuleSelectText, 7));
    FreeBattle_DrawRulesetOptionText(148, 276, 4, Text_GetString(NULL, 0, sRuleSelectText, 8));
    Font_SetActive(4, 0);

    sprintf(buffer, "%s1~100", Text_GetString(NULL, 0, sPokemonDetailText, 21));
    FreeBattle_DrawRulesetOptionText(430, 280, 0, buffer);
    sprintf(buffer, "%s50~55", Text_GetString(NULL, 0, sPokemonDetailText, 21));
    FreeBattle_DrawRulesetOptionText(430, 280, 1, buffer);
    sprintf(buffer, "%s25~30", Text_GetString(NULL, 0, sPokemonDetailText, 21));
    FreeBattle_DrawRulesetOptionText(430, 280, 2, buffer);
    sprintf(buffer, "%s15~20", Text_GetString(NULL, 0, sPokemonDetailText, 21));
    FreeBattle_DrawRulesetOptionText(430, 280, 3, buffer);
    sprintf(buffer, "%s1~100", Text_GetString(NULL, 0, sPokemonDetailText, 21));
    FreeBattle_DrawRulesetOptionText(430, 280, 4, buffer);

    Font_SetActive(0x10, 0);

    if (sSettingsState == FB_STATE_SELECT_RULESET) {
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #ffffff
    } else {
        Gfx_SetEnvColor(0x78, 0x78, 0xC8, 0xFF); // #7878c8
    }

    Font_Printf(100, 244, Text_GetString(NULL, 0, sRuleSelectText, 9));
    Font_EndTexturedTextRendering();

    if (sSettingsState == 3) {
        Ui_DrawAnimatedTextureMarker(108, (sHoveredRuleset * 24) + 276);
    }
}

void FreeBattle_UpdateRulesetSelection(void) {
    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
        Audio_PlaySoundEffectById(SFX_FREE_BATTLE_RULE_CONFIRM);
        sSettingsState = FB_STATE_CONFIRM;
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_B)) {
        Audio_PlaySoundEffectById(SFX_MENU_BACK);
        sHoveredRuleset = -1;
        sSettingsState = FB_STATE_SELECT_RIGHT_SIDE;
    } else if (FreeBattle_SelectionHasTagTeam() == 0) {
        if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DUP)) {
            Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
            sHoveredRuleset = (sHoveredRuleset + 4) % 5;
        } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DDOWN)) {
            Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
            sHoveredRuleset = (sHoveredRuleset + 1) % 5;
        }
    }
}

void FreeBattle_ClearRulesetSelection(void) {
    sHoveredRuleset = -1;
}

void FreeBattle_DrawComDifficultyText(s16 arg0, char* arg1) {
    if (arg0 == sComDifficultyWindow.hoveredDifficulty) {
        Gfx_SetEnvColor(0xFF, 0xFF, 0, 0xFF); 
    } else {
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
    }
    Font_Printf(sComDifficultyWindow.x + 52, sComDifficultyWindow.y + (arg0 * 28) + 43, arg1);
}

#if defined(VERSION_US_REV2)
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/free_battle_rules/free_battle_rules/FreeBattle_DrawComDifficultyWindow.s")
#else
// Popup shown when the right-side option is COM, not a confirmation dialog.
void FreeBattle_DrawComDifficultyWindow(void) {
    if (sComDifficultyWindow.openTicks > 0) {
        s16 tmp = sComDifficultyWindow.openTicks * 17;

        Ui_DrawBorderedPanel(sComDifficultyWindow.x, sComDifficultyWindow.y + (s16)((136 - tmp) / 2), 152, tmp, 0x2121);
        if (sComDifficultyWindow.openTicks == 8) {
            gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

            Gfx_DrawTextureRgba16(sComDifficultyWindow.x + 12, sComDifficultyWindow.y + 10, 36, 26, gComPlayerLabelIcon, 36, 0);

            gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

            Ui_DrawAnimatedTextureMarker(sComDifficultyWindow.x + 12,
                                         sComDifficultyWindow.y + (sComDifficultyWindow.hoveredDifficulty * 28) + 43);
            Font_BeginTranslucentTextRendering();
            Font_SetActive(0x10, 0);
            Font_Printf(sComDifficultyWindow.x + 48, sComDifficultyWindow.y + 11, Text_GetString(NULL, 0, sRuleSelectText, 10));
            FreeBattle_DrawComDifficultyText(0, Text_GetString(NULL, 0, sRuleSelectText, 11));
            FreeBattle_DrawComDifficultyText(1, Text_GetString(NULL, 0, sRuleSelectText, 12));
            FreeBattle_DrawComDifficultyText(2, Text_GetString(NULL, 0, sRuleSelectText, 13));
            Font_EndTexturedTextRendering();
        }
    }
}
#endif

void FreeBattle_InitComDifficultyWindow(void) {
    sComDifficultyWindow.x = 400;
    sComDifficultyWindow.y = 100;
    sComDifficultyWindow.openTicks = 0;
    sComDifficultyWindow.hoveredDifficulty = 1;
}

void FreeBattle_OpenComDifficultyWindow(void) {
    sPanelDrawBudget = 2;
    sComDifficultyWindow.openTicks++;
    if (sComDifficultyWindow.openTicks == 8) {
        sSettingsState = 4;
    }
}

void FreeBattle_CloseComDifficultyWindow(void) {
    sPanelDrawBudget = 2;
    sComDifficultyWindow.openTicks--;
    if (sComDifficultyWindow.openTicks == 0) {
        sSettingsState = sComDifficultyWindow.nextState;
    }
}

void FreeBattle_UpdateComDifficulty(void) {
    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
        sComDifficulty = sComDifficultyWindow.hoveredDifficulty;
        sComDifficultyWindow.nextState = 3;
        sHoveredRuleset = 0;
        sSettingsState = 6;
        Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_B)) {
        sComDifficultyWindow.nextState = 2;
        sSettingsState = 6;
        Audio_PlaySoundEffectById(SFX_MENU_BACK);
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DUP)) {
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
        sComDifficultyWindow.hoveredDifficulty = (sComDifficultyWindow.hoveredDifficulty + 2) % 3;
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DDOWN)) {
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
        sComDifficultyWindow.hoveredDifficulty = (sComDifficultyWindow.hoveredDifficulty + 1) % 3;
    }
}

void FreeBattle_PollInputs(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

void FreeBattle_DrawFrame(void) {
    BgStage_DrawFrame();
    if (sPanelDrawBudget > 0) {
        FreeBattle_DrawPanelFrame(76, 84, 488, 336);
        sPanelDrawBudget -= 1;
    }
    FreeBattle_DrawTagTeamWindow();
    FreeBattle_DrawRulesetWindow();
    FreeBattle_DrawComDifficultyWindow();
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
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/free_battle_rules/free_battle_rules/FreeBattle_ApplySettings.s")
#else
void FreeBattle_ApplySettings(void) {
    char sp44[12];
    TeamRoster* temp_v0;
    s32 i;

    Session_SetMode(0, sRulesetCategories[sHoveredRuleset], 0);

    for (i = 0; i < 4; i++) {
        if (sTagTeamOptions[sTagTeamSelection.leftSideTeam].controllerMask & (1 << i)) {
            sprintf(sp44, "%dP", i + 1);
            Team_AddTrainer(0, Trainer_Create(i, 0, sp44, Text_GetPlayerLabel(1)));
        }
    }

    if (sTagTeamSelection.rightSideTeam == 3) {
        gSessionContext.aiDifficulty = sComDifficulty;
        temp_v0 = Trainer_Create(-1, 0, "COM", Text_GetPlayerLabel(3));
        Team_AddTrainer(1, temp_v0);
        if (sTagTeamSelection.leftSideTeam == 3) {
            temp_v0->controllerPort = 2;
        } else {
            temp_v0->controllerPort = 3;
        }
    } else {
        for (i = 0; i < 4; i++) {
            if (sTagTeamOptions[sTagTeamSelection.rightSideTeam + 4].controllerMask & (1 << i)) {
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

s32 FreeBattle_SettingsLoop(void) {
    sSettingsState = 0;
    Audio_PlayMusicIfChanged(45);
    FreeBattle_ClearTagTeamSelection();
    FreeBattle_ClearRulesetSelection();
    FreeBattle_InitComDifficultyWindow();
    FreeBattle_FadeIn();
    FreeBattle_SlideInPanel();
    sSettingsState = 1;
    sTagTeamSelection.leftSideTeam = 0;

    while ((sSettingsState != 7) && (sSettingsState != 8)) {
        FreeBattle_PollInputs();
        switch (sSettingsState) {
            case 1:
                FreeBattle_UpdateLeftTagTeam();
                break;
            case 2:
                FreeBattle_UpdateRightTagTeam();
                break;
            case 3:
                FreeBattle_UpdateRulesetSelection();
                break;
            case 4:
                FreeBattle_UpdateComDifficulty();
                break;
            case 5:
                FreeBattle_OpenComDifficultyWindow();
                break;
            case 6:
                FreeBattle_CloseComDifficultyWindow();
                break;
        }
        FreeBattle_DrawFrame();
    }

    if (sSettingsState == 8) {
        FreeBattle_SlideOutPanel();
        FreeBattle_FadeOut();
        return 0;
    }

    FreeBattle_ApplySettings();
    FreeBattle_SlideOutPanel();
    return 1;
}

#if defined(VERSION_US_REV2)
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/free_battle_rules/free_battle_rules/FreeBattle_SettingsMain.s")
#else
s32 FreeBattle_SettingsMain(s32 arg0, s32 arg1) {
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
    sConnectedControllers = Cont_GetControllerBits();
    StageLoader_UpdateSegments();
    sp1C = FreeBattle_SettingsLoop();
    StageLoader_WaitForRetrace();

    main_pool_pop_state('RULE');
    return sp1C;
}
#endif
