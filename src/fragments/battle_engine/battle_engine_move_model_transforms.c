#include "battle_engine.h"
#include "src/model_animation.h"
#include "src/model_renderer.h"
#include "src/matrix.h"
#include "pokemon.h"

static f32 sModelTransformConstants[] = { 1.0f, 2.0f };

#define NINETY_DEGREES 0x4000

void BattleAnim_InitSwayOffset(ActivePokemon* pokemon, f32 y, s16 theta) {
    pokemon->y = y;
    pokemon->modelTransform.rotation.z = COSS(theta) * y;
    pokemon->modelTransform.rotation.y = SINS(theta) * y;
    pokemon->rotationZ = 0.01f;
}


void BattleAnim_UpdateTransform(ActivePokemon* owner, ActivePokemon* animated_pokemon, f32 height, f32 delta_y, s32 delta_theta) {
    f32 temp_attacker_rotation_z;
    f32 temp_attacker_y1;
    f32 base_y_rotation;
    UNUSED s32 pad;
    Vec3f rotation;
    UNUSED s32 pad2;
    f32 z_rotation_factor;
    f32 y_rotation_factor;
    f32 temp_attacker_y2;

    temp_attacker_rotation_z = animated_pokemon->rotationZ;
    temp_attacker_y1 = animated_pokemon->y;
    base_y_rotation = 18.0f;
    animated_pokemon->y = Math_StepToF(temp_attacker_y1, height, delta_y, delta_y);

    temp_attacker_y2 = animated_pokemon->y;
    if (height <= temp_attacker_y2) {
        if (&animated_pokemon->modelTransform.rotation) {}
        temp_attacker_y2 = 0.0f;
    }

    rotation.z = COSS(delta_theta) * temp_attacker_y2;
    rotation.y = SINS(delta_theta) * temp_attacker_y2;

    z_rotation_factor = -temp_attacker_rotation_z * ((temp_attacker_y1 * animated_pokemon->modelTransform.rotation.z) + rotation.z);
    y_rotation_factor = -base_y_rotation - (temp_attacker_rotation_z * ((temp_attacker_y1 * animated_pokemon->modelTransform.rotation.y) + rotation.y));

    animated_pokemon->modelTransform.rotation.z = (animated_pokemon->modelTransform.rotation.z + rotation.z + (z_rotation_factor * 0.05f));
    animated_pokemon->modelTransform.rotation.y = (animated_pokemon->modelTransform.rotation.y + rotation.y + (y_rotation_factor * 0.05f));

    rotation.y = animated_pokemon->modelTransform.rotation.y;
    animated_pokemon->modelTransform.rotation.x = 0.0f;

    BattleAnim_Vec3fSetPolarXZ(&rotation, animated_pokemon->modelTransform.rotation.z, owner->displayObject.rotation.y + NINETY_DEGREES);
    Vec3f_AddInPlace(&owner->displayObject.position, &rotation);
}

void BattleAnim_UpdateSwayPhysicsToTarget(UNUSED ActivePokemon* owner, ActivePokemon* animated_object, f32 target, f32 delta, s32 theta0, s32 theta1) {
    f32 object_z;
    f32 object_y;
    f32 adjustment;
    UNUSED s32 pad;
    Vec3f rotation;
    UNUSED s32 pad2;
    f32 adjusted_z_rotation;
    f32 adjusted_y_rotation;
    f32 temp_object_y;

    object_z = animated_object->rotationZ;
    object_y = animated_object->y;
    adjustment = 20.0f;
    animated_object->y = Math_StepToF(object_y, target, delta, delta);

    temp_object_y = animated_object->y;
    if (target <= temp_object_y) {
        if (&animated_object->modelTransform.rotation) {}
        temp_object_y = 0.0f;
    }

    rotation.z = COSS(theta0) * temp_object_y;
    rotation.y = SINS(theta0) * temp_object_y;

    adjusted_z_rotation = -object_z * ((object_y * animated_object->modelTransform.rotation.z) + rotation.z);
    adjusted_y_rotation = -adjustment - (object_z * ((object_y * animated_object->modelTransform.rotation.y) + rotation.y));

    animated_object->modelTransform.rotation.z = (animated_object->modelTransform.rotation.z + rotation.z + (adjusted_z_rotation * 0.05f));
    animated_object->modelTransform.rotation.y = (animated_object->modelTransform.rotation.y + rotation.y + (adjusted_y_rotation * 0.05f));
    rotation.y = animated_object->modelTransform.rotation.y;
    animated_object->modelTransform.rotation.x = 0.0f;

    BattleAnim_Vec3fSetPolarXZ(&rotation, animated_object->modelTransform.rotation.z, theta1);
    Vec3f_AddInPlace(&animated_object->pokeBall.position, &rotation);
}

void BattleScene_HideParticipantSecondaryModels(ActivePokemon* active_pokemon) {
    s32 i;

    for (i = 0; i < 2; i++) {
        Vec3f_SetComponentsDuplicate(&active_pokemon->afterimages[i].displayObject.scale, 1.0f, 1.0f, 1.0f);
        active_pokemon->afterimages[i].displayObject.node.flags &= ~1;
        active_pokemon->afterimages[i].minimized = 0;
    }
}

void BattleScene_ShowSecondaryModelsAtMainTransform(ActivePokemon* pokemon) {
    s32 i;
    s16 sp64[] = { 0x0, 0xE38, 0xF1C8 }; // D_84384608

    for (i = 0; i < 2; i++) {
        pokemon->afterimages[i].unk_16C = 0;
        pokemon->afterimages[i].unk_16A = sp64[i];
        pokemon->afterimages[i].displayObject.lodFraction = 0;
        pokemon->afterimages[i].displayObject.node.flags |= 1;
        BattleAnim_Vec3fCopy(&pokemon->afterimages[i].displayObject.position, &pokemon->displayObject.position);
        BattleAnim_Vec3sCopy(&pokemon->afterimages[i].displayObject.rotation, &pokemon->displayObject.rotation);
        BattleAnim_Vec3fCopy(&pokemon->afterimages[i].displayObject.scale, &pokemon->displayObject.scale);
    }
}

void BattleScene_UpdateSecondaryModelOrbit(ActivePokemon* pokemon) {
    s32 i;
    f32 tmp;
    Vec3f sp5C = { 0.0f, 0.0f, 0.0f };         // D_84384610
    f32 sp4C[] = { 0.3f, -0.4f, -0.2f, 0.4f }; // D_8438461C

    for (i = 0; i < 2; i++) {
        pokemon->afterimages[i].displayObject.lodFraction = Math_StepToS32(pokemon->afterimages[i].displayObject.lodFraction, 0x3C, 5, 5);
        pokemon->afterimages[i].unk_16C = Math_StepToS32(pokemon->afterimages[i].unk_16C, 0x38E, 0x64, 0x64);
        pokemon->afterimages[i].unk_16C = Math_StepToS32(pokemon->afterimages[i].unk_16C, 0x38E, 0x64, 0x64);
        pokemon->afterimages[i].unk_16A += pokemon->afterimages[i].unk_16C;

        tmp = SINS(pokemon->afterimages[i].unk_16A) * (BattleScene_GetParticipantModelRadiusScaled(pokemon) * sp4C[i]);
        BattleAnim_Vec3fSetPolarXZ(&sp5C, tmp, pokemon->displayObject.rotation.y);

        pokemon->afterimages[i].displayObject.position.x = pokemon->displayObject.position.x + sp5C.x;
        pokemon->afterimages[i].displayObject.position.y = pokemon->displayObject.position.y + sp5C.y;
        pokemon->afterimages[i].displayObject.position.z = pokemon->displayObject.position.z + sp5C.z;

        ModelAnim_SetFrame(&pokemon->afterimages[i].displayObject, pokemon->displayObject.transformAnim.frameFixed >> 0x10);
        ModelAnim_SetAnimation(&pokemon->afterimages[i].displayObject, pokemon->displayObject.transformAnim.animationId);
    }
}

void BattleScene_ShowSecondaryModelsAtMainPose(ActivePokemon* active_pokemon) {
    s32 i;

    for (i = 0; i < 2; i++) {
        active_pokemon->afterimages[i].minimized = 0;
        active_pokemon->afterimages[i].displayObject.lodFraction = 0x3C;
        active_pokemon->afterimages[i].displayObject.node.flags |= 1;
        Vec3f_SetComponentsDuplicate(&active_pokemon->afterimages[i].displayObject.scale, 1.0f, 1.0f, 1.0f);
        BattleAnim_Vec3fCopy(&active_pokemon->afterimages[i].displayObject.position, &active_pokemon->displayObject.position);
        BattleAnim_Vec3sCopy(&active_pokemon->afterimages[i].displayObject.rotation, &active_pokemon->displayObject.rotation);
    }
}

s32 BattleScene_ShrinkSecondaryModelsToScale(ActivePokemon* active_pokemon) {
    s32 i;
    f32 minimize_scale;
    f32 deltas[] = { 0.007f, 0.005f, 0.039f }; // D_8438462C

    for (i = 0; i < 2; i++) {
        BattleAnim_Vec3fCopy(&active_pokemon->afterimages[i].displayObject.position, &active_pokemon->displayObject.position);

        minimize_scale = Math_StepToF(active_pokemon->afterimages[i].displayObject.scale.x, 0.8f, deltas[i], deltas[i]);
        active_pokemon->afterimages[i].displayObject.scale.x = minimize_scale;
        active_pokemon->afterimages[i].displayObject.scale.y = minimize_scale;
        active_pokemon->afterimages[i].displayObject.scale.z = minimize_scale;

        ModelAnim_SetFrame(&active_pokemon->afterimages[i].displayObject, active_pokemon->displayObject.transformAnim.frameFixed >> 0x10);
        ModelAnim_SetAnimation(&active_pokemon->afterimages[i].displayObject, active_pokemon->displayObject.transformAnim.animationId);

        if (active_pokemon->afterimages[i].displayObject.scale.y < 0.8) {
            active_pokemon->afterimages[i].minimized = 1;
        }
    }

    return 0;
}

void BattleScene_ShowSecondaryModelsStaggeredFrame(ActivePokemon* active_pokemon) {
    s32 i;

    for (i = 0; i < 2; i++) {
        active_pokemon->afterimages[i].minimized = 0;
        active_pokemon->afterimages[i].displayObject.lodFraction = 0x41 - i * 4;
        active_pokemon->afterimages[i].displayObject.node.flags |= 1;

        ModelAnim_SetFrame(&active_pokemon->afterimages[i].displayObject, active_pokemon->displayObject.transformAnim.frameFixed >> 0x10);
        ModelAnim_SetAnimation(&active_pokemon->afterimages[i].displayObject, active_pokemon->displayObject.transformAnim.animationId);
        Vec3f_SetComponentsDuplicate(&active_pokemon->afterimages[i].displayObject.scale, 1.0f, 1.0f, 1.0f);
        BattleAnim_Vec3fCopy(&active_pokemon->afterimages[i].displayObject.position, &active_pokemon->displayObject.position);
        BattleAnim_Vec3sCopy(&active_pokemon->afterimages[i].displayObject.rotation, &active_pokemon->displayObject.rotation);
        BattleAnim_Vec3fCopy(&active_pokemon->afterimages[i].displayObject.scale, &active_pokemon->displayObject.scale);
    }
}

void BattleScene_UpdateSecondaryModelTrailPosition(ActivePokemon* pokemon, s16 new_position) {
    s32 i;
    f32 distance;
    s16 angle_x;
    s16 angle_y;
    UNUSED s32 pad;
    f32 eye[3] = { 0.2f, 0.25f, 0.3f };   // D_84384638
    f32 rate[3] = { 0.98f, 0.96f, 0.95f }; // D_84384644

    for (i = 0; i < 2; i++) {
        ModelAnim_SetFrame(&pokemon->afterimages[i].displayObject, pokemon->displayObject.transformAnim.frameFixed >> 0x10);
        ModelAnim_SetAnimation(&pokemon->afterimages[i].displayObject, pokemon->displayObject.transformAnim.animationId);
        Vec3f_CalculateDistanceAngles(&pokemon->displayObject.position, &pokemon->afterimages[i].displayObject.position, &distance, &angle_x, &angle_y);
        Camera_ComputeEyeFromAngles(&pokemon->displayObject.position, &pokemon->afterimages[i].displayObject.position, eye[i] * distance, angle_x, angle_y);
        BattleAnim_StepToS16(&pokemon->afterimages[i].displayObject.rotation.y, pokemon->displayObject.rotation.y, rate[i] * new_position);
    }
}

void BattleScene_ResetParticipantModelFlags(ActivePokemon* active_pokemon) {
    BattleScene_SetParticipantModelTransform(active_pokemon, 0, 0);
}

void BattleAnim_PokeBallThrowStart(UNUSED ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    animated_pokemon->pokeBall.node.flags |= 1;
    if (animated_pokemon == sActivePokemon[0]) {
        Vec3f_SetComponentsDuplicate(&animated_pokemon->pokeBall.position, 35.0f, 50.0f, -250.0f);
    } else {
        Vec3f_SetComponentsDuplicate(&animated_pokemon->pokeBall.position, -35.0f, 50.0f, -250.0f);
    }
    BattleAnim_InitSwayOffset(animated_pokemon, 20.0f, 0x1FFE);
    animated_pokemon->modelTransformTimer++;
    animated_pokemon->pokeBall.node.renderFlags &= ~0x20;
    ModelAnim_SetFrame(&animated_pokemon->pokeBall, 0);
}

void BattleAnim_PokeBallThrowContinue(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    if (animated_pokemon == sSide1ActivePokemon) {
        BattleAnim_UpdateSwayPhysicsToTarget(owner, animated_pokemon, 0, 0, 0x1FFE, 0x4440);
    } else {
        BattleAnim_UpdateSwayPhysicsToTarget(owner, animated_pokemon, 0, 0, 0x1FFE, 0x3BB8);
    }

    animated_pokemon->pokeBall.rotation.x += 5461;
    animated_pokemon->pokeBall.rotation.z += 3640;

    if (animated_pokemon->pokeBall.position.y < 0) {
        animated_pokemon->pokeBall.node.flags &= ~1;
        animated_pokemon->pokeBall.position.y = 0.0f;
        animated_pokemon->modelTransform.rotation.y = 0.0f;
        BattleScene_ResetParticipantModelFlags(animated_pokemon);
    }
}

/*
    Plays the animation where poke balls are thrown in the air at the stat of the battle
    Animation doesn't change model depending on what poke ball a pokemon was caught in
*/
void BattleAnim_PokeBallThrow(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    switch (animated_pokemon->modelTransformTimer) {
        case 0:
            BattleAnim_PokeBallThrowStart(owner, animated_pokemon);

        case 1:
            BattleAnim_PokeBallThrowContinue(owner, animated_pokemon);
            break;
    }
}

void BattleAnim_PokeBallLandStart(UNUSED ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    Vec3f_SetComponentsDuplicate(&animated_pokemon->pokeBall.position, animated_pokemon->facingDirection * -150.0f, 100.0f, 0.0f);
    animated_pokemon->modelTransformTimer++;
    BattleAnim_InitSwayOffset(animated_pokemon, 0.0f, 16380);
    animated_pokemon->pokeBall.node.flags |= 1;
    animated_pokemon->pokeBall.node.renderFlags &= ~0x20;
    ModelAnim_SetFrame(&animated_pokemon->pokeBall, 0);
}

void BattleAnim_PokeBallLandContinue(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    animated_pokemon->pokeBall.rotation.x += 5461;
    animated_pokemon->pokeBall.rotation.z += 3640;

    BattleAnim_UpdateSwayPhysicsToTarget(owner, animated_pokemon, 0.0f, 0.0f, NINETY_DEGREES, 0);

    if (animated_pokemon->pokeBall.position.y < 0) {
        animated_pokemon->pokeBall.rotation.x = 0;
        animated_pokemon->pokeBall.rotation.z = 0;
        if (animated_pokemon == sSide1ActivePokemon) {
            animated_pokemon->pokeBall.rotation.y = NINETY_DEGREES;
        } else {
            animated_pokemon->pokeBall.rotation.y = -NINETY_DEGREES;
        }
        animated_pokemon->modelTransform.rotation.y = 0.0f;
        animated_pokemon->pokeBall.position.y = 2.5f;
        BattleScene_ResetParticipantModelFlags(animated_pokemon);
    }
}

/*
    Plays the second part of the intro animation: poke balls landing on the ground after camera cuts 
    from the ceiling to the ground.
    
    When the second side's first pokemon is revealed, this animation does not play again.
    It does play on switch out.
*/
void BattleAnim_PokeBallLand(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    switch (animated_pokemon->modelTransformTimer) {
        case 0:
            BattleAnim_PokeBallLandStart(owner, animated_pokemon);

        case 1:
            BattleAnim_PokeBallLandContinue(owner, animated_pokemon);
            break;
    }
}

// fly start
void BattleAnim_FlyStart(UNUSED ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    Vec3f_SetComponentsDuplicate(&animated_pokemon->modelTransform, 0.0f, 0.0f, 0.0f);
    Vec3f_SetComponentsDuplicate(&animated_pokemon->modelTransform.rotation, 0.0f, 0.0f, 0.0f);
    BattleAnim_InitSwayOffset(animated_pokemon, sModelTransformConstants[0], NINETY_DEGREES);
    BattleScene_ShowSecondaryModelsStaggeredFrame(animated_pokemon);
    animated_pokemon->modelTransformTimer++;
}

// fly continue
void BattleAnim_FlyContinue(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    BattleAnim_UpdateTransform(owner, animated_pokemon, 1.45f, 0.015f, NINETY_DEGREES);
    BattleScene_UpdateSecondaryModelTrailPosition(animated_pokemon, 0);
    if (owner->displayObject.position.y >= 200.0f) {
        animated_pokemon->modelTransform.rotation.y = 0.0f;
        BattleScene_ResetParticipantModelFlags(animated_pokemon);
        BattleScene_HideParticipantSecondaryModels(animated_pokemon);
    }
}

/*
    Fly's animation: fly into the sky
    https://youtu.be/Dn5oyCQ3wUs?t=448
*/
void BattleAnim_Fly(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    switch (animated_pokemon->modelTransformTimer) {
        case 0:
            BattleAnim_FlyStart(owner, animated_pokemon);

        case 1:
            BattleAnim_FlyContinue(owner, animated_pokemon);
            break;
    }
}

// waterfall init
void BattleAnim_WaterfallStart(UNUSED ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    Vec3f_SetComponentsDuplicate(&animated_pokemon->modelTransform, 0.0f, 0.0f, 0.0f);
    Vec3f_SetComponentsDuplicate(&animated_pokemon->modelTransform.rotation, 0.0f, 0.0f, 0.0f);
    BattleAnim_InitSwayOffset(animated_pokemon, sModelTransformConstants[0], NINETY_DEGREES);
    animated_pokemon->modelTransformTimer++;
}

// waterfall continue
void BattleAnim_WaterfallContinue(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    BattleAnim_UpdateTransform(owner, animated_pokemon, 2.0f, 0.002f, NINETY_DEGREES);
    if ((owner->displayObject.position.y - BattleScene_GetParticipantModelRestingHeight(animated_pokemon)) >= 90.0f) {
        animated_pokemon->modelTransformTimer++;
        animated_pokemon->modelTransform.rotation.y = 0.0f;
    }
}

void BattleAnim_WaterfallEnd(UNUSED ActivePokemon* owner, UNUSED ActivePokemon* animated_pokemon) {
}

/*
    Waterfall's animation: climb a waterfall
    https://youtu.be/Dn5oyCQ3wUs?t=1360
*/
void BattleAnim_Waterfall(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    switch (animated_pokemon->modelTransformTimer) {
        case 0:
            BattleAnim_WaterfallStart(owner, animated_pokemon);

        case 1:
            BattleAnim_WaterfallContinue(owner, animated_pokemon);
            break;

        case 2:
            BattleAnim_WaterfallEnd(owner, animated_pokemon);
            break;
    }
}

s32 BattleAnim_ApproachAngleAndCheckThreshold(ActivePokemon* pokemon, s16 phase, s16 camera_offset_scale) {
    pokemon->displayObject.rotation.y -= phase;

    if (phase > 0) {
        if ((camera_offset_scale >= pokemon->displayObject.rotation.y) && ((camera_offset_scale - phase) < pokemon->displayObject.rotation.y)) {
            return 1;
        }
    } else if ((pokemon->displayObject.rotation.y >= camera_offset_scale) && (pokemon->displayObject.rotation.y < (camera_offset_scale - phase))) {
        return 1;
    }

    return 0;
}

void BattleAnim_SubmissionStart(UNUSED ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    BattleScene_ShowSecondaryModelsStaggeredFrame(animated_pokemon);
    animated_pokemon->oscillationPhase = 0;
    animated_pokemon->oscillationVelocityEasePhase = 0;
    animated_pokemon->animationState = 0;
    animated_pokemon->modelTransformTimer++;
}

void BattleAnim_SubmissionContinue(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    animated_pokemon->oscillationPhase = Math_StepToS32(animated_pokemon->oscillationPhase, 13104, 364, 364);
    
    if (BattleAnim_ApproachAngleAndCheckThreshold(owner, animated_pokemon->oscillationPhase, animated_pokemon->facingDirection << 14) != 0) {
        animated_pokemon->animationState += 1;
    }
    
    BattleScene_UpdateSecondaryModelTrailPosition(animated_pokemon, animated_pokemon->oscillationPhase);
    
    if (animated_pokemon->animationState == 3) {
        animated_pokemon->modelTransformTimer += 1;
    }
}

void BattleAnim_SubmissionEnd(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    f32 angle_offset = NINETY_DEGREES - owner->displayObject.rotation.y;
    f32 angle_offset_fractional;
    s16 new_phase;
    s32 done;
    
    angle_offset_fractional = (131072.0 - angle_offset) / 131072.0;
    if (angle_offset_fractional >= 1.0f) {
        angle_offset_fractional -= 1.0f;
    }
    
    new_phase = (animated_pokemon->oscillationPhase * angle_offset_fractional) * 0.8;
    if (new_phase < 3640) {
        new_phase = 3640;
    }
    done = BattleAnim_ApproachAngleAndCheckThreshold(owner, new_phase, animated_pokemon->facingDirection << 14);
    BattleScene_UpdateSecondaryModelTrailPosition(animated_pokemon, new_phase);

    if (done != 0) {
        owner->displayObject.rotation.y = animated_pokemon->facingDirection << 14;
        BattleScene_ResetParticipantModelFlags(animated_pokemon);
        BattleScene_HideParticipantSecondaryModels(animated_pokemon);
    }
}

/*
    Submission's Animation: rotate in place
    https://youtu.be/Dn5oyCQ3wUs?t=1173
*/
void BattleAnim_Submission(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    switch (animated_pokemon->modelTransformTimer) {
        case 0:
        BattleAnim_SubmissionStart(owner, animated_pokemon);
        
        case 1:
        BattleAnim_SubmissionContinue(owner, animated_pokemon);
            break;

        case 2:
            BattleAnim_SubmissionEnd(owner, animated_pokemon);
            break;
    }
}

void BattleAnim_DigStart(UNUSED ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    animated_pokemon->oscillationPhase = 0;
    animated_pokemon->oscillationVelocityEasePhase = 0;
    animated_pokemon->animationState = 0;
    animated_pokemon->modelTransformTimer++;
}

void BattleAnim_DigContinue(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    animated_pokemon->oscillationPhase = Math_StepToS32(animated_pokemon->oscillationPhase, 16380, 364, 364);

    if (BattleAnim_ApproachAngleAndCheckThreshold(owner, animated_pokemon->oscillationPhase, animated_pokemon->facingDirection << 14) != 0) {
        animated_pokemon->animationState += 1;
    }

    if (animated_pokemon->animationState >= 3) {
        BattleScene_ShowSecondaryModelsStaggeredFrame(animated_pokemon);
        animated_pokemon->modelTransformTimer += 1;
    }
}

void BattleAnim_DigEnd(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    f32 disappear_y;
    f32 hole_y;
    f32 digging_speed;
    UNUSED s16 pad;
    // Note: Moltres cannot naturally learn Dig
    u8 modified_dig_pokemon[] = { ONIX, MOLTRES }; // D_84384650

    BattleAnim_ApproachAngleAndCheckThreshold(owner, animated_pokemon->oscillationPhase, animated_pokemon->facingDirection << 14);

    if (BattleScene_ByteArrayContains(animated_pokemon->displayObject.objectType, modified_dig_pokemon, sizeof(modified_dig_pokemon)) != 0) {
        hole_y = 10.3f;
        disappear_y = 10.0f;
        digging_speed = 0.035f;
    } else {
        hole_y = 3.8f;
        disappear_y = 3.5f;
        digging_speed = 0.045f;
    }

    BattleScene_UpdateSecondaryModelTrailPosition(animated_pokemon, animated_pokemon->oscillationPhase);
    BattleAnim_ApproachF(&owner->displayObject.position.y, -(BattleAnim_GetOwnerModelHeight(animated_pokemon) * hole_y), digging_speed);

    if (owner->displayObject.position.y <= -(BattleAnim_GetOwnerModelHeight(animated_pokemon) * disappear_y)) {
        Vec3s_SetComponents(&owner->displayObject.rotation, 0, animated_pokemon->facingDirection << 14, 0);
        BattleScene_ResetParticipantModelFlags(animated_pokemon);
        BattleScene_HideParticipantSecondaryModels(animated_pokemon);
    }
}

/*
    Dig's animation: rotate in place and go downwards 
    https://www.youtube.com/watch?v=Dn5oyCQ3wUs&t=244s
*/
void BattleAnim_Dig(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    switch (animated_pokemon->modelTransformTimer) {
        case 0:
            BattleAnim_DigStart(owner, animated_pokemon);

        case 1:
            BattleAnim_DigContinue(owner, animated_pokemon);
            break;

        case 2:
            BattleAnim_DigEnd(owner, animated_pokemon);
            break;
    }
}

void BattleAnim_AgilityStart(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    animated_pokemon->oscillationPhase = 0;
    animated_pokemon->oscillationVelocityEasePhase = 0;
    animated_pokemon->animationState = 0;
    animated_pokemon->oscillationAmplitude = 0.0f;
    BattleScene_PositionParticipantModel(animated_pokemon);
    Vec3s_SetComponents(&owner->displayObject.rotation, 0, animated_pokemon->facingDirection << 0xE, 0);
    BattleScene_ShowSecondaryModelsStaggeredFrame(animated_pokemon);
    animated_pokemon->modelTransformTimer++;
}

void BattleAnim_AgilityContinue(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    f32 magnitude;
    Vec3f zero_vector = { 0.0f, 0.0f, 0.0f }; // D_84384654

    BattleAnim_ApproachF(&animated_pokemon->oscillationAmplitude, 50.0f, 0.1f);
    animated_pokemon->oscillationPhase += 0xE38;
    magnitude = SINS(animated_pokemon->oscillationPhase) * animated_pokemon->oscillationAmplitude;
    BattleAnim_Vec3fSetPolarXZ(&zero_vector, magnitude, owner->displayObject.rotation.y);
    BattleScene_PositionParticipantModel(animated_pokemon);
    Vec3f_AddInPlace(&owner->displayObject.position, &zero_vector);
    BattleScene_UpdateSecondaryModelTrailPosition(animated_pokemon, 0);

    if (animated_pokemon->oscillationAmplitude > 49.0f) {
        animated_pokemon->modelTransformTimer++;
    }
}

void BattleAnim_AgilityEnd(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    f32 magnitude;
    Vec3f zero_vector = { 0.0f, 0.0f, 0.0f };

    BattleAnim_ApproachF(&animated_pokemon->oscillationAmplitude, 0.0f, 0.1f);
    animated_pokemon->oscillationPhase += 3640;

    magnitude = SINS(animated_pokemon->oscillationPhase) * animated_pokemon->oscillationAmplitude;
    BattleAnim_Vec3fSetPolarXZ(&zero_vector, magnitude, owner->displayObject.rotation.y);

    BattleScene_PositionParticipantModel(animated_pokemon);
    Vec3f_AddInPlace(&owner->displayObject.position, &zero_vector);
    BattleScene_UpdateSecondaryModelTrailPosition(animated_pokemon, 0);

    if (animated_pokemon->oscillationAmplitude <= 0.5f) {
        BattleScene_PositionParticipantModel(animated_pokemon);
        Vec3s_SetComponents(&owner->displayObject.rotation, 0, animated_pokemon->facingDirection << 0xE, 0);
        BattleScene_ResetParticipantModelFlags(animated_pokemon);
        BattleScene_HideParticipantSecondaryModels(animated_pokemon);
    }
}

/*
    Agility's animation: pase back and forth
    https://www.youtube.com/watch?v=Dn5oyCQ3wUs&t=19s
*/
void BattleAnim_Agility(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    switch (animated_pokemon->modelTransformTimer) {
        case 0:
            BattleAnim_AgilityStart(owner, animated_pokemon);

        case 1:
            BattleAnim_AgilityContinue(owner, animated_pokemon);
            break;

        case 2:
            BattleAnim_AgilityEnd(owner, animated_pokemon);
            break;
    }
}

void BattleAnim_DoubleTeamStart(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    BattleScene_ShowSecondaryModelsAtMainTransform(animated_pokemon);
    animated_pokemon->doubleTeamTimer = 0;
    BattleScene_PositionParticipantModel(animated_pokemon);
    Vec3s_SetComponents(&owner->displayObject.rotation, 0, animated_pokemon->facingDirection << 14, 0);
    animated_pokemon->modelTransformTimer++;
}

void BattleAnim_DoubleTeamContinue(UNUSED ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    BattleScene_UpdateSecondaryModelOrbit(animated_pokemon);
    animated_pokemon->doubleTeamTimer++;
    if (animated_pokemon->doubleTeamTimer == 60) {
        BattleScene_ResetParticipantModelFlags(animated_pokemon);
        BattleScene_HideParticipantSecondaryModels(animated_pokemon);
    }
}

/*
    Double Team's animation: spawn shadow clones that sway side to side
    https://www.youtube.com/watch?v=Dn5oyCQ3wUs&t=285s
*/
void BattleAnim_DoubleTeam(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    switch (animated_pokemon->modelTransformTimer) {
        case 0:
            BattleAnim_DoubleTeamStart(owner, animated_pokemon);

        case 1:
            BattleAnim_DoubleTeamContinue(owner, animated_pokemon);
            break;
    }
}

void BattleAnim_ResetOwnerFacingAndExitStatusIdle(ActivePokemon* arg0) {
    UNUSED s32 pad;
    CameraNode* sp18 = sBattleScene.scene->camera;

    BattleScene_PositionParticipantModel(arg0);
    Vec3s_SetComponents(&arg0->displayObject.rotation, 0, arg0->facingDirection << 0xE, 0);
    Vec3f_SetComponentsDuplicate(&sp18->view.upDirection, 0.0f, 1.0f, 0.0f);
    BattleScene_ResetParticipantModelFlags(arg0);
}

void BattleAnim_SeismicTossStart(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    animated_pokemon->oscillationPhase = 0;
    animated_pokemon->oscillationAngularVelocity = 0;
    animated_pokemon->oscillationVelocityEasePhase = 0;
    animated_pokemon->animationState = 0;
    animated_pokemon->oscillationAmplitude = 0.0f;
    BattleScene_PositionParticipantModel(animated_pokemon);
    Vec3s_SetComponents(&owner->displayObject.rotation, 0, animated_pokemon->facingDirection << 0xE, 0);
    animated_pokemon->modelTransformTimer++;
}

void BattleAnim_SeismicTossContinue(UNUSED ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    CameraNode* camera = sBattleScene.scene->camera;
    f32 normal_x;
    f32 normal_z;
    f32 flip_angle;
    f32 x;
    f32 y;
    f32 z;
    f32 norm;
    f32 normal_y;

    BattleAnim_StepToS16(&animated_pokemon->oscillationVelocityEasePhase, 1456, 60);
    BattleAnim_StepToS16(&animated_pokemon->oscillationAngularVelocity, -32768, animated_pokemon->oscillationVelocityEasePhase);

    x = camera->view.targetPosition.x - camera->view.eyePosition.x;
    y = camera->view.targetPosition.y - camera->view.eyePosition.y;
    z = camera->view.targetPosition.z - camera->view.eyePosition.z;

    norm = sqrtf(SQ(x) + SQ(y) + SQ(z));

    normal_x = x / norm;
    normal_y = y / norm;
    normal_z = z / norm;

    flip_angle = 1.0 - COSS(animated_pokemon->oscillationAngularVelocity);

    camera->view.upDirection.x = (flip_angle * normal_x * normal_y) - (SINS(animated_pokemon->oscillationAngularVelocity) * normal_z);
    camera->view.upDirection.y = COSS(animated_pokemon->oscillationAngularVelocity) + ((flip_angle * normal_y) * normal_y);
    camera->view.upDirection.z = (SINS(animated_pokemon->oscillationAngularVelocity) * normal_x) + ((flip_angle * normal_y) * normal_z);

    if (animated_pokemon->displayObject.position.y < 0.0f) {
        animated_pokemon->displayObject.position.y = 0.0f;
    }
}

/*
    Seismic Toss' Animation: Flip the screen upside down
    https://www.youtube.com/watch?v=Dn5oyCQ3wUs&t=954s
*/
void BattleAnim_SeismicToss(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    switch (animated_pokemon->modelTransformTimer) {
        case 0:
            BattleAnim_SeismicTossStart(owner, animated_pokemon);

        case 1:
            BattleAnim_SeismicTossContinue(owner, animated_pokemon);
            break;
    }
}

void BattleAnim_MinimizeStart(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    if (owner->displayObject.scale.y != 1.0f) {
        BattleScene_ResetParticipantModelFlags(animated_pokemon);
    } else {
        BattleScene_ShowSecondaryModelsAtMainPose(animated_pokemon);
        animated_pokemon->modelTransformTimer++;
    }
}

void BattleAnim_MinimizeContinue(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    f32 final_scale;
    
    final_scale = Math_StepToF(owner->displayObject.scale.x, 0.8f, 0.01f, 0.01f);
    owner->displayObject.scale.x = final_scale;
    owner->displayObject.scale.y = final_scale;
    owner->displayObject.scale.z = final_scale;
    
    BattleScene_ShrinkSecondaryModelsToScale(animated_pokemon);
    
    if (owner->displayObject.scale.y <= 0.8) {
        BattleScene_ResetParticipantModelFlags(animated_pokemon);
        BattleScene_HideParticipantSecondaryModels(animated_pokemon);
    }
}

/*
    Minimize's Animation: make model smaller
    https://youtu.be/Dn5oyCQ3wUs?t=729
*/
void BattleAnim_Minimize(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    switch (animated_pokemon->modelTransformTimer) {
        case 0:
            BattleAnim_MinimizeStart(owner, animated_pokemon);

        case 1:
            BattleAnim_MinimizeContinue(owner, animated_pokemon);
            break;
    }
}

void BattleAnim_WithdrawStart(UNUSED ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    animated_pokemon->modelTransformTimer++;
}

void BattleAnim_WithdrawContinue(ActivePokemon* owner, UNUSED ActivePokemon* animated_pokemon) {
    owner->displayObject.scale.x = Math_StepToF(owner->displayObject.scale.x, 0.0f, 0.05f, 0.05f);
    owner->displayObject.scale.y = Math_StepToF(owner->displayObject.scale.y, 0.0f, 0.05f, 0.05f);
    owner->displayObject.scale.z = Math_StepToF(owner->displayObject.scale.z, 0.0f, 0.05f, 0.05f);

    if (owner->displayObject.scale.y <= 0.1) {
        Vec3f_SetComponentsDuplicate(&owner->displayObject.scale, 0.001f, 0.001f, 0.001f);
    }
}

/*
    Withdraw's animation for Pokemon other than Squirtle, Wartortle, Blastoise and Cloyster
    Make the model very small so that it looks like only a clam model appears
    TODO: Get video of it
*/
void BattleAnim_Withdraw(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    switch (animated_pokemon->modelTransformTimer) {
        case 0:
            BattleAnim_WithdrawStart(owner, animated_pokemon);

        case 1:
            BattleAnim_WithdrawContinue(owner, animated_pokemon);
            break;
    }
}

void BattleAnim_MeditateStart(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    animated_pokemon->oscillationVelocityEasePhase = 364;
    animated_pokemon->oscillationPhase = 0;
    animated_pokemon->oscillationAngularVelocity = 0;
    animated_pokemon->oscillationAmplitude = 0.4f;
    animated_pokemon->modelTransform.rotation.x = owner->displayObject.scale.x;
    animated_pokemon->modelTransform.rotation.y = owner->displayObject.scale.y;
    animated_pokemon->modelTransform.rotation.z = owner->displayObject.scale.z;
    animated_pokemon->modelTransformTimer++;
}

void BattleAnim_MeditateContinue(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    f32 scale_resize;

    // clang-format off
    if ((animated_pokemon->oscillationVelocityEasePhase >= 0) && (animated_pokemon->oscillationVelocityEasePhase <= 0x8000)) { animated_pokemon->oscillationVelocityEasePhase += 45; }
    // clang-format on

    animated_pokemon->oscillationAngularVelocity = SINS(animated_pokemon->oscillationVelocityEasePhase) * 182.0f * 15.0f;

    if ((animated_pokemon->oscillationPhase >= 0) && (animated_pokemon->oscillationPhase <= NINETY_DEGREES)) {
        animated_pokemon->oscillationPhase += animated_pokemon->oscillationAngularVelocity;
    }

    if (animated_pokemon->state.runtime.scaleResize == 0) {
        scale_resize = 1.0f;
    } else {
        scale_resize = 0.8f;
    }

    owner->displayObject.scale.x = scale_resize - (SINS(animated_pokemon->oscillationPhase) * animated_pokemon->oscillationAmplitude);
    owner->displayObject.scale.y = (SINS(animated_pokemon->oscillationPhase) * animated_pokemon->oscillationAmplitude) + scale_resize;
    owner->displayObject.scale.z = scale_resize - (SINS(animated_pokemon->oscillationPhase) * animated_pokemon->oscillationAmplitude);

    if ((animated_pokemon->oscillationPhase >= NINETY_DEGREES) && (animated_pokemon->oscillationPhase >= NINETY_DEGREES)) {
        animated_pokemon->oscillationAngularVelocity = 5460;
        animated_pokemon->modelTransformTimer++;
    }
}

void BattleAnim_MeditateEnd(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    f32 scale_resize;
    f32 sin_value;

    animated_pokemon->oscillationPhase += animated_pokemon->oscillationAngularVelocity;
    sin_value = SINS(animated_pokemon->oscillationPhase) * animated_pokemon->oscillationAmplitude;
    if (sin_value <= 0.01f) {
        animated_pokemon->oscillationAngularVelocity += 1092;
        animated_pokemon->oscillationAmplitude *= 0.8f;
        if (1) {} //karodo
        sin_value = SINS(animated_pokemon->oscillationPhase) * animated_pokemon->oscillationAmplitude;
    }

    if (animated_pokemon->state.runtime.scaleResize == 0) {
        scale_resize = 1.0f;
    } else {
        scale_resize = 0.8f;
    }

    owner->displayObject.scale.x = scale_resize - sin_value;
    owner->displayObject.scale.y = (SINS(animated_pokemon->oscillationPhase) * animated_pokemon->oscillationAmplitude) + scale_resize;
    owner->displayObject.scale.z = scale_resize - (SINS(animated_pokemon->oscillationPhase) * animated_pokemon->oscillationAmplitude);

    if (animated_pokemon->oscillationAmplitude <= 0.01f) {
        owner->displayObject.scale.x = scale_resize;
        owner->displayObject.scale.y = scale_resize;
        owner->displayObject.scale.z = scale_resize;
        BattleScene_ResetParticipantModelFlags(animated_pokemon);
    }
}

/*
    Meditate's animation: Orientalist mystic stretchy thing idk
    https://www.youtube.com/watch?v=Dn5oyCQ3wUs&t=690s
*/
void BattleAnim_Meditate(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    switch (animated_pokemon->modelTransformTimer) {
        case 0:
            BattleAnim_MeditateStart(owner, animated_pokemon);

        case 1:
            BattleAnim_MeditateContinue(owner, animated_pokemon);
            break;

        case 2:
            BattleAnim_MeditateEnd(owner, animated_pokemon);
            break;
    }
}

void BattleAnim_SquishHitReactionStart(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    animated_pokemon->oscillationAngularVelocity = 1820;
    animated_pokemon->oscillationAmplitude = 0.4f;
    animated_pokemon->modelTransform.rotation.x = owner->displayObject.scale.x;
    animated_pokemon->modelTransform.rotation.y = owner->displayObject.scale.y;
    animated_pokemon->modelTransform.rotation.z = owner->displayObject.scale.z;
    animated_pokemon->modelTransformTimer++;
}

void BattleAnim_SquishHitReactionContinue(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    BattleMonRuntime* runtime = &animated_pokemon->state.runtime;
    s16 angular_velocity;

    animated_pokemon->oscillationAngularVelocity += 0xCCC;
    angular_velocity = animated_pokemon->oscillationAngularVelocity;

    animated_pokemon->oscillationAmplitude = Math_StepToF(animated_pokemon->oscillationAmplitude, 0.0f, 0.02f, 0.02f);

    if (angular_velocity > 0) {
        angular_velocity *= -1;
    }

    if (runtime->scaleResize == 0) {
        owner->displayObject.scale.x = 1.0f - (SINS(angular_velocity) * animated_pokemon->oscillationAmplitude);
        owner->displayObject.scale.y = (SINS(angular_velocity) * animated_pokemon->oscillationAmplitude) + 1.0f;
        owner->displayObject.scale.z = 1.0f - (SINS(angular_velocity) * animated_pokemon->oscillationAmplitude);
    } else {
        owner->displayObject.scale.x = 0.8 - (SINS(angular_velocity) * animated_pokemon->oscillationAmplitude);
        owner->displayObject.scale.y = (SINS(angular_velocity) * animated_pokemon->oscillationAmplitude) + 0.8;
        owner->displayObject.scale.z = 0.8 - (SINS(angular_velocity) * animated_pokemon->oscillationAmplitude);
    }

    if (animated_pokemon->oscillationAmplitude <= 0.01f) {
        if (runtime->scaleResize == 0) {
            Vec3f_SetComponentsDuplicate(&owner->displayObject.scale, 1.0f, 1.0f, 1.0f);
        } else {
            Vec3f_SetComponentsDuplicate(&owner->displayObject.scale, 0.8f, 0.8f, 0.8f);
        }
        BattleScene_ResetParticipantModelFlags(animated_pokemon);
    }
}

//squish hit reaction from seismic toss, stomp, or body slam
//see BattleScene_StartHitReactionAnim @battle_engine_move_nation
/*
    Squish effect when pokemon is hit my Seismic Toss, Stomp, or Body Slam
    https://youtu.be/Dn5oyCQ3wUs?t=1133
*/
void BattleAnim_SquishHitReaction(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    switch (animated_pokemon->modelTransformTimer) {
        case 0:
            BattleAnim_SquishHitReactionStart(owner, animated_pokemon);

        case 1:
            BattleAnim_SquishHitReactionContinue(owner, animated_pokemon);
            break;
    }
}

void BattleAnim_LassoMoveHitReactionStart(UNUSED ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    animated_pokemon->doubleTeamTimer = 0;
    animated_pokemon->modelTransformTimer++;
}

void BattleAnim_LassoMoveHitReactionContinue(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    BattleMonRuntime* runtime = &animated_pokemon->state.runtime;
    UNUSED s32 pad;
    f32 scale_delta;

    if (runtime->scaleResize == 0) {
        scale_delta = (1.0f - Strip400_GetTailFadeRatio()) * 0.6f;
        BattleAnim_ApproachF(&owner->displayObject.scale.x, 1.0f - scale_delta, 0.5f);
        BattleAnim_ApproachF(&owner->displayObject.scale.y, 1.0f + scale_delta, 0.5f);
        BattleAnim_ApproachF(&owner->displayObject.scale.z, 1.0f - scale_delta, 0.5f);
    } else {
        scale_delta = (1.0f - Strip400_GetTailFadeRatio()) * 0.3f;
        BattleAnim_ApproachF(&owner->displayObject.scale.x, 0.8 - scale_delta, 0.5f);
        BattleAnim_ApproachF(&owner->displayObject.scale.y, scale_delta + 0.8, 0.5f);
        BattleAnim_ApproachF(&owner->displayObject.scale.z, 0.8 - scale_delta, 0.5f);
    }
}

/*
    Tightening animation that happens when being hit by Lasso moves
    Lasso moves: String Shot, Constrict, Bind, Wrap, and Disable
    https://youtu.be/Dn5oyCQ3wUs?t=268
*/
void BattleAnim_LassoMoveHitReaction(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    switch (animated_pokemon->modelTransformTimer) {
        case 0:
            BattleAnim_LassoMoveHitReactionStart(owner, animated_pokemon);

        case 1:
            BattleAnim_LassoMoveHitReactionContinue(owner, animated_pokemon);
            break;
    }
}

// surf init
void BattleAnim_SurfStart(UNUSED ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    animated_pokemon->animationState = 0;
    animated_pokemon->modelTransformTimer++;
}

// surf continue
void BattleAnim_SurfContinue(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    f32 rotation_x;
    f32 water_height;

    animated_pokemon->animationState++;
    water_height = TerrainGrid_SampleHeightAndSlope(owner->displayObject.position.x, owner->displayObject.position.z, &rotation_x);
    owner->displayObject.position.y = BattleScene_GetParticipantModelRestingHeight(animated_pokemon) + water_height;
    owner->displayObject.rotation.x = (s16)rotation_x * 182;

    if (animated_pokemon->animationState == 300) {
        owner->displayObject.position.y = BattleScene_GetParticipantModelRestingHeight(animated_pokemon);
        owner->displayObject.rotation.x = 0;
        BattleScene_ResetParticipantModelFlags(animated_pokemon);
    }
}

/*
    Surf's animation: have the pokemon ride a tidal wave
    https://youtu.be/Dn5oyCQ3wUs?t=1203
*/
void BattleAnim_Surf(ActivePokemon* owner, ActivePokemon* animated_pokemon) {
    switch (animated_pokemon->modelTransformTimer) {
        case 0:
            BattleAnim_SurfStart(owner, animated_pokemon);

        case 1:
            BattleAnim_SurfContinue(owner, animated_pokemon);
            break;
    }
}

void BattleAnim_TransformNop(UNUSED ActivePokemon* owner, UNUSED ActivePokemon* animated_pokemon) {
}


void BattleAnim_PlayTransformInducingAnimation(s32 side_index) {
    ActivePokemon* owner = sActivePokemon[side_index];
    ActivePokemon* animated_pokemon = sActivePokemon[side_index];

    switch (sActivePokemon[side_index]->modelTransformId) {
        case 0:
            BattleAnim_TransformNop(owner, animated_pokemon);
            break;

        case 1:
            BattleAnim_PokeBallThrow(owner, animated_pokemon);
            break;

        case 2:
            BattleAnim_PokeBallLand(owner, animated_pokemon);
            break;

        case 3:
            BattleAnim_Fly(owner, animated_pokemon);
            break;

        case 4:
            BattleAnim_Submission(owner, animated_pokemon);
            break;

        case 5:
            BattleAnim_Dig(owner, animated_pokemon);
            break;

        case 6:
            BattleAnim_Agility(owner, animated_pokemon);
            break;

        case 7:
            BattleAnim_DoubleTeam(owner, animated_pokemon);
            break;

        case 8:
            BattleAnim_SeismicToss(owner, animated_pokemon);
            break;

        case 9:
            BattleAnim_Minimize(owner, animated_pokemon);
            break;

        case 10:
            BattleAnim_Surf(owner, animated_pokemon);
            break;

        case 12:
            BattleAnim_Withdraw(owner, animated_pokemon);
            break;

        case 13:
            BattleAnim_Meditate(owner, animated_pokemon);
            break;

        case 14:
            BattleAnim_Waterfall(owner, animated_pokemon);
            break;

        case 15:
            BattleAnim_SquishHitReaction(owner, animated_pokemon);
            break;

        case 16:
            BattleAnim_LassoMoveHitReaction(owner, animated_pokemon);
            break;

        default:
            BattleScene_ResetParticipantModelFlags(animated_pokemon);
            break;
    }
}


/*
    This function and its related structs seem to be dead code
*/
void BattleAnim_SetOwnerSecondaryStatusFlags(ActivePokemon* arg0, u8 arg1, u8 arg2) {
    arg0->unk_4E8 = arg1;
    arg0->unk_4E9 = arg2;
}

/*
    This function and its related structs seem to be dead code
*/
void BattleAnim_ClearOwnerSecondaryStatusFlags(ActivePokemon* arg0) {
    arg0->unk_4E8 = 0;
    arg0->unk_4E9 = 0;
}

/*
    This function and its related structs seem to be dead code
*/
void BattleAnim_InitSecondarySwayOffset(ActivePokemon* arg0, f32 arg1, s16 arg2) {
    arg0->unk_4E4 = arg1;
    arg0->unk_4D8.z = COSS(arg2) * arg1;
    arg0->unk_4D8.y = SINS(arg2) * arg1;
}

/*
    This function and its related structs seem to be dead code
*/
void BattleAnim_UpdateSecondarySwayPhysics(ActivePokemon* arg0, ActivePokemon* arg1, UNUSED f32 arg2, UNUSED f32 arg3, UNUSED s32 arg4) {
    f32 tmp1 = 0.01f;
    f32 tmp2 = 18.0f;
    Vec3f sp2C;
    f32 temp_fa1;
    f32 temp_fa2;

    temp_fa1 = -tmp1 * arg1->unk_4D8.z;
    temp_fa2 = -tmp2 - (tmp1 * arg1->unk_4D8.y);

    arg1->unk_4D8.z = arg1->unk_4D8.z + (temp_fa1 * 0.05f);
    arg1->unk_4D8.y = arg1->unk_4D8.y + (temp_fa2 * 0.05f);
    sp2C.y = arg1->unk_4D8.y;
    arg1->unk_4D8.x = 0.0f;

    BattleAnim_Vec3fSetPolarXZ(&sp2C, arg1->unk_4D8.z, arg0->displayObject.rotation.y + 0x4000);
    Vec3f_AddInPlace(&arg0->displayObject.position, &sp2C);
}

/*
    This function and its related structs seem to be dead code
*/
void BattleAnim_SecondaryStatusCaseBSub0(UNUSED ActivePokemon* arg0, ActivePokemon* arg1) {
    Vec3f_SetComponentsDuplicate(&arg1->unk_4CC, 0.0f, 0.0f, 0.0f);
    Vec3f_SetComponentsDuplicate(&arg1->unk_4D8, 0.0f, 0.0f, 0.0f);
    BattleAnim_InitSecondarySwayOffset(arg1, 23.0f, 0x2E36);
    arg1->unk_4E9++;
}

/*
    This function and its related structs seem to be dead code
*/
void BattleAnim_SecondaryStatusCaseBSub1(ActivePokemon* arg0, ActivePokemon* arg1) {
    BattleAnim_UpdateSecondarySwayPhysics(arg0, arg1, 0, 0, 0x2E36);
    if (arg1->displayObject.position.y < BattleScene_GetParticipantModelRestingHeight(arg1)) {
        arg1->unk_4D8.y = 0.0f;
        arg1->displayObject.position.y = BattleScene_GetParticipantModelRestingHeight(arg1);
        BattleAnim_ClearOwnerSecondaryStatusFlags(arg1);
    }
}

/*
    This function and its related structs seem to be dead code
*/
void BattleAnim_SecondaryStatusCaseB(ActivePokemon* arg0, ActivePokemon* arg1) {
    switch (arg1->unk_4E9) {
        case 0:
            BattleAnim_SecondaryStatusCaseBSub0(arg0, arg1);

        case 1:
            BattleAnim_SecondaryStatusCaseBSub1(arg0, arg1);
            break;
    }
}

/*
    This function and its related structs seem to be dead code
*/
void BattleAnim_SecondaryStatusDefault(UNUSED ActivePokemon* arg0, UNUSED ActivePokemon* arg1) {
}

/*
    This function and its related structs seem to be dead code
*/
void BattleAnim_UpdateOwnerSecondaryStatusAnimation(s32 arg0) {
    ActivePokemon* temp_v0 = sActivePokemon[arg0];
    ActivePokemon* temp_v1 = sActivePokemon[arg0];

    if (temp_v1->unk_4E8 == 0xB) {
        BattleAnim_SecondaryStatusCaseB(temp_v0, temp_v1);
    } else {
        BattleAnim_SecondaryStatusDefault(temp_v0, temp_v1);
    }
}

static s32 pad_D_8438466C = 0;

s16 sChargeMoveCues[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    0,    0, 0xA9, 0, 0, 0, 0, 0, 0xAA, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0,    0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    0,    0, 0,    0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0,    0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0xAB, 0,    0, 0,    0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0xAC, 0, 0, 0, 0, 0, 0,    0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    0,    0, 0,    0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0xAD, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    0xAE, 0, 0,    0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0,    0, 0,
};

ThumbnailSettings sSideThumbnailSettings[2] = {
    {
        0xFFB8,
        0x0048,
        0x0020,
        10.0f,
        10.0f,
        1280.0f,
        0x0001,
    },
    {
        0x015F,
        0x0080,
        0x0020,
        10.0f,
        10.0f,
        1280.0f,
        0xFFFF,
    },
};

DioramaAnimData D_843847EC[] = {
    {
        0,
        0,
        0,
        0,
        14563,
        0.0f,
        4.5f,
        0,
        0,
        0,
        9102,
        0,
        0.0f,
        8.5f,
        0.10000000149011612f,
        0.019999999552965164f,
        30.0f,
    },
    {
        0,
        0,
        0,
        0,
        12743,
        0.0f,
        8.0f,
        0,
        0,
        0,
        0,
        0,
        0.0f,
        4.0f,
        0.15000000596046448f,
        0.009999999776482582f,
        30.0f,
    },
    {
        0,
        16384,
        0,
        0,
        7281,
        3.5f,
        3.0f,
        0,
        -16384,
        0,
        0,
        0,
        0.0f,
        3.0f,
        0.15000000596046448f,
        0.019999999552965164f,
        50.0f,
    },
};
DioramaAnimData D_84384888[] = {
    {
        0,
        0,
        0,
        364,
        7281,
        0.0f,
        8.0f,
        0,
        0,
        0,
        364,
        0,
        1.0f,
        2.0f,
        0.30000001192092896f,
        0.009999999776482582f,
        70.0f,
    },
    {
        0,
        0,
        0,
        0,
        30030,
        0.0f,
        3.5f,
        0,
        0,
        0,
        1820,
        3640,
        0.0f,
        20.0f,
        0.15000000596046448f,
        0.009999999776482582f,
        50.0f,
    },
};

DioramaAnimData D_843848F0 = {
    0, 0, 0, 0, 0, 0.0f, 0.0f, 0, 0, 0, 0, 5461, 0.0f, 3.5f, 0.20000000298023224f, 0.05999999865889549f, 200.0f,
};

DioramaAnimData D_84384924 = {
    0, 0, 0, 0, 0, 0.0f, 0.0f, 0, 0, 0, 4550, 31850, 0.0f, 6.0f, 0.20000000298023224f, 0.03999999910593033f, 200.0f,
};

DioramaAnimData D_84384958 = {
    0, 0, 0, 0, 0, 0.0f, 0.0f, 0, 0, 0, 3640, -32768, 0.0f, 6.0f, 0.20000000298023224f, 0.07999999821186066f, 200.0f,
};

DioramaAnimData D_8438498C = {
    0, 0, 0, 0, 0, 0.0f, 0.0f, 0, 0, 0, 0, 0, 0.0f, 3.5f, 0.20000000298023224f, 0.05999999865889549f, 200.0f,
};

DioramaAnimData D_843849C0[] = {
    {
        0,
        0,
        0,
        0,
        14563,
        1.0f,
        4.5f,
        0,
        0,
        0,
        9102,
        0,
        0.0f,
        8.5f,
        0.10000000149011612f,
        0.019999999552965164f,
        200.0f,
    },
    {
        0,
        0,
        0,
        10922,
        0,
        0.0f,
        8.5f,
        0,
        0,
        0,
        0,
        0,
        0.0f,
        4.0f,
        0.25f,
        0.019999999552965164f,
        200.0f,
    },
    {
        0,
        0,
        0,
        0,
        14563,
        0.0f,
        3.5f,
        0,
        0,
        0,
        9102,
        7281,
        0.0f,
        8.5f,
        0.15000000596046448f,
        0.019999999552965164f,
        200.0f,
    },
    {
        0,
        0,
        0,
        0,
        14563,
        0.0f,
        6.5f,
        0,
        0,
        0,
        0,
        0,
        0.0f,
        4.0f,
        0.20000000298023224f,
        0.019999999552965164f,
        200.0f,
    },
    {
        0,
        0,
        0,
        7281,
        16384,
        0.0f,
        7.5f,
        0,
        0,
        0,
        0,
        0,
        0.0f,
        3.5f,
        0.20000000298023224f,
        0.019999999552965164f,
        200.0f,
    },
    {
        0,
        0,
        0,
        -1820,
        16384,
        0.0f,
        4.5f,
        0,
        0,
        0,
        1820,
        0,
        0.0f,
        3.5f,
        0.20000000298023224f,
        0.019999999552965164f,
        200.0f,
    },
};

DioramaAnimData D_84384AF8 = {
    0, 0, 0, 2184, 0, 0.0f, 10.5f, 0, 0, 0, 182, 0, 0.0f, 2.0f, 0.20000000298023224f, 0.014999999664723873f, 50.0f,
};

DioramaAnimData D_84384B2C[] = {
    {
        0,
        0,
        0,
        10922,
        0,
        0.0f,
        600.0f,
        0,
        0,
        0,
        0,
        0,
        0.0f,
        550.0f,
        0.20000000298023224f,
        0.03999999910593033f,
        40.0f,
    },
    {
        0,
        0,
        0,
        10922,
        12743,
        0.0f,
        625.0f,
        0,
        0,
        0,
        0,
        0,
        0.0f,
        450.0f,
        0.20000000298023224f,
        0.029999999329447746f,
        40.0f,
    },
    {
        0,
        0,
        0,
        -546,
        0,
        0.0f,
        450.0f,
        0,
        0,
        0,
        10922,
        0,
        0.0f,
        500.0f,
        0.20000000298023224f,
        0.02500000037252903f,
        40.0f,
    },
    {
        0,
        0,
        0,
        0,
        -12743,
        0.0f,
        750.0f,
        0,
        0,
        0,
        0,
        0,
        0.0f,
        450.0f,
        0.20000000298023224f,
        0.03999999910593033f,
        40.0f,
    },
    {
        0,
        0,
        0,
        -546,
        0,
        0.0f,
        50.0f,
        0,
        0,
        0,
        -546,
        0,
        0.0f,
        400.0f,
        0.20000000298023224f,
        0.029999999329447746f,
        80.0f,
    },
};

Vec3f D_84384C30[] = {
    { 0.17000000178813934f, 11.5f, -1.5f },
    { 0.10999999940395355f, 27.0f, -2.5f },
    { 0.12999999523162842f, 55.0f, 15.0f },
    { 0.2199999988079071f, 0.0f, -0.5f },
    { 0.1599999964237213f, 14.5f, 2.5f },
    { 0.3199999928474426f, 0.0f, 0.0f },
    { 0.20000000298023224f, 7.0f, 0.5f },
    { 0.20999999344348907f, 12.0f, -3.5f },
    { 0.23000000417232513f, -18.5f, 9.5f },
    { 0.4099999964237213f, -2.5f, -1.5f },
    { 0.1899999976158142f, 7.0f, 1.0f },
    { 0.2800000011920929f, 0.0f, 4.0f },
    { 0.2800000011920929f, 2.5f, 0.5f },
    { 0.23999999463558197f, -1.5f, -3.0f },
    { 0.2199999988079071f, 6.0f, 6.5f },
    { 0.6800000071525574f, -1.5f, -1.5f },
    { 0.800000011920929f, 0.0f, -3.5f },
    { 0.49000000953674316f, -0.5f, -55.0f },
    { 0.27000001072883606f, 2.5f, 2.5f },
    { 0.2800000011920929f, 0.0f, 0.0f },
    { 0.28999999165534973f, 4.0f, -2.5f },
    { 0.23999999463558197f, 0.0f, 2.5f },
    { 0.27000001072883606f, 0.0f, -6.0f },
    { 0.2800000011920929f, 7.5f, 17.5f },
    { 0.33000001311302185f, 0.0f, -3.0f },
    { 0.4000000059604645f, 0.0f, -6.0f },
    { 0.4000000059604645f, 0.0f, -7.0f },
    { 0.28999999165534973f, 0.0f, -5.5f },
    { 0.20999999344348907f, 5.5f, 5.0f },
    { 0.20999999344348907f, 6.5f, 0.0f },
    { 0.2199999988079071f, 11.0f, -7.5f },
    { 0.23999999463558197f, 8.5f, 3.0f },
    { 0.1599999964237213f, 17.5f, 0.0f },
    { 0.20999999344348907f, 14.0f, -6.5f },
    { 0.18000000715255737f, 0.0f, 3.0f },
    { 0.15000000596046448f, 9.0f, 9.0f },
    { 0.3100000023841858f, 0.0f, -2.5f },
    { 0.18000000715255737f, 0.0f, 4.5f },
    { 0.23000000417232513f, 0.0f, 0.0f },
    { 0.1899999976158142f, 0.0f, 15.0f },
    { 0.4099999964237213f, -2.0f, 0.0f },
    { 0.5400000214576721f, -34.5f, -43.5f },
    { 0.20999999344348907f, 0.0f, -2.0f },
    { 0.20999999344348907f, 0.0f, 0.0f },
    { 0.23999999463558197f, 0.0f, 14.0f },
    { 0.27000001072883606f, 0.0f, -2.5f },
    { 0.12999999523162842f, 8.5f, 2.5f },
    { 0.12999999523162842f, 14.0f, 11.5f },
    { 0.4000000059604645f, 0.0f, 0.0f },
    { 0.2199999988079071f, 0.0f, 0.0f },
    { 0.20000000298023224f, 0.0f, 0.0f },
    { 0.4000000059604645f, 0.0f, 0.0f },
    { 0.15000000596046448f, 19.5f, -2.5f },
    { 0.1599999964237213f, 3.5f, 3.5f },
    { 0.1899999976158142f, 10.5f, -8.0f },
    { 0.4000000059604645f, 0.0f, 0.0f },
    { 0.20999999344348907f, 0.0f, -5.5f },
    { 0.23999999463558197f, 3.5f, 1.5f },
    { 0.1899999976158142f, 21.5f, 6.0f },
    { 0.28999999165534973f, 0.0f, 0.0f },
    { 0.23000000417232513f, 0.0f, 0.0f },
    { 0.2199999988079071f, -4.5f, -0.5f },
    { 0.23000000417232513f, 4.0f, 0.0f },
    { 0.1899999976158142f, 16.0f, -2.5f },
    { 0.18000000715255737f, 14.0f, -4.0f },
    { 0.15000000596046448f, 11.0f, 0.0f },
    { 0.25f, 8.5f, -6.0f },
    { 0.25f, 10.0f, -4.0f },
    { 0.3700000047683716f, 0.0f, -5.0f },
    { 0.20000000298023224f, 0.0f, 0.0f },
    { 0.14000000059604645f, 17.5f, 16.5f },
    { 0.23999999463558197f, 0.0f, 0.0f },
    { 0.23999999463558197f, 0.0f, 0.0f },
    { 0.28999999165534973f, 0.0f, 0.0f },
    { 0.1899999976158142f, 0.0f, 0.0f },
    { 0.1899999976158142f, 11.0f, 0.0f },
    { 0.1599999964237213f, 8.0f, 0.0f },
    { 0.17000000178813934f, -7.5f, -7.0f },
    { 0.25999999046325684f, 12.5f, 5.0f },
    { 0.1899999976158142f, 0.0f, 2.5f },
    { 0.2800000011920929f, 0.0f, 0.0f },
    { 0.23000000417232513f, 0.0f, 0.0f },
    { 0.28999999165534973f, 0.0f, -3.0f },
    { 0.25f, 0.0f, 0.0f },
    { 0.23000000417232513f, 0.0f, 0.0f },
    { 0.25999999046325684f, 8.5f, 2.5f },
    { 0.23000000417232513f, 22.5f, 7.5f },
    { 0.25f, 0.0f, -2.5f },
    { 0.18000000715255737f, 0.0f, 5.0f },
    { 0.20000000298023224f, 0.0f, 2.0f },
    { 0.28999999165534973f, 0.0f, 0.0f },
    { 0.1599999964237213f, 0.0f, 0.0f },
    { 0.17000000178813934f, 5.0f, 0.0f },
    { 0.25999999046325684f, 0.0f, -10.5f },
    { 0.03999999910593033f, 74.0f, -7.5f },
    { 0.20999999344348907f, 7.5f, -1.0f },
    { 0.20000000298023224f, 0.0f, -0.5f },
    { 0.2199999988079071f, 0.0f, 0.0f },
    { 0.20000000298023224f, 0.0f, 9.5f },
    { 0.17000000178813934f, 0.0f, 0.0f },
    { 0.1899999976158142f, 0.0f, 0.0f },
    { 0.1899999976158142f, 0.0f, -3.5f },
    { 0.25f, 0.0f, 9.0f },
    { 0.2199999988079071f, 1.5f, -1.0f },
    { 0.23999999463558197f, -2.5f, -4.5f },
    { 0.23000000417232513f, -4.0f, 0.0f },
    { 0.23000000417232513f, 0.0f, 0.0f },
    { 0.27000001072883606f, 0.0f, 0.0f },
    { 0.17000000178813934f, 0.0f, 4.0f },
    { 0.20999999344348907f, -11.0f, 5.5f },
    { 0.17000000178813934f, 8.0f, 0.0f },
    { 0.2199999988079071f, 0.0f, 4.5f },
    { 0.1599999964237213f, 0.0f, 12.5f },
    { 0.15000000596046448f, 4.5f, 8.0f },
    { 0.28999999165534973f, 0.0f, 0.0f },
    { 0.25f, 0.0f, 5.0f },
    { 0.25f, 6.5f, 3.0f },
    { 0.3100000023841858f, 0.0f, 0.5f },
    { 0.23999999463558197f, 18.0f, 0.0f },
    { 0.1899999976158142f, 0.0f, 0.0f },
    { 0.14000000059604645f, 0.0f, 0.0f },
    { 0.23999999463558197f, 0.0f, 0.0f },
    { 0.23999999463558197f, 12.5f, 0.0f },
    { 0.18000000715255737f, 0.0f, 0.0f },
    { 0.18000000715255737f, 9.0f, 0.0f },
    { 0.20999999344348907f, 10.0f, -2.5f },
    { 0.20999999344348907f, 0.0f, 0.0f },
    { 0.1599999964237213f, 12.0f, 5.0f },
    { 0.4000000059604645f, -1.5f, 23.0f },
    { 0.14000000059604645f, 49.0f, 28.0f },
    { 0.20000000298023224f, 22.5f, 8.5f },
    { 0.25999999046325684f, 0.0f, 0.0f },
    { 0.2800000011920929f, 0.0f, 0.0f },
    { 0.25999999046325684f, 0.0f, 4.0f },
    { 0.25f, 11.0f, 0.0f },
    { 0.23000000417232513f, 8.0f, 0.0f },
    { 0.18000000715255737f, 5.5f, 4.5f },
    { 0.2199999988079071f, -0.5f, 2.5f },
    { 0.25f, 0.0f, 0.0f },
    { 0.25999999046325684f, 0.0f, -1.5f },
    { 0.20999999344348907f, 0.0f, 3.5f },
    { 0.1599999964237213f, 32.5f, 26.5f },
    { 0.05000000074505806f, -15.5f, -133.5f },
    { 0.27000001072883606f, 5.5f, -10.0f },
    { 0.4000000059604645f, 0.0f, 0.0f },
    { 0.2199999988079071f, 7.5f, -2.5f },
    { 0.3499999940395355f, 0.0f, 0.0f },
    { 0.23999999463558197f, -9.5f, 0.0f },
    { 0.1899999976158142f, 0.0f, 2.5f },
    { 0.25999999046325684f, 0.0f, -6.5f },
    { 0.30000001192092896f, 0.0f, 3.0f },
    { 0.3499999940395355f, -45.0f, -5.0f },
    { 0.4000000059604645f, 0.0f, 0.0f },
    { 0.4000000059604645f, 0.0f, 0.0f },
    { 0.4000000059604645f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
};
BattleSceneStateCallback sBattleSceneStateTickSentinel = BattleScene_StateCallbackNop;
BattleSceneStateRow sBattleSceneStateRowTable[] = {
    {
        0,
        {
            BattleScene_LoadRowResourceList,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_TickStartRowAnimRecord,
            BattleScene_TickQueueRowEffectList3AtAnimEnd,
            BattleScene_TickPlaySpeciesSequence33,
            BattleScene_TickQueueList3Selector2OrC,
        },
        167,
        5,
        255,
    },
    {
        1,
        {
            BattleScene_EnterRowAnim166WithCry,
            BattleScene_SetOpponentEventTrackA6,
            BattleScene_ResetStateFrameCounter,
            BattleScene_TickRow1PlayOwnerCry,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
        },
        166,
        255,
        255,
    },
    {
        2,
        {
            BattleScene_StateCallbackNop,
            BattleScene_LoadRowResourceList,
            BattleScene_StateCallbackNop,
            BattleScene_TickAssetLoadQueueRowEffectList3,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
        },
        255,
        2,
        255,
    },
    {
        3,
        {
            BattleScene_EnterRow3StartMoveAnim,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_TickRow3PlayMoveEffectScript,
            BattleScene_TickRow3PostEffectCamera,
            Battle_HandleStatBoostingMoveAnimation,
            BattleScene_StateCallbackNop,
        },
        255,
        255,
        255,
    },
    {
        4,
        {
            BattleScene_StartHitReactionAnim,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_HandleLassoMoves,
            BattleScene_TickRow4PostEndCamera,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
        },
        255,
        255,
        255,
    },
    {
        5,
        {
            BattleScene_StateRow56EnterNop,
            BattleScene_LoadMoveResourceList,
            BattleScene_StateCallbackNop,
            BattleScene_StateRow5Slot3Nop,
            BattleScene_StateRow5Slot4Nop,
            BattleScene_StateRow5Slot5Nop,
            BattleScene_StateRow5Slot6Nop,
        },
        255,
        255,
        255,
    },
    {
        6,
        {
            BattleScene_StateRow56EnterNop,
            BattleScene_LoadMoveResourceList,
            BattleScene_StateCallbackNop,
            BattleScene_StateRow6Slot3Nop,
            BattleScene_StateRow6Slot4Nop,
            BattleScene_StateRow6Slot5Nop,
            BattleScene_StateRow6Slot6Nop,
        },
        255,
        255,
        255,
    },
    {
        7,
        {
            BattleScene_LoadRowResourceList,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_TickStartRowAnimRecord,
            BattleScene_StateRow7Slot4Nop,
            BattleScene_StateRow7Slot5Nop,
            BattleScene_TickQueueList3Selector2OrC,
        },
        167,
        5,
        255,
    },
    {
        8,
        {
            BattleScene_EnterRow8StartMappedMoveAnim,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_TickRow8PlayMoveEffectScriptB,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
        },
        255,
        255,
        255,
    },
    {
        9,
        {
            BattleScene_EnterRow9SetEventTrack,
            BattleScene_EnterRow9QueueEffectList6,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
        },
        255,
        255,
        255,
    },
    {
        10,
        {
            BattleScene_StateCallbackNop,
            BattleScene_LoadRowResourceList,
            BattleScene_StateCallbackNop,
            BattleScene_TickAssetLoadQueueRowEffectList3,
            BattleScene_TickRow10CleanupAndEnterState18,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
        },
        255,
        3,
        255,
    },
    {
        11,
        {
            BattleScene_EnterRow11StartHitReactionAnim,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_HandleLassoMoves,
            BattleScene_TickRow11PostEndCamera,
            BattleScene_TickRow11QueueEffectList6,
            BattleScene_StateCallbackNop,
        },
        255,
        255,
        255,
    },
    {
        12,
        {
            BattleScene_ResetStateFrameCounter,
            BattleScene_Row12Slot1CameraCleanup,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
        },
        255,
        255,
        255,
    },
    {
        13,
        {
            BattleScene_Row13PlaySequences14And21,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
        },
        255,
        255,
        255,
    },
    {
        14,
        {
            BattleScene_EnterRow11StartHitReactionAnim,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_TickRow14PlayMoveEffectEnd,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
        },
        255,
        255,
        255,
    },
    {
        15,
        {
            BattleScene_EnterRow15StartAnimA80Seq30,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
        },
        255,
        255,
        255,
    },
    {
        16,
        {
            BattleScene_EnterRowAnim166WithCry,
            BattleScene_SetOpponentEventTrackA6,
            BattleScene_ResetStateFrameCounter,
            BattleScene_TickRow16PlayCryAndSpeciesSound,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
        },
        166,
        255,
        255,
    },
    {
        17,
        {
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_TickStartRowAnimRecord,
            BattleScene_TickQueueList3Selector2OrCFromB10,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
        },
        177,
        255,
        255,
    },
    {
        18,
        {
            BattleScene_EnterRowAnim166WithCry,
            BattleScene_SetOpponentEventTrackA6,
            BattleScene_ResetStateFrameCounter,
            BattleScene_TickRow18PlayCryAndSpeciesSound,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
        },
        166,
        255,
        255,
    },
    {
        19,
        {
            BattleScene_EnterRow19StartMoveAnimMidFrame,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_TickRow19PlayMoveEffectScript,
            BattleScene_TickRow19PostEffectCamera,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
        },
        255,
        255,
        255,
    },
    {
        20,
        {
            BattleScene_StateCallbackNop,
            BattleScene_LoadRowResourceList,
            BattleScene_StateCallbackNop,
            BattleScene_TickAssetLoadQueueRowEffectList3,
            BattleScene_TickRow20CleanupAndEnterState18,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
        },
        255,
        1,
        255,
    },
    {
        21,
        {
            BattleScene_EnterRow21StartAnimRecord0,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_TickRow21PlaySpeciesSound,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
            BattleScene_StateCallbackNop,
        },
        255,
        255,
        255,
    },
};
