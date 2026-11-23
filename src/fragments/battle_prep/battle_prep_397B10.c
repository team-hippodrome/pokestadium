#include "battle_prep.h"
#include "src/geo_render.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/ui_graphics.h"
#include "src/audio_commands_category2.h"
#include "src/gfx_rect.h"
#include "src/matrix.h"
#include "src/memory.h"
#include "src/util.h"

s32 BattlePrepBanner_DrawPlate(s32, unk_func_80011B94*);
s32 BattlePrepBanner_DrawTitle(s32, unk_func_80011B94*);
s32 BattlePrepBanner_DrawDivision(s32, unk_func_80011B94*);
s32 BattlePrepBanner_ApplyRoundLabelSpin(s32, unk_D_86002F34_alt5*);
s32 BattlePrepBanner_DrawRoundLabel(s32, unk_func_80011B94*);

s32 OpponentLadder_DrawCupSlot(s32, unk_func_80011B94*);
s32 OpponentLadder_DrawCupHighlight(s32, unk_func_80011B94*);
s32 OpponentLadder_DrawCastleSlot(s32, unk_func_80011B94*);
s32 OpponentLadder_DrawCastleHighlight(s32, unk_func_80011B94*);

// battle_headers file index of the banner plate for each cup (mode - 1).
static u8 D_84B0FC40[] = {
    0x00, 0x00, 0x00, 0x01, 0x02, 0x03,
};
// battle_headers file index of the banner plate for each castle location (modeSelection).
static u8 D_84B0FC48[] = {
    0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0C,
};
// Banner title textures (200x28 IA8) for each cup (mode - 1): "POKé CUP" x3, "PETIT CUP", "PIKA CUP",
// "PRIME CUP".
static u8* D_84B0FC54[] = {
    D_30339F0, D_3034FD0, D_30365B0, D_3037B90, D_3039170, D_303A750,
};
// Banner title textures (200x28 IA8) for each castle location (modeSelection): "PEWTER GYM BATTLE" ...
// "VIRIDIAN GYM BATTLE", "ELITE FOUR BATTLE".
static u8* D_84B0FC6C[] = {
    D_303BD30, D_303D310, D_303E8F0, D_303FED0, D_30414B0, D_3042A90, D_3044070, D_3045650, D_3046C30,
};
// Round digit textures (32x32 IA8) "1"-"6", indexed by roundOrDemoId. Entry 0 is "FINAL BATTLE"
// (200x28 IA8), which the rival banner reads as its title through D_84B0FC6C[9].
static u8* D_84B0FC90[] = {
    D_3048210, D_3029270, D_3029670, D_3029A70, D_3029E70, D_302A270, D_302A670,
};
// Division textures (152x20 IA8): "POKé BALL", "GREAT BALL", "ULTRA BALL", "MASTER BALL".
static u8* D_84B0FCAC[] = {
    D_3030A70,
    D_3031650,
    D_3032230,
    D_3032E10,
};
// Banner gradient colors, indexed by mode - 1 (+ modeSelection in the castle). BattlePrepBanner_ApplyColors
// pairs each entry with the same entry of D_84B0FCFC.
static Color_RGBA8 D_84B0FCBC[] = {
    { 0x1E, 0x8C, 0x1E, 0xFF }, { 0x1E, 0x8C, 0x1E, 0xFF }, { 0x1E, 0x8C, 0x1E, 0xFF }, { 0x50, 0x50, 0xFF, 0xFF },
    { 0xB4, 0x8C, 0x1E, 0xFF }, { 0x96, 0x1E, 0x1E, 0xFF }, { 0x64, 0x64, 0x82, 0xFF }, { 0x50, 0x50, 0xFF, 0xFF },
    { 0xC8, 0x78, 0x1E, 0xFF }, { 0x82, 0x32, 0xC8, 0xFF }, { 0xB4, 0x3C, 0xB4, 0xFF }, { 0xB4, 0x8C, 0x1E, 0xFF },
    { 0x96, 0x1E, 0x1E, 0xFF }, { 0x1E, 0x8C, 0x1E, 0xFF }, { 0xB4, 0x50, 0x1E, 0xFF }, { 0x1E, 0x3C, 0xA0, 0xFF },
};
static Color_RGBA8 D_84B0FCFC[] = {
    { 0xC8, 0xC8, 0x1E, 0xFF }, { 0xC8, 0xC8, 0x1E, 0xFF }, { 0xC8, 0xC8, 0x1E, 0xFF }, { 0x64, 0xDC, 0xDC, 0xFF },
    { 0xFF, 0xFF, 0xFF, 0xFF }, { 0xC8, 0xC8, 0x1E, 0xFF }, { 0xFF, 0xFF, 0xFF, 0xFF }, { 0x64, 0xDC, 0xDC, 0xFF },
    { 0xDC, 0xDC, 0x1E, 0xFF }, { 0xDC, 0x64, 0xDC, 0xFF }, { 0xFF, 0xC8, 0xFF, 0xFF }, { 0xFF, 0xFF, 0xFF, 0xFF },
    { 0xC8, 0x82, 0x82, 0xFF }, { 0xC8, 0xC8, 0x1E, 0xFF }, { 0xC8, 0xC8, 0x1E, 0xFF }, { 0x1E, 0xDC, 0x82, 0xFF },
};
static Vtx D_84B0FD40[] = {
    VTX(-128, 48, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-128, 16, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-64, 16, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(-64, 48, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-64, 48, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),    VTX(-64, 16, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 16, 0, 4096, 1024, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(0, 48, 0, 4096, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 48, 0, 4096, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(0, 16, 0, 4096, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(64, 16, 0, 6144, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(64, 48, 0, 6144, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(64, 48, 0, 6144, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(64, 16, 0, 6144, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(128, 16, 0, 8192, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(128, 48, 0, 8192, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Vtx D_84B0FE40[] = {
    VTX(-128, 16, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),    VTX(-128, -16, 0, 0, 2048, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-64, -16, 0, 2048, 2048, 0xFF, 0xFF, 0xFF, 0xFF), VTX(-64, 16, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-64, 16, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(-64, -16, 0, 2048, 2048, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, -16, 0, 4096, 2048, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(0, 16, 0, 4096, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 16, 0, 4096, 1024, 0xFF, 0xFF, 0xFF, 0xFF),    VTX(0, -16, 0, 4096, 2048, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(64, -16, 0, 6144, 2048, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(64, 16, 0, 6144, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(64, 16, 0, 6144, 1024, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(64, -16, 0, 6144, 2048, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(128, -16, 0, 8192, 2048, 0xFF, 0xFF, 0xFF, 0xFF), VTX(128, 16, 0, 8192, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Vtx D_84B0FF40[] = {
    VTX(-128, -16, 0, 0, 2048, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(-128, -48, 0, 0, 3072, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-64, -48, 0, 2048, 3072, 0xFF, 0xFF, 0xFF, 0xFF), VTX(-64, -16, 0, 2048, 2048, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-64, -16, 0, 2048, 2048, 0xFF, 0xFF, 0xFF, 0xFF), VTX(-64, -48, 0, 2048, 3072, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, -48, 0, 4096, 3072, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(0, -16, 0, 4096, 2048, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, -16, 0, 4096, 2048, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(0, -48, 0, 4096, 3072, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(64, -48, 0, 6144, 3072, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(64, -16, 0, 6144, 2048, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(64, -16, 0, 6144, 2048, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(64, -48, 0, 6144, 3072, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(128, -48, 0, 8192, 3072, 0xFF, 0xFF, 0xFF, 0xFF), VTX(128, -16, 0, 8192, 2048, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B10040[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_PASS2),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B0FD40, 16, 0),
    gsDPLoadTextureTile(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 256, 0, 0, 0, 63, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureTile(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 256, 0, 64, 0, 127, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsDPLoadTextureTile(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 256, 0, 128, 0, 191, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsDPLoadTextureTile(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 256, 0, 192, 0, 255, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),
    gsSPVertex(D_84B0FE40, 16, 0),
    gsDPLoadTextureTile(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 256, 0, 0, 32, 63, 63, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureTile(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 256, 0, 64, 32, 127, 63, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsDPLoadTextureTile(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 256, 0, 128, 32, 191, 63, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsDPLoadTextureTile(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 256, 0, 192, 32, 255, 63, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD),
    gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),
    gsSPVertex(D_84B0FF40, 16, 0),
    gsDPLoadTextureTile(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 256, 0, 0, 64, 63, 95, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureTile(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 256, 0, 64, 64, 127, 95, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsDPLoadTextureTile(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 256, 0, 128, 64, 191, 95, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsDPLoadTextureTile(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 256, 0, 192, 64, 255, 95, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD),
    gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B10378[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B0FD40,
    0x05000000, 0x22010000, 0x00000000, 0x08000000, BattlePrepBanner_DrawPlate,
    D_84B10040, 0x06000000, 0x01000000, 0x00000000,
};
static Vtx D_84B103B0[12] = {
    VTX(-100, 14, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(-36, 14, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-36, 14, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF), VTX(36, 14, 0, 4352, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(36, 14, 0, 4352, 0, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(100, 14, 0, 6400, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-100, -14, 0, 0, 896, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(-36, -14, 0, 2048, 896, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-36, -14, 0, 2048, 896, 0xFF, 0xFF, 0xFF, 0xFF), VTX(36, -14, 0, 4352, 896, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(36, -14, 0, 4352, 896, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(100, -14, 0, 6400, 896, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B10470[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIM_LOD_FRAC, 0, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B103B0, 6, 0),
    gsSPEndDisplayList(),
};
static Gfx D_84B10498[] = {
    gsSPVertex(&D_84B103B0[6], 6, 6),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 200, 0x0F000000),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadTile(G_TX_LOADTILE, 0, 0, 0x00FC, 0x006C),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(G_TX_RENDERTILE, 0, 0, 0x00FC, 0x006C),
    gsSP2Triangles(0, 6, 7, 0, 0, 7, 1, 0),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 200, 0x0F000000),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 9, 0x0000, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadTile(G_TX_LOADTILE, 0x0100, 0, 0x021C, 0x006C),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 9, 0x0000, G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(G_TX_RENDERTILE, 0x0100, 0, 0x021C, 0x006C),
    gsSP2Triangles(2, 8, 9, 0, 2, 9, 3, 0),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 200, 0x0F000000),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadTile(G_TX_LOADTILE, 0x0220, 0, 0x031C, 0x006C),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(G_TX_RENDERTILE, 0x0220, 0, 0x031C, 0x006C),
    gsSP2Triangles(4, 10, 11, 0, 4, 11, 5, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B10568[] = {
    0x17000000, 0x0000000C, 0x00000000, 0x00000000, D_84B103B0, 0x05000000, 0x1B000000, 0x00000000,
    0x00000000, 0x00120000, 0x05000000, 0x22050000, D_84B10470, 0x06000000, 0x06000000, 0x01000000,
};
static u32 D_84B105A8[] = {
    0x17000000, 0x0000000C, 0x00000000, 0x00000000, D_84B103B0,
    0x05000000, 0x1B000000, 0x00000000, 0x00000000, 0x00120000,
    0x05000000, 0x22050000, 0x00000000, 0x08000000, BattlePrepBanner_DrawTitle,
    D_84B10498, 0x06000000, 0x06000000, 0x01000000, 0x00000000,
};
static Vtx D_84B105F8[] = {
    VTX(86, 14, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(114, 14, 0, 896, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-114, 14, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(-50, 14, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-50, 14, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF), VTX(22, 14, 0, 4352, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(22, 14, 0, 4352, 0, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(86, 14, 0, 6400, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Vtx D_84B10678[] = {
    VTX(86, -14, 0, 0, 896, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(114, -14, 0, 896, 896, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-114, -14, 0, 0, 896, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(-50, -14, 0, 2048, 896, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-50, -14, 0, 2048, 896, 0xFF, 0xFF, 0xFF, 0xFF), VTX(22, -14, 0, 4352, 896, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(22, -14, 0, 4352, 896, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(86, -14, 0, 6400, 896, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B106F8[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B105F8, 8, 0),
    gsSPEndDisplayList(),
};
static Gfx D_84B10718[] = {
    gsSPVertex(D_84B10678, 8, 8),
    gsDPSetCombineLERP(0, 0, 0, TEXEL0, TEXEL0, 0, PRIM_LOD_FRAC, 0, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED),
    gsDPLoadTextureBlock(D_3027890, G_IM_FMT_RGBA, G_IM_SIZ_16b, 28, 28, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 8, 9, 0, 0, 9, 1, 0),
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIM_LOD_FRAC, 0, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 200, 0x0F000000),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadTile(G_TX_LOADTILE, 0, 0, 0x00FC, 0x006C),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(G_TX_RENDERTILE, 0, 0, 0x00FC, 0x006C),
    gsSP2Triangles(2, 10, 11, 0, 2, 11, 3, 0),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 200, 0x0F000000),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 9, 0x0000, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadTile(G_TX_LOADTILE, 0x0100, 0, 0x021C, 0x006C),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 9, 0x0000, G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(G_TX_RENDERTILE, 0x0100, 0, 0x021C, 0x006C),
    gsSP2Triangles(4, 12, 13, 0, 4, 13, 5, 0),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 200, 0x0F000000),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadTile(G_TX_LOADTILE, 0x0220, 0, 0x031C, 0x006C),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(G_TX_RENDERTILE, 0x0220, 0, 0x031C, 0x006C),
    gsSP2Triangles(6, 14, 15, 0, 6, 15, 7, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B10838[] = {
    0x17000000, 0x00000010, 0x00000000, 0x00000000, D_84B105F8, 0x05000000, 0x1B000000, 0x00000000,
    0x00000000, 0x00120000, 0x05000000, 0x22050000, D_84B106F8, 0x06000000, 0x06000000, 0x01000000,
};
static u32 D_84B10878[] = {
    0x17000000, 0x00000010, 0x00000000, 0x00000000, D_84B105F8,
    0x05000000, 0x1B000000, 0x00000000, 0x00000000, 0x00120000,
    0x05000000, 0x22050000, 0x00000000, 0x08000000, BattlePrepBanner_DrawTitle,
    D_84B10718, 0x06000000, 0x06000000, 0x01000000, 0x00000000,
};
static Vtx D_84B108C8[] = {
    VTX(-76, 10, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(0, 10, 0, 2432, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 10, 0, 2432, 0, 0xFF, 0xFF, 0xFF, 0xFF),    VTX(76, 10, 0, 4864, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-76, -10, 0, 0, 640, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(0, -10, 0, 2432, 640, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, -10, 0, 2432, 640, 0xFF, 0xFF, 0xFF, 0xFF), VTX(76, -10, 0, 4864, 640, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B10948[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIM_LOD_FRAC, 0, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B108C8, 8, 0),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 152, 0x0F000000),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 10, 0x0000, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadTile(G_TX_LOADTILE, 0, 0, 0x012C, 0x004C),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 10, 0x0000, G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(G_TX_RENDERTILE, 0, 0, 0x012C, 0x004C),
    gsSP2Triangles(0, 4, 5, 0, 0, 5, 1, 0),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 152, 0x0F000000),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 10, 0x0000, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadTile(G_TX_LOADTILE, 0x0130, 0, 0x025C, 0x004C),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 10, 0x0000, G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(G_TX_RENDERTILE, 0x0130, 0, 0x025C, 0x004C),
    gsSP2Triangles(2, 6, 7, 0, 2, 7, 3, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B109F0[] = {
    0x17000000, 0x00000008, 0x00000000, 0x00000000, D_84B108C8,
    0x05000000, 0x1B000000, 0x00000000, 0x00000000, 0x00020000,
    0x05000000, 0x22050000, 0x00000000, 0x08000000, BattlePrepBanner_DrawDivision,
    D_84B10948, 0x06000000, 0x06000000, 0x01000000, 0x00000000,
};
static Vtx D_84B10A40[] = {
    VTX(-100, 14, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-36, 14, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-36, 14, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),    VTX(36, 14, 0, 4352, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(36, 14, 0, 4352, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(100, 14, 0, 6400, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-100, -14, 0, 0, 896, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(-36, -14, 0, 2048, 896, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-36, -14, 0, 2048, 896, 0xFF, 0xFF, 0xFF, 0xFF), VTX(36, -14, 0, 4352, 896, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(36, -14, 0, 4352, 896, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(100, -14, 0, 6400, 896, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B10B00[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIM_LOD_FRAC, 0, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B10A40, 12, 0),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 200, 0x0F000000),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadTile(G_TX_LOADTILE, 0, 0, 0x00FC, 0x006C),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(G_TX_RENDERTILE, 0, 0, 0x00FC, 0x006C),
    gsSP2Triangles(0, 6, 7, 0, 0, 7, 1, 0),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 200, 0x0F000000),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 9, 0x0000, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadTile(G_TX_LOADTILE, 0x0100, 0, 0x021C, 0x006C),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 9, 0x0000, G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(G_TX_RENDERTILE, 0x0100, 0, 0x021C, 0x006C),
    gsSP2Triangles(2, 8, 9, 0, 2, 9, 3, 0),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 200, 0x0F000000),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadTile(G_TX_LOADTILE, 0x0220, 0, 0x031C, 0x006C),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(G_TX_RENDERTILE, 0x0220, 0, 0x031C, 0x006C),
    gsSP2Triangles(4, 10, 11, 0, 4, 11, 5, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B10BE8[] = {
    0x17000000, 0x0000000C, 0x00000000, 0x00000000, D_84B10A40, 0x05000000,
    0x1B000000, 0x00000000, 0x00000000, 0xFFEE0000, 0x08000000, BattlePrepBanner_ApplyRoundLabelSpin,
    0x00000000, 0x05000000, 0x22050000, 0x00000000, 0x08000000, BattlePrepBanner_DrawRoundLabel,
    D_84B10B00, 0x06000000, 0x06000000, 0x06000000, 0x01000000, 0x00000000,
};
static Vtx D_84B10C48[] = {
    VTX(-64, 16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(0, 16, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 16, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(64, 16, 0, 4096, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-64, -16, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(0, -16, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, -16, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(64, -16, 0, 4096, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B10CC8[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIM_LOD_FRAC, 0, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B10C48, 8, 0),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 128, 0x0F000000),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadTile(G_TX_LOADTILE, 0, 0, 0x00FC, 0x007C),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(G_TX_RENDERTILE, 0, 0, 0x00FC, 0x007C),
    gsSP2Triangles(0, 4, 5, 0, 0, 5, 1, 0),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 128, 0x0F000000),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadTile(G_TX_LOADTILE, 0x0100, 0, 0x01FC, 0x007C),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(G_TX_RENDERTILE, 0x0100, 0, 0x01FC, 0x007C),
    gsSP2Triangles(2, 6, 7, 0, 2, 7, 3, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B10D70[] = {
    0x17000000, 0x00000008, 0x00000000, 0x00000000, D_84B10C48, 0x05000000,
    0x1B000000, 0x00000000, 0x00000000, 0xFFEE0000, 0x08000000, BattlePrepBanner_ApplyRoundLabelSpin,
    0x00000000, 0x05000000, 0x22050000, 0x00000000, 0x08000000, BattlePrepBanner_DrawRoundLabel,
    D_84B10CC8, 0x06000000, 0x06000000, 0x06000000, 0x01000000, 0x00000000,
};
static Gfx D_84B10DD0[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIM_LOD_FRAC, 0, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B10C48, 8, 0),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 128, 0x0302FA70),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadTile(G_TX_LOADTILE, 0, 0, 0x00FC, 0x007C),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(G_TX_RENDERTILE, 0, 0, 0x00FC, 0x007C),
    gsSP2Triangles(0, 4, 5, 0, 0, 5, 1, 0),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 128, 0x0302FA70),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadTile(G_TX_LOADTILE, 0x0100, 0, 0x01FC, 0x007C),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(G_TX_RENDERTILE, 0x0100, 0, 0x01FC, 0x007C),
    gsSP2Triangles(2, 6, 7, 0, 2, 7, 3, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B10E78[] = {
    0x17000000, 0x00000008, 0x00000000, 0x00000000, D_84B10C48, 0x05000000, 0x1B000000, 0x00000000,
    0x00000000, 0xFFEE0000, 0x05000000, 0x22050000, D_84B10DD0, 0x06000000, 0x06000000, 0x01000000,
};
static Vtx D_84B10EB8[] = {
    VTX(-80, 16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),        VTX(-16, 16, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-16, 16, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(48, 16, 0, 4096, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(48, 16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),         VTX(80, 16, 0, 1024, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-80, -16, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),    VTX(-16, -16, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-16, -16, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(48, -16, 0, 4096, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(48, -16, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(80, -16, 0, 1024, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B10F78[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIM_LOD_FRAC, 0, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B10EB8, 12, 0),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 128, 0x0F000000),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadTile(G_TX_LOADTILE, 0, 0, 0x00FC, 0x007C),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(G_TX_RENDERTILE, 0, 0, 0x00FC, 0x007C),
    gsSP2Triangles(0, 6, 7, 0, 0, 7, 1, 0),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 128, 0x0F000000),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadTile(G_TX_LOADTILE, 0x0100, 0, 0x01FC, 0x007C),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0x0000, G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(G_TX_RENDERTILE, 0x0100, 0, 0x01FC, 0x007C),
    gsSP2Triangles(2, 8, 9, 0, 2, 9, 3, 0),
    gsDPLoadTextureBlock(0x0E000000, G_IM_FMT_IA, G_IM_SIZ_8b, 32, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 10, 11, 0, 4, 11, 5, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B11060[] = {
    0x17000000, 0x0000000C, 0x00000000, 0x00000000, D_84B10EB8, 0x05000000,
    0x1B000000, 0x00000000, 0x00000000, 0xFFEE0000, 0x08000000, BattlePrepBanner_ApplyRoundLabelSpin,
    0x00000000, 0x05000000, 0x22050000, 0x00000000, 0x08000000, BattlePrepBanner_DrawRoundLabel,
    D_84B10F78, 0x06000000, 0x06000000, 0x01000000, 0x00000000, 0x00000000,
};
static Vtx D_84B110C0[] = {
    VTX(-32, 24, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-32, 0, 0, 0, 768, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, 0, 0, 2048, 768, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(32, 24, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, 0, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-32, -24, 0, 0, 768, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -24, 0, 2048, 768, 0xFF, 0xFF, 0xFF, 0xFF), VTX(32, 0, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-22, 22, 0, 128, 128, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(-22, 0, 0, 128, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(22, 0, 0, 1920, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(22, 22, 0, 1920, 128, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-22, 0, 0, 128, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(-22, -22, 0, 128, 896, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(22, -22, 0, 1920, 896, 0xFF, 0xFF, 0xFF, 0xFF), VTX(22, 0, 0, 1920, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Vtx D_84B111C0[] = {
    VTX(-32, 24, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-32, 16, 0, 0, 256, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-24, 16, 0, 256, 256, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(-24, 24, 0, 256, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(24, 24, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(24, 16, 0, 0, 256, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, 16, 0, 256, 256, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(32, 24, 0, 256, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-24, 24, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-24, 16, 0, 0, 256, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(24, 16, 0, 0, 256, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(24, 24, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, -16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(-32, -24, 0, 0, 256, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-24, -24, 0, 256, 256, 0xFF, 0xFF, 0xFF, 0xFF), VTX(-24, -16, 0, 256, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, 16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-32, -16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-24, -16, 0, 256, 0, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(-24, 16, 0, 256, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(24, -16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(24, -24, 0, 0, 256, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -24, 0, 256, 256, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(32, -16, 0, 256, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-24, -16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(-24, -24, 0, 0, 256, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(24, -24, 0, 0, 256, 0xFF, 0xFF, 0xFF, 0xFF),    VTX(24, -16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(24, 16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(24, -16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -16, 0, 256, 0, 0xFF, 0xFF, 0xFF, 0xFF),    VTX(32, 16, 0, 256, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B113C0[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B110C0, 16, 0),
    gsDPSetCombineLERP(0, 0, 0, TEXEL0, 0, 0, 0, TEXEL0, 0, 0, 0, COMBINED, COMBINED, 0, PRIM_LOD_FRAC, 0),
    gsDPLoadTextureBlock(D_3010800, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 24, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 6, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureBlock(D_3011400, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 24, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 6, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsDPSetCombineLERP(0, 0, 0, TEXEL0, 0, 0, 0, SHADE, 0, 0, 0, COMBINED, COMBINED, 0, PRIM_LOD_FRAC, 0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPLoadTextureBlock(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 6, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsDPLoadTextureBlock(0x0F001000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 6, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),
    gsSPEndDisplayList(),
};
static Gfx D_84B114F8[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, COMBINED,
                       COMBINED, 0, PRIM_LOD_FRAC, 0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B111C0, 32, 0),
    gsDPLoadTextureBlock(D_3018780, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureBlock(D_30187C0, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsDPLoadTextureBlock(D_3018700, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),
    gsSP2Triangles(16, 17, 18, 0, 16, 18, 19, 0),
    gsDPLoadTextureBlock(D_3018740, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(20, 21, 22, 0, 20, 22, 23, 0),
    gsSP2Triangles(24, 25, 26, 0, 24, 26, 27, 0),
    gsSP2Triangles(28, 29, 30, 0, 28, 30, 31, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B11640[] = {
    0x17000000,
    0x00000000,
    0x00000000,
    0x00000000,
    D_84B110C0,
    0x05000000,
    0x1B000000,
    0x00000000,
    0x00000000,
    0x00180000,
    0x05000000,
    0x22050000,
    0x00000000,
    0x08000000,
    OpponentLadder_DrawCupSlot,
    0x00000000,
    0x22050000,
    0x00000000,
    0x08000000,
    OpponentLadder_DrawCupHighlight,
    0x00000000,
    0x06000000,
    0x1B000000,
    0x00000000,
    0x00000000,
    0xFFE80000,
    0x05000000,
    0x22050000,
    0x00000000,
    0x08000000,
    OpponentLadder_DrawCupSlot,
    0x00000001,
    0x22050000,
    0x00000000,
    0x08000000,
    OpponentLadder_DrawCupHighlight,
    0x00000001,
    0x06000000,
    0x06000000,
    0x01000000,
};
static Vtx D_84B116E0[] = {
    VTX(-32, 48, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-32, 16, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, 16, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(32, 48, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, 16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-32, -16, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -16, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(32, 16, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, -16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-32, -48, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -48, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(32, -16, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Vtx D_84B117A0[] = {
    VTX(-32, 48, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-32, 40, 0, 0, 256, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-24, 40, 0, 256, 256, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(-24, 48, 0, 256, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(24, 48, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(24, 40, 0, 0, 256, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, 40, 0, 256, 256, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(32, 48, 0, 256, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-24, 48, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-24, 40, 0, 0, 256, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(24, 40, 0, 0, 256, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(24, 48, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, -40, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(-32, -48, 0, 0, 256, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-24, -48, 0, 256, 256, 0xFF, 0xFF, 0xFF, 0xFF), VTX(-24, -40, 0, 256, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, 40, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-32, -40, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-27, -40, 0, 160, 0, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(-27, 40, 0, 160, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(24, -40, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(24, -48, 0, 0, 256, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -48, 0, 256, 256, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(32, -40, 0, 256, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-24, -40, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(-24, -48, 0, 0, 256, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(24, -48, 0, 0, 256, 0xFF, 0xFF, 0xFF, 0xFF),    VTX(24, -40, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(27, 40, 0, 96, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(27, -40, 0, 96, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -40, 0, 256, 0, 0xFF, 0xFF, 0xFF, 0xFF),    VTX(32, 40, 0, 256, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B119A0[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B116E0, 20, 0),
    gsDPSetCombineLERP(0, 0, 0, TEXEL0, 0, 0, 0, TEXEL0, 0, 0, 0, COMBINED, COMBINED, 0, PRIM_LOD_FRAC, 0),
    gsDPLoadTextureBlock(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 6, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureBlock(0x0F001000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 6, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsDPLoadTextureBlock(0x0F002000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 6, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsSPEndDisplayList(),
};
static Gfx D_84B11A88[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, COMBINED,
                       COMBINED, 0, PRIM_LOD_FRAC, 0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B117A0, 32, 0),
    gsDPLoadTextureBlock(D_3018780, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureBlock(D_30187C0, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsDPLoadTextureBlock(D_3018700, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),
    gsSP2Triangles(16, 17, 18, 0, 16, 18, 19, 0),
    gsDPLoadTextureBlock(D_3018740, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(20, 21, 22, 0, 20, 22, 23, 0),
    gsSP2Triangles(24, 25, 26, 0, 24, 26, 27, 0),
    gsSP2Triangles(28, 29, 30, 0, 28, 30, 31, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B11BD0[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B116E0,
    0x05000000, 0x22050000, 0x00000000, 0x08000000, OpponentLadder_DrawCastleSlot,
    D_84B119A0, 0x22050000, 0x00000000, 0x08000000, OpponentLadder_DrawCastleHighlight,
    D_84B11A88, 0x06000000, 0x01000000,
};

/**
 * Geo draw callback (event 5) for banner object 0: binds the plaque background (plateTexture, a
 * 256x96 RGBA16 image from battle_headers) to segment 0x0F, then draws the node's display list.
 */
s32 BattlePrepBanner_DrawPlate(s32 event, unk_func_80011B94* display_node) {
    BattlePrepBanner* banner;

    if (event == 5) {
        banner = (BattlePrepBanner*)D_8006F09C->node.callbackArg;

        gDPPipeSync(gDisplayListHead++);
        gSPSegment(gDisplayListHead++, 0x0F, (u32)banner->plateTexture & 0x1FFFFFFF);
        gSPDisplayList(gDisplayListHead++, display_node->display_node_header.callbackArg);

        GeoRender_ApplyMaterialState();
    }
    return 0;
}

/**
 * Geo draw callback (event 5) for the banner title: binds titleTexture (the cup or gym name, e.g.
 * "PEWTER GYM BATTLE") to segment 0x0F, then draws the node's display list. Used by both the plain
 * title layout and the round 2 layout that adds the "R-2" icon.
 */
s32 BattlePrepBanner_DrawTitle(s32 event, unk_func_80011B94* display_node) {
    BattlePrepBanner* banner;

    if (event == 5) {
        banner = (BattlePrepBanner*)D_8006F09C->node.callbackArg;

        gDPPipeSync(gDisplayListHead++);
        gSPSegment(gDisplayListHead++, 0x0F, (u32)banner->titleTexture & 0x1FFFFFFF);
        gSPDisplayList(gDisplayListHead++, display_node->display_node_header.callbackArg);

        GeoRender_ApplyMaterialState();
    }
    return 0;
}

/**
 * Geo draw callback (event 5) for the banner round label: binds roundLabelTexture ("BATTLE",
 * "TRAINER", "LEADER", "SEMIFINAL", ...) to segment 0x0F and roundNumberTexture (the digit) to
 * segment 0x0E, then draws the node's display list.
 */
s32 BattlePrepBanner_DrawRoundLabel(s32 event, unk_func_80011B94* display_node) {
    BattlePrepBanner* banner;

    if (event == 5) {
        banner = (BattlePrepBanner*)D_8006F09C->node.callbackArg;

        gDPPipeSync(gDisplayListHead++);
        gSPSegment(gDisplayListHead++, 0x0F, (u32)banner->roundLabelTexture & 0x1FFFFFFF);
        gSPSegment(gDisplayListHead++, 0x0E, (u32)banner->roundNumberTexture & 0x1FFFFFFF);
        gSPDisplayList(gDisplayListHead++, display_node->display_node_header.callbackArg);

        GeoRender_ApplyMaterialState();
    }
    return 0;
}

/**
 * Geo draw callback (event 5) for the banner division line: binds divisionTexture ("POKé BALL",
 * "GREAT BALL", ...) to segment 0x0F, then draws the node's display list.
 */
s32 BattlePrepBanner_DrawDivision(s32 event, unk_func_80011B94* display_node) {
    BattlePrepBanner* banner;

    if (event == 5) {
        banner = (BattlePrepBanner*)D_8006F09C->node.callbackArg;

        gDPPipeSync(gDisplayListHead++);
        gSPSegment(gDisplayListHead++, 0x0F, (u32)banner->divisionTexture & 0x1FFFFFFF);
        gSPDisplayList(gDisplayListHead++, display_node->display_node_header.callbackArg);

        GeoRender_ApplyMaterialState();
    }
    return 0;
}

/**
 * Geo callback (event 2) on the round label's rotate node: sets its rotationZ from
 * roundLabelRotationZ[], indexed by which of the four round label copies (objects 9-12) is drawing.
 * This is what makes "BATTLE 1" / "TRAINER 1" spin into place.
 */
s32 BattlePrepBanner_ApplyRoundLabelSpin(s32 event, unk_D_86002F34_alt5* rotate_node) {
    BattlePrepBanner* banner;

    if (event == 2) {
        banner = (BattlePrepBanner*)D_8006F09C->node.callbackArg;

        rotate_node->unk_24.z = banner->roundLabelRotationZ[(D_8006F09C->effectSlot - banner->displayObjects->effectSlot) - 9];
    }
    return 0;
}

/**
 * Round intro step 1: slides the plaque background (object 0) in from the left, 32 units per frame
 * for 10 frames, from x = -288 to x = 0. Plays sound 0x01100001 on the first frame.
 */
void BattlePrepBanner_SlidePlateIn(BattlePrepBanner* banner, s16 frame) {
    if (frame == 0) {
        Audio_PlayCategory11SoundCommand(0x01100001, 0, 0);
    }

    if (frame < 0xA) {
        banner->displayObjects->position.x = (frame << 5) - 0x120;
    }
}

/**
 * Round intro step 2 (NTSC timing): slides the title in from the left at 20 units per frame. The
 * lead copy (objects 7 and 8) moves; the three trailing copies (objects 1-6, faded) take the lead's
 * previous positions each frame, which draws a motion-blur trail. Object 7 holds only the title's
 * top-row vertices, so once it arrives its damped sine sway shears the text left and right before it
 * settles. Plays sound 0x01100002 on frame 2.
 */
void BattlePrepBanner_SlideTitleInNTSC(BattlePrepBanner* banner, s16 frame) {
    s32 i;

    if (frame == 2) {
        Audio_PlayCategory11SoundCommand(0x01100002, 0, 0);
    }

    for (i = 1; i < 7; i += 2) {
        banner->displayObjects[i].position = banner->displayObjects[2 + i].position;
        banner->displayObjects[1 + i].position = banner->displayObjects[3 + i].position;

        banner->displayObjects[i].scale = banner->displayObjects[2 + i].scale;
        banner->displayObjects[1 + i].scale = banner->displayObjects[3 + i].scale;
    }

    if (frame < 0x12) {
        banner->displayObjects[7].position.x = (frame * 0x14) - 0x154;
    }

    if (frame < 0x10) {
        banner->displayObjects[8].position.x = (frame * 0x14) - 0x12C;
    }

    if ((frame >= 0x12) && (frame < 0x2B)) {
        banner->displayObjects[7].position.x = SINS((frame * 0x1555) + 0xFFFE955B) * (s16)(0x54 - (frame * 2));
    }
}

/**
 * PAL timing variant of BattlePrepBanner_SlideTitleInNTSC: the title slides in at 24 units per
 * frame with shorter frame windows.
 */
void BattlePrepBanner_SlideTitleInPAL(BattlePrepBanner* banner, s16 frame) {
    s32 i;

    if (frame == 2) {
        Audio_PlayCategory11SoundCommand(0x01100002, 0, 0);
    }

    for (i = 1; i < 7; i += 2) {
        banner->displayObjects[i].position = banner->displayObjects[2 + i].position;
        banner->displayObjects[1 + i].position = banner->displayObjects[3 + i].position;

        banner->displayObjects[i].scale = banner->displayObjects[2 + i].scale;
        banner->displayObjects[1 + i].scale = banner->displayObjects[3 + i].scale;
    }

    if (frame < 0xF) {
        banner->displayObjects[7].position.x = (frame * 0x18) - 0x150;
    }

    if (frame < 0xD) {
        banner->displayObjects[8].position.x = (frame * 0x18) - 0x120;
    }

    if ((frame >= 0xF) && (frame < 0x28)) {
        banner->displayObjects[7].position.x = SINS((frame * 0x1555) + 0xFFFED55A) * (s16)(0x4E - (frame * 2));
    }
}

/**
 * Round intro step 3 (NTSC timing): the round label (lead copy object 12) spins about Z while
 * growing from 1/16 to full size, trailed by copies 9-11, and the division line (lead copy object 16,
 * cups with divisions only) grows with it. On the last spin frame the title (objects 7 and 8) joins
 * the growth, then the title, round label and division bounce with a damped sine scale. Plays sound
 * 0x01100003 on frame 0 and 0x01100004 on frame 16.
 */
void BattlePrepBanner_SpinRoundLabelInNTSC(BattlePrepBanner* banner, s16 frame) {
    UNUSED s32 pad;
    s32 i;

    if (frame == 0) {
        Audio_PlayCategory11SoundCommand(0x01100003, 0, 0);
    }

    if (frame == 0x10) {
        Audio_PlayCategory11SoundCommand(0x01100004, 0, 0);
    }

    for (i = 0; i < 3; i++) {
        banner->displayObjects[9 + i].position = banner->displayObjects[10 + i].position;
        banner->displayObjects[9 + i].scale = banner->displayObjects[10 + i].scale;

        banner->roundLabelRotationZ[i] = banner->roundLabelRotationZ[i + 1];

        banner->displayObjects[13 + i].position = banner->displayObjects[14 + i].position;
        banner->displayObjects[13 + i].scale = banner->displayObjects[14 + i].scale;
    }

    if (frame < 0x12) {
        f32 grow_scale = (frame + 1) * 0.0625f;
        s32 spin_angle = (frame << 0xC) - 0x1000;

        banner->roundLabelRotationZ[3] = spin_angle;
        banner->displayObjects[12].position.x = 0.0f;
        banner->displayObjects[16].position.x = 0.0f;

        Vec3f_SetComponentsDuplicate(&banner->displayObjects[12].scale, grow_scale, grow_scale, grow_scale);
        Vec3f_SetComponentsDuplicate(&banner->displayObjects[16].scale, grow_scale, grow_scale, grow_scale);
        if (spin_angle >= 0xF000) {
            Vec3f_SetComponentsDuplicate(&banner->displayObjects[7].scale, grow_scale, grow_scale, grow_scale);
            Vec3f_SetComponentsDuplicate(&banner->displayObjects[8].scale, grow_scale, grow_scale, grow_scale);
        }
    } else if (frame < 0x1F) {
        f32 bounce_scale = (SINS((frame << 0xD) + 0xFFFE0000) * ((0x12C - (frame * 0xA)) / 1000.0f)) + 1.0f;

        Vec3f_SetComponentsDuplicate(&banner->displayObjects[7].scale, bounce_scale, bounce_scale, bounce_scale);
        Vec3f_SetComponentsDuplicate(&banner->displayObjects[8].scale, bounce_scale, bounce_scale, bounce_scale);
        Vec3f_SetComponentsDuplicate(&banner->displayObjects[12].scale, bounce_scale, bounce_scale, bounce_scale);
        Vec3f_SetComponentsDuplicate(&banner->displayObjects[16].scale, bounce_scale, bounce_scale, bounce_scale);
    }
}

/**
 * PAL timing variant of BattlePrepBanner_SpinRoundLabelInNTSC: the spin and growth take 13 frames
 * and sound 0x01100004 plays on frame 13.
 */
void BattlePrepBanner_SpinRoundLabelInPAL(BattlePrepBanner* banner, s16 frame) {
    UNUSED s32 pad;
    s32 i;

    if (frame == 0) {
        Audio_PlayCategory11SoundCommand(0x01100003, 0, 0);
    }

    if (frame == 0xD) {
        Audio_PlayCategory11SoundCommand(0x01100004, 0, 0);
    }

    for (i = 0; i < 3; i++) {
        banner->displayObjects[9 + i].position = banner->displayObjects[10 + i].position;
        banner->displayObjects[9 + i].scale = banner->displayObjects[10 + i].scale;

        banner->roundLabelRotationZ[i] = banner->roundLabelRotationZ[i + 1];

        banner->displayObjects[13 + i].position = banner->displayObjects[14 + i].position;
        banner->displayObjects[13 + i].scale = banner->displayObjects[14 + i].scale;
    }

    if (frame < 0xF) {
        f32 grow_scale = (frame + 1) / 13.0f;
        s32 spin_angle = (frame * 0x1333) - 0xCCA;

        banner->roundLabelRotationZ[3] = spin_angle;
        banner->displayObjects[12].position.x = 0.0f;
        banner->displayObjects[16].position.x = 0.0f;

        Vec3f_SetComponentsDuplicate(&banner->displayObjects[12].scale, grow_scale, grow_scale, grow_scale);
        Vec3f_SetComponentsDuplicate(&banner->displayObjects[16].scale, grow_scale, grow_scale, grow_scale);

        if (spin_angle >= 0xECCD) {
            Vec3f_SetComponentsDuplicate(&banner->displayObjects[7].scale, grow_scale, grow_scale, grow_scale);
            Vec3f_SetComponentsDuplicate(&banner->displayObjects[8].scale, grow_scale, grow_scale, grow_scale);
        }
    } else if (frame < 0x1C) {
        f32 bounce_scale = (SINS((frame << 0xD) + 0xFFFE6000) * ((0x10E - (frame * 0xA)) / 1000.0f)) + 1.0f;

        Vec3f_SetComponentsDuplicate(&banner->displayObjects[7].scale, bounce_scale, bounce_scale, bounce_scale);
        Vec3f_SetComponentsDuplicate(&banner->displayObjects[8].scale, bounce_scale, bounce_scale, bounce_scale);
        Vec3f_SetComponentsDuplicate(&banner->displayObjects[12].scale, bounce_scale, bounce_scale, bounce_scale);
        Vec3f_SetComponentsDuplicate(&banner->displayObjects[16].scale, bounce_scale, bounce_scale, bounce_scale);
    }
}

/**
 * State 1 update: plays the round intro animation, choosing NTSC or PAL timing from osTvType. The
 * plate slides in first, the title from frame 2, and the round label spins in from frame 30 (27 on
 * PAL). On frame 220 all 17 objects are parked off-screen at x = -320.
 */
void BattlePrepBanner_UpdateRoundIntro(BattlePrepBanner* banner) {
    s32 i;

    if (osTvType == OS_TV_PAL) {
        if (banner->timer >= 0) {
            BattlePrepBanner_SlidePlateIn(banner, banner->timer);
        }

        if (banner->timer >= 2) {
            BattlePrepBanner_SlideTitleInPAL(banner, banner->timer - 2);
        }

        if (banner->timer >= 0x1B) {
            BattlePrepBanner_SpinRoundLabelInPAL(banner, banner->timer - 0x1B);
        }
    } else {
        if (banner->timer >= 0) {
            BattlePrepBanner_SlidePlateIn(banner, banner->timer);
        }

        if (banner->timer >= 2) {
            BattlePrepBanner_SlideTitleInNTSC(banner, banner->timer - 2);
        }

        if (banner->timer >= 0x1E) {
            BattlePrepBanner_SpinRoundLabelInNTSC(banner, banner->timer - 0x1E);
        }
    }

    if (banner->timer == 0xDC) {
        for (i = 0; i < 17; i++) {
            banner->displayObjects[i].position.x = -320.0f;
        }
    }

    banner->timer++;
}

/**
 * State 3 update: slides the whole banner (plate, title, round label and division) off to the right
 * at 32 units per frame with trailing copies. Once the plate passes x = 416 all 17 objects are reset
 * and the banner goes idle. Plays sound 0x01100005 on the first frame.
 */
void BattlePrepBanner_UpdateSlideOut(BattlePrepBanner* banner) {
    s32 i;

    if (banner->timer++ == 0) {
        Audio_PlayCategory11SoundCommand(0x01100005, 0, 0);
    }

    for (i = 0; i < 5; i += 2) {
        banner->displayObjects[1 + i].position = banner->displayObjects[3 + i].position;
        banner->displayObjects[2 + i].position = banner->displayObjects[4 + i].position;
    }

    for (i = 0; i < 3; i++) {
        banner->displayObjects[9 + i].position = banner->displayObjects[10 + i].position;
        banner->displayObjects[13 + i].position = banner->displayObjects[14 + i].position;
    }

    banner->displayObjects[0].position.x += 32.0f;
    banner->displayObjects[7].position.x += 32.0f;
    banner->displayObjects[8].position.x += 32.0f;
    banner->displayObjects[12].position.x += 32.0f;
    banner->displayObjects[16].position.x += 32.0f;

    if (banner->displayObjects->position.x >= 416.0f) {
        for (i = 0; i < 17; i++) {
            BattlePrep_ResetDisplayObject(&banner->displayObjects[i]);
        }
        banner->state = 0;
    }
}

/**
 * State 2 update for the castle gym-clear plaque ("PEWTER GYM BATTLE / CLEAR"): for 10 frames the
 * plate slides in from the left while the title and the CLEAR label slide in from the right, at 32
 * units per frame with trailing copies. Goes idle on frame 32. Plays sounds 0x01100001 and
 * 0x01100002 on the first frame.
 */
void BattlePrepBanner_UpdateClearSlideIn(BattlePrepBanner* banner) {
    s32 i;

    if (banner->timer == 0) {
        Audio_PlayCategory11SoundCommand(0x01100001, 0, 0);
        Audio_PlayCategory11SoundCommand(0x01100002, 0, 0);
    }

    for (i = 0; i < 5; i += 2) {
        banner->displayObjects[1 + i].position = banner->displayObjects[3 + i].position;
        banner->displayObjects[2 + i].position = banner->displayObjects[4 + i].position;
    }

    for (i = 0; i < 3; i++) {
        banner->displayObjects[9 + i].position = banner->displayObjects[10 + i].position;
    }

    if (banner->timer < 0xA) {
        banner->displayObjects[0].position.x += 32.0f;
        banner->displayObjects[7].position.x -= 32.0f;
        banner->displayObjects[8].position.x -= 32.0f;
        banner->displayObjects[12].position.x -= 32.0f;
    }

    if (banner->timer == 0x20) {
        banner->state = 0;
    }
    banner->timer++;
}

/**
 * Runs the per-frame banner update for the current state: 1 round intro, 2 CLEAR plaque slide-in,
 * 3 slide-out; 0 is idle.
 */
void BattlePrepBanner_Update(BattlePrepBanner* banner) {
    switch (banner->state) {
        case 1:
            BattlePrepBanner_UpdateRoundIntro(banner);
            break;

        case 3:
            BattlePrepBanner_UpdateSlideOut(banner);
            break;

        case 2:
            BattlePrepBanner_UpdateClearSlideIn(banner);
            break;
    }
}

/**
 * Sets up object 0 as the plaque background (plateLayout) at (x, y, -289).
 */
void BattlePrepBanner_InitPlate(BattlePrepBanner* banner, s16 x, s16 y) {
    Model_InitDisplayObject(banner->displayObjects, 0, 0, banner->plateLayout);
    Vec3f_SetComponentsDuplicate(&banner->displayObjects->position, x, y, -289.0f);
}

/**
 * Sets up one copy of the title at (x, y, -289) as the object pair 2 * copy + 1 (titleTopLayout)
 * and 2 * copy + 2 (titleBottomLayout). Copy 3 is the opaque lead; copies 0-2 are the fading motion
 * trail, with alpha 80 + 40 * copy.
 */
void BattlePrepBanner_InitTitleCopy(BattlePrepBanner* banner, s16 copy, s16 x, s16 y) {
    s16 object_index;
    s16 is_lead;

    if (copy == 3) {
        is_lead = 1;
    } else {
        is_lead = 0;
    }
    object_index = (copy * 2) + 1;
    if (is_lead) {}

    Model_InitDisplayObject(&banner->displayObjects[object_index], is_lead, 0, banner->titleTopLayout);
    Model_InitDisplayObject(&banner->displayObjects[object_index + 1], is_lead, 0, banner->titleBottomLayout);
    Vec3f_SetComponentsDuplicate(&banner->displayObjects[object_index].position, x, y, -289.0f);
    Vec3f_SetComponentsDuplicate(&banner->displayObjects[object_index + 1].position, x, y, -289.0f);

    if (copy == 3) {
        banner->displayObjects[object_index].lodFraction = 0xFF;
        banner->displayObjects[object_index + 1].lodFraction = 0xFF;
    } else {
        banner->displayObjects[object_index].lodFraction = (copy * 0x28) + 0x50;
        banner->displayObjects[object_index + 1].lodFraction = (copy * 0x28) + 0x50;
    }
}

/**
 * Sets up round label copy `copy` as object 9 + copy (roundLabelLayout), off-screen at
 * (-320, 48, -289). Copy 3 is the opaque lead; copies 0-2 are the trail, with alpha 80 + 40 * copy.
 */
void BattlePrepBanner_InitRoundLabelCopy(BattlePrepBanner* banner, s16 copy) {
    s16 object_index = copy + 9;
    s16 is_lead;

    if (copy == 3) {
        is_lead = 1;
    } else {
        is_lead = 0;
    }

    Model_InitDisplayObject(&banner->displayObjects[object_index], is_lead, 0, banner->roundLabelLayout);
    Vec3f_SetComponentsDuplicate(&banner->displayObjects[object_index].position, -320.0f, 48.0f, -289.0f);

    if (copy == 3) {
        banner->displayObjects[object_index].lodFraction = 0xFF;
    } else {
        banner->displayObjects[object_index].lodFraction = (copy * 0x28) + 0x50;
    }
}

/**
 * Sets up CLEAR label copy `copy` as object 9 + copy (clearLabelLayout), off-screen at
 * (320, 0, -289). Copy 3 is the opaque lead; copies 0-2 are the trail, with alpha 80 + 40 * copy.
 */
void BattlePrepBanner_InitClearLabelCopy(BattlePrepBanner* banner, s16 copy) {
    s16 object_index = copy + 9;
    s16 is_lead;

    if (copy == 3) {
        is_lead = 1;
    } else {
        is_lead = 0;
    }

    Model_InitDisplayObject(&banner->displayObjects[object_index], is_lead, 0, banner->clearLabelLayout);
    Vec3f_SetComponentsDuplicate(&banner->displayObjects[object_index].position, 320.0f, 0.0f, -289.0f);

    if (copy == 3) {
        banner->displayObjects[object_index].lodFraction = 0xFF;
    } else {
        banner->displayObjects[object_index].lodFraction = (copy * 0x28) + 0x50;
    }
}

/**
 * Sets up division line copy `copy` as object 13 + copy (divisionLayout), off-screen at
 * (-320, 48, -289). Copy 3 is the opaque lead; copies 0-2 are the trail, with alpha 80 + 40 * copy.
 */
void BattlePrepBanner_InitDivisionCopy(BattlePrepBanner* banner, s16 copy) {
    s16 object_index = copy + 0xD;
    s16 is_lead;

    if (copy == 3) {
        is_lead = 1;
    } else {
        is_lead = 0;
    }

    Model_InitDisplayObject(&banner->displayObjects[object_index], is_lead, 0, banner->divisionLayout);
    Vec3f_SetComponentsDuplicate(&banner->displayObjects[object_index].position, -320.0f, 48.0f, -289.0f);

    if (copy == 3) {
        banner->displayObjects[object_index].lodFraction = 0xFF;
    } else {
        banner->displayObjects[object_index].lodFraction = (copy * 0x28) + 0x50;
    }
}

/**
 * Starts the round intro banner (state 1) on 17 display objects starting at display_objects:
 * plate (0), title copies (1-8), round label copies (9-12) and, only in modes 3 and 6 (the cups
 * with Poké Ball to Master Ball divisions), division copies (13-16). Every object's callbackArg
 * points at the banner so the geo callbacks can find its textures.
 */
void BattlePrepBanner_StartRoundIntro(BattlePrepBanner* banner, DisplayObject* display_objects) {
    s32 i;

    banner->displayObjects = display_objects;
    banner->timer = 0;
    banner->state = 1;
    BattlePrepBanner_InitPlate(banner, -0x140, 0x30);

    for (i = 0; i < 4; i++) {
        BattlePrepBanner_InitTitleCopy(banner, i, -0x140, 0x30);
        BattlePrepBanner_InitRoundLabelCopy(banner, i);
        if ((gSessionContext.mode == 3) || (gSessionContext.mode == 6)) {
            BattlePrepBanner_InitDivisionCopy(banner, i);
        }
    }

    for (i = 0; i < 17; i++) {
        banner->displayObjects[i].node.callbackArg = banner;
    }
}

/**
 * Resets the frame timer and switches the banner to state 3 (slide out to the right).
 */
void BattlePrepBanner_StartSlideOut(BattlePrepBanner* banner) {
    banner->timer = 0;
    banner->state = 3;
}

/**
 * Starts the castle gym-clear plaque (state 2) on 13 display objects starting at display_objects:
 * plate (0), title copies (1-8) and CLEAR label copies (9-12). Every object's callbackArg points at
 * the banner so the geo callbacks can find its textures.
 */
void BattlePrepBanner_StartClearPlaque(BattlePrepBanner* banner, DisplayObject* display_objects) {
    s32 i;

    banner->displayObjects = display_objects;
    banner->timer = 0;
    banner->state = 2;
    BattlePrepBanner_InitPlate(banner, -0x140, 0);

    for (i = 0; i < 4; i++) {
        BattlePrepBanner_InitTitleCopy(banner, i, 0x140, 0);
        BattlePrepBanner_InitClearLabelCopy(banner, i);
    }

    for (i = 0; i < 13; i++) {
        banner->displayObjects[i].node.callbackArg = banner;
    }
}

/**
 * Picks the banner textures for the current mode, division or castle location, and round:
 * - plateTexture: a battle_headers image (D_84B0FC40 / D_84B0FC48).
 * - titleTexture: the cup name (D_84B0FC54) or castle title (D_84B0FC6C).
 * - divisionTexture (cups): the division name (D_84B0FCAC).
 * - roundLabelTexture: "BATTLE", "SEMIFINAL" (round 7) or "FINAL BATTLE" (round 8) for cups;
 *   "TRAINER", "LEADER" (round 4), "ELITE FOUR" or "RIVAL" in the castle.
 * - roundNumberTexture: the round digit, set only for rounds below 7.
 */
void BattlePrepBanner_LoadTextures(BattlePrepBanner* banner) {
    BinArchive* headers_archive = ASSET_LOAD2(battle_headers, 1, 1);

    if (gSessionContext.mode == 7) {
        banner->plateTexture = BinArchive_GetFile(headers_archive, D_84B0FC48[gSessionContext.modeSelection]);
        // modeSelection 9 (rival) reads one past the end of D_84B0FC6C, into D_84B0FC90[0] ("FINAL BATTLE").
        banner->titleTexture = Util_ConvertAddrToVirtAddr(D_84B0FC6C[gSessionContext.modeSelection]);

        switch (gSessionContext.modeSelection) {
            default:
                if (gSessionContext.roundOrDemoId == 4) {
                    banner->roundLabelTexture = Util_ConvertAddrToVirtAddr(D_302CA70);
                } else {
                    banner->roundLabelTexture = Util_ConvertAddrToVirtAddr(D_302BA70);
                }
                break;

            case 8:
                banner->roundLabelTexture = Util_ConvertAddrToVirtAddr(D_302DA70);
                break;

            case 9:
                banner->roundLabelTexture = Util_ConvertAddrToVirtAddr(D_302EA70);
                break;
        }
    } else {
        banner->plateTexture = BinArchive_GetFile(headers_archive, D_84B0FC40[gSessionContext.mode - 1]);
        banner->titleTexture = Util_ConvertAddrToVirtAddr(D_84B0FC54[gSessionContext.mode - 1]);
        banner->divisionTexture = Util_ConvertAddrToVirtAddr(D_84B0FCAC[gSessionContext.modeSelection]);

        switch (gSessionContext.roundOrDemoId) {
            default:
                banner->roundLabelTexture = Util_ConvertAddrToVirtAddr(D_302AA70);
                break;

            case 7:
                banner->roundLabelTexture = Util_ConvertAddrToVirtAddr(D_30497F0);
                break;

            case 8:
                banner->roundLabelTexture = Util_ConvertAddrToVirtAddr(D_3048210);
                break;
        }
    }

    if (gSessionContext.roundOrDemoId < 7) {
        banner->roundNumberTexture = Util_ConvertAddrToVirtAddr(D_84B0FC90[gSessionContext.roundOrDemoId]);
    }
}

/**
 * For the round 2 title layout: measures the empty columns on the right of the title texture
 * (200-wide IA8, 28 rows), then moves the "R-2" icon quad (vertices 0, 1, 8 and 9 of
 * titleTopLayout's vertex array) left by that width, minus 4, so the icon sits just after the
 * title text.
 */
void BattlePrepBanner_FitRound2Icon(BattlePrepBanner* banner) {
    unk_D_86002F34* title_layout;
    s32 blank_width;
    s32 searching;
    u8* texel;
    u8* column;
    Vtx* vertices;
    s32 i;

    blank_width = -1;
    searching = 1;
    column = (u8*)banner->titleTexture + 0xC7;
    title_layout = banner->titleTopLayout;
    vertices = title_layout->unk_20;

    while (searching != 0) {
        texel = column;
        for (i = 0; i < 28; i++) {
            if (*texel & 0xF) {
                searching = 0;
                break;
            }
            texel += 0xC8;
        }
        blank_width++;
        column--;
    }

    vertices[0].v.ob[0] = (vertices[0].v.ob[0] - blank_width) + 4;
    vertices[1].v.ob[0] = (vertices[1].v.ob[0] - blank_width) + 4;
    vertices[8].v.ob[0] = (vertices[8].v.ob[0] - blank_width) + 4;
    vertices[9].v.ob[0] = (vertices[9].v.ob[0] - blank_width) + 4;
}

/**
 * Builds the banner geo layouts in one main pool block: the plate, the CLEAR label and the
 * division line; the title (the round 2 variant adds the "R-2" icon, which is then fitted to the
 * title); and the round label, choosing the variant with a digit, without a digit ("LEADER", "RIVAL")
 * or the wide one ("SEMIFINAL", "FINAL BATTLE").
 */
void BattlePrepBanner_LoadGeoLayouts(BattlePrepBanner* banner) {
    MemoryBlock* pool_block;

    pool_block = MainPool_AllocState(main_pool_get_available(), 0);
    banner->plateLayout = process_geo_layout(pool_block, D_84B10378);
    banner->clearLabelLayout = process_geo_layout(pool_block, D_84B10E78);
    banner->divisionLayout = process_geo_layout(pool_block, D_84B109F0);
    if (gSessionContext.inRound2 == 0) {
        banner->titleTopLayout = process_geo_layout(pool_block, D_84B10568);
        banner->titleBottomLayout = process_geo_layout(pool_block, D_84B105A8);
    } else {
        banner->titleTopLayout = process_geo_layout(pool_block, D_84B10838);
        banner->titleBottomLayout = process_geo_layout(pool_block, D_84B10878);
        BattlePrepBanner_FitRound2Icon(banner);
    }

    if (gSessionContext.mode == 7) {
        switch (gSessionContext.modeSelection) {
            default:
                if (gSessionContext.roundOrDemoId < 4) {
                    banner->roundLabelLayout = process_geo_layout(pool_block, D_84B11060);
                } else {
                    banner->roundLabelLayout = process_geo_layout(pool_block, D_84B10D70);
                }
                break;

            case 8:
                banner->roundLabelLayout = process_geo_layout(pool_block, D_84B11060);
                break;

            case 9:
                banner->roundLabelLayout = process_geo_layout(pool_block, D_84B10D70);
                break;
        }
    } else if (gSessionContext.roundOrDemoId < 7) {
        banner->roundLabelLayout = process_geo_layout(pool_block, D_84B11060);
    } else {
        banner->roundLabelLayout = process_geo_layout(pool_block, D_84B10BE8);
    }
    MainPool_FinalizeAllocation(pool_block);
}

/**
 * Gives a layout a vertical gradient: the first half of its vertices (the top row) get top_color
 * and the second half (the bottom row) get bottom_color.
 */
void BattlePrepBanner_SetVertexGradient(unk_D_86002F34* layout, Color_RGBA8* top_color, Color_RGBA8* bottom_color) {
    Vtx* vertex = layout->unk_20;
    s16 half_count = layout->unk_26 / 2;
    s16 i;

    for (i = 0; i < half_count; i++, vertex++) {
        vertex->v.cn[0] = top_color->r;
        vertex->v.cn[1] = top_color->g;
        vertex->v.cn[2] = top_color->b;
        vertex->v.cn[3] = top_color->a;
    }

    for (i = 0; i < half_count; i++, vertex++) {
        vertex->v.cn[0] = bottom_color->r;
        vertex->v.cn[1] = bottom_color->g;
        vertex->v.cn[2] = bottom_color->b;
        vertex->v.cn[3] = bottom_color->a;
    }
}

/**
 * Tints the title, division line, round label and CLEAR label with the gradient pair for the
 * current cup (D_84B0FCBC / D_84B0FCFC indexed by mode - 1) or, in the castle, for the current
 * location (offset by modeSelection).
 */
void BattlePrepBanner_ApplyColors(BattlePrepBanner* banner) {
    UNUSED s32 pad[2];
    s16 color_index;

    color_index = gSessionContext.mode - 1;
    if (gSessionContext.mode == 7) {
        color_index += gSessionContext.modeSelection;
    }

    BattlePrepBanner_SetVertexGradient(banner->titleTopLayout, &D_84B0FCBC[color_index], &D_84B0FCFC[color_index]);
    BattlePrepBanner_SetVertexGradient(banner->divisionLayout, &D_84B0FCFC[color_index], &D_84B0FCFC[color_index]);
    BattlePrepBanner_SetVertexGradient(banner->roundLabelLayout, &D_84B0FCFC[color_index], &D_84B0FCBC[color_index]);
    BattlePrepBanner_SetVertexGradient(banner->clearLabelLayout, &D_84B0FCFC[color_index], &D_84B0FCBC[color_index]);
}

/**
 * Resets the banner to idle, then loads its textures, builds its geo layouts and applies its colors.
 */
void BattlePrepBanner_Init(BattlePrepBanner* banner) {
    banner->state = 0;
    banner->timer = 0;
    BattlePrepBanner_LoadTextures(banner);
    BattlePrepBanner_LoadGeoLayouts(banner);
    BattlePrepBanner_ApplyColors(banner);
}

/**
 * Geo draw callback (event 5) for one cup ladder slot. Each ladder display object is a column with
 * two slots; the node's callbackArg picks the row (0 top, 1 bottom), giving portrait index
 * row * 4 + column. Binds that portrait to segment 0x0F and draws the olive slot frame with it.
 */
s32 OpponentLadder_DrawCupSlot(s32 event, unk_func_80011B94* display_node) {
    OpponentLadder* ladder;
    s32 column;
    s32 slot;

    if (event == 5) {
        ladder = (OpponentLadder*)D_8006F09C->node.callbackArg;
        column = ((D_8006F09C->effectSlot - ladder->displayObjects->effectSlot) % 8) - 4;
        slot = (display_node->display_node_header.callbackArg * 4) + column;

        gDPPipeSync(gDisplayListHead++);
        gSPSegment(gDisplayListHead++, 0x0F, (u32)ladder->portraits[slot] & 0x1FFFFFFF);
        gSPDisplayList(gDisplayListHead++, D_84B113C0);

        GeoRender_ApplyMaterialState();
    }
    return 0;
}

/**
 * Geo draw callback (event 5): when highlightCurrent is set, draws a pulsing yellow and red frame
 * (8x8 corner and edge pieces) around the cup ladder slot for the current round.
 */
s32 OpponentLadder_DrawCupHighlight(s32 event, unk_func_80011B94* display_node) {
    static s16 D_84B11C18 = 0;

    OpponentLadder* ladder;
    s32 alpha;
    s32 column;
    s32 slot;

    if (event == 5) {
        ladder = (OpponentLadder*)D_8006F09C->node.callbackArg;
        column = ((D_8006F09C->effectSlot - ladder->displayObjects->effectSlot) % 8) - 4;
        slot = column + display_node->display_node_header.callbackArg * 4;

        if ((ladder->highlightCurrent != 0) && ((slot + 1) == gSessionContext.roundOrDemoId)) {
            // clang-format off
            if (D_84B11C18 >= 0) { alpha = (s32)(SINS((s32)(SINS(D_84B11C18) * 16384.0f)) * 127.0f) + 0x7F;
            } else {
                alpha = (s32)(SINS(D_84B11C18) * 127.0f) + 0x7F;
            }
            // clang-format on

            gDPPipeSync(gDisplayListHead++);
            gDPSetPrimColor(gDisplayListHead++, 0, D_8006F09C->lodFraction, 200, 200, 30, alpha); // #C8C81E
            gDPSetEnvColor(gDisplayListHead++, 200, 30, 30, alpha); // #C81E1E
            gSPDisplayList(gDisplayListHead++, D_84B114F8);

            GeoRender_ApplyMaterialState();
            D_84B11C18 += 0x800;
        }
    }
    return 0;
}

/**
 * Geo draw callback (event 5) for one Gym Leader Castle ladder slot: binds that slot's rendered
 * card (trainer name and portrait, see OpponentLadder_RenderCastleCard) to segment 0x0F and draws
 * it.
 */
s32 OpponentLadder_DrawCastleSlot(s32 event, unk_func_80011B94* display_node) {
    OpponentLadder* ladder;
    s32 slot;

    if (event == 5) {
        ladder = (OpponentLadder*)D_8006F09C->node.callbackArg;
        slot = ((D_8006F09C->effectSlot - ladder->displayObjects->effectSlot) % 8) - 4;

        gDPPipeSync(gDisplayListHead++);

        gSPSegment(gDisplayListHead++, 0x0F, (u32)ladder->castleCards[slot]->img_p & 0x1FFFFFFF);
        gSPDisplayList(gDisplayListHead++, display_node->display_node_header.callbackArg);

        GeoRender_ApplyMaterialState();
    }
    return 0;
}

/**
 * Geo draw callback (event 5): when highlightCurrent is set, draws the pulsing yellow and red frame
 * around the Gym Leader Castle ladder slot for the current round.
 */
s32 OpponentLadder_DrawCastleHighlight(s32 event, unk_func_80011B94* display_node) {
    static s16 D_84B11C1C = 0;

    OpponentLadder* ladder;
    s32 alpha;
    s32 slot;

    if (event == 5) {
        ladder = (OpponentLadder*)D_8006F09C->node.callbackArg;
        slot = ((D_8006F09C->effectSlot - ladder->displayObjects->effectSlot) % 8);
        slot -= 4;

        if ((ladder->highlightCurrent != 0) && ((slot + 1) == gSessionContext.roundOrDemoId)) {
            // clang-format off
            if (D_84B11C1C >= 0) {                alpha = (s32)(SINS((s32)(SINS(D_84B11C1C) * 16384.0f)) * 127.0f)+ 0x7F;
            } else {
                alpha = (s32)(SINS(D_84B11C1C) * 127.0f) + 0x7F;
            }
            // clang-format on
            gDPPipeSync(gDisplayListHead++);

            gDPSetPrimColor(gDisplayListHead++, 0, D_8006F09C->lodFraction, 200, 200, 30, alpha); // #C8C81E
            gDPSetEnvColor(gDisplayListHead++, 200, 30, 30, alpha); // #C81E1E
            gSPDisplayList(gDisplayListHead++, display_node->display_node_header.callbackArg);

            GeoRender_ApplyMaterialState();
            D_84B11C1C += 0x800;
        }
    }
    return 0;
}

/**
 * Draws the defeated mark into a 64x64 portrait texture: D_3017F00, a 64x64 I4 "X", tinted
 * red-orange with alpha 160.
 */
void OpponentLadder_StampDefeatedMark(s32 portrait) {
    DepthBuffer image;
    u32 mark_texture = Util_ConvertAddrToVirtAddr(&D_3017F00);

    GfxImage_Initialize(&image, 0, 2, 0x40, 0x40, portrait);
    GfxImage_SetRenderTarget(&gDisplayListHead, &image);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetEnvColor(gDisplayListHead++, 255, 50, 20, 160); // #FF3214A0

    Gfx_DrawTextureI4(0, 0, 0x40, 0x40, mark_texture, 0x40, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

/**
 * Ends a ladder flip: replaces the four flipping slot columns (objects 12-15) with upright front
 * columns (objects 4-7) using the ladder layout.
 */
void OpponentLadder_FinishFlip(OpponentLadder* ladder) {
    s32 i;

    for (i = 0; i < 4; i++) {
        ModelRenderer_ClearDisplayObject(&ladder->displayObjects[i + 4]);
        BattlePrep_ResetDisplayObject(&ladder->displayObjects[i + 12]);
        Model_InitDisplayObject(&ladder->displayObjects[i + 4], 1, 0, ladder->slotLayout);
        ladder->displayObjects[i + 4].rotation.x = 0;
        ladder->displayObjects[i + 4].node.callbackArg = ladder;
    }
}

/**
 * State 1 update: slides the four ladder columns in from the right at 32 units per frame for 10
 * frames, ending at x = -96, -32, 32 and 96, then goes idle.
 */
void OpponentLadder_UpdateSlideIn(OpponentLadder* ladder) {
    s32 i;

    ladder->timer++;

    for (i = 0; i < 4; i++) {
        ladder->displayObjects[4 + i].position.x = (0xE0 + i * 0x40) - (ladder->timer << 5);
    }

    if (ladder->timer == 0xA) {
        ladder->timer = 0;
        ladder->state = 0;
    }
}

/**
 * State 2 update: slides the four ladder columns off to the left at 32 units per frame for 10
 * frames, then hides them and goes idle.
 */
void OpponentLadder_UpdateSlideOut(OpponentLadder* ladder) {
    s32 i;
    s32 start_x;

    ladder->timer++;

    for (i = 0, start_x = -0x60; i < 4; i++, start_x += 0x40) {
        ladder->displayObjects[4 + i].position.x = (s16)start_x - (ladder->timer << 5);
    }

    if (ladder->timer == 0xA) {
        for (i = 0; i < 4; i++) {
            BattlePrep_ResetDisplayObject(&ladder->displayObjects[4 + i]);
        }
        ladder->timer = 0;
        ladder->state = 0;
    }
}

/**
 * State 3 update, after a win: the ladder flips over into view (half a turn about X over 16
 * frames). On frame 16 it plays sound 0x0110000C and settles; on frame 32 it plays sound 0x01100014
 * and stamps the defeated mark on the current round's opponent. In the castle the slot card is then
 * redrawn so the mark shows. Stamps follow the round index (roundOrDemoId), not the opponent
 * actually fought.
 */
void OpponentLadder_UpdateFlipInAndStamp(OpponentLadder* ladder) {
    s32 i;
    s32 slot_count;

    ladder->timer++;
    if (ladder->timer < 0x10) {
        for (i = 0; i < 4; i++) {
            ladder->displayObjects[4 + i].rotation.x += 0x800;
            ladder->displayObjects[12 + i].rotation.x += 0x800;
        }
    } else if (ladder->timer == 0x10) {
        Audio_PlayCategory11SoundCommand(0x0110000C, 0, 0);
        OpponentLadder_FinishFlip(ladder);
    } else if (ladder->timer == 0x20) {
        if (gSessionContext.mode == 7) {
            slot_count = 4;
        } else {
            slot_count = 8;
        }

        Audio_PlayCategory11SoundCommand(0x01100014, gSessionContext.roundOrDemoId, slot_count);
        OpponentLadder_StampDefeatedMark(ladder->portraits[gSessionContext.roundOrDemoId - 1]);

        if (gSessionContext.mode == 7) {
            GfxImage_SetRenderTarget(&gDisplayListHead, ladder->castleCards[gSessionContext.roundOrDemoId - 1]);

            gSPDisplayList(gDisplayListHead++, D_8006F4E0);

            Gfx_DrawTextureRgba16(4, 0x24, 0x38, 0x1C, &ladder->portraits[gSessionContext.roundOrDemoId - 1][0x208], 0x40,
                                  0x200000);
            Gfx_DrawTextureRgba16(4, 0x40, 0x38, 0x1C, &ladder->portraits[gSessionContext.roundOrDemoId - 1][0x1008], 0x40,
                                  0x200000);

            gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
        }

        ladder->timer = 0;
        ladder->state = 0;
    }
}

/**
 * State 4 update, after a loss: the ladder flips over into view (half a turn about X over 16
 * frames), then plays sound 0x0110000C and settles. No new mark is stamped.
 */
void OpponentLadder_UpdateFlipIn(OpponentLadder* ladder) {
    s32 i;

    for (i = 0; i < 4; i++) {
        ladder->displayObjects[4 + i].rotation.x -= 0x800;
        ladder->displayObjects[12 + i].rotation.x -= 0x800;
    }

    ladder->timer++;
    if (ladder->timer == 0x10) {
        Audio_PlayCategory11SoundCommand(0x0110000C, 0, 0);
        OpponentLadder_FinishFlip(ladder);
        ladder->timer = 0;
        ladder->state = 0;
    }
}

/**
 * Runs the per-frame ladder update for the current state: 1 slide in, 2 slide out, 3 flip in and
 * stamp (win), 4 flip in (loss); 0 is idle.
 */
void OpponentLadder_Update(OpponentLadder* ladder) {
    switch (ladder->state) {
        case 1:
            OpponentLadder_UpdateSlideIn(ladder);
            break;

        case 2:
            OpponentLadder_UpdateSlideOut(ladder);
            break;

        case 3:
            OpponentLadder_UpdateFlipInAndStamp(ladder);
            break;

        case 4:
            OpponentLadder_UpdateFlipIn(ladder);
            break;
    }
}

/**
 * Starts a ladder animation on the display objects starting at display_objects. animation 0 is
 * the round intro: columns 4-7 wait off-screen right and slide in (state 1). Otherwise columns
 * 12-15 are placed face-down at their final positions and flip in: animation 1 after a win stamps
 * the defeated opponent and stops the highlight (state 3); animation 2 after a loss keeps it
 * (state 4).
 */
void OpponentLadder_Start(OpponentLadder* ladder, DisplayObject* display_objects, s16 animation) {
    s32 i;
    s16 x;

    ladder->displayObjects = display_objects;
    ladder->timer = 0;

    if (animation == 0) {
        for (i = 4; i < 8; i++) {
            Model_InitDisplayObject(&ladder->displayObjects[i], 1, 0, ladder->slotLayout);
            Vec3f_SetComponentsDuplicate(&ladder->displayObjects[i].position, 320.0f, -48.0f, -289.0f);
            ladder->displayObjects[i].node.callbackArg = (s32)ladder;
        }
        ladder->state = 1;
    } else {
        for (i = 12, x = -0x60; i < 16; i++) {
            Model_InitDisplayObject(&ladder->displayObjects[i], 1, 0, ladder->slotLayout);
            Vec3f_SetComponentsDuplicate(&ladder->displayObjects[i].position, x, -48.0f, -289.0f);
            ladder->displayObjects[i].rotation.x = -0x8000;
            x += 0x40;
            ladder->displayObjects[i].node.callbackArg = ladder;
        }

        if (animation == 1) {
            ladder->highlightCurrent = 0;
            ladder->state = 3;
        } else {
            ladder->highlightCurrent = 1;
            ladder->state = 4;
        }
    }
}

/**
 * Renders Gym Leader Castle ladder card `slot` (64x96): the olive card background, the trainer's
 * portrait and the trainer's name centered at the top. For the rival (modeSelection 9) only slot 0
 * gets a portrait and name.
 */
void OpponentLadder_RenderCastleCard(OpponentLadder* ladder, s16 slot, char* name) {
    GfxImage_SetRenderTarget(&gDisplayListHead, ladder->castleCards[slot]);

    gSPDisplayList(gDisplayListHead++, D_8006F4E0);

    Gfx_DrawTextureRgba16(0, 0, 0x40, 0x20, D_3012000, 0x40, 0x200000);
    Gfx_DrawTextureRgba16(0, 0x20, 0x40, 0x20, D_3013000, 0x40, 0x200000);
    Gfx_DrawTextureRgba16(0, 0x40, 0x40, 0x20, D_3014000, 0x40, 0x200000);

    if ((gSessionContext.modeSelection < 9) || (slot == 0)) {
        Gfx_DrawTextureRgba16(4, 0x24, 0x38, 0x1C, ladder->portraits[slot] + 0x208, 0x40, 0x200000);
        Gfx_DrawTextureRgba16(4, 0x40, 0x38, 0x1C, ladder->portraits[slot] + 0x1008, 0x40, 0x200000);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    if ((gSessionContext.modeSelection < 9) || (slot == 0)) {
        s32 name_width = Font_MeasureTextExtent(2, 0, name);

        Font_BeginTranslucentTextRendering();
        Font_SetLineHeight(0xC);
        Font_SetActive(2, 0);
        Font_Printf(0x20 - (name_width / 2), 6, name);
        Font_EndTexturedTextRendering();
    }
}

/**
 * Grows a main pool block and shifts its contents so the data starts on a 64-byte boundary;
 * returns the aligned address. Used for portrait textures.
 */
s32 BattlePrep_Align64PoolBlock(s32* block) {
    u32 size;
    s32 aligned;
    s32 i;
    u32 shift;

    aligned = (s32)block;
    shift = 0x40 - ((u32)block & 0x3F);
    if (shift != 0x40) {
        s32* src;
        s32* dst;

        size = main_pool_get_block_dist(block);
        src = (u32)block + size;
        dst = (u32)block + (size + shift);
        main_pool_realloc(block, (size + shift));
        for (; block < src;) {
            *--dst = *--src;
        }
        aligned += shift;
    }
    return aligned;
}

/**
 * Loads the ladder portraits. The trainer list for the current mode and division or location
 * (+31 in round 2) comes from trainer_archive; portraits come from portrait_archive. In the castle
 * the four location trainers are loaded and each slot card is rendered. In the cups, opponents up to
 * the current round get their portraits and later slots get the placeholder "?" portrait 0. Slots
 * before the current round are re-stamped with the defeated mark.
 */
void OpponentLadder_LoadPortraits(OpponentLadder* ladder, BinArchive* trainer_archive, BinArchive* portrait_archive) {
    s16 i;
    s16 roster_file;
    TrainerData* trainers;
    TrainerData* trainer;
    s16 portrait_file;

    switch (gSessionContext.mode) {
        case 1:
            roster_file = 6;
            break;

        case 2:
            roster_file = 7;
            break;

        case 3:
            roster_file = gSessionContext.modeSelection + 8;
            break;

        case 4:
            roster_file = 0;
            break;

        case 5:
            roster_file = 1;
            break;

        case 6:
            roster_file = gSessionContext.modeSelection + 2;
            break;

        case 7:
            roster_file = gSessionContext.modeSelection + 0xC;
            break;

        case 8:
            roster_file = 0x16;
            break;
    }

    if (gSessionContext.inRound2 != 0) {
        roster_file += 0x1F;
    }

    trainers = BinArchive_GetFile(trainer_archive, roster_file);
    if (gSessionContext.mode == 7) {
        for (i = 0; i < 4; i++) {
            trainer = i + ((void)0, trainers);

            portrait_file = (trainer->gfx_file_index >> 8) & 0xFF;
            ladder->portraits[i] = BattlePrep_Align64PoolBlock(BinArchive_GetFile(portrait_archive, portrait_file));
            if (i < (gSessionContext.roundOrDemoId - 1)) {
                OpponentLadder_StampDefeatedMark(ladder->portraits[i]);
            }
            ladder->castleCards[i] = GfxImage_Allocate(0, 2, 0x40, 0x60, 0);
            OpponentLadder_RenderCastleCard(ladder, i, trainer->name2);
        }
    } else {
        for (i = 0; i < 8; i++) {
            if (i < gSessionContext.roundOrDemoId) {
                portrait_file = (trainers[gSessionContext.opponentHistory[0][i]].gfx_file_index >> 8) & 0xFF;

                ladder->portraits[i] = BattlePrep_Align64PoolBlock(BinArchive_GetFile(portrait_archive, portrait_file));
                if (i < (gSessionContext.roundOrDemoId - 1)) {
                    OpponentLadder_StampDefeatedMark(ladder->portraits[i]);
                }
            } else {
                ladder->portraits[i] = BinArchive_GetFile(portrait_archive, 0);
            }
        }
    }
}

/**
 * Resets the ladder to idle with the highlight on, allocates the four castle slot cards, builds
 * the cup or castle ladder layout and loads the portraits.
 */
void OpponentLadder_Init(OpponentLadder* ladder, BinArchive* trainer_archive, BinArchive* portrait_archive) {
    s32 i;
    MemoryBlock* pool_block;

    ladder->state = 0;
    ladder->timer = 0;
    ladder->highlightCurrent = 1;
    ladder->unk_04 = 0;

    for (i = 0; i < 4; i++) {
        ladder->castleCards[i] = GfxImage_Allocate(0, 2, 0x40, 0x60, 0);
    }

    pool_block = MainPool_AllocState(main_pool_get_available(), 0);
    if (gSessionContext.mode == 7) {
        ladder->slotLayout = process_geo_layout(pool_block, D_84B11BD0);
    } else {
        ladder->slotLayout = process_geo_layout(pool_block, D_84B11640);
    }

    MainPool_FinalizeAllocation(pool_block);
    OpponentLadder_LoadPortraits(ladder, trainer_archive, portrait_archive);
}
