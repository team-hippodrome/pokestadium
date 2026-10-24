#include "demo_scenes.h"

void MtxF_PackToFixed(Mtx* arg0, MtxF* arg1) {
    arg0->m[0][0] =
        ((s32)(arg1->mf[0][0] * 65536.0f) & 0xFFFF0000) | (((s32)(arg1->mf[0][1] * 65536.0f) >> 0x10) & 0xFFFF);
    arg0->m[0][1] =
        ((s32)(arg1->mf[0][2] * 65536.0f) & 0xFFFF0000) | (((s32)(arg1->mf[0][3] * 65536.0f) >> 0x10) & 0xFFFF);
    arg0->m[0][2] =
        ((s32)(arg1->mf[1][0] * 65536.0f) & 0xFFFF0000) | (((s32)(arg1->mf[1][1] * 65536.0f) >> 0x10) & 0xFFFF);
    arg0->m[0][3] =
        ((s32)(arg1->mf[1][2] * 65536.0f) & 0xFFFF0000) | (((s32)(arg1->mf[1][3] * 65536.0f) >> 0x10) & 0xFFFF);
    arg0->m[1][0] =
        ((s32)(arg1->mf[2][0] * 65536.0f) & 0xFFFF0000) | (((s32)(arg1->mf[2][1] * 65536.0f) >> 0x10) & 0xFFFF);
    arg0->m[1][1] =
        ((s32)(arg1->mf[2][2] * 65536.0f) & 0xFFFF0000) | (((s32)(arg1->mf[2][3] * 65536.0f) >> 0x10) & 0xFFFF);
    arg0->m[1][2] =
        ((s32)(arg1->mf[3][0] * 65536.0f) & 0xFFFF0000) | (((s32)(arg1->mf[3][1] * 65536.0f) >> 0x10) & 0xFFFF);
    arg0->m[1][3] =
        ((s32)(arg1->mf[3][2] * 65536.0f) & 0xFFFF0000) | (((s32)(arg1->mf[3][3] * 65536.0f) >> 0x10) & 0xFFFF);
    arg0->m[2][0] = ((s32)(arg1->mf[0][0] * 65536.0f) << 0x10) | ((s32)(arg1->mf[0][1] * 65536.0f) & 0xFFFF);
    arg0->m[2][1] = ((s32)(arg1->mf[0][2] * 65536.0f) << 0x10) | ((s32)(arg1->mf[0][3] * 65536.0f) & 0xFFFF);
    arg0->m[2][2] = ((s32)(arg1->mf[1][0] * 65536.0f) << 0x10) | ((s32)(arg1->mf[1][1] * 65536.0f) & 0xFFFF);
    arg0->m[2][3] = ((s32)(arg1->mf[1][2] * 65536.0f) << 0x10) | ((s32)(arg1->mf[1][3] * 65536.0f) & 0xFFFF);
    arg0->m[3][0] = ((s32)(arg1->mf[2][0] * 65536.0f) << 0x10) | ((s32)(arg1->mf[2][1] * 65536.0f) & 0xFFFF);
    arg0->m[3][1] = ((s32)(arg1->mf[2][2] * 65536.0f) << 0x10) | ((s32)(arg1->mf[2][3] * 65536.0f) & 0xFFFF);
    arg0->m[3][2] = ((s32)(arg1->mf[3][0] * 65536.0f) << 0x10) | ((s32)(arg1->mf[3][1] * 65536.0f) & 0xFFFF);
    arg0->m[3][3] = ((s32)(arg1->mf[3][2] * 65536.0f) << 0x10) | ((s32)(arg1->mf[3][3] * 65536.0f) & 0xFFFF);
}

void MtxF_ScaleUpperRows(MtxF* arg0, f32 arg1) {
    arg0->mf[0][0] *= arg1;
    arg0->mf[1][0] *= arg1;
    arg0->mf[2][0] *= arg1;

    arg0->mf[0][1] *= arg1;
    arg0->mf[1][1] *= arg1;
    arg0->mf[2][1] *= arg1;

    arg0->mf[0][2] *= arg1;
    arg0->mf[1][2] *= arg1;
    arg0->mf[2][2] *= arg1;
}

void Matrix_SetTransformFromPosRot(MtxF* arg0, Vec3f arg1, Vec3s arg2) {
    UNUSED s32 pad[2];
    f32 temp_fa0;
    f32 temp_fa1;
    f32 temp_ft4;
    f32 temp_ft5;
    f32 temp_fv0;
    f32 temp_fv1;
    f32 sp0;

    temp_fv0 = SINS(arg2.x);
    temp_fv1 = COSS(arg2.x);

    temp_fa0 = SINS(arg2.y);
    temp_fa1 = COSS(arg2.y);

    temp_ft4 = SINS(arg2.z);
    temp_ft5 = COSS(arg2.z);

    sp0 = temp_fv0 * temp_fa0;
    arg0->mf[0][0] = (temp_fa1 * temp_ft5) + (sp0 * temp_ft4);
    arg0->mf[1][0] = (-temp_fa1 * temp_ft4) + (sp0 * temp_ft5);
    arg0->mf[2][0] = temp_fv1 * temp_fa0;
    arg0->mf[3][0] = arg1.x;

    arg0->mf[0][1] = temp_fv1 * temp_ft4;
    arg0->mf[1][1] = temp_fv1 * temp_ft5;
    arg0->mf[2][1] = -temp_fv0;
    arg0->mf[3][1] = arg1.y;

    sp0 = temp_fv0 * temp_fa1;
    arg0->mf[0][2] = (-temp_fa0 * temp_ft5) + (sp0 * temp_ft4);
    arg0->mf[1][2] = (temp_fa0 * temp_ft4) + (sp0 * temp_ft5);
    arg0->mf[2][2] = temp_fv1 * temp_fa1;
    arg0->mf[3][2] = arg1.z;

    arg0->mf[0][3] = 0.0f;
    arg0->mf[1][3] = 0.0f;
    arg0->mf[2][3] = 0.0f;
    arg0->mf[3][3] = 1.0f;
}
