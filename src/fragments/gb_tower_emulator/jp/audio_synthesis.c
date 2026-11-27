/* JP-only TU, ported from pokestadium0 (Stadium Zero, fragment000_80C00). */
/* Audio driver, matching OoT src/audio/internal/synthesis.c in function order. The driver is
 * Nintendo's "Nas" library (original names Nas_*); this revision sits between SM64 Shindou and
 * OoT. pokestadium0's docs/annotation/audio-driver.md has the per-function mapping. */

#include "global.h"
#include "gb_tower_emulator_jp.h"

typedef struct ReverbItem81107E70 {
    /* 0x00 */ char unk00[0x4];
    /* 0x04 */ s32 toDownsampleLeft; // @unverified: same offset/use as OoT's ReverbRingBufferItem.toDownsampleLeft (DMA'd to DMEM when not downsampling)
    /* 0x08 */ char unk08[0x6];
    /* 0x0E */ u16 startPosLow; // @unverified: low half of ReverbRingBufferItem.startPos (s32 at 0x0C), passed as the ring buffer offset
    /* 0x10 */ s16 lengthA; // @unverified: same offset/use as OoT's ReverbRingBufferItem.lengthA
    /* 0x12 */ s16 lengthB; // @unverified: same offset/use as OoT's ReverbRingBufferItem.lengthB (part after the ring buffer wraps)
} ReverbItem81107E70; // size = 0x14

typedef struct Reverb81107E70 {
    /* 0x000 */ u8 resampleFlags; // @unverified: same offset/use as OoT's SynthesisReverb.resampleFlags (cleared after the first resample)
    /* 0x001 */ u8 useReverb; // @unverified: same offset/use as OoT's SynthesisReverb.useReverb (AudioSynth_Update skips the reverb when 0)
    /* 0x002 */ u8 framesToIgnore; // @unverified: same offset/use as OoT's SynthesisReverb.framesToIgnore (counted down each frame)
    /* 0x003 */ u8 curFrame; // @unverified: same offset/use as OoT's SynthesisReverb.curFrame (toggled 0/1 each frame)
    /* 0x004 */ u8 downsampleRate; // @unverified: same offset/use as OoT's SynthesisReverb.downsampleRate
    /* 0x005 */ char unk005[0x1B];
    /* 0x020 */ s32 leftRingBuf; // @unverified: same offset/use as OoT's SynthesisReverb.leftRingBuf
    /* 0x024 */ s32 rightRingBuf; // @unverified: same offset/use as OoT's SynthesisReverb.rightRingBuf
    /* 0x028 */ char unk028[0x10];
    /* 0x038 */ ReverbItem81107E70 items[2][5]; // @unverified: same offset/use as OoT's SynthesisReverb.items
    /* 0x100 */ char unk100[0x58];
} Reverb81107E70; // size = 0x158

extern Reverb81107E70 D_812875B0[];

typedef struct AudioCmd {
    /* 0x00 */ u32 w0;
    /* 0x04 */ u32 w1;
} AudioCmd; // size = 0x8

extern NoteSampleState* D_81287728;

void osInvalDCache(void*, s32);

/**
 * Sets up the next reverb ring buffer item for update `update_index` of reverb `reverb_index`.
 * Corresponds to OoT AudioSynth_InitNextRingBuf (original name Nas_CpuFX). Same downsampling loops
 * and ring-buffer wrap split. Difference: Zero downsamples only when downsampleRate >= 3 (`slti 3`
 * in the ROM); OoT uses >= 2 and SM64 Shindou's prepare_reverb_ring_buffer uses != 1.
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void AudioSynth_InitNextRingBuf(s32 chunk_len, s32 update_index, s32 reverb_index) {
    ReverbRingBufferItem* item;
    s32 src_pos;
    s32 dst_pos;
    s32 n_samples;
    s32 excessive_samples;
    s32 pad[3]; // FAKE
    SynthesisReverb* reverb;

    reverb = (SynthesisReverb*)&D_812875B0[reverb_index];
    if (reverb->downsampleRate >= 3 && reverb->framesToIgnore == 0) {
        item = &reverb->items[reverb->curFrame][update_index];
        osInvalDCache(item->toDownsampleLeft, 0x340);
        for (src_pos = 0, dst_pos = 0; dst_pos < item->lengthA / 2; src_pos += reverb->downsampleRate, dst_pos++) {
            reverb->leftRingBuf[item->startPos + dst_pos] = item->toDownsampleLeft[src_pos];
            reverb->rightRingBuf[item->startPos + dst_pos] = item->toDownsampleRight[src_pos];
        }
        for (dst_pos = 0; dst_pos < item->lengthB / 2; src_pos += reverb->downsampleRate, dst_pos++) {
            reverb->leftRingBuf[dst_pos] = item->toDownsampleLeft[src_pos];
            reverb->rightRingBuf[dst_pos] = item->toDownsampleRight[src_pos];
        }
    }
    item = &reverb->items[reverb->curFrame][update_index];
    n_samples = chunk_len / reverb->downsampleRate;
    excessive_samples = (n_samples + reverb->nextRingBufPos) - reverb->bufSizePerChan;
    if (excessive_samples < 0) {
        item->lengthA = n_samples * 2;
        item->lengthB = 0;
        item->startPos = reverb->nextRingBufPos;
        reverb->nextRingBufPos += n_samples;
    } else {
        item->lengthA = (n_samples - excessive_samples) * 2;
        item->lengthB = excessive_samples * 2;
        item->startPos = reverb->nextRingBufPos;
        reverb->nextRingBufPos = excessive_samples;
    }
    item->numSamplesAfterDownsampling = n_samples;
    item->chunkLen = chunk_len;
}

#define SHL_8110(v, s, w) ((u32)(((u32)(v) & ((1 << (w)) - 1)) << (s)))
#define aLoadBuf_8110(pkt, c, dmem, count, addr)                                                  \
    {                                                                                              \
        AudioCmd* _a = (AudioCmd*)(pkt);                                                           \
        _a->w0 = (SHL_8110(c, 24, 8) | SHL_8110((count) >> 4, 16, 8) | SHL_8110(dmem, 0, 16));     \
        _a->w1 = (u32)(addr);                                                                      \
    }

/**
 * Loads part of a reverb ring buffer into DMEM (both channels).
 * Corresponds to OoT AudioSynth_LoadRingBufferPart (original name __LoadAuxBuf). Takes a reverb
 * index, as SM64 Shindou's synthesis_load_reverb_ring_buffer does; OoT passes the reverb pointer.
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void* AudioSynth_LoadRingBufferPart(AudioCmd* cmd, u16 dmem, u16 offset, s32 count, s32 channel) {
    aLoadBuf_8110(cmd++, 0x14, dmem, count, D_812875B0[channel].leftRingBuf + offset * 2 + 0x80000000);
    aLoadBuf_8110(cmd++, 0x14, dmem + 0x1A0, count, D_812875B0[channel].rightRingBuf + offset * 2 + 0x80000000);
    return cmd;
}

/**
 * Saves part of DMEM back into a reverb ring buffer (both channels).
 * Corresponds to OoT AudioSynth_SaveRingBufferPart (original name __SaveAuxBuf). Takes a reverb
 * index, as SM64 Shindou's synthesis_save_reverb_ring_buffer does; OoT passes the reverb pointer.
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void* AudioSynth_SaveRingBufferPart(AudioCmd* cmd, u16 dmem, u16 offset, s32 count, s32 channel) {
    aLoadBuf_8110(cmd++, 0x15, dmem, count, D_812875B0[channel].leftRingBuf + offset * 2 + 0x80000000);
    aLoadBuf_8110(cmd++, 0x15, dmem + 0x1A0, count, D_812875B0[channel].rightRingBuf + offset * 2 + 0x80000000);
    return cmd;
}

extern AudioBufferParameters sGbAudioOutputConfig;
extern s32 D_81288D88;

/**
 * Marks voice `voice_index` as not dirty in the later updates of this frame.
 * Corresponds to OoT func_800DB2C0 (original name __Nas_WaveTerminateProcess). OoT leaves it
 * unnamed; the name is MM's.
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void AudioSynth_DisableSampleStates(s32 update_index, s32 voice_index) {
    s32 i;

    for (i = update_index + 1; i < sGbAudioOutputConfig.ticksPerUpdate; i++) {
        if (!(&(D_81287728 + D_81288D88 * i)[voice_index])->needsInit) {
            (&(D_81287728 + D_81288D88 * i)[voice_index])->enabled = 0;
        } else {
            break;
        }
    }
}

extern Note* D_81287B04;
extern s32 D_81288D88;
/**
 * Copies each note's sample state into the per-update state.
 * Corresponds to OoT func_800DB03C (original name __Nas_PushDrvReg). OoT leaves it unnamed; the
 * name is MM's. Like OoT it also clears the harmonic index byte (unk06), which SM64 Shindou's
 * synthesis_load_note_subs_eu does not.
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void AudioSynth_SyncSampleStates(u32 arg0) {
    s32 i;
    NoteSampleState* src;
    NoteSampleState* dst;

    for (i = 0; i < D_81288D88; i++) {
        src = &D_81287B04[i].sampleState;
        dst = &D_81287728[D_81288D88 * arg0 + i];
        if (src->enabled) {
            *dst = *src;
            src->needsInit = 0;
        } else {
            dst->enabled = 0;
        }
        src->harmonicIndexCurAndPrev = 0;
    }
}

extern s8 D_8128771B;
void AudioSynth_InitNextRingBuf(s32 chunk_len, s32 update_index, s32 reverb_index);
AudioCmd* AudioSynth_DoOneAudioUpdate(s32* ai_buf, s32 ai_buf_len, AudioCmd* cmd, s32 update_index);
void AudioSeq_ProcessSequences(s32 arg0);
/**
 * Builds the audio command list for one frame of `arg3` samples.
 * Corresponds to OoT AudioSynth_Update (original name Nas_smzAudioFrame). Same chunk split between
 * samplesPerTickMax, samplesPerTickMin and samplesPerTick.
 * No caller (2026-11-13): nothing in the built ROM jumps to it or holds its address (no jal or j,
 * no pointer word, no lui/addiu pair in any fragment), so play cannot reach it.
 * @unverified Name and comment guessed from the code.
 */
AudioCmd* AudioSynth_Update(AudioCmd* arg0, s32* arg1, s32* arg2, s32 arg3) {
    AudioCmd* cmd = arg0;
    s32 i;
    s32 j;
    s32 chunk;
    s32 q;
    s32* buf;

    for (i = sGbAudioOutputConfig.ticksPerUpdate; i > 0; i--) {
        AudioSeq_ProcessSequences(i - 1);
        AudioSynth_SyncSampleStates(sGbAudioOutputConfig.ticksPerUpdate - i);
    }
    buf = arg2;
    for (i = sGbAudioOutputConfig.ticksPerUpdate; i > 0; i--) {
        if (i == 1) {
            chunk = arg3;
        } else {
            q = arg3 / i;
            if (q >= sGbAudioOutputConfig.samplesPerTickMax) {
                chunk = sGbAudioOutputConfig.samplesPerTickMax;
            } else if (sGbAudioOutputConfig.samplesPerTickMin >= q) {
                chunk = sGbAudioOutputConfig.samplesPerTickMin;
            } else {
                chunk = sGbAudioOutputConfig.samplesPerTick;
            }
        }
        for (j = 0; j < D_8128771B; j++) {
            if (D_812875B0[j].useReverb != 0) {
                AudioSynth_InitNextRingBuf(chunk, sGbAudioOutputConfig.ticksPerUpdate - i, j);
            }
        }
        cmd = AudioSynth_DoOneAudioUpdate(buf, chunk, cmd, sGbAudioOutputConfig.ticksPerUpdate - i);
        arg3 -= chunk;
        buf += chunk;
    }
    for (j = 0; j < D_8128771B; j++) {
        if (D_812875B0[j].framesToIgnore != 0) {
            D_812875B0[j].framesToIgnore--;
        }
        D_812875B0[j].curFrame ^= 1;
    }
    *arg1 = cmd - arg0;
    return cmd;
}

typedef struct Reverb81107AEC {
    /* 0x000 */ u8 resampleFlags;
    /* 0x001 */ u8 unk01;
    /* 0x002 */ u8 framesToIgnore; // @unverified: same offset/use as OoT's SynthesisReverb.framesToIgnore
    /* 0x003 */ u8 curFrame;
    /* 0x004 */ u8 downsampleRate;
    /* 0x005 */ char unk05[0x5];
    /* 0x00A */ u16 volume; // @unverified: same offset/use as OoT's SynthesisReverb.volume (aMix gain of the wet signal)
    /* 0x00C */ u16 decayRatio; // @unverified: same offset/use as OoT's SynthesisReverb.decayRatio (aMix gain + 0x8000)
    /* 0x00E */ u16 resampleRate; // @unverified: aResample pitch; AudioHeap_Init sets it to 0x8000 / downsampleRate (OoT's SynthesisReverb.unk_0E)
    /* 0x010 */ u16 leakRtl; // @unverified: same offset/use as OoT's SynthesisReverb.leakRtl
    /* 0x012 */ u16 leakLtr; // @unverified: same offset/use as OoT's SynthesisReverb.leakLtr
    /* 0x014 */ char unk14[0x14];
    /* 0x028 */ s32 resampleStateLeft; // @unverified: aResample state of the left channel (OoT's SynthesisReverb.unk_30)
    /* 0x02C */ s32 resampleStateRight; // @unverified: aResample state of the right channel (OoT's SynthesisReverb.unk_34)
    /* 0x030 */ char unk30[0x8];
    /* 0x038 */ ReverbRingBufferItem items[2][5];
    /* 0x100 */ char unk100[0x58];
} Reverb81107AEC; // size = 0x158
#define ACMD_81107AEC(pkt, a, b)            \
    {                                       \
        AudioCmd* _a = (AudioCmd*)(pkt);    \
        _a->w0 = (a);                       \
        _a->w1 = (b);                       \
    }
/**
 * Loads a reverb's ring buffer samples for this update.
 * Corresponds to OoT AudioSynth_LoadReverbSamples (original name Nas_LoadAuxBuffer). Zero does in
 * one function what OoT splits into AudioSynth_LoadRingBuffer1 (Nas_LoadAuxBuffer1), func_800DB4E4
 * (Nas_LoadAuxBufferC, the resampling path) and AudioSynth_LeakReverb (Nas_CrossMix). SM64 Shindou
 * has the single function too (synthesis_resample_and_mix_reverb). Difference from Shindou: the wet
 * mix is two aMix commands (reverbGain, then wetGain + 0x8000) instead of aAddMixer plus one aMix.
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void* AudioSynth_LoadReverbSamples(AudioCmd* cmd, s32 buf_len, s16 reverb_index, s16 update_index) {
    ReverbRingBufferItem* item;
    s16 start_pad;
    s16 padded_length_a;

    item = &((Reverb81107AEC*)&D_812875B0[reverb_index])->items[((Reverb81107AEC*)&D_812875B0[reverb_index])->curFrame][update_index];
    if (((Reverb81107AEC*)&D_812875B0[reverb_index])->downsampleRate == 1) {
        cmd = AudioSynth_LoadRingBufferPart(cmd, 0xC80, item->startPos, item->lengthA, reverb_index);
        if (item->lengthB != 0) {
            cmd = AudioSynth_LoadRingBufferPart(cmd, item->lengthA + 0xC80, 0, item->lengthB, reverb_index);
        }
        ACMD_81107AEC(cmd++, SHL_8110(0xC, 24, 8) | SHL_8110(0x34, 16, 8) | SHL_8110(((Reverb81107AEC*)&D_812875B0[reverb_index])->volume, 0, 16), 0x0C800940);
        ACMD_81107AEC(cmd++, SHL_8110(0xC, 24, 8) | SHL_8110(0x34, 16, 8) | SHL_8110(((Reverb81107AEC*)&D_812875B0[reverb_index])->decayRatio + 0x8000, 0, 16), 0x0C800C80);
    } else {
        start_pad = (item->startPos & 7) * 2;
        padded_length_a = (start_pad + item->lengthA + 0xF) & ~0xF;
        cmd = AudioSynth_LoadRingBufferPart(cmd, 0x3E0, item->startPos - start_pad / 2, 0x1A0, reverb_index);
        if (item->lengthB != 0) {
            cmd = AudioSynth_LoadRingBufferPart(cmd, padded_length_a + 0x3E0, 0, 0x1A0 - padded_length_a, reverb_index);
        }
        ACMD_81107AEC(cmd++, SHL_8110(8, 24, 8) | SHL_8110(start_pad + 0x3E0, 0, 16), SHL_8110(0xC80, 16, 16) | SHL_8110(buf_len * 2, 0, 16));
        ACMD_81107AEC(cmd++, SHL_8110(5, 24, 8) | SHL_8110(((Reverb81107AEC*)&D_812875B0[reverb_index])->resampleRate, 0, 16) | SHL_8110(((Reverb81107AEC*)&D_812875B0[reverb_index])->resampleFlags, 16, 8), ((Reverb81107AEC*)&D_812875B0[reverb_index])->resampleStateLeft + 0x80000000);
        ACMD_81107AEC(cmd++, SHL_8110(8, 24, 8) | SHL_8110(start_pad + 0x580, 0, 16), SHL_8110(0xE20, 16, 16) | SHL_8110(buf_len * 2, 0, 16));
        ACMD_81107AEC(cmd++, SHL_8110(5, 24, 8) | SHL_8110(((Reverb81107AEC*)&D_812875B0[reverb_index])->resampleRate, 0, 16) | SHL_8110(((Reverb81107AEC*)&D_812875B0[reverb_index])->resampleFlags, 16, 8), ((Reverb81107AEC*)&D_812875B0[reverb_index])->resampleStateRight + 0x80000000);
        ACMD_81107AEC(cmd++, SHL_8110(0xC, 24, 8) | SHL_8110(0x34, 16, 8) | SHL_8110(((Reverb81107AEC*)&D_812875B0[reverb_index])->volume, 0, 16), 0x0C800940);
        ACMD_81107AEC(cmd++, SHL_8110(0xC, 24, 8) | SHL_8110(0x34, 16, 8) | SHL_8110(((Reverb81107AEC*)&D_812875B0[reverb_index])->decayRatio + 0x8000, 0, 16), 0x0C800C80);
    }
    if ((((Reverb81107AEC*)&D_812875B0[reverb_index])->leakRtl != 0) || (((Reverb81107AEC*)&D_812875B0[reverb_index])->leakLtr != 0)) {
        ACMD_81107AEC(cmd++, SHL_8110(0xA, 24, 8) | SHL_8110(0xC80, 0, 16), SHL_8110(0x3E0, 16, 16) | SHL_8110(0x1A0, 0, 16));
        ACMD_81107AEC(cmd++, SHL_8110(0xC, 24, 8) | SHL_8110(0x1A, 16, 8) | SHL_8110(((Reverb81107AEC*)&D_812875B0[reverb_index])->leakRtl, 0, 16), 0x0E200C80);
        ACMD_81107AEC(cmd++, SHL_8110(0xC, 24, 8) | SHL_8110(0x1A, 16, 8) | SHL_8110(((Reverb81107AEC*)&D_812875B0[reverb_index])->leakLtr, 0, 16), 0x03E00E20);
    }
    return cmd;
}

void* AudioSynth_LoadRingBufferPart(AudioCmd* cmd, u16 dmem, u16 offset, s32 count, s32 channel);
extern Reverb81107E70 D_812875B0[];

/**
 * Loads this update's reverb ring buffer item (both parts when it wraps) into DMEM at 0x3E0.
 * Corresponds to OoT AudioSynth_LoadRingBuffer1AtTemp (original name Nas_LoadAux2nd). SM64 Shindou:
 * synthesis_load_reverb_samples.
 * @unverified Name follows the counterpart; not checked in play.
 */
void* AudioSynth_LoadRingBuffer1AtTemp(void* cmd, s16 channel, s16 slot) {
    ReverbItem81107E70* voice = &D_812875B0[channel].items[D_812875B0[channel].curFrame][slot];

    cmd = AudioSynth_LoadRingBufferPart(cmd, 0x3E0, voice->startPosLow, voice->lengthA, channel);
    if (voice->lengthB != 0) {
        cmd = AudioSynth_LoadRingBufferPart(cmd, voice->lengthA + 0x3E0, 0, voice->lengthB, channel);
    }
    return cmd;
}

void* AudioSynth_SaveRingBufferPart(AudioCmd* cmd, u16 dmem, u16 offset, s32 count, s32 channel);

/* Loads a voice's samples (or points the mixer at them in RAM) as one macro statement, like AUDIO_CMDS_811092F4. */
// FAKE: the do-while (0) wrapper in this macro is only there for register allocation.
#define AUDIO_LOAD_VOICE_81107F40(cmd, tmp, ch, voice, slot, channel)                                  \
    do {                                                                                              \
        if ((ch)->downsampleRate == 1) {                                                                        \
            cmd = AudioSynth_SaveRingBufferPart(cmd, 0xC80, (voice)->startPosLow, (voice)->lengthA, channel);                \
            if ((voice)->lengthB != 0) {                                                            \
                cmd = AudioSynth_SaveRingBufferPart(cmd, (voice)->lengthA + 0xC80, 0, (voice)->lengthB, channel);     \
            }                                                                                         \
        } else {                                                                                      \
            tmp = (cmd)++;                                                                            \
            tmp->w0 = 0x15340C80;                                                                     \
            tmp->w1 = (ch)->items[(ch)->curFrame][slot].toDownsampleLeft + 0x80000000;                               \
        }                                                                                             \
    } while (0)

/**
 * Saves the wet channels back to the ring buffer, or to the downsample buffer when the reverb is
 * downsampled, then clears the resample flags.
 * Corresponds to OoT AudioSynth_SaveReverbSamples (original name Nas_SaveAuxBuffer). SM64 Shindou:
 * synthesis_save_reverb_samples. Zero has no comb-filter branch (OoT's unk_18 path through
 * func_800DB680).
 * @unverified Name follows the counterpart; not checked in play.
 */
void* AudioSynth_SaveReverbSamples(AudioCmd* cmd, s16 channel, s16 slot) {
    ReverbItem81107E70* voice = &D_812875B0[channel].items[D_812875B0[channel].curFrame][slot];
    AudioCmd* c;

    AUDIO_LOAD_VOICE_81107F40(cmd, c, &D_812875B0[channel], voice, slot, channel);
    D_812875B0[channel].resampleFlags = 0;
    return cmd;
}

void* AudioSynth_SaveRingBufferPart(AudioCmd* cmd, u16 dmem, u16 offset, s32 count, s32 channel);

/**
 * Saves DMEM at 0x3E0 back into this update's reverb ring buffer item (both parts when it wraps).
 * Corresponds to OoT AudioSynth_SaveRingBuffer1AtTemp (original name Nas_SaveAux2nd). SM64 Shindou:
 * func_sh_802EDF24.
 * @unverified Name follows the counterpart; not checked in play.
 */
void* AudioSynth_SaveRingBuffer1AtTemp(void* cmd, s16 channel, s16 slot) {
    ReverbItem81107E70* voice = &D_812875B0[channel].items[D_812875B0[channel].curFrame][slot];

    cmd = AudioSynth_SaveRingBufferPart(cmd, 0x3E0, voice->startPosLow, voice->lengthA, channel);
    if (voice->lengthB != 0) {
        cmd = AudioSynth_SaveRingBufferPart(cmd, voice->lengthA + 0x3E0, 0, voice->lengthB, channel);
    }
    return cmd;
}

#define ZSHIFTL_814C(v, s, w) ((u32)(((u32)(v) & ((0x01 << (w)) - 1)) << (s)))
typedef struct Reverb8110814C {
    /* 0x000 */ u8 resampleFlags;
    /* 0x001 */ u8 useReverb;
    /* 0x002 */ char unk002[0x3];
    /* 0x005 */ s8 mixReverbIndex; // @unverified: from the code: index of another reverb whose ring buffer is mixed in when != -1 (OoT's SynthesisReverb.unk_05)
    /* 0x006 */ char unk006[0x2];
    /* 0x008 */ u16 mixReverbStrength; // @unverified: from the code: aMix gain for that mixed-in reverb (OoT's SynthesisReverb.unk_08)
    /* 0x00A */ char unk00A[0xF6];
    /* 0x100 */ s16* filterLeft;
    /* 0x104 */ s16* filterRight;
    /* 0x108 */ s16* filterLeftState;
    /* 0x10C */ s16* filterRightState;
    /* 0x110 */ char unk110[0x48];
} Reverb8110814C; // size = 0x158

extern s8 D_8128771A;
void* AudioSynth_LoadReverbSamples(AudioCmd* cmd, s32 buf_len, s16 reverb_index, s16 update_index);
void* AudioSynth_LoadRingBuffer1AtTemp(void* cmd, s16 channel, s16 slot);
void* AudioSynth_SaveReverbSamples(AudioCmd* cmd, s16 channel, s16 slot);
void* AudioSynth_SaveRingBuffer1AtTemp(void* cmd, s16 channel, s16 slot);
struct ZNoteSubEu;
AudioCmd* AudioSynth_ProcessSample(s32 note_index, struct ZNoteSubEu* note_sub_eu, struct NoteSynthesisState* synthesis_state, s16* ai_buf, s32 buf_len, AudioCmd* cmd, s32 update_index);

/**
 * Builds the commands for one audio update: reverbs, every note, then the mix into the AI buffer.
 * Corresponds to OoT AudioSynth_DoOneAudioUpdate (original name Nas_DriveRsp). OoT adds the reverb
 * filter, second ring buffer and leak calls; Zero's leak is inside AudioSynth_LoadReverbSamples.
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
AudioCmd* AudioSynth_DoOneAudioUpdate(s32* ai_buf, s32 ai_buf_len, AudioCmd* cmd, s32 update_index) {
    u8 sp94[60];
    NoteSampleState* note_sub_eu;
    s16 count;
    s16 reverb_index;
    s32 i;

    count = 0;
    if (D_8128771B == 0) {
        for (i = 0; i < D_81288D88; i++) {
            if (((NoteSampleState*)D_81287728)[D_81288D88 * update_index + i].enabled) {
                sp94[count++] = i;
            }
        }
    } else {
        for (reverb_index = 0; reverb_index < D_8128771B; reverb_index++) {
            for (i = 0; i < D_81288D88; i++) {
                note_sub_eu = (D_81288D88 * update_index + i) + (NoteSampleState*)D_81287728;
                if (count && count && count) {} // FAKE: block boundary that stops as1 propagating the note_sub_eu copy
                if (cmd) {} // karodo
                if (note_sub_eu->enabled && note_sub_eu->reverbIndex == reverb_index) {
                    sp94[count++] = i;
                }
            }
        }
        for (i = 0; i < D_81288D88; i++) {
            note_sub_eu = (D_81288D88 * update_index + i) + (NoteSampleState*)D_81287728;
            if (count && count && count) {} // FAKE: block boundary that stops as1 propagating the note_sub_eu copy
            if (note_sub_eu->enabled && note_sub_eu->reverbIndex >= D_8128771B) {
                sp94[count++] = i;
            }
        }
    }

    { AudioCmd* _a = cmd++; _a->w0 = ZSHIFTL_814C(2, 24, 8) | ZSHIFTL_814C(0x940, 0, 24); _a->w1 = 0x340; }

    i = 0;
    for (reverb_index = 0; reverb_index < D_8128771B; reverb_index++) {
        D_8128771A = ((Reverb8110814C*)&D_812875B0[reverb_index])->useReverb;
        if (D_8128771A) {
            cmd = AudioSynth_LoadReverbSamples(cmd, ai_buf_len, reverb_index, update_index);
            if (reverb_index) {} // karodo
        }
        while (i < count) {
            if (((NoteSampleState*)D_81287728)[sp94[i] + update_index * D_81288D88].reverbIndex == reverb_index) {
                cmd = AudioSynth_ProcessSample(sp94[i], (struct ZNoteSubEu*)&((NoteSampleState*)D_81287728)[sp94[i] + update_index * D_81288D88],
                                    (struct NoteSynthesisState*)&((Note*)D_81287B04)[sp94[i]].synthesisState, (s16*)ai_buf, ai_buf_len, cmd, update_index);
            } else {
                break;
            }
            i++;
        }
        if (((Reverb8110814C*)&D_812875B0[reverb_index])->useReverb) {
            if (((Reverb8110814C*)&D_812875B0[reverb_index])->filterLeft != NULL) {
                { AudioCmd* _a = cmd++; _a->w0 = ZSHIFTL_814C(0x1B, 24, 8) | ZSHIFTL_814C(2, 16, 8) | ZSHIFTL_814C(ai_buf_len * 2, 0, 16); _a->w1 = (u32)((Reverb8110814C*)&D_812875B0[reverb_index])->filterLeft; }
                { AudioCmd* _a = cmd++; _a->w0 = ZSHIFTL_814C(0x1B, 24, 8) | ZSHIFTL_814C(((Reverb8110814C*)&D_812875B0[reverb_index])->resampleFlags, 16, 8) | ZSHIFTL_814C(0xC80, 0, 16); _a->w1 = (u32)((Reverb8110814C*)&D_812875B0[reverb_index])->filterLeftState; }
            }
            if (((Reverb8110814C*)&D_812875B0[reverb_index])->filterRight != NULL) {
                { AudioCmd* _a = cmd++; _a->w0 = ZSHIFTL_814C(0x1B, 24, 8) | ZSHIFTL_814C(2, 16, 8) | ZSHIFTL_814C(ai_buf_len * 2, 0, 16); _a->w1 = (u32)((Reverb8110814C*)&D_812875B0[reverb_index])->filterRight; }
                { AudioCmd* _a = cmd++; _a->w0 = ZSHIFTL_814C(0x1B, 24, 8) | ZSHIFTL_814C(((Reverb8110814C*)&D_812875B0[reverb_index])->resampleFlags, 16, 8) | ZSHIFTL_814C(0xE20, 0, 16); _a->w1 = (u32)((Reverb8110814C*)&D_812875B0[reverb_index])->filterRightState; }
            }
            cmd = AudioSynth_SaveReverbSamples(cmd, reverb_index, update_index);
            if (((Reverb8110814C*)&D_812875B0[reverb_index])->mixReverbIndex != -1) {
                if (((SynthesisReverb*)&D_812875B0[((Reverb8110814C*)&D_812875B0[reverb_index])->mixReverbIndex])->downsampleRate == 1) {
                    cmd = AudioSynth_LoadRingBuffer1AtTemp(cmd, ((Reverb8110814C*)&D_812875B0[reverb_index])->mixReverbIndex, update_index);
                    cmd->w0 = ZSHIFTL_814C(0xC, 24, 8) | ZSHIFTL_814C(0x34, 16, 8) | ZSHIFTL_814C(((Reverb8110814C*)&D_812875B0[reverb_index])->mixReverbStrength, 0, 16); cmd->w1 = ZSHIFTL_814C(0xC80, 16, 16) | ZSHIFTL_814C(0x3E0, 0, 16); cmd++;
                    cmd = AudioSynth_SaveRingBuffer1AtTemp(cmd, ((Reverb8110814C*)&D_812875B0[reverb_index])->mixReverbIndex, update_index);
                }
            }
        }
    }

    while (i < count) {
        cmd = AudioSynth_ProcessSample(sp94[i], (struct ZNoteSubEu*)&((NoteSampleState*)D_81287728)[sp94[i] + update_index * D_81288D88],
                            (struct NoteSynthesisState*)&((Note*)D_81287B04)[sp94[i]].synthesisState, (s16*)ai_buf, ai_buf_len, cmd, update_index);
        i++;
    }

    update_index = ai_buf_len * 2;
    { AudioCmd* _a = cmd++; _a->w0 = ZSHIFTL_814C(0xD, 24, 8) | ZSHIFTL_814C(update_index >> 4, 16, 8) | ZSHIFTL_814C(0x3C0, 0, 16); _a->w1 = ZSHIFTL_814C(0x940, 16, 16) | ZSHIFTL_814C(0xAE0, 0, 16); }
    { AudioCmd* _a = cmd++; _a->w0 = ZSHIFTL_814C(0x15, 24, 8) | ZSHIFTL_814C((update_index * 2) >> 4, 16, 8) | ZSHIFTL_814C(0x3C0, 0, 16); _a->w1 = (u32)ai_buf + 0x80000000; }
    return cmd;
}

typedef struct ZAdpcmLoop {
    /* 0x00 */ u32 start;
    /* 0x04 */ u32 end;
    /* 0x08 */ u32 count;
    /* 0x0C */ u32 pad;
    /* 0x10 */ s16 state[16];
} ZAdpcmLoop;

typedef struct ZAdpcmBook {
    /* 0x00 */ s32 order;
    /* 0x04 */ s32 npredictors;
    /* 0x08 */ s16 book[1];
} ZAdpcmBook;

typedef struct ZAudioBankSample {
    /* 0x00 */ u32 codec : 4;
    /* 0x00 */ u32 medium : 2;
    /* 0x00 */ u32 bit1 : 1;
    /* 0x00 */ u32 isPatched : 1;
    /* 0x00 */ u32 size : 24;
    /* 0x04 */ u8* sampleAddr;
    /* 0x08 */ ZAdpcmLoop* loop;
    /* 0x0C */ ZAdpcmBook* book;
} ZAudioBankSample;

typedef struct ZAudioBankSound {
    /* 0x00 */ ZAudioBankSample* sample;
} ZAudioBankSound;

typedef struct ZNoteSubEu {
    /* 0x00 */ u8 enabled : 1;
    /* 0x00 */ u8 needsInit : 1;
    /* 0x00 */ u8 finished : 1;
    /* 0x00 */ u8 envMixerNeedsInit : 1;
    /* 0x00 */ u8 stereoStrongRight : 1;
    /* 0x00 */ u8 stereoStrongLeft : 1;
    /* 0x00 */ u8 stereoHeadsetEffects : 1;
    /* 0x00 */ u8 usesHeadsetPanEffects : 1;
    /* 0x01 */ u8 reverbIndex : 3;
    /* 0x01 */ u8 bookOffset : 2;
    /* 0x01 */ u8 unk01 : 2;
    /* 0x01 */ u8 hasTwoAdpcmParts : 1;
    /* 0x02 */ u8 synthesisVolume;
    /* 0x03 */ u8 headsetPanRight;
    /* 0x04 */ u8 headsetPanLeft;
    /* 0x05 */ u8 reverbVol;
    /* 0x06 */ u8 harmonicIndexCurAndPrev; // @unverified: same offset as OoT's NoteSampleState.harmonicIndexCurAndPrev, cleared in AudioSynth_SyncSampleStates
    /* 0x07 */ u8 unk_07;
    /* 0x08 */ u16 targetVolLeft; // @unverified: read at 0x08 as the target left volume in AudioSynth_ProcessEnvelope (OoT offset)
    /* 0x0A */ u16 targetVolRight; // @unverified: read at 0x0A as the target right volume in AudioSynth_ProcessEnvelope (OoT offset)
    /* 0x0C */ u16 resamplingRateFixedPoint;
    /* 0x10 */ ZAudioBankSound* audioBankSound;
    /* 0x14 */ s16* filter;
} ZNoteSubEu;

typedef struct ZSynthesisBuffers {
    /* 0x00 */ s16 adpcmdecState[0x10];
    /* 0x20 */ s16 finalResampleState[0x10];
    /* 0x40 */ s16 mixEnvelopeState[0x10];
    /* 0x60 */ s16 filterBuffer[0x10];
} ZSynthesisBuffers;

typedef struct ZNote {
    /* 0x00 */ char unk00[0xB4];
    /* 0xB4 */ ZNoteSubEu noteSubEu;
} ZNote; // size = 0xCC

void* AudioSynth_FinalResample(AudioCmd* cmd, s8* arg1, s32 arg2, u16 count, u16 arg4, s32 arg5);
extern s16 D_81239EC8[];
u8* AudioLoad_DmaSampleData(u8*, s32, s32, u8*, u32);
AudioCmd* AudioSynth_ProcessEnvelope(AudioCmd* cmd, ZNoteSubEu* note_sub_eu, NoteSynthesisState* synthesis_state, s32 ai_buf_len, u16 in_buf, s32 headset_pan_settings, s32 flags);

#define ZALIGN(val, amnt) (((val) + (1 << amnt) - 1) & ~((1 << amnt) - 1))
#define ZSHIFTL(v, s, w) ((u32)(((u32)(v) & ((0x01 << (w)) - 1)) << (s)))
#define ZV2P(addr) ((u8*)(addr) - 0x80000000U)
#define ZCMD(pkt, a, b)            \
    {                              \
        AudioCmd* _a = (AudioCmd*)(pkt);      \
        _a->w0 = (a);              \
        _a->w1 = (u32)(b);         \
    }
#define zLoadBuffer(pkt, s, d, c) ZCMD(pkt, ZSHIFTL(0x14, 24, 8) | ZSHIFTL((c) >> 4, 16, 8) | ZSHIFTL(d, 0, 16), s)
#define zSetBuffer(pkt, f, i, o, c) ZCMD(pkt, ZSHIFTL(8, 24, 8) | ZSHIFTL(f, 16, 8) | ZSHIFTL(i, 0, 16), ZSHIFTL(o, 16, 16) | ZSHIFTL(c, 0, 16))
#define zADPCMdec(pkt, f, s) ZCMD(pkt, ZSHIFTL(1, 24, 8) | ZSHIFTL(f, 16, 8), s)
#define zS8Dec(pkt, f, s) ZCMD(pkt, ZSHIFTL(0x17, 24, 8) | ZSHIFTL(f, 16, 8), s)
#define zDMEMMove(pkt, i, o, c) ZCMD(pkt, ZSHIFTL(0x0A, 24, 8) | ZSHIFTL(i, 0, 24), ZSHIFTL(o, 16, 16) | ZSHIFTL(c, 0, 16))
#define zClearBuffer(pkt, d, c) ZCMD(pkt, ZSHIFTL(2, 24, 8) | ZSHIFTL(d, 0, 24), c)
#define zSetLoop(pkt, a) ZCMD(pkt, ZSHIFTL(0x0F, 24, 8), a)
#define zLoadADPCM(pkt, c, d) ZCMD(pkt, ZSHIFTL(0x0B, 24, 8) | ZSHIFTL(c, 0, 24), d)
#define zDownsampleHalf(pkt, c, i, o) ZCMD(pkt, ZSHIFTL(0x11, 24, 8) | ZSHIFTL(c, 0, 16), ZSHIFTL(i, 16, 16) | ZSHIFTL(o, 0, 16))
#define zUnknown25(pkt, op, f, c, o, i) ZCMD(pkt, ZSHIFTL(op, 24, 8) | ZSHIFTL(f, 16, 8) | ZSHIFTL(c, 0, 16), ZSHIFTL(o, 16, 16) | ZSHIFTL(i, 0, 16))
#define zHiLoGain(pkt, g, buflen, i) ZCMD(pkt, ZSHIFTL(0x18, 24, 8) | ZSHIFTL(g, 16, 8) | ZSHIFTL(buflen, 0, 16), ZSHIFTL(i, 16, 16))
#define zFilter(pkt, f, countOrBuf, addr) ZCMD(pkt, ZSHIFTL(0x1B, 24, 8) | ZSHIFTL(f, 16, 8) | ZSHIFTL(countOrBuf, 0, 16), addr)

/**
 * Builds the commands that decode, resample and mix one note's sample.
 * Corresponds to OoT AudioSynth_ProcessNote (original name Nas_SynthMain). The name is MM's. OoT
 * adds the Haas effect and synthetic-wave paths.
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
AudioCmd* AudioSynth_ProcessSample(s32 note_index, ZNoteSubEu* note_sub_eu, NoteSynthesisState* synthesis_state, s16* ai_buf,
                        s32 buf_len, AudioCmd* cmd, s32 update_index) {
    UNUSED s32 pad0[3]; // FAKE
    ZAudioBankSample* audio_book_sample;
    ZAdpcmLoop* loop_info;
    s16* cur_loaded_book;
    UNUSED u8 padEU[0x04]; // FAKE
    UNUSED u8 pad8[0x04]; // FAKE
    s32 note_finished;
    s32 restart;
    s32 flags;
    u16 resampling_rate_fixed_point;
    s32 n_samples_to_load;
    UNUSED u8 pad7[0x0c]; // FAKE
    s32 sp130;
    UNUSED s32 temp_buf_len; // FAKE
    UNUSED u32 pad9; // FAKE
    s32 t0;
    u8* sample_addr;
    s32 s6;
    s32 samples_len_adjusted;
    s32 n_adpcm_samples_processed;
    s32 end_pos;
    s32 n_samples_to_process;
    UNUSED s32 samples_len_int; // FAKE
    s32 s2;
    s32 left_right;
    s32 s5;
    u32 samples_len_fixed_point;
    s32 s3;
    s32 n_samples_in_this_iteration;
    u32 a3;
    u8* v0_2;
    s32 unk_s6;
    s32 s5_aligned;
    s32 sp88;
    s32 sp84;
    u32 temp;
    s32 n_parts;
    s32 cur_part;
    s32 aligned;
    UNUSED u32 padSH1; // FAKE
    s32 resampled_temp_len;
    u16 note_samples_dmem_addr_before_resampling;
    UNUSED u32 padSH2; // FAKE
    s16 addr;
    u8 synthesis_volume;
    u32 n_entries;
    ZNote* note;
    u16 sp56;

    cur_loaded_book = NULL;
    note = (ZNote*)&D_81287B04[note_index];
    if (note_sub_eu->audioBankSound != NULL) {
        flags = 0;
        if (note_sub_eu->needsInit == TRUE) {
            flags = 1;
            synthesis_state->restart = 0;
            synthesis_state->samplePosInt = 0;
            synthesis_state->samplePosFrac = 0;
            synthesis_state->curVolLeft = 0;
            synthesis_state->curVolRight = 0;
            synthesis_state->prevHeadsetPanRight = 0;
            synthesis_state->prevHeadsetPanLeft = 0;
            synthesis_state->reverbVol = note_sub_eu->reverbVol;
            synthesis_state->numParts = 0;
            note->noteSubEu.finished = 0;
        }

        resampling_rate_fixed_point = note_sub_eu->resamplingRateFixedPoint;
        n_parts = note_sub_eu->hasTwoAdpcmParts + 1;
        samples_len_fixed_point = (resampling_rate_fixed_point * buf_len * 2) + synthesis_state->samplePosFrac;
        n_samples_to_load = (samples_len_fixed_point >> 0x10);
        synthesis_state->samplePosFrac = samples_len_fixed_point & 0xFFFF;

        if ((synthesis_state->numParts == 1) && (n_parts == 2)) {
        } else if ((synthesis_state->numParts == 2) && (n_parts == 1)) {
        } else {
            sp56 = 0;
        }

        synthesis_state->numParts = n_parts;

        audio_book_sample = note_sub_eu->audioBankSound->sample;
        loop_info = audio_book_sample->loop;
        end_pos = loop_info->end;
        sample_addr = audio_book_sample->sampleAddr;
        resampled_temp_len = 0;
        for (cur_part = 0; cur_part < n_parts; cur_part++) {
            n_adpcm_samples_processed = 0;
            s5 = 0;

            if (n_parts == 1) {
                samples_len_adjusted = n_samples_to_load;
            } else if (n_samples_to_load & 1) {
                samples_len_adjusted = (n_samples_to_load & ~1) + (cur_part * 2);
            } else {
                samples_len_adjusted = n_samples_to_load;
            }

            if (audio_book_sample->codec == 0) {
                if (cur_loaded_book != audio_book_sample->book->book) {
                    switch (note_sub_eu->bookOffset) {
                        case 1:
                            cur_loaded_book = D_81239EC8 + 1;
                            break;
                        case 2:
                        case 3:
                        default:
                            cur_loaded_book = audio_book_sample->book->book;
                            break;
                    }
                    n_entries = 16 * audio_book_sample->book->order * audio_book_sample->book->npredictors;
                    zLoadADPCM(cmd++, n_entries, ZV2P(cur_loaded_book));
                }
            }

            while (n_adpcm_samples_processed != samples_len_adjusted) {
                s32 samples_remaining;
                s32 s0;

                note_finished = FALSE;
                restart = FALSE;
                s5_aligned = 0;
                s2 = synthesis_state->samplePosInt & 0xf;
                samples_remaining = end_pos - synthesis_state->samplePosInt;
                n_samples_to_process = samples_len_adjusted - n_adpcm_samples_processed;

                if (s2 == 0 && synthesis_state->restart == FALSE) {
                    s2 = 16;
                }
                s6 = 16 - s2;
                if (n_samples_to_process < samples_remaining) {
                    t0 = (n_samples_to_process - s6 + 0xf) / 16;
                    s0 = t0 * 16;
                    s3 = s6 + s0 - n_samples_to_process;
                } else {
                    s0 = samples_remaining - s6;
                    s3 = 0;
                    if (s0 <= 0) {
                        s0 = 0;
                        s6 = samples_remaining;
                    }
                    t0 = (s0 + 0xf) / 16;
                    if (loop_info->count != 0) {
                        restart = 1;
                    } else {
                        note_finished = 1;
                    }
                }
                switch (audio_book_sample->codec) {
                    case 0:
                        unk_s6 = 9;
                        sp88 = 0x10;
                        sp84 = 0;
                        break;
                    case 1:
                        unk_s6 = 0x10;
                        sp88 = 0x10;
                        sp84 = 0;
                        break;
                    case 2:
                        goto skip;
                }
                if (t0 != 0) {
                    temp = (synthesis_state->samplePosInt + sp88 - s2) / 16;
                    if (audio_book_sample->medium == 0) {
                        v0_2 = sp84 + (temp * unk_s6) + sample_addr;
                    } else {
                        v0_2 = AudioLoad_DmaSampleData(sp84 + (temp * unk_s6) + sample_addr, ZALIGN(t0 * unk_s6 + 16, 4), flags,
                                             &synthesis_state->sampleDmaIndex, audio_book_sample->medium);
                    }

                    a3 = ((u32)v0_2 & 0xf);
                    aligned = ZALIGN(t0 * unk_s6 + 16, 4);
                    addr = 0x940 - aligned;
                    zLoadBuffer(cmd++, ZV2P(v0_2 - a3), addr, ZALIGN(t0 * unk_s6 + 16, 4));
                } else {
                    s0 = 0;
                    a3 = 0;
                }
                if (synthesis_state->restart != FALSE) {
                    zSetLoop(cmd++, ZV2P(audio_book_sample->loop->state));
                    flags = 2;
                    synthesis_state->restart = FALSE;
                }
                n_samples_in_this_iteration = s0 + s6 - s3;
                if (n_adpcm_samples_processed == 0) {
                    sp130 = s2 * 2;
                } else {
                    s5_aligned = ZALIGN(s5 + 16, 4);
                }
                switch (audio_book_sample->codec) {
                    case 0:
                        aligned = ZALIGN(t0 * unk_s6 + 16, 4);
                        addr = 0x940 - aligned;
                        zSetBuffer(cmd++, 0, addr + a3, 0x580 + s5_aligned, s0 * 2);
                        zADPCMdec(cmd++, flags, ZV2P(synthesis_state->synthesisBuffers->adpcmdecState));
                        break;
                    case 1:
                        aligned = ZALIGN(t0 * unk_s6 + 16, 4);
                        addr = 0x940 - aligned;
                        zSetBuffer(cmd++, 0, addr + a3, 0x580 + s5_aligned, s0 * 2);
                        zS8Dec(cmd++, flags, ZV2P(synthesis_state->synthesisBuffers->adpcmdecState));
                        break;
                }
                if (n_adpcm_samples_processed != 0) {
                    zDMEMMove(cmd++, 0x580 + s5_aligned + (s2 * 2), 0x580 + s5, n_samples_in_this_iteration * 2);
                }
                n_adpcm_samples_processed += n_samples_in_this_iteration;
                switch (flags) {
                    case 1:
                        sp130 = 0x20;
                        s5 = (s0 + 0x10) * 2;
                        break;
                    case 2:
                        s5 = (n_samples_in_this_iteration) * 2 + s5;
                        break;
                    default:
                        if (s5 != 0) {
                            s5 = (n_samples_in_this_iteration) * 2 + s5;
                        } else {
                            s5 = (s2 + (n_samples_in_this_iteration)) * 2;
                        }
                        break;
                }
                flags = 0;
            skip:
                if (note_finished) {
                    zClearBuffer(cmd++, 0x580 + s5, (samples_len_adjusted - n_adpcm_samples_processed) * 2);
                    note_sub_eu->finished = 1;
                    note->noteSubEu.finished = 1;
                    AudioSynth_DisableSampleStates(update_index, note_index);
                    break;
                }
                if (restart != 0) {
                    synthesis_state->restart = TRUE;
                    synthesis_state->samplePosInt = loop_info->start;
                } else {
                    synthesis_state->samplePosInt += n_samples_to_process;
                }
            }

            switch (n_parts) {
                case 1:
                    note_samples_dmem_addr_before_resampling = 0x580 + sp130;
                    break;
                case 2:
                    switch (cur_part) {
                        case 0:
                            zDownsampleHalf(cmd++, ZALIGN(samples_len_adjusted / 2, 3), 0x580 + sp130, 0x3E0);
                            resampled_temp_len = samples_len_adjusted;
                            note_samples_dmem_addr_before_resampling = 0x3E0;
                            if (note_sub_eu->finished != FALSE) {
                                zClearBuffer(cmd++, note_samples_dmem_addr_before_resampling + resampled_temp_len, samples_len_adjusted + 0x10);
                            }
                            break;
                        case 1:
                            zDownsampleHalf(cmd++, ZALIGN(samples_len_adjusted / 2, 3), 0x580 + sp130, resampled_temp_len + 0x3E0);
                            break;
                    }
            }
            if (note_sub_eu->finished != FALSE) {
                break;
            }
        }

        flags = 0;
        if (note_sub_eu->needsInit == TRUE) {
            flags = 1;
            note_sub_eu->needsInit = FALSE;
        }
        cmd = AudioSynth_FinalResample(cmd, (s8*)synthesis_state, buf_len * 2, resampling_rate_fixed_point,
                            note_samples_dmem_addr_before_resampling, sp56 | flags);

        if (note_sub_eu->bookOffset == 3) {
            zUnknown25(cmd++, 0x19, 0, buf_len * 2, 0x3C0, 0x3C0);
        }
        if (note_sub_eu->bookOffset == 2) {
            zUnknown25(cmd++, 0x03, 0, buf_len * 2, 0x3C0, 0x3C0);
        }

        synthesis_volume = note_sub_eu->synthesisVolume;
        if (synthesis_volume != 0) {
            if (synthesis_volume < 0x10) {
                synthesis_volume = 0x10;
            }
            zHiLoGain(cmd++, synthesis_volume, (buf_len + 0x10) * 2, 0x3C0);
        }

        if (note_sub_eu->filter) {
            zFilter(cmd++, 2, buf_len * 2, note_sub_eu->filter);
            zFilter(cmd++, flags, 0x3C0, synthesis_state->synthesisBuffers->filterBuffer);
        }

        if (note_sub_eu->headsetPanRight != 0 || synthesis_state->prevHeadsetPanRight != 0) {
            left_right = 1;
        } else if (note_sub_eu->headsetPanLeft != 0 || synthesis_state->prevHeadsetPanLeft != 0) {
            left_right = 2;
        } else {
            left_right = 0;
        }
        cmd = AudioSynth_ProcessEnvelope(cmd, note_sub_eu, synthesis_state, buf_len, 0x3C0, left_right, flags);
    }
    return cmd;
}

#define SHIFTL_81109(v, s, w) ((u32)(((u32)(v) & ((0x01 << (w)) - 1)) << (s)))
/* Emits the two-or-one command sequence as a single macro statement, like the libultra audio macros. */
// FAKE: the do-while (0) wrapper in this macro is only there for register allocation.
#define AUDIO_CMDS_811092F4(cmd, arg1, arg2, count, arg4, arg5)                                          \
    do {                                                                                                 \
        if ((count) == 0) {                                                                              \
            AudioCmd* a = (cmd)++;                                                                       \
            a->w0 = SHIFTL_81109(2, 24, 8) | SHIFTL_81109(0x3C0, 0, 24);                                 \
            a->w1 = (u32)(arg2);                                                                         \
        } else {                                                                                         \
            {                                                                                            \
                AudioCmd* a = (cmd)++;                                                                   \
                a->w0 = SHIFTL_81109(8, 24, 8) | SHIFTL_81109(arg4, 0, 16);                              \
                a->w1 = SHIFTL_81109(0x3C0, 16, 16) | SHIFTL_81109(arg2, 0, 16);                         \
            }                                                                                            \
            {                                                                                            \
                AudioCmd* a = (cmd)++;                                                                   \
                a->w0 = SHIFTL_81109(5, 24, 8) | SHIFTL_81109(arg5, 16, 8) | SHIFTL_81109(count, 0, 16); \
                a->w1 = (u32)(*(s32*)((arg1) + 0xC) + 0x80000020);                                       \
            }                                                                                            \
        }                                                                                                \
    } while (0)
/**
 * Adds the final resample command for a note.
 * Corresponds to OoT AudioSynth_FinalResample (original name Nas_Synth_Resample).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void* AudioSynth_FinalResample(AudioCmd* cmd, s8* arg1, s32 arg2, u16 count, u16 arg4, s32 arg5) {
    AUDIO_CMDS_811092F4(cmd, arg1, arg2, count, arg4, arg5);
    return cmd;
}

/**
 * Adds the envelope (volume ramp, pan, reverb send) commands for a note.
 * Corresponds to OoT AudioSynth_ProcessEnvelope (original name Nas_Synth_Envelope).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
AudioCmd* AudioSynth_ProcessEnvelope(AudioCmd* cmd, ZNoteSubEu* note_sub_eu, NoteSynthesisState* synthesis_state, s32 ai_buf_len, u16 in_buf, s32 headset_pan_settings, s32 flags) {
    s32 reverb_vol;
    u16 cur_vol_left;
    u16 target_vol_left;
    UNUSED s16 pad2; // FAKE: stack padding
    u16 cur_vol_right;
    s16 ramp_left;
    s16 ramp_right;
    s16 source_reverb_vol;
    u16 target_vol_right;
    s16 ramp_reverb;

    cur_vol_left = ((NoteSynthesisState*)synthesis_state)->curVolLeft;
    cur_vol_right = ((NoteSynthesisState*)synthesis_state)->curVolRight;
    target_vol_left = ((NoteSampleState*)note_sub_eu)->targetVolLeft;
    target_vol_right = ((NoteSampleState*)note_sub_eu)->targetVolRight;
    target_vol_left <<= 4;
    target_vol_right <<= 4;

    if (target_vol_left != cur_vol_left) {
        ramp_left = (target_vol_left - cur_vol_left) / (ai_buf_len >> 3);
    } else {
        ramp_left = 0;
    }
    if (target_vol_right != cur_vol_right) {
        ramp_right = (target_vol_right - cur_vol_right) / (ai_buf_len >> 3);
    } else {
        ramp_right = 0;
    }

    source_reverb_vol = ((NoteSynthesisState*)synthesis_state)->reverbVol;
    reverb_vol = ((NoteSampleState*)note_sub_eu)->reverbVol;
    if (reverb_vol != source_reverb_vol) {
        reverb_vol = ((((NoteSampleState*)note_sub_eu)->reverbVol & 0x7F) - (source_reverb_vol & 0x7F)) << 9;
        ramp_reverb = reverb_vol / (ai_buf_len >> 3);
        ((NoteSynthesisState*)synthesis_state)->reverbVol = ((NoteSampleState*)note_sub_eu)->reverbVol;
    } else {
        ramp_reverb = 0;
    }

    ((NoteSynthesisState*)synthesis_state)->curVolLeft = cur_vol_left + (ramp_left * (ai_buf_len >> 3));
    ((NoteSynthesisState*)synthesis_state)->curVolRight = cur_vol_right + (ramp_right * (ai_buf_len >> 3));

    // each command is kept on one line (as a macro would expand), the line numbers affect codegen
    { AudioCmd* _a = cmd++; _a->w0 = (((u32) ((((u32) 0x12) & ((0x01 << 8) - 1)) << 24)) | ((u32) ((((u32) ((source_reverb_vol & 0x7F) * 2)) & ((0x01 << 8) - 1)) << 16))) | ((u32) ((((u32) ramp_reverb) & ((0x01 << 16) - 1)) << 0)); _a->w1 = ((u32) ((((u32) ramp_left) & ((0x01 << 16) - 1)) << 16)) | ((u32) ((((u32) ramp_right) & ((0x01 << 16) - 1)) << 0)); }
    { AudioCmd* _a = cmd++; _a->w0 = (u32) ((((u32) 0x16) & ((0x01 << 8) - 1)) << 24); _a->w1 = ((u32) ((((u32) cur_vol_left) & ((0x01 << 16) - 1)) << 16)) | ((u32) ((((u32) cur_vol_right) & ((0x01 << 16) - 1)) << 0)); }
    { AudioCmd* _a = cmd++; _a->w0 = ((((((u32) ((((u32) 0x13) & ((0x01 << 8) - 1)) << 24)) | ((u32) ((((u32) (in_buf >> 4)) & ((0x01 << 8) - 1)) << 16))) | ((u32) ((((u32) ai_buf_len) & ((0x01 << 8) - 1)) << 8))) | ((u32) ((((u32) ((source_reverb_vol & 0x80) >> 7)) & ((0x01 << 1) - 1)) << 2))) | ((u32) ((((u32) ((NoteSampleState*)note_sub_eu)->stereoStrongRight) & ((0x01 << 1) - 1)) << 1))) | ((u32) ((((u32) ((NoteSampleState*)note_sub_eu)->stereoStrongLeft) & ((0x01 << 1) - 1)) << 0)); _a->w1 = 0x94AEC8E2; }
    return cmd;
}
