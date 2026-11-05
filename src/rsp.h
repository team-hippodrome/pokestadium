#ifndef _RSP_H_
#define _RSP_H_

#include "PR/leo.h"

struct UnkStruct800006C4_2 {
    char unk0;
    char filler1[0x1B];
    s32 unk1C;
    void* vaddr;
    s32 size;
    s32 unk28;
};

struct UnkStruct80000A80 {
    LEOCmd cmd;
    union {
        u32 lba;
        struct {
            char filler0[0x3];
            LEOSpdlMode mode;
        };
    };
    void* addr;
};

struct UnkStruct80083BD0 {
    OSMesg unk0;
    u8 unk4[0x18];
    OSMesgQueue queue1;
    OSMesgQueue queue2;
};

s32 Storage_InitializeLeoManager(void);
OSPiHandle* Storage_GetSramHandle(void);
s32 Storage_ReadSram(struct UnkStruct800006C4_2* arg0);
s32 Storage_WriteSram(struct UnkStruct800006C4_2* arg0);
s32 Storage_ReadRom(struct UnkStruct800006C4_2* arg0, s32 arg1);
s32 Storage_WriteRom(struct UnkStruct800006C4_2* arg0);
s32 Storage_ReadFlashArray(struct UnkStruct800006C4_2* arg0);
s32 Storage_StartFlashWriteBuffer(struct UnkStruct800006C4_2* arg0);
void* Storage_WaitForIoMessage(void);
void* Storage_ReadDisk(struct UnkStruct800006C4_2* arg0);
void* Storage_WriteDisk(struct UnkStruct800006C4_2* arg0);
void* Storage_ReadDiskId(struct UnkStruct80000A80* arg0);
void* Storage_SeekDisk(struct UnkStruct80000A80* arg0);
void* Storage_SetDiskMotor(struct UnkStruct80000A80* arg0);
void* Storage_ReadRtc(struct UnkStruct80000A80* arg0);
void* Storage_WriteRtc(struct UnkStruct80000A80* arg0);
void thread20_rsp(void* arg);
void rsp_init(void);
void Storage_ResetDisk(void);
void Storage_QueueRequest(void* arg0, s32 arg1);

#endif // _RSP_H_
