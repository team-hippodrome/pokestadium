#include "poke_icon.h"
#include "src/geo_node.h"
#include "src/geo_render.h"
#include "src/model_animation.h"
#include "src/display_object_textures.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/jpeg_stream.h"
#include "src/gfx_rect.h"
#include "src/matrix.h"
#include "src/dp_intro.h"
#include "src/geo_layout.h"
#include "src/memory.h"

static DisplayObject* D_800AC830;
static s32 D_800AC834;

s32 PokeIcon_CapturePreviewGeoNode(s32, GraphNode*);

s32 PokeIcon_CapturePreviewGeoNode(s32 arg0, GraphNode* arg1) {
    if (arg0 == 0) {
        D_800AC830 = arg1;
    }
    return 0;
}

void PokeIcon_InitPreviewScene(unk_func_8001B1FC* arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4) {
    static u32 D_8006F3E0[] = {
        0x0C00FFFF, 0x05000000, 0x0B00001E, 0x00000000, 0x010000E0, 0x00000000, 0x0000000A,
        0x015404E2, 0x05000000, 0x0D000000, 0x05000000, 0x14000000, 0x002D013B, 0xFFFFFF32,
        0x14000000, 0xFFD30087, 0x80808032, 0x16646464, 0x0F000001, 0x05000000, 0x1F000000,
        0x00000000, 0x00000000, 0xFF060000, 0x00640064, 0x00640000, 0x08000000, PokeIcon_CapturePreviewGeoNode,
        0x00000000, 0x06000000, 0x06000000, 0x06000000, 0x06000000, 0x01000000, 0x00000000,
        0x00000000,
    };
    MemoryBlock* sp24;
    MemoryBlock* temp_v0;

    sp24 = MainPool_AllocState(main_pool_get_available(), 0);
    arg0->unk_20 = process_geo_layout(sp24, D_8006F3E0);
    arg0->unk_24 = D_800AC830;
    MainPool_FinalizeAllocation(sp24);
    GeoCamera_SetViewport(arg0->unk_20->children, arg1, arg2, arg3, arg4);
    GeoCamera_SetPerspective(arg0->unk_20->children, 30.0f, 100.0f, 12800.0f);
}

unk_func_8001B1FC* PokeIcon_CreateModelPreview(ModelLoadContext* arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4, s16 arg5,
                                               s16 arg6, u8* arg7) {
    unk_func_8001B1FC* temp_v0 = main_pool_alloc(sizeof(unk_func_8001B1FC), 0);

    if (temp_v0 != NULL) {
        temp_v0->unk_00 = 6;
        temp_v0->unk_1C = arg0;
        temp_v0->unk_14 = 0;
        temp_v0->unk_04 = arg1;
        temp_v0->unk_06 = arg2;
        temp_v0->unk_08 = arg5;
        temp_v0->unk_0A = arg6;
        temp_v0->unk_0C = arg7;
        temp_v0->unk_28 = GfxImage_Allocate(0, 2, arg1, arg2, 0);
        temp_v0->unk_2C = GfxImage_Allocate(0, 2, arg1, arg2, 1);
        GfxImage_AttachDepthBuffer(temp_v0->unk_28, temp_v0->unk_2C);
        PokeIcon_InitPreviewScene(temp_v0, arg3, arg4, arg5, arg6);
    }
    return temp_v0;
}

void PokeIcon_SetPreviewMon(unk_func_8001B1FC* arg0, BattleMon* arg1, u16 arg2) {
    arg1_func_80010CA8 sp2C;
    s16 tmp1 = (s16)(D_8006FF00[arg1->species.id - 1].unk_14 >> 0x10) >> 6;
    s16 tmp2 = (s16)(D_8006FF00[arg1->species.id - 1].unk_14 >> 6) >> 4;
    s16 tmp3 = (s16)(D_8006FF00[arg1->species.id - 1].unk_14 << 6) >> 6;

    arg0->unk_00 |= 1;
    arg0->unk_14 = arg1;
    arg0->unk_12 = 0x100;
    arg0->unk_10 = 0;
    arg0->unk_02 = arg2;
    arg0->unk_18 = D_8006FF00[arg1->species.id - 1].unk_02 / 100.0f;

    arg0->unk_24->position.x = tmp1;
    arg0->unk_24->position.y = tmp2;
    arg0->unk_24->position.z = tmp3;

    arg0->unk_24->rotation = D_8006F05C;
    Model_ComputeSizeVariant(&sp2C, arg1);
    PokeIcon_RequestFrameLoad(arg0->unk_1C, arg1->species.id, sp2C);
    PokeIcon_WaitFrameLoad(arg0->unk_1C);
    ModelRenderer_ClearDisplayObject(arg0->unk_24);
    Model_InitDisplayObject(arg0->unk_24, 0, arg1->species.id, arg0->unk_1C->loadedModel->modelRoot->unk_00[0]);
    ModelAnim_SetAnimation(arg0->unk_24, 1);
    arg0->unk_24->node.renderFlags &= ~0x40;
    arg0->unk_24->node.renderFlags |= 0x80;
}

void PokeIcon_SetPreviewSpecies(unk_func_8001B1FC* arg0, s32 arg1, u16 arg2) {
    arg1_func_80010CA8 sp34;
    s16 tmp1 = (s16)(D_8006FF00[arg1 - 1].unk_14 >> 0x10) >> 6;
    s16 tmp2 = (s16)(D_8006FF00[arg1 - 1].unk_14 >> 6) >> 4;
    s16 tmp3 = (s16)(D_8006FF00[arg1 - 1].unk_14 << 6) >> 6;

    arg0->unk_00 &= ~1;
    arg0->unk_14 = arg1;
    arg0->unk_12 = 0x100;
    arg0->unk_10 = 0;
    arg0->unk_02 = arg2;
    arg0->unk_18 = D_8006FF00[arg1 - 1].unk_02 / 100.0f;

    arg0->unk_24->position.x = tmp1;
    arg0->unk_24->position.y = tmp2;
    arg0->unk_24->position.z = tmp3;

    arg0->unk_24->rotation = D_8006F05C;
    sp34.raw = 0;
    PokeIcon_RequestFrameLoad(arg0->unk_1C, arg1, sp34);
    PokeIcon_WaitFrameLoad(arg0->unk_1C);
    ModelRenderer_ClearDisplayObject(arg0->unk_24);
    Model_InitDisplayObject(arg0->unk_24, 0, arg1, arg0->unk_1C->loadedModel->modelRoot->unk_00[0]);
    ModelAnim_SetAnimation(arg0->unk_24, 1);
    arg0->unk_24->node.renderFlags &= ~0x40;
    arg0->unk_24->node.renderFlags |= 0x80;
}

s32 PokeIcon_HandlePreviewInput(Vec3s* arg0) {
    s32 var_v1 = 0;

    if (gPlayer1Controller->buttonDown & 0x800) {
        var_v1 = 1;
        arg0->x += 0x400;
    }

    if (gPlayer1Controller->buttonDown & 0x400) {
        var_v1 = 1;
        arg0->x -= 0x400;
    }

    if (gPlayer1Controller->buttonDown & 0x200) {
        var_v1 = 1;
        arg0->y += 0x400;
    }

    if (gPlayer1Controller->buttonDown & 0x100) {
        var_v1 = 1;
        arg0->y -= 0x400;
    }

    return var_v1;
}

s32 func_8001B6BC(Vec3s* arg0) {
    return 0;
}

void PokeIcon_AdvancePreviewMotion(unk_func_8001B1FC* arg0) {
    s32 var_a0;
    Vec3s* sp18 = &arg0->unk_24->rotation;
    s16 var_a2;
    s16 temp_lo;

    if (arg0->unk_12 < 0x100) {
        arg0->unk_12 += 0x10;
    }
    sp18->y += arg0->unk_12;
    var_a2 = arg0->unk_12;

    if (sp18->x < 0) {
        var_a0 = -sp18->x;
    } else {
        var_a0 = sp18->x;
    }

    temp_lo = var_a0 / 12;
    if (temp_lo < 8) {
        temp_lo = 8;
    }

    if (temp_lo < var_a2) {
        var_a2 = temp_lo;
    }
    sp18->x = Math_StepToS(sp18->x, 0, var_a2);
}

void PokeIcon_UpdatePreview(unk_func_8001B1FC* arg0) {
    f32 temp_fv0;
    s16 temp_v0_2;
    s32 temp_v0;
    s32 var_v1;

    var_v1 = 0;
    if (arg0->unk_00 & 2) {
        var_v1 = PokeIcon_HandlePreviewInput(&arg0->unk_24->rotation);
        if (var_v1 == 0) {
            var_v1 = func_8001B6BC(&arg0->unk_24->rotation);
        }
    }

    if (var_v1 != 0) {
        arg0->unk_12 = 0;
        arg0->unk_10 = 0x78;
    } else if (arg0->unk_00 & 4) {
        if (arg0->unk_10 > 0) {
            arg0->unk_10--;
        } else {
            PokeIcon_AdvancePreviewMotion(arg0);
        }
    }

    Vec3f_SetComponentsDuplicate(&D_800AC830->scale, arg0->unk_18, arg0->unk_18, arg0->unk_18);
}

void PokeIcon_DrawPreview(unk_func_8001B1FC* arg0) {
    s32 var_v0;
    s32 var_s2;
    s32 i;

    if (arg0->unk_0C == 0) {
        GfxImage_FillCurrent(&gDisplayListHead, arg0->unk_02);
        return;
    }

    var_s2 = arg0->unk_0C;

    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_COPY);
    gDPSetRenderMode(gDisplayListHead++, G_RM_NOOP, G_RM_NOOP2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);

    for (i = 0; i < arg0->unk_0A; i += 8) {
        var_v0 = arg0->unk_0A - i;
        if (var_v0 >= 9) {
            var_v0 = 8;
        }
        Gfx_DrawTextureRgba16(0, i, arg0->unk_08, var_v0, var_s2, arg0->unk_04, 0x200000);
        var_s2 += arg0->unk_04 * 0x10;
    }

    gDPPipeSync(gDisplayListHead++);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_PERSP);

    GfxImage_ClearDepthRectangle(&gDisplayListHead, 0, 0, arg0->unk_08, arg0->unk_0A);
}

u8* PokeIcon_RenderPreview(unk_func_8001B1FC* arg0) {
    PokeIcon_UpdatePreview(arg0);
    GfxImage_SetRenderTarget(&gDisplayListHead, arg0->unk_28);
    PokeIcon_DrawPreview(arg0);
    GeoRender_AdvanceFrameCounter();
    Geo_RenderRootNode(arg0->unk_20);

    if (D_800AC834 > 0) {
        Font_BeginTranslucentTextRendering();
        Font_SetActive(8, -4);

        switch (D_800AC834) {
            case 1:
                Font_Printf(0, 0x64, "S:%5.2f  H:%4d", arg0->unk_18, (s32)arg0->unk_24->position.y);
                break;

            case 2:
                Font_Printf(0, 0x64, "X:%4d  Z:%4d", (s32)arg0->unk_24->position.x, (s32)arg0->unk_24->position.z);
                break;

            case 3:
                Font_Printf(0, 0x64, "A:%04X  F:%3d", *(u16*)&arg0->unk_24->rotation.y,
                            arg0->unk_24->transformAnim.frameFixed >> 0x10);
                break;
        }
        Font_EndTexturedTextRendering();
    }
    return arg0->unk_28->img_p;
}
