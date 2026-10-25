#include "battle_engine.h"
#include "src/graphics_textures.h"
#include "src/ui_graphics.h"
#include "src/status_icons.h"
#include "src/game_state.h"
#include "src/text_system.h"
#include "src/3D140.h"
#include "src/gfx_buffer.h"
#include "src/gfx_rect.h"
#include "src/matrix.h"
#include "src/controller.h"
#include "src/memmap.h"
#include "src/memory.h"

Vec3f sEndBattleBannerPos;

static StatusConditionLabel sStatusLabelConfigs[] = {
    { 0, { 0xFF, 0xFF, 0xFF, 0xFF } }, // #FFFFFF OK
    { 1, { 0x4D, 0x4D, 0x4D, 0xFF } }, // #4D4D4D fainted
    { 2, { 0x9F, 0x91, 0xE8, 0xFF } }, // #9F91E8 sleep
    { 3, { 0x96, 0x50, 0xFF, 0xFF } }, // #9650FF poison
    { 4, { 0xF0, 0x5A, 0x46, 0xFF } }, // #F05A46 burn
    { 5, { 0x85, 0xD5, 0xF1, 0xFF } }, // #85D5F1 ice
    { 6, { 0xFF, 0xFF, 0x00, 0xFF } }, // #FFFF00 paralyze
};
static u8* sTypeLabels[] = {
    gTypeLabelNormal, gTypeLabelFighting, gTypeLabelFlying, gTypeLabelPoison, gTypeLabelGround, gTypeLabelRock, gTypeLabelNormal, gTypeLabelBug, gTypeLabelGhost, gTypeLabelNormal,
    gTypeLabelNormal, gTypeLabelNormal, gTypeLabelNormal, gTypeLabelNormal, gTypeLabelNormal, gTypeLabelNormal, gTypeLabelNormal, gTypeLabelNormal, gTypeLabelNormal, gTypeLabelNormal,
    gTypeLabelFire, gTypeLabelWater, gTypeLabelGrass, gTypeLabelElectric, gTypeLabelPsychic, gTypeLabelIce, gTypeLabelDragon, NULL,
};
static u8* sStatusIcons[] = {
    gStatusIconFainted, gStatusIconSleep, gStatusIconPoison, gStatusIconBurn, gStatusIconIce, gStatusIconParalyze,
};
static u8* sPlayerLabels[] = {
    gPlayerLabel1P,
    gPlayerLabel2P,
    gPlayerLabel3P,
    gPlayerLabel4P,
};

static Color_RGB8 sPlayerNameTagStartColor[] = {
    { 0, 0, 0x8B },       // #00008B dark blue
    { 0, 0x32, 0 },       // #003200 dark green
    { 0x1E, 0, 0 },       // #1E0000 dark red
    { 0x65, 0x49, 0x11 }, // #654911 dark gold
    { 0, 0, 0 },          // #000000 black
};
static Color_RGB8 sPlayerNameTagEndColor[] = {
    { 0x00, 0x00, 0xEE }, // #0000EE blue
    { 0x00, 0x79, 0x00 }, // #007900 green
    { 0x82, 0x00, 0x00 }, // #820000 red
    { 0x97, 0x89, 0x13 }, // #978913 gold
};
static Color_RGB8 sPlayerFlashColor[] = {
    { 0x13, 0x45, 0xFF }, // #1345FF bright blue
    { 0, 0x99, 0 },       // #009900 bright green
    { 0xCC, 0x51, 0x28 }, // #CC5128 orange-red
    { 0xCC, 0x8F, 0 },    // #CC8F00 amber
};
static Color_RGB8 sPlayer1DarkColor[] = {
    { 0x00, 0x00, 0x8B }, // #00008B dark blue
    { 0x00, 0x00, 0x8B }, // #00008B dark blue
    { 0x00, 0x00, 0x8B }, // #00008B dark blue
    { 0x00, 0x00, 0x8B }, // #00008B dark blue
};
static Color_RGB8 sPlayer1LightColor[] = {
    { 0x00, 0x00, 0xEE }, // #0000EE blue
    { 0x00, 0x00, 0xEE }, // #0000EE blue
    { 0x00, 0x00, 0xEE }, // #0000EE blue
    { 0x00, 0x00, 0xEE }, // #0000EE blue
};
static Color_RGB8 sPlayer2DarkColor[] = {
    { 0x00, 0x32, 0x00 }, // #003200 dark green
    { 0x00, 0x32, 0x00 }, // #003200 dark green
    { 0x00, 0x32, 0x00 }, // #003200 dark green
    { 0x00, 0x32, 0x00 }, // #003200 dark green
};
static Color_RGB8 sPlayer2LightColor[] = {
    { 0x00, 0x79, 0x00 }, // #007900 green
    { 0x00, 0x79, 0x00 }, // #007900 green
    { 0x00, 0x79, 0x00 }, // #007900 green
    { 0x00, 0x79, 0x00 }, // #007900 green
};
static Color_RGB8 sPlayer3DarkColor[] = {
    { 0x1E, 0x00, 0x00 }, // #1E0000 dark red
    { 0x1E, 0x00, 0x00 }, // #1E0000 dark red
    { 0x1E, 0x00, 0x00 }, // #1E0000 dark red
    { 0x1E, 0x00, 0x00 }, // #1E0000 dark red
};
static Color_RGB8 sPlayer3LightColor[] = {
    { 0x82, 0x00, 0x00 }, // #820000 red
    { 0x82, 0x00, 0x00 }, // #820000 red
    { 0x82, 0x00, 0x00 }, // #820000 red
    { 0x82, 0x00, 0x00 }, // #820000 red
};
static Color_RGB8 sPlayer4DarkColor[] = {
    { 0x65, 0x49, 0x11 }, // #654911 dark gold
    { 0x65, 0x49, 0x11 }, // #654911 dark gold
    { 0x65, 0x49, 0x11 }, // #654911 dark gold
    { 0x65, 0x49, 0x11 }, // #654911 dark gold
};
static Color_RGB8 sPlayer4LightColor[] = {
    { 0x97, 0x89, 0x13 }, // #978913 gold
    { 0x97, 0x89, 0x13 }, // #978913 gold
    { 0x97, 0x89, 0x13 }, // #978913 gold
    { 0x97, 0x89, 0x13 }, // #978913 gold
};
static Gfx sTexEdgeDecalDL[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetTexturePersp(G_TP_NONE),
    gsDPSetRenderMode(G_RM_TEX_EDGE, G_RM_TEX_EDGE2),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_ZBUFFER | G_LIGHTING),
    gsSPEndDisplayList(),
};

void func_8430FC10(void) {
}

void func_8430FC18(void) {
}

void func_8430FC20(void) {
}

void BattleHud_DrawTranslucentShadowRect(s32 x, s32 y, s32 width, s32 height, s32 alpha) {
    DepthBuffer* depth_buffer;
    Vtx* vertex;
    Mtx* matrix;
    Vp* viewport;

    depth_buffer = GfxImage_GetCurrent();
    vertex = Gfx_AllocDisplayList(sizeof(Vtx) * 4);
    matrix = Gfx_AllocDisplayList(sizeof(Mtx) * 1);
    viewport = Gfx_AllocDisplayList(sizeof(Vp) * 1);
    Gfx_SetViewportDimensions(viewport, depth_buffer->width, depth_buffer->height);

    gSPViewport(gDisplayListHead++, (u32)viewport & 0x1FFFFFFF);

    guOrtho(matrix, 0.5f, depth_buffer->width - 0.5f, depth_buffer->height - 0.5f, 0.5f, -2.0f, 2.0f, 1.0f);

    Gfx_SetVertexAttributes(&vertex[0], x, y, -1, 0, 0, 0, 0, 0, alpha);
    Gfx_SetVertexAttributes(&vertex[1], x + width, y, -1, 0, 0, 0, 0, 0, alpha);
    Gfx_SetVertexAttributes(&vertex[2], x, y + height, -1, 0, 0, 0, 0, 0, alpha);
    Gfx_SetVertexAttributes(&vertex[3], x + width, y + height, -1, 0, 0, 0, 0, 0, alpha);

    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gSPClearGeometryMode(gDisplayListHead++, G_ZBUFFER | G_LIGHTING);
    gDPSetCombineMode(gDisplayListHead++, G_CC_SHADE, G_CC_SHADE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gSPPerspNormalize(gDisplayListHead++, 0xFFFF);
    gSPMatrix(gDisplayListHead++, (u32)matrix & 0x1FFFFFFF, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPMatrix(gDisplayListHead++, (u32)&gIdentityMtx & 0x1FFFFFFF, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPVertex(gDisplayListHead++, (u32)vertex & 0x1FFFFFFF, 4, 0);
    gSP2Triangles(gDisplayListHead++, 0, 2, 3, 0, 0, 3, 1, 0);
}

void BattleHud_DrawGradientRect(s32 x, s32 y, s32 width, s32 height, Color_RGB8* start_color, Color_RGB8* end_color) {
    Vtx* vertex;
    DepthBuffer* depth_buffer;
    Mtx* matrix;
    Vp* viewport;

    depth_buffer = GfxImage_GetCurrent();
    vertex = Gfx_AllocDisplayList(sizeof(Vtx) * 4);
    matrix = Gfx_AllocDisplayList(sizeof(Mtx) * 1);
    viewport = Gfx_AllocDisplayList(sizeof(Vp) * 1);
    Gfx_SetViewportDimensions(viewport, depth_buffer->width, depth_buffer->height);

    gSPViewport(gDisplayListHead++, (u32)viewport & 0x1FFFFFFF);

    guOrtho(matrix, 0.5f, depth_buffer->width - 0.5f, depth_buffer->height - 0.5f, 0.5f, -2.0f, 2.0f, 1.0f);
    Gfx_SetVertexAttributes(&vertex[0], x, y, -1, 0, 0, start_color->r, start_color->g, start_color->b, 0xFF);
    Gfx_SetVertexAttributes(&vertex[1], x + width, y, -1, 0, 0, start_color->r, start_color->g, start_color->b, 0xFF);
    Gfx_SetVertexAttributes(&vertex[2], x, y + height, -1, 0, 0, end_color->r, end_color->g, end_color->b, 0xFF);
    Gfx_SetVertexAttributes(&vertex[3], x + width, y + height, -1, 0, 0, end_color->r, end_color->g, end_color->b, 0xFF);

    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gSPClearGeometryMode(gDisplayListHead++, G_ZBUFFER | G_LIGHTING);
    gDPSetCombineMode(gDisplayListHead++, G_CC_SHADE, G_CC_SHADE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gSPPerspNormalize(gDisplayListHead++, 0xFFFF);
    gSPMatrix(gDisplayListHead++, (u32)matrix & 0x1FFFFFFF, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPMatrix(gDisplayListHead++, (u32)&gIdentityMtx & 0x1FFFFFFF, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPVertex(gDisplayListHead++, (u32)vertex & 0x1FFFFFFF, 4, 0);
    gSP2Triangles(gDisplayListHead++, 0, 2, 3, 0, 0, 3, 1, 0);
}

void Battle_DrawGradientRectWrapper(s32 x, s32 y, s32 width, s32 height, Color_RGB8* start_color, Color_RGB8* end_color) {
    BattleHud_DrawGradientRect(x, y, width, height, start_color, end_color);
}

void func_84310360(void) {
}
/*
    Draws the HP Bar and changes it color accordingly
    Of note, tiers are the same for deciding the winner 
    see Battle_ClassifyHpTier @src/fragments/battle_engine/battle_engine_action_selection.c
*/
void BattleHud_DrawHpBar(s32 x, s32 y, f32 current_hp, s32 max_hp) {
    u8* color;
    s32 remaining_hp_percentage = ((max_hp - current_hp) * 48.0f) / max_hp;
    s32 hp_pixel_fill = 48 - remaining_hp_percentage;

    if ((current_hp * 48.0f) < (max_hp * 10)) {
        color = gHpBarRed;
    } else if ((current_hp * 48.0f) < (max_hp * 27)) {
        color = gHpBarYellow;
    } else {
        color = gHpBarGreen;
    }

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    gDPLoadTextureBlock(gDisplayListHead++, gHpBarLabel, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 5, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, x << 2, y << 2, (x + 0x10) << 2, (y + 5) << 2, G_TX_RENDERTILE,
                        0, 0, 0x0400, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, gHpBarEndCap, G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 5, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, (x + 64) << 2, y << 2, (x + 0x41) << 2, (y + 5) << 2,
                        G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

    if (hp_pixel_fill > 0) {
        gDPLoadTextureBlock(gDisplayListHead++, color, G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 5, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, (x + 0x10) << 2, y << 2, ((x + hp_pixel_fill) + 0x10) << 2,
                            (y + 5) << 2, G_TX_RENDERTILE, 0, 0, 0, 0x0400);
    }

    if (remaining_hp_percentage > 0) {
        gDPLoadTextureBlock(gDisplayListHead++, gHpBarEmpty, G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 5, 0,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, ((x + hp_pixel_fill) + 0x10) << 2, y << 2,
                            ((x + hp_pixel_fill) + remaining_hp_percentage + 0x10) << 2, (y + 5) << 2, G_TX_RENDERTILE, 0, 0, 0, 0x0400);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void BattleHud_Noop(void) {
}

void BattleHud_DrawLevelNumber(s32 x, s32 y, s32 level) {
    UNUSED s32 pad[2]; //might be a fake? string_buffer should be 4 bytes
    s8* level_string;
    s8 string_buffer[2];
    u8** level_digits;

    level_string = &string_buffer;
    level_digits = Memmap_GetSegmentVaddr(gHudGlyphTable);
    sprintf(string_buffer, "%d", level);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    if (level >= 100) {
        x -= 2;
        Gfx_DrawTextureRgba16(x, y, 8, 6, level_digits[string_buffer[0] - '0'], 8, 0);
        x += 8;
        level_string = &string_buffer[1];
    } else {
        Gfx_DrawTextureRgba16(x, y, 4, 6, level_digits[11], 4, 0);
        x += 5;
    }

    while (*level_string != 0) {
        Gfx_DrawTextureRgba16(x, y, 8, 6, level_digits[*level_string - '0'], 8, 0);
        x += 8;
        level_string++;
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void BattleHud_DrawHpFractionText(s32 x, s32 y, s32 current_hp, s32 max_hp) {
    UNUSED s32 pad[2];
    s8 string_buffer[7];
    UNUSED s32 pad2;
    u8** hp_digits;
    u8* current_char;
    s32 i;

    hp_digits = Memmap_GetSegmentVaddr(gHudGlyphTable);
    sprintf(string_buffer, "%3d/%3d", current_hp, max_hp);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    for (i = 0; i < sizeof(string_buffer); i++) {
        switch (string_buffer[i]) {
            case ' ':
                current_char = NULL;
                break;

            case '/':
                current_char = hp_digits[10];
                break;

            default:
                current_char = hp_digits[string_buffer[i] - '0'];
                break;
        }

        if (current_char != NULL) {
            Gfx_DrawTextureRgba16(x, y, 8, 6, current_char, 8, 0);
        }

        x += 8;
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

/*
    Draws the Trainer's name for Trainers who have not yet selected a move 
    If the current pokemon has an illegal move, the player's name will show up as violet
    see Battle_MarkIllegalMoves in @src/fragments/battle_engine/battle_engine_move_animation.c
*/
void BattleHud_DrawTrainerNameLabel(s16 x, s16 y, s32 port, char* name, s32 has_illegal_move) {
    Battle_DrawGradientRectWrapper(x, y, 70, 17, &sPlayerNameTagStartColor[port], &sPlayerNameTagEndColor[port]);
    Font_BeginTranslucentTextRendering();
    if (has_illegal_move != 0) {
        Gfx_SetEnvColor(0xFF, 0x64, 0xFF, 0xFF); // #FF64FF
    }
    Font_SetActive(2, 0);
    Font_Printf((x - (Font_MeasureTextExtent(2, 0, name) / 2)) + 0x23, y + 2, name);
    Font_EndTexturedTextRendering();
}

/*
    Draws the Trainer's name for Trainers who have yet selected a move 
    If the current pokemon has an illegal move, the player's name will show up as violet
    The nametag will blink and switch between the {trainer anme} and "READY!"
    see Battle_MarkIllegalMoves in @src/fragments/battle_engine/battle_engine_move_animation.c
*/
void BattleHud_DrawTrainerNameLabelBlink(s16 x, s16 y, s32 port, u8* name, s32 blink_phase, s32 has_illegal_move) {
    s32 ready_message_text_length;
    s16 blink_value;
    s16 alpha;

    blink_value = COSS(blink_phase) * 255.0f;
    alpha = blink_value;
    if (blink_value < 0) {
        alpha = blink_value * -1;
    }

    Battle_DrawGradientRectWrapper(x, y, 70, 17, &sPlayerNameTagStartColor[port], &sPlayerNameTagEndColor[port]);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);

    if (has_illegal_move != 0) {
        Gfx_SetEnvColor(0xFF, 0x64, 0xFF, (s8)alpha); // #FF64FF
    } else {
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, (s8)alpha); // #FFFFFF
    }

    if (blink_value > 0) {
        // READY!
        ready_message_text_length = Font_MeasureTextExtent(2, 0, Text_GetString(NULL, 0, sBattleMessagesTable, 0xAA)) / 2;
        Font_Printf((x - ready_message_text_length) + 35, y + 2, Text_GetString(NULL, 0, sBattleMessagesTable, 0xAA));
    } else {
        Font_Printf((x - (Font_MeasureTextExtent(2, 0, name) / 2)) + 35, y + 2, name);
    }

    Font_EndTexturedTextRendering();
}

void BattleHud_DrawTrainerNameLabelAuto(ActivePokemon* pokemon, TeamRoster* pokemon_team, s16 x, s16 y, s32 port, u16 blink_phase, s32 selected_move) {
    ActivePokemonState* state = &pokemon->state;
    s32 has_illegal_move = 0;
    s32 i;

    for (i = 0; i < 4; i++) {
        if (state->moveSlotIllegality[i] == 1) {
            has_illegal_move = 1;
        }
    }

    if (selected_move == 0) {
        BattleHud_DrawTrainerNameLabel(x, y, port, &pokemon_team->trainerName, has_illegal_move);
    } else {
        BattleHud_DrawTrainerNameLabelBlink(x, y, port, &pokemon_team->trainerName, blink_phase, has_illegal_move);
    }
}

void BattleHud_DrawMonInfoPanel(ActivePokemon* active_pokemon, BattleMon* pokemon_info, StatusConditionLabel* status_label, s32 x, s32 y,
                   s32 width, s32 length, s32 r, s32 g, s32 b, s32 alpha) {
    s32 name_length;
    s32 adjusted_width;

    if (x < 0) {
        adjusted_width = width + x;
        if (adjusted_width < 0) {
            Gfx_FillRectWithDisplayState(0, y, 0, length, r, g, b, alpha);
        } else {
            Gfx_FillRectWithDisplayState(0, y, adjusted_width, length, r, g, b, alpha);
        }
    } else {
        Gfx_FillRectWithDisplayState(x, y, width, length, r, g, b, alpha);
    }

    BattleHud_DrawLevelNumber(x + 3, y + 16, pokemon_info->level);
    BattleHud_DrawHpBar(x + 2, y + 25, sCurrentPokemonHp[BattleScene_GetParticipantSideIndex(active_pokemon)], pokemon_info->maxHp);
    BattleHud_DrawHpFractionText(x + 12, y + 31, pokemon_info->hp, pokemon_info->maxHp);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);

    name_length = Font_MeasureTextExtent(2, 0, Battle_GetActiveMonNickname(active_pokemon)) / 2;
    Font_Printf((x - name_length) + 35, y + 2, Battle_GetActiveMonNickname(active_pokemon));
    name_length = Font_MeasureTextExtent(1, 0, Text_GetString(NULL, 0, sStatusConditionNameTable, status_label->stringIndex));

    if (name_length >= 40) {
        Font_SetActive(1, -1);
        name_length = x + 26;
    } else {
        Font_SetActive(1, 0);
        name_length = (x - (name_length / 2)) + 44;
    }

    Gfx_SetEnvColor(status_label->color.r, status_label->color.g, status_label->color.b, 0xFF);
    Font_Printf(name_length, y + 14, Text_GetString(NULL, 0, sStatusConditionNameTable, status_label->stringIndex));
    Font_EndTexturedTextRendering();
}

s32 BattleHud_DrawBorderTile(s32 x, s32 y, s32 width, s32 height, u8* texture) {
    UNUSED s32 pad[4];
    s32 texture_step_x;
    s32 texture_step_y;

    if (width < 9) {
        texture_step_x = 0x400;
    } else {
        texture_step_x = 0;
    }

    // clang-format off
    if (height < 9) { texture_step_y = 0x400; } else { texture_step_y = 0; }
    // clang-format on

    gDPLoadTextureBlock(gDisplayListHead++, texture, G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRect(x, y, (x + width) - 1, (y + height) - 1, texture_step_x, texture_step_y, 0);
}

void func_84311420(void) {
}

void BattleHud_DrawBoxBorder(s16 x, s16 y, s16 width, s16 height) {
    s32 i;

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    BattleHud_DrawBorderTile(x, y - 1, 8, 8, gBorderTopLeft);
    BattleHud_DrawBorderTile(x, (y + height) - 6, 8, 8, gBorderBottomLeft);
    BattleHud_DrawBorderTile((x + width) - 6, y - 1, 8, 8, gBorderTopRight);
    BattleHud_DrawBorderTile((x + width) - 6, (y + height) - 6, 8, 8, gBorderBottomRight);

    for (i = 0; i < height / 8; i++) {
        if ((height - 4) >= (6 + i * 8)) {
            BattleHud_DrawBorderTile((x + width) - 6, y + (i * 8) + 4, 8, 8, gBorderRight);
        }
    }

    for (i = 0; i < height / 8; i++) {
        if ((height - 4) >= (6 + i * 8)) {
            BattleHud_DrawBorderTile(x, y + (i * 8) + 4, 8, 8, gBorderLeft);
        }
    }

    for (i = 0; i < width / 8; i++) {
        if ((width - 4) >= (6 + i * 8)) {
            BattleHud_DrawBorderTile(x + (i * 8) + 3, y - 1, 8, 8, gBorderTop);
        }

        if ((width - 4) >= (6 + i * 8)) {
            BattleHud_DrawBorderTile(x + (i * 8) + 3, (y + height) - 6, 8, 8, gBorderBottom);
        }
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void BattleHud_DrawParticipantTimer(s16 x, s16 y) {
    Color_RGB8 timer_colors[] = {
        { 0xB4, 0x00, 0x00 }, // #B40000 red
        { 0xCC, 0x8F, 0x00 }, // #CC8F00 amber
        { 0x00, 0xB4, 0x00 }, // #00B400 green
        { 0x00, 0x00, 0x00 }, // #000000 black
    };
    char string_buffer[16];
    UNUSED s32 pad[4];
    s32 color_index;
    s32 timer_string;

    timer_string = ((s32)((sOrderTimer / 30.0f) / 60.0f) * 0x3C) + ((s32)(sOrderTimer / 30.0f) % 60);
    if (timer_string >= 11) {
        color_index = 2;
    } else if (timer_string >= 4) {
        color_index = 1;
    } else {
        color_index = 0;
    }

    Gfx_FillRectWithDisplayState(x - 1, y, 0x14, 0xD, timer_colors[color_index].r, timer_colors[color_index].g, timer_colors[color_index].b, 0xBE);
    BattleHud_DrawBoxBorder(x - 2, y - 1, 0x14, 0xC);

    sprintf(string_buffer, "%02d", timer_string);

    Font_BeginTranslucentTextRendering();
    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #FFFFFF
    Font_SetActive(1, 0);
    Font_Printf(x + 3, y, string_buffer);
    Font_EndTexturedTextRendering();
    BattleHud_DrawTranslucentShadowRect(x - 3, y - 3, 0x19, 0x12, 0);
}

void BattleHud_DrawBattleTimer(s16 x, s16 y) {
    char string_buffer[16];
    f32 time_left = sBattleTimer / 30.0f;
    UNUSED s32 pad[2];

    Gfx_FillRectWithDisplayState(x, y, 39, 12, 0, 0, 0, 0x5A);
    BattleHud_DrawBoxBorder(x - 2, y - 1, 39, 12);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);
    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #FFFFFF
    sprintf(string_buffer, "%02d", (s32)(time_left / 60.0f));
    Font_SetActive(1, 0);
    Font_Printf(x + 3, y, string_buffer);
    Font_Printf(x + 16, y, ":");
    sprintf(string_buffer, "%02d", (s32)time_left % 60);
    Font_SetActive(1, 0);
    Font_Printf(x + 23, y, string_buffer);
    Font_EndTexturedTextRendering();
    BattleHud_DrawTranslucentShadowRect(x - 3, y - 3, 44, 17, 0);
}

void BattleHud_DrawPartyBallRow(ActivePokemon* pokemon, s16 x, s16 y) {
    u8* poke_ball_icon_textures[] = { gPokeBallStatusHealthy, gPokeBallStatusStatused, gPokeBallStatusFainted };
    UNUSED s32 pad;
    s32 teammate1_index;
    s32 teammate2_index;
    s32 pokemon_status;

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    if (pokemon == sActivePokemon[0]) {
        if (pokemon->sessionTeams->tagTeamSize == 1) {
            for (teammate1_index = 0; teammate1_index < pokemon->sessionTeams->teamMember[0]->numberOfPokemonBrought; teammate1_index++) {
                if (pokemon->sessionTeams->teamMember[0]->pokemonPicked[teammate1_index].hp == 0) {
                    pokemon_status = 2;
                } else if (pokemon->sessionTeams->teamMember[0]->pokemonPicked[teammate1_index].statusCondition != 0) {
                    pokemon_status = 1;
                } else {
                    pokemon_status = 0;
                }
                Gfx_DrawTextureRgba16(x + teammate1_index * 7, y, 8, 6, poke_ball_icon_textures[pokemon_status], 8, 0);
            }
        } else {
            for (teammate1_index = 0; teammate1_index < pokemon->sessionTeams->teamMember[0]->numberOfPokemonBrought; teammate1_index++) {
                if (pokemon->sessionTeams->teamMember[0]->pokemonPicked[teammate1_index].hp == 0) {
                    pokemon_status = 2;
                } else if (pokemon->sessionTeams->teamMember[0]->pokemonPicked[teammate1_index].statusCondition != 0) {
                    pokemon_status = 1;
                } else {
                    pokemon_status = 0;
                }
                Gfx_DrawTextureRgba16(x + teammate1_index * 7, y, 8, 6, poke_ball_icon_textures[pokemon_status], 8, 0);
            }

            for (teammate2_index = 0; teammate2_index < pokemon->sessionTeams->teamMember[1]->numberOfPokemonBrought; teammate2_index++) {
                if (pokemon->sessionTeams->teamMember[1]->pokemonPicked[teammate2_index].hp == 0) {
                    pokemon_status = 2;
                } else if (pokemon->sessionTeams->teamMember[1]->pokemonPicked[teammate2_index].statusCondition != 0) {
                    pokemon_status = 1;
                } else {
                    pokemon_status = 0;
                }
                Gfx_DrawTextureRgba16(x + teammate1_index * 7 + teammate2_index * 7, y, 8, 6, poke_ball_icon_textures[pokemon_status], 8, 0);
            }
        }
    }

    if (pokemon == sActivePokemon[1]) {
        if (pokemon->sessionTeams->tagTeamSize == 1) {
            for (teammate1_index = 0; teammate1_index < pokemon->sessionTeams->teamMember[0]->numberOfPokemonBrought; teammate1_index++) {
                if (pokemon->sessionTeams->teamMember[0]->pokemonPicked[teammate1_index].hp == 0) {
                    pokemon_status = 2;
                } else if (pokemon->sessionTeams->teamMember[0]->pokemonPicked[teammate1_index].statusCondition != 0) {
                    pokemon_status = 1;
                } else {
                    pokemon_status = 0;
                }
                Gfx_DrawTextureRgba16(x - teammate1_index * 7, y, 8, 6, poke_ball_icon_textures[pokemon_status], 8, 0);
            }
        } else {
            for (teammate1_index = 0; teammate1_index < pokemon->sessionTeams->teamMember[0]->numberOfPokemonBrought; teammate1_index++) {
                if (pokemon->sessionTeams->teamMember[0]->pokemonPicked[teammate1_index].hp == 0) {
                    pokemon_status = 2;
                } else if (pokemon->sessionTeams->teamMember[0]->pokemonPicked[teammate1_index].statusCondition != 0) {
                    pokemon_status = 1;
                } else {
                    pokemon_status = 0;
                }
                Gfx_DrawTextureRgba16(x - teammate1_index * 7, y, 8, 6, poke_ball_icon_textures[pokemon_status], 8, 0);
            }

            for (teammate2_index = 0; teammate2_index < pokemon->sessionTeams->teamMember[1]->numberOfPokemonBrought; teammate2_index++) {
                if (pokemon->sessionTeams->teamMember[1]->pokemonPicked[teammate2_index].hp == 0) {
                    pokemon_status = 2;
                } else if (pokemon->sessionTeams->teamMember[1]->pokemonPicked[teammate2_index].statusCondition != 0) {
                    pokemon_status = 1;
                } else {
                    pokemon_status = 0;
                }
                Gfx_DrawTextureRgba16(x - teammate1_index * 7 - teammate2_index * 7, y, 8, 6, poke_ball_icon_textures[pokemon_status], 8, 0);
            }
        }
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void BattleHud_DrawMonStatusBoxPlayer(ActivePokemon* active_pokemon, s16 x, s16 y, u16 blink_phase, s32 selected_move) {
    UNUSED s32 pad[3];
    TeamRoster* roster;
    BattleMon* party_pokemon;
    Color_RGB8* port_color;
    u32 status;

    roster = active_pokemon->sessionTeams->teamMember[active_pokemon->thumbnail.tagTeam];
    party_pokemon = &active_pokemon->sessionTeams->teamMember[active_pokemon->thumbnail.tagTeam]->pokemonPicked[active_pokemon->state.rosterIndex];

    if (party_pokemon->hp == 0) {
        status = 1;
    } else {
        status = Status_ToDisplayIconIndex(party_pokemon->statusCondition);
    }

    port_color = &sPlayerNameTagStartColor[roster->controllerPort];
    BattleHud_DrawMonInfoPanel(active_pokemon, party_pokemon, &sStatusLabelConfigs[status], x + 1, y + 18, 70, 40, port_color->r, port_color->g,
                  port_color->b, 0xB2);
    BattleHud_DrawTrainerNameLabelAuto(active_pokemon, roster, x + 1, y + 1, roster->controllerPort, blink_phase, selected_move);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    BattleHud_DrawBorderTile(x, y, 8, 8, gBorderTopLeft);
    BattleHud_DrawBorderTile(x, y + 15, 8, 8, D_3008200);
    BattleHud_DrawBorderTile(x, y + 54, 8, 8, D_3008200);
    BattleHud_DrawBorderTile(x + 65, y, 8, 8, gBorderTopRight);
    BattleHud_DrawBorderTile(x + 65, y + 15, 8, 8, gBorderTeeRight);
    BattleHud_DrawBorderTile(x + 65, y + 51, 8, 8, gBorderBottomRight);
    BattleHud_DrawBorderTile(x + 8, y, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 16, y, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 24, y, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 32, y, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 40, y, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 48, y, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 56, y, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 57, y, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 8, y + 18, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 16, y + 18, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 24, y + 18, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 32, y + 18, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 40, y + 18, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 48, y + 18, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 56, y + 18, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 57, y + 18, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 8, y + 51, 8, 8, gBorderBottom);
    BattleHud_DrawBorderTile(x + 16, y + 51, 8, 8, gBorderBottom);
    BattleHud_DrawBorderTile(x + 24, y + 51, 8, 8, gBorderBottom);
    BattleHud_DrawBorderTile(x + 32, y + 51, 8, 8, gBorderBottom);
    BattleHud_DrawBorderTile(x + 40, y + 51, 8, 8, gBorderBottom);
    BattleHud_DrawBorderTile(x + 48, y + 51, 8, 8, gBorderBottom);
    BattleHud_DrawBorderTile(x + 56, y + 51, 8, 8, gBorderBottom);
    BattleHud_DrawBorderTile(x + 57, y + 51, 8, 8, gBorderBottom);
    BattleHud_DrawBorderTile(x, y + 8, 8, 8, gBorderLeft);
    BattleHud_DrawBorderTile(x, y + 21, 8, 8, gBorderLeft);
    BattleHud_DrawBorderTile(x, y + 29, 8, 8, gBorderLeft);
    BattleHud_DrawBorderTile(x, y + 37, 8, 8, gBorderLeft);
    BattleHud_DrawBorderTile(x, y + 45, 8, 8, gBorderLeft);
    BattleHud_DrawBorderTile(x, y + 46, 8, 8, gBorderLeft);
    BattleHud_DrawBorderTile(x + 65, y + 8, 8, 8, gBorderRight);
    BattleHud_DrawBorderTile(x + 65, y + 21, 8, 8, gBorderRight);
    BattleHud_DrawBorderTile(x + 65, y + 29, 8, 8, gBorderRight);
    BattleHud_DrawBorderTile(x + 65, y + 37, 8, 8, gBorderRight);
    BattleHud_DrawBorderTile(x + 65, y + 45, 8, 8, gBorderRight);
    BattleHud_DrawBorderTile(x + 65, y + 47, 8, 8, gBorderRight);
    BattleHud_DrawBorderTile(x, y + 61, 8, 8, gBorderLeft);
    BattleHud_DrawBorderTile(x, y + 69, 8, 8, gBorderLeft);
    BattleHud_DrawBorderTile(x, y + 77, 8, 8, gBorderLeft);
    BattleHud_DrawBorderTile(x, y + 85, 8, 8, gBorderLeft);
    BattleHud_DrawBorderTile(x + 33, y + 58, 8, 8, gBorderRight);
    BattleHud_DrawBorderTile(x + 33, y + 61, 8, 8, gBorderRight);
    BattleHud_DrawBorderTile(x + 33, y + 69, 8, 8, gBorderRight);
    BattleHud_DrawBorderTile(x + 33, y + 77, 8, 8, gBorderRight);
    BattleHud_DrawBorderTile(x + 33, y + 85, 8, 8, gBorderRight);
    BattleHud_DrawBorderTile(x + 33, y + 89, 8, 8, gBorderBottomRight);
    BattleHud_DrawBorderTile(x, y + 89, 8, 8, gBorderBottomLeft);
    BattleHud_DrawBorderTile(x + 4, y + 89, 8, 8, gBorderBottom);
    BattleHud_DrawBorderTile(x + 12, y + 89, 8, 8, gBorderBottom);
    BattleHud_DrawBorderTile(x + 20, y + 89, 8, 8, gBorderBottom);
    BattleHud_DrawBorderTile(x + 28, y + 89, 8, 8, gBorderBottom);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    BattleHud_DrawTranslucentShadowRect(x - 3, y - 2, 78, 63, 0);
    BattleHud_DrawTranslucentShadowRect(x - 3, y + 58, 45, 41, 0);
    BattleHud_DrawPartyBallRow(active_pokemon, x, y + 98);
}

void BattleHud_DrawMonStatusBoxOpponent(ActivePokemon* active_pokemon, s16 x, s16 y, u16 blink_phase, s32 selected_move) {
    UNUSED s32 pad[3];
    TeamRoster* roster;
    BattleMon* party_pokemon;
    Color_RGB8* port_color;
    u32 status;

    roster = active_pokemon->sessionTeams->teamMember[active_pokemon->thumbnail.tagTeam];
    party_pokemon = &active_pokemon->sessionTeams->teamMember[active_pokemon->thumbnail.tagTeam]->pokemonPicked[active_pokemon->state.rosterIndex];

    if (party_pokemon->hp == 0) {
        status = 1;
    } else {
        status = Status_ToDisplayIconIndex(party_pokemon->statusCondition);
    }

    port_color = &sPlayerNameTagStartColor[roster->controllerPort];
    BattleHud_DrawMonInfoPanel(active_pokemon, party_pokemon, &sStatusLabelConfigs[status], x, y + 40, 70, 40, port_color->r, port_color->g, port_color->b,
                  0xB2);
    BattleHud_DrawTrainerNameLabelAuto(active_pokemon, roster, x, y + 80, roster->controllerPort, blink_phase, selected_move);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    BattleHud_DrawBorderTile(x, y + 76, 8, 8, D_3008200);
    BattleHud_DrawBorderTile(x, y + 91, 8, 8, gBorderBottomLeft);
    BattleHud_DrawBorderTile(x, y + 39, 8, 8, gBorderTopLeft);
    BattleHud_DrawBorderTile(x + 63, y + 36, 8, 8, gBorderTeeRight);
    BattleHud_DrawBorderTile(x + 63, y + 76, 8, 8, gBorderTeeRight);
    BattleHud_DrawBorderTile(x + 63, y + 91, 8, 8, gBorderBottomRight);
    BattleHud_DrawBorderTile(x + 7, y + 39, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 8, y + 39, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 16, y + 39, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 24, y + 39, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 32, y + 39, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 40, y + 39, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 48, y + 39, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 56, y + 39, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 7, y + 79, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 8, y + 79, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 16, y + 79, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 24, y + 79, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 32, y + 79, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 40, y + 79, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 48, y + 79, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 56, y + 79, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 7, y + 91, 8, 8, gBorderBottom);
    BattleHud_DrawBorderTile(x + 8, y + 91, 8, 8, gBorderBottom);
    BattleHud_DrawBorderTile(x + 16, y + 91, 8, 8, gBorderBottom);
    BattleHud_DrawBorderTile(x + 24, y + 91, 8, 8, gBorderBottom);
    BattleHud_DrawBorderTile(x + 32, y + 91, 8, 8, gBorderBottom);
    BattleHud_DrawBorderTile(x + 40, y + 91, 8, 8, gBorderBottom);
    BattleHud_DrawBorderTile(x + 48, y + 91, 8, 8, gBorderBottom);
    BattleHud_DrawBorderTile(x + 56, y + 91, 8, 8, gBorderBottom);
    BattleHud_DrawBorderTile(x, y + 44, 8, 8, gBorderLeft);
    BattleHud_DrawBorderTile(x, y + 52, 8, 8, gBorderLeft);
    BattleHud_DrawBorderTile(x, y + 60, 8, 8, gBorderLeft);
    BattleHud_DrawBorderTile(x, y + 68, 8, 8, gBorderLeft);
    BattleHud_DrawBorderTile(x, y + 82, 8, 8, gBorderLeft);
    BattleHud_DrawBorderTile(x, y + 83, 8, 8, gBorderLeft);
    BattleHud_DrawBorderTile(x + 63, y + 44, 8, 8, gBorderRight);
    BattleHud_DrawBorderTile(x + 63, y + 52, 8, 8, gBorderRight);
    BattleHud_DrawBorderTile(x + 63, y + 60, 8, 8, gBorderRight);
    BattleHud_DrawBorderTile(x + 63, y + 68, 8, 8, gBorderRight);
    BattleHud_DrawBorderTile(x + 63, y + 82, 8, 8, gBorderRight);
    BattleHud_DrawBorderTile(x + 63, y + 83, 8, 8, gBorderRight);
    BattleHud_DrawBorderTile(x + 63, y, 8, 8, gBorderTopRight);
    BattleHud_DrawBorderTile(x + 30, y, 8, 8, gBorderTopLeft);
    BattleHud_DrawBorderTile(x + 63, y + 8, 8, 8, gBorderRight);
    BattleHud_DrawBorderTile(x + 63, y + 16, 8, 8, gBorderRight);
    BattleHud_DrawBorderTile(x + 63, y + 24, 8, 8, gBorderRight);
    BattleHud_DrawBorderTile(x + 63, y + 32, 8, 8, gBorderRight);
    BattleHud_DrawBorderTile(x + 30, y + 8, 8, 8, gBorderLeft);
    BattleHud_DrawBorderTile(x + 30, y + 16, 8, 8, gBorderLeft);
    BattleHud_DrawBorderTile(x + 30, y + 24, 8, 8, gBorderLeft);
    BattleHud_DrawBorderTile(x + 30, y + 31, 8, 8, gBorderLeft);
    BattleHud_DrawBorderTile(x + 32, y, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 40, y, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 48, y, 8, 8, gBorderTop);
    BattleHud_DrawBorderTile(x + 55, y, 8, 8, gBorderTop);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    BattleHud_DrawTranslucentShadowRect(x + 27, y - 2, 47, 42, 0);
    BattleHud_DrawTranslucentShadowRect(x - 4, y + 38, 77, 61, 0);
    BattleHud_DrawPartyBallRow(active_pokemon, x + 0x41, y - 7);
}

void BattleHud_DrawThreeOptionLabelBar(UNUSED ActivePokemon* pokemon, s16 x, s16 y, s32 port) {
    UNUSED s32 pad[2];

    Battle_DrawGradientRectWrapper(x + 1, y + 1, 198, 16, &sPlayerNameTagStartColor[port], &sPlayerNameTagEndColor[port]);
    BattleHud_DrawBoxBorder(x, y, 198, 16);

    gSPDisplayList(gDisplayListHead++, gUiMaskedTextureDL);

    Gfx_DrawTextureRgba16Ia8(x + 7, y + 3, 16, 12, gButtonA, gButtonAMask, 0x10, 0x100000);
    Gfx_DrawTextureRgba16Ia8(x + 70, y + 3, 16, 12, gButtonB, gButtonBMask, 0x10, 0x100000);
    Gfx_DrawTextureRgba16Ia8(x + 134, y + 3, 16, 12, gButtonStart, gButtonStartMask, 0x10, 0x100000);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    Font_BeginTranslucentTextRendering();
    Font_SetActive(1, 0);
    Font_Printf(x + 24, y + 3, Text_GetString(NULL, 0, sBattleMessagesTable, 0x9E));
    Font_Printf(x + 88, y + 3, Text_GetString(NULL, 0, sBattleMessagesTable, 0x9F));
    Font_Printf(x + 152, y + 3, Text_GetString(NULL, 0, sBattleMessagesTable, 0xA0));
    Font_EndTexturedTextRendering();
    BattleHud_DrawTranslucentShadowRect(x - 2, y - 2, 204, 22, 0);
}

void BattleHud_DrawTwoOptionLabelBar(UNUSED ActivePokemon* pokemon, s16 x, s16 y, s32 port) {
    UNUSED s32 pad[2];

    Battle_DrawGradientRectWrapper(x, y, 125, 16, &sPlayerNameTagStartColor[port], &sPlayerNameTagEndColor[port]);
    BattleHud_DrawBoxBorder(x, y, 125, 16);

    gSPDisplayList(gDisplayListHead++, gUiMaskedTextureDL);

    Gfx_DrawTextureRgba16Ia8(x + 7, y + 3, 16, 12, gButtonL, gButtonLMask, 0x10, 0x100000);
    Gfx_DrawTextureRgba16Ia8(x + 64, y + 3, 16, 12, gButtonR, gButtonRMask, 0x10, 0x100000);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    Font_BeginTranslucentTextRendering();
    Font_SetActive(1, 0);
    Font_Printf(x + 24, y + 3, Text_GetString(NULL, 0, sBattleMessagesTable, 0xA9));
    Font_Printf(x + 81, y + 3, Text_GetString(NULL, 0, sBattleMessagesTable, 0xA8));
    Font_EndTexturedTextRendering();
    BattleHud_DrawTranslucentShadowRect(x - 2, y - 2, 131, 22, 0);
}

void BattleHud_DrawOneOptionLabelBar(UNUSED ActivePokemon* pokemon, s16 x, s16 y, s32 port) {
    s32 pad[2];

    Battle_DrawGradientRectWrapper(x + 1, y + 1, 71, 16, &sPlayerNameTagStartColor[port], &sPlayerNameTagEndColor[port]);
    BattleHud_DrawBoxBorder(x, y, 71, 16);

    gSPDisplayList(gDisplayListHead++, gUiMaskedTextureDL);

    Gfx_DrawTextureRgba16Ia8(x + 7, y + 3, 16, 12, gButtonR, gButtonRMask, 0x10, 0x100000);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    Font_BeginTranslucentTextRendering();
    Font_SetActive(1, 0);
    Font_Printf(x + 24, y + 3, Text_GetString(NULL, 0, sBattleMessagesTable, 0xA8));
    Font_EndTexturedTextRendering();
    BattleHud_DrawTranslucentShadowRect(x - 2, y - 2, 74, 22, 0);
}

void Battle_DrawRightAlignedText(s32 x, s32 y, char* string) {
    Font_Printf(x - (strlen(string) * 6), y, string);
}

s32 BattleHud_DrawMovePpTextLTR(s32 x, s32 y, s32 current_pp, s32 max_pp) {
    UNUSED s32 pad[2];
    s8 string_buffer[7];
    s32 i;
    u8** pp_digits;
    u8* digit;

    pp_digits = Memmap_GetSegmentVaddr(gHudGlyphTable);
    sprintf(string_buffer, "%3d/%3d", current_pp & 0x3F, max_pp);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    for (i = 0; i < 7; i++) {
        s32 c = string_buffer[i];

        switch (c) {
            case ' ':
                digit = NULL;
                break;

            case '/':
                digit = pp_digits[10];
                break;

            default:
                if ((c >= '0') && (c < ':')) {
                    digit = pp_digits[c - '0'];
                } else {
                    digit = NULL;
                }
                break;
        }

        if (digit != NULL) {
            Gfx_DrawTextureRgba16(x, y, 8, 6, digit, 8, 0);
            x += 8;
        }
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    return x;
}

s32 BattleHud_DrawMovePpTextRTL(s32 x, s32 y, s32 current_pp, s32 max_pp) {
    UNUSED s32 pad[2];
    s8 string_buffer[7];
    s32 i;
    u8** pp_digits;
    u8* digit;

    pp_digits = Memmap_GetSegmentVaddr(gHudGlyphTable);
    sprintf(string_buffer, "%3d/%3d", current_pp & 0x3F, max_pp);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    for (i = 7; i > 0; i--) {
        s32 c = string_buffer[i];

        switch (c) {
            case ' ':
                digit = NULL;
                break;

            case '/':
                digit = pp_digits[10];
                break;

            default:
                if ((c >= '0') && (c < ':')) {
                    digit = pp_digits[c - '0'];
                } else {
                    digit = NULL;
                }
                break;
        }

        if (digit != NULL) {
            Gfx_DrawTextureRgba16(x, y, 8, 6, digit, 8, 0);
            x -= 8;
        }
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    return x;
}

#define NUMBER_OF_MOVES_IN_GAME 166
void BattleHud_DrawMoveSelectMenu(ActivePokemon* active_pokemon, s16 x, s16 y, s32 port) {
    s32 i;
    s32 move_pp[] = { 0, 0, 0, 0 };
    u8 moves_max_pp[4];
    BattleMonRuntime* runtime;
    BattleMon* party_pokemon;
    UNUSED s32 pad;

    party_pokemon = &active_pokemon->sessionTeams->teamMember[active_pokemon->thumbnail.tagTeam]->pokemonPicked[active_pokemon->state.rosterIndex];
    runtime = &active_pokemon->state.runtime;

    Battle_DrawGradientRectWrapper(x + 1, y + 1, 199, 56, &sPlayerNameTagStartColor[port], &sPlayerNameTagEndColor[port]);
    BattleHud_DrawBoxBorder(x, y, 199, 56);

    for (i = 0; i < 4; i++) {
        if (runtime->battleStatus3 & 8) {
            moves_max_pp[i] = gMoveData[runtime->moves[i] - 1].pp / 5;
            if (moves_max_pp[i] >= 8) {
                moves_max_pp[i] = 7;
            }
            moves_max_pp[i] = gMoveData[runtime->moves[i] - 1].pp + (((runtime->pp[i] & 0xC0) >> 6) * moves_max_pp[i]);
        } else {
            moves_max_pp[i] = gMoveData[party_pokemon->moves[i] - 1].pp / 5;
            if (moves_max_pp[i] >= 8) {
                moves_max_pp[i] = 7;
            }
            moves_max_pp[i] = gMoveData[party_pokemon->moves[i] - 1].pp + (((party_pokemon->pp[i] & 0xC0) >> 6) * moves_max_pp[i]);
        }
    }

    if (runtime->battleStatus3 & 8) {
        if ((runtime->moves[0] != 0) && (runtime->moves[0] < (NUMBER_OF_MOVES_IN_GAME))) {
            move_pp[0] = BattleHud_DrawMovePpTextRTL(x + 82, y + 15, runtime->pp[0], moves_max_pp[0]);
        }

        if ((runtime->moves[1] != 0) && (runtime->moves[1] < NUMBER_OF_MOVES_IN_GAME)) {
            move_pp[1] = BattleHud_DrawMovePpTextLTR(x + 124, y + 28, runtime->pp[1], moves_max_pp[1]);
        }

        if ((runtime->moves[2] != 0) && (runtime->moves[2] < NUMBER_OF_MOVES_IN_GAME)) {
            move_pp[2] = BattleHud_DrawMovePpTextLTR(x + 112, y + 48, runtime->pp[2], moves_max_pp[2]);
        }

        if ((runtime->moves[3] != 0) && (runtime->moves[3] < NUMBER_OF_MOVES_IN_GAME)) {
            move_pp[3] = BattleHud_DrawMovePpTextRTL(x + 68, y + 35, runtime->pp[3], moves_max_pp[3]);
        }
    } else {
        if ((party_pokemon->moves[0] != 0) && (party_pokemon->moves[0] < NUMBER_OF_MOVES_IN_GAME)) {
            move_pp[0] = BattleHud_DrawMovePpTextRTL(x + 82, y + 15, party_pokemon->pp[0], moves_max_pp[0]);
        }

        if ((party_pokemon->moves[1] != 0) && (party_pokemon->moves[1] < NUMBER_OF_MOVES_IN_GAME)) {
            move_pp[1] = BattleHud_DrawMovePpTextLTR(x + 124, y + 28, party_pokemon->pp[1], moves_max_pp[1]);
        }

        if ((party_pokemon->moves[2] != 0) && (party_pokemon->moves[2] < NUMBER_OF_MOVES_IN_GAME)) {
            move_pp[2] = BattleHud_DrawMovePpTextLTR(x + 112, y + 48, party_pokemon->pp[2], moves_max_pp[2]);
        }

        if ((party_pokemon->moves[3] != 0) && (party_pokemon->moves[3] < NUMBER_OF_MOVES_IN_GAME)) {
            move_pp[3] = BattleHud_DrawMovePpTextRTL(x + 68, y + 35, party_pokemon->pp[3], moves_max_pp[3]);
        }
    }

    gSPDisplayList(gDisplayListHead++, gUiMaskedTextureDL);

    Gfx_DrawTextureRgba16Ia8(x + 93, y + 6, 16, 14, &gButtonCUp, &gButtonCUpMask, 0x10, 0x100000);
    Gfx_DrawTextureRgba16Ia8(x + 108, y + 22, 16, 14, &gButtonCRight, &gButtonCRightMask, 0x10, 0x100000);
    Gfx_DrawTextureRgba16Ia8(x + 93, y + 38, 16, 14, &gButtonCDown, &gButtonCDownMask, 0x10, 0x100000);
    Gfx_DrawTextureRgba16Ia8(x + 78, y + 22, 16, 14, &gButtonCLeft, &gButtonCLeftMask, 0x10, 0x100000);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    if (runtime->battleStatus3 & 8) {
        if ((runtime->moves[0] != 0) && (runtime->moves[0] < NUMBER_OF_MOVES_IN_GAME)) {
            Gfx_DrawTextureRgba16(move_pp[0] - 25, y + 14, 32, 9, sTypeLabels[gMoveData[runtime->moves[0] - 1].type],
                          32, 0);
        }

        if ((runtime->moves[1] != 0) && (runtime->moves[1] < NUMBER_OF_MOVES_IN_GAME)) {
            Gfx_DrawTextureRgba16(move_pp[1], y + 27, 32, 9, sTypeLabels[gMoveData[runtime->moves[1] - 1].type], 32, 0);
        }

        if ((runtime->moves[2] != 0) && (runtime->moves[2] < NUMBER_OF_MOVES_IN_GAME)) {
            Gfx_DrawTextureRgba16(move_pp[2], y + 47, 32, 9, sTypeLabels[gMoveData[runtime->moves[2] - 1].type], 32, 0);
        }

        if ((runtime->moves[3] != 0) && (runtime->moves[3] < NUMBER_OF_MOVES_IN_GAME)) {
            Gfx_DrawTextureRgba16(move_pp[3] - 24, y + 34, 32, 9, sTypeLabels[gMoveData[runtime->moves[3] - 1].type],
                          32, 0);
        }
    } else {
        if ((runtime->moves[0] != 0) && (runtime->moves[0] < NUMBER_OF_MOVES_IN_GAME)) {
            Gfx_DrawTextureRgba16(move_pp[0] - 25, y + 14, 32, 9, sTypeLabels[gMoveData[runtime->moves[0] - 1].type],
                          32, 0);
        }

        if ((runtime->moves[1] != 0) && (runtime->moves[1] < NUMBER_OF_MOVES_IN_GAME)) {
            Gfx_DrawTextureRgba16(move_pp[1], y + 27, 32, 9, sTypeLabels[gMoveData[runtime->moves[1] - 1].type], 32,
                          0);
        }

        if ((runtime->moves[2] != 0) && (runtime->moves[2] < NUMBER_OF_MOVES_IN_GAME)) {
            Gfx_DrawTextureRgba16(move_pp[2], y + 47, 32, 9, sTypeLabels[gMoveData[runtime->moves[2] - 1].type], 32,
                          0);
        }

        if ((runtime->moves[3] != 0) && (runtime->moves[3] < NUMBER_OF_MOVES_IN_GAME)) {
            Gfx_DrawTextureRgba16(move_pp[3] - 24, y + 34, 32, 9, sTypeLabels[gMoveData[runtime->moves[3] - 1].type],
                          32, 0);
        }
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    Font_BeginTranslucentTextRendering();
    Font_SetActive(1, 0);

    if (runtime->battleStatus3 & 8) {
        if ((runtime->moves[0] != 0) && (runtime->moves[0] < NUMBER_OF_MOVES_IN_GAME)) {
            if (active_pokemon->state.moveSlotIllegality[0] == 1) {
                Gfx_SetEnvColor(0xFF, 0x64, 0xFF, 0xFF); // #FF64FF
            } else {
                Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #FFFFFF
            }
            Battle_DrawRightAlignedText(x + 90, y + 4, Text_GetString(NULL, 0, sMoveNameTable, runtime->moves[0] - 1));
        }

        if ((runtime->moves[1] != 0) && (runtime->moves[1] < NUMBER_OF_MOVES_IN_GAME)) {
            if (active_pokemon->state.moveSlotIllegality[1] == 1) {
                Gfx_SetEnvColor(0xFF, 0x64, 0xFF, 0xFF); // #FF64FF
            } else {
                Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #FFFFFF
            }
            Font_Printf(x + 124, y + 17, Text_GetString(NULL, 0, sMoveNameTable, runtime->moves[1] - 1));
        }

        if ((runtime->moves[2] != 0) && (runtime->moves[2] < NUMBER_OF_MOVES_IN_GAME)) {
            if (active_pokemon->state.moveSlotIllegality[2] == 1) {
                Gfx_SetEnvColor(0xFF, 0x64, 0xFF, 0xFF); // #FF64FF
            } else {
                Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #FFFFFF
            }
            Font_Printf(x + 110, y + 37, Text_GetString(NULL, 0, sMoveNameTable, runtime->moves[2] - 1));
        }

        if ((runtime->moves[3] != 0) && (runtime->moves[3] < NUMBER_OF_MOVES_IN_GAME)) {
            if (active_pokemon->state.moveSlotIllegality[3] == 1) {
                Gfx_SetEnvColor(0xFF, 0x64, 0xFF, 0xFF); // #FF64FF
            } else {
                Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #FFFFFF
            }
            Battle_DrawRightAlignedText(x + 76, y + 24, Text_GetString(NULL, 0, sMoveNameTable, runtime->moves[3] - 1));
        }
    } else {
        if ((runtime->moves[0] != 0) && (runtime->moves[0] < NUMBER_OF_MOVES_IN_GAME)) {
            if (active_pokemon->state.moveSlotIllegality[0] == 1) {
                Gfx_SetEnvColor(0xFF, 0x64, 0xFF, 0xFF); // #FF64FF
            } else {
                Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #FFFFFF
            }
            Battle_DrawRightAlignedText(x + 90, y + 4, Text_GetString(NULL, 0, sMoveNameTable, runtime->moves[0] - 1));
        }

        if ((runtime->moves[1] != 0) && (runtime->moves[1] < NUMBER_OF_MOVES_IN_GAME)) {
            if (active_pokemon->state.moveSlotIllegality[1] == 1) {
                Gfx_SetEnvColor(0xFF, 0x64, 0xFF, 0xFF); // #FF64FF
            } else {
                Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #FFFFFF
            }
            Font_Printf(x + 124, y + 17, Text_GetString(NULL, 0, sMoveNameTable, runtime->moves[1] - 1));
        }

        if ((runtime->moves[2] != 0) && (runtime->moves[2] < NUMBER_OF_MOVES_IN_GAME)) {
            if (active_pokemon->state.moveSlotIllegality[2] == 1) {
                Gfx_SetEnvColor(0xFF, 0x64, 0xFF, 0xFF); // #FF64FF
            } else {
                Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #FFFFFF
            }
            Font_Printf(x + 110, y + 37, Text_GetString(NULL, 0, sMoveNameTable, runtime->moves[2] - 1));
        }

        if ((runtime->moves[3] != 0) && (runtime->moves[3] < NUMBER_OF_MOVES_IN_GAME)) {
            if (active_pokemon->state.moveSlotIllegality[3] == 1) {
                Gfx_SetEnvColor(0xFF, 0x64, 0xFF, 0xFF); // #FF64FF
            } else {
                Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #FFFFFF
            }
            Battle_DrawRightAlignedText(x + 76, y + 24, Text_GetString(NULL, 0, sMoveNameTable, runtime->moves[3] - 1));
        }
    }

    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF); // #FFFFFF

    if (runtime->moves[0] >= NUMBER_OF_MOVES_IN_GAME) {
        Battle_DrawRightAlignedText(x + 90, y + 4, "？？？？？");
    }

    if (runtime->moves[1] >= NUMBER_OF_MOVES_IN_GAME) {
        Font_Printf(x + 124, y + 17, "？？？？？");
    }

    if (runtime->moves[2] >= NUMBER_OF_MOVES_IN_GAME) {
        Font_Printf(x + 110, y + 37, "？？？？？");
    }

    if (runtime->moves[3] >= NUMBER_OF_MOVES_IN_GAME) {
        Battle_DrawRightAlignedText(x + 76, y + 24, "？？？？？");
    }

    Font_EndTexturedTextRendering();
    BattleHud_DrawTranslucentShadowRect(x - 2, y - 2, 205, 62, 0);
}

void BattleHud_DrawForfeitConfirmDialog(s16 x, s16 y, s16 port, UNUSED s32 arg3) {
    UNUSED s32 pad[2];
    s32 player_color;

    if (Trainer_GetSlots()[port].isHuman & 2) {
        player_color = 4;
    } else {
        player_color = port;
    }

    Battle_DrawGradientRectWrapper(x + 1, y + 1, 198, 40, &sPlayerNameTagStartColor[player_color], &sPlayerNameTagEndColor[player_color]);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);
    // Of note: even the US version has this in Japanese
    // Western Version likely use BattleHud_DrawTwoOptionCursorMenu below
    Font_Printf(x + 46, y + 6, "こうさんしますか?"); // Do you surrender?
    Font_Printf(x + 56, y + 24, "いいえ"); // No
    Font_Printf(x + 129, y + 24, "はい"); // Yes
    Font_EndTexturedTextRendering();

    gSPDisplayList(gDisplayListHead++, gUiMaskedTextureDL);

    Gfx_DrawTextureRgba16Ia8(x + 33, y + 23, 24, 14, gHandCursor, gHandCursorMask, 0x18, 0x100000);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    BattleHud_DrawBoxBorder(x, y, 198, 40);
    BattleHud_DrawTranslucentShadowRect(x - 3, y - 2, 204, 45, 0);
}

void BattleHud_DrawPlainText(s16 x, s16 y, char* fmt) {
    Font_BeginTranslucentTextRendering();
    Font_SetActive(1, 0);
    Font_Printf(x, y, fmt);
    Font_EndTexturedTextRendering();
}

void BattleHud_DrawDoublesRosterRow(ActivePokemon* active_pokemon, TeamRoster* roster, BattleMon* party_pokemon, s32 x, s32 y,
                   s32 party_slot) {
    s32 status_icon;

    BattleHud_DrawLevelNumber(x + 3, y + 4, party_pokemon->level);
    BattleHud_DrawHpBar(x + 30, y + 14, party_pokemon->hp, party_pokemon->maxHp);
    BattleHud_DrawHpFractionText(x + 38, y + 20, party_pokemon->hp, party_pokemon->maxHp);

    gSPDisplayList(gDisplayListHead++, gUiMaskedTextureDL);

    if (party_slot == 0) {
        Gfx_DrawTextureRgba16Ia8(x + 2, y + 0xD, 0x10, 0xE, gButtonCUp, gButtonCUpMask, 0x10, 0x100000);
    }

    if (party_slot == 1) {
        Gfx_DrawTextureRgba16Ia8(x + 2, y + 0xD, 0x10, 0xE, gButtonCLeft, gButtonCLeftMask, 0x10, 0x100000);
    }

    if (party_slot == 2) {
        Gfx_DrawTextureRgba16Ia8(x + 2, y + 0xD, 0x10, 0xC, gButtonB, gButtonBMask, 0x10, 0x100000);
    }

    if (party_slot == 3) {
        Gfx_DrawTextureRgba16Ia8(x + 2, y + 0xD, 0x10, 0xE, gButtonCRight, gButtonCRightMask, 0x10, 0x100000);
    }

    if (party_slot == 4) {
        Gfx_DrawTextureRgba16Ia8(x + 2, y + 0xD, 0x10, 0xE, gButtonCDown, gButtonCDownMask, 0x10, 0x100000);
    }

    if (party_slot == 5) {
        Gfx_DrawTextureRgba16Ia8(x + 2, y + 0xD, 0x10, 0xC, gButtonA, gButtonAMask, 0x10, 0x100000);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    Font_BeginTranslucentTextRendering();
    Font_SetActive(1, 0);
    Font_Printf(x + 0x1D, y + 2, party_pokemon->nickname);
    Font_EndTexturedTextRendering();

    if (roster->pokemonPicked[party_slot].hp == 0) {
        status_icon = 1;
    } else {
        status_icon = Status_ToDisplayIconIndex(roster->pokemonPicked[party_slot].statusCondition);
    }

    if (status_icon != 0) {
        gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

        Gfx_DrawTextureRgba16(x + 0x11, y + 0x14, 0x14, 9, sStatusIcons[status_icon - 1], 0x14, 0);
        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
    }
}

void BattleHud_DrawPartyRosterRow(ActivePokemon* active_pokemon, TeamRoster* roster, BattleMon* party_pokemon, s32 x, s32 y,
                   s32 party_slot) {
    s32 status_icon;

    BattleHud_DrawLevelNumber(x + 3, y + 4, party_pokemon->level);
    BattleHud_DrawHpBar(x + 0x1E, y + 0xE, party_pokemon->hp, party_pokemon->maxHp);
    BattleHud_DrawHpFractionText(x + 0x26, y + 0x14, party_pokemon->hp, party_pokemon->maxHp);

    gSPDisplayList(gDisplayListHead++, gUiMaskedTextureDL);

    if (party_slot == 0) {
        Gfx_DrawTextureRgba16Ia8(x + 2, y + 0xD, 0x10, 0xE, gButtonCUp, gButtonCUpMask, 0x10, 0x100000);
    }

    if (party_slot == 1) {
        Gfx_DrawTextureRgba16Ia8(x + 2, y + 0xD, 0x10, 0xE, gButtonCRight, gButtonCRightMask, 0x10, 0x100000);
    }

    if (party_slot == 2) {
        Gfx_DrawTextureRgba16Ia8(x + 2, y + 0xD, 0x10, 0xE, gButtonCDown, gButtonCDownMask, 0x10, 0x100000);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    Font_BeginTranslucentTextRendering();
    Font_SetActive(1, 0);
    Font_Printf(x + 0x1D, y + 2, party_pokemon->nickname);
    Font_EndTexturedTextRendering();

    if (roster->pokemonPicked[party_slot].hp == 0) {
        status_icon = 1;
    } else {
        status_icon = Status_ToDisplayIconIndex(roster->pokemonPicked[party_slot].statusCondition);
    }

    if (status_icon != 0) {
        gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

        Gfx_DrawTextureRgba16(x + 0x11, y + 0x14, 0x14, 9, sStatusIcons[status_icon - 1], 0x14, 0);

        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
    }
}

void BattleHud_DrawTeamStatusList(ActivePokemon* pokemon, TeamRoster* team_roster, s16 x, s16 y, s32 port) {
    UNUSED s32 pad[2];
    Color_RGB8* dark_colors[] = {
        sPlayer1DarkColor,
        sPlayer2DarkColor,
        sPlayer3DarkColor,
        sPlayer4DarkColor,
    };
    Color_RGB8* light_colors[] = {
        sPlayer1LightColor,
        sPlayer2LightColor,
        sPlayer3LightColor,
        sPlayer4LightColor,
    };
    UNUSED s32 pad2[4];

    Battle_DrawGradientRectWrapper(x + 1, y + 1, 99, 15, &dark_colors[port][0], &light_colors[port][0]);
    Battle_DrawGradientRectWrapper(x + 1, y + 16, 99, 29, &dark_colors[port][1], &light_colors[port][1]);
    Battle_DrawGradientRectWrapper(x + 1, y + 45, 99, 29, &dark_colors[port][2], &light_colors[port][2]);
    Battle_DrawGradientRectWrapper(x + 1, y + 74, 99, 29, &dark_colors[port][3], &light_colors[port][3]);

    BattleHud_DrawPlainText((x - (Font_MeasureTextExtent(1, 0, team_roster->trainerName) / 2)) + 50, y + 3, team_roster->trainerName);

    if (team_roster->numberOfPokemonBrought > 0) {
        BattleHud_DrawPartyRosterRow(pokemon, team_roster, &team_roster->pokemonPicked[0], x + 1, y + 16, 0);
    }

    if (team_roster->numberOfPokemonBrought >= 2) {
        BattleHud_DrawPartyRosterRow(pokemon, team_roster, &team_roster->pokemonPicked[1], x + 1, y + 45, 1);
    }

    if (team_roster->numberOfPokemonBrought >= 3) {
        BattleHud_DrawPartyRosterRow(pokemon, team_roster, &team_roster->pokemonPicked[2], x + 1, y + 74, 2);
    }

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    Gfx_DrawTextureRgba16(x + 1, y, 24, 16, sPlayerLabels[port], 0x18, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    BattleHud_DrawBoxBorder(x, y, 99, 102);
    BattleHud_DrawTranslucentShadowRect(x - 2, y - 2, 105, 106, 0);
}

void BattleHud_DrawDoublesTeamStatusList(ActivePokemon* pokemon, s16 x, s16 y, s32 port) {
    TeamRoster* teammate_roster;
    TeamRoster* self_roster;
    BattleMon* ptr = &pokemon->teamRoster->pokemonPicked[0];
    s32 i;
    Color_RGB8* dark_colors[] = {
        sPlayer1DarkColor,
        sPlayer2DarkColor,
        sPlayer3DarkColor,
        sPlayer4DarkColor,
    };
    Color_RGB8* light_colors[] = {
        sPlayer1LightColor,
        sPlayer2LightColor,
        sPlayer3LightColor,
        sPlayer4LightColor,
    };
    s32 trainer_name_text_width;
    BattleMon* pokemon_in_battle[6];

    teammate_roster = pokemon->sessionTeams->teamMember[pokemon->thumbnail.tagTeam];
    self_roster = pokemon->teamRoster;

    pokemon_in_battle[5] = &self_roster->pokemonPicked[0];
    pokemon_in_battle[5] = &self_roster->pokemonPicked[0];
    pokemon_in_battle[4] = &self_roster->pokemonPicked[1];
    pokemon_in_battle[3] = &self_roster->pokemonPicked[2];
    pokemon_in_battle[2] = &self_roster->pokemonPicked[3];
    pokemon_in_battle[1] = &self_roster->pokemonPicked[4];
    pokemon_in_battle[0] = &self_roster->pokemonPicked[5];

    Battle_DrawGradientRectWrapper(x + 1, y + 1, 200, 15, &dark_colors[port][0], &light_colors[port][0]);
    Battle_DrawGradientRectWrapper(x + 1, y + 16, 200, 29, &dark_colors[port][1], &light_colors[port][1]);
    Battle_DrawGradientRectWrapper(x + 1, y + 45, 200, 29, &dark_colors[port][2], &light_colors[port][2]);
    Battle_DrawGradientRectWrapper(x + 1, y + 74, 200, 29, &dark_colors[port][3], &light_colors[port][3]);
    trainer_name_text_width = Font_MeasureTextExtent(1, 0, teammate_roster->trainerName) / 2;
    BattleHud_DrawPlainText((x - trainer_name_text_width) + 50, y + 3, teammate_roster->trainerName);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    Gfx_DrawTextureRgba16(x + 1, y, 24, 16, *(sPlayerLabels + port), 0x18, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    if (teammate_roster->numberOfPokemonBrought > 0) {
        BattleHud_DrawDoublesRosterRow(pokemon, teammate_roster, pokemon_in_battle[5], x + 1, y + 16, 0);
    }

    if (teammate_roster->numberOfPokemonBrought >= 2) {
        BattleHud_DrawDoublesRosterRow(pokemon, teammate_roster, pokemon_in_battle[4], x + 1, y + 45, 1);
    }

    if (teammate_roster->numberOfPokemonBrought >= 3) {
        BattleHud_DrawDoublesRosterRow(pokemon, teammate_roster, pokemon_in_battle[3], x + 1, y + 74, 2);
    }

    if (teammate_roster->numberOfPokemonBrought >= 4) {
        BattleHud_DrawDoublesRosterRow(pokemon, teammate_roster, pokemon_in_battle[2], x + 102, y + 16, 3);
    }

    if (teammate_roster->numberOfPokemonBrought >= 5) {
        BattleHud_DrawDoublesRosterRow(pokemon, teammate_roster, pokemon_in_battle[1], x + 102, y + 45, 4);
    }

    if (teammate_roster->numberOfPokemonBrought >= 6) {
        BattleHud_DrawDoublesRosterRow(pokemon, teammate_roster, pokemon_in_battle[0], x + 102, y + 74, 5);
    }

    BattleHud_DrawBoxBorder(x, y, 200, 102);
    BattleHud_DrawTranslucentShadowRect(x - 2, y - 2, 206, 106, 0);
}

void BattleHud_DrawCompactMonStatusBox(ActivePokemon* active_pokemon, s16 x, s16 y, s32 port) {
    UNUSED s32 pad;
    Color_RGB8* player_color;
    u32 status_icon;
    ActivePokemonState* state;
    u16 flash_timer;
    BattleMon* party_pokemon;

    party_pokemon = &active_pokemon->sessionTeams->teamMember[active_pokemon->thumbnail.tagTeam]->pokemonPicked[active_pokemon->state.rosterIndex];
    state = &active_pokemon->state;

    if (party_pokemon->hp == 0) {
        status_icon = 1;
    } else {
        status_icon = Status_ToDisplayIconIndex(party_pokemon->statusCondition);
    }

    if (state->flashTimer < 0xFF00) {
        state->flashTimer -= 0x30;
    }

    flash_timer = state->flashTimer;
    if ((flash_timer >> 8) & 1) {
        player_color = &sPlayerNameTagStartColor[port];
    } else {
        player_color = &sPlayerFlashColor[port];
    }

    BattleHud_DrawMonInfoPanel(active_pokemon, party_pokemon, &sStatusLabelConfigs[status_icon], x + 1, y + 1, 70, 40, player_color->r, player_color->g, player_color->b,
                  0xB2);
    BattleHud_DrawBoxBorder(x, y, 70, 40);
    BattleHud_DrawTranslucentShadowRect(x - 2, y - 2, 76, 45, 0);
}

void BattleHud_DrawMessageBox(s32 port, s16 x, s16 y, char* text) {
    Battle_DrawGradientRectWrapper(x + 1, y + 1, 238, 36, &sPlayerNameTagStartColor[port], &sPlayerNameTagEndColor[port]);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);
    Font_SetLineHeight(16);
    Font_Printf(x + 0xA, y + 5, text);
    Font_EndTexturedTextRendering();
    BattleHud_DrawBoxBorder(x, y, 238, 36);
    BattleHud_DrawTranslucentShadowRect(x - 2, y - 1, 244, 41, 0);
}

s32 Battle_DrawTextIfNonEmpty(char* text, s16 x, s16 y) {
    if (*(s8*)text == 0) {
        return 0;
    }
    Font_BeginTranslucentTextRendering();
    Font_SetActive(1, 0);
    Font_Printf(x, y, text);
    Font_EndTexturedTextRendering();
    return 1;
}

void BattleHud_DrawExpandingCaptionBox(BattleBox* battle_box, s16 center_x, s16 center_y, Vec3f corner) {
    s16 current_width;
    s16 current_height;
    Color_RGB8* color;
    ActivePokemon* active_pokemon;
    TeamRoster* roster;

    active_pokemon = sActivePokemon[battle_box->side];
    roster = active_pokemon->sessionTeams->teamMember[active_pokemon->thumbnail.tagTeam];
    color = &sPlayerNameTagStartColor[roster->controllerPort];

    current_width = (center_x - corner.x) * 2;
    current_height = (center_y - corner.y) * 2;

    Gfx_FillRectWithDisplayState(corner.x, corner.y, current_width, current_height, color->r, color->g, color->b, 0x96);
    BattleHud_DrawBoxBorder(corner.x, corner.y, current_width, current_height);
    BattleHud_DrawTranslucentShadowRect(battle_box->textOrigin.x - 2.0f, battle_box->textOrigin.y - 1.0f, 224, 43, 0);
}

void BattleHud_InitCaptionBoxPopup(BattleBox* battle_box, s16 center_x, s16 center_y) {
    Vec3f corner;
    Vec3f center;
    Vec3f full_corner;
    UNUSED s32 pad;

    battle_box->displayTimer = 0;
    battle_box->lineCount = 0;
    Vec3f_SetComponentsDuplicate(&center, center_x, center_y, 0.0f);
    Vec3f_SetComponentsDuplicate(&full_corner, center_x - 110.0f, center_y - 19.0f, 0.0f);
    Vec3f_CalculateDistanceAngles(&center, &full_corner, &battle_box->cornerDistance, &battle_box->rotationX, &battle_box->rotationY);
    Camera_ComputeEyeFromAngles(&center, &battle_box->textOrigin, battle_box->cornerDistance, battle_box->rotationX, battle_box->rotationY);
    battle_box->currentDistance = battle_box->cornerDistance * 0.3f;
    Camera_ComputeEyeFromAngles(&center, &corner, battle_box->currentDistance, battle_box->rotationX, battle_box->rotationY);
    BattleHud_DrawExpandingCaptionBox(battle_box, center_x, center_y, corner);
}

s32 BattleHud_UpdateCaptionBoxPopup(BattleBox* battle_box, s16 center_x, s16 center_y) {
    Vec3f corner;
    Vec3f center;

    Vec3f_SetComponentsDuplicate(&center, center_x, center_y, 0.0f);
    battle_box->currentDistance = Math_StepToF(battle_box->currentDistance, battle_box->cornerDistance, 15.0f, 15.0f);
    Camera_ComputeEyeFromAngles(&center, &corner, battle_box->currentDistance, battle_box->rotationX, battle_box->rotationY);
    BattleHud_DrawExpandingCaptionBox(battle_box, center_x, center_y, corner);

    if (battle_box->currentDistance == battle_box->cornerDistance) {
        return 1;
    }
    return 0;
}

void BattleHud_DrawExpandingCaptionBoxBlack(BattleBox* battle_box, s16 center_x, s16 center_y, Vec3f corner) {
    Gfx_FillRectWithDisplayState(corner.x, corner.y, (s16)((center_x - corner.x) * 2), (s16)((center_y - corner.y) * 2), 0, 0, 0, 0x96);
    BattleHud_DrawBoxBorder(corner.x, corner.y, (s16)((center_x - corner.x) * 2), (s16)((center_y - corner.y) * 2));
    BattleHud_DrawTranslucentShadowRect(battle_box->textOrigin.x - 2.0f, battle_box->textOrigin.y - 1.0f, 0xE0, 0x2B, 0);
}

void BattleHud_InitCaptionBoxPopupBlack(BattleBox* battle_box, s16 center_x, s16 center_y) {
    Vec3f corner;
    Vec3f center;
    Vec3f full_corner;
    UNUSED s32 pad;

    battle_box->displayTimer = 0;
    battle_box->lineCount = 0;
    Vec3f_SetComponentsDuplicate(&center, center_x, center_y, 0.0f);
    Vec3f_SetComponentsDuplicate(&full_corner, center_x - 50.0f, center_y - 10.0f, 0.0f);
    Vec3f_CalculateDistanceAngles(&center, &full_corner, &battle_box->cornerDistance, &battle_box->rotationX, &battle_box->rotationY);
    Camera_ComputeEyeFromAngles(&center, &battle_box->textOrigin, battle_box->cornerDistance, battle_box->rotationX, battle_box->rotationY);
    battle_box->currentDistance = battle_box->cornerDistance * 0.3f;
    Camera_ComputeEyeFromAngles(&center, &corner, battle_box->currentDistance, battle_box->rotationX, battle_box->rotationY);
    BattleHud_DrawExpandingCaptionBoxBlack(battle_box, center_x, center_y, corner);
}

s32 BattleHud_UpdateCaptionBoxPopupBlack(BattleBox* battle_box, s16 center_x, s16 center_y) {
    Vec3f corner;
    Vec3f center;

    Vec3f_SetComponentsDuplicate(&center, center_x, center_y, 0.0f);
    battle_box->currentDistance = Math_StepToF(battle_box->currentDistance, battle_box->cornerDistance, 15.0f, 15.0f);
    Camera_ComputeEyeFromAngles(&center, &corner, battle_box->currentDistance, battle_box->rotationX, battle_box->rotationY);
    BattleHud_DrawExpandingCaptionBoxBlack(battle_box, center_x, center_y, corner);

    if (battle_box->currentDistance == battle_box->cornerDistance) {
        return 1;
    }
    return 0;
}

void BattleHud_DrawTextBox(s32 port, s16 x, s16 y, char* text) {
    Battle_DrawGradientRectWrapper(x + 1, y + 1, 200, 36, &sPlayerNameTagStartColor[port], &sPlayerNameTagEndColor[port]);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);
    Font_SetLineHeight(16);
    Font_Printf(x + 10, y + 5, text);
    Font_EndTexturedTextRendering();
    BattleHud_DrawBoxBorder(x, y, 200, 37);
    BattleHud_DrawTranslucentShadowRect(x - 2, y - 1, 205, 40, 0);
}

// International forfeit box
void BattleHud_DrawTwoOptionCursorMenu(ActivePokemon* pokemon, s16 x, s16 y, s32 port) {
    UNUSED s32 pad;
    s32 text_length;
    UNUSED s32 pad2[2];

    Battle_DrawGradientRectWrapper(x + 1, y + 1, 200, 36, &sPlayerNameTagStartColor[port], &sPlayerNameTagEndColor[port]);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);
    text_length = Font_MeasureTextExtent(2, 0, Text_GetString(NULL, 0, sBattleMessagesTable, 0xA1)) / 2;
    Font_Printf((x - text_length) + 97, y + 5, Text_GetString(NULL, 0, sBattleMessagesTable, 0xA1));
    Font_Printf(x + 50, y + 20, Text_GetString(NULL, 0, sBattleMessagesTable, 0xA3));
    Font_Printf(x + 122, y + 20, Text_GetString(NULL, 0, sBattleMessagesTable, 0xA2));
    Font_EndTexturedTextRendering();

    gSPDisplayList(gDisplayListHead++, gUiMaskedTextureDL);

    Gfx_DrawTextureRgba16Ia8(x + (pokemon->state.forfeitCursorHover * 73) + 27, y + 20, 24, 14, &gHandCursor, &gHandCursorMask, 0x18,
                  0x100000);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    BattleHud_DrawBoxBorder(x, y, 200, 37);
    BattleHud_DrawTranslucentShadowRect(x - 2, y - 1, 205, 40, 0);
}

void BattleScene_DrawStripWipeIfActive(void) {
    if (BattleScene_GetStripWipeState() != 0) {
        BattleScene_DrawStripWipe();
    }
}

void Battle_DrawScrollingTextureRect(s16 x, s16 y, s16 width, s16 height, s16 texture_x, s16 texture_y, s16 texture_spread_x, s16 texture_spread_y) {
    Rectangle text_box;

    GfxRect_SetBounds(&text_box, x, y, (x + width) - 1, (y + height) - 1);

    if (GfxRect_ClipToScissor(&text_box) != 0) {
        if (texture_spread_x != 0) {
            texture_x += ((text_box.left - x) * texture_spread_x) >> 5;
        }

        if (texture_spread_y != 0) {
            texture_y += ((text_box.top - y) * texture_spread_y) >> 5;
        }

        text_box.right += 1;
        text_box.bottom += 1;

        gSPTextureRectangle(gDisplayListHead++, text_box.left << 2, text_box.top << 2, text_box.right << 2, text_box.bottom << 2, G_TX_RENDERTILE,
                            texture_x, texture_y, texture_spread_x, texture_spread_y);
    }
}

void Battle_DrawTiledStripSegment(s32 x, s32 y, s32 width, s32 height, u8* texture, UNUSED s32 load_width) {
    gDPLoadTextureTile(gDisplayListHead++, texture, G_IM_FMT_RGBA, G_IM_SIZ_32b, width, 0, 0, 0, width - 1, height - 1, 0,
                       G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                       G_TX_NOLOD);

    Battle_DrawScrollingTextureRect(x, y, width, height, 0, 0, 0x400, 0);
}

/*
    Draws the pokemon Stadium logo in attact mode
*/
void Battle_DrawDemoStadiumLogo(void) {
    s32 i;

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    for (i = 0; i < 48; i++) {
        Battle_DrawTiledStripSegment(30, i + 23, 88, 1, &gStadiumLogo[i * 352], 88);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    BattleHud_DrawTranslucentShadowRect(28, 22, 94, 52, 0);
}

void Battle_DrawGradientTextureRect(u8* texture, s32 x, s32 y, s32 width, s32 height, u8 texture_width, u8 texture_height, u8 tile_left, u8 tile_top, u8 tile_right,
                   u8 tile_bottom, u8 texture_right, u8 texture_left) {
    UNUSED s32 pad;
    DepthBuffer* image = GfxImage_GetCurrent();
    Vtx* vertices = Gfx_AllocDisplayList(sizeof(Vtx) * 4);
    Mtx* projection = Gfx_AllocDisplayList(sizeof(Mtx) * 1);
    Vp* viewport = Gfx_AllocDisplayList(sizeof(Vp));

    Gfx_SetViewportDimensions(viewport, image->width, image->height);

    gSPViewport(gDisplayListHead++, (u32)viewport & 0x1FFFFFFF);

    guOrtho(projection, 0.5f, image->width - 0.5f, image->height - 0.5f, 0.5f, -2.0f, 2.0f, 1.0f);

    Gfx_SetVertexAttributes(&vertices[0], x, y, -1, texture_left << 5, 0, 0xFF, 0xF0, 0, 0xFF); // #FFF000
    Gfx_SetVertexAttributes(&vertices[1], x + width, y, -1, texture_right << 5, 0, 0xFF, 0xF0, 0, 0xFF); // #FFF000
    Gfx_SetVertexAttributes(&vertices[2], x, y + height, -1, texture_left << 5, texture_height << 5, 0xFF, 0, 0x64, 0xFF); // #FF0064
    Gfx_SetVertexAttributes(&vertices[3], x + width, y + height, -1, texture_right << 5, texture_height << 5, 0xFF, 0, 0x64, 0xFF); // #FF0064

    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_2CYCLE);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_PERSP);
    gSPClearGeometryMode(gDisplayListHead++, G_ZBUFFER | G_LIGHTING);
    gDPSetCombineLERP(gDisplayListHead++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, 0, 0, 0, TEXEL0, COMBINED, 0,
                      SHADE, 0, 0, 0, 0, COMBINED);
    gDPSetRenderMode(gDisplayListHead++, G_RM_PASS, G_RM_XLU_SURF2);
    gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 255);
    gDPSetPrimColor(gDisplayListHead++, 0, 0, 255, 255, 255, 255);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_BILERP);
    gSPTexture(gDisplayListHead++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);

    gDPLoadTextureTile(gDisplayListHead++, texture, G_IM_FMT_IA, G_IM_SIZ_8b, texture_width, 0, tile_left, tile_top, tile_right - 1, tile_bottom - 1, 0,
                       G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                       G_TX_NOLOD);

    gSPPerspNormalize(gDisplayListHead++, 0xFFFF);
    gSPMatrix(gDisplayListHead++, (u32)projection & 0x1FFFFFFF, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPMatrix(gDisplayListHead++, (u32)&gIdentityMtx & 0x1FFFFFFF, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPVertex(gDisplayListHead++, (u32)vertices & 0x1FFFFFFF, 4, 0);
    gSP2Triangles(gDisplayListHead++, 0, 2, 3, 0, 0, 3, 1, 0);
    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
    gSPTexture(gDisplayListHead++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF);
}

void Battle_DrawEndBattleBanner(void) {
    sEndBattleBannerPos.x = Math_StepToF(sEndBattleBannerPos.x, 52.0f, 80.0f, 80.0f);
    Battle_DrawGradientTextureRect(gEndBattleBanner, sEndBattleBannerPos.x, sEndBattleBannerPos.y, 0x6C, 0x20, 0xD8, 0x20, 0, 0, 0x6C, 0x20, 0x6B, 0);
    Battle_DrawGradientTextureRect(gEndBattleBanner, (s32)sEndBattleBannerPos.x + 0x6C, sEndBattleBannerPos.y, 0x6C, 0x20, 0xD8, 0x20, 0x6C, 0, 0xD8, 0x20, 0xD7,
                  0x6C);
}

void Battle_DrawDemoControllerWarning(void) {
    static s32 sScrollOffset = 0;

    s32 text_width;
    s16 text_x;

    if (!(Cont_GetControllerBits() & 1)) {
        text_width = Font_MeasureTextExtent(4, 0, Text_GetString(NULL, 0, sBattleControllerDisconnectedMessage, 0));
        text_x = 0x50 - sScrollOffset;

        gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 16, 0, 304, 240);

        Font_BeginTranslucentTextRendering();
        Font_SetActive(4, 0);
        Font_Printf(text_x, 0xB3, Text_GetString(NULL, 0, sBattleControllerDisconnectedMessage, 0));

        text_x += text_width + 0x1E;
        if (text_x < 0x140) {
            Font_Printf(text_x, 0xB3, Text_GetString(NULL, 0, sBattleControllerDisconnectedMessage, 0));
        }
        Font_EndTexturedTextRendering();

        gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, 320, 240);

        sScrollOffset++;
        if (sScrollOffset >= (text_width + 0x50)) {
            sScrollOffset = 0x32;
        }
    }
}

void BattleHud_DrawOwnSideStatus(ActivePokemon* active_pokemon, ActivePokemonState* state) {
    TeamRoster* roster = active_pokemon->sessionTeams->teamMember[active_pokemon->thumbnail.tagTeam];
    UNUSED s32 pad;
    s32 higher_port_member;

    if (gSessionContext.mode != SESSION_MODE_ATTRACT_DEMO_BATTLE) {
        if (state->selectedCommandOption == 3) {
            BattleHud_DrawMonStatusBoxPlayer(active_pokemon, active_pokemon->thumbnail.camera->viewport.x - 2, active_pokemon->thumbnail.camera->viewport.y - 0x3A, state->actionDelayTimer,
                          1);
        } else {
            BattleHud_DrawMonStatusBoxPlayer(active_pokemon, active_pokemon->thumbnail.camera->viewport.x - 2, active_pokemon->thumbnail.camera->viewport.y - 0x3A, state->actionDelayTimer,
                          0);
        }

        if ((state->selectedCommandOption != 3) && (active_pokemon->thumbnail.camera->viewport.x == 0x17)) {
            switch (state->selectedCommandOption) {
                case 2:
                    if (state->showingCommandOptions == 1) {
                        if (active_pokemon->sessionTeams->tagTeamSize == 2) {
                            if (active_pokemon->sessionTeams->teamMember[1]->controllerPort < active_pokemon->sessionTeams->teamMember[0]->controllerPort) {
                                higher_port_member = 0;
                            } else {
                                higher_port_member = 1;
                            }
                            BattleHud_DrawTeamStatusList(active_pokemon, active_pokemon->sessionTeams->teamMember[higher_port_member], 96, 0xF,
                                          active_pokemon->sessionTeams->teamMember[higher_port_member]->controllerPort);
                            BattleHud_DrawTeamStatusList(active_pokemon, active_pokemon->sessionTeams->teamMember[higher_port_member == 0], 198, 0xF,
                                          active_pokemon->sessionTeams->teamMember[higher_port_member == 0]->controllerPort);
                        } else if (active_pokemon->teamRoster->numberOfPokemonBrought < 4) {
                            BattleHud_DrawTeamStatusList(active_pokemon, active_pokemon->sessionTeams->teamMember[0], 96, 0xF, roster->controllerPort);
                        } else {
                            BattleHud_DrawDoublesTeamStatusList(active_pokemon, 96, 0xF, roster->controllerPort);
                        }
                    } else if (state->unk_2D == 0x10) {
                        BattleHud_DrawOneOptionLabelBar(active_pokemon, 96, 0xF, roster->controllerPort);
                    } else {
                        BattleHud_DrawTwoOptionLabelBar(active_pokemon, 96, 0xF, roster->controllerPort);
                    }
                    break;

                case 1:
                    if (state->showingCommandOptions == 1) {
                        BattleHud_DrawMoveSelectMenu(active_pokemon, 96, 0xF, roster->controllerPort);
                    } else {
                        BattleHud_DrawTwoOptionLabelBar(active_pokemon, 96, 0xF, roster->controllerPort);
                    }
                    break;

                case 4:
                    BattleHud_DrawTwoOptionCursorMenu(active_pokemon, 0x5F, 14, roster->controllerPort);
                    break;

                default:
                    BattleHud_DrawThreeOptionLabelBar(active_pokemon, 96, 0xF, roster->controllerPort);
                    break;
            }
        }
    }
}

void BattleHud_DrawOpponentSideStatus(ActivePokemon* active_pokemon, ActivePokemonState* state) {
    TeamRoster* roster = active_pokemon->sessionTeams->teamMember[active_pokemon->thumbnail.tagTeam];
    s32 higher_port_member;

    if (gSessionContext.mode != SESSION_MODE_ATTRACT_DEMO_BATTLE) {
        if (state->selectedCommandOption == 3) {
            BattleHud_DrawMonStatusBoxOpponent(active_pokemon, active_pokemon->thumbnail.camera->viewport.x - 0x20, active_pokemon->thumbnail.camera->viewport.y - 2, state->actionDelayTimer,
                          1);
        } else {
            BattleHud_DrawMonStatusBoxOpponent(active_pokemon, active_pokemon->thumbnail.camera->viewport.x - 0x20, active_pokemon->thumbnail.camera->viewport.y - 2, state->actionDelayTimer,
                          0);
        }

        if ((state->selectedCommandOption != 3) && (active_pokemon->thumbnail.camera->viewport.x == 0x103)) {
            switch (state->selectedCommandOption) {
                case 2:
                    if (state->showingCommandOptions == 1) {
                        if (active_pokemon->sessionTeams->tagTeamSize == 2) {
                            if (active_pokemon->sessionTeams->teamMember[1]->controllerPort < active_pokemon->sessionTeams->teamMember[0]->controllerPort) {
                                higher_port_member = 0;
                            } else {
                                higher_port_member = 1;
                            }
                            BattleHud_DrawTeamStatusList(active_pokemon, active_pokemon->sessionTeams->teamMember[higher_port_member == 0], 0x16, 0x79,
                                          active_pokemon->sessionTeams->teamMember[higher_port_member == 0]->controllerPort);
                            BattleHud_DrawTeamStatusList(active_pokemon, active_pokemon->sessionTeams->teamMember[higher_port_member], 0x7C, 0x79,
                                          active_pokemon->sessionTeams->teamMember[higher_port_member]->controllerPort);
                        } else if (active_pokemon->teamRoster->numberOfPokemonBrought < 4) {
                            BattleHud_DrawTeamStatusList(active_pokemon, active_pokemon->sessionTeams->teamMember[0], 0x7C, 0x79, roster->controllerPort);
                        } else {
                            BattleHud_DrawDoublesTeamStatusList(active_pokemon, 0x18, 0x79, roster->controllerPort);
                        }
                    } else if (state->unk_2D == 0x10) {
                        BattleHud_DrawOneOptionLabelBar(active_pokemon, 0x98, 0xCE, roster->controllerPort);
                    } else {
                        BattleHud_DrawTwoOptionLabelBar(active_pokemon, 0x62, 0xCE, roster->controllerPort);
                    }
                    break;

                case 1:
                    if (state->showingCommandOptions == 1) {
                        BattleHud_DrawMoveSelectMenu(active_pokemon, 0x19, 0xA6, roster->controllerPort);
                    } else {
                        BattleHud_DrawTwoOptionLabelBar(active_pokemon, 0x62, 0xCE, roster->controllerPort);
                    }
                    break;

                case 4:
                    BattleHud_DrawTwoOptionCursorMenu(active_pokemon, 0x17, 0xBB, roster->controllerPort);
                    break;

                default:
                    BattleHud_DrawThreeOptionLabelBar(active_pokemon, 0x18, 0xCE, roster->controllerPort);
                    break;
            }
        }
    }
}
