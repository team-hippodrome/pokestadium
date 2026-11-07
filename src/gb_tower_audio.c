#include "gb_tower_audio.h"
#include <PR/os_internal_reg.h>
#include "dp_intro.h"
#include "sections.h"
#include "memory.h"
#include "dp_intro.h"
#include "jpeg_stream.h"
#include "scheduler.h"
#include "util.h"
#include "src/fragments/gb_tower_emulator/gb_tower_emulator.h"

unk_D_800AA660* gGbTowerEmuThread;
unk_D_800AA664* gGbTowerAudioThread;
static char sSavedPollMask;

extern u8 D_81200000[];

/**
 * GB Tower audio thread: waits on the scheduler client queue and calls
 * GbEmu_AudioVideoTick each tick.
 */
void GbTower_AudioThread(UNUSED void* arg0) {
    void (*func)(void*) = Util_ConvertAddrToVirtAddr(&GbEmu_AudioVideoTick);

    __osSetFpcCsr(0x01000C01);
    Sched_InitClientQueue(gGbTowerAudioThread, 0, 1);
    Sched_AddClient(gGbTowerAudioThread);

    while (1) {
        Sched_WaitClientQueue(gGbTowerAudioThread);
        if (D_800A62E0.unk_A38 >= 0x15) {
            continue;
        }
        func(gGbTowerAudioThread);
    }
}

/**
 * GB Tower emulator thread: calls GbEmu_Init once, starts the audio thread, then
 * calls GbEmu_RunFrame on each scheduler tick.
 */
void GbTower_EmuThread(UNUSED void* arg0) {
    void (*func1)(void* func) = Util_ConvertAddrToVirtAddr(&GbEmu_Init);
    void (*func2)(void* func) = Util_ConvertAddrToVirtAddr(&GbEmu_RunFrame);

    __osSetFpcCsr(0x01000C01);
    Sched_InitClientQueue(gGbTowerEmuThread, 0, 1);
    Sched_AddClient(gGbTowerEmuThread);
    func1(gGbTowerEmuThread);
    osStartThread(&gGbTowerAudioThread->thread);

    while (1) {
        Sched_WaitClientQueue(gGbTowerEmuThread);
        if (D_800A62E0.unk_A38 >= 0x15) {
            continue;
        }
        func2(gGbTowerEmuThread);
    }
}

/**
 * Starts a GB Tower session: allocates both thread blocks, loads the emulator
 * fragment and fonts, copies the session parameters and starts the emulator thread.
 */
void GbTower_Start(GbTowerSessionParams* arg0) {
    s32 temp_v0;

    main_pool_push_state('GBEM');
    gGbTowerEmuThread = main_pool_alloc(0x2210, 0);
    gGbTowerAudioThread = main_pool_alloc(0x21E0, 0);

    FRAGMENT_LOAD(FRAGMENT_GB_TOWER_EMULATOR);
    temp_v0 = ASSET_LOAD2(fonts, 1, 1);

    gGbTowerEmuThread->font1 = BinArchive_GetFile(temp_v0, 0);
    gGbTowerEmuThread->font2 = BinArchive_GetFile(temp_v0, 1);
    gGbTowerEmuThread->session = *arg0;
    osCreateMesgQueue(&gGbTowerEmuThread->queue2, &gGbTowerEmuThread->mesg, 1);
    osCreateThread(&gGbTowerAudioThread->thread, 10, GbTower_AudioThread, NULL, (u32)gGbTowerAudioThread + 0x21E0, 0x11);
    osCreateThread(&gGbTowerEmuThread->thread, 8, GbTower_EmuThread, NULL, (u32)gGbTowerEmuThread + 0x21E0, 0xF);
    sSavedPollMask = GbTower_SetPollMask(0);
    osStartThread(&gGbTowerEmuThread->thread);
}

/**
 * Blocks until the emulator reports its exit code.
 */
OSMesg* GbTower_RecvExitMesg(void) {
    OSMesg mesg;

    osRecvMesg(&gGbTowerEmuThread->queue2, &mesg, OS_MESG_BLOCK);
    return mesg;
}

/**
 * Ends the session: copies the session parameters back (button bindings), restores
 * the pak poll mask and destroys both threads.
 */
void GbTower_Stop(GbTowerSessionParams* arg0) {
    *arg0 = gGbTowerEmuThread->session;
    GbTower_SetPollMask(sSavedPollMask);
    osViBlack(1U);
    Display_ApplyPendingVideoMode();
    Sched_RemoveClient(gGbTowerEmuThread);
    Sched_RemoveClient(gGbTowerAudioThread);
    osDestroyThread(&gGbTowerEmuThread->thread);
    osDestroyThread(&gGbTowerAudioThread->thread);
    main_pool_pop_state('GBEM');
}
