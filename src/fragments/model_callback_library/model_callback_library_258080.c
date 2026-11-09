#include "model_callback_library.h"
#include "src/fragments/clefairy_game/clefairy_game.h"
#include "src/fragments/rattata_game/rattata_game.h"
#include "src/fragments/victory_palace/victory_palace.h"
#include "src/gfx_buffer.h"
#include "src/geo_render.h"
#include "src/battle_hud.h"

typedef struct {
    /* 0x00 */ u8 rgb0[3];    // start color
    /* 0x03 */ u8 frameStart; // frame where this keyframe begins
} Keyframe;                   // size = 0x04

// .bss
static s32 sScrollingIconFrame0;
static u8 pad_D_81004B78[0x8];
static s32 sScrollingIconFrame1;
static u8 pad_D_81004B88[0x8];
static s32 sScrollingIconFrame2;
static u8 pad_D_81004B98[0x8];
static s32 sScrollingIconFrame3;
static u8 pad_D_81004BA8[0x8];
static s32 sDigitGlyphIndex;
static char sNameBuffer[0x28];

/**
 * Loads a 32x64 CI8 texture with a 256-color palette (arg1), choosing the image row from
 * rattata_game's D_862082A0[0] and one of four animation frames from sScrollingIconFrame0.
 */
void DisplayList_BuildScrollingIconSegment0(Gfx* gfx, u16* arg1, u8* arg2) {
    gDPPipeSync(gfx++);
    gSPTexture(gfx++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gfx++, G_TT_RGBA16);
    gDPLoadTLUT_pal256(gfx++, arg1);
    gDPLoadTextureTile(gfx++, (arg2 + ((D_862082A0[0] * 4) & ~0x1F) + ((sScrollingIconFrame0 & 3) << 0xB)), G_IM_FMT_CI,
                       G_IM_SIZ_8b, 0x20, 0x40, 0, 0, 0x20 - 1, 0x40 - 1, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                       G_TX_NOMIRROR | G_TX_CLAMP, 5, 6, G_TX_NOLOD, G_TX_NOLOD);
    gSPEndDisplayList(gfx++);
}

/**
 * Geo callback (jump-table slot 0x180). arg0 == 0 resets the frame counter; the build pass
 * builds DisplayList_BuildScrollingIconSegment0 and advances it. Only non-species models reference it.
 */
void DisplayList_InitScrollingIconSegment0(s32 arg0, DisplayListState* state) {
    Gfx* gfx;
    DisplayListAddresses* addresses;

    addresses = state->addresses;
    switch (arg0) {
        case 0:
            sScrollingIconFrame0 = 0;
            return;
        case 2:
            gfx = Gfx_AllocDisplayList(0xA0);
            state->gfx = gfx;
            DisplayList_BuildScrollingIconSegment0(gfx, addresses->segments[0], addresses->segments[1]);
            sScrollingIconFrame0 += 1;
            return;
    }
}

/**
 * Loads a 32x64 CI8 texture with a 256-color palette (arg1), choosing the image row from
 * rattata_game's D_862082A0[1] and one of four animation frames from sScrollingIconFrame1.
 */
void DisplayList_BuildScrollingIconSegment1(Gfx* gfx, u16* arg1, u8* arg2) {
    gDPPipeSync(gfx++);
    gSPTexture(gfx++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gfx++, G_TT_RGBA16);
    gDPLoadTLUT_pal256(gfx++, arg1);
    gDPLoadTextureTile(gfx++, (arg2 + ((D_862082A0[1] * 4) & ~0x1F) + ((sScrollingIconFrame1 & 3) << 0xB)), G_IM_FMT_CI,
                       G_IM_SIZ_8b, 0x20, 0x40, 0, 0, 0x20 - 1, 0x40 - 1, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                       G_TX_NOMIRROR | G_TX_CLAMP, 5, 6, G_TX_NOLOD, G_TX_NOLOD);
    gSPEndDisplayList(gfx++);
}

/**
 * Geo callback (jump-table slot 0x188). arg0 == 0 resets the frame counter; the build pass
 * builds DisplayList_BuildScrollingIconSegment1 and advances it. Only non-species models reference it.
 */
void DisplayList_InitScrollingIconSegment1(s32 arg0, DisplayListState* state) {
    Gfx* gfx;
    DisplayListAddresses* addresses;

    addresses = state->addresses;
    switch (arg0) { /* irregular */
        case 0:
            sScrollingIconFrame1 = 0;
            return;
        case 2:
            gfx = Gfx_AllocDisplayList(0xA0);
            state->gfx = gfx;
            DisplayList_BuildScrollingIconSegment1(gfx, addresses->segments[0], addresses->segments[1]);
            sScrollingIconFrame1 += 1;
            return;
    }
}

/**
 * Loads a 32x64 CI8 texture with a 256-color palette (arg1), choosing the image row from
 * rattata_game's D_862082A0[2] and one of four animation frames from sScrollingIconFrame2.
 */
void DisplayList_BuildScrollingIconSegment2(Gfx* gfx, u16* arg1, u8* arg2) {
    gDPPipeSync(gfx++);
    gSPTexture(gfx++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gfx++, G_TT_RGBA16);
    gDPLoadTLUT_pal256(gfx++, arg1);
    gDPLoadTextureTile(gfx++, (arg2 + ((D_862082A0[2] * 4) & ~0x1F) + ((sScrollingIconFrame2 & 3) << 0xB)), G_IM_FMT_CI,
                       G_IM_SIZ_8b, 0x20, 0x40, 0, 0, 0x20 - 1, 0x40 - 1, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                       G_TX_NOMIRROR | G_TX_CLAMP, 5, 6, G_TX_NOLOD, G_TX_NOLOD);
    gSPEndDisplayList(gfx++);
}

/**
 * Geo callback (jump-table slot 0x190). arg0 == 0 resets the frame counter; the build pass
 * builds DisplayList_BuildScrollingIconSegment2 and advances it. Only non-species models reference it.
 */
void DisplayList_InitScrollingIconSegment2(s32 arg0, DisplayListState* state) {
    Gfx* gfx;
    DisplayListAddresses* addresses;

    addresses = state->addresses;
    switch (arg0) { /* irregular */
        case 0:
            sScrollingIconFrame2 = 0;
            return;
        case 2:
            gfx = Gfx_AllocDisplayList(0xA0);
            state->gfx = gfx;
            DisplayList_BuildScrollingIconSegment2(gfx, addresses->segments[0], addresses->segments[1]);
            sScrollingIconFrame2 += 1;
            return;
    }
}

/**
 * Loads a 32x64 CI8 texture with a 256-color palette (arg1), choosing the image row from
 * rattata_game's D_862082A0[3] and one of four animation frames from sScrollingIconFrame3.
 */
void DisplayList_BuildScrollingIconSegment3(Gfx* gfx, u16* arg1, u8* arg2) {
    gDPPipeSync(gfx++);
    gSPTexture(gfx++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gfx++, G_TT_RGBA16);
    gDPLoadTLUT_pal256(gfx++, arg1);
    gDPLoadTextureTile(gfx++, (arg2 + ((D_862082A0[3] * 4) & ~0x1F) + ((sScrollingIconFrame3 & 3) << 0xB)), G_IM_FMT_CI,
                       G_IM_SIZ_8b, 0x20, 0x40, 0, 0, 0x20 - 1, 0x40 - 1, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                       G_TX_NOMIRROR | G_TX_CLAMP, 5, 6, G_TX_NOLOD, G_TX_NOLOD);
    gSPEndDisplayList(gfx++);
}

/**
 * Geo callback (jump-table slot 0x198). arg0 == 0 resets the frame counter; the build pass
 * builds DisplayList_BuildScrollingIconSegment3 and advances it. Only non-species models reference it.
 */
void DisplayList_InitScrollingIconSegment3(s32 arg0, DisplayListState* state) {
    Gfx* gfx;
    DisplayListAddresses* addresses;

    addresses = state->addresses;
    switch (arg0) { /* irregular */
        case 0:
            sScrollingIconFrame3 = 0;
            return;
        case 2:
            gfx = Gfx_AllocDisplayList(0xA0);
            state->gfx = gfx;
            DisplayList_BuildScrollingIconSegment3(gfx, addresses->segments[0], addresses->segments[1]);
            sScrollingIconFrame3 += 1;
            return;
    }
}

/**
 * Geo callback (jump-table slot 0x1B0). Writes a raw G_SETTILESIZE command whose offsets grow with
 * a 7-bit counter, so the texture slides each frame.
 */
void DisplayList_InitPulsingRectSegment(s32 arg0, DisplayListState* state) {
    static u8 D_81004190 = 0;
    s32 temp_a1;
    s32 temp_a2;
    Gfx* gfx;

    if (arg0 == 2) {
        gfx = Gfx_AllocDisplayList(0x10);
        temp_a1 = D_81004190 * 3;
        temp_a2 = D_81004190 * -6;
        state->gfx = gfx;
        // Keep expanded GBI commands on one line for IDO matching.
        // clang-format off
        {    Gfx *_gfx = (Gfx *)(gfx++); _gfx->words.w0 = (((temp_a1 & 0xFFF) << 0xC) | 0xF2000000 | (temp_a2 & 0xFFF));    _gfx->words.w1 = ((((0x27F << (temp_a1 + 2)) & 0xFFF) << 0xC) | ((0x7F << (temp_a2 + 2)) & 0xFFF));    }
        // clang-format on
        gSPEndDisplayList(gfx++);
        D_81004190++;
        D_81004190 &= 0x7F;
    }
}

/**
 * Geo callback (jump-table slot 0x1B8). Interpolates the primitive color through a 50-frame
 * red-yellow-green-blue-purple keyframe loop and scrolls two 320x64 tiles in opposite directions.
 */
void DisplayList_InitColorCycleBoxSegment(s32 arg0, DisplayListState* state) {
    static u8 D_81004194 = 0;
    static u8 D_81004198 = 0;
    static Keyframe D_8100419C[6] = {
        { { 0xFF, 0x32, 0x32 }, 0 },  { { 0xC8, 0xC8, 0x00 }, 10 }, { { 0x64, 0xFF, 0x00 }, 20 },
        { { 0x00, 0xC8, 0xFF }, 30 }, { { 0x96, 0x32, 0xC8 }, 40 }, { { 0xFF, 0x32, 0x32 }, 50 },
    };
    u8 i;
    u8 c;
    Keyframe* kf;
    Gfx* gfx;
    u8 sp48[3];

    if (arg0 == 2) {
        gfx = Gfx_AllocDisplayList(sizeof(Gfx) * 4);
        state->gfx = gfx;
        for (i = 0; i < 5; i++) {
            if (D_81004198 == D_8100419C[i].frameStart) {
                for (c = 0; c < 3; c++) {
                    sp48[c] = D_8100419C[i].rgb0[c];
                }
                break;
            }
            if (D_8100419C[i].frameStart < D_81004198 && D_81004198 < D_8100419C[i + 1].frameStart) {
                for (c = 0; c < 3; c++) {
                    sp48[c] = (s32)(((f32)((D_8100419C[i + 1].rgb0[c] - D_8100419C[i].rgb0[c]) *
                                           (D_81004198 - D_8100419C[i].frameStart)) /
                                     (f32)(D_8100419C[i + 1].frameStart - D_8100419C[i].frameStart)) +
                                    D_8100419C[i].rgb0[c] + 0.5f);
                }
                break;
            }
        }
        gDPSetPrimColor(gfx++, 0, 0x80, sp48[0], sp48[1], sp48[2], 180);
        gDPSetTileSize(gfx++, 0, -2 * D_81004194, 0, (320 - 1) << ((-2 * D_81004194) + 2), (64 - 1) << 2);
        gDPSetTileSize(gfx++, 1, 2 * D_81004194, 0, (320 - 1) << ((2 * D_81004194) + 2), (64 - 1) << 2);
        gSPEndDisplayList(gfx++);
        D_81004194++;
        D_81004194 &= 0x7F;
        D_81004198++;
        D_81004198 &= 0xFF;
        if ((s32)D_81004198 >= 0x32) {
            D_81004198 = 0;
        }
    }
}

/**
 * Writes the setup for the digit/name glyph plate: calls D_810041D0, sets the primitive alpha
 * from D_800AF770[5], the environment color, and loads the 4-bit texture at D_3008000. Resets
 * sDigitGlyphIndex so the following glyph callbacks start at the first digit.
 */
void DisplayList_BuildDigitDisplayBackground(Gfx* gfx) {
    s32 temp_a3;

    // Keep expanded GBI commands on one line for IDO matching.
    // clang-format off
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xDE000000;    _gfx->words.w1 = &D_810041D0;    }
    temp_a3 = ((s32) ((D_800AF770[5]) * 0x55) / 255) + 0xAA;
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = ((temp_a3 & 0xFF) | 0xFA000000);    _gfx->words.w1 = -0x3701;    }
#if defined(VERSION_JP)
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xFB000000;    _gfx->words.w1 = 0x64320AFF;    }
#else
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xFB000000;    _gfx->words.w1 = 0x501E0AFF;    }
#endif
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xFD700000;    _gfx->words.w1 = Util_ConvertAddrToVirtAddr(&D_3008000);    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF5700000;    _gfx->words.w1 = 0x07094260;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE6000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF3000000;    _gfx->words.w1 = 0x070FF200;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE7000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF5680800;    _gfx->words.w1 = 0x94260;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF2000000;    _gfx->words.w1 = 0x7C03C;    }
    // clang-format on
    gSPEndDisplayList(gfx++);
    sDigitGlyphIndex = 0;
}

/**
 * Returns the length of a NUL-terminated string.
 */
s32 Util_Strlen(s8* arg0) {
    s32 var_v1;
    s8 temp_t7;
    s8* var_v0;

    var_v1 = 0;
    var_v0 = arg0;
    if (*arg0 != 0) {
        do {
            var_v1 += 1;
            var_v0 += 1;
        } while (var_v0[0] != 0);
    }
    return var_v1;
}

#if defined(VERSION_US_REV2)
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/model_callback_library/model_callback_library_258080/DisplayList_BuildDigitOrGlyphSegment.s")
#else
/**
 * Loads one 16x16 IA8 glyph from the font sheet. Glyphs 0-2 are the hundreds, tens and units of
 * D_800AF770[effectSlot % 5] + 1. Later glyphs spell that species' name (from gVictoryPokemonNames),
 * shifted right by (10 - name length) * 32 units. Advances sDigitGlyphIndex.
 */
void DisplayList_BuildDigitOrGlyphSegment(Gfx* gfx) {
    u16 temp_v1;
    u8 sp4D;
    s32 pad[4];
    u8 var_v0 = D_8006F09C->effectSlot % 5;
    u8 temp_t0;
    char* sp34;
    Mtx* sp30;
    s32 sp2C;

    temp_t0 = D_800AF770[var_v0];
    switch (sDigitGlyphIndex) {
        case 0:
            sp4D = (((s32)(temp_t0 + 1) / 100) + 0x10) & 0xFF;
            break;
        case 1:
            sp4D = (((s32)((s32)(temp_t0 + 1) % 100) / 10) + 0x10) & 0xFF;
            break;
        case 2:
            sp4D = (((s32)(temp_t0 + 1) % 10) + 0x10) & 0xFF;
            break;
        default: {
#if defined(VERSION_JP)
            sp34 = D_8006FF00[temp_t0].name;
#else
            sp34 = Text_GetString(0, 0, gVictoryPokemonNames, temp_t0);
#endif
            sprintf(sNameBuffer, "%s     ", sp34);
            temp_v1 = sDigitGlyphIndex - 3;
#if defined(VERSION_JP)
            // Japanese names are two-byte EUC-JP characters; masking the pair with 0x7F7F gives
            // the JIS code that this build's Font_MapGlyphCode maps to a glyph index.
            //@asm/jp/graphics_textures.s
            sp4D = Font_MapGlyphCode((u16)(((sNameBuffer[temp_v1 * 2] << 8) + sNameBuffer[temp_v1 * 2 + 1]) & 0x7F7F));
#else
            sp4D = Font_MapGlyphCode((u8)sNameBuffer[temp_v1]);
#endif
            sp2C = 0xA - Util_Strlen((s8*)sp34);
            sp30 = Gfx_AllocDisplayList(sizeof(Mtx));
#if defined(VERSION_JP)
            guTranslate(sp30, (f32)sp2C * 22.0f, 0.0f, 0.0f);
#else
            guTranslate(sp30, (f32)sp2C * 32.0f, 0.0f, 0.0f);
#endif
            gSPMatrix(gfx++, sp30, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
            break;
        }
    }
#if defined(VERSION_JP)
    gDPLoadTextureTile(gfx++,
                       Util_ConvertAddrToVirtAddr(D_3000210 + ((sp4D & 0xF) << 4) + ((((sp4D & 0xF0) >> 4) << 4) << 8)),
                       G_IM_FMT_IA, G_IM_SIZ_8b, 256, 17, 0, 0, 16, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                       G_TX_NOMIRROR | G_TX_CLAMP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);
#else
    gDPLoadTextureTile(gfx++,
                       Util_ConvertAddrToVirtAddr(D_3000000 + ((sp4D & 0xF) << 4) + ((((sp4D & 0xF0) >> 4) << 4) << 8)),
                       G_IM_FMT_IA, G_IM_SIZ_8b, 256, 17, 0, 0, 16, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                       G_TX_NOMIRROR | G_TX_CLAMP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);
#endif
    gSPEndDisplayList(gfx++);
    sDigitGlyphIndex++;
}
#endif

/**
 * Geo callback (jump-table slot 0x1C8). Builds DisplayList_BuildDigitDisplayBackground.
 */
void DisplayList_InitDigitDisplayBackground(s32 arg0, DisplayListState* state) {
    Gfx* gfx;

    if (arg0 == 2) {
        gfx = Gfx_AllocDisplayList(0xF0);
        state->gfx = gfx;
        DisplayList_BuildDigitDisplayBackground(gfx);
    }
}

/**
 * Geo callback (jump-table slot 0x1C0). Builds DisplayList_BuildDigitOrGlyphSegment; each call
 * draws the next glyph.
 */
void DisplayList_InitDigitOrGlyphSegment(s32 arg0, DisplayListState* state) {
    Gfx* gfx;

    if (arg0 == 2) {
        gfx = Gfx_AllocDisplayList(0xA0);
        state->gfx = gfx;
        DisplayList_BuildDigitOrGlyphSegment(gfx);
    }
}
