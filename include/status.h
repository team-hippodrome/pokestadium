#ifndef BATTLE_STATE_FLAGS_H
#define BATTLE_STATE_FLAGS_H

/*
 * ActivePokemonState.flags
 *
 * Presentation state, not the authoritative gameplay status.
 * Proposed names describe observed setters and consumers.
 * Keep the struct member u16.
 */
#define ACTIVE_SLEEP         0x0001 /* Sleep presentation active. */
#define ACTIVE_FREEZE        0x0002 /* Freeze presentation active. */
#define ACTIVE_AIR_SETTLE    0x0004 /* Pending height = 200 and airborne-state setup. */
#define ACTIVE_RETURN_RESET  0x0008 /* Two-turn-move return cleanup pending. */
#define ACTIVE_DIG_EMERGE    0x0010 /* Diglett/Dugtrio emergence presentation. */

/* 0x0020, 0x0040, 0x0080: unassigned. */

#define ACTIVE_HOLD_RENDER   0x0100 /* Repeatedly clears model render flag 0x20. */
#define ACTIVE_AIRBORNE      0x0200 /* Airborne presentation active. */
#define ACTIVE_AIR_ACTION    0x0400 /* Action started while airborne. */
#define ACTIVE_SUBSTITUTE    0x0800 /* Substitute presentation active. */
#define ACTIVE_SUB_BREAK     0x1000 /* Substitute break presentation pending. */

/* 0x2000: unassigned. */

#define ACTIVE_UNDERGROUND   0x4000 /* Underground/Dig presentation active. */

/* 0x8000: unassigned. */

/*
 * BattleMon.statusCondition
 *
 * Bits 0–2 hold a sleep counter, not independent status flags.
 * Toxic uses STATUS_POISON plus separate runtime state.
 * Fainting is not encoded as a status bit here.
 * Keep the struct member u8.
 */
#define STATUS_NONE          0x00
#define STATUS_SLEEP_MASK    0x07
#define STATUS_POISON        0x08
#define STATUS_BURN          0x10
#define STATUS_FREEZE        0x20
#define STATUS_PARALYSIS     0x40

/* 0x80: unassigned. */

/*
 * BattleMonRuntime.immobilizedReason (offset 0x4C)
 *
 * Volatile per-turn battle state. Despite the field name, only some bits block
 * the Pokemon from acting. Setters are the Battle_Effect_* functions in
 * battle_engine_35DF70.c; consumers are in battle_engine_359F90.c
 * (Battle_CheckMonCanAct and the move-effect turn handler).
 * Keep the struct member u8.
 */
#define IMMOBILIZED_BIDING            0x01 /* Set by Battle_Effect_Bide; stored damage in unk_54, turns left in unk_4F. */
#define IMMOBILIZED_THRASHING         0x02 /* Set by Battle_Effect_Thrash; clears into IMMOBILIZED_CONFUSED. */
#define IMMOBILIZED_MULTI_HIT         0x04 /* Set by Battle_Effect_MultiHit; cleared when the hits finish. */
#define IMMOBILIZED_FLINCHED          0x08 /* Set on the target by Battle_Effect_Flinch; cleared at turn start and when it costs the action. */
#define IMMOBILIZED_CHARGING          0x10 /* Set by Battle_Effect_TwoTurnMove; cleared after the attack executes. */
#define IMMOBILIZED_TRAPPING          0x20 /* Set on the user by Battle_Effect_Trap (Bind, Wrap, String Shot); locks the opponent's action. */
#define IMMOBILIZED_SEMI_INVULNERABLE 0x40 /* Set with IMMOBILIZED_CHARGING for Dig and the 0x2B two-turn effect; cleared with it. */
#define IMMOBILIZED_CONFUSED          0x80 /* Set by the confusion effects; turns left in unk_50. */

/* Actions locked in: Thrash or two-turn charge. */
#define IMMOBILIZED_ACTION_LOCKED     (IMMOBILIZED_THRASHING | IMMOBILIZED_CHARGING)

#endif /* BATTLE_STATE_FLAGS_H */