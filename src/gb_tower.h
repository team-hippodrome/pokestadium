#ifndef _SRC_GB_TOWER_H_
#define _SRC_GB_TOWER_H_

#include "global.h"

s32 GbPak_VerifyPattern(s32 port, void* pattern);
s32 GbPak_DetectRam(s32 port, u8* pattern);
s32 GbTower_ProbePak(s32 port, u8* wasReady, u8* pattern);
s32 GbTower_IsPakInserted(s32 port, u8 keepPowered);
void GbTower_PowerOffAllPaks(void);
s32 GbPak_IsPokemonCartridge(OSGbpakId* header);
s32 GbTower_ScanPaks(void);
s32 GbPak_IsCartOff(s32 port);
s32 GbPak_IsCartOn(s32 port);
s32 GbTower_ReinitPak(s32 port);
s32 GbTower_VerifyPakStillInserted(s32 port, void* pattern);
s32 GbPak_MbcRead(s32 port, void* buffer, u16 address, u16 size);
s32 GbPak_MbcWrite(s32 port, void* buffer, u16 address, u16 size);
s32 GbPak_RawRead(s32 port, u8* buffer, u16 address, u16 size);
s32 GbPak_RawWrite(s32 port, u8* buffer, u16 address, u16 size);
int GbPak_SelectRomBank(s32 port, s32 bank);
s32 GbPak_ReadRom(s32 port, u8* buffer, s32 romAddress, s32 size);
u8 GbTower_SetPollMask(u8 mask);
void GbTower_PollPakConnection(void);

#endif /* _SRC_GB_TOWER_H_ */
