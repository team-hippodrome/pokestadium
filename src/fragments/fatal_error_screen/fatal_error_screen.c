#include "fatal_error_screen.h"
#include "src/graphics_textures.h"
#include "src/ui_graphics.h"
#include "src/text_system.h"
#include "src/jpeg_stream.h"
#include "src/gfx_buffer.h"
#include "src/memory.h"
#include "src/stage_loader.h"

// Controller port (0-3) whose Transfer Pak failed; selects where the diagram's cable is plugged in.
static s32 sErrorPort;
// Error code passed to Game_ShutdownAndLoadFragment (1-3); selects the message in text file 0xD.
static s32 sErrorMessageId;
// Text file 0xD: the three Transfer Pak / Game Boy error messages, indexed by sErrorMessageId - 1.
static char** sFatalErrorText;

/**
 * Fills the 320x240 screen with the green Poke Ball wallpaper from transfer_pak_error_ui
 * (64x64 RGBA16 at segment offset 0x0), drawn in COPY mode as 64x16 strips.
 * The three arguments (0x55, 0x70, 0x88 from FatalError_DrawFrame) are unused.
 */
void FatalError_DrawBackgroundPattern(s32 arg0, s32 arg1, s32 arg2) {
    s32 i;
    s32 j;
    s32 idx;

    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_COPY);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(gDisplayListHead++, G_RM_NOOP, G_RM_NOOP2);

    for (i = 0; i < 0xF0; i += 0x10) {
        for (j = 0; j < 0x140; j += 0x40) {
            idx = (i % 64) * 32;
            Gfx_DrawTextureRgba16(j, i, 0x40, 0x10, &D_2000000[idx * 4], 0x40, 0x200000);
        }
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

/**
 * Draws the 24x24 yellow "!" warning icon (transfer_pak_error_ui offset 0x2000) at (x, y).
 */
void FatalError_DrawIcon(s32 x, s32 y) {
    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_COPY);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_NOOP, G_RM_NOOP2);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGBA, G_CC_DECALRGBA);

    Gfx_DrawTextureRgba16(x, y, 0x18, 0x18, D_2002000, 0x18, 0x200000);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

/**
 * Draws the dark green bordered message window with the warning icon and the error text.
 * message_id is 1-based: 1 = Transfer Pak not set properly, 2 = problem with the Game Boy
 * Game Pak's connector, 3 = Game Boy saved data corrupted (choose NEW GAME).
 */
void FatalError_DrawMessagePanel(s32 message_id) {
    Ui_DrawBorderedPanel(0x16, 0x10, 0x114, 0x76, 0x11C5); // #103910
    FatalError_DrawIcon(0x27, 0x22);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);
    Font_SetLineHeight(0x10);
    Font_Printf(0x52, 0x21, Text_GetString(NULL, 0, sFatalErrorText, message_id - 1));
    Font_EndTexturedTextRendering();
}

/**
 * Draws the hardware diagram below the message: a controller with a Transfer Pak and Game Boy
 * cartridge crossed out by a red X, the N64 console front, and a controller cable routed from
 * the controller to the console's controller port number `port` (0-3).
 * The cable is built from 8x8 RGBA16 corner/end pieces plus gSPTextureRectangle spans that
 * stretch a single texel row or column of the previously loaded piece.
 */
void FatalError_DrawPortDiagram(s32 port) {
    // Screen X of the console's four controller jacks (ports 1-4) on the console texture drawn at X 0xAA.
    u16 jack[] = {
        0x00BC,
        0x00C6,
        0x00DF,
        0x00E9,
    };

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    // Cable corner leaving the controller, then a horizontal span stretched back toward the controller.
    Gfx_DrawTextureRgba16(0x98, 0xA1, 8, 8, D_20069A0, 8, 0);

    gSPTextureRectangle(gDisplayListHead++, 0x01C8, 0x0284, 0x0260, 0x02A4, G_TX_RENDERTILE, 0, 0, 0, 0x0400);
    gSPDisplayList(gDisplayListHead++, D_8006F4E0);

    // 64x70 controller with Transfer Pak, in three strips, then the 24x24 red X over the Transfer Pak.
    Gfx_DrawTextureRgba16(0x4D, 0x94, 0x40, 0x20, D_2002900, 0x40, 0x200000);
    Gfx_DrawTextureRgba16(0x4D, 0xB4, 0x40, 0x20, D_2003900, 0x40, 0x200000);
    Gfx_DrawTextureRgba16(0x4D, 0xD4, 0x40, 6, D_2004900, 0x40, 0x200000);
    Gfx_DrawTextureRgba16(0x5D, 0x98, 0x18, 0x18, D_2002480, 0x18, 0x200000);
    // 100x36 N64 console front, in two strips.
    Gfx_DrawTextureRgba16(0xAA, 0xAA, 0x64, 0x12, D_2004C00, 0x64, 0x200000);
    Gfx_DrawTextureRgba16(0xAA, 0xBC, 0x64, 0x12, D_2005A10, 0x64, 0x200000);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    // Lower-left cable corner with a vertical span stretched up to the first corner.
    Gfx_DrawTextureRgba16(0x9B, 0xD4, 8, 8, D_20068A0, 8, 0);

    gSPTextureRectangle(gDisplayListHead++, 0x026C, 0x02A4, 0x028C, 0x0350, G_TX_RENDERTILE, 0, 0, 0x0400, 0);

    // Lower-right corner below the selected port, with a horizontal span back to the left corner.
    Gfx_DrawTextureRgba16(jack[port], 0xD4, 8, 8, D_2006920, 8, 0);

    gSPTextureRectangle(gDisplayListHead++, 0x028C, 0x0350, jack[port] << 2, 0x0370, G_TX_RENDERTILE, 0, 0, 0, 0x0400);

    // Plug end entering the selected port, with a vertical span down to the corner.
    Gfx_DrawTextureRgba16(jack[port], 0xC0, 8, 8, D_2006820, 8, 0);

    gSPTextureRectangle(gDisplayListHead++, jack[port] << 2, 0x0320, (jack[port] + 8) << 2, 0x0350, G_TX_RENDERTILE, 0,
                        0x00E0, 0x0400, 0);
    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

/**
 * Per-frame draw callback: wallpaper, message window and port diagram for the stored error.
 */
s32 FatalError_DrawFrame(u8 arg0) {
    BgStage_DrawFrame();
    FatalError_DrawBackgroundPattern(0x55, 0x70, 0x88);
    FatalError_DrawMessagePanel(sErrorMessageId);
    FatalError_DrawPortDiagram(sErrorPort);
    return 0;
}

/**
 * Entry point of the Transfer Pak fatal error screen, reached only through
 * Game_ShutdownAndLoadFragment(port, message_id) after the game has blanked video, powered off
 * all Transfer Paks and faded out audio. //@src/game_state.c
 * Callers in gb_tower.c and pokemon_stats.c mostly pass 1 for Transfer Pak communication
 * failures (osGbpak* errors, failed connection polling, Game Boy save changed before a write)
 * and 2 for cartridge-side failures (connector check, cartridge RAM detection, cartridge reset
 * or removal); State_GbTower forwards the Game Boy emulator exit code, where 3 means an invalid
 * Game Boy opcode or an emulator heap allocation failure.
 * Loads transfer_pak_error_ui and text file 0xD, then draws forever: the only exit is turning
 * the console off, as the message instructs.
 */
void FatalError_Main(s32 port, s32 message_id) {
    StageContext* stage_context;

    sErrorPort = port;
    sErrorMessageId = message_id;

    main_pool_push_state('FATL');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    stage_context = StageContext_Allocate(0, 1, 2, 0, 2, 1);
    Font_Init(6, 0);
    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_2000000, transfer_pak_error_ui, 0);
    Text_InitStringTables();
    sFatalErrorText = Text_GetStringTable(0xD);
    StageContext_Activate(stage_context);

    while (true) {
        BgStage_WaitForCondition(FatalError_DrawFrame, 8, 8);
    }
}
