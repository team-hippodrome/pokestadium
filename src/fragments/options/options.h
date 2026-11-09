#ifndef _FRAGMENT56_H_
#define _FRAGMENT56_H_

#include "global.h"

void Options_DrawBackground(void);
void Options_DrawPanelBox(s16 left, s16 bottom, s16 right, s16 top, u8 r, u8 g, u8 b, u8 alpha);
void Options_DrawValueCapsule(s16 x, s16 y);
void Options_DrawMainPanel(s16 opening_progress, s32 fill);
void Options_DrawConfirmPrompt(s32 number_of_yeses, s32 hovering_yes, s32 opening_progress);
void Options_DrawEraseCompleteMessage(s32 raw_height);
void Options_Draw(s32 main_open_prog, s32 delete_open_prog, s32 yeses, s32 hovering_yes_for_delete);
s32 Options_ConfirmPrompt(s32 yeses);
void Options_ShowEraseCompleteScreen(void);
void Options_EraseSaveData(void);
void Options_ApplySoundSettings(u16 pending_settings);
s16 Options_ApplySelection(void);
void Options_Loop(void);
s32 Options_Main(UNUSED s32 arg0, UNUSED s32 arg1);

#endif // _FRAGMENT56_H_
