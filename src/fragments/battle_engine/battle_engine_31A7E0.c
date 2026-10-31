#include "battle_engine.h"

/*
Moves covered by this file:
    - poison string
    - Twineedle
    - Pin missile
    - Spike Cannon
*/

void BattleAnim_InitSecondaryOwnerParticleBlueFadeGrow(Particle* particle) {
    Particle_Scale_SetScaled(particle, 0.1f);
    Particle_SetPrimColor(particle, 0xFF, 0xFF, 0xFF); // #FFFFFF
    Particle_SetEnvColor(particle, 0x64, 0xC8, 0xFF); // #64C8FF
    Particle_SetPrimAlpha(particle, 0xFF);
    Particle_AdvanceLifecycle(particle);
}

void BattleAnim_UpdateSecondaryOwnerParticleBlueFadeGrow(Particle* particle) {
    switch (particle->lifecycle) {
        case 1:
            BattleAnim_InitSecondaryOwnerParticleBlueFadeGrow(particle);
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

void BattleAnim_InitSecondaryOwnerParticleBlueGrow(Particle* arg0) {
    Particle_Scale_SetScaled(arg0, 0.1f);
    Particle_SetPrimColor(arg0, 0xFF, 0xFF, 0xFF);
    Particle_SetEnvColor(arg0, 0x64, 0xC8, 0xFF);
    Particle_SetPrimAlpha(arg0, 0xFF);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_UpdateSecondaryOwnerParticleBlueGrow(Particle* arg0) {
    if (arg0->lifecycle == 1) {
        BattleAnim_InitSecondaryOwnerParticleBlueGrow(arg0);
    }

    if (Particle_IsRunning(arg0) != 0) {
        Particle_Scale_Approach(arg0, 0.8f, 0.08f);
    }
}

// poison sting
void BattleAnim_ConfigureSecondaryOwnerParticleVariant(Particle* arg0, u8 arg1) {
    switch (arg1) {
        case 1:
            arg0->updateCallback = BattleAnim_UpdateSecondaryOwnerParticleBlueFadeGrow;
            arg0->descriptor = &gBattleAnimParticleDescriptors[25];
            break;

        case 2:
            arg0->updateCallback = BattleAnim_UpdateSecondaryOwnerParticleBlueGrow;
            arg0->descriptor = &gBattleAnimParticleDescriptors[22];
            break;
    }
}
