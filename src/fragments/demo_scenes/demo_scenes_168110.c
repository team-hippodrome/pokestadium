#include "demo_scenes.h"
#include "src/matrix.h"

typedef struct CameraFollowModelIds {
    u8 unk_00[1];
} CameraFollowModelIds; // size = ??

// Species whose follow point uses unk_04 / 1.5 instead of 0 when raised: 0x8E (142, Aerodactyl).
static CameraFollowModelIds sCameraFollowModelIds = { 0x8E };

/**
 * Returns the ceremony model's rotationY plus a half turn (0x8000); keyframe Y angles are relative to it.
 */
s16 ClearCeremony_GetReverseFacingRotationY(void) {
    return sCeremonyModel->displayObject.rotation.y + 0x8000;
}

/**
 * Returns 1 if value occurs among the first count bytes of array, else 0.
 */
s32 Util_ByteInArray(u8 value, u8* array, s32 count) {
    while (count--) {
        if (value == *array) {
            return 1;
        }
        array++;
    }
    return 0;
}

/**
 * Computes the camera follow point above a party model from its dimensions record: the model position
 * plus unk_04, or, for models raised by a nonzero unk_08, plus 0 (unk_04 / 1.5 for species 142,
 * Aerodactyl, listed in sCameraFollowModelIds). Negative heights fall back to unk_04.
 */
void ClearCeremony_ComputeFollowPoint(ClearCeremonyModel* arg0, Vec3f* arg1) {
    f32 sp24 = 0.0f;
    CameraFollowModelIds sp20 = sCameraFollowModelIds;

    if (Util_ByteInArray(sCeremonyModel->modelId, sp20.unk_00, 1) != 0) {
        sp24 = sSpeciesModelDimensions.unk_04 / 1.5f;
    }

    if (sSpeciesModelDimensions.unk_08 != 0) {
        arg1->x = arg0->displayObject.position.x;
        arg1->y = arg0->displayObject.position.y + sp24;
        arg1->z = arg0->displayObject.position.z;
    } else {
        arg1->x = arg0->displayObject.position.x;
        arg1->y = arg0->displayObject.position.y + sSpeciesModelDimensions.unk_04;
        arg1->z = arg0->displayObject.position.z;
    }

    if (arg1->y < 0) {
        arg1->y = sSpeciesModelDimensions.unk_04;
    }
}

/**
 * Places the camera directly at the keyframe's start: look-at and eye positions from the follow point
 * using the start distances/angles (Y relative to the model), look-at raised by dimensions unk_0C, and
 * eye height clamped to at least 10.
 */
void ClearCeremony_UpdateOrbitCamera(void) {
    Camera_ComputeEyeFromAngles(&sCeremonyCamera->followPoint, &sCeremonyCamera->camera->view.targetPosition, sCeremonyCamera->targetDistance,
                                sCeremonyCamera->targetRotationX, ClearCeremony_GetReverseFacingRotationY() + sCeremonyCamera->targetRotationY);
    Camera_ComputeEyeFromAngles(&sCeremonyCamera->followPoint, &sCeremonyCamera->camera->view.eyePosition, sCeremonyCamera->eyeDistance,
                                sCeremonyCamera->eyeRotationX, ClearCeremony_GetReverseFacingRotationY() + sCeremonyCamera->eyeRotationY);

    sCeremonyCamera->camera->view.targetPosition.y += sSpeciesModelDimensions.unk_0C;
    if (sCeremonyCamera->camera->view.eyePosition.y <= 10.0f) {
        sCeremonyCamera->camera->view.eyePosition.y = 10.0f;
    }
}

/**
 * Scales a keyframe distance by the species' dimensions unk_00 factor and limits it to max_distance.
 */
f32 ClearCeremony_ScaleOrbitDistance(f32 distance, f32 max_distance) {
    f32 scaled_distance = sSpeciesModelDimensions.unk_00 * distance;

    if (max_distance < scaled_distance) {
        return max_distance;
    }
    return scaled_distance;
}

/**
 * Sets the look-at start (arg0, arg1) and end (arg2, arg3) rotationX/rotationY.
 */
void ClearCeremonyCam_SetTargetAngles(s16 arg0, s16 arg1, s16 arg2, s16 arg3) {
    sCeremonyCamera->targetRotationX = arg0;
    sCeremonyCamera->targetRotationY = arg1;
    sCeremonyCamera->endTargetRotationX = arg2;
    sCeremonyCamera->endTargetRotationY = arg3;
}

/**
 * Sets the look-at start and end distances from the follow point.
 */
void ClearCeremonyCam_SetTargetDistance(f32 arg0, f32 arg1) {
    sCeremonyCamera->targetDistance = arg0;
    sCeremonyCamera->endTargetDistance = arg1;
}

/**
 * Sets the eye start (arg0, arg1) and end (arg2, arg3) rotationX/rotationY.
 */
void ClearCeremonyCam_SetEyeAngles(s16 arg0, s16 arg1, s16 arg2, s16 arg3) {
    sCeremonyCamera->eyeRotationX = arg0;
    sCeremonyCamera->eyeRotationY = arg1;
    sCeremonyCamera->endEyeRotationX = arg2;
    sCeremonyCamera->endEyeRotationY = arg3;
}

/**
 * Sets the eye start and end distances from the follow point.
 */
void ClearCeremonyCam_SetEyeDistance(f32 arg0, f32 arg1) {
    sCeremonyCamera->eyeDistance = arg0;
    sCeremonyCamera->endEyeDistance = arg1;
}

/**
 * Sets the interpolation goal and per-frame step used to ease the camera toward its end pose.
 */
void ClearCeremonyCam_SetLerpParams(f32 arg0, f32 arg1) {
    sCeremonyCamera->lerpGoal = arg0;
    sCeremonyCamera->lerpStep = arg1;
}

/**
 * Loads a DioramaAnimData keyframe into sCeremonyCamera: field of view (200.0 means keep the current one
 * and ease to 30), species-scaled distances capped at 500, and angles relative to the model's facing.
 */
void ClearCeremony_LoadCameraKeyframe(DioramaAnimData* arg0, CameraNode* arg1) {
    f32 sp34;
    f32 sp30;
    f32 sp2C;
    f32 sp28;

    sCeremonyCamera->lerpFraction = 0.0f;
    if (arg0->verticalFieldOfView != 200.0f) {
        arg1->perspective.verticalFieldOfView = sCeremonyCamera->verticalFieldOfView = arg0->verticalFieldOfView;
    } else {
        sCeremonyCamera->verticalFieldOfView = arg0->verticalFieldOfView;
    }

    sp34 = ClearCeremony_ScaleOrbitDistance(arg0->startingDistance, 500.0f);
    sp30 = ClearCeremony_ScaleOrbitDistance(arg0->distanceOffset, 500.0f);
    sp2C = ClearCeremony_ScaleOrbitDistance(arg0->eyeDistance, 500.0f);
    sp28 = ClearCeremony_ScaleOrbitDistance(arg0->distance, 500.0f);

    ClearCeremonyCam_SetTargetAngles(arg0->startingRotationX, ClearCeremony_GetReverseFacingRotationY() + arg0->startingRotationY,
                               arg0->rotationXOffset, ClearCeremony_GetReverseFacingRotationY() + arg0->rotationYOffset);
    ClearCeremonyCam_SetTargetDistance(sp34, sp30);

    ClearCeremonyCam_SetEyeAngles(arg0->eyeRotationX, ClearCeremony_GetReverseFacingRotationY() + arg0->eyeRotationY,
                                 arg0->rotationX, ClearCeremony_GetReverseFacingRotationY() + arg0->rotationY);
    ClearCeremonyCam_SetEyeDistance(sp2C, sp28);
    ClearCeremonyCam_SetLerpParams(arg0->targetZ, arg0->speed);
}

/**
 * Moves the eye and look-at a growing fraction (eased lerpFraction) of the way to their end positions.
 * Returns 1 once both are within 1 unit of their end positions.
 */
s32 ClearCeremony_UpdateCameraKeyframe(CameraNode* camera) {
    Vec3f sp5C;
    Vec3f sp50;
    Vec3f sp44;
    Vec3f sp38;
    f32 sp34;
    f32 sp30;
    s16 sp2E;
    s16 sp2C;

    if (sCeremonyCamera->verticalFieldOfView != 200.0f) {
        camera->perspective.verticalFieldOfView = sCeremonyCamera->verticalFieldOfView;
    } else {
        Math_EaseTowardF(&camera->perspective.verticalFieldOfView, 30.0f, 0.06f);
    }

    Math_EaseTowardF(&sCeremonyCamera->lerpFraction, sCeremonyCamera->lerpGoal, sCeremonyCamera->lerpStep);
    Camera_ComputeEyeFromAngles(&sCeremonyCamera->followPoint, &sp50, sCeremonyCamera->endEyeDistance, sCeremonyCamera->endEyeRotationX,
                                ClearCeremony_GetReverseFacingRotationY() + sCeremonyCamera->endEyeRotationY);

    if (sp50.y <= 10.0f) {
        sp50.y = 10.0f;
    }

    Vec3f_Copy(&sp44, &camera->view.eyePosition);
    Vec3f_CalculateDistanceAngles(&sp44, &sp50, &sp34, &sp2E, &sp2C);
    Camera_ComputeEyeFromAngles(&sp44, &camera->view.eyePosition, sCeremonyCamera->lerpFraction * sp34, sp2E, sp2C);
    Camera_ComputeEyeFromAngles(&sCeremonyCamera->followPoint, &sp38, sCeremonyCamera->endTargetDistance, sCeremonyCamera->endTargetRotationX,
                                ClearCeremony_GetReverseFacingRotationY() + sCeremonyCamera->endTargetRotationY);

    sp38.y += sSpeciesModelDimensions.unk_0C;

    Vec3f_Copy(&sp5C, &camera->view.targetPosition);
    Vec3f_CalculateDistanceAngles(&sp5C, &sp38, &sp30, &sp2E, &sp2C);
    Camera_ComputeEyeFromAngles(&sp5C, &camera->view.targetPosition, sCeremonyCamera->lerpFraction * sp30, sp2E, sp2C);

    if ((sp34 <= 1.0f) && (sp30 <= 1.0f)) {
        return 1;
    }
    return 0;
}
