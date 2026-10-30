#include "battle_engine.h"
#include "src/geo_node.h"
#include "src/model_animation.h"
#include "src/text_system.h"
#include "src/audio_commands.h"
#include "src/matrix.h"
#include "src/memory.h"
#include "src/util.h"

#if defined(VERSION_JP)
// Points to the EUC-JP format "ゆけっ！ %s" ("Go! %s"); one of the adjacent format pointers from
// D_84386D80_jp in battle_engine_309ED0 data. The send-out messages pass the name for %s.
extern char* D_84386D84_jp;
#endif

void BattleScene_IntroSetupCamera(CameraNode* camera) {
    BattleScene_ResetCameraFrameCounter();
    BattleScene_SetCameraClipPlanes(camera, 20.0f, 12800.0f);
    BattleAnim_LoadModeResourceList(sActivePokemon[0]->sessionTeams, 3);
    camera->perspective.verticalFieldOfView = 40.0f;
    sActivePokemon[0]->displayObject.node.flags &= ~1;
    sActivePokemon[1]->displayObject.node.flags &= ~1;
    Vec3f_SetComponentsDuplicate(&camera->view.targetPosition, 0.0f, 75.0f, 0.0f);
    Vec3f_SetComponentsDuplicate(&camera->view.eyePosition, -1.79f, 0.075f, 155.2f);
    sBattleScene.scene->unk_54 = 1;
    BattleScene_StartBattleMusic();
    sBattleScene.scene->phaseStep += 1;
}

void BattleScene_IntroSendOutFirstMon(UNUSED CameraNode* camera) {
    if ((BattleScene_AdvanceCameraFrameCounterUntil(0xA) != 0) && (BattleAnim_TryRegisterAssetTables(sActivePokemon[0]) != 0)) {
        sActivePokemon[0]->displayObject.node.flags &= ~1;
        sActivePokemon[1]->displayObject.node.flags &= ~1;
        BattleScene_SetParticipantModelTransform(sActivePokemon[0], 1, 0);
        Audio_PlayCommand(1, 0xFE, 0);
        BattleAnim_QueueEffectList3(8, sActivePokemon[0], sActivePokemon[0], 0xFF, 0xFF);
        sBattleScene.scene->phaseStep += 1;
    }
}

void BattleScene_IntroSendOutSecondMon(UNUSED CameraNode* camera) {
    sActivePokemon[0]->displayObject.node.flags &= ~1;
    sActivePokemon[1]->displayObject.node.flags &= ~1;
    if (sActivePokemon[0]->pokeBall.position.y == 0.0f) {
        BattleScene_SelectBattleMusic();
        Audio_PlayCommand(1, 1, 0);
        BattleScene_SetParticipantModelTransform(sActivePokemon[1], 1, 0);
        BattleAnim_QueueEffectList3(0xA, sActivePokemon[1], sActivePokemon[1], 0xFF, 0xFF);
        sBattleScene.scene->phaseStep += 1;
    }
}

void BattleScene_IntroNarrowFov(CameraNode* camera) {
    Vec3f starting_position;

    sActivePokemon[0]->displayObject.node.flags &= ~1;
    sActivePokemon[1]->displayObject.node.flags &= ~1;

    if (sActivePokemon[1]->pokeBall.position.y == 0.0f) {
        camera->perspective.verticalFieldOfView = 30.0f;
        BattleScene_SetParticipantModelTransform(sActivePokemon[0], 2, 0);
        BattleAnim_QueueEffectList3(0xB, sActivePokemon[0], sActivePokemon[0], 0xFF, 0xFF);
        Vec3f_SetComponentsDuplicate(&starting_position, -150.0f, 5.0f, 0.0f);
        BattleScene_SetCameraAtFromAnchor(sActivePokemon[0], camera, starting_position, 0, 0, 0.0f, 0.0f);
        BattleScene_SetCameraEyeFromPosition(camera, starting_position, sBattleScene.scene->rotationX = 0x71C, 0x4000, sBattleScene.scene->unk_84 = 300.0f, 0.0f);
        sBattleScene.scene->phaseStep += 1;
    }
}

void BattleScene_IntroPanCamera(CameraNode* camera) {
    Vec3f sp2C;

    Vec3f_SetComponentsDuplicate(&sp2C, -150.0f, 5.0f, 0.0f);
    sActivePokemon[0]->displayObject.node.flags &= ~1;
    sActivePokemon[1]->displayObject.node.flags &= ~1;
    BattleAnim_ApproachF(&sBattleScene.scene->unk_84, 40.0f, 0.1f);
    BattleAnim_StepToS16(&sBattleScene.scene->rotationX, 0xB6, 0x1E);
    BattleScene_SetCameraEyeFromPosition(camera, sp2C, sBattleScene.scene->rotationX, 0x4000, sBattleScene.scene->unk_84, 0.0f);

    if ((sBattleScene.scene->rotationX >= 0x2E) && (sActivePokemon[0]->pokeBall.position.y == 2.5f)) {
        Audio_PlayCommand(2, 0, 0);
        sActivePokemon[0]->pokeBall.node.renderFlags |= 0x20;
        sBattleScene.scene->phaseStep += 1;
    }
}

void BattleScene_IntroWaitFirstMonAnim(UNUSED CameraNode* camera) {
    sActivePokemon[0]->displayObject.node.flags &= ~1;
    sActivePokemon[1]->displayObject.node.flags &= ~1;
    if (ModelAnim_IsAnimationDone(&sActivePokemon[0]->pokeBall) == 1) {
        sBattleScene.scene->phaseStep += 1;
    }
}

void BattleScene_IntroRevealFirstMon(CameraNode* camera) {
    ActivePokemonState* sp2C = &sActivePokemon[0]->state;
    TeamRoster* roster = sActivePokemon[0]->sessionTeams->teamMember[sp2C->ownerTeamMemberIndex];

    sActivePokemon[0]->thumbnail.camera->node.flags |= 1;
    sActivePokemon[0]->thumbnail.displayObject.node.flags |= 1;
    BattleThumbnail_TriggerOwnerModelAnimation(sActivePokemon[0]);
    sActivePokemon[0]->displayObject.node.flags &= ~1;
    sActivePokemon[1]->displayObject.node.flags &= ~1;
    BattleAnim_QueueEffectList3(3, sActivePokemon[0], sActivePokemon[0], 0xFF, 0xFF);
    sActivePokemon[0]->displayObject.node.flags |= 1;
    sActivePokemon[0]->pokeBall.node.flags &= ~1;
    BattleScene_SelectCameraFollowPreset(sActivePokemon[0], camera, 3);
    BattleScene_EnterStateRow(sActivePokemon[0], 0x10);
    sBattleScene.scene->actingSide = 0;
#if defined(VERSION_JP)
    Battle_QueueMessage(sBattleMessageQueues->buffer[1], 0x21, D_84386D84_jp,
                        roster->pokemonPicked[sp2C->actionChosen].nickname);
#else
    Text_SetStringToken(0x19, (u32)roster->pokemonPicked[sp2C->actionChosen].nickname);
    Battle_QueueMessage(sBattleMessageQueues->buffer[1], 0x21);
#endif
    BattleBox_Open(0xC);
    Battle_TriggerTrainerPresentationCue(sActivePokemon[0]);
    sBattleScene.scene->substepFrameCounter = 0;
    sBattleScene.scene->phaseStep += 1;
}

void BattleScene_IntroFirstMonSettle(CameraNode* camera) {
    if (sBattleScene.scene->substepFrameCounter++ == 1) {
        BattleScene_SelectCameraFollowPreset(sActivePokemon[0], camera, 3);
    }

    sActivePokemon[0]->thumbnail.camera->node.flags &= ~1;
    sActivePokemon[0]->thumbnail.displayObject.node.flags &= ~1;
    sActivePokemon[1]->displayObject.node.flags &= ~1;
    BattleScene_ReframeAndRecomputeDistanceForSwitch(camera, sActivePokemon[0]);

    if (BattleScene_IsRowWaitCompleteWithEventCount(sActivePokemon[0], 0x3C) != 0) {
        sBattleScene.scene->phaseStep += 1;
    }
}

void BattleScene_IntroRevealSecondMon(CameraNode* camera) {
    ActivePokemonState* second_pokemon_state = &sActivePokemon[1]->state;
    TeamRoster* second_trainer_roster = sActivePokemon[1]->sessionTeams->teamMember[second_pokemon_state->ownerTeamMemberIndex];

    sActivePokemon[1]->thumbnail.camera->node.flags |= 1;
    sActivePokemon[1]->thumbnail.displayObject.node.flags |= 1;
    BattleThumbnail_TriggerOwnerModelAnimation(sActivePokemon[1]);
    Audio_PlayCommand(2, 0, 0);
    BattleAnim_QueueEffectList3(3, sActivePokemon[1], sActivePokemon[1], 0xFF, 0xFF);
    BattleScene_ResetCameraFrameCounter();
    sActivePokemon[1]->displayObject.node.flags |= 1;
    BattleScene_SelectCameraFollowPreset(sActivePokemon[1], camera, 3);
    BattleScene_EnterStateRow(sActivePokemon[1], 0x10);
    sBattleScene.scene->substepFrameCounter = 0;
    sBattleScene.scene->actingSide = 1;
#if defined(VERSION_JP)
    Battle_QueueMessage(sBattleMessageQueues->buffer[1], 0x21, D_84386D84_jp,
                        second_trainer_roster->pokemonPicked[second_pokemon_state->actionChosen].nickname);
#else
    Text_SetStringToken(0x19, (u32)second_trainer_roster->pokemonPicked[second_pokemon_state->actionChosen].nickname);
    Battle_QueueMessage(sBattleMessageQueues->buffer[1], 0x21);
#endif
    if (gSessionContext.mode != 8) {
        BattleBox_Open(12);
    }
    Battle_TriggerTrainerPresentationCue(sActivePokemon[1]);
    sBattleScene.scene->phaseStep += 1;
}

void BattleScene_IntroSecondMonSettle(CameraNode* camera) {
    if (sBattleScene.scene->substepFrameCounter++ == 1) {
        BattleScene_SelectCameraFollowPreset(sActivePokemon[1], camera, 3);
    }
    sActivePokemon[1]->thumbnail.camera->node.flags &= ~1;
    sActivePokemon[1]->thumbnail.displayObject.node.flags &= ~1;
    sActivePokemon[0]->displayObject.node.flags &= ~1;
    BattleScene_ReframeAndRecomputeDistanceForSwitch(camera, sActivePokemon[1]);
    if (BattleScene_IsRowWaitCompleteWithEventCount(sActivePokemon[1], 0x3C) != 0) {
        BattleScene_BeginBattlePhase();
        sBattleScene.scene->phaseStep += 1;
    }
}

void BattleScene_IntroStepNop(UNUSED CameraNode* camera) {
}

void BattleScene_IntroInitNop(void) {
}

void BattleScene_UpdateIntroSequence(CameraNode* camera) {
    switch (sBattleScene.scene->phaseStep) {
        case 0:
            BattleScene_IntroSetupCamera(camera);

        case 1:
            BattleScene_IntroSendOutFirstMon(camera);
            break;

        case 2:
            BattleScene_IntroSendOutSecondMon(camera);
            break;

        case 3:
            BattleScene_IntroNarrowFov(camera);
            break;

        case 4:
            BattleScene_IntroPanCamera(camera);
            break;

        case 5:
            BattleScene_IntroWaitFirstMonAnim(camera);
            break;

        case 6:
            BattleScene_IntroRevealFirstMon(camera);
            break;

        case 7:
            BattleScene_IntroFirstMonSettle(camera);
            break;

        case 8:
            BattleScene_IntroRevealSecondMon(camera);
            break;

        case 9:
            BattleScene_IntroSecondMonSettle(camera);
            break;

        case 10:
            BattleScene_IntroStepNop(camera);
            break;
    }
}
