#ifndef BATTLE_STATE_FLAGS_H
#define BATTLE_STATE_FLAGS_H

/*
 * ActivePokemonState.flags
 *
 * Presentation state, not the authoritative gameplay status.
 * Proposed names describe observed setters and consumers.
 * Keep the struct member u16.
 */
#define ACTIVE_SLEEP 0x0001        /* Sleep presentation active. */
#define ACTIVE_FREEZE 0x0002       /* Freeze presentation active. */
#define ACTIVE_AIR_SETTLE 0x0004   /* Pending height = 200 and airborne-state setup. */
#define ACTIVE_RETURN_RESET 0x0008 /* Two-turn-move return cleanup pending. */
#define ACTIVE_DIG_EMERGE 0x0010   /* Diglett/Dugtrio emergence presentation. */

/* 0x0020, 0x0040, 0x0080: unassigned. */

#define ACTIVE_HOLD_RENDER 0x0100 /* Repeatedly clears model render flag 0x20. */
#define ACTIVE_AIRBORNE 0x0200    /* Airborne presentation active. */
#define ACTIVE_AIR_ACTION 0x0400  /* Action started while airborne. */
#define ACTIVE_SUBSTITUTE 0x0800  /* Substitute presentation active. */
#define ACTIVE_SUB_BREAK 0x1000   /* Substitute break presentation pending. */

/* 0x2000: unassigned. */

#define ACTIVE_UNDERGROUND 0x4000 /* Underground/Dig presentation active. */

/* 0x8000: unassigned. */

/*
 * BattleMon.statusCondition and BattleMonRuntime.pendingStatusCondition
 *
 * BattleEffect_Sleep stores the duration in bits 0-2; Battle_CheckMonCanAct
 * decrements that value. Battle_StoreMajorStatus copies the runtime byte
 * back to the picked Pokemon's statusCondition.
 * //@src/fragments/battle_engine/battle_engine_mechanics.c
 * //@src/fragments/battle_engine/battle_engine_move_mechanics.c
 * Toxic uses STATUS_POISON plus separate runtime state.
 * Fainting is not encoded as a status bit here.
 * Keep the struct member u8.
 */
#define STATUS_NONE 0x00
#define STATUS_SLEEP_MASK 0x07
#define STATUS_POISON 0x08
#define STATUS_BURN 0x10
#define STATUS_FREEZE 0x20
#define STATUS_PARALYSIS 0x40
/*
 * BattleMonRuntime.volatileStatusFlags (offset 0x4D).
 * These masks belong to a different byte from STATUS_* above.
 * Setters are the move-effect handlers; Battle_CheckMonCanAct consumes
 * recharge/Rage and Battle_ApplyDamage consumes Substitute.
 */
#define VOLATILE_STATUS_SKIP_ACCURACY 0x01
#define VOLATILE_STATUS_MIST 0x02
#define VOLATILE_STATUS_FOCUS_ENERGY 0x04
/* 0x08: meaning unresolved; retained by Haze. */
#define VOLATILE_STATUS_SUBSTITUTE 0x10
#define VOLATILE_STATUS_RECHARGE 0x20
#define VOLATILE_STATUS_RAGE 0x40
#define VOLATILE_STATUS_LEECH_SEED 0x80

/*
 * BattleMonRuntime.uniqueStatusFlags (offset 0x4E).
 * InitBattleParameters reads the screen bits when selecting defensive stats.
 */
#define UNIQUE_STATUS_TOXIC 0x01
#define UNIQUE_STATUS_LIGHT_SCREEN 0x02
#define UNIQUE_STATUS_REFLECT 0x04
#define UNIQUE_STATUS_TRANSFORM 0x08

/*
 * BattleMonRuntime.immobilizedReason (offset 0x4C)
 *
 * Volatile per-turn battle state. Despite the field name, only some bits block
 * the Pokemon from acting. Setters are the BattleEffect_* functions in
 * battle_engine_move_mechanics.c; consumers are in battle_engine_mechanics.c
 * (Battle_CheckMonCanAct and the move-effect turn handler).
 * Keep the struct member u8.
 */
#define IMMOBILIZED_BIDING 0x01    /* Bide damage in effectCounter; duration in effectTurnsLeft. */
#define IMMOBILIZED_THRASHING 0x02 /* Set by BattleEffect_Thrash; clears into IMMOBILIZED_CONFUSED. */
#define IMMOBILIZED_MULTI_HIT 0x04 /* Set by BattleEffect_MultiHit; cleared when the hits finish. */
#define IMMOBILIZED_FLINCHED \
    0x08 /* Set on the target by BattleEffect_Flinch; cleared at turn start and when it costs the action. */
#define IMMOBILIZED_CHARGING 0x10 /* Set by BattleEffect_TwoTurnMove; cleared after the attack executes. */
#define IMMOBILIZED_TRAPPING \
    0x20 /* Set on the user by BattleEffect_Trap; locks the opponent's action. */
#define IMMOBILIZED_SEMI_INVULNERABLE \
    0x40 /* Set with IMMOBILIZED_CHARGING for Dig and the 0x2B two-turn effect; cleared with it. */
#define IMMOBILIZED_CONFUSED 0x80 /* Set by the confusion effects; duration in confusionTurnsLeft. */

/* Actions locked in: Thrash or two-turn charge. */
#define IMMOBILIZED_ACTION_LOCKED (IMMOBILIZED_THRASHING | IMMOBILIZED_CHARGING)

/*Replacement States*/
#define REPLACEMENT_NONE 0
#define REPLACEMENT_VOLUNTARY_SWITCH 0x0F
#define REPLACEMENT_INVOLUNTARY_SWITCH 0x10
#define REPLACEMENT_FAINT_PENDING 0x11
#define REPLACEMENT_DEFEAT_PENDING 0x12
#define REPLACEMENT_DEFEAT_COMPLETE 0x13

#endif /* BATTLE_STATE_FLAGS_H */
