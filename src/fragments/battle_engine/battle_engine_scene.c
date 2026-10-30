#include "battle_engine.h"
#include "src/geo_node.h"
#include "src/model_animation.h"
#include "src/text_system.h"
#include "src/audio_commands.h"
#include "src/matrix.h"
#include "src/memory.h"
#include "src/util.h"
#include "status.h"

#define VISIBILITY_PHASE_HIDE_OPPONENT 21

BattleSceneContext sBattleScene;
static s32 pad_D_84390280;
static s32 sDefaultFramingSide;
s32 D_84390288;
static s32 pad_D_84390290[4];

void BattleScene_FrameDefaultSide(CameraNode* camera) {
    f32 camera_distance;

    BattleScene_SetCameraClipPlanes(camera, 20.0f, 12800.0f);
    BattleAnim_GetOwnerDefaultAnchorPosition(sActivePokemon[sDefaultFramingSide], &sBattleScene.scene->cameraAnchor);
    camera_distance = BattleScene_ScaleCameraDistance(sActivePokemon[sDefaultFramingSide], 3.5f, 300.0f);
    BattleScene_SetCameraAtFromAnchor(sActivePokemon[sDefaultFramingSide], camera, sBattleScene.scene->cameraAnchor, 0, BattleScene_GetParticipantFacingAngle(sActivePokemon[sDefaultFramingSide]),
                  0.0f, 0.0f);
    BattleScene_SetCameraEyeFromAt(camera, 0, BattleScene_GetParticipantFacingAngle(sActivePokemon[sDefaultFramingSide]), camera_distance, 0.0f);

    if (sActivePokemon[0]) {} // karodo
    sBattleScene.scene->phaseStep += 1;
    if (sDefaultFramingSide) {}
}

void BattleScene_FrameSideZero(CameraNode* camera) {
    f32 camera_distance;

    BattleScene_SetCameraClipPlanes(camera, 20.0f, 12800.0f);
    BattleAnim_GetOwnerDefaultAnchorPosition(sActivePokemon[0], &sBattleScene.scene->cameraAnchor);
    camera_distance = BattleScene_ScaleCameraDistance(sActivePokemon[0], 3.5f, 300.0f);
    BattleScene_SetCameraAtFromAnchor(sActivePokemon[0], camera, sBattleScene.scene->cameraAnchor, 0, BattleScene_GetParticipantFacingAngle(*sActivePokemon), 0.0f, 0.0f);
    BattleScene_SetCameraEyeFromAt(camera, 0, BattleScene_GetParticipantFacingAngle(*sActivePokemon), camera_distance, 0.0f);

    if (sActivePokemon[0]) {} // karodo
    sBattleScene.scene->phaseStep += 1;
}

void BattleScene_InitializeMode(CameraNode* camera, UNUSED ActivePokemon* first_pokemon, UNUSED ActivePokemon* second_pokemon) {
    sBattleScene.scene->winner = 0;
    sBattleScene.scene->verticalFieldOfView = 30.0f;
    sBattleScene.scene->unk_6C = 10.0f;

    switch (sBattleScene.scene->mode) {
        case BATTLE_SCENE_MODE_FRAME_DEFAULT_SIDE:
            BattleScene_FrameDefaultSide(camera);
            break;

        case BATTLE_SCENE_MODE_ACTION_SELECTION:
            BattleScene_SelectNextBRollCameraShot(camera);
            break;

        case BATTLE_SCENE_MODE_BATTLE:
            BattleScene_InitializeBattlePresentation(camera);
            BattleScene_SetPhase(camera, 1);
            break;

        case BATTLE_SCENE_MODE_FRAME_SIDE_ZERO:
            BattleScene_FrameSideZero(camera);
            break;

        case BATTLE_SCENE_MODE_INTRO:
            BattleScene_IntroSetupCamera(camera);
            break;
    }
    sBattleScene.scene->camera = camera;
}

void BattleScene_AllocateMemory(void) {
    sBattleScene.scene = main_pool_alloc(sizeof(BattleScene), 0);
    bzero(sBattleScene.scene, sizeof(BattleScene));
}

void BattleScene_UpdateCameraDefaultModeNoop(UNUSED CameraNode* camera) {
}

void BattleScene_UpdateCameraOwner0FramingModeNoop(UNUSED CameraNode* camera) {
}

void BattleScene_AvoidDegenerateCameraLookAt(CameraNode* camera) {
    if (BattleAnim_Distance2D(camera->view.targetPosition.x, camera->view.targetPosition.z, camera->view.eyePosition.x, camera->view.eyePosition.z) < 0.0001f) {
        camera->view.targetPosition.z += 0.001f;
    }
}

void BattleScene_TickCameraSubstateDefault(CameraNode* camera) {
    switch (sBattleScene.scene->transitionStep) {
        case CAMERA_TRANSITION_STEP_IDLE:
            sBattleScene.scene->transitionStage = 0;
            break;

        case CAMERA_TRANSITION_STEP_BEGIN:
            if (sBattleScene.scene->unk_44 == 2) {
                BattleThumbnail_ClearBothOwnerIdleFlags();
            }
            sBattleScene.scene->transitionStep++;
            break;

        case CAMERA_TRANSITION_STEP_WAIT_THUMBNAILS:
            if (sActivePokemon[0]->thumbnail.camera->viewport.x == -0x48) {
                sBattleScene.scene->transitionStep++;
            }
            break;

        case CAMERA_TRANSITION_STEP_COVER:
            GeoCamera_SetBackgroundTexture(camera, 0x12C, 0xA, 2, Util_ConvertAddrToVirtAddr((u32)D_1000000));
            GeoCamera_SetBackground(camera, 5, 0xA, 0, 0, 0);
            sBattleScene.scene->transitionStage = 0;
            sBattleScene.scene->transitionStep += 1;

        case CAMERA_TRANSITION_STEP_WAIT_COVER:
            if (camera->overlay.stepIndex == 0xA) {
                BattleScene_ResetModelReloadStep();
                BattleScene_ResetBothParticipantsAnimationState();
                BattleScene_CleanupParticipantEffectMode(sActivePokemon[0]);
                BattleScene_CleanupParticipantEffectMode(sActivePokemon[1]);
                sBattleScene.scene->transitionStep += 1;
                sBattleScene.scene->transitionStage = 1;
            }
            break;

        case CAMERA_TRANSITION_STEP_RESTORE_MODELS:
            if ((BattleScene_RestoreBothSubstitutes() == 0) && (BattleScene_RevealSubstituteForAction(sActivePokemon[sBattleScene.scene->sideCurrentlySwitching]) == 0)) {
                if (sBattleScene.scene->unk_44 == 1) {
                    BattleAnim_CleanupEffects(2);
                    BattleScene_QuantizeBattleTimer();
                }
                Battle_StopBeingGoldAfterHarden(sActivePokemon[0]);
                Battle_StopBeingGoldAfterHarden(sActivePokemon[1]);
                sBattleScene.scene->transitionStage = 2;
                GeoCamera_SetBackgroundTexture(camera, 0xA, 0x12C, 2, Util_ConvertAddrToVirtAddr((u32)D_1000000));
                GeoCamera_SetBackground(camera, 4, 0xA, 0, 0, 0);
                camera->perspective.verticalFieldOfView = 30.0f;
                sBattleScene.scene->verticalFieldOfView = 30.0f;
                sBattleScene.scene->transitionStep += 1;
            }
            break;

        case CAMERA_TRANSITION_STEP_UNCOVER:
            BattleScene_ResetModelReloadStep();
            camera->perspective.verticalFieldOfView = 30.0f;
            sBattleScene.scene->verticalFieldOfView = 30.0f;
            sBattleScene.scene->transitionStage = 0;
            if (camera->overlay.stepIndex == 0xA) {
                sBattleScene.scene->transitionStep = CAMERA_TRANSITION_STEP_IDLE;
                if (sBattleScene.scene->unk_44 == 1) {
                    BattleThumbnail_SetBothOwnerIdleFlags();
                }
            }
            break;
    }
}

void BattleScene_TickCameraSubstateStripWipe(CameraNode* camera) {
    switch (sBattleScene.scene->transitionStep) {
        case STRIP_TRANSITION_STEP_IDLE:
            sBattleScene.scene->transitionStage = 0;
            break;

        case STRIP_TRANSITION_STEP_BEGIN:
            if (sBattleScene.scene->unk_44 == 2) {
                BattleThumbnail_ClearBothOwnerIdleFlags();
            }
            sBattleScene.scene->transitionStep++;
            break;

        case STRIP_TRANSITION_STEP_WAIT_THUMBNAILS:
            if (sActivePokemon[0]->thumbnail.camera->viewport.x == -0x48) {
                sBattleScene.scene->transitionStep++;
            }
            break;

        case STRIP_TRANSITION_STEP_COVER:
            BattleScene_SetStripWipeState(3, 0x1E);
            BattleScene_UpdateStripWipe();
            sBattleScene.scene->transitionStage = 0;
            sBattleScene.scene->transitionStep += 1;

        case STRIP_TRANSITION_STEP_WAIT_COVER:
            BattleScene_UpdateStripWipe();
            if (BattleScene_GetStripWipeState() == 1) {
                BattleScene_ResetModelReloadStep();
                BattleScene_ResetBothParticipantsAnimationState();
                BattleScene_CleanupParticipantEffectMode(sActivePokemon[0]);
                BattleScene_CleanupParticipantEffectMode(sActivePokemon[1]);
                sBattleScene.scene->transitionStep += 1;
                sBattleScene.scene->transitionStage = 1;
            }
            break;

        case STRIP_TRANSITION_STEP_HOLD_FIRST:
            sBattleScene.scene->transitionStage = 1;
            sBattleScene.scene->transitionStep += 1;
            break;

        case STRIP_TRANSITION_STEP_HOLD_SECOND:
            sBattleScene.scene->transitionStage = 1;
            sBattleScene.scene->transitionStep += 1;
            break;

        case STRIP_TRANSITION_STEP_HOLD_THIRD:
            sBattleScene.scene->transitionStage = 1;
            sBattleScene.scene->transitionStep += 1;
            break;

        case STRIP_TRANSITION_STEP_RESTORE_MODELS:
            if ((BattleScene_RestoreBothSubstitutes() == 0) && (BattleScene_RevealSubstituteForAction(sActivePokemon[sBattleScene.scene->sideCurrentlySwitching]) == 0)) {
                if (sBattleScene.scene->unk_44 == 1) {
                    BattleAnim_CleanupEffects(2);
                    BattleScene_QuantizeBattleTimer();
                }
                Battle_StopBeingGoldAfterHarden(sActivePokemon[0]);
                Battle_StopBeingGoldAfterHarden(sActivePokemon[1]);
                sBattleScene.scene->transitionStage = 2;
                BattleScene_SetStripWipeState(2, 0x1E);
                BattleScene_UpdateStripWipe();
                camera->perspective.verticalFieldOfView = 30.0f;
                sBattleScene.scene->verticalFieldOfView = 30.0f;
                sBattleScene.scene->transitionStep += 1;
            }
            break;

        case STRIP_TRANSITION_STEP_UNCOVER:
            BattleScene_ResetModelReloadStep();
            camera->perspective.verticalFieldOfView = 30.0f;
            sBattleScene.scene->verticalFieldOfView = 30.0f;
            sBattleScene.scene->transitionStage = 0;
            BattleScene_UpdateStripWipe();
            if (BattleScene_GetStripWipeState() == 0) {
                sBattleScene.scene->transitionStep = STRIP_TRANSITION_STEP_IDLE;
                if (sBattleScene.scene->unk_44 == 1) {
                    BattleThumbnail_SetBothOwnerIdleFlags();
                }
            }
            break;
    }
}

void BattleScene_TickCameraSubstate(CameraNode* camera) {
    if (sBattleScene.scene->transitionStyle == 0) {
        BattleScene_TickCameraSubstateDefault(camera);
    } else {
        BattleScene_TickCameraSubstateStripWipe(camera);
    }
}

u16 BattleScene_IsOwnerInViewWedge(ActivePokemon* owner, CameraNode* camera, ActivePokemon* tested_pokemon, s16 half_angle, f32 wedge_length) {
    UNUSED s32 pad[6];
    f32 far_edge_cross;
    f32 right_edge_cross;
    f32 left_edge_cross;
    f32 left_x;
    f32 right_x;
    UNUSED s32 pad2;
    f32 left_z;
    f32 right_z;
    UNUSED s32 pad3;
    f32 view_distance;
    s16 view_rotation_x;
    s16 view_rotation_y;
    f32 wedge_base_z;
    ActivePokemon* pokemon;
    f32 wedge_base_x;
    s32 inside_wedge = 0;

    BattleAnim_CalculateDistanceAngles(&camera->view.targetPosition, &camera->view.eyePosition, &view_distance, &view_rotation_x, &view_rotation_y);
    wedge_base_z = (COSS((view_rotation_y - 0x8000)) * 50.0f) + camera->view.eyePosition.z;
    wedge_base_x = (SINS((view_rotation_y - 0x8000)) * 30.0f) + camera->view.eyePosition.x;
    left_z = (COSS(((view_rotation_y - half_angle) + 0x8000)) * wedge_length) + wedge_base_z;
    left_x = (SINS(((view_rotation_y - half_angle) + 0x8000)) * wedge_length) + wedge_base_x;
    right_z = (COSS((view_rotation_y + half_angle + 0x8000)) * wedge_length) + wedge_base_z;
    right_x = (SINS((view_rotation_y + half_angle + 0x8000)) * wedge_length) + wedge_base_x;

    pokemon = tested_pokemon;

    far_edge_cross = ((left_z - pokemon->displayObject.position.z) * (right_x - pokemon->displayObject.position.x)) -
           ((right_z - pokemon->displayObject.position.z) * (left_x - pokemon->displayObject.position.x));
    right_edge_cross = ((right_z - pokemon->displayObject.position.z) * (wedge_base_x - pokemon->displayObject.position.x)) -
           ((wedge_base_z - pokemon->displayObject.position.z) * (right_x - pokemon->displayObject.position.x));
    left_edge_cross = ((wedge_base_z - pokemon->displayObject.position.z) * (left_x - pokemon->displayObject.position.x)) -
           ((left_z - pokemon->displayObject.position.z) * (wedge_base_x - pokemon->displayObject.position.x));

    if (((far_edge_cross >= 0.0f) && (right_edge_cross >= 0.0f) && (left_edge_cross >= 0.0f)) || ((far_edge_cross <= 0.0f) && (right_edge_cross <= 0.0f) && (left_edge_cross <= 0.0f))) {
        inside_wedge = 1;
    }
    return inside_wedge;
}

u16 BattleScene_IsOwnerInViewWedgeAlt(ActivePokemon* owner, CameraNode* camera, ActivePokemon* tested_pokemon, s16 half_angle, f32 wedge_length) {
    UNUSED s32 pad[6];
    f32 far_edge_cross;
    f32 right_edge_cross;
    f32 left_edge_cross;
    f32 left_x;
    f32 right_x;
    UNUSED s32 pad2;
    f32 left_z;
    f32 right_z;
    ActivePokemon* pokemon;
    f32 view_distance;
    s16 view_rotation_x;
    s16 view_rotation_y;
    Vec3f base_position;
    s32 inside_wedge;

    inside_wedge = 0;
    BattleAnim_CalculateDistanceAngles(&camera->view.targetPosition, &camera->view.eyePosition, &view_distance, &view_rotation_x, &view_rotation_y);
    base_position.z = (COSS((view_rotation_y - 0x8000)) * 30.0f) + camera->view.eyePosition.z;
    base_position.x = (SINS((view_rotation_y - 0x8000)) * 30.0f) + camera->view.eyePosition.x;
    left_z = (COSS(((view_rotation_y - half_angle) + 0x8000)) * wedge_length) + base_position.z;
    left_x = (SINS(((view_rotation_y - half_angle) + 0x8000)) * wedge_length) + base_position.x;
    right_z = (COSS((view_rotation_y + half_angle + 0x8000)) * wedge_length) + base_position.z;
    right_x = (SINS((view_rotation_y + half_angle + 0x8000)) * wedge_length) + base_position.x;

    pokemon = tested_pokemon;

    far_edge_cross = ((left_z - pokemon->displayObject.position.z) * (right_x - pokemon->displayObject.position.x)) -
           ((right_z - pokemon->displayObject.position.z) * (left_x - pokemon->displayObject.position.x));
    right_edge_cross = ((right_z - pokemon->displayObject.position.z) * (base_position.x - pokemon->displayObject.position.x)) -
           ((base_position.z - pokemon->displayObject.position.z) * (right_x - pokemon->displayObject.position.x));
    left_edge_cross = ((base_position.z - pokemon->displayObject.position.z) * (left_x - pokemon->displayObject.position.x)) -
           ((left_z - pokemon->displayObject.position.z) * (base_position.x - pokemon->displayObject.position.x));

    if (((far_edge_cross >= 0.0f) && (right_edge_cross >= 0.0f) && (left_edge_cross >= 0.0f)) || ((far_edge_cross <= 0.0f) && (right_edge_cross <= 0.0f) && (left_edge_cross <= 0.0f))) {
        inside_wedge = 1;
    }

    base_position.x = tested_pokemon->displayObject.position.x;
    base_position.y = tested_pokemon->displayObject.position.y;
    base_position.z = tested_pokemon->displayObject.position.z;

    if ((BattleScene_GetParticipantModelRadiusScaled(owner) * 2.5f) >= BattleAnim_Vec3fDistance(camera->view.eyePosition, base_position)) {
        inside_wedge = 0;
    }
    return inside_wedge;
}

void BattleScene_UpdateBothOwnersVisibilityCulling(CameraNode* camera) {
    if (BattleScene_IsOwnerInViewWedge(sActivePokemon[0], camera, sActivePokemon[0], 0x2AA8, 2500.0f) != 0) {
        sActivePokemon[0]->displayObject.node.flags |= 1;
    } else {
        sActivePokemon[0]->displayObject.node.flags &= ~1;
    }

    if (BattleScene_IsOwnerInViewWedge(sActivePokemon[1], camera, sActivePokemon[1], 0x2AA8, 2500.0f) != 0) {
        sActivePokemon[1]->displayObject.node.flags |= 1;
    } else {
        sActivePokemon[1]->displayObject.node.flags &= ~1;
    }
}

void BattleScene_UpdateOpponentVisibilityCulling(CameraNode* camera) {
    ActivePokemon* opponent = sActivePokemon[!sBattleScene.scene->actingSide];

    if (BattleScene_IsOwnerInViewWedgeAlt(opponent, camera, opponent, 0x2AA8, 2500.0f) != 0) {
        opponent->displayObject.node.flags |= 1;
    } else {
        opponent->displayObject.node.flags &= 0xFFFE;
    }
}

void func_8432A414(void) {
}

void BattleScene_ShowBothOwners(void) {
    sActivePokemon[0]->displayObject.node.flags |= 1;
    sActivePokemon[1]->displayObject.node.flags |= 1;
}

void BattleScene_HideOpponentOwner(void) {
    Battle_ResetModelToIdleAnim(sActivePokemon[!sBattleScene.scene->actingSide]);
    sActivePokemon[!sBattleScene.scene->actingSide]->displayObject.node.flags &= ~1;
}

void BattleScene_HideActiveOwner(void) {
    sActivePokemon[sBattleScene.scene->actingSide]->displayObject.node.flags &= ~1;
    Battle_ResetModelToIdleAnim(sActivePokemon[sBattleScene.scene->actingSide]);
}

void BattleScene_HideAbsentParticipants(void) {
    ActivePokemon* first_pokemon = sActivePokemon[0];
    ActivePokemon* second_pokemon = sActivePokemon[1];

    if ((first_pokemon->state.replacementState == REPLACEMENT_INVOLUNTARY_SWITCH) || ((first_pokemon = sActivePokemon[0])->state.replacementState == REPLACEMENT_DEFEAT_COMPLETE)) {
        first_pokemon->displayObject.node.flags &= ~1;
    }

    if ((second_pokemon->state.replacementState == REPLACEMENT_INVOLUNTARY_SWITCH) || (second_pokemon->state.replacementState == REPLACEMENT_DEFEAT_COMPLETE)) {
        second_pokemon->displayObject.node.flags &= ~1;
    }
}

void BattleScene_UpdateVisibilityBeforeMode(CameraNode* camera) {
    UNUSED s32 pad;
    ActivePokemonState* acting_state = &sActingPokemon->state;
    s32 rotation_y_degrees;
    ActivePokemonState* opponent_state = &sNonActingPokemon->state;
    f32 view_distance;
    s16 view_rotation_x;
    s16 view_rotation_y;

    switch (sBattleScene.scene->mode) {
        case BATTLE_SCENE_MODE_FRAME_DEFAULT_SIDE:
        case BATTLE_SCENE_MODE_FRAME_SIDE_ZERO:
            break;

        case BATTLE_SCENE_MODE_ACTION_SELECTION:
            BattleScene_ShowBothOwners();

            switch (sBattleScene.scene->phaseStep) {
                case BROLL_STEP_SETUP_SEND_OUT_SIDE_ZERO:
                case BROLL_STEP_UPDATE_SEND_OUT_SIDE_ZERO:
                case BROLL_STEP_SETUP_SEND_OUT_SIDE_ONE:
                case BROLL_STEP_UPDATE_SEND_OUT_SIDE_ONE:
                case BROLL_STEP_UPDATE_STATIC_OVERVIEW:
                    BattleScene_ShowBothOwners();
                    break;

                case BROLL_STEP_SETUP_FIRST_HEIGHT_SHOT:
                case BROLL_STEP_UPDATE_FIRST_HEIGHT_SHOT:
                case BROLL_STEP_SETUP_SECOND_HEIGHT_SHOT:
                case BROLL_STEP_UPDATE_SECOND_HEIGHT_SHOT:
                    sActivePokemon[sBattleScene.scene->actingSide]->displayObject.node.flags &= ~1;
                    break;

                default:
                    BattleScene_UpdateOpponentVisibilityCulling(camera);
                    break;
            }
            break;

        case BATTLE_SCENE_MODE_BATTLE:
            BattleScene_ShowBothOwners();

            switch (sBattleScene.scene->phase) {
                case BATTLE_PHASE_FIRST_ACTION:
                case BATTLE_PHASE_SECOND_ACTION:
                    switch (sBattleScene.scene->phaseStep) {
                        case ACTION_STEP_SETUP:
                            if ((acting_state->replacementState == REPLACEMENT_VOLUNTARY_SWITCH) && (sBattleScene.scene->actionPass == BATTLE_ACTION_SECOND)) {
                                BattleScene_HideActiveOwner();
                            } else {
                                Vec3f_CalculateDistanceAngles(&sBattleScene.scene->savedCameraTarget, &sBattleScene.scene->savedCameraEye, &view_distance, &view_rotation_x,
                                              &view_rotation_y);
                                rotation_y_degrees = view_rotation_y / 182;
                                if (((rotation_y_degrees == 0x5A) || (rotation_y_degrees == -0x5A)) && (sBattleScene.scene->actionPass == BATTLE_ACTION_SECOND)) {
                                    BattleScene_HideOpponentOwner();
                                } else {
                                    BattleScene_UpdateOpponentVisibilityCulling(camera);
                                }
                            }
                            break;

                        case ACTION_STEP_ANIMATE:
                            sBattleScene.scene->visibilityPolicy = BATTLE_VISIBILITY_DEFAULT;
                            if (!(acting_state->flags & 0x4400)) {
                                BattleScene_HideOpponentOwner();
                            }
                            break;

                        case ACTION_STEP_FINALIZE:
                            if ((sBattleScene.scene->phaseSubstep == 0) && (sBattleScene.scene->modelReloadStep >= 0) &&
                                (sBattleScene.scene->modelReloadStep < 2) && (opponent_state->flags & 0x800)) {
                                BattleScene_HideOpponentOwner();
                            } else if (((sBattleScene.scene->phaseSubstep == 0) || (sBattleScene.scene->phaseSubstep == 1)) &&
                                       (sBattleScene.scene->moveEffectKind == MOVE_PRESENTATION_NORMAL) && (sBattleScene.scene->hitReactionKind != BATTLE_HIT_REACTION_MULTI_HIT)) {
                                Battle_ResetModelToIdleAnim(sActingPokemon);
                                sActingPokemon->displayObject.lodFraction = 0;
                            } else {
                                if (acting_state->flags & 0x4000) {
                                    if ((sActingPokemon->displayObject.objectType != 0x32) &&
                                        (sActingPokemon->displayObject.objectType != 0x33)) {
                                        sActingPokemon->displayObject.lodFraction = 0;
                                    } else {
                                        sActingPokemon->displayObject.lodFraction = 0xFF;
                                    }
                                } else {
                                    sActingPokemon->displayObject.lodFraction = 0xFF;
                                }
                                BattleScene_HideActiveOwner();
                            }
                            sBattleScene.scene->visibilityPolicy = BATTLE_VISIBILITY_AFTER_ACTION;
                            break;
                    }
                    break;

                case BATTLE_PHASE_ENTER_QUEUED:
                    if (sBattleScene.scene->visibilityPolicy == BATTLE_VISIBILITY_AFTER_ACTION) {
                        BattleScene_HideOpponentOwner();
                    } else if (sBattleScene.scene->visibilityPolicy == BATTLE_VISIBILITY_HIDE_OPPONENT) {
                        sActivePokemon[!sBattleScene.scene->actingSide]->displayObject.node.flags &= ~1;
                    } else if (sBattleScene.scene->visibilityPolicy == BATTLE_VISIBILITY_QUEUED_ACTION) {
                        sActivePokemon[sBattleScene.scene->actingSide]->displayObject.node.flags &= ~1;
                    } else if (sBattleScene.scene->visibilityPolicy == BATTLE_VISIBILITY_SWITCHING) {
                        sActivePokemon[sBattleScene.scene->actingSide]->displayObject.node.flags &= ~1;
                    }
                    break;

                case BATTLE_PHASE_END_TURN:
                    if (sBattleScene.scene->visibilityPolicy == BATTLE_VISIBILITY_AFTER_ACTION) {
                        BattleScene_HideActiveOwner();
                    } else if (sBattleScene.scene->visibilityPolicy == BATTLE_VISIBILITY_HIDE_OPPONENT) {
                        sActivePokemon[!sBattleScene.scene->actingSide]->displayObject.node.flags &= ~1;
                    } else if (sBattleScene.scene->visibilityPolicy == BATTLE_VISIBILITY_QUEUED_ACTION) {
                        sActivePokemon[!sBattleScene.scene->actingSide]->displayObject.node.flags &= ~1;
                    } else if (sBattleScene.scene->visibilityPolicy == BATTLE_VISIBILITY_SWITCHING) {
                        sActivePokemon[!sBattleScene.scene->actingSide]->displayObject.node.flags &= ~1;
                    }
                    break;

                case BATTLE_PHASE_VOLUNTARY_SWITCH:
                    sBattleScene.scene->visibilityPolicy = BATTLE_VISIBILITY_SWITCHING;
                    if (sBattleScene.scene->actionPass == BATTLE_ACTION_SECOND) {
                        BattleScene_HideOpponentOwner();
                    } else if (sBattleScene.scene->phaseStep == 0) {
                        BattleScene_UpdateBothOwnersVisibilityCulling(camera);
                    } else {
                        BattleScene_HideOpponentOwner();
                    }
                    break;

                case BATTLE_PHASE_SEND_OUT_AFTER_FAINT:
                    sBattleScene.scene->visibilityPolicy = BATTLE_VISIBILITY_QUEUED_ACTION;
                    switch (sBattleScene.scene->phaseStep) {
                        case SEND_OUT_STEP_SETUP:
                            BattleScene_UpdateBothOwnersVisibilityCulling(camera);
                            break;

                        default:
                            BattleScene_HideOpponentOwner();
                            break;
                    }
                    break;

                case BATTLE_PHASE_FLY_START:
                    sBattleScene.scene->visibilityPolicy = BATTLE_VISIBILITY_DEFAULT;
                    switch (sBattleScene.scene->phaseStep) {
                        case FLY_STEP_WAIT_MAX_HEIGHT:
                        case FLY_STEP_RESOLVE_HP:
                        case FLY_STEP_POST_SEQUENCE_VISIBILITY:
                            BattleScene_ShowBothOwners();
                            break;

                        default:
                            BattleScene_HideOpponentOwner();
                            break;
                    }
                    break;

                case BATTLE_PHASE_DISPATCH_MOVE_EFFECT:
                    sBattleScene.scene->visibilityPolicy = BATTLE_VISIBILITY_DEFAULT;
                    BattleScene_HideOpponentOwner();
                    break;

                case BATTLE_PHASE_SELF_TARGET:
                case BATTLE_PHASE_DIG_START:
                case BATTLE_PHASE_CHARGE:
                case BATTLE_PHASE_TRANSFORM:
                case BATTLE_PHASE_JUMP_KICK_CRASH:
                case BATTLE_PHASE_UNABLE_TO_ACT:
                case BATTLE_PHASE_CONFUSION_SELF_HIT:
                case VISIBILITY_PHASE_HIDE_OPPONENT:
                case BATTLE_PHASE_SUBSTITUTE:
                case BATTLE_PHASE_BIDE:
                case BATTLE_PHASE_NO_ACTION:
                case BATTLE_PHASE_HAZE:
                case BATTLE_PHASE_FULL_PARALYSIS:
                case BATTLE_PHASE_WITHDRAW:
                    sBattleScene.scene->visibilityPolicy = BATTLE_VISIBILITY_QUEUED_ACTION;
                    BattleScene_HideOpponentOwner();
                    break;

                case BATTLE_PHASE_RECOIL:
                case BATTLE_PHASE_RESIDUAL_WITHOUT_MOVE:
                    sBattleScene.scene->visibilityPolicy = BATTLE_VISIBILITY_QUEUED_ACTION;
                    BattleScene_HideOpponentOwner();
                    break;

                case BATTLE_PHASE_RESIDUAL_AFTER_MOVE:
                    sBattleScene.scene->visibilityPolicy = BATTLE_VISIBILITY_DEFAULT;
                    BattleScene_HideOpponentOwner();
                    break;

                case BATTLE_PHASE_MOVE_FAILED:
                case BATTLE_PHASE_RAGE:
                    sBattleScene.scene->visibilityPolicy = BATTLE_VISIBILITY_DEFAULT;
                    BattleScene_HideActiveOwner();
                    break;

                case BATTLE_PHASE_EXPLOSION:
                    sBattleScene.scene->visibilityPolicy = BATTLE_VISIBILITY_DEFAULT;

                    switch (sBattleScene.scene->phaseStep) {
                        case EXPLOSION_STEP_SETUP:
                        case EXPLOSION_STEP_WAIT_ACTION:
                        case EXPLOSION_STEP_RESTORE_TARGET_SUBSTITUTE:
                            BattleScene_HideOpponentOwner();
                            break;

                        default:
                            BattleScene_HideActiveOwner();
                            break;
                    }
                    break;

                case BATTLE_PHASE_FAINT:
                case BATTLE_PHASE_RESIDUAL_FAINT:
                    sBattleScene.scene->visibilityPolicy = BATTLE_VISIBILITY_DEFAULT;
                    switch (sBattleScene.scene->phaseStep) {
                        case FAINT_STEP_BEGIN:
                        case FAINT_STEP_ANIMATE:
                            sActivePokemon[sBattleScene.scene->actingSide]->displayObject.node.flags &= ~1;
                            break;
                    }
                    break;
            }
            break;
    }
    BattleScene_HideAbsentParticipants();
}

void BattleScene_UpdateVisibilityAfterMode(CameraNode* camera) {
    switch (sBattleScene.scene->mode) {
        case BATTLE_SCENE_MODE_ACTION_SELECTION:
            BattleScene_ShowBothOwners();

            switch (sBattleScene.scene->phaseStep) {
                case BROLL_STEP_SETUP_SEND_OUT_SIDE_ZERO:
                case BROLL_STEP_UPDATE_SEND_OUT_SIDE_ZERO:
                case BROLL_STEP_SETUP_SEND_OUT_SIDE_ONE:
                case BROLL_STEP_UPDATE_SEND_OUT_SIDE_ONE:
                case BROLL_STEP_UPDATE_STATIC_OVERVIEW:
                    BattleScene_ShowBothOwners();
                    break;

                case BROLL_STEP_SETUP_FIRST_HEIGHT_SHOT:
                case BROLL_STEP_UPDATE_FIRST_HEIGHT_SHOT:
                case BROLL_STEP_SETUP_SECOND_HEIGHT_SHOT:
                case BROLL_STEP_UPDATE_SECOND_HEIGHT_SHOT:
                    sActivePokemon[sBattleScene.scene->actingSide]->displayObject.node.flags &= ~1;
                    break;

                default:
                    BattleScene_UpdateOpponentVisibilityCulling(camera);
                    break;
            }
            break;

        case BATTLE_SCENE_MODE_BATTLE:
            switch (sBattleScene.scene->phase) {
                case BATTLE_PHASE_EXPLOSION:
                    switch (sBattleScene.scene->phaseStep) {
                        case EXPLOSION_STEP_SETUP:
                        case EXPLOSION_STEP_WAIT_ACTION:
                        case EXPLOSION_STEP_RESTORE_TARGET_SUBSTITUTE:
                        case EXPLOSION_STEP_RESOLVE_HP:
                            sActivePokemon[!sBattleScene.scene->actingSide]->displayObject.node.flags &= ~1;
                            sActivePokemon[sBattleScene.scene->actingSide]->displayObject.node.flags |= 1;
                            break;

                        default:
                            sActivePokemon[sBattleScene.scene->actingSide]->displayObject.node.flags &= ~1;
                            sActivePokemon[!sBattleScene.scene->actingSide]->displayObject.node.flags |= 1;
                    }
                    break;

                case BATTLE_PHASE_FAINT:
                case BATTLE_PHASE_RESIDUAL_FAINT:
                    switch (sBattleScene.scene->phaseStep) {
                        case FAINT_STEP_BEGIN:
                        case FAINT_STEP_ANIMATE:
                            sActivePokemon[sBattleScene.scene->actingSide]->displayObject.node.flags &= ~1;
                            sActivePokemon[!sBattleScene.scene->actingSide]->displayObject.node.flags |= 1;
                            break;
                    }
                    break;
            }
            break;
    }
    BattleScene_HideAbsentParticipants();
}

void BattleScene_Update(CameraNode* camera) {
    BattleScene_TickCameraSubstate(camera);
    BattleScene_ApplyCameraPhaseDefaults();
    BattleScene_UpdateVisibilityBeforeMode(camera);

    switch (sBattleScene.scene->mode) {
        case BATTLE_SCENE_MODE_FRAME_DEFAULT_SIDE:
            BattleScene_UpdateCameraDefaultModeNoop(camera);
            break;

        case BATTLE_SCENE_MODE_ACTION_SELECTION:
            BattleScene_UpdateBRollCameraSequence(camera);
            break;

        case BATTLE_SCENE_MODE_BATTLE:
            BattleScene_TickBattleTimer();
            BattleScene_UpdatePhase(camera);
            break;

        case BATTLE_SCENE_MODE_FRAME_SIDE_ZERO:
            BattleScene_UpdateCameraOwner0FramingModeNoop(camera);
            break;

        case BATTLE_SCENE_MODE_INTRO:
            BattleScene_UpdateIntroSequence(camera);
            break;

        default:
            sBattleScene.scene->winner = 3;
            break;
    }

    BattleScene_UpdateVisibilityAfterMode(camera);
    BattleScene_TickStateCallbacks();
    BattleAnim_TickParticipantStatusAnimation(sSide1ActivePokemon);
    BattleAnim_TickParticipantStatusAnimation(sSide2ActivePokemon);
    BattleScene_ApplyCameraShake(camera);
    BattleScene_ClampCameraEyeFloor(camera);
    BattleScene_CameraHelperNop(camera);
    BattleScene_AvoidDegenerateCameraLookAt(camera);
}

s32 BattleScene_Dispatch(s32 operation, CameraNode* camera) {
    switch (operation) {
        case BATTLE_SCENE_DISPATCH_ALLOCATE:
            BattleScene_AllocateMemory();
            break;

        case BATTLE_SCENE_DISPATCH_UPDATE:
        case BATTLE_SCENE_DISPATCH_UPDATE_ALTERNATE:
            BattleScene_Update(camera);
            break;

        case BATTLE_SCENE_DISPATCH_INITIALIZE_MODE:
            BattleScene_InitializeMode(camera, sActivePokemon[0], sActivePokemon[1]);
            break;
    }
    return sBattleScene.scene->winner;
}
