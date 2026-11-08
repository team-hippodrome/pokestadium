#include "credits.h"
#include "src/geo_node.h"
#include "src/geo_render.h"
#include "src/geo_layout.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/text_system.h"
#include "src/jpeg_stream.h"
#include "src/audio_loop_point.h"
#include "src/audio_category_fade.h"
#include "src/gfx_buffer.h"
#include "src/gfx_rect.h"
#include "src/matrix.h"
#include "src/controller.h"
#include "src/dp_intro.h"
#include "src/fragments/particle_math/particle_math.h"
#include "src/memory.h"
#include "src/stage_loader.h"

/**
 * Staff-roll color keyframe read by Credits_InterpolateRoleColor. colors[0]/[1] are the
 * env/prim colors for role headings and colors[2]/[3] those for staff names.
 */
typedef struct CreditsRoleColorKeyframe {
    /* 0x00 */ u16 frame; // page timer value at which this keyframe applies
    /* 0x02 */ u8 alpha;
    /* 0x03 */ Color_RGB8 colors[4];
} CreditsRoleColorKeyframe; // size = 0x10

// Fog node of sArenaGeoLayout; callbackArg is 1 when the arena supplied fog parameters.
static unk_D_8690A610 sArenaFog;
// Containers for the geo layouts returned by arena archive entries 0, 1 and 3.
static struct GraphNode sArenaLayout0;
static struct GraphNode sArenaLayout1;
static struct GraphNode sArenaLayout3;
static s8 sArenaIndex; // stadium_models arena file shown behind the roll; -1 = none
static u8 sCameraShot; // 0-5 orbit, 6-8 pivot orbit, 9-14 keyframe blend
static BinArchive* sStadiumModelArchive; // stadium_models; entry N is battle arena N
static Color_RGBA8_u32 sArenaTintColor; // copied to textureTint by Credits_BackgroundNodePostCallback
static GraphNode* sArenaRoot;
static CameraNode* sArenaCamera;
// Orbit camera state passed to Camera_ComputeEyeFromAngles by Credits_UpdateCamera.
static s16 sCameraRotationX;
static s16 sCameraRotationY;
static s16 sCameraDistance;
static s32 pad_D_86A061F8[2];
static FontContext* sFontContext;
static s32 pad_D_86A06208[2];
u8 sFrameDelta;
u8 sRoleTextColumn;
u8 sFrameDeltaPrimed; // 0 until Credits_UpdateFrameDelta has a previous retrace count
CreditsRolePage* sCurrentRolePage; // NULL after the last page; the end logo is drawn instead
u32 sLastRetraceCount; // D_800A62E0.unk_A34 at the previous Credits_UpdateFrameDelta

// Arena backdrop schedule: eight timed segments (1320 or 1080 frames, 9120 in total, the
// same as the sRolePages durations) and an end marker.
// Segment 3 also reloads a new arena at 1/3 and 2/3 of its duration (Credits_MainLoop).
CreditsBackgroundSegment sBackgroundSegments[9] = {
    {
        0x00,
        0x00,
        0x0528,
    },
    {
        0x00,
        0x01,
        0x0438,
    },
    {
        0x00,
        0x02,
        0x0438,
    },
    {
        0x00,
        0x03,
        0x0438,
    },
    {
        0x00,
        0x04,
        0x0438,
    },
    {
        0x00,
        0x05,
        0x0438,
    },
    {
        0x00,
        0x06,
        0x0528,
    },
    {
        0x00,
        0x07,
        0x0438,
    },
    {
        0xFF,
        0x00,
        0x0000,
    },
};
u8 sSegmentIndex = 0; // index into sBackgroundSegments
// Value from arena archive entry 2 (the battle backdrop). Credits_DrawFrame fills the frame
// with it when below 0x10000 (RGBA5551), with 1 when 0 or -1, and with 0xA6BF when it is a
// texture address. //@src/fragments/battle_engine/battle_engine_main.c Battle_DrawStageBackdrop
u32 sArenaBackdrop = 0;
u16 sSegmentTimer = 0; // frames elapsed in the current backdrop segment
// Root geo layout: camera, fog, then three arena layout containers each wrapped by
// Credits_BackgroundNodePostCallback, then the model renderer display root D_800AC840
// (reset by ModelRenderer_InitDisplayRoots). //@src/model_renderer.c
u32 sArenaGeoLayout[] = {
    0x0C00FFFF,
    0x05000000,
    0x0B00001E,
    0x00000000,
    0x014000F0,
    0x0000000F,
    0x00000000,
    0x00000000,
    0x05000000,
    0x0D000000,
    0x05000000,
    0x07000000,
    &sArenaFog,
    0x14000000,
    0x002D0019,
    0xFFFFFF28,
    0x16646464,
    0x0F000002,
    0x05000000,
    0x1F00FFFF,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00640064,
    0x00640000,
    0x08000000,
    Credits_BackgroundNodePostCallback,
    0x00000000,
    0x05000000,
    0x07000000,
    &sArenaLayout0,
    0x06000000,
    0x06000000,
    0x0F000003,
    0x05000000,
    0x1F00FFFF,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00640064,
    0x00640000,
    0x08000000,
    Credits_BackgroundNodePostCallback,
    0x00000000,
    0x05000000,
    0x07000000,
    &sArenaLayout1,
    0x06000000,
    0x06000000,
    0x0F000002,
    0x05000000,
    0x1F00FFFF,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00640064,
    0x00640000,
    0x08000000,
    Credits_BackgroundNodePostCallback,
    0x00000000,
    0x05000000,
    0x07000000,
    &sArenaLayout3,
    0x06000000,
    0x06000000,
    0x0F000003,
    0x05000000,
    0x0A000000,
    &D_800AC840,
    0x06000000,
    0x06000000,
    0x06000000,
    0x06000000,
    0x01000000,
};

/**
 * Geo callback wrapped around each arena layout container in sArenaGeoLayout. On event 2 it
 * copies sArenaTintColor into the node's textureTint, matching the battle engine's stage tint
 * callback. //@src/fragments/battle_engine/battle_engine_main.c Battle_StageMaterialTintCallback
 */
s32 Credits_BackgroundNodePostCallback(s32 event, GraphNode* node) {
    DisplayObject* object = (DisplayObject*)node;

    if (event == 2) {
        object->textureTint.rgba = sArenaTintColor.rgba;
    }
    return 0;
}

/**
 * Sets the arena texture tint. A non-white tint disables fog; white re-enables it only when
 * the arena supplied fog parameters (sArenaFog.node.callbackArg == 1).
 * Same operation as Battle_SetStageTintColor. //@src/fragments/battle_engine/battle_engine_main.c
 */
void Credits_SetBackgroundTint(u8 r, u8 g, u8 b) {
    u8 white_mask = r & g & b;

    sArenaTintColor.r = r;
    sArenaTintColor.g = g;
    sArenaTintColor.b = b;

    if (white_mask == 0xFF) {
        if (sArenaFog.node.callbackArg == 1) {
            sArenaFog.node.flags |= 1;
        }
    } else {
        sArenaFog.node.flags &= ~1;
    }
}

/**
 * Loads battle arena `arena_index` from stadium_models as the staff-roll backdrop: entries 0,
 * 1 and 3 are geo layouts for the three layout containers, entry 2 is the backdrop value and
 * entry 4 optional fog parameters (same selectors as the battle engine's arena load). Then
 * starts the camera move `shot` for the current segment: 0-8 orbit blends, 9-14 keyframe
 * blends (40 frames shorter in segment 7), and any other value (0xFF) keeps the current move
 * but restores its target height and distance. Waits for the frame in flight first.
 */
void Credits_LoadBackgroundScene(s8 arena_index, u8 shot) {
    MemoryBlock* layout_pool;
    FragmentEntry arena_entry;
    u32* arena_layout;
    unk_D_8690A610_018* fog_parameters;

    while (Display_IsFrameReady() == 0) {}

    GeoNode_CreateContainer(NULL, &sArenaLayout0);
    GeoNode_CreateContainer(NULL, &sArenaLayout1);
    GeoNode_CreateContainer(NULL, &sArenaLayout3);

    main_pool_pop_state('BACK');

    main_pool_push_state('BACK');

    arena_entry = BinArchive_GetFile(sStadiumModelArchive, arena_index);
    layout_pool = MainPool_AllocState(main_pool_get_available(), 0);

    arena_layout = arena_entry(0, 0);
    if (arena_layout != NULL) {
        GraphNode_AppendChild(&sArenaLayout0, process_geo_layout(layout_pool, arena_layout));
    }

    arena_layout = arena_entry(1, 0);
    if (arena_layout != NULL) {
        GraphNode_AppendChild(&sArenaLayout1, process_geo_layout(layout_pool, arena_layout));
    }

    arena_layout = arena_entry(3, 0);
    if (arena_layout != NULL) {
        GraphNode_AppendChild(&sArenaLayout3, process_geo_layout(layout_pool, arena_layout));
    }

    sArenaBackdrop = arena_entry(2, 0);
    MainPool_FinalizeAllocation(layout_pool);

    fog_parameters = arena_entry(4, 0);
    if (fog_parameters == NULL) {
        sArenaFog.node.callbackArg = 0;
        sArenaFog.node.flags &= ~1;
    } else {
        sArenaFog.unk_18.unk_00 = fog_parameters->unk_00;
        sArenaFog.unk_18.unk_02 = fog_parameters->unk_02;
        sArenaFog.unk_18.unk_04.rgba = fog_parameters->unk_04.rgba;
        sArenaFog.node.callbackArg = 1;
    }

    Credits_SetBackgroundTint(0xFF, 0xFF, 0xFF);

    if (shot < 6) {
        Credits_InitCameraOrbitBlend(&sCameraRotationX, &sCameraRotationY, &sCameraDistance, sArenaCamera, sBackgroundSegments[sSegmentIndex].duration, arena_index, shot);
    } else if (shot < 9) {
        Credits_InitCameraOrbitBlend(&sCameraRotationX, &sCameraRotationY, &sCameraDistance, sArenaCamera, sBackgroundSegments[sSegmentIndex].duration, arena_index, shot);
    } else if (shot < 0xF) {
        if (sSegmentIndex == 7) {
            Credits_InitCameraKeyframeBlend(sArenaCamera, sBackgroundSegments[sSegmentIndex].duration - 0x28, arena_index, shot);
        } else {
            Credits_InitCameraKeyframeBlend(sArenaCamera, sBackgroundSegments[sSegmentIndex].duration, arena_index, shot);
        }
    } else {
        sArenaCamera->view.targetPosition.y = sOrbitShots[arena_index][sCameraShot].targetHeight;
        sCameraDistance = sOrbitShots[arena_index][sCameraShot].distance;
    }
}

/**
 * Reads the controllers once per frame; Credits_MainLoop checks gPlayer1Controller to skip
 * the end logo.
 */
void Credits_PollInput(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

/**
 * Recomputes the camera from the orbit state. Shots 0-5 place the eye around the look-at point
 * at sCameraDistance; shots 6-8 place both eye and target on the same ray from sOrbitPivot
 * (target 500 units farther), so the camera looks outward from the pivot. Keyframe shots
 * (9-14) are left to Credits_StepCameraKeyframeBlend.
 */
void Credits_UpdateCamera(void) {
    if (sCameraShot < 6) {
        Camera_ComputeEyeFromAngles(&sArenaCamera->view.targetPosition, &sArenaCamera->view.eyePosition, sCameraDistance,
                                    sCameraRotationX, sCameraRotationY);
    } else if (sCameraShot < 9) {
        Camera_ComputeEyeFromAngles(&sOrbitPivot, &sArenaCamera->view.eyePosition, sCameraDistance, sCameraRotationX, sCameraRotationY);
        Camera_ComputeEyeFromAngles(&sOrbitPivot, &sArenaCamera->view.targetPosition, sCameraDistance + 500.0f, sCameraRotationX,
                                    sCameraRotationY);
    }
}

/**
 * Draws a full-screen black rectangle that fades the backdrop in over the first 8 frames of a
 * segment and out over its last 40 frames. Nothing is drawn after the end marker.
 */
void Credits_DrawFadeOverlay(void) {
    u8 alpha;

    if (sBackgroundSegments[sSegmentIndex].endMarker != -1) {
        if (sSegmentTimer < 8) {
            alpha = (s8)ROUND_MAX(((-(f32)sSegmentTimer / 8) + 1.0f) * 255.0f);
            if (alpha != 0) {
                Gfx_FillRectWithDisplayState(0, 0, 0x140, 0xF0, 0, 0, 0, alpha);
            }
        } else {
            if ((sBackgroundSegments[sSegmentIndex].duration - 0x28) < sSegmentTimer) {
                s32 duration = sBackgroundSegments[sSegmentIndex].duration;

                alpha = (s32)ROUND_MAX((((sSegmentTimer - duration) + 0x28) * 255.0f) / 40.0f);
                if (alpha != 0) {
                    Gfx_FillRectWithDisplayState(0, 0, 0x140, 0xF0, 0, 0, 0, alpha);
                }
            }
        }
    }
}

/**
 * Draws the 2D layer over the arena for draw modes 1 (main roll) and 2 (outro): the backdrop
 * fade, then the staff roll, which is held back for the first 240 frames of segment 0.
 */
void Credits_DrawFadeAndRoleList(u8 draw_mode) {
    if ((draw_mode == 1) || (draw_mode == 2)) {
        Credits_DrawFadeOverlay();
        if ((sSegmentIndex != 0) || (sSegmentTimer >= 0xF0)) {
            Credits_DrawRoleList();
        }
    }
}

/**
 * Renders one frame. The frame is first filled from sArenaBackdrop (see its declaration).
 * Draw mode 0 shows only the Pokemon Stadium logo (intro); modes 1 and 2 render the arena
 * scene graph and then Credits_DrawFadeAndRoleList.
 */
void Credits_DrawFrame(u8 draw_mode) {
    BgStage_DrawFrame();

    if ((sArenaBackdrop == -1) || (sArenaBackdrop == 0)) {
        GfxImage_FillCurrent(&gDisplayListHead, 1);
    } else if (sArenaBackdrop < 0x10000) {
        GfxImage_FillCurrent(&gDisplayListHead, sArenaBackdrop);
    } else {
        GfxImage_FillCurrent(&gDisplayListHead, 0xA6BF);
    }

    if (draw_mode == 0) {
        Credits_DrawStadiumLogo(0);
    } else {
        Geo_RenderRootNode(sArenaRoot);
        Credits_DrawFadeAndRoleList(draw_mode);
    }

    BgStage_AdvanceFrame();
}

/**
 * Intro: fades out all audio, then shows the Pokemon Stadium logo for 240 frames: a 30-frame
 * fade in from black, a 90-frame hold and a 120-frame fade to black.
 */
void Credits_IntroLoop(void) {
    u8 i;
    u8 j;
    u8 phase_frames;
    s32 frame_count;

    Audio_FadeOutAll(0x1E);

    for (i = 0; i < 3; i++) {
        frame_count = phase_frames;

        switch (i) {
            case 0:
                phase_frames = 0x1E;
                StageFade_StartFromOpaque(phase_frames);
                frame_count = 0x1E;
                break;

            case 1:
                phase_frames = 0x5A;
                frame_count = phase_frames;
                break;

            case 2:
                phase_frames = 0x78;
                StageFade_StartFromTransparent(phase_frames);
                frame_count = phase_frames;
                break;

            default:
                frame_count = phase_frames;
                break;
        }

        for (j = 0; j < frame_count; j++) {
            Credits_PollInput();
            Credits_DrawFrame(0);
        }
    }
}

/**
 * Main staff-roll loop. Starts music track 0x42 when `is_round_1` is set and 0x43 otherwise,
 * then each frame advances sFrameDelta, the camera move and the backdrop segment timer.
 * When a segment ends it picks and loads the next arena (segment 3 also swaps arenas at 1/3
 * and 2/3 of its length keeping the camera move); after the end marker the arena is cleared.
 * Exits 721 frames after the end logo is fully opaque, or after 361 frames on A or Start.
 */
void Credits_MainLoop(s32 is_round_1) {
    // Extra arena swaps already made during segment 3 (at most 2).
    static u8 sSegment3ReloadCount = 0;

    s32 running = 1;

    StageLoader_RunFrames(0x3C);
    StageFade_StartFromOpaque(0xB4);

    if (is_round_1 != 0) {
        Audio_StartMusicTrack(0x42);
    } else {
        Audio_StartMusicTrack(0x43);
    }

    Credits_SelectNextScene(&sArenaIndex, &sCameraShot);
    Credits_LoadBackgroundScene(sArenaIndex, sCameraShot);

    while (running != 0) {
        Credits_UpdateFrameDelta();
        Particle_UpdateFrameCounters();

        if (sBackgroundSegments[sSegmentIndex].duration != 0) {
            if (sBackgroundSegments[sSegmentIndex].endMarker != -1) {
                if (sCameraShot < 9) {
                    Credits_StepCameraOrbitBlend();
                } else {
                    Credits_StepCameraKeyframeBlend();
                }
            }

            if (sSegmentTimer >= sBackgroundSegments[sSegmentIndex].duration) {
                sSegmentIndex++;
                if (sBackgroundSegments[sSegmentIndex].duration != 0) {
                    sBackgroundSegments[sSegmentIndex].duration =
                        sBackgroundSegments[sSegmentIndex - 1].duration + (sBackgroundSegments[sSegmentIndex].duration - sSegmentTimer);
                }

                sSegmentTimer = 0;

                if (sBackgroundSegments[sSegmentIndex].endMarker != -1) {
                    Credits_SelectNextScene(&sArenaIndex, &sCameraShot);
                    Credits_LoadBackgroundScene(sArenaIndex, sCameraShot);
                } else {
                    sArenaIndex = -1;
                    sArenaBackdrop = 0;

                    GeoNode_CreateContainer(NULL, &sArenaLayout0);
                    GeoNode_CreateContainer(NULL, &sArenaLayout1);
                    GeoNode_CreateContainer(NULL, &sArenaLayout3);

                    sArenaFog.node.flags &= ~1;
                    if (sBackgroundSegments[sSegmentIndex].duration == 0) {
                        main_pool_pop_state('BACK');
                    }
                }
            } else if ((sSegmentIndex == 3) && (sSegment3ReloadCount < 2) &&
                       (sSegmentTimer >= (((sBackgroundSegments[sSegmentIndex].duration * (sSegment3ReloadCount + 1)) / 3) - 1))) {
                Credits_SelectNextScene(&sArenaIndex, &sCameraShot);
                Credits_LoadBackgroundScene(sArenaIndex, 0xFF);
                sSegment3ReloadCount++;
            }
        }

        Credits_PollInput();
        Credits_UpdateCamera();

        if (sLogoHoldTimer >= 0x2D1) {
            running = 0;
        } else if ((sLogoHoldTimer >= 0x169) &&
                   ((gPlayer1Controller->buttonPressed & 0x1000) || (gPlayer1Controller->buttonPressed & 0x8000))) {
            running = 0;
        }

        Credits_DrawFrame(1);

        if (sBackgroundSegments[sSegmentIndex].duration != 0) {
            sSegmentTimer += sFrameDelta;
        }
    }
}

static s32 pad_D_86A026FC = 0;
// Page color/alpha keyframes: invisible at frame 0, opaque with lighter colors at frame 8,
// settled colors from frame 30, then a 20-frame fade out. Frames for keyframes 3 and 4 are
// rewritten per page by Credits_InterpolateRoleColor.
CreditsRoleColorKeyframe sRoleColorKeyframes[] = {
    {
        0,
        0,
        {
            { 0xFF, 0, 0xE6 }, // #FF00E6
            { 0xFF, 0xC8, 0xFF }, // #FFC8FF
            { 0, 0x46, 0xFF }, // #0046FF
            { 0x64, 0xFF, 0xFF }, // #64FFFF
        },
    },
    {
        8,
        0xFF,
        {
            { 0xFF, 0xC8, 0xFF }, // #FFC8FF
            { 0xFF, 0xFF, 0xFF }, // #FFFFFF
            { 0x64, 0xFF, 0xFF }, // #64FFFF
            { 0xFF, 0xFF, 0xFF }, // #FFFFFF
        },
    },
    {
        0x1E,
        0xFF,
        {
            { 0xFF, 0, 0xFF }, // #FF00FF
            { 0xFF, 0xC8, 0xFF }, // #FFC8FF
            { 0, 0x46, 0xFF }, // #0046FF
            { 0x64, 0xFF, 0xFF }, // #64FFFF
        },
    },
    {
        0x153,
        0xFF,
        {
            { 0xFF, 0, 0xFF }, // #FF00FF
            { 0xFF, 0xC8, 0xFF }, // #FFC8FF
            { 0, 0x46, 0xFF }, // #0046FF
            { 0x64, 0xFF, 0xFF }, // #64FFFF
        },
    },
    {
        0x167,
        0,
        {
            { 0xFF, 0, 0xE6 }, // #FF00E6
            { 0xFF, 0xC8, 0xFF }, // #FFC8FF
            { 0, 0x46, 0xFF }, // #0046FF
            { 0x64, 0xFF, 0xFF }, // #64FFFF
        },
    },
};

// The 187 staff-roll lines in display order, one per string of textdata table 45 (US 1.0
// assets/us/textdata/45.bin). Entry 0 is "DIRECTOR" / "Takao Shimizu" ...; the last is
// "EXECUTIVE PRODUCER" / "Hiroshi Yamauchi". Style 2 entries are blank spacers whose table
// strings are empty (lines 126 and 155).
static CreditsRoleLine sRoleLines[] = {
    {
        0,
        { &D_86A05B60, &D_86A05B64 },
    },
    {
        1,
        { &D_86A05B68, &D_86A05B6C },
    },
    {
        0,
        { &D_86A05B70, &D_86A05B74 },
    },
    {
        1,
        { &D_86A05B78, &D_86A05B7C },
    },
    {
        0,
        { &D_86A05B80, &D_86A05B84 },
    },
    {
        1,
        { &D_86A05B88, &D_86A05B8C },
    },
    {
        0,
        { &D_86A05B90, &D_86A05B94 },
    },
    {
        1,
        { &D_86A05B98, &D_86A05B9C },
    },
    {
        0,
        { &D_86A05BA0, &D_86A05BA4 },
    },
    {
        1,
        { &D_86A05BA8, &D_86A05BAC },
    },
    {
        0,
        { &D_86A05BB0, &D_86A05BB4 },
    },
    {
        1,
        { &D_86A05BB8, &D_86A05BBC },
    },
    {
        0,
        { &D_86A05BC0, &D_86A05BC4 },
    },
    {
        1,
        { &D_86A05BC8, &D_86A05BCC },
    },
    {
        0,
        { &D_86A05BD0, &D_86A05BD4 },
    },
    {
        1,
        { &D_86A05BD8, &D_86A05BDC },
    },
    {
        0,
        { &D_86A05BE0, &D_86A05BE4 },
    },
    {
        1,
        { &D_86A05BE8, &D_86A05BEC },
    },
    {
        1,
        { &D_86A05BF0, &D_86A05BF4 },
    },
    {
        1,
        { &D_86A05BF8, &D_86A05BFC },
    },
    {
        0,
        { &D_86A05C00, &D_86A05C04 },
    },
    {
        1,
        { &D_86A05C08, &D_86A05C0C },
    },
    {
        0,
        { &D_86A05C10, &D_86A05C14 },
    },
    {
        1,
        { &D_86A05C18, &D_86A05C1C },
    },
    {
        0,
        { &D_86A05C20, &D_86A05C24 },
    },
    {
        1,
        { &D_86A05C28, &D_86A05C2C },
    },
    {
        0,
        { &D_86A05C30, &D_86A05C34 },
    },
    {
        1,
        { &D_86A05C38, &D_86A05C3C },
    },
    {
        1,
        { &D_86A05C40, &D_86A05C44 },
    },
    {
        1,
        { &D_86A05C48, &D_86A05C4C },
    },
    {
        1,
        { &D_86A05C50, &D_86A05C54 },
    },
    {
        1,
        { &D_86A05C58, &D_86A05C5C },
    },
    {
        0,
        { &D_86A05C60, &D_86A05C64 },
    },
    {
        1,
        { &D_86A05C68, &D_86A05C6C },
    },
    {
        1,
        { &D_86A05C70, &D_86A05C74 },
    },
    {
        1,
        { &D_86A05C78, &D_86A05C7C },
    },
    {
        1,
        { &D_86A05C80, &D_86A05C84 },
    },
    {
        0,
        { &D_86A05C88, &D_86A05C8C },
    },
    {
        1,
        { &D_86A05C90, &D_86A05C94 },
    },
    {
        1,
        { &D_86A05C98, &D_86A05C9C },
    },
    {
        0,
        { &D_86A05CA0, &D_86A05CA4 },
    },
    {
        1,
        { &D_86A05CA8, &D_86A05CAC },
    },
    {
        1,
        { &D_86A05CB0, &D_86A05CB4 },
    },
    {
        1,
        { &D_86A05CB8, &D_86A05CBC },
    },
    {
        1,
        { &D_86A05CC0, &D_86A05CC4 },
    },
    {
        1,
        { &D_86A05CC8, &D_86A05CCC },
    },
    {
        1,
        { &D_86A05CD0, &D_86A05CD4 },
    },
    {
        1,
        { &D_86A05CD8, &D_86A05CDC },
    },
    {
        1,
        { &D_86A05CE0, &D_86A05CE4 },
    },
    {
        0,
        { &D_86A05CE8, &D_86A05CEC },
    },
    {
        1,
        { &D_86A05CF0, &D_86A05CF4 },
    },
    {
        1,
        { &D_86A05CF8, &D_86A05CFC },
    },
    {
        1,
        { &D_86A05D00, &D_86A05D04 },
    },
    {
        1,
        { &D_86A05D08, &D_86A05D0C },
    },
    {
        1,
        { &D_86A05D10, &D_86A05D14 },
    },
    {
        1,
        { &D_86A05D18, &D_86A05D1C },
    },
    {
        1,
        { &D_86A05D20, &D_86A05D24 },
    },
    {
        1,
        { &D_86A05D28, &D_86A05D2C },
    },
    {
        0,
        { &D_86A05D30, &D_86A05D34 },
    },
    {
        1,
        { &D_86A05D38, &D_86A05D3C },
    },
    {
        1,
        { &D_86A05D40, &D_86A05D44 },
    },
    {
        1,
        { &D_86A05D48, &D_86A05D4C },
    },
    {
        1,
        { &D_86A05D50, &D_86A05D54 },
    },
    {
        1,
        { &D_86A05D58, &D_86A05D5C },
    },
    {
        1,
        { &D_86A05D60, &D_86A05D64 },
    },
    {
        1,
        { &D_86A05D68, &D_86A05D6C },
    },
    {
        1,
        { &D_86A05D70, &D_86A05D74 },
    },
    {
        0,
        { &D_86A05D78, &D_86A05D7C },
    },
    {
        1,
        { &D_86A05D80, &D_86A05D84 },
    },
    {
        1,
        { &D_86A05D88, &D_86A05D8C },
    },
    {
        1,
        { &D_86A05D90, &D_86A05D94 },
    },
    {
        1,
        { &D_86A05D98, &D_86A05D9C },
    },
    {
        1,
        { &D_86A05DA0, &D_86A05DA4 },
    },
    {
        1,
        { &D_86A05DA8, &D_86A05DAC },
    },
    {
        1,
        { &D_86A05DB0, &D_86A05DB4 },
    },
    {
        1,
        { &D_86A05DB8, &D_86A05DBC },
    },
    {
        0,
        { &D_86A05DC0, &D_86A05DC4 },
    },
    {
        1,
        { &D_86A05DC8, &D_86A05DCC },
    },
    {
        1,
        { &D_86A05DD0, &D_86A05DD4 },
    },
    {
        1,
        { &D_86A05DD8, &D_86A05DDC },
    },
    {
        1,
        { &D_86A05DE0, &D_86A05DE4 },
    },
    {
        1,
        { &D_86A05DE8, &D_86A05DEC },
    },
    {
        1,
        { &D_86A05DF0, &D_86A05DF4 },
    },
    {
        1,
        { &D_86A05DF8, &D_86A05DFC },
    },
    {
        1,
        { &D_86A05E00, &D_86A05E04 },
    },
    {
        0,
        { &D_86A05E08, &D_86A05E0C },
    },
    {
        1,
        { &D_86A05E10, &D_86A05E14 },
    },
    {
        1,
        { &D_86A05E18, &D_86A05E1C },
    },
    {
        1,
        { &D_86A05E20, &D_86A05E24 },
    },
    {
        0,
        { &D_86A05E28, &D_86A05E2C },
    },
    {
        1,
        { &D_86A05E30, &D_86A05E34 },
    },
    {
        0,
        { &D_86A05E38, &D_86A05E3C },
    },
    {
        1,
        { &D_86A05E40, &D_86A05E44 },
    },
    {
        1,
        { &D_86A05E48, &D_86A05E4C },
    },
    {
        0,
        { &D_86A05E50, &D_86A05E54 },
    },
    {
        1,
        { &D_86A05E58, &D_86A05E5C },
    },
    {
        0,
        { &D_86A05E60, &D_86A05E64 },
    },
    {
        1,
        { &D_86A05E68, &D_86A05E6C },
    },
    {
        1,
        { &D_86A05E70, &D_86A05E74 },
    },
    {
        0,
        { &D_86A05E78, &D_86A05E7C },
    },
    {
        1,
        { &D_86A05E80, &D_86A05E84 },
    },
    {
        0,
        { &D_86A05E88, &D_86A05E8C },
    },
    {
        1,
        { &D_86A05E90, &D_86A05E94 },
    },
    {
        1,
        { &D_86A05E98, &D_86A05E9C },
    },
    {
        0,
        { &D_86A05EA0, &D_86A05EA4 },
    },
    {
        1,
        { &D_86A05EA8, &D_86A05EAC },
    },
    {
        1,
        { &D_86A05EB0, &D_86A05EB4 },
    },
    {
        1,
        { &D_86A05EB8, &D_86A05EBC },
    },
    {
        0,
        { &D_86A05EC0, &D_86A05EC4 },
    },
    {
        1,
        { &D_86A05EC8, &D_86A05ECC },
    },
    {
        0,
        { &D_86A05ED0, &D_86A05ED4 },
    },
    {
        1,
        { &D_86A05ED8, &D_86A05EDC },
    },
    {
        0,
        { &D_86A05EE0, &D_86A05EE4 },
    },
    {
        1,
        { &D_86A05EE8, &D_86A05EEC },
    },
    {
        1,
        { &D_86A05EF0, &D_86A05EF4 },
    },
    {
        0,
        { &D_86A05EF8, &D_86A05EFC },
    },
    {
        1,
        { &D_86A05F00, &D_86A05F04 },
    },
    {
        1,
        { &D_86A05F08, &D_86A05F0C },
    },
    {
        1,
        { &D_86A05F10, &D_86A05F14 },
    },
    {
        1,
        { &D_86A05F18, &D_86A05F1C },
    },
    {
        1,
        { &D_86A05F20, &D_86A05F24 },
    },
    {
        0,
        { &D_86A05F28, &D_86A05F2C },
    },
    {
        1,
        { &D_86A05F30, &D_86A05F34 },
    },
    {
        1,
        { &D_86A05F38, &D_86A05F3C },
    },
    {
        1,
        { &D_86A05F40, &D_86A05F44 },
    },
    {
        1,
        { &D_86A05F48, &D_86A05F4C },
    },
    {
        2,
        { &D_86A05F50, &D_86A05F54 },
    },
    {
        1,
        { &D_86A05F58, &D_86A05F5C },
    },
    {
        1,
        { &D_86A05F60, &D_86A05F64 },
    },
    {
        1,
        { &D_86A05F68, &D_86A05F6C },
    },
    {
        0,
        { &D_86A05F70, &D_86A05F74 },
    },
    {
        1,
        { &D_86A05F78, &D_86A05F7C },
    },
    {
        1,
        { &D_86A05F80, &D_86A05F84 },
    },
    {
        1,
        { &D_86A05F88, &D_86A05F8C },
    },
    {
        1,
        { &D_86A05F90, &D_86A05F94 },
    },
    {
        1,
        { &D_86A05F98, &D_86A05F9C },
    },
    {
        1,
        { &D_86A05FA0, &D_86A05FA4 },
    },
    {
        0,
        { &D_86A05FA8, &D_86A05FAC },
    },
    {
        1,
        { &D_86A05FB0, &D_86A05FB4 },
    },
    {
        0,
        { &D_86A05FB8, &D_86A05FBC },
    },
    {
        1,
        { &D_86A05FC0, &D_86A05FC4 },
    },
    {
        0,
        { &D_86A05FC8, &D_86A05FCC },
    },
    {
        1,
        { &D_86A05FD0, &D_86A05FD4 },
    },
    {
        1,
        { &D_86A05FD8, &D_86A05FDC },
    },
    {
        1,
        { &D_86A05FE0, &D_86A05FE4 },
    },
    {
        1,
        { &D_86A05FE8, &D_86A05FEC },
    },
    {
        0,
        { &D_86A05FF0, &D_86A05FF4 },
    },
    {
        1,
        { &D_86A05FF8, &D_86A05FFC },
    },
    {
        1,
        { &D_86A06000, &D_86A06004 },
    },
    {
        1,
        { &D_86A06008, &D_86A0600C },
    },
    {
        1,
        { &D_86A06010, &D_86A06014 },
    },
    {
        0,
        { &D_86A06018, &D_86A0601C },
    },
    {
        1,
        { &D_86A06020, &D_86A06024 },
    },
    {
        1,
        { &D_86A06028, &D_86A0602C },
    },
    {
        1,
        { &D_86A06030, &D_86A06034 },
    },
    {
        2,
        { &D_86A06038, &D_86A0603C },
    },
    {
        1,
        { &D_86A06040, &D_86A06044 },
    },
    {
        1,
        { &D_86A06048, &D_86A0604C },
    },
    {
        1,
        { &D_86A06050, &D_86A06054 },
    },
    {
        1,
        { &D_86A06058, &D_86A0605C },
    },
    {
        1,
        { &D_86A06060, &D_86A06064 },
    },
    {
        1,
        { &D_86A06068, &D_86A0606C },
    },
    {
        0,
        { &D_86A06070, &D_86A06074 },
    },
    {
        1,
        { &D_86A06078, &D_86A0607C },
    },
    {
        1,
        { &D_86A06080, &D_86A06084 },
    },
    {
        1,
        { &D_86A06088, &D_86A0608C },
    },
    {
        1,
        { &D_86A06090, &D_86A06094 },
    },
    {
        1,
        { &D_86A06098, &D_86A0609C },
    },
    {
        0,
        { &D_86A060A0, &D_86A060A4 },
    },
    {
        1,
        { &D_86A060A8, &D_86A060AC },
    },
    {
        1,
        { &D_86A060B0, &D_86A060B4 },
    },
    {
        0,
        { &D_86A060B8, &D_86A060BC },
    },
    {
        1,
        { &D_86A060C0, &D_86A060C4 },
    },
    {
        0,
        { &D_86A060C8, &D_86A060CC },
    },
    {
        1,
        { &D_86A060D0, &D_86A060D4 },
    },
    {
        1,
        { &D_86A060D8, &D_86A060DC },
    },
    {
        1,
        { &D_86A060E0, &D_86A060E4 },
    },
    {
        0,
        { &D_86A060E8, &D_86A060EC },
    },
    {
        1,
        { &D_86A060F0, &D_86A060F4 },
    },
    {
        1,
        { &D_86A060F8, &D_86A060FC },
    },
    {
        0,
        { &D_86A06100, &D_86A06104 },
    },
    {
        1,
        { &D_86A06108, &D_86A0610C },
    },
    {
        1,
        { &D_86A06110, &D_86A06114 },
    },
    {
        1,
        { &D_86A06118, &D_86A0611C },
    },
    {
        1,
        { &D_86A06120, &D_86A06124 },
    },
    {
        0,
        { &D_86A06128, &D_86A0612C },
    },
    {
        1,
        { &D_86A06130, &D_86A06134 },
    },
};

// The staff roll in 27 pages followed by two terminators (duration 0). Page 0 points at
// sRoleLines; Credits_DrawRoleList chains each following page to the line after the previous
// page's last line. Durations sum to 9120 frames, the same total as sBackgroundSegments.
CreditsRolePage sRolePages[29] = {
    {
        0,
        360,
        40,
        6,
        sRoleLines,
    },
    {
        0,
        360,
        40,
        6,
        NULL,
    },
    {
        0,
        368,
        48,
        8,
        NULL,
    },
    {
        0,
        352,
        40,
        6,
        NULL,
    },
    {
        0,
        360,
        40,
        6,
        NULL,
    },
    {
        0,
        368,
        48,
        5,
        NULL,
    },
    {
        0,
        352,
        40,
        3,
        NULL,
    },
    {
        0,
        360,
        40,
        9,
        NULL,
    },
    {
        0,
        368,
        48,
        9,
        NULL,
    },
    {
        0,
        352,
        40,
        9,
        NULL,
    },
    {
        0,
        360,
        40,
        9,
        NULL,
    },
    {
        0,
        368,
        48,
        9,
        NULL,
    },
    {
        0,
        232,
        40,
        4,
        NULL,
    },
    {
        0,
        240,
        40,
        5,
        NULL,
    },
    {
        0,
        240,
        40,
        5,
        NULL,
    },
    {
        0,
        368,
        48,
        9,
        NULL,
    },
    {
        0,
        352,
        40,
        7,
        NULL,
    },
    {
        0,
        360,
        40,
        6,
        NULL,
    },
    {
        0,
        368,
        48,
        9,
        NULL,
    },
    {
        0,
        352,
        40,
        9,
        NULL,
    },
    {
        0,
        240,
        40,
        2,
        NULL,
    },
    {
        0,
        360,
        40,
        10,
        NULL,
    },
    {
        0,
        368,
        48,
        11,
        NULL,
    },
    {
        0,
        352,
        40,
        9,
        NULL,
    },
    {
        0,
        360,
        40,
        9,
        NULL,
    },
    {
        0,
        360,
        40,
        5,
        NULL,
    },
    {
        0,
        240,
        40,
        2,
        NULL,
    },
    {
        0,
        0,
        40,
        0,
        NULL,
    },
    {
        0,
        0,
        0,
        0,
        NULL,
    },
};

/**
 * Outro: fades audio categories 1 and 2 and the screen to black over 120 frames, then waits
 * 60 frames.
 */
void Credits_OutroLoop(void) {
    u8 i;

    Audio_FadeCategories(1, 0x78);
    Audio_FadeCategories(2, 0x78);
    StageFade_StartFromTransparent(0x78);

    for (i = 0; i < 120; i++) {
        Credits_PollInput();
        Credits_DrawFrame(2);
    }

    StageLoader_RunFrames(0x3C);
}

/**
 * Takes the camera node from the root of sArenaGeoLayout and sets the initial orbit
 * (rotationX 0x100, rotationY 0, distance 120) and clip planes.
 */
void Credits_InitCamera(void) {
    sArenaCamera = sArenaRoot->children;
    sCameraRotationX = 0x100;
    sCameraRotationY = 0;
    sCameraDistance = 0x78;

    sArenaCamera->perspective.nearPlane = 50.0f;
    sArenaCamera->perspective.farPlane = 6400.0f;
    Camera_ComputeEyeFromAngles(&sArenaCamera->view.targetPosition, &sArenaCamera->view.eyePosition, sCameraDistance, sCameraRotationX,
                                sCameraRotationY);
}

/**
 * Opens the stadium_models archive, builds sArenaGeoLayout with an empty arena, prepares the
 * staff roll from sRolePages and pushes the 'BACK' pool state that each arena load replaces.
 */
void Credits_InitScene(s32 is_round_1) {
    MemoryBlock* layout_pool;
    MemoryBlock* temp_v0;

    sStadiumModelArchive = ASSET_LOAD2(stadium_models, 1, 1);
    GeoNode_CreateFog(0, &sArenaFog, 0x3C0, 0x3E8, 0xFF, 0xFF, 0xFF, 0xFF);
    GeoNode_CreateContainer(NULL, &sArenaLayout0);
    GeoNode_CreateContainer(NULL, &sArenaLayout1);
    GeoNode_CreateContainer(NULL, &sArenaLayout3);
    Credits_SetBackgroundTint(0xFF, 0xFF, 0xFF);
    layout_pool = MainPool_AllocState(main_pool_get_available(), 0);
    sArenaRoot = process_geo_layout(layout_pool, &sArenaGeoLayout);
    MainPool_FinalizeAllocation(layout_pool);
    ModelRenderer_InitDisplayRoots();
    Credits_InitCamera();
    Credits_InitRoleList(1, is_round_1, sRolePages);
    sArenaIndex = -1;

    main_pool_push_state('BACK');
}

/**
 * Fragment entry for the staff credits. State_RunCredits passes `is_round_1` = 1 unless
 * gSessionContext.inRound2 is set; Kids Club dispatch case 11 passes 0. Runs the logo
 * intro, the staff roll over random battle arenas and the outro.
 * //@src/game_state.c State_RunCredits //@src/minigame_dispatch.c
 */
s32 Credits_Main(s32 unused, s32 is_round_1) {
    StageContext* stage_context;

    main_pool_push_state('ROLE');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    stage_context = StageContext_Allocate(0, 1, 3, 1, 1, 1);
    sFontContext = Font_Init(4, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_3000000, kids_club_game_ui, 0);
    FRAGMENT_LOAD(FRAGMENT_PARTICLE_DATA_LIBRARY);

    Credits_InitScene(is_round_1);
    StageContext_Activate(stage_context);
    Credits_IntroLoop();
    Credits_MainLoop(is_round_1);
    Credits_OutroLoop();
    StageContext_Deactivate();
    Font_Free();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('ROLE');

    return 0;
}

/**
 * Computes the four staff-roll colors for page time `timer` by linear interpolation between
 * sRoleColorKeyframes. Keyframes 3 and 4 are moved so the fade out ends 1 frame before the
 * page's trailing blank period. Returns 0 (draw nothing) once timer reaches
 * duration - blank_frames, otherwise 1.
 */
s32 Credits_InterpolateRoleColor(Color_RGBA8_u32* colors, u16 timer, u16 duration, u8 blank_frames) {
    u8 i;
    u8 j;
    u8 alpha;

    if (timer >= (duration - blank_frames)) {
        return 0;
    }

    sRoleColorKeyframes[3].frame = (duration - blank_frames) - 0x15;
    sRoleColorKeyframes[4].frame = (duration - blank_frames) - 1;

    for (i = 0; i < 5; i++) {
        if (timer == sRoleColorKeyframes[i].frame) {
            for (j = 0; j < 4; j++) {
                Color_SetRGBA(&colors[j], sRoleColorKeyframes[i].colors[j].r, sRoleColorKeyframes[i].colors[j].g, sRoleColorKeyframes[i].colors[j].b,
                              sRoleColorKeyframes[i].alpha);
            }
            break;
        }

        if ((sRoleColorKeyframes[i].frame < timer) && (timer < sRoleColorKeyframes[i + 1].frame)) {
            alpha =
                (s32)ROUND_MAX((((sRoleColorKeyframes[i + 1].alpha - sRoleColorKeyframes[i].alpha) * (timer - sRoleColorKeyframes[i].frame)) /
                                (f32)(sRoleColorKeyframes[i + 1].frame - sRoleColorKeyframes[i].frame)) +
                               sRoleColorKeyframes[i].alpha);

            for (j = 0; j < 4; j++) {
                colors[j].r = (s32)ROUND_MAX(
                    (((sRoleColorKeyframes[i + 1].colors[j].r - sRoleColorKeyframes[i].colors[j].r) * (timer - sRoleColorKeyframes[i].frame)) /
                     (f32)(sRoleColorKeyframes[i + 1].frame - sRoleColorKeyframes[i].frame)) +
                    sRoleColorKeyframes[i].colors[j].r);
                colors[j].g = (s32)ROUND_MAX(
                    (((sRoleColorKeyframes[i + 1].colors[j].g - sRoleColorKeyframes[i].colors[j].g) * (timer - sRoleColorKeyframes[i].frame)) /
                     (f32)(sRoleColorKeyframes[i + 1].frame - sRoleColorKeyframes[i].frame)) +
                    sRoleColorKeyframes[i].colors[j].g);
                colors[j].b = (s32)ROUND_MAX(
                    (((sRoleColorKeyframes[i + 1].colors[j].b - sRoleColorKeyframes[i].colors[j].b) * (timer - sRoleColorKeyframes[i].frame)) /
                     (f32)(sRoleColorKeyframes[i + 1].frame - sRoleColorKeyframes[i].frame)) +
                    sRoleColorKeyframes[i].colors[j].b);
                colors[j].a = alpha;
            }

            break;
        }
    }
    return 1;
}

/**
 * The storage at 0x86A06218 is this function's block-scope `static char** sCreditsText`:
 * only the block-scope spelling produces the target codegen (a file-scope definition,
 * static or not, costs an extra callee-saved register that caches the symbol's address
 * across the loop).
 *
 * IDO emits a block-scope static's `.lcomm` in its internal symbol-table walk order, which
 * for this translation unit interleaves the file's statics and globals with the externs
 * referenced by the sRoleColorKeyframes/sRolePages tables. On its own, this static's `.lcomm` would
 * be emitted just ahead of sFrameDelta and the bss globals would slide four bytes. The six
 * never-defined `lcomm_walk_pad_*` prototypes below exist only to grow that symbol table
 * so the walk places this static between sCurrentRolePage and sLastRetraceCount; they emit no code,
 * data, or bss. Deleting any one of them, or adding another, shifts the bss layout.
 */
static void lcomm_walk_pad_0(void);
static void lcomm_walk_pad_1(void);
static void lcomm_walk_pad_2(void);
static void lcomm_walk_pad_3(void);
static void lcomm_walk_pad_4(void);
static void lcomm_walk_pad_5(void);

/**
 * Prepares the staff roll: loads every string of textdata table 45 (the US staff credits)
 * into sRoleLines[i].text[0], sets the starting frame delta, clears the text column and
 * frame-delta history, and makes `pages` the current page. `is_round_1` is unused.
 */
void Credits_InitRoleList(u8 initial_frame_delta, u8 is_round_1, CreditsRolePage* pages) {
    static char** sCreditsText;

    u16 i;
    u16 line_count;
    CreditsRoleLine* line;

    line_count = Credits_GetRoleLineCount();
    Text_InitStringTables();
    sCreditsText = Text_GetStringTable(0x2D);

    for (i = 0, line = sRoleLines; i < line_count; i++, line++) {
        line->text[0] = Text_GetString(NULL, 0, sCreditsText, i);
    }

    sFrameDelta = initial_frame_delta;
    sRoleTextColumn = 0;
    sFrameDeltaPrimed = 0;
    sCurrentRolePage = pages;
}

/**
 * Sets sFrameDelta to the number of retraces (D_800A62E0.unk_A34, incremented by
 * Sched_HandleRetrace) since the previous call, so the staff roll and camera keep real-time
 * speed when frames are dropped. The first call only records the count.
 * //@src/scheduler.c Sched_HandleRetrace
 *
 * FAKE: the wrap-around arm widens the counter to 64 bits before subtracting, which is what makes
 * IDO stage the value through the low half of a register pair (addiu t5, v0, 0) and only then
 * do the 32-bit subtract (subu t9, t5, v1). The two chained 0x100000000 subtractions cancel
 * out of the low word (the result is truncated to 8 bits anyway), but they are what puts the
 * pair at t4:t5 instead of t2:t3.
 */
void Credits_UpdateFrameDelta(void) {
    s32 retrace_count = D_800A62E0.unk_A34;

    if (sFrameDeltaPrimed == 0) {
        sFrameDeltaPrimed = 1;
    } else if ((u32)sLastRetraceCount < (u32)retrace_count) {
        sFrameDelta = retrace_count - sLastRetraceCount;
    } else {
        sFrameDelta = ((u64)(u32)retrace_count - 0x100000000ULL - 0x100000000ULL) - sLastRetraceCount;
    }
    sLastRetraceCount = retrace_count;
}

/**
 * Draws and advances the current staff-roll page. Lines are centered horizontally at x 160;
 * the block is centered vertically using 17 pixels per printed line, 12 extra pixels before a
 * heading that follows a name, 4 before a name that follows a heading and 10 per spacer.
 * Headings use colors 0/1 and names colors 2/3 from Credits_InterpolateRoleColor. When the
 * page timer reaches its duration the next page is chained to the following lines and its
 * duration absorbs the overshoot; after the last page the Pokemon Stadium logo fades in.
 */
void Credits_DrawRoleList(void) {
    u8 heading_gap;
    u8 heading_count;
    u8 spacer_count;
    u8 i;
    s32 current_style;
    Color_RGBA8_u32 colors[4];
    u16 y;
    CreditsRoleLine* line;
    CreditsRolePage finished_page;

    if (sCurrentRolePage == NULL) {
        Credits_DrawStadiumLogo(1);
        return;
    }

    heading_count = 0;
    spacer_count = 0;

    if (Credits_InterpolateRoleColor(colors, sCurrentRolePage->timer, sCurrentRolePage->duration, sCurrentRolePage->blankFrames) != 0) {
        Font_BeginTranslucentTextRendering();
        Font_EnableTwoCycleTexturing();
        Font_SetActive(4, 0);

        current_style = 0;

        Gfx_SetEnvColor(colors[0].r, colors[0].g, colors[0].b, colors[0].a);
        Gfx_SetPrimColor(colors[1].r, colors[1].g, colors[1].b, colors[1].a);

        line = sCurrentRolePage->lines;
        for (i = 0; i < sCurrentRolePage->lineCount; i++, line++) {
            switch (line->style) {
                case 0:
                    heading_count++;
                    break;

                case 2:
                    spacer_count++;
                    break;
            }
        }

        if (heading_count >= 2) {
            heading_gap = ((heading_count * 4) + (heading_count * 0xC) + (spacer_count * 0xA)) - 0xC;
        } else {
            heading_gap = (heading_count * 4) + (spacer_count * 0xA);
        }

        y = (((-((sCurrentRolePage->lineCount - spacer_count) * 0x11) - heading_gap) + 0xF0) / 2) & 0xFFFF;

        for (i = 0, line = sCurrentRolePage->lines; i < sCurrentRolePage->lineCount; i++, line++) {
            if (line->style == 2) {
                y += 0xA;
                continue;
            }

            if (current_style != line->style) {
                switch (line->style) {
                    case 0:
                        Gfx_SetEnvColor(colors[0].r, colors[0].g, colors[0].b, colors[0].a);
                        Gfx_SetPrimColor(colors[1].r, colors[1].g, colors[1].b, colors[1].a);
                        current_style = 0;
                        y += 0xC;
                        break;

                    case 1:
                        Gfx_SetEnvColor(colors[2].r, colors[2].g, colors[2].b, colors[2].a);
                        Gfx_SetPrimColor(colors[3].r, colors[3].g, colors[3].b, colors[3].a);
                        current_style = 1;
                        y += 4;
                        break;
                }
            }

            Font_Printf(0xA0 - (Font_MeasureTextExtent(0, 0, line->text[sRoleTextColumn]) / 2), (y * 4) >> 2,
                        line->text[sRoleTextColumn]);
            y += 0x11;
        }

        Font_DisableTwoCycleTexturing();
        Font_EndTexturedTextRendering();
    }

    sCurrentRolePage->timer = sCurrentRolePage->timer + sFrameDelta;
    if (sCurrentRolePage->timer >= sCurrentRolePage->duration) {
        finished_page = *sCurrentRolePage;

        sCurrentRolePage++;

        sCurrentRolePage->lines = &finished_page.lines[finished_page.lineCount];
        if (sCurrentRolePage->duration == 0) {
            sCurrentRolePage = NULL;
        } else {
            sCurrentRolePage->duration = (sCurrentRolePage->duration - finished_page.timer) + finished_page.duration;
            sCurrentRolePage->timer = 0;
        }
    }
}
