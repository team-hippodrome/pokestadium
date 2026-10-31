#include "battle_engine.h"
#include "src/model_renderer.h"

/*
This file covers these classes of moves:
    - moves that change the hue of the user (rage -> red, recover -> rainbow)
    - moves that change the hue of the target (psybeam -> rainbow, fire punch -> red, acid -> green)
    - moves that change the hue of the arena (OHKO -> pitch black, ice beam -> aqua, earthquake -> red)
    - cutting moves (cut, scratch, vine whip)


as well as a specific "flash" effect (descriptor25) used by these moves:
    - Ice beam
    - ember
    - Blizzard
    - psybeam
    - Aurora beam
    - hyper beam
    - solarbeam 
    - thunder shock
    - Thunderbolt
    - thunder
    - confusion
    - psychic
    - night shade
    - confuse ray
    - light screen
    - reflect
    - self-destruct
    - egg bomb
    - bone club
    - flash 
    - crabhammer
    - explosion
    - tri attack


Also:
    - pokeball flash effect (hue change)
*/

void BattleAnim_SetupGlobalTintColorFadeDetach(Particle* arg0) {
    BattleAnim_LoadGlobalTintToParticle(arg0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackGlobalTintColorFadeDetach(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupGlobalTintColorFadeDetach(arg0);
            break;

        case 2:
            if (ParticleGfx_ApproachPrimPalette(arg0, arg0->unk_CF, Particle_FieldAA_GetS16_0C068(arg0)) != 0) {
                Particle_AdvanceLifecycle(arg0);
            }
            break;

        case 3:
            Particle_DisableAndDetach(arg0);
            break;
    }

    BattleAnim_SetGlobalTintFromParticle(arg0);
}

void BattleAnim_SetupOwnerModelTintRampDetach(Particle* arg0) {
    ParticleGfx_RecolorPrim(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_FieldAE_SetS16_0BF4C(arg0, arg0->unk_A6);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackOwnerModelTintRampDetach(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupOwnerModelTintRampDetach(arg0);
            break;

        case 2:
            if (Particle_FieldAE_Approach_0BF60(arg0, arg0->unk_CD, arg0->unk_AA) != 0) {
                Particle_AdvanceLifecycle(arg0);
            }
            break;

        case 3:
            Particle_DisableAndDetach(arg0);
            break;
    }

    if (Particle_IsRunning(arg0) != 0) {
        Model_SetMaterialColor(&arg0->pokemon->displayObject, arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->unk_AE);
    }
}

void BattleAnim_SetupOwnerModelTintPulseDetach(Particle* arg0) {
    ParticleGfx_RecolorPrim(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_FieldAE_SetS16_0BF4C(arg0, arg0->unk_CE);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackOwnerModelTintPulseDetach(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupOwnerModelTintPulseDetach(arg0);
            break;

        case 2:
            if (Particle_FieldAE_Approach_0BF60(arg0, arg0->unk_CD, arg0->unk_AA) != 0) {
                Particle_AdvanceLifecycle(arg0);
            }
            break;

        case 3:
            Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0);
            break;

        case 4:
            if (Particle_FieldAE_Approach_0BF60(arg0, 0, arg0->unk_AA) != 0) {
                Particle_AdvanceLifecycle(arg0);
            }
            break;

        case 5:
            Particle_DisableAndDetach(arg0);
            break;
    }

    if (Particle_IsRunning(arg0) != 0) {
        Model_SetMaterialColor(&arg0->pokemon->displayObject, arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->unk_AE);
    }
}

void func_8433209C(void) {
}

void BattleAnim_CallbackOwnerModelTintHoldFadeDetach(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupOwnerModelTintPulseDetach(arg0);
            break;

        case 2:
            Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0);
            break;

        case 3:
            if (Particle_FieldAE_Approach_0BF60(arg0, 0, arg0->unk_AA) != 0) {
                Particle_AdvanceLifecycle(arg0);
            }
            break;

        case 4:
            Particle_DisableAndDetach(arg0);
            break;
    }

    if (Particle_IsRunning(arg0) != 0) {
        Model_SetMaterialColor(&arg0->pokemon->displayObject, arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->unk_AE);
    }
}

// better name: BattleAnim_SetupBeamBoomWaterfallHueChange
/*
    sets up hue changes for being hit by:
        - boom (self-destruct and explosion)
        - a few water moves (Hydro pump, bubblebeam, water fall)
        - Hyper Beam
        - Egg Bomb
        - Solar Beam
*/
void BattleAnim_SetupOwnerModelTintPaletteApproachInactive(Particle* arg0) {
    ParticleGfx_RecolorPrim(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_FieldAE_SetS16_0BF4C(arg0, arg0->unk_A6);
    Particle_AdvanceLifecycle(arg0);
}

// better name: BattleAnim_RunBeamBoomWaterfallHueChange
/*
    handles hue changes for being hit by:
        - boom (self-destruct and explosion)
        - a few water moves (Hydro pump, bubblebeam, waterfall)
        - Hyper Beam
        - Egg Bomb
        - Solar Beam
*/
void BattleAnim_CallbackOwnerModelTintPaletteApproachInactive(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupOwnerModelTintPaletteApproachInactive(arg0);
            break;

        case 2:
            if (ParticleGfx_ApproachPrimPalette(arg0, arg0->unk_CD, arg0->unk_AA) != 0) {
                Particle_FieldAE_SetS16_0BF4C(arg0, 0);
                Particle_AdvanceLifecycle(arg0);
            }
            break;

        case 3:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRunning(arg0) != 0) {
        Model_SetMaterialColor(&arg0->pokemon->displayObject, arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->unk_AE);
    }
}

void BattleAnim_SetupOwnerModelTintPaletteApproach(Particle* arg0) {
    ParticleGfx_RecolorPrim(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_FieldAE_SetS16_0BF4C(arg0, arg0->unk_A6);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackOwnerModelTintPaletteApproach(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupOwnerModelTintPaletteApproach(arg0);
            break;

        case 2:
            if (ParticleGfx_ApproachPrimPalette(arg0, arg0->unk_CD, arg0->unk_AA) != 0) {
                Particle_AdvanceLifecycle(arg0);
            }
            break;

        case 3:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRunning(arg0) != 0) {
        Model_SetMaterialColor(&arg0->pokemon->displayObject, arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->unk_AE);
    }
}

// Better name: BattleAnim_SetupIceBeamBlizzardHitHueChange
/*
    Sets the hue coloration change of a pokemon hit by ice beam or blizzard
*/
void BattleAnim_SetupOwnerModelTintPaletteCycle(Particle* particle) {
    ParticleGfx_RecolorPrim(particle, Particle_FieldCF_GetS16_0C058(particle));
    Particle_FieldAE_SetS16_0BF4C(particle, 0);
    Particle_AdvanceLifecycle(particle);
}

// Better name: BattleAnim_RunIceBeamBlizzardHitHueChange
/*
    Runs the hue coloration of a pokemon hit by ice beam or blizzard
*/
void BattleAnim_CallbackOwnerModelTintPaletteCycle(Particle* particle) {
    switch (particle->lifecycle) {
        case 1:
            BattleAnim_SetupOwnerModelTintPaletteCycle(particle);
            break;

        case 2:
            Particle_FieldAE_ApproachAndStepState_0BF98(particle, particle->unk_A6, particle->unk_CE);
            break;

        case 3:
            if (ParticleGfx_ApproachPrimPalette(particle, particle->unk_CD, particle->unk_AA) != 0) {
                Particle_AdvanceLifecycle(particle);
            }
            break;

        case 4:
            Particle_FieldAE_ApproachAndStepState_0BF98(particle, 0, particle->unk_CE);
            break;

        case 5:
            Particle_MarkInactive(particle);
            break;
    }

    if (Particle_IsRunning(particle) != 0) {
        Model_SetMaterialColor(&particle->pokemon->displayObject, particle->primRed, particle->primGreen, particle->primBlue, particle->unk_AE);
    }
}

void BattleAnim_SetupOwnerModelTintPaletteApproachRampDown(Particle* arg0) {
    ParticleGfx_RecolorPrim(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_FieldAE_SetS16_0BF4C(arg0, arg0->unk_A6);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackOwnerModelTintPaletteApproachRampDown(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupOwnerModelTintPaletteApproachRampDown(arg0);
            break;

        case 2:
            if (ParticleGfx_ApproachPrimPalette(arg0, arg0->unk_CD, arg0->unk_AA) != 0) {
                Particle_AdvanceLifecycle(arg0);
            }
            break;

        case 3:
            Particle_FieldAE_ApproachAndStepState_0BF98(arg0, 0, arg0->unk_CE);
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRunning(arg0) != 0) {
        Model_SetMaterialColor(&arg0->pokemon->displayObject, arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->unk_AE);
    }
}

// better name: BattleAnim_SetupDrainFlameHueChange
/*
    Sets up the hue change upon being hit by
        - A Draining move (absorb, mega drain, leech life)
        - A flame-emitting move (Flamethrower, Fire blast, fire spin, dragon rage)
*/
void BattleAnim_SetupOwnerModelTintPaletteFade(Particle* arg0) {
    ParticleGfx_RecolorPrim(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_FieldAE_SetS16_0BF4C(arg0, 0);
    Particle_AdvanceLifecycle(arg0);
}

// better name: BattleAnim_RunDrainFlameHueChange
/*
    Runs the hue change upon being hit by
        - A Draining move (absorb, mega drain, leech life)
        - A flame-emitting move (Flamethrower, Fire blast, fire spin, dragon rage)
*/
void BattleAnim_CallbackOwnerModelTintPaletteFade(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupOwnerModelTintPaletteFade(arg0);
            break;

        case 2:
            if (Particle_FieldAE_Approach_0BF60(arg0, arg0->unk_A6, arg0->unk_CE) != 0) {
                Particle_AdvanceLifecycle(arg0);
            }
            break;

        case 3:
            if (ParticleGfx_ApproachPrimPalette(arg0, arg0->unk_CD, arg0->unk_AA) != 0) {
                Particle_AdvanceLifecycle(arg0);
            }
            break;

        case 4:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRunning(arg0) != 0) {
        Model_SetMaterialColor(&arg0->pokemon->displayObject, arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->unk_AE);
    }
}

void BattleAnim_SetupOwnerModelTintRandomPaletteCycle(Particle* arg0) {
    ParticleGfx_RecolorPrim(arg0, sParticlePaletteSetA[ParticleMath_RandomRange(6)]);
    Particle_FieldCF_SetU8_0BEEC(arg0, sParticlePaletteSetA[ParticleMath_RandomRange(6)]);
    Particle_SetPrimAlpha(arg0, arg0->unk_CD);

    if (arg0->unk_AA == 0) {
        arg0->unk_AA = 1;
    }

    Particle_FieldA6_SetS16_0BCA8(arg0, 0xFF / arg0->unk_AA);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackOwnerModelTintRandomPaletteCycle(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupOwnerModelTintRandomPaletteCycle(arg0);
            break;

        case 2:
            ParticleGfx_ApproachPrimPalette(arg0, arg0->unk_CF, arg0->unk_AA);
            if (Particle_FieldA6_DecrementToZeroStep1_0BDAC(arg0) != 0) {
                Particle_FieldA6_SetS16_0BCA8(arg0, 0xFF / arg0->unk_AA);
                Particle_FieldCF_SetU8_0BEEC(arg0, sParticlePaletteSetA[ParticleMath_RandomRange(6)]);
            }
            Model_SetMaterialColor(&arg0->pokemon->displayObject, arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->primAlpha);
            break;
    }
}

// better name: BattleAnim_SetupRainbowHueChange
/*
    Sets up the rainbow hue shift triggered by the user's Recover or by an incoming Psybeam or Aurora Beam.
*/ 
void BattleAnim_SetupOwnerModelTintPaletteRotateFadeDetach(Particle* particle) {
    ParticleGfx_RecolorPrim(particle, sParticlePaletteSetA[0]);
    Particle_FieldAE_SetS16_0BF4C(particle, 1);
    Particle_FieldCF_SetU8_0BEEC(particle, sParticlePaletteSetA[1]);
    Particle_SetPrimAlpha(particle, particle->unk_CD);

    if (particle->unk_AA == 0) {
        particle->unk_AA = 1;
    }

    Particle_FieldA8_SetS16_0BE14(particle, 0xFF / particle->unk_AA);
    Particle_AdvanceLifecycle(particle);
}

// better name: BattleAnim_RunRainbowHueChange
/*
    Runs the rainbow hue shift triggered by the user's Recover or by an incoming Psybeam or Aurora Beam.
*/ 
void BattleAnim_CallbackOwnerModelTintPaletteRotateFadeDetach(Particle* particle) {
    switch (particle->lifecycle) {
        case 1:
            BattleAnim_SetupOwnerModelTintPaletteRotateFadeDetach(particle);
            break;

        case 2:
            Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(particle);
            if (Particle_FieldA8_DecrementToZeroStep1_0BE6C(particle) != 0) {
                Particle_FieldA8_SetS16_0BE14(particle, 0xFF / particle->unk_AA);
                particle->unk_AE = (particle->unk_AE + 1) % 6;
                Particle_FieldCF_SetU8_0BEEC(particle, sParticlePaletteSetA[particle->unk_AE]);
            }
            ParticleGfx_ApproachPrimPalette(particle, particle->unk_CF, particle->unk_AA);
            Model_SetMaterialColor(&particle->pokemon->displayObject, particle->primRed, particle->primGreen, particle->primBlue, particle->primAlpha);
            break;

        case 3:
            if (Particle_PrimAlpha_Approach_0BC2C(particle, 0, 8) != 0) {
                Particle_AdvanceLifecycle(particle);
            }
            Model_SetMaterialColor(&particle->pokemon->displayObject, particle->primRed, particle->primGreen, particle->primBlue, particle->primAlpha);
            break;

        case 4:
            Particle_DisableAndDetach(particle);
            break;
    }
}

// better name: BattleAnim_SetupInvisibility
/*
    Set up Fading the user's model to invisible for Teleport, defense curl, sky attack, acid armor and sharpen.
    These moves do not replace the user's model like transform or substitute and do not have unique animation varients like withdrawal;
    which are not covered by this function.
*/
void BattleAnim_SetupOwnerModelAlphaFade(Particle* arg0) {
    arg0->primAlpha = arg0->unk_A6;
    Particle_AdvanceLifecycle(arg0);
}

// better name: BattleAnim_RunInvisibility
/*
    Set up Fading the user's model to invisible for Teleport, defense curl, sky attack, acid armor and sharpen.
    These moves do not replace the user's model like transform or substitute and do not have unique animation varients like withdrawal;
    which are not covered by this function.
*/
void BattleAnim_CallbackOwnerModelAlphaFade(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupOwnerModelAlphaFade(arg0);
            break;

        case 2:
            if (Particle_PrimAlpha_Approach_0BC2C(arg0, arg0->unk_CD, arg0->unk_AA) != 0) {
                Particle_AdvanceLifecycle(arg0);
            }
            break;

        case 3:
            Particle_MarkInactive(arg0);
            break;
    }

    if (Particle_IsRunning(arg0) != 0) {
        Model_SetMaterialAlpha(&arg0->pokemon->displayObject, arg0->primAlpha);
    }
}

void BattleAnim_SetupParticleRecolorTransparent(Particle* arg0) {
    ParticleGfx_RecolorPrim(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_SetPrimAlpha(arg0, 0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackStartEffect125OwnerColorAlphaFade(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupParticleRecolorTransparent(arg0);
            break;

        case 2:
            Particle_PrimAlpha_Approach_0BC2C(arg0, 0x80, 4);
            break;

        case 3:
            if (Particle_PrimAlpha_Approach_0BC2C(arg0, 0, 8) != 0) {
                Particle_DisableAndDetach(arg0);
            }
            break;
    }

    Model_SetMaterialColor(&arg0->pokemon->displayObject, arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->primAlpha);
}

// Better name: BattleAnim_SetupWhiteHueChange
/*
    Sets up coloration changes to pure white for using Blizzard, Confuse Ray, Self-destruct, Flash, or Explosion.
    Blizzard's effect is more subtle and doesn't change the user completely white like the other listed moves.
    It is used in blizzard to make the user's hue match that of the arena.
    Also used for the poke ball's hue change.
*/
void BattleAnim_SetupDescriptor25AlphaFadeInHoldOut(Particle* particle) {
    ParticleGfx_RecolorPrim(particle, Particle_FieldCF_GetS16_0C058(particle));
    Particle_SetPrimAlpha(particle, 0);
    Particle_AdvanceLifecycle(particle);
}

// Better name: BattleAnim_RunWhiteHueChange
/*
    Runs coloration changes to pure white for using Blizzard, Confuse Ray, Self-destruct, Flash, or Explosion.
    Blizzard's effect is more subtle and doesn't change the user completely white like the other listed moves.
    It is used in blizzard to make the user's hue match that of the arena.
    Also used for the poke ball's hue change.
*/
void BattleAnim_CallbackDescriptor25AlphaFadeInHoldOut(Particle* particle) {
    switch (particle->lifecycle) {
        case 1:
            BattleAnim_SetupDescriptor25AlphaFadeInHoldOut(particle);
            break;

        case 2:
            Particle_PrimAlpha_ApproachAndStepState_0BC5C(particle, particle->unk_CD, particle->unk_AA);
            break;

        case 3:
            Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(particle);
            break;

        case 4:
            if (Particle_PrimAlpha_Approach_0BC2C(particle, 0, particle->unk_AA) != 0) {
                Particle_MarkInactive(particle);
            }
            break;
    }
}

void BattleAnim_SetupDescriptor25AlphaHoldFadeOut(Particle* arg0) {
    ParticleGfx_RecolorPrim(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_SetPrimAlpha(arg0, arg0->unk_CD);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackDescriptor25AlphaHoldFadeOut(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupDescriptor25AlphaHoldFadeOut(arg0);
            break;

        case 2:
            Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0);
            break;

        case 3:
            if (Particle_PrimAlpha_Approach_0BC2C(arg0, 0, arg0->unk_AA) != 0) {
                Particle_MarkInactive(arg0);
            }
            break;
    }
}

void BattleAnim_SetupDescriptor25AlphaFadeInHold(Particle* arg0) {
    ParticleGfx_RecolorPrim(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_SetPrimAlpha(arg0, 0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackDescriptor25AlphaFadeInHold(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupDescriptor25AlphaFadeInHold(arg0);
            break;

        case 2:
            Particle_PrimAlpha_ApproachAndStepState_0BC5C(arg0, arg0->unk_CD, arg0->unk_AA);
            break;

        case 3:
            if (Particle_FieldA6_DecrementToZeroStep1_0BDAC(arg0) != 0) {
                Particle_MarkInactive(arg0);
            }
            break;
    }
}

void BattleAnim_SetupDescriptor25AlphaHoldDetach(Particle* arg0) {
    ParticleGfx_RecolorPrim(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_SetPrimAlpha(arg0, arg0->unk_CD);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackDescriptor25AlphaHoldDetach(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupDescriptor25AlphaHoldDetach(arg0);
            break;

        case 2:
            if (Particle_FieldA6_DecrementToZeroStep1_0BDAC(arg0) != 0) {
                Particle_DisableAndDetach(arg0);
            }
            break;
    }
}

void BattleAnim_SetupDescriptor25PaletteFadeOut(Particle* arg0) {
    ParticleGfx_RecolorPrim(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_SetPrimAlpha(arg0, 0xFF);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackDescriptor25PaletteFadeOut(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupDescriptor25PaletteFadeOut(arg0);
            break;

        case 2:
            Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(arg0);
            break;

        case 3:
            ParticleGfx_ApproachPrimPalette(arg0, arg0->unk_CD, arg0->unk_AA);
            if (Particle_PrimAlpha_Approach_0BC2C(arg0, 0, arg0->unk_AA) != 0) {
                Particle_MarkInactive(arg0);
            }
            break;
    }
}

void BattleAnim_SetupDescriptor25PaletteFadeIn(Particle* arg0) {
    ParticleGfx_RecolorPrim(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_SetPrimAlpha(arg0, 0);
    Particle_AdvanceLifecycle(arg0);
}

void BattleAnim_CallbackDescriptor25PaletteFadeIn(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupDescriptor25PaletteFadeIn(arg0);
            break;

        case 2:
            ParticleGfx_ApproachPrimPalette(arg0, arg0->unk_CD, arg0->unk_AA);
            Particle_PrimAlpha_ApproachAndStepState_0BC5C(arg0, 0xFF, arg0->unk_AA);
            break;

        case 3:
            if (Particle_FieldA6_DecrementToZeroStep1_0BDAC(arg0) != 0) {
                Particle_MarkInactive(arg0);
            }
            break;
    }
}
