#include "gym_leader_castle_clear.h"
#include "src/geo_render.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/ui_graphics.h"
#include "src/jpeg_stream.h"
#include "src/gfx_buffer.h"
#include "src/gfx_rect.h"
#include "src/matrix.h"
#include "src/controller.h"
#include "src/memmap.h"
#include "src/memory.h"
#include "src/stage_loader.h"

s32 GymLeaderCastleClear_GeoClearTextCallback(s32, GraphNode*);
s32 GymLeaderCastleClear_GeoBannerCallback(s32, GraphNode*);

/**
 * One keyframe of the "CLEAR" text pop-in, read from sClearTextKeyframes.
 * duration is the frame count used to interpolate from this keyframe to the next; -1 marks the final keyframe.
 * scale is the uniform model scale. The prim and env fields are the RGB primitive/environment colors fed to the
 * G_CC_BLENDPEDECALA combiner in sClearTextDL (bright texels take the primitive color, dark texels the
 * environment color).
 */
typedef struct ClearTextKeyframe {
    /* 0x00 */ s16 duration;
    /* 0x04 */ f32 scale;
    /* 0x08 */ s16 primR;
    /* 0x0A */ s16 primG;
    /* 0x0C */ s16 primB;
    /* 0x0E */ s16 envR;
    /* 0x10 */ s16 envG;
    /* 0x12 */ s16 envB;
} ClearTextKeyframe; // size = 0x14

/**
 * Closing iris wipe drawn by GymLeaderCastleClear_DrawIrisWipe.
 * state: 0 = inactive, 1 = closing, 2 = closed (screen fully black).
 * timer counts down from duration; scale shrinks the starburst cutout from 7.0 to 0.0 as timer reaches 0.
 */
typedef struct IrisWipe {
    /* 0x00 */ s16 state;
    /* 0x02 */ s16 timer;
    /* 0x04 */ s16 duration;
    /* 0x08 */ f32 scale;
} IrisWipe; // size = 0xC

/**
 * Alpha ramp for the castle background image.
 * mode: 0 = idle (alpha stays at its initial 192), 1 = ramping, 2 = done (alpha 255, drawn opaque).
 */
typedef struct BackgroundFade {
    /* 0x00 */ s16 mode;
    /* 0x02 */ s16 timer;
    /* 0x04 */ s16 alpha;
} BackgroundFade; // size = 0x8

// Shared `backgrounds` archive; file 14 is the 320x240 castle exterior image.
static BinArchive* sBackgroundArchive;
static u8* sBackgroundImage;
// Screen sequence: 0 = waiting for stage fade-in, 1 = banner/CLEAR animations, 2 = iris wipe closing.
static s16 sSequenceState;
// Frames counted after both animations finish; starts the background ramp at 1 and the iris wipe at 30.
static s16 sHoldTimer;
// Set from gSessionContext.inRound2; enables the "R-2" badge beside the banner.
static s16 sShowRound2Badge;
// Screen position (640x480 space) of the "R-2" badge; follows the banner slide, -100 while hidden.
static s16 sRound2BadgeX;
static s16 sRound2BadgeY;
// Graph roots built from sRootGeoLayout, sBannerGeoLayout and sClearTextGeoLayout.
static GraphNode* sRootGraphNode;
static GraphNode* sBannerGraphNode;
static GraphNode* sClearTextGraphNode;
// [0] = "GYM LEADER CASTLE" banner (kind 1), [1] = "CLEAR" text (kind 2).
static CastleClearObject sScreenObjects[2];
static CastleClearObject* sBannerObject;
static CastleClearObject* sClearTextObject;
static IrisWipe sIrisWipe;
static BackgroundFade sBackgroundFade;

// Two 200x14 quads forming the 200x28 "GYM LEADER CASTLE" banner (top and bottom halves of the lettering).
// Vertex colors give a vertical gradient: top #7832FF, middle #BB288E, bottom #FF1E1E.
static Vtx sBannerVtx[] = {
    VTX(-100, 14, 0, 0, 0, 0x78, 0x32, 0xFF, 0xFF),      VTX(-100, 0, 0, 0, 448, 0xBB, 0x28, 0x8E, 0xFF),
    VTX(100, 0, 0, 6400, 448, 0xBB, 0x28, 0x8E, 0xFF),   VTX(100, 14, 0, 6400, 0, 0x78, 0x32, 0xFF, 0xFF),
    VTX(-100, 0, 0, 0, 0, 0xBB, 0x28, 0x8E, 0xFF),       VTX(-100, -14, 0, 0, 448, 0xFF, 0x1E, 0x1E, 0xFF),
    VTX(100, -14, 0, 6400, 448, 0xFF, 0x1E, 0x1E, 0xFF), VTX(100, 0, 0, 6400, 0, 0xBB, 0x28, 0x8E, 0xFF),
};

// Draws the banner: texels modulated by the vertex-color gradient. Segment 0x0F points at
// gym_leader_castle_clear_gfx, where 0x0000 and 0x0AF0 are IA8 200x14 textures of the upper and lower halves of
// "GYM LEADER CASTLE".
static Gfx sBannerDL[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(SHADE, 0, TEXEL0, 0, 0, 0, 0, TEXEL0, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(sBannerVtx, 8, 0),
    gsDPLoadTextureBlock(0x0F000000, G_IM_FMT_IA, G_IM_SIZ_8b, 200, 14, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureBlock(0x0F000AF0, G_IM_FMT_IA, G_IM_SIZ_8b, 200, 14, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSPEndDisplayList(),
};

// Model layout for the banner; GymLeaderCastleClear_GeoBannerCallback updates and draws it.
static u32 sBannerGeoLayout[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, sBannerVtx,
    0x05000000, 0x22050000, 0x00000000, 0x08000000, GymLeaderCastleClear_GeoBannerCallback,
    0x00000000, 0x06000000, 0x01000000, 0x00000000,
};

// 128x32 quad for the "CLEAR" text.
static Vtx sClearTextVtx[] = {
    VTX(-64, 16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-64, -16, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(64, -16, 0, 4096, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(64, 16, 0, 4096, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};

// Draws "CLEAR" (IA8 128x32 at gym_leader_castle_clear_gfx 0x15E0), colored by primitive/environment color.
static Gfx sClearTextDL[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineMode(G_CC_BLENDPEDECALA, G_CC_PASS2),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(sClearTextVtx, 4, 0),
    gsDPLoadTextureBlock(0x0F000000, G_IM_FMT_IA, G_IM_SIZ_8b, 128, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPEndDisplayList(),
};

// Model layout for the "CLEAR" text; GymLeaderCastleClear_GeoClearTextCallback updates and draws it.
static u32 sClearTextGeoLayout[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, sClearTextVtx,
    0x05000000, 0x22050000, 0x00000000, 0x08000000, GymLeaderCastleClear_GeoClearTextCallback,
    0x00000000, 0x06000000, 0x01000000,
};

// Scene root: 640x480 perspective camera (same command block as other menu scenes) with the
// ModelRenderer display root D_800AC840, so attached CastleClearObjects are rendered.
static u32 sRootGeoLayout[] = {
    0x0C000000,  0x05000000, 0x0B00002D, 0x00000000, 0x028001E0, 0x00000000, 0xFDBD0000,
    0x00000243,  0x05000000, 0x0D000000, 0x05000000, 0x0F000002, 0x05000000, 0x0A000000,
    &D_800AC840, 0x06000000, 0x06000000, 0x06000000, 0x06000000, 0x01000000,
};

// "CLEAR" pop-in: scale 0.0 -> 2.4 over 12 frames while turning red, settles to 2.0 over 4 frames with a white
// face and #C86400 outline, then turns to a #C8C81E face with a black outline over 4 frames and stops.
static ClearTextKeyframe sClearTextKeyframes[] = {
    { 12, 0.0f, 0, 0, 0, 0, 0, 0 },
    { 4, 2.4f, 0xFF, 0, 0, 0, 0, 0 },
    { 4, 2.0f, 0xFF, 0xFF, 0xFF, 200, 100, 0 },
    { -1, 2.0f, 200, 200, 30, 0, 0, 0 },
};

/**
 * Per-frame update for the banner object (called through the geo callback with the current DisplayObject).
 * While animState is 1, slideOffset is pulled toward 0 by a constant acceleration with 0.75 velocity damping,
 * so the banner slides in from the left (offset 1.0 = x -520), overshoots, and settles at x 0. Once the
 * offset crosses zero slowly enough the animation is marked finished (animState 2). Also moves the
 * "R-2" badge so it stays 200 px right of the banner center.
 */
void GymLeaderCastleClear_UpdateBannerSlide(DisplayObject* display_object) {
    CastleClearObject* object = (CastleClearObject*)display_object;
    f32 previous_offset;
    s32 settled;
    f32 screen_x;

    switch (object->animState) {
        case 1:
            if (object->slideOffset >= 0.0f) {
                object->slideVelocity -= 0.065f;
            } else {
                object->slideVelocity += 0.065f;
            }

            settled = 0;
            previous_offset = object->slideOffset;
            object->slideOffset += object->slideVelocity;

            if (((object->slideOffset * previous_offset) < 0.0f) && (fabsf(object->slideVelocity) < 0.13f)) {
                settled = 1;
            }

            if (settled == 0) {
                object->slideVelocity *= 0.75f;
                screen_x = (object->slideOffset * -520.0f) + 320.0f;
            } else {
                object->animState = 2;
                object->timer = 0;
                screen_x = 320.0f;
            }

            object->displayObject.position.x = screen_x - 320.0f;
            sRound2BadgeX = (s16)screen_x + 200;
            sRound2BadgeY = 172;
            break;

        case 0:
        case 2:
        case 3:
            break;
    }
}

/**
 * Geo callback for the banner model. Phase 2 runs GymLeaderCastleClear_UpdateBannerSlide; phase 5 binds
 * gym_leader_castle_clear_gfx to segment 0x0F and emits sBannerDL.
 */
s32 GymLeaderCastleClear_GeoBannerCallback(s32 arg0, UNUSED GraphNode* arg1) {
    switch (arg0) {
        case 0:
            break;

        case 2:
            GymLeaderCastleClear_UpdateBannerSlide(D_8006F09C);
            break;

        case 5:
            gDPPipeSync(gDisplayListHead++);

            gSPSegment(gDisplayListHead++, 0x0F, Memmap_GetSegmentVaddr(D_3000000));
            gSPDisplayList(gDisplayListHead++, sBannerDL);

            GeoRender_ApplyMaterialState();
            break;
    }

    return 0;
}

/**
 * Per-frame update for the "CLEAR" text while animState is 1: linearly interpolates color and scale between
 * the current and next entry of sClearTextKeyframes, advancing to the next segment when timer reaches 0.
 * Reaching the keyframe whose duration is -1 marks the animation finished (animState 2).
 */
void GymLeaderCastleClear_UpdateClearTextKeyframes(CastleClearObject* object) {
    ClearTextKeyframe* from;
    ClearTextKeyframe* to;
    f32 scale;

    switch (object->animState) {
        case 1:
            object->timer--;
            from = &sClearTextKeyframes[object->keyframeIndex];
            to = &sClearTextKeyframes[object->keyframeIndex + 1];

            object->primR = to->primR + (((from->primR - to->primR) * object->timer) / object->duration);
            object->primG = to->primG + (((from->primG - to->primG) * object->timer) / object->duration);
            object->primB = to->primB + (((from->primB - to->primB) * object->timer) / object->duration);

            object->envR = to->envR + (((from->envR - to->envR) * object->timer) / object->duration);
            object->envG = to->envG + (((from->envG - to->envG) * object->timer) / object->duration);
            object->envB = to->envB + (((from->envB - to->envB) * object->timer) / object->duration);

            scale = to->scale + (((from->scale - to->scale) * object->timer) / object->duration);

            if (object->timer <= 0) {
                object->keyframeIndex++;
                from = &sClearTextKeyframes[object->keyframeIndex];

                object->timer = object->duration = from->duration;
                object->primR = from->primR;
                object->primG = from->primG;
                object->primB = from->primB;
                object->envR = from->envR;
                object->envG = from->envG;
                object->envB = from->envB;

                scale = from->scale;

                if (object->timer == -1) {
                    object->duration = 0;
                    object->animState = 2;
                    object->timer = object->duration;
                }
            }

            Vec3f_SetComponentsDuplicate(&object->displayObject.scale, scale, scale, scale);
            break;

        case 0:
        case 2:
        case 3:
            break;
    }
}

/**
 * Geo callback for the "CLEAR" text model. Phase 2 advances its keyframes; phase 5 binds segment 0x0F to the
 * "CLEAR" texture, sets primitive/environment colors from the object, and emits sClearTextDL.
 */
s32 GymLeaderCastleClear_GeoClearTextCallback(s32 arg0, GraphNode* arg1) {
    CastleClearObject* object = (CastleClearObject*)D_8006F09C;

    switch (arg0) {
        case 0:
            break;

        case 2:
            GymLeaderCastleClear_UpdateClearTextKeyframes(object);
            break;

        case 5:
            gDPPipeSync(gDisplayListHead++);

            gSPSegment(gDisplayListHead++, 0x0F, Memmap_GetSegmentVaddr(D_30015E0));
            gDPSetPrimColor(gDisplayListHead++, 0, 0, object->primR, object->primG, object->primB, 0xFF);
            gDPSetEnvColor(gDisplayListHead++, object->envR, object->envG, object->envB, 0xFF);
            gSPDisplayList(gDisplayListHead++, sClearTextDL);

            GeoRender_ApplyMaterialState();
            break;
    }
    return 0;
}

/**
 * Resets the iris wipe to inactive (fully open).
 */
void GymLeaderCastleClear_InitIrisWipe(void) {
    sIrisWipe.state = 0;
    sIrisWipe.timer = 0;
    sIrisWipe.scale = 7.0f;
}

/**
 * Advances a closing iris wipe by one frame; scale = 7.0 * timer / duration, and state becomes 2 at 0.
 */
void GymLeaderCastleClear_UpdateIrisWipe(void) {
    s32 finished;

    switch (sIrisWipe.state) {
        case 1:
            sIrisWipe.timer--;
            sIrisWipe.scale = (sIrisWipe.timer * 7.0f) / sIrisWipe.duration;

            finished = sIrisWipe.timer <= 0;
            if (finished) {
                sIrisWipe.state = 2;
                sIrisWipe.timer = sIrisWipe.duration = 0;
                sIrisWipe.scale = 0.0f;
            }
            break;

        case 0:
        case 2:
            break;
    }
}

/**
 * Draws the iris wipe. The cutout is the 64x64 I4 texture at gym_leader_castle_clear_gfx 0x2C00 (one quadrant
 * of a starburst), mirrored on both axes into a 128x128 starburst scaled by sIrisWipe.scale and centered on
 * screen. The combiner makes the area outside the starburst opaque black, and fill rectangles blacken the
 * screen outside the textured rectangle. When closed, the whole screen is filled black.
 */
void GymLeaderCastleClear_DrawIrisWipe(void) {
    s16 clamped_y1;
    s16 y2;
    s16 clamped_x1;
    s16 x2;
    s16 width;
    s16 height;
    s16 y1;
    s16 x1;
    s32 texture_step;

    Gfx_SetScissorRect(&gDisplayListHead, 0, 0, 640, 480);
    if (sIrisWipe.state == 0) {
        return;
    }

    if ((sIrisWipe.scale <= 0.0f) || (sIrisWipe.state == 2)) {
        gSPDisplayList(gDisplayListHead++, D_8006F4C0);
        gDPSetFillColor(gDisplayListHead++, 0x00010001);
        gDPFillRectangle(gDisplayListHead++, 0, 0, 639, 479);
        return;
    }

    texture_step = ROUND_MAX(1024.0f / sIrisWipe.scale);
    width = ROUND_MAX(128.0f * sIrisWipe.scale);
    height = ROUND_MAX(128.0f * sIrisWipe.scale);

    x1 = (640 - width) / 2;
    y1 = (480 - height) / 2;

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetCombineLERP(gDisplayListHead++, 1, TEXEL0, ENVIRONMENT, 0, 1, TEXEL0, ENVIRONMENT, 0, 1, TEXEL0, ENVIRONMENT,
                      0, 1, TEXEL0, ENVIRONMENT, 0);
    gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 255);
    gDPLoadTextureBlock_4b(gDisplayListHead++, D_3002C00, G_IM_FMT_I, 64, 64, 0, G_TX_MIRROR | G_TX_WRAP,
                           G_TX_MIRROR | G_TX_WRAP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(x1, y1, width, height, 0, 0, texture_step, texture_step, 0);

    clamped_x1 = x1;
    clamped_y1 = y1;

    gSPDisplayList(gDisplayListHead++, D_8006F4C0);
    gDPSetFillColor(gDisplayListHead++, 0x00010001);

    x2 = x1 + width;
    y2 = y1 + height;

    if (x1 < 0) {
        clamped_x1 = 0;
    }

    if (y1 < 0) {
        clamped_y1 = 0;
    }

    if (x2 >= 641) {
        x2 = 640;
    }

    if (y2 >= 481) {
        y2 = 480;
    }

    if (clamped_y1 > 0) {
        gDPFillRectangle(gDisplayListHead++, 0, 0, 639, clamped_y1 - 1);
    }

    if (y2 < 480) {
        gDPFillRectangle(gDisplayListHead++, 0, y2, 639, 479);
    }

    if ((clamped_x1 > 0) && (clamped_y1 >= 0) && (y2 < 481)) {
        gDPFillRectangle(gDisplayListHead++, 0, clamped_y1, clamped_x1 - 1, y2 - 1);
    }

    if ((x2 < 641) && (clamped_y1 >= 0) && (y2 < 481)) {
        gDPFillRectangle(gDisplayListHead++, x2, clamped_y1, 639, y2 - 1);
    }
}

/**
 * Starts closing the iris wipe over `duration` frames (ignored if duration is not positive).
 */
void GymLeaderCastleClear_StartIrisWipe(s16 duration) {
    if (duration > 0) {
        sIrisWipe.state = 1;
        sIrisWipe.timer = sIrisWipe.duration = duration;
        sIrisWipe.scale = 7.0f;
    }
}

/**
 * Resets the background ramp: idle, with the castle image at alpha 192.
 */
void GymLeaderCastleClear_InitBackgroundFade(void) {
    sBackgroundFade.mode = 0;
    sBackgroundFade.timer = 0;
    sBackgroundFade.alpha = 192;
}

/**
 * Advances the background ramp: alpha rises linearly from 192 to 255 over 10 frames.
 */
void GymLeaderCastleClear_UpdateBackgroundFade(void) {
    switch (sBackgroundFade.mode) {
        case 1:
            sBackgroundFade.timer--;
            sBackgroundFade.alpha = ((sBackgroundFade.timer * -0x3F) / 10) + 0xFF;
            if (sBackgroundFade.timer <= 0) {
                sBackgroundFade.mode = 2;
                sBackgroundFade.timer = 0;
                sBackgroundFade.alpha = 0xFF;
            }
            break;

        case 2:
            break;
    }
}

/**
 * Draws a 320x240 RGBA16 image as 16x16 tiles (0x200 bytes each) upscaled 2x to fill 640x480, blended with
 * the given alpha over the cleared framebuffer.
 */
void GymLeaderCastleClear_DrawTranslucentBackground(u8* texture, s16 alpha) {
    s32 i;
    s32 j;

    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, 640, 480);
    gDPSetCombineLERP(gDisplayListHead++, 0, 0, 0, TEXEL0, TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, TEXEL0, TEXEL0, 0,
                      ENVIRONMENT, 0);
    gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, alpha);

    gDPPipeSync(gDisplayListHead++);

    for (i = 0; i < 480; i += 0x20) {
        for (j = 0; j < 640; j += 0x20) {
            gDPLoadTextureBlock(gDisplayListHead++, texture, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 16, 0,
                                G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(gDisplayListHead++, j << 2, i << 2, (j + 0x20) << 2, (i + 0x20) << 2, G_TX_RENDERTILE,
                                0, 0, 0x0200, 0x0200);

            texture += 0x200;
        }
    }
}

/**
 * Draws the castle background image, opaque once the ramp reaches 255 and translucent before that.
 */
void GymLeaderCastleClear_DrawBackground(void) {
    if (sBackgroundFade.alpha < 0xFF) {
        GymLeaderCastleClear_DrawTranslucentBackground(sBackgroundImage, sBackgroundFade.alpha);
    } else {
        Gfx_DrawTiledRgba16Image(sBackgroundImage);
    }
}

/**
 * Sets the background ramp mode; mode 1 restarts the 10-frame ramp from alpha 192.
 */
void GymLeaderCastleClear_StartBackgroundFade(s16 mode) {
    sBackgroundFade.mode = mode;
    if (sBackgroundFade.mode == 1) {
        sBackgroundFade.timer = 10;
        sBackgroundFade.alpha = 192;
    }
}

/**
 * Polls controllers once per frame. This screen does not act on any input.
 */
void GymLeaderCastleClear_ReadInputs(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

/**
 * Sets an object's animState. When starting (state 1), the banner (kind 1) is placed at slideOffset 1.0 with
 * zero velocity, and the "CLEAR" text (kind 2) is reset to the first keyframe's colors and scale.
 */
void GymLeaderCastleClear_StartObjectAnimation(CastleClearObject* object, s16 state) {
    ClearTextKeyframe* first;

    object->animState = state;

    if (object->animState == 1) {
        switch (object->kind) {
            case 1:
                object->slideVelocity = 0.0f;
                object->slideOffset = 1.0f;
                break;

            case 2:
                object->keyframeIndex = 0;
                first = &sClearTextKeyframes[object->keyframeIndex];

                object->timer = object->duration = first->duration;
                object->primR = first->primR;
                object->primG = first->primG;
                object->primB = first->primB;
                object->envR = first->envR;
                object->envG = first->envG;
                object->envB = first->envB;

                Vec3f_SetComponentsDuplicate(&object->displayObject.scale, first->scale, first->scale, first->scale);
                break;
        }
    }
}

/**
 * Emits a scissored textured rectangle from screen-space position/size and texture coordinates/steps.
 */
void GymLeaderCastleClear_DrawScissoredTextureRect(s16 x1, s16 y1, s16 width, s16 height, s16 texture_start_x,
                                                 s16 texture_start_y, s16 texture_spread_x, s16 texture_spread_y) {
    gSPScisTextureRectangle(gDisplayListHead++, x1 << 2, y1 << 2, ((x1 + width) - 1) << 2, ((y1 + height) - 1) << 2, 0,
                            texture_start_x, texture_start_y, texture_spread_x, texture_spread_y);
}

/**
 * Draws the red "R-2" badge (RGBA16 28x28 at gym_leader_castle_clear_gfx 0x25E0) at 1.8x scale, positioned at
 * sRound2BadgeX/sRound2BadgeY beside the banner.
 */
void GymLeaderCastleClear_DrawRound2Badge(void) {
    f32 scale = 1.8f;
    s16 step;
    s16 size;

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    size = ROUND_MAX(28 * scale);
    step = ROUND_MAX(1024.0f / scale);

    gDPLoadTextureBlock(gDisplayListHead++, D_30025E0, G_IM_FMT_RGBA, G_IM_SIZ_16b, 28, 28, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    GymLeaderCastleClear_DrawScissoredTextureRect(sRound2BadgeX, sRound2BadgeY, size, size, 0, 0, step, step);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

/**
 * Resets the sequence, hides the badge offscreen, chooses whether to show it (Round 2 only), and resets both fades.
 */
void GymLeaderCastleClear_InitState(void) {
    sSequenceState = 0;
    if (gSessionContext.inRound2 != 0) {
        sShowRound2Badge = 1;
    } else {
        sShowRound2Badge = 0;
    }
    sRound2BadgeY = -100;
    sRound2BadgeX = sRound2BadgeY;
    GymLeaderCastleClear_InitIrisWipe();
    GymLeaderCastleClear_InitBackgroundFade();
}

/**
 * Draws one frame: castle background, banner and "CLEAR" models, optional "R-2" badge, then the iris wipe.
 */
void GymLeaderCastleClear_Draw(void) {
    BgStage_DrawFrame();
    GymLeaderCastleClear_DrawBackground();
    Geo_RenderRootNode(sRootGraphNode);
    if (sShowRound2Badge != 0) {
        GymLeaderCastleClear_DrawRound2Badge();
    }
    GymLeaderCastleClear_DrawIrisWipe();
    BgStage_AdvanceFrame();
}

/**
 * Advances the screen sequence once per frame and returns 0 when the screen is finished.
 * After the stage fade-in completes, the banner slides in; once it settles the "CLEAR" text pops in.
 * When both are finished, the background ramps to full brightness, and 30 frames later the iris wipe closes
 * over 15 frames with the clear color set to black. The screen ends when the wipe is fully closed.
 */
s32 GymLeaderCastleClear_AdvanceState(void) {
    s32 keep_running = 1;

    switch (sSequenceState) {
        case 0:
            if (StageContext_GetFadeMode() == 0) {
                sSequenceState = 1;
                sHoldTimer = 0;
                GymLeaderCastleClear_StartObjectAnimation(sBannerObject, 1);
            }
            break;

        case 1:
            if (sBannerObject->animState == 2) {
                switch (sClearTextObject->animState) {
                    case 0:
                        GymLeaderCastleClear_StartObjectAnimation(sClearTextObject, 1);
                        break;

                    case 2:
                        sHoldTimer += 1;
                        if (sHoldTimer == 1) {
                            GymLeaderCastleClear_StartBackgroundFade(1);
                        }

                        if (sHoldTimer >= 30) {
                            sSequenceState = 2;
                            sHoldTimer = 0;
                            GymLeaderCastleClear_StartIrisWipe(0xF);
                            StageContext_SetClearColor(1);
                        }
                        break;
                }
            }
            break;

        case 2:
            if (sIrisWipe.state == 2) {
                keep_running = 0;
            }
            break;
    }

    return keep_running;
}

/**
 * Runs the screen's frame loop until GymLeaderCastleClear_AdvanceState reports completion.
 */
void GymLeaderCastleClear_RunLoop(void) {
    GymLeaderCastleClear_InitState();
    StageFade_StartFromOpaque(0xF);

    do {
        GymLeaderCastleClear_ReadInputs();
        GymLeaderCastleClear_UpdateIrisWipe();
        GymLeaderCastleClear_UpdateBackgroundFade();
        GymLeaderCastleClear_Draw();
    } while (GymLeaderCastleClear_AdvanceState() != 0);
}

/**
 * Builds the three graph roots and attaches the two display objects: the banner at (-520, 44, -579), scale 2.0,
 * and the "CLEAR" text at (0, -24, -579), initially scale 0.
 */
void GymLeaderCastleClear_InitGraphics(void) {
    CastleClearObject* object;
    MemoryBlock* memory_block = MainPool_AllocState(main_pool_get_available(), 0);
    s32 i;

    sRootGraphNode = process_geo_layout(memory_block, sRootGeoLayout);
    sBannerGraphNode = process_geo_layout(memory_block, sBannerGeoLayout);
    sClearTextGraphNode = process_geo_layout(memory_block, sClearTextGeoLayout);

    MainPool_FinalizeAllocation(memory_block);
    ModelRenderer_InitDisplayRoots();

    for (i = 0; i < 2; i++) {
        object = &sScreenObjects[i];

        ModelRenderer_AttachDisplayObject(&object->displayObject);

        object->primR = object->primG = object->primB = 0;
        object->envR = object->envG = object->envB = 0;
        object->kind = 0;
        object->animState = 0;
        object->timer = 0;
        object->duration = 0;
        object->keyframeIndex = 0;
        object->slideOffset = 0.0f;
        object->slideVelocity = 0.0f;

        switch (i) {
            case 0:
                Vec3f_SetComponentsDuplicate(&object->displayObject.position, -520.0f, 44.0f, -579.0f);
                Vec3f_SetComponentsDuplicate(&object->displayObject.scale, 2.0f, 2.0f, 2.0f);
                Model_InitDisplayObject(&object->displayObject, 0, 0, sBannerGraphNode);
                object->kind = 1;
                sBannerObject = object;
                break;

            case 1:
                Vec3f_SetComponentsDuplicate(&object->displayObject.position, 0.0, -24.0f, -579.0f);
                Vec3f_SetComponentsDuplicate(&object->displayObject.scale, 0.0f, 0.0f, 0.0f);
                Model_InitDisplayObject(&object->displayObject, 0, 0, sClearTextGraphNode);
                object->kind = 2;
                sClearTextObject = object;
                break;
        }
    }
}

/**
 * Entry point of the "GYM LEADER CASTLE CLEAR" screen. Called by State_GymLeaderCastle after the final
 * Gym Leader Castle battle is won (transitionFlags 0x200), before the demo scenes that follow.
 * Loads the shared menu UI, this fragment's graphics, and background image 14 (castle exterior).
 * //@src/game_state.c
 */
s32 GymLeaderCastleClear_Main(UNUSED s32 arg0, UNUSED s32 arg1) {
    StageContext* stage_context;

    main_pool_push_state('CLRG');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    stage_context = StageContext_Allocate(1, 0, 2, 0, 2, 1);
    Font_Init(0x10, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_3000000, gym_leader_castle_clear_gfx, 0);
    sBackgroundArchive = ASSET_LOAD2(backgrounds, 1, 1);
    sBackgroundImage = BinArchive_GetFile(sBackgroundArchive, 14);

    GymLeaderCastleClear_InitGraphics();
    StageContext_Activate(stage_context);
    GymLeaderCastleClear_RunLoop();
    StageLoader_RunFrames(2);
    StageContext_Deactivate();
    Font_Free();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('CLRG');

    return 0;
}
