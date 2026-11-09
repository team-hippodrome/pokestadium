#include "particle_data_library.h"
#include "src/gfx_buffer.h"
#include "src/fragments/drowzee_game/drowzee_game.h"

/**
 * Geo callback (jump-table slot 0x1D8). Writes one of two raw G_SETCOMBINE commands, chosen by
 * drowzee_game's D_86402848 flag.
 */
void DisplayList_InitToggledBlendModeSegment(s32 arg0, DisplayListState* state) {
    Gfx* gfx;

    if (arg0 == 2) {
        gfx = (Gfx*)Gfx_AllocDisplayList(0x10);
        state->gfx = gfx;
        // Keep expanded GBI commands on one line for IDO matching.
        // clang-format off
        if (D_86402848 == 0) {
            {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xFC119604;    _gfx->words.w1 = 0xFF13FFFF;    }
        } else {
            {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xFCFFFFFF;    _gfx->words.w1 = 0xFFFDF638;    }
        }
        // clang-format on
        gSPEndDisplayList(gfx++);
    }
}
