#ifndef _FRAGMENT27_H_
#define _FRAGMENT27_H_

#include "global.h"
#include "fragments/lab_pc_list/lab_pc_list.h"

typedef struct unk_func_8860439C_sp64 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ char* unk_2C;
    /* 0x30 */ s32 unk_30;
    /* 0x34 */ Color_RGBA8 unk_34;
    /* 0x38 */ Color_RGBA8 unk_38;
} unk_func_8860439C_sp64; // size = 0x3C

s32 LabPC_ComputeExpForLevel(s32 arg0, s32 arg1);
void LabPC_SwapMonMoves(BattleMon* arg0, s32 arg1, s32 arg2);
void func_88600240(void);
void LabPC_InitDualArrowHeader(WidgetNode* arg0, s32 arg1, s32 arg2, UNUSED MemoryPool* arg3);
s32 LabPC_DrawDualArrowHeader(UNUSED WidgetNode* arg0, s32 arg1, s32 arg2);
void LabPC_InitArrowLabelA(WidgetNode* arg0, s32 arg1, s32 arg2, UNUSED MemoryPool* arg3);
s32 LabPC_DrawArrowLabelA(UNUSED WidgetNode* arg0, s32 arg1, s32 arg2);
void LabPC_InitArrowLabelB(WidgetNode* arg0, s32 arg1, s32 arg2, UNUSED MemoryPool* arg3);
s32 LabPC_DrawArrowLabelB(UNUSED WidgetNode* arg0, s32 arg1, s32 arg2);
s32 LabPC_DrawCenteredLabel(unk_func_8860439C_sp64* arg0, s32 arg1, s32 arg2);
void LabPC_InitIconWidget(unk_func_88205880_0098_008_040_02C* arg0, s32 arg1, s32 arg2, s32 arg3);
s32 LabPC_DrawIconWidget(unk_func_88205880_0098_008_040_02C* arg0, s32 arg1, s32 arg2);
void LabPC_InitMoveNameWidget(unk_func_88205880_0098_008_040_03C* arg0, s32 arg1, s32 arg2);
void LabPC_SetMoveNameId(unk_func_88205880_0098_008_040_03C* arg0, s32 arg1);
s32 LabPC_DrawMoveNameWidget(unk_func_88205880_0098_008_040_03C* arg0, s32 arg1, s32 arg2);
void LabPC_InitPpWidget(unk_func_88205880_0098_008_040_05C* arg0, s32 arg1, s32 arg2);
void LabPC_SetPpWidgetMove(unk_func_88205880_0098_008_040_05C* arg0, s32 arg1, s32 arg2);
s32 LabPC_DrawPpWidget(unk_func_88205880_0098_008_040_05C* arg0, s32 arg1, s32 arg2);
void LabPC_InitFlyWidget(unk_func_88205880_0098_008_040_06C* arg0, s32 arg1, s32 arg2);
s32 LabPC_UpdateFlyWidget(unk_func_88205880_0098_008_040_06C* arg0);
void LabPC_StartFlyWidget(unk_func_88205880_0098_008_040_06C* arg0, WidgetPoint arg1);
void LabPC_StopFlyWidget(unk_func_88205880_0098_008_040_06C* arg0);
void LabPC_InitMonSummaryWidget(unk_func_88205880_0098_008_044* arg0, s32 arg1, s32 arg2);
void LabPC_SetSummaryMon(unk_func_88205880_0098_008_044* arg0, BattleMon* arg1);
s32 LabPC_DrawMonSummaryWidget(unk_func_88205880_0098_008_044* arg0, s32 arg1, s32 arg2);
#if defined(VERSION_JP)
void LabPC_InitTypeListWidget(unk_func_88205880_0098_008_048* arg0, s32 arg1, s32 arg2);
#else
void LabPC_InitTypeListWidget(unk_func_88205880_0098_008_048* arg0, s32 arg1, s32 arg2, unk_func_8850182C* arg3);
#endif
void LabPC_SetTypeListMon(unk_func_88205880_0098_008_048* arg0, BattleMon* arg1);
s32 LabPC_DrawTypeListWidget(unk_func_88205880_0098_008_048* arg0, s32 arg1, s32 arg2);
#if defined(VERSION_JP)
void LabPC_InitStatWidget(unk_func_88205880_0098_008_04C* arg0, s32 arg1, s32 arg2);
#else
void LabPC_InitStatWidget(unk_func_88205880_0098_008_04C* arg0, s32 arg1, s32 arg2, unk_func_8850182C* arg3);
#endif
void LabPC_SetStatMon(unk_func_88205880_0098_008_04C* arg0, BattleMon* arg1);
s32 LabPC_DrawStatWidget(unk_func_88205880_0098_008_04C* arg0, s32 arg1, s32 arg2);
s32 LabPC_DrawMonPreviewWidget(unk_func_88205880_0098_008_050* arg0, s32 arg1, s32 arg2);
void LabPC_SetPreviewMon(unk_func_88205880_0098_008_050* arg0, BattleMon* arg1);
void LabPC_InitMovesPanel(unk_func_88205880_0098_008_040* arg0, s32 arg1, s32 arg2, MemoryPool* arg3);
void LabPC_SetMovesPanelMon(unk_func_88205880_0098_008_040* arg0, BattleMon* arg1);
void LabPC_GroupMovesPanelFlyWidgets(unk_func_88205880_0098_008_040* arg0, unk_func_8850CD44* arg1, MemoryPool* arg2);
void LabPC_SwapMovesPanelSlots(unk_func_88205880_0098_008_040* arg0, s32 arg1, s32 arg2);
void LabPC_ResetMovesPanelSlots(unk_func_88205880_0098_008_040* arg0, s32 arg1, s32 arg2);
void LabPC_InitTrainerInfoWidget(unk_func_88205880_0098_008_054* arg0, s32 arg1, s32 arg2);
void LabPC_SetTrainerInfoMon(unk_func_88205880_0098_008_054* arg0, BattleMon* arg1);
s32 LabPC_DrawTrainerInfoWidget(unk_func_88205880_0098_008_054* arg0, s32 arg1, s32 arg2);
void LabPC_InitMoveDetailWidget(unk_func_88205880_0098_008_058* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);
void LabPC_SetMoveDetailId(unk_func_88205880_0098_008_058* arg0, s32 arg1);
s32 LabPC_DrawMoveDetailWidget(unk_func_88205880_0098_008_058* arg0, s32 arg1, s32 arg2);
void LabPC_SetGridMenuItemCount(unk_func_88205880_0098_008_070* arg0, s32 arg1);
void LabPC_InitMoveGridMenu(unk_func_88205880_0098_008_070* arg0, MemoryPool* arg1);
s32 LabPC_HandleMoveGridInput(unk_func_88205880_0098_008_070* arg0, Controller* arg1);
void LabPC_SetDetailPanelMon(unk_func_88205880_0098_008* arg0, BattleMon* arg1, s32 arg2);
void LabPC_SetDetailPanelMode(unk_func_88205880_0098_008* arg0, s32 arg1);
s32 LabPC_GetDetailPanelMode(unk_func_88205880_0098_008* arg0);
void LabPC_ShowMoveDetailPage(unk_func_88205880_0098_008* arg0, s32 arg1);
void LabPC_SelectDetailPanelPage(unk_func_88205880_0098_008* arg0, s32 arg1);
s32 LabPC_HandleDetailPanelInput(unk_func_88205880_0098_008* arg0, Controller* arg1);
s32 LabPC_DispatchDetailPanelInput(WidgetNode* arg0, Controller* arg1);
s32 LabPC_RunDetailPanel(unk_func_88205880_0098_008* arg0);
unk_func_88205880_0098_008* LabPC_CreateMonDetailPanel(unk_func_88205880_0098* arg0);
s32 LabPC_IconListDispatch(u32 arg0, unk_func_88205880_0098* arg1);

#endif // _FRAGMENT27_H_
