#include "global.h"
#include "input.h"

static s32 pad_D_800AC880[0x2];
static s32 D_800AC888;
static Controller* gController;

void Input_ResetRepeatState(void) {
}

void Input_SetRepeatController(Controller* arg0) {
    D_800AC888 = 0xA;
    gController = arg0;
}

s32 Input_GetRepeatedDPad(void) {
    if (gController->buttonPressed & BTN_DUP) {
        D_800AC888 = 0xA;
        return BTN_DUP;
    }

    if (gController->buttonPressed & BTN_DDOWN) {
        D_800AC888 = 0xA;
        return BTN_DDOWN;
    }

    if (gController->buttonPressed & BTN_DLEFT) {
        D_800AC888 = 0xA;
        return BTN_DLEFT;
    }

    if (gController->buttonPressed & BTN_DRIGHT) {
        D_800AC888 = 0xA;
        return BTN_DRIGHT;
    }

    D_800AC888--;
    if (D_800AC888 <= 0) {
        D_800AC888 = 0;

        // clang-format off
        if (gController->buttonDown & BTN_DUP) {
            D_800AC888 = 2; return BTN_DUP;
        }
        // clang-format on

        if (gController->buttonDown & BTN_DDOWN) {
            D_800AC888 = 2;
            return BTN_DDOWN;
        }

        if (gController->buttonDown & BTN_DLEFT) {
            D_800AC888 = 2;
            return BTN_DLEFT;
        }

        if (gController->buttonDown & BTN_DRIGHT) {
            D_800AC888 = 2;
            return BTN_DRIGHT;
        }
    }

    return 0;
}
