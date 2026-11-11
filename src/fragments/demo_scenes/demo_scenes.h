#ifndef _FRAGMENT17_H_
#define _FRAGMENT17_H_

#include "global.h"

typedef struct DioramaSequence {
    /* 0x00 */ Vec3f focus;
    /* 0x0C */ s16 startRotationX;
    /* 0x0E */ s16 startRotationY;
    /* 0x10 */ char unk10[0x2];
    /* 0x12 */ s16 eyeRotationX;
    /* 0x14 */ s16 eyeRotationY;
    /* 0x18 */ f32 startDistance;
    /* 0x1C */ f32 eyeDistance;
    /* 0x20 */ s16 endRotationX;
    /* 0x22 */ s16 endRotationY;
    /* 0x24 */ char unk24[0x2];
    /* 0x26 */ s16 unk_26;
    /* 0x28 */ s16 unk_28;
    /* 0x2C */ f32 endDistance;
    /* 0x30 */ f32 advanceDistance;
    /* 0x34 */ f32 travelSpeed;
    /* 0x38 */ f32 unk_38;
    /* 0x3C */ f32 verticalFieldOfView;
    /* 0x40 */ char unk40[0x4];
    /* 0x44 */ s32 group;
    /* 0x48 */ s32 skipRequested;
    /* 0x4C */ s32 state;
    /* 0x50 */ s32 stateTimer;
    /* 0x54 */ char unk54[0x14];
    /* 0x68 */ f32 travelDistance;
    /* 0x6C */ f32 remainingDistance;
    /* 0x70 */ f32 unk_70;
    /* 0x74 */ f32 unk_74;
} DioramaSequence; // size >= 0x78

typedef struct DioramaScene {
    /* 0x00 */ CameraNode* camera;
    /* 0x04 */ DioramaSequence sequence;
} DioramaScene; // size = 0x7C

// Camera keyframe shared by the boot diorama and the clear ceremony. The field names do not
// match how the boot diorama uses them: distanceOffset is the end distance, distance is the
// remaining distance at which the group ends, and targetZ is the per-frame travel speed.
typedef struct DioramaAnimData {
    /* 0x00 */ s16 startingRotationX;
    /* 0x02 */ s16 startingRotationY;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 eyeRotationX;
    /* 0x08 */ s16 eyeRotationY;
    /* 0x0C */ f32 startingDistance;
    /* 0x10 */ f32 eyeDistance;
    /* 0x14 */ s16 rotationXOffset;
    /* 0x16 */ s16 rotationYOffset;
    /* 0x18 */ s16 unk_18;
    /* 0x1A */ s16 rotationX;
    /* 0x1C */ s16 rotationY;
    /* 0x20 */ f32 distanceOffset;
    /* 0x24 */ f32 distance;
    /* 0x28 */ f32 targetZ;
    /* 0x2C */ f32 speed;
    /* 0x30 */ f32 verticalFieldOfView;
} DioramaAnimData; // size = 0x34

typedef struct DioramaModelSlot {
    /* 0x000 */ unk_D_86002F30* model;
    /* 0x004 */ DisplayObject displayObject;
    /* 0x16C */ s32 isLoaded;
} DioramaModelSlot; // size = 0x170

typedef struct DioramaObjectDef {
    /* 0x00 */ Vec3f position;
    /* 0x0C */ u8 modelId;
    /* 0x0E */ s16 unk_0E;
    /* 0x10 */ u16 animationIndex;
    /* 0x12 */ char unk12[0x2];
} DioramaObjectDef; // size = 0x14

typedef struct SpriteColorParams {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ u16 unk_02;
    /* 0x04 */ u16 unk_04;
    /* 0x06 */ u16 unk_06;
    /* 0x08 */ u16 unk_08;
    /* 0x0A */ u16 unk_0A;
    /* 0x0C */ u16 unk_0C;
    /* 0x0E */ u16 unk_0E;
    /* 0x10 */ u16 unk_10;
    /* 0x12 */ u16 unk_12;
    /* 0x14 */ u16 unk_14;
    /* 0x16 */ u16 unk_16;
} SpriteColorParams; // size = 0x18

typedef struct GfxTileParams {
    /* 0x00 */ SpriteColorParams unk_00;
    /* 0x18 */ SpriteColorParams unk_18;
} GfxTileParams; // size = 0x30

typedef struct ClearCeremonyModel {
    /* 0x000 */ unk_D_86002F30* model;
    /* 0x004 */ DisplayObject displayObject;
    /* 0x16C */ s32 sequenceStep;
    /* 0x170 */ s32 idleCueActive;
    /* 0x174 */ s32 modelId;
    /* 0x178 */ s32 frameTimer;
    /* 0x17C */ s32 partySlot;
    /* 0x180 */ s16 arenaTint;
} ClearCeremonyModel; // size = 0x184

typedef struct ClearCeremonyTextPanel {
    /* 0x00 */ char text[0x80];
    /* 0x80 */ s32 texture;
    /* 0x84 */ s32 hasSettled;
    /* 0x88 */ s32 colorStage;
    /* 0x8C */ s32 unk_8C;
    /* 0x90 */ s32 drawState;
    /* 0x94 */ char unk94[0x4];
    /* 0x98 */ f32 positionX;
    /* 0x9C */ s16 positionY;
    /* 0x9E */ s16 width;
    /* 0xA0 */ s16 height;
    /* 0xA4 */ f32 unk_A4;
    /* 0xA8 */ f32 lastVelocity;
    /* 0xAC */ f32 velocity;
    /* 0xB0 */ SpriteColorParams colors;
} ClearCeremonyTextPanel; // size = 0xC8

typedef struct ClearCeremonyCamera {
    /* 0x00 */ CameraNode* camera;
    /* 0x04 */ Vec3f followPoint;
    /* 0x10 */ s16 targetRotationX;
    /* 0x12 */ s16 targetRotationY;
    /* 0x14 */ char unk14[0x2];
    /* 0x16 */ s16 eyeRotationX;
    /* 0x18 */ s16 eyeRotationY;
    /* 0x1C */ f32 targetDistance;
    /* 0x20 */ f32 eyeDistance;
    /* 0x24 */ s16 endTargetRotationX;
    /* 0x26 */ s16 endTargetRotationY;
    /* 0x28 */ char unk28[0x2];
    /* 0x2A */ s16 endEyeRotationX;
    /* 0x2C */ s16 endEyeRotationY;
    /* 0x30 */ f32 endTargetDistance;
    /* 0x34 */ f32 endEyeDistance;
    /* 0x38 */ f32 lerpGoal;
    /* 0x3C */ f32 lerpStep;
    /* 0x40 */ f32 verticalFieldOfView;
    /* 0x44 */ f32 lerpFraction;
    /* 0x48 */ char unk48[0x34];
} ClearCeremonyCamera; // size = 0x7C

typedef struct DioramaPresetData {
    /* 0x00 */ Vec3f cameraFocus;
    /* 0x0C */ s16 lightRotationX;
    /* 0x0E */ s16 lightRotationY;
    /* 0x10 */ u16 lightR;
    /* 0x12 */ u16 lightG;
    /* 0x14 */ u16 lightB;
    /* 0x16 */ u16 lightA;
    /* 0x18 */ s16 lightRotationXStep;
    /* 0x1A */ s16 lightRotationYStep;
} DioramaPresetData; // size = 0x1C

extern DioramaAnimData sClearCeremonyCameraKeyframes[];
extern s32 sDioramaSequenceFinished;
extern char** sControllerErrorText;
extern ClearCeremonyModel* sCeremonyModel;
extern ClearCeremonyCamera* sCeremonyCamera;
extern unk_70D3A0 sSpeciesModelDimensions;

s32 Stage_DispatchDemoScene(UNUSED s32 arg0, UNUSED s32 arg1);

s32 Diorama_LightCallback(s32 arg0, GraphNode* arg1);
s32 Diorama_CloudLayerCallback(s32 arg0, GraphNode* arg1);
s32 Diorama_WaterLayerCallback(s32 arg0, GraphNode* arg1);
s32 Diorama_LayerSwitchCallback(s32 arg0, GraphNode* arg1);
void Diorama_ClearModelSlots(void);
void Diorama_FreeModelSlots(DioramaModelSlot* arg0);
void Diorama_PushModelPool(void);
void Diorama_InitModelSlot(DioramaModelSlot* arg0, DioramaObjectDef* arg1);
void Diorama_SetModelSlotPosition(DioramaModelSlot* arg0, DioramaObjectDef* arg1);
void Diorama_SetLightFromGroup(s32 arg0);
void Diorama_LoadModelGroup(s32 arg0);
void Diorama_SwitchModelGroup(s32 arg0);
void Diorama_SetGroupCamera(s32 arg0);
void func_86B00680(void);
void Diorama_LoopIdleAnimations(void);
void Diorama_DrawSkipFadeOverlay(void);
void Diorama_DrawSkyGradient(void);
s32 Diorama_SlideModelsForward(s32 arg0);
void Diorama_BeginGroupTransition(void);
void Diorama_FinishSequence(void);
void Diorama_UpdateModelVisibility(s32 arg0);
void Diorama_ReadControllerInput(void);
void Diorama_InitSceneState(void);
s32 Diorama_UpdateStateMachine(void);
void func_86B01004(void);
void Diorama_RenderFrame(void);
void Diorama_RunLoop(void);
void Diorama_RenderFinalFrame(void);
void Diorama_InitScene(void);
s32 Stage_ShowBootDiorama(void);

u16 MathUtil_RandomU16Range(s32);
Vec3f* Vec3f_Copy(Vec3f*, Vec3f*);
void Math_EaseTowardF(f32*, f32, f32);

u16 Diorama_IsModelInCameraCone(CameraNode* camera, DisplayObject* arg1, s16 arg2, s16 arg3, f32 arg4, f32 arg5);
void DioramaCamKeyframe_SetAngles(DioramaSequence* arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4);
void DioramaCamKeyframe_SetDistance(DioramaSequence* arg0, f32 arg1, f32 arg2);
void DioramaCamKeyframe_SetAtAngles(DioramaSequence* arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4);
void DioramaCamKeyframe_SetAtDistance(DioramaSequence* arg0, f32 arg1, f32 arg2);
void DioramaCamKeyframe_SetTransitionSpeed(DioramaSequence* arg0, f32 arg1, f32 arg2);
void Diorama_LoadCameraKeyframe(DioramaAnimData* arg0, DioramaScene* arg1);
void Diorama_SetCameraToKeyframeStart(DioramaSequence* arg0, DioramaScene* arg1);
s32 Diorama_UpdateCameraKeyframe(DioramaSequence* arg0, DioramaScene* arg1);

void Diorama_DrawTitleBackground(u8*);
void Diorama_DrawFullscreenTexture(u8*, s32, s32, s32, s32, u8, u8);
void Diorama_DrawNoControllerWarning(void);

s32 Completion_LightAngleCallbackA(s32 arg0, GraphNode* arg1);
s32 Completion_LightAngleCallbackB(s32 arg0, GraphNode* arg1);
void Completion_SelectGiftPokemon(void);
void Completion_LoadModels(void);
void Completion_ResetSceneStep(void);
void Completion_ResetDialogToIdle(void);
void Completion_PositionGiftModel(void);
void Completion_InitPresentBanner(void);
void Completion_InitCamera(void);
s32 Completion_RevealNextDialogChar(void);
void Completion_DrawReceivePrompt(s16 arg0, s16 arg1);
void Completion_DrawRevealedDialogText(s16 arg0, s16 arg1);
void Completion_ResetDialogText(void);
void Completion_DrawDialogBox(void);
void Completion_UpdateGiftDialog(void);
void Completion_UpdateGiftDialogOakHolding(void);
void Completion_DrawPresentBanner(void);
s32 Completion_AnimatePresentBannerColor(void);
Gfx* Completion_DrawAButtonPrompt(void);
s32 Completion_SlidePresentBannerIn(void);
s32 Completion_SlidePresentBannerOut(void);
s32 Completion_MoveCameraToFront(void);
void Completion_StartGiftDialog(void);
void Completion_AbortDialogToIdle(void);
void Completion_HandleAcceptDeclineInput(void);
void Completion_HandleContinuePrompt(void);
void Completion_UpdateIdleCueTrack(void);
void Completion_LoopGiftIdleAnimation(void);
void Completion_StepAnchorRewind(void);
void Completion_ReadControllerInput(void);
s32 Completion_UpdateSceneSteps(void);
void Completion_DrawDialog(void);
void Completion_RenderFrame(UNUSED s32 arg0);
void Completion_FadeIn(void);
void Completion_RunLoop(void);
void Completion_FadeOutAndSave(void);
void Completion_InitScene(void);
s32 Stage_ShowCompletionScene(void);

void UI_DrawSplitGradientPanel(u8* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, u8 width, u8 height, u8 uls, u8 ult,
                               u8 lrs, u8 lrt, u8 argB, u8 argC, GfxTileParams* argD, s32 argE);
void UI_DrawAlphaQuad(s32, s32, s32, s32, s32);

s32 ClearCeremony_LightAngleCallbackA(s32 arg0, GraphNode* arg1);
s32 ClearCeremony_LightAngleCallbackB(s32 arg0, GraphNode* arg1);
s32 ClearCeremony_ArenaTintCallback(s32 arg0, GraphNode* arg1);
void ClearCeremony_SetArenaTint(u8 arg0, u8 arg1, u8 arg2);
void ClearCeremony_LoadSpeciesBattleData(void);
void ClearCeremony_LoadPartyModel(void);
void ClearCeremony_ReplacePartyModel(void);
void ClearCeremony_SetupOrbitCamera(void);
void ClearCeremony_LoadTrophyModel(void);
void ClearCeremony_SetupTrophyCamera(void);
s32 ClearCeremony_StepRotationOvershoot(DisplayObject* arg0, s16 arg1, s16 arg2);
s32 ClearCeremony_StepRotationToTarget(DisplayObject* arg0, s16 arg1, s16 arg2);
s32 ClearCeremony_UpdateTrophyDescent(void);
void ClearCeremony_EmitTrophyParticles(void);
void ClearCeremony_DimArenaTint(void);
void ClearCeremony_UpdateCryEventTrack(void);
void ClearCeremony_SetupTextPanel(ClearCeremonyTextPanel* arg0, u8* arg1, f32 arg2, s16 arg3, s16 arg4, s16 arg5);
void ClearCeremony_ConfigureTextPanels(void);
void ClearCeremony_StartPartyShowcase(void);
void ClearCeremony_BeginOrbitCamera(void);
void ClearCeremony_StartTrophyShowcase(void);
void ClearCeremony_BeginTrophyCamera(void);
void ClearCeremony_HideTextPanels(void);
void ClearCeremony_SelectPartyRoster(void);
void ClearCeremony_ReadControllerInput(void);
s32 ClearCeremony_UpdateSequence(void);
s32 ClearCeremony_StepPanelBounce(ClearCeremonyTextPanel* arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5);
void ClearCeremony_DrawTextPanels(void);
void ClearCeremony_RenderFrame(void);
void ClearCeremony_RenderBlackFrame(void);
void ClearCeremony_FadeInSequence(void);
void ClearCeremony_RunLoop(void);
void ClearCeremony_FadeOutSequence(void);
void ClearCeremony_LoadScene(void);
s32 Stage_ShowClearCeremony(void);

s16 ClearCeremony_GetReverseFacingRotationY(void);
s32 Util_ByteInArray(u8 arg0, u8* arg1, s32 arg2);
void ClearCeremony_ComputeFollowPoint(ClearCeremonyModel* arg0, Vec3f* arg1);
void ClearCeremony_UpdateOrbitCamera(void);
f32 ClearCeremony_ScaleOrbitDistance(f32 arg0, f32 arg1);
void ClearCeremonyCam_SetTargetAngles(s16 arg0, s16 arg1, s16 arg2, s16 arg3);
void ClearCeremonyCam_SetTargetDistance(f32 arg0, f32 arg1);
void ClearCeremonyCam_SetEyeAngles(s16 arg0, s16 arg1, s16 arg2, s16 arg3);
void ClearCeremonyCam_SetEyeDistance(f32 arg0, f32 arg1);
void ClearCeremonyCam_SetLerpParams(f32 arg0, f32 arg1);
void ClearCeremony_LoadCameraKeyframe(DioramaAnimData* arg0, CameraNode* arg1);
s32 ClearCeremony_UpdateCameraKeyframe(CameraNode* camera);

void WipeTransition_Start(s32 arg0, s32 arg1);
s32 WipeTransition_GetState(void);
void WipeTransition_Update(void);
Gfx* WipeTransition_BuildBrickDisplayList(Gfx* gfx, u32 arg1);
void WipeTransition_Draw(void);
void UI_DrawClippedTextureRect(s16 arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4, s16 arg5, s16 arg6, s16 arg7);
void UI_DrawTiledTexture(s32 arg0, s32 arg1, s32 arg2, s32 arg3, u8* arg4, UNUSED s32 arg5);
void UI_DrawGradientTexture(u8* arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4, SpriteColorParams* arg5);
void UI_DrawTextureRgba16(u8* arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4);

void MewtwoIntro_DrawOverlay(void);
void MewtwoIntro_InitCamera(void);
void MewtwoIntro_ReadControllerInput(void);
void MewtwoIntro_DrawBackgroundTiles(u8* arg0);
void MewtwoIntro_RenderFrame(void);
s32 MewtwoIntro_UpdateFade(void);
void MewtwoIntro_RunLoop(void);
void MewtwoIntro_InitScene(void);
s32 Stage_ShowMewtwoIntro(void);

void FirstClearBonus_InitCamera(void);
void FirstClearBonus_InitState(void);
void FirstClearBonus_EmptyFunc(void);
void FirstClearBonus_DrawDialogFrame(s16 arg0, s16 arg1, s16 arg2, s16 arg3, u8 arg4, u8 arg5, u8 arg6, u8 arg7);
void FirstClearBonus_CloseDialog(void);
void FirstClearBonus_StartDialog(void);
s32 FirstClearBonus_RevealNextChar(void);
void FirstClearBonus_DrawDialogBox(void);
void FirstClearBonus_DrawDialogText(void);
void FirstClearBonus_UpdateDialogSequence(void);
void FirstClearBonus_ReadControllerInput(void);
void FirstClearBonus_NoopStub2(void);
void FirstClearBonus_DrawBackgroundTiles(u8* arg0);
void FirstClearBonus_RenderFrame(void);
s32 FirstClearBonus_UpdateScene(void);
void FirstClearBonus_RunLoop(void);
void FirstClearBonus_InitScene(void);
s32 Stage_ShowFirstClearBonusScene(void);

#endif // _FRAGMENT17_H_
