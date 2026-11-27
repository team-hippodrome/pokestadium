#include "lab_widget_tree.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/gb_data.h"
#include "src/table_view.h"
#include "src/text_system.h"
#include "src/gb_save.h"
#include "src/audio_sfx.h"
#include "src/memmap.h"
#include "src/memory.h"
#include "src/pokemon_stats.h"

void WidgetTree_InitScrollableGridInputProxy(unk_func_8850BD40* arg0, unk_func_8821421C_038_034* arg1) {
    WidgetTree_InitNode(arg0, sizeof(unk_func_8850BD40));
    arg0->unk_10 = WidgetTree_HandleScrollableGridInputProxy;
    arg0->unk_14 = WidgetTree_SetScrollableGridInputProxyState;
    arg0->unk_1C = arg1;
}

s32 WidgetTree_HandleScrollableGridInputProxy(unk_func_8850BD40* arg0, Controller* arg1) {
    s32 var_v1 = 0;

    if (arg0->unk_1C->unk_00.unk_00.unk_20 != NULL) {
        var_v1 = arg0->unk_1C->unk_00.unk_00.unk_20(arg0->unk_1C, arg1);
    }
    return var_v1;
}

void WidgetTree_SetScrollableGridInputProxyState(unk_func_8850BD40* arg0, s32 arg1) {
    arg0->unk_1C->unk_00.unk_00.unk_24(&arg0->unk_1C->unk_00.unk_00, arg1);
}

void WidgetTree_InitVerticalMenu(unk_func_8830867C_02C_048_000* arg0) {
    WidgetTree_InitNode(arg0, sizeof(unk_func_8830867C_02C_048_000));
    arg0->unk_10 = WidgetTree_HandleVerticalMenuInput;
    arg0->unk_14 = WidgetTree_SetVerticalMenuState;
    arg0->unk_18 = 0;
    arg0->unk_1C = 0;
    arg0->unk_20 = 0;
}

s32 WidgetTree_HandleVerticalMenuInput(unk_func_8830867C_02C_048_000* arg0, Controller* arg1) {
    s32 ret = 0;

    if (arg0->unk_20 > 0) {
        unk_func_8830867C_02C_048_000* var_a0 = arg0->unk_00.unk_04;
        s32 i;

        for (i = 0; i < arg0->unk_1C; i++) {
            var_a0 = var_a0->unk_00.unk_08;
        }
        ret = var_a0->unk_10(var_a0, arg1);
    }

    if (ret & 1) {
        return ret;
    }

    if (ret == 0) {
        if (arg1->buttonPressed & 0x4000) {
            ret = 0x80000002;
        } else if (arg1->buttonPressed & 0x8000) {
            ret = 0x80000004;
        }
    }

    if (ret & 2) {
        if (arg0->unk_1C > 0) {
            arg0->unk_1C--;
            WidgetTree_UpdateVerticalMenuItems(arg0);
            ret |= 1;
        }
    } else if (ret & 4) {
        if (arg0->unk_1C < (arg0->unk_20 - 1)) {
            arg0->unk_1C++;
            WidgetTree_UpdateVerticalMenuItems(arg0);
            ret |= 1;
        }
    }

    return ret;
}

void WidgetTree_SetVerticalMenuState(unk_func_8830867C_02C_048_000* arg0, s32 arg1) {
    arg0->unk_18 = arg1;
    WidgetTree_UpdateVerticalMenuItems(arg0);
}

void WidgetTree_AppendVerticalMenuItem(unk_func_8830867C_02C_048_000* arg0, WidgetLinkHeader* arg1) {
    WidgetTree_AppendChild(&arg0->unk_00, arg1);
    arg0->unk_20++;
}

void WidgetTree_UpdateVerticalMenuItems(unk_func_8830867C_02C_048_000* arg0) {
    s32 temp_s3;
    s32 temp_s4;
    s32 var_v0;
    s32 var_v1;
    unk_func_8830867C_02C_048_000* var_s1;
    s32 i;

    var_s1 = arg0->unk_00.unk_04;
    temp_s4 = arg0->unk_18 & 0xFF;
    temp_s3 = arg0->unk_18 & 0xFF00;

    for (i = 0; i < arg0->unk_20; i++) {
        var_v0 = 0;
        var_v1 = temp_s3;
        if (i < arg0->unk_1C) {
            var_v1 |= 0x100;
        } else if (i == arg0->unk_1C) {
            var_v0 = 1;
        }

        var_s1->unk_14(var_s1, (var_v0 & temp_s4) | var_v1);
        var_s1 = var_s1->unk_00.unk_08;
    }
}

void WidgetTree_SetVerticalMenuSelection(unk_func_8830867C_02C_048_000* arg0, s32 arg1) {
    arg0->unk_1C = arg1;
    WidgetTree_UpdateVerticalMenuItems(arg0);
}

void WidgetTree_InitHorizontalMenu(unk_func_8830867C_02C_048_000* arg0) {
    WidgetTree_InitNode(arg0, sizeof(unk_func_8830867C_02C_048_000));
    arg0->unk_10 = WidgetTree_HandleHorizontalMenuInput;
    arg0->unk_14 = WidgetTree_SetHorizontalMenuState;
    arg0->unk_18 = 0;
    arg0->unk_1C = 0;
    arg0->unk_20 = 0;
}

s32 WidgetTree_HandleHorizontalMenuInput(unk_func_8830867C_02C_048_000* arg0, Controller* arg1) {
    s32 var_a3 = 0;

    if (arg0->unk_20 > 0) {
        unk_func_8830867C_02C_048_000* var_a0 = arg0->unk_00.unk_04;
        s32 i;

        for (i = 0; i < arg0->unk_1C; i++) {
            var_a0 = var_a0->unk_00.unk_08;
        }
        var_a3 = var_a0->unk_10(var_a0, arg1);
    }

    if (var_a3 == 0) {
        s32 var_v0_2 = arg0->unk_1C;

        if (arg1->buttonPressed & 0x200) {
            if (var_v0_2 == 0) {
                var_v0_2 = arg0->unk_20 - 1;
            } else {
                var_v0_2--;
            }
        } else if (arg1->buttonPressed & 0x100) {
            if (var_v0_2 + 1 == arg0->unk_20) {
                var_v0_2 = 0;
            } else {
                var_v0_2++;
            }
        }

        if (var_v0_2 != arg0->unk_1C) {
            arg0->unk_1C = var_v0_2;
            WidgetTree_UpdateHorizontalMenuItems(arg0);
            var_a3 = 0x80000008;
        }
    }
    return var_a3;
}

void WidgetTree_SetHorizontalMenuState(unk_func_8830867C_02C_048_000* arg0, s32 arg1) {
    arg0->unk_18 = arg1;
    WidgetTree_UpdateHorizontalMenuItems(arg0);
}

void WidgetTree_AppendHorizontalMenuItem(unk_func_8830867C_02C_048_000* arg0, WidgetLinkHeader* arg1) {
    WidgetTree_AppendChild(&arg0->unk_00, arg1);
    arg0->unk_20++;
}

void WidgetTree_UpdateHorizontalMenuItems(unk_func_8830867C_02C_048_000* arg0) {
    unk_func_8830867C_02C_048_000* var_s0 = arg0->unk_00.unk_04;
    s32 i;

    for (i = 0; i < arg0->unk_20; i++) {
        s32 var_a1 = 0;

        if (i == arg0->unk_1C) {
            var_a1 = arg0->unk_18;
        }
        var_s0->unk_14(var_s0, var_a1);
        var_s0 = var_s0->unk_00.unk_08;
    }
}

void WidgetTree_InitGridMenu(WidgetGridMenu* arg0, s32 arg1, s32 arg2, MemoryPool* arg3) {
    s32 i;

    WidgetTree_InitNode(arg0, sizeof(WidgetGridMenu));
    arg0->unk_10 = WidgetTree_HandleGridMenuInput;
    arg0->unk_14 = WidgetTree_SetGridMenuState;
    arg0->unk_18 = mem_pool_alloc(arg3, (arg2 * sizeof(WidgetNode*)) * arg1);

    for (i = 0; i < arg2 * arg1; i++) {
        arg0->unk_18[i] = NULL;
    }

    arg0->unk_20 = 0;
    arg0->unk_24 = 0;
    arg0->unk_28 = 0;
    arg0->unk_30 = arg1;
    arg0->unk_2C = arg2;
    arg0->unk_1C = NULL;
    arg0->unk_36 = 0;
    arg0->unk_34 = 0;
    arg0->unk_38 = -3;
    arg0->unk_3A = -3;
    arg0->unk_3C = 6;
    arg0->unk_3E = 6;
}

s32 WidgetTree_HandleGridMenuInput(WidgetGridMenu* arg0, Controller* arg1) {
    s32 sp5C;
    UNUSED s32 pad1;
    s32 sp54;
    s32 sp50;
    s32 sp4C;
    s32 sp48;
    UNUSED s32 pad2;
    s32 i;
    s32 j;
    s32 var_t4;
    s32 sp24;

    sp5C = 0;
    var_t4 = 0;
    if (arg0->unk_18[arg0->unk_24] != NULL) {
        if (arg0->unk_18[arg0->unk_24]->unk_20 != NULL) {
            sp5C = arg0->unk_18[arg0->unk_24]->unk_20(arg0->unk_18[arg0->unk_24], arg1);
        }
    }

    if (sp5C != 0) {
        return sp5C;
    }

    sp54 = arg0->unk_24 % arg0->unk_2C;
    sp50 = arg0->unk_24 / arg0->unk_2C;
    sp24 = Input_GetRepeatedDPad() & 0xFFFF;

    if (sp24 == 0x800) {
        for (i = 0, sp48 = sp50; i < arg0->unk_30 - 1; i++) {
            sp48 = ((sp48 == 0) ? arg0->unk_30 : sp48) - 1;

            for (j = 0, sp4C = arg0->unk_34; j < arg0->unk_2C;) {
                if ((arg0->unk_18[sp48 * arg0->unk_2C + sp4C] != NULL) &&
                    !(arg0->unk_18[sp48 * arg0->unk_2C + sp4C]->unk_28 & 2)) {
                    var_t4 = 1;
                    break;
                }

                j++;
                sp4C = ((sp4C == 0) ? arg0->unk_2C : sp4C) - 1;
            }

            if (var_t4 != 0) {
                break;
            }
        }

        if ((arg0->unk_28 & 0x200) && ((var_t4 == 0) || (sp50 < sp48))) {
            sp5C = 8;
            var_t4 = 0;
        }
    } else if (sp24 == 0x400) {
        for (i = 0, sp48 = sp50; i < arg0->unk_30 - 1; i++) {
            if (sp48 == arg0->unk_30 - 1) {
                sp48 = 0;
            } else {
                sp48++;
            }

            for (j = 0, sp4C = arg0->unk_34; j < arg0->unk_2C;) {
                if ((arg0->unk_18[sp48 * arg0->unk_2C + sp4C] != NULL) &&
                    !(arg0->unk_18[sp48 * arg0->unk_2C + sp4C]->unk_28 & 2)) {
                    var_t4 = 1;
                    break;
                }

                j++;
                sp4C = ((sp4C == 0) ? arg0->unk_2C : sp4C) - 1;
            }

            if (var_t4 != 0) {
                break;
            }
        }

        if ((arg0->unk_28 & 0x200) && ((var_t4 == 0) || (sp48 < sp50))) {
            sp5C = 8;
            var_t4 = 0;
        }
    } else if (sp24 == 0x100) {
        for (j = 0, sp4C = sp54; j < arg0->unk_2C - 1; j++) {
            if (sp4C == arg0->unk_2C - 1) {
                sp4C = 0;
            } else {
                sp4C++;
            }

            for (i = 0, sp48 = arg0->unk_36; i < arg0->unk_30;) {
                if ((arg0->unk_18[sp48 * arg0->unk_2C + sp4C] != NULL) &&
                    !(arg0->unk_18[sp48 * arg0->unk_2C + sp4C]->unk_28 & 2)) {
                    var_t4 = 1;
                    break;
                }

                i++;
                sp48 = ((sp48 == 0) ? arg0->unk_30 : sp48) - 1;
            }

            if (var_t4 != 0) {
                break;
            }
        }

        if ((arg0->unk_28 & 0x100) && ((var_t4 == 0) || (sp4C < sp54))) {
            sp5C = 8;
            var_t4 = 0;
        }
    } else if (sp24 == 0x200) {
        for (j = 0, sp4C = sp54; j < arg0->unk_2C - 1; j++) {
            sp4C = ((sp4C == 0) ? arg0->unk_2C : sp4C) - 1;

            for (i = 0, sp48 = arg0->unk_36; i < arg0->unk_30;) {
                if ((arg0->unk_18[sp48 * arg0->unk_2C + sp4C] != NULL) &&
                    !(arg0->unk_18[sp48 * arg0->unk_2C + sp4C]->unk_28 & 2)) {
                    var_t4 = 1;
                    break;
                }

                i++;
                sp48 = ((sp48 == 0) ? arg0->unk_30 : sp48) - 1;
            }

            if (var_t4 != 0) {
                break;
            }
        }

        if ((arg0->unk_28 & 0x100) && ((var_t4 == 0) || (sp54 < sp4C))) {
            sp5C = 8;
            var_t4 = 0;
        }
    }

    if (var_t4 != 0) {
        arg0->unk_24 = (arg0->unk_2C * sp48) + sp4C;
        if ((sp24 == 0x800) || (sp24 == 0x400)) {
            arg0->unk_36 = sp48;
        } else {
            arg0->unk_34 = sp4C;
        }
        WidgetTree_UpdateGridMenuItems(arg0);
        sp5C = 0x80000009;
    }
    return sp5C;
}

void WidgetTree_SetGridMenuState(WidgetGridMenu* arg0, s32 arg1) {
    arg0->unk_20 = arg1;
    WidgetTree_UpdateGridMenuItems(arg0);
}

void WidgetTree_UpdateGridMenuItems(WidgetGridMenu* arg0) {
    s32 i;

    if (arg0->unk_1C != NULL) {
        arg0->unk_1C->unk_00.unk_28 &= 0xFFFE;
    }

    for (i = 0; i < arg0->unk_2C * arg0->unk_30; i++) {
        WidgetNode* temp_s1 = arg0->unk_18[i];

        if (temp_s1 != NULL) {
            s32 var_s2 = 0;

            if (i == arg0->unk_24) {
                var_s2 = arg0->unk_20;
                if (arg0->unk_1C != NULL) {
                    arg0->unk_1C->unk_00.unk_10 = temp_s1->unk_10;
                    arg0->unk_1C->unk_00.unk_14 = temp_s1->unk_14;
                    arg0->unk_1C->unk_00.unk_10.unk_00 = temp_s1->unk_10.unk_00 + arg0->unk_38;
                    arg0->unk_1C->unk_00.unk_10.unk_02 = temp_s1->unk_10.unk_02 + arg0->unk_3A;
                    arg0->unk_1C->unk_00.unk_14.unk_00 = temp_s1->unk_14.unk_00 + arg0->unk_3C;
                    arg0->unk_1C->unk_00.unk_14.unk_02 = temp_s1->unk_14.unk_02 + arg0->unk_3E;
                    arg0->unk_1C->unk_00.unk_24(&arg0->unk_1C->unk_00, var_s2);
                    if (var_s2 & 0x101) {
                        arg0->unk_1C->unk_00.unk_28 |= 1;
                    }
                }
            }
            temp_s1->unk_24(temp_s1, var_s2);
        }
    }
}

void WidgetTree_SelectFirstEnabledGridItem(WidgetGridMenu* arg0) {
    s32 i;

    for (i = 0; i < arg0->unk_2C * arg0->unk_30; i++) {
        if ((arg0->unk_18[i] != NULL) && !(arg0->unk_18[i]->unk_28 & 2)) {
            arg0->unk_24 = i;
            arg0->unk_34 = i % arg0->unk_2C;
            arg0->unk_36 = i / arg0->unk_2C;
            break;
        }
    }
}

void WidgetTree_SetGridMenuSelection(WidgetGridMenu* arg0, s32 arg1) {
    arg0->unk_24 = arg1;
    arg0->unk_34 = arg1 % arg0->unk_2C;
    arg0->unk_36 = arg1 / arg0->unk_2C;
    WidgetTree_UpdateGridMenuItems(arg0);
}

void WidgetTree_InitChildSelectionProxy(unk_func_8830867C_02C_048_000* arg0) {
    WidgetTree_InitNode(&arg0->unk_00, sizeof(unk_func_8830867C_02C_048_000));
    arg0->unk_10 = WidgetTree_HandleChildSelectionProxyInput;
    arg0->unk_14 = WidgetTree_SetChildSelectionProxyState;
    arg0->unk_18 = 0;
    arg0->unk_1C = 0;
    arg0->unk_20 = 0;
}

s32 WidgetTree_HandleChildSelectionProxyInput(unk_func_8830867C_02C_048_000* arg0, Controller* arg1) {
    s32 i;
    s32 var_v1 = 0;

    if (arg0->unk_20 > 0) {
        unk_func_8830867C_02C_048_000* var_a0 = arg0->unk_00.unk_04;

        for (i = 0; i < arg0->unk_1C; i++) {
            var_a0 = var_a0->unk_00.unk_08;
        }
        var_v1 = var_a0->unk_10(var_a0, arg1);
    }
    return var_v1;
}

void WidgetTree_SetChildSelectionProxyState(unk_func_8830867C_02C_048_000* arg0, s32 arg1) {
    arg0->unk_18 = arg1;
    WidgetTree_UpdateChildSelectionProxyItems(arg0);
}

void WidgetTree_AppendChildSelectionProxyItem(unk_func_8830867C_02C_048_000* arg0, WidgetLinkHeader* arg1) {
    WidgetTree_AppendChild(&arg0->unk_00.unk_00, arg1);
    arg0->unk_20++;
}

void WidgetTree_UpdateChildSelectionProxyItems(unk_func_8830867C_02C_048_000* arg0) {
    s32 var_a1;
    s32 i;
    unk_func_8830867C_02C_048_000* var_s0 = arg0->unk_00.unk_04;

    for (i = 0; i < arg0->unk_20; i++) {
        if (i == arg0->unk_1C) {
            var_a1 = arg0->unk_18;
        } else {
            var_a1 = 0;
        }
        var_s0->unk_14(var_s0, var_a1);
        var_s0 = var_s0->unk_00.unk_08;
    }
}

void WidgetTree_SetChildSelectionProxySelection(unk_func_8830867C_02C_048* arg0, s32 arg1) {
    arg0->unk_00.unk_1C = arg1;
    WidgetTree_UpdateChildSelectionProxyItems(&arg0->unk_00);
}

void WidgetTree_InitChildWidgetGroup(unk_func_8850CD44* arg0, s32 arg1, MemoryPool* arg2) {
    s32 i;

    ((func88500668)Memmap_GetFragmentVaddr(WidgetTree_InitNode))(arg0, sizeof(unk_func_8850CD44));
    arg0->unk_10 = WidgetTree_HandleChildWidgetGroupInput;
    arg0->unk_14 = WidgetTree_SetChildWidgetGroupState;
    arg0->unk_18 = 0;
    arg0->unk_20 = arg1;
    arg0->unk_1C = mem_pool_alloc(arg2, arg1 * 4);

    for (i = 0; i < arg1; i++) {
        arg0->unk_1C[i] = NULL;
    }
}

s32 WidgetTree_HandleChildWidgetGroupInput(unk_func_8850CD44* arg0, Controller* arg1) {
    s32 i;

    for (i = 0; i < arg0->unk_20; i++) {
        if (arg0->unk_1C[i]->unk_00.unk_28 & 0x100) {
            return 0x40;
        }
    }
    return 4;
}

void WidgetTree_SetChildWidgetGroupState(unk_func_8850CD44* arg0, s32 arg1) {
    s32 i;

    for (i = 0; i < arg0->unk_20; i++) {
        arg0->unk_1C[i]->unk_00.unk_24(arg0->unk_1C[i], arg1);
    }
}

void WidgetTree_RunChildWidgetGroup(unk_func_8850CD44* arg0, Controller* arg1) {
    s32 temp_v0;
    s32 var_s0 = 0;

    while (var_s0 == 0) {
        Ui_SendMessageAndPollInput(var_s0);
        temp_v0 = arg0->unk_10(arg0, arg1);
        if (!(temp_v0 & 1) && (temp_v0 & 4)) {
            var_s0 = 1;
        }
    }
}
