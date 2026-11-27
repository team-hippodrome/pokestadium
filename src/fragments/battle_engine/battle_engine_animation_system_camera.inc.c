void BattleAnim_Initialize(void) {
    BattleAnim_InitParticleSystem();
    BattleAnim_Reset();
}

void BattleAnim_Reset(void) {
    s32 i;

    gBattleAnimCurrentOwner = NULL;
    sD_843902BC = 0;
    sParticleRenderContext.scale.x = 1.0f;
    sParticleRenderContext.scale.y = 1.0f;
    sParticleRenderContext.scale.z = 1.0f;
    D_843902F4 = 0;
    sD_84392040.unk_0C = 1.0f;
    sD_84392040.unk_10 = 1.0f;
    sBattleAnimEffectCallbackIndex = 0;

    for (i = 0; i < 8; i++) {
        gBattleAnimEffectUpdateFuncs[i] = NULL;
        gBattleAnimEffectDrawFuncs[i] = NULL;
    }

    gSavedPrimColorR = 0xFF;
    gSavedPrimColorG = 0xFF;
    gSavedPrimColorB = 0xFF;
    gPrimColorA = 0xFF;
    gPrimColorR = 0xFF;
    gPrimColorG = 0xFF;
    gPrimColorB = 0xFF;
    gEnvColorR = 0xFF;
    gEnvColorG = 0xFF;
    gEnvColorB = 0x64;
    gFogColorR = 0;
    gFogColorG = 0;
    gFogColorB = 0;
    sD_84390344 = 1.0f;

    Vec3f_SetComponentsDuplicate(&sD_84390360, 0.0f, 0.0f, 0.0f);
    Vec3s_SetComponents(&sD_8439036C, 0, 0, 0);
    Vec3s_SetComponents(&sD_84390374, 0, 0, 0);
    Vec3f_SetComponentsDuplicate(&D_84390348, 0.0f, 0.0f, 0.0f);
    Vec3s_SetComponents(&sD_84390354, 0, 0, 0);
    BattleAnim_ResetAssetTable();
    BattleAnim_ResetDisplayContexts();
    sD_843903B0 = 0;
    sD_843903B4 = 0;
    BattleAnim_EnableEffectSpawning();
}

void BattleAnim_GetPointAlongCameraRay(Vec3f* arg0, f32 arg1) {
    arg0->x = sParticleCameraState->eyePosition.x + (arg1 * sParticleCameraState->lookDirection.x);
    arg0->y = sParticleCameraState->eyePosition.y + (arg1 * sParticleCameraState->lookDirection.y);
    arg0->z = sParticleCameraState->eyePosition.z + (arg1 * sParticleCameraState->lookDirection.z);
}

void BattleAnim_GetCameraEyeAngles(s16* arg0, s16* arg1) {
    CameraNode* temp_v0 = BattleAnim_GetCameraContext();
    f32 sp20 = temp_v0->view.eyePosition.x - temp_v0->view.targetPosition.x;
    f32 sp1C = temp_v0->view.eyePosition.y - temp_v0->view.targetPosition.y;
    f32 sp18 = temp_v0->view.eyePosition.z - temp_v0->view.targetPosition.z;

    *arg0 = MathUtil_Atan2s(sqrtf(SQ(sp20) + SQ(sp18)), sp1C);
    *arg1 = MathUtil_Atan2s(sp18, sp20);
}

void BattleAnim_GetCameraEyeYaw(s16* arg0) {
    CameraNode* temp_v0 = BattleAnim_GetCameraContext();
    f32 x = temp_v0->view.eyePosition.x - temp_v0->view.targetPosition.x;
    f32 z = temp_v0->view.eyePosition.z - temp_v0->view.targetPosition.z;

    *arg0 = MathUtil_Atan2s(z, x);
}

void BattleAnim_GetCameraLookAngles(s16* arg0, s16* arg1) {
    CameraNode* temp_v0 = BattleAnim_GetCameraContext();
    f32 sp20 = temp_v0->view.targetPosition.x - temp_v0->view.eyePosition.x;
    f32 sp1C = temp_v0->view.targetPosition.y - temp_v0->view.eyePosition.y;
    f32 sp18 = temp_v0->view.targetPosition.z - temp_v0->view.eyePosition.z;

    *arg0 = MathUtil_Atan2s(sqrtf(SQ(sp20) + SQ(sp18)), sp1C);
    *arg1 = MathUtil_Atan2s(sp18, sp20);
}

void BattleAnim_GetCameraLookYaw(s16* arg0) {
    CameraNode* temp_v0 = BattleAnim_GetCameraContext();
    f32 x = temp_v0->view.targetPosition.x - temp_v0->view.eyePosition.x;
    f32 z = temp_v0->view.targetPosition.z - temp_v0->view.eyePosition.z;

    *arg0 = MathUtil_Atan2s(z, x);
}

void BattleAnim_GetCameraStoredAngles76(s16* arg0, s16* arg1) {
    *arg0 = sParticleCameraState->unk_76;
    *arg1 = sParticleCameraState->unk_78;
}

void BattleAnim_GetCameraStoredYaw78(s16* arg0) {
    *arg0 = sParticleCameraState->unk_78;
}

void BattleAnim_GetCameraStoredAngles70(s16* arg0, s16* arg1) {
    *arg0 = sParticleCameraState->unk_70;
    *arg1 = sParticleCameraState->unk_72;
}

void BattleAnim_GetCameraStoredYaw72(s16* arg0) {
    *arg0 = sParticleCameraState->unk_72;
}
