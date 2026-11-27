/* JP-only TU, ported from pokestadium0 (Stadium Zero, fragment000_80C00). */
/* Audio driver, matching the end of OoT heap.c and the start of OoT load.c (src/audio/internal) in
 * function order. The driver is Nintendo's "Nas" library (original names Nas_*); this revision
 * sits between SM64 Shindou and OoT. See docs/annotation/audio-driver.md for the per-function
 * mapping. */
/* Fixed-layout split; original translation-unit boundaries remain unconfirmed. */
/* The US and PAL builds have this code in src/fragments/gb_tower_emulator/gb_tower_emulator_86920.c. */

#include "global.h"
#include "gb_tower_emulator_jp.h"

extern AudioBufferParameters sGbAudioOutputConfig;
extern volatile s32 sGbAudioFrameCount;
extern s32 D_812346D0;
extern s32 sGbAudioBufferIndex;
extern s16* sGbAudioBuffers[3];
extern s16 sGbAudioBufferLengths[3];
extern u32 D_812346F4;
extern volatile s32 D_812346FC;

#include "m2c_macros.h"

typedef struct SampleCacheEntry8110BA8C {
    /* 0x00 */ s8 inUse;
    /* 0x01 */ s8 origMedium;
    /* 0x02 */ s8 sampleBankId;
    /* 0x04 */ s32 sampleAddr;
    /* 0x08 */ u8* allocatedAddr;
    /* 0x0C */ s32 unk_0C;
    /* 0x10 */ u32 size;
} SampleCacheEntry8110BA8C; // size = 0x14

typedef struct SampleCache8110BA8C {
    /* 0x00 */ u8* startRamAddr;
    /* 0x04 */ u8* curRamAddr;
    /* 0x08 */ s32 size;
    /* 0x0C */ s32 numAllocatedEntries;
    /* 0x10 */ SampleCacheEntry8110BA8C entries[1];
    /* 0x24 */ s32 numEntries;
} SampleCache8110BA8C;

typedef struct {
    u8 pad0 : 4;
    u8 medium : 2; // @unverified: bits 2-3 of Sample byte 0; AudioHeap_UnapplySampleCache restores the original medium here (OoT's Sample.medium)
    u8 pad1 : 2;
} SampleMedium8110D4E0;

typedef struct SampleDma8110C1C0 {
    /* 0x00 */ u8* ramAddr;
    /* 0x04 */ u32 devAddr;
    /* 0x08 */ u16 sizeUnused;
    /* 0x0A */ u16 size;
    /* 0x0C */ u8 unused;
    /* 0x0D */ u8 reuseIndex; // @unverified: same offset/use as OoT's SampleDma.reuseIndex
    /* 0x0E */ u8 ttl; // @unverified: same offset/use as OoT's SampleDma.ttl (counted down in AudioLoad_DecreaseSampleDmaTtls)
} SampleDma8110C1C0; // size = 0x10
extern SampleDma8110C1C0* D_81288B28;

typedef struct Sample8110CD0C {
    /* 0x00 */ u32 unk00_0 : 4;
    /* 0x00 */ u32 medium : 2; // @unverified: Sample.medium bits (0 = RAM after the load), as in OoT's AudioLoad_SyncLoadSample
    /* 0x00 */ u32 unk00_6 : 1;
    /* 0x00 */ u32 isRelocated : 1; // @unverified: bit 7 of Sample byte 0, OoT's Sample.isRelocated (load only relocated samples)
    /* 0x00 */ u32 size : 24; // @unverified: Sample.size (24 bits), passed as the DMA size
    /* 0x04 */ s32 sampleAddr; // @unverified: Sample.sampleAddr: ROM address in, RAM address out
} Sample8110CD0C;

typedef struct SequencePlayerFromSkipTicks {
    /* 0x000 */ s32 skipTicks; // @unverified: this view starts at SequencePlayer+0xD8, OoT's SequencePlayer.skipTicks (set by AudioLoad_SyncInitSeqPlayerSkipTicks)
    /* 0x004 */ char unk004[0x158];
} SequencePlayerFromSkipTicks; // size = 0x15C
extern SequencePlayerFromSkipTicks D_81287BE0[];

typedef struct IoMesg8110DBA0 {
    /* 0x00 */ u16 type;
    /* 0x02 */ u8 pri;
    /* 0x03 */ u8 status;
    /* 0x04 */ void* retQueue;
    /* 0x08 */ void* dramAddr;
    /* 0x0C */ u32 devAddr;
    /* 0x10 */ u32 size;
} IoMesg8110DBA0;

typedef struct SlowLoads8110E500 {
    /* 0x00 */ s32 index;
    /* 0x04 */ AudioSlowLoad slots[2];
} SlowLoads8110E500;

typedef struct Sample8110E71C {
    /* 0x00 */ u8 flags; // @unverified: Sample byte 0 (codec/medium/flags); AudioLoad_FinishSlowLoad clears medium (bits 2-3) to RAM
    /* 0x01 */ u8 unk_01[3];
    /* 0x04 */ s32 sampleAddr; // @unverified: Sample.sampleAddr, pointed at the slow-loaded copy (OoT's AudioLoad_FinishSlowLoad)
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ s32 unk_0C;
} Sample8110E71C;

typedef struct Entry8110EABC {
    /* 0x00 */ s8 active;
    /* 0x01 */ s8 state;
    /* 0x02 */ s8 medium;
    /* 0x04 */ s32 ramAddr;
    /* 0x08 */ s32 romAddr;
    /* 0x0C */ s32 curAddr;
    /* 0x10 */ u32 size;
    /* 0x14 */ u32 chunkSize;
    /* 0x18 */ OSMesg info;
    /* 0x1C */ OSMesgQueue* doneQueue;
    /* 0x20 */ OSMesgQueue queue;
    /* 0x38 */ OSMesg msg;
    /* 0x3C */ char unk3C[0x18];
} Entry8110EABC; // size = 0x54
extern Entry8110EABC D_81288A20[1];

typedef struct RelocInfo8110EF7C {
    /* 0x00 */ s32 sampleBankId1;
    /* 0x04 */ s32 sampleBankId2;
    /* 0x08 */ s32 baseAddr1;
    /* 0x0C */ s32 baseAddr2;
    /* 0x10 */ u32 medium1;
    /* 0x14 */ u32 medium2;
} RelocInfo8110EF7C;

typedef struct {
    u32 bits31_28 : 4;
    u32 medium : 2; // @unverified: Sample.medium bits (OoT layout)
    u32 unk_bit26 : 1; // @unverified: Sample bit 26, same position as OoT's Sample.unk_bit26
    u32 bit24 : 1;
    u32 size : 24; // @unverified: Sample.size (24 bits)
} SampleFlags8110F64C;

typedef struct Sample8110EE3C {
    /* 0x00 */ u32 codec : 4;
    /* 0x00 */ u32 medium : 2;
    /* 0x00 */ u32 unk_bit26 : 1;
    /* 0x00 */ u32 isRelocated : 1;
    /* 0x00 */ u32 size : 24;
    /* 0x04 */ s32 sampleAddr;
    /* 0x08 */ s32 loop;
    /* 0x0C */ s32 book;
} Sample8110EE3C;

typedef struct Sample8110EF7C {
    /* 0x00 */ u32 codec : 4;
    /* 0x00 */ u32 medium : 2;
    /* 0x00 */ u32 unk_bit26 : 1;
    /* 0x00 */ u32 isRelocated : 1;
    /* 0x00 */ u32 size : 24;
    /* 0x04 */ s32 sampleAddr;
} Sample8110EF7C;

typedef struct Item8110F4D0 {
    /* 0x0 */ s32 unk0;
    /* 0x4 */ s32 key;
} Item8110F4D0;


u32 GbApu_AllocZeroed(AudioAllocPool* arg0, s32 arg1);
s32 GbApu_Alloc(AudioAllocPool* arg0, s32 arg1);
void AudioHeap_InitPool(AudioAllocPool* arg0, s32 arg1, s32 arg2);

extern u8 D_81287AF4[1];
extern u8 D_81287AF5[1];
extern u8 D_81287AF6[1];

/* Declarations copied from audio_heap.c (Stadium 1: gb_tower_emulator_86920.c). */
s32 AudioHeap_SearchCaches(s32, M2C_UNK, s32);
s32 AudioPlayback_NoteDisable(Note*);
s32 AudioPlayback_AudioListRemove(Note*);
s32 AudioSeq_AudioListPushBack(s8*, Note*);
extern Note* D_81287B04;
extern s8 D_81287DD8;
extern s32 D_81288D88;
M2C_UNK AudioSeq_SequencePlayerDisable(M2C_UNK *);                   /* extern */
extern s8 D_81287B08;
extern s16 D_81288D6C;
void AudioHeap_InitPool(AudioAllocPool* arg0, s32 arg1, s32 arg2);
extern M2C_UNK D_81287730;
extern u8 D_81287750[];

extern s32 D_81255B6C;
extern s32 D_81287740;
s32 GbApu_Alloc(AudioAllocPool* arg0, s32 arg1);
void AudioHeap_InitPool(AudioAllocPool* arg0, s32 arg1, s32 arg2);
extern M2C_UNK D_81287730;
extern M2C_UNK D_81287760;
extern M2C_UNK D_81287790;
s32 GbApu_Alloc(AudioAllocPool* arg0, s32 arg1);
void AudioHeap_InitPool(AudioAllocPool* arg0, s32 arg1, s32 arg2);
extern M2C_UNK D_81287790;
extern M2C_UNK D_812877A0;
extern M2C_UNK D_812877B0;
s32 GbApu_Alloc(AudioAllocPool* arg0, s32 arg1);
void AudioHeap_InitPool(AudioAllocPool* arg0, s32 arg1, s32 arg2);
extern M2C_UNK D_812877A0;
extern M2C_UNK D_812877C0;
extern M2C_UNK D_812877C4;
extern M2C_UNK D_81287820;
extern M2C_UNK D_81287824;
extern M2C_UNK D_81287880;
extern M2C_UNK D_81287884;
s32 GbApu_Alloc(AudioAllocPool* arg0, s32 arg1);
void AudioHeap_InitPool(AudioAllocPool* arg0, s32 arg1, s32 arg2);
extern M2C_UNK D_812877B0;
extern M2C_UNK D_812877E0;
extern M2C_UNK D_812877E4;
extern M2C_UNK D_81287840;
extern M2C_UNK D_81287844;
extern M2C_UNK D_812878A0;
extern M2C_UNK D_812878A4;
extern u8 D_81287AF4[1];
extern u8 D_81287AF5[1];
extern u8 D_81287AF6[1];
void AudioLoad_SetFontLoadStatus(s32 arg0, s32 arg1);
s32 AudioHeap_SearchRegularCaches(s32 arg0, s32 arg1, s32 arg2);
s32 AudioHeap_SearchPermanentCache(s32 arg0, s32 arg1);
extern s16 D_8123A648[];
extern s16 D_8123A838[];
extern SynthesisReverb D_812875B0[];
extern s8 D_8128771B;
extern s16 D_81288D62;
extern AudioSpec D_81255B34[];
extern s16 D_81255B68;
extern u16 D_81287718;
extern s16 D_8128771C;
extern void* D_81287728;
extern s32 D_81287738;
extern AudioSessionPoolSplit D_81287AC0;
extern AudioCachePoolSplit D_81287AD0;
extern AudioCommonPoolSplit D_81287AD8;
extern AudioCommonPoolSplit D_81287AE8;
extern u8 D_81287AF8;
extern s32 D_81288D84;
extern s16 D_81288D8C;
extern void* D_81288DA0[2];
extern s32 D_81288E54;
extern f32 D_81288E50;
extern u32 D_81288B2C;
extern s32 D_81288A1C;
void AudioLoad_InitSampleDmaBuffers(s32 num_notes);
void AudioSeq_ResetSequencePlayer(s32);
void osWritebackDCacheAll(void);
void AudioLoad_LoadPermanentSamples(void);
void AudioPlayback_InitNoteFreeList(void);
void AudioPlayback_NoteInitAll(void);
void AudioSeq_InitSequencePlayerChannels(s32);
void AudioSeq_InitSequencePlayers(void);
#define EXTERNAL_POOL_AFF0 (*(AudioAllocPool*)&D_81287740)



extern AudioPermanentCache D_812878E0;
/**
 * Returns the permanent cache entry for table `arg0`, id `arg1`, or 0.
 * Corresponds to OoT AudioHeap_SearchPermanentCache (original name EmemOnCheck).
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment guessed from the code.
 */
s32 AudioHeap_SearchPermanentCache(s32 arg0, s32 arg1) {
    s32 i;

    for (i = 0; i < D_812878E0.pool.numEntries; i++) {
        if (arg0 == D_812878E0.entries[i].tableType && arg1 == D_812878E0.entries[i].id) {
            return D_812878E0.entries[i].ramAddr;
        }
    }

    return 0;
}

/**
 * Allocates `size` bytes in the permanent cache for table `x`, id `y`.
 * Corresponds to OoT AudioHeap_AllocPermanent (original name EmemAlloc).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
s32 AudioHeap_AllocPermanent(s32 x, s32 y, s32 size) {
    UNUSED s32 pad; // FAKE: stack padding
    s32 index = D_812878E0.pool.numEntries;

    D_812878E0.entries[index].ramAddr = GbApu_Alloc((M2C_UNK*)&D_812878E0, size);
    if (D_812878E0.entries[index].ramAddr == 0) {
        return 0;
    }
    D_812878E0.entries[index].tableType = x;
    D_812878E0.entries[index].id = y;
    D_812878E0.entries[index].size = size;
    // No return here; the original falls off the end with the allocation still in v0.
}

SampleCacheEntry8110BA8C* AudioHeap_AllocTemporarySampleCacheEntry(u32 size);
/**
 * Allocates a temporary sample cache entry of `arg0` bytes for font `arg1`.
 * Corresponds to OoT AudioHeap_AllocSampleCache (original name Nas_Alloc_Single). OoT merges this
 * and AudioHeap_AllocPersistentSampleCache into one AudioHeap_AllocSampleCache with a cache
 * argument. SM64 Shindou keeps two functions, like Zero (func_sh_802f1d40, func_sh_802f1d90).
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment guessed from the code.
 */
s32 AudioHeap_AllocTemporarySampleCache(s32 arg0, s32 arg1, s32 arg2, s8 arg3) {
    s8* p = (s8*)AudioHeap_AllocTemporarySampleCacheEntry(arg0);

    if (p != NULL) {
        M2C_FIELD(p, u8*, 2) = arg1;
        M2C_FIELD(p, s32*, 0xC) = arg2;
        M2C_FIELD(p, s8*, 1) = arg3;
        return M2C_FIELD(p, s32*, 8);
    }
    return 0;
}

s8* AudioHeap_AllocPersistentSampleCacheEntry(s32 arg0);
/**
 * Allocates a persistent sample cache entry of `arg0` bytes for font `arg1`.
 * Corresponds to OoT AudioHeap_AllocSampleCache (original name Nas_Alloc_Single). See
 * AudioHeap_AllocTemporarySampleCache: OoT merges the two.
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment guessed from the code.
 */
s32 AudioHeap_AllocPersistentSampleCache(s32 arg0, s32 arg1, s32 arg2, s8 arg3) {
    s8* p = AudioHeap_AllocPersistentSampleCacheEntry(arg0);

    if (p != NULL) {
        M2C_FIELD(p, u8*, 2) = arg1;
        M2C_FIELD(p, s32*, 0xC) = arg2;
        M2C_FIELD(p, s8*, 1) = arg3;
        return M2C_FIELD(p, s32*, 8);
    }
    return 0;
}

s32 GbApu_Alloc(AudioAllocPool* arg0, s32 arg1);
void AudioHeap_InitPool(AudioAllocPool* arg0, s32 arg1, s32 arg2);
extern M2C_UNK D_812877A0;
extern M2C_UNK D_812877B0;
extern M2C_UNK D_81287A70;
extern s32 D_81287A78;
extern s32 D_81287A94;

extern SampleCache8110BA8C D_81287A98;
extern s32 D_81287AA0;
extern s32 D_81287ABC;

/**
 * Sets up the persistent and temporary sample caches from the misc pool.
 * Corresponds to OoT AudioHeap_InitSampleCaches (original name Nas_Init_Single). Zero allocates
 * with GbApu_Alloc; OoT tries the external pool first.
 * Seen in play in pokestadium0 (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play in pokestadium0 (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in
 * the Transfer Pak; called from AudioHeap_Init.
 */
void AudioHeap_InitSampleCaches(s32 arg0, s32 arg1) {
    s32 temp_v0;
    s32 temp_v0_2;

    temp_v0 = GbApu_Alloc(&D_812877A0, arg0);
    if (temp_v0 == 0) {
        D_81287A78 = 0;
    } else {
        AudioHeap_InitPool(&D_81287A70, temp_v0, arg0);
    }
    temp_v0_2 = GbApu_Alloc(&D_812877B0, arg1);
    if (temp_v0_2 == 0) {
        D_81287AA0 = 0;
    } else {
        AudioHeap_InitPool((M2C_UNK*)&D_81287A98, temp_v0_2, arg1);
    }
    D_81287A94 = 0;
    D_81287ABC = 0;
}

extern AudioPreloadReq D_81288018[];
extern s32 D_81288A1C;
void AudioHeap_DiscardSampleCacheEntry(SampleCacheEntry8110BA8C* entry);

/**
 * Takes `size` bytes from the temporary sample cache, discarding the entries it overwrites.
 * Corresponds to OoT AudioHeap_AllocTemporarySampleCacheEntry (original name
 * __Nas_Alloc_Single_Auto_Inner).
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment guessed from the code.
 */
SampleCacheEntry8110BA8C* AudioHeap_AllocTemporarySampleCacheEntry(u32 size) {
    u8* alloc_after;
    u8* alloc_before;
    void* mem;
    s32 index;
    s32 i;
    SampleCacheEntry8110BA8C* entry;
    AudioPreloadReq* preload;
    SampleCache8110BA8C* cache;
    u8* start_ram_addr;
    u8* end_ram_addr;

    cache = &D_81287A98;
    alloc_before = cache->curRamAddr;
    mem = (void*)GbApu_Alloc((M2C_UNK*)cache, size);
    if (mem == NULL) {
        u8* old = cache->curRamAddr;

        cache->curRamAddr = cache->startRamAddr;
        mem = (void*)GbApu_Alloc((M2C_UNK*)cache, size);
        if (mem == NULL) {
            cache->curRamAddr = old;
            return NULL;
        }
        alloc_before = cache->startRamAddr;
    }

    alloc_after = cache->curRamAddr;

    index = -1;
    for (i = 0; i < D_81288A1C; i++) {
        preload = &D_81288018[i];
        if (preload->isFree == 0) {
            start_ram_addr = preload->ramAddr;
            end_ram_addr = preload->ramAddr + preload->sample->size - 1;

            if (end_ram_addr < alloc_before && start_ram_addr < alloc_before) {
                continue;
            }
            if (end_ram_addr >= alloc_after && start_ram_addr >= alloc_after) {
                continue;
            }

            preload->isFree = 1;
        }
    }

    for (i = 0; i < cache->numEntries; i++) {
        if (!cache->entries[i].inUse) {
            continue;
        }

        start_ram_addr = cache->entries[i].allocatedAddr;
        end_ram_addr = start_ram_addr + cache->entries[i].size - 1;

        if (end_ram_addr < alloc_before && start_ram_addr < alloc_before) {
            continue;
        }
        if (end_ram_addr >= alloc_after && start_ram_addr >= alloc_after) {
            continue;
        }

        AudioHeap_DiscardSampleCacheEntry(&cache->entries[i]);
        if (index == -1) {
            index = i;
        }
    }

    if (index == -1) {
        index = cache->numEntries++;
    }

    entry = &cache->entries[index];
    entry->inUse = 1;
    entry->allocatedAddr = mem;
    entry->size = size;

    return entry;
}

/* First byte of a Sample; AudioHeap_UnapplySampleCache restores its medium (bits 2-3). */

extern SoundFont* D_81288D50;
extern AudioTable* D_81288D40;
s32 AudioLoad_IsFontLoadComplete(s32 arg0);
u8* AudioPlayback_GetInstrumentInner(s32, s32);
u8* AudioPlayback_GetDrum(s32, s32);

void AudioHeap_UnapplySampleCache(u8* arg0, SampleMedium8110D4E0* arg1);

/**
 * Points every font that used a cached sample back at the original sample.
 * Corresponds to OoT AudioHeap_DiscardSampleCacheEntry (original name __KillSwMember). OoT moves
 * the per-font loop into AudioHeap_UnapplySampleCacheForFont (__SearchBank).
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment guessed from the code.
 */
void AudioHeap_DiscardSampleCacheEntry(SampleCacheEntry8110BA8C* entry) {
    u8* inst;
    s32 num_fonts;
    s32 font_id;
    s32 sample_bank_id1;
    s32 sample_bank_id2;
    s32 inst_id;
    u8* drum;
    s32 drum_id;

    num_fonts = *(s16*)D_81288D40;
    for (font_id = 0; font_id < num_fonts; font_id++) {
        sample_bank_id1 = D_81288D50[font_id].sampleBankId1;
        sample_bank_id2 = D_81288D50[font_id].sampleBankId2;
        if (((sample_bank_id1 != 0xFF) && (entry->sampleBankId == sample_bank_id1)) ||
            ((sample_bank_id2 != 0xFF) && (entry->sampleBankId == sample_bank_id2)) || entry->sampleBankId == 0) {
            if (AudioHeap_SearchCaches(1, 2, font_id) != 0) {
                if (AudioLoad_IsFontLoadComplete(font_id) != 0) {
                    for (inst_id = 0; inst_id < D_81288D50[font_id].numInstruments; inst_id++) {
                        inst = AudioPlayback_GetInstrumentInner(font_id, inst_id);
                        if (inst != NULL) {
                            if (inst[1] != 0) {
                                AudioHeap_UnapplySampleCache((u8*)entry, M2C_FIELD(inst, SampleMedium8110D4E0**, 8));
                            }
                            if (inst[2] != 0x7F) {
                                if (1) { // FAKE
                                    AudioHeap_UnapplySampleCache((u8*)entry, M2C_FIELD(inst, SampleMedium8110D4E0**, 0x18));
                                }
                            }
                            AudioHeap_UnapplySampleCache((u8*)entry, M2C_FIELD(inst, SampleMedium8110D4E0**, 0x10));
                        }
                    }
                    for (drum_id = 0; drum_id < D_81288D50[font_id].numDrums; drum_id++) {
                        drum = AudioPlayback_GetDrum(font_id, drum_id);
                        if (drum != NULL) {
                            AudioHeap_UnapplySampleCache((u8*)entry, M2C_FIELD(drum, SampleMedium8110D4E0**, 4));
                        }
                    }
                }
            }
        }
    }
}

/**
 * If sample arg1 still points at the cached copy in sample cache entry arg0 (entry +0x08 equals
 * sample->sampleAddr at +0x04), points it back at the original address (entry +0x0C) and restores
 * the original medium (entry +0x01) into bits 2-3 of the sample's first byte (a bit-field write:
 * the retail code masks with 0xFFF3 / 0x0C).
 * Corresponds to OoT AudioHeap_UnapplySampleCache (original name __RomAddrSet).
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name guessed from the code; comment kept.
 */
void AudioHeap_UnapplySampleCache(u8* arg0, SampleMedium8110D4E0* arg1) {
    if ((arg1 != NULL) && (M2C_FIELD(arg0, s32*, 8) == M2C_FIELD(arg1, s32*, 4))) {
        M2C_FIELD(arg1, s32*, 4) = M2C_FIELD(arg0, s32*, 0xC);
        arg1->medium = M2C_FIELD(arg0, s8*, 1);
    }
}

s32 GbApu_Alloc(AudioAllocPool* arg0, s32 arg1);
/**
 * Takes `arg0` bytes from the persistent sample cache.
 * Corresponds to OoT AudioHeap_AllocPersistentSampleCacheEntry (original name
 * __Nas_Alloc_Single_Stay_Inner).
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment guessed from the code.
 */
s8 *AudioHeap_AllocPersistentSampleCacheEntry(s32 arg0)
{
  s32 temp_v0_2;
  s8 *temp_v0;
  s32 *new_var; // FAKE: permuter temp
  new_var = (s32 *) (&D_81287A94); // FAKE: permuter temp
  temp_v0_2 = GbApu_Alloc(&(*((s8 *) (&D_81287A70))), arg0); // FAKE: decomp slop, raw pointer casts stand in for struct fields
  if (temp_v0_2 == 0)
  {
    return 0;
  }
  temp_v0 = (((*new_var) * 0x14) + 0x10) + (&(*((s8 *) (&D_81287A70)))); // FAKE: decomp slop, raw pointer casts stand in for struct fields
  *((s8 *) (((s8 *) temp_v0) + 0)) = 1; // FAKE: decomp slop, raw pointer casts stand in for struct fields
  *((s32 *) (((s8 *) temp_v0) + 8)) = temp_v0_2; // FAKE: decomp slop, raw pointer casts stand in for struct fields
  *((s32 *) (((s8 *) temp_v0) + 0x10)) = arg0; // FAKE: decomp slop, raw pointer casts stand in for struct fields
  *new_var += 1; // FAKE: permuter temp
  return temp_v0;
}

/**
 * Discards the sample cache entries of fonts that are no longer loaded.
 * Corresponds to OoT AudioHeap_DiscardSampleCaches (original name Emem_KillSwMember). OoT moves the
 * per-font loop into AudioHeap_DiscardSampleCacheForFont.
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment guessed from the code.
 */
void AudioHeap_DiscardSampleCaches(void) {
    UNUSED s32 pad2[1]; // FAKE: stack padding
    s32 j;
    s32 count;
    UNUSED s32 pad[5]; // FAKE: stack padding
    s32 a;
    s32 b;
    s8* slot;
    s32 k;
    s32 m;
    s32 i;
    u8* p;
    s32 off;

    count = *(s16*)D_81288D40;
    for (i = 0; i < count; i++) {
        a = D_81288D50[i].sampleBankId1;
        b = D_81288D50[i].sampleBankId2;
        if ((a != 0xFF) || (b != 0xFFU)) {
            off = 0x12; // FAKE: the same offset through a local, only for register allocation
            if ((AudioHeap_SearchCaches(1, 3, i) != 0) && (AudioLoad_IsFontLoadComplete(i) != 0)) {
                slot = (s8*)&D_81287A70;
                for (j = 0; j < D_81287A94; j++, slot += 0x14) {
                    if ((a != slot[off]) && (b != slot[0x12]) && (slot[0x12] != 0)) {
                        break;
                    }
                    { // FAKE: bare block, only for register allocation
                        for (k = 0; k < D_81288D50[i].numInstruments; k++) {
                            p = AudioPlayback_GetInstrumentInner(i, k);
                            if (p != NULL) {
                                if (p[1] != 0) {
                                    AudioHeap_UnapplySampleCache(slot + 0x10, M2C_FIELD(p, SampleMedium8110D4E0**, 8));
                                }
                                if (p[2] != 0x7F) {
                                    AudioHeap_UnapplySampleCache(slot + 0x10, M2C_FIELD(p, SampleMedium8110D4E0**, 0x18));
                                }
                                AudioHeap_UnapplySampleCache(slot + 0x10, M2C_FIELD(p, SampleMedium8110D4E0**, 0x10));
                            }
                        }
                        for (m = 0; m < D_81288D50[i].numDrums; m++) {
                            p = AudioPlayback_GetDrum(i, m);
                            if (p != NULL) {
                                AudioHeap_UnapplySampleCache(slot + 0x10, M2C_FIELD(p, SampleMedium8110D4E0**, 4));
                            }
                        }
                    }
                }
            }
        }
    }
}

extern u32 D_81288B2C;
extern u32 D_81288B30;
extern s32 D_81288B34;
extern u8 D_81288B38[];
extern u8 D_81288C38[];
extern u8 D_81288D3A;
extern u8 D_81288D3B;
/**
 * Ticks down the time-to-live of each sample DMA buffer and returns expired ones to the free list.
 * Corresponds to OoT AudioLoad_DecreaseSampleDmaTtls (original name Nas_WaveDmaFrameWork).
 * No caller (2026-11-13): nothing in the built ROM jumps to it or holds its address (no jal or j,
 * no pointer word, no lui/addiu pair in any fragment), so play cannot reach it.
 * @unverified Name and comment guessed from the code.
 */
void AudioLoad_DecreaseSampleDmaTtls(void) {
    u32 i;
    SampleDma8110C1C0* t;
    for (i = 0; i < D_81288B30; i++) {
        t = &D_81288B28[i];
        if (t->ttl != 0) {
            if (--t->ttl == 0) {
                t->reuseIndex = D_81288D3A;
                D_81288B38[D_81288D3A++] = i;
            }
        }
    }
    for (i = D_81288B30; i < D_81288B2C; i++) {
        t = &D_81288B28[i];
        if (t->ttl != 0) {
            if (--t->ttl == 0) {
                t->reuseIndex = D_81288D3B;
                D_81288C38[D_81288D3B++] = i;
            }
        }
    }
    D_81288B34 = 0;
}

extern u8 D_81288D38;
extern u8 D_81288D39;
extern u8 D_81288AD8[];
extern u32 D_81288D94;
extern OSMesgQueue D_81288AB8;
extern s8 D_8123AF3C[];
s32 AudioLoad_Dma(IoMesg8110DBA0* mb, s32 pri, s32 direction, u32 dev_addr, void* dram_addr, u32 size, void* mq, s32 kind, void* arg8);

/**
 * Returns RAM holding the sample bytes at `dev_addr`, reusing a cached DMA buffer or starting a new DMA.
 * Corresponds to OoT AudioLoad_DmaSampleData (original name Nas_WaveDmaCallBack).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void* AudioLoad_DmaSampleData(u32 dev_addr, u32 size, s32 arg2, u8* dma_index_ref, s32 medium) {
    UNUSED s32 pad1; // FAKE: stack padding
    SampleDma8110C1C0* dma;
    s32 has_dma = FALSE;
    u32 dma_dev_addr;
    UNUSED u32 pad2; // FAKE: stack padding
    u32 dma_index;
    u32 transfer;
    s32 buffer_pos;
    u32 i;

    if (arg2 != 0 || *dma_index_ref >= D_81288B30) {
        for (i = D_81288B30; i < D_81288B2C; i++) {
            dma = &D_81288B28[i];
            buffer_pos = dev_addr - dma->devAddr;
            if (0 <= buffer_pos && (u32)buffer_pos <= (dma->size - size)) {
                if (dma->ttl == 0 && D_81288D39 != D_81288D3B) {
                    if (dma->reuseIndex != D_81288D39) {
                        D_81288C38[dma->reuseIndex] = D_81288C38[D_81288D39];
                        D_81288B28[D_81288C38[D_81288D39]].reuseIndex = dma->reuseIndex;
                    }
                    D_81288D39++;
                }
                dma->ttl = 32;
                *dma_index_ref = (u8)i;
                return (dma->ramAddr + dev_addr) - dma->devAddr;
            }
        }

        if (D_81288D39 != D_81288D3B && arg2 != 0) {
            dma_index = D_81288C38[D_81288D39];
            D_81288D39++;
            dma = D_81288B28 + dma_index;
            has_dma = TRUE;
        }
    } else {
        dma = D_81288B28 + *dma_index_ref;
        buffer_pos = dev_addr - dma->devAddr;
        if (0 <= buffer_pos && (u32)buffer_pos <= dma->size - size) {
            if (dma->ttl == 0) {
                if (dma->reuseIndex != D_81288D38) {
                    D_81288B38[dma->reuseIndex] = D_81288B38[D_81288D38];
                    D_81288B28[D_81288B38[D_81288D38]].reuseIndex = dma->reuseIndex;
                }
                D_81288D38++;
            }
            dma->ttl = 2;
            return (dma->ramAddr + dev_addr) - dma->devAddr;
        }
    }

    if (!has_dma) {
        if (D_81288D38 == D_81288D3A) {
            return NULL;
        }
        dma_index = D_81288B38[D_81288D38++];
        dma = D_81288B28 + dma_index;
        has_dma = TRUE;
    }

    transfer = dma->size;
    dma_dev_addr = dev_addr & ~0xF;
    dma->ttl = 3;
    dma->devAddr = dma_dev_addr;
    dma->sizeUnused = transfer;
    AudioLoad_Dma(&D_81288AD8[D_81288D94++ * 0x18], 0, 0, dma_dev_addr, dma->ramAddr, transfer, &D_81288AB8, medium, D_8123AF3C);
    *dma_index_ref = dma_index;
    return (dev_addr - dma_dev_addr) + dma->ramAddr;
}

extern s32 D_81288D80;

/**
 * Allocates the sample DMA buffers: three per note (times the buffer
 * multiplier) of 0xA0 bytes for the first list, then one per note for the
 * second, trying D_81287740 when D_81287760 is full. Each list's reuse queue
 * is filled with the buffer indices and the rest of the queue cleared.
 * Corresponds to OoT AudioLoad_InitSampleDmaBuffers (original name Nas_WaveDmaNew).
 * Seen in play in pokestadium0 (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play in pokestadium0 (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in
 * the Transfer Pak; called from AudioHeap_Init.
 */
void AudioLoad_InitSampleDmaBuffers(s32 num_notes) {
    s32 i;

    D_81288D80 = 0xA0;
    D_81288B28 = (SampleDma8110C1C0*)GbApu_Alloc(&D_81287760, 4 * D_81288D88 * sizeof(SampleDma8110C1C0) * sGbAudioOutputConfig.specUnk4);
    for (i = 0; i < 3 * D_81288D88 * sGbAudioOutputConfig.specUnk4; i++) {
        if ((D_81288B28[D_81288B2C].ramAddr = (u8*)GbApu_Alloc(&D_81287760, D_81288D80)) == NULL) {
            if (D_81287740 == 0) {
                break;
            }
            if ((D_81288B28[D_81288B2C].ramAddr = (u8*)GbApu_Alloc((M2C_UNK*)&D_81287740, D_81288D80)) == NULL) {
                break;
            }
        }
        D_81288B28[D_81288B2C].size = D_81288D80;
        D_81288B28[D_81288B2C].devAddr = 0;
        D_81288B28[D_81288B2C].sizeUnused = 0;
        D_81288B28[D_81288B2C].unused = 0;
        D_81288B28[D_81288B2C].ttl = 0;
        D_81288B2C++;
    }

    for (i = 0; (u32)i < D_81288B2C; i++) {
        D_81288B38[i] = i;
        D_81288B28[i].reuseIndex = i;
    }

    for (i = D_81288B2C; i < 0x100; i++) {
        D_81288B38[i] = 0;
    }

    D_81288D38 = 0;
    D_81288D3A = D_81288B2C;
    D_81288B30 = D_81288B2C;
    D_81288D80 = 0xA0;

    for (i = 0; i < D_81288D88; i++) {
        if ((D_81288B28[D_81288B2C].ramAddr = (u8*)GbApu_Alloc(&D_81287760, D_81288D80)) == NULL) {
            if (D_81287740 == 0) {
                break;
            }
            if ((D_81288B28[D_81288B2C].ramAddr = (u8*)GbApu_Alloc((M2C_UNK*)&D_81287740, D_81288D80)) == NULL) {
                break;
            }
        }
        D_81288B28[D_81288B2C].size = D_81288D80;
        D_81288B28[D_81288B2C].devAddr = 0;
        D_81288B28[D_81288B2C].sizeUnused = 0;
        D_81288B28[D_81288B2C].unused = 0;
        D_81288B28[D_81288B2C].ttl = 0;
        D_81288B2C++;
    }

    for (i = D_81288B30; (u32)i < D_81288B2C; i++) {
        D_81288C38[i - D_81288B30] = i;
        D_81288B28[i].reuseIndex = i - D_81288B30;
    }

    for (i = D_81288B2C; i < 0x100; i++) {
        D_81288C38[i] = D_81288B30;
    }

    D_81288D39 = 0;
    D_81288D3B = D_81288B2C - D_81288B30;
}

extern u8 D_81287AF5[1];

/**
 * Returns 1 when font `arg0` is loaded (status 2 or above) or is 0xFF.
 * Corresponds to OoT AudioLoad_IsFontLoadComplete (original name Nas_CheckIDbank).
 * Seen in play in pokestadium0 (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play in pokestadium0 (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in
 * the Transfer Pak; called from AudioSeq_SequencePlayerDisable.
 */
s32 AudioLoad_IsFontLoadComplete(s32 arg0) {
    if (arg0 == 0xFF) {
        return 1;
    }
    if (((u8*)&D_81287AF5)[arg0] >= 2) {
        return 1;
    }
    return 0;
}

extern u8 D_81287AF6[1];

/**
 * Returns 1 when sequence `arg0` is loaded (status 2 or above) or is 0xFF.
 * Corresponds to OoT AudioLoad_IsSeqLoadComplete (original name Nas_CheckIDseq).
 * Seen in play in pokestadium0 (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play in pokestadium0 (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in
 * the Transfer Pak; called from AudioSeq_SequencePlayerDisable.
 */
s32 AudioLoad_IsSeqLoadComplete(s32 arg0) {
    if (arg0 == 0xFF) {
        return 1;
    }
    if (((u8*)&D_81287AF6)[arg0] >= 2) {
        return 1;
    }
    return 0;
}

extern u8 D_81287AF4[1];

/**
 * Returns 1 when sample bank `arg0` is loaded (status 2 or above) or is 0xFF.
 * Corresponds to OoT AudioLoad_IsSampleLoadComplete (original name Nas_CheckIDwave).
 * No caller (2026-11-13): nothing in the built ROM jumps to it or holds its address (no jal or j,
 * no pointer word, no lui/addiu pair in any fragment), so play cannot reach it.
 * @unverified Name and comment guessed from the code.
 */
s32 AudioLoad_IsSampleLoadComplete(s32 arg0) {
    if (arg0 == 0xFF) {
        return 1;
    }
    if (((u8*)&D_81287AF4)[arg0] >= 2) {
        return 1;
    }
    return 0;
}

extern u8 D_81287AF5[1];
/**
 * Sets font `arg0`'s load status unless it is permanent (5).
 * Corresponds to OoT AudioLoad_SetFontLoadStatus (original name Nas_WriteIDbank).
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment guessed from the code.
 */
void AudioLoad_SetFontLoadStatus(s32 arg0, s32 arg1) {
    u8 *temp_v0;

    if (arg0 != 0xFF) {
        temp_v0 = arg0 + &D_81287AF5;
        if (*temp_v0 != 5) {
            *temp_v0 = arg1;
        }
    }
}

extern u8 D_81287AF6[1];
/**
 * Sets sequence `arg0`'s load status unless it is permanent (5).
 * Corresponds to OoT AudioLoad_SetSeqLoadStatus (original name Nas_WriteIDseq).
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment guessed from the code.
 */
void AudioLoad_SetSeqLoadStatus(s32 arg0, s32 arg1) {
    u8 *temp_v0;

    if (arg0 != 0xFF) {
        temp_v0 = arg0 + &D_81287AF6;
        if (*temp_v0 != 5) {
            *temp_v0 = arg1;
        }
    }
}

extern u8 D_81287AF4[1];
/**
 * Sets sample bank `arg0`'s load status unless it is permanent (5).
 * Corresponds to OoT AudioLoad_SetSampleFontLoadStatus (original name Nas_WriteIDwaveOnly).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void AudioLoad_SetSampleLoadStatus(s32 arg0, s32 arg1) {
    u8 *temp_v0;

    if (arg0 != 0xFF) {
        temp_v0 = arg0 + &D_81287AF4;
        if (*temp_v0 != 5) {
            *temp_v0 = arg1;
        }
    }
}

/**
 * Stores `table`'s ROM base `rom_addr` and medium parameter `unk_medium_param`, and adds the base to every
 * cartridge entry (medium 2) that has a size.
 * Corresponds to OoT AudioLoad_InitTable (original name Nas_BankHeaderInit).
 * Seen in play in pokestadium0 (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in
 * the Transfer Pak; called from AudioLoad_Init.
 */
void AudioLoad_InitTable(AudioTable* table, u32 rom_addr, u16 unk_medium_param) {
    s32 i;

    table->unkMediumParam = unk_medium_param;
    table->romAddr = rom_addr;
    for (i = 0; i < table->numEntries; i++) {
        if ((table->entries[i].size != 0) && (table->entries[i].medium == 2)) {
            table->entries[i].romAddr += rom_addr;
        }
    }
}

extern u8* D_81288D48;
extern u16 D_81288D4C;
s32 AudioLoad_GetRealTableIndex(s32 arg0, s32 arg1);
s32 AudioLoad_SyncLoadFont(s32 arg0);

/**
 * Loads every font of sequence `arg0`; returns the last font's data and stores its id in `arg1`.
 * Corresponds to OoT AudioLoad_SyncLoadSeqFonts (original name Nas_PreLoadBank). Zero canonicalizes
 * the sequence id with AudioLoad_GetRealTableIndex first; OoT does not.
 * @unverified Name follows the counterpart; not checked in play.
 */
s32 AudioLoad_SyncLoadSeqFonts(s32 arg0, s32* arg1) {
    s32 off;
    s32 count;
    u8 id;
    s32 ret;

    if (arg0 >= D_81288D4C) {
        return 0;
    }
    off = ((u16*)D_81288D48)[AudioLoad_GetRealTableIndex(0, arg0)];
    id = 0xFF;
    count = *(off + D_81288D48);
    off++;
    while (count > 0) {
        id = D_81288D48[off++];
        ret = AudioLoad_SyncLoadFont(id);
        count--;
    }
    *arg1 = id;
    return ret;
}

extern u16 D_81288D4C;
s32 AudioLoad_GetRealTableIndex(s32 arg0, s32 arg1);
s32 AudioLoad_SyncLoadSeqFonts(s32 arg0, s32* arg1);
s32 AudioLoad_SyncLoadSeq(s32 arg0);
/**
 * Loads sequence `arg0`'s fonts (bit 2 of `arg1`) and its data (bit 1).
 * Corresponds to OoT AudioLoad_SyncLoadSeqParts (original name Nas_PreLoadSeq). Zero canonicalizes
 * the sequence id first; OoT does not.
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioLoad_SyncLoadSeqParts(s32 arg0, s32 arg1) {
    s32 sp18[2];

    if (arg0 < D_81288D4C) {
        arg0 = AudioLoad_GetRealTableIndex(0, arg0);
        if (arg1 & 2) {
            AudioLoad_SyncLoadSeqFonts(arg0, sp18);
        }
        if (arg1 & 1) {
            AudioLoad_SyncLoadSeq(arg0);
        }
    }
}

void AudioLoad_SyncDmaUnkMedium(s32 arg0, M2C_UNK arg1, M2C_UNK arg2, M2C_UNK arg3);
void AudioLoad_SyncDma(s32 arg0, s32 arg1, u32 arg2, s32 arg3);
extern u8* D_81288D44;
/**
 * Copies a relocated sample that is not in RAM into the persistent sample cache and points it
 * there.
 * Corresponds to OoT AudioLoad_SyncLoadSample (original name __Nas_LoadVoice_Inner).
 * @unverified Name follows the counterpart; not checked in play.
 */
s32 AudioLoad_SyncLoadSample(Sample8110CD0C* arg0, s32 arg1) {
    s32 ret;

    if (arg0->isRelocated == 1) {
        if (arg0->medium != 0) {
            ret = AudioHeap_AllocPersistentSampleCache(arg0->size, arg1, arg0->sampleAddr, arg0->medium);
            if (ret == 0) {
                return -1;
            }
            if (arg0->medium == 1) {
                AudioLoad_SyncDmaUnkMedium(arg0->sampleAddr, ret, arg0->size, *(s16*)(D_81288D44 + 2));
            } else {
                AudioLoad_SyncDma(arg0->sampleAddr, ret, arg0->size, arg0->medium);
            }
            arg0->medium = 0;
            arg0->sampleAddr = ret;
        }
    }
}

u8* AudioPlayback_GetInstrumentInner(s32, s32);
u8* AudioPlayback_GetDrum(s32, s32);
/**
 * Loads the samples of instrument `arg1` (all three ranges) or, for 0x7F, of drum `arg2` in font
 * `arg0`.
 * Corresponds to OoT AudioLoad_SyncLoadInstrument (original name Nas_LoadVoice).
 * @unverified Name follows the counterpart; not checked in play.
 */
s32 AudioLoad_SyncLoadInstrument(s32 arg0, s32 arg1, s32 arg2) {
    u8 *p;

    if (arg1 < 0x7F) {
        p = AudioPlayback_GetInstrumentInner(arg0, arg1);
        if (p == NULL) {
            return -1;
        }
        if (p[1] != 0) {
            AudioLoad_SyncLoadSample(M2C_FIELD(p, Sample8110CD0C **, 8), arg0);
        }
        AudioLoad_SyncLoadSample(M2C_FIELD(p, Sample8110CD0C **, 0x10), arg0);
        if (p[2] != 0x7F) {
            AudioLoad_SyncLoadSample(M2C_FIELD(p, Sample8110CD0C **, 0x18), arg0);
        }
    } else if (arg1 == 0x7F) {
        p = AudioPlayback_GetDrum(arg0, arg2);
        if (p == NULL) {
            return -1;
        }
        AudioLoad_SyncLoadSample(M2C_FIELD(p, Sample8110CD0C **, 4), arg0);
        return 0;
    }
}

s32 osSendMesg(OSMesgQueue*, OSMesg, s32);
s32 AudioLoad_GetRealTableIndex(s32 arg0, s32 arg1);
s32 AudioLoad_AsyncLoadInner(s32 table_type, s32 id, s32 n_chunks, s32 ret_data, OSMesgQueue* ret_queue);

/**
 * Starts an async load of sample bank `arg0`, or sends an empty message to `arg3` when nothing needs loading.
 * Corresponds to OoT AudioLoad_AsyncLoadSampleBank (original name Nas_PreLoadWave_BG). Zero calls
 * AudioLoad_AsyncLoadInner directly; OoT goes through AudioLoad_AsyncLoad (Nas_PreLoad_BG). SM64
 * Shindou does the same as Zero (func_sh_802f30f4).
 * No caller (2026-11-13): nothing in the built ROM jumps to it or holds its address (no jal or j,
 * no pointer word, no lui/addiu pair in any fragment), so play cannot reach it.
 * @unverified Name and comment guessed from the code.
 */
void AudioLoad_AsyncLoadSampleBank(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    if (AudioLoad_AsyncLoadInner(2, AudioLoad_GetRealTableIndex(2, arg0), arg1, arg2, arg3) == 0) {
        osSendMesg(arg3, 0, 0);
    }
}

extern u8* D_81288D48;
/**
 * Starts an async load of every font of sequence `arg0`.
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioLoad_AsyncLoadSeqFonts(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 off;
    s32 count;

    off = ((u16*)D_81288D48)[AudioLoad_GetRealTableIndex(0, arg0)];
    count = *(off + D_81288D48);
    off++;
    while (count > 0) {
        AudioLoad_AsyncLoadInner(1, AudioLoad_GetRealTableIndex(1, D_81288D48[off++]), arg1, arg2, arg3);
        count--;
    }
}

/**
 * Corresponds to OoT AudioLoad_GetFontsForSequence (original name Nas_SeqToBank). Stadium 1's name
 * Audio_CountActiveSoundsByCategory did not fit.
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name was previously used only by callers of this address; the definition was still func_.
 */
u8* AudioLoad_GetFontsForSequence(s32 arg0, s32* arg1) {
    s32 off;

    off = ((u16*)D_81288D48)[AudioLoad_GetRealTableIndex(0, arg0)];
    *arg1 = *(off + D_81288D48);
    off++;
    if (*arg1 == 0) {
        return NULL;
    }
    return D_81288D48 + off;
}

void AudioLoad_DiscardFont(s32 arg0);

/**
 * Discards every font of sequence `arg0` that is not permanently cached and marks it unloaded.
 * Corresponds to OoT AudioLoad_DiscardSeqFonts (original name Nas_FlushBank).
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioLoad_DiscardSeqFonts(s32 arg0) {
    s32 off;
    s32 count;
    s32 id;

    off = ((u16*)D_81288D48)[AudioLoad_GetRealTableIndex(0, arg0)];
    count = *(off + D_81288D48);
    off++;
    while (count > 0) {
        count--;
        id = AudioLoad_GetRealTableIndex(1, D_81288D48[off++]);
        if (AudioHeap_SearchPermanentCache(1, id) == 0) {
            AudioLoad_DiscardFont(id);
            AudioLoad_SetFontLoadStatus(id, 0);
        }
    }
}

/**
 * Clears font `arg0` from the font cache entries, then discards the notes that use it.
 * Corresponds to OoT AudioLoad_DiscardFont (original name __Kill_Bank).
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioLoad_DiscardFont(s32 arg0) {
    AudioPersistentCache* persistent;
    s32 none;
    AudioCache* tbl = (AudioCache*)&D_81287820;
    u32 i;

    none = -1;
    if (arg0 == tbl->temporary.entries[0].id) {
        tbl->temporary.entries[0].id = none;
    } else if (arg0 == tbl->temporary.entries[1].id) {
        tbl->temporary.entries[1].id = none;
    }
    persistent = &tbl->persistent;
    for (i = 0; i < persistent->numEntries; i++) {
        if (arg0 == persistent->entries[i].id) {
            persistent->entries[i].id = none;
        }
    }
    AudioHeap_DiscardFont(arg0);
}

s32 AudioLoad_SyncInitSeqPlayerInternal(s32 arg0, s32 arg1, s32 arg2);

/**
 * Plays sequence arg1 on sequence player arg0 from the start: clears the player's skipTicks
 * (D_81287BE0 is a view of the players starting at SequencePlayer.skipTicks, stride 0x15C) and
 * returns AudioLoad_SyncInitSeqPlayerInternal(arg0, arg1, arg2). Returns 0 and does nothing while
 * the audio reset timer D_812346FC runs (OoT's resetTimer). Sibling of
 * AudioLoad_SyncInitSeqPlayerSkipTicks, which stores its arg2 instead of 0 and passes 0 as the last
 * argument.
 * Corresponds to OoT AudioLoad_SyncInitSeqPlayer (original name Nas_StartMySeq).
 * No caller (2026-11-13): nothing in the built ROM jumps to it or holds its address (no jal or j,
 * no pointer word, no lui/addiu pair in any fragment), so play cannot reach it.
 * @unverified Name guessed from the code; comment kept.
 */
s32 AudioLoad_SyncInitSeqPlayer(s32 arg0, s32 arg1, s32 arg2) {
    if (D_812346FC != 0) {
        return 0;
    }
    D_81287BE0[arg0].skipTicks = 0;
    return AudioLoad_SyncInitSeqPlayerInternal(arg0, arg1, arg2);
}

/**
 * Like AudioLoad_SyncInitSeqPlayer, but skips the first `arg2` ticks.
 * Corresponds to OoT AudioLoad_SyncInitSeqPlayerSkipTicks (original name Nas_StartSeq_Skip).
 * No caller (2026-11-13): nothing in the built ROM jumps to it or holds its address (no jal or j,
 * no pointer word, no lui/addiu pair in any fragment), so play cannot reach it.
 * @unverified Name and comment guessed from the code.
 */
s32 AudioLoad_SyncInitSeqPlayerSkipTicks(s32 arg0, s32 arg1, s32 arg2) {
    if (D_812346FC != 0) {
        return 0;
    }
    D_81287BE0[arg0].skipTicks = arg2;
    return AudioLoad_SyncInitSeqPlayerInternal(arg0, arg1, 0);
}

s32 AudioLoad_SyncLoadSeq(s32 arg0);
void AudioSeq_ResetSequencePlayer(s32);
void AudioSeq_SkipForwardSequence(s8*);
/**
 * Loads the sequence and its fonts, then initializes player `arg0` to play it.
 * Corresponds to OoT AudioLoad_SyncInitSeqPlayerInternal (original name __Nas_StartSeq).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
s32 AudioLoad_SyncInitSeqPlayerInternal(s32 arg0, s32 arg1, s32 arg2) {
    s8* slot;
    u8 id;
    s32 count;
    s32 off;
    s32 r;
    s32 i;

    if (arg1 >= D_81288D4C) {
        return 0;
    }
    slot = &D_81287B08 + arg0 * 0x15C;
    arg1 = AudioLoad_GetRealTableIndex(0, arg1);
    AudioSeq_SequencePlayerDisable(slot);
    id = 0xFF;
    /* do-while(0) block (likely a macro in the original) is required for the register choice */
    do {  // FAKE
        off = ((u16*)D_81288D48)[arg1];
        count = *(off + D_81288D48);
        off++;
    } while (0); // FAKE
    while (count > 0) {
        id = D_81288D48[off++];
        AudioLoad_SyncLoadFont(id);
        count--;
    }
    r = AudioLoad_SyncLoadSeq(arg1);
    AudioSeq_ResetSequencePlayer(arg0);
    M2C_FIELD(slot, s8*, 4) = arg1;
    M2C_FIELD(slot, u8*, 5) = id;
    ((SequencePlayer*)slot)->enabled = 1;
    M2C_FIELD(slot, s32*, 0x18) = r;
    M2C_FIELD(slot, s32*, 0x74) = r;
    M2C_FIELD(slot, s8*, 0x8C) = 0;
    M2C_FIELD(slot, s16*, 0x10) = 0;
    ((SequencePlayer*)slot)->finished = 0;
    AudioSeq_SkipForwardSequence(slot);
    for (i = 0; i < 16; i++) { // karodo: empty loop over the 16 channels
    }
}

s32 AudioLoad_SyncLoad(s32 table_type, s32 id, s32* did_allocate);

/**
 * Loads sequence `arg0`'s data synchronously.
 * Corresponds to OoT AudioLoad_SyncLoadSeq (original name __Load_Seq). OoT also refuses while a
 * load of the sequence is in progress; Zero does not check.
 * @unverified Name follows the counterpart; not checked in play.
 */
s32 AudioLoad_SyncLoadSeq(s32 arg0) {
    s32 idx;
    s32 sp18;

    idx = AudioLoad_GetRealTableIndex(0, arg0);
    AudioLoad_SyncLoad(0, idx, &sp18);
}

u8* AudioLoad_GetLoadTable(s32 arg0);
s32 AudioLoad_SearchCaches(s32 arg0, s32 arg1);
#define ENTRY ((u8*)&((s32(*)[4])table)[arg0])
/**
 * Returns sample bank `arg0`'s address and stores its medium in `arg1`: the cached copy, its ROM
 * address when its cache policy is 4, or a fresh synchronous load.
 * Corresponds to OoT AudioLoad_TrySyncLoadSampleBank (original name __Load_Wave). OoT adds a noLoad
 * argument; Zero's two-argument form is SM64 Shindou's func_sh_802f3598.
 * @unverified Name follows the counterpart; not checked in play.
 */
s32 AudioLoad_TrySyncLoadSampleBank(s32 arg0, s32* arg1) {
    s32 ret;
    u8* table = AudioLoad_GetLoadTable(2);
    s32 state;
    s32 sp28;

    arg0 = AudioLoad_GetRealTableIndex(2, arg0);
    if ((ret = AudioLoad_SearchCaches(2, arg0)) != 0) {
        AudioLoad_SetSampleLoadStatus(arg0, 2);
        *arg1 = 0;
        return ret;
    }
    state = (s8)ENTRY[0x19];
    if (state == 4) {
        *arg1 = (s8)ENTRY[0x18];
        return *(s32*)(ENTRY + 0x10);
    }
    if ((ret = AudioLoad_SyncLoad(2, arg0, &sp28)) != 0) {
        *arg1 = 0;
        return ret;
    }
    *arg1 = (s8)ENTRY[0x18];
    return *(s32*)(ENTRY + 0x10);
}
#undef ENTRY

s32 AudioLoad_TrySyncLoadSampleBank(s32 arg0, s32* arg1);
void AudioLoad_RelocateFontAndPreloadSamples(s32 font_id, s32 font_data, RelocInfo8110EF7C* reloc_info, s32 async);
/**
 * Loads font `arg0` and its two sample banks, relocating it when newly allocated.
 * Corresponds to OoT AudioLoad_SyncLoadFont (original name __Load_Ctrl). SM64 Shindou:
 * func_sh_802f3688.
 * @unverified Name follows the counterpart; not checked in play.
 */
s32 AudioLoad_SyncLoadFont(s32 arg0) {
    s32 ret;
    s32 a;
    s32 b;
    s32 sp38;
    s32 sp34;
    s32 sp30;
    s32 sp20[4];

    arg0 = AudioLoad_GetRealTableIndex(1, arg0);
    a = D_81288D50[arg0].sampleBankId1;
    b = D_81288D50[arg0].sampleBankId2;
    sp20[0] = a;
    sp20[1] = b;
    if (a != 0xFF) {
        sp20[2] = AudioLoad_TrySyncLoadSampleBank(a, &sp30);
    } else {
        sp20[2] = 0;
    }
    if (b != 0xFF) {
        sp20[3] = AudioLoad_TrySyncLoadSampleBank(b, &sp34);
    } else {
        sp20[3] = 0;
    }
    ret = AudioLoad_SyncLoad(1, arg0, &sp38);
    if (ret == 0) {
        return 0;
    }
    if (sp38 == 1) {
        AudioLoad_RelocateFontAndPreloadSamples(arg0, ret, sp20, 0);
    }
    return ret;
}

s32 AudioHeap_AllocCached(s32, s32, s32, s32);

/**
 * Loads entry id of audio table tableType (0 sequences, 1 sound fonts, 2 sample
 * banks) synchronously and returns its RAM address, or 0 if no memory could be
 * allocated. *didAllocate says whether new memory was used (0 when the entry
 * was already cached). The table's load status is updated.
 * Corresponds to OoT AudioLoad_SyncLoad (original name __Load_Bank).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name guessed from the code; comment kept.
 */
s32 AudioLoad_SyncLoad(s32 table_type, s32 id, s32* did_allocate) {
    u32 size;
    AudioTable* table;
    s32 ram_addr;
    u32 medium;
    s32 status;
    u32 rom_addr;
    s32 cache_policy;
    UNUSED s32 pad; // FAKE: stack padding

    ram_addr = AudioLoad_SearchCaches(table_type, id);
    if (ram_addr != 0) {
        *did_allocate = 0;
        status = 2;
    } else {
        table = (AudioTable*)AudioLoad_GetLoadTable(table_type);
        size = table->entries[id].size;
        size = ALIGN16(size);
        medium = table->entries[id].medium;
        cache_policy = table->entries[id].cachePolicy;
        rom_addr = table->entries[id].romAddr;
        switch (cache_policy) {
            case 0:
                ram_addr = AudioHeap_AllocPermanent(table_type, id, size);
                if (ram_addr == 0) {
                    return ram_addr;
                }
                break;
            case 1:
                ram_addr = AudioHeap_AllocCached(table_type, size, 1, id);
                if (ram_addr == 0) {
                    return ram_addr;
                }
                break;
            case 2:
                ram_addr = AudioHeap_AllocCached(table_type, size, 0, id);
                if (ram_addr == 0) {
                    return ram_addr;
                }
                break;
            case 3:
            case 4:
                ram_addr = AudioHeap_AllocCached(table_type, size, 2, id);
                if (ram_addr == 0) {
                    return ram_addr;
                }
                break;
        }

        *did_allocate = 1;
        if (medium == 1) {
            AudioLoad_SyncDmaUnkMedium(rom_addr, ram_addr, size, table->unkMediumParam);
        } else {
            AudioLoad_SyncDma(rom_addr, ram_addr, size, medium);
        }

        status = cache_policy == 0 ? 5 : 2;
    }

    switch (table_type) {
        case 0:
            AudioLoad_SetSeqLoadStatus(id, status);
            break;
        case 1:
            AudioLoad_SetFontLoadStatus(id, status);
            break;
        case 2:
            AudioLoad_SetSampleLoadStatus(id, status);
            break;
    }

    return ram_addr;
}


/**
 * Returns the real index of entry `arg1` in load table `arg0`: an entry with size 0 is an alias whose ROM address field holds the real index.
 * Corresponds to OoT AudioLoad_GetRealTableIndex (original name __Link_BankNum).
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment guessed from the code.
 */
s32 AudioLoad_GetRealTableIndex(s32 arg0, s32 arg1) {
    s32* entry = (s32*)(AudioLoad_GetLoadTable(arg0) + arg1 * 16);

    if (entry[5] == 0) {
        arg1 = entry[4];
    }
    return arg1;
}

s32 AudioHeap_SearchCaches(s32, M2C_UNK, s32);

/**
 * Looks entry `arg1` of table `arg0` up in the permanent cache, then the other caches.
 * Corresponds to OoT AudioLoad_SearchCaches (original name __Check_Cache).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
s32 AudioLoad_SearchCaches(s32 arg0, s32 arg1) {
    s32 temp_v0;
    s32 temp_v0_2;

    temp_v0_2 = AudioHeap_SearchPermanentCache(arg0, arg1);
    if (temp_v0_2 != 0) {
        return temp_v0_2;
    }
    temp_v0 = AudioHeap_SearchCaches(arg0, 2, arg1);
    if (temp_v0 != 0) {
        return temp_v0;
    }
    return 0;
}

extern AudioTable* D_81288D3C;
extern AudioTable* D_81288D40;
extern u8* D_81288D44;

/**
 * Returns the audio table for table type arg0: the sequence table D_81288D3C (0), the sound font
 * table D_81288D40 (1) or the sample bank table D_81288D44 (2), and NULL for any other value. The
 * tables are AudioTable (16-byte header, then 16-byte entries).
 * Matching note: the result is built in a local and returned once; separate
 * `return` statements in each case give a different register choice.
 * Corresponds to OoT AudioLoad_GetLoadTable.
 * Seen in play in pokestadium0 (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play in pokestadium0 (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in
 * the Transfer Pak; called from AudioLoad_LoadPermanentSamples.
 */
u8* AudioLoad_GetLoadTable(s32 arg0) {
    u8* ret;
    ret = NULL;
    switch (arg0) {
    case 0:
        ret = (u8*)D_81288D3C;
        break;
    case 1:
        ret = (u8*)D_81288D40;
        break;
    case 2:
        ret = D_81288D44;
        break;
    }
    return ret;
}

void AudioLoad_RelocateSample(Sample8110EE3C** sample_p, s32 font_data_addr, RelocInfo8110EF7C* reloc_info);

/**
 * Relocates a sound font loaded at fontData: the drum and instrument pointer
 * lists, each drum's and instrument's samples and envelope, and records the
 * font's drum and instrument lists in D_81288D50.
 * Corresponds to OoT AudioLoad_RelocateFont (original name Nas_BankOfsToAddr_Inner).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name guessed from the code; comment kept.
 */
void AudioLoad_RelocateFont(s32 font_id, u32 font_data, void* reloc_info) {
    u32 reloc2;
    u32 reloc;
    s32 i;
    u8* drum;
    u32* ptrs = (u32*)font_data;
    s32 num_drums = D_81288D50[font_id].numDrums;
    u8* inst;
    s32 num_instruments = D_81288D50[font_id].numInstruments;

    reloc2 = ptrs[0];
    if ((reloc2 != 0) && (num_drums != 0)) {
        ptrs[0] = reloc2 + font_data;
        for (i = 0; i < num_drums; i++) {
            reloc = ((u32*)ptrs[0])[i];
            if (reloc != 0) {
                reloc += font_data;
                ((u32*)ptrs[0])[i] = reloc;
                drum = (u8*)reloc;
                if (drum[2] == 0) {
                    AudioLoad_RelocateSample(drum + 4, font_data, reloc_info);
                    reloc = *(u32*)(drum + 0xC);
                    drum[2] = 1;
                    *(u32*)(drum + 0xC) = reloc + font_data;
                }
            }
        }
    }

    if (num_instruments >= 0x80) {
        num_instruments = 0x7F;
    }

    for (i = 1; i <= num_instruments; i++) {
        if (ptrs[i] != 0) {
            ptrs[i] += font_data;
            inst = (u8*)ptrs[i];
            if (inst[0] == 0) {
                if (inst[1] != 0) {
                    AudioLoad_RelocateSample(inst + 8, font_data, reloc_info);
                }
                AudioLoad_RelocateSample(inst + 0x10, font_data, reloc_info);
                if (inst[2] != 0x7F) {
                    AudioLoad_RelocateSample(inst + 0x18, font_data, reloc_info);
                }
                reloc = *(u32*)(inst + 4);
                inst[0] = 1;
                *(u32*)(inst + 4) = reloc + font_data;
            }
        }
    }

    D_81288D50[font_id].drums = (u8**)ptrs[0];
    D_81288D50[font_id].instruments = (u8**)(ptrs + 1);
}

extern s8 D_8123B08C[];
extern s8 D_8123B098[];
extern OSMesgQueue D_81288AF0;
extern u8 D_81288B10[];
s32 AudioLoad_Dma(IoMesg8110DBA0* mb, s32 pri, s32 direction, u32 dev_addr, void* dram_addr, u32 size, void* mq, s32 kind, void* arg8);
/**
 * Copies `arg2` bytes (rounded up to 16) from ROM `arg0` to RAM `arg1` in 1 KB DMA chunks and waits for each.
 * Corresponds to OoT AudioLoad_SyncDma (original name Nas_FastCopy).
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment guessed from the code.
 */
void AudioLoad_SyncDma(s32 arg0, s32 arg1, u32 arg2, s32 arg3) {

    arg2 = (arg2 + 0xF) & ~0xF;
    osInvalDCache(arg1, arg2);
    while (TRUE) {
        if (arg2 < 0x400) {
            break;
        }
        AudioLoad_Dma(D_81288B10, 1, 0, arg0, arg1, 0x400, &D_81288AF0, arg3, D_8123B08C);
        osRecvMesg(&D_81288AF0, 0, 1);
        arg2 -= 0x400;
        arg0 += 0x400;
        arg1 += 0x400;
    }
    if (arg2 != 0) {
        AudioLoad_Dma(D_81288B10, 1, 0, arg0, arg1, arg2, &D_81288AF0, arg3, D_8123B098);
        osRecvMesg(&D_81288AF0, 0, 1);
    }
}

/**
 * Empty: the unknown medium (1) is never loaded synchronously.
 * Corresponds to OoT AudioLoad_SyncDmaUnkMedium (original name Nas_FastDiskCopy).
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioLoad_SyncDmaUnkMedium(s32 arg0, M2C_UNK arg1, M2C_UNK arg2, M2C_UNK arg3) {

}

/**
 * Starts a PI DMA for audio data that replies to the message queue `mq`; refuses (-1) while the audio
 * reset timer D_812346FC is above 16 (OoT's resetTimer).
 * Corresponds to OoT AudioLoad_Dma (original name Nas_StartDma).
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment guessed from the code.
 */
s32 AudioLoad_Dma(IoMesg8110DBA0* mb, s32 pri, s32 direction, u32 dev_addr, void* dram_addr, u32 size, void* mq, s32 kind, void* arg8) {
    s32 handle;

    if ((u32)D_812346FC > 16) {
        return -1;
    }
    switch (kind) {
        case 2:
            handle = osCartRomInit();
            break;
        case 3:
            handle = osDriveRomInit();
            break;
        default:
            return 0;
    }
    if (size & 0xF) {
        size = (size + 0xF) & ~0xF;
    }
    mb->pri = pri;
    mb->retQueue = mq;
    mb->dramAddr = dram_addr;
    mb->devAddr = dev_addr;
    mb->size = size;
    M2C_FIELD(handle, s32*, 0x14) = 2;
    osEPiStartDma(handle, mb, direction);
    return 0;
}

/* JP-only TU, ported from pokestadium0 (Stadium Zero, fragment000_80C00). */
/* Audio driver, matching the rest of OoT load.c (src/audio/internal) in function order. The driver
 * is Nintendo's "Nas" library (original names Nas_*); this revision sits between SM64 Shindou and
 * OoT. pokestadium0's docs/annotation/audio-driver.md has the per-function mapping. */
/* The US and PAL builds have this code in src/fragments/battle_engine/battle_engine_309ED0.c. */

#include "gb_tower_emulator_jp.h"

#include "m2c_macros.h"

void AudioLoad_Unused1A(void);
void AudioLoad_Unused1B(void);
void AudioLoad_ProcessLoads(s32 reset_status);
void AudioLoad_Unused2(void);

/**
 * Empty.
 * No OoT counterpart is certain. Zero has two empty stubs between AudioLoad_Dma and
 * AudioLoad_SyncLoadSimple, as SM64 Shindou does (func_sh_802f3ec4, func_sh_802f3ed4); OoT keeps
 * one there, AudioLoad_Unused1 (__OfsToLbaOfs).
 * No caller (2026-11-14): nothing in the built ROM jumps to it or holds its address, so play
 * cannot reach it.
 * @unverified Name from the code; it never runs.
 */
void AudioLoad_Unused1A(void) {
}

/**
 * Empty.
 * See AudioLoad_Unused1A.
 * No caller (2026-11-14): nothing in the built ROM jumps to it or holds its address, so play
 * cannot reach it.
 * @unverified Name from the code; it never runs.
 */
void AudioLoad_Unused1B(void) {
}


/**
 * Loads entry `arg1` of table `arg0` synchronously and ignores the result.
 * Corresponds to OoT AudioLoad_SyncLoadSimple (original name EmemLoad).
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioLoad_SyncLoadSimple(Particle* arg0, s32 arg1) {
    s32 sp1C;

    AudioLoad_SyncLoad(arg0, arg1, &sp1C);
}

extern u8 D_81287AF4[];
extern u8 D_81287AF5[];
extern u8 D_81287AF6[];
s32 AudioLoad_SearchCaches(s32 arg0, s32 arg1);

s32 AudioHeap_AllocPermanent(s32 x, s32 y, s32 size);
s32 AudioHeap_AllocCached(s32, s32, s32, s32);
s32 osSendMesg(OSMesgQueue*, OSMesg, s32);
void AudioLoad_SetFontLoadStatus(s32 arg0, s32 arg1);
void AudioLoad_SetSeqLoadStatus(s32 arg0, s32 arg1);
void AudioLoad_SetSampleLoadStatus(s32 arg0, s32 arg1);
Entry8110EABC* AudioLoad_StartAsyncLoad(s32 dev_addr, s32 ram_addr, u32 size, s32 medium, s32 n_chunks, OSMesgQueue* ret_queue, u32 ret_msg);

/**
 * Starts an asynchronous load of entry id of audio table tableType (0 sequences,
 * 1 sound fonts, 2 sample banks). Returns 0 if a load of it is already in
 * progress or no memory could be allocated. If the entry is already cached the
 * cached address is returned and retData is posted to retQueue at once;
 * otherwise memory is allocated by the entry's cache policy and a chunked DMA
 * is started. The table's load status is updated either way.
 * Corresponds to OoT AudioLoad_AsyncLoadInner (original name __Load_Bank_BG). OoT also handles the
 * unknown-medium queue.
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name guessed from the code; comment kept.
 */
s32 AudioLoad_AsyncLoadInner(s32 table_type, s32 id, s32 n_chunks, s32 ret_data, OSMesgQueue* ret_queue) {
    u32 size;
    AudioTable* table;
    s32 ram_addr;
    s32 medium;
    s8 cache_policy;
    u32 dev_addr;
    s32 load_status;

    switch (table_type) {
        case 0:
            if (D_81287AF6[id] == 1) {
                return 0;
            }
            break;
        case 1:
            if (D_81287AF5[id] == 1) {
                return 0;
            }
            break;
        case 2:
            if (D_81287AF4[id] == 1) {
                return 0;
            }
            break;
    }

    ram_addr = AudioLoad_SearchCaches(table_type, id);
    if (ram_addr != 0) {
        load_status = 2;
        osSendMesg(ret_queue, (OSMesg)(ret_data << 0x18), 0);
    } else {
        table = (AudioTable*)AudioLoad_GetLoadTable(table_type);
        size = table->entries[id].size;
        size = ALIGN16(size);
        medium = table->entries[id].medium;
        cache_policy = table->entries[id].cachePolicy;
        dev_addr = table->entries[id].romAddr;
        load_status = 2;

        switch (cache_policy) {
            case 0:
                ram_addr = AudioHeap_AllocPermanent(table_type, id, size);
                if (ram_addr == 0) {
                    return ram_addr;
                }
                load_status = 5;
                break;
            case 1:
                ram_addr = AudioHeap_AllocCached(table_type, size, 1, id);
                if (ram_addr == 0) {
                    return ram_addr;
                }
                break;
            case 2:
                ram_addr = AudioHeap_AllocCached(table_type, size, 0, id);
                if (ram_addr == 0) {
                    return ram_addr;
                }
                break;
            case 3:
            case 4:
                ram_addr = AudioHeap_AllocCached(table_type, size, 2, id);
                if (ram_addr == 0) {
                    return ram_addr;
                }
                break;
        }

        AudioLoad_StartAsyncLoad(dev_addr, ram_addr, size, medium, n_chunks, ret_queue, (ret_data << 0x18) | (table_type << 0x10) | (id << 8) | load_status);
        load_status = 1;
    }

    switch (table_type) {
        case 0:
            AudioLoad_SetSeqLoadStatus(id, load_status);
            break;
        case 1:
            AudioLoad_SetFontLoadStatus(id, load_status);
            break;
        case 2:
            AudioLoad_SetSampleLoadStatus(id, load_status);
            break;
    }

    return ram_addr;
}

/**
 * Runs the per-frame load work: slow sample loads, sample preloads, then async loads.
 * Corresponds to OoT AudioLoad_ProcessLoads (original name Nas_BgDmaFrameWork): the same
 * three calls in the same order.
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioLoad_ProcessLoads(s32 reset_status) {
    AudioLoad_ProcessSlowLoads(reset_status);
    AudioLoad_ProcessSamplePreloads(reset_status);
    AudioLoad_ProcessAsyncLoads(reset_status);
}

extern SoundFont* D_81288D50;

extern volatile s32 D_812346FC;
extern s32 D_81255B6C;
extern u64 D_81289710[];
extern u64 D_812875A0;
extern u64 D_812896F8;
extern f32 D_81288E50;
extern s32 D_81288E54;
extern s16 sGbAudioBufferLengths[3];
extern s16* sGbAudioBuffers[3];
extern volatile s32 sGbAudioFrameCount;
extern s32 D_812346D0;
extern s32 sGbAudioBufferIndex;
extern s8 D_81288D8E;
extern s32 D_81288DAC;
extern u8 D_81288DB0[];

extern OSMesg D_81288B08[];

extern OSMesg D_81288AD0[];
extern OSMesgQueue D_81288A78;
extern OSMesg D_81288A90[];
extern OSMesgQueue D_81288A98;
extern OSMesg D_81288AB0[];


extern s32 D_81255B70;
extern s32 D_81255B74;

extern AudioPermanentCache D_812878E0;
extern volatile s8 D_81287AF7;



extern u8* D_81288D44;
extern u8* D_81288D48;

extern AudioTable D_81255BD0;
extern AudioTable D_81255B90;
extern AudioTable D_81255BB0;
extern u8 D_81255B80[];
extern u8 D_0012A1B0_jp[];
extern u8 D_00129D00_jp[];
extern u8 D_00129DB0_jp[];
void AudioThread_InitMesgQueues(void);
void AudioHeap_InitMainPools(s32);
s32 GbApu_Alloc(AudioAllocPool* arg0, s32 arg1);
void AudioHeap_InitPool(AudioAllocPool* arg0, s32 arg1, s32 arg2);
s32 AudioHeap_ResetStep(void);
void AudioLoad_InitTable(AudioTable* table, u32 rom_addr, u16 unk_medium_param);

/**
 * Initializes the audio system: clears the heap and the audio globals, sets up
 * the message queues and the three output buffers, points the sequence, font
 * and sample tables at their ROM copies and fills in each font's sample bank
 * ids and instrument and drum counts.
 * Corresponds to OoT AudioLoad_Init (original name Nas_InitAudio). OoT adds the DMA and unused
 * handlers and moves the per-font loop into AudioLoad_InitSoundFont (__SetVlute).
 * Seen in play in pokestadium0 (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play in pokestadium0 (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in
 * the Transfer Pak; called from GbEmu_Init.
 */
void AudioLoad_Init(void) {
    s32 num_fonts;
    void* ram_addr;
    s32 i;
    s32 j;
    u64* ptr;
    s16* buf;
    s32 count;

    D_812346FC = 0;

    for (i = 0; i < D_81255B6C / 8; i++) {
        D_81289710[i] = 0;
    }

    ptr = &D_812875A0;
    for (count = ((u32)&D_812896F8 - (u32)&D_812875A0) / 8; count >= 0; count--) {
        *ptr++ = 0;
    }

    D_81288E50 = 16.713f;
    D_81288E54 = 60;
    AudioThread_InitMesgQueues();

    for (i = 0; i < 3; i++) {
        sGbAudioBufferLengths[i] = 160;
    }

    sGbAudioFrameCount = 0;
    D_812346D0 = 0;
    sGbAudioBufferIndex = 0;
    D_81288D8E = 0;
    D_81288DAC = 0;
    *(s32*)(D_81288DB0 + 0x34) = 0;
    *(s32*)(D_81288DB0 + 0x84) = 0;

    osCreateMesgQueue(&D_81288AF0, D_81288B08, 1);
    osCreateMesgQueue(&D_81288AB8, D_81288AD0, 1);
    osCreateMesgQueue(&D_81288A78, D_81288A90, 1);
    osCreateMesgQueue(&D_81288A98, D_81288AB0, 1);
    D_81288D94 = 0;
    D_81288B2C = 0;
    AudioHeap_InitMainPools(D_81255B70);

    for (i = 0; i < 3; i++) {
        buf = sGbAudioBuffers[i] = GbApu_Alloc(D_81287750, 0xFC0);
        for (j = 0; j < 2016; j++) {
            *buf++ = 0;
        }
    }

    D_81287AF8 = 0;
    D_81287AF7 = 1;
    AudioHeap_ResetStep();

    D_81288D3C = &D_81255BD0;
    D_81288D40 = &D_81255B90;
    D_81288D44 = (u8*)&D_81255BB0;
    D_81288D48 = D_81255B80;
    D_81288D4C = D_81288D3C->numEntries;

    AudioLoad_InitTable(D_81288D3C, D_0012A1B0_jp, 0);
    AudioLoad_InitTable(D_81288D40, D_00129D00_jp, 0);
    AudioLoad_InitTable((AudioTable*)D_81288D44, D_00129DB0_jp, 0);

    num_fonts = D_81288D40->numEntries;
    D_81288D50 = GbApu_Alloc(D_81287750, num_fonts * sizeof(SoundFont));
    for (i = 0; i < num_fonts; i++) {
        D_81288D50[i].sampleBankId1 = D_81288D40->entries[i].shortData1 >> 8 & 0xFF;
        D_81288D50[i].sampleBankId2 = D_81288D40->entries[i].shortData1 & 0xFF;
        D_81288D50[i].numInstruments = D_81288D40->entries[i].shortData2 >> 8 & 0xFF;
        D_81288D50[i].numDrums = D_81288D40->entries[i].shortData2 & 0xFF;
    }

    ram_addr = GbApu_Alloc(D_81287750, D_81255B74);
    if (ram_addr == NULL) {
        D_81255B74 = 0;
    }
    AudioHeap_InitPool(&D_812878E0, ram_addr, D_81255B74);
}


extern SlowLoads8110E500 D_81283AB0;
s32 AudioHeap_AllocTemporarySampleCache(s32 arg0, s32 arg1, s32 arg2, s8 arg3);
s32 AudioLoad_GetFontSample(s32 arg0, s32 arg1);

/**
 * Queues a slow load of the sample for instrument or drum `arg1` (0x80 and up are drums) of font
 * `arg0`; `status` reports progress.
 * Corresponds to OoT AudioLoad_SlowLoadSample (original name VoiceLoad).
 * @unverified Name follows the counterpart; not checked in play.
 */
s32 AudioLoad_SlowLoadSample(s32 arg0, u8 arg1, s8* status) {
    Sample* header = (Sample*)AudioLoad_GetFontSample(arg0, arg1);
    AudioSlowLoad* load;

    if (header == NULL) {
        *status = 0;
        return -1;
    }
    if (header->medium == 0) {
        *status = 2;
        return 0;
    }
    load = &D_81283AB0.slots[D_81283AB0.index];
    if (load->state == 3) {
        load->state = 0;
    }
    load->sample = *header;
    load->status = status;
    load->curRamAddr = AudioHeap_AllocTemporarySampleCache(header->size, arg0, (s32)header->sampleAddr, header->medium);
    if (load->curRamAddr == 0) {
        if ((header->medium == 1) || (header->codec == 2)) {
            *status = 0;
            return -1;
        }
        *status = 3;
        return -1;
    }
    load->state = 1;
    load->bytesRemaining = ALIGN16(header->size);
    load->ramAddr = load->curRamAddr;
    load->curDevAddr = (s32)header->sampleAddr;
    load->medium = header->medium;
    load->seqOrFontId = arg0;
    load->instId = arg1;
    D_81283AB0.index ^= 1;
    return 0;
}

u8* AudioPlayback_GetInstrumentInner(s32, s32);
u8* AudioPlayback_GetDrum(s32, s32);
/**
 * Returns the sample of instrument `arg1` (below 0x80) or drum `arg1` - 0x80 of font `arg0`, or
 * NULL.
 * Corresponds to OoT AudioLoad_GetFontSample (original name __GetWaveTable). OoT also returns
 * sound-effect samples for ids of 0x100 and up.
 * @unverified Name follows the counterpart; not checked in play.
 */
s32 AudioLoad_GetFontSample(s32 arg0, s32 arg1) {
    s32* p;
    s32 v;

    if (arg1 < 0x80) {
        p = AudioPlayback_GetInstrumentInner(arg0, arg1);
        if (p == NULL) {
            return 0;
        }
        v = p[4];
    } else {
        p = AudioPlayback_GetDrum(arg0, arg1 - 0x80);
        if (p == NULL) {
            return 0;
        }
        v = p[1];
    }
    return v;
}

/**
 * Empty.
 * Corresponds to OoT AudioLoad_Unused2.
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioLoad_Unused2(void) {
}

/** The Sample that AudioLoad_GetFontSample returns for a font instrument or drum. */


/**
 * Finishes a slow load: looks up the sample of (arg0->seqOrFontId, arg0->instId) with
 * AudioLoad_GetFontSample and, if there is one, keeps a copy of its 16-byte header in arg0->sample,
 * points its sampleAddr at the loaded copy (arg0->ramAddr) and clears bits 2-3 of its first byte
 * (medium = RAM).
 * Corresponds to OoT AudioLoad_FinishSlowLoad (original name __SwapLoadLps).
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioLoad_FinishSlowLoad(AudioSlowLoad* arg0) {
    Sample8110E71C* entry;

    entry = (Sample8110E71C*)AudioLoad_GetFontSample(arg0->seqOrFontId, arg0->instId);
    if (entry != NULL) {
        arg0->sample = *(Sample*)entry;
        entry->sampleAddr = arg0->ramAddr;
        entry->flags &= 0xFFF3;
    }
}

void AudioLoad_DmaSlowCopy(AudioSlowLoad* arg0, s32 arg1);
void AudioLoad_DmaSlowCopyUnkMedium(s32 arg0, M2C_UNK arg1, M2C_UNK arg2, M2C_UNK arg3);

/**
 * Steps the two slow sample loads, copying at most 0x1000 bytes per call and finishing a load
 * once nothing is left.
 * Corresponds to OoT AudioLoad_ProcessSlowLoads (original name LpsDma).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name guessed from the code; comment kept.
 */
void AudioLoad_ProcessSlowLoads(s32 reset_status) {
    u8* table = D_81288D44;
    AudioSlowLoad* load;
    s32 i;

    for (i = 0; i < 2; i++) {
        load = &D_81283AB0.slots[i];
        switch (D_81283AB0.slots[i].state) {
            case 2:
                osRecvMesg(&load->msgQueue, NULL, 1);
                if (reset_status != 0) {
                    load->state = 3;
                    break;
                }
            case 1:
                load->state = 2;
                if (load->bytesRemaining == 0) {
                    AudioLoad_FinishSlowLoad(load);
                    load->state = 3;
                    *load->status = 1;
                } else if (load->bytesRemaining < 0x1000) {
                    if (load->medium == 1) {
                        u32 size = load->bytesRemaining;

                        AudioLoad_DmaSlowCopyUnkMedium(load->curDevAddr, load->curRamAddr, size, *(s16*)(table + 2));
                    } else {
                        AudioLoad_DmaSlowCopy(load, load->bytesRemaining);
                    }
                    load->bytesRemaining = 0;
                } else {
                    if (load->medium == 1) {
                        AudioLoad_DmaSlowCopyUnkMedium(load->curDevAddr, load->curRamAddr, 0x1000, *(s16*)(table + 2));
                    } else {
                        AudioLoad_DmaSlowCopy(load, 0x1000);
                    }
                    load->bytesRemaining -= 0x1000;
                    load->curRamAddr += 0x1000;
                    load->curDevAddr += 0x1000;
                }
                break;
        }
    }
}

extern u8 D_8123B2AC[];
s32 AudioLoad_Dma(IoMesg8110DBA0* mb, s32 pri, s32 direction, u32 dev_addr, void* dram_addr, u32 size, void* mq, s32 kind, void* arg8);

/**
 * Starts the DMA for the next chunk of a slow load.
 * Corresponds to OoT AudioLoad_DmaSlowCopy (original name __Nas_SlowCopy).
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioLoad_DmaSlowCopy(AudioSlowLoad* arg0, s32 arg1) {
    osInvalDCache((void*)arg0->curRamAddr, arg1);
    osCreateMesgQueue(&arg0->msgQueue, &arg0->msg, 1);
    AudioLoad_Dma((s8*)&arg0->ioMesg, 0, 0, arg0->curDevAddr, arg0->curRamAddr, arg1, &arg0->msgQueue, arg0->medium, D_8123B2AC);
}

/**
 * Empty: slow loads from the unknown medium are not supported.
 * Corresponds to OoT AudioLoad_DmaSlowCopyUnkMedium (original name __Nas_SlowDiskCopy).
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioLoad_DmaSlowCopyUnkMedium(s32 arg0, M2C_UNK arg1, M2C_UNK arg2, M2C_UNK arg3) {

}


Entry8110EABC* AudioLoad_StartAsyncLoad(s32 dev_addr, s32 ram_addr, u32 size, s32 medium, s32 n_chunks, OSMesgQueue* ret_queue, u32 ret_msg);

void osCreateMesgQueue(OSMesgQueue*, OSMesg*, s32);
/**
 * Claims the free async load slot and starts a chunked DMA of size bytes from
 * dev_addr to ram_addr. n_chunks == 0 means 0x1000-byte chunks; otherwise the size
 * is split into n_chunks pieces rounded up to 0x100 (at least 0x100). ret_msg is
 * sent to ret_queue when the load finishes. Returns NULL when no slot is free.
 * Corresponds to OoT AudioLoad_StartAsyncLoad (original name Nas_BgCopyReq).
 */
Entry8110EABC* AudioLoad_StartAsyncLoad(s32 dev_addr, s32 ram_addr, u32 size, s32 medium, s32 n_chunks, OSMesgQueue* ret_queue, u32 ret_msg) {
    Entry8110EABC* load;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(D_81288A20); i++) {
        if (D_81288A20[i].active == 0) {
            load = &D_81288A20[i];
            break;
        }
    }

    if (i == ARRAY_COUNT(D_81288A20)) {
        return NULL;
    }

    load->active = 1;
    load->romAddr = dev_addr;
    load->ramAddr = ram_addr;
    load->curAddr = ram_addr;
    load->size = size;

    if (n_chunks == 0) {
        load->chunkSize = 0x1000;
    } else {
        load->chunkSize = ((s32)size / n_chunks + 0xFF) & ~0xFF;
        if (load->chunkSize < 0x100) {
            load->chunkSize = 0x100;
        }
    }

    load->doneQueue = ret_queue;
    load->state = 3;
    load->medium = medium;
    load->info = (OSMesg)ret_msg;
    osCreateMesgQueue(&load->queue, &load->msg, 1);
    return load;
}

extern volatile s32 D_812346FC;
void AudioLoad_ProcessAsyncLoad(Entry8110EABC* load, s32 reset_status);
/**
 * Advances every active async load by one step.
 * Corresponds to OoT AudioLoad_ProcessAsyncLoads (original name Nas_BgCopyMain). OoT adds the
 * unknown-medium load queue. Stadium 1's name Battle_ApplyAnimStateEffects did not fit.
 */
void AudioLoad_ProcessAsyncLoads(s32 arg0) {
    s32 i;

    if (D_812346FC != 1U) {
        for (i = 0; i < 1; i++) {
            if (D_81288A20[i].active == 1) {
                AudioLoad_ProcessAsyncLoad(&D_81288A20[i], arg0);
            }
        }
    }
}


s32 AudioLoad_TrySyncLoadSampleBank(s32 arg0, s32* arg1);
void AudioLoad_AsyncDma(s8* arg0, u32 arg1);
void AudioLoad_AsyncDmaUnkMedium(s32 arg0, M2C_UNK arg1, u32 arg2, M2C_UNK arg3);
void AudioLoad_RelocateFontAndPreloadSamples(s32 font_id, s32 font_data, RelocInfo8110EF7C* reloc_info, s32 async);

/**
 * Advances one async load by one chunk; when it is done, sets the load status, relocates a font and
 * sends the done message.
 * Corresponds to OoT AudioLoad_ProcessAsyncLoad (original name __BgCopySub). Zero has OoT's
 * AudioLoad_FinishAsyncLoad (__BgCopyFinishProcess) inlined.
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioLoad_ProcessAsyncLoad(Entry8110EABC* load, s32 reset_status) {
    u8* table = D_81288D44;
    u32 info;
    s32 font_id;
    UNUSED s32 pad[2]; // FAKE
    u32 sample_bank_id1;
    u32 sample_bank_id2;
    RelocInfo8110EF7C reloc_info;

    if (load->state >= 2) {
        load->state--;
        return;
    }
    if (load->state == 1) {
        load->state = 0;
    } else if (reset_status != 0) {
        osRecvMesg(&load->queue, 0, 1);
        load->active = 0;
        return;
    } else if (osRecvMesg(&load->queue, 0, 0) == -1) {
        return;
    }
    if (load->size == 0) {
        info = (u32)load->info;
        switch ((u8)(info >> 16)) {
            case 0:
                AudioLoad_SetSeqLoadStatus((u8)(info >> 8), (u8)info);
                break;
            case 2:
                AudioLoad_SetSampleLoadStatus((u8)(info >> 8), (u8)info);
                break;
            case 1:
                font_id = (u8)(info >> 8);
                AudioLoad_SetFontLoadStatus(font_id, (u8)info);
                sample_bank_id1 = D_81288D50[font_id].sampleBankId1;
                sample_bank_id2 = D_81288D50[font_id].sampleBankId2;
                reloc_info.sampleBankId1 = sample_bank_id1;
                reloc_info.sampleBankId2 = sample_bank_id2;
                reloc_info.baseAddr1 = (sample_bank_id1 != 0xFF) ? AudioLoad_TrySyncLoadSampleBank(sample_bank_id1, &reloc_info.medium1) : 0;
                reloc_info.baseAddr2 = (sample_bank_id2 != 0xFF) ? AudioLoad_TrySyncLoadSampleBank(sample_bank_id2, &reloc_info.medium2) : 0;
                AudioLoad_RelocateFontAndPreloadSamples(font_id, load->ramAddr, &reloc_info, 1);
                break;
        }
        load->active = 0;
        osSendMesg(load->doneQueue, load->info, 0);
        return;
    }
    if (load->size < load->chunkSize) {
        if (load->medium == 1) {
            AudioLoad_AsyncDmaUnkMedium(load->romAddr, load->curAddr, load->size, *((s16*)(table + 2)));
        } else {
            AudioLoad_AsyncDma((s8*)load, load->size);
        }
        load->size = 0;
        return;
    }
    if (load->medium == 1) {
        AudioLoad_AsyncDmaUnkMedium(load->romAddr, load->curAddr, load->chunkSize, *((s16*)(table + 2)));
    } else {
        AudioLoad_AsyncDma((s8*)load, load->chunkSize);
    }
    load->size -= load->chunkSize;
    load->romAddr += load->chunkSize;
    load->curAddr += load->chunkSize;
}

extern u8 D_8123B374[];

/**
 * Starts the DMA for the next chunk of an async load.
 * Corresponds to OoT AudioLoad_AsyncDma (original name __Nas_BgCopy).
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioLoad_AsyncDma(s8* arg0, u32 arg1) {
    arg1 = (arg1 + 0xF) & ~0xF;
    osInvalDCache((void*)M2C_FIELD(arg0, s32*, 0xC), arg1);
    osCreateMesgQueue((OSMesgQueue*)(arg0 + 0x20), (OSMesg*)(arg0 + 0x38), 1);
    AudioLoad_Dma(arg0 + 0x3C, 0, 0, M2C_FIELD(arg0, s32*, 8), M2C_FIELD(arg0, s32*, 0xC), arg1, (OSMesgQueue*)(arg0 + 0x20), arg0[2], D_8123B374);
    if (arg1) {}  // karodo
}

/**
 * Empty: async loads from the unknown medium are not supported.
 * Corresponds to OoT AudioLoad_AsyncDmaUnkMedium (original name __Nas_BgDiskCopy).
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioLoad_AsyncDmaUnkMedium(s32 arg0, M2C_UNK arg1, u32 arg2, M2C_UNK arg3) {

}

/* First word of a Sample (OoT layout: codec, medium, unk_bit26, isRelocated, size). */

extern SampleFlags8110F64C *D_81287E18[];
extern s32 D_81288A18;


/**
 * Relocates a sound font sample: the sample pointer and its loop and book
 * pointers are made absolute against the font's RAM address, and the sample
 * data address against the sample bank chosen by its medium. Samples flagged
 * for preloading that are not in RAM are added to D_81287E18.
 * Corresponds to OoT AudioLoad_RelocateSample (original name __WaveTouch).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name guessed from the code; comment kept.
 */
void AudioLoad_RelocateSample(Sample8110EE3C** sample_p, s32 font_data_addr, RelocInfo8110EF7C* reloc_info) {
    Sample8110EE3C* sample;
    s32 reloc;

    if ((u32)*sample_p <= 0x80000000) {
        reloc = (u32)*sample_p + font_data_addr;
        *sample_p = (Sample8110EE3C*)reloc;
        sample = (Sample8110EE3C*)reloc;

        if ((sample->size != 0) && (sample->isRelocated != 1)) {
            reloc = sample->loop + font_data_addr;
            sample->loop = reloc;
            reloc = sample->book + font_data_addr;
            sample->book = reloc;

            switch (sample->medium) {
                case 0:
                    reloc = sample->sampleAddr + reloc_info->baseAddr1;
                    sample->sampleAddr = reloc;
                    sample->medium = reloc_info->medium1;
                    break;
                case 1:
                    reloc = sample->sampleAddr + reloc_info->baseAddr2;
                    sample->sampleAddr = reloc;
                    sample->medium = reloc_info->medium2;
                    break;
                case 2:
                case 3:
                    break;
            }

            sample->isRelocated = 1;
            if (sample->unk_bit26 && (sample->medium != 0)) {
                D_81287E18[D_81288A18++] = (SampleFlags8110F64C*)sample;
            }
        }
    }
}


extern s32 D_81283B78;
extern AudioPreloadReq D_81288018[];
extern s32 D_81288A1C;
extern OSMesgQueue D_81288A98;
extern u8* D_81288D44;
void AudioLoad_RelocateFont(s32 font_id, u32 font_data, void* reloc_info);
void AudioLoad_SyncDmaUnkMedium(s32 arg0, M2C_UNK arg1, M2C_UNK arg2, M2C_UNK arg3);
void AudioLoad_SyncDma(s32 arg0, s32 arg1, u32 arg2, s32 arg3);

s32 AudioHeap_AllocPersistentSampleCache(s32 arg0, s32 arg1, s32 arg2, s8 arg3);


/**
 * Relocates font `font_id` and loads the samples it marks for preloading, synchronously or (with
 * `async`) through the preload queue.
 * Corresponds to OoT AudioLoad_RelocateFontAndPreloadSamples (original name Nas_BankOfsToAddr).
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioLoad_RelocateFontAndPreloadSamples(s32 font_id, s32 font_data, RelocInfo8110EF7C* reloc_info, s32 async) {
    UNUSED s32 pad[2]; // FAKE: stack padding
    s32 sample_ram_addr;
    Sample8110EF7C* sample;
    s32 size;
    s32 n_chunks;
    s32 preload_in_progress;
    s32 i;

    preload_in_progress = 0;
    if (D_81288A1C != 0) {
        preload_in_progress = 1;
    } else {
        D_81283B78 = 0;
    }

    D_81288A18 = 0;
    AudioLoad_RelocateFont(font_id, font_data, reloc_info);

    size = 0;
    for (i = 0; i < D_81288A18; i++) {
        size += ALIGN16(((Sample8110EF7C*)D_81287E18[i])->size);
    }

    for (i = 0; i < D_81288A18; i++) {
        if (D_81288A1C == 120) {
            break;
        }

        sample = (Sample8110EF7C*)D_81287E18[i];
        switch (async) {
            case 0:
                if (sample->medium == reloc_info->medium1) {
                    // Assigning size here keeps it live, which the ROM's register allocation needs.
                    sample_ram_addr = AudioHeap_AllocPersistentSampleCache(size = sample->size, reloc_info->sampleBankId1, sample->sampleAddr, sample->medium);
                } else if (sample->medium == reloc_info->medium2) {
                    sample_ram_addr = AudioHeap_AllocPersistentSampleCache(sample->size, reloc_info->sampleBankId2, sample->sampleAddr, sample->medium);
                }
                break;

            case 1:
                if (sample->medium == reloc_info->medium1) {
                    sample_ram_addr = AudioHeap_AllocTemporarySampleCache(sample->size, reloc_info->sampleBankId1, sample->sampleAddr, sample->medium);
                } else if (sample->medium == reloc_info->medium2) {
                    sample_ram_addr = AudioHeap_AllocTemporarySampleCache(sample->size, reloc_info->sampleBankId2, sample->sampleAddr, sample->medium);
                }
                break;
        }

        if (sample_ram_addr == 0) {
            continue;
        }

        switch (async) {
            case 0:
                if (sample->medium == 1) {
                    AudioLoad_SyncDmaUnkMedium(sample->sampleAddr, sample_ram_addr, sample->size, *(s16*)(D_81288D44 + 2));
                    sample->sampleAddr = sample_ram_addr;
                    sample->medium = 0;
                } else {
                    AudioLoad_SyncDma(sample->sampleAddr, sample_ram_addr, sample->size, sample->medium);
                    sample->sampleAddr = sample_ram_addr;
                    sample->medium = 0;
                }
                break;

            case 1:
                D_81288018[D_81288A1C].sample = (Sample*)sample;
                D_81288018[D_81288A1C].ramAddr = (u8*)sample_ram_addr;
                D_81288018[D_81288A1C].encodedInfo = (D_81288A1C << 24) | 0xFFFFFF;
                D_81288018[D_81288A1C].isFree = 0;
                D_81288018[D_81288A1C].endAndMediumKey = sample->sampleAddr + sample->size + sample->medium;
                D_81288A1C++;
                break;
        }
    }
    D_81288A18 = 0;

    if ((D_81288A1C != 0) && !preload_in_progress) {
        sample = (Sample8110EF7C*)D_81288018[D_81288A1C - 1].sample;
        n_chunks = (sample->size >> 12) + 1;
        AudioLoad_StartAsyncLoad(sample->sampleAddr, (s32)D_81288018[D_81288A1C - 1].ramAddr, sample->size, sample->medium, n_chunks, &D_81288A98, D_81288018[D_81288A1C - 1].encodedInfo);
    }
}

/**
 * Finishes queued sample preloads. When the message for a finished DMA arrives,
 * the sample is pointed at its RAM copy (if it still matches the request), and
 * the next pending request on the stack is started. Returns 0 while a DMA is
 * still in flight or on reset (arg0 != 0), otherwise 1.
 * Corresponds to OoT AudioLoad_ProcessSamplePreloads (original name Nas_CheckBgWave). Stadium 1's
 * name BattleAnim_EndSleepStatusAnimation did not fit.
 */
s32 AudioLoad_ProcessSamplePreloads(s32 arg0) {
    Sample8110EF7C* sample;
    u32 preload_index;
    u32 key;
    u32 n_chunks;
    s32 pad; // FAKE: stack padding

    if (D_81288A1C > 0) {
        if (arg0 != 0) {
            osRecvMesg(&D_81288A98, &preload_index, 0);
            D_81288A1C = 0;
            return 0;
        }
        if (osRecvMesg(&D_81288A98, &preload_index, 0) == -1) {
            return 0;
        }

        preload_index >>= 24;
        if (D_81288018[preload_index].isFree == 0) {
            sample = (Sample8110EF7C*)D_81288018[preload_index].sample;
            key = sample->sampleAddr + sample->size + sample->medium;
            if (key == D_81288018[preload_index].endAndMediumKey) {
                sample->sampleAddr = (s32)D_81288018[preload_index].ramAddr;
                sample->medium = 0;
            }
            D_81288018[preload_index].isFree = 1;
        }

        for (;;) {
            if (D_81288A1C <= 0) {
                break;
            }
            if (D_81288018[D_81288A1C - 1].isFree == 1) {
                D_81288A1C--;
                continue;
            }

            sample = (Sample8110EF7C*)D_81288018[D_81288A1C - 1].sample;
            n_chunks = (sample->size >> 12) + 1;
            key = sample->sampleAddr + sample->size + sample->medium;
            if (key != D_81288018[D_81288A1C - 1].endAndMediumKey) {
                D_81288018[D_81288A1C - 1].isFree = 1;
                D_81288A1C--;
            } else {
                AudioLoad_StartAsyncLoad(sample->sampleAddr, (s32)D_81288018[D_81288A1C - 1].ramAddr, sample->size, sample->medium, n_chunks, &D_81288A98, D_81288018[D_81288A1C - 1].encodedInfo);
                break;
            }
        }
    }
    return 1;
}


/**
 * Appends `arg0` to the list `list` unless an entry with the same `key` (offset 0x4)
 * is already present among the first `count` entries.
 *
 * @param arg0  Item to add.
 * @param count Number of entries currently in `list`.
 * @param list  Array of item pointers; must have room for `count + 1` entries.
 * @return The new entry count (count + 1 if added, otherwise unchanged).
 * Corresponds to OoT AudioLoad_AddToSampleSet (original name __AddList).
 */
s32 AudioLoad_AddToSampleSet(Item8110F4D0* arg0, s32 count, Item8110F4D0** list) {
    s32 i;

    for (i = 0; i < count; i++) {
        if (arg0->key == list[i]->key) {
            break;
        }
    }
    if (i == count) {
        list[count] = arg0;
        count++;
    }
    return count;
}


/**
 * Collects the distinct samples of font `arg0`'s drums and instruments into `arg1` and returns how
 * many there are.
 * Corresponds to OoT AudioLoad_GetSamplesForFont (original name MakeWaveList).
 * @unverified Name follows the counterpart; not checked in play.
 */
s32 AudioLoad_GetSamplesForFont(s32 arg0, Item8110F4D0** arg1) {
    s32 count = 0;
    s32 i;
    s32 n1 = D_81288D50[arg0].numDrums;
    s32 n0 = D_81288D50[arg0].numInstruments;
    s32* p;
    u8* q;

    for (i = 0; i < n1; i++) {
        p = AudioPlayback_GetDrum(arg0, i);
        if (p != 0) {
            // FAKE: do-while(0) wrapper only changes register allocation; needed to match
            do {  // FAKE
                count = AudioLoad_AddToSampleSet((Item8110F4D0*)p[1], count, arg1);
            } while (0); // FAKE
        }
    }
    for (i = 0; i < n0; i++) {
        q = (u8*)AudioPlayback_GetInstrumentInner(arg0, i);
        if (q != 0) {
            if (q[1] != 0) {
                count = AudioLoad_AddToSampleSet(*(Item8110F4D0**)(q + 8), count, arg1);
            }
            if (q[2] != 0x7F) {
                count = AudioLoad_AddToSampleSet(*(Item8110F4D0**)(q + 0x18), count, arg1);
            }
            count = AudioLoad_AddToSampleSet(*(Item8110F4D0**)(q + 0x10), count, arg1);
        }
    }
    return count;
}


/**
 * Appends the sample *arg0 to the preload list D_81287E18 (count D_81288A18, no bounds check) when
 * it has a size, is flagged for preloading (unk_bit26) and is not in RAM (medium != 0).
 * AudioLoad_PreloadSamplesForFont collects a font's samples here and then preloads them.
 * Matching note: the bit-field reads are what produce the ROM's shift
 * sequence; the equivalent masks and shifts on a plain word differ in
 * temporary register numbering.
 * Corresponds to OoT AudioLoad_AddUsedSample (original name __Reload).
 */
void AudioLoad_AddUsedSample(SampleFlags8110F64C **arg0) {
    SampleFlags8110F64C *rec;

    rec = *arg0;
    if (rec->size && rec->unk_bit26 && rec->medium) {
        D_81287E18[D_81288A18] = rec;
        D_81288A18 += 1;
    }
}

/**
 * Gathers the samples used by a font's instruments and drums, then copies them into the sample
 * cache (async == 0) or queues them on the preload stack (async == 1).
 * Corresponds to OoT AudioLoad_PreloadSamplesForFont (original name WaveReload).
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name guessed from the code; comment kept.
 */
void AudioLoad_PreloadSamplesForFont(s32 font_id, s32 async, RelocInfo8110EF7C* reloc_info) {
    s32 num_drums;
    s32 num_instruments;
    s32 pad[2]; // FAKE
    u8* instrument;
    u8* drum;
    s32 sample_ram_addr;
    Sample8110EF7C* sample;
    s32 size;
    s32 n_chunks;
    s32 preload_in_progress;
    s32 i;

    preload_in_progress = 0;
    if (D_81288A1C != 0) {
        preload_in_progress = 1;
    }
    D_81288A18 = 0;
    num_instruments = D_81288D50[font_id].numInstruments;
    num_drums = D_81288D50[font_id].numDrums;

    for (i = 0; i < num_instruments; i++) {
        instrument = (u8*)AudioPlayback_GetInstrumentInner(font_id, i);
        if (instrument != NULL) {
            if (instrument[1] != 0) {
                AudioLoad_AddUsedSample((SampleFlags8110F64C**)(instrument + 8));
                if (!reloc_info->medium2) {} // karodo
            }
            if (instrument[2] != 0x7F) {
                AudioLoad_AddUsedSample((SampleFlags8110F64C**)(instrument + 0x18));
            }
            AudioLoad_AddUsedSample((SampleFlags8110F64C**)(instrument + 0x10));
        }
    }

    for (i = 0; i < num_drums; i++) {
        drum = (u8*)AudioPlayback_GetDrum(font_id, i);
        if (drum != NULL) {
            AudioLoad_AddUsedSample((SampleFlags8110F64C**)(drum + 4));
        }
    }

    if (D_81288A18 == 0) {
        return;
    }

    size = 0;
    for (i = 0; i < D_81288A18; i++) {
        size += ALIGN16(((Sample8110EF7C*)D_81287E18[i])->size);
    }
    if (size) {} // karodo

    for (i = 0; i < D_81288A18; i++) {
        if (D_81288A1C == 120) {
            break;
        }

        sample = (Sample8110EF7C*)D_81287E18[i];
        switch (async) {
            case 0:
                if (sample->medium == reloc_info->medium1) {
                    sample_ram_addr = AudioHeap_AllocPersistentSampleCache(sample->size, reloc_info->sampleBankId1, sample->sampleAddr, sample->medium);
                } else if (sample->medium == reloc_info->medium2) {
                    sample_ram_addr = AudioHeap_AllocPersistentSampleCache(sample->size, reloc_info->sampleBankId2, sample->sampleAddr, sample->medium);
                }
                break;

            case 1:
                if (sample->medium == reloc_info->medium1) {
                    sample_ram_addr = AudioHeap_AllocTemporarySampleCache(sample->size, reloc_info->sampleBankId1, sample->sampleAddr, sample->medium);
                } else if (sample->medium == reloc_info->medium2) {
                    sample_ram_addr = AudioHeap_AllocTemporarySampleCache(sample->size, reloc_info->sampleBankId2, sample->sampleAddr, sample->medium);
                }
                break;
        }

        if (sample_ram_addr == 0) {
            continue;
        }

        switch (async) {
            case 0:
                if (sample->medium == 1) {
                    AudioLoad_SyncDmaUnkMedium(sample->sampleAddr, sample_ram_addr, sample->size, *(s16*)(D_81288D44 + 2));
                    sample->sampleAddr = sample_ram_addr;
                    sample->medium = 0;
                } else {
                    AudioLoad_SyncDma(sample->sampleAddr, sample_ram_addr, sample->size, sample->medium);
                    sample->sampleAddr = sample_ram_addr;
                    sample->medium = 0;
                }
                break;

            case 1:
                D_81288018[D_81288A1C].sample = (Sample*)sample;
                D_81288018[D_81288A1C].ramAddr = (u8*)sample_ram_addr;
                D_81288018[D_81288A1C].encodedInfo = (D_81288A1C << 24) | 0xFFFFFF;
                D_81288018[D_81288A1C].isFree = 0;
                D_81288018[D_81288A1C].endAndMediumKey = sample->sampleAddr + sample->size + sample->medium;
                D_81288A1C++;
                break;
        }
    }
    D_81288A18 = 0;

    if ((D_81288A1C != 0) && !preload_in_progress) {
        sample = (Sample8110EF7C*)D_81288018[D_81288A1C - 1].sample;
        n_chunks = (sample->size >> 12) + 1;
        AudioLoad_StartAsyncLoad(sample->sampleAddr, (s32)D_81288018[D_81288A1C - 1].ramAddr, sample->size, sample->medium, n_chunks, &D_81288A98, D_81288018[D_81288A1C - 1].encodedInfo);
    }
}

extern s32 D_812878EC;
s32 AudioLoad_GetRealTableIndex(s32 arg0, s32 arg1);
void AudioLoad_PreloadSamplesForFont(s32 font_id, s32 async, RelocInfo8110EF7C* reloc_info);
/**
 * Preloads the samples of every permanently cached font.
 * Corresponds to OoT AudioLoad_LoadPermanentSamples (original name EmemReload).
 * Seen in play in pokestadium0 (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in
 * the Transfer Pak; called from AudioHeap_Init.
 */
void AudioLoad_LoadPermanentSamples(void) {
    AudioTable* base = (AudioTable*)AudioLoad_GetLoadTable(2);
    s32 i;
    s32 idx;
    s32 buf[8];
    s32 id;

    for (i = 0; i < D_812878EC; i++) {
        if (D_812878E0.entries[i].tableType == 1) {
            id = AudioLoad_GetRealTableIndex(1, D_812878E0.entries[i].id);
            buf[0] = D_81288D50[id].sampleBankId1;
            buf[0] = AudioLoad_GetRealTableIndex(2, buf[0]);
            buf[1] = D_81288D50[id].sampleBankId2;
            buf[1] = AudioLoad_GetRealTableIndex(2, buf[1]);
            buf[4] = base->entries[buf[0]].medium;
            idx = buf[1];
            buf[5] = base->entries[idx].medium;
            AudioLoad_PreloadSamplesForFont(id, 0, buf);
        }
    }
}
