/* Declarations for the JP-only GB Tower emulator and GB audio driver TUs in this directory, ported
 * from pokestadium0 src/fragments/audio_and_gb_emulator/. */
#ifndef FRAGMENT_AUDIO_AND_GB_EMULATOR_H
#define FRAGMENT_AUDIO_AND_GB_EMULATOR_H

#include "global.h"
#include "m2c_macros.h"

/* Struct types shared by the GB emulator files (gb_emu_*.c, gb_pak.c). */

typedef struct unk_D_8122C4FA {
  /* 0x00 */ u8 unk_00;
  /* 0x01 */ u8 unk_01;
} unk_D_8122C4FA;

typedef struct unk_D_8122B2F8 {
  /* 0x000000 */ char unk000000[0x1FEAE8];
} unk_D_8122B2F8;

typedef struct unk_D_8122B1E8 {
  /* 0x00 */ OSMesg mesg[1];
  /* 0x04 */ OSMesgQueue queue;
  /* 0x1C */ char unk1C[0x4];
  /* 0x20 */ s32 unk_20;
  /* 0x24 */ s32 unk_24;
  /* 0x28 */ void* unk_28;
  /* 0x2C */ s32 unk_2C;
  /* 0x30 */ char unk30[0x8];
  /* 0x38 */ s32 unk_38;
  /* 0x3C */ s32 unk_3C;
  /* 0x40 */ void* unk_40;
  /* 0x44 */ char unk44[0x4];
  /* 0x48 */ void* unk_48;
  /* 0x4C */ void* unk_4C;
  /* 0x50 */ u32 unk_50;
  /* 0x54 */ s32 unk_54;
  /* 0x58 */ void* unk_58;
  /* 0x5C */ s32 unk_5C;
  /* 0x60 */ char unk60[0x8];
} unk_D_8122B1E8;

typedef struct unk_D_8120D7F3 {
    u8 pad[0x116];
    u8 unk116;
} unk_D_8120D7F3;

/* Struct types shared by the audio driver files (audio_*.c). */

/* A bump allocator over one region of the audio heap. */
typedef struct AudioAllocPool {
  /* 0x00 */ u8* startRamAddr; // @unverified: same offset/use as OoT's AudioAllocPool.startRamAddr (AudioHeap_InitPool)
  /* 0x04 */ u8* curRamAddr; // @unverified: same offset/use as OoT's AudioAllocPool.curRamAddr (bumped by each alloc)
  /* 0x08 */ s32 size; // @unverified: same offset/use as OoT's AudioAllocPool.size
  /* 0x0C */ s32 numEntries; // @unverified: same offset/use as OoT's AudioAllocPool.numEntries (counts allocations)
} AudioAllocPool; // size = 0x10

typedef struct unk_D_812346E0 {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
} unk_D_812346E0;

typedef struct unk_D_81231450 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
} unk_D_81231450;

typedef struct unk_D_81234650 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
} unk_D_81234650;

typedef struct AudioCacheEntry {
    /* 0x00 */ u8* ramAddr; // @unverified: same offset/use as OoT's AudioCacheEntry.addr
    /* 0x04 */ u32 size; // @unverified: same offset/use as OoT's AudioCacheEntry.size
    /* 0x08 */ s16 tableType; // @unverified: same offset/use as OoT's AudioCacheEntry.tableType (0 sequence, 1 font, 2 sample bank)
    /* 0x0A */ s16 id; // @unverified: same offset/use as OoT's AudioCacheEntry.id
} AudioCacheEntry; // size = 0xC

typedef struct AudioPersistentCache {
    /* 0x00 */ u32 numEntries;
    /* 0x04 */ AudioAllocPool pool;
    /* 0x14 */ AudioCacheEntry entries[1];
} AudioPersistentCache; // size = 0x20

typedef struct AudioTemporaryCache {
    /* 0x00 */ u32 nextSide;
    /* 0x04 */ AudioAllocPool pool;
    /* 0x14 */ AudioCacheEntry entries[2];
} AudioTemporaryCache; // size = 0x2C

/* A loaded font (D_81288D50[fontId]). */
typedef struct SoundFont {
    /* 0x00 */ u8 numInstruments; // @unverified: same offset/use as OoT's SoundFont.numInstruments (high byte of shortData2; instrument loop bound)
    /* 0x01 */ u8 numDrums; // @unverified: same offset/use as OoT's SoundFont.numDrums (low byte of shortData2; drum loop bound)
    /* 0x02 */ u8 sampleBankId1; // @unverified: same offset/use as OoT's SoundFont.sampleBankId1 (high byte of shortData1)
    /* 0x03 */ u8 sampleBankId2; // @unverified: same offset/use as OoT's SoundFont.sampleBankId2 (low byte of shortData1)
    /* 0x04 */ u8** instruments; // @unverified: same offset/use as OoT's SoundFont.instruments (AudioPlayback_GetInstrumentInner)
    /* 0x08 */ u8** drums; // @unverified: same offset/use as OoT's SoundFont.drums (AudioPlayback_GetDrum)
} SoundFont; // size = 0xC

/* One of the sequence, font and sample bank caches (D_812877C0, D_81287820, D_81287880; 0x60 apart). */
typedef struct AudioCache {
    /* 0x00 */ AudioPersistentCache persistent;
    /* 0x20 */ AudioTemporaryCache temporary;
} AudioCache;

/* The permanent pool and the entries allocated from it (D_812878E0). */
typedef struct AudioPermanentCache {
    /* 0x00 */ AudioAllocPool pool; // @unverified: allocated from by AudioLoad's permanent alloc; pool.numEntries indexes the next entry (OoT's permanentPool)
    /* 0x10 */ AudioCacheEntry entries[1]; // @unverified: same use as OoT's AudioContext.permanentCache
} AudioPermanentCache;



typedef struct ReverbSettings {
    /* 0x00 */ u8 downsampleRate;
    /* 0x01 */ u8 windowSize;
    /* 0x02 */ u16 decayRatio;
    /* 0x04 */ u16 volume; // @unverified: copied to SynthesisReverb.volume (OoT's ReverbSettings.volume)
    /* 0x06 */ u16 leakRtl; // @unverified: copied to SynthesisReverb.leakRtl (OoT's ReverbSettings.leakRtl)
    /* 0x08 */ u16 leakLtr; // @unverified: copied to SynthesisReverb.leakLtr (OoT's ReverbSettings.leakLtr)
    /* 0x0A */ s8 mixReverbIndex; // @unverified: copied to SynthesisReverb.mixReverbIndex (OoT's ReverbSettings.unk_10)
    /* 0x0C */ u16 mixReverbStrength; // @unverified: copied to SynthesisReverb.mixReverbStrength (OoT's ReverbSettings.unk_12)
    /* 0x0E */ s16 lowPassFilterCutoffLeft;
    /* 0x10 */ s16 lowPassFilterCutoffRight;
} ReverbSettings; // size = 0x12

typedef struct AudioSpec {
    /* 0x00 */ u32 frequency;
    /* 0x04 */ u8 unk_04;
    /* 0x05 */ u8 numNotes;
    /* 0x06 */ u8 numSequencePlayers;
    /* 0x07 */ u8 numReverbs;
    /* 0x08 */ ReverbSettings* reverbSettings;
    /* 0x0C */ u16 unk_0C;
    /* 0x10 */ s32 persistentSeqMem;
    /* 0x14 */ s32 persistentFontMem;
    /* 0x18 */ s32 persistentSampleMem;
    /* 0x1C */ s32 temporarySeqMem;
    /* 0x20 */ s32 temporaryFontMem;
    /* 0x24 */ s32 temporarySampleMem;
    /* 0x28 */ s32 persistentSampleCacheSize;
    /* 0x2C */ s32 temporarySampleCacheSize;
} AudioSpec; // size = 0x30

typedef struct LoopAFF0 {
    /* 0x00 */ u32 start;
    /* 0x04 */ u32 end;
    /* 0x08 */ u32 count;
} LoopAFF0;

typedef struct Sample {
    /* 0x0 */ u32 codec : 4; // @unverified: same bits/use as OoT's Sample.codec (2 = s8 PCM)
    /* 0x0 */ u32 medium : 2; // @unverified: same bits/use as OoT's Sample.medium (0 = RAM)
    /* 0x0 */ u32 unk_bit26 : 1;
    /* 0x0 */ u32 isRelocated : 1; // @unverified: same bit/use as OoT's Sample.isRelocated
    /* 0x0 */ u32 size : 24; // @unverified: same bits/use as OoT's Sample.size
    /* 0x4 */ u8* sampleAddr; // @unverified: same offset/use as OoT's Sample.sampleAddr
    /* 0x8 */ void* loop; // @unverified: same offset/use as OoT's Sample.loop
    /* 0xC */ void* book; // @unverified: same offset/use as OoT's Sample.book
} Sample; // size = 0x10

/* A queued sample preload (OoT's AudioPreloadReq), D_81288018 */
typedef struct AudioPreloadReq {
    /* 0x00 */ u32 endAndMediumKey; // @unverified: same offset/use as OoT's AudioPreloadReq.endAndMediumKey (sampleAddr + size + medium)
    /* 0x04 */ Sample* sample; // @unverified: same offset/use as OoT's AudioPreloadReq.sample
    /* 0x08 */ u8* ramAddr; // @unverified: same offset/use as OoT's AudioPreloadReq.ramAddr
    /* 0x0C */ u32 encodedInfo; // @unverified: same offset/use as OoT's AudioPreloadReq.encodedInfo
    /* 0x10 */ s32 isFree; // @unverified: same offset/use as OoT's AudioPreloadReq.isFree
} AudioPreloadReq; // size = 0x14

/* One entry of an audio table (OoT's AudioTableEntry): sequences, sound fonts or sample banks */
typedef struct AudioTableEntry {
    /* 0x00 */ u32 romAddr; // @unverified: same offset/use as OoT's AudioTableEntry.romAddr
    /* 0x04 */ u32 size; // @unverified: same offset/use as OoT's AudioTableEntry.size
    /* 0x08 */ s8 medium; // @unverified: same offset/use as OoT's AudioTableEntry.medium
    /* 0x09 */ s8 cachePolicy; // @unverified: same offset/use as OoT's AudioTableEntry.cachePolicy
    /* 0x0A */ s16 shortData1; // @unverified: same offset/use as OoT's AudioTableEntry.shortData1
    /* 0x0C */ s16 shortData2; // @unverified: same offset/use as OoT's AudioTableEntry.shortData2
    /* 0x0E */ s16 shortData3; // @unverified: same offset/use as OoT's AudioTableEntry.shortData3
} AudioTableEntry; // size = 0x10

/* An audio table header followed by its entries (OoT's AudioTable) */
typedef struct AudioTable {
    /* 0x00 */ s16 numEntries; // @unverified: same offset/use as OoT's AudioTable.numEntries
    /* 0x02 */ s16 unkMediumParam; // @unverified: same offset/use as OoT's AudioTable.unkMediumParam
    /* 0x04 */ u32 romAddr; // @unverified: same offset/use as OoT's AudioTable.romAddr
    /* 0x08 */ char pad[0x8];
    /* 0x10 */ AudioTableEntry entries[1];
} AudioTable;

/* A slow sample load (OoT's AudioSlowLoad): AudioLoad_ProcessSlowLoads (AudioLoad_SlowLoadSample starts one,
   AudioLoad_FinishSlowLoad finishes it) */
typedef struct AudioSlowLoad {
    /* 0x00 */ u8 medium; // @unverified: same offset/use as OoT's AudioSlowLoad.medium
    /* 0x01 */ u8 seqOrFontId; // @unverified: same offset/use as OoT's AudioSlowLoad.seqOrFontId
    /* 0x02 */ u8 instId; // @unverified: same offset/use as OoT's AudioSlowLoad.instId
    /* 0x04 */ s32 curDevAddr; // @unverified: same offset/use as OoT's AudioSlowLoad.curDevAddr
    /* 0x08 */ s32 curRamAddr; // @unverified: same offset/use as OoT's AudioSlowLoad.curRamAddr (the allocation; advanced while loading)
    /* 0x0C */ s32 ramAddr; // @unverified: same offset/use as OoT's AudioSlowLoad.ramAddr (start of the loaded sample)
    /* 0x10 */ s32 state; // @unverified: same offset/use as OoT's AudioSlowLoad.state (1 start, 2 loading, 3 done)
    /* 0x14 */ s32 bytesRemaining; // @unverified: same offset/use as OoT's AudioSlowLoad.bytesRemaining
    /* 0x18 */ s8* status; // @unverified: same offset/use as OoT's AudioSlowLoad.status
    /* 0x1C */ Sample sample; // @unverified: same offset/use as OoT's AudioSlowLoad.sample (copy of the font's sample header)
    /* 0x2C */ OSMesgQueue msgQueue; // @unverified: same offset/use as OoT's AudioSlowLoad.msgQueue (AudioLoad_DmaSlowCopy)
    /* 0x44 */ OSMesg msg; // @unverified: same offset/use as OoT's AudioSlowLoad.msg
    /* 0x48 */ OSIoMesg ioMesg; // @unverified: same offset/use as OoT's AudioSlowLoad.ioMesg
} AudioSlowLoad; // size = 0x60

typedef struct SoundFontSound {
    /* 0x0 */ Sample* sample; // @unverified: same offset/use as OoT's SoundFontSound.sample
    /* 0x4 */ f32 tuning; // @unverified: same offset/use as OoT's SoundFontSound.tuning
} SoundFontSound; // size = 0x8

typedef struct ReverbRingBufferItem {
    /* 0x00 */ s16 numSamplesAfterDownsampling;
    /* 0x02 */ s16 chunkLen;
    /* 0x04 */ s16* toDownsampleLeft;
    /* 0x08 */ s16* toDownsampleRight;
    /* 0x0C */ s32 startPos; // @unverified: same offset/use as OoT's ReverbRingBufferItem.startPos
    /* 0x10 */ s16 lengthA; // @unverified: same offset/use as OoT's ReverbRingBufferItem.lengthA
    /* 0x12 */ s16 lengthB; // @unverified: same offset/use as OoT's ReverbRingBufferItem.lengthB (part after the ring buffer wraps)
} ReverbRingBufferItem; // size = 0x14

typedef struct SynthesisReverb {
    /* 0x000 */ u8 resampleFlags;
    /* 0x001 */ u8 useReverb;
    /* 0x002 */ u8 framesToIgnore;
    /* 0x003 */ u8 curFrame;
    /* 0x004 */ u8 downsampleRate;
    /* 0x005 */ s8 mixReverbIndex; // @unverified: from the code: index of another reverb mixed in when != -1 (OoT's SynthesisReverb.unk_05)
    /* 0x006 */ u16 windowSize;
    /* 0x008 */ u16 mixReverbStrength; // @unverified: from the code: aMix gain of that mixed-in reverb (OoT's SynthesisReverb.unk_08)
    /* 0x00A */ u16 volume; // @unverified: same offset/use as OoT's SynthesisReverb.volume
    /* 0x00C */ u16 decayRatio;
    /* 0x00E */ s16 resampleRate; // @unverified: 0x8000 / downsampleRate, the aResample pitch (OoT's SynthesisReverb.unk_0E)
    /* 0x010 */ u16 leakRtl; // @unverified: same offset/use as OoT's SynthesisReverb.leakRtl
    /* 0x012 */ u16 leakLtr; // @unverified: same offset/use as OoT's SynthesisReverb.leakLtr
    /* 0x014 */ s32 nextRingBufPos;
    /* 0x018 */ s32 unk_18;
    /* 0x01C */ s32 bufSizePerChan;
    /* 0x020 */ s16* leftRingBuf;
    /* 0x024 */ s16* rightRingBuf;
    /* 0x028 */ void* resampleStateLeft; // @unverified: 0x20-byte aResample state, left (OoT's SynthesisReverb.unk_30)
    /* 0x02C */ void* resampleStateRight; // @unverified: 0x20-byte aResample state, right (OoT's SynthesisReverb.unk_34)
    /* 0x030 */ void* unk_30;
    /* 0x034 */ void* unk_34;
    /* 0x038 */ ReverbRingBufferItem items[2][5];
    /* 0x100 */ s16* filterLeft;
    /* 0x104 */ s16* filterRight;
    /* 0x108 */ s16* filterLeftState;
    /* 0x10C */ s16* filterRightState;
    /* 0x110 */ SoundFontSound sound; // @unverified: same offset/use as OoT's SynthesisReverb.sound (AudioSeq layers play it for instrument ids 0xC0+)
    /* 0x118 */ Sample sample;
    /* 0x128 */ LoopAFF0 loop;
    /* 0x134 */ u8 unk134[0x158 - 0x134];
} SynthesisReverb; // size = 0x158

typedef struct AudioSessionPoolSplit {
    /* 0x00 */ s32 miscPoolSize;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ s32 cachePoolSize;
} AudioSessionPoolSplit; // size = 0x10

typedef struct AudioCachePoolSplit {
    /* 0x00 */ s32 persistentCommonPoolSize; // @unverified: same offset/use as OoT's AudioCachePoolSplit.persistentCommonPoolSize
    /* 0x04 */ s32 temporaryCommonPoolSize; // @unverified: same offset/use as OoT's AudioCachePoolSplit.temporaryCommonPoolSize
} AudioCachePoolSplit; // size = 0x8

typedef struct AudioCommonPoolSplit {
    /* 0x00 */ s32 seqCacheSize; // @unverified: same offset/use as OoT's AudioCommonPoolSplit.seqCacheSize
    /* 0x04 */ s32 fontCacheSize; // @unverified: same offset/use as OoT's AudioCommonPoolSplit.fontCacheSize
    /* 0x08 */ s32 sampleBankCacheSize; // @unverified: same offset/use as OoT's AudioCommonPoolSplit.sampleBankCacheSize
} AudioCommonPoolSplit; // size = 0xC


typedef struct GbApuChannelState {
  /* 0x00 */ u8 enabled; // @unverified: from the code: channel on (the NR52 status bit); set on trigger, cleared at length/envelope end and on power-off
  /* 0x01 */ char unk01[0x3];
  /* 0x04 */ s32 retriggerTimer; // @unverified: from the code: wave channel only; set to 0x140 on trigger, the old note fades out while it counts down
  /* 0x08 */ union {
      u16 index; // @unverified: top half of the accumulator, passed to GbApu_GetWaveformSample
      s32 acc; // @unverified: the accumulator, advanced by step and masked to 21 bits
  } phase; // @unverified: from the code: waveform phase, a 21-bit accumulator whose top bits index the waveform
  /* 0x0C */ u32 step; // @unverified: from the code: phase increment per sample, from the frequency (GbApu_FreqToStep*) or the NR43 noise table
  /* 0x10 */ u16 frequency; // @unverified: from the code: the 11-bit NRx3/NRx4 frequency (noise: the NR43 shift); changed by the square 1 sweep
  /* 0x12 */ u16 unk_12;
  /* 0x14 */ s32 lengthTimer; // @unverified: from the code: length * 200 samples, counted down while lengthEnabled; 0 turns the channel off
  /* 0x18 */ s32 lengthEnabled; // @unverified: from the code: NRx4 bit 6 (counter/consecutive)
  /* 0x1C */ s32 length; // @unverified: from the code: 64 - NRx1 length (256 - NR31 for the wave channel)
  /* 0x20 */ u8 amplitude; // @unverified: from the code: target output level, from the envelope volume through a table
  /* 0x21 */ u8 initialVolume; // @unverified: from the code: NRx2 initial volume (NR32 output level for the wave channel)
  /* 0x22 */ char unk22[0x2];
  /* 0x24 */ u32 envelopeCounter; // @unverified: from the code: samples since the last envelope step
  /* 0x28 */ u32 envelopePeriod; // @unverified: from the code: NRx2 envelope sweep * 800 samples; 0 = envelope off
  /* 0x2C */ char unk2C[0x1];
  /* 0x2D */ u8 envelopeVolume; // @unverified: from the code: current envelope volume 0-15
  /* 0x2E */ u8 envelopeDir; // @unverified: from the code: NRx2 envelope direction (0 down, 1 up)
  /* 0x2F */ char unk2F[0x1];
  /* 0x30 */ u32 sweepCounter; // @unverified: from the code: square 1 only; samples since the last frequency sweep step
  /* 0x34 */ u32 sweepPeriod; // @unverified: from the code: square 1 only; NR10 sweep time * 400 samples
  /* 0x38 */ u8 sweepShift; // @unverified: from the code: square 1 only; NR10 sweep shift
  /* 0x39 */ u8 sweepDir; // @unverified: from the code: square 1 only; NR10 sweep direction (0 up, 1 down)
  /* 0x3A */ u8 sweepEnabled; // @unverified: from the code: square 1 only; set when sweepPeriod != 0
  /* 0x3B */ u8 duty; // @unverified: from the code: NRx1 duty (index of the square waveform)
  /* 0x3C */ u8 appliedAmplitude; // @unverified: from the code: amplitude actually multiplied into the sample; fades to 0 after the channel stops
  /* 0x3D */ char unk3D[0x3];
  /* 0x40 */ u32 appliedStep; // @unverified: from the code: step kept for the fade-out; the noise channel slews it toward step
  /* 0x44 */ u16 prevSample; // @unverified: from the code: wave channel only; last output, averaged with the next sample
  /* 0x46 */ u16 unk46;
  /* 0x48 */ u32 stepSlew; // @unverified: from the code: noise channel only; per-sample change of appliedStep, |step - appliedStep| >> 7
  /* 0x4C */ u8 outOfRange; // @unverified: from the code: 1 while the frequency is outside 1-0x7FE, the channel then fades out
  /* 0x4D */ char unk4D[0x3];
} GbApuChannelState;

typedef struct GbApuStatusOverride {
  /* 0x00 */ u8 square1; // @unverified: from the code (GbApu_QueueRegisterWrite, GbMem_ReadIoRegister): pending NR52 status of square 1: 0 none, 1 off, 2 triggered, until the queued write is rendered
  /* 0x01 */ u8 square2; // @unverified: from the code (GbApu_QueueRegisterWrite, GbMem_ReadIoRegister): same for square 2
  /* 0x02 */ u8 wave; // @unverified: from the code (GbApu_QueueRegisterWrite, GbMem_ReadIoRegister): same for the wave channel
  /* 0x03 */ u8 noise; // @unverified: from the code (GbApu_QueueRegisterWrite, GbMem_ReadIoRegister): same for the noise channel
} GbApuStatusOverride;

typedef struct GbApuRegs {
    /* 0x00 NR10 */ u32 : 1;
                    u32 sweepTime : 3;
                    u32 sweepDir : 1;
                    u32 sweepShift : 3;
    /* 0x01 NR11 */ u32 duty1 : 2;
                    u32 length1 : 6;
    /* 0x02 NR12 */ u32 volume1 : 4;
                    u32 envDir1 : 1;
                    u32 envSweep1 : 3;
    /* 0x03 NR13 */ u32 freqLo1 : 8;
    /* 0x04 NR14 */ u32 trigger1 : 1;
                    u32 counter1 : 1;
                    u32 : 3;
                    u32 freqHi1 : 3;
    /* 0x05 */      u32 : 8;
    /* 0x06 NR21 */ u32 duty2 : 2;
                    u32 length2 : 6;
    /* 0x07 NR22 */ u32 volume2 : 4;
                    u32 envDir2 : 1;
                    u32 envSweep2 : 3;
    /* 0x08 NR23 */ u32 freqLo2 : 8;
    /* 0x09 NR24 */ u32 trigger2 : 1;
                    u32 counter2 : 1;
                    u32 : 3;
                    u32 freqHi2 : 3;
    /* 0x0A NR30 */ u32 waveOn : 1;
                    u32 : 7;
    /* 0x0B NR31 */ u32 length3 : 8;
    /* 0x0C NR32 */ u32 : 1;
                    u32 level3 : 2;
                    u32 : 5;
    /* 0x0D NR33 */ u32 freqLo3 : 8;
    /* 0x0E NR34 */ u32 trigger3 : 1;
                    u32 counter3 : 1;
                    u32 : 3;
                    u32 freqHi3 : 3;
    /* 0x0F */      u32 : 8;
    /* 0x10 NR41 */ u32 : 2;
                    u32 length4 : 6;
    /* 0x11 NR42 */ u32 volume4 : 4;
                    u32 envDir4 : 1;
                    u32 envSweep4 : 3;
    /* 0x12 NR43 */ u16 shift4 : 4;
                    u32 width4 : 1;
                    u32 ratio4 : 3;
    /* 0x13 NR44 */ u32 trigger4 : 1;
                    u32 counter4 : 1;
                    u32 : 6;
    /* 0x14 NR50 */ u32 vinLeft : 1;
                    u32 leftVolume : 3;
                    u32 vinRight : 1;
                    u32 rightVolume : 3;
    /* 0x15 NR51 */ u32 noiseLeft : 1;
                    u32 waveLeft : 1;
                    u32 sq2Left : 1;
                    u32 sq1Left : 1;
                    u32 noiseRight : 1;
                    u32 waveRight : 1;
                    u32 sq2Right : 1;
                    u32 sq1Right : 1;
    /* 0x16 NR52 */ u32 power : 1;
                    u32 : 7;
    /* 0x17 */      u32 : 8;
} GbApuRegs;

#define GB_APU_REGS (*(GbApuRegs*)D_81138A80)

#define GB_APU_WRITE_QUEUE_LEN 1800

typedef struct GbApuWrite {
    /* 0x0 */ u8 reg;
    /* 0x1 */ u8 value;
    /* 0x2 */ u16 time;
} GbApuWrite;

typedef union GbApuMesg {
    OSMesg mesg;
    GbApuWrite write;
} GbApuMesg;

// The Game Boy emulator core's state (D_812599E0). Merges Screen81104930 (gb_emu_draw.c and
// gb_emu_frame.c) and GbEmuRunState (gb_emu_frame.c), three views of the same variable.
typedef struct GbEmuState {
    /* 0x00 */ u8 unk_00[6];
    /* 0x06 */ u8 unk_06; // set to 1 around the core start in GbEmu_LoadCartridge, cleared in GbEmu_UpdateFrame
    /* 0x07 */ u8 unk_07;
    /* 0x08 */ u8 unk_08[4];
    /* 0x0C */ u16 gbPc; // GB program counter saved when the core stops (seen in play); 0 after the boot ROM is patched in
    /* 0x0E */ u8 unk_0E[2];
    /* 0x10 */ u8* mem; // @unverified: 0x27F80-byte GB address space and emulator tables (GbEmu_LoadCartridge); 0xFF42 is SCY
    /* 0x14 */ u8 unk_14[8];
    /* 0x1C */ u8* rom; // cartridge ROM buffer (seen in play)
    /* 0x20 */ u8* sram; // @unverified: cartridge RAM buffer, sramSize bytes (GbEmu_LoadCartridge)
    /* 0x24 */ u32 buttons;
    /* 0x28 */ u8 unk_28[0x10];
    /* 0x38 */ s32 unk_38;
    /* 0x3C */ u32 romSize; // @unverified: 0x80000 or 0x100000 from GbPakChannel.romSize (GbEmu_LoadCartridge)
    /* 0x40 */ u32 sramSize; // @unverified: 0x8000 (GbEmu_LoadCartridge)
    /* 0x44 */ u8* unk_44;
    /* 0x48 */ u8* unk_48;
    /* 0x4C */ s32 framebuffer;
    /* 0x50 */ u8* unk_50;
    /* 0x54 */ s32 colorIndex;
} GbEmuState; // size = 0x58

// One Transfer Pak channel of the Game Boy emulator (D_81259668[4]). Merges the three identical
// copies that gb_emu_draw.c, gb_emu_frame.c and gb_pak.c had.
typedef struct GbPakChannel {
    /* 0x00 */ char unk00[0x48]; // the start of an OSGbpakId (osGbpakReadId in gb_emu_draw.c)
    /* 0x48 */ u8 romSize; // @unverified: OSGbpakId.rom_size; 4 selects a 512 KiB ROM, else 1 MiB (gb_emu_draw.c)
    /* 0x49 */ char unk49[0x3];
    /* 0x4C */ u8 maskRomVersion; // @unverified: OSGbpakId.version; picks the bank list of a game revision (gb_emu_draw.c)
    /* 0x4D */ char unk4D[0x1];
    /* 0x4E */ u8 checksum[2];
    /* 0x50 */ OSPfs* pfs;
    /* 0x54 */ u8* buf;
    /* 0x58 */ u8* romBuf;
    /* 0x5C */ u8* sramBuf;
    /* 0x60 */ volatile u8* bankState;
    /* 0x64 */ volatile u8* sramState;
    /* 0x68 */ u8* bankList;
    /* 0x6C */ u8* bankEntry;
    /* 0x70 */ u8* ramBuf;
    /* 0x74 */ u8* blockState;
    /* 0x78 */ u8* verifyBuf;
    /* 0x7C */ u8* blockDirty;
    /* 0x80 */ s32 size;
    /* 0x84 */ s32 sum;
    /* 0x88 */ char unk88[0x4];
    /* 0x8C */ s32 addr;
    /* 0x90 */ u32 ramSize;
    /* 0x94 */ s32 sramAddr;
    /* 0x98 */ u8 unk98;
    /* 0x99 */ volatile u8 request;
    /* 0x9A */ volatile u8 error;
    /* 0x9B */ u8 status;
    /* 0x9C */ u8 channel;
    /* 0x9D */ char unk9D[0x3];
    /* 0xA0 */ u8 gameIndex; // @unverified: GbPak_IdentifyCartridge, 0 not a supported game; colorIndex is gameIndex - 1
    /* 0xA1 */ u8 unkA1;
    /* 0xA2 */ u8 unkA2;
    /* 0xA3 */ char unkA3[0x1];
} GbPakChannel; // size = 0xA4

typedef struct AudioBufferParameters {
    /* 0x00 */ s16 specUnk4; // @unverified: same offset/use as OoT's AudioBufferParameters.specUnk4 (set from AudioSpec.unk_04; 2 doubles the AI buffer bounds)
    /* 0x02 */ u16 samplingFrequency; // @unverified: same offset/use as OoT's AudioBufferParameters.samplingFrequency
    /* 0x04 */ s16 aiSamplingFrequency; // @unverified: same offset/use as OoT's AudioBufferParameters.aiSamplingFrequency (osAiSetFrequency result)
    /* 0x06 */ s16 samplesPerFrameTarget; // @unverified: same offset/use as OoT's AudioBufferParameters.samplesPerFrameTarget
    /* 0x08 */ s16 maxAiBufferLength; // @unverified: same offset/use as OoT's AudioBufferParameters.maxAiBufferLength
    /* 0x0A */ s16 minAiBufferLength; // @unverified: same offset/use as OoT's AudioBufferParameters.minAiBufferLength
    /* 0x0C */ s16 ticksPerUpdate; // @unverified: same offset/use as OoT's AudioBufferParameters.ticksPerUpdate
    /* 0x0E */ s16 samplesPerTick; // @unverified: same offset/use as OoT's AudioBufferParameters.samplesPerTick
    /* 0x10 */ s16 samplesPerTickMax; // @unverified: same offset/use as OoT's AudioBufferParameters.samplesPerTickMax
    /* 0x12 */ s16 samplesPerTickMin; // @unverified: same offset/use as OoT's AudioBufferParameters.samplesPerTickMin
    /* 0x14 */ s16 numSequencePlayers; // @unverified: same offset/use as OoT's AudioBufferParameters.numSequencePlayers
    /* 0x16 */ u8 pad16[0x18 - 0x16];
    /* 0x18 */ f32 resampleRate; // @unverified: same offset/use as OoT's AudioBufferParameters.resampleRate (32000 / samplingFrequency)
    /* 0x1C */ f32 ticksPerUpdateInv; // @unverified: same offset/use as OoT's AudioBufferParameters.ticksPerUpdateInv
    /* 0x20 */ f32 ticksPerUpdateInvScaled; // @unverified: same offset/use as OoT's AudioBufferParameters.ticksPerUpdateInvScaled
    /* 0x24 */ f32 ticksPerUpdateScaled; // @unverified: same offset/use as OoT's AudioBufferParameters.ticksPerUpdateScaled
    /* 0x28 */ u8 pad28[0x38 - 0x28];
} AudioBufferParameters;

typedef struct GbAudioStream {
    /* 0x0 */ s32 id;
    /* 0x4 */ s32 pos;
} GbAudioStream;

#define GB_AUDIO_STREAM_DATA ((s8**)D_8115265C)

#define GB_AUDIO_STREAM_SIZE ((s32*)D_81152680)

typedef struct GbStereoSample {
    /* 0x0 */ s16 left;
    /* 0x2 */ s16 right;
} GbStereoSample;

/* A node or head of a circular list (D_81287DC8 is the free list of sequence layers). */
typedef struct AudioListItem {
    /* 0x00 */ struct AudioListItem* prev; // @unverified: same offset/use as OoT's AudioListItem.prev (NULL when not on a list)
    /* 0x04 */ struct AudioListItem* next; // @unverified: same offset/use as OoT's AudioListItem.next
    /* 0x08 */ union {
        void* value; // @unverified: same offset/use as OoT's AudioListItem.u.value (the note or layer an item belongs to)
        s32 count; // @unverified: same offset/use as OoT's AudioListItem.u.count (list heads)
    } u;
    /* 0x0C */ struct NotePool* pool; // @unverified: same offset/use as OoT's AudioListItem.pool
} AudioListItem; // size = 0x10

/* The shared sequence player, channel, layer and note types. Their layouts were put together from the
 * per-function views the code used before (each view only named what its function touched); offsets
 * agree with OoT's z64audio.h except where noted. */

struct SequencePlayer;
struct SequenceChannel;
struct SequenceLayer;
struct Note;

typedef struct NotePool {
    /* 0x00 */ AudioListItem disabled; // @unverified: same offset/use as OoT's NotePool.disabled
    /* 0x10 */ AudioListItem decaying; // @unverified: same offset/use as OoT's NotePool.decaying
    /* 0x20 */ AudioListItem releasing; // @unverified: same offset/use as OoT's NotePool.releasing
    /* 0x30 */ AudioListItem active; // @unverified: same offset/use as OoT's NotePool.active
} NotePool; // size = 0x40

typedef struct SeqScriptState {
    /* 0x00 */ u8* pc; // @unverified: same offset/use as OoT's SeqScriptState.pc (read by the script step functions)
    /* 0x04 */ u8* stack[4]; // @unverified: same offset/use as OoT's SeqScriptState.stack (call/loop return addresses)
    /* 0x14 */ u8 remLoopIters[4]; // @unverified: same offset/use as OoT's SeqScriptState.remLoopIters
    /* 0x18 */ u8 depth; // @unverified: same offset/use as OoT's SeqScriptState.depth
    /* 0x19 */ s8 value; // @unverified: same offset/use as OoT's SeqScriptState.value
    /* 0x1A */ u8 unk_1A[2];
} SeqScriptState; // size = 0x1C

typedef struct EnvelopePoint {
    /* 0x0 */ s16 delay; // @unverified: same offset/use as OoT's EnvelopePoint.delay (negative values are commands)
    /* 0x2 */ s16 arg; // @unverified: same offset/use as OoT's EnvelopePoint.arg
} EnvelopePoint; // size = 0x4

typedef struct AdsrSettings {
    /* 0x0 */ u8 decayIndex; // @unverified: same offset/use as OoT's AdsrSettings.decayIndex (release rate)
    /* 0x1 */ u8 sustain; // @unverified: same offset/use as OoT's AdsrSettings.sustain
    /* 0x2 */ u8 unk_02[2];
    /* 0x4 */ EnvelopePoint* envelope; // @unverified: same offset/use as OoT's AdsrSettings.envelope
} AdsrSettings; // size = 0x8

typedef struct Portamento {
    /* 0x0 */ u8 mode; // @unverified: same offset/use as OoT's Portamento.mode (modes 1 and 2 are reset to 0)
    /* 0x4 */ f32 cur; // @unverified: same offset/use as OoT's Portamento.cur
    /* 0x8 */ f32 speed; // @unverified: same offset/use as OoT's Portamento.speed
    /* 0xC */ f32 extent; // @unverified: same offset/use as OoT's Portamento.extent
} Portamento; // size = 0x10


typedef struct Drum {
    /* 0x0 */ u8 adsrDecayIndex; // @unverified: same offset/use as OoT's Drum.adsrDecayIndex (copied to the layer's release rate)
    /* 0x1 */ u8 pan; // @unverified: same offset/use as OoT's Drum.pan
    /* 0x2 */ u8 isRelocated;
    /* 0x3 */ u8 unk_03;
    /* 0x4 */ SoundFontSound sound; // @unverified: same offset/use as OoT's Drum.sound
    /* 0xC */ EnvelopePoint* envelope; // @unverified: same offset/use as OoT's Drum.envelope
} Drum; // size = 0x10

typedef union Stereo {
    struct {
        u8 bit0 : 1;
        u8 bit1 : 1;
        u8 bit2 : 1;
        u8 usesHeadsetPanEffects : 1; // @unverified: same bit/use as OoT's Stereo.s.usesHeadsetPanEffects
        u8 stereoHeadsetEffects : 2; // @unverified: same bits/use as OoT's Stereo.s.stereoHeadsetEffects (switch in AudioPlayback_InitSampleState)
        u8 strongRight : 1; // @unverified: same bit/use as OoT's Stereo.s.strongRight
        u8 strongLeft : 1; // @unverified: same bit/use as OoT's Stereo.s.strongLeft
    } s;
    u8 asByte;
} Stereo; // size = 0x1

typedef struct SequencePlayer {
    /* 0x000 */ u32 enabled : 1; // @unverified: players with this bit are stepped each update, OoT's SequencePlayer.enabled
    /* 0x000 */ u32 finished : 1; // @unverified: same bit/use as OoT's SequencePlayer.finished
    /* 0x000 */ u32 muted : 1; // @unverified: same bit/use as OoT's SequencePlayer.muted
    /* 0x000 */ u32 seqDmaInProgress : 1; // @unverified: same bit/use as OoT's SequencePlayer.seqDmaInProgress
    /* 0x000 */ u32 fontDmaInProgress : 1; // @unverified: same bit/use as OoT's SequencePlayer.fontDmaInProgress
    /* 0x000 */ u32 recalculateVolume : 1; // @unverified: same bit/use as OoT's SequencePlayer.recalculateVolume
    /* 0x000 */ u32 stopScript : 1; // @unverified: same bit/use as OoT's SequencePlayer.stopScript
    /* 0x000 */ u32 applyBend : 1; // @unverified: same bit/use as OoT's SequencePlayer.applyBend
    /* 0x001 */ u8 state; // @unverified: same offset/use as OoT's SequencePlayer.state (fade state)
    /* 0x002 */ u8 noteAllocPolicy; // @unverified: same offset/use as OoT's SequencePlayer.noteAllocPolicy
    /* 0x003 */ u8 muteBehavior; // @unverified: same offset/use as OoT's SequencePlayer.muteFlags
    /* 0x004 */ u8 seqId; // @unverified: same offset/use as OoT's SequencePlayer.seqId
    /* 0x005 */ u8 defaultFont; // @unverified: same offset/use as OoT's SequencePlayer.defaultFont
    /* 0x006 */ u8 unk_06[2];
    /* 0x008 */ u16 tempo; // @unverified: same offset/use as OoT's SequencePlayer.tempo (ticks per minute)
    /* 0x00A */ u16 tempoAcc; // @unverified: same offset/use as OoT's SequencePlayer.tempoAcc
    /* 0x00C */ s16 tempoChange; // @unverified: same offset/use as OoT's SequencePlayer.tempoChange
    /* 0x00E */ s16 transposition; // @unverified: same offset/use as OoT's SequencePlayer.transposition
    /* 0x010 */ u16 delay; // @unverified: same offset/use as OoT's SequencePlayer.delay
    /* 0x012 */ u16 fadeTimer; // @unverified: same offset/use as OoT's SequencePlayer.fadeTimer
    /* 0x014 */ u16 storedFadeTimer; // @unverified: same offset/use as OoT's SequencePlayer.storedFadeTimer
    /* 0x016 */ u8 unk_16[2];
    /* 0x018 */ u8* seqData; // @unverified: same offset/use as OoT's SequencePlayer.seqData
    /* 0x01C */ f32 fadeVolume; // @unverified: same offset/use as OoT's SequencePlayer.fadeVolume
    /* 0x020 */ f32 fadeVelocity; // @unverified: same offset/use as OoT's SequencePlayer.fadeVelocity
    /* 0x024 */ f32 volume; // @unverified: same offset/use as OoT's SequencePlayer.volume
    /* 0x028 */ f32 muteVolumeScale; // @unverified: same offset/use as OoT's SequencePlayer.muteVolumeScale
    /* 0x02C */ f32 fadeVolumeScale; // @unverified: same offset/use as OoT's SequencePlayer.fadeVolumeScale
    /* 0x030 */ f32 appliedFadeVolume; // @unverified: same offset/use as OoT's SequencePlayer.appliedFadeVolume
    /* 0x034 */ struct SequenceChannel* channels[16]; // @unverified: same offset/use as OoT's SequencePlayer.channels
    /* 0x074 */ SeqScriptState scriptState; // @unverified: same offset/use as OoT's SequencePlayer.scriptState
    /* 0x090 */ u8* shortNoteVelocityTable; // @unverified: same offset/use as OoT's SequencePlayer.shortNoteVelocityTable
    /* 0x094 */ u8* shortNoteGateTimeTable; // @unverified: same offset/use as OoT's SequencePlayer.shortNoteGateTimeTable
    /* 0x098 */ NotePool notePool; // @unverified: same offset/use as OoT's SequencePlayer.notePool
    /* 0x0D8 */ s32 skipTicks; // @unverified: same offset/use as OoT's SequencePlayer.skipTicks
    /* 0x0DC */ u32 scriptCounter; // @unverified: same offset/use as OoT's SequencePlayer.scriptCounter
    /* 0x0E0 */ u8 unk_0E0[0x154 - 0xE0];
    /* 0x154 */ s8 soundScriptIO[8]; // @unverified: same offset/use as OoT's SequencePlayer.soundScriptIO
} SequencePlayer; // size = 0x15C

typedef struct SequenceChannel {
    /* 0x00 */ u8 enabled : 1; // @unverified: same bit/use as OoT's SequenceChannel.enabled
    /* 0x00 */ u8 finished : 1; // @unverified: same bit/use as OoT's SequenceChannel.finished
    /* 0x00 */ u8 stopScript : 1; // @unverified: same bit/use as OoT's SequenceChannel.stopScript
    /* 0x00 */ u8 muted : 1; // @unverified: same bit as OoT's SequenceChannel.muted (SM64's stopSomething2)
    /* 0x00 */ u8 hasInstrument : 1; // @unverified: cleared when the instrument lookup fails, OoT's SequenceChannel.hasInstrument
    /* 0x00 */ u8 stereoHeadsetEffects : 1; // @unverified: same bit/use as OoT's SequenceChannel.stereoHeadsetEffects
    /* 0x00 */ u8 largeNotes : 1; // @unverified: same bit/use as OoT's SequenceChannel.largeNotes
    /* 0x00 */ u8 unused : 1;
    /* 0x01 */ union {
        struct {
            u8 freqScale : 1; // @unverified: same bit/use as OoT's SequenceChannel.changes.s.freqScale
            u8 volume : 1; // @unverified: same bit/use as OoT's SequenceChannel.changes.s.volume
            u8 pan : 1; // @unverified: same bit/use as OoT's SequenceChannel.changes.s.pan
            u8 rest : 5;
        } s;
        u8 asByte; // @unverified: set to 0xFF when a channel is initialized, as OoT's changes.asByte
    } changes;
    /* 0x02 */ u8 noteAllocPolicy; // @unverified: same offset/use as OoT's SequenceChannel.noteAllocPolicy
    /* 0x03 */ u8 muteBehavior; // @unverified: same offset/use as OoT's SequenceChannel.muteFlags
    /* 0x04 */ u8 reverb; // @unverified: same offset/use as OoT's SequenceChannel.reverb (target reverb volume)
    /* 0x05 */ u8 notePriority; // @unverified: same offset/use as OoT's SequenceChannel.notePriority
    /* 0x06 */ u8 someOtherPriority; // @unverified: same offset/use as OoT's SequenceChannel.someOtherPriority
    /* 0x07 */ u8 fontId; // @unverified: same offset/use as OoT's SequenceChannel.fontId
    /* 0x08 */ u8 reverbIndex; // @unverified: same offset/use as OoT's SequenceChannel.reverbIndex
    /* 0x09 */ u8 bookOffset; // @unverified: same offset/use as OoT's SequenceChannel.bookOffset
    /* 0x0A */ u8 newPan; // @unverified: same offset/use as OoT's SequenceChannel.newPan
    /* 0x0B */ u8 panChannelWeight; // @unverified: same offset/use as OoT's SequenceChannel.panChannelWeight
    /* 0x0C */ u8 gain; // @unverified: same offset/use as OoT's SequenceChannel.gain
    /* 0x0D */ u8 velocityRandomVariance; // @unverified: same offset/use as OoT's SequenceChannel.velocityRandomVariance
    /* 0x0E */ u8 gateTimeRandomVariance; // @unverified: same offset/use as OoT's SequenceChannel.gateTimeRandomVariance
    /* 0x0F */ u8 combFilterSize; // @unverified: same offset/use as OoT's SequenceChannel.combFilterSize
    /* 0x10 */ u16 vibratoRateStart; // @unverified: same offset/use as OoT's SequenceChannel.vibratoRateStart
    /* 0x12 */ u16 vibratoExtentStart; // @unverified: same offset/use as OoT's SequenceChannel.vibratoExtentStart
    /* 0x14 */ u16 vibratoRateTarget; // @unverified: same offset/use as OoT's SequenceChannel.vibratoRateTarget
    /* 0x16 */ u16 vibratoExtentTarget; // @unverified: same offset/use as OoT's SequenceChannel.vibratoExtentTarget
    /* 0x18 */ u16 vibratoRateChangeDelay; // @unverified: same offset/use as OoT's SequenceChannel.vibratoRateChangeDelay
    /* 0x1A */ u16 vibratoExtentChangeDelay; // @unverified: same offset/use as OoT's SequenceChannel.vibratoExtentChangeDelay
    /* 0x1C */ u16 vibratoDelay; // @unverified: same offset/use as OoT's SequenceChannel.vibratoDelay
    /* 0x1E */ u16 delay; // @unverified: same offset/use as OoT's SequenceChannel.delay (zeroed on enable)
    /* 0x20 */ u16 combFilterGain; // @unverified: same offset/use as OoT's SequenceChannel.combFilterGain
    /* 0x22 */ s16 instOrWave; // @unverified: same offset/use as OoT's SequenceChannel.instOrWave
    /* 0x24 */ s16 transposition; // @unverified: same offset/use as OoT's SequenceChannel.transposition
    /* 0x26 */ u8 unk_26[2];
    /* 0x28 */ f32 volumeScale; // @unverified: same offset/use as OoT's SequenceChannel.volumeScale
    /* 0x2C */ f32 volume; // @unverified: same offset/use as OoT's SequenceChannel.volume
    /* 0x30 */ s32 pan; // @unverified: same offset/use as OoT's SequenceChannel.pan
    /* 0x34 */ f32 appliedVolume; // @unverified: same offset/use as OoT's SequenceChannel.appliedVolume
    /* 0x38 */ f32 freqScale; // @unverified: same offset/use as OoT's SequenceChannel.freqScale
    /* 0x3C */ u8 (*dynTable)[][2]; // @unverified: same offset/use as OoT's SequenceChannel.dynTable
    /* 0x40 */ struct Note* noteUnused;
    /* 0x44 */ struct SequenceLayer* layerUnused;
    /* 0x48 */ u8* instrument; // @unverified: same offset/use as OoT's SequenceChannel.instrument
    /* 0x4C */ struct SequencePlayer* seqPlayer; // @unverified: same offset/use as OoT's SequenceChannel.seqPlayer
    /* 0x50 */ struct SequenceLayer* layers[4]; // @unverified: same offset/use as OoT's SequenceChannel.layers
    /* 0x60 */ SeqScriptState scriptState; // @unverified: same offset/use as OoT's SequenceChannel.scriptState
    /* 0x7C */ AdsrSettings adsr; // @unverified: same offset/use as OoT's SequenceChannel.adsr
    /* 0x84 */ NotePool notePool; // @unverified: same offset/use as OoT's SequenceChannel.notePool
    /* 0xC4 */ s8 soundScriptIO[8]; // @unverified: same offset/use as OoT's SequenceChannel.soundScriptIO
    /* 0xCC */ u16 unk_CC;
    /* 0xCE */ u8 unk_CE[2];
    /* 0xD0 */ s16* filter; // @unverified: same offset/use as OoT's SequenceChannel.filter
} SequenceChannel; // size = 0xD4

typedef struct SequenceLayer {
    /* 0x00 */ u8 enabled : 1; // @unverified: cleared by AudioSeq_SeqLayerDisable, OoT's SequenceLayer.enabled
    /* 0x00 */ u8 finished : 1; // @unverified: set by AudioSeq_SeqLayerDisable, OoT's SequenceLayer.finished
    /* 0x00 */ u8 stopSomething : 1; // @unverified: same bit as SM64's SequenceChannelLayer.stopSomething
    /* 0x00 */ u8 continuousNotes : 1; // @unverified: same bit/use as OoT's SequenceLayer.continuousNotes
    /* 0x00 */ u8 bit3 : 1;
    /* 0x00 */ u8 notePropertiesNeedInit : 1; // @unverified: set at the end of AudioSeq_SeqLayerProcessScriptStep1, as OoT's SequenceLayer.notePropertiesNeedInit
    /* 0x00 */ u8 ignoreDrumPan : 1; // @unverified: same bit/use as OoT's SequenceLayer.ignoreDrumPan
    /* 0x00 */ u8 bit1 : 1; // @unverified: set by AudioPlayback_ProcessNotes when the note is released; tested with continuousNotes in AudioSeq_SeqLayerProcessScriptStep5
    /* 0x01 */ Stereo stereo; // @unverified: same offset/use as OoT's SequenceLayer.stereo
    /* 0x02 */ u8 instOrWave; // @unverified: same offset/use as OoT's SequenceLayer.instOrWave
    /* 0x03 */ u8 status; // @unverified: compared with 1/2 in the layer step functions
    /* 0x04 */ u8 noteDuration; // @unverified: same offset/use as OoT's SequenceLayer.gateTime
    /* 0x05 */ u8 portamentoTargetNote; // @unverified: same offset/use as OoT's SequenceLayer.portamentoTargetNote
    /* 0x06 */ u8 pan; // @unverified: same offset/use as OoT's SequenceLayer.pan (0..127)
    /* 0x07 */ u8 notePan; // @unverified: same offset/use as OoT's SequenceLayer.notePan
    /* 0x08 */ Portamento portamento; // @unverified: same offset/use as OoT's SequenceLayer.portamento
    /* 0x18 */ AdsrSettings adsr; // @unverified: same offset/use as OoT's SequenceLayer.adsr
    /* 0x20 */ u16 portamentoTime; // @unverified: same offset/use as OoT's SequenceLayer.portamentoTime
    /* 0x22 */ s16 transposition; // @unverified: same offset/use as OoT's SequenceLayer.transposition
    /* 0x24 */ f32 freqScale; // @unverified: same offset/use as OoT's SequenceLayer.freqScale
    /* 0x28 */ f32 bend; // @unverified: same offset/use as OoT's SequenceLayer.bend
    /* 0x2C */ f32 velocitySquare2; // @unverified: same offset/use as OoT's SequenceLayer.velocitySquare2
    /* 0x30 */ f32 velocitySquare; // @unverified: same offset/use as OoT's SequenceLayer.velocitySquare
    /* 0x34 */ f32 noteVelocity; // @unverified: same offset/use as OoT's SequenceLayer.noteVelocity
    /* 0x38 */ f32 noteFreqScale; // @unverified: same offset/use as OoT's SequenceLayer.noteFreqScale
    /* 0x3C */ s16 shortNoteDefaultDelay; // @unverified: same offset/use as OoT's SequenceLayer.shortNoteDefaultDelay
    /* 0x3E */ s16 lastDelay; // @unverified: same offset/use as OoT's SequenceLayer.lastDelay
    /* 0x40 */ s16 delay; // @unverified: same offset/use as OoT's SequenceLayer.delay
    /* 0x42 */ s16 gateDelay; // @unverified: same offset/use as OoT's SequenceLayer.gateDelay
    /* 0x44 */ s16 duration; // @unverified: same offset/use as OoT's SequenceLayer.delay2
    /* 0x46 */ u8 unk_46[2];
    /* 0x48 */ struct Note* note; // @unverified: same offset/use as OoT's SequenceLayer.note
    /* 0x4C */ u8* instrument; // @unverified: same offset/use as OoT's SequenceLayer.instrument
    /* 0x50 */ SoundFontSound* sound; // @unverified: same offset/use as OoT's SequenceLayer.sound
    /* 0x54 */ struct SequenceChannel* channel; // @unverified: same offset/use as OoT's SequenceLayer.channel
    /* 0x58 */ SeqScriptState scriptState; // @unverified: same offset/use as OoT's SequenceLayer.scriptState
    /* 0x74 */ AudioListItem listItem; // @unverified: same offset/use as OoT's SequenceLayer.listItem (D_81287DC8 is the free list)
} SequenceLayer; // size = 0x84

typedef struct NoteSynthesisState {
    /* 0x00 */ u8 restart; // @unverified: same offset/use as OoT's NoteSynthesisState.atLoopPoint/stopLoop byte
    /* 0x01 */ u8 sampleDmaIndex; // @unverified: same offset/use as OoT's NoteSynthesisState.sampleDmaIndex
    /* 0x02 */ u8 prevHeadsetPanRight; // @unverified: same offset/use as OoT's NoteSynthesisState.prevHaasEffectRightDelaySize
    /* 0x03 */ u8 prevHeadsetPanLeft; // @unverified: same offset/use as OoT's NoteSynthesisState.prevHaasEffectLeftDelaySize
    /* 0x04 */ u8 reverbVol; // @unverified: same offset/use as OoT's NoteSynthesisState.reverbVol
    /* 0x05 */ u8 numParts; // @unverified: same offset/use as OoT's NoteSynthesisState.numParts
    /* 0x06 */ u16 samplePosFrac; // @unverified: same offset/use as OoT's NoteSynthesisState.samplePosFrac
    /* 0x08 */ s32 samplePosInt; // @unverified: same offset/use as OoT's NoteSynthesisState.samplePosInt
    /* 0x0C */ struct ZSynthesisBuffers* synthesisBuffers; // @unverified: same offset/use as OoT's NoteSynthesisState.synthesisBuffers
    /* 0x10 */ u16 curVolLeft; // @unverified: same offset/use as OoT's NoteSynthesisState.curVolLeft
    /* 0x12 */ u16 curVolRight; // @unverified: same offset/use as OoT's NoteSynthesisState.curVolRight
    /* 0x14 */ u8 unk_14[0xC];
} NoteSynthesisState; // size = 0x20

typedef struct NoteAttributes {
    /* 0x0 */ u8 reverbVol; // @unverified: same offset/use as OoT's NoteAttributes.reverb
    /* 0x1 */ u8 gain; // @unverified: same offset/use as OoT's NoteAttributes.gain
    /* 0x2 */ u8 pan; // @unverified: same offset/use as OoT's NoteAttributes.pan
    /* 0x3 */ Stereo stereo; // @unverified: same offset/use as OoT's NoteAttributes.stereo
    /* 0x4 */ f32 freqScale; // @unverified: same offset/use as OoT's NoteAttributes.freqScale
    /* 0x8 */ f32 velocity; // @unverified: same offset/use as OoT's NoteAttributes.velocity
    /* 0xC */ s16* filter; // @unverified: same offset/use as OoT's NoteAttributes.filter
} NoteAttributes; // size = 0x10

typedef struct AdsrState {
    /* 0x00 */ u8 action; // @unverified: same offset/use as OoT's AdsrState.action (0x10 = release)
    /* 0x01 */ u8 state; // @unverified: same offset/use as OoT's AdsrState.action.s.state
    /* 0x02 */ s16 envIndex; // @unverified: same offset/use as OoT's AdsrState.envIndex
    /* 0x04 */ s16 delay; // @unverified: same offset/use as OoT's AdsrState.delay
    /* 0x06 */ u8 unk_06[2];
    /* 0x08 */ f32 sustain; // @unverified: same offset/use as OoT's AdsrState.sustain
    /* 0x0C */ f32 velocity; // @unverified: same offset/use as OoT's AdsrState.velocity
    /* 0x10 */ f32 fadeOutVel; // @unverified: same offset/use as OoT's AdsrState.fadeOutVel
    /* 0x14 */ f32 current; // @unverified: same offset/use as OoT's AdsrState.current
    /* 0x18 */ f32 target; // @unverified: same offset/use as OoT's AdsrState.target
    /* 0x1C */ u8 unk_1C[4];
    /* 0x20 */ EnvelopePoint* envelope; // @unverified: same offset/use as OoT's AdsrState.envelope
} AdsrState; // size = 0x24

typedef struct VibratoState {
    /* 0x00 */ struct SequenceChannel* channel; // @unverified: same offset/use as OoT's VibratoState.channel
    /* 0x04 */ u32 time; // @unverified: same offset/use as OoT's VibratoState.time
    /* 0x08 */ s16* curve; // @unverified: same offset/use as OoT's VibratoState.curve
    /* 0x0C */ f32 extent; // @unverified: same offset/use as OoT's VibratoState.extent
    /* 0x10 */ f32 rate; // @unverified: same offset/use as OoT's VibratoState.rate
    /* 0x14 */ u8 active; // @unverified: same offset/use as OoT's VibratoState.active
    /* 0x16 */ u16 rateChangeTimer; // @unverified: same offset/use as OoT's VibratoState.rateChangeTimer
    /* 0x18 */ u16 extentChangeTimer; // @unverified: same offset/use as OoT's VibratoState.extentChangeTimer
    /* 0x1A */ u16 delay; // @unverified: same offset/use as OoT's VibratoState.delay
} VibratoState; // size = 0x1C

typedef struct NotePlaybackState {
    /* 0x00 */ u8 priority; // @unverified: same offset/use as OoT's NotePlaybackState.priority
    /* 0x01 */ u8 waveId; // @unverified: same offset/use as OoT's NotePlaybackState.waveId
    /* 0x02 */ u8 sampleCountIndex; // @unverified: same offset/use as OoT's NotePlaybackState.harmonicIndex
    /* 0x03 */ u8 fontId; // @unverified: same offset/use as OoT's NotePlaybackState.fontId
    /* 0x04 */ u8 unk_04;
    /* 0x05 */ u8 unk_05;
    /* 0x06 */ s16 adsrVolScale; // @unverified: same offset/use as OoT's NotePlaybackState.adsrVolScaleUnused
    /* 0x08 */ f32 portamentoFreqScale; // @unverified: same offset/use as OoT's NotePlaybackState.portamentoFreqScale
    /* 0x0C */ f32 vibratoFreqScale; // @unverified: same offset/use as OoT's NotePlaybackState.vibratoFreqScale
    /* 0x10 */ struct SequenceLayer* prevParentLayer; // @unverified: same offset/use as OoT's NotePlaybackState.prevParentLayer
    /* 0x14 */ struct SequenceLayer* parentLayer; // @unverified: same offset/use as OoT's NotePlaybackState.parentLayer
    /* 0x18 */ struct SequenceLayer* wantedParentLayer; // @unverified: same offset/use as OoT's NotePlaybackState.wantedParentLayer
    /* 0x1C */ NoteAttributes attributes; // @unverified: same offset/use as OoT's NotePlaybackState.attributes
    /* 0x2C */ AdsrState adsr; // @unverified: same offset/use as OoT's NotePlaybackState.adsr
    /* 0x50 */ Portamento portamento; // @unverified: same offset/use as OoT's NotePlaybackState.portamento
    /* 0x60 */ VibratoState vibratoState; // @unverified: same offset/use as OoT's NotePlaybackState.vibratoState
    /* 0x7C */ u8 unk_7C[8];
} NotePlaybackState; // size = 0x84

typedef struct NoteSampleState {
    /* 0x00 */ volatile u8 enabled : 1; // @unverified: same bit/use as OoT's NoteSampleState.bitField0.enabled
    /* 0x00 */ u8 needsInit : 1; // @unverified: same bit/use as OoT's NoteSampleState.bitField0.needsInit
    /* 0x00 */ u8 finished : 1; // @unverified: same bit/use as OoT's NoteSampleState.bitField0.finished
    /* 0x00 */ u8 envMixerNeedsInit : 1; // @unverified: same bit/use as OoT's NoteSampleState.bitField0.unused
    /* 0x00 */ u8 stereoStrongRight : 1; // @unverified: same bit/use as OoT's NoteSampleState.bitField0.strongRight
    /* 0x00 */ u8 stereoStrongLeft : 1; // @unverified: same bit/use as OoT's NoteSampleState.bitField0.strongLeft
    /* 0x00 */ u8 stereoHeadsetEffects : 1; // @unverified: same bit/use as OoT's NoteSampleState.bitField0.stereoHeadsetEffects
    /* 0x00 */ u8 usesHeadsetPanEffects : 1; // @unverified: same bit/use as OoT's NoteSampleState.bitField0.usesHeadsetPanEffects
    /* 0x01 */ u8 reverbIndex : 3; // @unverified: same bits/use as OoT's NoteSampleState.bitField1.reverbIndex
    /* 0x01 */ u8 bookOffset : 2; // @unverified: same bits/use as OoT's NoteSampleState.bitField1.bookOffset
    /* 0x01 */ u8 isSyntheticWave : 2; // @unverified: same bits as OoT's isSyntheticWave and hasTwoParts, read together
    /* 0x01 */ u8 bit1_0 : 1;
    /* 0x02 */ u8 gain; // @unverified: same offset/use as OoT's NoteSampleState.gain
    /* 0x03 */ u8 headsetPanRight; // @unverified: same offset/use as OoT's NoteSampleState.haasEffectRightDelaySize
    /* 0x04 */ u8 headsetPanLeft; // @unverified: same offset/use as OoT's NoteSampleState.haasEffectLeftDelaySize
    /* 0x05 */ u8 reverbVol; // @unverified: same offset/use as OoT's NoteSampleState.targetReverbVol
    /* 0x06 */ u8 harmonicIndexCurAndPrev; // @unverified: same offset/use as OoT's NoteSampleState.harmonicIndexCurAndPrev
    /* 0x07 */ u8 unk_07;
    /* 0x08 */ u16 targetVolLeft; // @unverified: same offset/use as OoT's NoteSampleState.targetVolLeft
    /* 0x0A */ u16 targetVolRight; // @unverified: same offset/use as OoT's NoteSampleState.targetVolRight
    /* 0x0C */ u8 unk_0C[4];
    /* 0x10 */ s32 tunedSample; // @unverified: same offset/use as OoT's NoteSampleState.tunedSample
    /* 0x14 */ s16* filter; // @unverified: same offset/use as OoT's NoteSampleState.filter
} NoteSampleState; // size = 0x18

typedef struct Note {
    /* 0x00 */ AudioListItem listItem; // @unverified: same offset/use as OoT's Note.listItem
    /* 0x10 */ NoteSynthesisState synthesisState; // @unverified: same offset/use as OoT's Note.synthesisState
    /* 0x30 */ NotePlaybackState playbackState; // @unverified: same offset/use as OoT's Note.playbackState
    /* 0xB4 */ NoteSampleState sampleState; // @unverified: same offset/use as OoT's Note.sampleState
} Note; // size = 0xCC



// Yay0 files in fragment1_misc_yay0 (retail ROM 0xADC80), named by their retail ROM address.
// Offsets from the segment rather than fixed addresses, so they follow it when an earlier
// segment changes size.
#define D_F4130 (fragment1_misc_yay0_ROM_START + 0x464B0)

#define D_F4920 (fragment1_misc_yay0_ROM_START + 0x46CA0)

#define D_F5450 (fragment1_misc_yay0_ROM_START + 0x477D0)

#define D_FDE40 (fragment1_misc_yay0_ROM_START + 0x501C0)

#define D_102BA0 (fragment1_misc_yay0_ROM_START + 0x54F20)

typedef struct Particle Particle;
#include "controller.h"

extern u8 gGbMemoryMap[];

extern u16 D_81138A96;

extern u32 D_81138A90;

extern s16 D_81180700;

extern s16 D_81180702;

extern u8* D_81180704;

extern GbApuChannelState gGbApuSquare1;

extern GbApuChannelState gGbApuSquare2;

extern GbApuChannelState gGbApuWave;

extern GbApuChannelState gGbApuNoise;

extern GbApuWrite D_81180848[GB_APU_WRITE_QUEUE_LEN];

extern u8 D_81182468[0x40];

extern s32 D_811824A8;

extern GbApuStatusOverride D_811824B8;

extern OSMesgQueue D_811824C0;

extern OSMesg D_811824D8[GB_APU_WRITE_QUEUE_LEN + 1];

extern volatile s32 sGbAudioFrameCount;

extern s32 D_812346D0;

extern s32 sGbAudioBufferIndex;

extern s16* sGbAudioBuffers[3];

extern s16 sGbAudioBufferLengths[3];

extern u32 D_812346F4;

extern SequencePlayer D_81287B08_jp[];

extern OSMesgQueue* D_812896A0_jp;

extern s32 D_81138A70;

extern u8 D_81138A80[0x10];

extern s32 D_81138AD0;

extern s32 D_81138AD4;

extern s32 D_81138AD8;

extern u16 D_81138ADC[32];

extern u8 D_81138B1C[0x10];

extern u8 D_81138B2C[0x4];

extern u8 D_81138B30[0x10];

extern u8 D_81138B40[0x10];

extern u32 D_81138B50[9];

extern s32 D_81138B80;

extern GbAudioStream D_81138B84;

extern GbAudioStream D_81138B8C;

extern GbAudioStream D_81138B94;

extern u8 D_8115265C[0x24];

extern u8 D_81152680[0x24];

extern struct AudioListItem D_81287DC8;

extern SequenceLayer D_81287C68[1];

extern s16 D_81288D6C;

// audio_load_2.c
void AudioLoad_Unused1A(void);
void AudioLoad_Unused1B(void);
void AudioLoad_Unused2(void);
void AudioLoad_ProcessLoads(s32 reset_status);
void AudioLoad_ProcessSlowLoads(s32 reset_status);
s32 AudioLoad_ProcessSamplePreloads(s32 arg0);
void AudioLoad_ProcessAsyncLoads(s32 arg0);

// audio_thread.c
s32 AudioThread_GetFontsForSequence(s32 arg0);
void AudioThread_QueueCmdS32(s32 cmd, s32 arg1);
void AudioThread_Unused1(void);
void AudioThread_Unused4(void);
void AudioThread_InitMesgQueuesImpl(void);
void AudioThread_InitMesgQueues(void);
void AudioThread_Unused5(void);
void AudioThread_ResetExternalPool(void);

// audio_playback.c
void AudioPlayback_ProcessNotes(void);

// audio_seqplayer.c
void AudioSeq_SeqLayerDisable(SequenceLayer* arg0);
void AudioSeq_SeqLayerFree(s8** arg0, s32 arg1);
void AudioSeq_SequenceChannelDisable(SequenceChannel*);
void AudioSeq_SequenceChannelEnable(SequencePlayer* seq_player, u8 channel_index, void* script);
s32 AudioSeq_AudioListPopBack(AudioListItem* list);
void AudioSeq_InitLayerFreelist(void);
u8 AudioSeq_ScriptReadU8(u8 **arg0);
s16 AudioSeq_ScriptReadS16(u8** arg0);
void AudioSeq_SeqLayerProcessScriptStep1(SequenceLayer* arg0);

// audio_seqplayer_2.c
/* Sets SequenceChannel.volume (+0x2C) to arg1 / 127.0f (0.0 to about 2.0). */
void AudioSeq_SequenceChannelSetVolume(u8* arg0, u8 arg1);

// audio_seqplayer_3.c
void Audio_ScheduleProcessCmds(void);
void Audio_QueueGlobalMute(void);
void Audio_QueueGlobalUnmute(void);
void Audio_FadeSeqPlayerVolumeToPercent(s32 arg0, s32 arg1, u8 arg2);
void Audio_FadeSeqPlayerVolumeBack(s32 arg0, s32 arg1);
void Audio_Unused1(void);
void Audio_QueueChannelMute(s32 arg0, s32 arg1, s32 arg2);
void Audio_InitSfxSeqPlayer(void);
void Audio_SetSfxSeqPlayerIo(s32 arg0, s32 arg1, s32 arg2);
void Audio_ClearSfxSeqPlayerIo4(void);
void GbApu_SetNoiseLoopBounds(s32 arg0, s32 arg1);
void AudioSeq_SequencePlayerProcessSequence(struct SequencePlayer*);
void AudioSeq_ProcessSequences(s32 arg0);
void AudioSeq_SkipForwardSequence(s8 *arg0);

// gb_apu.c
u32 GbApu_FreqToStep(u16 arg0);
u32 GbApu_ConfigureNoiseTable(u16 arg0, u16 arg1, u16 arg2);
u32 GbApu_FreqToStep2x(u16 arg0);
void GbApu_Unused1(void);
s32 GbApu_GetWaveformSample(u32 arg0, s32 arg1);
u16 GbApu_UpdateSquare1Channel(void);
s32 GbApu_UpdateSquare2Channel(void);
u16 GbApu_UpdateWaveChannel(void);
u16 GbApu_Unused2(void);
u16 GbApu_UpdateNoiseChannel(void);
void GbApu_WriteRegister(u16 reg, u8 value);
void GbApu_RenderBuffer(s32 numSamples, s16* out);
void GbApu_QueueRegisterWrite(u16 arg0, u8 arg1, u16 arg2);
void GbApu_ResetChannels(void);
void GbAudio_RampToSilence(void);
void GbAudio_FrameStub(void);
u8 GbMem_ReadIoRegister(u16 arg0);
void GbAudio_SetMenuSoundMode(s32 arg0);
void GbAudio_PlayMenuSound(s32 arg0);

// gb_apu_2.c
void GbAudio_RenderMenuSounds(s32 numSamples, s16* out);
void GbAudio_Unused1(UNUSED s32 arg0);
void GbAudio_Unused2(UNUSED s32 arg0);

#endif // FRAGMENT_AUDIO_AND_GB_EMULATOR_H
