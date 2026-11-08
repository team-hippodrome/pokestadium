#include "n64_logo.h"
#include "src/save_data.h"
#include "src/gb_data.h"
#include "src/ui_graphics.h"
#include "src/text_system.h"
#include "src/3D140.h"
#include "src/jpeg_stream.h"
#include "src/audio_sfx.h"
#include "src/audio_channel.h"
#include "src/gfx_buffer.h"
#include "src/gfx_rect.h"
#include "src/memory.h"
#include "src/stage_loader.h"

/**
 * Draws the 128x112 RGBA16 "NINTENDO 64" logo (n64_logo_texture, loaded at segment 0x02)
 * centered at screen (96, 64)-(223, 175), as seven 128x16 strips in COPY mode.
 */
void N64Logo_DrawLogoTexture(void) {
    s32 rect_y;
    u8* texture_strip;
    u32 tmp = 0xF;

    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_COPY);
    gDPSetRenderMode(gDisplayListHead++, G_RM_NOOP, G_RM_NOOP2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, 320, 240);
    gDPPipeSync(gDisplayListHead++);

    texture_strip = D_2000000;
    for (rect_y = 0x40; rect_y < 0xB0; rect_y += 0x10, texture_strip += 0x1000) {
        gDPLoadTextureBlock(gDisplayListHead++, texture_strip, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 16, 0,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, 7, 4, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, 0x0180, rect_y << 2, 0x037C, (rect_y + tmp) << 2, G_TX_RENDERTILE, 0, 0,
                            0x1000, 0x0400);
    }

    gDPPipeSync(gDisplayListHead++);
    gDPSetTextureConvert(gDisplayListHead++, G_TC_FILT);
}

/**
 * Per-frame draw callback: clears the framebuffer to black (RGBA5551 0x0001) and draws the logo.
 */
s32 N64Logo_DrawFrame(UNUSED u8 arg0) {
    BgStage_DrawFrame();
    GfxImage_FillCurrent(&gDisplayListHead, 1);
    N64Logo_DrawLogoTexture();

    return 0;
}

/**
 * Busy-waits until 1,000,000 microseconds (1 second) have passed since start_time.
 * ticks * 64 / 3000 is the libultra OS_CYCLES_TO_USEC conversion for the 46.875 MHz counter.
 */
void N64Logo_WaitForElapsed(u64 start_time) {
    u32 elapsed_usec = 0xF;

    while (elapsed_usec < 0xF4240) {
        elapsed_usec = __ull_div(__ll_mul(osGetTime() - start_time, 0x40), 0xBB8);
    }
}

/**
 * Boot-time Transfer Pak scan, run once while the logo is held on screen.
 * GbSave_ScanAllPorts (via GbTower_ScanPaks) initializes every plugged-in Transfer Pak,
 * identifies supported Pokemon Red/Green/Blue/Yellow cartridges for the region, checks
 * cartridge RAM, resets the per-port Game Boy save cache and enables disconnect polling for
 * ports holding a recognized Pokemon cartridge. This is the only caller of
 * GbSave_ScanAllPorts, so later Transfer Pak features rely on this boot scan.
 * N64Logo_Main starts its 1-second hold timer before the scan, so the controller-bus work
 * overlaps the logo instead of adding a blank delay.
 * A faulty pak or cartridge sends the game straight to the fatal error screen via
 * Game_ShutdownAndLoadFragment. //@src/pokemon_stats.c //@src/gb_tower.c
 * Allocations made during the scan are released when the 'GPAK' pool state is popped.
 */
void N64Logo_ScanTransferPaks(void) {
    main_pool_push_state('GPAK');

    GbSave_ScanAllPorts();

    main_pool_pop_state('GPAK');
}

/**
 * Boot logo sequence, loaded by State_N64LogoIntro and State_FastN64Logo. //@src/game_state.c
 * Fades in the Nintendo 64 logo, applies the saved sound options (bit 0 = mono output,
 * bit 1 = Announcer voice off, as in Options_ApplySoundSettings), plays SFX_N64_LOGO, scans the
 * Transfer Paks while holding the logo for at least 1 second from the sound, then fades out.
 * Returns 2.
 */
s32 N64Logo_Main(UNUSED s32 arg0, UNUSED s32 arg1) {
    u64 sound_start_time;
    StageContext* stage_context;
    u16 sound_options;

    main_pool_push_state('LOGO');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    stage_context = StageContext_Allocate(0, 1, 2, 0, 2, 1);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_2000000, n64_logo_texture, 0);

    Save_EnsureBankLoaded(2);
    StageContext_Activate(stage_context);
    StageFade_Start(1);
    BgStage_RunUntilCondition(3, N64Logo_DrawFrame);
#if !defined(VERSION_JP)
    // JP N64Logo_Main goes straight from the fade-in to Save_GetOptionsField02 without loading text tables.
    Text_InitStringTables();
    Text_LoadPlayerLabels();
    Text_LoadNameTable();
#endif
    Save_GetOptionsField02(&sound_options);

    if (sound_options & 1) {
        Audio_SetMonoOutput(1);
    } else {
        Audio_SetMonoOutput(0);
    }

    if (sound_options & 2) {
        Audio_SetVoiceEnabled(0);
    } else {
        Audio_SetVoiceEnabled(1);
    }

    Audio_PlaySoundEffectById(SFX_N64_LOGO);
    sound_start_time = osGetTime();
    N64Logo_ScanTransferPaks();
    N64Logo_WaitForElapsed(sound_start_time);
    StageFade_Start(0x10);
    BgStage_RunUntilCondition(0x10, N64Logo_DrawFrame);
    StageContext_Deactivate();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('LOGO');
    return 2;
}
