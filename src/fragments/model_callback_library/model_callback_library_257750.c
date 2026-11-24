#include "model_callback_library.h"
#include "geo_render.h"

/**
 * Calls segments[0] only while the current display object's textureMode is 0, then always calls
 * segments[1].
 */
void DisplayList_BuildOptionalPlusSegment(Gfx* gfx, DisplayListAddresses* addresses) {
    if (D_8006F09C->textureMode == 0) {
        gSPDisplayList(gfx++, addresses->segments[0]);
    }
    gSPDisplayList(gfx++, addresses->segments[1]);
    gSPEndDisplayList(gfx++);
}

/**
 * Geo callback (jump-table slot 0xF8). Builds DisplayList_BuildOptionalPlusSegment on the build
 * pass. Referenced by the Weedle, Diglett, Dugtrio, Magnemite, Magneton, Voltorb, Kabuto and Dragonair
 * models (not tested in the debugger).
 */
void DisplayList_InitOptionalPlusSegment(s32 arg0, DisplayListState* state) {
    Gfx* gfx;
    DisplayListAddresses* addresses;

    if (arg0 == 2) {
        addresses = state->addresses;
        gfx = Gfx_AllocDisplayList(0x50);
        state->gfx = gfx;
        DisplayList_BuildOptionalPlusSegment(gfx, addresses);
    }
}
