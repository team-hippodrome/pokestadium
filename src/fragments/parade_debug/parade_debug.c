#include "parade_debug.h"
#include "src/controller.h"
#include "src/display_object_textures.h"
#include "src/geo_layout.h"
#include "src/geo_render.h"
#include "src/gfx_buffer.h"
#include "src/gfx_rect.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/jpeg_stream.h"
#include "src/matrix.h"
#include "src/memory.h"
#include "src/model_renderer.h"
#include "src/stage_loader.h"
#include "src/ui_graphics.h"

static u32 sParadeGeoLayout[] = {
    0x0C00FFFF, 0x05000000, 0x0B00001E, 0x00000000, 0x014000F0, 0x0000000F, 0x00000000, 0x00000000,
    0x05000000, 0x0D000000, 0x05000000, 0x0F000002, 0x05000000, 0x1F00FFFF, 0x00000000, 0x00000000,
    0x00000000, 0x00640064, 0x00640000, 0x05000000, 0x00000000, 0x8D000020, 0x06000000, 0x06000000,
    0x14000000, 0x002B0012, 0xFFFFFF32, 0x16FFFFFF, 0x0F000003, 0x05000000, 0x0A000000, &D_800AC840,
    0x06000000, 0x06000000, 0x06000000, 0x06000000, 0x01000000,
};

static GraphNode* sParadeRootNode;
static CameraNode* sParadeCamera;
static s16 sParadeCameraPitch;
static s16 sParadeCameraYaw;
static s16 sParadeCameraDistance;

void ParadeDebug_ReadInput(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

void ParadeDebug_InitCamera(void) {
    sParadeCamera = sParadeRootNode->children;
    sParadeCameraPitch = 0x200;
    sParadeCameraYaw = 0;
    sParadeCameraDistance = 100;
    Camera_ComputeEyeFromAngles(&sParadeCamera->view.targetPosition, &sParadeCamera->view.eyePosition,
                                sParadeCameraDistance, sParadeCameraPitch, sParadeCameraYaw);
    sParadeCamera->perspective.nearPlane = 16.0f;
    sParadeCamera->perspective.farPlane = 8192.0f;
}

void ParadeDebug_UpdateCamera(void) {
    Controller* controller = gPlayer1Controller;

    if (controller->buttonDown & BTN_DUP) {
        sParadeCameraPitch += 0x100;
    }
    if (controller->buttonDown & BTN_DDOWN) {
        sParadeCameraPitch -= 0x100;
    }
    if (controller->buttonDown & BTN_DLEFT) {
        sParadeCameraYaw += 0x100;
    }
    if (controller->buttonDown & BTN_DRIGHT) {
        sParadeCameraYaw -= 0x100;
    }
    if (controller->buttonDown & BTN_CUP) {
        sParadeCameraDistance -= 10;
    }
    if (controller->buttonDown & BTN_CDOWN) {
        sParadeCameraDistance += 10;
    }

    if (sParadeCameraPitch > 0x3F00) {
        sParadeCameraPitch = 0x3F00;
    }
    if (sParadeCameraPitch < 0) {
        sParadeCameraPitch = 0;
    }
    if (sParadeCameraDistance < 50) {
        sParadeCameraDistance = 50;
    }

    Camera_ComputeEyeFromAngles(&sParadeCamera->view.targetPosition, &sParadeCamera->view.eyePosition,
                                sParadeCameraDistance, sParadeCameraPitch, sParadeCameraYaw);
}

void ParadeDebug_DrawHud(s32 mode) {
    Color_RGB8 top_color = { 0x00, 0x00, 0x1E };
    Color_RGB8 bottom_color = { 0x00, 0x00, 0xFA };

    Ui_DrawGradientPanel(200, 190, 90, 32, &top_color, &bottom_color);
    Ui_DrawBorderedPanel(30, 190, 160, 32, 0x141);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, -2);
    Gfx_SetEnvColor(0xFF, 0xC8, 0x32, 0xFF);

    switch (mode) {
        case 0:
            // The original spells it "OPENNING".
            Font_Printf(205, 194, "OPENNING");
            break;

        case 1:
            Font_Printf(205, 194, "PLAYING");
            break;

        case 2:
            Font_Printf(205, 194, "ENDING");
            break;
    }

    if (mode == 1) {
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
        Font_Printf(35, 194, "CAMERA D:%d", sParadeCameraDistance);
        Font_Printf(35, 206, "X:%04hX Y:%04hX", sParadeCameraPitch, sParadeCameraYaw);
    }

    Font_EndTexturedTextRendering();
}

void ParadeDebug_DrawFrame(s32 mode) {
    BgStage_DrawFrame();
    GfxImage_FillCurrent(&gDisplayListHead, 0xA6BF);
    GeoRender_AdvanceFrameCounter();
    Geo_RenderRootNode(sParadeRootNode);
    ParadeDebug_DrawHud(mode);
    BgStage_AdvanceFrame();
}

void ParadeDebug_RunLoop(void) {
    s32 running = 1;

    StageFade_StartFromOpaque(0x10);

    while (running) {
        ParadeDebug_ReadInput();
        ParadeDebug_UpdateCamera();
        Parade_UpdateActors();
        if (gPlayer1Controller->buttonPressed & BTN_START) {
            running = 0;
        }
        ParadeDebug_DrawFrame(1);
    }
}

void ParadeDebug_InitScene(void) {
    MemoryBlock* sp1C = MainPool_AllocState(main_pool_get_available(), 0);

    sParadeRootNode = process_geo_layout(sp1C, &sParadeGeoLayout);
    MainPool_FinalizeAllocation(sp1C);
    ModelRenderer_InitDisplayRoots();
    PokeIcon_OpenModelArchives();
    Parade_InitActors();
    ParadeDebug_InitCamera();
}

s32 ParadeDebug_Main(UNUSED s32 arg0, UNUSED s32 arg1) {
    StageContext* sp24;

    main_pool_push_state('MINI');

    Gfx_InitDisplayListBuffers(0x20000, 0);
    sp24 = StageContext_Allocate(0, 1, 3, 1, 2, 1);
    Font_Init(6, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    FRAGMENT_LOAD(FRAGMENT_MODEL_CALLBACK_LIBRARY);
    Fragment_Load((((u32)D_8D000000 & 0x0FF00000) >> 0x14) - 0x10, _5C7A70_ROM_START, _5C7A70_ROM_END);

    ParadeDebug_InitScene();
    StageContext_Activate(sp24);
    ParadeDebug_RunLoop();
    StageContext_Deactivate();
    Font_Free();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('MINI');

    return 0;
}
