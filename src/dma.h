#ifndef _DMA_H_
#define _DMA_H_

struct UnkStruct80000E80 {
    char unk0;
    char filler4[0x18];
    s32 unk1C;
    u8* unk20;
    s32 unk24;
    s32 unk28;
};

s32 Storage_QueueRomRead(s32 arg0, void* arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5);
s32 Storage_QueueRomWrite(s32 arg0, void* arg1, s32 arg2, s32 arg3, s32 arg4);
s32 Storage_QueueSramTransfer(s32 arg0, void* arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5);
s32 Storage_QueueFlashTransfer(s32 arg0, void* arg1, s16 arg2, s32 arg3, s32 arg4);
s32 Storage_QueueDiskTransfer(s32 arg0, void* arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5);
s32 Storage_QueueDiskIdRead(s32 arg0, s32 arg1);
s32 Storage_QueueDiskSeek(s32 arg0, s32 arg1, s32 arg2);
s32 Storage_QueueDiskMotorOff(s32 arg0, s32 arg1);
s32 Storage_QueueDiskReset(s32 arg0, s32 arg1);
s32 Storage_QueueRtcWrite(void* arg0, s32 arg1, s32 arg2);
s16 Storage_GetDiskStatus(void);
s32 Storage_QueueRtcRead(void* arg0, s32 arg1, s32 arg2);

#endif // _DMA_H_
