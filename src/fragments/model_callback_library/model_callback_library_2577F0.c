#include "model_callback_library.h"
#include "battle_hud.h"
#include "geo_render.h"
#include "gfx_buffer.h"

// .data
static Gfx sScrollingPanelCombineDl[] = {
    gsDPPipeSync(),
    gsDPSetCombineLERP(TEXEL1, TEXEL0, ENV_ALPHA, TEXEL0, TEXEL1, TEXEL0, ENVIRONMENT, TEXEL0, COMBINED, 0, SHADE, 0,
                       COMBINED, 0, PRIM_LOD_FRAC, 0),
    gsDPSetEnvColor(0xFF, 0xFF, 0xFF, 0x64),
    gsSPEndDisplayList(),
};

// .bss
static s16 sScrollingPanelOffset;

/**
 * Draws two 32x32 RGBA16 textures (segments[0], segments[1]) blended by sScrollingPanelCombineDl,
 * with their tile offsets scrolled by sScrollingPanelOffset in different directions so the surface
 * seems to flow. Skipped while textureMode != 0. The offset only moves when D_800AF7B0 for the
 * active renderer mode is 0 and ModelPhase_IsIdle returns 0.
 */
void DisplayList_BuildScrollingDualPanel(Gfx* gfx, DisplayListAddresses* addresses) {
    s16 temp_t3;
    s16 temp_t4;
    s16 temp_t2;
    u8 temp_v0;

    temp_v0 = ModelRenderer_GetActiveMode();
    if (D_8006F09C->textureMode == 0) {
        temp_t2 = (sScrollingPanelOffset >> 4);
        temp_t3 = 0x4000 - (sScrollingPanelOffset >> 4);
        temp_t4 = 0x4000 - ((sScrollingPanelOffset >> 3) & 0xFFFF);

        gSPDisplayList(gfx++, sScrollingPanelCombineDl);

        gDPLoadTextureTile(gfx++, addresses->segments[0], G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, 0, 31, 31, 0,
                           G_TX_WRAP, G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);
        gDPSetTileSize(gfx++, G_TX_RENDERTILE, temp_t2, temp_t3, temp_t2 + 0x1F, temp_t3 + 0x1F);

        gDPLoadMultiTile(gfx++, addresses->segments[1], 0x100, 1, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, 0, 31, 31, 0,
                         G_TX_WRAP, G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);
        gDPSetTileSize(gfx++, 1, temp_t3, temp_t4, temp_t3, temp_t4 + 0x1F);

        if (D_800AF7B0[temp_v0 & 1] == 0) {
            if (ModelPhase_IsIdle() == 0) {
                sScrollingPanelOffset--;
            }
        }
    }
    gSPEndDisplayList(gfx++);
}

/**
 * Geo callback (jump-table slot 0x100). Builds DisplayList_BuildScrollingDualPanel on the build
 * pass. Referenced by the Grimer and Muk models; the debugger hit it for both in battle (sludge surface).
 */
void DisplayList_InitScrollingDualPanel(s32 arg0, DisplayListState* state) {
    Gfx* gfx;
    DisplayListAddresses* addresses;

    if (arg0 == 2) {
        addresses = state->addresses;
        gfx = Gfx_AllocDisplayList(0xF0);
        state->gfx = gfx;
        DisplayList_BuildScrollingDualPanel(gfx, addresses);
    }
}
