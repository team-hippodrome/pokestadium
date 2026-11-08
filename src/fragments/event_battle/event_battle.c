#include "event_battle.h"
#include "game_state.h"
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
#include "pokemon.h"

static char** sEventBattleText;
static char** sPokemonDetailText;
static EventBattleMenuState sEventBattleMenuState;
static EventBattleDescriptionPanel sDescriptionPanel;
static EventBattleDescriptionPanel sPartyEligibilityBanner;
static EventBattleSettingsPanel sSettingsPanel;
static EventBattleSelectionOption sSelectionOptions[6];
static EventBattleTrainerParty sTrainerCards[2];
static EventBattleActionLabel sActionLabels[3];

// FAKE: DrawFrame reads entry 8 beyond this eight-entry table; adjacent
// sBackActionBoundsData supplies the back action's x, y, width, and height.
static EventBattleSelectionEntry sSelectionEntries[] = {
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
static s16 sBackActionBoundsData[] = {
    408, 385, 152, 48, 0, 0,
};
static EventBattleCupRules sCupRules[] = {
    // Nintendo Cup variants share the same level range, combined cap, and species mask.
    {
        50,
        55,
        155,
        0x01,
    },
    {
        50,
        55,
        155,
        0x01,
    },
    {
        50,
        55,
        155,
        0x01,
    },
    // Petite Cup
    {
        25,
        30,
        80,
        0x08,
    },
    // Pika Cup
    {
        15,
        20,
        50,
        0x10,
    },
    // Prime Cup
    {
        1,
        100,
        300,
        0x20,
    },
};

/* Draw selection-corner textures with a sinusoidal inset around a rectangle. */
void EventBattle_DrawSelectionCorners(s16 x, s16 y, s16 width, s16 height) {
    static s16 sSelectionCornerPhase = 0;

    s16 pulse = SINS(sSelectionCornerPhase) * 2;
    UNUSED s32 pad[2]; // FAKE: unused stack padding retained for matching.

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetEnvColor(gDisplayListHead++, 0xF0, 0xD4, 0x68, 0xFF); // #F0D468

    Gfx_DrawTextureIa8(x + pulse, y + pulse, 16, 16, gBottomLeftSelectionCorner, 16, 0);
    Gfx_DrawTextureIa8(x + pulse, ((y + height) - pulse) - 16, 16, 16, gTopLeftSelectionCorner, 16, 0);
    Gfx_DrawTextureIa8(((x + width) - pulse) - 16, y + pulse, 16, 16, gBottomRightSelectionCorner, 16, 0);
    Gfx_DrawTextureIa8(((x + width) - pulse) - 16, ((y + height) - pulse) - 16, 16, 16, gTopRightSelectionCorner,
                       16, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    sSelectionCornerPhase += 0x2000;
}

/* Draw the textured outline used by selection cards and action labels. */
void EventBattle_DrawSelectionOutline(s16 x, s16 y, s16 width, s16 height) {
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
/* Draw a tinted filled warning panel with scaled 16-pixel corner textures. */
void EventBattle_DrawWarningPanelBackground(s16 x, s16 y, s16 width, s16 height, u8 r, u8 g, u8 b, u8 alpha) {
    s16 height_adjustment;
    s16 width_adjustment;
    s16 inner_width;
    s16 inner_height;

    if (width >= 32) {
        width_adjustment = 16;
    } else {
        width_adjustment = width / 2;
    }

    if (height >= 32) {
        height_adjustment = 16;
    } else {
        height_adjustment = height / 2;
    }

    inner_width = width - (width_adjustment * 2);
    inner_height = height - (height_adjustment * 2);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetEnvColor(gDisplayListHead++, r, g, b, alpha);
    gDPLoadTextureBlock_4b(gDisplayListHead++, D_20288E0, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(x, y, width_adjustment, height_adjustment, 0, 0, 0x4000 / width_adjustment, 0x4000 / height_adjustment, 0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_2028960, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped((x + width) - width_adjustment, y, width_adjustment, height_adjustment, 0, 0, 0x4000 / width_adjustment, 0x4000 / height_adjustment,
                                0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_20287E0, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(x, (y + height) - height_adjustment, width_adjustment, height_adjustment, 0, 0, 0x4000 / width_adjustment, 0x4000 / height_adjustment,
                                0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_2028860, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped((x + width) - width_adjustment, (y + height) - height_adjustment, width_adjustment, height_adjustment, 0, 0, 0x4000 / width_adjustment,
                                0x4000 / height_adjustment, 0);

    gSPDisplayList(gDisplayListHead++, gUiTranslucentFillDL);

    if ((inner_width > 0) && (inner_height > 0)) {
        Gfx_FillRectRgba(x + width_adjustment, y, inner_width, height_adjustment, r, g, b, alpha);
        Gfx_FillRectRgba(x, y + width_adjustment, width, inner_height, r, g, b, alpha);
        Gfx_FillRectRgba(x + width_adjustment, y + width_adjustment + inner_height, inner_width, height_adjustment, r, g, b, alpha);
    } else if (inner_width > 0) {
        Gfx_FillRectRgba(x + width_adjustment, y, inner_width, height, r, g, b, alpha);
    } else if (inner_height > 0) {
        Gfx_FillRectRgba(x, y + height_adjustment, width, inner_height, r, g, b, alpha);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

/* Draw a filled description panel with corner extents clamped to eight pixels. */
void EventBattle_DrawDescriptionBackground(s16 x, s16 y, s16 width, s16 height, u8 r, u8 g, u8 b, u8 alpha) {
    s16 inner_width;
    s16 inner_height;
    s16 corner_height;
    s16 corner_width;

    if (width >= 16) {
        corner_width = 8;
    } else {
        corner_width = width / 2;
    }

    if (height >= 16) {
        corner_height = 8;
    } else {
        corner_height = height / 2;
    }

    inner_width = width - (corner_width * 2);
    inner_height = height - (corner_height * 2);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetEnvColor(gDisplayListHead++, r, g, b, alpha);
    gDPLoadTextureBlock_4b(gDisplayListHead++, gUiPanelTopLeftTexture, G_IM_FMT_I, 16, 8, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(x, y, corner_width, corner_height, 0, 0, 0x2000 / corner_width, 0x2000 / corner_height, 0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, gUiPanelTopRightTexture, G_IM_FMT_I, 16, 8, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped((x + width) - corner_width, y, corner_width, corner_height, 0, 0, 0x2000 / corner_width, 0x2000 / corner_height,
                                0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, gUiPanelBottomLeftTexture, G_IM_FMT_I, 16, 8, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(x, (y + height) - corner_height, corner_width, corner_height, 0, 0, 0x2000 / corner_width, 0x2000 / corner_height,
                                0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, gUiPanelBottomRightTexture, G_IM_FMT_I, 16, 8, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped((x + width) - corner_width, (y + height) - corner_height, corner_width, corner_height, 0, 0, 0x2000 / corner_width,
                                0x2000 / corner_height, 0);

    gSPDisplayList(gDisplayListHead++, gUiTranslucentFillDL);

    if ((inner_width > 0) && (inner_height > 0)) {
        Gfx_FillRectRgba(x + corner_width, y, inner_width, corner_height, r, g, b, alpha);
        Gfx_FillRectRgba(x, y + corner_width, width, inner_height, r, g, b, alpha);
        Gfx_FillRectRgba(x + corner_width, (y + corner_width) + inner_height, inner_width, corner_height, r, g, b, alpha);
    } else if (inner_width > 0) {
        Gfx_FillRectRgba(x + corner_width, y, inner_width, height, r, g, b, alpha);
    } else if (inner_height > 0) {
        Gfx_FillRectRgba(x, y + corner_height, width, inner_height, r, g, b, alpha);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

/* Draw a tinted textured border and solid interior for a rectangular panel. */
void EventBattle_DrawFilledPanel(s16 x, s16 y, s16 width, s16 height, u8 r, u8 g, u8 b, u8 alpha) {
    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetEnvColor(gDisplayListHead++, r, g, b, alpha);

    Gfx_DrawTextureIa8(x, y, 8, 8, D_2000B40, 8, 0);
    Gfx_DrawTextureIa8((x + width) - 8, y, 8, 8, D_2000B80, 8, 0);
    Gfx_DrawTexturedRectClipped(x + 8, y, width - 16, 8, 0, 0, 0, 0x400, 0);
    Gfx_DrawTextureIa8(x, (y + height) - 8, 8, 8, D_2000BC0, 8, 0);
    Gfx_DrawTexturedRectClipped(x, y + 8, 8, height - 16, 0, 0, 0x400, 0, 0);
    Gfx_DrawTextureIa8((x + width) - 8, (y + height) - 8, 8, 8, D_2000C00, 8, 0);
    Gfx_DrawTexturedRectClipped(x + 8, (y + height) - 8, width - 16, 8, 0, 0, 0, 0x400, 0);
    Gfx_DrawTexturedRectClipped((x + width) - 8, y + 8, 8, height - 16, 0, 0, 0x400, 0, 0);

    gSPDisplayList(gDisplayListHead++, gUiTranslucentFillDL);

    Gfx_FillRectRgba(x + 8, y + 8, width - 16, height - 16, r, g, b, alpha);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

/* Draw the title-bar Poke Ball from its two 48-by-24 texture halves. */
void EventBattle_DrawTitlePokeBall(s16 x, s16 y) {
    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    Gfx_DrawTextureRgba16(x, y, 48, 24, gPokeBallIconTop, 48, 0);
    Gfx_DrawTextureRgba16(x, y + 24, 48, 24, gPokeBallIconBottom, 48, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

/* Draw a 32-pixel-tall title strip using environment and primitive RGB colors. */
void EventBattle_DrawTitleBarBackground(s16 x, s16 y, s16 width, Color_RGBA8* env_color, Color_RGBA8* prim_color) {
    gSPDisplayList(gDisplayListHead++, D_8006F558);

    gDPSetEnvColor(gDisplayListHead++, env_color->r, env_color->g, env_color->b, 0xFF);
    gDPSetPrimColor(gDisplayListHead++, 0, 0, prim_color->r, prim_color->g, prim_color->b, 0xFF);

    Gfx_DrawTextureIa8(x, y, 16, 32, D_20003C0, 16, 0);
    Gfx_DrawTextureIa8((x + width) - 16, y, 16, 32, D_20005C0, 16, 0);
    Gfx_DrawTexturedRectClipped(x + 16, y, width - 32, 32, 0, 0, 0, 0x400, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

/* Draw the title-bar background, Poke Ball, and shadowed event-battle title. */
void EventBattle_DrawTitleBar(void) {
    Color_RGBA8 env_color;
    Color_RGBA8 prim_color;

    Color_SetRGB(&env_color, 0x64, 0x64, 0xC8); // #6464C8
    Color_SetRGB(&prim_color, 0x28, 0x28, 0x8C); // #28288C
    EventBattle_DrawTitleBarBackground(72, 40, 520, &env_color, &prim_color);
    EventBattle_DrawTitlePokeBall(48, 32);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(0x10, 0);
    Gfx_SetEnvColor(0x00, 0x00, 0x00, 0xFF); // #000000
    Font_Printf(106, 44, Text_GetString(NULL, 0, sEventBattleText, 8));
    Gfx_SetEnvColor(0xFF, 0xFF, 0x77, 0xFF); // #FFFF77
    Font_Printf(104, 42, Text_GetString(NULL, 0, sEventBattleText, 8));
    Font_EndTexturedTextRendering();
}

/* Draw the animated description panel and its selected-option text when fully open. */
void EventBattle_DrawDescriptionPanel(EventBattleDescriptionPanel* panel) {
    s16 opening_progress;
    s16 x;
    s16 y;

    if (panel->height >= 16) {
        opening_progress = panel->height;
        x = panel->x;
        y = panel->y + ((60 - opening_progress) / 2);

        EventBattle_DrawDescriptionBackground(x, y, 528, opening_progress, 0x1E, 0x1E, 0x82, 0x96); // #1E1E82
        if (((60 - opening_progress) == 0) && (sEventBattleMenuState.cursorPosition < 8)) {
            Font_BeginTranslucentTextRendering();
            Font_SetActive(0x08, 0);
            Font_SetLineHeight(24);
            Font_Printf(x + 32, y + 8,
                        Text_GetString(NULL, 0, sEventBattleText, sSelectionOptions[sEventBattleMenuState.cursorPosition].descriptionTextId));
            Font_EndTexturedTextRendering();
        }
    }
}

/* Advance the description-panel height toward 60 pixels or zero in six-pixel steps. */
void EventBattle_UpdateDescriptionPanel(EventBattleDescriptionPanel* panel) {
    switch (panel->animationState) {
        case 1:
            panel->height += 6;
            if (panel->height == 60) {
                panel->animationState = 0;
            }
            break;

        case 2:
            panel->height -= 6;
            if (panel->height == 0) {
                panel->animationState = 0;
            }
            break;
    }
}

/* Initialize a closed description panel at the supplied position. */
void EventBattle_InitDescriptionPanel(EventBattleDescriptionPanel* panel, s16 x, s16 y) {
    panel->animationState = 0;
    panel->height = 0;
    panel->x = x;
    panel->y = y;
}

/* Draw a selection card with its label and optional cup icon, eligibility colors, and level range. */
void EventBattle_DrawSelectionOption(EventBattleSelectionOption* selection_option) {
    s16 x;
    s16 y;

    if (selection_option->slideState != 0) {
        x = selection_option->currentX;
        y = selection_option->currentY;

        EventBattle_DrawSelectionOutline(x, y, 248, 56);

        gSPDisplayList(gDisplayListHead++, D_8006F470);

        if (selection_option->icon != NULL) {
            if (selection_option->bothPartiesEligible != 0) {
                Gfx_FillRectRgb(x + 4, y + 4, 240, 48, 0x1E, 0x1E, 0x64); // #1E1E64
            } else {
                Gfx_FillRectRgb(x + 4, y + 4, 240, 48, 0x64, 0x1E, 0x1E); // #641E1E
            }

            gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

            Gfx_DrawTextureRgba16(x + 8, y + 8, 40, 40, selection_option->icon, 40, 0);
        } else {
            Gfx_FillRectRgb(x + 4, y + 4, 240, 48, 0x1E, 0x5A, 0x1E); // #1E5A1E
        }

        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

        Font_BeginTranslucentTextRendering();
        if (selection_option->icon != NULL) {
            s32 text_y = y + 8;
            char format[64];

            Font_SetActive(0x08, 0);

            // Cup Title Color
            if (selection_option->bothPartiesEligible != 0) {
                Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #FFFFFF
            } else {
                Gfx_SetEnvColor(0xDC, 0x37, 0x37, 0xFF); // #DC3737
            }

            Font_Printf(x + 52, text_y, selection_option->text);
            Font_SetActive(0x04, 0);
            // Cup Level Range Color
            Gfx_SetEnvColor(0x92, 0xC1, 0xFF, 0xFF); // #92C1FF
            sprintf(format, "%s%d~%d", Text_GetString(NULL, 0, sPokemonDetailText, 21), selection_option->minLevel, selection_option->maxLevel);
            Font_Printf((x - (s16)Font_MeasureTextExtent(0x04, 0, format)) + 234, y + 32, format);
        } else {
            Font_SetActive(0x08, 0);
            Font_Printf((x - ((s16)Font_MeasureTextExtent(0x08, 0, selection_option->text) / 2)) + 122, y + 18,
                        selection_option->text);
        }
        Font_EndTexturedTextRendering();
    }
}

/* Advance a selection card's horizontal entrance or exit in 32-pixel steps. */
void EventBattle_UpdateSelectionOptionSlide(EventBattleSelectionOption* option) {
    switch (option->slideState) {
        case 2:
            option->slideOffsetSteps -= 1;
            if (option->slideOffsetSteps == 0) {
                option->slideState = 1;
            }

            if (option->restingX < 320) {
                option->currentX = option->restingX - (option->slideOffsetSteps << 5);
            } else {
                option->currentX = option->restingX + (option->slideOffsetSteps << 5);
            }
            break;

        case 3:
            option->slideOffsetSteps += 1;
            if (option->slideOffsetSteps == 10) {
                option->slideState = 0;
            }

            if (option->restingX < 320) {
                option->currentX = option->restingX - (option->slideOffsetSteps << 5);
            } else {
                option->currentX = option->restingX + (option->slideOffsetSteps << 5);
            }
            break;
    }
}

/* Initialize a hidden card and derive cup levels and both-party eligibility for a supplied ruleset. */
void EventBattle_InitSelectionOption(EventBattleSelectionOption* option, s16 x, s16 y, u8* icon, s8* text, s16 rule_category, s16 description_text_id) {
    option->slideState = 0;
    option->slideOffsetSteps = 10;
    option->restingX = x;
    option->restingY = y;
    option->currentX = x;
    option->currentY = y;
    option->icon = icon;
    option->text = text;
    option->minLevel = 0;
    option->maxLevel = 0;
    option->descriptionTextId = description_text_id;
    option->bothPartiesEligible = 1;

    if (rule_category != 0) {
        option->minLevel = sCupRules[rule_category - 1].minLevel;
        option->maxLevel = sCupRules[rule_category - 1].maxLevel;
        option->bothPartiesEligible = EventBattle_CheckPartyEligibility(&sTrainerCards[0], rule_category);
        option->bothPartiesEligible &= EventBattle_CheckPartyEligibility(&sTrainerCards[1], rule_category);
    }
}

/* Draw a Pokemon slot panel with an ineligibility tint or its controller-side tint. */
void EventBattle_DrawPokemonSlotBackground(s16 controller_port, s16 x, s16 y, s32 is_ineligible) {
    if (is_ineligible != 0) {
        EventBattle_DrawFilledPanel(x, y, 132, 44, 0x64, 0x1E, 0x1E, 0xFF); // #641E1E
    } else if (controller_port == 0) {
        EventBattle_DrawFilledPanel(x, y, 132, 44, 0x3C, 0x3C, 0xA0, 0xFF); // #3C3CA0
    } else {
        EventBattle_DrawFilledPanel(x, y, 132, 44, 0x1E, 0x64, 0x1E, 0xFF); // #1E641E
    }
}

static s16 sPokemonPartySlotXOffsets[] = { 120, 252, 384, 120, 252, 384 };
static s16 sPokemonPartySlotYOffsets[] = { 4, 4, 4, 48, 48, 48 };

/* Draw the trainer identity panel and six Pokemon-slot backgrounds for a party. */
void EventBattle_DrawPartyPanelBackground(EventBattleTrainerParty* trainer_card, s16 x, s16 y) {
    s16 i;
    s16 controller_port;
    s16 slot_x;
    s16 slot_y;

    EventBattle_DrawSelectionOutline(x, y, 520, 96);
    if (trainer_card->controllerPort == 0) {
        EventBattle_DrawFilledPanel(x + 4, y + 4, 116, 88, 0x64, 0x64, 0xC8, 0xFF); // #6464C8
    } else {
        EventBattle_DrawFilledPanel(x + 4, y + 4, 116, 88, 0x1E, 0x82, 0x1E, 0xFF); // #1E821E
    }

    for (i = 0; i < 6; i++) {
        controller_port = trainer_card->controllerPort;
        slot_x = sPokemonPartySlotXOffsets[i] + x;
        slot_y = sPokemonPartySlotYOffsets[i] + y;
        EventBattle_DrawPokemonSlotBackground(controller_port, slot_x, slot_y, trainer_card->pokemon[i].unk_25);
    }
}

/* Draw and pulse the warning when the three lowest party levels exceed the combined level cap. */
void EventBattle_DrawLevelCapWarning(EventBattleTrainerParty* trainer_card, s16 x, s16 y) {
    char warning_text_buffer[256];
    s16 alpha;

    if (trainer_card->levelCapCompliancePossible == 0) {
        alpha = (trainer_card->levelCapWarningPhase >= 0) ? (s16)(SINS((s16)(SINS(trainer_card->levelCapWarningPhase) * 16384.0f)) * 127.0f) + 127
                                     : (s16)(SINS(trainer_card->levelCapWarningPhase) * 127.0f) + 127;

        Gfx_FillRectWithDisplayState(x + 120, y + 4, 396, 88, 0x64, 0x1E, 0x1E, (alpha * 2) / 3); // #641E1E
        Font_BeginTranslucentTextRendering();
        Font_SetActive(0x10, 0);
        Gfx_SetEnvColor(0xFF, 0xFF, 0x00, alpha); // #FFFF00
        Font_SetLineHeight(32);
        Text_SetNumberToken(3, sCupRules[trainer_card->ruleCategory - 1].levelCap);
        Font_Printf(x + 136, y + 20, Text_GetString(warning_text_buffer, 256, sEventBattleText, 9));
        Font_EndTexturedTextRendering();
        trainer_card->levelCapWarningPhase += 0x300;
    }
}

/* Draw the party's identity, Pokemon icons, nicknames, levels, and optional level-cap warning. */
void EventBattle_DrawTrainerCard(EventBattleTrainerParty* trainer_card) {
    u8* port_label;
    s16 i;
    s16 x;
    s16 y;
    s16 slot_x;
    s16 slot_y;
    s16 pokemon_name_x;
    s16 pokemon_name_y;

    if (trainer_card->slideState != 0) {
        x = trainer_card->currentX;
        y = trainer_card->currentY;

        if (trainer_card->controllerPort == 0) {
            port_label = gPlayer1LabelLarge;
        } else {
            port_label = gPlayer2LabelLarge;
        }

        EventBattle_DrawPartyPanelBackground(trainer_card, x, y);

        gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

        Gfx_DrawTextureRgba16(x + 26, y + 8, 72, 18, port_label, 72, 0);
        Gfx_DrawTextureRgba16(x + 26, y + 26, 72, 18, port_label + 0xA20, 72, 0);

        for (i = 0; i < 6; i++) {
            slot_x = sPokemonPartySlotXOffsets[i] + x + 2;
            slot_y = sPokemonPartySlotYOffsets[i] + y + 2;
            Gfx_DrawTextureRgba16(slot_x, slot_y, 40, 40, trainer_card->pokemonIcons[i], 40, 0);
        }

        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

        Font_BeginTranslucentTextRendering();
        Font_SetActive(0x08, 0);
        Font_Printf((x - (Font_MeasureTextExtent(0x08, 0, trainer_card->trainerName) / 2)) + 62, y + 47, trainer_card->trainerName);
        Font_SetActive(0x04, 0);
        Font_Printf((x - (Font_MeasureTextExtent(0x04, 0, "ID  00000") / 2)) + 62, y + 70, "ID  %05d",
                    trainer_card->trainerId);
        Font_SetActive(0x04, 0);

        for (i = 0; i < 6; i++) {
            slot_x = sPokemonPartySlotXOffsets[i] + x + 44;
            slot_y = sPokemonPartySlotYOffsets[i] + y + 5;
            Font_Printf(slot_x, slot_y, trainer_card->pokemon[i].nickname);
        }

        Font_SetActive(0x04, 0);

        for (i = 0; i < 6; i++) {
            pokemon_name_x = sPokemonPartySlotXOffsets[i] + x + 44;
            pokemon_name_y = sPokemonPartySlotYOffsets[i] + y + 23;
            Font_Printf(pokemon_name_x, pokemon_name_y, "%s%d", Text_GetString(NULL, 0, sPokemonDetailText, 21), trainer_card->pokemon[i].level);
        }

        Font_EndTexturedTextRendering();
        EventBattle_DrawLevelCapWarning(trainer_card, x, y);
    }
}

/* Advance a trainer card's horizontal entrance or exit in 64-pixel steps. */
void EventBattle_UpdateTrainerCardSlide(EventBattleTrainerParty* trainer_card) {
    switch (trainer_card->slideState) {
        case 2:
            trainer_card->slideOffsetSteps -= 1;
            if (trainer_card->slideOffsetSteps == 0) {
                trainer_card->slideState = 1;
            }

            if (trainer_card->controllerPort == 0) {
                trainer_card->currentX = trainer_card->restingX - (trainer_card->slideOffsetSteps << 6);
            } else {
                trainer_card->currentX = trainer_card->restingX + (trainer_card->slideOffsetSteps << 6);
            }
            break;

        case 3:
            trainer_card->slideOffsetSteps += 1;
            if (trainer_card->slideOffsetSteps == 10) {
                trainer_card->slideState = 0;
            }

            if (trainer_card->controllerPort == 0) {
                trainer_card->currentX = trainer_card->restingX - (trainer_card->slideOffsetSteps << 6);
            } else {
                trainer_card->currentX = trainer_card->restingX + (trainer_card->slideOffsetSteps << 6);
            }
            break;
    }
}

/* Initialize a hidden trainer card and load its cartridge identity, six Pokemon, and icon textures. */
void EventBattle_InitTrainerCard(EventBattleTrainerParty* trainer_card, s16 x, s16 y, s16 controller_port) {
    s16 i;

    trainer_card->slideState = 0;
    trainer_card->controllerPort = controller_port;
    trainer_card->slideOffsetSteps = 10;
    trainer_card->restingX = x;
    trainer_card->restingY = y;
    trainer_card->currentX = x;
    trainer_card->currentY = y;
    trainer_card->trainerId = GbSave_GetTrainerId(controller_port);
    GbSave_GetPlayerName(controller_port, trainer_card->trainerName);

    for (i = 0; i < 6; i++) {
        Deck_ReadEntry(32, controller_port, 0, i, &trainer_card->pokemon[i]);
        PokeIcon_LoadModelTextureForMon(trainer_card->pokemonIcons[i], 0, &trainer_card->pokemon[i]);
    }
}

/* Check move IDs against the supported range, stopping at the first empty move slot. */
s32 EventBattle_HasValidMoveIds(BattleMon* pokemon) {
    s32 i;
    s32 has_valid_move_ids = 1;

    for (i = 0; i < 4; i++) {
        if (pokemon->moves[i] == 0) {
            break;
        }

        if (pokemon->moves[i] >= 166) {
            has_valid_move_ids = 0;
        }
    }

    return has_valid_move_ids;
}

/*
 * Mark each slot for species, move-ID, level-range, and duplicate-species eligibility.
 * Return whether all six slots pass, excluding the combined level cap. Cache the
 * three-lowest-level cap result only when all slots pass; otherwise suppress that warning.
 */
u8 EventBattle_CheckPartyEligibility(EventBattleTrainerParty* trainer_card, s16 rule_category) {
    EventBattleCupRules* cup_rules;
    s16 lowest_levels[3];
    BattleMon* pokemon;
    s16 i;
    s16 j;

    pokemon = trainer_card->pokemon;

    for (i = 0; i < 3; i++) {
        lowest_levels[i] = 999;
    }

    for (i = 0; i < 6; i++, pokemon++) {
        pokemon->unk_25 = 1;
        if ((pokemon->species.id > 0) && (pokemon->species.id < 152)) {
            if (EventBattle_HasValidMoveIds(pokemon) != 0) {
                // FAKE: the all-ones mask preserves the unsigned index expression used for matching.
                cup_rules = &sCupRules[(rule_category - 1) & 0xFFFFFFFF];

                if (D_8006FF00[pokemon->species.id - 1].cupEligibilityFlags & cup_rules->speciesEligibilityMask) {
                    if ((pokemon->level >= cup_rules->minLevel) && (cup_rules->maxLevel >= pokemon->level)) {
                        pokemon->unk_25 = 0;
                    }
                }

                for (j = 0; j < i; j++) {
                    if (pokemon->species.id == trainer_card->pokemon[j].species.id) {
                        pokemon->unk_25 = 1;
                        trainer_card->pokemon[j].unk_25 = 1;
                    }
                }
            }
        }

        if (lowest_levels[2] >= pokemon->level) {
            lowest_levels[2] = pokemon->level;
        }

        if (lowest_levels[1] >= pokemon->level) {
            lowest_levels[2] = lowest_levels[1];
            lowest_levels[1] = pokemon->level;
        }

        if (lowest_levels[0] >= pokemon->level) {
            lowest_levels[1] = lowest_levels[0];
            lowest_levels[0] = pokemon->level;
        }
    }

    for (i = 0, j = 0; i < 6; i++) {
        if (trainer_card->pokemon[i].unk_25 == 0) {
            j++;
        }
    }

    trainer_card->allPokemonEligible = j == 6;
    if (trainer_card->allPokemonEligible) {
        trainer_card->levelCapCompliancePossible = sCupRules[rule_category - 1].levelCap >= (lowest_levels[0] + lowest_levels[1] + lowest_levels[2]);
    } else {
        trainer_card->levelCapCompliancePossible = 1;
    }
    return trainer_card->allPokemonEligible;
}

/* Begin the trainer card's entrance, reset its warning phase, and evaluate its selected rules. */
u8 EventBattle_ShowTrainerCardForRules(EventBattleTrainerParty* trainer_card, s16 rule_category) {
    trainer_card->ruleCategory = rule_category;
    trainer_card->levelCapWarningPhase = 0;
    trainer_card->slideState = 2;
    return EventBattle_CheckPartyEligibility(trainer_card, rule_category);
}

/* Draw the vertically scaled banner and show its warning text when fully open and ineligible. */
void EventBattle_DrawPartyEligibilityBanner(EventBattleDescriptionPanel* warning) {
    s16 x;
    s16 slice_height;
    s16 i;
    s16 y;

    if (warning->height > 0) {
        slice_height = warning->height / 4;
        x = warning->x;
        y = warning->y + ((56 - warning->height) / 2);

        gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

        for (i = 0; i < 4; i++) {
            gDPLoadTextureBlock(gDisplayListHead++, &D_302C000[i * 0xAF0], G_IM_FMT_RGBA, G_IM_SIZ_16b, 100, 14, 0,
                                G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);

            Gfx_DrawTexturedRectClipped(x, y + (i * slice_height), 100, slice_height, 0, 0, 0x400, 0x3800 / slice_height, 0);
        }

        if ((warning->bothPartiesEligible == 0) && (warning->height == 56)) {
            EventBattle_DrawWarningPanelBackground(112, y + 8, 416, 40, 0x00, 0x00, 0x00, 0x96); // #000000

            gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

            Font_BeginTranslucentTextRendering();
            Font_SetActive(0x10, 0);
            Gfx_SetEnvColor(0xFF, 0x50, 0x64, 0xFF); // #FF5064
            Font_Printf(144, y + 14, Text_GetString(NULL, 0, sEventBattleText, 10));
            Font_EndTexturedTextRendering();
        } else {
            gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
        }
    }
}

/* Advance the eligibility banner's height and play an error effect when an ineligible banner opens. */
void EventBattle_UpdatePartyEligibilityBanner(EventBattleDescriptionPanel* panel) {
    switch (panel->animationState) {
        case 1:
            panel->height += 7;
            if (panel->height == 56) {
                if (panel->bothPartiesEligible == 0) {
                    Audio_PlaySoundEffectById(SFX_MENU_ERROR);
                }
                panel->animationState = 0;
            }
            break;

        case 2:
            panel->height -= 7;
            if (panel->height == 0) {
                panel->animationState = 0;
            }
            break;
    }
}

/* Initialize a closed eligibility banner at the supplied position. */
void EventBattle_InitPartyEligibilityBanner(EventBattleDescriptionPanel* panel, s16 x, s16 y) {
    panel->animationState = 0;
    panel->height = 0;
    panel->x = x;
    panel->y = y;
}

/* Store the combined party-eligibility result and begin opening the banner. */
void EventBattle_OpenPartyEligibilityBanner(EventBattleDescriptionPanel* panel, s32 both_parties_eligible) {
    panel->bothPartiesEligible = both_parties_eligible;
    panel->animationState = 1;
}

/* Draw an action label at its animated height and show its text when fully open. */
void EventBattle_DrawActionLabel(EventBattleActionLabel* option) {
    UNUSED s32 pad; // FAKE: unused stack padding retained for matching.
    s16 opening_progress;
    s16 x;
    s16 y;

    if (option->height >= 10) {
        opening_progress = option->height;
        x = option->x;
        y = option->y + ((48 - opening_progress) / 2);
        EventBattle_DrawSelectionOutline(x, y, 152, opening_progress);

        gSPDisplayList(gDisplayListHead++, D_8006F470);

        Gfx_FillRectRgb(x + 4, y + 4, 144, opening_progress - 8, 0x3C, 0x3C, 0xA0); // #3C3CA0

        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

        if (option->height == 48) {
            Font_BeginTranslucentTextRendering();
            Font_SetActive(0x10, 0);
            Font_Printf((x - ((s16)Font_MeasureTextExtent(0x10, 0, option->text) / 2)) + 76, y + 11,
                        option->text);
            Font_EndTexturedTextRendering();
        }
    }
}

/* Animate an action label between six and 48 pixels in height. */
void EventBattle_UpdateActionLabel(EventBattleActionLabel* option) {
    switch (option->animationState) {
        case 1:
            option->height += 6;
            if (option->height == 48) {
                option->animationState = 0;
            }
            break;

        case 2:
            option->height -= 6;
            if (option->height == 6) {
                option->animationState = 0;
            }
            break;
    }
}

/* Initialize a collapsed action label with its position and text. */
void EventBattle_InitActionLabel(EventBattleActionLabel* option, s16 x, s16 y, s8* text) {
    option->animationState = 0;
    option->height = 6;
    option->x = x;
    option->y = y;
    option->text = text;
}

/* Draw a row's label and optional formatted value using its selection and enabled-state colors. */
void EventBattle_DrawSettingsRow(EventBattleSettingsPanel* settings_window, s16 option_index, s16 label_text_id, s16 value_text_id, s16 value_number) {
    char text[64];
    UNUSED s32 pad; // FAKE: unused stack padding retained for matching.

    if (((option_index == 1) || (option_index == 2)) && (settings_window->timeLimitsEnabled == 0)) {
        Gfx_SetEnvColor(0x50, 0x50, 0xB4, 0xFF); // #5050B4
    } else if (option_index == settings_window->cursorPosition) {
        Gfx_SetEnvColor(0xFF, 0xFF, 0x00, 0xFF); // #FFFF00
    } else {
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #FFFFFF
    }

    Font_Printf(settings_window->x + 52, settings_window->y + option_index * 33 + 47, Text_GetString(NULL, 0, sEventBattleText, label_text_id));

    if (value_text_id >= 0) {
        Text_SetNumberToken(2, value_number);
        Text_GetString(text, sizeof(text), sEventBattleText, value_text_id);
        Font_Printf((settings_window->x - (Font_MeasureTextExtent(0, 0, text) / 2)) + 344,
                    settings_window->y + option_index * 33 + 47, text, value_number);
    }
}

/* Draw a 120-by-28 value-field strip using the enabled or disabled tint. */
void EventBattle_DrawSettingsValueBackground(s16 x, s16 y, s16 enabled) {
    if (enabled != 0) {
        Gfx_SetEnvColor(0x82, 0x1E, 0x1E, 0xFF); // #821E1E
    } else {
        Gfx_SetEnvColor(0x32, 0x32, 0x96, 0xFF); // #323296
    }

    Gfx_DrawTextureI4(x, y, 16, 28, D_30242C0, 16, 0);
    Gfx_DrawTextureI4(x + 104, y, 16, 28, D_30243A0, 16, 0);
    Gfx_DrawTexturedRectClipped(x + 16, y, 88, 28, 0, 0, 0, 0x400, 0);
}

/* Draw pulsing decrement and increment arrows allowed by the selected time setting's bounds. */
void EventBattle_DrawTimeSettingArrows(s16 x, s16 y, s16 cursor_position, s16 option_setting) {
    static s16 sOptionArrowPhase = 0;

    s16 arrow_offset = SINS(sOptionArrowPhase) * 2;

    if (cursor_position == 1) {
        // Draw left arrow for Battle Time
        if (option_setting >= 6) {
            Gfx_DrawTextureRgba16(x - arrow_offset, y + (cursor_position * 33), 20, 32, D_302FBC0, 20, 0);
        }
        // Draw right arrow for Battle Time
        if (option_setting < 90) {
            Gfx_DrawTextureRgba16(x + arrow_offset + 148, y + (cursor_position * 33), 20, 32, D_30300C0, 20, 0);
        }
    } else {
        // Draw left arrow for Move Selection Time
        if (option_setting >= 11) {
            Gfx_DrawTextureRgba16(x - arrow_offset, y + (cursor_position * 33), 20, 32, D_302FBC0, 20, 0);
        }
        // Draw right arrow for Move Selection Time
        if (option_setting < 90) {
            Gfx_DrawTextureRgba16(x + arrow_offset + 148, y + (cursor_position * 33), 20, 32, D_30300C0, 20, 0);
        }
    }
    sOptionArrowPhase += 0x2000;
}

/* Draw the animated settings window and reveal its controls when fully open. */
void EventBattle_DrawSettingsPanel(EventBattleSettingsPanel* settings_window) {
    char* title_string;
    s16 time_limits_index;
    s16 y;
    s16 x;
    UNUSED s16 pad; // FAKE: unused stack padding retained for matching.
    s16 opening_progress;

    if (settings_window->height >= 15) {
        opening_progress = settings_window->height;
        x = settings_window->x;
        y = settings_window->y + ((186 - opening_progress) / 2);

        Ui_DrawBorderedPanelNoFill(x, y, 446, opening_progress);

        if (opening_progress == 186) {
            EventBattle_DrawFilledPanel(x + 7, y + 7, 432, 32, 0x82, 0x1E, 0x1E, 0xFF); // #821E1E
            EventBattle_DrawFilledPanel(x + 7, y + 39, 432, 140, 0x1E, 0x1E, 0x64, 0xFF); // #1E1E64
            Ui_DrawAnimatedTextureMarker(x + 14, y + (settings_window->cursorPosition * 33) + 49);

            gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

            if (settings_window->timeLimitsEnabled == 1) {
                if (settings_window->cursorPosition == 1) {
                    EventBattle_DrawTimeSettingArrows(x + 260, y + 45, settings_window->cursorPosition, settings_window->battleLimitMinutes);
                }

                if (settings_window->cursorPosition == 2) {
                    EventBattle_DrawTimeSettingArrows(x + 260, y + 45, settings_window->cursorPosition, settings_window->orderLimitSeconds);
                }
            }

            EventBattle_DrawSettingsValueBackground(x + 284, y + 46, 1);
            EventBattle_DrawSettingsValueBackground(x + 284, y + 79, settings_window->timeLimitsEnabled);
            EventBattle_DrawSettingsValueBackground(x + 284, y + 112, settings_window->timeLimitsEnabled);

            gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

            Font_BeginTranslucentTextRendering();
            Font_SetActive(0x10, 0);
            title_string = Text_GetString(NULL, 0, sEventBattleText, 11);
            Font_Printf((x - (Font_MeasureTextExtent(0x10, 0, title_string) / 2)) + 223, y + 10, title_string);

            if (settings_window->timeLimitsEnabled == 1) {
                time_limits_index = 12;
            } else {
                time_limits_index = 13;
            }

            EventBattle_DrawSettingsRow(settings_window, 0, 14, time_limits_index, 0);
            EventBattle_DrawSettingsRow(settings_window, 1, 16, 15, settings_window->battleLimitMinutes);
            EventBattle_DrawSettingsRow(settings_window, 2, 17, 18, settings_window->orderLimitSeconds);
            EventBattle_DrawSettingsRow(settings_window, 3, 19, -1, 0);
            Font_EndTexturedTextRendering();
        } else {
            EventBattle_DrawFilledPanel(x + 7, y + 7, 432, opening_progress - 14, 0x1E, 0x1E, 0x64, 0xFF); // #1E1E64
        }
    }
}

/* Animate the settings window and process time edits, confirmation, or cancellation while open. */
void EventBattle_UpdateSettingsPanel(EventBattleSettingsPanel* settings_window) {
    s32 repeated_dpad;

    switch (settings_window->state) {
        case 1:
            repeated_dpad = Input_GetRepeatedDPad();
            if ((repeated_dpad == BTN_DLEFT) && (settings_window->timeLimitsEnabled == 1)) {
                if (settings_window->cursorPosition == 1) {
                    if (settings_window->battleLimitMinutes >= 6) {
                        settings_window->battleLimitMinutes -= 5;
                        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                    }
                }

                if (settings_window->cursorPosition == 2) {
                    if (settings_window->orderLimitSeconds >= 11) {
                        settings_window->orderLimitSeconds -= 10;
                        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                    }
                }
            } else if ((repeated_dpad == BTN_DRIGHT) && (settings_window->timeLimitsEnabled == 1)) {
                if (settings_window->cursorPosition == 1) {
                    if (settings_window->battleLimitMinutes < 90) {
                        settings_window->battleLimitMinutes += 5;
                        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                    }
                }

                if (settings_window->cursorPosition == 2) {
                    if (settings_window->orderLimitSeconds < 90) {
                        settings_window->orderLimitSeconds += 10;
                        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                    }
                }
            } else {
                if (gPlayer1Controller->buttonPressed & BTN_DUP) {
                    if (settings_window->cursorPosition > 0) {
                        settings_window->cursorPosition--;
                        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                    }
                } else if (gPlayer1Controller->buttonPressed & BTN_DDOWN) {
                    if (settings_window->cursorPosition < 3) {
                        settings_window->cursorPosition++;
                        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                    }
                } else if (gPlayer1Controller->buttonPressed & BTN_A) {
                    if (settings_window->cursorPosition == 0) {
                        settings_window->timeLimitsEnabled ^= 0x01;
                        Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
                    }

                    if (settings_window->cursorPosition == 3) {
                        gEventBattleSettings[0] = settings_window->timeLimitsEnabled;
                        gEventBattleSettings[1] = settings_window->battleLimitMinutes;
                        gEventBattleSettings[2] = settings_window->orderLimitSeconds;
                        settings_window->state = 3;
                        Audio_PlaySoundEffectById(SFX_BIG_CONFIRM);
                    }
                } else {
                    if (gPlayer1Controller->buttonPressed & BTN_B) {
                        settings_window->timeLimitsEnabled = gEventBattleSettings[0];
                        settings_window->battleLimitMinutes = gEventBattleSettings[1];
                        settings_window->orderLimitSeconds = gEventBattleSettings[2];
                        settings_window->state = 3;
                        Audio_PlaySoundEffectById(SFX_MENU_BACK);
                    }
                }
            }
            break;

        case 2:
            settings_window->height += 22;
            if (settings_window->height == 186) {
                Input_SetRepeatController(gPlayer1Controller);
                settings_window->state = 1;
            }
            break;

        case 3:
            settings_window->height -= 22;
            if (settings_window->height == 10) {
                settings_window->state = 0;
            }
            break;
    }
}

/* Initialize a closed settings window and optionally load the saved time-limit choices. */
void EventBattle_InitSettingsPanel(EventBattleSettingsPanel* settings_window, s16 x, s16 y, s32 load_saved_settings) {
    settings_window->state = 0;
    settings_window->height = 10;
    settings_window->x = x;
    settings_window->y = y;
    settings_window->cursorPosition = 0;

    if (load_saved_settings != 0) {
        settings_window->timeLimitsEnabled = gEventBattleSettings[0];
        settings_window->battleLimitMinutes = gEventBattleSettings[1];
        settings_window->orderLimitSeconds = gEventBattleSettings[2];
    }
}

/* Begin opening the settings window with the cursor on its time-limit toggle. */
void EventBattle_OpenSettingsPanel(EventBattleSettingsPanel* settings_panel) {
    Audio_PlaySoundEffectById(SFX_PANEL_OPEN);
    settings_panel->state = 2;
    settings_panel->cursorPosition = 0;
}

/* Tile the intensity texture across the screen using the event-battle background colors. */
void EventBattle_DrawBackground(void) {
    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetCombineLERP(gDisplayListHead++, ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE, TEXEL0, 0, ENVIRONMENT, 0,
                      ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE, TEXEL0, 0, ENVIRONMENT, 0);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetEnvColor(gDisplayListHead++, 0x8C, 0xBE, 0xC8, 0xFF); // #8CBEC8
    gDPSetPrimColor(gDisplayListHead++, 0, 0, 0x6E, 0xA0, 0xAA, 0xFF); // #6EA0AA
    gSPClearGeometryMode(gDisplayListHead++, G_ZBUFFER | G_LIGHTING);

    gDPLoadTextureBlock_4b(gDisplayListHead++, gEventBattleBackground, G_IM_FMT_I, 128, 64, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 7, 6, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(0, 0, 640, 480, 0, 0, 0x400, 0x400, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

/* Render the widgets and optional selection corners between the stage's frame calls. */
void EventBattle_DrawFrame(s16 selection_index) {
    s16 i;

    BgStage_DrawFrame();
    EventBattle_DrawBackground();
    EventBattle_DrawTitleBar();
    EventBattle_DrawDescriptionPanel(&sDescriptionPanel);
    EventBattle_DrawPartyEligibilityBanner(&sPartyEligibilityBanner);
    EventBattle_DrawSettingsPanel(&sSettingsPanel);
    EventBattle_DrawTrainerCard(&sTrainerCards[0]);
    EventBattle_DrawTrainerCard(&sTrainerCards[1]);

    for (i = 0; i < 6; i++) {
        EventBattle_DrawSelectionOption(&sSelectionOptions[i]);
    }

    for (i = 0; i < 3; i++) {
        EventBattle_DrawActionLabel(&sActionLabels[i]);
    }

    if (selection_index >= 0) {
        EventBattle_DrawSelectionCorners(sSelectionEntries[selection_index].x, sSelectionEntries[selection_index].y,
                                         sSelectionEntries[selection_index].width, sSelectionEntries[selection_index].height);
    }

    BgStage_AdvanceFrame();
}

/* Poll controllers and advance the widgets' input handling and animations. */
void EventBattle_UpdateWidgets(void) {
    s16 i;

    Controller_PollInputs();
    EventBattle_UpdateDescriptionPanel(&sDescriptionPanel);
    EventBattle_UpdatePartyEligibilityBanner(&sPartyEligibilityBanner);
    EventBattle_UpdateSettingsPanel(&sSettingsPanel);
    EventBattle_UpdateTrainerCardSlide(&sTrainerCards[0]);
    EventBattle_UpdateTrainerCardSlide(&sTrainerCards[1]);

    for (i = 0; i < 6; i++) {
        EventBattle_UpdateSelectionOptionSlide(&sSelectionOptions[i]);
    }

    for (i = 0; i < 3; i++) {
        EventBattle_UpdateActionLabel(&sActionLabels[i]);
    }
}

/* Run two-column mode navigation until a command is chosen, defaulting the next game state to the main menu. */
void EventBattle_RunModeSelection(EventBattleMenuState* state) {
    if (state->state == 3) {
        do {
            EventBattle_UpdateWidgets();
            if (gPlayer1Controller->buttonPressed & BTN_DUP) {
                if (state->cursorPosition >= 2) {
                    state->cursorPosition -= 2;
                    Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                }
            } else if (gPlayer1Controller->buttonPressed & BTN_DDOWN) {
                if (state->cursorPosition < 4) {
                    state->cursorPosition += 2;
                    Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                }
            } else if (gPlayer1Controller->buttonPressed & BTN_DLEFT) {
                if ((state->cursorPosition & 0x01) == 1) {
                    state->cursorPosition--;
                    Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                }
            } else if (gPlayer1Controller->buttonPressed & BTN_DRIGHT) {
                if (!(state->cursorPosition & 0x01)) {
                    state->cursorPosition++;
                    Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                }
            } else if (gPlayer1Controller->buttonPressed & BTN_A) {
                switch (state->cursorPosition) {
                    case 4:
                        state->state = 5;
                        Audio_PlaySoundEffectById(28);
                        break;

                    case 5:
                        state->state = 2;
                        Audio_PlaySoundEffectById(SFX_MENU_BACK);
                        break;

                    default:
                        state->state = 7;
                        Audio_PlaySoundEffectById(28);
                        break;
                }
            } else if (gPlayer1Controller->buttonPressed & BTN_B) {
                Audio_PlaySoundEffectById(SFX_MENU_BACK);
                state->state = 2;
            }
            EventBattle_DrawFrame(state->cursorPosition);
        } while (state->state == 3);
    }
    state->nextGameState = 4;
}

/* Fade in the screen, slide in its selection cards, and enter mode navigation. */
void EventBattle_EnterModeSelection(EventBattleMenuState* state) {
    s16 i;
    s16 j;

    Audio_PlayMusicIfChanged(45);
    StageFade_StartFromOpaque(8);

    for (i = 0; i < 6; i++) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    sDescriptionPanel.animationState = 1;
    Audio_PlayCategory11SoundCommand(0x01100001, 0, 0);

    for (j = 0; j < 6; j++) {
        sSelectionOptions[j].slideState = 2;
    }

    for (i = 0; i < 12; i++) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    state->state = 3;
}

/* Close the mode-selection widgets, fade out, and finish the menu state machine. */
void EventBattle_ExitToMainMenu(EventBattleMenuState* state) {
    s16 i;
    s16 j;

    sDescriptionPanel.animationState = 2;

    for (i = 0; i < 6; i++) {
        sSelectionOptions[i].slideState = 3;
    }

    for (j = 0; j < 6; j++) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    Audio_StopMusic(16);
    StageContext_SetClearColor(0xFFFF); // #FFFFFF
    StageFade_StartFromTransparent(8);

    while (StageContext_GetFadeMode() != 1) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    state->state = 0;
}

/* Hide mode cards, evaluate and reveal both parties, then open the available party actions. */
void EventBattle_EnterPartyReview(EventBattleMenuState* state) {
    s16 i;
    s16 j;
    u8 first_action_index;

    sDescriptionPanel.animationState = 2;

    for (i = 0; i < 6; i++) {
        sSelectionOptions[i].slideState = 3;
    }

    for (j = 0; j < 12; j++) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    Audio_PlayCategory11SoundCommand(0x01100001, 0, 0);
    state->mayProceedToBattle = EventBattle_ShowTrainerCardForRules(&sTrainerCards[0], sSelectionEntries[state->cursorPosition].ruleCategory);
    state->mayProceedToBattle &= EventBattle_ShowTrainerCardForRules(&sTrainerCards[1], sSelectionEntries[state->cursorPosition].ruleCategory);
    EventBattle_OpenPartyEligibilityBanner(&sPartyEligibilityBanner, state->mayProceedToBattle);

    first_action_index = !state->mayProceedToBattle;
    state->actionCursor = first_action_index;

    for (i = first_action_index; i < 3; i++) {
        sActionLabels[i].animationState = 1;
    }

    for (j = 0; j < 12; j++) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    state->state = 4;
}

/* Slide out both parties, close their actions and banner, and restore mode-selection cards. */
void EventBattle_ReturnToModeSelection(EventBattleMenuState* state) {
    u8 first_action_index;
    s16 i;
    s16 j;

    first_action_index = !state->mayProceedToBattle;
    sTrainerCards[0].slideState = 3;
    sTrainerCards[1].slideState = 3;
    sPartyEligibilityBanner.animationState = 2;

    for (i = first_action_index; i < 3; i++) {
        sActionLabels[i].animationState = 2;
    }

    for (j = 0; j < 12; j++) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    sDescriptionPanel.animationState = 1;
    Audio_PlayCategory11SoundCommand(0x01100001, 0, 0);

    for (i = 0; i < 6; i++) {
        sSelectionOptions[i].slideState = 2;
    }

    for (j = 0; j < 12; j++) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    state->state = 3;
}

/* Evaluate and reveal the remembered selection's parties and available actions without the mode-card transition. */
void EventBattle_ResumePartyReview(EventBattleMenuState* state) {
    u8 first_action_index;
    s16 i;
    s16 j;

    Audio_PlayCategory11SoundCommand(0x01100001, 0, 0);
    state->mayProceedToBattle = EventBattle_ShowTrainerCardForRules(&sTrainerCards[0], sSelectionEntries[state->cursorPosition].ruleCategory);
    state->mayProceedToBattle &= EventBattle_ShowTrainerCardForRules(&sTrainerCards[1], sSelectionEntries[state->cursorPosition].ruleCategory);
    EventBattle_OpenPartyEligibilityBanner(&sPartyEligibilityBanner, state->mayProceedToBattle);

    first_action_index = !state->mayProceedToBattle;
    state->actionCursor = first_action_index;

    for (i = first_action_index; i < 3; i++) {
        sActionLabels[i].animationState = 1;
    }

    for (j = 0; j < 12; j++) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    state->state = 4;
}

/* Set Pikachu's partner flag when its trainer identity matches the identified Yellow cartridge player. */
void EventBattle_MarkPartnerPikachu(BattleMon* pokemon, s16 controller_port) {
    GbSavePlayerIdentity pokemon_trainer_identity;
    GbSavePlayerIdentity cartridge_player_identity;

    if ((GbSave_GetPortGame(controller_port) == 7) && (pokemon->species.id == PIKACHU)) {
        GbSave_CopyPlayerIdentity(controller_port, &cartridge_player_identity);
        _bcopy(pokemon->unk_46, pokemon_trainer_identity.unk_02, 11);
        pokemon_trainer_identity.unk_00 = pokemon->unk_0E;
        if (GbSave_PlayerIdentityMatches(&pokemon_trainer_identity, &cartridge_player_identity)) {
            pokemon->unk_52 |= 0x80;
        }
    }
}

/* Create a session trainer, mark partner Pikachu, and add all six party Pokemon to its battle side. */
void EventBattle_AddPartyToSession(EventBattleTrainerParty* trainer_card) {
    s16 i;
    TeamRoster* trainer = Trainer_Create(trainer_card->controllerPort, trainer_card->trainerId, trainer_card->trainerName, Text_GetPlayerLabel(1));

    for (i = 0; i < 6; i++) {
        EventBattle_MarkPartnerPikachu(&trainer_card->pokemon[i], trainer_card->controllerPort);
        Trainer_AddPokemon(trainer, &trainer_card->pokemon[i]);
    }

    Team_AddTrainer(trainer_card->controllerPort, trainer);
}

/* Prepare both session parties and time limits, fade out, and return the event-battle launch state. */
void EventBattle_ExitToBattle(EventBattleMenuState* state) {
    s16 i;
    s16 j;

    Session_SetMode(SESSION_MODE_EVENT_BATTLE, sSelectionEntries[state->cursorPosition].ruleCategory, 0);
    EventBattle_AddPartyToSession(&sTrainerCards[0]);
    EventBattle_AddPartyToSession(&sTrainerCards[1]);

    if (sSettingsPanel.timeLimitsEnabled == 1) {
        gSessionContext.timeLimitsEnabled = sSettingsPanel.timeLimitsEnabled;
        gSessionContext.battleLimitMinutes = sSettingsPanel.battleLimitMinutes;
        gSessionContext.orderLimitSeconds = sSettingsPanel.orderLimitSeconds;
    }
    sTrainerCards[0].slideState = 3;
    sTrainerCards[1].slideState = 3;
    sPartyEligibilityBanner.animationState = 2;

    for (i = 0; i < 3; i++) {
        sActionLabels[i].animationState = 2;
    }

    for (j = 0; j < 6; j++) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    Audio_StopMusic(16);
    StageContext_SetClearColor(0xFFFF); // #FFFFFF
    StageFade_StartFromTransparent(8);

    while (StageContext_GetFadeMode() != 1) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    state->nextGameState = 42;
    state->state = 0;
}

/* Navigate the available party-review actions until start, rental rules, or back is selected. */
void EventBattle_RunPartyActionMenu(EventBattleMenuState* state) {
    u8 first_action_index;

    first_action_index = !state->mayProceedToBattle;

    if (state->state == 4) {
        do {
            EventBattle_UpdateWidgets();
            if (gPlayer1Controller->buttonPressed & BTN_DLEFT) {
                if (first_action_index < state->actionCursor) {
                    state->actionCursor--;
                    Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                }
            } else if (gPlayer1Controller->buttonPressed & BTN_DRIGHT) {
                if (state->actionCursor < 2) {
                    state->actionCursor++;
                    Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                }
            } else if (gPlayer1Controller->buttonPressed & BTN_A) {
                switch (state->actionCursor) {
                    case 0:
                        state->state = 6;
                        Audio_PlaySoundEffectById(32);
                        break;

                    case 1:
                        state->state = 9;
                        Audio_PlaySoundEffectById(28);
                        break;

                    case 2:
                        state->state = 8;
                        Audio_PlaySoundEffectById(SFX_MENU_BACK);
                        break;
                }
            } else if (gPlayer1Controller->buttonPressed & BTN_B) {
                Audio_PlaySoundEffectById(SFX_MENU_BACK);
                state->actionCursor = 2;
                state->state = 8;
            }
            EventBattle_DrawFrame(state->actionCursor + 6);
        } while (state->state == 4);
    }
}

/* Hide mode-selection cards, run the settings window until it closes, and restore mode navigation. */
void EventBattle_RunSettingsPanel(EventBattleMenuState* state) {
    s16 i;
    s16 j;

    sDescriptionPanel.animationState = 2;

    for (i = 0; i < 6; i++) {
        sSelectionOptions[i].slideState = 3;
    }

    for (j = 0; j < 12; j++) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    EventBattle_OpenSettingsPanel(&sSettingsPanel);

    while (sSettingsPanel.state != 0) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    sDescriptionPanel.animationState = 1;
    Audio_PlayCategory11SoundCommand(0x01100001, 0, 0);

    for (i = 0; i < 6; i++) {
        sSelectionOptions[i].slideState = 2;
    }

    for (j = 0; j < 12; j++) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    state->state = 3;
}

/* Set the selected session rules, close party review, and return the state that dispatches rental rules. */
void EventBattle_ExitToRentalRules(EventBattleMenuState* state) {
    s16 i;
    s16 j;

    Session_SetMode(10, sSelectionEntries[state->cursorPosition].ruleCategory, 0);

    sTrainerCards[0].slideState = 3;
    sTrainerCards[1].slideState = 3;
    sPartyEligibilityBanner.animationState = 2;

    for (i = 0; i < 3; i++) {
        sActionLabels[i].animationState = 2;
    }

    for (j = 0; j < 12; j++) {
        EventBattle_UpdateWidgets();
        EventBattle_DrawFrame(-1);
    }

    state->nextGameState = 18;
    state->state = 0;
}

/* Dispatch selection-screen transitions and interactive loops until an exit state ends the menu. */
void EventBattle_RunMenuStateMachine(EventBattleMenuState* state) {
    while (state->state != 0) {
        switch (state->state) {
            case 1:
                EventBattle_EnterModeSelection(state);
                break;

            case 2:
                EventBattle_ExitToMainMenu(state);
                break;

            case 3:
                EventBattle_RunModeSelection(state);
                break;

            case 4:
                EventBattle_RunPartyActionMenu(state);
                break;

            case 7:
                EventBattle_EnterPartyReview(state);
                break;

            case 8:
                EventBattle_ReturnToModeSelection(state);
                break;

            case 6:
                EventBattle_ExitToBattle(state);
                break;

            case 5:
                EventBattle_RunSettingsPanel(state);
                break;

            case 9:
                EventBattle_ExitToRentalRules(state);
                break;

            case 10:
                EventBattle_ResumePartyReview(state);
                break;
        }
    }
}

/* Initialize screen widgets and choose fresh mode selection or a remembered party-review entry. */
void EventBattle_InitSelectionScreen(EventBattleMenuState* state, s16 selection_index) {
    s16 i;

    if (selection_index == -1) {
        state->state = 1;
        state->cursorPosition = 2;
    } else {
        state->state = 10;
        state->cursorPosition = selection_index;
    }
    state->nextGameState = 18;

    EventBattle_InitDescriptionPanel(&sDescriptionPanel, 56, 380);
    EventBattle_InitPartyEligibilityBanner(&sPartyEligibilityBanner, 270, 192);
    EventBattle_InitSettingsPanel(&sSettingsPanel, 104, 147, (selection_index + 1) == 0);

    EventBattle_InitTrainerCard(&sTrainerCards[0], 64, 92, 0);
    EventBattle_InitTrainerCard(&sTrainerCards[1], 64, 248, 1);

    EventBattle_InitSelectionOption(&sSelectionOptions[0], 68, 132, D_3028E00, Text_GetString(NULL, 0, sEventBattleText, 21), 3, 0);
    EventBattle_InitSelectionOption(&sSelectionOptions[1], 324, 132, D_3029A80, Text_GetString(NULL, 0, sEventBattleText, 24), 4, 1);
    EventBattle_InitSelectionOption(&sSelectionOptions[2], 68, 200, D_302A700, Text_GetString(NULL, 0, sEventBattleText, 26), 6, 5);
    EventBattle_InitSelectionOption(&sSelectionOptions[3], 324, 200, D_302B380, Text_GetString(NULL, 0, sEventBattleText, 25), 5, 3);
    EventBattle_InitSelectionOption(&sSelectionOptions[4], 68, 268, NULL, Text_GetString(NULL, 0, sEventBattleText, 27), 0, 6);
    EventBattle_InitSelectionOption(&sSelectionOptions[5], 324, 268, NULL, Text_GetString(NULL, 0, sEventBattleText, 28), 0, 7);

    EventBattle_InitActionLabel(&sActionLabels[0], 80, 385, Text_GetString(NULL, 0, sEventBattleText, 29));
    EventBattle_InitActionLabel(&sActionLabels[1], 244, 385, Text_GetString(NULL, 0, sEventBattleText, 30));
    EventBattle_InitActionLabel(&sActionLabels[2], 408, 385, Text_GetString(NULL, 0, sEventBattleText, 31));

    if (selection_index == -1) {
        for (i = 2; i >= 0; i--) {
            if (sSelectionOptions[i].bothPartiesEligible != 0) {
                state->cursorPosition = i;
            }
        }
    }
}

/* Load screen assets and cartridge data, run the menu, remember its selection, and return its next game state. */
s16 EventBattle_RunSelectionScreen(void) {
    static s16 sPreviousSelectionIndex = -1;

    main_pool_push_state('EVSL');

    Font_Init(0x1C, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_2000000, common_menu2_ui, 0);
    ASSET_LOAD(D_3000000, menu_select_ui, 0);

    PokeIcon_OpenModelArchive();
    GbSave_EnsureMainDataLoaded(0);
    GbSave_EnsureMainDataLoaded(1);
    StageLoader_UpdateSegments();
    EventBattle_InitSelectionScreen(&sEventBattleMenuState, sPreviousSelectionIndex);
    EventBattle_RunMenuStateMachine(&sEventBattleMenuState);
    sPreviousSelectionIndex = sEventBattleMenuState.cursorPosition;
    StageLoader_WaitForRetrace();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('EVSL');

    return sEventBattleMenuState.nextGameState;
}

/* Own the event-battle stage and text resources, alternating selection with rental rules until another state is returned. */
s32 EventBattle_Main(UNUSED s32 arg0, UNUSED s32 arg1) {
    s16 next_game_state = STATE_EVENT_BATTLE;

    main_pool_push_state('EBTL');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    StageContext_Activate(StageContext_Allocate(1, 0, 2, 0, 2, 1));
    Text_InitStringTables();
    sEventBattleText = Text_GetStringTable(16);
    sPokemonDetailText = Text_GetStringTable(6);

    while (next_game_state == STATE_EVENT_BATTLE) {
        next_game_state = EventBattle_RunSelectionScreen();
        if (next_game_state == STATE_EVENT_BATTLE) {
            FRAGMENT_LOAD_AND_CALL(FRAGMENT_RENTAL_RULES, 0, 0);
        }
    }

    StageContext_Deactivate();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('EBTL');

    return next_game_state;
}
