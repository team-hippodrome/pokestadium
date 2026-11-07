#include "gb_tower.h"
#include "hal_libc.h"
#include "controller.h"
#include "game_state.h"
#include "gb_mbc.h"

// gb_tower.c
// dont forget to pass -signed when compiling

OSPfs gGbPakPfs[4];
u8 gGbPakPortReady[4];
u8 gGbPakInitMask;
u8 gGbPakUsableMask;
u8 gGbPakPlugMask;
u8 gGbPakPollMask;
s32 gGbPakDisconnected;
u8 gGbPakFailCount[4];

// these first 4 funcs might be part of a hal_libc file or something instead of GB Tower.

s32 GbPak_MbcRead(s32 port, void* buffer, u16 address, u16 size);
s32 GbPak_MbcWrite(s32 port, void* buffer, u16 address, u16 size);

// TODO: These 0x20 sizes probably belong to a struct size or something. Use
// that instead of assuming sizeof is the same
/**
 * Writes a 32-byte pattern to cartridge RAM 0xA000 through the MBC, reads it back
 * and returns 1 if it matches and the pak status bit 2 is clear.
 */
s32 GbPak_VerifyPattern(s32 port, void* pattern) {
    u8 status;
    u8 read_back[0x20];
    s32 result = 0;
    UNUSED u8 padding[8];

    if ((GbPak_MbcWrite(port, pattern, 0, 0x20) == 0) && (GbPak_MbcRead(port, (uintptr_t)&read_back, 0, 0x20) == 0) &&
        (bcmp(&read_back, pattern, 0x20) == 0) && (osGbpakGetStatus(&gGbPakPfs[port], &status) == 0)) {
        result = ((status & 4) != 0) == 0;
    }
    return result;
}

/**
 * Detects working cartridge RAM by verifying 0x55, 0xAA and then an osGetCount
 * pattern; the last pattern is left in the pattern buffer for GbTower_VerifyPakStillInserted.
 */
s32 GbPak_DetectRam(s32 port, u8* pattern) {
    s32 result = 0;
    u8 local_pattern[0x20];

    if (pattern == NULL) {
        pattern = local_pattern;
    }
    HAL_Memset((char*)pattern, 0x55, 0x20);
    if (GbPak_VerifyPattern(port, pattern) != 0) {
        HAL_Memset((char*)pattern, 0xAA, 0x20);
        if (GbPak_VerifyPattern(port, pattern) != 0) {
            s32 i;
            for (i = 0; i < ARRAY_COUNT(local_pattern); i++) {
                pattern[i] = osGetCount();
            }
            result = GbPak_VerifyPattern(port, pattern);
        }
    }
    return result;
}

/**
 * Prepares the pak on a port once (gGbPakPortReady): reads the ID, enables
 * cartridge RAM and runs GbPak_DetectRam. Shuts down with error 1 if the read fails.
 */
s32 GbTower_ProbePak(s32 port, u8* was_ready, u8* pattern) {
    u8 status;
    OSGbpakId gbpak_id;
    s32 has_ram = 0;

    if (was_ready != NULL) {
        *was_ready = gGbPakPortReady[port];
    }

    if (gGbPakPortReady[port] == 1) {
        return 1;
    }

    if (osGbpakReadId(&gGbPakPfs[port], &gbpak_id, &status) != 0) {
        Game_ShutdownAndLoadFragment(port, 1);
    }
    if ((status & OS_GBPAK_RSTB_STATUS) && (osGbpakCheckConnector(&gGbPakPfs[port], &status) == 0)) {
        osGbmbcRamEnable(&gGbPakPfs[port]);
        has_ram = GbPak_DetectRam(port, pattern);
    }
    gGbPakPortReady[port] = 1;
    return has_ram;
}

/**
 * Returns 1 if the pak has not latched a reset (OS_GBPAK_RSTB_DETECTION clear), i.e. the
 * cartridge was not removed; powers the pak off when keep_powered == 0.
 */
s32 GbTower_IsPakInserted(s32 port, u8 keep_powered) {
    u8 status;
    s32 not_reset;

    if (osGbpakGetStatus(&gGbPakPfs[port], &status) != 0) {
        Game_ShutdownAndLoadFragment(port, 1);
    }

    not_reset = !((status & OS_GBPAK_RSTB_DETECTION) != 0);
    if (keep_powered == 0) {
        osGbpakPower(&gGbPakPfs[port], 0);
        gGbPakPortReady[port] = 0;
    }
    return not_reset;
}

/**
 * Powers off every pak that was initialized by GbTower_ScanPaks.
 */
void GbTower_PowerOffAllPaks(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        if (gGbPakInitMask & (1 << i)) {
            osGbpakPower(&gGbPakPfs[i], 0);
            gGbPakPortReady[i] = 0;
        }
    }
}

/**
 * Returns 1 if the cartridge header is a supported Pokemon Red/Green/Blue/Yellow
 * cartridge for this region.
 */
#if defined(VERSION_US_REV2)
#pragma GLOBAL_ASM("asm/us/nonmatchings/gb_tower/GbPak_IsPokemonCartridge.s")
#else
s32 GbPak_IsPokemonCartridge(OSGbpakId* header) {
    // make sure its 0 terminated. dont want those dirty hackers abusing a Strcmp exploit
    header->game_title[15] = '\0';

#if defined(VERSION_JP)
    // Accept only Japanese cartridges (header country code 0).
    if (header->country_code == 0) {
#else
    // is the cartridge non-Japanese?
    if (header->country_code == 1) {
#endif
        // Which supported POKeMON version is this?
        if (HAL_Strcmp((char*)header->game_title, "POKEMON RED") == 0) {
            return 1;
        }
        if (HAL_Strcmp((char*)header->game_title, "POKEMON GREEN") == 0) {
            return 1;
        }
        if (HAL_Strcmp((char*)header->game_title, "POKEMON BLUE") == 0) {
            return 1;
        }
        if (HAL_Strcmp((char*)header->game_title, "POKEMON YELLOW") == 0) {
            return 1;
        }
    }
    // This is not a supported cartridge.
    return 0;
}
#endif

/**
 * Initializes every plugged-in Transfer Pak and checks its cartridge. Returns
 * (gGbPakUsableMask << 16) | gGbPakInitMask.
 */
s32 GbTower_ScanPaks(void) {
    s32 i;
    s32 read_id_error;
    u8 status;
    OSGbpakId gbpak_id;

    gGbPakUsableMask = 0;
    gGbPakInitMask = 0;
    gGbPakPollMask = 0;
    gGbPakDisconnected = 0;
    osPfsIsPlug(&gSIEventMesgQueue, &gGbPakPlugMask);

    for (i = 0; i < 4; i++) {
#if defined(VERSION_JP)
        // Port 3 is probed like the others; there is no Controller_CheckAccessoryEeprom gate.
        if (gGbPakPlugMask & (1 << i)) {
#else
        if ((gGbPakPlugMask & (1 << i)) && ((i != 3) || (Controller_CheckAccessoryEeprom() == 0))) {
#endif
            if (osGbpakInit(&gSIEventMesgQueue, &gGbPakPfs[i], i) == 0) {
                gGbPakInitMask |= (1 << i);
                read_id_error = osGbpakReadId(&gGbPakPfs[i], &gbpak_id, &status);
                if (read_id_error == 0) {
                    if (!(status & 8)) {
                        Game_ShutdownAndLoadFragment(i, 2);
                    }
                    if (GbPak_IsPokemonCartridge(&gbpak_id) != 0) {
#if !defined(VERSION_JP)
                        if (read_id_error == 4) {
                            // what. the redundant read_id_error functionless check is apparently needed to match.
                        }
#endif
                        if (osGbpakCheckConnector(&gGbPakPfs[i], &status) == 0) {
                            osGbmbcRamEnable(&gGbPakPfs[i]);
                            gGbPakPortReady[i] = 1;
                            gGbPakUsableMask |= (1 << i);
                        } else {
                            Game_ShutdownAndLoadFragment(i, 2);
                        }
                    } else {
                        gGbPakUsableMask |= (1 << i);
                        gGbPakPortReady[i] = 1;
                    }
                } else {
                    gGbPakPortReady[i] = 0;
                    if (read_id_error == 4) {
                        Game_ShutdownAndLoadFragment(i, 2);
                    }
                }
            }
        }
    }
    return (gGbPakUsableMask << 0x10) | gGbPakInitMask;
}

/**
 * Returns 1 if the Transfer Pak reports no powered cartridge; shuts down on a pak error.
 */
s32 GbPak_IsCartOff(s32 port) {
    u8 status;
    s32 ret = osGbpakGetStatus(&gGbPakPfs[port], &status);
    UNUSED u8 filler;

    // check the error code returned (if applicable) by the osGbpakGetStatus
    // call.
    if ((ret == PFS_ERR_NOPACK) || (ret == PFS_ERR_DEVICE) || (ret == PFS_ERR_CONTRFAIL)) {
        Game_ShutdownAndLoadFragment(port, 1);
    }
    return !((status & OS_GBPAK_GBCART_ON) != 0);
}

/**
 * Returns 1 if the Transfer Pak reports a powered cartridge; shuts down on a pak error.
 */
s32 GbPak_IsCartOn(s32 port) {
    u8 status;
    s32 ret = osGbpakGetStatus(&gGbPakPfs[port], &status);

    if ((ret == PFS_ERR_NOPACK) || (ret == PFS_ERR_DEVICE) || (ret == PFS_ERR_CONTRFAIL)) {
        Game_ShutdownAndLoadFragment(port, 1);
    }
    return ((status & OS_GBPAK_GBCART_ON) != 0);
}

/**
 * Re-initializes the pak on a port and enables cartridge RAM; returns 1 on success.
 */
s32 GbTower_ReinitPak(s32 port) {
    UNUSED u8 filler[4];
    u8 status;
    OSGbpakId gbpak_id;

    gGbPakPortReady[port] = 0;
    if ((osGbpakInit(&gSIEventMesgQueue, &gGbPakPfs[port], port) == 0) &&
        (osGbpakReadId(&gGbPakPfs[port], &gbpak_id, &status) == 0) && (status & OS_GBPAK_RSTB_STATUS) &&
        (osGbpakCheckConnector(&gGbPakPfs[port], &status) == 0)) {

        osGbmbcRamEnable(&gGbPakPfs[port]);
        gGbPakPortReady[port] = 1;
    }

    return gGbPakPortReady[port] == 1;
}

/**
 * Returns 1 if the 32-byte pattern left by GbPak_DetectRam is still in
 * cartridge RAM and the pak was not reset, i.e. the same cartridge is still inserted.
 */
s32 GbTower_VerifyPakStillInserted(s32 port, void* pattern) {
    u8 status;
    u8 read_back[0x20];
    s32 result;

    result = 0;
    if ((GbPak_MbcRead(port, (uintptr_t)&read_back, 0, 0x20) == 0) && (bcmp(&read_back, pattern, 0x20) == 0) &&
        (osGbpakGetStatus(&gGbPakPfs[port], &status) == 0) && !(status & OS_GBPAK_RSTB_DETECTION)) {
        result = 1;
    }
    return result;
}

/**
 * Reads cartridge memory through osGbmbcReadWrite if the port is usable.
 */
s32 GbPak_MbcRead(s32 port, void* buffer, u16 address, u16 size) {
    s32 ret;

    ret = 1;
    if (gGbPakUsableMask & (1 << port)) {
        ret = osGbmbcReadWrite(&gGbPakPfs[port], 0, address, buffer, size);
    }
    return ret;
}

/**
 * Writes cartridge memory through osGbmbcReadWrite if the port is usable.
 */
s32 GbPak_MbcWrite(s32 port, void* buffer, u16 address, u16 size) {
    s32 ret;

    ret = 1;
    if (gGbPakUsableMask & (1 << port)) {
        ret = osGbmbcReadWrite(&gGbPakPfs[port], 1, address, (uintptr_t)buffer, size);
    }
    return ret;
}

/**
 * Reads raw Transfer Pak address space through osGbpakReadWrite if the port is usable.
 */
s32 GbPak_RawRead(s32 port, u8* buffer, u16 address, u16 size) {
    s32 ret;

    ret = 1;
    if (gGbPakUsableMask & (1 << port)) {
        ret = osGbpakReadWrite(&gGbPakPfs[port], 0, address, buffer, size);
    }
    return ret;
}

/**
 * Writes raw Transfer Pak address space through osGbpakReadWrite if the port is usable.
 */
s32 GbPak_RawWrite(s32 port, u8* buffer, u16 address, u16 size) {
    s32 ret;

    ret = 1;
    if (gGbPakUsableMask & (1 << port)) {
        ret = osGbpakReadWrite(&gGbPakPfs[port], 1, address, buffer, size);
    }
    return ret;
}

/**
 * Selects a ROM bank by writing 0x5000, the upper bank bits to 0x4000 and the
 * lower 5 bits to 0x2000.
 */
int GbPak_SelectRomBank(s32 port, s32 bank) {
    u8 buffer[0x20];
    int i;
    int ret;

    for (i = 0; i < 0x20; i++) {
        buffer[i] = 0;
    }
    ret = GbPak_RawWrite(port, buffer, 0x5000U, 0x20U);
    if (ret) {
        return ret;
    }
    for (i = 0; i < 0x20; i++) {
        buffer[i] = (bank / 32);
    }
    ret = GbPak_RawWrite(port, buffer, 0x4000U, 0x20U);
    if (ret) {
        return ret;
    }
    for (i = 0; i < 0x20; i++) {
        buffer[i] = (bank % 32);
    }
    return GbPak_RawWrite(port, buffer, 0x2000U, 0x20U);
}

/**
 * Reads a linear range of cartridge ROM, aligned to 32 bytes, switching banks
 * for addresses at or above 0x4000.
 */
s32 GbPak_ReadRom(s32 port, u8* buffer, s32 rom_address, s32 size) {
    s32 ret = 1;
    u32 address = (rom_address & ~0x1F);
    u32 end_address = ((rom_address + size) + 0x1F) & ~0x1F;

    s32 bank_chunk_size;
    s32 chunk_size;
    u32 bank_end;
    u32 chunk_end;

    if (address < 0x4000U) {
        if (end_address < 0x4001U) {
            chunk_end = end_address;
        } else {
            chunk_end = 0x4000;
        }
        chunk_size = chunk_end - address;
        ret = GbPak_RawRead(port, buffer, address, chunk_size);
        if (ret != 0) {
            return ret;
        }
        address += chunk_size;
        buffer = &buffer[chunk_size];
    }
    while (address < end_address) {
        bank_end = (address & ~0x3FFF) + 0x4000;
        if (bank_end >= end_address) {
            chunk_end = end_address;
        } else {
            chunk_end = bank_end;
        }
        bank_chunk_size = chunk_end - address;
        ret = GbPak_SelectRomBank(port, address >> 0xE);
        if (ret != 0) {
            return ret;
        }
        ret = GbPak_RawRead(port, buffer, ((address & 0x3FFF) + 0x4000) & 0xFFFF, bank_chunk_size);
        if (ret != 0) {
            return ret;
        }
        address += bank_chunk_size;
        buffer = &buffer[bank_chunk_size];
    }
    return ret;
}

/**
 * Sets the ports polled by GbTower_PollPakConnection; returns the old mask.
 */
u8 GbTower_SetPollMask(u8 mask) {
    u8 old_mask = gGbPakPollMask;

    gGbPakPollMask = mask;
    return old_mask;
}

#if defined(VERSION_JP)
/**
 * Polls one pak per call, rotating through ports 0-3. A port enabled by
 * GbTower_SetPollMask that fails osGbpakGetStatus 3 times in a row (JP: once) latches
 * gGbPakDisconnected and calls Game_ShutdownAndLoadFragment(port, 1).
 */
void GbTower_PollPakConnection(void) {
    u8 status;
    static s32 sPollPort = 0; // .data

    if (gGbPakDisconnected == 0) {
        if (gGbPakPollMask & (1 << sPollPort)) {
            if (osGbpakGetStatus(&gGbPakPfs[sPollPort], &status) != 0) {
                gGbPakDisconnected = 1;
                Game_ShutdownAndLoadFragment(sPollPort, 1);
            }
        }
        sPollPort = (sPollPort + 1) & 3;
    }
}

/**
 * Checks every pak enabled by GbTower_SetPollMask in one call, using the same
 * failure handling as GbTower_PollPakConnection.
 */
void GbTower_PollAllPaks_jp(void) {
    s32 i;
    u8 status;

    for (i = 0; i < 4; i++) {
        if (gGbPakPollMask & (1 << i)) {
            if (osGbpakGetStatus(&gGbPakPfs[i], &status) != 0) {
                gGbPakDisconnected = 1;
                Game_ShutdownAndLoadFragment(i, 1);
            }
        }
    }
}
#else
/**
 * Polls one pak per call, rotating through ports 0-3. A port enabled by
 * GbTower_SetPollMask that fails osGbpakGetStatus 3 times in a row (JP: once) latches
 * gGbPakDisconnected and calls Game_ShutdownAndLoadFragment(port, 1).
 */
void GbTower_PollPakConnection(void) {
    u8 status;
    static s32 sPollPort = 0; // .data

    if (gGbPakDisconnected == 0) {
        if (gGbPakPollMask & (1 << sPollPort)) {
            if (osGbpakGetStatus(&gGbPakPfs[sPollPort], &status) == 0) {
                gGbPakFailCount[sPollPort] = 0;
            } else {
                gGbPakFailCount[sPollPort]++;
                if (gGbPakFailCount[sPollPort] == 3) {
                    gGbPakDisconnected = 1;
                    Game_ShutdownAndLoadFragment(sPollPort, 1);
                }
            }
        }
        sPollPort = (sPollPort + 1) & 3;
    }
}
#endif
