#include "event_battle.h"
#include "src/poke_icon.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/ui_graphics.h"
#include "src/gb_data.h"
#include "src/session.h"
#include "src/text_system.h"
#include "src/jpeg_stream.h"
#include "src/audio_sfx.h"
#include "src/audio_commands_category2.h"
#include "src/gfx_buffer.h"
#include "src/DDC0.h"
#include "src/matrix.h"
#include "src/memory.h"
#include "src/stage_loader.h"
#include "src/pokemon_stats.h"

static char** sEventBattleText;
static char** sPokemonDetailText;
static unk_D_82305A28 D_82305A28;
static DescriptionPanel D_82305A30;
static DescriptionPanel D_82305A38;
static unk_D_82305A40 D_82305A40;
static EventBattleSelectionOption D_82305A50[6];
static unk_D_82305AF8 D_82305AF8[2];
static EventBattleOptionLabel D_8230F528[3];

static EventBattleModeEntry gEventBattleModes[] = {
    {
        68,
        132,
        248,
        56,
        3,
    },
    {
        324,
        132,
        248,
        56,
        4,
    },
    {
        68,
        200,
        248,
        56,
        6,
    },
    {
        324,
        200,
        248,
        56,
        5,
    },
    {
        68,
        268,
        248,
        56,
        254,
    },
    {
        324,
        268,
        248,
        56,
        255,
    },
    {
        80,
        385,
        152,
        48,
        1,
    },
    {
        244,
        385,
        152,
        48,
        2,
    },
};
static s16 D_82305970[] = {
    408, 385, 152, 48, 0, 0,
};
static unk_D_82305978 D_8230597C[] = {
    {
        50,
        55,
        155,
        1,
    },
    {
        50,
        55,
        155,
        1,
    },
    {
        50,
        55,
        155,
        1,
    },
    {
        25,
        30,
        80,
        8,
    },
    {
        15,
        20,
        50,
        16,
    },
    {
        1,
        100,
        300,
        32,
    },
};

void EventBattle_DrawSelectionCorners(s16 left, s16 top, s16 right, s16 bottom) {
    static s16 D_823059AC = 0;

    s16 pulse = SINS(D_823059AC) * 2;
    UNUSED s32 pad[2];

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetEnvColor(gDisplayListHead++, 240, 212, 104, 255);

    Gfx_DrawTextureIa8(left + pulse, top + pulse, 16, 16, gBottomLeftSelectionCorner, 16, 0);
    Gfx_DrawTextureIa8(left + pulse, ((top + bottom) - pulse) - 16, 16, 16, gTopLeftSelectionCorner, 16, 0);
    Gfx_DrawTextureIa8(((left + right) - pulse) - 16, top + pulse, 16, 16, gBottomRightSelectionCorner, 16, 0);
    Gfx_DrawTextureIa8(((left + right) - pulse) - 16, ((top + bottom) - pulse) - 16, 16, 16, gTopRightSelectionCorner,
                       16, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    D_823059AC += 0x2000;
}

/*
    Draws the white frame around the cup select buttons
*/
void EventBattle_DrawPanelFrame(s16 x, s16 y, s16 width, s16 height) {
    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    Gfx_DrawTextureIa8(x - 3, y - 3, 8, 8, D_2000340, 8, 0);
    Gfx_DrawTextureIa8((x + width) - 5, y - 3, 8, 8, D_2000380, 8, 0);
    Gfx_DrawTexturedRectClipped(x + 4, y - 3, width - 8, 8, 0, 0, 0, 0x400, 0);
    Gfx_DrawTextureIa8(x - 3, (y + height) - 5, 8, 8, D_20002C0, 8, 0);
    Gfx_DrawTexturedRectClipped(x - 3, y + 4, 8, height - 8, 0, 0, 0x400, 0, 0);
    Gfx_DrawTextureIa8((x + width) - 5, (y + height) - 5, 8, 8, D_2000300, 8, 0);
    Gfx_DrawTexturedRectClipped(x + 4, (y + height) - 5, width - 8, 8, 0, 0, 0, 0x400, 0);
    Gfx_DrawTexturedRectClipped((x + width) - 5, y + 4, 8, height - 8, 0, 0, 0x400, 0, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}
// Better name: EventBattle_DrawNonEligibleWarningBorder
/*
    Draws the "At least one Pokemon is not eligible" warning's black transparent border
    after selecting a cup where not all pokemon on both cart's parties are eligible.
*/
void EventBattle_DrawSlicedFrame(s16 border_x, s16 border_y, s16 x, s16 y, u8 r, u8 g, u8 b, u8 alpha) {
    s16 height_adjustment;
    s16 width_adjustment;
    s16 width;
    s16 height;

    if (x >= 32) {
        width_adjustment = 16;
    } else {
        width_adjustment = x / 2;
    }

    if (y >= 32) {
        height_adjustment = 16;
    } else {
        height_adjustment = y / 2;
    }

    width = x - (width_adjustment * 2);
    height = y - (height_adjustment * 2);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetEnvColor(gDisplayListHead++, r, g, b, alpha);
    gDPLoadTextureBlock_4b(gDisplayListHead++, D_20288E0, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(border_x, border_y, width_adjustment, height_adjustment, 0, 0, 0x4000 / width_adjustment, 0x4000 / height_adjustment, 0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_2028960, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped((border_x + x) - width_adjustment, border_y, width_adjustment, height_adjustment, 0, 0, 0x4000 / width_adjustment, 0x4000 / height_adjustment,
                                0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_20287E0, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(border_x, (border_y + y) - height_adjustment, width_adjustment, height_adjustment, 0, 0, 0x4000 / width_adjustment, 0x4000 / height_adjustment,
                                0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_2028860, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped((border_x + x) - width_adjustment, (border_y + y) - height_adjustment, width_adjustment, height_adjustment, 0, 0, 0x4000 / width_adjustment,
                                0x4000 / height_adjustment, 0);

    gSPDisplayList(gDisplayListHead++, gUiTranslucentFillDL);

    if ((width > 0) && (height > 0)) {
        Gfx_FillRectRgba(border_x + width_adjustment, border_y, width, height_adjustment, r, g, b, alpha);
        Gfx_FillRectRgba(border_x, border_y + width_adjustment, x, height, r, g, b, alpha);
        Gfx_FillRectRgba(border_x + width_adjustment, border_y + width_adjustment + height, width, height_adjustment, r, g, b, alpha);
    } else if (width > 0) {
        Gfx_FillRectRgba(border_x + width_adjustment, border_y, width, y, r, g, b, alpha);
    } else if (height > 0) {
        Gfx_FillRectRgba(border_x, border_y + height_adjustment, x, height, r, g, b, alpha);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

// Better name: EventBattle_DrawInfoBoxBorder
/*
    Draws the blue border around the text that describes the selection option
*/
void EventBattle_DrawThinSlicedFrame(s16 x, s16 y, s16 raw_width, s16 raw_height, u8 r, u8 g, u8 b, u8 alpha) {
    s16 smoothing_width;
    s16 smoothing_height;
    s16 height;
    s16 width;

    if (raw_width >= 16) {
        width = 8;
    } else {
        width = raw_width / 2;
    }

    if (raw_height >= 16) {
        height = 8;
    } else {
        height = raw_height / 2;
    }

    smoothing_width = raw_width - (width * 2);
    smoothing_height = raw_height - (height * 2);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetEnvColor(gDisplayListHead++, r, g, b, alpha);
    gDPLoadTextureBlock_4b(gDisplayListHead++, gUiPanelTopLeftTexture, G_IM_FMT_I, 16, 8, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(x, y, width, height, 0, 0, 0x2000 / width, 0x2000 / height, 0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, gUiPanelTopRightTexture, G_IM_FMT_I, 16, 8, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped((x + raw_width) - width, y, width, height, 0, 0, 0x2000 / width, 0x2000 / height,
                                0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, gUiPanelBottomLeftTexture, G_IM_FMT_I, 16, 8, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(x, (y + raw_height) - height, width, height, 0, 0, 0x2000 / width, 0x2000 / height,
                                0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, gUiPanelBottomRightTexture, G_IM_FMT_I, 16, 8, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped((x + raw_width) - width, (y + raw_height) - height, width, height, 0, 0, 0x2000 / width,
                                0x2000 / height, 0);

    gSPDisplayList(gDisplayListHead++, gUiTranslucentFillDL);

    if ((smoothing_width > 0) && (smoothing_height > 0)) {
        Gfx_FillRectRgba(x + width, y, smoothing_width, height, r, g, b, alpha);
        Gfx_FillRectRgba(x, y + width, raw_width, smoothing_height, r, g, b, alpha);
        Gfx_FillRectRgba(x + width, (y + width) + smoothing_height, smoothing_width, height, r, g, b, alpha);
    } else if (smoothing_width > 0) {
        Gfx_FillRectRgba(x + width, y, smoothing_width, raw_height, r, g, b, alpha);
    } else if (smoothing_height > 0) {
        Gfx_FillRectRgba(x, y + height, raw_width, smoothing_height, r, g, b, alpha);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

// Better name: EventBattle_DrawPokemonSlotBackground
/*
    Draws the red/green border for eligible/ineligible pokemon
*/
void EventBattle_DrawSmallFrame(s16 left, s16 top, s16 right, s16 bottom, u8 r, u8 g, u8 b, u8 alpha) {
    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetEnvColor(gDisplayListHead++, r, g, b, alpha);

    Gfx_DrawTextureIa8(left, top, 8, 8, D_2000B40, 8, 0);
    Gfx_DrawTextureIa8((left + right) - 8, top, 8, 8, D_2000B80, 8, 0);
    Gfx_DrawTexturedRectClipped(left + 8, top, right - 16, 8, 0, 0, 0, 0x400, 0);
    Gfx_DrawTextureIa8(left, (top + bottom) - 8, 8, 8, D_2000BC0, 8, 0);
    Gfx_DrawTexturedRectClipped(left, top + 8, 8, bottom - 16, 0, 0, 0x400, 0, 0);
    Gfx_DrawTextureIa8((left + right) - 8, (top + bottom) - 8, 8, 8, D_2000C00, 8, 0);
    Gfx_DrawTexturedRectClipped(left + 8, (top + bottom) - 8, right - 16, 8, 0, 0, 0, 0x400, 0);
    Gfx_DrawTexturedRectClipped((left + right) - 8, top + 8, 8, bottom - 16, 0, 0, 0x400, 0, 0);

    gSPDisplayList(gDisplayListHead++, gUiTranslucentFillDL);

    Gfx_FillRectRgba(left + 8, top + 8, right - 16, bottom - 16, r, g, b, alpha);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

// Better name: EventBattle_DrawTitleBarPokeBall
/*
    Draws the Poke Ball at the top of the screen
*/
void EventBattle_DrawHeaderImage(s16 x, s16 y) {
    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    Gfx_DrawTextureRgba16(x, y, 48, 24, gPokeBallIconTop, 48, 0);
    Gfx_DrawTextureRgba16(x, y + 24, 48, 24, gPokeBallIconBottom, 48, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

// Better name: EventBattle_DrawTitleBarBackground
/*
    Draws the blue TitleBar background at the top of the screen
*/
void EventBattle_DrawTintedBar(s16 x, s16 y, s16 arg2, Color_RGBA8* env_color, Color_RGBA8* arg4) {
    gSPDisplayList(gDisplayListHead++, D_8006F558);

    gDPSetEnvColor(gDisplayListHead++, env_color->r, env_color->g, env_color->b, 255);
    gDPSetPrimColor(gDisplayListHead++, 0, 0, arg4->r, arg4->g, arg4->b, 255);

    Gfx_DrawTextureIa8(x, y, 16, 32, D_20003C0, 16, 0);
    Gfx_DrawTextureIa8((x + arg2) - 16, y, 16, 32, D_20005C0, 16, 0);
    Gfx_DrawTexturedRectClipped(x + 16, y, arg2 - 32, 32, 0, 0, 0, 0x400, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

/*
    Draws the title text and colors its background and text
*/
void EventBattle_DrawTitleBar(void) {
    Color_RGBA8 border;
    Color_RGBA8 fill;

    Color_SetRGB(&border, 0x64, 0x64, 0xC8); // #6464C8
    Color_SetRGB(&fill, 0x28, 0x28, 0x8C); // #28288C
    EventBattle_DrawTintedBar(72, 40, 520, &border, &fill);
    EventBattle_DrawHeaderImage(48, 32);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(0x10, 0);
    Gfx_SetEnvColor(0, 0, 0, 0xFF); // #000000
    Font_Printf(106, 44, Text_GetString(NULL, 0, sEventBattleText, 8));
    Gfx_SetEnvColor(0xFF, 0xFF, 0x77, 0xFF); // #FFFF77
    Font_Printf(104, 42, Text_GetString(NULL, 0, sEventBattleText, 8));
    Font_EndTexturedTextRendering();
}

void EventBattle_DrawInfoBox(DescriptionPanel* panel) {
    s16 opening_progress;
    s16 x;
    s16 y;

    if (panel->openingProgress >= 16) {
        opening_progress = panel->openingProgress;
        x = panel->x;
        y = panel->y + ((60 - opening_progress) / 2);

        EventBattle_DrawThinSlicedFrame(x, y, 528, opening_progress, 0x1E, 0x1E, 0x82, 0x96); // #1E1E82
        if (((60 - opening_progress) == 0) && (D_82305A28.unk_02 < 8)) {
            Font_BeginTranslucentTextRendering();
            Font_SetActive(8, 0);
            Font_SetLineHeight(24);
            Font_Printf(x + 32, y + 8,
                        Text_GetString(NULL, 0, sEventBattleText, D_82305A50[D_82305A28.unk_02].unk_18));
            Font_EndTexturedTextRendering();
        }
    }
}

void EventBattle_UpdateInfoBox(DescriptionPanel* panel) {
    switch (panel->expanding) {
        case 1:
            panel->openingProgress += 6;
            if (panel->openingProgress == 60) {
                panel->expanding = 0;
            }
            break;

        case 2:
            panel->openingProgress -= 6;
            if (panel->openingProgress == 0) {
                panel->expanding = 0;
            }
            break;
    }
}

void EventBattle_InitInfoBox(DescriptionPanel* panel, s16 x, s16 y) {
    panel->expanding = 0;
    panel->openingProgress = 0;
    panel->x = x;
    panel->y = y;
}

void EventBattle_DrawRuleCard(EventBattleSelectionOption* arg0) {
    s16 temp_s1;
    s16 temp_s2;

    if (arg0->visible != 0) {
        temp_s1 = arg0->unk_08;
        temp_s2 = arg0->unk_0A;

        EventBattle_DrawPanelFrame(temp_s1, temp_s2, 248, 56);

        gSPDisplayList(gDisplayListHead++, D_8006F470);

        if (arg0->unk_0C != NULL) {
            if (arg0->displayEligibility != 0) {
                Gfx_FillRectRgb(temp_s1 + 4, temp_s2 + 4, 240, 48, 0x1E, 0x1E, 0x64);
            } else {
                Gfx_FillRectRgb(temp_s1 + 4, temp_s2 + 4, 240, 48, 0x64, 0x1E, 0x1E);
            }

            gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

            Gfx_DrawTextureRgba16(temp_s1 + 8, temp_s2 + 8, 40, 40, arg0->unk_0C, 40, 0);
        } else {
            Gfx_FillRectRgb(temp_s1 + 4, temp_s2 + 4, 240, 48, 0x1E, 0x5A, 0x1E);
        }

        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

        Font_BeginTranslucentTextRendering();
        if (arg0->unk_0C != NULL) {
            s32 sp3C = temp_s2 + 8;
            char sp44[64];

            Font_SetActive(8, 0);

            if (arg0->displayEligibility != 0) {
                Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
            } else {
                Gfx_SetEnvColor(0xDC, 0x37, 0x37, 0xFF);
            }

            Font_Printf(temp_s1 + 52, sp3C, arg0->unk_10);
            Font_SetActive(4, 0);
            Gfx_SetEnvColor(0x92, 0xC1, 0xFF, 0xFF);
            sprintf(sp44, "%s%d~%d", Text_GetString(NULL, 0, sPokemonDetailText, 21), arg0->unk_14, arg0->unk_16);
            Font_Printf((temp_s1 - (s16)Font_MeasureTextExtent(4, 0, sp44)) + 234, temp_s2 + 32, sp44);
        } else {
            Font_SetActive(8, 0);
            Font_Printf((temp_s1 - ((s16)Font_MeasureTextExtent(8, 0, arg0->unk_10) / 2)) + 122, temp_s2 + 18,
                        arg0->unk_10);
        }
        Font_EndTexturedTextRendering();
    }
}

void EventBattle_UpdateRuleCardSlide(EventBattleSelectionOption* arg0) {
    switch (arg0->visible) {
        case 2:
            arg0->unk_02 -= 1;
            if (arg0->unk_02 == 0) {
                arg0->visible = 1;
            }

            if (arg0->unk_04 < 320) {
                arg0->unk_08 = arg0->unk_04 - (arg0->unk_02 << 5);
            } else {
                arg0->unk_08 = arg0->unk_04 + (arg0->unk_02 << 5);
            }
            break;

        case 3:
            arg0->unk_02 += 1;
            if (arg0->unk_02 == 10) {
                arg0->visible = 0;
            }

            if (arg0->unk_04 < 320) {
                arg0->unk_08 = arg0->unk_04 - (arg0->unk_02 << 5);
            } else {
                arg0->unk_08 = arg0->unk_04 + (arg0->unk_02 << 5);
            }
            break;
    }
}

void EventBattle_InitRuleCard(EventBattleSelectionOption* arg0, s16 arg1, s16 arg2, u8* arg3, s8* arg4, s16 arg5, s16 arg6) {
    arg0->visible = 0;
    arg0->unk_02 = 10;
    arg0->unk_04 = arg1;
    arg0->unk_06 = arg2;
    arg0->unk_08 = arg1;
    arg0->unk_0A = arg2;
    arg0->unk_0C = arg3;
    arg0->unk_10 = arg4;
    arg0->unk_14 = 0;
    arg0->unk_16 = 0;
    arg0->unk_18 = arg6;
    arg0->displayEligibility = 1;

    if (arg5 != 0) {
        arg0->unk_14 = D_8230597C[arg5 - 1].unk_00;
        arg0->unk_16 = D_8230597C[arg5 - 1].unk_02;
        arg0->displayEligibility = EventBattle_CheckTeamLegality(&D_82305AF8[0], arg5);
        arg0->displayEligibility &= EventBattle_CheckTeamLegality(&D_82305AF8[1], arg5);
    }
}

void EventBattle_DrawMonCell(s16 arg0, s16 arg1, s16 arg2, s32 arg3) {
    if (arg3 != 0) {
        EventBattle_DrawSmallFrame(arg1, arg2, 132, 44, 0x64, 0x1E, 0x1E, 0xFF);
    } else if (arg0 == 0) {
        EventBattle_DrawSmallFrame(arg1, arg2, 132, 44, 0x3C, 0x3C, 0xA0, 0xFF);
    } else {
        EventBattle_DrawSmallFrame(arg1, arg2, 132, 44, 0x1E, 0x64, 0x1E, 0xFF);
    }
}

static s16 D_823059B0[] = { 120, 252, 384, 120, 252, 384 };
static s16 D_823059BC[] = { 4, 4, 4, 48, 48, 48 };

void EventBattle_DrawTeamPanel(unk_D_82305AF8* arg0, s16 arg1, s16 arg2) {
    s16 i;
    s16 tmp1;
    s16 tmp2;
    s16 tmp3;

    EventBattle_DrawPanelFrame(arg1, arg2, 520, 96);
    if (arg0->unk_0001 == 0) {
        EventBattle_DrawSmallFrame(arg1 + 4, arg2 + 4, 116, 88, 0x64, 0x64, 0xC8, 0xFF);
    } else {
        EventBattle_DrawSmallFrame(arg1 + 4, arg2 + 4, 116, 88, 0x1E, 0x82, 0x1E, 0xFF);
    }

    for (i = 0; i < 6; i++) {
        tmp1 = arg0->unk_0001;
        tmp2 = D_823059B0[i] + arg1;
        tmp3 = D_823059BC[i] + arg2;
        EventBattle_DrawMonCell(tmp1, tmp2, tmp3, arg0->unk_0020[i].unk_25);
    }
}

void EventBattle_DrawRuleNotice(unk_D_82305AF8* arg0, s16 arg1, s16 arg2) {
    char sp40[256];
    s16 sp3E;

    if (arg0->unk_0003 == 0) {
        sp3E = (arg0->unk_0010 >= 0) ? (s16)(SINS((s16)(SINS(arg0->unk_0010) * 16384.0f)) * 127.0f) + 127
                                     : (s16)(SINS(arg0->unk_0010) * 127.0f) + 127;

        Gfx_FillRectWithDisplayState(arg1 + 120, arg2 + 4, 396, 88, 0x64, 0x1E, 0x1E, (sp3E * 2) / 3);
        Font_BeginTranslucentTextRendering();
        Font_SetActive(16, 0);
        Gfx_SetEnvColor(0xFF, 0xFF, 0, sp3E);
        Font_SetLineHeight(32);
        Text_SetNumberToken(3, D_8230597C[arg0->unk_0004 - 1].unk_04);
        Font_Printf(arg1 + 136, arg2 + 20, Text_GetString(sp40, 256, sEventBattleText, 9));
        Font_EndTexturedTextRendering();
        arg0->unk_0010 += 0x300;
    }
}

void EventBattle_DrawTrainerCard(unk_D_82305AF8* arg0) {
    u8* var_s0;
    s16 i;
    s16 sp58;
    s16 sp56;
    s16 tmp1;
    s16 tmp2;
    s16 tmp3;
    s16 tmp4;

    if (arg0->unk_0000 != 0) {
        sp58 = arg0->unk_000C;
        sp56 = arg0->unk_000E;

        if (arg0->unk_0001 == 0) {
            var_s0 = D_3024C80;
        } else {
            var_s0 = D_30260C0;
        }

        EventBattle_DrawTeamPanel(arg0, sp58, sp56);

        gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

        Gfx_DrawTextureRgba16(sp58 + 26, sp56 + 8, 72, 18, var_s0, 72, 0);
        Gfx_DrawTextureRgba16(sp58 + 26, sp56 + 26, 72, 18, var_s0 + 0xA20, 72, 0);

        for (i = 0; i < 6; i++) {
            tmp1 = D_823059B0[i] + sp58 + 2;
            tmp2 = D_823059BC[i] + sp56 + 2;
            Gfx_DrawTextureRgba16(tmp1, tmp2, 40, 40, arg0->unk_0218[i], 40, 0);
        }

        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

        Font_BeginTranslucentTextRendering();
        Font_SetActive(8, 0);
        Font_Printf((sp58 - (Font_MeasureTextExtent(8, 0, arg0->unk_0014) / 2)) + 62, sp56 + 47, arg0->unk_0014);
        Font_SetActive(4, 0);
        Font_Printf((sp58 - (Font_MeasureTextExtent(4, 0, "ID  00000") / 2)) + 62, sp56 + 70, "ID  %05d",
                    arg0->unk_0012);
        Font_SetActive(4, 0);

        for (i = 0; i < 6; i++) {
            tmp1 = D_823059B0[i] + sp58 + 44;
            tmp2 = D_823059BC[i] + sp56 + 5;
            Font_Printf(tmp1, tmp2, arg0->unk_0020[i].nickname);
        }

        Font_SetActive(4, 0);

        for (i = 0; i < 6; i++) {
            tmp3 = D_823059B0[i] + sp58 + 44;
            tmp4 = D_823059BC[i] + sp56 + 23;
            Font_Printf(tmp3, tmp4, "%s%d", Text_GetString(NULL, 0, sPokemonDetailText, 21), arg0->unk_0020[i].level);
        }

        Font_EndTexturedTextRendering();
        EventBattle_DrawRuleNotice(arg0, sp58, sp56);
    }
}

void EventBattle_UpdateTrainerCardSlide(unk_D_82305AF8* arg0) {
    switch (arg0->unk_0000) {
        case 2:
            arg0->unk_0006 -= 1;
            if (arg0->unk_0006 == 0) {
                arg0->unk_0000 = 1;
            }

            if (arg0->unk_0001 == 0) {
                arg0->unk_000C = arg0->unk_0008 - (arg0->unk_0006 << 6);
            } else {
                arg0->unk_000C = arg0->unk_0008 + (arg0->unk_0006 << 6);
            }
            break;

        case 3:
            arg0->unk_0006 += 1;
            if (arg0->unk_0006 == 10) {
                arg0->unk_0000 = 0;
            }

            if (arg0->unk_0001 == 0) {
                arg0->unk_000C = arg0->unk_0008 - (arg0->unk_0006 << 6);
            } else {
                arg0->unk_000C = arg0->unk_0008 + (arg0->unk_0006 << 6);
            }
            break;
    }
}

void EventBattle_InitTrainerCard(unk_D_82305AF8* arg0, s16 arg1, s16 arg2, s16 arg3) {
    s16 i;

    arg0->unk_0000 = 0;
    arg0->unk_0001 = arg3;
    arg0->unk_0006 = 10;
    arg0->unk_0008 = arg1;
    arg0->unk_000A = arg2;
    arg0->unk_000C = arg1;
    arg0->unk_000E = arg2;
    arg0->unk_0012 = GbSave_GetTrainerId(arg3);
    GbSave_GetPlayerName(arg3, arg0->unk_0014);

    for (i = 0; i < 6; i++) {
        Deck_ReadEntry(32, arg3, 0, i, &arg0->unk_0020[i]);
        PokeIcon_LoadModelTextureForMon(arg0->unk_0218[i], 0, &arg0->unk_0020[i]);
    }
}

s32 EventBattle_HasLegalMoves(BattleMon* arg0) {
    s32 i;
    s32 var_v1 = 1;

    for (i = 0; i < 4; i++) {
        if (arg0->moves[i] == 0) {
            break;
        }

        if (arg0->moves[i] >= 166) {
            var_v1 = 0;
        }
    }

    return var_v1;
}

u8 EventBattle_CheckTeamLegality(unk_D_82305AF8* arg0, s16 arg1) {
    unk_D_82305978* ptr1;
    s16 sp54[3];
    BattleMon* var_s1;
    s16 i;
    s16 j;

    var_s1 = arg0->unk_0020;

    for (i = 0; i < 3; i++) {
        sp54[i] = 999;
    }

    for (i = 0; i < 6; i++, var_s1++) {
        var_s1->unk_25 = 1;
        if ((var_s1->species.id > 0) && (var_s1->species.id < 152)) {
            if (EventBattle_HasLegalMoves(var_s1) != 0) {
                ptr1 = &D_8230597C[(arg1 - 1) & 0xFFFFFFFF];

                if (D_8006FF00[var_s1->species.id - 1].unk_0C & ptr1->unk_06) {
                    if ((var_s1->level >= ptr1->unk_00) && (ptr1->unk_02 >= var_s1->level)) {
                        var_s1->unk_25 = 0;
                    }
                }

                for (j = 0; j < i; j++) {
                    if (var_s1->species.id == arg0->unk_0020[j].species.id) {
                        var_s1->unk_25 = 1;
                        arg0->unk_0020[j].unk_25 = 1;
                    }
                }
            }
        }

        if (sp54[2] >= var_s1->level) {
            sp54[2] = var_s1->level;
        }

        if (sp54[1] >= var_s1->level) {
            sp54[2] = sp54[1];
            sp54[1] = var_s1->level;
        }

        if (sp54[0] >= var_s1->level) {
            sp54[1] = sp54[0];
            sp54[0] = var_s1->level;
        }
    }

    for (i = 0, j = 0; i < 6; i++) {
        if (arg0->unk_0020[i].unk_25 == 0) {
            j++;
        }
    }

    arg0->unk_0002 = j == 6;
    if (arg0->unk_0002) {
        arg0->unk_0003 = D_8230597C[arg1 - 1].unk_04 >= (sp54[0] + sp54[1] + sp54[2]);
    } else {
        arg0->unk_0003 = 1;
    }
    return arg0->unk_0002;
}

u8 EventBattle_ValidateTeamForRule(unk_D_82305AF8* arg0, s16 arg1) {
    arg0->unk_0004 = arg1;
    arg0->unk_0010 = 0;
    arg0->unk_0000 = 2;
    return EventBattle_CheckTeamLegality(arg0, arg1);
}

void EventBattle_DrawNoticeBanner(DescriptionPanel* arg0) {
    s16 temp_s3;
    s16 temp_s2;
    s16 i;
    s16 sp58;

    if (arg0->openingProgress > 0) {
        temp_s2 = arg0->openingProgress / 4;
        temp_s3 = arg0->x;
        sp58 = arg0->y + ((56 - arg0->openingProgress) / 2);

        gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

        for (i = 0; i < 4; i++) {
            gDPLoadTextureBlock(gDisplayListHead++, &D_302C000[i * 0xAF0], G_IM_FMT_RGBA, G_IM_SIZ_16b, 100, 14, 0,
                                G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);

            Gfx_DrawTexturedRectClipped(temp_s3, sp58 + (i * temp_s2), 100, temp_s2, 0, 0, 0x400, 0x3800 / temp_s2, 0);
        }

        if ((arg0->unk_01 == 0) && (arg0->openingProgress == 56)) {
            EventBattle_DrawSlicedFrame(112, sp58 + 8, 416, 40, 0, 0, 0, 0x96);

            gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

            Font_BeginTranslucentTextRendering();
            Font_SetActive(16, 0);
            Gfx_SetEnvColor(0xFF, 0x50, 0x64, 0xFF);
            Font_Printf(144, sp58 + 14, Text_GetString(NULL, 0, sEventBattleText, 10));
            Font_EndTexturedTextRendering();
        } else {
            gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
        }
    }
}

void EventBattle_UpdateNoticeBanner(DescriptionPanel* arg0) {
    switch (arg0->expanding) {
        case 1:
            arg0->openingProgress += 7;
            if (arg0->openingProgress == 56) {
                if (arg0->unk_01 == 0) {
                    Audio_PlaySoundEffectById(SFX_MENU_ERROR);
                }
                arg0->expanding = 0;
            }
            break;

        case 2:
            arg0->openingProgress -= 7;
            if (arg0->openingProgress == 0) {
                arg0->expanding = 0;
            }
            break;
    }
}

void EventBattle_InitNoticeBanner(DescriptionPanel* arg0, s16 arg1, s16 arg2) {
    arg0->expanding = 0;
    arg0->openingProgress = 0;
    arg0->x = arg1;
    arg0->y = arg2;
}

void EventBattle_ShowNoticeBanner(DescriptionPanel* arg0, s32 arg1) {
    arg0->unk_01 = arg1;
    arg0->expanding = 1;
}

void EventBattle_DrawOptionLabel(EventBattleOptionLabel* arg0) {
    UNUSED s32 pad;
    s16 sp3A;
    s16 sp38;
    s16 sp36;

    if (arg0->height >= 10) {
        sp3A = arg0->height;
        sp38 = arg0->x;
        sp36 = arg0->y + ((48 - sp3A) / 2);
        EventBattle_DrawPanelFrame(sp38, sp36, 152, sp3A);

        gSPDisplayList(gDisplayListHead++, D_8006F470);

        Gfx_FillRectRgb(sp38 + 4, sp36 + 4, 144, sp3A - 8, 0x3C, 0x3C, 0xA0);

        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

        if (arg0->height == 48) {
            Font_BeginTranslucentTextRendering();
            Font_SetActive(16, 0);
            Font_Printf((sp38 - ((s16)Font_MeasureTextExtent(16, 0, arg0->text) / 2)) + 76, sp36 + 11,
                        arg0->text);
            Font_EndTexturedTextRendering();
        }
    }
}

void EventBattle_UpdateOptionLabel(EventBattleOptionLabel* arg0) {
    switch (arg0->animState) {
        case 1:
            arg0->height += 6;
            if (arg0->height == 48) {
                arg0->animState = 0;
            }
            break;

        case 2:
            arg0->height -= 6;
            if (arg0->height == 6) {
                arg0->animState = 0;
            }
            break;
    }
}

void EventBattle_InitOptionLabel(EventBattleOptionLabel* arg0, s16 arg1, s16 arg2, s8* arg3) {
    arg0->animState = 0;
    arg0->height = 6;
    arg0->x = arg1;
    arg0->y = arg2;
    arg0->text = arg3;
}

void EventBattle_DrawSettingLine(unk_D_82305A40* arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4) {
    char sp30[64];
    UNUSED s32 pad;

    if (((arg1 == 1) || (arg1 == 2)) && (arg0->unk_01 == 0)) {
        Gfx_SetEnvColor(0x50, 0x50, 0xB4, 0xFF);
    } else if (arg1 == arg0->unk_0C) {
        Gfx_SetEnvColor(0xFF, 0xFF, 0, 0xFF);
    } else {
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
    }

    Font_Printf(arg0->unk_08 + 52, arg0->unk_0A + arg1 * 33 + 47, Text_GetString(NULL, 0, sEventBattleText, arg2));

    if (arg3 >= 0) {
        Text_SetNumberToken(2, arg4);
        Text_GetString(sp30, sizeof(sp30), sEventBattleText, arg3);
        Font_Printf((arg0->unk_08 - (Font_MeasureTextExtent(0, 0, sp30) / 2)) + 344,
                    arg0->unk_0A + arg1 * 33 + 47, sp30, arg4);
    }
}

void EventBattle_DrawSettingBar(s16 arg0, s16 arg1, s16 arg2) {
    if (arg2 != 0) {
        Gfx_SetEnvColor(0x82, 0x1E, 0x1E, 0xFF);
    } else {
        Gfx_SetEnvColor(0x32, 0x32, 0x96, 0xFF);
    }

    Gfx_DrawTextureI4(arg0, arg1, 16, 28, D_30242C0, 16, 0);
    Gfx_DrawTextureI4(arg0 + 104, arg1, 16, 28, D_30243A0, 16, 0);
    Gfx_DrawTexturedRectClipped(arg0 + 16, arg1, 88, 28, 0, 0, 0, 0x400, 0);
}

void EventBattle_DrawSpinnerArrows(s16 arg0, s16 arg1, s16 arg2, s16 arg3) {
    static s16 D_823059C8 = 0;

    s16 sp36 = SINS(D_823059C8) * 2;

    if (arg2 == 1) {
        if (arg3 >= 6) {
            Gfx_DrawTextureRgba16(arg0 - sp36, arg1 + (arg2 * 33), 20, 32, D_302FBC0, 20, 0);
        }

        if (arg3 < 90) {
            Gfx_DrawTextureRgba16(arg0 + sp36 + 148, arg1 + (arg2 * 33), 20, 32, D_30300C0, 20, 0);
        }
    } else {
        if (arg3 >= 11) {
            Gfx_DrawTextureRgba16(arg0 - sp36, arg1 + (arg2 * 33), 20, 32, D_302FBC0, 20, 0);
        }

        if (arg3 < 90) {
            Gfx_DrawTextureRgba16(arg0 + sp36 + 148, arg1 + (arg2 * 33), 20, 32, D_30300C0, 20, 0);
        }
    }
    D_823059C8 += 0x2000;
}

void EventBattle_DrawSettingsPanel(unk_D_82305A40* arg0) {
    char* sp54;
    s16 var_a3;
    s16 temp_s1;
    s16 temp_s2;
    UNUSED s16 pad;
    s16 sp4A;

    if (arg0->unk_06 >= 15) {
        sp4A = arg0->unk_06;
        temp_s2 = arg0->unk_08;
        temp_s1 = arg0->unk_0A + ((186 - sp4A) / 2);

        Ui_DrawBorderedPanelNoFill(temp_s2, temp_s1, 446, sp4A);

        if (sp4A == 186) {
            EventBattle_DrawSmallFrame(temp_s2 + 7, temp_s1 + 7, 432, 32, 0x82, 0x1E, 0x1E, 0xFF);
            EventBattle_DrawSmallFrame(temp_s2 + 7, temp_s1 + 39, 432, 140, 0x1E, 0x1E, 0x64, 0xFF);
            Ui_DrawAnimatedTextureMarker(temp_s2 + 14, temp_s1 + (arg0->unk_0C * 33) + 49);

            gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

            if (arg0->unk_01 == 1) {
                if (arg0->unk_0C == 1) {
                    EventBattle_DrawSpinnerArrows(temp_s2 + 260, temp_s1 + 45, arg0->unk_0C, arg0->unk_02);
                }

                if (arg0->unk_0C == 2) {
                    EventBattle_DrawSpinnerArrows(temp_s2 + 260, temp_s1 + 45, arg0->unk_0C, arg0->unk_04);
                }
            }

            EventBattle_DrawSettingBar(temp_s2 + 284, temp_s1 + 46, 1);
            EventBattle_DrawSettingBar(temp_s2 + 284, temp_s1 + 79, arg0->unk_01);
            EventBattle_DrawSettingBar(temp_s2 + 284, temp_s1 + 112, arg0->unk_01);

            gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

            Font_BeginTranslucentTextRendering();
            Font_SetActive(16, 0);
            sp54 = Text_GetString(NULL, 0, sEventBattleText, 11);
            Font_Printf((temp_s2 - (Font_MeasureTextExtent(16, 0, sp54) / 2)) + 223, temp_s1 + 10, sp54);

            if (arg0->unk_01 == 1) {
                var_a3 = 12;
            } else {
                var_a3 = 13;
            }

            EventBattle_DrawSettingLine(arg0, 0, 14, var_a3, 0);
            EventBattle_DrawSettingLine(arg0, 1, 16, 15, arg0->unk_02);
            EventBattle_DrawSettingLine(arg0, 2, 17, 18, arg0->unk_04);
            EventBattle_DrawSettingLine(arg0, 3, 19, -1, 0);
            Font_EndTexturedTextRendering();
        } else {
            EventBattle_DrawSmallFrame(temp_s2 + 7, temp_s1 + 7, 432, sp4A - 14, 0x1E, 0x1E, 0x64, 0xFF);
        }
    }
}

void EventBattle_UpdateSettingsPanel(unk_D_82305A40* arg0) {
    s32 temp_v0_2;

    switch (arg0->unk_00) {
        case 1:
            temp_v0_2 = Input_GetRepeatedDPad();
            if ((temp_v0_2 == 0x200) && (arg0->unk_01 == 1)) {
                if (arg0->unk_0C == 1) {
                    if (arg0->unk_02 >= 6) {
                        arg0->unk_02 -= 5;
                        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                    }
                }

                if (arg0->unk_0C == 2) {
                    if (arg0->unk_04 >= 11) {
                        arg0->unk_04 -= 10;
                        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                    }
                }
            } else if ((temp_v0_2 == 0x100) && (arg0->unk_01 == 1)) {
                if (arg0->unk_0C == 1) {
                    if (arg0->unk_02 < 90) {
                        arg0->unk_02 += 5;
                        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                    }
                }

                if (arg0->unk_0C == 2) {
                    if (arg0->unk_04 < 90) {
                        arg0->unk_04 += 10;
                        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                    }
                }
            } else {
                if (gPlayer1Controller->buttonPressed & 0x800) {
                    if (arg0->unk_0C > 0) {
                        arg0->unk_0C--;
                        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                    }
                } else if (gPlayer1Controller->buttonPressed & 0x400) {
                    if (arg0->unk_0C < 3) {
                        arg0->unk_0C++;
                        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                    }
                } else if (gPlayer1Controller->buttonPressed & 0x8000) {
                    if (arg0->unk_0C == 0) {
                        arg0->unk_01 ^= 1;
                        Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
                    }

                    if (arg0->unk_0C == 3) {
                        D_80075680[0] = arg0->unk_01;
                        D_80075680[1] = arg0->unk_02;
                        D_80075680[2] = arg0->unk_04;
                        arg0->unk_00 = 3;
                        Audio_PlaySoundEffectById(29);
                    }
                } else {
                    if (gPlayer1Controller->buttonPressed & 0x4000) {
                        arg0->unk_01 = D_80075680[0];
                        arg0->unk_02 = D_80075680[1];
                        arg0->unk_04 = D_80075680[2];
                        arg0->unk_00 = 3;
                        Audio_PlaySoundEffectById(SFX_MENU_BACK);
                    }
                }
            }
            break;

        case 2:
            arg0->unk_06 += 22;
            if (arg0->unk_06 == 186) {
                Input_SetRepeatController(gPlayer1Controller);
                arg0->unk_00 = 1;
            }
            break;

        case 3:
            arg0->unk_06 -= 22;
            if (arg0->unk_06 == 10) {
                arg0->unk_00 = 0;
            }
            break;
    }
}

void EventBattle_InitSettingsPanel(unk_D_82305A40* arg0, s16 arg1, s16 arg2, s32 arg3) {
    arg0->unk_00 = 0;
    arg0->unk_06 = 10;
    arg0->unk_08 = arg1;
    arg0->unk_0A = arg2;
    arg0->unk_0C = 0;

    if (arg3 != 0) {
        arg0->unk_01 = D_80075680[0];
        arg0->unk_02 = D_80075680[1];
        arg0->unk_04 = D_80075680[2];
    }
}

void EventBattle_OpenSettingsPanel(unk_D_82305A40* arg0) {
    Audio_PlaySoundEffectById(SFX_PANEL_OPEN);
    arg0->unk_00 = 2;
    arg0->unk_0C = 0;
}

void EventBattle_DrawBackground(void) {
    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetCombineLERP(gDisplayListHead++, ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE, TEXEL0, 0, ENVIRONMENT, 0,
                      ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE, TEXEL0, 0, ENVIRONMENT, 0);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetEnvColor(gDisplayListHead++, 140, 190, 200, 255);
    gDPSetPrimColor(gDisplayListHead++, 0, 0, 110, 160, 170, 255);
    gSPClearGeometryMode(gDisplayListHead++, G_ZBUFFER | G_LIGHTING);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_302EBC0, G_IM_FMT_I, 128, 64, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 7, 6, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(0, 0, 640, 480, 0, 0, 0x400, 0x400, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void EventBattle_DrawFrame(s16 arg0) {
    s16 i;

    BgStage_DrawFrame();
    EventBattle_DrawBackground();
    EventBattle_DrawTitleBar();
    EventBattle_DrawInfoBox(&D_82305A30);
    EventBattle_DrawNoticeBanner(&D_82305A38);
    EventBattle_DrawSettingsPanel(&D_82305A40);
    EventBattle_DrawTrainerCard(&D_82305AF8[0]);
    EventBattle_DrawTrainerCard(&D_82305AF8[1]);

    for (i = 0; i < 6; i++) {
        EventBattle_DrawRuleCard(&D_82305A50[i]);
    }

    for (i = 0; i < 3; i++) {
        EventBattle_DrawOptionLabel(&D_8230F528[i]);
    }

    if (arg0 >= 0) {
        EventBattle_DrawSelectionCorners(gEventBattleModes[arg0].unk_00, gEventBattleModes[arg0].unk_02,
                                         gEventBattleModes[arg0].unk_04, gEventBattleModes[arg0].unk_06);
    }

    BgStage_AdvanceFrame();
}

void EventBattle_UpdateWidgets(void) {
    s16 i;

    Controller_PollInputs();
    EventBattle_UpdateInfoBox(&D_82305A30);
    EventBattle_UpdateNoticeBanner(&D_82305A38);
    EventBattle_UpdateSettingsPanel(&D_82305A40);
    EventBattle_UpdateTrainerCardSlide(&D_82305AF8[0]);
    EventBattle_UpdateTrainerCardSlide(&D_82305AF8[1]);

    for (i = 0; i < 6; i++) {
        EventBattle_UpdateRuleCardSlide(&D_82305A50[i]);
    }

    for (i = 0; i < 3; i++) {
        EventBattle_UpdateOptionLabel(&D_8230F528[i]);
    }
}

void EventBattle_UpdateModeSelect(unk_D_82305A28* arg0) {
    if (arg0->unk_00 == 3) {
        do {
            EventBattle_UpdateWidgets();
            if (gPlayer1Controller->buttonPressed & 0x800) {
                if (arg0->unk_02 >= 2) {
                    arg0->unk_02 -= 2;
                    Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                }
            } else if (gPlayer1Controller->buttonPressed & 0x400) {
                if (arg0->unk_02 < 4) {
                    arg0->unk_02 += 2;
                    Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                }
            } else if (gPlayer1Controller->buttonPressed & 0x200) {
                if ((arg0->unk_02 & 1) == 1) {
                    arg0->unk_02--;
                    Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                }
            } else if (gPlayer1Controller->buttonPressed & 0x100) {
                if (!(arg0->unk_02 & 1)) {
                    arg0->unk_02++;
                    Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                }
            } else if (gPlayer1Controller->buttonPressed & 0x8000) {
                switch (arg0->unk_02) {
                    case 4:
                        arg0->unk_00 = 5;
                        Audio_PlaySoundEffectById(28);
                        break;

                    case 5:
                        arg0->unk_00 = 2;
                        Audio_PlaySoundEffectById(SFX_MENU_BACK);
                        break;

                    default:
                        arg0->unk_00 = 7;
                        Audio_PlaySoundEffectById(28);
                        break;
                }
            } else if (gPlayer1Controller->buttonPressed & 0x4000) {
                Audio_PlaySoundEffectById(SFX_MENU_BACK);
                arg0->unk_00 = 2;
            }
            EventBattle_DrawFrame(arg0->unk_02);
        } while (arg0->unk_00 == 3);
    }
    arg0->unk_06 = 4;
}

void EventBattle_EnterModeSelect(unk_D_82305A28* arg0) {
    s16 i;
    s16 j;

    Audio_PlayMusicIfChanged(45);
    StageFade_StartFromOpaque(8);

    for (i = 0; i < 6; i++) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    D_82305A30.expanding = 1;
    Audio_PlayCategory11SoundCommand(0x01100001, 0, 0);

    for (j = 0; j < 6; j++) {
        D_82305A50[j].visible = 2;
    }

    for (i = 0; i < 12; i++) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    arg0->unk_00 = 3;
}

void EventBattle_ExitModeSelect(unk_D_82305A28* arg0) {
    s16 i;
    s16 j;

    D_82305A30.expanding = 2;

    for (i = 0; i < 6; i++) {
        D_82305A50[i].visible = 3;
    }

    for (j = 0; j < 6; j++) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    Audio_StopMusic(16);
    StageContext_SetClearColor(0xFFFF);
    StageFade_StartFromTransparent(8);

    while (StageContext_GetFadeMode() != 1) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    arg0->unk_00 = 0;
}

void EventBattle_ValidateTeamSelection(unk_D_82305A28* arg0) {
    s16 i;
    s16 j;
    u8 tmp1;

    D_82305A30.expanding = 2;

    for (i = 0; i < 6; i++) {
        D_82305A50[i].visible = 3;
    }

    for (j = 0; j < 12; j++) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    Audio_PlayCategory11SoundCommand(0x01100001, 0, 0);
    arg0->unk_01 = EventBattle_ValidateTeamForRule(&D_82305AF8[0], gEventBattleModes[arg0->unk_02].unk_08);
    arg0->unk_01 &= EventBattle_ValidateTeamForRule(&D_82305AF8[1], gEventBattleModes[arg0->unk_02].unk_08);
    EventBattle_ShowNoticeBanner(&D_82305A38, arg0->unk_01);

    tmp1 = !arg0->unk_01;
    arg0->unk_04 = tmp1;

    for (i = tmp1; i < 3; i++) {
        D_8230F528[i].animState = 1;
    }

    for (j = 0; j < 12; j++) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    arg0->unk_00 = 4;
}

void EventBattle_ReturnToModeSelect(unk_D_82305A28* arg0) {
    u8 temp_v1;
    s16 i;
    s16 j;

    temp_v1 = !arg0->unk_01;
    D_82305AF8[0].unk_0000 = 3;
    D_82305AF8[1].unk_0000 = 3;
    D_82305A38.expanding = 2;

    for (i = temp_v1; i < 3; i++) {
        D_8230F528[i].animState = 2;
    }

    for (j = 0; j < 12; j++) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    D_82305A30.expanding = 1;
    Audio_PlayCategory11SoundCommand(0x01100001, 0, 0);

    for (i = 0; i < 6; i++) {
        D_82305A50[i].visible = 2;
    }

    for (j = 0; j < 12; j++) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    arg0->unk_00 = 3;
}

void EventBattle_ValidateInitialSelection(unk_D_82305A28* arg0) {
    u8 temp_v1;
    s16 i;
    s16 j;

    Audio_PlayCategory11SoundCommand(0x01100001, 0, 0);
    arg0->unk_01 = EventBattle_ValidateTeamForRule(&D_82305AF8[0], gEventBattleModes[arg0->unk_02].unk_08);
    arg0->unk_01 &= EventBattle_ValidateTeamForRule(&D_82305AF8[1], gEventBattleModes[arg0->unk_02].unk_08);
    EventBattle_ShowNoticeBanner(&D_82305A38, arg0->unk_01);

    temp_v1 = !arg0->unk_01;
    arg0->unk_04 = temp_v1;

    for (i = temp_v1; i < 3; i++) {
        D_8230F528[i].animState = 1;
    }

    for (j = 0; j < 12; j++) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    arg0->unk_00 = 4;
}

void EventBattle_MarkPartnerPikachu(BattleMon* arg0, s16 arg1) {
    GbSavePlayerIdentity sp30;
    GbSavePlayerIdentity sp20;

    if ((GbSave_GetPortGame(arg1) == 7) && (arg0->species.id == 25)) {
        GbSave_CopyPlayerIdentity(arg1, &sp20);
        _bcopy(arg0->unk_46, sp30.unk_02, 11);
        sp30.unk_00 = arg0->unk_0E;
        if (GbSave_PlayerIdentityMatches(&sp30, &sp20)) {
            arg0->unk_52 |= 0x80;
        }
    }
}

void EventBattle_BuildTrainerFromCard(unk_D_82305AF8* arg0) {
    s16 i;
    TeamRoster* temp_s4 = Trainer_Create(arg0->unk_0001, arg0->unk_0012, arg0->unk_0014, Text_GetPlayerLabel(1));

    for (i = 0; i < 6; i++) {
        EventBattle_MarkPartnerPikachu(&arg0->unk_0020[i], arg0->unk_0001);
        Trainer_AddPokemon(temp_s4, &arg0->unk_0020[i]);
    }

    Team_AddTrainer(arg0->unk_0001, temp_s4);
}

void EventBattle_StartBattle(unk_D_82305A28* arg0) {
    s16 i;
    s16 j;

    Session_SetMode(10, gEventBattleModes[arg0->unk_02].unk_08, 0);
    EventBattle_BuildTrainerFromCard(&D_82305AF8[0]);
    EventBattle_BuildTrainerFromCard(&D_82305AF8[1]);

    if (D_82305A40.unk_01 == 1) {
        gSessionContext.timeLimitsEnabled = D_82305A40.unk_01;
        gSessionContext.battleLimitMinutes = D_82305A40.unk_02;
        gSessionContext.orderLimitSeconds = D_82305A40.unk_04;
    }
    D_82305AF8[0].unk_0000 = 3;
    D_82305AF8[1].unk_0000 = 3;
    D_82305A38.expanding = 2;

    for (i = 0; i < 3; i++) {
        D_8230F528[i].animState = 2;
    }

    for (j = 0; j < 6; j++) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    Audio_StopMusic(16);
    StageContext_SetClearColor(0xFFFF);
    StageFade_StartFromTransparent(8);

    while (StageContext_GetFadeMode() != 1) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    arg0->unk_06 = 42;
    arg0->unk_00 = 0;
}

void EventBattle_UpdateActionMenu(unk_D_82305A28* arg0) {
    u8 temp_s7;

    temp_s7 = !arg0->unk_01;

    if (arg0->unk_00 == 4) {
        do {
            EventBattle_UpdateWidgets();
            if (gPlayer1Controller->buttonPressed & 0x200) {
                if (temp_s7 < arg0->unk_04) {
                    arg0->unk_04--;
                    Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                }
            } else if (gPlayer1Controller->buttonPressed & 0x100) {
                if (arg0->unk_04 < 2) {
                    arg0->unk_04++;
                    Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                }
            } else if (gPlayer1Controller->buttonPressed & 0x8000) {
                switch (arg0->unk_04) {
                    case 0:
                        arg0->unk_00 = 6;
                        Audio_PlaySoundEffectById(32);
                        break;

                    case 1:
                        arg0->unk_00 = 9;
                        Audio_PlaySoundEffectById(28);
                        break;

                    case 2:
                        arg0->unk_00 = 8;
                        Audio_PlaySoundEffectById(SFX_MENU_BACK);
                        break;
                }
            } else if (gPlayer1Controller->buttonPressed & 0x4000) {
                Audio_PlaySoundEffectById(SFX_MENU_BACK);
                arg0->unk_04 = 2;
                arg0->unk_00 = 8;
            }
            EventBattle_DrawFrame(arg0->unk_04 + 6);
        } while (arg0->unk_00 == 4);
    }
}

void EventBattle_RunSettingsPanel(unk_D_82305A28* arg0) {
    s16 i;
    s16 j;

    D_82305A30.expanding = 2;

    for (i = 0; i < 6; i++) {
        D_82305A50[i].visible = 3;
    }

    for (j = 0; j < 12; j++) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    EventBattle_OpenSettingsPanel(&D_82305A40);

    while (D_82305A40.unk_00 != 0) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    D_82305A30.expanding = 1;
    Audio_PlayCategory11SoundCommand(0x01100001, 0, 0);

    for (i = 0; i < 6; i++) {
        D_82305A50[i].visible = 2;
    }

    for (j = 0; j < 12; j++) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    arg0->unk_00 = 3;
}

void EventBattle_ExitWithMode(unk_D_82305A28* arg0) {
    s16 i;
    s16 j;

    Session_SetMode(10, gEventBattleModes[arg0->unk_02].unk_08, 0);

    D_82305AF8[0].unk_0000 = 3;
    D_82305AF8[1].unk_0000 = 3;
    D_82305A38.expanding = 2;

    for (i = 0; i < 3; i++) {
        D_8230F528[i].animState = 2;
    }

    for (j = 0; j < 12; j++) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    arg0->unk_06 = 18;
    arg0->unk_00 = 0;
}

void EventBattle_RunSelect(unk_D_82305A28* arg0) {
    while (arg0->unk_00 != 0) {
        switch (arg0->unk_00) {
            case 1:
                EventBattle_EnterModeSelect(arg0);
                break;

            case 2:
                EventBattle_ExitModeSelect(arg0);
                break;

            case 3:
                EventBattle_UpdateModeSelect(arg0);
                break;

            case 4:
                EventBattle_UpdateActionMenu(arg0);
                break;

            case 7:
                EventBattle_ValidateTeamSelection(arg0);
                break;

            case 8:
                EventBattle_ReturnToModeSelect(arg0);
                break;

            case 6:
                EventBattle_StartBattle(arg0);
                break;

            case 5:
                EventBattle_RunSettingsPanel(arg0);
                break;

            case 9:
                EventBattle_ExitWithMode(arg0);
                break;

            case 10:
                EventBattle_ValidateInitialSelection(arg0);
                break;
        }
    }
}

void EventBattle_InitSelect(unk_D_82305A28* arg0, s16 arg1) {
    s16 i;

    if (arg1 == -1) {
        arg0->unk_00 = 1;
        arg0->unk_02 = 2;
    } else {
        arg0->unk_00 = 10;
        arg0->unk_02 = arg1;
    }
    arg0->unk_06 = 18;

    EventBattle_InitInfoBox(&D_82305A30, 56, 380);
    EventBattle_InitNoticeBanner(&D_82305A38, 270, 192);
    EventBattle_InitSettingsPanel(&D_82305A40, 104, 147, (arg1 + 1) == 0);

    EventBattle_InitTrainerCard(&D_82305AF8[0], 64, 92, 0);
    EventBattle_InitTrainerCard(&D_82305AF8[1], 64, 248, 1);

    EventBattle_InitRuleCard(&D_82305A50[0], 68, 132, D_3028E00, Text_GetString(NULL, 0, sEventBattleText, 21), 3, 0);
    EventBattle_InitRuleCard(&D_82305A50[1], 324, 132, D_3029A80, Text_GetString(NULL, 0, sEventBattleText, 24), 4, 1);
    EventBattle_InitRuleCard(&D_82305A50[2], 68, 200, D_302A700, Text_GetString(NULL, 0, sEventBattleText, 26), 6, 5);
    EventBattle_InitRuleCard(&D_82305A50[3], 324, 200, D_302B380, Text_GetString(NULL, 0, sEventBattleText, 25), 5, 3);
    EventBattle_InitRuleCard(&D_82305A50[4], 68, 268, NULL, Text_GetString(NULL, 0, sEventBattleText, 27), 0, 6);
    EventBattle_InitRuleCard(&D_82305A50[5], 324, 268, NULL, Text_GetString(NULL, 0, sEventBattleText, 28), 0, 7);

    EventBattle_InitOptionLabel(&D_8230F528[0], 80, 385, Text_GetString(NULL, 0, sEventBattleText, 29));
    EventBattle_InitOptionLabel(&D_8230F528[1], 244, 385, Text_GetString(NULL, 0, sEventBattleText, 30));
    EventBattle_InitOptionLabel(&D_8230F528[2], 408, 385, Text_GetString(NULL, 0, sEventBattleText, 31));

    if (arg1 == -1) {
        for (i = 2; i >= 0; i--) {
            if (D_82305A50[i].displayEligibility != 0) {
                arg0->unk_02 = i;
            }
        }
    }
}

s16 EventBattle_ShowSelect(void) {
    static s16 D_823059CC = -1;

    main_pool_push_state('EVSL');

    Font_Init(28, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_2000000, common_menu2_ui, 0);
    ASSET_LOAD(D_3000000, menu_select_ui, 0);

    PokeIcon_OpenModelArchive();
    GbSave_EnsureMainDataLoaded(0);
    GbSave_EnsureMainDataLoaded(1);
    StageLoader_UpdateSegments();
    EventBattle_InitSelect(&D_82305A28, D_823059CC);
    EventBattle_RunSelect(&D_82305A28);
    D_823059CC = D_82305A28.unk_02;
    StageLoader_WaitForRetrace();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('EVSL');

    return D_82305A28.unk_06;
}

s32 EventBattle_Main(UNUSED s32 arg0, UNUSED s32 arg1) {
    s16 temp_v0 = STATE_EVENT_BATTLE;

    main_pool_push_state('EBTL');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    StageContext_Activate(StageContext_Allocate(1, 0, 2, 0, 2, 1));
    Text_InitStringTables();
    sEventBattleText = Text_GetStringTable(16);
    sPokemonDetailText = Text_GetStringTable(6);

    while (temp_v0 == STATE_EVENT_BATTLE) {
        temp_v0 = EventBattle_ShowSelect();
        if (temp_v0 == STATE_EVENT_BATTLE) {
            FRAGMENT_LOAD_AND_CALL(FRAGMENT_RENTAL_RULES, 0, 0);
        }
    }

    StageContext_Deactivate();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('EBTL');

    return temp_v0;
}
