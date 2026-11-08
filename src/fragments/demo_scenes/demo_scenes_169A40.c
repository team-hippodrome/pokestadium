#include "demo_scenes.h"
#include "src/geo_render.h"
#include "src/model_animation.h"
#include "src/geo_layout.h"
#include "src/display_object_textures.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/save_data.h"
#include "src/jpeg_stream.h"
#include "src/gfx_buffer.h"
#include "src/gfx_rect.h"
#include "src/DDC0.h"
#include "src/matrix.h"
#include "src/controller.h"
#include "src/memory.h"
#include "src/stage_loader.h"

static u32 sStageGeoLayoutCommands[] = {
    0x0C00FFFF, 0x05000000, 0x0B00001E,  0x00000000, 0x014000F0, 0x00000032, 0x00000000, 0x00000118,
    0x05000000, 0x0D000000, 0x05000000,  0x14000000, 0x002B0012, 0xFFFFFF32, 0x16FFFFFF, 0x0F000003,
    0x05000000, 0x0A000000, &D_800AC840, 0x06000000, 0x06000000, 0x06000000, 0x06000000, 0x01000000,
};

static unk_D_86002F30* sMewtwoModelData;
static GraphNode* sMewtwoSceneRoot;
static DisplayObject sMewtwoDisplayObject;
static CameraNode* sMewtwoCamera;
static BinArchive* sMewtwoBackgroundArchive;
static u8* sMewtwoBackgroundTiles;
static u8 sMewtwoMaterialAlpha;
static ModeSettings sMewtwoModeSettings;

/**
 * Empty overlay hook called after the scene graph in MewtwoIntro_RenderFrame.
 */
void MewtwoIntro_DrawOverlay(void) {
}

/**
 * Positions the scene camera 200 units in front of the origin with a 45-degree field of view.
 */
void MewtwoIntro_InitCamera(void) {
    sMewtwoCamera = sMewtwoSceneRoot->children;
    Vec3f_SetComponentsDuplicate(&sMewtwoCamera->view.targetPosition, 0.0f, 0.0f, 0.0f);
    Vec3f_SetComponentsDuplicate(&sMewtwoCamera->view.eyePosition, 0.0f, 0.0f, 200.0f);
    sMewtwoCamera->perspective.nearPlane = 16.0f;
    sMewtwoCamera->perspective.farPlane = 32768.0f;
    sMewtwoCamera->perspective.verticalFieldOfView = 45.0f;
}

/**
 * Polls the controllers once per frame and resets input repeat state.
 */
void MewtwoIntro_ReadControllerInput(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

/**
 * Draws a 320x240 RGBA16 backdrop as 20 x 15 tiles of 16x16 pixels (0x200 bytes each).
 */
void MewtwoIntro_DrawBackgroundTiles(u8* arg0) {
    s32 i;
    s32 j;

    if (1) {}

    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGB, G_CC_DECALRGB);

    gDPPipeSync(gDisplayListHead++);

    for (i = 0; i < 15; i++) {
        for (j = 0; j < 20; j++, arg0 += 0x200) {
            Gfx_DrawTextureRgba16(j * 0x10, i * 0x10, 0x10, 0x10, arg0, 0x10, 0);
        }
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

/**
 * Draws the backdrop, clears depth, renders the cutscene model and the empty overlay.
 */
void MewtwoIntro_RenderFrame(void) {
    BgStage_DrawFrame();
    MewtwoIntro_DrawBackgroundTiles(sMewtwoBackgroundTiles);
    GfxImage_ClearDepthRectangle(&gDisplayListHead, 0, 0, 0x140, 0xF0);
    GeoRender_AdvanceFrameCounter();
    Geo_RenderRootNode(sMewtwoSceneRoot);
    MewtwoIntro_DrawOverlay();
    BgStage_AdvanceFrame();
}

/**
 * From animation frame 95 raises the model's black material alpha by 20 per frame; at frame 100 starts a
 * 3-frame fade out. Returns 1 when the model animation has finished.
 */
s32 MewtwoIntro_UpdateFade(void) {
    s32 var_v0;

    if ((sMewtwoDisplayObject.transformAnim.frameFixed >> 0x10) >= 0x5F) {
        // u8 alpha wraps after 13 steps (frame 108) if the animation is still running.
        Model_SetMaterialColor(&sMewtwoDisplayObject, 0, 0, 0, sMewtwoMaterialAlpha += 0x14);
    }

    if ((sMewtwoDisplayObject.transformAnim.frameFixed >> 0x10) == 0x64) {
        StageContext_SetClearColor(1);
        StageFade_StartFromTransparent(3);
    }

    var_v0 = ModelAnim_IsAnimationDone(&sMewtwoDisplayObject) != 0;
    if (var_v0) {
        return 1;
    }
    //!bug: no return value on this path; MewtwoIntro_RunLoop reads whatever is left in v0.
}

/**
 * Fades in over 12 frames, starts music 0x30 and runs the cutscene until the animation finishes.
 */
void MewtwoIntro_RunLoop(void) {
    s32 var_s0 = 1;

    StageFade_StartFromOpaque(0xC);
    Audio_PlayMusicIfChanged(0x30);

    while (var_s0 != 0) {
        MewtwoIntro_ReadControllerInput();
        if (MewtwoIntro_UpdateFade() != 0) {
            var_s0 = 0;
        }
        MewtwoIntro_RenderFrame();
    }

    StageLoader_RunFrames(2);
}

/**
 * Builds the scene graph and camera and loads model-archive entry 0xD6 with animation 1 at the origin.
 */
void MewtwoIntro_InitScene(void) {
    MemoryBlock* sp1C = MainPool_AllocState(main_pool_get_available(), 0);

    sMewtwoSceneRoot = process_geo_layout(sp1C, &sStageGeoLayoutCommands);

    MainPool_FinalizeAllocation(sp1C);
    ModelRenderer_InitDisplayRoots();
    MewtwoIntro_InitCamera();
    ModelRenderer_AttachDisplayObject(&sMewtwoDisplayObject);
    PokeIcon_OpenModelArchives();

    sMewtwoModelData = Model_LoadByArchiveIndex(0xD6);
    Model_InitDisplayObject(&sMewtwoDisplayObject, 0, 0xD6, sMewtwoModelData->unk_08->unk_00[0]);
    ModelAnim_SetAnimation(&sMewtwoDisplayObject, 1);
    Vec3f_SetComponentsDuplicate(&sMewtwoDisplayObject.position, 0.0f, 0.0f, 0.0f);
    sMewtwoMaterialAlpha = 0;
}

/**
 * Entry point for demo-scene mode 24, run before VS Mewtwo //@src/game_state.c. Uses backgrounds archive
 * file 1 (daytime area-select backdrop) only in Round 1 with clearedModes bit 0x100, otherwise file 2
 * (night backdrop).
 */
s32 Stage_ShowMewtwoIntro(void) {
    StageContext* sp24;

    main_pool_push_state('MTWO');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    sp24 = StageContext_Allocate(0, 1, 3, 1, 2, 1);
    Font_Init(6, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    FRAGMENT_LOAD(FRAGMENT_PARTICLE_DATA_LIBRARY);

    Save_EnsureBankLoaded(2);
    Save_GetModeSettings(&sMewtwoModeSettings, gSessionContext.inRound2);

    if (gSessionContext.inRound2 == 1) {
        sMewtwoBackgroundArchive = ASSET_LOAD2(backgrounds, 1, 1);
        sMewtwoBackgroundTiles = BinArchive_GetFile(sMewtwoBackgroundArchive, 2);
    } else if (sMewtwoModeSettings.clearedModes & 0x100) {
        sMewtwoBackgroundArchive = ASSET_LOAD2(backgrounds, 1, 1);
        sMewtwoBackgroundTiles = BinArchive_GetFile(sMewtwoBackgroundArchive, 1);
    } else {
        sMewtwoBackgroundArchive = ASSET_LOAD2(backgrounds, 1, 1);
        sMewtwoBackgroundTiles = BinArchive_GetFile(sMewtwoBackgroundArchive, 2);
    }

    MewtwoIntro_InitScene();
    StageContext_Activate(sp24);
    MewtwoIntro_RunLoop();
    StageContext_Deactivate();
    Font_Free();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('MTWO');

    return 0;
}
