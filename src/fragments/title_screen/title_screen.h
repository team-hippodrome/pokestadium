#ifndef _FRAGMENT36_H_
#define _FRAGMENT36_H_

#include "global.h"

void func_82100020(void);
void func_82100028(UNUSED s32 arg0, UNUSED s32 arg1);
void Title_PollDPad(void);
s32 Title_ResolveStartPressState(void);
void Title_DrawPressStartBlink(s16 arg0, s16 arg1);
void Title_DrawNoControllerWarning(void);
void Title_DrawBackgroundImage(u8* arg0);
#if defined(VERSION_JP)
void Title_DrawDiskErrorMessage(s32 disk_status);
void Title_DrawFrame(s32 disk_status);
s32 Title_IsBootStateIdle(s32 boot_state);
s32 Title_ShouldStartAttractDemo(s32 boot_state);
void Title_PollInputs(void);
#else
void Title_DrawFrame(void);
s32 Title_ShouldStartAttractDemo(void);
#endif
s32 Title_CountEligibleDemoEntries(s32 arg0);
s32 Title_FindEligibleDemoEntry(s32 arg0, u32 arg1);
void Title_PickDemoMatchup(u8* arg0);
s32 Title_StartAttractDemo(void);
s32 func_821009AC(void);
s32 Title_Loop(void);
void Title_FadeIn(void);
void Title_FadeOut(s32 arg0);
void Title_LoadAssets(void);
s32 Title_Main(UNUSED s32 arg0, s16* arg1);

#endif // _FRAGMENT36_H_
