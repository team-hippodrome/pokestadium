#include "scheduler.h"
#include "src/game_state.h"
#include "src/DDC0.h"
#include "src/profiler.h"

unk_D_800A62E0 D_800A62E0;

void Sched_ResetTaskState(UnkStruct80001380* arg0) {
    osWritebackDCacheAll();
    arg0->unk_1C = 0;
    arg0->unk_1E = 0;
}

void Sched_WaitForTaskMessage(UnkStruct80001380* arg0) {
    void* sp1C;

    osRecvMesg(&arg0->queue, &sp1C, 1);
}

s32 Sched_StartTask(UnkStruct80001380* arg0) {
    s32 var_v1 = 0;

    if (arg0->unk_1C == 2) {
        osSpTaskLoad(&arg0->task);
        osSpTaskStartGo(&arg0->task);
        arg0->unk_1C = 1;
        var_v1 = 1;
    } else if (arg0->unk_1C == 0) {
        switch (arg0->task.t.type) {
            case M_AUDTASK:
                profiler_log_vblank_time();
                break;

            case M_GFXTASK:
                profiler_log_gfx_time(TASKS_QUEUED);
                arg0->unk_1E = 1;
                osDpSetStatus(0x3C0);
                break;

            case 4:
                arg0->unk_1E = 2;
                break;
        }

        osSpTaskLoad(&arg0->task);
        osSpTaskStartGo(&arg0->task);

        arg0->unk_1C = 1;
        var_v1 = 1;
    }
    return var_v1;
}

s32 Sched_RequestTaskYield(UnkStruct80001380* arg0) {
    s32 var_v1 = 0;

    switch (arg0->task.t.type) {
        case 1:
        case 4:
            if (arg0->unk_1C == 1) {
                arg0->unk_1C = 2;
                osSpTaskYield();
                var_v1 = 1;
            }
            break;
    }

    return var_v1;
}

s32 Sched_HandleRspCompletion(UnkStruct80001380* arg0) {
    s32 sp24 = 0;

    if (arg0->unk_1C == 2) {
        if (osSpTaskYielded(&arg0->task) == 0) {
            arg0->unk_1C = 3;
            switch (arg0->task.t.type) {
                case M_GFXTASK:
                    profiler_log_gfx_time(RSP_COMPLETE);
                    if (arg0->unk_1E == 2) {
                        osSendMesg(&arg0->queue, 'DONE', 0);
                    }
                    break;

                case 4:
                    osSendMesg(&arg0->queue, 'DONE', 0);
                    break;
            }
        }
        sp24 = 1;
    } else {
        switch (arg0->task.t.type) {
            case M_AUDTASK:
                if (arg0->unk_1C == 1) {
                    profiler_log_vblank_time();
                    osSendMesg(&arg0->queue, 'DONE', 0);
                }
                break;

            case M_GFXTASK:
                profiler_log_gfx_time(RSP_COMPLETE);
                if (arg0->unk_1E == 2) {
                    osSendMesg(&arg0->queue, 'DONE', 0);
                }
                break;

            case 4:
                osSendMesg(&arg0->queue, 'DONE', 0);
                break;
        }
        arg0->unk_1C = 3;
    }
    return sp24;
}

void Sched_HandleRdpCompletion(UnkStruct80001380* arg0) {
    profiler_log_gfx_time(RDP_COMPLETE);
    if (arg0->unk_1C == 3) {
        osSendMesg(&arg0->queue, 'DONE', 0);
    }
    arg0->unk_1E = 2;
}

void Sched_InitClientQueue(UnkStruct80083CA0_2* arg0, s32 arg1, s32 arg2) {
    arg0->unk_1DC = arg1;
    arg0->unk_1D8 = 0;
    osCreateMesgQueue(&arg0->unk_1C0, &arg0->unk_1B0, arg2);
}

s32 Sched_WaitClientQueue(UnkStruct80083CA0_2* arg0) {
    void* sp1C;

    osRecvMesg(&arg0->unk_1C0, &sp1C, 1);
    return sp1C;
}

s32 Sched_TryReceiveClientQueue(UnkStruct80083CA0_2* arg0) {
    u32 sp1C = 'NULL';

    if (arg0->unk_1C0.validCount != 0) {
        osRecvMesg(&arg0->unk_1C0, &sp1C, 1);
    }

    return sp1C;
}

void Sched_PromotePendingTasks(void) {
    if ((D_800A62E0.unk_A18 == NULL) && (D_800A62E0.unk_A20 != NULL)) {
        D_800A62E0.unk_A18 = D_800A62E0.unk_A20;
        D_800A62E0.unk_A20 = NULL;
    }

    if ((D_800A62E0.unk_A1C == NULL) && (D_800A62E0.unk_A24 != NULL)) {
        D_800A62E0.unk_A1C = D_800A62E0.unk_A24;
        D_800A62E0.unk_A24 = NULL;
    }
}

void func_80004DB4(void) {
}

void Sched_NotifyClients(u32 arg0) {
    unk_D_800AA660* var_s0 = D_800A62E0.unk_A10;

    while (var_s0 != NULL) {
        if (D_800A62E0.unk_A38 == 0) {
            if (var_s0->unk_01DC != 2) {
                osSendMesg(&var_s0->queue1, arg0, 0);
            }
        } else if (var_s0->unk_01DC != 0) {
            osSendMesg(&var_s0->queue1, arg0, 0);
        }
        var_s0 = var_s0->nextClient;
    }
}

UnkStruct80001380* Sched_TryStartTask(UnkStruct80001380* arg0) {
    if (arg0 != NULL) {
        if (Sched_StartTask(arg0) == 0) {
            arg0 = NULL;
        }
    }
    return arg0;
}

void Sched_StartPendingTask(void) {
    if (D_800A62E0.unk_A14 == NULL) {
        D_800A62E0.unk_A14 = Sched_TryStartTask(D_800A62E0.unk_A1C);
    }
}

s32 Sched_TryYieldActiveTask(void) {
    s32 ret = 0;

    if (D_800A62E0.unk_A14 != NULL) {
        ret = Sched_RequestTaskYield(D_800A62E0.unk_A14);
    }

    return ret;
}

void Sched_HandlePreNMI(void) {
    unk_D_800AA660* var_s0 = D_800A62E0.unk_A10;

    while (var_s0 != NULL) {
        if (var_s0->unk_01DC == 0) {
            osStopThread(&var_s0->thread);
        }
        var_s0 = var_s0->nextClient;
    }
    D_800A62E0.unk_A38 = 1;
    Audio_FadeOutForShutdown();
    Game_ResetStorageDevice();
}

void Sched_HandleRetrace(void) {
    UnkStruct80001380* var_v0;

    D_800A62E0.time = osGetTime();
    D_800A62E0.unk_A34++;

    if ((D_800A62E0.unk_A38 > 0) && (D_800A62E0.unk_A38 < 0x1E)) {
        D_800A62E0.unk_A38++;
        if (D_800A62E0.unk_A38 == 0x1A) {
            while (osAfterPreNMI() != 0) {
                IO_WRITE(SP_STATUS_REG, 0x8000);
            }
        }
    }

    Sched_PromotePendingTasks();
    func_80004DB4();

    if (D_800A62E0.unk_A18 != NULL) {
        if (D_800A62E0.unk_A14 == NULL) {
            D_800A62E0.unk_A14 = Sched_TryStartTask(D_800A62E0.unk_A18);
        } else {
            Sched_TryYieldActiveTask();
        }
    } else if ((D_800A62E0.unk_A1C != NULL) && (D_800A62E0.unk_A14 == NULL)) {
        D_800A62E0.unk_A14 = Sched_TryStartTask(D_800A62E0.unk_A1C);
    }

    Sched_NotifyClients('VTRE');
}

void Sched_HandleRspEvent(void) {
    UnkStruct80001380* sp24 = D_800A62E0.unk_A14;
    s32 tmp = Sched_HandleRspCompletion(D_800A62E0.unk_A14);

    D_800A62E0.unk_A14 = NULL;

    if (tmp) {
        D_800A62E0.unk_A14 = Sched_TryStartTask(D_800A62E0.unk_A18);
    }

    if (sp24->unk_1C == 3) {
        switch (sp24->task.t.type) {
            case M_AUDTASK:
                D_800A62E0.unk_A18 = 0;
                D_800A62E0.unk_A28 = 0;
                D_800A62E0.unk_A14 = Sched_TryStartTask(D_800A62E0.unk_A1C);
                break;

            case M_GFXTASK:
                if (D_800A62E0.unk_A1C->unk_1E == 2) {
                    D_800A62E0.unk_A1C = NULL;
                    D_800A62E0.unk_A2C = 0;
                }
                break;

            case 4:
                D_800A62E0.unk_A1C = NULL;
                D_800A62E0.unk_A30 = 0;
                break;
        }
    }
}

void Sched_HandleRdpEvent(void) {
    Sched_HandleRdpCompletion(D_800A62E0.unk_A1C);
    if (D_800A62E0.unk_A1C->unk_1C == 3) {
        D_800A62E0.unk_A1C = NULL;
        D_800A62E0.unk_A2C = 0;
    }
}

void Sched_ThreadEntry(void* arg0) {
    s32 sp24;

    __osSetFpcCsr(0x01000C01);
    osCreateMesgQueue(&D_800A62E0.queue, D_800A62E0.stack, 0x10);
    osViSetEvent(&D_800A62E0.queue, 0x66, 1);
    osSetEventMesg(4, &D_800A62E0.queue, 0x64);
    osSetEventMesg(9, &D_800A62E0.queue, 0x65);
    osSetEventMesg(0xE, &D_800A62E0.queue, 0x68);

    while (true) {
        osRecvMesg(&D_800A62E0.queue, &sp24, 1);

        switch (sp24) {
            case 0x66:
                Sched_HandleRetrace();
                break;

            case 0x64:
                Sched_HandleRspEvent();
                break;

            case 0x65:
                Sched_HandleRdpEvent();
                break;

            case 0x67:
                Sched_StartPendingTask();
                break;

            case 0x68:
                Sched_HandlePreNMI();
                break;
        }
    }
}

void Sched_Init(void) {
    D_800A62E0.unk_A10 = NULL;
    D_800A62E0.unk_A14 = NULL;
    D_800A62E0.unk_A18 = 0;
    D_800A62E0.unk_A1C = NULL;
    D_800A62E0.unk_A20 = NULL;
    D_800A62E0.unk_A24 = NULL;
    D_800A62E0.unk_A28 = 0;
    D_800A62E0.unk_A2C = 0;
    osCreateThread(&D_800A62E0.thread, 3, Sched_ThreadEntry, NULL, D_800A62E0.stack, 0x64);
    osStartThread(&D_800A62E0.thread);
}

void Sched_AddClient(unk_D_800AA660* arg0) {
    unk_D_800AA660* var_v1;
    unk_D_800AA660* var_a1;

    if (D_800A62E0.unk_A10 == NULL) {
        D_800A62E0.unk_A10 = arg0;
    } else {
        var_v1 = D_800A62E0.unk_A10;
        var_a1 = var_v1->nextClient;

        while (var_a1 != NULL) {
            var_v1 = var_a1;
            var_a1 = var_a1->nextClient;
        }
        var_v1->nextClient = arg0;
    }
    arg0->nextClient = NULL;
}

void Sched_RemoveClient(unk_D_800AA660* arg0) {
    unk_D_800AA660** var_v0 = &D_800A62E0.unk_A10;
    unk_D_800AA660* var_v1 = D_800A62E0.unk_A10;

    while (var_v1 != NULL) {
        if (var_v1 == arg0) {
            *var_v0 = var_v1->nextClient;
            break;
        }
        var_v0 = &var_v1->nextClient;
        var_v1 = var_v1->nextClient;
    }
}

void Sched_SubmitTask(UnkStruct80001380* arg0, s32 arg1) {
    if (arg0 != NULL) {
        Sched_ResetTaskState(arg0);

        switch (arg0->task.t.type) {
            case 2:
                D_800A62E0.unk_A20 = arg0;
                break;

            case 1:
                if ((arg1 != 0) && (D_800A62E0.unk_A1C == NULL)) {
                    D_800A62E0.unk_A1C = arg0;
                    D_800A62E0.unk_A24 = NULL;
                    osSendMesg(&D_800A62E0.queue, 0x67, 0);
                } else {
                    D_800A62E0.unk_A24 = arg0;
                }
                break;

            case 4:
                if (D_800A62E0.unk_A1C == NULL) {
                    D_800A62E0.unk_A1C = arg0;
                    D_800A62E0.unk_A24 = NULL;
                    osSendMesg(&D_800A62E0.queue, 0x67, 0);
                }
                break;
        }
    }
}
