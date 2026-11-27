/* JP-only TU, ported from pokestadium0 (Stadium Zero, fragment000_80C00). */

#include "global.h"
#include "gb_tower_emulator_jp.h"
#include "src/memory.h"
#include "src/gfx_rect.h"
#include "src/jpeg_stream.h"
#include "src/game_state.h"

void GbAudio_SetMenuSoundMode(s32);
void GbEmu_PauseEmulation(void);
void GbEmu_Delay(OSTime);
void osViSetXScale(f32);
void crash_screen_set_draw_info(s32, s32, s32);
extern s8* sRenderTaskPending[];
extern s32 sGbEmuExitCode;
/**
 * Stops the menu sounds, pauses emulation, blanks the display, waits 20 ms, resets the X scale,
 * shows the current framebuffer and clears it to black.
 * Seen in play in pokestadium0 (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in
 * the Transfer Pak; called from GbEmu_Init.
 */
void GbEmu_PauseAndBlankScreen(void) {
    GbAudio_SetMenuSoundMode(0);
    GbEmu_PauseEmulation();
    osViBlack(1);
    GbEmu_Delay(20);
    osViSetXScale(1.0f);
    osViSwapBuffer(*(s32*)(sRenderTaskPending[sGbEmuExitCode] + 8));
    crash_screen_set_draw_info(*(s32*)(sRenderTaskPending[sGbEmuExitCode] + 8), 0x140, 0x10);
    bzero(*(s32*)(sRenderTaskPending[sGbEmuExitCode] + 8), 0x25800);
}

extern u8 D_8121E918[];
extern s32 D_8121CAF0;
extern u8 D_8121E924[];
extern u8 D_8121E934[];
extern u8 D_8121E964[];
extern u8 D_8121E974[];
extern u8 D_8121E9A8[];
extern u8 D_8121E9CC[];
extern u8 D_8121E9E0[];
void GbEmu_DrawTextCommands(s8* arg0, u8* arg1);
s32 GbEmu_BlitWithPixelOp(s32, s32*, s32, s32, s32);
extern s32 D_8121CB68_jp;
/**
 * Draws the GB Player menu (メニュー, "Menu") over the GB screen: ゲームをつづける ("Continue game"),
 * ゲームをやめる ("Quit game") and せつめい・コンフィグ ("Help/Config") with the pointer on item `cursor`
 * and that item's help text; `cursor` 5 draws the menu behind the ゲームをやめる prompt.
 * Seen in play in pokestadium0 (2026-11-14) ran when C-Up opened the GB Player menu; called from GbEmu_RunFrame.
 */
void GbEmu_DrawPauseMenu(s8* fb, u8 cursor) {
    s32 i;
    u8* last;

    fb += 0x787C;
    GbEmu_DrawTextCommands(fb, D_8121E918);
    for (i = 0; i != 0x140; i += 0x20) {
        GbEmu_BlitWithPixelOp((s32)(fb + 0xC806 + i), &D_8121CAF0, 0, 0xC, 5);
    }
    GbEmu_BlitWithPixelOp((s32)(fb + (cursor & 3) * 0x2D00 + 0x4618), &D_8121CB68_jp, 0, 0x10, 0xA);
    D_8121E924[4] = 0xA;
    D_8121E964[4] = 0xA;
    D_8121E9CC[4] = 0xA;
    if (cursor == 0) {
        D_8121E924[4] = 8;
        last = D_8121E934;
    } else if (cursor == 1) {
        D_8121E964[4] = 8;
        last = D_8121E974;
    } else if (cursor == 2) {
        D_8121E9CC[4] = 8;
        last = D_8121E9E0;
    } else if (cursor == 5) {
        D_8121E964[4] = 8;
        last = D_8121E9A8;
    }
    GbEmu_DrawTextCommands(fb, D_8121E924);
    GbEmu_DrawTextCommands(fb, D_8121E964);
    GbEmu_DrawTextCommands(fb, D_8121E9CC);
    GbEmu_DrawTextCommands(fb, last);
}

extern s32 D_8121D6A8_jp[];
extern s32 D_8121D828_jp[];
extern s32 D_8121DB28_jp[];
extern s32 D_8121DCA8_jp[];
extern s32 D_8121DE28_jp[];

/**
 * Returns the icon of N64 button choice `index` for the START/SELECT bindings: 0 START, 1 L, 2 R,
 * 3 C-Left, 4 C-Down.
 * Seen in play in pokestadium0 (2026-11-14) ran on the せつめい・コンフィグ ("Help/Config") screen while the START
 * binding was changed; called from GbEmu_DrawButtonConfig.
 */
s32* GbEmu_GetButtonIcon(s32 index) {
    index &= 7;
    return (index == 1) ? D_8121DB28_jp : (index == 2) ? D_8121DCA8_jp : (index == 3) ? D_8121D828_jp : (index == 4) ? D_8121D6A8_jp : D_8121DE28_jp;
}

s32* GbEmu_GetButtonIcon(s32 index);
s32 GbEmu_BlitWithPixelOp(s32, s32*, s32, s32, s32);
extern s32 D_8121E5A8;

/*
 * Draws the row of five N64 button icons on the せつめい・コンフィグ ("Help/Config") screen with the frame on choice
 * `cursor & 7`. GbEmu_DrawButtonChoices
 *
 * arg0 is treated as a plain address and offsets are added to it in bytes.
 * Calls GbEmu_BlitWithPixelOp once with (arg0 + (arg1 & 7) * 0x24, &D_8121E5A8, 0,
 * 0x18, 0x12), then five more times with (arg0 + 0x786 + n * 0x24,
 * GbEmu_GetButtonIcon(n), 0, 0x10, 0xC) for n = 0..4. What GbEmu_BlitWithPixelOp and
 * GbEmu_GetButtonIcon do, and what arg0 points to, is not established from this
 * function.
 *
 * The empty `if (i) {}` is a matching-only construct (found with
 * decomp-permuter): the extra use of `i` makes IDO keep `arg1 & 7` in s0 and
 * reuse s0 as the loop counter, as the retail code does. Without it the mask is
 * computed in a temporary and the output differs.
 * Seen in play in pokestadium0 (2026-11-14) ran on the せつめい・コンフィグ ("Help/Config") screen while the START
 * binding was changed; called from GbEmu_DrawButtonConfig.
 */
void GbEmu_DrawButtonChoices(s32 fb, s32 cursor) {
    s32 i;
    s32 p;

    i = cursor & 7;
    GbEmu_BlitWithPixelOp(fb + i * 0x24, &D_8121E5A8, 0, 0x18, 0x12);
    if (i) {  // karodo
    }
    p = fb + 0x786;
    for (i = 0; i != 5; i++) {
        GbEmu_BlitWithPixelOp(p, GbEmu_GetButtonIcon(i), 0, 0x10, 0xC);
        p += 0x24;
    }
}

extern u8 D_8121E908;
extern u8 D_8121E914[2];
extern s32 D_8121D9A8_jp;
extern s32 D_8121D3A8_jp;
extern s32 D_8121CCA8_jp;
extern s32 D_8121CD28_jp;
extern s32 D_8121CDA8_jp;
extern s32 D_8121CE28_jp;
extern s32 D_8121CEA8_jp;
extern s32 D_8121CF28_jp;
extern s32 D_8121D028_jp;
extern s32 D_8121CFA8_jp;
extern s32 D_8121E1E8;
extern s32 D_8121E428;
extern s32 D_8121DFA8;
extern u8 D_8121EA0C[];
extern u8 D_8121EA54[];
extern u8 D_8121EA7C[];
extern u8 D_8121EAA0[];
extern u8 D_8121EAAC[];
/*
 * Draws the せつめい・コンフィグ ("Help/Config") screen into `screen`: the START and SELECT bindings, the B
 * ボタンでもどる ("B Button: Back") hint and, while `slide` animates the row open, the button choices for the binding
 * picked by bit 3 of `cursor`. GB Player option screen. Draws the panel frame tiles into the framebuffer (three loops),
 * then the two cursors and the selected options. Register allocation depends on reusing the same few locals: one IDO
 * live range per local, so i, j, q and r each carry several roles on purpose. Seen in play in pokestadium0 (2026-11-14)
 * ran on the せつめい・コンフィグ ("Help/Config") screen while the START binding was changed; called from
 * GbEmu_RunFrame.
 */
void GbEmu_DrawButtonConfig(s32 screen, s32 cursor, s32 slide) {
    s32 fb;
    s32 q;
    s32 r;
    s32 i;
    s32 j;

    screen += 0x787C;
    fb = screen;
    if (D_8121E908 == 0) {
        fb = screen - 0x4B00;
    } else {
        GbEmu_BlitWithPixelOp(screen + 0xF22, &D_8121D9A8_jp, 0, 0x10, 0xC);
        GbEmu_DrawTextCommands((s8*)screen, D_8121EA0C);
        if (D_8121E908 == 1) {
            GbEmu_DrawTextCommands((s8*)screen, D_8121EA54);
        } else {
            GbEmu_DrawTextCommands((s8*)screen, D_8121EA7C);
        }
    }
    GbEmu_DrawTextCommands((s8*)fb, D_8121EAA0);
    GbEmu_DrawTextCommands((s8*)fb, D_8121EAAC);
    GbEmu_BlitWithPixelOp(fb + 0x138AE, &D_8121D3A8_jp, 0, 0x10, 0xC);
    fb += 0xBB80;
    GbEmu_BlitWithPixelOp(fb, &D_8121CCA8_jp, 0, 8, 8);
    for (i = 0; i < 18; i++) {
        r = fb; // FAKE
        if (i == 1) {
            i = 8;
        }
        j = i * 0x10 + r + 0x10;
        r = 0; // FAKE
        GbEmu_BlitWithPixelOp(j, &D_8121CD28_jp, 0, 8, 8);
        j += 0x10;
    }
    GbEmu_BlitWithPixelOp(j, &D_8121CDA8_jp, 0, 8, 8);
    for (j = 0, q = fb + 0x1400, r = fb + 0x1530; j < 0x7800; j += 0x1400, q += 0x1400, r += 0x1400) {
        GbEmu_BlitWithPixelOp(q, &D_8121CE28_jp, 0, 8, 8);
        GbEmu_BlitWithPixelOp(r, &D_8121CEA8_jp, 0, 8, 8);
    }
    GbEmu_BlitWithPixelOp(q, &D_8121CF28_jp, 0, 8, 8);
    GbEmu_BlitWithPixelOp(r, &D_8121D028_jp, 0, 8, 8);
    for (j = 0, q = fb + 0x8C10; j != 0x120; j += 0x10, q += 0x10) {
        if (j && j && j) {} // FAKE
        GbEmu_BlitWithPixelOp(q, &D_8121CFA8_jp, 0, 8, 8);
    }
    fb -= 0xBB80;
    q = 0x2D;
    r = 0x2B;
    if (cursor & 8) {
        if (slide != 0) {
            r = slide * 6 + 0x2B;
            j = 1;
            if (slide == -6) {
                GbEmu_DrawButtonChoices(fb + 0xFF76, cursor);
            }
        } else {
            GbEmu_BlitWithPixelOp(fb + 0x10BA8, &D_8121CB68_jp, 0, 0x10, 0xA);
            j = 3;
        }
    } else if (slide != 0) {
        q = slide * 6 + 0x2D;
        j = 2;
        if (slide == -6) {
            GbEmu_DrawButtonChoices(fb + 0xCAF6, cursor);
        }
    } else {
        GbEmu_BlitWithPixelOp(fb + 0xD728, &D_8121CB68_jp, 0, 0x10, 0xA);
        j = 3;
    }
    i = fb + q * 2;
    q = 0; // FAKE
    GbEmu_BlitWithPixelOp(i + 0xD200, &D_8121E1E8, 0, 0x18, 0xC);
    GbEmu_BlitWithPixelOp(i + 0xD236, &D_8121E428, 0, 0x10, 0xC);
    i = fb + r * 2;
    GbEmu_BlitWithPixelOp(i + 0x10680, &D_8121DFA8, 0, 0x18, 0xC);
    GbEmu_BlitWithPixelOp(i + 0x106BA, &D_8121E428, 0, 0x10, 0xC);
    if (j & 1) {
        GbEmu_BlitWithPixelOp(fb + 0xD2C4, GbEmu_GetButtonIcon(D_8121E914[0]), 0, 0x10, 0xC);
    }
    if (j & 2) {
        GbEmu_BlitWithPixelOp(fb + 0x10744, GbEmu_GetButtonIcon(D_8121E914[1]), 0, 0x10, 0xC);
    }
}

void GbEmu_DrawPauseMenu(s8* fb, u8 cursor);
void GbEmu_DrawTextCommands(s8* arg0, u8* arg1);
s32 GbEmu_BlitWithPixelOp(s32, s32*, s32, s32, s32);
extern u8 D_8121EABC[];
extern s32 D_8121CB68_jp;

/**
 * Draws the ゲームをやめる prompt (「レポート」した? ホントに おわってもいい?, "Did you save? Really quit?") with the
 * pointer on はい
 * ("Yes", `no` = 0) or いいえ ("No").
 * Seen in play in pokestadium0 (2026-11-14) ran when ゲームをやめる ("Quit game") showed its 「レポート」した? ("Did
 * you save?") prompt; called from GbEmu_RunFrame.
 */
void GbEmu_DrawQuitConfirm(s8* fb, s32 no) {
    GbEmu_DrawPauseMenu(fb, 5);
    fb += 0x787C;
    D_8121EABC[no ? 4 : 0xC] = 8;
    D_8121EABC[no ? 0xC : 4] = 0xA;
    GbEmu_DrawTextCommands(fb, D_8121EABC);
    GbEmu_BlitWithPixelOp((s32)&((u16*)(fb + 0x13B00))[no ? 0xE : 0x45], &D_8121CB68_jp, 0, 0x10, 0xA);
}

typedef struct GbPlayerTask {
    /* 0x00 */ OSMesg mesg[1];
    /* 0x04 */ OSMesgQueue queue;
    /* 0x1C */ u8 unk_1C[4];
    /* 0x20 */ u8 task[0x14];
    /* 0x34 */ u8 unk_34[8];
    /* 0x3C */ u8 unk_3C[4];
    /* 0x40 */ u8 unk_40[8];
    /* 0x48 */ s32 unk_48;
    /* 0x4C */ u8 unk_4C[0x1C];
} GbPlayerTask; // size = 0x68

extern u8 fragment1_misc_yay0_ROM_START[];
extern u8 D_00129D00_jp[];
extern u8 D_8121E908;
extern s16 D_8121E90C;
extern s16 D_8121E910;
extern u8 D_8121E914[2];
extern u8 D_8122B660[];
extern GbPlayerTask sGbPlayerTasks[2];
extern u8* sGbEmuHeapStart;
extern s32 gGbPakServiceThread;
extern s32 D_812580E4;
extern s32 D_812580E8;
extern u8 D_812580EC;
extern u8 D_812580ED;
extern u8 D_812580EE;
extern u8 D_812580EF;
extern u8 D_81259A90[];
void GbPak_StartIoThread(s32, s32, OSMesgQueue*);
s32 GbEmu_LoadCartridge(s32, u8*, s32, s32, s8*);
void AudioLoad_Init(void);
void GbApu_Reset(u8*);
void GbApu_ResetChannels(void);

/**
 * GB Player entry: allocates both framebuffers and the emulator heap, decompresses the emulator
 * data, inits the sound driver, the GB APU and the video mode, starts the Transfer Pak I/O thread
 * and loads the cartridge, then reads the speed-up level and the START/SELECT bindings from the
 * Stadium save.
 */
void GbEmu_Init(s8* params) {
    sRenderTaskPending[0] = (s8*)GfxImage_Allocate(0, 2, 0x140, 0xF0, 1);
    sRenderTaskPending[1] = (s8*)GfxImage_Allocate(0, 2, 0x140, 0xF0, 1);
    sGbEmuHeapStart = main_pool_alloc(0x1D8000, 0);
    if (sGbEmuHeapStart == NULL) {
        D_812580EE = 1;
        D_812580E8 = 3;
        return;
    }
    sGbEmuExitCode = 0;
    AudioLoad_Init();
    Dma_WriteChunks((u32)sGbEmuHeapStart + 0x80000, (u32)fragment1_misc_yay0_ROM_START,
                    ((u32)D_00129D00_jp + 1) & ~1, 0);
    Yay0_Decompress(sGbEmuHeapStart + 0x80000, sGbEmuHeapStart);
    GbApu_Reset(sGbEmuHeapStart);
    sGbEmuHeapStart += 0x80000;
    GbApu_ResetChannels();
    osCreateMesgQueue(&sGbPlayerTasks[0].queue, sGbPlayerTasks[0].mesg, 1);
    osCreateMesgQueue(&sGbPlayerTasks[1].queue, sGbPlayerTasks[1].mesg, 1);
    osViSetMode(&osViModeNtscLpn1);
    osViSetSpecialFeatures(0x2A);
    osViSetSpecialFeatures(0x80);
    GbEmu_PauseAndBlankScreen();
    Yay0_Decompress(D_8122B660, D_81259A90);
    GbPak_StartIoThread(0xC, 0x12, &gSIEventMesgQueue);
    D_8121E908 = *(u8*)(params + 0x2206);
    if (D_8121E908 >= 3) {
        D_8121E908 = 0;
    }
    D_812580EF = D_8121E908;
    switch (GbEmu_LoadCartridge(*(s32*)(sRenderTaskPending[sGbEmuExitCode] + 8), sGbEmuHeapStart, 0x158000,
                                GbSave_GetActivePort(), params)) {
        case 1:
        default:
            D_812580EE = 1;
            D_812580E8 = 1;
            break;
        case 2:
            D_812580EE = 1;
            D_812580E8 = 2;
            break;
        case 0:
            D_812580EE = 0;
            D_812580E8 = 0;
            break;
    }
    D_812580EC = 0;
    D_812580ED = 0;
    gGbPakServiceThread = 0;
    D_812580E4 = 0;
    D_8121E914[0] = ((*(s16*)(params + 0x2204)) & 0xFF00) >> 8;
    D_8121E914[1] = ((*(s16*)(params + 0x2204)) & 0xFF) + 1;
    if ((D_8121E914[0] >= 5) || (D_8121E914[1] >= 5) || (D_8121E914[0] == D_8121E914[1])) {
        D_8121E914[0] = 0;
        D_8121E914[1] = 1;
    }
    D_8121E90C = (D_8121E914[0] == 1)   ? 0x20
                 : (D_8121E914[0] == 2) ? 0x10
                 : (D_8121E914[0] == 3) ? 2
                 : (D_8121E914[0] == 4) ? 4
                                        : 0x1000;
    D_8121E910 = (D_8121E914[1] == 0)   ? 0x1000
                 : (D_8121E914[1] == 2) ? 0x10
                 : (D_8121E914[1] == 3) ? 2
                 : (D_8121E914[1] == 4) ? 4
                                        : 0x20;
}

extern s16 D_800AD928_jp;
extern u8* D_800603C0;
extern u8 D_81154BF0[];
extern u8 D_81259A40;
extern u8 D_811565A1;
extern u8 D_8111E4D0[];
extern u8 D_8111E4E4[];
extern u8 D_8111E540[];
s32 GbEmu_UpdateFrame(void);
void GbAudio_FrameStub(void);
void GbAudio_RampToSilence(void);
s32 GbPak_HasUnsavedSramBlocks(void);
void GbEmu_BlackScreenAndFail(s32);
void func_80004F34(void*, s32);
void GbEmu_ClearStatusBar(void);
void func_811177DC(s32);
void Cont_StartReadInputs(void);
void Cont_ReadInputs(void);
s32 GbPak_CheckConnectionBlocking(void);
void GbEmu_DrawStatusBar(void);
void GbEmu_ClearPalettes(void);
void GbPak_PowerOff(void);
s32 GbPak_IsCartridgeInserted(void);
s32 GbPak_ReidentifyCartridge(void);
s32 GbPak_VerifyRamBlocks(u8);
void GbPak_StopIoThread(s32);

/**
 * GB Player per-frame state machine: runs the emulator, fades in, and drives the menu (4), the
 * button config (7-10), the quit prompt (11), the 「レポートできませんでした」 ("Couldn't save") cartridge recovery
 * (20-26) and the shutdown, which writes the bindings back to the save. Seen in play in pokestadium0 (2026-11-14) ran
 * when the GB Player started (main menu GB, はい "Yes") with JP Green in the Transfer Pak; called from
 * GbPlayer_EmulatorThreadEntry.
 */
void GbEmu_RunFrame(s8* params);
#ifdef NON_MATCHING
// TODO: this is still the pokestadium0 version, with JP symbol names. The JP function differs
// from it (GbEmu_Init, for one, reads the save fields 4 bytes later) and has not been compared
// against this C yet.
void GbEmu_RunFrame(s8* params) {
    s32 sel;
    u8* fade;
    s32 keep;
    GbPlayerTask* task;
    static u8 D_812580F0;
    static u8 D_812580F1;
    static u8 D_812580F2;
    static u8 D_812580F3;
    static s32 D_812580F4;

    if (D_800AD928_jp != 0) {
        if (D_812580ED != 6) {
            D_812580ED = 6;
            D_812580F0 = 20;
        }
        goto fade_out;
    }
    if (D_812580EE != 0) {
        goto shutdown;
    }
    if (gGbPakServiceThread == 0) {
        gGbPakServiceThread = GbEmu_UpdateFrame();
    }
    GbAudio_FrameStub();
    _bcopy(D_81154BF0, sGbPlayerTasks[sGbEmuExitCode].task, 0x40);
    sGbPlayerTasks[sGbEmuExitCode].unk_48 = *(s32*)(sRenderTaskPending[sGbEmuExitCode ^ 1] + 8);
    if (D_812580EC != 0) {
        osRecvMesg(&sGbPlayerTasks[sGbEmuExitCode ^ 1].queue, 0, 1);
    }
    if ((gGbPakServiceThread == -1) || (gGbPakServiceThread == -2)) {
        if ((D_812580ED < 20) && (GbPak_HasUnsavedSramBlocks() != 0)) {
            gGbPakServiceThread = 1;
            D_812580ED = 20;
        } else {
        error:
            GbEmu_BlackScreenAndFail(1);
            goto shutdown;
        }
    } else if (gGbPakServiceThread == -3) {
        GbEmu_BlackScreenAndFail(2);
        goto shutdown;
    } else if (gGbPakServiceThread == -100) {
        GbEmu_BlackScreenAndFail(3);
        goto shutdown;
    }

    fade = sGbPlayerTasks[sGbEmuExitCode].unk_3C;
    switch (D_812580ED) {
        case 0:
            fade[3] = 0xFF;
            D_812580F0 = 30;
            D_812580ED = 1;
            break;

        case 1:
            if (--D_812580F0 != 0) {
                fade[3] = (s32)((D_812580F0 / 30.0f) * 63.0f) + 0xC0;
            } else {
                fade[3] = 0;
                D_812580ED = 2;
            }
            break;

        case 2:
            if (gGbPakServiceThread == 1) {
                GbEmu_ClearStatusBar();
                D_812580ED = 3;
                D_812580F0 = 15;
                GbAudio_SetMenuSoundMode(1);
                func_811177DC(1);
            }
            break;

        case 3:
            Cont_StartReadInputs();
            Cont_ReadInputs();
            if (--D_812580F0 != 0) {
                if (GbPak_CheckConnectionBlocking() != 0) {
                    goto error;
                }
                fade[3] = 0xBF - (s32)((D_812580F0 / 15.0f) * 63.0f);
            } else {
                fade[3] = 0xBF;
                if (GbPak_CheckConnectionBlocking() != 0) {
                    goto error;
                }
                D_812580ED = 4;
                D_812580F1 = 0;
            }
            break;

        case 4:
            fade[3] = 0xBF;
            Cont_StartReadInputs();
            Cont_ReadInputs();
            if (GbPak_CheckConnectionBlocking() != 0) {
                goto error;
            }
            keep = 1;
            if (D_812580F1 & 4) {
                sel = D_812580F1 & 3;
                if (*(u16*)(D_800603C0 + 6) == 0) {
                    if (sel == 0) {
                        D_8121E90C = (D_8121E914[0] == 1)   ? 0x20
                                     : (D_8121E914[0] == 2) ? 0x10
                                     : (D_8121E914[0] == 3) ? 2
                                     : (D_8121E914[0] == 4) ? 4
                                                            : 0x1000;
                        D_8121E910 = (D_8121E914[1] == 0)   ? 0x1000
                                     : (D_8121E914[1] == 2) ? 0x10
                                     : (D_8121E914[1] == 3) ? 2
                                     : (D_8121E914[1] == 4) ? 4
                                                            : 0x20;
                        gGbPakServiceThread = 0;
                        D_812580ED = 2;
                        func_811177DC(1);
                        GbAudio_SetMenuSoundMode(0);
                        GbEmu_DrawStatusBar();
                        keep = 0;
                    } else if (sel == 1) {
                        D_812580F1 = 1;
                        D_812580ED = 11;
                    } else {
                        D_812580F1 = 2;
                        D_812580ED = 7;
                        D_812580F2 = 0;
                    }
                }
            } else {
                sel = *(u16*)(D_800603C0 + 8);
                if (sel & 0xC00) {
                    func_811177DC(7);
                    if (*(u16*)(D_800603C0 + 8) & 0x400) {
                        if ((D_812580F1 & 3) >= 2) {
                            D_812580F1 &= 0xFC;
                        } else {
                            D_812580F1++;
                        }
                    } else {
                        if (D_812580F1 & 3) {
                            D_812580F1--;
                        } else {
                            D_812580F1 = (D_812580F1 & 0xFC) + 2;
                        }
                    }
                } else if (!(*(u16*)(D_800603C0 + 6) & 8) && (sel & 0xD000)) {
                    if (sel & 0x4000) {
                        D_812580F1 = 4;
                    } else {
                        D_812580F1 |= 4;
                        if (D_812580F1 != 4) {
                            if (D_812580F1 == 5) {
                                func_811177DC(2);
                            } else {
                                func_811177DC(6);
                            }
                        }
                    }
                }
            }
            if (keep) {
                GbEmu_DrawPauseMenu(*(s8**)(sRenderTaskPending[sGbEmuExitCode] + 8), D_812580F1 & 3);
            }
            break;

        case 5:
            if (--D_812580F0 != 0) {
                fade[3] = 0xFF - (s32)((D_812580F0 / 30.0f) * 63.0f);
                break;
            }
        shutdown:
        GbPak_StopIoThread(1 << D_81259A40);
        osViBlack(1);
        GbAudio_RampToSilence();
        GbEmu_Delay(20);
        GbEmu_ClearStatusBar();
        osWritebackDCacheAll();
        task = &sGbPlayerTasks[sGbEmuExitCode ^ 1];
        if (task->queue.validCount != 0) {
            osRecvMesg(&task->queue, 0, 1);
        }
        task = &sGbPlayerTasks[sGbEmuExitCode];
        if (task->queue.validCount != 0) {
            osRecvMesg(&task->queue, 0, 1);
        }
        *(s16*)(params + 0x2200) = (D_8121E914[0] << 8) | ((D_8121E914[1] - 1) & 0xFF);
        osSendMesg((OSMesgQueue*)(params + 0x21B4), D_812580E8, 1);
        while (1) {
        }

        case 6:
        fade_out:
            osViBlack(1);
            if (D_812580F0 != 0) {
                u8 a;

                D_812580F0--;
                fade = sGbPlayerTasks[sGbEmuExitCode].unk_3C;
                a = 0xFF - (s32)((D_812580F0 / 20.0f) * 63.0f);
                if ((a & 0x3F) >= (fade[3] & 0x3F)) {
                    fade[3] = a;
                }
                fade = sGbPlayerTasks[sGbEmuExitCode].unk_34;
                fade[0] = 0;
                break;
            }
            goto shutdown;

        case 7:
            fade[3] = 0xBF;
            Cont_StartReadInputs();
            Cont_ReadInputs();
            if (GbPak_CheckConnectionBlocking() != 0) {
                goto error;
            }
            sel = *(u16*)(D_800603C0 + 8);
            if (sel & 0xC00) {
                func_811177DC(7);
                D_812580F2 ^= 8;
            } else if (sel & 0x8000) {
                func_811177DC(6);
                D_812580ED = 8;
                D_812580F4 = 0;
                D_812580F2 = (D_8121E914[(D_812580F2 & 8) ? 1 : 0] & 7) | (D_812580F2 & 0xF8);
            } else if (sel & 0x4000) {
                func_811177DC(8);
                D_812580ED = 4;
            } else {
                switch (sel) {
                    case 0x1000:
                        sel = 0;
                        goto set7;
                    case 0x20:
                        sel = 1;
                        goto set7;
                    case 0x10:
                        sel = 2;
                        goto set7;
                    case 2:
                        sel = 3;
                        goto set7;
                    case 4:
                        sel = 4;
                    set7:
                        if (sel == D_8121E914[(D_812580F2 & 8) ? 0 : 1]) {
                            func_811177DC(3);
                        } else {
                            D_8121E914[(D_812580F2 & 8) ? 1 : 0] = sel;
                            func_811177DC(6);
                        }
                        break;
                }
            }
            GbEmu_DrawButtonConfig(*(s32*)(sRenderTaskPending[sGbEmuExitCode] + 8), D_812580F2, 0);
            break;

        case 8:
            fade[3] = 0xBF;
            if (GbPak_CheckConnectionBlocking() != 0) {
                goto error;
            }
            if (--D_812580F4 < -6) {
                D_812580F4 = -6;
                D_812580ED = 9;
            }
            GbEmu_DrawButtonConfig(*(s32*)(sRenderTaskPending[sGbEmuExitCode] + 8), D_812580F2, D_812580F4);
            break;

        case 9:
            fade[3] = 0xBF;
            Cont_StartReadInputs();
            Cont_ReadInputs();
            if (GbPak_CheckConnectionBlocking() != 0) {
                goto error;
            }
            sel = *(u16*)(D_800603C0 + 8);
            if (sel & 0x4000) {
                D_812580ED = 10;
                func_811177DC(8);
            } else if (sel & 0x300) {
                func_811177DC(5);
                if (*(u16*)(D_800603C0 + 8) & 0x200) {
                    if (D_812580F2 & 7) {
                        D_812580F2--;
                        goto call9;
                    }
                } else if ((D_812580F2 & 7) < 4) {
                    D_812580F2++;
                }
            } else if (sel & 0x8000) {
                if ((D_812580F2 & 7) == D_8121E914[(D_812580F2 & 8) ? 0 : 1]) {
                    func_811177DC(3);
                } else {
                    func_811177DC(6);
                    D_8121E914[(D_812580F2 & 8) ? 1 : 0] = D_812580F2 & 7;
                    D_812580ED = 10;
                }
            } else {
                switch (sel) {
                    case 0x1000:
                        sel = 0;
                        goto set9;
                    case 0x20:
                        sel = 1;
                        goto set9;
                    case 0x10:
                        sel = 2;
                        goto set9;
                    case 2:
                        sel = 3;
                        goto set9;
                    case 4:
                        sel = 4;
                    set9:
                        if (sel == D_8121E914[(D_812580F2 & 8) ? 0 : 1]) {
                            func_811177DC(3);
                        } else {
                            D_8121E914[(D_812580F2 & 8) ? 1 : 0] = sel;
                            D_812580ED = 10;
                            func_811177DC(6);
                        }
                        break;
                }
            }
        call9:
            GbEmu_DrawButtonConfig(*(s32*)(sRenderTaskPending[sGbEmuExitCode] + 8), D_812580F2, D_812580F4);
            break;

        case 10:
            fade[3] = 0xBF;
            if (GbPak_CheckConnectionBlocking() != 0) {
                goto error;
            }
            if (++D_812580F4 == 0) {
                D_812580ED = 7;
            }
            GbEmu_DrawButtonConfig(*(s32*)(sRenderTaskPending[sGbEmuExitCode] + 8), D_812580F2, D_812580F4);
            break;

        case 11:
            fade[3] = 0xBF;
            Cont_StartReadInputs();
            Cont_ReadInputs();
            if (GbPak_CheckConnectionBlocking() != 0) {
                goto error;
            }
            GbEmu_DrawQuitConfirm(*(s8**)(sRenderTaskPending[sGbEmuExitCode] + 8), D_812580F1 & 1);
            if (!(D_812580F1 & 2)) {
                sel = *(u16*)(D_800603C0 + 8);
                if (sel & 0x4000) {
                    func_811177DC(8);
                back4:
                    D_812580ED = 4;
                    D_812580F1 = 0;
                } else if (sel & 0x300) {
                    func_811177DC(5);
                    D_812580F1 ^= 1;
                } else if (sel & 0x9000) {
                    func_811177DC(6);
                    D_812580F1 |= 2;
                }
            } else if (D_812580F1 == 3) {
                GbEmu_ClearPalettes();
                D_812580F0 = 30;
                D_812580ED = 5;
            } else {
                goto back4;
            }
            break;

        case 20:
            fade[3] = 0xFF;
            GbAudio_SetMenuSoundMode(1);
            GbPak_PowerOff();
            D_812580ED = 21;
            break;

        case 21:
            fade[3] = 0xFF;
            GbEmu_DrawTextCommands(*(s8**)(sRenderTaskPending[sGbEmuExitCode] + 8), D_8111E4D0);
            GbEmu_DrawTextCommands(*(s8**)(sRenderTaskPending[sGbEmuExitCode] + 8), D_8111E4E4);
            if (GbPak_IsCartridgeInserted() == 0) {
                D_812580F3 = 30;
                D_812580ED = 22;
            }
            break;

        case 22:
            GbEmu_DrawTextCommands(*(s8**)(sRenderTaskPending[sGbEmuExitCode] + 8), D_8111E4D0);
        case 26:
            fade[3] = 0xFF;
            GbEmu_DrawTextCommands(*(s8**)(sRenderTaskPending[sGbEmuExitCode] + 8), D_8111E4E4);
            if (GbPak_IsCartridgeInserted() != 0) {
                if (D_812580F3 == 0) {
                    if (GbPak_ReidentifyCartridge() == 0) {
                        D_812580ED = 23;
                    } else {
                        D_812580ED = 25;
                    }
                }
                D_812580F3 = 30;
            } else if (D_812580F3 != 0) {
                D_812580F3--;
            }
            break;

        case 23:
            fade[3] = 0xFF;
            GbEmu_DrawTextCommands(*(s8**)(sRenderTaskPending[sGbEmuExitCode] + 8), D_8111E4E4);
            D_812580ED = 24;
            break;

        case 24:
            fade[3] = 0xFF;
            GbEmu_DrawTextCommands(*(s8**)(sRenderTaskPending[sGbEmuExitCode] + 8), D_8111E4E4);
            if (GbPak_VerifyRamBlocks(D_81259A40) == 0) {
                GbAudio_SetMenuSoundMode(0);
                gGbPakServiceThread = 0;
                D_812580ED = 2;
                D_811565A1 = 10;
                D_812580E8 = 0;
            } else {
                GbPak_PowerOff();
                D_812580ED = 25;
            }
            break;

        case 25:
            fade[3] = 0xFF;
            GbEmu_DrawTextCommands(*(s8**)(sRenderTaskPending[sGbEmuExitCode] + 8), D_8111E540);
            GbEmu_DrawTextCommands(*(s8**)(sRenderTaskPending[sGbEmuExitCode] + 8), D_8111E4E4);
            if (GbPak_IsCartridgeInserted() == 0) {
                D_812580F3 = 30;
                D_812580ED = 26;
            }
            break;
    }

    osWritebackDCacheAll();
    osViSwapBuffer(*(void**)(sRenderTaskPending[sGbEmuExitCode] + 8));
    crash_screen_set_draw_info(*(s32*)(sRenderTaskPending[sGbEmuExitCode] + 8), 0x140, 0x10);
    if (D_800AD928_jp == 0) {
        func_80004F34(&sGbPlayerTasks[sGbEmuExitCode], 0);
        D_812580EC = 1;
        sGbEmuExitCode ^= 1;
    }
}
#else
// clang-format off
#pragma GLOBAL_ASM("asm/jp/nonmatchings/fragments/gb_tower_emulator/jp/gb_emu_screen/GbEmu_RunFrame.s")
// clang-format on
#endif
