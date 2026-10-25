#include "battle_engine.h"
#include "src/graphics_textures.h"
#include "src/ui_graphics.h"
#include "src/status_icons.h"
#include "src/game_state.h"
#include "src/text_system.h"
#include "src/3D140.h"
#include "src/gfx_buffer.h"
#include "src/gfx_rect.h"
#include "src/matrix.h"
#include "src/controller.h"
#include "src/memmap.h"
#include "src/memory.h"
#include "stdarg.h"
#include "moves.h"

BattleMessageQueue* sBattleMessageQueues;

static u8 D_84385990[] = { SWORDS_DANCE, GROWTH };
static u8 D_84385994[] = { AGILITY, HARDEN, METRONOME, EGG_BOMB, KINESIS };
static u8 D_8438599C[] = { QUICK_ATTACK, NIGHT_SHADE, SCREECH, RECOVER, SOFTBOILED };
static u8 D_843859A4[] = {
    POUND,       SCRATCH,       VICEGRIP,     WING_ATTACK,   FLY,          BIND,        SLAM,          HORN_ATTACK,
    BODY_SLAM,   WRAP,          THRASH,        POISON_STING,  BITE,         GROWL,       ROAR,          SING,
    SUPERSONIC,  DRILL_PECK,    SEISMIC_TOSS,  ABSORB,        MEGA_DRAIN,   DRAGON_RAGE, FISSURE,       DIG,
    TOXIC,       CONFUSION,     DOUBLE_TEAM,   MINIMIZE,      SMOKESCREEN, DEFENSE_CURL, BARRIER,      MIRROR_MOVE,
    SMOG,        SWIFT,         AMNESIA,       BARRAGE,       LOVELY_KISS,  DIZZY_PUNCH, PSYWAVE,       ACID_ARMOR,
    CRABHAMMER,  BONEMERANG,    ROCK_SLIDE,    CONVERSION,    SUBSTITUTE,  STRUGGLE,
};

void Battle_ClearAllMessageQueues(void) {
    sBattleMessageQueues->buffer[1][0] = 0;
    sBattleMessageQueues->buffer[1][1] = 0;
    sBattleMessageQueues->buffer[2][0] = 0;
    sBattleMessageQueues->buffer[2][1] = 0;
    sBattleMessageQueues->buffer[3][0] = 0;
    sBattleMessageQueues->buffer[3][1] = 0;
    sBattleMessageQueues->buffer[4][0] = 0;
    sBattleMessageQueues->buffer[4][1] = 0;
    sBattleMessageQueues->buffer[5][0] = 0;
    sBattleMessageQueues->buffer[5][1] = 0;
    sBattleMessageQueues->buffer[6][0] = 0;
    sBattleMessageQueues->buffer[6][1] = 0;
    sBattleMessageQueues->buffer[16][0] = 0;
    sBattleMessageQueues->buffer[16][1] = 0;
    sBattleMessageQueues->buffer[17][0] = 0;
    sBattleMessageQueues->buffer[17][1] = 0;
    sBattleMessageQueues->buffer[7][0] = 0;
    sBattleMessageQueues->buffer[7][1] = 0;
    sBattleMessageQueues->buffer[11][0] = 0;
    sBattleMessageQueues->buffer[11][1] = 0;
    sBattleMessageQueues->buffer[10][0] = 0;
    sBattleMessageQueues->buffer[10][1] = 0;
    sBattleMessageQueues->buffer[12][0] = 0;
    sBattleMessageQueues->buffer[12][1] = 0;
    sBattleMessageQueues->buffer[13][0] = 0;
    sBattleMessageQueues->buffer[13][1] = 0;
    sBattleMessageQueues->buffer[14][0] = 0;
    sBattleMessageQueues->buffer[14][1] = 0;
    sBattleMessageQueues->buffer[15][0] = 0;
    sBattleMessageQueues->buffer[15][1] = 0;
}

void Battle_ClearMessageCursor(void) {
    sBattleMessageQueues->unk_004 = 0;
    sBattleMessageQueues->buffer[0][0] = 0;
    sBattleMessageQueues->buffer[0][1] = 0;
    sBattleMessageQueues->unk_008[0] = 0;
    sBattleMessageQueues->unk_008[1] = 0;
}

void Battle_SimpleStringFormat(s8* buffer, s8* format, ...) {
    va_list args;
    s8* src;
    s8* fmt;
    s32 len;

    len = 0;
    va_start(args, format);
    fmt = format;

    while (*fmt != 0) {
        if (*fmt != '%') {
            buffer[len++] = *fmt;
        } else {
            fmt++;
            if (*fmt == 's') {
                src = va_arg(args, s8*);
                while (*src != 0) {
                    buffer[len] = *src;
                    src++;
                    len++;
                }
            }
        }
        fmt++;
    }
    buffer[len] = 0;
}

void Battle_QueueMessage(s8* text, s8 file_number_index) {
    u8 overflow_helper = file_number_index;
    *text = file_number_index;

    if (overflow_helper >= 128) {
        if ((overflow_helper - 128) >= 11) {
            Text_GetString(text + 1, 0x40, sBattleMessagesTable, overflow_helper - 0x48);
        } else {
            Text_GetString(text + 1, 0x40, sBattleMessagesTable, overflow_helper - 0x49);
        }
    } else {
        Text_GetString(text + 1, 0x40, sBattleMessagesTable, sBattleMessagesTableFileNumbers[file_number_index]);
    }
}

void Battle_QueueStatusText(char* text, s32 side) {
    UNUSED s32 pad[3];
    ActivePokemonState* state = &sActivePokemon[side]->state;
    UNUSED s32 pad2[2];

    Battle_ClearMessageCursor();
    sBattleMessageQueues->controllerPort = sActivePokemon[side]->sessionTeams->teamMember[state->teamMemberIndex]->controllerPort;
    Battle_SimpleStringFormat(sBattleMessageQueues->buffer[0], text + 1);
    *text = 0;
}

void func_84317B38(void) {
}

void Battle_DrawCompactStatusBoxesIfVisible(void) {
    ActivePokemon* pokemon1 = sActivePokemon[0];
    ActivePokemon* pokemon2 = sActivePokemon[1];

    if ((gSessionContext.mode != SESSION_MODE_ATTRACT_DEMO_BATTLE) && (sBattleScene.scene->unk_30 == 0)) {
        BattleHud_DrawCompactMonStatusBox(pokemon1, 21, 15, pokemon1->sessionTeams->teamMember[pokemon1->thumbnail.tagTeam]->controllerPort);
        BattleHud_DrawCompactMonStatusBox(pokemon2, 225, 15, pokemon2->sessionTeams->teamMember[pokemon2->thumbnail.tagTeam]->controllerPort);
    }
}

void Battle_QueueMoveCategoryUsedMessage(void) {
    u32 file_number_offset;
    BattleMonRuntime* runtime = &sActivePokemon[sBattleScene.scene->side]->state.runtime;

    runtime->lastMove = runtime->moveCache.lastMove;

    // TODO: find out what makes these attack require special handling
    if (BattleScene_ByteArrayContains(runtime->moveCache.lastMove, D_84385990, 2) != 0) {
        file_number_offset = 0;
    } else if (BattleScene_ByteArrayContains(runtime->moveCache.lastMove, D_84385994, 5) != 0) {
        file_number_offset = 1;
    } else if (BattleScene_ByteArrayContains(runtime->moveCache.lastMove, D_8438599C, 5) != 0) {
        file_number_offset = 2;
    } else if (BattleScene_ByteArrayContains(runtime->moveCache.lastMove, D_843859A4, 46) != 0) {
        file_number_offset = 3;
    } else {
        file_number_offset = 4;
    }

    if ((runtime->moveCache.lastMove == METRONOME) || (runtime->moveCache.lastMove == MIRROR_MOVE)) {
        Text_SetStringToken(25, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->side]));
        Text_SetStringToken(29, Battle_GetActiveMoveName(sActivePokemon[sBattleScene.scene->side]));

        switch (file_number_offset) {
            case 0:
                Battle_QueueMessage(sBattleMessageQueues->buffer[13], 0x14);
                break;

            case 1:
                Battle_QueueMessage(sBattleMessageQueues->buffer[13], 0x15);
                break;

            case 2:
                Battle_QueueMessage(sBattleMessageQueues->buffer[13], 0x16);
                break;

            case 3:
                Battle_QueueMessage(sBattleMessageQueues->buffer[13], 0x17);
                break;

            case 4:
                Battle_QueueMessage(sBattleMessageQueues->buffer[13], 0x18);
                break;
        }
    } else {
        Text_SetStringToken(25, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->side]));
        Text_SetStringToken(29, Battle_GetActiveMoveName(sActivePokemon[sBattleScene.scene->side]));

        switch (file_number_offset) {
            case 0:
                Battle_QueueMessage(sBattleMessageQueues->buffer[2], 0x14);
                break;

            case 1:
                Battle_QueueMessage(sBattleMessageQueues->buffer[2], 0x15);
                break;

            case 2:
                Battle_QueueMessage(sBattleMessageQueues->buffer[2], 0x16);
                break;

            case 3:
                Battle_QueueMessage(sBattleMessageQueues->buffer[2], 0x17);
                break;

            case 4:
                Battle_QueueMessage(sBattleMessageQueues->buffer[2], 0x18);
                break;
        }
    }
}

void Battle_QueueMoveCategoryMessage(void) {
    u32 file_number_offset;
    UNUSED s32 pad;
    BattleMonRuntime* runtime = &sActivePokemon[sBattleScene.scene->side]->state.runtime;

    if (BattleScene_ByteArrayContains(runtime->usedMove, D_84385990, 2) != 0) {
        file_number_offset = 0;
    } else if (BattleScene_ByteArrayContains(runtime->usedMove, D_84385994, 5) != 0) {
        file_number_offset = 1;
    } else if (BattleScene_ByteArrayContains(runtime->usedMove, D_8438599C, 5) != 0) {
        file_number_offset = 2;
    } else if (BattleScene_ByteArrayContains(runtime->usedMove, D_843859A4, 46) != 0) {
        file_number_offset = 3;
    } else {
        file_number_offset = 4;
    }

    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->side]));
    Text_SetStringToken(0x1D, Battle_GetActiveMoveName(sActivePokemon[sBattleScene.scene->side]));

    switch (file_number_offset) {
        case 0:
            Battle_QueueMessage(sBattleMessageQueues->buffer[2], 0x14);
            break;

        case 1:
            Battle_QueueMessage(sBattleMessageQueues->buffer[2], 0x15);
            break;

        case 2:
            Battle_QueueMessage(sBattleMessageQueues->buffer[2], 0x16);
            break;

        case 3:
            Battle_QueueMessage(sBattleMessageQueues->buffer[2], 0x17);
            break;

        case 4:
            Battle_QueueMessage(sBattleMessageQueues->buffer[2], 0x18);
            break;
    }
}

void Battle_QueueMetronomeMoveMessage(void) {
    s32 file_name_offset;
    s32 metronome_selected_move;
    BattleMonRuntime* runtime;

    runtime = &sActivePokemon[sBattleScene.scene->side]->state.runtime;

    if (BattleScene_ByteArrayContains(runtime->usedMove, D_84385990, 2) != 0) {
        file_name_offset = 0;
    } else if (BattleScene_ByteArrayContains(runtime->usedMove, D_84385994, 5) != 0) {
        file_name_offset = 1;
    } else if (BattleScene_ByteArrayContains(runtime->usedMove, D_8438599C, 5) != 0) {
        file_name_offset = 2;
    } else if (BattleScene_ByteArrayContains(runtime->usedMove, D_843859A4, 0x2E) != 0) {
        file_name_offset = 3;
    } else {
        file_name_offset = 4;
    }

    if (runtime->usedMove == METRONOME) {
        do {
            metronome_selected_move = Battle_Random();
        } while ((metronome_selected_move == 0) || (metronome_selected_move >= STRUGGLE) || metronome_selected_move == METRONOME);

        runtime->usedMove = metronome_selected_move;

        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->side]));
        Text_SetStringToken(0x1D, Battle_GetActiveMoveName(sActivePokemon[sBattleScene.scene->side]));

        switch (file_name_offset) {
            case 0:
                Battle_QueueMessage(sBattleMessageQueues->buffer[2], 0x14);
                break;

            case 1:
                Battle_QueueMessage(sBattleMessageQueues->buffer[2], 0x15);
                break;

            case 2:
                Battle_QueueMessage(sBattleMessageQueues->buffer[2], 0x16);
                break;

            case 3:
                Battle_QueueMessage(sBattleMessageQueues->buffer[2], 0x17);
                break;

            case 4:
                Battle_QueueMessage(sBattleMessageQueues->buffer[2], 0x18);
                break;
        }

        runtime->usedMove = METRONOME;

        Text_SetStringToken(25, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->side]));
        Text_SetStringToken(29, Battle_GetActiveMoveName(sActivePokemon[sBattleScene.scene->side]));

        switch (file_name_offset) {
            case 0:
                Battle_QueueMessage(sBattleMessageQueues->buffer[13], 0x14);
                break;

            case 1:
                Battle_QueueMessage(sBattleMessageQueues->buffer[13], 0x15);
                break;

            case 2:
                Battle_QueueMessage(sBattleMessageQueues->buffer[13], 0x16);
                break;

            case 3:
                Battle_QueueMessage(sBattleMessageQueues->buffer[13], 0x17);
                break;

            case 4:
                Battle_QueueMessage(sBattleMessageQueues->buffer[13], 0x18);
                break;
        }
    } else {
        Text_SetStringToken(25, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->side]));
        Text_SetStringToken(29, Battle_GetActiveMoveName(sActivePokemon[sBattleScene.scene->side]));

        switch (file_name_offset) {
            case 0:
                Battle_QueueMessage(sBattleMessageQueues->buffer[2], 0x14);
                break;

            case 1:
                Battle_QueueMessage(sBattleMessageQueues->buffer[2], 0x15);
                break;

            case 2:
                Battle_QueueMessage(sBattleMessageQueues->buffer[2], 0x16);
                break;

            case 3:
                Battle_QueueMessage(sBattleMessageQueues->buffer[2], 0x17);
                break;

            case 4:
                Battle_QueueMessage(sBattleMessageQueues->buffer[2], 0x18);
                break;
        }
    }
}

void BattleBox_Open(s32 arg0) {
    BattleBox* text_box = &sBattleMessageQueues->textBox;

    text_box->visibility = 0;
    text_box->unk_04 = arg0;
}

void BattleBox_Close(void) {
    BattleBox* battle_box = &sBattleMessageQueues->textBox;

    battle_box->visibility = 0xFF;
}

void BattleBox_PlayTextBlip(s8* arg0) {
    s32 temp_v0;

    if (arg0[1] == '\x00') {
        return;
    }

    temp_v0 = arg0[0] & 0x7F;

    if ((arg0[0] & 0x80) == '\x00') {
        Audio_QueuePendingSoundId(arg0[0]);
    } else if (temp_v0 == 'Q') {
        Audio_QueuePendingSoundId(0xBC);
    } else if (temp_v0 == 'R') {
        Audio_QueuePendingSoundId(0xB5);
    } else if (temp_v0 == 'S') {
        Audio_QueuePendingSoundId(0xB6);
    } else if (temp_v0 == 'T') {
        Audio_QueuePendingSoundId(0xB7);
    } else if (temp_v0 == 'U') {
        Audio_QueuePendingSoundId(0xB8);
    } else if (temp_v0 == 'V') {
        Audio_QueuePendingSoundId(0xB9);
    } else if (temp_v0 == 'W') {
        Audio_QueuePendingSoundId(0xBA);
    } else if (temp_v0 == 'X') {
        Audio_QueuePendingSoundId(0xBB);
    } else {
        Audio_QueuePendingSoundId(temp_v0 + 0x64);
    }
}

void BattleBox_DrawMessage(BattleBox* box, s16 x, s16 y) {
    UNUSED s32 pad;
    s32 temp_s1;

    if (1) {}// karodo

    switch (box->unk_04) {
        case 0:
            if (Battle_DrawTextIfNonEmpty(&sBattleMessageQueues->buffer[13][1], x + 4, y + (box->lineCount * 12) + 3) != 0) {
                box->lineCount += 1;
            }

            if (Battle_DrawTextIfNonEmpty(&sBattleMessageQueues->buffer[2][1], x + 4, y + (box->lineCount * 12) + 3) != 0) {
                box->lineCount += 1;
            }

            if (Battle_DrawTextIfNonEmpty(&sBattleMessageQueues->buffer[5][1], x + 4, y + (box->lineCount * 12) + 3) != 0) {
                box->lineCount += 1;
            }
            break;

        case 3:
            if (Battle_DrawTextIfNonEmpty(&sBattleMessageQueues->buffer[1][1], x + 4, y + (box->lineCount * 12) + 3) != 0) {
                box->lineCount += 1;
            }

            if (Battle_DrawTextIfNonEmpty(&sBattleMessageQueues->buffer[4][1], x + 4, y + (box->lineCount * 12) + 3) != 0) {
                box->lineCount += 1;
            }

            if (Battle_DrawTextIfNonEmpty(&sBattleMessageQueues->buffer[7][1], x + 4, y + (box->lineCount * 12) + 3) != 0) {
                box->lineCount += 1;
            }
            break;

        case 1:
            if (Battle_DrawTextIfNonEmpty(&sBattleMessageQueues->buffer[14][1], x + 4, y + (box->lineCount * 12) + 3) != 0) {
                box->lineCount += 1;
            }

            if (Battle_DrawTextIfNonEmpty(&sBattleMessageQueues->buffer[1][1], x + 4, y + (box->lineCount * 12) + 3) != 0) {
                box->lineCount += 1;
            }

            if (Battle_DrawTextIfNonEmpty(&sBattleMessageQueues->buffer[4][1], x + 4, y + (box->lineCount * 12) + 3) != 0) {
                box->lineCount += 1;
            }

            if ((box->lineCount < 3) &&
                (Battle_DrawTextIfNonEmpty(&sBattleMessageQueues->buffer[7][1], x + 4, y + (box->lineCount * 12) + 3) != 0)) {
                box->lineCount += 1;
            }
            break;

        case 2:
            if (Battle_DrawTextIfNonEmpty(&sBattleMessageQueues->buffer[1][1], x + 4, y + (box->lineCount * 12) + 3) != 0) {
                box->lineCount += 1;
            }

            if (Battle_DrawTextIfNonEmpty(&sBattleMessageQueues->buffer[4][1], x + 4, y + (box->lineCount * 12) + 3) != 0) {
                box->lineCount += 1;
            }

            if (Battle_DrawTextIfNonEmpty(&sBattleMessageQueues->buffer[7][1], x + 4, y + (box->lineCount * 12) + 3) != 0) {
                box->lineCount += 1;
            }
            break;

        case 4:
            Battle_DrawTextIfNonEmpty(&sBattleMessageQueues->buffer[8][1], x + 4, y + 3);
            break;

        case 5:
            Battle_DrawTextIfNonEmpty(&sBattleMessageQueues->buffer[9][1], x + 4, y + 3);
            break;

        case 6:
            Battle_DrawTextIfNonEmpty(&sBattleMessageQueues->buffer[15][1], x + 4, y + 3);
            break;

        case 7:
            Battle_DrawTextIfNonEmpty(&sBattleMessageQueues->buffer[12][1], x + 4, y + 3);
            Battle_DrawTextIfNonEmpty(&sBattleMessageQueues->buffer[10][1], x + 4, y + 0x10);
            break;

        case 8:
            Battle_DrawTextIfNonEmpty(&sBattleMessageQueues->buffer[16 + box->side][1], x + 4, y + 3);
            break;

        case 9:
        case 10:
            Battle_DrawTextIfNonEmpty(&sBattleMessageQueues->buffer[6][1], x + 4, y + 3);
            break;

        case 11:
            Battle_DrawTextIfNonEmpty(&sBattleMessageQueues->buffer[1][1], x + 4, y + 3);
            break;

        case 12:
            Battle_DrawTextIfNonEmpty(&sBattleMessageQueues->buffer[1][1], x + 4, y + 3);
            break;

        case 13:
            temp_s1 = Font_MeasureTextExtent(1, 0, Text_GetString(NULL, 0, sBattleMessagesTable, 0xA4)) / 2;
            Battle_DrawTextIfNonEmpty(Text_GetString(NULL, 0, sBattleMessagesTable, 0xA4), 0xA1 - temp_s1, 0x71);
            break;

        case 14:
            Battle_DrawEndBattleBanner();
            break;

        case 15:
            Battle_DrawTextIfNonEmpty(&sBattleMessageQueues->buffer[1][1], x + 4, y + (box->lineCount * 0xC) + 3);
            break;
    }

    box->lineCount = 0;
}

s32 BattleBox_SelectMessage(BattleBox* box) {
    BattleMessageQueue* temp_v0;
    s32 on_side1;

    box->lineCount = 0;

    switch (box->unk_04) {
        case 0:
            if ((sBattleMessageQueues->buffer[2][1] == 0) && (sBattleMessageQueues->buffer[13][1] == 0) && (sBattleMessageQueues->buffer[5][1] == 0)) {
                box->visibility = 0xFF;
                return 1;
            }

            box->side = sBattleScene.scene->side;
            if (sBattleMessageQueues->buffer[2][1] != 0) {
                BattleBox_PlayTextBlip(sBattleMessageQueues->buffer[2]);
            }

            if (sBattleMessageQueues->buffer[13][1] != 0) {
                BattleBox_PlayTextBlip(sBattleMessageQueues->buffer[13]);
            }

            if (sBattleMessageQueues->buffer[5][1] != 0) {
                BattleBox_PlayTextBlip(sBattleMessageQueues->buffer[5]);
            }
            box->visibility += 1;
            break;

        case 1:
        case 3:
            box->side = sBattleScene.scene->side;
            BattleBox_PlayTextBlip(sBattleMessageQueues->buffer[1]);
            BattleBox_PlayTextBlip(sBattleMessageQueues->buffer[4]);
            BattleBox_PlayTextBlip(sBattleMessageQueues->buffer[7]);
            BattleBox_PlayTextBlip(sBattleMessageQueues->buffer[14]);
            BattleBox_PlayTextBlip(sBattleMessageQueues->buffer[11]);
            if ((sBattleMessageQueues->buffer[1][1] == 0) && (sBattleMessageQueues->buffer[4][1] == 0) && (sBattleMessageQueues->buffer[7][1] == 0) &&
                (sBattleMessageQueues->buffer[14][1] == 0) && (sBattleMessageQueues->buffer[11][1] == 0)) {
                box->visibility = 0xFF;
                return 1;
            }
            box->visibility += 1;
            break;

        case 2:
            box->side = sBattleScene.scene->side;
            BattleBox_PlayTextBlip(sBattleMessageQueues->buffer[1]);
            BattleBox_PlayTextBlip(sBattleMessageQueues->buffer[4]);
            BattleBox_PlayTextBlip(sBattleMessageQueues->buffer[7]);
            if ((sBattleMessageQueues->buffer[1][1] == 0) && (sBattleMessageQueues->buffer[4][1] == 0) && (sBattleMessageQueues->buffer[7][1] == 0)) {
                box->visibility = 0xFF;
                return 1;
            }
            box->visibility += 1;
            break;

        case 15:
            box->side = sBattleScene.scene->side;
            BattleBox_PlayTextBlip(sBattleMessageQueues->buffer[1]);
            if (sBattleMessageQueues->buffer[1][1] == 0) {
                box->visibility = 0xFF;
                return 1;
            }
            box->visibility += 1;
            break;

        case 8:
            if (sBattleMessageQueues->buffer[16 + box->side][1] == 0) {
                box->visibility = 0xFF;
                return 1;
            }
            on_side1 = sBattleScene.scene->side == 0;
            box->visibility += 1;
            box->side = on_side1;
            break;

        case 9:
            on_side1 = sBattleScene.scene->side == 0;
            box->visibility += 1;
            box->side = on_side1;
            break;

        case 4:
        case 5:
        case 6:
            BattleBox_PlayTextBlip(sBattleMessageQueues->buffer[8]);
            BattleBox_PlayTextBlip(sBattleMessageQueues->buffer[9]);
            BattleBox_PlayTextBlip(sBattleMessageQueues->buffer[15]);
            box->visibility += 1;
            break;

        case 14:
            sEndBattleBannerPos.z = 0.0f;
            sEndBattleBannerPos.x = 321.0f;
            sEndBattleBannerPos.y = 160.0f;
            box->side = sBattleScene.scene->side;
            box->visibility += 1;
            break;

        default:
            box->side = sBattleScene.scene->side;
            box->visibility += 1;
            break;
    }
    return 0;
}

void BattleBox_Update(s16 arg0, s16 arg1) {
    BattleBox* temp_s0 = &sBattleMessageQueues->textBox;

    if (gSessionContext.mode == 0x10) {
        return;
    }

    switch (temp_s0->visibility) {
        case 0:
            temp_s0->displayTimer = 0;
            if (temp_s0->unk_04 == 0xD) {
                BattleHud_InitCaptionBoxPopupBlack(temp_s0, 0xA0, 0x78);
                temp_s0->visibility++;
            } else if (temp_s0->unk_04 == 0xE) {
                BattleBox_SelectMessage(temp_s0);
            } else if (BattleBox_SelectMessage(temp_s0) == 0) {
                BattleHud_InitCaptionBoxPopup(temp_s0, arg0, arg1);
            }
            break;

        case 1:
            if (temp_s0->unk_04 == 0xD) {
                if (BattleHud_UpdateCaptionBoxPopupBlack(temp_s0, 0xA0, 0x78) != 0) {
                    BattleBox_DrawMessage(temp_s0, temp_s0->textOrigin.x, temp_s0->textOrigin.y);
                    temp_s0->visibility++;
                }
            } else {
                if (temp_s0->unk_04 == 0xE) {
                    ProcessAudioCommandQueue(BattleScene_GetParticipantDisplayPlayerNumber(sActivePokemon[temp_s0->side]));
                    BattleBox_DrawMessage(temp_s0, temp_s0->textOrigin.x, temp_s0->textOrigin.y);
                    temp_s0->visibility++;
                } else if (BattleHud_UpdateCaptionBoxPopup(temp_s0, arg0, arg1) != 0) {
                    ProcessAudioCommandQueue(BattleScene_GetParticipantDisplayPlayerNumber(sActivePokemon[temp_s0->side]));
                    BattleBox_DrawMessage(temp_s0, temp_s0->textOrigin.x, temp_s0->textOrigin.y);
                    temp_s0->visibility++;
                }
            }
            break;

        case 2:
            if (temp_s0->unk_04 == 0xD) {
                BattleHud_UpdateCaptionBoxPopupBlack(temp_s0, 0xA0, 0x78);
                BattleBox_DrawMessage(temp_s0, temp_s0->textOrigin.x, temp_s0->textOrigin.y);

                temp_s0->displayTimer++;
                if (temp_s0->displayTimer == 0x46) {
                    temp_s0->visibility++;
                }
            } else {
                if (temp_s0->unk_04 == 0xE) {
                    BattleBox_DrawMessage(temp_s0, temp_s0->textOrigin.x, temp_s0->textOrigin.y);
                } else {
                    BattleHud_UpdateCaptionBoxPopup(temp_s0, arg0, arg1);
                    BattleBox_DrawMessage(temp_s0, temp_s0->textOrigin.x, temp_s0->textOrigin.y);

                    temp_s0->displayTimer++;
                    if (temp_s0->displayTimer == 0x2D) {
                        temp_s0->visibility++;
                    }
                }
            }
            break;
    }
}

void BattleScene_ClearParticipantMessageBuffer(ActivePokemon* active_pokemon) {
    active_pokemon->captionState.active = 0;
    active_pokemon->captionState.unk_04 = 0;
    active_pokemon->captionState.unk_48[0] = 0;
    active_pokemon->captionState.unk_08[0] = 0;
}

void BattleScene_BuildParticipantMessage(ActivePokemon* arg0, s32 arg1) {
    sBattleMessageQueues->controllerPort = arg0->sessionTeams->teamMember[arg0->state.ownerTeamMemberIndex]->controllerPort;
    arg0->captionState.active = 1;
    Text_SetStringToken(0x19, arg0->teamRoster->pokemonPicked[arg0->state.rosterIndex].nickname);
    Text_GetString(arg0->captionState.unk_48, 0x40, sBattleMessagesTable, arg1);
}

void BattleScene_RevealParticipantMessageChars(ActivePokemon* arg0) {
    TeamRoster* temp_t0 = arg0->sessionTeams->teamMember[arg0->state.ownerTeamMemberIndex];

    if (arg0->captionState.unk_48[arg0->captionState.unk_04] != '\x00') {
        arg0->captionState.unk_08[arg0->captionState.unk_04] = arg0->captionState.unk_48[arg0->captionState.unk_04];
        arg0->captionState.unk_04++;
    }

    if (arg0->captionState.unk_48[arg0->captionState.unk_04] != '\x00') {
        arg0->captionState.unk_08[arg0->captionState.unk_04] = arg0->captionState.unk_48[arg0->captionState.unk_04];
        arg0->captionState.unk_04++;
    }

    arg0->captionState.unk_08[arg0->captionState.unk_04] = '\x00';

    if (arg0->captionState.unk_48[0] != 0) {
        if (arg0 == sSide1ActivePokemon) {
            BattleHud_DrawTextBox(temp_t0->controllerPort, 0x5F, 0xE, &arg0->captionState.unk_08);
        } else {
            BattleHud_DrawTextBox(temp_t0->controllerPort, 0x17, 0xBB, &arg0->captionState.unk_08);
        }
    }
}

void BattleScene_TickHudTimers(void) {
    ActivePokemonState* sp1C = &sActivePokemon[0]->state;
    ActivePokemonState* sp18 = &sActivePokemon[1]->state;

    if ((gSessionContext.timeLimitsEnabled != 0) && (sActivePokemon[1]->thumbnail.camera->viewport.x == 0x103) &&
        (sActivePokemon[0]->thumbnail.camera->viewport.x == 0x17)) {
        BattleHud_DrawBattleTimer(0x101, 0x64);
        if (D_84390134 == 0) {
            if (sp1C->selectedCommandOption != 3) {
                BattleHud_DrawParticipantTimer(0x46, 0x4D);
            }

            if (sp18->selectedCommandOption != 3) {
                BattleHud_DrawParticipantTimer(0xE8, 0x96);
            }
        }
    }
}

void Battle_InitMessageQueues(void) {
    sBattleMessageQueues = main_pool_alloc(sizeof(BattleMessageQueue), 0);
    bzero(sBattleMessageQueues, sizeof(BattleMessageQueue));
}

void BattleScene_ResetMessageQueue(void) {
    sBattleMessageQueues->unk_004 = 0;
    sBattleMessageQueues->unk_008[sBattleMessageQueues->unk_004] = 0;
    Battle_ClearMessageCursor();
}

void Battle_RenderHud(void) {
    ActivePokemonState* side1_state = &sActivePokemon[0]->state;
    ActivePokemonState* side2_state = &sActivePokemon[1]->state;

    if (1) {}

    BattleBox_Update(0xA0, 0xC8);

    if (gSessionContext.mode == SESSION_MODE_ATTRACT_DEMO_BATTLE) {
        Battle_DrawDemoControllerWarning();
        Battle_DrawDemoStadiumLogo();
    }

    switch (sBattleScene.scene->unk_1C) {
        case 1:
            BattleHud_DrawOwnSideStatus(sActivePokemon[0], side1_state);
            BattleHud_DrawOpponentSideStatus(sActivePokemon[1], side2_state);
            BattleScene_RevealParticipantMessageChars(sActivePokemon[0]);
            BattleScene_RevealParticipantMessageChars(sActivePokemon[1]);
            BattleScene_TickHudTimers();
            BattleScene_DrawStripWipeIfActive();
            break;

        case 2:
            Battle_DrawCompactStatusBoxesIfVisible();
            BattleScene_DrawStripWipeIfActive();
            break;
    }
}
