#ifndef _20330_H_
#define _20330_H_

#include "global.h"
#include "src/controller.h"

void Input_ResetRepeatState(void);
void Input_SetRepeatController(Controller* controller);
s32 Input_GetRepeatedDPad(void);

#endif // _20330_H_
