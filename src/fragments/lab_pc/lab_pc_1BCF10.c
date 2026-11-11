#include "lab_pc.h"
#include "src/table_view.h"
#include "src/memmap.h"

typedef struct unk_func_8820BE14_02C_038_02C_02C {
    /* 0x00 */ char unk00[0x8];
    /* 0x08 */ s32 unk_08;
} unk_func_8820BE14_02C_038_02C_02C; // size >= 0xC

typedef struct unk_func_8820BE14_02C_038_02C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ unk_func_8820BE14_02C_038_02C_02C* unk_2C;
    /* 0x30 */ char unk30[0xE];
    /* 0x3E */ s16 unk_3E;
    /* 0x40 */ char unk40[0xA];
    /* 0x4A */ s16 unk_4A;
    /* 0x4C */ char unk4C[0xC];
    /* 0x58 */ s32 unk_58;
} unk_func_8820BE14_02C_038_02C; // size >= 0x5C

void LabPC_InitTabWidget(unk_func_882149A0* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6,
                         Color_RGBA8 arg7, Color_RGBA8 arg8, Color_RGBA8 arg9) {
    ((func885007CC)Memmap_GetFragmentVaddr(WidgetTree_InitWidget))(arg0, sizeof(unk_func_882149A0));

    arg0->unk_00.unk_18 = LabPC_DrawTabWidget;

    arg0->unk_00.unk_10.unk_00 = arg1;
    arg0->unk_00.unk_10.unk_02 = arg2;
    arg0->unk_00.unk_14.unk_00 = arg3;
    arg0->unk_00.unk_14.unk_02 = arg4;

    arg0->unk_2C = arg5;
    arg0->unk_30 = arg6;
    arg0->unk_32 = arg7;
    arg0->unk_36 = arg8;
    arg0->unk_3A = arg9;
    arg0->unk_40 = 0;
}

s32 LabPC_DrawTabWidget(unk_func_882149A0* arg0, s32 arg1, s32 arg2) {
    Color_RGBA8 sp16C;

    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_POINT);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGBA, G_CC_DECALRGBA);

    gDPLoadTextureBlock(gDisplayListHead++, D_4001638, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, arg1 << 2, arg2 << 2, ((arg0->unk_00.unk_14.unk_00 + arg1) - 8) << 2,
                        (arg2 + 8) << 2, G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, D_4001710, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, ((arg0->unk_00.unk_14.unk_00 + arg1) - 8) << 2, arg2 << 2,
                        (arg0->unk_00.unk_14.unk_00 + arg1) << 2, ((arg2 + arg0->unk_00.unk_14.unk_02) - 8) << 2,
                        G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, D_40016C8, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, (arg1 + 8) << 2, ((arg2 + arg0->unk_00.unk_14.unk_02) - 8) << 2,
                        (arg0->unk_00.unk_14.unk_00 + arg1) << 2, (arg2 + arg0->unk_00.unk_14.unk_02) << 2,
                        G_TX_RENDERTILE, (arg0->unk_00.unk_14.unk_00 * -0x20) + 0x200, 0, 0x0400, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, D_4001680, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, arg1 << 2, (arg2 + 8) << 2, (arg1 + 8) << 2,
                        (arg2 + arg0->unk_00.unk_14.unk_02) << 2, G_TX_RENDERTILE, 0,
                        (arg0->unk_00.unk_14.unk_02 * -0x20) + 0x200, 0x0400, 0x0400);

    if (arg0->unk_00.unk_2A & 0x100) {
        s32 temp_a0;

        gDPPipeSync(gDisplayListHead++);

        gDPSetCycleType(gDisplayListHead++, G_CYC_FILL);
        gDPSetRenderMode(gDisplayListHead++, G_RM_NOOP, G_RM_NOOP2);

        // clang-format off
        temp_a0 = RGBA5551(Color_ScaleRgb(arg0->unk_3A, 1.4f).r, Color_ScaleRgb(arg0->unk_3A, 1.4f).g, Color_ScaleRgb(arg0->unk_3A, 1.4f).b, 1); gDPPipeSync(gDisplayListHead++);
        // clang-format on

        gDPSetFillColor(gDisplayListHead++, (temp_a0 << 0x10) | temp_a0);
        gDPFillRectangle(gDisplayListHead++, arg1 + 1, arg2 + 1, (arg1 + (u32)arg0->unk_00.unk_14.unk_00) - 2,
                         (arg2 + arg0->unk_00.unk_14.unk_02) - 2);
    }

    sp16C = (arg0->unk_00.unk_2A & 0x101) ? arg0->unk_32 : arg0->unk_36;

    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_POINT);
    gDPSetCombineMode(gDisplayListHead++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);

    gDPSetPrimColor(gDisplayListHead++, 0, 0, sp16C.r, sp16C.g, sp16C.b, sp16C.a);

    if (arg0->unk_40 != 0) {
        gDPLoadTextureBlock(gDisplayListHead++, arg0->unk_2C, G_IM_FMT_IA, G_IM_SIZ_16b, arg0->unk_30,
                            arg0->unk_00.unk_14.unk_02 - 6, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        gSPTextureRectangle(gDisplayListHead++, (arg1 + 3) << 2, (arg2 + 3) << 2,
                            ((arg1 + (u32)arg0->unk_00.unk_14.unk_00) - 3) << 2,
                            ((arg2 + arg0->unk_00.unk_14.unk_02) - 3) << 2, G_TX_RENDERTILE,
                            (arg0->unk_00.unk_14.unk_00 - 7) << 5, 0, 0xFC00, 0x0400);
    } else {
        gDPLoadTextureBlock(gDisplayListHead++, arg0->unk_2C, G_IM_FMT_IA, G_IM_SIZ_16b, arg0->unk_30,
                            arg0->unk_00.unk_14.unk_02 - 6, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        gSPTextureRectangle(gDisplayListHead++, (arg1 + 3) << 2, (arg2 + 3) << 2,
                            ((arg1 + (u32)arg0->unk_00.unk_14.unk_00) - 3) << 2,
                            ((arg2 + arg0->unk_00.unk_14.unk_02) - 3) << 2, G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);
    }
    return 0;
}

void LabPC_TabWidget_ToggleFlip(unk_func_882149A0* arg0) {
    arg0->unk_40 = !arg0->unk_40;
}

void LabPC_InitScrollableBoxGrid(unk_func_8820BE14_02C_038* arg0, s32 arg1, s32 arg2, s32 arg3, void (*arg4)(),
                                 s32 arg5, s32 arg6, MemoryPool* arg7) {
    ((func88506DCC)Memmap_GetFragmentVaddr(WidgetTree_InitScrollableGrid))(arg0, arg1, arg2, arg4, arg5, arg6, arg3,
                                                                           arg7);

    arg0->unk_70 = 0;
    arg0->unk_74 = arg0->unk_00.unk_00.unk_1C;
    arg0->unk_78 = arg0->unk_00.unk_00.unk_18;
    arg0->unk_7C = arg0->unk_00.unk_00.unk_20;

    LabPC_SetGridBorderActive(&arg0->unk_00, 1);

    arg0->unk_00.unk_44->unk_00.unk_28 |= 0x100;
}

void LabPC_SetGridBorderActive(unk_func_88200FA0_030_030* arg0, s32 arg1) {
    s32 var_v0;

    if (arg1 != 0) {
        var_v0 = 0;
    } else {
        var_v0 = 1;
    }
    arg0->unk_44->unk_2C = D_88218230[var_v0];
}

void LabPC_BoxGrid_BindMessagePanel(unk_func_8820BE14_02C_038* arg0, unk_func_88507D4C* arg1) {
    arg0->unk_84 = arg1;
    arg0->unk_80 = arg1->unk_00.unk_18;
}

s32 LabPC_ScrollableGrid_UpdateScroll(unk_func_8820BE14_02C_038* arg0) {
    s32 sp24;
    s32 var_a2;
    s32 sp1C;
    s32 var_a3;
    s32 var_v1;

    if (arg0->unk_56 & 0x800) {
        if (!(arg0->unk_54 & 0x800)) {
            sp24 = arg0->unk_00.unk_38 * arg0->unk_00.unk_3E;
            if ((arg0->unk_5C - sp24) < arg0->unk_64) {
                var_a2 = (arg0->unk_5C - sp24);
            } else {
                var_a2 = arg0->unk_64;
            }
            arg0->unk_5C = arg0->unk_5C - var_a2;

            ((func88506FC0)Memmap_GetFragmentVaddr(WidgetTree_ClampScrollableGridTop))(arg0);

            if (sp24 == arg0->unk_5C) {
                arg0->unk_56 &= ~0x800;
            }
        }
    } else if (arg0->unk_56 & 0x400) {
        if (!(arg0->unk_54 & 0x400)) {
            sp1C = ((arg0->unk_00.unk_38 + 1) * arg0->unk_00.unk_3E) - 1;
            if ((sp1C - arg0->unk_5C) < arg0->unk_64) {
                var_a2 = (sp1C - arg0->unk_5C);
            } else {
                var_a2 = arg0->unk_64;
            }
            arg0->unk_5C = arg0->unk_5C + var_a2;

            ((func88506FE0)Memmap_GetFragmentVaddr(WidgetTree_ClampScrollableGridBottom))(arg0);

            if (sp1C == arg0->unk_5C) {
                arg0->unk_56 &= ~0x400;
                arg0->unk_5C = (arg0->unk_5C - arg0->unk_00.unk_3E) + 1;
            }
        }
    } else if (arg0->unk_56 & 0x200) {
        if (!(arg0->unk_54 & 0x200)) {
            if ((arg0->unk_5C - arg0->unk_60) < arg0->unk_68) {
                var_v1 = (arg0->unk_5C - arg0->unk_60);
            } else {
                var_v1 = arg0->unk_68;
            }
            arg0->unk_5C -= var_v1;

            if (arg0->unk_58 < var_v1) {
                var_a2 = arg0->unk_58;
            } else {
                var_a2 = var_v1;
            }
            arg0->unk_58 -= var_a2;

            arg0->unk_00.unk_38 = arg0->unk_5C / arg0->unk_00.unk_3E;
            if (arg0->unk_60 == arg0->unk_5C) {
                arg0->unk_56 &= ~0x200;
            }
        }
    } else if ((arg0->unk_56 & 0x100) && !(arg0->unk_54 & 0x100)) {
        s32 tmp1 = arg0->unk_00.unk_3E * (arg0->unk_00.unk_2C->unk_08 + 1);
        s32 tmp2 = arg0->unk_00.unk_3E * arg0->unk_00.unk_4A;

        if ((arg0->unk_60 - arg0->unk_5C) < arg0->unk_68) {
            var_a3 = (arg0->unk_60 - arg0->unk_5C);
        } else {
            var_a3 = arg0->unk_68;
        }
        arg0->unk_5C += var_a3;

        var_v1 = (tmp1 - arg0->unk_58) - tmp2;

        if (var_v1 <= 0) {
            var_v1 = 0;
        }

        if (var_v1 < var_a3) {
            var_a2 = var_v1;
        } else {
            var_a2 = var_a3;
        }

        arg0->unk_58 += var_a2;
        arg0->unk_00.unk_38 = arg0->unk_5C / arg0->unk_00.unk_3E;
        if (arg0->unk_60 == arg0->unk_5C) {
            arg0->unk_56 &= ~0x100;
        }
    }

    arg0->unk_54 = 0;

    var_v1 = arg0->unk_5C - arg0->unk_58;
    if (arg0->unk_56 & 0x400) {
        var_v1 = (var_v1 - arg0->unk_00.unk_3E) + 1;
    }

    arg0->unk_00.unk_44->unk_00.unk_10.unk_00 = arg0->unk_00.unk_50;
    arg0->unk_00.unk_44->unk_00.unk_10.unk_02 = arg0->unk_00.unk_52 + var_v1;
    return 0;
}

s32 LabPC_ScrollableGrid_DrawCells(unk_func_8820BE14_02C_038* arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 i;
    s32 temp_lo;
    s32 temp_s5;
    s32 var_s2;
    s32 var_s4;
    s32 var_s6;
    s32 var_v0;
    s32 var_v1_2;
    u16 var_v1;
    unk_func_88205880_00D0** var_s3;
    unk_func_88205880_00D0* var_a2;

    temp_lo = arg0->unk_00.unk_3E * arg0->unk_00.unk_4A;
    var_s3 = NULL;
    var_s6 = 0;
    var_s4 = 0;

    if (arg0->unk_00.unk_2C != NULL) {
        var_s3 = arg0->unk_00.unk_2C->unk_00;
        if (arg0->unk_00.unk_30 != 0) {
            var_s6 = arg0->unk_00.unk_2C->unk_0C;
        }
        var_s4 = arg0->unk_00.unk_2C->unk_08 + 1;
    }

    gDPPipeSync(gDisplayListHead++);

    gDPSetScissorFrac(gDisplayListHead++, G_SC_NON_INTERLACE, arg1 * 4.0f, arg2 * 4.0f,
                      (arg0->unk_00.unk_3C + arg1) * 4.0f, (arg2 + temp_lo) * 4.0f);

    var_s2 = arg2 - (arg0->unk_58 % arg0->unk_00.unk_3E);
    i = arg0->unk_58 / arg0->unk_00.unk_3E;
    temp_s5 = (((arg0->unk_58 + temp_lo) - 1) / arg0->unk_00.unk_3E) + 1;

    for (; i < temp_s5; i++) {
        var_a2 = NULL;
        var_v1 = 0;

        if ((i < var_s4) && (i != arg0->unk_00.unk_38)) {
            if (i < arg0->unk_00.unk_38) {
                var_v1_2 = 0;
            } else {
                var_v1_2 = -1;
            }

            var_v0 = var_v1_2 + i;
            if (arg0->unk_00.unk_30 != 0) {
                var_a2 = (u8*)var_s3 + var_s6 * var_v0;
            } else {
                var_a2 = var_s3[var_v0];
            }

            var_v1 = arg0->unk_00.unk_34[var_v0];
        } else if (i == arg0->unk_00.unk_38) {
            var_v1 = 1;
        }

        arg0->unk_00.unk_40(arg1, var_s2, var_a2, i, var_v1, arg0);

        var_s2 += arg0->unk_00.unk_3E;
    }

    gDPPipeSync(gDisplayListHead++);
    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, 640, 480);

    return 0;
}

s32 func_88216000(unk_func_8820BE14_02C_038* arg0, s32 arg1, s32 arg2) {
    s32 sp12C;
    s32 height = 16;
    s32 width = 8;
    s32 temp_s0;

    sp12C = ((unk_func_8820BE14_02C_038_02C*)arg0->unk_00.unk_2C)->unk_3E *
            ((unk_func_8820BE14_02C_038_02C*)arg0->unk_00.unk_2C)->unk_4A;

    temp_s0 = ((arg0->unk_00.unk_00.unk_14.unk_00 / 2) + arg1) - 9;

    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_POINT);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGBA, G_CC_DECALRGBA);

    gDPLoadTextureBlock(gDisplayListHead++, D_40022E0, G_IM_FMT_I, G_IM_SIZ_8b, width, height, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    gSPTextureRectangle(gDisplayListHead++, arg1 << 2, (arg2 * 4) & 0xFFF,
                        (arg0->unk_00.unk_00.unk_14.unk_00 + arg1) << 2, (arg2 + 0x10) << 2, G_TX_RENDERTILE, 0, 0,
                        0x0400, 0x0400);

    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_COPY);
    gDPSetRenderMode(gDisplayListHead++, G_RM_NOOP, G_RM_NOOP2);
    gDPSetAlphaCompare(gDisplayListHead++, G_AC_THRESHOLD);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);

    if (arg0->unk_00.unk_30 == 0) {
        gDPLoadTextureBlock(gDisplayListHead++,
                            (((unk_func_8820BE14_02C_038_02C*)(arg0->unk_00.unk_2C))->unk_58 > 0) ? D_4002368
                                                                                                  : D_4002D88,
                            G_IM_FMT_RGBA, G_IM_SIZ_16b, 20, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, temp_s0 << 2, (arg2 * 4) & 0xFFF, (temp_s0 + 0x11) << 2,
                            (arg2 + 0xF) << 2, G_TX_RENDERTILE, 0, 0, 0x1000, 0x0400);

        gDPLoadTextureBlock(gDisplayListHead++,
                            (((unk_func_8820BE14_02C_038_02C*)(arg0->unk_00.unk_2C))->unk_58 > 0) ? D_4002878
                                                                                                  : D_4003298,
                            G_IM_FMT_RGBA, G_IM_SIZ_16b, 20, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, (temp_s0 - 0x1C) << 2, (arg2 * 4) & 0xFFF, (temp_s0 - 9) << 2,
                            (arg2 + 0xF) << 2, G_TX_RENDERTILE, 0, 0, 0x1000, 0x0400);
    } else {
        s32 var_t5 = 0;
        if ((((unk_func_8820BE14_02C_038_02C*)(arg0->unk_00.unk_2C))->unk_2C != NULL) &&
            ((((unk_func_8820BE14_02C_038_02C*)(arg0->unk_00.unk_2C))->unk_58 + sp12C) <
             (((unk_func_8820BE14_02C_038_02C*)(arg0->unk_00.unk_2C))->unk_3E *
              (((unk_func_8820BE14_02C_038_02C*)(arg0->unk_00.unk_2C))->unk_2C->unk_08 + 1)))) {
            var_t5 = 1;
        }

        gDPLoadTextureBlock(gDisplayListHead++, var_t5 != 0 ? D_40025F0 : D_4003010, G_IM_FMT_RGBA, G_IM_SIZ_16b, 20,
                            16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, temp_s0 << 2, (arg2 * 4) & 0xFFF, (temp_s0 + 0x11) << 2,
                            (arg2 + 0xF) << 2, G_TX_RENDERTILE, 0, 0, 0x1000, 0x0400);

        gDPLoadTextureBlock(gDisplayListHead++, var_t5 != 0 ? D_4002B00 : D_4003520, G_IM_FMT_RGBA, G_IM_SIZ_16b, 20,
                            16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, (temp_s0 + 0x1A) << 2, (arg2 * 4) & 0xFFF, (temp_s0 + 0x2D) << 2,
                            (arg2 + 0xF) << 2, G_TX_RENDERTILE, 0, 0, 0x1000, 0x0400);
    }

    gDPPipeSync(gDisplayListHead++);

    gDPSetAlphaCompare(gDisplayListHead++, G_AC_NONE);
    return 0;
}

s32 func_882169D4(unk_func_8820BE14_02C_038* arg0, Controller* arg1) {
    s32 sp24 = arg0->unk_00.unk_38;
    s32 temp_t3 = arg0->unk_60;
    s32 var_t2 = 0;
    s32 sp18 = 0;
    s32 var_a1;

    if ((arg0->unk_00.unk_2C == NULL) || (arg0->unk_00.unk_2C->unk_08 == 0)) {
        return 0;
    }

    // clang-format off
    if (arg1->buttonPressed & 0x800) {
        arg0->unk_6C = 8; arg0->unk_54 = 0x800;
    } else if (arg1->buttonPressed & 0x400) {
        arg0->unk_6C = 8; arg0->unk_54 = 0x400;
    } else if (arg1->buttonPressed & 0x200) {
        arg0->unk_6C = 8; arg0->unk_54 = 0x200;
    } else if (arg1->buttonPressed & 0x100) {
        arg0->unk_6C = 8; arg0->unk_54 = 0x100;
    } else if (arg0->unk_6C > 0) {
        arg0->unk_6C--;
    } else {
        arg0->unk_54 = arg1->buttonDown;
    }
    // clang-format on

    if ((arg0->unk_54 & 0x800) && ((arg0->unk_56 & ~0x800) == 0)) {
        if ((arg0->unk_56 == 0) && (arg0->unk_5C == 0)) {
            sp18 = 1;
        }

        arg0->unk_56 |= 0x800;

        if (arg0->unk_5C < arg0->unk_64) {
            var_a1 = arg0->unk_5C;
        } else {
            var_a1 = arg0->unk_64;
        }

        arg0->unk_5C = arg0->unk_5C - var_a1;
        ((func88506FC0)Memmap_GetFragmentVaddr(WidgetTree_ClampScrollableGridTop))(arg0);
        var_t2 = 0;
        arg0->unk_00.unk_38 = arg0->unk_5C / arg0->unk_00.unk_3E;
        if (sp24 != arg0->unk_00.unk_38) {
            var_t2 = 0x80000000;
        }
    } else if ((arg0->unk_54 & 0x400) && ((arg0->unk_56 & ~0x400) == 0)) {
        s32 temp_lo_2 = arg0->unk_00.unk_3E * (arg0->unk_00.unk_2C->unk_08 + 1);
        s32 temp_lo_6;

        if (arg0->unk_56 == 0) {
            arg0->unk_5C = (arg0->unk_5C + arg0->unk_00.unk_3E) - 1;
            if (temp_lo_2 == (arg0->unk_5C + 1)) {
                sp18 = 1;
            }
        }

        arg0->unk_56 |= 0x400;

        temp_lo_6 = (temp_lo_2 - arg0->unk_5C) - 1;
        if (temp_lo_6 < arg0->unk_64) {
            var_a1 = temp_lo_6;
        } else {
            var_a1 = arg0->unk_64;
        }
        arg0->unk_5C += var_a1;

        ((func88506FE0)Memmap_GetFragmentVaddr(WidgetTree_ClampScrollableGridBottom))(arg0);

        var_t2 = 0;
        arg0->unk_00.unk_38 = arg0->unk_5C / arg0->unk_00.unk_3E;
        if (sp24 != arg0->unk_00.unk_38) {
            var_t2 = 0x80000000;
        }
    } else if ((arg0->unk_54 & 0x200) && ((arg0->unk_56 & ~0x200) == 0)) {
        s32 temp_a3 = arg0->unk_00.unk_3E * arg0->unk_00.unk_4A;

        if (arg0->unk_56 == 0) {
            arg0->unk_60 = arg0->unk_5C;
            if (arg0->unk_60 > 0) {
                var_t2 = 0x80000000;
            }
        }

        arg0->unk_56 |= 0x200;

        if (arg0->unk_5C < arg0->unk_68) {
            var_a1 = arg0->unk_5C;
        } else {
            var_a1 = arg0->unk_68;
        }

        arg0->unk_5C -= var_a1;

        if (arg0->unk_5C < arg0->unk_60) {
            if (temp_a3 < arg0->unk_60) {
                var_a1 = temp_a3;
            } else {
                var_a1 = arg0->unk_60;
            }
            arg0->unk_60 -= var_a1;
        }

        arg0->unk_00.unk_38 = arg0->unk_5C / arg0->unk_00.unk_3E;

        if (arg0->unk_58 < arg0->unk_68) {
            var_a1 = arg0->unk_58;
        } else {
            var_a1 = arg0->unk_68;
        }

        arg0->unk_58 -= var_a1;

        if (temp_t3 != arg0->unk_60) {
            var_t2 |= 0x80000000;
        }
    } else if (arg0->unk_54 & 0x100) {
        if (!(arg0->unk_56 & ~0x100)) {
            s32 temp_lo_2 = arg0->unk_00.unk_3E * (arg0->unk_00.unk_2C->unk_08 + 1);
            s32 temp_lo_6 = arg0->unk_00.unk_3E * arg0->unk_00.unk_4A;
            s32 temp_a3 = (temp_lo_2 - arg0->unk_5C) - arg0->unk_00.unk_3E;

            if (arg0->unk_56 == 0) {
                arg0->unk_60 = arg0->unk_5C;
                if (temp_a3 > 0) {
                    var_t2 = 0x80000000;
                }
            }

            arg0->unk_56 |= 0x100;

            if (temp_a3 < arg0->unk_68) {
                var_a1 = temp_a3;
            } else {
                var_a1 = arg0->unk_68;
            }

            arg0->unk_5C += var_a1;

            if (arg0->unk_60 < arg0->unk_5C) {
                temp_a3 = (temp_lo_2 - arg0->unk_60) - arg0->unk_00.unk_3E;
                if (temp_lo_6 < temp_a3) {
                    var_a1 = temp_lo_6;
                } else {
                    var_a1 = temp_a3;
                }
                arg0->unk_60 += var_a1;
            }

            arg0->unk_00.unk_38 = arg0->unk_5C / arg0->unk_00.unk_3E;

            temp_a3 = (temp_lo_2 - arg0->unk_58) - temp_lo_6;
            if (temp_a3 <= 0) {
                temp_a3 = 0;
            }

            if (temp_a3 < arg0->unk_68) {
                var_a1 = temp_a3;
            } else {
                var_a1 = arg0->unk_68;
            }

            arg0->unk_58 += var_a1;

            if (temp_t3 != arg0->unk_60) {
                var_t2 |= 0x80000000;
            }
        }
    }

    if (arg0->unk_56 == 0) {
        var_t2 = 0;
    } else {
        var_t2 |= 8;
        if (sp18 == 0) {
            var_t2 |= 1;
        }
    }
    return var_t2;
}

void LabPC_ScrollableGrid_SetSwapMode(unk_func_8820BE14_02C_038* arg0, s32 arg1) {
    if (arg1 == 0) {
        arg0->unk_70 = 1;
        arg0->unk_00.unk_00.unk_1C = LabPC_ScrollableGrid_UpdateScroll;
        arg0->unk_00.unk_00.unk_18 = LabPC_ScrollableGrid_DrawCells;
        arg0->unk_00.unk_00.unk_20 = func_882169D4;
        arg0->unk_84->unk_00.unk_18 = func_88216000;
        if (arg0->unk_00.unk_2C->unk_08 > 0) {
            ((func88507AE4)Memmap_GetFragmentVaddr(WidgetTree_SetScrollableGridIndex))(arg0, arg0->unk_00.unk_38 + 1);
        }
    } else {
        arg0->unk_70 = 0;
        arg0->unk_00.unk_00.unk_1C = arg0->unk_74;
        arg0->unk_00.unk_00.unk_18 = arg0->unk_78;
        arg0->unk_00.unk_00.unk_20 = arg0->unk_7C;
        arg0->unk_84->unk_00.unk_18 = arg0->unk_80;
        if ((arg1 == 1) && (arg0->unk_00.unk_38 > 0) && (arg0->unk_00.unk_2C->unk_08 > 0)) {
            ((func88507B80)Memmap_GetFragmentVaddr(WidgetTree_SetScrollableGridIndexPreserveScroll))(
                arg0, arg0->unk_00.unk_38 - 1);
        }
    }
}

void LabPC_BindGridDeckData(unk_func_8820BE14_02C_038* arg0, unk_func_8820BE14_06C_000* arg1, u32 arg2) {
    arg0->unk_88 = &arg1->unk_00;
    arg0->unk_8C = arg2;
    ((func88506EE4)Memmap_GetFragmentVaddr(WidgetTree_BindScrollableGridData))(arg0, &arg1->unk_04);
}

void LabPC_BindGridStridedData(unk_func_8820BE14_02C_038* arg0, s32* arg1, s32 arg2) {
    arg0->unk_88 = arg1;
    arg0->unk_8C = arg2;
    ((func88506F50)Memmap_GetFragmentVaddr(WidgetTree_BindScrollableGridStridedData))(arg0, arg1 + 1);
}

void LabPC_InitPageCounterWidget(unk_func_882170A8* arg0, s32 arg1, s32 arg2, unk_func_8820BE14_02C_038* arg3,
                                 s32 arg4) {
    ((func885007CC)Memmap_GetFragmentVaddr(WidgetTree_InitWidget))(arg0, sizeof(unk_func_882170A8));

    arg0->unk_00.unk_18 = LabPC_PageCounterWidget_Draw;
    arg0->unk_00.unk_10.unk_00 = arg1;
    arg0->unk_00.unk_10.unk_02 = arg2;
    arg0->unk_2C = arg3;
    arg0->unk_30 = arg4;
}

void LabPC_DrawPageCounterDigits(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    Gfx_DrawNumberDigits(arg0, arg1, arg2, arg4);
    Gfx_DrawNumberDigits(((arg4 * 9) + arg0) + 9, arg1, arg3, arg4);

    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_POINT);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGBA, G_CC_DECALRGBA);

    gDPLoadTextureBlock(gDisplayListHead++, D_4006A60, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 10, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, ((arg4 * 9) + arg0) << 2, (arg1 + 2) << 2, (((arg4 * 9) + arg0) + 8) << 2,
                        (arg1 + 0xC) << 2, G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);
}

s32 LabPC_PageCounterWidget_Draw(unk_func_882170A8* arg0, s32 arg1, s32 arg2) {
    unk_func_8820BE14_06C* temp_v0 = arg0->unk_2C->unk_00.unk_2C;

    if (temp_v0 == NULL) {
        return 0;
    }

    LabPC_DrawPageCounterDigits(arg1, arg2, temp_v0->unk_08, temp_v0->unk_04, arg0->unk_30);
    return 0;
}

void LabPC_InitFlyingIconWidget(unk_func_882173EC* arg0, s32 arg1, s32 arg2) {
    ((func885007CC)Memmap_GetFragmentVaddr(WidgetTree_InitWidget))(arg0, sizeof(unk_func_882173EC));

    arg0->unk_00.unk_1C = LabPC_FlyingIconWidget_Update;
    arg0->unk_30.unk_00 = arg1;
    arg0->unk_30.unk_02 = arg2;
    arg0->unk_2C = 0x10;
    arg0->unk_00.unk_28 &= ~1;
}

typedef struct unk_func_88217460 {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
} unk_func_88217460; // size >= 0x8

s32 LabPC_FlyingIconWidget_Update(unk_func_882173EC* arg0) {
    f32 temp_fa1;
    f32 temp_ft4;
    f32 tmp = 15.0f;
    f32 sp38;
    f32 sp34;
    f32 sp30;
    f32 sp28[2];
    f32 sp24;
    f32 sp1C;

    sp1C = (tmp * 2.0f) / 7.5f;
    sp38 = -sp1C / 7.5f;

    if (arg0->unk_2C >= 0x10) {
        arg0->unk_00.unk_28 &= ~1;
        return 0;
    }

    if (arg0->unk_2C >= 0xF) {
        arg0->unk_00.unk_28 &= ~0x100;
    }

    sp30 = arg0->unk_34.unk_00 - arg0->unk_30.unk_00;
    sp34 = arg0->unk_34.unk_02 - arg0->unk_30.unk_02;

    sp28[0] = sp34;
    sp28[1] = -sp30;

    Vec2f_Normalize(sp28);

    sp24 = (arg0->unk_2C * sp38) + sp1C;

    temp_fa1 = (sp1C + sp24) * arg0->unk_2C * 0.5f;

    sp28[0] *= temp_fa1;
    sp28[1] *= temp_fa1;

    temp_ft4 = arg0->unk_2C / tmp;

    sp30 *= temp_ft4;
    sp34 *= temp_ft4;

    sp30 += sp28[0];
    sp34 += sp28[1];

    arg0->unk_00.unk_10.unk_00 = arg0->unk_30.unk_00 + (s16)sp30;
    arg0->unk_00.unk_10.unk_02 = arg0->unk_30.unk_02 + (s16)sp34;

    arg0->unk_2C++;
    return 0;
}

void LabPC_FlyingIconWidget_Start(unk_func_882173EC* arg0, WidgetPoint arg1, WidgetPoint arg2) {
    WidgetPoint tmp = arg1;

    arg0->unk_30 = tmp;
    arg0->unk_00.unk_10 = (0, tmp);
    arg0->unk_34 = arg2;
    arg0->unk_2C = 0;
    arg0->unk_00.unk_28 |= 0x101;
}

void LabPC_InitSlideIconWidget(unk_func_8821421C_02C_06C_02C_068* arg0, s32 arg1, s32 arg2) {
    ((func885007CC)Memmap_GetFragmentVaddr(WidgetTree_InitWidget))(arg0, sizeof(unk_func_8821421C_02C_06C_02C_068));

    arg0->unk_00.unk_1C = LabPC_SlideIconWidget_Update;
    arg0->unk_00.unk_10.unk_00 = arg1;
    arg0->unk_00.unk_10.unk_02 = arg2;
    arg0->unk_2C = 0x10;
    arg0->unk_00.unk_28 &= ~1;
}

s32 LabPC_SlideIconWidget_Update(unk_func_8821421C_02C_06C_02C_068* arg0) {
    if (arg0->unk_2C >= 0x10) {
        arg0->unk_00.unk_28 &= ~1;
        return 0;
    }

    if (arg0->unk_2C >= 0xF) {
        arg0->unk_00.unk_28 &= ~0x100;
    }

    arg0->unk_2C++;
    return 0;
}

void LabPC_SlideIconWidget_Start(unk_func_8821421C_02C_06C_02C_068* arg0, WidgetPoint arg1) {
    arg0->unk_00.unk_10 = arg1;
    arg0->unk_00.unk_28 |= 0x101;
    arg0->unk_2C = 0;
}

void LabPC_InitModeToggleIcon(unk_func_88217740* arg0, s32 arg1, s32 arg2) {
    ((func885007CC)Memmap_GetFragmentVaddr(WidgetTree_InitWidget))(arg0, sizeof(unk_func_88217740));

    arg0->unk_00.unk_18 = LabPC_ModeToggleIcon_Draw;
    arg0->unk_00.unk_10.unk_00 = arg1;
    arg0->unk_00.unk_10.unk_02 = arg2;

    LabPC_ModeToggleIcon_SetMode(arg0, 1);
}

s32 LabPC_ModeToggleIcon_Draw(unk_func_88217740* arg0, s32 arg1, s32 arg2) {
    Color_RGBA8 sp6C;

    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_POINT);
    gDPSetCombineMode(gDisplayListHead++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
    gDPLoadTextureBlock(gDisplayListHead++, D_8821EE88, G_IM_FMT_IA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    sp6C = D_88218238[(arg0->unk_2C != 0) ? 0 : 1];

    gDPSetPrimColor(gDisplayListHead++, 0, 0, sp6C.r, sp6C.g, sp6C.b, sp6C.a);
    gSPTextureRectangle(gDisplayListHead++, arg1 << 2, arg2 << 2, (arg1 + 0x20) << 2, (arg2 + 0x14) << 2,
                        G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

    sp6C = D_88218238[(arg0->unk_2C == 0) ? 0 : 1];

    gDPPipeSync(gDisplayListHead++);

    gDPSetPrimColor(gDisplayListHead++, 0, 0, sp6C.r, sp6C.g, sp6C.b, sp6C.a);
    gSPTextureRectangle(gDisplayListHead++, (arg1 + 0x20) << 2, arg2 << 2, (arg1 + 0x40) << 2, (arg2 + 0x14) << 2,
                        G_TX_RENDERTILE, 0x0400, 0, 0x0400, 0x0400);

    return 0;
}

void LabPC_ModeToggleIcon_SetMode(unk_func_88217740* arg0, s32 arg1) {
    arg0->unk_30 = 0;
    arg0->unk_2C = arg1;
}

void LabPC_ModeToggleIcon_SetPressed(unk_func_88217740* arg0) {
    arg0->unk_30 = 1;
}

void LabPC_ModeToggleIcon_ClearPressed(unk_func_88217740* arg0) {
    arg0->unk_30 = 0;
}

void LabPC_ModeToggleIcon_ToggleMode(unk_func_88217740* arg0) {
    LabPC_ModeToggleIcon_SetMode(arg0, arg0->unk_2C == 0);
}
