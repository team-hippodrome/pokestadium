#ifndef _DDC0_H_
#define _DDC0_H_

#include "global.h"

void func_8000D1C0(void);
void func_8000D1C8(void);
void func_8000D1D0(void);
void func_8000D1D8(void);
void Audio_ResetCurrentMusic(void);
void Audio_PlayMusicIfChanged(s32 arg0);
void Audio_PlayMusicIfChangedImmediate(s32 arg0);
void Audio_StopMusic(s32 arg0);
s32 Audio_WaitForIdle(s32 arg0);
void Audio_FadeOutForShutdown(void);
void Audio_Disable(void);
void Audio_Enable(void);
void Audio_ThreadMain(void*);
void Audio_StartThread(void);

#endif // _DDC0_H_
