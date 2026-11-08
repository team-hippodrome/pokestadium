#include "demo_scenes.h"
#include "src/math_util.h"

/**
 * Returns a random value in [0, arg0].
 */
u16 MathUtil_RandomU16Range(s32 arg0) {
    return ((f32)(u32)MathUtil_Random16() / 65535.0) * arg0;
}

/**
 * Copies arg1 into arg0 and returns arg0.
 */
Vec3f* Vec3f_Copy(Vec3f* arg0, Vec3f* arg1) {
    arg0->x = arg1->x;
    arg0->y = arg1->y;
    arg0->z = arg1->z;
    return arg0;
}

/**
 * Copies arg1 into arg0 and returns arg0.
 */
Vec3s* Vec3s_Copy(Vec3s* arg0, Vec3s* arg1) {
    arg0->x = arg1->x;
    arg0->y = arg1->y;
    arg0->z = arg1->z;
    return arg0;
}

/**
 * Sets every component of arg0 to 0.
 */
void Vec3f_Zero(Vec3f* arg0) {
    arg0->x = arg0->y = arg0->z = 0.0f;
}

/**
 * Moves *arg0 the fraction arg2 of the way to arg1, snapping results within 0.001 of zero to 0.
 */
void Math_EaseTowardF(f32* arg0, f32 arg1, f32 arg2) {
    *arg0 = *arg0 + ((arg1 - *arg0) * arg2);

    if ((*arg0 < 0.001) && (*arg0 > -0.001)) {
        *arg0 = 0.0f;
    }
}

/**
 * Returns the distance between two points.
 */
f32 Vec3f_Distance(Vec3f arg0, Vec3f arg1) {
    return sqrtf(SQ(arg1.x - arg0.x) + SQ(arg1.y - arg0.y) + SQ(arg1.z - arg0.z));
}

/**
 * Writes the transposed 3x3 rotation part of arg1 into arg0, multiplying each row by the length
 * of the matching source row, and clears the translation.
 */
void MtxF_TransposeScaled(MtxF* arg0, MtxF* arg1) {
    f32 sp24 = sqrtf(SQ(arg1->mf[0][0]) + SQ(arg1->mf[0][1]) + SQ(arg1->mf[0][2]));
    f32 sp20 = sqrtf(SQ(arg1->mf[1][0]) + SQ(arg1->mf[1][1]) + SQ(arg1->mf[1][2]));
    f32 sp1C = sqrtf(SQ(arg1->mf[2][0]) + SQ(arg1->mf[2][1]) + SQ(arg1->mf[2][2]));

    arg0->mf[0][0] = arg1->mf[0][0] * sp24;
    arg0->mf[0][1] = arg1->mf[1][0] * sp24;
    arg0->mf[0][2] = arg1->mf[2][0] * sp24;
    arg0->mf[0][3] = 0.0f;

    arg0->mf[1][0] = arg1->mf[0][1] * sp20;
    arg0->mf[1][1] = arg1->mf[1][1] * sp20;
    arg0->mf[1][2] = arg1->mf[2][1] * sp20;
    arg0->mf[1][3] = 0.0f;

    arg0->mf[2][0] = arg1->mf[0][2] * sp1C;
    arg0->mf[2][1] = arg1->mf[1][2] * sp1C;
    arg0->mf[2][2] = arg1->mf[2][2] * sp1C;
    arg0->mf[2][3] = 0.0f;

    arg0->mf[3][0] = 0.0f;
    arg0->mf[3][1] = 0.0f;
    arg0->mf[3][2] = 0.0f;
    arg0->mf[3][3] = 1.0f;
}
