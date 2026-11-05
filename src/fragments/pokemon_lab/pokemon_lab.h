#ifndef _FRAGMENT22_H_
#define _FRAGMENT22_H_

#include "global.h"
#include "src/fragments/widget_tree/widget_tree.h"

typedef struct unk_func_8800071C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ u8* unk_2C;
    /* 0x30 */ u8* unk_30;
    /* 0x34 */ Color_RGBA8 unk_34;
    /* 0x38 */ Color_RGBA8 unk_38;
} unk_func_8800071C; // size = 0x3C

typedef struct unk_func_88000830 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ u8** unk_30;
    /* 0x34 */ s32 unk_34;
} unk_func_88000830; // size = 0x38

typedef struct unk_func_88000C18 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ Color_RGBA8 unk_2C;
    /* 0x30 */ s32 unk_30;
} unk_func_88000C18; // size = 0x34

typedef struct unk_func_88000CC8 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ u8* unk_2C;
    /* 0x30 */ u8* unk_30[2];
    /* 0x38 */ u8* unk_38[3];
    /* 0x44 */ s32 unk_44;
    /* 0x48 */ s32 unk_48;
    /* 0x4C */ s32 unk_4C;
    /* 0x50 */ WidgetTextList* unk_50;
} unk_func_88000CC8; // size = 0x54

void Lab_InitTiledBackgroundWidget(WidgetNode* arg0, s32 arg1, s32 arg2);
s32 Lab_DrawTiledBackgroundWidget(WidgetNode* arg0, s32 arg1, s32 arg2);
void Lab_InitDualLayerIconWidget(unk_func_8800071C* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, u8* arg5, u8* arg6,
                                 Color_RGBA8 arg7, Color_RGBA8 arg8);
s32 Lab_DrawDualLayerIconWidget(WidgetNode* arg0, s32 arg1, s32 arg2);
void Lab_InitLoopAnimWidget(unk_func_88000830* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, u8** arg6);
s32 Lab_UpdateLoopAnimWidget(WidgetNode* arg0);
s32 Lab_DrawLoopAnimWidget(unk_func_88000830* arg0, s32 arg1, s32 arg2);
void Lab_InitFrameTimerWidget(unk_func_88000C18* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, Color_RGBA8 arg5);
void Lab_UpdateFrameTimerWidget(WidgetNode* arg0);
void Lab_SetFrameTimerState(WidgetMainMenu* arg0, s32 arg1);
void Lab_InitAnimatedIconWidget(unk_func_88000CC8* arg0, s32 arg1, s32 arg2, unk_func_88500020* arg3, u8* arg4,
                                u8* arg5, u8* arg6, u8* arg7, u8* arg8, WidgetTextList* arg9);
void Lab_UpdateAnimatedIconWidget(WidgetNode* arg0);
s32 Lab_DrawAnimatedIconWidget(WidgetNode* arg0, s32 arg1, s32 arg2);
void Lab_ShowMenuItemDescription(WidgetMainMenu* arg0);
void Lab_SetSelectedMenuItem(WidgetMainMenu* arg0, s32 arg1);
s32 Lab_PollMenuInput(WidgetMainMenu* arg0, Controller* controller);
s32 Lab_CountConnectedCarts(void);
s32 Lab_HandleMainMenuInput(WidgetMainMenu* arg0);
void Lab_MainMenuThread(WidgetMainMenu* arg0);
void Lab_DrawFrame(WidgetMainMenu* arg0);
s32 Lab_RunFrameLoop(WidgetMainMenu* arg0);
s32 Lab_Main(s32 arg0, UNUSED s32 arg1);

#endif // _FRAGMENT22_H_
