#ifndef JPEG_DECODER_H
#define JPEG_DECODER_H

#include "global.h"
#include "jpegutils.h"

typedef union JpegTaskData {
    struct {
        /* 0x00 */ u32 address;
        /* 0x04 */ u32 mbCount;
        /* 0x08 */ u32 mode;
        /* 0x0C */ u32 qTableYPtr;
        /* 0x10 */ u32 qTableUPtr;
        /* 0x14 */ u32 qTableVPtr;
        /* 0x18 */ u32 mbSize; // This field is used by the microcode to save the macroblock size during a yield
    };
    long long int force_structure_alignment;
} JpegTaskData; // size = 0x20

typedef struct JpegWork {
    /* 0x000 */ JpegTaskData taskData;
    /* 0x020 */ u64 yieldData[0x200 / sizeof(u64)];
    /* 0x220 */ JpegQuantizationTable qTableY;
    /* 0x2A0 */ JpegQuantizationTable qTableU;
    /* 0x320 */ JpegQuantizationTable qTableV;
    /* 0x3A0 */ u8 codesLengths[0x110];
    /* 0x4B0 */ u16 codes[0x108];
    /* 0x6C0 */ u16 data[4][0x180];
} JpegWork; // size = 0x12C0

typedef struct JpegDecoder {
    /* 0x00 */ void* imageData;
    /* 0x04 */ u8 mode;
    /* 0x05 */ u8 unk_05;
    /* 0x08 */ JpegHuffmanTable* hTablePtrs[4];
    /* 0x18 */ u8 unk_18;
} JpegDecoder; // size = 0x1C

typedef struct JpegContext {
    /* 0x00 */ u8 dqtCount;
    /* 0x04 */ u8* dqtPtr[3];
    /* 0x10 */ u8 dhtCount;
    /* 0x14 */ u8* dhtPtr[4];
    /* 0x24 */ void* imageData;
    /* 0x28 */ u32 mode; // 0 if Y V0 is 1 and 2 if Y V0 is 2
    /* 0x30 */ OSScTask scTask;
    /* 0x98 */ OSMesgQueue mq;
    /* 0xB0 */ OSMesg msg;
    /* 0xB4 */ JpegWork* workBuf;
} JpegContext; // size = 0xB8

typedef struct JpegDecoderState {
    /* 0x00 */ u32 byteIdx;
    /* 0x04 */ u8 bitIdx;
    /* 0x05 */ u8 dontSkip;
    /* 0x08 */ u32 curWord;
    /* 0x0C */ s16 unk_0C;
    /* 0x0E */ s16 unk_0E;
    /* 0x10 */ s16 unk_10;
} JpegDecoderState; // size = 0x14

s32 Jpeg_DecodeMcus(JpegDecoder*, u16*, s32, u8, JpegDecoderState*);
s32 Jpeg_DecodeBlock(JpegHuffmanTable*, JpegHuffmanTable*, u16*, s16*);
s32 Jpeg_DecodeHuffmanSymbol(JpegHuffmanTable*, s16*, s8*);
u16 Jpeg_ReadBits(u8);

#endif // JPEG_DECODER_H
