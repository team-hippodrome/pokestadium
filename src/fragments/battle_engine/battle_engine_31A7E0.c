#include "battle_engine.h"

/*
Moves covered by this file:
    - poison string
    - Twineedle
    - Pin missile
    - Spike Cannon
*/

void BattleAnim_InitStingerTrailColor(Particle* particle) {
    Particle_Scale_SetScaled(particle, 0.1f);
    Particle_SetPrimColor(particle, 0xFF, 0xFF, 0xFF); // #FFFFFF
    Particle_SetEnvColor(particle, 0x64, 0xC8, 0xFF); // #64C8FF
    Particle_SetPrimAlpha(particle, 0xFF);
    Particle_AdvanceLifecycle(particle);
}

void BattleAnim_UpdateStingerProjectileLifecycle(Particle* particle) {
    switch (particle->lifecycle) {
        case 1:
            BattleAnim_InitStingerTrailColor(particle);
            break;

        case 2:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(particle, 0, 7, 1, 2, 0);
            Particle_PrimAlpha_Approach_0BC2C(particle, 0x20, 8);
            break;

        case 3:
            Particle_MarkInactive(particle);
            break;
    }

    if (Particle_IsRunning(particle) != 0) {
        Particle_Scale_Approach(particle, 0.5f, 0.05f);
    }
}

void BattleAnim_InitStingerTrailColorDuplicate(Particle* particle) {
    Particle_Scale_SetScaled(particle, 0.1f);
    Particle_SetPrimColor(particle, 0xFF, 0xFF, 0xFF); // #FFFFFF
    Particle_SetEnvColor(particle, 0x64, 0xC8, 0xFF); // #64C8FF
    Particle_SetPrimAlpha(particle, 0xFF);
    Particle_AdvanceLifecycle(particle);
}

void BattleAnim_SpawnStingerProjectile(Particle* particle) {
    if (particle->lifecycle == 1) {
        BattleAnim_InitStingerTrailColorDuplicate(particle);
    }

    if (Particle_IsRunning(particle) != 0) {
        Particle_Scale_Approach(particle, 0.8f, 0.08f);
    }
}

void BattleAnim_InitStingerMoveParticle(Particle* particle, u8 step) {
    switch (step) {
        case 1:
            particle->updateCallback = BattleAnim_UpdateStingerProjectileLifecycle;
            particle->descriptor = &gBattleAnimParticleDescriptors[25];
            break;

        case 2:
            particle->updateCallback = BattleAnim_SpawnStingerProjectile;
            particle->descriptor = &gBattleAnimParticleDescriptors[22];
            break;
    }
}
