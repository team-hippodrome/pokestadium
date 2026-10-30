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

#define ACTION_ANIMATION_NORMAL 0
#define ACTION_ANIMATION_MULTI_HIT 10
#define ACTION_ANIMATION_CONFUSION 11
#define ACTION_ANIMATION_DIG_RETURN 12
#define ACTION_ANIMATION_AIRBORNE 13

ActivePokemon* sNonActingPokemon;
ActivePokemon* sActingPokemon;
static f32 sFlyCameraDistance;
static s32 pad_D_84390210[4];
static s32 sSide1HpConsistent;
static s32 sSide2HpConsistent;
static s32 sNormalOrFlyingAnimationRNG;
s32 sMultihitCount;
s32 sMultihitCountdown;
s32 sMultihitEffectCountdown;

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
u16 sDeepBurrowSpeciesPair = (ONIX << 8) | MOLTRES;
Vec3f s100UnitsUpFromCenter = { 0.0f, 100.0f, 0.0f };
Vec3f s10UnitsUpFromCenter = { 0.0f, 10.0f, 0.0f };

void BattleScene_RestoreUndergroundDiglett(ActivePokemon* pokemon) {
    ActivePokemonState* state = &pokemon->state;

    if ((pokemon->displayObject.objectType == DIGLETT) && (state->flags & ACTIVE_UNDERGROUND)) {
        MovePresentationCue* move_cue = &sSideBattleCues[BattleScene_GetParticipantSideIndex(pokemon)]->moveCues[DIG - 1];

        ModelAnim_SetAnimation(&pokemon->displayObject, move_cue->id);
        pokemon->displayObject.node.renderFlags &= ~0x20;
    }
}

void BattleScene_ResetModelReloadStep(void) {
    sBattleScene.scene->modelReloadStep = 0;
}

s32 BattleScene_RevealSubstituteUser(ActivePokemon* pokemon) {
    ActivePokemonState* state = &pokemon->state;
    UNUSED s32 pad;
    s32 busy = 0;

    if (!(state->flags & ACTIVE_SUBSTITUTE)) {
        return 0;
    }

    if (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_SUBSTITUTE) {
        return 0;
    }

    if ((state->flags & ACTIVE_SUBSTITUTE) && (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_NO_ACTION)) {
        return 0;
    }

    switch (sBattleScene.scene->modelReloadStep) {
        case 0:
            BattleScene_DestroySubstitute(pokemon);
            busy = 1;
            sBattleScene.scene->modelReloadStep++;
            break;

        case 1:
            busy = 1;
            if (pokemon->sessionTeams->uniqueStatusFlags & 0x10) {
                pokemon->sessionTeams->uniqueStatusFlags &= ~0x90;
                Battle_ReloadDisplayModel(pokemon);
                BattleScene_RestoreUndergroundDiglett(pokemon);
                sBattleScene.scene->modelReloadStep++;
            }
            break;

        case 2:
            BattleScene_ResetModelReloadStep();
            break;
    }
    return busy;
}

s32 BattleScene_RestoreSubstituteDoll(ActivePokemon* pokemon) {
    UNUSED s32 pad[2];
    s32 busy = 0;

    if (!(pokemon->state.flags & ACTIVE_SUBSTITUTE)) {
        return 0;
    }

    switch (sBattleScene.scene->modelReloadStep) {
        case 0:
            Battle_MakeSubstitute(pokemon);
            busy = 1;
            sBattleScene.scene->modelReloadStep++;
            break;

        case 1:
            busy = 1;
            if (pokemon->sessionTeams->uniqueStatusFlags & 0x10) {
                pokemon->sessionTeams->uniqueStatusFlags &= ~0x90;
                Battle_ReloadSubstituteDoll(pokemon);
                sBattleScene.scene->modelReloadStep++;
            }
            break;

        case 2:
            BattleScene_ResetModelReloadStep();
            break;
    }
    return busy;
}

s32 BattleScene_BreakSubstitute(ActivePokemon* pokemon) {
    UNUSED s32 pad[2];
    Vec3f owner_anchor;
    s32 busy = 0;

    if (!(pokemon->state.flags & ACTIVE_SUB_BREAK)) {
        return 0;
    }

    switch (sBattleScene.scene->modelReloadStep) {
        case 0:
            BattleScene_DestroySubstitute(pokemon);
            busy = 1;
            sBattleScene.scene->modelReloadStep++;
            break;

        case 1:
            busy = 1;
            if (pokemon->sessionTeams->uniqueStatusFlags & 0x10) {
                pokemon->sessionTeams->uniqueStatusFlags &= ~0x90;
                Battle_ReloadDisplayModel(pokemon);
                Battle_ClearRuntimeFlags(pokemon, (ACTIVE_SUB_BREAK | ACTIVE_SUBSTITUTE));
                busy = 0;
                BattleScene_ResetModelReloadStep();
                sBattleScene.scene->modelReloadStep++;
            }
            break;

        case 2:
            BattleScene_ResetModelReloadStep();
            BattleAnim_GetOwnerDefaultAnchorPosition(pokemon, &owner_anchor);
            BattleAnim_Vec3fCopy(&sBattleScene.scene->camera->view.targetPosition, &owner_anchor);
            break;
    }
    return busy;
}

s32 BattleScene_RevealSubstituteForAction(ActivePokemon* pokemon) {
    UNUSED s32 pad[2];
    s32 busy = 0;

    if (sBattleScene.scene->unk_44 != 2) {
        return 0;
    }

    if (!(pokemon->state.flags & ACTIVE_SUBSTITUTE)) {
        return 0;
    }

    if ((sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_SUBSTITUTE) || (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_NO_ACTION)) {
        return 0;
    }

    switch (sBattleScene.scene->modelReloadStep) {
        case 0:
            BattleScene_DestroySubstitute(pokemon);
            busy = 1;
            sBattleScene.scene->modelReloadStep += 1;
            break;

        case 1:
            busy = 1;
            if (pokemon->sessionTeams->uniqueStatusFlags & 0x10) {
                pokemon->sessionTeams->uniqueStatusFlags &= ~0x90;
                Battle_ReloadDisplayModel(pokemon);
                BattleScene_RestoreUndergroundDiglett(pokemon);
                sBattleScene.scene->modelReloadStep += 1;
            }
            break;

        case 2:
            BattleScene_ResetModelReloadStep();
            break;
    }
    return busy;
}

s32 BattleScene_RestoreBothSubstitutes(void) {
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

    switch (sBattleScene.scene->modelReloadStep) {
        case 0:
            if (pokemon_state1->flags & ACTIVE_SUBSTITUTE) {
                Battle_MakeSubstitute(sActivePokemon[0]);
            }
            advanced = 1;
            sBattleScene.scene->modelReloadStep = sBattleScene.scene->modelReloadStep + 1;
            break;

        case 1:
            advanced = 1;
            if (pokemon_state1->flags & ACTIVE_SUBSTITUTE) {
                if (sActivePokemon[0]->sessionTeams->uniqueStatusFlags & 0x10) {
                    sActivePokemon[0]->sessionTeams->uniqueStatusFlags &= ~0x90;
                    Battle_ReloadSubstituteDoll(sActivePokemon[0]);
                    sBattleScene.scene->modelReloadStep += 1;
                }
            } else {
                sBattleScene.scene->modelReloadStep = sBattleScene.scene->modelReloadStep + 1;
            }
            break;

        case 2:
            if (pokemon_state2->flags & ACTIVE_SUBSTITUTE) {
                Battle_MakeSubstitute(sActivePokemon[1]);
            }
            advanced = 1;
            sBattleScene.scene->modelReloadStep = sBattleScene.scene->modelReloadStep + 1;
            break;

        case 3:
            advanced = 1;
            if (pokemon_state2->flags & ACTIVE_SUBSTITUTE) {
                if (sActivePokemon[1]->sessionTeams->uniqueStatusFlags & 0x10) {
                    sActivePokemon[1]->sessionTeams->uniqueStatusFlags &= ~0x90;
                    Battle_ReloadSubstituteDoll(sActivePokemon[1]);
                    sBattleScene.scene->modelReloadStep += 1;
                }
            } else {
                sBattleScene.scene->modelReloadStep = sBattleScene.scene->modelReloadStep + 1;
            }
            break;

        case 4:
            BattleScene_ResetModelReloadStep();
            break;
    }

    return advanced;
}

void Battle_ClearExpiredTrappingFlags(void) {
    BattleMonRuntime* pokemon_runtime1 = &sActivePokemon[0]->state.runtime;
    BattleMonRuntime* pokemon_runtime2 = &sActivePokemon[1]->state.runtime;

    if (pokemon_runtime1->isTrapped == 0) {
        pokemon_runtime1->immobilizedReason &= ~IMMOBILIZED_TRAPPING;
    }

    if (pokemon_runtime2->isTrapped == 0) {
        pokemon_runtime2->immobilizedReason &= ~IMMOBILIZED_TRAPPING;
    }
}

void BattleScene_NoOp8431FCC4(void) {
    // Empty and uncalled in this source tree; its original role is unknown.
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

void BattleScene_ResetPhaseSubstep(void) {
    sBattleScene.scene->phaseSubstep = 0;
}

void BattleScene_BeginPhaseSubstep(void) {
    sBattleScene.scene->phaseSubstep = 1;
}

void BattleScene_SetPhase(UNUSED CameraNode* camera, s32 phase) {
    sBattleScene.scene->phase = phase;
    sBattleScene.scene->phaseStep = 0;
}

void BattleScene_SaveCameraFov(CameraNode* camera) {
    sBattleScene.scene->verticalFieldOfView = camera->perspective.verticalFieldOfView;
}

void BattleScene_ResetCameraFov(CameraNode* camera) {
    camera->perspective.verticalFieldOfView = 30.0f;
    sBattleScene.scene->verticalFieldOfView = 30.0f;
}

s32 BattleScene_GetInitialCameraCue(ActivePokemon* pokemon) {
    MovePresentationCue* move_cue = &sSideBattleCues[BattleScene_GetParticipantSideIndex(pokemon)]->moveCues[pokemon->state.runtime.usedMove - 1];

    return move_cue->initialCameraCue;
}

s32 BattleScene_GetChargeInitialCameraCue(ActivePokemon* pokemon) {
    UNUSED s32 pad;
    s32 charge_move_index = sChargeMoveCues[pokemon->state.runtime.usedMove - 1];
    MovePresentationCue* move_cue = &sSideBattleCues[BattleScene_GetParticipantSideIndex(pokemon)]->moveCues[charge_move_index];

    return move_cue->initialCameraCue;
}

s32 BattleScene_GetFollowupCameraCue(ActivePokemon* pokemon) {
    MovePresentationCue* move_cue = &sSideBattleCues[BattleScene_GetParticipantSideIndex(pokemon)]->moveCues[pokemon->state.runtime.usedMove - 1];

    return move_cue->followupCameraCue;
}

s32 BattleScene_GetChargeFollowupCameraCue(ActivePokemon* pokemon) {
    UNUSED s32 pad;
    s32 charge_move_index = sChargeMoveCues[pokemon->state.runtime.usedMove - 1];
    MovePresentationCue* move_cue = &sSideBattleCues[BattleScene_GetParticipantSideIndex(pokemon)]->moveCues[charge_move_index];

    return move_cue->followupCameraCue;
}

s32 BattleScene_GetCameraShotDuration(ActivePokemon* pokemon) {
    MovePresentationCue* move_cue = &sSideBattleCues[BattleScene_GetParticipantSideIndex(pokemon)]->moveCues[pokemon->state.runtime.usedMove - 1];

    if (move_cue->cameraHoldFrames == 0) {
        return 0xF;
    }
    return move_cue->cameraHoldFrames;
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
    sBattleScene.scene->phase = BATTLE_PHASE_INITIALIZE;
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

    if ((state1->replacementState == REPLACEMENT_VOLUNTARY_SWITCH) || (state1->replacementState == REPLACEMENT_INVOLUNTARY_SWITCH)) {
        if (sActivePokemon[0]->teamRoster->isHuman & 2) {
            BattleAI_RecordUsedMove(state1->rosterIndex, state2->rosterIndex, 2);
        }
        sBattleScene.scene->actingSide = 0;
        sBattleScene.scene->sideCurrentlySwitching = 0;
        sBattleScene.scene->phase = BATTLE_PHASE_INITIALIZE;
        sBattleScene.scene->unk_44 = 2;
        BeginBattleForEntity(sActivePokemon[0]);
        sBattleScene.scene->actingSide = side;
    } else {
        if (sActivePokemon[0]->teamRoster->isHuman & 2) {
            BattleAI_RecordUsedMove(state2->rosterIndex, state1->rosterIndex, 2);
        }
        sBattleScene.scene->actingSide = 1;
        sBattleScene.scene->sideCurrentlySwitching = 1;
        sBattleScene.scene->phase = BATTLE_PHASE_INITIALIZE;
        sBattleScene.scene->unk_44 = 2;
        BeginBattleForEntity(sActivePokemon[1]);
        sBattleScene.scene->actingSide = side;
    }
}

s32 BattleScene_PrepareReplacementTransition(void) {
    ActivePokemonState* pokemon1_state = &sActivePokemon[sBattleScene.scene->actingSide]->state;
    ActivePokemonState* acting_state_alias = &sActivePokemon[sBattleScene.scene->actingSide]->state;
    ActivePokemonState* pokemon2_state = &sActivePokemon[!sBattleScene.scene->actingSide]->state;

    if ((sActivePokemon[sBattleScene.scene->actingSide]->state.replacementState == REPLACEMENT_VOLUNTARY_SWITCH) || (pokemon1_state->replacementState == REPLACEMENT_INVOLUNTARY_SWITCH)) {
        if (sActivePokemon[sBattleScene.scene->actingSide]->teamRoster->isHuman & 2) {
            BattleAI_RecordUsedMove(acting_state_alias->rosterIndex, pokemon2_state->rosterIndex, 2);
        }
        Battle_ClearRuntimeFlags(sActivePokemon[sBattleScene.scene->actingSide], ACTIVE_SUBSTITUTE | ACTIVE_SUB_BREAK);
        sBattleScene.scene->phaseStep = 0;
        sBattleScene.scene->stepFrameTimer = 0;
        if (pokemon1_state->replacementState == REPLACEMENT_INVOLUNTARY_SWITCH) {
            sBattleScene.scene->phase = BATTLE_PHASE_SEND_OUT_AFTER_FAINT;
        } else {
            BattleAnim_GetOwnerDefaultAnchorPosition(sActivePokemon[sBattleScene.scene->actingSide], &gCameraPreset.startingPosition);
            BattleScene_LoadCameraPreset(&sSwitchCameraPreset, sBattleScene.scene->camera, sActivePokemon[sBattleScene.scene->actingSide]);
            sBattleScene.scene->phase = BATTLE_PHASE_VOLUNTARY_SWITCH;
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

    Battle_ClearExpiredTrappingFlags();
    state1->selectedCommandOption = 0;
    state2->selectedCommandOption = 0;

    if ((state1->replacementState == REPLACEMENT_INVOLUNTARY_SWITCH) || (state2->replacementState == REPLACEMENT_INVOLUNTARY_SWITCH) || (sBattleScene.scene->unk_2E == 2)) {
        BattleScene_SetCameraSubstate(1, 0);
    } else {
        BattleScene_SetCameraSubstate(1, 1);
    }

    sBattleScene.scene->unk_44 = 1;
    sBattleScene.scene->phaseStep += 1;
}

s32 BattleScene_TransitionAndClearMoveEffect(s32 phase) {
    BattleScene_ResetPhaseSubstep();
    BattleScene_ResetCameraFrameCounter();
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_NORMAL;
    sBattleScene.scene->rageFollowupPhase = 0;
    sBattleScene.scene->phaseStep = 0;
    sBattleScene.scene->stepFrameTimer = 0;
    sBattleScene.scene->phase = phase;
    return 1;
}

s32 BattleScene_TransitionPreservingMoveEffect(s32 phase) {
    BattleScene_ResetPhaseSubstep();
    BattleScene_ResetCameraFrameCounter();
    sBattleScene.scene->rageFollowupPhase = 0;
    sBattleScene.scene->phaseStep = 0;
    sBattleScene.scene->stepFrameTimer = 0;
    sBattleScene.scene->phase = phase;
    return 1;
}

s32 BattleScene_TransitionToRage(s32 phase) {
    BattleScene_ResetPhaseSubstep();
    BattleScene_ResetCameraFrameCounter();
    sBattleScene.scene->rageFollowupPhase = 0;
    sBattleScene.scene->phaseStep = 0;
    sBattleScene.scene->stepFrameTimer = 0;
    sBattleScene.scene->phase = phase;
    return 1;
}

void BattleScene_SwitchActiveSideAndSetPhase(CameraNode* camera) {
    sBattleScene.scene->actingSide = !sBattleScene.scene->actingSide;
    BattleScene_SetPhase(camera, BATTLE_PHASE_FAINT);
}

void Battle_ActSecondAttacker(u8 next_phase) {
    ActivePokemonState* pokemon_state;

    sBattleScene.scene->actingSide = !sBattleScene.scene->actingSide;
    pokemon_state = &sActivePokemon[sBattleScene.scene->actingSide]->state;

    if ((pokemon_state->replacementState == REPLACEMENT_VOLUNTARY_SWITCH) || (pokemon_state->replacementState == REPLACEMENT_INVOLUNTARY_SWITCH)) {
        BeginBattleForEntity(sActivePokemon[sBattleScene.scene->actingSide]);
    } else {
        Battle_ResolveMonAction();
    }

    sBattleScene.scene->actionPass = BATTLE_ACTION_SECOND;
    sBattleScene.scene->unk_A0.z = 0.0f;
    BattleScene_SetPhase(sBattleScene.scene->camera, BATTLE_PHASE_ENTER_QUEUED);
    sBattleScene.scene->nextPhase = next_phase;
}

/**
 * Phase 36. Waits for the acting Pokemon's animation to finish, then jumps to the phase that
 * Battle_ActSecondAttacker stored in nextPhase.
 */
void BattleScene_PhaseEnterQueuedPhase(void) {
    if (BattleScene_RevealSubstituteUser(sActivePokemon[sBattleScene.scene->actingSide]) == 0) {
        sBattleScene.scene->phase = sBattleScene.scene->nextPhase;
    }
}

void BattleScene_CleanupActionAndAdvance(void) {
    BattleMonRuntime* acting_pokemon_state;
    BattleMonRuntime* other_pokemon_state;
    ActivePokemonState* acting_pokemon_runtime;

    if (sBattleScene.scene->actionPass == BATTLE_ACTION_FIRST) {
        BattleAnim_ClearRockThrowingMoveFlag(sActivePokemon[sBattleScene.scene->actingSide]);
        Audio_FadeAndClearStoredSounds();
        BattleScene_ResetModelReloadStep();

        acting_pokemon_state = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;
        other_pokemon_state = &sActivePokemon[!sBattleScene.scene->actingSide]->state.runtime;
        acting_pokemon_runtime = &sActivePokemon[!sBattleScene.scene->actingSide]->state;

        if ((((acting_pokemon_state->usedMove & 0xF0) == 0xF0) || ((other_pokemon_state->usedMove & 0xF0) == 0xF0)) && (acting_pokemon_runtime->replacementState != REPLACEMENT_VOLUNTARY_SWITCH) &&
            (acting_pokemon_runtime->replacementState != REPLACEMENT_INVOLUNTARY_SWITCH)) {
            sBattleScene.scene->phaseStep = 0;
            sBattleScene.scene->stepFrameTimer = 0;
            sBattleScene.scene->phase = BATTLE_PHASE_END_TURN;

            if ((acting_pokemon_state->usedMove & 0xF0) == 0xF0) {
                acting_pokemon_state->usedMove = STRUGGLE;
            }

            if ((other_pokemon_state->usedMove & 0xF0) == 0xF0) {
                other_pokemon_state->usedMove = STRUGGLE;
            }
        } else {
            Battle_ActSecondAttacker(BATTLE_PHASE_SECOND_ACTION);
            sBattleScene.scene->phaseStep = 0;
            sBattleScene.scene->stepFrameTimer = 0;
        }
    } else {
        acting_pokemon_state = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;
        other_pokemon_state = &sActivePokemon[!sBattleScene.scene->actingSide]->state.runtime;

        BattleAnim_ClearRockThrowingMoveFlag(sActivePokemon[sBattleScene.scene->actingSide]);
        Audio_FadeAndClearStoredSounds();
        BattleScene_ResetModelReloadStep();

        sBattleScene.scene->phaseStep = 0;
        sBattleScene.scene->stepFrameTimer = 0;
        sBattleScene.scene->phase = BATTLE_PHASE_END_TURN;

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
        BattleScene_ResetModelReloadStep();
        Battle_ActSecondAttacker(BATTLE_PHASE_SECOND_ACTION);
        sBattleScene.scene->phaseStep = 0;
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_NORMAL;
        sBattleScene.scene->stepFrameTimer = 0;
        return 1;
    }
    return 0;
}

void BattleScene_CompleteActionPhase(void) {
    if ((sBattleScene.scene->actionPass == BATTLE_ACTION_FIRST) && ((sNonActingPokemon->state.runtime.usedMove & 0xF0) != 0xF0)) {
        BattleAnim_ClearRockThrowingMoveFlag(sActivePokemon[sBattleScene.scene->actingSide]);
        Audio_FadeAndClearStoredSounds();
        BattleScene_ResetModelReloadStep();
        Battle_ActSecondAttacker(BATTLE_PHASE_SECOND_ACTION);
        sBattleScene.scene->phaseStep = 0;
        sBattleScene.scene->stepFrameTimer = 0;
    } else if (BattleScene_HandleFaintStatusCompletion() == 0) {
        BattleAnim_ClearRockThrowingMoveFlag(sActivePokemon[sBattleScene.scene->actingSide]);
        Audio_FadeAndClearStoredSounds();
        BattleScene_ResetModelReloadStep();
        sBattleScene.scene->phaseStep = 0;
        sBattleScene.scene->stepFrameTimer = 0;
        sBattleScene.scene->phase = BATTLE_PHASE_END_TURN;
    }
}

void BattleScene_ResolveDisplayedHpCompletion(CameraNode* camera) {
    if ((sSide1HpConsistent == 1) && (sSide2HpConsistent == 1)) {
        if (Battle_GetActiveMonHp(sActivePokemon[sBattleScene.scene->actingSide]) == 0) {
            BattleScene_SwitchActiveSideAndSetPhase(camera);
        } else if (Battle_GetActiveMonHp(sActivePokemon[!sBattleScene.scene->actingSide]) == 0) {
            BattleScene_SetPhase(camera, BATTLE_PHASE_FAINT);
        } else {
            BattleScene_CompleteActionPhase();
        }
    }
}

void BattleScene_UpdateDisplayedHpAndTransition(CameraNode* camera) {
    BattleMonRuntime* acting_pokemon_runtime = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;

    sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[0]);
    sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[1]);
    BattleScene_ResetPhaseSubstep();
    BattleScene_ResetCameraFrameCounter();

    if (Battle_GetActiveMonHp(sActivePokemon[sBattleScene.scene->actingSide]) == 0) {
        BattleScene_SwitchActiveSideAndSetPhase(camera);
    } else if (Battle_GetActiveMonHp(sActivePokemon[!sBattleScene.scene->actingSide]) == 0) {
        BattleScene_ResolveDisplayedHpCompletion(camera);
    } else if (sBattleScene.scene->rageFollowupPhase == BATTLE_PHASE_RAGE) {
        BattleScene_TransitionToRage(BATTLE_PHASE_RAGE);
    } else if (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_RECOIL) {
        BattleScene_TransitionAndClearMoveEffect(BATTLE_PHASE_RECOIL);
    } else if ((acting_pokemon_runtime->unk_15 & 0x18) || (acting_pokemon_runtime->unk_4D & 0x80)) {
        BattleScene_TransitionAndClearMoveEffect(BATTLE_PHASE_RESIDUAL_AFTER_MOVE);
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
        case MOVE_PRESENTATION_SELF_TARGET:
        case MOVE_PRESENTATION_WITHDRAW:
        case MOVE_PRESENTATION_TELEPORT:
            BattleScene_EnterStateRow(sActingPokemon, 3);
            return BattleScene_TransitionPreservingMoveEffect(BATTLE_PHASE_DISPATCH_MOVE_EFFECT);

        case MOVE_PRESENTATION_EXPLOSION:
            BattleScene_EnterStateRow(sActingPokemon, 3);
            return BattleScene_TransitionPreservingMoveEffect(BATTLE_PHASE_DISPATCH_MOVE_EFFECT);

        case MOVE_PRESENTATION_NO_ACTION:
            return BattleScene_TransitionPreservingMoveEffect(BATTLE_PHASE_DISPATCH_MOVE_EFFECT);

        case MOVE_PRESENTATION_HEAL:
            BattleScene_EnterStateRow(sActingPokemon, 3);
            return BattleScene_TransitionPreservingMoveEffect(BATTLE_PHASE_DISPATCH_MOVE_EFFECT);

        case MOVE_PRESENTATION_TRANSFORM:
            return BattleScene_TransitionPreservingMoveEffect(BATTLE_PHASE_DISPATCH_MOVE_EFFECT);

        case MOVE_PRESENTATION_SUBSTITUTE:
            return BattleScene_TransitionPreservingMoveEffect(BATTLE_PHASE_DISPATCH_MOVE_EFFECT);

        case MOVE_PRESENTATION_BIDE:
            return BattleScene_TransitionPreservingMoveEffect(BATTLE_PHASE_DISPATCH_MOVE_EFFECT);

        case MOVE_PRESENTATION_HAZE:
            BattleScene_EnterStateRow(sActingPokemon, 3);
            return BattleScene_TransitionPreservingMoveEffect(BATTLE_PHASE_DISPATCH_MOVE_EFFECT);

        case MOVE_PRESENTATION_FLY_START:
            BattleScene_EnterStateRow(sActingPokemon, 8);
            return BattleScene_TransitionPreservingMoveEffect(BATTLE_PHASE_DISPATCH_MOVE_EFFECT);

        case MOVE_PRESENTATION_DIG_START:
            BattleScene_EnterStateRow(sActingPokemon, 8);
            return BattleScene_TransitionPreservingMoveEffect(BATTLE_PHASE_DISPATCH_MOVE_EFFECT);

        case MOVE_PRESENTATION_CHARGE:
            BattleScene_EnterStateRow(sActingPokemon, 8);
            return BattleScene_TransitionPreservingMoveEffect(BATTLE_PHASE_DISPATCH_MOVE_EFFECT);

        case MOVE_PRESENTATION_UNABLE_TO_ACT:
            BattleScene_EnterStateRow(sActingPokemon, 9);
            return BattleScene_TransitionPreservingMoveEffect(BATTLE_PHASE_DISPATCH_MOVE_EFFECT);

        case MOVE_PRESENTATION_FULL_PARALYSIS:
            return BattleScene_TransitionPreservingMoveEffect(BATTLE_PHASE_DISPATCH_MOVE_EFFECT);
    }
    return 0;
}

s32 BattleScene_ActionPresentationCleanupCheck(void) {
    s32 move_effect = sBattleScene.scene->moveEffectKind;

    if (move_effect != MOVE_PRESENTATION_JUMP_KICK_CRASH) {
        if (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_CONFUSION_SELF_HIT) {
            return BattleScene_TransitionAndClearMoveEffect(BATTLE_PHASE_CONFUSION_SELF_HIT);
        }
    } else {
        return BattleScene_TransitionAndClearMoveEffect(BATTLE_PHASE_JUMP_KICK_CRASH);
    }

    if ((sBattleMoveFailed != 0) || (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_FAILED)) {
        return BattleScene_TransitionAndClearMoveEffect(BATTLE_PHASE_MOVE_FAILED);
    }
    return 0;
}

s32 BattleScene_PrepareResidualStatusPresentation(void) {
    s32 idx = sBattleScene.scene->actingSide;
    BattleMonRuntime* pokemon_runtime = &sActivePokemon[idx]->state.runtime;

    if (((pokemon_runtime->unk_4D & 0x80) || (pokemon_runtime->unk_15 & 0x18)) && ((pokemon_runtime->usedMove & 0xF0) == 0xF0)) {
        return BattleScene_TransitionAndClearMoveEffect(BATTLE_PHASE_RESIDUAL_WITHOUT_MOVE);
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
    BattleScene_ResetModelReloadStep();
    sBattleScene.scene->actionPass = BATTLE_ACTION_FIRST;
    BattleScene_ResetPhaseSubstep();
    sBattleScene.scene->stepFrameTimer = 0;
    Battle_ResetModelToIdleAnim(sActivePokemon[0]);
    Battle_ResetModelToIdleAnim(sActivePokemon[1]);
    sBattleScene.scene->phaseStep++;
}

void BattleScene_PrepareWinnerPresentation(CameraNode* camera) {
    ActivePokemonState* other_pokemon_state = &sActivePokemon[!sBattleScene.scene->actingSide]->state;
    ActivePokemonState* acting_pokemon_state = &sActivePokemon[sBattleScene.scene->actingSide]->state;
    s16 acting_pokemon_species = sActivePokemon[sBattleScene.scene->actingSide]->displayObject.objectType;
    MovePresentationCue* acting_pokemon_idle_cue = &sSideBattleCues[sBattleScene.scene->actingSide]->idleCues;
    ActivePokemon* acting_pokemon = sActivePokemon[sBattleScene.scene->actingSide];

    if (((other_pokemon_state->replacementState == REPLACEMENT_DEFEAT_COMPLETE) && (acting_pokemon_state->replacementState == REPLACEMENT_DEFEAT_COMPLETE)) ||
        ((other_pokemon_state->replacementState == REPLACEMENT_INVOLUNTARY_SWITCH) && (acting_pokemon_state->replacementState == REPLACEMENT_DEFEAT_COMPLETE)) ||
        ((acting_pokemon_state->replacementState == REPLACEMENT_INVOLUNTARY_SWITCH) && (other_pokemon_state->replacementState == REPLACEMENT_DEFEAT_COMPLETE))) {
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

void BattleScene_BeginFaintPresentation(CameraNode* camera) {
    if (BattleScene_RevealSubstituteUser(sActivePokemon[!sBattleScene.scene->actingSide]) == 0) {
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

void BattleScene_UpdateFaintAnimation(CameraNode* camera) {
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

void BattleScene_AdvanceFaintCameraClose(CameraNode* camera) {
    BattleScene_ApproachCameraTarget(camera, sActivePokemon[!sBattleScene.scene->actingSide], 1);
    if (BattleScene_AdvanceCameraFrameCounter(0x19) != 0) {
        Audio_FadeAndClearStoredSounds();
        sBattleScene.scene->phaseStep++;
    }
}

void BattleScene_FinishFaintCameraClose(CameraNode* camera) {
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

void BattleScene_CompleteFaintPresentation(CameraNode* camera) {
    ActivePokemonState* other_pokemon_state = &sActivePokemon[!sBattleScene.scene->actingSide]->state;
    ActivePokemonState* acting_pokemon_state = &sActivePokemon[sBattleScene.scene->actingSide]->state;

    if (BattleScene_AdvanceCameraFrameCounter(0xA) != 0) {
        BattleScene_ResetCameraFrameCounter();
        BattleScene_ResetPhaseSubstep();
        sBattleScene.scene->phaseStep++;
        if (other_pokemon_state->replacementState == REPLACEMENT_INVOLUNTARY_SWITCH) {
            sBattleScene.scene->stepFrameTimer = 0;
            sBattleScene.scene->phaseStep = 0;

            if ((acting_pokemon_state->replacementState == REPLACEMENT_FAINT_PENDING) || (acting_pokemon_state->replacementState == REPLACEMENT_DEFEAT_PENDING)) {
                sBattleScene.scene->phase = BATTLE_PHASE_FAINT;
                sBattleScene.scene->actingSide = !sBattleScene.scene->actingSide;
            } else if ((other_pokemon_state->replacementState == REPLACEMENT_DEFEAT_COMPLETE) || (acting_pokemon_state->replacementState == REPLACEMENT_DEFEAT_COMPLETE)) {
                BattleBox_Open(0xE);
                Vec3f_SetComponentsDuplicate(&camera->view.targetPosition, 0.0f, 75.0f, 0.0f);
                Vec3f_SetComponentsDuplicate(&camera->view.eyePosition, 0.0f, 80.0f, 400.0f);
                Audio_StartBattleMusicSequence();
                sBattleScene.scene->actingSide = !sBattleScene.scene->actingSide;
                sBattleScene.scene->phaseStep = 8;
            } else {
                sBattleScene.scene->phase = BATTLE_PHASE_END_TURN;
            }
        } else if ((acting_pokemon_state->replacementState == REPLACEMENT_DEFEAT_PENDING) || (acting_pokemon_state->replacementState == REPLACEMENT_FAINT_PENDING)) {
            sBattleScene.scene->stepFrameTimer = 0;
            sBattleScene.scene->phaseStep = 0;
            sBattleScene.scene->phase = BATTLE_PHASE_FAINT;
            sBattleScene.scene->actingSide = !sBattleScene.scene->actingSide;
        } else {
            BattleScene_PrepareWinnerPresentation(camera);
        }
        Audio_FadeAndClearStoredSounds();
    }
}

void BattleScene_FaintStepNoOp(UNUSED CameraNode* camera) {
    // Retained faint-phase step; the function body has no effect.
}

void BattleScene_SetupWinnerCamera(CameraNode* camera) {
    ActivePokemon* pokemon;
    UNUSED s32 pad;
    f32 target_height;
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

        target_height = (sSideCameraTransform[sBattleScene.scene->actingSide]->coordinates.y + pokemon->displayObject.position.y) + sModelCameraAdjustments[camera_adjustment_index].z;
        eye_distance = ((sSideCameraTransform[sBattleScene.scene->actingSide]->coordinates.z + sModelCameraAdjustments[camera_adjustment_index].y) * pokemon->facingDirection) + pokemon->displayObject.position.x;

        Vec3f_SetComponentsDuplicate(&camera->view.targetPosition, eye_distance, target_height, pokemon->displayObject.position.z);

        eye_distance = sSideCameraTransform[sBattleScene.scene->actingSide]->coordinates.x * sModelCameraAdjustments[camera_adjustment_index].x;
        eye_rotation_x = sSideCameraTransform[sBattleScene.scene->actingSide]->rotationX;
        eye_rotation_y = (pokemon->facingDirection * sSideCameraTransform[sBattleScene.scene->actingSide]->rotationY) + BattleScene_GetParticipantFacingAngle(pokemon);
        Camera_ComputeEyeFromAngles(&camera->view.targetPosition, &camera->view.eyePosition, eye_distance, eye_rotation_x, eye_rotation_y);
        sBattleScene.scene->phaseStep++;
    }
}

void BattleScene_CompleteWinnerCameraFade(UNUSED CameraNode* camera) {
    if (BattleScene_AdvanceCameraFrameCounter(0x50) != 0) {
        sBattleScene.scene->winner = sBattleScene.scene->actingSide + 1;
        BattleBox_Close();
        Audio_FadeOutAll(0x14);
    }
}

void BattleScene_CompleteWinnerExtendedFade(UNUSED CameraNode* camera) {
    if (BattleScene_AdvanceCameraFrameCounter(0x7D) != 0) {
        sBattleScene.scene->winner = sBattleScene.scene->actingSide + 1;
        Audio_FadeOutAll(0x14);
    }
}

void BattleScene_UpdateAlternateFaintAnimation(CameraNode* camera) {
    UNUSED s32 pad[2];
    ActivePokemonState* acting_pokemon_state = &sActivePokemon[!sBattleScene.scene->actingSide]->state;
    MovePresentationCue* move_cue = &sSideBattleCues[!sBattleScene.scene->actingSide]->unk_B10;

    if (BattleAnim_IsAssetLoadBusy(sActivePokemon[!sBattleScene.scene->actingSide]) == 0) {
        if (sBattleScene.scene->stepFrameTimer == 0x1D) {
            BattleScene_SelectCameraShot(sActivePokemon[!sBattleScene.scene->actingSide], camera, 4);
        }

        if (BattleScene_AdvanceCameraFrameCounterUntil(0x1E) != 0) {
            BattleScene_ApproachCameraZoom(camera, sActivePokemon[!sBattleScene.scene->actingSide]);
        }

        if (((move_cue->animationDuration >> 1) == (sActivePokemon[!sBattleScene.scene->actingSide]->displayObject.transformAnim.frameFixed >> 0x10)) &&
            (acting_pokemon_state->replacementState != REPLACEMENT_DEFEAT_PENDING)) {
            BattleBox_Open(9);
        }

        if (move_cue->frameCounter == ((sActivePokemon[!sBattleScene.scene->actingSide]->displayObject.transformAnim.frameFixed >> 0x10) + 3)) {
            sActivePokemon[!sBattleScene.scene->actingSide]->displayObject.node.renderFlags &= ~0x20;
            Battle_SetRuntimeFlags(sActivePokemon[!sBattleScene.scene->actingSide], 0x100);
            BattleScene_ResetCameraFrameCounter();
            BattleScene_UpdateCameraModelOffset(sActivePokemon[!sBattleScene.scene->actingSide], camera);
            sBattleScene.scene->phaseStep = 2;
        }

        BattleScene_ApproachCameraTarget(camera, sActivePokemon[!sBattleScene.scene->actingSide], 0);
    }
}

/**
 * Phase 5. Faint presentation for the Pokemon on the non-acting side: waits for its animation, plays the
 * faint cue, closes in with the camera, then continues to the next action, the end of the turn, or (steps 6-8)
 * the winner camera shots that set scene->winner. Entered when a displayed HP reaches 0, e.g. from
 * BattleScene_SwitchActiveSideAndSetPhase and BattleScene_ResolveDisplayedHpCompletion.
 */
void BattleScene_PhaseFaint(CameraNode* camera) {
    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_BeginFaintPresentation(camera);
            break;

        case 1:
            BattleScene_UpdateFaintAnimation(camera);
            break;

        case 2:
            BattleScene_AdvanceFaintCameraClose(camera);
            break;

        case 3:
            BattleScene_FinishFaintCameraClose(camera);
            break;

        case 4:
            BattleScene_CompleteFaintPresentation(camera);
            break;

        case 5:
            BattleScene_FaintStepNoOp(camera);
            break;

        case 6:
            BattleScene_SetupWinnerCamera(camera);
            break;

        case 7:
            BattleScene_CompleteWinnerCameraFade(camera);
            break;

        case 8:
            BattleScene_CompleteWinnerExtendedFade(camera);
            break;

        case 10:
            BattleScene_UpdateAlternateFaintAnimation(camera);
            break;
    }
}

void BattleScene_CompleteResidualFaintPresentation(UNUSED CameraNode* camera) {
    ActivePokemonState* fainted_state = &sActivePokemon[!sBattleScene.scene->actingSide]->state;

    if (BattleScene_AdvanceCameraFrameCounter(2) != 0) {
        BattleScene_ResetCameraFrameCounter();
        BattleScene_ResetPhaseSubstep();
        sBattleScene.scene->phaseStep += 1;
        if (fainted_state->replacementState == REPLACEMENT_INVOLUNTARY_SWITCH) {
            sBattleScene.scene->stepFrameTimer = 0;
            sBattleScene.scene->phaseStep = 0;
            sBattleScene.scene->phase = BATTLE_PHASE_END_TURN;
        } else {
            sBattleScene.scene->winner = sBattleScene.scene->actingSide + 1;
        }
        Audio_FadeAndClearStoredSounds();
    }
}

/**
 * Phase 34. Same faint sequence as BattleScene_PhaseFaint, entered from BattleScene_PhaseResidualDamageFaintCheck.
 * After the faint it goes to BattleScene_PhaseEndTurn when the fainted side's replacementState is
 * REPLACEMENT_INVOLUNTARY_SWITCH; otherwise it sets scene->winner.
 */
void BattleScene_PhaseFaintAfterResidualDamage(CameraNode* camera) {
    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_BeginFaintPresentation(camera);
            break;

        case 1:
            BattleScene_UpdateFaintAnimation(camera);
            break;

        case 2:
            BattleScene_AdvanceFaintCameraClose(camera);
            break;

        case 3:
            BattleScene_FinishFaintCameraClose(camera);
            break;

        case 4:
            BattleScene_CompleteResidualFaintPresentation(camera);
            break;

        case 5:
            BattleScene_FaintStepNoOp(camera);
            break;

        case 10:
            BattleScene_UpdateAlternateFaintAnimation(camera);
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
    s32 rng;
    ActivePokemonState* pokemon1_state = &sActivePokemon[0]->state;
    ActivePokemonState* pokemon2_state = &sActivePokemon[1]->state;

    BattleScene_ClearStateTickCallbacks();
    BattleScene_ResetModelReloadStep();

    rng = BattleAnim_RandomRange(5);
    if (rng >= 5) {
        rng = 0;
    }

    if ((pokemon1_state->flags & ACTIVE_UNDERGROUND) || ((pokemon2_state->flags & ACTIVE_UNDERGROUND) != 0)) {
        rng = 2;
    } else if ((pokemon1_state->flags & ACTIVE_AIRBORNE) || (pokemon2_state->flags & ACTIVE_AIRBORNE)) {
        rng = 4;
    } else if ((pokemon1_state->replacementState == REPLACEMENT_INVOLUNTARY_SWITCH) || (pokemon2_state->replacementState == REPLACEMENT_INVOLUNTARY_SWITCH)) {
        rng = 2;
    }

    BattleAnim_Vec3fCopy(&gCameraPreset.startingPosition, &s100UnitsUpFromCenter);
    BattleScene_LoadCameraPresetUnscaled(&D_84384B2C[rng], camera);
    BattleScene_ApplyCameraPreset(camera, sActingPokemon, 0);
    sBattleScene.scene->phaseStep++;
}

void BattleScene_AdvanceIntroCamera(CameraNode* camera) {
    if (BattleScene_AnimateCameraPreset(camera, sActingPokemon, 0, 1) != 0) {
        BattleScene_SetPhase(camera, BATTLE_PHASE_FIRST_ACTION);
    }
}

/**
 * Phase 1. Camera intro at the start of a turn: picks one of five intro presets (fixed choices when a
 * Pokemon is underground, airborne, or being replaced after a faint), plays it, then enters phase 2.
 */
void BattleScene_PhaseTurnIntroCamera(CameraNode* camera) {
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
    s32 camera_cue;

    if (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_CHARGE) {
        camera_cue = BattleScene_GetChargeInitialCameraCue(sActivePokemon[sBattleScene.scene->actingSide]);
    } else if ((sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_NO_ACTION) || (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_UNABLE_TO_ACT) ||
               (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_FULL_PARALYSIS)) {
        camera_cue = 0;
    } else {
        camera_cue = BattleScene_GetInitialCameraCue(sActivePokemon[sBattleScene.scene->actingSide]);
    }

    if ((active_pokemon_runtime->usedMove == ICE_BEAM) || (active_pokemon_runtime->usedMove == SKY_ATTACK)) {
        if ((sBattleScene.scene->moveEffectKind != MOVE_PRESENTATION_NO_ACTION) && (sBattleScene.scene->moveEffectKind != MOVE_PRESENTATION_UNABLE_TO_ACT) &&
            (sBattleScene.scene->moveEffectKind != MOVE_PRESENTATION_FULL_PARALYSIS)) {
            camera_cue = 0x16;
        }
    }

    switch (camera_cue) {
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
            BattleScene_SelectCameraPreset(sActingPokemon, camera, sBattleScene.scene->cameraPresetId = camera_cue, 1);
            if (active_pokemon_state->flags & ACTIVE_UNDERGROUND) {
                camera->view.targetPosition.y = 0.0f;
            }
            break;
    }
}

s32 BattleScene_AdvanceActionPresentationSetup(CameraNode* camera) {
    UNUSED s32 pad;
    ActivePokemonState* active_pokemon_state = &sActivePokemon[sBattleScene.scene->actingSide]->state;

    BattleScene_ResetCameraFrameCounter();
    BattleScene_SelectActionCameraPreset(camera);

    if (BattleScene_ActionPresentationTransitionCheck() != 0) {
        return 0;
    }

    if (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_CONFUSION_SELF_HIT) {
        BattleScene_EnterStateRow(sActivePokemon[sBattleScene.scene->actingSide], 0x15);
    } else if (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_MULTI_HIT) {
        BattleScene_EnterStateRow(sActivePokemon[sBattleScene.scene->actingSide], 3);
    } else if (active_pokemon_state->flags & (ACTIVE_AIR_ACTION | ACTIVE_UNDERGROUND)) {
        BattleScene_EnterStateRow(sActivePokemon[sBattleScene.scene->actingSide], 3);
        BattleScene_SelectCameraPreset(sActivePokemon[sBattleScene.scene->actingSide], camera, sBattleScene.scene->cameraPresetId = 2, 1);
    } else {
        BattleScene_EnterStateRow(sActivePokemon[sBattleScene.scene->actingSide], 3);
    }

    BattleScene_ResetPhaseSubstep();
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
                BattleScene_BeginPhaseSubstep();

                if ((sBattleScene.scene->actionPass == BATTLE_ACTION_SECOND) && (sNonActingPokemon->unk_4C8 == 1)) {
                    BattleScene_ResetOwnerPresentationFlags(sNonActingPokemon);
                }

                BattleAnim_GetOwnerDefaultAnchorPosition(sActingPokemon, &gCameraPreset.startingPosition);

                if (pokemon_state->flags & ACTIVE_UNDERGROUND) {
                    gCameraPreset.startingPosition.y = 15.000001f;
                }

                if (sBattleScene.scene->actionPass == BATTLE_ACTION_SECOND) {
                    BattleScene_ResetCameraFov(camera);

                    if ((sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_NO_ACTION) || (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_UNABLE_TO_ACT) ||
                        (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_FULL_PARALYSIS)) {
                        BattleScene_SelectCameraFollowPreset(sActingPokemon, camera, 3);
                    } else {
                        BattleScene_SelectCameraFollowPreset(sActingPokemon, camera, 4);
                    }

                    if (pokemon_state->flags & ACTIVE_UNDERGROUND) {
                        camera->view.targetPosition.y = 15.000001f;
                    }
                    Battle_ResetModelToIdleAnim(sActingPokemon);
                } else if ((sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_NO_ACTION) || (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_UNABLE_TO_ACT) ||
                           (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_FULL_PARALYSIS)) {
                    BattleScene_LoadCameraPreset(&D_8438498C, camera, sActingPokemon);
                } else {
                    BattleScene_LoadCameraPreset(&sSwitchCameraPreset, camera, sActingPokemon);
                }
            }
            break;

        case 1:
            if (BattleAnim_TryRegisterAssetTables(sActingPokemon) != 0) {
                sMultihitEffectCountdown = sMultihitCountdown = sMultihitCount;
                BattleBox_Open(0);
                BattleScene_ResetCameraFrameCounter();
                sBattleScene.scene->phaseSubstep += 1;
            }
            break;

        case 2:
            if (sBattleScene.scene->actionPass == BATTLE_ACTION_SECOND) {
                if ((sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_NO_ACTION) || (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_UNABLE_TO_ACT) ||
                    (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_FULL_PARALYSIS)) {
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
    s32 camera_cue;
    s32 camera_preset;

    if (sMultihitCountdown != sMultihitCount) {
        camera_cue = BattleScene_GetFollowupCameraCue(sActivePokemon[sBattleScene.scene->actingSide]) & 0xFFFFFFFF;
        switch (camera_cue) {
            case 20:
                do {
                    camera_preset = D_84385B70[BattleAnim_RandomIntWithWarmup(5.0f)];
                } while (camera_preset == sBattleScene.scene->cameraPresetId);
                sBattleScene.scene->cameraPresetId = camera_preset;
                BattleScene_SelectCameraPreset(sActingPokemon, camera, sBattleScene.scene->cameraPresetId, 1);
                break;

            case 21:
                do {
                    camera_preset = D_84385B78[BattleAnim_RandomIntWithWarmup(4.0f)];
                } while (camera_preset == sBattleScene.scene->cameraPresetId);
                sBattleScene.scene->cameraPresetId = camera_preset;
                BattleScene_SelectCameraPreset(sActingPokemon, camera, sBattleScene.scene->cameraPresetId, 1);
                break;

            case 22:
                do {
                    camera_preset = D_84385B7C[BattleAnim_RandomIntWithWarmup(3.0f)];
                } while (camera_preset == sBattleScene.scene->cameraPresetId);
                sBattleScene.scene->cameraPresetId = camera_preset;
                BattleScene_SelectCameraPreset(sActingPokemon, camera, sBattleScene.scene->cameraPresetId, 1);
                break;

            case 23:
                do {
                    camera_preset = D_84385B80[BattleAnim_RandomIntWithWarmup(6.0f)];
                } while (camera_preset == sBattleScene.scene->cameraPresetId);
                sBattleScene.scene->cameraPresetId = camera_preset;
                if (1) {}
                if (1) {}
                if (1) {}
                if (1) {}
                BattleScene_SelectCameraPreset(sActingPokemon, camera, sBattleScene.scene->cameraPresetId, 1);
                break;

            case 24:
                do {
                    camera_preset = D_84385B88[BattleAnim_RandomIntWithWarmup(4.0f)];
                } while (camera_preset == sBattleScene.scene->cameraPresetId);
                sBattleScene.scene->cameraPresetId = camera_preset;
                BattleScene_SelectCameraPreset(sActingPokemon, camera, sBattleScene.scene->cameraPresetId, 1);
                break;

            default:
                if (camera_cue != sBattleScene.scene->cameraPresetId) {
                    BattleScene_SelectCameraPreset(sActingPokemon, camera, camera_cue, 1);
                }
                break;
        }
    }
}

void BattleScene_SelectActionCameraPresetFromTiming(CameraNode* camera, s32 use_charge_cue) {
    BattleMonRuntime* pokemon_runtime = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;
    s32 camera_cue;

    if ((sBattleScene.scene->cameraPresetId != 1) && (sBattleScene.scene->cameraPresetId != 4) && (sBattleScene.scene->cameraPresetId != 5) &&
        (sBattleScene.scene->cameraPresetId != 8) && (sBattleScene.scene->cameraPresetId != 9) && (sBattleScene.scene->cameraPresetId != 0xA) &&
        (sBattleScene.scene->cameraPresetId != 0xB)) {
        if (use_charge_cue == 1) {
            camera_cue = BattleScene_GetChargeFollowupCameraCue(sActivePokemon[sBattleScene.scene->actingSide]);
        } else {
            camera_cue = BattleScene_GetFollowupCameraCue(sActivePokemon[sBattleScene.scene->actingSide]);
        }

        if ((pokemon_runtime->usedMove == ICE_BEAM) || (pokemon_runtime->usedMove == SKY_ATTACK)) {
            camera_cue = 0x19;
        }

        if (camera_cue == 25) {
            return;
        }

        switch (camera_cue) {
            case 20:
                sBattleScene.scene->cameraPresetId = D_84385B70[BattleAnim_RandomIntWithWarmup(ARRAY_COUNT(D_84385B70))];
                if (camera_cue != sBattleScene.scene->cameraPresetId) {
                    BattleScene_SelectCameraPreset(sActingPokemon, camera, sBattleScene.scene->cameraPresetId, 1);
                }
                break;

            case 21:
                sBattleScene.scene->cameraPresetId = D_84385B78[BattleAnim_RandomIntWithWarmup(ARRAY_COUNT(D_84385B78))];
                if (camera_cue != sBattleScene.scene->cameraPresetId) {
                    BattleScene_SelectCameraPreset(sActingPokemon, camera, sBattleScene.scene->cameraPresetId, 1);
                }
                break;

            case 22:
                sBattleScene.scene->cameraPresetId = D_84385B7C[BattleAnim_RandomIntWithWarmup(ARRAY_COUNT(D_84385B7C))];
                if (camera_cue != sBattleScene.scene->cameraPresetId) {
                    BattleScene_SelectCameraPreset(sActingPokemon, camera, sBattleScene.scene->cameraPresetId, 1);
                }
                break;

            case 23:
                sBattleScene.scene->cameraPresetId = D_84385B80[BattleAnim_RandomIntWithWarmup(ARRAY_COUNT(D_84385B80))];
                if (camera_cue != sBattleScene.scene->cameraPresetId) {
                    BattleScene_SelectCameraPreset(sActingPokemon, camera, sBattleScene.scene->cameraPresetId, 1);
                }
                break;

            case 24:
                sBattleScene.scene->cameraPresetId = D_84385B88[BattleAnim_RandomIntWithWarmup(ARRAY_COUNT(D_84385B88))];
                if (camera_cue != sBattleScene.scene->cameraPresetId) {
                    BattleScene_SelectCameraPreset(sActingPokemon, camera, sBattleScene.scene->cameraPresetId, 1);
                }
                break;

            default:
                if (camera_cue != sBattleScene.scene->cameraPresetId) {
                    if (1) {}
                    BattleScene_SelectCameraPreset(sActingPokemon, camera, camera_cue, 1);
                }
                break;
        }
    }
}

s32 BattleScene_ApplyActionCameraMode(CameraNode* camera, s32 allow_reframe) {
    UNUSED s32 pad;
    s32 camera_handled = 0;

    switch (sBattleScene.scene->cameraPresetId) {
        case 1:
            BattleAnim_ApproachF(&camera->perspective.verticalFieldOfView, 60.0f, 0.03f);
            camera_handled = 1;
            break;

        case 4:
        case 5:
        case 8:
        case 9:
        case 10:
        case 11:
            BattleScene_AnimateCameraPresetToOwner(camera, sActingPokemon);
            camera_handled = 1;
            break;

        default:
            if ((sActingPokemon->modelTransformId != 6) && (sActingPokemon->modelTransformId != 4) && (allow_reframe != 0)) {
                BattleScene_ReframeAndRecomputeDistance(camera, sActingPokemon);
            }
            break;
    }
    return camera_handled;
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
                BattleScene_BeginPhaseSubstep();
                BattleScene_SelectActionCameraPresetFromTiming(camera, 0);
            }
            break;

        case 1:
            BattleScene_ApplyActionCameraMode(camera, 1);
            if (Battle_IsRowAnimationSettled(sActingPokemon, 0x32) != 0) {
                BattleScene_ResetCameraFrameCounter();
                BattleScene_ResetPhaseSubstep();
                BattleScene_CleanupActionPresentation(camera);
            }
            break;
    }
}

void BattleScene_AnimateMultiHitAction(CameraNode* camera) {
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
            BattleScene_ResetPhaseSubstep();
            sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_NORMAL;
            Audio_FadeAndClearStoredSounds();
            BattleScene_CleanupActionPresentation(camera);
            break;
    }
}

void BattleScene_AnimateConfusionAction(CameraNode* camera) {
    switch (sBattleScene.scene->phaseSubstep) {
        case 0:
            if (BattleScene_AdvanceCameraFrameCounter(0xF) != 0) {
                BattleAnim_QueueEffectList(7, sActingPokemon);
                BattleScene_ResetCameraFrameCounter();
                BattleScene_BeginPhaseSubstep();
                BattleScene_SelectCameraFollowPreset(sActingPokemon, camera, 3);
            }
            break;

        case 1:
            BattleScene_ReframeAndRecomputeDistance(camera, sActingPokemon);
            if (Battle_IsRowAnimationSettled(sActingPokemon, 0x1E) != 0) {
                BattleScene_ResetCameraFrameCounter();
                BattleScene_ResetPhaseSubstep();
                BattleScene_CleanupActionPresentation(camera);
            }
            break;
    }
}

void BattleScene_AnimateDigReturnAction(CameraNode* camera) {
    UNUSED s32 pad;
    BattleMonRuntime* pokemon_runtime = &sActingPokemon->state.runtime;

    BattleScene_GetParticipantSideIndex(sActingPokemon);

    switch (sBattleScene.scene->phaseSubstep) {
        case 0:
            BattleScene_ResetCameraFrameCounter();
            BattleScene_BeginPhaseSubstep();
            Vec3f_SetComponentsDuplicate(&gCameraPreset.startingPosition, sActingPokemon->facingDirection * -175.0f, 25.0f, 0.0f);
            BattleScene_LoadCameraPreset(&D_84384958, camera, sActingPokemon);
            if ((sActingPokemon->displayObject.objectType != DUGTRIO) && (sActingPokemon->displayObject.objectType != DIGLETT)) {
                BattleAnim_PlayMoveEffectScript(sActingPokemon, pokemon_runtime->usedMove);
            }
            break;

        case 1:
            if (BattleScene_AnimateCameraPreset(camera, sActingPokemon, 1, 0) != 0) {
                BattleScene_ResetCameraFrameCounter();
                BattleScene_ResetPhaseSubstep();
                BattleScene_CleanupActionPresentation(camera);
            }
            break;
    }
}

void BattleScene_AnimateAirborneAction(CameraNode* camera) {
    switch (sBattleScene.scene->phaseSubstep) {
        case 0:
            BattleScene_ResetCameraFrameCounter();
            BattleScene_BeginPhaseSubstep();
            BattleAnim_GetOwnerDefaultAnchorPosition(sActingPokemon, &gCameraPreset.startingPosition);
            BattleScene_LoadCameraPreset(&D_84384924, camera, sActingPokemon);
            break;

        case 1:
            BattleScene_AnimateCameraPresetToOwner(camera, sActingPokemon);
            if (Battle_IsRowAnimationSettled(sActingPokemon, 0x1E) != 0) {
                BattleScene_ResetCameraFrameCounter();
                BattleScene_ResetPhaseSubstep();
                BattleScene_CleanupActionPresentation(camera);
            }
            break;
    }
}

void BattleScene_ActionSetupAnimationStep(CameraNode* camera) {
    ActivePokemonState* animated_pokemon_state = &sActivePokemon[sBattleScene.scene->actingSide]->state;
    u32 animation_path = ACTION_ANIMATION_NORMAL;

    if (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_MULTI_HIT) {
        animation_path = ACTION_ANIMATION_MULTI_HIT;
    } else if (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_CONFUSION_SELF_HIT) {
        animation_path = ACTION_ANIMATION_CONFUSION;
    } else if (animated_pokemon_state->flags & ACTIVE_UNDERGROUND) {
        animation_path = ACTION_ANIMATION_DIG_RETURN;
    } else if (animated_pokemon_state->flags & ACTIVE_DIG_EMERGE) {
        animation_path = ACTION_ANIMATION_DIG_RETURN;
    } else if (animated_pokemon_state->flags & ACTIVE_AIR_ACTION) {
        animation_path = ACTION_ANIMATION_AIRBORNE;
    }

    switch (animation_path) {
        case ACTION_ANIMATION_NORMAL:
            BattleScene_ActionSetupAnimationDefault(camera);
            break;

        case ACTION_ANIMATION_MULTI_HIT:
            BattleScene_AnimateMultiHitAction(camera);
            break;

        case ACTION_ANIMATION_CONFUSION:
            BattleScene_AnimateConfusionAction(camera);
            break;

        case ACTION_ANIMATION_DIG_RETURN:
            BattleScene_AnimateDigReturnAction(camera);
            break;

        case ACTION_ANIMATION_AIRBORNE:
            BattleScene_AnimateAirborneAction(camera);
            break;

        default:
            BattleScene_ActionSetupAnimationDefault(camera);
            break;
    }
}

void BattleScene_ResetSeismicTossFacing(ActivePokemon* pokemon) {
    if (pokemon->state.runtime.usedMove == SEISMIC_TOSS) {
        BattleAnim_ResetOwnerFacingAndExitStatusIdle(pokemon);
    }
}

s32 BattleScene_IsFireSpin(ActivePokemon* pokemon) {
    if (pokemon->state.runtime.usedMove == FIRE_SPIN) {
        return 1;
    }
    return 0;
}

void BattleScene_CompleteHpUpdateTransition(CameraNode* camera, s32* side_hp_is_consistent, s32* other_side_hp_is_consistent) {
    if ((*side_hp_is_consistent == 1) && (*other_side_hp_is_consistent == 1)) {
        BattleBox_Close();
        Battle_ResetModelToIdleAnim(sActingPokemon);
        sBattleScene.scene->hitReactionKind = BATTLE_HIT_REACTION_NONE;
        BattleScene_ResetCameraFrameCounter();
        BattleScene_ResetCameraSubstepCounter();
        BattleScene_ResetPhaseSubstep();
        BattleScene_ResetModelReloadStep();
        BattleScene_UpdateDisplayedHpAndTransition(camera);
    }
}

void BattleScene_PresentTargetStatus(CameraNode* camera) {
    switch (sBattleScene.scene->phaseSubstep) {
        case 0:
            if (BattleScene_RestoreSubstituteDoll(sNonActingPokemon) == 0) {
                BattleScene_SelectCameraShot(sNonActingPokemon, camera, 0);
                BattleScene_ResetCameraFrameCounter();
                BattleScene_BeginPhaseSubstep();
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

void BattleScene_PresentTargetStatDrop(CameraNode* camera) {
    s32 effect_checkpoint;

    switch (sBattleScene.scene->phaseSubstep) {
        case 0:
            if (BattleScene_RestoreSubstituteDoll(sNonActingPokemon) == 0) {
                Battle_ApplyAnimStateEffects(sNonActingPokemon);
                BattleScene_SelectCameraShot(sNonActingPokemon, camera, 0);
                BattleScene_ResetCameraFrameCounter();
                BattleScene_BeginPhaseSubstep();
                BattleScene_EnterStateRow(sActingPokemon, 0xB);
                BattleScene_CleanupOpponentSpecialAnimFlag(sNonActingPokemon);
                BattleBox_Open(1);
            }
            break;

        case 1:
            BattleScene_FrameCamera(camera, sNonActingPokemon);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sNonActingPokemon);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActingPokemon);
            effect_checkpoint = BattleScene_GetOpponentEffectCheckpoint(sActingPokemon);
            if ((effect_checkpoint == 2) && ((BattleScene_IsOpponentRowCheckpointReached(sNonActingPokemon) != 0) || (BattleScene_AdvanceCameraFrameCounter(0x64) != 0))) {
                BattleScene_CompleteHpUpdateTransition(camera, &sSide1HpConsistent, &sSide2HpConsistent);
            } else if (effect_checkpoint == 1) {
                BattleScene_CompleteHpUpdateTransition(camera, &sSide1HpConsistent, &sSide2HpConsistent);
            }
            break;
    }
}

void BattleScene_PresentDisable(CameraNode* camera) {
    BattleMonRuntime* acting_runtime = &sActingPokemon->state.runtime;
    UNUSED s32 pad;
    UNUSED s16 pad2;
    s16 hit_anchor;

    switch (sBattleScene.scene->phaseSubstep) {
        case 0:
            if (BattleScene_RestoreSubstituteDoll(sNonActingPokemon) == 0) {
                Battle_ApplyAnimStateEffects(sNonActingPokemon);
                BattleScene_SelectCameraShot(sNonActingPokemon, camera, 0);
                BattleScene_ResetCameraFrameCounter();
                BattleScene_BeginPhaseSubstep();
                BattleScene_CleanupOpponentSpecialAnimFlag(sNonActingPokemon);
                BattleBox_Open(1);
                hit_anchor = BattleScene_GetHitCueDefaultAnchor(sNonActingPokemon);
                BattleAnim_QueueMoveEffectEnd(acting_runtime->usedMove, sActingPokemon, sNonActingPokemon, hit_anchor, BattleScene_GetHitCueAlternateAnchor(sNonActingPokemon));
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
    BattleMonRuntime* target_runtime = &sNonActingPokemon->state.runtime;
    UNUSED s32 pad;
    s32 camera_settled;
    s32 framing_settled;
    s32 hold_frames;

    switch (sBattleScene.scene->phaseSubstep) {
        case 0:
            if (BattleScene_RestoreSubstituteDoll(sNonActingPokemon) == 0) {
                BattleScene_ResetSeismicTossFacing(sActingPokemon);
                BattleBox_Open(1);
                BattleScene_ResetCameraFrameCounter();
                BattleScene_BeginPhaseSubstep();
                BattleScene_SetDamageShakeAmplitude(sNonActingPokemon);
                if (sBattleScene.scene->hitReactionKind == BATTLE_HIT_REACTION_SECONDARY) {
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
            BattleScene_ResetModelReloadStep();
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sNonActingPokemon);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActingPokemon);
            if (target_runtime->displayHp != 0) {
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
            framing_settled = BattleScene_FrameCamera(camera, sNonActingPokemon);
            if ((sSide1HpConsistent == 1) && (sSide2HpConsistent == 1) && (framing_settled != 0) && (BattleScene_AdvanceCameraFrameCounter(0xF) != 0)) {
                sBattleScene.scene->phaseSubstep = 6;
            }
            break;

        case 5:
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sNonActingPokemon);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActingPokemon);
            camera_settled = BattleScene_FrameCamera(camera, sNonActingPokemon);
            if ((BattleScene_IsFireSpin(sActingPokemon) != 0) && ((sTypeEffectiveness < 0xA) || (sBattleMoveFailed != 0)) &&
                !(target_runtime->unk_15 & 0x20)) {
                hold_frames = 0x5A;
            } else {
                hold_frames = 0xF;
            }

            if ((sSide1HpConsistent == 1) && (sSide2HpConsistent == 1) && (camera_settled != 0) && (BattleScene_AdvanceCameraFrameCounter(hold_frames) != 0)) {
                sBattleScene.scene->phaseSubstep++;
            }
            break;

        case 6:
            if (BattleScene_BreakSubstitute(sNonActingPokemon) == 0) {
                sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sNonActingPokemon);
                sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActingPokemon);
                BattleScene_FrameCamera(camera, sNonActingPokemon);
                BattleScene_CompleteHpUpdateTransition(camera, &sSide1HpConsistent, &sSide2HpConsistent);
            }
            break;
    }
}

void BattleScene_NoOp84324404(void) {
    // Empty and uncalled in this source tree; its original role is unknown.
}

void BattleScene_NoOp8432440C(void) {
    // Empty and uncalled in this source tree; its original role is unknown.
}

void BattleScene_RunRepeatedHitHpTransition(CameraNode* camera) {
    BattleMonRuntime* acting_pokemon_runtime = &sActingPokemon->state.runtime;
    MovePresentationCue* move_cue = &sSideBattleCues[BattleScene_GetParticipantSideIndex(sNonActingPokemon)]->moveCues[acting_pokemon_runtime->usedMove - 1];
    volatile s32 frames_left;
    s32 camera_settled;

    if (sNonActingPokemon->displayObject.objectType == SUBSTITUTE_DOLL) {
        frames_left = 0xF;
    } else if (move_cue->frameCounter == 0xFF) {
        frames_left = sActingPokemon->displayObject.transformAnim.data->endFrame - move_cue->currentFrame;
    } else {
        frames_left = move_cue->frameCounter - move_cue->currentFrame;
    }

    switch (sBattleScene.scene->phaseSubstep) {
        case 0:
            if (BattleScene_RestoreSubstituteDoll(sNonActingPokemon) == 0) {
                BattleScene_ResetModelReloadStep();
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
            if (BattleScene_AdvanceCameraFrameCounter(frames_left) != 0) {
                Audio_FadeAndClearStoredSounds();
                BattleAnim_CleanupEffects(1);
                sBattleScene.scene->phaseSubstep = 0;
            }
            break;

        case 2:
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sNonActingPokemon);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActingPokemon);

            if (acting_pokemon_runtime->displayHp != 0) {
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
            camera_settled = BattleScene_FrameCamera(camera, sNonActingPokemon);
            if ((sSide1HpConsistent == 1) && (sSide2HpConsistent == 1) && (camera_settled != 0) && (BattleScene_AdvanceCameraFrameCounter(0xF) != 0)) {
                sBattleScene.scene->phaseSubstep++;
            }
            break;

        case 6:
            if (BattleScene_BreakSubstitute(sNonActingPokemon) == 0) {
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
    if (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_TARGET_STATUS) {
        BattleScene_PresentTargetStatus(camera);
    } else if (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_DISABLE) {
        BattleScene_PresentDisable(camera);
    } else if (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_TARGET_STAT_DROP) {
        BattleScene_PresentTargetStatDrop(camera);
    } else if (sBattleScene.scene->hitReactionKind == BATTLE_HIT_REACTION_MULTI_HIT) {
        BattleScene_RunRepeatedHitHpTransition(camera);
    } else {
        BattleScene_RunActionEffectHpTransition(camera);
    }
}

/**
 * Phase 2. The first attacker's action: action camera setup, move animation setup, then finalize.
 * Steps are identical to BattleScene_PhaseSecondAttackerAction.
 */
void BattleScene_PhaseFirstAttackerAction(CameraNode* camera) {
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

/**
 * Phase 3. The second attacker's action; Battle_ActSecondAttacker(BATTLE_PHASE_SECOND_ACTION) queues it through
 * BattleScene_PhaseEnterQueuedPhase. Steps are identical to BattleScene_PhaseFirstAttackerAction.
 */
void BattleScene_PhaseSecondAttackerAction(CameraNode* camera) {
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

/**
 * Phase 37. Entered from BattleScene_ActionPresentationTransitionCheck for moves with a dedicated
 * presentation. Holds the action camera for the move cue's shot duration, opens the move box, then switches
 * to the phase that matches scene->moveEffectKind (7, 12-15, 17, 21, 25, 27-31, 38, 39). Returns 1 on a switch.
 */
s32 BattleScene_PhaseDispatchMoveEffect(void) {
    BattleScene_ApplyActionCameraMode(sBattleScene.scene->camera, 0);

    if (BattleScene_AdvanceCameraFrameCounter(BattleScene_GetCameraShotDuration(sActingPokemon)) == 0) {
        return 0;
    }

    if ((sBattleScene.scene->moveEffectKind != MOVE_PRESENTATION_EXPLOSION) && (sBattleScene.scene->moveEffectKind != MOVE_PRESENTATION_TELEPORT)) {
        BattleBox_Open(3);
    }

    if (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_BIDE) {
        QueuePlayerDefeatMessage(BattleScene_GetParticipantDisplayPlayerNumber(sActingPokemon));
    }

    switch (sBattleScene.scene->moveEffectKind) {
        case MOVE_PRESENTATION_SELF_TARGET:
            return BattleScene_TransitionAndClearMoveEffect(BATTLE_PHASE_SELF_TARGET);

        case MOVE_PRESENTATION_HEAL:
            return BattleScene_TransitionAndClearMoveEffect(BATTLE_PHASE_SELF_TARGET);

        case MOVE_PRESENTATION_TELEPORT:
            return BattleScene_TransitionAndClearMoveEffect(BATTLE_PHASE_SELF_TARGET);

        case MOVE_PRESENTATION_BIDE:
            return BattleScene_TransitionAndClearMoveEffect(BATTLE_PHASE_BIDE);

        case MOVE_PRESENTATION_EXPLOSION:
            return BattleScene_TransitionAndClearMoveEffect(BATTLE_PHASE_EXPLOSION);

        case MOVE_PRESENTATION_NO_ACTION:
            return BattleScene_TransitionAndClearMoveEffect(BATTLE_PHASE_NO_ACTION);

        case MOVE_PRESENTATION_TRANSFORM:
            return BattleScene_TransitionAndClearMoveEffect(BATTLE_PHASE_TRANSFORM);

        case MOVE_PRESENTATION_SUBSTITUTE:
            return BattleScene_TransitionAndClearMoveEffect(BATTLE_PHASE_SUBSTITUTE);

        case MOVE_PRESENTATION_HAZE:
            return BattleScene_TransitionAndClearMoveEffect(BATTLE_PHASE_HAZE);

        case MOVE_PRESENTATION_WITHDRAW:
            return BattleScene_TransitionAndClearMoveEffect(BATTLE_PHASE_WITHDRAW);

        case MOVE_PRESENTATION_FLY_START:
            return BattleScene_TransitionAndClearMoveEffect(BATTLE_PHASE_FLY_START);

        case MOVE_PRESENTATION_DIG_START:
            return BattleScene_TransitionAndClearMoveEffect(BATTLE_PHASE_DIG_START);

        case MOVE_PRESENTATION_CHARGE:
            return BattleScene_TransitionAndClearMoveEffect(BATTLE_PHASE_CHARGE);

        case MOVE_PRESENTATION_UNABLE_TO_ACT:
            return BattleScene_TransitionAndClearMoveEffect(BATTLE_PHASE_UNABLE_TO_ACT);

        case MOVE_PRESENTATION_FULL_PARALYSIS:
            return BattleScene_TransitionAndClearMoveEffect(BATTLE_PHASE_FULL_PARALYSIS);
    }

    return 0;
}

/**
 * Phase 7 (moveEffectKind 7, 21, 39). Self-targeted move presentation: applies the animation state effects
 * when the move cue's animationDuration is reached, waits for the row animation, then settles both HP bars.
 */
void BattleScene_PhaseSelfTargetMove(CameraNode* camera) {
    BattleMonRuntime* acting_pokemon_runtime = &sActingPokemon->state.runtime;
    MovePresentationCue* move_cue = &sSideBattleCues[BattleScene_GetParticipantSideIndex(sActingPokemon)]->moveCues[acting_pokemon_runtime->usedMove - 1];

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_SaveCameraFov(camera);
            BattleScene_SelectActionCameraPresetFromTiming(camera, 0);
            BattleScene_ResetCameraFrameCounter();
            BattleScene_ResetCameraSubstepCounter();
            if (move_cue->animationDuration < 21) {
                Battle_ApplyAnimStateEffects(sActingPokemon);
            }
            sBattleScene.scene->phaseStep++;
            break;

        case 1:
            BattleScene_ApplyActionCameraMode(camera, 1);
            if (sActingPokemon->animationTimer == (move_cue->animationDuration + 1)) {
                Battle_ApplyAnimStateEffects(sActingPokemon);
            }

            if (sActingPokemon->animationTimer >= (move_cue->animationDuration + 10)) {
                sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
                sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            }

            if ((sActingPokemon->animationTimer == (move_cue->animationDuration + 18)) && (acting_pokemon_runtime->usedMove == TELEPORT)) {
                BattleBox_Open(15);
            }

            if (Battle_IsRowAnimationSettled(sActingPokemon, 60) != 0) {
                BattleScene_ResetCameraFrameCounter();
                sBattleScene.scene->phaseStep++;
            }
            break;

        case 2:
            if (sActingPokemon->animationTimer == (move_cue->animationDuration + 1)) {
                Battle_ApplyAnimStateEffects(sActingPokemon);
            }
            BattleScene_ApplyActionCameraMode(camera, 1);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            if ((sSide1HpConsistent == 1) && (sSide2HpConsistent == 1) && (BattleScene_AdvanceCameraFrameCounter(0xF) != 0)) {
                if ((sNonActingPokemon->state.runtime.usedMove & 0xF0) == 0xF0) {
                    sBattleScene.scene->actionPass = BATTLE_ACTION_COMPLETE;
                }
                BattleScene_ResetCameraFrameCounter();
                BattleScene_ResetModelReloadStep();
                BattleScene_WaitForDisplayedHpAnimations(camera);
            }
            break;
    }
}

/**
 * Phase 38 (moveEffectKind 38). Withdraw move presentation: owner-shot camera, row animation, then HP settle.
 */
void BattleScene_PhaseWithdrawMove(CameraNode* camera) {
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
            if (Battle_IsRowAnimationSettled(sActingPokemon, 60) != 0) {
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 2:
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            if ((sNonActingPokemon->state.runtime.usedMove & 0xF0) == 0xF0) {
                sBattleScene.scene->actionPass = BATTLE_ACTION_COMPLETE;
            }
            BattleScene_ResetCameraFrameCounter();
            BattleScene_ResetModelReloadStep();
            BattleScene_WaitForDisplayedHpAnimations(camera);
            break;
    }
}

/**
 * Phase 27 (moveEffectKind 27). Bide presentation: holds the action camera for 40 frames, then settles HP.
 */
void BattleScene_PhaseBide(CameraNode* camera) {
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
            if (BattleScene_AdvanceCameraFrameCounter(40) != 0) {
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 2:
            BattleScene_ApplyActionCameraMode(camera, 1);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            if ((sNonActingPokemon->state.runtime.usedMove & 0xF0) == 0xF0) {
                sBattleScene.scene->actionPass = BATTLE_ACTION_COMPLETE;
            }
            BattleScene_ResetCameraFrameCounter();
            BattleScene_ResetModelReloadStep();
            BattleScene_WaitForDisplayedHpAnimations(camera);
            break;
    }
}

/**
 * Phase 30 (moveEffectKind 30). Explosion and Self-Destruct: clears the user's substitute, plays the move,
 * then cuts to the non-acting Pokemon for its damage and enters BattleScene_PhaseFaint if its display HP is 0.
 */
void BattleScene_PhaseExplosion(CameraNode* camera) {
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
            if (Battle_IsRowAnimationSettled(sActingPokemon, 60) != 0) {
                BattleScene_ResetCameraFrameCounter();
                sBattleScene.scene->phaseStep++;
            }
            break;

        case 2:
            if (BattleScene_RestoreSubstituteDoll(sNonActingPokemon) == 0) {
                BattleAnim_CleanupEffects(1);
                Audio_FadeAndClearStoredSounds();
                BattleScene_SelectCameraShot(sNonActingPokemon, camera, BattleAnim_RandomRange(2));
                BattleScene_EnterStateRow(sActingPokemon, 4);
                BattleScene_ResetCameraFrameCounter();
                sBattleScene.scene->phaseStep++;
            }
            break;

        case 3:
            if (BattleScene_BreakSubstitute(sNonActingPokemon) == 0) {
                BattleScene_ReframeAndRecomputeDistance(camera, sNonActingPokemon);
                if (sBattleScene.scene->stepFrameTimer == 0) {
                    BattleBox_Open(1);
                }
                sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
                sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
                if (BattleScene_AdvanceCameraFrameCounter(50) != 0) {
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
                        sBattleScene.scene->actionPass = BATTLE_ACTION_COMPLETE;
                    }
                    BattleScene_ResetCameraFrameCounter();
                    BattleScene_ResetModelReloadStep();
                    BattleScene_SetPhase(camera, BATTLE_PHASE_FAINT);
                }
            }
            break;

        case 5:
            BattleScene_SelectCameraFollowPreset(sActingPokemon, camera, 3);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            if ((sNonActingPokemon->state.runtime.usedMove & 0xF0) == 0xF0) {
                sBattleScene.scene->actionPass = BATTLE_ACTION_COMPLETE;
            }
            BattleScene_ResetCameraFrameCounter();
            BattleScene_ResetModelReloadStep();
            BattleScene_ResolveDisplayedHpCompletion(camera);
            break;
    }
}

/**
 * Phase 28 (MOVE_PRESENTATION_NO_ACTION). Shared no-action presentation: flinching, failed follow-up moves,
 * failed Transform/Substitute, and healing at full HP can all select this phase.
 * Follow camera for 40 frames, then settles HP.
 */
void BattleScene_PhaseNoAction(CameraNode* camera) {
    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_SaveCameraFov(camera);
            Battle_ApplyAnimStateEffects(sActingPokemon);
            BattleScene_SelectCameraFollowPreset(sActingPokemon, camera, 3);
            BattleScene_ResetCameraFrameCounter();
            sBattleScene.scene->phaseStep += 1;
            break;

        case 1:
            if (BattleScene_AdvanceCameraFrameCounter(40) != 0) {
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 2:
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            if ((sNonActingPokemon->state.runtime.usedMove & 0xF0) == 0xF0) {
                sBattleScene.scene->actionPass = BATTLE_ACTION_COMPLETE;
            }
            BattleScene_ResetCameraFrameCounter();
            BattleScene_ResetModelReloadStep();
            BattleScene_WaitForDisplayedHpAnimations(camera);
            break;
    }
}

/**
 * Phase 15 (moveEffectKind 15). The Transform move: sets the acting side's session-team display model from
 * the opponent's (the opponent's unk_BE when its battleStatus3 bit 8 is set, its id when its display model
 * is 0x98), loads that model, plays the move effect script, and waits on row 18.
 */
void BattleScene_PhaseTransform(CameraNode* camera) {
    UNUSED s32 pad[3];
    BattleMonRuntime* acting_pokemon_runtime = &sActingPokemon->state.runtime;
    BattleMonRuntime* non_acting_pokemon_runtime = &sNonActingPokemon->state.runtime;
    ActivePokemonState* acting_pokemon_state = &sActingPokemon->state;
    ActivePokemonState* non_acting_pokemon_state = &sNonActingPokemon->state;

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_SelectActionCameraPresetFromTiming(camera, 0);
            BattleScene_RecenterCameraOnOwner(camera, sActingPokemon);
            if (acting_pokemon_runtime->unk_4D & 0x10) {
                if (non_acting_pokemon_runtime->battleStatus3 & 8) {
                    acting_pokemon_state->model = sActingPokemon->sessionTeams->displayModel = non_acting_pokemon_state->unk_BE;
                } else if (sNonActingPokemon->sessionTeams->displayModel == SUBSTITUTE_DOLL) {
                    acting_pokemon_state->model = sActingPokemon->sessionTeams->displayModel = non_acting_pokemon_runtime->id;
                } else {
                    acting_pokemon_state->model = sActingPokemon->sessionTeams->displayModel = sNonActingPokemon->sessionTeams->displayModel;
                }
            } else if (non_acting_pokemon_runtime->battleStatus3 & 8) {
                sActingPokemon->sessionTeams->displayModel = non_acting_pokemon_state->unk_BE;
            } else if (sNonActingPokemon->sessionTeams->displayModel == SUBSTITUTE_DOLL) {
                sActingPokemon->sessionTeams->displayModel = non_acting_pokemon_runtime->id;
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
                BattleAnim_PlayMoveEffectScript(sActingPokemon, acting_pokemon_runtime->usedMove);
                BattleAnim_PlaySpeciesReactionSound(sActingPokemon, acting_pokemon_runtime, 0);
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 2:
            BattleScene_RecenterCameraOnOwner(camera, sActingPokemon);
            BattleAnim_PlaySpeciesReactionSound(sActingPokemon, acting_pokemon_runtime, 0);
            BattleScene_EnterStateRow(sActingPokemon, 18);
            sBattleScene.scene->phaseStep += 1;
            break;

        case 3:
            BattleScene_RecenterCameraOnOwner(camera, sActingPokemon);
            sActingPokemon->thumbnail.camera->node.flags |= 1;
            sActingPokemon->thumbnail.displayObject.node.flags |= 1;
            BattleThumbnail_TriggerOwnerModelAnimation(sActingPokemon);
            if (BattleScene_IsRowWaitComplete(sActingPokemon, 50) != 0) {
                BattleScene_ResetModelReloadStep();
                BattleScene_UpdateDisplayedHpAndTransition(camera);
            }
            break;
    }
}

/**
 * Phase 25 (moveEffectKind 25). Substitute creation: plays the move effect script and calls
 * Battle_MakeSubstitute unless ACTIVE_SUBSTITUTE is already set, then sets ACTIVE_SUBSTITUTE in step 3.
 */
void BattleScene_PhaseSubstitute(CameraNode* camera) {
    BattleMonRuntime* acting_runtime = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;
    ActivePokemonState* acting_state = &sActivePokemon[sBattleScene.scene->actingSide]->state;

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_SaveCameraFov(camera);
            BattleAnim_PlayMoveEffectScript(sActingPokemon, acting_runtime->usedMove);
            BattleScene_SelectActionCameraPresetFromTiming(camera, 0);
            if (!(acting_state->flags & ACTIVE_SUBSTITUTE)) {
                Battle_MakeSubstitute(sActingPokemon);
            }
            sBattleScene.scene->phaseStep += 1;
            break;

        case 1:
            BattleScene_ApplyActionCameraMode(camera, 0);
            if (!(acting_state->flags & ACTIVE_SUBSTITUTE)) {
                BattleScene_ResetOwnerModelIfLoaded(sActingPokemon);
            }
            if (BattleAnim_IsSessionBusy(sActingPokemon) == 0) {
                BattleAnim_PlaySpeciesReactionSound(sActingPokemon, acting_runtime, 0);
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
            if (!(acting_state->flags & ACTIVE_SUBSTITUTE)) {
                Battle_SetRuntimeFlags(sActingPokemon, ACTIVE_SUBSTITUTE);
            }
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            if ((sNonActingPokemon->state.runtime.usedMove & 0xF0) == 0xF0) {
                sBattleScene.scene->actionPass = BATTLE_ACTION_COMPLETE;
            }
            BattleScene_ResetModelReloadStep();
            BattleScene_WaitForDisplayedHpAnimations(camera);
            break;
    }
}

s32 BattleScene_EndStatusAnimationsAndCommit(ActivePokemon* pokemon) {
    BattleAnim_ClearFreezeStatusAnimation(pokemon);
    BattleAnim_EndSleepStatusAnimation(pokemon);
    Battle_ApplyAnimStateEffects(pokemon);
}

/**
 * Phase 29 (moveEffectKind 29). Haze presentation: after the row animation it ends the non-acting Pokemon's
 * freeze and sleep animations, resets both models' scale to 1.0, then settles HP.
 */
void BattleScene_PhaseHaze(CameraNode* camera) {
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
                sBattleScene.scene->actionPass = BATTLE_ACTION_COMPLETE;
            }
            Vec3f_SetComponentsDuplicate(&sActingPokemon->displayObject.scale, 1.0f, 1.0f, 1.0f);
            Vec3f_SetComponentsDuplicate(&sNonActingPokemon->displayObject.scale, 1.0f, 1.0f, 1.0f);
            BattleScene_ResetCameraFrameCounter();
            BattleScene_ResetModelReloadStep();
            BattleScene_WaitForDisplayedHpAnimations(camera);
            break;
    }
}

/**
 * Phase 12 (moveEffectKind 12). Fly's launch: sets ACTIVE_AIRBORNE, follows the Pokemon up until its
 * position.y reaches 200 with the flyingIdleCue animation and a 60-degree field of view, then settles HP.
 */
void BattleScene_PhaseFly(CameraNode* camera) {
    MovePresentationCue* flying_idle_cue = &sSideBattleCues[sBattleScene.scene->actingSide]->flyingIdleCue;
    UNUSED s32 pad;
    Vec3f camera_target;

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_ResetCameraFrameCounter();
            BattleScene_ResetPhaseSubstep();
            BattleScene_SelectCameraFollowPreset(sActingPokemon, camera, 3);
            BattleScene_ApproachCameraTargetFromOwner(camera, sActingPokemon);
            Battle_SetRuntimeFlags(sActingPokemon, ACTIVE_AIRBORNE);
            sBattleScene.scene->phaseStep += 1;
            break;

        case 1:
            if (sActingPokemon->displayObject.position.y >= 100.0f) {
                BattleAnim_GetOwnerDefaultAnchorPosition(sActingPokemon, &camera_target);
                camera_target.y += BattleAnim_GetOwnerModelHeight(sActingPokemon);
                sFlyCameraDistance = BattleScene_ScaleCameraDistance(sActingPokemon, 3.0f, 500.0f);
                BattleScene_SetCameraEyeFromPosition(camera, camera_target, 0x11C6, BattleScene_GetParticipantFacingAngle(sActingPokemon) - (sActingPokemon->facingDirection * 0x754E),
                              sFlyCameraDistance, BattleAnim_GetOwnerModelHeight(sActingPokemon));
                Vec3f_SetComponentsDuplicate(&camera_target, 0.0f, 200.0f, 0.0f);
                BattleAnim_Vec3fCopy(&camera->view.targetPosition, &camera_target);
                BattleAnim_CleanupEffects(2);
                ModelAnim_SetAnimation(&sActingPokemon->displayObject, flying_idle_cue->id);
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
                BattleScene_ResetModelReloadStep();
                BattleScene_UpdateDisplayedHpAndTransition(camera);
            }
            break;
    }
}

/**
 * Phase 13 (moveEffectKind 13). Dig's burrow: waits until the Pokemon sinks below the ground (position.y
 * below a multiple of its model height; Diglett/Dugtrio wait on their row animation instead), sets
 * ACTIVE_UNDERGROUND, then settles HP.
 */
void BattleScene_PhaseDig(CameraNode* camera) {
    UNUSED s32 pad;
    f32 depth_multiplier;
    UNUSED s16 pad2;
    u16 deep_burrow_species = sDeepBurrowSpeciesPair;

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_SaveCameraFov(camera);
            BattleScene_ResetCameraFrameCounter();
            BattleScene_ResetPhaseSubstep();
            BattleScene_SelectCameraFollowPreset(sActingPokemon, camera, 3);
            sBattleScene.scene->phaseStep += 1;
            break;

        case 1:
            if (sActingPokemon->modelTransformTimer == 2) {
                BattleScene_UpdateCameraVerticalFraming(camera, sActingPokemon);
                sActingPokemon->displayObject.node.renderFlags &= 0xFFBF;
            }

            if ((sActingPokemon->displayObject.objectType == DIGLETT) || (sActingPokemon->displayObject.objectType == DUGTRIO)) {
                if (BattleScene_IsRowWaitComplete(sActingPokemon, 0x1E) != 0) {
                    BattleBox_Close();
                    Battle_SetRuntimeFlags(sActingPokemon, ACTIVE_UNDERGROUND);
                    sActingPokemon->displayObject.node.renderFlags &= 0xFFDF;
                    sBattleScene.scene->phaseStep += 1;
                }
            } else if (BattleAnim_IsAssetLoadBusy(sActingPokemon) == 0) {
                if (BattleScene_ByteArrayContains(sActingPokemon->displayObject.objectType, (u8*)&deep_burrow_species, 2) != 0) {
                    depth_multiplier = 10.0f;
                } else {
                    depth_multiplier = 3.5f;
                }

                if (sActingPokemon->displayObject.position.y <= (-BattleAnim_GetOwnerModelHeight(sActingPokemon) * depth_multiplier)) {
                    Battle_SetRuntimeFlags(sActingPokemon, ACTIVE_UNDERGROUND);
                    sActingPokemon->displayObject.node.renderFlags &= 0xFFDF;
                    sBattleScene.scene->phaseStep += 1;
                }
            }
            break;

        case 2:
            if ((sActingPokemon->displayObject.objectType != DIGLETT) && (sActingPokemon->displayObject.objectType != DUGTRIO)) {
                sActingPokemon->displayObject.node.renderFlags |= 0x40;
            }
            BattleScene_UpdateCameraVerticalFraming(camera, sActingPokemon);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            BattleScene_ResetModelReloadStep();
            BattleScene_WaitForDisplayedHpAnimations(camera);
            break;
    }
}

/**
 * Phase 14 (moveEffectKind 14). Charge turn of a two-turn move: plays the animation using the timing field
 * keyed by the move (BattleScene_IsMoveKeyedRowWaitComplete), then settles HP.
 */
void BattleScene_PhaseTwoTurnCharge(CameraNode* camera) {
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
                BattleScene_ResetModelReloadStep();
                BattleScene_WaitForDisplayedHpAnimations(camera);
            }
            break;
    }
}

/**
 * Phase 17 (moveEffectKind 17). A Pokemon that cannot act because it is asleep or immobilized: applies
 * the status animation, waits 60 frames, then settles HP.
 */
void BattleScene_PhaseSleepOrImmobilized(CameraNode* camera) {
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
            BattleScene_ResetModelReloadStep();
            BattleScene_WaitForDisplayedHpAnimations(camera);
            break;
    }
}

/**
 * Phase 20 (moveEffectKind 20). The acting Pokemon damages itself: opens box 2, plays row 15 (the same row
 * as BattleScene_PhaseJumpKickCrash) with the damage shake, then updates HP. Matches confusion self-damage.
 */
void BattleScene_PhaseConfusionSelfHit(CameraNode* camera) {
    BattleScene_ResetCameraFov(camera);

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            if (BattleScene_RestoreSubstituteDoll(sActingPokemon) == 0) {
                BattleBox_Open(2);
                BattleScene_ResetOwnerPresentationFlags(sActingPokemon);
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 1:
            BattleScene_ResetModelReloadStep();
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
            if (BattleScene_BreakSubstitute(sActingPokemon) == 0) {
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 4:
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            BattleScene_FrameCamera(camera, sActingPokemon);
            BattleScene_ResetModelReloadStep();
            BattleScene_UpdateDisplayedHpAndTransition(camera);
            break;
    }
}

/**
 * Phase 31 (moveEffectKind 31). Full paralysis: applies the status animation, sets unk_4C8, waits 60 frames,
 * then settles HP.
 */
void BattleScene_PhaseFullParalysis(CameraNode* camera) {
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
            BattleScene_ResetModelReloadStep();
            BattleScene_WaitForDisplayedHpAnimations(camera);
            break;
    }
}

void BattleScene_NoOp84326A70(void) {
    // Empty and uncalled in this source tree; its original role is unknown.
}

/**
 * Phase 4. End of turn: BattleScene_BeginBattlePhase clears the selected commands and prepares the camera
 * substate for the next action selection.
 */
void BattleScene_PhaseEndTurn(UNUSED CameraNode* camera) {
    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_BeginBattlePhase();

        case 1:
            break;
    }
}

/**
 * Phase 16 (moveEffectKind 16). Hi Jump Kick crash: plays row 15 with the species' hitCue animation and the
 * damage shake on the acting Pokemon, then updates HP.
 */
void BattleScene_PhaseJumpKickCrash(CameraNode* camera) {
    UNUSED s32 pad;
    s32 idx = sBattleScene.scene->actingSide;
    MovePresentationCue* hit_cue = &sSideBattleCues[idx]->hitCue;

    BattleScene_ResetCameraFov(camera);

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_ResetModelReloadStep();
            BattleBox_Open(2);
            sBattleScene.scene->phaseStep += 1;
            break;

        case 1:
            if (BattleScene_RestoreSubstituteDoll(sActingPokemon) == 0) {
                Battle_ResetModelToIdleAnim(sActingPokemon);
                BattleAnim_CleanupEffects(1);
                BattleScene_EnterStateRow(sActingPokemon, 0xF);
                BattleScene_SelectCameraShot(sActingPokemon, camera, 3);
                BattleScene_SetDamageShakeAmplitude(sActingPokemon);
                BattleScene_ResetModelReloadStep();
                ModelAnim_SetAnimation(&sActingPokemon->displayObject, hit_cue->id);
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 2:
            if (BattleScene_AdvanceCameraFrameCounter(0x28) != 0) {
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 3:
            if (BattleScene_BreakSubstitute(sActingPokemon) == 0) {
                BattleBox_Close();
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 4:
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            if ((sSide1HpConsistent != 0) && (sSide2HpConsistent != 0)) {
                BattleScene_ResetModelReloadStep();
                BattleScene_UpdateDisplayedHpAndTransition(camera);
            }
            break;
    }
}

/**
 * Phase 8 (moveEffectKind 8, or sBattleMoveFailed set). Move failed presentation: frames the non-acting
 * Pokemon, enters row 12, calls BattleAnim_UpdateMoveFailedFlag, and holds 50 frames before the HP update.
 */
void BattleScene_PhaseMoveFailedMessage(CameraNode* camera) {
    ActivePokemonState* non_acting_pokemon_state = &sNonActingPokemon->state;

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            if (BattleScene_RestoreSubstituteDoll(sNonActingPokemon) == 0) {
                BattleBox_Open(2);
                BattleScene_ResetSeismicTossFacing(sActingPokemon);
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
                BattleScene_ResetPhaseSubstep();
                BattleScene_ResetCameraFrameCounter();
                BattleScene_ResetModelReloadStep();
                BattleScene_UpdateDisplayedHpAndTransition(camera);
            }
            break;
    }
}

/**
 * Phase 6. Poison/burn-style residual damage after a move: BattleScene_UpdateDisplayedHpAndTransition enters it
 * when the acting Pokemon has unk_15 bits 0x18 or unk_4D bit 0x80 set. Plays effect list 5 or 6 (unk_15 bits)
 * and effect list 10 (unk_4D bit 0x80), then flips the acting side and enters BattleScene_PhaseFaint if the
 * acting Pokemon reaches 0 HP.
 */
void BattleScene_PhaseResidualDamageAfterMove(CameraNode* camera) {
    BattleMonRuntime* acting_runtime = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;
    BattleMonRuntime* other_runtime = &sActivePokemon[!sBattleScene.scene->actingSide]->state.runtime;

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            Battle_StopBeingGoldAfterHarden(sActingPokemon);
            BattleScene_SetCameraOwnerStandardShot(camera, sActivePokemon[sBattleScene.scene->actingSide]);
            BattleScene_ResetModelReloadStep();
            BattleScene_CleanupParticipantEffectMode(sActingPokemon);
            BattleAnim_CleanupEffects(2);
            sBattleScene.scene->phaseStep += 1;
            break;

        case 1:
            if (BattleScene_RevealSubstituteUser(sActivePokemon[sBattleScene.scene->actingSide]) == 0) {
                Battle_ResetModelToIdleAnim(sActingPokemon);
                Audio_FadeAndClearStoredSounds();
                BattleAnim_CleanupEffects(1);
                BattleScene_SetCameraOwnerStandardShot(camera, sActivePokemon[sBattleScene.scene->actingSide]);
                BattleScene_EnterStateRow(sActivePokemon[sBattleScene.scene->actingSide], 0xD);

                if (acting_runtime->unk_15 & 0x18) {
                    BattleBox_Open(4);
                    acting_runtime->displayHp = acting_runtime->unk_0E;
                    other_runtime->displayHp = other_runtime->unk_0E;
                    if (acting_runtime->unk_15 & 0x10) {
                        BattleAnim_QueueEffectList(6, sActingPokemon);
                    } else {
                        BattleAnim_QueueEffectList(5, sActingPokemon);
                    }
                    sBattleScene.scene->phaseStep += 1;
                } else {
                    BattleBox_Open(5);
                    Audio_PlayCommand(0x22, 0, 0);
                    acting_runtime->displayHp = acting_runtime->unk_10;
                    other_runtime->displayHp = other_runtime->unk_10;
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
                    BattleScene_ResetPhaseSubstep();
                    BattleScene_SetPhase(camera, BATTLE_PHASE_FAINT);
                    sBattleScene.scene->actingSide = !sBattleScene.scene->actingSide;
                } else if (acting_runtime->unk_4D & 0x80) {
                    BattleScene_ResetCameraFrameCounter();
                    Audio_PlayCommand(0x22, 0, 0);
                    acting_runtime->displayHp = acting_runtime->unk_10;
                    other_runtime->displayHp = other_runtime->unk_10;
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
            if (BattleScene_BreakSubstitute(sNonActingPokemon) == 0) {
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 5:
            BattleScene_ResetModelReloadStep();
            BattleScene_SetCameraOwnerStandardShot(camera, sActivePokemon[sBattleScene.scene->actingSide]);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            BattleScene_ResolveDisplayedHpCompletion(camera);
            break;
    }
}

/**
 * Phase 9 (moveEffectKind 9). The acting Pokemon takes damage from its own move: plays effect list 9, the
 * hit cue animation, and the damage shake while displayHp moves toward unk_12, then updates HP.
 */
void BattleScene_PhaseRecoilDamage(CameraNode* camera) {
    BattleMonRuntime* acting_runtime = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;
    BattleMonRuntime* other_runtime = &sActivePokemon[!sBattleScene.scene->actingSide]->state.runtime;

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_CleanupParticipantEffectMode(sActingPokemon);
            BattleAnim_CleanupEffects(2);
            BattleScene_SetCameraOwnerWideShot(camera, sActivePokemon[sBattleScene.scene->actingSide]);
            sBattleScene.scene->phaseStep += 1;
            break;

        case 1:
            if (BattleScene_RevealSubstituteUser(sActivePokemon[sBattleScene.scene->actingSide]) == 0) {
                Battle_ResetModelToIdleAnim(sActingPokemon);
                Audio_FadeAndClearStoredSounds();
                BattleAnim_CleanupEffects(1);
                BattleAnim_QueueEffectList(9, sActingPokemon);
                BattleScene_ResetCameraFrameCounter();
                BattleScene_SetCameraOwnerWideShot(camera, sActivePokemon[sBattleScene.scene->actingSide]);
                acting_runtime->displayHp = acting_runtime->unk_12;
                other_runtime->displayHp = other_runtime->unk_12;
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
            if (BattleScene_BreakSubstitute(sNonActingPokemon) == 0) {
                BattleScene_FrameCamera(camera, sActingPokemon);
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 4:
            BattleScene_ResetModelReloadStep();
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            BattleScene_FrameCamera(camera, sActingPokemon);
            if ((sSide1HpConsistent == 1) && (sSide2HpConsistent == 1) && (BattleScene_AdvanceCameraFrameCounterUntil(0x3C) != 0)) {
                BattleScene_UpdateDisplayedHpAndTransition(camera);
            }
            break;
    }
}

/**
 * Phase 10 (selected when scene->rageFollowupPhase is 10). Rage attack increase: plays effect list 6 on the non-acting
 * Pokemon with a standard shot, opens the box at index 7, and holds the shot before updating HP.
 */
void BattleScene_PhaseRage(CameraNode* camera) {
    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_CleanupParticipantEffectMode(sActingPokemon);
            if (BattleScene_RevealSubstituteUser(sActivePokemon[sBattleScene.scene->actingSide]) == 0) {
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

/**
 * Phase 35. Same residual damage presentation as BattleScene_PhaseResidualDamageAfterMove for a Pokemon that
 * has no move this turn (usedMove high nibble 0xF); entered by BattleScene_PrepareResidualStatusPresentation.
 */
void BattleScene_PhaseResidualDamageWithoutMove(CameraNode* camera) {
    BattleMonRuntime* acting_runtime = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;
    BattleMonRuntime* other_runtime = &sActivePokemon[!sBattleScene.scene->actingSide]->state.runtime;

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_CleanupParticipantEffectMode(sActingPokemon);
            BattleAnim_CleanupEffects(2);
            sBattleScene.scene->phaseStep += 1;
            break;

        case 1:
            if (BattleScene_RevealSubstituteUser(sActivePokemon[sBattleScene.scene->actingSide]) == 0) {
                Battle_ResetModelToIdleAnim(sActingPokemon);
                Audio_FadeAndClearStoredSounds();
                BattleAnim_CleanupEffects(1);
                BattleScene_SetCameraOwnerStandardShot(camera, sActivePokemon[sBattleScene.scene->actingSide]);
                BattleScene_EnterStateRow(sActivePokemon[sBattleScene.scene->actingSide], 0xD);

                if (acting_runtime->unk_15 & 0x18) {
                    BattleBox_Open(4);
                    acting_runtime->displayHp = acting_runtime->unk_0E;
                    other_runtime->displayHp = other_runtime->unk_0E;
                    if (acting_runtime->unk_15 & 0x10) {
                        BattleAnim_QueueEffectList(6, sActingPokemon);
                    } else {
                        BattleAnim_QueueEffectList(5, sActingPokemon);
                    }
                    sBattleScene.scene->phaseStep += 1;
                } else {
                    BattleBox_Open(5);
                    acting_runtime->displayHp = acting_runtime->unk_10;
                    other_runtime->displayHp = other_runtime->unk_10;
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
                    BattleScene_ResetPhaseSubstep();
                    BattleScene_SetPhase(camera, BATTLE_PHASE_FAINT);
                    sBattleScene.scene->actingSide = !sBattleScene.scene->actingSide;
                } else if (acting_runtime->unk_4D & 0x80) {
                    Audio_PlayCommand(0x22, 0, 0);
                    acting_runtime->displayHp = acting_runtime->unk_10;
                    other_runtime->displayHp = other_runtime->unk_10;
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
            if (BattleScene_BreakSubstitute(sNonActingPokemon) == 0) {
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 5:
            BattleScene_ResetModelReloadStep();
            BattleScene_SetCameraOwnerStandardShot(camera, sActivePokemon[sBattleScene.scene->actingSide]);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[!sBattleScene.scene->actingSide]);
            BattleScene_ResolveDisplayedHpCompletion(camera);
            break;
    }
}

/**
 * Phase 33. Shows residual damage on the acting Pokemon only (the other HP bar is treated as settled). If
 * it reaches 0 HP, flips the acting side and enters BattleScene_PhaseFaintAfterResidualDamage; otherwise
 * continues with BattleScene_CleanupActionAndAdvance.
 */
void BattleScene_PhaseResidualDamageFaintCheck(CameraNode* camera) {
    BattleMonRuntime* acting_runtime = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;
    BattleMonRuntime* other_runtime = &sActivePokemon[!sBattleScene.scene->actingSide]->state.runtime;

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
            acting_runtime->displayHp = acting_runtime->unk_0E;
            other_runtime->displayHp = other_runtime->unk_0E;
            sBattleScene.scene->phaseStep += 1;
            break;

        case 2:
            BattleScene_CameraShotTargetClose(camera, sActivePokemon[sBattleScene.scene->actingSide]);
            sSide1HpConsistent = BattleScene_AnimateDisplayedHpToRuntime(sActivePokemon[sBattleScene.scene->actingSide]);
            sSide2HpConsistent = 1;

            if ((sSide1HpConsistent == 1) && (sSide2HpConsistent == 1) && (BattleScene_AdvanceCameraFrameCounterUntil(0x32) != 0)) {
                BattleScene_ResetCameraFrameCounter();
                if (Battle_GetActiveMonHp(sActivePokemon[sBattleScene.scene->actingSide]) == 0) {
                    BattleScene_ResetPhaseSubstep();
                    BattleScene_SetPhase(camera, BATTLE_PHASE_RESIDUAL_FAINT);
                    sBattleScene.scene->actingSide = !sBattleScene.scene->actingSide;
                } else {
                    BattleScene_CleanupActionAndAdvance();
                }
            }
            break;
    }
}

void BattleScene_ClearMonStatusAnimations(ActivePokemon* pokemon) {
    BattleAnim_ClearFreezeStatusAnimation(pokemon);
    BattleAnim_ClearSleepStatusAnimation(pokemon);
}

/**
 * Phase 11. Voluntary switch (REPLACEMENT_VOLUNTARY_SWITCH, selected by BattleScene_PrepareReplacementTransition):
 * withdraws the acting Pokemon (row 2), loads the chosen team member's model, and sends it out.
 */
void BattleScene_PhaseVoluntarySwitch(CameraNode* camera) {
    ActivePokemonState* acting_state = &sActivePokemon[sBattleScene.scene->actingSide]->state;
    ActivePokemon* acting_pokemon = sActivePokemon[sBattleScene.scene->actingSide];
    BattleMonRuntime* acting_runtime = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;

    if (sBattleScene.scene->actingSide) {}

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            if (sBattleScene.scene->actionPass == BATTLE_ACTION_SECOND) {
                BattleScene_ResetCameraFov(camera);
                BattleScene_SelectCameraFollowPreset(acting_pokemon, camera, 4);
                PlayerDefeated(BattleScene_GetParticipantDisplayPlayerNumber(acting_pokemon));
                sBattleScene.scene->phaseStep += 1;
            } else if (BattleScene_AnimateCameraPreset(camera, acting_pokemon, 1, 1) != 0) {
                BattleBox_Open(0xA);
                PlayerDefeated(BattleScene_GetParticipantDisplayPlayerNumber(acting_pokemon));
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 1:
            BattleScene_ApproachCameraZoom(camera, acting_pokemon);
            BattleScene_EnterStateRow(acting_pokemon, 2);
            BattleScene_ClearMonStatusAnimations(acting_pokemon);
            sBattleScene.scene->phaseStep += 1;
            break;

        case 2:
            BattleScene_ApproachCameraZoom(camera, acting_pokemon);
            if (BattleScene_AdvanceCameraFrameCounter(0x69) != 0) {
                Vec3f_SetComponentsDuplicate(&camera->view.targetPosition, acting_pokemon->facingDirection * -150.0f, 0.0, 0.0f);
                if (sBattleScene.scene && sBattleScene.scene) {}
                Vec3f_SetComponentsDuplicate(&camera->view.eyePosition, acting_pokemon->facingDirection * -50.0f, 5.0f, 0.0f);
                BattleScene_SetParticipantModelTransform(acting_pokemon, 2, 0);
                BattleAnim_QueueEffectList3(0xD, acting_pokemon, acting_pokemon, 0xFF, 0xFF);
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 3:
            if (acting_pokemon->pokeBall.position.y == 2.5f) {
                acting_pokemon->pokeBall.node.renderFlags |= 0x20;
            }

            if (ModelAnim_IsAnimationDone(&acting_pokemon->pokeBall) == 1) {
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 4:
            acting_pokemon->pokeBall.node.flags &= ~1;
            acting_pokemon->sessionTeams->unk_04 = &acting_pokemon->sessionTeams->teamMember[acting_state->ownerTeamMemberIndex]->pokemonPicked[acting_state->actionChosen];
            acting_pokemon->sessionTeams->uniqueStatusFlags |= 0x80;
            sBattleScene.scene->phaseStep += 1;
            break;

        case 5:
            if (acting_pokemon->sessionTeams->uniqueStatusFlags & 0x10) {
                acting_pokemon->sessionTeams->uniqueStatusFlags &= ~0x90;
                Battle_LoadOwnerModelForSwitchIn(acting_pokemon);
                BattleThumbnail_TickOwnerCameraFraming(acting_pokemon);
                sCurrentPokemonHp[sBattleScene.scene->actingSide] = acting_pokemon->state.runtime.displayHp;
                BattleAnim_QueueEffectList3(3, acting_pokemon, acting_pokemon, 0xFF, 0xFF);
                Audio_PlayCommand(2, 0, 0);
                BattleScene_RebindParticipantTeamMember(acting_pokemon, acting_state->ownerTeamMemberIndex);
                camera->perspective.verticalFieldOfView = 40.0f;
                BattleBox_Open(0xC);
                Battle_TriggerTrainerPresentationCue(acting_pokemon);
                sBattleDamage = 0;
                sBattleScene.scene->phaseStep += 1;
            }
            break;

        case 6:
            BattleScene_EnterStateRow(acting_pokemon, 0x12);
            BattleScene_SelectCameraFollowPreset(acting_pokemon, camera, 3);
            BattleScene_ResetCameraFrameCounter();
            sBattleScene.scene->phaseStep += 1;
            break;

        case 7:
            BattleScene_ReframeAndRecomputeDistanceForSwitch(camera, acting_pokemon);
            if (BattleScene_IsRowWaitCompleteWithEventCount(acting_pokemon, 0x3C) != 0) {
                Audio_FadeAndClearStoredSounds();
                BattleAnim_CleanupEffects(2);
                BattleScene_ResetCameraFrameCounter();
                Vec3f_SetComponentsDuplicate(&acting_pokemon->displayObject.scale, 1.0f, 1.0f, 1.0f);
                if (acting_runtime->usedMove == 0xFE) {
                    acting_runtime->usedMove = 0xA5;
                }
                BattleScene_CleanupActionAndAdvance();
                acting_state->flags &= ~0x1800;
                BattleBox_Close();
                sBattleScene.scene->unk_54 = !sBattleScene.scene->actingSide + 1;
            }
            break;
    }
}

/**
 * Phase 18. Replacement after a faint (REPLACEMENT_INVOLUNTARY_SWITCH, selected by
 * BattleScene_PrepareReplacementTransition): loads the chosen team member's model, opens its Poke Ball, and
 * sends it out. Unlike BattleScene_PhaseVoluntarySwitch, there is no withdraw step.
 */
void BattleScene_PhaseSendOutAfterFaint(CameraNode* camera) {
    ActivePokemonState* acting_pokemon_state = &sActivePokemon[sBattleScene.scene->actingSide]->state;
    ActivePokemonState* other_pokemon_state = &sActivePokemon[!sBattleScene.scene->actingSide]->state;
    UNUSED s32 pad;
    ActivePokemon* acting_pokemon = sActivePokemon[sBattleScene.scene->actingSide];
    BattleMonRuntime* acting_pokemon_runtime = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;

    switch (sBattleScene.scene->phaseStep) {
        case 0:
            if (sBattleScene.scene->actionPass == BATTLE_ACTION_SECOND) {
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
                if (other_pokemon_state->replacementState != REPLACEMENT_INVOLUNTARY_SWITCH) {
                    sBattleScene.scene->actionPass = BATTLE_ACTION_COMPLETE;
                }
                sBattleScene.scene->unk_2E = 2;
                BattleScene_CleanupActionAndAdvance();
                acting_pokemon_state->flags &= 0xE7FF;
                sBattleScene.scene->unk_54 = !sBattleScene.scene->actingSide + 1;
            }
            break;
    }
}

/**
 * Runs the per-frame battle presentation phase selected by scene->phase and phaseStep. Move-specific phases
 * (7, 12-15, 17, 25, 27-31, 38) are entered through BattleScene_PhaseDispatchMoveEffect.
 */
void BattleScene_UpdatePhase(CameraNode* camera) {
    BattleScene_UpdateActiveOwnerPointers();

    switch (sBattleScene.scene->phase) {
        case BATTLE_PHASE_INITIALIZE:
            BattleScene_InitializeBattlePresentation(camera);
            BattleScene_SetPhase(camera, BATTLE_PHASE_TURN_INTRO);

        case BATTLE_PHASE_TURN_INTRO:
            // battle orders finished, begin turn
            BattleScene_PhaseTurnIntroCamera(camera);
            break;
        case BATTLE_PHASE_FIRST_ACTION:
            // first pokemon move
            BattleScene_PhaseFirstAttackerAction(camera);
            break;
        case BATTLE_PHASE_SECOND_ACTION:
            // second pokemon move
            BattleScene_PhaseSecondAttackerAction(camera);
            break;
        case BATTLE_PHASE_END_TURN:
            // end turn
            BattleScene_PhaseEndTurn(camera);
            break;
        case BATTLE_PHASE_SELF_TARGET:
            // Double Team, Minimize, Rest; moveEffectKind 7, 21, 39
            BattleScene_PhaseSelfTargetMove(camera);
            break;
        case BATTLE_PHASE_WITHDRAW:
            // withdraw
            BattleScene_PhaseWithdrawMove(camera);
            break;
        case BATTLE_PHASE_BIDE:
            // bide
            BattleScene_PhaseBide(camera);
            break;
        case BATTLE_PHASE_EXPLOSION:
            // explosion and self-destruct
            BattleScene_PhaseExplosion(camera);
            break;
        case BATTLE_PHASE_HAZE:
            // haze
            BattleScene_PhaseHaze(camera);
            break;
        case BATTLE_PHASE_NO_ACTION:
            // flinch
            // also healing moves when used at full hp
            BattleScene_PhaseNoAction(camera);
            break;
        case BATTLE_PHASE_SUBSTITUTE:
            // substitute creation
            BattleScene_PhaseSubstitute(camera);
            break;
        case BATTLE_PHASE_FLY_START:
            // fly
            BattleScene_PhaseFly(camera);
            break;
        case BATTLE_PHASE_DIG_START:
            // dig
            BattleScene_PhaseDig(camera);
            break;
        case BATTLE_PHASE_CHARGE:
            // 2 turn charge moves
            BattleScene_PhaseTwoTurnCharge(camera);
            break;
        case BATTLE_PHASE_TRANSFORM:
            // Transform move
            BattleScene_PhaseTransform(camera);
            break;
        case BATTLE_PHASE_UNABLE_TO_ACT:
            // sleeping or immobilized
            BattleScene_PhaseSleepOrImmobilized(camera);
            break;
        case BATTLE_PHASE_FULL_PARALYSIS:
            // full paralysis
            BattleScene_PhaseFullParalysis(camera);
            break;
        case BATTLE_PHASE_CONFUSION_SELF_HIT:
            // self-damage (matches confusion)
            BattleScene_PhaseConfusionSelfHit(camera);
            break;
        case BATTLE_PHASE_MOVE_FAILED:
            // move failed
            BattleScene_PhaseMoveFailedMessage(camera);
            break;
        case BATTLE_PHASE_RAGE:
            // rage attack increase
            BattleScene_PhaseRage(camera);
            break;
        case BATTLE_PHASE_RESIDUAL_AFTER_MOVE:
            // poison/burn-style residual damage after a move
            BattleScene_PhaseResidualDamageAfterMove(camera);
            break;
        case BATTLE_PHASE_RESIDUAL_FAINT_CHECK:
            // residual damage on the acting Pokemon only; faint goes to phase 34
            BattleScene_PhaseResidualDamageFaintCheck(camera);
            break;
        case BATTLE_PHASE_RESIDUAL_WITHOUT_MOVE:
            // residual damage for a Pokemon with no move this turn
            BattleScene_PhaseResidualDamageWithoutMove(camera);
            break;
        case BATTLE_PHASE_RECOIL:
            // the acting Pokemon takes damage from its own move (recoil)
            BattleScene_PhaseRecoilDamage(camera);
            break;
        case BATTLE_PHASE_JUMP_KICK_CRASH:
            // hi jump kick crash
            BattleScene_PhaseJumpKickCrash(camera);
            break;
        case BATTLE_PHASE_VOLUNTARY_SWITCH:
            // voluntary switch (withdraw, then send out)
            BattleScene_PhaseVoluntarySwitch(camera);
            break;
        case BATTLE_PHASE_SEND_OUT_AFTER_FAINT:
            // send out the replacement after a faint
            BattleScene_PhaseSendOutAfterFaint(camera);
            break;
        case BATTLE_PHASE_FAINT:
            // faint presentation; also the winner camera when the last Pokemon faints
            BattleScene_PhaseFaint(camera);
            break;
        case BATTLE_PHASE_RESIDUAL_FAINT:
            // faint after residual damage
            BattleScene_PhaseFaintAfterResidualDamage(camera);
            break;
        case BATTLE_PHASE_ENTER_QUEUED:
            // wait for the owner animation, then enter the phase queued in nextPhase
            BattleScene_PhaseEnterQueuedPhase();
            break;
        case BATTLE_PHASE_DISPATCH_MOVE_EFFECT:
            // hold the action camera, then switch to the phase for the move effect kind
            BattleScene_PhaseDispatchMoveEffect();
            break;
    }
}
