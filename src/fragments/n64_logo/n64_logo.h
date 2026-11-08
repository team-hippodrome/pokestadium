#ifndef _FRAGMENT35_H_
#define _FRAGMENT35_H_

#include "global.h"

void N64Logo_DrawLogoTexture(void);
s32 N64Logo_DrawFrame(UNUSED u8 arg0);
void N64Logo_WaitForElapsed(u64 start_time);
void N64Logo_ScanTransferPaks(void);
s32 N64Logo_Main(UNUSED s32 arg0, UNUSED s32 arg1);

#endif // _FRAGMENT35_H_
