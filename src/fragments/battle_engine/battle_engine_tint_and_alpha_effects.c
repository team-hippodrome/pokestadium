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

/*
    Moves: Absorb (use), Aurora Beam (use/hit), Barrier (use), Bide (use), Blizzard (use/hit), Bubble Beam
    (use/hit), Confuse Ray (use/hit), Confusion (use/hit), Conversion (use), Double-Edge (use), Dragon Rage
    (use), Dream Eater (use), Earthquake (use), Explosion (use/hit), Fire Blast (use/hit), Fire Spin (use),
    Fissure (use), Flamethrower (use), Flash (use/hit), Focus Energy (use), Glare (use), Guillotine (use), Horn
    Drill (use), Hydro Pump (use), Hyper Beam (use), Ice Beam (use/hit), Leech Life (use), Leer (use), Light
    Screen (use), Lovely Kiss (use), Mega Drain (use), Mimic (use), Night Shade (use), Petal Dance (use),
    Psybeam (use/hit), Psychic (use/hit), Psywave (use), Rage (use), Razor Leaf (use/hit), Recover (use),
    Reflect (use), Self-Destruct (use/hit), Skull Bash (use), Sky Attack (use/hit), Solar Beam (use/hit), Sonic
    Boom (use), Swift (use), Take Down (use), Thunder (use/hit), Thunder Shock (use/hit), Thunder Wave (hit),
    Thunderbolt (use/hit), Tri Attack (use)
    TODO: Human contributor add screenshot
*/
void BattleAnim_SetupBackgroundRecolor(Particle* arg0) {
    BattleAnim_LoadGlobalTintToParticle(arg0);
    Particle_AdvanceLifecycle(arg0);
}

/*
    recolors background (everything but the pokemon)
    Examples:
        - OHKO moves -> Black
        - ice beam -> aqua
        - earthquake -> red
*/
void BattleAnim_RunBackgroundRecolor(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupBackgroundRecolor(arg0);
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

/*
    Moves: Acid (hit), Acid Armor (use), Confuse Ray (use/hit), Confusion (hit), Defense Curl (use), Dragon Rage
    (hit), Ember (hit), Explosion (use/hit), Fire Punch (hit), Flamethrower (hit), Flash (use/hit), Guillotine
    (hit), Harden (use), Horn Drill (hit), Hyper Beam (use/hit), Ice Punch (hit), Lick (hit), Mega Kick (hit),
    Mega Punch (hit), Night Shade (hit), Psychic (hit), Self-Destruct (use/hit), Sharpen (use), Skull Bash
    (hit), Sky Attack (use), Sludge (hit), Solar Beam (use), Substitute (use), Thunder Punch (hit), Toxic (hit),
    Transform (use)
    TODO: Human contributor add screenshot
*/
void BattleAnim_SetupTargetHueChange(Particle* arg0) {
    ParticleGfx_RecolorPrim(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_FieldAE_SetS16_0BF4C(arg0, arg0->unk_A6);
    Particle_AdvanceLifecycle(arg0);
}

/*
    handles changing the hue for the target
    examples:
        - ice punch -> aqua
        - fire blast -> charcol
        - Psybeam -> rainbow
        - acid -> light green
    Also handles move changes when target faints from a move
*/
void BattleAnim_RunTargetHueChange(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupTargetHueChange(arg0);
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
        Model_SetMaterialColor(&arg0->pokemon->displayObject, arg0->primRed, arg0->primGreen, arg0->primBlue,
                               arg0->unk_AE);
    }
}

/*
    Moves: Barrier (use), Bide (use), Confusion (use), Focus Energy (use), Growth (use), Light Screen (use),
    Lovely Kiss (hit), Poison Gas (hit), Poison Powder (hit), Psychic (use), Psywave (hit), Rage (use), Reflect
    (use), Sleep Powder (hit), Smog (hit), Soft-Boiled (use), Spore (hit), Struggle (use), Stun Spore (hit),
    Thrash (use)
    TODO: Human contributor add screenshot
*/
void BattleAnim_SetupUserHueChange(Particle* arg0) {
    ParticleGfx_RecolorPrim(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_FieldAE_SetS16_0BF4C(arg0, arg0->unk_CE);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Changes User's Hue. Examples:
        - Thrash -> red
        - Recover -> rainbow
*/
void BattleAnim_RunUserHueChange(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupUserHueChange(arg0);
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
        Model_SetMaterialColor(&arg0->pokemon->displayObject, arg0->primRed, arg0->primGreen, arg0->primBlue,
                               arg0->unk_AE);
    }
}

void func_8433209C(void) {
}

void BattleAnim_CallbackOwnerModelTintHoldFadeDetach(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupUserHueChange(arg0);
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
        Model_SetMaterialColor(&arg0->pokemon->displayObject, arg0->primRed, arg0->primGreen, arg0->primBlue,
                               arg0->unk_AE);
    }
}

/*
    sets up hue changes for being hit by:
        - boom (self-destruct and explosion)
        - a few water moves (Hydro pump, bubblebeam, water fall)
        - Hyper Beam
        - Egg Bomb
        - Solar Beam
*/
void BattleAnim_SetupBeamBoomWaterfallHueChange(Particle* arg0) {
    ParticleGfx_RecolorPrim(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_FieldAE_SetS16_0BF4C(arg0, arg0->unk_A6);
    Particle_AdvanceLifecycle(arg0);
}

/*
    handles hue changes for being hit by:
        - boom (self-destruct and explosion)
        - a few water moves (Hydro pump, bubblebeam, waterfall)
        - Hyper Beam
        - Egg Bomb
        - Solar Beam
*/
void BattleAnim_RunBeamBoomWaterfallHueChange(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupBeamBoomWaterfallHueChange(arg0);
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
        Model_SetMaterialColor(&arg0->pokemon->displayObject, arg0->primRed, arg0->primGreen, arg0->primBlue,
                               arg0->unk_AE);
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
        Model_SetMaterialColor(&arg0->pokemon->displayObject, arg0->primRed, arg0->primGreen, arg0->primBlue,
                               arg0->unk_AE);
    }
}

/*
    Sets the hue coloration change of a pokemon hit by ice beam or blizzard
*/
void BattleAnim_SetupIceBeamBlizzardHitHueChange(Particle* particle) {
    ParticleGfx_RecolorPrim(particle, Particle_FieldCF_GetS16_0C058(particle));
    Particle_FieldAE_SetS16_0BF4C(particle, 0);
    Particle_AdvanceLifecycle(particle);
}

/*
    Runs the hue coloration of a pokemon hit by ice beam or blizzard
*/
void BattleAnim_RunIceBeamBlizzardHitHueChange(Particle* particle) {
    switch (particle->lifecycle) {
        case 1:
            BattleAnim_SetupIceBeamBlizzardHitHueChange(particle);
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
        Model_SetMaterialColor(&particle->pokemon->displayObject, particle->primRed, particle->primGreen,
                               particle->primBlue, particle->unk_AE);
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
        Model_SetMaterialColor(&arg0->pokemon->displayObject, arg0->primRed, arg0->primGreen, arg0->primBlue,
                               arg0->unk_AE);
    }
}

/*
    Sets up the hue change upon being hit by
        - A Draining move (absorb, mega drain, leech life)
        - A flame-emitting move (Flamethrower, Fire blast, fire spin, dragon rage)
*/
void BattleAnim_SetupDrainFlameHueChange(Particle* arg0) {
    ParticleGfx_RecolorPrim(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_FieldAE_SetS16_0BF4C(arg0, 0);
    Particle_AdvanceLifecycle(arg0);
}

/*
    Runs the hue change upon being hit by
        - A Draining move (absorb, mega drain, leech life)
        - A flame-emitting move (Flamethrower, Fire blast, fire spin, dragon rage)
*/
void BattleAnim_RunDrainFlameHueChange(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupDrainFlameHueChange(arg0);
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
        Model_SetMaterialColor(&arg0->pokemon->displayObject, arg0->primRed, arg0->primGreen, arg0->primBlue,
                               arg0->unk_AE);
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
            Model_SetMaterialColor(&arg0->pokemon->displayObject, arg0->primRed, arg0->primGreen, arg0->primBlue,
                                   arg0->primAlpha);
            break;
    }
}

/*
    Sets up the rainbow hue shift triggered by the user's Recover or by an incoming Psybeam or Aurora Beam.
*/
void BattleAnim_SetupRainbowHueChange(Particle* particle) {
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

/*
    Runs the rainbow hue shift triggered by the user's Recover or by an incoming Psybeam or Aurora Beam.
*/
void BattleAnim_RunRainbowHueChange(Particle* particle) {
    switch (particle->lifecycle) {
        case 1:
            BattleAnim_SetupRainbowHueChange(particle);
            break;

        case 2:
            Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(particle);
            if (Particle_FieldA8_DecrementToZeroStep1_0BE6C(particle) != 0) {
                Particle_FieldA8_SetS16_0BE14(particle, 0xFF / particle->unk_AA);
                particle->unk_AE = (particle->unk_AE + 1) % 6;
                Particle_FieldCF_SetU8_0BEEC(particle, sParticlePaletteSetA[particle->unk_AE]);
            }
            ParticleGfx_ApproachPrimPalette(particle, particle->unk_CF, particle->unk_AA);
            Model_SetMaterialColor(&particle->pokemon->displayObject, particle->primRed, particle->primGreen,
                                   particle->primBlue, particle->primAlpha);
            break;

        case 3:
            if (Particle_PrimAlpha_Approach_0BC2C(particle, 0, 8) != 0) {
                Particle_AdvanceLifecycle(particle);
            }
            Model_SetMaterialColor(&particle->pokemon->displayObject, particle->primRed, particle->primGreen,
                                   particle->primBlue, particle->primAlpha);
            break;

        case 4:
            Particle_DisableAndDetach(particle);
            break;
    }
}

/*
    Set up Fading the user's model to invisible for Teleport, defense curl, sky attack, acid armor and sharpen.
    These moves do not replace the user's model like transform or substitute and do not have unique animation varients
   like withdrawal; which are not covered by this function.
*/
void BattleAnim_SetupInvisibility(Particle* arg0) {
    arg0->primAlpha = arg0->unk_A6;
    Particle_AdvanceLifecycle(arg0);
}

/*
    Set up Fading the user's model to invisible for Teleport, defense curl, sky attack, acid armor and sharpen.
    These moves do not replace the user's model like transform or substitute and do not have unique animation varients
   like withdrawal; which are not covered by this function.
*/
void BattleAnim_RunInvisibility(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupInvisibility(arg0);
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

    Model_SetMaterialColor(&arg0->pokemon->displayObject, arg0->primRed, arg0->primGreen, arg0->primBlue,
                           arg0->primAlpha);
}

/*
    Sets up coloration changes to pure white for using Blizzard, Confuse Ray, Self-destruct, Flash, or Explosion.
    Blizzard's effect is more subtle and doesn't change the user completely white like the other listed moves.
    It is used in blizzard to make the user's hue match that of the arena.
    Also used for the poke ball's hue change.
*/
void BattleAnim_SetupWhiteHueChange(Particle* particle) {
    ParticleGfx_RecolorPrim(particle, Particle_FieldCF_GetS16_0C058(particle));
    Particle_SetPrimAlpha(particle, 0);
    Particle_AdvanceLifecycle(particle);
}

/*
    Runs coloration changes to pure white for using Blizzard, Confuse Ray, Self-destruct, Flash, or Explosion.
    Blizzard's effect is more subtle and doesn't change the user completely white like the other listed moves.
    It is used in blizzard to make the user's hue match that of the arena.
    Also used for the poke ball's hue change.
*/
void BattleAnim_RunWhiteHueChange(Particle* particle) {
    switch (particle->lifecycle) {
        case 1:
            BattleAnim_SetupWhiteHueChange(particle);
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

/*
    Moves: Bone Club (use), Confuse Ray (use/hit), Confusion (hit), Crabhammer (use/hit), Cut (use/hit), Egg
    Bomb (hit), Ember (use/hit), Explosion (use), Fire Punch (hit), Flash (use/hit), Fury Swipes (use/hit),
    Night Shade (use/hit), Psychic (hit), Scratch (use/hit), Self-Destruct (use), Slash (use/hit), Thunder
    (use/hit), Thunder Punch (hit), Thunder Shock (use/hit), Thunderbolt (use/hit), Vine Whip (use/hit)
    TODO: Human contributor add screenshot
*/
void BattleAnim_SetupFullScreenHueChange(Particle* arg0) {
    ParticleGfx_RecolorPrim(arg0, Particle_FieldCF_GetS16_0C058(arg0));
    Particle_SetPrimAlpha(arg0, arg0->unk_CD);
    Particle_AdvanceLifecycle(arg0);
}

/*
    handles full screen hue overlay changes to a single color. Examples:
        - Vine whip -> green
        - Slash -> red
        - Fire punch -> white

    BattleAnim_DrawFadingFullScreenColorOverlay in @battle_engine_particle_renderers.c is the equivalent for moves
    that change both the full screen hue and the pokemon hue at the same time
*/
void BattleAnim_RunFullScreenHueChange(Particle* arg0) {
    switch (arg0->lifecycle) {
        case 1:
            BattleAnim_SetupFullScreenHueChange(arg0);
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

/*
    Used to change the Pokemon and screen hue at the same time for Blizzard, Boom, Confuse Ray, and Flash
*/
void BattleAnim_RunPokemonAndFullScreenHueChange(Particle* arg0) {
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
