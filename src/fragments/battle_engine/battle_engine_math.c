#include "battle_engine.h"
#include "src/math_util.h"

/**
 * Converts the vector from `from` to `to` into spherical form.
 *
 * Writes its length to *euclidean_distance, its angle above the XZ plane to
 * *elevation (0 = horizontal, 0x4000 = straight up), and its angle in the XZ
 * plane to *azimuth, measured from +Z toward +X so that
 * BattleAnim_Vec3fOffsetByPolarXZ reproduces the horizontal direction.
 * MathUtil_Atan2s(y, x) returns the angle of (x, y) measured from the y axis,
 * hence the (horizontal, delta_y) and (delta_z, delta_x) argument orders.
 */
//@src/math_util.c
void BattleAnim_CalculateDistanceAngles(Vec3f* from, Vec3f* to, f32* euclidean_distance, s16* elevation, s16* azimuth) {
    f32 delta_x = to->x - from->x;
    f32 delta_y = to->y - from->y;
    f32 delta_z = to->z - from->z;

    *euclidean_distance = sqrtf(SQ(delta_x) + SQ(delta_y) + SQ(delta_z));
    *elevation = MathUtil_Atan2s(sqrtf(SQ(delta_x) + SQ(delta_z)), delta_y);
    *azimuth = MathUtil_Atan2s(delta_z, delta_x);
}

/**
 * Sets all three components of `dest` to 0.0f and returns `dest`.
 */
Vec3f* BattleAnim_Vec3fSetZero(Vec3f* dest) {
    dest->x = 0.0f;
    dest->y = 0.0f;
    dest->z = 0.0f;
    return dest;
}

/**
 * Sets `dest` to `origin` displaced by `radius` in the XZ plane at angle
 * `theta` (s16 angle units, measured from +Z toward +X: x += sin, z += cos).
 * The Y component is copied unchanged. Returns `dest`.
 */
Vec3f* BattleAnim_Vec3fOffsetByPolarXZ(Vec3f* dest, Vec3f* origin, f32 radius, s16 theta) {
    dest->x = (SINS(theta) * radius) + origin->x;
    dest->y = origin->y;
    dest->z = (COSS(theta) * radius) + origin->z;
    return dest;
}

/**
 * Writes the XZ components of `dest` from polar form: x = cos(theta) * magnitude,
 * z = sin(theta) * magnitude (theta measured from +X toward +Z, unlike
 * BattleAnim_Vec3fOffsetByPolarXZ, which measures from +Z toward +X). The Y
 * component is left untouched.
 */
void BattleAnim_Vec3fSetPolarXZ(Vec3f* dest, f32 magnitude, s16 theta) {
    dest->x = COSS(theta) * magnitude;
    dest->z = SINS(theta) * magnitude;
}

/**
 * Copies all three components of `src` into `dest` and returns `dest`.
 */
Vec3f* BattleAnim_Vec3fCopy(Vec3f* dest, Vec3f* src) {
    dest->x = src->x;
    dest->y = src->y;
    dest->z = src->z;
    return dest;
}

/**
 * Copies all three components of the s16 vector `src` into `dest` and returns
 * `dest`. Callers use it to copy display object rotations.
 */
Vec3s* BattleAnim_Vec3sCopy(Vec3s* dest, Vec3s* src) {
    dest->x = src->x;
    dest->y = src->y;
    dest->z = src->z;
    return dest;
}

/**
 * Multiplies `dest` in place by a separate factor per axis and returns `dest`.
 * No callers exist in src.
 */
Vec3f* BattleAnim_Vec3fScaleComponents(Vec3f* dest, f32 scale_x, f32 scale_y, f32 scale_z) {
    dest->x = dest->x * scale_x;
    dest->y = dest->y * scale_y;
    dest->z = dest->z * scale_z;
    return dest;
}

/**
 * Returns MathUtil_Atan2s of the offset from (x0, y0) to (x1, y1). Argument
 * order follows MathUtil_Atan2s(y, x). No callers exist in src.
 */
//@src/math_util.c
s16 BattleAnim_Atan2sFromPoints(f32 y0, f32 x0, f32 y1, f32 x1) {
    return MathUtil_Atan2s(y1 - y0, x1 - x0);
}

/**
 * Returns the Euclidean distance between points `a` and `b`, passed by value.
 */
f32 BattleAnim_Vec3fDistance(Vec3f a, Vec3f b) {
    return sqrtf(SQ(b.x - a.x) + SQ(b.y - a.y) + SQ(b.z - a.z));
}

/**
 * Returns the distance between (x0, y0) and (x1, y1) in a 2D plane. The camera
 * code passes X and Z world coordinates. The squared deltas are kept in
 * function-local statics (.bss words) instead of stack temporaries.
 */
f32 BattleAnim_Distance2D(f32 x0, f32 y0, f32 x1, f32 y1) {
    static f32 sDistance2DSquaredDeltaX;
    static f32 sDistance2DSquaredDeltaY;

    sDistance2DSquaredDeltaX = SQ(x0 - x1);
    sDistance2DSquaredDeltaY = SQ(y0 - y1);

    if (sDistance2DSquaredDeltaX + sDistance2DSquaredDeltaY == 0.0f) {
        return 0.0f;
    }

    return sqrtf(sDistance2DSquaredDeltaX + sDistance2DSquaredDeltaY);
}

/**
 * Returns a random integer in [0, count - 1]. Before drawing, it discards
 * (osGetCount() & 0xFF) values from MathUtil_Random_ZeroOne, so the result
 * also depends on the CPU cycle counter and is not reproducible from the RNG
 * state alone. The clamp keeps the result below `count` if the scaled draw
 * rounds up to it. Callers pass ARRAY_COUNT(table) or a float literal to
 * pick a random table entry.
 */
//@src/math_util.c
s32 BattleAnim_RandomIntWithWarmup(f32 count) {
    s32 warmup_count;
    s32 index;

    warmup_count = osGetCount() & 0xFF;
    while (warmup_count-- > 0) {
        MathUtil_Random_ZeroOne();
    }

    index = MathUtil_Random_ZeroOne() * count;
    if (count <= index) {
        index = (s32)count - 1;
    }
    return index;
}

/**
 * Returns a random value in [0, range]. MathUtil_Random16 / 65535 spans the
 * closed interval [0, 1], so `range` itself is returned when the draw is 0xFFFF.
 * Callers must handle that case: BattleScene_InitializeIntroCamera resets a
 * BattleAnim_RandomRange(5) result of 5 to 0.
 */
//@src/math_util.c
//@src/fragments/battle_engine/battle_engine_battle_phases.c
u16 BattleAnim_RandomRange(s32 range) {
    f32 random_value = (u32)MathUtil_Random16();

    return (random_value / 65535.0) * range;
}

/**
 * Moves *dest toward `finish` by at most |rate| and returns 0 if *dest now
 * equals `finish`, otherwise 1. The difference is computed in s16, so for
 * angle values it takes the shorter way around the 16-bit circle.
 */
s32 BattleAnim_StepToS16(s16* dest, s16 finish, s16 rate) {
    s16 delta_to_finish = finish - *dest;

    if (rate < 0) {
        rate *= -1;
    }

    if (delta_to_finish > 0) {
        delta_to_finish -= rate;
        if (delta_to_finish >= 0) {
            *dest = finish - delta_to_finish;
        } else {
            *dest = finish;
        }
    } else {
        delta_to_finish += rate;
        if (delta_to_finish <= 0) {
            *dest = finish - delta_to_finish;
        } else {
            *dest = finish;
        }
    }

    if (finish == *dest) {
        return 0;
    }

    return 1;
}

/**
 * Moves *dest toward `finish` by a step proportional to the remaining
 * distance: step = distance * |rate|, then limited to a magnitude of 40..1092
 * (about 0.2 to 6 degrees as s16 angle units). Returns 0 if *dest now equals
 * `finish`, otherwise 1. The camera code uses it to ease azimuth and elevation
 * angles, passing BattleScene's unk_94.z as `rate`.
 */
s32 BattleAnim_StepToS16VariableRate(s16* dest, s16 finish, f32 rate) {
    f32 step;
    s16 delta_to_finish;

    delta_to_finish = finish - *dest;
    if (rate < 0.0f) {
        rate = -1.0f * rate;
    }

    step = delta_to_finish * rate;

    if (delta_to_finish > 0) {
        if ((s16)step >= 1092) {
            step = 1092.0f;
        }

        if ((s16)step < 41) {
            step = 40.0f;
        }

        delta_to_finish -= (s16)step;
        if (delta_to_finish >= 0) {
            *dest = finish - delta_to_finish;
        } else {
            *dest = finish;
        }
    } else {
        if ((s16)step < -1091) {
            step = -1092.0f;
        }

        if ((s16)step >= -40) {
            step = -40.0f;
        }

        delta_to_finish -= (s16)step;
        if (delta_to_finish <= 0) {
            *dest = finish - delta_to_finish;
        } else {
            *dest = finish;
        }
    }

    if (finish == *dest) {
        return 0;
    }
    return 1;
}

/**
 * Exponential approach: moves *dest toward `value` by the fraction `speed` of
 * the remaining distance (speed 0.1f closes 10% per call). A result within
 * +/-0.001 of zero is snapped to exactly 0.0f, even when `value` is not 0.
 */
void BattleAnim_ApproachF(f32* dest, f32 value, f32 speed) {
    *dest = *dest + ((value - *dest) * speed);
    if ((*dest < 0.001) && (*dest > -0.001)) {
        *dest = 0.0f;
    }
}

/**
 * Integer variant of BattleAnim_ApproachF without the snap to zero. `speed` is
 * an s16 multiplier, so any value other than 0 or 1 overshoots `value`. No
 * callers exist in src.
 */
void BattleAnim_ApproachS16(s16* dest, s16 value, s16 speed) {
    *dest = *dest + ((value - *dest) * speed);
}

/**
 * u16 version of BattleAnim_ApproachS16; the same overshoot caveat applies.
 * No callers exist in src.
 */
void BattleAnim_ApproachU16(u16* dest, u16 value, s16 speed) {
    *dest = *dest + ((value - *dest) * speed);
}

/**
 * Writes the four uniform cubic B-spline basis weights for parameter `t` in
 * [0, 1] to dest[0..3]; they weight four consecutive control points and sum
 * to 1 (dest[0] = (1 - t)^3 / 6, dest[3] = t^3 / 6). Identified by comparing
 * the polynomials with the standard basis; no callers exist in src.
 * The powers of t are held in function-local statics.
 */
void BattleAnim_BSplineWeightsUniform(f32* dest, f32 t) {
    static f32 sUniformTCubed;
    static f32 sUniformTSquared;
    static f32 sUniformOneMinusT;

    sUniformOneMinusT = 1.0f - t;
    sUniformTSquared = SQ(t);
    sUniformTCubed = sUniformTSquared * t;

    dest[0] = (SQ(sUniformOneMinusT) * sUniformOneMinusT) / 6.0f;
    dest[1] = ((0.5f * sUniformTCubed) - sUniformTSquared) + 0.6666667f;
    dest[2] = (-0.5f * sUniformTCubed) + (0.5f * sUniformTSquared) + (0.5f * t) + 0.16666667f;
    dest[3] = sUniformTCubed / 6.0;
}

/**
 * Cubic B-spline basis weights for the first segment of a clamped uniform
 * spline (knots 0, 0, 0, 0, 1, 2, ...), so dest[0] = (1 - t)^3 and t = 0 selects
 * the first control point. Compared against a Cox-de Boor evaluation of that
 * knot vector; no callers exist in src.
 */
void BattleAnim_BSplineWeightsClampedFirst(f32* dest, f32 t) {
    static f32 sClampedFirstTCubed;
    static f32 sClampedFirstTSquared;
    static f32 sClampedFirstOneMinusT;

    sClampedFirstOneMinusT = 1.0 - t;
    sClampedFirstTSquared = SQ(t);
    sClampedFirstTCubed = sClampedFirstTSquared * t;

    dest[0] = SQ(sClampedFirstOneMinusT) * sClampedFirstOneMinusT;
    dest[1] = (((21.0f * sClampedFirstTCubed) / 12.0f) - (9.0f * sClampedFirstTSquared * 0.5f)) + (3.0f * t);
    dest[2] = ((-11.0f * sClampedFirstTCubed) / 12.0f) + (3.0f * sClampedFirstTSquared * 0.5f);
    dest[3] = sClampedFirstTCubed / 6.0f;
}

/**
 * Cubic B-spline basis weights for the second segment of the clamped uniform
 * spline described at BattleAnim_BSplineWeightsClampedFirst (dest[0] =
 * (1 - t)^3 / 4). No callers exist in src.
 */
void BattleAnim_BSplineWeightsClampedSecond(f32* dest, f32 t) {
    static f32 sClampedSecondTCubed;
    static f32 sClampedSecondTSquared;
    static f32 sClampedSecondOneMinusT;

    sClampedSecondOneMinusT = 1.0f - t;
    sClampedSecondTSquared = SQ(t);
    sClampedSecondTCubed = sClampedSecondTSquared * t;

    dest[0] = SQ(sClampedSecondOneMinusT) * sClampedSecondOneMinusT * 0.25f;
    dest[1] = ((0.5833333f * sClampedSecondTCubed) - (1.25f * sClampedSecondTSquared)) + (t * 0.25f) + 0.5833333f;
    dest[2] = (-0.5f * sClampedSecondTCubed) + (0.5f * sClampedSecondTSquared) + (0.5f * t) + 0.16666667f;
    dest[3] = sClampedSecondTCubed / 6.0f;
}

/**
 * Mirror image of BattleAnim_BSplineWeightsClampedSecond for the second-to-last
 * segment of the clamped uniform spline: the result equals that function's
 * output at (1 - t) with the order reversed (dest[3] = t^3 / 4). No callers
 * exist in src.
 */
void BattleAnim_BSplineWeightsClampedPenultimate(f32* dest, f32 t) {
    static f32 sClampedPenultimateTCubed;
    static f32 sClampedPenultimateOneMinusT;
    static f32 sClampedPenultimateOneMinusTSquared;
    static f32 sClampedPenultimateOneMinusTCubed;

    sClampedPenultimateOneMinusT = 1.0f - t;
    sClampedPenultimateOneMinusTSquared = SQ(sClampedPenultimateOneMinusT);
    sClampedPenultimateOneMinusTCubed = sClampedPenultimateOneMinusTSquared * sClampedPenultimateOneMinusT;
    sClampedPenultimateTCubed = CB(t);

    dest[0] = sClampedPenultimateOneMinusTCubed / 6.0f;
    dest[1] = (-0.5f * sClampedPenultimateOneMinusTCubed) + (0.5f * sClampedPenultimateOneMinusTSquared) + (0.5f * sClampedPenultimateOneMinusT) + 0.16666667f;
    dest[2] = (((7.0f * sClampedPenultimateOneMinusTCubed) / 12.0f) - (5.0f * sClampedPenultimateOneMinusTSquared * 0.25f)) + (sClampedPenultimateOneMinusT * 0.25f) + 0.5833333f;
    dest[3] = sClampedPenultimateTCubed * 0.25f;
}

/**
 * Mirror image of BattleAnim_BSplineWeightsClampedFirst for the last segment of
 * the clamped uniform spline: the result equals that function's output at
 * (1 - t) with the order reversed (dest[3] = t^3, so t = 1 selects the last
 * control point). No callers exist in src.
 */
void BattleAnim_BSplineWeightsClampedLast(f32* dest, f32 t) {
    static f32 sClampedLastTCubed;
    static f32 sClampedLastOneMinusT;
    static f32 sClampedLastOneMinusTSquared;
    static f32 sClampedLastOneMinusTCubed;

    sClampedLastOneMinusT = 1.0f - t;
    sClampedLastOneMinusTSquared = SQ(sClampedLastOneMinusT);
    sClampedLastOneMinusTCubed = sClampedLastOneMinusTSquared * sClampedLastOneMinusT;
    sClampedLastTCubed = CB(t);

    dest[0] = sClampedLastOneMinusTCubed / 6.0f;
    dest[1] = ((-11.0f * sClampedLastOneMinusTCubed) / 12.0f) + (3.0f * sClampedLastOneMinusTSquared * 0.5f);
    dest[2] = (((21.0f * sClampedLastOneMinusTCubed) / 12.0f) - (9.0f * sClampedLastOneMinusTSquared * 0.5f)) + (3.0f * sClampedLastOneMinusT);
    dest[3] = sClampedLastTCubed;
}
