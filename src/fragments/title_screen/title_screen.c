#include "title_screen.h"
#include "src/graphics_textures.h"
#include "src/save_data.h"
#include "src/game_state.h"
#include "src/input.h"
#include "src/text_system.h"
#include "src/session.h"
#include "src/jpeg_stream.h"
#include "src/audio_sound_state.h"
#include "src/audio_sfx.h"
#include "src/audio_data.h"
#include "src/gfx_buffer.h"
#include "src/DDC0.h"
#include "src/controller.h"
#include "src/math_util.h"
#include "src/memory.h"
#include "src/stage_loader.h"
#if defined(VERSION_JP)
#include "src/leo.h"
#include "src/audio_category_fade.h"
#include "include/string.h"
#endif

#if !defined(VERSION_JP)
static char** sControllerErrorText;
static s32 pad_D_82100EA4;
#endif
static ModeSettings gTitleModeSettings;
static BinArchive* gTitleBackgroundArchive;
static u8* gTitleBackgroundImage;
static void* gTitleAssetBuffer;
static void* gTitleAudioBuffer;
static u16 gTitleStartPressMode;
static s16* gTitleAttractDemoProgress;
#if !defined(VERSION_JP)
static s32 gTitleInputReady;
#endif

#if defined(VERSION_JP)
#define TITLE_BLINK_START_FRAME 0
// Cleared-mode mask that selects the all-clear title background.
#define TITLE_ALL_MODES_CLEARED 0x1FE
#define TITLE_BOOT_STATE_PARAM s32 boot_state
#define TITLE_BOOT_STATE_IS_IDLE(boot_state) Title_IsBootStateIdle(boot_state)
#define TITLE_POLL_INPUTS() Title_PollInputs()
#define TITLE_DRAW_FRAME() Title_DrawFrame(0)
#else
#define TITLE_BLINK_START_FRAME 0x40
#define TITLE_ALL_MODES_CLEARED 0x1F8
#define TITLE_BOOT_STATE_PARAM void
#define TITLE_BOOT_STATE_IS_IDLE(boot_state) 1
#define TITLE_POLL_INPUTS() Controller_PollInputs()
#define TITLE_DRAW_FRAME() Title_DrawFrame()
#endif

void func_82100020(void) {
}

void func_82100028(UNUSED s32 arg0, UNUSED s32 arg1) {
}

void Title_PollDPad(void) {
    Input_GetRepeatedDPad();
}

s32 Title_ResolveStartPressState(void) {
    static s32 D_82100DC0 = 0;

    s32 var_v1 = 4;

    if (D_82100DC0 != 0) {
        switch (gTitleStartPressMode) {
            case 0:
                break;

            case 1:
                var_v1 = 0x80;
                break;

            case 2:
                var_v1 = 0x81;
                break;

            case 3:
                var_v1 = 0x82;
                break;

            case 4:
                var_v1 = 0x20;
                break;

            case 5:
                var_v1 = 0x25;
                break;

#if !defined(VERSION_JP)
            case 6:
                var_v1 = 0x11;
                break;
#endif
        }
    }

    return var_v1;
}

#if defined(VERSION_JP)
// "Controller 1 is not connected. Turn off the power and insert a controller into Controller Socket 1."
static char sNoControllerScrollText[] = "　　　コントローラ１がせつぞくされていません。でんげんをきって、コントローラを"
                                        "コントローラコネクタ１にさしこんでください。　　　　　";

/* Draws the 64DD disk error panel; the high byte of the status selects the message and the low byte is the code. */
void Title_DrawDiskErrorMessage(s32 disk_status) {
    // FAKE: unused stack slot retained for matching.
    UNUSED s32 pad;
    s32 message = (disk_status >> 8) & 0xFF;

    if (message != 0) {
        Gfx_FillRectWithDisplayState(30, 93, 260, 90, 0, 0, 0, 150);
        Font_BeginTranslucentTextRendering();
        Font_SetActive(2, -2);

        switch (message) {
            case 1:
                // "Error: %2d"
                Font_Printf(50, 96, "エラー：%2d", disk_status & 0xFF);
                // "Please read the instruction manual."
                Font_Printf(60, 131, "とりあつかいせつめいしょをおよみください。");
                break;

            case 2:
                // "Error: %2d"
                Font_Printf(50, 96, "エラー：%2d", disk_status & 0xFF);
                // "Caution: Do not remove the disk"
                Font_Printf(60, 111, "ちゅうい：アクセスランプてんめつちゅうに");
                // "while the access lamp is blinking."
                Font_Printf(110, 123, "ディスクをぬかないでください。");
                // "For details, please read"
                Font_Printf(65, 139, "くわしくは、とりあつかいせつめいしょを");
                // "the instruction manual."
                Font_Printf(125, 151, "およみください。");
                // "Please insert the disk again."
                Font_Printf(65, 166, "もういちどディスクをさしこんでください。");
                break;

            case 3:
                // "Error: %2d"
                Font_Printf(50, 96, "エラー：%2d", disk_status & 0xFF);
                // "The eject button may have been pressed."
                Font_Printf(45, 121, "イジェクトボタンがおされているかもしれません。");
                // "Remove the disk once"
                Font_Printf(105, 146, "いちどディスクをぬいて");
                // "and insert it properly."
                Font_Printf(90, 161, "きちんと さしこんでください。");
                break;

            case 4:
                // "Error: %2d"
                Font_Printf(50, 96, "エラー：%2d", disk_status & 0xFF);
                // "Is the Expansion Pak"
                Font_Printf(95, 111, "メモリーかくちょうパックが");
                // "inserted correctly?"
                Font_Printf(80, 126, "ただしく さしこんでありますか？");
                // "For details, please read"
                Font_Printf(65, 151, "くわしくは、とりあつかいせつめいしょを");
                // "the instruction manual."
                Font_Printf(125, 166, "およみください。");
                break;

            case 5:
                // "The wrong disk may"
                Font_Printf(70, 111, "まちがった ディスクが そうにゅう");
                // "have been inserted."
                Font_Printf(80, 126, "されているかのうせいがあります。");
                // "Please check."
                Font_Printf(105, 161, "かくにんしてください。");
                break;
        }

        Font_EndTexturedTextRendering();
    }
}

#endif
void Title_DrawPressStartBlink(s16 arg0, s16 arg1) {
    static s32 titlePressStartBlinkFrame = TITLE_BLINK_START_FRAME;

    s32 temp_v0 = titlePressStartBlinkFrame - 0x30;
    s32 var_s0 = 0;

    if (temp_v0 >= 0) {
        if (temp_v0 < 0x10) {
            var_s0 = temp_v0 * 0x10;
        } else if (temp_v0 < 0x23) {
            var_s0 = 0xFF;
        } else if (temp_v0 < 0x32) {
            var_s0 = (-temp_v0 * 0x10) + 0x320;
        }
    }

    titlePressStartBlinkFrame++;
    if (titlePressStartBlinkFrame > 0x6C) {
        titlePressStartBlinkFrame = 0x30;
    }

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, var_s0);

#if defined(VERSION_JP)
    // The JP "Press Start" image is 88x16, drawn in four strips.
    Gfx_DrawTextureIa16(arg0, arg1, 88, 4, D_2000000, 88, 0);
    Gfx_DrawTextureIa16(arg0, arg1 + 4, 88, 4, D_20002C0, 88, 0);
    Gfx_DrawTextureIa16(arg0, arg1 + 8, 88, 4, D_2000580, 88, 0);
    Gfx_DrawTextureIa16(arg0, arg1 + 12, 88, 4, D_2000840, 88, 0);
#else
    gDPLoadTextureTile(gDisplayListHead++, D_2000000, G_IM_FMT_IA, G_IM_SIZ_8b, 100, 0, 0, 0, 100 - 1, 15 - 1, 0,
                       G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                       G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(arg0, arg1, 100, 15, 0, 0, 0x400, 0x400, 0);
#endif

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

#if defined(VERSION_JP)
void Title_DrawNoControllerWarning(void) {
    static s32 titleNoControllerWarningFrame = 0;

    // FAKE: unused stack slot retained for matching.
    UNUSED s32 pad;
    s32 length;
    s32 x;
    s32 char_index;
    s32 i;

    if (!(Cont_GetControllerBits() & 1)) {
        length = strlen(sNoControllerScrollText) / 2;
        x = 16 - (titleNoControllerWarningFrame % 16);
        char_index = titleNoControllerWarningFrame / 16;

        gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 16, 0, 304, 240);

        Font_BeginTranslucentTextRendering();
        Font_SetActive(4, 0);

        for (i = 0; i < 19; i++) {
            Font_DrawCharAt(x, 184, ((s16*)sNoControllerScrollText)[char_index]);
            char_index++;
            if (char_index > length - 1) {
                char_index = 0;
            }
            x += 16;
        }

        Font_EndTexturedTextRendering();

        gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, 320, 240);

        titleNoControllerWarningFrame++;
        if (titleNoControllerWarningFrame >= length * 16) {
            titleNoControllerWarningFrame = 0;
        }
    }
}
#else
void Title_DrawNoControllerWarning(void) {
    static s32 titleNoControllerWarningFrame = 0;

    s32 sp24;
    s16 temp_s0;

    if (!(Cont_GetControllerBits() & 1)) {
        sp24 = Font_MeasureTextExtent(4, 0, Text_GetString(NULL, 0, sControllerErrorText, 0));
        temp_s0 = 0x50 - titleNoControllerWarningFrame;

        gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 16, 0, 304, 240);

        Font_BeginTranslucentTextRendering();
        Font_SetActive(4, 0);
        Font_Printf(temp_s0, 0xB3, Text_GetString(NULL, 0, sControllerErrorText, 0));

        temp_s0 += sp24 + 30;
        if (temp_s0 < 0x140) {
            Font_Printf(temp_s0, 0xB3, Text_GetString(NULL, 0, sControllerErrorText, 0));
        }

        Font_EndTexturedTextRendering();

        gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, 320, 240);

        titleNoControllerWarningFrame++;
        if (titleNoControllerWarningFrame >= (sp24 + 0x50)) {
            titleNoControllerWarningFrame = 0x32;
        }
    }
}
#endif

void Title_DrawBackgroundImage(u8* arg0) {
    s32 i;
    s32 j;

    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGB, G_CC_DECALRGB);

    gDPPipeSync(gDisplayListHead++);

    for (i = 0; i < 0xF0; i += 0x10) {
        for (j = 0; j < 0x140; j += 0x10, arg0 += 0x200) {
            Gfx_DrawTextureRgba16(j, i, 0x10, 0x10, arg0, 0x10, 0);
        }
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

#if defined(VERSION_JP)
void Title_DrawFrame(s32 disk_status) {
    BgStage_DrawFrame();
    Title_DrawBackgroundImage(gTitleBackgroundImage);
    if (Cont_GetControllerBits() & 1) {
        Title_DrawPressStartBlink(0x74, 0xB9);
    }
    Title_DrawNoControllerWarning();
    func_82100028(0x64, 0x6E);
    Title_DrawDiskErrorMessage(disk_status);
    BgStage_AdvanceFrame();
}
#else
void Title_DrawFrame(void) {
    BgStage_DrawFrame();
    Title_DrawBackgroundImage(gTitleBackgroundImage);
    if ((Cont_GetControllerBits() & 1) && (gTitleInputReady != 0)) {
        Title_DrawPressStartBlink(0x6E, 0xAF);
    }
    Title_DrawNoControllerWarning();
    func_82100028(0x64, 0x50);
    BgStage_AdvanceFrame();
}
#endif

static u8 gTitleDemoEligibilityMasks[] = {
    0x21, 0x22, 0x18, 0x21, 0x22, 0x14, 0x21, 0x22, 0x14, 0x21, 0x22, 0x24, 0x21, 0x22, 0x14, 0x21, 0x22, 0x24, 0x21,
    0x22, 0x21, 0x12, 0x21, 0x30, 0x03, 0x22, 0x21, 0x24, 0x21, 0x22, 0x14, 0x21, 0x22, 0x14, 0x21, 0x22, 0x21, 0x24,
    0x21, 0x24, 0x21, 0x24, 0x21, 0x30, 0x24, 0x21, 0x22, 0x22, 0x24, 0x21, 0x18, 0x21, 0x24, 0x21, 0x24, 0x22, 0x14,
    0x21, 0x14, 0x21, 0x22, 0x24, 0x21, 0x22, 0x18, 0x21, 0x24, 0x30, 0x21, 0x30, 0x24, 0x22, 0x24, 0x21, 0x24, 0x18,
    0x22, 0x24, 0x22, 0x24, 0x21, 0x18, 0x21, 0x22, 0x24, 0x22, 0x28, 0x24, 0x28, 0x21, 0x12, 0x21, 0x24, 0x18, 0x12,
    0x22, 0x18, 0x21, 0x24, 0x21, 0x18, 0x21, 0x18, 0x21, 0x24, 0x24, 0x24, 0x28, 0x21, 0x24, 0x22, 0x24, 0x18, 0x22,
    0x18, 0x21, 0x24, 0x21, 0x24, 0x22, 0x18, 0x24, 0x12, 0x18, 0x24, 0x28, 0x18, 0x18, 0x21, 0x18, 0x18, 0x18, 0x21,
    0x18, 0x18, 0x14, 0x22, 0x21, 0x24, 0x21, 0x24, 0x14, 0x18, 0x18, 0x18, 0x14, 0x21, 0x22, 0x18, 0x20, 0x28,
};

s32 Title_CountEligibleDemoEntries(s32 arg0) {
    s32 var_v1 = 0;
    s32 i;

    for (i = 0; i < 151; i++) {
        if (gTitleDemoEligibilityMasks[i] & arg0) {
            var_v1++;
        }
    }

    return var_v1;
}

s32 Title_FindEligibleDemoEntry(s32 arg0, u32 arg1) {
    s32 i;

    for (i = 0; i < 150; i++) {
        if (gTitleDemoEligibilityMasks[i] & arg1) {
            arg0--;
            if (arg0 == -1) {
                break;
            }
        }
    }

    return i + 1;
}

void Title_PickDemoMatchup(u8* arg0) {
    s32 temp_s0;
    s32 sp38;
    UNUSED s32 pad[2];
    s32 sp2C;

    sp2C = Title_CountEligibleDemoEntries(arg0[2]);

    temp_s0 = osGetCount() & 0xFF;
    while (temp_s0-- > 0) {
        MathUtil_Random_ZeroOne();
    }

    temp_s0 = MathUtil_Random_ZeroOne() * sp2C;
    do {
        sp38 = MathUtil_Random_ZeroOne() * sp2C;
    } while (temp_s0 == sp38);

    Session_SetupDemoBattle(arg0[0], arg0[1], Title_FindEligibleDemoEntry(temp_s0, arg0[2]),
                            Title_FindEligibleDemoEntry(sp38, arg0[2]));
}

static u8 gTitleAttractDemoScenarios[2][4] = {
    { 4, 0, 1, 3 },
    { 5, 0, 2, 9 },
};

static u8 D_82100E6C[4][4] = {
    { 3, 3, 4, 13 },
    { 6, 3, 8, 3 },
    { 7, 4, 16, 9 },
    { 0, 0, 32, 13 },
};

s32 Title_StartAttractDemo(void) {
    u8 temp_a1 = (*gTitleAttractDemoProgress >> 8);
    u8 temp_a2 = *gTitleAttractDemoProgress;
    s16 mod = temp_a2 % 2;
    s16 low_index = temp_a2 / 2;

    if (mod == 0) {
        Title_PickDemoMatchup(gTitleAttractDemoScenarios[low_index]);
    } else {
        gSessionContext.mode = 0x19;
        gSessionContext.roundOrDemoId = gTitleAttractDemoScenarios[low_index][3];
    }

    temp_a1 = ((temp_a1 + 1) % 4);
    temp_a2 = (temp_a2 + 1) % 12;
    *gTitleAttractDemoProgress = (temp_a1 << 8) | temp_a2;
    return 2;
}

#if defined(VERSION_JP)
/* Returns whether the 64DD boot state machine is in a state that allows leaving the title screen. */
s32 Title_IsBootStateIdle(s32 boot_state) {
    return (boot_state == 5) || (boot_state == 0) || (boot_state == 1) || (boot_state == 3);
}
#endif

s32 Title_ShouldStartAttractDemo(TITLE_BOOT_STATE_PARAM) {
    static u32 titleAttractIdleTimer = 0;

    s32 sp1C = 0;

    if (titleAttractIdleTimer >= 0x259) {
        if ((Audio_GetActivityScore() == 0) && TITLE_BOOT_STATE_IS_IDLE(boot_state)) {
            sp1C = 1;
        }
    } else {
        titleAttractIdleTimer += 1;
    }
    return sp1C;
}

#if defined(VERSION_JP)
void Title_PollInputs(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}
#endif

s32 func_821009AC(void) {
    return 0;
}

#if defined(VERSION_JP)
s32 Title_Loop(void) {
    s32 var_s0 = 0;
    s32 boot_status;
    s32 boot_state;
    u16 disk_status;

    Input_SetRepeatController(gPlayer1Controller);

    while (var_s0 == 0) {
        Title_PollInputs();

        boot_status = Leo_RunBootStateMachine(gTitleAssetBuffer);
        boot_state = (boot_status >> 16) & 0xFFFF;
        disk_status = boot_status;

        if (boot_state == 10) {
            Audio_FadeCategoriesAndStreams(16);
            var_s0 = 3;
        } else if (gPlayer1Controller->buttonPressed & 0x9000) {
            if (Title_IsBootStateIdle(boot_state)) {
                Audio_PlaySoundEffectById(SFX_TITLE_START);
                var_s0 = Title_ResolveStartPressState();
            }
        } else if (Title_ShouldStartAttractDemo(boot_state) != 0) {
            var_s0 = Title_StartAttractDemo();
        } else {
            var_s0 = func_821009AC();
            if (var_s0 == 0) {
                Title_PollDPad();
            }
        }

        Title_DrawFrame(disk_status);
    }

    return var_s0;
}
#else
s32 Title_Loop(void) {
    s32 var_s0 = 0;
    s32 var_s2 = 30;

    Input_SetRepeatController(gPlayer1Controller);

    while (var_s0 == 0) {
        if (gTitleInputReady == 0) {
            var_s2--;
            if (var_s2 <= 0) {
                gTitleInputReady = 1;
            }
        }

        Controller_PollInputs();

        if ((gPlayer1Controller->buttonPressed & 0x9000) && (gTitleInputReady != 0)) {
            Audio_PlaySoundEffectById(SFX_TITLE_START);
            var_s0 = Title_ResolveStartPressState();
        } else if (Title_ShouldStartAttractDemo() != 0) {
            var_s0 = Title_StartAttractDemo();
        } else {
            var_s0 = func_821009AC();
            if (var_s0 == 0) {
                Title_PollDPad();
            }
        }

        Title_DrawFrame();
    }

    return var_s0;
}
#endif

void Title_FadeIn(void) {
    if (StageContext_GetFadeMode() == 1) {
        StageFade_StartFromOpaque(0xA);
        while (StageContext_GetFadeMode() != 0) {
            TITLE_POLL_INPUTS();
            TITLE_DRAW_FRAME();
        }
    }
}

void Title_FadeOut(s32 arg0) {
    if (arg0 == 3) {
        StageContext_SetClearColor(1);
    } else {
        StageContext_SetClearColor(0xFFFF);
    }

    StageFade_StartFromTransparent(8);

    while (StageContext_GetFadeMode() != 1) {
        TITLE_POLL_INPUTS();
        TITLE_DRAW_FRAME();
    }
}

void Title_LoadAssets(void) {
    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    // bin contains a JP logo and press start, but this bin is loaded for the "Press Start" gfx
    ASSET_LOAD(D_2000000, title_ui, 0);

    gTitleAssetBuffer = main_pool_alloc(0x4510, 0);
    gTitleAudioBuffer = main_pool_alloc(0x100000, 0);

    gTitleBackgroundArchive = ASSET_LOAD2(backgrounds, 1, 1);

    if (gTitleModeSettings.clearedModes == TITLE_ALL_MODES_CLEARED) {
        gTitleBackgroundImage = BinArchive_GetFile(gTitleBackgroundArchive, 0x11);
    } else {
        gTitleBackgroundImage = BinArchive_GetFile(gTitleBackgroundArchive, 0);
    }

#if !defined(VERSION_JP)
    gTitleInputReady = 0;
#endif
}

s32 Title_Main(UNUSED s32 arg0, s16* arg1) {
    s32 sp24;

    gTitleAttractDemoProgress = arg1;

    if (StageContext_GetCurrent() == NULL) {
        main_pool_push_state('TITL');

        Gfx_InitDisplayListBuffers(0x10000, 0);
        StageContext_Activate(StageContext_Allocate(0, 1, 3, 1, 2, 1));
    } else {
        StageLoader_UpdateSegments();
    }

    Save_EnsureBankLoaded(2);
    Save_GetModeSettings(&gTitleModeSettings, 0);
    Font_Init(6, 0);
    Title_LoadAssets();
#if !defined(VERSION_JP)
    Text_InitStringTables();
    sControllerErrorText = Text_GetStringTable(0x14);
#endif
    Audio_LoadWaveTablesAndMusic(gTitleAudioBuffer);
    Title_FadeIn();
    sp24 = Title_Loop();
    Title_FadeOut(sp24);
    StageContext_Deactivate();
#if defined(VERSION_JP)
    Leo_AbortPendingBootRequest();
#endif
    Audio_WaitForIdle(0);
    Audio_ClearWaveDataLoaded();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('TITL');
    return sp24;
}
