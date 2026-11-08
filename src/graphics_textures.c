#include "global.h"
#include "graphics_textures.h"
#include "include/string.h"
#include "src/jpeg_stream.h"
#include "src/gfx_buffer.h"
#include "src/ui_graphics.h"
#include "src/gfx_rect.h"
#include "src/matrix.h"
#include "src/memory.h"
#include "stdarg.h"
#include "lib/ultralib/src/libc/xstdio.h"

FontContext* gFontContext;

void Gfx_DrawTexturedRectClipped(s16 x1, s16 y1, s16 width, s16 height, s16 texture_start_x, s16 texture_start_y,
                                 s16 texture_step_x, s16 texture_step_y, s32 texture_spread_x_mode) {
    struct Rectangle rectangle;

    GfxRect_SetBounds(&rectangle, x1, y1, (x1 + width) - 1, (y1 + height) - 1);

    if (GfxRect_ClipToScissor(&rectangle) != 0) {
        if (texture_step_x != 0) {
            texture_start_x += ((rectangle.left - x1) * texture_step_x) >> 5;
        }

        if (texture_step_y != 0) {
            texture_start_y += ((rectangle.top - y1) * texture_step_y) >> 5;
        }

        if (texture_spread_x_mode == 0x200000) {
            texture_step_x *= 4;
        } else {
            rectangle.right += 1;
            rectangle.bottom += 1;
        }

        gSPTextureRectangle(gDisplayListHead++, rectangle.left * 4, rectangle.top * 4, rectangle.right * 4,
                            rectangle.bottom * 4, G_TX_RENDERTILE, texture_start_x, texture_start_y, texture_step_x,
                            texture_step_y);
    }
}

void Gfx_DrawTexturedRect(s32 x1, s32 y1, s32 width, s32 height, s32 texture_step_x, s32 texture_step_y,
                          s32 texture_step_x_mode) {
    Gfx_DrawTexturedRectClipped(x1, y1, (width - x1) + 1, (height - y1) + 1, 0, 0, texture_step_x, texture_step_y,
                                texture_step_x_mode);
}

void Gfx_FillRectRgb(s16 x1, s16 y1, s16 width, s16 height, u8 r, u8 g, u8 b) {
    gDPSetPrimColor(gDisplayListHead++, 0, 0, r, g, b, 0xFF);

    Gfx_DrawTexturedRectClipped(x1, y1, width, height, 0, 0, 0, 0, 0);
}

void Gfx_FillRectRgba(s16 x1, s16 y1, s16 width, s16 height, u8 r, u8 g, u8 b, u8 alpha) {
    gDPSetPrimColor(gDisplayListHead++, 0, 0, r, g, b, alpha);

    Gfx_DrawTexturedRectClipped(x1, y1, width, height, 0, 0, 0, 0, 0);
}

void Gfx_DrawTextureRgba16(s16 x1, s16 y1, s16 draw_width, s16 height, u8* texture, s16 load_width,
                           s32 texture_spread_x_mode) {
    gDPLoadTextureBlock(gDisplayListHead++, texture, G_IM_FMT_RGBA, G_IM_SIZ_16b, load_width, height, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(x1, y1, draw_width, height, 0, 0, 1024, 1024, texture_spread_x_mode);
}

void Gfx_DrawTextureRgba32(s16 x1, s16 y1, s16 draw_width, s16 height, u8* texture, s16 load_width,
                           s32 texture_spread_x_mode) {
    gDPLoadTextureBlock(gDisplayListHead++, texture, G_IM_FMT_RGBA, G_IM_SIZ_32b, load_width, height, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(x1, y1, draw_width, height, 0, 0, 0x400, 0x400, texture_spread_x_mode);
}

void Gfx_DrawTextureIa8(s16 x1, s16 y1, s16 draw_width, s16 height, u8* texture, s16 load_width,
                        s32 texture_spread_x_mode) {
    gDPLoadTextureBlock(gDisplayListHead++, texture, G_IM_FMT_IA, G_IM_SIZ_8b, load_width, height, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    Gfx_DrawTexturedRectClipped(x1, y1, draw_width, height, 0, 0, 0x400, 0x400, texture_spread_x_mode);
}

void Gfx_DrawTextureIa16(s16 x1, s16 y1, s16 draw_width, s16 height, u8* texture, s16 load_width,
                         s32 texture_spread_x_mode) {
    gDPLoadTextureBlock(gDisplayListHead++, texture, G_IM_FMT_IA, G_IM_SIZ_16b, load_width, height, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    Gfx_DrawTexturedRectClipped(x1, y1, draw_width, height, 0, 0, 0x400, 0x400, texture_spread_x_mode);
}

void Gfx_DrawTextureI4(s16 x1, s16 y1, s16 draw_width, s16 height, u8* texture, s16 load_width,
                       s32 texture_spread_x_mode) {
    gDPLoadTextureBlock_4b(gDisplayListHead++, texture, G_IM_FMT_I, load_width, height, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                           G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(x1, y1, draw_width, height, 0, 0, 0x400, 0x400, texture_spread_x_mode);
}

void Gfx_DrawTextureI8(s16 x1, s16 y1, s16 draw_width, s16 height, u8* texture, s16 load_width,
                       s32 texture_spread_x_mode) {
    gDPLoadTextureBlock(gDisplayListHead++, texture, G_IM_FMT_I, G_IM_SIZ_8b, load_width, height, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(x1, y1, draw_width, height, 0, 0, 0x400, 0x400, texture_spread_x_mode);
}

void Gfx_DrawTextureI16(s16 x1, s16 y1, s16 draw_width, s16 height, u8* texture, s16 load_width,
                        s32 texture_spread_x_mode) {
    gDPLoadTextureBlock(gDisplayListHead++, texture, G_IM_FMT_I, G_IM_SIZ_16b, load_width, height, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(x1, y1, draw_width, height, 0, 0, 0x400, 0x400, texture_spread_x_mode);
}

void Gfx_DrawTextureRgba16Ia8(s16 x1, s16 y1, s16 draw_width, s16 height, u8* texture, u8* texture_multi,
                              s16 load_width, s32 texture_x_spread_mode) {
    gDPLoadTextureBlock(gDisplayListHead++, texture, G_IM_FMT_RGBA, G_IM_SIZ_16b, load_width, height, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gDPLoadMultiBlock(gDisplayListHead++, texture_multi, 0x0100, 1, G_IM_FMT_I, G_IM_SIZ_8b, load_width, height, 0,
                      G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                      G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(x1, y1, draw_width, height, 0, 0, 0x400, 0x400, texture_x_spread_mode);
}

void Gfx_DrawTiledRgba16Image(u8* image) {
    s32 i;
    s32 j;

    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, 640, 480);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGB, G_CC_DECALRGB);
    gDPPipeSync(gDisplayListHead++);

    for (i = 0; i < 0x1E0; i += 0x20) {
        for (j = 0; j < 0x280; j += 0x20) {
            gDPLoadTextureBlock(gDisplayListHead++, image, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 16, 0,
                                G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(gDisplayListHead++, j << 2, i << 2, (j + 0x20) << 2, (i + 0x20) << 2, G_TX_RENDERTILE,
                                0, 0, 0x0200, 0x0200);
            image += 0x200;
        }
    }
}


/* Restores a rectangle from a tiled background image using its starting position and dimensions. */
void Gfx_DrawRgba16ImageRegion(u32 image_address, s16 x1, s16 y1, s16 width, s16 height) {
    s32 div = 32;
    s16 end_x;
    s16 end_y;
    s16 sp36;
    s16 sp30;
    s16 j;
    s16 i;
    s16 final_y1;
    s16 final_x1;
    s16 final_x2;
    s16 final_y2;
    s16 texture_start_x;
    s16 texture_start_y;

    end_x = x1 + width;
    end_y = y1 + height;

    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, 640, 480);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGB, G_CC_DECALRGB);
    gDPPipeSync(gDisplayListHead++);

    for (i = (y1 >> 5) << 5; i < end_y; i += 32) {
        for (j = (x1 >> 5) << 5; j < end_x; j += 32) {
            if (j < x1) {
                final_x1 = x1;
            } else {
                final_x1 = j;
            }

            if (end_x < j + 32) {
                final_x2 = end_x;
            } else {
                final_x2 = j + 32;
            }

            if (i < y1) {
                final_y1 = y1;
            } else {
                final_y1 = i;
            }

            if (end_y < i + 32) {
                final_y2 = end_y;
            } else {
                final_y2 = i + 32;
            }

            texture_start_x = final_x1 - j;
            texture_start_y = final_y1 - i;

            gDPLoadTextureBlock(gDisplayListHead++, ((((i / div) * 20) + (j / div)) << 9) + image_address, G_IM_FMT_RGBA,
                                G_IM_SIZ_16b, 16, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(gDisplayListHead++, final_x1 * 4, final_y1 * 4, final_x2 * 4, final_y2 * 4, G_TX_RENDERTILE,
                                texture_start_x << 4, texture_start_y << 4, 0x0200, 0x0200);
        }
    }
}

void Gfx_DrawTiledImageMesh(s16 arg0, s16 arg1, s16 arg2, f32 arg3, Vtx* arg4, u32 arg5) {
    s16 j;
    s16 i;
    Vec3s spCC;
    Vec3f spC0;
    Vec3f spB4;
    MtxF* temp_s4;
    DepthBuffer* temp_s1;
    Vp* temp_s2;
    MtxF* temp_s0;
    MtxF* temp_s3;

    temp_s3 = Gfx_AllocDisplayList(0x40);
    temp_s4 = Gfx_AllocDisplayList(0x40);
    temp_s0 = Gfx_AllocDisplayList(0x40);
    temp_s2 = Gfx_AllocDisplayList(0x10);
    temp_s1 = GfxImage_GetCurrent();

    Vec3s_SetComponents(&spCC, 0, 0, arg2);
    Vec3f_SetComponentsDuplicate(&spC0, arg0, arg1, 0.0f);
    Vec3f_SetComponentsDuplicate(&spB4, arg3, arg3, 1.0f);
    MtxF_SetRotationTranslationF(temp_s0, &spC0, &spCC);
    MtxF_ScaleRows(temp_s0, temp_s0, &spB4);
    MtxF_ToFixed(temp_s3, temp_s0);
    Gfx_SetViewportDimensions(temp_s2, temp_s1->width, temp_s1->height);

    gSPViewport(gDisplayListHead++, (u32)temp_s2 & 0x1FFFFFFF);

    guOrtho(temp_s4, 0.5f, temp_s1->width - 0.5f, temp_s1->height - 0.5f, 0.5f, -2.0f, 2.0f, 1.0f);

    gSPMatrix(gDisplayListHead++, (u32)temp_s4 & 0x1FFFFFFF, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPMatrix(gDisplayListHead++, (u32)temp_s3 & 0x1FFFFFFF, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_PERSP);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGB, G_CC_DECALRGB);
    gSPClearGeometryMode(gDisplayListHead++, G_ZBUFFER | G_CULL_BOTH | G_LIGHTING);
    gSPTexture(gDisplayListHead++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);

    for (i = 0; i < 15; i++) {
        for (j = 0; j < 20; j++) {
            gDPLoadTextureBlock(gDisplayListHead++, arg5, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 16, 0,
                                G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
            gSPVertex(gDisplayListHead++, arg4, 4, 0);
            gSP2Triangles(gDisplayListHead++, 0, 1, 2, 0, 0, 2, 3, 0);

            arg5 += 0x200;
            arg4 += 4;
        }
    }
}

Vtx* func_8001E490(void) {
    Vtx* sp54;
    Vtx* var_s0;
    s16 i;
    s16 j;

    sp54 = main_pool_alloc(0x4B00, 0);
    if (sp54 != NULL) {
        var_s0 = sp54;

        for (i = -0xF0; i < 0xF0; i += 0x20) {
            for (j = -0x140; j < 0x140; j += 0x20) {
                Gfx_SetVertexAttributes(var_s0++, j + 0, i + 0, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
                Gfx_SetVertexAttributes(var_s0++, j + 0, i + 0x20, 0, 0, 0x200, 0xFF, 0xFF, 0xFF, 0xFF);
                Gfx_SetVertexAttributes(var_s0++, j + 0x20, i + 0x20, 0, 0x200, 0x200, 0xFF, 0xFF, 0xFF, 0xFF);
                Gfx_SetVertexAttributes(var_s0++, j + 0x20, i + 0, 0, 0x200, 0, 0xFF, 0xFF, 0xFF, 0xFF);
            }
        }
    }

    return sp54;
}

void Gfx_SetVertexAttributes(Vtx* vertex, s16 x, s16 y, s16 z, s16 texture_x, s16 texture_y, u8 r, u8 g, u8 b,
                             u8 alpha) {
    vertex->v.ob[0] = x;
    vertex->v.ob[1] = y;
    vertex->v.ob[2] = z;
    vertex->v.flag = 0;
    vertex->v.tc[0] = texture_x;
    vertex->v.tc[1] = texture_y;
    vertex->v.cn[0] = r;
    vertex->v.cn[1] = g;
    vertex->v.cn[2] = b;
    vertex->v.cn[3] = alpha;
}

void Gfx_SetViewportDimensions(Vp* viewport, s16 width, s16 height) {
    viewport->vp.vtrans[0] = width * 2;
    viewport->vp.vtrans[1] = height * 2;
    viewport->vp.vtrans[2] = 0x1FF;
    viewport->vp.vtrans[3] = 0;
    viewport->vp.vscale[0] = width * 2;
    viewport->vp.vscale[1] = height * 2;
    viewport->vp.vscale[2] = 0x1FF;
    viewport->vp.vscale[3] = 0;
}

u8 Font_MapGlyphCode(u16 index) {
    u8 glyph_code = 0;

    if ((index >= 0x20) && (index < 0x80)) {
        glyph_code = ((u8*)gUiRestoreTextureDL)[index];
    } else if ((index >= 0xA0) && (index < 0x100)) {
        glyph_code = ((u8*)D_8006F608)[sizeof(Gfx) + index];
    }

    return glyph_code;
}

u8 Font_GetGlyphAdvance(FontSet* font_set, u16 index) {
    s32 idx = Font_MapGlyphCode(index);

    return font_set->glyphWidths[idx];
}

void Font_ResetContext(UNUSED s32 arg0, UNUSED s32 arg1) {
    gFontContext = NULL;
}

void Font_FreeSetAsset(UNUSED s32 arg0, s32 arg1) {
    if (gFontContext != NULL) {
        gFontContext->unk_00[arg1].glyphWidths = 0;
        gFontContext->unk_00[arg1].texture = 0;
    }
}

s32 Font_GetSetIndexFromMask(s32 mask) {
    s32 index = 0;

    if (mask != 0x1) {
        if (mask == 0x2) {
            index = 1;
        } else if (mask == 0x4) {
            index = 2;
        } else if (mask == 0x8) {
            index = 3;
        } else if (mask == 0x10) {
            index = 4;
        } else if (mask == 0x20) {
            index = 5;
        }
    }

    return index;
}

void Font_LoadSet(s32 arg0, s32 arg1, s32 arg2) {
    s32 sp1C;
    ret_func_8000484C* temp_v0_2;

    sp1C = Font_GetSetIndexFromMask(arg0);
    if (gFontContext->unk_00[sp1C].glyphWidths == 0) {
        temp_v0_2 = BinArchive_GetFile(gFontContext->unk_48, sp1C);
        gFontContext->setIndex = sp1C;
        gFontContext->unk_00[sp1C].width = arg1;
        gFontContext->unk_00[sp1C].height = arg2;
        gFontContext->unk_00[sp1C].glyphWidths = temp_v0_2;
        gFontContext->unk_00[sp1C].texture = &temp_v0_2->unk_90;
        main_pool_set_func(temp_v0_2, sp1C, &Font_FreeSetAsset);
    }
}

FontContext* Font_Init(s32 load_size, s32 side) {
    gFontContext = main_pool_alloc_with_func(sizeof(*gFontContext), side, 'PRNT', Font_ResetContext);
    bzero(gFontContext, sizeof(*gFontContext));
    gFontContext->unk_48 = ASSET_LOAD2(fonts, 1, 1);
    Font_LoadSizes(load_size);

    return gFontContext;
}

void Font_Free(void) {
    main_pool_try_free(gFontContext);
}

void Font_LoadSizes(s32 arg0) {
    if (gFontContext != NULL) {
        if (arg0 & 0x20) {
            Font_LoadSet(0x20, 0x20, 0x20);
        }

        if (arg0 & 0x10) {
            Font_LoadSet(0x10, 0x18, 0x18);
        }

        if (arg0 & 8) {
            Font_LoadSet(8, 0x18, 0x14);
        }

        if (arg0 & 4) {
            Font_LoadSet(4, 0x10, 0x10);
        }

        if (arg0 & 2) {
            Font_LoadSet(2, 0x10, 0xC);
        }

        if (arg0 & 1) {
            Font_LoadSet(1, 0x10, 0xA);
        }
    }
}

void Font_UnloadSizes(s32 arg0) {
    if (gFontContext != NULL) {
        if (arg0 & 1) {
            if (gFontContext->unk_00[0].glyphWidths != NULL) {
                main_pool_try_free(gFontContext->unk_00[0].glyphWidths);
            }
        }

        if (arg0 & 2) {
            if (gFontContext->unk_00[1].glyphWidths != NULL) {
                main_pool_try_free(gFontContext->unk_00[1].glyphWidths);
            }
        }

        if (arg0 & 4) {
            if (gFontContext->unk_00[2].glyphWidths != NULL) {
                main_pool_try_free(gFontContext->unk_00[2].glyphWidths);
            }
        }

        if (arg0 & 8) {
            if (gFontContext->unk_00[3].glyphWidths != NULL) {
                main_pool_try_free(gFontContext->unk_00[3].glyphWidths);
            }
        }

        if (arg0 & 0x10) {
            if (gFontContext->unk_00[4].glyphWidths != NULL) {
                main_pool_try_free(gFontContext->unk_00[4].glyphWidths);
            }
        }

        if (arg0 & 0x20) {
            if (gFontContext->unk_00[5].glyphWidths != NULL) {
                main_pool_try_free(gFontContext->unk_00[5].glyphWidths);
            }
        }
    }
}

void Font_SetActive(s32 mask, s32 letter_spacing) {
    if (gFontContext != NULL) {
        gFontContext->setIndex = Font_GetSetIndexFromMask(mask);
        gFontContext->letterSpacing = letter_spacing;
        gFontContext->flags &= ~2;
    }
}

s32 Font_GetSetInfo(s32 arg0, FontSet* arg1) {
    s32 var_v1 = 0;

    if ((arg1 != NULL) && (gFontContext != NULL)) {
        *arg1 = gFontContext->unk_00[Font_GetSetIndexFromMask(arg0)];
        var_v1 = 1;
    }

    return var_v1;
}

s16 Font_DrawChar(s16 x, s16 y, s16 char_index) {
    FontSet* character;
    s32 sp88;
    s16 width;

    character = &gFontContext->unk_00[gFontContext->setIndex];
    sp88 = Font_MapGlyphCode(char_index) * character->width * character->height;
    width = Font_GetGlyphAdvance(character, char_index);

    if ((gFontContext->flags & 1) && !(gFontContext->flags & 2)) {
        gFontContext->flags |= 2;

        gDPLoadMultiBlock(gDisplayListHead++, (u32)character->texture + (character->width * 0x90 * character->height), 0x0100,
                          1, G_IM_FMT_I, G_IM_SIZ_8b, character->width, character->height, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                          G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    }

    if ((u32)character->texture + sp88 != gFontContext->unk_4C) {
        gFontContext->unk_4C = (u32)character->texture + sp88;

        gDPLoadTextureBlock(gDisplayListHead++, (u32)character->texture + sp88, G_IM_FMT_IA, G_IM_SIZ_8b, character->width,
                            character->height, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    }

    Gfx_DrawTexturedRectClipped(x, y, width, character->height, 0, 0, 0x400, 0x400, 0);

    x += width + gFontContext->letterSpacing;
    if (gFontContext->setIndex >= 2) {
        x -= 2;
    } else {
        x -= 1;
    }

    return x;
}

char* String_CopyBytesAndAdvance(char* buffer, const char* data, u32 size) {
    return (char*)memcpy(buffer, data, size) + size;
}

void Font_Printf(s32 x, s32 y, const char* fmt, ...) {
    char sp50[0x100];
    s32 temp_s5 = x - 1;
    s32 var_s1 = x - 1;
    s32 temp_v0;
    s32 var_v0;
    char* var_s0;
    s16 chr;

    va_list args;
    va_start(args, fmt);

    temp_v0 = _Printf(String_CopyBytesAndAdvance, sp50, fmt, args);

    if (temp_v0 > 0) {
        sp50[temp_v0] = 0;
        chr = *sp50;
        var_s0 = &sp50[1];

        while (chr != 0) {
            var_v0 = 1;

            if (!(gFontContext->flags & 4) && (chr == 0xA)) {
                var_s1 = temp_s5;
                var_v0 = 0;
                y += gFontContext->lineHeight;
            }

            if (var_v0 != 0) {
                var_s1 = Font_DrawChar(var_s1, y, chr & 0xFF);
            }

            chr = *var_s0;
            var_s0++;
        }
    }

    va_end(args);
}

void Font_DrawCharAt(s32 x, s32 y, s32 char_index) {
    Font_DrawChar(x - 1, y, char_index & 0xFF);
}

void Gfx_SetEnvColor(s32 r, s32 g, s32 b, s32 alpha) {
    gDPSetEnvColor(gDisplayListHead++, r, g, b, alpha);
}

void Gfx_SetPrimColor(s32 r, s32 g, s32 b, s32 alpha) {
    gDPSetPrimColor(gDisplayListHead++, 0, 0, r, g, b, alpha);
}

void Font_SetLineHeight(s32 line_height) {
    gFontContext->lineHeight = line_height;
}

void Font_DisableLineBreak(void) {
    gFontContext->flags |= 4;
}

void Font_EnableLineBreak(void) {
    gFontContext->flags &= ~4;
}

void Font_BeginTranslucentTextRendering(void) {
    gFontContext->flags = 0;
    gFontContext->lineHeight = 0x28;
    gFontContext->unk_4C = 0;

    gSPDisplayList(gDisplayListHead++, D_8006F710);
}

void Font_EndTexturedTextRendering(void) {
    gSPDisplayList(gDisplayListHead++, D_8006F750);
}

void Font_EnableTwoCycleTexturing(void) {
    gFontContext->flags |= 1;
    gFontContext->flags &= ~2;

    gSPDisplayList(gDisplayListHead++, D_8006F768);
}

void Font_DisableTwoCycleTexturing(void) {
    gFontContext->flags &= ~3;
    gSPDisplayList(gDisplayListHead++, D_8006F798);
}

void Gfx_FillRectWithDisplayState(s32 x1, s32 y1, s32 width, s32 length, s32 r, s32 g, s32 b, s32 alpha) {
    gSPDisplayList(gDisplayListHead++, gUiTranslucentFillDL);

    Gfx_FillRectRgba(x1, y1, width, length, r, g, b, alpha);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

s32 Font_MeasureTextExtent(s32 x, s32 y, const char* fmt, ...) {
    s32 temp_s0;
    s32 temp_v0;
    s32 var_s2;
    char sp5C[0x100];
    s32 var_a0;
    s32 chr;
    u8* var_s1;
    FontSet* temp_s4;

    va_list args;
    va_start(args, fmt);

    var_s2 = 0;
    if (x == 0) {
        var_a0 = gFontContext->setIndex;
    } else {
        var_a0 = Font_GetSetIndexFromMask(x);
    }

    temp_s4 = &gFontContext->unk_00[var_a0];
    if (temp_s4->glyphWidths != NULL) {
        temp_s0 = 0;
        temp_v0 = _Printf(String_CopyBytesAndAdvance, sp5C, fmt, args);
        if (temp_v0 > 0) {
            sp5C[temp_v0] = 0;

            chr = *sp5C;
            var_s1 = &sp5C[1];

            while (chr != 0) {
                if (chr == 0xA) {
                    temp_s0 = 0;
                } else {
                    temp_s0 += Font_GetGlyphAdvance(temp_s4, chr & 0xFF) + y;
                    if (var_a0 >= 2) {
                        temp_s0 -= 2;
                    } else {
                        temp_s0 -= 1;
                    }
                }

                if (var_s2 < temp_s0) {
                    var_s2 = temp_s0;
                }

                chr = *var_s1;
                var_s1++;
            }
        }
    }

    va_end(args);

    return var_s2;
}

u8 Font_GetCharWidth(UNUSED s32 arg0, s32 index) {
    return Font_GetGlyphAdvance(&gFontContext->unk_00[gFontContext->setIndex], index & 0xFF);
}
