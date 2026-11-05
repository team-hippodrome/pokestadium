#include "battle_engine.h"
#include "moves.h"
#include "src/text_system.h"

/*actual battle logic*/

typedef void (*MoveEffectCallback)(void);

static const char D_8438DC80[] = "は" "\n" "こんらんした";
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
    Battle_Effect_Sleep, Battle_Effect_Poison, Battle_Effect_DrainHP, Battle_Effect_SecondaryStatusChance, Battle_Effect_SecondaryStatusChance, Battle_Effect_SecondaryStatusChance, Battle_Effect_Explosion,
    Battle_Effect_DrainHP, NULL,          Battle_Effect_RaiseOwnStat, Battle_Effect_RaiseOwnStat, Battle_Effect_RaiseOwnStat, Battle_Effect_RaiseOwnStat, Battle_Effect_RaiseOwnStat,
    Battle_Effect_RaiseOwnStat, Battle_Effect_PayDay, NULL,          Battle_Effect_LowerStat, Battle_Effect_LowerStat, Battle_Effect_LowerStat, Battle_Effect_LowerStat,
    Battle_Effect_LowerStat, Battle_Effect_LowerStat, Battle_Effect_Conversion, Battle_Effect_Haze, Battle_Effect_Bide, Battle_Effect_Thrash, Battle_Effect_ForceSwitchOrFlee,
    Battle_Effect_MultiHit, Battle_Effect_MultiHit, Battle_Effect_Flinch, Battle_Effect_Sleep, Battle_Effect_Poison, Battle_Effect_SecondaryStatusChance, Battle_Effect_SecondaryStatusChance,
    Battle_Effect_SecondaryStatusChance, Battle_Effect_Flinch, Battle_Effect_OHKO, Battle_Effect_TwoTurnMove, NULL,          NULL,          Battle_Effect_Trap,
    Battle_Effect_TwoTurnMove, Battle_Effect_MultiHit, NULL,          Battle_Effect_Mist, Battle_Effect_FocusEnergy, Battle_Effect_Recoil, Battle_Effect_Confuse,
    Battle_Effect_RaiseOwnStat, Battle_Effect_RaiseOwnStat, Battle_Effect_RaiseOwnStat, Battle_Effect_RaiseOwnStat, Battle_Effect_RaiseOwnStat, Battle_Effect_RaiseOwnStat, Battle_Effect_Heal,
    Battle_Effect_Transform, Battle_Effect_LowerStat, Battle_Effect_LowerStat, Battle_Effect_LowerStat, Battle_Effect_LowerStat, Battle_Effect_LowerStat, Battle_Effect_LowerStat,
    Battle_Effect_ReflectLightScreen, Battle_Effect_ReflectLightScreen, Battle_Effect_Poison, Battle_Effect_Paralyze, Battle_Effect_LowerStat, Battle_Effect_LowerStat, Battle_Effect_LowerStat,
    Battle_Effect_LowerStat, Battle_Effect_LowerStat, Battle_Effect_LowerStat, Battle_Effect_LowerStat, Battle_Effect_LowerStat, Battle_Effect_ConfuseChance, Battle_Effect_MultiHit,
    NULL,          Battle_Effect_Substitute, Battle_Effect_Recharge, Battle_Effect_Rage, Battle_Effect_Mimic, NULL,          Battle_Effect_LeechSeed,
    Battle_Effect_Splash, Battle_Effect_Disable, NULL,
};

u16 Battle_ApplyStatStageMultiplier(u16* arg0, u16 arg1, u8 arg2) {
    *arg0 = (arg1 * gStatStageMultipliers[arg2 - 1].unk_00) / gStatStageMultipliers[arg2 - 1].unk_01;

    if (*arg0 == 0) {
        *arg0 = 1;
    }

    if (*arg0 > 999) {
        *arg0 = 999;
    }

    return *arg0;
}

void Battle_Effect_Sleep(void) {
    s32 sp24;
    s32 var_v1;

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_TARGET_STATUS;
    sp24 = sBattleTarget->unk_4D & 0x20;
    sBattleTarget->unk_4D &= ~0x20;

    if (sp24 == 0) {
        if (sBattleTarget->unk_15 & 7) {
            sConfused = 0;
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x80);
            return;
        }

        if (sBattleTarget->unk_15 == 0) {
            HandleMoveEffectFlags();
            if ((sBattleMoveFailed == 0) &&
                ((Battle_SleepClauseInEffect(sActivePokemon[!sBattleScene.scene->actingSide]) == 0) || (D_8438AC60[0] == 2))) {
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
    if ((Battle_SleepClauseInEffect(sActivePokemon[!sBattleScene.scene->actingSide]) != 0) && (D_8438AC60[0] != 2)) {
        sConfused = 0;
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x7F);
        if ((sp24 != 0) && (D_8438AC60[0] == 1)) {
            sBattleTarget->unk_4D |= 0x20;
        }
    } else {
        do {
            if (D_8438AC60[0] == 2) {
                var_v1 = Battle_Random() & 7;
            } else {
                var_v1 = Battle_Random() & 3;
            }
        } while (var_v1 == 0);
        sBattleMoveFailed = 0;
        sBattleTarget->unk_15 = var_v1;
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x7E);
    }
}

void Battle_Effect_Poison(void) {
    char* sp1C = sBattleMessageQueues->buffer[1];

    if (!(sBattleTarget->unk_4D & 0x10) && (sBattleTarget->unk_15 == 0) && (sBattleTarget->roster[6] != 3) &&
        (sBattleTarget->roster[7] != 3)) {
        switch (sBattleUser->moveCache.unk_01) {
            case 2:
                if (sBattleScene.scene->moveEffectKind != MOVE_PRESENTATION_MULTI_HIT) {
                    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_NORMAL;
                    sp1C = sBattleMessageQueues->buffer[7];
                } else {
                    sp1C = sBattleMessageQueues->buffer[11];
                }

                if (Battle_Random() >= 0x34) {
                    return;
                }
                break;

            case 33:
                sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_NORMAL;
                sp1C = sBattleMessageQueues->buffer[7];
                if (Battle_Random() >= 0x67) {
                    return;
                }
                break;

            default:
                sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_TARGET_STATUS;
                sTypeEffectiveness = 0xA;
                HandleMoveEffectFlags();
                if (sBattleMoveFailed != 0) {
                    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                    Battle_QueueMessage(sp1C, -0x7D);
                    return;
                }
                break;
        }

        sBattleTarget->unk_15 |= 8;
        if (sBattleUser->moveCache.lastMove == TOXIC) {
            sBattleTarget->battleStatus3 |= 1;
            sBattleTarget->unk_51 = 0;
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sp1C, -0x7C);
        } else {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sp1C, -0x7B);
        }
    } else if (sBattleUser->moveCache.unk_01 == 0x42) {
        if (sBattleScene.scene->moveEffectKind != MOVE_PRESENTATION_MULTI_HIT) {
            sBattleMoveFailed = 1;
            sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_TARGET_STATUS;
            sConfused = 0;
        }
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sp1C, -0x7A);
    }
}

void Battle_Effect_DrainHP(void) {
    sBattleDamage /= 2;
    if (sBattleDamage == 0) {
        sBattleDamage = 1;
    }

    if (sBattleUser->hp < sBattleUser->displayHp + sBattleDamage) {
        sBattleUser->displayHp = sBattleUser->hp;
    } else {
        sBattleUser->displayHp += sBattleDamage;
    }

    if (sBattleUser->moveCache.unk_01 == 8) {
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
    sBattleUser->unk_15 = 0;
    sBattleUser->unk_4D &= ~0x80;
}

void Battle_Effect_SecondaryStatusChance(void) {
    BattleMonRuntime* temp_a2 = &sActivePokemon[!sBattleScene.scene->actingSide]->state.runtime;
    ActivePokemonState* temp_a0 = &sActivePokemon[!sBattleScene.scene->actingSide]->state;

    if (sBattleTarget->unk_4D & 0x10) {
        return;
    }

    if (sBattleTarget->unk_15 != 0) {
        if ((sBattleTarget->unk_15 & 0x20) && (sBattleUser->moveCache.unk_03 == 0x14)) {
            sBattleTarget->unk_15 = 0;
            if (temp_a0->moveIndex != 4) {
                temp_a2->usedMove = temp_a2->moves[temp_a0->selectedMoveIndex];
            } else {
                temp_a0->moveIndex = 0;
                temp_a0->selectedMoveIndex = 0;
                temp_a2->usedMove = 0xA5;
            }
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x77);
        }
        return;
    }

    if ((sBattleTarget->roster[6] != (*sBattleUser).moveCache.unk_03) &&
        (sBattleTarget->roster[7] != (*sBattleUser).moveCache.unk_03)) {
        if ((sBattleUser->moveCache.unk_01 < 7) || ((D_8438AC60[0] != 2) && (sBattleUser->moveCache.lastMove == BLIZZARD))) {
            if (Battle_Random() >= 0x1A) {
                return;
            }
        } else if (Battle_Random() >= 0x4D) {
            return;
        }

        switch (sBattleUser->moveCache.unk_01) {
            case 4:
            case 34:
                sBattleTarget->unk_15 = 0x10;
                ApplyBurnAttackCut(sActivePokemon[!sBattleScene.scene->actingSide]);
                Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                Battle_QueueMessage(sBattleMessageQueues->buffer[7], -0x76);
                break;

            case 5:
            case 35:
                if (Battle_FreezeClauseInEffect(sActivePokemon[!sBattleScene.scene->actingSide]) == 0) {
                    sBattleTarget->unk_4D &= ~0x20;
                    sBattleTarget->unk_15 = 0x20;
                    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                    Battle_QueueMessage(sBattleMessageQueues->buffer[7], -0x75);
                }
                break;

            case 6:
            case 36:
                sBattleTarget->unk_15 = 0x40;
                ApplyParalysisSpeedCut(sActivePokemon[!sBattleScene.scene->actingSide]);
                Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                Battle_QueueMessage(sBattleMessageQueues->buffer[7], -0x74);
                break;
        }
    }
}

void Battle_ApplyRageStatBoost(void) {
    s32 sp2C;
    s32 sp28;
    s32 sp24;
    s32 var_a3;
    u16* sp1C;
    u16* sp18;

    sBattleScene.scene->rageFollowupPhase = BATTLE_PHASE_RAGE;

    var_a3 = 1;
    sp24 = sBattleUser->moveCache.unk_01 - 0xA;

    if (sp24 >= 8) {
        var_a3 = 2;
        sp24 -= 0x28;
    }

    if (sBattleUser->unk_5C[sp24] != 0xD) {
        sBattleUser->unk_5C[sp24] += var_a3;
        if (sBattleUser->unk_5C[sp24] >= 0xE) {
            sBattleUser->unk_5C[var_a3] = 0xD;
        }

        sp28 = 0;
        sp2C = 0;

        if (sp24 < 4) {
            switch (sp24) {
                case 0:
                    sp18 = &sBattleUser->unk_38;
                    sp1C = &sBattleUser->unk_2A;
                    sp2C = 1;
                    break;

                case 1:
                    sp18 = &sBattleUser->unk_3A;
                    sp1C = &sBattleUser->unk_2C;
                    break;

                case 2:
                    sp18 = &sBattleUser->unk_3C;
                    sp1C = &sBattleUser->speed;
                    sp28 = 1;
                    break;

                case 3:
                    sp18 = &sBattleUser->unk_3E;
                    sp1C = &sBattleUser->unk_30;
                    break;
            }

            if (*sp1C != 0x3E7) {
                *sp1C = (*sp18 * gStatStageMultipliers[sBattleUser->unk_5C[sp24] - 1].unk_00) /
                        gStatStageMultipliers[sBattleUser->unk_5C[sp24] - 1].unk_01;
                if (*sp1C >= 0x3E8) {
                    *sp1C = 0x3E7;
                }
            } else {
                goto block_29;
            }
        }

        if (sBattleUser->moveCache.lastMove == MINIMIZE) {
            sBattleUser->scaleResize = 1;
        }

        if (var_a3 == 2) {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[10], sp24 + 0x8D);
        } else {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[10], sp24 + 0x93);
        }

        if ((sp28 != 0) || (D_8438AC60[0] != 1)) {
            ApplyParalysisSpeedCut(sActivePokemon[!sBattleScene.scene->actingSide]);
        }

        if ((sp2C != 0) || (D_8438AC60[0] != 1)) {
            ApplyBurnAttackCut(sActivePokemon[!sBattleScene.scene->actingSide]);
        }
    } else {
    block_29:
        Battle_QueueMessage(sBattleMessageQueues->buffer[10], -0x67);
    }
}

void Battle_Effect_RaiseOwnStat(void) {
    s32 sp2C;
    s32 sp28;
    s32 sp24;
    s32 var_a3;
    u16* sp1C;
    u16* sp18;

    if (sBattleUser->moveCache.lastMove == WITHDRAW) {
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_WITHDRAW;
    } else {
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_SELF_TARGET;
    }

    var_a3 = 1;
    sp24 = sBattleUser->moveCache.unk_01 - 0xA;
    if (sp24 >= 8) {
        var_a3 = 2;
        sp24 -= 0x28;
    }

    if (sBattleUser->unk_5C[sp24] != 0xD) {
        sBattleUser->unk_5C[sp24] += var_a3;
        if (sBattleUser->unk_5C[sp24] >= 0xE) {
            sBattleUser->unk_5C[sp24] = 0xD;
        }

        sp28 = 0;
        sp2C = 0;

        if (sp24 < 4) {
            switch (sp24) {
                case 0:
                    sp18 = &sBattleUser->unk_38;
                    sp1C = &sBattleUser->unk_2A;
                    sp2C = 1;
                    break;

                case 1:
                    sp18 = &sBattleUser->unk_3A;
                    sp1C = &sBattleUser->unk_2C;
                    break;

                case 2:
                    sp18 = &sBattleUser->unk_3C;
                    sp1C = &sBattleUser->speed;
                    sp28 = 1;
                    break;

                case 3:
                    sp18 = &sBattleUser->unk_3E;
                    sp1C = &sBattleUser->unk_30;
                    break;
            }

            if (*sp1C != 0x3E7) {
                *sp1C = (*sp18 * gStatStageMultipliers[sBattleUser->unk_5C[sp24] - 1].unk_00) /
                        gStatStageMultipliers[sBattleUser->unk_5C[sp24] - 1].unk_01;
                if (*sp1C >= 0x3E8) {
                    *sp1C = 0x3E7;
                }
            } else {
                goto block_32;
            }
        }

        if (sBattleUser->moveCache.lastMove == MINIMIZE) {
            sBattleUser->scaleResize = 1;
        }

        if (var_a3 == 2) {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[1], sp24 + 0x8D);
        } else {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[1], sp24 + 0x93);
        }

        if ((sp28 != 0) || (D_8438AC60[0] != 1)) {
            ApplyParalysisSpeedCut(sActivePokemon[sBattleScene.scene->actingSide]);
        }

        if ((sp2C != 0) || (D_8438AC60[0] != 1)) {
            ApplyBurnAttackCut(sActivePokemon[sBattleScene.scene->actingSide]);
        }
    } else {
    block_32:
        sStatCanBeChangedFurther = 0;
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x67);
    }
}

void Battle_Effect_LowerStat(void) {
    s32 sp2C;
    s32 sp28;
    s32 sp24;
    s32 sp20;
    u16* sp1C;
    u16* sp18;

    if (sBattleUser->moveCache.unk_01 < 0x44) {
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_TARGET_STAT_DROP;
    } else {
        sBattleScene.scene->hitReactionKind = BATTLE_HIT_REACTION_SECONDARY;
    }

    if (sBattleTarget->unk_4D & 0x10) {
        goto end;
    }

    sp24 = sBattleUser->moveCache.unk_01;
    sp20 = 1;

    if (sp24 >= 0x44) {
        if (Battle_Random() < 0x55) {
            sp24 -= 0x44;
            goto block_14;
        }
    } else {
        HandleMoveEffectFlags();

        if (sBattleMoveFailed != 0) {
            goto end;
        }

        if (sBattleTarget->immobilizedReason & 0x40) {
            goto end;
        }

        sp24 -= 0x12;
        if (sp24 >= 9) {
            sp20 = 2;
            sp24 -= 0x28;
        }

    block_14:
        if (sBattleTarget->unk_5C[sp24] != 1) {
            sBattleTarget->unk_5C[sp24] -= sp20;
            if (sBattleTarget->unk_5C[sp24] <= 0) {
                sBattleTarget->unk_5C[sp24] = 1;
            }

            sp28 = 0;
            sp2C = 0;

            if (sp24 < 4) {
                switch (sp24) {
                    case 0:
                        sp2C = 1;
                        sp18 = &sBattleTarget->unk_38;
                        sp1C = &sBattleTarget->unk_2A;
                        break;

                    case 1:
                        sp18 = &sBattleTarget->unk_3A;
                        sp1C = &sBattleTarget->unk_2C;
                        break;

                    case 2:
                        sp28 = 1;
                        sp18 = &sBattleTarget->unk_3C;
                        sp1C = &sBattleTarget->speed;
                        break;

                    case 3:
                        sp18 = &sBattleTarget->unk_3E;
                        sp1C = &sBattleTarget->unk_30;
                        break;
                }

                if (*sp1C != 1) {
                    *sp1C = (*sp18 * gStatStageMultipliers[sBattleTarget->unk_5C[sp24] - 1].unk_00) /
                            gStatStageMultipliers[sBattleTarget->unk_5C[sp24] - 1].unk_01;
                    if (*sp1C == 0) {
                        *sp1C = 1;
                    }
                } else {
                    goto block_39;
                }
            }

            if (sp20 == 2) {
                Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                Battle_QueueMessage(sBattleMessageQueues->buffer[7], sp24 + 0x9A);
            } else {
                Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
                Battle_QueueMessage(sBattleMessageQueues->buffer[7], sp24 + 0xA0);
            }

            if ((sp28 != 0) || (D_8438AC60[0] != 1)) {
                ApplyParalysisSpeedCut(sActivePokemon[!sBattleScene.scene->actingSide]);
            }

            if ((sp2C != 0) || (D_8438AC60[0] != 1)) {
                ApplyBurnAttackCut(sActivePokemon[!sBattleScene.scene->actingSide]);
            }
            return;
        }
    }

block_39:
    if (sBattleUser->moveCache.unk_01 < 0x44) {
        sConfused = 0;
        sStatCanBeChangedFurther = 0;
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x5A);
        return;
    }

end:
    if (sBattleUser->moveCache.unk_01 < 0x44) {
        sConfused = 0;
        sStatCanBeChangedFurther = 0;
        if (D_843C4DA9 == 0) {
            Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x59);
        }
    } else {
        sStatCanBeChangedFurther = 0;
    }
}

void Battle_Effect_Bide(void) {
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_BIDE;
    sBattleUser->moveCache.unk_01 = 0;
    sBattleUser->immobilizedReason |= 1;
    sBattleUser->unk_54 = 0;
    sBattleUser->isTrapped = (Battle_Random() & 1) + 2;
}

void Battle_Effect_Thrash(void) {
    sBattleUser->moveCache.unk_01 = 0;
    sBattleUser->immobilizedReason |= 2;
    sBattleUser->isTrapped = (Battle_Random() & 1) + 2;
}

void Battle_Effect_ForceSwitchOrFlee(void) {
    if (sBattleUser->moveCache.lastMove == TELEPORT) {
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_TELEPORT;
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x53);
        return;
    }
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_FAILED;
    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
    Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x52);
}

void Battle_Effect_MultiHit(void) {
    s32 var_v1;

    if (!(sBattleUser->immobilizedReason & 4)) {
        sBattleUser->immobilizedReason |= 4;

        switch (sBattleUser->moveCache.unk_01) {
            case 0x4D:
                sBattleUser->moveCache.unk_01 = 2;

            case 0x2C:
                sBattleUser->unk_54 = 2;
                sBattleUser->isTrapped = sBattleUser->unk_54;
                break;

            default:
                var_v1 = Battle_Random() & 3;
                if (var_v1 >= 2) {
                    var_v1 = Battle_Random() & 3;
                }
                sBattleUser->isTrapped = sBattleUser->unk_54 = var_v1 + 2;
                break;
        }

        sMultihitCount = sBattleUser->unk_54 - 1;
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_MULTI_HIT;
    }
}

void Battle_Effect_Flinch(void) {
    if (!(sBattleTarget->unk_4D & 0x10)) {
        if (D_8438AC60[0] == 1) {
            if (sBattleUser->moveCache.unk_01 == 0x1F) {
                if (Battle_Random() >= 0x1A) {
                    return;
                }
            } else if (Battle_Random() >= 0x4D) {
                return;
            }
        } else {
            sBattleTarget->unk_4D &= ~0x20;
            if (sBattleUser->moveCache.unk_01 == 0x1F) {
                if (Battle_Random() >= 0x1A) {
                    return;
                }
            } else if (Battle_Random() >= 0x4D) {
                return;
            }
        }
        sBattleTarget->immobilizedReason |= 8;
    }
}

void Battle_Effect_OHKO(void) {
    sBattleDamage = 0;
    sBattleCritFlag = 0xFF;
    if (sBattleUser->speed < sBattleTarget->speed) {
        sBattleMoveFailed = 1;
        return;
    }
    sBattleDamage = 0xFFFF;
    sBattleCritFlag = 2;
}

void Battle_Effect_TwoTurnMove(void) {
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_CHARGE;
    sBattleUser->immobilizedReason |= 0x10;

    if ((sBattleUser->moveCache.unk_01 == 0x2B) || (sBattleUser->moveCache.lastMove == DIG)) {
        sBattleUser->immobilizedReason |= 0x40;
        if (sBattleUser->moveCache.lastMove == DIG) {
            sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_DIG_START;
        }

        if (sBattleUser->moveCache.unk_01 == 0x2B) {
            sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_FLY_START;
        }
    }

    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));

    switch (sBattleUser->moveCache.lastMove) {
        case 0xD:
            Battle_QueueMessage(sBattleMessageQueues->buffer[2], -0x51);
            break;

        case 0x4C:
            Battle_QueueMessage(sBattleMessageQueues->buffer[2], -0x50);
            break;

        case 0x82:
            Battle_QueueMessage(sBattleMessageQueues->buffer[2], -0x4F);
            break;

        case 0x8F:
            Battle_QueueMessage(sBattleMessageQueues->buffer[2], -0x4E);
            break;

        case 0x13:
            Battle_QueueMessage(sBattleMessageQueues->buffer[4], -0x4D);
            break;

        case 0x5B:
            Battle_QueueMessage(sBattleMessageQueues->buffer[4], -0x4C);
            break;
    }
}

void Battle_Effect_Trap(void) {
    s32 var_v1;

    if (!(sBattleUser->immobilizedReason & 0x20) && ((D_8438AC60[0] != 1) || (sBattleUser->moveCache.unk_03 != 0) ||
                                         ((sBattleTarget->roster[6] != 8) && (sBattleTarget->roster[7] != 8)))) {
        sBattleTarget->unk_4D &= 0xFFDF;
        sBattleUser->immobilizedReason |= 0x20;
        var_v1 = Battle_Random() & 3;
        if (var_v1 >= 2) {
            var_v1 = Battle_Random() & 3;
        }
        sBattleUser->isTrapped = var_v1 + 1;
    }
}

void Battle_Effect_Mist(void) {
    if (sBattleUser->unk_4D & 2) {
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_SELF_TARGET;
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x4B);
        return;
    }
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_SELF_TARGET;
    sBattleUser->unk_4D |= 2;
    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
    Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x4A);
}

void Battle_Effect_FocusEnergy(void) {
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_SELF_TARGET;
    if (sBattleUser->unk_4D & 4) {
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x49);
        return;
    }
    sBattleUser->unk_4D |= 4;
    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
    Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x48);
}

void Battle_Effect_Recoil(void) {
    s32 var_v1;

    sBattleUser->unk_12 = sBattleUser->displayHp;

    if (sBattleUser->moveCache.lastMove == STRUGGLE) {
        var_v1 = sBattleDamage >> 1;
    } else {
        var_v1 = sBattleDamage >> 2;
    }

    if (var_v1 == 0) {
        var_v1 += 1;
    }

    if (sBattleUser->unk_12 < var_v1) {
        sBattleUser->unk_12 = 0;
    } else {
        sBattleUser->unk_12 -= var_v1;
    }

    sBattleTarget->unk_12 = sBattleTarget->displayHp;
    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
    Battle_QueueMessage(sBattleMessageQueues->buffer[15], -0x47);
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_RECOIL;
}

void Battle_Effect_ConfuseChance(void) {
    if (!(sBattleTarget->unk_4D & 0x10) && (Battle_Random() < 0x1A) && !(sBattleTarget->immobilizedReason & 0x80)) {
        sBattleScene.scene->hitReactionKind = BATTLE_HIT_REACTION_SECONDARY;
        sStatCanBeChangedFurther = 0;
        sBattleTarget->immobilizedReason |= 0x80;
        sBattleTarget->unk_50 = (Battle_Random() & 3) + 2;
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[7], -0x46);
    }
}

void Battle_Effect_Confuse(void) {
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_TARGET_STATUS;
    sConfused = 0;

    if (!(sBattleTarget->unk_4D & 0x10)) {
        HandleMoveEffectFlags();
        if ((sBattleMoveFailed == 0) && !(sBattleTarget->immobilizedReason & 0x80)) {
            sConfused = 1;
            sBattleTarget->immobilizedReason |= 0x80;
            sBattleTarget->unk_50 = (Battle_Random() & 3) + 2;
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x45);
            return;
        }
    }

    if (D_843C4DA9 == 0) {
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x44);
    }
}

void Battle_Effect_Paralyze(void) {
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_TARGET_STATUS;
    if (sBattleTarget->unk_15 == 0) {
        if ((sBattleUser->moveCache.unk_03 != 0x17) || ((sBattleTarget->roster[6] != 4) && (sBattleTarget->roster[7] != 4))) {
            HandleMoveEffectFlags();
            if (sBattleMoveFailed == 0) {
                sBattleTarget->unk_15 |= 0x40;
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
    s32 temp_v1;

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_NO_ACTION;
    if (sBattleUser->unk_4D & 0x10) {
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], BATTLE_MSG_SUBSTITUTE_EXISTS);
        return;
    }

    temp_v1 = sBattleUser->hp >> 2;
    if (temp_v1 >= sBattleUser->displayHp) {
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], BATTLE_MSG_SUBSTITUTE_LOW_HP);
        return;
    }

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_SUBSTITUTE;
    sBattleUser->unk_59 = temp_v1;
    sBattleUser->displayHp -= temp_v1;
    sBattleUser->unk_4D |= 0x10;
    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
    Battle_QueueMessage(sBattleMessageQueues->buffer[1], BATTLE_MSG_SUBSTITUTE_CREATED);
}

void Battle_Effect_Recharge(void) {
    sBattleUser->unk_4D |= 0x20;
}

void Battle_Effect_Rage(void) {
    sBattleUser->unk_4D |= 0x40;
}

void Battle_Effect_Mimic(void) {
    ActivePokemonState* sp24 = &sActivePokemon[sBattleScene.scene->actingSide]->state;
    s32 var_t0;
    u8 temp_v0;

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_SELF_TARGET;
    HandleMoveEffectFlags();

    if ((sBattleMoveFailed == 0) && !(sBattleTarget->immobilizedReason & 0x40)) {
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

    if ((sBattleMoveFailed == 0) && (sBattleTarget->roster[6] != 0x16) && (sBattleTarget->roster[7] != 0x16)) {
        if (!(sBattleTarget->unk_4D & 0x80)) {
            sBattleTarget->unk_4D |= 0x80;
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
    s32 sp24;
    s32 temp_v1;

    HandleMoveEffectFlags();

    if (sBattleMoveFailed == 0) {
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_DISABLE;
        if (sBattleTarget->movesDisabled == 0) {
            do {
                sp24 = Battle_Random() & 3;
            } while (sBattleTarget->moves[sp24] == 0);
            temp_v1 = (Battle_Random() & 7) + 1;
            sBattleTarget->movesDisabled = (sp24 * 0x10) + temp_v1;
            sBattleTarget->unk_57 = sBattleTarget->moves[sp24];
            Text_SetStringToken(0x1E, Text_GetString(NULL, 0, sMoveNameTable, sBattleTarget->unk_57 - 1));
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
    if (!(sBattleTarget->immobilizedReason & 0x40)) {
        sBattleUser->roster[6] = sBattleTarget->roster[6];
        sBattleUser->roster[7] = sBattleTarget->roster[7];
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x38);
    } else {
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x37);
    }
}

void Battle_Effect_Haze(void) {
    s32 i;
    BattleMonRuntime* temp_t0 = &sActivePokemon[0]->state.runtime;
    BattleMonRuntime* temp_a3 = &sActivePokemon[1]->state.runtime;

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_HAZE;

    for (i = 0; i < 8; i++) {
        temp_t0->unk_5C[i] = 7;
        temp_a3->unk_5C[i] = 7;
    }

    temp_t0->unk_2A = temp_t0->unk_38;
    temp_t0->unk_2C = temp_t0->unk_3A;
    temp_t0->speed = temp_t0->unk_3C;
    temp_t0->unk_30 = temp_t0->unk_3E;
    temp_a3->unk_2A = temp_a3->unk_38;
    temp_a3->unk_2C = temp_a3->unk_3A;
    temp_a3->speed = temp_a3->unk_3C;
    temp_a3->unk_30 = temp_a3->unk_3E;

    if ((sBattleTarget->unk_15 & 0x27) && !(sBattleTarget->immobilizedReason & 0x10)) {
        sBattleTarget->usedMove = 0xFE;
    }

    sBattleTarget->unk_15 = 0;
    // clang-format off
    if (D_8438AC60[0] == 1) { sBattleUser->unk_15 = 0; }
    // clang-format on

    temp_t0->movesDisabled = temp_a3->movesDisabled = 0;
    temp_t0->unk_57 = temp_a3->unk_57 = 0;
    temp_t0->immobilizedReason &= ~0x80;
    temp_t0->unk_4D &= 0x78;
    temp_t0->battleStatus3 &= 8;
    temp_a3->immobilizedReason &= ~0x80;
    temp_a3->unk_4D &= 0x78;
    temp_a3->battleStatus3 &= 8;
    sBattleUser->scaleResize = 0;
    sBattleTarget->scaleResize = 0;

    Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x36);
}

void Battle_Effect_Heal(void) {
    u8 temp_v0;
    ActivePokemonState* temp_a1;
    BattleMonRuntime* temp_a2;

    temp_a1 = &sActivePokemon[sBattleScene.scene->actingSide]->state;
    temp_a2 = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_HEAL;
    if (sBattleUser->displayHp != sBattleUser->hp) {
        if (sBattleUser->moveCache.lastMove == REST) {
            temp_v0 = sBattleUser->unk_15;

            sBattleUser->unk_15 = 2;
            temp_a2->roster[temp_a1->rosterIndex + temp_a1->teamMemberIndex * 3] = 2;

            if (temp_v0 != 0) {
                if (D_8438AC60[0] == 1) {
                    sBattleUser->battleStatus3 &= ~1;
                    Battle_ApplyStatStageMultiplier(&sBattleUser->unk_2A, sBattleUser->unk_38, sBattleUser->unk_5C[0]);
                    Battle_ApplyStatStageMultiplier(&sBattleUser->speed, sBattleUser->unk_3C, sBattleUser->unk_5C[2]);
                }
                Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
                Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x35);
            } else {
                Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
                Battle_QueueMessage(sBattleMessageQueues->buffer[1], -0x34);
            }
            sBattleUser->displayHp += sBattleUser->hp;
        } else {
            sBattleUser->displayHp += sBattleUser->hp / 2;
        }

        if (sBattleUser->hp < sBattleUser->displayHp) {
            sBattleUser->displayHp = sBattleUser->hp;
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
    ActivePokemonState* sp20;
    ActivePokemonState* sp1C;
    BattleMonRuntime* ptr;

    sp20 = &sActivePokemon[sBattleScene.scene->actingSide]->state;
    sp1C = &sActivePokemon[!sBattleScene.scene->actingSide]->state;

    if ((D_8438AC60[0] == 1) && ((((sBattleTarget->battleStatus3 & 8) == 0) && (sBattleTarget->id == 0x84)) ||
                                 (((sBattleTarget->battleStatus3 & 8) != 0) && (sp1C->unk_BE == 0x84)))) {
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_NO_ACTION;
        Text_SetStringToken(0x1B, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], BATTLE_MSG_TRANSFORM_DITTO_BLOCKED);
        return;
    }

    ptr = sBattleUser;
    if (!(ptr->immobilizedReason & 0x40) &&
        ((D_8438AC60[0] != 1) || (sBattleTarget->moves[0] != 0x90) || (sBattleTarget->moves[1] != 0))) {
        sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_TRANSFORM;
        Text_SetStringToken(0x1B, Battle_GetActiveMonNickname(sActivePokemon[!sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[4], -0x31);
        sBattleUser->battleStatus3 |= 8;

        if (sBattleTarget->battleStatus3 & 8) {
            sp20->unk_BE = sp1C->unk_BE;
        } else {
            sp20->unk_BE = sBattleTarget->id;
        }

        sBattleUser->roster[6] = sBattleTarget->roster[6];
        sBattleUser->roster[7] = sBattleTarget->roster[7];
        sBattleUser->moves[0] = sBattleTarget->moves[0];
        sBattleUser->moves[1] = sBattleTarget->moves[1];
        sBattleUser->moves[2] = sBattleTarget->moves[2];
        sBattleUser->moves[3] = sBattleTarget->moves[3];
        sBattleUser->unk_24 = sBattleTarget->unk_24;
        sBattleUser->unk_2A = sBattleTarget->unk_2A;
        sBattleUser->unk_2C = sBattleTarget->unk_2C;
        sBattleUser->speed = sBattleTarget->speed;
        sBattleUser->unk_30 = sBattleTarget->unk_30;

        for (i = 0; i < 4; i++) {
            if (sBattleTarget->moves[i] == 0) {
                break;
            }
            sBattleUser->pp[i] = 5;
        }

        sBattleUser->unk_38 = sBattleTarget->unk_38;
        sBattleUser->unk_3A = sBattleTarget->unk_3A;
        sBattleUser->unk_3C = sBattleTarget->unk_3C;
        sBattleUser->unk_3E = sBattleTarget->unk_3E;

        for (i = 0; i < 8; i++) {
            sBattleUser->unk_5C[i] = sBattleTarget->unk_5C[i];
        }
        return;
    }

    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_NO_ACTION;
    Battle_QueueMessage(sBattleMessageQueues->buffer[1], BATTLE_MSG_TRANSFORM_FAILED);
}

void Battle_Effect_ReflectLightScreen(void) {
    sBattleScene.scene->moveEffectKind = MOVE_PRESENTATION_SELF_TARGET;
    if (sBattleUser->moveCache.unk_01 == 0x40) {
        if (!(sBattleUser->battleStatus3 & 2)) {
            sBattleUser->battleStatus3 |= 2;
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
            Battle_QueueMessage(sBattleMessageQueues->buffer[1], BATTLE_MSG_LIGHT_SCREEN_SET);
        } else {
            Battle_QueueMessage(sBattleMessageQueues->buffer[1], BATTLE_MSG_LIGHT_SCREEN_FAILED);
        }
    } else if (!(sBattleUser->battleStatus3 & 4)) {
        sBattleUser->battleStatus3 |= 4;
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->actingSide]));
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], BATTLE_MSG_REFLECT_SET);
    } else {
        Battle_QueueMessage(sBattleMessageQueues->buffer[1], BATTLE_MSG_REFLECT_FAILED);
    }
}

void Battle_RunMoveEffect(void) {
    BattleMonRuntime* ptr = &sActivePokemon[sBattleScene.scene->actingSide]->state.runtime;
    s32 idx = ptr->moveCache.unk_01;

    if (sMoveEffectHandlers[idx - 1] != NULL) {
        sMoveEffectHandlers[idx - 1]();
    }
}

void Battle_ExecuteMoveEffect(void) {
    Battle_RunMoveEffect();
}
