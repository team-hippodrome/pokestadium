#ifndef _FRAGMENT56_H_
#define _FRAGMENT56_H_

#include "global.h"

void Options_DrawBackground(void);
void Options_DrawFilledPanel(s16 x, s16 y, s16 width, s16 height, u8 red, u8 green, u8 blue, u8 alpha);
void Options_DrawValueCapsule(s16 x, s16 y);
void Options_DrawMainPanel(s16 opening_progress, s32 show_cursor);
void Options_DrawErasePrompt(s32 confirmation_step, s32 selected_choice, s32 opening_progress);
void Options_DrawEraseCompleteMessage(s32 opening_progress);
void Options_DrawFrame(s32 panel_step, s32 dialog_step, s32 confirmation_step, s32 selected_choice);
s32 Options_ConfirmSaveErasure(s32 confirmation_step);
void Options_ShowEraseCompleteScreen(void);
void Options_EraseSaveData(void);
void Options_ApplySoundSettings(u16 pending_settings);
s16 Options_HandleSelection(void);
void Options_RunMenu(void);
s32 Options_Main(UNUSED s32 arg0, UNUSED s32 arg1);

#endif // _FRAGMENT56_H_
