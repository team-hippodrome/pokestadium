#include "global.h"
#include <PR/os_internal_reg.h>
#include "dp_intro.h"
#include "scheduler.h"
#include "gfx_rect.h"
#include "crash_screen.h"
#include "profiler.h"
#include "memory.h"

extern u64 F3DEX2_data_bin[]; // F3DEX2_data.bin

// .data
s32 D_80068B70 = 0;
char D_80068B74[] = {
    0x0E, 0x10, 0x17, 0x19, 0x00, 0x02, 0x09, 0x0B, 0x1C, 0x1E, 0x25, 0x27, 0x2A, 0x2C,
    0x33, 0x35, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

// .rodata
const f32 D_8007AF10[4] = {
    0.8333333135f,
    0.0f,
    0.0f,
    0.0f,
};

// .bss
UnkStruct80083CA0_2 D_80083CA0;
u64 D_80084760[0x100 / sizeof(u64)];
u64 D_80084860[0x408 / sizeof(u64)];
u64 D_80084C68[0xC08 / sizeof(u64)];
u64 D_80085870[0x20000 / sizeof(u64)];

// function prototypes
void Display_ClearFramebufferLine(u16 color);
void Display_SetBufferConfig(struct UnkArray4* arg0, u8 arg1, u8 arg2, u8 arg3, u8 arg4, s32 arg5, s32 arg6, s32 arg7);

void DisplayTask_Initialize(UnkStruct80001380* arg0) {
    arg0->task.t.type = M_GFXTASK;
    arg0->task.t.flags = 0;
    arg0->task.t.ucode_boot = D_80084760;
    arg0->task.t.ucode_boot_size = 0x100;
    arg0->task.t.ucode = _binary_assets_us_F3DEX2_bin_start;
    arg0->task.t.ucode_data = F3DEX2_data_bin;
    arg0->task.t.ucode_size = 0x1000;
    arg0->task.t.ucode_data_size = 0x800;
    arg0->task.t.dram_stack = (void*)ALIGN16((uintptr_t)D_80084860);
    arg0->task.t.dram_stack_size = 0x400;
    arg0->task.t.yield_data_ptr = (void*)ALIGN16((uintptr_t)D_80084C68);
    arg0->task.t.yield_data_size = 0xC00;
    arg0->task.t.output_buff = (D_80085870);
    arg0->task.t.output_buff_size = (D_80085870 + (0x20000 / sizeof(u64)));
    osCreateMesgQueue(&arg0->queue, &arg0->mesg, 1);
}

void DisplayTask_Submit(UnkStruct80001380* arg0, struct UnkArray4* arg1, s32 arg2) {
    arg0->task.t.data_ptr = (void*)(uintptr_t)arg1->unk_04;
    arg0->task.t.data_size = arg1->unk_08;
    Sched_SubmitTask(arg0, arg2);
}

void Video_SetMode(s8 arg0, s8 arg1) {
    s32 var_v0;

    if (osTvType == 2) {
        var_v0 = (arg0 * 2) + arg1 + 8;
    } else {
        var_v0 = (arg0 * 2) + arg1 + 4;
    }

    osViSetMode(&osViModeTable[(s32)D_80068B74[var_v0]]);
    osViSetSpecialFeatures(0x40U);
    osViSetSpecialFeatures(2U);
    osViSetSpecialFeatures(0x10U);
    if (osTvType == 0) {
        osViSetYScale(D_8007AF10[0]);
    }
}

void DisplayWorker_SetBufferConfig(struct UnkArray4* arg0) {
    if (arg0 == NULL) {
        D_80083CA0.unk_A8C.unk_04 = 0;
        D_80083CA0.unk_A8C.unk_08 = 0;
        D_80083CA0.unk_A8C.unk_0C = NULL;
        D_80083CA0.unk_A8C.unk_01 = D_80083CA0.unk_A9C.unk_01;
        D_80083CA0.unk_A8C.unk_02 = D_80083CA0.unk_A9C.unk_02;
        D_80083CA0.unk_A8C.unk_00 = D_80083CA0.unk_A9C.unk_00;
    } else {
        D_80083CA0.unk_A8C.unk_01 = arg0->unk_01;
        D_80083CA0.unk_A8C.unk_02 = arg0->unk_02;
        D_80083CA0.unk_A8C.unk_00 = arg0->unk_00;
        D_80083CA0.unk_A8C.unk_04 = arg0->unk_04;
        D_80083CA0.unk_A8C.unk_08 = arg0->unk_08;
        D_80083CA0.unk_A8C.unk_0C = arg0->unk_0C;
        D_80083CA0.unk_A8C.unk_03 = arg0->unk_03;
    }
}

void DisplayWorker_ProcessFrame(void) {
    s32 i;
    s32 sp20 = 0;

    if (D_80083CA0.unk_A9C.unk_04 != 0) {
        Sched_WaitForTaskMessage(&D_80083CA0.unk_A20);
    }

    if ((D_80083CA0.unk_A8C.unk_04 != 0) && (D_80083CA0.unk_AAC.unk_0C != D_80083CA0.unk_A8C.unk_0C) &&
        (D_80083CA0.unk_AAC.unk_03 != D_80083CA0.unk_A8C.unk_03)) {
        DisplayTask_Submit(&D_80083CA0.unk_A20, &D_80083CA0.unk_A8C, 1);
        sp20 = 1;
    }

    for (i = 1; i < D_80083CA0.unk_AAC.unk_00; i++) {
        Sched_WaitClientQueue(&D_80083CA0);
    }

    while (D_80083CA0.unk_1C0.validCount > 0) {
        Sched_TryReceiveClientQueue(&D_80083CA0);
    }

    if (D_80083CA0.unk_A9C.unk_0C != NULL) {
        osViSwapBuffer(D_80083CA0.unk_A9C.unk_0C->img_p);
        osViRepeatLine(0);
        if ((D_80083CA0.unk_A9C.unk_01 != D_80083CA0.unk_AAC.unk_01) ||
            (D_80083CA0.unk_A9C.unk_02 != D_80083CA0.unk_AAC.unk_02)) {
            Video_SetMode((s8)D_80083CA0.unk_A9C.unk_01, (s8)D_80083CA0.unk_A9C.unk_02);
        }
        if (D_80068B70 != 0) {
            osViBlack(1U);
        } else {
            osViBlack(0U);
        }
        crash_screen_set_draw_info((u16*)D_80083CA0.unk_A9C.unk_0C->img_p, D_80083CA0.unk_A9C.unk_0C->width, 0x10);
    } else {
        osViRepeatLine(1);
        osViSwapBuffer(D_80083CA0.unk_9E0->img_p);
        if ((D_80083CA0.unk_A9C.unk_01 != D_80083CA0.unk_AAC.unk_01) ||
            (D_80083CA0.unk_A9C.unk_02 != D_80083CA0.unk_AAC.unk_02)) {
            Video_SetMode((s8)D_80083CA0.unk_A9C.unk_01, (s8)D_80083CA0.unk_A9C.unk_02);
        }
    }

    if ((sp20 == 0) && (D_80083CA0.unk_A8C.unk_04 != 0)) {
        DisplayTask_Submit(&D_80083CA0.unk_A20, &D_80083CA0.unk_A8C, 0);
    }

    D_80083CA0.unk_AAC = D_80083CA0.unk_A9C;
    D_80083CA0.unk_A9C = D_80083CA0.unk_A8C;
    profiler_log_thread5_time(THREAD5_END);
    Sched_WaitClientQueue(&D_80083CA0);
}

void DisplayWorker_DrainEvents(void) {
    Sched_WaitClientQueue(&D_80083CA0);
    while (D_80083CA0.unk_1C0.validCount > 0) {
        Sched_TryReceiveClientQueue(&D_80083CA0);
    }
    Sched_WaitClientQueue(&D_80083CA0);
}

void DisplayWorker_ThreadMain(UNUSED void* arg) {
    __osSetFpcCsr(0x01000C01);
    Display_SetBufferConfig(&D_80083CA0.unk_AAC, 0, 1, 2, 0xFF, 0, 0, 0);
    Display_SetBufferConfig(&D_80083CA0.unk_A9C, 0, 1, 2, 0xFF, 0, 0, 0);
    Sched_InitClientQueue(&D_80083CA0.thread, 0, 4);
    Sched_AddClient(&D_80083CA0);
    DisplayTask_Initialize(&D_80083CA0.unk_A20);
    while (1) {
        void* sp4C;
        if (D_800A62E0.unk_A38 > 0) {
            DisplayWorker_DrainEvents();
            continue;
        }
        profiler_log_thread5_time(THREAD5_START);
        D_80083CA0.unk_A8A = 1;
        osRecvMesg(&D_80083CA0.unk_9EC, &sp4C, 1);
        D_80083CA0.unk_A8A = 0;
        profiler_log_thread5_time(UNK_EVENT_1);
        profiler_log_thread5_time(UNK_EVENT_2);
        DisplayWorker_SetBufferConfig(sp4C);
        DisplayWorker_ProcessFrame();
        osSendMesg(&D_80083CA0.unk_A04, (void*)'DONE', 0);
    }
}

void DisplayWorker_Initialize(void) {
    switch (osTvType) {
        case 1:
        case 2:
            break;

        default:
            osViBlack(1U);
            while (1) {
                ;
            }
            break;
    }

    osCreateThread(&D_80083CA0.thread, 5, DisplayWorker_ThreadMain, NULL, &D_80083CA0.unk_9E0, 0x28);
    osStartThread(&D_80083CA0.thread);
    osCreateMesgQueue(&D_80083CA0.unk_9EC, &D_80083CA0.unk_9E4, 1);
    osCreateMesgQueue(&D_80083CA0.unk_A04, &D_80083CA0.unk_9E8, 1);
    D_80083CA0.unk_A88 = 0;
    D_80083CA0.unk_A8A = 0;
    D_80083CA0.unk_9E0 = GfxImage_Allocate(0, IMAGE_SIZE_BITS_16b, 0x280, 1, MEMORY_POOL_RIGHT);
    Display_ClearFramebufferLine(1);
    Dma_WriteChunks(&D_80084760, 0xB0000B70, 0xB0000C70, 0);
}

/**
 * Write a specific color to a line on the framebuffer for a 640x480 resolution.
 */
void Display_ClearFramebufferLine(u16 color) {
    s32 width = 640;
    u16* buf = (u16*)D_80083CA0.unk_9E0->img_p;

    while (width-- > 0) {
        *(buf)++ = color;
    }

    osWritebackDCache(D_80083CA0.unk_9E0->img_p, 0x500);
}

u16 Display_GetFramebufferClearColor(void) {
    u16* ptr = (u16*)D_80083CA0.unk_9E0->img_p;
    return *ptr;
}

s32 Display_IsCurrentFramebuffer(void) {
    s32 result = 0;

    if (osViGetCurrentFramebuffer() == D_80083CA0.unk_9E0->img_p) {
        result = 1;
    }

    return result;
}

void Display_WaitForCompletion(void) {
    osRecvMesg(&D_80083CA0.unk_A04, NULL, 1);
}

void Display_QueueFramebufferRequest(void* arg0) {
    osSendMesg(&D_80083CA0.unk_9EC, arg0, 0);
}

void Display_WaitForFrames(s32 arg0) {
    while (arg0-- > 0) {
        Display_QueueFramebufferRequest(0);
        Display_WaitForCompletion();
    }
}

void Display_SetBufferConfig(struct UnkArray4* arg0, u8 arg1, u8 arg2, u8 arg3, u8 arg4, s32 arg5, s32 arg6, s32 arg7) {
    arg0->unk_01 = arg1;
    arg0->unk_02 = arg2;
    arg0->unk_00 = arg3;
    arg0->unk_03 = arg4;
    arg0->unk_04 = arg5;
    arg0->unk_08 = arg6;
    arg0->unk_0C = (struct unk_D_80068BB0*)(uintptr_t)arg7;
}

s32 Display_GetWorkerStatus(void) {
    return (s32)D_80083CA0.unk_AAC.unk_0C;
}

void Display_ApplyPendingVideoMode(void) {
    Video_SetMode((s8)D_80083CA0.unk_AAC.unk_01, (s8)D_80083CA0.unk_AAC.unk_02);
}

s32 Display_IsFrameReady(void) {
    s32 result = 1;

    if (D_80083CA0.unk_A9C.unk_04 != 0) {
        result = D_80083CA0.unk_A20.queue.validCount > 0;
    }
    return result;
}

void Display_EnableBlackout(void) {
    D_80068B70 = 1;
}

void Display_DisableBlackout(void) {
    D_80068B70 = 0;
}
