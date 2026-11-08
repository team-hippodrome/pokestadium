#include "demo_scenes.h"
#include "src/graphics_textures.h"
#include "src/gfx_buffer.h"
#include "src/gfx_rect.h"

/**
 * Draws an IA8 texture tile (uls..lrs, ult..lrt of a width x height image) as a screen-space quad at
 * (x, y, w, h) using an orthographic projection. Cycle 1 tints the texel between the env and prim colors
 * of tile_params->unk_00; cycle 2 multiplies by per-vertex shade colors, giving a vertical gradient
 * between unk_00.unk_0C..unk_10 and unk_00.unk_12..unk_16 (flip_gradient != 0 swaps top and bottom).
 * Called by the completion scene //@src/fragments/demo_scenes/demo_scenes_161E60.c
 */
void UI_DrawSplitGradientPanel(u8* texture, s32 x, s32 y, s32 w, s32 h, u8 width, u8 height, u8 uls, u8 ult,
                               u8 lrs, u8 lrt, u8 argB, u8 argC, GfxTileParams* tile_params, s32 flip_gradient) {
    DepthBuffer* temp_s1 = GfxImage_GetCurrent();
    Vtx* temp_s6 = Gfx_AllocDisplayList(sizeof(Vtx) * 4);
    Mtx* spD4 = Gfx_AllocDisplayList(sizeof(Mtx) * 1);
    Vp* temp_v0 = Gfx_AllocDisplayList(sizeof(Vp) * 1);

    Gfx_SetViewportDimensions(temp_v0, temp_s1->width, temp_s1->height);

    gSPViewport(gDisplayListHead++, (u32)temp_v0 & 0x1FFFFFFF);

    guOrtho(spD4, 0.5f, temp_s1->width - 0.5f, temp_s1->height - 0.5f, 0.5f, -2.0f, 2.0f, 1.0f);
    if (flip_gradient != 0) {
        Gfx_SetVertexAttributes(&temp_s6[0], x, y, -1, argC << 5, 0, tile_params->unk_00.unk_12, tile_params->unk_00.unk_14,
                                tile_params->unk_00.unk_16, 0xFF);
        Gfx_SetVertexAttributes(&temp_s6[1], x + w, y, -1, argB << 5, 0, tile_params->unk_00.unk_12,
                                tile_params->unk_00.unk_14, tile_params->unk_00.unk_16, 0xFF);
        Gfx_SetVertexAttributes(&temp_s6[2], x, y + h, -1, argC << 5, height << 5, tile_params->unk_00.unk_0C,
                                tile_params->unk_00.unk_0E, tile_params->unk_00.unk_10, 0xFF);
        Gfx_SetVertexAttributes(&temp_s6[3], x + w, y + h, -1, argB << 5, height << 5, tile_params->unk_00.unk_0C,
                                tile_params->unk_00.unk_0E, tile_params->unk_00.unk_10, 0xFF);
    } else {
        Gfx_SetVertexAttributes(&temp_s6[0], x, y, -1, argC << 5, 0, tile_params->unk_00.unk_0C, tile_params->unk_00.unk_0E,
                                tile_params->unk_00.unk_10, 0xFF);
        Gfx_SetVertexAttributes(&temp_s6[1], x + w, y, -1, argB << 5, 0, tile_params->unk_00.unk_0C,
                                tile_params->unk_00.unk_0E, tile_params->unk_00.unk_10, 0xFF);
        Gfx_SetVertexAttributes(&temp_s6[2], x, y + h, -1, argC << 5, height << 5, tile_params->unk_00.unk_12,
                                tile_params->unk_00.unk_14, tile_params->unk_00.unk_16, 0xFF);
        Gfx_SetVertexAttributes(&temp_s6[3], x + w, y + h, -1, argB << 5, height << 5, tile_params->unk_00.unk_12,
                                tile_params->unk_00.unk_14, tile_params->unk_00.unk_16, 0xFF);
    }

    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_2CYCLE);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_PERSP);
    gSPClearGeometryMode(gDisplayListHead++, G_ZBUFFER | G_LIGHTING);
    gDPSetCombineLERP(gDisplayListHead++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, 0, 0, 0, TEXEL0, COMBINED, 0,
                      SHADE, 0, 0, 0, 0, COMBINED);
    gDPSetRenderMode(gDisplayListHead++, G_RM_PASS, G_RM_XLU_SURF2);
    gDPSetEnvColor(gDisplayListHead++, tile_params->unk_00.unk_06, tile_params->unk_00.unk_08, tile_params->unk_00.unk_0A, 255);
    gDPSetPrimColor(gDisplayListHead++, 0, 0, tile_params->unk_00.unk_00, tile_params->unk_00.unk_02, tile_params->unk_00.unk_04, 255);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_BILERP);
    gSPTexture(gDisplayListHead++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);

    gDPLoadTextureTile(gDisplayListHead++, texture, G_IM_FMT_IA, G_IM_SIZ_8b, width, height, uls, ult, lrs - 1, lrt - 1, 0,
                       G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                       G_TX_NOLOD);

    gSPPerspNormalize(gDisplayListHead++, 0xFFFF);
    gSPMatrix(gDisplayListHead++, (u32)spD4 & 0x1FFFFFFF, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPMatrix(gDisplayListHead++, (u32)&gIdentityMtx & 0x1FFFFFFF, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPVertex(gDisplayListHead++, (u32)temp_s6 & 0x1FFFFFFF, 4, 0);
    gSP2Triangles(gDisplayListHead++, 0, 2, 3, 0, 0, 3, 1, 0);
    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
    gSPTexture(gDisplayListHead++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF);
}

/**
 * Draws an untextured screen-space quad (x, y, w, h) whose four vertices are black with alpha arg4,
 * using translucent blending. Used as a darkening overlay //@src/fragments/demo_scenes/demo_scenes_161E60.c
 */
void UI_DrawAlphaQuad(s32 x, s32 y, s32 width, s32 height, s32 alpha) {
    DepthBuffer* temp_s1 = GfxImage_GetCurrent();
    Vtx* temp_s2 = Gfx_AllocDisplayList(sizeof(Vtx) * 4);
    Mtx* sp84 = Gfx_AllocDisplayList(sizeof(Mtx) * 1);
    Vp* sp80 = Gfx_AllocDisplayList(sizeof(Vp) * 1);

    Gfx_SetViewportDimensions(sp80, temp_s1->width, temp_s1->height);

    gSPViewport(gDisplayListHead++, (u32)sp80 & 0x1FFFFFFF);

    guOrtho(sp84, 0.5f, temp_s1->width - 0.5f, temp_s1->height - 0.5f, 0.5f, -2.0f, 2.0f, 1.0f);

    Gfx_SetVertexAttributes(&temp_s2[0], x, y, -1, 0, 0, 0, 0, 0, alpha);
    Gfx_SetVertexAttributes(&temp_s2[1], x + width, y, -1, 0, 0, 0, 0, 0, alpha);
    Gfx_SetVertexAttributes(&temp_s2[2], x, y + height, -1, 0, 0, 0, 0, 0, alpha);
    Gfx_SetVertexAttributes(&temp_s2[3], x + width, y + height, -1, 0, 0, 0, 0, 0, alpha);

    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gSPClearGeometryMode(gDisplayListHead++, G_ZBUFFER | G_LIGHTING);
    gDPSetCombineMode(gDisplayListHead++, G_CC_SHADE, G_CC_SHADE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gSPPerspNormalize(gDisplayListHead++, 0xFFFF);

    gSPMatrix(gDisplayListHead++, (u32)sp84 & 0x1FFFFFFF, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPMatrix(gDisplayListHead++, (u32)&gIdentityMtx & 0x1FFFFFFF, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPVertex(gDisplayListHead++, (u32)temp_s2 & 0x1FFFFFFF, 4, 0);
    gSP2Triangles(gDisplayListHead++, 0, 2, 3, 0, 0, 3, 1, 0);
}
