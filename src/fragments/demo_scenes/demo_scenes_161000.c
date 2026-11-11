#include "demo_scenes.h"
#include "src/matrix.h"

/**
 * Returns 1 if the display object is inside a triangle on the xz plane in front of the camera.
 * The triangle's apex sits arg5 units behind the eye; its sides spread arg2 to the left and arg3
 * to the right of the viewing direction and reach arg4 units forward.
 */
u16 Diorama_IsModelInCameraCone(CameraNode* camera, DisplayObject* arg1, s16 arg2, s16 arg3, f32 arg4, f32 arg5) {
    UNUSED s32 pad[6];
    f32 sp6C;
    f32 sp68;
    f32 sp64;
    f32 sp60;
    f32 sp5C;
    DisplayObject* arg11;
    f32 sp54;
    f32 sp50;
    UNUSED s32 pad3;
    f32 sp48;
    s16 sp46;
    s16 sp44;
    f32 sp40;
    UNUSED s32 pad2;
    f32 sp38;
    s32 sp34 = 0;

    Vec3f_CalculateDistanceAngles(&camera->view.targetPosition, &camera->view.eyePosition, &sp48, &sp46, &sp44);
    sp40 = (COSS(sp44 + 0x8000) * arg5) + camera->view.eyePosition.z;
    sp38 = (SINS(sp44 + 0x8000) * arg5) + camera->view.eyePosition.x;

    sp54 = (COSS((sp44 - arg3) + 0x8000) * arg4) + sp40;
    sp60 = (SINS((sp44 - arg3) + 0x8000) * arg4) + sp38;

    sp50 = (COSS((sp44 + arg2) + 0x8000) * arg4) + sp40;
    sp5C = (SINS((sp44 + arg2) + 0x8000) * arg4) + sp38;

    sp6C = ((sp54 - arg1->position.z) * (sp5C - arg1->position.x)) -
           ((sp50 - arg1->position.z) * (sp60 - arg1->position.x));

    arg11 = arg1;

    sp68 = ((sp50 - arg11->position.z) * (sp38 - arg11->position.x)) -
           ((sp40 - arg11->position.z) * (sp5C - arg11->position.x));
    sp64 = ((sp40 - arg11->position.z) * (sp60 - arg11->position.x)) -
           ((sp54 - arg11->position.z) * (sp38 - arg11->position.x));

    if (((sp6C >= 0.0f) && (sp68 >= 0.0f) && (sp64 >= 0.0f)) || ((sp6C <= 0.0f) && (sp68 <= 0.0f) && (sp64 <= 0.0f))) {
        sp34 = 1;
    }

    return sp34;
}

/**
 * Sets the start and end rotationX/Y of the camera target path around the focus point.
 */
void DioramaCamKeyframe_SetAngles(DioramaSequence* arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4) {
    arg0->startRotationX = arg1;
    arg0->startRotationY = arg2;
    arg0->endRotationX = arg3;
    arg0->endRotationY = arg4;
}

/**
 * Sets the start and end distance of the camera target from the focus point.
 */
void DioramaCamKeyframe_SetDistance(DioramaSequence* arg0, f32 arg1, f32 arg2) {
    arg0->startDistance = arg1;
    arg0->endDistance = arg2;
}

/**
 * Sets the eye rotationX/Y around the camera target, plus two unused angles.
 */
void DioramaCamKeyframe_SetAtAngles(DioramaSequence* arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4) {
    arg0->eyeRotationX = arg1;
    arg0->eyeRotationY = arg2;
    arg0->unk_26 = arg3;
    arg0->unk_28 = arg4;
}

/**
 * Sets the eye distance from the camera target and the remaining distance at which the group
 * ends.
 */
void DioramaCamKeyframe_SetAtDistance(DioramaSequence* arg0, f32 arg1, f32 arg2) {
    arg0->eyeDistance = arg1;
    arg0->advanceDistance = arg2;
}

/**
 * Sets how far the camera target moves per frame, plus one unused value.
 */
void DioramaCamKeyframe_SetTransitionSpeed(DioramaSequence* arg0, f32 arg1, f32 arg2) {
    arg0->travelSpeed = arg1;
    arg0->unk_38 = arg2;
}

/**
 * Copies a DioramaAnimData keyframe into the sequence state and resets the distance traveled.
 * A vertical field of view of 255 keeps the current value and eases it toward 30.
 */
void Diorama_LoadCameraKeyframe(DioramaAnimData* arg0, DioramaScene* arg1) {
    DioramaSequence* ptr = &arg1->sequence;
    CameraNode* ptr2 = arg1->camera;
    f32 sp34;
    f32 sp30;
    f32 sp2C;
    f32 sp28;

    ptr->unk_74 = 0.0f;
    ptr->travelDistance = 0;

    if (arg0->verticalFieldOfView != 255.0f) {
        ptr2->perspective.verticalFieldOfView = ptr->verticalFieldOfView = arg0->verticalFieldOfView;
    } else {
        ptr->verticalFieldOfView = arg0->verticalFieldOfView;
    }

    sp34 = arg0->startingDistance;
    sp30 = arg0->distanceOffset;
    sp2C = arg0->eyeDistance;
    sp28 = arg0->distance;

    DioramaCamKeyframe_SetAngles(ptr, arg0->startingRotationX, arg0->startingRotationY, arg0->rotationXOffset,
                                 arg0->rotationYOffset);
    DioramaCamKeyframe_SetDistance(ptr, sp34, sp30);
    DioramaCamKeyframe_SetAtAngles(ptr, arg0->eyeRotationX, arg0->eyeRotationY, arg0->rotationX, arg0->rotationY);
    DioramaCamKeyframe_SetAtDistance(ptr, sp2C, sp28);
    DioramaCamKeyframe_SetTransitionSpeed(ptr, arg0->targetZ, arg0->speed);
}

/**
 * Places the camera target at the keyframe start position and the eye relative to the target.
 */
void Diorama_SetCameraToKeyframeStart(DioramaSequence* arg0, DioramaScene* arg1) {
    CameraNode* sp2C = arg1->camera;

    Camera_ComputeEyeFromAngles(&arg0->focus, &sp2C->view.targetPosition, arg0->startDistance, arg0->startRotationX, arg0->startRotationY);
    Camera_ComputeEyeFromAngles(&sp2C->view.targetPosition, &sp2C->view.eyePosition, arg0->eyeDistance, arg0->eyeRotationX,
                                arg0->eyeRotationY);
}

/**
 * Moves the camera target from the keyframe start position toward its end position by
 * travelSpeed per frame and keeps the eye at a fixed offset from the target. Returns 1 when the
 * target is within 5 units of the end.
 */
s32 Diorama_UpdateCameraKeyframe(DioramaSequence* arg0, DioramaScene* arg1) {
    CameraNode* temp_s2 = arg1->camera;
    Vec3f sp58;
    Vec3f sp4C;
    UNUSED s32 pad[3];
    f32 sp3C;
    s16 sp3A;
    s16 sp38;

    if (arg0->verticalFieldOfView != 255.0f) {
        temp_s2->perspective.verticalFieldOfView = arg0->verticalFieldOfView;
    } else {
        Math_EaseTowardF(&temp_s2->perspective.verticalFieldOfView, 30.0f, 0.06f);
    }

    Camera_ComputeEyeFromAngles(&arg0->focus, &sp58, arg0->startDistance, arg0->startRotationX, arg0->startRotationY);
    Camera_ComputeEyeFromAngles(&arg0->focus, &sp4C, arg0->endDistance, arg0->endRotationX, arg0->endRotationY);
    Vec3f_CalculateDistanceAngles(&sp58, &sp4C, &sp3C, &sp3A, &sp38);

    arg1->sequence.travelDistance = Math_StepToF(arg1->sequence.travelDistance, sp3C, arg0->travelSpeed, arg0->travelSpeed);

    Camera_ComputeEyeFromAngles(&sp58, &temp_s2->view.targetPosition, arg1->sequence.travelDistance, sp3A, sp38);
    Camera_ComputeEyeFromAngles(&temp_s2->view.targetPosition, &temp_s2->view.eyePosition, arg0->eyeDistance, arg0->eyeRotationX,
                                arg0->eyeRotationY);

    arg1->sequence.remainingDistance = sp3C - arg1->sequence.travelDistance;
    if (arg1->sequence.remainingDistance <= 5.0f) {
        return 1;
    }

    return 0;
}
