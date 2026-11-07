#include "battle_engine.h"
#include "src/gb_data.h"
#include "src/text_system.h"
#include "src/matrix.h"
#include "moves.h"
#include "status.h"

/*
 * Turn resolution, damage calculation, status continuations and runtime setup.
 * Individual effect initializers are dispatched from the companion unit.
 * //@src/fragments/battle_engine/battle_engine_move_mechanics.c
 */

typedef struct unk_D_843C4DA0 {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ u8 unk_02;
    /* 0x03 */ u8 unk_03;
    /* 0x04 */ u8 unk_04;
    /* 0x05 */ u8 unk_05;
    /* 0x06 */ char unk06[0x3];
    /* 0x09 */ u8 unk_09;
    /* 0x0A */ char unk0A[0xA];
    /* 0x13 */ u8 unk_13;
    /* 0x14 */ char unk14[0x1];
    /* 0x15 */ u8 unk_15;
    /* 0x16 */ char unk16[0x1];
    /* 0x17 */ u8 unk_17;
    /* 0x18 */ char unk18[0xB];
    /* 0x22 */ u8 unk_22;
    /* 0x24 */ u16 unk_24;
    /* 0x26 */ u16 unk_26;
    /* 0x28 */ u16 unk_28;
    /* 0x2A */ u16 unk_2A;
    /* 0x2C */ u16 unk_2C;
    /* 0x2E */ char unk2E[0x1E];
} unk_D_843C4DA0; // size = 0x4C

static u16 D_843C4DA0;
static u8 D_843C4DA2;
static u8 D_843C4DA3;
u8 sBattleMoveFailed;
u8 sBattleCritFlag;
static u8 D_843C4DA6;
static u8 D_843C4DA7;
static u8 D_843C4DA8;
u8 D_843C4DA9;
u8 D_843C4DAA;
static u8 D_843C4DAB;
static u8 D_843C4DAC;
static u8 D_843C4DAD;
static u8 D_843C4DAE;
static u8 D_843C4DAF;
static u8 D_843C4DB0;
static u8 D_843C4DB1;
static u8 D_843C4DB2;
static u8 D_843C4DB3;
static u8 D_843C4DB4;
static u8 D_843C4DB5;
static u8 D_843C4DB6;
static u8 D_843C4DB7;
static u8 D_843C4DB8;
static u8 D_843C4DB9;
static u8 D_843C4DBA;
static u8 D_843C4DBB;
static u8 D_843C4DBC;
static u8 D_843C4DBD;
static u8 D_843C4DBE;
static u8 D_843C4DBF;
static u8 D_843C4DC0;
static u8 D_843C4DC1;
static u8 D_843C4DC2;
u16 sBattleDamage;
static u16 D_843C4DC6;
static u16 D_843C4DC8;
static u16 D_843C4DCA;
static u16 D_843C4DCC;
static s32 pad_D_843C4DD0;
static s32 pad_D_843C4DD4;
static s32 pad_D_843C4DD8;
static s32 pad_D_843C4DDC;
static s32 pad_D_843C4DE0;
static s32 pad_D_843C4DE4;
static s32 pad_D_843C4DE8;
s32 sTypeEffectiveness;
static s32 pad_D_843C4DF0[21];
u8 sConfused;
u8 sStatCanBeChangedFurther;
static s32 pad_D_843C4E48[252];
BattleMonRuntime* sBattleUser;
BattleMonRuntime* sBattleTarget;
static BattleMonRuntime D_843C5240[2];

// Byte 0 selects mechanics branches: 1 is the default; 2 changes sleep duration/clause handling.
// The complete identities of the alternative profiles remain unresolved.
u8 sBattleMechanicsSetting[] = {
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x4A, 0x00, 0x00,
    0x60, 0x69, 0x75, 0x78, 0x85, 0x00, 0x00, 0x00, 0x61, 0x64, 0x66, 0x68, 0x8C, 0x00, 0x00, 0x00,
    0x01, 0x0A, 0x0B, 0x11, 0x13, 0x14, 0x15, 0x1E, 0x22, 0x23, 0x25, 0x27, 0x2B, 0x2C, 0x2D, 0x2E,
    0x2F, 0x40, 0x44, 0x46, 0x47, 0x51, 0x59, 0x5A, 0x5B, 0x5C, 0x67, 0x6A, 0x6B, 0x6E, 0x6F, 0x76,
    0x7A, 0x80, 0x84, 0x8B, 0x8D, 0x91, 0x94, 0x96, 0x97, 0x9A, 0x9C, 0x9F, 0xA3, 0xA4, 0x00, 0x00,
};
static u8 D_8438ACB0[] = { 1, 0x20, 0x43, 0x54, 0x56 };
StatStageMultiplier sStatStageMultipliers[] = {
    { 0x19, 0x64 }, { 0x1C, 0x64 }, { 0x21, 0x64 }, { 0x28, 0x64 }, { 0x32, 0x64 }, { 0x42, 0x64 }, { 0x01, 0x01 },
    { 0x0F, 0x0A }, { 0x02, 0x01 }, { 0x19, 0x0A }, { 0x03, 0x01 }, { 0x23, 0x0A }, { 0x04, 0x01 },
};
u8 D_8438ACD4[] = {
    0x01, 0x03, 0x24, 0x64, 0x2B, 0x64, 0x01, 0x02, 0x42, 0x64, 0x4B, 0x64, 0x01,
    0x01, 0x85, 0x64, 0xA6, 0x64, 0x02, 0x01, 0xE9, 0x64, 0x85, 0x32, 0x03, 0x01,
};
u8 sTypeEffects[] = {
    0x15, 0x14, 0x14, 0x14, 0x16, 0x14, 0x14, 0x19, 0x14, 0x16, 0x15, 0x14, 0x17, 0x15, 0x14, 0x15, 5,    0x14,
    4,    2,    0,    0x15, 0x15, 5,    0x14, 0x14, 5,    0x17, 0x17, 5,    0x19, 0x19, 5,    0x16, 0x16, 5,
    0x18, 0x18, 5,    0x14, 0x15, 5,    0x16, 0x14, 5,    0x15, 0x16, 5,    0x17, 0x16, 5,    0,    5,    5,
    0,    8,    0,    8,    8,    0x14, 0x14, 7,    0x14, 0x14, 5,    5,    0x15, 4,    0x14, 0x17, 4,    0,
    0x17, 2,    0x14, 0x16, 4,    0x14, 0x16, 7,    5,    0x16, 3,    5,    0x16, 5,    0x14, 0x16, 2,    5,
    0x19, 0x15, 5,    0x19, 0x16, 0x14, 0x19, 4,    0x14, 0x19, 2,    0x14, 1,    0,    0x14, 1,    3,    5,
    1,    2,    5,    1,    0x18, 5,    1,    7,    5,    1,    5,    0x14, 1,    0x19, 0x14, 1,    8,    0,
    3,    0x16, 0x14, 3,    3,    5,    3,    4,    5,    3,    7,    0x14, 3,    5,    5,    3,    8,    5,
    4,    0x14, 0x14, 4,    0x17, 0x14, 4,    0x16, 5,    4,    7,    5,    4,    5,    0x14, 4,    3,    0x14,
    2,    0x17, 5,    2,    1,    0x14, 2,    7,    0x14, 2,    0x16, 0x14, 2,    5,    5,    0x18, 1,    0x14,
    0x18, 3,    0x14, 7,    0x14, 5,    7,    0x16, 0x14, 7,    1,    5,    7,    2,    5,    7,    0x18, 0x14,
    7,    8,    5,    7,    3,    0x14, 5,    0x14, 0x14, 5,    1,    5,    5,    4,    5,    5,    2,    0x14,
    5,    7,    0x14, 5,    0x19, 0x14, 8,    0,    0,    8,    0x18, 0,    0x14, 0x1A, 5,    0x15, 0x1A, 5,
    0x17, 0x1A, 5,    0x16, 0x1A, 5,    0x19, 0x1A, 0x14, 0x1A, 0x1A, 0x14, 0xFF, 0,    0,
};
static u32 D_8438ADEC[] = {
    0x18191C2E,
    0x2F313839,
    0x40414243,
    0x4F525455,
};
static u32 D_8438ADFC[] = {
    0x1B000000,
};
static u32 D_8438AE00[] = {
    0x28290000,
};
static u32 D_8438AE04[] = {
    0x010A0B0C, 0x0D0E0F12, 0x13141516, 0x171A2032, 0x33343536, 0x373A3B3C, 0x3D3E3F00,
};
static u32 D_8438AE20[] = {
    0x03070810,
    0x1D1E2C30,
    0x4D510000,
};
static u32 D_8438AE2C[] = {
    0x03070810,
    0x111D1E27,
    0x28292B2C,
    0x2D301B00,
};
static s32 D_8438AE3C = 0x4D2;
static u32 D_8438AE40 = 0x1A85;
static u32 D_8438AE44 = 0x3039;
static u32 D_8438AE48 = 0x10932;
static u8 sHighCritMoves[] = { KARATE_CHOP, RAZOR_LEAF, CRABHAMMER, SLASH };

/* Empty entry point; no behavior is implemented here. */
void func_8436F6D0(void) {
}

/* Advance the battle RNG streams by a timer-derived count and return their XOR's low byte. */
u8 Battle_Random(void) {
    u8 var_v1;
    s32 ret;

    var_v1 = (osGetCount() & 0xFF0) >> 4;

    while (var_v1 > 0) {
        D_8438AE3C = (D_8438AE3C * 0x41C64E6D) + 0x303B;
        if (D_8438AE40 & 1) {
            D_8438AE40 ^= 0x11020;
        }
        D_8438AE40 >>= 1;
        var_v1--;
    }

    ret = D_8438AE3C ^ D_8438AE40;
    return ret;
}

/* Generate an accuracy roll using a separate pair of timer-stepped RNG streams. */
u8 Battle_RandomAccuracy(void) {
    u8 var_v1;
    s32 ret;

    var_v1 = (osGetCount() & 0xFF0) >> 4;

    while (var_v1 > 0) {
        D_8438AE44 = (D_8438AE44 * 0x41C64E6D) + 0x303B;
        if (D_8438AE48 & 1) {
            D_8438AE48 ^= 0x11020;
        }
        D_8438AE48 >>= 1;
        var_v1--;
    }
    ret = D_8438AE44 ^ D_8438AE48;
    return ret;
}

/* Save the target's current HP and queue an HP-band message for the acting participant. */
void ShowDamageMessage(s32 arg0, s32 arg1) {
    TeamRoster* temp_a3;
    BattleMonRuntime* temp_a2_2;
    ActivePokemonState* temp_v1;
    s32 sp18;

    temp_v1 = &sActivePokemon[arg0]->state;
    temp_a2_2 = &sActivePokemon[arg1]->state.runtime;
    temp_a3 = sActivePokemon[arg0]->sessionTeams->tagTeamMember[temp_v1->ownerTeamMemberIndex];

    if (temp_a2_2->displayHp == 0) {
        Text_SetStringToken(0x19, temp_a3->pokemonPicked[temp_v1->actionChosen].nickname);
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], 0x21);
        return;
    }

    temp_v1->unk_28 = temp_a2_2->displayHp;
    sp18 = (temp_a2_2->displayHp * 0x19) / (temp_a2_2->maxHp >> 2);

    Text_SetStringToken(0x19, temp_a3->pokemonPicked[temp_v1->actionChosen].nickname);

    if (sp18 >= 0x46) {
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], 0x21);
    } else if (sp18 >= 0x28) {
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], 0x22);
    } else if (sp18 >= 0xA) {
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], 0x23);
    } else {
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], 0x24);
    }
}

/* Clear the incoming participant's temporary effects and the opponent's trapping lock. */
void Battle_ResetTransientRuntimeState(s32 arg0, s32 arg1) {
    BattleMonRuntime* sp1C = &sActivePokemon[arg0]->state.runtime;
    BattleMonRuntime* sp18 = &sActivePokemon[arg1]->state.runtime;

    ShowDamageMessage(arg0, arg1);
    D_843C4DC2 = 0;
    D_843C4DB3 = 0;
    sp1C->moveCache.moveId = 0;
    sp1C->lastMove = 0;
    sp18->lastMove = 0;
    sp1C->unk_4A = 0;
    sp1C->unk_4B = 0;
    sp1C->immobilizedReason = 0;
    sp1C->volatileStatusFlags = 0;
    sp1C->uniqueStatusFlags = 0;
    sp1C->disabledMoveState = 0;
    sp1C->disabledMove = 0;
    sp1C->scaleResize = 0;
    sp18->immobilizedReason &= ~0x20;
    D_843C4DA3 = 1;
}

/* Queue a damage-band summary from the HP difference saved before the participant's action. */
void Battle_QueueDamageSummary(ActivePokemon* active_pokemon) {
    ActivePokemonState* sp1C = &sActivePokemon[0]->state;
    ActivePokemonState* sp18 = &sActivePokemon[1]->state;
    BattleMonRuntime* temp_a1 = &sActivePokemon[0]->state.runtime;
    BattleMonRuntime* temp_a2 = &sActivePokemon[1]->state.runtime;
    s32 var_v0;

    if (active_pokemon == sActivePokemon[0]) {
        var_v0 = ((sp1C->unk_28 - temp_a2->displayHp) * 0x19) / (temp_a2->maxHp >> 2);
    } else {
        var_v0 = ((sp18->unk_28 - temp_a1->displayHp) * 0x19) / (temp_a1->maxHp >> 2);
    }

    if (var_v0 == 0) {
        Battle_QueueMessage(sBattleMessageQueues->buffer[6], 0x25);
    } else if (var_v0 < 0x1E) {
        Battle_QueueMessage(sBattleMessageQueues->buffer[6], 0x26);
    } else if (var_v0 < 0x46) {
        Battle_QueueMessage(sBattleMessageQueues->buffer[6], 0x27);
    } else {
        Battle_QueueMessage(sBattleMessageQueues->buffer[6], 0x28);
    }

    if (sp1C->replacementState == 0x10) {
        Battle_QueueMessage(sBattleMessageQueues->buffer[6], 0);
    }
}

/* Scale damage of at least two HP by a random factor from 217/255 through 255/255. */
void Battle_ApplyDamageRandomFactor(void) {
    s32 temp_v0;

    if (sBattleDamage >= 2) {
        temp_v0 = 0;
        while (temp_v0 < 0xD9) {
            temp_v0 = Battle_Random();
        }
        sBattleDamage = (sBattleDamage * temp_v0) / 255;
    }
}

/* Return which participant acts first using Quick Attack, Counter, Speed and a random tie break. */
u16 Battle_DetermineMoveOrder(ActivePokemon* pokemon, ActivePokemon* other_pokemon) {
    BattleMonRuntime* runtime = &pokemon->state.runtime;
    BattleMonRuntime* other_runtime = &other_pokemon->state.runtime;

    if (runtime->usedMove == QUICK_ATTACK) {
        if (other_runtime->usedMove != QUICK_ATTACK) {
            return 0;
        }
    } else if (other_runtime->usedMove == QUICK_ATTACK) {
        return 1;
    }

    if (runtime->usedMove == COUNTER) {
        if (other_runtime->usedMove != COUNTER) {
            return 1;
        }
    } else if (other_runtime->usedMove == COUNTER) {
        return 0;
    }

    if (other_runtime->modifiedSpeed < runtime->modifiedSpeed) {
        return 0;
    }

    if (runtime->modifiedSpeed < other_runtime->modifiedSpeed) {
        return 1;
    }

    if (Battle_Random() < 0x80) {
        return 0;
    }
    return 1;
}

/* Test membership in a byte array of the supplied length. */
s32 IsByteInArray(u8 lookup, u8* array, s32 array_length) {
    while (array_length--) {
        if (lookup == *array) {
            return 1;
        }
        array++;
    }

    return 0;
}

/* Select a move in the first scratch runtime using the current scratch move-slot index. */
void Battle_SetActiveMoveFromSlotIndex(void) {
    D_843C5240[0].usedMove = D_843C5240[0].moves[D_843C4DC2];
}

/* Execute the cached effect and report whether it left the move-failed flag clear. */
s32 Battle_CheckMoveSuccess(void) {
    BattleEffect_ExecuteMoveEffect();

    if (sBattleMoveFailed == 1) {
        return 0;
    }
    return 1;
}

/* Calculate the requested stored stat from the selected base-stat/experience entries and DVs. */
void ComputePokemonStat(s32 arg0, BattleMon* arg1) {
    u16 i;
    u16 spC;
    u16 spA;
    u16 tmp;
    u16 sp6;
    u16 temp_a0;

    switch (arg0) {
        case 1:
            spA = gSpeciesData[arg1->level].baseHitPoints;
            spC = arg1->unk_14;
            break;

        case 2:
            spA = gSpeciesData[arg1->level].baseAttack;
            spC = arg1->unk_16;
            break;

        case 3:
            spA = gSpeciesData[arg1->level].baseDefense;
            spC = arg1->unk_18;
            break;

        case 4:
            spA = gSpeciesData[arg1->level].baseSpeed;
            spC = arg1->unk_1A;
            break;

        case 5:
            spA = gSpeciesData[arg1->level].baseSpecial;
            spC = arg1->unk_1C;
            break;
    }

    for (i = 1; i < 255; i++) {
        if (SQ(i) >= spC) {
            break;
        }
    }

    tmp = D_843C5240[0].unk_24;

    switch (arg0) {
        case 1:
            sp6 = ((tmp & 0x1000) >> 9) | ((tmp & 0x100) >> 6) | ((tmp & 0x10) >> 3) | (tmp & 1);
            break;

        case 2:
            sp6 = (tmp >> 0xC) & 0xF;
            break;

        case 3:
            sp6 = (tmp >> 8) & 0xF;
            break;

        case 4:
            sp6 = (tmp >> 4) & 0xF;
            break;

        case 5:
            sp6 = tmp & 0xF;
            break;
    }

    temp_a0 = ((((spA + sp6) * 2) + (i >> 2)) * arg1->level) / 100;
    if (arg0 == 1) {
        temp_a0 += arg1->level + 0xA;
    } else {
        temp_a0 += 5;
    }

    if (temp_a0 >= 0x3E8) {
        temp_a0 = 0x3E7;
    }

    switch (arg0) {
        case 1:
            arg1->maxHp = temp_a0;
            break;

        case 2:
            arg1->attack = temp_a0;
            break;

        case 3:
            arg1->defense = temp_a0;
            break;

        case 4:
            arg1->speed = temp_a0;
            break;

        case 5:
            arg1->special = temp_a0;
            break;
    }
}

/* Invoke the stored-stat calculator for selectors 0 through 4, preserving its original range. */
void ComputeAllPokemonStats(BattleMon* arg0) {
    ComputePokemonStat(0, arg0);
    ComputePokemonStat(1, arg0);
    ComputePokemonStat(2, arg0);
    ComputePokemonStat(3, arg0);
    ComputePokemonStat(4, arg0);
}

/* Select damage power, offensive/defensive stats and level, including screens and critical hits. */
void InitBattleParameters(void) {
    sBattleDamage = 0;
    D_843C4DC6 = sBattleUser->moveCache.power;
    if (D_843C4DC6) {
        if (sBattleUser->moveCache.type < 0x14) {
            D_843C4DCA = sBattleTarget->modifiedDefense;
            if (sBattleTarget->uniqueStatusFlags & UNIQUE_STATUS_REFLECT) {
                D_843C4DCA *= 2;
            }

            if (sBattleCritFlag != 0) {
                D_843C4DCA = sBattleTarget->unmodifiedDefense;
                D_843C4DC8 = sBattleUser->unmodifiedAttack;
            } else {
                D_843C4DC8 = sBattleUser->modifiedAttack;
            }
        } else {
            D_843C4DCA = sBattleTarget->modifiedSpecial;
            if (sBattleTarget->uniqueStatusFlags & UNIQUE_STATUS_LIGHT_SCREEN) {
                D_843C4DCA *= 2;
            }

            if (sBattleCritFlag != 0) {
                D_843C4DCA = sBattleTarget->unmodifiedSpecial;
                D_843C4DC8 = sBattleUser->unmodifiedSpecial;
            } else {
                D_843C4DC8 = sBattleUser->modifiedSpecial;
            }
        }

        if (D_843C4DC8 & 0xFF00) {
            if (D_843C4DCA & 0xFF00) {
                D_843C4DC8 >>= 2;
                D_843C4DCA >>= 2;
                if (D_843C4DC8 == 0) {
                    D_843C4DC8 = 1;
                }
            }
        }

        D_843C4DCC = sBattleUser->unk_26;
        if (sBattleCritFlag != 0) {
            D_843C4DCC *= 2;
        }
    }
}

/* Calculate base damage, halving defense for boom moves and delegating OHKO success. */
s32 Battle_CalcDamage(void) {
    s32 temp_v0 = sBattleUser->moveCache.effect;

    if (temp_v0 == 7) {
        D_843C4DCA >>= 1;
        if (D_843C4DCA == 0) {
            D_843C4DCA = 1;
        }
    }

    if ((temp_v0 != 0x1D) && (temp_v0 != 0x1E)) {
        if (temp_v0 == 0x26) {
            return Battle_CheckMoveSuccess();
        }

        if (D_843C4DC6 == 0) {
            return 0;
        }
    }

    sBattleDamage += (((((D_843C4DCC * 2) / 5) + 2) * D_843C4DC6 * D_843C4DC8) / D_843C4DCA) / 50;
    if (sBattleDamage > 0x3E4) {
        sBattleDamage = 0x3E4;
    }
    sBattleDamage += 2;

    return 1;
}

/* Subtract damage from Substitute or target HP and update the associated continuation state. */
void Battle_ApplyDamage(void) {
    if (sBattleTarget->volatileStatusFlags & 0x10) {
        if (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_CONFUSION_SELF_HIT) {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[7], 0x2B);
            if (sBattleTarget->substituteHp < sBattleDamage) {
                sBattleTarget->volatileStatusFlags &= 0xFFEF;
                Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
                Battle_QueueMessage(sBattleMessageQueues->buffer[7], 0x2C);

                if ((sBattleMechanicsSetting[0] == 1) && (sBattleUser->moveCache.effect == 0x1D)) {
                    BattleEffect_ExecuteMoveEffect();
                    sBattleUser->effectTurnsLeft = 1;
                }

                if ((sBattleMechanicsSetting[0] != 1) || (sBattleUser->moveCache.effect != 7)) {
                    sBattleUser->moveCache.effect = 0;
                }

                Battle_SetRuntimeFlags(sActivePokemon[sBattleScene.scene->actingSide], 0x1000);
            } else {
                sBattleTarget->substituteHp -= sBattleDamage;
            }
        } else {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[7], 0x2B);
            if (sBattleTarget->substituteHp < sBattleDamage) {
                sBattleTarget->volatileStatusFlags &= 0xFFEF;
                Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                Battle_QueueMessage(sBattleMessageQueues->buffer[7], 0x2C);

                if ((sBattleMechanicsSetting[0] == 1) && (sBattleUser->moveCache.effect == 0x1D)) {
                    BattleEffect_ExecuteMoveEffect();
                    sBattleUser->effectTurnsLeft = 1;
                }

                if ((sBattleMechanicsSetting[0] != 1) || (sBattleUser->moveCache.effect != 7)) {
                    sBattleUser->moveCache.effect = 0;
                }

                Battle_SetRuntimeFlags(sActivePokemon[!sBattleScene.scene->actingSide], 0x1000);
            } else {
                sBattleTarget->substituteHp -= sBattleDamage;
            }
        }
    } else if (D_84390288 == 0) {
        if (sBattleDamage < sBattleTarget->displayHp) {
            sBattleTarget->displayHp -= sBattleDamage;
        } else {
            sBattleDamage = sBattleTarget->displayHp;
            sBattleTarget->displayHp = 0;
        }
    }
}

/* Spend PP unless the active continuation suppresses it; update stored PP outside Transform. */
void Battle_DecrementPP(s32 arg0) {
    BattleMonRuntime* temp_v0;
    ActivePokemonState* temp_a1;
    BattleMon* temp_a2;

    temp_v0 = &sActivePokemon[arg0]->state.runtime;
    temp_a1 = &sActivePokemon[arg0]->state;
    temp_a2 = &sActivePokemon[arg0]->teamRoster->pokemonPicked[temp_a1->rosterIndex];

    if ((temp_v0->usedMove != 0xA5) && !(temp_v0->immobilizedReason & 7) && !(temp_v0->volatileStatusFlags & 0x40)) {
        temp_v0->pp[temp_a1->selectedMoveIndex]--;
        if (!(temp_v0->uniqueStatusFlags & 8)) {
            temp_a2->pp[temp_a1->selectedMoveIndex] -= 1;
        }
    }
}

/* Scale cached accuracy by the user's accuracy and target's evasion stages, then clamp it. */
void ApplyAccuracyStageMultipliers(void) {
    StatStageMultiplier* var_v0;
    s32 var_a2;
    s32 idx;
    s32 idx2;

    if (sBattleMechanicsSetting[0] == 1) {
        var_v0 = D_8438ACD4;
    } else {
        var_v0 = sStatStageMultipliers;
    }

    idx = sBattleUser->statChanges[4];
    idx2 = sBattleTarget->statChanges[5];
    idx2 = 0xE - idx2;

    var_a2 = (sBattleUser->moveCache.accuracy * var_v0[idx - 1].multiplier) / var_v0[idx - 1].divisor;
    if (var_a2 == 0) {
        var_a2 = 1;
    }

    var_a2 = (var_a2 * var_v0[idx2 - 1].multiplier) / var_v0[idx2 - 1].divisor;
    if (var_a2 == 0) {
        var_a2 = 1;
    }

    if (var_a2 >= 0x100) {
        var_a2 = 0xFF;
    }
    sBattleUser->moveCache.accuracy = var_a2;
}

/* Clear damage, mark failure and cancel the user's trapping continuation. */
void MarkAttackFailed(void) {
    sBattleDamage = 0;
    sBattleMoveFailed = 1;
    sBattleUser->immobilizedReason &= ~0x20;
}

/* Check recharge, Dream Eater, Substitute, semi-invulnerability, Mist and accuracy. */
void HandleMoveEffectFlags(void) {
    if (sBattleUser->moveCache.effect == 0x50) {
        if (!(sBattleUser->volatileStatusFlags & 0x20) && (sBattleMechanicsSetting[0] == 1)) {
            sBattleUser->volatileStatusFlags |= 0x20;
        }
    }

    if ((sBattleUser->moveCache.effect == 8) && !(sBattleTarget->pendingStatusCondition & 7)) {
        MarkAttackFailed();
        return;
    }

    if (sBattleTarget->volatileStatusFlags & 0x10) {
        if (sBattleUser->moveCache.effect == 17) {
            return;
        }

        if (sBattleUser->moveCache.effect == 3 || sBattleUser->moveCache.effect == 8 ||
            ((sBattleMechanicsSetting[0] == 1) && (IsByteInArray(sBattleUser->moveCache.effect, D_8438ACB0, 5) != 0))) {
            MarkAttackFailed();
            return;
        }
    }

    if (sBattleTarget->immobilizedReason & 0x40) {
        if (sBattleUser->moveCache.effect != 0x11) {
            MarkAttackFailed();
        }
    } else {
        if ((((sBattleUser->moveCache.effect >= 0x12) && (sBattleUser->moveCache.effect < 0x1A)) ||
             ((sBattleUser->moveCache.effect >= 0x3A) && (sBattleUser->moveCache.effect < 0x42))) &&
            (sBattleTarget->volatileStatusFlags & 2)) {
            MarkAttackFailed();
            return;
        }

        if (!(sBattleUser->volatileStatusFlags & 1)) {
            if ((sBattleMechanicsSetting[0] != 1) || (sBattleUser->moveCache.effect != 0x11)) {
                ApplyAccuracyStageMultipliers();
            }

            D_843C4DA2 = Battle_RandomAccuracy();
            if (D_843C4DA2 == 0xFF) {
                D_843C4DA2 = Battle_RandomAccuracy();
            }

            if (D_843C4DA2 >= sBattleUser->moveCache.accuracy) {
                MarkAttackFailed();
            }
        }
    }
}

/* Copy the runtime major-status byte to the selected roster Pokemon. */
void Battle_StoreMajorStatus(ActivePokemon* arg0) {
    BattleMon* ptr = &arg0->teamRoster->pokemonPicked[arg0->state.rosterIndex];

    ptr->statusCondition = arg0->state.runtime.pendingStatusCondition;
}

/* Quarter modified Speed when paralyzed, retaining a minimum of one. */
void ApplyParalysisSpeedCut(ActivePokemon* arg0) {
    BattleMonRuntime* temp_v0 = &arg0->state.runtime;

    if (arg0->state.runtime.pendingStatusCondition & 0x40) {
        temp_v0->modifiedSpeed >>= 2;
        if (temp_v0->modifiedSpeed == 0) {
            temp_v0->modifiedSpeed = 1;
        }
    }
}

/* Halve modified Attack when burned, retaining a minimum of one. */
void ApplyBurnAttackCut(ActivePokemon* arg0) {
    BattleMonRuntime* temp_v0 = &arg0->state.runtime;

    if (arg0->state.runtime.pendingStatusCondition & 0x10) {
        temp_v0->modifiedAttack >>= 1;
        if (temp_v0->modifiedAttack == 0) {
            temp_v0->modifiedAttack = 1;
        }
    }
}

/* Populate runtime species, HP, types, moves, stats and neutral stages from the selected roster. */
void Battle_InitPokemonRuntimeState(ActivePokemon* arg0) {
    ActivePokemonState* sp28;
    BattleMonRuntime* sp24;
    BattleMon* sp34;
    s32 i;
    s32 j;

    sp28 = &arg0->state;
    sp24 = &arg0->state.runtime;
    sp34 = &arg0->sessionTeams->tagTeamMember[sp28->ownerTeamMemberIndex]->pokemonPicked[sp28->actionChosen];

    sp24->unk_14 = sp28->rosterIndex;
    sp24->id = sp34->unk_00.unk_00;
    sp24->displayHp = sp34->hp;
    sp24->pendingStatusCondition = sp34->statusCondition;
    sp24->types[0] = sp34->type1;
    sp24->types[1] = sp34->type2;
    sp24->unk_1E = sp34->unk_08;

    sp24->moves[0] = sp34->moves[0];
    sp24->moves[1] = sp34->moves[1];
    sp24->moves[2] = sp34->moves[2];
    sp24->moves[3] = sp34->moves[3];
    sp24->unk_24 = sp34->unk_1E;

    sp24->pp[0] = sp34->pp[0];
    sp24->pp[1] = sp34->pp[1];
    sp24->pp[2] = sp34->pp[2];
    sp24->pp[3] = sp34->pp[3];

    if ((sp34->moves[0] == 0) || (sp34->moves[0] >= 0xA6)) {
        sp24->pp[0] = 0;
    }

    if ((sp34->moves[1] == 0) || (sp34->moves[1] >= 0xA6)) {
        sp24->pp[1] = 0;
    }

    if ((sp34->moves[2] == 0) || (sp34->moves[2] >= 0xA6)) {
        sp24->pp[2] = 0;
    }

    if ((sp34->moves[3] == 0) || (sp34->moves[3] >= 0xA6)) {
        sp24->pp[3] = 0;
    }

    for (i = 0; i < 4; i++) {
        if (sp34->moves[i] == 0) {
            for (j = i; j < 4; j++) {
                sp24->pp[j] = 0;
                sp24->moves[j] = sp34->moves[j] = 0;
            }
            break;
        }
    }

    sp24->unk_26 = sp34->level;
    sp24->maxHp = sp34->maxHp;
    sp24->modifiedAttack = sp34->attack;
    sp24->modifiedDefense = sp34->defense;
    sp24->modifiedSpeed = sp34->speed;
    sp24->modifiedSpecial = sp34->special;

    ApplyParalysisSpeedCut(arg0);
    ApplyBurnAttackCut(arg0);

    sp24->unk_36 = gBaseStats[sp24->id].baseHP;
    sp24->unmodifiedAttack = sp34->attack;
    sp24->unmodifiedDefense = sp34->defense;
    sp24->unmodifiedSpeed = sp34->speed;
    sp24->unmodifiedSpecial = sp34->special;

    // clang-format off
    for (i = 0; i < 8; i++) { sp24->statChanges[i] = 7; }
    // clang-format on
}

/* Report whether the roster entry selected by actionChosen - 1 has HP remaining. */
s32 Battle_IsPartyMemberAlive(ActivePokemon* arg0) {
    if (arg0->teamRoster->pokemonPicked[arg0->state.actionChosen - 1].hp != 0) {
        return 1;
    }
    return 0;
}

/* Empty entry point; no behavior is implemented here. */
void func_84370E70(void) {
}

/* Empty entry point; no behavior is implemented here. */
void func_84370E78(void) {
}

/* Load all six cached move-data bytes for usedMove; no animation is executed here. */
void Battle_LoadMoveCache(void) {
    sBattleUser->moveCache.moveId = gMoveData[sBattleUser->usedMove - 1].moveId;
    sBattleUser->moveCache.effect = gMoveData[sBattleUser->usedMove - 1].effect;
    sBattleUser->moveCache.power = gMoveData[sBattleUser->usedMove - 1].power;
    sBattleUser->moveCache.type = gMoveData[sBattleUser->usedMove - 1].type;
    sBattleUser->moveCache.accuracy = gMoveData[sBattleUser->usedMove - 1].accuracy;
    sBattleUser->moveCache.pp = gMoveData[sBattleUser->usedMove - 1].pp;
}

/* Temporarily make the damaged Rage user the effect user and apply its Attack boost. */
void Battle_ApplyRageContinuation(void) {
    BattleMonRuntime* sp24;

    if ((sBattleTarget->volatileStatusFlags & 0x40) && ((sBattleTarget->statChanges[0]) != 0xD)) {
        sp24 = sBattleUser;

        sBattleUser = sBattleTarget;
        sBattleUser->moveCache.moveId = 0;
        sBattleUser->moveCache.effect = 10;

        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[12], 0x1F);
        BattleEffect_ApplyStatBoost();

        sBattleUser->moveCache.moveId = RAGE;
        sBattleUser->moveCache.effect = 0;

        sBattleUser = sp24;
    }
}

/* Queue the effectiveness response from the accumulated type-effectiveness category. */
void Battle_QueueEffectivenessMessage(void) {
    sTypeEffectiveness = D_843C4DB3 & 0x7F;
    if (sTypeEffectiveness >= 0xB) {
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], 0x29);
    } else if (sTypeEffectiveness < 0xA) {
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], 0x2A);
    }
}

/* Queue the critical/OHKO response, record the result on the target and clear the shared flag. */
void Battle_QueueCriticalHitMessage(void) {
    BattleMonRuntime* sp1C = &sActivePokemon[!sBattleScene.scene->actingSide]->state.runtime;

    switch (sBattleCritFlag) {
        case 1:
            Battle_QueueMessage(sBattleMessageQueues->buffer[4], 0x1D);
            sBattleScene.scene->hitReactionKind = BATTLE_HIT_REACTION_SUBSTITUTE;
            break;
        case 2:
            Battle_QueueMessage(sBattleMessageQueues->buffer[4], 0x1E);
            sBattleScene.scene->hitReactionKind = BATTLE_HIT_REACTION_SUBSTITUTE;
            break;
    }
    sp1C->unk_5B = sBattleCritFlag;
    sBattleCritFlag = 0;
}

/* Select ordinary, half-HP or fixed damage, then apply it to HP/Substitute. */
void Battle_ResolveMoveDamage(void) {
    s32 var_s0;
    s32 temp_v0;

    switch (sBattleUser->moveCache.effect) {
        case 38:
            break;

        case 40:
            sBattleDamage = sBattleTarget->displayHp / 2;
            if (sBattleDamage == 0) {
                sBattleDamage = 1;
            }
            break;

        case 41:
            switch (sBattleUser->moveCache.moveId) {
                case 0x45:
                case 0x65:
                    var_s0 = sBattleUser->unk_26;
                    break;

                case 0x31:
                    var_s0 = 0x14;
                    break;

                case 0x52:
                    var_s0 = 0x28;
                    break;

                default:
                    var_s0 = sBattleUser->unk_26;
                    var_s0 = (var_s0 >> 1) + var_s0;
                    do {
                        do {
                            temp_v0 = Battle_Random();
                        } while (temp_v0 == 0);
                    } while (temp_v0 >= var_s0);
                    var_s0 = temp_v0;
            }
            sBattleDamage = var_s0;
            break;

        default:
            if (sBattleUser->moveCache.power != 0) {
                if (sBattleUser->immobilizedReason & 4) {
                    D_843C4DA0 += sBattleDamage;
                } else {
                    D_843C4DA0 = sBattleDamage;
                }
                break;
            }
            return;
    }

    if (sBattleDamage != 0) {
        Battle_ApplyDamage();
    }
}

/* Queue move failure and apply Jump Kick crash damage by temporarily swapping user and target. */
void Battle_QueueMoveFailureAndApplyRecoil(void) {
    BattleMonRuntime* sp1C;

    if (D_843C4DB3 & 0x7F) {
        if (sBattleCritFlag == 0xFF) {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[1], 0x19);
        } else {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[1], 0x1A);
        }
    } else {
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], 0x1B);
    }

    sBattleCritFlag = 0;

    if (sBattleUser->moveCache.effect == 0x2D) {
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_JUMP_KICK_CRASH;
        sBattleDamage >>= 3;
        if (sBattleDamage == 0) {
            sBattleDamage++;
        }

        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], 0x1C);

        sp1C = sBattleUser;
        sBattleUser = sBattleTarget;
        sBattleTarget = sp1C;
        sBattleScene.scene->actingSide = !sBattleScene.scene->actingSide;
        Battle_ApplyDamage();
        sBattleScene.scene->actingSide = !sBattleScene.scene->actingSide;
        sBattleTarget = sBattleUser;
        sBattleUser = sp1C;
    }
}

/* Refund one PP to the selected move, including stored PP when not transformed. */
void Battle_RefundSelectedMovePP(void) {
    BattleMonRuntime* ptr;
    BattleMon* temp_a0;
    ActivePokemonState* temp_a1;

    ptr = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;
    temp_a0 = &sActivePokemon[sBattleScene.scene->actingSide]
                   ->teamRoster->pokemonPicked[sActivePokemon[sBattleScene.scene->actingSide]->state.rosterIndex];
    temp_a1 = &sActivePokemon[sBattleScene.scene->actingSide]->state;

    sBattleUser->pp[temp_a1->selectedMoveIndex]++;
    if (!(ptr->uniqueStatusFlags & 8)) {
        temp_a0->pp[temp_a1->selectedMoveIndex]++;
    }
}

/* Refund the selected slot's PP, choose a non-Metronome move and load its cache. */
s32 Battle_Effect_Metronome(void) {
    s32 move_pulled;

    Battle_RefundSelectedMovePP();
    do {
        do {
            move_pulled = Battle_Random();
        } while (move_pulled == 0 || move_pulled >= 0xA5);
    } while (move_pulled == METRONOME);
    sBattleUser->usedMove = move_pulled;
    Battle_LoadMoveCache();
    return 1;
}

/* Stage poison/burn HP loss, scaling it by the Toxic counter when active. */
void Battle_ApplyPoisonOrBurnDamage(void) {
    u16 var_a0;

    if (sBattleUser->currentHp != 0xFFFF) {
        sBattleUser->unk_0E = sBattleUser->currentHp;
    } else {
        sBattleUser->unk_0E = sBattleUser->displayHp;
    }

    var_a0 = (sBattleUser->maxHp >> 4) & 0xFFFF;
    if (var_a0 == 0) {
        var_a0++;
    }

    if (sBattleUser->uniqueStatusFlags & 1) {
        sBattleUser->toxicCounter++;
        var_a0 *= sBattleUser->toxicCounter;
    }

    if (sBattleUser->unk_0E < var_a0) {
        sBattleUser->unk_0E = 0;
    } else {
        sBattleUser->unk_0E -= var_a0;
    }

    sBattleUser->unk_10 = sBattleUser->unk_0E;
}

/* Stage Leech Seed HP transfer, including the Toxic-counter multiplier and healing cap. */
void Battle_ApplyLeechSeedDamage(void) {
    u16 var_a0;

    if (!(sBattleUser->pendingStatusCondition & 0x18)) {
        if (sBattleUser->currentHp != 0xFFFF) {
            sBattleUser->unk_10 = sBattleUser->currentHp;
        } else {
            sBattleUser->unk_10 = sBattleUser->displayHp;
        }
    }

    var_a0 = (sBattleUser->maxHp >> 4) & 0xFFFF;
    if (var_a0 == 0) {
        var_a0++;
    }

    if (sBattleUser->uniqueStatusFlags & 1) {
        sBattleUser->toxicCounter++;
        var_a0 *= sBattleUser->toxicCounter;
    }

    if (sBattleUser->unk_10 < var_a0) {
        var_a0 = sBattleUser->unk_10;
        sBattleUser->unk_10 = 0;
    } else {
        sBattleUser->unk_10 -= var_a0;
    }

    sBattleTarget->unk_10 = sBattleTarget->displayHp;
    sBattleTarget->unk_10 += var_a0;

    if (sBattleTarget->maxHp < sBattleTarget->unk_10) {
        sBattleTarget->unk_10 = sBattleTarget->maxHp;
    }
}

/* Queue and stage poison/burn and Leech Seed effects after the action. */
s32 Battle_ApplyResidualEffects(void) {
    if (sBattleTarget->displayHp == 0) {
        return 0;
    }

    if (sBattleUser->pendingStatusCondition & 0x18) {
        if (sBattleUser->pendingStatusCondition & 8) {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[8], 0);
        } else {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[8], 1);
        }
        Battle_ApplyPoisonOrBurnDamage();
        sBattleTarget->unk_0E = sBattleTarget->displayHp;
    }

    if (sBattleUser->volatileStatusFlags & 0x80) {
        Battle_ApplyLeechSeedDamage();
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[9], 2);
    }

    if (sBattleUser->displayHp != 0) {
        return 0;
    }
    return 1;
}

/* Empty entry point; no behavior is implemented here. */
void func_843718DC(void) {
}

/* Load Mirror Move's copied move after rejecting an absent or recursive target move. */
s32 Battle_TryLoadFollowupMoveFromTarget(void) {
    if ((sBattleTarget->lastMove == 0x77) || (sBattleTarget->lastMove == 0)) {
        sBattleMessageQueues->buffer[13][0] = 0;
        sBattleMessageQueues->buffer[13][1] = 0;
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_NO_ACTION;
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], 0x20);
        return 0;
    }

    sBattleUser->usedMove = sBattleTarget->lastMove;
    Battle_RefundSelectedMovePP();
    Battle_LoadMoveCache();
    return 1;
}

/* Apply same-type damage bonus and each matching type-chart entry, recording effectiveness. */
void Battle_ApplyTypeEffectAndSTAB(void) {
    u8 var_v0;
    u8 var_v1;
    u8 temp_a0;
    u8 temp_a1;
    u8 temp_a3;
    u8 temp_t1;
    TypeEffectivenessEntry* var_a3;

    var_v0 = 3;

    temp_a1 = sBattleUser->types[0];
    temp_a3 = sBattleUser->types[1];

    var_v1 = sBattleTarget->types[0];
    temp_a0 = sBattleTarget->types[1];

    temp_t1 = sBattleUser->moveCache.type;

    if ((sBattleMechanicsSetting[0] != 1) || (sBattleUser->moveCache.moveId != STRUGGLE)) {
        if ((temp_t1 == temp_a1) || (temp_t1 == temp_a3)) {
            sBattleDamage += sBattleDamage >> 1;
            D_843C4DB3 |= 0x80;
        }

        var_a3 = &sTypeEffects[0];
        while (var_a3->unk_00 != 0xFF) {
            if ((var_a3->unk_00 == temp_t1) && ((var_a3->unk_01 == var_v1) || (var_a3->unk_01 == temp_a0))) {
                if (var_a3->unk_02 != 0) {
                    if (var_a3->unk_02 >= 0xB) {
                        var_v0++;
                    } else {
                        var_v0--;
                    }
                }

                sBattleDamage = (sBattleDamage * var_a3->unk_02) / 10;

                if (sBattleDamage == 0) {
                    sBattleMoveFailed = 1;
                }
            }
            var_a3++;
        }

        switch (var_v0) {
            case 1:
            case 2:
                D_843C4DB3 = (D_843C4DB3 & 0x80) + 5;
                break;

            case 3:
                break;

            case 4:
            case 5:
                D_843C4DB3 = (D_843C4DB3 & 0x80) + 0x14;
                break;
        }
    }
}

/* Replace Counter damage with twice the eligible Normal/Fighting damage, or mark failure. */
s32 Battle_Effect_Counter(void) {
    if (sBattleUser->usedMove != 0x44) {
        return 1;
    }

    sBattleMoveFailed = 1;

    if (sBattleTarget->usedMove == 0x44) {
        return 0;
    }

    if (sBattleTarget->moveCache.power == 0) {
        return 0;
    }

    if ((sBattleTarget->moveCache.type == 0) || (sBattleTarget->moveCache.type == 1)) {
        if (sBattleDamage == 0) {
            return 0;
        }

        if (sBattleDamage >= 0x8000) {
            sBattleDamage = 0xFFFF;
        } else {
            sBattleDamage *= 2;
        }

        sBattleMoveFailed = 0;
        sBattleCritFlag = 0;
        HandleMoveEffectFlags();
        return 0;
    }
    return 0;
}

/* Roll the rule-dependent critical-hit threshold, including Focus Energy and high-crit moves. */
s32 Battle_CheckCriticalHit(void) {
    UNUSED s32 pad;
    s32 sp18;

    sBattleCritFlag = 0;
    if (sBattleMechanicsSetting[0] == 1) {
        sp18 = (gBaseStats[sBattleUser->id].baseDefense + 0x4C) >> 2;
    } else {
        sp18 = gBaseStats[sBattleUser->id].baseDefense >> 1;
    }

    if (sBattleUser->moveCache.power == 0) {
        return 0;
    }

    if (sBattleMechanicsSetting[0] == 1) {
        if (sBattleUser->volatileStatusFlags & 4) {
            sp18 = (sp18 * 4) + 0xA0;
        } else {
            sp18 *= 2;
        }

        if (IsByteInArray(sBattleUser->moveCache.moveId, sHighCritMoves, 4) != 0) {
            sp18 *= 4;
        } else {
            sp18 >>= 1;
        }
    } else {
        if (sBattleUser->volatileStatusFlags & 4) {
            sp18 >>= 1;
        } else {
            sp18 *= 2;
            if (sp18 >= 0x100) {
                sp18 = 0xFF;
            }
        }

        if (IsByteInArray(sBattleUser->moveCache.moveId, sHighCritMoves, 4) != 0) {
            sp18 *= 4;
            if (sp18 >= 0x100) {
                sp18 = 0xFF;
            }
        } else {
            sp18 >>= 1;
        }
    }

    if (sp18 >= 0x100) {
        sp18 = 0xFF;
    }

    if (sp18 <= 0) {
        sp18 = 1;
    }

    D_843C4DA2 = Battle_Random();
    if (D_843C4DA2 >= sp18) {
        return 0;
    }
    sBattleCritFlag = 1;
    return 1;
}

/* Resolve status and locked-move continuations, returning the turn-processing branch selector. */
s32 Battle_CheckMonCanAct(void) {
    BattleMonRuntime* sp2C;
    ActivePokemonState* temp_v0;
    u16 sp26;
    u8 sp25;
    BattleMonRuntime* temp_v1;

    temp_v0 = &sActivePokemon[sBattleScene.scene->actingSide]->state;
    temp_v1 = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;

    if (sBattleUser->pendingStatusCondition & 7) {
        sBattleUser->pendingStatusCondition = (sBattleUser->pendingStatusCondition & 7) - 1;
        if (temp_v1->roster[temp_v0->rosterIndex + temp_v0->teamMemberIndex * 3] != 0) {
            temp_v1->roster[temp_v0->rosterIndex + temp_v0->teamMemberIndex * 3]--;
        }

        if (sBattleUser->pendingStatusCondition != 0) {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[1], 5);
            sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_UNABLE_TO_ACT;
        } else {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[1], 6);
            sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_UNABLE_TO_ACT;
        }

        sBattleUser->lastMove = 0;
        return 0;
    }

    if (sBattleUser->pendingStatusCondition & 0x20) {
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], 7);
        sBattleUser->lastMove = 0;
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_UNABLE_TO_ACT;
        return 0;
    }

    if (sBattleTarget->immobilizedReason & 0x20) {
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], 8);
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_UNABLE_TO_ACT;
        return 0;
    }

    if (sBattleUser->immobilizedReason & 8) {
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], 9);
        sBattleUser->volatileStatusFlags &= 0xFFDF;
        sBattleUser->immobilizedReason &= 0xFFF7;
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_NO_ACTION;
        return 0;
    }

    if (sBattleUser->volatileStatusFlags & 0x20) {
        sBattleUser->volatileStatusFlags &= ~0x20;
        if (sBattleMechanicsSetting[0] == 1) {
            sBattleDamage = 0;
        }
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_UNABLE_TO_ACT;
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], 0xA);
        return 0;
    }

    if (sBattleUser->disabledMoveState != 0) {
        sBattleUser->disabledMoveState--;
        if (!(sBattleUser->disabledMoveState & 0xF)) {
            sBattleUser->disabledMoveState = 0;
            sBattleUser->disabledMove = 0;
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[13], 0xB);
        }
    }

    if (sBattleUser->immobilizedReason & 0x80) {
        sBattleUser->confusionTurnsLeft--;
        if (sBattleUser->confusionTurnsLeft == 0) {
            sBattleUser->immobilizedReason &= 0xFF7F;
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[5], 0xC);
        } else {
            Battle_QueueMoveCategoryMessage();
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[5], 0xD);
            D_843C4DA2 = Battle_Random();
            if (D_843C4DA2 >= 0x80) {
                Battle_QueueMetronomeMoveMessage();
                sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_CONFUSION_SELF_HIT;
                sBattleUser->immobilizedReason &= 0x80;
                Battle_QueueMessage(sBattleMessageQueues->buffer[4], 0xE);

                sp26 = sBattleTarget->modifiedDefense;
                sBattleTarget->modifiedDefense = sBattleUser->modifiedDefense;
                sp25 = sBattleUser->moveCache.effect;
                sBattleUser->moveCache.effect = 0;
                sBattleCritFlag = 0;
                sBattleUser->moveCache.power = 0x28;
                sBattleUser->moveCache.type = 0;
                InitBattleParameters();
                Battle_CalcDamage();
                sBattleUser->moveCache.effect = sp25;
                sBattleTarget->modifiedDefense = sp26;

                sp2C = sBattleUser;
                sBattleUser = sBattleTarget;
                sBattleTarget = sp2C;
                Battle_ApplyDamage();
                sBattleTarget = sBattleUser;
                sBattleUser = sp2C;
                goto label1;
            }
        }
    }

    if ((sBattleUser->disabledMove != 0) && (sBattleUser->disabledMove == sBattleUser->usedMove)) {
        sBattleUser->immobilizedReason &= 0xFFEF;
        Text_SetStringToken(0x1D, gMoveDisplayInfo[sBattleUser->usedMove - 1].unk_04);
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], 0xF);
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_UNABLE_TO_ACT;
        return 0;
    }

    if (sBattleUser->pendingStatusCondition & 0x40) {
        D_843C4DA2 = Battle_Random();
        if (D_843C4DA2 < 0x3F) {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[1], 0x10);
            sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_FULL_PARALYSIS;
            sBattleDamage = 0;
        label1:
            sBattleUser->immobilizedReason &= 0xFF8C;
            return 0;
        }
    }

    if (sBattleUser->immobilizedReason & 1) {
        sBattleUser->moveCache.moveId = 0;
        sBattleUser->effectCounter += sBattleDamage;
        sBattleUser->effectTurnsLeft -= 1;
        if (sBattleUser->effectTurnsLeft != 0) {
            sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_BIDE;
            return 0;
        }
        sBattleUser->immobilizedReason &= 0xFFFE;
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[2], 0x11);
        sBattleUser->moveCache.power = 1;

        sBattleDamage = sBattleUser->effectCounter * 2;
        if (!sBattleDamage) {
            sBattleMoveFailed = 1;
        }

        if ((sBattleMechanicsSetting[0] == 1) && (sBattleTarget->immobilizedReason & 0x40)) {
            sBattleMoveFailed = 1;
        }

        sBattleUser->effectCounter = 0;
        sBattleUser->moveCache.moveId = BIDE;
        return 5;
    }

    if (sBattleUser->immobilizedReason & 2) {
        sBattleUser->moveCache.moveId = THRASH;
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[2], 0x12);
        sBattleUser->effectTurnsLeft -= 1;
        if (sBattleUser->effectTurnsLeft != 0) {
            return 4;
        }

        Battle_QueueMessage(sBattleMessageQueues->buffer[5], -0x45);
        sBattleUser->immobilizedReason &= 0xFFFD;
        sBattleUser->immobilizedReason |= 0x80;
        sBattleUser->confusionTurnsLeft = (Battle_Random() & 3) + 2;
        return 4;
    }

    if (sBattleUser->immobilizedReason & 0x20) {
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[2], 0x13);
        sBattleUser->effectTurnsLeft -= 1;
        return 6;
    }

    if (sBattleUser->volatileStatusFlags & 0x40) {
        sBattleUser->moveCache.effect = 0;
        return 3;
    }
    return 2;
}

#define PRINTF(x) ((void)(x))

/* Resolve the current action through move selection, hit/damage checks and effect dispatch. */
void ProcessBattleTurn(void) {
    s8 sp2C[12];

    if (sBattleUser->usedMove != 0xFF) {
        sBattleMoveFailed = 0;
        D_843C4DB5 = 0;
        D_843C4DA9 = 0;
        D_843C4DB3 = 0xA;

        switch (Battle_CheckMonCanAct()) {
            case 2:
                Battle_LoadMoveCache();
                if (sBattleUser->immobilizedReason & 0x10) {
                    sBattleUser->immobilizedReason &= ~(0x40 | 0x10);
                    Battle_SetRuntimeFlags(sActivePokemon[sBattleScene.scene->actingSide], 8);
                } else {
                block_5:
                    switch (sBattleUser->moveCache.effect) {
                        case 0x27:
                        case 0x2B:
                            if (sBattleMechanicsSetting[0] == 1) {
                                sBattleDamage = 0;
                            }

                            BattleEffect_ExecuteMoveEffect();
                            sBattleUser->lastMove = sBattleUser->usedMove;
                            return;
                    }
                }
            case 3:
                Battle_QueueMoveCategoryUsedMessage();
                Battle_DecrementPP(sBattleScene.scene->actingSide);
                if (IsByteInArray(sBattleUser->moveCache.effect, D_8438ADEC, 0x10) != 0) {
                    if (sBattleMechanicsSetting[0] == 1) {
                        sBattleDamage = 0;
                    }

                    BattleEffect_ExecuteMoveEffect();
                    return;
                }

                if (IsByteInArray(sBattleUser->moveCache.effect, D_8438ADFC, 1) != 0) {
                    BattleEffect_ExecuteMoveEffect();
                }
            case 4:
                if (IsByteInArray(sBattleUser->moveCache.effect, D_8438AE00, 2) == 0) {
                    Battle_CheckCriticalHit();
                    if (Battle_Effect_Counter() != 0) {
                        InitBattleParameters();
                        if (Battle_CalcDamage() == 0) {
                            goto block_29;
                        }
                        Battle_ApplyTypeEffectAndSTAB();
                        Battle_ApplyDamageRandomFactor();
                    } else {
                        goto block_24;
                    }
                }
                if (sBattleMechanicsSetting[0] == 1) {
                    sBattleUser->moveCache.accuracy = gMoveData[sBattleUser->usedMove - 1].accuracy;
                }

                HandleMoveEffectFlags();
            case 5:
            block_24:
                if (sBattleMoveFailed != 0) {
                    switch (sBattleUser->moveCache.effect) {
                        case 7:
                            goto block_28;
                        default:
                            goto block_28;
                    }
                }
            case 6:
            block_28:
            block_29:
                switch (sBattleUser->moveCache.effect) {
                    case 9:
                        if (Battle_TryLoadFollowupMoveFromTarget() == 0) {
                            break;
                        }
                        D_843C4DB5 = 0;
                        goto block_5;
                    case 0x53:
                        Battle_Effect_Metronome();
                        goto block_5;
                    default:
                        if (IsByteInArray(sBattleUser->moveCache.effect, D_8438AE04, 0x1B) != 0) {
                            BattleEffect_ExecuteMoveEffect();
                            return;
                        }

                        if (sBattleMoveFailed != 0) {
                            Battle_QueueMoveFailureAndApplyRecoil();

                            if (sBattleUser->moveCache.effect != 7) {
                                break;
                            }
                        } else {
                            Battle_ResolveMoveDamage();
                            Battle_QueueCriticalHitMessage();
                            Battle_QueueEffectivenessMessage();
                            D_843C4DA9 = 1;
                        }
                        if (IsByteInArray(sBattleUser->moveCache.effect, D_8438AE20, 0xA) != 0) {
                            BattleEffect_ExecuteMoveEffect();
                        }
                        if (sBattleTarget->displayHp == 0) {
                            if (sBattleMechanicsSetting[0] != 1) {
                                return;
                            }
                            if (sBattleUser->moveCache.effect != 0x50) {
                                return;
                            }
                            sBattleDamage = 0;
                            return;
                        }
                        Battle_ApplyRageContinuation();
                        if (sBattleUser->immobilizedReason & 4) {
                            sBattleUser->effectTurnsLeft--;
                            if (sBattleUser->effectTurnsLeft != 0) {
                                goto block_28;
                            }
                            sBattleUser->immobilizedReason &= ~4;
                            sprintf(sp2C, "%d", sBattleUser->effectCounter);
                            // separate stubbed print
                            PRINTF(" SpAttackTbl6\n");
                            Text_SetNumberToken(2, sBattleUser->effectCounter);
                            Battle_QueueMessage(sBattleMessageQueues->buffer[14], 4);
                            sBattleScene.scene->hitReactionKind = BATTLE_HIT_REACTION_MULTI_HIT;
                            sBattleUser->effectCounter = 0;
                            sBattleDamage = D_843C4DA0;
                        }
                        if (sBattleUser->moveCache.effect == 0) {
                            break;
                        }
                        if (IsByteInArray(sBattleUser->moveCache.effect, D_8438AE2C, 0xF) != 0) {
                            break;
                        }
                        BattleEffect_ExecuteMoveEffect();
                        break;
                }
                break;
            default:
                break;
        }
    }
    D_843C4DB7 = 0;
}

/* Mark fainting, clear continuation state and remove temporary effects from the opponent. */
void Battle_HandleFaint(ActivePokemon* arg0) {
    s32 sp1C = BattleScene_GetParticipantSideIndex(arg0);
    ActivePokemonState* temp_a10;
    BattleMonRuntime* temp_a11;
    BattleMonRuntime* temp_a01;

    temp_a10 = &sActivePokemon[sp1C]->state;
    temp_a11 = &sActivePokemon[sp1C]->state.runtime;
    temp_a01 = &sActivePokemon[!sp1C]->state.runtime;

    temp_a01->effectCounter = 0;
    temp_a01->lastMove = 0;
    temp_a01->immobilizedReason &= ~4;

    temp_a11->unk_4A = 0;
    temp_a11->unk_4B = 0;
    temp_a11->immobilizedReason = 0;
    temp_a11->volatileStatusFlags &= 0x80;
    temp_a11->uniqueStatusFlags = 0;
    temp_a11->disabledMoveState = 0;
    temp_a11->disabledMove = 0;
    temp_a11->scaleResize = 0;
    temp_a11->lastMove = 0;

    if (temp_a10->replacementState != 0xF) {
        temp_a10->replacementState = 0x11;
    }

    Battle_QueueDamageSummary(arg0);
    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sp1C]));
    Battle_QueueMessage(sBattleMessageQueues->buffer[16 + sp1C], 3);
}

/* Test remaining roster HP using the active Pokemon's staged HP value for this checkpoint. */
void Battle_CheckDefeatAtStatusHp(ActivePokemon* arg0) {
    ActivePokemonState* ptr1 = &arg0->state;
    BattleMon* temp_v0;
    s32 i;
    s32 var_s2 = 0;

    for (i = 0; i < BattleScene_GetTeamMemberTeamSize(arg0); i++) {
        if (i == ptr1->rosterIndex) {
            var_s2 += arg0->state.runtime.unk_0E;
        } else {
            temp_v0 = &arg0->teamRoster->pokemonPicked[i];
            var_s2 += temp_v0->hp;
        }
    }

    if (arg0->sessionTeams->tagTeamSize == 2) {
        for (i = 0; i < BattleScene_GetPartnerTeamSize(arg0); i++) {
            temp_v0 = &arg0->sessionTeams->tagTeamMember[!ptr1->teamMemberIndex]->pokemonPicked[i];
            var_s2 += temp_v0->hp;
        }
    }

    if (var_s2 == 0) {
        Battle_QueueDamageSummary(arg0);
        ptr1->replacementState = 0x12;
    } else {
        Battle_HandleFaint(arg0);
    }
}

/* Test remaining roster HP using the active Pokemon's post-Leech-Seed HP value. */
void Battle_CheckDefeatAtSeedHp(ActivePokemon* arg0) {
    ActivePokemonState* ptr1 = &arg0->state;
    BattleMon* temp_v0;
    s32 i;
    s32 var_s2 = 0;

    for (i = 0; i < BattleScene_GetTeamMemberTeamSize(arg0); i++) {
        if (i == ptr1->rosterIndex) {
            var_s2 += arg0->state.runtime.unk_10;
        } else {
            temp_v0 = &arg0->teamRoster->pokemonPicked[i];
            var_s2 += temp_v0->hp;
        }
    }

    if (arg0->sessionTeams->tagTeamSize == 2) {
        for (i = 0; i < BattleScene_GetPartnerTeamSize(arg0); i++) {
            temp_v0 = &arg0->sessionTeams->tagTeamMember[!ptr1->teamMemberIndex]->pokemonPicked[i];
            var_s2 += temp_v0->hp;
        }
    }

    if (var_s2 == 0) {
        Battle_QueueDamageSummary(arg0);
        ptr1->replacementState = 0x12;
    } else {
        Battle_HandleFaint(arg0);
    }
}

/* Test remaining roster HP using the active Pokemon's post-action HP value. */
void Battle_CheckDefeatAtCurrentHp(ActivePokemon* arg0) {
    ActivePokemonState* ptr1 = &arg0->state;
    BattleMon* temp_v0;
    s32 i;
    s32 var_s2 = 0;

    for (i = 0; i < BattleScene_GetTeamMemberTeamSize(arg0); i++) {
        if (i == ptr1->rosterIndex) {
            var_s2 += arg0->state.runtime.currentHp;
        } else {
            temp_v0 = &arg0->teamRoster->pokemonPicked[i];
            var_s2 += temp_v0->hp;
        }
    }

    if (arg0->sessionTeams->tagTeamSize == 2) {
        for (i = 0; i < BattleScene_GetPartnerTeamSize(arg0); i++) {
            temp_v0 = &arg0->sessionTeams->tagTeamMember[!ptr1->teamMemberIndex]->pokemonPicked[i];
            var_s2 += temp_v0->hp;
        }
    }

    if (var_s2 == 0) {
        Battle_QueueDamageSummary(arg0);
        ptr1->replacementState = 0x12;
    } else {
        Battle_HandleFaint(arg0);
    }
}

/* Test remaining roster HP using the active Pokemon's displayed HP value. */
void Battle_CheckDefeatAtDisplayHp(ActivePokemon* arg0) {
    ActivePokemonState* ptr1 = &arg0->state;
    BattleMon* temp_v0;
    s32 i;
    s32 var_s2 = 0;

    for (i = 0; i < BattleScene_GetTeamMemberTeamSize(arg0); i++) {
        if (i == ptr1->rosterIndex) {
            var_s2 += arg0->state.runtime.displayHp;
        } else {
            temp_v0 = &arg0->teamRoster->pokemonPicked[i];
            var_s2 += temp_v0->hp;
        }
    }

    if (arg0->sessionTeams->tagTeamSize == 2) {
        for (i = 0; i < BattleScene_GetPartnerTeamSize(arg0); i++) {
            temp_v0 = &arg0->sessionTeams->tagTeamMember[!ptr1->teamMemberIndex]->pokemonPicked[i];
            var_s2 += temp_v0->hp;
        }
    }

    if (var_s2 == 0) {
        Battle_QueueDamageSummary(arg0);
        ptr1->replacementState = 0x12;
    } else {
        Battle_HandleFaint(arg0);
    }
}

/* Test whether staged poison/burn damage leaves the side without a surviving roster member. */
s32 Battle_CheckSideDefeatAfterResidualStatus(ActivePokemon* arg0) {
    ActivePokemonState* ptr1 = &arg0->state;
    BattleMon* temp_v0;
    s32 i;
    s32 var_s2;

    if (arg0->state.runtime.pendingStatusCondition & 0x18) {
        var_s2 = 0;
        if (arg0->state.runtime.unk_0E == 0) {
            for (i = 0; i < BattleScene_GetTeamMemberTeamSize(arg0); i++) {
                temp_v0 = &arg0->teamRoster->pokemonPicked[i];
                var_s2 += temp_v0->hp;
            }

            if (arg0->sessionTeams->tagTeamSize == 2) {
                for (i = 0; i < BattleScene_GetPartnerTeamSize(arg0); i++) {
                    temp_v0 = &arg0->sessionTeams->tagTeamMember[!ptr1->teamMemberIndex]->pokemonPicked[i];
                    var_s2 += temp_v0->hp;
                }
            }

            if (var_s2 == 0) {
                ptr1->replacementState = 0x12;
                return 1;
            }
            Battle_HandleFaint(arg0);
        }
        return 1;
    }
    return 0;
}

/* Select the appropriate roster-HP defeat check for direct damage and residual checkpoints. */
s32 Battle_CheckFaintAfterMove(void) {
    if (sBattleTarget->displayHp == 0) {
        Battle_CheckDefeatAtDisplayHp(sActivePokemon[!sBattleScene.scene->actingSide]);
    }

    if (sBattleUser->displayHp == 0) {
        Battle_CheckDefeatAtDisplayHp(sActivePokemon[sBattleScene.scene->actingSide]);
    }

    if ((sBattleUser->displayHp != 0) && (sBattleTarget->displayHp != 0) && (sBattleUser->currentHp == 0)) {
        Battle_CheckDefeatAtCurrentHp(sActivePokemon[sBattleScene.scene->actingSide]);
        return 0;
    } else if ((sBattleUser->displayHp != 0) && (sBattleTarget->displayHp != 0) && (sBattleUser->pendingStatusCondition & 0x18) &&
               (sBattleUser->unk_0E == 0)) {
        Battle_CheckDefeatAtStatusHp(sActivePokemon[sBattleScene.scene->actingSide]);
        return 0;
    } else if ((sBattleUser->displayHp != 0) && (sBattleTarget->displayHp != 0) && (sBattleUser->volatileStatusFlags & 0x80) &&
               (sBattleUser->unk_10 == 0)) {
        Battle_CheckDefeatAtSeedHp(sActivePokemon[sBattleScene.scene->actingSide]);
        return 0;
    }
}

/* Initialize one action's shared state, resolve its turn and residuals, then check defeat. */
void Battle_ResolveMonAction(void) {
    BattleMonRuntime* sp1C = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;
    BattleMonRuntime* sp18 = &sActivePokemon[!sBattleScene.scene->actingSide]->state.runtime;
    ActivePokemonState* ptr1 = &sActivePokemon[0]->state;
    ActivePokemonState* ptr2 = &sActivePokemon[1]->state;

    ptr1->moveSelectionLocked = 0;
    ptr2->moveSelectionLocked = 0;

    sConfused = 1;
    sStatCanBeChangedFurther = 1;

    sBattleScene.scene->hitReactionKind = BATTLE_HIT_REACTION_NONE;

    sBattleScene.scene->rageFollowupPhase = 0;
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_NORMAL;

    sp1C->currentHp = 0xFFFF;
    sp18->currentHp = 0xFFFF;

    D_843C4DA0 = 0;

    Battle_ClearAllMessageQueues();
    Battle_StoreMajorStatus(sActivePokemon[0]);
    Battle_StoreMajorStatus(sActivePokemon[1]);

    sBattleUser = sp1C;
    sBattleTarget = sp18;
    ProcessBattleTurn();
    Battle_ApplyResidualEffects();

    if (D_84390288 == 0) {
        Battle_CheckFaintAfterMove();
    }
}

/* Initialize an entering participant's runtime and clear its transient battle state. */
void BeginBattleForEntity(ActivePokemon* arg0) {
    ActivePokemonState* sp24;
    ActivePokemonState* sp20;
    BattleMonRuntime* ptr1;
    BattleMonRuntime* ptr2;

    ptr1 = &arg0->state.runtime;

    sp24 = &sActivePokemon[0]->state;
    sp20 = &sActivePokemon[1]->state;

    ptr2 = &sActivePokemon[BattleScene_GetParticipantSideIndex(arg0) == 0]->state.runtime;

    sp24->moveSelectionLocked = 0;
    sp20->moveSelectionLocked = 0;
    ptr1->currentHp = 0xFFFF;
    ptr2->currentHp = 0xFFFF;

    sConfused = 1;
    sStatCanBeChangedFurther = 1;

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_NORMAL;
    sBattleScene.scene->rageFollowupPhase = 0;

    sBattleUser = ptr1;
    sBattleTarget = ptr2;

    Battle_QueueDamageSummary(arg0);
    Vec3f_SetComponentsDuplicate(&arg0->modelTransform.unk_00, 0.0f, 0.0f, 0.0f);
    Vec3f_SetComponentsDuplicate(&arg0->modelTransform.rotation, 0.0f, 0.0f, 0.0f);
    Battle_InitPokemonRuntimeState(arg0);

    if (arg0 == sActivePokemon[0]) {
        Battle_ResetTransientRuntimeState(0, 1);
    } else {
        Battle_ResetTransientRuntimeState(1, 0);
    }
}
