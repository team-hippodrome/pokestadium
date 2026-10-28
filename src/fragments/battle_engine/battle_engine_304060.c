#include "battle_engine.h"
#include "game_state.h"
#include "src/model_animation.h"
#include "src/matrix.h"
#include "status.h"
#include "pokemon.h"

typedef struct unk_D_843859E0 {
    /* 0x00 */ u8 id;
    /* 0x04 */ f32 distance;
    /* 0x08 */ s16 rotationX;
    /* 0x0A */ s16 rotationY;
    /* 0x0C */ f32 verticalFieldOfView;
} ArenaCameraConfig; // size = 0x10

static ArenaCameraConfig sArenaConfig[] = {
    {
        0x00,
        2002.1199951171875f,
        1367,
        -20914,
        50.0f,
    },
    {
        0x01,
        1693.1199951171875f,
        183,
        -11871,
        50.0f,
    },
    {
        0x02,
        1934.97998046875f,
        1217,
        8182,
        50.0f,
    },
    {
        0x03,
        1733.780029296875f,
        397,
        6520,
        50.0f,
    },
    {
        0x04,
        1102.3900146484375f,
        1577,
        -3562,
        50.0f,
    },
    {
        0x05,
        1425.7900390625f,
        2285,
        -5775,
        50.0f,
    },
    {
        0x06,
        871.3599853515625f,
        1825,
        -8898,
        50.0f,
    },
    {
        0x07,
        1170.8199462890625f,
        4982,
        -6725,
        50.0f,
    },
    {
        0x11,
        1645.5699462890625f,
        7861,
        -122,
        50.0f,
    },
    {
        0x01,
        1741.530029296875f,
        6993,
        -6029,
        50.0f,
    },
    {
        0x06,
        1608.3199462890625f,
        6419,
        -16201,
        50.0f,
    },
    {
        0x00,
        1122.8599853515625f,
        2584,
        -14668,
        50.0f,
    },
    {
        0x00,
        1667.239990234375f,
        5360,
        4438,
        50.0f,
    },
    {
        0x00,
        1315.9599609375f,
        3001,
        -7883,
        50.0f,
    },
    {
        0x00,
        1575.4300537109375f,
        5834,
        10901,
        50.0f,
    },
    {
        0x00,
        719.8599853515625f,
        3047,
        -16384,
        50.0f,
    },
    {
        0x00,
        939.3900146484375f,
        2300,
        0,
        50.0f,
    },
    {
        0x01,
        560.239990234375f,
        773,
        -8863,
        50.0f,
    },
    {
        0x01,
        2002.1199951171875f,
        1367,
        -20914,
        30.0f,
    },
    {
        0x01,
        2002.1199951171875f,
        1367,
        -20914,
        30.0f,
    },
};

s32 BattleScene_AreBothParticipantModelsCompact(void) {
    if (((BattleScene_GetParticipantModelHeight(sActivePokemon[0]) + BattleScene_GetParticipantModelHeight(sActivePokemon[1])) <= 85.0f) &&
        (BattleScene_GetParticipantModelRestingHeight(sActivePokemon[0]) == 0.0f) && (BattleScene_GetParticipantModelRestingHeight(sActivePokemon[1]) == 0.0f) &&
        (sActivePokemon[0]->displayObject.position.y == 0.0f) && (sActivePokemon[1]->displayObject.position.y == 0.0f)) {
        return 1;
    }
    return 0;
}

/**
 * If the Pokemon on the current side is underground, selects the next b-roll shot and returns 1.
 * Returns 0 otherwise.
 */
s32 BattleScene_SkipBRollCameraShotIfUnderground(UNUSED ActivePokemon* arg0, CameraNode* camera) {
    ActivePokemonState* state = &sActivePokemon[sBattleScene.scene->sideBeingAnimated]->state;

    if ((state->unk_2D == 0x10) || ((state->flags & ACTIVE_UNDERGROUND) != 0)) {
        BattleScene_SelectNextBRollCameraShot(camera);
        return 1;
    }
    return 0;
}

s32 BattleScene_IsEitherActivePokemonUnderground(UNUSED ActivePokemon* arg0, UNUSED CameraNode* arg1) {
    ActivePokemonState* state1 = &sActivePokemon[sBattleScene.scene->sideBeingAnimated]->state;
    s32 idx = sBattleScene.scene->sideBeingAnimated == 0;
    ActivePokemonState* state2 = &sActivePokemon[idx]->state;

    if ((state1->flags & ACTIVE_UNDERGROUND) || (state2->flags & ACTIVE_UNDERGROUND)) {
        return 1;
    }
    return 0;
}

/**
 * B-roll shot 0, arena orbit: a wide shot at the height of the combatants (50 degree field of view)
 * that BattleScene_BRollCameraUpdateArenaOrbit rotates around the arena.
 */
void BattleScene_BRollCameraSetupArenaOrbit(CameraNode* camera) {
    BattleScene_ResetCameraFrameCounter();
    camera->perspective.verticalFieldOfView = 50.0f;
    if (BattleScene_AreBothParticipantModelsCompact() != 0) {
        BattleScene_SetCameraAtFromPosition(camera, s10UnitsUpFromCenter, 0, 0, 0.0f, 0.0f);
        BattleScene_SetCameraEyeAndStoreAngles(camera, s10UnitsUpFromCenter, 0x5B0, 0, 550.0f, 0.0f);
    } else {
        BattleScene_SetCameraAtFromPosition(camera, s100UnitsUpFromCenter, 0, 0, 0.0f, 0.0f);
        BattleScene_SetCameraEyeAndStoreAngles(camera, s100UnitsUpFromCenter, 0x5B0, 0, 550.0f, 0.0f);
    }
    camera->view.eyePosition.z *= 0.6;
    sBattleScene.scene->cameraCounter++;
}

/**
 * B-roll shot 2, rotating arena overview: an elevated shot of the arena (30 degree field of view)
 * that BattleScene_BRollCameraUpdateRotatingArenaOverview rotates around the arena.
 */
void BattleScene_BRollCameraSetupRotatingArenaOverview(CameraNode* camera) {
    BattleScene_ResetCameraFrameCounter();
    camera->perspective.verticalFieldOfView = 30.0f;
    if (BattleScene_AreBothParticipantModelsCompact() != 0) {
        BattleScene_SetCameraAtFromPosition(camera, s10UnitsUpFromCenter, 0, 0, 0.0f, 0.0f);
        BattleScene_SetCameraEyeAndStoreAngles(camera, s10UnitsUpFromCenter, 0x1A4E, 0, 1000.0f, 0.0f);
    } else {
        BattleScene_SetCameraAtFromPosition(camera, s100UnitsUpFromCenter, 0, 0, 0.0f, 0.0f);
        BattleScene_SetCameraEyeAndStoreAngles(camera, s100UnitsUpFromCenter, 0x1A4E, 0, 1000.0f, 0.0f);
    }
    sBattleScene.scene->cameraCounter++;
}

/**
 * B-roll shots 4 (side 0) and 6 (side 1), static zoomed out shot: a fixed camera behind the Pokemon
 * on the given side, at a random distance and height. Skipped if that Pokemon is underground.
 */
void BattleScene_BRollCameraSetupStaticZoomedOutShot(CameraNode* camera, s32 side) {
    UNUSED s32 pad;
    f32 pokemon_radius;
    f32 pokemon_height;
    s16 rotation_y_rng;
    ActivePokemonState* state;
    UNUSED s32 pad2;
    u16 height_rng;
    ActivePokemon* pokemon;

    state = &sActivePokemon[side]->state;
    BattleScene_ClearCameraOffsetVectors();
    BattleScene_SetActivePokemonIndex(side);
    pokemon = sActivePokemon[sBattleScene.scene->sideBeingAnimated];

    if (BattleScene_SkipBRollCameraShotIfUnderground(pokemon, camera) == 0) {
        BattleAnim_GetOwnerDefaultAnchorPosition(pokemon, &sBattleScene.scene->cameraStartingPosition);
        if (!(state->flags & 2)) {
            BattleScene_EnterStateRow(pokemon, 1);
        }

        pokemon_radius = BattleScene_GetParticipantModelRadiusScaled(pokemon) * 3.5f;
        height_rng = BattleAnim_RandomRange(3);
        pokemon_radius *= ((BattleScene_GetParticipantModelBoundsHeight(pokemon) * 0.5f * height_rng) + 1.0f);

        if (pokemon_radius > 325.0f) {
            pokemon_radius = 325.0f;
        }

        height_rng = BattleAnim_RandomRange(4);
        pokemon_height = BattleAnim_GetOwnerModelHeight(pokemon) * height_rng;
        rotation_y_rng = BattleAnim_RandomRange(180);

        if (state->flags & ACTIVE_AIRBORNE) {
            pokemon_height = 0.0f;
        }

        BattleScene_SetCameraAtFromAnchor(pokemon, camera, sBattleScene.scene->cameraStartingPosition, 0, 0, 0, 0.0f);
        BattleScene_SetCameraEyeFromPosition(camera, sBattleScene.scene->cameraStartingPosition, 0, 0x4000 - (rotation_y_rng * 182), pokemon_radius, pokemon_height);
        BattleScene_CameraPresetNop(camera);
        BattleScene_UpdateCameraModelOffset(pokemon, camera);

        if (pokemon->displayObject.objectType == ONIX) {
            camera->perspective.verticalFieldOfView = 50.0f;
        } else {
            camera->perspective.verticalFieldOfView = 30.0f;
        }
        sBattleScene.scene->cameraCounter++;
    }
}

/**
 * B-roll shots 8 (side 0) and 10 (side 1), zoom shot: loads a camera preset for the Pokemon on the given
 * side that BattleScene_BRollCameraUpdateZoomShot animates. Skipped if that Pokemon is underground.
 */
void BattleScene_BRollCameraSetupZoomShot(CameraNode* camera, s32 side) {
    UNUSED s32 pad[2];
    DioramaAnimData* camera_preset;
    s32 rng;
    ActivePokemon* pokemon;

    BattleScene_ClearCameraOffsetVectors();
    BattleScene_SetActivePokemonIndex(side);
    pokemon = sActivePokemon[sBattleScene.scene->sideBeingAnimated];

    if (BattleScene_SkipBRollCameraShotIfUnderground(pokemon, camera) == 0) {
        if (pokemon->state.flags & ACTIVE_AIRBORNE) {
            camera_preset = &D_84384888[0];
        } else {
            rng = BattleAnim_RandomRange(5);
            if (rng >= 5) {
                rng = 0;
            }
            camera_preset = &D_843847EC[rng];
        }
        camera->perspective.verticalFieldOfView = camera_preset->verticalFieldOfView;
        BattleAnim_GetOwnerDefaultAnchorPosition(pokemon, &gCameraPreset.startingPosition);
        BattleScene_LoadCameraPreset(camera_preset, camera, pokemon);
        BattleScene_ApplyCameraPreset(camera, pokemon, 1);
        BattleScene_CameraPresetNop(camera);
        sBattleScene.scene->cameraCounter++;
    }
}

/**
 * B-roll shots 12 (side 0) and 14 (side 1), send-out shot: positions the camera with
 * BattleScene_SetCameraSendOutShot for the given side. Skipped if that Pokemon is underground.
 */
void BattleScene_BRollCameraSetupSendOutShot(CameraNode* camera, s32 side) {
    camera->perspective.verticalFieldOfView = 30.0f;
    BattleScene_SetActivePokemonIndex(side);
    if (BattleScene_SkipBRollCameraShotIfUnderground(sActivePokemon[side], camera) == 0) {
        BattleScene_SetCameraSendOutShot(camera, side);
        sBattleScene.scene->cameraCounter++;
    }
}

/**
 * B-roll shot 16, height shot on the first side: used when the two Pokemon are at different heights.
 * The side is unk_54 - 1; BattleScene_BRollCameraUpdateHeightShotFirstSide then continues with the other side.
 * Skipped if either Pokemon is underground.
 */
void BattleScene_BRollCameraSetupHeightShotFirstSide(CameraNode* camera) {
    BattleScene_SetActivePokemonIndex(sBattleScene.scene->unk_54 - 1);
    if (BattleScene_IsEitherActivePokemonUnderground(sActivePokemon[sBattleScene.scene->sideBeingAnimated], camera) != 0) {
        sBattleScene.scene->unk_54 = 0;
        BattleScene_SelectNextBRollCameraShot(camera);
    } else {
        BattleScene_SetCameraActivePokemonHeightShot(camera, sBattleScene.scene->sideBeingAnimated);
        sBattleScene.scene->cameraCounter++;
    }
}

/**
 * B-roll shot 18, height shot on the second side: the side swapped in by
 * BattleScene_BRollCameraUpdateHeightShotFirstSide. Skipped if either Pokemon is underground.
 */
void BattleScene_BRollCameraSetupHeightShotSecondSide(CameraNode* camera) {
    BattleScene_SetActivePokemonIndex(sBattleScene.scene->sideBeingAnimated);
    sBattleScene.scene->unk_54 = 0;
    if (BattleScene_IsEitherActivePokemonUnderground(sActivePokemon[sBattleScene.scene->sideBeingAnimated], camera) != 0) {
        BattleScene_SelectNextBRollCameraShot(camera);
    } else {
        BattleScene_SetCameraActivePokemonHeightShot(camera, sBattleScene.scene->sideBeingAnimated);
        sBattleScene.scene->cameraCounter++;
    }
}

/**
 * B-roll shot 20, static arena overview: a fixed shot of the arena chosen by session mode
 * (or rule category in the attract demo), using sArenaConfig.
 */
void BattleScene_BRollCameraSetupStaticArenaOverview(CameraNode* camera) {
    s16 arena_index;

    BattleScene_ResetCameraFrameCounter();
    if (gSessionContext.mode == SESSION_MODE_ATTRACT_DEMO_BATTLE) {
        arena_index = sArenaConfig[gSessionContext.ruleCategory].id;
        if (gSessionContext.ruleCategory == RULE_CATEGORY_GYM_LEADER_CASTLE) {
            arena_index += gSessionContext.modeSelection;
        }
    } else {
        arena_index = sArenaConfig[gSessionContext.mode].id;
        if (gSessionContext.mode == SESSION_MODE_GYM_LEADER_CASTLE) {
            arena_index += gSessionContext.modeSelection;
        }
    }
    camera->perspective.verticalFieldOfView = sArenaConfig[arena_index].verticalFieldOfView;
    Vec3f_SetComponentsDuplicate(&camera->view.targetPosition, 0.0f, 10.0f, 0.0f);
    Camera_ComputeEyeFromAngles(&camera->view.targetPosition, &camera->view.eyePosition, sArenaConfig[arena_index].distance, sArenaConfig[arena_index].rotationX,
                  sArenaConfig[arena_index].rotationY);
    sBattleScene.scene->cameraCounter++;
}

/**
 * B-roll shots 22 (side 0) and 24 (side 1), static close up: a fixed 60 degree field of view shot of the
 * Pokemon on the given side. Skipped if that Pokemon is underground, a Substitute doll, or frozen.
 */
void BattleScene_BRollCameraSetupStaticCloseUp(CameraNode* camera, s32 side) {
    UNUSED s32 pad[5];
    s32 camera_adjustment_index;
    ActivePokemon* pokemon;
    f32 target_x;
    s16 target_rotation_x;
    s16 target_rotation_y;
    f32 target_y;

    BattleScene_SetCameraClipPlanes(camera, 10.0f, 12800.0f);
    camera->perspective.verticalFieldOfView = 60.0f;
    BattleScene_SetActivePokemonIndex(side);
    pokemon = sActivePokemon[side];

    if (BattleScene_SkipBRollCameraShotIfUnderground(pokemon, camera) == 0) {
        if ((pokemon->displayObject.objectType == SUBSTITUTE_DOLL) || (pokemon->state.flags & ACTIVE_FREEZE)) {
            BattleScene_SelectNextBRollCameraShot(camera);
            return;
        }

        if ((pokemon->displayObject.objectType == ONIX) && (side == 1)) {
            camera_adjustment_index = 0x97;
        } else {
            camera_adjustment_index = (pokemon->displayObject.objectType - 1) & 0xFF;
        }

        target_y = (sSideCameraTransform[side]->coordinates.y + pokemon->displayObject.position.y) + sModelCameraAdjustments[camera_adjustment_index].z;
        target_x = ((sSideCameraTransform[side]->coordinates.z + sModelCameraAdjustments[camera_adjustment_index].y) * pokemon->facingDirection) + pokemon->displayObject.position.x;

        Vec3f_SetComponentsDuplicate(&camera->view.targetPosition, target_x, target_y, pokemon->displayObject.position.z);

        target_x = sSideCameraTransform[sBattleScene.scene->sideBeingAnimated]->coordinates.x * sModelCameraAdjustments[camera_adjustment_index].x;
        target_rotation_x = sSideCameraTransform[sBattleScene.scene->sideBeingAnimated]->rotationX;
        target_rotation_y = (pokemon->facingDirection * sSideCameraTransform[sBattleScene.scene->sideBeingAnimated]->rotationY) + BattleScene_GetParticipantFacingAngle(pokemon);

        Camera_ComputeEyeFromAngles(&camera->view.targetPosition, &camera->view.eyePosition, target_x, target_rotation_x, target_rotation_y);
        sBattleScene.scene->cameraCounter++;
    }
}

/**
 * Selects the next b-roll shot and runs its setup function. Shots follow the order in the table below,
 * except that shot 16 is chosen when unk_54 is set and the Pokemon are at different heights. Even
 * cameraCounter values are shot setups and odd values are the matching per-frame updates in
 * BattleScene_UpdateBRollCameraSequence.
 */
void BattleScene_SelectNextBRollCameraShot(CameraNode* camera) {
    UNUSED s32 pad[2];
    s32 sp20[] = {
        // D_84385B20
        0x00000000, 0x00000008, 0x00000004, 0x00000014, 0x0000000A,
        0x00000006, 0x00000002, 0x00000016, 0x00000018, 0x000000FF,
    };

    BattleScene_SetCameraClipPlanes(camera, 20.0f, 12800.0f);
    camera->perspective.verticalFieldOfView = 30.0f;
    if ((sBattleScene.scene->unk_54 != 0) && (BattleScene_AreActivePokemonHeightsSeparated(camera) != 0)) {
        sBattleScene.scene->cameraCounter = 0x10;
    } else {
        sBattleScene.scene->unk_54 = 0;
        if (sp20[sBattleScene.scene->unk_0C] == 0xFF) {
            sBattleScene.scene->unk_0C = 0;
        }
        sBattleScene.scene->cameraCounter = sp20[sBattleScene.scene->unk_0C];
        sBattleScene.scene->unk_0C++;
    }

    Battle_ResetModelToIdleAnim(*sActivePokemon);
    Battle_ResetModelToIdleAnim(sActivePokemon[1]);
    BattleScene_ResetCameraFrameCounter();

    switch (sBattleScene.scene->cameraCounter) {
        case 0:
            // Rotation around Arena circle
            BattleScene_BRollCameraSetupArenaOrbit(camera);
            break;

        case 2:
            // rotating arena overview
            BattleScene_BRollCameraSetupRotatingArenaOverview(camera);
            break;

        case 4:
            // static zoomed out shot on side 1
            BattleScene_BRollCameraSetupStaticZoomedOutShot(camera, 0);
            break;

        case 6:
            // static zoomed out shot on side 2
            BattleScene_BRollCameraSetupStaticZoomedOutShot(camera, 1);
            break;

        case 8:
            // zoom in/out on pokemon on side 1
            BattleScene_BRollCameraSetupZoomShot(camera, 0);
            break;

        case 10:
            // zoom in/out on pokemon on side 2
            BattleScene_BRollCameraSetupZoomShot(camera, 1);
            break;

        // Shots 12, 14, 16, and 18 are not entries of the shot-order table above. This function selects
        // 16 directly; 18 is entered from BattleScene_BRollCameraUpdateHeightShotFirstSide.
        // Nothing in this function selects 12 or 14.
        case 12:
            BattleScene_BRollCameraSetupSendOutShot(camera, 0);
            break;

        case 14:
            BattleScene_BRollCameraSetupSendOutShot(camera, 1);
            break;

        case 16:
            BattleScene_BRollCameraSetupHeightShotFirstSide(camera);
            break;

        case 18:
            BattleScene_BRollCameraSetupHeightShotSecondSide(camera);
            break;

        case 20:
            // static arena overview
            BattleScene_BRollCameraSetupStaticArenaOverview(camera);
            break;

        case 22:
            // static close up on side 1
            BattleScene_BRollCameraSetupStaticCloseUp(camera, 0);
            break;

        case 24:
            // static close up on side 2
            BattleScene_BRollCameraSetupStaticCloseUp(camera, 1);
            break;
    }
}

/**
 * Per-frame update for b-roll shot 0 (arena orbit): rotates the camera around the arena, then selects the next shot.
 */
void BattleScene_BRollCameraUpdateArenaOrbit(CameraNode* camera) {
    camera->perspective.verticalFieldOfView = 50.0f;
    if (BattleScene_ApproachCameraRotationY(camera, 0xB6, 0x78DC) != 0) {
        BattleScene_SelectNextBRollCameraShot(camera);
    } else {
        camera->view.eyePosition.z *= 0.6;
    }
}

/**
 * Per-frame update for b-roll shot 2 (rotating arena overview): rotates the camera around the arena,
 * keeping the field of view set by the setup, then selects the next shot.
 */
void BattleScene_BRollCameraUpdateRotatingArenaOverview(CameraNode* camera) {
    if (BattleScene_ApproachCameraRotationY(camera, 0xB6, 0x78DC) != 0) {
        BattleScene_SelectNextBRollCameraShot(camera);
    }
}

/**
 * Per-frame update for b-roll shots 4 and 6 (static zoomed out shot): holds until the Pokemon's animation
 * finishes, or for 90 frames if it is asleep, frozen, or a Substitute doll.
 */
void BattleScene_BRollCameraUpdateStaticZoomedOutShot(CameraNode* camera) {
    UNUSED s32 pad;
    ActivePokemonState* state = &sActivePokemon[sBattleScene.scene->sideBeingAnimated]->state;

    if ((state->unk_2D == 0x10) || ((state->flags & ACTIVE_UNDERGROUND) != 0)) {
        BattleScene_SelectNextBRollCameraShot(camera);
        return;
    }

    if (!(state->flags & ACTIVE_AIRBORNE)) {
        BattleScene_ApplyOwnerFramingOnly(camera, sActivePokemon[sBattleScene.scene->sideBeingAnimated]);
    }

    if ((state->flags & ACTIVE_SLEEP) || (state->flags & ACTIVE_FREEZE) || (sActivePokemon[sBattleScene.scene->sideBeingAnimated]->displayObject.objectType == SUBSTITUTE_DOLL)) {
        if (BattleScene_AdvanceCameraFrameCounter(90) != 0) {
            BattleScene_SelectNextBRollCameraShot(camera);
        }
    } else if (ModelAnim_IsAnimationDone(&sActivePokemon[sBattleScene.scene->sideBeingAnimated]->displayObject) != 0) {
        BattleScene_SelectNextBRollCameraShot(camera);
    }
}

/**
 * Per-frame update for b-roll shots 8 and 10 (zoom shot): animates the camera preset for at most 90 frames,
 * then selects the next shot.
 */
void BattleScene_BRollCameraUpdateZoomShot(CameraNode* camera) {
    ActivePokemon* state = sActivePokemon[sBattleScene.scene->sideBeingAnimated];

    if ((state->state.unk_2D == 0x10) || (state->state.flags & ACTIVE_UNDERGROUND)) {
        BattleScene_SelectNextBRollCameraShot(camera);
    } else if ((BattleScene_AdvanceCameraFrameCounter(90) != 0) || (BattleScene_AnimateCameraPreset(camera, state, 1, 0) != 0)) {
        BattleScene_SelectNextBRollCameraShot(camera);
    }
}

/**
 * Per-frame update for b-roll shots 12 and 14 (send-out shot): holds at a 60 degree field of view for 60 frames.
 */
void BattleScene_BRollCameraUpdateSendOutShot(CameraNode* camera) {
    ActivePokemonState* ptr = &sActivePokemon[sBattleScene.scene->sideBeingAnimated]->state;

    if ((ptr->unk_2D == 0x10) || (ptr->flags & ACTIVE_UNDERGROUND)) {
        BattleScene_SelectNextBRollCameraShot(camera);
    } else {
        camera->perspective.verticalFieldOfView = 60.0f;
        if (BattleScene_AdvanceCameraFrameCounter(60) != 0) {
            BattleScene_SelectNextBRollCameraShot(camera);
        }
    }
}

/**
 * Per-frame update for b-roll shot 16 (height shot, first side): once the camera has approached the Pokemon,
 * swaps to the other side and starts shot 18.
 */
void BattleScene_BRollCameraUpdateHeightShotFirstSide(CameraNode* camera) {
    if (BattleScene_ApproachCameraActivePokemonHeightShot(camera, sBattleScene.scene->sideBeingAnimated) != 0) {
        s32 idx = sBattleScene.scene->sideBeingAnimated == 0;

        sBattleScene.scene->sideBeingAnimated = idx;
        sBattleScene.scene->cameraCounter = 0x12;
        BattleScene_BRollCameraSetupHeightShotSecondSide(camera);
    }
}

/**
 * Per-frame update for b-roll shot 18 (height shot, second side): selects the next shot once the camera
 * has approached the Pokemon.
 */
void BattleScene_BRollCameraUpdateHeightShotSecondSide(CameraNode* camera) {
    if (BattleScene_ApproachCameraActivePokemonHeightShot(camera, sBattleScene.scene->sideBeingAnimated) != 0) {
        BattleScene_SelectNextBRollCameraShot(camera);
    }
}

/**
 * Per-frame update for b-roll shot 20 (static arena overview): holds for 120 frames.
 */
void BattleScene_BRollCameraUpdateStaticArenaOverview(CameraNode* camera) {
    if (BattleScene_AdvanceCameraFrameCounter(0x78) != 0) {
        BattleScene_SelectNextBRollCameraShot(camera);
    }
}

/**
 * Per-frame update for b-roll shots 22 and 24 (static close up): holds for 60 frames at a 60 degree field of view.
 */
void BattleScene_BRollCameraUpdateStaticCloseUp(CameraNode* camera) {
    BattleScene_SetCameraClipPlanes(camera, 10.0f, 12800.0f);
    camera->perspective.verticalFieldOfView = 60.0f;
    if (BattleScene_AdvanceCameraFrameCounter(0x3C) != 0) {
        BattleScene_SelectNextBRollCameraShot(camera);
    }
}

/**
 * Per-frame update while unk_1C is 1 (b-roll camera mode): runs the action selection phase and
 * dispatches to the update function of the current b-roll shot (odd cameraCounter values).
 */
void BattleScene_UpdateBRollCameraSequence(CameraNode* camera) {
    if ((gSessionContext.mode == SESSION_MODE_ATTRACT_DEMO_BATTLE) && (sBattleScene.scene->unk_1C == 1)) {
        BattleScene_UpdateActionSelectPhase();
    } else if (sBattleScene.scene->unk_1C == 1) {
        if ((sBattleScene.scene->unk_30 == 0) && (sActivePokemon[0]->thumbnail.camera->viewport.x == 0x17) &&
            (sActivePokemon[1]->thumbnail.camera->viewport.x == 0x103)) {
            BattleScene_UpdateActionSelectPhase();
        } else if ((sBattleScene.scene->unk_30 == 0) && (sActivePokemon[0]->thumbnail.camera->viewport.x < 0xE) &&
                   (sActivePokemon[1]->thumbnail.camera->viewport.x >= 0x10D)) {
            BattleScene_ResetActionSelectionTimer();
        }
    }

    switch (sBattleScene.scene->cameraCounter) {
        case 1:
            BattleScene_BRollCameraUpdateArenaOrbit(camera);
            break;

        case 3:
            BattleScene_BRollCameraUpdateRotatingArenaOverview(camera);
            break;

        case 5:
            BattleScene_BRollCameraUpdateStaticZoomedOutShot(camera);
            break;

        case 7:
            BattleScene_BRollCameraUpdateStaticZoomedOutShot(camera);
            break;

        case 9:
            BattleScene_BRollCameraUpdateZoomShot(camera);
            break;

        case 11:
            BattleScene_BRollCameraUpdateZoomShot(camera);
            break;

        case 13:
            BattleScene_BRollCameraUpdateSendOutShot(camera);
            break;

        case 15:
            BattleScene_BRollCameraUpdateSendOutShot(camera);
            break;

        case 17:
            BattleScene_BRollCameraUpdateHeightShotFirstSide(camera);
            break;

        case 19:
            BattleScene_BRollCameraUpdateHeightShotSecondSide(camera);
            break;

        case 21:
            BattleScene_BRollCameraUpdateStaticArenaOverview(camera);
            break;

        case 23:
            BattleScene_BRollCameraUpdateStaticCloseUp(camera);
            break;

        case 25:
            BattleScene_BRollCameraUpdateStaticCloseUp(camera);
            break;

        case 50:
            BattleScene_SelectNextBRollCameraShot(camera);
            break;
    }
}
