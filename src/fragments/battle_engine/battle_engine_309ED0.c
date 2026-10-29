#include "battle_engine.h"
#include "src/model_animation.h"
#include "src/model_renderer.h"
#include "src/audio_commands.h"
#include "src/3D140.h"
#include "src/audio_fade.h"
#include "src/audio_loop_point.h"
#include "src/cry.h"
#include "src/matrix.h"
#include "pokemon.h"
#include "moves.h"
#include "status.h"

ActivePokemon* sNonActingPokemon;
ActivePokemon* sActingPokemon;
static f32 D_84390208;
static s32 pad_D_84390210[4];
static s32 sSide1HpConsistent;
static s32 sSide2HpConsistent;
static s32 sNormalOrFlyingAnimationRNG;
s32 sMultihitCount;
s32 sMultihitCountdown;
s32 D_84390234;

u8 D_84385B70[] = {
    0x00, 0x03, 0x02, 0x06, 0x07,
};
u8 D_84385B78[] = {
    0x00,
    0x02,
    0x06,
    0x07,
};
u8 D_84385B7C[] = {
    0x02,
    0x06,
    0x07,
};
u8 D_84385B80[] = {
    0x04, 0x05, 0x08, 0x09, 0x0A, 0x0B,
};
u8 D_84385B88[] = {
    0x05,
    0x09,
    0x0A,
    0x0B,
};
u16 D_84385B8C = 0x5F92;
Vec3f s100UnitsUpFromCenter = { 0.0f, 100.0f, 0.0f };
Vec3f s10UnitsUpFromCenter = { 0.0f, 10.0f, 0.0f };

void BattleScene_ApplyModelAnimationSlot5A(ActivePokemon* pokemon) {
    ActivePokemonState* state = &pokemon->state;

    if ((pokemon->displayObject.objectType == DIGLETT) && (state->flags & ACTIVE_UNDERGROUND)) {
        MovePresentationCue* move_cue = &sSideBattleCues[BattleScene_GetParticipantSideIndex(pokemon)]->moveCues[FISSURE];

        ModelAnim_SetAnimation(&pokemon->displayObject, move_cue->id);
        pokemon->displayObject.node.renderFlags &= ~0x20;
    }
}

void BattleScene_ResetAnimationStep(void) {
    sBattleScene.scene->updateCounter = 0;
}

s32 BattleScene_AdvanceOwnerAnimation(ActivePokemon* pokemon) {
    ActivePokemonState* state = &pokemon->state;
    UNUSED s32 pad;
    s32 sp1C = 0;

    if (!(state->flags & ACTIVE_SUBSTITUTE)) {
        return 0;
    }

    if (sBattleScene.scene->moveEffectKind == 0x19) {
        return 0;
    }

    if ((state->flags & ACTIVE_SUBSTITUTE) && (sBattleScene.scene->moveEffectKind == 0x1C)) {
        return 0;
    }

    switch (sBattleScene.scene->updateCounter) {
        case 0:
            BattleScene_DestroySubstitute(pokemon);
            sp1C = 1;
            sBattleScene.scene->updateCounter++;
            break;

        case 1:
            sp1C = 1;
            if (pokemon->sessionTeams->uniqueStatusFlags & 0x10) {
                pokemon->sessionTeams->uniqueStatusFlags &= ~0x90;
                Battle_ReloadDisplayModel(pokemon);
                BattleScene_ApplyModelAnimationSlot5A(pokemon);
                sBattleScene.scene->updateCounter++;
            }
            break;

        case 2:
            BattleScene_ResetAnimationStep();
            break;
    }
    return sp1C;
}

s32 BattleScene_AdvanceOwnerAnimation98(ActivePokemon* arg0) {
    UNUSED s32 pad[2];
    s32 sp1C = 0;

    if (!(arg0->state.flags & 0x800)) {
        return 0;
    }

    switch (sBattleScene.scene->updateCounter) {
        case 0:
            Battle_MakeSubstitute(arg0);
            sp1C = 1;
            sBattleScene.scene->updateCounter++;
            break;

        case 1:
            sp1C = 1;
            if (arg0->sessionTeams->uniqueStatusFlags & 0x10) {
                arg0->sessionTeams->uniqueStatusFlags &= ~0x90;
                Battle_ReloadSubstituteDoll(arg0);
                sBattleScene.scene->updateCounter++;
            }
            break;

        case 2:
            BattleScene_ResetAnimationStep();
            break;
    }
    return sp1C;
}

s32 BattleScene_AdvanceOwnerAnimationAndClearFlags(ActivePokemon* arg0) {
    UNUSED s32 pad[2];
    Vec3f sp34;
    s32 var_s0 = 0;

    if (!(arg0->state.flags & 0x1000)) {
        return 0;
    }

    switch (sBattleScene.scene->updateCounter) {
        case 0:
            BattleScene_DestroySubstitute(arg0);
            var_s0 = 1;
            sBattleScene.scene->updateCounter++;
            break;

        case 1:
            var_s0 = 1;
            if (arg0->sessionTeams->uniqueStatusFlags & 0x10) {
                arg0->sessionTeams->uniqueStatusFlags &= ~0x90;
                Battle_ReloadDisplayModel(arg0);
                Battle_ClearRuntimeFlags(arg0, 0x1800);
                var_s0 = 0;
                BattleScene_ResetAnimationStep();
                sBattleScene.scene->updateCounter++;
            }
            break;

        case 2:
            BattleScene_ResetAnimationStep();
            BattleAnim_GetOwnerDefaultAnchorPosition(arg0, &sp34);
            BattleAnim_Vec3fCopy(&sBattleScene.scene->camera->view.targetPosition, &sp34);
            break;
    }
    return var_s0;
}

s32 BattleScene_AdvanceDoublesOwnerAnimation(ActivePokemon* arg0) {
    UNUSED s32 pad[2];
    s32 sp1C = 0;

    if (sBattleScene.scene->unk_44 != 2) {
        return 0;
    }

    if (!(arg0->state.flags & 0x800)) {
        return 0;
    }

    if ((sBattleScene.scene->moveEffectKind == 0x19) || (sBattleScene.scene->moveEffectKind == 0x1C)) {
        return 0;
    }

    switch (sBattleScene.scene->updateCounter) {
        case 0:
            BattleScene_DestroySubstitute(arg0);
            sp1C = 1;
            sBattleScene.scene->updateCounter += 1;
            break;

        case 1:
            sp1C = 1;
            if (arg0->sessionTeams->uniqueStatusFlags & 0x10) {
                arg0->sessionTeams->uniqueStatusFlags &= ~0x90;
                Battle_ReloadDisplayModel(arg0);
                BattleScene_ApplyModelAnimationSlot5A(arg0);
                sBattleScene.scene->updateCounter += 1;
            }
            break;

        case 2:
            BattleScene_ResetAnimationStep();
            break;
    }
    return sp1C;
}

s32 BattleScene_AdvanceBothOwnerAnimations(void) {
    UNUSED s32 pad[2];
    s32 advanced = 0;
    ActivePokemonState* pokemon_state1 = &sActivePokemon[0]->state;
    ActivePokemonState* pokemon_state2 = &sActivePokemon[1]->state;

    if (sBattleScene.scene->unk_44 != 1) {
        return 0;
    }

    if (!(pokemon_state1->flags & ACTIVE_SUBSTITUTE) && !(pokemon_state2->flags & ACTIVE_SUBSTITUTE)) {
        return 0;
    }

    switch (sBattleScene.scene->updateCounter) {
        case 0:
            if (pokemon_state1->flags & ACTIVE_SUBSTITUTE) {
                Battle_MakeSubstitute(sActivePokemon[0]);
            }
            advanced = 1;
            sBattleScene.scene->updateCounter = sBattleScene.scene->updateCounter + 1;
            break;

        case 1:
            advanced = 1;
            if (pokemon_state1->flags & ACTIVE_SUBSTITUTE) {
                if (sActivePokemon[0]->sessionTeams->uniqueStatusFlags & 0x10) {
                    sActivePokemon[0]->sessionTeams->uniqueStatusFlags &= ~0x90;
                    Battle_ReloadSubstituteDoll(sActivePokemon[0]);
                    sBattleScene.scene->updateCounter += 1;
                }
            } else {
                sBattleScene.scene->updateCounter = sBattleScene.scene->updateCounter + 1;
            }
            break;

        case 2:
            if (pokemon_state2->flags & ACTIVE_SUBSTITUTE) {
                Battle_MakeSubstitute(sActivePokemon[1]);
            }
            advanced = 1;
            sBattleScene.scene->updateCounter = sBattleScene.scene->updateCounter + 1;
            break;

        case 3:
            advanced = 1;
            if (pokemon_state2->flags & ACTIVE_SUBSTITUTE) {
                if (sActivePokemon[1]->sessionTeams->uniqueStatusFlags & 0x10) {
                    sActivePokemon[1]->sessionTeams->uniqueStatusFlags &= ~0x90;
                    Battle_ReloadSubstituteDoll(sActivePokemon[1]);
                    sBattleScene.scene->updateCounter += 1;
                }
            } else {
                sBattleScene.scene->updateCounter = sBattleScene.scene->updateCounter + 1;
            }
            break;

        case 4:
            BattleScene_ResetAnimationStep();
            break;
    }

    return advanced;
}

void BattleScene_ClearInactiveOwnerAnimationFlags(void) {
    BattleMonRuntime* pokemon_runtime1 = &sActivePokemon[0]->state.runtime;
    BattleMonRuntime* pokemon_runtime2 = &sActivePokemon[1]->state.runtime;

    if (pokemon_runtime1->isTrapped == 0) {
        pokemon_runtime1->immobilizedReason &= ~IMMOBILIZED_TRAPPING;
    }

    if (pokemon_runtime2->isTrapped == 0) {
        pokemon_runtime2->immobilizedReason &= ~IMMOBILIZED_TRAPPING;
    }
}

void func_8431FCC4(void) {
}

s32 BattleScene_AnimateDisplayedHpToRuntime(ActivePokemon* active_pokemon) {
    BattleMonRuntime* runtime = &active_pokemon->state.runtime;
    BattleMon* party_pokemon = &active_pokemon->teamRoster->pokemonPicked[active_pokemon->state.rosterIndex];
    f32 damage;
    UNUSED s16 pad;
    s16 side;

    if (D_84390288 != 0) {
        return 1;
    }

    if (party_pokemon->hp == runtime->displayHp) {
        return 1;
    }

    damage = runtime->hp / 90.0f;
    side = BattleScene_GetParticipantSideIndex(active_pokemon);
    sCurrentPokemonHp[side] = Math_StepToF(sCurrentPokemonHp[BattleScene_GetParticipantSideIndex(active_pokemon)], runtime->displayHp, damage, damage);
    party_pokemon->hp = sCurrentPokemonHp[BattleScene_GetParticipantSideIndex(active_pokemon)];
    if (party_pokemon->hp == runtime->displayHp) {
        damage = sCurrentPokemonHp[BattleScene_GetParticipantSideIndex(active_pokemon)] - party_pokemon->hp;
        sCurrentPokemonHp[BattleScene_GetParticipantSideIndex(active_pokemon)] -= damage;
        if (active_pokemon->displayObject.objectType == SUBSTITUTE_DOLL) {
            return 1;
        }
        return 1;
    }
    return 0;
}

void BattleScene_ClearPhaseFlag(void) {
    sBattleScene.scene->phaseSubstep = 0;
}

void BattleScene_SetPhaseFlag(void) {
    sBattleScene.scene->phaseSubstep = 1;
}

void BattleScene_SetPhase(UNUSED CameraNode* camera, s32 arg1) {
    sBattleScene.scene->phase = arg1;
    sBattleScene.scene->phaseStep = 0;
}

void BattleScene_SaveCameraFov(CameraNode* camera) {
    sBattleScene.scene->verticalFieldOfView = camera->perspective.verticalFieldOfView;
}

void BattleScene_ResetCameraFov(CameraNode* camera) {
    camera->perspective.verticalFieldOfView = 30.0f;
    sBattleScene.scene->verticalFieldOfView = 30.0f;
}

s32 BattleScene_GetAnimationTimingField0D(ActivePokemon* pokemon) {
    MovePresentationCue* move_cue = &sSideBattleCues[BattleScene_GetParticipantSideIndex(pokemon)]->moveCues[pokemon->state.runtime.usedMove - 1];

    return move_cue->unk_0D;
}

s32 BattleScene_GetMappedAnimationTimingField0D(ActivePokemon* pokemon) {
    UNUSED s32 pad;
    s32 charge_move_index = sChargeMoveCues[pokemon->state.runtime.usedMove - 1];
    MovePresentationCue* move_cue = &sSideBattleCues[BattleScene_GetParticipantSideIndex(pokemon)]->moveCues[charge_move_index];

    return move_cue->unk_0D;
}

s32 BattleScene_GetAnimationTimingField0E(ActivePokemon* pokemon) {
    MovePresentationCue* move_cue = &sSideBattleCues[BattleScene_GetParticipantSideIndex(pokemon)]->moveCues[pokemon->state.runtime.usedMove - 1];

    return move_cue->unk_0E;
}

s32 BattleScene_GetMappedAnimationTimingField0E(ActivePokemon* pokemon) {
    UNUSED s32 pad;
    s32 charge_move_index = sChargeMoveCues[pokemon->state.runtime.usedMove - 1];
    MovePresentationCue* move_cue = &sSideBattleCues[BattleScene_GetParticipantSideIndex(pokemon)]->moveCues[charge_move_index];

    return move_cue->unk_0E;
}

s32 BattleScene_GetCameraShotDuration(ActivePokemon* pokemon) {
    MovePresentationCue* move_cue = &sSideBattleCues[BattleScene_GetParticipantSideIndex(pokemon)]->moveCues[pokemon->state.runtime.usedMove - 1];

    if (move_cue->unk_0F == 0) {
        return 0xF;
    }
    return move_cue->unk_0F;
}

void Battle_StartTurn(void) {
    s32 side;
    ActivePokemonState* state1;
    ActivePokemonState* state2;
    BattleMonRuntime* runtime;

    state1 = &sActivePokemon[0]->state;
    state2 = &sActivePokemon[1]->state;
    BattleScene_SetCameraSubstate(1, 1);

    side = sBattleScene.scene->actingSide;
    sBattleScene.scene->actingSide = Battle_DetermineMoveOrder(sActivePokemon[0], sActivePokemon[1]);

    runtime = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;
    if ((runtime->usedMove & 0xF0) == 0xF0) {
        sBattleScene.scene->actingSide = !sBattleScene.scene->actingSide;
    }

    if (sActivePokemon[0]->teamRoster->isHuman & 2) {
        if (sBattleScene.scene->actingSide == 0) {
            BattleAI_RecordUsedMove(state1->rosterIndex, state2->rosterIndex, 0);
        } else {
            BattleAI_RecordUsedMove(state1->rosterIndex, state2->rosterIndex, 1);
        }
    } else if (sActivePokemon[1]->teamRoster->isHuman & 2) {
        if (sBattleScene.scene->actingSide == 1) {
            BattleAI_RecordUsedMove(state2->rosterIndex, state1->rosterIndex, 0);
        } else {
            BattleAI_RecordUsedMove(state2->rosterIndex, state1->rosterIndex, 1);
        }
    }

    sBattleScene.scene->sideCurrentlySwitching = sBattleScene.scene->actingSide;
    sBattleScene.scene->phase = 0;
    state1->flashTimer = 0;
    state2->flashTimer = 0;
    sBattleScene.scene->unk_44 = 2;
    Battle_ResolveMonAction();
    sBattleScene.scene->actingSide = side;
}

void BattleScene_InitializeSwitchActions(void) {
    s32 side;
    ActivePokemonState* state1;
    ActivePokemonState* state2;

    state1 = &sActivePokemon[0]->state;
    state2 = &sActivePokemon[1]->state;
    BattleScene_SetCameraSubstate(1, 1);

    side = sBattleScene.scene->actingSide;

    if ((state1->replacementState == 0xF) || (state1->replacementState == 0x10)) {
        if (sActivePokemon[0]->teamRoster->isHuman & 2) {
            BattleAI_RecordUsedMove(state1->rosterIndex, state2->rosterIndex, 2);
        }
        sBattleScene.scene->actingSide = 0;
        sBattleScene.scene->sideCurrentlySwitching = 0;
        sBattleScene.scene->phase = 0;
        sBattleScene.scene->unk_44 = 2;
        BeginBattleForEntity(sActivePokemon[0]);
        sBattleScene.scene->actingSide = side;
    } else {
        if (sActivePokemon[0]->teamRoster->isHuman & 2) {
            BattleAI_RecordUsedMove(state2->rosterIndex, state1->rosterIndex, 2);
        }
        sBattleScene.scene->actingSide = 1;
        sBattleScene.scene->sideCurrentlySwitching = 1;
        sBattleScene.scene->phase = 0;
        sBattleScene.scene->unk_44 = 2;
        BeginBattleForEntity(sActivePokemon[1]);
        sBattleScene.scene->actingSide = side;
    }
}

s32 BattleScene_PrepareReplacementTransition(void) {
    ActivePokemonState* pokemon1_state = &sActivePokemon[sBattleScene.scene->actingSide]->state;
    ActivePokemonState* temp_a3 = &sActivePokemon[sBattleScene.scene->actingSide]->state;
    ActivePokemonState* pokemon2_state = &sActivePokemon[!sBattleScene.scene->actingSide]->state;

    if ((sActivePokemon[sBattleScene.scene->actingSide]->state.replacementState == 0xF) || (pokemon1_state->replacementState == 0x10)) {
        if (sActivePokemon[sBattleScene.scene->actingSide]->teamRoster->isHuman & 2) {
            BattleAI_RecordUsedMove(temp_a3->rosterIndex, pokemon2_state->rosterIndex, 2);
        }
        Battle_ClearRuntimeFlags(sActivePokemon[sBattleScene.scene->actingSide], ACTIVE_SUBSTITUTE | ACTIVE_SUB_BREAK);
        sBattleScene.scene->phaseStep = 0;
        sBattleScene.scene->stepFrameTimer = 0;
        if (pokemon1_state->replacementState == 0x10) {
            sBattleScene.scene->phase = 0x12;
        } else {
            BattleAnim_GetOwnerDefaultAnchorPosition(sActivePokemon[sBattleScene.scene->actingSide], &gCameraPreset.startingPosition);
            BattleScene_LoadCameraPreset(&sSwitchCameraPreset, sBattleScene.scene->camera, sActivePokemon[sBattleScene.scene->actingSide]);
            sBattleScene.scene->phase = 0xB;
        }
        return 1;
    }

    return 0;
}

void BattleScene_BeginBattlePhase(void) {
    ActivePokemonState* state1 = &sActivePokemon[0]->state;
    ActivePokemonState* state2 = &sActivePokemon[1]->state;

    if ((BattleScene_IsBattleCountdownComplete() == 0) && (D_84390134 != 1)) {
        AudioFrameUpdate(1);
    }

    BattleScene_ClearInactiveOwnerAnimationFlags();
    state1->selectedCommandOption = 0;
    state2->selectedCommandOption = 0;

    if ((state1->replacementState == 0x10) || (state2->replacementState == 0x10) || (sBattleScene.scene->unk_2E == 2)) {
        BattleScene_SetCameraSubstate(1, 0);
    } else {
        BattleScene_SetCameraSubstate(1, 1);
    }

    sBattleScene.scene->unk_44 = 1;
    sBattleScene.scene->phaseStep += 1;
}

s32 BattleScene_TransitionToPhaseFull(s32 phase) {
    BattleScene_ClearPhaseFlag();
    BattleScene_ResetCameraFrameCounter();
    sBattleScene.scene->moveEffectKind = 0;
    sBattleScene.scene->unk_4C = 0;
    sBattleScene.scene->phaseStep = 0;
    sBattleScene.scene->stepFrameTimer = 0;
    sBattleScene.scene->phase = phase;
    return 1;
}

s32 BattleScene_TransitionToPhase(s32 phase) {
    BattleScene_ClearPhaseFlag();
    BattleScene_ResetCameraFrameCounter();
    sBattleScene.scene->unk_4C = 0;
    sBattleScene.scene->phaseStep = 0;
    sBattleScene.scene->stepFrameTimer = 0;
    sBattleScene.scene->phase = phase;
    return 1;
}

s32 BattleScene_TransitionToPhaseDuplicate(s32 phase) {
    BattleScene_ClearPhaseFlag();
    BattleScene_ResetCameraFrameCounter();
    sBattleScene.scene->unk_4C = 0;
    sBattleScene.scene->phaseStep = 0;
    sBattleScene.scene->stepFrameTimer = 0;
    sBattleScene.scene->phase = phase;
    return 1;
}

void BattleScene_SwitchActiveSideAndSetPhase(CameraNode* camera) {
    sBattleScene.scene->actingSide = !sBattleScene.scene->actingSide;
    BattleScene_SetPhase(camera, 5);
}

void Battle_ActSecondAttacker(u8 next_phase) {
    ActivePokemonState* pokemon_state;

    sBattleScene.scene->actingSide = !sBattleScene.scene->actingSide;
    pokemon_state = &sActivePokemon[sBattleScene.scene->actingSide]->state;

    if ((pokemon_state->replacementState == 0xF) || (pokemon_state->replacementState == 0x10)) {
        BeginBattleForEntity(sActivePokemon[sBattleScene.scene->actingSide]);
    } else {
        Battle_ResolveMonAction();
    }

    sBattleScene.scene->unk_34 = 1;
    sBattleScene.scene->unk_A0.z = 0.0f;
    BattleScene_SetPhase(sBattleScene.scene->camera, 0x24);
    sBattleScene.scene->nextPhase = next_phase;
}

void BattleScene_PhaseResumeSaved(void) {
    if (BattleScene_AdvanceOwnerAnimation(sActivePokemon[sBattleScene.scene->actingSide]) == 0) {
        sBattleScene.scene->phase = sBattleScene.scene->nextPhase;
    }
}

void BattleScene_CleanupActionAndAdvance(void) {
    BattleMonRuntime* acting_pokemon_state;
    BattleMonRuntime* other_pokemon_state;
    ActivePokemonState* acting_pokemon_runtime;

    if (sBattleScene.scene->unk_34 == 0) {
        BattleAnim_ClearRockThrowingMoveFlag(sActivePokemon[sBattleScene.scene->actingSide]);
        Audio_FadeAndClearStoredSounds();
        BattleScene_ResetAnimationStep();

        acting_pokemon_state = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;
        other_pokemon_state = &sActivePokemon[!sBattleScene.scene->actingSide]->state.runtime;
        acting_pokemon_runtime = &sActivePokemon[!sBattleScene.scene->actingSide]->state;

        if ((((acting_pokemon_state->usedMove & 0xF0) == 0xF0) || ((other_pokemon_state->usedMove & 0xF0) == 0xF0)) && (acting_pokemon_runtime->replacementState != 0xF) &&
            (acting_pokemon_runtime->replacementState != 0x10)) {
            sBattleScene.scene->phaseStep = 0;
            sBattleScene.scene->stepFrameTimer = 0;
            sBattleScene.scene->phase = 4;

            if ((acting_pokemon_state->usedMove & 0xF0) == 0xF0) {
                acting_pokemon_state->usedMove = STRUGGLE;
            }

            if ((other_pokemon_state->usedMove & 0xF0) == 0xF0) {
                other_pokemon_state->usedMove = STRUGGLE;
            }
        } else {
            Battle_ActSecondAttacker(3);
            sBattleScene.scene->phaseStep = 0;
            sBattleScene.scene->stepFrameTimer = 0;
        }
    } else {
        acting_pokemon_state = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;
        other_pokemon_state = &sActivePokemon[!sBattleScene.scene->actingSide]->state.runtime;

        BattleAnim_ClearRockThrowingMoveFlag(sActivePokemon[sBattleScene.scene->actingSide]);
        Audio_FadeAndClearStoredSounds();
        BattleScene_ResetAnimationStep();

        sBattleScene.scene->phaseStep = 0;
        sBattleScene.scene->stepFrameTimer = 0;
        sBattleScene.scene->phase = 4;

        if ((acting_pokemon_state->usedMove & 0xF0) == 0xF0) {
            acting_pokemon_state->usedMove = STRUGGLE;
        }

        if ((other_pokemon_state->usedMove & 0xF0) == 0xF0) {
            other_pokemon_state->usedMove = STRUGGLE;
        }
    }
}

s32 BattleScene_HandleFaintStatusCompletion(void) {
    BattleMonRuntime* non_acting_pokemon_runtime = &sNonActingPokemon->state.runtime;

    if (((non_acting_pokemon_runtime->unk_4D & 0x80) || (non_acting_pokemon_runtime->unk_15 & 0x18)) && ((non_acting_pokemon_runtime->usedMove & 0xF0) == 0xF0)) {
        BattleAnim_ClearRockThrowingMoveFlag(sActivePokemon[sBattleScene.scene->actingSide]);
        Audio_FadeAndClearStoredSounds();
        BattleScene_ResetAnimationStep();
        Battle_ActSecondAttacker(3);
        sBattleScene.scene->phaseStep = 0;
        sBattleScene.scene->moveEffectKind = 0;
        sBattleScene.scene->stepFrameTimer = 0;
        return 1;
    }
    return 0;
}

void BattleScene_CompleteActionPhase(void) {
    if ((sBattleScene.scene->unk_34 == 0) && ((sNonActingPokemon->state.runtime.usedMove & 0xF0) != 0xF0)) {
        BattleAnim_ClearRockThrowingMoveFlag(sActivePokemon[sBattleScene.scene->actingSide]);
        Audio_FadeAndClearStoredSounds();
        BattleScene_ResetAnimationStep();
        Battle_ActSecondAttacker(3);
        sBattleScene.scene->phaseStep = 0;
        sBattleScene.scene->stepFrameTimer = 0;
    } else if (BattleScene_HandleFaintStatusCompletion() == 0) {
        BattleAnim_ClearRockThrowingMoveFlag(sActivePokemon[sBattleScene.scene->actingSide]);
        Audio_FadeAndClearStoredSounds();
        BattleScene_ResetAnimationStep();
        sBattleScene.scene->phaseStep = 0;
        sBattleScene.scene->stepFrameTimer = 0;
        sBattleScene.scene->phase = 4;
    }
}

void BattleScene_ResolveDisplayedHpCompletion(CameraNode* camera) {
    if ((sSide1HpConsistent == 1) && (sSide2HpConsistent == 1)) {
        if (Battle_GetActiveMonHp(sActivePokemon[sBattleScene.scene->actingSide]) == 0) {
            BattleScene_SwitchActiveSideAndSetPhase(camera);
        } else if (Battle_GetActiveMonHp(sActivePokemon[!sBattleScene.scene->actingSide]) == 0) {
            BattleScene_SetPhase(camera, 5);
        } else {
            BattleScene_CompleteActionPhase();
        }
    }
}

void BattleScene_UpdateDisplayedHpAndTransition(CameraNode* camera) {
    BattleMonRuntime* sp1C = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;

    sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[0]);
    sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[1]);
    BattleScene_ClearPhaseFlag();
    BattleScene_ResetCameraFrameCounter();

    if (Battle_GetActiveMonHp(sActivePokemon[sBattleScene.scene->actingSide]) == 0) {
        BattleScene_SwitchActiveSideAndSetPhase(camera);
    } else if (Battle_GetActiveMonHp(sActivePokemon[!sBattleScene.scene->actingSide]) == 0) {
        BattleScene_ResolveDisplayedHpCompletion(camera);
    } else if (sBattleScene.scene->unk_4C == 0xA) {
        BattleScene_TransitionToPhaseDuplicate(0xA);
    } else if (sBattleScene.scene->moveEffectKind == 9) {
        BattleScene_TransitionToPhaseFull(9);
    } else if ((sp1C->unk_15 & 0x18) || (sp1C->unk_4D & 0x80)) {
        BattleScene_TransitionToPhaseFull(6);
    } else {
        BattleScene_ResolveDisplayedHpCompletion(camera);
    }
}

void BattleScene_WaitForDisplayedHpAnimations(CameraNode* camera) {
    if ((sSide1HpConsistent == 1) && (sSide2HpConsistent == 1)) {
        BattleBox_Close();
        BattleScene_UpdateDisplayedHpAndTransition(camera);
    }
}

s32 BattleScene_ActionPresentationTransitionCheck(void) {
    switch (sBattleScene.scene->moveEffectKind) {
        case 7:
        case 38:
        case 39:
            BattleScene_EnterStateRow(sActingPokemon, 3);
            return BattleScene_TransitionToPhase(0x25);

        case 30:
            BattleScene_EnterStateRow(sActingPokemon, 3);
            return BattleScene_TransitionToPhase(0x25);

        case 28:
            return BattleScene_TransitionToPhase(0x25);

        case 21:
            BattleScene_EnterStateRow(sActingPokemon, 3);
            return BattleScene_TransitionToPhase(0x25);

        case 15:
            return BattleScene_TransitionToPhase(0x25);

        case 25:
            return BattleScene_TransitionToPhase(0x25);

        case 27:
            return BattleScene_TransitionToPhase(0x25);

        case 29:
            BattleScene_EnterStateRow(sActingPokemon, 3);
            return BattleScene_TransitionToPhase(0x25);

        case 12:
            BattleScene_EnterStateRow(sActingPokemon, 8);
            return BattleScene_TransitionToPhase(0x25);

        case 13:
            BattleScene_EnterStateRow(sActingPokemon, 8);
            return BattleScene_TransitionToPhase(0x25);

        case 14:
            BattleScene_EnterStateRow(sActingPokemon, 8);
            return BattleScene_TransitionToPhase(0x25);

        case 17:
            BattleScene_EnterStateRow(sActingPokemon, 9);
            return BattleScene_TransitionToPhase(0x25);

        case 31:
            return BattleScene_TransitionToPhase(0x25);
    }
    return 0;
}

s32 BattleScene_ActionPresentationCleanupCheck(void) {
    s32 tmp = sBattleScene.scene->moveEffectKind;

    if (tmp != 0x10) {
        if (sBattleScene.scene->moveEffectKind == 0x14) {
            return BattleScene_TransitionToPhaseFull(0x14);
        }
    } else {
        return BattleScene_TransitionToPhaseFull(0x10);
    }

    if ((sBattleMoveFailed != 0) || (sBattleScene.scene->moveEffectKind == 8)) {
        return BattleScene_TransitionToPhaseFull(8);
    }
    return 0;
}

s32 BattleScene_PrepareResidualStatusPresentation(void) {
    s32 idx = sBattleScene.scene->actingSide;
    BattleMonRuntime* pokemon_runtime = &sActivePokemon[idx]->state.runtime;

    if (((pokemon_runtime->unk_4D & 0x80) || (pokemon_runtime->unk_15 & 0x18)) && ((pokemon_runtime->usedMove & 0xF0) == 0xF0)) {
        return BattleScene_TransitionToPhaseFull(0x23);
    }
    return 0;
}

void BattleScene_UpdateActiveOwnerPointers(void) {
    s32 side;

    sActingPokemon = sActivePokemon[sBattleScene.scene->actingSide];
    side = sBattleScene.scene->actingSide == 0;
    sNonActingPokemon = sActivePokemon[side];
}

void BattleScene_InitializeBattlePresentation(CameraNode* camera) {
    AudioFrameUpdate(0);
    BattleScene_SetCameraClipPlanes(camera, 20.0f, 12800.0f);
    BattleScene_ResetAnimationStep();
    sBattleScene.scene->unk_34 = 0;
    BattleScene_ClearPhaseFlag();
    sBattleScene.scene->stepFrameTimer = 0;
    Battle_ResetModelToIdleAnim(sActivePokemon[0]);
    Battle_ResetModelToIdleAnim(sActivePokemon[1]);
    sBattleScene.scene->phaseStep++;
}

void BattleScene_SendOutPresentationFinalize(CameraNode* camera) {
    ActivePokemonState* other_pokemon_state = &sActivePokemon[!sBattleScene.scene->actingSide]->state;
    ActivePokemonState* acting_pokemon_state = &sActivePokemon[sBattleScene.scene->actingSide]->state;
    s16 acting_pokemon_species = sActivePokemon[sBattleScene.scene->actingSide]->displayObject.objectType;
    MovePresentationCue* acting_pokemon_idle_cue = &sSideBattleCues[sBattleScene.scene->actingSide]->idleCues;
    ActivePokemon* acting_pokemon = sActivePokemon[sBattleScene.scene->actingSide];

    if (((other_pokemon_state->replacementState == 0x13) && (acting_pokemon_state->replacementState == 0x13)) ||
        ((other_pokemon_state->replacementState == 0x10) && (acting_pokemon_state->replacementState == 0x13)) ||
        ((acting_pokemon_state->replacementState == 0x10) && (other_pokemon_state->replacementState == 0x13))) {
        BattleBox_Open(0xE);
        Vec3f_SetComponentsDuplicate(&camera->view.targetPosition, 0.0f, 75.0f, 0.0f);
        Vec3f_SetComponentsDuplicate(&camera->view.eyePosition, 0.0f, 80.0f, 400.0f);
        Audio_StartBattleMusicSequence();
        sBattleScene.scene->phaseStep = 8;
        return;
    }

    if (acting_pokemon_state->flags & (ACTIVE_RETURN_RESET | ACTIVE_UNDERGROUND)) {
        acting_pokemon->displayObject.lodFraction = 0xFF;
        BattleAnim_StopOwnerCategoryParticles(4, acting_pokemon);
        acting_pokemon->displayObject.position.y = sPokemonModelDimensions[sBattleScene.scene->actingSide].restingHeight;
        acting_pokemon->displayObject.node.renderFlags &= ~0x2;
        acting_pokemon->displayObject.node.renderFlags |= 0x20;
        acting_pokemon->displayObject.node.flags |= 1;
        Battle_ClearRuntimeFlags(acting_pokemon, (ACTIVE_UNDERGROUND | ACTIVE_AIR_ACTION | ACTIVE_AIRBORNE | ACTIVE_RETURN_RESET));
    }

    if (sActivePokemon[sBattleScene.scene->actingSide]->modelTransformId == 0xA) {
        BattleScene_SetParticipantModelTransform(sActivePokemon[sBattleScene.scene->actingSide], 0, 0);
    }

    if (sActivePokemon[sBattleScene.scene->actingSide]->modelTransformId == 0xD) {
        BattleScene_SetParticipantModelTransform(sActivePokemon[sBattleScene.scene->actingSide], 0, 0);
    }

    if (sActivePokemon[sBattleScene.scene->actingSide]->modelTransformId == 0x10) {
        BattleScene_SetParticipantModelTransform(sActivePokemon[sBattleScene.scene->actingSide], 0, 0);
    }

    sActivePokemon[sBattleScene.scene->actingSide]->displayObject.rotation.x = 0;
    if ((acting_pokemon_species == DIGLETT) || (acting_pokemon_species == DUGTRIO)) {
        sActivePokemon[sBattleScene.scene->actingSide]->displayObject.node.renderFlags |= 0x20;
        sActivePokemon[sBattleScene.scene->actingSide]->displayObject.node.flags |= 1;
        ModelAnim_SetAnimation(&sActivePokemon[sBattleScene.scene->actingSide]->displayObject, acting_pokemon_idle_cue->id);
    }

    Battle_ResetModelToIdleAnim(sActivePokemon[sBattleScene.scene->actingSide]);
    Vec3f_SetComponentsDuplicate(&sActivePokemon[sBattleScene.scene->actingSide]->displayObject.scale, 1.0f, 1.0f, 1.0f);
    sActivePokemon[sBattleScene.scene->actingSide]->displayObject.position.y = BattleScene_GetParticipantModelRestingHeight(sActivePokemon[sBattleScene.scene->actingSide]);
    BattleAnim_GetOwnerDefaultAnchorPosition(sActivePokemon[sBattleScene.scene->actingSide], &gCameraPreset.startingPosition);
    BattleScene_LoadCameraPreset(&D_84384AF8, camera, sActivePokemon[sBattleScene.scene->actingSide]);
    BattleScene_ApplyCameraPreset(camera, sActivePokemon[sBattleScene.scene->actingSide], 1);
    Audio_StartBattleMusicSequence();
    sBattleScene.scene->phaseStep = 6;
}

s32 BattleScene_ResetOwnerStatusPresentation(ActivePokemon* pokemon) {
    UNUSED s32 pad;
    ActivePokemonState* state = &pokemon->state;

    sBattleScene.scene->unk_54 = 0;
    BattleAnim_ResetFreezeStatusAndRestoreAnimation(pokemon);

    if (state->flags & ACTIVE_AIRBORNE) {
        Battle_ClearRuntimeFlags(pokemon, 0x60C);
        return 1;
    }

    if (state->flags & (ACTIVE_RETURN_RESET | ACTIVE_UNDERGROUND)) {
        pokemon->displayObject.lodFraction = 0xFF;
        BattleAnim_StopOwnerCategoryParticles(4, pokemon);
        pokemon->displayObject.position.y = sPokemonModelDimensions[BattleScene_GetParticipantSideIndex(pokemon)].restingHeight;
        pokemon->displayObject.node.renderFlags &= ~0x2;
        pokemon->displayObject.node.renderFlags |= 0x20;
        pokemon->displayObject.node.flags |= 1;
        Battle_ClearRuntimeFlags(pokemon, (ACTIVE_UNDERGROUND | ACTIVE_AIR_ACTION | ACTIVE_AIRBORNE | ACTIVE_RETURN_RESET));
    }

    BattleAnim_EndSleepStatusAnimation(pokemon);
    state->alpha = 0xFF;
    Model_SetMaterialColor(&pokemon->displayObject, 0xFF, 0xFF, 0xFF, state->alpha);
    BattleScene_CleanupParticipantEffectMode(pokemon);
    pokemon->displayObject.rotation.x = 0;
    return 0;
}

void BattleScene_AdvanceSendOutPreparation(CameraNode* camera) {
    if (BattleScene_AdvanceOwnerAnimation(sActivePokemon[!sBattleScene.scene->actingSide]) == 0) {
        Battle_ClearRuntimeFlags(sActivePokemon[!sBattleScene.scene->actingSide], (ACTIVE_SUBSTITUTE | ACTIVE_SUB_BREAK));
        Audio_FadeAndClearStoredSounds();
        BattleAnim_CleanupEffects(2);
        BattleScene_ResetCameraFrameCounter();
        RemovePlayer(BattleScene_GetParticipantDisplayPlayerNumber(sActivePokemon[!sBattleScene.scene->actingSide]));

        if (BattleScene_ResetOwnerStatusPresentation(sActivePokemon[!sBattleScene.scene->actingSide]) != 0) {
            BattleScene_EnterStateRow(sActivePokemon[!sBattleScene.scene->actingSide], 0x11);
            BattleScene_ClearCameraOffsetVectors();
            sBattleScene.scene->phaseStep = 0xA;
        } else {
            BattleScene_EnterStateRow(sActivePokemon[!sBattleScene.scene->actingSide], 0);
            BattleScene_ClearCameraOffsetVectors();
            sBattleScene.scene->phaseStep++;
        }

        BattleScene_SelectCameraShot(sActivePokemon[!sBattleScene.scene->actingSide], camera, 3);

        if (BattleScene_IsPartnerPikachu(sActivePokemon[!sBattleScene.scene->actingSide]) != 0) {
            Cry_Play(PARTNER_PIKACHU, 1);
        } else {
            Cry_Play(sActivePokemon[!sBattleScene.scene->actingSide]->displayObject.objectType, 1);
        }
    }
}

void BattleScene_AdvanceSendOutCamera(CameraNode* camera) {
    UNUSED s32 pad[2];
    ActivePokemonState* active_pokemon_state = &sActivePokemon[!sBattleScene.scene->actingSide]->state;
    MovePresentationCue* faint_cue = &sSideBattleCues[!sBattleScene.scene->actingSide]->faintCue;

    if (BattleAnim_IsAssetLoadBusy(sActivePokemon[!sBattleScene.scene->actingSide]) == 0) {
        if (sBattleScene.scene->stepFrameTimer == 0x1D) {
            BattleScene_SelectCameraShot(sActivePokemon[!sBattleScene.scene->actingSide], camera, 4);
        }

        if (BattleScene_AdvanceCameraFrameCounterUntil(0x1E) != 0) {
            BattleScene_ApproachCameraZoom(camera, sActivePokemon[!sBattleScene.scene->actingSide]);
        }

        if (((faint_cue->animationDuration >> 1) == (sActivePokemon[!sBattleScene.scene->actingSide]->displayObject.transformAnim.frameFixed >> 0x10)) &&
            (active_pokemon_state->replacementState != 0x12)) {
            BattleBox_Open(9);
        }

        if (faint_cue->frameCounter == ((sActivePokemon[!sBattleScene.scene->actingSide]->displayObject.transformAnim.frameFixed >> 0x10) + 3)) {
            sActivePokemon[!sBattleScene.scene->actingSide]->displayObject.node.renderFlags &= ~0x20;
            Battle_SetRuntimeFlags(sActivePokemon[!sBattleScene.scene->actingSide], ACTIVE_HOLD_RENDER);
            BattleScene_ResetCameraFrameCounter();
            sBattleScene.scene->phaseStep += 1;
        }
        BattleScene_ApproachCameraTarget(camera, sActivePokemon[!sBattleScene.scene->actingSide], 0);
    }
}

void BattleScene_AdvanceSendOutCameraClose(CameraNode* camera) {
    BattleScene_ApproachCameraTarget(camera, sActivePokemon[!sBattleScene.scene->actingSide], 1);
    if (BattleScene_AdvanceCameraFrameCounter(0x19) != 0) {
        Audio_FadeAndClearStoredSounds();
        sBattleScene.scene->phaseStep++;
    }
}

void BattleScene_FinishSendOutCameraClose(CameraNode* camera) {
    ActivePokemonState* opposite_side_pokemon_state = &sActivePokemon[!sBattleScene.scene->actingSide]->state;

    BattleScene_ApproachCameraTarget(camera, sActivePokemon[!sBattleScene.scene->actingSide], 1);

    if (BattleScene_AdvanceCameraFrameCounter(5) != 0) {
        BattleAnim_CleanupEffects(1);
        Audio_FadeCommand(3, 0);

        if (opposite_side_pokemon_state->replacementState != REPLACEMENT_DEFEAT_PENDING) {
            opposite_side_pokemon_state->replacementState = REPLACEMENT_INVOLUNTARY_SWITCH;
        }

        if (opposite_side_pokemon_state->replacementState == REPLACEMENT_DEFEAT_PENDING) {
            opposite_side_pokemon_state->replacementState = REPLACEMENT_DEFEAT_COMPLETE;
        }

        sBattleScene.scene->phaseStep++;
    }
}

void BattleScene_CompleteSendOutPresentation(CameraNode* camera) {
    ActivePokemonState* other_pokemon_state = &sActivePokemon[!sBattleScene.scene->actingSide]->state;
    ActivePokemonState* acting_pokemon_state = &sActivePokemon[sBattleScene.scene->actingSide]->state;

    if (BattleScene_AdvanceCameraFrameCounter(0xA) != 0) {
        BattleScene_ResetCameraFrameCounter();
        BattleScene_ClearPhaseFlag();
        sBattleScene.scene->phaseStep++;
        if (other_pokemon_state->replacementState == REPLACEMENT_INVOLUNTARY_SWITCH) {
            sBattleScene.scene->stepFrameTimer = 0;
            sBattleScene.scene->phaseStep = 0;

            if ((acting_pokemon_state->replacementState == REPLACEMENT_FAINT_PENDING) || (acting_pokemon_state->replacementState == REPLACEMENT_DEFEAT_PENDING)) {
                sBattleScene.scene->phase = 5;
                sBattleScene.scene->actingSide = !sBattleScene.scene->actingSide;
            } else if ((other_pokemon_state->replacementState == REPLACEMENT_DEFEAT_COMPLETE) || (acting_pokemon_state->replacementState == REPLACEMENT_DEFEAT_COMPLETE)) {
                BattleBox_Open(0xE);
                Vec3f_SetComponentsDuplicate(&camera->view.targetPosition, 0.0f, 75.0f, 0.0f);
                Vec3f_SetComponentsDuplicate(&camera->view.eyePosition, 0.0f, 80.0f, 400.0f);
                Audio_StartBattleMusicSequence();
                sBattleScene.scene->actingSide = !sBattleScene.scene->actingSide;
                sBattleScene.scene->phaseStep = 8;
            } else {
                sBattleScene.scene->phase = 4;
            }
        } else if ((acting_pokemon_state->replacementState == REPLACEMENT_DEFEAT_PENDING) || (acting_pokemon_state->replacementState == REPLACEMENT_FAINT_PENDING)) {
            sBattleScene.scene->stepFrameTimer = 0;
            sBattleScene.scene->phaseStep = 0;
            sBattleScene.scene->phase = 5;
            sBattleScene.scene->actingSide = !sBattleScene.scene->actingSide;
        } else {
            BattleScene_SendOutPresentationFinalize(camera);
        }
        Audio_FadeAndClearStoredSounds();
    }
}

void func_84321D34(UNUSED CameraNode* camera) {
}

void BattleScene_SetupSendOutOwnerShot(CameraNode* camera) {
    ActivePokemon* pokemon;
    UNUSED s32 pad;
    f32 sp44;
    f32 eye_distance;
    s16 eye_rotation_x;
    UNUSED s16 pad2;
    u8 camera_adjustment_index;
    s16 eye_rotation_y;

    pokemon = sActivePokemon[sBattleScene.scene->actingSide];
    camera_adjustment_index = pokemon->displayObject.objectType - 1;

    if (BattleScene_AnimateCameraPreset(camera, pokemon, 1, 0) != 0) {
        BattleScene_ResetCameraFrameCounter();
        BattleBox_Open(0xE);
        BattleScene_SetCameraClipPlanes(camera, 10.0f, 12800.0f);
        camera->perspective.verticalFieldOfView = 60.0f;
        if ((sActivePokemon[sBattleScene.scene->actingSide]->displayObject.objectType == ONIX) && (sBattleScene.scene->actingSide == 1)) {
            // Onix is the last camera adjustment, which explains why everything else is off by 1
            camera_adjustment_index = 151;
        }

        sp44 = (sSideCameraTransform[sBattleScene.scene->actingSide]->coordinates.y + pokemon->displayObject.position.y) + sModelCameraAdjustments[camera_adjustment_index].z;
        eye_distance = ((sSideCameraTransform[sBattleScene.scene->actingSide]->coordinates.z + sModelCameraAdjustments[camera_adjustment_index].y) * pokemon->facingDirection) + pokemon->displayObject.position.x;

        Vec3f_SetComponentsDuplicate(&camera->view.targetPosition, eye_distance, sp44, pokemon->displayObject.position.z);

        eye_distance = sSideCameraTransform[sBattleScene.scene->actingSide]->coordinates.x * sModelCameraAdjustments[camera_adjustment_index].x;
        eye_rotation_x = sSideCameraTransform[sBattleScene.scene->actingSide]->rotationX;
        eye_rotation_y = (pokemon->facingDirection * sSideCameraTransform[sBattleScene.scene->actingSide]->rotationY) + BattleScene_GetParticipantFacingAngle(pokemon);
        Camera_ComputeEyeFromAngles(&camera->view.targetPosition, &camera->view.eyePosition, eye_distance, eye_rotation_x, eye_rotation_y);
        sBattleScene.scene->phaseStep++;
    }
}

void BattleScene_CompleteSendOutCameraFade(UNUSED CameraNode* camera) {
    if (BattleScene_AdvanceCameraFrameCounter(0x50) != 0) {
        sBattleScene.scene->winner = sBattleScene.scene->actingSide + 1;
        BattleBox_Close();
        Audio_FadeOutAll(0x14);
    }
}

void BattleScene_CompleteSendOutExtendedFade(UNUSED CameraNode* camera) {
    if (BattleScene_AdvanceCameraFrameCounter(0x7D) != 0) {
        sBattleScene.scene->winner = sBattleScene.scene->actingSide + 1;
        Audio_FadeOutAll(0x14);
    }
}

void BattleScene_AdvanceSendOutFollowupCamera(CameraNode* camera) {
    UNUSED s32 pad[2];
    ActivePokemonState* sp24 = &sActivePokemon[!sBattleScene.scene->actingSide]->state;
    MovePresentationCue* sp20 = &sSideBattleCues[!sBattleScene.scene->actingSide]->unk_B10;

    if (BattleAnim_IsAssetLoadBusy(sActivePokemon[!sBattleScene.scene->actingSide]) == 0) {
        if (sBattleScene.scene->stepFrameTimer == 0x1D) {
            BattleScene_SelectCameraShot(sActivePokemon[!sBattleScene.scene->actingSide], camera, 4);
        }

        if (BattleScene_AdvanceCameraFrameCounterUntil(0x1E) != 0) {
            BattleScene_ApproachCameraZoom(camera, sActivePokemon[!sBattleScene.scene->actingSide]);
        }

        if (((sp20->animationDuration >> 1) == (sActivePokemon[!sBattleScene.scene->actingSide]->displayObject.transformAnim.frameFixed >> 0x10)) &&
            (sp24->replacementState != 0x12)) {
            BattleBox_Open(9);
        }

        if (sp20->frameCounter == ((sActivePokemon[!sBattleScene.scene->actingSide]->displayObject.transformAnim.frameFixed >> 0x10) + 3)) {
            sActivePokemon[!sBattleScene.scene->actingSide]->displayObject.node.renderFlags &= ~0x20;
            Battle_SetRuntimeFlags(sActivePokemon[!sBattleScene.scene->actingSide], 0x100);
            BattleScene_ResetCameraFrameCounter();
            BattleScene_UpdateCameraModelOffset(sActivePokemon[!sBattleScene.scene->actingSide], camera);
            sBattleScene.scene->phaseStep = 2;
        }

        BattleScene_ApproachCameraTarget(camera, sActivePokemon[!sBattleScene.scene->actingSide], 0);
    }
}

void BattleScene_PhaseSendOut(CameraNode* camera) {
    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_AdvanceSendOutPreparation(camera);
            break;

        case 1:
            BattleScene_AdvanceSendOutCamera(camera);
            break;

        case 2:
            BattleScene_AdvanceSendOutCameraClose(camera);
            break;

        case 3:
            BattleScene_FinishSendOutCameraClose(camera);
            break;

        case 4:
            BattleScene_CompleteSendOutPresentation(camera);
            break;

        case 5:
            func_84321D34(camera);
            break;

        case 6:
            BattleScene_SetupSendOutOwnerShot(camera);
            break;

        case 7:
            BattleScene_CompleteSendOutCameraFade(camera);
            break;

        case 8:
            BattleScene_CompleteSendOutExtendedFade(camera);
            break;

        case 10:
            BattleScene_AdvanceSendOutFollowupCamera(camera);
            break;
    }
}

void BattleScene_CompleteSendOutFollowup(UNUSED CameraNode* camera) {
    ActivePokemonState* sp1C = &sActivePokemon[!sBattleScene.scene->actingSide]->state;

    if (BattleScene_AdvanceCameraFrameCounter(2) != 0) {
        BattleScene_ResetCameraFrameCounter();
        BattleScene_ClearPhaseFlag();
        sBattleScene.scene->phaseStep += 1;
        if (sp1C->replacementState == 0x10) {
            sBattleScene.scene->stepFrameTimer = 0;
            sBattleScene.scene->phaseStep = 0;
            sBattleScene.scene->phase = 4;
        } else {
            sBattleScene.scene->winner = sBattleScene.scene->actingSide + 1;
        }
        Audio_FadeAndClearStoredSounds();
    }
}

void BattleScene_PhaseSendOutAfterFaint(CameraNode* camera) {
    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_AdvanceSendOutPreparation(camera);
            break;

        case 1:
            BattleScene_AdvanceSendOutCamera(camera);
            break;

        case 2:
            BattleScene_AdvanceSendOutCameraClose(camera);
            break;

        case 3:
            BattleScene_FinishSendOutCameraClose(camera);
            break;

        case 4:
            BattleScene_CompleteSendOutFollowup(camera);
            break;

        case 5:
            func_84321D34(camera);
            break;

        case 10:
            BattleScene_AdvanceSendOutFollowupCamera(camera);
            break;
    }
}

void BattleScene_ResetOwnerPresentationFlags(ActivePokemon* pokemon) {
    ActivePokemonState* state = &pokemon->state;

    if (state->flags & ACTIVE_AIRBORNE) {
        pokemon->displayObject.position.y = sPokemonModelDimensions[BattleScene_GetParticipantSideIndex(pokemon)].restingHeight;
        Battle_ClearRuntimeFlags(pokemon, 0x608);
    } else if (state->flags & (ACTIVE_UNDERGROUND | ACTIVE_RETURN_RESET)) {
        pokemon->displayObject.lodFraction = 0xFF;
        BattleAnim_StopOwnerCategoryParticles(4, pokemon);
        pokemon->displayObject.position.y = sPokemonModelDimensions[BattleScene_GetParticipantSideIndex(pokemon)].restingHeight;
        pokemon->displayObject.node.renderFlags &= ~0x2;
        pokemon->displayObject.node.renderFlags |= 0x20;
        pokemon->displayObject.node.flags |= 1;
        Battle_ClearRuntimeFlags(pokemon, 0x4608);
    }
}

void BattleScene_InitializeIntroCamera(CameraNode* camera) {
    s32 sp24;
    ActivePokemonState* pokemon1_state = &sActivePokemon[0]->state;
    ActivePokemonState* pokemon2_state = &sActivePokemon[1]->state;

    BattleScene_ClearStateTickCallbacks();
    BattleScene_ResetAnimationStep();

    sp24 = BattleAnim_RandomRange(5);
    if (sp24 >= 5) {
        sp24 = 0;
    }

    if ((pokemon1_state->flags & ACTIVE_UNDERGROUND) || ((pokemon2_state->flags & ACTIVE_UNDERGROUND) != 0)) {
        sp24 = 2;
    } else if ((pokemon1_state->flags & ACTIVE_AIRBORNE) || (pokemon2_state->flags & ACTIVE_AIRBORNE)) {
        sp24 = 4;
    } else if ((pokemon1_state->replacementState == 0x10) || (pokemon2_state->replacementState == 0x10)) {
        sp24 = 2;
    }

    BattleAnim_Vec3fCopy(&gCameraPreset.startingPosition, &s100UnitsUpFromCenter);
    BattleScene_LoadCameraPresetUnscaled(&D_84384B2C[sp24], camera);
    BattleScene_ApplyCameraPreset(camera, sActingPokemon, 0);
    sBattleScene.scene->phaseStep++;
}

void BattleScene_AdvanceIntroCamera(CameraNode* camera) {
    if (BattleScene_AnimateCameraPreset(camera, sActingPokemon, 0, 1) != 0) {
        BattleScene_SetPhase(camera, 2);
    }
}

void BattleScene_PhaseIntro(CameraNode* camera) {
    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_InitializeIntroCamera(camera);
            break;

        case 1:
            BattleScene_AdvanceIntroCamera(camera);
            break;
    }
}

void BattleScene_SetMonMoveResourceFlag(ActivePokemon* pokemon) {
    pokemon->state.flashTimer = 0x800;
}

void BattleScene_PrepareMoveResourcePresentation(UNUSED CameraNode* camera) {
    BattleAnim_CleanupEffects(2);
    BattleScene_SetMonMoveResourceFlag(sActingPokemon);
    BattleScene_LoadMoveResourceList(sActingPokemon);
}

void BattleScene_SelectActionCameraPreset(CameraNode* camera) {
    BattleMonRuntime* active_pokemon_runtime = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;
    ActivePokemonState* active_pokemon_state = &sActivePokemon[sBattleScene.scene->actingSide]->state;
    s32 var_t0;

    if (sBattleScene.scene->moveEffectKind == 0xE) {
        var_t0 = BattleScene_GetMappedAnimationTimingField0D(sActivePokemon[sBattleScene.scene->actingSide]);
    } else if ((sBattleScene.scene->moveEffectKind == 0x1C) || (sBattleScene.scene->moveEffectKind == 0x11) ||
               (sBattleScene.scene->moveEffectKind == 0x1F)) {
        var_t0 = 0;
    } else {
        var_t0 = BattleScene_GetAnimationTimingField0D(sActivePokemon[sBattleScene.scene->actingSide]);
    }

    if ((active_pokemon_runtime->usedMove == ICE_BEAM) || (active_pokemon_runtime->usedMove == SKY_ATTACK)) {
        if ((sBattleScene.scene->moveEffectKind != 0x1C) && (sBattleScene.scene->moveEffectKind != 0x11) &&
            (sBattleScene.scene->moveEffectKind != 0x1F)) {
            var_t0 = 0x16;
        }
    }

    switch (var_t0) {
        case 20:
            BattleScene_SelectCameraPreset(sActingPokemon, camera,
                          sBattleScene.scene->cameraPresetId = D_84385B70[BattleAnim_RandomIntWithWarmup(ARRAY_COUNT(D_84385B70))], 1);
            break;

        case 21:
            BattleScene_SelectCameraPreset(sActingPokemon, camera,
                          sBattleScene.scene->cameraPresetId = D_84385B78[BattleAnim_RandomIntWithWarmup(ARRAY_COUNT(D_84385B78))], 1);
            break;

        case 22:
            BattleScene_SelectCameraPreset(sActingPokemon, camera,
                          sBattleScene.scene->cameraPresetId = D_84385B7C[BattleAnim_RandomIntWithWarmup(ARRAY_COUNT(D_84385B7C))], 1);
            break;

        case 23:
            BattleScene_SelectCameraPreset(sActingPokemon, camera,
                          sBattleScene.scene->cameraPresetId = D_84385B80[BattleAnim_RandomIntWithWarmup(ARRAY_COUNT(D_84385B80))], 1);
            break;

        case 24:
            BattleScene_SelectCameraPreset(sActingPokemon, camera,
                          sBattleScene.scene->cameraPresetId = D_84385B88[BattleAnim_RandomIntWithWarmup(ARRAY_COUNT(D_84385B88))], 1);
            break;

        default:
            BattleScene_SelectCameraPreset(sActingPokemon, camera, sBattleScene.scene->cameraPresetId = var_t0, 1);
            if (active_pokemon_state->flags & 0x4000) {
                camera->view.targetPosition.y = 0.0f;
            }
            break;
    }
}

s32 BattleScene_AdvanceActionPresentationSetup(CameraNode* camera) {
    UNUSED s32 pad;
    ActivePokemonState* sp20 = &sActivePokemon[sBattleScene.scene->actingSide]->state;

    BattleScene_ResetCameraFrameCounter();
    BattleScene_SelectActionCameraPreset(camera);

    if (BattleScene_ActionPresentationTransitionCheck() != 0) {
        return 0;
    }

    if (sBattleScene.scene->moveEffectKind == 0x14) {
        BattleScene_EnterStateRow(sActivePokemon[sBattleScene.scene->actingSide], 0x15);
    } else if (sBattleScene.scene->moveEffectKind == 0x18) {
        BattleScene_EnterStateRow(sActivePokemon[sBattleScene.scene->actingSide], 3);
    } else if (sp20->flags & (ACTIVE_AIR_ACTION | ACTIVE_UNDERGROUND)) {
        BattleScene_EnterStateRow(sActivePokemon[sBattleScene.scene->actingSide], 3);
        BattleScene_SelectCameraPreset(sActivePokemon[sBattleScene.scene->actingSide], camera, sBattleScene.scene->cameraPresetId = 2, 1);
    } else {
        BattleScene_EnterStateRow(sActivePokemon[sBattleScene.scene->actingSide], 3);
    }

    BattleScene_ClearPhaseFlag();
    return 1;
}

void BattleScene_ActionSetupCameraStep(CameraNode* camera) {
    ActivePokemonState* pokemon_state = &sActingPokemon->state;

    BattleAnim_ApproachF(&camera->perspective.verticalFieldOfView, 30.0f, 0.05f);

    switch (sBattleScene.scene->phaseSubstep) {
        case 0:
            BattleScene_CleanupParticipantEffectMode(sActivePokemon[0]);
            BattleScene_CleanupParticipantEffectMode(sActivePokemon[1]);

            if ((BattleScene_PrepareReplacementTransition() == 0) && (BattleScene_PrepareResidualStatusPresentation() == 0)) {
                Battle_ClearOpponentHopFlagAndSyncMove(sActingPokemon);
                BattleScene_PrepareMoveResourcePresentation(camera);
                BattleScene_SetPhaseFlag();

                if ((sBattleScene.scene->unk_34 == 1) && (sNonActingPokemon->unk_4C8 == 1)) {
                    BattleScene_ResetOwnerPresentationFlags(sNonActingPokemon);
                }

                BattleAnim_GetOwnerDefaultAnchorPosition(sActingPokemon, &gCameraPreset.startingPosition);

                if (pokemon_state->flags & ACTIVE_UNDERGROUND) {
                    gCameraPreset.startingPosition.y = 15.000001f;
                }

                if (sBattleScene.scene->unk_34 == 1) {
                    BattleScene_ResetCameraFov(camera);

                    if ((sBattleScene.scene->moveEffectKind == 0x1C) || (sBattleScene.scene->moveEffectKind == 0x11) ||
                        (sBattleScene.scene->moveEffectKind == 0x1F)) {
                        BattleScene_SelectCameraFollowPreset(sActingPokemon, camera, 3);
                    } else {
                        BattleScene_SelectCameraFollowPreset(sActingPokemon, camera, 4);
                    }

                    if (pokemon_state->flags & ACTIVE_UNDERGROUND) {
                        camera->view.targetPosition.y = 15.000001f;
                    }
                    Battle_ResetModelToIdleAnim(sActingPokemon);
                } else if ((sBattleScene.scene->moveEffectKind == 0x1C) || (sBattleScene.scene->moveEffectKind == 0x11) ||
                           (sBattleScene.scene->moveEffectKind == 0x1F)) {
                    BattleScene_LoadCameraPreset(&D_8438498C, camera, sActingPokemon);
                } else {
                    BattleScene_LoadCameraPreset(&sSwitchCameraPreset, camera, sActingPokemon);
                }
            }
            break;

        case 1:
            if (BattleAnim_TryRegisterAssetTables(sActingPokemon) != 0) {
                D_84390234 = sMultihitCountdown = sMultihitCount;
                BattleBox_Open(0);
                BattleScene_ResetCameraFrameCounter();
                sBattleScene.scene->phaseSubstep += 1;
            }
            break;

        case 2:
            if (sBattleScene.scene->unk_34 == 1) {
                if ((sBattleScene.scene->moveEffectKind == 0x1C) || (sBattleScene.scene->moveEffectKind == 0x11) ||
                    (sBattleScene.scene->moveEffectKind == 0x1F)) {
                    if (BattleScene_AdvanceCameraFrameCounter(0xA) != 0) {
                        BattleScene_ResetCameraFov(camera);
                        if (BattleScene_AdvanceActionPresentationSetup(camera) != 0) {
                            sBattleScene.scene->phaseStep = 1;
                        }
                    }
                } else if (BattleScene_AdvanceCameraFrameCounter(0x28) != 0) {
                    BattleScene_ResetCameraFov(camera);
                    if (BattleScene_AdvanceActionPresentationSetup(camera) != 0) {
                        sBattleScene.scene->phaseStep = 1;
                    }
                }
            } else if (BattleScene_AnimateCameraPreset(camera, sActingPokemon, 1, 1) != 0) {
                BattleScene_ResetCameraFov(camera);
                if (BattleScene_AdvanceActionPresentationSetup(camera) != 0) {
                    sBattleScene.scene->phaseStep = 1;
                }
            }
            break;
    }
}

void BattleScene_RefreshActionCameraPreset(CameraNode* camera) {
    s32 temp_v0;
    s32 temp_t0;

    if (sMultihitCountdown != sMultihitCount) {
        temp_v0 = BattleScene_GetAnimationTimingField0E(sActivePokemon[sBattleScene.scene->actingSide]) & 0xFFFFFFFF;
        switch (temp_v0) {
            case 20:
                do {
                    temp_t0 = D_84385B70[BattleAnim_RandomIntWithWarmup(5.0f)];
                } while (temp_t0 == sBattleScene.scene->cameraPresetId);
                sBattleScene.scene->cameraPresetId = temp_t0;
                BattleScene_SelectCameraPreset(sActingPokemon, camera, sBattleScene.scene->cameraPresetId, 1);
                break;

            case 21:
                do {
                    temp_t0 = D_84385B78[BattleAnim_RandomIntWithWarmup(4.0f)];
                } while (temp_t0 == sBattleScene.scene->cameraPresetId);
                sBattleScene.scene->cameraPresetId = temp_t0;
                BattleScene_SelectCameraPreset(sActingPokemon, camera, sBattleScene.scene->cameraPresetId, 1);
                break;

            case 22:
                do {
                    temp_t0 = D_84385B7C[BattleAnim_RandomIntWithWarmup(3.0f)];
                } while (temp_t0 == sBattleScene.scene->cameraPresetId);
                sBattleScene.scene->cameraPresetId = temp_t0;
                BattleScene_SelectCameraPreset(sActingPokemon, camera, sBattleScene.scene->cameraPresetId, 1);
                break;

            case 23:
                do {
                    temp_t0 = D_84385B80[BattleAnim_RandomIntWithWarmup(6.0f)];
                } while (temp_t0 == sBattleScene.scene->cameraPresetId);
                sBattleScene.scene->cameraPresetId = temp_t0;
                if (1) {}
                if (1) {}
                if (1) {}
                if (1) {}
                BattleScene_SelectCameraPreset(sActingPokemon, camera, sBattleScene.scene->cameraPresetId, 1);
                break;

            case 24:
                do {
                    temp_t0 = D_84385B88[BattleAnim_RandomIntWithWarmup(4.0f)];
                } while (temp_t0 == sBattleScene.scene->cameraPresetId);
                sBattleScene.scene->cameraPresetId = temp_t0;
                BattleScene_SelectCameraPreset(sActingPokemon, camera, sBattleScene.scene->cameraPresetId, 1);
                break;

            default:
                if (temp_v0 != sBattleScene.scene->cameraPresetId) {
                    BattleScene_SelectCameraPreset(sActingPokemon, camera, temp_v0, 1);
                }
                break;
        }
    }
}

void BattleScene_SelectActionCameraPresetFromTiming(CameraNode* camera, s32 arg1) {
    BattleMonRuntime* pokemon_runtime = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;
    s32 sp18;

    if ((sBattleScene.scene->cameraPresetId != 1) && (sBattleScene.scene->cameraPresetId != 4) && (sBattleScene.scene->cameraPresetId != 5) &&
        (sBattleScene.scene->cameraPresetId != 8) && (sBattleScene.scene->cameraPresetId != 9) && (sBattleScene.scene->cameraPresetId != 0xA) &&
        (sBattleScene.scene->cameraPresetId != 0xB)) {
        if (arg1 == 1) {
            sp18 = BattleScene_GetMappedAnimationTimingField0E(sActivePokemon[sBattleScene.scene->actingSide]);
        } else {
            sp18 = BattleScene_GetAnimationTimingField0E(sActivePokemon[sBattleScene.scene->actingSide]);
        }

        if ((pokemon_runtime->usedMove == ICE_BEAM) || (pokemon_runtime->usedMove == SKY_ATTACK)) {
            sp18 = 0x19;
        }

        if (sp18 == 25) {
            return;
        }

        switch (sp18) {
            case 20:
                sBattleScene.scene->cameraPresetId = D_84385B70[BattleAnim_RandomIntWithWarmup(ARRAY_COUNT(D_84385B70))];
                if (sp18 != sBattleScene.scene->cameraPresetId) {
                    BattleScene_SelectCameraPreset(sActingPokemon, camera, sBattleScene.scene->cameraPresetId, 1);
                }
                break;

            case 21:
                sBattleScene.scene->cameraPresetId = D_84385B78[BattleAnim_RandomIntWithWarmup(ARRAY_COUNT(D_84385B78))];
                if (sp18 != sBattleScene.scene->cameraPresetId) {
                    BattleScene_SelectCameraPreset(sActingPokemon, camera, sBattleScene.scene->cameraPresetId, 1);
                }
                break;

            case 22:
                sBattleScene.scene->cameraPresetId = D_84385B7C[BattleAnim_RandomIntWithWarmup(ARRAY_COUNT(D_84385B7C))];
                if (sp18 != sBattleScene.scene->cameraPresetId) {
                    BattleScene_SelectCameraPreset(sActingPokemon, camera, sBattleScene.scene->cameraPresetId, 1);
                }
                break;

            case 23:
                sBattleScene.scene->cameraPresetId = D_84385B80[BattleAnim_RandomIntWithWarmup(ARRAY_COUNT(D_84385B80))];
                if (sp18 != sBattleScene.scene->cameraPresetId) {
                    BattleScene_SelectCameraPreset(sActingPokemon, camera, sBattleScene.scene->cameraPresetId, 1);
                }
                break;

            case 24:
                sBattleScene.scene->cameraPresetId = D_84385B88[BattleAnim_RandomIntWithWarmup(ARRAY_COUNT(D_84385B88))];
                if (sp18 != sBattleScene.scene->cameraPresetId) {
                    BattleScene_SelectCameraPreset(sActingPokemon, camera, sBattleScene.scene->cameraPresetId, 1);
                }
                break;

            default:
                if (sp18 != sBattleScene.scene->cameraPresetId) {
                    if (1) {}
                    BattleScene_SelectCameraPreset(sActingPokemon, camera, sp18, 1);
                }
                break;
        }
    }
}

s32 BattleScene_ApplyActionCameraMode(CameraNode* camera, s32 arg1) {
    UNUSED s32 pad;
    s32 sp18 = 0;

    switch (sBattleScene.scene->cameraPresetId) {
        case 1:
            BattleAnim_ApproachF(&camera->perspective.verticalFieldOfView, 60.0f, 0.03f);
            sp18 = 1;
            break;

        case 4:
        case 5:
        case 8:
        case 9:
        case 10:
        case 11:
            BattleScene_AnimateCameraPresetToOwner(camera, sActingPokemon);
            sp18 = 1;
            break;

        default:
            if ((sActingPokemon->modelTransformId != 6) && (sActingPokemon->modelTransformId != 4) && (arg1 != 0)) {
                BattleScene_ReframeAndRecomputeDistance(camera, sActingPokemon);
            }
            break;
    }
    return sp18;
}

void BattleScene_CleanupActionPresentation(UNUSED CameraNode* camera) {
    BattleScene_CleanupParticipantHideFlag(sActingPokemon);
    Battle_ClearRuntimeFlags(sActingPokemon, 0x10);
    if (BattleScene_ActionPresentationCleanupCheck() == 0) {
        sBattleScene.scene->phaseStep = 2;
    }
    BattleAnim_CleanupEffects(1);
}

s32 BattleScene_WaitForActionCameraDuration(UNUSED CameraNode* camera) {
    if (BattleScene_AdvanceCameraFrameCounter(BattleScene_GetCameraShotDuration(sActingPokemon)) != 0) {
        return 1;
    }
    return 0;
}

void BattleScene_ActionSetupAnimationDefault(CameraNode* camera) {
    switch (sBattleScene.scene->phaseSubstep) {
        case 0:
            BattleScene_ApplyActionCameraMode(camera, 1);
            if (BattleScene_WaitForActionCameraDuration(camera) != 0) {
                BattleScene_ResetCameraFrameCounter();
                BattleScene_SetPhaseFlag();
                BattleScene_SelectActionCameraPresetFromTiming(camera, 0);
            }
            break;

        case 1:
            BattleScene_ApplyActionCameraMode(camera, 1);
            if (Battle_IsRowAnimationSettled(sActingPokemon, 0x32) != 0) {
                BattleScene_ResetCameraFrameCounter();
                BattleScene_ClearPhaseFlag();
                BattleScene_CleanupActionPresentation(camera);
            }
            break;
    }
}

void BattleScene_ActionSetupAnimationVariantA(CameraNode* camera) {
    switch (sBattleScene.scene->phaseSubstep) {
        case 0:
            BattleScene_ReframeAndRecomputeDistance(camera, sActingPokemon);
            if (Battle_IsRowAnimationSettled(sActingPokemon, 0x32) != 0) {
                BattleScene_SaveCameraFov(camera);
                sMultihitCountdown--;
                BattleScene_RefreshActionCameraPreset(camera);
                if ((sMultihitCountdown < 0) || (sBattleMoveFailed != 0)) {
                    sMultihitCountdown = sMultihitCount;
                    sBattleScene.scene->phaseSubstep++;
                } else {
                    BattleScene_EnterStateRow(sActivePokemon[sBattleScene.scene->actingSide], 0x13);
                    Audio_FadeAndClearStoredSounds();
                    BattleAnim_CleanupEffects(1);
                    sBattleScene.scene->phaseSubstep = 0;
                }
            }
            break;

        case 1:
            BattleScene_ReframeAndRecomputeDistance(camera, sActingPokemon);
            BattleScene_ResetCameraFrameCounter();
            BattleScene_ClearPhaseFlag();
            sBattleScene.scene->moveEffectKind = 0;
            Audio_FadeAndClearStoredSounds();
            BattleScene_CleanupActionPresentation(camera);
            break;
    }
}

void BattleScene_ActionSetupAnimationVariantB(CameraNode* camera) {
    switch (sBattleScene.scene->phaseSubstep) {
        case 0:
            if (BattleScene_AdvanceCameraFrameCounter(0xF) != 0) {
                BattleAnim_QueueEffectList(7, sActingPokemon);
                BattleScene_ResetCameraFrameCounter();
                BattleScene_SetPhaseFlag();
                BattleScene_SelectCameraFollowPreset(sActingPokemon, camera, 3);
            }
            break;

        case 1:
            BattleScene_ReframeAndRecomputeDistance(camera, sActingPokemon);
            if (Battle_IsRowAnimationSettled(sActingPokemon, 0x1E) != 0) {
                BattleScene_ResetCameraFrameCounter();
                BattleScene_ClearPhaseFlag();
                BattleScene_CleanupActionPresentation(camera);
            }
            break;
    }
}

void BattleScene_ActionSetupAnimationVariantC(CameraNode* camera) {
    UNUSED s32 pad;
    BattleMonRuntime* pokemon_runtime = &sActingPokemon->state.runtime;

    BattleScene_GetParticipantSideIndex(sActingPokemon);

    switch (sBattleScene.scene->phaseSubstep) {
        case 0:
            BattleScene_ResetCameraFrameCounter();
            BattleScene_SetPhaseFlag();
            Vec3f_SetComponentsDuplicate(&gCameraPreset.startingPosition, sActingPokemon->facingDirection * -175.0f, 25.0f, 0.0f);
            BattleScene_LoadCameraPreset(&D_84384958, camera, sActingPokemon);
            if ((sActingPokemon->displayObject.objectType != DUGTRIO) && (sActingPokemon->displayObject.objectType != DIGLETT)) {
                BattleAnim_PlayMoveEffectScript(sActingPokemon, pokemon_runtime->usedMove);
            }
            break;

        case 1:
            if (BattleScene_AnimateCameraPreset(camera, sActingPokemon, 1, 0) != 0) {
                BattleScene_ResetCameraFrameCounter();
                BattleScene_ClearPhaseFlag();
                BattleScene_CleanupActionPresentation(camera);
            }
            break;
    }
}

void BattleScene_ActionSetupAnimationVariantD(CameraNode* camera) {
    switch (sBattleScene.scene->phaseSubstep) {
        case 0:
            BattleScene_ResetCameraFrameCounter();
            BattleScene_SetPhaseFlag();
            BattleAnim_GetOwnerDefaultAnchorPosition(sActingPokemon, &gCameraPreset.startingPosition);
            BattleScene_LoadCameraPreset(&D_84384924, camera, sActingPokemon);
            break;

        case 1:
            BattleScene_AnimateCameraPresetToOwner(camera, sActingPokemon);
            if (Battle_IsRowAnimationSettled(sActingPokemon, 0x1E) != 0) {
                BattleScene_ResetCameraFrameCounter();
                BattleScene_ClearPhaseFlag();
                BattleScene_CleanupActionPresentation(camera);
            }
            break;
    }
}

void BattleScene_ActionSetupAnimationStep(CameraNode* camera) {
    ActivePokemonState* animated_pokemon_state = &sActivePokemon[sBattleScene.scene->actingSide]->state;
    u32 var_v1 = 0;

    if (sBattleScene.scene->moveEffectKind == 0x18) {
        var_v1 = 10;
    } else if (sBattleScene.scene->moveEffectKind == 0x14) {
        var_v1 = 11;
    } else if (animated_pokemon_state->flags & ACTIVE_UNDERGROUND) {
        var_v1 = 12;
    } else if (animated_pokemon_state->flags & ACTIVE_DIG_EMERGE) {
        var_v1 = 12;
    } else if (animated_pokemon_state->flags & ACTIVE_AIR_ACTION) {
        var_v1 = 13;
    }

    switch (var_v1) {
        case 0:
            BattleScene_ActionSetupAnimationDefault(camera);
            break;

        case 10:
            BattleScene_ActionSetupAnimationVariantA(camera);
            break;

        case 11:
            BattleScene_ActionSetupAnimationVariantB(camera);
            break;

        case 12:
            BattleScene_ActionSetupAnimationVariantC(camera);
            break;

        case 13:
            BattleScene_ActionSetupAnimationVariantD(camera);
            break;

        default:
            BattleScene_ActionSetupAnimationDefault(camera);
            break;
    }
}

void BattleScene_StopSpecialOwnerAnimation(ActivePokemon* pokemon) {
    if (pokemon->state.runtime.usedMove == SEISMIC_TOSS) {
        BattleAnim_ResetOwnerFacingAndExitStatusIdle(pokemon);
    }
}

s32 BattleScene_IsSpecialOwnerAnimation(ActivePokemon* pokemon) {
    if (pokemon->state.runtime.usedMove == FIRE_SPIN) {
        return 1;
    }
    return 0;
}

void BattleScene_CompleteHpUpdateTransition(CameraNode* camera, s32* side_hp_is_consistent, s32* other_side_hp_is_consistent) {
    if ((*side_hp_is_consistent == 1) && (*other_side_hp_is_consistent == 1)) {
        BattleBox_Close();
        Battle_ResetModelToIdleAnim(sActingPokemon);
        sBattleScene.scene->unk_1A = 0;
        BattleScene_ResetCameraFrameCounter();
        BattleScene_ResetCameraSubstepCounter();
        BattleScene_ClearPhaseFlag();
        BattleScene_ResetAnimationStep();
        BattleScene_UpdateDisplayedHpAndTransition(camera);
    }
}

void BattleScene_RunBasicHpTransition(CameraNode* camera) {
    switch (sBattleScene.scene->phaseSubstep) {
        case 0:
            if (BattleScene_AdvanceOwnerAnimation98(sNonActingPokemon) == 0) {
                BattleScene_SelectCameraShot(sNonActingPokemon, camera, 0);
                BattleScene_ResetCameraFrameCounter();
                BattleScene_SetPhaseFlag();
                BattleScene_EnterStateRow(sActingPokemon, 0xE);
                BattleScene_CleanupOpponentSpecialAnimFlag(sNonActingPokemon);
                Battle_ApplyAnimStateEffects(sNonActingPokemon);
                BattleBox_Open(1);
            }
            break;

        case 1:
            BattleScene_FrameCamera(camera, sNonActingPokemon);
            if ((BattleScene_IsOpponentRowCheckpointReached(sNonActingPokemon) != 0) || (BattleScene_AdvanceCameraFrameCounter(0x5A) != 0)) {
                sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sNonActingPokemon);
                sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActingPokemon);
                BattleScene_CompleteHpUpdateTransition(camera, &sSide1HpConsistent, &sSide2HpConsistent);
            }
            break;
    }
}

void BattleScene_RunConditionalHpTransition(CameraNode* camera) {
    s32 sp1C;

    switch (sBattleScene.scene->phaseSubstep) {
        case 0:
            if (BattleScene_AdvanceOwnerAnimation98(sNonActingPokemon) == 0) {
                Battle_ApplyAnimStateEffects(sNonActingPokemon);
                BattleScene_SelectCameraShot(sNonActingPokemon, camera, 0);
                BattleScene_ResetCameraFrameCounter();
                BattleScene_SetPhaseFlag();
                BattleScene_EnterStateRow(sActingPokemon, 0xB);
                BattleScene_CleanupOpponentSpecialAnimFlag(sNonActingPokemon);
                BattleBox_Open(1);
            }
            break;

        case 1:
            BattleScene_FrameCamera(camera, sNonActingPokemon);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sNonActingPokemon);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActingPokemon);
            sp1C = BattleScene_GetOpponentEffectCheckpoint(sActingPokemon);
            if ((sp1C == 2) && ((BattleScene_IsOpponentRowCheckpointReached(sNonActingPokemon) != 0) || (BattleScene_AdvanceCameraFrameCounter(0x64) != 0))) {
                BattleScene_CompleteHpUpdateTransition(camera, &sSide1HpConsistent, &sSide2HpConsistent);
            } else if (sp1C == 1) {
                BattleScene_CompleteHpUpdateTransition(camera, &sSide1HpConsistent, &sSide2HpConsistent);
            }
            break;
    }
}

void BattleScene_RunMoveEffectEndHpTransition(CameraNode* camera) {
    BattleMonRuntime* sp34 = &sActingPokemon->state.runtime;
    UNUSED s32 pad;
    UNUSED s16 pad2;
    s16 sp2C;

    switch (sBattleScene.scene->phaseSubstep) {
        case 0:
            if (BattleScene_AdvanceOwnerAnimation98(sNonActingPokemon) == 0) {
                Battle_ApplyAnimStateEffects(sNonActingPokemon);
                BattleScene_SelectCameraShot(sNonActingPokemon, camera, 0);
                BattleScene_ResetCameraFrameCounter();
                BattleScene_SetPhaseFlag();
                BattleScene_CleanupOpponentSpecialAnimFlag(sNonActingPokemon);
                BattleBox_Open(1);
                sp2C = BattleScene_GetHitCueDefaultAnchor(sNonActingPokemon);
                BattleAnim_QueueMoveEffectEnd(sp34->usedMove, sActingPokemon, sNonActingPokemon, sp2C, BattleScene_GetHitCueAlternateAnchor(sNonActingPokemon));
                BattleScene_SetParticipantModelTransform(sNonActingPokemon, 0x10, 0);
            }
            break;

        case 1:
            BattleScene_FrameCamera(camera, sNonActingPokemon);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sNonActingPokemon);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActingPokemon);
            if ((BattleScene_IsOpponentRowCheckpointReached(sNonActingPokemon) != 0) || (BattleScene_AdvanceCameraFrameCounter(0x5A) != 0)) {
                BattleScene_CompleteHpUpdateTransition(camera, &sSide1HpConsistent, &sSide2HpConsistent);
            }
            break;
    }
}

void BattleScene_RunActionEffectHpTransition(CameraNode* camera) {
    BattleMonRuntime* sp24 = &sNonActingPokemon->state.runtime;
    UNUSED s32 pad;
    s32 sp1C;
    s32 temp_v0;
    s32 var_a0;

    switch (sBattleScene.scene->phaseSubstep) {
        case 0:
            if (BattleScene_AdvanceOwnerAnimation98(sNonActingPokemon) == 0) {
                BattleScene_StopSpecialOwnerAnimation(sActingPokemon);
                BattleBox_Open(1);
                BattleScene_ResetCameraFrameCounter();
                BattleScene_SetPhaseFlag();
                BattleScene_SetDamageShakeAmplitude(sNonActingPokemon);
                if (sBattleScene.scene->unk_1A == 3) {
                    BattleScene_EnterStateRow(sActingPokemon, 0xB);
                } else {
                    BattleScene_EnterStateRow(sActingPokemon, 4);
                }
                BattleScene_SelectCameraShot(sNonActingPokemon, camera, 0);
                Battle_ApplyAnimStateEffects(sNonActingPokemon);
                BattleScene_CleanupOpponentSpecialAnimFlag(sNonActingPokemon);
                BattleAnim_ClearFreezeStatusAnimationIfThawed(sNonActingPokemon);
            }
            break;

        case 1:
            if (BattleScene_AdvanceCameraFrameCounter(5) != 0) {
                BattleScene_ResetOwnerPresentationFlags(sActingPokemon);
                sBattleScene.scene->phaseSubstep++;
            }
            break;

        case 2:
            BattleScene_ResetAnimationStep();
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sNonActingPokemon);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActingPokemon);
            if (sp24->displayHp != 0) {
                if (BattleScene_IsOpponentRowCheckpointReached(sNonActingPokemon) != 0) {
                    sBattleScene.scene->phaseSubstep = 4;
                } else if (BattleScene_IsRowAnimWaitComplete(sNonActingPokemon, 2) != 0) {
                    sBattleScene.scene->phaseSubstep = 5;
                } else if (BattleScene_FrameCamera(camera, sNonActingPokemon) != 0) {
                    sBattleScene.scene->phaseSubstep = 3;
                }
            } else if (BattleScene_FrameCamera(camera, sNonActingPokemon) != 0) {
                sBattleScene.scene->phaseSubstep = 4;
            }
            break;

        case 3:
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sNonActingPokemon);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActingPokemon);
            BattleScene_FrameCamera(camera, sNonActingPokemon);
            if (BattleScene_IsOpponentRowCheckpointReached(sNonActingPokemon) != 0) {
                sBattleScene.scene->phaseSubstep++;
            } else if (BattleScene_IsRowAnimWaitComplete(sNonActingPokemon, 0xF) != 0) {
                sBattleScene.scene->phaseSubstep++;
            }
            break;

        case 4:
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sNonActingPokemon);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActingPokemon);
            temp_v0 = BattleScene_FrameCamera(camera, sNonActingPokemon);
            if ((sSide1HpConsistent == 1) && (sSide2HpConsistent == 1) && (temp_v0 != 0) && (BattleScene_AdvanceCameraFrameCounter(0xF) != 0)) {
                sBattleScene.scene->phaseSubstep = 6;
            }
            break;

        case 5:
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sNonActingPokemon);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActingPokemon);
            sp1C = BattleScene_FrameCamera(camera, sNonActingPokemon);
            if ((BattleScene_IsSpecialOwnerAnimation(sActingPokemon) != 0) && ((sTypeEffectiveness < 0xA) || (sBattleMoveFailed != 0)) &&
                !(sp24->unk_15 & 0x20)) {
                var_a0 = 0x5A;
            } else {
                var_a0 = 0xF;
            }

            if ((sSide1HpConsistent == 1) && (sSide2HpConsistent == 1) && (sp1C != 0) && (BattleScene_AdvanceCameraFrameCounter(var_a0) != 0)) {
                sBattleScene.scene->phaseSubstep++;
            }
            break;

        case 6:
            if (BattleScene_AdvanceOwnerAnimationAndClearFlags(sNonActingPokemon) == 0) {
                sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sNonActingPokemon);
                sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActingPokemon);
                BattleScene_FrameCamera(camera, sNonActingPokemon);
                BattleScene_CompleteHpUpdateTransition(camera, &sSide1HpConsistent, &sSide2HpConsistent);
            }
            break;
    }
}

void func_84324404(void) {
}

void func_8432440C(void) {
}

void BattleScene_RunRepeatedHitHpTransition(CameraNode* camera) {
    BattleMonRuntime* sp24 = &sActingPokemon->state.runtime;
    MovePresentationCue* temp_v1 = &sSideBattleCues[BattleScene_GetParticipantSideIndex(sNonActingPokemon)]->moveCues[sp24->usedMove - 1];
    volatile s32 sp1C;
    s32 temp_v0_2;

    if (sNonActingPokemon->displayObject.objectType == 0x98) {
        sp1C = 0xF;
    } else if (temp_v1->frameCounter == 0xFF) {
        sp1C = sActingPokemon->displayObject.transformAnim.data->endFrame - temp_v1->unk_06;
    } else {
        sp1C = temp_v1->frameCounter - temp_v1->unk_06;
    }

    switch (sBattleScene.scene->phaseSubstep) {
        case 0:
            if (BattleScene_AdvanceOwnerAnimation98(sNonActingPokemon) == 0) {
                BattleScene_ResetAnimationStep();
                BattleScene_SetDamageShakeAmplitude(sNonActingPokemon);
                BattleScene_EnterStateRow(sActingPokemon, 4);
                sMultihitCountdown--;
                BattleScene_SelectCameraShot(sNonActingPokemon, camera, sMultihitCountdown + 1);
                if (sMultihitCountdown < 0) {
                    BattleScene_SelectCameraShot(sNonActingPokemon, camera, sMultihitCountdown + 1);
                    BattleScene_FrameCamera(camera, sNonActingPokemon);
                    Battle_ApplyAnimStateEffects(sNonActingPokemon);
                    BattleScene_ResetCameraFrameCounter();
                    sBattleScene.scene->phaseSubstep = 2;
                    BattleScene_CleanupOpponentSpecialAnimFlag(sNonActingPokemon);
                    BattleBox_Open(1);
                } else {
                    sBattleScene.scene->phaseSubstep++;
                }
            }
            break;

        case 1:
            BattleScene_ReframeAndRecomputeDistance(camera, sNonActingPokemon);
            if (BattleScene_AdvanceCameraFrameCounter(sp1C) != 0) {
                Audio_FadeAndClearStoredSounds();
                BattleAnim_CleanupEffects(1);
                sBattleScene.scene->phaseSubstep = 0;
            }
            break;

        case 2:
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sNonActingPokemon);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActingPokemon);

            if (sp24->displayHp != 0) {
                if (BattleScene_IsOpponentRowCheckpointReached(sNonActingPokemon) != 0) {
                    sBattleScene.scene->phaseSubstep = 5;
                }

                if (BattleScene_IsRowAnimWaitComplete(sNonActingPokemon, 0xF) != 0) {
                    sBattleScene.scene->phaseSubstep = 5;
                }

                if (BattleScene_FrameCamera(camera, sNonActingPokemon) != 0) {
                    sBattleScene.scene->phaseSubstep = 4;
                }
            } else if (BattleScene_FrameCamera(camera, sNonActingPokemon) != 0) {
                BattleScene_ResetCameraFrameCounter();
                sBattleScene.scene->phaseSubstep = 5;
            }
            break;

        case 3:
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sNonActingPokemon);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActingPokemon);

            if (BattleScene_IsOpponentRowCheckpointReached(sNonActingPokemon) != 0) {
                sBattleScene.scene->phaseSubstep = 5;
            }

            if (BattleScene_FrameCamera(camera, sNonActingPokemon) != 0) {
                sBattleScene.scene->phaseSubstep = 9;
            }
            break;

        case 4:
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sNonActingPokemon);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActingPokemon);

            if (BattleScene_IsOpponentRowCheckpointReached(sNonActingPokemon) != 0) {
                sBattleScene.scene->phaseSubstep = 5;
            }

            BattleScene_FrameCamera(camera, sNonActingPokemon);

            if (BattleScene_IsRowAnimWaitComplete(sNonActingPokemon, 0xF) != 0) {
                sBattleScene.scene->phaseSubstep++;
            }
            break;

        case 5:
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sNonActingPokemon);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActingPokemon);
            temp_v0_2 = BattleScene_FrameCamera(camera, sNonActingPokemon);
            if ((sSide1HpConsistent == 1) && (sSide2HpConsistent == 1) && (temp_v0_2 != 0) && (BattleScene_AdvanceCameraFrameCounter(0xF) != 0)) {
                sBattleScene.scene->phaseSubstep++;
            }
            break;

        case 6:
            if (BattleScene_AdvanceOwnerAnimationAndClearFlags(sNonActingPokemon) == 0) {
                sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sNonActingPokemon);
                sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActingPokemon);
                BattleScene_FrameCamera(camera, sNonActingPokemon);
                BattleScene_CompleteHpUpdateTransition(camera, &sSide1HpConsistent, &sSide2HpConsistent);
            }
            break;
    }
}

void BattleScene_ActionSetupFinalizeStep(CameraNode* camera) {
    BattleScene_ResetCameraFov(camera);
    Battle_ApplyAnimStateEffects(sActivePokemon[!sBattleScene.scene->actingSide]);
    if (sBattleScene.scene->moveEffectKind == 0x16) {
        BattleScene_RunBasicHpTransition(camera);
    } else if (sBattleScene.scene->moveEffectKind == 0x20) {
        BattleScene_RunMoveEffectEndHpTransition(camera);
    } else if (sBattleScene.scene->moveEffectKind == 0x1A) {
        BattleScene_RunConditionalHpTransition(camera);
    } else if (sBattleScene.scene->unk_1A == 1) {
        BattleScene_RunRepeatedHitHpTransition(camera);
    } else {
        BattleScene_RunActionEffectHpTransition(camera);
    }
}

void BattleScene_Phase2(CameraNode* camera) {
    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_ActionSetupCameraStep(camera);
            break;

        case 1:
            BattleScene_ActionSetupAnimationStep(camera);
            break;

        case 2:
            BattleScene_ActionSetupFinalizeStep(camera);
            break;
    }
}

void BattleScene_Phase3(CameraNode* camera) {
    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_ActionSetupCameraStep(camera);
            break;

        case 1:
            BattleScene_ActionSetupAnimationStep(camera);
            break;

        case 2:
            BattleScene_ActionSetupFinalizeStep(camera);
            break;
    }
}

s32 BattleScene_PhaseSelectMoveAnim(void) {
    BattleScene_ApplyActionCameraMode(sBattleScene.scene->camera, 0);

    if (BattleScene_AdvanceCameraFrameCounter(BattleScene_GetCameraShotDuration(sActingPokemon)) == 0) {
        return 0;
    }

    if ((sBattleScene.scene->moveEffectKind != 0x1E) && (sBattleScene.scene->moveEffectKind != 0x27)) {
        BattleBox_Open(3);
    }

    if (sBattleScene.scene->moveEffectKind == 0x1B) {
        QueuePlayerDefeatMessage(BattleScene_GetParticipantDisplayPlayerNumber(sActingPokemon));
    }

    switch (sBattleScene.scene->moveEffectKind) {
        case 7:
            return BattleScene_TransitionToPhaseFull(7);

        case 21:
            return BattleScene_TransitionToPhaseFull(7);

        case 39:
            return BattleScene_TransitionToPhaseFull(7);

        case 27:
            return BattleScene_TransitionToPhaseFull(0x1B);

        case 30:
            return BattleScene_TransitionToPhaseFull(0x1E);

        case 28:
            return BattleScene_TransitionToPhaseFull(0x1C);

        case 15:
            return BattleScene_TransitionToPhaseFull(0xF);

        case 25:
            return BattleScene_TransitionToPhaseFull(0x19);

        case 29:
            return BattleScene_TransitionToPhaseFull(0x1D);

        case 38:
            return BattleScene_TransitionToPhaseFull(0x26);

        case 12:
            return BattleScene_TransitionToPhaseFull(0xC);

        case 13:
            return BattleScene_TransitionToPhaseFull(0xD);

        case 14:
            return BattleScene_TransitionToPhaseFull(0xE);

        case 17:
            return BattleScene_TransitionToPhaseFull(0x11);

        case 31:
            return BattleScene_TransitionToPhaseFull(0x1F);
    }

    return 0;
}

void BattleScene_Phase7(CameraNode* camera) {
    BattleMonRuntime* sp1C = &sActingPokemon->state.runtime;
    MovePresentationCue* sp18 = &sSideBattleCues[BattleScene_GetParticipantSideIndex(sActingPokemon)]->moveCues[sp1C->usedMove - 1];

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_SaveCameraFov(camera);
            BattleScene_SelectActionCameraPresetFromTiming(camera, 0);
            BattleScene_ResetCameraFrameCounter();
            BattleScene_ResetCameraSubstepCounter();
            if (sp18->animationDuration < 0x15) {
                Battle_ApplyAnimStateEffects(sActingPokemon);
            }
            sBattleScene.scene->phaseStep++;
            break;

        case 1:
            BattleScene_ApplyActionCameraMode(camera, 1);
            if (sActingPokemon->animationTimer == (sp18->animationDuration + 1)) {
                Battle_ApplyAnimStateEffects(sActingPokemon);
            }

            if (sActingPokemon->animationTimer >= (sp18->animationDuration + 0xA)) {
                sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
                sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            }

            if ((sActingPokemon->animationTimer == (sp18->animationDuration + 0x12)) && (sp1C->usedMove == 0x64)) {
                BattleBox_Open(0xF);
            }

            if (Battle_IsRowAnimationSettled(sActingPokemon, 0x3C) != 0) {
                BattleScene_ResetCameraFrameCounter();
                sBattleScene.scene->phaseStep++;
            }
            break;

        case 2:
            if (sActingPokemon->animationTimer == (sp18->animationDuration + 1)) {
                Battle_ApplyAnimStateEffects(sActingPokemon);
            }
            BattleScene_ApplyActionCameraMode(camera, 1);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            if ((sSide1HpConsistent == 1) && (sSide2HpConsistent == 1) && (BattleScene_AdvanceCameraFrameCounter(0xF) != 0)) {
                if ((sNonActingPokemon->state.runtime.usedMove & 0xF0) == 0xF0) {
                    sBattleScene.scene->unk_34 = 3;
                }
                BattleScene_ResetCameraFrameCounter();
                BattleScene_ResetAnimationStep();
                BattleScene_WaitForDisplayedHpAnimations(camera);
            }
            break;
    }
}

void BattleScene_Phase38(CameraNode* camera) {
    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_SaveCameraFov(camera);
            BattleScene_SetCameraOwnerShot(camera, sActingPokemon);
            BattleScene_UpdateCameraModelOffset(sActingPokemon, camera);
            Battle_ApplyAnimStateEffects(sActingPokemon);
            BattleScene_ResetCameraFrameCounter();
            BattleScene_ResetCameraSubstepCounter();
            sBattleScene.scene->phaseStep++;
            break;

        case 1:
            if (Battle_IsRowAnimationSettled(sActingPokemon, 0x3C) != 0) {
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 2:
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            if ((sNonActingPokemon->state.runtime.usedMove & 0xF0) == 0xF0) {
                sBattleScene.scene->unk_34 = 3;
            }
            BattleScene_ResetCameraFrameCounter();
            BattleScene_ResetAnimationStep();
            BattleScene_WaitForDisplayedHpAnimations(camera);
            break;
    }
}

void BattleScene_Phase27(CameraNode* camera) {
    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_SaveCameraFov(camera);
            Battle_ApplyAnimStateEffects(sActingPokemon);
            BattleScene_SelectActionCameraPresetFromTiming(camera, 0);
            BattleScene_ResetCameraFrameCounter();
            sBattleScene.scene->phaseStep++;
            break;

        case 1:
            BattleScene_ApplyActionCameraMode(camera, 1);
            if (BattleScene_AdvanceCameraFrameCounter(0x28) != 0) {
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 2:
            BattleScene_ApplyActionCameraMode(camera, 1);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            if ((sNonActingPokemon->state.runtime.usedMove & 0xF0) == 0xF0) {
                sBattleScene.scene->unk_34 = 3;
            }
            BattleScene_ResetCameraFrameCounter();
            BattleScene_ResetAnimationStep();
            BattleScene_WaitForDisplayedHpAnimations(camera);
            break;
    }
}

void BattleScene_Phase30(CameraNode* camera) {
    BattleMonRuntime* non_acting_pokemon_runtime = &sNonActingPokemon->state.runtime;

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            Battle_ClearRuntimeFlags(sActingPokemon, ACTIVE_SUBSTITUTE | ACTIVE_SUB_BREAK);
            BattleScene_SaveCameraFov(camera);
            BattleScene_ResetCameraFrameCounter();
            BattleScene_SelectActionCameraPresetFromTiming(camera, 0);
            Battle_ApplyAnimStateEffects(sActingPokemon);
            BattleScene_ResetCameraFrameCounter();
            sBattleScene.scene->phaseStep++;
            break;

        case 1:
            BattleScene_ApplyActionCameraMode(camera, 1);
            if (Battle_IsRowAnimationSettled(sActingPokemon, 0x3C) != 0) {
                BattleScene_ResetCameraFrameCounter();
                sBattleScene.scene->phaseStep++;
            }
            break;

        case 2:
            if (BattleScene_AdvanceOwnerAnimation98(sNonActingPokemon) == 0) {
                BattleAnim_CleanupEffects(1);
                Audio_FadeAndClearStoredSounds();
                BattleScene_SelectCameraShot(sNonActingPokemon, camera, BattleAnim_RandomRange(2));
                BattleScene_EnterStateRow(sActingPokemon, 4);
                BattleScene_ResetCameraFrameCounter();
                sBattleScene.scene->phaseStep++;
            }
            break;

        case 3:
            if (BattleScene_AdvanceOwnerAnimationAndClearFlags(sNonActingPokemon) == 0) {
                BattleScene_ReframeAndRecomputeDistance(camera, sNonActingPokemon);
                if (sBattleScene.scene->stepFrameTimer == 0) {
                    BattleBox_Open(1);
                }
                sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
                sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
                if (BattleScene_AdvanceCameraFrameCounter(0x32) != 0) {
                    sBattleScene.scene->phaseStep += 1;
                }
                if (BattleScene_IsOpponentRowCheckpointReached(sNonActingPokemon) != 0) {
                    BattleScene_ResetCameraFrameCounter();
                    sBattleScene.scene->phaseStep++;
                }
            }
            break;

        case 4:
            BattleScene_ReframeAndRecomputeDistance(camera, sNonActingPokemon);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            if ((sSide2HpConsistent == 1) && (sSide1HpConsistent == 1)) {
                if (non_acting_pokemon_runtime->displayHp != 0) {
                    BattleAnim_CleanupEffects(1);
                    Audio_FadeAndClearStoredSounds();
                    BattleScene_SelectCameraFollowPreset(sActingPokemon, camera, 3);
                    sBattleScene.scene->phaseStep++;
                } else {
                    if ((sNonActingPokemon->state.runtime.usedMove & 0xF0) == 0xF0) {
                        sBattleScene.scene->unk_34 = 3;
                    }
                    BattleScene_ResetCameraFrameCounter();
                    BattleScene_ResetAnimationStep();
                    BattleScene_SetPhase(camera, 5);
                }
            }
            break;

        case 5:
            BattleScene_SelectCameraFollowPreset(sActingPokemon, camera, 3);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            if ((sNonActingPokemon->state.runtime.usedMove & 0xF0) == 0xF0) {
                sBattleScene.scene->unk_34 = 3;
            }
            BattleScene_ResetCameraFrameCounter();
            BattleScene_ResetAnimationStep();
            BattleScene_ResolveDisplayedHpCompletion(camera);
            break;
    }
}

void BattleScene_Phase28(CameraNode* camera) {
    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_SaveCameraFov(camera);
            Battle_ApplyAnimStateEffects(sActingPokemon);
            BattleScene_SelectCameraFollowPreset(sActingPokemon, camera, 3);
            BattleScene_ResetCameraFrameCounter();
            sBattleScene.scene->phaseStep += 1;
            break;

        case 1:
            if (BattleScene_AdvanceCameraFrameCounter(0x28) != 0) {
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 2:
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            if ((sNonActingPokemon->state.runtime.usedMove & 0xF0) == 0xF0) {
                sBattleScene.scene->unk_34 = 3;
            }
            BattleScene_ResetCameraFrameCounter();
            BattleScene_ResetAnimationStep();
            BattleScene_WaitForDisplayedHpAnimations(camera);
            break;
    }
}

void BattleScene_Phase15(CameraNode* camera) {
    UNUSED s32 pad[3];
    BattleMonRuntime* sp30 = &sActingPokemon->state.runtime;
    BattleMonRuntime* sp2C = &sNonActingPokemon->state.runtime;
    ActivePokemonState* sp28 = &sActingPokemon->state;
    ActivePokemonState* sp24 = &sNonActingPokemon->state;

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_SelectActionCameraPresetFromTiming(camera, 0);
            BattleScene_RecenterCameraOnOwner(camera, sActingPokemon);
            if (sp30->unk_4D & 0x10) {
                if (sp2C->battleStatus3 & 8) {
                    sp28->model = sActingPokemon->sessionTeams->displayModel = sp24->unk_BE;
                } else if (sNonActingPokemon->sessionTeams->displayModel == 0x98) {
                    sp28->model = sActingPokemon->sessionTeams->displayModel = sp2C->id;
                } else {
                    sp28->model = sActingPokemon->sessionTeams->displayModel = sNonActingPokemon->sessionTeams->displayModel;
                }
            } else if (sp2C->battleStatus3 & 8) {
                sActingPokemon->sessionTeams->displayModel = sp24->unk_BE;
            } else if (sNonActingPokemon->sessionTeams->displayModel == 0x98) {
                sActingPokemon->sessionTeams->displayModel = sp2C->id;
            } else {
                sActingPokemon->sessionTeams->displayModel = sNonActingPokemon->sessionTeams->displayModel;
            }
            sActingPokemon->sessionTeams->unk_04 = &sActingPokemon->teamRoster->pokemonPicked[sActingPokemon->state.rosterIndex];
            sActingPokemon->sessionTeams->uniqueStatusFlags |= 0x80;
            sBattleScene.scene->phaseStep += 1;
            break;

        case 1:
            BattleScene_RecenterCameraOnOwner(camera, sActingPokemon);
            if (sActingPokemon->sessionTeams->uniqueStatusFlags & 0x10) {
                sActingPokemon->sessionTeams->uniqueStatusFlags &= ~0x90;
                Battle_LoadOwnerModelForSpecies(sActingPokemon);
                BattleThumbnail_TickOwnerCameraFraming(sActingPokemon);
                BattleAnim_PlayMoveEffectScript(sActingPokemon, sp30->usedMove);
                BattleAnim_PlaySpeciesReactionSound(sActingPokemon, sp30, 0);
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 2:
            BattleScene_RecenterCameraOnOwner(camera, sActingPokemon);
            BattleAnim_PlaySpeciesReactionSound(sActingPokemon, sp30, 0);
            BattleScene_EnterStateRow(sActingPokemon, 0x12);
            sBattleScene.scene->phaseStep += 1;
            break;

        case 3:
            BattleScene_RecenterCameraOnOwner(camera, sActingPokemon);
            sActingPokemon->thumbnail.camera->node.flags |= 1;
            sActingPokemon->thumbnail.displayObject.node.flags |= 1;
            BattleThumbnail_TriggerOwnerModelAnimation(sActingPokemon);
            if (BattleScene_IsRowWaitComplete(sActingPokemon, 0x32) != 0) {
                BattleScene_ResetAnimationStep();
                BattleScene_UpdateDisplayedHpAndTransition(camera);
            }
            break;
    }
}

void BattleScene_Phase25(CameraNode* camera) {
    BattleMonRuntime* sp1C = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;
    ActivePokemonState* sp18 = &sActivePokemon[sBattleScene.scene->actingSide]->state;

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_SaveCameraFov(camera);
            BattleAnim_PlayMoveEffectScript(sActingPokemon, sp1C->usedMove);
            BattleScene_SelectActionCameraPresetFromTiming(camera, 0);
            if (!(sp18->flags & 0x800)) {
                Battle_MakeSubstitute(sActingPokemon);
            }
            sBattleScene.scene->phaseStep += 1;
            break;

        case 1:
            BattleScene_ApplyActionCameraMode(camera, 0);
            if (!(sp18->flags & 0x800)) {
                BattleScene_ResetOwnerModelIfLoaded(sActingPokemon);
            }
            if (BattleAnim_IsSessionBusy(sActingPokemon) == 0) {
                BattleAnim_PlaySpeciesReactionSound(sActingPokemon, sp1C, 0);
                BattleScene_SelectCameraFollowPreset(sActingPokemon, camera, 3);
                BattleScene_ResetCameraFrameCounter();
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 2:
            BattleScene_SelectCameraFollowPreset(sActingPokemon, camera, 3);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            if (BattleScene_AdvanceCameraFrameCounter(0x2E) != 0) {
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 3:
            if (!(sp18->flags & 0x800)) {
                Battle_SetRuntimeFlags(sActingPokemon, 0x800);
            }
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            if ((sNonActingPokemon->state.runtime.usedMove & 0xF0) == 0xF0) {
                sBattleScene.scene->unk_34 = 3;
            }
            BattleScene_ResetAnimationStep();
            BattleScene_WaitForDisplayedHpAnimations(camera);
            break;
    }
}

s32 BattleScene_EndStatusAnimationsAndCommit(ActivePokemon* arg0) {
    BattleAnim_ClearFreezeStatusAnimation(arg0);
    BattleAnim_EndSleepStatusAnimation(arg0);
    Battle_ApplyAnimStateEffects(arg0);
}

void BattleScene_Phase29(CameraNode* camera) {
    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_SaveCameraFov(camera);
            Battle_ApplyAnimStateEffects(sActingPokemon);
            BattleScene_SelectActionCameraPresetFromTiming(camera, 0);
            BattleScene_ResetCameraFrameCounter();
            BattleScene_ResetCameraSubstepCounter();
            sBattleScene.scene->phaseStep += 1;
            break;

        case 1:
            BattleScene_ApplyActionCameraMode(camera, 1);
            if (Battle_IsRowAnimationSettled(sActingPokemon, 0x3C) != 0) {
                BattleScene_EndStatusAnimationsAndCommit(sNonActingPokemon);
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 2:
            BattleScene_ApplyActionCameraMode(camera, 1);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            if ((sNonActingPokemon->state.runtime.usedMove & 0xF0) == 0xF0) {
                sBattleScene.scene->unk_34 = 3;
            }
            Vec3f_SetComponentsDuplicate(&sActingPokemon->displayObject.scale, 1.0f, 1.0f, 1.0f);
            Vec3f_SetComponentsDuplicate(&sNonActingPokemon->displayObject.scale, 1.0f, 1.0f, 1.0f);
            BattleScene_ResetCameraFrameCounter();
            BattleScene_ResetAnimationStep();
            BattleScene_WaitForDisplayedHpAnimations(camera);
            break;
    }
}

void BattleScene_Phase12(CameraNode* camera) {
    MovePresentationCue* sp4C = &sSideBattleCues[sBattleScene.scene->actingSide]->flyingIdleCue;
    UNUSED s32 pad;
    Vec3f sp3C;

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_ResetCameraFrameCounter();
            BattleScene_ClearPhaseFlag();
            BattleScene_SelectCameraFollowPreset(sActingPokemon, camera, 3);
            BattleScene_ApproachCameraTargetFromOwner(camera, sActingPokemon);
            Battle_SetRuntimeFlags(sActingPokemon, 0x200);
            sBattleScene.scene->phaseStep += 1;
            break;

        case 1:
            if (sActingPokemon->displayObject.position.y >= 100.0f) {
                BattleAnim_GetOwnerDefaultAnchorPosition(sActingPokemon, &sp3C);
                sp3C.y += BattleAnim_GetOwnerModelHeight(sActingPokemon);
                D_84390208 = BattleScene_ScaleCameraDistance(sActingPokemon, 3.0f, 500.0f);
                BattleScene_SetCameraEyeFromPosition(camera, sp3C, 0x11C6, BattleScene_GetParticipantFacingAngle(sActingPokemon) - (sActingPokemon->facingDirection * 0x754E),
                              D_84390208, BattleAnim_GetOwnerModelHeight(sActingPokemon));
                Vec3f_SetComponentsDuplicate(&sp3C, 0.0f, 200.0f, 0.0f);
                BattleAnim_Vec3fCopy(&camera->view.targetPosition, &sp3C);
                BattleAnim_CleanupEffects(2);
                ModelAnim_SetAnimation(&sActingPokemon->displayObject, sp4C->id);
                camera->perspective.verticalFieldOfView = 60.0f;
                sBattleScene.scene->phaseStep += 1;
            } else {
                BattleScene_ApproachCameraTargetFromOwner(camera, sActingPokemon);
            }
            break;

        case 2:
            BattleAnim_ApproachF(&camera->perspective.verticalFieldOfView, 60.0f, 0.2f);
            if (sActingPokemon->displayObject.position.y >= 200.0f) {
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 3:
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            if ((sSide1HpConsistent == 1) && (sSide2HpConsistent == 1) && (BattleScene_AdvanceCameraFrameCounterUntil(0x1E) != 0)) {
                Battle_SetRuntimeFlags(sActivePokemon[sBattleScene.scene->actingSide], 4);
                BattleAnim_ClearEffectStateFlag();
                BattleScene_ResetCameraFrameCounter();
                BattleScene_ResetAnimationStep();
                BattleScene_UpdateDisplayedHpAndTransition(camera);
            }
            break;
    }
}

void BattleScene_Phase13(CameraNode* camera) {
    UNUSED s32 pad;
    f32 sp28;
    UNUSED s16 pad2;
    u16 sp24 = D_84385B8C;

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_SaveCameraFov(camera);
            BattleScene_ResetCameraFrameCounter();
            BattleScene_ClearPhaseFlag();
            BattleScene_SelectCameraFollowPreset(sActingPokemon, camera, 3);
            sBattleScene.scene->phaseStep += 1;
            break;

        case 1:
            if (sActingPokemon->modelTransformTimer == 2) {
                BattleScene_UpdateCameraVerticalFraming(camera, sActingPokemon);
                sActingPokemon->displayObject.node.renderFlags &= 0xFFBF;
            }

            if ((sActingPokemon->displayObject.objectType == 0x32) || (sActingPokemon->displayObject.objectType == 0x33)) {
                if (BattleScene_IsRowWaitComplete(sActingPokemon, 0x1E) != 0) {
                    BattleBox_Close();
                    Battle_SetRuntimeFlags(sActingPokemon, 0x4000);
                    sActingPokemon->displayObject.node.renderFlags &= 0xFFDF;
                    sBattleScene.scene->phaseStep += 1;
                }
            } else if (BattleAnim_IsAssetLoadBusy(sActingPokemon) == 0) {
                if (BattleScene_ByteArrayContains(sActingPokemon->displayObject.objectType, (u8*)&sp24, 2) != 0) {
                    sp28 = 10.0f;
                } else {
                    sp28 = 3.5f;
                }

                if (sActingPokemon->displayObject.position.y <= (-BattleAnim_GetOwnerModelHeight(sActingPokemon) * sp28)) {
                    Battle_SetRuntimeFlags(sActingPokemon, 0x4000);
                    sActingPokemon->displayObject.node.renderFlags &= 0xFFDF;
                    sBattleScene.scene->phaseStep += 1;
                }
            }
            break;

        case 2:
            if ((sActingPokemon->displayObject.objectType != 0x32) && (sActingPokemon->displayObject.objectType != 0x33)) {
                sActingPokemon->displayObject.node.renderFlags |= 0x40;
            }
            BattleScene_UpdateCameraVerticalFraming(camera, sActingPokemon);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            BattleScene_ResetAnimationStep();
            BattleScene_WaitForDisplayedHpAnimations(camera);
            break;
    }
}

void BattleScene_Phase14(CameraNode* camera) {
    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_SelectActionCameraPresetFromTiming(camera, 1);
            BattleScene_SaveCameraFov(camera);
            BattleScene_ResetCameraFrameCounter();
            BattleScene_ResetCameraSubstepCounter();
            Battle_ApplyAnimStateEffects(sActingPokemon);
            sBattleScene.scene->phaseStep += 1;
            break;

        case 1:
            BattleScene_ApplyActionCameraMode(camera, 1);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            if (BattleScene_IsMoveKeyedRowWaitComplete(sActingPokemon, 0x1E) != 0) {
                BattleScene_ResetAnimationStep();
                BattleScene_WaitForDisplayedHpAnimations(camera);
            }
            break;
    }
}

void BattleScene_Phase17(CameraNode* camera) {
    ActivePokemonState* acting_pokemon_state = &sActivePokemon[sBattleScene.scene->actingSide]->state;

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_ResetCameraFrameCounter();
            BattleScene_SaveCameraFov(camera);
            Battle_ApplyAnimStateEffects(sActingPokemon);
            if (!(acting_pokemon_state->flags & 2)) {
                BattleScene_SelectActionCameraPresetFromTiming(camera, 0);
            }
            sBattleScene.scene->phaseStep += 1;
            break;

        case 1:
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            if (BattleScene_AdvanceCameraFrameCounter(0x3C) != 0) {
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 2:
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            BattleScene_ResetAnimationStep();
            BattleScene_WaitForDisplayedHpAnimations(camera);
            break;
    }
}

void BattleScene_PhaseFaint(CameraNode* camera) {
    BattleScene_ResetCameraFov(camera);

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            if (BattleScene_AdvanceOwnerAnimation98(sActingPokemon) == 0) {
                BattleBox_Open(2);
                BattleScene_ResetOwnerPresentationFlags(sActingPokemon);
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 1:
            BattleScene_ResetAnimationStep();
            BattleScene_ClearStateTickCallbacks();
            BattleScene_EnterStateRow(sActingPokemon, 0xF);
            BattleScene_SelectCameraShot(sActingPokemon, camera, 3);
            BattleScene_SetDamageShakeAmplitude(sActingPokemon);
            BattleAnim_QueueEffectList(8, sActingPokemon);
            sBattleScene.scene->phaseStep += 1;
            break;

        case 2:
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            BattleScene_FrameCamera(camera, sActingPokemon);
            if (BattleScene_IsRowZeroCheckpointReached(sActingPokemon) != 0) {
                sBattleScene.scene->phaseStep += 1;
            } else if (BattleScene_IsRowWaitComplete(sActingPokemon, 0x32) != 0) {
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 3:
            BattleScene_FrameCamera(camera, sActingPokemon);
            if (BattleScene_AdvanceOwnerAnimationAndClearFlags(sActingPokemon) == 0) {
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 4:
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            BattleScene_FrameCamera(camera, sActingPokemon);
            BattleScene_ResetAnimationStep();
            BattleScene_UpdateDisplayedHpAndTransition(camera);
            break;
    }
}

void BattleScene_Phase31(CameraNode* camera) {
    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_SaveCameraFov(camera);
            Battle_ApplyAnimStateEffects(sActingPokemon);
            sActingPokemon->unk_4C8 = 1;
            BattleScene_ResetCameraFrameCounter();
            sBattleScene.scene->phaseStep += 1;
            break;

        case 1:
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            if (BattleScene_AdvanceCameraFrameCounter(0x3C) != 0) {
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 2:
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            BattleScene_ResetAnimationStep();
            BattleScene_WaitForDisplayedHpAnimations(camera);
            break;
    }
}

void func_84326A70(void) {
}

void BattleScene_Phase4(UNUSED CameraNode* camera) {
    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_BeginBattlePhase();

        case 1:
            break;
    }
}

void BattleScene_Phase16(CameraNode* camera) {
    UNUSED s32 pad;
    s32 idx = sBattleScene.scene->actingSide;
    MovePresentationCue* sp1C = &sSideBattleCues[idx]->hitCue;

    BattleScene_ResetCameraFov(camera);

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_ResetAnimationStep();
            BattleBox_Open(2);
            sBattleScene.scene->phaseStep += 1;
            break;

        case 1:
            if (BattleScene_AdvanceOwnerAnimation98(sActingPokemon) == 0) {
                Battle_ResetModelToIdleAnim(sActingPokemon);
                BattleAnim_CleanupEffects(1);
                BattleScene_EnterStateRow(sActingPokemon, 0xF);
                BattleScene_SelectCameraShot(sActingPokemon, camera, 3);
                BattleScene_SetDamageShakeAmplitude(sActingPokemon);
                BattleScene_ResetAnimationStep();
                ModelAnim_SetAnimation(&sActingPokemon->displayObject, sp1C->id);
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 2:
            if (BattleScene_AdvanceCameraFrameCounter(0x28) != 0) {
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 3:
            if (BattleScene_AdvanceOwnerAnimationAndClearFlags(sActingPokemon) == 0) {
                BattleBox_Close();
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 4:
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            if ((sSide1HpConsistent != 0) && (sSide2HpConsistent != 0)) {
                BattleScene_ResetAnimationStep();
                BattleScene_UpdateDisplayedHpAndTransition(camera);
            }
            break;
    }
}

void BattleScene_PhaseMoveFailedMessage(CameraNode* camera) {
    ActivePokemonState* non_acting_pokemon_state = &sNonActingPokemon->state;

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            if (BattleScene_AdvanceOwnerAnimation98(sNonActingPokemon) == 0) {
                BattleBox_Open(2);
                BattleScene_StopSpecialOwnerAnimation(sActingPokemon);
                BattleScene_ResetCameraFov(camera);
                BattleAnim_CleanupEffects(1);

                if (non_acting_pokemon_state->flags & ACTIVE_AIRBORNE) {
                    BattleScene_SelectCameraShot(sNonActingPokemon, camera, sNormalOrFlyingAnimationRNG = 2);
                } else {
                    if (sNonActingPokemon->displayObject.objectType == BEEDRILL) {
                        BattleScene_SelectCameraShot(sNonActingPokemon, camera, 0);
                    }
                    BattleScene_SelectCameraShot(sNonActingPokemon, camera, sNormalOrFlyingAnimationRNG = BattleAnim_RandomRange(2));
                }

                BattleScene_EnterStateRow(sActingPokemon, 0xC);
                BattleScene_ResetCameraFrameCounter();
                BattleAnim_UpdateMoveFailedFlag();
                sBattleScene.scene->phaseStep += 1;
                BattleScene_CleanupOpponentAnimationFlags(sNonActingPokemon);
            }
            break;

        case 1:
            if ((sNonActingPokemon->displayObject.objectType == BEEDRILL) && !(non_acting_pokemon_state->flags & ACTIVE_AIRBORNE)) {
                BattleScene_FrameCamera(camera, sNonActingPokemon);
            }

            if (BattleScene_AdvanceCameraFrameCounter(0x32) != 0) {
                BattleBox_Close();
                BattleScene_ResetOwnerPresentationFlags(sActingPokemon);
                BattleScene_CleanupParticipantEffectMode(sActingPokemon);
                BattleScene_ClearPhaseFlag();
                BattleScene_ResetCameraFrameCounter();
                BattleScene_ResetAnimationStep();
                BattleScene_UpdateDisplayedHpAndTransition(camera);
            }
            break;
    }
}

void BattleScene_PhaseAttackHit(CameraNode* camera) {
    BattleMonRuntime* sp24 = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;
    BattleMonRuntime* sp20 = &sActivePokemon[!sBattleScene.scene->actingSide]->state.runtime;

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            Battle_StopBeingGoldAfterHarden(sActingPokemon);
            BattleScene_SetCameraOwnerStandardShot(camera, sActivePokemon[sBattleScene.scene->actingSide]);
            BattleScene_ResetAnimationStep();
            BattleScene_CleanupParticipantEffectMode(sActingPokemon);
            BattleAnim_CleanupEffects(2);
            sBattleScene.scene->phaseStep += 1;
            break;

        case 1:
            if (BattleScene_AdvanceOwnerAnimation(sActivePokemon[sBattleScene.scene->actingSide]) == 0) {
                Battle_ResetModelToIdleAnim(sActingPokemon);
                Audio_FadeAndClearStoredSounds();
                BattleAnim_CleanupEffects(1);
                BattleScene_SetCameraOwnerStandardShot(camera, sActivePokemon[sBattleScene.scene->actingSide]);
                BattleScene_EnterStateRow(sActivePokemon[sBattleScene.scene->actingSide], 0xD);

                if (sp24->unk_15 & 0x18) {
                    BattleBox_Open(4);
                    sp24->displayHp = sp24->unk_0E;
                    sp20->displayHp = sp20->unk_0E;
                    if (sp24->unk_15 & 0x10) {
                        BattleAnim_QueueEffectList(6, sActingPokemon);
                    } else {
                        BattleAnim_QueueEffectList(5, sActingPokemon);
                    }
                    sBattleScene.scene->phaseStep += 1;
                } else {
                    BattleBox_Open(5);
                    Audio_PlayCommand(0x22, 0, 0);
                    sp24->displayHp = sp24->unk_10;
                    sp20->displayHp = sp20->unk_10;
                    BattleAnim_QueueEffectList(0xA, sActingPokemon);
                    BattleScene_ResetCameraFrameCounter();
                    sBattleScene.scene->phaseStep = 3;
                }
            }
            break;

        case 2:
            BattleScene_SetCameraOwnerStandardShot(camera, sActivePokemon[sBattleScene.scene->actingSide]);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);

            if ((sSide1HpConsistent == 1) && (sSide2HpConsistent == 1) && (BattleScene_AdvanceCameraFrameCounterUntil(0x32) != 0)) {
                BattleScene_ResetCameraFrameCounter();
                if (Battle_GetActiveMonHp(sActivePokemon[sBattleScene.scene->actingSide]) == 0) {
                    BattleScene_ClearPhaseFlag();
                    BattleScene_SetPhase(camera, 5);
                    sBattleScene.scene->actingSide = !sBattleScene.scene->actingSide;
                } else if (sp24->unk_4D & 0x80) {
                    BattleScene_ResetCameraFrameCounter();
                    Audio_PlayCommand(0x22, 0, 0);
                    sp24->displayHp = sp24->unk_10;
                    sp20->displayHp = sp20->unk_10;
                    BattleAnim_QueueEffectList(0xA, sActingPokemon);
                    sBattleScene.scene->phaseStep += 1;
                } else {
                    sBattleScene.scene->phaseStep = 4;
                }
            }
            break;

        case 3:
            BattleScene_SetCameraOwnerStandardShot(camera, sActivePokemon[sBattleScene.scene->actingSide]);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);

            if ((BattleScene_AdvanceCameraFrameCounterUntil(0x32) != 0) && (sSide1HpConsistent == 1) && (sSide2HpConsistent == 1)) {
                sBattleScene.scene->phaseStep += 1;
                BattleScene_ResetCameraFrameCounter();
            }
            break;

        case 4:
            if (BattleScene_AdvanceOwnerAnimationAndClearFlags(sNonActingPokemon) == 0) {
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 5:
            BattleScene_ResetAnimationStep();
            BattleScene_SetCameraOwnerStandardShot(camera, sActivePokemon[sBattleScene.scene->actingSide]);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            BattleScene_ResolveDisplayedHpCompletion(camera);
            break;
    }
}

void BattleScene_PhaseDrainDamage(CameraNode* camera) {
    BattleMonRuntime* sp1C = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;
    BattleMonRuntime* sp18 = &sActivePokemon[!sBattleScene.scene->actingSide]->state.runtime;

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_CleanupParticipantEffectMode(sActingPokemon);
            BattleAnim_CleanupEffects(2);
            BattleScene_SetCameraOwnerWideShot(camera, sActivePokemon[sBattleScene.scene->actingSide]);
            sBattleScene.scene->phaseStep += 1;
            break;

        case 1:
            if (BattleScene_AdvanceOwnerAnimation(sActivePokemon[sBattleScene.scene->actingSide]) == 0) {
                Battle_ResetModelToIdleAnim(sActingPokemon);
                Audio_FadeAndClearStoredSounds();
                BattleAnim_CleanupEffects(1);
                BattleAnim_QueueEffectList(9, sActingPokemon);
                BattleScene_ResetCameraFrameCounter();
                BattleScene_SetCameraOwnerWideShot(camera, sActivePokemon[sBattleScene.scene->actingSide]);
                sp1C->displayHp = sp1C->unk_12;
                sp18->displayHp = sp18->unk_12;
                BattleBox_Open(6);
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 2:
            if (BattleScene_ApproachCloseCameraShot(camera, sActivePokemon[sBattleScene.scene->actingSide]) != 0) {
                BattleScene_SelectCameraShot(sActingPokemon, camera, 3);
                BattleScene_StartHitCueAnimation(sActingPokemon);
                BattleAnim_PlayBattleSequenceById(sActingPokemon, 0x1F);
                BattleScene_SetDamageShakeAmplitude(sActingPokemon);
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 3:
            if (BattleScene_AdvanceOwnerAnimationAndClearFlags(sNonActingPokemon) == 0) {
                BattleScene_FrameCamera(camera, sActingPokemon);
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 4:
            BattleScene_ResetAnimationStep();
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            BattleScene_FrameCamera(camera, sActingPokemon);
            if ((sSide1HpConsistent == 1) && (sSide2HpConsistent == 1) && (BattleScene_AdvanceCameraFrameCounterUntil(0x3C) != 0)) {
                BattleScene_UpdateDisplayedHpAndTransition(camera);
            }
            break;
    }
}

void BattleScene_Phase10(CameraNode* camera) {
    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_CleanupParticipantEffectMode(sActingPokemon);
            if (BattleScene_AdvanceOwnerAnimation(sActivePokemon[sBattleScene.scene->actingSide]) == 0) {
                Battle_ResetModelToIdleAnim(sActingPokemon);
                Audio_FadeAndClearStoredSounds();
                BattleAnim_CleanupEffects(1);
                BattleScene_ResetCameraFrameCounter();
                BattleAnim_QueueEffectList6(3, sNonActingPokemon);
                Audio_PlayCommand(0x23, 0, 0);
                BattleScene_SetCameraOwnerStandardShot(camera, sNonActingPokemon);
                BattleBox_Open(7);
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 1:
            if (BattleScene_AdvanceCameraFrameCounter(0x37) != 0) {
                sBattleScene.scene->phaseStep += 1;
            }

        case 2:
            if (BattleScene_AdvanceCameraFrameCounter(0x3C) != 0) {
                BattleScene_UpdateDisplayedHpAndTransition(camera);
            }
            break;
    }
}

void BattleScene_PhaseAttackHitRecoil(CameraNode* camera) {
    BattleMonRuntime* sp24 = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;
    BattleMonRuntime* sp20 = &sActivePokemon[!sBattleScene.scene->actingSide]->state.runtime;

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_CleanupParticipantEffectMode(sActingPokemon);
            BattleAnim_CleanupEffects(2);
            sBattleScene.scene->phaseStep += 1;
            break;

        case 1:
            if (BattleScene_AdvanceOwnerAnimation(sActivePokemon[sBattleScene.scene->actingSide]) == 0) {
                Battle_ResetModelToIdleAnim(sActingPokemon);
                Audio_FadeAndClearStoredSounds();
                BattleAnim_CleanupEffects(1);
                BattleScene_SetCameraOwnerStandardShot(camera, sActivePokemon[sBattleScene.scene->actingSide]);
                BattleScene_EnterStateRow(sActivePokemon[sBattleScene.scene->actingSide], 0xD);

                if (sp24->unk_15 & 0x18) {
                    BattleBox_Open(4);
                    sp24->displayHp = sp24->unk_0E;
                    sp20->displayHp = sp20->unk_0E;
                    if (sp24->unk_15 & 0x10) {
                        BattleAnim_QueueEffectList(6, sActingPokemon);
                    } else {
                        BattleAnim_QueueEffectList(5, sActingPokemon);
                    }
                    sBattleScene.scene->phaseStep += 1;
                } else {
                    BattleBox_Open(5);
                    sp24->displayHp = sp24->unk_10;
                    sp20->displayHp = sp20->unk_10;
                    BattleAnim_QueueEffectList(0xA, sActingPokemon);
                    sBattleScene.scene->phaseStep = 3;
                }
            }
            break;

        case 2:
            BattleScene_SetCameraOwnerStandardShot(camera, sActivePokemon[sBattleScene.scene->actingSide]);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            if ((sSide1HpConsistent == 1) && (sSide2HpConsistent == 1) && (BattleScene_AdvanceCameraFrameCounterUntil(0x32) != 0)) {
                BattleScene_ResetCameraFrameCounter();
                if (Battle_GetActiveMonHp(sActivePokemon[sBattleScene.scene->actingSide]) == 0) {
                    BattleScene_ClearPhaseFlag();
                    BattleScene_SetPhase(camera, 5);
                    sBattleScene.scene->actingSide = !sBattleScene.scene->actingSide;
                } else if (sp24->unk_4D & 0x80) {
                    Audio_PlayCommand(0x22, 0, 0);
                    sp24->displayHp = sp24->unk_10;
                    sp20->displayHp = sp20->unk_10;
                    BattleAnim_QueueEffectList(0xA, sActingPokemon);
                    sBattleScene.scene->phaseStep += 1;
                } else {
                    sBattleScene.scene->phaseStep = 4;
                }
            }
            break;

        case 3:
            BattleScene_SetCameraOwnerStandardShot(camera, sActivePokemon[sBattleScene.scene->actingSide]);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            if ((BattleScene_AdvanceCameraFrameCounterUntil(0x32) != 0) && (sSide1HpConsistent == 1) && (sSide2HpConsistent == 1)) {
                sBattleScene.scene->phaseStep += 1;
                BattleScene_ResetCameraFrameCounter();
            }
            break;

        case 4:
            if (BattleScene_AdvanceOwnerAnimationAndClearFlags(sNonActingPokemon) == 0) {
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 5:
            BattleScene_ResetAnimationStep();
            BattleScene_SetCameraOwnerStandardShot(camera, sActivePokemon[sBattleScene.scene->actingSide]);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            BattleScene_ResolveDisplayedHpCompletion(camera);
            break;
    }
}

void BattleScene_PhaseCheckFaintedEndTurn(CameraNode* camera) {
    BattleMonRuntime* sp1C = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;
    BattleMonRuntime* sp18 = &sActivePokemon[!sBattleScene.scene->actingSide]->state.runtime;

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_CleanupParticipantEffectMode(sActingPokemon);
            BattleAnim_CleanupEffects(2);
            sBattleScene.scene->phaseStep += 1;
            break;

        case 1:
            Battle_ResetModelToIdleAnim(sActingPokemon);
            Audio_FadeAndClearStoredSounds();
            BattleAnim_CleanupEffects(1);
            BattleScene_CameraShotTargetClose(camera, sActivePokemon[sBattleScene.scene->actingSide]);
            BattleScene_EnterStateRow(sActivePokemon[sBattleScene.scene->actingSide], 0xD);
            sp1C->displayHp = sp1C->unk_0E;
            sp18->displayHp = sp18->unk_0E;
            sBattleScene.scene->phaseStep += 1;
            break;

        case 2:
            BattleScene_CameraShotTargetClose(camera, sActivePokemon[sBattleScene.scene->actingSide]);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = 1;

            if ((sSide1HpConsistent == 1) && (sSide2HpConsistent == 1) && (BattleScene_AdvanceCameraFrameCounterUntil(0x32) != 0)) {
                BattleScene_ResetCameraFrameCounter();
                if (Battle_GetActiveMonHp(sActivePokemon[sBattleScene.scene->actingSide]) == 0) {
                    BattleScene_ClearPhaseFlag();
                    BattleScene_SetPhase(camera, 0x22);
                    sBattleScene.scene->actingSide = !sBattleScene.scene->actingSide;
                } else {
                    BattleScene_CleanupActionAndAdvance();
                }
            }
            break;
    }
}

void BattleScene_ClearMonStatusAnimations(ActivePokemon* arg0) {
    BattleAnim_ClearFreezeStatusAnimation(arg0);
    BattleAnim_ClearSleepStatusAnimation(arg0);
}

void BattleScene_PhaseWithdrawFainted(CameraNode* camera) {
    ActivePokemonState* sp34 = &sActivePokemon[sBattleScene.scene->actingSide]->state;
    ActivePokemon* temp_s0 = sActivePokemon[sBattleScene.scene->actingSide];
    BattleMonRuntime* sp2C = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;

    if (sBattleScene.scene->actingSide) {}

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            if (sBattleScene.scene->unk_34 == 1) {
                BattleScene_ResetCameraFov(camera);
                BattleScene_SelectCameraFollowPreset(temp_s0, camera, 4);
                PlayerDefeated(BattleScene_GetParticipantDisplayPlayerNumber(temp_s0));
                sBattleScene.scene->phaseStep += 1;
            } else if (BattleScene_AnimateCameraPreset(camera, temp_s0, 1, 1) != 0) {
                BattleBox_Open(0xA);
                PlayerDefeated(BattleScene_GetParticipantDisplayPlayerNumber(temp_s0));
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 1:
            BattleScene_ApproachCameraZoom(camera, temp_s0);
            BattleScene_EnterStateRow(temp_s0, 2);
            BattleScene_ClearMonStatusAnimations(temp_s0);
            sBattleScene.scene->phaseStep += 1;
            break;

        case 2:
            BattleScene_ApproachCameraZoom(camera, temp_s0);
            if (BattleScene_AdvanceCameraFrameCounter(0x69) != 0) {
                Vec3f_SetComponentsDuplicate(&camera->view.targetPosition, temp_s0->facingDirection * -150.0f, 0.0, 0.0f);
                if (sBattleScene.scene && sBattleScene.scene) {}
                Vec3f_SetComponentsDuplicate(&camera->view.eyePosition, temp_s0->facingDirection * -50.0f, 5.0f, 0.0f);
                BattleScene_SetParticipantModelTransform(temp_s0, 2, 0);
                BattleAnim_QueueEffectList3(0xD, temp_s0, temp_s0, 0xFF, 0xFF);
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 3:
            if (temp_s0->pokeBall.position.y == 2.5f) {
                temp_s0->pokeBall.node.renderFlags |= 0x20;
            }

            if (ModelAnim_IsAnimationDone(&temp_s0->pokeBall) == 1) {
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 4:
            temp_s0->pokeBall.node.flags &= ~1;
            temp_s0->sessionTeams->unk_04 = &temp_s0->sessionTeams->teamMember[sp34->ownerTeamMemberIndex]->pokemonPicked[sp34->actionChosen];
            temp_s0->sessionTeams->uniqueStatusFlags |= 0x80;
            sBattleScene.scene->phaseStep += 1;
            break;

        case 5:
            if (temp_s0->sessionTeams->uniqueStatusFlags & 0x10) {
                temp_s0->sessionTeams->uniqueStatusFlags &= ~0x90;
                Battle_LoadOwnerModelForSwitchIn(temp_s0);
                BattleThumbnail_TickOwnerCameraFraming(temp_s0);
                sCurrentPokemonHp[sBattleScene.scene->actingSide] = temp_s0->state.runtime.displayHp;
                BattleAnim_QueueEffectList3(3, temp_s0, temp_s0, 0xFF, 0xFF);
                Audio_PlayCommand(2, 0, 0);
                BattleScene_RebindParticipantTeamMember(temp_s0, sp34->ownerTeamMemberIndex);
                camera->perspective.verticalFieldOfView = 40.0f;
                BattleBox_Open(0xC);
                Battle_TriggerTrainerPresentationCue(temp_s0);
                sBattleDamage = 0;
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 6:
            BattleScene_EnterStateRow(temp_s0, 0x12);
            BattleScene_SelectCameraFollowPreset(temp_s0, camera, 3);
            BattleScene_ResetCameraFrameCounter();
            sBattleScene.scene->phaseStep += 1;
            break;

        case 7:
            BattleScene_ReframeAndRecomputeDistanceForSwitch(camera, temp_s0);
            if (BattleScene_IsRowWaitCompleteWithEventCount(temp_s0, 0x3C) != 0) {
                Audio_FadeAndClearStoredSounds();
                BattleAnim_CleanupEffects(2);
                BattleScene_ResetCameraFrameCounter();
                Vec3f_SetComponentsDuplicate(&temp_s0->displayObject.scale, 1.0f, 1.0f, 1.0f);
                if (sp2C->usedMove == 0xFE) {
                    sp2C->usedMove = 0xA5;
                }
                BattleScene_CleanupActionAndAdvance();
                sp34->flags &= ~0x1800;
                BattleBox_Close();
                sBattleScene.scene->unk_54 = !sBattleScene.scene->actingSide + 1;
            }
            break;
    }
}

void BattleScene_PhaseWithdrawSwitch(CameraNode* camera) {
    ActivePokemonState* acting_pokemon_state = &sActivePokemon[sBattleScene.scene->actingSide]->state;
    ActivePokemonState* other_pokemon_state = &sActivePokemon[!sBattleScene.scene->actingSide]->state;
    UNUSED s32 pad;
    ActivePokemon* acting_pokemon = sActivePokemon[sBattleScene.scene->actingSide];
    BattleMonRuntime* acting_pokemon_runtime = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            if (sBattleScene.scene->unk_34 == 1) {
                BattleScene_ResetCameraFov(camera);
                BattleScene_ResetCameraFrameCounter();
                BattleBox_Open(0xB);
                BattleAnim_LoadModeResourceList(acting_pokemon->sessionTeams, 3);
                Vec3f_SetComponentsDuplicate(&camera->view.targetPosition, acting_pokemon->facingDirection * -150.0f, 0.0, 0.0f);
                Vec3f_SetComponentsDuplicate(&camera->view.eyePosition, acting_pokemon->facingDirection * -50.0f, 5.0f, 0.0f);
                Vec3f_SetComponentsDuplicate(&acting_pokemon->displayObject.position, acting_pokemon->facingDirection * -150.0f, 25.0f, 0.0f);
                sBattleScene.scene->phaseStep += 1;
            } else {
                BattleAnim_ApproachF(&camera->perspective.verticalFieldOfView, 30.0f, 0.05f);
                if (BattleScene_AnimateCameraIntro(camera, acting_pokemon) != 0) {
                    BattleScene_ResetCameraFov(camera);
                    BattleScene_ResetCameraFrameCounter();
                    BattleBox_Open(0xB);
                    BattleAnim_LoadModeResourceList(acting_pokemon->sessionTeams, 3);
                    sBattleScene.scene->phaseStep += 1;
                }
            }
            break;

        case 1:
            if (BattleAnim_TryRegisterAssetTables(acting_pokemon) != 0) {
                acting_pokemon->sessionTeams->displayModel = acting_pokemon_runtime->id;
                BattleScene_SetParticipantModelTransform(acting_pokemon, 2, 0);
                BattleAnim_QueueEffectList3(0xD, acting_pokemon, acting_pokemon, 0xFF, 0xFF);
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 2:
            if (acting_pokemon->pokeBall.position.y == 2.5f) {
                acting_pokemon->pokeBall.node.renderFlags |= 0x20;
            }

            if (ModelAnim_IsAnimationDone(&acting_pokemon->pokeBall) == 1) {
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 3:
            acting_pokemon->pokeBall.node.flags &= ~1;
            acting_pokemon->sessionTeams->uniqueStatusFlags |= 0x80;
            acting_pokemon->sessionTeams->unk_04 = &acting_pokemon->sessionTeams->teamMember[acting_pokemon_state->ownerTeamMemberIndex]->pokemonPicked[acting_pokemon_state->actionChosen];
            sBattleScene.scene->phaseStep += 1;
            break;

        case 4:
            if (acting_pokemon->sessionTeams->uniqueStatusFlags & 0x10) {
                acting_pokemon->sessionTeams->uniqueStatusFlags &= ~0x90;
                Battle_LoadOwnerModelForSwitchIn(acting_pokemon);
                BattleThumbnail_TickOwnerCameraFraming(acting_pokemon);
                sCurrentPokemonHp[BattleScene_GetParticipantSideIndex(acting_pokemon)] = acting_pokemon->state.runtime.displayHp;
                Audio_PlayCommand(2, 0, 0);
                BattleScene_RebindParticipantTeamMember(acting_pokemon, acting_pokemon_state->ownerTeamMemberIndex);
                BattleScene_ResetCameraFrameCounter();
                BattleAnim_QueueEffectList3(3, acting_pokemon, acting_pokemon, 0xFF, 0xFF);
                camera->perspective.verticalFieldOfView = 40.0f;
                Battle_TriggerTrainerPresentationCue(acting_pokemon);
                sBattleDamage = 0;
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 5:
            BattleScene_EnterStateRow(acting_pokemon, 0x12);
            BattleScene_SelectCameraFollowPreset(acting_pokemon, camera, 3);
            sBattleScene.scene->phaseStep += 1;
            break;

        case 6:
            BattleScene_ReframeAndRecomputeDistanceForSwitch(camera, acting_pokemon);
            if (BattleScene_IsRowWaitCompleteWithEventCount(acting_pokemon, 0x3C) != 0) {
                if ((acting_pokemon_runtime->usedMove & 0xF0) == 0xF0) {
                    acting_pokemon_runtime->usedMove = STRUGGLE;
                }
                Audio_FadeAndClearStoredSounds();
                BattleAnim_CleanupEffects(2);
                BattleScene_ResetCameraFrameCounter();
                Vec3f_SetComponentsDuplicate(&acting_pokemon->displayObject.scale, 1.0f, 1.0f, 1.0f);
                if (other_pokemon_state->replacementState != 0x10) {
                    sBattleScene.scene->unk_34 = 3;
                }
                sBattleScene.scene->unk_2E = 2;
                BattleScene_CleanupActionAndAdvance();
                acting_pokemon_state->flags &= 0xE7FF;
                sBattleScene.scene->unk_54 = !sBattleScene.scene->actingSide + 1;
            }
            break;
    }
}

void BattleScene_UpdatePhase(CameraNode* camera) {
    BattleScene_UpdateActiveOwnerPointers();

    switch (sBattleScene.scene->phase) {
        case 0:
            BattleScene_InitializeBattlePresentation(camera);
            BattleScene_SetPhase(camera, 1);

        case 1:
            // battle orders finished, begin turn
            BattleScene_PhaseIntro(camera);
            break;
        case 2:
            // first pokemon move
            BattleScene_Phase2(camera);
            break;
        case 3:
            // second pokemon move
            BattleScene_Phase3(camera);
            break;
        case 4:
            // end turn
            BattleScene_Phase4(camera);
            break;
        case 7:
            // double team
            // minimize
            // rest
            // self-target moves?
            BattleScene_Phase7(camera);
            break;
        case 38:
            // withdraw
            BattleScene_Phase38(camera);
            break;
        case 27:
            // bide
            // bide 27th move by alphabetical order, but that seems to be a coincidence
            BattleScene_Phase27(camera);
            break;
        case 30:
            // explosion and self-destruct
            BattleScene_Phase30(camera);
            break;
        case 29:
            // haze
            BattleScene_Phase29(camera);
            break;
        case 28:
            // flinch
            BattleScene_Phase28(camera);
            break;
        case 25:
            BattleScene_Phase25(camera);
            break;
        case 12:
            // fly
            BattleScene_Phase12(camera);
            break;
        case 13:
            BattleScene_Phase13(camera);
            break;
        case 14:
            // 2 turn charge moves
            BattleScene_Phase14(camera);
            break;
        case 15:
            BattleScene_Phase15(camera);
            break;
        case 17:
            // sleeping or immobilized
            BattleScene_Phase17(camera);
            break;
        case 31:
            // full paralysis
            BattleScene_Phase31(camera);
            break;
        case 20:
            // causally self-damage by confusion
            BattleScene_PhaseFaint(camera);
            break;
        case 8:
            // move failed
            BattleScene_PhaseMoveFailedMessage(camera);
            break;
        case 10:
            // rage attack increase
            BattleScene_Phase10(camera);
            break;
        case 6:
            // mislabeled , case 2 and 3 already handle hit animation
            // actually poison (and probably burn) damage
            BattleScene_PhaseAttackHit(camera);
            break;
        case 33:
            BattleScene_PhaseCheckFaintedEndTurn(camera);
            break;
        case 35:
            BattleScene_PhaseAttackHitRecoil(camera);
            break;
        case 9:
            // actually being hit with recoil
            BattleScene_PhaseDrainDamage(camera);
            break;
        case 16:
            BattleScene_Phase16(camera);
            break;
        case 11:
            BattleScene_PhaseWithdrawFainted(camera);
            break;
        case 18:
            // this is SendOutAfterFaint
            BattleScene_PhaseWithdrawSwitch(camera);
            break;
        case 5:
            // this is actuallyh phase faint
            BattleScene_PhaseSendOut(camera);
            break;
        case 34:
            BattleScene_PhaseSendOutAfterFaint(camera);
            break;
        case 36:
            BattleScene_PhaseResumeSaved();
            break;
        case 37:
            // charge moves start?
            BattleScene_PhaseSelectMoveAnim();
            break;
    }
}
