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

#if defined(VERSION_JP)
// Second halves of the WIN and LOSE labels, and the two halves of each side's DRAW label.
extern u8 D_301B080[];
extern u8 D_301D900[];
extern u8 D_3020180[];
extern u8 D_3022A00[];
extern u8 D_3024300[];
extern u8 D_3025C00[];
extern u8 D_3027500[];
extern u8 D_3028E00[];
#endif

#if defined(VERSION_JP)
// 72x72 WIN/LOSE label, drawn as three 72x24 strips.
static Vtx D_84B16AA0[] = {
    VTX(-36, 36, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),        VTX(-36, 12, 0, 0, 768, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(36, 12, 0, 2304, 768, 0xFF, 0xFF, 0xFF, 0xFF),    VTX(36, 36, 0, 2304, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-36, 12, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),        VTX(-36, -12, 0, 0, 768, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(36, -12, 0, 2304, 768, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(36, 12, 0, 2304, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-36, -12, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-36, -36, 0, 0, 768, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(36, -36, 0, 2304, 768, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(36, -12, 0, 2304, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B16BA0[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(0, 0, 0, TEXEL0, 0, 0, 0, TEXEL0, 0, 0, 0, COMBINED, COMBINED, 0, PRIM_LOD_FRAC, 0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B16AA0, 12, 0),
    gsDPLoadTextureBlock(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 72, 24, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 7, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureBlock(0x0F000D80, G_IM_FMT_RGBA, G_IM_SIZ_16b, 72, 24, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 7, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsDPLoadTextureBlock(0x0F001B00, G_IM_FMT_RGBA, G_IM_SIZ_16b, 72, 24, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 7, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B16CC8[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B16AA0,
    0x05000000, 0x22050000, 0x00000000, 0x08000000, BattlePrepResultBanner_DrawWinOrLoseLabel,
    D_84B16BA0, 0x06000000, 0x01000000, 0x00000000,
};
// 64x50 DRAW label, drawn as two 64x25 strips.
static Vtx D_84B16CE8_jp[] = {
    VTX(-32, 25, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),        VTX(-32, 0, 0, 0, 800, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, 0, 0, 2048, 800, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(32, 25, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, 0, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),         VTX(-32, -25, 0, 0, 800, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -25, 0, 2048, 800, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(32, 0, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B16D68_jp[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(0, 0, 0, TEXEL0, 0, 0, 0, TEXEL0, 0, 0, 0, COMBINED, COMBINED, 0, PRIM_LOD_FRAC, 0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B16CE8_jp, 8, 0),
    gsDPLoadTextureBlock(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 25, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 7, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureBlock(0x0F000C80, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 25, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 7, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B16CFC[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B16CE8_jp,
    0x05000000, 0x22050000, 0x00000000, 0x08000000, BattlePrepResultBanner_DrawTieLabel,
    D_84B16D68_jp, 0x06000000, 0x01000000, 0x00000000,
};
#else
static Vtx D_84B16AA0[] = {
    VTX(-64, 32, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),        VTX(-64, -32, 0, 0, 2048, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, -32, 0, 1024, 2048, 0xFF, 0xFF, 0xFF, 0xFF), VTX(-32, 32, 0, 1024, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, 32, 0, 1024, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(-32, -32, 0, 1024, 2048, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, -32, 0, 2048, 2048, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(0, 32, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 32, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(0, -32, 0, 2048, 2048, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -32, 0, 3072, 2048, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(32, 32, 0, 3072, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, 32, 0, 3072, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(32, -32, 0, 3072, 2048, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(64, -32, 0, 4096, 2048, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(64, 32, 0, 4096, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B16BA0[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(0, 0, 0, TEXEL0, 0, 0, 0, TEXEL0, 0, 0, 0, COMBINED, COMBINED, 0, PRIM_LOD_FRAC, 0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B16AA0, 16, 0),
    gsDPLoadTextureTile(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 0, 0, 31, 63, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureTile(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 32, 0, 63, 63, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsDPLoadTextureTile(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 64, 0, 95, 63, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsDPLoadTextureTile(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 96, 0, 127, 63, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B16CC8[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B16AA0,
    0x05000000, 0x22050000, 0x00000000, 0x08000000, BattlePrepResultBanner_DrawWinOrLoseLabel,
    D_84B16BA0, 0x06000000, 0x01000000,
};
static u32 D_84B16CFC[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B16AA0,
    0x05000000, 0x22050000, 0x00000000, 0x08000000, BattlePrepResultBanner_DrawTieLabel,
    D_84B16BA0, 0x06000000, 0x01000000,
};
#endif
static Vtx D_84B16D30[] = {
    VTX(-16, 16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-16, -16, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(16, -16, 0, 1024, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(16, 16, 0, 1024, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B16D70[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, TEXEL0, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B16D30, 4, 0),
    gsDPLoadTextureBlock_4b(0x0F000000, G_IM_FMT_I, 32, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                            5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B16DD8[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B16D30,
    0x05000000, 0x22050000, 0x00000000, 0x08000000, BattlePrepResultBanner_DrawSmokePuff,
    D_84B16D70, 0x06000000, 0x01000000, 0x00000000,
};
static Vtx D_84B16E10[] = {
    VTX(-16, 16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-16, -16, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(16, -16, 0, 1024, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(16, 16, 0, 1024, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B16E50[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(TEXEL0, 0, ENVIRONMENT, 0, TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B16E10, 4, 0),
#if defined(VERSION_JP)
    gsDPLoadTextureBlock_4b(D_3028E00, G_IM_FMT_I, 32, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, 5,
                            5, G_TX_NOLOD, G_TX_NOLOD),
#else
    gsDPLoadTextureBlock_4b(D_3024800, G_IM_FMT_I, 32, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, 5,
                            5, G_TX_NOLOD, G_TX_NOLOD),
#endif
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B16EB8[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B16E10,
    0x05000000, 0x22050000, 0x00000000, 0x08000000, BattlePrepResultBanner_DrawStarParticle,
    D_84B16E50, 0x06000000, 0x01000000,
};
static Color_RGBA8 D_84B16EEC[] = { {
                                        0xFF,
                                        0x00,
                                        0x00,
                                        0xFF,
                                    },
                                    {
                                        0xE6,
                                        0x64,
                                        0x19,
                                        0xFF,
                                    },
                                    {
                                        0xFF,
                                        0xFF,
                                        0x00,
                                        0xFF,
                                    },
                                    {
                                        0xFF,
                                        0xFF,
                                        0xFF,
                                        0xFF,
                                    },
                                    {
                                        0xFF,
                                        0xFF,
                                        0xFF,
                                        0x80,
                                    } };

/**
 * Geo draw callback (phase 5) for a WIN or LOSE banner: binds D_3018800 ("WIN") when the
 * banner's outcome is nonzero, else D_301C800 ("LOSE"), to segment 0x0F.
 */
#if defined(VERSION_JP)
// JP banners are two halves; each half binds its own part of the label.
s32 BattlePrepResultBanner_DrawWinOrLoseLabel(s32 callback_phase, unk_func_80011B94* node) {
    ResultBanner* result_banner;
    u8* texture;
    s32 half;

    if (callback_phase == 5) {
        result_banner = (ResultBanner*)D_8006F09C->node.callbackArg;
        half = D_8006F09C->effectSlot - result_banner->banner->effectSlot;
        if (result_banner->outcome != 0) {
            if (half == 0) {
                texture = D_3018800;
            } else {
                texture = D_301B080;
            }
        } else if (half == 0) {
            texture = D_301D900;
        } else {
            texture = D_3020180;
        }

        gDPPipeSync(gDisplayListHead++);
        gSPSegment(gDisplayListHead++, 0x0F, (u32)Memmap_GetSegmentVaddr(texture) & 0x1FFFFFFF);
        gSPDisplayList(gDisplayListHead++, node->display_node_header.callbackArg);

        GeoRender_ApplyMaterialState();
    }
    return 0;
}
#else
s32 BattlePrepResultBanner_DrawWinOrLoseLabel(s32 callback_phase, unk_func_80011B94* node) {
    ResultBanner* result_banner;
    u8* texture;

    if (callback_phase == 5) {
        result_banner = (ResultBanner*)D_8006F09C->node.callbackArg;
        if (result_banner->outcome != 0) {
            texture = D_3018800;
        } else {
            texture = D_301C800;
        }

        gDPPipeSync(gDisplayListHead++);
        gSPSegment(gDisplayListHead++, 0x0F, (u32)Memmap_GetSegmentVaddr(texture) & 0x1FFFFFFF);
        gSPDisplayList(gDisplayListHead++, node->display_node_header.callbackArg);

        GeoRender_ApplyMaterialState();
    }
    return 0;
}
#endif

/**
 * Geo draw callback (phase 5) for a DRAW banner: binds D_3020800 ("DRAW") to segment 0x0F.
 */
#if defined(VERSION_JP)
// JP draws a different two-part DRAW label for each side.
s32 BattlePrepResultBanner_DrawTieLabel(s32 callback_phase, unk_func_80011B94* node) {
    ResultBanner* result_banner;
    u8* texture;
    s32 half;

    if (callback_phase == 5) {
        result_banner = (ResultBanner*)D_8006F09C->node.callbackArg;
        half = D_8006F09C->effectSlot - result_banner->banner->effectSlot;
        if (result_banner->isPlayerSide != 0) {
            if (half == 0) {
                texture = D_3027500;
            } else {
                texture = D_3025C00;
            }
        } else if (half == 0) {
            texture = D_3024300;
        } else {
            texture = D_3022A00;
        }

        gDPPipeSync(gDisplayListHead++);
        gSPSegment(gDisplayListHead++, 0x0F, (u32)Memmap_GetSegmentVaddr(texture) & 0x1FFFFFFF);
        gSPDisplayList(gDisplayListHead++, node->display_node_header.callbackArg);

        GeoRender_ApplyMaterialState();
    }
    return 0;
}
#else
s32 BattlePrepResultBanner_DrawTieLabel(s32 callback_phase, unk_func_80011B94* node) {
    UNUSED s32 pad;

    if (callback_phase == 5) {
        gDPPipeSync(gDisplayListHead++);
        gSPSegment(gDisplayListHead++, 0x0F, (u32)Memmap_GetSegmentVaddr(D_3020800) & 0x1FFFFFFF);
        gSPDisplayList(gDisplayListHead++, node->display_node_header.callbackArg);

        GeoRender_ApplyMaterialState();
    }
    return 0;
}
#endif

/**
 * Geo draw callback (phase 5) for a LOSE smoke puff: binds the puff's current smoke cloud frame
 * to segment 0x0F and tints it with the puff's color.
 */
s32 BattlePrepResultBanner_DrawSmokePuff(s32 callback_phase, unk_func_80011B94* node) {
    ResultBannerSmokePuff* smoke_puff;

    if (callback_phase == 5) {
        smoke_puff = (ResultBannerSmokePuff*)D_8006F09C->node.callbackArg;

        gDPPipeSync(gDisplayListHead++);
        gSPSegment(gDisplayListHead++, 0x0F, (u32)smoke_puff->texture & 0x1FFFFFFF);
        gDPSetEnvColor(gDisplayListHead++, smoke_puff->color.r, smoke_puff->color.g, smoke_puff->color.b, 255);
        gSPDisplayList(gDisplayListHead++, node->display_node_header.callbackArg);

        GeoRender_ApplyMaterialState();
    }
    return 0;
}

/**
 * Starts a LOSE smoke puff on a 52x26 ellipse around (center_x, center_y) at angle, at scale 2.0.
 * Its tint starts as red 0x50, green 0x64; UpdateSmokePuff sets the blue each frame.
 */
void BattlePrepResultBanner_InitSmokePuff(ResultBannerSmokePuff* smoke_puff, unk_D_86002F34* geo, s16 center_x, s16 center_y, s16 angle) {
    smoke_puff->timer = 0;
    smoke_puff->angle = angle;
    smoke_puff->color.r = 0x50;
    smoke_puff->color.g = 0x64;
    Model_InitDisplayObject(smoke_puff->displayObject, 0, 0, geo);
    Vec3f_SetComponentsDuplicate(&smoke_puff->displayObject->scale, 2.0f, 2.0f, 2.0f);
    smoke_puff->displayObject->node.callbackArg = smoke_puff;
#if defined(VERSION_JP)
    // JP releases the smoke puffs on a circle instead of a 2:1 ellipse.
    smoke_puff->displayObject->position.x = (COSS(angle) * 26.0f) + center_x;
#else
    smoke_puff->displayObject->position.x = (COSS(angle) * 52.0f) + center_x;
#endif
    smoke_puff->displayObject->position.y = (SINS(angle) * 26.0f) + center_y;
    smoke_puff->displayObject->position.z = -289.0f;
}

/**
 * Animates a LOSE smoke puff for 16 frames: plays the 8-frame smoke cloud animation D_2023240
 * (two frames per image), lowers the blue tint from 0x82 so the puff darkens, and drifts it 2 units
 * per frame along its angle. Then frees the object.
 */
void BattlePrepResultBanner_UpdateSmokePuff(ResultBannerSmokePuff* smoke_puff) {
    s16 unused_frame;
    DisplayObject* unused_object;
    DisplayObject* unused_object_2;

    if (smoke_puff->timer < 0x10) {
        smoke_puff->texture = Memmap_GetSegmentVaddr(((s32*)Memmap_GetSegmentVaddr(D_2023240))[(smoke_puff->timer / 2)]);
        smoke_puff->color.b = ((smoke_puff->timer * -0x5A) / 16) + 0x82;
        smoke_puff->displayObject->position.x += 2.0f * COSS(smoke_puff->angle);
        smoke_puff->displayObject->position.y += 2.0f * SINS(smoke_puff->angle);
        smoke_puff->timer++;
    } else if (smoke_puff->timer == 0x10) {
        BattlePrep_ResetDisplayObject(smoke_puff->displayObject);
        smoke_puff->timer++;
    }
}

/**
 * Geo draw callback (phase 5) for a WIN star (texture D_3024800): tints it with the star's
 * RGBA color.
 */
s32 BattlePrepResultBanner_DrawStarParticle(s32 callback_phase, unk_func_80011B94* node) {
    ResultBannerStar* star;

    if (callback_phase == 5) {
        star = (ResultBannerStar*)D_8006F09C->node.callbackArg;

        gDPPipeSync(gDisplayListHead++);
        gDPSetEnvColor(gDisplayListHead++, star->color.r, star->color.g, star->color.b, star->color.a);
        gSPDisplayList(gDisplayListHead++, node->display_node_header.callbackArg);

        GeoRender_ApplyMaterialState();
    }
    return 0;
}

/**
 * Starts a mode 0 star on a 60x30 ellipse around (center_x, center_y), flying outward at angle.
 * The star occupies stars[n][0]; its trail and flash copies go in stars[n][1-7].
 */
void BattlePrepResultBanner_InitStarParticle(ResultBannerStar* star, unk_D_86002F34* geo, s16 center_x, s16 center_y,
                                             s16 angle) {
    star->timer = 0;
    star->mode = 0;
    star->angle = angle;
    Model_InitDisplayObject(star->displayObject, 2, 0, geo);
    star->displayObject->node.callbackArg = star;
#if defined(VERSION_JP)
    // JP throws the stars on a circle instead of a 2:1 ellipse.
    star->displayObject->position.x = (COSS(angle) * 30.0f) + center_x;
#else
    star->displayObject->position.x = (COSS(angle) * 60.0f) + center_x;
#endif
    star->displayObject->position.y = (SINS(angle) * 30.0f) + center_y;
    star->displayObject->position.z = -289.0f;
}

/**
 * Spawns a mode 1 trail copy of star into trail (same color, model, position and scale).
 */
void BattlePrepResultBanner_SpawnTrailCopy(ResultBannerStar* trail, ResultBannerStar* star) {
    trail->timer = 0;
    trail->mode = 1;
    trail->color = star->color;
    Model_InitDisplayObject(trail->displayObject, 1, 0, &star->displayObject->node.children->node);
    trail->displayObject->node.callbackArg = trail;
    trail->displayObject->position = star->displayObject->position;
    trail->displayObject->scale = star->displayObject->scale;
}

/**
 * Spawns a mode 2 white flash copy of star into flash at half scale, heading at star's angle plus
 * angle_offset.
 */
void BattlePrepResultBanner_SpawnFlashCopy(ResultBannerStar* flash, ResultBannerStar* star, s16 angle_offset) {
    flash->timer = 0;
    flash->mode = 2;
    flash->angle = star->angle + angle_offset;

    Color_SetRGBA(&flash->color, 0xFF, 0xFF, 0xFF, 0xFF);
    Model_InitDisplayObject(flash->displayObject, 1, 0, &star->displayObject->node.children->node);
    flash->displayObject->node.callbackArg = flash;
    flash->displayObject->position = star->displayObject->position;
    Vec3f_SetComponentsDuplicate(&flash->displayObject->scale, 0.5f, 0.5f, 0.5f);
}

/**
 * Spawns a mode 3 scatter star at a random offset (up to 25 x, 20 y) from origin, pale yellow with
 * a random blue component, at scale 0.6.
 */
void BattlePrepResultBanner_SpawnScatterParticle(ResultBannerStar* star, unk_D_86002F34* geo, Vec3f* origin) {
    star->timer = 0;
    star->mode = 3;
    Color_SetRGBA(&star->color, 0xFF, 0xFF, 0xC8, 0xFF);
    star->color.b = (MathUtil_Random16() % 200) + 0x32;
    Model_InitDisplayObject(star->displayObject, 2, 0, geo);
    Vec3f_SetComponentsDuplicate(&star->displayObject->scale, 0.6f, 0.6f, 0.6f);
    star->displayObject->node.callbackArg = star;
    star->displayObject->position.x = ((MathUtil_Random_ZeroOne() - 0.5f) * 50.0f) + origin->x;
    star->displayObject->position.y = ((MathUtil_Random_ZeroOne() - 0.5f) * 40.0f) + origin->y;
    star->displayObject->position.z = origin->z;
}

/**
 * Star mode 0: for 16 frames the star flies outward (sinking slightly), grows from scale 0.3 and
 * ramps its color red, orange, yellow, white, then translucent white (D_84B16EEC). Every 4 frames
 * it leaves a trail copy in the next of stars[n][1-4]. On frame 16 it bursts into three white
 * flash copies (stars[n][5-7]) at its angle -0x6500, +0 and +0x6500, and frees itself.
 */
void BattlePrepResultBanner_UpdateStarFlyOutStep(ResultBannerStar* star) {
    f32 scale;
    Color_RGBA8* ramp_start;
    Color_RGBA8* ramp_end;

    if (star->timer < 0x10) {
        scale = ((star->timer * 0.5f) / 12.0f) + 0.3f;

        ramp_start = &D_84B16EEC[star->timer / 4];
        ramp_end = &D_84B16EEC[star->timer / 4 + 1];

        star->color.r = (((ramp_end->r - ramp_start->r) * (star->timer % 4)) / 4) + ramp_start->r;
        star->color.g = (((ramp_end->g - ramp_start->g) * (star->timer % 4)) / 4) + ramp_start->g;
        star->color.b = (((ramp_end->b - ramp_start->b) * (star->timer % 4)) / 4) + ramp_start->b;
        star->color.a = (((ramp_end->a - ramp_start->a) * (star->timer % 4)) / 4) + ramp_start->a;

        star->displayObject->position.x += 3.0f * COSS(star->angle);
        star->displayObject->position.y += (3.0f * SINS(star->angle)) - (star->timer * 0.0625f);
        Vec3f_SetComponentsDuplicate(&star->displayObject->scale, scale, scale, scale);
        if (!(star->timer & 3)) {
            BattlePrepResultBanner_SpawnTrailCopy(&star[star->timer / 4 + 1], star);
        }
        star->timer++;
    } else if (star->timer == 0x10) {
        BattlePrepResultBanner_SpawnFlashCopy(&star[5], &star[0], -0x6500);
        BattlePrepResultBanner_SpawnFlashCopy(&star[6], &star[0], 0);
        BattlePrepResultBanner_SpawnFlashCopy(&star[7], &star[0], 0x6500);
        BattlePrep_ResetDisplayObject(star->displayObject);
        star->timer = -1;
    }
}

/**
 * Star mode 1: holds a trail copy for 8 frames, shrinks it by 0.8x per frame until frame 16, then
 * frees it.
 */
void BattlePrepResultBanner_UpdateTrailFadeStep(ResultBannerStar* star) {
    star->timer++;
    if ((star->timer >= 8) && (star->timer < 0x10)) {
        Vec3f_SetComponentsDuplicate(&star->displayObject->scale, star->displayObject->scale.x * 0.8f, star->displayObject->scale.x * 0.8f,
                                     star->displayObject->scale.x * 0.8f);
    } else if (star->timer == 0x10) {
        BattlePrep_ResetDisplayObject(star->displayObject);
        star->timer = -1;
    }
}

/**
 * Star mode 2: moves a flash copy outward at a slowing speed and shrinks it by 0.9x per frame for
 * 16 frames, then frees it.
 */
void BattlePrepResultBanner_UpdateFlashFadeStep(ResultBannerStar* star) {
    f32 scale;
    f32 speed;

    if (star->timer < 0x10) {
        scale = star->displayObject->scale.x * 0.9f;
        speed = (20.0f - star->timer) / 5.0f;
        star->displayObject->position.x += speed * COSS(star->angle);
        star->displayObject->position.y += speed * SINS(star->angle);
        Vec3f_SetComponentsDuplicate(&star->displayObject->scale, scale, scale, scale);
        star->timer++;
    } else if (star->timer == 0x10) {
        BattlePrep_ResetDisplayObject(star->displayObject);
        star->timer = -1;
    }
}

/**
 * Star mode 3: for 20 frames a scatter star shrinks, fades (alpha -5 per frame) and falls 2 units
 * per frame, then frees it.
 */
void BattlePrepResultBanner_UpdateScatterFallStep(ResultBannerStar* star) {
    if (star->timer < 0x14) {
        Vec3f_SetComponentsDuplicate(&star->displayObject->scale, star->displayObject->scale.x - 0.025f,
                                     star->displayObject->scale.x - 0.025f, star->displayObject->scale.x - 0.025f);
        star->color.a -= 5;
        star->displayObject->position.y -= 2.0f;
        star->timer++;
    } else if (star->timer == 0x14) {
        BattlePrep_ResetDisplayObject(star->displayObject);
        star->timer = -1;
    }
}

/**
 * Updates a star by its mode: 0 fly out, 1 trail, 2 flash, 3 scatter.
 */
void BattlePrepResultBanner_UpdateStarParticle(ResultBannerStar* star) {
    switch (star->mode) {
        case 0:
            BattlePrepResultBanner_UpdateStarFlyOutStep(star);
            break;

        case 1:
            BattlePrepResultBanner_UpdateTrailFadeStep(star);
            break;

        case 2:
            BattlePrepResultBanner_UpdateFlashFadeStep(star);
            break;

        case 3:
            BattlePrepResultBanner_UpdateScatterFallStep(star);
            break;
    }
}

/**
 * Banner state 1: runs the burst thrown when the banner lands (stars for WIN, smoke puffs for
 * LOSE) for 36 frames, then returns to idle state 0.
 */
void BattlePrepResultBanner_UpdateParticleBurst(ResultBanner* result_banner) {
    s32 i;
    s32 j;

    result_banner->timer++;
    if (result_banner->timer < 0x25) {
        if (result_banner->outcome != 0) {
            for (i = 0; i < 10; i++) {
                for (j = 0; j < 8; j++) {
                    if (result_banner->stars[i][j].timer >= 0) {
                        BattlePrepResultBanner_UpdateStarParticle(&result_banner->stars[i][j]);
                    }
                }
            }
        } else {
            for (i = 0; i < 10; i++) {
                if (result_banner->smokePuffs[i].timer >= 0) {
                    BattlePrepResultBanner_UpdateSmokePuff(&result_banner->smokePuffs[i]);
                }
            }
        }
    } else {
        result_banner->timer = 0;
        result_banner->state = 0;
    }
}

/**
 * Banner state 2: the WIN or LOSE banner swings in. It turns visible, moves 8 units per frame
 * toward the camera and turns a quarter circle (rotationZ 0x400 per frame) on a 70.71-unit arc,
 * around (82, -2) for the player's banner and (-82, -98) for the opponent's. On frame 16 a WIN
 * banner throws 10 stars and a LOSE banner releases 10 smoke puffs around itself; then state 1.
 */
#if defined(VERSION_JP)
// JP swings the two banner halves in side by side; the first five particles come from banner[0]
// and the rest from banner[1].
void BattlePrepResultBanner_UpdateFlightArc(ResultBanner* result_banner) {
    s16 i;
    s16 angle;

    result_banner->timer++;
    if (result_banner->timer > 0) {
        result_banner->banner[0].node.flags |= 1;
        result_banner->banner[1].node.flags |= 1;
        result_banner->banner[0].position.z -= 8.0f;
        result_banner->banner[1].position.z -= 8.0f;
        if (result_banner->isPlayerSide != 0) {
            result_banner->banner[0].rotation.z += 0x400;
            result_banner->banner[1].rotation.z += 0x400;
            result_banner->banner[0].position.x = (COSS(result_banner->banner[0].rotation.z + 0x6000) * 70.71f) + 46.0f;
            result_banner->banner[0].position.y = (SINS(result_banner->banner[0].rotation.z + 0x6000) * 70.71f) + -2.0f;
            result_banner->banner[1].position.x = (COSS(result_banner->banner[1].rotation.z + 0x6000) * 70.71f) + 118.0f;
            result_banner->banner[1].position.y = (SINS(result_banner->banner[1].rotation.z + 0x6000) * 70.71f) + -2.0f;
        } else {
            result_banner->banner[0].rotation.z -= 0x400;
            result_banner->banner[1].rotation.z -= 0x400;
            result_banner->banner[0].position.x = (COSS(result_banner->banner[0].rotation.z + 0x2000) * 70.71f) + -118.0f;
            result_banner->banner[0].position.y = (SINS(result_banner->banner[0].rotation.z + 0x2000) * 70.71f) + -98.0f;
            result_banner->banner[1].position.x = (COSS(result_banner->banner[1].rotation.z + 0x2000) * 70.71f) + -46.0f;
            result_banner->banner[1].position.y = (SINS(result_banner->banner[1].rotation.z + 0x2000) * 70.71f) + -98.0f;
        }

        if (result_banner->timer == 0x10) {
            if (result_banner->outcome != 0) {
                angle = 0x2000;
                for (i = 0; i < 5; i++, angle += 0x3000) {
                    BattlePrepResultBanner_InitStarParticle(&result_banner->stars[i], result_banner->starGeo, result_banner->banner[0].position.x,
                                                            result_banner->banner[0].position.y, angle);
                    BattlePrepResultBanner_UpdateStarParticle(&result_banner->stars[i]);
                }

                angle = 0x6000;
                for (i = 5; i < 10; i++, angle -= 0x3000) {
                    BattlePrepResultBanner_InitStarParticle(&result_banner->stars[i], result_banner->starGeo, result_banner->banner[1].position.x,
                                                            result_banner->banner[1].position.y, angle);
                    BattlePrepResultBanner_UpdateStarParticle(&result_banner->stars[i]);
                }

                result_banner->timer = 0;
                result_banner->state = 1;
            } else {
                angle = 0x3000;
                for (i = 0; i < 5; i++, angle += 0x2800) {
                    BattlePrepResultBanner_InitSmokePuff(&result_banner->smokePuffs[i], result_banner->smokePuffGeo, result_banner->banner[0].position.x,
                                                      result_banner->banner[0].position.y, angle);
                    BattlePrepResultBanner_UpdateSmokePuff(&result_banner->smokePuffs[i]);
                }

                angle = 0x5000;
                for (i = 5; i < 10; i++, angle -= 0x2800) {
                    BattlePrepResultBanner_InitSmokePuff(&result_banner->smokePuffs[i], result_banner->smokePuffGeo, result_banner->banner[1].position.x,
                                                      result_banner->banner[1].position.y, angle);
                    BattlePrepResultBanner_UpdateSmokePuff(&result_banner->smokePuffs[i]);
                }

                result_banner->timer = 0;
                result_banner->state = 1;
            }
        }
    }
}
#else
void BattlePrepResultBanner_UpdateFlightArc(ResultBanner* result_banner) {
    s16 i;
    s16 angle;

    result_banner->timer++;
    if (result_banner->timer > 0) {
        result_banner->banner->node.flags |= 1;
        result_banner->banner->position.z -= 8.0f;
        if (result_banner->isPlayerSide != 0) {
            result_banner->banner->rotation.z += 0x400;
            result_banner->banner->position.x = (COSS(result_banner->banner->rotation.z + 0x6000) * 70.71f) + 82.0f;
            result_banner->banner->position.y = (SINS(result_banner->banner->rotation.z + 0x6000) * 70.71f) + -2.0f;
        } else {
            result_banner->banner->rotation.z -= 0x400;
            result_banner->banner->position.x = (COSS(result_banner->banner->rotation.z + 0x2000) * 70.71f) + -82.0f;
            result_banner->banner->position.y = (SINS(result_banner->banner->rotation.z + 0x2000) * 70.71f) + -98.0f;
        }

        if (result_banner->timer == 0x10) {
            if (result_banner->outcome != 0) {
                angle = 0x2000;
                for (i = 0; i < 5; i++, angle += 0x3000) {
                    BattlePrepResultBanner_InitStarParticle(&result_banner->stars[i], result_banner->starGeo, result_banner->banner->position.x,
                                                            result_banner->banner->position.y, angle);
                    BattlePrepResultBanner_UpdateStarParticle(&result_banner->stars[i]);
                }

                angle = 0x6000;
                for (i = 5; i < 10; i++, angle -= 0x3000) {
                    BattlePrepResultBanner_InitStarParticle(&result_banner->stars[i], result_banner->starGeo, result_banner->banner->position.x,
                                                            result_banner->banner->position.y, angle);
                    BattlePrepResultBanner_UpdateStarParticle(&result_banner->stars[i]);
                }

                result_banner->timer = 0;
                result_banner->state = 1;
            } else {
                angle = 0x3000;
                for (i = 0; i < 5; i++, angle += 0x2800) {
                    BattlePrepResultBanner_InitSmokePuff(&result_banner->smokePuffs[i], result_banner->smokePuffGeo, result_banner->banner->position.x,
                                                      result_banner->banner->position.y, angle);
                    BattlePrepResultBanner_UpdateSmokePuff(&result_banner->smokePuffs[i]);
                }

                angle = 0x5000;
                for (i = 5; i < 10; i++, angle -= 0x2800) {
                    BattlePrepResultBanner_InitSmokePuff(&result_banner->smokePuffs[i], result_banner->smokePuffGeo, result_banner->banner->position.x,
                                                      result_banner->banner->position.y, angle);
                    BattlePrepResultBanner_UpdateSmokePuff(&result_banner->smokePuffs[i]);
                }

                result_banner->timer = 0;
                result_banner->state = 1;
            }
        }
    }
}
#endif

/**
 * Banner state 3: the player's WIN banner is thrown off screen. After 4 frames it flies left 20
 * units per frame on a gravity arc (velocityY from 10, -0.5 per frame) while spinning, and sheds
 * 8 scatter stars per frame for 10 frames. It is freed on frame 34 and the banner returns to
 * state 0.
 */
#if defined(VERSION_JP)
// JP throws the two banner halves apart, each shedding half of the scatter stars.
void BattlePrepResultBanner_UpdateStarThrowExit(ResultBanner* result_banner) {
    s32 i;
    s32 j;

    result_banner->timer++;
    if (result_banner->timer >= 4) {
        result_banner->banner[0].position.y += result_banner->velocityY;
        result_banner->banner[1].position.y += result_banner->velocityY;
        result_banner->velocityY -= 0.5f;
        result_banner->banner[0].position.x -= 20.0f;
        result_banner->banner[1].position.x -= 16.0f;
        result_banner->banner[0].rotation.z += 0x400;
        result_banner->banner[1].rotation.z -= 0x300;

        if (result_banner->timer < 0xE) {
            for (i = 0; i < 4; i++) {
                BattlePrepResultBanner_SpawnScatterParticle(&result_banner->stars[result_banner->timer - 4][0 + i], result_banner->starGeo,
                                                         &result_banner->banner[0].position);
                BattlePrepResultBanner_SpawnScatterParticle(&result_banner->stars[result_banner->timer - 4][4 + i], result_banner->starGeo,
                                                         &result_banner->banner[1].position);
            }
        }

        for (j = 0; j < 10; j++) {
            for (i = 0; i < 8; i++) {
                if (result_banner->stars[j][i].timer >= 0) {
                    BattlePrepResultBanner_UpdateStarParticle(&result_banner->stars[j][i]);
                }
            }
        }

        if (result_banner->timer == 0x22) {
            BattlePrep_ResetDisplayObject(&result_banner->banner[0]);
            BattlePrep_ResetDisplayObject(&result_banner->banner[1]);
            result_banner->state = 0;
        }
    }
}
#else
void BattlePrepResultBanner_UpdateStarThrowExit(ResultBanner* result_banner) {
    s32 i;
    s32 j;

    result_banner->timer++;
    if (result_banner->timer >= 4) {
        result_banner->banner->position.y += result_banner->velocityY;
        result_banner->velocityY -= 0.5f;
        result_banner->banner->position.x -= 20.0f;
        result_banner->banner->rotation.z += 0x400;

        if (result_banner->timer < 0xE) {
            for (i = 0; i < 4; i++) {
                BattlePrepResultBanner_SpawnScatterParticle(&result_banner->stars[result_banner->timer - 4][0 + i], result_banner->starGeo,
                                                         &result_banner->banner->position);
                BattlePrepResultBanner_SpawnScatterParticle(&result_banner->stars[result_banner->timer - 4][4 + i], result_banner->starGeo,
                                                         &result_banner->banner->position);
            }
        }

        for (j = 0; j < 10; j++) {
            for (i = 0; i < 8; i++) {
                if (result_banner->stars[j][i].timer >= 0) {
                    BattlePrepResultBanner_UpdateStarParticle(&result_banner->stars[j][i]);
                }
            }
        }

        if (result_banner->timer == 0x22) {
            BattlePrep_ResetDisplayObject(result_banner->banner);
            result_banner->state = 0;
        }
    }
}
#endif

/**
 * Banner state 4: the banner drops off screen. After 4 frames it moves by velocityY, which falls
 * by 1 per frame. A WIN banner also slides left 10 units per frame while spinning; a LOSE banner
 * spins the other way in place. It is freed on frame 32 and the banner returns to state 0.
 */
#if defined(VERSION_JP)
// JP banners are two halves (banner[0] and banner[1]) that fall and spin apart.
void BattlePrepResultBanner_UpdateFallExit(ResultBanner* result_banner) {
    result_banner->timer++;
    if (result_banner->timer >= 4) {
        result_banner->banner[0].position.y += result_banner->velocityY;
        result_banner->banner[1].position.y += result_banner->velocityY;
        result_banner->velocityY -= 1.0f;
        if (result_banner->outcome != 0) {
            result_banner->banner[0].position.x -= 10.0f;
            result_banner->banner[1].position.x -= 8.0f;
            result_banner->banner[0].rotation.z += 0x400;
            result_banner->banner[1].rotation.z -= 0x300;
        } else {
            result_banner->banner[0].rotation.z -= 0x400;
            result_banner->banner[1].rotation.z += 0x300;
        }

        if (result_banner->timer == 0x20) {
            BattlePrep_ResetDisplayObject(&result_banner->banner[0]);
            BattlePrep_ResetDisplayObject(&result_banner->banner[1]);
            result_banner->state = 0;
        }
    }
}
#else
void BattlePrepResultBanner_UpdateFallExit(ResultBanner* result_banner) {
    result_banner->timer++;
    if (result_banner->timer >= 4) {
        result_banner->banner->position.y += result_banner->velocityY;
        result_banner->velocityY -= 1.0f;
        if (result_banner->outcome != 0) {
            result_banner->banner->position.x -= 10.0f;
            result_banner->banner->rotation.z += 0x400;
        } else {
            result_banner->banner->rotation.z -= 0x400;
        }

        if (result_banner->timer == 0x20) {
            BattlePrep_ResetDisplayObject(result_banner->banner);
            result_banner->state = 0;
        }
    }
}
#endif

/**
 * Banner state 5: the DRAW banner slides in. The player's banner moves right 40 units per frame
 * for 7 frames, from x = -280 to the center; the opponent's banner stays off screen at x = -372.
 * Returns to state 0 on frame 44.
 */
#if defined(VERSION_JP)
// JP slides the two banner halves in one after the other, then lets them drift.
void BattlePrepResultBanner_UpdateDrawSlideIn(ResultBanner* result_banner) {
    if (result_banner->timer < 0x14) {
        if (result_banner->isPlayerSide != 0) {
            if (result_banner->timer < 7) {
                result_banner->banner[0].position.x += 40.0f;
            }
            if (result_banner->timer < 9) {
                result_banner->banner[1].position.x += 40.0f;
            }
        } else {
            if (result_banner->timer < 0xB) {
                result_banner->banner[0].position.x += 40.0f;
            }
            if (result_banner->timer < 0xD) {
                result_banner->banner[1].position.x += 40.0f;
            }
        }
    } else if (result_banner->isPlayerSide != 0) {
        result_banner->banner[1].position.x -= 2.0f;
    } else {
        result_banner->banner[0].position.x -= 4.0f;
        result_banner->banner[1].position.x -= 6.0f;
    }

    result_banner->timer++;
    if (result_banner->timer == 0x2C) {
        result_banner->timer = 0;
        result_banner->state = 0;
    }
}
#else
void BattlePrepResultBanner_UpdateDrawSlideIn(ResultBanner* result_banner) {
    if ((result_banner->timer < 0x14) && (result_banner->isPlayerSide != 0) && (result_banner->timer < 7)) {
        result_banner->banner->position.x += 40.0f;
    }

    result_banner->timer++;
    if (result_banner->timer == 0x2C) {
        result_banner->timer = 0;
        result_banner->state = 0;
    }
}
#endif

/**
 * Banner state 7: the player's DRAW banner slides off to the right, 40 units per frame, and the
 * banner returns to state 0 after 11 frames.
 */
#if defined(VERSION_JP)
// JP slides both banners off, each half two frames after the previous one.
void BattlePrepResultBanner_UpdateDrawExit(ResultBanner* result_banner) {
    if (result_banner->isPlayerSide != 0) {
        if (result_banner->timer >= 0) {
            result_banner->banner[0].position.x += 40.0f;
        }
        if (result_banner->timer >= 2) {
            result_banner->banner[1].position.x += 40.0f;
        }
    } else {
        if (result_banner->timer >= 4) {
            result_banner->banner[0].position.x += 40.0f;
        }
        if (result_banner->timer >= 6) {
            result_banner->banner[1].position.x += 40.0f;
        }
    }

    result_banner->timer++;
    if (result_banner->timer == 0xB) {
        result_banner->timer = 0;
        result_banner->state = 0;
    }
}
#else
void BattlePrepResultBanner_UpdateDrawExit(ResultBanner* result_banner) {
    s32 unused_timer; // FAKE: written in both branches and never read

    if (result_banner->isPlayerSide != 0) {
        if (result_banner->timer >= 0) {
            result_banner->banner->position.x += 40.0f;
        }
        unused_timer = result_banner->timer;
    } else {
        unused_timer = result_banner->timer;
    }

    result_banner->timer++;
    if (result_banner->timer == 0xB) {
        result_banner->timer = 0;
        result_banner->state = 0;
    }
}
#endif

/**
 * Per-frame update for a result banner: 1 particle burst, 2 flight arc, 3 star throw exit,
 * 4 fall exit, 5 DRAW slide in, 7 DRAW exit.
 */
void BattlePrepResultBanner_Update(ResultBanner* result_banner) {
    switch (result_banner->state) {
        case 1:
            BattlePrepResultBanner_UpdateParticleBurst(result_banner);
            break;

        case 2:
            BattlePrepResultBanner_UpdateFlightArc(result_banner);
            break;

        case 3:
            BattlePrepResultBanner_UpdateStarThrowExit(result_banner);
            break;

        case 4:
            BattlePrepResultBanner_UpdateFallExit(result_banner);
            break;

        case 5:
            BattlePrepResultBanner_UpdateDrawSlideIn(result_banner);
            break;

        case 7:
            BattlePrepResultBanner_UpdateDrawExit(result_banner);
            break;
    }
}

/**
 * Shows the banner in banner_object and hands it the particle objects starting at particle_objects
 * (10 smoke puffs, then 10x8 stars). A DRAW banner starts the state 5 slide in from x = -280 (player)
 * or -372 (opponent). A WIN or LOSE banner starts hidden at (32, 48) for the player or (-32, -48) for
 * the opponent and enters the state 2 flight arc; LOSE starts 6 frames later. Both banners of a
 * result get the same particle objects; only a WIN uses stars and only a LOSE uses smoke puffs.
 */
#if defined(VERSION_JP)
// JP launches the two banner halves (banner_object[0] and [1]) side by side.
void BattlePrepResultBanner_Launch(ResultBanner* result_banner, DisplayObject* banner_object, DisplayObject* particle_objects) {
    s32 i;
    s32 j;

    result_banner->banner = banner_object;
    result_banner->particleObjects = particle_objects;
    Model_InitDisplayObject(banner_object, 1, 0, result_banner->bannerGeo);
    Model_InitDisplayObject(&result_banner->banner[1], 1, 0, result_banner->bannerGeo);
    result_banner->banner[0].node.callbackArg = result_banner;
    result_banner->banner[1].node.callbackArg = result_banner;

    if (result_banner->outcome == -1) {
        result_banner->timer = 0;
        result_banner->state = 5;
        if (result_banner->isPlayerSide != 0) {
            Vec3f_SetComponentsDuplicate(&result_banner->banner[0].position, -196.0f, 0.0f, -289.0f);
            Vec3f_SetComponentsDuplicate(&result_banner->banner[1].position, -284.0f, 0.0f, -289.0f);
        } else {
            Vec3f_SetComponentsDuplicate(&result_banner->banner[0].position, -372.0f, 0.0f, -289.0f);
            Vec3f_SetComponentsDuplicate(&result_banner->banner[1].position, -460.0f, 0.0f, -289.0f);
        }
        return;
    }

    if (result_banner->outcome != 0) {
        result_banner->timer = 0;
    } else {
        result_banner->timer = -6;
    }

    result_banner->state = 2;
    result_banner->banner[0].node.flags &= ~1;
    result_banner->banner[1].node.flags &= ~1;
    if (result_banner->isPlayerSide != 0) {
        Vec3f_SetComponentsDuplicate(&result_banner->banner[0].position, -4.0f, 48.0f, -161.0f);
        Vec3f_SetComponentsDuplicate(&result_banner->banner[1].position, 68.0f, 48.0f, -161.0f);
        result_banner->banner[0].rotation.z = -0x4000;
        result_banner->banner[1].rotation.z = -0x4000;
    } else {
        Vec3f_SetComponentsDuplicate(&result_banner->banner[0].position, -68.0f, -48.0f, -161.0f);
        Vec3f_SetComponentsDuplicate(&result_banner->banner[1].position, 4.0f, -48.0f, -161.0f);
        result_banner->banner[0].rotation.z = 0x4000;
        result_banner->banner[1].rotation.z = 0x4000;
    }

    for (i = 0; i < 10; i++) {
        result_banner->smokePuffs[i].displayObject = particle_objects++;
    }

    for (i = 0; i < 10; i++) {
        for (j = 0; j < 8; j++) {
            result_banner->stars[i][j].displayObject = particle_objects++;
            result_banner->stars[i][j].timer = -1;
        }
    }
}
#else
void BattlePrepResultBanner_Launch(ResultBanner* result_banner, DisplayObject* banner_object, DisplayObject* particle_objects) {
    s32 i;
    s32 j;

    result_banner->banner = banner_object;
    result_banner->particleObjects = particle_objects;
    Model_InitDisplayObject(banner_object, 1, 0, result_banner->bannerGeo);
    result_banner->banner->node.callbackArg = result_banner;

    if (result_banner->outcome == -1) {
        result_banner->timer = 0;
        result_banner->state = 5;
        if (result_banner->isPlayerSide != 0) {
            Vec3f_SetComponentsDuplicate(&result_banner->banner->position, -280.0f, 0.0f, -289.0f);
        } else {
            Vec3f_SetComponentsDuplicate(&result_banner->banner->position, -372.0f, 0.0f, -289.0f);
        }
        return;
    }

    if (result_banner->outcome != 0) {
        result_banner->timer = 0;
    } else {
        result_banner->timer = -6;
    }

    result_banner->state = 2;
    result_banner->banner->node.flags &= ~1;
    if (result_banner->isPlayerSide != 0) {
        Vec3f_SetComponentsDuplicate(&result_banner->banner->position, 32.0f, 48.0f, -161.0f);
        result_banner->banner->rotation.z = -0x4000;
    } else {
        Vec3f_SetComponentsDuplicate(&result_banner->banner->position, -32.0f, -48.0f, -161.0f);
        result_banner->banner->rotation.z = 0x4000;
    }

    for (i = 0; i < 10; i++) {
        result_banner->smokePuffs[i].displayObject = particle_objects++;
    }

    for (i = 0; i < 10; i++) {
        for (j = 0; j < 8; j++) {
            result_banner->stars[i][j].displayObject = particle_objects++;
            result_banner->stars[i][j].timer = -1;
        }
    }
}
#endif

/**
 * Starts the banner exit: state 7 for DRAW, the state 3 star throw for the player's WIN banner,
 * otherwise the state 4 fall with initial velocityY 20 (opponent's WIN) or -10 (LOSE).
 */
void BattlePrepResultBanner_StartExit(ResultBanner* result_banner) {
    result_banner->timer = 0;
    if (result_banner->outcome == -1) {
        result_banner->state = 7;
    } else if (result_banner->isPlayerSide != 0) {
        if (result_banner->outcome != 0) {
            result_banner->velocityY = 10.0f;
            result_banner->state = 3;
        } else {
            result_banner->velocityY = -10.0f;
            result_banner->state = 4;
        }
    } else {
        if (result_banner->outcome != 0) {
            result_banner->velocityY = 20.0f;
        } else {
            result_banner->velocityY = -10.0f;
        }
        result_banner->state = 4;
    }
}

/**
 * Resets the banner and stores its side (is_player_side) and outcome (-1 DRAW, 0 LOSE, nonzero
 * WIN). Builds the banner geo layout (D_84B16CFC with the DRAW texture, else D_84B16CC8 for WIN or
 * LOSE) plus the smoke puff and star layouts. BattlePrep_InitResultScene calls it for both sides.
 */
void BattlePrepResultBanner_Init(ResultBanner* result_banner, s8 is_player_side, s8 outcome) {
    GraphNode* unused_node;
    MemoryBlock* memory_block;

    result_banner->state = 0;
    result_banner->timer = 0;
    result_banner->isPlayerSide = is_player_side;
    result_banner->outcome = outcome;

    memory_block = MainPool_AllocState(main_pool_get_available(), 0);
    if (outcome == -1) {
        result_banner->bannerGeo = process_geo_layout(memory_block, D_84B16CFC);
    } else {
        result_banner->bannerGeo = process_geo_layout(memory_block, D_84B16CC8);
    }
    result_banner->smokePuffGeo = process_geo_layout(memory_block, D_84B16DD8);
    result_banner->starGeo = process_geo_layout(memory_block, D_84B16EB8);
    MainPool_FinalizeAllocation(memory_block);
}
