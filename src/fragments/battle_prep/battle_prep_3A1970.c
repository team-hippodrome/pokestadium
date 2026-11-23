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

static Vtx D_84B16F00[] = {
    VTX(-66, 14, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-66, -2, 0, 0, 512, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(50, -2, 0, 3712, 512, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(50, 14, 0, 3712, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(50, 14, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(50, -2, 0, 0, 512, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(66, -2, 0, 512, 512, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(66, 14, 0, 512, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-66, -2, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-66, -14, 0, 0, 384, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(50, -14, 0, 3712, 384, 0xFF, 0xFF, 0xFF, 0xFF), VTX(50, -2, 0, 3712, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(50, -2, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(50, -14, 0, 0, 384, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(66, -14, 0, 512, 384, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(66, -2, 0, 512, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-64, 10, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-64, -10, 0, 0, 640, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, -10, 0, 2048, 640, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(0, 10, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 10, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(0, -10, 0, 2048, 640, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(64, -10, 0, 4096, 640, 0xFF, 0xFF, 0xFF, 0xFF), VTX(64, 10, 0, 4096, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B17080[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, TEXEL0, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B16F00, 24, 0),
    gsDPSetEnvColor(30, 115, 115, 255),
    gsDPLoadTextureBlock(D_3028030, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 12, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureBlock(D_30280F0, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 12, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsDPLoadTextureBlock(D_3027EB0, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 12, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsDPLoadTextureBlock(D_3027F70, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 12, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),
    gsDPPipeSync(),
    gsDPSetEnvColor(220, 220, 30, 255),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 128, 0x03028870),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadTile(G_TX_LOADTILE, 0, 0, 0x00FC, 0x004C),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(G_TX_RENDERTILE, 0, 0, 0x00FC, 0x004C),
    gsSP2Triangles(16, 17, 18, 0, 16, 18, 19, 0),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 128, 0x03028870),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadTile(G_TX_LOADTILE, 0x0100, 0, 0x01FC, 0x004C),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(G_TX_RENDERTILE, 0x0100, 0, 0x01FC, 0x004C),
    gsSP2Triangles(20, 21, 22, 0, 20, 22, 23, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B17240[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B16F00,
    0x05000000, 0x22050000, D_84B17080, 0x06000000, 0x01000000,
};
static Vtx D_84B17268[] = {
    VTX(-40, 16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-40, -4, 0, 0, 640, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(24, -4, 0, 2048, 640, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(24, 16, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(24, 16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(24, -4, 0, 0, 640, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(40, -4, 0, 512, 640, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(40, 16, 0, 512, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-40, -4, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-40, -16, 0, 0, 384, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(24, -16, 0, 2048, 384, 0xFF, 0xFF, 0xFF, 0xFF), VTX(24, -4, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(24, -4, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(24, -16, 0, 0, 384, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(40, -16, 0, 512, 384, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(40, -4, 0, 512, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-36, 12, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-36, -12, 0, 0, 768, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(36, -12, 0, 2304, 768, 0xFF, 0xFF, 0xFF, 0xFF), VTX(36, 12, 0, 2304, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B173A8[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, TEXEL0, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B17268, 20, 0),
    gsDPSetEnvColor(30, 115, 115, 255),
    gsDPLoadTextureBlock(D_3028030, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 12, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureBlock(D_30280F0, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 12, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsDPLoadTextureBlock(D_3027EB0, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 12, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsDPLoadTextureBlock(D_3027F70, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 12, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),
    gsDPPipeSync(),
    gsDPSetEnvColor(220, 220, 30, 255),
    gsDPLoadTextureBlock(D_30281B0, G_IM_FMT_IA, G_IM_SIZ_8b, 72, 24, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(16, 17, 18, 0, 16, 18, 19, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B17528[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B17268,
    0x05000000, 0x22050000, D_84B173A8, 0x06000000, 0x01000000,
};

/**
 * Result label state 1: holds the label at its current size. Nothing in battle_prep sets state 1.
 */
void BattlePrepResultLabel_HoldStep(ResultLabel* result_label) {
}

/**
 * Result label state 2: grows the label 1/8 per frame; after 8 frames it is at full size and the
 * label returns to idle state 0 (still shown).
 */
void BattlePrepResultLabel_GrowStep(ResultLabel* result_label) {
    result_label->scaleStep++;
    if (result_label->scaleStep == 8) {
        result_label->state = 0;
    }
    Vec3f_SetComponentsDuplicate(&result_label->displayObject->scale, result_label->scaleStep * 0.125f, result_label->scaleStep * 0.125f,
                                 result_label->scaleStep * 0.125f);
}

/**
 * Result label state 3: shrinks the label 1/8 per frame; at zero it clears the object and returns
 * to state 0.
 */
void BattlePrepResultLabel_ShrinkStep(ResultLabel* result_label) {
    result_label->scaleStep--;
    if (result_label->scaleStep == 0) {
        result_label->state = 0;
        ModelRenderer_ClearDisplayObject(result_label->displayObject);
    } else {
        Vec3f_SetComponentsDuplicate(&result_label->displayObject->scale, result_label->scaleStep * 0.125f, result_label->scaleStep * 0.125f,
                                     result_label->scaleStep * 0.125f);
    }
}

/**
 * Per-frame update for a result label: 1 hold, 2 grow, 3 shrink.
 */
void BattlePrepResultLabel_Update(ResultLabel* result_label) {
    switch ((s8)result_label->state) {
        case 1:
            BattlePrepResultLabel_HoldStep(result_label);
            break;

        case 2:
            BattlePrepResultLabel_GrowStep(result_label);
            break;

        case 3:
            BattlePrepResultLabel_ShrinkStep(result_label);
            break;
    }
}

/**
 * Pops a result label in: builds it in display_object at scale 0.1 and starts the state 2 grow.
 * PERFECT is placed at (32, 26) and EXTRA CONTINUE at (-96, 80).
 */
void BattlePrepResultLabel_StartGrow(ResultLabel* result_label, DisplayObject* display_object) {
    result_label->displayObject = display_object;
    result_label->scaleStep = 0;
    result_label->state = 2;
    Model_InitDisplayObject(display_object, 1, 0, result_label->geo);
    result_label->displayObject->node.callbackArg = result_label;
    if (result_label->kind == 0) {
        Vec3f_SetComponentsDuplicate(&result_label->displayObject->position, 32.0f, 26.0f, -288.0f);
    } else {
        Vec3f_SetComponentsDuplicate(&result_label->displayObject->position, -96.0f, 80.0f, -288.0f);
    }
    Vec3f_SetComponentsDuplicate(&result_label->displayObject->scale, 0.1f, 0.1f, 0.1f);
}

/**
 * Starts shrinking a result label out of view (state 3, from full size).
 */
void BattlePrepResultLabel_StartShrink(ResultLabel* result_label) {
    result_label->scaleStep = 8;
    result_label->state = 3;
}

/**
 * Resets a result label and builds its geo layout: kind 0 = PERFECT (D_3028870), kind 1 = EXTRA
 * CONTINUE (D_30281B0, tinted yellow, with four small 16x12 pieces tinted teal).
 */
void BattlePrepResultLabel_Init(ResultLabel* result_label, u8 kind) {
    MemoryBlock* memory_block;

    result_label->state = 0;
    result_label->scaleStep = 0;
    result_label->kind = kind;

    memory_block = MainPool_AllocState(main_pool_get_available(), 0);
    if (kind == 0) {
        result_label->geo = process_geo_layout(memory_block, D_84B17240);
    } else {
        result_label->geo = process_geo_layout(memory_block, D_84B17528);
    }
    MainPool_FinalizeAllocation(memory_block);
}
