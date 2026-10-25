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

unk_D_843901A0* gBattleMessageQueues;

static u8 D_84385990[] = { 0xE, 0x4A };
static u8 D_84385994[] = { 0x60, 0x69, 0x75, 0x78, 0x85 };
static u8 D_8438599C[] = { 0x61, 0x64, 0x66, 0x68, 0x8C };
static u8 D_843859A4[] = {
    1,    0xA,  0xB,  0x11, 0x13, 0x14, 0x15, 0x1E, 0x22, 0x23, 0x25, 0x27, 0x2B, 0x2C, 0x2D, 0x2E,
    0x2F, 0x40, 0x44, 0x46, 0x47, 0x51, 0x59, 0x5A, 0x5B, 0x5C, 0x67, 0x6A, 0x6B, 0x6E, 0x6F, 0x76,
    0x7A, 0x80, 0x84, 0x8B, 0x8D, 0x91, 0x94, 0x96, 0x97, 0x9A, 0x9C, 0x9F, 0xA3, 0xA4,
};

void Battle_ClearAllMessageQueues(void) {
    gBattleMessageQueues->unk_048[1][0] = 0;
    gBattleMessageQueues->unk_048[1][1] = 0;
    gBattleMessageQueues->unk_048[2][0] = 0;
    gBattleMessageQueues->unk_048[2][1] = 0;
    gBattleMessageQueues->unk_048[3][0] = 0;
    gBattleMessageQueues->unk_048[3][1] = 0;
    gBattleMessageQueues->unk_048[4][0] = 0;
    gBattleMessageQueues->unk_048[4][1] = 0;
    gBattleMessageQueues->unk_048[5][0] = 0;
    gBattleMessageQueues->unk_048[5][1] = 0;
    gBattleMessageQueues->unk_048[6][0] = 0;
    gBattleMessageQueues->unk_048[6][1] = 0;
    gBattleMessageQueues->unk_048[16][0] = 0;
    gBattleMessageQueues->unk_048[16][1] = 0;
    gBattleMessageQueues->unk_048[17][0] = 0;
    gBattleMessageQueues->unk_048[17][1] = 0;
    gBattleMessageQueues->unk_048[7][0] = 0;
    gBattleMessageQueues->unk_048[7][1] = 0;
    gBattleMessageQueues->unk_048[11][0] = 0;
    gBattleMessageQueues->unk_048[11][1] = 0;
    gBattleMessageQueues->unk_048[10][0] = 0;
    gBattleMessageQueues->unk_048[10][1] = 0;
    gBattleMessageQueues->unk_048[12][0] = 0;
    gBattleMessageQueues->unk_048[12][1] = 0;
    gBattleMessageQueues->unk_048[13][0] = 0;
    gBattleMessageQueues->unk_048[13][1] = 0;
    gBattleMessageQueues->unk_048[14][0] = 0;
    gBattleMessageQueues->unk_048[14][1] = 0;
    gBattleMessageQueues->unk_048[15][0] = 0;
    gBattleMessageQueues->unk_048[15][1] = 0;
}

void Battle_ClearMessageCursor(void) {
    gBattleMessageQueues->unk_004 = 0;
    gBattleMessageQueues->unk_048[0][0] = 0;
    gBattleMessageQueues->unk_048[0][1] = 0;
    gBattleMessageQueues->unk_008[0] = 0;
    gBattleMessageQueues->unk_008[1] = 0;
}

void Battle_SimpleStringFormat(s8* arg0, s8* arg1, ...) {
    va_list args;
    s8* src;
    s8* fmt;
    s32 len;

    len = 0;
    va_start(args, arg1);
    fmt = arg1;

    while (*fmt != 0) {
        if (*fmt != 0x25) {
            arg0[len++] = *fmt;
        } else {
            fmt++;
            if (*fmt == 0x73) {
                src = va_arg(args, s8*);
                while (*src != 0) {
                    arg0[len] = *src;
                    src++;
                    len++;
                }
            }
        }
        fmt++;
    }
    arg0[len] = 0;
}

void Battle_QueueMessage(s8* arg0, s8 arg1) {
    u8 tmp = arg1;
    *arg0 = arg1;

    if (tmp >= 0x80) {
        if ((tmp - 0x80) >= 0xB) {
            Text_GetString(arg0 + 1, 0x40, sBattleMessagesTable, tmp - 0x48);
        } else {
            Text_GetString(arg0 + 1, 0x40, sBattleMessagesTable, tmp - 0x49);
        }
    } else {
        Text_GetString(arg0 + 1, 0x40, sBattleMessagesTable, D_84385BC0[arg1]);
    }
}

void Battle_QueueStatusText(char* arg0, s32 arg1) {
    UNUSED s32 pad[3];
    ActivePokemonState* sp28 = &sActivePokemon[arg1]->state;
    UNUSED s32 pad2[2];

    Battle_ClearMessageCursor();
    gBattleMessageQueues->unk_000 = sActivePokemon[arg1]->sessionTeams->teamMember[sp28->teamMemberIndex]->controllerPort;
    Battle_SimpleStringFormat(gBattleMessageQueues->unk_048[0], arg0 + 1);
    *arg0 = 0;
}

void func_84317B38(void) {
}

void Battle_DrawCompactStatusBoxesIfVisible(void) {
    ActivePokemon* ptr1 = sActivePokemon[0];
    ActivePokemon* ptr2 = sActivePokemon[1];

    if ((gSessionContext.mode != 0x10) && (sBattleScene.scene->unk_30 == 0)) {
        BattleHud_DrawCompactMonStatusBox(ptr1, 0x15, 0xF, ptr1->sessionTeams->teamMember[ptr1->thumbnail.tagTeam]->controllerPort);
        BattleHud_DrawCompactMonStatusBox(ptr2, 0xE1, 0xF, ptr2->sessionTeams->teamMember[ptr2->thumbnail.tagTeam]->controllerPort);
    }
}

void Battle_QueueMoveCategoryUsedMessage(void) {
    u32 sp1C;
    BattleMonRuntime* sp18 = &sActivePokemon[sBattleScene.scene->unk_2C]->state.runtime;

    sp18->unk_58 = sp18->moveCache.lastMove;

    if (BattleScene_ByteArrayContains(sp18->moveCache.lastMove, D_84385990, 2) != 0) {
        sp1C = 0;
    } else if (BattleScene_ByteArrayContains(sp18->moveCache.lastMove, D_84385994, 5) != 0) {
        sp1C = 1;
    } else if (BattleScene_ByteArrayContains(sp18->moveCache.lastMove, D_8438599C, 5) != 0) {
        sp1C = 2;
    } else if (BattleScene_ByteArrayContains(sp18->moveCache.lastMove, D_843859A4, 0x2E) != 0) {
        sp1C = 3;
    } else {
        sp1C = 4;
    }

    if ((sp18->moveCache.lastMove == 0x76) || (sp18->moveCache.lastMove == 0x77)) {
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->unk_2C]));
        Text_SetStringToken(0x1D, Battle_GetActiveMoveName(sActivePokemon[sBattleScene.scene->unk_2C]));

        switch (sp1C) {
            case 0:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[13], 0x14);
                break;

            case 1:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[13], 0x15);
                break;

            case 2:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[13], 0x16);
                break;

            case 3:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[13], 0x17);
                break;

            case 4:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[13], 0x18);
                break;
        }
    } else {
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->unk_2C]));
        Text_SetStringToken(0x1D, Battle_GetActiveMoveName(sActivePokemon[sBattleScene.scene->unk_2C]));

        switch (sp1C) {
            case 0:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[2], 0x14);
                break;

            case 1:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[2], 0x15);
                break;

            case 2:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[2], 0x16);
                break;

            case 3:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[2], 0x17);
                break;

            case 4:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[2], 0x18);
                break;
        }
    }
}

void Battle_QueueMoveCategoryMessage(void) {
    u32 sp24;
    UNUSED s32 pad;
    BattleMonRuntime* sp1C = &sActivePokemon[sBattleScene.scene->unk_2C]->state.runtime;

    if (BattleScene_ByteArrayContains(sp1C->usedMove, D_84385990, 2) != 0) {
        sp24 = 0;
    } else if (BattleScene_ByteArrayContains(sp1C->usedMove, D_84385994, 5) != 0) {
        sp24 = 1;
    } else if (BattleScene_ByteArrayContains(sp1C->usedMove, D_8438599C, 5) != 0) {
        sp24 = 2;
    } else if (BattleScene_ByteArrayContains(sp1C->usedMove, D_843859A4, 0x2E) != 0) {
        sp24 = 3;
    } else {
        sp24 = 4;
    }

    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->unk_2C]));
    Text_SetStringToken(0x1D, Battle_GetActiveMoveName(sActivePokemon[sBattleScene.scene->unk_2C]));

    switch (sp24) {
        case 0:
            Battle_QueueMessage(gBattleMessageQueues->unk_048[2], 0x14);
            break;

        case 1:
            Battle_QueueMessage(gBattleMessageQueues->unk_048[2], 0x15);
            break;

        case 2:
            Battle_QueueMessage(gBattleMessageQueues->unk_048[2], 0x16);
            break;

        case 3:
            Battle_QueueMessage(gBattleMessageQueues->unk_048[2], 0x17);
            break;

        case 4:
            Battle_QueueMessage(gBattleMessageQueues->unk_048[2], 0x18);
            break;
    }
}

void Battle_QueueMetronomeMoveMessage(void) {
    s32 sp2C;
    s32 tmp;
    BattleMonRuntime* sp24;

    sp24 = &sActivePokemon[sBattleScene.scene->unk_2C]->state.runtime;

    if (BattleScene_ByteArrayContains(sp24->usedMove, D_84385990, 2) != 0) {
        sp2C = 0;
    } else if (BattleScene_ByteArrayContains(sp24->usedMove, D_84385994, 5) != 0) {
        sp2C = 1;
    } else if (BattleScene_ByteArrayContains(sp24->usedMove, D_8438599C, 5) != 0) {
        sp2C = 2;
    } else if (BattleScene_ByteArrayContains(sp24->usedMove, D_843859A4, 0x2E) != 0) {
        sp2C = 3;
    } else {
        sp2C = 4;
    }

    if (sp24->usedMove == METRONOME) {
        do {
            tmp = Battle_Random();
        } while ((tmp == 0) || (tmp >= 0xA5) || tmp == 0x76);

        sp24->usedMove = tmp;

        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->unk_2C]));
        Text_SetStringToken(0x1D, Battle_GetActiveMoveName(sActivePokemon[sBattleScene.scene->unk_2C]));

        switch (sp2C) {
            case 0:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[2], 0x14);
                break;

            case 1:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[2], 0x15);
                break;

            case 2:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[2], 0x16);
                break;

            case 3:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[2], 0x17);
                break;

            case 4:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[2], 0x18);
                break;
        }

        sp24->usedMove = METRONOME;

        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->unk_2C]));
        Text_SetStringToken(0x1D, Battle_GetActiveMoveName(sActivePokemon[sBattleScene.scene->unk_2C]));

        switch (sp2C) {
            case 0:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[13], 0x14);
                break;

            case 1:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[13], 0x15);
                break;

            case 2:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[13], 0x16);
                break;

            case 3:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[13], 0x17);
                break;

            case 4:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[13], 0x18);
                break;
        }
    } else {
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(sActivePokemon[sBattleScene.scene->unk_2C]));
        Text_SetStringToken(0x1D, Battle_GetActiveMoveName(sActivePokemon[sBattleScene.scene->unk_2C]));

        switch (sp2C) {
            case 0:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[2], 0x14);
                break;

            case 1:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[2], 0x15);
                break;

            case 2:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[2], 0x16);
                break;

            case 3:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[2], 0x17);
                break;

            case 4:
                Battle_QueueMessage(gBattleMessageQueues->unk_048[2], 0x18);
                break;
        }
    }
}

void BattleBox_Open(s32 arg0) {
    BattleBox* ptr = &gBattleMessageQueues->unk_4C8;

    ptr->unk_00 = 0;
    ptr->unk_04 = arg0;
}

void BattleBox_Close(void) {
    BattleBox* battle_box = &gBattleMessageQueues->unk_4C8;

    battle_box->unk_00 = 0xFF;
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

void BattleBox_DrawMessage(BattleBox* arg0, s16 arg1, s16 arg2) {
    UNUSED s32 pad;
    s32 temp_s1;

    if (1) {}

    switch (arg0->unk_04) {
        case 0:
            if (Battle_DrawTextIfNonEmpty(&gBattleMessageQueues->unk_048[13][1], arg1 + 4, arg2 + (arg0->lineCount * 0xC) + 3) != 0) {
                arg0->lineCount += 1;
            }

            if (Battle_DrawTextIfNonEmpty(&gBattleMessageQueues->unk_048[2][1], arg1 + 4, arg2 + (arg0->lineCount * 0xC) + 3) != 0) {
                arg0->lineCount += 1;
            }

            if (Battle_DrawTextIfNonEmpty(&gBattleMessageQueues->unk_048[5][1], arg1 + 4, arg2 + (arg0->lineCount * 0xC) + 3) != 0) {
                arg0->lineCount += 1;
            }
            break;

        case 3:
            if (Battle_DrawTextIfNonEmpty(&gBattleMessageQueues->unk_048[1][1], arg1 + 4, arg2 + (arg0->lineCount * 0xC) + 3) != 0) {
                arg0->lineCount += 1;
            }

            if (Battle_DrawTextIfNonEmpty(&gBattleMessageQueues->unk_048[4][1], arg1 + 4, arg2 + (arg0->lineCount * 0xC) + 3) != 0) {
                arg0->lineCount += 1;
            }

            if (Battle_DrawTextIfNonEmpty(&gBattleMessageQueues->unk_048[7][1], arg1 + 4, arg2 + (arg0->lineCount * 0xC) + 3) != 0) {
                arg0->lineCount += 1;
            }
            break;

        case 1:
            if (Battle_DrawTextIfNonEmpty(&gBattleMessageQueues->unk_048[14][1], arg1 + 4, arg2 + (arg0->lineCount * 0xC) + 3) != 0) {
                arg0->lineCount += 1;
            }

            if (Battle_DrawTextIfNonEmpty(&gBattleMessageQueues->unk_048[1][1], arg1 + 4, arg2 + (arg0->lineCount * 0xC) + 3) != 0) {
                arg0->lineCount += 1;
            }

            if (Battle_DrawTextIfNonEmpty(&gBattleMessageQueues->unk_048[4][1], arg1 + 4, arg2 + (arg0->lineCount * 0xC) + 3) != 0) {
                arg0->lineCount += 1;
            }

            if ((arg0->lineCount < 3) &&
                (Battle_DrawTextIfNonEmpty(&gBattleMessageQueues->unk_048[7][1], arg1 + 4, arg2 + (arg0->lineCount * 0xC) + 3) != 0)) {
                arg0->lineCount += 1;
            }
            break;

        case 2:
            if (Battle_DrawTextIfNonEmpty(&gBattleMessageQueues->unk_048[1][1], arg1 + 4, arg2 + (arg0->lineCount * 0xC) + 3) != 0) {
                arg0->lineCount += 1;
            }

            if (Battle_DrawTextIfNonEmpty(&gBattleMessageQueues->unk_048[4][1], arg1 + 4, arg2 + (arg0->lineCount * 0xC) + 3) != 0) {
                arg0->lineCount += 1;
            }

            if (Battle_DrawTextIfNonEmpty(&gBattleMessageQueues->unk_048[7][1], arg1 + 4, arg2 + (arg0->lineCount * 0xC) + 3) != 0) {
                arg0->lineCount += 1;
            }
            break;

        case 4:
            Battle_DrawTextIfNonEmpty(&gBattleMessageQueues->unk_048[8][1], arg1 + 4, arg2 + 3);
            break;

        case 5:
            Battle_DrawTextIfNonEmpty(&gBattleMessageQueues->unk_048[9][1], arg1 + 4, arg2 + 3);
            break;

        case 6:
            Battle_DrawTextIfNonEmpty(&gBattleMessageQueues->unk_048[15][1], arg1 + 4, arg2 + 3);
            break;

        case 7:
            Battle_DrawTextIfNonEmpty(&gBattleMessageQueues->unk_048[12][1], arg1 + 4, arg2 + 3);
            Battle_DrawTextIfNonEmpty(&gBattleMessageQueues->unk_048[10][1], arg1 + 4, arg2 + 0x10);
            break;

        case 8:
            Battle_DrawTextIfNonEmpty(&gBattleMessageQueues->unk_048[16 + arg0->side][1], arg1 + 4, arg2 + 3);
            break;

        case 9:
        case 10:
            Battle_DrawTextIfNonEmpty(&gBattleMessageQueues->unk_048[6][1], arg1 + 4, arg2 + 3);
            break;

        case 11:
            Battle_DrawTextIfNonEmpty(&gBattleMessageQueues->unk_048[1][1], arg1 + 4, arg2 + 3);
            break;

        case 12:
            Battle_DrawTextIfNonEmpty(&gBattleMessageQueues->unk_048[1][1], arg1 + 4, arg2 + 3);
            break;

        case 13:
            temp_s1 = Font_MeasureTextExtent(1, 0, Text_GetString(NULL, 0, sBattleMessagesTable, 0xA4)) / 2;
            Battle_DrawTextIfNonEmpty(Text_GetString(NULL, 0, sBattleMessagesTable, 0xA4), 0xA1 - temp_s1, 0x71);
            break;

        case 14:
            Battle_DrawEndBattleBanner();
            break;

        case 15:
            Battle_DrawTextIfNonEmpty(&gBattleMessageQueues->unk_048[1][1], arg1 + 4, arg2 + (arg0->lineCount * 0xC) + 3);
            break;
    }

    arg0->lineCount = 0;
}

s32 BattleBox_SelectMessage(BattleBox* arg0) {
    unk_D_843901A0* temp_v0;
    s32 tmp;

    arg0->lineCount = 0;

    switch (arg0->unk_04) {
        case 0:
            if ((gBattleMessageQueues->unk_048[2][1] == 0) && (gBattleMessageQueues->unk_048[13][1] == 0) && (gBattleMessageQueues->unk_048[5][1] == 0)) {
                arg0->unk_00 = 0xFF;
                return 1;
            }

            arg0->side = sBattleScene.scene->unk_2C;
            if (gBattleMessageQueues->unk_048[2][1] != 0) {
                BattleBox_PlayTextBlip(gBattleMessageQueues->unk_048[2]);
            }

            if (gBattleMessageQueues->unk_048[13][1] != 0) {
                BattleBox_PlayTextBlip(gBattleMessageQueues->unk_048[13]);
            }

            if (gBattleMessageQueues->unk_048[5][1] != 0) {
                BattleBox_PlayTextBlip(gBattleMessageQueues->unk_048[5]);
            }
            arg0->unk_00 += 1;
            break;

        case 1:
        case 3:
            arg0->side = sBattleScene.scene->unk_2C;
            BattleBox_PlayTextBlip(gBattleMessageQueues->unk_048[1]);
            BattleBox_PlayTextBlip(gBattleMessageQueues->unk_048[4]);
            BattleBox_PlayTextBlip(gBattleMessageQueues->unk_048[7]);
            BattleBox_PlayTextBlip(gBattleMessageQueues->unk_048[14]);
            BattleBox_PlayTextBlip(gBattleMessageQueues->unk_048[11]);
            if ((gBattleMessageQueues->unk_048[1][1] == 0) && (gBattleMessageQueues->unk_048[4][1] == 0) && (gBattleMessageQueues->unk_048[7][1] == 0) &&
                (gBattleMessageQueues->unk_048[14][1] == 0) && (gBattleMessageQueues->unk_048[11][1] == 0)) {
                arg0->unk_00 = 0xFF;
                return 1;
            }
            arg0->unk_00 += 1;
            break;

        case 2:
            arg0->side = sBattleScene.scene->unk_2C;
            BattleBox_PlayTextBlip(gBattleMessageQueues->unk_048[1]);
            BattleBox_PlayTextBlip(gBattleMessageQueues->unk_048[4]);
            BattleBox_PlayTextBlip(gBattleMessageQueues->unk_048[7]);
            if ((gBattleMessageQueues->unk_048[1][1] == 0) && (gBattleMessageQueues->unk_048[4][1] == 0) && (gBattleMessageQueues->unk_048[7][1] == 0)) {
                arg0->unk_00 = 0xFF;
                return 1;
            }
            arg0->unk_00 += 1;
            break;

        case 15:
            arg0->side = sBattleScene.scene->unk_2C;
            BattleBox_PlayTextBlip(gBattleMessageQueues->unk_048[1]);
            if (gBattleMessageQueues->unk_048[1][1] == 0) {
                arg0->unk_00 = 0xFF;
                return 1;
            }
            arg0->unk_00 += 1;
            break;

        case 8:
            if (gBattleMessageQueues->unk_048[16 + arg0->side][1] == 0) {
                arg0->unk_00 = 0xFF;
                return 1;
            }
            tmp = sBattleScene.scene->unk_2C == 0;
            arg0->unk_00 += 1;
            arg0->side = tmp;
            break;

        case 9:
            tmp = sBattleScene.scene->unk_2C == 0;
            arg0->unk_00 += 1;
            arg0->side = tmp;
            break;

        case 4:
        case 5:
        case 6:
            BattleBox_PlayTextBlip(gBattleMessageQueues->unk_048[8]);
            BattleBox_PlayTextBlip(gBattleMessageQueues->unk_048[9]);
            BattleBox_PlayTextBlip(gBattleMessageQueues->unk_048[15]);
            arg0->unk_00 += 1;
            break;

        case 14:
            sEndBattleBannerPos.z = 0.0f;
            sEndBattleBannerPos.x = 321.0f;
            sEndBattleBannerPos.y = 160.0f;
            arg0->side = sBattleScene.scene->unk_2C;
            arg0->unk_00 += 1;
            break;

        default:
            arg0->side = sBattleScene.scene->unk_2C;
            arg0->unk_00 += 1;
            break;
    }
    return 0;
}

void BattleBox_Update(s16 arg0, s16 arg1) {
    BattleBox* temp_s0 = &gBattleMessageQueues->unk_4C8;

    if (gSessionContext.mode == 0x10) {
        return;
    }

    switch (temp_s0->unk_00) {
        case 0:
            temp_s0->displayTimer = 0;
            if (temp_s0->unk_04 == 0xD) {
                BattleHud_InitCaptionBoxPopupBlack(temp_s0, 0xA0, 0x78);
                temp_s0->unk_00++;
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
                    temp_s0->unk_00++;
                }
            } else {
                if (temp_s0->unk_04 == 0xE) {
                    ProcessAudioCommandQueue(BattleScene_GetParticipantDisplayPlayerNumber(sActivePokemon[temp_s0->side]));
                    BattleBox_DrawMessage(temp_s0, temp_s0->textOrigin.x, temp_s0->textOrigin.y);
                    temp_s0->unk_00++;
                } else if (BattleHud_UpdateCaptionBoxPopup(temp_s0, arg0, arg1) != 0) {
                    ProcessAudioCommandQueue(BattleScene_GetParticipantDisplayPlayerNumber(sActivePokemon[temp_s0->side]));
                    BattleBox_DrawMessage(temp_s0, temp_s0->textOrigin.x, temp_s0->textOrigin.y);
                    temp_s0->unk_00++;
                }
            }
            break;

        case 2:
            if (temp_s0->unk_04 == 0xD) {
                BattleHud_UpdateCaptionBoxPopupBlack(temp_s0, 0xA0, 0x78);
                BattleBox_DrawMessage(temp_s0, temp_s0->textOrigin.x, temp_s0->textOrigin.y);

                temp_s0->displayTimer++;
                if (temp_s0->displayTimer == 0x46) {
                    temp_s0->unk_00++;
                }
            } else {
                if (temp_s0->unk_04 == 0xE) {
                    BattleBox_DrawMessage(temp_s0, temp_s0->textOrigin.x, temp_s0->textOrigin.y);
                } else {
                    BattleHud_UpdateCaptionBoxPopup(temp_s0, arg0, arg1);
                    BattleBox_DrawMessage(temp_s0, temp_s0->textOrigin.x, temp_s0->textOrigin.y);

                    temp_s0->displayTimer++;
                    if (temp_s0->displayTimer == 0x2D) {
                        temp_s0->unk_00++;
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
    gBattleMessageQueues->unk_000 = arg0->sessionTeams->teamMember[arg0->state.ownerTeamMemberIndex]->controllerPort;
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
    gBattleMessageQueues = main_pool_alloc(sizeof(unk_D_843901A0), 0);
    bzero(gBattleMessageQueues, sizeof(unk_D_843901A0));
}

void BattleScene_ResetMessageQueue(void) {
    gBattleMessageQueues->unk_004 = 0;
    gBattleMessageQueues->unk_008[gBattleMessageQueues->unk_004] = 0;
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
