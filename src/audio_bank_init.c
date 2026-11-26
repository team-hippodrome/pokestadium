#include "audio_commands.h"
#include "src/audio_commands_category2.h"
#include "src/audio_commands.h"
#include "src/3D140.h"
#include "src/libnumus/player.h"
#include "src/rom_device.h"
#include "src/audio_channel.h"
#include "src/audio_cache.h"
#include "src/cry.h"
#include "src/audio_fade.h"

// Split from audio_commands: the PAL releases link audio_loop_point and cry between the two files.

// .bss
u8 D_800B32A0[0x493E0];
u32* D_800FC680;
u8* D_800FC684;
u8* D_800FC688;
u8* D_800FC68C;
u8* D_800FC690;
u8 pad_D_800FC694[4];
u8* D_800FC698[3];
u8* D_800FC6A4;
u8* D_800FC6A8;
u8* D_800FC6AC;
u8* D_800FC6B0[3];
MoveSoundTable* D_800FC6C0[3];
unk_D_800FC6CC* D_800FC6CC;
u8* D_800FC6D0;
unk_D_800FC6D4* D_800FC6D4;
s16* D_800FC6D8;
u8* D_800FC6DC;
u32* gAudioRegions;
File1Header* D_800FC6E4;
ALSeqFile* D_800FC6E8;
File2SubHeader1* D_800FC6EC;
File2SubHeader2* D_800FC6F0;
ALSeqFile* D_800FC6F4;
ALSeqFile* D_800FC6F8;
File2SubHeader2* D_800FC6FC;
ALSeqFile* D_800FC700;
ALSeqFile* D_800FC704;
ALSeqFile* D_800FC708;
s32 pad_D_800FC70C;
s32 pad_D_800FC710;
ALSeqFile* D_800FC714;
u8 pad_D_800FC718[0x80];
u32* D_800FC798;

// .data
static s32 pad_D_80077C80[2] = { 0 };
#if defined(VERSION_PAL)
// Set when the move sound bank buffer is reused, so the next move sound reloads it.
u8 sMoveSoundBankStale = 0;
#endif

void Audio_CreateFrameTaskWrapper(void* arg0) {
    Audio_CreateFrameTask(arg0);
}

void Audio_RelocateOffsets(u32* arg0, u32 arg1, s32 arg2) {
    u32 i;

    for (i = 0; i < arg2; i++, arg0++) {
        *arg0 += arg1;
    }
}

#define AUDIO_HEAP_SIZE 300000

s32 Audio_InitBanks(void) {
    s32 i;
    FileHeaders* sp78;
    UNUSED s32 pad[3];
    u32 num_files;
    musConfig config;

    config.channels = 0x18;
    config.vvoices = 0x34;
    config.pvoices = 0x18;
    config.heap = D_800B32A0;
    config.heap_length = AUDIO_HEAP_SIZE;
    config.syn_updates = 0x100;
    config.syn_output_rate = 32000;
    config.syn_rsp_cmds = 0x440;
    config.syn_retraceCount = 1;
    config.syn_num_dma_bufs = NTSC_PAL(0x30, 0x44);
    config.syn_dma_buf_size = NTSC_PAL(0x400, 0x380);
    config.command_queue_max_size = NTSC_PAL(0x400, 0x800);

    Audio_InitializeVoiceManager(&config);
    Rom_InitDevice(2);

    D_800FC6DC = alHeapAlloc(audio_heap, 1, 0x61A8);
    D_800FC798 = alHeapAlloc(audio_heap, 8, 4);
    gAudioRegions = alHeapAlloc(audio_heap, 7, 4);

    // Copy rom data from 0x15C0000 into gAudioRegions
    Rom_DmaRead(&gAudioData, gAudioRegions, sizeof(u8*) * 7);
    // Add the address 0x15C0000 to every offset (besides the first which is the total size)
    // to make absolute offsets.
    Audio_RelocateOffsets(&gAudioRegions[1], &gAudioData, 6);
    sp78 = D_800FC798;
    // Copy data from the offset at 0x15C0004 into sp78, a sub-header for this "archive,"
    // specifying the size of the header and number of files etc. Header at 0x15C0020
    Rom_DmaRead(gAudioRegions[1], sp78, 0xC);
    num_files = sp78->file1.num_files;
    // Allocate vram, copy rom bytes, and add address to make absolute,
    // for all of the first archive's files, including header.
    D_800FC6E4 = alHeapAlloc(audio_heap, (0xC / sizeof(u32)) + num_files, sizeof(u32));
    Rom_DmaRead(gAudioRegions[1], D_800FC6E4, 0xC + (num_files * 4));
    // Update just the first 2 pointers from the header,
    // pointer to first file, and pointer to N64 wave tables.
    Audio_RelocateOffsets(&D_800FC6E4->offset1, gAudioRegions[1], 2);
    // Make absolute all the offsets after the header, for the archive files at 0x15C002C
    Audio_RelocateOffsets(D_800FC6E4->files, gAudioRegions[1], num_files);
    // Copy the first file into D_800FC6DC. What is this?
    // Yay0 compressed at 0x15C0160
    Rom_DmaRead(D_800FC6E4->offset1, D_800FC6DC, D_800FC6E4->wave_tables_offset - D_800FC6E4->offset1);

    // Decompressed first file memory.
    D_800FC684 = alHeapAlloc(audio_heap, 1, 0x4120);

    // Decompress first file Yay0 from D_800FC6DC, into D_800FC684
    Audio_CopyDataWithCacheSync(D_800FC6DC, D_800FC684, 0x4120);

    // ??
    Audio_RelocateSoundBankWrapper(D_800FC684, D_800FC6E4->wave_tables_offset);

    D_800FC680 = alHeapAlloc(audio_heap, 1, 0x98D8);

    // Load second archive header at 0x16F27E0
    Rom_DmaRead(gAudioRegions[2], sp78, 4 + 4);
    num_files = sp78->seq.seqCount;
    // Memory to hold archive toc
    D_800FC6E8 = alHeapAlloc(audio_heap, 1, 4 + (num_files * sizeof(ALSeqData)));
    // Read the alSeqFile into D_800FC6E8
    Rom_DmaRead(gAudioRegions[2], D_800FC6E8, 4 + (num_files * sizeof(ALSeqData)));
    // Initialise the memory as an alSeqFile
    alSeqFileNew(D_800FC6E8, gAudioRegions[2]);

    // Read the first archive sub header from 0x16F2804, into D_800FC6EC
    D_800FC6EC = alHeapAlloc(audio_heap, 3, 4);
    Rom_DmaRead(D_800FC6E8->seqArray[0].offset, D_800FC6EC, sizeof(File2SubHeader1));
    Audio_RelocateOffsets(D_800FC6EC, D_800FC6E8->seqArray[0].offset, sizeof(File2SubHeader1) / sizeof(u32));

    // Read the first file from the sub header into D_800FC6DC (0x16F2814)
    Rom_DmaRead(D_800FC6EC->offset1, D_800FC6DC, D_800FC6EC->offset2 - D_800FC6EC->offset1);
    // Decompress the yay0 file into D_800FC688
    D_800FC688 = alHeapAlloc(audio_heap, 1, 0xBB8);
    Audio_CopyDataWithCacheSync(D_800FC6DC, D_800FC688, 0xBB8);
    // ???
    Audio_RelocateSoundBankTable(D_800FC688);

    // Read the second file from the sub header into D_800FC6DC (0x16F2BD0)
    Rom_DmaRead(D_800FC6EC->offset2, D_800FC6DC, D_800FC6EC->wave_tables_offset - D_800FC6EC->offset2);
    // Decompress the yay0 file into D_800FC68C
    D_800FC68C = alHeapAlloc(audio_heap, 1, 0x157C);
    Audio_CopyDataWithCacheSync(D_800FC6DC, D_800FC68C, 0x157C);
    // ???
    Audio_RelocateSoundBankWrapper(D_800FC68C, D_800FC6EC->wave_tables_offset);

    // Read archive 2 sub file 2 (0x1722F74)
    Rom_DmaRead(D_800FC6E8->seqArray[1].offset, sp78, sizeof(File2SubHeader2));
    num_files = sp78->file2sub2.num_files;

    D_800FC6F0 = alHeapAlloc(audio_heap, 4, (sizeof(File2SubHeader2) / sizeof(u32)) + num_files);
    Rom_DmaRead(D_800FC6E8->seqArray[1].offset, D_800FC6F0, sizeof(File2SubHeader2) + (num_files * sizeof(u32)));
    Audio_RelocateOffsets(&D_800FC6F0->offset1, D_800FC6E8->seqArray[1].offset, num_files + 2);
    Rom_DmaRead(D_800FC6F0->offset1, D_800FC6DC, D_800FC6F0->wave_tables_offset - D_800FC6F0->offset1);
    // Decompress the yay0 file into D_800FC688
    D_800FC690 = alHeapAlloc(audio_heap, 1, 0x2EE0);
    Audio_CopyDataWithCacheSync(D_800FC6DC, D_800FC690, 0x2EE0);
    // ???
    Audio_RelocateSoundBankWrapper(D_800FC690, D_800FC6F0->wave_tables_offset);

    D_800FC698[0] = alHeapAlloc(audio_heap, 1, 0x258);
    D_800FC698[1] = alHeapAlloc(audio_heap, 1, 0x258);
    D_800FC698[2] = alHeapAlloc(audio_heap, 1, 0x258);
#if defined(VERSION_PAL)
    sMoveSoundBankStale = 0;
#endif

    // Read the second seq array (0x17C9C24)
    Rom_DmaRead(D_800FC6E8->seqArray[2].offset, sp78, sizeof(File2SubHeader2));
    num_files = sp78->file2sub2.num_files;

    D_800FC6FC = alHeapAlloc(audio_heap, 4, num_files + 3);
    Rom_DmaRead(D_800FC6E8->seqArray[2].offset, D_800FC6FC, sizeof(File2SubHeader2) + (num_files * sizeof(u32)));
    Audio_RelocateOffsets(&D_800FC6FC->offset1, D_800FC6E8->seqArray[2].offset, num_files + 2);
    Rom_DmaRead(D_800FC6FC->offset1, D_800FC6DC, D_800FC6FC->wave_tables_offset - D_800FC6FC->offset1);
    D_800FC6AC = alHeapAlloc(audio_heap, 1, 0x9C4);
    // Decompress
    Audio_CopyDataWithCacheSync(D_800FC6DC, D_800FC6AC, 0x9C4);
    Audio_RelocateSoundBankWrapper(D_800FC6AC, D_800FC6FC->wave_tables_offset);

    D_800FC6B0[0] = alHeapAlloc(audio_heap, 1, 0x3E8);
    D_800FC6B0[1] = alHeapAlloc(audio_heap, 1, 0x3E8);
    D_800FC6B0[2] = alHeapAlloc(audio_heap, 1, 0x3E8);

    // Read seq header from 0x17DC304
    Rom_DmaRead(D_800FC6E8->seqArray[3].offset, sp78, 4 + 4);
    num_files = sp78->seq.seqCount;

    D_800FC6F4 = alHeapAlloc(audio_heap, 4, 4 + (num_files * sizeof(ALSeqData)));
    Rom_DmaRead(D_800FC6E8->seqArray[3].offset, D_800FC6F4, 4 + (num_files * sizeof(ALSeqData)));
    alSeqFileNew(D_800FC6F4, D_800FC6E8->seqArray[3].offset);

    // Load second seq header from 0x1850C20
    Rom_DmaRead(D_800FC6F4->seqArray[1].offset, sp78, 4 + 4);
    num_files = sp78->seq.seqCount;

    D_800FC6F8 = alHeapAlloc(audio_heap, 4, 4 + (num_files * sizeof(ALSeqData)));
    Rom_DmaRead(D_800FC6F4->seqArray[1].offset, D_800FC6F8, 4 + (num_files * sizeof(ALSeqData)));
    alSeqFileNew(D_800FC6F8, D_800FC6F4->seqArray[1].offset);

    D_800FC6A4 = alHeapAlloc(audio_heap, 1, 0x1388);
    D_800FC6A8 = alHeapAlloc(audio_heap, 1, 0xBB8);

    // load third archive from the main list, archive is at 0x1978820
    Rom_DmaRead(gAudioRegions[3], sp78, 4 + 4);
    num_files = sp78->seq.seqCount;

    D_800FC700 = alHeapAlloc(audio_heap, 1, 4 + (num_files * sizeof(ALSeqData)));
    Rom_DmaRead(gAudioRegions[3], D_800FC700, 4 + (num_files * sizeof(ALSeqData)));
    alSeqFileNew(D_800FC700, gAudioRegions[3]);

    // load fourth archive from the main list, archive is at 0x197C1E0
    Rom_DmaRead(gAudioRegions[4], sp78, 4 + 4);
    num_files = sp78->seq.seqCount;

    D_800FC704 = alHeapAlloc(audio_heap, 1, 4 + (num_files * sizeof(ALSeqData)));
    Rom_DmaRead(gAudioRegions[4], D_800FC704, 4 + (num_files * sizeof(ALSeqData)));
    alSeqFileNew(D_800FC704, gAudioRegions[4]);

    D_800FC6C0[0] = alHeapAlloc(audio_heap, 1, 0x44C);
    D_800FC6C0[1] = alHeapAlloc(audio_heap, 1, 0x44C);
    D_800FC6C0[2] = alHeapAlloc(audio_heap, 1, 0x44C);

    // load the fourth archive's first seq, at 0x197C1FC
    Rom_DmaRead(D_800FC704->seqArray[0].offset, sp78, 4 + 4);
    num_files = sp78->seq.seqCount;
    D_800FC708 = alHeapAlloc(audio_heap, 1, 4 + (num_files * sizeof(ALSeqData)));
    Rom_DmaRead(D_800FC704->seqArray[0].offset, D_800FC708, 4 + (num_files * sizeof(ALSeqData)));
    alSeqFileNew(D_800FC708, D_800FC704->seqArray[0].offset);

    D_80079364 = 0;

    // load the fourth archive's third seq at 0x1A2AD20
    Rom_DmaRead(D_800FC704->seqArray[2].offset, sp78, 4 + 4);
    num_files = sp78->seq.seqCount;
    D_800FC714 = alHeapAlloc(audio_heap, 1, 4 + (num_files * sizeof(ALSeqData)));
    Rom_DmaRead(D_800FC704->seqArray[2].offset, D_800FC714, 4 + (num_files * sizeof(ALSeqData)));
    alSeqFileNew(D_800FC714, D_800FC704->seqArray[2].offset);

    D_800FC6D8 = alHeapAlloc(audio_heap, 0xB80, 2);

    // load the 5th main archive at 0x1FBA260
    D_800FC6D4 = alHeapAlloc(audio_heap, 0x1388, 1);
    Rom_DmaRead(gAudioRegions[5], D_800FC6D4, gAudioRegions[6] - gAudioRegions[5]);

    // load the 6th main archive at 0x1FBB220
    D_800FC6CC = alHeapAlloc(audio_heap, 0x100, 1);
    Rom_DmaRead(gAudioRegions[6], D_800FC6CC, 0x100);

    for (i = 0; i < 32; i++) {
        D_800FC6CC[i].unk_00 += gAudioRegions[6];
    }

#if defined(VERSION_PAL)
    // PAL shares this buffer with the move sound bank instead of allocating its own, so whoever
    // overwrites it marks the move sound bank stale (see Effect_TriggerMoveVisual).
    D_800FC6D0 = D_800FC690;
#else
    D_800FC6D0 = alHeapAlloc(audio_heap, 0x2328, 1);
#endif

    Audio_SetChannelVolume(2, 2);
    Audio_SetChannelVolume(1, 3);

    D_800FC820 = 0;

    return audio_heap->cur - audio_heap->base;
}
