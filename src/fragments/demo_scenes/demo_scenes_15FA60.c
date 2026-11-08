#include "demo_scenes.h"
#include "src/geo_render.h"
#include "src/model_animation.h"
#include "src/geo_layout.h"
#include "src/display_object_textures.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/save_data.h"
#include "src/text_system.h"
#include "src/jpeg_stream.h"
#include "src/audio_loop_point.h"
#include "src/gfx_rect.h"
#include "src/matrix.h"
#include "src/controller.h"
#include "src/fragments/particle_math/particle_math.h"
#include "src/memory.h"
#include "src/stage_loader.h"

typedef struct DioramaLight {
    /* 0x00 */ s16 rotationX;
    /* 0x02 */ s16 rotationY;
    /* 0x04 */ u8 a;
    /* 0x05 */ u8 r;
    /* 0x06 */ u8 g;
    /* 0x07 */ u8 b;
    /* 0x08 */ s16 rotationXStep;
    /* 0x0A */ s16 rotationYStep;
} DioramaLight; // size = 0xC

extern u32 sDioramaSkyGradientTex[];
extern DioramaObjectDef* sDioramaObjectDefs[];
extern DioramaAnimData sDioramaAnimData[];
extern DioramaPresetData sDioramaPresetData[];
extern u32 sDioramaCloudLayerLayout[];
extern u32 sDioramaWaterLayerLayout[];

s32 Diorama_LightCallback(s32, GraphNode*);
s32 Diorama_CloudLayerCallback(s32, GraphNode*);
s32 Diorama_WaterLayerCallback(s32, GraphNode*);
s32 Diorama_LayerSwitchCallback(s32, GraphNode*);

// rotationX of the cloud layer (flying group) and water layer (water group).
static s16 sDioramaCloudRotationX = -0x3C6E;
static s16 sDioramaWaterRotationX = 0x38E;

/**
 * Scene graph of the boot diorama: camera, a switch node that shows the cloud layer for group 2
 * and the water layer for group 3, a light node driven by Diorama_LightCallback, and the model
 * display list root.
 */
static u32 sDioramaGeoLayout[] = {
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
    0x1A050000,
    0x08000000,
    Diorama_LayerSwitchCallback,
    0x00000000,
    0x05000000,
    0x0F000002,
    0x05000000,
    0x06000000,
    0x0F000002,
    0x05000000,
    0x06000000,
    0x0F000002,
    0x05000000,
    0x1F00FFFF,
    0xC3920000,
    0x00000000,
    0xFC400000,
    0x00640064,
    0x00640000,
    0x08000000,
    Diorama_CloudLayerCallback,
    0x00000000,
    0x05000000,
    0x00000000,
    sDioramaCloudLayerLayout,
    0x06000000,
    0x06000000,
    0x0F000002,
    0x05000000,
    0x1F00FFFF,
    0x00000000,
    0x00000000,
    0x0514F79A,
    0x00640064,
    0x00640000,
    0x08000000,
    Diorama_WaterLayerCallback,
    0x00000000,
    0x05000000,
    0x00000000,
    sDioramaWaterLayerLayout,
    0x06000000,
    0x06000000,
    0x0F000002,
    0x05000000,
    0x06000000,
    0x06000000,
    0x14000000,
    0xF8E40000,
    0xFFC89BFF,
    0x08000000,
    Diorama_LightCallback,
    0x00000000,
    0x16FFFFFF,
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

static GraphNode* sDioramaRootNode;
static DioramaScene* sDioramaScene;
static DioramaLight* sDioramaLight;
// Set when the finale ends; enables the title background and stops the no-controller warning.
s32 sDioramaSequenceFinished;
#if !defined(VERSION_JP)
// String table 0x14, read by Diorama_DrawNoControllerWarning. The Japanese build has neither, and its .bss
// places sDioramaBackgroundArchive directly after sDioramaSequenceFinished.
// String table 0x14; entry 0 is the "Controller 1 is not connected" message.
char** sControllerErrorText;
#endif
static BinArchive* sDioramaBackgroundArchive;
static u8* sDioramaTitleBackground;
static DioramaModelSlot sDioramaModelSlots[4];
static s32 sDioramaSkipFadeAlpha;
// Return value of Stage_ShowBootDiorama: 1 = played to the end, 2 = skipped.
static s32 sDioramaExitCode;
// Mode settings word compared with 0x1F8 (0x1FE in Japan) to pick the title background.
// Save_GetModeSettings writes a whole ModeSettings (8 bytes) here, more than this u16.
static u16 sDioramaClearedModes;

/**
 * Geo callback for the diorama light node. Each frame, advances the light rotationX/Y by the
 * current group's step and copies the rotation and color into the node.
 */
s32 Diorama_LightCallback(s32 arg0, GraphNode* arg1) {
    if (arg0 == 2) {
        unk_D_86002F34_alt18* ptr = (unk_D_86002F34_alt18*)arg1;

        sDioramaLight->rotationX += sDioramaLight->rotationXStep;
        sDioramaLight->rotationY += sDioramaLight->rotationYStep;

        ptr->unk_1C = sDioramaLight->rotationX;
        ptr->unk_1E = sDioramaLight->rotationY;
        ptr->r = sDioramaLight->r;
        ptr->g = sDioramaLight->g;
        ptr->b = sDioramaLight->b;
        ptr->a = sDioramaLight->a;
    }

    return 0;
}

/**
 * Geo callback for the cloud layer drawn under the flying group: scales it by 1.5 and tilts it
 * by sDioramaCloudRotationX.
 */
s32 Diorama_CloudLayerCallback(s32 arg0, GraphNode* arg1) {
    if (arg0 == 2) {
        DisplayObject* ptr = (DisplayObject*)arg1;

        ptr->scale.x = 1.5f;
        ptr->scale.y = 1.5f;
        ptr->scale.z = 1.5f;
        ptr->rotation.x = sDioramaCloudRotationX;
    }

    return 0;
}

/**
 * Geo callback for the water surface layer drawn in the water group: unit scale, tilted by
 * sDioramaWaterRotationX.
 */
s32 Diorama_WaterLayerCallback(s32 arg0, GraphNode* arg1) {
    if (arg0 == 2) {
        DisplayObject* ptr = (DisplayObject*)arg1;

        ptr->scale.x = 1.0f;
        ptr->scale.y = 1.0f;
        ptr->scale.z = 1.0f;

        ptr->rotation.x = sDioramaWaterRotationX;
    }
    return 0;
}

/**
 * Geo callback for the switch node that picks which layer is drawn. The selected child is the
 * current model group, so group 2 shows the cloud layer and group 3 the water layer.
 */
s32 Diorama_LayerSwitchCallback(s32 arg0, GraphNode* arg1) {
    if (arg0 == 2) {
        unk_D_86002F34_alt21* ptr = (unk_D_86002F34_alt21*)arg1;

        ptr->unk_1A = sDioramaScene->sequence.group;
    }
    return 0;
}

/**
 * Marks all four model slots as unloaded.
 */
void Diorama_ClearModelSlots(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        sDioramaModelSlots[i].isLoaded = 0;
    }
}

/**
 * Detaches the display objects of the previous group and pops the 'MONS' pool state.
 *
 * //!bug The loop runs while isLoaded != 1, so it stops at the first loaded slot. Slot 0 is
 * always loaded when this is called, so no display object is detached.
 */
void Diorama_FreeModelSlots(DioramaModelSlot* arg0) {
    while (arg0->isLoaded != 1) {
        ModelRenderer_ClearDisplayObject(&arg0->displayObject);
        arg0++;
    }
    main_pool_pop_state('MONS');
}

/**
 * Pushes the 'MONS' pool state that holds the current group's models.
 */
void Diorama_PushModelPool(void) {
    main_pool_push_state('MONS');
}

/**
 * Loads the model for an object definition into a slot, attaches its display object and starts
 * the definition's animation at frame 0. Model 6 (Charizard) gets effect slot 0 (probably its
 * tail flame); every other model gets 0xFF (no effect).
 */
void Diorama_InitModelSlot(DioramaModelSlot* arg0, DioramaObjectDef* arg1) {
    ModelRenderer_AttachDisplayObject(&arg0->displayObject);

    arg0->model = Model_LoadByArchiveIndex(arg1->modelId);

    Model_InitDisplayObject(&arg0->displayObject, 0, arg1->modelId, arg0->model->unk_08->unk_00[0]);
    ModelAnim_SetAnimation(&arg0->displayObject, arg1->animationIndex);

    if (arg1->modelId == 6) {
        arg0->displayObject.effectSlot = 0;
    } else {
        arg0->displayObject.effectSlot = 0xFF;
    }

    ModelAnim_SetFrame(&arg0->displayObject, 0);
    arg0->displayObject.node.renderFlags &= ~0x40;
}

/**
 * Copies an object definition's position to the slot's display object.
 */
void Diorama_SetModelSlotPosition(DioramaModelSlot* arg0, DioramaObjectDef* arg1) {
    arg0->displayObject.position.x = arg1->position.x;
    arg0->displayObject.position.y = arg1->position.y;
    arg0->displayObject.position.z = arg1->position.z;
}

/**
 * Loads the light rotation, rotation steps and color for a model group from sDioramaPresetData.
 */
void Diorama_SetLightFromGroup(s32 arg0) {
    sDioramaLight->rotationX = sDioramaPresetData[arg0].lightRotationX;
    sDioramaLight->rotationY = sDioramaPresetData[arg0].lightRotationY;
    sDioramaLight->rotationXStep = sDioramaPresetData[arg0].lightRotationXStep;
    sDioramaLight->rotationYStep = sDioramaPresetData[arg0].lightRotationYStep;
    sDioramaLight->r = sDioramaPresetData[arg0].lightR;
    sDioramaLight->g = sDioramaPresetData[arg0].lightG;
    sDioramaLight->b = sDioramaPresetData[arg0].lightB;
    sDioramaLight->a = sDioramaPresetData[arg0].lightA;
}

/**
 * Loads the models of the first group (no previous group to free) and sets its light.
 */
void Diorama_LoadModelGroup(s32 arg0) {
    DioramaObjectDef* var_s0 = sDioramaObjectDefs[arg0];
    DioramaModelSlot* var_s1 = sDioramaModelSlots;

    Diorama_ClearModelSlots();
    Diorama_PushModelPool();

    while (var_s0->modelId != 0x98) {
        Diorama_InitModelSlot(var_s1, var_s0);
        Diorama_SetModelSlotPosition(var_s1, var_s0);
        var_s1->isLoaded = 1;

        var_s1++;
        var_s0++;
    }

    Diorama_SetLightFromGroup(arg0);
}

/**
 * Frees the previous group and loads the models of the given group, then sets its light. The
 * finale group (4) also starts the event track of its only model.
 */
void Diorama_SwitchModelGroup(s32 arg0) {
    DioramaObjectDef* sp2C = sDioramaObjectDefs[arg0];
    DioramaModelSlot* var_s1 = sDioramaModelSlots;

    Diorama_FreeModelSlots(sDioramaModelSlots);
    Diorama_ClearModelSlots();
    Diorama_PushModelPool();

    while (sp2C->modelId != 0x98) {
        Diorama_InitModelSlot(var_s1, sp2C);
        Diorama_SetModelSlotPosition(var_s1, sp2C);
        var_s1->isLoaded = 1;

        var_s1++;
        sp2C++;
    }

    Diorama_SetLightFromGroup(arg0);

    if (arg0 == 4) {
        ModelAnim_SetEventTrack(&sDioramaModelSlots[0].displayObject, 0);
    }
}

/**
 * Places the camera for a model group. The finale group uses a fixed camera; the other groups
 * load their camera keyframe from sDioramaAnimData around the group's focus point.
 */
void Diorama_SetGroupCamera(s32 arg0) {
    if (arg0 == 4) {
        sDioramaScene->camera->view.targetPosition.x = -0.04f;
        sDioramaScene->camera->view.targetPosition.y = 24.2f;
        sDioramaScene->camera->view.targetPosition.z = -4.08f;
        sDioramaScene->camera->view.eyePosition.x = 0.04f;
        sDioramaScene->camera->view.eyePosition.y = 24.2f;
        sDioramaScene->camera->view.eyePosition.z = 177.41f;
        sDioramaScene->camera->perspective.verticalFieldOfView = 25.0f;
    } else {
        sDioramaScene->sequence.focus.x = sDioramaPresetData[arg0].cameraFocus.x;
        sDioramaScene->sequence.focus.y = sDioramaPresetData[arg0].cameraFocus.y;
        sDioramaScene->sequence.focus.z = sDioramaPresetData[arg0].cameraFocus.z;

        Diorama_LoadCameraKeyframe(&sDioramaAnimData[arg0], sDioramaScene);
        Diorama_SetCameraToKeyframeStart(&sDioramaScene->sequence, sDioramaScene);
    }
}

/**
 * Empty function.
 */
void func_86B00680(void) {
}

/**
 * Restarts any finished model animation of the current group so the Pokémon keep moving. Does
 * nothing during the load states (5-8) or in the finale group.
 */
void Diorama_LoopIdleAnimations(void) {
    s32 i = 0;

    if ((sDioramaScene->sequence.state != 5) && (sDioramaScene->sequence.state != 6) && (sDioramaScene->sequence.state != 7) &&
        (sDioramaScene->sequence.state != 8)) {
        if (sDioramaScene->sequence.group != 4) {
            while (sDioramaObjectDefs[sDioramaScene->sequence.group][i].modelId != 0x98) {
                if (ModelAnim_IsAnimationDone(&sDioramaModelSlots[i].displayObject) != 0) {
                    ModelAnim_SetFrame(&sDioramaModelSlots[i].displayObject, 0);
                }
                i++;
            }
        }
    }
}

/**
 * After the player skips the diorama, draws a black fullscreen rectangle that fades in.
 */
void Diorama_DrawSkipFadeOverlay(void) {
    if (sDioramaScene->sequence.skipRequested == 1) {
        Gfx_FillRectWithDisplayState(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, sDioramaSkipFadeAlpha);
    }
}

/**
 * Draws the sky gradient behind the flying group (group 2).
 */
void Diorama_DrawSkyGradient(void) {
    if (sDioramaScene->sequence.group == 2) {
        Diorama_DrawFullscreenTexture(&sDioramaSkyGradientTex, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 4, 0x80);
    }
}

/**
 * Moves every model of a group 20 units along +z. Used by the flying and water groups, whose
 * camera stays still while the Pokémon pass it. Returns 1 once all models are past z = 650.
 */
s32 Diorama_SlideModelsForward(s32 arg0) {
    DioramaModelSlot* var_v0 = sDioramaModelSlots;
    DioramaObjectDef* var_v1 = sDioramaObjectDefs[arg0];
    s32 var_a1 = 1;

    while (var_v1->modelId != 0x98) {
        var_v0->displayObject.position.z += 20.0f;
        if (var_v0->displayObject.position.z <= 650.0f) {
            var_a1 = 0;
        }
        var_v0++;
        var_v1++;
    }
    return var_a1;
}

/**
 * Starts the 12-frame fade out at the end of a model group.
 */
void Diorama_BeginGroupTransition(void) {
    StageContext_SetClearColor(1);
    StageFade_StartFromTransparent(0xC);
    sDioramaScene->sequence.state = 0;
    sDioramaScene->sequence.stateTimer = 0;
}

/**
 * Called when the finale animation is done: hides the finale model and sets
 * sDioramaSequenceFinished, which makes Diorama_DrawTitleBackground draw the title screen art.
 */
void Diorama_FinishSequence(void) {
    sDioramaScene->sequence.state = 9;
    sDioramaScene->sequence.stateTimer = 0;
    sDioramaModelSlots->displayObject.node.flags &= ~1;
    sDioramaSequenceFinished = 1;
}

/**
 * Shows only the models of a group that are inside a triangle in front of the camera (on the
 * xz plane) and hides the rest. The triangle's half angles, depth and back offset are chosen per
 * group.
 */
void Diorama_UpdateModelVisibility(s32 arg0) {
    DioramaObjectDef* ptr1 = sDioramaObjectDefs[arg0];
    DioramaModelSlot* ptr2 = &sDioramaModelSlots[0];
    s16 sp88[] = {
        // D_86B0BD20
        0x1554,
        0x1554,
        0x1554,
        0x1554,
    };
    s16 sp80[] = {
        // D_86B0BD28
        0x18E2,
        0x11C6,
        0x1554,
        0x1554,
    };
    f32 sp70[] = { 1000.0f, 1000.0f, 2000.0f, 2000.0f }; // D_86B0BD30
    f32 sp60[] = { 1.0f, 1.0f, 100.0f, 50.0f };          // D_86B0BD40

    while (ptr1->modelId != 0x98) {
        if (Diorama_IsModelInCameraCone(sDioramaScene->camera, &ptr2->displayObject, sp88[arg0], sp80[arg0], sp70[arg0],
                                        sp60[arg0]) != 0) {
            ptr2->displayObject.node.flags |= 1;
        } else {
            ptr2->displayObject.node.flags &= ~1;
        }
        ptr2++;
        ptr1++;
    }
}

/**
 * Reads the controllers for this frame.
 */
void Diorama_ReadControllerInput(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

/**
 * Allocates the scene and light state, sets up the camera and starts in the wait state (8).
 */
void Diorama_InitSceneState(void) {
    sDioramaScene = main_pool_alloc(sizeof(DioramaScene), 0);
    bzero(sDioramaScene, sizeof(DioramaScene));

    sDioramaLight = main_pool_alloc(sizeof(DioramaLight), 0);
    bzero(sDioramaLight, sizeof(DioramaLight));

    sDioramaScene->camera = sDioramaRootNode->children;

    sDioramaScene->camera->perspective.nearPlane = 50.0f;
    sDioramaScene->camera->perspective.farPlane = 8400.0f;
    sDioramaScene->camera->perspective.verticalFieldOfView = 30.0f;
    sDioramaScene->camera->view.targetPosition.x = 50.0f;
    sDioramaScene->camera->view.targetPosition.y = 50.0f;
    sDioramaScene->camera->view.targetPosition.z = 50.0f;
    sDioramaScene->camera->view.eyePosition.x = 0.0f;
    sDioramaScene->camera->view.eyePosition.y = 0.0f;
    sDioramaScene->camera->view.eyePosition.z = 0.0f;
    sDioramaScene->sequence.state = 8;
    sDioramaScene->sequence.stateTimer = 0;
    sDioramaScene->sequence.skipRequested = 0;
}

/**
 * Updates the boot diorama for one frame and returns 1 when it should end.
 *
 * Pressing Start, A or B skips the diorama: music fades, the screen fades to black and the exit
 * code becomes 2. When the finale ends normally the exit code is 1.
 *
 * States:
 * - 8: wait 12 frames, then load group 0 and start music track 0x29.
 * - 6: wait 5 frames, then fade in.
 * - 7: run the group. Groups 0 and 1 move the camera along their keyframe until it is near
 *      the end; groups 2 and 3 slide the models past a still camera; group 4 plays the finale
 *      animation.
 * - 0-4 (default): keep moving during the 15-frame fade out.
 * - 5: load the next group, or end the diorama after group 4.
 * - 9: short wait after the finale before state 5.
 */
s32 Diorama_UpdateStateMachine(void) {
    // The Japanese build skips the diorama with A (0x8000) or B (0x4000) only; Start (0x1000) is
    // not tested (its code has no andi 0x1000 before the A/B tests).
#if defined(VERSION_JP)
    if (((gPlayer1Controller->buttonPressed & 0x8000) || (gPlayer1Controller->buttonPressed & 0x4000)) &&
        (sDioramaScene->sequence.skipRequested == 0)) {
#else
    if (((gPlayer1Controller->buttonPressed & 0x1000) || (gPlayer1Controller->buttonPressed & 0x8000) ||
         (gPlayer1Controller->buttonPressed & 0x4000)) &&
        (sDioramaScene->sequence.skipRequested == 0)) {
#endif
        if ((sDioramaScene->sequence.state != 8) && (sDioramaScene->sequence.state != 5)) {
            sDioramaScene->sequence.skipRequested = 1;
            Audio_FadeOutAll(0xA);
            sDioramaSkipFadeAlpha = 0;
            sDioramaExitCode = 2;
        }
    }

    if (sDioramaScene->sequence.skipRequested == 1) {
        sDioramaSkipFadeAlpha = Math_StepToS32(sDioramaSkipFadeAlpha, 0xFF, 0x1E, 0x1E);
        if (sDioramaSkipFadeAlpha == 0xFF) {
            return 1;
        }
    }

    switch (sDioramaScene->sequence.state) {
        case 5:
            if (sDioramaScene->sequence.skipRequested == 1) {
                return 1;
            }

            if (sDioramaScene->sequence.group == 4) {
                sDioramaScene->sequence.stateTimer = 0;
                sDioramaExitCode = 1;
                return 1;
            }

            sDioramaScene->sequence.group++;
            Diorama_SetGroupCamera(sDioramaScene->sequence.group);
            Diorama_SwitchModelGroup(sDioramaScene->sequence.group);
            sDioramaScene->sequence.state = 6;
            sDioramaScene->sequence.stateTimer = 0;
            break;

        case 6:
            if (sDioramaScene->sequence.stateTimer++ >= 4) {
                StageContext_SetClearColor(1);
                StageFade_StartFromOpaque(0x20);
                sDioramaScene->sequence.state = 7;
                if ((sDioramaScene->sequence.group == 0) || (sDioramaScene->sequence.group == 1)) {
                    Diorama_UpdateCameraKeyframe(&sDioramaScene->sequence, sDioramaScene);
                }
            }
            break;

        case 7:
            if (sDioramaScene->sequence.group == 4) {
                if (ModelAnim_IsAnimationDone(&sDioramaModelSlots[0].displayObject) != 0) {
                    Diorama_FinishSequence();
                }
            } else {
                if (sDioramaScene->sequence.group >= 2) {
                    if (Diorama_SlideModelsForward(sDioramaScene->sequence.group) != 0) {
                        Diorama_BeginGroupTransition();
                    }
                } else if (sDioramaScene->sequence.remainingDistance < sDioramaScene->sequence.advanceDistance) {
                    Diorama_BeginGroupTransition();
                }

                if ((sDioramaScene->sequence.group == 0) || (sDioramaScene->sequence.group == 1)) {
                    Diorama_UpdateCameraKeyframe(&sDioramaScene->sequence, sDioramaScene);
                }

                Diorama_UpdateModelVisibility(sDioramaScene->sequence.group);
            }
            break;

        case 8:
            if (sDioramaScene->sequence.stateTimer++ >= 0xB) {
                sDioramaScene->sequence.group = 0;
                Diorama_SetGroupCamera(sDioramaScene->sequence.group);
                sDioramaScene->sequence.state = 6;
                sDioramaScene->sequence.stateTimer = 0;
                Diorama_LoadModelGroup(sDioramaScene->sequence.group);
                Audio_StartMusicTrack(0x29);
            }
            break;

        case 9:
            if (sDioramaScene->sequence.stateTimer++ >= 3) {
                sDioramaScene->sequence.state = 5;
                sDioramaScene->sequence.stateTimer = 0;
            }
            break;

        default:
            if (sDioramaScene->sequence.group != 4) {
                if (sDioramaScene->sequence.group >= 2) {
                    Diorama_SlideModelsForward(sDioramaScene->sequence.group);
                } else {
                    Diorama_UpdateCameraKeyframe(&sDioramaScene->sequence, sDioramaScene);
                }
            }

            if (sDioramaScene->sequence.stateTimer++ >= 0xE) {
                sDioramaScene->sequence.state = 5;
                sDioramaScene->sequence.stateTimer = 0;
            }
            break;
    }

    return 0;
}

/**
 * Empty function.
 */
void func_86B01004(void) {
}

/**
 * Draws one frame: sky gradient, title background (only after the finale), the 3D scene, the
 * skip fade and, outside Japan, the no-controller warning.
 */
void Diorama_RenderFrame(void) {
    BgStage_DrawFrame();
    GfxImage_FillCurrent(&gDisplayListHead, 1);
    GeoRender_AdvanceFrameCounter();
    Diorama_DrawSkyGradient();
    Diorama_DrawTitleBackground(sDioramaTitleBackground);
    Geo_RenderRootNode(sDioramaRootNode);
    Diorama_DrawSkipFadeOverlay();
#if !defined(VERSION_JP)
    // The Japanese build has no Diorama_DrawNoControllerWarning.
    //@src/fragments/demo_scenes/demo_scenes_161590.c
    Diorama_DrawNoControllerWarning();
#endif
    BgStage_AdvanceFrame();
}

/**
 * Main loop of the boot diorama; runs until Diorama_UpdateStateMachine returns nonzero.
 */
void Diorama_RunLoop(void) {
    s32 var_s1 = 1;
    s32 var_s0 = 0;

    while (var_s1 != 0) {
        Diorama_ReadControllerInput();
        Particle_UpdateFrameCounters();

        if (Diorama_UpdateStateMachine() != 0) {
            var_s1 = 0;
        }

        var_s0++;
        if (var_s0 == 0x50) {
            var_s0 = 0;
        }

        Diorama_LoopIdleAnimations();
        Diorama_RenderFrame();
    }
}

/**
 * Draws one more frame after the loop ends.
 */
void Diorama_RenderFinalFrame(void) {
    Diorama_RenderFrame();
}

/**
 * Builds the diorama scene graph, opens the Pokémon model archives and resets the scene state.
 */
void Diorama_InitScene(void) {
    MemoryBlock* sp1C;

    sp1C = MainPool_AllocState(main_pool_get_available(), 0);
    sDioramaRootNode = process_geo_layout(sp1C, &sDioramaGeoLayout);

    MainPool_FinalizeAllocation(sp1C);
    ModelRenderer_InitDisplayRoots();
    PokeIcon_OpenModelArchives();
    Diorama_InitSceneState();

    sDioramaSkipFadeAlpha = 0;
    sDioramaSequenceFinished = 0;
}

/**
 * Shows the opening diorama after the N64 logo: four groups of Pokémon (two ground groups, a
 * flying group over the clouds and a water group) followed by a finale, ending on the title
 * screen art. Returns 1 when it plays to the end and 2 when the player skips it.
 */
s32 Stage_ShowBootDiorama(void) {
    main_pool_push_state('MINI');

    Font_Init(6, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    FRAGMENT_LOAD(FRAGMENT_PARTICLE_DATA_LIBRARY);
    Fragment_Load((((u32)D_8D000000 & 0x0FF00000) >> 0x14) - 0x10, _5C7A70_ROM_START, _5C7A70_ROM_END);

    Save_EnsureBankLoaded(2);
    Save_GetModeSettings(&sDioramaClearedModes, 0);
    sDioramaBackgroundArchive = ASSET_LOAD2(backgrounds, 1, 1);

    // The Japanese build compares the mode settings with 0x1FE instead of 0x1F8 when choosing
    // background file 0x11, and loads no string table (no Text_* calls before Diorama_InitScene).
#if defined(VERSION_JP)
    if (sDioramaClearedModes == 0x1FE) {
#else
    if (sDioramaClearedModes == 0x1F8) {
#endif
        sDioramaTitleBackground = BinArchive_GetFile(sDioramaBackgroundArchive, 0x11);
    } else {
        sDioramaTitleBackground = BinArchive_GetFile(sDioramaBackgroundArchive, 0);
    }

#if !defined(VERSION_JP)
    Text_InitStringTables();
    sControllerErrorText = Text_GetStringTable(0x14);
#endif
    Diorama_InitScene();
    StageLoader_UpdateSegments();
    Diorama_RunLoop();
    Diorama_RenderFinalFrame();
    StageLoader_WaitForRetrace();
    Font_Free();

    main_pool_pop_state('MINI');

    return sDioramaExitCode;
}
