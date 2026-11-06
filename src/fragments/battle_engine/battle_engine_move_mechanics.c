#include "battle_engine.h"
#include "moves.h"
#include "src/text_system.h"
#include "status.h"
#include "types.h"

/*actual battle logic*/
#define STADIUM 1

typedef void (*MoveEffectCallback)(void);

static const char D_8438DC80[] = "は"
                                 "\n"
                                 "こんらんした";
static const char D_8438DC90[] = "SP %d\n";
static const char D_8438DC98[] = "index %d  step %d\n";
static const char D_8438DCAC[] = "index %d  step %d\n";
static const char D_8438DCC0[] = "index %d\n";
static const char D_8438DCCC[] = "paramStep %d index %d  step %d\n";
static const char D_8438DCEC[] = "\n%%%%%%%%%%%%%%%%%%%%%%%\n";
static const char D_8438DD08[] = "INDEX %d   WARIKOMI %d\n";
static const char D_8438DD20[] = "%%%%%%%%%%%%%%%%%%%%%%%\n\n";
static const char D_8438DD3C[] = "FAIL\n";
static const char D_8438DD44[] = "index %d\n";
static const char D_8438DD50[] = "index %d\n";
static const char D_8438DD5C[] = "FAIL\n";
static const char D_8438DD64[] = "FAIL2\n";
static const char D_8438DD6C[] = "FAIL3\n";
static const char D_8438DD74[] = "offp->count1 %d\n";
static const char D_8438DD88[] = "\n\n";
static const char D_8438DD8C[] = "******** spAttackMain ********\n";
static const char D_8438DDAC[] = "!!!! SPATTACK Num %d!!!!\n";
static const char D_8438DDC8[] = "******** spAttackMainEnd ********\n";

static char* D_8438AE50 = D_8438DC80;
static MoveEffectCallback sMoveEffectHandlers[] = {
    Battle_Effect_Sleep,
    Battle_Effect_Poison,
    Battle_Effect_DrainHP,
    Battle_Effect_SecondaryStatusChance,
    Battle_Effect_SecondaryStatusChance,
    Battle_Effect_SecondaryStatusChance,
    Battle_Effect_Explosion,
    Battle_Effect_DrainHP,
    NULL,
    Battle_Effect_RaiseOwnStat,
    Battle_Effect_RaiseOwnStat,
    Battle_Effect_RaiseOwnStat,
    Battle_Effect_RaiseOwnStat,
    Battle_Effect_RaiseOwnStat,
    Battle_Effect_RaiseOwnStat,
    Battle_Effect_PayDay,
    NULL,
    Battle_Effect_LowerStat,
    Battle_Effect_LowerStat,
    Battle_Effect_LowerStat,
    Battle_Effect_LowerStat,
    Battle_Effect_LowerStat,
    Battle_Effect_LowerStat,
    Battle_Effect_Conversion,
    Battle_Effect_Haze,
    Battle_Effect_Bide,
    Battle_Effect_Thrash,
    Battle_Effect_ForceSwitchOrFlee,
    Battle_Effect_MultiHit,
    Battle_Effect_MultiHit,
    Battle_Effect_Flinch,
    Battle_Effect_Sleep,
    Battle_Effect_Poison,
    Battle_Effect_SecondaryStatusChance,
    Battle_Effect_SecondaryStatusChance,
    Battle_Effect_SecondaryStatusChance,
    Battle_Effect_Flinch,
    Battle_Effect_OHKO,
    Battle_Effect_TwoTurnMove,
    NULL,
    NULL,
    Battle_Effect_Trap,
    Battle_Effect_TwoTurnMove,
    Battle_Effect_MultiHit,
    NULL,
    Battle_Effect_Mist,
    Battle_Effect_FocusEnergy,
    Battle_Effect_Recoil,
    Battle_Effect_Confuse,
    Battle_Effect_RaiseOwnStat,
    Battle_Effect_RaiseOwnStat,
    Battle_Effect_RaiseOwnStat,
    Battle_Effect_RaiseOwnStat,
    Battle_Effect_RaiseOwnStat,
    Battle_Effect_RaiseOwnStat,
    Battle_Effect_Heal,
    Battle_Effect_Transform,
    Battle_Effect_LowerStat,
    Battle_Effect_LowerStat,
    Battle_Effect_LowerStat,
    Battle_Effect_LowerStat,
    Battle_Effect_LowerStat,
    Battle_Effect_LowerStat,
    Battle_Effect_ReflectLightScreen,
    Battle_Effect_ReflectLightScreen,
    Battle_Effect_Poison,
    Battle_Effect_Paralyze,
    Battle_Effect_LowerStat,
    Battle_Effect_LowerStat,
    Battle_Effect_LowerStat,
    Battle_Effect_LowerStat,
    Battle_Effect_LowerStat,
    Battle_Effect_LowerStat,
    Battle_Effect_LowerStat,
    Battle_Effect_LowerStat,
    Battle_Effect_ConfuseChance,
    Battle_Effect_MultiHit,
    NULL,
    Battle_Effect_Substitute,
    Battle_Effect_Recharge,
    Battle_Effect_Rage,
    Battle_Effect_Mimic,
    NULL,
    Battle_Effect_LeechSeed,
    Battle_Effect_Splash,
    Battle_Effect_Disable,
    NULL,
};

u16 Battle_ApplyStatStageMultiplier(u16* stat_ptr, u16 stat_stage_delta, u8 stat_index) {
    *stat_ptr = (stat_stage_delta * sStatStageMultipliers[stat_index - 1].multiplier) / sStatStageMultipliers[stat_index - 1].devisor;

    if (*stat_ptr == 0) {
        *stat_ptr = 1;
    }

    if (*stat_ptr > 999) {
        *stat_ptr = 999;
    }

    return *stat_ptr;
}

void Battle_Effect_Sleep(void) {
    s32 is_frozen;
    s32 sleep_turns;

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_TARGET_STATUS;
    is_frozen = sBattleTarget->statusCondition & STATUS_FREEZE;
    sBattleTarget->statusCondition &= ~STATUS_FREEZE;

    if (is_frozen == 0) {
        if (sBattleTarget->pendingStatusCondition & 7) {
            sConfused = 0;
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x80);
            return;
        }

        if (sBattleTarget->pendingStatusCondition == 0) {
            HandleMoveEffectFlags();
            if ((sBattleMoveFailed == 0) &&
                ((Battle_SleepClauseInEffect(sActivePokemon[!sBattleScene.scene->actingSide]) == 0) ||
                 (sBattleMechanicsSetting[0] == 2))) {
                goto block_9;
            }
        }
        sBattleDamage = 0;
        sBattleMoveFailed = 1;
        sConfused = 0;
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x7F);
        return;
    }

block_9:
    if ((Battle_SleepClauseInEffect(sActivePokemon[!sBattleScene.scene->actingSide]) != 0) && (sBattleMechanicsSetting[0] != 2)) {
        sConfused = 0;
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x7F);
        if ((is_frozen != 0) && (sBattleMechanicsSetting[0] == 1)) {
            sBattleTarget->statusCondition |= STATUS_FREEZE;
        }
    } else {
        do {
            if (sBattleMechanicsSetting[0] == 2) {
                sleep_turns = Battle_Random() & 7;
            } else {
                sleep_turns = Battle_Random() & 3;
            }
        } while (sleep_turns == 0);
        sBattleMoveFailed = 0;
        sBattleTarget->pendingStatusCondition = sleep_turns;
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x7E);
    }
}

void Battle_Effect_Poison(void) {
    char* buffer = sBattleMessageQueues->buffer[1];

    if (!(sBattleTarget->statusCondition & STATUS_BURN) && (sBattleTarget->pendingStatusCondition == 0) && (sBattleTarget->types[0] != 3) &&
        (sBattleTarget->types[1] != 3)) {
        switch (sBattleUser->moveCache.effect) {
            case 2:
                if (sBattleScene.scene->moveEffectKind != MOVE_PRESENTATION_MULTI_HIT) {
                    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_NORMAL;
                    buffer = sBattleMessageQueues->buffer[7];
                } else {
                    buffer = sBattleMessageQueues->buffer[11];
                }

                if (Battle_Random() >= 52) {
                    return;
                }
                break;

            case 33:
                sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_NORMAL;
                buffer = sBattleMessageQueues->buffer[7];
                if (Battle_Random() >= 103) {
                    return;
                }
                break;

            default:
                sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_TARGET_STATUS;
                sTypeEffectiveness = 0xA;
                HandleMoveEffectFlags();
                if (sBattleMoveFailed != 0) {
                    Text_SetStringToken(0x19,
                                        Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                    Battle_QueueMessage(buffer, -0x7D);
                    return;
                }
                break;
        }

        sBattleTarget->pendingStatusCondition |= 8;
        if (sBattleUser->moveCache.moveId == TOXIC) {
            sBattleTarget->uniqueStatusFlags |= 1;
            sBattleTarget->unk_51 = 0;
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            Battle_QueueMessage(buffer, -0x7C);
        } else {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            Battle_QueueMessage(buffer, -0x7B);
        }
    } else if (sBattleUser->moveCache.effect == 0x42) {
        if (sBattleScene.scene->moveEffectKind != MOVE_PRESENTATION_MULTI_HIT) {
            sBattleMoveFailed = 1;
            sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_TARGET_STATUS;
            sConfused = 0;
        }
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        Battle_QueueMessage(buffer, -0x7A);
    }
}

void Battle_Effect_DrainHP(void) {
    sBattleDamage /= 2;
    if (sBattleDamage == 0) {
        sBattleDamage = 1;
    }

    if (sBattleUser->maxHp < sBattleUser->displayHp + sBattleDamage) {
        sBattleUser->displayHp = sBattleUser->maxHp;
    } else {
        sBattleUser->displayHp += sBattleDamage;
    }

    if (sBattleUser->moveCache.effect == 8) {
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[7], -0x79);
    } else {
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[7], -0x78);
    }
}

void Battle_Effect_Explosion(void) {
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_EXPLOSION;
    sBattleUser->displayHp = 0;
    sBattleUser->pendingStatusCondition = 0;
    sBattleUser->statusCondition &= ~STATUS_LEECH_SEED;
}

void Battle_Effect_SecondaryStatusChance(void) {
    BattleMonRuntime* runtime = &sActivePokemon[!sBattleScene.scene->actingSide]->state.runtime;
    ActivePokemonState* state = &sActivePokemon[!sBattleScene.scene->actingSide]->state;

    if (sBattleTarget->statusCondition & STATUS_BURN) {
        return;
    }

    if (sBattleTarget->pendingStatusCondition != 0) {
        if ((sBattleTarget->pendingStatusCondition & STATUS_FREEZE) && (sBattleUser->moveCache.type == 0x14)) {
            sBattleTarget->pendingStatusCondition = 0;
            if (state->moveIndex != 4) {
                runtime->usedMove = runtime->moves[state->selectedMoveIndex];
            } else {
                state->moveIndex = 0;
                state->selectedMoveIndex = 0;
                runtime->usedMove = STRUGGLE;
            }
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            // The Ice melted!
            Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x77);
        }
        return;
    }

    if ((sBattleTarget->types[0] != (*sBattleUser).moveCache.type) &&
        (sBattleTarget->types[1] != (*sBattleUser).moveCache.type)) {
        if ((sBattleUser->moveCache.effect < 7) ||
            ((sBattleMechanicsSetting[0] != 2) && (sBattleUser->moveCache.moveId == BLIZZARD))) {
            if (Battle_Random() >= 26) {
                return;
            }
        } else if (Battle_Random() >= 77) {
            return;
        }

        switch (sBattleUser->moveCache.effect) {
            case EFFECT_BURN_10_PERCENT:
            case EFFECT_BURN_30_PERCENT:
                sBattleTarget->pendingStatusCondition = STATUS_BURN;
                ApplyBurnAttackCut(sActivePokemon[!sBattleScene.scene->actingSide]);
                Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                Battle_QueueMessage(sBattleMessageQueues->buffer[7], -0x76);
                break;

            case EFFECT_FREEZE_10_PERCENT:
            case EFFECT_FREEZE_30_PERCENT:
                if (Battle_FreezeClauseInEffect(sActivePokemon[!sBattleScene.scene->actingSide]) == 0) {
                    sBattleTarget->statusCondition &= ~STATUS_FREEZE;
                    sBattleTarget->pendingStatusCondition = 0x20;
                    Text_SetStringToken(0x19,
                                        Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                    Battle_QueueMessage(sBattleMessageQueues->buffer[7], -0x75);
                }
                break;

            case EFFECT_PARALYZE_10_PERCENT:
            case EFFECT_PARALYZE_30_PERCENT:
                sBattleTarget->pendingStatusCondition = STATUS_PARALYSIS;
                ApplyParalysisSpeedCut(sActivePokemon[!sBattleScene.scene->actingSide]);
                Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                Battle_QueueMessage(sBattleMessageQueues->buffer[7], -0x74);
                break;
        }
    }
}

// better name: BattleEffect_ApplyStatBoost
void Battle_ApplyRageStatBoost(void) {
    s32 attack_modified;
    s32 speed_modified;
    s32 move_effect_index;
    s32 increase_amount;
    u16* stat_value;
    u16* unmodified_stat_value;

    sBattleScene.scene->rageFollowupPhase = BATTLE_PHASE_RAGE;

    increase_amount = 1;
    move_effect_index = sBattleUser->moveCache.effect - EFFECT_ATTACK_UP1;
    if (move_effect_index >= (EFFECT_ATTACK_UP1 - 2)) {
        increase_amount = 2;
        move_effect_index -= (EFFECT_ATTACK_UP2 - EFFECT_ATTACK_UP1);
    }

    if (sBattleUser->statChanges[move_effect_index] != 13) {
        sBattleUser->statChanges[move_effect_index] += increase_amount;
        if (sBattleUser->statChanges[move_effect_index] >= 14) {
            sBattleUser->statChanges[increase_amount] = 13;
        }

        speed_modified = 0;
        attack_modified = 0;

        if (move_effect_index < 4) {
            switch (move_effect_index) {
                case 0:
                    unmodified_stat_value = &sBattleUser->unmodifiedAttack;
                    stat_value = &sBattleUser->modifiedAttack;
                    attack_modified = 1;
                    break;

                case 1:
                    unmodified_stat_value = &sBattleUser->unmodifiedDefense;
                    stat_value = &sBattleUser->modifiedDefense;
                    break;

                case 2:
                    unmodified_stat_value = &sBattleUser->unmodifiedSpeed;
                    stat_value = &sBattleUser->modifiedSpeed;
                    speed_modified = 1;
                    break;

                case 3:
                    unmodified_stat_value = &sBattleUser->unmodifiedSpecial;
                    stat_value = &sBattleUser->modifiedSpecial;
                    break;
            }

            if (*stat_value != 999) {
                *stat_value = (*unmodified_stat_value * sStatStageMultipliers[sBattleUser->statChanges[move_effect_index] - 1].multiplier) /
                        sStatStageMultipliers[sBattleUser->statChanges[move_effect_index] - 1].devisor;
                if (*stat_value >= 1000) {
                    *stat_value = 999;
                }
            } else {
                goto nothing_happens;
            }
        }

        if (sBattleUser->moveCache.moveId == MINIMIZE) {
            sBattleUser->scaleResize = 1;
        }

        if (increase_amount == 2) {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[10], move_effect_index + 0x8D);
        } else {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[10], move_effect_index + 0x93);
        }

        if ((speed_modified != 0) || (sBattleMechanicsSetting[0] != 1)) {
            ApplyParalysisSpeedCut(sActivePokemon[!sBattleScene.scene->actingSide]);
        }

        if ((attack_modified != 0) || (sBattleMechanicsSetting[0] != 1)) {
            ApplyBurnAttackCut(sActivePokemon[!sBattleScene.scene->actingSide]);
        }
    } else {
    nothing_happens:
        // Nothing Happened!
        Battle_QueueMessage(sBattleMessageQueues->buffer[10], -0x67);
    }
}

void Battle_Effect_RaiseOwnStat(void) {
    s32 attack_modified;
    s32 speed_modified;
    s32 move_effect_index;
    s32 increase_amount;
    u16* modified_stat_value;
    u16* unmodified_stat_value;

    if (sBattleUser->moveCache.moveId == WITHDRAW) {
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_WITHDRAW;
    } else {
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_SELF_TARGET;
    }

    increase_amount = 1;
    move_effect_index = sBattleUser->moveCache.effect - EFFECT_ATTACK_UP1;
    if (move_effect_index >= (EFFECT_ATTACK_UP1 - 2)) {
        increase_amount = 2;
        move_effect_index -= (EFFECT_ATTACK_UP2 - EFFECT_ATTACK_UP1);
    }

    if (sBattleUser->statChanges[move_effect_index] != 13) {
        sBattleUser->statChanges[move_effect_index] += increase_amount;
        if (sBattleUser->statChanges[move_effect_index] >= 14) {
            sBattleUser->statChanges[move_effect_index] = 13;
        }

        speed_modified = 0;
        attack_modified = 0;

        if (move_effect_index < 4) {
            switch (move_effect_index) {
                case 0:
                    unmodified_stat_value = &sBattleUser->unmodifiedAttack;
                    modified_stat_value = &sBattleUser->modifiedAttack;
                    attack_modified = 1;
                    break;

                case 1:
                    unmodified_stat_value = &sBattleUser->unmodifiedDefense;
                    modified_stat_value = &sBattleUser->modifiedDefense;
                    break;

                case 2:
                    unmodified_stat_value = &sBattleUser->unmodifiedSpeed;
                    modified_stat_value = &sBattleUser->modifiedSpeed;
                    speed_modified = 1;
                    break;

                case 3:
                    unmodified_stat_value = &sBattleUser->unmodifiedSpecial;
                    modified_stat_value = &sBattleUser->modifiedSpecial;
                    break;
            }

            if (*modified_stat_value != 999) {
                *modified_stat_value = (*unmodified_stat_value * sStatStageMultipliers[sBattleUser->statChanges[move_effect_index] - 1].multiplier) /
                        sStatStageMultipliers[sBattleUser->statChanges[move_effect_index] - 1].devisor;
                if (*modified_stat_value >= 1000) {
                    *modified_stat_value = 999;
                }
            } else {
                goto nothing_happened;
            }
        }

        if (sBattleUser->moveCache.moveId == MINIMIZE) {
            sBattleUser->scaleResize = 1;
        }

        if (increase_amount == 2) {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[1], move_effect_index + 0x8D);
        } else {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[1], move_effect_index + 0x93);
        }

        if ((speed_modified != 0) || (sBattleMechanicsSetting[0] != 1)) {
            ApplyParalysisSpeedCut(sActivePokemon[sBattleScene.scene->actingSide]);
        }

        if ((attack_modified != 0) || (sBattleMechanicsSetting[0] != 1)) {
            ApplyBurnAttackCut(sActivePokemon[sBattleScene.scene->actingSide]);
        }
    } else {
    nothing_happened:
        sStatCanBeChangedFurther = 0;
        // Nothing Happened!
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x67);
    }
}

void Battle_Effect_LowerStat(void) {
    s32 attack_modified;
    s32 speed_modified;
    s32 move_effect_index;
    s32 amount;
    u16* modified_stat_value;
    u16* unmodified_stat_value;

    if (sBattleUser->moveCache.effect < EFFECT_ATTACK_DOWN_33_PERCENT) {
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_TARGET_STAT_DROP;
    } else {
        sBattleScene.scene->hitReactionKind = BATTLE_HIT_REACTION_SECONDARY;
    }

    if (sBattleTarget->statusCondition & STATUS_BURN) {
        goto move_failed;
    }

    move_effect_index = sBattleUser->moveCache.effect;
    amount = 1;

    if (move_effect_index >= EFFECT_ATTACK_DOWN_33_PERCENT) {
        if (Battle_Random() < 85) {
            move_effect_index -= EFFECT_ATTACK_DOWN_33_PERCENT;
            goto side_effect;
        }
    } else {
        HandleMoveEffectFlags();

        if (sBattleMoveFailed != 0) {
            goto move_failed;
        }

        if (sBattleTarget->immobilizedReason & IMMOBILIZED_SEMI_INVULNERABLE) {
            goto move_failed;
        }

        move_effect_index -= EFFECT_ATTACK_DOWN1;
        if (move_effect_index >= (EFFECT_ATTACK_UP1 - 1)) {
            amount = 2;
            move_effect_index -= (EFFECT_ATTACK_UP2 - EFFECT_ATTACK_UP1);
        }

    side_effect:
        if (sBattleTarget->statChanges[move_effect_index] != 1) {
            sBattleTarget->statChanges[move_effect_index] -= amount;
            if (sBattleTarget->statChanges[move_effect_index] <= 0) {
                sBattleTarget->statChanges[move_effect_index] = 1;
            }

            speed_modified = 0;
            attack_modified = 0;

            if (move_effect_index < 4) {
                switch (move_effect_index) {
                    case 0:
                        attack_modified = 1;
                        unmodified_stat_value = &sBattleTarget->unmodifiedAttack;
                        modified_stat_value = &sBattleTarget->modifiedAttack;
                        break;

                    case 1:
                        unmodified_stat_value = &sBattleTarget->unmodifiedDefense;
                        modified_stat_value = &sBattleTarget->modifiedDefense;
                        break;

                    case 2:
                        speed_modified = 1;
                        unmodified_stat_value = &sBattleTarget->unmodifiedSpeed;
                        modified_stat_value = &sBattleTarget->modifiedSpeed;
                        break;

                    case 3:
                        unmodified_stat_value = &sBattleTarget->unmodifiedSpecial;
                        modified_stat_value = &sBattleTarget->modifiedSpecial;
                        break;
                }

                if (*modified_stat_value != 1) {
                    *modified_stat_value = (*unmodified_stat_value * sStatStageMultipliers[sBattleTarget->statChanges[move_effect_index] - 1].multiplier) /
                            sStatStageMultipliers[sBattleTarget->statChanges[move_effect_index] - 1].devisor;
                    if (*modified_stat_value == 0) {
                        *modified_stat_value = 1;
                    }
                } else {
                    goto nothing_happened;
                }
            }

            if (amount == 2) {
                Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                Battle_QueueMessage(sBattleMessageQueues->buffer[7], move_effect_index + 0x9A);
            } else {
                Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                Battle_QueueMessage(sBattleMessageQueues->buffer[7], move_effect_index + 0xA0);
            }

            if ((speed_modified != 0) || (sBattleMechanicsSetting[0] != 1)) {
                ApplyParalysisSpeedCut(sActivePokemon[!sBattleScene.scene->actingSide]);
            }

            if ((attack_modified != 0) || (sBattleMechanicsSetting[0] != 1)) {
                ApplyBurnAttackCut(sActivePokemon[!sBattleScene.scene->actingSide]);
            }
            return;
        }
    }

nothing_happened:
    if (sBattleUser->moveCache.effect < EFFECT_ATTACK_DOWN_33_PERCENT) {
        sConfused = 0;
        sStatCanBeChangedFurther = 0;
        // Nothing happened!
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x5A);
        return;
    }

move_failed:
    if (sBattleUser->moveCache.effect < EFFECT_ATTACK_DOWN_33_PERCENT) {
        sConfused = 0;
        sStatCanBeChangedFurther = 0;
        if (D_843C4DA9 == 0) {
            // But, it failed!
            Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x59);
        }
    } else {
        sStatCanBeChangedFurther = 0;
    }
}

void Battle_Effect_Bide(void) {
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_BIDE;
    sBattleUser->moveCache.effect = 0;
    sBattleUser->immobilizedReason |= 1;
    sBattleUser->multihitCount = 0;
    sBattleUser->isTrapped = (Battle_Random() & 1) + 2;
}

void Battle_Effect_Thrash(void) {
    sBattleUser->moveCache.effect = 0;
    sBattleUser->immobilizedReason |= 2;
    sBattleUser->isTrapped = (Battle_Random() & 1) + 2;
}

// Better name: BattleEffect_WildBattleOnlyMoves
/*
    Handles Teleport, Roar, and Whirlwind which do nothing in stadium 
    since the moves only have effect in wild battles in gen 1
*/
void Battle_Effect_ForceSwitchOrFlee(void) {
    if (sBattleUser->moveCache.moveId == TELEPORT) {
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_TELEPORT;
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x53);
        return;
    }
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_FAILED;
    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
    // But, it failed!
    Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x52);
}

void Battle_Effect_MultiHit(void) {
    s32 rng;

    if (!(sBattleUser->immobilizedReason & IMMOBILIZED_MULTI_HIT)) {
        sBattleUser->immobilizedReason |= IMMOBILIZED_MULTI_HIT;

        switch (sBattleUser->moveCache.effect) {
            case EFFECT_TWINEEDLE:
                sBattleUser->moveCache.effect = EFFECT_POISON_20_PERCENT;

            case EFFECT_ATTACK_TWICE:
                sBattleUser->multihitCount = 2;
                sBattleUser->isTrapped = sBattleUser->multihitCount;
                break;

            default:
                rng = Battle_Random() & 3;
                if (rng >= 2) {
                    rng = Battle_Random() & 3;
                }
                sBattleUser->isTrapped = sBattleUser->multihitCount = rng + 2;
                break;
        }

        sMultihitCount = sBattleUser->multihitCount - 1;
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_MULTI_HIT;
    }
}

void Battle_Effect_Flinch(void) {
    // Burnt Pokemon can't be flinched?
    // Maybe handles it so burn damage still applies?
    if (!(sBattleTarget->statusCondition & STATUS_BURN)) {
        if (sBattleMechanicsSetting[0] == STADIUM) {
            if (sBattleUser->moveCache.effect == EFFECT_FLINCH_10_PERCENT) {
                if (Battle_Random() >= 26) {
                    return;
                }
            } else if (Battle_Random() >= 77) {
                return;
            }
        } else {
            sBattleTarget->statusCondition &= ~STATUS_FREEZE;
            if (sBattleUser->moveCache.effect == EFFECT_FLINCH_10_PERCENT) {
                if (Battle_Random() >= 26) {
                    return;
                }
            } else if (Battle_Random() >= 77) {
                return;
            }
        }
        sBattleTarget->immobilizedReason |= IMMOBILIZED_FLINCHED;
    }
}

void Battle_Effect_OHKO(void) {
    sBattleDamage = 0;
    sBattleCritFlag = 0xFF;
    if (sBattleUser->modifiedSpeed < sBattleTarget->modifiedSpeed) {
        sBattleMoveFailed = 1;
        return;
    }
    sBattleDamage = 0xFFFF;
    sBattleCritFlag = 2;
}

void Battle_Effect_TwoTurnMove(void) {
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_CHARGE;
    sBattleUser->immobilizedReason |= IMMOBILIZED_CHARGING;

    if ((sBattleUser->moveCache.effect == EFFECT_FLY) || (sBattleUser->moveCache.moveId == DIG)) {
        sBattleUser->immobilizedReason |= IMMOBILIZED_SEMI_INVULNERABLE;
        if (sBattleUser->moveCache.moveId == DIG) {
            sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_DIG_START;
        }

        if (sBattleUser->moveCache.effect == EFFECT_FLY) {
            sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_FLY_START;
        }
    }

    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));

    switch (sBattleUser->moveCache.moveId) {
        case RAZOR_WIND:
            Battle_QueueMessage(sBattleMessageQueues->buffer[2], -0x51);
            break;

        case SOLARBEAM:
            Battle_QueueMessage(sBattleMessageQueues->buffer[2], -0x50);
            break;

        case SKULL_BASH:
            Battle_QueueMessage(sBattleMessageQueues->buffer[2], -0x4F);
            break;

        case SKY_ATTACK:
            Battle_QueueMessage(sBattleMessageQueues->buffer[2], -0x4E);
            break;

        case FLY:
            Battle_QueueMessage(sBattleMessageQueues->buffer[4], -0x4D);
            break;

        case DIG:
            Battle_QueueMessage(sBattleMessageQueues->buffer[4], -0x4C);
            break;
    }
}

void Battle_Effect_Trap(void) {
    s32 rng;

    if (!(sBattleUser->immobilizedReason & STATUS_FREEZE) &&
        ((sBattleMechanicsSetting[0] != STADIUM) || (sBattleUser->moveCache.type != NORMAL) ||
         ((sBattleTarget->types[0] != GHOST) && (sBattleTarget->types[1] != GHOST)))) {
        sBattleTarget->statusCondition &= (0xFFFF & ~STATUS_FREEZE);
        sBattleUser->immobilizedReason |= STATUS_FREEZE;
        rng = Battle_Random() & 3;
        if (rng >= 2) {
            rng = Battle_Random() & 3;
        }
        sBattleUser->isTrapped = rng + 1;
    }
}

void Battle_Effect_Mist(void) {
    if (sBattleUser->statusCondition & STATUS_MIST) {
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_SELF_TARGET;
        // But, it failed!
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x4B);
        return;
    }
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_SELF_TARGET;
    sBattleUser->statusCondition |= STATUS_MIST;
    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
    Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x4A);
}

void Battle_Effect_FocusEnergy(void) {
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_SELF_TARGET;
    if (sBattleUser->statusCondition & STATUS_FOCUS_ENERGY) {
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x49);
        return;
    }
    sBattleUser->statusCondition |= STATUS_FOCUS_ENERGY;
    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
    Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x48);
}

void Battle_Effect_Recoil(void) {
    s32 recoil_amount;

    sBattleUser->currentHp = sBattleUser->displayHp;

    if (sBattleUser->moveCache.moveId == STRUGGLE) {
        recoil_amount = sBattleDamage >> 1;
    } else {
        recoil_amount = sBattleDamage >> 2;
    }

    if (recoil_amount == 0) {
        recoil_amount += 1;
    }

    if (sBattleUser->currentHp < recoil_amount) {
        sBattleUser->currentHp = 0;
    } else {
        sBattleUser->currentHp -= recoil_amount;
    }

    sBattleTarget->currentHp = sBattleTarget->displayHp;
    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
    Battle_QueueMessage(sBattleMessageQueues->buffer[15], -0x47);
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_RECOIL;
}

void Battle_Effect_ConfuseChance(void) {
    if (!(sBattleTarget->statusCondition & STATUS_BURN) && (Battle_Random() < 26) && !(sBattleTarget->immobilizedReason & IMMOBILIZED_CONFUSED)) {
        sBattleScene.scene->hitReactionKind = BATTLE_HIT_REACTION_SECONDARY;
        sStatCanBeChangedFurther = 0;
        sBattleTarget->immobilizedReason |= IMMOBILIZED_CONFUSED;
        sBattleTarget->confusedCounter = (Battle_Random() & 3) + 2;
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[7], -0x46);
    }
}

void Battle_Effect_Confuse(void) {
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_TARGET_STATUS;
    sConfused = 0;

    if (!(sBattleTarget->statusCondition & STATUS_BURN)) {
        HandleMoveEffectFlags();
        if ((sBattleMoveFailed == 0) && !(sBattleTarget->immobilizedReason & IMMOBILIZED_CONFUSED)) {
            sConfused = 1;
            sBattleTarget->immobilizedReason |= IMMOBILIZED_CONFUSED;
            sBattleTarget->confusedCounter = (Battle_Random() & 3) + 2;
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x45);
            return;
        }
    }

    if (D_843C4DA9 == 0) {
        // But, it failed!
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x44);
    }
}

void Battle_Effect_Paralyze(void) {
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_TARGET_STATUS;
    if (sBattleTarget->pendingStatusCondition == 0) {
        if ((sBattleUser->moveCache.type != ELECTRIC) ||
            ((sBattleTarget->types[0] != GROUND) && (sBattleTarget->types[1] != GROUND))) {
            HandleMoveEffectFlags();
            if (sBattleMoveFailed == 0) {
                sBattleTarget->pendingStatusCondition |= STATUS_PARALYSIS;
                ApplyParalysisSpeedCut(sActivePokemon[!sBattleScene.scene->actingSide]);
                Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x43);
                return;
            }
        } else {
            sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_FAILED;
            sConfused = 0;
            Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x42);
            return;
        }
    }

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_FAILED;
    sConfused = 0;
    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
    Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x41);
}

void Battle_Effect_Substitute(void) {
    s32 substitute_health;

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_NO_ACTION;
    if (sBattleUser->statusCondition & STATUS_BURN) {
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], BATTLE_MSG_SUBSTITUTE_EXISTS);
        return;
    }

    substitute_health = sBattleUser->maxHp >> 2;
    if (substitute_health >= sBattleUser->displayHp) {
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], BATTLE_MSG_SUBSTITUTE_LOW_HP);
        return;
    }

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_SUBSTITUTE;
    sBattleUser->substituteHp = substitute_health;
    sBattleUser->displayHp -= substitute_health;
    sBattleUser->statusCondition |= STATUS_BURN;
    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
    Battle_QueueMessage(sBattleMessageQueues->buffer[1], BATTLE_MSG_SUBSTITUTE_CREATED);
}

void Battle_Effect_Recharge(void) {
    sBattleUser->statusCondition |= STATUS_FREEZE;
}

void Battle_Effect_Rage(void) {
    sBattleUser->statusCondition |= STATUS_PARALYSIS;
}

void Battle_Effect_Mimic(void) {
    ActivePokemonState* sp24 = &sActivePokemon[sBattleScene.scene->actingSide]->state;
    s32 var_t0;
    u8 temp_v0;

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_SELF_TARGET;
    HandleMoveEffectFlags();

    if ((sBattleMoveFailed == 0) && !(sBattleTarget->immobilizedReason & IMMOBILIZED_SEMI_INVULNERABLE)) {
        D_843C4DAA = 1;
        do {
            var_t0 = BattleAnim_RandomRange(4);
            if (var_t0 >= 4) {
                var_t0 = 3;
            }
            temp_v0 = sBattleTarget->moves[var_t0];
        } while (temp_v0 == 0);

        sBattleUser->moves[sp24->selectedMoveIndex] = temp_v0;
        Text_SetStringToken(0x1E, Text_GetString(NULL, 0, sMoveNameTable, sBattleTarget->moves[var_t0] - 1));
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x40);
        return;
    }

    Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x3F);
}

void Battle_Effect_LeechSeed(void) {
    HandleMoveEffectFlags();

    if ((sBattleMoveFailed == 0) && (sBattleTarget->types[0] != GRASS) && (sBattleTarget->types[1] != GRASS)) {
        if (!(sBattleTarget->statusCondition & STATUS_LEECH_SEED)) {
            sBattleTarget->statusCondition |= STATUS_LEECH_SEED;
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x3E);
            return;
        }
    }

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_FAILED;
    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
    Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x3D);
}

void Battle_Effect_Splash(void) {
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_SELF_TARGET;
    Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x3C);
}

void Battle_Effect_Disable(void) {
    s32 move_slot_to_disable;
    s32 disable_duration;

    HandleMoveEffectFlags();

    if (sBattleMoveFailed == 0) {
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_DISABLE;
        if (sBattleTarget->movesDisabled == 0) {
            do {
                move_slot_to_disable = Battle_Random() & 3;
            } while (sBattleTarget->moves[move_slot_to_disable] == 0);
            disable_duration = (Battle_Random() & 7) + 1;
            sBattleTarget->movesDisabled = (move_slot_to_disable * 0x10) + disable_duration;
            sBattleTarget->disabledMove = sBattleTarget->moves[move_slot_to_disable];
            Text_SetStringToken(0x1E, Text_GetString(NULL, 0, sMoveNameTable, sBattleTarget->disabledMove - 1));
            Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x3B);
            return;
        }
    }

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_FAILED;
    Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x3A);
}

void Battle_Effect_PayDay(void) {
    Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x39);
}

void Battle_Effect_Conversion(void) {
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_SELF_TARGET;
    if (!(sBattleTarget->immobilizedReason & IMMOBILIZED_SEMI_INVULNERABLE)) {
        sBattleUser->types[0] = sBattleTarget->types[0];
        sBattleUser->types[1] = sBattleTarget->types[1];
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x38);
    } else {
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x37);
    }
}

void Battle_Effect_Haze(void) {
    s32 i;
    BattleMonRuntime* pokemon_runtime1 = &sActivePokemon[0]->state.runtime;
    BattleMonRuntime* pokemon_runtime2 = &sActivePokemon[1]->state.runtime;

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_HAZE;

    for (i = 0; i < 8; i++) {
        pokemon_runtime1->statChanges[i] = 7;
        pokemon_runtime2->statChanges[i] = 7;
    }

    pokemon_runtime1->modifiedAttack = pokemon_runtime1->unmodifiedAttack;
    pokemon_runtime1->modifiedDefense = pokemon_runtime1->unmodifiedDefense;
    pokemon_runtime1->modifiedSpeed = pokemon_runtime1->unmodifiedSpeed;
    pokemon_runtime1->modifiedSpecial = pokemon_runtime1->unmodifiedSpecial;
    pokemon_runtime2->modifiedAttack = pokemon_runtime2->unmodifiedAttack;
    pokemon_runtime2->modifiedDefense = pokemon_runtime2->unmodifiedDefense;
    pokemon_runtime2->modifiedSpeed = pokemon_runtime2->unmodifiedSpeed;
    pokemon_runtime2->modifiedSpecial = pokemon_runtime2->unmodifiedSpecial;

    if ((sBattleTarget->pendingStatusCondition & (STATUS_FREEZE | STATUS_SLEEP_MASK)) && !(sBattleTarget->immobilizedReason & IMMOBILIZED_CHARGING)) {
        sBattleTarget->usedMove = 0xFE;
    }

    sBattleTarget->pendingStatusCondition = 0;
    // clang-format off
    if (sBattleMechanicsSetting[0] == 1) { sBattleUser->pendingStatusCondition = 0; }
    // clang-format on

    pokemon_runtime1->movesDisabled = pokemon_runtime2->movesDisabled = 0;
    pokemon_runtime1->disabledMove = pokemon_runtime2->disabledMove = 0;
    pokemon_runtime1->immobilizedReason &= ~IMMOBILIZED_CONFUSED;
    pokemon_runtime1->statusCondition &= (STATUS_POISON | STATUS_FREEZE | STATUS_BURN | STATUS_PARALYSIS);
    pokemon_runtime1->uniqueStatusFlags &= 8;
    pokemon_runtime2->immobilizedReason &= ~IMMOBILIZED_CONFUSED;
    pokemon_runtime2->statusCondition &= (STATUS_POISON | STATUS_FREEZE | STATUS_BURN | STATUS_PARALYSIS);
    pokemon_runtime2->uniqueStatusFlags &= 8;
    sBattleUser->scaleResize = 0;
    sBattleTarget->scaleResize = 0;

    Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x36);
}

void Battle_Effect_Heal(void) {
    u8 pending_status_condition;
    ActivePokemonState* state;
    BattleMonRuntime* runtime;

    state = &sActivePokemon[sBattleScene.scene->actingSide]->state;
    runtime = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_HEAL;
    if (sBattleUser->displayHp != sBattleUser->maxHp) {
        if (sBattleUser->moveCache.moveId == REST) {
            pending_status_condition = sBattleUser->pendingStatusCondition;

            sBattleUser->pendingStatusCondition = 2;
            runtime->roster[state->rosterIndex + state->teamMemberIndex * 3] = 2;

            if (pending_status_condition != 0) {
                if (sBattleMechanicsSetting[0] == 1) {
                    sBattleUser->uniqueStatusFlags &= ~1;
                    Battle_ApplyStatStageMultiplier(&sBattleUser->modifiedAttack, sBattleUser->unmodifiedAttack, sBattleUser->statChanges[0]);
                    Battle_ApplyStatStageMultiplier(&sBattleUser->modifiedSpeed, sBattleUser->unmodifiedSpeed, sBattleUser->statChanges[2]);
                }
                Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
                Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x35);
            } else {
                Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
                Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x34);
            }
            sBattleUser->displayHp += sBattleUser->maxHp;
        } else {
            sBattleUser->displayHp += sBattleUser->maxHp / 2;
        }

        if (sBattleUser->maxHp < sBattleUser->displayHp) {
            sBattleUser->displayHp = sBattleUser->maxHp;
        }

        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[4], -0x33);
        return;
    }

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_NO_ACTION;
    sBattleMoveFailed = 1;
    Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x32);
}

void Battle_Effect_Transform(void) {
    s32 i;
    ActivePokemonState* pokemon1_state;
    ActivePokemonState* pokemon2_state;
    BattleMonRuntime* user;

    pokemon1_state = &sActivePokemon[sBattleScene.scene->actingSide]->state;
    pokemon2_state = &sActivePokemon[!sBattleScene.scene->actingSide]->state;

    if ((sBattleMechanicsSetting[0] == STADIUM) && ((((sBattleTarget->uniqueStatusFlags & 8) == 0) && (sBattleTarget->id == 0x84)) ||
                                 (((sBattleTarget->uniqueStatusFlags & 8) != 0) && (pokemon2_state->transformedIntoSpecies == 0x84)))) {
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_NO_ACTION;
        Text_SetStringToken(0x1B, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], BATTLE_MSG_TRANSFORM_DITTO_BLOCKED);
        return;
    }

    user = sBattleUser;
    if (!(user->immobilizedReason & STATUS_PARALYSIS) &&
        ((sBattleMechanicsSetting[0] != STADIUM) || (sBattleTarget->moves[0] != TRANSFORM) || (sBattleTarget->moves[1] != 0))) {
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_TRANSFORM;
        Text_SetStringToken(0x1B, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[4], -0x31);
        sBattleUser->uniqueStatusFlags |= UNIQUE_STATUS_TRANSFORM;

        if (sBattleTarget->uniqueStatusFlags & UNIQUE_STATUS_TRANSFORM) {
            pokemon1_state->transformedIntoSpecies = pokemon2_state->transformedIntoSpecies;
        } else {
            pokemon1_state->transformedIntoSpecies = sBattleTarget->id;
        }

        sBattleUser->types[0] = sBattleTarget->types[0];
        sBattleUser->types[1] = sBattleTarget->types[1];
        sBattleUser->moves[0] = sBattleTarget->moves[0];
        sBattleUser->moves[1] = sBattleTarget->moves[1];
        sBattleUser->moves[2] = sBattleTarget->moves[2];
        sBattleUser->moves[3] = sBattleTarget->moves[3];
        sBattleUser->unk_24 = sBattleTarget->unk_24;
        sBattleUser->modifiedAttack = sBattleTarget->modifiedAttack;
        sBattleUser->modifiedDefense = sBattleTarget->modifiedDefense;
        sBattleUser->modifiedSpeed = sBattleTarget->modifiedSpeed;
        sBattleUser->modifiedSpecial = sBattleTarget->modifiedSpecial;

        for (i = 0; i < 4; i++) {
            if (sBattleTarget->moves[i] == 0) {
                break;
            }
            sBattleUser->pp[i] = 5;
        }

        sBattleUser->unmodifiedAttack = sBattleTarget->unmodifiedAttack;
        sBattleUser->unmodifiedDefense = sBattleTarget->unmodifiedDefense;
        sBattleUser->unmodifiedSpeed = sBattleTarget->unmodifiedSpeed;
        sBattleUser->unmodifiedSpecial = sBattleTarget->unmodifiedSpecial;

        for (i = 0; i < 8; i++) {
            sBattleUser->statChanges[i] = sBattleTarget->statChanges[i];
        }
        return;
    }

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_NO_ACTION;
    Battle_QueueMessage(sBattleMessageQueues->buffer[1], BATTLE_MSG_TRANSFORM_FAILED);
}

void Battle_Effect_ReflectLightScreen(void) {
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_SELF_TARGET;
    if (sBattleUser->moveCache.effect == EFFECT_LIGHT_SCREEN) {
        if (!(sBattleUser->uniqueStatusFlags & UNIQUE_STATUS_LIGHT_SCREEN)) {
            sBattleUser->uniqueStatusFlags |= UNIQUE_STATUS_LIGHT_SCREEN;
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[1], BATTLE_MSG_LIGHT_SCREEN_SET);
        } else {
            Battle_QueueMessage(sBattleMessageQueues->buffer[1], BATTLE_MSG_LIGHT_SCREEN_FAILED);
        }
    } else if (!(sBattleUser->uniqueStatusFlags & UNIQUE_STATUS_REFLECT)) {
        sBattleUser->uniqueStatusFlags |= UNIQUE_STATUS_REFLECT;
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], BATTLE_MSG_REFLECT_SET);
    } else {
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], BATTLE_MSG_REFLECT_FAILED);
    }
}

void Battle_RunMoveEffect(void) {
    BattleMonRuntime* runtime = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;
    s32 idx = runtime->moveCache.effect;

    if (sMoveEffectHandlers[idx - 1] != NULL) {
        sMoveEffectHandlers[idx - 1]();
    }
}

void Battle_ExecuteMoveEffect(void) {
    Battle_RunMoveEffect();
}
