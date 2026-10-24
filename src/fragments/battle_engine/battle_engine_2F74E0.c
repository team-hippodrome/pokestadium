#include "battle_engine.h"
#include "src/audio_commands.h"
#include "src/3D140.h"
#include "src/audio_sfx.h"
#include "src/audio_loop_point.h"
#include "controller.h"
#include "moves.h"
#include "status.h"

#define SELECTED_COMMAND_NONE 0
#define SELECTED_COMMAND_FIGHT 1
#define SELECTED_COMMAND_SWITCH 2
#define SELECTED_COMMAND_FORFEIT 3

static f32 sElapsedOrderSelectionTime;
f32 sOrderTimer;
f32 sBattleTimer;
static f32 sInitialOrderTimer;
static f32 sInitalBattleTimer;
u8 D_84390134;
static s32 sActionChosen;
static s32 sFallbackToAiChoice;
static s32 pad_D_84390140;
static u16 D_84390144;
static Controller* sTeammate1Controller;
static Controller* sTeammate2Controller;
static Controller sAttractScreenController;
s32 sAttractInputIndex;
unk_D_84390178 sAttractScreenTimer;

static u8 sPowersOfTwo[] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
};

void BattleScene_InitializeBattleTimers(void) {
    sInitialOrderTimer = gSessionContext.orderLimitSeconds * 30;
    sInitalBattleTimer = gSessionContext.battleLimitMinutes * 1800;
    sBattleTimer = gSessionContext.battleLimitMinutes * 1800;
}

void BattleScene_QuantizeBattleTimer(void) {
    s32 tmp = (s32)(sBattleTimer / 30.0f) * 30;

    sBattleTimer = tmp;
}

/*
    Battle timer for event battle (Mode where two plays fight with their transfer pak party pokemon)
    Of note, this keeps ticking even as move animations happen
*/
s32 BattleScene_TickBattleTimer(void) {
    if (sBattleTimer == 0.0f) {
        return 1;
    }

    sBattleTimer -= 1.0f;
    return 0;
}

s32 BattleScene_IsBattleCountdownComplete(void) {
    if (gSessionContext.timeLimitsEnabled == 0) {
        return 0;
    }

    if ((((s32)sBattleTimer % 30) == 0) && (sBattleTimer != 0.0f)) {
        Audio_PlayCommand(0x3E, 0, 0);
    }

    if (sBattleTimer == 0.0f) {
        return 1;
    }
    return 0;
}

s32 Battle_ClassifyHpTier(s32 current_hp, s32 max_hp) {
    s32 current_hp_units = current_hp * 48;

    if (current_hp_units < (max_hp * 10)) {
        return 1;
    }

    if (current_hp_units < (max_hp * 27)) {
        return 2;
    }

    return 3;
}

s32 BattleScene_DetermineWinner(void) {
    BattleMon* pokemon;
    s32 pokemon_index;
    s32 side1_pokemon_left = 0;
    s32 side2_pokemon_left = 0;
    s32 side1_hp_tier_sum = 0;
    s32 side2_hp_tier_sum = 0;

    for (pokemon_index = 0; pokemon_index < BattleScene_GetTeamMemberTeamSize(sActivePokemon[0]); pokemon_index++) {
        pokemon = &sActivePokemon[0]->teamRoster->pokemonPicked[pokemon_index];
        if (pokemon->hp != 0) {
            side1_pokemon_left++;
        }
    }

    for (pokemon_index = 0; pokemon_index < BattleScene_GetTeamMemberTeamSize(sActivePokemon[1]); pokemon_index++) {
        pokemon = &sActivePokemon[1]->teamRoster->pokemonPicked[pokemon_index];
        if (pokemon->hp != 0) {
            side2_pokemon_left++;
        }
    }

    if (side1_pokemon_left != side2_pokemon_left) {
        if (side2_pokemon_left < side1_pokemon_left) {
            return 1;
        }
        return 2;
    }

    for (pokemon_index = 0; pokemon_index < BattleScene_GetTeamMemberTeamSize(sActivePokemon[0]); pokemon_index++) {
        pokemon = &sActivePokemon[0]->teamRoster->pokemonPicked[pokemon_index];
        side1_hp_tier_sum += Battle_ClassifyHpTier(pokemon->hp, pokemon->maxHp);
    }

    for (pokemon_index = 0; pokemon_index < BattleScene_GetTeamMemberTeamSize(sActivePokemon[1]); pokemon_index++) {
        pokemon = &sActivePokemon[1]->teamRoster->pokemonPicked[pokemon_index];
        side2_hp_tier_sum += Battle_ClassifyHpTier(pokemon->hp, pokemon->maxHp);
    }

    if (side1_hp_tier_sum == side2_hp_tier_sum) {
        return 3;
    }

    if (side2_hp_tier_sum < side1_hp_tier_sum) {
        return 1;
    }

    if (side1_hp_tier_sum < side2_hp_tier_sum) {
        return 2;
    }
}

s32 BattleScene_UpdateResultPresentation(void) {
    s32 sp1C = 0;
    s32 var_v0;

    if (gSessionContext.timeLimitsEnabled == 0) {
        return 0;
    }

    if (BattleScene_IsBattleCountdownComplete() != 0) {
        switch (D_84390134) {
            case 1:
                BattleBox_Open(0xD);
                D_84390134 = 2;
                Audio_PlayCommand(0x3F, 0, 0);
                Audio_StartResultMusicSequence();
                sp1C = 1;
                break;

            case 2:
                sp1C = 1;

                var_v0 = 0;
                if (sElapsedOrderSelectionTime >= 120.0f) {
                    var_v0 = 1;
                }

                sElapsedOrderSelectionTime += 1.0f;

                if (var_v0 != 0) {
                    sBattleScene.scene->winner = BattleScene_DetermineWinner();
                    Audio_FadeOutAll(0x14);
                }
                break;
        }
    }
    return sp1C;
}

s32 BattleScene_IsActionSelectionTimerExpired(void) {
    if (gSessionContext.timeLimitsEnabled == 0) {
        return 0;
    }

    if (sOrderTimer == 329.0f) {
        Audio_PlayCommand(0x3C, 0, 0);
    }

    if (sOrderTimer == 91.0f) {
        Audio_PlayCommand(0x3D, 3, 0);
    }

    if (sOrderTimer == 61.0f) {
        Audio_PlayCommand(0x3D, 2, 0);
    }

    if (sOrderTimer == 31.0f) {
        Audio_PlayCommand(0x3D, 1, 0);
    }

    if (sOrderTimer == 1.0f) {
        Audio_PlayCommand(0x3D, 0, 0);
    }

    if (sOrderTimer == 0.0f) {
        return 1;
    }
    return 0;
}

void BattleScene_ResetActionSelectionTimerHelper(void) {
    sOrderTimer = gSessionContext.orderLimitSeconds * 30;
}

s32 BattleScene_TickActionSelectionTimers(void) {
    if (sOrderTimer > 0.0f) {
        sOrderTimer -= 1.0f;
    }

    if (sElapsedOrderSelectionTime >= 150.0f) {
        return 1;
    }

    sElapsedOrderSelectionTime += 1.0f;
    return 0;
}

void BattleScene_ResetActionSelectionTimer(void) {
    sElapsedOrderSelectionTime = 0.0f;
    BattleScene_ResetActionSelectionTimerHelper();
}

s32 BattleScene_ApplyForcedActionDelay(ActivePokemon* arg0) {
    ActivePokemonState* temp_v1 = &arg0->state;

    if (arg0->state.selectedCommandOption == 3) {
        temp_v1->actionDelayTimer += 0x38E;
        return 1;
    }
    return 0;
}

void BattleScene_SyncActiveHpToPartyState(void) {
    BattleMonRuntime* runtime1;
    BattleMonRuntime* runtime2;
    ActivePokemonState* state1;
    ActivePokemonState* state2;
    BattleMon* pokemon1;
    BattleMon* pokemon2;

    state1 = &sActivePokemon[0]->state;
    state2 = &sActivePokemon[1]->state;
    runtime1 = &sActivePokemon[0]->state.runtime;
    runtime2 = &sActivePokemon[1]->state.runtime;

    pokemon1 = &sActivePokemon[0]->teamRoster->pokemonPicked[state1->rosterIndex];
    pokemon2 = &sActivePokemon[1]->teamRoster->pokemonPicked[state2->rosterIndex];

    if (D_84390288 != 0) {
        D_843900A8[0] = runtime1->unk_0C = runtime1->hp;
        D_843900A8[1] = runtime2->unk_0C = runtime2->hp;

        pokemon1->hp = runtime1->hp;
        pokemon2->hp = runtime2->hp;
    }
}

s32 BattleScene_SelectFallbackWhenNoUsableMoves(ActivePokemon* pokemon) {
    s32 i;
    s32 num_usable_moves;
    ActivePokemonState* state;
    BattleMonRuntime* runtime;

    state = &pokemon->state;
    runtime = &pokemon->state.runtime;
    num_usable_moves = 0;

    if (pokemon->state.runtime.movesDisabled != 0) {
        for (i = 0; i < 4; i++) {
            if (((runtime->movesDisabled >> 4) != i) && (runtime->moves[i] != 0)) {
                num_usable_moves |= runtime->moveUsability[i] & 0x3F;
            }
        }
    } else {
        for (i = 0; i < 4; i++) {
            if (runtime->moves[i] != 0) {
                num_usable_moves |= runtime->moveUsability[i] & 0x3F;
            }
        }
    }

    if (num_usable_moves == 0) {
        state->actionDelayTimer = 0;
        state->selectedCommandOption = 3;
        state->moveIndex = 4;
        return 1;
    }
    return 0;
}

s32 BattleScene_IsMoveUnavailable(ActivePokemon* pokemon, s32 move) {
    BattleMonRuntime* runtime = &pokemon->state.runtime;

    if (!(runtime->moveUsability[move] & 0x3F)) {
        return 1;
    }

    if ((runtime->movesDisabled & 0xF) && (move == (runtime->movesDisabled >> 4))) {
        return 1;
    }

    return 0;
}

void BattleScene_AutoSelectAvailableMove(ActivePokemon* pokemon) {
    s32 i;
    ActivePokemonState* state = &pokemon->state;

    if (BattleScene_SelectFallbackWhenNoUsableMoves(pokemon) == 0) {
        for (i = 0; i < 4; i++) {
            if (BattleScene_IsMoveUnavailable(pokemon, i) == 0) {
                state->selectedCommand = SELECTED_COMMAND_FIGHT;
                state->selectedCommandOption = 3;
                state->moveIndex = i;
                state->selectedMoveIndex = i;
                break;
            }
        }
    }
}

void BattleScene_AutoSelectReplacementMon(ActivePokemon* pokemon) {
    s32 i;
    ActivePokemonState* state = &pokemon->state;

    for (i = 0; i < pokemon->sessionTeams->teamMember[0]->numberOfPokemonBrought; i++) {
        if ((pokemon->sessionTeams->teamMember[0]->pokemonPicked[i].hp != 0) && (i != state->rosterIndex)) {
            BattleAnim_PlayMenuConfirmSoundEffect();
            state->showingCommandOptions = 0;
            state->actionDelayTimer = 0;
            state->selectedCommandOption = 3;
            state->actionChosen = i;
            return;
        }
    }

    BattleScene_AutoSelectAvailableMove(pokemon);
}

void BattleScene_SelectActiveController(ActivePokemon* pokemon) {
    ActivePokemonState* state = &pokemon->state;

    state->ownerTeamMemberIndex = state->teamMemberIndex;
    state->controller = &gControllers[pokemon->sessionTeams->teamMember[state->ownerTeamMemberIndex]->controllerPort];
}

void BattleScene_HandleActionSelectionCancel(s32 side) {
    ActivePokemonState* state;
    ActivePokemonState* opponent_state;
    UNUSED s32 pad;
    Controller* cont;

    state = &sActivePokemon[side]->state;
    opponent_state = &sActivePokemon[side == 0]->state;
    cont = state->controller;

    if ((state->selectedCommandOption != 3) || !(cont->buttonPressed & BTN_L)) {
        return;
    }

    if (state->unk_2D == 0x10) {
        if (state->selectedCommand == SELECTED_COMMAND_SWITCH) {
            BattleScene_SelectActiveController(sActivePokemon[side]);
        }
        BattleAnim_PlayMenuBackSoundEffect();
        state->selectedCommandOption = 2;
    }

    if (opponent_state->unk_2D != 0x10) {
        if (state->selectedCommand == SELECTED_COMMAND_SWITCH) {
            BattleScene_SelectActiveController(sActivePokemon[side]);
        }
        BattleAnim_PlayMenuBackSoundEffect();
        state->selectedCommandOption = 0;
    }
}

void Battle_InitializeActiveRuntimeState(void) {
    BattleScene_InitializeBattleTimers();
    Battle_InitPokemonRuntimeState(sActivePokemon[0]);
    Battle_InitPokemonRuntimeState(sActivePokemon[1]);

    D_843900A8[0] = sActivePokemon[0]->state.runtime.unk_0C;
    D_843900A8[1] = sActivePokemon[1]->state.runtime.unk_0C;

    Battle_ResetTransientRuntimeState(0, 1);
    Battle_ResetTransientRuntimeState(1, 0);
}

s32 BattleScene_CpuHasNoUsableMoves(ActivePokemon* pokemon) {
    BattleMonRuntime* runtime = &pokemon->state.runtime;
    s32 i;
    s32 has_usable_moves = 0;

    if (runtime->movesDisabled != 0) {
        for (i = 0; i < 4; i++) {
            if (((runtime->movesDisabled >> 4) != i) && (runtime->moves[i] != 0)) {
                has_usable_moves |= runtime->moveUsability[i] & 0x3F;
            }
        }
    } else {
        for (i = 0; i < 4; i++) {
            if (runtime->moves[i] != 0) {
                has_usable_moves |= runtime->moveUsability[i] & 0x3F;
            }
        }
    }

    if (has_usable_moves == 0) {
        return 1;
    }
    return 0;
}

s32 BattleScene_WaitForCpuChoice(ActivePokemon* pokemon1, ActivePokemon* pokemon2) {
    UNUSED s32 pad[2];
    ActivePokemonState* state1 = &pokemon1->state;
    ActivePokemonState* state2 = &pokemon2->state;

    if ((pokemon1->state.unk_2D == 0x10) && (pokemon2->state.unk_2D == 0x10)) {
        if (state1->selectedCommandOption == 3) {
            BattleScene_ApplyForcedActionDelay(pokemon1);
            return 1;
        }

        state1->selectedCommand = SELECTED_COMMAND_SWITCH;
        state1->selectedCommandOption = 2;

        if (sElapsedOrderSelectionTime == 20.0f) {
            BattleAnim_PlayMenuConfirmSoundEffect();
            sActionChosen = BattleAI_ChooseAction(0);
            state1->showingCommandOptions = 0;
            state1->actionChosen = sActionChosen - 4;
            state1->actionDelayTimer = 0;
            state1->selectedCommandOption = 3;
            state1->selectedCommand = SELECTED_COMMAND_SWITCH;
        }
        return 1;
    }

    if (state1->unk_2D == 0x10) {
        state1->selectedCommand = SELECTED_COMMAND_SWITCH;
        state1->selectedCommandOption = 2;
        state2->selectedCommandOption = 3;
        state2->selectedCommand = SELECTED_COMMAND_FIGHT;

        if (sElapsedOrderSelectionTime == 20.0f) {
            BattleAnim_PlayMenuConfirmSoundEffect();
            sActionChosen = BattleAI_ChooseAction(0);
            state1->showingCommandOptions = 0;
            state1->actionChosen = sActionChosen - 4;
            state1->actionDelayTimer = 0;
            state1->selectedCommandOption = 3;
            state1->selectedCommand = SELECTED_COMMAND_SWITCH;
        }
        return 1;
    }

    return 0;
}

void BattleScene_CompleteCpuForcedAction(ActivePokemon* pokemon, UNUSED ActivePokemon* arg1) {
    ActivePokemonState* state = &pokemon->state;

    BattleScene_ApplyForcedActionDelay(pokemon);
    state->moveSelectionLocked = 1;
    state->selectedCommand = SELECTED_COMMAND_FIGHT;
    state->selectedMoveIndex = state->moveIndex;
    state->selectedCommandOption = 3;
}

void BattleScene_CompleteCpuMoveAction(ActivePokemon* pokemon1, ActivePokemon* pokemon2) {
    ActivePokemonState* state2 = &pokemon2->state;
    ActivePokemonState* state1 = &pokemon1->state;

    BattleScene_ApplyForcedActionDelay(pokemon1);
    state1->moveSelectionLocked = 1;

    if ((sElapsedOrderSelectionTime == 30.0f) && (sActionChosen < 4)) {
        BattleAnim_PlayMenuConfirmSoundEffect();
    }

    if ((sActionChosen < 4) && (sElapsedOrderSelectionTime >= 30.0f)) {
        state1->selectedCommandOption = 1;
        state1->selectedCommand = SELECTED_COMMAND_FIGHT;
    }

    if (((sElapsedOrderSelectionTime >= 90.0f) || ((state2->selectedCommandOption == 3) && (sElapsedOrderSelectionTime >= 10.0f))) && (sActionChosen < 4)) {
        BattleAnim_PlayMenuConfirmSoundEffect();
        state1->selectedCommand = SELECTED_COMMAND_FIGHT;
        state1->selectedCommandOption = 3;
        state1->selectedMoveIndex = state1->moveIndex;
    }
}

void BattleScene_ApplyCpuChoice(ActivePokemon* pokemon1, ActivePokemon* pokemon2) {
    ActivePokemonState* state1;
    ActivePokemonState* state2;
    s32 move_chosen;

    state1 = &pokemon1->state;
    state2 = &pokemon2->state;

    if ((sElapsedOrderSelectionTime == 30.0f) && (sActionChosen < 4)) {
        BattleAnim_PlayMenuConfirmSoundEffect();
    }

    if (sActionChosen < 4) {
        if (sElapsedOrderSelectionTime >= 30.0f) {
            state1->selectedCommandOption = 1;
            state1->selectedCommand = SELECTED_COMMAND_FIGHT;
        }
    }

    if ((sElapsedOrderSelectionTime >= 90.0f) || ((state2->selectedCommandOption == 3) && (sElapsedOrderSelectionTime >= 10.0f))) {
        if (sActionChosen < 4) {
            BattleAnim_PlayMenuConfirmSoundEffect();
            state1->selectedCommand = SELECTED_COMMAND_FIGHT;
            state1->selectedCommandOption = 3;
            if (BattleScene_CpuHasNoUsableMoves(pokemon1) != 0) {
                state1->moveIndex = 4;
                state1->selectedMoveIndex = 4;
            } else {
                move_chosen = sActionChosen;
                state1->moveIndex = move_chosen;
                state1->selectedMoveIndex = move_chosen;
            }
        }
    }
}

s32 BattleScene_ApplyCpuSwitchChoice(ActivePokemon* pokemon1, ActivePokemon* pokemon2) {
    ActivePokemonState* state1 = &pokemon1->state;
    ActivePokemonState* state2 = &pokemon2->state;

    if ((sElapsedOrderSelectionTime == 30.0f) && (sActionChosen >= 4)) {
        BattleAnim_PlayMenuConfirmSoundEffect();
    }

    if (sElapsedOrderSelectionTime >= 30.0f) {
        if (sActionChosen >= 4) {
            state1->selectedCommandOption = 2;
            state1->selectedCommand = SELECTED_COMMAND_SWITCH;
        }
    }

    if ((sElapsedOrderSelectionTime >= 90.0f) || ((state2->selectedCommandOption == 3) && (sElapsedOrderSelectionTime >= 10.0f))) {
        if (sActionChosen >= 4) {
            BattleAnim_PlayMenuConfirmSoundEffect();
            state1->showingCommandOptions = 0;
            state1->actionChosen = sActionChosen - 4;
            state1->actionDelayTimer = 0;
            state1->selectedCommandOption = 3;
            state1->selectedCommand = SELECTED_COMMAND_SWITCH;
        }
    }

    if (sActionChosen >= 4) {
        return 1;
    }
    return 0;
}

void BattleScene_TriggerCpuChoice(UNUSED BattleMonRuntime* arg0, UNUSED BattleMonRuntime* arg1, UNUSED ActivePokemonState* arg2,
                   UNUSED ActivePokemonState* arg3, UNUSED ActivePokemon* arg4) {
    if ((sElapsedOrderSelectionTime == 1.0f) && (sFallbackToAiChoice == 0)) {
        sFallbackToAiChoice = 1;
        sActionChosen = BattleAI_ChooseAction(0);
    }
}

s32 BattleScene_CollectCpuAction(ActivePokemon* pokemon1, ActivePokemon* pokemon2) {
    BattleMonRuntime* runtime1 = &pokemon1->state.runtime;
    BattleMonRuntime* runtime2 = &pokemon2->state.runtime;
    ActivePokemonState* state1 = &pokemon1->state;
    ActivePokemonState* state2 = &pokemon2->state;

    if (BattleScene_WaitForCpuChoice(pokemon1, pokemon2) != 0) {
        return 0;
    }

    if (!(runtime1->unk_4D & 0x60)) {
        runtime1->immobilizedReason &= ~IMMOBILIZED_FLINCHED;
        runtime2->immobilizedReason &= ~IMMOBILIZED_FLINCHED;

        if (!(runtime1->immobilizedReason & IMMOBILIZED_ACTION_LOCKED)) {
            if (BattleScene_ApplyForcedActionDelay(pokemon1) != 0) {
                return 0;
            }

            if (sElapsedOrderSelectionTime == 0.0f) {
                sFallbackToAiChoice = 0;
                return 0;
            }

            BattleScene_TriggerCpuChoice(runtime1, runtime2, state1, state2, pokemon2);

            if (BattleScene_ApplyCpuSwitchChoice(pokemon1, pokemon2) != 0) {
                return 0;
            }

            if (!(runtime1->unk_15 & 0x27)) {
                if (!(runtime1->immobilizedReason & (IMMOBILIZED_BIDING | IMMOBILIZED_TRAPPING))) {
                    if (runtime2->immobilizedReason & IMMOBILIZED_TRAPPING) {
                        BattleScene_CompleteCpuMoveAction(pokemon1, pokemon2);
                        runtime1->usedMove = 0xFF;
                    } else if (BattleScene_ApplyForcedActionDelay(pokemon1) != 0) {
                        return 0;
                    } else {
                        if (sElapsedOrderSelectionTime == 0.0f) {
                            sFallbackToAiChoice = 0;
                            return 0;
                        }
                        BattleScene_TriggerCpuChoice(runtime1, runtime2, state1, state2, pokemon2);
                        BattleScene_ApplyCpuChoice(pokemon1, pokemon2);
                    }
                } else {
                    BattleScene_CompleteCpuMoveAction(pokemon1, pokemon2);
                }
            } else {
                BattleScene_CompleteCpuMoveAction(pokemon1, pokemon2);
            }
        } else {
            BattleScene_CompleteCpuForcedAction(pokemon1, pokemon2);
        }
    } else {
        BattleScene_CompleteCpuForcedAction(pokemon1, pokemon2);
    }

    return 0;
}

void BattleScene_ForfeitPrompt(ActivePokemon* pokemon) {
    ActivePokemonState* state = &pokemon->state;
    ActivePokemonState* opponent_state = &sActivePokemon[BattleScene_GetParticipantSideIndex(pokemon) == 0]->state;

    if (pokemon->sessionTeams->tagTeamSize == 1) {
        sTeammate1Controller = &gControllers[pokemon->sessionTeams->teamMember[0]->controllerPort];
        sTeammate2Controller = &gControllers[pokemon->sessionTeams->teamMember[0]->controllerPort];
    } else {
        sTeammate1Controller = &gControllers[pokemon->sessionTeams->teamMember[0]->controllerPort];
        sTeammate2Controller = &gControllers[pokemon->sessionTeams->teamMember[1]->controllerPort];
    }

    if (opponent_state->selectedCommandOption == 4) {
        state->selectedCommandOption = 0;
        state->selectedCommand = SELECTED_COMMAND_NONE;
        return;
    }

    if (state->selectedCommandOption != 4) {
        state->forfeitCursorHover = 0;
    }

    state->selectedCommandOption = 4;

    if ((sBattleScene.scene->winner == 0) &&
        (BTN_IS_PRESSED(sTeammate1Controller, BTN_DLEFT) || BTN_IS_PRESSED(sTeammate2Controller, BTN_DLEFT))) {
        state->forfeitCursorHover = 0;
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
    }

    if ((sBattleScene.scene->winner == 0) &&
        (BTN_IS_PRESSED(sTeammate1Controller, BTN_DRIGHT) || BTN_IS_PRESSED(sTeammate2Controller, BTN_DRIGHT))) {
        state->forfeitCursorHover = 1;
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
    }

    if ((state->forfeitCursorHover == 1) && (BTN_IS_PRESSED(sTeammate1Controller, BTN_A) || BTN_IS_PRESSED(sTeammate2Controller, BTN_A)) &&
        (sBattleScene.scene->winner == 0)) {
        Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
        sBattleScene.scene->winner = (BattleScene_GetParticipantSideIndex(pokemon) == 0) + 1;
        Audio_FadeOutAll(0x14);
    } else if ((state->forfeitCursorHover == 0) && (BTN_IS_PRESSED(sTeammate1Controller, BTN_A) || BTN_IS_PRESSED(sTeammate2Controller, BTN_A))) {
        state->selectedCommandOption = 0;
        state->selectedCommand = SELECTED_COMMAND_NONE;
        Audio_PlaySoundEffectById(SFX_MENU_BACK);
    }
}

void BattleScene_CommitPlayerSwitchAction(ActivePokemon* arg0) {
    ActivePokemonState* temp_v0 = &arg0->state;
    BattleMonRuntime* temp_v1 = &sActivePokemon[BattleScene_GetParticipantSideIndex(arg0) == 0]->state.runtime;

    if (temp_v1->immobilizedReason & IMMOBILIZED_TRAPPING) {
        arg0->state.runtime.usedMove = 0xFF;
    }

    if (temp_v0->unk_2D != 0x10) {
        temp_v0->unk_2D = 0xF;
    }

    temp_v0->showingCommandOptions = 0;
    BattleScene_ClearParticipantMessageBuffer(arg0);
}

s32 BattleScene_ValidateSelectedPartyMember(ActivePokemon* arg0) {
    if (arg0->sessionTeams->teamMember[arg0->state.ownerTeamMemberIndex]->pokemonPicked[arg0->state.actionChosen].hp != 0) {
        return 1;
    }

    BattleScene_ClearParticipantMessageBuffer(arg0);
    BattleScene_BuildParticipantMessage(arg0, 3);
    BattleAnim_PlayMenuErrorSoundEffect();
    return 0;
}

void BattleScene_QueuePartySelectionConfirmMessage(ActivePokemon* arg0) {
    BattleScene_ClearParticipantMessageBuffer(arg0);
    BattleScene_BuildParticipantMessage(arg0, 4);
}

void BattleScene_AssignActiveCoopController(ActivePokemon* arg0, s32 arg1) {
    if (arg1 == 1) {
        sTeammate1Controller = &gControllers[arg0->sessionTeams->teamMember[0]->controllerPort];
        sTeammate2Controller = &gControllers[arg0->sessionTeams->teamMember[0]->controllerPort];
    } else {
        sTeammate1Controller = &gControllers[arg0->sessionTeams->teamMember[0]->controllerPort];
        sTeammate2Controller = &gControllers[arg0->sessionTeams->teamMember[1]->controllerPort];
    }

    if (arg1 == 1) {
        if (BTN_IS_PRESSED(sTeammate1Controller, BTN_A | BTN_B | BTN_CUP | BTN_CDOWN | BTN_CLEFT | BTN_CRIGHT)) {
            arg0->state.ownerTeamMemberIndex = 0;
            BattleScene_SetParticipantController(arg0, sTeammate1Controller);
        } else if (BTN_IS_PRESSED(sTeammate2Controller, BTN_A | BTN_B | BTN_CUP | BTN_CDOWN | BTN_CLEFT | BTN_CRIGHT)) {
            BattleScene_SetParticipantController(arg0, sTeammate2Controller);
            arg0->state.ownerTeamMemberIndex = 0;
        }
    } else if (BTN_IS_PRESSED(sTeammate1Controller, BTN_A | BTN_B | BTN_CUP | BTN_CDOWN | BTN_CLEFT | BTN_CRIGHT)) {
        BattleScene_SetParticipantController(arg0, sTeammate1Controller);
        arg0->state.ownerTeamMemberIndex = 0;
    } else if (BTN_IS_PRESSED(sTeammate2Controller, BTN_A | BTN_B | BTN_CUP | BTN_CDOWN | BTN_CLEFT | BTN_CRIGHT)) {
        BattleScene_SetParticipantController(arg0, sTeammate2Controller);
        arg0->state.ownerTeamMemberIndex = 1;
    }
}

void BattleScene_HandlePartySlotCursorInput(ActivePokemon* pokemon, Controller* controller) {
    ActivePokemonState* state;
    TeamRoster* current_team_member;

    state = &pokemon->state;

    BattleScene_AssignActiveCoopController(pokemon, pokemon->sessionTeams->tagTeamSize);
    current_team_member = pokemon->sessionTeams->teamMember[state->ownerTeamMemberIndex];
    controller = state->controller;

    if (current_team_member->numberOfPokemonBrought < 4) {
        if (BTN_IS_PRESSED(controller, BTN_CUP)) {
            state->actionChosen = 0;
        } else if (BTN_IS_PRESSED(controller, BTN_CRIGHT)) {
            state->actionChosen = 1;
        } else if (BTN_IS_PRESSED(controller, BTN_CDOWN)) {
            state->actionChosen = 2;
        } else {
            return;
        }
    } else {
        if (BTN_IS_PRESSED(controller, BTN_CUP)) {
            state->actionChosen = 0;
        }

        if (BTN_IS_PRESSED(controller, BTN_CLEFT)) {
            state->actionChosen = 1;
        }

        if (BTN_IS_PRESSED(controller, BTN_B)) {
            state->actionChosen = 2;
        }

        if (BTN_IS_PRESSED(controller, BTN_CRIGHT)) {
            state->actionChosen = 3;
        }

        if (BTN_IS_PRESSED(controller, BTN_CDOWN)) {
            state->actionChosen = 4;
        }

        if (BTN_IS_PRESSED(controller, BTN_A)) {
            state->actionChosen = 5;
        }
    }

    if (BTN_IS_PRESSED(controller, BTN_A | BTN_B | BTN_CUP | BTN_CDOWN | BTN_CLEFT | BTN_CRIGHT)) {
        if ((state->rosterIndex == state->actionChosen) && (state->ownerTeamMemberIndex == state->teamMemberIndex)) {
            if (BattleScene_ValidateSelectedPartyMember(pokemon) != 0) {
                BattleAnim_PlayMenuErrorSoundEffect();
                BattleScene_QueuePartySelectionConfirmMessage(pokemon);
            }
        } else if (((current_team_member->numberOfPokemonBrought - 1) >= state->actionChosen) && (BattleScene_ValidateSelectedPartyMember(pokemon) != 0)) {
            BattleAnim_PlayMenuConfirmSoundEffect();
            state->showingCommandOptions = 0;
            state->actionDelayTimer = 0;
            state->selectedCommandOption = 3;
        }
    }
}

void BattleScene_HandlePlayerPartySelection(ActivePokemon* pokemon) {
    UNUSED s32 pad;
    s32 idx;
    ActivePokemonState* state = &pokemon->state;

    if (pokemon->sessionTeams->tagTeamSize == 1) {
        sTeammate1Controller = &gControllers[pokemon->sessionTeams->teamMember[0]->controllerPort];
        sTeammate2Controller = &gControllers[pokemon->sessionTeams->teamMember[0]->controllerPort];
    } else {

        sTeammate1Controller = &gControllers[pokemon->sessionTeams->teamMember[state->ownerTeamMemberIndex]->controllerPort];
        idx = state->ownerTeamMemberIndex == 0;
        sTeammate2Controller = &gControllers[pokemon->sessionTeams->teamMember[idx]->controllerPort];
    }

    if (pokemon->captionState.active != 0) {
        if (BTN_IS_PRESSED(state->controller,
                           BTN_A | BTN_B | BTN_Z | BTN_L | BTN_R | BTN_CUP | BTN_CDOWN | BTN_CLEFT | BTN_CRIGHT)) {
            BattleScene_ClearParticipantMessageBuffer(pokemon);
        }
    } else if (state->selectedCommandOption == 0) {
        state->selectedCommandOption = 2;
    } else if ((BTN_IS_PRESSED(sTeammate1Controller, BTN_L) || BTN_IS_PRESSED(sTeammate2Controller, BTN_L)) &&
               (state->unk_2D != 0x10) && !BTN_IS_DOWN(sTeammate1Controller, BTN_R) && !BTN_IS_DOWN(sTeammate2Controller, BTN_R)) {
        BattleScene_SelectActiveController(pokemon);
        BattleAnim_PlayMenuBackSoundEffect();
        state->selectedCommandOption = 0;
    } else {
        state->showingCommandOptions = 0;
        if (BTN_IS_DOWN(sTeammate1Controller, BTN_R) || BTN_IS_DOWN(sTeammate2Controller, BTN_R)) {
            if (BTN_IS_PRESSED(sTeammate1Controller, BTN_R) || BTN_IS_PRESSED(sTeammate2Controller, BTN_R)) {
                BattleAnim_PlayMenuScrollSoundEffect();
            }
            state->showingCommandOptions = 1;
        }
        BattleScene_HandlePartySlotCursorInput(pokemon, state->controller);
    }
}

void BattleScene_CommitSelectedMoveAction(ActivePokemon* pokemon) {
    ActivePokemonState* state = &pokemon->state;
    BattleMonRuntime* runtime = &pokemon->state.runtime;

    if (pokemon->state.moveIndex != 4) {
        state->selectedMoveIndex = state->moveIndex;
        if (state->moveSelectionLocked == 0) {
            runtime->usedMove = runtime->moves[state->selectedMoveIndex];
        }
    } else {
        if (runtime->immobilizedReason & IMMOBILIZED_TRAPPING) {
            state->selectedMoveIndex = state->moveIndex;
        } else {
            state->moveIndex = 0;
            state->selectedMoveIndex = 0;
            runtime->usedMove = 0xA5;
        }
    }

    state->showingCommandOptions = 0;
    BattleScene_ClearParticipantMessageBuffer(pokemon);
}

s32 BattleScene_HandleNoUsableMoveSelection(ActivePokemon* pokemon) {
    s32 i;
    s32 temp_a1;
    s32 num_usable_moves;
    ActivePokemonState* state;
    BattleMonRuntime* runtime;
    Controller* controller;

    state = &pokemon->state;
    runtime = &pokemon->state.runtime;
    controller = pokemon->state.controller;
    num_usable_moves = 0;

    if (runtime->movesDisabled != 0) {
        temp_a1 = runtime->movesDisabled >> 4;
        for (i = 0; i < 4; i++) {
            if ((temp_a1 != i) && (runtime->moves[i] != 0)) {
                num_usable_moves |= runtime->moveUsability[i] & 0x3F;
            }
        }
    } else {
        for (i = 0; i < 4; i++) {
            if (runtime->moves[i] != 0) {
                num_usable_moves |= runtime->moveUsability[i] & 0x3F;
            }
        }
    }

    if (num_usable_moves == 0) {
        if (pokemon->captionState.active == 0) {
            if (state->selectedCommandOption == 0) {
                state->selectedCommandOption = 1;
            }
            BattleScene_ClearParticipantMessageBuffer(pokemon);
            BattleScene_BuildParticipantMessage(pokemon, 0);
        } else if (controller->buttonPressed & BTN_CLEFT) {
            BattleScene_ClearParticipantMessageBuffer(pokemon);
            state->selectedCommandOption = 0;
            BattleAnim_PlayMenuBackSoundEffect();
        } else if (controller->buttonPressed & BTN_A) {
            BattleScene_ClearParticipantMessageBuffer(pokemon);
            state->actionDelayTimer = 0;
            state->selectedCommandOption = 3;
            state->moveIndex = 4;
        }
        return 1;
    }
    return 0;
}

s32 BattleScene_ValidateSelectedMove(ActivePokemon* pokemon) {
    UNUSED s32 pad[2];
    ActivePokemonState* state = &pokemon->state;
    BattleMonRuntime* runtime = &pokemon->state.runtime;

    if (!(runtime->moveUsability[state->moveIndex] & 0x3F)) {
        BattleAnim_PlayMenuErrorSoundEffect();
        BattleScene_ClearParticipantMessageBuffer(pokemon);
        BattleScene_BuildParticipantMessage(pokemon, 1);
        return 1;
    }

    if ((runtime->movesDisabled & 0xF) && (state->moveIndex == (runtime->movesDisabled >> 4))) {
        BattleAnim_PlayMenuErrorSoundEffect();
        BattleScene_ClearParticipantMessageBuffer(pokemon);
        BattleScene_BuildParticipantMessage(pokemon, 2);
        return 1;
    }

    BattleAnim_PlayMenuConfirmSoundEffect();
    state->actionDelayTimer = 0;
    state->selectedCommandOption = 3;
    state->showingCommandOptions = 0;
    return 0;
}

void BattleScene_SelectMoveFromButtons(ActivePokemon* pokemon) {
    Controller* controller = pokemon->state.controller;

    if (BTN_IS_PRESSED(controller, BTN_CUP)) {
        // Struggle is the last move by index number
        if ((pokemon->state.runtime.moves[0] != 0) && (pokemon->state.runtime.moves[0] < (STRUGGLE + 1))) {
            pokemon->state.moveIndex = 0;
            BattleScene_ValidateSelectedMove(pokemon);
            return;
        }
    }

    if (BTN_IS_PRESSED(controller, BTN_CRIGHT)) {
        if ((pokemon->state.runtime.moves[1] != 0) && (pokemon->state.runtime.moves[1] < (STRUGGLE + 1))) {
            pokemon->state.moveIndex = 1;
            BattleScene_ValidateSelectedMove(pokemon);
            return;
        }
    }

    if (BTN_IS_PRESSED(controller, BTN_CDOWN)) {
        if ((pokemon->state.runtime.moves[2] != 0) && (pokemon->state.runtime.moves[2] < (STRUGGLE + 1))) {
            pokemon->state.moveIndex = 2;
            BattleScene_ValidateSelectedMove(pokemon);
            return;
        }
    }

    if (BTN_IS_PRESSED(controller, BTN_CLEFT)) {
        if ((pokemon->state.runtime.moves[3] != 0) && (pokemon->state.runtime.moves[3] < (STRUGGLE + 1))) {
            pokemon->state.moveIndex = 3;
            BattleScene_ValidateSelectedMove(pokemon);
            return;
        }
    }

    if (BTN_IS_PRESSED(controller, BTN_CUP | BTN_CRIGHT | BTN_CDOWN | BTN_CLEFT)) {
        BattleAnim_PlayMenuErrorSoundEffect();
    }
}

void BattleScene_HandlePlayerMoveSelection(ActivePokemon* pokemon) {
    UNUSED s32 pad[2];
    Controller* controller = pokemon->state.controller;
    ActivePokemonState* state = &pokemon->state;

    if ((state->selectedCommand == SELECTED_COMMAND_FIGHT) && (BattleScene_HandleNoUsableMoveSelection(pokemon) == 0)) {
        if (pokemon->captionState.active != 0) {
            if (BTN_IS_PRESSED(controller,
                               BTN_A | BTN_B | BTN_Z | BTN_L | BTN_R | BTN_CUP | BTN_CDOWN | BTN_CLEFT | BTN_CRIGHT)) {
                BattleScene_ClearParticipantMessageBuffer(pokemon);
            }
        } else {
            if (state->selectedCommandOption == 0) {
                state->selectedCommandOption = 1;
            } else if (BTN_IS_PRESSED(controller, BTN_L) && !BTN_IS_DOWN(controller, BTN_R)) {
                state->selectedCommandOption = 0;
                BattleAnim_PlayMenuBackSoundEffect();
            } else {
                state->showingCommandOptions = 0;
                if (BTN_IS_DOWN(controller, BTN_R)) {
                    state->showingCommandOptions = 1;
                }

                if (BTN_IS_PRESSED(controller, BTN_R)) {
                    BattleAnim_PlayMenuScrollSoundEffect();
                }

                BattleScene_SelectMoveFromButtons(pokemon);
            }
        }
    }
}

s32 BattleScene_HandlePlayerMoveSelectionLocked(ActivePokemon* pokemon, UNUSED Controller* arg1) {
    pokemon->state.moveSelectionLocked = 1;
    BattleScene_HandlePlayerMoveSelection(pokemon);
    pokemon->state.moveSelectionLocked = 1;
    return 0;
}

void BattleScene_ConfirmForcedMoveRepeat(ActivePokemon* arg0) {
    ActivePokemonState* state = &arg0->state;

    BattleScene_ApplyForcedActionDelay(arg0);
    state->moveSelectionLocked = 1;
    state->selectedCommandOption = 3;
    state->selectedMoveIndex = state->moveIndex;
}

void BattleScene_PollActionMenuRequestButtons(ActivePokemon* pokemon) {
    ActivePokemonState* state = &pokemon->state;
    ActivePokemonState* temp_state = &sActivePokemon[BattleScene_GetParticipantSideIndex(pokemon) == 0]->state;

    if (state->selectedCommandOption == 0) {
        if (pokemon->sessionTeams->tagTeamSize == 1) {
            sTeammate1Controller = &gControllers[pokemon->sessionTeams->teamMember[0]->controllerPort];
            sTeammate2Controller = &gControllers[pokemon->sessionTeams->teamMember[0]->controllerPort];
        } else {
            sTeammate1Controller = &gControllers[pokemon->sessionTeams->teamMember[0]->controllerPort];
            sTeammate2Controller = &gControllers[pokemon->sessionTeams->teamMember[1]->controllerPort];
        }

        if (state->controller->buttonPressed & A_BUTTON) {
            // FIGHT
            state->selectedCommand = SELECTED_COMMAND_FIGHT;
        } else if ((sTeammate1Controller->buttonPressed | sTeammate2Controller->buttonPressed) & B_BUTTON) {
            // SWITCH
            state->selectedCommand = SELECTED_COMMAND_SWITCH;
        } else if ((sTeammate1Controller->buttonPressed | sTeammate2Controller->buttonPressed) & START_BUTTON) {
            // FORFEIT
            state->selectedCommand = SELECTED_COMMAND_FORFEIT;
        } else {
            // NOT CHOSEN
            state->selectedCommand = SELECTED_COMMAND_NONE;
        }

        if ((state->controller->buttonPressed & A_BUTTON) ||
            (((sTeammate1Controller->buttonPressed | sTeammate2Controller->buttonPressed) & B_BUTTON) != 0) ||
            (((sTeammate1Controller->buttonPressed | sTeammate2Controller->buttonPressed) & START_BUTTON) && (temp_state->selectedCommandOption != 4))) {
            BattleAnim_PlayMenuConfirmSoundEffect();
        }
    }
}

s32 BattleScene_HandlePlayerActionInput(ActivePokemon* pokemon) {
    BattleScene_GetParticipantSideIndex(pokemon);

    if (BattleScene_ApplyForcedActionDelay(pokemon) != 0) {
        return 1;
    }

    if (BattleScene_IsActionSelectionTimerExpired() != 0) {
        if (pokemon->state.selectedCommand == SELECTED_COMMAND_SWITCH) {
            BattleScene_AutoSelectReplacementMon(pokemon);
        } else {
            BattleScene_AutoSelectAvailableMove(pokemon);
        }
        return 1;
    }

    BattleScene_PollActionMenuRequestButtons(pokemon);

    switch (pokemon->state.selectedCommand) {
        case SELECTED_COMMAND_SWITCH:
            BattleScene_HandlePlayerPartySelection(pokemon);
            return 1;

        case SELECTED_COMMAND_FORFEIT:
            BattleScene_ForfeitPrompt(pokemon);
            return 1;

        case SELECTED_COMMAND_FIGHT:
            break;
    }
    return 0;
}

s32 BattleScene_HandlePlayerForcedActionState(ActivePokemon* pokemon1, ActivePokemon* pokemon2) {
    ActivePokemonState* state1 = &pokemon1->state;
    ActivePokemonState* state2 = &pokemon2->state;

    if ((state1->unk_2D == 0x10) && (state2->unk_2D == 0x10)) {
        if (state1->selectedCommandOption == 3) {
            BattleScene_ApplyForcedActionDelay(pokemon1);
            return 1;
        }

        if (BattleScene_IsActionSelectionTimerExpired() != 0) {
            BattleScene_AutoSelectReplacementMon(pokemon1);
            return 1;
        }

        state1->selectedCommand = SELECTED_COMMAND_SWITCH;
        state1->selectedCommandOption = 2;
        BattleScene_HandlePlayerPartySelection(pokemon1);
        return 1;
    }

    if (state1->unk_2D == 0x10) {
        if (BattleScene_IsActionSelectionTimerExpired() != 0) {
            BattleScene_AutoSelectReplacementMon(pokemon1);
            return 1;
        }
        state1->selectedCommand = SELECTED_COMMAND_SWITCH;
        state2->selectedCommand = SELECTED_COMMAND_FIGHT;
        state1->selectedCommandOption = 2;
        state2->selectedCommandOption = 3;
        BattleScene_HandlePlayerPartySelection(pokemon1);
        return 1;
    }
    return 0;
}

void BattleScene_ShowParticipantOwnerAndSecondaryModels(void) {
    sActivePokemon[0]->thumbnail.camera->node.flags |= 1;
    sActivePokemon[0]->thumbnail.displayObject.node.flags |= 1;
    sActivePokemon[1]->thumbnail.camera->node.flags |= 1;
    sActivePokemon[1]->thumbnail.displayObject.node.flags |= 1;
}

void BattleScene_HideParticipantOwnerAndSecondaryModels(void) {
    sActivePokemon[0]->thumbnail.camera->node.flags &= ~1;
    sActivePokemon[0]->thumbnail.displayObject.node.flags &= ~1;
    sActivePokemon[1]->thumbnail.camera->node.flags &= ~1;
    sActivePokemon[1]->thumbnail.displayObject.node.flags &= ~1;
}

void BattleScene_AdvanceAttractInputScript(void) {
    static u16 sAttractInputScript[][3] = {
        { 0x17C, 0, 0 },        { 0xA, BTN_L, 0 },  { 0xA, BTN_R, 0 }, { 0xC8, BTN_START, 0 },
        { 0xC8, BTN_START, 0 }, { 0xDC, BTN_A, 0 }, { 0, 0, 0 },
    };
    Controller* controller = &sAttractScreenController;

    controller->buttonPressed = 0;

    if (sAttractScreenTimer.timeLeft == 0) {
        sAttractScreenTimer.timeLeft = sAttractInputScript[sAttractInputIndex][0];
        if (sAttractScreenTimer.timeLeft != 0) {
            controller->buttonPressed = sAttractInputScript[sAttractInputIndex][1];
            controller->buttonDown = sAttractInputScript[sAttractInputIndex][2];
            sAttractInputIndex++;
        }
    }

    if (sAttractScreenTimer.timeLeft > 0) {
        sAttractScreenTimer.timeLeft--;
    }
}

void BattleScene_CheckForcedActionCondition(ActivePokemon* pokemon1, ActivePokemon* pokemon2) {
    BattleMonRuntime* runtime1 = &pokemon1->state.runtime;
    BattleMonRuntime* runtime2 = &pokemon2->state.runtime;

    if (!(pokemon1->state.runtime.unk_4D & 0x60)) {
        runtime1->immobilizedReason &= ~IMMOBILIZED_FLINCHED;
        runtime2->immobilizedReason &= ~IMMOBILIZED_FLINCHED;
        if (runtime1->immobilizedReason & IMMOBILIZED_ACTION_LOCKED) {
            pokemon1->state.moveSelectionLocked = 1;
        }
    } else {
        pokemon1->state.moveSelectionLocked = 1;
    }
}

void BattleScene_TickAttractModeInput(void) {
    ActivePokemonState* sp24 = &sActivePokemon[0]->state;
    ActivePokemonState* sp20 = &sActivePokemon[1]->state;
    BattleMonRuntime* sp1C = &sActivePokemon[0]->state.runtime;
    BattleMonRuntime* sp18 = &sActivePokemon[1]->state.runtime;
    Controller* cont = &sAttractScreenController;

    BattleScene_AdvanceAttractInputScript();
    BattleScene_CheckForcedActionCondition(sActivePokemon[0], sActivePokemon[1]);
    BattleScene_CheckForcedActionCondition(sActivePokemon[1], sActivePokemon[0]);

    if (cont->buttonPressed & 0x20) {
        BattleScene_ShowParticipantOwnerAndSecondaryModels();
        BattleThumbnail_TriggerOwnerModelAnimation(sActivePokemon[0]);
        BattleThumbnail_TriggerOwnerModelAnimation(sActivePokemon[1]);
    }

    if (cont->buttonPressed & 0x10) {
        BattleScene_HideParticipantOwnerAndSecondaryModels();
    }

    if (cont->buttonPressed & 0x1000) {
        sp24->selectedCommand = SELECTED_COMMAND_NONE;
        sp20->selectedCommand = SELECTED_COMMAND_NONE;

        if (sp24->moveSelectionLocked == 0) {
            sp1C->usedMove = BattleAI_ResolveRandomMove(0, 0);
        }

        if (sp20->moveSelectionLocked == 0) {
            sp18->usedMove = BattleAI_ResolveRandomMove(1, 0);
        }

        sp24->selectedCommandOption = 3;
        sp20->selectedCommandOption = 3;

        if ((sp24->selectedCommandOption == 3) && (sp20->selectedCommandOption == 3)) {
            Battle_StartTurn();
        }
    }

    BattleScene_SyncActiveHpToPartyState();

    if (cont->buttonPressed & 0x2000) {
        sAttractInputIndex = 0;
        sAttractScreenTimer.timeLeft = 0;
    }

    if (cont->buttonPressed & 0x8000) {
        sBattleScene.scene->winner = 3;
    }
}

void BattleScene_CollectPlayerAction(ActivePokemon* arg0, ActivePokemon* arg1) {
    UNUSED s32 pad;
    BattleMonRuntime* sp20 = &arg0->state.runtime;
    BattleMonRuntime* sp1C = &arg1->state.runtime;

    if (BattleScene_HandlePlayerForcedActionState(arg0, arg1) == 0) {
        if (!(sp20->unk_4D & 0x60)) {
            sp20->immobilizedReason &= ~IMMOBILIZED_FLINCHED;
            sp1C->immobilizedReason &= ~IMMOBILIZED_FLINCHED;
            if (!(sp20->immobilizedReason & IMMOBILIZED_ACTION_LOCKED)) {
                if (BattleScene_HandlePlayerActionInput(arg0) == 0) {
                    if (!(sp20->unk_15 & 0x27)) {
                        if (!(sp20->immobilizedReason & (IMMOBILIZED_BIDING | IMMOBILIZED_TRAPPING))) {
                            if (sp1C->immobilizedReason & IMMOBILIZED_TRAPPING) {
                                BattleScene_HandlePlayerMoveSelectionLocked(arg0, arg0->state.controller);
                                sp20->usedMove = 0xFF;
                            } else {
                                BattleScene_HandlePlayerMoveSelection(arg0);
                            }
                        } else {
                            BattleScene_HandlePlayerMoveSelectionLocked(arg0, arg0->state.controller);
                        }
                    } else {
                        BattleScene_HandlePlayerMoveSelectionLocked(arg0, arg0->state.controller);
                    }
                }
            } else {
                BattleScene_ConfirmForcedMoveRepeat(arg0);
            }
        } else {
            BattleScene_ConfirmForcedMoveRepeat(arg0);
        }
    }
}

void BattleScene_CollectActions(void) {
    ActivePokemonState* sp24 = &sActivePokemon[0]->state;
    ActivePokemonState* sp20 = &sActivePokemon[1]->state;
    BattleMonRuntime* sp1C = &sActivePokemon[0]->state.runtime;
    BattleMonRuntime* sp18 = &sActivePokemon[1]->state.runtime;

    if (sActivePokemon[0]->teamRoster->unk_000 & 2) {
        D_84390144 = sBattleDamage;
        BattleScene_CollectCpuAction(sActivePokemon[0], sActivePokemon[1]);
        sBattleDamage = D_84390144;
    } else {
        BattleScene_CollectPlayerAction(sActivePokemon[0], sActivePokemon[1]);
    }

    if (sActivePokemon[1]->teamRoster->unk_000 & 2) {
        D_84390144 = sBattleDamage;
        BattleScene_CollectCpuAction(sActivePokemon[1], sActivePokemon[0]);
        sBattleDamage = D_84390144;
    } else {
        BattleScene_CollectPlayerAction(sActivePokemon[1], sActivePokemon[0]);
    }

    if ((sp24->selectedCommandOption == 3) && (sp20->selectedCommandOption == 3)) {
        if (BattleScene_IsBattleCountdownComplete() != 0) {
            D_84390134 = 1;
        }

        if (sp24->selectedCommand == SELECTED_COMMAND_SWITCH) {
            BattleScene_CommitPlayerSwitchAction(sActivePokemon[0]);
        } else {
            BattleScene_CommitSelectedMoveAction(sActivePokemon[0]);
        }

        if (sp20->selectedCommand == SELECTED_COMMAND_SWITCH) {
            BattleScene_CommitPlayerSwitchAction(sActivePokemon[1]);
        } else {
            BattleScene_CommitSelectedMoveAction(sActivePokemon[1]);
        }

        if ((sp24->selectedCommand == SELECTED_COMMAND_SWITCH) || (sp20->selectedCommand == SELECTED_COMMAND_SWITCH)) {
            BattleScene_InitializeSwitchActions();
        } else {
            Battle_StartTurn();
        }
    } else {
        if (!(sActivePokemon[0]->teamRoster->unk_000 & 2) && !(sp1C->unk_4D & 0x60) && !(sp1C->immobilizedReason & IMMOBILIZED_ACTION_LOCKED)) {
            BattleScene_HandleActionSelectionCancel(0);
        }

        if (!(sActivePokemon[1]->teamRoster->unk_000 & 2) && !(sp18->unk_4D & 0x60) && !(sp18->immobilizedReason & IMMOBILIZED_ACTION_LOCKED)) {
            BattleScene_HandleActionSelectionCancel(1);
        }
    }
}

void BattleScene_UpdateActionCollection(void) {
    D_84390288 = 0;
    if (gSessionContext.mode == 0x10) {
        BattleScene_TickAttractModeInput();
    } else {
        BattleScene_CollectActions();
    }
}

void BattleScene_UpdateActionSelectPhase(void) {
    ActivePokemonState* sp1C = &sActivePokemon[0]->state;
    ActivePokemonState* sp18 = &sActivePokemon[1]->state;

    if (BattleScene_UpdateResultPresentation() == 0) {
        sActivePokemon[0]->unk_4C8 = 0;
        sActivePokemon[1]->unk_4C8 = 0;

        if (sp1C->moveSelectionLocked != 1) {
            sp1C->moveSelectionLocked = 0;
        }

        if (sp18->moveSelectionLocked != 1) {
            sp18->moveSelectionLocked = 0;
        }

        BattleScene_UpdateActionCollection();
        BattleScene_TickBattleTimer();
        BattleScene_TickActionSelectionTimers();
    }
}
