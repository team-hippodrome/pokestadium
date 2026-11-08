#include "demo_scenes.h"
#include "src/geo_node.h"
#include "src/geo_render.h"
#include "src/model_animation_events.h"
#include "src/geo_layout.h"
#include "src/display_object_textures.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/game_state.h"
#include "src/jpeg_stream.h"
#include "src/audio_loop_point.h"
#include "src/audio_commands_category2.h"
#include "src/gfx_buffer.h"
#include "src/gfx_rect.h"
#include "src/matrix.h"
#include "src/controller.h"
#include "src/fragments/minigame_actor_framework/minigame_actor_framework.h"
#include "src/fragments/particle_math/particle_math.h"
#include "src/memory.h"
#include "src/stage_loader.h"
#include "src/util.h"
#include "src/gb_save.h"

extern u8 D_3000180[];
extern u8 D_3000780[];
extern u8 D_3001220[];
extern u8 D_3001B80[];
extern u8 D_30024E0[];
extern u8 D_30032A0[];
extern u8 D_3004060[];
extern u8 D_3004E20[];
extern u8 D_3005780[];
extern u8 D_30060E0[];
extern u8 D_3006B80[];
extern u8 D_300C6D0[];

s32 ClearCeremony_LightAngleCallbackA(s32, GraphNode*);
s32 ClearCeremony_LightAngleCallbackB(s32, GraphNode*);
s32 ClearCeremony_ArenaTintCallback(s32, GraphNode*);

typedef struct TrophyModelInfo {
    /* 0x00 */ u8 modelId;
    /* 0x04 */ f32 cameraTargetHeight;
    /* 0x08 */ f32 particleHeight;
} TrophyModelInfo; // size = 0xC

// Caption color stages for ClearCeremony_DrawTextPanels: prim RGB, env RGB, then per-channel step sizes.
// Stage 0 #FFE100 on #000000; stages 1 and 3 #FFFFFF on #FFFF64; stage 2 #FFFF9B on #9B9B00.
// The color cycle reads index colorStage + 1 up to 4; index 4 lands in sSettledPanelColors, which is
// stored immediately after this array, so the two behave as one five-entry table.
static SpriteColorParams sClearPanelColorStages[4] = {
    {
        0x00FF,
        0x00E1,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
    },
    {
        0x00FF,
        0x00FF,
        0x00FF,
        0x00FF,
        0x00FF,
        0x0064,
        0x00FF,
        0x0007,
        0x0040,
        0x0040,
        0x0040,
        0x0019,
    },
    {
        0x00FF,
        0x00FF,
        0x009B,
        0x009B,
        0x009B,
        0x0000,
        0x00FF,
        0x00FF,
        0x0032,
        0x0032,
        0x0032,
        0x0032,
    },
    {
        0x00FF,
        0x00FF,
        0x00FF,
        0x00FF,
        0x00FF,
        0x0064,
        0x00FF,
        0x00FF,
        0x0080,
        0x0080,
        0x0080,
        0x0032,
    },
};
// Same layout as SpriteColorParams (big-endian u16s): prim #FFE100, env #000000. Used as the settled
// caption colors and as color stage 4 (see sClearPanelColorStages).
static u8 sSettledPanelColors[] = {
    0, 0xFF, 0, 0xE1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0, 3, 0, 0x11, 0, 0x11, 0, 0x11, 0, 7,
};

// Trophy models indexed by sTrophyIndex: 0..3 = ball tier (modeSelection) for ruleCategory 3 and 6,
// 4 = ruleCategory 1, 2 (and Petit Cup in US 1.2), 5 = Pika Cup (and Petit Cup outside US 1.2).
// US 1.2 gives entry 5 its own model (0xA6); other versions repeat 0xA5. Entries 6..19 are unused filler.
static TrophyModelInfo sTrophyModels[20] = {
    { 0xA7, 47.5f, 30.0f }, { 0xA8, 47.5f, 30.0f }, { 0xA9, 47.5f, 30.0f }, { 0xAA, 47.5f, 30.0f },
#if defined(VERSION_US_REV2)
    { 0xA5, 66.5f, 85.0f }, { 0xA6, 50.0f, 55.0f }, { 0xA7, 0.0f, 0.0f },   { 0xA7, 0.0f, 0.0f },
#else
    { 0xA5, 66.5f, 85.0f }, { 0xA5, 66.5f, 85.0f }, { 0xA7, 0.0f, 0.0f },   { 0xA7, 0.0f, 0.0f },
#endif
    { 0xA7, 0.0f, 0.0f },   { 0xA7, 0.0f, 0.0f },   { 0xA7, 0.0f, 0.0f },   { 0xA7, 0.0f, 0.0f },
    { 0xA7, 0.0f, 0.0f },   { 0xA7, 0.0f, 0.0f },   { 0xA7, 0.0f, 0.0f },   { 0xA7, 0.0f, 0.0f },
    { 0xA7, 0.0f, 0.0f },   { 0xA7, 0.0f, 0.0f },   { 0xA7, 0.0f, 0.0f },   { 0xA7, 0.0f, 0.0f },
};

// stadium_models archive file per ruleCategory; Gym Leader Castle (7) adds modeSelection to 0x11.
static u8 sArenaArchiveIndexByRule[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x11, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
};

static unk_D_8690A610 sArenaFogNode;
static DisplayNodeHeader sArenaGeoNodeFile0;
static DisplayNodeHeader sArenaGeoNodeFile1;
static DisplayNodeHeader sArenaGeoNodeFile3;
static u32 sArenaFillColor;
static Color_RGBA8_u32 sArenaTint;
static u8 sParticleSoundPlayed; // Set once sound 0x0110000F has played for the particle bursts
static s16 sArenaArchiveIndex;
static s16 sTrophyIndex;
static s16 sTrophySpinPhase;
static f32 sTrophyDescentRate;
static void* sArenaFile2Data; // Arena archive file 2; also read into sArenaFillColor, otherwise unused
static ClearCeremonyTextPanel* sNicknamePanel;
static ClearCeremonyTextPanel* sClearTextPanel;
static ClearCeremonyTextPanel* sCupNamePanel;
static ClearCeremonyTextPanel* sBallTierPanel;
static ClearCeremonyTextPanel* sRound2BadgePanel;
static s16 sOrbitSpeed;
static s16 sOrbitRotationX;
static s16 sOrbitRotationY;
static f32 sOrbitDistance;
static s32 sFrameCounter;
static s32 sCeremonyVariant; // 0 = party then trophy, 1 = Master Ball tier party showcase, 2 = trophy only
ClearCeremonyModel* sCeremonyModel;
ClearCeremonyCamera* sCeremonyCamera;
static u8 sSpeciesBattleCues[0xBE0]; // 0xBC0 bytes are loaded; byte 0xA51 selects an event track
unk_70D3A0 sSpeciesModelDimensions;
static unk_D_70B10 sSpeciesSwayLayout;
static TeamRoster* sShowcaseRoster;
static BattleSessionTeams* sShowcaseSide;

static u32 sCeremonyGeoLayout[] = {
    0x0C00FFFF,  0x05000000,
    0x0B00001E,  0x00000000,
    0x014000F0,  0x0000000F,
    0x00000000,  0x00000000,
    0x05000000,  0x0D000000,
    0x05000000,  0x07000000,
    &sArenaFogNode, 0x14000000,
    0x002D0019,  0xFFFFFF28,
    0x08000000,  ClearCeremony_LightAngleCallbackA,
    0x00000000,  0x14000000,
    0x002D0019,  0x80808028,
    0x08000000,  ClearCeremony_LightAngleCallbackB,
    0x00000000,  0x16646464,
    0x0F000002,  0x05000000,
    0x1F00FFFF,  0x00000000,
    0x00000000,  0x00000000,
    0x00640064,  0x00640000,
    0x08000000,  ClearCeremony_ArenaTintCallback,
    0x00000000,  0x05000000,
    0x07000000,  &sArenaGeoNodeFile0,
    0x06000000,  0x06000000,
    0x0F000003,  0x05000000,
    0x1F00FFFF,  0x00000000,
    0x00000000,  0x00000000,
    0x00640064,  0x00640000,
    0x08000000,  ClearCeremony_ArenaTintCallback,
    0x00000000,  0x05000000,
    0x07000000,  &sArenaGeoNodeFile1,
    0x06000000,  0x06000000,
    0x0F000002,  0x05000000,
    0x1F00FFFF,  0x00000000,
    0x00000000,  0x00000000,
    0x00640064,  0x00640000,
    0x08000000,  ClearCeremony_ArenaTintCallback,
    0x00000000,  0x05000000,
    0x07000000,  &sArenaGeoNodeFile3,
    0x06000000,  0x06000000,
    0x0F000003,  0x05000000,
    0x0A000000,  &D_800AC840,
    0x06000000,  0x0F000002,
    0x05000000,  0x0A000000,
    &D_800AC858, 0x06000000,
    0x09000000,  0x06000000,
    0x06000000,  0x06000000,
    0x01000000,
};

/**
 * Geo light-node callback (sCeremonyGeoLayout). On the render pass (arg0 == 2) it points the light
 * relative to the current camera: the eye's angles around the look-at target, offset by +0x2000 / -0x2000.
 */
s32 ClearCeremony_LightAngleCallbackA(s32 arg0, GraphNode* arg1) {
    unk_D_86002F34_alt18* ptr = (unk_D_86002F34_alt18*)arg1;
    f32 sp28;
    s16 sp26;
    s16 sp24;

    if (arg0 == 2) {
        Vec3f_CalculateDistanceAngles(&D_8006F088->view.targetPosition, &D_8006F088->view.eyePosition, &sp28, &sp26,
                                      &sp24);
        ptr->unk_1C = sp26 + 0x2000;
        ptr->unk_1E = sp24 - 0x2000;
    }

    return 0;
}

/**
 * Second light-node callback; like ClearCeremony_LightAngleCallbackA but mirrors rotation and offsets it
 * by 0x6000, so the two lights stay fixed relative to the orbiting camera.
 */
s32 ClearCeremony_LightAngleCallbackB(s32 arg0, GraphNode* arg1) {
    unk_D_86002F34_alt18* ptr = (unk_D_86002F34_alt18*)arg1;
    f32 sp28;
    s16 sp26;
    s16 sp24;

    if (arg0 == 2) {
        Vec3f_CalculateDistanceAngles(&D_8006F088->view.targetPosition, &D_8006F088->view.eyePosition, &sp28, &sp26,
                                      &sp24);
        ptr->unk_1C = -0x2000 - sp26;
        ptr->unk_1E = sp24 + 0x6000;
    }

    return 0;
}

/**
 * Render-pass callback on the three arena geo wrappers: copies sArenaTint into the node's textureTint so
 * ClearCeremony_SetArenaTint can dim the whole arena model.
 */
s32 ClearCeremony_ArenaTintCallback(s32 arg0, GraphNode* arg1) {
    DisplayObject* ptr = (DisplayObject*)arg1;

    if (arg0 == 2) {
        ptr->textureTint.rgba = sArenaTint.rgba;
    }

    return 0;
}

/**
 * Stores the arena tint color. The arena fog node is enabled only at full white (and only when the arena
 * archive supplied fog parameters, callbackArg == 1); any dimmer tint disables fog.
 */
void ClearCeremony_SetArenaTint(u8 arg0, u8 arg1, u8 arg2) {
    u8 v = arg0 & arg1 & arg2;

    sArenaTint.r = arg0;
    sArenaTint.g = arg1;
    sArenaTint.b = arg2;

    if (v == 0xFF) {
        if (sArenaFogNode.node.callbackArg == 1) {
            sArenaFogNode.node.flags |= 1;
        }
    } else {
        sArenaFogNode.node.flags &= ~1;
    }
}

/**
 * DMAs three per-species records for sCeremonyModel->modelId (species ID, 1-based): the 0xBC0-byte battle
 * cue bank (D_80075BD0 offsets), the 0x10-byte model dimensions (D_70110) and the 0x20-byte sway layout
 * (D_70B10). Battle_LoadAnimationAndMarkMoveIllegality loads the same tables
 * //@src/fragments/battle_engine/battle_engine_move_animation.c
 */
void ClearCeremony_LoadSpeciesBattleData(void) {
    UNUSED s32 pad[2];
    u8* sp24;
    u8* temp_a1;
    u8* sp1C;

    temp_a1 = (u32)_70D3A0_ROM_START + (D_80075BD0[sCeremonyModel->modelId - 1] & 0xFFFFFF);
    sp24 = (u32)_70D3A0_ROM_START +
           ((((sCeremonyModel->modelId ^ 0) * sizeof(unk_D_70B10)) - sizeof(unk_D_70B10) + (u32)D_70B10) & 0xFFFFFF);
    sp1C = (u32)_70D3A0_ROM_START +
           (((sCeremonyModel->modelId * sizeof(unk_70D3A0)) - sizeof(unk_70D3A0) + (u32)D_70110) & 0xFFFFFF);

    Dma_WriteChunks(sSpeciesBattleCues, temp_a1, temp_a1 + 0xBC0, 0);
    Dma_WriteChunks(&sSpeciesModelDimensions, sp1C, sp1C + sizeof(unk_70D3A0), 0);
    Dma_WriteChunks(&sSpeciesSwayLayout, sp24, sp24 + sizeof(unk_D_70B10), 0);
}

/**
 * Loads the picked Pokemon in sCeremonyModel->partySlot of the showcased roster: attaches its display
 * object, loads its model, starts animation 0, loads its battle data and raises it by the dimensions'
 * unk_08 height offset. Clears the event-track state used by ClearCeremony_UpdateCryEventTrack.
 */
void ClearCeremony_LoadPartyModel(void) {
    sCeremonyModel->modelId = sShowcaseRoster->pokemonPicked[sCeremonyModel->partySlot].species.id;
    ModelRenderer_AttachDisplayObject(&sCeremonyModel->displayObject);
    PokeIcon_OpenModelArchives();

    sCeremonyModel->model = Model_LoadForPokemon(&sShowcaseRoster->pokemonPicked[sCeremonyModel->partySlot]);
    Model_InitDisplayObject(&sCeremonyModel->displayObject, 0, sCeremonyModel->modelId, sCeremonyModel->model->unk_08->unk_00[0]);
    ModelAnim_SetAnimation(&sCeremonyModel->displayObject, 0);
    sCeremonyModel->displayObject.effectSlot = 0;
    ClearCeremony_LoadSpeciesBattleData();
    sCeremonyModel->displayObject.position.y += sSpeciesModelDimensions.unk_08;
    sCeremonyModel->cryTrackActive = 0;
    ModelAnim_SetEventFrame(&sCeremonyModel->displayObject, 0);
    ModelAnim_SetEventTrack(&sCeremonyModel->displayObject, -1);
}

/**
 * Frees the previous party model by popping and re-pushing the 'MONS' pool state, then loads the
 * Pokemon in the current party slot.
 */
void ClearCeremony_ReplacePartyModel(void) {
    ModelRenderer_ClearDisplayObject(&sCeremonyModel->displayObject);

    main_pool_pop_state('MONS');

    main_pool_push_state('MONS');

    ClearCeremony_LoadPartyModel();
}

/**
 * Configures the camera for one party Pokemon: 30-degree vertical field of view, 30..6400 clip range,
 * follow point from the model, then loads keyframe sClearCeremonyCameraKeyframes[partySlot].
 */
void ClearCeremony_SetupOrbitCamera(void) {
    sCeremonyCamera->camera->perspective.verticalFieldOfView = 30.0f;
    sCeremonyCamera->camera->perspective.nearPlane = 30.0f;
    sCeremonyCamera->camera->perspective.farPlane = 6400.0f;
    ClearCeremony_ComputeFollowPoint(sCeremonyModel, &sCeremonyCamera->followPoint);
    ClearCeremony_LoadCameraKeyframe(&sClearCeremonyCameraKeyframes[sCeremonyModel->partySlot], sCeremonyCamera->camera);
    ClearCeremony_UpdateOrbitCamera();
}

/**
 * Replaces the party model with trophy model sTrophyModels[sTrophyIndex].modelId (a model-archive index,
 * not a species) and places it 300 units above the arena floor so it can descend.
 */
void ClearCeremony_LoadTrophyModel(void) {
    ModelRenderer_ClearDisplayObject(&sCeremonyModel->displayObject);

    main_pool_pop_state('MONS');

    main_pool_push_state('MONS');

    sCeremonyModel->modelId = sTrophyModels[sTrophyIndex].modelId;
    ModelRenderer_AttachDisplayObject(&sCeremonyModel->displayObject);
    PokeIcon_OpenModelArchives();
    sCeremonyModel->model = Model_LoadByArchiveIndex(sCeremonyModel->modelId);
    Model_InitDisplayObject(&sCeremonyModel->displayObject, 0, sCeremonyModel->modelId, sCeremonyModel->model->unk_08->unk_00[0]);
    ModelAnim_SetAnimation(&sCeremonyModel->displayObject, 0);
    Vec3f_SetComponentsDuplicate(&sCeremonyModel->displayObject.position, 0.0f, 300.0f, 0.0f);
}

/**
 * Fixed camera for the trophy: 40-degree field of view, look-at at the trophy plus its cameraTargetHeight,
 * eye at (0, 100, 250), or (0, 100, 328) for the larger trophy entries 4 and 5. Resets descent state.
 */
void ClearCeremony_SetupTrophyCamera(void) {
    sTrophySpinPhase = 0;
    sTrophyDescentRate = 0.0f;
    sCeremonyCamera->camera->perspective.verticalFieldOfView = 40.0f;
    sCeremonyCamera->camera->perspective.nearPlane = 30.0f;
    sCeremonyCamera->camera->perspective.farPlane = 6400.0f;

    Vec3f_SetComponentsDuplicate(&sCeremonyCamera->camera->view.targetPosition, sCeremonyModel->displayObject.position.x,
                                 sTrophyModels[sTrophyIndex].cameraTargetHeight + sCeremonyModel->displayObject.position.y,
                                 sCeremonyModel->displayObject.position.z);

    if ((sTrophyIndex == 4) || (sTrophyIndex == 5)) {
        Vec3f_SetComponentsDuplicate(&sCeremonyCamera->camera->view.eyePosition, 0, 100.0f, 328.0f);
    } else {
        Vec3f_SetComponentsDuplicate(&sCeremonyCamera->camera->view.eyePosition, 0, 100.0f, 250.0f);
    }
}

/**
 * Subtracts step from the object's rotationY and returns 1 on the frame it passes target (within one step).
 * Used for the trophy's first full spin before it eases to rest.
 */
s32 ClearCeremony_StepRotationOvershoot(DisplayObject* display_object, s16 step, s16 target) {
    if (step > 0) {
        display_object->rotation.y -= step;
        if ((display_object->rotation.y < target) && ((target - step) < display_object->rotation.y)) {
            return 1;
        }
    } else {
        display_object->rotation.y -= step;
        if ((target < display_object->rotation.y) && (display_object->rotation.y < (target - step))) {
            return 1;
        }
    }
    return 0;
}

/**
 * Spins rotationY toward target; once within about 60 degrees (0x3D * 182 units) it slows to 1/9 of the
 * remaining angle. Snaps to target and returns 1 on arrival.
 */
s32 ClearCeremony_StepRotationToTarget(DisplayObject* display_object, s16 step, s16 target) {
    if ((((display_object->rotation.y - target) / 182) < 0x3D) && (((display_object->rotation.y - target) / 182) >= 0)) {
        step = (display_object->rotation.y - target) / 9;
    }
    display_object->rotation.y -= step;

    if (step > 0) {
        if ((target >= display_object->rotation.y) && (display_object->rotation.y >= (target - step))) {
            display_object->rotation.y = target;
            return 1;
        }
    } else if ((display_object->rotation.y >= target) && ((target - step) >= display_object->rotation.y)) {
        display_object->rotation.y = target;
        return 1;
    }
    return 0;
}

/**
 * Per-frame trophy entrance: one overshooting spin then an eased stop at rotationY 0, while the height
 * eases to the floor with an accelerating rate. The camera look-at follows. Returns 1 when y <= 0.5.
 */
s32 ClearCeremony_UpdateTrophyDescent(void) {
    if (sTrophySpinPhase < 3) {
        if ((sTrophySpinPhase <= 0) && (ClearCeremony_StepRotationOvershoot(&sCeremonyModel->displayObject, 0x71C, 0) != 0)) {
            sTrophySpinPhase++;
        }

        if ((sTrophySpinPhase > 0) && (ClearCeremony_StepRotationToTarget(&sCeremonyModel->displayObject, 0x71C, 0) != 0)) {
            sTrophySpinPhase++;
        }
    }

    Math_EaseTowardF(&sTrophyDescentRate, 0.1f, 0.02f);
    Math_EaseTowardF(&sCeremonyModel->displayObject.position.y, 0.0f, sTrophyDescentRate);

    if (sCeremonyModel->displayObject.position.y <= 0.25f) {
        sCeremonyModel->displayObject.position.y = 0.0f;
    }

    Vec3f_SetComponentsDuplicate(&sCeremonyCamera->camera->view.targetPosition, sCeremonyModel->displayObject.position.x,
                                 sTrophyModels[sTrophyIndex].cameraTargetHeight + sCeremonyModel->displayObject.position.y,
                                 sCeremonyModel->displayObject.position.z);

    if (sCeremonyModel->displayObject.position.y <= 0.5f) {
        return 1;
    }
    return 0;
}

/**
 * Plays sound command 0x0110000F once, then on odd frames emits falling particle bursts at the trophy's
 * x/z and particleHeight +-20; a second, third and fourth stream join at frame 10, 20 and 30.
 */
void ClearCeremony_EmitTrophyParticles(void) {
    Vec3f sp44;

    if (sParticleSoundPlayed == 0) {
        sParticleSoundPlayed = 1;
        Audio_PlayCategory11SoundCommand(0x0110000F, 0, 0);
    }

    if (sCeremonyModel->frameTimer & 1) {
        Vec3f_SetComponentsDuplicate(&sp44, sCeremonyModel->displayObject.position.x,
                                     sTrophyModels[sTrophyIndex].particleHeight + (MathUtil_RandomU16Range(0x28) - 0x14),
                                     sCeremonyModel->displayObject.position.z);
        Particle_EmitBurstAtTransform(1.0f, sp44, sCeremonyModel->displayObject.rotation,
                                      MiniFx_UpdatePalette17DelayedRevealFallParticle,
                                      &gMiniFxParticleDescriptorIa8Variant, 1);

        if ((sCeremonyModel->frameTimer >= 0xA) && (sCeremonyModel->frameTimer & 1)) {
            Vec3f_SetComponentsDuplicate(&sp44, sCeremonyModel->displayObject.position.x,
                                         sTrophyModels[sTrophyIndex].particleHeight + (MathUtil_RandomU16Range(0x28) - 0x14),
                                         sCeremonyModel->displayObject.position.z);
            Particle_EmitBurstAtTransform(1.0f, sp44, sCeremonyModel->displayObject.rotation,
                                          MiniFx_UpdatePalette17DelayedRevealFallParticle,
                                          &gMiniFxParticleDescriptorIa8Variant, 1);
        }

        if ((sCeremonyModel->frameTimer >= 0x14) && (sCeremonyModel->frameTimer & 1)) {
            Vec3f_SetComponentsDuplicate(&sp44, sCeremonyModel->displayObject.position.x,
                                         sTrophyModels[sTrophyIndex].particleHeight + (MathUtil_RandomU16Range(0x28) - 0x14),
                                         sCeremonyModel->displayObject.position.z);
            Particle_EmitBurstAtTransform(1.0f, sp44, sCeremonyModel->displayObject.rotation,
                                          MiniFx_UpdatePalette17DelayedRevealFallParticle,
                                          &gMiniFxParticleDescriptorIa8Variant, 1);
        }

        if ((sCeremonyModel->frameTimer >= 0x1E) && (sCeremonyModel->frameTimer & 1)) {
            Vec3f_SetComponentsDuplicate(&sp44, sCeremonyModel->displayObject.position.x,
                                         sTrophyModels[sTrophyIndex].particleHeight + (MathUtil_RandomU16Range(0x28) - 0x14),
                                         sCeremonyModel->displayObject.position.z);
            Particle_EmitBurstAtTransform(1.0f, sp44, sCeremonyModel->displayObject.rotation,
                                          MiniFx_UpdatePalette17DelayedRevealFallParticle,
                                          &gMiniFxParticleDescriptorIa8Variant, 1);
        }
    }
}

/**
 * Steps the arena tint from its current level toward 100 by 2 per frame, darkening the arena behind the
 * trophy after it has landed.
 */
void ClearCeremony_DimArenaTint(void) {
    sCeremonyModel->arenaTint = Math_StepToF(sCeremonyModel->arenaTint, 100.0f, 2.0f, 2.0f);
    ClearCeremony_SetArenaTint(sCeremonyModel->arenaTint, sCeremonyModel->arenaTint, sCeremonyModel->arenaTint);
}

/**
 * Every 50 frames, if no event track is playing and cue-bank byte 0xA51 is not 0xFF, starts that model
 * event track; when it nears its end, clears it. The "cry" meaning of byte 0xA51 is not yet verified.
 */
void ClearCeremony_UpdateCryEventTrack(void) {
    if (((sCeremonyModel->frameTimer % 50) == 0) && (sCeremonyModel->cryTrackActive == 0) && (sSpeciesBattleCues[0xA51] != 0xFF)) {
        ModelAnim_ClearEventTrack(&sCeremonyModel->displayObject);
        ModelAnim_SetEventTrack(&sCeremonyModel->displayObject, sSpeciesBattleCues[0xA51]);
        sCeremonyModel->cryTrackActive = 1;
    } else if ((sCeremonyModel->cryTrackActive == 1) && (ModelAnim_IsEventTrackNearEnd(&sCeremonyModel->displayObject) == 1)) {
        sCeremonyModel->cryTrackActive = 0;
        ModelAnim_SetEventFrame(&sCeremonyModel->displayObject, 0);
        ModelAnim_SetEventTrack(&sCeremonyModel->displayObject, -1);
    }
}

/**
 * Initializes a sliding text panel: texture, start x, y, width and height; clears motion state and loads
 * the stage-0 colors (yellow on black) from sClearPanelColorStages.
 */
void ClearCeremony_SetupTextPanel(ClearCeremonyTextPanel* arg0, u8* arg1, f32 arg2, s16 arg3, s16 arg4, s16 arg5) {
    arg0->texture = arg1;
    arg0->positionX = arg2;
    arg0->positionY = arg3;
    arg0->width = arg4;
    arg0->height = arg5;

    arg0->colorStage = 0;
    arg0->unk_8C = 0;
    arg0->hasSettled = 0;
    arg0->unk_A4 = 0;
    arg0->velocity = 0.0f;

    arg0->colors.unk_00 = sClearPanelColorStages->unk_00;
    arg0->colors.unk_02 = sClearPanelColorStages->unk_02;
    arg0->colors.unk_04 = sClearPanelColorStages->unk_04;
    arg0->colors.unk_06 = sClearPanelColorStages->unk_06;
    arg0->colors.unk_08 = sClearPanelColorStages->unk_08;
    arg0->colors.unk_0A = sClearPanelColorStages->unk_0A;
}

/**
 * Selects the cup_clear_ui textures for the trophy screen: the "R-2" badge in Round 2, "CLEAR", the ball
 * tier by modeSelection (0 POKe BALL, 1 GREAT BALL, 2 ULTRA BALL, 3 MASTER BALL) and the cup name by
 * ruleCategory (1..3 POKe CUP copies, 4 PETIT CUP, 5 PIKA CUP, 6 PRIME CUP, otherwise POKe CUP).
 */
void ClearCeremony_ConfigureTextPanels(void) {
    if (gSessionContext.inRound2 == 1) {
        ClearCeremony_SetupTextPanel(sRound2BadgePanel, &D_300C6D0, -250.0f, 0xC8, 0x18, 0x18);
    }

    ClearCeremony_SetupTextPanel(sClearTextPanel, &D_3000180, -250.0f, 0xC8, 0x40, 0x18);

    if (gSessionContext.modeSelection == 0) {
        ClearCeremony_SetupTextPanel(sBallTierPanel, &D_30060E0, -170.0f, 0x3C, 0x88, 0x14);
    } else if (gSessionContext.modeSelection == 1) {
        ClearCeremony_SetupTextPanel(sBallTierPanel, &D_3006B80, -170.0f, 0x3C, 0x78, 0x14);
    } else if (gSessionContext.modeSelection == 2) {
        ClearCeremony_SetupTextPanel(sBallTierPanel, &D_3004E20, -170.0f, 0x3C, 0x78, 0x14);
    } else if (gSessionContext.modeSelection == 3) {
        ClearCeremony_SetupTextPanel(sBallTierPanel, &D_3005780, -170.0f, 0x3C, 0x78, 0x14);
    }

    switch (gSessionContext.ruleCategory) {
        case 1:
            ClearCeremony_SetupTextPanel(sCupNamePanel, &D_30024E0, -100.0f, 0x1E, 0xB0, 0x14);
            break;

        case 2:
            ClearCeremony_SetupTextPanel(sCupNamePanel, &D_30032A0, -100.0f, 0x1E, 0xB0, 0x14);
            break;

        case 3:
            ClearCeremony_SetupTextPanel(sCupNamePanel, &D_3004060, -100.0f, 0x1E, 0xB0, 0x14);
            break;

        case 4:
            ClearCeremony_SetupTextPanel(sCupNamePanel, &D_3000780, -100.0f, 0x1E, 0x88, 0x14);
            break;

        case 5:
            ClearCeremony_SetupTextPanel(sCupNamePanel, &D_3001220, -100.0f, 0x1E, 0x78, 0x14);
            break;

        case 6:
            ClearCeremony_SetupTextPanel(sCupNamePanel, &D_3001B80, -100.0f, 0x1E, 0x78, 0x14);
            break;

        default:
            ClearCeremony_SetupTextPanel(sCupNamePanel, &D_30024E0, -100.0f, 0x1E, 0xB0, 0x14);
            break;
    }
}

/**
 * Begins the ceremony at party slot 0: pushes the 'MONS' pool state and loads the first picked Pokemon.
 */
void ClearCeremony_StartPartyShowcase(void) {
    sCeremonyModel->sequenceStep = 0;
    sCeremonyModel->partySlot = 0;
    sCeremonyModel->modelId = 0;

    main_pool_push_state('MONS');

    ClearCeremony_LoadPartyModel();
}

/**
 * Takes the camera node from the scene root, publishes it as the active camera (D_87906054) and places it
 * with ClearCeremony_SetupOrbitCamera.
 */
void ClearCeremony_BeginOrbitCamera(void) {
    sCeremonyCamera->camera = D_87906050->node.children;
    D_87906054 = sCeremonyCamera->camera;
    ClearCeremony_SetupOrbitCamera();
}

/**
 * Begins the ceremony directly at the trophy (sequence step 6, slot 6), skipping the party showcase.
 */
void ClearCeremony_StartTrophyShowcase(void) {
    sCeremonyModel->sequenceStep = 6;
    sCeremonyModel->partySlot = 6;
    sCeremonyModel->modelId = 0;

    main_pool_push_state('MONS');

    ClearCeremony_LoadTrophyModel();
}

/**
 * Takes the camera node from the scene root, publishes it as the active camera and places it with
 * ClearCeremony_SetupTrophyCamera.
 */
void ClearCeremony_BeginTrophyCamera(void) {
    sCeremonyCamera->camera = D_87906050->node.children;
    D_87906054 = sCeremonyCamera->camera;
    ClearCeremony_SetupTrophyCamera();
}

/**
 * Sets the shared panel draw state to 3, in which ClearCeremony_DrawTextPanels draws nothing.
 */
void ClearCeremony_HideTextPanels(void) {
    sNicknamePanel->drawState = 3;
}

/**
 * Chooses the battle side whose unk_1C is 1 and showcases its first tag-team member's picked Pokemon.
 */
void ClearCeremony_SelectPartyRoster(void) {
    if (gSessionContext.battleSides[0].unk_1C == 1) {
        sShowcaseRoster = gSessionContext.battleSides[0].tagTeamMember[0];
        sShowcaseSide = &gSessionContext.battleSides[0];
    } else {
        sShowcaseRoster = gSessionContext.battleSides[1].tagTeamMember[0];
        sShowcaseSide = &gSessionContext.battleSides[1];
    }
}

/**
 * Polls the controllers once per frame and resets input repeat state.
 */
void ClearCeremony_ReadControllerInput(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

/**
 * Ceremony state machine. Steps 1-3 show each party Pokemon with its nickname (appearing at frame 25)
 * until its camera keyframe finishes, then close and reopen the band wipe on the next slot; Gym Leader
 * Castle (ruleCategory 7) ends after slot 5. Step 6/7 drop the trophy in under the opening wipe, step 8
 * shows the text panels and particles, and step 9 dims the arena and slowly orbits the camera until A or
 * B (0xC000) is pressed. Returns 1 when the ceremony should end.
 */
s32 ClearCeremony_UpdateSequence(void) {
    switch (sCeremonyModel->sequenceStep) {
        case 0:
            ClearCeremony_SetupOrbitCamera();
            ClearCeremony_UpdateCameraKeyframe(sCeremonyCamera->camera);
            sCeremonyModel->cryTrackActive = 0;
            sCeremonyModel->sequenceStep += 1;
            break;

        case 1:
            ClearCeremony_UpdateCryEventTrack();
            if (sCeremonyModel->frameTimer++ == 0x19) {
                sNicknamePanel->drawState = 0;
            }

            if (ClearCeremony_UpdateCameraKeyframe(sCeremonyCamera->camera) != 0) {
                if ((gSessionContext.ruleCategory == 7) && (sCeremonyModel->partySlot == 5)) {
                    return 1;
                }
                WipeTransition_Start(3, 0x1E);
                sCeremonyModel->sequenceStep += 1;
            }
            break;

        case 2:
            WipeTransition_Update();
            if (WipeTransition_GetState() == 1) {
                sCeremonyModel->partySlot++;
                if (sCeremonyModel->partySlot == 6) {
                    sCeremonyModel->cryTrackActive = 0;
                    ClearCeremony_LoadTrophyModel();
                    ClearCeremony_SetupTrophyCamera();
                    sNicknamePanel->drawState = 3;
                    sCeremonyModel->frameTimer = 0;
                    WipeTransition_Start(2, 0x1E);
                    sCeremonyModel->sequenceStep = 6;
                } else {
                    sCeremonyModel->cryTrackActive = 0;
                    ClearCeremony_ReplacePartyModel();
                    ClearCeremony_SetupOrbitCamera();
                    sNicknamePanel->drawState = 3;
                    sCeremonyModel->frameTimer = 0;
                    WipeTransition_Start(2, 0x1E);
                    sCeremonyModel->sequenceStep++;
                }
            }
            break;

        case 3:
            WipeTransition_Update();
            if (sCeremonyModel->frameTimer++ > 0) {
                ClearCeremony_UpdateCameraKeyframe(sCeremonyCamera->camera);
            }

            if (WipeTransition_GetState() == 0) {
                sCeremonyModel->frameTimer = 0;
                sCeremonyModel->sequenceStep = 1;
            }
            break;

        case 6:
            sCeremonyModel->arenaTint = 0xFF;
            WipeTransition_Update();
            ClearCeremony_UpdateTrophyDescent();
            if (WipeTransition_GetState() == 0) {
                sCeremonyModel->sequenceStep++;
            }
            break;

        case 7:
            if (ClearCeremony_UpdateTrophyDescent() != 0) {
                sCeremonyModel->frameTimer = 0;
                sNicknamePanel->drawState = 0;
                Audio_PlayCategory11SoundCommand(0x01100002, 0, 0);
                sParticleSoundPlayed = 0;
                sCeremonyModel->sequenceStep++;
            }
            break;

        case 8:
            if (sCupNamePanel->hasSettled != 0) {
                ClearCeremony_EmitTrophyParticles();
            }
            if (sCeremonyModel->frameTimer++ >= 0x41) {
                sOrbitSpeed = 0;
                Vec3f_CalculateDistanceAngles(&D_87906054->view.targetPosition, &D_87906054->view.eyePosition,
                                              &sOrbitDistance, &sOrbitRotationX, &sOrbitRotationY);
                sCeremonyModel->sequenceStep++;
            }
            break;

        case 9:
            ClearCeremony_DimArenaTint();

            if (sCupNamePanel->hasSettled != 0) {
                ClearCeremony_EmitTrophyParticles();
            }

            if (gPlayer1Controller->buttonDown & 0xC000) {
                return 1;
            }

            if (sCeremonyModel->frameTimer++ >= 0x3E9) {
                sCeremonyModel->frameTimer = 0x64;
            }

            sOrbitSpeed = Math_StepToS32(sOrbitSpeed, 0x3C, 1, 1);
            sOrbitRotationY += sOrbitSpeed;
            Camera_ComputeEyeFromAngles(&D_87906054->view.targetPosition, &D_87906054->view.eyePosition, sOrbitDistance,
                                        sOrbitRotationX, sOrbitRotationY);
            break;
    }

    if ((sCeremonyVariant == 2) && (sFrameCounter == 0x12C)) {
        Audio_StartMusicTrack(0x5B);
    }
    return 0;
}

/**
 * Spring-like slide for a text panel: integrates velocity (capped at 28), and when the panel passes
 * rest_x it clamps there and reverses velocity scaled by restitution. Returns 1 when two
 * successive bounces have the same velocity, i.e. the panel has settled.
 */
s32 ClearCeremony_StepPanelBounce(ClearCeremonyTextPanel* panel, f32 restitution, f32 arg2, f32 arg3, f32 rest_x, f32 arg5) {
    f32 temp_fa1;
    f32 sp20;
    f32 temp_fv0;
    f32 temp_fv0_2;
    f32 temp_fv0_4;
    f32* new_var;

    sp20 = panel->unk_A4;

    temp_fv0 = Math_StepToF(sp20, 0.0f, 0.0f, 0.0f);
    panel->unk_A4 = temp_fv0;
    if (temp_fv0 >= 0.0f) {
        temp_fv0 = 0;
        if ((panel && panel) && panel) {} // FAKE
    }
    temp_fv0_2 = COSS(0) * temp_fv0;
    temp_fa1 = -arg3;

    new_var = &panel->velocity; // FAKE

    temp_fv0_4 = (-temp_fa1) - (arg2 * ((sp20 * panel->velocity) + temp_fv0_2));
    panel->velocity = (temp_fv0_4 * arg5) + ((*new_var) + temp_fv0_2);

    if (panel->velocity >= 28.0) {
        panel->velocity = 28.0f;
    }

    panel->positionX += panel->velocity;
    if (temp_fa1 >= 0) {
        if ((panel->positionX < rest_x) && (panel->positionX >= 0.0f)) {
            panel->positionX = rest_x;
            panel->velocity *= -restitution;
        }
    } else if ((rest_x < panel->positionX) && (panel->positionX >= 0.0f)) {
        panel->positionX = rest_x;
        panel->velocity *= -restitution;
        if (panel->lastVelocity == panel->velocity) {
            return 1;
        }
        panel->lastVelocity = panel->velocity;
    }

    return 0;
}

/**
 * Draws the ceremony captions. On the trophy (slot 6): configures the panels, bounces in CLEAR, the cup
 * name, the R-2 badge and (except for trophy entries 4 and 5) the ball tier, then cycles the panel colors
 * through sClearPanelColorStages before drawing them statically. On party slots: prints the Pokemon's
 * nickname, centered and bouncing in from the left. Always draws the band wipe last.
 */
void ClearCeremony_DrawTextPanels(void) {
    if (sCeremonyModel->partySlot == 6) {
        switch (sNicknamePanel->drawState) {
            case 3:
                break;

            case 0:
                ClearCeremony_ConfigureTextPanels();
                sNicknamePanel->drawState++;
                break;

            case 1:
                ClearCeremony_StepPanelBounce(sClearTextPanel, 0.25f, 0.0f, 3.0f, 0xA3 - (sClearTextPanel->width / 2), 1.7f);
                UI_DrawGradientTexture(sClearTextPanel->texture, sClearTextPanel->positionX, sClearTextPanel->positionY, sClearTextPanel->width,
                                       sClearTextPanel->height, &sClearTextPanel->colors);
                sCupNamePanel->hasSettled =
                    ClearCeremony_StepPanelBounce(sCupNamePanel, 0.25f, 0.0f, 3.0f, 0xA3 - (sCupNamePanel->width / 2), 1.7f);
                UI_DrawGradientTexture(sCupNamePanel->texture, sCupNamePanel->positionX, sCupNamePanel->positionY, sCupNamePanel->width,
                                       sCupNamePanel->height, &sClearTextPanel->colors);

                if (gSessionContext.inRound2 == 1) {
                    UI_DrawTextureRgba16(sRound2BadgePanel->texture, (s16)sCupNamePanel->positionX + sCupNamePanel->width,
                                         sCupNamePanel->positionY - 2, sRound2BadgePanel->width, sRound2BadgePanel->height);
                }

                if ((sTrophyIndex != 4) && (sTrophyIndex != 5)) {
                    ClearCeremony_StepPanelBounce(sBallTierPanel, 0.25f, 0.0f, 3.0f, 0xA3 - (sBallTierPanel->width / 2), 1.7f);
                    UI_DrawGradientTexture(sBallTierPanel->texture, sBallTierPanel->positionX, sBallTierPanel->positionY,
                                           sBallTierPanel->width, sBallTierPanel->height, &sClearTextPanel->colors);
                }

                if (sCupNamePanel->hasSettled != 0) {
                    sClearTextPanel->colors.unk_00 =
                        Math_StepToS32(sClearTextPanel->colors.unk_00, sClearPanelColorStages[sClearTextPanel->colorStage + 1].unk_00,
                                       sClearPanelColorStages[sClearTextPanel->colorStage + 1].unk_0C,
                                       sClearPanelColorStages[sClearTextPanel->colorStage + 1].unk_0C);
                    sClearTextPanel->colors.unk_02 =
                        Math_StepToS32(sClearTextPanel->colors.unk_02, sClearPanelColorStages[sClearTextPanel->colorStage + 1].unk_02,
                                       sClearPanelColorStages[sClearTextPanel->colorStage + 1].unk_0E,
                                       sClearPanelColorStages[sClearTextPanel->colorStage + 1].unk_0E);
                    sClearTextPanel->colors.unk_04 =
                        Math_StepToS32(sClearTextPanel->colors.unk_04, sClearPanelColorStages[sClearTextPanel->colorStage + 1].unk_04,
                                       sClearPanelColorStages[sClearTextPanel->colorStage + 1].unk_10,
                                       sClearPanelColorStages[sClearTextPanel->colorStage + 1].unk_10);
                    sClearTextPanel->colors.unk_06 =
                        Math_StepToS32(sClearTextPanel->colors.unk_06, sClearPanelColorStages[sClearTextPanel->colorStage + 1].unk_06,
                                       sClearPanelColorStages[sClearTextPanel->colorStage + 1].unk_12,
                                       sClearPanelColorStages[sClearTextPanel->colorStage + 1].unk_12);
                    sClearTextPanel->colors.unk_08 =
                        Math_StepToS32(sClearTextPanel->colors.unk_08, sClearPanelColorStages[sClearTextPanel->colorStage + 1].unk_08,
                                       sClearPanelColorStages[sClearTextPanel->colorStage + 1].unk_14,
                                       sClearPanelColorStages[sClearTextPanel->colorStage + 1].unk_14);
                    sClearTextPanel->colors.unk_0A =
                        Math_StepToS32(sClearTextPanel->colors.unk_0A, sClearPanelColorStages[sClearTextPanel->colorStage + 1].unk_0A,
                                       sClearPanelColorStages[sClearTextPanel->colorStage + 1].unk_16,
                                       sClearPanelColorStages[sClearTextPanel->colorStage + 1].unk_16);
                    if ((sClearTextPanel->colors.unk_00 == sClearPanelColorStages[sClearTextPanel->colorStage + 1].unk_00) &&
                        (sClearTextPanel->colors.unk_02 == sClearPanelColorStages[sClearTextPanel->colorStage + 1].unk_02) &&
                        (sClearTextPanel->colors.unk_04 == sClearPanelColorStages[sClearTextPanel->colorStage + 1].unk_04) &&
                        (sClearTextPanel->colors.unk_06 == sClearPanelColorStages[sClearTextPanel->colorStage + 1].unk_06) &&
                        (sClearTextPanel->colors.unk_08 == sClearPanelColorStages[sClearTextPanel->colorStage + 1].unk_08) &&
                        (sClearTextPanel->colors.unk_0A == sClearPanelColorStages[sClearTextPanel->colorStage + 1].unk_0A)) {

                        sClearTextPanel->colorStage++;
                        if (sClearTextPanel->colorStage == 4) {
                            sNicknamePanel->drawState++;
                        }
                    }
                }
                break;

            case 2:
                UI_DrawGradientTexture(sClearTextPanel->texture, sClearTextPanel->positionX, sClearTextPanel->positionY, sClearTextPanel->width,
                                       sClearTextPanel->height, sSettledPanelColors);
                UI_DrawGradientTexture(sCupNamePanel->texture, sCupNamePanel->positionX, sCupNamePanel->positionY, sCupNamePanel->width,
                                       sCupNamePanel->height, sSettledPanelColors);

                if (gSessionContext.inRound2 == 1) {
                    UI_DrawTextureRgba16(sRound2BadgePanel->texture, (s16)sCupNamePanel->positionX + sCupNamePanel->width,
                                         sCupNamePanel->positionY - 2, sRound2BadgePanel->width, sRound2BadgePanel->height);
                }

                if ((sTrophyIndex != 4) && (sTrophyIndex != 5)) {
                    UI_DrawGradientTexture(sBallTierPanel->texture, sBallTierPanel->positionX, sBallTierPanel->positionY,
                                           sBallTierPanel->width, sBallTierPanel->height, sSettledPanelColors);
                }
                break;
        }
    } else {
        switch (sNicknamePanel->drawState) {
            case 3:
                break;

            case 0:
                sNicknamePanel->positionX = -80.0f;
                sNicknamePanel->positionY = 0xB4;
                sNicknamePanel->unk_A4 = 0.0f;
                sNicknamePanel->velocity = 0.0f;
                sprintf(sNicknamePanel->text, "%s", sShowcaseRoster->pokemonPicked[sCeremonyModel->partySlot].nickname);
                sNicknamePanel->drawState++;
                break;

            case 1:
                Font_BeginTranslucentTextRendering();
                Font_EnableTwoCycleTexturing();
                Font_SetActive(0x10, 0);
                Gfx_SetPrimColor(0, 0x82, 0xFF, 0xFF); // #0082FF
                Gfx_SetEnvColor(0xC8, 0xFF, 0xFF, 0xFF); // #C8FFFF
                ClearCeremony_StepPanelBounce(sNicknamePanel, 0.35f, 0.0f, 4.0f, 160.0f, 1.4f);
                Font_Printf((s32)sNicknamePanel->positionX - (Font_MeasureTextExtent(0x10, 0, sNicknamePanel->text) / 2),
                            sNicknamePanel->positionY, sNicknamePanel->text);
                Font_DisableTwoCycleTexturing();
                Font_EndTexturedTextRendering();
                break;
        }
    }

    WipeTransition_Draw();
}

/**
 * Renders one ceremony frame: clears to the arena's fill color (archive file 2; 0 or -1 means 0x0001,
 * values >= 0x10000 mean 0xA6BF), draws the scene graph, captions, particles and wipe.
 */
void ClearCeremony_RenderFrame(void) {
    BgStage_DrawFrame();

    if ((sArenaFillColor == -1) || (sArenaFillColor == 0)) {
        GfxImage_FillCurrent(&gDisplayListHead, 1);
    } else if (sArenaFillColor < 0x10000) {
        GfxImage_FillCurrent(&gDisplayListHead, sArenaFillColor);
    } else {
        GfxImage_FillCurrent(&gDisplayListHead, 0xA6BF);
    }

    GeoRender_AdvanceFrameCounter();
    Geo_RenderRootNode(&D_87906050->node);
    ClearCeremony_DrawTextPanels();
    MiniFx_DrawParticles();
    BgStage_AdvanceFrame();
}

/**
 * Renders the scene graph but covers it with an opaque black full-screen rectangle (fade-in lead frames).
 */
void ClearCeremony_RenderBlackFrame(void) {
    BgStage_DrawFrame();
    GfxImage_FillCurrent(&gDisplayListHead, 1);
    GeoRender_AdvanceFrameCounter();
    Geo_RenderRootNode(&D_87906050->node);
    Gfx_FillRectWithDisplayState(0, 0, 0x140, 0xF0, 0, 0, 0, 0xFF);
    BgStage_AdvanceFrame();
}

/**
 * Spends 18 frames polling input, rendering black and starting a 16-frame fade on the first 3, then starts
 * music 0x44 for the trophy-only variant or music 8 otherwise.
 */
void ClearCeremony_FadeInSequence(void) {
    s32 i;

    for (i = 0; i < 18; i++) {
        ClearCeremony_ReadControllerInput();

        if (i < 3) {
            StageContext_SetClearColor(1);
            StageFade_StartFromOpaque(0x10);
            ClearCeremony_RenderBlackFrame();
        }
    }

    if (sCeremonyVariant == 2) {
        Audio_StartMusicTrack(0x44);
    } else {
        Audio_StartMusicTrack(8);
    }
}

/**
 * Main ceremony loop: polls input, updates particles and the sequence, advances particle frame counters
 * and renders, until ClearCeremony_UpdateSequence returns 1.
 */
void ClearCeremony_RunLoop(void) {
    s32 var_s0 = 1;
    void (*temp_s2)(void) = (void*)Util_ConvertAddrToVirtAddr(Particle_UpdateFrameCountersAlias);

    sFrameCounter = 0;
    while (var_s0 != 0) {
        sFrameCounter++;

        ClearCeremony_ReadControllerInput();
        MiniFx_UpdateParticles();

        if (ClearCeremony_UpdateSequence() != 0) {
            var_s0 = 0;
        }

        temp_s2();
        ClearCeremony_RenderFrame();
    }
}

/**
 * Fades the screen and audio out over 10 frames, or over 100 frames with 100 frames before and 30 after
 * for Gym Leader Castle, continuing to render the scene throughout.
 */
void ClearCeremony_FadeOutSequence(void) {
    s32 i;
    s32 var_s1;
    void (*temp_s2)(void) = Util_ConvertAddrToVirtAddr(Particle_UpdateFrameCountersAlias);

    if (gSessionContext.ruleCategory == 7) {
        for (i = 0; i < 100; i++) {
            ClearCeremony_ReadControllerInput();
            MiniFx_UpdateParticles();

            temp_s2();

            ClearCeremony_RenderFrame();
        }
    }

    if (gSessionContext.ruleCategory == 7) {
        var_s1 = 0x64;
    } else {
        var_s1 = 0xA;
    }

    StageFade_StartFromTransparent(var_s1);
    Audio_FadeOutAll(var_s1 - 2);

    for (i = 0; i < var_s1; i++) {
        if (gSessionContext.ruleCategory == 7) {
            ClearCeremony_ReadControllerInput();
            MiniFx_UpdateParticles();
            temp_s2();
            ClearCeremony_RenderFrame();
        } else {
            ClearCeremony_ReadControllerInput();
            MiniFx_UpdateParticles();
            ClearCeremony_UpdateSequence();
            temp_s2();
            ClearCeremony_RenderFrame();
        }
    }

    if (gSessionContext.ruleCategory == 7) {
        for (i = 0; i < 30; i++) {
            ClearCeremony_ReadControllerInput();
            MiniFx_UpdateParticles();

            temp_s2();

            ClearCeremony_RenderFrame();
        }
    }
}

/**
 * Builds the ceremony scene: picks the stadium_models arena by ruleCategory (plus modeSelection for Gym
 * Leader Castle) and the trophy entry by ruleCategory/modeSelection, attaches arena geo files 0, 1 and 3
 * under tinted wrappers, reads the fill color (file 2) and fog (file 4), allocates state, and chooses the
 * variant: party-then-trophy (categories 1, 2, 4, 5, 7 and the Master Ball tier) or trophy only.
 */
void ClearCeremony_LoadScene(void) {
    MemoryBlock* sp34;
    UNUSED s32 pad;
    FragmentEntry sp2C;
    void* temp_v0_3;
    unk_D_8690A610_018* temp_v0_6;

    sArenaArchiveIndex = sArenaArchiveIndexByRule[gSessionContext.ruleCategory];
    if (gSessionContext.ruleCategory == 7) {
        sArenaArchiveIndex += gSessionContext.modeSelection;
    }

    switch (gSessionContext.ruleCategory) {
        case 1:
            sTrophyIndex = 4;
            break;

        case 2:
            sTrophyIndex = 4;
            break;

        case 3:
            sTrophyIndex = gSessionContext.modeSelection;
            break;

        case 4:
#if defined(VERSION_US_REV2)
            sTrophyIndex = 4;
#else
            sTrophyIndex = 5;
#endif
            break;

        case 5:
            sTrophyIndex = 5;
            break;

        case 6:
            sTrophyIndex = gSessionContext.modeSelection;
            break;

        default:
            sTrophyIndex = 0;
            break;
    }

    sp2C = BinArchive_GetFile(BinArchive_Open(stadium_models_ROM_START, _5C7A70_ROM_START, 1, 1), sArenaArchiveIndex);

    sArenaFile2Data = sp2C(2, 0);
    GeoNode_CreateFog(0, &sArenaFogNode, 0x3C0, 0x3E8, 0xFF, 0xFF, 0xFF, 0xFF);
    GeoNode_CreateContainer(NULL, &sArenaGeoNodeFile0);
    GeoNode_CreateContainer(NULL, &sArenaGeoNodeFile1);
    GeoNode_CreateContainer(NULL, &sArenaGeoNodeFile3);
    sp34 = MainPool_AllocState(main_pool_get_available(), 0);
    D_87906050 = process_geo_layout(sp34, sCeremonyGeoLayout);

    temp_v0_3 = sp2C(0, 0);
    if (temp_v0_3 != NULL) {
        GraphNode_AppendChild(&sArenaGeoNodeFile0, process_geo_layout(sp34, temp_v0_3));
    }

    temp_v0_3 = sp2C(1, 0);
    if (temp_v0_3 != NULL) {
        GraphNode_AppendChild(&sArenaGeoNodeFile1, process_geo_layout(sp34, temp_v0_3));
    }

    temp_v0_3 = sp2C(3, 0);
    if (temp_v0_3 != NULL) {
        GraphNode_AppendChild(&sArenaGeoNodeFile3, process_geo_layout(sp34, temp_v0_3));
    }

    sArenaFillColor = sp2C(2, 0);
    MainPool_FinalizeAllocation(sp34);

    temp_v0_6 = sp2C(4, 0);
    if (temp_v0_6 == NULL) {
        sArenaFogNode.node.callbackArg = 0;
        sArenaFogNode.node.flags &= ~1;
    } else {
        sArenaFogNode.unk_18.unk_00 = temp_v0_6->unk_00;
        sArenaFogNode.unk_18.unk_02 = temp_v0_6->unk_02;
        sArenaFogNode.unk_18.unk_04.rgba = temp_v0_6->unk_04.rgba;
        sArenaFogNode.node.callbackArg = 1;
    }

    ModelRenderer_InitDisplayRoots();
    ClearCeremony_SetArenaTint(0xFF, 0xFF, 0xFF);
    MiniFx_Init();

    sCeremonyModel = main_pool_alloc(sizeof(ClearCeremonyModel), 0);
    bzero(sCeremonyModel, sizeof(ClearCeremonyModel));

    sCeremonyCamera = main_pool_alloc(sizeof(ClearCeremonyCamera), 0);
    bzero(sCeremonyCamera, sizeof(ClearCeremonyCamera));

    sNicknamePanel = main_pool_alloc(sizeof(ClearCeremonyTextPanel), 0);
    bzero(sNicknamePanel, sizeof(ClearCeremonyTextPanel));

    sClearTextPanel = main_pool_alloc(sizeof(ClearCeremonyTextPanel), 0);
    bzero(sClearTextPanel, sizeof(ClearCeremonyTextPanel));

    sCupNamePanel = main_pool_alloc(sizeof(ClearCeremonyTextPanel), 0);
    bzero(sCupNamePanel, sizeof(ClearCeremonyTextPanel));

    sBallTierPanel = main_pool_alloc(sizeof(ClearCeremonyTextPanel), 0);
    bzero(sBallTierPanel, sizeof(ClearCeremonyTextPanel));

    sRound2BadgePanel = main_pool_alloc(sizeof(ClearCeremonyTextPanel), 0);
    bzero(sRound2BadgePanel, sizeof(ClearCeremonyTextPanel));

    ClearCeremony_SelectPartyRoster();

    switch (gSessionContext.ruleCategory) {
        case 1:
        case 2:
        case 4:
        case 5:
        case 7:
            ClearCeremony_StartPartyShowcase();
            ClearCeremony_BeginOrbitCamera();
            ClearCeremony_HideTextPanels();
            sCeremonyVariant = 0;
            break;

        default:
            if (gSessionContext.modeSelection == 3) {
                sCeremonyVariant = 1;
                ClearCeremony_StartPartyShowcase();
                ClearCeremony_BeginOrbitCamera();
                ClearCeremony_HideTextPanels();
            } else {
                sCeremonyVariant = 2;
                ClearCeremony_StartTrophyShowcase();
                ClearCeremony_BeginTrophyCamera();
                ClearCeremony_HideTextPanels();
            }
            break;
    }
}

/**
 * Entry point for demo-scene mode 17, run after a cup is cleared (transitionFlags & 2) and after the Gym
 * Leader Castle clear //@src/game_state.c. Loads common_menu1_ui and cup_clear_ui (segment 3) and runs
 * the ceremony.
 */
s32 Stage_ShowClearCeremony(void) {
    StageContext* sp24;

    main_pool_push_state('DEMO');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    sp24 = StageContext_Allocate(0, 1, 3, 1, 2, 1);
    Font_Init(0x10, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_3000000, cup_clear_ui, 0);

    FRAGMENT_LOAD(FRAGMENT_PARTICLE_DATA_LIBRARY);

    ClearCeremony_LoadScene();
    StageContext_Activate(sp24);
    ClearCeremony_FadeInSequence();
    ClearCeremony_RunLoop();
    ClearCeremony_FadeOutSequence();
    StageContext_Deactivate();
    Font_Free();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('DEMO');

    return 0;
}
