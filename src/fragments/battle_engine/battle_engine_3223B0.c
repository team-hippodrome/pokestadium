#include "battle_engine.h"
#include "src/math_util.h"
#include "pokemon.h"

/*
This file covers these classes of moves:

This file covers this {non obvious effect} for:
    - 


As well as effects for these specific moves:
    - 



*/

void BattleAnim_SetupSurfHitSpark(Particle* arg0) {
    s16 sp26 = Particle_FieldAC_GetS16_0C038(arg0);
    s16 sp24 = Particle_FieldCF_GetS16_0C058(arg0);

    Particle_Scale_SetScaled(arg0, (ParticleMath_RandomRange(0xA) * 0.01f) + 0.05f);
    Particle_MotionStep_SetYScaled(arg0, (ParticleMath_RandomRange(0x1E) * 0.1f) + 1.0f);
    Particle_RotationStep_SetY(arg0, ParticleMath_RandomRange(0x10000));
    Particle_StepMagnitude_SetScaled(arg0, 5.0f);
    Particle_MagnitudeStep_SetScaled(arg0, ParticleMath_RandomRange(0xF) + 5.0f);
    Particle_FieldAE_SetS16_0BF4C(arg0, ParticleMath_RandomRangeOffset(0x1000, 0x800));
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, sp24);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp26);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Handles the hit spark from being hit by Surf
*/
void BattleAnim_RunSurfHitSpark(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupSurfHitSpark(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_EnableRendering(arg0);
                Particle_FieldA6_SetS16_0BCA8(arg0, 0xA);
            }
            break;

        case 3:
            if (Particle_FieldA6_DecrementToZeroStep1_0BDAC(arg0) != 0) {
                Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 3, 1, 1, 0);
            }
            break;

        case 4:
            Particle_DisableAndDetach(arg0);
            break;
    }

    if (Particle_IsRenderingEnabled(arg0) != 0) {
        Particle_MotionOffset_AddYFromMotionStep(arg0);
        Particle_StepMagnitude_Approach(arg0, arg0->magnitudeStep, 0.5f);
        Particle_UpdateRotatingMotionOffsetXZ(arg0, arg0->stepMagnitude, arg0->unk_AE);
    }
}

void BattleAnim_SetupBubbleBubbles(Particle* arg0, s16 arg1) {
    s16 sp2E = Particle_FieldAC_GetS16_0C038(arg0);

    Particle_FieldCA_SetU8_0BECC(arg0, 7);
    Particle_Scale_SetScaled(arg0, 0.1f);
    Particle_Rotation_AddVec(arg0, ParticleMath_RandomSigned(0xC00), ParticleMath_RandomSigned(0x1000), 0);
    ParticleGfx_RecolorEnvRandomPalette(arg0);
    Particle_StepMagnitude_SetScaled(arg0, (ParticleMath_RandomRange(6) * 0.5f) + 4.0f);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp2E * 2);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Spawns Bubbles for Bubble
*/
void BattleAnim_RunBubbleBubbles(Particle* arg0) {
    s16 temp_a1 = Particle_FieldCF_GetS16_0C058(arg0);

    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupBubbleBubbles(arg0, temp_a1);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_FieldA6_SetS16_0BCA8(arg0, ParticleMath_RandomRange(0xA) + 0x19);
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            Particle_Scale_Approach(arg0, 0.15f, 0.01f);
            Particle_AddMotionOffsetFromRotationTemplate1(arg0);
            if ((Particle_FieldA6_DecrementToZeroStep1_0BDAC(arg0) != 0) && (Particle_PrimAlpha_Approach_0BC2C(arg0, 0x20, 0x20) != 0)) {
                Particle_AdvanceLifecycle(arg0);
            }
            break;

        case 4:
            Particle_DisableAndDetach(arg0);
            break;
    }
}

void BattleAnim_SetupBubbleBeamBubblesSlow(Particle* arg0) {
    UNUSED s32 pad[3];
    s16 sp22 = Particle_FieldAC_GetS16_0C038(arg0);

    Particle_FieldCF_GetS16_0C058(arg0);
    Particle_Scale_SetScaled(arg0, 0.05f);
    Particle_SetRandomPositionOffsetXYZ(arg0, 4, 2, 2);
    Particle_Rotation_AddX(arg0, ParticleMath_RandomSignedOffset(0x200, 0xE00));
    Particle_RotationStep_SetX(arg0, ParticleMath_RandomRange(0x1000) + 0x100);
    Particle_FieldB0_SetS16_0BFEC(arg0, -0x1800 - ParticleMath_RandomRange(0x1000));
    Particle_Rotation_AddY(arg0, ParticleMath_RandomSigned(0x3000));
    Particle_StepMagnitude_SetScaled(arg0, (ParticleMath_RandomRange(0xF) * 0.1f) + 1.0f);
    ParticleGfx_RecolorEnvRandomPalette(arg0);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp22 * 2);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_ContinueBubbleBeamBubblesSlow(Particle* arg0) {
    if (arg0->phase == 1) {
        Particle_Rotation_DecrementXTo(arg0, arg0->unk_B0, arg0->rotationStep.x);
        Particle_AddMotionOffsetFromRotationTemplate1(arg0);
    }
}

/*
    handles the slower additional bubbles that appear when using bubble beam as opposed to bubble
*/
void BattleAnim_RunBubbleBeamBubblesSlow(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupBubbleBeamBubblesSlow(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_BeginPhase(arg0);
                Particle_EnableRendering(arg0);
                Particle_FieldA6_SetS16_0BCA8(arg0, ParticleMath_RandomRange(0xA) + 8);
            }
            break;

        case 3:
            Particle_Scale_Approach(arg0, 0.15f, 0.02f);
            if (Particle_FieldA6_DecrementToZeroStep1_0BDAC(arg0) != 0) {
                Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 3, 1, 1, 0);
            }
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRenderingEnabled(arg0) != 0) {
        BattleAnim_ContinueBubbleBeamBubblesSlow(arg0);
    }
}

void BattleAnim_SetupBubbleBeamBubblesFast(Particle* arg0) {
    s16 sp56 = Particle_FieldAC_GetS16_0C038(arg0);
    f32 var_fa0;
    UNUSED s32 pad[2];
    Vec3f sp3C;
    Vec3f sp30;
    s16 sp2E;

    Particle_FieldCA_SetU8_0BECC(arg0, 7);
    Particle_Scale_SetScaled(arg0, 0.1f);
    Particle_SetScaleParameter20(arg0, 0.2f);
    Particle_SetScaleParameter24(arg0, 0.02f);
    BattleAnim_GetSecondaryOwnerAnchorPosition(&sp3C);
    BattleAnim_GetTertiaryOwnerBone9Position(&sp30);
    var_fa0 = sp30.x - sp3C.x;
    if (var_fa0 <= 0.0f) {
        var_fa0 = -var_fa0;
    }
    sp2E = -MathUtil_Atan2s(var_fa0, sp30.y - sp3C.y);
    Particle_Rotation_AddVec(arg0, sp2E + ParticleMath_RandomSigned(0x800), ParticleMath_RandomSigned(0x800), 0);
    Particle_SetRandomPositionOffsetXYZ(arg0, 4, 2, 2);
    ParticleGfx_RecolorEnvRandomPalette(arg0);
    Particle_StepMagnitude_SetScaled(arg0, (ParticleMath_RandomRange(6) * 0.5f) + 5.0f);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp56 * 2);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

/*
    handles the faster additional bubbles that appear when using bubble beam as opposed to bubble
*/
void BattleAnim_RunBubbleBeamBubblesFast(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupBubbleBeamBubblesFast(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_FieldA6_SetS16_0BCA8(arg0, ParticleMath_RandomRange(0x14) + 0xF);
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            Particle_ApproachScaleUsingFields20And24(arg0);
            Particle_AddMotionOffsetFromRotationTemplate1(arg0);
            if (Particle_FieldA6_DecrementToZeroStep1_0BDAC(arg0) != 0) {
                Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 3, 1, 1, 0);
            }
            break;

        case 4:
            Particle_DisableAndDetach(arg0);
            break;
    }
}

void BattleAnim_SetupUnusedRotatingSparkVariant(Particle* arg0) {
    UNUSED s32 pad[2];
    f32 temp_fv0;
    Vec3f sp40;
    Vec3f sp34;
    s16 sp32;
    s16 sp30 = Particle_FieldAC_GetS16_0C038(arg0);
    s16 sp2E = Particle_FieldCE_GetS32_0C040(arg0);

    Particle_FieldCA_SetU8_0BECC(arg0, 7);
    Particle_Scale_SetScaled(arg0, 0.1f);
    BattleAnim_GetSecondaryOwnerAnchorPosition(&sp40);
    BattleAnim_GetTertiaryOwnerBone9Position(&sp34);

    temp_fv0 = sp34.x - sp40.x;
    if (temp_fv0 <= 0.0f) {
        temp_fv0 = -temp_fv0;
    }

    sp32 = MathUtil_Atan2s(temp_fv0, sp34.y - sp40.y) * -1;
    Particle_Rotation_AddY(arg0, ParticleMath_RandomSigned(0x400));

    switch (sp2E) {
        case 0:
            Particle_FieldAE_SetS16_0BF4C(arg0, sp32 + 0x800);
            Particle_FieldB0_SetS16_0BFEC(arg0, 0x1800);
            break;

        case 1:
            Particle_FieldAE_SetS16_0BF4C(arg0, sp32 - 0x1000);
            Particle_FieldB0_SetS16_0BFEC(arg0, 0x1000);
            break;

        case 2:
            Particle_FieldAE_SetS16_0BF4C(arg0, sp32 + 0xC00);
            Particle_FieldB0_SetS16_0BFEC(arg0, -0x1400);
            break;

        case 3:
            Particle_FieldAE_SetS16_0BF4C(arg0, sp32 - 0xA00);
            Particle_FieldB0_SetS16_0BFEC(arg0, -0x2000);
            break;
    }

    Particle_StepMagnitude_SetScaled(arg0, 5.25f);
    ParticleGfx_RecolorEnvRandomPalette(arg0);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp30 * 2);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_MotionUnusedRotatingSparkVariant(Particle* arg0) {
    switch (arg0->phase) {
        case 1:
            if (Particle_TickPhaseTimer(arg0, 2) != 0) {
                Particle_AdvancePhase(arg0);
                Particle_Rotation_AddX(arg0, arg0->unk_AE);
            }
            break;

        case 2:
            Particle_Rotation_AddZ(arg0, arg0->unk_B0);
            break;
    }

    if (arg0->phase > 0) {
        Particle_AddMotionOffsetFromRotationTemplate5(arg0);
    }
}

void BattleAnim_CallbackUnusedRotatingSparkVariant(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupUnusedRotatingSparkVariant(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_BeginPhase(arg0);
                Particle_EnableRendering(arg0);
                Particle_FieldA6_SetS16_0BCA8(arg0, 0x1E);
            }
            break;

        case 3:
            Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0);
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRenderingEnabled(arg0) != 0) {
        BattleAnim_MotionUnusedRotatingSparkVariant(arg0);
    }
}

void BattleAnim_SetupUnusedAimedSparkVariant(Particle* arg0) {
    s16 sp4E = Particle_FieldCE_GetS32_0C040(arg0);
    Vec3f sp40;
    Vec3f sp34;
    s16 temp_v1;

    Particle_FieldCA_SetU8_0BECC(arg0, 7);
    Particle_Scale_SetScaled(arg0, 0.1f);
    Particle_SetScaleParameter20(arg0, 0.1f);
    BattleAnim_GetSecondaryOwnerAnchorPosition(&sp40);
    BattleAnim_GetTertiaryOwnerBone9Position(&sp34);

    temp_v1 = ParticleMath_AngleXBetweenPoints(sp40, sp34);

    switch (sp4E) {
        case 0:
            Particle_Rotation_AddX(arg0, temp_v1 + 0x2000);
            Particle_Rotation_AddY(arg0, -0x4000);
            break;

        case 1:
            Particle_Rotation_AddX(arg0, temp_v1 + 0x6000);
            Particle_Rotation_AddY(arg0, -0x4000);
            break;
    }

    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackUnusedAimedSparkVariant(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupUnusedAimedSparkVariant(arg0);
            break;

        case 2:
            Particle_PrimAlpha_ApproachAndStepState_0BC5C(arg0, 0xFF, 4);
            break;

        case 3:
            Particle_TickDelayAndAdvanceLifecycle(arg0, 0x46);
            break;

        case 4:
            Particle_PrimAlpha_ApproachAndStepState_0BC5C(arg0, 0, 0x10);
            break;

        case 5:
            Particle_DisableAndDetach(arg0);
            break;
    }

    Particle_Scale_Approach(arg0, 50.0f, 0.2f);
    Particle_Field20_Approach_0BA84(arg0, 1.0f, 0.1f);
}

void BattleAnim_SetupWaterGunAndHydroPumpEmitterFade(Particle* arg0) {
    switch (Particle_FieldCE_GetS32_0C040(arg0)) {
        case 0:
            Particle_Scale_SetScaled(arg0, 0.5f);
            Particle_SetScaleParameter20(arg0, 1.2f);
            Particle_SetScaleParameter24(arg0, 0.1f);
            break;

        case 1:
            Particle_Scale_SetScaled(arg0, 0.3f);
            Particle_SetScaleParameter20(arg0, 0.8f);
            Particle_SetScaleParameter24(arg0, 0.05f);
            break;
    }

    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0x26);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Handles the fading of the burst emitters for Water Gun and Hydro Pump
*/
void BattleAnim_RunWaterGunAndHydroPumpEmitterFade(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupWaterGunAndHydroPumpEmitterFade(arg0);
            break;

        case 2:
            Particle_ApproachScaleUsingFields20And24(arg0);
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 2, 0);
            Particle_PrimAlpha_Approach_0BC2C(arg0, 0x20, 8);
            break;

        case 3:
            Particle_MarkInactive(arg0);
            break;
    }
}

void BattleAnim_SetupHydroPumpBurstChild(Particle* arg0) {
    UNUSED s32 pad;

    Particle_FieldAC_GetS16_0C038(arg0);
    Particle_Scale_SetScaled(arg0, 0.5f);
    Particle_Rotation_AddVec(arg0, ParticleMath_RandomRange(0x10000), ParticleMath_RandomRange(0x10000), 0);
    Particle_StepMagnitude_SetScaled(arg0, 1.0f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0x27);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Handles a child particle emitted by the Hydro Pump burst
*/
void BattleAnim_RunHydroPumpBurstChild(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupHydroPumpBurstChild(arg0);
            break;

        case 2:
            Particle_Scale_Approach(arg0, 0.3f, 0.05f);
            Particle_PrimAlpha_Approach_0BC2C(arg0, 0x20, 0x10);
            Particle_StepMagnitude_Approach(arg0, 0.5f, 0.05f);
            Particle_AddMotionOffsetFromRotationTemplate1(arg0);
            Particle_PositionOffset_AddYScaled(arg0, -1.0f);
            Particle_TickDelayAndAdvanceLifecycle(arg0, 0x10);
            break;

        case 3:
            Particle_MarkInactive(arg0);
            break;
    }
}

void BattleAnim_SetupHydroPumpMetadataBurstChild(Particle* arg0) {
    s16 sp26 = Particle_FieldCE_GetS32_0C040(arg0);

    Particle_Scale_SetScaled(arg0, 0.6f);

    if (sp26 == 1) {
        Particle_Field38_AddXZFromField94YOffset180_08F38(arg0, 6.0f);
    }

    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0x28);
    Particle_SetPrimAlpha(arg0, 0xC8);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Handles a metadata-controlled Hydro Pump burst particle
*/
void BattleAnim_RunHydroPumpMetadataBurstChild(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupHydroPumpMetadataBurstChild(arg0);
            break;

        case 2:
            if (Particle_PrimAlpha_Approach_0BC2C(arg0, 0x80, 0x20) != 0) {
                Particle_AdvanceLifecycle(arg0);
            }
            break;

        case 3:
            Particle_DisableAndDetach(arg0);
            break;
    }

    Particle_Scale_Approach(arg0, 0.35f, 0.05f);
}

void BattleAnim_SetupHydroPumpBurstEmitter(Particle* arg0) {
    Particle_Scale_SetScaled(arg0, 0.7f);
    Particle_StepMagnitude_SetScaled(arg0, 12.0f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0x28);
    Particle_SetPrimAlpha(arg0, 0xFF);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Emits metadata-controlled particles and child projectiles for Hydro Pump
*/
void BattleAnim_RunHydroPumpBurstEmitter(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupHydroPumpBurstEmitter(arg0);
            break;

        case 2:
            BattleAnim_EmitParticleBurstWithMetadata(arg0, BattleAnim_RunHydroPumpMetadataBurstChild, &gBattleAnimParticleDescriptors[8], 1, 0, 0);
            BattleAnim_EmitParticleBurstWithMetadata(arg0, BattleAnim_RunHydroPumpMetadataBurstChild, &gBattleAnimParticleDescriptors[8], 1, 0, 1);
            BattleAnim_EmitParticleBurst(arg0, BattleAnim_RunHydroPumpBurstChild, &gBattleAnimParticleDescriptors[0xA], 4);
            Particle_TickDelayAndAdvanceLifecycle(arg0, 0x14);
            break;

        case 3:
            Particle_DisableAndDetach(arg0);
            break;
    }

    Particle_AddMotionOffsetFromRotationTemplate1(arg0);
}

void BattleAnim_SetupWaterGunProjectileChild(Particle* arg0) {
    UNUSED s32 pad;

    Particle_FieldAC_GetS16_0C038(arg0);
    Particle_Scale_SetScaled(arg0, 0.2f);
    Particle_Rotation_AddVec(arg0, ParticleMath_RandomRange(0x10000), ParticleMath_RandomRange(0x10000), 0);
    Particle_StepMagnitude_SetScaled(arg0, 0.5f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0x27);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Handles the child projectiles for Water gun
*/
void BattleAnim_RunWaterGunProjectileChild(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupWaterGunProjectileChild(arg0);
            break;

        case 2:
            Particle_Scale_Approach(arg0, 0.1f, 0.02f);
            Particle_PrimAlpha_Approach_0BC2C(arg0, 0x20, 0xC);
            Particle_AddMotionOffsetFromRotationTemplate1(arg0);
            Particle_PositionOffset_AddYScaled(arg0, -0.5f);
            Particle_TickDelayAndAdvanceLifecycle(arg0, 0x10);
            break;

        case 3:
            Particle_MarkInactive(arg0);
            break;
    }
}

void BattleAnim_SetupWaterGunProjectileMetadata(Particle* arg0) {
    s16 sp26 = Particle_FieldCE_GetS32_0C040(arg0);

    Particle_Scale_SetScaled(arg0, 0.2f);

    switch (sp26) {
        case 1:
            Particle_Field38_AddXZFromField94YOffset180_08F38(arg0, 3.0f);
            break;

        case 2:
            Particle_Field38_AddXZFromField94YOffset180_08F38(arg0, 6.0f);
            break;
    }

    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0x28);
    Particle_SetPrimAlpha(arg0, 0xC8);
    Particle_AdvanceLifecycle(arg0);
}
/*
    Handles the metadata for the water gun projectile
*/
void BattleAnim_RunWaterGunProjectileMetadata(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupWaterGunProjectileMetadata(arg0);
            break;

        case 2:
            if (Particle_PrimAlpha_Approach_0BC2C(arg0, 0x80, 0x20) != 0) {
                Particle_AdvanceLifecycle(arg0);
            }
            break;

        case 3:
            Particle_DisableAndDetach(arg0);
            break;
    }

    Particle_Scale_Approach(arg0, 0.1f, 0.02f);
}

void BattleAnim_SetupWaterGunProjectile(Particle* arg0) {
    Particle_Scale_SetScaled(arg0, 0.2f);
    Particle_StepMagnitude_SetScaled(arg0, 10.0f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0x28);
    Particle_SetPrimAlpha(arg0, 0xFF);
    Particle_AdvanceLifecycle(arg0);
}
/*
    Handles the parent projectile for Water gun
*/
void BattleAnim_RunWaterGunProjectile(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupWaterGunProjectile(arg0);
            break;

        case 2:
            BattleAnim_EmitParticleBurstWithMetadata(arg0, BattleAnim_RunWaterGunProjectileMetadata, &gBattleAnimParticleDescriptors[8], 1, 0, 0);
            BattleAnim_EmitParticleBurstWithMetadata(arg0, BattleAnim_RunWaterGunProjectileMetadata, &gBattleAnimParticleDescriptors[8], 1, 0, 1);
            BattleAnim_EmitParticleBurstWithMetadata(arg0, BattleAnim_RunWaterGunProjectileMetadata, &gBattleAnimParticleDescriptors[8], 1, 0, 2);
            BattleAnim_EmitParticleBurst(arg0, BattleAnim_RunWaterGunProjectileChild, &gBattleAnimParticleDescriptors[0xA], 4);
            Particle_TickDelayAndAdvanceLifecycle(arg0, 0x20);
            break;

        case 3:
            Particle_DisableAndDetach(arg0);
            break;
    }
    Particle_AddMotionOffsetFromRotationTemplate1(arg0);
}

void BattleAnim_SetupIceBeamColdBreath(Particle* arg0) {
    UNUSED s32 pad[3];
    s16 sp22 = Particle_FieldAC_GetS16_0C038(arg0);
    s16 sp20 = Particle_FieldCF_GetS16_0C058(arg0);

    Particle_Scale_SetScaled(arg0, 0.25f);
    Particle_SetRandomPositionOffsetXYZ(arg0, 4, 2, 2);
    Particle_Rotation_AddX(arg0, ParticleMath_RandomSignedOffset(0x200, -0xE00));
    Particle_RotationStep_SetX(arg0, ParticleMath_RandomRange(0x400) + 0x100);
    Particle_FieldB0_SetS16_0BFEC(arg0, ParticleMath_RandomRange(0x1000) + 0x1000);
    Particle_Rotation_AddY(arg0, ParticleMath_RandomSigned(0x1800));
    Particle_StepMagnitude_SetScaled(arg0, (ParticleMath_RandomRange(0x28) * 0.1f) + 1.0f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, sp20);
    Particle_SetPrimAlpha(arg0, 0xC8);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp22 * 2);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_ContinueIceBeamColdBreath(Particle* arg0) {
    if (arg0->phase == 1) {
        Particle_ApproachRotationXTo(arg0, arg0->unk_B0, arg0->rotationStep.x);
        Particle_AddMotionOffsetFromRotationTemplate1(arg0);
        Particle_PositionOffset_AddYScaled(arg0, -0.2f);
    }
}

/*
    Handles the cold breath that emanates from a pokemon's mouth
    when they use ice beam
*/
void BattleAnim_RunIceBeamColdBreath(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupIceBeamColdBreath(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_BeginPhase(arg0);
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            Particle_Scale_Approach(arg0, 0.7f, 0.1f);
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 2, 0);
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRenderingEnabled(arg0) != 0) {
        BattleAnim_ContinueIceBeamColdBreath(arg0);
    }
}

void BattleAnim_SetupMegaKickAndPunchProjectile(Particle* arg0) {
    s16 sp26 = Particle_FieldAC_GetS16_0C038(arg0);

    Particle_Scale_SetScaled(arg0, 0.75f - (sp26 * 0.1f));
    Particle_StepMagnitude_SetScaled(arg0, 10.0f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0x28);
    Particle_SetPrimAlpha(arg0, (0xFF - (sp26 << 5)) & 0xFF);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp26 + 1);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

/*
    handles projectile for Mega kick and mega punch 
*/
void BattleAnim_RunMegaKickAndPunchProjectile(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupMegaKickAndPunchProjectile(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            Particle_TickDelayAndAdvanceLifecycle(arg0, 0xA);
            Particle_AddMotionOffsetFromRotationTemplate1(arg0);
            break;

        case 4:
            Particle_DisableAndDetach(arg0);
            break;
    }
}

void BattleAnim_SetupInitialBeamEmanator(Particle* arg0) {
    Particle_Scale_SetScaled(arg0, 1.0f);
    Particle_SetScaleParameter20(arg0, 2.0f);
    Particle_SetScaleParameter24(arg0, 0.75f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_AdvanceLifecycle(arg0);
}

/*
    Runs the first emanator that shows up in Ice Beam, Psybeam, Aurora Beam, Hyper Beam, Solar Beam, and Tri Attack.
    Most Beam mosts switch to the Pulsing emanator after a few frames
*/
void BattleAnim_RunInitialBeamEmanator(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupInitialBeamEmanator(arg0);
            break;

        case 2:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 3, 1, 1, 0);
            Particle_ApproachScaleUsingFields20And24(arg0);
            break;

        case 3:
            Particle_DisableAndDetach(arg0);
            break;
    }
}

void BattleAnim_SetupPulsingBeamEmanator(Particle* arg0) {
    Particle_Scale_SetScaled(arg0, 0.4f);
    Particle_SetScaleParameter20(arg0, 0.7f);
    Particle_SetScaleParameter24(arg0, 0.1f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_SetTextureFrame(arg0, 1);
    Particle_Rotation_SetZ(arg0, ParticleMath_RandomRange(0x10000));
    Particle_AdvanceLifecycle(arg0);
}

/*
    handles the pulsing beam emanator for Ice Beam, Psybeam, Aurora Beam, Hyper Beam, and Solar Beam
*/
void BattleAnim_RunPulsingBeamEmanator(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupPulsingBeamEmanator(arg0);
            break;

        case 2:
            if (Particle_ApproachStoredScaleAndAdvanceLifecycle(arg0) != 0) {
                Particle_SetScaleParameter20(arg0, 0.2f);
            }
            break;

        case 3:
            Particle_ApproachStoredScaleAndAdvanceLifecycle(arg0);
            break;

        case 4:
            Particle_SetScaleParameter20(arg0, 0.7f);
            Particle_SetScaleParameter24(arg0, 0.1f);
            arg0->lifecycle = 2;
            break;
    }

    if (Particle_FieldA6_DecrementToZeroStep1_0BDAC(arg0) != 0) {
        Particle_DisableAndDetach(arg0);
    }
}

void BattleAnim_SetupStrongBeamHitEffect(Particle* arg0) {
    Particle_Scale_SetScaled(arg0, 2.0f);
    Particle_Field38_AddXZFromField94YOffset180_08F38(arg0, 10.0f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_SetPrimAlpha(arg0, 0xFF);
    Particle_SetTextureFrame(arg0, 1);
    Particle_Rotation_SetZ(arg0, ParticleMath_RandomRange(0x10000));
    Particle_AdvanceLifecycle(arg0);
}

/*
    Handles the hit effect from being hit by Ice beam, Hyper Beam, Solarbeam, or Sky attack
*/
void BattleAnim_RunStrongBeamHitEffect(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupStrongBeamHitEffect(arg0);
            break;

        case 2:
            Particle_Scale_ApproachAndStepState(arg0, 5.0f, 1.0f);
            break;

        case 3:
            Particle_Scale_ApproachAndStepState(arg0, 0.0f, 1.0f);
            Particle_PrimAlpha_Approach_0BC2C(arg0, 0x40, 0x20);
            break;

        case 4:
            Particle_DisableAndDetach(arg0);
            break;
    }
}

void BattleAnim_SetupPsybeamHitSparkFlicker(Particle* arg0) {
    Particle_FieldCF_GetS16_0C058(arg0);
    Particle_Scale_SetScaled(arg0, 2.0f);
    Particle_Field38_AddXZFromField94YOffset180_08F38(arg0, 10.0f);
    ParticleGfx_RecolorPrimFromPaletteSetB(arg0, ParticleMath_RandomRange(6));
    ParticleGfx_RecolorEnvFromPaletteSetB(arg0, ParticleMath_RandomRange(6));
    Particle_SetTextureFrame(arg0, 1);
    Particle_Rotation_SetZ(arg0, ParticleMath_RandomRange(0x10000));
    Particle_AdvanceLifecycle(arg0);
}

/*
    handles the hit spark when being hit by psybeam
*/
void BattleAnim_RunPsybeamHitSparkFlicker(Particle* arg0) {
    Particle_FieldCF_GetS16_0C058(arg0);

    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupPsybeamHitSparkFlicker(arg0);
            break;

        case 2:
            Particle_Scale_ApproachAndStepState(arg0, 5.0f, 1.0f);
            break;

        case 3:
            Particle_Scale_ApproachAndStepState(arg0, 0.0f, 1.0f);
            Particle_PrimAlpha_Approach_0BC2C(arg0, 0x40, 0x20);
            break;

        case 4:
            Particle_DisableAndDetach(arg0);
            break;
    }

    if (sParticleFrameMod16 == 0) {
        ParticleGfx_RecolorPrimFromPaletteSetB(arg0, ParticleMath_RandomRange(6));
        ParticleGfx_RecolorEnvFromPaletteSetB(arg0, ParticleMath_RandomRange(6));
    }
}


void BattleAnim_SetupAuroraBeamHitSpark(Particle* arg0) {
    UNUSED s16 pad;
    s16 sp24;

    Particle_FieldCF_GetS16_0C058(arg0);
    sp24 = Particle_FieldAC_GetS16_0C038(arg0);
    Particle_Scale_SetScaled(arg0, 2.0f);
    Particle_Field38_AddXZFromField94YOffset180_08F38(arg0, 10.0f);
    ParticleGfx_RecolorPrim(arg0, 0xA);
    ParticleGfx_RecolorEnvFromPaletteSetA(arg0, sp24 % 6);
    Particle_SetTextureFrame(arg0, 1);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp24 * 2);
    Particle_Rotation_AddZ(arg0, (sp24 << 0x10) / 10);
    Particle_RotationStep_SetZ(arg0, ParticleMath_RandomSignedOffsetBySign(0x800, 0x400));
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Handles the hit spark when being hit by aurora beam
*/
void BattleAnim_RunAuroraBeamHitSpark(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupAuroraBeamHitSpark(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            Particle_Scale_ApproachAndStepState(arg0, 4.0f, 1.0f);
            break;

        case 4:
            Particle_TickDelayAndAdvanceLifecycle(arg0, 8);
            break;

        case 5:
            Particle_Scale_ApproachAndStepState(arg0, 0.0f, 1.0f);
            break;

        case 6:
            Particle_DisableAndDetach(arg0);
            break;
    }

    Particle_Rotation_AddZFromRotationStep(arg0);
}

void BattleAnim_SetupStrongOutwardMovingLargeHitSpark(Particle* arg0) {
    Particle_FieldAC_GetS16_0C038(arg0);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_Scale_SetScaled(arg0, 1.35f);
    Particle_PositionOffset_AddXZFromRotationY(arg0, 5.0f);
    Particle_StepMagnitude_SetScaled(arg0, 5.5f);
    Particle_Rotation_AddVec(arg0, ParticleMath_RandomRange(0x10000), ParticleMath_RandomRange(0x10000), 0);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Handles hit effect shared by Hydro Pump, Ice Beam, Hyper Beam, Solar Beam, Waterfall, and Sky Attack
*/
void BattleAnim_RunStrongOutwardMovingLargeHitSpark(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupStrongOutwardMovingLargeHitSpark(arg0);
            break;

        case 2:
            Particle_PrimAlpha_Approach_0BC2C(arg0, 0x20, 0x16);
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 2, 0);
            break;

        case 3:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRunning(arg0) != 0) {
        Particle_StepMagnitude_Approach(arg0, 3.4f, 1.0f);
        Particle_AddMotionOffsetFromRotationTemplate1(arg0);
    }
}

void BattleAnim_SetupBlizzardHitUpwardColdAir(Particle* arg0) {
    Particle_FieldAC_GetS16_0C038(arg0);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_Scale_SetScaled(arg0, 1.0f);
    Particle_PositionOffset_AddXZFromRotationY(arg0, 5.0f);
    Particle_StepMagnitude_SetScaled(arg0, 4.0f);
    Particle_Rotation_AddVec(arg0, ParticleMath_RandomRange(0x10000), ParticleMath_RandomRange(0x10000), 0);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Handles the cold air that emanates upward from pokemon hit by blizzard
*/
void BattleAnim_RunBlizzardHitUpwardColdAir(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupBlizzardHitUpwardColdAir(arg0);
            break;

        case 2:
            Particle_PrimAlpha_Approach_0BC2C(arg0, 0x20, 0x16);
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 2, 0);
            break;

        case 3:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRunning(arg0) != 0) {
        Particle_StepMagnitude_Approach(arg0, 2.5f, 0.75f);
        Particle_AddMotionOffsetFromRotationTemplate1(arg0);
    }
}

void BattleAnim_SetupWaterGunHitEffect(Particle* arg0) {
    Particle_FieldAC_GetS16_0C038(arg0);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_Scale_SetScaled(arg0, 0.6f);
    Particle_PositionOffset_AddXZFromRotationY(arg0, 5.0f);
    Particle_StepMagnitude_SetScaled(arg0, 3.0f);
    Particle_Rotation_AddVec(arg0, ParticleMath_RandomRange(0x10000), ParticleMath_RandomRange(0x10000), 0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_RunWaterGunHitEffect(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupWaterGunHitEffect(arg0);
            break;

        case 2:
            Particle_PrimAlpha_Approach_0BC2C(arg0, 0x20, 0x16);
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 2, 0);
            break;

        case 3:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRunning(arg0) != 0) {
        Particle_StepMagnitude_Approach(arg0, 1.5f, 0.4f);
        Particle_AddMotionOffsetFromRotationTemplate1(arg0);
    }
}

void BattleAnim_SetupWeakBeamHitEffect(Particle* arg0) {
    UNUSED s32 pad;

    switch (Particle_FieldCF_GetS16_0C058(arg0)) {
        case 0:
            ParticleGfx_RecolorPrim(arg0, 0xA);
            ParticleGfx_RecolorEnvFromPaletteSetA(arg0, ParticleMath_RandomRange(6));
            break;

        case 1:
            ParticleGfx_RecolorPrimFromPaletteSetB(arg0, ParticleMath_RandomRange(6));
            ParticleGfx_RecolorEnvFromPaletteSetB(arg0, ParticleMath_RandomRange(6));
            break;

        case 2:
            ParticleGfx_RecolorPrimEnvFromPaletteSetB(arg0, ParticleMath_RandomRange(6));
            break;
    }

    Particle_Scale_SetScaled(arg0, 0.8f);
    Particle_PositionOffset_AddXZFromRotationY(arg0, 5.0f);
    Particle_StepMagnitude_SetScaled(arg0, 3.0f);
    Particle_Rotation_AddVec(arg0, ParticleMath_RandomRange(0x10000), ParticleMath_RandomRange(0x10000), 0);
    Particle_AdvanceLifecycle(arg0);
}

/*
    handles shared effect being hit by Psybeam, bubble beam, and aurora beam
*/
void BattleAnim_RunWeakBeamHitEffect(Particle* arg0) {
    s16 sp2E = Particle_FieldCF_GetS16_0C058(arg0);

    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupWeakBeamHitEffect(arg0);
            break;

        case 2:
            Particle_PrimAlpha_Approach_0BC2C(arg0, 0x20, 0x16);
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 2, 0);
            break;

        case 3:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRunning(arg0) != 0) {
        Particle_StepMagnitude_Approach(arg0, 1.5f, 0.5f);
        Particle_AddMotionOffsetFromRotationTemplate1(arg0);

        switch (sp2E) {
            case 0:
                ParticleGfx_RecolorEnvFromPaletteSetA(arg0, ParticleMath_RandomRange(6));
                break;

            case 1:
                if (sParticleFrameMod8 == 0) {
                    ParticleGfx_RecolorPrimFromPaletteSetB(arg0, ParticleMath_RandomRange(6));
                    ParticleGfx_RecolorEnvFromPaletteSetB(arg0, ParticleMath_RandomRange(6));
                }
                break;

            case 2:
                ParticleGfx_RecolorPrimEnvFromPaletteSetB(arg0, ParticleMath_RandomRange(6));
                break;
        }
    }
}

void BattleAnim_SetupTriAttackHitSpark(Particle* arg0) {
    s16 sp2E;
    s16 sp2C;
    s16 sp2A;

    sp2E = Particle_FieldAC_GetS16_0C038(arg0);
    sp2C = Particle_FieldCF_GetS16_0C058(arg0);
    sp2A = Particle_FieldCE_GetS32_0C040(arg0);

    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, sp2C);
    Particle_Scale_SetScaled(arg0, 0.6f);
    Particle_SetScaleParameter20(arg0, 1.2f);
    Particle_SetScaleParameter24(arg0, 0.2f);

    switch (sp2A) {
        case 0:
            Particle_SetPosition(arg0, 0.0f, 10.0f, 0.0f);
            break;

        case 1:
            Particle_SetPosition(arg0, 0.0f, -5.0f, 10.0f);
            break;

        case 2:
            Particle_SetPosition(arg0, 0.0f, -5.0f, -10.0f);
            break;
    }

    Particle_PositionOffset_AddXZFromRotationY(arg0, 5.0f);
    Particle_StepMagnitude_SetScaled(arg0, 1.0f);
    Particle_Rotation_AddVec(arg0, ParticleMath_RandomRange(0x10000), ParticleMath_RandomRange(0x10000), 0);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp2E);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Handles the Tri Attack hit spark
*/
void BattleAnim_RunTriAttackHitSpark(Particle* arg0) {
    Particle_FieldCE_GetS32_0C040(arg0);

    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupTriAttackHitSpark(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            Particle_ApproachScaleUsingFields20And24(arg0);
            Particle_PrimAlpha_Approach_0BC2C(arg0, 0x20, 0x16);
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 2, 0);
            Particle_StepMagnitude_Approach(arg0, 1.5f, 0.1f);
            Particle_AddMotionOffsetFromRotationTemplate1(arg0);
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }
}

/*
    Handles hit effect shared by Hydro Pump, Ice Beam, Hyper Beam, Solar Beam, Waterfall, and Sky Attack
*/
void BattleAnim_SetupStrongOutwardMovingSmallHitSpark(Particle* arg0) {
    s16 sp2E = Particle_FieldAC_GetS16_0C038(arg0);
    s16 sp2C = Particle_FieldCF_GetS16_0C058(arg0);

    Particle_SetRandomPositionOffsetXYZ(arg0, 0, 0xF, 0x14);
    Particle_PositionOffset_AddYScaled(arg0, 5.0f);
    Particle_Scale_SetScaled(arg0, 0.2f);
    Particle_StepMagnitude_SetScaled(arg0, 2.5f);
    Particle_Rotation_AddVec(arg0, ParticleMath_RandomSigned(0x2000), ParticleMath_RandomSigned(0x2000), 0);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, sp2C);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp2E * 4);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

// handles some shared hit effect between Hydro Pump, Ice Beam, Aurora Beam, Solar Beam, Waterfall, Sky Attack
/*
    Moves: Aurora Beam (hit), Hydro Pump (hit), Ice Beam (hit), Sky Attack (hit), Solar Beam (hit), Waterfall
    (hit)
    TODO: Human contributor add screenshot
*/
void BattleAnim_RunStrongOutwardMovingSmallHitSpark(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupStrongOutwardMovingSmallHitSpark(arg0);
            break;

        case 2:
            if (Particle_TickDelayAndAdvanceLifecycle(arg0, arg0->unk_A6) != 0) {
                Particle_FieldA6_SetS16_0BCA8(arg0, 2);
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 1, 0);
            Particle_Scale_Approach(arg0, 0.5f, 0.1f);
            Particle_AddMotionOffsetFromRotationTemplate1(arg0);
            if (Particle_FieldA6_DecrementToZeroStep1_0BDAC(arg0) != 0) {
                Particle_PrimAlpha_Approach_0BC2C(arg0, 0, 0x20);
            }
            break;

        case 4:
            Particle_DisableAndDetach(arg0);
            break;
    }
}

void BattleAnim_SetupUnusedIndexedFadeVariantA(Particle* arg0) {
    s16 sp2E = Particle_FieldAC_GetS16_0C038(arg0);

    Particle_FieldCF_GetS16_0C058(arg0);
    Particle_SetRandomPositionOffsetXYZ(arg0, 0xA, 0x14, 0x14);
    Particle_Scale_SetScaled(arg0, 0.25f);
    Particle_StepMagnitude_SetScaled(arg0, 2.5f);
    Particle_Rotation_AddVec(arg0, ParticleMath_RandomSigned(0x2000), ParticleMath_RandomSigned(0x2000), 0);
    ParticleGfx_RecolorPrimRandomPalette(arg0);
    Particle_SetPrimAlpha(arg0, 0xC8);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp2E * 4);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackUnusedIndexedFadeVariantA(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupUnusedIndexedFadeVariantA(arg0);
            break;

        case 2:
            if (Particle_TickDelayAndAdvanceLifecycle(arg0, arg0->unk_A6) != 0) {
                Particle_FieldA6_SetS16_0BCA8(arg0, 2);
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 1, 0);
            Particle_Scale_Approach(arg0, 0.4f, 0.1f);
            Particle_AddMotionOffsetFromRotationTemplate1(arg0);
            if (Particle_FieldA6_DecrementToZeroStep1_0BDAC(arg0) != 0) {
                Particle_PrimAlpha_Approach_0BC2C(arg0, 0, 0x20);
            }
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }
}

void BattleAnim_SetupUnusedIndexedFadeVariantB(Particle* arg0) {
    s16 sp2E = Particle_FieldAC_GetS16_0C038(arg0);

    Particle_FieldCF_GetS16_0C058(arg0);
    Particle_SetRandomPositionOffsetXYZ(arg0, 0xA, 0x14, 0x14);
    Particle_Scale_SetScaled(arg0, 0.4f);
    Particle_StepMagnitude_SetScaled(arg0, 2.5f);
    Particle_Rotation_AddVec(arg0, ParticleMath_RandomSigned(0x2000), ParticleMath_RandomSigned(0x2000), 0);
    Particle_SetPrimColor(arg0, 0xFF, 0xFF, 0xFF);
    ParticleGfx_RecolorEnvRandomPalette(arg0);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp2E * 4);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackUnusedIndexedFadeVariantB(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupUnusedIndexedFadeVariantB(arg0);
            break;

        case 2:
            if (Particle_TickDelayAndAdvanceLifecycle(arg0, arg0->unk_A6) != 0) {
                Particle_EnableRendering(arg0);
                Particle_SetTextureFrame(arg0, 1);
            }
            break;

        case 3:
            Particle_Scale_Approach(arg0, 0.5f, 0.025f);
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 2, 7, 1, 1, 0);
            Particle_AddMotionOffsetFromRotationTemplate1(arg0);
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }
}

void BattleAnim_SetupBubbleBubbleBeamSharedEffect(Particle* arg0) {
    Vec3s sp28;
    UNUSED s16 pad;
    s16 sp24;

    Particle_FieldCF_GetS16_0C058(arg0);
    sp24 = Particle_FieldAC_GetS16_0C038(arg0);
    Particle_Scale_SetScaled(arg0, 0.05f);
    Particle_SetRotationTargetXY(arg0, arg0->rotation.x, arg0->rotation.y + 0x8000);
    Particle_RotationStep_SetXY(arg0, 0x800, 0x300);
    sp28.x = sp24 << 0xC;
    sp28.y = ParticleMath_RandomSignedOffsetBySign(0x3000, 0x1000);
    sp28.z = 0;
    Particle_Rotation_AddVecFromVec3s(arg0, sp28);
    ParticleGfx_RecolorEnvRandomPalette(arg0);
    Particle_FieldA6_SetS16_0BCA8(arg0, ParticleMath_RandomRange(0x10) + 8);
    Particle_StepMagnitude_SetScaled(arg0, 4.0f);
    Particle_AdvanceLifecycle(arg0);
}

/*
    handles effects shared by bubble and bubble beam
*/
void BattleAnim_RunBubbleBubbleBeamSharedEffect(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupBubbleBubbleBeamSharedEffect(arg0);
            break;

        case 2:
            Particle_Scale_Approach(arg0, 0.15f, 0.01f);
            Particle_StepMagnitude_Approach(arg0, 1.0f, 0.7f);
            Particle_ApproachRotationTargetY(arg0);
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_FieldA6_SetS16_0BCA8(arg0, ParticleMath_RandomRangeOffset(0xC, 0x20));
                if (arg0->rotation.y >= arg0->rotationTarget.y) {
                    Particle_RotationTarget_SetX(arg0, -0x4000);
                } else {
                    Particle_RotationTarget_SetX(arg0, 0x4000);
                }
            }
            break;

        case 3:
            Particle_ApproachRotationTargetX(arg0);
            Particle_StepMagnitude_Approach(arg0, 0.5f, 0.2f);
            Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0);
            break;

        case 4:
            Particle_ApproachRotationTargetX(arg0);
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 3, 1, 1, 0);
            break;

        case 5:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRunning(arg0) != 0) {
        Particle_AddMotionOffsetFromRotationTemplate3(arg0);
    }
}

void BattleAnim_SetupHydroPumpColorShiftFade(Particle* arg0) {
    Particle_Scale_SetScaled(arg0, 0.2f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_SetPrimAlpha(arg0, 0xC8);
    Particle_FieldA6_SetS16_0BCA8(arg0, 0x18);
    arg0->stepMagnitude = 0.02f;
    Particle_AdvanceLifecycle(arg0);
}

/*
    Handles the growth, color shift, and fade of a Hydro Pump particle
*/
void BattleAnim_RunHydroPumpColorShiftFade(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupHydroPumpColorShiftFade(arg0);
            break;

        case 2:
            Particle_Scale_Approach(arg0, 2.0f, 0.02f);
            Particle_ApproachEnvGreen(arg0, 0xC8, 2);
            Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0);
            break;

        case 3:
            ParticleMath_ApproachFloat(&arg0->stepMagnitude, 0.008f, 0.001f);
            Particle_Scale_Approach(arg0, 2.0f, arg0->stepMagnitude);
            Particle_PrimR_Approach_0BAE4(arg0, 0x78, 8);
            Particle_PrimB_Approach_0BB44(arg0, 0x96, 8);
            Particle_PrimG_Approach_0BB14(arg0, 0x96, 8);
            Particle_EnvR_Approach_0BB90(arg0, 0x78, 8);
            Particle_ApproachEnvBlue(arg0, 0x96, 8);
            Particle_ApproachEnvGreen(arg0, 0x96, 8);
            Particle_PrimAlpha_ApproachAndStepState_0BC5C(arg0, 0, 0xC);
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }
}

void BattleAnim_SetupWaterGunEmitterColoration(Particle* arg0) {
    s16 sp26 = Particle_FieldCE_GetS32_0C040(arg0);

    Particle_Scale_SetScaled(arg0, 0.05f);
    Particle_SetScaleParameter20(arg0, 0.3f);
    Particle_SetScaleParameter24(arg0, 0.005f);

    switch (sp26) {
        case 0:
            Particle_Rotation_SetY(arg0, 0);
            break;

        case 1:
            Particle_Rotation_SetY(arg0, 0x5500);
            break;

        case 2:
            Particle_Rotation_SetY(arg0, -0x5600);
            break;
    }

    Particle_AddPositionOffsetFromRotationDirection(arg0, 10.0f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_SetPrimAlpha(arg0, 0xC8);
    Particle_FieldA6_SetS16_0BCA8(arg0, 0x18);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Handles the coloring of the burst emitter for Water gun
*/
void BattleAnim_RunWaterGunEmitterColoration(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupWaterGunEmitterColoration(arg0);
            break;

        case 2:
            Particle_ApproachEnvGreen(arg0, 0xC8, 2);
            Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0);
            break;

        case 3:
            Particle_PrimR_Approach_0BAE4(arg0, 0x78, 8);
            Particle_PrimB_Approach_0BB44(arg0, 0x96, 8);
            Particle_PrimG_Approach_0BB14(arg0, 0x96, 8);
            Particle_EnvR_Approach_0BB90(arg0, 0x78, 8);
            Particle_ApproachEnvBlue(arg0, 0x96, 8);
            Particle_ApproachEnvGreen(arg0, 0x96, 8);
            Particle_PrimAlpha_ApproachAndStepState_0BC5C(arg0, 0, 0xC);
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }

    Particle_ApproachScaleUsingFields20And24(arg0);
}


void BattleAnim_SetupHydroPumpWaterGunProjectileHit(Particle* arg0) {
    UNUSED s16 pad;
    s16 sp2C;

    Particle_FieldAC_GetS16_0C038(arg0);
    sp2C = Particle_FieldCF_GetS16_0C058(arg0);
    Particle_Scale_SetScaled(arg0, 0.5f);
    Particle_Rotation_AddVec(arg0, ParticleMath_RandomRange(0x10000), ParticleMath_RandomRange(0x10000), 0);
    Particle_PositionOffset_AddXZFromRotationY(arg0, 5.0f);
    Particle_StepMagnitude_SetScaled(arg0, 1.0f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, sp2C);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Handles the projectile shared by being hit by water gun and hydro pump
*/
void BattleAnim_RunHydroPumpWaterGunProjectileHit(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupHydroPumpWaterGunProjectileHit(arg0);
            break;

        case 2:
            Particle_Scale_Approach(arg0, 0.3f, 0.02f);
            Particle_PrimAlpha_Approach_0BC2C(arg0, 0x20, 0xC);
            Particle_AddMotionOffsetFromRotationTemplate1(arg0);
            Particle_PositionOffset_AddYScaled(arg0, -1.0f);
            Particle_TickDelayAndAdvanceLifecycle(arg0, 0x10);
            break;

        case 3:
            Particle_MarkInactive(arg0);
            break;
    }

    if (arg0->worldPosition.y < 0.0f) {
        Particle_MarkInactive(arg0);
    }
}


/*
    Starts the effect for Mega Punch
*/
void BattleAnim_StartMegaPunchEffect(void) {
    BattleAnim_CreateEffectSlotForCurrentOwner(4, BattleAnim_RunMegaKickAndPunchProjectile, &gBattleAnimParticleDescriptors[8], 0, 4, 0x11, gBattleAnimCurrentAnchor, 0, 0);
}

/*
    Dispatches mega kick animation for all pokemon other than Charmander and Jynx
*/
void BattleAnim_DispatchNormalMegaKickAnimation(void) {
    s32 has_special_mega_kick_animation = 0;

    if (BattleAnim_IsCurrentOwnerSpecies(JYNX) != 0) {
        has_special_mega_kick_animation = 1;
    }

    if (BattleAnim_IsCurrentOwnerSpecies(CHARMANDER) != 0) {
        has_special_mega_kick_animation = 1;
    }

    if (has_special_mega_kick_animation == 0) {
        BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunMegaKickAndPunchProjectile, &gBattleAnimParticleDescriptors[8], 0, 4, 0x11, gBattleAnimCurrentAnchor, 0, 0);
    }
}

// initializes using bubble
void BattleAnim_InitializeBubbleUse(void) {
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 1, 0x3C, BattleAnim_RunBubbleBubbles, &gBattleAnimParticleDescriptors[0x45], 0, 1, 0x11, gBattleAnimCurrentAnchor, 0, 0);
}

// initializes being hit by bubble
void BattleAnim_InitializeBubbleHit(void) {
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunDefaultHitSpark, &gBattleAnimParticleDescriptors[0x34], 0, 1, 0x11, 9, 3, 5);
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunBubbleBubbleBeamSharedEffect, &gBattleAnimParticleDescriptors[0x45], 0, 0x10, 0x11, 9, 0, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(8, BattleAnim_RunBubbleBubbleBeamSharedEffect, &gBattleAnimParticleDescriptors[0x45], 0, 0x10, 0x11, 9, 0, 0);
}

// initializes water gun
void BattleAnim_InitializeWaterGunUse(void) {
    BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunWaterGunAndHydroPumpEmitterFade, &gBattleAnimParticleDescriptors[0xE], 0, 1, 0x11, gBattleAnimCurrentAnchor, 0, 1);
    BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunWaterGunAndHydroPumpEmitterFade, &gBattleAnimParticleDescriptors[0x19], 0, 1, 0x11, gBattleAnimCurrentAnchor, 0, 1);
    BattleAnim_CreateEffectSlotForCurrentOwner(4, BattleAnim_RunWaterGunProjectile, &sDefaultParticleDescriptor, 0, 1, 0x11, gBattleAnimCurrentAnchor, 0, 0);
}

void BattleAnim_InitializeWaterGunHit(void) {
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 1, 0xF, BattleAnim_RunWaterGunHitEffect, &gBattleAnimParticleDescriptors[0x19], 0, 1, 0x10, gBattleAnimCurrentAnchor, 0x26, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(0xC, BattleAnim_RunWaterGunEmitterColoration, &gBattleAnimParticleDescriptors[0x4B], 0, 1, 0xA, 0, 0x4C, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(0xC, BattleAnim_RunWaterGunEmitterColoration, &gBattleAnimParticleDescriptors[0x4B], 0, 1, 0xA, 0, 0x4C, 1);
    BattleAnim_CreateEffectSlotForCurrentOwner(0xC, BattleAnim_RunWaterGunEmitterColoration, &gBattleAnimParticleDescriptors[0x4B], 0, 1, 0xA, 0, 0x4C, 2);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(4, 1, 0x10, BattleAnim_RunHydroPumpWaterGunProjectileHit, &gBattleAnimParticleDescriptors[0xA], 0, 2, 0x11, gBattleAnimCurrentAnchor, 0x4C, 0);
}

// initializes hydro pump
void BattleAnim_InitializeHydroPumpUse(void) {
    BattleAnim_SpawnGlobalTintColorFadeDetach(0, 0x40, 6);
    BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunWaterGunAndHydroPumpEmitterFade, &gBattleAnimParticleDescriptors[0xE], 0, 1, 0x11, gBattleAnimCurrentAnchor, 0, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunWaterGunAndHydroPumpEmitterFade, &gBattleAnimParticleDescriptors[0x19], 0, 1, 0x11, gBattleAnimCurrentAnchor, 0, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(4, BattleAnim_RunHydroPumpBurstEmitter, &gBattleAnimParticleDescriptors[8], 0, 1, 0x11, gBattleAnimCurrentAnchor, 0, 0);
}

// initializes hydro pump
void BattleAnim_InitializeHydroPumpHit(void) {
    BattleAnim_SpawnBeamBoomWaterfallHueChange(4, 0xA, 0x35, 0x10, 0xFF);
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunExpandingHitSpark, &gBattleAnimParticleDescriptors[4], 0, 1, 0x10, gBattleAnimCurrentAnchor, 0x35, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 5, 3, BattleAnim_RunStrongOutwardMovingLargeHitSpark, &gBattleAnimParticleDescriptors[0x19], 0, 8, 0x10, gBattleAnimCurrentAnchor, 0x26, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0xA, 1, 8, BattleAnim_RunStrongOutwardMovingSmallHitSpark, &gBattleAnimParticleDescriptors[0x19], 0, 3, 0x11, gBattleAnimCurrentAnchor, 0x26, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(0xC, BattleAnim_RunHydroPumpColorShiftFade, &gBattleAnimParticleDescriptors[0x4B], 0, 1, 0xA, 0, 0x4C, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(2, 1, 0x10, BattleAnim_RunHydroPumpWaterGunProjectileHit, &gBattleAnimParticleDescriptors[0xA], 0, 4, 0x11, gBattleAnimCurrentAnchor, 0x4C, 0);
}

// initializes ice beam
void BattleAnim_InitializeIceBeamUse(void) {
    BattleAnim_SpawnWhiteHueChange(0, 0, 0x20, 0xA, 0xA0);
    BattleAnim_SpawnGlobalTintColorFadeDetach(0, 8, 0x35);
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunInitialBeamEmanator, &gBattleAnimParticleDescriptors[0x53], 0, 1, 0x10, gBattleAnimCurrentAnchor, 0x40, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(6, BattleAnim_RunPulsingBeamEmanator, &gBattleAnimParticleDescriptors[0x53], 0x6E, 1, 0x10, gBattleAnimCurrentAnchor, 0x40, 1);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 3, 0x1E, BattleAnim_RunIceBeamColdBreath, &gBattleAnimParticleDescriptors[0x4A], 0, 1, 0x11, gBattleAnimCurrentAnchor, 0x13, 0);
}

// initializes psybeam
void BattleAnim_InitializePsybeamUse(void) {
    BattleAnim_SpawnWhiteHueChange(0, 0, 0x20, 0xA, 0xA0);
    BattleAnim_SpawnGlobalTintColorFadeDetach(0, 8, 6);
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunInitialBeamEmanator, &gBattleAnimParticleDescriptors[0x53], 0, 1, 0x10, gBattleAnimCurrentAnchor, 0xD, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(6, BattleAnim_RunPulsingBeamEmanator, &gBattleAnimParticleDescriptors[0x53], 0x46, 1, 0x10, gBattleAnimCurrentAnchor, 0xD, 1);
}

void BattleAnim_InitializeBubbleBeamUse(void) {
    BattleAnim_SpawnGlobalTintColorFadeDetach(0, 8, 6);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 1, 0x3C, BattleAnim_RunBubbleBeamBubblesFast, &gBattleAnimParticleDescriptors[0x45], 0, 4, 0x11, gBattleAnimCurrentAnchor, 0, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 1, 0x3C, BattleAnim_RunBubbleBeamBubblesSlow, &gBattleAnimParticleDescriptors[0x45], 0, 1, 0x11, gBattleAnimCurrentAnchor, 0, 0);
}

// initialize using aurora beam
void BattleAnim_InitializeAuroraBeamUse(void) {
    BattleAnim_SpawnWhiteHueChange(0, 0, 0x20, 0xA, 0xA0);
    BattleAnim_SpawnGlobalTintColorFadeDetach(0, 8, 6);
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunInitialBeamEmanator, &gBattleAnimParticleDescriptors[0x53], 0, 1, 0x10, gBattleAnimCurrentAnchor, 0x41, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(6, BattleAnim_RunPulsingBeamEmanator, &gBattleAnimParticleDescriptors[0x53], 0x46, 1, 0x10, gBattleAnimCurrentAnchor, 0x41, 1);
}

// initializes using hyper beam
void BattleAnim_InitializeHyperBeamUse(void) {
    BattleAnim_SpawnWhiteHueChange(0, 0, 0x20, 0xA, 0xA0);
    BattleAnim_SpawnOwnerModelTintRampDetach(0, 0xA, 0xFF, 0, 0x10);
    BattleAnim_SpawnGlobalTintColorFadeDetach(0, 0x20, 0x10);
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunInitialBeamEmanator, &gBattleAnimParticleDescriptors[0x53], 0, 1, 0x10, gBattleAnimCurrentAnchor, 0x2C, 0);
    BattleAnim_InitializeHyperBeamUseSequence();
    BattleAnim_CreateEffectSlotForCurrentOwner(6, BattleAnim_RunPulsingBeamEmanator, &gBattleAnimParticleDescriptors[0x53], 0x46, 1, 0x10, gBattleAnimCurrentAnchor, 0x2C, 1);
}

// initializes using solar beam after charging
void BattleAnim_InitializeSolarBeamUse1(void) {
    BattleAnim_SpawnWhiteHueChange(0, 0, 0x20, 0xA, 0xA0);
    BattleAnim_SpawnGlobalTintColorFadeDetach(0, 8, 0x22);
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunInitialBeamEmanator, &gBattleAnimParticleDescriptors[0x53], 0, 1, 0x10, gBattleAnimCurrentAnchor, 0x17, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(6, BattleAnim_RunPulsingBeamEmanator, &gBattleAnimParticleDescriptors[0x53], 0x46, 1, 0x10, gBattleAnimCurrentAnchor, 0x17, 1);
}

// initializes being using tri attack
void BattleAnim_InitializeTriAttackUse(void) {
    BattleAnim_SpawnWhiteHueChange(0, 0, 0x20, 0xA, 0xA0);
    BattleAnim_SpawnGlobalTintColorFadeDetach(0, 8, 0x22);
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunInitialBeamEmanator, &gBattleAnimParticleDescriptors[0x53], 0, 1, 0x10, gBattleAnimCurrentAnchor, 0x17, 0);
}

void InitParticleComboVariant(void) {
    BattleAnim_SpawnBeamBoomWaterfallHueChange(4, 0xA, 0x35, 0xC, 0xFF);
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunExpandingHitSpark, &gBattleAnimParticleDescriptors[4], 0, 1, 0x10, gBattleAnimCurrentAnchor, 0x35, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 5, 3, BattleAnim_RunStrongOutwardMovingLargeHitSpark, &gBattleAnimParticleDescriptors[0x19], 0, 8, 0x10, gBattleAnimCurrentAnchor, 0x26, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0xA, 1, 3, BattleAnim_RunStrongOutwardMovingSmallHitSpark, &gBattleAnimParticleDescriptors[0x19], 0, 8, 0x11, gBattleAnimCurrentAnchor, 0x26, 0);
}

// sets being hit by ice beam
void BattleAnim_InitializeIceBeamHit(void) {
    BattleAnim_SpawnIceBeamBlizzardHitHueChange(0, 0xA, 0x35, 0x10, 0xDC, 0x20);
    BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunExpandingHitSpark, &gBattleAnimParticleDescriptors[4], 0, 1, 0x10, gBattleAnimCurrentAnchor, 0x35, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 5, 3, BattleAnim_RunStrongOutwardMovingLargeHitSpark, &gBattleAnimParticleDescriptors[0x19], 0, 8, 0x10, gBattleAnimCurrentAnchor, 0x26, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(4, 4, 2, BattleAnim_RunStrongBeamHitEffect, &gBattleAnimParticleDescriptors[0x54], 0, 1, 0x10, gBattleAnimCurrentAnchor, 0x33, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0xC, 1, 3, BattleAnim_RunStrongOutwardMovingSmallHitSpark, &gBattleAnimParticleDescriptors[0x19], 0, 8, 0x11, gBattleAnimCurrentAnchor, 0x26, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0xE, 4, 2, BattleAnim_RunIceBeamIcePunchEffect, &gBattleAnimParticleDescriptors[0x3D], 0, 6, 0x10, 0x64, 0, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 2, 6, BattleAnim_RunIceBeamHitEffect, &gBattleAnimParticleDescriptors[0x42], 0, 8, 0x11, 9, 0x18, 0xB);
    BattleAnim_SpawnGlobalTintColorFadeDetach(0x30, 0x20, 0xA);
}

// initializes being hit by psybeam
void BattleAnim_InitializePsybeamHit(void) {
    BattleAnim_SpawnRainbowHueChange(4, 0x32, 0x20, 0xA0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(4, 4, 3, BattleAnim_RunPsybeamHitSparkFlicker, &gBattleAnimParticleDescriptors[0x54], 0, 1, 0x10, gBattleAnimCurrentAnchor, 0, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunPsybeamHitSparkDrift, &gBattleAnimParticleDescriptors[0x1B], 0, 1, 0x10, gBattleAnimCurrentAnchor, 0x23, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(3, 5, 3, BattleAnim_RunWeakBeamHitEffect, &gBattleAnimParticleDescriptors[0x19], 0, 8, 0x10, gBattleAnimCurrentAnchor, 1, 0);
    BattleAnim_SpawnGlobalTintColorFadeDetach(0x30, 0x20, 0xA);
}

// initializes being hit by bubble beam 
void BattleAnim_InitializeBubbleBeamHit(void) {
    BattleAnim_SpawnBeamBoomWaterfallHueChange(4, 0xA, 0x23, 0x10, 0xFF);
    BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunExpandingHitSpark, &gBattleAnimParticleDescriptors[4], 0, 1, 0x10, gBattleAnimCurrentAnchor, 0x35, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(3, 5, 3, BattleAnim_RunWeakBeamHitEffect, &gBattleAnimParticleDescriptors[0x19], 0, 8, 0x10, gBattleAnimCurrentAnchor, 0, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunBubbleBubbleBeamSharedEffect, &gBattleAnimParticleDescriptors[0x45], 0, 0x10, 0x11, 9, 0, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(8, BattleAnim_RunBubbleBubbleBeamSharedEffect, &gBattleAnimParticleDescriptors[0x45], 0, 0x10, 0x11, 9, 0, 0);
    BattleAnim_SpawnGlobalTintColorFadeDetach(0x30, 0x20, 0xA);
}

// initialize being hit by aurora beam
void BattleAnim_InitializeAuroraBeamHit(void) {
    BattleAnim_SpawnRainbowHueChange(4, 0x32, 0x20, 0xA0);
    BattleAnim_CreateEffectSlotForCurrentOwner(4, BattleAnim_RunAuroraBeamHitSpark, &gBattleAnimParticleDescriptors[0x54], 0, 9, 0x10, gBattleAnimCurrentAnchor, 0, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(3, 5, 3, BattleAnim_RunWeakBeamHitEffect, &gBattleAnimParticleDescriptors[0x19], 0, 8, 0x10, gBattleAnimCurrentAnchor, 0, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0xC, 1, 3, BattleAnim_RunStrongOutwardMovingSmallHitSpark, &gBattleAnimParticleDescriptors[0x19], 0, 8, 0x11, gBattleAnimCurrentAnchor, 0x39, 0);
    BattleAnim_SpawnGlobalTintColorFadeDetach(0x16, 0x20, 0xA);
}

// initializes being hit by hyper beam
void BattleAnim_InitializeHyperBeamHit(void) {
    UNUSED s32 pad;

    BattleAnim_SpawnOwnerModelTintRampDetach(0, 0x19, 0xFF, 0, 8);
    BattleAnim_SpawnBeamBoomWaterfallHueChange(4, 0x19, 0x19, 0x10, 0xFF);
    BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunExpandingHitSpark, &gBattleAnimParticleDescriptors[4], 0, 1, 0x10, gBattleAnimCurrentAnchor, 0x19, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunStrongBeamHitEffect, &gBattleAnimParticleDescriptors[0x34], 0, 1, 0x10, gBattleAnimCurrentAnchor, 0x2C, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(4, 8, 1, BattleAnim_RunStrongBeamHitEffect, &gBattleAnimParticleDescriptors[0x54], 0, 1, 0x10, gBattleAnimCurrentAnchor, 0x2C, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(3, 5, 2, BattleAnim_RunStrongOutwardMovingLargeHitSpark, &gBattleAnimParticleDescriptors[0x19], 0, 8, 0x10, gBattleAnimCurrentAnchor, 0xF, 0);
    BattleAnim_InitializeHyperBeamHitSequence();
}

// initializes being hit by solar beam
void BattleAnim_InitializeSolarBeamHit(void) {
    UNUSED s32 pad;

    BattleAnim_SpawnBeamBoomWaterfallHueChange(4, 0xA, 0x19, 0x10, 0xFF);
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunStrongBeamHitEffect, &gBattleAnimParticleDescriptors[0x34], 0, 1, 0x10, gBattleAnimCurrentAnchor, 0x17, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(4, 4, 1, BattleAnim_RunStrongBeamHitEffect, &gBattleAnimParticleDescriptors[0x54], 0, 1, 0x10, gBattleAnimCurrentAnchor, 0x17, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunExpandingHitSpark, &gBattleAnimParticleDescriptors[4], 0, 1, 0x10, gBattleAnimCurrentAnchor, 0x1A, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(3, 5, 3, BattleAnim_RunStrongOutwardMovingLargeHitSpark, &gBattleAnimParticleDescriptors[0x19], 0, 8, 0x10, gBattleAnimCurrentAnchor, 0x17, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0xC, 1, 3, BattleAnim_RunStrongOutwardMovingSmallHitSpark, &gBattleAnimParticleDescriptors[0x19], 0, 8, 0x11, gBattleAnimCurrentAnchor, 0x17, 0);
    BattleAnim_SpawnGlobalTintColorFadeDetach(0x16, 0x20, 0xA);
}

// initializes being hit by sky attack
void BattleAnim_InitializeSkyAttackHitSequence(void) {
    UNUSED s32 pad;

    BattleAnim_SpawnBeamBoomWaterfallHueChange(4, 0xA, 0x19, 0x10, 0xFF);
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunStrongBeamHitEffect, &gBattleAnimParticleDescriptors[0x34], 0, 1, 0x10, gBattleAnimCurrentAnchor, 0x29, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(4, 4, 1, BattleAnim_RunStrongBeamHitEffect, &gBattleAnimParticleDescriptors[0x54], 0, 1, 0x10, gBattleAnimCurrentAnchor, 0x29, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunExpandingHitSpark, &gBattleAnimParticleDescriptors[4], 0, 1, 0x10, gBattleAnimCurrentAnchor, 0x1A, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(3, 5, 3, BattleAnim_RunStrongOutwardMovingLargeHitSpark, &gBattleAnimParticleDescriptors[0x19], 0, 8, 0x10, gBattleAnimCurrentAnchor, 0x29, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0xC, 1, 3, BattleAnim_RunStrongOutwardMovingSmallHitSpark, &gBattleAnimParticleDescriptors[0x19], 0, 8, 0x11, gBattleAnimCurrentAnchor, 0x29, 0);
    BattleAnim_SpawnGlobalTintColorFadeDetach(0x16, 0x20, 0xA);
}

// initializes being hit by tri attack
void BattleAnim_InitializeTriAttackHit(void) {
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 2, 8, BattleAnim_RunTriAttackHitSpark, &gBattleAnimParticleDescriptors[0x19], 0, 1, 0x11, gBattleAnimCurrentAnchor, 0x49, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 2, 8, BattleAnim_RunTriAttackHitSpark, &gBattleAnimParticleDescriptors[0x19], 0, 1, 0x11, gBattleAnimCurrentAnchor, 0x4A, 1);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 2, 8, BattleAnim_RunTriAttackHitSpark, &gBattleAnimParticleDescriptors[0x19], 0, 1, 0x11, gBattleAnimCurrentAnchor, 0x4B, 2);
}
