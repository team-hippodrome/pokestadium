#include "global.h"

// Bump allocator for the GB Tower audio output buffers. Same shape and semantics as libultra's
// ALHeap (base, cur, len, count) and alHeapInit/alHeapDBAlloc, but with plain integer addresses.
typedef struct GbAudioHeap {
    /* 0x00 */ s32 base;  // 16-byte aligned start of the arena
    /* 0x04 */ s32 cur;   // next free address
    /* 0x08 */ s32 len;   // usable size in bytes
    /* 0x0C */ s32 count; // number of successful allocations
} GbAudioHeap; // size = 0x10

typedef struct unk_D_812346E0 {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
} unk_D_812346E0; // size = 0x8

typedef struct unk_D_81231450 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
} unk_D_81231450; // size = 0x8

typedef struct unk_D_81234650 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
} unk_D_81234650; // size = 0x8

// N64 audio interface (AI) output settings, filled in by GbAudio_InitOutput.
typedef struct GbAudioOutputConfig {
    /* 0x00 */ u8 pad0[2];
    /* 0x02 */ u16 frequency;          // requested AI sample rate (48000 Hz)
    /* 0x04 */ s16 actualFrequency;    // rate returned by osAiSetFrequency
    /* 0x06 */ s16 samplesPerFrame;    // frequency / sGbAudioFrameRate, rounded up to 16 (800)
    /* 0x08 */ s16 maxSamplesPerFrame; // samplesPerFrame + 16
    /* 0x0A */ s16 minSamplesPerFrame; // samplesPerFrame - 16
    /* 0x0C */ s16 unk_0C;
    /* 0x0E */ u8 pad0E[0x2A];
} GbAudioOutputConfig; // size = 0x38

// .data
extern s32 D_812286EC;
extern s32 D_812286F0;

// .bss
u64 sGbAudioHeapMemory[0x640]; // arena for sGbAudioHeap
u64 D_81234650[0x6];
GbAudioHeap sGbAudioHeap;
GbAudioOutputConfig sGbAudioOutputConfig;
volatile s32 sGbAudioFrameCount; // incremented by every GbAudio_UpdateFrame call
s32 D_812346CC;
s32 D_812346D0;
s32 sGbAudioBufferIndex; // sGbAudioBuffers slot rendered most recently (0..2)
f32 D_812346D8; // set to 16.713 (ms per frame?) by GbAudio_InitOutput, never read
s32 sGbAudioFrameRate; // 60; divides the AI rate into samplesPerFrame
s16* sGbAudioBuffers[3];      // triple-buffered interleaved stereo s16 output, 0xFC0 bytes each
s16 sGbAudioBufferLengths[3]; // stereo sample pairs rendered into each buffer
s16 D_812346F2; // only used as the end address of sGbAudioBufferLengths
u32 D_812346F4; // folded from osGetCount and rendered samples each frame, never read
s32 D_812346F8;
volatile s32 D_812346FC; // once nonzero, counts up per frame and stops AI submission after 16
s32 D_81234700;
s32 D_81234704;
s32 D_81234708;
s32 D_8123470C;

/**
 * Like alHeapDBAlloc: carves a 16-byte aligned block and zeroes it. Returns 0 when the arena is full.
 */
u32 GbApu_AllocZeroed(GbAudioHeap* heap, s32 size) {
    s32 alignedSize;
    u32 allocation;
    u32 cursor;
    alignedSize = ALIGN16(size);
    allocation = heap->cur;
    if ((heap->base + heap->len) >= (allocation + alignedSize)) {
        heap->cur += alignedSize;
        cursor = allocation;
        while (cursor < heap->cur) {
            *((u8*)cursor++) = 0;
        }
    } else {
        return 0;
    }
    heap->count++;
    return allocation;
}

/**
 * GbApu_AllocZeroed without the clear.
 */
s32 GbApu_Alloc(GbAudioHeap* heap, s32 size) {
    s32 allocation;
    u32 alignedSize;

    alignedSize = ALIGN16(size);
    allocation = heap->cur;
    if ((heap->base + heap->len) >= (heap->cur + alignedSize)) {
        heap->cur = alignedSize + allocation;
    } else {
        return 0;
    }
    heap->count++;
    return allocation;
}

/**
 * Like alHeapInit.
 */
void GbApu_InitAllocator(GbAudioHeap* heap, s32 base, s32 len) {
    heap->base = ALIGN16(base);
    heap->cur = ALIGN16(base);
    heap->len = (s32)(len - (base & 0xF));
    heap->count = 0;
}

/**
 * Clears the audio arena and state, sets the AI to 48 kHz, derives the per-frame sample budget
 * and allocates the three output buffers.
 */
void GbAudio_InitOutput(void) {
    u64* bssCursor;
    s16* lengthCursor;
    s32 sampleIndex;
    u32 remaining;
    s16* bufferCursor;
    s32 i;

    D_812346FC = 0;
    for (i = 0; i < D_812286EC / 8; i++) {
        sGbAudioHeapMemory[i] = 0;
    }
    bssCursor = D_81234650;
    remaining = (u32)((s32)&D_81234700 - (s32)D_81234650) >> 3;
    if ((s32)remaining >= 0) {
        do {
            remaining--;
            *bssCursor = 0;
            ++bssCursor;
        } while ((s32)remaining >= 0);
    }
    D_812346D8 = 16.713f;
    sGbAudioFrameRate = 0x3C;
    do {
    } while (0);
    sGbAudioOutputConfig.frequency = 0xBB80;
    sGbAudioOutputConfig.actualFrequency = osAiSetFrequency(sGbAudioOutputConfig.frequency);
    sGbAudioOutputConfig.samplesPerFrame = (s16)(((sGbAudioOutputConfig.frequency / sGbAudioFrameRate) + 0xF) & ~0xF);
    lengthCursor = (s16*)&sGbAudioBufferLengths;
    sGbAudioOutputConfig.minSamplesPerFrame = (s16)(sGbAudioOutputConfig.samplesPerFrame - 0x10);
    sGbAudioOutputConfig.maxSamplesPerFrame = (s16)(sGbAudioOutputConfig.samplesPerFrame + 0x10);
    do {
        *lengthCursor = 0xA0;
        lengthCursor++;
    } while (lengthCursor < (s16*)&D_812346F2);
    sGbAudioFrameCount = 0;
    sGbAudioBufferIndex = 0;
    GbApu_InitAllocator(&sGbAudioHeap, (s32)sGbAudioHeapMemory, D_812286F0);
    for (i = 0; i < 3; i++) {
        sGbAudioBuffers[i] = (s16*)GbApu_Alloc(&sGbAudioHeap, 0xFC0);
        bufferCursor = sGbAudioBuffers[i];
        for (sampleIndex = 0; sampleIndex != 0xFC0 / 2; sampleIndex++) {
            *bufferCursor = 0;
            bufferCursor++;
        }
    }
}
