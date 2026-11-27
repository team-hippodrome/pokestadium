#include "lab_pc_list.h"
#include "src/gb_data.h"
#include "src/save_data.h"
#include "src/game_state.h"
#include "src/table_view.h"
#include "src/gb_save.h"
#include "src/hal_libc.h"
#include "src/memory.h"

typedef struct unk_func_883164C0 {
#if defined(VERSION_JP)
    /* 0x00 */ char unk00[0x58];
#else
    /* 0x00 */ char unk00[0x5C];
#endif
} unk_func_883164C0; // size = 0x5C (JP: 0x58)

void LabPCList_AllocateBoxSlots(unk_func_8830867C_02C_0E4* arg0, s32 arg1, s32 arg2, MemoryPool* arg3) {
    s32 i;
    s32 j;
    s32 temp_lo;
    unk_func_8820BE14_06C_000_alt* var_s1;
    unk_func_883164C0** var_s2;
    unk_func_883164C0* var_s3;

    temp_lo = arg0->unk_0C.unk_04 * arg1;

    var_s3 = mem_pool_alloc(arg3, temp_lo * sizeof(unk_func_883164C0));
    var_s2 = mem_pool_alloc(arg3, temp_lo * sizeof(unk_func_883164C0*));

    var_s1 = arg0->unk_0C.unk_00;
    bzero(var_s3, temp_lo * sizeof(unk_func_883164C0));

    arg0->unk_0C.unk_08 = arg0->unk_0C.unk_04;

    for (i = 0; i < arg0->unk_0C.unk_08; i++) {
        var_s1[i].unk_00 = arg0;
        var_s1[i].unk_04.unk_00 = i;
        var_s1[i].unk_04.unk_04 = arg2;
        var_s1[i].unk_04.unk_08 = 0;

        // clang-format off
        for (j = 0; j < arg1; j++) { var_s2[j] = &var_s3[j]; }
        // clang-format on

        PointerList_Initialize(&var_s1[i].unk_10, var_s2, arg1, 0);

        var_s2 += arg1;
        var_s3 += arg1;
    }
}

void LabPCList_LoadAllBoxes(unk_func_8830867C_02C_144* arg0) {
    s32 i;
    s32 j;
    s32 var_s1;
    DeckHandle* temp_v0;
    unk_func_8830867C_02C_0CC_000_000* var_s3;
    unk_func_8830867C_02C_0CC_000_000_010_000* var_s0;
    unk_func_8830867C_02C_144_000* var_s5;

    var_s5 = arg0->unk_00;
    if (GbSave_GetBoxCount(var_s5[1].unk_04) == 0) {
        GbSave_MarkBoxDataLoaded(var_s5[1].unk_04);
    }

    for (i = 0; i < arg0->unk_08; i++, var_s5++) {
        var_s3 = var_s5->unk_0C;

        for (j = 0; j < var_s5->unk_14; j++, var_s3++) {
            temp_v0 = Deck_Open(var_s5->unk_00, var_s5->unk_04, j, 0);
            var_s1 = 0;

            if (temp_v0 != NULL) {
                var_s0 = var_s3->unk_10.unk_00;

                while (true) {
                    if (Deck_ReadEntries(&var_s0->unk_00->unk_08, 1, temp_v0) == 0) {
                        break;
                    }

                    var_s0->unk_00->unk_00 = var_s3;
                    var_s0->unk_00->unk_04 = var_s1;

                    var_s0++;
                    var_s1++;
                }

                Deck_CloseAndFlush(temp_v0);
            }

            var_s3->unk_10.unk_08 = var_s1;
        }
    }
}

void LabPCList_FlattenEntries(unk_func_88200FA0_030_038* arg0, unk_func_8830867C_02C_144* arg1) {
    s32 i;
    s32 j;
    s32 x;
    unk_func_88205880_00D0** var_v0;
    unk_func_8830867C_02C_0CC_000_000* var_v1;
    unk_func_8830867C_02C_0CC_000_000_010_000* var_t0;
    unk_func_8830867C_02C_144_000* var_a2;

    var_v0 = arg0->unk_00;
    var_a2 = arg1->unk_00;

    for (i = 0; i < arg1->unk_08; i++, var_a2++) {
        var_v1 = var_a2->unk_0C;

        for (j = 0; j < var_a2->unk_14; j++, var_v1++) {
            if (var_v1->unk_08 != 0) {
                var_t0 = var_v1->unk_10.unk_00;

                for (x = 0; x < var_v1->unk_10.unk_08; x++, var_t0++, var_v0++) {
                    *var_v0 = var_t0->unk_00;
                }
            }
        }
    }

    arg0->unk_08 = var_v0 - arg0->unk_00;
}

s32 LabPCList_GetCategoryDirtyFlag(s32 arg0) {
    s32 ret = 0;

    switch (arg0) {
        case 32:
            ret = 1;
            break;

        case 33:
            ret = 4;
            break;

        case 17:
            ret = 0x10;
            break;
    }

    return ret;
}

s32 LabPCList_CheckDirtyBoxes(unk_func_8830867C_02C_144* arg0) {
    s32 var_s7 = 0;
    s32 j;
    s32 x;
    s32 i;
    DeckHandle* temp_v0;
    unk_func_8830867C_02C_0CC_000_000* var_s3;
    unk_func_8830867C_02C_0CC_000_000_010_000* var_s0;
    unk_func_8830867C_02C_144_000* var_s5 = arg0->unk_00;
    BattleMon sp4C;

    for (i = 0; i < arg0->unk_08; i++, var_s5++) {
        var_s3 = var_s5->unk_0C;

        for (j = 0; j < var_s5->unk_14; j++, var_s3++) {
            if (var_s3->unk_0C != 0) {
                temp_v0 = Deck_Open(var_s5->unk_00, var_s5->unk_04, j, 0);
                if (temp_v0 == NULL) {
                    continue;
                }

                var_s3->unk_0C = 0;
                if (Deck_GetEntryCountForType(var_s5->unk_00, var_s5->unk_04, j) != var_s3->unk_10.unk_08) {
                    var_s3->unk_0C = 1;
                    var_s7 |= LabPCList_GetCategoryDirtyFlag(var_s5->unk_00);
                } else {
                    var_s0 = var_s3->unk_10.unk_00;

                    for (x = 0; x < var_s3->unk_10.unk_08; x++, var_s0++) {
                        bzero(&sp4C, sizeof(BattleMon));
                        Deck_ReadEntries(&sp4C, 1, temp_v0);
                        if (bcmp(&sp4C, &var_s0->unk_00->unk_08, sizeof(BattleMon)) != 0) {
                            var_s3->unk_0C = 1;
                            var_s7 |= LabPCList_GetCategoryDirtyFlag(var_s5->unk_00);
                            break;
                        }
                    }
                }
                Deck_CloseAndFlush(temp_v0);
            }
        }
    }

    return var_s7;
}

void LabPCList_SaveDirtyBoxes(unk_func_8830867C_02C_144* arg0, s32 arg1) {
    UNUSED s32 pad[4];
    s32 i;
    s32 j;
    s32 x;
    s32 var_s7;
    DeckHandle* temp_s3;
    unk_func_8830867C_02C_0CC_000_000* var_s4;
    unk_func_8830867C_02C_0CC_000_000_010_000* var_s0;
    unk_func_8830867C_02C_144_000* var_s2;
    s32 tmp;

    var_s2 = arg0->unk_00;

    for (i = 0; i < arg0->unk_08; i++, var_s2++) {
        var_s4 = var_s2->unk_0C;
        var_s7 = 0;

        for (j = 0; j < var_s2->unk_14; j++, var_s4++) {
            if (var_s4->unk_0C != 0) {
                var_s0 = var_s4->unk_10.unk_00;
                temp_s3 = Deck_Open(var_s2->unk_00, var_s2->unk_04, j, 1);

                for (x = 0; x < var_s4->unk_10.unk_08; x++, var_s0++) {
                    Deck_WriteEntries(&var_s0->unk_00->unk_08, 1, temp_s3);
                    if ((var_s2->unk_00 == 0x20) || (var_s2->unk_00 == 0x21)) {
                        if ((var_s0->unk_00->unk_08.species.id > 0) && (var_s0->unk_00->unk_08.species.id < 0x98)) {
                            GbSave_SetSeenOwnedBits(var_s2->unk_04, var_s0->unk_00->unk_08.species.id, 3);
                        }
                    }
                }

                tmp = var_s2->unk_00;
                if ((tmp == 0x10) || (tmp == 0x11) || (tmp == 0x12)) {
                    Deck_CloseAndFlushBox(temp_s3);
                } else {
                    Deck_CloseAndFlush(temp_s3);
                }

                if (1) {}
            }
        }
    }
}

void LabPCList_InitDeckCategories(unk_func_8830867C_02C_154* arg0, MemoryPool* arg1) {
    UNUSED s32 pad;
    s32 temp_s1;
    char sp54[4];
    unk_func_8002D860 sp48;
    char sp38[16];
    s32 i;

    arg0->unk_00 = 0x23;
    arg0->unk_01 = GbSave_GetActivePort();
    arg0->unk_02 = 0;
    arg0->unk_04 = 1;
    ByteMatrix_Allocate(&arg0[0].unk_0C, 2, 0x14, arg1);

    arg0[1].unk_00 = 0x22;
    arg0[1].unk_01 = arg0->unk_01;
    arg0[1].unk_02 = 0;
    arg0[1].unk_04 = 1;
    ByteMatrix_Allocate(&arg0[1].unk_0C, 2, 0x32, arg1);

    arg0[2].unk_00 = 0x12;
    arg0[2].unk_01 = 0;
    ByteMatrix_Allocate(&arg0[2].unk_0C, 2, 0x64, arg1);

    GbSave_BuildPortSummary(&sp48, arg0->unk_01);

    for (i = 0; i < 4; i++) {
        if ((Deck_IsSaveEntryPresent(0x12, i) != 0) && (Deck_GetSaveEntryCount(0x12, i) > 0)) {
            temp_s1 = Deck_GetSaveEntryTrainerId(0x12, i);
            Deck_GetSaveEntryName(0x12, i, &sp38);
            if ((sp48.unk_08 == temp_s1) && (HAL_Strcmp(sp54, sp38) == 0)) {
                break;
            }
        }
    }

    if (i < 4) {
        arg0[2].unk_02 = i;
        arg0[2].unk_04 = 1;
    } else {
        arg0[2].unk_04 = 0;
    }
}

void LabPCList_ReadDeckCategory(unk_func_8830867C_02C_154* arg0) {
    DeckHandle* sp24;

    sp24 = Deck_Open(arg0->unk_00, arg0->unk_01, arg0->unk_02, 0);
    arg0->unk_14 = Deck_ReadEntries(arg0->unk_0C, arg0->unk_10, sp24);
    Deck_CloseAndFlush(sp24);
    arg0->unk_08 = 0;
}

void LabPCList_WriteDeckCategory(unk_func_8830867C_02C_154* arg0) {
    DeckHandle* sp24 = Deck_Open(arg0->unk_00, arg0->unk_01, arg0->unk_02, 1);

    Deck_WriteEntries(arg0->unk_0C, arg0->unk_14, sp24);
    Deck_CloseAndFlush(sp24);
    arg0->unk_08 = 0;
}

s32 LabPCList_IsTmItemId(s32* arg0) {
    if ((*arg0 >= 0xC9) && (*arg0 < 0xFB)) {
        *arg0 -= 0x65;
        return 1;
    }
    if ((*arg0 >= 0xC4) && (*arg0 < 0xC9)) {
        *arg0 += 4;
        return 1;
    }
    return 0;
}

s32 LabPCList_IsHmItemId(s32* arg0) {
    static u8 D_8831A250[] = { 0x23, 0x24, 0x25, 0x26, 0x27, 0x28 };

    s32 i;

    for (i = 0; i < 6; i++) {
        if (*arg0 == D_8831A250[i]) {
            *arg0 = i;
            return 1;
        }
    }

    return 0;
}

s32 LabPCList_IsSpecialTmItemId(s32* arg0) {
    static u8 D_8831A258[] = { 0x4F };

    s32 i;

    for (i = 0; i < 1; i++) {
        if (*arg0 == D_8831A258[i]) {
            *arg0 = i + 0x32;
            return 1;
        }
    }

    return 0;
}

s32 LabPCList_CompareTmHmEntries(unk_func_88200FA0_030_038* arg0, unk_func_88200FA0_030_038* arg1) {
    return (arg0->unk_08 - arg1->unk_08 != 0) ? arg0->unk_08 - arg1->unk_08 : (s32)arg0[1].unk_00 - (s32)arg1[1].unk_00;
}

void LabPCList_BuildTmHmList(unk_func_88200FA0_030_038* arg0, unk_func_8830867C_02C_1A8* arg1,
                             unk_func_8830867C_02C_154* arg2, s32 arg3, s32 arg4) {
    unk_func_88205880_00D0** var_s6;
    unk_func_8830867C_02C_154* var_s4;
    s32 i;
    unk_func_8830867C_02C_1A8* var_s1;
    s32 j;
    s32 sp50;
    s32 var_s0;
    s32 var_v0;
    u8* var_s2;

    var_s6 = arg0->unk_00;
    var_s4 = arg2;
    var_s1 = arg1;
    PointerList_Initialize(arg0, var_s6, arg4, 0);

    for (i = 0; i < arg3; i++, var_s4++) {
        if (var_s4->unk_04 != 0) {
            var_s2 = var_s4->unk_0C;

            for (j = 0; j < var_s4->unk_14; j++, var_s2 += 2) {
                var_s0 = 0;
                sp50 = *var_s2;
                if (LabPCList_IsTmItemId(&sp50) != 0) {
                    var_s0 = 1;
                } else if (LabPCList_IsHmItemId(&sp50) != 0) {
                    var_s0 = 1;
                } else if (LabPCList_IsSpecialTmItemId(&sp50) != 0) {
                    var_s0 = 1;
                }

                if (var_s0 != 0) {
                    var_s1->unk_00 = var_s2;
                    var_s1->unk_04 = var_s4;
                    var_s1->unk_08 = sp50;

                    if (var_s4->unk_00 == 0x23) {
                        var_s1->unk_0C = 0;
                    } else {
                        if (var_s4->unk_00 == 0x22) {
                            var_v0 = 1;
                        } else {
                            var_v0 = 2;
                        }
                        var_s1->unk_0C = var_v0;
                    }
                    *var_s6 = var_s1;

                    var_s1++;
                    var_s6++;
                }
            }
        }
    }

    arg0->unk_08 = ((s32)var_s1 - (s32)arg1) >> 4;
    PointerList_Sort(arg0, LabPCList_CompareTmHmEntries);
}

void LabPCList_ConsumeUsedTmItem(unk_func_8830867C_04C_030_02C_000* arg0) {
    if ((LabItem_IsQuantitylessItemForOperation(arg0->unk_00[0]->unk_00) != 0) || (arg0->unk_00[0]->unk_01 < 2)) {
        s32 tmp = arg0->unk_00[1]->unk_0C.unk_00;

        WordList_Remove(&arg0->unk_00[1]->unk_0C, ((s32)arg0->unk_00[0] - tmp) >> 1);
    } else {
        arg0->unk_00[0]->unk_01--;
    }
    arg0->unk_00[1]->unk_08 = 1;
}
