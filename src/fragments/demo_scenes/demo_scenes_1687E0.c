#include "demo_scenes.h"
#include "src/graphics_textures.h"
#include "src/gfx_rect.h"

static Gfx sGfxSetup_BrickWipe[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsSPClearGeometryMode(G_ZBUFFER | G_CULL_BACK | G_LIGHTING),
    gsDPSetRenderMode(G_RM_XLU_SURF, G_RM_NOOP2),
    gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
    gsDPSetPrimColor(0, 0, 5, 5, 5, 255), // #050505
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPSetTexturePersp(G_TP_NONE),
    gsSPEndDisplayList(),
};

static Gfx sGfxRestore_BrickWipe[] = {
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPSetTexturePersp(G_TP_PERSP),
    gsSPSetGeometryMode(G_ZBUFFER | G_CULL_BACK | G_LIGHTING),
    gsSPEndDisplayList(),
};

// Party camera keyframes indexed by partySlot 0..5 (ClearCeremony_SetupOrbitCamera). The trophy (slot 6)
// uses a fixed camera, so entries 6..9 have no reader in this fragment.
DioramaAnimData sClearCeremonyCameraKeyframes[] = {
    {
        0,
        0,
        0,
        0,
        12743,
        0.0f,
        10.0f,
        0,
        0,
        0,
        0,
        0,
        0.0f,
        4.0f,
        0.15000000596046448f,
        0.009999999776482582f,
        30.0f,
    },
    {
        0,
        0,
        0,
        0,
        0,
        0.0f,
        10.0f,
        0,
        0,
        0,
        0,
        0,
        0.0f,
        3.0f,
        0.15000000596046448f,
        0.009999999776482582f,
        200.0f,
    },
    {
        0,
        0,
        0,
        7281,
        16384,
        0.0f,
        10.0f,
        0,
        0,
        0,
        0,
        0,
        0.0f,
        3.5f,
        0.15000000596046448f,
        0.009999999776482582f,
        200.0f,
    },
    {
        0,
        0,
        0,
        25486,
        9102,
        0.0f,
        10.0f,
        0,
        0,
        0,
        0,
        -380,
        0.5f,
        4.0f,
        0.15000000596046448f,
        0.009999999776482582f,
        200.0f,
    },
    {
        0,
        12743,
        0,
        -3640,
        9102,
        3.0f,
        10.0f,
        0,
        0,
        0,
        1820,
        0,
        0.0f,
        3.5f,
        0.15000000596046448f,
        0.009999999776482582f,
        200.0f,
    },
    {
        0,
        0,
        0,
        14563,
        0,
        0.0f,
        10.0f,
        0,
        0,
        0,
        0,
        0,
        0.0f,
        4.0f,
        0.15000000596046448f,
        0.009999999776482582f,
        200.0f,
    },
    {
        -7282,
        0,
        0,
        25486,
        14563,
        2.0f,
        10.0f,
        0,
        0,
        0,
        0,
        -198,
        0.5f,
        4.0f,
        0.15000000596046448f,
        0.009999999776482582f,
        200.0f,
    },
    {
        0,
        0,
        0,
        -3640,
        23665,
        0.0f,
        10.0f,
        0,
        0,
        0,
        1820,
        0,
        0.0f,
        3.5f,
        0.15000000596046448f,
        0.009999999776482582f,
        200.0f,
    },
    {
        0,
        0,
        0,
        14563,
        0,
        0.0f,
        10.0f,
        0,
        0,
        0,
        0,
        0,
        0.0f,
        4.0f,
        0.15000000596046448f,
        0.009999999776482582f,
        200.0f,
    },
    {
        0,
        0,
        0,
        -3640,
        0,
        0.0f,
        7.0f,
        0,
        0,
        0,
        1820,
        0,
        0.0f,
        3.5f,
        0.15000000596046448f,
        0.009999999776482582f,
        200.0f,
    },
};

static s16 sWipeState; // 0 idle, 1 covered, 2 opening, 3 closing
static s16 sWipeOffset; // 0 = fully covered, 0x140 = fully open
static s16 sWipeSpeed;

/**
 * Starts the band wipe in state (0 idle, 1 covered, 2 opening, 3 closing) at speed units per frame.
 * States 0 and 3 start fully open (offset 0x140); states 1 and 2 start fully covered (offset 0).
 */
void WipeTransition_Start(s32 state, s32 speed) {
    sWipeState = state;
    sWipeSpeed = speed;

    // Equivalent to state 0 or 3.
    if ((sWipeState == 0) || ((sWipeState != 1) && (sWipeState != 2) && (sWipeState == 3))) {
        sWipeOffset = 0x140;
    } else {
        sWipeOffset = 0;
    }
}

/**
 * Returns the band wipe state (0 idle, 1 covered, 2 opening, 3 closing).
 */
s32 WipeTransition_GetState(void) {
    return sWipeState;
}

/**
 * Advances the band wipe: opening (2) moves the offset up to 0x140 then goes idle; closing (3) moves it
 * down to 0 then reports covered (1).
 */
void WipeTransition_Update(void) {
    s16 tmp = sWipeOffset;

    switch (sWipeState) {
        case 2:
            if (tmp < 0x140) {
                tmp += sWipeSpeed;
            } else {
                sWipeState = 0;
            }
            break;

        case 3:
            if (tmp > 0) {
                tmp -= sWipeSpeed;
            } else {
                sWipeState = 1;
            }

            if (tmp < 0) {
                tmp = 0;
            }
            break;
    }

    sWipeOffset = tmp;
}

/**
 * Emits the band wipe: five 48-pixel horizontal bands of the stretched cup_clear_ui segment-3 stripe
 * texture, alternate bands sliding in from the left and right by the wipe offset. parity selects which
 * bands come from which side.
 */
Gfx* WipeTransition_BuildBrickDisplayList(Gfx* gfx, u32 parity) {
    s32 idx1 = parity % 2;
    s32 idx2 = (parity + 1) % 2;
    s32 i;
    s16 xl;
    s16 yl;

    gSPDisplayList(gfx++, sGfxSetup_BrickWipe);

    gDPLoadTextureBlock_4b(gfx++, D_3000000, G_IM_FMT_I, 12, 48, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gDPPipeSync(gfx++);

    for (i = idx1; i < 5; i += 2) {
        xl = (sWipeOffset * 2) - 0x140;
        yl = (i * 0x30);

        gSPScisTextureRectangle(gfx++, xl << 2, yl << 2, (xl + 0x280) << 2, (yl + 24) << 2, 0, 0, 0, 1 << 6, 1 << 10);
        gSPScisTextureRectangle(gfx++, xl << 2, (yl + 24) << 2, (xl + 0x280) << 2, (yl + 48) << 2, 0, 0, 0, 1 << 6,
                                1 << 10);
    }

    for (i = idx2; i < 5; i += 2) {
        xl = -sWipeOffset * 2;
        yl = (i * 0x30);

        gSPScisTextureRectangle(gfx++, xl << 2, yl << 2, (xl + 0x280) << 2, (yl + 24) << 2, 0, 0x500, 0, -(1 << 6),
                                1 << 10);
        gSPScisTextureRectangle(gfx++, xl << 2, (yl + 24) << 2, (xl + 0x280) << 2, (yl + 48) << 2, 0, 0x500, 0,
                                -(1 << 6), 1 << 10);
    }

    gSPDisplayList(gfx++, sGfxRestore_BrickWipe);

    return gfx;
}

/**
 * Appends the band wipe to the display list whenever it is not idle.
 */
void WipeTransition_Draw(void) {
    Gfx* var_a0 = gDisplayListHead;

    if (sWipeState != 0) {
        if (sWipeState == 2) {
            var_a0 = WipeTransition_BuildBrickDisplayList(var_a0, 0);
        } else {
            var_a0 = WipeTransition_BuildBrickDisplayList(var_a0, 1);
        }
    }
    gDisplayListHead = var_a0;
}

/**
 * Draws a texture rectangle clipped to the current scissor box, advancing the s/t texture origin by the
 * clipped amount so the visible part stays aligned.
 */
void UI_DrawClippedTextureRect(s16 arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4, s16 arg5, s16 arg6, s16 arg7) {
    Rectangle sp30;

    GfxRect_SetBounds(&sp30, arg0, arg1, (arg0 + arg2) - 1, (arg1 + arg3) - 1);

    if (GfxRect_ClipToScissor(&sp30) != 0) {
        if (arg6 != 0) {
            arg4 += ((sp30.left - arg0) * arg6) >> 5;
        }

        if (arg7 != 0) {
            arg5 += ((sp30.top - arg1) * arg7) >> 5;
        }

        sp30.right += 1;
        sp30.bottom += 1;

        gSPTextureRectangle(gDisplayListHead++, sp30.left << 2, sp30.top << 2, sp30.right << 2, sp30.bottom << 2,
                            G_TX_RENDERTILE, arg4, arg5, arg6, arg7);
    }
}

/**
 * Loads a width x height IA8 texture and draws it 1:1 at (x, y).
 */
void UI_DrawTiledTexture(s32 arg0, s32 arg1, s32 arg2, s32 arg3, u8* arg4, UNUSED s32 arg5) {
    gDPLoadTextureTile(gDisplayListHead++, arg4, G_IM_FMT_IA, G_IM_SIZ_8b, arg2, arg3, 0, 0, arg2 - 1, arg3 - 1, 0,
                       G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                       G_TX_NOLOD);

    UI_DrawClippedTextureRect(arg0, arg1, arg2, arg3, 0, 0, 0x400, 0x400);
}

/**
 * Draws an IA8 texture whose intensity blends from the env color (dark texels) to the prim color (bright
 * texels) of colors, with texel alpha; used for the ceremony captions.
 */
void UI_DrawGradientTexture(u8* arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4, SpriteColorParams* arg5) {
    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetCombineLERP(gDisplayListHead++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                      PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetEnvColor(gDisplayListHead++, (u8)arg5->unk_06, (u8)arg5->unk_08, (u8)arg5->unk_0A, 255);
    gDPSetPrimColor(gDisplayListHead++, 0, 0, (u8)arg5->unk_00, (u8)arg5->unk_02, (u8)arg5->unk_04, 255);
    gSPClearGeometryMode(gDisplayListHead++, G_ZBUFFER | G_LIGHTING);

    UI_DrawTiledTexture(arg1, arg2, arg3, arg4, arg0, arg3);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

/**
 * Draws an RGBA16 texture at (x, y) with the env-tint texture setup, e.g. the "R-2" badge.
 */
void UI_DrawTextureRgba16(u8* arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4) {
    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    Gfx_DrawTextureRgba16(arg1, arg2, arg3, arg4, arg0, arg3, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}
