#ifndef _2E460_H_
#define _2E460_H_

#include "global.h"
#include "game_state.h"

typedef struct unk_func_8002EBD8_arg0 {
    /* 0x00 */ char unk00[0x8];
    /* 0x08 */ u16 unk_08;
} unk_func_8002EBD8_arg0; // size >= 0xA

typedef struct unk_func_8002E128_arg0 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ s32 unk_0C;
} unk_func_8002E128_arg0; // size >= 0x10

typedef struct unk_func_8002DCB8_arg0 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
} unk_func_8002DCB8_arg0; // size >= 0xC

typedef struct unk_func_8002DD98_arg0 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
} unk_func_8002DD98_arg0; // size >= 0xC

typedef struct unk_func_8002DE78_arg0 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ s32 unk_0C;
} unk_func_8002DE78_arg0; // size >= 0x10

typedef struct unk_func_8002D860 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
} unk_func_8002D860; // size = 0xC

typedef struct ret_func_8002ED08 {
    /* 0x00 */ u8 unk_00[12];
} ret_func_8002ED08; // size = 0xC

typedef struct unk_func_8820E99C_030_030 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ s32 unk_0C;
    /* 0x10 */ s32 unk_10;
} unk_func_8820E99C_030_030; // size = 0x14

typedef struct unk_func_8830867C_044_038_030 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ s32 unk_0C;
    /* 0x10 */ s32 unk_10;
} unk_func_8830867C_044_038_030; // size = 0x14

typedef struct unk_D_8007596C {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ u16 unk_02;
} unk_D_8007596C; // size = 0x4

extern unk_func_88500020 D_800758E0;
extern unk_func_88500020 D_800758F0[];
extern unk_D_8007596C D_8007596C[];
extern s32 D_80075BD0[];

void GbSave_BuildPortSummary(unk_func_8002D860* arg0, s32 arg1);
void Gfx_DrawTiledTextureStrip(unk_func_88500020* arg0, s32 arg1, s32 a2);
s32 Ui_AdjustVerticalSelection(unk_func_8002DCB8_arg0* arg0, s32 arg1);
s32 Ui_HandleVerticalSelectionInput(s32 arg0);
s32 Ui_AdjustHorizontalSelection(unk_func_8002DD98_arg0* arg0, s32 arg1);
s32 Ui_HandleHorizontalSelectionInput(unk_func_8002DD98_arg0* arg0);
void Ui_AdjustSteppedScrollSelection(unk_func_8002DE78_arg0* arg0);
s32 Ui_AdjustNumericSelection(unk_func_8830867C_044_038_030* arg0, s32 arg1);
s32 Ui_HandleNumericSelectionInput(unk_func_8820E99C_030_030* arg0);
s32 Ui_AdjustSteppedRangeSelection(unk_func_8002E128_arg0* arg0);
void Gfx_DrawNumber(s32 arg0, s32 arg1, s32 arg2);
void Gfx_DrawNumberFixedWidth(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void Gfx_DrawNumberDigits(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32 Ui_GetSelectionDirection(unk_func_8002EBD8_arg0* arg0);
s32 GbSave_GameToCategoryIndex(s32 arg0);
Color_RGBA8 Color_InterpolateFromGray(Color_RGBA8 arg0, f32 arg1);
s32 BattleMon_CountMoves(BattleMon* arg0);
ret_func_8002ED08* Move_GetDisplayInfo(s32 arg0);
MoveData* Move_GetData(s32 arg0);
s32 Species_GetMoveSlot(BattleMon* arg0, s32 arg1);
s32 Move_NormalizeDisplayIndex(s32 arg0);
s32 Deck_GetEntryCountForType(s32 arg0, s32 arg1, s32 arg2);
void Ui_InitInputMessageQueue(void);
void Ui_SendMessageAndPollInput(OSMesg arg0);
void Ui_SendNullMessage(void);
void Ui_ReceiveMessage(OSMesg arg0);
void State_PokemonLab(void);
#if defined(VERSION_JP)
char* func_8002E160_jp(s32 item_id, char* dest);
#endif
s32 LabItem_IsQuantitylessItem(s32 arg0);
s32 LabItem_IsQuantitylessItemForOperation(s32 arg0);

#endif // _2E460_H_
