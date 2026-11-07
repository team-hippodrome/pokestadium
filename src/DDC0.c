#include <PR/os_internal_reg.h>
#include "global.h"
#include "dp_intro.h"
#include "profiler.h"
#include "audio_loop_point.h"
#include "scheduler.h"
#include "gb_tower_audio.h"
#include "audio_commands.h"
#include "src/audio_category_fade.h"
#include "src/audio_sound_state.h"
#include "src/audio_channel.h"
#include "src/audio_loop_point.h"

typedef struct unk_D_800A8480 {
    /* 0x0000 */ OSThread thread;
    /* 0x01B0 */ char unk01B0[0x2030];
} unk_D_800A8480; // size = 0x21E0

static s32 D_800A83A0;
static UnkStruct80001380 D_800A83A8[2];
static s32 D_800A8478;
static s32 D_800A847C;
static unk_D_800A8480 D_800A8480;

void func_8000D1C0(void) {
}

void func_8000D1C8(void) {
}

void func_8000D1D0(void) {
}

void func_8000D1D8(void) {
}

void Audio_ResetCurrentMusic(void) {
    D_800A847C = -1;
}

void Audio_PlayMusicIfChanged(s32 arg0) {
    if (arg0 != D_800A847C) {
        if (D_800A847C >= 0) {
            Audio_FadeOutAll(0);
        }
        Audio_StartMusicTrack(arg0);
        D_800A847C = arg0;
    }
}

void Audio_PlayMusicIfChangedImmediate(s32 arg0) {
    if (arg0 != D_800A847C) {
        Audio_StartMusicTrack(arg0);
        D_800A847C = arg0;
    }
}

void Audio_StopMusic(s32 arg0) {
    if (D_800A847C >= 0) {
        Audio_FadeOutAll(arg0);
        D_800A847C = -1;
    }
}

s32 Audio_WaitForIdle(s32 arg0) {
    s32 retvar = 0;

    if (arg0 != 0) {
        Audio_FadeCategoriesAndStreams(2);
    }
    while (Audio_GetActivityScore() != 0) {
        if (retvar < 1000000) {
            retvar++;
        } else if (retvar == 1000000) {
            Audio_FadeCategoriesAndStreamsDefault();
            retvar++;
        }
    }
    return retvar;
}

void Audio_FadeOutForShutdown(void) {
    Audio_FadeCategoriesAndStreamsWithDuration(0x10);
}

void Audio_Disable(void) {
    D_800A83A0 = 0;
    Audio_SetEnabled(0);
}

void Audio_Enable(void) {
    Audio_SetEnabled(1);
    D_800A83A0 = 1;
}

void Audio_ThreadMain(void* unused) {
    __osSetFpcCsr(0x01000C01);
    Sched_InitClientQueue(&D_800A8480.thread, 1, 1);
    Sched_AddClient(&D_800A8480.thread);
    D_800A83A0 = 1;
    D_800A847C = -1;
    D_800A8478 = 0;
    osCreateMesgQueue(&D_800A83A8[0].queue, &D_800A83A8[0].mesg, 1);
    osCreateMesgQueue(&D_800A83A8[1].queue, &D_800A83A8[1].mesg, 1);
    osSendMesg(&D_800A83A8[0].queue, (void*)'DONE', 0);
    osSendMesg(&D_800A83A8[1].queue, (void*)'DONE', 0);
    Audio_InitBanks();
    Audio_SetMonoOutput(0);
    Audio_SetChannelVolume(3, 4);

    // thread loop
    while (1) {
        Sched_WaitClientQueue(&D_800A8480.thread);
        profiler_log_thread4_time();
        if ((D_800A83A0 != 0) && (D_800A62E0.unk_A38 < 0x15)) {
            Audio_CreateFrameTaskWrapper(&D_800A83A8[D_800A8478].task);
            Sched_SubmitTask(&D_800A83A8[D_800A8478], 0);
        }
        D_800A8478 ^= 1;
        profiler_log_thread4_time();
    }
}

void Audio_StartThread(void) {
    osCreateThread(&D_800A8480.thread, 4, Audio_ThreadMain, NULL, &gGbTowerEmuThread, 0x50);
    osStartThread(&D_800A8480.thread);
}
