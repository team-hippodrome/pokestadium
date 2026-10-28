#include "battle_engine.h"
#include "moves.h"
#include "src/geo_render.h"
#include "src/matrix.h"
#include "pokemon.h"
#include "status.h"

unk_D_843901B0 gCameraPreset;
f32 gCameraDistTarget;
f32 sDistanceBetweenCameraEyeAndAnchor;

static s32 D_84385B50 = 0;

f32 BattleScene_ScaleCameraDistance(ActivePokemon* pokemon, f32 scale, f32 max_allowed) {
    f32 temp_fv1 = BattleScene_GetParticipantModelRadiusScaled(pokemon) * scale;

    if (max_allowed < temp_fv1) {
        return max_allowed;
    }
    return temp_fv1;
}

s32 BattleScene_AdvanceCameraSubstepCounter(s32 arg0) {
    sBattleScene.scene->substepFrameCounter++;
    if (sBattleScene.scene->substepFrameCounter >= arg0) {
        sBattleScene.scene->substepFrameCounter = 0;
        return 1;
    }
    return 0;
}

s32 BattleScene_AdvanceCameraFrameCounter(s32 frame_limit) {
    sBattleScene.scene->stepFrameTimer++;
    if (sBattleScene.scene->stepFrameTimer >= frame_limit) {
        sBattleScene.scene->stepFrameTimer = 0;
        return 1;
    }
    return 0;
}

s32 BattleScene_AdvanceCameraFrameCounterUntil(s32 arg0) {
    if (sBattleScene.scene->stepFrameTimer >= arg0) {
        return 1;
    }
    sBattleScene.scene->stepFrameTimer++;
    return 0;
}

s32 BattleScene_IsCameraFrameCounterAtLeast(s32 arg0) {
    if (sBattleScene.scene->stepFrameTimer >= arg0) {
        return 1;
    }
    return 0;
}

void BattleScene_SetActivePokemonIndex(s32 arg0) {
    sBattleScene.scene->actingSide = arg0;
}

void BattleScene_ResetCameraFrameCounter(void) {
    sBattleScene.scene->stepFrameTimer = 0;
}

void BattleScene_ResetCameraSubstepCounter(void) {
    sBattleScene.scene->substepFrameCounter = 0;
}

void BattleScene_SetCameraAtAndEyeFromAngles(CameraNode* camera, f32 x, f32 y, f32 z, f32 distance, s16 rotation_x, s16 rotation_y) {
    Vec3f_SetComponentsDuplicate(&camera->view.targetPosition, x, y, z);
    Camera_ComputeEyeFromAngles(&camera->view.targetPosition, &camera->view.eyePosition, distance, rotation_x, rotation_y);
}

void BattleScene_GetOwnerCameraAnchor(ActivePokemon* pokemon, Vec3f* dest) {
    UNUSED s32 pad;
    u8 sp18[] = {
        MAGNETON, KANGASKHAN, MUK, ZAPDOS, ONIX, GYARADOS, GRIMER, EXEGGCUTE, CATERPIE,
    };

    if (BattleScene_ByteArrayContains(pokemon->displayObject.objectType, sp18, 9) != 0) {
        BattleScene_GetOwnerBoneOrDefaultAnchor(pokemon, dest, 9);
    } else {
        BattleScene_GetOwnerBoneOrDefaultAnchor(pokemon, dest, 100);
    }
}

void BattleScene_GetOwnerBoneOrDefaultAnchor(ActivePokemon* pokemon, Vec3f* dest, s32 lookup_id) {
    if (GeoRender_FindAnchorPosition(&pokemon->displayObject, lookup_id, dest) == NULL) {
        BattleAnim_GetOwnerDefaultAnchorPosition(pokemon, dest);
        dest->x = BattleScene_GetParticipantModelXOffset(pokemon);
        dest->z = 0.0f;
    }
}

void BattleScene_SetCameraModeAndStep(s32 camera_mode, s32 camera_step) {
    sBattleScene.scene->mode = camera_mode;
    sBattleScene.scene->phaseStep = camera_step;
}

void BattleScene_SetCameraClipPlanes(CameraNode* camera_node, f32 near_plane, f32 far_plane) {
    camera_node->perspective.nearPlane = near_plane;
    camera_node->perspective.farPlane = far_plane;
}

void BattleScene_SetCameraSubstate(s32 arg0, u16 arg1) {
    sBattleScene.scene->unk_30 = arg0;
    sBattleScene.scene->unk_2E = arg1;
}

void BattleScene_ApplyCameraPhaseDefaults(void) {
    if (sBattleScene.scene->unk_18 == 2) {
        if (sBattleScene.scene->unk_44 == 1) {
            BattleScene_SetCameraModeAndStep(1, 0x32);
        }

        if (sBattleScene.scene->unk_44 == 2) {
            sBattleScene.scene->actingSide = sBattleScene.scene->sideCurrentlySwitching;
            BattleScene_SetCameraModeAndStep(2, 0);
        }
    }
}

void BattleScene_SetDamageShakeAmplitude(ActivePokemon* arg0) {
    BattleMon* ptr = &arg0->teamRoster->pokemonPicked[arg0->state.rosterIndex];

    sBattleScene.scene->unk_68 = ((ptr->hp - arg0->state.runtime.displayHp) / 996.0f) * 1400.0f;

    if (sBattleScene.scene->unk_68 == 0.0f) {
        sBattleScene.scene->unk_68 = 0.0f;
    } else if (sBattleScene.scene->unk_68 <= 20.0f) {
        sBattleScene.scene->unk_68 = 20.0f;
    } else if (sBattleScene.scene->unk_68 >= 50.0f) {
        sBattleScene.scene->unk_68 = 50.0f;
    }

    sBattleScene.scene->unk_7A = 0;

    if (D_84390288 != 0) {
        sBattleScene.scene->unk_68 = 25.0f;
    }
}

void BattleScene_ApplyCameraShake(CameraNode* arg0) {
    UNUSED s32 pad;
    f32 sp30;
    f32 temp_fv0;
    s16 sp2A;
    s16 sp28;

    BattleAnim_CalculateDistanceAngles(&arg0->view.eyePosition, &arg0->view.targetPosition, &sp30, &sp28, &sp2A);
    sBattleScene.scene->unk_7A += 0x38E0;
    BattleAnim_ApproachF(&sBattleScene.scene->unk_68, 0.0f, 0.18f);

    temp_fv0 = COSS(sBattleScene.scene->unk_7A) * sBattleScene.scene->unk_68;

    arg0->view.eyePosition.y += temp_fv0 / 3.0f;
    arg0->view.targetPosition.y += temp_fv0 / 3.0f;
    arg0->view.eyePosition.x += temp_fv0 * SINS(sp2A + 0x4000);
    arg0->view.targetPosition.x += temp_fv0 * SINS(sp2A + 0x4000);
}

void BattleScene_StartCameraShake(UNUSED ActivePokemon* active_pokemon) {
    sBattleScene.scene->unk_68 = 30.0f;
    sBattleScene.scene->unk_7A = 0;
}

s32 BattleScene_IsCameraAngleSettled(s16 arg0, s16 arg1) {
    s16 temp_v1;
    s16 var_v1;

    temp_v1 = arg1 - arg0;
    var_v1 = arg1;
    if (temp_v1 > 0) {
        temp_v1 -= 0xE38;
        if (temp_v1 > 0) {
            var_v1 -= temp_v1;
        } else {
            var_v1 = arg1;
        }
    } else {
        temp_v1 += 0xE38;
        if (temp_v1 < 0) {
            var_v1 -= temp_v1;
        } else {
            var_v1 = arg1;
        }
    }

    if (var_v1 == arg1) {
        return 1;
    }
    return 0;
}

/**
 * Subtracts delta_theta from the scene's rotationY each call and recomputes the camera eye from the scene's
 * distance and rotationX/rotationY around the target. Returns 1 once rotationY reaches final_angle_y.
 */
s32 BattleScene_ApproachCameraRotationY(CameraNode* camera, s16 delta_theta, s16 final_angle_y) {
    if (delta_theta > 0) {
        sBattleScene.scene->rotationY -= delta_theta;
        if ((sBattleScene.scene->rotationY < final_angle_y) && ((final_angle_y - delta_theta) < sBattleScene.scene->rotationY)) {
            sBattleScene.scene->rotationY = final_angle_y;
            return 1;
        }
    } else {
        sBattleScene.scene->rotationY -= delta_theta;
        if ((final_angle_y < sBattleScene.scene->rotationY) && (sBattleScene.scene->rotationY < (final_angle_y - delta_theta))) {
            sBattleScene.scene->rotationY = final_angle_y;
            return 1;
        }
    }

    Camera_ComputeEyeFromAngles(&camera->view.targetPosition, &camera->view.eyePosition, sBattleScene.scene->distance, sBattleScene.scene->rotationX,
                  sBattleScene.scene->rotationY);
    return 0;
}

/**
 * Same rotationY stepping as BattleScene_ApproachCameraRotationY, without recomputing the camera eye.
 */
s32 BattleScene_ApproachCameraRotationYNoEyeUpdate(UNUSED CameraNode* arg0, s16 arg1, s16 arg2) {
    sBattleScene.scene->rotationY -= arg1;

    if (arg1 > 0) {
        if ((arg2 >= sBattleScene.scene->rotationY) && (sBattleScene.scene->rotationY >= (arg2 - arg1))) {
            sBattleScene.scene->rotationY = arg2;
            return 1;
        }
    } else if ((sBattleScene.scene->rotationY >= arg2) && ((arg2 - arg1) >= sBattleScene.scene->rotationY)) {
        sBattleScene.scene->rotationY = arg2;
        return 1;
    }
    return 0;
}

void BattleScene_SetCameraAnglesAndDistance(s16 rotation_x, s16 rotation_y, f32 distance) {
    sBattleScene.scene->rotationX = rotation_x;
    sBattleScene.scene->rotationY = rotation_y;
    sBattleScene.scene->distance = distance;
}

void BattleScene_SetCameraSendOutShot(CameraNode* camera, s32 opposite_side) {
    static u8 sOnix = ONIX;
    static u16 D_84385B64 = 0x38F;

    ActivePokemonState* state;
    UNUSED s32 pad;
    ActivePokemon* opponent_pokemon;
    Vec3f anchor_position;
    f32 camera_distance;
    UNUSED u16 pad2;
    UNUSED u8 pad3;
    u8 onix;
    UNUSED u16 pad4;
    u16 sp4C;
    s32 idx = sBattleScene.scene->actingSide == 0;

    state = &sActivePokemon[idx]->state;
    onix = sOnix;
    sp4C = D_84385B64;
    opponent_pokemon = sActivePokemon[opposite_side == 0];
    camera->perspective.verticalFieldOfView = 60.0f;

    BattleScene_ResetCameraFrameCounter();
    BattleAnim_GetOwnerDefaultAnchorPosition(opponent_pokemon, &anchor_position);

    anchor_position.y /= 3.0f;
    if (opponent_pokemon->state.flags & ACTIVE_UNDERGROUND) {
        anchor_position.y = 0.0f;
    }

    BattleScene_SetCameraAtFromAnchor(opponent_pokemon, camera, anchor_position, 0, 0, 0.0f, 0.0f);

    if ((state->replacementState != 0x10) && !(state->flags & ACTIVE_UNDERGROUND)) {
        BattleScene_EnterStateRow(opponent_pokemon, 1);
    }

    opponent_pokemon = sActivePokemon[opposite_side];
    if (opponent_pokemon->modelTransform.layoutConfig.backCameraShotOffset.x == 0.0f) {
        BattleAnim_GetOwnerDefaultAnchorPosition(opponent_pokemon, &anchor_position);
    } else {
        BattleAnim_Vec3fCopy(&anchor_position, &opponent_pokemon->modelTransform.layoutConfig.backCameraShotOffset);
        anchor_position.y += opponent_pokemon->displayObject.position.y - BattleScene_GetParticipantModelRestingHeight(opponent_pokemon);
    }

    camera_distance = BattleScene_ScaleCameraDistance(opponent_pokemon, 3.0f, 300.0f);
    if ((BattleScene_ByteArrayContains(opponent_pokemon->displayObject.objectType, &onix, 1) != 0) && (opposite_side == 1)) {
        BattleScene_SetCameraEyeFromPosition(camera, anchor_position, 0, BattleScene_GetParticipantFacingAngle(opponent_pokemon) - (opponent_pokemon->facingDirection * 0x6AA4), camera_distance, 20.0f);
    } else if (BattleScene_ByteArrayContains(opponent_pokemon->displayObject.objectType, (u8*)&sp4C, 2) != 0) {
        BattleScene_SetCameraEyeFromPosition(camera, anchor_position, 0, BattleScene_GetParticipantFacingAngle(opponent_pokemon) - (opponent_pokemon->facingDirection * 0x6AA4), camera_distance, 20.0f);
    } else {
        BattleScene_SetCameraEyeFromPosition(camera, anchor_position, 0, BattleScene_GetParticipantFacingAngle(opponent_pokemon) - (opponent_pokemon->facingDirection * 0x754E), camera_distance, 0.0f);
    }
}

void BattleScene_ClampCameraEyeFloor(CameraNode* camera) {
    if (camera->view.eyePosition.y <= 13.0f) {
        camera->view.eyePosition.y = 13.0f;
    }
}

void BattleScene_UpdateCameraModelOffset(ActivePokemon* pokemon, CameraNode* camera) {
    ModelTransform* model_transform = &pokemon->modelTransform;
    UNUSED s32 pad[5];
    f32 y_offset_adjustment = 0.0f;

    if (BattleScene_ByteArrayContains(pokemon->displayObject.objectType, &sAerodactyl, 1) != 0) {
        y_offset_adjustment = BattleAnim_GetOwnerModelHeight(pokemon) / 1.5f;
    } else if (BattleScene_ByteArrayContains(pokemon->displayObject.objectType, &sFearow, 1) != 0) {
        y_offset_adjustment = BattleAnim_GetOwnerModelHeight(pokemon) * 0.5f;
    } else if ((BattleScene_ByteArrayContains(pokemon->displayObject.objectType, &sFarfetchD, 1) != 0) && (pokemon->state.flags & ACTIVE_AIRBORNE)) {
        y_offset_adjustment = 2.0f * BattleAnim_GetOwnerModelHeight(pokemon);
    } else if ((BattleScene_ByteArrayContains(pokemon->displayObject.objectType, sDoduoEvoLine, 2) != 0) && (pokemon->state.flags & ACTIVE_AIRBORNE)) {
        y_offset_adjustment = BattleAnim_GetOwnerModelHeight(pokemon) * 1.5f;
    }

    Vec3f_SetComponentsDuplicate(&pokemon->modelTransform.anchorAdjustment, pokemon->modelTransform.layoutConfig.frontCameraShotOffset.x, pokemon->modelTransform.layoutConfig.frontCameraShotOffset.y, pokemon->modelTransform.layoutConfig.frontCameraShotOffset.z);
    pokemon->modelTransform.anchorAdjustment.y = (pokemon->displayObject.position.y - BattleScene_GetParticipantModelRestingHeight(pokemon)) + pokemon->modelTransform.anchorAdjustment.y + y_offset_adjustment;
    BattleAnim_Vec3fCopy(&sBattleScene.scene->savedCameraTarget, &camera->view.targetPosition);
    BattleAnim_Vec3fCopy(&sBattleScene.scene->savedCameraEye, &camera->view.eyePosition);
    Vec3f_CalculateDistanceAngles(&sBattleScene.scene->savedCameraTarget, &sBattleScene.scene->savedCameraEye, &sBattleScene.scene->targetDistance, &model_transform->rotationX,
                  &model_transform->rotationY);
    Vec3f_CalculateDistanceAngles(&pokemon->modelTransform.anchorAdjustment, &camera->view.targetPosition, &model_transform->distance, &model_transform->rotationX, &model_transform->rotationY);
    sBattleScene.scene->unk_6C = 0.0f;
    sBattleScene.scene->unk_74 = 0.0f;
}

void BattleScene_ClearCameraOffsetVectors(void) {
    Vec3f_SetComponentsDuplicate(&sBattleScene.scene->targetPosition, 0.0f, 0.0f, 0.0f);
    Vec3f_SetComponentsDuplicate(&sBattleScene.scene->unk_A0, 0.0f, 0.0f, 0.0f);
}

void BattleScene_SetCameraAtFromAnchor(ActivePokemon* pokemon, CameraNode* camera, Vec3f starting_position, s16 rotation_x, s16 rotation_y, f32 distance, f32 y_offset) {
    Camera_ComputeEyeFromAngles(&starting_position, &camera->view.targetPosition, distance, rotation_x, rotation_y);
    camera->view.targetPosition.y += y_offset;
    camera->view.targetPosition.y += BattleScene_GetParticipantCameraTargetOffsetY(pokemon);
}

void BattleScene_SetCameraAtFromPosition(CameraNode* camera, Vec3f starting_position, s16 rotation_x, s16 rotation_y, f32 distance, f32 y_offset) {
    Camera_ComputeEyeFromAngles(&starting_position, &camera->view.targetPosition, distance, rotation_x, rotation_y);
    camera->view.targetPosition.y += y_offset;
}

void BattleScene_SetCameraEyeFromPosition(CameraNode* camera, Vec3f starting_position, s16 rotation_x, s16 rotation_y, f32 distance, f32 y_offset) {
    Camera_ComputeEyeFromAngles(&starting_position, &camera->view.eyePosition, distance, rotation_x, rotation_y);
    camera->view.eyePosition.y += y_offset;
}

void BattleScene_SetCameraEyeFromAt(CameraNode* camera, s16 rotation_x, s16 rotation_y, f32 distance, f32 y_offset) {
    Camera_ComputeEyeFromAngles(&camera->view.targetPosition, &camera->view.eyePosition, distance, rotation_x, rotation_y);
    camera->view.eyePosition.y += y_offset;
}

void BattleScene_SetCameraEyeAndStoreAngles(CameraNode* camera, Vec3f starting_postion, s16 rotation_x, s16 rotation_y, f32 distance, f32 y_offset) {
    BattleScene_SetCameraAnglesAndDistance(rotation_x, rotation_y, distance);
    Camera_ComputeEyeFromAngles(&starting_postion, &camera->view.eyePosition, distance, rotation_x, rotation_y);
    camera->view.eyePosition.y += y_offset;
}

void BattleScene_SetCameraEyeAndStoreAnglesFromAt(CameraNode* camera, s16 rotation_x, s16 rotation_y, f32 distance, f32 y_offset) {
    BattleScene_SetCameraAnglesAndDistance(rotation_x, rotation_y, distance);
    Camera_ComputeEyeFromAngles(&camera->view.targetPosition, &camera->view.eyePosition, distance, rotation_x, rotation_y);
    camera->view.eyePosition.y += y_offset;
}

void BattleScene_CameraHelperNop(UNUSED CameraNode* arg0) {
}

void BattleScene_SnapFloatPair(f32* a, f32* b) {
    f32 difference;

    if (*a != *b) {
        if (*b < *a) {
            difference = *a - *b;
            if (difference <= 1e-05) {
                *b = *a;
            }
        } else {
            difference = *b - *a;
            if (difference <= 1e-05) {
                *a = *b;
            }
        }
    }
}

void BattleScene_ApplyOwnerFramingAdjustments(CameraNode* camera, ActivePokemon* pokemon, UNUSED f32 arg2) {
    ModelTransform* transform = &pokemon->modelTransform;
    f32 speed;
    Vec3f anchor;
    Vec3f target_position;
    Vec3f intermediate_anchor;
    Vec3f sp40;
    f32 distance_adjustment;
    f32 adjusted_width;
    s16 x_angle_to_anchor;
    s16 y_angle_to_anchor;

    BattleAnim_Vec3fCopy(&intermediate_anchor, &camera->view.targetPosition);
    BattleAnim_Vec3fCopy(&sp40, &camera->view.targetPosition);
    BattleScene_GetOwnerCameraAnchor(pokemon, &anchor);
    Vec3f_CalculateDistanceAngles(&anchor, &pokemon->modelTransform.anchorAdjustment, &distance_adjustment, &x_angle_to_anchor, &y_angle_to_anchor);

    switch (pokemon->displayObject.objectType) {
        case ONIX:
            camera->perspective.verticalFieldOfView = 50.0f;
            return;

        case GYARADOS:
            camera->perspective.verticalFieldOfView = 50.0f;
            if (distance_adjustment < (BattleScene_GetParticipantModelWidth(pokemon) * 0.5f)) {
                BattleAnim_Vec3fCopy(&anchor, &pokemon->modelTransform.anchorAdjustment);
            }
            break;

        case NIDORAN_F: // karodo
        case ZUBAT: // karodo
        case DIGLETT: // karodo
        case DUGTRIO: // karodo
        case CLOYSTER:
            if (distance_adjustment < (BattleScene_GetParticipantModelWidth(pokemon) * 5.0f)) {
                BattleAnim_Vec3fCopy(&anchor, &pokemon->modelTransform.anchorAdjustment);
            }
            break;

        case PIDGEOT:
            if (distance_adjustment < (BattleScene_GetParticipantModelWidth(pokemon) * 1.5f)) {
                BattleAnim_Vec3fCopy(&anchor, &pokemon->modelTransform.anchorAdjustment);
            }
            break;

        case JIGGLYPUFF: // karodo
        case GRIMER: // karodo
        case MUK: // karodo
        case MEWTWO:
            if (distance_adjustment < BattleScene_GetParticipantModelWidth(pokemon)) {
                BattleAnim_Vec3fCopy(&anchor, &pokemon->modelTransform.anchorAdjustment);
            }
            break;

        case GEODUDE:
        case ELECTRODE:
        case EXEGGUTOR:
            if (distance_adjustment < (BattleScene_GetParticipantModelWidth(pokemon) / 1.2f)) {
                BattleAnim_Vec3fCopy(&anchor, &pokemon->modelTransform.anchorAdjustment);
            }
            break;

        case SQUIRTLE: // karodo
        case PIDGEOTTO: // karodo
        case FEAROW: // karodo
        case GOLEM: // karodo
        case GASTLY: // karodo
        case LAPRAS:
            if (distance_adjustment < (BattleScene_GetParticipantModelWidth(pokemon) / 3.0f)) {
                BattleAnim_Vec3fCopy(&anchor, &pokemon->modelTransform.anchorAdjustment);
            }
            break;

        default:
            if (distance_adjustment < (BattleScene_GetParticipantModelWidth(pokemon) * 0.5f)) {
                BattleAnim_Vec3fCopy(&anchor, &pokemon->modelTransform.anchorAdjustment);
            }
            break;
    }

    anchor.y = pokemon->modelTransform.anchorAdjustment.y;
    Camera_ComputeEyeFromAngles(&anchor, &target_position, transform->distance, transform->rotationX, transform->rotationY);
    intermediate_anchor.y = target_position.y;
    Vec3f_CalculateDistanceAngles(&intermediate_anchor, &target_position, &distance_adjustment, &x_angle_to_anchor, &y_angle_to_anchor);

    switch (pokemon->displayObject.objectType) {
        case GRIMER: // karodo
        case MUK: // karodo
        case JOLTEON:
            speed = distance_adjustment / (BattleScene_GetParticipantModelWidth(pokemon) * 50.0f);
            break;

        case FEAROW: // karodo
        case GOLEM: // karodo
        case MEWTWO:
            speed = distance_adjustment / (BattleScene_GetParticipantModelWidth(pokemon) * 1.5f);
            break;

        default:
            adjusted_width = distance_adjustment / BattleScene_GetParticipantModelWidth(pokemon);
            if (adjusted_width <= 0.1f) {
                speed = distance_adjustment / (BattleScene_GetParticipantModelWidth(pokemon) * 15.0f);
            } else if (adjusted_width <= 0.2f) {
                speed = distance_adjustment / (BattleScene_GetParticipantModelWidth(pokemon) * 7.0f);
            } else if (adjusted_width <= 0.3f) {
                speed = distance_adjustment / (BattleScene_GetParticipantModelWidth(pokemon) * 5.0f);
            } else if (adjusted_width <= 0.4f) {
                speed = distance_adjustment / (BattleScene_GetParticipantModelWidth(pokemon) * 3.0f);
            } else if (adjusted_width <= 0.5f) {
                speed = distance_adjustment / (BattleScene_GetParticipantModelWidth(pokemon) * 1.5f);
            } else {
                speed = distance_adjustment / BattleScene_GetParticipantModelWidth(pokemon);
            }
            break;
    }

    if (speed > 1.0f) {
        speed = 1.0f;
    }

    Camera_ComputeEyeFromAngles(&intermediate_anchor, &camera->view.targetPosition, distance_adjustment * speed, 0, y_angle_to_anchor);
    BattleScene_GetOwnerCameraAnchor(pokemon, &anchor);
    Camera_ComputeEyeFromAngles(&anchor, &target_position, transform->distance, transform->rotationX, transform->rotationY);

    distance_adjustment = sp40.y - target_position.y;
    if (distance_adjustment < 0.0f) {
        distance_adjustment *= -1.0f;
    }

    if (pokemon->modelTransformId == 0xD) {
        speed = 0;
    } else if ((pokemon->displayObject.objectType == GRIMER) || (pokemon->displayObject.objectType == MUK)) {
        adjusted_width = distance_adjustment / BattleScene_GetParticipantModelWidth(pokemon);
        if (adjusted_width <= 0.05f) {
            speed = distance_adjustment / (BattleScene_GetParticipantModelWidth(pokemon) * 90.0f);
        } else if (adjusted_width <= 0.1f) {
            speed = distance_adjustment / (BattleScene_GetParticipantModelWidth(pokemon) * 70.0f);
        } else if (adjusted_width <= 0.2f) {
            speed = distance_adjustment / (BattleScene_GetParticipantModelWidth(pokemon) * 65.0f);
        } else if (adjusted_width <= 0.3f) {
            speed = distance_adjustment / (BattleScene_GetParticipantModelWidth(pokemon) * 63.0f);
        } else {
            speed = distance_adjustment / (BattleScene_GetParticipantModelWidth(pokemon) * 45.0f);
        }
    } else {
        adjusted_width = distance_adjustment / BattleScene_GetParticipantModelWidth(pokemon);
        if (adjusted_width <= 0.05f) {
            speed = distance_adjustment / (BattleScene_GetParticipantModelWidth(pokemon) * 50.0f);
        } else if (adjusted_width <= 0.1f) {
            speed = distance_adjustment / (BattleScene_GetParticipantModelWidth(pokemon) * 30.0f);
        } else if (adjusted_width <= 0.2f) {
            speed = distance_adjustment / (BattleScene_GetParticipantModelWidth(pokemon) * 25.0f);
        } else if (adjusted_width <= 0.3f) {
            speed = distance_adjustment / (BattleScene_GetParticipantModelWidth(pokemon) * 23.0f);
        } else {
            speed = distance_adjustment / (BattleScene_GetParticipantModelWidth(pokemon) * 15.0f);
        }
    }

    if (speed > 1.0f) {
        speed = 1.0f;
    }

    BattleAnim_ApproachF(&sp40.y, target_position.y, speed);
    camera->view.targetPosition.y = sp40.y;
    BattleScene_SnapFloatPair(&camera->view.targetPosition.x, &camera->view.eyePosition.x);
    BattleScene_SnapFloatPair(&camera->view.targetPosition.y, &camera->view.eyePosition.y);
    BattleScene_SnapFloatPair(&camera->view.targetPosition.z, &camera->view.eyePosition.z);
}

void BattleScene_RecomputeCameraDistance(CameraNode* camera, ActivePokemon* pokemon) {
    UNUSED s32 pad[5];
    f32 delta_to_target_distance;
    f32 step_distance;
    s32 angle_y_degrees;
    s16 sp3E;
    s16 sp3C;
    f32 raw_distance;
    s16 angle_x;
    s16 raw_angle_y;
    u8 pokemon_that_use_raw_distance[] = { VILEPLUME, PERSIAN, STARMIE, GLOOM, WEEZING };

    Vec3f_CalculateDistanceAngles(&sBattleScene.scene->savedCameraTarget, &sBattleScene.scene->savedCameraEye, &raw_distance, &angle_x, &raw_angle_y);
    Vec3f_CalculateDistanceAngles(&camera->view.targetPosition, &camera->view.eyePosition, &step_distance, &sp3E, &sp3C);

    angle_y_degrees = raw_angle_y / 182;
    if ((angle_y_degrees == 90) || (angle_y_degrees == -90)) {
        if (BattleScene_ByteArrayContains(pokemon->displayObject.objectType, pokemon_that_use_raw_distance, sizeof(pokemon_that_use_raw_distance)) != 0) {
            step_distance = raw_distance;
        } else {
            step_distance = Math_StepToF(step_distance, raw_distance, 30.0f, 5.0f);
        }
    } else {
        delta_to_target_distance = sBattleScene.scene->targetDistance - step_distance;
        if (delta_to_target_distance < 0.0f) {
            delta_to_target_distance *= -1.0f;
        }

        if (step_distance < raw_distance) {
            step_distance = Math_StepToF(step_distance, sBattleScene.scene->targetDistance, delta_to_target_distance * 0.4f, delta_to_target_distance * 0.08f);
        }
    }
    Camera_ComputeEyeFromAngles(&camera->view.targetPosition, &camera->view.eyePosition, step_distance, angle_x, raw_angle_y);
}

void BattleScene_ReframeAndRecomputeDistance(CameraNode* camera, ActivePokemon* pokemon) {
    BattleScene_ApplyOwnerFramingAdjustments(camera, pokemon, 50.0f);
    BattleScene_RecomputeCameraDistance(camera, pokemon);
}

void BattleScene_ReframeAndRecomputeDistanceForSwitch(CameraNode* camera, ActivePokemon* pokemon) {
    BattleScene_ApplyOwnerFramingAdjustments(camera, pokemon, 90.0f);
    BattleScene_RecomputeCameraDistance(camera, pokemon);
}

void BattleScene_ApplyOwnerFramingOnly(CameraNode* camera, ActivePokemon* pokemon) {
    BattleScene_ApplyOwnerFramingAdjustments(camera, pokemon, 50.0f);
}

void BattleScene_UpdateCameraTargetFromOwner(CameraNode* camera, ActivePokemon* pokemon) {
    ModelTransform* model_transform = &pokemon->modelTransform;
    UNUSED s32 pad;
    Vec3f starting_eye;
    Vec3f target;
    Vec3f starting_position;
    f32 distance;
    f32 distance_scale_down;
    s16 angle_x;
    s16 angle_y;

    BattleAnim_Vec3fCopy(&starting_position, &camera->view.targetPosition);
    BattleScene_GetOwnerCameraAnchor(pokemon, &starting_eye);
    Vec3f_CalculateDistanceAngles(&starting_eye, &pokemon->modelTransform.anchorAdjustment, &distance, &angle_x, &angle_y);

    if (distance < (BattleScene_GetParticipantModelWidth(pokemon) * 1.1f)) {
        BattleAnim_Vec3fCopy(&starting_eye, &pokemon->modelTransform.anchorAdjustment);
    }

    Camera_ComputeEyeFromAngles(&starting_eye, &target, model_transform->distance, model_transform->rotationX, model_transform->rotationY);
    Vec3f_CalculateDistanceAngles(&starting_position, &target, &distance, &angle_x, &angle_y);

    distance_scale_down = (distance / (BattleScene_GetParticipantModelWidth(pokemon) * 80.0f)) * 3.0f;
    if (distance_scale_down >= 1.0f) {
        distance_scale_down = 1.0f;
    }

    Camera_ComputeEyeFromAngles(&starting_position, &camera->view.targetPosition, distance * distance_scale_down, angle_x, angle_y);
}

s32 BattleScene_FrameCamera(CameraNode* camera, ActivePokemon* pokemon) {
    Vec3f anchor_position;
    UNUSED s32 pad[3];
    Vec3f target_position;
    Vec3f eye_position;
    UNUSED s32 pad2[3];
    s16 current_rotation_y;
    s16 target_rotation_y;
    s16 current_rotation_x;
    s16 target_rotation_x;
    UNUSED s32 pad3[2];
    BattleMonRuntime* runtime;

    runtime = &sActivePokemon[BattleScene_GetParticipantSideIndex(pokemon) == 0]->state.runtime;

    if (pokemon->state.flags & ACTIVE_UNDERGROUND) {
        return 1;
    }

    if (runtime->usedMove == FIRE_SPIN) {
        BattleScene_SelectCameraShot(pokemon, camera, 4);
        return 1;
    }

    BattleAnim_Vec3fCopy(&target_position, &camera->view.targetPosition);
    BattleAnim_Vec3fCopy(&eye_position, &camera->view.eyePosition);

    if ((pokemon->displayObject.objectType != ONIX) && (pokemon->modelTransformId != 0xD)) {
        BattleScene_UpdateCameraTargetFromOwner(camera, pokemon);
    }

    BattleAnim_GetOwnerDefaultAnchorPosition(pokemon, &anchor_position);
    gCameraDistTarget = BattleScene_ScaleCameraDistance(pokemon, 3.5f, 300.0f);
    BattleScene_SetCameraEyeFromPosition(camera, anchor_position, 0, BattleScene_GetParticipantFacingAngle(pokemon), gCameraDistTarget, 0.0f);

    if (camera->view.eyePosition.y <= 13.0f) {
        camera->view.eyePosition.y = 13.0f;
    }

    BattleAnim_CalculateDistanceAngles(&anchor_position, &camera->view.eyePosition, &sDistanceBetweenCameraEyeAndAnchor, &target_rotation_x, &target_rotation_y);
    BattleAnim_CalculateDistanceAngles(&anchor_position, &eye_position, &gCameraDistTarget, &current_rotation_x, &current_rotation_y);
    BattleAnim_ApproachF(&gCameraDistTarget, sDistanceBetweenCameraEyeAndAnchor, 0.2f);
    BattleAnim_ApproachF(&sBattleScene.scene->targetPosition.z, 0.2f, 0.03f);
    BattleAnim_StepToS16VariableRate(&current_rotation_y, target_rotation_y, sBattleScene.scene->targetPosition.z);
    BattleAnim_StepToS16VariableRate(&current_rotation_x, target_rotation_x, sBattleScene.scene->targetPosition.z);
    BattleScene_SetCameraEyeFromPosition(camera, anchor_position, current_rotation_x, current_rotation_y, gCameraDistTarget, 0.0f);

    if ((target_rotation_x == current_rotation_x) && (target_rotation_y == current_rotation_y)) {
        return 1;
    }
    return 0;
}

void BattleScene_ApproachCameraTargetFromOwner(CameraNode* camera, ActivePokemon* pokemon) {
    ModelTransform* model_transform = &pokemon->modelTransform;
    Vec3f anchor;
    Vec3f intermediate_position;
    UNUSED s32 pad[3];
    Vec3f starting_position;
    f32 distance;
    UNUSED s32 pad2[1];
    s16 rotation_x;
    s16 rotation_y;

    BattleAnim_Vec3fCopy(&starting_position, &camera->view.targetPosition);
    BattleScene_GetOwnerCameraAnchor(pokemon, &anchor);
    Camera_ComputeEyeFromAngles(&anchor, &intermediate_position, model_transform->distance, model_transform->rotationX, model_transform->rotationY);
    Vec3f_CalculateDistanceAngles(&starting_position, &intermediate_position, &distance, &rotation_x, &rotation_y);
    sBattleScene.scene->targetPosition.z = Math_StepToF(sBattleScene.scene->targetPosition.z, 0.8f, 0.05f, 0.05f);
    Camera_ComputeEyeFromAngles(&starting_position, &camera->view.targetPosition, sBattleScene.scene->targetPosition.z * distance, rotation_x, rotation_y);
}

void BattleScene_UpdateCameraVerticalFraming(CameraNode* camera, ActivePokemon* pokemon) {
    ModelTransform* model_transform = &pokemon->modelTransform;
    Vec3f anchor_position;
    Vec3f target_position;
    UNUSED s32 pad[3];
    Vec3f unused_tp;
    UNUSED s32 pad2[3];

    BattleAnim_Vec3fCopy(&unused_tp, &camera->view.targetPosition);
    BattleScene_GetOwnerCameraAnchor(pokemon, &anchor_position);
    Camera_ComputeEyeFromAngles(&anchor_position, &target_position, model_transform->distance, model_transform->rotationX, model_transform->rotationY);
    BattleAnim_ApproachF(&camera->view.targetPosition.y, target_position.y, 0.1f);
    if (camera->view.targetPosition.y < -10.0f) {
        camera->view.targetPosition.y = -10.0f;
    }
    BattleAnim_ApproachF(&camera->view.eyePosition.y, BattleAnim_GetOwnerModelHeight(pokemon) * 4.0f, 0.04f);
}

void BattleScene_RecenterCameraOnOwner(CameraNode* camera, ActivePokemon* pokemon) {
    Vec3f anchor_position;
    Vec3f starting_position;
    Vec3f sp5C;
    f32 distance;
    s16 eye_rotation_x;
    s16 eye_rotation_y;
    s16 current_azimuth;
    s16 target_azimuth;
    s16 current_elevation;
    s16 target_elevation;

    BattleAnim_Vec3fCopy(&starting_position, &camera->view.targetPosition);
    BattleAnim_Vec3fCopy(&sp5C, &camera->view.eyePosition);
    BattleAnim_GetOwnerDefaultAnchorPosition(pokemon, &anchor_position);
    gCameraDistTarget = BattleScene_ScaleCameraDistance(pokemon, 3.5f, 300.0f);
    BattleScene_SetCameraAtFromAnchor(pokemon, camera, anchor_position, 0, BattleScene_GetParticipantFacingAngle(pokemon), 0.0f, 0.0f);
    BattleScene_SetCameraEyeFromPosition(camera, anchor_position, 0, BattleScene_GetParticipantFacingAngle(pokemon), gCameraDistTarget, 0.0f);
    BattleAnim_CalculateDistanceAngles(&camera->view.targetPosition, &camera->view.eyePosition, &sDistanceBetweenCameraEyeAndAnchor, &target_elevation, &target_azimuth);
    BattleAnim_CalculateDistanceAngles(&starting_position, &sp5C, &gCameraDistTarget, &current_elevation, &current_azimuth);
    BattleAnim_ApproachF(&gCameraDistTarget, sDistanceBetweenCameraEyeAndAnchor, 0.05f);
    BattleAnim_StepToS16(&current_azimuth, target_azimuth, 264);
    BattleAnim_StepToS16(&current_elevation, target_elevation, 182);
    BattleAnim_CalculateDistanceAngles(&starting_position, &camera->view.targetPosition, &distance, &eye_rotation_x, &eye_rotation_y);
    Camera_ComputeEyeFromAngles(&starting_position, &camera->view.targetPosition, distance / 8, eye_rotation_x, eye_rotation_y);
    BattleScene_SetCameraEyeFromAt(camera, current_elevation, current_azimuth, gCameraDistTarget, 0.0f);
}

void BattleScene_ApproachCameraTarget(CameraNode* camera, ActivePokemon* pokemon, UNUSED s32 arg2) {
    ModelTransform* pokemon_model = &pokemon->modelTransform;
    Vec3f anchor_position;
    Vec3f target_position;
    Vec3f starting_position;
    f32 distance;
    s16 elevation;
    s16 azimuth;
    UNUSED s32 pad;

    BattleScene_GetOwnerCameraAnchor(pokemon, &anchor_position);
    BattleAnim_Vec3fCopy(&starting_position, &camera->view.targetPosition);
    Camera_ComputeEyeFromAngles(&anchor_position, &target_position, pokemon_model->distance, pokemon_model->rotationX, pokemon_model->rotationY);
    BattleAnim_CalculateDistanceAngles(&starting_position, &target_position, &distance, &elevation, &azimuth);
    Camera_ComputeEyeFromAngles(&starting_position, &camera->view.targetPosition, distance * 0.1f, elevation, azimuth);
}

void BattleScene_ApproachCameraZoom(CameraNode* camera, ActivePokemon* pokemon) {
    s16 elevation;
    s16 azimuth;

    gCameraDistTarget = BattleScene_ScaleCameraDistance(pokemon, 8.0f, 500.0f);
    BattleAnim_CalculateDistanceAngles(&camera->view.targetPosition, &camera->view.eyePosition, &sDistanceBetweenCameraEyeAndAnchor, &elevation, &azimuth);
    BattleAnim_ApproachF(&sDistanceBetweenCameraEyeAndAnchor, gCameraDistTarget, 0.03f);
    BattleAnim_StepToS16(&elevation, 3640, 91);
    BattleScene_SetCameraEyeFromAt(camera, elevation, azimuth, sDistanceBetweenCameraEyeAndAnchor, 0.0f);
}

s32 BattleScene_ApproachCloseCameraShot(CameraNode* camera, ActivePokemon* pokemon) {
    Vec3f target_position;
    Vec3f eye_position;
    f32 distance;
    s16 angle_y;
    s16 angle_x;

    BattleAnim_Vec3fCopy(&target_position, &camera->view.targetPosition);
    BattleAnim_Vec3fCopy(&eye_position, &camera->view.eyePosition);
    BattleScene_CameraShotTargetClose(camera, pokemon);
    Vec3f_CalculateDistanceAngles(&eye_position, &camera->view.eyePosition, &distance, &angle_x, &angle_y);
    Camera_ComputeEyeFromAngles(&eye_position, &camera->view.eyePosition, distance / 2, angle_x, angle_y);
    BattleAnim_Vec3fCopy(&camera->view.targetPosition, &target_position);

    if (distance <= 0.5f) {
        return 1;
    }
    return 0;
}

s32 BattleScene_AnimateCameraPreset(CameraNode* camera, ActivePokemon* pokemon, s32 adjust_y_offset, s32 adjust_vertical_fov) {
    UNUSED s32 pad;
    Vec3f final_target_position;
    Vec3f inital_target_position;
    Vec3f eye_position;
    Vec3f camera_target_offset;
    f32 distance;
    f32 eye_distance_from_target;
    s16 eye_elevation_from_target;
    s16 eye_azimuth_from_target;

    if (gCameraPreset.verticalFieldOfView != 200.0f) {
        camera->perspective.verticalFieldOfView = gCameraPreset.verticalFieldOfView;
    } else if (adjust_vertical_fov == 1) {
        BattleAnim_ApproachF(&camera->perspective.verticalFieldOfView, 30.0f, 0.06f);
    } else {
        camera->perspective.verticalFieldOfView = 30.0f;
    }

    BattleAnim_ApproachF(&sBattleScene.scene->targetPosition.z, gCameraPreset.targetZ, gCameraPreset.speed);
    Camera_ComputeEyeFromAngles(&gCameraPreset.startingPosition, &inital_target_position, gCameraPreset.distance, gCameraPreset.rotationX, gCameraPreset.rotationY);
    BattleAnim_Vec3fCopy(&eye_position, &camera->view.eyePosition);
    BattleAnim_CalculateDistanceAngles(&eye_position, &inital_target_position, &distance, &eye_elevation_from_target, &eye_azimuth_from_target);
    Camera_ComputeEyeFromAngles(&eye_position, &camera->view.eyePosition, sBattleScene.scene->targetPosition.z * distance, eye_elevation_from_target, eye_azimuth_from_target);
    Camera_ComputeEyeFromAngles(&gCameraPreset.startingPosition, &camera_target_offset, gCameraPreset.distanceOffset, gCameraPreset.rotationXOffset, gCameraPreset.rotationYOffset);

    if (adjust_y_offset == 1) {
        camera_target_offset.y += BattleScene_GetParticipantCameraTargetOffsetY(pokemon);
    }

    BattleAnim_Vec3fCopy(&final_target_position, &camera->view.targetPosition);
    BattleAnim_CalculateDistanceAngles(&final_target_position, &camera_target_offset, &eye_distance_from_target, &eye_elevation_from_target, &eye_azimuth_from_target);
    Camera_ComputeEyeFromAngles(&final_target_position, &camera->view.targetPosition, sBattleScene.scene->targetPosition.z * eye_distance_from_target, eye_elevation_from_target, eye_azimuth_from_target);

    if ((distance <= 1.75f) && (eye_distance_from_target <= 1.75f)) {
        return 1;
    }
    return 0;
}

s32 BattleScene_AnimateCameraPresetToOwner(CameraNode* camera, ActivePokemon* pokemon) {
    Vec3f anchor_position;
    Vec3f target_position;
    Vec3f eye_position;
    Vec3f eye_target_position;
    f32 distance;
    f32 distance_to_target;
    f32 eye_distance;
    f32 clamped_distance;
    s16 angle_x;
    s16 angle_y;

    if (gCameraPreset.verticalFieldOfView != 200.0f) {
        camera->perspective.verticalFieldOfView = gCameraPreset.verticalFieldOfView;
    } else {
        camera->perspective.verticalFieldOfView = 30.0f;
    }

    BattleAnim_ApproachF(&sBattleScene.scene->targetPosition.z, gCameraPreset.targetZ, gCameraPreset.speed);
    BattleScene_GetOwnerCameraAnchor(pokemon, &anchor_position);
    Vec3f_CalculateDistanceAngles(&gCameraPreset.startingPosition, &anchor_position, &distance, &angle_x, &angle_y);

    clamped_distance = distance / (BattleScene_GetParticipantModelWidth(pokemon) * 40.0f);
    if (clamped_distance >= 0.6f) {
        clamped_distance = 1.0f;
    }

    distance *= clamped_distance;
    distance += gCameraPreset.distance;

    Camera_ComputeEyeFromAngles(&gCameraPreset.startingPosition, &target_position, distance, gCameraPreset.rotationX, gCameraPreset.rotationY);
    BattleAnim_Vec3fCopy(&eye_position, &camera->view.eyePosition);
    BattleAnim_CalculateDistanceAngles(&eye_position, &target_position, &distance_to_target, &angle_x, &angle_y);
    Camera_ComputeEyeFromAngles(&eye_position, &camera->view.eyePosition, sBattleScene.scene->targetPosition.z * distance_to_target, angle_x, angle_y);
    BattleScene_GetOwnerCameraAnchor(pokemon, &anchor_position);
    Vec3f_CalculateDistanceAngles(&gCameraPreset.startingPosition, &anchor_position, &distance, &angle_x, &angle_y);

    clamped_distance = distance / (BattleScene_GetParticipantModelWidth(pokemon) * 40.0f);
    if (clamped_distance >= 0.6f) {
        clamped_distance = 1;
    }
    distance *= clamped_distance;
    distance += gCameraPreset.distanceOffset;

    Camera_ComputeEyeFromAngles(&gCameraPreset.startingPosition, &eye_target_position, distance, gCameraPreset.rotationXOffset, gCameraPreset.rotationYOffset);
    eye_target_position.y += BattleScene_GetParticipantCameraTargetOffsetY(pokemon);
    BattleAnim_Vec3fCopy(&anchor_position, &camera->view.targetPosition);
    BattleAnim_CalculateDistanceAngles(&anchor_position, &eye_target_position, &eye_distance, &angle_x, &angle_y);
    Camera_ComputeEyeFromAngles(&anchor_position, &camera->view.targetPosition, sBattleScene.scene->targetPosition.z * eye_distance, angle_x, angle_y);

    if ((distance_to_target <= 1.0f) && (eye_distance <= 1.0f)) {
        return 1;
    }
    return 0;
}

void BattleScene_CameraPresetNop(UNUSED CameraNode* camera) {
    if (camera) {} // karodo
}

void BattleScene_ApplyCameraPreset(CameraNode* camera, ActivePokemon* pokemon, s32 update_offset) {
    BattleScene_SetCameraAtFromPosition(camera, gCameraPreset.startingPosition, gCameraPreset.startingRotationX, gCameraPreset.startingRotationY, gCameraPreset.startingDistance, 0.0f);
    if (update_offset == 1) {
        camera->view.targetPosition.y += BattleScene_GetParticipantCameraTargetOffsetY(pokemon);
        BattleScene_SetCameraEyeFromPosition(camera, camera->view.targetPosition, gCameraPreset.eyeRotationX, gCameraPreset.eyeRotationY, gCameraPreset.eyeDistance, 0.0f);
    } else {
        BattleScene_SetCameraEyeFromPosition(camera, gCameraPreset.startingPosition, gCameraPreset.eyeRotationX, gCameraPreset.eyeRotationY, gCameraPreset.eyeDistance, 0.0f);
    }
}

void BattleScene_SetCameraPresetAtAngles(s16 starting_rotation_x, s16 starting_rotation_y, s16 rotation_x_offset, s16 rotation_y_offset) {
    gCameraPreset.startingRotationX = starting_rotation_x;
    gCameraPreset.startingRotationY = starting_rotation_y;
    gCameraPreset.rotationXOffset = rotation_x_offset;
    gCameraPreset.rotationYOffset = rotation_y_offset;
}

void BattleScene_SetCameraPresetAtDistances(f32 starting_distance, f32 distance_offset) {
    gCameraPreset.startingDistance = starting_distance;
    gCameraPreset.distanceOffset = distance_offset;
}

void BattleScene_SetCameraPresetEyeAngles(s16 eye_rotation_x, s16 eye_rotation_y, s16 rotation_x, s16 rotation_y) {
    gCameraPreset.eyeRotationX = eye_rotation_x;
    gCameraPreset.eyeRotationY = eye_rotation_y;
    gCameraPreset.rotationX = rotation_x;
    gCameraPreset.rotationY = rotation_y;
}

void BattleScene_SetCameraPresetEyeDistances(f32 eye_distance, f32 distance) {
    gCameraPreset.eyeDistance = eye_distance;
    gCameraPreset.distance = distance;
}

void BattleScene_SetCameraPresetSmoothing(f32 target_z, f32 speed) {
    gCameraPreset.targetZ = target_z;
    gCameraPreset.speed = speed;
}

void BattleScene_LoadCameraPreset(DioramaAnimData* diorama, CameraNode* camera, ActivePokemon* pokemon) {
    f32 starting_distance;
    f32 distance_offset;
    f32 eye_distance;
    f32 distance;
    f32 vert_fov;
    UNUSED s16 pad;
    s16 pokemon_facing_angle;

    BattleScene_ClearCameraOffsetVectors();
    if (diorama->verticalFieldOfView != 200.0f) {
        vert_fov = diorama->verticalFieldOfView;
        gCameraPreset.verticalFieldOfView = vert_fov;
        camera->perspective.verticalFieldOfView = vert_fov;
    } else {
        gCameraPreset.verticalFieldOfView = diorama->verticalFieldOfView;
    }

    starting_distance = BattleScene_ScaleCameraDistance(pokemon, diorama->startingDistance, 500.0f);
    distance_offset = BattleScene_ScaleCameraDistance(pokemon, diorama->distanceOffset, 500.0f);
    eye_distance = BattleScene_ScaleCameraDistance(pokemon, diorama->eyeDistance, 500.0f);
    distance = BattleScene_ScaleCameraDistance(pokemon, diorama->distance, 500.0f);
    pokemon_facing_angle = BattleScene_GetParticipantFacingAngle(pokemon);

    BattleScene_SetCameraPresetAtAngles(diorama->startingRotationX, pokemon_facing_angle - (diorama->startingRotationY * pokemon->facingDirection), diorama->rotationXOffset,
                  BattleScene_GetParticipantFacingAngle(pokemon) - (diorama->rotationYOffset * pokemon->facingDirection));
    BattleScene_SetCameraPresetAtDistances(starting_distance, distance_offset);
    pokemon_facing_angle = BattleScene_GetParticipantFacingAngle(pokemon);
    BattleScene_SetCameraPresetEyeAngles(diorama->eyeRotationX, pokemon_facing_angle - (diorama->eyeRotationY * pokemon->facingDirection), diorama->rotationX,
                  BattleScene_GetParticipantFacingAngle(pokemon) - (diorama->rotationY * pokemon->facingDirection));
    BattleScene_SetCameraPresetEyeDistances(eye_distance, distance);
    BattleScene_SetCameraPresetSmoothing(diorama->targetZ, diorama->speed);
}

void BattleScene_LoadCameraPresetUnscaled(DioramaAnimData* diorama, CameraNode* camera) {
    f32 starting_distance;
    f32 distance_offset;
    f32 eye_distance;
    f32 distance;
    f32 vert_fov;

    BattleScene_ClearCameraOffsetVectors();

    if (diorama->verticalFieldOfView != 200.0f) {
        vert_fov = diorama->verticalFieldOfView;
        gCameraPreset.verticalFieldOfView = vert_fov;
        camera->perspective.verticalFieldOfView = vert_fov;
    } else {
        gCameraPreset.verticalFieldOfView = diorama->verticalFieldOfView;
    }

    starting_distance = diorama->startingDistance;
    distance_offset = diorama->distanceOffset;
    eye_distance = diorama->eyeDistance;
    distance = diorama->distance;

    BattleScene_SetCameraPresetAtAngles(diorama->startingRotationX, diorama->startingRotationY, diorama->rotationXOffset, diorama->rotationYOffset);
    BattleScene_SetCameraPresetAtDistances(starting_distance, distance_offset);
    BattleScene_SetCameraPresetEyeAngles(diorama->eyeRotationX, diorama->eyeRotationY, diorama->rotationX, diorama->rotationY);
    BattleScene_SetCameraPresetEyeDistances(eye_distance, distance);
    BattleScene_SetCameraPresetSmoothing(diorama->targetZ, diorama->speed);
}

s32 BattleScene_AnimateCameraIntro(CameraNode* camera, ActivePokemon* pokemon) {
    Vec3f pokemon_facing_direction;
    Vec3f target_position;
    Vec3f target_eye_position;
    Vec3f eye_position;
    Vec3f target_look_at;
    f32 distance;
    UNUSED s32 pad;
    s16 rotation_x;
    s16 rotation_y;

    BattleAnim_ApproachF(&camera->perspective.verticalFieldOfView, 30.0f, 0.06f);
    Vec3f_SetComponentsDuplicate(&pokemon_facing_direction, pokemon->facingDirection * -150.0f, 5.0f, 0.0f);
    BattleAnim_ApproachF(&sBattleScene.scene->targetPosition.z, 0.2f, 0.06f);
    Camera_ComputeEyeFromAngles(&pokemon_facing_direction, &target_eye_position, 100.0f, 0, BattleScene_GetParticipantFacingAngle(pokemon));
    BattleAnim_Vec3fCopy(&eye_position, &camera->view.eyePosition);
    BattleAnim_CalculateDistanceAngles(&eye_position, &target_eye_position, &distance, &rotation_x, &rotation_y);
    Camera_ComputeEyeFromAngles(&eye_position, &camera->view.eyePosition, sBattleScene.scene->targetPosition.z * distance, rotation_x, rotation_y);
    Camera_ComputeEyeFromAngles(&pokemon_facing_direction, &target_look_at, 0.0f, 0, BattleScene_GetParticipantFacingAngle(pokemon));
    BattleAnim_Vec3fCopy(&target_position, &camera->view.targetPosition);
    BattleAnim_CalculateDistanceAngles(&target_position, &target_look_at, &distance, &rotation_x, &rotation_y);

    if (distance >= 1.0f) {
        Camera_ComputeEyeFromAngles(&target_position, &camera->view.targetPosition, sBattleScene.scene->targetPosition.z * distance, rotation_x, rotation_y);
        return 0;
    }
    return 1;
}

s32 BattleScene_AreActivePokemonHeightsSeparated(UNUSED CameraNode* arg0) {
    Vec3f anchor1;
    Vec3f anchor2;
    f32 pokemon_height_difference;

    BattleAnim_GetOwnerDefaultAnchorPosition(sActivePokemon[0], &anchor1);
    BattleAnim_GetOwnerDefaultAnchorPosition(sActivePokemon[1], &anchor2);

    pokemon_height_difference = anchor1.y - anchor2.y;
    if (anchor1.y < anchor2.y) {
        pokemon_height_difference *= -1.0f;
    }

    if (pokemon_height_difference > 35.0f) {
        return 1;
    }
    return 0;
}

void BattleScene_SetCameraActivePokemonHeightShot(CameraNode* camera, s32 side) {
    UNUSED s32 pad;
    Vec3f original_anchor_position;
    Vec3f anchor_position;
    Vec3f opponent_anchor_position;
    ActivePokemon* pokemon;

    BattleScene_ClearCameraOffsetVectors();
    camera->perspective.verticalFieldOfView = 30.0f;

    pokemon = sActivePokemon[side];
    BattleAnim_GetOwnerDefaultAnchorPosition(pokemon, &original_anchor_position);
    BattleScene_SetCameraEyeFromPosition(camera, original_anchor_position, 0, pokemon->facingDirection << 0xE, 100.0f, 0.0f);
    BattleAnim_GetOwnerDefaultAnchorPosition(sActivePokemon[side], &anchor_position);

    BattleAnim_GetOwnerDefaultAnchorPosition(sActivePokemon[side == 0], &opponent_anchor_position);
    pokemon = sActivePokemon[side == 0];
    BattleAnim_GetOwnerDefaultAnchorPosition(pokemon, &original_anchor_position);

    if (opponent_anchor_position.y < anchor_position.y) {
        original_anchor_position.y = anchor_position.y * 1.8f;
        BattleScene_SetCameraAtFromAnchor(pokemon, camera, original_anchor_position, 0, 0, 0, 0);
    } else {
        original_anchor_position.y = 0.0f;
        BattleScene_SetCameraAtFromAnchor(pokemon, camera, original_anchor_position, 0, 0, 0, 0);
    }
}

s32 BattleScene_ApproachCameraActivePokemonHeightShot(CameraNode* camera, s32 side) {
    UNUSED s32 pad;
    Vec3f target_position;
    Vec3f opponent_anchor;
    Vec3f anchor;
    Vec3f original_opponent_anchor;
    f32 distance;
    s16 rotation_x;
    s16 rotation_y;

    BattleAnim_ApproachF(&sBattleScene.scene->targetPosition.z, 0.1f, 0.05f);
    BattleAnim_GetOwnerDefaultAnchorPosition(sActivePokemon[side], &anchor);
    BattleAnim_GetOwnerDefaultAnchorPosition(sActivePokemon[!side], &original_opponent_anchor);

    if (original_opponent_anchor.y < anchor.y) {
        BattleAnim_GetOwnerDefaultAnchorPosition(sActivePokemon[!side], &opponent_anchor);
    } else {
        BattleAnim_GetOwnerDefaultAnchorPosition(sActivePokemon[!side], &opponent_anchor);
        opponent_anchor.y *= 1.3f;
    }

    BattleAnim_Vec3fCopy(&target_position, &camera->view.targetPosition);
    BattleAnim_CalculateDistanceAngles(&target_position, &opponent_anchor, &distance, &rotation_x, &rotation_y);
    Camera_ComputeEyeFromAngles(&target_position, &camera->view.targetPosition, sBattleScene.scene->targetPosition.z * distance, rotation_x, rotation_y);

    if (distance <= 1.0f) {
        return 1;
    }
    return 0;
}

void BattleScene_SetCameraOwnerShot(CameraNode* camera, ActivePokemon* pokemon) {
    BattleAnim_GetOwnerDefaultAnchorPosition(pokemon, &sBattleScene.scene->cameraAnchor);

    sBattleScene.scene->cameraAnchor.x = BattleScene_GetParticipantModelXOffset(pokemon);
    sBattleScene.scene->cameraAnchor.z = 0.0f;

    if (pokemon->state.runtime.moveCache.lastMove == WITHDRAW) {
        gCameraDistTarget = BattleScene_ScaleCameraDistance(pokemon, 4.5f, 300.0f);
        if (gCameraDistTarget <= 100.0f) {
            gCameraDistTarget = 100.0f;
        }

        if (sBattleScene.scene->cameraAnchor.y < 20.0f) {
            sBattleScene.scene->cameraAnchor.y = 20.0f;
        }
    } else {
        gCameraDistTarget = BattleScene_ScaleCameraDistance(pokemon, 3.5f, 300.0f);
    }

    BattleScene_SetCameraAtFromAnchor(pokemon, camera, sBattleScene.scene->cameraAnchor, 0, BattleScene_GetParticipantFacingAngle(pokemon), 0.0f, 0.0f);
    BattleScene_SetCameraEyeFromPosition(camera, sBattleScene.scene->cameraAnchor, 0, BattleScene_GetParticipantFacingAngle(pokemon), gCameraDistTarget, 0.0f);
}

void BattleScene_SetCameraOwnerWideShot(CameraNode* camera, ActivePokemon* pokemon) {
    f32 distance;

    BattleAnim_GetOwnerDefaultAnchorPosition(pokemon, &sBattleScene.scene->cameraAnchor);
    distance = BattleScene_ScaleCameraDistance(pokemon, 8.0f, 300.0f);
    BattleScene_SetCameraAtFromAnchor(pokemon, camera, sBattleScene.scene->cameraAnchor, 0, BattleScene_GetParticipantFacingAngle(pokemon), 0.0f, 0.0f);
    BattleScene_SetCameraEyeFromAt(camera, 0, pokemon->facingDirection << 14, distance, 0.0f);
}

void BattleScene_SetCameraPresetClose(CameraNode* camera, ActivePokemon* pokemon, s32 update_offset) {
    BattleAnim_GetOwnerDefaultAnchorPosition(pokemon, &sBattleScene.scene->cameraAnchor);
    gCameraDistTarget = BattleScene_ScaleCameraDistance(pokemon, 0.5f, 100.0f);
    sDistanceBetweenCameraEyeAndAnchor = BattleScene_ScaleCameraDistance(pokemon, 5.0f, 300.0f);
    BattleScene_SetCameraAtFromAnchor(pokemon, camera, sBattleScene.scene->cameraAnchor, 0, BattleScene_GetParticipantFacingAngle(pokemon), gCameraDistTarget, 0.0f);
    BattleScene_SetCameraEyeFromPosition(camera, sBattleScene.scene->cameraAnchor, 0, BattleScene_GetParticipantFacingAngle(pokemon) - (pokemon->facingDirection * 0x31C7), sDistanceBetweenCameraEyeAndAnchor, 0.0f);

    if (update_offset == 1) {
        BattleScene_UpdateCameraModelOffset(pokemon, camera);
    } else {
        BattleScene_ReframeAndRecomputeDistance(camera, pokemon);
        BattleScene_ReframeAndRecomputeDistance(camera, pokemon);
    }

    BattleScene_ReframeAndRecomputeDistance(camera, pokemon);
}

void BattleScene_SetCameraPresetAngledClose(CameraNode* camera, ActivePokemon* pokemon, s32 update_offset) {
    BattleAnim_GetOwnerDefaultAnchorPosition(pokemon, &sBattleScene.scene->cameraAnchor);
    gCameraDistTarget = BattleScene_ScaleCameraDistance(pokemon, 0.5f, 100.0f);
    sDistanceBetweenCameraEyeAndAnchor = BattleScene_ScaleCameraDistance(pokemon, 5.0f, 300.0f);
    BattleScene_SetCameraAtFromAnchor(pokemon, camera, sBattleScene.scene->cameraAnchor, 0, BattleScene_GetParticipantFacingAngle(pokemon), gCameraDistTarget, 0.0f);
    BattleScene_SetCameraEyeFromPosition(camera, sBattleScene.scene->cameraAnchor, 0x71C, BattleScene_GetParticipantFacingAngle(pokemon) - (pokemon->facingDirection * 0x1555), sDistanceBetweenCameraEyeAndAnchor,
                  0.0f);
    if (update_offset == 1) {
        BattleScene_UpdateCameraModelOffset(pokemon, camera);
    } else {
        BattleScene_ReframeAndRecomputeDistance(camera, pokemon);
        BattleScene_ReframeAndRecomputeDistance(camera, pokemon);
    }
    BattleScene_ReframeAndRecomputeDistance(camera, pokemon);
}

void BattleScene_SetCameraPresetOffsetClose(CameraNode* camera, ActivePokemon* pokemon, s32 update_offset) {
    BattleAnim_GetOwnerDefaultAnchorPosition(pokemon, &sBattleScene.scene->cameraAnchor);
    gCameraDistTarget = BattleScene_ScaleCameraDistance(pokemon, 0.5f, 100.0f);
    sDistanceBetweenCameraEyeAndAnchor = BattleScene_ScaleCameraDistance(pokemon, 5.0f, 300.0f);
    BattleScene_SetCameraAtFromAnchor(pokemon, camera, sBattleScene.scene->cameraAnchor, 0, BattleScene_GetParticipantFacingAngle(pokemon), gCameraDistTarget, 0.0f);
    BattleScene_SetCameraEyeFromPosition(camera, sBattleScene.scene->cameraAnchor, 0xE38, BattleScene_GetParticipantFacingAngle(pokemon), sDistanceBetweenCameraEyeAndAnchor, 0.0f);
    if (update_offset == 1) {
        BattleScene_UpdateCameraModelOffset(pokemon, camera);
    } else {
        BattleScene_ReframeAndRecomputeDistance(camera, pokemon);
        BattleScene_ReframeAndRecomputeDistance(camera, pokemon);
    }
    BattleScene_ReframeAndRecomputeDistance(camera, pokemon);
}

void BattleScene_SetCameraPresetLowClose(CameraNode* camera, ActivePokemon* pokemon, s32 update_offset) {
    UNUSED s32 pad[4];
    s16 facing_angle;

    BattleAnim_GetOwnerDefaultAnchorPosition(pokemon, &sBattleScene.scene->cameraAnchor);
    gCameraDistTarget = BattleScene_ScaleCameraDistance(pokemon, 0.5f, 100.0f);
    sDistanceBetweenCameraEyeAndAnchor = BattleScene_ScaleCameraDistance(pokemon, 5.0f, 300.0f);
    BattleScene_SetCameraAtFromAnchor(pokemon, camera, sBattleScene.scene->cameraAnchor, 0, BattleScene_GetParticipantFacingAngle(pokemon), gCameraDistTarget, 0.0f);
    facing_angle = BattleScene_GetParticipantFacingAngle(pokemon);
    BattleScene_SetCameraEyeFromPosition(camera, sBattleScene.scene->cameraAnchor, 0, facing_angle - (pokemon->facingDirection * 0x1555), sDistanceBetweenCameraEyeAndAnchor,
                  -BattleAnim_GetOwnerModelHeight(pokemon) * 0.8);
    if (update_offset == 1) {
        BattleScene_UpdateCameraModelOffset(pokemon, camera);
    } else {
        BattleScene_ReframeAndRecomputeDistance(camera, pokemon);
        BattleScene_ReframeAndRecomputeDistance(camera, pokemon);
    }
    BattleScene_ReframeAndRecomputeDistance(camera, pokemon);
}

void BattleScene_SetCameraPresetWide(CameraNode* camera, ActivePokemon* pokemon, s32 update_offset) {
    BattleAnim_GetOwnerDefaultAnchorPosition(pokemon, &sBattleScene.scene->cameraAnchor);
    gCameraDistTarget = BattleScene_ScaleCameraDistance(pokemon, 3.5f, 300.0f);
    BattleScene_SetCameraAtFromAnchor(pokemon, camera, sBattleScene.scene->cameraAnchor, 0, BattleScene_GetParticipantFacingAngle(pokemon), 0.0f, 0.0f);
    BattleScene_SetCameraEyeFromPosition(camera, sBattleScene.scene->cameraAnchor, 0, BattleScene_GetParticipantFacingAngle(pokemon), gCameraDistTarget, 0.0f);
    if (update_offset == 1) {
        BattleScene_UpdateCameraModelOffset(pokemon, camera);
    } else {
        BattleScene_ReframeAndRecomputeDistance(camera, pokemon);
        BattleScene_ReframeAndRecomputeDistance(camera, pokemon);
    }
    BattleScene_ReframeAndRecomputeDistance(camera, pokemon);
}

void BattleScene_SetCameraPresetWideStatic(CameraNode* camera, ActivePokemon* pokemon, s32 update_offset) {
    BattleAnim_GetOwnerDefaultAnchorPosition(pokemon, &sBattleScene.scene->cameraAnchor);
    gCameraDistTarget = BattleScene_ScaleCameraDistance(pokemon, 3.5f, 300.0f);
    BattleScene_SetCameraAtFromAnchor(pokemon, camera, sBattleScene.scene->cameraAnchor, 0, BattleScene_GetParticipantFacingAngle(pokemon), 0.0f, 0.0f);
    BattleScene_SetCameraEyeFromPosition(camera, sBattleScene.scene->cameraAnchor, 0, BattleScene_GetParticipantFacingAngle(pokemon), gCameraDistTarget, 0.0f);
    if (update_offset == 1) {
        BattleScene_UpdateCameraModelOffset(pokemon, camera);
    }
}

void BattleScene_SetCameraPresetFromTable(CameraNode* camera, ActivePokemon* pokemon, s32 diorama_data_index, s32 update_offset) {
    BattleAnim_GetOwnerDefaultAnchorPosition(pokemon, &gCameraPreset.startingPosition);
    BattleScene_LoadCameraPreset(&D_843849C0[diorama_data_index], camera, pokemon);
    BattleScene_ApplyCameraPreset(camera, pokemon, 1);
    BattleScene_CameraPresetNop(camera);
    if (update_offset == 1) {
        BattleScene_UpdateCameraModelOffset(pokemon, camera);
    }
}

void BattleScene_CameraShotTargetStandard(CameraNode* camera, ActivePokemon* pokemon) {
    BattleScene_ClearCameraOffsetVectors();
    BattleAnim_GetOwnerDefaultAnchorPosition(pokemon, &sBattleScene.scene->cameraAnchor);
    gCameraDistTarget = BattleScene_ScaleCameraDistance(pokemon, 5.0f, 400.0f);
    BattleScene_SetCameraAtFromAnchor(pokemon, camera, sBattleScene.scene->cameraAnchor, 0, pokemon->facingDirection << 0xE, 0.0f, 0.0f);
    BattleScene_SetCameraEyeFromPosition(camera, sBattleScene.scene->cameraAnchor, 0, pokemon->facingDirection * 0xE38, gCameraDistTarget, 0.0f);
    BattleScene_UpdateCameraModelOffset(pokemon, camera);
}

void BattleScene_CameraShotTargetAngled(CameraNode* camera, ActivePokemon* pokemon) {
    BattleScene_ClearCameraOffsetVectors();
    BattleAnim_GetOwnerDefaultAnchorPosition(pokemon, &sBattleScene.scene->cameraAnchor);
    gCameraDistTarget = BattleScene_ScaleCameraDistance(pokemon, 5.0f, 400.0f);
    BattleScene_SetCameraAtFromAnchor(pokemon, camera, sBattleScene.scene->cameraAnchor, 0, pokemon->facingDirection << 0xE, 0.0f, 0.0f);
    BattleScene_SetCameraEyeFromPosition(camera, sBattleScene.scene->cameraAnchor, 0x1555, pokemon->facingDirection * 0xE38, gCameraDistTarget, 0.0f);
    BattleScene_UpdateCameraModelOffset(pokemon, camera);
}

void BattleScene_CameraShotTargetHigh(CameraNode* camera, ActivePokemon* pokemon) {
    BattleAnim_GetOwnerDefaultAnchorPosition(pokemon, &sBattleScene.scene->cameraAnchor);
    gCameraDistTarget = BattleScene_ScaleCameraDistance(pokemon, 5.0f, 400.0f);
    BattleScene_SetCameraAtFromAnchor(pokemon, camera, sBattleScene.scene->cameraAnchor, 0, pokemon->facingDirection << 0xE, 0.0f, 0.0f);
    BattleScene_SetCameraEyeFromPosition(camera, sBattleScene.scene->cameraAnchor, 0, pokemon->facingDirection * 0xE38, gCameraDistTarget, 2.0f * BattleAnim_GetOwnerModelHeight(pokemon));
    BattleScene_UpdateCameraModelOffset(pokemon, camera);
}

void BattleScene_CameraShotTargetLow(CameraNode* camera, ActivePokemon* pokemon) {
    BattleAnim_GetOwnerDefaultAnchorPosition(pokemon, &sBattleScene.scene->cameraAnchor);
    gCameraDistTarget = BattleScene_ScaleCameraDistance(pokemon, 5.0f, 400.0f);
    BattleScene_SetCameraAtFromAnchor(pokemon, camera, sBattleScene.scene->cameraAnchor, 0, pokemon->facingDirection << 0xE, 0.0f, 0.0f);
    BattleScene_SetCameraEyeFromPosition(camera, sBattleScene.scene->cameraAnchor, 0, pokemon->facingDirection * 0xE38, gCameraDistTarget, -BattleAnim_GetOwnerModelHeight(pokemon) * 0.8f);
    if (camera->view.eyePosition.y <= 15.0f) {
        camera->view.eyePosition.y = 15.0f;
    }
    BattleScene_UpdateCameraModelOffset(pokemon, camera);
}

void BattleScene_CameraShotTargetClose(CameraNode* camera, ActivePokemon* pokemon) {
    BattleAnim_GetOwnerDefaultAnchorPosition(pokemon, &sBattleScene.scene->cameraAnchor);
    gCameraDistTarget = BattleScene_ScaleCameraDistance(pokemon, 3.5f, 300.0f);
    BattleScene_SetCameraAtFromAnchor(pokemon, camera, sBattleScene.scene->cameraAnchor, 0, BattleScene_GetParticipantFacingAngle(pokemon), 0.0f, 0.0f);
    BattleScene_SetCameraEyeFromPosition(camera, sBattleScene.scene->cameraAnchor, 0, BattleScene_GetParticipantFacingAngle(pokemon), gCameraDistTarget, 0.0f);
    BattleScene_UpdateCameraModelOffset(pokemon, camera);
}

void BattleScene_SetCameraFollowPresetNear(CameraNode* camera, ActivePokemon* pokemon, s32 update_offset) {
    BattleAnim_GetOwnerDefaultAnchorPosition(pokemon, &sBattleScene.scene->cameraAnchor);
    gCameraDistTarget = BattleScene_ScaleCameraDistance(pokemon, 3.5f, 300.0f);
    sDistanceBetweenCameraEyeAndAnchor = BattleScene_ScaleCameraDistance(pokemon, 0.5f, 100.0f);
    BattleScene_SetCameraAtFromAnchor(pokemon, camera, sBattleScene.scene->cameraAnchor, 0, BattleScene_GetParticipantFacingAngle(pokemon), sDistanceBetweenCameraEyeAndAnchor, 0.0f);
    BattleScene_SetCameraEyeFromPosition(camera, sBattleScene.scene->cameraAnchor, 0, BattleScene_GetParticipantFacingAngle(pokemon) - (pokemon->facingDirection * 0x1555), gCameraDistTarget, 0.0f);
    if (update_offset == 1) {
        BattleScene_UpdateCameraModelOffset(pokemon, camera);
    }
}

void BattleScene_SetCameraOwnerStandardShot(CameraNode* camera, ActivePokemon* pokemon) {
    BattleAnim_GetOwnerDefaultAnchorPosition(pokemon, &sBattleScene.scene->cameraAnchor);
    gCameraDistTarget = BattleScene_ScaleCameraDistance(pokemon, 3.5f, 300.0f);
    BattleScene_SetCameraAtFromAnchor(pokemon, camera, sBattleScene.scene->cameraAnchor, 0, BattleScene_GetParticipantFacingAngle(pokemon), 0.0f, 0.0f);
    BattleScene_SetCameraEyeFromPosition(camera, sBattleScene.scene->cameraAnchor, 0, BattleScene_GetParticipantFacingAngle(pokemon), gCameraDistTarget, 0.0f);
    BattleScene_UpdateCameraModelOffset(pokemon, camera);
}

void BattleScene_SelectCameraPreset(ActivePokemon* pokemon, CameraNode* camera, s32 shot_type, s32 update_offset) {
    BattleScene_ClearCameraOffsetVectors();
    switch (shot_type) {
        case 0:
            BattleScene_SetCameraPresetWide(camera, pokemon, update_offset);
            break;

        case 1:
            BattleScene_SetCameraPresetWideStatic(camera, pokemon, update_offset);
            break;

        case 2:
            BattleScene_SetCameraPresetClose(camera, pokemon, update_offset);
            break;

        case 3:
            BattleScene_SetCameraPresetOffsetClose(camera, pokemon, update_offset);
            break;

        case 4:
            BattleScene_SetCameraPresetFromTable(camera, pokemon, 0, update_offset);
            break;

        case 5:
            BattleScene_SetCameraPresetFromTable(camera, pokemon, 1, update_offset);
            break;

        case 6:
            BattleScene_SetCameraPresetAngledClose(camera, pokemon, update_offset);
            break;

        case 7:
            BattleScene_SetCameraPresetLowClose(camera, pokemon, update_offset);
            break;

        case 8:
            BattleScene_SetCameraPresetFromTable(camera, pokemon, 2, update_offset);
            break;

        case 9:
            BattleScene_SetCameraPresetFromTable(camera, pokemon, 3, update_offset);
            break;

        case 10:
            BattleScene_SetCameraPresetFromTable(camera, pokemon, 4, update_offset);
            break;

        case 11:
            BattleScene_SetCameraPresetFromTable(camera, pokemon, 5, update_offset);
            break;

        default:
            BattleScene_SetCameraPresetClose(camera, pokemon, update_offset);
            break;
    }
}

void BattleScene_SelectCameraFollowPreset(ActivePokemon* pokemon, CameraNode* camera, s32 shot_type) {
    BattleScene_ClearCameraOffsetVectors();
    switch (shot_type) {
        case 0:
            BattleScene_SetCameraPresetClose(camera, pokemon, 1);
            break;
        case 1:
            BattleScene_SetCameraPresetAngledClose(camera, pokemon, 1);
            break;
        case 2:
            BattleScene_SetCameraPresetLowClose(camera, pokemon, 1);
            break;
        case 3:
            BattleScene_SetCameraPresetWide(camera, pokemon, 1);
            break;
        case 4:
            BattleScene_SetCameraFollowPresetNear(camera, pokemon, 1);
            break;
        default:
            BattleScene_SetCameraPresetClose(camera, pokemon, 1);
            break;
    }
}

void BattleScene_SelectCameraShot(ActivePokemon* pokemon, CameraNode* camera, s32 shot_type) {
    BattleScene_ClearCameraOffsetVectors();
    switch (shot_type) {
        case 0:
            BattleScene_CameraShotTargetStandard(camera, pokemon);
            break;

        case 1:
            BattleScene_CameraShotTargetHigh(camera, pokemon);
            break;

        case 2:
            BattleScene_CameraShotTargetLow(camera, pokemon);
            break;

        case 3:
            BattleScene_CameraShotTargetClose(camera, pokemon);
            break;

        case 4:
            BattleScene_CameraShotTargetAngled(camera, pokemon);
            break;

        case 5:
            BattleScene_CameraShotTargetHigh(camera, pokemon);
            break;

        case 6:
            BattleScene_CameraShotTargetLow(camera, pokemon);
            break;

        case 7:
            BattleScene_CameraShotTargetClose(camera, pokemon);
            break;

        default:
            BattleScene_CameraShotTargetClose(camera, pokemon);
            break;
    }
}
