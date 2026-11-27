#include "battle_engine.h"
#include "src/math_util.h"

// Only the Japanese release has this file. It sits between battle_engine_shared_move_effects and
// battle_engine_effect_render_pipeline.

// Japanese-only particle that scatters in a ring around the owner with one of four palettes. Nothing in
// the Japanese ROM points at it.
void BattleAnim_SetupUnusedRingScatterParticle(Particle* arg0) {
    UNUSED s32 pad[3];
    Vec3s sp34;
    f32 sp30;
    s16 sp2E;
    UNUSED s16 pad2;
    UNUSED s16 pad3;
    s16 palette;

    sp2E = Particle_FieldAC_GetS16_0C038(arg0);
    Particle_FieldCE_GetS32_0C040(arg0);

    palette = 0x17;
    switch ((s16)ParticleMath_RandomRange(4)) {
        case 0:
            break;

        case 1:
            palette = 0xC;
            break;

        case 2:
            palette = 0x39;
            break;

        case 3:
            palette = 0xE;
            break;
    }

    ParticleGfx_RecolorPrimEnvFromPalettePair(arg0, palette);
    Particle_Scale_SetScaled(arg0, 0.1f);
    Particle_SetScaleParameter20(arg0, 0.5f);
    Particle_SetScaleParameter24(arg0, 0.04f);

    sp34.x = ParticleMath_RandomRange(0x10000);
    sp34.y = ParticleMath_RandomRange(0x10000);
    sp34.z = 0;
    Particle_Rotation_SetVecFromVec3s(arg0, sp34);

    sp30 = ParticleMath_RandomRange(200) + 600.0f;
    arg0->positionOffset.x = SINS(sp34.y) * sp30;
    arg0->positionOffset.y = ParticleMath_RandomSigned(150) + 200;
    arg0->positionOffset.z = COSS(sp34.y) * sp30;

    if (arg0->unk_CE == 1) {
        Particle_StepMagnitude_SetScaled(arg0, ParticleMath_RandomRange(20) + 5.0f);
        Particle_MotionStep_SetVecFromField74AnglesXY(arg0);
    }

    Particle_FieldA6_SetS16_0BCA8(arg0, sp2E % 10);
    Particle_DisableRendering(arg0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_RunUnusedRingScatterParticle(Particle* arg0) {
    if (Particle_HasFlags(arg0, 0x1000)) {
        Particle_MarkInactive(arg0);
    }

    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupUnusedRingScatterParticle(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0)) {
                Particle_EnableRendering(arg0);
            }
            break;

        case 3:
            if (Particle_ApproachStoredScaleAndAdvanceLifecycle(arg0)) {
                Particle_SetScaleParameter20(arg0, 0.1f);
            }
            break;

        case 4:
            Particle_ApproachStoredScaleAndAdvanceLifecycle(arg0);
            break;

        case 5:
            Particle_SetScaleParameter20(arg0, 0.5f);
            arg0->lifecycle = 3;
            break;
    }

    if (Particle_IsRenderingEnabled(arg0) && arg0->unk_CE) {
        Particle_MotionOffset_AddVecFromMotionStep(arg0);
    }
}

void func_84359274_jp(void) {
}
