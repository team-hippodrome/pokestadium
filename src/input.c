#include "global.h"
#include "input.h"

#if defined(VERSION_JP)
#include "src/dma.h"
#include "src/leo.h"
#include "src/graphics_textures.h"

extern s32 D_8006F7C0[];
s32 Audio_GetDiskReadyStatus(u8* status);
void Audio_SetDiskReadyPolling(s32 enable);

// Used only by the JP disk polling below. The other versions keep the space.
static u8 sDiskReadyLast;
static s32 sDiskResetPending;
#else
static s32 pad_D_800AC880[0x2];
#endif
static s32 sRepeatDelayFrames;
static Controller* sRepeatController;

#if defined(VERSION_JP)
void Storage_StartDiskReadyPolling(void) {
    if (Storage_GetDiskStatus()) {
        D_8006F7C0[0] = 1;
        sDiskResetPending = 0;
        sDiskReadyLast = 0;
        Audio_SetDiskReadyPolling(1);
    }
}

void Storage_StopDiskReadyPolling(void) {
    if (Storage_GetDiskStatus()) {
        D_8006F7C0[0] = 0;
        sDiskResetPending = 0;
        sDiskReadyLast = 0;
        Audio_SetDiskReadyPolling(0);
    }
}

// While polling, a falling edge of the disk ready bit resets the boot state and then holds
// START pressed on controller 1.
void Storage_PollDiskReady(void) {
    u8 status;

    if (D_8006F7C0[0] != 0) {
        if (Storage_GetDiskStatus()) {
            if (sDiskResetPending) {
                gPlayer1Controller->buttonDown |= BTN_START;
                gPlayer1Controller->buttonPressed |= BTN_START;
            } else if (Audio_GetDiskReadyStatus(&status) != 8) {
                status &= 1;
                if ((status ^ sDiskReadyLast) & sDiskReadyLast) {
                    Leo_ResetBootStateIfReady();
                    sDiskResetPending = 1;
                    gPlayer1Controller->buttonDown |= BTN_START;
                    gPlayer1Controller->buttonPressed |= BTN_START;
                }
                sDiskReadyLast = status;
            }
        }
    }
}
#endif

/* Input-repeat initialization hook: empty, except that JP polls the disk drive here. */
void Input_ResetRepeatState(void) {
#if defined(VERSION_JP)
    Storage_PollDiskReady();
#endif
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
