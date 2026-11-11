#include "demo_scenes.h"

/**
 * Converts a float matrix to the RSP fixed-point Mtx layout: each element as s15.16, with integer halves
 * in the first eight words and fraction halves in the last eight (like guMtxF2L).
 */
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

/**
 * Multiplies the upper-left 3x3 (rotation/basis) part of the matrix by scale, leaving translation intact.
 */
void MtxF_ScaleUpperRows(MtxF* mtx, f32 scale) {
    mtx->mf[0][0] *= scale;
    mtx->mf[1][0] *= scale;
    mtx->mf[2][0] *= scale;

    mtx->mf[0][1] *= scale;
    mtx->mf[1][1] *= scale;
    mtx->mf[2][1] *= scale;

    mtx->mf[0][2] *= scale;
    mtx->mf[1][2] *= scale;
    mtx->mf[2][2] *= scale;
}

/**
 * Builds a transform from translation position and binary-angle rotations about X, Y and Z (row 3 holds
 * the translation).
 */
void Matrix_SetTransformFromPosRot(MtxF* mtx, Vec3f position, Vec3s rotation) {
    UNUSED s32 pad[2];
    f32 temp_fa0;
    f32 temp_fa1;
    f32 temp_ft4;
    f32 temp_ft5;
    f32 temp_fv0;
    f32 temp_fv1;
    f32 sp0;

    temp_fv0 = SINS(rotation.x);
    temp_fv1 = COSS(rotation.x);

    temp_fa0 = SINS(rotation.y);
    temp_fa1 = COSS(rotation.y);

    temp_ft4 = SINS(rotation.z);
    temp_ft5 = COSS(rotation.z);

    sp0 = temp_fv0 * temp_fa0;
    mtx->mf[0][0] = (temp_fa1 * temp_ft5) + (sp0 * temp_ft4);
    mtx->mf[1][0] = (-temp_fa1 * temp_ft4) + (sp0 * temp_ft5);
    mtx->mf[2][0] = temp_fv1 * temp_fa0;
    mtx->mf[3][0] = position.x;

    mtx->mf[0][1] = temp_fv1 * temp_ft4;
    mtx->mf[1][1] = temp_fv1 * temp_ft5;
    mtx->mf[2][1] = -temp_fv0;
    mtx->mf[3][1] = position.y;

    sp0 = temp_fv0 * temp_fa1;
    mtx->mf[0][2] = (-temp_fa0 * temp_ft5) + (sp0 * temp_ft4);
    mtx->mf[1][2] = (temp_fa0 * temp_ft4) + (sp0 * temp_ft5);
    mtx->mf[2][2] = temp_fv1 * temp_fa1;
    mtx->mf[3][2] = position.z;

    mtx->mf[0][3] = 0.0f;
    mtx->mf[1][3] = 0.0f;
    mtx->mf[2][3] = 0.0f;
    mtx->mf[3][3] = 1.0f;
}
