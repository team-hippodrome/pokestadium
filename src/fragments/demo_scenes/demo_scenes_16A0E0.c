#include "demo_scenes.h"
#include "src/geo_render.h"
#include "src/model_animation.h"
#include "src/geo_layout.h"
#include "src/display_object_textures.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/text_system.h"
#include "src/jpeg_stream.h"
#include "src/audio_loop_point.h"
#include "src/gfx_buffer.h"
#include "src/gfx_rect.h"
#include "src/matrix.h"
#include "src/controller.h"
#include "src/memory.h"
#include "src/stage_loader.h"

typedef struct FirstClearBonusScene {
    /* 0x00 */ s32 step;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
} FirstClearBonusScene; // size >= 0xC

typedef struct FirstClearBonusDialog {
    /* 0x00 */ s32 state;
    /* 0x04 */ s32 revealIndex;
    /* 0x08 */ s32 holdTimer;
    /* 0x0C */ s32 width;
    /* 0x10 */ s32 height;
} FirstClearBonusDialog; // size >= 0x14

#if !defined(VERSION_JP)
static u32 sFirstClearBonusTextData[] = {
    0xA4AAA4E1, 0xA4C7A4C8, 0xA4A6A1AA, 0x20A5C9A1, 0xBCA5C9A1, 0xBCA4CE0A, 0xA5B2A1BC, 0xA5E0A5DC, 0xA1BCA5A4,
    0xA4F2A5B2, 0xA5C3A5C8, 0xA4C0A1AA, 0x00000000, 0x4742A5D3, 0xA5EBA4CE, 0xA5B2A1BC, 0xA5E0A5DC, 0xA1BCA5A4,
    0xA4AC0A20, 0x20A5B9A5, 0xD4A1BCA5, 0xC9A5A2A5, 0xC3A5D7A1, 0xAA000000, 0xA4B9A4B4, 0xA4A4A4BE, 0xA1AA20A5,
    0xC9A1BCA5, 0xC9A5EAA5, 0xAAA4CE0A, 0xA5B2A1BC, 0xA5E0A5DC, 0xA1BCA5A4, 0xA4F2A5B2, 0xA5C3A5C8, 0xA4C0A1AA,
    0x00000000, 0x4742A5D3, 0xA5EBA4CE, 0xA5B2A1BC, 0xA5E0A5DC, 0xA1BCA5A4, 0xA4AC0AA4, 0xB5A4E9A4, 0xCBA5B9A5,
    0xD4A1BCA5, 0xC9A5A2A5, 0xC3A5D7A4, 0xC0A1AA00,
};
#else
// EUC-JP dialog lines. The Japanese FirstClearBonus_UpdateDialogSequence formats them directly;
// the other builds read entries 0x91..0x94 of string table 0x1E instead and leave these unused.
// Mode 0x16 (Doduo Game Boy):
// "Congratulations! You got Doduo's Game Boy!"
static char sFirstClearBonusTextData[] = "おめでとう！ ドードーの\nゲームボーイをゲットだ！";
// "The GB Building's Game Boys sped up!"
static char sDoduoSpeedUpText[] = "GBビルのゲームボーイが\n  スピードアップ！";
// Other modes (Dodrio Game Boy):
// "Amazing! You got Dodrio's Game Boy!"
static char sDodrioGameBoyText[] = "すごいぞ！ ドードリオの\nゲームボーイをゲットだ！";
// "The GB Building's Game Boys sped up even more!"
static char sDodrioSpeedUpText[] = "GBビルのゲームボーイが\nさらにスピードアップだ！";
#endif

static u32 sFirstClearBonusGeoLayout[] = {
    0x0C00FFFF, 0x05000000, 0x0B00001E,  0x00000000, 0x014000F0, 0x00000000, 0x00000000, 0x00000118,
    0x05000000, 0x0D000000, 0x05000000,  0x14000000, 0x002B0012, 0xFFFFFF32, 0x16FFFFFF, 0x0F000003,
    0x05000000, 0x0A000000, &D_800AC840, 0x06000000, 0x06000000, 0x06000000, 0x06000000, 0x01000000,
};

#if !defined(VERSION_JP)
static char** sBattleMessagesTable;
#endif
static unk_D_86002F30* sGameBoyModelData;
static GraphNode* sGameBoySceneRoot;
static DisplayObject sGameBoyDisplayObject;
static CameraNode* sGameBoyCamera;
static BinArchive* sGameBoyBackgroundArchive;
static u8* sGameBoyBackgroundTiles;
static s8 sDialogSourceText[0x100];
static s8 sDialogVisibleText[0x100];
static s16 sCameraRotationX;
static s16 sCameraRotationY;
static s16 sCameraDistance;
static FirstClearBonusScene sSceneState;
static FirstClearBonusDialog sDialog;

/**
 * Places the camera 120 units from its look-at with clip range 50..6400.
 */
void FirstClearBonus_InitCamera(void) {
    sGameBoyCamera = sGameBoySceneRoot->children;

    sCameraRotationX = 0;
    sCameraRotationY = 0;
    sCameraDistance = 0x78;

    Camera_ComputeEyeFromAngles(&sGameBoyCamera->view.targetPosition, &sGameBoyCamera->view.eyePosition, sCameraDistance, sCameraRotationX,
                                sCameraRotationY);

    sGameBoyCamera->perspective.nearPlane = 50.0f;
    sGameBoyCamera->perspective.farPlane = 6400.0f;
}

/**
 * Resets the scene state machine and sets the dialog box to its collapsed height (10).
 */
void FirstClearBonus_InitState(void) {
    sSceneState.step = 0;
    sSceneState.unk_04 = 0;
    sSceneState.unk_08 = 0;
    sDialog.height = 0xA;
}

/**
 * Empty function with no callers in this file.
 */
void FirstClearBonus_EmptyFunc(void) {
}

/**
 * Draws a rounded translucent box (x, y, w, h, rgba): four corner textures up to 8 pixels in radius plus
 * filled rectangles for the edges and center.
 */
void FirstClearBonus_DrawDialogFrame(s16 arg0, s16 arg1, s16 arg2, s16 arg3, u8 arg4, u8 arg5, u8 arg6, u8 arg7) {
    s16 sp48;
    s16 sp44;
    s16 var_s2;
    s16 var_s3;

    if (arg2 >= 0x10) {
        var_s3 = 8;
    } else {
        var_s3 = arg2 / 2;
    }

    if (arg3 >= 0x10) {
        var_s2 = 8;
    } else {
        var_s2 = arg3 / 2;
    }

    sp48 = arg2 - (var_s3 * 2);
    sp44 = arg3 - (var_s2 * 2);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetEnvColor(gDisplayListHead++, arg4, arg5, arg6, arg7);

    gDPLoadTextureBlock_4b(gDisplayListHead++, gUiPanelTopLeftTexture, G_IM_FMT_I, 16, 8, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(arg0, arg1, var_s3, var_s2, 0, 0, 0x2000 / var_s3, 0x2000 / var_s2, 0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, gUiPanelTopRightTexture, G_IM_FMT_I, 16, 8, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped((arg0 + arg2) - var_s3, arg1, var_s3, var_s2, 0, 0, 0x2000 / var_s3, 0x2000 / var_s2,
                                0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, gUiPanelBottomLeftTexture, G_IM_FMT_I, 16, 8, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(arg0, (arg1 + arg3) - var_s2, var_s3, var_s2, 0, 0, 0x2000 / var_s3, 0x2000 / var_s2,
                                0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, gUiPanelBottomRightTexture, G_IM_FMT_I, 16, 8, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped((arg0 + arg2) - var_s3, (arg1 + arg3) - var_s2, var_s3, var_s2, 0, 0, 0x2000 / var_s3,
                                0x2000 / var_s2, 0);

    gSPDisplayList(gDisplayListHead++, gUiTranslucentFillDL);

    if ((sp48 > 0) && (sp44 > 0)) {
        Gfx_FillRectRgba(arg0 + var_s3, arg1, sp48, var_s2, arg4, arg5, arg6, arg7);
        Gfx_FillRectRgba(arg0, arg1 + var_s3, arg2, sp44, arg4, arg5, arg6, arg7);
        Gfx_FillRectRgba(arg0 + var_s3, (arg1 + var_s3) + sp44, sp48, var_s2, arg4, arg5, arg6, arg7);
    } else if (sp48 > 0) {
        Gfx_FillRectRgba(arg0 + var_s3, arg1, sp48, arg3, arg4, arg5, arg6, arg7);
    } else if (sp44 > 0) {
        Gfx_FillRectRgba(arg0, arg1 + var_s2, arg2, sp44, arg4, arg5, arg6, arg7);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

/**
 * Sets the dialog state to 0xFF, in which the box collapses and stays hidden.
 */
void FirstClearBonus_CloseDialog(void) {
    sDialog.state = 0xFF;
}

/**
 * Sets the dialog state to 0, which loads the first message and opens the box.
 */
void FirstClearBonus_StartDialog(void) {
    sDialog.state = 0;
}

#if defined(VERSION_JP)
// Reveals one character of the dialog text per call: a leading newline or space is copied first,
// then one byte if it is 0x21..0x3F, otherwise two bytes (an EUC-JP character).
/**
 * Typewriter effect: copies the next part of sDialogSourceText into sDialogVisibleText each frame.
 * Returns 1 when the terminator is reached.
 */
s32 FirstClearBonus_RevealNextChar(void) {
    if (sDialogSourceText[sDialog.revealIndex] == 0) {
        return 1;
    }

    if ((sDialogSourceText[sDialog.revealIndex] == '\n') || (sDialogSourceText[sDialog.revealIndex] == ' ')) {
        sDialogVisibleText[sDialog.revealIndex] = sDialogSourceText[sDialog.revealIndex];
        sDialog.revealIndex++;
    }

    if ((sDialogSourceText[sDialog.revealIndex] > 0x20) && (sDialogSourceText[sDialog.revealIndex] < 0x40)) {
        sDialogVisibleText[sDialog.revealIndex] = sDialogSourceText[sDialog.revealIndex];
        sDialog.revealIndex++;
    } else {
        sDialogVisibleText[sDialog.revealIndex] = sDialogSourceText[sDialog.revealIndex];
        sDialogVisibleText[sDialog.revealIndex + 1] = sDialogSourceText[sDialog.revealIndex + 1];
        sDialog.revealIndex += 2;
    }
    return 0;
}
#else
/**
 * Typewriter effect: copies the next part of sDialogSourceText into sDialogVisibleText each frame.
 * Returns 1 when the terminator is reached.
 */
s32 FirstClearBonus_RevealNextChar(void) {
    if (sDialogSourceText[sDialog.revealIndex] == 0) {
        return 1;
    }

    sDialogVisibleText[sDialog.revealIndex++] = sDialogSourceText[sDialog.revealIndex];

    if (sDialogSourceText[sDialog.revealIndex] == 0) {
        return 1;
    }

    sDialogVisibleText[sDialog.revealIndex++] = sDialogSourceText[sDialog.revealIndex];
    return 0;
}
#endif

/**
 * Draws the dialog box 200 pixels wide (current width), centered vertically on y = 190.
 */
void FirstClearBonus_DrawDialogBox(void) {
    // #5050F0 at alpha 0x80
    FirstClearBonus_DrawDialogFrame(0x3C, 0xBE - (sDialog.height / 2), sDialog.width, sDialog.height, 0x50,
                                    0x50, 0xF0, 0x80);
}

/**
 * Prints the revealed dialog text with font 2 at (86, 175).
 */
void FirstClearBonus_DrawDialogText(void) {
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, 0);
    Font_SetLineHeight(0x10);
    Font_Printf(0x56, 0xAF, "%s", sDialogVisibleText);
    Font_EndTexturedTextRendering();
}

/**
 * Dialog state machine: loads the "got the Game Boy" message (Doduo for mode 22, Dodrio otherwise), opens
 * the box, reveals it, holds 60 frames, then reveals the "GB Building Game Boys speed up" message, holds
 * 65 frames and collapses the box.
 */
void FirstClearBonus_UpdateDialogSequence(void) {
    s32 i;

    switch (sDialog.state) {
        case 0x0:
#if defined(VERSION_JP)
            if (gSessionContext.mode == 0x16) {
                sprintf(sDialogSourceText, sFirstClearBonusTextData);
            } else {
                sprintf(sDialogSourceText, sDodrioGameBoyText);
            }
#else
            if (gSessionContext.mode == 0x16) {
                sprintf(sDialogSourceText, Text_GetString(NULL, 0, sBattleMessagesTable, 0x91));
            } else {
                sprintf(sDialogSourceText, Text_GetString(NULL, 0, sBattleMessagesTable, 0x93));
            }
#endif

            sDialog.revealIndex = 0;
            sDialog.holdTimer = 0;
            sDialog.width = 0xC8;
            sDialog.height = 0xA;

            // clang-format off
            for (i = 0; i < 255; i++) { sDialogVisibleText[i] = 0; }
            // clang-format on

            FirstClearBonus_DrawDialogBox();
            sDialog.state++;
            break;

        case 0x1:
            sDialog.height = Math_StepToS32(sDialog.height, 0x28, 8, 8);
            if (sDialog.height == 0x28) {
                sDialog.state++;
            }
            FirstClearBonus_DrawDialogBox();
            break;

        case 0x2:
            FirstClearBonus_DrawDialogBox();
            if (FirstClearBonus_RevealNextChar() != 0) {
                sDialog.state++;
            }
            FirstClearBonus_DrawDialogText();
            break;

        case 0x3:
            FirstClearBonus_DrawDialogBox();
            FirstClearBonus_DrawDialogText();

            if (sDialog.holdTimer++ == 0x3C) {
#if defined(VERSION_JP)
                if (gSessionContext.mode == 0x16) {
                    sprintf(sDialogSourceText, sDoduoSpeedUpText);
                } else {
                    sprintf(sDialogSourceText, sDodrioSpeedUpText);
                }
#else
                if (gSessionContext.mode == 0x16) {
                    sprintf(sDialogSourceText, Text_GetString(NULL, 0, sBattleMessagesTable, 0x92));
                } else {
                    sprintf(sDialogSourceText, Text_GetString(NULL, 0, sBattleMessagesTable, 0x94));
                }
#endif

                sDialog.revealIndex = 0;
                sDialog.holdTimer = 0;

                // clang-format off
                for (i = 0; i < 255; i++) { sDialogVisibleText[i] = 0; }
                // clang-format on

                sDialog.state++;
            }
            break;

        case 0x4:
            FirstClearBonus_DrawDialogBox();
            if (FirstClearBonus_RevealNextChar() != 0) {
                sDialog.state++;
            }
            FirstClearBonus_DrawDialogText();
            break;

        case 0x5:
            FirstClearBonus_DrawDialogBox();
            FirstClearBonus_DrawDialogText();

            if (sDialog.holdTimer++ == 0x41) {
                sDialog.state = 0xFF;
            }
            break;

        case 0xFF:
            sDialog.height = Math_StepToS32(sDialog.height, 0, 8, 8);
            if (sDialog.height >= 0xB) {
                FirstClearBonus_DrawDialogBox();
            }
            break;
    }
}

/**
 * Polls the controllers once per frame and resets input repeat state.
 */
void FirstClearBonus_ReadControllerInput(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

/**
 * Empty function with no callers in this file.
 */
void FirstClearBonus_NoopStub2(void) {
}

/**
 * Draws a 320x240 RGBA16 backdrop as 20 x 15 tiles of 16x16 pixels (0x200 bytes each).
 */
void FirstClearBonus_DrawBackgroundTiles(u8* arg0) {
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
 * Draws the backdrop, clears depth, renders the Game Boy model and updates/draws the dialog.
 */
void FirstClearBonus_RenderFrame(void) {
    BgStage_DrawFrame();
    FirstClearBonus_DrawBackgroundTiles(sGameBoyBackgroundTiles);
    GfxImage_ClearDepthRectangle(&gDisplayListHead, 0, 0, 0x140, 0xF0);
    GeoRender_AdvanceFrameCounter();
    Geo_RenderRootNode(sGameBoySceneRoot);
    FirstClearBonus_UpdateDialogSequence();
    BgStage_AdvanceFrame();
}

/**
 * Scene state machine: shows the model, starts music 0x38 at animation frame 10, and at frame 114 clears
 * render flag 0x20 and starts the dialog with music 0x13; when the dialog closes, sets 0x20 again and
 * waits for the animation to finish. Returns 1 when done.
 */
s32 FirstClearBonus_UpdateScene(void) {
    s32 sp24 = 0;

    switch (sSceneState.step) {
        case 0:
            sGameBoyDisplayObject.node.renderFlags |= 0x20;
            sGameBoyDisplayObject.node.flags |= 1;
            sSceneState.step++;
            break;

        case 1:
            if ((sGameBoyDisplayObject.transformAnim.frameFixed >> 0x10) == 0xA) {
                Audio_StartMusicTrack(0x38);
            }

            if ((sGameBoyDisplayObject.transformAnim.frameFixed >> 0x10) == 0x72) {
                sSceneState.unk_08 = 0;
                sGameBoyDisplayObject.node.renderFlags &= 0xFFDF;
                FirstClearBonus_StartDialog();
                Audio_StartMusicTrack(0x13);
                sSceneState.step++;
            }
            break;

        case 2:
            if (sDialog.state == 0xFF) {
                sSceneState.unk_08 = 0;
                sGameBoyDisplayObject.node.renderFlags |= 0x20;
                FirstClearBonus_CloseDialog();
                sSceneState.step++;
            }
            break;

        case 3:
            if (ModelAnim_IsAnimationDone(&sGameBoyDisplayObject) != 0) {
                sSceneState.step++;
            }
            break;

        case 4:
            sp24 = 1;
            break;
    }

    return sp24;
}

/**
 * Fades in over 10 frames, runs the scene until it finishes, then fades to white over 8 frames.
 */
void FirstClearBonus_RunLoop(void) {
    s32 i;
    s32 var_s2 = 1;

    StageContext_SetClearColor(1);
    StageFade_StartFromOpaque(0xA);

    for (i = 0; i < 10; i++) {
        FirstClearBonus_ReadControllerInput();
        FirstClearBonus_RenderFrame();
    }

    while (var_s2 != 0) {
        FirstClearBonus_ReadControllerInput();
        if (FirstClearBonus_UpdateScene() != 0) {
            var_s2 = 0;
        }
        FirstClearBonus_RenderFrame();
    }

    StageContext_SetClearColor(0xFFFF);
    StageFade_StartFromTransparent(8);

    for (i = 0; i < 8; i++) {
        FirstClearBonus_ReadControllerInput();
        FirstClearBonus_RenderFrame();
    }

    StageLoader_RunFrames(2);
}

/**
 * Builds the scene and loads the Game Boy model: archive entry 0xD5 for mode 22 (Doduo) or 0xD4 for
 * mode 23 (Dodrio), hidden until FirstClearBonus_UpdateScene starts.
 */
void FirstClearBonus_InitScene(void) {
    MemoryBlock* temp_v0 = MainPool_AllocState(main_pool_get_available(), 0);

    sGameBoySceneRoot = process_geo_layout(temp_v0, &sFirstClearBonusGeoLayout);
    MainPool_FinalizeAllocation(temp_v0);

    ModelRenderer_InitDisplayRoots();
    FirstClearBonus_InitCamera();
    FirstClearBonus_InitState();
    FirstClearBonus_CloseDialog();
    ModelRenderer_AttachDisplayObject(&sGameBoyDisplayObject);
    PokeIcon_OpenModelArchives();

    if (gSessionContext.mode == 0x16) {
        sGameBoyModelData = Model_LoadByArchiveIndex(0xD5);
        Model_InitDisplayObject(&sGameBoyDisplayObject, 0, 0xD5, sGameBoyModelData->unk_08->unk_00[0]);
    } else {
        sGameBoyModelData = Model_LoadByArchiveIndex(0xD4);
        Model_InitDisplayObject(&sGameBoyDisplayObject, 0, 0xD4, sGameBoyModelData->unk_08->unk_00[0]);
    }

    ModelAnim_SetAnimation(&sGameBoyDisplayObject, 0);

    sGameBoyDisplayObject.node.renderFlags &= ~0x20;
    sGameBoyDisplayObject.node.flags &= ~1;

    Vec3f_SetComponentsDuplicate(&sGameBoyDisplayObject.position, 0.0f, 11.0f, -105.0f);
}

/**
 * Entry point for demo-scene modes 22 and 23, run after a clear that speeds up the GB Tower
 * (transitionFlags & 8 / & 0x10) //@src/game_state.c. Backgrounds archive file 0x12 or 0x13 is the GB
 * Building backdrop.
 */
s32 Stage_ShowFirstClearBonusScene(void) {
    StageContext* sp24;

    main_pool_push_state('GBOY');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    sp24 = StageContext_Allocate(0, 1, 3, 1, 2, 1);
    Font_Init(2, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_2000000, common_menu2_ui, 0);
    FRAGMENT_LOAD(FRAGMENT_PARTICLE_DATA_LIBRARY);

#if !defined(VERSION_JP)
    Text_InitStringTables();
    sBattleMessagesTable = Text_GetStringTable(0x1E);
#endif

    if (gSessionContext.mode == 0x16) {
        sGameBoyBackgroundArchive = ASSET_LOAD2(backgrounds, 1, 1);
        sGameBoyBackgroundTiles = BinArchive_GetFile(sGameBoyBackgroundArchive, 0x12);
    } else {
        sGameBoyBackgroundArchive = ASSET_LOAD2(backgrounds, 1, 1);
        sGameBoyBackgroundTiles = BinArchive_GetFile(sGameBoyBackgroundArchive, 0x13);
    }

    FirstClearBonus_InitScene();
    StageContext_Activate(sp24);
    FirstClearBonus_RunLoop();
    StageContext_Deactivate();
    Font_Free();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('GBOY');

    return 0;
}
