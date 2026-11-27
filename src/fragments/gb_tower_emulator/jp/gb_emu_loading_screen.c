#include "global.h"
#include "gb_tower_emulator_jp.h"
#include "variables.h"
#include "m2c_macros.h"


extern u16 D_812255C0[];
extern u16 D_81225640[];
extern u16 D_812256C0[];
extern u16 D_81225740[];
extern u16 D_812257C0[];
extern u16 D_81225840[];
extern u16 D_812258C0[];
extern u16 D_81225940[];
/**
 * Draws a 46x46 framed panel (8x8 edge and corner tiles from sUiGraphics) with a
 * solid interior, used behind each party icon on the loading screen.
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment from Stadium 1, matched by code similarity.
 */
void GbEmu_DrawFramedPanel(u16* arg0) {
    s32 i;
    s32 j;

    for (i = 1; i != 5; i++) {
        GbEmu_BlitWithPixelOp(&arg0[i * 8], D_81225640, 0xFFFF, 8, 8);
        GbEmu_BlitWithPixelOp(&arg0[38 * 320 + i * 8], D_812258C0, 0xFFFF, 8, 8);
        GbEmu_BlitWithPixelOp(&arg0[i * 8 * 320], D_81225740, 0xFFFF, 8, 8);
        GbEmu_BlitWithPixelOp(&arg0[i * 8 * 320 + 38], D_812257C0, 0xFFFF, 8, 8);
    }
    GbEmu_BlitWithPixelOp(arg0, D_812255C0, 0, 8, 8);
    GbEmu_BlitWithPixelOp(&arg0[38], D_812256C0, 0, 8, 8);
    GbEmu_BlitWithPixelOp(&arg0[38 * 320], D_81225840, 0, 8, 8);
    GbEmu_BlitWithPixelOp(&arg0[38 * 320 + 38], D_81225940, 0, 8, 8);

    for (j = 0; j < 30; j++) {
        for (i = 0; i < 30; i++) {
            arg0[(j + 8) * 320 + i + 8] = 0xC14;
        }
    }
}

extern u16 D_8122A7B8[];
extern u16 D_8122AE38[];
extern u16 D_81229F38[];
extern u16 D_8122A3B8[];
extern u16 D_8122AA38[];
extern u16 D_8122B238[];
extern u16 D_8122A338[];
extern u8 D_81259A81;

/**
 * Draws frame `frame` of the cable animation between the party panels on the loading screen.
 * Not reached in play (2026-11-14): it did not run in the GB Player with JP Green (boot, intro,
 * menu, button config, speed-up, recovery, quit).
 * @unverified Name from the code and Stadium 1.
 */
void GbEmu_DrawTransferAnimation(s32 addr, s32 frame) {
    s32 i;

    for (i = 0; i < 6; i++) {
        GbEmu_BlitWithPixelOp(addr, (u8*)&D_8122A7B8[frame * 0x28], 0, 8, 5);
        addr += 0x10;
    }
    GbEmu_BlitWithPixelOp(addr, (u8*)&D_8122AE38[frame * 0x40], 0, 8, 8);
    addr += 0x1406;
    for (i = 0; i < 5; i++) {
        GbEmu_BlitWithPixelOp(addr, (u8*)&D_81229F38[frame * 0x40], 0, 8, 8);
        addr += 0x1400;
    }
    GbEmu_BlitWithPixelOp(addr, (u8*)&D_8122A3B8[frame * 0x40], 0, 8, 8);
    addr += 0x10;
    switch (D_81259A81) {
        case 1:
            i = 7;
            break;
        case 2:
            i = 0xA;
            break;
        case 3:
            i = 0xB;
            break;
        default:
            i = 6;
            break;
    }
    for (; i > 0; i--) {
        GbEmu_BlitWithPixelOp(addr + 0x780, (u8*)&D_8122A7B8[frame * 0x28], 0, 8, 5);
        addr += 0x10;
    }
    GbEmu_BlitWithPixelOp(addr, (u8*)&D_8122AA38[frame * 0x40], 0, 8, 8);
    addr -= 0x13FA;
    GbEmu_BlitWithPixelOp(addr, (u8*)&D_8122B238[frame * 0x40], 0, 8, 8);
    addr -= 0x1400;
    GbEmu_BlitWithPixelOp(addr, (u8*)&D_8122B238[frame * 0x40], 0, 8, 8);
    addr -= 0x1400;
    GbEmu_BlitWithPixelOp(addr, (u8*)D_8122A338, 0, 8, 8);
}

extern s32 D_81259A60;
extern s32 D_81259A64;
extern u32 D_81259A68;
extern u32 D_81259A6C;
extern u16* D_81259A70;
extern u16* D_81259A74;
extern u32 D_81259A78;
extern u8 D_81259A80;
extern u8 D_81259A81;
extern u16 D_811228C0[];
extern u16 D_81124CC0[];
extern u16 D_81124D40[];
extern u16 D_81124DC0[];
extern u16 D_811270E8[];
extern u16 D_81127390[];
extern u16 D_81127638[];
extern u16 D_811278E0[];
extern u16 D_81127B88[];
extern u8 D_8112A938[];
void GbEmu_DrawTextCommands(u16*, u8*);
void GbEmu_DrawFramedPanel(u16* arg0);
u32 osGetCount(void);
/**
 * Draws the cartridge loading screen: the background, the six party icons in framed panels, the
 * Game Boy for `port` and the cartridge picture for `version`, then starts the cable animation.
 * Not reached in play (2026-11-14): it did not run in the GB Player with JP Green (boot, intro,
 * menu, button config, speed-up, recovery, quit).
 * @unverified Name from the code and Stadium 1.
 */
void GbEmu_DrawLoadingScreen(u16* dst, u8 (*party_icons)[0xC80], s32 port, s32 version);
#ifdef NON_MATCHING
void GbEmu_DrawLoadingScreen(u16* dst, u8 (*party_icons)[0xC80], s32 port, s32 version) {
    s32 y;
    s32 x;
    u16* row;
    s32 j;
    s32 step;
    s32 i;

    D_81259A60 = (s32)dst;
    step = 0x5000;
    for (y = 0; y < 0x14000; y += step) {
        row = dst + y;
        for (j = 0; j != 320u; j += 64) {
            GbEmu_BlitWithPixelOp(row, D_811228C0, 0xFFFF, 64, (y == 0xF000) ? 48 : 64);
            row += 64;
        }
    }

    D_81259A70 = (u16*)GbEmu_AllocAligned64(0x2580, 0);
    _bcopy(dst + 0x10540, D_81259A70, 0x2580);
    D_81259A74 = (u16*)GbEmu_AllocAligned64(0x2580, 0);
    bzero(D_81259A74, 0x2580);
    GbEmu_DrawTextCommands(D_81259A74, D_8112A938);

    for (i = 0; i < 6; i++) {
        GbEmu_DrawFramedPanel(dst + 0xB916 + i * 46);
        osInvalDCache(party_icons[i], sizeof(party_icons[i]));
        GbEmu_BlitWithPixelOp(dst + 0xBCD9 + i * 46, (u16*)party_icons[i], 0, 40, 40);
    }

    for (j = 0; j < 6; j++) {
        for (i = 0; i < 320; i++) {
            dst[(j + 27) * 320 + i] = 0x25A;
            dst[(j + 121) * 320 + i] = 0x25A;
        }
    }

    for (i = 0; i < 320; i += 8) {
        GbEmu_BlitWithPixelOp(dst + 33 * 320 + i, D_81124CC0, 0xFFFF, 8, 8);
        GbEmu_BlitWithPixelOp(dst + 113 * 320 + i, D_81124D40, 0xFFFF, 8, 8);
    }

    for (j = 0; j < 72; j++) {
        for (i = 0; i < 320; i++) {
            dst[(j + 41) * 320 + i] = 0x2BA4;
        }
    }

    switch (port) {
        case 1:
            D_81259A64 = (s32)((u8*)dst + 2);
            break;
        case 2:
            D_81259A64 = (s32)((u8*)dst + 4);
            break;
        case 3:
            D_81259A64 = (s32)((u8*)dst + 6);
            break;
        default:
            D_81259A64 = (s32)dst;
            break;
    }

    GbEmu_BlitWithPixelOp(dst + 0x4E38, D_81127B88, 0, 0x56, 0x21);
    GbEmu_BlitWithPixelOp((u16*)(D_81259A64 + 0x61E0), D_81124DC0, 0, 0x3C, 0x4B);

    switch (version) {
        case 0:
            GbEmu_BlitWithPixelOp((u16*)(D_81259A64 + 0x7608), D_811270E8, 0, 0x13, 0x11);
            break;
        case 1:
            GbEmu_BlitWithPixelOp((u16*)(D_81259A64 + 0x7608), D_81127390, 0, 0x13, 0x11);
            break;
        case 2:
            GbEmu_BlitWithPixelOp((u16*)(D_81259A64 + 0x7608), D_81127638, 0, 0x13, 0x11);
            break;
        case 3:
            GbEmu_BlitWithPixelOp((u16*)(D_81259A64 + 0x7608), D_811278E0, 0, 0x13, 0x11);
            break;
    }

    D_81259A81 = port;
    GbEmu_DrawTransferAnimation(D_81259A64 + 0x87BA, 0);
    osWritebackDCacheAll();
    D_81259A80 = 0;
    D_81259A78 = 0x800000;
    D_81259A68 = osGetCount();
    D_81259A6C = 0;
}
#else
// clang-format off
#pragma GLOBAL_ASM("asm/jp/nonmatchings/fragments/gb_tower_emulator/jp/gb_emu_loading_screen/GbEmu_DrawLoadingScreen.s")
// clang-format on
#endif

/**
 * Adds a scrolling image layer to the destination with per-channel saturation.
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment from Stadium 1, matched by code similarity.
 */
void GbEmu_BlendScrollingLayer(u16* arg0, u16* arg1, s16* arg2, s32 arg3, s32 arg4) {
    s32 i;
    s32 j;
    s32 var_v1;
    s32 var_t4;

    for (j = 0; j < arg3; j++) {
        for (i = 0; i < 320; i++) {
            var_v1 = (arg0[(j * 320) + ((arg4 + i) % 320)] & 0xF800) + (arg1[(j * 320) + i] & 0xF800);
            if (var_v1 > 0xF800) {
                var_v1 = 0xF800;
            }
            var_t4 = var_v1;
            var_v1 = (arg0[(j * 320) + ((arg4 + i) % 320)] & 0x7C0) + (arg1[(j * 320) + i] & 0x7C0);
            if (var_v1 > 0x7C0) {
                var_v1 = 0x7C0;
            }
            var_t4 |= var_v1;
            var_v1 = (arg0[(j * 320) + ((arg4 + i) % 320)] & 0x3E) + (arg1[(j * 320) + i] & 0x3E);
            if (var_v1 > 0x3E) {
                var_v1 = 0x3E;
            }
            arg2[(j * 320) + i] = var_t4 | var_v1;
        }
    }
}

extern u32 D_81259A7C;
extern u32 D_81259A78;
extern u8 D_81259A80;
extern s32 D_81259A64;
extern u32 D_81259A68;
extern u32 D_81259A6C;
extern s32 D_81259A60;
extern u16* D_81259A74;
extern u16* D_81259A70;
void GbEmu_DrawTransferAnimation(s32 addr, s32 frame);
/**
 * Advances the loading-screen cable animation one step when its timer runs out.
 * Not reached in play (2026-11-14): it did not run in the GB Player with JP Green (boot, intro,
 * menu, button config, speed-up, recovery, quit).
 * @unverified Name from the code and Stadium 1.
 */
void GbEmu_AnimateLoadingScreen(void) {
    if ((osGetCount() - D_81259A7C) > D_81259A78) {
        D_81259A7C = osGetCount();
        D_81259A80 = (D_81259A80 + 1) & 7;
        GbEmu_DrawTransferAnimation(D_81259A64 + 0x87BA, D_81259A80);
        if (D_81259A78 > 0x80000) {
            D_81259A78 -= D_81259A78 >> 8;
        }
    }
    if ((osGetCount() - D_81259A68) >= 0x200000) {
        D_81259A68 = osGetCount();
        D_81259A6C++;
        if (D_81259A6C >= 0x140) {
            D_81259A6C = 0;
        }
        GbEmu_BlendScrollingLayer(D_81259A74, D_81259A70, (s16*)(D_81259A60 + 0x20A80), 0xF, D_81259A6C);
        osWritebackDCacheAll();
    }
}

