#ifndef _FRAGMENT28_H_
#define _FRAGMENT28_H_

#include "global.h"
#include "src/fragments/lab_widget_tree/lab_widget_tree.h"

typedef struct unk_func_888044BC_038_02C_02C_000 {
    /* 0x00 */ u8 unk_00;
} unk_func_888044BC_038_02C_02C_000; // size >= 0x4

typedef struct unk_func_888044BC_038_02C_02C {
    /* 0x00 */ unk_func_888044BC_038_02C_02C_000** unk_00;
    /* 0x04 */ char unk04[0x4];
    /* 0x08 */ s32 unk_08;
} unk_func_888044BC_038_02C_02C; // size >= 0x0C

typedef struct unk_func_888044BC_038_02C_044 {
    /* 0x00 */ WidgetNode unk_00;
} unk_func_888044BC_038_02C_044; // size >= 0x2C

typedef struct unk_func_888044BC_038_02C_000 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ unk_func_888044BC_038_02C_02C* unk_2C;
    /* 0x30 */ char unk30[0x4];
    /* 0x34 */ u16* unk_34;
    /* 0x38 */ s32 unk_38;
    /* 0x3C */ s16 unk_3C;
    /* 0x3E */ s16 unk_3E;
    /* 0x40 */ char unk40[0x4];
    /* 0x44 */ unk_func_888044BC_038_02C_044* unk_44;
    /* 0x48 */ char unk48[0x2];
    /* 0x4A */ s16 unk_4A;
    /* 0x4C */ char unk4C[0x4];
    /* 0x50 */ WidgetPoint unk_50;
    /* 0x54 */ s16 unk_54;
    /* 0x58 */ s32 unk_58;
    /* 0x5C */ s32 unk_5C;
    /* 0x60 */ char unk60[0xC];
    /* 0x6C */ s32 unk_6C;
} unk_func_888044BC_038_02C_000; // size = 0x70

typedef struct unk_func_888044BC_038_02C {
    /* 0x00 */ unk_func_888044BC_038_02C_000 unk_00;
    /* 0x70 */ unk_func_88205880_A030 unk_70;
    /* 0x7C */ unk_func_88205880_A030 unk_7C;
    /* 0x88 */ s32 unk_88;
    /* 0x8C */ s32 unk_8C;
} unk_func_888044BC_038_02C; // size = 0x90

typedef struct unk_func_888044BC_038 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ unk_func_888044BC_038_02C* unk_2C;
} unk_func_888044BC_038; // size = 0x30

typedef struct unk_func_888044BC_03C_02C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ char unk_30[8];
    /* 0x38 */ unk_func_88200FA0_030_038* unk_38;
    /* 0x3C */ s32 unk_3C;
    /* 0x40 */ s32 unk_40;
    /* 0x44 */ s32 unk_44;
} unk_func_888044BC_03C_02C; // size = 0x48

typedef struct unk_func_888044BC_03C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ unk_func_888044BC_03C_02C* unk_2C;
} unk_func_888044BC_03C; // size = 0x30

typedef struct unk_func_888044BC_040_030 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ s32 unk_2C;
} unk_func_888044BC_040_030; // size = 0x30

typedef struct unk_func_888044BC_040 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ char unk2C[0x4];
    /* 0x30 */ unk_func_888044BC_040_030* unk_30;
    /* 0x34 */ unk_func_88200FA0_030_030* unk_34;
    /* 0x38 */ unk_func_88200FA0_030_038 unk_38;
} unk_func_888044BC_040; // size = 0x44

typedef struct unk_func_888044BC_044_02C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ s32 unk_2C;
} unk_func_888044BC_044_02C; // size = 0x30

typedef struct unk_func_888044BC_044 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ unk_func_888044BC_044_02C* unk_2C;
} unk_func_888044BC_044; // size = 0x30

typedef struct unk_func_888044BC_048 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ unk_func_8850878C* unk_2C;
} unk_func_888044BC_048; // size = 0x30

typedef struct unk_func_888044BC_04C_02C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ s32 unk_2C;
} unk_func_888044BC_04C_02C; // size = 0x30

typedef struct unk_func_888044BC_04C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ unk_func_888044BC_04C_02C* unk_2C;
} unk_func_888044BC_04C; // size = 0x30

typedef struct unk_func_888044BC_050 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ unk_func_888044BC_038_02C* unk_2C;
    /* 0x30 */ s32 unk_30;
} unk_func_888044BC_050; // size = 0x34

typedef struct unk_func_888044BC {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ unk_func_88500994* unk_2C;
    /* 0x30 */ unk_func_88500994* unk_30;
    /* 0x34 */ WidgetDelayedNode* unk_34;
    /* 0x38 */ unk_func_888044BC_038* unk_38;
    /* 0x3C */ unk_func_888044BC_03C* unk_3C;
    /* 0x40 */ unk_func_888044BC_040* unk_40;
    /* 0x44 */ unk_func_888044BC_044* unk_44;
    /* 0x48 */ unk_func_888044BC_048* unk_48;
    /* 0x4C */ unk_func_888044BC_04C* unk_4C;
    /* 0x50 */ unk_func_888044BC_050* unk_50;
    /* 0x54 */ unk_func_888044BC_050* unk_54;
} unk_func_888044BC; // size = 0x58

void Pokedex_DrawCheckboxIcon(s32 arg0, s32 arg1, s32 arg2);
s32 Pokedex_DrawListScrollArrows(unk_func_888044BC_050* arg0, s32 arg1, s32 arg2);
void Pokedex_DrawSpeciesListRow(s32 arg0, s32 arg1, char* arg2, UNUSED s32 arg3, s32 arg4,
                                unk_func_888044BC_038_02C* arg5);
void Pokedex_BuildSpeciesListWidget(unk_func_888044BC_038_02C* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4,
                                    MemoryPool* arg5);
s32 Pokedex_GetSelectedSpeciesIndex(unk_func_888044BC_038_02C* arg0);
void Pokedex_ToggleListSortOrder(unk_func_888044BC_038_02C* arg0);
void Pokedex_BuildSpeciesListPanel(unk_func_888044BC_038* arg0, s32 arg1, s32 arg2, s32 arg3, MemoryPool* arg4);
s32 Pokedex_HandleSpeciesListInput(unk_func_888044BC_038* arg0, Controller* arg1);
void Pokedex_DrawListSelectionFrame(s32 arg0, s32 arg1, s32 arg2, s32 arg3, Color_RGBA8 arg4);
void Pokedex_InitTrainerInfoWidget(unk_func_888044BC_03C_02C* arg0, s32 arg1, s32 arg2);
void Pokedex_LoadTrainerInfo(unk_func_888044BC_03C_02C* arg0, s32 arg1);
void Pokedex_DrawTrainerInfoCard(unk_func_888044BC_03C_02C* arg0, s32 arg1, s32 arg2);
void Pokedex_BuildTrainerInfoPanel(unk_func_888044BC_03C* arg0, s32 arg1, s32 arg2, s32 arg3, MemoryPool* arg4);
void Pokedex_InitSortOrderLabel(unk_func_888044BC_040_030* arg0, s32 arg1, s32 arg2);
s32 Pokedex_DrawSortOrderLabel(unk_func_888044BC_040_030* arg0, s32 arg1, s32 arg2);
void Pokedex_ToggleSortOrderLabel(unk_func_888044BC_040_030* arg0);
void Pokedex_DrawEntryMenuItem(s32 arg0, s32 arg1, char* arg2, UNUSED s32 arg3, UNUSED s32 arg4,
                               unk_func_88200FA0_030_030* arg5);
void Pokedex_BuildEntryMenuWidget(unk_func_888044BC_040* arg0, s32 arg1, s32 arg2, MemoryPool* arg3);
s32 Pokedex_HandleEntryMenuInput(unk_func_888044BC_040* arg0, Controller* arg1);
void Pokedex_InitDataPanel(unk_func_888044BC_044_02C* arg0, s32 arg1, s32 arg2);
void Pokedex_SetDataPanelSpecies(unk_func_888044BC_044_02C* arg0, s32 arg1);
void Pokedex_FormatHeightString(s32 arg0, s32 arg1, s32 arg2);
void Pokedex_FormatWeightString(s32 arg0, s32 arg1, s32 arg2);
s32 Pokedex_DrawDataPanel(unk_func_888044BC_044_02C* arg0, s32 arg1, s32 arg2);
void Pokedex_BuildDataPanel(unk_func_888044BC_044* arg0, s32 arg1, s32 arg2, MemoryPool* arg3);
void Pokedex_BuildFlavorTextPanel(unk_func_888044BC_048* arg0, s32 arg1, s32 arg2, MemoryPool* arg3);
void Pokedex_SetFlavorText(unk_func_888044BC_048* arg0, s32 arg1, s32 arg2);
void Pokedex_InitPreviewWidget(unk_func_888044BC_04C_02C* arg0, s32 arg1, s32 arg2);
void Pokedex_SetPreviewSpecies(unk_func_888044BC_04C_02C* arg0, s32 arg1);
s32 Pokedex_DrawPreview(unk_func_888044BC_04C_02C* arg0, s32 arg1, s32 arg2);
void Pokedex_BuildPreviewPanel(unk_func_888044BC_04C* arg0, s32 arg1, s32 arg2, MemoryPool* arg3);
void Pokedex_BuildRootWidget(unk_func_888044BC* arg0, s32 arg1, s32 arg2, u8* arg3, MemoryPool* arg4);
s32 func_88803614(void);
s32 Pokedex_ShowEntryMenu(void);
void Pokedex_LoadIcon(u8* arg0, s32 arg1);
s32 Pokedex_ShowEntry(void);
s32 Pokedex_PlayCry(void);
s32 Pokedex_RedrawAreaMapTransition(u8 arg0);
void Pokedex_EnterAreaMapScreen(void);
void Pokedex_ExitAreaMapScreen(void);
s32 Pokedex_ShowAreaMapScreen(void);
s32 Pokedex_RedrawRootTransition(u8 arg0);
void Pokedex_RunEntryFlow(void);
s32 Pokedex_GetHighestSeenSpeciesIndex(void);
void Pokedex_InitFramebuffers(void);
s32 Pokedex_Main(UNUSED s32 arg0, UNUSED s32 arg1);

s32 Pokedex_FindNextMapArea(s32 arg0);
s32 Pokedex_FindPrevMapArea(s32 arg0);
void Pokedex_BuildMapMarkerDisplayList(void);
void Pokedex_BuildMapMarkersDisplayList(void);
void Pokedex_BuildOverviewMarkersDisplayList(void);
Gfx* Pokedex_DrawMapPinIcon(Gfx* arg0, s16 arg1, s16 arg2);
Gfx* func_88805AEC(Gfx* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6);
void Pokedex_BuildMapLabelsDisplayList(void);
void Pokedex_ResetCameraEase(void);
void Pokedex_UpdateCameraEaseSpeed(void);
void Pokedex_EaseVec3Toward(Vec3f* arg0, Vec3f* arg1);
void Pokedex_UpdateAreaMapFrame(void);
s32 Pokedex_GetCountTier(s32 arg0);
s32 Pokedex_DrawAreaMapFrame(UNUSED u8 arg0);
s32 Pokedex_CheckAreaMapExit(void);
void func_88807D04(s32 arg0, MainPoolState* arg1, UNUSED s32 arg2, s32 arg3, s32 arg4, char* arg5, char** arg6);
void Pokedex_ExitAreaMapPool(void);

#endif // _FRAGMENT28_H_
