#include "global.h"
#include "input.h"

static s32 pad_D_800AC880[0x2];
static s32 sRepeatDelayFrames;
static Controller* sRepeatController;

/* Empty input-repeat initialization hook. */
void Input_ResetRepeatState(void) {
}

/* Select the controller to poll and initialize its D-pad repeat delay. */
void Input_SetRepeatController(Controller* controller) {
    sRepeatDelayFrames = 10;
    sRepeatController = controller;
}

/* Return one D-pad press, repeating held directions after 10 frames and then every 2 frames. */
s32 Input_GetRepeatedDPad(void) {
    if (sRepeatController->buttonPressed & BTN_DUP) {
        sRepeatDelayFrames = 10;
        return BTN_DUP;
    }

    if (sRepeatController->buttonPressed & BTN_DDOWN) {
        sRepeatDelayFrames = 10;
        return BTN_DDOWN;
    }

    if (sRepeatController->buttonPressed & BTN_DLEFT) {
        sRepeatDelayFrames = 10;
        return BTN_DLEFT;
    }

    if (sRepeatController->buttonPressed & BTN_DRIGHT) {
        sRepeatDelayFrames = 10;
        return BTN_DRIGHT;
    }

    sRepeatDelayFrames--;
    if (sRepeatDelayFrames <= 0) {
        sRepeatDelayFrames = 0;

        // FAKE: keep the packed assignment/return line for matching.
        // clang-format off
        if (sRepeatController->buttonDown & BTN_DUP) {
            sRepeatDelayFrames = 2; return BTN_DUP;
        }
        // clang-format on

        if (sRepeatController->buttonDown & BTN_DDOWN) {
            sRepeatDelayFrames = 2;
            return BTN_DDOWN;
        }

        if (sRepeatController->buttonDown & BTN_DLEFT) {
            sRepeatDelayFrames = 2;
            return BTN_DLEFT;
        }

        if (sRepeatController->buttonDown & BTN_DRIGHT) {
            sRepeatDelayFrames = 2;
            return BTN_DRIGHT;
        }
    }

    return 0;
}
