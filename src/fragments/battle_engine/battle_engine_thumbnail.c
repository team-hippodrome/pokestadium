#include "battle_engine.h"
#include "src/model_animation.h"
#include "src/model_renderer.h"
#include "src/model_renderer_callbacks.h"
#include "src/matrix.h"
#include "status.h"

/*
    Cause the Pokemon's idle animation to play in the thumbnail.
    The thumbnails in battles are not loaded icons, they are a frozen model of the Pokemon's idle animation
    You can see this by disabling the check in Battle_RenderActivePokemonThumbnail in @src/fragments/battle_engine/battle_engine_main.c

    Speculation: Some models changed between Stadium 0 and Stadium 1 (most apparent with Nidoking)
    They could have choosen to render the thumbnails this way in case models changed mid-development.
*/
void BattleThumbnail_TriggerOwnerModelAnimation(ActivePokemon* active_pokemon) {
    ModelAnim_SetFrame(&active_pokemon->thumbnail.displayObject, active_pokemon->thumbnail.idleTimer);
    ModelAnim_SetAnimation(&active_pokemon->thumbnail.displayObject, active_pokemon->thumbnail.idleAnimation);
    BattleThumbnail_SetFlagToUpdatePokemonThumbnail(active_pokemon->thumbnail.camera);
}

/*
    Updates upon the entering the move phase for a single pokemon
*/
void BattleThumbnail_ClearParticipantOwnerIdleFlags(ActivePokemon* active_pokemon) {
    active_pokemon->thumbnail.showing = 0;
}

/*
    Updates upon the exiting the move phase for a single pokemon
*/
void BattleThumbnail_SetParticipantOwnerIdleFlags(ActivePokemon* active_pokemon) {
    active_pokemon->thumbnail.showing = 1;
}

/*
    Updates upon the entering the move phase for both pokemon
*/
void BattleThumbnail_ClearBothOwnerIdleFlags(void) {
    sActivePokemon[0]->thumbnail.showing = 0;
    sActivePokemon[1]->thumbnail.showing = 0;
}

/*
    Updates upon the exiting the move phase for both pokemon
*/
void BattleThumbnail_SetBothOwnerIdleFlags(void) {
    sActivePokemon[0]->thumbnail.showing = 1;
    sActivePokemon[1]->thumbnail.showing = 1;
}

/*
    Sets flag to update the thumbnail.
    Battle_RenderActivePokemonThumbnail in @src/fragments/battle_engine/battle_engine_main.c
*/
void BattleThumbnail_SetFlagToUpdatePokemonThumbnail(CameraNode* camera) {
    camera->node.flags |= 0x10;
}

/*
    Loads the camera framing constants for the owner's thumbnail.
*/
void BattleThumbnail_LoadOwnerCameraFramingConstants(ActivePokemon* active_pokemon) {
    s32 side;

    if (active_pokemon == sActivePokemon[0]) {
        side = 0;
    } else {
        side = 1;
    }

    active_pokemon->thumbnail.camera->viewport.width = sSideThumbnailSettings[side].width;
    active_pokemon->thumbnail.camera->viewport.x = sSideThumbnailSettings[side].x;
    active_pokemon->thumbnail.camera->viewport.y = sSideThumbnailSettings[side].y;

    active_pokemon->thumbnail.side = sSideThumbnailSettings[side].unk_14;

    active_pokemon->thumbnail.camera->perspective.nearPlane = sSideThumbnailSettings[side].nearPlane;
    active_pokemon->thumbnail.camera->perspective.farPlane = sSideThumbnailSettings[side].farPlane;
    active_pokemon->thumbnail.camera->perspective.verticalFieldOfView = sSideThumbnailSettings[side].verticalFieldOfView;
}

/*
    Sets up the camera framing for both pokemons's thumbnails.
*/
void BattleThumbnail_SetupOwnerCameraFraming(ActivePokemon* side1_pokemon, ActivePokemon* side2_pokemon) {
    if (sBattleScene.scene->unk_1C == 1) {
        BattleThumbnail_SetBothOwnerIdleFlags();
    } else {
        BattleThumbnail_ClearBothOwnerIdleFlags();
    }
    BattleThumbnail_LoadOwnerCameraFramingConstants(side1_pokemon);
    BattleThumbnail_LoadOwnerCameraFramingConstants(side2_pokemon);
}

/*
    Nop
*/
void BattleThumbnail_OwnerCameraLifecycleNop(void) {
}

/*
    Applies the sets coordinates for the Camera that generates the owner's thumbnail.
*/
void BattleThumbnail_ApplyOwnerCameraFraming(ActivePokemon* active_pokemon) {
    s32 side;
    CameraNode* camera;
    f32 tmp_z;
    f32 tmp_y;

    camera = active_pokemon->thumbnail.camera;
    side = BattleScene_GetParticipantSideIndex(active_pokemon);

    active_pokemon->thumbnail.rotationX = sSidePresentationTransform[side]->rotationX;
    if (sSidePresentationTransform[side]->unk_10 == 0xFF) {
        active_pokemon->thumbnail.rotationY = sSidePresentationTransform[side]->rotationY * active_pokemon->thumbnail.side;
        tmp_z = sSidePresentationTransform[side]->coordinates.z * active_pokemon->thumbnail.side;
    } else {
        active_pokemon->thumbnail.rotationY = sSidePresentationTransform[side]->rotationY;
        tmp_z = sSidePresentationTransform[side]->coordinates.z;
    }

    active_pokemon->thumbnail.z = tmp_z;
    active_pokemon->thumbnail.x = sSidePresentationTransform[side]->coordinates.x;
    active_pokemon->thumbnail.idleTimer = sSidePresentationTransform[side]->idleTimer;
    tmp_y = sSidePresentationTransform[side]->coordinates.y;
    active_pokemon->thumbnail.y = tmp_y;

    BattleScene_SetCameraAtAndEyeFromAngles(camera, active_pokemon->thumbnail.displayObject.position.x + active_pokemon->thumbnail.z, active_pokemon->thumbnail.displayObject.position.y + tmp_y,
                  active_pokemon->thumbnail.displayObject.position.z, active_pokemon->thumbnail.x, active_pokemon->thumbnail.rotationX, active_pokemon->thumbnail.rotationY);
}

/*
    Updates the camera coordinates for the owner's thumbnail.
*/
void BattleThumbnail_TickOwnerCameraFraming(ActivePokemon* active_pokemon) {
    BattleThumbnail_ApplyOwnerCameraFraming(active_pokemon);
}

/*
    Initializes the camera coordinates for both owner's thumbnails.
*/
void BattleThumbnail_InitializeOwnerCameraFraming(CameraNode* side1_camera_node, ActivePokemon* side1_pokemon, CameraNode* side2_camera_node, ActivePokemon* side2_pokemon) {
    side1_pokemon->thumbnail.camera = side1_camera_node;
    side2_pokemon->thumbnail.camera = side2_camera_node;
    BattleThumbnail_SetupOwnerCameraFraming(side1_pokemon, side2_pokemon);
    BattleThumbnail_TickOwnerCameraFraming(side1_pokemon);
    BattleThumbnail_TickOwnerCameraFraming(side2_pokemon);
}

/*
    Handles the slide-in animation for the owner's thumbnail for when move selection is ongoing.
*/
void BattleThumbnail_TickOwnerSlideIn(ActivePokemon* active_pokemon, s32 delta) {
    s16 var_a0; // FAKE
    CameraNode* camera_node;

    active_pokemon->thumbnail.camera->node.flags |= 1;
    active_pokemon->thumbnail.displayObject.node.flags |= 1;

    if (active_pokemon->thumbnail.side == 1) {
        // This Substitute condition can never actually fire as switching out a pokemon destroys the substitute.
        if ((active_pokemon->thumbnail.camera->viewport.x == -72) && !(active_pokemon->state.flags & ACTIVE_SUBSTITUTE)) {
            BattleThumbnail_TriggerOwnerModelAnimation(active_pokemon);
        }

        if ((ModelRenderer_IsReady(0) != 0) && (active_pokemon->thumbnail.status == 0)) {
            active_pokemon->thumbnail.status = 2;
            ModelRenderer_OnSlotReady(0);
        }
        active_pokemon->thumbnail.camera->viewport.x = Math_StepToS32(active_pokemon->thumbnail.camera->viewport.x, 0x17, delta, delta);
    } else {
        if ((active_pokemon->thumbnail.camera->viewport.x == 351) && !(active_pokemon->state.flags & ACTIVE_SUBSTITUTE)) {
            BattleThumbnail_TriggerOwnerModelAnimation(active_pokemon);
        }

        active_pokemon->thumbnail.camera->viewport.x = Math_StepToS32(active_pokemon->thumbnail.camera->viewport.x, 0x103, delta, delta);

        if ((ModelRenderer_IsReady(1) != 0) && (active_pokemon->thumbnail.status == 0)) {
            active_pokemon->thumbnail.status = 2;
            ModelRenderer_OnSlotReady(1);
        }
    }
}

/*
    Handles the slide-in animation for the owner's thumbnail for moves selection is over
*/
void BattleThumbnail_TickOwnerSlideOut(ActivePokemon* active_pokemon, s32 delta) {
    if (active_pokemon->thumbnail.side == 1) {
        active_pokemon->thumbnail.status = 0;
        active_pokemon->thumbnail.camera->viewport.x = Math_StepToS32(active_pokemon->thumbnail.camera->viewport.x, -72, delta, delta);
        if (active_pokemon->thumbnail.camera->viewport.x == -72) {
            active_pokemon->thumbnail.camera->node.flags &= ~1;
            active_pokemon->thumbnail.displayObject.node.flags &= ~1;
        }
    } else {
        active_pokemon->thumbnail.status = 0;
        active_pokemon->thumbnail.camera->viewport.x = Math_StepToS32(active_pokemon->thumbnail.camera->viewport.x, 351, delta, delta);
        if (active_pokemon->thumbnail.camera->viewport.x == 351) {
            active_pokemon->thumbnail.camera->node.flags &= ~1;
            active_pokemon->thumbnail.displayObject.node.flags &= ~1;
        }
    }
}

/*
    Handles the slide animation for the owner's thumbnail.
*/
void BattleThumbnail_TickOwnerSlideAnimation(ActivePokemon* active_pokemon) {
    switch (active_pokemon->thumbnail.showing) {
        case 0:
            BattleThumbnail_TickOwnerSlideOut(active_pokemon, 9);
            break;

        case 1:
            BattleThumbnail_TickOwnerSlideIn(active_pokemon, 9);
            break;
    }
}

/*
    Avoids drawing the thumbnails in the attract demo
*/
void BattleThumbnail_TickBothOwnerSlideAnimations(ActivePokemon* side1_pokemon, ActivePokemon* side2_pokemon) {
    if (gSessionContext.mode != SESSION_MODE_ATTRACT_DEMO_BATTLE) {
        BattleThumbnail_TickOwnerSlideAnimation(side1_pokemon);
        BattleThumbnail_TickOwnerSlideAnimation(side2_pokemon);
    }
}
/*
    Dispatches the thumbnail's rendering based on the current of the battle
*/
s32 BattleThumbnail_DispatchOwnerCameraLifecycle(s32 state, CameraNode* side1_camera_node, CameraNode* side2_camera_node) {
    switch (state) {
        case 0:
            BattleThumbnail_OwnerCameraLifecycleNop();
            break;

        case 1:
        case 2:
            BattleThumbnail_TickBothOwnerSlideAnimations(sActivePokemon[0], sActivePokemon[1]);
            break;

        case 3:
            BattleThumbnail_InitializeOwnerCameraFraming(side1_camera_node, sActivePokemon[0], side2_camera_node, sActivePokemon[1]);
            break;
    }
    return 0;
}
