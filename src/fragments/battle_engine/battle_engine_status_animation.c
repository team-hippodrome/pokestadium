#include "battle_engine.h"
#include "status.h"
#include "src/model_animation.h"
#include "src/model_animation_events.h"
#include "src/model_renderer.h"

void BattleAnim_StartFreezeStatusAnimation(ActivePokemon* active_pokemon) {
    UNUSED s32 pad[2];
    BattleMon* party_pokemon;
    ActivePokemonState* pokemon_state;
    BattleMonRuntime* runtime;
    MovePresentationCue* hit_cue;
    s32 side_index = BattleScene_GetParticipantSideIndex(active_pokemon);

    runtime = &active_pokemon->state.runtime;
    pokemon_state = &active_pokemon->state;
    party_pokemon = &active_pokemon->teamRoster->pokemonPicked[pokemon_state->rosterIndex];
    hit_cue = &sSideBattleCues[side_index]->hitCue;

    if ((party_pokemon->statusCondition & STATUS_FREEZE) && (pokemon_state->flags & ACTIVE_FREEZE)) {
        active_pokemon->displayObject.node.renderFlags &= ~0x20;
    }

    if ((runtime->unk_15 & STATUS_FREEZE) && (party_pokemon->statusCondition & STATUS_FREEZE) && !(pokemon_state->flags & ACTIVE_FREEZE) &&
        ((active_pokemon->displayObject.transformAnim.frameFixed >> 0x10) >= 3)) {
        ModelAnim_SetAnimation(&active_pokemon->displayObject, hit_cue->id);
        ModelAnim_SetFrame(&active_pokemon->displayObject, 4);
        ModelAnim_SetEventTrack(&active_pokemon->displayObject, hit_cue->unk_01);
        ModelAnim_SetEventFrame(&active_pokemon->displayObject, 3);
        active_pokemon->displayObject.node.renderFlags &= ~0x20;
        Battle_SetRuntimeFlags(active_pokemon, ACTIVE_FREEZE);
        BattleAnim_QueueEffectList(1, active_pokemon);
        BattleAnim_PlayBattleSequenceById(active_pokemon, 0x13);
    }
}

void BattleAnim_RestoreBaseModelAnimation(ActivePokemon* pokemon) {
    MovePresentationCue* idle_cue = &sSideBattleCues[BattleScene_GetParticipantSideIndex(pokemon)]->idleCues;

    ModelAnim_SetAnimation(&pokemon->displayObject, idle_cue->id);
}

void BattleAnim_ClearFreezeStatusAnimation(ActivePokemon* pokemon) {
    if (pokemon->state.flags & ACTIVE_FREEZE) {
        BattleAnim_StopOwnerCategoryParticles(1, pokemon);
        pokemon->displayObject.node.renderFlags |= 0x20;
        Battle_ClearRuntimeFlags(pokemon, ACTIVE_FREEZE);
        BattleAnim_RestoreBaseModelAnimation(pokemon);
    }
}

void BattleAnim_ResetFreezeStatusAndRestoreAnimation(ActivePokemon* pokemon) {
    BattleMon* party_pokemon = &pokemon->teamRoster->pokemonPicked[pokemon->state.rosterIndex];

    if (pokemon->state.flags & ACTIVE_FREEZE) {
        party_pokemon->statusCondition = 0;
        BattleAnim_StopOwnerCategoryParticles(1, pokemon);
        pokemon->displayObject.node.renderFlags |= 0x20;
        Battle_ClearRuntimeFlags(pokemon, ACTIVE_FREEZE);
        BattleAnim_RestoreBaseModelAnimation(pokemon);
    }
}

void BattleAnim_ClearFreezeStatusAnimationIfThawed(ActivePokemon* pokemon) {
    if ((pokemon->state.flags & ACTIVE_FREEZE) && !(pokemon->state.runtime.unk_15 & STATUS_FREEZE)) {
        BattleAnim_StopOwnerCategoryParticles(1, pokemon);
        pokemon->displayObject.node.renderFlags |= 0x20;
        Battle_ClearRuntimeFlags(pokemon, ACTIVE_FREEZE);
        BattleAnim_RestoreBaseModelAnimation(pokemon);
    }
}

void BattleAnim_UpdateSleepStatusAnimation(ActivePokemon* active_pokemon) {
    ActivePokemonState* pokemon_state = &active_pokemon->state;
    BattleMonRuntime* runtime = &pokemon_state->runtime;
    BattleMon* party_pokemon = &active_pokemon->teamRoster->pokemonPicked[active_pokemon->state.rosterIndex];

    if ((sBattleScene.scene->phase != BATTLE_PHASE_VOLUNTARY_SWITCH) || (sBattleScene.scene->phaseStep >= 6)) {
        if ((party_pokemon->statusCondition & STATUS_SLEEP_MASK) && (runtime->unk_15 & STATUS_SLEEP_MASK) && !(active_pokemon->state.flags & ACTIVE_SLEEP)) {
            BattleAnim_SetModelEventTrackAndState(active_pokemon, BATTLE_CUE_SLEEP_IDLE, 2);
            BattleAnim_QueueEffectList(3, active_pokemon);
            BattleAnim_PlayBattleSequenceById(active_pokemon, 0x32);
            Battle_SetRuntimeFlags(active_pokemon, ACTIVE_SLEEP);
        }

        if (!(party_pokemon->statusCondition & STATUS_SLEEP_MASK) && (pokemon_state->flags & ACTIVE_SLEEP)) {
            if (sSideBattleCues[BattleScene_GetParticipantSideIndex(active_pokemon)]->wakeUpEventTrack == 0xFF) {
                pokemon_state->unk_36 = 0;
                BattleAnim_SetModelEventTrackFromRow(active_pokemon, BATTLE_CUE_IDLE);
            } else {
                BattleAnim_SetModelEventTrackAndState(active_pokemon, 0xB8, 1);
            }
            BattleAnim_StopOwnerCategoryParticles(3, active_pokemon);
            Battle_ClearRuntimeFlags(active_pokemon, ACTIVE_SLEEP);
        }
    }
}

void BattleAnim_EndSleepStatusAnimation(ActivePokemon* pokemon) {
    BattleMon* party_pokemon = &pokemon->teamRoster->pokemonPicked[pokemon->state.rosterIndex];

    if (pokemon->state.flags & ACTIVE_SLEEP) {
        party_pokemon->statusCondition = 0;
        if (sSideBattleCues[BattleScene_GetParticipantSideIndex(pokemon)]->wakeUpEventTrack == 0xFF) {
            pokemon->state.unk_36 = 0;
            BattleAnim_SetModelEventTrackFromRow(pokemon, BATTLE_CUE_IDLE);
        } else {
            BattleAnim_SetModelEventTrackAndState(pokemon, 0xB8, 1);
        }
        BattleAnim_StopOwnerCategoryParticles(3, pokemon);
        Battle_ClearRuntimeFlags(pokemon, ACTIVE_SLEEP);
    }
}

void BattleAnim_ClearSleepStatusAnimation(ActivePokemon* pokemon) {
    if (pokemon->state.flags & ACTIVE_SLEEP) {
        BattleAnim_StopOwnerCategoryParticles(3, pokemon);
        Battle_ClearRuntimeFlags(pokemon, ACTIVE_SLEEP);
    }
}
