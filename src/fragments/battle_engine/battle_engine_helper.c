#include "battle_engine.h"
#include "src/model_animation.h"
#include "src/model_animation_events.h"
#include "src/model_renderer.h"
#include "src/text_system.h"
#include "src/matrix.h"
#include "pokemon.h"
#include "status.h"

s32 BattleScene_ByteArrayContains(u8 lookup, u8* array, s32 array_length) {
    while (array_length--) {
        if (lookup == *array) {
            return 1;
        }
        array++;
    }

    return 0;
}

BattleMon* BattleScene_GetParticipantSessionMon(ActivePokemon* active_pokemon) {
    BattleMon* pokemon = &active_pokemon->sessionTeams->teamMember[active_pokemon->state.ownerTeamMemberIndex]->pokemonPicked[active_pokemon->state.rosterIndex];

    return pokemon;
}

s32 BattleScene_IsPartnerPikachu(ActivePokemon* active_pokemon) {
    BattleMon* pokemon = BattleScene_GetParticipantSessionMon(active_pokemon);

    if ((active_pokemon->displayObject.objectType == PIKACHU) && (pokemon->unk_52 & 0x80)) {
        return 1;
    }
    return 0;
}

f32 BattleScene_GetParticipantModelRestingHeight(ActivePokemon* active_pokemon) {
    return sPokemonModelDimensions[BattleScene_GetParticipantSideIndex(active_pokemon)].restingHeight;
}

f32 BattleScene_GetParticipantModelWidth(ActivePokemon* active_pokemon) {
    return sPokemonModelDimensions[BattleScene_GetParticipantSideIndex(active_pokemon)].width;
}

f32 BattleScene_GetParticipantModelHeight(ActivePokemon* active_pokemon) {
    return sPokemonModelDimensions[BattleScene_GetParticipantSideIndex(active_pokemon)].height;
}

f32 BattleScene_GetParticipantCameraTargetOffsetY(ActivePokemon* active_pokemon) {
    return sPokemonModelDimensions[BattleScene_GetParticipantSideIndex(active_pokemon)].cameraTargetOffsetY;
}

void BattleScene_PositionParticipantModel(ActivePokemon* active_pokemon) {
    if (BattleScene_GetParticipantSideIndex(active_pokemon) == 0) {
        if (active_pokemon->displayObject.objectType == GYARADOS) {
            Vec3f_SetComponentsDuplicate(&active_pokemon->displayObject.position, -225.0f, BattleScene_GetParticipantModelRestingHeight(active_pokemon), 0.0f);
        } else if ((active_pokemon->displayObject.objectType == VENUSAUR) || (active_pokemon->displayObject.objectType == LAPRAS)) {
            Vec3f_SetComponentsDuplicate(&active_pokemon->displayObject.position, -175.0f, BattleScene_GetParticipantModelRestingHeight(active_pokemon), 0.0f);
        } else if (active_pokemon->displayObject.objectType == ONIX) {
            Vec3f_SetComponentsDuplicate(&active_pokemon->displayObject.position, -225.0f, BattleScene_GetParticipantModelRestingHeight(active_pokemon), 7.5000005f);
        } else {
            Vec3f_SetComponentsDuplicate(&active_pokemon->displayObject.position, -150.0f, BattleScene_GetParticipantModelRestingHeight(active_pokemon), 0.0f);
        }
    } else if ((active_pokemon->displayObject.objectType == GYARADOS) || (active_pokemon->displayObject.objectType == ONIX)) {
        Vec3f_SetComponentsDuplicate(&active_pokemon->displayObject.position, 225.0f, BattleScene_GetParticipantModelRestingHeight(active_pokemon), 0.0f);
    } else if ((active_pokemon->displayObject.objectType == VENUSAUR) || (active_pokemon->displayObject.objectType == LAPRAS)) {
        Vec3f_SetComponentsDuplicate(&active_pokemon->displayObject.position, 175.0f, BattleScene_GetParticipantModelRestingHeight(active_pokemon), 0.0f);
    } else {
        Vec3f_SetComponentsDuplicate(&active_pokemon->displayObject.position, 150.0f, BattleScene_GetParticipantModelRestingHeight(active_pokemon), 0.0f);
    }
}

f32 BattleScene_GetParticipantModelXOffset(ActivePokemon* active_pokemon) {
    if (BattleScene_GetParticipantSideIndex(active_pokemon) == 0) {
        if ((active_pokemon->displayObject.objectType == GYARADOS) || (active_pokemon->displayObject.objectType == ONIX)) {
            return -200.0f;
        }

        if ((active_pokemon->displayObject.objectType == VENUSAUR) || (active_pokemon->displayObject.objectType == LAPRAS)) {
            return -175.0f;
        }
        return -150.0f;
    }

    if ((active_pokemon->displayObject.objectType == GYARADOS) || (active_pokemon->displayObject.objectType == ONIX)) {
        return 200.0f;
    }

    if ((active_pokemon->displayObject.objectType == VENUSAUR) || (active_pokemon->displayObject.objectType == LAPRAS)) {
        return 175.0f;
    }

    return 150.0f;
}

void BattleScene_SetParticipantModelScale(ActivePokemon* active_pokemon, f32 scale) {
    active_pokemon->displayObject.scale.x = scale;
    active_pokemon->displayObject.scale.y = scale;
    active_pokemon->displayObject.scale.z = scale;
}

s32 BattleScene_GetTeamMemberTeamSize(ActivePokemon* active_pokemon) {
    return active_pokemon->sessionTeams->teamMember[active_pokemon->state.teamMemberIndex]->numberOfPokemonBrought;
}

s32 BattleScene_GetPartnerTeamSize(ActivePokemon* active_pokemon) {
    s32 idx = active_pokemon->state.teamMemberIndex == 0;

    return active_pokemon->sessionTeams->teamMember[idx]->numberOfPokemonBrought;
}

s32 BattleScene_GetParticipantSideIndex(ActivePokemon* active_pokemon) {
    if (active_pokemon == sSide1ActivePokemon) {
        return 0;
    }
    return 1;
}

u8 BattleScene_GetMoveCueDefaultAnchor(ActivePokemon* active_pokemon) {
    UNUSED s32 pad;
    UNUSED s16 pad2;
    s16 move_index = active_pokemon->state.runtime.usedMove - 1;
    MovePresentationCue* move_cue = &sSideBattleCues[BattleScene_GetParticipantSideIndex(active_pokemon)]->moveCues[move_index];

    return move_cue->defaultAnchor;
}

u8 BattleScene_GetMoveCueAlternateAnchor(ActivePokemon* active_pokemon) {
    UNUSED s32 pad;
    UNUSED s16 pad2;
    s16 move_index = active_pokemon->state.runtime.usedMove - 1;
    MovePresentationCue* move_cue = &sSideBattleCues[BattleScene_GetParticipantSideIndex(active_pokemon)]->moveCues[move_index];

    return move_cue->alternateAnchor;
}

s16 BattleScene_GetHitCueDefaultAnchor(ActivePokemon* active_pokemon) {
    MovePresentationCue* hit_cue = &sSideBattleCues[BattleScene_GetParticipantSideIndex(active_pokemon)]->hitCue;

    return hit_cue->defaultAnchor;
}

s32 BattleScene_GetHitCueAlternateAnchor(ActivePokemon* active_pokemon) {
    MovePresentationCue* hit_cue = &sSideBattleCues[BattleScene_GetParticipantSideIndex(active_pokemon)]->hitCue;

    return hit_cue->alternateAnchor;
}

f32 BattleScene_GetParticipantModelBoundsHeight(ActivePokemon* active_pokemon) {
    return ((BattleScene_GetParticipantModelHeight(active_pokemon) * 2) / 50.0f) * active_pokemon->displayObject.scale.y;
}

f32 BattleScene_GetParticipantUnscaledBoundsHeight(ActivePokemon* active_pokemon) {
    return (BattleScene_GetParticipantModelHeight(active_pokemon) * 2) / 50.0f;
}

s16 BattleScene_GetParticipantUsedMove(ActivePokemon* active_pokemon) {
    return active_pokemon->state.runtime.usedMove;
}

void BattleScene_GetParticipantModelBoundsScaled(ActivePokemon* pokemon, Vec3f* dest) {
    dest->x = ((BattleScene_GetParticipantModelWidth(pokemon) * 2) / 50.0f) * pokemon->displayObject.scale.x;
    dest->y = ((BattleScene_GetParticipantModelHeight(pokemon) * 2) / 50.0f) * pokemon->displayObject.scale.y;
    dest->z = ((BattleScene_GetParticipantModelWidth(pokemon) * 2) / 50.0f) * pokemon->displayObject.scale.x;
}

void BattleScene_GetParticipantModelRotation(ActivePokemon* pokemon, Vec3s* dest) {
    dest->x = pokemon->displayObject.rotation.x;
    dest->y = pokemon->displayObject.rotation.y;
    dest->z = pokemon->displayObject.rotation.z;
}

u16 Battle_GetActiveMonHp(ActivePokemon* active_pokemon) {
    BattleMon* party_pokemon = &active_pokemon->teamRoster->pokemonPicked[active_pokemon->state.rosterIndex];

    return party_pokemon->hp;
}

s16 BattleScene_GetParticipantFacingAngle(ActivePokemon* active_pokemon) {
    return active_pokemon->facingDirection << 0xE;
}

s32 BattleAnim_GetSpeciesBySideIndex(s32 side) {
    return sActivePokemon[side]->displayObject.objectType;
}

s32 BattleAnim_GetOwnerSpecies(ActivePokemon* active_pokemon) {
    return active_pokemon->displayObject.objectType;
}

u8 BattleScene_IsParticipantModelVisible(ActivePokemon* active_pokemon) {
    s32 visible = active_pokemon->displayObject.node.flags & 1;
    return visible;
}

void BattleScene_GetParticipantModelPosition(ActivePokemon* pokemon, Vec3f* dest) {
    dest->x = pokemon->displayObject.position.x;
    dest->y = pokemon->displayObject.position.y;
    dest->z = pokemon->displayObject.position.z;
}

void BattleAnim_GetOwnerDefaultAnchorPosition(ActivePokemon* pokemon, Vec3f* anchor_position) {
    f32 y_offset = 0.0f;

    //guess: these are pokemon where their wins are higher than their head in their idle animation
    if (BattleScene_ByteArrayContains(pokemon->displayObject.objectType, &sAerodactyl, 1) != 0) {
        y_offset = BattleAnim_GetOwnerModelHeight(pokemon) / 1.5f;
    } else if (BattleScene_ByteArrayContains(pokemon->displayObject.objectType, &sFearow, 1) != 0) {
        y_offset = BattleAnim_GetOwnerModelHeight(pokemon) * 0.5f;
    } else if ((BattleScene_ByteArrayContains(pokemon->displayObject.objectType, &sFarfetchD, 1) != 0) && (pokemon->state.flags & ACTIVE_AIRBORNE)) {
        y_offset = 2.0f * BattleAnim_GetOwnerModelHeight(pokemon);
    } else if ((BattleScene_ByteArrayContains(pokemon->displayObject.objectType, sDoduoEvoLine, 2) != 0) && (pokemon->state.flags & ACTIVE_AIRBORNE)) {
        y_offset = BattleAnim_GetOwnerModelHeight(pokemon) * 1.5f;
    }

    if (BattleScene_GetParticipantModelRestingHeight(pokemon) != 0.0f) {
        anchor_position->x = pokemon->displayObject.position.x;
        anchor_position->y = pokemon->displayObject.position.y + y_offset;
        anchor_position->z = pokemon->displayObject.position.z;
    } else {
        anchor_position->x = pokemon->displayObject.position.x;
        anchor_position->y = BattleAnim_GetOwnerModelHeight(pokemon) + pokemon->displayObject.position.y + y_offset;
        anchor_position->z = pokemon->displayObject.position.z;
    }

    if (anchor_position->y < 0.0f) {
        anchor_position->y = BattleAnim_GetOwnerModelHeight(pokemon);
    }
}

void BattleScene_GetParticipantRestingPosition(ActivePokemon* pokemon, Vec3f* dest) {
    if (BattleScene_GetParticipantSideIndex(pokemon) == 0) {
        if (pokemon->displayObject.objectType == GYARADOS) {
            Vec3f_SetComponentsDuplicate(dest, -225.0f, BattleScene_GetParticipantModelRestingHeight(pokemon), 0.0f);
        } else if ((pokemon->displayObject.objectType == VENUSAUR) || (pokemon->displayObject.objectType == LAPRAS)) {
            Vec3f_SetComponentsDuplicate(dest, -175.0f, BattleScene_GetParticipantModelRestingHeight(pokemon), 0.0f);
        } else if (pokemon->displayObject.objectType == ONIX) {
            Vec3f_SetComponentsDuplicate(dest, -225.0f, BattleScene_GetParticipantModelRestingHeight(pokemon), 7.5000005f);
        } else {
            Vec3f_SetComponentsDuplicate(dest, -150.0f, BattleScene_GetParticipantModelRestingHeight(pokemon), 0.0f);
        }
    } else if ((pokemon->displayObject.objectType == GYARADOS) || (pokemon->displayObject.objectType == ONIX)) {
        Vec3f_SetComponentsDuplicate(dest, 225.0f, BattleScene_GetParticipantModelRestingHeight(pokemon), 0.0f);
    } else if ((pokemon->displayObject.objectType == VENUSAUR) || (pokemon->displayObject.objectType == LAPRAS)) {
        Vec3f_SetComponentsDuplicate(dest, 175.0f, BattleScene_GetParticipantModelRestingHeight(pokemon), 0.0f);
    } else {
        Vec3f_SetComponentsDuplicate(dest, 150.0f, BattleScene_GetParticipantModelRestingHeight(pokemon), 0.0f);
    }
}

void BattleScene_GetParticipantPokeBallPosition(ActivePokemon* pokemon, Vec3f* dest) {
    dest->x = pokemon->pokeBall.position.x;
    dest->y = pokemon->pokeBall.position.y + 2.5f;
    dest->z = pokemon->pokeBall.position.z;
}

f32 BattleAnim_GetOwnerModelHeight(ActivePokemon* active_pokemon) {
    if (active_pokemon->modelTransformId == 0x10) {
        return BattleScene_GetParticipantModelHeight(active_pokemon);
    }
    return BattleScene_GetParticipantModelHeight(active_pokemon) * active_pokemon->displayObject.scale.y;
}

f32 BattleScene_GetParticipantModelRadiusScaled(ActivePokemon* active_pokemon) {
    return BattleScene_GetParticipantModelWidth(active_pokemon) * active_pokemon->displayObject.scale.y;
}

ModelNodeView* Battle_GetLoadedModelNode(ActivePokemon* active_pokemon, s32 node_index) {
    return active_pokemon->sessionTeams->loadedModel->unk_00[2]->unk_00[node_index];
}

s32 Battle_GetDisplayModelId(ActivePokemon* active_pokemon, UNUSED s32 unused) {
    return active_pokemon->sessionTeams->displayModel;
}

char* Battle_GetActiveMoveName(ActivePokemon* active_pokemon) {
    return Text_GetString(NULL, 0, sMoveNameTable, active_pokemon->state.runtime.usedMove - 1);
}

u8* Battle_GetActiveMonNickname(ActivePokemon* active_pokemon) {
    return active_pokemon->teamRoster->pokemonPicked[active_pokemon->state.rosterIndex].nickname;
}

void BattleScene_SetParticipantModelTransform(ActivePokemon* active_pokemon, s32 transform_id, s32 transform_timer) {
    active_pokemon->modelTransformId = transform_id;
    active_pokemon->modelTransformTimer = transform_timer;
}

void Battle_ReturnToIdleAnimIfFinished(ActivePokemon* active_pokemon, ActivePokemon* animated_pokemon) {
    s32 idx;
    MovePresentationCue* idle_cue;
    MovePresentationCue* flying_cue;
    MovePresentationCue* sleep_idle_cue;
    ActivePokemonState* pokemon_state = &active_pokemon->state;

    idx = BattleScene_GetParticipantSideIndex(active_pokemon);
    idle_cue = &sSideBattleCues[idx]->idleCues;
    idx = BattleScene_GetParticipantSideIndex(active_pokemon);
    flying_cue = &sSideBattleCues[idx]->flyingIdleCue;
    idx = BattleScene_GetParticipantSideIndex(active_pokemon);
    sleep_idle_cue = &sSideBattleCues[idx]->sleepIdleCue;

    if (!(pokemon_state->flags & (ACTIVE_UNDERGROUND | ACTIVE_FREEZE))) {
        if ((BattleAnim_IsSessionBusy(active_pokemon) == 0) && (animated_pokemon->displayObject.transformAnim.animationId != -1) &&
            (ModelAnim_IsFinished(&animated_pokemon->displayObject) == 1)) {
            if (pokemon_state->flags & ACTIVE_AIRBORNE) {
                ModelAnim_SetAnimation(&animated_pokemon->displayObject, flying_cue->id);
            } else if ((pokemon_state->flags & ACTIVE_SLEEP) && (BattleScene_ByteArrayContains(animated_pokemon->displayObject.objectType, sPokemonWithAlternateSleepIdle, 8) != 0)) {
                ModelAnim_SetAnimation(&animated_pokemon->displayObject, sleep_idle_cue->id);
            } else {
                if (animated_pokemon->displayObject.objectType == DITTO) {
                    idx = BattleAnim_RandomRange(5);
                    ModelAnim_SetAnimation(&animated_pokemon->displayObject, idx + 0xE);
                } else {
                    ModelAnim_SetAnimation(&animated_pokemon->displayObject, idle_cue->id);
                }

                if (!(pokemon_state->flags & ACTIVE_SLEEP)) {
                    BattleAnim_SetModelEventTrackAndState(active_pokemon, BATTLE_CUE_IDLE, 1);
                }
            }

            ModelAnim_SetFrame(&animated_pokemon->displayObject, 0);
        }
    }
}

void Battle_ResetModelToIdleAnim(ActivePokemon* active_pokemon) {
    s32 idx;
    ActivePokemonState* state;
    MovePresentationCue* idle_cue;
    MovePresentationCue* flying_idle_cue;
    MovePresentationCue* sleep_idle_cue;

    state = &active_pokemon->state;
    idx = BattleScene_GetParticipantSideIndex(active_pokemon);
    idle_cue = &sSideBattleCues[idx]->idleCues;
    idx = BattleScene_GetParticipantSideIndex(active_pokemon);
    flying_idle_cue = &sSideBattleCues[idx]->flyingIdleCue;
    idx = BattleScene_GetParticipantSideIndex(active_pokemon);
    sleep_idle_cue = &sSideBattleCues[idx]->sleepIdleCue;

    if (!(state->flags & (ACTIVE_UNDERGROUND | ACTIVE_FREEZE)) && (BattleAnim_IsSessionBusy(active_pokemon) == 0) && (active_pokemon->displayObject.transformAnim.animationId != -1)) {
        ModelAnim_SetFrame(&active_pokemon->displayObject, 0);
        if (state->flags & ACTIVE_AIRBORNE) {
            ModelAnim_SetAnimation(&active_pokemon->displayObject, flying_idle_cue->id);
        } else if ((state->flags & ACTIVE_SLEEP) && (BattleScene_ByteArrayContains(active_pokemon->displayObject.objectType, sPokemonWithAlternateSleepIdle, 8) != 0)) {
            ModelAnim_SetAnimation(&active_pokemon->displayObject, sleep_idle_cue->id);
        } else {
            if (active_pokemon->displayObject.objectType == DITTO) {
                ModelAnim_SetAnimation(&active_pokemon->displayObject, BattleAnim_RandomRange(5) + 14);
            } else {
                ModelAnim_SetAnimation(&active_pokemon->displayObject, idle_cue->id);
            }

            if (!(state->flags & ACTIVE_SLEEP)) {
                BattleAnim_SetModelEventTrackAndState(active_pokemon, BATTLE_CUE_IDLE, 1);
            }
        }
    }
}

void Battle_SetModelToHitAnim(ActivePokemon* active_pokemon) {
    MovePresentationCue* hit_cue = &sSideBattleCues[BattleScene_GetParticipantSideIndex(active_pokemon)]->hitCue;

    if (active_pokemon->displayObject.transformAnim.animationId != -1) {
        ModelAnim_SetFrame(&active_pokemon->displayObject, 0);
        ModelAnim_SetAnimation(&active_pokemon->displayObject, hit_cue->id);
    }
}

void BattleScene_BindParticipantTeamRoster(ActivePokemon* active_pokemon, s32 team_member_index) {
    active_pokemon->teamRoster = active_pokemon->sessionTeams->teamMember[team_member_index];
}

void BattleScene_SetParticipantTeamMember(ActivePokemon* active_pokemon, s32 team_member_index) {
    active_pokemon->state.teamMemberIndex = team_member_index;
}

void BattleScene_SetParticipantController(ActivePokemon* active_pokemon, Controller* controller) {
    active_pokemon->state.controller = controller;
}

void BattleScene_RebindParticipantTeamMember(ActivePokemon* pokemon, s32 team_member_index) {
    ActivePokemonState* state = &pokemon->state;

    BattleScene_BindParticipantTeamRoster(pokemon, team_member_index);
    BattleScene_SetParticipantTeamMember(pokemon, team_member_index);
    state->rosterIndex = state->actionChosen;
    pokemon->thumbnail.tagTeam = state->teamMemberIndex;
}

s32 Battle_FreezeClauseInEffect(ActivePokemon* active_pokemon) {
    s32 i;
    ActivePokemonState* state;
    BattleMon* pokemon;

    state = &active_pokemon->state;

    for (i = 0; i < BattleScene_GetTeamMemberTeamSize(active_pokemon); i++) {
        pokemon = &active_pokemon->teamRoster->pokemonPicked[i];
        if ((pokemon->statusCondition & STATUS_FREEZE) && (active_pokemon->state.runtime.roster[state->rosterIndex + state->teamMemberIndex * 3] == 0)) {
            return 1;
        }
    }

    if (active_pokemon->sessionTeams->tagTeamSize == 2) {
        for (i = 0; i < BattleScene_GetPartnerTeamSize(active_pokemon); i++) {
            pokemon = &active_pokemon->sessionTeams->teamMember[state->teamMemberIndex == 0]->pokemonPicked[i];
            if ((pokemon->statusCondition & STATUS_FREEZE) &&
                (active_pokemon->state.runtime.roster[state->rosterIndex + (state->teamMemberIndex == 0) * 3] == 0)) {
                return 1;
            }
        }
    }

    return 0;
}

s32 Battle_SleepClauseInEffect(ActivePokemon* active_pokemon) {
    s32 i;
    ActivePokemonState* state;
    BattleMon* pokemon;

    state = &active_pokemon->state;

    for (i = 0; i < BattleScene_GetTeamMemberTeamSize(active_pokemon); i++) {
        pokemon = &active_pokemon->teamRoster->pokemonPicked[i];
        if ((pokemon->statusCondition & STATUS_SLEEP_MASK) && (active_pokemon->state.runtime.roster[state->rosterIndex + state->teamMemberIndex * 3] == 0)) {
            return 1;
        }
    }

    if (active_pokemon->sessionTeams->tagTeamSize == 2) {
        for (i = 0; i < BattleScene_GetPartnerTeamSize(active_pokemon); i++) {
            pokemon = &active_pokemon->sessionTeams->teamMember[state->teamMemberIndex == 0]->pokemonPicked[i];
            if ((pokemon->statusCondition & STATUS_SLEEP_MASK) && (active_pokemon->state.runtime.roster[state->rosterIndex + (state->teamMemberIndex == 0) * 3] == 0)) {
                return 1;
            }
        }
    }
    return 0;
}

s32 BattleScene_GetCombinedTeamSize(ActivePokemon* active_pokemon) {
    if (active_pokemon->sessionTeams->tagTeamSize != 2) {
        return active_pokemon->sessionTeams->teamMember[0]->numberOfPokemonBrought;
    }
    return active_pokemon->sessionTeams->teamMember[1]->numberOfPokemonBrought + active_pokemon->sessionTeams->teamMember[0]->numberOfPokemonBrought;
}

u8 BattleScene_GetRosterTeamSize(TeamRoster* roster) {
    return roster->numberOfPokemonBrought;
}

s32 BattleScene_GetParticipantDisplayPlayerNumber(ActivePokemon* active_pokemon) {
    return active_pokemon->sessionTeams->teamMember[active_pokemon->thumbnail.tagTeam]->controllerPort + 1;
}

void BattleScene_ReinitParticipantBodyModel(ActivePokemon* owner, ActivePokemon* pokemon, s32 object_type, s32 mark_move_illegality) {
    UNUSED s32 pad;
    ModelNodeView* model_node = Battle_GetLoadedModelNode(owner, 0);

    BattleScene_GetParticipantSideIndex(owner);
    ModelRenderer_ClearDisplayObject(&pokemon->displayObject);

    if (owner == sActivePokemon[0]) {
        pokemon->displayObject.effectSlot = 0;
    } else {
        pokemon->displayObject.effectSlot = 1;
    }

    Model_InitDisplayObject(&pokemon->displayObject, 0, object_type, model_node);
    Battle_LoadAnimationAndMarkMoveIllegality(owner, mark_move_illegality);
    ModelAnim_SetFrame(&pokemon->displayObject, 0);
    ModelAnim_SetAnimation(&pokemon->displayObject, 0);

    if ((pokemon->displayObject.objectType == DIGLETT) || (pokemon->displayObject.objectType == DUGTRIO)) {
        pokemon->displayObject.node.renderFlags &= 0xFFBF;
    } else {
        pokemon->displayObject.node.renderFlags |= 0x40;
    }

    if (!(owner->state.flags & ACTIVE_AIRBORNE)) {
        if (owner == sActivePokemon[0]) {
            BattleScene_PositionParticipantModel(owner);
            pokemon->displayObject.rotation.y = 0x4000;
        } else {
            BattleScene_PositionParticipantModel(owner);
            pokemon->displayObject.rotation.y = -0x4000;
        }
    }
}

void BattleScene_ReinitParticipantThumbnailModel(ActivePokemon* pokemon, ThumbnailState* thumbnail, s16 object_type) {
    ModelNodeView* model_node;

    model_node = Battle_GetLoadedModelNode(pokemon, 0);
    ModelRenderer_ClearDisplayObject(&thumbnail->displayObject);

    if (pokemon == sActivePokemon[0]) {
        thumbnail->displayObject.effectSlot = 0xFF;
    } else {
        thumbnail->displayObject.effectSlot = 0xFF;
    }

    if (pokemon == sActivePokemon[0]) {
        Model_InitDisplayObject(&thumbnail->displayObject, 1, object_type, model_node);
    } else {
        Model_InitDisplayObject(&thumbnail->displayObject, 2, object_type, model_node);
    }

    ModelAnim_SetFrame(&thumbnail->displayObject, 0);
    ModelAnim_SetAnimation(&thumbnail->displayObject, 0);
    thumbnail->displayObject.node.renderFlags &= ~0x40;
}

void BattleScene_ReinitParticipantAfterimages(ActivePokemon* pokemon, s16 object_type) {
    s32 i;
    ModelNodeView* model_node;
    PokemonAfterimage* afterimage;

    model_node = Battle_GetLoadedModelNode(pokemon, 0);

    for (i = 0; i < 2; i++) {
        afterimage = &pokemon->afterimages[i];

        ModelRenderer_ClearDisplayObject(&afterimage->displayObject);
        if (pokemon == sActivePokemon[0]) {
            afterimage->displayObject.effectSlot = 0xFF;
        } else {
            afterimage->displayObject.effectSlot = 0xFF;
        }

        Model_InitDisplayObject(&afterimage->displayObject, 0, object_type, model_node);
        ModelAnim_SetFrame(&afterimage->displayObject, 0);
        ModelAnim_SetAnimation(&afterimage->displayObject, 0);

        afterimage->displayObject.node.renderFlags &= ~0x40;

        if (pokemon == sActivePokemon[0]) {
            Vec3f_SetComponentsDuplicate(&afterimage->displayObject.position, -150.0f, 0.0f, 0.0f);
            afterimage->displayObject.rotation.y = 0x4000;
        } else {
            Vec3f_SetComponentsDuplicate(&afterimage->displayObject.position, 150.0f, 0.0f, 0.0f);
            afterimage->displayObject.rotation.y = -0x4000;
        }

        afterimage->displayObject.node.flags &= ~1;
    }
}

void Battle_ResetEventTrack(ActivePokemon* active_pokemon) {
    if (active_pokemon->state.flags & ACTIVE_SLEEP) {
        BattleAnim_TrySetModelEventTrackFromRow(active_pokemon, 0xAF, 2);
    } else {
        active_pokemon->state.unk_36 = 0;
        ModelAnim_SetEventFrame(&active_pokemon->displayObject, 0);
        ModelAnim_SetEventTrack(&active_pokemon->displayObject, -1);
    }
}

void Battle_LoadOwnerModelForSpecies(ActivePokemon* active_pokemon) {
    ActivePokemonState* state = &active_pokemon->state;
    UNUSED s32 pad[3];
    s16 object_type;

    Battle_GetLoadedModelNode(active_pokemon, 0);
    object_type = Battle_GetDisplayModelId(active_pokemon, 0);
    BattleScene_ReinitParticipantBodyModel(active_pokemon, active_pokemon, object_type, 0);
    BattleScene_ReinitParticipantThumbnailModel(active_pokemon, &active_pokemon->thumbnail, object_type);
    BattleScene_ReinitParticipantAfterimages(active_pokemon, object_type);
    Battle_ResetEventTrack(active_pokemon);
    state->flags &= (ACTIVE_SUBSTITUTE | ACTIVE_SUB_BREAK);
    Vec3f_SetComponentsDuplicate(&active_pokemon->displayObject.scale, 1.0f, 1.0f, 1.0f);
}

void Battle_LoadOwnerModelForSwitchIn(ActivePokemon* active_pokemon) {
    ActivePokemonState* pokemon_state = &active_pokemon->state;
    UNUSED s32 pad[3];
    s16 model_id;

    Battle_GetLoadedModelNode(active_pokemon, 0);
    model_id = Battle_GetDisplayModelId(active_pokemon, 0);
    BattleScene_ReinitParticipantBodyModel(active_pokemon, active_pokemon, model_id, 1);
    BattleScene_ReinitParticipantThumbnailModel(active_pokemon, &active_pokemon->thumbnail, model_id);
    BattleScene_ReinitParticipantAfterimages(active_pokemon, model_id);
    Battle_ResetEventTrack(active_pokemon);
    pokemon_state->replacementState = 0;
    pokemon_state->flags &= (ACTIVE_SUBSTITUTE | ACTIVE_SUB_BREAK);
    Vec3f_SetComponentsDuplicate(&active_pokemon->displayObject.scale, 1.0f, 1.0f, 1.0f);
}

void Battle_ResetOwnerModelToSubstituteDoll(ActivePokemon* active_pokemon) {
    Model_SetMaterialColor(&active_pokemon->displayObject, 0xFF, 0xFF, 0xFF, active_pokemon->state.alpha = 0xFF);
    active_pokemon->state.model = active_pokemon->displayObject.objectType;
    BattleScene_ReinitParticipantBodyModel(active_pokemon, active_pokemon, SUBSTITUTE_DOLL, 0);
    BattleScene_ReinitParticipantAfterimages(active_pokemon, SUBSTITUTE_DOLL);
    Battle_ResetEventTrack(active_pokemon);
}

void Battle_ReloadSubstituteDoll(ActivePokemon* active_pokemon) {
    BattleScene_ReinitParticipantBodyModel(active_pokemon, active_pokemon, SUBSTITUTE_DOLL, 0);
    BattleScene_ReinitParticipantAfterimages(active_pokemon, SUBSTITUTE_DOLL);
    Battle_ResetEventTrack(active_pokemon);
}

void Battle_ReloadDisplayModel(ActivePokemon* active_pokemon) {
    UNUSED s32 pad[3];
    ActivePokemonState* state = &active_pokemon->state;
    s16 object_type = Battle_GetDisplayModelId(active_pokemon, 0);

    state->alpha = 0xFF;
    Model_SetMaterialColor(&active_pokemon->displayObject, 0xFF, 0xFF, 0xFF, state->alpha);
    BattleScene_ReinitParticipantBodyModel(active_pokemon, active_pokemon, object_type, 0);
    BattleScene_ReinitParticipantAfterimages(active_pokemon, object_type);
    Battle_ResetEventTrack(active_pokemon);
}
