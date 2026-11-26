#include "credits.h"
#include "src/graphics_textures.h"
#include "src/gfx_buffer.h"
#include "include/string.h"
#include "stdarg.h"
#include "lib/ultralib/src/libc/xstdio.h"

#ifdef VERSION_JP
// JP draws the staff roll with its own text routines. Positions are in quarter pixels, and
// ASCII text is kerned with per-glyph bearings instead of the font's advance widths.

/**
 * Returns the extra kerning, in quarter pixels, between `prev` and `cur`: 4 for the pairs
 * A-T, A-W and A-Y, and for T, W or Y followed by A, a, e, i, o, s or u; otherwise 0.
 */
s32 Credits_GetKerning(s32 prev, s32 cur) {
    switch (prev) {
        case 'T':
        case 'W':
        case 'Y':
            switch (cur) {
                case 'A':
                case 'a':
                case 'e':
                case 'i':
                case 'o':
                case 's':
                case 'u':
                    return 4;
            }
            break;

        case 'A':
            switch (cur) {
                case 'T':
                case 'W':
                case 'Y':
                    return 4;
            }
            break;
    }
    return 0;
}

/**
 * Writes how many quarter pixels to trim before (`left`) and after (`right`) the ASCII glyph
 * `cur`, including its kerning against `prev`.
 */
void Credits_GetGlyphTrim(s32 prev, s32 cur, s8* left, s8* right) {
    // Left and right bearings of the printable ASCII glyphs, indexed from ' '.
    static s8 sGlyphBearings[0x60][2] = {
        { 16, 16 }, { 0, 0 },   { 0, 0 },   { 0, 0 },   { 0, 0 },   { 0, 0 },   { 0, 0 },  { 0, 0 },   { 0, 0 },
        { 0, 0 },   { 0, 0 },   { 0, 0 },   { 0, 0 },   { 0, 0 },   { 0, 0 },   { 0, 0 },  { 0, 0 },   { 0, 0 },
        { 4, 8 },   { 4, 10 },  { 5, 8 },   { 0, 0 },   { 4, 8 },   { 0, 0 },   { 0, 0 },  { 0, 0 },   { 0, 0 },
        { 0, 0 },   { 0, 0 },   { 0, 0 },   { 0, 0 },   { 0, 0 },   { 0, 0 },   { 4, 8 },  { 6, 10 },  { 4, 8 },
        { 7, 8 },   { 8, 12 },  { 7, 16 },  { 7, 6 },   { 12, 7 },  { 19, 23 }, { 8, 15 }, { 8, 12 },  { 10, 12 },
        { 3, 6 },   { 10, 6 },  { 3, 5 },   { 7, 11 },  { 4, 6 },   { 5, 8 },   { 7, 12 }, { 8, 12 },  { 7, 10 },
        { 4, 12 },  { 4, 0 },   { 10, 8 },  { 4, 8 },   { 8, 12 },  { 0, 0 },   { 0, 0 },  { 0, 0 },   { 0, 0 },
        { 0, 0 },   { 0, 0 },   { 7, 14 },  { 9, 11 },  { 10, 11 }, { 7, 12 },  { 8, 13 }, { 16, 20 }, { 7, 14 },
        { 8, 15 },  { 18, 24 }, { 8, 22 },  { 8, 16 },  { 24, 24 }, { 5, 3 },   { 9, 12 }, { 7, 12 },  { 8, 12 },
        { 8, 16 },  { 15, 19 }, { 12, 11 }, { 16, 16 }, { 8, 15 },  { 8, 12 },  { 3, 9 },  { 12, 16 }, { 8, 14 },
        { 9, 16 },  { 0, 0 },   { 0, 0 },   { 0, 0 },   { 0, 0 },   { 0, 0 },
    };

    *left = sGlyphBearings[cur - ' '][0] - 4;
    *right = sGlyphBearings[cur - ' '][1] - 8;
    *left += Credits_GetKerning(prev, cur);
}

/**
 * Returns the width of `str` in quarter pixels when drawn with `ctx`'s active font set.
 * Bytes from 0x80 start a two-byte EUC-JP character.
 */
s32 Credits_MeasureText(FontContext* ctx, u8* str) {
    static s32 sMeasurePrevChar = 0; // Last character measured.

    s32 width;
    s32 c;
    s32 prev;
    FontSet* set;
    s8 trim[2]; // Left and right.

    width = 0;
    set = &ctx->unk_00[ctx->setIndex];
    c = *str++;
    if (c != 0) {
        prev = sMeasurePrevChar;
        do {
            if ((c >= 0x80) && (*str != 0)) {
                str++;
                trim[0] = 0;
                trim[1] = 0;
            } else if ((c >= 0x20) && (c < 0x7F)) {
                Credits_GetGlyphTrim(prev, c, &trim[0], &trim[1]);
            } else {
                trim[0] = 0;
                trim[1] = 0;
            }
            prev = c;
            c = *str++;
            width = ((width + ((set->cellWidth + ctx->letterSpacing) * 4)) - trim[0]) - trim[1];
        } while (c != 0);
        sMeasurePrevChar = prev;
    }
    return width - ((ctx->letterSpacing + 2) << 2);
}

/**
 * Draws the bound glyph texture as a quad of four vertices, for the 3D-projected staff roll.
 * `x`, `y`, `width` and `height` are in quarter pixels from the top left of the screen.
 */
void Credits_DrawGlyphQuad(s16 x, s16 y, s16 width, s16 height) {
    Vtx* vtx = Gfx_AllocDisplayList(sizeof(Vtx) * 4);

    x -= 0x280;
    y -= 0x1E0;

    Gfx_SetVertexAttributes(&vtx[0], x, y, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
    Gfx_SetVertexAttributes(&vtx[1], x + width, y, 0, width << 3, 0, 0xFF, 0xFF, 0xFF, 0xFF);
    Gfx_SetVertexAttributes(&vtx[2], x, y + height, 0, 0, height << 3, 0xFF, 0xFF, 0xFF, 0xFF);
    Gfx_SetVertexAttributes(&vtx[3], x + width, y + height, 0, width << 3, height << 3, 0xFF, 0xFF, 0xFF, 0xFF);

    gSPVertex(gDisplayListHead++, vtx, 4, 0);
    gSP2Triangles(gDisplayListHead++, 0, 2, 3, 0, 0, 3, 1, 0);
}

/**
 * Draws glyph `c` at (`x`, `y`) in quarter pixels and returns the x of the next glyph. The
 * second round draws a texture rectangle; the first round draws a projected quad.
 */
s32 Credits_DrawGlyph(s32 x, s32 y, s16 c) {
    FontSet* set = &sFontContext->unk_00[sFontContext->setIndex];
    s32 offset = Font_MapGlyphCode(c) * set->width * set->height;

    if ((sFontContext->flags & 1) && !(sFontContext->flags & 2)) {
        sFontContext->flags |= 2;

        gDPLoadMultiBlock(gDisplayListHead++, (u32)set->texture + ((set->width << 8) * set->height), 0x0100, 1,
                          G_IM_FMT_I, G_IM_SIZ_8b, set->width, set->height, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                          G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    }

    if (set->texture + offset != sFontContext->unk_4C) {
        sFontContext->unk_4C = set->texture + offset;

        gDPLoadTextureBlock(gDisplayListHead++, (u32)set->texture + offset, G_IM_FMT_IA, G_IM_SIZ_8b, set->width,
                            set->height, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    }

    if (sRoleTextColumn != 0) {
        gSPTextureRectangle(gDisplayListHead++, x, y, x + ((set->cellWidth - 1) << 2), y + (set->height * 4),
                            G_TX_RENDERTILE, 0, 0, 0x400, 0x400);
        return ((set->cellWidth + sFontContext->letterSpacing) * 4) + x;
    } else {
        Credits_DrawGlyphQuad(x, y, set->cellWidth * 4, set->height * 4);
        return ((set->cellWidth + sFontContext->letterSpacing) * 4) + x;
    }
}

char* Credits_CopyPrintfBytes(char* buffer, const char* data, u32 size) {
    return (char*)memcpy(buffer, data, size) + size;
}

/**
 * Like Font_Printf, but at (`x`, `y`) in quarter pixels and with Credits_DrawGlyph. The
 * full-width space 0xA1A1 is trimmed like an ASCII space.
 */
void Credits_Printf(s32 x, s32 y, const char* fmt, ...) {
    static s32 sPrintPrevChar = 0; // Last character drawn.

    u8 buffer[0x100];
    s32 len;
    s32 c;
    s32 start_x = x;
    s8 trim[2]; // Left and right.
    s32 draw;
    u8* p;

    va_list args;
    va_start(args, fmt);

    len = _Printf(Credits_CopyPrintfBytes, (char*)buffer, fmt, args);
    if (len > 0) {
        buffer[len] = 0;
        c = buffer[0];
        p = &buffer[1];

        while (c != 0) {
            draw = 1;
            if (!(sFontContext->flags & 4) && (c == '\n')) {
                x = start_x;
                draw = 0;
                y += sFontContext->lineHeight * 4;
            }

            if (draw) {
                if ((c >= 0x80) && (*p != 0)) {
                    c = (c << 8) | *p;
                    p++;
                    if (c == 0xA1A1) {
                        Credits_GetGlyphTrim(' ', ' ', &trim[0], &trim[1]);
                    } else {
                        trim[0] = 0;
                        trim[1] = 0;
                    }
                } else if ((c >= 0x20) && (c < 0x7F)) {
                    Credits_GetGlyphTrim(sPrintPrevChar, c, &trim[0], &trim[1]);
                } else {
                    trim[0] = 0;
                    trim[1] = 0;
                }
                x = Credits_DrawGlyph(x - trim[0], y, c & 0x7F7F) - trim[1];
                // FAKE: keeps trim[1] in a register.
                if (trim[1]) {}
            }

            sPrintPrevChar = c;
            c = *p++;
        }
    }

    va_end(args);
}
#endif
