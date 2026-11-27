/* Stadium Zero fragment000_80C00. */
/* Audio driver, matching OoT src/audio/internal/heap.c in function order. The driver is Nintendo's
 * "Nas" library (original names Nas_*); this revision sits between SM64 Shindou and OoT. See
 * docs/annotation/audio-driver.md for the per-function mapping. */
/* C ported from the Stadium 1 decomp: src/fragments/gb_tower_emulator/gb_tower_emulator_86920.c. */

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

u32 GbApu_AllocZeroed(AudioAllocPool* arg0, s32 arg1);
s32 GbApu_Alloc(AudioAllocPool* arg0, s32 arg1);
void AudioHeap_InitPool(AudioAllocPool* arg0, s32 arg1, s32 arg2);

extern u8 D_81287AF4[1];
extern u8 D_81287AF5[1];
extern u8 D_81287AF6[1];

/**
 * Marks the fonts that are not permanently loaded (status 5) as unloaded.
 * Corresponds to OoT AudioHeap_ResetLoadStatus (original name Nas_ResetIDtable).
 * Seen in play (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from AudioHeap_Init.
 */
void AudioHeap_ResetLoadStatus(void) {
    s32 i;

    for (i = 0; i < 1; i++) {
        if (D_81287AF5[i] != 5) {
            D_81287AF5[i] = 0;
        }
    }
    for (i = 0; i < 1; i++) {
        if (D_81287AF4[i] != 5) {
            D_81287AF4[i] = 0;
        }
    }
    for (i = 0; i < 1; i++) {
        if (D_81287AF6[i] != 5) {
            D_81287AF6[i] = 0;
        }
    }
}

s32 AudioPlayback_NoteDisable(Note*);
s32 AudioPlayback_AudioListRemove(Note*);
s32 AudioSeq_AudioListPushBack(s8*, Note*);
extern Note* D_81287B04;
extern s8 D_81287DD8;
extern s32 D_81288D88;

/**
 * Stops every note that plays font arg0. Walks the D_81288D88 notes at D_81287B04 (the array
 * pointer is re-read on every iteration); for each note whose playbackState.fontId is arg0, it
 * first disables and finishes the parent layer when playbackState.unk_04 is 0 and the note has a
 * priority, then disables the note, takes it off its list and pushes it onto the disabled list
 * D_81287DD8.
 *
 * Corresponds to OoT AudioHeap_DiscardFont (original name Nas_ForceStopChannel).
 * Matching notes: the two single-bit writes are declared as bit-fields; the
 * bit-field form gives the retail `andi 0xFF7F` / `ori 0x40` read-modify-write
 * sequences and register numbering (a plain `&= 0xFF7F; |= 0x40` on a byte
 * was one temporary register off). Indexing the array with the loop counter,
 * instead of a separate byte offset, gives the retail loop setup order.
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name guessed from the code; comment kept.
 */
void AudioHeap_DiscardFont(s32 arg0) {
    s32 i;
    Note* p;

    for (i = 0; i < D_81288D88; i++) {
        p = &D_81287B04[i];
        if (arg0 == p->playbackState.fontId) {
            if ((p->playbackState.unk_04 == 0) && (p->playbackState.priority != 0)) {
                p->playbackState.parentLayer->enabled = 0;
                p->playbackState.parentLayer->finished = 1;
            }
            AudioPlayback_NoteDisable(p);
            AudioPlayback_AudioListRemove(p);
            AudioSeq_AudioListPushBack(&D_81287DD8, p);
        }
    }
}

typedef struct unk_D_81184668 {
    /* 0x000 */ u32 enabled : 1;
    /* 0x000 */ u32 unk_00_bits : 31;
    /* 0x004 */ u8 seqId;
    /* 0x005 */ char unk005[0x15C - 0x5];
} unk_D_81184668; // size = 0x15C

void AudioSeq_SequencePlayerDisable(unk_D_81184668* seqPlayer);
extern unk_D_81184668 D_81287B08[];
extern s16 D_81288D6C;
/**
 * Stops every sequence player that plays sequence `arg0` (AudioSeq_SequencePlayerDisable is
 * AudioSeq_SequencePlayerDisable).
 * Corresponds to OoT AudioHeap_DiscardSequence (original name Nas_ForceStopSeq).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void AudioHeap_DiscardSequence(s32 arg0) {
    s32 i;

    for (i = 0; i < D_81288D6C; i++) {
        if (D_81287B08[i].enabled && D_81287B08[i].seqId == arg0) {
            AudioSeq_SequencePlayerDisable(&D_81287B08[i]);
        }
    }
}

/**
 * Like alHeapDBAlloc: carves a 16-byte aligned block and zeroes it. Returns 0 when the arena is full.
 * Corresponds to OoT GbApu_AllocZeroed (original name Nas_HeapAlloc_CL). Zero clears the block
 * in its own loop; OoT calls GbApu_Alloc and then clears. Stadium 1's name GbApu_AllocZeroed
 * did not fit.
 * Seen in play (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from AudioHeap_Init.
 */
u32 GbApu_AllocZeroed(AudioAllocPool* arg0, s32 arg1) {
  s32 temp_v0;
  u32 temp_a2;
  u32 var_a1;
  temp_v0 = ALIGN16(arg1);
  temp_a2 = arg0->curRamAddr;
  if ((u32)(arg0->startRamAddr + arg0->size) >= (temp_a2 + temp_v0)) {
      arg0->curRamAddr += temp_v0;
      var_a1 = temp_a2;
      while (var_a1 < (u32)arg0->curRamAddr) {
          *((u8*)var_a1++) = 0;
      }
  } else {
      return 0;
  }
  arg0->numEntries++;
  return temp_a2;
}

/**
 * GbApu_AllocZeroed without the clear.
 * Corresponds to OoT GbApu_Alloc (original name Nas_HeapAlloc). Same 16-byte alignment, bounds
 * test and entry count. Stadium 1's name GbApu_Alloc did not fit.
 * Seen in play (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from AudioLoad_Init.
 */
s32 GbApu_Alloc(AudioAllocPool* arg0, s32 arg1) {
  s32 temp_a2;
  u32 temp_v0;

  temp_v0 = ALIGN16(arg1);
  temp_a2 = arg0->curRamAddr;
  if ((arg0->startRamAddr + arg0->size) >= (arg0->curRamAddr + temp_v0)) {
      arg0->curRamAddr = temp_v0 + temp_a2;
  } else {
      return 0;
  }
  arg0->numEntries++;
  return temp_a2;
}

/**
 * Like alHeapInit.
 * Corresponds to OoT AudioHeap_InitPool (original name Nas_HeapInit). Stadium 1's name
 * GbApu_InitAllocator did not fit.
 * Seen in play (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from AudioHeap_InitMainPools.
 */
void AudioHeap_InitPool(AudioAllocPool* arg0, s32 arg1, s32 arg2) {
  arg0->startRamAddr = ALIGN16(arg1);
  arg0->curRamAddr = ALIGN16(arg1);
  arg0->size = (s32) (arg2 - (arg1 & 0xF));
  arg0->numEntries = 0;
}

/**
 * Empties a persistent cache and rewinds its pool.
 * Corresponds to OoT AudioHeap_InitPersistentCache (original name Nas_SzStayClear). This was
 * AudioHeap_PersistentCacheClear; OoT and MM use AudioHeap_InitPersistentCache for this reset.
 * Seen in play (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from AudioHeap_InitPersistentPoolsAndCaches.
 */
void AudioHeap_InitPersistentCache(s8 *arg0) {
    M2C_FIELD(arg0, s32 *, 0x10) = 0;
    M2C_FIELD(arg0, s32 *, 0) = 0;
    M2C_FIELD(arg0, s32 *, 8) = (s32) M2C_FIELD(arg0, s32 *, 4);
}

/**
 * Empties an AudioTemporaryCache: nextSide (+0x00) and pool.numEntries (+0x10) become 0, the pool is
 * rewound (pool.curRamAddr = pool.startRamAddr), entry 0 is placed at the pool's start and entry 1
 * at its end (start + size), and both entry ids (+0x1E, +0x2A) become -1 (free).
 *
 * The sum is written with the fields in this order, and read from memory
 * rather than through a local, because IDO orders the two operands of the
 * addu differently otherwise.
 * Corresponds to OoT AudioHeap_InitTemporaryCache (original name Nas_SzAutoClear). This was
 * AudioHeap_TemporaryCacheClear; OoT and MM use AudioHeap_InitTemporaryCache for this reset.
 * Seen in play (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from AudioHeap_InitTemporaryPoolsAndCaches.
 */
void AudioHeap_InitTemporaryCache(s32* arg0) {
    arg0[4] = 0;
    arg0[0] = 0;
    arg0[8] = arg0[3] + arg0[1];
    ((s16*)arg0)[0xF] = -1;
    ((s16*)arg0)[0x15] = -1;
    arg0[2] = arg0[1];
    arg0[5] = arg0[1];
}

/**
 * Rewinds an allocation pool to its start.
 * Corresponds to OoT AudioHeap_ResetPool (original name Nas_SzCustomClear).
 * No caller (2026-11-13): nothing in the built ROM jumps to it or holds its address (no jal or j,
 * no pointer word, no lui/addiu pair in any fragment), so play cannot reach it.
 * @unverified Name and comment guessed from the code.
 */
void AudioHeap_ResetPool(s8 *arg0) {
    M2C_FIELD(arg0, s32 *, 0xC) = 0;
    M2C_FIELD(arg0, s32 *, 4) = (s32) M2C_FIELD(arg0, s32 *, 0);
}

void AudioHeap_InitPool(AudioAllocPool* arg0, s32 arg1, s32 arg2);
extern M2C_UNK D_81287730;
extern M2C_UNK D_81287750;
extern u8 D_81289710[];
extern s32 D_81255B6C;
extern s32 D_81287740;

/**
 * Splits the audio heap D_81289710 (D_81255B6C bytes): the first arg0 bytes become the init pool
 * D_81287750, the rest the session pool D_81287730, and the external pool D_81287740 is cleared.
 * AudioHeap_InitPool is AudioHeap_InitPool and GbApu_Alloc is GbApu_Alloc; this file still calls
 * them by address.
 * Corresponds to OoT AudioHeap_InitMainPools (original name Nas_SzHeapReset).
 * Seen in play (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from AudioLoad_Init.
 */
void AudioHeap_InitMainPools(s32 arg0) {
    AudioHeap_InitPool(&D_81287750, (s32)D_81289710, arg0);
    AudioHeap_InitPool(&D_81287730, (s32)(D_81289710 + arg0), D_81255B6C - arg0);
    D_81287740 = 0;
}

s32 GbApu_Alloc(AudioAllocPool* arg0, s32 arg1);
void AudioHeap_InitPool(AudioAllocPool* arg0, s32 arg1, s32 arg2);
extern M2C_UNK D_81287730;
extern M2C_UNK D_81287760;
extern M2C_UNK D_81287790;

/**
 * Splits the session pool into the misc and cache pools.
 * Corresponds to OoT AudioHeap_InitSessionPools (original name Nas_SzHeapDivide).
 * Seen in play (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from AudioHeap_Init.
 */
void AudioHeap_InitSessionPools(s8 *arg0) {
    M2C_FIELD(&D_81287730, s32 *, 4) = (s32) M2C_FIELD(&D_81287730, s32 *, 0);
    AudioHeap_InitPool(&D_81287760, GbApu_Alloc(&D_81287730, M2C_FIELD(arg0, s32 *, 0)), M2C_FIELD(arg0, s32 *, 0));
    AudioHeap_InitPool(&D_81287790, GbApu_Alloc(&D_81287730, M2C_FIELD(arg0, s32 *, 0xC)), M2C_FIELD(arg0, s32 *, 0xC));
}

s32 GbApu_Alloc(AudioAllocPool* arg0, s32 arg1);
void AudioHeap_InitPool(AudioAllocPool* arg0, s32 arg1, s32 arg2);
extern M2C_UNK D_81287790;
extern M2C_UNK D_812877A0;
extern M2C_UNK D_812877B0;

/**
 * Splits the cache pool into the persistent and temporary pools.
 * Corresponds to OoT AudioHeap_InitCachePools (original name Nas_SzDataDivide).
 * Seen in play (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from AudioHeap_Init.
 */
void AudioHeap_InitCachePools(s8 *arg0) {
    M2C_FIELD(&D_81287790, s32 *, 4) = (s32) M2C_FIELD(&D_81287790, s32 *, 0);
    AudioHeap_InitPool(&D_812877A0, GbApu_Alloc(&D_81287790, M2C_FIELD(arg0, s32 *, 0)), M2C_FIELD(arg0, s32 *, 0));
    AudioHeap_InitPool(&D_812877B0, GbApu_Alloc(&D_81287790, M2C_FIELD(arg0, s32 *, 4)), M2C_FIELD(arg0, s32 *, 4));
}

s32 GbApu_Alloc(AudioAllocPool* arg0, s32 arg1);
void AudioHeap_InitPool(AudioAllocPool* arg0, s32 arg1, s32 arg2);
extern M2C_UNK D_812877A0;
extern M2C_UNK D_812877C0;
extern M2C_UNK D_812877C4;
extern M2C_UNK D_81287820;
extern M2C_UNK D_81287824;
extern M2C_UNK D_81287880;
extern M2C_UNK D_81287884;

/**
 * Splits the persistent pool between the sequence, font and sample caches.
 * Corresponds to OoT AudioHeap_InitPersistentPoolsAndCaches (original name Nas_SzStayDivide). This
 * was AudioHeap_InitPersistentCache, which OoT uses for the reset above.
 * Seen in play (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from AudioHeap_Init.
 */
void AudioHeap_InitPersistentPoolsAndCaches(s8 *arg0) {
    M2C_FIELD(&D_812877A0, s32 *, 4) = (s32) M2C_FIELD(&D_812877A0, s32 *, 0);
    AudioHeap_InitPool(&D_812877C4, GbApu_Alloc(&D_812877A0, M2C_FIELD(arg0, s32 *, 0)), M2C_FIELD(arg0, s32 *, 0));
    AudioHeap_InitPool(&D_81287824, GbApu_Alloc(&D_812877A0, M2C_FIELD(arg0, s32 *, 4)), M2C_FIELD(arg0, s32 *, 4));
    AudioHeap_InitPool(&D_81287884, GbApu_Alloc(&D_812877A0, M2C_FIELD(arg0, s32 *, 8)), M2C_FIELD(arg0, s32 *, 8));
    AudioHeap_InitPersistentCache(&D_812877C0);
    AudioHeap_InitPersistentCache(&D_81287820);
    AudioHeap_InitPersistentCache(&D_81287880);
}

s32 GbApu_Alloc(AudioAllocPool* arg0, s32 arg1);
void AudioHeap_InitPool(AudioAllocPool* arg0, s32 arg1, s32 arg2);
extern M2C_UNK D_812877B0;
extern M2C_UNK D_812877E0;
extern M2C_UNK D_812877E4;
extern M2C_UNK D_81287840;
extern M2C_UNK D_81287844;
extern M2C_UNK D_812878A0;
extern M2C_UNK D_812878A4;

/**
 * Splits the temporary pool between the sequence, font and sample caches.
 * Corresponds to OoT AudioHeap_InitTemporaryPoolsAndCaches (original name Nas_SzAutoDivide). This
 * was AudioHeap_InitTemporaryCache, which OoT uses for the reset above.
 * Seen in play (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from AudioHeap_Init.
 */
void AudioHeap_InitTemporaryPoolsAndCaches(s8 *arg0) {
    M2C_FIELD(&D_812877B0, s32 *, 4) = (s32) M2C_FIELD(&D_812877B0, s32 *, 0);
    AudioHeap_InitPool(&D_812877E4, GbApu_Alloc(&D_812877B0, M2C_FIELD(arg0, s32 *, 0)), M2C_FIELD(arg0, s32 *, 0));
    AudioHeap_InitPool(&D_81287844, GbApu_Alloc(&D_812877B0, M2C_FIELD(arg0, s32 *, 4)), M2C_FIELD(arg0, s32 *, 4));
    AudioHeap_InitPool(&D_812878A4, GbApu_Alloc(&D_812877B0, M2C_FIELD(arg0, s32 *, 8)), M2C_FIELD(arg0, s32 *, 8));
    AudioHeap_InitTemporaryCache(&D_812877E0);
    AudioHeap_InitTemporaryCache(&D_81287840);
    AudioHeap_InitTemporaryCache(&D_812878A0);
}




extern u8 D_81287AF4[1];
extern u8 D_81287AF5[1];
extern u8 D_81287AF6[1];
void AudioLoad_SetFontLoadStatus(s32, s32);

/**
 * Allocates `size` bytes for entry `id` of a table's persistent or temporary cache, evicting the older temporary entry when needed.
 * Corresponds to OoT AudioHeap_AllocCached (original name Nas_SzHeapAlloc). OoT also discards
 * sample banks when it evicts (AudioHeap_DiscardSampleBank); Zero does not.
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void* AudioHeap_AllocCached(s32 table_type, s32 size, s32 cache, s32 id) {
    AudioCache* loaded_cache;
    AudioTemporaryCache* t_cache;
    AudioAllocPool* temp_pool;
    void* persistent_addr;
    void* temp_addr;
    u8 load_status_entry0;
    u8 load_status_entry1;
    s32 i;
    u8* load_status;

    switch (table_type) {
        case 0:
            loaded_cache = (AudioCache*)&D_812877C0;
            load_status = D_81287AF6;
            break;
        case 1:
            loaded_cache = (AudioCache*)&D_81287820;
            load_status = D_81287AF5;
            break;
        case 2:
            loaded_cache = (AudioCache*)&D_81287880;
            load_status = D_81287AF4;
            break;
    }

    if (cache == 0) {
        t_cache = &loaded_cache->temporary;
        temp_pool = &t_cache->pool;

        if (t_cache->entries[0].id == -1) {
            load_status_entry0 = 0;
        } else {
            load_status_entry0 = load_status[t_cache->entries[0].id];
        }

        if (t_cache->entries[1].id == -1) {
            load_status_entry1 = 0;
        } else {
            load_status_entry1 = load_status[t_cache->entries[1].id];
        }

        if (table_type == 1) {
            if (load_status_entry0 == 4) {
                for (i = 0; i < D_81288D88; i++) {
                    if (D_81287B04[i].playbackState.fontId == t_cache->entries[0].id && D_81287B04[i].sampleState.enabled) {
                        break;
                    }
                }

                if (i == D_81288D88) {
                    AudioLoad_SetFontLoadStatus(t_cache->entries[0].id, 3);
                    load_status_entry0 = 3;
                }
            }

            if (load_status_entry1 == 4) {
                for (i = 0; i < D_81288D88; i++) {
                    if (D_81287B04[i].playbackState.fontId == t_cache->entries[1].id && D_81287B04[i].sampleState.enabled) {
                        break;
                    }
                }

                if (i == D_81288D88) {
                    AudioLoad_SetFontLoadStatus(t_cache->entries[1].id, 3);
                    load_status_entry1 = 3;
                }
            }
        }

        if (load_status_entry0 == 0) {
            t_cache->nextSide = 0;
        } else if (load_status_entry1 == 0) {
            t_cache->nextSide = 1;
        } else if (load_status_entry0 == 3 && load_status_entry1 == 3) {
            // Use the opposite side from last time.
        } else if (load_status_entry0 == 3) {
            t_cache->nextSide = 0;
        } else if (load_status_entry1 == 3) {
            t_cache->nextSide = 1;
        } else {
            if (table_type == 0) {
                if (load_status_entry0 == 2) {
                    for (i = 0; i < D_81288D6C; i++) {
                        if (((SequencePlayer*)&D_81287B08)[i].enabled && ((SequencePlayer*)&D_81287B08)[i].seqId == t_cache->entries[0].id) {
                            break;
                        }
                    }
                    if (i == D_81288D6C) {
                        t_cache->nextSide = 0;
                        goto done;
                    }
                }
                if (load_status_entry1 == 2) {
                    for (i = 0; i < D_81288D6C; i++) {
                        if (((SequencePlayer*)&D_81287B08)[i].enabled && ((SequencePlayer*)&D_81287B08)[i].seqId == t_cache->entries[1].id) {
                            break;
                        }
                    }
                    if (i == D_81288D6C) {
                        t_cache->nextSide = 1;
                        goto done;
                    }
                }
            } else if (table_type == 1) {
                if (load_status_entry0 == 2) {
                    for (i = 0; i < D_81288D88; i++) {
                        if (D_81287B04[i].playbackState.fontId == t_cache->entries[0].id && D_81287B04[i].sampleState.enabled) {
                            break;
                        }
                    }
                    if (i == D_81288D88) {
                        t_cache->nextSide = 0;
                        goto done;
                    }
                }
                if (load_status_entry1 == 2) {
                    for (i = 0; i < D_81288D88; i++) {
                        if (D_81287B04[i].playbackState.fontId == t_cache->entries[1].id && D_81287B04[i].sampleState.enabled) {
                            break;
                        }
                    }
                    if (i == D_81288D88) {
                        t_cache->nextSide = 1;
                        goto done;
                    }
                }
            }

            if (t_cache->nextSide == 0) {
                if (load_status_entry0 == 1) {
                    if (load_status_entry1 == 1) {
                        goto fail;
                    }
                    t_cache->nextSide = 1;
                }
            } else {
                if (load_status_entry1 == 1) {
                    if (load_status_entry0 == 1) {
                        goto fail;
                    }
                    t_cache->nextSide = 0;
                }
            }

            if (0) { // FAKE: dead block that only holds the fail label
            fail:
                return NULL;
            }
        }

    done:
        if (t_cache->entries[t_cache->nextSide].id != -1) {
            load_status[t_cache->entries[t_cache->nextSide].id] = 0;
            if (table_type == 1) {
                AudioHeap_DiscardFont(t_cache->entries[t_cache->nextSide].id);
            }
        }

        switch (t_cache->nextSide) {
            case 0:
                t_cache->entries[0].ramAddr = temp_pool->startRamAddr;
                t_cache->entries[0].id = id;
                t_cache->entries[0].size = size;
                temp_pool->curRamAddr = temp_pool->startRamAddr + size;

                if (t_cache->entries[1].id != -1 && t_cache->entries[1].ramAddr < temp_pool->curRamAddr) {
                    load_status[t_cache->entries[1].id] = 0;
                    switch (table_type) {
                        case 0:
                            AudioHeap_DiscardSequence(t_cache->entries[1].id);
                            break;
                        case 1:
                            AudioHeap_DiscardFont(t_cache->entries[1].id);
                            break;
                    }
                    t_cache->entries[1].id = -1;
                    t_cache->entries[1].ramAddr = temp_pool->startRamAddr + temp_pool->size;
                }
                temp_addr = t_cache->entries[0].ramAddr;
                break;

            case 1:
                t_cache->entries[1].ramAddr = (u8*)((u32)(temp_pool->startRamAddr + temp_pool->size - size) & ~0xF);
                t_cache->entries[1].id = id;
                t_cache->entries[1].size = size;
                if (t_cache->entries[0].id != -1 && t_cache->entries[1].ramAddr < temp_pool->curRamAddr) {
                    load_status[t_cache->entries[0].id] = 0;
                    switch (table_type) {
                        case 0:
                            AudioHeap_DiscardSequence(t_cache->entries[0].id);
                            break;
                        case 1:
                            AudioHeap_DiscardFont(t_cache->entries[0].id);
                            break;
                    }
                    t_cache->entries[0].id = -1;
                    temp_pool->curRamAddr = temp_pool->startRamAddr;
                }
                temp_addr = t_cache->entries[1].ramAddr;
                break;

            default:
                return NULL;
        }

        t_cache->nextSide ^= 1;
        return temp_addr;
    }

    persistent_addr = (void*)GbApu_Alloc((M2C_UNK*)&loaded_cache->persistent.pool, size);
    loaded_cache->persistent.entries[loaded_cache->persistent.numEntries].ramAddr = persistent_addr;

    if (persistent_addr == NULL) {
        switch (cache) {
            case 2:
                return AudioHeap_AllocCached(table_type, size, 0, id);
            case 0:
            case 1:
                return NULL;
        }
    }

    loaded_cache->persistent.entries[loaded_cache->persistent.numEntries].id = id;
    loaded_cache->persistent.entries[loaded_cache->persistent.numEntries].size = size;
    return loaded_cache->persistent.entries[loaded_cache->persistent.numEntries++].ramAddr;
}

s32 AudioHeap_SearchRegularCaches(s32 arg0, s32 arg1, s32 arg2);
s32 AudioHeap_SearchPermanentCache(s32 arg0, s32 arg1);

/**
 * Looks up entry `arg2` of table `arg0`, in the permanent cache first, then the regular caches
 * (none when cache policy `arg1` is 3).
 * Corresponds to OoT AudioHeap_SearchCaches (original name Nas_SzCacheCheck).
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment guessed from the code.
 */
s32 AudioHeap_SearchCaches(s32 arg0, s32 arg1, s32 arg2) {
    s32 result = AudioHeap_SearchPermanentCache(arg0, arg2);

    if (result != 0) {
        return result;
    }
    if (arg1 == 3) {
        return 0;
    }
    return AudioHeap_SearchRegularCaches(arg0, arg1, arg2);
}

/**
 * Looks up entry `arg2` of table `arg0` in its temporary or persistent cache.
 * Corresponds to OoT AudioHeap_SearchRegularCaches (original name __Nas_SzCacheCheck_Inner).
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment guessed from the code.
 */
s32 AudioHeap_SearchRegularCaches(s32 arg0, s32 arg1, s32 arg2) {
    AudioTemporaryCache* temporary;
    AudioCache* tbl;
    AudioPersistentCache* persistent;
    u32 i;
    switch (arg0) {
    case 0:
        tbl = (AudioCache*)&D_812877C0;
        break;
    case 1:
        tbl = (AudioCache*)&D_81287820;
        break;
    case 2:
        tbl = (AudioCache*)&D_81287880;
        break;
    }
    temporary = &tbl->temporary;
    if (arg1 == 0) {
        if (arg2 == temporary->entries[0].id) {
            temporary->nextSide = 1;
            return (s32)temporary->entries[0].ramAddr;
        }
        if (arg2 == temporary->entries[1].id) {
            temporary->nextSide = 0;
            return (s32)temporary->entries[1].ramAddr;
        }
        return 0;
    }
    persistent = &tbl->persistent;
    for (i = 0; i < persistent->numEntries; i++) {
        if (arg2 == tbl->persistent.entries[i].id) {
            return (s32)tbl->persistent.entries[i].ramAddr;
        }
    }
    if (arg1 == 2) {
        return AudioHeap_SearchCaches(arg0, 0, arg2);
    }
    return 0;
}

/**
 * Unused in the code seen so far. Fills out[0..15] from two recurrences in p
 * and q seeded with 262159.0f. The first loop writes to out instead of tmp, so
 * the second loop copies partly uninitialized values.
 * Corresponds to OoT func_800DF1D8 (original name Nas_InitFilterCoef; MM func_8018C4E4): the same
 * 262159.0f seeds, the same four initial terms and the same out/tmp bug.
 * No caller (2026-11-13): nothing in the built ROM jumps to it or holds its address (no jal or j,
 * no pointer word, no lui/addiu pair in any fragment), so play cannot reach it.
 * @unverified Name guessed from the code; comment kept.
 */
void AudioHeap_ComputeFilterCoefficients(f32 p, f32 q, u16* out) {
    s32 i;
    f32 tmp[16];

    tmp[0] = (f32)(q * 262159.0f);
    tmp[8] = (f32)(p * 262159.0f);
    tmp[1] = (f32)((q * p) * 262159.0f);
    tmp[9] = (f32)(((p * p) + q) * 262159.0f);

    //!bug Writes out instead of tmp, so the copy below reads tmp[2..7] and tmp[10..15] uninitialized.
    for (i = 2; i < 8; i++) {
        out[i] = q * tmp[i - 2] + p * tmp[i - 1];
        out[8 + i] = q * tmp[6 + i] + p * tmp[7 + i];
    }

    for (i = 0; i < 16; i++) {
        out[i] = tmp[i];
    }
}

/**
 * AudioHeap_ClearFilter
 *
 * Sets the eight s16 values at arg0 to zero (the loop is unrolled by four in
 * the retail code).
 * Corresponds to OoT AudioHeap_ClearFilter (original name Nas_ClearFilter).
 * No caller (2026-11-13): nothing in the built ROM jumps to it or holds its address (no jal or j,
 * no pointer word, no lui/addiu pair in any fragment), so play cannot reach it.
 * @unverified Name guessed from the code; comment kept.
 */
void AudioHeap_ClearFilter(s16* arg0) {
    s32 i;

    for (i = 0; i < 8; i++) {
        arg0[i] = 0;
    }
}

extern s16 D_8123A648[];

/**
 * AudioHeap_LoadLowPassFilter
 *
 * Copies eight s16 values into arg0 from entry arg1 of the table at
 * D_8123A648 (entries are 8 s16 = 16 bytes long; the loop is unrolled by four
 * in the retail code).
 * Corresponds to OoT AudioHeap_LoadLowPassFilter (original name Nas_SetLPFilter).
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name guessed from the code; comment kept.
 */
void AudioHeap_LoadLowPassFilter(s16* arg0, s32 arg1) {
    s16* src;
    s32 i;

    src = &D_8123A648[arg1 * 8];
    for (i = 0; i < 8; i++) {
        arg0[i] = src[i];
    }
}

extern s16 D_8123A838[];

/**
 * AudioHeap_LoadHighPassFilter
 *
 * Copies eight s16 values into arg0 from the table at D_8123A838. The entry is
 * numbered from 1: entry arg1 starts at D_8123A838 + (arg1 - 1) * 16 bytes
 * (8 s16 per entry; the loop is unrolled by four in the retail code).
 * Corresponds to OoT AudioHeap_LoadHighPassFilter (original name Nas_SetHPFilter).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name guessed from the code; comment kept.
 */
void AudioHeap_LoadHighPassFilter(s16* arg0, s32 arg1) {
    s16* src;
    s32 i;

    src = &D_8123A838[arg1 * 8 - 8];
    for (i = 0; i < 8; i++) {
        arg0[i] = src[i];
    }
}

/**
 * Loads a low-pass, high-pass or band-pass filter from the two cutoffs.
 * Corresponds to OoT AudioHeap_LoadFilter (original name Nas_SetBPFilter).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void AudioHeap_LoadFilter(s16* arg0, s32 arg1, s32 arg2) {
    s32 i;

    if ((arg1 == 0) && (arg2 == 0)) {
        AudioHeap_LoadLowPassFilter(arg0, 0);
    } else if (arg2 == 0) {
        AudioHeap_LoadLowPassFilter(arg0, arg1);
    } else if (arg1 == 0) {
        AudioHeap_LoadHighPassFilter(arg0, arg2);
    } else {
        for (i = 0; i < 8; i++) {
            arg0[i] = (D_8123A648[arg2 * 8 + i] + D_8123A838[arg2 * 8 + i]) / 2;
        }
    }
}

extern SynthesisReverb D_812875B0[];
extern s8 D_8128771B;
/**
 * Steps every reverb's settings once per audio frame.
 * Corresponds to OoT AudioHeap_UpdateReverbs (original name __Nas_DelayDown). OoT moves the
 * per-reverb body into AudioHeap_UpdateReverb.
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment guessed from the code.
 */
void AudioHeap_UpdateReverbs(void) {
    s32 n;
    s32 i;
    s32 j;

    if (*(s16*)&sGbAudioOutputConfig == 2) {
        n = 2;
    } else {
        n = 1;
    }
    for (i = 0; i < D_8128771B; i++) {
        for (j = 0; j < n; j++) {
            D_812875B0[i].decayRatio -= D_812875B0[i].decayRatio / 3;
        }
    }
}

extern s16 D_81288D62;

/**
 * Clears the audio buffer that is being filled.
 * Corresponds to OoT AudioHeap_ClearCurrentAiBuffer (original name __Nas_DacClear).
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment guessed from the code.
 */
void AudioHeap_ClearCurrentAiBuffer(void) {
    s32 idx = sGbAudioBufferIndex;
    s32 i;

    sGbAudioBufferLengths[idx] = D_81288D62;
    for (i = 0; i < 2016; i++) {
        sGbAudioBuffers[idx][i] = 0;
    }
}

extern volatile u8 D_81287AF7;
extern s32 D_81287AFC;
void AudioHeap_Init(void);
void AudioHeap_DiscardSampleCaches(void);

/**
 * Runs one step of the audio reset: fade out, stop notes, clear buffers, reinitialize.
 * Corresponds to OoT AudioHeap_ResetStep (original name Nas_SpecChange). Zero calls
 * AudioSeq_SequencePlayerDisable where OoT calls AudioSeq_SequencePlayerDisableAsFinished, and has
 * no sample-bank discard.
 * Seen in play (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from AudioLoad_Init.
 */
s32 AudioHeap_ResetStep(void) {
    s32 i;
    s32 j;
    s32 sp24;

    if (sGbAudioOutputConfig.specUnk4 == 2) {
        sp24 = 2;
    } else {
        sp24 = 1;
    }

    switch (D_81287AF7) {
        case 5:
            for (i = 0; i < sGbAudioOutputConfig.numSequencePlayers; i++) {
                AudioSeq_SequencePlayerDisable(&D_81287B08[i]);
            }
            D_81287AFC = 4 / sp24;
            D_81287AF7--;
            break;
        case 4:
            if (D_81287AFC != 0) {
                D_81287AFC--;
                AudioHeap_UpdateReverbs();
            } else {
                for (i = 0; i < D_81288D88; i++) {
                    if (D_81287B04[i].sampleState.enabled && D_81287B04[i].playbackState.adsr.state != 0) {
                        D_81287B04[i].playbackState.adsr.fadeOutVel = sGbAudioOutputConfig.ticksPerUpdateInv;
                        D_81287B04[i].playbackState.adsr.action |= 0x10;
                    }
                }
                D_81287AFC = 16 / sp24;
                D_81287AF7--;
            }
            break;
        case 3:
            if (D_81287AFC != 0) {
                D_81287AFC--;
                AudioHeap_UpdateReverbs();
            } else {
                D_81287AFC = 4 / sp24;
                D_81287AF7--;
            }
            break;
        case 2:
            AudioHeap_ClearCurrentAiBuffer();
            if (!D_81287AFC && !D_81287AFC && !D_81287AFC) {} // FAKE: empty test found by a fake search; only changes register use
            if (D_81287AFC != 0) {
                D_81287AFC--;
            } else {
                D_81287AF7--;
                AudioHeap_DiscardSampleCaches();
            }
            if (!sGbAudioOutputConfig.specUnk4) {} // karodo
            break;
        case 1:
            AudioHeap_Init();
            D_81287AF7 = 0;
            for (i = 0; i < 3; i++) {
                sGbAudioBufferLengths[i] = sGbAudioOutputConfig.maxAiBufferLength;
                for (j = 0; j < 2016; j++) {
                    sGbAudioBuffers[i][j] = 0;
                }
            }
            break;
    }

    if (D_81287AF7 < 3) {
        return 0;
    }
    return 1;
}


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
// AudioHeap_InitSampleCaches is defined as void in the next file, but this file calls it as
// if it returned a value (as with an implicit declaration); the call only
// matches that way.
s32 AudioHeap_InitSampleCaches(s32, s32);
void AudioLoad_InitSampleDmaBuffers(s32);
void AudioSeq_ResetSequencePlayer(s32);
void osWritebackDCacheAll(void);
void AudioLoad_LoadPermanentSamples(void);
void AudioPlayback_InitNoteFreeList(void);
void AudioPlayback_NoteInitAll(void);
void AudioSeq_InitSequencePlayerChannels(s32);
void AudioSeq_InitSequencePlayers(void);

#define EXTERNAL_POOL_AFF0 (*(AudioAllocPool*)&D_81287740)

/**
 * Lays out the audio heap and reverbs for the current audio spec.
 * Corresponds to OoT AudioHeap_Init (original name __Nas_MemoryReconfig). OoT adds the external
 * pool, ADSR decay table and the slow/async/script load initializers.
 * Seen in play (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from AudioHeap_ResetStep.
 */
void AudioHeap_Init(void) {
    UNUSED s32 pad[5]; // FAKE: stack pad of unknown origin; only reserves frame space
    s32 persistent_mem;
    s32 temporary_mem;
    s32 total_mem;
    s32 want_misc;
    s32 j;
    s32 i;
    ReverbSettings* settings;
    SynthesisReverb* reverb;
    AudioSpec* spec = &D_81255B34[D_81287AF8];

    D_81288B2C = 0;
    sGbAudioOutputConfig.samplingFrequency = spec->frequency;
    sGbAudioOutputConfig.aiSamplingFrequency = osAiSetFrequency(sGbAudioOutputConfig.samplingFrequency);
    sGbAudioOutputConfig.samplesPerFrameTarget = ((sGbAudioOutputConfig.samplingFrequency / D_81288E54) + 0xF) & 0xFFF0;
    sGbAudioOutputConfig.minAiBufferLength = sGbAudioOutputConfig.samplesPerFrameTarget - 0x10;
    sGbAudioOutputConfig.maxAiBufferLength = sGbAudioOutputConfig.samplesPerFrameTarget + 0x10;
    sGbAudioOutputConfig.ticksPerUpdate = ((sGbAudioOutputConfig.samplesPerFrameTarget + 0x10) / 208) + 1;
    sGbAudioOutputConfig.samplesPerTick = (sGbAudioOutputConfig.samplesPerFrameTarget / sGbAudioOutputConfig.ticksPerUpdate) & ~7;
    sGbAudioOutputConfig.samplesPerTickMax = sGbAudioOutputConfig.samplesPerTick + 8;
    sGbAudioOutputConfig.samplesPerTickMin = sGbAudioOutputConfig.samplesPerTick - 8;
    sGbAudioOutputConfig.resampleRate = 32000.0f / (s32)sGbAudioOutputConfig.samplingFrequency;
    sGbAudioOutputConfig.ticksPerUpdateInvScaled = (1.0f / 256.0f) / sGbAudioOutputConfig.ticksPerUpdate;
    sGbAudioOutputConfig.ticksPerUpdateScaled = sGbAudioOutputConfig.ticksPerUpdate / 4.0f;
    sGbAudioOutputConfig.ticksPerUpdateInv = 1.0f / sGbAudioOutputConfig.ticksPerUpdate;
    D_81288D88 = spec->numNotes;
    sGbAudioOutputConfig.numSequencePlayers = spec->numSequencePlayers;
    if (sGbAudioOutputConfig.numSequencePlayers >= 2) {
        sGbAudioOutputConfig.numSequencePlayers = 1;
    }
    D_81287718 = spec->unk_0C;
    D_81288D8C = (u32)(sGbAudioOutputConfig.ticksPerUpdate * 2880000.0f / D_81255B68 / D_81288E50);
    sGbAudioOutputConfig.specUnk4 = spec->unk_04;
    sGbAudioOutputConfig.samplesPerFrameTarget *= sGbAudioOutputConfig.specUnk4;
    sGbAudioOutputConfig.maxAiBufferLength *= sGbAudioOutputConfig.specUnk4;
    sGbAudioOutputConfig.minAiBufferLength *= sGbAudioOutputConfig.specUnk4;
    sGbAudioOutputConfig.ticksPerUpdate *= sGbAudioOutputConfig.specUnk4;
    if (sGbAudioOutputConfig.specUnk4 >= 2) {
        sGbAudioOutputConfig.maxAiBufferLength -= 0x10;
    }
    D_81288D84 = D_81288D88 * 20 * sGbAudioOutputConfig.ticksPerUpdate + spec->numReverbs * 32 + 480;
    persistent_mem = spec->persistentSeqMem + spec->persistentFontMem + spec->persistentSampleMem + spec->persistentSampleCacheSize + 0x10;
    temporary_mem = spec->temporarySeqMem + spec->temporaryFontMem + spec->temporarySampleMem + spec->temporarySampleCacheSize + 0x10;
    total_mem = persistent_mem + temporary_mem;
    want_misc = D_81287738 - total_mem - 0x100;
    D_81287AC0.miscPoolSize = want_misc;
    D_81287AC0.cachePoolSize = total_mem;
    AudioHeap_InitSessionPools((s8*)&D_81287AC0);
    D_81287AD0.persistentCommonPoolSize = persistent_mem;
    D_81287AD0.temporaryCommonPoolSize = temporary_mem;
    AudioHeap_InitCachePools((s8*)&D_81287AD0);
    D_81287AD8.seqCacheSize = spec->persistentSeqMem;
    D_81287AD8.fontCacheSize = spec->persistentFontMem;
    D_81287AD8.sampleBankCacheSize = spec->persistentSampleMem;
    AudioHeap_InitPersistentPoolsAndCaches((s8*)&D_81287AD8);
    D_81287AE8.seqCacheSize = spec->temporarySeqMem;
    D_81287AE8.fontCacheSize = spec->temporaryFontMem;
    D_81287AE8.sampleBankCacheSize = spec->temporarySampleMem;
    AudioHeap_InitTemporaryPoolsAndCaches((s8*)&D_81287AE8);
    AudioHeap_InitSampleCaches(spec->persistentSampleCacheSize, spec->temporarySampleCacheSize);
    if (EXTERNAL_POOL_AFF0.startRamAddr != NULL) {
        EXTERNAL_POOL_AFF0.curRamAddr = EXTERNAL_POOL_AFF0.startRamAddr;
    }
    AudioHeap_ResetLoadStatus();
    D_81287B04 = (Note*)GbApu_AllocZeroed((AudioAllocPool*)&D_81287760, D_81288D88 * 0xCC);
    AudioPlayback_NoteInitAll();
    AudioPlayback_InitNoteFreeList();
    D_81287728 = (void*)GbApu_AllocZeroed((AudioAllocPool*)&D_81287760, sGbAudioOutputConfig.ticksPerUpdate * D_81288D88 * 0x18);
    for (i = 0; i != 2; i++) {
        D_81288DA0[i] = (void*)GbApu_AllocZeroed((AudioAllocPool*)&D_81287760, D_81288D84 * 8);
    }
    for (i = 0; i < 1; i++) {
        reverb = &D_812875B0[i];
        reverb->useReverb = 0;
    }
    D_8128771B = spec->numReverbs;
    for (i = 0; i < D_8128771B; i++) {
        reverb = &D_812875B0[i];
        settings = &spec->reverbSettings[i];

        reverb->downsampleRate = settings->downsampleRate;
        reverb->windowSize = settings->windowSize * 64;
        reverb->windowSize /= reverb->downsampleRate;
        reverb->decayRatio = settings->decayRatio;
        reverb->volume = settings->volume;
        reverb->leakRtl = settings->leakRtl;
        reverb->leakLtr = settings->leakLtr;
        reverb->mixReverbIndex = settings->mixReverbIndex;
        reverb->mixReverbStrength = settings->mixReverbStrength;
        reverb->useReverb = 8;
        reverb->leftRingBuf = (s16*)GbApu_AllocZeroed((AudioAllocPool*)&D_81287760, reverb->windowSize * 2);
        reverb->rightRingBuf = (s16*)GbApu_AllocZeroed((AudioAllocPool*)&D_81287760, reverb->windowSize * 2);
        reverb->nextRingBufPos = 0;
        reverb->unk_18 = 0;
        reverb->curFrame = 0;
        reverb->bufSizePerChan = reverb->windowSize;
        reverb->framesToIgnore = 2;
        reverb->resampleFlags = 1;
        reverb->sound.sample = &reverb->sample;
        reverb->sample.loop = &reverb->loop;
        reverb->sound.tuning = 1.0f;
        reverb->sample.codec = 3;
        reverb->sample.medium = 0;
        reverb->sample.size = reverb->windowSize * 2;
        reverb->sample.sampleAddr = (u8*)reverb->leftRingBuf;
        reverb->loop.start = 0;
        reverb->loop.count = 1;
        reverb->loop.end = reverb->windowSize;
        if (reverb->downsampleRate != 1) {
            reverb->resampleRate = 0x8000 / reverb->downsampleRate;
            reverb->resampleStateLeft = (void*)GbApu_AllocZeroed((AudioAllocPool*)&D_81287760, 0x20);
            reverb->resampleStateRight = (void*)GbApu_AllocZeroed((AudioAllocPool*)&D_81287760, 0x20);
            reverb->unk_30 = (void*)GbApu_AllocZeroed((AudioAllocPool*)&D_81287760, 0x20);
            reverb->unk_34 = (void*)GbApu_AllocZeroed((AudioAllocPool*)&D_81287760, 0x20);
            for (j = 0; j < sGbAudioOutputConfig.ticksPerUpdate; j++) {
                s16* mem = (s16*)GbApu_AllocZeroed((AudioAllocPool*)&D_81287760, 0x340);

                reverb->items[0][j].toDownsampleLeft = mem;
                reverb->items[0][j].toDownsampleRight = mem + 0xD0;
                mem = (s16*)GbApu_AllocZeroed((AudioAllocPool*)&D_81287760, 0x340);
                reverb->items[1][j].toDownsampleLeft = mem;
                reverb->items[1][j].toDownsampleRight = mem + 0xD0;
            }
        }
        if (settings->lowPassFilterCutoffLeft != 0) {
            reverb->filterLeftState = (s16*)GbApu_Alloc((AudioAllocPool*)&D_81287760, 0x20);
            reverb->filterLeft = (s16*)GbApu_Alloc((AudioAllocPool*)&D_81287760, 0x10);
            AudioHeap_LoadLowPassFilter(reverb->filterLeft, settings->lowPassFilterCutoffLeft);
        } else {
            reverb->filterLeft = NULL;
        }
        if (settings->lowPassFilterCutoffRight != 0) {
            reverb->filterRightState = (s16*)GbApu_Alloc((AudioAllocPool*)&D_81287760, 0x20);
            reverb->filterRight = (s16*)GbApu_Alloc((AudioAllocPool*)&D_81287760, 0x10);
            AudioHeap_LoadLowPassFilter(reverb->filterRight, settings->lowPassFilterCutoffRight);
        } else {
            reverb->filterRight = NULL;
        }
    }
    AudioSeq_InitSequencePlayers();
    for (j = 0; j < sGbAudioOutputConfig.numSequencePlayers; j++) {
        AudioSeq_InitSequencePlayerChannels(j);
        AudioSeq_ResetSequencePlayer(j);
    }
    AudioLoad_InitSampleDmaBuffers(D_81288D88);
    D_81288A1C = 0;
    D_8128771C = 0x1000;
    AudioLoad_LoadPermanentSamples();
    osWritebackDCacheAll();
}
