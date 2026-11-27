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

// Fog node of sArenaGeoLayout; callbackArg is 1 when the arena supplied fog parameters.
static unk_D_8690A610 sArenaFog;
// Containers for the geo layouts returned by arena archive entries 0, 1 and 3.
static struct GraphNode sArenaLayout0;
static struct GraphNode sArenaLayout1;
static struct GraphNode sArenaLayout3;
static s8 sArenaIndex;                   // stadium_models arena file shown behind the roll; -1 = none
static u8 sCameraShot;                   // 0-5 orbit, 6-8 pivot orbit, 9-14 keyframe blend
static BinArchive* sStadiumModelArchive; // stadium_models; entry N is battle arena N
static Color_RGBA8_u32 sArenaTintColor;  // copied to textureTint by Credits_BackgroundNodePostCallback
static GraphNode* sArenaRoot;
static CameraNode* sArenaCamera;
// Orbit camera state passed to Camera_ComputeEyeFromAngles by Credits_UpdateCamera.
static s16 sCameraRotationX;
static s16 sCameraRotationY;
static s16 sCameraDistance;
static s32 pad_D_86A061F8[2];
FontContext* sFontContext;

// Arena backdrop schedule: eight timed segments (1320 or 1080 frames, 9120 in total, the
// same as the sRolePages durations) and an end marker.
// Segment 3 also reloads a new arena at 1/3 and 2/3 of its duration (Credits_MainLoop).
CreditsBackgroundSegment sBackgroundSegments[9] = {
    {
        0x00,
        0x00,
        NTSC_PAL(0x0528, 0x044C),
    },
    {
        0x00,
        0x01,
        NTSC_PAL(0x0438, 0x0384),
    },
    {
        0x00,
        0x02,
        NTSC_PAL(0x0438, 0x0384),
    },
    {
        0x00,
        0x03,
        NTSC_PAL(0x0438, 0x0384),
    },
    {
        0x00,
        0x04,
        NTSC_PAL(0x0438, 0x0384),
    },
    {
        0x00,
        0x05,
        NTSC_PAL(0x0438, 0x0384),
    },
    {
        0x00,
        0x06,
        NTSC_PAL(0x0528, 0x044C),
    },
    {
        0x00,
        0x07,
        NTSC_PAL(0x0438, 0x0384),
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
        Credits_InitCameraOrbitBlend(&sCameraRotationX, &sCameraRotationY, &sCameraDistance, sArenaCamera,
                                     sBackgroundSegments[sSegmentIndex].duration, arena_index, shot);
    } else if (shot < 9) {
        Credits_InitCameraOrbitBlend(&sCameraRotationX, &sCameraRotationY, &sCameraDistance, sArenaCamera,
                                     sBackgroundSegments[sSegmentIndex].duration, arena_index, shot);
    } else if (shot < 0xF) {
        if (sSegmentIndex == 7) {
            Credits_InitCameraKeyframeBlend(sArenaCamera, sBackgroundSegments[sSegmentIndex].duration - NTSC_PAL(0x28, 0x21),
                                            arena_index, shot);
        } else {
            Credits_InitCameraKeyframeBlend(sArenaCamera, sBackgroundSegments[sSegmentIndex].duration, arena_index,
                                            shot);
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
        Camera_ComputeEyeFromAngles(&sArenaCamera->view.targetPosition, &sArenaCamera->view.eyePosition,
                                    sCameraDistance, sCameraRotationX, sCameraRotationY);
    } else if (sCameraShot < 9) {
        Camera_ComputeEyeFromAngles(&sOrbitPivot, &sArenaCamera->view.eyePosition, sCameraDistance, sCameraRotationX,
                                    sCameraRotationY);
        Camera_ComputeEyeFromAngles(&sOrbitPivot, &sArenaCamera->view.targetPosition, sCameraDistance + 500.0f,
                                    sCameraRotationX, sCameraRotationY);
    }
}

/**
 * Draws a full-screen black rectangle that fades the backdrop in over the first 8 frames of a
 * segment and out over its last 40 frames. Nothing is drawn after the end marker.
 */
void Credits_DrawFadeOverlay(void) {
    u8 alpha;

    if (sBackgroundSegments[sSegmentIndex].endMarker != -1) {
        if (sSegmentTimer < NTSC_PAL(8, 7)) {
            alpha = (s8)ROUND_MAX(((-(f32)sSegmentTimer / NTSC_PAL(8, 7)) + 1.0f) * 255.0f);
            if (alpha != 0) {
                Gfx_FillRectWithDisplayState(0, 0, 0x140, 0xF0, 0, 0, 0, alpha);
            }
        } else {
            if ((sBackgroundSegments[sSegmentIndex].duration - NTSC_PAL(0x28, 0x21)) < sSegmentTimer) {
                s32 duration = sBackgroundSegments[sSegmentIndex].duration;

                alpha = (s32)ROUND_MAX((((sSegmentTimer - duration) + NTSC_PAL(0x28, 0x21)) * 255.0f) / NTSC_PAL(40.0f, 33.0f));
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
        if ((sSegmentIndex != 0) || (sSegmentTimer >= NTSC_PAL(240, 200))) {
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

    Audio_FadeOutAll(NTSC_PAL(30, 25));

    for (i = 0; i < 3; i++) {
        frame_count = phase_frames;

        switch (i) {
            case 0:
                phase_frames = NTSC_PAL(30, 25);
                StageFade_StartFromOpaque(phase_frames);
                frame_count = NTSC_PAL(30, 25);
                break;

            case 1:
                phase_frames = NTSC_PAL(90, 75);
                frame_count = phase_frames;
                break;

            case 2:
                phase_frames = NTSC_PAL(120, 100);
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
 * Exits 721 frames (601 in PAL) after the end logo is fully opaque, or after 361 (301) on A or Start.
 */
void Credits_MainLoop(s32 is_round_1) {
    // Extra arena swaps already made during segment 3 (at most 2).
    static u8 sSegment3ReloadCount = 0;

    s32 running = 1;

#ifdef VERSION_JP
    // JP starts without the 60-frame wait and its fade-in.
    StageFade_StartFromOpaque(1);
#else
    StageLoader_RunFrames(NTSC_PAL(60, 50));
    StageFade_StartFromOpaque(NTSC_PAL(180, 150));
#endif

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
                        sBackgroundSegments[sSegmentIndex - 1].duration +
                        (sBackgroundSegments[sSegmentIndex].duration - sSegmentTimer);
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
                       (sSegmentTimer >=
                        (((sBackgroundSegments[sSegmentIndex].duration * (sSegment3ReloadCount + 1)) / 3) - 1))) {
                Credits_SelectNextScene(&sArenaIndex, &sCameraShot);
                Credits_LoadBackgroundScene(sArenaIndex, 0xFF);
                sSegment3ReloadCount++;
            }
        }

        Credits_PollInput();
        Credits_UpdateCamera();

        if (sLogoHoldTimer >= NTSC_PAL(721, 601)) {
            running = 0;
        } else if ((sLogoHoldTimer >= NTSC_PAL(361, 301)) &&
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
/**
 * Outro: fades audio categories 1 and 2 and the screen to black over 120 frames, then waits
 * 60 frames (no wait in JP).
 */
void Credits_OutroLoop(void) {
    u8 i;

    Audio_FadeCategories(1, NTSC_PAL(120, 100));
    Audio_FadeCategories(2, NTSC_PAL(120, 100));
    StageFade_StartFromTransparent(NTSC_PAL(120, 100));

    for (i = 0; i < NTSC_PAL(120, 100); i++) {
        Credits_PollInput();
        Credits_DrawFrame(2);
    }

#ifndef VERSION_JP
    StageLoader_RunFrames(NTSC_PAL(60, 50));
#endif
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
    Camera_ComputeEyeFromAngles(&sArenaCamera->view.targetPosition, &sArenaCamera->view.eyePosition, sCameraDistance,
                                sCameraRotationX, sCameraRotationY);
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
    FRAGMENT_LOAD(FRAGMENT_MODEL_CALLBACK_LIBRARY);

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
