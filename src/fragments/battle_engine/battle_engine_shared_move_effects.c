#include "battle_engine.h"
#include "src/model_animation.h"
#include "src/geo_render.h"
#include "src/gfx_buffer.h"
#include "src/matrix.h"
#include "src/math_util.h"
#include "pokemon.h"
#include "moves.h"

void BattleAnim_SetupSlashingMoveUse(Particle* arg0) {
    s16 sp26 = Particle_FieldAC_GetS16_0C038(arg0);
    s16 sp24 = Particle_FieldCF_GetS16_0C058(arg0);

    switch (Particle_FieldCE_GetS32_0C040(arg0)) {
        case 0:
            Particle_Rotation_AddX(arg0, 0x4000);
            break;

        case 1:
            Particle_Rotation_AddX(arg0, -0x3000);
            Particle_Rotation_AddY(arg0, 0x4000);
            Particle_Rotation_AddZ(arg0, 0x2000);
            break;

        case 2:
            Particle_Rotation_AddX(arg0, 0x2000);
            Particle_Rotation_AddY(arg0, 0x2000);
            Particle_Rotation_AddZ(arg0, 0x2000);
            break;

        case 3:
            Particle_Rotation_AddX(arg0, -0x8000);
            Particle_Rotation_AddY(arg0, 0x2000);
            Particle_Rotation_AddZ(arg0, 0x6000);
            break;

        case 4:
            Particle_Rotation_AddX(arg0, -0x4000);
            Particle_Rotation_AddY(arg0, 0x4000);
            Particle_Rotation_AddZ(arg0, 0x4000);
            break;
    }

    Particle_PositionOffset_AddYScaled(arg0, 10.0f);
    Particle_AddPositionFromRotationTemplate1(arg0, (sp26 - 1) * 10.0f);
    Particle_Scale_SetScaled(arg0, 1.0f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, sp24);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Moves: Cut (use), Fury Swipes (use), Scratch (use), Slash (use), Vine Whip (use)
    TODO: Human contributor add screenshot
*/
void BattleAnim_RunSlashingMoveUse(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupSlashingMoveUse(arg0);
            break;

        case 2:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 1, 0);
            break;

        case 3:
            Particle_PrimAlpha_ApproachAndStepState_0BC5C(arg0, 0, 0x40);
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }
    Particle_Scale_Approach(arg0, 3.5f, 0.25f);
}

void BattleAnim_SetupRazorWindHitEffect(Particle* arg0) {
    s16 sp26 = Particle_FieldAC_GetS16_0C038(arg0);

    Particle_FieldCF_GetS16_0C058(arg0);

    switch (sp26) {
        case 0:
            Particle_Rotation_AddX(arg0, 0x5800);
            Particle_Rotation_AddY(arg0, 0x5800);
            Particle_Rotation_AddZ(arg0, -0x6000);
            break;

        case 1:
            Particle_Rotation_AddX(arg0, 0x6000);
            Particle_Rotation_AddY(arg0, 0);
            Particle_Rotation_AddZ(arg0, 0x5800);
            break;

        case 2:
            Particle_Rotation_AddX(arg0, 0x2000);
            Particle_Rotation_AddY(arg0, 0x2000);
            Particle_Rotation_AddZ(arg0, 0x2000);
            break;

        case 3:
            Particle_Rotation_AddX(arg0, 0x6000);
            break;

        case 4:
            Particle_Rotation_AddX(arg0, 0);
            Particle_Rotation_AddY(arg0, 0);
            Particle_Rotation_AddZ(arg0, 0);
            break;

        case 5:
            Particle_Rotation_AddX(arg0, 0x6000);
            Particle_Rotation_AddY(arg0, 0x2800);
            Particle_Rotation_AddZ(arg0, 0x5800);
            break;
    }

    Particle_PositionOffset_AddYScaled(arg0, 10.0f);
    Particle_Scale_SetScaled(arg0, 1.0f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0xC);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp26 * 3);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_RunRazorWindHitEffect(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupRazorWindHitEffect(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 1, 0);
            Particle_Scale_Approach(arg0, 3.5f, 0.25f);
            break;

        case 4:
            Particle_PrimAlpha_ApproachAndStepState_0BC5C(arg0, 0, 0x40);
            Particle_Scale_Approach(arg0, 3.5f, 0.25f);
            break;

        case 5:
            Particle_MarkInactive(arg0);
            break;
    }
}

void BattleAnim_SetupPetalDanceRazorLeafHitEffect(Particle* arg0) {
    s16 sp26 = Particle_FieldAC_GetS16_0C038(arg0);
    s16 sp24 = Particle_FieldCF_GetS16_0C058(arg0);

    switch (sp26) {
        case 0:
            Particle_Rotation_AddX(arg0, 0x5800);
            Particle_Rotation_AddY(arg0, 0x5800);
            Particle_Rotation_AddZ(arg0, -0x6000);
            break;

        case 1:
            Particle_Rotation_AddX(arg0, 0x6000);
            Particle_Rotation_AddY(arg0, 0);
            Particle_Rotation_AddZ(arg0, 0x5800);
            break;

        case 2:
            Particle_Rotation_AddX(arg0, 0x2000);
            Particle_Rotation_AddY(arg0, 0x2000);
            Particle_Rotation_AddZ(arg0, 0x2000);
            break;

        case 3:
            Particle_Rotation_AddX(arg0, 0x6000);
            break;

        case 4:
            Particle_Rotation_AddX(arg0, 0);
            Particle_Rotation_AddY(arg0, 0);
            Particle_Rotation_AddZ(arg0, 0);
            break;

        case 5:
            Particle_Rotation_AddX(arg0, 0x6000);
            Particle_Rotation_AddY(arg0, 0x2800);
            Particle_Rotation_AddZ(arg0, 0x5800);
            break;
    }

    switch (sp24) {
        case 0:
            Particle_SetRandomPositionOffsetXSymmetric(arg0, 0xA, 3);
            Particle_SetRandomPositionOffsetYOffset(arg0, 0xF, 0xA);
            Particle_SetRandomPositionOffsetZSymmetric(arg0, 0xF, 3);
            ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0xD);
            break;

        case 1:
            Particle_SetRandomPositionOffsetXSymmetric(arg0, 0xA, 5);
            Particle_SetRandomPositionOffsetYOffset(arg0, 0x14, 5);
            Particle_SetRandomPositionOffsetZSymmetric(arg0, 0xA, 5);
            ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0xE);
            break;
    }

    Particle_Scale_SetScaled(arg0, 1.0f);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp26 * 2);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_RunPetalDanceRazorLeafHitEffect(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupPetalDanceRazorLeafHitEffect(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 2, 1, 0);
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }
}

void BattleAnim_SetupSlashingMoveFullScreenFlash(Particle* arg0) {
    s16 sp26 = BattleAnim_GetOwnerSpecies(gBattleAnimSecondaryOwner);

    Particle_Scale_SetScaled(arg0, 1.0f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, Particle_FieldCF_GetS16_0C058(arg0));

    if (sp26 == MACHOP) {
        Particle_SetPosition(arg0, 320.0f, 0.0f, 0.0f);
        Particle_Rotation_AddZ(arg0, -0x2800);
    } else {
        Particle_Rotation_AddZ(arg0, 0x2500);
    }
    Particle_AdvanceLifecycle(arg0);
}

/*
    Moves: Cut (use), Fury Swipes (use), Scratch (use), Slash (use), Vine Whip (use)
    TODO: Human contributor add screenshot
*/
void BattleAnim_RunSlashingMoveFullScreenFlash(Particle* arg0) {
    s16 sp26 = Particle_FieldCE_GetS32_0C040(arg0);

    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupSlashingMoveFullScreenFlash(arg0);
            break;

        case 2:
            Particle_TickDelayAndAdvanceLifecycle(arg0, 0x14);
            break;

        case 3:
            Particle_DisableAndDetach(arg0);
            break;
    }

    Particle_Scale_Approach(arg0, 6.2f, 1.0f);

    if (sp26 == 0) {
        Particle_PrimG_Approach_0BB14(arg0, 0, 0x18);
        Particle_PrimB_Approach_0BB44(arg0, 0, 0x18);
        Particle_ApproachEnvGreen(arg0, 0, 0x18);
        Particle_ApproachEnvBlue(arg0, 0, 0x18);
    }
}

void BattleAnim_SetupSlashMoveWhiteFlashOverlay(Particle* arg0) {
    s16 sp26 = BattleAnim_GetOwnerSpecies(gBattleAnimSecondaryOwner);

    Particle_Scale_SetScaled(arg0, 1.0f);

    if (sp26 == 0x42) {
        Particle_Rotation_AddZ(arg0, 0x2500);
    } else {
        Particle_SetPosition(arg0, 320.0f, 0.0f, 0.0f);
        Particle_Rotation_AddZ(arg0, -0x2800);
    }

    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_AdvanceLifecycle(arg0);
}

/*
    Moves: Crabhammer (hit), Cut (hit), Fury Swipes (hit), Scratch (hit), Slash (hit), Vine Whip (hit)
    TODO: Human contributor add screenshot
*/
void BattleAnim_RunSlashMoveWhiteFlashOverlay(Particle* arg0) {
    s16 sp26 = Particle_FieldCE_GetS32_0C040(arg0);

    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupSlashMoveWhiteFlashOverlay(arg0);
            break;

        case 2:
            Particle_TickDelayAndAdvanceLifecycle(arg0, 0x14);
            break;

        case 3:
            Particle_MarkInactive(arg0);
            break;
    }

    Particle_Scale_Approach(arg0, 6.0f, 1.0f);

    if (sp26 == 0) {
        Particle_PrimG_Approach_0BB14(arg0, 0, 0x18);
        Particle_PrimB_Approach_0BB44(arg0, 0, 0x18);
        Particle_ApproachEnvGreen(arg0, 0, 0x18);
        Particle_ApproachEnvBlue(arg0, 0, 0x18);
    }
    Particle_PrimAlpha_Approach_0BC2C(arg0, 0, 0x10);
}

void BattleAnim_SetupSlashingMoveHitSpark(Particle* arg0) {
    s16 sp26 = Particle_FieldCE_GetS32_0C040(arg0);
    s16 sp24 = Particle_FieldCF_GetS16_0C058(arg0);
    s16 sp22 = BattleAnim_GetOwnerSpecies(gBattleAnimSecondaryOwner);

    Particle_Scale_SetScaled(arg0, 1.0f);

    if (sp22 == MACHOP) {
        Particle_Rotation_AddX(arg0, -0x2000);
        Particle_Rotation_AddY(arg0, 0x4000);
    } else {
        Particle_Rotation_AddX(arg0, 0x2000);
        Particle_Rotation_AddY(arg0, 0x4000);
    }

    switch (sp26) {
        case 0:
            Particle_PositionOffset_AddYScaled(arg0, -10.0f);
            break;

        case 2:
            Particle_PositionOffset_AddYScaled(arg0, 10.0f);
            break;
    }

    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, sp24);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Moves: Crabhammer (hit), Cut (hit), Scratch (hit), Slash (hit), Vine Whip (hit)
    TODO: Human contributor add screenshot
*/
void BattleAnim_RunSlashingMoveHitSpark(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupSlashingMoveHitSpark(arg0);
            break;

        case 2:
            Particle_TickDelayAndAdvanceLifecycle(arg0, 8);
            break;

        case 3:
            Particle_PrimAlpha_ApproachAndStepState_0BC5C(arg0, 0, 0x20);
            break;

        case 4:
            Particle_DisableAndDetach(arg0);
            break;
    }

    Particle_AnimateFramesForward(arg0, 0, 7, 1, 2, 0);
    Particle_Scale_Approach(arg0, 5.0f, 0.4f);
}

void BattleAnim_SetupFurySwipesHitEffect(Particle* arg0) {
    s16 sp26 = Particle_FieldCE_GetS32_0C040(arg0);
    s16 sp24 = Particle_FieldCF_GetS16_0C058(arg0);
    s16 sp22 = BattleAnim_GetOwnerSpecies(gBattleAnimSecondaryOwner);

    Particle_Scale_SetScaled(arg0, 1.0f);

    if (sp22 == 0x42) {
        Particle_Rotation_AddX(arg0, -0x2000);
        Particle_Rotation_AddY(arg0, 0x4000);
    } else {
        Particle_Rotation_AddX(arg0, 0x2000);
        Particle_Rotation_AddY(arg0, 0x4000);
    }

    switch (sp26) {
        case 0:
            Particle_PositionOffset_AddYScaled(arg0, -10.0f);
            break;

        case 2:
            Particle_PositionOffset_AddYScaled(arg0, 10.0f);
            break;
    }

    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, sp24);
    Particle_SetPrimAlpha(arg0, 0x80);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_RunFurySwipesHitEffect(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupFurySwipesHitEffect(arg0);
            break;

        case 2:
            Particle_TickDelayAndAdvanceLifecycle(arg0, 8);
            break;

        case 3:
            Particle_PrimAlpha_ApproachAndStepState_0BC5C(arg0, 0, 0x10);
            break;

        case 4:
            Particle_DisableAndDetach(arg0);
            break;
    }

    Particle_AnimateFramesForward(arg0, 0, 7, 1, 2, 0);
    Particle_Scale_Approach(arg0, 5.0f, 0.4f);
}

void BattleAnim_CallbackEndEffects39And41And45And46And83Descriptor3DMetadataBurstChild_InitParticle(Particle* arg0) {
    s16 sp26 = Particle_FieldAC_GetS16_0C038(arg0);
    s16 sp24 = Particle_FieldCF_GetS16_0C058(arg0);

    Particle_Scale_SetScaled(arg0, 1.0f);
    Particle_StepMagnitude_SetScaled(arg0, 0.4f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, sp24);
    Particle_SetPrimAlpha(arg0, 0x64);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp26 * 4);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackEndEffects39And41And45And46And83Descriptor3DMetadataBurstChild(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_CallbackEndEffects39And41And45And46And83Descriptor3DMetadataBurstChild_InitParticle(arg0);
            break;

        case 2:
            if (Particle_TickDelayAndAdvanceLifecycle(arg0, arg0->unk_A6) != 0) {
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 2, 0);
            Particle_PrimAlpha_Approach_0BC2C(arg0, 0x64, 6);
            Particle_AddVelocityXZFromRotationYPlusHalfTurn(arg0);
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }
}

void BattleAnim_SetupSmallSand(Particle* arg0) {
    s16 sp26 = Particle_FieldAC_GetS16_0C038(arg0);
    s16 sp24 = Particle_FieldCF_GetS16_0C058(arg0);

    Particle_Scale_SetScaled(arg0, 0.25f);
    arg0->motionTarget.y = (MathUtil_Random_ZeroOne() * 0.5f) + 0.25f;
    Particle_Rotation_AddY(arg0, 0x1800 - ((0x3000 / Particle_FieldAA_GetS16_0C068(arg0)) * sp26));
    Particle_StepMagnitude_SetScaled(arg0, (2.0f * MathUtil_Random_ZeroOne()) + 3.0f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, sp24);
    Particle_SetPrimAlpha(arg0, 0xB4);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Moves: Sand Attack (use)
    TODO: Human contributor add screenshot
*/
void BattleAnim_RunSmallSand(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupSmallSand(arg0);
            break;

        case 2:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 2, 0);
            Particle_Scale_Approach(arg0, 0.75f, 0.025f);
            Particle_StepMagnitude_Approach(arg0, 0.5f, 0.25f);
            Particle_MotionOffset_AddYScaled(arg0, arg0->motionTarget.y);
            Particle_AddVelocityXZFromRotationYViaImpulse(arg0);
            break;

        case 3:
            Particle_PrimAlpha_ApproachAndStepState_0BC5C(arg0, 0, 0x20);
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }
}

void BattleAnim_SetupLargeSand(Particle* arg0) {
    s16 sp26 = Particle_FieldAC_GetS16_0C038(arg0);
    s16 sp24 = Particle_FieldCF_GetS16_0C058(arg0);

    Particle_Scale_SetScaled(arg0, 0.5f);
    arg0->motionTarget.y = (MathUtil_Random_ZeroOne() * 0.5f) + 0.25f;
    Particle_Rotation_AddY(arg0, 0x1800 - ((0x3000 / Particle_FieldAA_GetS16_0C068(arg0)) * sp26));
    Particle_StepMagnitude_SetScaled(arg0, (MathUtil_Random_ZeroOne() * 2.5f) + 3.0f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, sp24);
    Particle_SetPrimAlpha(arg0, 0xB4);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Moves: Sand Attack (use)
    TODO: Human contributor add screenshot
*/
void BattleAnim_RunLargeSand(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupLargeSand(arg0);
            break;

        case 2:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 3, 0);
            Particle_Scale_Approach(arg0, 1.0f, 0.05f);
            Particle_StepMagnitude_Approach(arg0, 0.5f, 0.25f);
            Particle_MotionOffset_AddYScaled(arg0, arg0->motionTarget.y);
            break;

        case 3:
            if (Particle_PrimAlpha_Approach_0BC2C(arg0, 0, 0x20) != 0) {
                Particle_AdvanceLifecycle(arg0);
            }
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }
    Particle_AddVelocityXZFromRotationYViaImpulse(arg0);
}

void BattleAnim_SetupSandAttackUseEffect(Particle* arg0) {
    UNUSED s16 pad;
    s16 sp24 = Particle_FieldAC_GetS16_0C038(arg0);

    Particle_Scale_SetScaled(arg0, 0.25f);
    arg0->motionTarget.y = (MathUtil_Random_ZeroOne() * 0.5f) + 0.4f;
    Particle_Rotation_AddY(arg0, 0x800 - ((0x1000 / Particle_FieldAA_GetS16_0C068(arg0)) * sp24));
    Particle_StepMagnitude_SetScaled(arg0, (2.0f * MathUtil_Random_ZeroOne()) + 5.0f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0x32);
    Particle_SetPrimAlpha(arg0, 0xFF);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_RunSandAttackUseEffect(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupSandAttackUseEffect(arg0);
            break;

        case 2:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 2, 0);
            Particle_PrimAlpha_Approach_0BC2C(arg0, 0x64, 9);
            Particle_Scale_Approach(arg0, 0.4f, 0.025f);
            Particle_MotionOffset_AddYScaled(arg0, arg0->motionTarget.y);
            Particle_AddVelocityXZFromRotationYViaImpulse(arg0);
            break;

        case 3:
            Particle_MarkInactive(arg0);
            break;
    }
}

void BattleAnim_SetupUnusedRiseImpulseVariant(Particle* arg0) {
    s16 sp26 = Particle_FieldAC_GetS16_0C038(arg0);

    Particle_Scale_SetScaled(arg0, 0.4f);
    Particle_SetRandomPositionOffsetXYZ(arg0, 0, 0, 0xA);
    arg0->motionTarget.y = (MathUtil_Random_ZeroOne() * 0.5f) + 0.25f;
    Particle_Rotation_AddY(arg0, 0x2000 - ((0x4000 / Particle_FieldAA_GetS16_0C068(arg0)) * sp26));
    Particle_StepMagnitude_SetScaled(arg0, MathUtil_Random_ZeroOne() + 1.0f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0xA);
    Particle_SetPrimAlpha(arg0, 0x64);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackUnusedRiseImpulseVariant(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupUnusedRiseImpulseVariant(arg0);
            break;

        case 2:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 2, 0);
            Particle_Scale_Approach(arg0, 0.75f, 0.025f);
            Particle_StepMagnitude_Approach(arg0, 0.5f, 0.2f);
            Particle_MotionOffset_AddYScaled(arg0, arg0->motionTarget.y);
            Particle_AddVelocityXZFromRotationYViaImpulse(arg0);
            break;

        case 3:
            if (Particle_PrimAlpha_Approach_0BC2C(arg0, 0, 0x20) != 0) {
                Particle_AdvanceLifecycle(arg0);
            }
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }
}

void BattleAnim_SetupSandAttackHitEffect(Particle* arg0) {
    s16 sp26 = Particle_FieldAC_GetS16_0C038(arg0);

    Particle_Scale_SetScaled(arg0, 0.3f);
    Particle_SetRandomPositionOffsetXYZ(arg0, 0xA, 5, 0xA);
    Particle_PositionOffset_AddYScaled(arg0, 15.0f);
    Particle_StepMagnitude_SetScaled(arg0, (ParticleMath_RandomRange(0xA) * 0.1f) + 1.5f);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp26 * 2);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0x32);
    Particle_SetPrimAlpha(arg0, 0xFF);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_RunSandAttackHitEffect(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupSandAttackHitEffect(arg0);
            break;

        case 2:
            if (Particle_TickDelayAndAdvanceLifecycle(arg0, arg0->unk_A6) != 0) {
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            Particle_Scale_Approach(arg0, 0.5f, 0.02f);
            Particle_AnimateFramesForward(arg0, 0, 7, 1, 2, 0);
            Particle_PrimAlpha_Approach_0BC2C(arg0, 0x64, 6);
            if (Particle_Field50_AddNegField74Y_CheckField2CYNonPositive_09F84(arg0) != 0) {
                Particle_AdvanceLifecycle(arg0);
            }
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }
}

void BattleAnim_CallbackSharedDescriptors3EAnd3FAnimatedRiseFade_InitParticle(Particle* arg0) {
    s16 sp26 = Particle_FieldAC_GetS16_0C038(arg0);

    Particle_Scale_SetScaled(arg0, 0.25f);
    Particle_Rotation_AddY(arg0, sp26 << 0xC);
    Particle_StepMagnitude_SetScaled(arg0, (2.0f * MathUtil_Random_ZeroOne()) + 6.0f);
    Particle_MotionStep_SetYScaled(arg0, (MathUtil_Random_ZeroOne() * 3.0f) + 1.0f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0xA);
    Particle_SetPrimAlpha(arg0, 0x64);
    Particle_FieldA6_SetS16_0BCA8(arg0, 7);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackSharedDescriptors3EAnd3FAnimatedRiseFade(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_CallbackSharedDescriptors3EAnd3FAnimatedRiseFade_InitParticle(arg0);
            break;

        case 2:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 2, 0);
            Particle_Scale_Approach(arg0, 0.8f, 0.1f);
            Particle_StepMagnitude_Approach(arg0, 1.0, 1.0f);
            if (Particle_FieldA6_DecrementToZeroStep1_0BDAC(arg0) != 0) {
                Particle_MotionStep_ApproachY(arg0, 0.5f, 0.4f);
                Particle_MotionOffset_AddYFromMotionStep(arg0);
            }
            break;

        case 3:
            Particle_MotionOffset_AddYFromMotionStep(arg0);
            Particle_PrimAlpha_ApproachAndStepState_0BC5C(arg0, 0, 0x20);
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRunning(arg0) != 0) {
        Particle_AddVelocityXZFromRotationYViaImpulse(arg0);
    }
}

void BattleAnim_SetupIceBeamIcePunchEffect(Particle* arg0) {
    s16 sp26 = Particle_FieldAC_GetS16_0C038(arg0);

    switch (Particle_FieldCF_GetS16_0C058(arg0)) {
        case 0:
            Particle_Scale_SetScaled(arg0, 0.75f);
            Particle_SetRandomPositionOffsetXYZ(arg0, 0xF, 0xF, 0xF);
            Particle_PositionOffset_AddYScaled(arg0, 15.0f);
            Particle_StepMagnitude_SetScaled(arg0, 1.0f);
            break;

        case 1:
            Particle_Scale_SetScaled(arg0, 0.5f);
            Particle_SetRandomPositionOffsetXYZ(arg0, 3, 3, 3);
            Particle_StepMagnitude_SetScaled(arg0, 0.4f);
            break;
    }

    Particle_FieldA6_SetS16_0BCA8(arg0, sp26 * 4);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0x13);
    Particle_SetPrimAlpha(arg0, 0xC8);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_RunIceBeamIcePunchEffect(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupIceBeamIcePunchEffect(arg0);
            break;

        case 2:
            if (Particle_TickDelayAndAdvanceLifecycle(arg0, arg0->unk_A6) != 0) {
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 2, 0);
            Particle_PrimAlpha_Approach_0BC2C(arg0, 0x64, 6);
            Particle_Field50_AddNegField74Y_CheckField2CYNonPositive_09F84(arg0);
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }
}

void BattleAnim_CallbackOrphanDescriptor34ColorFadeDetach_InitParticle(Particle* arg0) {
    s16 sp26 = Particle_FieldAC_GetS16_0C038(arg0);

    Particle_SetRandomRotatedPositionOffsetXY(arg0, 0xF, 0xF);
    Particle_Scale_SetScaled(arg0, 1.0f);
    Particle_StepMagnitude_SetScaled(arg0, 0.2f);
    Particle_SetPrimColor(arg0, 0xFF, 0xFF, 0xFF);
    Particle_SetEnvColor(arg0, 0xFF, 0x64, 0);
    Particle_SetPrimAlpha(arg0, 0xFF);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp26 * 4);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackOrphanDescriptor34ColorFadeDetach(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_CallbackOrphanDescriptor34ColorFadeDetach_InitParticle(arg0);
            break;

        case 2:
            if (Particle_TickDelayAndAdvanceLifecycle(arg0, arg0->unk_A6) != 0) {
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 2, 0);
            Particle_PrimR_Approach_0BAE4(arg0, 0x64, 9);
            Particle_EnvR_Approach_0BB90(arg0, 0, 0x10);
            Particle_ApproachEnvGreen(arg0, 0x32, 3);
            Particle_ApproachEnvBlue(arg0, 0xFF, 0x10);
            if (Particle_TickDelayTimer(arg0, 8) != 0) {
                Particle_PrimAlpha_Approach_0BC2C(arg0, 0, 0x20);
            }
            Particle_AddVelocityXZFromRotationYPlusHalfTurn(arg0);
            break;

        case 4:
            Particle_DisableAndDetach(arg0);
            break;
    }
}

void BattleAnim_SetupGroundedCircularHitSpark(Particle* arg0) {
    Particle_Scale_SetScaled(arg0, 0.1f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0x14);
    Particle_SetPrimAlpha(arg0, 0xFF);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Moves: Body Slam (hit), Crabhammer (hit), Double-Edge (hit), Fly (hit), Seismic Toss (hit), Slam (hit),
    Stomp (hit), Submission (hit), Thrash (hit)
    TODO: Human contributor add screenshot
*/
void BattleAnim_RunGroundedCircularHitSpark(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupGroundedCircularHitSpark(arg0);
            break;

        case 2:
            Particle_Scale_Approach(arg0, 0.75f, 0.08125f);
            Particle_PrimAlpha_ApproachAndStepState_0BC5C(arg0, 0, 0x20);
            break;

        case 3:
            Particle_DisableAndDetach(arg0);
            break;
    }
}

void BattleAnim_SetupUnusedGlitterVariant(Particle* arg0) {
    Vec3s sp28;

    Particle_FieldAC_GetS16_0C038(arg0);
    Particle_Scale_SetScaled(arg0, 0.1f);
    sp28.x = MathUtil_Random16();
    sp28.y = MathUtil_Random16();
    sp28.z = 0;
    Particle_Rotation_SetVecFromVec3s(arg0, sp28);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0x15);
    Particle_SetPrimAlpha(arg0, 0xFF);
    Particle_FieldA6_SetS16_0BCA8(arg0, ParticleMath_RandomRange(5) + 8);
    Particle_StepMagnitude_SetScaled(arg0, 5.0f);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackUnusedGlitterVariant(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupUnusedGlitterVariant(arg0);
            break;

        case 2:
            Particle_StepMagnitude_Approach(arg0, 2.0f, 0.5f);
            if ((Particle_FieldA6_DecrementToZeroStep1_0BDAC(arg0) != 0) &&
                (Particle_PrimAlpha_Approach_0BC2C(arg0, 0, 0x40) != 0)) {
                Particle_AdvanceLifecycle(arg0);
            }
            break;

        case 3:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRunning(arg0) != 0) {
        Particle_AddMotionOffsetFromRotationTemplate1(arg0);
    }
}

void BattleAnim_SetupPointyMoveHitSparkChild(Particle* arg0) {
    Vec3s sp28;
    s16 sp26 = Particle_FieldCF_GetS16_0C058(arg0);

    Particle_FieldAC_GetS16_0C038(arg0);

    sp28.x = ParticleMath_RandomSigned(0x2000);
    sp28.y = ParticleMath_RandomSignedOffset(0x2000, 0x8000);
    sp28.z = 0;
    Particle_Rotation_AddVecFromVec3s(arg0, sp28);

    switch (sp26) {
        case 0:
            Particle_Scale_SetScaled(arg0, 0.2f);
            ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0x15);
            break;

        case 1:
            Particle_Scale_SetScaled(arg0, 0.2f);
            ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0x1B);
            break;
    }

    Particle_SetPrimAlpha(arg0, 0xFF);
    Particle_FieldA6_SetS16_0BCA8(arg0, ParticleMath_RandomRange(5) + 8);
    Particle_StepMagnitude_SetScaled(arg0, 6.0f);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Moves: Horn Attack (hit), Horn Drill (hit), Peck (hit), Pin Missile (hit), Poison Sting (hit), Spike Cannon
    (hit), Twineedle (hit)
    TODO: Human contributor add screenshot
*/
void BattleAnim_RunPointyMoveHitSparkChild(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupPointyMoveHitSparkChild(arg0);
            break;

        case 2:
            Particle_Scale_Approach(arg0, 0.1f, 0.01f);
            Particle_StepMagnitude_Approach(arg0, 3.0f, 0.75f);
            if ((Particle_FieldA6_DecrementToZeroStep1_0BDAC(arg0) != 0) &&
                (Particle_PrimAlpha_Approach_0BC2C(arg0, 0, 0x40) != 0)) {
                Particle_AdvanceLifecycle(arg0);
            }
            break;

        case 3:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRunning(arg0) != 0) {
        Particle_AddMotionOffsetFromRotationTemplate1(arg0);
    }
}

void BattleAnim_SetupPhysicalHitPurpleSmoke(Particle* arg0) {
    Vec3s sp28;
    s16 sp26 = Particle_FieldCF_GetS16_0C058(arg0);

    Particle_FieldAC_GetS16_0C038(arg0);
    Particle_Scale_SetScaled(arg0, 1.0f);

    switch (sp26) {
        case 0:
            ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 8);
            Particle_SetPrimAlpha(arg0, 0x64);
            Particle_StepMagnitude_SetScaled(arg0, 5.0f);
            sp28.x = ParticleMath_RandomSigned(0x2000);
            sp28.y = ParticleMath_RandomSignedOffset(0x2000, 0x8000);
            sp28.z = 0;
            Particle_Rotation_AddVecFromVec3s(arg0, sp28);
            break;

        case 1:
            ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 8);
            Particle_SetPrimAlpha(arg0, 0x64);
            Particle_StepMagnitude_SetScaled(arg0, 5.0f);
            sp28.x = ParticleMath_RandomSigned(0x2000);
            sp28.y = ParticleMath_RandomSignedOffset(0x2000, 0x8000);
            sp28.z = 0;
            Particle_Rotation_AddVecFromVec3s(arg0, sp28);
            break;

        case 2:
            ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 8);
            Particle_SetPrimAlpha(arg0, 0x64);
            Particle_PositionOffset_SetYScaled(arg0, -10.0f);
            Particle_StepMagnitude_SetScaled(arg0, 5.0f);
            sp28.x = ParticleMath_RandomSigned(0x2000);
            sp28.y = ParticleMath_RandomSignedOffset(0x2000, 0x8000);
            sp28.z = 0;
            Particle_Rotation_AddVecFromVec3s(arg0, sp28);
            break;

        case 3:
            ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 9);
            Particle_SetPrimAlpha(arg0, 0x96);
            Particle_StepMagnitude_SetScaled(arg0, 3.0f);
            sp28.x = ParticleMath_RandomSigned(0x2000);
            sp28.y = ParticleMath_RandomSignedOffset(0x2000, 0x8000);
            sp28.z = 0;
            Particle_Rotation_AddVecFromVec3s(arg0, sp28);
            break;

        case 4:
            ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 5);
            Particle_SetPrimAlpha(arg0, 0x96);
            Particle_StepMagnitude_SetScaled(arg0, 4.0f);
            sp28.x = ParticleMath_RandomSigned(0x3000);
            sp28.y = ParticleMath_RandomSignedOffset(0x3000, 0x8000);
            sp28.z = 0;
            Particle_Rotation_AddVecFromVec3s(arg0, sp28);
            break;
    }

    Particle_FieldA6_SetS16_0BCA8(arg0, ParticleMath_RandomRange(5) + 8);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Moves: Barrage (hit), Bide (hit), Bone Club (hit), Bonemerang (hit), Comet Punch (hit), Counter (hit), Dizzy
    Punch (hit), Double Kick (hit), Double Slap (hit), Drill Peck (hit), Headbutt (hit), Hi Jump Kick (hit),
    Jump Kick (hit), Karate Chop (hit), Low Kick (hit), Mega Kick (hit), Mega Punch (hit), Night Shade (hit),
    Pay Day (hit), Pound (hit), Quick Attack (hit), Rage (hit), Rolling Kick (hit), Skull Bash (hit), Sonic Boom
    (hit), Struggle (hit), Tackle (hit), Take Down (hit), Wing Attack (hit)
    TODO: Human contributor add screenshot
*/
void BattleAnim_RunPhysicalHitPurpleSmoke(Particle* arg0) {
    s16 sp2E = Particle_FieldCF_GetS16_0C058(arg0);

    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupPhysicalHitPurpleSmoke(arg0);
            break;
        case 2:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 2, 0);

            switch (sp2E) {
                case 3:
                    Particle_PrimR_Approach_0BAE4(arg0, 0, 9);
                    Particle_StepMagnitude_Approach(arg0, 1.0f, 0.75f);
                    Particle_PrimAlpha_Approach_0BC2C(arg0, 0x32, 6);
                    break;

                case 4:
                    Particle_StepMagnitude_Approach(arg0, 1.0f, 0.75f);
                    Particle_PositionOffset_AddYScaled(arg0, 0.5f);
                    Particle_PrimAlpha_Approach_0BC2C(arg0, 0x32, 6);
                    break;

                default:
                    Particle_PrimG_Approach_0BB14(arg0, 0x64, 6);
                    Particle_StepMagnitude_Approach(arg0, 2.0f, 0.5f);
                    break;
            }
            break;

        case 3:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRunning(arg0) != 0) {
        Particle_AddMotionOffsetFromRotationTemplate1(arg0);
    }
}

void BattleAnim_SetupSwiftHitEffect1(Particle* arg0) {
    s16 sp2E = Particle_FieldAC_GetS16_0C038(arg0);
    Vec3s sp28;

    Particle_Scale_SetScaled(arg0, 0.1f);
    if (arg0->origin.y < 0.0f) {
        arg0->origin.y = 0.0f;
    }
    sp28.x = ParticleMath_RandomRange(0x10000);
    sp28.y = ParticleMath_RandomRange(0x10000);
    sp28.z = 0;
    Particle_Rotation_SetVecFromVec3s(arg0, sp28);
    Particle_RotationStep_SetZ(arg0, ParticleMath_RandomSignedOffsetBySign(0x400, 0x400));
    Particle_StepMagnitude_SetScaled(arg0, 8.0f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0x17);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp2E * 0);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_RunSwiftHitEffect1(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupSwiftHitEffect1(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            Particle_Scale_ApproachAndStepState(arg0, 0.3f, 0.05f);
            Particle_StepMagnitude_Approach(arg0, 3.0f, 1.0f);
            break;

        case 4:
            Particle_TickDelayAndAdvanceLifecycle(arg0, 2);
            break;

        case 5:
            Particle_Scale_ApproachAndStepState(arg0, 0.05f, 0.05f);
            break;

        case 6:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRenderingEnabled(arg0) != 0) {
        Particle_Rotation_AddZFromRotationStep(arg0);
        Particle_MotionStep_SetVecFromField74AnglesXY(arg0);
        Particle_MotionOffset_AddVecFromMotionStep(arg0);
    }
}

void BattleAnim_SetupSwiftHitEffect2(Particle* arg0) {
    BattleAnim_EmitParticleBurst(arg0, BattleAnim_RunSwiftHitEffect1, &gBattleAnimParticleDescriptors[0x42], 0xC);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_RunSwiftHitEffect2(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupSwiftHitEffect2(arg0);
            break;

        case 2:
            Particle_MarkInactive(arg0);
            break;
    }
}

/*
    Moves: Barrier (use), Harden (use), Ice Beam (hit), Ice Punch (use/hit), Kinesis (use), Leech Seed
    (use/hit), Light Screen (use), Pay Day (hit), Recover (use), Reflect (use), Soft-Boiled (use), Swift (hit),
    Swords Dance (use)
    TODO: Human contributor add screenshot
*/
void BattleAnim_SetupSparkleUse(Particle* arg0) {
    s16 sp2E = Particle_FieldAC_GetS16_0C038(arg0);
    s16 sp2C = Particle_FieldCE_GetS32_0C040(arg0);
    s16 sp2A = Particle_FieldCF_GetS16_0C058(arg0);
    Vec3s sp24;

    sp24.x = ParticleMath_RandomRange(0x10000);
    sp24.y = ParticleMath_RandomRange(0x10000);
    sp24.z = 0;

    Particle_Rotation_SetVecFromVec3s(arg0, sp24);
    Particle_StepMagnitude_SetScaled(arg0, 0.0f);
    Particle_MagnitudeStep_SetScaled(arg0, 0.0f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, sp2A);
    Particle_Scale_SetScaled(arg0, 0.0f);
    Particle_SetScaleParameter20(arg0, 0.3f);
    Particle_SetScaleParameter24(arg0, 0.05f);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp2E * 2);

    switch (sp2C) {
        case 0:
            Particle_AddPositionFromRotationTemplate1(arg0, ParticleMath_RandomRangeOffset(0x1E, 0xA));
            Particle_SetScaleParameter20(arg0, 0.2f);
            break;

        case 1:
            if (arg0->origin.y < 0) {
                arg0->origin.y = 0;
            }
            Particle_AddPositionFromRotationTemplate1(arg0, ParticleMath_RandomRangeOffset(0x28, 0xA));
            Particle_SetScaleParameter20(arg0, 0.3f);
            Particle_SetScaleParameter24(arg0, 0.04f);
            break;

        case 2:
            Particle_SetScaleParameter20(arg0, 0.1f);
            Particle_SetScaleParameter24(arg0, 0.03f);
            Particle_AddPositionFromRotationTemplate1(arg0, ParticleMath_RandomRangeOffset(0xC, 5));
            Particle_FieldA6_SetS16_0BCA8(arg0, sp2E * 1);
            break;

        case 3:
            Particle_StepMagnitude_SetScaled(arg0, 0.2f);
            Particle_Scale_SetScaled(arg0, 0.1f);
            Particle_AddPositionFromRotationTemplate1(arg0, ParticleMath_RandomRangeOffset(0x14, 0x1E));
            Particle_FieldA6_SetS16_0BCA8(arg0, sp2E * 4);
            break;

        case 5:
            Particle_SetRandomPositionOffsetXYZ(arg0, 0, 0xF, 0x19);
            Particle_PositionOffset_AddYScaled(arg0, 15.0f);
            Particle_SetScaleParameter20(arg0, 0.15f);
            Particle_SetScaleParameter24(arg0, 0.02f);
            Particle_StepMagnitude_SetScaled(arg0, 0.1f);
            break;

        case 6:
            Particle_AddPositionFromRotationTemplate1(arg0, ParticleMath_RandomRangeOffset(0x1E, 0x14));
            Particle_StepMagnitude_SetScaled(arg0, 0.3f);
            break;

        case 8:
            Particle_AddPositionFromRotationTemplate1(arg0, ParticleMath_RandomRangeOffset(8, 2));
            Particle_SetScaleParameter20(arg0, 0.1f);
            Particle_SetScaleParameter24(arg0, 0.04f);
            Particle_StepMagnitude_SetScaled(arg0, 0.4f);
            Particle_FieldA6_SetS16_0BCA8(arg0, 0);
            break;

        case 7:
            Particle_AddPositionFromRotationTemplate1(arg0, ParticleMath_RandomRangeOffset(0xA, 2));
            Particle_SetScaleParameter20(arg0, 0.2f);
            Particle_SetScaleParameter24(arg0, 0.025f);
            Particle_StepMagnitude_SetScaled(arg0, 1.0f);
            break;

        case 9:
            Particle_AddPositionFromRotationTemplate1(arg0, ParticleMath_RandomRangeOffset(0x1E, 0x14));
            Particle_SetScaleParameter20(arg0, 0.3f);
            Particle_SetScaleParameter24(arg0, 0.025f);
            Particle_StepMagnitude_SetScaled(arg0, 0.0f);
            Particle_MagnitudeStep_SetScaled(arg0, 0.02f);
            break;

        case 10:
            Particle_AddPositionFromRotationTemplate1(arg0, ParticleMath_RandomRangeOffset(0x19, 5));
            Particle_PositionOffset_AddYScaled(arg0, 10.0f);
            Particle_SetScaleParameter20(arg0, 0.2f);
            break;

        case 11:
            Particle_AddPositionFromRotationTemplate1(arg0, ParticleMath_RandomRangeOffset(5, 5));
            Particle_StepMagnitude_SetScaled(arg0, ParticleMath_RandomRange(5) + 5.0f);
            break;
    }
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Moves: Barrier (use), Harden (use), Ice Punch (use/hit), Kinesis (use), Leech Seed (use/hit), Light Screen
    (use), Pay Day (hit), Recover (use), Reflect (use), Soft-Boiled (use), Swift (hit), Swords Dance (use)
    TODO: Human contributor add screenshot
*/
void BattleAnim_RunSparkleHit(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupSparkleUse(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            if (Particle_ApproachStoredScaleAndAdvanceLifecycle(arg0) != 0) {
                Particle_SetScaleParameter20(arg0, 0.0f);
            }
            break;

        case 4:
            Particle_ApproachStoredScaleAndAdvanceLifecycle(arg0);
            break;

        case 5:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRenderingEnabled(arg0) != 0) {
        Particle_Field74_AdvanceAndAddNegY_CheckField2CYNonPositive_09FD8(arg0);
        if (arg0->worldPosition.y < 0.0f) {
            Particle_MarkInactive(arg0);
        }
    }
}

void BattleAnim_SetupBlizzardHitEffect(Particle* arg0) {
    s16 sp26 = Particle_FieldAC_GetS16_0C038(arg0);
    s16 sp24 = Particle_FieldCF_GetS16_0C058(arg0);

    Particle_Scale_SetScaled(arg0, 0.0f);
    Particle_SetScaleParameter20(arg0, 0.3f);
    Particle_SetScaleParameter24(arg0, 0.05f);
    Particle_SetRandomPositionOffsetXYZ(arg0, 0xA, 0x14, 0x14);
    Particle_PositionOffset_AddXZFromRotationY(arg0, 15.0f);
    Particle_PositionOffset_AddYScaled(arg0, 7.0f);
    Particle_StepMagnitude_SetScaled(arg0, ParticleMath_RandomRange(5) + 8.0f);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp26 * 2);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, sp24);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_RunBlizzardHitEffect(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupBlizzardHitEffect(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            if (Particle_ApproachStoredScaleAndAdvanceLifecycle(arg0) != 0) {
                Particle_SetScaleParameter20(arg0, 0.0f);
            }
            break;

        case 4:
            Particle_ApproachStoredScaleAndAdvanceLifecycle(arg0);
            break;

        case 5:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRenderingEnabled(arg0) != 0) {
        Particle_AddVelocityXZFromRotationYPlusHalfTurn(arg0);
    }
}

void BattleAnim_RunIceBeamHitEffect(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupSparkleUse(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            if (Particle_ApproachStoredScaleAndAdvanceLifecycle(arg0) != 0) {
                Particle_SetScaleParameter20(arg0, 0.0f);
            }
            break;

        case 4:
            Particle_ApproachStoredScaleAndAdvanceLifecycle(arg0);
            break;

        case 5:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRenderingEnabled(arg0) != 0) {
        Particle_AddMotionOffsetFromRotationTemplate1(arg0);
        if (arg0->worldPosition.y < 0.0f) {
            Particle_MarkInactive(arg0);
        }
    }
}

void BattleAnim_SetupThunderPunchHitEffect(Particle* arg0) {
    s16 sp26 = Particle_FieldAC_GetS16_0C038(arg0);

    Particle_Scale_SetScaled(arg0, 0.7f);
    Particle_PositionOffset_SetYScaled(arg0, 10.0f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0x19);
    Particle_SetPrimAlpha(arg0, 0xFF);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp26 * 4);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_RunThunderPunchHitEffect(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupThunderPunchHitEffect(arg0);
            break;

        case 2:
            if (Particle_TickDelayAndAdvanceLifecycle(arg0, arg0->unk_A6) != 0) {
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 5, 1, 1, 2);
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }
}

void BattleAnim_SetupDefaultHitSpark(Particle* arg0) {
    s16 sp26 = Particle_FieldAC_GetS16_0C038(arg0);
    s16 sp24 = Particle_FieldCE_GetS32_0C040(arg0);

    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, Particle_FieldCF_GetS16_0C058(arg0));

    switch (sp24) {
        case 1:
            Particle_StepMagnitude_SetScaled(arg0, 0.2f);
            Particle_Scale_SetScaled(arg0, 1.0f);
            break;

        case 5:
            Particle_StepMagnitude_SetScaled(arg0, 0.2f);
            Particle_Scale_SetScaled(arg0, 1.0f);
            break;

        case 6:
            Particle_StepMagnitude_SetScaled(arg0, 0.4f);
            Particle_Scale_SetScaled(arg0, 1.5f);
            break;

        case 16:
            Particle_StepMagnitude_SetScaled(arg0, 0.4f);
            Particle_Scale_SetScaled(arg0, 2.0f);
            break;

        case 7:
            Particle_StepMagnitude_SetScaled(arg0, 0.4f);
            Particle_Scale_SetScaled(arg0, 1.5f);
            break;

        case 8:
            Particle_StepMagnitude_SetScaled(arg0, 0.4f);
            Particle_Scale_SetScaled(arg0, 1.5f);
            Particle_PositionOffset_AddYScaled(arg0, -10.0f);
            break;

        case 12:
            Particle_Scale_SetScaled(arg0, 0.75f);
            Particle_StepMagnitude_SetScaled(arg0, 0.4f);
            Particle_SetRandomPositionOffsetXYZ(arg0, 8, 0xA, 8);
            break;

        case 13:
            Particle_Scale_SetScaled(arg0, 1.0f);
            Particle_StepMagnitude_SetScaled(arg0, 0.0f);
            Particle_SetRandomPositionOffsetX(arg0, 0x1E);
            Particle_SetRandomPositionOffsetYOffset(arg0, 0x14, 0xA);
            Particle_SetRandomPositionOffsetZ(arg0, 0x1E);
            break;

        case 14:
            Particle_Scale_SetScaled(arg0, 1.0f);
            Particle_StepMagnitude_SetScaled(arg0, 0.0f);
            Particle_SetRandomPositionOffsetX(arg0, 0x14);
            Particle_SetRandomPositionOffsetYOffset(arg0, 0x14, 0xA);
            Particle_SetRandomPositionOffsetZ(arg0, 0xF);
            break;

        case 15:
            Particle_Scale_SetScaled(arg0, 1.5f);
            Particle_StepMagnitude_SetScaled(arg0, 0.3f);
            Particle_SetRandomPositionOffsetXYZ(arg0, 0xA, 0xA, 0xA);
            break;
    }

    Particle_SetPrimAlpha(arg0, 0xFF);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp26 * 4);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Moves: Barrage (hit), Bide (hit), Bone Club (hit), Bonemerang (hit), Bubble (hit), Comet Punch (hit),
    Confuse Ray (use/hit), Confusion (hit), Counter (hit), Dig (hit), Dizzy Punch (hit), Double Kick (hit),
    Double Slap (hit), Drill Peck (hit), Earthquake (hit), Fissure (hit), Flash (use), Fury Attack (hit), Gust
    (hit), Headbutt (hit), Hi Jump Kick (hit), Ice Punch (hit), Jump Kick (hit), Karate Chop (hit), Low Kick
    (hit), Mega Kick (hit), Mega Punch (hit), Night Shade (hit), Pay Day (hit), Pound (hit), Psychic (hit),
    Psywave (hit), Quick Attack (hit), Rage (hit), Rolling Kick (hit), Skull Bash (hit), Sonic Boom (hit),
    Struggle (hit), Tackle (hit), Take Down (hit), Thunder Punch (hit), Wing Attack (hit)
    TODO: Human contributor add screenshot
*/
void BattleAnim_RunDefaultHitSpark(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupDefaultHitSpark(arg0);
            break;

        case 2:
            if (Particle_TickDelayAndAdvanceLifecycle(arg0, arg0->unk_A6) != 0) {
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 3, 1, 1, 0);
            Particle_AddVelocityXZFromRotationYPlusHalfTurn(arg0);
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }
}

void BattleAnim_SetupStrongPurpleHitSmoke(Particle* arg0) {
    s16 sp26 = Particle_FieldAC_GetS16_0C038(arg0);

    switch (Particle_FieldCF_GetS16_0C058(arg0)) {
        case 0:
            ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0);
            Particle_StepMagnitude_SetScaled(arg0, 0.2f);
            Particle_Scale_SetScaled(arg0, 1.0f);
            Particle_FieldA6_SetS16_0BCA8(arg0, sp26 * 2);
            Particle_PositionOffset_AddYScaled(arg0, 15.0f);
            break;

        case 1:
            ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0);
            Particle_StepMagnitude_SetScaled(arg0, 0.2f);
            Particle_Scale_SetScaled(arg0, 1.0f);
            Particle_FieldA6_SetS16_0BCA8(arg0, sp26 * 2);
            break;

        case 2:
            ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0);
            Particle_StepMagnitude_SetScaled(arg0, 0.2f);
            Particle_Scale_SetScaled(arg0, 1.0f);
            Particle_FieldA6_SetS16_0BCA8(arg0, sp26 * 2);
            Particle_PositionOffset_AddYScaled(arg0, -10.0f);
            break;

        case 3:
            ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 1);
            Particle_Scale_SetScaled(arg0, 0.5f);
            Particle_StepMagnitude_SetScaled(arg0, 0.4f);
            Particle_FieldA6_SetS16_0BCA8(arg0, sp26 * 2);
            break;

        case 4:
            ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 2);
            Particle_Scale_SetScaled(arg0, 0.5f);
            Particle_StepMagnitude_SetScaled(arg0, 0.4f);
            Particle_FieldA6_SetS16_0BCA8(arg0, sp26 * 2);
            break;

        case 5:
            ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 3);
            Particle_StepMagnitude_SetScaled(arg0, 0.2f);
            Particle_Scale_SetScaled(arg0, 1.0f);
            Particle_FieldA6_SetS16_0BCA8(arg0, sp26 * 2);
            break;

        case 6:
            ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0);
            Particle_StepMagnitude_SetScaled(arg0, 0.4f);
            Particle_Scale_SetScaled(arg0, 1.5f);
            Particle_FieldA6_SetS16_0BCA8(arg0, sp26 * 2);
            break;

        case 9:
            ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0);
            Particle_StepMagnitude_SetScaled(arg0, 0.2f);
            Particle_Scale_SetScaled(arg0, 1.0f);
            Particle_SetRandomPositionOffsetXYZ(arg0, 0xF, 0xF, 0xF);
            Particle_PositionOffset_AddYScaled(arg0, 10.0f);
            Particle_FieldA6_SetS16_0BCA8(arg0, sp26 * 2);
            break;

        case 10:
            ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0);
            Particle_StepMagnitude_SetScaled(arg0, 0.2f);
            Particle_Scale_SetScaled(arg0, 1.0f);
            Particle_SetRandomPositionOffsetXYZ(arg0, 0x14, 0, 0x14);
            Particle_PositionOffset_SetYScaled(arg0, 0.0f);
            Particle_FieldA6_SetS16_0BCA8(arg0, sp26 * 2);
            break;

        case 11:
            ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0);
            Particle_StepMagnitude_SetScaled(arg0, 0.2f);
            Particle_Scale_SetScaled(arg0, 1.0f);
            Particle_SetRandomPositionOffsetXYZ(arg0, 0xA, 0, 0xA);
            Particle_PositionOffset_SetYScaled(arg0, 30.0f - (sp26 * 5.0f));
            Particle_FieldA6_SetS16_0BCA8(arg0, sp26 * 2);
            break;
    }

    Particle_SetPrimAlpha(arg0, 0xFF);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Moves: Bite (hit), Body Slam (hit), Clamp (hit), Crabhammer (hit), Double-Edge (hit), Fly (hit), Guillotine
    (hit), Hyper Fang (hit), Seismic Toss (hit), Slam (hit), Stomp (hit), Submission (hit), Super Fang (hit),
    Thrash (hit), Vice Grip (hit)
    TODO: Human contributor add screenshot
*/
void BattleAnim_RunStrongPurpleHitSmoke(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupStrongPurpleHitSmoke(arg0);
            break;

        case 2:
            if (Particle_TickDelayAndAdvanceLifecycle(arg0, arg0->unk_A6) != 0) {
                Particle_EnableRendering(arg0);
                Particle_FieldA6_SetS16_0BCA8(arg0, 2);
            }
            break;

        case 3:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 3, 1, 1, 0);
            Particle_AddVelocityXZFromRotationYPlusHalfTurn(arg0);
            if ((Particle_FieldA6_DecrementToZeroStep1_0BDAC != 0) && (arg0->phase == 0)) {
                Particle_AdvancePhase(arg0);
            }
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }

    if (arg0->phase == 1) {
        BattleAnim_EmitParticleBurstWithMetadata(
            arg0, BattleAnim_CallbackEndEffects39And41And45And46And83Descriptor3DMetadataBurstChild,
            &gBattleAnimParticleDescriptors[0x3D], 1, 8, 0);
        Particle_AdvancePhase(arg0);
    }
}

void BattleAnim_SetupPointyMoveHitSpark(Particle* arg0) {
    s16 sp26 = Particle_FieldAC_GetS16_0C038(arg0);

    switch (Particle_FieldCF_GetS16_0C058(arg0)) {
        case 3:
            ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 1);
            Particle_Scale_SetScaled(arg0, 1.5f);
            Particle_StepMagnitude_SetScaled(arg0, 0.4f);
            Particle_FieldA6_SetS16_0BCA8(arg0, sp26 * 2);
            Particle_FieldCF_SetU8_0BEEC(arg0, 0);
            break;

        case 4:
            ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 2);
            Particle_Scale_SetScaled(arg0, 1.5f);
            Particle_StepMagnitude_SetScaled(arg0, 0.4f);
            Particle_FieldA6_SetS16_0BCA8(arg0, sp26 * 2);
            Particle_FieldCF_SetU8_0BEEC(arg0, 1);
            break;
    }

    Particle_SetPrimAlpha(arg0, 0xFF);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Moves: Horn Attack (hit), Horn Drill (hit), Peck (hit), Pin Missile (hit), Poison Sting (hit), Spike Cannon
    (hit), Twineedle (hit)
    TODO: Human contributor add screenshot
*/
s32 BattleAnim_RunPointyMoveHitSpark(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupPointyMoveHitSpark(arg0);
            break;

        case 2:
            if (Particle_TickDelayAndAdvanceLifecycle(arg0, arg0->unk_A6) != 0) {
                Particle_EnableRendering(arg0);
                Particle_FieldA6_SetS16_0BCA8(arg0, 2);
            }
            break;

        case 3:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 3, 1, 1, 0);
            Particle_AddVelocityXZFromRotationYPlusHalfTurn(arg0);
            if ((Particle_FieldA6_DecrementToZeroStep1_0BDAC != NULL) && (arg0->phase == 0)) {
                Particle_AdvancePhase(arg0);
            }
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }

    switch (arg0->phase) {
        case 1:
            BattleAnim_EmitParticleBurst(arg0, BattleAnim_RunPointyMoveHitSparkChild,
                                         &gBattleAnimParticleDescriptors[0x38], 8);
            Particle_AdvancePhase(arg0);
            break;

        case 2:
            if (Particle_TickPhaseTimer(arg0, 1) != 0) {
                BattleAnim_EmitParticleBurst(arg0, BattleAnim_RunPointyMoveHitSparkChild,
                                             &gBattleAnimParticleDescriptors[0x38], 4);
                Particle_AdvancePhase(arg0);
            }
            break;

        case 3:
            if (Particle_TickPhaseTimer(arg0, 1) != 0) {
                BattleAnim_EmitParticleBurst(arg0, BattleAnim_RunPointyMoveHitSparkChild,
                                             &gBattleAnimParticleDescriptors[0x38], 4);
                Particle_AdvancePhase(arg0);
            }
            break;
    }
}

void BattleAnim_SetupFirePunchUseEffect1(Particle* arg0) {
    Particle_FieldAC_GetS16_0C038(arg0);
    Particle_Scale_SetScaled(arg0, 0.05f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 7);
    Particle_SetPrimAlpha(arg0, 0xFF);
    Particle_FieldA6_SetS16_0BCA8(arg0, 0x32);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_RunFirePunchUseEffect1(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupFirePunchUseEffect1(arg0);
            break;

        case 2:
            Particle_AnimateFramesForward(arg0, 0, 5, 1, 1, -1);
            Particle_Scale_Approach(arg0, 0.3f, 0.01f);
            Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0);
            break;

        case 3:
            Particle_MarkInactive(arg0);
            break;
    }
}

void BattleAnim_SetupFirePunchUseEffect2(Particle* arg0) {
    s16 sp2E;
    s16 sp2C;
    Vec3s sp24;

    sp2E = Particle_FieldAC_GetS16_0C038(arg0);
    sp2C = sp2E % 3;
    Particle_Scale_SetScaled(arg0, 0.4f);
    Particle_StepMagnitude_SetScaled(arg0, 1.0f);

    sp24.x = MathUtil_Random16();
    sp24.y = MathUtil_Random16();
    sp24.z = 0;
    Particle_Rotation_SetVecFromVec3s(arg0, sp24);

    switch (sp2C) {
        case 0:
            Particle_SetPosition(arg0, 0.0f, -2.0f, 0.0f);
            break;

        case 1:
            Particle_SetPosition(arg0, 0.0f, 2.0f, 2.0f);
            break;

        case 2:
            Particle_SetPosition(arg0, 0.0f, 2.0f, -2.0f);
            break;
    }

    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0x17);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp2E * 4);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_RunFirePunchUseEffect2(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupFirePunchUseEffect2(arg0);
            break;

        case 2:
            if (Particle_TickDelayAndAdvanceLifecycle(arg0, arg0->unk_A6) != 0) {
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 1, 0);
            Particle_StepMagnitude_Approach(arg0, 0.2f, 0.25f);
            Particle_Scale_Approach(arg0, 0.5f, 0.02f);
            Particle_PrimB_Approach_0BB44(arg0, 0, 0x20);
            Particle_ApproachEnvGreen(arg0, 0, 0x20);
            if (arg0->textureFrame >= 6) {
                Particle_PrimG_Approach_0BB14(arg0, 0, 0x20);
                Particle_PrimAlpha_Approach_0BC2C(arg0, 0x64, 0x4E);
            }
            Particle_AddVelocityXZFromRotationYViaImpulse(arg0);
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }
}

void BattleAnim_SetupThunderPunchUseEffect(Particle* arg0) {
    s16 sp2E = Particle_FieldAC_GetS16_0C038(arg0);
    s16 sp2C = sp2E % 3;
    Vec3s sp24;

    Particle_Scale_SetScaled(arg0, 0.4f);
    Particle_StepMagnitude_SetScaled(arg0, 1.0f);

    sp24.x = MathUtil_Random16();
    sp24.y = MathUtil_Random16();
    sp24.z = 0;
    Particle_Rotation_SetVecFromVec3s(arg0, sp24);

    switch (sp2C) {
        case 0:
            Particle_SetPosition(arg0, 0.0f, -2.0f, 0.0f);
            break;

        case 1:
            Particle_SetPosition(arg0, 0.0f, 2.0f, 2.0f);
            break;

        case 2:
            Particle_SetPosition(arg0, 0.0f, 2.0f, -2.0f);
            break;
    }

    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, 0x17);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp2E * 4);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_RunThunderPunchUseEffect(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupThunderPunchUseEffect(arg0);
            break;

        case 2:
            if (Particle_TickDelayAndAdvanceLifecycle(arg0, arg0->unk_A6) != 0) {
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 1, 0);
            Particle_StepMagnitude_Approach(arg0, 0.2f, 0.25f);
            Particle_Scale_Approach(arg0, 0.5f, 0.02f);
            Particle_PrimB_Approach_0BB44(arg0, 0, 0x20);
            Particle_ApproachEnvGreen(arg0, 0, 0x20);
            if (arg0->textureFrame >= 6) {
                Particle_PrimG_Approach_0BB14(arg0, 0, 0x20);
                Particle_PrimAlpha_Approach_0BC2C(arg0, 0x64, 0x4E);
            }
            Particle_AddVelocityXZFromRotationYViaImpulse(arg0);
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }
}

void BattleAnim_RecolorParticleByQueuedMoveId(Particle* arg0) {
    s16 sp1E;

    switch (BattleAnim_GetQueuedMoveId()) {
        case POISONPOWDER:
            sp1E = 0x43;
            break;
        case STUN_SPORE:
            sp1E = 5;
            break;
        case SLEEP_POWDER:
            sp1E = 0xC;
            break;
        case SPORE:
            sp1E = 5;
            break;
        case SAND_ATTACK:
            sp1E = 0x31;
            break;
        case SMOKESCREEN:
            sp1E = 0x1C;
            break;
        case SMOG:
            sp1E = 0x1E;
            break;
        case MIST:
            sp1E = 0x1D;
            break;
        case HAZE:
            sp1E = 0x1E;
            break;
        case POISON_GAS:
            sp1E = 0x1E;
            break;
    }
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, sp1E);
}

void BattleAnim_CallbackStartEffects28And53And65And76Descriptor47AnimatedScaleMotion_InitParticle(Particle* arg0) {
    Vec3s sp28;
    s16 sp26 = Particle_FieldAC_GetS16_0C038(arg0);
    s16 sp24 = Particle_FieldCF_GetS16_0C058(arg0);

    Particle_Scale_SetScaled(arg0, 1.0f);
    sp28.x = MathUtil_Random16();
    sp28.y = MathUtil_Random16();
    sp28.z = 0;
    Particle_Rotation_SetVecFromVec3s(arg0, sp28);

    switch (sp24) {
        case 0:
            Particle_SetRandomPositionOffsetXYZ(arg0, 0xF, 0xF, 0xF);
            Particle_PositionOffset_AddYScaled(arg0, 10.0f);
            Particle_StepMagnitude_SetScaled(arg0, 1.0f);
            Particle_FieldA6_SetS16_0BCA8(arg0, sp26 * 2);
            break;

        case 1:
            Particle_SetRandomPositionOffsetXYZ(arg0, 0xA, 0xA, 0xA);
            Particle_PositionOffset_AddYScaled(arg0, 10.0f);
            Particle_StepMagnitude_SetScaled(arg0, 0.0f);
            Particle_FieldA6_SetS16_0BCA8(arg0, sp26 * 2);
            break;
    }

    BattleAnim_RecolorParticleByQueuedMoveId(arg0);
    Particle_SetPrimAlpha(arg0, 0xC8);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackStartEffects28And53And65And76Descriptor47AnimatedScaleMotion(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_CallbackStartEffects28And53And65And76Descriptor47AnimatedScaleMotion_InitParticle(arg0);
            break;

        case 2:
            if (Particle_TickDelayAndAdvanceLifecycle(arg0, arg0->unk_A6) != 0) {
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 2, 0);
            Particle_PrimAlpha_Approach_0BC2C(arg0, 0x64, 6);
            Particle_Scale_Approach(arg0, 2.0f, 0.075f);
            Particle_AddMotionOffsetFromRotationTemplate1(arg0);
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }
}

void BattleAnim_SetupAcidArmorUseEffect1(Particle* arg0) {
    Particle_Scale_SetScaled(arg0, 0.2f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_SetPrimAlpha(arg0, 0xC8);
    Particle_FieldA6_SetS16_0BCA8(arg0, 0x18);
    arg0->stepMagnitude = 0.02f;
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_RunAcidArmorUseEffect1(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupAcidArmorUseEffect1(arg0);
            break;

        case 2:
            Particle_Scale_Approach(arg0, 2.0f, 0.02f);
            Particle_ApproachEnvGreen(arg0, 0xC8, 2);
            Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0);
            break;

        case 3:
            ParticleMath_ApproachFloat(&arg0->stepMagnitude, 0.008f, 0.001f);
            Particle_Scale_Approach(arg0, 2.0f, arg0->stepMagnitude);
            Particle_ApproachEnvGreen(arg0, 0xC8, 2);
            Particle_PrimAlpha_ApproachAndStepState_0BC5C(arg0, 0, 0xC);
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }
}

void BattleAnim_SetupAcidArmorUseEffect2(Particle* arg0) {
    s16 sp26 = Particle_FieldAC_GetS16_0C038(arg0);

    Particle_Scale_SetScaled(arg0, 1.0f);
    Particle_SetRandomPositionOffsetXYZ(arg0, 0xF, 0xF, 0xF);
    Particle_StepMagnitude_SetScaled(arg0, (ParticleMath_RandomRange(0x32) * 0.02f) + 0.5f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_SetPrimAlpha(arg0, 0x64);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp26 * 4);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_RunAcidArmorUseEffect2(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupAcidArmorUseEffect2(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_FieldA6_SetS16_0BCA8(arg0, ParticleMath_RandomRange(4) + 4);
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0);
            Particle_AddVerticalMotionScalar(arg0);
            break;

        case 4:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 2, 0);
            Particle_PrimAlpha_Approach_0BC2C(arg0, 0x40, 4);
            Particle_AddVerticalMotionScalar(arg0);
            break;

        case 5:
            Particle_MarkInactive(arg0);
            break;
    }
}

void BattleAnim_InitSharedVerticalFadeParticle_ApplySpeciesOffset(Particle* arg0) {
    f32 sp24 = 0.0f;
    f32 sp20 = 0.0f;
    s32 sp1C = 0;

    switch (BattleAnim_GetOwnerSpecies(arg0->pokemon)) {
        case 0x9:
            sp20 = 5.0f;
            break;

        case 0x19:
            sp20 = 0.0f;
            Particle_Scale_SetScaled(arg0, 1.0f);
            break;

        case 0x37:
            sp20 = 8.0f;
            break;

        case 0x49:
            sp1C = 1;
            break;

        case 0x50:
            sp1C = 1;
            break;

        case 0x74:
            sp20 = 5.0f;
            break;

        case 0x82:
            Particle_Scale_SetScaled(arg0, 1.5f);
            sp24 = -5.0f;
            sp20 = 10.0f;
            sp1C = 1;
            break;

        case 0x83:
            Particle_Scale_SetScaled(arg0, 1.5f);
            sp20 = 5.0f;
            sp1C = 1;
            break;

        case 0x86:
            sp20 = 10.0f;
            break;

        case 0x95:
            sp20 = 10.0f;
            break;
    }

    if (sp1C == 0) {
        Particle_SetRandomPositionOffsetXYZ(arg0, 0xA, 0, 0x23);
    } else {
        Particle_SetRandomPositionOffsetXYZ(arg0, 0xF, 0, 0x2D);
    }

    Particle_PositionOffset_AddXZFromRotationY(arg0, 10.0f + sp24);
    Particle_PositionOffset_AddYScaled(arg0, 5.0f + sp20);
}

void BattleAnim_InitSharedVerticalFadeParticle(Particle* arg0) {
    Vec3s sp30;
    s16 sp2E = Particle_FieldAC_GetS16_0C038(arg0);
    u16 sp2C = Particle_FieldCE_GetS32_0C040(arg0);

    Particle_FieldB0_SetS16_0BFEC(arg0, 2);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp2E * 2);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, Particle_FieldCF_GetS16_0C058(arg0));

    switch (sp2C) {
        case 0:
            Particle_Scale_SetScaled(arg0, 1.0f);
            Particle_SetRandomPositionOffsetXYZ(arg0, 0xA, 0xA, 0xA);
            Particle_StepMagnitude_SetScaled(arg0, (ParticleMath_RandomRange(0x32) * 0.02f) + 0.5f);
            Particle_SetPrimAlpha(arg0, 0x64);
            Particle_FieldAE_SetS16_0BF4C(arg0, 4);
            break;

        case 1:
            Particle_Scale_SetScaled(arg0, 1.0f);
            Particle_SetRandomPositionOffsetXYZ(arg0, 0xA, 0xF, 0x3C);
            Particle_StepMagnitude_SetScaled(arg0, (ParticleMath_RandomRange(0x32) * 0.04f) + 1.0f);
            Particle_SetPrimAlpha(arg0, 0x64);
            Particle_FieldAE_SetS16_0BF4C(arg0, 4);
            break;

        case 2:
            Particle_Scale_SetScaled(arg0, 1.5f);
            Particle_SetRandomPositionOffsetXYZ(arg0, 0xF, 0x14, 0x19);
            Particle_StepMagnitude_SetScaled(arg0, (ParticleMath_RandomRange(0x32) * 0.07f) + 4.0f);
            Particle_SetPrimAlpha(arg0, 0xA0);
            Particle_FieldA6_SetS16_0BCA8(arg0, sp2E);
            Particle_FieldAE_SetS16_0BF4C(arg0, 0xA);
            break;

        case 3:
            Particle_Scale_SetScaled(arg0, 1.0f);
            Particle_SetRandomPositionOffsetXYZ(arg0, 0xA, 0, 0x19);
            Particle_PositionOffset_AddXZFromRotationY(arg0, 10.0f);
            Particle_PositionOffset_AddYScaled(arg0, 5.0f);
            Particle_StepMagnitude_SetScaled(arg0, (ParticleMath_RandomRange(0xA) * 0.05f) + 0.2f);
            Particle_SetPrimAlpha(arg0, 0xFF);
            Particle_FieldAE_SetS16_0BF4C(arg0, 2);
            Particle_FieldB0_SetS16_0BFEC(arg0, 2);
            break;

        case 8:
            Particle_Scale_SetScaled(arg0, 1.2f);
            BattleAnim_InitSharedVerticalFadeParticle_ApplySpeciesOffset(arg0);
            Particle_StepMagnitude_SetScaled(arg0, (ParticleMath_RandomRange(0xA) * 0.05f) + 0.2f);
            Particle_SetPrimAlpha(arg0, 0xFF);
            Particle_FieldAE_SetS16_0BF4C(arg0, 2);
            Particle_FieldB0_SetS16_0BFEC(arg0, 2);
            Particle_FieldA6_SetS16_0BCA8(arg0, sp2E);
            break;

        case 12:
            Particle_Scale_SetScaled(arg0, 1.2f);
            Particle_SetRandomPositionOffsetXYZ(arg0, 0xA, 0, 0x28);
            Particle_PositionOffset_AddXZFromRotationY(arg0, 10.0f);
            Particle_PositionOffset_AddYScaled(arg0, 5.0f);
            Particle_StepMagnitude_SetScaled(arg0, (ParticleMath_RandomRange(0xA) * 0.05f) + 0.2f);
            Particle_SetPrimAlpha(arg0, 0xFF);
            Particle_FieldAE_SetS16_0BF4C(arg0, 2);
            Particle_FieldB0_SetS16_0BFEC(arg0, 2);
            Particle_FieldA6_SetS16_0BCA8(arg0, sp2E);
            break;

        case 4:
            Particle_Scale_SetScaled(arg0, 0.5f);
            Particle_SetRandomPositionOffsetXYZ(arg0, 0xA, 0, 0x14);
            Particle_StepMagnitude_SetScaled(arg0, (ParticleMath_RandomRange(0xA) * 0.05f) + 0.5f);
            Particle_SetPrimAlpha(arg0, 0xFF);
            Particle_FieldAE_SetS16_0BF4C(arg0, 2);
            Particle_FieldB0_SetS16_0BFEC(arg0, 2);
            break;

        case 5:
            Particle_Scale_SetScaled(arg0, 1.0f);
            Particle_SetRandomPositionOffsetXYZ(arg0, 0xF, 0xF, 0xF);
            Particle_StepMagnitude_SetScaled(arg0, (ParticleMath_RandomRange(0x32) * 0.02f) + 1.0f);
            Particle_SetPrimAlpha(arg0, 0x80);
            Particle_FieldAE_SetS16_0BF4C(arg0, 4);
            break;

        case 6:
            Particle_Scale_SetScaled(arg0, 0.5f);
            Particle_SetRandomPositionOffsetXYZ(arg0, 5, 5, 5);
            Particle_StepMagnitude_SetScaled(arg0, 0.5f);
            Particle_SetPrimAlpha(arg0, 0xC8);
            Particle_FieldAE_SetS16_0BF4C(arg0, 4);
            break;

        case 7:
            Particle_Scale_SetScaled(arg0, 0.5f);
            Particle_SetRandomPositionOffsetXYZ(arg0, 0xA, 0, 5);
            Particle_PositionOffset_AddYScaled(arg0, 10.0f);
            Particle_StepMagnitude_SetScaled(arg0, (ParticleMath_RandomRange(0x14) * 0.02f) + 0.5f);
            Particle_SetPrimAlpha(arg0, 0xC8);
            Particle_FieldAE_SetS16_0BF4C(arg0, 4);
            break;

        case 9:
            Particle_Scale_SetScaled(arg0, 1.0f);
            Particle_SetRandomPositionOffsetXYZ(arg0, 0xA, 0, 0xA);
            sp30.x = ParticleMath_RandomSigned(0x1800) - 0x4000;
            sp30.y = ParticleMath_RandomRange(0x10000);
            sp30.z = 0;
            Particle_Rotation_AddVecFromVec3s(arg0, sp30);
            Particle_StepMagnitude_SetScaled(arg0, (ParticleMath_RandomRange(0x14) * 0.15f) + 3.0f);
            Particle_MotionStep_SetVecFromField74AnglesXY(arg0);
            Particle_FieldA6_SetS16_0BCA8(arg0, sp2E);
            Particle_FieldAE_SetS16_0BF4C(arg0, 0xA);
            break;

        case 10:
            Particle_Scale_SetScaled(arg0, 0.8f);
            Particle_SetRandomPositionOffsetXYZ(arg0, 0xF, 0, 0xF);
            sp30.x = ParticleMath_RandomSigned(0x1000) - 0x4000;
            sp30.y = ParticleMath_RandomRange(0x10000);
            sp30.z = 0;
            Particle_Rotation_AddVecFromVec3s(arg0, sp30);
            Particle_StepMagnitude_SetScaled(arg0, (ParticleMath_RandomRange(0x14) * 0.15f) + 4.0f);
            Particle_MotionStep_SetVecFromField74AnglesXY(arg0);
            Particle_FieldA6_SetS16_0BCA8(arg0, 0);
            Particle_FieldAE_SetS16_0BF4C(arg0, 0xA);
            break;

        case 11:
            Particle_Scale_SetScaled(arg0, 0.05f);
            Particle_SetRandomPositionOffsetXYZ(arg0, 0xA, 0, 0xA);
            sp30.x = ParticleMath_RandomSigned(0x1000) - 0x4000;
            sp30.y = ParticleMath_RandomRange(0x10000);
            sp30.z = 0;
            Particle_Rotation_AddVecFromVec3s(arg0, sp30);
            Particle_StepMagnitude_SetScaled(arg0, (ParticleMath_RandomRange(0x14) * 0.3f) + 6.0f);
            Particle_MotionStep_SetVecFromField74AnglesXY(arg0);
            Particle_SetPrimAlpha(arg0, 0xFF);
            Particle_FieldA6_SetS16_0BCA8(arg0, sp2E);
            Particle_FieldAE_SetS16_0BF4C(arg0, 0xA);
            break;
    }

    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Moves: Acid (hit), Bide (use), Defense Curl (use), Dragon Rage (hit), Earthquake (hit), Fissure (hit),
    Flamethrower (hit), Focus Energy (use), Leech Seed (use), Lick (hit), Rage (use), Sharpen (use), Sky Attack
    (use), Sludge (hit), Struggle (use), Substitute (use), Surf (use), Thrash (use), Toxic (hit), Transform
    (use), Waterfall (use/hit)
    TODO: Human contributor add screenshot
*/
void BattleAnim_RunSharedAnimatedVerticalFade(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_InitSharedVerticalFadeParticle(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, arg0->unk_B0, 0);
            Particle_PrimAlpha_Approach_0BC2C(arg0, 0x40, arg0->unk_AE);
            Particle_AddVerticalMotionScalar(arg0);
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }
}

void BattleAnim_ApplySpeciesKeyedSpawnOffset(Particle* arg0) {
    f32 sp1C = 50.0f;
    f32 sp18 = 30.0f;

    switch (BattleAnim_GetOwnerSpecies(arg0->pokemon)) {
        case VENUSAUR:
            sp1C = 40.0f;
            break;

        case ONIX:
            sp1C = 25.0f;
            break;

        case EXEGGUTOR:
            sp1C = 35.0f;
            break;

        case CHANSEY:
            sp18 = 35.0f;
            break;

        case KANGASKHAN:
            sp1C = 40.0f;
            break;

        case GYARADOS:
            sp1C = 20.0f;
            break;

        case LAPRAS:
            sp1C = 30.0f;
            break;
    }

    Particle_PositionOffset_AddXZFromRotationY(arg0, sp1C);
    Particle_PositionOffset_AddYScaled(arg0, sp18);
    Particle_MagnitudeStep_SetScaled(arg0, 0.4f);
}

/*
    Moves: Surf (hit)
    TODO: Human contributor add screenshot
*/
void BattleAnim_RunSplashOfWater(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_InitSharedVerticalFadeParticle(arg0);
            BattleAnim_ApplySpeciesKeyedSpawnOffset(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_EnableRendering(arg0);
                Particle_BeginPhase(arg0);
                Particle_FieldA6_SetS16_0BCA8(arg0, 0x14);
            }
            break;

        case 3:
            Particle_AnimateFramesForward(arg0, 0, 7, 1, 2, 0);
            Particle_PrimAlpha_Approach_0BC2C(arg0, 0x40, 2);
            Particle_Field74_AdvanceAndAddNegY_CheckField2CYNonPositive_09FD8(arg0);
            Particle_AddVelocityXZFromRotationYPlusHalfTurnFromArgument(arg0, 10.0f);
            Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0);
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }

    switch (arg0->phase) {
        case 1:
            Particle_MagnitudeStep_Approach(arg0, 1.0f, 0.1f);
            if (Particle_TickPhaseTimer(arg0, 4) != 0) {
                Particle_AdvancePhase(arg0);
                Particle_MagnitudeStep_SetScaled(arg0, 0.2f);
            }
            break;

        case 2:
            Particle_MagnitudeStep_Approach(arg0, -0.5f, 0.2f);
            break;
    }
}

/*
    Moves: Dig (hit)
    TODO: Human contributor add screenshot
*/
void BattleAnim_RunDigHitDust(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_InitSharedVerticalFadeParticle(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, arg0->unk_B0, 0);
            Particle_PrimAlpha_Approach_0BC2C(arg0, 0x40, arg0->unk_AE);
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRunning(arg0) != 0) {
        Particle_MotionStep_DecrementYUntilAtMost(arg0, -2.0f, 0.3f);
        Particle_MotionOffset_AddVecFromMotionStep(arg0);
    }
}

/*
    Moves: Dig (hit)
    TODO: Human contributor add screenshot
*/
void BattleAnim_RunDigHitRocks(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_InitSharedVerticalFadeParticle(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            if (arg0->worldPosition.y < 0.0f) {
                Particle_MarkInactive(arg0);
            }
            break;
    }

    if (Particle_IsRunning(arg0) != 0) {
        Particle_MotionStep_DecrementYUntilAtMost(arg0, -10.0f, 0.8f);
        Particle_MotionOffset_AddVecFromMotionStep(arg0);
    }
}

void BattleAnim_SetupBarrageUseEffect2(Particle* arg0) {
    UNUSED s32 pad;

    Particle_FieldAC_GetS16_0C038(arg0);
    Particle_FieldCD_GetU8_0C048(arg0);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_Scale_SetScaled(arg0, 0.5f);
    Particle_Rotation_AddVec(arg0, ParticleMath_RandomRange(0x10000), ParticleMath_RandomRange(0x10000), 0);
    Particle_StepMagnitude_SetScaled(arg0, 0.75f);
    Particle_SetPrimAlpha(arg0, 0xFF);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_RunBarrageUseEffect2(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupBarrageUseEffect2(arg0);
            break;

        case 2:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 2, 0);
            Particle_PrimAlpha_Approach_0BC2C(arg0, 0x80, 8);
            Particle_AddMotionOffsetFromRotationTemplate1(arg0);
            break;

        case 3:
            Particle_MarkInactive(arg0);
            break;
    }
}

void BattleAnim_CallbackDescriptor50BurstChild_InitParticle(Particle* arg0) {
    Vec3s sp28;
    s16 sp26 = Particle_FieldCF_GetS16_0C058(arg0);

    Particle_FieldAC_GetS16_0C038(arg0);
    sp28.x = ParticleMath_RandomSignedOffset(0x1000, -0x2000);
    sp28.y = ParticleMath_RandomSignedOffset(0x1000, 0x8000);
    sp28.z = 0;
    Particle_Rotation_AddVecFromVec3s(arg0, sp28);
    Particle_Scale_SetScaled(arg0, 0.05f);
    Particle_SetScaleParameter20(arg0, 0.25f);
    Particle_SetScaleParameter24(arg0, 0.02f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, sp26);
    Particle_FieldA6_SetS16_0BCA8(arg0, 8);
    Particle_StepMagnitude_SetScaled(arg0, (ParticleMath_RandomRange(8) * 0.25f) + 2.0f);
    Particle_MotionStep_SetVecFromField74AnglesXY(arg0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackDescriptor50BurstChild(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_CallbackDescriptor50BurstChild_InitParticle(arg0);
            break;

        case 2:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 1, 7, 1, 2, 0);
            if (Particle_FieldA6_DecrementToZeroStep1_0BDAC(arg0) != 0) {
                Particle_PrimAlpha_Approach_0BC2C(arg0, 0x64, 0x10);
            }
            break;

        case 3:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRunning(arg0) != 0) {
        Particle_ApproachScaleUsingFields20And24(arg0);
        Particle_MotionStep_DecrementYUntilAtMost(arg0, -1.5f, 0.1f);
        Particle_MotionOffset_AddVecFromMotionStep(arg0);
    }
}

void BattleAnim_CallbackDescriptor50BurstEmitter_InitParticle(Particle* arg0) {
    switch (arg0->unk_A6) {
        case 0:
            Particle_SetPosition(arg0, 0.0f, -5.0f, -10.0f);
            break;

        case 1:
            Particle_SetPosition(arg0, 0.0f, 8.0f, 8.0f);
            break;

        case 2:
            Particle_SetPosition(arg0, 0.0f, 3.0f, 0.0f);
            break;

        case 3:
            Particle_SetPosition(arg0, 0.0f, -3.0f, 5.0f);
            break;
    }

    Particle_FieldA6_SetS16_0BCA8(arg0, 6);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackDescriptor50BurstEmitter(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_CallbackDescriptor50BurstEmitter_InitParticle(arg0);
            break;

        case 2:
            BattleAnim_EmitParticleBurst(arg0, BattleAnim_CallbackDescriptor50BurstChild,
                                         &gBattleAnimParticleDescriptors[0x50], 1);
            Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0);
            break;

        case 3:
            Particle_MarkInactive(arg0);
            break;
    }
}

void func_843553B4(void) {
}

void BattleAnim_CallbackUnusedFallingTrailVariant(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_InitSharedVerticalFadeParticle(arg0);
            break;

        case 2:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 1, 7, 1, 2, 0);
            if (Particle_FieldA6_DecrementToZeroStep1_0BDAC(arg0) != 0) {
                Particle_PrimAlpha_Approach_0BC2C(arg0, 0x64, 0x10);
            }
            break;

        case 3:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRunning(arg0) != 0) {
        Particle_ApproachScaleUsingFields20And24(arg0);
        Particle_MotionStep_DecrementYUntilAtMost(arg0, -1.5f, 0.2f);
        Particle_MotionOffset_AddVecFromMotionStep(arg0);
    }
}

void BattleAnim_SetupSolarBeamUseEffect(Particle* arg0) {
    s16 sp26 = Particle_FieldCF_GetS16_0C058(arg0);

    Particle_Scale_SetScaled(arg0, 0.2f);
    Particle_PositionOffset_AddYScaled(arg0, 5.0f);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, sp26);
    Particle_SetPrimAlpha(arg0, 0x40);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_RunSolarBeamUseEffect(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupSolarBeamUseEffect(arg0);
            break;

        case 2:
            Particle_Scale_Approach(arg0, 8.0f, 0.1f);
            Particle_PrimAlpha_Approach_0BC2C(arg0, 0xFF, 4);
            break;

        case 3:
            Particle_DisableAndDetach(arg0);
            break;
    }
}

void BattleAnim_SetupQiChargeChild(Particle* arg0) {
    s16 sp26 = Particle_FieldAC_GetS16_0C038(arg0);
    s16 sp24 = Particle_FieldCF_GetS16_0C058(arg0);

    Particle_StepMagnitude_SetScaled(arg0, 100.0f);
    Particle_MagnitudeStep_SetScaled(arg0, 3.0f);
    Particle_MotionTarget_SetYScaled(arg0, 2.0f);
    Particle_RotationStep_SetY(arg0, 0x400);
    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, sp24);
    Particle_Scale_SetScaled(arg0, 0.5f - (sp26 * 0.04f));
    Particle_SetPrimAlpha(arg0, (0xFF - (sp26 * 0x10)) & 0xFF);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp26 + 1);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Moves: Conversion (use), Mimic (use), Recover (use), Sky Attack (use), Solar Beam (use)
    TODO: Human contributor add screenshot
*/
void BattleAnim_RunQiChargeChild(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupQiChargeChild(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_EnableRendering(arg0);
                Particle_FieldA6_SetS16_0BCA8(arg0, 0x18);
            }
            break;

        case 3:
            if ((Particle_FieldA6_DecrementToZeroStep1_0BDAC(arg0) != 0) &&
                (Particle_Scale_Approach(arg0, 0.0f, 0.05f) != 0)) {
                Particle_AdvanceLifecycle(arg0);
            }
            break;

        case 4:
            Particle_DisableAndDetach(arg0);
            break;
    }

    if (Particle_IsRenderingEnabled(arg0) != 0) {
        Particle_StepMagnitude_ApproachUsingMagnitudeStep(arg0, 0.0f);
        Particle_Rotation_AddYFromRotationStep(arg0);
        Particle_SetMotionOffsetXZFromRotationY(arg0);
        Particle_MotionOffset_ApproachYToMotionTarget(arg0, 0.0f);
        Particle_ApproachEnvBlue(arg0, 0xFF, 4);
        Particle_ApproachEnvGreen(arg0, 0xFF, 4);
    }
}

void BattleAnim_SetupQiCharge(Particle* arg0) {
    Particle_FieldAC_GetS16_0C038(arg0);
    Particle_Rotation_SetY(arg0, ParticleMath_RandomRange(0x10000));
    Particle_MotionOffset_SetYScaled(arg0, ParticleMath_RandomSignedOffsetBySign(0x28, 0x28));
    BattleAnim_EmitParticleBurst(arg0, BattleAnim_RunQiChargeChild, &gBattleAnimParticleDescriptors[0x44], 5);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Moves: Conversion (use), Mimic (use), Recover (use), Sky Attack (use), Solar Beam (use)
    TODO: Human contributor add screenshot
*/
void BattleAnim_RunQiCharge(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupQiCharge(arg0);
            break;

        case 2:
            Particle_DisableAndDetach(arg0);
            break;
    }
}

void BattleAnim_CallbackDescriptor26BurstChild_InitParticle(Particle* arg0) {
    s16 sp26 = Particle_FieldAC_GetS16_0C038(arg0);

    Particle_SetPrimColor(arg0, 0xDC, 0xFF, 0xFF);
    ParticleGfx_RecolorEnv(arg0, 0x35);
    arg0->scale = (sp26 * 0.05f) + 0.5f;
    Particle_StepMagnitude_SetScaled(arg0, 2.0f);
    Particle_SetPrimAlpha(arg0, (0xFF - (sp26 << 5)) & 0xFF);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp26 + 1);
    Particle_SetTextureFrame(arg0, 0xF);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackDescriptor26BurstChild(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_CallbackDescriptor26BurstChild_InitParticle(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_EnableRendering(arg0);
                ParticleMath_ScaleDirectionToLength(&arg0->motionStep, arg0->motionOffset, D_84390348,
                                                    arg0->stepMagnitude);
            }
            break;

        case 3:
            Particle_MotionStep_ScaleVec(arg0, 1.5f);
            if (Particle_TickDelayAndAdvanceLifecycle(arg0, 4) != 0) {
                Particle_StepMagnitude_SetScaled(arg0, 10.0f);
                Particle_FieldA6_SetS16_0BCA8(arg0,
                                              ParticleMath_ScaleDirectionToLength(&arg0->motionStep, arg0->motionOffset,
                                                                                  D_84390348, arg0->stepMagnitude));
            }
            break;

        case 4:
            Particle_Scale_Approach(arg0, 0.1f, 0.02f);
            Particle_PrimR_Approach_0BAE4(arg0, 0xFF, 4);
            Particle_EnvR_Approach_0BB90(arg0, 0xFF, 8);
            Particle_ApproachEnvGreen(arg0, 0xFF, 8);
            Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0);
            break;

        case 5:
            Particle_DisableAndDetach(arg0);
            break;
    }

    if (Particle_IsRenderingEnabled(arg0) != 0) {
        Particle_MotionOffset_AddVecFromMotionStep(arg0);
    }
}

void BattleAnim_CallbackDescriptor26BurstEmitter_InitParticle(Particle* arg0) {
    s16 sp26 = Particle_FieldAC_GetS16_0C038(arg0);

    Particle_Scale_SetScaled(arg0, 0.5f);
    Particle_SetMotionOffsetRandomDirection(arg0, 100.0f);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp26 + 1);
    ParticleGfx_RecolorPrim(arg0, 0xA);
    ParticleGfx_RecolorEnv(arg0, 0x35);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackDescriptor26BurstEmitter(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_CallbackDescriptor26BurstEmitter_InitParticle(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            if (Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 0xF, 2, 1, 0) != 0) {
                BattleAnim_EmitParticleBurst(arg0, BattleAnim_CallbackDescriptor26BurstChild,
                                             &gBattleAnimParticleDescriptors[0x26], 5);
            }
            break;

        case 4:
            Particle_DisableAndDetach(arg0);
            break;
    }
}

void BattleAnim_SetupUnusedGrowingSparkVariant(Particle* arg0) {
    f32 sp24;
    s16 sp22 = Particle_FieldAC_GetS16_0C038(arg0);

    sp24 = (sp22 + 15.0f) / 20.0f;
    if (sp24 >= 1.0f) {
        sp24 = 1.0f;
    }

    Particle_Scale_SetScaled(arg0, sp24);
    Particle_SetScaleParameter20(arg0, 8.0f * sp24);
    Particle_SetScaleParameter24(arg0, 0.35f * sp24);
    Particle_PositionOffset_AddYScaled(arg0, 10.0f);
    Particle_SetTextureFrame(arg0, 0xF);
    ParticleGfx_RecolorPrim(arg0, 0xA);
    ParticleGfx_RecolorEnv(arg0, 0x35);
    Particle_SetPrimAlpha(arg0, 0xFF);
    Particle_FieldA6_SetS16_0BCA8(arg0, sp22 * 8);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackUnusedGrowingSparkVariant(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupUnusedGrowingSparkVariant(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0) != 0) {
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            Particle_TickDelayAndAdvanceLifecycle(arg0, 0xC);
            break;

        case 4:
            Particle_PrimAlpha_ApproachAndStepState_0BC5C(arg0, 0x20, 0x20);
            break;

        case 5:
            Particle_DisableAndDetach(arg0);
            break;
    }

    if (Particle_IsRenderingEnabled(arg0) != 0) {
        Particle_ApproachScaleUsingFields20And24(arg0);
        Particle_ApproachEnvGreen(arg0, 0, 0x10);
        Particle_ApproachEnvBlue(arg0, 0, 8);
    }
}

/*
    Moves: Bone Club (hit), Dizzy Punch (hit), Double Slap (hit), Drill Peck (hit), Headbutt (hit), Pay Day
    (hit)
    TODO: Human contributor add screenshot
*/
void BattleAnim_RunStrongContactMoveHitSpark(void) {
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunDefaultHitSpark, &gBattleAnimParticleDescriptors[0x34],
                                               0, 1, 0x11, 7, 0, 7);
    BattleAnim_CreateEffectSlotForCurrentOwner(2, BattleAnim_RunPhysicalHitPurpleSmoke,
                                               &gBattleAnimParticleDescriptors[0x3D], 0, 8, 0x11, 7, 1, 0);
}

/*
    Used by Comet Punch, Wing Attack, Double Kick, Sonic Boom, Night Shade
*/
void BattleAnim_InitializeUniqueSharedHitSpark(void) {
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunDefaultHitSpark, &gBattleAnimParticleDescriptors[0x34],
                                               0, 1, 0x11, 7, 0, 6);
    BattleAnim_CreateEffectSlotForCurrentOwner(2, BattleAnim_RunPhysicalHitPurpleSmoke,
                                               &gBattleAnimParticleDescriptors[0x3D], 0, 8, 0x11, 7, 1, 0);
}

void BattleAnim_InitializeLowKickHit(void) {
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunDefaultHitSpark, &gBattleAnimParticleDescriptors[0x34],
                                               0, 1, 0x11, 9, 0, 8);
    BattleAnim_CreateEffectSlotForCurrentOwner(2, BattleAnim_RunPhysicalHitPurpleSmoke,
                                               &gBattleAnimParticleDescriptors[0x3D], 0, 8, 0x11, 9, 2, 0);
}

/*
    Moves: Bide (hit), Bonemerang (hit), Counter (hit), Hi Jump Kick (hit), Jump Kick (hit), Karate Chop (hit),
    Pound (hit), Quick Attack (hit), Rage (hit), Rolling Kick (hit), Struggle (hit), Tackle (hit), Take Down
    (hit)
    TODO: Human contributor add screenshot
*/
void BattleAnim_InitializeDefaultLimbForMove(void) {
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunDefaultHitSpark, &gBattleAnimParticleDescriptors[0x34],
                                               0, 1, 0x11, 9, 0, 6);
    BattleAnim_CreateEffectSlotForCurrentOwner(2, BattleAnim_RunPhysicalHitPurpleSmoke,
                                               &gBattleAnimParticleDescriptors[0x3D], 0, 8, 0x11, 9, 0, 0);
}

/*
    Moves: Mega Kick (hit), Mega Punch (hit), Skull Bash (hit)
    TODO: Human contributor add screenshot
*/
void BattleAnim_InitializeSpecialLimbForMove(void) {
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunDefaultHitSpark, &gBattleAnimParticleDescriptors[0x34],
                                               0, 1, 0x11, 9, 0, 0x10);
    BattleAnim_CreateEffectSlotForCurrentOwner(2, BattleAnim_RunPhysicalHitPurpleSmoke,
                                               &gBattleAnimParticleDescriptors[0x3D], 0, 8, 0x11, 9, 0, 0);
}

/*
    Moves: Horn Attack (hit), Horn Drill (hit), Peck (hit), Pin Missile (hit), Spike Cannon (hit)
    TODO: Human contributor add screenshot
*/
void BattleAnim_InitializePointyMoveHitSparks(void) {
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunPointyMoveHitSpark,
                                               &gBattleAnimParticleDescriptors[0x34], 0, 1, 0x11, 9, 3, 0);
}

void BattleAnim_InitializePoisonStingTwineedleHit(void) {
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunPointyMoveHitSpark,
                                               &gBattleAnimParticleDescriptors[0x34], 0, 1, 0x11, 9, 4, 0);
}

void BattleAnim_InitializeFuryAttackGustHit(void) {
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunDefaultHitSpark, &gBattleAnimParticleDescriptors[0x34],
                                               0, 7, 0x11, 9, 1, 0xC);
}

void BattleAnim_InitializeIcePunchUse(void) {
    s32 temp_s1 = gBattleAnimCurrentAnchor & 0xFF;

    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(
        0, 8, 6, BattleAnim_RunSparkleHit, &gBattleAnimParticleDescriptors[0x42], 0, 8, 0x11, temp_s1, 0x18, 2);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(
        1, 8, 6, BattleAnim_RunSparkleHit, &gBattleAnimParticleDescriptors[0x42], 0, 8, 0x11, temp_s1, 0x18, 2);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(
        2, 8, 6, BattleAnim_RunSparkleHit, &gBattleAnimParticleDescriptors[0x42], 0, 8, 0x11, temp_s1, 0x18, 2);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(
        4, 8, 6, BattleAnim_RunSparkleHit, &gBattleAnimParticleDescriptors[0x42], 0, 8, 0x11, temp_s1, 0x18, 2);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 6, 0xC, BattleAnim_RunIceBeamIcePunchEffect,
                                                        &gBattleAnimParticleDescriptors[0x4A], 0, 2, 0x11, temp_s1, 1,
                                                        0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(1, 6, 0xA, BattleAnim_RunIceBeamIcePunchEffect,
                                                        &gBattleAnimParticleDescriptors[0x4A], 0, 2, 0x11, temp_s1, 1,
                                                        0);
}

void BattleAnim_InitializeIcePunchHit(void) {
    UNUSED s32 pad;

    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunDefaultHitSpark, &gBattleAnimParticleDescriptors[0x34],
                                               0, 1, 0x11, 9, 3, 5);
    BattleAnim_CreateEffectSlotForCurrentOwner(2, BattleAnim_RunIceBeamIcePunchEffect,
                                               &gBattleAnimParticleDescriptors[0x3D], 0, 8, 0x10, 0x64, 0, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunIceBeamIcePunchEffect,
                                               &gBattleAnimParticleDescriptors[0x3D], 0, 8, 0x10, 0x64, 0, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(4, BattleAnim_RunSparkleHit, &gBattleAnimParticleDescriptors[0x42], 0, 8,
                                               9, 0, 0x18, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(8, BattleAnim_RunSparkleHit, &gBattleAnimParticleDescriptors[0x42], 0, 4,
                                               9, 0, 0x18, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(0xC, BattleAnim_RunSparkleHit, &gBattleAnimParticleDescriptors[0x42], 0,
                                               4, 9, 0, 0x18, 0);
    BattleAnim_SpawnOwnerModelTintRampDetach(0, 0x35, 0xE0, 0, 0x20);
}

void BattleAnim_InitializeThunderPunchHit(void) {
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunDefaultHitSpark, &gBattleAnimParticleDescriptors[0x34],
                                               0, 1, 0x11, 9, 3, 5);
    BattleAnim_CreateEffectSlotForCurrentOwner(4, BattleAnim_RunThunderPunchHitEffect,
                                               &gBattleAnimParticleDescriptors[0x40], 0, 1, 0x10, 0x64, 0, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 4, 5, BattleAnim_RunStrongElectricSpark,
                                                        &gBattleAnimParticleDescriptors[0x11], 8, 4, 2, 0, 0, 0);
    BattleAnim_SpawnDescriptor25AlphaHoldFadeOut(0, 0, 0x40, 0xA, 0xC8);
    BattleAnim_SpawnOwnerModelTintRampDetach(0, 0x19, 0xE1, 0, 0x40);
}

void BattleAnim_OrphanDescriptor34Emitter(void) {
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_CallbackOrphanDescriptor34ColorFadeDetach,
                                               &gBattleAnimParticleDescriptors[0x34], 0, 5, 0x11,
                                               gBattleAnimCurrentAnchor, 0, 0);
}

void BattleAnim_InitializeSubmissionUse(void) {
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(
        0, 3, 2, BattleAnim_CallbackSharedDescriptors3EAnd3FAnimatedRiseFade, &gBattleAnimParticleDescriptors[0x3F], 0,
        0x10, 0xA, 0, 0x11, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(
        0x28, 3, 2, BattleAnim_CallbackSharedDescriptors3EAnd3FAnimatedRiseFade, &gBattleAnimParticleDescriptors[0x3F],
        0, 0x10, 0xA, 0, 0x11, 0);
}

void BattleAnim_InitializeStompHit(void) {
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunStrongPurpleHitSmoke,
                                               &gBattleAnimParticleDescriptors[0x34], 0, 1, 0x11, 9, 0, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunGroundedCircularHitSpark,
                                               &gBattleAnimParticleDescriptors[0x36], 0, 1, 0xA, 0, 0, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(
        4, 3, 2, BattleAnim_CallbackSharedDescriptors3EAnd3FAnimatedRiseFade, &gBattleAnimParticleDescriptors[0x3F], 0,
        0x10, 0xA, 0, 0x11, 0);
}

/*
    Moves: Body Slam (hit), Crabhammer (hit), Double-Edge (hit), Fly (hit), Seismic Toss (hit), Slam (hit),
    Thrash (hit)
    TODO: Human contributor add screenshot
*/
void BattleAnim_InitializeGroundedCircularHitSpark(void) {
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunStrongPurpleHitSmoke,
                                               &gBattleAnimParticleDescriptors[0x34], 0, 5, 0x11, 0x64, 9, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(8, BattleAnim_RunGroundedCircularHitSpark,
                                               &gBattleAnimParticleDescriptors[0x36], 0, 1, 0xA, 0, 0, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(
        0xC, 3, 2, BattleAnim_CallbackSharedDescriptors3EAnd3FAnimatedRiseFade, &gBattleAnimParticleDescriptors[0x3F],
        0, 0x10, 0xA, 0, 0x11, 0);
}

void BattleAnim_InitializeSubmissionHit(void) {
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunStrongPurpleHitSmoke,
                                               &gBattleAnimParticleDescriptors[0x34], 0, 5, 0xA, 0, 0xA, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(8, BattleAnim_RunGroundedCircularHitSpark,
                                               &gBattleAnimParticleDescriptors[0x36], 0, 1, 0xA, 0, 0, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(
        0xC, 3, 2, BattleAnim_CallbackSharedDescriptors3EAnd3FAnimatedRiseFade, &gBattleAnimParticleDescriptors[0x3F],
        0, 0x10, 0xA, 0, 0x11, 0);
}

/*
    Moves: Bite (hit), Clamp (hit), Guillotine (hit), Hyper Fang (hit), Super Fang (hit), Vice Grip (hit)
    TODO: Human contributor add screenshot
*/
void BattleAnim_InitializeBitingMoveEndEffect(void) {
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunStrongPurpleHitSmoke,
                                               &gBattleAnimParticleDescriptors[0x34], 0, 7, 0xA, 0, 0xB, 0);
}

void BattleAnim_InitializeSwiftUse(void) {
    BattleAnim_SpawnGlobalTintColorFadeDetach(0, 8, 0x22);
}

void BattleAnim_InitializeSwiftHit(void) {
    BattleAnim_CreateEffectSlotForCurrentOwner(0xC, BattleAnim_RunSparkleHit, &gBattleAnimParticleDescriptors[0x42], 0,
                                               8, 9, 0, 0x17, 1);
    BattleAnim_CreateEffectSlotForCurrentOwner(0x12, BattleAnim_RunSparkleHit, &gBattleAnimParticleDescriptors[0x42], 0,
                                               0xA, 9, 0, 0x17, 1);
    BattleAnim_CreateEffectSlotForCurrentOwner(0x20, BattleAnim_RunSparkleHit, &gBattleAnimParticleDescriptors[0x42], 0,
                                               5, 9, 0, 0x17, 1);
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunSwiftHitEffect2, &sDefaultParticleDescriptor, 0, 2, 9,
                                               0, 0, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunSwiftHitEffect2, &sDefaultParticleDescriptor, 0, 2, 9,
                                               0, 0, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(5, BattleAnim_RunSwiftHitEffect2, &sDefaultParticleDescriptor, 0, 2, 9,
                                               0, 0, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(8, BattleAnim_RunSwiftHitEffect2, &sDefaultParticleDescriptor, 0, 2, 9,
                                               0, 0, 0);
}

void BattleAnim_StartEffect137Descriptor3FRisingFade(void) {
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(
        0, 3, 2, BattleAnim_CallbackSharedDescriptors3EAnd3FAnimatedRiseFade, &gBattleAnimParticleDescriptors[0x3F], 0,
        0x10, 0xA, 0, 0x11, 0);
}

void ParticleEffect_TextureNodeSetup(void) {
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 3, 2, BattleAnim_RunSmallSand,
                                                        &gBattleAnimParticleDescriptors[0x4A], 0, 3, 0x11,
                                                        gBattleAnimCurrentAnchor, 0x11, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 4, 2, BattleAnim_RunLargeSand,
                                                        &gBattleAnimParticleDescriptors[0x4A], 0, 2, 0x11,
                                                        gBattleAnimCurrentAnchor, 0x11, 0);
}

/*
    Moves: Cut (use), Scratch (use), Slash (use)
    TODO: Human contributor add screenshot
*/
void BattleAnim_InitializeCutSlashScratchEffect(void) {
    UNUSED s32 pad;

    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunSlashingMoveUse, &gBattleAnimParticleDescriptors[0x29],
                                               0, 1, 0x10, gBattleAnimCurrentAnchor, 0xB, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(4, BattleAnim_RunSlashingMoveUse, &gBattleAnimParticleDescriptors[0x29],
                                               0, 1, 0x10, gBattleAnimCurrentAnchor, 0xB, 1);
    BattleAnim_CreateEffectSlotForCurrentOwner(0xA, BattleAnim_RunSlashingMoveUse,
                                               &gBattleAnimParticleDescriptors[0x29], 0, 1, 0x10,
                                               gBattleAnimCurrentAnchor, 0xB, 2);
    BattleAnim_CreateEffectSlotForCurrentOwner(0x1E, BattleAnim_RunSlashingMoveFullScreenFlash,
                                               &gBattleAnimParticleDescriptors[0x41], 0, 1, 0x18, 0, 0xF, 0);
    BattleAnim_SpawnDescriptor25AlphaHoldFadeOutFullAlpha(0x23, 0, 0x10, 0x14);
}

/*
    Moves: Crabhammer (hit), Cut (hit), Scratch (hit), Slash (hit)
    TODO: Human contributor add screenshot
*/
void BattleAnim_InitializeSlashMoveWhiteFlashOverlay(void) {
    BattleAnim_SpawnDescriptor25AlphaHoldFadeOut(0, 2, 0x10, 0x14, 0xFF);
    BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunSlashingMoveHitSpark,
                                               &gBattleAnimParticleDescriptors[0x2A], 0, 1, 0x10,
                                               gBattleAnimCurrentAnchor, 0x10, 2);
    BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunSlashMoveWhiteFlashOverlay,
                                               &gBattleAnimParticleDescriptors[0x41], 0, 1, 0x18, 0, 0xF, 0);
}

void BattleAnim_InitializeScratchSlashUse(void) {
    s16 tmp = BattleAnim_GetOwnerSpecies(gBattleAnimSecondaryOwner);

    if (tmp == 0x7B) {
        BattleAnim_InitializeCutSlashScratchEffect();
        return;
    }

    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunSlashingMoveUse, &gBattleAnimParticleDescriptors[0x29],
                                               0, 3, 0x10, gBattleAnimCurrentAnchor, 0xB, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(4, BattleAnim_RunSlashingMoveUse, &gBattleAnimParticleDescriptors[0x29],
                                               0, 3, 0x10, gBattleAnimCurrentAnchor, 0xB, 1);
    BattleAnim_CreateEffectSlotForCurrentOwner(0xA, BattleAnim_RunSlashingMoveUse,
                                               &gBattleAnimParticleDescriptors[0x29], 0, 3, 0x10,
                                               gBattleAnimCurrentAnchor, 0xB, 2);
    BattleAnim_CreateEffectSlotForCurrentOwner(0x1E, BattleAnim_RunSlashingMoveFullScreenFlash,
                                               &gBattleAnimParticleDescriptors[0x28], 0, 1, 0x18, 0, 0xF, 0);
    BattleAnim_SpawnDescriptor25AlphaHoldFadeOutFullAlpha(0x23, 0, 0x10, 0x14);
}

void BattleAnim_InitializeScratchSlashHit(void) {
    s16 tmp = BattleAnim_GetOwnerSpecies(gBattleAnimSecondaryOwner);

    if (tmp == 0x7B) {
        BattleAnim_InitializeSlashMoveWhiteFlashOverlay();
        return;
    }

    BattleAnim_SpawnDescriptor25AlphaHoldFadeOut(0, 2, 0x10, 0x14, 0xFF);
    BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunSlashingMoveHitSpark,
                                               &gBattleAnimParticleDescriptors[0x2A], 0, 1, 0x10,
                                               gBattleAnimCurrentAnchor, 0x10, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunSlashingMoveHitSpark,
                                               &gBattleAnimParticleDescriptors[0x2A], 0, 1, 0x10,
                                               gBattleAnimCurrentAnchor, 0x10, 1);
    BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunSlashingMoveHitSpark,
                                               &gBattleAnimParticleDescriptors[0x2A], 0, 1, 0x10,
                                               gBattleAnimCurrentAnchor, 0x10, 2);
    BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunSlashMoveWhiteFlashOverlay,
                                               &gBattleAnimParticleDescriptors[0x28], 0, 1, 0x18, 0, 0xF, 0);
}

void BattleAnim_InitializeFurySwipesUse1(void) {
    UNUSED s16 pad;
    s16 sp3C = BattleAnim_GetOwnerSpecies(gBattleAnimSecondaryOwner);
    s16 sp3A = 3;

    if (sp3C == 0x7B) {
        sp3A = 1;
    }

    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunSlashingMoveUse, &gBattleAnimParticleDescriptors[0x29],
                                               0, sp3A, 0x10, gBattleAnimCurrentAnchor, 0xB, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(4, BattleAnim_RunSlashingMoveUse, &gBattleAnimParticleDescriptors[0x29],
                                               0, sp3A, 0x10, gBattleAnimCurrentAnchor, 0xB, 1);
    BattleAnim_CreateEffectSlotForCurrentOwner(0xA, BattleAnim_RunSlashingMoveUse,
                                               &gBattleAnimParticleDescriptors[0x29], 0, sp3A, 0x10,
                                               gBattleAnimCurrentAnchor, 0xB, 2);

    if (sp3C == 0x7B) {
        BattleAnim_CreateEffectSlotForCurrentOwner(0x1E, BattleAnim_RunSlashingMoveFullScreenFlash,
                                                   &gBattleAnimParticleDescriptors[0x41], 0, 1, 0x18, 0, 0xF, 0);
    } else {
        BattleAnim_CreateEffectSlotForCurrentOwner(0x1E, BattleAnim_RunSlashingMoveFullScreenFlash,
                                                   &gBattleAnimParticleDescriptors[0x28], 0, 1, 0x18, 0, 0xF, 0);
    }
    BattleAnim_SpawnDescriptor25AlphaHoldFadeOutFullAlpha(0x23, 0, 0x10, 0x14);
}

void BattleAnim_InitializeFurySwipesUse2(void) {
    s16 tmp = BattleAnim_GetOwnerSpecies(gBattleAnimSecondaryOwner);

    if (tmp == 0x7B) {
        BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunSlashingMoveFullScreenFlash,
                                                   &gBattleAnimParticleDescriptors[0x41], 0, 1, 0x18, 0, 0xF, 0);
    } else {
        BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunSlashingMoveFullScreenFlash,
                                                   &gBattleAnimParticleDescriptors[0x28], 0, 1, 0x18, 0, 0xF, 0);
    }
    BattleAnim_SpawnDescriptor25AlphaHoldFadeOut(5, 0, 8, 0x14, 0x80);
}

void BattleAnim_InitializeFurySwipesHit(void) {
    s16 tmp = BattleAnim_GetOwnerSpecies(gBattleAnimSecondaryOwner);

    if (tmp == 0x7B) {
        BattleAnim_SpawnDescriptor25AlphaHoldFadeOut(0, 2, 8, 0x14, 0x80);
        BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunFurySwipesHitEffect,
                                                   &gBattleAnimParticleDescriptors[0x2A], 0, 1, 0x10,
                                                   gBattleAnimCurrentAnchor, 0x10, 2);
        BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunSlashMoveWhiteFlashOverlay,
                                                   &gBattleAnimParticleDescriptors[0x41], 0, 1, 0x18, 0, 0xF, 0);
    } else {
        BattleAnim_SpawnDescriptor25AlphaHoldFadeOut(0, 2, 8, 0x14, 0x80);
        BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunFurySwipesHitEffect,
                                                   &gBattleAnimParticleDescriptors[0x2A], 0, 1, 0x10,
                                                   gBattleAnimCurrentAnchor, 0x10, 0);
        BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunFurySwipesHitEffect,
                                                   &gBattleAnimParticleDescriptors[0x2A], 0, 1, 0x10,
                                                   gBattleAnimCurrentAnchor, 0x10, 1);
        BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunFurySwipesHitEffect,
                                                   &gBattleAnimParticleDescriptors[0x2A], 0, 1, 0x10,
                                                   gBattleAnimCurrentAnchor, 0x10, 2);
        BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunSlashMoveWhiteFlashOverlay,
                                                   &gBattleAnimParticleDescriptors[0x28], 0, 1, 0x18, 0, 0xF, 0);
    }
}

void BattleAnim_InitializeVineWhipUse(void) {
    UNUSED s32 pad;

    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunSlashingMoveUse, &gBattleAnimParticleDescriptors[0x29],
                                               0, 1, 0x10, gBattleAnimCurrentAnchor, 0xD, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(4, BattleAnim_RunSlashingMoveUse, &gBattleAnimParticleDescriptors[0x29],
                                               0, 1, 0x10, gBattleAnimCurrentAnchor, 0xD, 1);
    BattleAnim_CreateEffectSlotForCurrentOwner(0xA, BattleAnim_RunSlashingMoveUse,
                                               &gBattleAnimParticleDescriptors[0x29], 0, 1, 0x10,
                                               gBattleAnimCurrentAnchor, 0xD, 2);
    BattleAnim_CreateEffectSlotForCurrentOwner(0x1E, BattleAnim_RunSlashingMoveFullScreenFlash,
                                               &gBattleAnimParticleDescriptors[0x41], 0, 1, 0x18, 0, 0xD, 1);
    BattleAnim_SpawnDescriptor25AlphaHoldFadeOutFullAlpha(0x23, 0, 0x10, 0x29);
}

void BattleAnim_InitializeVineWhipHit(void) {
    UNUSED s32 pad;

    BattleAnim_SpawnDescriptor25AlphaHoldFadeOut(0, 2, 0x10, 0x29, 0xFF);
    BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunSlashingMoveHitSpark,
                                               &gBattleAnimParticleDescriptors[0x2A], 0, 1, 0x10,
                                               gBattleAnimCurrentAnchor, 0xD, 2);
    BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunSlashMoveWhiteFlashOverlay,
                                               &gBattleAnimParticleDescriptors[0x41], 0, 1, 0x18, 0, 0xD, 1);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 1, 5, BattleAnim_RunSlowFallingHitEffect,
                                                        &gBattleAnimParticleDescriptors[0x2B], 0, 2, 0xE, 0, 0, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 1, 5, BattleAnim_RunSlowFallingHitEffect,
                                                        &gBattleAnimParticleDescriptors[0x2B], 0, 2, 0xE, 0, 0, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 2, 0xF, BattleAnim_RunSlowFallingHitEffect,
                                                        &gBattleAnimParticleDescriptors[0x2B], 0, 1, 0xE, 0, 0, 0);
}

void BattleAnim_InitializeFirePunchUse(void) {
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 1, 0x24, BattleAnim_RunFirePunchUseEffect2,
                                                        &gBattleAnimParticleDescriptors[0x46], 0, 1, 0x11,
                                                        gBattleAnimCurrentAnchor, 0, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunFirePunchUseEffect1,
                                               &gBattleAnimParticleDescriptors[0x40], 0, 1, 0x10,
                                               gBattleAnimCurrentAnchor, 0, 0);
}

void BattleAnim_InitializeRazorWindHit(void) {
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunRazorWindHitEffect,
                                               &gBattleAnimParticleDescriptors[0x29], 0, 6, 0x10, 0x64, 0, 0);
}

void BattleAnim_InitializeRazorLeafUse(void) {
    BattleAnim_SpawnGlobalTintColorFadeDetach(0, 8, 0x28);
}

void BattleAnim_InitializeRazorLeafHit(void) {
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunPetalDanceRazorLeafHitEffect,
                                               &gBattleAnimParticleDescriptors[0x29], 0, 8, 0x10, 9, 0, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(0x10, BattleAnim_RunPetalDanceRazorLeafHitEffect,
                                               &gBattleAnimParticleDescriptors[0x29], 0, 0xA, 0x10, 9, 0, 0);
    BattleAnim_SpawnGlobalTintColorFadeDetach(0x46, 4, 0xA);
}

void BattleAnim_InitializePetalDanceUse(void) {
    BattleAnim_SpawnGlobalTintColorFadeDetach(0, 8, 0x17);
}

void BattleAnim_InitializePetalDanceHitSequence(void) {
    BattleAnim_AddSpawnDelay(0x10);
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunPetalDanceRazorLeafHitEffect,
                                               &gBattleAnimParticleDescriptors[0x29], 0, 8, 0x10, 9, 1, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(0x10, BattleAnim_RunPetalDanceRazorLeafHitEffect,
                                               &gBattleAnimParticleDescriptors[0x29], 0, 8, 0x10, 9, 1, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(0x20, BattleAnim_RunPetalDanceRazorLeafHitEffect,
                                               &gBattleAnimParticleDescriptors[0x29], 0, 8, 0x10, 9, 1, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(0x30, BattleAnim_RunPetalDanceRazorLeafHitEffect,
                                               &gBattleAnimParticleDescriptors[0x29], 0, 8, 0x10, 9, 1, 0);
}

/*
    Moves: Poison Gas (hit), Poison Powder (hit), Sleep Powder (hit), Smog (hit), Spore (hit), Stun Spore (hit)
    TODO: Human contributor add screenshot
*/
void BattleAnim_InitializePowderSporeGasMoveHit(void) {
    switch (BattleAnim_GetQueuedMoveId()) {
        case POISONPOWDER:
            BattleAnim_SpawnOwnerModelTintPulseDetach(0, 0x64, 0x3E, 0, 0xA0, 4);
            break;

        case STUN_SPORE:
            BattleAnim_SpawnOwnerModelTintPulseDetach(0, 0x64, 0x19, 0, 0xA0, 4);
            break;

        case SLEEP_POWDER:
            BattleAnim_SpawnOwnerModelTintPulseDetach(0, 0x64, 0x2F, 0, 0xA0, 4);
            break;

        case SPORE:
            BattleAnim_SpawnOwnerModelTintPulseDetach(0, 0x64, 0x19, 0, 0xA0, 4);
            break;

        case SMOG:
            BattleAnim_SpawnOwnerModelTintPulseDetach(0, 0x64, 0x3E, 0, 0xA0, 4);
            break;

        case POISON_GAS:
            BattleAnim_SpawnOwnerModelTintPulseDetach(0, 0x64, 0x3E, 0, 0xA0, 4);
            break;
    }
    BattleAnim_CreateEffectSlotForCurrentOwner(
        0, BattleAnim_CallbackStartEffects28And53And65And76Descriptor47AnimatedScaleMotion,
        &gBattleAnimParticleDescriptors[0x47], 0, 7, 0x10, 0x64, 1, 0);
}

void BattleAnim_InitializeSandAttackUse(void) {
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 2, 3, BattleAnim_RunSandAttackUseEffect,
                                                        &gBattleAnimParticleDescriptors[0x49], 0, 8, 0x11,
                                                        gBattleAnimCurrentAnchor, 0, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 3, 2, BattleAnim_RunSmallSand,
                                                        &gBattleAnimParticleDescriptors[0x47], 0, 6, 0x11,
                                                        gBattleAnimCurrentAnchor, 0x31, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 4, 2, BattleAnim_RunLargeSand,
                                                        &gBattleAnimParticleDescriptors[0x47], 0, 4, 0x11,
                                                        gBattleAnimCurrentAnchor, 0x31, 0);
}

void BattleAnim_InitializeSandAttackHit(void) {
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunSandAttackHitEffect,
                                               &gBattleAnimParticleDescriptors[0x49], 0, 5, 0x10, 0x64, 0, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(3, BattleAnim_RunSandAttackHitEffect,
                                               &gBattleAnimParticleDescriptors[0x49], 0, 5, 0x10, 0x64, 0, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(
        0, BattleAnim_CallbackStartEffects28And53And65And76Descriptor47AnimatedScaleMotion,
        &gBattleAnimParticleDescriptors[0x47], 0, 7, 0x10, 0x64, 1, 0);
}

void BattleAnim_InitializeAcidArmorUse(void) {
    BattleAnim_SpawnInvisibility(0, 0xFF, 0x30, 6);
    BattleAnim_SpawnInvisibility(0x24, 0, 0, 0xFF);
    BattleAnim_SpawnOwnerModelTintRampDetach(0, 0x23, 0, 0xFF, 8);
    BattleAnim_AddSpawnDelay(6);
    BattleAnim_CreateEffectSlotForCurrentOwner(2, BattleAnim_RunAcidArmorUseEffect1,
                                               &gBattleAnimParticleDescriptors[0x4B], 0, 1, 0xA, 0, 0x2F, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 8, 4, BattleAnim_RunAcidArmorUseEffect2,
                                                        &gBattleAnimParticleDescriptors[0x3D], 0, 3, 9, 0, 0x30, 0);
}

/*
    Moves: Flash (use)
    TODO: Human contributor add screenshot
*/
void BattleAnim_InitializeFlashUse(void) {
    BattleAnim_SpawnGlobalTintColorFadeDetach(0, 0x20, 6);
    BattleAnim_SpawnOwnerModelTintRampDetach(8, 0xA, 0, 0xFF, 0x40);
    BattleAnim_SpawnDescriptor25AlphaFadeInHoldFullAlpha(0xC, 5, 0x10, 0xA);
    BattleAnim_CreateEffectSlotForCurrentOwner(0xC, BattleAnim_RunDefaultHitSpark,
                                               &gBattleAnimParticleDescriptors[0x34], 0, 4, 9, 0, 0x18, 0xD);
    BattleAnim_AddSpawnDelay(0x21);
    BattleAnim_SpawnDescriptor25AlphaHoldFadeOutFullAlpha(0, 0, 0x10, 0xA);
    BattleAnim_SpawnGlobalTintColorFadeDetach(0, 0x10, 0xA);
    BattleAnim_SpawnOwnerModelTintRampDetach(0, 0xA, 0xFF, 0, 0x10);
}

void BattleAnim_InitializeFlashHit(void) {
    BattleAnim_SpawnGlobalTintColorFadeDetach(0, 0x20, 6);
    BattleAnim_SpawnOwnerModelTintRampDetach(8, 0xA, 0, 0xFF, 0x40);
    BattleAnim_SpawnDescriptor25AlphaHoldFadeOutFullAlpha(0, 0, 0x10, 0xA);
    BattleAnim_AddSpawnDelay(0x21);
    BattleAnim_SpawnGlobalTintColorFadeDetach(0, 0x10, 0xA);
    BattleAnim_SpawnOwnerModelTintRampDetach(0, 0xA, 0xFF, 0, 0x10);
}

void BattleAnim_InitializeBideRageUse(void) {
    UNUSED s32 pad;

    BattleAnim_SpawnOwnerModelTintPulseDetach(0, 1, 0xC, 0, 0xC8, 0x10);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(6, 4, 0xE, BattleAnim_RunSharedAnimatedVerticalFade,
                                                        &gBattleAnimParticleDescriptors[0x3D], 0, 1, 0x11, 7, 0x1A, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(7, 8, 7, BattleAnim_RunSharedAnimatedVerticalFade,
                                                        &gBattleAnimParticleDescriptors[0x3D], 0, 1, 0x11, 7, 0x1A, 0);
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunOscillatingFlameUnderneathPokemon,
                                               &gBattleAnimParticleDescriptors[0x4F], 0, 0x20, 7, 0, 0, 0);
    BattleAnim_InitializeBideRageUseSequence();
}

void BattleAnim_InitializeStruggleThrashUse(void) {
    BattleAnim_SpawnOwnerModelTintPulseDetach(0, 1, 0xC, 0, 0xC8, 4);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(
        4, 3, 2, BattleAnim_CallbackSharedDescriptors3EAnd3FAnimatedRiseFade, &gBattleAnimParticleDescriptors[0x3F], 0,
        0x10, 0xA, 0, 0x11, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(6, 4, 0xE, BattleAnim_RunSharedAnimatedVerticalFade,
                                                        &gBattleAnimParticleDescriptors[0x3D], 0, 1, 0x11, 7, 0x33, 7);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(7, 8, 7, BattleAnim_RunSharedAnimatedVerticalFade,
                                                        &gBattleAnimParticleDescriptors[0x3D], 0, 1, 0x11, 7, 0x33, 7);
}

void BattleAnim_InitializeDoubleEdgeTakeDownUse(void) {
    BattleAnim_SpawnGlobalTintColorFadeDetach(0, 0x20, 6);
    BattleAnim_InitializeSpeedLines();
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(
        8, 3, 2, BattleAnim_CallbackSharedDescriptors3EAnd3FAnimatedRiseFade, &gBattleAnimParticleDescriptors[0x3F], 0,
        0x10, 0xA, 0, 0x11, 0);
}

void BattleAnim_InitializeBodySlamUse(void) {
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(
        4, 3, 2, BattleAnim_CallbackSharedDescriptors3EAnd3FAnimatedRiseFade, &gBattleAnimParticleDescriptors[0x3F], 0,
        0x10, 0xA, 0, 0x11, 0);
}

void BattleAnim_InitializeEarthquakeFissureHitSequence(void) {
    BattleAnim_CreateEffectSlotForCurrentOwner(4, BattleAnim_RunSharedAnimatedVerticalFade,
                                               &gBattleAnimParticleDescriptors[0x4A], 0, 8, 0xA, 0, 0x33, 1);
    BattleAnim_CreateEffectSlotForCurrentOwner(5, BattleAnim_RunSharedAnimatedVerticalFade,
                                               &gBattleAnimParticleDescriptors[0x4A], 0, 8, 0xA, 0, 0x33, 1);
    BattleAnim_CreateEffectSlotForCurrentOwner(6, BattleAnim_RunSharedAnimatedVerticalFade,
                                               &gBattleAnimParticleDescriptors[0x4A], 0, 8, 0xA, 0, 0x33, 1);
    BattleAnim_CreateEffectSlotForCurrentOwner(7, BattleAnim_RunSharedAnimatedVerticalFade,
                                               &gBattleAnimParticleDescriptors[0x4A], 0, 8, 0xA, 0, 0x33, 1);
    BattleAnim_CreateEffectSlotForCurrentOwner(8, BattleAnim_RunDefaultHitSpark, &gBattleAnimParticleDescriptors[0x51],
                                               0, 5, 0x11, 9, 0x17, 0xE);
    BattleAnim_CreateEffectSlotForCurrentOwner(0x10, BattleAnim_RunDefaultHitSpark,
                                               &gBattleAnimParticleDescriptors[0x51], 0, 5, 0x11, 9, 0x17, 0xE);
}

void BattleAnim_EndEffectUnusedVerticalFadeAndDelayedFrameSequence(void) {
    BattleAnim_CreateEffectSlotForCurrentOwner(4, BattleAnim_RunSharedAnimatedVerticalFade,
                                               &gBattleAnimParticleDescriptors[0x4A], 0, 8, 0xA, 0, 0x33, 1);
    BattleAnim_CreateEffectSlotForCurrentOwner(5, BattleAnim_RunSharedAnimatedVerticalFade,
                                               &gBattleAnimParticleDescriptors[0x4A], 0, 8, 0xA, 0, 0x33, 1);
    BattleAnim_CreateEffectSlotForCurrentOwner(6, BattleAnim_RunSharedAnimatedVerticalFade,
                                               &gBattleAnimParticleDescriptors[0x4A], 0, 8, 0xA, 0, 0x33, 1);
    BattleAnim_CreateEffectSlotForCurrentOwner(7, BattleAnim_RunSharedAnimatedVerticalFade,
                                               &gBattleAnimParticleDescriptors[0x4A], 0, 8, 0xA, 0, 0x33, 1);
    BattleAnim_CreateEffectSlotForCurrentOwner(8, BattleAnim_RunDefaultHitSpark, &gBattleAnimParticleDescriptors[0x51],
                                               0, 5, 0x11, 9, 0x17, 0xE);
    BattleAnim_CreateEffectSlotForCurrentOwner(0x10, BattleAnim_RunDefaultHitSpark,
                                               &gBattleAnimParticleDescriptors[0x51], 0, 5, 0x11, 9, 0x17, 0xE);
}

void BattleAnim_InitializeDefenseCurlSharpenUseSequence(void) {
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunSharedAnimatedVerticalFade,
                                               &gBattleAnimParticleDescriptors[0x3D], 0, 8, 0xE, 0, 0x33, 2);
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunSharedAnimatedVerticalFade,
                                               &gBattleAnimParticleDescriptors[0x3D], 0, 2, 0xE, 0, 0x33, 2);
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunSharedAnimatedVerticalFade,
                                               &gBattleAnimParticleDescriptors[0x3D], 0, 2, 0xE, 0, 0x33, 2);
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunSharedAnimatedVerticalFade,
                                               &gBattleAnimParticleDescriptors[0x3D], 0, 2, 0xE, 0, 0x33, 2);
    BattleAnim_CreateEffectSlotForCurrentOwner(4, BattleAnim_RunSharedAnimatedVerticalFade,
                                               &gBattleAnimParticleDescriptors[0x3D], 0, 8, 0xE, 0, 0x33, 2);
    BattleAnim_CreateEffectSlotForCurrentOwner(8, BattleAnim_RunSharedAnimatedVerticalFade,
                                               &gBattleAnimParticleDescriptors[0x3D], 0, 8, 0xE, 0, 0x33, 2);
}

void BattleAnim_InitializeSubstituteTransformUse(void) {
    BattleAnim_SpawnOwnerModelTintRampDetach(4, 0xA, 0, 0xFF, 0x40);
    BattleAnim_SpawnOwnerModelTintRampDetach(9, 0xA, 0xFF, 0, 0x40);
    BattleAnim_CreateEffectSlotForCurrentOwner(0, BattleAnim_RunSharedAnimatedVerticalFade,
                                               &gBattleAnimParticleDescriptors[0x3D], 0, 8, 0xE, 0, 0x33, 2);
    BattleAnim_CreateEffectSlotForCurrentOwner(1, BattleAnim_RunSharedAnimatedVerticalFade,
                                               &gBattleAnimParticleDescriptors[0x3D], 0, 8, 0xE, 0, 0x33, 2);
    BattleAnim_CreateEffectSlotForCurrentOwner(2, BattleAnim_RunSharedAnimatedVerticalFade,
                                               &gBattleAnimParticleDescriptors[0x3D], 0, 8, 0xE, 0, 0x33, 2);
    BattleAnim_CreateEffectSlotForCurrentOwner(4, BattleAnim_RunSharedAnimatedVerticalFade,
                                               &gBattleAnimParticleDescriptors[0x3D], 0, 8, 0xE, 0, 0x33, 2);
    BattleAnim_CreateEffectSlotForCurrentOwner(4, BattleAnim_RunSharedAnimatedVerticalFade,
                                               &gBattleAnimParticleDescriptors[0x3D], 0, 8, 0xE, 0, 0x33, 2);
    BattleAnim_CreateEffectSlotForCurrentOwner(8, BattleAnim_RunSharedAnimatedVerticalFade,
                                               &gBattleAnimParticleDescriptors[0x3D], 0, 8, 0xE, 0, 0x33, 2);
}

/*
    Moves: Conversion (use), Mimic (use), Recover (use), Sky Attack (use), Solar Beam (use)
    TODO: Human contributor add screenshot
*/
void BattleAnim_InitializeQiCharge(s32 arg0) {
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0, 1, 4, BattleAnim_RunQiCharge, &sDefaultParticleDescriptor, 0,
                                                        1, 2, 0, arg0, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(8, 1, 4, BattleAnim_RunQiCharge, &sDefaultParticleDescriptor, 0,
                                                        1, 2, 0, arg0, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0x10, 1, 8, BattleAnim_RunQiCharge, &sDefaultParticleDescriptor,
                                                        0, 1, 2, 0, arg0, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0x18, 1, 8, BattleAnim_RunQiCharge, &sDefaultParticleDescriptor,
                                                        0, 1, 2, 0, arg0, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0x20, 1, 8, BattleAnim_RunQiCharge, &sDefaultParticleDescriptor,
                                                        0, 1, 2, 0, arg0, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0x30, 1, 8, BattleAnim_RunQiCharge, &sDefaultParticleDescriptor,
                                                        0, 1, 2, 0, arg0, 0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(0x3C, 1, 8, BattleAnim_RunQiCharge, &sDefaultParticleDescriptor,
                                                        0, 1, 2, 0, arg0, 0);
}

/*
    Moves: Conversion (use), Mimic (use)
    TODO: Human contributor add screenshot
*/
void BattleAnim_InitializeMimicConversionUse(void) {
    BattleAnim_SpawnGlobalTintColorFadeDetach(0, 8, 6);
    BattleAnim_InitializeQiCharge(0xE);
    BattleAnim_SpawnGlobalTintColorFadeDetach(0x64, 0x20, 0xA);
}

void BattleAnim_InitializeSolarBeamUseSequence(void) {
    BattleAnim_SpawnGlobalTintColorFadeDetach(0, 8, 6);
    BattleAnim_InitializeQiCharge(0x17);
    BattleAnim_CreateEffectSlotForCurrentOwner(0xA, BattleAnim_RunSolarBeamUseEffect,
                                               &gBattleAnimParticleDescriptors[0x3C], 0x1A, 1, 5, 0, 0x33, 0);
    BattleAnim_SpawnGlobalTintColorFadeDetach(0x64, 0x20, 0xA);
}

void BattleAnim_InitializeGrowthUse(void) {
    BattleAnim_SpawnOwnerModelTintPulseDetach(0xA, 1, 0xA, 0, 0xFF, 0x20);
}

void BattleAnim_InitializeRecoverUse(void) {
    BattleAnim_SpawnGlobalTintColorFadeDetach(0, 8, 6);
    BattleAnim_InitializeQiCharge(0xC);
    BattleAnim_SpawnRainbowHueChange(4, 0x32, 0x20, 0xA0);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(4, 8, 5, BattleAnim_RunSparkleHit,
                                                        &gBattleAnimParticleDescriptors[0x42], 0, 8, 9, 0, 0xC, 9);
}

void BattleAnim_InitializeHardenUse(void) {
    BattleAnim_SpawnOwnerModelTintRampDetach(0, 0xA, 0xFF, 0, 0x20);
    BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(4, 2, 2, BattleAnim_RunSparkleHit,
                                                        &gBattleAnimParticleDescriptors[0x42], 0, 8, 9, 0, 0x17, 1);
}
