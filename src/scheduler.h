#ifndef _5580_H_
#define _5580_H_

#include "global.h"

typedef struct UnkStruct80001380 {
    /* 0x00 */ OSMesg mesg;
    /* 0x04 */ OSMesgQueue queue;
    /* 0x1C */ u16 unk_1C;
    /* 0x1E */ u16 unk_1E;
    /* 0x20 */ OSTask task;
    /* 0x60 */ char unk60[0x8];
} UnkStruct80001380; // size = 0x68

typedef struct unk_D_800A62E0 {
    /* 0x000 */ OSThread thread;
    /* 0x1B0 */ char unk1B0[0x800];
    /* 0x9B0 */ u8 stack[64];
    /* 0x9F0 */ OSMesgQueue queue;
    /* 0xA08 */ u64 time;
    /* 0xA10 */ unk_D_800AA660* unk_A10;
    /* 0xA14 */ UnkStruct80001380* unk_A14;
    /* 0xA18 */ UnkStruct80001380* unk_A18;
    /* 0xA1C */ UnkStruct80001380* unk_A1C;
    /* 0xA20 */ UnkStruct80001380* unk_A20;
    /* 0xA24 */ UnkStruct80001380* unk_A24;
    /* 0xA28 */ s32 unk_A28;
    /* 0xA2C */ s32 unk_A2C;
    /* 0xA30 */ s32 unk_A30;
    /* 0xA34 */ s32 unk_A34;
    /* 0xA38 */ s16 unk_A38;
    /* 0xA3A */ char unkA3A[0x6];
} unk_D_800A62E0; // size = 0xA40

struct unk_D_80068BB0;

typedef struct UnkArray4 {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x03 */ u8 unk_03;
    /* 0x04 */ u32 unk_04;
    /* 0x08 */ u32 unk_08;
    /* 0x0C */ struct unk_D_80068BB0* unk_0C;
} UnkArray4; // size = 0x10

typedef struct UnkStruct80083CA0_2 {
    /* 0x000 */ OSThread thread;
    /* 0x1B0 */ OSMesg unk_1B0;
    /* 0x1B4 */ char unk1B4[0xC];
    /* 0x1C0 */ OSMesgQueue unk_1C0;
    /* 0x1D8 */ s32 unk_1D8;
    /* 0x1DC */ s32 unk_1DC;
    /* 0x1E0 */ u8 unk1E0[0x800];
    /* 0x9E0 */ struct unk_D_80068BB0* unk_9E0;
    /* 0x9E4 */ OSMesg unk_9E4;
    /* 0x9E8 */ OSMesg unk_9E8;
    /* 0x9EC */ OSMesgQueue unk_9EC;
    /* 0xA04 */ OSMesgQueue unk_A04;
    /* 0xA1C */ char unkA1C[0x4];
    /* 0xA20 */ UnkStruct80001380 unk_A20;
    /* 0xA88 */ u16 unk_A88;
    /* 0xA8A */ u8 unk_A8A;
    /* 0xA8B */ u8 unk_A8B;
    /* 0xA8C */ UnkArray4 unk_A8C;
    /* 0xA9C */ UnkArray4 unk_A9C;
    /* 0xAAC */ UnkArray4 unk_AAC;
    /* 0xABC */ char unkABC[0x4];
} UnkStruct80083CA0_2; // size = 0xAC0

extern unk_D_800A62E0 D_800A62E0;

void Sched_InitClientQueue(UnkStruct80083CA0_2*, s32, s32);
s32 Sched_WaitClientQueue(UnkStruct80083CA0_2*);
s32 Sched_TryReceiveClientQueue(UnkStruct80083CA0_2*);
void Sched_Init(void);
void Sched_AddClient(unk_D_800AA660*);
void Sched_ResetTaskState(UnkStruct80001380* arg0);
void Sched_WaitForTaskMessage(UnkStruct80001380* arg0);
void Sched_RemoveClient(unk_D_800AA660* arg0);
void Sched_SubmitTask(UnkStruct80001380* arg0, s32 arg1);

#endif // _5580_H_
