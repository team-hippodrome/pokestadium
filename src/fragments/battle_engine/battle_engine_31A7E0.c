#include "battle_engine.h"

void BattleAnim_InitSecondaryOwnerParticleBlueFadeGrow(Particle* arg0) {
    Particle_Scale_SetScaled(arg0, 0.1f);
    Particle_SetPrimColor(arg0, 0xFF, 0xFF, 0xFF);
    Particle_SetEnvColor(arg0, 0x64, 0xC8, 0xFF);
    Particle_SetPrimAlpha(arg0, 0xFF);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_UpdateSecondaryOwnerParticleBlueFadeGrow(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_InitSecondaryOwnerParticleBlueFadeGrow(arg0);
            break;

        case 2:
            Particle_AnimateFramesForwardAndAdvanceLifecycle(arg0, 0, 7, 1, 2, 0);
            Particle_PrimAlpha_Approach_0BC2C(arg0, 0x20, 8);
            break;

        case 3:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRunning(arg0) != 0) {
        Particle_Scale_Approach(arg0, 0.5f, 0.05f);
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
