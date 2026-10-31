#include "particle_math.h"
#include "include/math.h"
#include "src/matrix.h"
#include "src/math_util.h"

/*
 * Fragment-owned state is initialized by the battle/minigame particle drivers.
 * The matrix pool accepts at most 364 submitted matrices per index reset.
 * Keep the duplicate tentative index declaration: this pass is annotation only.
 */
ParticleMatrixPool* sParticleMatrixPool;
s32 sParticleMatrixPoolIndex;
s32 sParticleMatrixPoolIndex;
ParticleRenderContext sParticleRenderContext;
ParticleCameraState* sParticleCameraState;
u32 sParticleFrameCounter;
s32 sParticleFrameMod64;
s32 sParticleFrameMod32;
s32 sParticleFrameMod16;
s32 sParticleFrameMod8;
s32 sParticleFrameMod4;
u32 sParticleFrameMod3;
s32 sParticleFrameMod2;
u32 sParticleFrameDiv64;
u32 sParticleFrameDiv32;
u32 sParticleFrameDiv16;
u32 sParticleFrameDiv8;
u32 sParticleFrameDiv4;
u32 sParticleFrameDiv3;
u32 sParticleFrameDiv2;
s32 sParticleFrameDiv64Parity;
s32 sParticleFrameDiv32Parity;
s32 sParticleFrameDiv16Parity;
s32 sParticleFrameDiv8Parity;
s32 sParticleFrameDiv4Parity;
s32 sParticleFrameDiv3Parity;
s32 sParticleFrameDiv2Parity;
s32 sParticleRandomOffset;
static s32 pad_D_8140E714[2];
Particle* sParticlePool;
BattleAnimEffectSlot* gBattleAnimEffectSlots;
ParticleEmitter* gMiniFxEmitters;
s32 sParticlePoolIndex;
s32 gBattleAnimEffectSlotIndex;
s32 gMiniFxEmitterIndex;

/* Set every bit selected by mask. */
void ParticleMath_BitwiseOr(s32* value, s32 mask) {
    *value |= mask;
}

/* Clear every bit selected by mask. */
void ParticleMath_BitwiseAndNot(s32* value, s32 mask) {
    *value &= ~mask;
}

/* Toggle every bit selected by mask. */
void ParticleMath_BitwiseXor(s32* value, s32 mask) {
    *value ^= mask;
}

/* Return whether any bit selected by mask is set. */
s32 ParticleMath_HasBits(s32* value, s32 mask) {
    s32 var_v1 = 0;

    if (*value & mask) {
        var_v1 = 1;
    }
    return var_v1;
}

/* Return whether all bits selected by mask are clear. */
s32 ParticleMath_LacksBits(s32* value, s32 mask) {
    s32 var_v1 = 0;

    if (!(*value & mask)) {
        var_v1 = 1;
    }
    return var_v1;
}

/* Increase value toward target by step; return 1 only when this call reaches or crosses the target. */
s32 ParticleMath_ApproachFloatUp(f32* value, f32 target, f32 step) {
    s32 var_v1 = 0;

    if (*value < target) {
        *value += step;
        if (target <= *value) {
            *value = target;
            var_v1 = 1;
        }
    }
    return var_v1;
}

/* Decrease value toward target by step; return 1 only when this call reaches or crosses the target. */
s32 ParticleMath_ApproachFloatDown(f32* value, f32 target, f32 step) {
    s32 var_v1 = 0;

    if (target < *value) {
        *value -= step;
        if (*value <= target) {
            *value = target;
            var_v1 = 1;
        }
    }
    return var_v1;
}

/* Increase value toward target by step; return 1 only when this call reaches or crosses the target. */
s32 ParticleMath_ApproachS32Up(s32* value, s32 target, s32 step) {
    s32 var_v1 = 0;

    if (*value < target) {
        *value += step;
        if (*value >= target) {
            *value = target;
            var_v1 = 1;
        }
    }
    return var_v1;
}

/* Decrease value toward target by step; return 1 only when this call reaches or crosses the target. */
s32 ParticleMath_ApproachS32Down(s32* value, s32 target, s32 step) {
    s32 var_v1 = 0;

    if (target < *value) {
        *value -= step;
        if (target >= *value) {
            *value = target;
            var_v1 = 1;
        }
    }
    return var_v1;
}

/* Increase value toward target by step; return 1 only when this call reaches or crosses the target. */
s32 ParticleMath_ApproachS16Up(s16* value, s16 target, s16 step) {
    s32 var_v1 = 0;

    if (*value < target) {
        *value += step;
        if (*value >= target) {
            *value = target;
            var_v1 = 1;
        }
    }
    return var_v1;
}

/* Increase value toward target by step; return 1 only when this call reaches or crosses the target. */
s32 ParticleMath_ApproachU16Up(u16* value, u16 target, u16 step) {
    s32 var_v1 = 0;

    if (*value < target) {
        *value += step;
        if (*value >= target) {
            *value = target;
            var_v1 = 1;
        }
    }
    return var_v1;
}

/* Decrease value toward target by step; return 1 only when this call reaches or crosses the target. */
s32 ParticleMath_ApproachS16Down(s16* value, s16 target, s16 step) {
    s32 var_v1 = 0;

    if (target < *value) {
        *value -= step;
        if (target >= *value) {
            *value = target;
            var_v1 = 1;
        }
    }
    return var_v1;
}

/* Decrease value toward target by step; return 1 only when this call reaches or crosses the target. */
s32 ParticleMath_ApproachU16Down(u16* value, s32 target, s32 step) {
    s32 var_v1 = 0;
    s32 temp_v0 = *value;

    if (target < temp_v0) {
        temp_v0 -= step;
        if (target >= temp_v0) {
            temp_v0 = target;
            var_v1 = 1;
        }
        *value = temp_v0;
    }
    return var_v1;
}

/* Increase value toward target by the pointed-to step; return 1 only when this call reaches or crosses the target. */
s32 ParticleMath_ApproachFloatUpByPointer(f32* value, f32 target, f32* step) {
    s32 var_v1 = 0;

    if (*value < target) {
        *value += *step;
        if (target <= *value) {
            *value = target;
            var_v1 = 1;
        }
    }
    return var_v1;
}

/* Decrease value toward target by the pointed-to step; return 1 only when this call reaches or crosses the target. */
s32 ParticleMath_ApproachFloatDownByPointer(f32* value, f32 target, f32* step) {
    s32 var_v1 = 0;

    if (target < *value) {
        *value -= *step;
        if (*value <= target) {
            *value = target;
            var_v1 = 1;
        }
    }
    return var_v1;
}

/* Increase value toward target by the pointed-to step; return 1 only when this call reaches or crosses the target. */
s32 ParticleMath_ApproachS32UpByPointer(s32* value, s32 target, s32* step) {
    s32 var_v1 = 0;

    if (*value < target) {
        *value += *step;
        if (*value >= target) {
            *value = target;
            var_v1 = 1;
        }
    }
    return var_v1;
}

/* Decrease value toward target by the pointed-to step; return 1 only when this call reaches or crosses the target. */
s32 ParticleMath_ApproachS32DownByPointer(s32* value, s32 target, s32* step) {
    s32 var_v1 = 0;

    if (target < *value) {
        *value -= *step;
        if (target >= *value) {
            *value = target;
            var_v1 = 1;
        }
    }
    return var_v1;
}

/* Increase value toward target by the pointed-to step; return 1 only when this call reaches or crosses the target. */
s32 ParticleMath_ApproachS16UpByPointer(s16* value, s16 target, s16* step) {
    s32 var_v1 = 0;

    if (*value < target) {
        *value += *step;
        if (*value >= target) {
            *value = target;
            var_v1 = 1;
        }
    }
    return var_v1;
}

/* Decrease value toward target by the pointed-to step; return 1 only when this call reaches or crosses the target. */
s32 ParticleMath_ApproachS16DownByPointer(s16* value, s16 target, s16* step) {
    s32 var_v1 = 0;

    if (target < *value) {
        *value -= *step;
        if (target >= *value) {
            *value = target;
            var_v1 = 1;
        }
    }
    return var_v1;
}

/* Move value toward target, clamp overshoot, and return 1 at the target. */
s32 ParticleMath_ApproachS8(s8* value, s8 target, s8 step) {
    s32 var_v1 = 0;
    s32 var_a3 = *value;

    if (var_a3 < target) {
        if (step >= 0) {
            var_a3 += step;
        } else {
            var_a3 -= step;
        }

        if (var_a3 >= target) {
            var_a3 = target;
            var_v1 = 1;
        }
    } else if (target < var_a3) {
        if (step >= 0) {
            var_a3 -= step;
        } else {
            var_a3 += step;
        }

        if (target >= var_a3) {
            var_a3 = target;
            var_v1 = 1;
        }
    } else {
        var_v1 = 1;
    }

    *value = var_a3;
    return var_v1;
}

/* Move value toward target, clamp overshoot, and return 1 at the target. */
s32 ParticleMath_ApproachU8(u8* value, u8 target, u8 step) {
    s32 var_v1 = 0;
    s32 var_a3 = *value;

    if (var_a3 < target) {
        var_a3 += step;
        if (var_a3 >= target) {
            var_a3 = target;
            var_v1 = 1;
        }
    } else if (target < var_a3) {
        var_a3 -= step;
        if (target >= var_a3) {
            var_a3 = target;
            var_v1 = 1;
        }
    } else {
        var_v1 = 1;
    }

    *value = var_a3;
    return var_v1;
}

/* Move value toward target, clamp overshoot, and return 1 at the target. */
s32 ParticleMath_ApproachS16(s16* value, s16 target, s16 step) {
    s32 var_v1 = 0;
    s32 var_a3 = *value;

    if (var_a3 < target) {
        if (step >= 0) {
            var_a3 += step;
        } else {
            var_a3 -= step;
        }

        if (var_a3 >= target) {
            var_a3 = target;
            var_v1 = 1;
        }
    } else if (target < var_a3) {
        if (step >= 0) {
            var_a3 -= step;
        } else {
            var_a3 += step;
        }

        if (target >= var_a3) {
            var_a3 = target;
            var_v1 = 1;
        }
    } else {
        var_v1 = 1;
    }

    *value = var_a3;
    return var_v1;
}

/* Approach a wrapping 16-bit angle along the selected short arc; return 1 when the target is reached. */
s32 ParticleMath_ApproachAngle(s16* value, s16 target, s16 step) {
    s32 a0 = *(u16*)value;
    s32 var_v1 = 0;
    s32 var_a1 = (u16)target;

    if ((u16)target == a0) {
        var_a1 = (u16)target;
        var_v1 = 1;
    } else {
        var_a1 = (u16)target - a0;
        if (var_a1 < -0x8000) {
            var_a1 = step + a0;
            if (((u16)target - var_a1) <= -65536) {
                var_a1 = (u16)target;
                var_v1 = 1;
            }
        } else if (var_a1 < 0) {
            var_a1 = a0 - step;
            if (((u16)target - var_a1) >= 0) {
                var_a1 = (u16)target;
                var_v1 = 1;
            }
        } else if (var_a1 < 0x8000) {
            var_a1 = step + a0;
            if (((u16)target - var_a1) <= 0) {
                var_a1 = (u16)target;
                var_v1 = 1;
            }
        } else {
            var_a1 = a0 - step;
            if (((u16)target - var_a1) >= 0x10000) {
                var_a1 = (u16)target;
                var_v1 = 1;
            }
        }
    }

    *value = var_a1;
    return var_v1;
}

/* Move value toward target, clamp overshoot, and return 1 at the target. */
s32 ParticleMath_ApproachS32(s32* value, s32 target, s32 step) {
    s32 temp_v0 = *value;
    s32 var_v1 = 0;

    if (temp_v0 < target) {
        if (step >= 0) {
            *value = temp_v0 + step;
        } else {
            *value = temp_v0 - step;
        }

        if (*value >= target) {
            *value = target;
            var_v1 = 1;
        }
    } else if (target < temp_v0) {
        if (step >= 0) {
            *value = temp_v0 - step;
        } else {
            *value = temp_v0 + step;
        }

        if (target >= *value) {
            *value = target;
            var_v1 = 1;
        }
    } else {
        var_v1 = 1;
    }

    return var_v1;
}

/* Move value toward target, clamp overshoot, and return 1 at the target. */
s32 ParticleMath_ApproachFloat(f32* value, f32 target, f32 step) {
    s32 var_v1 = 0;

    if (*value < target) {
        if (step >= 0.0f) {
            *value += step;
        } else {
            *value -= step;
        }

        if (target <= *value) {
            *value = target;
            var_v1 = 1;
        }
    } else if (target < *value) {
        if (step >= 0.0f) {
            *value -= step;
        } else {
            *value += step;
        }

        if (*value <= target) {
            *value = target;
            var_v1 = 1;
        }
    } else {
        var_v1 = 1;
    }

    return var_v1;
}

/* Advance by the signed step, clamp at the approached endpoint, and reverse the step there. */
void ParticleMath_BounceFloat(f32* value, f32 minimum, f32 maximum, f32* step) {
    *value += *step;

    if (*step >= 0.0f) {
        if (maximum <= *value) {
            *value = maximum;
            *step = -*step;
        }
    } else if (*value <= minimum) {
        *value = minimum;
        *step = -*step;
    }
}

/* Advance by the signed step, clamp at the approached endpoint, and reverse the step there. */
void ParticleMath_BounceS16(s16* value, s16 minimum, s16 maximum, s16* step) {
    *value += *step;
    if (*step >= 0) {
        if (*value >= maximum) {
            *value = maximum;
            *step = -*step;
        }
    } else if (minimum >= *value) {
        *value = minimum;
        *step = -*step;
    }
}

/* Copy all three vector components to the destination. */
void ParticleMath_CopyVec3f(Vec3f* destination, Vec3f* source) {
    destination->x = source->x;
    destination->y = source->y;
    destination->z = source->z;
}

/* Copy all three vector components to the destination. */
void ParticleMath_CopyVec3s(Vec3s* destination, Vec3s* source) {
    destination->x = source->x;
    destination->y = source->y;
    destination->z = source->z;
}

/* Add all three source components to the destination in place. */
void ParticleMath_AddVec3f(Vec3f* destination, Vec3f* source) {
    destination->x += source->x;
    destination->y += source->y;
    destination->z += source->z;
}

/* Add all three source components to the destination in place. */
void ParticleMath_AddVec3s(Vec3s* destination, Vec3s* source) {
    destination->x += source->x;
    destination->y += source->y;
    destination->z += source->z;
}

/* Return the sum of the three component-wise products. */
f32 ParticleMath_DotVec3f(Vec3f lhs, Vec3f rhs) {
    return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
}

/* Compare squared 2D distance with squared radius, including the boundary. */
s32 ParticleMath_IsWithinRadius2D(f32 first_x, f32 first_y, f32 second_x, f32 second_y, f32 radius) {
    s32 var_v1 = 0;

    if ((SQ(second_x - first_x) + SQ(second_y - first_y)) <= SQ(radius)) {
        var_v1 = 1;
    }
    return var_v1;
}

/* Increment the additive offset mixed into the next guRandom result. */
void ParticleMath_AdvanceRandomOffset(void) {
    sParticleRandomOffset++;
}

/* Mix guRandom with the shared offset, divide by 10, then reduce modulo range; zero range returns zero. */
u32 ParticleMath_RandomRange(s32 range) {
    u32 temp_a0 = guRandom() + sParticleRandomOffset;
    u32 var_v1;

    if (range == 0) {
        var_v1 = 0;
    } else {
        var_v1 = (temp_a0 / 10) % range;
    }
    return var_v1;
}

/* Add offset to a random value in [0, range) for a positive range. */
s32 ParticleMath_RandomRangeOffset(s32 range, s32 offset) {
    return ParticleMath_RandomRange(range) + offset;
}

/* Return a random integer in [-range, range) for a positive range. */
s32 ParticleMath_RandomSigned(s32 range) {
    return ParticleMath_RandomRange(range * 2) - range;
}

/* Add offset to the signed random result. */
s32 ParticleMath_RandomSignedOffset(s32 range, s32 offset) {
    return ParticleMath_RandomSigned(range) + offset;
}

/* Move a signed random result farther from zero by offset; zero takes the positive branch. */
s32 ParticleMath_RandomSignedOffsetBySign(s32 range, s32 offset) {
    s32 temp_v0 = ParticleMath_RandomSigned(range);

    if (temp_v0 >= 0) {
        temp_v0 += offset;
    } else {
        temp_v0 -= offset;
    }
    return temp_v0;
}

/* Empty callback retained at its original entry point. */
void ParticleMath_NoopStub(UNUSED s32 unused_value) {
}

/* Return MathUtil_Atan2s for the vector's Y and Z components, in that argument order. */
s16 ParticleMath_Atan2YZ(Vec3f vector) {
    return MathUtil_Atan2s(vector.y, vector.z);
}

/* Return MathUtil_Atan2s for the vector's Z and X components, in that argument order. */
s16 ParticleMath_Atan2ZX(Vec3f vector) {
    return MathUtil_Atan2s(vector.z, vector.x);
}

/* Return MathUtil_Atan2s for the vector's X and Y components, in that argument order. */
s16 ParticleMath_Atan2XY(Vec3f vector) {
    return MathUtil_Atan2s(vector.x, vector.y);
}

/* Write the YZ, ZX, and XY atan2 results to the three destination components. */
void ParticleMath_Vec3ToAngles(Vec3s* destination, Vec3f vector) {
    destination->x = MathUtil_Atan2s(vector.y, vector.z);
    destination->y = MathUtil_Atan2s(vector.z, vector.x);
    destination->z = MathUtil_Atan2s(vector.x, vector.y);
}

/* Return x*cos(angle) - y*sin(angle), the rotated 2D X component. */
f32 ParticleMath_Rotate2DX(f32 point_x, f32 point_y, s16 angle) {
    return (COSS(angle) * point_x) - (SINS(angle) * point_y);
}

/* Return x*sin(angle) + y*cos(angle), the rotated 2D Y component. */
f32 ParticleMath_Rotate2DY(f32 point_x, f32 point_y, s16 angle) {
    return (SINS(angle) * point_x) + (COSS(angle) * point_y);
}

/* Rotate the supplied vector around the Y axis and return the destination. */
Vec3f* ParticleMath_RotateVec3fY(Vec3f* destination, Vec3f vector, Vec3s rotation) {
    f32 temp_fv0 = SINS(rotation.y);
    f32 temp_fv1 = COSS(rotation.y);

    destination->x = (vector.x * temp_fv1) + (vector.z * temp_fv0);
    destination->y = vector.y;
    destination->z = (-vector.x * temp_fv0) + (vector.z * temp_fv1);

    return destination;
}

/* Build (length*sin(angle), 0, length*cos(angle)) and return the destination. */
Vec3f* ParticleMath_MakeXZVectorFromAngle(Vec3f* destination, f32 length, s16 angle) {
    f32 temp_fv0 = SINS(angle);
    f32 temp_fv1 = COSS(angle);

    destination->x = length * temp_fv0;
    destination->y = 0.0f;
    destination->z = length * temp_fv1;

    return destination;
}

/* Apply the explicit X/Y-angle transform to a copied vector and return the destination. */
Vec3f* ParticleMath_RotateVec3fXY(Vec3f* destination, Vec3f vector, Vec3s rotation) {
    f32 x = vector.x;
    f32 y = vector.y;
    f32 z = vector.z;
    f32 temp_fv0 = SINS(rotation.x);
    f32 temp_fv1 = COSS(rotation.x);
    f32 temp_ft4 = SINS(rotation.y);
    f32 temp_ft5 = COSS(rotation.y);

    destination->x = (x * temp_ft5) + (z * temp_ft4);
    destination->y = ((x * temp_fv0 * temp_ft4) + (y * temp_fv1)) - (z * temp_fv0 * temp_ft5);
    destination->z = (-x * temp_fv1 * temp_ft4) + (y * temp_fv0) + (z * temp_fv1 * temp_ft5);

    return destination;
}

/* Return the Euclidean distance between two 3D points. */
f32 ParticleMath_Distance3D(Vec3f start, Vec3f end) {
    return sqrtf(SQ(end.x - start.x) + SQ(end.y - start.y) + SQ(end.z - start.z));
}

/* Return the distance between the points projected onto the XZ plane. */
f32 ParticleMath_DistanceXZ(Vec3f start, Vec3f end) {
    return sqrtf(SQ(end.x - start.x) + SQ(end.z - start.z));
}

/* Normalize end - start into the destination; zero-length behavior belongs to Vec3f_Normalize. */
Vec3f* ParticleMath_NormalizedDirection(Vec3f* destination, Vec3f start, Vec3f end) {
    destination->x = end.x - start.x;
    destination->y = end.y - start.y;
    destination->z = end.z - start.z;

    Vec3f_Normalize(destination);

    return destination;
}

/* Divide the 3D distance by the divisor and convert the result to s32. */
s32 ParticleMath_DistanceScaled(Vec3f start, Vec3f end, f32 divisor) {
    return ParticleMath_Distance3D(start, end) / divisor;
}

/* Scale end - start to length and return the original distance / length as s32; no zero guard. */
s32 ParticleMath_ScaleDirectionToLength(Vec3f* destination, Vec3f start, Vec3f end, f32 length) {
    f32 temp_fv1;

    destination->x = end.x - start.x;
    destination->y = end.y - start.y;
    destination->z = end.z - start.z;

    temp_fv1 = length / sqrtf(SQ(destination->x) + SQ(destination->y) + SQ(destination->z));

    destination->x = destination->x * temp_fv1;
    destination->y = destination->y * temp_fv1;
    destination->z = destination->z * temp_fv1;

    return 1.0f / temp_fv1;
}

/* Rotate a +Z vector by independently sampled X/Y angles; this is not a uniform-sphere sampler. */
Vec3f* ParticleMath_RandomDirection(Vec3f* destination, f32 length) {
    Vec3f sp2C;
    Vec3s sp24;

    sp2C.x = 0.0f;
    sp2C.y = 0.0f;
    sp2C.z = length;

    sp24.x = ParticleMath_RandomRange(0x10000);
    sp24.y = ParticleMath_RandomRange(0x10000);

    ParticleMath_RotateVec3fXY(destination, sp2C, sp24);
    return destination;
}

/* Multiply all three components in place and return the vector. */
Vec3f* ParticleMath_ScaleVec3f(Vec3f* vector, f32 scale) {
    vector->x *= scale;
    vector->y *= scale;
    vector->z *= scale;

    return vector;
}

/* Return the YZ-component atan2 of end - start in that argument order. */
s16 ParticleMath_AngleXBetweenPoints(Vec3f start, Vec3f end) {
    return MathUtil_Atan2s(end.y - start.y, end.z - start.z);
}

/* Return the ZX-component atan2 of end - start in that argument order. */
s16 ParticleMath_AngleYBetweenPoints(Vec3f start, Vec3f end) {
    return MathUtil_Atan2s(end.z - start.z, end.x - start.x);
}

/* Return the XY-component atan2 of end - start in that argument order. */
s16 ParticleMath_AngleZBetweenPoints(Vec3f start, Vec3f end) {
    return MathUtil_Atan2s(end.x - start.x, end.y - start.y);
}

/* Select a fixed angular increment from the signed difference, or snap when the high bytes agree. */
s16 ParticleMath_ApproachAngleCoarse(s16 angle, s16 target_angle) {
    s16 temp_v0 = target_angle - angle;

    if ((angle >> 8) != (target_angle >> 8)) {
        if (temp_v0 < 0x400) {
            angle += 0x80;
        } else if (temp_v0 < 0x800) {
            angle += 0x200;
        } else if (temp_v0 < 0x4000) {
            angle += 0x400;
        } else if (temp_v0 < 0x8000) {
            angle += 0x700;
        } else if (temp_v0 < 0xC000) {
            angle -= 0x700;
        } else if (temp_v0 < 0xF800) {
            angle -= 0x400;
        } else if (temp_v0 < 0xFC00) {
            angle -= 0x200;
        } else {
            angle -= 0x80;
        }
    } else {
        angle = target_angle;
    }

    return angle;
}

/* Empty callback retained at its original entry point. */
void ParticleGfx_NoopStub1(void) {
}

/* Convert a float matrix into the next pool slot and load it as MODELVIEW; stop emitting when all 364 slots are used. */
void ParticleGfx_PushMatrix(MtxF* matrix) {
    if (sParticleMatrixPoolIndex < 0x16C) {
        MtxF_ToFixed(&sParticleMatrixPool->unk_0000[sParticleMatrixPoolIndex], matrix);

        gSPMatrix(gDisplayListHead++, (u32)&sParticleMatrixPool->unk_0000[sParticleMatrixPoolIndex] & 0x1FFFFFFF,
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        sParticleMatrixPoolIndex++;
    }
}

/* Build a translated 2D rotation matrix and apply uniform scale. */
void ParticleGfx_Build2DTransformMatrix(MtxF* matrix, s16 translation_x, s16 translation_y, s16 rotation_z, f32 scale) {
    f32 temp_fv0 = SINS(rotation_z) * scale;
    f32 temp_fv1 = COSS(rotation_z) * scale;

    matrix->mf[0][0] = temp_fv1;
    matrix->mf[0][1] = -temp_fv0;
    matrix->mf[0][2] = 0.0f;
    matrix->mf[0][3] = 0.0f;

    matrix->mf[1][0] = temp_fv0;
    matrix->mf[1][1] = temp_fv1;
    matrix->mf[1][2] = 0.0f;
    matrix->mf[1][3] = 0.0f;

    matrix->mf[2][0] = 0.0f;
    matrix->mf[2][1] = 0.0f;
    matrix->mf[2][2] = 1.0f;
    matrix->mf[2][3] = 0.0f;

    matrix->mf[3][0] = translation_x;
    matrix->mf[3][1] = translation_y;
    matrix->mf[3][2] = 0.0f;
    matrix->mf[3][3] = 1.0f;
}

/* Build a translated 2D rotation matrix and scale only its Y basis. */
void ParticleGfx_Build2DTransformMatrixYScaled(MtxF* matrix, s16 translation_x, s16 translation_y, s16 rotation_z, f32 scale_y) {
    f32 temp_fv0 = SINS(rotation_z);
    f32 temp_fv1 = COSS(rotation_z);

    matrix->mf[0][0] = temp_fv1;
    matrix->mf[0][1] = -temp_fv0;
    matrix->mf[0][2] = 0.0f;
    matrix->mf[0][3] = 0.0f;

    matrix->mf[1][0] = temp_fv0 * scale_y;
    matrix->mf[1][1] = temp_fv1 * scale_y;
    matrix->mf[1][2] = 0.0f;
    matrix->mf[1][3] = 0.0f;

    matrix->mf[2][0] = 0.0f;
    matrix->mf[2][1] = 0.0f;
    matrix->mf[2][2] = 1.0f;
    matrix->mf[2][3] = 0.0f;

    matrix->mf[3][0] = translation_x;
    matrix->mf[3][1] = translation_y;
    matrix->mf[3][2] = 0.0f;
    matrix->mf[3][3] = 1.0f;
}

/* Build the 2D transform with an added half turn and load it through the matrix pool. */
void ParticleGfx_Push2DTransform(s16 translation_x, s16 translation_y, s16 rotation_z, f32 scale) {
    MtxF sp20;

    ParticleGfx_Build2DTransformMatrix(&sp20, translation_x, translation_y, rotation_z + 0x8000, scale);
    ParticleGfx_PushMatrix(&sp20);
}

/* Build the 2D transform with an added half turn and load it through the matrix pool. */
void ParticleGfx_Push2DTransformYScaled(s16 translation_x, s16 translation_y, s16 rotation_z, f32 scale_y) {
    MtxF sp20;

    ParticleGfx_Build2DTransformMatrixYScaled(&sp20, translation_x, translation_y, rotation_z + 0x8000, scale_y);
    ParticleGfx_PushMatrix(&sp20);
}

/* Build a translated 3D rotation matrix and apply uniform scale. */
void ParticleGfx_BuildTransformMatrix(MtxF* matrix, Vec3f position, Vec3s rotation, f32 scale) {
    f32 temp_fa1;
    f32 temp_fs0;
    f32 temp_fs1;
    f32 temp_ft4;
    f32 temp_fv0;
    f32 temp_fv1;

    temp_ft4 = SINS(rotation.x);
    temp_fv0 = COSS(rotation.x);

    temp_fv1 = SINS(rotation.y);
    temp_fa1 = COSS(rotation.y);

    temp_fs0 = SINS(rotation.z);
    temp_fs1 = COSS(rotation.z);

    matrix->mf[0][0] = ((temp_fa1 * temp_fs1) + (temp_ft4 * temp_fv1 * temp_fs0)) * scale;
    matrix->mf[0][1] = temp_fv0 * temp_fs0 * scale;
    matrix->mf[0][2] = ((-temp_fv1 * temp_fs1) + (temp_ft4 * temp_fa1 * temp_fs0)) * scale;
    matrix->mf[0][3] = 0.0f;

    matrix->mf[1][0] = ((-temp_fa1 * temp_fs0) + (temp_ft4 * temp_fv1 * temp_fs1)) * scale;
    matrix->mf[1][1] = temp_fv0 * temp_fs1 * scale;
    matrix->mf[1][2] = ((temp_fv1 * temp_fs0) + (temp_ft4 * temp_fa1 * temp_fs1)) * scale;
    matrix->mf[1][3] = 0.0f;

    matrix->mf[2][0] = temp_fv0 * temp_fv1 * scale;
    matrix->mf[2][1] = -temp_ft4 * scale;
    matrix->mf[2][2] = temp_fv0 * temp_fa1 * scale;
    matrix->mf[2][3] = 0.0f;

    matrix->mf[3][0] = position.x;
    matrix->mf[3][1] = position.y;
    matrix->mf[3][2] = position.z;
    matrix->mf[3][3] = 1.0f;
}

/* Build a translated 3D rotation matrix and scale only its Y basis. */
void ParticleGfx_BuildTransformMatrixYScaled(MtxF* matrix, Vec3f position, Vec3s rotation, f32 scale_y) {
    f32 temp_fa0;
    f32 temp_fa1;
    f32 temp_fs0;
    f32 temp_ft5;
    f32 temp_fv0;
    f32 temp_fv1;

    temp_fa1 = SINS(rotation.x);
    temp_fv0 = COSS(rotation.x);

    temp_fv1 = SINS(rotation.y);
    temp_fa0 = COSS(rotation.y);

    temp_ft5 = SINS(rotation.z);
    temp_fs0 = COSS(rotation.z);

    matrix->mf[0][0] = (temp_fa0 * temp_fs0) + (temp_fa1 * temp_fv1 * temp_ft5);
    matrix->mf[0][1] = temp_fv0 * temp_ft5;
    matrix->mf[0][2] = (-temp_fv1 * temp_fs0) + (temp_fa1 * temp_fa0 * temp_ft5);
    matrix->mf[0][3] = 0.0f;

    matrix->mf[1][0] = ((-temp_fa0 * temp_ft5) + (temp_fa1 * temp_fv1 * temp_fs0)) * scale_y;
    matrix->mf[1][1] = temp_fv0 * temp_fs0 * scale_y;
    matrix->mf[1][2] = ((temp_fv1 * temp_ft5) + (temp_fa1 * temp_fa0 * temp_fs0)) * scale_y;
    matrix->mf[1][3] = 0.0f;

    matrix->mf[2][0] = temp_fv0 * temp_fv1;
    matrix->mf[2][1] = -temp_fa1;
    matrix->mf[2][2] = temp_fv0 * temp_fa0;
    matrix->mf[2][3] = 0.0f;

    matrix->mf[3][0] = position.x;
    matrix->mf[3][1] = position.y;
    matrix->mf[3][2] = position.z;
    matrix->mf[3][3] = 1.0f;
}

/* Build a translated 3D rotation matrix and scale its X/Y bases separately. */
void ParticleGfx_BuildTransformMatrixXYScaled(MtxF* matrix, Vec3f position, Vec3s rotation, f32 scale_x, f32 scale_y) {
    f32 temp_fa0;
    f32 temp_fa1;
    f32 temp_fs0;
    f32 temp_ft5;
    f32 temp_fv0;
    f32 temp_fv1;

    temp_fa1 = SINS(rotation.x);
    temp_fv0 = COSS(rotation.x);

    temp_fv1 = SINS(rotation.y);
    temp_fa0 = COSS(rotation.y);

    temp_ft5 = SINS(rotation.z);
    temp_fs0 = COSS(rotation.z);

    matrix->mf[0][0] = ((temp_fa0 * temp_fs0) + (temp_fa1 * temp_fv1 * temp_ft5)) * scale_x;
    matrix->mf[0][1] = temp_fv0 * temp_ft5 * scale_x;
    matrix->mf[0][2] = ((-temp_fv1 * temp_fs0) + (temp_fa1 * temp_fa0 * temp_ft5)) * scale_x;
    matrix->mf[0][3] = 0.0f;

    matrix->mf[1][0] = ((-temp_fa0 * temp_ft5) + (temp_fa1 * temp_fv1 * temp_fs0)) * scale_y;
    matrix->mf[1][1] = temp_fv0 * temp_fs0 * scale_y;
    matrix->mf[1][2] = ((temp_fv1 * temp_ft5) + (temp_fa1 * temp_fa0 * temp_fs0)) * scale_y;
    matrix->mf[1][3] = 0.0f;

    matrix->mf[2][0] = temp_fv0 * temp_fv1;
    matrix->mf[2][1] = -temp_fa1;
    matrix->mf[2][2] = temp_fv0 * temp_fa0;
    matrix->mf[2][3] = 0.0f;

    matrix->mf[3][0] = position.x;
    matrix->mf[3][1] = position.y;
    matrix->mf[3][2] = position.z;
    matrix->mf[3][3] = 1.0f;
}

/* Build a translated billboard matrix by transposing the supplied view basis and applying uniform scale. */
void ParticleGfx_BuildBillboardMatrix(MtxF* matrix, MtxF* view_matrix, Vec3f position, f32 scale) {
    matrix->mf[0][0] = view_matrix->mf[0][0] * scale;
    matrix->mf[0][1] = view_matrix->mf[1][0] * scale;
    matrix->mf[0][2] = view_matrix->mf[2][0] * scale;
    matrix->mf[0][3] = 0.0f;

    matrix->mf[1][0] = view_matrix->mf[0][1] * scale;
    matrix->mf[1][1] = view_matrix->mf[1][1] * scale;
    matrix->mf[1][2] = view_matrix->mf[2][1] * scale;
    matrix->mf[1][3] = 0.0f;

    matrix->mf[2][0] = view_matrix->mf[0][2] * scale;
    matrix->mf[2][1] = view_matrix->mf[1][2] * scale;
    matrix->mf[2][2] = view_matrix->mf[2][2] * scale;
    matrix->mf[2][3] = 0.0f;

    matrix->mf[3][0] = position.x;
    matrix->mf[3][1] = position.y;
    matrix->mf[3][2] = position.z;
    matrix->mf[3][3] = 1.0f;
}

/* Build a translated billboard matrix by transposing the supplied view basis and scaling only the second basis vector. */
void ParticleGfx_BuildBillboardMatrixYScaled(MtxF* matrix, MtxF* view_matrix, Vec3f position, f32 scale_y) {
    matrix->mf[0][0] = view_matrix->mf[0][0];
    matrix->mf[0][1] = view_matrix->mf[1][0];
    matrix->mf[0][2] = view_matrix->mf[2][0];
    matrix->mf[0][3] = 0.0f;

    matrix->mf[1][0] = view_matrix->mf[0][1] * scale_y;
    matrix->mf[1][1] = view_matrix->mf[1][1] * scale_y;
    matrix->mf[1][2] = view_matrix->mf[2][1] * scale_y;
    matrix->mf[1][3] = 0.0f;

    matrix->mf[2][0] = view_matrix->mf[0][2];
    matrix->mf[2][1] = view_matrix->mf[1][2];
    matrix->mf[2][2] = view_matrix->mf[2][2];
    matrix->mf[2][3] = 0.0f;

    matrix->mf[3][0] = position.x;
    matrix->mf[3][1] = position.y;
    matrix->mf[3][2] = position.z;
    matrix->mf[3][3] = 1.0f;
}

/* Build a translated billboard matrix by rotating its first two basis vectors and applying uniform scale. */
void ParticleGfx_BuildBillboardRotatedMatrix(MtxF* matrix, MtxF* view_matrix, Vec3f position, f32 scale, s16 rotation_z) {
    f32 temp_fv0 = SINS(rotation_z);
    f32 temp_fv1 = COSS(rotation_z);

    matrix->mf[0][0] = ((view_matrix->mf[0][0] * temp_fv1) + (view_matrix->mf[0][1] * temp_fv0)) * scale;
    matrix->mf[0][1] = ((view_matrix->mf[1][0] * temp_fv1) + (view_matrix->mf[1][1] * temp_fv0)) * scale;
    matrix->mf[0][2] = ((view_matrix->mf[2][0] * temp_fv1) + (view_matrix->mf[2][1] * temp_fv0)) * scale;
    matrix->mf[0][3] = 0.0f;

    matrix->mf[1][0] = ((view_matrix->mf[0][0] * -temp_fv0) + (view_matrix->mf[0][1] * temp_fv1)) * scale;
    matrix->mf[1][1] = ((view_matrix->mf[1][0] * -temp_fv0) + (view_matrix->mf[1][1] * temp_fv1)) * scale;
    matrix->mf[1][2] = ((view_matrix->mf[2][0] * -temp_fv0) + (view_matrix->mf[2][1] * temp_fv1)) * scale;
    matrix->mf[1][3] = 0.0f;

    matrix->mf[2][0] = view_matrix->mf[0][2] * scale;
    matrix->mf[2][1] = view_matrix->mf[1][2] * scale;
    matrix->mf[2][2] = view_matrix->mf[2][2] * scale;
    matrix->mf[2][3] = 0.0f;

    matrix->mf[3][0] = position.x;
    matrix->mf[3][1] = position.y;
    matrix->mf[3][2] = position.z;
    matrix->mf[3][3] = 1.0f;
}

/* Build a translated billboard matrix by rotating its first two basis vectors and scaling only the second basis vector. */
void ParticleGfx_BuildBillboardRotatedMatrixYScaled(MtxF* matrix, MtxF* view_matrix, Vec3f position, f32 scale_y, s16 rotation_z) {
    f32 temp_fv0 = SINS(rotation_z);
    f32 temp_fv1 = COSS(rotation_z);

    matrix->mf[0][0] = (view_matrix->mf[0][0] * temp_fv1) + (view_matrix->mf[0][1] * temp_fv0);
    matrix->mf[0][1] = (view_matrix->mf[1][0] * temp_fv1) + (view_matrix->mf[1][1] * temp_fv0);
    matrix->mf[0][2] = (view_matrix->mf[2][0] * temp_fv1) + (view_matrix->mf[2][1] * temp_fv0);
    matrix->mf[0][3] = 0.0f;

    matrix->mf[1][0] = ((view_matrix->mf[0][0] * -temp_fv0) + (view_matrix->mf[0][1] * temp_fv1)) * scale_y;
    matrix->mf[1][1] = ((view_matrix->mf[1][0] * -temp_fv0) + (view_matrix->mf[1][1] * temp_fv1)) * scale_y;
    matrix->mf[1][2] = ((view_matrix->mf[2][0] * -temp_fv0) + (view_matrix->mf[2][1] * temp_fv1)) * scale_y;
    matrix->mf[1][3] = 0.0f;

    matrix->mf[2][0] = view_matrix->mf[0][2];
    matrix->mf[2][1] = view_matrix->mf[1][2];
    matrix->mf[2][2] = view_matrix->mf[2][2];
    matrix->mf[2][3] = 0.0f;

    matrix->mf[3][0] = position.x;
    matrix->mf[3][1] = position.y;
    matrix->mf[3][2] = position.z;
    matrix->mf[3][3] = 1.0f;
}

/* Build a scaled basis from a normalized direction and cross products, with the supplied translation. */
void ParticleGfx_BuildDirectionMatrix(MtxF* matrix, Vec3f direction, Vec3f position, s16 reference_angle, f32 scale) {
    Vec3f sp3C;
    Vec3f sp30;
    Vec3f sp24;

    Vec3f_SetComponentsDuplicate(&sp3C, SINS(reference_angle), 0.0f, COSS(reference_angle));
    Vec3f_Normalize(&direction);
    Vec3f_CrossProduct(&sp30, &direction, &sp3C);
    Vec3f_Normalize(&sp30);
    Vec3f_CrossProduct(&sp24, &sp30, &direction);
    Vec3f_Normalize(&sp24);

    matrix->mf[0][0] = sp30.x * scale;
    matrix->mf[0][1] = sp30.y * scale;
    matrix->mf[0][2] = sp30.z * scale;
    matrix->mf[3][0] = position.x;

    matrix->mf[1][0] = direction.x * scale;
    matrix->mf[1][1] = direction.y * scale;
    matrix->mf[1][2] = direction.z * scale;
    matrix->mf[3][1] = position.y;

    matrix->mf[2][0] = sp24.x * scale;
    matrix->mf[2][1] = sp24.y * scale;
    matrix->mf[2][2] = sp24.z * scale;
    matrix->mf[3][2] = position.z;

    matrix->mf[0][3] = 0.0f;
    matrix->mf[1][3] = 0.0f;
    matrix->mf[2][3] = 0.0f;
    matrix->mf[3][3] = 1.0f;
}

/* Build the corresponding transform and load it through the matrix pool. */
void ParticleGfx_PushTransformMatrix(Vec3f position, Vec3s rotation, f32 scale) {
    MtxF sp28;

    ParticleGfx_BuildTransformMatrix(&sp28, position, rotation, scale);
    ParticleGfx_PushMatrix(&sp28);
}

/* Build the corresponding transform and load it through the matrix pool. */
void ParticleGfx_PushTransformMatrixYScaled(Vec3f position, Vec3s rotation, f32 scale_y) {
    MtxF sp28;

    ParticleGfx_BuildTransformMatrixYScaled(&sp28, position, rotation, scale_y);
    ParticleGfx_PushMatrix(&sp28);
}

/* Build the corresponding transform and load it through the matrix pool. */
void ParticleGfx_PushTransformMatrixXYScaled(Vec3f position, Vec3s rotation, f32 scale_x, f32 scale_y) {
    MtxF sp28;

    ParticleGfx_BuildTransformMatrixXYScaled(&sp28, position, rotation, scale_x, scale_y);
    ParticleGfx_PushMatrix(&sp28);
}

/* Build the corresponding transform and load it through the matrix pool. */
void ParticleGfx_PushBillboardMatrix(CameraNode* camera, Vec3f position, f32 scale) {
    MtxF sp20;

    ParticleGfx_BuildBillboardMatrix(&sp20, &camera->view.floatMatrix, position, scale);
    ParticleGfx_PushMatrix(&sp20);
}

/* Build the corresponding transform and load it through the matrix pool. */
void ParticleGfx_PushBillboardRotatedMatrix(CameraNode* camera, Vec3f position, f32 scale, s16 rotation_z) {
    MtxF sp28;

    ParticleGfx_BuildBillboardRotatedMatrix(&sp28, &camera->view.floatMatrix, position, scale, rotation_z);
    ParticleGfx_PushMatrix(&sp28);
}

/* Build the corresponding transform and load it through the matrix pool. */
void ParticleGfx_PushBillboardMatrixYScaled(CameraNode* camera, Vec3f position, f32 scale_y) {
    MtxF sp20;

    ParticleGfx_BuildBillboardMatrixYScaled(&sp20, &camera->view.floatMatrix, position, scale_y);
    ParticleGfx_PushMatrix(&sp20);
}

/* Build the corresponding transform and load it through the matrix pool. */
void ParticleGfx_PushBillboardRotatedMatrixYScaled(CameraNode* camera, Vec3f position, f32 scale_y, s16 rotation_z) {
    MtxF sp28;

    ParticleGfx_BuildBillboardRotatedMatrixYScaled(&sp28, &camera->view.floatMatrix, position, scale_y, rotation_z);
    ParticleGfx_PushMatrix(&sp28);
}

/* Add half turns to rotation X/Y, build the transform, and load it through the matrix pool. */
void ParticleGfx_PushTransformMatrixWithHalfTurnXY(Vec3f position, Vec3s rotation, f32 scale) {
    MtxF sp30;
    Vec3s sp28;

    sp28.x = rotation.x + 0x8000;
    sp28.y = rotation.y + 0x8000;
    sp28.z = rotation.z;

    ParticleGfx_BuildTransformMatrix(&sp30, position, sp28, scale);
    ParticleGfx_PushMatrix(&sp30);
}

/* Build the corresponding transform and load it through the matrix pool. */
void ParticleGfx_PushDirectionMatrix(Vec3f position, Vec3f direction, f32 scale) {
    MtxF sp30;

    ParticleGfx_BuildDirectionMatrix(&sp30, direction, position, 0, scale);
    ParticleGfx_PushMatrix(&sp30);
}
