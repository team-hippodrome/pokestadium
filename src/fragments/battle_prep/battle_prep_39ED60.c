#include "battle_prep.h"
#include "src/geo_render.h"
#include "src/display_object_textures.h"
#include "src/model_renderer.h"
#include "src/ui_graphics.h"
#include "src/audio_commands_category2.h"
#include "src/gfx_rect.h"
#include "src/matrix.h"
#include "src/math_util.h"
#include "src/memmap.h"
#include "src/memory.h"

static s16 D_84B16390[] = { -0x60, -0x20, 0x20, 0x60, -0x60, -0x20, 0x20, 0x60 };
static s16 D_84B163A0[] = { 0x30, 0x30, 0x30, 0x30, -0x30, -0x30, -0x30, -0x30 };
static f32 D_84B163B0[] = { 0.1f, 0.3f, 0.5f, 0.7f, 0.9f, 1.0f, 1.0f, 0.9f, 0.7f, 0.5f, 0.3f, 0.1f };
static Vtx D_84B163E0[] = {
    VTX(-32, 48, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-32, 16, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, 16, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(32, 48, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, 16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-32, -16, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -16, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(32, 16, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, -16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-32, -48, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -48, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(32, -16, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B164A0[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(0, 0, 0, TEXEL0, 0, 0, 0, TEXEL0, 0, 0, 0, COMBINED, COMBINED, 0, PRIM_LOD_FRAC, 0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B163E0, 12, 0),
    gsDPLoadTextureBlock(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 6, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureBlock(0x0F001000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 6, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsDPLoadTextureBlock(0x0F002000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 6, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B16588[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B163E0,
    0x05000000, 0x22010000, 0x00000000, 0x08000000, BattlePrepBadgeAward_DrawPlate,
    D_84B164A0, 0x06000000, 0x01000000, 0x00000000,
};
static Vtx D_84B165C0[] = {
    VTX(-24, 25, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-24, 0, 0, 0, 800, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(24, 0, 0, 1536, 800, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(24, 25, 0, 1536, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-24, 0, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-24, -25, 0, 0, 800, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(24, -25, 0, 1536, 800, 0xFF, 0xFF, 0xFF, 0xFF), VTX(24, 0, 0, 1536, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B16640[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(0, 0, 0, TEXEL0, 0, 0, 0, TEXEL0, 0, 0, 0, COMBINED, COMBINED, 0, PRIM_LOD_FRAC, 0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B165C0, 8, 0),
    gsDPLoadTextureBlock(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 48, 25, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 6, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureBlock(0x0F000960, G_IM_FMT_RGBA, G_IM_SIZ_16b, 48, 25, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 6, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B166E8[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B165C0,
    0x05000000, 0x1B000000, 0x00000000, 0x00000000, 0x00050000,
    0x05000000, 0x22050000, 0x00000000, 0x08000000, BattlePrepBadgeAward_DrawMedal,
    D_84B16640, 0x06000000, 0x06000000, 0x01000000, 0x00000000,
};
static Vtx D_84B16738[] = {
    VTX(-16, 16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-16, -16, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(16, -16, 0, 1024, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(16, 16, 0, 1024, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B16778[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineMode(G_CC_MODULATEIA, G_CC_PASS2),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B16738, 4, 0),
    gsDPLoadTextureBlock_4b(0x0F000000, G_IM_FMT_I, 32, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                            5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B167E0[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B16738,
    0x05000000, 0x22050000, 0x00000000, 0x08000000, BattlePrepBadgeAward_DrawSmokePuff,
    D_84B16778, 0x06000000, 0x01000000, 0x00000000,
};
static Vtx D_84B16818[] = {
    VTX(-32, 32, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-32, 0, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, 0, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(32, 32, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, 0, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),        VTX(-32, -32, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -32, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(32, 0, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B16898[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, COMBINED, 0, 0,
                       0, COMBINED),
    gsDPSetPrimColor(0, 0, 0, 255, 255, 255),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B16818, 8, 0),
    gsDPLoadTextureBlock_4b(0x02022640, G_IM_FMT_I, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                            6, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureBlock_4b(0x02022A40, G_IM_FMT_I, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                            6, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B16948[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B16818,
    0x05000000, 0x22050000, 0x00000000, 0x08000000, BattlePrepBadgeAward_DrawShockwave,
    D_84B16898, 0x06000000, 0x01000000, 0x00000000,
};
static Vtx D_84B16980[] = {
    VTX(-16, 16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-16, -16, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(16, -16, 0, 1024, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(16, 16, 0, 1024, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B169C0[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, COMBINED, 0, 0, 0,
                       COMBINED),
    gsDPSetEnvColor(255, 255, 0, 255),
    gsDPSetPrimColor(0, 0, 255, 255, 255, 255),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B16980, 4, 0),
    gsDPLoadTextureBlock(D_2022E40, G_IM_FMT_IA, G_IM_SIZ_8b, 32, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B16A38[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B16980, 0x05000000,
    0x22050000, D_84B169C0, 0x25000000, 0x06000000, 0x01000000,
};

/**
 * Geo draw callback (phase 5) for a badge plate: binds the plate texture for the object's slot
 * ((effectSlot - first slot) % 8, badge_ui D_2018000 table: BOULDER ... EARTH) to segment 0x0F.
 */
s32 BattlePrepBadgeAward_DrawPlate(s32 callback_phase, unk_func_80011B94* node) {
    BattlePrepBadgeAward* badge_award;
    s32 plate_index;

    if (callback_phase == 5) {
        badge_award = (BattlePrepBadgeAward*)D_8006F09C->node.callbackArg;
        plate_index = (D_8006F09C->effectSlot - badge_award->displayObjects->effectSlot) % 8;

        gDPPipeSync(gDisplayListHead++);
        gSPSegment(gDisplayListHead++, 0x0F,
                   (u32)Memmap_GetSegmentVaddr(*(s32*)(badge_award->plateTextures + plate_index * 4)) & 0x1FFFFFFF);
        gSPDisplayList(gDisplayListHead++, node->display_node_header.callbackArg);

        GeoRender_ApplyMaterialState();
    }
    return 0;
}

/**
 * Geo draw callback (phase 5) for a badge medal: binds the medal texture for the object's slot
 * ((effectSlot - first slot) % 8, badge_ui D_2021620 table) to segment 0x0F.
 */
s32 BattlePrepBadgeAward_DrawMedal(s32 callback_phase, unk_func_80011B94* node) {
    BattlePrepBadgeAward* badge_award;
    s32 medal_index;

    if (callback_phase == 5) {
        badge_award = (BattlePrepBadgeAward*)D_8006F09C->node.callbackArg;
        medal_index = (D_8006F09C->effectSlot - badge_award->displayObjects->effectSlot) % 8;

        gDPPipeSync(gDisplayListHead++);
        gSPSegment(gDisplayListHead++, 0x0F,
                   (u32)Memmap_GetSegmentVaddr(*(s32*)(badge_award->medalTextures + medal_index * 4)) & 0x1FFFFFFF);
        gSPDisplayList(gDisplayListHead++, node->display_node_header.callbackArg);

        GeoRender_ApplyMaterialState();
    }
    return 0;
}

/**
 * Geo draw callback (phase 5) for the smoke puff that pops when a medal appears: binds the puff's
 * current frame of the 8-frame smoke cloud animation (badge_ui D_2023240) to segment 0x0F and
 * advances one frame per draw. After the eighth frame the puff stops drawing (frame -1).
 */
s32 BattlePrepBadgeAward_DrawSmokePuff(s32 callback_phase, unk_func_80011B94* node) {
    BattlePrepBadgeAward* badge_award;
    s32 puff_index;

    if (callback_phase == 5) {
        badge_award = (BattlePrepBadgeAward*)D_8006F09C->node.callbackArg;
        puff_index = (D_8006F09C->effectSlot - badge_award->displayObjects->effectSlot) - 0x10;

        if (badge_award->smokePuffFrames[puff_index] >= 0) {
            gDPPipeSync(gDisplayListHead++);
            gSPSegment(gDisplayListHead++, 0x0F,
                       (u32)Memmap_GetSegmentVaddr(badge_award->smokePuffTextures[badge_award->smokePuffFrames[puff_index]]) & 0x1FFFFFFF);
            gSPDisplayList(gDisplayListHead++, node->display_node_header.callbackArg);

            GeoRender_ApplyMaterialState();

            badge_award->smokePuffFrames[puff_index]++;
            if (badge_award->smokePuffFrames[puff_index] == 8) {
                badge_award->smokePuffFrames[puff_index] = -1;
            }
        }
    }
    return 0;
}

/**
 * Geo draw callback (phase 5) for the ring (texture D_2022640/D_2022A40) that bursts out of the
 * newest medal: each draw grows its scale by 0.5 and fades its blue tint toward transparent. It
 * stops drawing once the scale passes 4.0.
 */
s32 BattlePrepBadgeAward_DrawShockwave(s32 callback_phase, unk_func_80011B94* node) {
    UNUSED s32 pad[2];
    s16 alpha;
    f32 scale;

    if (callback_phase == 5) {
        scale = D_8006F09C->scale.x;
        alpha = (4.5f - scale) * 255.0f * 0.25f;
        if (scale <= 4.0f) {
            scale += 0.5f;
            Vec3f_SetComponentsDuplicate(&D_8006F09C->scale, scale, scale, scale);

            gDPPipeSync(gDisplayListHead++);
            gDPSetEnvColor(gDisplayListHead++, 0, 0, 255, alpha);
            gSPDisplayList(gDisplayListHead++, node->display_node_header.callbackArg);

            GeoRender_ApplyMaterialState();
        }
    }
    return 0;
}

/**
 * Empty function with no callers in battle_prep.
 */
void func_84B0B054(void) {
}

/**
 * Badge award state 1: the eight badge plates flip face-up. Plays sound 0x0110000D on the first
 * frame, then turns slots 0-7 and 8-15 about rotationY by 0x800 per frame for 15 frames, the top
 * row one way and the bottom row the other. On frame 16 it rebuilds the plates face-up in slots
 * 0-7 (rotationY 0; their positions are not set here), frees the turning plates in slots 8-15,
 * clears the smoke puff frames and enters state 2.
 */
void BattlePrepBadgeAward_FlipInStep(BattlePrepBadgeAward* badge_award) {
    s32 i;

    if (badge_award->timer == 0) {
        Audio_PlayCategory11SoundCommand(0x0110000D, 0, 0);
    }

    badge_award->timer++;
    if (badge_award->timer < 0x10) {
        for (i = 0; i < 4; i++) {
            badge_award->displayObjects[0 + i].rotation.y -= 0x800;
            badge_award->displayObjects[8 + i].rotation.y -= 0x800;
        }

        for (i = 4; i < 8; i++) {
            badge_award->displayObjects[0 + i].rotation.y += 0x800;
            badge_award->displayObjects[8 + i].rotation.y += 0x800;
        }
    } else {
        for (i = 0; i < 8; i++) {
            ModelRenderer_ClearDisplayObject(&badge_award->displayObjects[i]);
            BattlePrep_ResetDisplayObject(&badge_award->displayObjects[8 + i]);
            Model_InitDisplayObject(&badge_award->displayObjects[i], 0, 0, badge_award->plateGeo);
            badge_award->displayObjects[i].rotation.y = 0;
            badge_award->displayObjects[i].node.callbackArg = badge_award;
        }

        for (i = 0; i < 8; i++) {
            badge_award->smokePuffFrames[i] = -1;
        }

        badge_award->timer = 0;
        badge_award->state = 2;
    }
}

/**
 * Badge award state 2: medals appear on the plates of the cup rounds already won.
 * Every reveal_interval frames (5 PAL, 6 NTSC) the next earlier round's medal appears in slot
 * 8+n with sound 0x0110000E (parameter n + 1) and a small smoke puff (slot 16+n, scale 2.5).
 * The medal for the round just won (roundOrDemoId - 1) follows newest_medal_delay frames later
 * (17 PAL, 20 NTSC) with sound 0x0110000F, a large smoke puff (scale 6.0) and the shockwave ring
 * (slot 24). At cleanup_delay (34 PAL, 40 NTSC) it frees slots 16-24 and enters the state 3
 * twinkle loop.
 */
void BattlePrepBadgeAward_RevealStep(BattlePrepBadgeAward* badge_award) {
    s16 i;
    s16 medal_index;
#if defined(VERSION_JP)
    // JP has no PAL release, so the NTSC timings are constants.
    enum {
        reveal_interval = 6,
        newest_medal_delay = 20,
        cleanup_delay = 40,
    };
#else
    s16 reveal_interval;
    s16 newest_medal_delay;
    s16 cleanup_delay;
#endif

#if !defined(VERSION_JP)
    if (osTvType == OS_TV_PAL) {
        reveal_interval = 5;
        newest_medal_delay = 0x11;
        cleanup_delay = 0x22;
    } else {
        reveal_interval = 6;
        newest_medal_delay = 0x14;
        cleanup_delay = 0x28;
    }
#endif

    medal_index = badge_award->timer / reveal_interval;
    badge_award->timer++;
    if (gSessionContext.roundOrDemoId - 1 < medal_index) {
        medal_index = gSessionContext.roundOrDemoId - 1;
    }

    if (medal_index < gSessionContext.roundOrDemoId - 1) {
        if ((badge_award->timer % reveal_interval) == 0) {
            Audio_PlayCategory11SoundCommand(0x0110000E, medal_index + 1, 0);
            Model_InitDisplayObject(&badge_award->displayObjects[8 + medal_index], 1, 0, badge_award->medalGeo);
            Vec3f_SetComponentsDuplicate(&badge_award->displayObjects[8 + medal_index].position, D_84B16390[medal_index], D_84B163A0[medal_index],
                                         -289.0f);
            badge_award->displayObjects[8 + medal_index].node.callbackArg = badge_award;
            Model_InitDisplayObject(&badge_award->displayObjects[16 + medal_index], 0, 0, badge_award->smokePuffGeo);
            Vec3f_SetComponentsDuplicate(&badge_award->displayObjects[16 + medal_index].position, D_84B16390[medal_index],
                                         D_84B163A0[medal_index] + 5, -289.0f);
            Vec3f_SetComponentsDuplicate(&badge_award->displayObjects[16 + medal_index].scale, 2.5f, 2.5f, 2.5f);
            badge_award->displayObjects[16 + medal_index].node.callbackArg = badge_award;
            badge_award->smokePuffFrames[medal_index] = 0;
        }
    } else if (medal_index == gSessionContext.roundOrDemoId - 1) {
        if (newest_medal_delay == (badge_award->timer - (medal_index * reveal_interval))) {
            Audio_PlayCategory11SoundCommand(0x0110000F, 0, 0);
            Model_InitDisplayObject(&badge_award->displayObjects[8 + medal_index], 1, 0, badge_award->medalGeo);
            Vec3f_SetComponentsDuplicate(&badge_award->displayObjects[8 + medal_index].position, D_84B16390[medal_index], D_84B163A0[medal_index],
                                         -289.0f);
            badge_award->displayObjects[8 + medal_index].node.callbackArg = badge_award;
            Model_InitDisplayObject(&badge_award->displayObjects[16 + medal_index], 0, 0, badge_award->smokePuffGeo);
            Vec3f_SetComponentsDuplicate(&badge_award->displayObjects[16 + medal_index].position, D_84B16390[medal_index],
                                         D_84B163A0[medal_index] + 5, -289.0f);
            Vec3f_SetComponentsDuplicate(&badge_award->displayObjects[16 + medal_index].scale, 6.0f, 6.0f, 6.0f);
            badge_award->displayObjects[16 + medal_index].node.callbackArg = badge_award;
            badge_award->smokePuffFrames[medal_index] = 0;
            Model_InitDisplayObject(&badge_award->displayObjects[24], 0, 0, badge_award->shockwaveGeo);
            Vec3f_SetComponentsDuplicate(&badge_award->displayObjects[24].position, D_84B16390[medal_index], D_84B163A0[medal_index] + 5,
                                         -289.0f);
            badge_award->displayObjects[24].node.callbackArg = badge_award;

            for (i = 0; i < gSessionContext.roundOrDemoId - 1; i++) {
                badge_award->displayObjects[8 + i].rootId = 0;
            }
        } else {
            if (cleanup_delay == (badge_award->timer - (medal_index * reveal_interval))) {
                for (i = 16; i < 25; i++) {
                    BattlePrep_ResetDisplayObject(&badge_award->displayObjects[i]);
                }

                badge_award->displayObjects[8 + medal_index].rootId = 0;
                badge_award->timer = 0;
                badge_award->state = 3;
            }
        }
    }
}

/**
 * Moves a twinkle sprite (four-point star texture D_2022E40, tinted yellow) to a random point
 * 15-35 units from the center of medal medal_index, building the object on first use, and
 * restarts its pulse (callbackArg = 0). Medals of rounds not reached yet
 * (medal_index >= roundOrDemoId) get no twinkle.
 */
void BattlePrepBadgeAward_SpawnTwinkle(BattlePrepBadgeAward* badge_award, DisplayObject* display_object, s16 medal_index) {
    u16 angle;
    f32 radius;

    if (medal_index < gSessionContext.roundOrDemoId) {
        if (display_object->node.children == NULL) {
            Model_InitDisplayObject(display_object, 2, 0, badge_award->twinkleGeo);
        }

        angle = MathUtil_Random16();
        radius = (MathUtil_Random_ZeroOne() * 20.0f) + 15.0f;

        display_object->node.callbackArg = 0;

        display_object->position.x = D_84B16390[medal_index] + (COSS(angle) * radius);
        display_object->position.y = (D_84B163A0[medal_index] + 5) + (SINS(angle) * radius);
        display_object->position.z = -289.0f;
    }
}

/**
 * Badge award state 3, the looping idle: each frame places one twinkle on a top-row medal (0-3)
 * and one on a bottom-row medal (7-4), and pulses every twinkle's scale through the 12-entry
 * D_84B163B0 curve (0.1 up to 1.0 and back down). BattlePrep_ShowCupResult waits for this state.
 */
void BattlePrepBadgeAward_TwinkleStep(BattlePrepBadgeAward* badge_award) {
    s32 i;
    s32 medal_index = badge_award->timer % 4;

    BattlePrepBadgeAward_SpawnTwinkle(badge_award, &badge_award->displayObjects[16 + badge_award->timer], medal_index);
    BattlePrepBadgeAward_SpawnTwinkle(badge_award, &badge_award->displayObjects[28 + badge_award->timer], 7 - medal_index);

    for (i = 16; i < 41; i++) {
        if ((s32)badge_award->twinkleGeo == (s32)badge_award->displayObjects[i].node.children) {
            s32 scale_index = badge_award->displayObjects[i].node.callbackArg;

            Vec3f_SetComponentsDuplicate(&badge_award->displayObjects[i].scale, D_84B163B0[scale_index], D_84B163B0[scale_index], D_84B163B0[scale_index]);
            badge_award->displayObjects[i].node.callbackArg = (scale_index + 1) % 12;
        }
    }

    badge_award->timer = (badge_award->timer + 1) % 12;
}

/**
 * Badge award state 4: the plate grid flies apart. For 15 frames the top row tips about
 * rotationX and rises while the bottom row tips the other way and falls; every plate also spins
 * about rotationZ and drifts sideways a little faster each frame. Then it returns to idle state 0.
 */
void BattlePrepBadgeAward_ScatterStep(BattlePrepBadgeAward* badge_award) {
    static s16 D_84B16A64[] = { 0x300, 0x150, -0x150, -0x300, -0x300, -0x150, 0x150, 0x300 };
    static f32 D_84B16A74[] = { -3.0f, -2.0f, 2.0f, 3.0f, -3.0f, -2.0f, 2.0f, 3.0f };

    s32 i;

    badge_award->timer++;
    if (badge_award->timer < 0x10) {
        for (i = 0; i < 4; i++) {
            badge_award->displayObjects[0 + i].rotation.x += 0x300;
            badge_award->displayObjects[8 + i].rotation.x += 0x300;
            badge_award->displayObjects[4 + i].rotation.x -= 0x300;
            badge_award->displayObjects[12 + i].rotation.x -= 0x300;

            badge_award->displayObjects[0 + i].position.y += 2.0f * badge_award->timer * 0.5f;
            badge_award->displayObjects[8 + i].position.y += 2.0f * badge_award->timer * 0.5f;
            badge_award->displayObjects[4 + i].position.y -= 2.0f * badge_award->timer * 0.5f;
            badge_award->displayObjects[12 + i].position.y -= 2.0f * badge_award->timer * 0.5f;
        }

        for (i = 0; i < 8; i++) {
            badge_award->displayObjects[0 + i].rotation.z += D_84B16A64[i];
            badge_award->displayObjects[8 + i].rotation.z += D_84B16A64[i];

            badge_award->displayObjects[0 + i].position.x += (badge_award->timer * D_84B16A74[i] * 0.5f);
            badge_award->displayObjects[8 + i].position.x += (badge_award->timer * D_84B16A74[i] * 0.5f);
        }
    } else {
        badge_award->timer = 0;
        badge_award->state = 0;
    }
}

/**
 * Per-frame update for the badge award: 1 flip in, 2 reveal medals, 3 twinkle loop,
 * 4 scatter exit.
 */
void BattlePrepBadgeAward_Update(BattlePrepBadgeAward* badge_award) {
    switch (badge_award->state) {
        case 1:
            BattlePrepBadgeAward_FlipInStep(badge_award);
            break;

        case 2:
            BattlePrepBadgeAward_RevealStep(badge_award);
            break;

        case 3:
            BattlePrepBadgeAward_TwinkleStep(badge_award);
            break;

        case 4:
            BattlePrepBadgeAward_ScatterStep(badge_award);
            break;
    }
}

/**
 * Starts the badge plate grid after a cup win (called by BattlePrep_ShowCupResult). Takes the
 * display object array, builds the eight plates face-down (rotationY -0x8000) in slots 8-15 as a
 * 4x2 grid (x = -96, -32, 32, 96; y = 48 top row, -48 bottom row) and enters state 1.
 */
void BattlePrepBadgeAward_StartFlipIn(BattlePrepBadgeAward* badge_award, DisplayObject* display_objects) {
    s32 i;

    badge_award->displayObjects = display_objects;
    badge_award->timer = 0;
    badge_award->state = 1;

    for (i = 0; i < 8; i++) {
        Model_InitDisplayObject(&badge_award->displayObjects[8 + i], 0, 0, badge_award->plateGeo);
        badge_award->displayObjects[8 + i].rotation.y = -0x8000;
        badge_award->displayObjects[8 + i].node.callbackArg = badge_award;
    }

    Vec3f_SetComponentsDuplicate(&badge_award->displayObjects[8].position, -96.0f, 48.0f, -289.0f);
    Vec3f_SetComponentsDuplicate(&badge_award->displayObjects[9].position, -32.0f, 48.0f, -289.0f);
    Vec3f_SetComponentsDuplicate(&badge_award->displayObjects[10].position, 32.0f, 48.0f, -289.0f);
    Vec3f_SetComponentsDuplicate(&badge_award->displayObjects[11].position, 96.0f, 48.0f, -289.0f);
    Vec3f_SetComponentsDuplicate(&badge_award->displayObjects[12].position, -96.0f, -48.0f, -289.0f);
    Vec3f_SetComponentsDuplicate(&badge_award->displayObjects[13].position, -32.0f, -48.0f, -289.0f);
    Vec3f_SetComponentsDuplicate(&badge_award->displayObjects[14].position, 32.0f, -48.0f, -289.0f);
    Vec3f_SetComponentsDuplicate(&badge_award->displayObjects[15].position, 96.0f, -48.0f, -289.0f);
}

/**
 * Frees the smoke puff, shockwave and twinkle objects (slots 16-40) and starts the state 4
 * scatter exit. No function in battle_prep calls this, so the scatter exit appears unused.
 */
void BattlePrepBadgeAward_StartScatter(BattlePrepBadgeAward* badge_award) {
    s32 i;

    for (i = 16; i < 41; i++) {
        BattlePrep_ResetDisplayObject(&badge_award->displayObjects[i]);
    }

    badge_award->timer = 0;
    badge_award->state = 4;
}

/**
 * Resets the badge award, looks up its three texture tables (plates, medals, smoke frames) and
 * builds its five geo layouts (plate, medal, smoke puff, shockwave ring, twinkle).
 * BattlePrep_InitResultScene calls this for every result screen. The plate grid is only shown
 * when BattlePrep_ShowCupResult calls BattlePrepBadgeAward_StartFlipIn after a cup win.
 */
void BattlePrepBadgeAward_Init(BattlePrepBadgeAward* badge_award) {
    MemoryBlock* memory_block;

    badge_award->state = 0;
    badge_award->timer = 0;
    badge_award->medalTextures = Memmap_GetSegmentVaddr(D_2021620);
    badge_award->plateTextures = Memmap_GetSegmentVaddr(D_2018000);
    badge_award->smokePuffTextures = Memmap_GetSegmentVaddr(D_2023240);

    memory_block = MainPool_AllocState(main_pool_get_available(), 0);

    badge_award->plateGeo = process_geo_layout(memory_block, D_84B16588);
    badge_award->medalGeo = process_geo_layout(memory_block, D_84B166E8);
    badge_award->smokePuffGeo = process_geo_layout(memory_block, D_84B167E0);
    badge_award->shockwaveGeo = process_geo_layout(memory_block, D_84B16948);
    badge_award->twinkleGeo = process_geo_layout(memory_block, D_84B16A38);

    MainPool_FinalizeAllocation(memory_block);
}
