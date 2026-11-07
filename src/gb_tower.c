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

s32 GbPak_MbcRead(s32 arg0, void* arg1, u16 arg2, u16 arg3);
s32 GbPak_MbcWrite(s32 arg0, void* arg1, u16 arg2, u16 arg3);

// TODO: These 0x20 sizes probably belong to a struct size or something. Use
// that instead of assuming sizeof is the same
/**
 * Writes a 32-byte pattern to cartridge RAM 0xA000 through the MBC, reads it back
 * and returns 1 if it matches and the pak status bit 2 is clear.
 */
s32 GbPak_VerifyPattern(s32 arg0, void* arg1) {
    u8 sp47;
    u8 sp24[0x20];
    s32 sp20 = 0;
    UNUSED u8 padding[8];

    if ((GbPak_MbcWrite(arg0, arg1, 0, 0x20) == 0) && (GbPak_MbcRead(arg0, (uintptr_t)&sp24, 0, 0x20) == 0) &&
        (bcmp(&sp24, arg1, 0x20) == 0) && (osGbpakGetStatus(&gGbPakPfs[arg0], &sp47) == 0)) {
        sp20 = ((sp47 & 4) != 0) == 0;
    }
    return sp20;
}

/**
 * Detects working cartridge RAM by verifying 0x55, 0xAA and then an osGetCount
 * pattern; the last pattern is left in arg1 for GbTower_VerifyPakStillInserted.
 */
s32 GbPak_DetectRam(s32 arg0, u8* arg1) {
    s32 var_s0 = 0;
    u8 sp34[0x20];

    if (arg1 == NULL) {
        arg1 = sp34;
    }
    HAL_Memset((char*)arg1, 0x55, 0x20);
    if (GbPak_VerifyPattern(arg0, arg1) != 0) {
        HAL_Memset((char*)arg1, 0xAA, 0x20);
        if (GbPak_VerifyPattern(arg0, arg1) != 0) {
            s32 i;
            for (i = 0; i < ARRAY_COUNT(sp34); i++) {
                arg1[i] = osGetCount();
            }
            var_s0 = GbPak_VerifyPattern(arg0, arg1);
        }
    }
    return var_s0;
}

/**
 * Prepares the pak on a port once (gGbPakPortReady): reads the ID, enables
 * cartridge RAM and runs GbPak_DetectRam. Shuts down with error 1 if the read fails.
 */
s32 GbTower_ProbePak(s32 arg0, u8* arg1, u8* arg2) {
    u8 status;
    OSGbpakId gbpakId;
    s32 sp28 = 0;

    if (arg1 != NULL) {
        *arg1 = gGbPakPortReady[arg0];
    }

    if (gGbPakPortReady[arg0] == 1) {
        return 1;
    }

    if (osGbpakReadId(&gGbPakPfs[arg0], &gbpakId, &status) != 0) {
        Game_ShutdownAndLoadFragment(arg0, 1);
    }
    if ((status & OS_GBPAK_RSTB_STATUS) && (osGbpakCheckConnector(&gGbPakPfs[arg0], &status) == 0)) {
        osGbmbcRamEnable(&gGbPakPfs[arg0]);
        sp28 = GbPak_DetectRam(arg0, arg2);
    }
    gGbPakPortReady[arg0] = 1;
    return sp28;
}

/**
 * Returns 1 if the pak reports a reset/removal; powers the pak off when arg1 == 0.
 */
s32 GbTower_CheckPakRemoved(s32 arg0, u8 arg1) {
    u8 status;
    s32 temp_v1;

    if (osGbpakGetStatus(&gGbPakPfs[arg0], &status) != 0) {
        Game_ShutdownAndLoadFragment(arg0, 1);
    }

    temp_v1 = !((status & OS_GBPAK_RSTB_DETECTION) != 0);
    if (arg1 == 0) {
        osGbpakPower(&gGbPakPfs[arg0], 0);
        gGbPakPortReady[arg0] = 0;
    }
    return temp_v1;
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
    s32 temp_v0;
    u8 status;
    OSGbpakId sp4C;

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
                temp_v0 = osGbpakReadId(&gGbPakPfs[i], &sp4C, &status);
                if (temp_v0 == 0) {
                    if (!(status & 8)) {
                        Game_ShutdownAndLoadFragment(i, 2);
                    }
                    if (GbPak_IsPokemonCartridge(&sp4C) != 0) {
#if !defined(VERSION_JP)
                        if (temp_v0 == 4) {
                            // what. the redundant temp_v0 functionless check is apparently needed to match.
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
                    if (temp_v0 == 4) {
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
s32 GbPak_IsCartOff(s32 arg0) {
    u8 status;
    s32 ret = osGbpakGetStatus(&gGbPakPfs[arg0], &status);
    UNUSED u8 filler;

    // check the error code returned (if applicable) by the osGbpakGetStatus
    // call.
    if ((ret == PFS_ERR_NOPACK) || (ret == PFS_ERR_DEVICE) || (ret == PFS_ERR_CONTRFAIL)) {
        Game_ShutdownAndLoadFragment(arg0, 1);
    }
    return !((status & OS_GBPAK_GBCART_ON) != 0);
}

/**
 * Returns 1 if the Transfer Pak reports a powered cartridge; shuts down on a pak error.
 */
s32 GbPak_IsCartOn(s32 arg0) {
    u8 status;
    s32 ret = osGbpakGetStatus(&gGbPakPfs[arg0], &status);

    if ((ret == PFS_ERR_NOPACK) || (ret == PFS_ERR_DEVICE) || (ret == PFS_ERR_CONTRFAIL)) {
        Game_ShutdownAndLoadFragment(arg0, 1);
    }
    return ((status & OS_GBPAK_GBCART_ON) != 0);
}

/**
 * Re-initializes the pak on a port and enables cartridge RAM; returns 1 on success.
 */
s32 GbTower_ReinitPak(s32 arg0) {
    UNUSED u8 filler[4];
    u8 status;
    OSGbpakId sp28;

    gGbPakPortReady[arg0] = 0;
    if ((osGbpakInit(&gSIEventMesgQueue, &gGbPakPfs[arg0], arg0) == 0) &&
        (osGbpakReadId(&gGbPakPfs[arg0], &sp28, &status) == 0) && (status & OS_GBPAK_RSTB_STATUS) &&
        (osGbpakCheckConnector(&gGbPakPfs[arg0], &status) == 0)) {

        osGbmbcRamEnable(&gGbPakPfs[arg0]);
        gGbPakPortReady[arg0] = 1;
    }

    return gGbPakPortReady[arg0] == 1;
}

/**
 * Returns 1 if the 32-byte pattern left by GbPak_DetectRam is still in
 * cartridge RAM and the pak was not reset, i.e. the same cartridge is still inserted.
 */
s32 GbTower_VerifyPakStillInserted(s32 arg0, void* arg1) {
    u8 status;
    u8 sp1C[0x20];
    s32 sp18;

    sp18 = 0;
    if ((GbPak_MbcRead(arg0, (uintptr_t)&sp1C, 0, 0x20) == 0) && (bcmp(&sp1C, arg1, 0x20) == 0) &&
        (osGbpakGetStatus(&gGbPakPfs[arg0], &status) == 0) && !(status & OS_GBPAK_RSTB_DETECTION)) {
        sp18 = 1;
    }
    return sp18;
}

/**
 * Reads cartridge memory through osGbmbcReadWrite if the port is usable.
 */
s32 GbPak_MbcRead(s32 arg0, void* arg1, u16 arg2, u16 arg3) {
    s32 var_v1;

    var_v1 = 1;
    if (gGbPakUsableMask & (1 << arg0)) {
        var_v1 = osGbmbcReadWrite(&gGbPakPfs[arg0], 0, arg2, arg1, arg3);
    }
    return var_v1;
}

/**
 * Writes cartridge memory through osGbmbcReadWrite if the port is usable.
 */
s32 GbPak_MbcWrite(s32 arg0, void* arg1, u16 arg2, u16 arg3) {
    s32 var_v1;

    var_v1 = 1;
    if (gGbPakUsableMask & (1 << arg0)) {
        var_v1 = osGbmbcReadWrite(&gGbPakPfs[arg0], 1, arg2, (uintptr_t)arg1, arg3);
    }
    return var_v1;
}

/**
 * Reads raw Transfer Pak address space through osGbpakReadWrite if the port is usable.
 */
s32 GbPak_RawRead(s32 arg0, u8* arg1, u16 arg2, u16 arg3) {
    s32 var_v1;

    var_v1 = 1;
    if (gGbPakUsableMask & (1 << arg0)) {
        var_v1 = osGbpakReadWrite(&gGbPakPfs[arg0], 0, arg2, arg1, arg3);
    }
    return var_v1;
}

/**
 * Writes raw Transfer Pak address space through osGbpakReadWrite if the port is usable.
 */
s32 GbPak_RawWrite(s32 arg0, u8* arg1, u16 arg2, u16 arg3) {
    s32 var_v1;

    var_v1 = 1;
    if (gGbPakUsableMask & (1 << arg0)) {
        var_v1 = osGbpakReadWrite(&gGbPakPfs[arg0], 1, arg2, arg1, arg3);
    }
    return var_v1;
}

/**
 * Selects a ROM bank by writing 0x5000, the upper bank bits to 0x4000 and the
 * lower 5 bits to 0x2000.
 */
int GbPak_SelectRomBank(s32 arg0, s32 arg1) {
    u8 sp28[0x20];
    int i;
    int ret;

    for (i = 0; i < 0x20; i++) {
        sp28[i] = 0;
    }
    ret = GbPak_RawWrite(arg0, sp28, 0x5000U, 0x20U);
    if (ret) {
        return ret;
    }
    for (i = 0; i < 0x20; i++) {
        sp28[i] = (arg1 / 32);
    }
    ret = GbPak_RawWrite(arg0, sp28, 0x4000U, 0x20U);
    if (ret) {
        return ret;
    }
    for (i = 0; i < 0x20; i++) {
        sp28[i] = (arg1 % 32);
    }
    return GbPak_RawWrite(arg0, sp28, 0x2000U, 0x20U);
}

/**
 * Reads a linear range of cartridge ROM, aligned to 32 bytes, switching banks
 * for addresses at or above 0x4000.
 */
s32 GbPak_ReadRom(s32 arg0, u8* arg1, s32 arg2, s32 arg3) {
    s32 ret = 1;
    u32 temp_s1 = (arg2 & ~0x1F);
    u32 temp_s4 = ((arg2 + arg3) + 0x1F) & ~0x1F;

    s32 temp_s1_2;
    s32 temp_v0_4;
    u32 temp_v0_5;
    u32 var_v1_2;

    if (temp_s1 < 0x4000U) {
        if (temp_s4 < 0x4001U) {
            var_v1_2 = temp_s4;
        } else {
            var_v1_2 = 0x4000;
        }
        temp_v0_4 = var_v1_2 - temp_s1;
        ret = GbPak_RawRead(arg0, arg1, temp_s1, temp_v0_4);
        if (ret != 0) {
            return ret;
        }
        temp_s1 += temp_v0_4;
        arg1 = &arg1[temp_v0_4];
    }
    while (temp_s1 < temp_s4) {
        temp_v0_5 = (temp_s1 & ~0x3FFF) + 0x4000;
        if (temp_v0_5 >= temp_s4) {
            var_v1_2 = temp_s4;
        } else {
            var_v1_2 = temp_v0_5;
        }
        temp_s1_2 = var_v1_2 - temp_s1;
        ret = GbPak_SelectRomBank(arg0, temp_s1 >> 0xE);
        if (ret != 0) {
            return ret;
        }
        ret = GbPak_RawRead(arg0, arg1, ((temp_s1 & 0x3FFF) + 0x4000) & 0xFFFF, temp_s1_2);
        if (ret != 0) {
            return ret;
        }
        temp_s1 += temp_s1_2;
        arg1 = &arg1[temp_s1_2];
    }
    return ret;
}

/**
 * Sets the ports polled by GbTower_PollPakConnection; returns the old mask.
 */
u8 GbTower_SetPollMask(u8 arg0) {
    u8 temp_v0 = gGbPakPollMask;

    gGbPakPollMask = arg0;
    return temp_v0;
}

#if defined(VERSION_JP)
/**
 * Polls one pak per call, rotating through ports 0-3. A port enabled by
 * GbTower_SetPollMask that fails osGbpakGetStatus 3 times in a row (JP: once) latches
 * gGbPakDisconnected and calls Game_ShutdownAndLoadFragment(port, 1).
 */
void GbTower_PollPakConnection(void) {
    u8 sp1F;
    static s32 sPollPort = 0; // .data

    if (gGbPakDisconnected == 0) {
        if (gGbPakPollMask & (1 << sPollPort)) {
            if (osGbpakGetStatus(&gGbPakPfs[sPollPort], &sp1F) != 0) {
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
    u8 sp1F;
    static s32 sPollPort = 0; // .data

    if (gGbPakDisconnected == 0) {
        if (gGbPakPollMask & (1 << sPollPort)) {
            if (osGbpakGetStatus(&gGbPakPfs[sPollPort], &sp1F) == 0) {
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
