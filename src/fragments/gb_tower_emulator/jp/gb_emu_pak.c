/* Stadium Zero fragment000_80C00. */
/* C ported from the Stadium 1 decomp: src/fragments/gb_tower_emulator/gb_tower_emulator_7F9A0.c. */

#include "PR/ultratypes.h"
// GbPak_VerifyRamBlocks only matches when bcmp takes an unsigned length (the constant is not
// shared with the 0x100 stored in GbPakChannel.size), so this file declares the libc functions
// itself instead of using ultralib's os_libc.h.
#define _OS_LIBC_H_
void bcopy(const void*, void*, int);
int bcmp(const void*, const void*, u32);
void bzero(void*, int);
int sprintf(char* s, const char* fmt, ...);
void osSyncPrintf(const char* fmt, ...);
#include "global.h"
#include "gb_tower_emulator_jp.h"
#include "variables.h"

// Transfer-icon tile indices for GbEmu_DrawAccessIcon, six per direction.
const u8 D_81257660[12] = { 0x7A, 0x3B, 0x8A, 0xCA, 0xB6, 0xBC, 0x5D, 0x3B, 0x8D, 0xCA, 0xB6, 0xBC };
// Glyph indices of the five status-bar characters drawn by GbEmu_DrawStatusBar.
const u8 D_8125766C[5] = { 0xF3, 0x71, 0x65, 0x4C, 0x3B };
// Debug format string with no reference in the JP build.
const char D_81257674[] = "ks_get_memblock: from 0x%x, 0x%x bytes as %s\n";

// This file's .bss (0x81258100-0x81259A60). The GB Player I/O code must see GbPakChannel
// D_81259668 as defined in this file: with only an extern declaration GbPak_StartIoThread
// reloads %hi for each store.
UNUSED u32 D_81258100[2];
OSThread D_81258108;
u8 D_812582B8[0x1000]; // stack of the I/O thread D_81258108; D_812592B8 is its top
u8 D_812592B8[0x100];
u8 D_812593B8[0x100];
OSPfs D_812594B8[4];
OSMesg D_81259658[4];
GbPakChannel D_81259668[4];
OSMesgQueue D_812598F8;
s32 D_81259910;
volatile u8 D_81259914;
volatile u8 D_81259915;
volatile u8 D_81259916;
volatile u8 D_81259917;
s8 D_81259918;
s8 D_81259919;
UNUSED u8 D_8125991A;
UNUSED u8 D_8125991B;
UNUSED u32 D_8125991C;
u8 D_81259920[0x20];
u8 D_81259940[0x20];
u8 D_81259960[0x80];
GbEmuState D_812599E0;
u8* D_81259A38;
u8* D_81259A3C;
u8 D_81259A40;
u8 D_81259A41;
u8 D_81259A42;
u8 D_81259A43;
s32 D_81259A44;
u8 D_81259A48;
u8 D_81259A49;
s32 D_81259A4C;
u32 D_81259A50;
u32 D_81259A54;
s32 D_81259A58;
s32 D_81259A5C;
extern OSThread gGbPakServiceThread;
extern u8 gGbPakServiceThreadStack[0x1000];
extern OSMesg gGbPakServiceQueueMessages[4];
extern OSMesgQueue gGbPakServiceQueue;
extern volatile u8 gGbPakServiceThreadActive;
extern u8 gGbEmuPresentationMode;
extern unk_D_8122C4FA gGbEmuButtonBindingSelections;
extern u8* gGbEmuFont1Data;
extern u8* gGbEmuFont2Data;
void func_812070A0(void);
void GbApu_SelectRegionClock(s32);
void GbApu_QueueRegisterWrite(u16, u8, u16);
void GbApu_ResetChannels(void);
void GbApu_Reset(unk_D_8122B2F8*);
void GbAudio_FrameStub(void);
void GbAudio_SetMenuSoundMode(s32);
#define GB_EMU_PAL_Y_SCALE 0.833f

#include "m2c_macros.h"

typedef struct unk_func_812009D0 {
  /* 0x00 */ u8 unk00[0x34];
  /* 0x34 */ u8 data[0x10];  
  /* 0x44 */ u16 unk_44;
  /* 0x46 */ u8 unk_46;
  /* 0x47 */ u8 unk47;
  /* 0x48 */ u8 unk_48;
  /* 0x49 */ u8 unk_49;
  /* 0x4A */ u8 unk_4A;
} unk_func_812009D0;

typedef struct GbpakBankEntry {
    /* 0x0 */ u8 bank; // bit 7 set: SRAM bank, clear: ROM bank
    /* 0x1 */ u8 unk1[5];
} GbpakBankEntry; // size = 0x6
extern volatile u8 D_81259916;

typedef struct GbEmuTask {
    /* 0x00 */ u32 type;
    /* 0x04 */ u32 flags;
    /* 0x08 */ void* ucode;
    /* 0x0C */ s32 ucodeSize;
    /* 0x10 */ u8 unk_10[4];
    /* 0x14 */ u8 unk_14;
    /* 0x15 */ u8 unk_15;
    /* 0x16 */ u8 unk_16[2];
    /* 0x18 */ u8* unk_18;
    /* 0x1C */ union {
        s32 word;
        u8 bytes[4];
    } unk_1C;
    /* 0x20 */ s32 unk_20;
    /* 0x24 */ u8 unk_24[0xC];
    /* 0x30 */ s32 unk_30;
    /* 0x34 */ u8 unk_34[4];
    /* 0x38 */ s32 unk_38;
} GbEmuTask;


s32 GbEmu_PixelOpDitherA(s32 arg0, s32 arg1);
s32 GbEmu_PixelOpDitherB(s32 arg0, s32 arg1);
s32 GbEmu_PixelOpAverage555(s32 arg0, s32 arg1);
s32 GbEmu_PixelOpReplace(UNUSED s32 arg0, s32 arg1);
void GbEmu_BlitWithPixelOp(u16* dst, u16* src, s32 mode, s32 width, s32 height);
void GbEmu_BlitTinted(u16* dst, u16* src, s32 color, s32 width, s32 height);
void GbEmu_BlendColorWithAlphaMap(u16* dst, u8* alpha_map, s32 color, s32 width, s32 height, u32 alpha_stride);
void GbEmu_Delay(OSTime arg0);
void GbEmu_BlendScrollingLayer(u16* arg0, u16* arg1, s16* arg2, s32 arg3, s32 arg4);

extern u8 D_81259920[];
/**
 * Writes `value` to cartridge address `address` (an MBC1 register) through the Transfer Pak: puts
 * the value in the last byte of the 0x20-byte block D_81259920 and writes the block with
 * osGbpakReadWrite. Returns the osGbpakReadWrite result.
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbPak_InitializeCartridgeMapper, whose MBC1 writes the 2026-11-12
 * Transfer Pak trace showed.
 */
s32 GbPak_WriteFilledRegisterBlock(void *arg0, s32 address, s32 value) {
    M2C_FIELD(D_81259920, s8 *, 0x1F) = value;
    return osGbpakReadWrite(M2C_FIELD(arg0, s32 *, 0x50), 1, address, D_81259920, 0x20);
}

/**
 * Puts the MBC1 into a known state: RAM enable (0x0A), RAM banking mode,
 * bank 0, ROM banking mode, ROM bank 1. Caches pakRomBank/pakRamBank/pakMbcMode.
 * Seen in play (2026-11-12): the Transfer Pak trace shows the MBC1 writes 0x0000=0x0A, 0x6000=1,
 * 0x4000=0, 0x6000=0, 0x4000=0 and 0x2100=1 in that order.
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbPak_IoThreadMain.
 */
s32 GbPak_InitializeCartridgeMapper(unk_D_8122B2C0* arg0) {
  s32 ret;

  if (!(ret = GbPak_WriteFilledRegisterBlock(arg0, 0, 0xA))    && !(ret = GbPak_WriteFilledRegisterBlock(arg0, 0x6000, 1))
   && !(ret = GbPak_WriteFilledRegisterBlock(arg0, 0x4000, 0)) && !(ret = GbPak_WriteFilledRegisterBlock(arg0, 0x6000, 0))
   && !(ret = GbPak_WriteFilledRegisterBlock(arg0, 0x4000, 0)) && !(ret = GbPak_WriteFilledRegisterBlock(arg0, 0x2100, 1))) {
      *((u8*)arg0 + 0x9D) = 1;
      *((u8*)arg0 + 0x9E) = 0;
      *((u8*)arg0 + 0x9F) = 0;
  }
  
  return ret;
}

/*
 * GbPak_VerifyProbeBlock
 *
 * Operates on a record arg0 that looks like the one GbPak_TransferBankedRam
 * takes (it holds a handle at 0x50 and is passed to GbPak_SelectRamBank in both),
 * but that is inferred from those uses only. Sets the byte at 0xA2 to 2 and
 * saves then clears the byte at 0x98. Calls GbPak_SelectRamBank(arg0, word94 >> 13) (the same
 * bank-select helper GbPak_TransferBankedRam uses with address / 0x2000);
 * if that returns 0, calls osGbpakReadWrite with the handle at 0x50, the
 * mode 0 (GbPak_WriteFilledRegisterBlock passes 1 here), the address (word94 & 0x1FFF) | 0xA000,
 * the buffer D_81259940 and a length of 0x20; if that also returns 0, calls
 * bcmp(D_81259940, entry, 0x20) where entry is the 0x20-byte record
 * number byte9C of the table at D_81259960. If the last result is 0 the byte
 * at 0xA2 is reset to 0. The saved byte at 0x98 is restored and the first
 * non-zero (or final) result is returned.
 *
 * What osGbpakReadWrite mode 0 and bcmp do, and what the two tables
 * hold, is not established. Matching notes: the unused first local `pad`
 * places `sp28` at sp+0x28, and the table index is scaled as
 * `(x << 1) << 4` because a single `<< 5` (or `* 0x20`) uses one temporary
 * register fewer and does not match.
 */
s32 GbPak_SelectRamBank(unk_D_8122B2C0* arg0, s32 arg1);
extern u8 D_81259940[];
extern u8 D_81259960[];
/**
 * Reads the 0x20-byte cartridge RAM probe block back at sramAddr and compares it with the copy for
 * this channel in D_81259960. Returns 0 when it matches.
 * Seen in play (2026-11-14) ran in the 「レポートできませんでした」 ("Couldn't save") recovery flow (state forced to 20, cartridge
 * removal faked by stubbing GbPak_IsCartridgeInserted); called from GbPak_IoThreadMain.
 */
s32 GbPak_VerifyProbeBlock(void *ch) {
    s32 pad; // FAKE: stack padding
    s32 sp28;
    s32 var_v0;
    s32 var_v1;

    M2C_FIELD(ch, s8 *, 0xA2) = 2;
    sp28 = (s32) M2C_FIELD(ch, u8 *, 0x98);
    M2C_FIELD(ch, u8 *, 0x98) = 0U;
    var_v0 = GbPak_SelectRamBank(ch, (u32) M2C_FIELD(ch, u32 *, 0x94) >> 0xD);
    var_v1 = var_v0;
    if (var_v0 == 0) {
        var_v0 = osGbpakReadWrite(M2C_FIELD(ch, s32 *, 0x50), 0, ((M2C_FIELD(ch, u32 *, 0x94) & 0x1FFF) | 0xA000) & 0xFFFF, D_81259940, 0x20);
        var_v1 = var_v0;
        if (var_v0 == 0) {
            var_v0 = bcmp(D_81259940, D_81259960 + ((M2C_FIELD(ch, u8 *, 0x9C) << 1) << 4), 0x20);
            var_v1 = var_v0;
        }
    }
    if (var_v0 == 0) {
        M2C_FIELD(ch, s8 *, 0xA2) = 0;
    }
    M2C_FIELD(ch, u8 *, 0x98) = (u8) sp28;
    return var_v1;
}


/**
 * Reads the Transfer Pak status into status (0x9B) and fails with 0x63 unless bits 0 and 3 are
 * both set, i.e. the pak is powered and a cartridge is inserted. Seen in play (2026-11-12): runs
 * before every transfer while Pocket Monsters Green loads.
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbPak_IoThreadMain.
 */
s32 GbPak_CheckCartInserted(s8 *arg0) {
    s32 temp_v0;
    s32 var_v1;

    M2C_FIELD(arg0, s8 *, 0xA2) = 1;
    temp_v0 = osGbpakCheckConnector(M2C_FIELD(arg0, OSPfs**, 0x50), (u8*)arg0 + 0x9B);
    var_v1 = temp_v0;
    if ((temp_v0 == 0) && ((M2C_FIELD(arg0, u8 *, 0x9B) & 9) != 9)) {
        var_v1 = 0x63;
    }
    if (var_v1 != 0) {
        return var_v1;
    }
    M2C_FIELD(arg0, s8 *, 0xA2) = 0;
    return 0;
}

s32 GbPak_InitializeCartridgeMapper(unk_D_8122B2C0* arg0);
s32 GbPak_WriteVerifiedSramBlock(s32 arg0, s32 arg1, s32 arg2);

/**
 * Before each transfer: checks the connector, resets the mapper, re-reads the
 * cartridge header ID and compares it with cartridgeId (detects a swapped cartridge),
 * then re-verifies the RAM probe block if one was written.
 * Seen in play (2026-11-12): runs before each ROM read while Green streams in.
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbPak_ReadRomSummed.
 */
s32 GbPak_ProbeCartridge(u8 *arg0) {
    s32 ret;

    if (((!arg0) && (!arg0)) && (!arg0)) {  // FAKE
    }
    ret = 0;
    if (!(M2C_FIELD(arg0, u8 *, 0x9B) & 1)) {
        ret = GbPak_CheckCartInserted(arg0);
        if (ret == 0) {
            ret = GbPak_InitializeCartridgeMapper(arg0);
            if ((ret == 0) && (M2C_FIELD(arg0, u8 *, 0x9B) & 1) && (M2C_FIELD(arg0, u8 *, 0x98) != 0)) {
                ret = GbPak_VerifyProbeBlock(arg0);
                if (ret == 0) {
                    ret = GbPak_WriteVerifiedSramBlock(M2C_FIELD(arg0, u8 *, 0x9C), M2C_FIELD(arg0, s32 *, 0x94), M2C_FIELD(arg0, u8 *, 0x98));
                }
            }
        }
        if (ret != 0) {
            M2C_FIELD(arg0, u8 *, 0x9B) &= ~1;
        }
    }
    return ret;
}

/**
 * Selects the MBC1 ROM bank for a 0x4000-0x7FFF read of a linear ROM address,
 * switching to ROM banking mode and writing the upper bank bits when needed.
 * Seen in play (2026-11-12): its writes reached the emulated MBC1 as 0x6000=0, 0x4000=bank/32 and
 * 0x2100=bank (Transfer Pak trace).
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbPak_ReadRomSummed.
 */
s32 GbPak_SelectRomAddress(unk_D_8122B2C0* arg0, s32 arg1) {
    s32 temp_a2;
    s32 var_v1;

    var_v1 = 0;
    M2C_FIELD(arg0, u8*, 0x9E) = 0xFF;
    if (M2C_FIELD(arg0, u8*, 0x9F) != 0) {
        M2C_FIELD(arg0, u8*, 0x9D) = 0xFF;
        var_v1 = GbPak_WriteFilledRegisterBlock(arg0, 0x6000, 0);
        if (var_v1 == 0) {
            M2C_FIELD(arg0, u8*, 0x9F) = 0;
        }
    }
    if (var_v1 == 0) {
        if (arg1 != M2C_FIELD(arg0, u8*, 0x9D)) {
            temp_a2 = arg1 / 32;
            if (temp_a2 != ((s32)M2C_FIELD(arg0, u8*, 0x9D) / 32)) {
                var_v1 = GbPak_WriteFilledRegisterBlock(arg0, 0x4000, temp_a2);
            }
            if (var_v1 == 0) {
                var_v1 = GbPak_WriteFilledRegisterBlock(arg0, 0x2100, arg1);
                if (var_v1 == 0) {
                    M2C_FIELD(arg0, u8*, 0x9D) = arg1;
                }
            }
        }
    }
    return var_v1;
}

/**
 * Selects an 8 KB cartridge RAM bank (MBC1 RAM banking mode).
 * Seen in play (2026-11-12): wrote 0x6000=1 and 0x4000=bank on the emulated MBC1 before the SRAM
 * probe.
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbPak_WriteVerifiedSramBlock.
 */
s32 GbPak_SelectRamBank(unk_D_8122B2C0* arg0, s32 arg1) {
    s32 var_v1;

    var_v1 = 0;
    M2C_FIELD(arg0, u8*, 0x9D) = 0xFF;
    if (M2C_FIELD(arg0, u8*, 0x9F) != 1) {
        M2C_FIELD(arg0, u8*, 0x9E) = 0xFF;
        var_v1 = GbPak_WriteFilledRegisterBlock(arg0, 0x6000, 1);
        if (var_v1 == 0) {
            M2C_FIELD(arg0, u8*, 0x9F) = 1;
        }
    }
    if ((var_v1 == 0) && (arg1 != M2C_FIELD(arg0, u8*, 0x9E))) {
        var_v1 = GbPak_WriteFilledRegisterBlock(arg0, 0x4000, arg1);
        if (var_v1 == 0) {
            M2C_FIELD(arg0, u8*, 0x9E) = arg1;
        }
    }
    return var_v1;
}

extern GbPakChannel D_81259668[4];
/**
 * Reads the Transfer Pak status; returns 0 only when bit 7 is set and bit 6 is clear.
 * Seen in play (2026-11-12): polled every fourth frame (request 8) while Green runs; the emulated
 * status 0x89 passes.
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbPak_IoThreadMain.
 */
s32 GbPak_CheckTransferStatus(OSPfs* arg0) {
    s32 ret = osGbpakGetStatus(arg0, &D_81259668[arg0->channel].status);

    if (ret == 0) {
        ret = (D_81259668[arg0->channel].status & 0xC0) ^ 0x80;
    }
    return ret;
}

s32 GbPak_ProbeCartridge(u8* arg0);
s32 GbPak_SelectRomAddress(unk_D_8122B2C0* arg0, s32 arg1);
s32 GbPak_CheckTransferStatus(OSPfs* arg0);
/**
 * Reads size bytes of cartridge ROM at addr into buf through the Transfer Pak, selecting the MBC
 * bank for the 0x4000-0x7FFF window, and adds every byte read to sum. The bank checksum check in
 * GbPak_IoThreadMain (request 6) compares these sums with the known-cartridge tables. Seen in
 * play: two 0x2000-byte reads per bank while Green loads.
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbPak_IoThreadMain.
 */
s32 GbPak_ReadRomSummed(u8* arg0) {
    s32 ret;
    u8* buf;
    s32 addr;
    s32 remaining;
    s32 size;
    s32 sum;
    s32 off;

    ret = GbPak_ProbeCartridge(arg0);
    buf = M2C_FIELD(arg0, u8**, 0x54);
    addr = M2C_FIELD(arg0, s32*, 0x8C);
    remaining = M2C_FIELD(arg0, s32*, 0x80);
    if ((ret == 0) && (addr < 0x4000)) {
        if ((addr + remaining) >= 0x4001) {
            size = 0x4000 - addr;
        } else {
            size = remaining;
        }
        ret = osGbpakReadWrite(M2C_FIELD(arg0, OSPfs**, 0x50), 0, addr, buf, size);
        if (ret == 0) {
            sum = 0;
            for (ret = size; ret != 0; ret--) {
                sum += *buf++;
            }
            M2C_FIELD(arg0, s32*, 0x84) += sum;
            remaining -= size;
            addr = 0x4000;
        }
    }
    while ((ret == 0) && (remaining != 0)) {
        ret = GbPak_SelectRomAddress(arg0, addr / 0x4000);
        if (ret == 0) {
            off = addr & 0x3FFF;
            if ((off + remaining) >= 0x4001) {
                size = 0x4000 - off;
            } else {
                size = remaining;
            }
            ret = osGbpakReadWrite(M2C_FIELD(arg0, OSPfs**, 0x50), 0, (off | 0x4000) & 0xFFFF, buf, size);
            if (ret == 0) {
                sum = 0;
                for (ret = size; ret != 0; ret--) {
                    sum += *buf++;
                }
                M2C_FIELD(arg0, s32*, 0x84) += sum;
                remaining -= size;
                addr = (addr + 0x4000) & ~0x3FFF;
            }
        }
    }
    if (ret == 0) {
        ret = GbPak_CheckTransferStatus(M2C_FIELD(arg0, OSPfs**, 0x50));
    }
    return ret;
}

s32 GbPak_ProbeCartridge(u8* arg0);
s32 GbPak_SelectRamBank(unk_D_8122B2C0* arg0, s32 arg1);
s32 GbPak_CheckTransferStatus(OSPfs* arg0);

/**
 * Reads (is_write == 0) or writes (is_write != 0) transferSize bytes of banked cartridge
 * RAM at gbAddress, selecting each 8 KB bank as it goes.
 * Seen in play (2026-11-12): starting GB from the main menu ran this as a read (is_write 0) of
 * cartridge RAM, just before GbPak_CheckGen1SaveChecksum checked the save; returned 0.
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbPak_IoThreadMain.
 */
s32 GbPak_TransferBankedRam(u8* arg0, s32 arg1) {
    s32 temp_v0;
    s32 var_s0;
    s32 var_s1;
    u8* var_s3;
    s32 var_v0;
    s32 var_v1;
    s32 var_s2;

    var_v0 = GbPak_ProbeCartridge(arg0);
    var_v1 = var_v0;
    var_s3 = M2C_FIELD(arg0, u8**, 0x54);
    var_s2 = M2C_FIELD(arg0, s32*, 0x8C);
    var_s1 = M2C_FIELD(arg0, s32*, 0x80);
    while ((var_v0 == 0) && (var_s1 != 0)) {
        var_v0 = GbPak_SelectRamBank(arg0, var_s2 / 0x2000);
        var_v1 = var_v0;
        if (var_v0 == 0) {
            temp_v0 = var_s2 & 0x1FFF;
            if ((temp_v0 + var_s1) >= 0x2001) {
                var_s0 = 0x2000 - temp_v0;
            } else {
                var_s0 = var_s1;
            }
            var_v0 = osGbpakReadWrite(M2C_FIELD(arg0, OSPfs**, 0x50), arg1, (temp_v0 | 0xA000) & 0xFFFF, var_s3, var_s0);
            var_v1 = var_v0;
            var_s3 += var_s0;
            var_s1 -= var_s0;
            var_s2 = (var_s2 + 0x2000) & ~0x1FFF;
        }
    }
    if (var_v0 == 0) {
        var_v1 = GbPak_CheckTransferStatus(M2C_FIELD(arg0, OSPfs**, 0x50));
    }
    return var_v1;
}

/* One 6-byte entry of the bank list; the list ends with a 0xFF bank byte. */
extern OSMesgQueue D_812598F8;
/**
 * Called once a frame while the cartridge streams in. Queues the next ROM bank (request 6) or
 * SRAM bank (request 7) from the channel's bank list that is not loaded yet. Once all are loaded
 * it checks the running byte sum against the header's global checksum (error 2 on mismatch),
 * powers the pak off (request 4) and sets D_81259916 to 2. Seen in play: streamed all 32 banks of
 * Green, after which bankState was all zero.
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbEmu_UpdateFrame.
 */
void GbPak_QueueNextBankLoad(GbPakChannel* ch) {
    GbpakBankEntry* entry;

    if ((D_81259916 != 0) && (ch->request == 0)) {
        if (ch->error) {
            return;
        }
        entry = (GbpakBankEntry*)ch->bankList;
        if (D_81259916 == 2) {
            D_81259916 = 0;
            return;
        }
        for (; entry->bank != 0xFF; entry++) {
            if (entry->bank & 0x80) {
                volatile u8* state = &ch->sramState[entry->bank & 0x7F];
                if (*state) {
                    *state = 2;
                    ch->addr = (entry->bank & 0x7F) << 13;
                    ch->buf = ch->sramBuf + ch->addr;
                    ch->size = 0x2000;
                    ch->request = 7;
                    ch->unkA1 = 1;
                    goto found;
                }
            } else {
                volatile u8* state = &ch->bankState[entry->bank & 0x7F];
                if (*state) {
                    *state = 2;
                    ch->request = 6;
                    ch->addr = (entry->bank & 0x7F) << 14;
                    ch->buf = ch->romBuf + ch->addr;
                    ch->size = 0x2000;
                    ch->unkA1 = 1;
                    goto found;
                }
            }
        }

        if (((*((u16 *) ch->checksum)) ^ ((ch->sum - ch->checksum[0]) - ch->checksum[1])) & 0xFFFF) {
            ch->error = 2;
            return;
        }
        ch->request = 4;
        osSendMesg(&D_812598F8, ch->channel, 1);
        D_81259916 = 2;
        return;
    found:
        ch->bankEntry = (u8*)entry;
        osSendMesg(&D_812598F8, ch->channel, 1);
    }
}

extern volatile u8 D_81259915;
extern volatile u8 D_81259916;
extern OSMesgQueue D_812598F8;

/**
 * Queues the ROM bank `bank` for loading (request 6, state 3) ahead of the bank list order and
 * waits for the I/O thread. Returns -2 when no load is running or the channel is busy, and -1
 * when the bank is not in the list or the transfer failed. Seen in play: GbEmu_LoadCartridge
 * requests bank 0 first.
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbEmu_LoadCartridge.
 */
s32 GbPak_RequestBankLoad(s32 chan, s32 bank) {
    u8* entry;
    u8* state;

    while (D_81259915 != 0) {
    }
    if ((D_81259916 == 0) || (D_81259668[chan].request != 0) || (D_81259668[chan].error != 0)) {
        return -2;
    }
    for (entry = D_81259668[chan].bankList; *entry != 0xFF; entry += 6) {
        if (bank == *entry) {
            goto found;
        }
    }
    return -1;

found:
    state = &D_81259668[chan].bankState[bank];
    if (*state != 0) {
        *state = 3;
        D_81259668[chan].request = 6;
        D_81259668[chan].addr = (*entry & 0x7F) << 14;
        D_81259668[chan].buf = D_81259668[chan].addr + D_81259668[chan].romBuf;
        D_81259668[chan].size = 0x2000;
        D_81259668[chan].bankEntry = entry;
        if (!entry && !entry && !entry) {} // FAKE: empty test found by the permuter; only changes instruction scheduling
        osSendMesg(&D_812598F8, D_81259668[chan].channel, 1);
        while (D_81259915 != 0) {
        }
    }
    if (D_81259668[chan].error != 0) {
        return -1;
    }
    return 0;
}

extern volatile u8 D_81259915;
extern volatile u8 D_81259916;
extern volatile u8 D_81259917;
extern OSMesgQueue D_812598F8;

/**
 * Once the ROM has loaded, finds the first 256-byte block of cartridge RAM whose blockState is 0
 * (changed by the game) and queues request 5 to write it back to the cartridge. Returns 1 while
 * the I/O thread is busy, 2 when a write was queued, 3 when it must wait, 4 on a pak error and 0
 * when nothing is dirty. Seen in play: runs every frame of GbEmu_UpdateFrame once Green is
 * running.
 * Seen in play (2026-11-14) ran while Green's intro played in the GB Player; called from
 * GbEmu_UpdateFrame.
 */
s32 GbPak_WriteBackDirtySramBlock(s32 arg0) {
    s32 chan = arg0;
    GbPakChannel* ch;
    GbPakChannel* ch2;
    u32 i;

    if (D_81259915 != 0) {
        return 1;
    }
    ch = &D_81259668[chan];
    ch2 = ch;
    if (ch->error != 0) {
        return 4;
    }
    for (i = 0; i < ch2->ramSize >> 8; i++) {
        if (ch->blockState[i] == 0) {
            if (D_81259916 == 0 && D_81259917 != 0) {
                ch->blockState[i] = 1;
                ch->blockDirty[i] = 0;
                ch->request = 5;
                ch->addr = i << 8;
                ch->buf = ch->ramBuf + (i << 8);
                ch->size = 0x100;
                osSendMesg(&D_812598F8, chan, 1);
                return 2;
            }
            return 3;
        }
    }
    if (ch2->error == 0 && (ch2->status & 1)) {
        ch->request = 4;
        osSendMesg(&D_812598F8, chan, 1);
    }
    return 0;
}

/* Fixed-layout split; original translation-unit boundaries remain unconfirmed. */
/* Stadium Zero fragment000_80C00. */
/* C ported from the Stadium 1 decomp: src/fragments/gb_tower_emulator/gb_tower_emulator_7F9A0.c. */

#include "global.h"
#include "gb_tower_emulator_jp.h"
#include "variables.h"
extern OSThread gGbPakServiceThread;
extern u8 gGbPakServiceThreadStack[0x1000];
extern OSMesg gGbPakServiceQueueMessages[4];
extern OSMesgQueue gGbPakServiceQueue;
extern volatile u8 gGbPakServiceThreadActive;
extern u8 gGbEmuPresentationMode;
extern unk_D_8122C4FA gGbEmuButtonBindingSelections;
extern u8* gGbEmuFont1Data;
extern u8* gGbEmuFont2Data;
void func_812070A0(void);
void GbApu_SelectRegionClock(s32);
void GbApu_QueueRegisterWrite(u16, u8, u16);
void GbApu_ResetChannels(void);
void GbApu_Reset(unk_D_8122B2F8*);
void GbAudio_FrameStub(void);
void GbAudio_SetMenuSoundMode(s32);
#define GB_EMU_PAL_Y_SCALE 0.833f

#include "m2c_macros.h"

s32 GbEmu_PixelOpDitherA(s32 arg0, s32 arg1);
s32 GbEmu_PixelOpDitherB(s32 arg0, s32 arg1);
s32 GbEmu_PixelOpAverage555(s32 arg0, s32 arg1);
s32 GbEmu_PixelOpReplace(UNUSED s32 arg0, s32 arg1);
void GbEmu_BlitWithPixelOp(u16* dst, u16* src, s32 mode, s32 width, s32 height);
void GbEmu_BlitTinted(u16* dst, u16* src, s32 color, s32 width, s32 height);
void GbEmu_BlendColorWithAlphaMap(u16* dst, u8* alpha_map, s32 color, s32 width, s32 height, u32 alpha_stride);
void GbEmu_Delay(OSTime arg0);
void GbEmu_BlendScrollingLayer(u16* arg0, u16* arg1, s16* arg2, s32 arg3, s32 arg4);

extern u8 D_81259920[];
s32 GbPak_WriteFilledRegisterBlock(void* arg0, s32 address, s32 value);

s32 GbPak_InitializeCartridgeMapper(unk_D_8122B2C0* arg0);

/*
 * GbPak_VerifyProbeBlock
 *
 * Operates on a record arg0 that looks like the one GbPak_TransferBankedRam
 * takes (it holds a handle at 0x50 and is passed to GbPak_SelectRamBank in both),
 * but that is inferred from those uses only. Sets the byte at 0xA2 to 2 and
 * saves then clears the byte at 0x98. Calls GbPak_SelectRamBank(arg0, word94 >> 13) (the same
 * bank-select helper GbPak_TransferBankedRam uses with address / 0x2000);
 * if that returns 0, calls osGbpakReadWrite with the handle at 0x50, the
 * mode 0 (GbPak_WriteFilledRegisterBlock passes 1 here), the address (word94 & 0x1FFF) | 0xA000,
 * the buffer D_81259940 and a length of 0x20; if that also returns 0, calls
 * bcmp(D_81259940, entry, 0x20) where entry is the 0x20-byte record
 * number byte9C of the table at D_81259960. If the last result is 0 the byte
 * at 0xA2 is reset to 0. The saved byte at 0x98 is restored and the first
 * non-zero (or final) result is returned.
 *
 * What osGbpakReadWrite mode 0 and bcmp do, and what the two tables
 * hold, is not established. Matching notes: the unused first local `pad`
 * places `sp28` at sp+0x28, and the table index is scaled as
 * `(x << 1) << 4` because a single `<< 5` (or `* 0x20`) uses one temporary
 * register fewer and does not match.
 */
s32 GbPak_SelectRamBank(unk_D_8122B2C0* arg0, s32 arg1);
extern u8 D_81259940[];
extern u8 D_81259960[];
s32 GbPak_VerifyProbeBlock(void* ch);


s32 GbPak_CheckCartInserted(s8* arg0);

s32 GbPak_InitializeCartridgeMapper(unk_D_8122B2C0* arg0);
s32 GbPak_WriteVerifiedSramBlock(s32 arg0, s32 arg1, s32 arg2);

/*
 * GbPak_ProbeCartridge
 *
 * Returns 0 right away when bit 0 of the byte at arg0 + 0x9B is set.
 * Otherwise runs GbPak_CheckCartInserted(arg0), then GbPak_InitializeCartridgeMapper(arg0) while the
 * result is 0, then, if bit 0 of the byte at 0x9B is (now) set and the byte
 * at 0x98 is non-zero, GbPak_VerifyProbeBlock(arg0), then
 * GbPak_WriteVerifiedSramBlock(byte 0x9C, word 0x94, byte 0x98), stopping at the first
 * non-zero result. If the result is non-zero, clears bit 0 of the byte at
 * 0x9B. Returns the result.
 *
 * Same record as GbPak_TransferBankedRam and GbPak_VerifyProbeBlock take; what the
 * flag bit and the other fields mean is not established.
 *
 * Matching note: the empty `if` on `arg0` has no meaning of its own. It was
 * found with decomp-permuter and is the only form tried that makes the
 * compiler copy arg0 to s0 before the first load, as the ROM does; the
 * triple test is needed (one or two do not match). The same kind of empty
 * test already exists in Registration_WriteTeam.
 */
s32 GbPak_ProbeCartridge(u8* arg0);

s32 GbPak_SelectRomAddress(unk_D_8122B2C0* arg0, s32 arg1);

s32 GbPak_SelectRamBank(unk_D_8122B2C0* arg0, s32 arg1);

extern GbPakChannel D_81259668[4];
s32 GbPak_CheckTransferStatus(OSPfs* arg0);

s32 GbPak_ProbeCartridge(u8* arg0);
s32 GbPak_SelectRomAddress(unk_D_8122B2C0* arg0, s32 arg1);
s32 GbPak_CheckTransferStatus(OSPfs* arg0);
s32 GbPak_ReadRomSummed(u8* arg0);

s32 GbPak_ProbeCartridge(u8* arg0);
s32 GbPak_SelectRamBank(unk_D_8122B2C0* arg0, s32 arg1);
s32 GbPak_CheckTransferStatus(OSPfs* arg0);

s32 GbPak_TransferBankedRam(u8* arg0, s32 arg1);



extern volatile u8 D_81259915;
extern volatile u8 D_81259916;
extern OSMesgQueue D_812598F8;

s32 GbPak_RequestBankLoad(s32 chan, s32 bank);

extern volatile u8 D_81259915;
extern volatile u8 D_81259916;
extern volatile u8 D_81259917;
extern OSMesgQueue D_812598F8;

s32 GbPak_WriteBackDirtySramBlock(s32 arg0);

/**
 * Reads each clean 0x100-byte block of cartridge RAM back and compares it with the cached copy; a
 * mismatch rewrites the block and restarts the pass, up to 20 times. Returns -1 on failure.
 * Seen in play (2026-11-14) ran in the 「レポートできませんでした」 ("Couldn't save") recovery flow (state forced to 20, cartridge
 * removal faked by stubbing GbPak_IsCartridgeInserted); called from GbEmu_RunFrame.
 */
s32 GbPak_VerifyRamBlocks(s32 arg0) {
    GbPakChannel* ch2;
    GbPakChannel* ch;
    s32 retries;
    s32 r;
    u32 i;
    s32 off;

    ch = &D_81259668[arg0];
    ch2 = ch;
    retries = 20;
    for (;;) {
        r = GbPak_WriteBackDirtySramBlock(arg0);
        if (r >= 3) {
        fail:
            for (i = 0; i < ch->ramSize >> 8; i++) {
                ch->blockState[i] = ch->blockDirty[i];
            }
            return -1;
        }
        if (r == 0) {
            break;
        }
    }
    while (D_81259915 != 0) {
    }
    if (ch->error != 0) {
        goto fail;
    }
restart:
    for (i = 0; i < ch2->ramSize >> 8; i++) {
        off = i << 8;
        if (ch->blockDirty[i] == 0) {
            ch->request = 2;
            ch->addr = i << 8;
            ch->size = 0x100;
            ch->buf = ch->verifyBuf;
            osSendMesg(&D_812598F8, arg0, 1);
            while (D_81259915 != 0) {
            }
            if (ch->error != 0) {
                goto fail;
            }
            if (bcmp(ch->verifyBuf, ch->ramBuf + (i << 8), 0x100) != 0) {
                if (retries == 0) {
                    goto fail;
                }
                if ((GbPak_CheckCartInserted((s8*)ch) != 0) || ((ch->unk98 != 0) && (GbPak_VerifyProbeBlock(ch) != 0))) {
                    goto fail;
                }
                retries--;
                ch->request = 3;
                ch->addr = off;
                ch->buf = ch->ramBuf + off;
                ch->size = 0x100;
                osSendMesg(&D_812598F8, arg0, 1);
                while (D_81259915 != 0) {
                }
                if (ch->error) {
                    goto fail;
                }
                goto restart;
            }
        }
    }
    return 0;
}

void osDpGetCounters(u32*);
u32 osGetCount(void);

/**
 * Fills the 32-byte probe block (probePattern and unk_5D90) for the writable-RAM
 * probe: mode 3 all zeros, mode 2 all ones, mode 1 DP counters and osGetCount values
 * so the pattern differs each run.
 * Seen in play (2026-11-13) ran on the GB Player loading screen (ロードちゅう, "Loading").
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbPak_WriteVerifiedSramBlock.
 */
void GbPak_PrepareProbeBlock(u32* arg0, s32 arg1) {
    s32 i;

    switch (arg1) {
        case 3:
            for (i = 0; i < 8; i++) {
                arg0[i] = 0;
            }
            break;
        case 2:
            for (i = 0; i < 8; i++) {
                arg0[i] = -1;
            }
            break;
        case 1:
            osDpGetCounters(arg0);
            arg0[3] = osGetCount();
            osWritebackDCacheAll();
            osDpGetCounters(arg0 + 4);
            arg0[7] = osGetCount();
            break;
    }
}

void GbPak_PrepareProbeBlock(u32* arg0, s32 arg1);

/**
 * Selects the SRAM bank for the address, then writes a 0x20-byte block at 0xA000 + offset and
 * reads it back until three writes in a row compare equal.
 * Seen in play (2026-11-12): wrote and read back 0x55, 0xAA and 0xAC blocks at GB 0xA000 when the
 * GB item started.
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbEmu_LoadCartridge.
 */
s32 GbPak_WriteVerifiedSramBlock(s32 arg0, s32 arg1, s32 arg2) {
    GbPakChannel* ch = &D_81259668[arg0];
    s32 ret;
    s32 n;
    u8* entry;
    u16 addr;

    ch->unk98 = 0;
    if (arg2 == 0) {
        return 0;
    }
    ch->unkA2 = 3;
    ret = GbPak_SelectRamBank((u8*)ch, arg1 / 0x2000);
    n = 3;
    if (ret == 0) {
        entry = (u8*)D_81259960 + ((arg0 << 1) << 4);
        addr = (arg1 & 0x1FFF) | 0xA000;
        do {
            GbPak_PrepareProbeBlock(entry, n);
            ret = osGbpakReadWrite((s32)ch->pfs, 1, addr, (M2C_UNK*)entry, 0x20);
            if (ret == 0 && (ret = osGbpakReadWrite((s32)ch->pfs, 0, addr, D_81259940, 0x20)) == 0 &&
                (ret = bcmp(D_81259940, entry, 0x20)) == 0 && --n == 0) {
                ch->unk98 = arg2;
                ch->sramAddr = arg1;
                ch->unkA2 = 0;
                break;
            }
        } while (ret == 0);
    }
    return ret;
}

extern s32 D_81259910;
extern OSPfs D_812594B8[];
extern u8 D_812592B8[];
extern u8 D_812593B8[];
extern volatile u8 D_81259914;
s32 GbPak_TransferBankedRam(u8* arg0, s32 arg1);
s32 GbPak_CheckGen1SaveChecksum(s8* arg0);

/**
 * Game Boy Pak I/O thread: takes a channel number from the queue and serves that channel's
 * request (init and read the cartridge ID, read or write cartridge RAM). 0xFF ends the thread.
 * Seen in play (2026-11-12): serviced requests 6 (bank load with checksum check), 8 (status) and
 * 4 (power off) while Green loaded. A bank whose 0x2000-byte sums differ from the table sets
 * error 1.
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak.
 */
void GbPak_IoThreadMain(s32 arg0) {
    s32 pad0; // FAKE
    s32 chan;
    OSPfs* pfs;
    s32 pad1; // FAKE
    u8 id[0x50];
    u8* idp;
    GbPakChannel* ch;

    for (;;) {
        if (D_812598F8.validCount == 0) {
            D_81259915 = 0;
        }
        osRecvMesg(&D_812598F8, (OSMesg*)&chan, OS_MESG_BLOCK);
        if (chan == 0xFF) {
            D_81259914 = 0;
            osDestroyThread(NULL);
        }
        ch = &D_81259668[chan];
        pfs = &D_812594B8[chan];
        D_81259915 = 1;
        idp = NULL;
        osGetCount();
        if (ch->request & 0x80) {
            bzero(id, 0x50);
            if (osGbpakInit(*(OSMesgQueue**)&D_81259910, pfs, chan) != 0) {
                goto error;
            }
            if (ch->request == 0x80) {
                idp = (u8*)ch;
                goto retry;
            }
            idp = id;
        retry:
            if (osGbpakReadId(pfs, (OSGbpakId*)idp, &ch->status) != 0) {
                goto error;
            }
            if (GbPak_CheckCartInserted((s8*)ch) != 0) {
                goto error;
            }
            if (GbPak_InitializeCartridgeMapper((s8*)ch) != 0) {
                goto error;
            }
            if (GbPak_CheckTransferStatus(pfs) != 0) {
                goto error;
            }
            if (!(ch->status & 1)) {
                goto error;
            }
            if (ch->unk98 != 0 && GbPak_VerifyProbeBlock(ch) != 0) {
                goto error;
            }
            if (idp != (u8*)ch && bcmp(idp, ch, 0x50) != 0) {
                goto error;
            }
            ch->unk9D[3] = 0;
            ch->sum = 0;
            ch->unkA3[0] = 0;
            goto next;
        error:
            if (ch->unkA1 != 0 && --ch->unkA1 != 0 && idp != NULL) {
                idp = id;
                goto retry;
            }
            ch->error = 1;
            ch->request = 0;
            continue;
        }
    next:
        idp = (u8*)ch;
        switch (ch->request & 0x7F) {
            case 1:
                if (GbPak_ReadRomSummed((u8*)ch) != 0) {
                    goto error;
                }
                break;
            case 2:
                if (GbPak_TransferBankedRam(ch, 0) != 0) {
                    goto error;
                }
                break;
            case 3:
                if (GbPak_TransferBankedRam(ch, 1) != 0) {
                    goto error;
                }
                break;
            case 5:
                if (GbPak_ProbeCartridge((u8*)ch) != 0) {
                    goto error;
                }
                if (GbPak_SelectRamBank((u8*)ch, (u32)ch->addr >> 13) != 0) {
                    goto error;
                }
                _bcopy(ch->buf, D_812592B8, 0x100);
                if (osGbpakReadWrite(pfs, OS_WRITE, (ch->addr & 0x1FFF) | 0xA000, D_812592B8, ch->size) == 0 &&
                    osGbpakReadWrite(pfs, OS_READ, (ch->addr & 0x1FFF) | 0xA000, D_812593B8, ch->size) == 0 &&
                    bcmp(D_812592B8, D_812593B8, 0x100) == 0) {
                    break;
                }
                ch->blockState[(u32)ch->addr >> 8] = 0;
                goto error;
            case 6:
                *(s32*)ch->unk88 = ch->sum;
                if (GbPak_ReadRomSummed((u8*)ch) != 0) {
                    goto error;
                }
                if ((((u16*)ch->bankEntry)[1] ^ (ch->sum - *(s32*)ch->unk88)) & 0xFFFF) {
                    goto error;
                }
                *(s32*)ch->unk88 = ch->sum;
                ch->buf += ch->size;
                ch->addr += ch->size;
                if (GbPak_ReadRomSummed((u8*)ch) != 0) {
                    goto error;
                }
                if ((((u16*)ch->bankEntry)[2] ^ (ch->sum - *(s32*)ch->unk88)) & 0xFFFF) {
                    goto error;
                }
                ch->bankState[ch->bankEntry[0] & 0x7F] = 0;
                break;
            case 7:
            loop7:
                if (GbPak_TransferBankedRam(ch, 0) != 0) {
                    goto error;
                }
                if (GbPak_CheckGen1SaveChecksum((s8*)ch) != 0 && ch->unkA3[0] < 3) {
                    ch->unkA3[0]++;
                    goto loop7;
                }
                ch->sramState[ch->bankEntry[0] & 0x7F] = 0;
                break;
            case 8:
                if (GbPak_CheckTransferStatus(pfs) != 0) {
                    goto error;
                }
                break;
            case 4:
                ch->status &= ~1;
                if (osGbpakPower(ch->pfs, 0) != 0) {
                    goto error;
                }
                break;
        }
        ch->request = 0;
        ch->unkA1 = 0x18;
    }
}

extern volatile u8 D_81259914;
extern volatile u8 D_81259915;
extern volatile u8 D_81259916;
extern volatile u8 D_81259917;
extern s8 D_81259918;
extern s8 D_81259919;
extern s32 D_81259910;
extern OSMesgQueue D_812598F8;
extern OSPfs D_812594B8[4];
extern OSMesg D_81259658[4];
extern OSThread D_81258108;
extern u8 D_812592B8[];
void osCreateMesgQueue(OSMesgQueue*, OSMesg*, s32);
void osStartThread(OSThread*);
void GbPak_IoThreadMain(s32 arg0);
/**
 * Clears the four Transfer Pak channel records, gives each its OSPfs and starts the Game Boy Pak
 * I/O thread (GbPak_IoThreadMain) on the given message queue.
 * Seen in play (2026-11-12): runs when the GB item is entered, before the cartridge loads.
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbEmu_Init.
 */
void GbPak_StartIoThread(s32 arg0, s32 arg1, s32 arg2) {
    s32 i;

    D_81259910 = arg2;
    D_81259914 = 1;
    D_81259915 = 0;
    D_81259916 = 0;
    D_81259917 = 0;
    D_81259918 = 0;
    D_81259919 = 0;
    bzero((s32) D_81259668, 0x290);
    bzero((s32) D_81259920, 0x20);
    for (i = 0; i < 4; i++) {
        D_81259668[i].channel = i;
        D_81259668[i].unkA1 = 0x18;
        D_81259668[i].pfs = &D_812594B8[i];
    }
    osCreateMesgQueue(&D_812598F8, D_81259658, 4);
    osCreateThread(&D_81258108, arg0, GbPak_IoThreadMain, 0, D_812592B8, arg1);
    osStartThread(&D_81258108);
}

extern volatile u8 D_81259914;
extern volatile u8 D_81259915;
extern volatile u8 D_81259916;
extern volatile u8 D_81259917;
extern s8 D_81259918;
extern s8 D_81259919;
extern s32 D_81259910;
extern OSMesgQueue D_812598F8;
/**
 * Waits for the I/O thread to go idle, sends it the 0xFF quit message and waits until it has
 * exited.
 * Seen in play (2026-11-13) with a JP Green cart in the Transfer Pak, it ran once when the GB
 * session ended (Continue was picked on a save the game refused, and Stadium showed its "report is
 * not correct" panel); it did not run while the game played.
 * Seen in play (2026-11-14) ran when はい ("Yes") on the ゲームをやめる ("Quit game") prompt left the GB Player; called from
 * GbEmu_RunFrame.
 */
void GbPak_StopIoThread(s32 arg0) {
    s32 i;

    while (D_81259915 != 0) {
    }
    for (i = 0; i < 4; i++) {
        if ((1 << i) & arg0) {
            osGbpakPower(*(s32 *)((u8 *)&D_81259668[i] + 0x50), 0);
        }
    }
    while (D_81259915 != 0) {
    }
    osSendMesg(&D_812598F8, 0xFF, 1);
    while (D_81259914 != 0) {
    }
    D_81259910 = 0;
    D_81259915 = 0;
    D_81259916 = 0;
    D_81259917 = 0;
    D_81259918 = 0;
    D_81259919 = 0;
}

extern u8* D_81259A38;
extern u8* D_81259A3C;
extern u16 D_8120E300[];
extern u16 D_8120E320[];
extern u16 D_8120E340[];
extern u16 D_8120E360[];
extern u16 D_8120E380[];
extern u16 D_8120E3A0[];
extern u16 D_8120E3C0[];
extern u16 D_8120E3E0[];
extern u16 D_8120E400[];
extern u16 D_8120E420[];
extern u16 D_8120E440[];
extern u16 D_8120E460[];
/**
 * Draws a list of text commands into a 320-wide RGBA5551 image. Each command is
 * 0x80|flags, x, y, font height (0x0A = font1, 0x0C = font2), palette index, length,
 * then ASCII characters mapped through sFont1GlyphTable/sFont2GlyphTable.
 * A zero byte ends the list.
 * Seen in play (2026-11-12): opening the GB Player menu (C-Up) passed the list 80 38 07 0C 0B 04 +
 * 4 glyphs (メニュー, "Menu"), 80 20 1B 0C 0A 08 + 8 glyphs (ゲームをつづける, "Continue game"), C0 03 59 0C 09 0A + 10 glyphs
 * (このメニューをぬけて, "leave this menu and"), matching flags, x, y, font height, palette, length and characters; the same glyph
 * bytes repeat for the same kana (71 65 4C 3B = メニュー).
 * Seen in play (2026-11-14) ran when C-Up opened the GB Player menu; called from GbEmu_DrawPauseMenu.
 */
void GbEmu_DrawTextCommands(u16* fb, u8* cmd) {
    u8* ptr;
    u8* data;
    u8* src;
    u16* dst;
    u16* pal;
    u8 flags;
    u8 y;
    s32 width;
    s32 x;
    s32 dir;
    s32 trans;
    s32 tile_size;
    s32 row;
    s32 col;
    s32 n;

    flags = *cmd;
    ptr = cmd;
    while (flags) {
        trans = flags;
        x = ptr[1] + ((flags & 1) << 8);
        y = ptr[2];
        if (flags & 0x40) {
            dir = -1;
        } else {
            dir = 0;
        }
        dst = fb + ((y * 0x140) + x);
        trans &= 0x20;
        switch (ptr[3]) {
            case 0xC:
                width = 0xC;
                tile_size = 0xC;
                data = D_81259A3C;
                break;
            case 0xA:
            default:
                data = D_81259A38;
                width = 0xA;
                tile_size = 0xA;
                break;
        }

        switch (ptr[4]) {
            case 1:
                pal = D_8120E320;
                break;
            case 2:
                pal = D_8120E340;
                break;
            case 3:
                pal = D_8120E360;
                break;
            case 4:
                pal = D_8120E380;
                break;
            case 5:
                pal = D_8120E3A0;
                break;
            case 6:
                pal = D_8120E3C0;
                break;
            case 7:
                pal = D_8120E3E0;
                break;
            case 8:
                pal = D_8120E400;
                break;
            case 9:
                pal = D_8120E420;
                break;
            case 10:
                pal = D_8120E440;
                break;
            case 11:
                pal = D_8120E460;
                break;
            case 0: // FAKE
            default:
                pal = D_8120E300;
                break;
        }

        n = ptr[5];
        while (n != 0) {
            n--;
            src = data + ((ptr[6] * tile_size) * 0x10);
            for (row = 0; row != tile_size; row++) {
                for (col = 0; col != width; col++) {
                    if (trans || (pal[*src & 0xF] != 0)) {
                        *dst = pal[*src & 0xF];
                    }
                    dst++;
                    src++;
                }
                dst = (dst - width) + 0x140;
                src = (src - width) + 0x10;
            }
            ptr++;
            dst -= ((tile_size * 0x140) - width) - dir;
        }

        flags = ptr[6];
        ptr += 6;
    }
}

/**
 * Pixel op: keeps the destination when the source alpha bit is clear, else the source.
 * Seen in play (2026-11-13) ran on the GB Player screen while Pocket Monsters Green booted; also
 * when the GB Player C-Up menu opened.
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbEmu_BlitWithPixelOp.
 */
s32 GbEmu_PixelOpDitherA(s32 arg0, s32 arg1) {
  return (arg1 & 1) ? arg1 : arg0;
}

/**
 * Pixel op: alternate keyed source/destination selection.
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name matches Stadium 1. Comment from Stadium 1.
 */
s32 GbEmu_PixelOpDitherB(s32 arg0, s32 arg1) {
  return (arg1 & 1) ? arg0 : arg1;
}

/**
 * Pixel op: averages source and destination per RGBA5551 channel.
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name matches Stadium 1. Comment from Stadium 1.
 */
s32 GbEmu_PixelOpAverage555(s32 arg0, s32 arg1) {
  return (((s32) ((arg0 & 0xF800) + (arg1 & 0xF800)) >> 1) & 0xF800) | (((s32) ((arg0 & 0x7C0) + (arg1 & 0x7C0)) >> 1) & 0x7C0) | (((s32) ((arg0 & 0x3E) + (arg1 & 0x3E)) >> 1) & 0x3E);
}

/**
 * Pixel op: replaces the destination with the source.
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name matches Stadium 1. Comment from Stadium 1.
 */
s32 GbEmu_PixelOpReplace(UNUSED s32 arg0, s32 arg1) {
  return arg1;
}

/**
 * Copies a width x height RGBA5551 image into a 320-wide destination, combining
 * each pixel with the selected pixel op.
 * Seen in play (2026-11-13) ran on the GB Player screen while Pocket Monsters Green booted; also
 * when the GB Player C-Up menu opened.
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbEmu_DrawAccessOverlay.
 */
void GbEmu_BlitWithPixelOp(u16* dst, u16* src, s32 mode, s32 width, s32 height) {
  s32 (*operation)(s32, s32);
  s32 x;
  s32 y;
  width = ((width + 3) & 0xFFC);
  switch (mode) {
      case 0:  operation = GbEmu_PixelOpDitherA; break;
      case 1:  operation = GbEmu_PixelOpDitherB; break;
      case 2:  operation = GbEmu_PixelOpAverage555; break;
      default: operation = GbEmu_PixelOpReplace; break;
  }

  for (x = 0; x != height; x++) {
      for (y = 0; y != width; y++) {
          *dst = operation(*dst, *src);
          dst++;
          src++;
      }
      dst = dst - width + 0x140;
  }
}

extern s32 D_81259A2C;

/**
 * Clears the 0x2800-byte status bar image below the GB screen and writes back the data cache.
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbEmu_LoadCartridge.
 */
void GbEmu_ClearStatusBar(void) {
    bzero(D_81259A2C, 0x2800);
    osWritebackDCacheAll();
}

/**
 * Copies an image using its blue channel as intensity to scale the given color.
 * Seen in play (2026-11-13) ran on the GB Player screen while Pocket Monsters Green booted; also
 * when the GB Player C-Up menu closed (B); also on the GB Player loading screen (ロードちゅう, "Loading").
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbEmu_DrawTintedBar.
 */
void GbEmu_BlitTinted(u16* dst, u16* src, s32 color, s32 width, s32 height) {
    s32 x;
    s32 y;
    s32 intensity;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            intensity = (src[y * width + x] & 0x3E) >> 1;
            dst[y * 320 + x] = ((((color & 0xF800) * intensity) / 31) & 0xF800) |
                               ((((color & 0x7C0) * intensity) / 31) & 0x7C0) |
                               ((((color & 0x3E) * intensity) / 31) & 0x3E);
        }
    }
}

/**
 * Additively blends a solid color into the destination using a 4-bit alpha map.
 * Seen in play (2026-11-13) ran on the GB Player screen while Pocket Monsters Green booted; also
 * when the GB Player C-Up menu closed (B); also on the GB Player loading screen (ロードちゅう, "Loading").
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbEmu_DrawAccessOverlay.
 */
void GbEmu_BlendColorWithAlphaMap(u16* dst, u8* alpha_map, s32 color, s32 width, s32 height, u32 alpha_stride) {
  u16* dst_row;
  u8* alpha_row;
  s32 x;
  s32 y;
  s32 var_t0;
  s32 var_t4;
  s32 var_a0;
  for (y = 0; y < height; y++) {
      for (x = 0; x < width; x++) {
          var_a0 = alpha_map[y * alpha_stride + x] & 0xF;
          var_t0 = (dst[y * 320 + x] & 0xF800) + (((color & 0xF800) * var_a0) / 15 & 0xF800);
          if (var_t0 > 0xF800) var_t0 = 0xF800;
          var_t4 = var_t0;
          var_t0 = (dst[y * 320 + x] & 0x07C0) + (((color & 0x07C0) * var_a0) / 15 & 0x07C0);
          if (var_t0 > 0x07C0) var_t0 = 0x07C0;
          var_t4 |= var_t0;
          var_t0 = (dst[y * 320 + x] & 0x003E) + (((color & 0x003E) * var_a0) / 15 & 0x003E);
          if (var_t0 > 0x003E) var_t0 = 0x003E;
          dst[y * 320 + x] = (s16)(var_t4 | var_t0);
      }
  }
}

extern u16 D_81220BF8[];
extern u16 D_81220CF8[];
extern u16 D_81220DF8[];

/**
 * Draws a horizontal bar into a 16-bit framebuffer from tinted 8x16 tiles: left cap, repeated
 * middle, right cap.
 * Seen in play (2026-11-13) ran on the GB Player screen while Pocket Monsters Green booted; also
 * when the GB Player C-Up menu closed (B); also on the GB Player loading screen (ロードちゅう, "Loading").
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbEmu_DrawAccessOverlay.
 */
void GbEmu_DrawTintedBar(u8* arg0, s32 arg1, s32 arg2) {
    s32 i;

    for (i = 8; i < arg2 - 8; i += 8) {
        GbEmu_BlitTinted((u16*)arg0 + i, D_81220CF8, arg1, 8, 0x10);
    }
    GbEmu_BlitTinted((u16*)arg0, D_81220BF8, arg1, 8, 0x10);
    GbEmu_BlitTinted((u16*)(arg0 + arg2 * 2 - 0x10), D_81220DF8, arg1, 8, 0x10);
}

extern u16 D_812215D8[];
void osWritebackDCache(void*, s32);
/**
 * Draws one 12x11 frame of the animated Transfer Pak access icon; direction selects
 * the mirrored (zero) or normal direction, frame selects the frame (0-10).
 * Seen in play (2026-11-12): draws the animated pak icon on the 「ロードちゅう」 ("Loading") banner below the GB
 * screen while Green loads.
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbEmu_DrawAccessOverlay.
 */
void GbEmu_DrawAccessIcon(u16* arg0, s32 arg1, s32 arg2) {
    u16* src;
    u32 row;
    s32 y;
    s32 x;
    u16 pixel;

    if (arg1 != 0) {
        src = &D_812215D8[arg2 * 0x84];
        arg1 = 1;
    } else {
        src = &D_812215D8[arg2 * 0x84 + 0x83];
        arg1 = -1;
    }
    for (y = 0; y < 11; y++) {
        for (x = 0; x < 12; x++) {
            pixel = *src;
            if (pixel & 1) {
                row = y;
                arg0[row * 0x140 + x + 0x2D2] = pixel;
            }
            src += arg1;
        }
    }
    osWritebackDCache(arg0, 0x1E00);
}

extern s32 D_812599F0;
extern u8 D_8121E908;
extern u8 D_811221C0[];
extern u8 D_81121B30[];
extern u8 D_811220D0[];
extern u8 D_81122010[];
extern u8 D_81122070[];
extern u8 D_81121DA0[];
void GbEmu_BlitWithPixelOp(u16* dst, u16* src, s32 mode, s32 width, s32 height);

/**
 * Draws the speed-up indicator in the status bar: the speed lamps 1, 2 (and 3 when two levels are
 * unlocked), lit up to the current speed in GB memory 0x101B9 (1, 2 or 4).
 * Seen in play (2026-11-14) ran when C-Right changed the speed (speed-up unlocked by poking
 * D_8121E908 to 2); called from GbEmu_DrawStatusBar.
 */
void GbEmu_DrawSpeedIndicator(s32 fb);
#ifdef NON_MATCHING
void GbEmu_DrawSpeedIndicator(s32 fb) {
    s32 sp2C;
    s32 mode;

    sp2C = ((u8*)D_812599F0)[0x101B9];
    mode = D_8121E908;

    if (mode == 1) {
        fb += 0xA8;
        GbEmu_BlitWithPixelOp(fb + 0x16BE, D_811221C0, 0, 0x10, 5);
        GbEmu_BlitWithPixelOp(fb + 0x506, D_81121B30, 0, 0x18, 0xD);
    } else if (mode == 2) {
        fb += 0x94;
        GbEmu_BlitWithPixelOp(fb + 0x16BE, D_811220D0, 0, 0x18, 5);
        GbEmu_BlitWithPixelOp(fb + 0x55A, (sp2C >= 3) ? D_81122010 : D_81122070, 0, 8, 6);
        GbEmu_BlitWithPixelOp(fb + 0x506, D_81121DA0, 0, 0x18, 0xD);
    }
    if (D_8121E908 != 0) {
        GbEmu_BlitWithPixelOp(fb + 0x53A, D_81122010, 0, 8, 6);
        GbEmu_BlitWithPixelOp(fb + 0x54A, (sp2C >= 2) ? D_81122010 : D_81122070, 0, 8, 6);
    }
}
#else
// clang-format off
#pragma GLOBAL_ASM("asm/jp/nonmatchings/fragments/gb_tower_emulator/jp/gb_emu_pak/GbEmu_DrawSpeedIndicator.s")
// clang-format on
#endif

extern s32 D_81259A30;
extern u16 D_81222A5C[];
extern s32 sGbEmu;
extern s32 D_81259A34;
extern u8* D_81259A38;
extern u8 D_81259A42;
extern u8 D_81221018[];
extern u16 D_81221158[];
/**
 * Draws the ロードちゅう ("Loading") Transfer Pak access banner into the status bar, with the access icon facing
 * `direction`.
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbEmu_LoadCartridge.
 */
void GbEmu_DrawAccessOverlay(s32 direction) {
    s32 pos;
    s32 dst;
    s32 i;
    u8* p;

    pos = D_81259A30;
    bzero(pos, 0x2800);
    pos += 0xAE;
    GbEmu_DrawTintedBar((u8*)pos, D_81222A5C[D_81259A34], 0x70);
    i = 0;
    dst = pos + 0x788;
    if (direction != 0) {
        direction = 6;
    }
    p = &D_81257660[direction];
    do {
        GbEmu_BlendColorWithAlphaMap((u16*)dst, D_81259A38 + *p * 0xA0, 0xFFFE, 12, 10, 16); // #FFFFFF
        i += 0x12;
        dst += 0x12;
        p++;
    } while (i != 0x6C);
    GbEmu_BlitWithPixelOp(pos + 0x7FE, D_81221018, 0, 0x10, 0xA);
    GbEmu_BlitWithPixelOp(pos + 0x5C2, (u8*)&D_81221158[D_81259A34 * 12 * 12], 0, 0xC, 0xC);
    GbEmu_DrawAccessIcon((u16*)pos, direction, 0);
    D_81259A42 = 0;
    sGbEmu = D_81259A30;
}


extern GbEmuState D_812599E0;
extern u16 D_81222A5C[];
extern u8 D_81220EF8[];
extern u8* D_81259A3C;
extern s32 sGbEmu;

/**
 * Draws the status bar below the GB screen: the tinted bar, the ▲でメニュー ("▲: Menu") hint and, when a speed-up
 * level is unlocked (D_8121E908), the speed indicator.
 * Seen in play (2026-11-14) ran while Green's intro played in the GB Player; called from
 * GbEmu_UpdateFrame.
 */
void GbEmu_DrawStatusBar(void) {

    s32 pos;
    s32 dst;
    u8* p;

    GbEmu_ClearStatusBar();
    switch (D_8121E908) {
        case 1:

            pos = D_812599E0.framebuffer + 0xA4;
            GbEmu_DrawTintedBar((u8*)pos, D_81222A5C[D_812599E0.colorIndex], 0x7C);
            GbEmu_DrawSpeedIndicator(D_812599E0.framebuffer);
            pos += 0x2DE;
            break;
        case 2:

            pos = D_812599E0.framebuffer + 0x94;
            GbEmu_DrawTintedBar((u8*)pos, D_81222A5C[D_812599E0.colorIndex], 0x84);
            GbEmu_DrawSpeedIndicator(D_812599E0.framebuffer);
            pos += 0x2EE;
            break;
        case 0:
        default:

            pos = D_812599E0.framebuffer + 0xCE;
            GbEmu_DrawTintedBar((u8*)pos, D_81222A5C[D_812599E0.colorIndex], 0x50);
            pos += 0x288;
            break;
    }
    GbEmu_BlitWithPixelOp(pos + 0x280, D_81220EF8, 0, 0xC, 0xC);
    dst = pos + 0x29C;
    /* The comma expression is what gives the ROM's register allocation. */
    for (p = D_8125766C; (0, p) != &D_8125766C[5]; p++) { // FAKE: comma expression
        GbEmu_BlendColorWithAlphaMap((u16*)dst, D_81259A3C + *p * 0xC0, 0xFFFE, 12, 12, 16); // #FFFFFF
        dst += 0x18;
    }
    sGbEmu = D_812599E0.framebuffer;
}

extern s32 D_812599F0;

/**
 * Clears the 16 RGBA5551 GB palette colors at GB memory +0x12D00 (4 palettes of 4), so the GB
 * screen turns black before the fade out. Seen in RAM (2026-11-14): the 0x20 bytes held F7FA DDB0
 * ABEE 1884 ... before the quit.
 * Seen in play (2026-11-14) ran when はい ("Yes") on the ゲームをやめる ("Quit game") prompt left the GB Player; called from
 * GbEmu_RunFrame.
 */
void GbEmu_ClearPalettes(void) {
    bzero(D_812599F0 + 0x12D00, 0x20);
}

extern u32 D_81259A50;
extern u32 D_81259A54;
/**
 * Bump allocator: aligns the heap pointer to 64 bytes, reserves the size and hangs when the heap
 * runs out.
 * Seen in play (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbEmu_LoadCartridge.
 */
s32 GbEmu_AllocAligned64(s32 arg0, M2C_UNK arg1)
{
  s32 new_var; // FAKE: permuter temp
  s32 temp_v1;
  u32 temp_t6;
  temp_v1 = (D_81259A50 + 0x3F) & (~0x3F);
  new_var = temp_v1; // FAKE: permuter temp
  ; // FAKE: empty statement
  if ((D_81259A50 = new_var + arg0) >= ((u32) D_81259A54)) // FAKE: permuter temp
  {
    loop_1:
    goto loop_1; // FAKE: m2c's endless hang

  }
  return new_var; // FAKE: permuter temp
}

s32 GbPak_RequestBankLoad(s32 chan, s32 bank);
extern u8 D_81259A40;

/**
 * Requests a ROM bank load for the GB Player's port (D_81259A40); called from inside GbCpu_Run when
 * the game switches banks.
 * Not reached in play (2026-11-14): it did not run in the GB Player with JP Green (boot, intro,
 * menu, button config, speed-up, recovery, quit).
 * @unverified Name from the code and Stadium 1.
 */
void GbPak_RequestBankLoadCurrentPort(s32 bank) {
    GbPak_RequestBankLoad(D_81259A40, bank);
}

/**
 * Blocks on an OS timer for the given number of milliseconds.
 * Seen in play (2026-11-13) ran when quitting the GB Player (C-Up menu, Quit game, Yes) and when it
 * started again.
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbEmu_PauseAndBlankScreen.
 */
void GbEmu_Delay(OSTime arg0) {
  OSMesgQueue sp68;
  void* sp64;
  OSTimer sp40;

  osCreateMesgQueue(&sp68, &sp64, 1);
  osSetTimer(&sp40, (1000 * arg0) * 64 / 3, 0, &sp68, NULL);
  osRecvMesg(&sp68, NULL, 1);
}

void _bcopy(void*, void*, s32);
extern u8 D_81223550[];

/**
 * Compares the cartridge header title and version bytes with the four known cartridges in
 * D_81223550 and returns 4..1 for a match, 0 for none.
 * Seen in play (2026-11-12): the table D_81223550 holds YELLOW, BLUE, GREEN and RED with
 * destination 0 (Japan) and licensee 01, so it returns 4 Yellow, 3 Blue, 2 Green, 1 Red; Pocket
 * Monsters Green got 2 and went on to load. The version byte is not compared.
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbEmu_LoadCartridge.
 */
s32 GbPak_IdentifyCartridge(u8* arg0) {
    u8 buf[8];
    u8* entry = D_81223550;
    s32 i;

    _bcopy(arg0 + 0x44, buf, 8);
    for (i = 4; i != 0; i--, entry += 0x18) {
        buf[3] = entry[0x13];
        if ((bcmp(arg0 + 0x34, entry, 0x10) == 0) && (bcmp(buf, entry + 0x10, 8) == 0)) {
            break;
        }
    }
    return i;
}

/**
 * For a cartridge with RAM bank 1 loaded, checks the Gen 1 main save: the player name at 0x598
 * must end in 0x50 within 6 bytes (else 11), and the byte after 0x598-0x1593 must be the inverted
 * sum (else 10). Returns 0 when the save is valid.
 * Seen in play (2026-11-12): starting GB from the main menu with a valid Pocket Monsters Green
 * save ran this right after the cartridge RAM was read, and it returned 0. The game then booted
 * (GAME FREAK logo).
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbPak_IoThreadMain.
 */
s32 GbPak_CheckGen1SaveChecksum(s8* arg0) {
    s32 i;
    s32 sum;

    if ((M2C_FIELD(arg0, u32*, 0x8C) >> 13) == 1) {
        for (i = 0x598; i < 0x59E; i++) {
            if (M2C_FIELD(arg0, u8**, 0x54)[i] == 0x50) {
                goto found;
            }
        }
        return 11;
    found:
        sum = 0;
        for (i = 0x598; i < 0x1594; i++) {
            sum += M2C_FIELD(arg0, u8**, 0x54)[i];
        }
        sum = (sum & 0xFF) ^ 0xFF;
        if (sum != M2C_FIELD(arg0, u8**, 0x54)[i]) {
            return 10;
        }
    }
    return 0;
}

extern s32 D_812580E8;

/**
 * Blacks out the screen, resets the VI X scale, stores the error code in D_812580E8 and returns
 * -1.
 * Seen in play (2026-11-12): error 1 after a failed bank checksum blacked out the screen and led
 * to the 「64GBパックが ただしくセットされていません」 ("The Transfer Pak is not inserted correctly") screen.
 */
s32 GbEmu_BlackScreenAndFail(s32 arg0) {
    osViBlack(1);
    GbEmu_Delay(20);
    osViSetXScale(1.0f);
    D_812580E8 = arg0;
    return -1;
}

void GbEmu_AnimateLoadingScreen(void);
void func_800093E0(void*, void*);
s32 func_80025BD8(void);
void osViSetXScale(f32);
void GbEmu_PauseAndBlankScreen(void);
void GbPak_QueueNextBankLoad(GbPakChannel* ch);
void GbEmu_DrawLoadingScreen(u16*, u8 (*)[0xC80], s32, s32);
void GbCpu_Reset(GbEmuState*);
extern u8 D_8111E570[];
extern u8 D_811565A1;
extern s16 D_800AD928_jp;
extern u8 D_811565A3;
extern u8 D_811565A4;
extern u8 D_811565A5;
extern u8* D_811565A8;
extern s32 D_811565B4;
extern u8 D_8112AF3C[];
extern u8 D_81130024[];
extern u8 D_811645F0[];
extern u8 D_811725F0[];
extern u8 D_811223F4[];
extern u8 D_811224D4[];
extern u8 D_811225B4[];
extern u8 D_81122694[];
extern u8 D_81122774[];
extern f32 D_FLT_81154254;

/**
 * Starts a Game Boy session on Transfer Pak channel arg3: allocates the GB memory, ROM and RAM
 * buffers and identifies the cartridge on every port (GbPak_IdentifyCartridge). A cartridge
 * without a bank list (Yellow, or a revision the tables don't cover) has its four SRAM banks and
 * whole ROM copied before play; Red, Green and Blue instead stream their banks in the background
 * while the game runs. For a streamed load it keeps ROM bytes 0x000-0x101 in D_811565A8 and
 * replaces them with the boot stub D_8111E570, which ends in `jr $` at 0x100. GbEmu_UpdateFrame
 * puts the real bytes back once the GB PC reaches 0x100 while banks are still loading. Seen in
 * play: with Green this showed the 「ロードちゅう」 ("Loading") banner and the Nintendo logo, then the game's own
 * copyright screen.
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbEmu_Init.
 */
s32 GbEmu_LoadCartridge(s32 arg0, s32 arg1, s32 arg2, s32 arg3, u8* arg4);
#ifdef NON_MATCHING
s32 GbEmu_LoadCartridge(s32 arg0, s32 arg1, s32 arg2, s32 arg3, u8* arg4) {
    GbPakChannel* cur;
    GbPakChannel* slot;
    u8* buf;
    s32 i;
    u32 j;
    u8* p;
    s32 pad[2]; // FAKE

    D_811565B4 = 0;
    D_81259A50 = arg1;
    D_81259A54 = arg1 + arg2;
    D_81259A40 = arg3;
    D_81259A38 = M2C_FIELD(arg4, u8**, 0x21CC);
    D_81259A3C = M2C_FIELD(arg4, u8**, 0x21D0);
    D_812599E0.mem = (u8*)GbEmu_AllocAligned64(0x27F80, 0);
    D_812599E0.rom = (u8*)GbEmu_AllocAligned64(0x100000, 0);
    D_812599E0.sramSize = 0x8000;
    D_812599E0.sram = (u8*)GbEmu_AllocAligned64(D_812599E0.sramSize, 0);
    D_812599E0.unk_44 = (u8*)GbEmu_AllocAligned64(0xF000, 0);
    D_812599E0.unk_48 = (u8*)GbEmu_AllocAligned64(0xF000, 0);
    sGbEmu = D_812599E0.framebuffer = GbEmu_AllocAligned64(0x2800, 0);
    D_812599E0.unk_50 = (u8*)GbEmu_AllocAligned64(0x2800, 0);
    D_811565A8 = (u8*)GbEmu_AllocAligned64(0x102, 0);
    D_811565A3 = 0;
    D_81259917 = 1;
    D_811565A1 = 0;
    D_811565A5 = 0x20;
    D_811565A4 = 0;
    bzero((s32)D_812599E0.mem, 0xFF80);
    for (i = 0, p = D_812599E0.mem + 0x16B20; i < 0x40; i++) {
        *p++ = 1;
    }
    for (i = 0, p = D_812599E0.mem + 0x16B60; i < 0x20; i++) {
        *p++ = 1;
    }
    GbEmu_ClearStatusBar();
    i = func_80025BD8();
    D_81259668[i].request = 0x80;
    osSendMesg(&D_812598F8, i, 1);
    while (D_81259915 != 0) {
    }

    buf = (u8*)GbEmu_AllocAligned64(0x200, 0);
    for (i = 0; i < 4; i++) {
        slot = &D_81259668[i];
        slot->bankState = D_812599E0.mem + 0x16B20;
        slot->sramState = D_812599E0.mem + 0x16B60;
        slot->ramBuf = D_812599E0.sram;
        slot->blockState = D_812599E0.mem + 0x16B80;
        slot->verifyBuf = buf;
        slot->blockDirty = buf + 0x100;
        slot->ramSize = D_812599E0.sramSize;
        for (j = 0; j < (D_812599E0.sramSize >> 8); j++) {
            slot->blockDirty[j] = 0xFF;
            slot->blockState[j] = 0xFF;
        }
        slot->gameIndex = GbPak_IdentifyCartridge((u8*)slot);
        slot->romBuf = D_812599E0.rom;
        slot->sramBuf = D_812599E0.sram;
    }
    cur = &D_81259668[D_81259A40];
    if (cur->error != 0) {
        GbEmu_BlackScreenAndFail(1);
        return (cur->unkA2 != 0) ? 2 : 1;
    }
    if (cur->gameIndex == 0) {
        return GbEmu_BlackScreenAndFail(1);
    }
    D_812599E0.colorIndex = cur->gameIndex - 1;
    while (D_81259915 != 0) {
    }
    if (GbPak_WriteVerifiedSramBlock(D_81259A40, 0, 0xFF) != 0) {
        GbEmu_BlackScreenAndFail(1);
        return (cur->unkA2 != 0) ? 2 : 1;
    }
    D_812599E0.unk_38 = 0xE;
    if (cur->romSize == 4) {
        D_812599E0.romSize = 0x80000;
    } else {
        D_812599E0.romSize = 0x100000;
    }
    D_812599E0.unk_07 = 0;
    D_812599E0.unk_06 = 1;
    GbCpu_Reset(&D_812599E0);
    GbEmu_ClearStatusBar();

    /* FAKE: the unsigned `1U` compares stop the constant 1 from sharing a register with unk_06 = 1. */
    cur->bankList = (cur->gameIndex == 4)   ? NULL
                    : (cur->gameIndex == 3) ? ((cur->maskRomVersion == 0) ? D_81122774 : NULL)
                    : (cur->gameIndex == 2) ? ((cur->maskRomVersion == 1U) ? D_81122694 : (cur->maskRomVersion == 0) ? D_811225B4 : NULL)
                                        : ((cur->maskRomVersion == 1U) ? D_811224D4 : (cur->maskRomVersion == 0) ? D_811223F4 : NULL);
    cur->sum = 0;

    if (cur->bankList == NULL) {
        GbEmu_DrawLoadingScreen((u16*)arg0, M2C_FIELD(arg4, u8 (**)[0xC80], 0x2204), func_80025BD8(), D_812599E0.colorIndex);
        GbEmu_DrawStatusBar();
        osWritebackDCacheAll();
        osViBlack(0);
        for (i = 0; i < 4; i++) {
            cur->request = 2;
            cur->addr = i * 0x2000;
            cur->buf = i * 0x2000 + D_812599E0.sram;
            cur->size = 0x2000;
            osSendMesg(&D_812598F8, D_81259A40, 1);
            while (D_81259915 != 0) {
                GbEmu_AnimateLoadingScreen();
            }
            if (cur->error) goto fail; /* one line: gives the ROM's beqz/b pair */
            if (D_800AD928_jp != 0) {
                GbEmu_BlackScreenAndFail(0);
            }
        }
        for (i = 0; i < (D_812599E0.romSize >> 13); i++) {
            cur->request = 1;
            if (1) {} // karodo
            if (1) {} // karodo
            if (1) {} // karodo
            cur->addr = i * 0x2000;
            cur->buf = i * 0x2000 + D_812599E0.rom;
            cur->size = 0x2000;
            osSendMesg(&D_812598F8, D_81259A40, 1);
            while (D_81259915 != 0) {
                GbEmu_AnimateLoadingScreen();
            }
            if (cur->error) goto fail; /* one line: gives the ROM's beqz/b pair */
            if (D_800AD928_jp != 0) {
                GbEmu_BlackScreenAndFail(0);
            }
        }
        for (i = 0; i < (D_812599E0.romSize >> 14); i++) {
            cur->bankState[i] = 0;
        }
        for (i = 0; i < (D_812599E0.sramSize >> 13); i++) {
            cur->sramState[i] = 0;
        }
        func_800093E0(D_8112AF3C, D_811645F0);
        func_800093E0(D_81130024, D_811725F0);
    } else {
        GbEmu_DrawAccessOverlay(0);
        osWritebackDCacheAll();
        D_81259916 = 1;
        GbPak_RequestBankLoad(D_81259A40, 0);
        while (*cur->bankState != 0) {
            GbPak_QueueNextBankLoad(cur);
            if (cur->error != 0) {
            fail:
                GbEmu_BlackScreenAndFail(1);
                return (cur->unkA2 != 0) ? 2 : 1;
            }
            if (D_800AD928_jp != 0) {
                GbEmu_BlackScreenAndFail(0);
            }
        }
        D_811565A4 = 1;
        for (i = 0; i < 258; i++) {
            D_811565A8[i] = D_812599E0.rom[i];
            D_812599E0.rom[i] = D_8111E570[i];
        }
        D_812599E0.gbPc = 0;
        D_812599E0.mem[0xFF42] = 0x64;
    }
    D_812599E0.unk_06 = 0;
    GbEmu_PauseAndBlankScreen();
    osViSetXScale(D_FLT_81154254);
    osViBlack(0);
    return 0;
}
#else
// clang-format off
#pragma GLOBAL_ASM("asm/jp/nonmatchings/fragments/gb_tower_emulator/jp/gb_emu_pak/GbEmu_LoadCartridge.s")
// clang-format on
#endif

/* Fixed-layout split at GbEmu_UpdateFrame so its jump table lands at 0x81154258. */
/* Stadium Zero fragment000_80C00. */
/* C ported from the Stadium 1 decomp: src/fragments/gb_tower_emulator/gb_tower_emulator_7F9A0.c. */

#include "global.h"
#include "gb_tower_emulator_jp.h"
#include "variables.h"

#include "m2c_macros.h"

// Declared here instead of in a header: the callers of these disagree on their types.
void GbAudio_UpdateFrame();

void GbEmu_BlitWithPixelOp(u16* dst, u16* src, s32 mode, s32 width, s32 height);
void GbEmu_BlendScrollingLayer(u16* arg0, u16* arg1, s16* arg2, s32 arg3, s32 arg4);

s32 GbPak_VerifyProbeBlock(void* ch);

extern GbPakChannel D_81259668[4];
extern volatile u8 D_81259915;
extern volatile u8 D_81259916;
extern OSMesgQueue D_812598F8;
s32 GbPak_WriteBackDirtySramBlock(s32 arg0);
u32 osGetCount(void);
extern s32 D_812599F0;
extern u8 D_8121E908;
void GbEmu_BlitWithPixelOp(u16* dst, u16* src, s32 mode, s32 width, s32 height);
extern u8 D_81259A42;
extern GbEmuState D_812599E0;
extern u8 D_81259A40;
void _bcopy(void*, void*, s32);
extern s32 D_812580E8;

void GbEmu_ClearStatusBar(void);
void GbEmu_DrawAccessIcon(u16* arg0, s32 arg1, s32 arg2);
void GbEmu_DrawSpeedIndicator(s32 fb);
void GbEmu_DrawAccessOverlay(s32 direction);
void GbEmu_DrawStatusBar(void);



#define GB_RUN (&D_812599E0)
#define GB_MEM (GB_RUN->mem)

extern volatile u8 D_81259915;
extern volatile u8 D_81259916;
extern OSMesgQueue D_812598F8;
extern u8 D_81259A40;
extern u8 D_811565A1;
extern u8 D_811565A3;
extern u8 D_811565A4;
extern u8 D_811565A5;
extern u8* D_811565A8;
extern u8 D_811565B8;
extern u8 D_811565B9;
extern u8 D_8121E908;
extern u16 D_8111E30C;
extern u16 D_8111E310;
extern u16 D_811228B4;
extern u8 D_811228B8;
extern s32 D_812580E8;
extern u8 D_81154C0F;
extern u8 D_81154C4F;
extern GbEmuTask D_81154BF0;
extern u8* D_800603C0;
extern u8 D_8112AF3C[];
extern u8 D_81130024[];
extern u8 D_811565F0[];
extern u8 D_811645F0[];
extern u8 D_811725F0[];
extern u8 D_81135178[];
extern u8 D_81135378[];
extern u8 D_8113556C[];
extern u8 D_8111F470[];
extern u8 D_8111E678[];
void func_800093E0(void*, void*);
void GbPak_QueueNextBankLoad(GbPakChannel* ch);
void Cont_StartReadInputs(void);
void Cont_ReadInputs(void);
u8 GbCpu_Run(void*, s32);
void osInvalDCache(void*, s32);
void GbAudio_SetMenuSoundMode(s32);

/**
 * Runs one frame of the GB Player emulator (Stadium 1's GbEmu_UpdateFrame).
 * Services the Transfer Pak, builds the emulated joypad from the controller,
 * lets C-Right cycle the emulation speed, steps the CPU core that many times
 * and queues the RSP task that draws the Game Boy screen. When enabled, Z
 * cycles the graphics set copied in for the screen frame. Returns 1 when only
 * C-Up is held, a negative value on a Transfer Pak error, -100 when the core
 * stopped, otherwise 0.
 * Seen in play (2026-11-12): runs Green frame by frame and, while banks load, restores the header
 * bytes once the GB PC (gbPc) reaches 0x100.
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbEmu_RunFrame.
 */
s32 GbEmu_UpdateFrame(void);
#ifdef NON_MATCHING
s32 GbEmu_UpdateFrame(void) {
    s32 i;
    s32 err;
    s32 status;
    u8* src;
    // FAKE: state and timer only exist so that IDO addresses D_811228B8, D_811565B8 and D_811565B9 with a
    // fresh lui on every access, as in retail. Function-scope statics give the same code and are the likely
    // original, but would move these bytes into this file's own data/bss sections.
    u8* state;
    u8* timer;
    s32 j;
    s32 count;
    s32 cycles;
    u8 steps;
    s32 btn;
    u8 val;
    s32 remap_a;
    s32 remap_b;
    s32 pad; // FAKE: unused, only reserves the stack slot the retail frame has
    u8* dst;

    status = 0;
    if (D_81259916 != 0) {
        // While banks stream in, the GB runs a boot stub that ends in `jr $` at 0x100 (see
        // GbEmu_LoadCartridge). Once it gets there, put the real header bytes back.
        if (D_811565A4 != 0 && GB_RUN->gbPc == 0x100) {
            if (--D_811565A4 == 0) {
                func_800093E0(D_8112AF3C, D_811645F0);
                func_800093E0(D_81130024, D_811725F0);
                for (j = 0; j < 258; j++) {
                    GB_RUN->rom[j] = D_811565A8[j];
                }
            }
        }
        GbPak_QueueNextBankLoad(&D_81259668[D_81259A40]);
        if (D_81259916 != 0) {
            if (D_81259668[D_81259A40].error != 0) {
                status = (D_81259668[D_81259A40].unkA2 != 0) ? -3 : -1;
            }
            if (++D_811565A3 >= 11) {
                D_811565A3 = 0;
            }
            GbEmu_DrawAccessIcon((u16*)(GB_RUN->unk_50 + 0xB0), 0, D_811565A3);
            GB_RUN->buttons = 0;
        } else {
            GbEmu_ClearStatusBar();
            GbEmu_DrawStatusBar();
        }
    } else {
        for (i = 0; i < 5; i++) {
            D_81259668[D_81259A40].blockDirty[i] = 0xFF;
            D_81259668[D_81259A40].blockState[i] = D_81259668[D_81259A40].blockDirty[i];
        }
        timer = &D_811565B8; // FAKE: frame counter here, reused below for the D_811565B9 timer
        err = GbPak_WriteBackDirtySramBlock(D_81259A40);
        if (err == 0) {
            Cont_StartReadInputs();
            Cont_ReadInputs();
            if ((++*timer & 3) == 0) {
                D_81259668[D_81259A40].request = 8;
                osSendMesg(&D_812598F8, D_81259A40, 1);
            }
            remap_a = (gPlayer1Controller->buttonDown & D_8111E30C) ? 0x1000 : 0;
            remap_b = (gPlayer1Controller->buttonDown & D_8111E310) ? 0x2000 : 0;
            GB_RUN->buttons = (gPlayer1Controller->buttonDown & ~0x3000) | remap_a | remap_b;
            if (D_8121E908 != 0) {
                j = GB_MEM[0x101B9];
                btn = gPlayer1Controller->buttonPressed;
                if (btn & 1) {
                    gPlayer1Controller->buttonPressed = btn & 0xFFFE;
                    switch (j) {
                        case 1:
                            j = 2;
                            break;
                        case 2:
                            if (D_8121E908 >= 2) {
                                j = 4;
                                break;
                            }
                        default:
                            j = 1;
                            break;
                    }
                    GB_MEM[0x101B9] = j;
                    GbEmu_DrawSpeedIndicator(GB_RUN->framebuffer);
                }
            }
        } else if (D_81259668[D_81259A40].error != 0) {
            status = (D_81259668[D_81259A40].unkA2 != 0) ? -3 : -2;
        } else {
            if (err == 2) {
                if (D_811565A1 == 0) {
                    GbEmu_DrawAccessOverlay(1);
                }
                D_811565A1 = 10;
            }
            GB_RUN->buttons = 0;
        }
    }

    if (D_811565A1 != 0) {
        //!bug err is only set on the D_81259916 == 0 path; with a pak present this reads an uninitialized local
        if (err == 0) {
            D_811565A1 = D_811565A1 - 1;
        }
        if (D_811565A1 == 0) {
            GbEmu_DrawStatusBar();
        } else {
            if (++D_81259A42 >= 11) {
                D_81259A42 = 0;
            }
            GbEmu_DrawAccessIcon((u16*)(GB_RUN->unk_50 + 0xB0), 1, D_81259A42);
        }
        GB_RUN->buttons = 0;
    }

    if (GB_RUN->unk_06 != 0) {
        GB_RUN->unk_06 = 0;
        GbAudio_SetMenuSoundMode(0);
    }

    if (status == 0) {
        count = GB_MEM[0x101B9];
        if ((GB_MEM[0x1017D] == 0) && (GB_MEM[0x10181] == 0) && (GB_MEM[0xFF40] & 0x80) &&
            ((count != 1) || (GB_RUN->colorIndex != 3))) {
            i = GB_MEM[0x101D1];
            if (i == 0) {
                if (GB_MEM[0x101CD] != 0) {
                    GB_MEM[0x101CD] = 0;
                    D_811228B4 = 0xFFFF;
                    i = 10;
                }
            } else {
                i--;
                if (i == 10) {
                    GB_MEM[0x101CD] = D_811228B4 != GB_MEM[0xFF47];
                } else if ((i < 10) && (GB_MEM[0x101CD] != 0)) {
                    GB_MEM[0x101CD] = 0;
                    i = 20;
                    D_811228B4 = GB_MEM[0xFF47];
                }
            }
        } else {
            GB_MEM[0x101CD] = 0;
            i = 0;
        }
        GB_MEM[0x101D1] = i;

        steps = count;
        cycles = 0;
        state = &D_811228B8;
        timer = &D_811565B9;
        while (count != 0) {
            *(s32*)(GB_MEM + 0x10100) = cycles;
            if (count == 1) {
                GB_MEM[0x101C1] = 0;
            } else {
                GB_MEM[0x101C1] = 1;
            }
            i = GbCpu_Run(&D_812599E0, 0);
            if ((i != 0) || (GB_MEM[0x1017D] != 0) || (cycles += 17248, GB_MEM[0x10181] != 0) ||
                (GB_MEM[0x1018D] != 0)) {
                break;
            }
            count--;
            if ((steps == 4) || (steps == 2)) {
                val = GB_MEM[0xFF47];
                switch (*state) {
                    case 0:
                        GB_MEM[0x101D5] = 0;
                        if (val == 0xE4) {
                            *state = 1;
                        }
                        break;
                    case 1:
                        if ((val == 0x1B) || (val == 0xE6)) {
                            *state = 2;
                        } else if (val != 0xE4) {
                            *state = 0;
                        }
                        break;
                    case 2:
                    case 4:
                        if (val == 0xE4) {
                            (*state)++;
                            *timer = steps * 20;
                        } else if (val == 0) {
                            *state = 6;
                            *timer = steps * 20;
                        } else if ((val != 0x1B) && (val != 0xE6)) {
                            *state = 0;
                        }
                        break;
                    case 3:
                    case 5:
                        if (val == 0xE4) {
                            if (--*timer == 0) {
                                *state = 1;
                            }
                        } else if ((val == 0x1B) || (val == 0xE6)) {
                            (*state)++;
                            *timer = steps * 20;
                        } else {
                            *state = 0;
                        }
                        break;
                    case 6:
                        GB_MEM[0x101D1] = 0;
                        GB_MEM[0xFF47] = 0xE4;
                        GB_MEM[0x101D5] = 0xFF;
                        if (val == 0x40) {
                            *state = 0;
                        }
                        if (--*timer == 0) {
                            *state = 0;
                        }
                        break;
                }
            } else {
                GB_MEM[0x101D5] = 0;
            }
        }
        if (1) { // karodo
        }
    } else {
        i = 0;
    }

    while (D_81259668[D_81259A40].request == 8) {
    }
    if (D_81259668[D_81259A40].error != 0) {
        D_812580E8 = 2;
        status = (D_81259668[D_81259A40].unkA2 != 0) ? -3 : -1;
    }

    if (GB_MEM[0x1017D] || GB_MEM[0x10181] || GB_MEM[0x1018D]) {
        if (GB_MEM[0x10181]) {
            if (GB_MEM[0x10181] != GB_MEM[0x10185]) {
                GB_MEM[0x10185] = GB_MEM[0x10181];
                GB_MEM[0x1018D] = 2;
            } else {
                GB_MEM[0x1018D]--;
                if (GB_MEM[0x1018D] == 0) {
                    osInvalDCache(GB_MEM + 0x12F00, 0x1000);
                    GB_MEM[0x10181] = 0;
                    GB_MEM[0x10185] = GB_MEM[0x10181];
                }
            }
        }
        D_81154BF0.type = 2;
        D_81154BF0.flags = 0;
        D_81154BF0.ucode = D_8111F470;
        D_81154BF0.ucodeSize = 0xE38;
        D_81154BF0.unk_18 = GB_MEM + 0xFE00;
        D_81154BF0.unk_1C.word = 0x0E300100;
        D_81154BF0.unk_30 = *(s32*)(GB_MEM + 0x10140);
        D_81154BF0.unk_38 = *(s32*)(GB_MEM + 0x10190);
        D_81154BF0.unk_20 = GB_RUN->unk_44;
        D_81154BF0.unk_14 = 0x10;
        D_81154BF0.unk_15 = 0xD1 - (D_811565A5 / 2);
        D_81154BF0.unk_1C.bytes[3] = GB_MEM[0x101A1];
    } else {
        D_81154BF0.type = 2;
        D_81154BF0.flags = 0;
        D_81154BF0.ucode = D_8111E678;
        D_81154BF0.ucodeSize = 0xDF8;
        D_81154BF0.unk_18 = GB_MEM + 0xFE00;
        D_81154BF0.unk_1C.word = 0x0E300100;
        D_81154BF0.unk_30 = *(s32*)(GB_MEM + 0x10140);
        D_81154BF0.unk_38 = *(s32*)(GB_MEM + 0x10190);
        D_81154BF0.unk_20 = GB_RUN->unk_44;
        D_81154BF0.unk_14 = 0x10;
        D_81154BF0.unk_15 = 0xD1 - (D_811565A5 / 2);
        D_81154BF0.unk_1C.bytes[3] = GB_MEM[0x101A1];
        if ((D_81259915 == 0) && (status == 0)) {
            if ((GB_RUN->buttons == 8) && (D_81154C0F == 0)) {
                return 1;
            }
            btn = gPlayer1Controller->buttonPressed;
            if ((btn & 0x2000) && (D_81154C0F == 0) && (D_8121E908 != 0)) {
                gPlayer1Controller->buttonPressed = btn & 0xDFFF;
                if (((GB_MEM[0x1019A] | GB_MEM[0x1019B] | GB_MEM[0x1019D]) == 0) && (GB_MEM[0x101C9] != 0)) {
                    if (++D_81154C4F > D_8121E908) {
                        D_81154C4F = 0;
                    }
                    // Each case keeps both assignments on one line; IDO only schedules these blocks like retail that way.
                    switch (D_81154C4F) {
                        case 1:
                            src = D_811645F0; dst = D_81135378;
                            break;
                        case 2:
                            src = D_811725F0; dst = D_8113556C;
                            break;
                        default:
                            src = D_811565F0; dst = D_81135178;
                            break;
                    }
                    _bcopy(src, GB_RUN->unk_48 + 0x800, 0xE000);
                    _bcopy(dst, GB_MEM + 0x27D80, 0x200);
                    GB_MEM[0x1019D] = 1;
                }
            }
        }
    }

    if ((D_811565A5 != 0) && (D_811565A5 != 0x20)) {
        D_811565A5 = D_811565A5 - 1;
    }
    if (status != 0) {
        return status;
    }
    if (i != 0) {
        return -100;
    }
    return 0;
}
#else
// clang-format off
#pragma GLOBAL_ASM("asm/jp/nonmatchings/fragments/gb_tower_emulator/jp/gb_emu_pak/GbEmu_UpdateFrame.s")
// clang-format on
#endif

extern OSMesgQueue D_812598F8;
extern u8 D_81259A40;

/**
 * Asks the I/O thread for a connector check (request 8) and spins until it is done. Returns -1
 * when the channel reports an error.
 * Seen in play (2026-11-14) ran when C-Up opened the GB Player menu; called from GbEmu_RunFrame.
 */
s32 GbPak_CheckConnectionBlocking(void) {
    volatile u8* chan = (volatile u8*)&D_81259668[D_81259A40];

    while (chan[0x99] != 0) {
    }
    if (chan[0x9A] != 0) {
        return -1;
    }
    chan[0x99] = 8;
    osSendMesg(&D_812598F8, (OSMesg)(u32)D_81259A40, OS_MESG_BLOCK);
    while (chan[0x99] != 0) {
    }
    if (chan[0x9A] != 0) {
        return -1;
    }
    return 0;
}

extern u8 D_81259A40;
/**
 * Asks the I/O thread to re-init the pak and read the cartridge ID into the channel (request
 * 0x80), waits, and then checks the RAM probe block. Returns the error, or 0 when it is the same
 * cartridge.
 * Seen in play (2026-11-14) ran in the 「レポートできませんでした」 ("Couldn't save") recovery flow (state forced to 20, cartridge
 * removal faked by stubbing GbPak_IsCartridgeInserted); called from GbEmu_RunFrame.
 */
s32 GbPak_ReidentifyCartridge(void) {
    GbPakChannel* ch = &D_81259668[D_81259A40];

    ch->request = 0x80;
    ch->error = 0;
    osSendMesg(&D_812598F8, D_81259A40, 1);
    while (D_81259915 != 0) {
    }

    if (ch->error != 0) {
        return ch->error;
    }
    if (ch->unk98 != 0) {
        return GbPak_VerifyProbeBlock(ch);
    }
    return 0;
}

/**
 * Asks the I/O thread for a status check (request 8), waits, and returns the GB cartridge-present
 * bit (0x80) of the pak status.
 * Seen in play (2026-11-14) ran in the 「レポートできませんでした」 ("Couldn't save") recovery flow (state forced to 20, cartridge
 * removal faked by stubbing GbPak_IsCartridgeInserted); called from GbEmu_RunFrame.
 */
s32 GbPak_IsCartridgeInserted(void) {
    u8 idx = D_81259A40;
    volatile GbPakChannel* ch = &D_81259668[idx];

    ch->status = 0;
    ch->request = 8;
    osSendMesg(&D_812598F8, idx, 1);
    while (D_81259915 != 0) {
    }
    return D_81259668[D_81259A40].status & 0x80;
}

extern M2C_UNK D_8115623C;
extern u8 D_811565A1;

/**
 * Returns 1 when D_811565A1 is set or any of the 128 cartridge RAM blocks is still waiting to be
 * written back; the menu then goes to the 「レポートできませんでした」 ("Couldn't save") recovery instead of failing.
 * Seen in play (2026-11-14) ran when the emulator result D_81154C40 was poked to -1 from the GB
 * Player menu; the game went to 「レポートできませんでした」 ("Couldn't save"); called from GbEmu_RunFrame.
 */
s32 GbPak_HasUnsavedSramBlocks(void);
#ifdef NON_MATCHING
s32 GbPak_HasUnsavedSramBlocks(void) {
    s32 var_v0;
    u8 *var_v1;

    var_v0 = 0;
    if (D_811565A1 != 0) {
        return 1;
    }
    var_v1 = *((M2C_UNK*)((u8*)&D_8115623C + (D_81259A40 * 0xA4))); // FAKE: cast chain
loop_3:
    var_v0 += 4;
    if (M2C_FIELD(var_v1, u8 *, 0) == 0) {
        return 1;
    }
    if (M2C_FIELD(var_v1, u8 *, 1) == 0) {
        return 1;
    }
    if (M2C_FIELD(var_v1, u8 *, 2) == 0) {
        return 1;
    }
    if (M2C_FIELD(var_v1, u8 *, 3) == 0) {
        return 1;
    }
    var_v1 += 4;
    if (var_v0 == 0x80) {
        return 0;
    }
    goto loop_3; // FAKE: m2c loop
}
#else
// clang-format off
#pragma GLOBAL_ASM("asm/jp/nonmatchings/fragments/gb_tower_emulator/jp/gb_emu_pak/GbPak_HasUnsavedSramBlocks.s")
// clang-format on
#endif

extern u8 D_81259701[];
extern OSMesgQueue D_812598F8;
extern volatile u8 D_81259915;

/**
 * Asks the I/O thread to power the Transfer Pak off (request 4) and waits.
 * Seen in play (2026-11-14) ran in the 「レポートできませんでした」 ("Couldn't save") recovery flow (state forced to 20, cartridge
 * removal faked by stubbing GbPak_IsCartridgeInserted); called from GbEmu_RunFrame.
 */
void GbPak_PowerOff(void) {
    D_81259701[D_81259A40 * 0xA4] = 4;
    osSendMesg(&D_812598F8, (OSMesg)(u32)D_81259A40, OS_MESG_BLOCK);
    while (D_81259915 != 0) {
    }
}

extern s8 D_812599E6;

/**
 * Sets D_812599E6, which stops the per-frame GB audio update.
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbEmu_PauseAndBlankScreen.
 */
void GbEmu_PauseEmulation(void) {
    D_812599E6 = 1;
}

extern s16 D_82608542;
extern s16 D_82608544;
extern s16 D_8260854C;
extern s16 D_8260854E;
extern s16 D_800AD928_jp;
extern s32 D_812599F0;
void GbAudio_UpdateFrame();
/**
 * Per-frame tail of the GB Player emulator: unless emulation is paused (D_812599E6) or
 * the screen is being blanked (D_800AD928_jp), runs the GB audio frame and mirrors the
 * emulated NR52 (0xFF26) register into GB memory; while D_800AD928_jp is set it blanks video.
 * Seen in play (2026-11-13) ran every frame from the moment JP Green booted in the N64 GB Player.
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from GbPlayer_OutputThreadEntry.
 */
void GbEmu_AudioVideoTick(UNUSED s32 arg0) {
    if ((*(u8*)&D_812599E6 == 0) && (D_800AD928_jp == 0)) {
        GbAudio_UpdateFrame();
        ((u8*)D_812599F0)[0xFF26] = GbMem_ReadIoRegister(0xFF26);
    }
    if (D_800AD928_jp != 0) {
        osViBlack(1);
    }
}
