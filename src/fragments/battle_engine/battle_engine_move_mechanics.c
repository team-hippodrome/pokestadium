#include "battle_engine.h"
#include "moves.h"
#include "src/text_system.h"
#include "status.h"
#include "types.h"

/*
 * Move-effect dispatch and runtime mutations. Effect IDs come from gMoveData;
 * the table below is indexed by effect - 1, including unused dispatch slots.
 * //@src/gb_data.c
 * Turn continuations and damage application live in battle_engine_mechanics.c.
 * //@src/fragments/battle_engine/battle_engine_mechanics.c
 */
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
    BattleEffect_Sleep,
    BattleEffect_Poison,
    BattleEffect_DrainHP,
    BattleEffect_SecondaryStatusChance,
    BattleEffect_SecondaryStatusChance,
    BattleEffect_SecondaryStatusChance,
    BattleEffect_Explosion,
    BattleEffect_DrainHP,
    NULL,
    BattleEffect_RaiseOwnStat,
    BattleEffect_RaiseOwnStat,
    BattleEffect_RaiseOwnStat,
    BattleEffect_RaiseOwnStat,
    BattleEffect_RaiseOwnStat,
    BattleEffect_RaiseOwnStat,
    BattleEffect_PayDay,
    NULL,
    BattleEffect_LowerStat,
    BattleEffect_LowerStat,
    BattleEffect_LowerStat,
    BattleEffect_LowerStat,
    BattleEffect_LowerStat,
    BattleEffect_LowerStat,
    BattleEffect_Conversion,
    BattleEffect_Haze,
    BattleEffect_Bide,
    BattleEffect_Thrash,
    BattleEffect_WildBattleOnlyMoves,
    BattleEffect_MultiHit,
    BattleEffect_MultiHit,
    BattleEffect_Flinch,
    BattleEffect_Sleep,
    BattleEffect_Poison,
    BattleEffect_SecondaryStatusChance,
    BattleEffect_SecondaryStatusChance,
    BattleEffect_SecondaryStatusChance,
    BattleEffect_Flinch,
    BattleEffect_OHKO,
    BattleEffect_TwoTurnMove,
    NULL,
    NULL,
    BattleEffect_Trap,
    BattleEffect_TwoTurnMove,
    BattleEffect_MultiHit,
    NULL,
    BattleEffect_Mist,
    BattleEffect_FocusEnergy,
    BattleEffect_Recoil,
    BattleEffect_Confuse,
    BattleEffect_RaiseOwnStat,
    BattleEffect_RaiseOwnStat,
    BattleEffect_RaiseOwnStat,
    BattleEffect_RaiseOwnStat,
    BattleEffect_RaiseOwnStat,
    BattleEffect_RaiseOwnStat,
    BattleEffect_Heal,
    BattleEffect_Transform,
    BattleEffect_LowerStat,
    BattleEffect_LowerStat,
    BattleEffect_LowerStat,
    BattleEffect_LowerStat,
    BattleEffect_LowerStat,
    BattleEffect_LowerStat,
    BattleEffect_ReflectLightScreen,
    BattleEffect_ReflectLightScreen,
    BattleEffect_Poison,
    BattleEffect_Paralyze,
    BattleEffect_LowerStat,
    BattleEffect_LowerStat,
    BattleEffect_LowerStat,
    BattleEffect_LowerStat,
    BattleEffect_LowerStat,
    BattleEffect_LowerStat,
    BattleEffect_LowerStat,
    BattleEffect_LowerStat,
    BattleEffect_ConfuseChance,
    BattleEffect_MultiHit,
    NULL,
    BattleEffect_Substitute,
    BattleEffect_Recharge,
    BattleEffect_Rage,
    BattleEffect_Mimic,
    NULL,
    BattleEffect_LeechSeed,
    BattleEffect_Splash,
    BattleEffect_Disable,
    NULL,
};

/* Rebuild one modified stat from its base value and 1-based stage, clamped to 1-999. */
u16 BattleEffect_ApplyStatStageMultiplier(u16* stat_ptr, u16 base_stat, u8 stat_stage) {
    *stat_ptr = (base_stat * sStatStageMultipliers[stat_stage - 1].multiplier) / sStatStageMultipliers[stat_stage - 1].divisor;

    if (*stat_ptr == 0) {
        *stat_ptr = 1;
    }

    if (*stat_ptr > 999) {
        *stat_ptr = 999;
    }

    return *stat_ptr;
}

/* Apply a sleep duration, checking the sleep clause and the target's recharge state. */
// JP: register allocation differs; the C below is otherwise equivalent.
#if defined(VERSION_JP) && !defined(NON_MATCHING)
#pragma GLOBAL_ASM("asm/jp/nonmatchings/fragments/battle_engine/battle_engine_move_mechanics/BattleEffect_Sleep.s")
#else
void BattleEffect_Sleep(void) {
    s32 was_recharging;
    s32 sleep_turns;

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_TARGET_STATUS;
    was_recharging = sBattleTarget->volatileStatusFlags & VOLATILE_STATUS_RECHARGE;
    sBattleTarget->volatileStatusFlags &= ~VOLATILE_STATUS_RECHARGE;

    if (was_recharging == 0) {
        if (sBattleTarget->pendingStatusCondition & STATUS_SLEEP_MASK) {
            sConfused = 0;
            BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[1], -0x80, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
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
        BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[1], -0x7F, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        return;
    }

block_9:
    if ((Battle_SleepClauseInEffect(sActivePokemon[!sBattleScene.scene->actingSide]) != 0) && (sBattleMechanicsSetting[0] != 2)) {
        sConfused = 0;
        BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[1], -0x7F, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        if ((was_recharging != 0) && (sBattleMechanicsSetting[0] == 1)) {
            sBattleTarget->volatileStatusFlags |= VOLATILE_STATUS_RECHARGE;
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
        BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[1], -0x7E, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
    }
}
#endif

/* Apply poison or Toxic, or roll the secondary poison chance of a damaging move. */
void BattleEffect_Poison(void) {
    char* buffer = sBattleMessageQueues->buffer[1];

    if (!(sBattleTarget->volatileStatusFlags & VOLATILE_STATUS_SUBSTITUTE) && (sBattleTarget->pendingStatusCondition == 0) && (sBattleTarget->types[0] != POISON) &&
        (sBattleTarget->types[1] != POISON)) {
        switch (sBattleUser->moveCache.effect) {
            case EFFECT_POISON_20_PERCENT:
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

            case EFFECT_POISON_40_PERCENT:
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
                    BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                    BATTLE_QUEUE_MESSAGE1(buffer, -0x7D, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                    return;
                }
                break;
        }

        sBattleTarget->pendingStatusCondition |= STATUS_POISON;
        if (sBattleUser->moveCache.moveId == TOXIC) {
            sBattleTarget->uniqueStatusFlags |= UNIQUE_STATUS_TOXIC;
            sBattleTarget->toxicCounter = 0;
            BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            BATTLE_QUEUE_MESSAGE1(buffer, -0x7C, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        } else {
            BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            BATTLE_QUEUE_MESSAGE1(buffer, -0x7B, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        }
    } else if (sBattleUser->moveCache.effect == EFFECT_POISON) {
        if (sBattleScene.scene->moveEffectKind != MOVE_PRESENTATION_MULTI_HIT) {
            sBattleMoveFailed = 1;
            sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_TARGET_STATUS;
            sConfused = 0;
        }
        BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        BATTLE_QUEUE_MESSAGE1(buffer, -0x7A, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
    }
}

/* Restore half the dealt damage, at least one HP, for draining moves and Dream Eater. */
void BattleEffect_DrainHP(void) {
    sBattleDamage /= 2;
    if (sBattleDamage == 0) {
        sBattleDamage = 1;
    }

    if (sBattleUser->maxHp < sBattleUser->displayHp + sBattleDamage) {
        sBattleUser->displayHp = sBattleUser->maxHp;
    } else {
        sBattleUser->displayHp += sBattleDamage;
    }

    if (sBattleUser->moveCache.effect == EFFECT_DREAM_EATER) {
        BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[7], -0x79, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
    } else {
        BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[7], -0x78, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
    }
}

/* Faint the user of Explosion/Self-Destruct and clear its major status and Leech Seed. */
void BattleEffect_Explosion(void) {
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_EXPLOSION;
    sBattleUser->displayHp = 0;
    sBattleUser->pendingStatusCondition = 0;
    sBattleUser->volatileStatusFlags &= ~VOLATILE_STATUS_LEECH_SEED;
}

/* Roll burn/freeze/paralysis, or thaw a frozen target hit by a Fire-type move. */
void BattleEffect_SecondaryStatusChance(void) {
    BattleMonRuntime* runtime = &sActivePokemon[!sBattleScene.scene->actingSide]->state.runtime;
    ActivePokemonState* state = &sActivePokemon[!sBattleScene.scene->actingSide]->state;

    if (sBattleTarget->volatileStatusFlags & VOLATILE_STATUS_SUBSTITUTE) {
        return;
    }

    if (sBattleTarget->pendingStatusCondition != 0) {
        if ((sBattleTarget->pendingStatusCondition & STATUS_FREEZE) && (sBattleUser->moveCache.type == FIRE)) {
            sBattleTarget->pendingStatusCondition = 0;
            if (state->moveIndex != 4) {
                runtime->usedMove = runtime->moves[state->selectedMoveIndex];
            } else {
                state->moveIndex = 0;
                state->selectedMoveIndex = 0;
                runtime->usedMove = STRUGGLE;
            }
            BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            // The Ice melted!
            BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[1], -0x77, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
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
                BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[7], -0x76, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                break;

            case EFFECT_FREEZE_10_PERCENT:
            case EFFECT_FREEZE_30_PERCENT:
                if (Battle_FreezeClauseInEffect(sActivePokemon[!sBattleScene.scene->actingSide]) == 0) {
                    sBattleTarget->volatileStatusFlags &= ~VOLATILE_STATUS_RECHARGE;
                    sBattleTarget->pendingStatusCondition = STATUS_FREEZE;
                    BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                    BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[7], -0x75, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                }
                break;

            case EFFECT_PARALYZE_10_PERCENT:
            case EFFECT_PARALYZE_30_PERCENT:
                sBattleTarget->pendingStatusCondition = STATUS_PARALYSIS;
                ApplyParalysisSpeedCut(sActivePokemon[!sBattleScene.scene->actingSide]);
                BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[7], -0x74, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                break;
        }
    }
}

/* Apply the Rage follow-up stat boost, using its separate message queue. */
void BattleEffect_ApplyStatBoost(void) {
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
            //!bug The original code clamps increase_amount's entry instead of move_effect_index's.
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
                        sStatStageMultipliers[sBattleUser->statChanges[move_effect_index] - 1].divisor;
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
            BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            BATTLE_QUEUE_OFFSET_MESSAGE1(sBattleMessageQueues->buffer[10], move_effect_index, 0x8D, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        } else {
            BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            BATTLE_QUEUE_OFFSET_MESSAGE1(sBattleMessageQueues->buffer[10], move_effect_index, 0x93, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
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
        BATTLE_QUEUE_MESSAGE0(sBattleMessageQueues->buffer[10], -0x67);
    }
}

/* Raise the user's selected stat stage, update its value and reapply status penalties. */
void BattleEffect_RaiseOwnStat(void) {
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
                        sStatStageMultipliers[sBattleUser->statChanges[move_effect_index] - 1].divisor;
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
            BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            BATTLE_QUEUE_OFFSET_MESSAGE1(sBattleMessageQueues->buffer[1], move_effect_index, 0x8D, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
        } else {
            BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            BATTLE_QUEUE_OFFSET_MESSAGE1(sBattleMessageQueues->buffer[1], move_effect_index, 0x93, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
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
        BATTLE_QUEUE_MESSAGE0(sBattleMessageQueues->buffer[1], -0x67);
    }
}

/* Lower a target stat directly or by secondary-effect chance, with stage/value limits. */
void BattleEffect_LowerStat(void) {
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

    if (sBattleTarget->volatileStatusFlags & VOLATILE_STATUS_SUBSTITUTE) {
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
                            sStatStageMultipliers[sBattleTarget->statChanges[move_effect_index] - 1].divisor;
                    if (*modified_stat_value == 0) {
                        *modified_stat_value = 1;
                    }
                } else {
                    goto nothing_happened;
                }
            }

            if (amount == 2) {
                BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                BATTLE_QUEUE_OFFSET_MESSAGE1(sBattleMessageQueues->buffer[7], move_effect_index, 0x9A, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            } else {
                BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                BATTLE_QUEUE_OFFSET_MESSAGE1(sBattleMessageQueues->buffer[7], move_effect_index, 0xA0, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
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
        BATTLE_QUEUE_MESSAGE0(sBattleMessageQueues->buffer[1], -0x5A);
        return;
    }

move_failed:
    if (sBattleUser->moveCache.effect < EFFECT_ATTACK_DOWN_33_PERCENT) {
        sConfused = 0;
        sStatCanBeChangedFurther = 0;
        if (D_843C4DA9 == 0) {
            // But, it failed!
            BATTLE_QUEUE_MESSAGE0(sBattleMessageQueues->buffer[1], -0x59);
        }
    } else {
        sStatCanBeChangedFurther = 0;
    }
}

/* Start Bide with a cleared damage accumulator and a two- or three-turn duration. */
void BattleEffect_Bide(void) {
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_BIDE;
    sBattleUser->moveCache.effect = 0;
    sBattleUser->immobilizedReason |= IMMOBILIZED_BIDING;
    sBattleUser->effectCounter = 0;
    sBattleUser->effectTurnsLeft = (Battle_Random() & 1) + 2;
}

/* Lock Thrash/Petal Dance into a two- or three-turn continuation. */
void BattleEffect_Thrash(void) {
    sBattleUser->moveCache.effect = 0;
    sBattleUser->immobilizedReason |= IMMOBILIZED_THRASHING;
    sBattleUser->effectTurnsLeft = (Battle_Random() & 1) + 2;
}

/*
    Queue the trainer-battle failure response for Teleport, Roar and Whirlwind.
    Teleport has a separate presentation kind and message from the other two.
*/
void BattleEffect_WildBattleOnlyMoves(void) {
    if (sBattleUser->moveCache.moveId == TELEPORT) {
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_TELEPORT;
        BATTLE_QUEUE_MESSAGE0(sBattleMessageQueues->buffer[1], -0x53);
        return;
    }
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_FAILED;
    BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
    // But, it failed!
    BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[1], -0x52, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
}

/* Initialize total/remaining hits; Twineedle then uses the secondary poison effect. */
void BattleEffect_MultiHit(void) {
    s32 rng;

    if (!(sBattleUser->immobilizedReason & IMMOBILIZED_MULTI_HIT)) {
        sBattleUser->immobilizedReason |= IMMOBILIZED_MULTI_HIT;

        switch (sBattleUser->moveCache.effect) {
            case EFFECT_TWINEEDLE:
                sBattleUser->moveCache.effect = EFFECT_POISON_20_PERCENT;

            case EFFECT_ATTACK_TWICE:
                sBattleUser->effectCounter = 2;
                sBattleUser->effectTurnsLeft = sBattleUser->effectCounter;
                break;

            default:
                rng = Battle_Random() & 3;
                if (rng >= 2) {
                    rng = Battle_Random() & 3;
                }
                sBattleUser->effectTurnsLeft = sBattleUser->effectCounter = rng + 2;
                break;
        }

        sMultihitCount = sBattleUser->effectCounter - 1;
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_MULTI_HIT;
    }
}

/* Roll flinching unless Substitute blocks it; non-Stadium rules also clear recharge. */
void BattleEffect_Flinch(void) {
    if (!(sBattleTarget->volatileStatusFlags & VOLATILE_STATUS_SUBSTITUTE)) {
        if (sBattleMechanicsSetting[0] == STADIUM) {
            if (sBattleUser->moveCache.effect == EFFECT_FLINCH_10_PERCENT) {
                if (Battle_Random() >= 26) {
                    return;
                }
            } else if (Battle_Random() >= 77) {
                return;
            }
        } else {
            sBattleTarget->volatileStatusFlags &= ~VOLATILE_STATUS_RECHARGE;
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

/* Reject a slower OHKO user, otherwise set the damage sentinel and OHKO result. */
void BattleEffect_OHKO(void) {
    sBattleDamage = 0;
    sBattleCritFlag = 0xFF;
    if (sBattleUser->modifiedSpeed < sBattleTarget->modifiedSpeed) {
        sBattleMoveFailed = 1;
        return;
    }
    sBattleDamage = 0xFFFF;
    sBattleCritFlag = 2;
}

/* Start a charge turn, mark Fly/Dig semi-invulnerability and queue the move's message. */
void BattleEffect_TwoTurnMove(void) {
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

    BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));

    switch (sBattleUser->moveCache.moveId) {
        case RAZOR_WIND:
            BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[2], -0x51, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            break;

        case SOLARBEAM:
            BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[2], -0x50, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            break;

        case SKULL_BASH:
            BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[2], -0x4F, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            break;

        case SKY_ATTACK:
            BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[2], -0x4E, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            break;

        case FLY:
            BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[4], -0x4D, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            break;

        case DIG:
            BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[4], -0x4C, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            break;
    }
}

/* Start a trapping continuation, clearing target recharge and checking Normal/Ghost immunity. */
void BattleEffect_Trap(void) {
    s32 rng;

    if (!(sBattleUser->immobilizedReason & IMMOBILIZED_TRAPPING) &&
        ((sBattleMechanicsSetting[0] != STADIUM) || (sBattleUser->moveCache.type != NORMAL) ||
         ((sBattleTarget->types[0] != GHOST) && (sBattleTarget->types[1] != GHOST)))) {
        sBattleTarget->volatileStatusFlags &= (0xFFFF & ~VOLATILE_STATUS_RECHARGE);
        sBattleUser->immobilizedReason |= IMMOBILIZED_TRAPPING;
        rng = Battle_Random() & 3;
        if (rng >= 2) {
            rng = Battle_Random() & 3;
        }
        sBattleUser->effectTurnsLeft = rng + 1;
    }
}

/* Set Mist on the user, or queue failure when it is already active. */
void BattleEffect_Mist(void) {
    if (sBattleUser->volatileStatusFlags & VOLATILE_STATUS_MIST) {
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_SELF_TARGET;
        // But, it failed!
        BATTLE_QUEUE_MESSAGE0(sBattleMessageQueues->buffer[1], -0x4B);
        return;
    }
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_SELF_TARGET;
    sBattleUser->volatileStatusFlags |= VOLATILE_STATUS_MIST;
    BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
    BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[1], -0x4A, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
}

/* Set Focus Energy on the user, or queue failure when it is already active. */
void BattleEffect_FocusEnergy(void) {
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_SELF_TARGET;
    if (sBattleUser->volatileStatusFlags & VOLATILE_STATUS_FOCUS_ENERGY) {
        BATTLE_QUEUE_MESSAGE0(sBattleMessageQueues->buffer[1], -0x49);
        return;
    }
    sBattleUser->volatileStatusFlags |= VOLATILE_STATUS_FOCUS_ENERGY;
    BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
    BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[1], -0x48, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
}

/* Stage recoil HP loss: half damage for Struggle, otherwise one quarter, at least one HP. */
void BattleEffect_Recoil(void) {
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
    BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
    BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[15], -0x47, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_RECOIL;
}

/* Roll secondary confusion and initialize its duration unless Substitute blocks it. */
void BattleEffect_ConfuseChance(void) {
    if (!(sBattleTarget->volatileStatusFlags & VOLATILE_STATUS_SUBSTITUTE) && (Battle_Random() < 26) && !(sBattleTarget->immobilizedReason & IMMOBILIZED_CONFUSED)) {
        sBattleScene.scene->hitReactionKind = BATTLE_HIT_REACTION_SECONDARY;
        sStatCanBeChangedFurther = 0;
        sBattleTarget->immobilizedReason |= IMMOBILIZED_CONFUSED;
        sBattleTarget->confusionTurnsLeft = (Battle_Random() & 3) + 2;
        BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[7], -0x46, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
    }
}

/* Apply direct confusion after move-success checks, or queue its failure response. */
void BattleEffect_Confuse(void) {
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_TARGET_STATUS;
    sConfused = 0;

    if (!(sBattleTarget->volatileStatusFlags & VOLATILE_STATUS_SUBSTITUTE)) {
        HandleMoveEffectFlags();
        if ((sBattleMoveFailed == 0) && !(sBattleTarget->immobilizedReason & IMMOBILIZED_CONFUSED)) {
            sConfused = 1;
            sBattleTarget->immobilizedReason |= IMMOBILIZED_CONFUSED;
            sBattleTarget->confusionTurnsLeft = (Battle_Random() & 3) + 2;
            BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[1], -0x45, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            return;
        }
    }

    if (D_843C4DA9 == 0) {
        // But, it failed!
        BATTLE_QUEUE_MESSAGE0(sBattleMessageQueues->buffer[1], -0x44);
    }
}

/* Apply paralysis and its Speed cut, with the Electric/Ground immunity check. */
void BattleEffect_Paralyze(void) {
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_TARGET_STATUS;
    if (sBattleTarget->pendingStatusCondition == 0) {
        if ((sBattleUser->moveCache.type != ELECTRIC) ||
            ((sBattleTarget->types[0] != GROUND) && (sBattleTarget->types[1] != GROUND))) {
            HandleMoveEffectFlags();
            if (sBattleMoveFailed == 0) {
                sBattleTarget->pendingStatusCondition |= STATUS_PARALYSIS;
                ApplyParalysisSpeedCut(sActivePokemon[!sBattleScene.scene->actingSide]);
                BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[1], -0x43, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                return;
            }
        } else {
            sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_FAILED;
            sConfused = 0;
            BATTLE_QUEUE_MESSAGE0(sBattleMessageQueues->buffer[1], -0x42);
            return;
        }
    }

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_FAILED;
    sConfused = 0;
    BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
    BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[1], -0x41, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
}

/* Spend one quarter of maximum HP to create a Substitute when enough HP remains. */
void BattleEffect_Substitute(void) {
    s32 substitute_health;

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_NO_ACTION;
    if (sBattleUser->volatileStatusFlags & VOLATILE_STATUS_SUBSTITUTE) {
        BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
        BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[1], BATTLE_MSG_SUBSTITUTE_EXISTS, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
        return;
    }

    substitute_health = sBattleUser->maxHp >> 2;
    if (substitute_health >= sBattleUser->displayHp) {
        BATTLE_QUEUE_MESSAGE0(sBattleMessageQueues->buffer[1], BATTLE_MSG_SUBSTITUTE_LOW_HP);
        return;
    }

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_SUBSTITUTE;
    sBattleUser->substituteHp = substitute_health;
    sBattleUser->displayHp -= substitute_health;
    sBattleUser->volatileStatusFlags |= VOLATILE_STATUS_SUBSTITUTE;
    BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
    BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[1], BATTLE_MSG_SUBSTITUTE_CREATED, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
}

/* Mark the user as needing a recharge turn; Battle_CheckMonCanAct consumes the flag. */
void BattleEffect_Recharge(void) {
    sBattleUser->volatileStatusFlags |= VOLATILE_STATUS_RECHARGE;
}

/* Enable Rage's move lock and subsequent Attack boosts. */
void BattleEffect_Rage(void) {
    sBattleUser->volatileStatusFlags |= VOLATILE_STATUS_RAGE;
}

/* Replace the selected move slot with a randomly chosen nonempty target move. */
void BattleEffect_Mimic(void) {
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
        BATTLE_SET_MESSAGE_TOKEN(0x1E, Text_GetString(NULL, 0, sMoveNameTable, sBattleTarget->moves[var_t0] - 1));
        BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[1], -0x40, gMoveDisplayInfo[sBattleTarget->moves[var_t0] - 1].unk_04);
        return;
    }

    BATTLE_QUEUE_MESSAGE0(sBattleMessageQueues->buffer[1], -0x3F);
}

/* Seed a non-Grass target after move-success checks unless already seeded. */
void BattleEffect_LeechSeed(void) {
    HandleMoveEffectFlags();

    if ((sBattleMoveFailed == 0) && (sBattleTarget->types[0] != GRASS) && (sBattleTarget->types[1] != GRASS)) {
        if (!(sBattleTarget->volatileStatusFlags & VOLATILE_STATUS_LEECH_SEED)) {
            sBattleTarget->volatileStatusFlags |= VOLATILE_STATUS_LEECH_SEED;
            BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[1], -0x3E, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            return;
        }
    }

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_FAILED;
    BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
    BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[1], -0x3D, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
}

/* Select the self-target presentation and queue Splash's no-effect message. */
void BattleEffect_Splash(void) {
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_SELF_TARGET;
    BATTLE_QUEUE_MESSAGE0(sBattleMessageQueues->buffer[1], -0x3C);
}

/* Disable a random nonempty target move, packing its slot and duration into one byte. */
void BattleEffect_Disable(void) {
    s32 move_slot_to_disable;
    s32 disable_duration;

    HandleMoveEffectFlags();

    if (sBattleMoveFailed == 0) {
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_DISABLE;
        if (sBattleTarget->disabledMoveState == 0) {
            do {
                move_slot_to_disable = Battle_Random() & 3;
            } while (sBattleTarget->moves[move_slot_to_disable] == 0);
            disable_duration = (Battle_Random() & 7) + 1;
            sBattleTarget->disabledMoveState = (move_slot_to_disable * 0x10) + disable_duration;
            sBattleTarget->disabledMove = sBattleTarget->moves[move_slot_to_disable];
            BATTLE_SET_MESSAGE_TOKEN(0x1E, Text_GetString(NULL, 0, sMoveNameTable, sBattleTarget->disabledMove - 1));
            BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[1], -0x3B, gMoveDisplayInfo[sBattleTarget->disabledMove - 1].unk_04);
            return;
        }
    }

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_FAILED;
    BATTLE_QUEUE_MESSAGE0(sBattleMessageQueues->buffer[1], -0x3A);
}

/* Queue Pay Day's effect message; this handler does not update a money total. */
void BattleEffect_PayDay(void) {
    BATTLE_QUEUE_MESSAGE0(sBattleMessageQueues->buffer[1], -0x39);
}

/* Copy both target types to the user unless the target is semi-invulnerable. */
void BattleEffect_Conversion(void) {
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_SELF_TARGET;
    if (!(sBattleTarget->immobilizedReason & IMMOBILIZED_SEMI_INVULNERABLE)) {
        sBattleUser->types[0] = sBattleTarget->types[0];
        sBattleUser->types[1] = sBattleTarget->types[1];
        BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[1], -0x38, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
    } else {
        BATTLE_QUEUE_MESSAGE0(sBattleMessageQueues->buffer[1], -0x37);
    }
}

/* Reset stages/stats on both sides and clear the rule-dependent status/effect subsets. */
void BattleEffect_Haze(void) {
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

    pokemon_runtime1->disabledMoveState = pokemon_runtime2->disabledMoveState = 0;
    pokemon_runtime1->disabledMove = pokemon_runtime2->disabledMove = 0;
    pokemon_runtime1->immobilizedReason &= ~IMMOBILIZED_CONFUSED;
    pokemon_runtime1->volatileStatusFlags &= (0x08 | VOLATILE_STATUS_RECHARGE | VOLATILE_STATUS_SUBSTITUTE | VOLATILE_STATUS_RAGE);
    pokemon_runtime1->uniqueStatusFlags &= UNIQUE_STATUS_TRANSFORM;
    pokemon_runtime2->immobilizedReason &= ~IMMOBILIZED_CONFUSED;
    pokemon_runtime2->volatileStatusFlags &= (0x08 | VOLATILE_STATUS_RECHARGE | VOLATILE_STATUS_SUBSTITUTE | VOLATILE_STATUS_RAGE);
    pokemon_runtime2->uniqueStatusFlags &= UNIQUE_STATUS_TRANSFORM;
    sBattleUser->scaleResize = 0;
    sBattleTarget->scaleResize = 0;

    BATTLE_QUEUE_MESSAGE0(sBattleMessageQueues->buffer[1], -0x36);
}

/* Heal half maximum HP, or apply Rest's sleep and full heal, capped at maximum HP. */
void BattleEffect_Heal(void) {
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
                    sBattleUser->uniqueStatusFlags &= ~UNIQUE_STATUS_TOXIC;
                    BattleEffect_ApplyStatStageMultiplier(&sBattleUser->modifiedAttack, sBattleUser->unmodifiedAttack, sBattleUser->statChanges[0]);
                    BattleEffect_ApplyStatStageMultiplier(&sBattleUser->modifiedSpeed, sBattleUser->unmodifiedSpeed, sBattleUser->statChanges[2]);
                }
                BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
                BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[1], -0x35, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            } else {
                BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
                BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[1], -0x34, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            }
            sBattleUser->displayHp += sBattleUser->maxHp;
        } else {
            sBattleUser->displayHp += sBattleUser->maxHp / 2;
        }

        if (sBattleUser->maxHp < sBattleUser->displayHp) {
            sBattleUser->displayHp = sBattleUser->maxHp;
        }

        BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
        BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[4], -0x33, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
        return;
    }

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_NO_ACTION;
    sBattleMoveFailed = 1;
    BATTLE_QUEUE_MESSAGE0(sBattleMessageQueues->buffer[1], -0x32);
}

/* Copy the target's battle form, moves, stats and stages while preserving the user's HP. */
// JP: register allocation differs; the C below is otherwise equivalent.
#if defined(VERSION_JP) && !defined(NON_MATCHING)
#pragma GLOBAL_ASM("asm/jp/nonmatchings/fragments/battle_engine/battle_engine_move_mechanics/BattleEffect_Transform.s")
#else
void BattleEffect_Transform(void) {
    s32 i;
    ActivePokemonState* pokemon1_state;
    ActivePokemonState* pokemon2_state;
    BattleMonRuntime* user;

    pokemon1_state = &sActivePokemon[sBattleScene.scene->actingSide]->state;
    pokemon2_state = &sActivePokemon[!sBattleScene.scene->actingSide]->state;

    if ((sBattleMechanicsSetting[0] == STADIUM) && ((((sBattleTarget->uniqueStatusFlags & 8) == 0) && (sBattleTarget->id == 0x84)) ||
                                 (((sBattleTarget->uniqueStatusFlags & 8) != 0) && (pokemon2_state->transformedIntoSpecies == 0x84)))) {
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_NO_ACTION;
        BATTLE_SET_MESSAGE_TOKEN(0x1B, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        BATTLE_QUEUE_MESSAGE2(sBattleMessageQueues->buffer[1], BATTLE_MSG_TRANSFORM_DITTO_BLOCKED,
                              Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]),
                              Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        return;
    }

    user = sBattleUser;
    if (!(user->immobilizedReason & IMMOBILIZED_SEMI_INVULNERABLE) &&
        ((sBattleMechanicsSetting[0] != STADIUM) || (sBattleTarget->moves[0] != TRANSFORM) || (sBattleTarget->moves[1] != 0))) {
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_TRANSFORM;
        BATTLE_SET_MESSAGE_TOKEN(0x1B, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[4], -0x31, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
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
    BATTLE_QUEUE_MESSAGE0(sBattleMessageQueues->buffer[1], BATTLE_MSG_TRANSFORM_FAILED);
}
#endif

/* Set the appropriate independent screen bit, or queue the already-active response. */
void BattleEffect_ReflectLightScreen(void) {
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_SELF_TARGET;
    if (sBattleUser->moveCache.effect == EFFECT_LIGHT_SCREEN) {
        if (!(sBattleUser->uniqueStatusFlags & UNIQUE_STATUS_LIGHT_SCREEN)) {
            sBattleUser->uniqueStatusFlags |= UNIQUE_STATUS_LIGHT_SCREEN;
            BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[1], BATTLE_MSG_LIGHT_SCREEN_SET, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
        } else {
            BATTLE_QUEUE_MESSAGE0(sBattleMessageQueues->buffer[1], BATTLE_MSG_LIGHT_SCREEN_FAILED);
        }
    } else if (!(sBattleUser->uniqueStatusFlags & UNIQUE_STATUS_REFLECT)) {
        sBattleUser->uniqueStatusFlags |= UNIQUE_STATUS_REFLECT;
        BATTLE_SET_MESSAGE_TOKEN(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
        BATTLE_QUEUE_MESSAGE1(sBattleMessageQueues->buffer[1], BATTLE_MSG_REFLECT_SET, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
    } else {
        BATTLE_QUEUE_MESSAGE0(sBattleMessageQueues->buffer[1], BATTLE_MSG_REFLECT_FAILED);
    }
}

/* Dispatch the cached effect through its 1-based handler slot; NULL slots do nothing. */
void BattleEffect_RunMoveEffect(void) {
    BattleMonRuntime* runtime = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;
    s32 idx = runtime->moveCache.effect;

    if (sMoveEffectHandlers[idx - 1] != NULL) {
        sMoveEffectHandlers[idx - 1]();
    }
}

/* Entry point used by the turn engine to invoke the cached move-effect handler. */
void BattleEffect_ExecuteMoveEffect(void) {
    BattleEffect_RunMoveEffect();
}
