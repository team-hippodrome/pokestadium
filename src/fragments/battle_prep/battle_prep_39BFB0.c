#include "battle_prep.h"
#include "src/geo_render.h"
#include "src/display_object_textures.h"
#include "src/model_renderer.h"
#include "src/ui_graphics.h"
#include "src/audio_commands_category2.h"
#include "src/gfx_rect.h"
#include "src/matrix.h"
#include "src/math_util.h"
#include "src/memmap.h"
#include "src/memory.h"

static s8 D_84B14C10[] = {
    0xFF, 0x00, 0x01, 0x02, 0x06, 0x07, 0x08, 0xFF, 0xFF, 0x03, 0x04, 0x05, 0x09, 0x0A, 0x0B, 0xFF,
};
static Vtx D_84B14C20[] = {
    VTX(-32, 48, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-32, 16, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, 16, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(32, 48, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, 16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-32, -16, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -16, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(32, 16, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, -16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-32, -48, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -48, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(32, -16, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B14CE0[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(0, 0, 0, TEXEL0, 0, 0, 0, SHADE, 0, 0, 0, COMBINED, COMBINED, 0, PRIM_LOD_FRAC, 0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B14C20, 12, 0),
    gsDPLoadTextureBlock(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureBlock(0x0F001000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsDPLoadTextureBlock(0x0F002000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B14DC8[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B14C20,
    0x05000000, 0x22050000, 0x00000000, 0x08000000, BattlePrepTeamIcons_DrawTrainerCard,
    D_84B14CE0, 0x06000000, 0x01000000, 0x00000000,
};
static Vtx D_84B14E00[] = {
    VTX(-32, 48, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-32, 16, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, 16, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(32, 48, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, 16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-32, -16, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -16, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(32, 16, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, -16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-32, -48, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -48, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(32, -16, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Vtx D_84B14EC0[] = {
    VTX(-30, 45, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-30, 30, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 30, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(0, 45, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 45, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(0, 30, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(30, 30, 0, 4096, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(30, 45, 0, 4096, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Vtx D_84B14F40[] = {
    VTX(-30, 30, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-30, 15, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 15, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(0, 30, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 30, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(0, 15, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(30, 15, 0, 4096, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(30, 30, 0, 4096, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Vtx D_84B14FC0[] = {
    VTX(-30, 15, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(-30, 0, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 0, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(0, 15, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 15, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),    VTX(0, 0, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(30, 0, 0, 4096, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(30, 15, 0, 4096, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Vtx D_84B15040[] = {
    VTX(-30, 0, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),        VTX(-30, -15, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, -15, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(0, 0, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 0, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(0, -15, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(30, -15, 0, 4096, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(30, 0, 0, 4096, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Vtx D_84B150C0[] = {
    VTX(-30, -15, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-30, -30, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, -30, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(0, -15, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, -15, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(0, -30, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(30, -30, 0, 4096, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(30, -15, 0, 4096, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Vtx D_84B15140[] = {
    VTX(-30, -30, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-30, -45, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, -45, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(0, -30, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, -30, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(0, -45, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(30, -45, 0, 4096, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(30, -30, 0, 4096, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Vtx D_84B151C0[] = {
    VTX(-30, 30, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-30, -30, 0, 0, 2048, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(30, -30, 0, 2048, 2048, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(30, 30, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B15200[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(0, 0, 0, TEXEL0, 0, 0, 0, TEXEL0, 0, 0, 0, COMBINED, COMBINED, 0, PRIM_LOD_FRAC, 0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B14E00, 12, 0),
    gsDPLoadTextureBlock(0x0E000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureBlock(0x0E001000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsDPLoadTextureBlock(0x0E002000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsSPVertex(D_84B14EC0, 8, 0),
    gsDPLoadTextureTile(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 0, 0, 63, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureTile(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 64, 0, 127, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSPVertex(D_84B14F40, 8, 0),
    gsDPLoadTextureTile(0x0F002000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 0, 0, 63, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureTile(0x0F002000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 64, 0, 127, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSPVertex(D_84B14FC0, 8, 0),
    gsDPLoadTextureTile(0x0F004000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 0, 0, 63, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureTile(0x0F004000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 64, 0, 127, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSPVertex(D_84B15040, 8, 0),
    gsDPLoadTextureTile(0x0F006000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 0, 0, 63, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureTile(0x0F006000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 64, 0, 127, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSPVertex(D_84B150C0, 8, 0),
    gsDPLoadTextureTile(0x0F008000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 0, 0, 63, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureTile(0x0F008000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 64, 0, 127, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSPVertex(D_84B15140, 8, 0),
    gsDPLoadTextureTile(0x0F00A000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 0, 0, 63, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureTile(0x0F00A000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 64, 0, 127, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSPEndDisplayList(),
};
static Gfx D_84B15618[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(TEXEL0, 0, ENVIRONMENT, 0, TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, COMBINED, COMBINED, 0,
                       PRIM_LOD_FRAC, 0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B151C0, 4, 0),
    gsDPSetEnvColor(255, 50, 20, 160), // #FF3214A0
    gsDPLoadTextureBlock_4b(D_3017F00, G_IM_FMT_I, 64, 64, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                            G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B15688[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B14E00,
    0x05000000, 0x22050000, 0x00000000, 0x08000000, BattlePrepTeamIcons_DrawIconCardSingle,
    D_84B15200, 0x22050000, D_84B15618, 0x25000000, 0x06000000,
    0x01000000,
};
static Vtx D_84B156C8[] = {
    VTX(-32, 48, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-32, 16, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, 16, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(32, 48, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, 16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-32, -16, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -16, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(32, 16, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, -16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-32, -48, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -48, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(32, -16, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-20, 30, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-20, -10, 0, 0, 1280, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(20, -10, 0, 1280, 1280, 0xFF, 0xFF, 0xFF, 0xFF), VTX(20, 30, 0, 1280, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-24, -20, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-24, -44, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(24, -44, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(24, -20, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Vtx D_84B15808[] = {
    VTX(-20, 30, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-20, -10, 0, 0, 2048, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(20, -10, 0, 2048, 2048, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(20, 30, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B15848[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(0, 0, 0, TEXEL0, 0, 0, 0, TEXEL0, 0, 0, 0, COMBINED, COMBINED, 0, PRIM_LOD_FRAC, 0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B156C8, 20, 0),
    gsDPLoadTextureBlock(0x0E000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureBlock(0x0E001000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsDPLoadTextureBlock(0x0E002000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsDPLoadTextureBlock(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 40, 40, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),
    gsDPLoadTextureBlock(D_3016F00, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(16, 17, 18, 0, 16, 18, 19, 0),
    gsSPEndDisplayList(),
};
static Gfx D_84B159B0[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(TEXEL0, 0, ENVIRONMENT, 0, TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, COMBINED, COMBINED, 0,
                       PRIM_LOD_FRAC, 0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B15808, 4, 0),
    gsDPSetEnvColor(255, 50, 20, 160), // #FF3214A0
    gsDPLoadTextureBlock_4b(D_3017F00, G_IM_FMT_I, 64, 64, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                            G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B15A20[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B156C8,
    0x05000000, 0x22050000, 0x00000000, 0x08000000, BattlePrepTeamIcons_DrawFaintedFaceSingle,
    D_84B15848, 0x22050000, D_84B159B0, 0x25000000, 0x06000000,
    0x01000000,
};
static Vtx D_84B15A60[] = {
    VTX(-32, 24, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-32, 0, 0, 0, 768, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, 0, 0, 2048, 768, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(32, 24, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, 0, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-32, -24, 0, 0, 768, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -24, 0, 2048, 768, 0xFF, 0xFF, 0xFF, 0xFF), VTX(32, 0, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Vtx D_84B15AE0[] = {
    VTX(-30, 22, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(-30, 7, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 7, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(0, 22, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 22, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),    VTX(0, 7, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(30, 7, 0, 4096, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(30, 22, 0, 4096, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Vtx D_84B15B60[] = {
    VTX(-30, 7, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-30, -7, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, -7, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(0, 7, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 7, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(0, -7, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(30, -7, 0, 4096, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(30, 7, 0, 4096, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Vtx D_84B15BE0[] = {
    VTX(-30, -7, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-30, -22, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, -22, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(0, -7, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, -7, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(0, -22, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(30, -22, 0, 4096, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(30, -7, 0, 4096, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Vtx D_84B15C60[] = {
    VTX(-22, 22, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-22, -22, 0, 0, 2048, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(22, -22, 0, 2048, 2048, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(22, 22, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B15CA0[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(0, 0, 0, TEXEL0, 0, 0, 0, TEXEL0, 0, 0, 0, COMBINED, COMBINED, 0, PRIM_LOD_FRAC, 0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B15A60, 8, 0),
    gsDPLoadTextureBlock(0x0E000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 24, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureBlock(0x0E000C00, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 24, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSPVertex(D_84B15AE0, 8, 0),
    gsDPLoadTextureTile(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 0, 0, 63, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureTile(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 64, 0, 127, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSPVertex(D_84B15B60, 8, 0),
    gsDPLoadTextureTile(0x0F002000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 0, 0, 63, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureTile(0x0F002000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 64, 0, 127, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSPVertex(D_84B15BE0, 8, 0),
    gsDPLoadTextureTile(0x0F004000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 0, 0, 63, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureTile(0x0F004000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 64, 0, 127, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSPEndDisplayList(),
};
static Gfx D_84B15EE0[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(TEXEL0, 0, ENVIRONMENT, 0, TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, COMBINED, COMBINED, 0,
                       PRIM_LOD_FRAC, 0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B15C60, 4, 0),
    gsDPSetEnvColor(255, 50, 20, 160), // #FF3214A0
    gsDPLoadTextureBlock_4b(D_3017F00, G_IM_FMT_I, 64, 64, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                            G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B15F50[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B15A60,
    0x05000000, 0x1B000000, 0x00000000, 0x00000000, 0x00180000,
    0x05000000, 0x22050000, 0x00000000, 0x08000000, BattlePrepTeamIcons_DrawIconCardDouble,
    D_84B15CA0, 0x22050000, D_84B15EE0, 0x25000000, 0x06000000,
    0x06000000, 0x01000000,
};
static u32 D_84B15FA8[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B15A60,
    0x05000000, 0x1B000000, 0x00000000, 0x00000000, 0xFFE80000,
    0x05000000, 0x22050000, 0x00000000, 0x08000000, BattlePrepTeamIcons_DrawIconCardDouble,
    D_84B15CA0, 0x22050000, D_84B15EE0, 0x25000000, 0x06000000,
    0x06000000, 0x01000000,
};
static Vtx D_84B16000[] = {
    VTX(-32, 24, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-32, 0, 0, 0, 768, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, 0, 0, 2048, 768, 0xFF, 0xFF, 0xFF, 0xFF),    VTX(32, 24, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, 0, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),        VTX(-32, -24, 0, 0, 768, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -24, 0, 2048, 768, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(32, 0, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-14, 20, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-14, -8, 0, 0, 1280, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(14, -8, 0, 1280, 1280, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(14, 20, 0, 1280, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-12, -8, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-12, -20, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(12, -20, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(12, -8, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Vtx D_84B16100[] = {
    VTX(-14, 20, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-14, -8, 0, 0, 2048, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(14, -8, 0, 2048, 2048, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(14, 20, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B16140[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(0, 0, 0, TEXEL0, 0, 0, 0, TEXEL0, 0, 0, 0, COMBINED, COMBINED, 0, PRIM_LOD_FRAC, 0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B16000, 16, 0),
    gsDPLoadTextureBlock(0x0E000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 24, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureBlock(0x0E000C00, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 24, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsDPLoadTextureBlock(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 40, 40, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsDPLoadTextureBlock(D_3016F00, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),
    gsSPEndDisplayList(),
};
static Gfx D_84B16268[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(TEXEL0, 0, ENVIRONMENT, 0, TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, COMBINED, COMBINED, 0,
                       PRIM_LOD_FRAC, 0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B16100, 4, 0),
    gsDPSetEnvColor(255, 50, 20, 160), // #FF3214A0
    gsDPLoadTextureBlock_4b(D_3017F00, G_IM_FMT_I, 64, 64, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                            G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B162D8[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B16000,
    0x05000000, 0x1B000000, 0x00000000, 0x00000000, 0x00180000,
    0x05000000, 0x22050000, 0x00000000, 0x08000000, BattlePrepTeamIcons_DrawFaintedFaceDouble,
    D_84B16140, 0x22050000, D_84B16268, 0x25000000, 0x06000000,
    0x06000000, 0x01000000,
};
static u32 D_84B16330[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B16000,
    0x05000000, 0x1B000000, 0x00000000, 0x00000000, 0xFFE80000,
    0x05000000, 0x22050000, 0x00000000, 0x08000000, BattlePrepTeamIcons_DrawFaintedFaceDouble,
    D_84B16140, 0x22050000, D_84B16268, 0x25000000, 0x06000000,
    0x06000000, 0x01000000, 0x00000000, 0x00000000,
};

/**
 * Geo callback (draw phase 5) for a trainer card on the result screen. Binds segment 0x0F to side0CardImage for
 * display object 0 (side 0, top-left) or side1CardImage otherwise, draws the display list in the node's
 * callbackArg, and reapplies material state. The card images are composed by BattlePrep_DrawTrainerStatusIcon.
 */
s32 BattlePrepTeamIcons_DrawTrainerCard(s32 phase, unk_func_80011B94* display_node) {
    BattlePrepTeamIcons* team_icons;
    u8* card_image;

    if (phase == 5) {
        team_icons = (BattlePrepTeamIcons*)D_8006F09C->node.callbackArg;
        if (team_icons->displayObjects->effectSlot == D_8006F09C->effectSlot) {
            card_image = team_icons->side0CardImage->img_p;
        } else {
            card_image = team_icons->side1CardImage->img_p;
        }

        gDPPipeSync(gDisplayListHead++);
        gSPSegment(gDisplayListHead++, 0x0F, (u32)card_image & 0x1FFFFFFF);
        gSPDisplayList(gDisplayListHead++, display_node->display_node_header.callbackArg);

        GeoRender_ApplyMaterialState();
    }
    return 0;
}

/**
 * Geo callback (draw phase 5) for a card in the single layout (three per side). Binds segment 0x0F to the slot's
 * PokeIcon image (icon plus HP bar) and segment 0x0E to the tall blue side 0 panel D_3006000 (display objects 1-3)
 * or green side 1 panel D_3009000 (4-6), draws the card, then shows the next sibling, the red X (D_3017F00), only
 * when isFaintMarked is set for the slot.
 */
s32 BattlePrepTeamIcons_DrawIconCardSingle(s32 phase, unk_func_80011B94* display_node) {
    BattlePrepTeamIcons* team_icons;
    s32 slot;
    u8* panel_texture;

    if (phase == 5) {
        team_icons = (BattlePrepTeamIcons*)D_8006F09C->node.callbackArg;
        slot = D_8006F09C->effectSlot - team_icons->displayObjects->effectSlot;
        if (slot < 4) {
            panel_texture = D_3006000;
        } else {
            panel_texture = D_3009000;
        }

        gDPPipeSync(gDisplayListHead++);
        gSPSegment(gDisplayListHead++, 0x0F, (u32)PokeIcon_GetImage(team_icons->iconRenderList, slot - 1) & 0x1FFFFFFF);
        gSPSegment(gDisplayListHead++, 0x0E, (u32)Memmap_GetSegmentVaddr(panel_texture) & 0x1FFFFFFF);
        gSPDisplayList(gDisplayListHead++, display_node->display_node_header.callbackArg);

        GeoRender_ApplyMaterialState();

        if (team_icons->isFaintMarked[slot - 1] != 0) {
            display_node->display_node_header.nextSibling->flags |= 1;
        } else {
            display_node->display_node_header.nextSibling->flags &= ~1;
        }
    }
    return 0;
}

/**
 * Geo callback (draw phase 5) for a fainted face in the single layout (display objects 9-14 during the flip, then
 * 1-6). Binds segment 0x0F to the slot's 40x40 model texture in modelTextures and segment 0x0E to the D_3006000 /
 * D_3009000 panel. The geo layout adds the "FAINTED" label (D_3016F00) and the red X.
 */
s32 BattlePrepTeamIcons_DrawFaintedFaceSingle(s32 phase, unk_func_80011B94* display_node) {
    BattlePrepTeamIcons* team_icons;
    s32 slot;
    u8* panel_texture;

    if (phase == 5) {
        team_icons = (BattlePrepTeamIcons*)D_8006F09C->node.callbackArg;
        slot = (D_8006F09C->effectSlot - team_icons->displayObjects->effectSlot) % 8;
        if (slot < 4) {
            panel_texture = D_3006000;
        } else {
            panel_texture = D_3009000;
        }

        gDPPipeSync(gDisplayListHead++);
        gSPSegment(gDisplayListHead++, 0x0F, (u32)team_icons->modelTextures[slot - 1] & 0x1FFFFFFF);
        gSPSegment(gDisplayListHead++, 0x0E, (u32)Memmap_GetSegmentVaddr(panel_texture) & 0x1FFFFFFF);
        gSPDisplayList(gDisplayListHead++, display_node->display_node_header.callbackArg);

        GeoRender_ApplyMaterialState();
    }
    return 0;
}

/**
 * Geo callback (draw phase 5) for a half-height card in the double layout (six per side). Maps the display object
 * to a slot (objects 1-6 and 9-14 through D_84B14C10, flip objects 16-39 by (index - 16) % 12), binds that slot's
 * PokeIcon image and the blue D_300C000 (slots 0-5, side 0) or green D_300D800 (slots 6-11, side 1) panel, draws,
 * and shows the red X sibling only when isFaintMarked is set.
 */
s32 BattlePrepTeamIcons_DrawIconCardDouble(s32 phase, unk_func_80011B94* display_node) {
    u8* panel_texture;
    BattlePrepTeamIcons* team_icons;
    s32 slot;

    if (phase == 5) {
        team_icons = (BattlePrepTeamIcons*)D_8006F09C->node.callbackArg;
        slot = D_8006F09C->effectSlot - team_icons->displayObjects->effectSlot;

        if (slot >= 0x10) {
            slot = (slot - 0x10) % 12;
        } else {
            slot = D_84B14C10[slot];
        }

        if (slot < 6) {
            panel_texture = D_300C000;
        } else {
            panel_texture = D_300D800;
        }

        gDPPipeSync(gDisplayListHead++);
        gSPSegment(gDisplayListHead++, 0x0F, (u32)PokeIcon_GetImage(team_icons->iconRenderList, slot) & 0x1FFFFFFF);
        gSPSegment(gDisplayListHead++, 0x0E, (u32)Memmap_GetSegmentVaddr(panel_texture) & 0x1FFFFFFF);
        gSPDisplayList(gDisplayListHead++, display_node->display_node_header.callbackArg);

        GeoRender_ApplyMaterialState();

        if (team_icons->isFaintMarked[slot] != 0) {
            display_node->display_node_header.nextSibling->flags |= 1;
        } else {
            display_node->display_node_header.nextSibling->flags &= ~1;
        }
    }
    return 0;
}

/**
 * Double-layout counterpart of BattlePrepTeamIcons_DrawFaintedFaceSingle: uses the slot mapping of
 * BattlePrepTeamIcons_DrawIconCardDouble and binds the slot's model texture and the D_300C000 / D_300D800 panel.
 */
s32 BattlePrepTeamIcons_DrawFaintedFaceDouble(s32 phase, unk_func_80011B94* display_node) {
    s32 slot;
    BattlePrepTeamIcons* team_icons;
    u8* panel_texture;

    if (phase == 5) {
        team_icons = (BattlePrepTeamIcons*)D_8006F09C->node.callbackArg;
        slot = D_8006F09C->effectSlot - team_icons->displayObjects->effectSlot;

        if (slot >= 0x10) {
            slot = (slot - 0x10) % 12;
        } else {
            slot = D_84B14C10[slot];
        }

        if (slot < 6) {
            panel_texture = D_300C000;
        } else {
            panel_texture = D_300D800;
        }

        gDPPipeSync(gDisplayListHead++);
        gSPSegment(gDisplayListHead++, 0x0F, (u32)team_icons->modelTextures[slot] & 0x1FFFFFFF);
        gSPSegment(gDisplayListHead++, 0x0E, (u32)Memmap_GetSegmentVaddr(panel_texture) & 0x1FFFFFFF);
        gSPDisplayList(gDisplayListHead++, display_node->display_node_header.callbackArg);

        GeoRender_ApplyMaterialState();
    }
    return 0;
}

/**
 * Redraws the HP bar into each picked Pokémon's icon image for both sides (side 0 slots 0-2, side 1 slots 3-5). The
 * bar shows max(hp, maxHp * min_hp_percent / 100), so lowering min_hp_percent from 100 drains the bars from full
 * down to each Pokémon's HP at the end of the battle.
 */
void BattlePrepTeamIcons_DrawHpBarsSingle(BattlePrepTeamIcons* team_icons, s32 min_hp_percent) {
    s32 i;
    s32 j;
    UNUSED s32 pad;
    TeamRoster* trainer;
    s32 min_hp_scaled;
    s32 hp_scaled;

    for (i = 0; i < 2; i++) {
        trainer = gSessionContext.battleSides[i].tagTeamMember[0];

        for (j = 0; j < trainer->numberOfPokemonBrought; j++) {
            min_hp_scaled = trainer->pokemonPicked[j].maxHp * min_hp_percent;
            hp_scaled = trainer->pokemonPicked[j].hp * 0x64;
            if (hp_scaled < min_hp_scaled) {
                hp_scaled = min_hp_scaled;
            }
            GfxImage_SetRenderTarget(&gDisplayListHead, team_icons->iconRenderList->unk_08[i * 3 + j].unk_0C);
            BattleHud_DrawHpBarTexture(4, 0xB4, 0x78, hp_scaled, trainer->pokemonPicked[j].maxHp * 0x64);
        }
    }
}

/**
 * Double-layout counterpart of BattlePrepTeamIcons_DrawHpBarsSingle: slot = side * 6 + member * 3 + k, skipping
 * absent tag-team members.
 */
void BattlePrepTeamIcons_DrawHpBarsDouble(BattlePrepTeamIcons* team_icons, s32 min_hp_percent) {
    s32 i;
    s32 j;
    s32 k;
    s32 min_hp_scaled;
    s32 hp_scaled;
    TeamRoster* trainer;

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            trainer = gSessionContext.battleSides[i].tagTeamMember[j];

            if (trainer != NULL) {
                for (k = 0; k < trainer->numberOfPokemonBrought; k++) {
                    min_hp_scaled = trainer->pokemonPicked[k].maxHp * min_hp_percent;
                    hp_scaled = trainer->pokemonPicked[k].hp * 0x64;
                    if (hp_scaled < min_hp_scaled) {
                        hp_scaled = min_hp_scaled;
                    }
                    GfxImage_SetRenderTarget(&gDisplayListHead, team_icons->iconRenderList->unk_08[(i * 6) + (j * 3) + k].unk_0C);
                    BattleHud_DrawHpBarTexture(4, 0x54, 0x78, hp_scaled, trainer->pokemonPicked[k].maxHp * 0x64);
                }
            }
        }
    }
}

/**
 * Sets isFaintMarked, which shows the red X, for each picked Pokémon whose BattleMon unk_25 equals faint_order, and
 * plays sound 0x0110000B per mark with the slot + 1 as its argument. unk_25 behaves as the Pokémon's faint order (0
 * = did not faint): BattlePrepTeamIcons_HpDrainStep* passes 1, 2, 3, ... in turn.
 */
void BattlePrepTeamIcons_MarkFaintedSingle(BattlePrepTeamIcons* team_icons, s32 faint_order) {
    UNUSED s32 pad;
    TeamRoster* trainer;
    s32 slot;
    s32 i;
    s32 j;

    for (i = 0; i < 2; i++) {
        trainer = gSessionContext.battleSides[i].tagTeamMember[0];

        for (j = 0; j < trainer->numberOfPokemonBrought; j++) {
            if (faint_order == trainer->pokemonPicked[j].unk_25) {
                slot = (i * 3) + j;
                team_icons->isFaintMarked[slot] = 1;
                Audio_PlayCategory11SoundCommand(0x0110000B, slot + 1, 0);
            }
        }
    }
}

/**
 * Double-layout counterpart of BattlePrepTeamIcons_MarkFaintedSingle (slot = side * 6 + member * 3 + k).
 */
void BattlePrepTeamIcons_MarkFaintedDouble(BattlePrepTeamIcons* team_icons, s32 faint_order) {
    s32 i;
    s32 j;
    s32 k;
    TeamRoster* trainer;
    s32 slot;

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            trainer = gSessionContext.battleSides[i].tagTeamMember[j];

            if (trainer != NULL) {
                for (k = 0; k < trainer->numberOfPokemonBrought; k++) {
                    if (faint_order == trainer->pokemonPicked[k].unk_25) {
                        slot = (i * 6) + (j * 3) + k;
                        team_icons->isFaintMarked[slot] = 1;
                        Audio_PlayCategory11SoundCommand(0x0110000B, slot + 1, 1);
                    }
                }
            }
        }
    }
}

/**
 * Single layout: binds cards 1-6 to iconGeo and lays them out three per row, 64 units apart at z = -289 (side 0 at
 * y = +48 right of its trainer card, side 1 at y = -48 left of its card). They start turned 0x5800 (top) / -0x5800
 * (bottom) about Y for BattlePrepTeamIcons_FlipInStepSingle.
 */
void BattlePrepTeamIcons_InitIconCardsSingle(BattlePrepTeamIcons* team_icons) {
    s32 i;

    for (i = 1; i < 7; i++) {
        Model_InitDisplayObject(&team_icons->displayObjects[i], 0, 0, team_icons->iconGeo);
        team_icons->displayObjects[i].node.callbackArg = team_icons;
    }

    for (i = 1; i < 4; i++) {
        team_icons->displayObjects[i].rotation.y = 0x5800;
        Vec3f_SetComponentsDuplicate(&team_icons->displayObjects[i].position, (i * 64.0f) - 96.0f, 48.0f, -289.0f);
    }

    for (i = 0; i < 3; i++) {
        team_icons->displayObjects[4 + i].rotation.y = -0x5800;
        Vec3f_SetComponentsDuplicate(&team_icons->displayObjects[4 + i].position, (i * 64.0f) - 96.0f, -48.0f, -289.0f);
    }
}

/**
 * Single layout: binds fainted faces 9-14 to faintedGeo at the same positions as cards 1-6, turned -0x8000 about Y
 * to face away, so BattlePrepTeamIcons_FlipFaintedStepSingle can turn them into view together with the cards.
 */
void BattlePrepTeamIcons_InitFaintedFacesSingle(BattlePrepTeamIcons* team_icons) {
    s32 i;

    for (i = 1; i < 7; i++) {
        Model_InitDisplayObject(&team_icons->displayObjects[8 + i], 0, 0, team_icons->faintedGeo);
        team_icons->displayObjects[8 + i].node.callbackArg = team_icons;
        team_icons->displayObjects[8 + i].rotation.y = -0x8000;
    }

    for (i = 1; i < 4; i++) {
        Vec3f_SetComponentsDuplicate(&team_icons->displayObjects[8 + i].position, (i * 64.0f) - 96.0f, 48.0f, -289.0f);
    }

    for (i = 0; i < 3; i++) {
        Vec3f_SetComponentsDuplicate(&team_icons->displayObjects[12 + i].position, (i * 64.0f) - 96.0f, -48.0f, -289.0f);
    }
}

/**
 * Double layout: binds flip objects 16-21 (side 0) and 22-27 (side 1), upper halves to iconGeo and lower halves (a
 * tag team's second member) to lowerIconGeo, at the single layout's positions and starting Y turn.
 */
void BattlePrepTeamIcons_InitIconCardsDouble(BattlePrepTeamIcons* team_icons) {
    s32 i;

    for (i = 1; i < 4; i++) {
        Model_InitDisplayObject(&team_icons->displayObjects[15 + i], 0, 0, team_icons->iconGeo);
        Model_InitDisplayObject(&team_icons->displayObjects[18 + i], 0, 0, team_icons->lowerIconGeo);
        team_icons->displayObjects[15 + i].node.callbackArg = team_icons;
        team_icons->displayObjects[18 + i].node.callbackArg = team_icons;

        team_icons->displayObjects[15 + i].rotation.y = 0x5800;
        team_icons->displayObjects[18 + i].rotation.y = 0x5800;

        Vec3f_SetComponentsDuplicate(&team_icons->displayObjects[15 + i].position, (i * 64.0f) - 96.0f, 48.0f, -289.0f);
        Vec3f_SetComponentsDuplicate(&team_icons->displayObjects[18 + i].position, (i * 64.0f) - 96.0f, 48.0f, -289.0f);
    }

    for (i = 0; i < 3; i++) {
        Model_InitDisplayObject(&team_icons->displayObjects[22 + i], 0, 0, team_icons->iconGeo);
        Model_InitDisplayObject(&team_icons->displayObjects[25 + i], 0, 0, team_icons->lowerIconGeo);
        team_icons->displayObjects[22 + i].node.callbackArg = team_icons;
        team_icons->displayObjects[25 + i].node.callbackArg = team_icons;

        team_icons->displayObjects[22 + i].rotation.y = -0x5800;
        team_icons->displayObjects[25 + i].rotation.y = -0x5800;

        Vec3f_SetComponentsDuplicate(&team_icons->displayObjects[22 + i].position, (i * 64.0f) - 96.0f, -48.0f, -289.0f);
        Vec3f_SetComponentsDuplicate(&team_icons->displayObjects[25 + i].position, (i * 64.0f) - 96.0f, -48.0f, -289.0f);
    }
}

/**
 * Double layout: binds fainted-face flip objects 28-33 (side 0) and 34-39 (side 1), upper halves to faintedGeo and
 * lower halves to lowerFaintedGeo, at the card positions and turned -0x8000 about Y to face away.
 */
void BattlePrepTeamIcons_InitFaintedFacesDouble(BattlePrepTeamIcons* team_icons) {
    s32 i;

    for (i = 1; i < 4; i++) {
        Model_InitDisplayObject(&team_icons->displayObjects[27 + i], 0, 0, team_icons->faintedGeo);
        Model_InitDisplayObject(&team_icons->displayObjects[30 + i], 0, 0, team_icons->lowerFaintedGeo);

        team_icons->displayObjects[27 + i].node.callbackArg = team_icons;
        team_icons->displayObjects[30 + i].node.callbackArg = team_icons;

        team_icons->displayObjects[27 + i].rotation.y = -0x8000;
        team_icons->displayObjects[30 + i].rotation.y = -0x8000;

        Vec3f_SetComponentsDuplicate(&team_icons->displayObjects[27 + i].position, (i * 64.0f) - 96.0f, 48.0f, -289.0f);
        Vec3f_SetComponentsDuplicate(&team_icons->displayObjects[30 + i].position, (i * 64.0f) - 96.0f, 48.0f, -289.0f);
    }

    for (i = 0; i < 3; i++) {
        Model_InitDisplayObject(&team_icons->displayObjects[34 + i], 0, 0, team_icons->faintedGeo);
        Model_InitDisplayObject(&team_icons->displayObjects[37 + i], 0, 0, team_icons->lowerFaintedGeo);
        team_icons->displayObjects[34 + i].node.callbackArg = team_icons;
        team_icons->displayObjects[37 + i].node.callbackArg = team_icons;

        team_icons->displayObjects[34 + i].rotation.y = -0x8000;
        team_icons->displayObjects[37 + i].rotation.y = -0x8000;

        Vec3f_SetComponentsDuplicate(&team_icons->displayObjects[34 + i].position, (i * 64.0f) - 96.0f, -48.0f, -289.0f);
        Vec3f_SetComponentsDuplicate(&team_icons->displayObjects[37 + i].position, (i * 64.0f) - 96.0f, -48.0f, -289.0f);
    }
}

/**
 * For frames 0-10, turns display objects 0-3 by -0x800 and 4-7 by +0x800 about Y, flipping the trainer cards and
 * cards in from their 0x5800 start, and draws the HP bars full. Plays sound 0x01100009 on frame 0.
 */
void BattlePrepTeamIcons_FlipInStepSingle(BattlePrepTeamIcons* team_icons, s16 frame) {
    s16 i;

    if (frame == 0) {
        Audio_PlayCategory11SoundCommand(0x01100009, 0, 0);
    }

    if (frame < 0xB) {
        for (i = 0; i < 4; i++) {
            team_icons->displayObjects[i].rotation.y -= 0x800;
        }

        for (i = 4; i < 8; i++) {
            team_icons->displayObjects[i].rotation.y += 0x800;
        }

        BattlePrepTeamIcons_DrawHpBarsSingle(team_icons, 0x64);
    }
}

/**
 * Double-layout counterpart of BattlePrepTeamIcons_FlipInStepSingle: flips trainer cards 0 and 7 and flip objects
 * 16-27.
 */
void BattlePrepTeamIcons_FlipInStepDouble(BattlePrepTeamIcons* team_icons, s16 frame) {
    s16 i;

    if (frame == 0) {
        Audio_PlayCategory11SoundCommand(0x01100009, 0, 0);
    }

    if (frame < 0xB) {
        team_icons->displayObjects[0].rotation.y -= 0x800;
        team_icons->displayObjects[7].rotation.y += 0x800;

        for (i = 16; i < 22; i++) {
            team_icons->displayObjects[i].rotation.y -= 0x800;
        }

        for (i = 22; i < 28; i++) {
            team_icons->displayObjects[i].rotation.y += 0x800;
        }

        BattlePrepTeamIcons_DrawHpBarsDouble(team_icons, 0x64);
    }
}

/**
 * NTSC: frames 0-25 drain the HP bars from 100% by 4 points per frame (sound 0x0110000A on frame 0); frames 26-61
 * then stamp the red X on the next faint order every 6 frames (faint orders 1-6).
 */
void BattlePrepTeamIcons_HpDrainStepSingleNTSC(BattlePrepTeamIcons* team_icons, s16 frame) {
    if (frame == 0) {
        Audio_PlayCategory11SoundCommand(0x0110000A, 0, 0);
    }

    if (frame < 0x1A) {
        BattlePrepTeamIcons_DrawHpBarsSingle(team_icons, 0x64 - (frame * 4));
    } else if (frame < 0x3E) {
        if (((frame - 0x1A) % 6) == 0) {
            BattlePrepTeamIcons_MarkFaintedSingle(team_icons, ((frame - 0x1A) / 6) + 1);
        }
    }
}

/**
 * Double-layout counterpart of BattlePrepTeamIcons_HpDrainStepSingleNTSC that stamps the next faint order every 3
 * frames (faint orders 1-12).
 */
void BattlePrepTeamIcons_HpDrainStepDoubleNTSC(BattlePrepTeamIcons* team_icons, s16 frame) {
    if (frame == 0) {
        Audio_PlayCategory11SoundCommand(0x0110000A, 0, 0);
    }

    if (frame < 0x1A) {
        BattlePrepTeamIcons_DrawHpBarsDouble(team_icons, 0x64 - (frame * 4));
    } else if (frame < 0x3E) {
        if (((frame - 0x1A) % 3) == 0) {
            BattlePrepTeamIcons_MarkFaintedDouble(team_icons, ((frame - 0x1A) / 3) + 1);
        }
    }
}

/**
 * PAL timing of BattlePrepTeamIcons_HpDrainStepSingleNTSC: frames 0-20 drain 5 points per frame, then frames 21-50
 * stamp the next faint order every 5 frames.
 */
void BattlePrepTeamIcons_HpDrainStepSinglePAL(BattlePrepTeamIcons* team_icons, s16 frame) {
    if (frame == 0) {
        Audio_PlayCategory11SoundCommand(0x0110000A, 0, 0);
    }

    if (frame < 0x15) {
        BattlePrepTeamIcons_DrawHpBarsSingle(team_icons, 0x64 - (frame * 5));
    } else if (frame < 0x33) {
        if (((frame - 0x15) % 5) == 0) {
            BattlePrepTeamIcons_MarkFaintedSingle(team_icons, ((frame - 0x15) / 5) + 1);
        }
    }
}

/**
 * PAL timing of BattlePrepTeamIcons_HpDrainStepDoubleNTSC: frames 0-20 drain 5 points per frame, then frames 21-56
 * stamp the next faint order every 3 frames.
 */
void BattlePrepTeamIcons_HpDrainStepDoublePAL(BattlePrepTeamIcons* team_icons, s16 frame) {
    if (frame == 0) {
        Audio_PlayCategory11SoundCommand(0x0110000A, 0, 0);
    }

    if (frame < 0x15) {
        BattlePrepTeamIcons_DrawHpBarsDouble(team_icons, 0x64 - (frame * 5));
    } else if (frame < 0x39) {
        if (((frame - 0x15) % 3) == 0) {
            BattlePrepTeamIcons_MarkFaintedDouble(team_icons, ((frame - 0x15) / 3) + 1);
        }
    }
}

/**
 * Turns fainted Pokémon's cards over. Frame 0 sets up the fainted faces; frames 0-15 spin each fainted Pokémon's
 * card and its fainted face by 0x800 per frame about Y (180 degrees in total), so the "FAINTED" face comes into
 * view. On frame 16, clears the flip objects and rebinds cards 1-6 to faintedGeo (fainted) or iconGeo (did not
 * faint, or empty slot) with no turn.
 */
void BattlePrepTeamIcons_FlipFaintedStepSingle(BattlePrepTeamIcons* team_icons, s16 frame) {
    s32 i;
    BattleMon* side0_pokemon = gSessionContext.battleSides[0].tagTeamMember[0]->pokemonPicked;
    BattleMon* side1_pokemon = gSessionContext.battleSides[1].tagTeamMember[0]->pokemonPicked;

    if (frame == 0) {
        BattlePrepTeamIcons_InitFaintedFacesSingle(team_icons);
    }

    if (frame < 0x10) {
        for (i = 0; i < gSessionContext.battleSides[0].tagTeamMember[0]->numberOfPokemonBrought; i++) {
            if (side0_pokemon[i].unk_25 != 0) {
                team_icons->displayObjects[1 + i].rotation.y += 0x800;
                team_icons->displayObjects[9 + i].rotation.y += 0x800;
            }
        }

        for (i = 0; i < gSessionContext.battleSides[1].tagTeamMember[0]->numberOfPokemonBrought; i++) {
            if (side1_pokemon[i].unk_25 != 0) {
                team_icons->displayObjects[4 + i].rotation.y -= 0x800;
                team_icons->displayObjects[12 + i].rotation.y -= 0x800;
            }
        }
    }

    if (frame == 0x10) {
        for (i = 1; i < 7; i++) {
            ModelRenderer_ClearDisplayObject(&team_icons->displayObjects[i]);
            BattlePrep_ResetDisplayObject(&team_icons->displayObjects[8 + i]);
            team_icons->displayObjects[i].rotation.y = 0;
        }

        for (i = 0; i < 3; i++) {
            if ((i >= gSessionContext.battleSides[0].tagTeamMember[0]->numberOfPokemonBrought) ||
                (side0_pokemon[i].unk_25 == 0)) {
                Model_InitDisplayObject(&team_icons->displayObjects[1 + i], 0, 0, team_icons->iconGeo);
            } else {
                Model_InitDisplayObject(&team_icons->displayObjects[1 + i], 0, 0, team_icons->faintedGeo);
            }
        }

        for (i = 0; i < 3; i++) {
            if ((i >= gSessionContext.battleSides[1].tagTeamMember[0]->numberOfPokemonBrought) ||
                (side1_pokemon[i].unk_25 == 0)) {
                Model_InitDisplayObject(&team_icons->displayObjects[4 + i], 0, 0, team_icons->iconGeo);
            } else {
                Model_InitDisplayObject(&team_icons->displayObjects[4 + i], 0, 0, team_icons->faintedGeo);
            }
        }
    }
}

/**
 * Double-layout counterpart of BattlePrepTeamIcons_FlipFaintedStepSingle, spinning flip objects 16-39. On frame 16,
 * builds the final cards from which flip objects turned: side 0 upper halves on display objects 1-3 and lower
 * halves on 9-11 (y = +48), side 1 on 4-6 and 12-14 (y = -48). Then resets objects 16-39.
 */
void BattlePrepTeamIcons_FlipFaintedStepDouble(BattlePrepTeamIcons* team_icons, s16 frame) {
    s32 i;
    BattleMon* side0_pokemon = &gSessionContext.battleSides[0].tagTeamMember[0]->pokemonPicked[0];
    BattleMon* side0_partner_pokemon = NULL;
    BattleMon* side1_pokemon = &gSessionContext.battleSides[1].tagTeamMember[0]->pokemonPicked[0];
    BattleMon* side1_partner_pokemon = NULL;

    if (gSessionContext.battleSides[0].tagTeamSize == 2) {
        side0_partner_pokemon = &gSessionContext.battleSides[0].tagTeamMember[1]->pokemonPicked[0];
    }

    if (gSessionContext.battleSides[1].tagTeamSize == 2) {
        side1_partner_pokemon = &gSessionContext.battleSides[1].tagTeamMember[1]->pokemonPicked[0];
    }

    if (frame == 0) {
        BattlePrepTeamIcons_InitFaintedFacesDouble(team_icons);
    }

    if (frame < 0x10) {
        for (i = 0; i < gSessionContext.battleSides[0].tagTeamMember[0]->numberOfPokemonBrought; i++) {
            if (side0_pokemon[i].unk_25 != 0) {
                team_icons->displayObjects[16 + i].rotation.y += 0x800;
                team_icons->displayObjects[28 + i].rotation.y += 0x800;
            }
        }

        for (i = 0; i < gSessionContext.battleSides[1].tagTeamMember[0]->numberOfPokemonBrought; i++) {
            if (side1_pokemon[i].unk_25 != 0) {
                team_icons->displayObjects[22 + i].rotation.y -= 0x800;
                team_icons->displayObjects[34 + i].rotation.y -= 0x800;
            }
        }

        if (side0_partner_pokemon != NULL) {
            for (i = 0; i < gSessionContext.battleSides[0].tagTeamMember[1]->numberOfPokemonBrought; i++) {
                if (side0_partner_pokemon[i].unk_25 != 0) {
                    team_icons->displayObjects[19 + i].rotation.y += 0x800;
                    team_icons->displayObjects[31 + i].rotation.y += 0x800;
                }
            }
        }

        if (side1_partner_pokemon != NULL) {
            for (i = 0; i < gSessionContext.battleSides[1].tagTeamMember[1]->numberOfPokemonBrought; i++) {
                if (side1_partner_pokemon[i].unk_25 != 0) {
                    team_icons->displayObjects[25 + i].rotation.y -= 0x800;
                    team_icons->displayObjects[37 + i].rotation.y -= 0x800;
                }
            }
        }
    }

    if (frame == 0x10) {
        for (i = 0; i < 3; i++) {
            if (team_icons->displayObjects[16 + i].rotation.y == 0) {
                Model_InitDisplayObject(&team_icons->displayObjects[1 + i], 0, 0, team_icons->iconGeo);
            } else {
                Model_InitDisplayObject(&team_icons->displayObjects[1 + i], 0, 0, team_icons->faintedGeo);
            }
            team_icons->displayObjects[1 + i].node.callbackArg = team_icons;
            Vec3f_SetComponentsDuplicate(&team_icons->displayObjects[1 + i].position, (i * 64.0f) - 32.0f, 48.0f, -289.0f);
        }

        for (i = 0; i < 3; i++) {
            if (team_icons->displayObjects[19 + i].rotation.y == 0) {
                Model_InitDisplayObject(&team_icons->displayObjects[9 + i], 0, 0, team_icons->lowerIconGeo);
            } else {
                Model_InitDisplayObject(&team_icons->displayObjects[9 + i], 0, 0, team_icons->lowerFaintedGeo);
            }
            team_icons->displayObjects[9 + i].node.callbackArg = team_icons;
            Vec3f_SetComponentsDuplicate(&team_icons->displayObjects[9 + i].position, (i * 64.0f) - 32.0f, 48.0f, -289.0f);
        }

        for (i = 0; i < 3; i++) {
            if (team_icons->displayObjects[22 + i].rotation.y == 0) {
                Model_InitDisplayObject(&team_icons->displayObjects[4 + i], 0, 0, team_icons->iconGeo);
            } else {
                Model_InitDisplayObject(&team_icons->displayObjects[4 + i], 0, 0, team_icons->faintedGeo);
            }
            team_icons->displayObjects[4 + i].node.callbackArg = team_icons;
            Vec3f_SetComponentsDuplicate(&team_icons->displayObjects[4 + i].position, (i * 64.0f) - 96.0f, -48.0f, -289.0f);
        }

        for (i = 0; i < 3; i++) {
            if (team_icons->displayObjects[25 + i].rotation.y == 0) {
                Model_InitDisplayObject(&team_icons->displayObjects[12 + i], 0, 0, team_icons->lowerIconGeo);
            } else {
                Model_InitDisplayObject(&team_icons->displayObjects[12 + i], 0, 0, team_icons->lowerFaintedGeo);
            }
            team_icons->displayObjects[12 + i].node.callbackArg = team_icons;
            Vec3f_SetComponentsDuplicate(&team_icons->displayObjects[12 + i].position, (i * 64.0f) - 96.0f, -48.0f, -289.0f);
        }

        for (i = 16; i < 40; i++) {
            BattlePrep_ResetDisplayObject(&team_icons->displayObjects[i]);
        }
    }
}

/**
 * State 3 tick of the result animation: cards flip in from frame 0, the HP bars drain and the faint marks appear
 * from frame 12, and fainted cards turn over from frame 74 (62 on PAL). Returns to state 0 at frame 96 (80 on PAL).
 */
void BattlePrepTeamIcons_UpdateAnimationStep(BattlePrepTeamIcons* team_icons) {
    if (osTvType == OS_TV_PAL) {
        if (team_icons->isDoubleLayout == 1) {
            if (team_icons->timer >= 0) {
                BattlePrepTeamIcons_FlipInStepDouble(team_icons, team_icons->timer);
            }

            if (team_icons->timer >= 0xC) {
                BattlePrepTeamIcons_HpDrainStepDoublePAL(team_icons, team_icons->timer - 0xC);
            }

            if (team_icons->timer >= 0x3E) {
                BattlePrepTeamIcons_FlipFaintedStepDouble(team_icons, team_icons->timer - 0x3E);
            }
        } else {
            if (team_icons->timer >= 0) {
                BattlePrepTeamIcons_FlipInStepSingle(team_icons, team_icons->timer);
            }

            if (team_icons->timer >= 0xC) {
                BattlePrepTeamIcons_HpDrainStepSinglePAL(team_icons, team_icons->timer - 0xC);
            }

            if (team_icons->timer >= 0x3E) {
                BattlePrepTeamIcons_FlipFaintedStepSingle(team_icons, team_icons->timer - 0x3E);
            }
        }

        team_icons->timer++;
        if (team_icons->timer == 0x50) {
            team_icons->state = 0;
        }
    } else {
        if (team_icons->isDoubleLayout == 1) {
            if (team_icons->timer >= 0) {
                BattlePrepTeamIcons_FlipInStepDouble(team_icons, team_icons->timer);
            }

            if (team_icons->timer >= 0xC) {
                BattlePrepTeamIcons_HpDrainStepDoubleNTSC(team_icons, team_icons->timer - 0xC);
            }

            if (team_icons->timer >= 0x4A) {
                BattlePrepTeamIcons_FlipFaintedStepDouble(team_icons, team_icons->timer - 0x4A);
            }
        } else {
            if (team_icons->timer >= 0) {
                BattlePrepTeamIcons_FlipInStepSingle(team_icons, team_icons->timer);
            }

            if (team_icons->timer >= 0xC) {
                BattlePrepTeamIcons_HpDrainStepSingleNTSC(team_icons, team_icons->timer - 0xC);
            }

            if (team_icons->timer >= 0x4A) {
                BattlePrepTeamIcons_FlipFaintedStepSingle(team_icons, team_icons->timer - 0x4A);
            }
        }

        team_icons->timer++;
        if (team_icons->timer == 0x60) {
            team_icons->state = 0;
        }
    }
}

/**
 * State 1 tick: renders one PokeIcon slot per frame. Once at least 30 frames have passed and every slot is idle,
 * resets timer and sets state 2, so BattlePrepTeamIcons_InitDisplayObjects can build the scene.
 */
void BattlePrepTeamIcons_UpdateLoadingStep(BattlePrepTeamIcons* team_icons) {
    PokeIcon_ProcessNextSlot(team_icons->iconRenderList, 0);

    team_icons->timer++;
    if ((team_icons->timer >= 0x1E) && (PokeIcon_AreSlotsIdle(team_icons->iconRenderList) != 0)) {
        team_icons->timer = 0;
        team_icons->state = 2;
    }
}

/**
 * Per-frame update for the result team display: state 1 loads the icons, state 3 runs the result animation; states
 * 0 and 2 do nothing.
 */
void BattlePrepTeamIcons_Update(BattlePrepTeamIcons* team_icons) {
    switch (team_icons->state) {
        case 1:
            BattlePrepTeamIcons_UpdateLoadingStep(team_icons);
            break;

        case 3:
            BattlePrepTeamIcons_UpdateAnimationStep(team_icons);
            break;
    }
}

/**
 * Attaches a 0x80x0xC0 depth buffer to the icon render list and queues a species icon for each picked Pokémon: side
 * 0 into slots 0-2, side 1 into slots 3-5.
 */
void BattlePrepTeamIcons_QueueIconsSingle(BattlePrepTeamIcons* team_icons) {
    s32 i;
    s32 slot;
    ModelLoadContext* load_context;

    load_context = PokeIcon_AllocFramebuffers(5);
    PokeIcon_AttachDepthBuffer(team_icons->iconRenderList, load_context, GfxImage_Allocate(0, 2, 0x80, 0xC0, 1));

    for (slot = 0, i = 0; i < gSessionContext.battleSides[0].tagTeamMember[0]->numberOfPokemonBrought; slot++, i++) {
        func_8001A324(team_icons->iconRenderList, slot, gSessionContext.battleSides[0].tagTeamMember[0]->pokemonPicked[i].species.id,
                      0);
    }

    for (slot = 3, i = 0; i < gSessionContext.battleSides[1].tagTeamMember[0]->numberOfPokemonBrought; slot++, i++) {
        func_8001A324(team_icons->iconRenderList, slot, gSessionContext.battleSides[1].tagTeamMember[0]->pokemonPicked[i].species.id,
                      0);
    }
}

/**
 * Double-layout counterpart of BattlePrepTeamIcons_QueueIconsSingle with a 0x80x0x60 depth buffer: side 0's first
 * and second members go into slots 0-2 and 3-5, side 1's into 6-8 and 9-11. Absent second members are skipped.
 */
void BattlePrepTeamIcons_QueueIconsDouble(BattlePrepTeamIcons* team_icons) {
    s32 i;
    s32 slot;
    ModelLoadContext* load_context;

    load_context = PokeIcon_AllocFramebuffers(5);
    PokeIcon_AttachDepthBuffer(team_icons->iconRenderList, load_context, GfxImage_Allocate(0, 2, 0x80, 0x60, 1));

    for (slot = 0, i = 0; i < gSessionContext.battleSides[0].tagTeamMember[0]->numberOfPokemonBrought; slot++, i++) {
        func_8001A324(team_icons->iconRenderList, slot, gSessionContext.battleSides[0].tagTeamMember[0]->pokemonPicked[i].species.id,
                      0);
    }

    for (slot = 6, i = 0; i < gSessionContext.battleSides[1].tagTeamMember[0]->numberOfPokemonBrought; slot++, i++) {
        func_8001A324(team_icons->iconRenderList, slot, gSessionContext.battleSides[1].tagTeamMember[0]->pokemonPicked[i].species.id,
                      0);
    }

    if (gSessionContext.battleSides[0].tagTeamMember[1] != NULL) {
        for (slot = 3, i = 0; i < gSessionContext.battleSides[0].tagTeamMember[1]->numberOfPokemonBrought; slot++, i++) {
            func_8001A324(team_icons->iconRenderList, slot,
                          gSessionContext.battleSides[0].tagTeamMember[1]->pokemonPicked[i].species.id, 0);
        }
    }

    if (gSessionContext.battleSides[1].tagTeamMember[1] != NULL) {
        for (slot = 9, i = 0; i < gSessionContext.battleSides[1].tagTeamMember[1]->numberOfPokemonBrought; slot++, i++) {
            func_8001A324(team_icons->iconRenderList, slot,
                          gSessionContext.battleSides[1].tagTeamMember[1]->pokemonPicked[i].species.id, 0);
        }
    }
}

/**
 * Opens the PokeIcon model archives, loads the particle data library fragment, queues the picked Pokémon's icons
 * for the current layout, and draws both trainer cards (side 0 on D_3000000, side 1 on D_3003000, each with the
 * trainer's portrait from portrait_archive). Sets state 1 (loading).
 */
void BattlePrepTeamIcons_Load(BattlePrepTeamIcons* team_icons, BinArchive* portrait_archive) {
    u8* side0_portrait = BinArchive_GetFile(portrait_archive, gSessionContext.battleSides[0].tagTeamMember[0]->rosterInfo->unk_003);
    u8* side1_portrait = BinArchive_GetFile(portrait_archive, gSessionContext.battleSides[1].tagTeamMember[0]->rosterInfo->unk_003);

    PokeIcon_OpenModelArchives();

    FRAGMENT_LOAD(FRAGMENT_PARTICLE_DATA_LIBRARY);

    if (team_icons->isDoubleLayout == 1) {
        BattlePrepTeamIcons_QueueIconsDouble(team_icons);
    } else {
        BattlePrepTeamIcons_QueueIconsSingle(team_icons);
    }

    BattlePrep_RenderTrainerPortrait(team_icons->side0CardImage, 0, side0_portrait, D_3000000);
    BattlePrep_RenderTrainerPortrait(team_icons->side1CardImage, 1, side1_portrait, D_3003000);
    team_icons->state = 1;
}

/**
 * Once loading has reached state 2: stores the display objects, binds trainer cards 0 (top-left) and 7
 * (bottom-right) to trainerCardGeo, turned 0x5800 / -0x5800 about Y for the flip-in, sets up the cards for the
 * current layout, and enters state 3. Returns 1 once in state 3.
 */
s32 BattlePrepTeamIcons_InitDisplayObjects(BattlePrepTeamIcons* team_icons, DisplayObject* display_objects) {
    if (team_icons->state == 2) {
        team_icons->state = 3;
        team_icons->displayObjects = display_objects;
        team_icons->timer = 0;
        display_objects->node.callbackArg = team_icons;

        team_icons->displayObjects[7].node.callbackArg = team_icons;
        team_icons->displayObjects[0].rotation.y = 0x5800;
        team_icons->displayObjects[7].rotation.y = -0x5800;

        Vec3f_SetComponentsDuplicate(&team_icons->displayObjects[0].position, -96.0f, 48.0f, -289.0f);
        Vec3f_SetComponentsDuplicate(&team_icons->displayObjects[7].position, 96.0f, -48.0f, -289.0f);
        Model_InitDisplayObject(&team_icons->displayObjects[0], 0, 0, team_icons->trainerCardGeo);
        Model_InitDisplayObject(&team_icons->displayObjects[7], 0, 0, team_icons->trainerCardGeo);

        if (team_icons->isDoubleLayout == 1) {
            BattlePrepTeamIcons_InitIconCardsDouble(team_icons);
        } else {
            BattlePrepTeamIcons_InitIconCardsSingle(team_icons);
        }
    }
    return team_icons->state == 3;
}

/**
 * Loads each picked Pokémon's 40x40 model texture into modelTextures for the fainted faces: side 0 into slots 0-2,
 * side 1 into slots 3-5.
 */
void BattlePrepTeamIcons_LoadModelTexturesSingle(BattlePrepTeamIcons* team_icons) {
    s32 i;
    s32 slot;

    for (i = 0, slot = 0; i < gSessionContext.battleSides[0].tagTeamMember[0]->numberOfPokemonBrought; slot++, i++) {
        PokeIcon_LoadModelTextureForMon(
            team_icons->modelTextures[slot], gSessionContext.battleSides[0].tagTeamMember[0]->pokemonPicked[i].species.id, NULL);
    }

    for (i = 0, slot = 3; i < gSessionContext.battleSides[1].tagTeamMember[0]->numberOfPokemonBrought; slot++, i++) {
        PokeIcon_LoadModelTextureForMon(
            team_icons->modelTextures[slot], gSessionContext.battleSides[1].tagTeamMember[0]->pokemonPicked[i].species.id, NULL);
    }
}

/**
 * Double-layout counterpart of BattlePrepTeamIcons_LoadModelTexturesSingle: side 0's first and second members go
 * into slots 0-2 and 3-5, side 1's into 6-8 and 9-11.
 */
void BattlePrepTeamIcons_LoadModelTexturesDouble(BattlePrepTeamIcons* team_icons) {
    s32 i;
    s32 slot;

    for (i = 0, slot = 0; i < gSessionContext.battleSides[0].tagTeamMember[0]->numberOfPokemonBrought; slot++, i++) {
        PokeIcon_LoadModelTextureForMon(
            &team_icons->modelTextures[slot], gSessionContext.battleSides[0].tagTeamMember[0]->pokemonPicked[i].species.id, NULL);
    }

    for (i = 0, slot = 6; i < gSessionContext.battleSides[1].tagTeamMember[0]->numberOfPokemonBrought; slot++, i++) {
        PokeIcon_LoadModelTextureForMon(
            &team_icons->modelTextures[slot], gSessionContext.battleSides[1].tagTeamMember[0]->pokemonPicked[i].species.id, NULL);
    }

    if (gSessionContext.battleSides[0].tagTeamMember[1] != NULL) {
        for (i = 0, slot = 3; i < gSessionContext.battleSides[0].tagTeamMember[1]->numberOfPokemonBrought; slot++, i++) {
            PokeIcon_LoadModelTextureForMon(
                &team_icons->modelTextures[slot], gSessionContext.battleSides[0].tagTeamMember[1]->pokemonPicked[i].species.id,
                NULL);
        }
    }

    if (gSessionContext.battleSides[1].tagTeamMember[1] != NULL) {
        for (i = 0, slot = 9; i < gSessionContext.battleSides[1].tagTeamMember[1]->numberOfPokemonBrought; slot++, i++) {
            PokeIcon_LoadModelTextureForMon(
                &team_icons->modelTextures[slot], gSessionContext.battleSides[1].tagTeamMember[1]->pokemonPicked[i].species.id,
                NULL);
        }
    }
}

/**
 * Initializes the result team display: allocates the two 64x96 trainer card images; selects the double layout for
 * rule category 8 when side 0 brought 4+ Pokémon, or for rule category 0 with a tag team or 4+ brought on either
 * side (numberOfPokemonBrought); clears the faint marks; creates the PokeIcon render list (12 slots of 0x80x0x60,
 * or 6 of 0x80x0xC0) and the model texture buffer; builds the geo layouts; and loads the model textures.
 */
void BattlePrepTeamIcons_Init(BattlePrepTeamIcons* team_icons) {
    MemoryBlock* geo_pool;
    s32 i;

    team_icons->state = 0;
    team_icons->timer = 0;
    team_icons->isDoubleLayout = 0;
    team_icons->side0CardImage = GfxImage_Allocate(0, 2, 0x40, 0x60, 0);
    team_icons->side1CardImage = GfxImage_Allocate(0, 2, 0x40, 0x60, 0);

    if ((gSessionContext.ruleCategory == 8) &&
        (gSessionContext.battleSides[0].tagTeamMember[0]->numberOfPokemonBrought >= 4)) {
        team_icons->isDoubleLayout = 1;
    }

    if (gSessionContext.ruleCategory == 0) {
        if ((gSessionContext.battleSides[0].tagTeamSize == 2) || (gSessionContext.battleSides[1].tagTeamSize == 2)) {
            team_icons->isDoubleLayout = 1;
        } else {
            if (gSessionContext.battleSides[0].tagTeamMember[0]->numberOfPokemonBrought >= 4) {
                team_icons->isDoubleLayout = 1;
            }

            if (gSessionContext.battleSides[1].tagTeamMember[0]->numberOfPokemonBrought >= 4) {
                team_icons->isDoubleLayout = 1;
            }
        }
    }

    for (i = 0; i < 12; i++) {
        team_icons->isFaintMarked[i] = 0;
    }

    if (team_icons->isDoubleLayout == 1) {
        team_icons->iconRenderList = PokeIcon_CreateRenderList(NULL, 0xC, 0x80, 0x60);
        team_icons->modelTextures = ALIGN64((u32)main_pool_alloc(0x9640, 0));
    } else {
        team_icons->iconRenderList = PokeIcon_CreateRenderList(NULL, 6, 0x80, 0xC0);
        team_icons->modelTextures = ALIGN64((u32)main_pool_alloc(0x4B40, 0));
    }

    PokeIcon_SetNameTable(team_icons->iconRenderList, sTeamIconsSpeciesIcons);

    geo_pool = MainPool_AllocState(main_pool_get_available(), 0);
    if (team_icons->isDoubleLayout == 1) {
        team_icons->iconGeo = process_geo_layout(geo_pool, D_84B15F50);
        team_icons->lowerIconGeo = process_geo_layout(geo_pool, D_84B15FA8);
        team_icons->faintedGeo = process_geo_layout(geo_pool, D_84B162D8);
        team_icons->lowerFaintedGeo = process_geo_layout(geo_pool, D_84B16330);
    } else {
        team_icons->iconGeo = process_geo_layout(geo_pool, D_84B15688);
        team_icons->faintedGeo = process_geo_layout(geo_pool, D_84B15A20);
    }

    team_icons->trainerCardGeo = process_geo_layout(geo_pool, D_84B14DC8);
    MainPool_FinalizeAllocation(geo_pool);

    if (team_icons->isDoubleLayout == 1) {
        BattlePrepTeamIcons_LoadModelTexturesDouble(team_icons);
    } else {
        BattlePrepTeamIcons_LoadModelTexturesSingle(team_icons);
    }
}
