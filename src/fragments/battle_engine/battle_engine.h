#ifndef _FRAGMENT62_H_
#define _FRAGMENT62_H_

#include "global.h"
#include "src/game_state.h"
#include "src/3D140.h"
#include "src/gfx_rect.h"
#include "src/stage_loader.h"
#include "src/fragments/demo_scenes/demo_scenes.h"
#include "src/fragments/particle_math/particle_math.h"

typedef s32 (*func_D_84390300)(void);
typedef void (*func_D_84390320)(void);

typedef struct unk_D_84390010_654_09C {
    /* 0x00 */ char unk00[0x10];
} unk_D_84390010_654_09C; // size = 0x10

typedef struct unk_D_84390010_654_0AC {
    /* 0x00 */ char unk00[0x10];
} unk_D_84390010_654_0AC; // size = 0x10

typedef struct unk_D_84390010_168 {
    /* 0x000 */ DisplayObject displayObject;
    /* 0x168 */ char unk168[0x1];
    /* 0x169 */ u8 minimized;
    /* 0x16A */ s16 unk_16A;
    /* 0x16C */ s16 unk_16C;
    /* 0x16E */ char unk16E[0x2];
} PokemonAfterimage; // size = 0x170

typedef struct ActivePokemonState {
    /* 0x00 */ char unk00[0x4];
    /* 0x04 */ u32 selectedCommand;
    /* 0x08 */ s32 rosterIndex;
    /* 0x0C */ s32 selectedMoveIndex;
    /* 0x10 */ u32 selectedCommandOption;
    /* 0x14 */ s32 moveIndex;
    /* 0x18 */ u32 actionChosen;
    /* 0x1C */ u8 showingCommandOptions;
    /* 0x1E */ u16 actionDelayTimer;
    /* 0x20 */ char unk20[0x8];
    /* 0x28 */ u16 unk_28;
    /* 0x2A */ u8 moveSelectionLocked;
    /* 0x2B */ u8 teamMemberIndex;
    /* 0x2C */ u8 ownerTeamMemberIndex;
    /* 0x2D */ u8 replacementState;
    /* 0x2E */ u8 alpha;
    /* 0x2F */ u8 forfeitCursorHover;
    /* 0x30 */ u16 flashTimer;
    /* 0x32 */ char unk32[0x2];
    /* 0x34 */ u16 flags;
    /* 0x36 */ u16 unk_36;
    /* 0x38 */ BattleMonRuntime runtime;
    /* 0x9C */ unk_D_84390010_654_09C unk_9C;
    /* 0xAC */ unk_D_84390010_654_0AC unk_AC;
    /* 0xBC */ s16 model;
    /* 0xBE */ s16 transformedIntoSpecies;
    /* 0xC0 */ char unkC0[1];
    /* 0xC1 */ u8 moveSlotIllegality[4];
    /* 0xC8 */ Controller* controller;
} ActivePokemonState; // size = 0xCC

typedef struct ThumbnailState {
    /* 0x000 */ DisplayObject displayObject;
    /* 0x168 */ CameraNode* camera;
    /* 0x16C */ s32 tagTeam;
    /* 0x170 */ s32 showing;
    /* 0x174 */ char unk174[0x4];
    /* 0x178 */ s32 side;
    /* 0x17C */ s32 idleAnimation;
    /* 0x180 */ s32 idleTimer;
    /* 0x184 */ s16 rotationX;
    /* 0x186 */ s16 rotationY;
    /* 0x188 */ f32 x;
    /* 0x18C */ s32 status;
    /* 0x190 */ f32 z;
    /* 0x194 */ f32 y;
    /* 0x198 */ char unk198[0x4];
} ThumbnailState; // size = 0x19C

typedef struct MonCaptionState {
    /* 0x00 */ s32 active;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s8 unk_08[4];
    /* 0x0C */ char unk0C[60];
    /* 0x48 */ s8 unk_48[4];
    /* 0x4C */ char unk4C[60];
} MonCaptionState; // size = 0x88

typedef struct SpeciesLearnsetBuffer {
    /* 0x00 */ u8 unk_00[10];
    /* 0x0A */ u8 unk_0A[4];
    /* 0x0E */ char unk0E[0x12];
} SpeciesLearnsetBuffer; // size = 0x20

typedef struct SwayLayoutConfig {
    /* 0x00 */ Vec3f frontCameraShotOffset;
    /* 0x0C */ Vec3f backCameraShotOffset;
    /* 0x18 */ u8 unk_18;
    /* 0x19 */ u8 unk_19;
    /* 0x1A */ u8 unk_1A;
    /* 0x1B */ u8 unk_1B;
    /* 0x1C */ char unk1C[4];
} SwayLayoutConfig; // size = 0x20

typedef struct SwayState {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ Vec3f rotation;
    /* 0x18 */ Vec3f anchorAdjustment;
    /* 0x24 */ SwayLayoutConfig layoutConfig;
    /* 0x44 */ s16 rotationX;
    /* 0x46 */ s16 rotationY;
    /* 0x48 */ char unk48[0x4];
    /* 0x4C */ f32 distance;
} ModelTransform; // size >= 0x50

typedef struct ActivePokemon {
    /* 0x000 */ DisplayObject displayObject;
    /* 0x168 */ PokemonAfterimage afterimages[2];
    /* 0x448 */ ModelTransform modelTransform;
    /* 0x498 */ f32 y;
    /* 0x49C */ f32 rotationZ;
    /* 0x4A0 */ f32 oscillationAmplitude;
    /* 0x4A4 */ s16 oscillationVelocityEasePhase;
    /* 0x4A6 */ s16 oscillationPhase;
    /* 0x4A8 */ s16 oscillationAngularVelocity;
    /* 0x4AC */ s32 doubleTeamTimer;
    /* 0x4B0 */ s32 facingDirection;
    /* 0x4B4 */ s32 modelTransformId;
    /* 0x4B8 */ s32 modelTransformTimer;
    /* 0x4BC */ char unk4BC[0x4];
    /* 0x4C0 */ s32 animationState;
    /* 0x4C4 */ s32 animationTimer;
    /* 0x4C8 */ s32 unk_4C8;
    /* 0x4CC */ Vec3f unk_4CC;
    /* 0x4D8 */ Vec3f unk_4D8;
    /* 0x4E4 */ f32 unk_4E4;
    /* 0x4E8 */ u8 unk_4E8;
    /* 0x4E9 */ u8 unk_4E9;
    /* 0x4EC */ DisplayObject pokeBall;
    /* 0x654 */ ActivePokemonState state;
    /* 0x720 */ BattleSessionTeams* sessionTeams;
    /* 0x724 */ TeamRoster* teamRoster;
    /* 0x728 */ ThumbnailState thumbnail;
    /* 0x8C4 */ MonCaptionState captionState;
    /* 0x94C */ SpeciesLearnsetBuffer redBlueLearnsets[3];
    /* 0x9AC */ SpeciesLearnsetBuffer yellowLearnsets[3];
} ActivePokemon; // size = 0xA0C

typedef struct MovePresentationCue {
    /* 0x00 */ u8 id;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 defaultAnchor;
    /* 0x03 */ u8 alternateAnchor;
    /* 0x04 */ u8 animationDuration;
    /* 0x05 */ u8 flags1;
    /* 0x06 */ u8 currentFrame;
    /* 0x07 */ u8 endFrame;
    /* 0x08 */ u8 unk_08;
    /* 0x09 */ u8 unk_09;
    /* 0x0A */ u8 frameCounter;
    /* 0x0B */ u8 animationTimer;
    /* 0x0C */ u8 checkpointFrame;
    /* 0x0D */ u8 initialCameraCue;
    /* 0x0E */ u8 followupCameraCue;
    /* 0x0F */ u8 cameraHoldFrames;
} MovePresentationCue; // size = 0x10

typedef struct PokemonBattleCues {
    /* 0x000 */ MovePresentationCue moveCues[165];
    /* 0xA50 */ MovePresentationCue idleCues;
    /* 0xA60 */ char unkA60[0x10];
    /* 0xA70 */ MovePresentationCue faintCue;
    /* 0xA80 */ MovePresentationCue hitCue;
    /* 0xA90 */ char chargeCues[0x60];
    /* 0xAF0 */ MovePresentationCue sleepIdleCue;
    /* 0xB00 */ MovePresentationCue flyingIdleCue;
    /* 0xB10 */ MovePresentationCue unk_B10;
    /* 0xB20 */ MovePresentationCue hitReactionOverride;
    /* 0xB30 */ char unkB30[0x51];
    /* 0xB81 */ u8 wakeUpEventTrack;
    /* 0xB82 */ char unkB82[0x5E];
} PokemonBattleCues; // size = 0xBE0

// moveCues[] has one entry per move, so cue indices past the last move alias the
// named cues that follow it (each MovePresentationCue is 0x10 bytes).
#define BATTLE_CUE_IDLE 165       // idleCues (0xA50)
#define BATTLE_CUE_A60 166        // unkA60 (0xA60)
#define BATTLE_CUE_FAINT 167      // faintCue (0xA70)
#define BATTLE_CUE_HIT 168        // hitCue (0xA80)
#define BATTLE_CUE_SLEEP_IDLE 175 // sleepIdleCue (0xAF0)
#define BATTLE_CUE_B00 176        // unk_B00 (0xB00)

// Gen 1 type-effectiveness multipliers, in tenths; see sTypeEffectiveness.
#define TYPE_EFFECTIVENESS_NONE 0
#define TYPE_EFFECTIVENESS_NOT_VERY 5
#define TYPE_EFFECTIVENESS_NEUTRAL 10
#define TYPE_EFFECTIVENESS_SUPER 20

// Message indices passed to Battle_QueueMessage. Values of 128 and above skip
// sBattleMessagesTableFileNumbers and select string table 30 entry (index - 72);
// strings quoted below are the US text for that entry (the table is loaded by
// Text_GetStringTable(30) in //@src/fragments/battle_engine/battle_engine_move_animation.c).
#define BATTLE_MSG_TRANSFORM_FAILED 208        // "But, it failed!"
#define BATTLE_MSG_TRANSFORM_DITTO_BLOCKED 209 // "It couldn't transform into DITTO!"
#define BATTLE_MSG_LIGHT_SCREEN_SET 210        // "It's protected from SPECIAL attacks!"
#define BATTLE_MSG_LIGHT_SCREEN_FAILED 211     // "But, it failed!"
#define BATTLE_MSG_REFLECT_SET 212             // "It gained armor!"
#define BATTLE_MSG_REFLECT_FAILED 213          // "But, it failed!"
#define BATTLE_MSG_SUBSTITUTE_EXISTS 214       // "It has a SUBSTITUTE!"
#define BATTLE_MSG_SUBSTITUTE_LOW_HP 215       // "Not enough HP!"
#define BATTLE_MSG_SUBSTITUTE_CREATED 216      // "It created a SUBSTITUTE!"

// BattleScene.moveEffectKind selects presentation, not moveCache.unk_01's
// gameplay effect. Producers: BattleEffect_* and Battle_CheckMonCanAct.
#define MOVE_PRESENTATION_NORMAL 0
#define MOVE_PRESENTATION_SELF_TARGET 7
#define MOVE_PRESENTATION_FAILED 8
#define MOVE_PRESENTATION_RECOIL 9
#define MOVE_PRESENTATION_FLY_START 12
#define MOVE_PRESENTATION_DIG_START 13
#define MOVE_PRESENTATION_CHARGE 14
#define MOVE_PRESENTATION_TRANSFORM 15
#define MOVE_PRESENTATION_JUMP_KICK_CRASH 16
#define MOVE_PRESENTATION_UNABLE_TO_ACT 17
#define MOVE_PRESENTATION_CONFUSION_SELF_HIT 20
#define MOVE_PRESENTATION_HEAL 21
#define MOVE_PRESENTATION_TARGET_STATUS 22
#define MOVE_PRESENTATION_MULTI_HIT 24
#define MOVE_PRESENTATION_SUBSTITUTE 25
#define MOVE_PRESENTATION_TARGET_STAT_DROP 26
#define MOVE_PRESENTATION_BIDE 27
#define MOVE_PRESENTATION_NO_ACTION 28
#define MOVE_PRESENTATION_HAZE 29
#define MOVE_PRESENTATION_EXPLOSION 30
#define MOVE_PRESENTATION_FULL_PARALYSIS 31
#define MOVE_PRESENTATION_DISABLE 32
#define MOVE_PRESENTATION_WITHDRAW 38
#define MOVE_PRESENTATION_TELEPORT 39

// BattleScene.phase is a separate namespace: healing and Teleport share the
// self-target phase, while status and stat drops finish in the ordinary action.
// Dispatch and transitions: BattleScene_UpdatePhase and BattleScene_PhaseDispatchMoveEffect.
#define BATTLE_PHASE_INITIALIZE 0
#define BATTLE_PHASE_TURN_INTRO 1
#define BATTLE_PHASE_FIRST_ACTION 2
#define BATTLE_PHASE_SECOND_ACTION 3
#define BATTLE_PHASE_END_TURN 4
#define BATTLE_PHASE_FAINT 5
#define BATTLE_PHASE_RESIDUAL_AFTER_MOVE 6
#define BATTLE_PHASE_SELF_TARGET 7
#define BATTLE_PHASE_MOVE_FAILED 8
#define BATTLE_PHASE_RECOIL 9
#define BATTLE_PHASE_RAGE 10
#define BATTLE_PHASE_VOLUNTARY_SWITCH 11
#define BATTLE_PHASE_FLY_START 12
#define BATTLE_PHASE_DIG_START 13
#define BATTLE_PHASE_CHARGE 14
#define BATTLE_PHASE_TRANSFORM 15
#define BATTLE_PHASE_JUMP_KICK_CRASH 16
#define BATTLE_PHASE_UNABLE_TO_ACT 17
#define BATTLE_PHASE_SEND_OUT_AFTER_FAINT 18
#define BATTLE_PHASE_CONFUSION_SELF_HIT 20
#define BATTLE_PHASE_SUBSTITUTE 25
#define BATTLE_PHASE_BIDE 27
#define BATTLE_PHASE_NO_ACTION 28
#define BATTLE_PHASE_HAZE 29
#define BATTLE_PHASE_EXPLOSION 30
#define BATTLE_PHASE_FULL_PARALYSIS 31
#define BATTLE_PHASE_RESIDUAL_FAINT_CHECK 33
#define BATTLE_PHASE_RESIDUAL_FAINT 34
#define BATTLE_PHASE_RESIDUAL_WITHOUT_MOVE 35
#define BATTLE_PHASE_ENTER_QUEUED 36
#define BATTLE_PHASE_DISPATCH_MOVE_EFFECT 37
#define BATTLE_PHASE_WITHDRAW 38

#define BATTLE_ACTION_FIRST 0
#define BATTLE_ACTION_SECOND 1
#define BATTLE_ACTION_COMPLETE 3

// Producers: Battle_ApplyDamageAnimation, ProcessBattleTurn and secondary effects.
#define BATTLE_HIT_REACTION_NONE 0
#define BATTLE_HIT_REACTION_MULTI_HIT 1
#define BATTLE_HIT_REACTION_SUBSTITUTE 2
#define BATTLE_HIT_REACTION_SECONDARY 3

#define BATTLE_SCENE_MODE_FRAME_DEFAULT_SIDE 0
#define BATTLE_SCENE_MODE_ACTION_SELECTION 1
#define BATTLE_SCENE_MODE_BATTLE 2
#define BATTLE_SCENE_MODE_FRAME_SIDE_ZERO 3
#define BATTLE_SCENE_MODE_INTRO 4
#define ACTION_CAMERA_WIDE 0
#define ACTION_CAMERA_WIDE_STATIC 1
#define ACTION_CAMERA_CLOSE 2
#define ACTION_CAMERA_OFFSET_CLOSE 3
#define ACTION_CAMERA_TABLE_FIRST 4
#define ACTION_CAMERA_TABLE_SECOND 5
#define ACTION_CAMERA_ANGLED_CLOSE 6
#define ACTION_CAMERA_LOW_CLOSE 7
#define ACTION_CAMERA_TABLE_THIRD 8
#define ACTION_CAMERA_TABLE_FOURTH 9
#define ACTION_CAMERA_TABLE_FIFTH 10
#define ACTION_CAMERA_TABLE_SIXTH 11
#define ACTION_CAMERA_RANDOM_VARIED 20
#define ACTION_CAMERA_RANDOM_WIDE_CLOSE 21
#define ACTION_CAMERA_RANDOM_CLOSE 22
#define ACTION_CAMERA_RANDOM_TABLE 23
#define ACTION_CAMERA_RANDOM_LATER_TABLE 24
#define ACTION_CAMERA_KEEP 25
#define BATTLE_MODEL_TRANSFORM_NONE 0
#define BATTLE_MODEL_TRANSFORM_POKEBALL_THROW 1
#define BATTLE_MODEL_TRANSFORM_POKEBALL_LAND 2
#define BATTLE_MODEL_TRANSFORM_FLY 3
#define BATTLE_MODEL_TRANSFORM_SUBMISSION 4
#define BATTLE_MODEL_TRANSFORM_DIG 5
#define BATTLE_MODEL_TRANSFORM_AGILITY 6
#define BATTLE_MODEL_TRANSFORM_DOUBLE_TEAM 7
#define BATTLE_MODEL_TRANSFORM_SEISMIC_TOSS 8
#define BATTLE_MODEL_TRANSFORM_MINIMIZE 9
#define BATTLE_MODEL_TRANSFORM_SURF 10
#define BATTLE_MODEL_TRANSFORM_WITHDRAW 12
#define BATTLE_MODEL_TRANSFORM_MEDITATE 13
#define BATTLE_MODEL_TRANSFORM_WATERFALL 14
#define BATTLE_MODEL_TRANSFORM_SQUISH_HIT 15
#define BATTLE_MODEL_TRANSFORM_LASSO_HIT 16
#define BATTLE_VISIBILITY_DEFAULT 0
#define BATTLE_VISIBILITY_AFTER_ACTION 1
#define BATTLE_VISIBILITY_HIDE_OPPONENT 2
#define BATTLE_VISIBILITY_QUEUED_ACTION 3
#define BATTLE_VISIBILITY_SWITCHING 4
#define BROLL_STEP_SETUP_ORBIT 0
#define BROLL_STEP_UPDATE_ORBIT 1
#define BROLL_STEP_SETUP_ROTATING_OVERVIEW 2
#define BROLL_STEP_UPDATE_ROTATING_OVERVIEW 3
#define BROLL_STEP_SETUP_WIDE_SIDE_ZERO 4
#define BROLL_STEP_UPDATE_WIDE_SIDE_ZERO 5
#define BROLL_STEP_SETUP_WIDE_SIDE_ONE 6
#define BROLL_STEP_UPDATE_WIDE_SIDE_ONE 7
#define BROLL_STEP_SETUP_ZOOM_SIDE_ZERO 8
#define BROLL_STEP_UPDATE_ZOOM_SIDE_ZERO 9
#define BROLL_STEP_SETUP_ZOOM_SIDE_ONE 10
#define BROLL_STEP_UPDATE_ZOOM_SIDE_ONE 11
#define BROLL_STEP_SETUP_SEND_OUT_SIDE_ZERO 12
#define BROLL_STEP_UPDATE_SEND_OUT_SIDE_ZERO 13
#define BROLL_STEP_SETUP_SEND_OUT_SIDE_ONE 14
#define BROLL_STEP_UPDATE_SEND_OUT_SIDE_ONE 15
#define BROLL_STEP_SETUP_FIRST_HEIGHT_SHOT 16
#define BROLL_STEP_UPDATE_FIRST_HEIGHT_SHOT 17
#define BROLL_STEP_SETUP_SECOND_HEIGHT_SHOT 18
#define BROLL_STEP_UPDATE_SECOND_HEIGHT_SHOT 19
#define BROLL_STEP_SETUP_STATIC_OVERVIEW 20
#define BROLL_STEP_UPDATE_STATIC_OVERVIEW 21
#define BROLL_STEP_SETUP_CLOSE_SIDE_ZERO 22
#define BROLL_STEP_UPDATE_CLOSE_SIDE_ZERO 23
#define BROLL_STEP_SETUP_CLOSE_SIDE_ONE 24
#define BROLL_STEP_UPDATE_CLOSE_SIDE_ONE 25
#define BROLL_STEP_SELECT_NEXT 50
#define BROLL_STEP_ORDER_END 255
#define FOLLOW_CAMERA_CLOSE 0
#define FOLLOW_CAMERA_ANGLED_CLOSE 1
#define FOLLOW_CAMERA_LOW_CLOSE 2
#define FOLLOW_CAMERA_WIDE 3
#define FOLLOW_CAMERA_NEAR 4
#define TARGET_CAMERA_STANDARD 0
#define TARGET_CAMERA_HIGH 1
#define TARGET_CAMERA_LOW 2
#define TARGET_CAMERA_CLOSE 3
#define TARGET_CAMERA_ANGLED 4
#define TARGET_CAMERA_HIGH_ALTERNATE 5
#define TARGET_CAMERA_LOW_ALTERNATE 6
#define TARGET_CAMERA_CLOSE_ALTERNATE 7
#define CAMERA_TRANSITION_STEP_IDLE 0
#define CAMERA_TRANSITION_STEP_BEGIN 1
#define CAMERA_TRANSITION_STEP_WAIT_THUMBNAILS 2
#define CAMERA_TRANSITION_STEP_COVER 3
#define CAMERA_TRANSITION_STEP_WAIT_COVER 4
#define CAMERA_TRANSITION_STEP_RESTORE_MODELS 5
#define CAMERA_TRANSITION_STEP_UNCOVER 6
#define STRIP_TRANSITION_STEP_IDLE 0
#define STRIP_TRANSITION_STEP_BEGIN 1
#define STRIP_TRANSITION_STEP_WAIT_THUMBNAILS 2
#define STRIP_TRANSITION_STEP_COVER 3
#define STRIP_TRANSITION_STEP_WAIT_COVER 4
#define STRIP_TRANSITION_STEP_HOLD_FIRST 5
#define STRIP_TRANSITION_STEP_HOLD_SECOND 6
#define STRIP_TRANSITION_STEP_HOLD_THIRD 7
#define STRIP_TRANSITION_STEP_RESTORE_MODELS 8
#define STRIP_TRANSITION_STEP_UNCOVER 9
#define BATTLE_SCENE_DISPATCH_ALLOCATE 0
#define BATTLE_SCENE_DISPATCH_UPDATE 1
#define BATTLE_SCENE_DISPATCH_UPDATE_ALTERNATE 2
#define BATTLE_SCENE_DISPATCH_INITIALIZE_MODE 3
#define RESTORE_BOTH_STEP_START_FIRST 0
#define RESTORE_BOTH_STEP_WAIT_FIRST 1
#define RESTORE_BOTH_STEP_START_SECOND 2
#define RESTORE_BOTH_STEP_WAIT_SECOND 3
#define RESTORE_BOTH_STEP_FINISH 4
#define ACTION_HP_STEP_SETUP 0
#define ACTION_HP_STEP_WAIT_DELAY 1
#define ACTION_HP_STEP_TRACK_REACTION 2
#define ACTION_HP_STEP_WAIT_CHECKPOINT 3
#define ACTION_HP_STEP_WAIT_HP_AND_CAMERA 4
#define ACTION_HP_STEP_WAIT_HP_AND_HOLD 5
#define ACTION_HP_STEP_COMPLETE 6
#define REPEATED_HP_STEP_START_HIT 0
#define REPEATED_HP_STEP_WAIT_HIT 1
#define REPEATED_HP_STEP_TRACK_REACTION 2
#define REPEATED_HP_STEP_WAIT_CHECKPOINT 3
#define REPEATED_HP_STEP_WAIT_ROW 4
#define REPEATED_HP_STEP_WAIT_HP_AND_CAMERA 5
#define REPEATED_HP_STEP_COMPLETE 6
#define REPEATED_HP_STEP_UNHANDLED_CAMERA_SETTLED 9
#define INTRO_STEP_SETUP_CAMERA 0
#define INTRO_STEP_SEND_FIRST 1
#define INTRO_STEP_SEND_SECOND 2
#define INTRO_STEP_NARROW_FOV 3
#define INTRO_STEP_PAN_CAMERA 4
#define INTRO_STEP_WAIT_FIRST_ANIMATION 5
#define INTRO_STEP_REVEAL_FIRST 6
#define INTRO_STEP_SETTLE_FIRST 7
#define INTRO_STEP_REVEAL_SECOND 8
#define INTRO_STEP_SETTLE_SECOND 9
#define INTRO_STEP_DONE 10
#define CONFUSION_HIT_STEP_SETUP 0
#define CONFUSION_HIT_STEP_START_HIT 1
#define CONFUSION_HIT_STEP_WAIT_REACTION 2
#define CONFUSION_HIT_STEP_BREAK_SUBSTITUTE 3
#define CONFUSION_HIT_STEP_RESOLVE_HP 4
#define CRASH_STEP_SETUP 0
#define CRASH_STEP_RESTORE_SUBSTITUTE 1
#define CRASH_STEP_WAIT_REACTION 2
#define CRASH_STEP_BREAK_SUBSTITUTE 3
#define CRASH_STEP_RESOLVE_HP 4
#define RECOIL_STEP_SETUP 0
#define RECOIL_STEP_REVEAL_USER 1
#define RECOIL_STEP_APPROACH_CAMERA 2
#define RECOIL_STEP_BREAK_TARGET_SUBSTITUTE 3
#define RECOIL_STEP_RESOLVE_HP 4
#define FAINT_STEP_BEGIN 0
#define FAINT_STEP_ANIMATE 1
#define FAINT_STEP_ADVANCE_CLOSE_CAMERA 2
#define FAINT_STEP_FINISH_CLOSE_CAMERA 3
#define FAINT_STEP_COMPLETE 4
#define FAINT_STEP_NOOP 5
#define FAINT_STEP_SETUP_WINNER_CAMERA 6
#define FAINT_STEP_WINNER_FADE 7
#define FAINT_STEP_WINNER_EXTENDED_FADE 8
#define FAINT_STEP_ALTERNATE_ANIMATION 10
#define EXPLOSION_STEP_SETUP 0
#define EXPLOSION_STEP_WAIT_ACTION 1
#define EXPLOSION_STEP_RESTORE_TARGET_SUBSTITUTE 2
#define EXPLOSION_STEP_WAIT_TARGET_REACTION 3
#define EXPLOSION_STEP_CHECK_TARGET_HP 4
#define EXPLOSION_STEP_RESOLVE_HP 5
#define TRANSFORM_STEP_REQUEST_MODEL 0
#define TRANSFORM_STEP_WAIT_MODEL 1
#define TRANSFORM_STEP_START_ROW 2
#define TRANSFORM_STEP_WAIT_ROW 3
#define SUBSTITUTE_STEP_CREATE 0
#define SUBSTITUTE_STEP_WAIT_SESSION 1
#define SUBSTITUTE_STEP_WAIT_HP_DELAY 2
#define SUBSTITUTE_STEP_COMMIT 3
#define FLY_STEP_SETUP 0
#define FLY_STEP_WAIT_ASCENT 1
#define FLY_STEP_WAIT_MAX_HEIGHT 2
#define FLY_STEP_RESOLVE_HP 3
#define FLY_STEP_POST_SEQUENCE_VISIBILITY 4
#define RESIDUAL_STEP_SETUP 0
#define RESIDUAL_STEP_START_DAMAGE 1
#define RESIDUAL_STEP_WAIT_STATUS_DAMAGE 2
#define RESIDUAL_STEP_WAIT_LEECH_DAMAGE 3
#define RESIDUAL_STEP_BREAK_SUBSTITUTE 4
#define RESIDUAL_STEP_RESOLVE_HP 5
#define SWITCH_STEP_SETUP 0
#define SWITCH_STEP_START_WITHDRAW 1
#define SWITCH_STEP_WAIT_WITHDRAW 2
#define SWITCH_STEP_WAIT_POKEBALL 3
#define SWITCH_STEP_REQUEST_MODEL 4
#define SWITCH_STEP_WAIT_MODEL 5
#define SWITCH_STEP_START_SEND_OUT 6
#define SWITCH_STEP_WAIT_SEND_OUT 7
#define SEND_OUT_STEP_SETUP 0
#define SEND_OUT_STEP_WAIT_ASSETS 1
#define SEND_OUT_STEP_WAIT_POKEBALL 2
#define SEND_OUT_STEP_REQUEST_MODEL 3
#define SEND_OUT_STEP_WAIT_MODEL 4
#define SEND_OUT_STEP_START_SEND_OUT 5
#define SEND_OUT_STEP_WAIT_SEND_OUT 6
#define ACTION_STEP_SETUP 0
#define ACTION_STEP_ANIMATE 1
#define ACTION_STEP_FINALIZE 2

typedef struct BattleScene {
    /* 0x00 */ char unk00[0x8];
    /* 0x08 */ s8 visibilityPolicy;
    /* 0x0C */ s32 bRollShotIndex;
    /* 0x10 */ s32 substepFrameCounter;
    /* 0x14 */ s32 stepFrameTimer;
    /* 0x18 */ u16 transitionStage;
    /* 0x1A */ u8 hitReactionKind;
    /* 0x1C */ s32 mode;
    /* 0x20 */ s32 phaseStep;
    /* 0x24 */ s32 modelReloadStep;
    /* 0x28 */ char unk28[0x2];
    /* 0x2A */ u16 sideCurrentlySwitching;
    /* 0x2C */ u16 actingSide;
    /* 0x2E */ u16 transitionStyle;
    /* 0x30 */ s32 transitionStep;
    /* 0x34 */ s32 actionPass;
    /* 0x38 */ s32 phase;
    /* 0x3C */ s32 phaseSubstep;
    /* 0x40 */ s32 winner;
    /* 0x44 */ s32 unk_44;
    /* 0x48 */ s32 moveEffectKind;
    /* 0x4C */ s32 rageFollowupPhase;
    /* 0x50 */ s32 cameraPresetId;
    /* 0x54 */ u8 heightShotSide;
    /* 0x56 */ s16 rotationX;
    /* 0x58 */ s16 rotationY;
    /* 0x5A */ char unk5A[0x2];
    /* 0x5C */ f32 distance;
    /* 0x60 */ f32 verticalFieldOfView;
    /* 0x64 */ char unk64[0x4];
    /* 0x68 */ f32 shakeAmplitude;
    /* 0x6C */ f32 unk_6C;
    /* 0x70 */ f32 targetDistance;
    /* 0x74 */ f32 unk_74;
    /* 0x78 */ u8 nextPhase;
    /* 0x7A */ s16 shakeAngle;
    /* 0x7C */ char unk7C[0x8];
    /* 0x84 */ f32 introEyeDistance;
    /* 0x88 */ char unk88[0xC];
    /* 0x94 */ Vec3f targetPosition;
    /* 0xA0 */ Vec3f unk_A0;
    /* 0xAC */ char unkAC[0xC];
    /* 0xB8 */ Vec3f savedCameraTarget;
    /* 0xC4 */ Vec3f savedCameraEye;
    /* 0xD0 */ Vec3f cameraAnchor;
    /* 0xDC */ CameraNode* camera;
} BattleScene; // size = 0xE0

typedef struct BattleSceneContext {
    /* 0x00 */ BattleScene* scene;
    /* 0x04 */ char unk04[0x3C];
} BattleSceneContext; // size = 0x40

typedef void (*BattleSceneStateCallback)(ActivePokemon*);

typedef struct BattleSceneStateRow {
    /* 0x00 */ s16 unk_00;
    /* 0x04 */ BattleSceneStateCallback unk_04[7];
    /* 0x20 */ s32 unk_20;
    /* 0x24 */ s32 mode;
    /* 0x28 */ s32 unk_28;
} BattleSceneStateRow; // size = 0x2C

typedef struct unk_D_843C4E44 {
    /* 0x000 */ u8 unk_000;
    /* 0x001 */ u8 unk_001;
    /* 0x3F2 */ char unk02[0x3F2];
} unk_D_843C4E44; // size >= 0x3F4

typedef struct unk_D_843C4DEC {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ char unk04[0x54];
} unk_D_843C4DEC; // size = 0x58

typedef struct BattleSceneStateTickSlot {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ BattleSceneStateCallback callback;
} BattleSceneStateTickSlot; // size = 0x8

typedef struct TagTeamPartners {
    /* 0x00 */ u8 firstPartner;
    /* 0x01 */ u8 secondPartner;
    /* 0x02 */ char names[14];
} TagTeamPartners; // size = 0x10

typedef struct unk_D_84390178 {
    /* 0x00 */ s32 timeLeft;
    /* 0x04 */ char unk04[0x14];
} unk_D_84390178; // size >= 0x18

typedef struct SpeciesModelTransform {
    /* 0x00 */ Vec3f coordinates;
    /* 0x0C */ s16 rotationX;
    /* 0x0E */ s16 rotationY;
    /* 0x10 */ s16 unk_10;
    /* 0x12 */ u16 idleTimer;
    /* 0x14 */ char unk14[0xC];
} SpeciesModelTransform; // size = 0x20

typedef struct unk_D_84390028 {
    /* 0x00 */ f32 width;
    /* 0x04 */ f32 height;
    /* 0x08 */ f32 restingHeight;
    /* 0x0C */ f32 cameraTargetOffsetY;
} ModelDimensions; // size = 0x10

typedef struct unk_D_84384608 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ u16 unk_04;
} unk_D_84384608; // size = 0x8

typedef struct unk_D_843847BC {
    /* 0x00 */ s16 x;
    /* 0x02 */ s16 y;
    /* 0x04 */ s16 width;
    /* 0x08 */ f32 verticalFieldOfView;
    /* 0x0C */ f32 nearPlane;
    /* 0x10 */ f32 farPlane;
    /* 0x14 */ s16 unk_14;
} ThumbnailSettings; // size = 0x18

typedef struct BattleCameraPreset {
    /* 0x00 */ Vec3f startingPosition;
    /* 0x0C */ s16 startingRotationX;
    /* 0x0E */ s16 startingRotationY;
    /* 0x10 */ char unk10[0x2];
    /* 0x12 */ s16 eyeRotationX;
    /* 0x14 */ s16 eyeRotationY;
    /* 0x18 */ f32 startingDistance;
    /* 0x1C */ f32 eyeDistance;
    /* 0x20 */ s16 rotationXOffset;
    /* 0x22 */ s16 rotationYOffset;
    /* 0x24 */ char unk24[0x2];
    /* 0x26 */ s16 rotationX;
    /* 0x28 */ s16 rotationY;
    /* 0x2C */ f32 distanceOffset;
    /* 0x30 */ f32 distance;
    /* 0x34 */ f32 targetZ;
    /* 0x38 */ f32 speed;
    /* 0x3C */ f32 verticalFieldOfView;
} BattleCameraPreset; // size >= 0x40

typedef struct BattleBox {
    /* 0x00 */ s32 visibility;
    /* 0x04 */ s32 openingProgress;
    /* 0x08 */ s32 displayTimer;
    /* 0x0C */ s32 side;
    /* 0x10 */ s32 lineCount;
    /* 0x14 */ Vec3f textOrigin;
    /* 0x20 */ f32 cornerDistance;
    /* 0x24 */ f32 currentDistance;
    /* 0x28 */ s16 rotationX;
    /* 0x2A */ s16 rotationY;
} BattleBox; // size = 0x2C

typedef struct unk_D_843901A0 {
    /* 0x000 */ u8 controllerPort;
    /* 0x004 */ s32 unk_004;
    /* 0x008 */ u8 unk_008[4];
    /* 0x00C */ char unk00C[0x3C];
    /* 0x048 */ s8 buffer[18][64];
    /* 0x4C8 */ BattleBox textBox;
} BattleMessageQueue; // size = 0x4F4

typedef struct StatusConditionLabel {
    /* 0x00 */ s32 stringIndex;
    /* 0x04 */ Color_RGBA8 color;
} StatusConditionLabel; // size = 0x8

typedef struct arg1_func_84344CE8 {
    /* 0x00 */ char unk00[0x14];
    /* 0x14 */ u8* unk_14;
    /* 0x18 */ Gfx* unk_18;
} arg1_func_84344CE8; // size >= 0x1C

typedef struct arg1_func_8434E21C {
    /* 0x00 */ u8* unk_00;
    /* 0x04 */ u8* unk_04;
    /* 0x08 */ char unk08[0x2];
    /* 0x0A */ s16 unk_0A;
} arg1_func_8434E21C; // size >= 0xC

typedef struct RibbonMeshVertexState {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ s16 unk_0C;
    /* 0x0E */ s16 unk_0E;
    /* 0x10 */ Color_RGBA8 unk_10;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ f32 unk_18;
} RibbonMeshVertexState; // size = 0x1C

typedef struct RibbonMesh {
    /* 0x000 */ s32 unk_000;
    /* 0x004 */ f32 unk_004;
    /* 0x008 */ f32 unk_008;
    /* 0x00C */ f32 unk_00C;
    /* 0x010 */ f32 unk_010;
    /* 0x014 */ f32 unk_014;
    /* 0x018 */ s16 unk_018;
    /* 0x01A */ s16 unk_01A;
    /* 0x01C */ s16 unk_01C;
    /* 0x01E */ s16 unk_01E;
    /* 0x020 */ f32 unk_020;
    /* 0x024 */ s32 unk_024;
    /* 0x028 */ s32 unk_028;
    /* 0x02C */ s32 unk_02C;
    /* 0x030 */ s32 unk_030;
    /* 0x034 */ s32 unk_034;
    /* 0x038 */ s32 unk_038;
    /* 0x03C */ s32 unk_03C;
    /* 0x040 */ s32 unk_040;
    /* 0x044 */ s32 unk_044;
    /* 0x048 */ char unk048[0x8];
    /* 0x050 */ Vtx unk_050[31 * 2];
    /* 0x430 */ RibbonMeshVertexState unk_430[31 * 2];
} RibbonMesh; // size = 0xAF8

typedef struct unk_D_843975E8 {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ f32 unk_1C;
    /* 0x20 */ f32 unk_20;
    /* 0x24 */ char unk24[0x4];
} unk_D_843975E8; // size = 0x28

typedef struct Radial20Node {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ u8 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ Vec3f unk_10;
    /* 0x1C */ Vec3f unk_1C;
    /* 0x28 */ char unk28[0x20];
} Radial20Node; // size = 0x48

typedef struct Radial20 {
    /* 0x000 */ s16 unk_000;
    /* 0x002 */ s16 unk_002;
    /* 0x004 */ s16 unk_004;
    /* 0x006 */ s16 unk_006;
    /* 0x008 */ s16 unk_008;
    /* 0x00A */ Color_RGBA8 unk_00A;
    /* 0x00E */ Color_RGB8 unk_00E;
    /* 0x014 */ f32 unk_014;
    /* 0x018 */ f32 unk_018;
    /* 0x01C */ Vtx* unk_01C;
    /* 0x020 */ char unk020[0x18];
    /* 0x038 */ s32 unk_038[3];
    /* 0x044 */ char unk044[0x4];
    /* 0x048 */ Radial20Node unk_048[20];
} Radial20; // size = 0x5E8

typedef struct Trail40Node {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ s16 unk_18;
    /* 0x1A */ s16 unk_1A;
} Trail40Node; // size = 0x1C

typedef struct Trail40 {
    /* 0x0000 */ s16 unk_0000;
    /* 0x0002 */ s16 unk_0002;
    /* 0x0004 */ s16 unk_0004;
    /* 0x0008 */ f32 unk_0008;
    /* 0x000C */ Vtx* unk_000C;
    /* 0x0010 */ Mtx* unk_0010;
    /* 0x0014 */ Trail40Node unk_0014[16][16];
} Trail40; // size >= 0x1C14

typedef struct Trail40VariantNode {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ s16 unk_18;
    /* 0x1A */ s16 unk_1A;
    /* 0x1C */ f32 unk_1C;
} Trail40VariantNode; // size = 0x20

typedef struct Trail40Variant {
    /* 0x0000 */ s16 unk_0000;
    /* 0x0002 */ s16 unk_0002;
    /* 0x0004 */ s16 unk_0004;
    /* 0x0008 */ f32 unk_0008;
    /* 0x000C */ Vtx* unk_000C;
    /* 0x0010 */ Mtx* unk_0010;
    /* 0x0014 */ Trail40VariantNode unk_0014[16][16];
} Trail40Variant; // size >= 0x2014

typedef struct TexturedRibbonSheetNode {
    /* 0x00 */ f32 unk_00;
    /* 0x08 */ u8 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ Vec3f unk_0C;
    /* 0x18 */ Vec3f unk_18;
    /* 0x24 */ Vec3f unk_24;
    /* 0x30 */ Vtx unk_30[2];
} TexturedRibbonSheetNode; // size = 0x50

typedef struct TexturedRibbonSheet {
    /* 0x000 */ s16 unk_000;
    /* 0x002 */ s16 unk_002;
    /* 0x004 */ s16 unk_004;
    /* 0x006 */ s16 unk_006;
    /* 0x008 */ s16 unk_008;
    /* 0x00A */ Color_RGB8 unk_00A;
    /* 0x010 */ f32 unk_010;
    /* 0x014 */ f32 unk_014;
    /* 0x018 */ Vtx* unk_018;
    /* 0x01C */ char unk01C[0x1C];
    /* 0x038 */ TexturedRibbonSheetNode unk_038[10];
} TexturedRibbonSheet; // size = 0x358

typedef struct FourStreamTrailNode {
    /* 0x00 */ u8 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ Vec3f unk_0C;
    /* 0x18 */ Vec3f unk_18;
    /* 0x24 */ f32 unk_24;
    /* 0x28 */ f32 unk_28;
    /* 0x2C */ Vec3f unk_2C;
    /* 0x38 */ Vec3f unk_38;
} FourStreamTrailNode; // size = 0x44

typedef struct FourStreamTrail {
    /* 0x000 */ s16 unk_000;
    /* 0x002 */ s16 unk_002;
    /* 0x004 */ s16 unk_004;
    /* 0x006 */ s16 unk_006;
    /* 0x008 */ Color_RGB8 unk_008;
    /* 0x00C */ f32 unk_00C;
    /* 0x010 */ f32 unk_010;
    /* 0x014 */ f32 unk_014;
    /* 0x018 */ Vtx* unk_018;
    /* 0x01C */ Vec3f unk_01C;
    /* 0x028 */ char unk028[0xC];
    /* 0x034 */ FourStreamTrailNode unk_034[15];
} FourStreamTrail; // size = 0x430

typedef struct OwnerLinkedFourStreamNode {
    /* 0x00 */ u8 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ Vec3f unk_0C;
    /* 0x18 */ Vec3f unk_18;
    /* 0x24 */ f32 unk_24;
    /* 0x28 */ f32 unk_28;
    /* 0x2C */ Vec3f unk_2C;
    /* 0x38 */ Vec3f unk_38;
} OwnerLinkedFourStreamNode; // size = 0x44

typedef struct OwnerLinkedFourStream {
    /* 0x000 */ s16 unk_000;
    /* 0x002 */ s16 unk_002;
    /* 0x004 */ s16 unk_004;
    /* 0x006 */ s16 unk_006;
    /* 0x008 */ Color_RGB8 unk_008;
    /* 0x00C */ f32 unk_00C;
    /* 0x010 */ f32 unk_010;
    /* 0x014 */ f32 unk_014;
    /* 0x018 */ Vtx* unk_018;
    /* 0x01C */ Vec3f unk_01C;
    /* 0x028 */ char unk028[0xC];
    /* 0x034 */ Particle* unk_034;
    /* 0x038 */ OwnerLinkedFourStreamNode unk_038[15];
} OwnerLinkedFourStream; // size = 0x434

typedef struct OwnerAnchoredFourStreamNode {
    /* 0x00 */ u8 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ Vec3f unk_0C;
    /* 0x18 */ Vec3f unk_18;
    /* 0x24 */ f32 unk_24;
    /* 0x28 */ f32 unk_28;
    /* 0x2C */ Vec3f unk_2C;
    /* 0x38 */ Vec3f unk_38;
} OwnerAnchoredFourStreamNode; // size = 0x44

typedef struct OwnerAnchoredFourStream {
    /* 0x000 */ s16 unk_000;
    /* 0x002 */ s16 unk_002;
    /* 0x004 */ s16 unk_004;
    /* 0x006 */ s16 unk_006;
    /* 0x008 */ Color_RGB8 unk_008;
    /* 0x010 */ f32 unk_00C;
    /* 0x010 */ f32 unk_010;
    /* 0x014 */ Vtx* unk_014;
    /* 0x018 */ Particle* unk_018;
    /* 0x01C */ OwnerAnchoredFourStreamNode unk_01C[15];
    /* 0x418 */ f32 unk_418;
} OwnerAnchoredFourStream; // size = 0x41C

typedef struct Trail50EffectNode {
    /* 0x00 */ u8 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ Vec3f unk_0C;
    /* 0x18 */ Vec3f unk_18;
    /* 0x24 */ Vec3f unk_24;
    /* 0x30 */ f32 unk_30;
    /* 0x34 */ f32 unk_34;
    /* 0x38 */ Vec3f unk_38;
    /* 0x44 */ Vec3f unk_44;
} Trail50EffectNode; // size = 0x50

typedef struct Trail50Effect {
    /* 0x000 */ s16 unk_000;
    /* 0x002 */ s16 unk_002;
    /* 0x004 */ s16 unk_004;
    /* 0x006 */ s16 unk_006;
    /* 0x008 */ s16 unk_008;
    /* 0x00A */ Color_RGB8 unk_00A;
    /* 0x010 */ f32 unk_010;
    /* 0x014 */ f32 unk_014;
    /* 0x018 */ f32 unk_018;
    /* 0x01C */ char unk01C[0x4];
    /* 0x020 */ Vtx* unk_020;
    /* 0x024 */ Vec3f unk_024;
    /* 0x030 */ char unk030[0x10];
    /* 0x040 */ Trail50EffectNode unk_040[10];
} Trail50Effect; // size = 0x360

typedef struct unk_D_843C2C00_0B0 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ u8 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ Vec3f unk_10;
    /* 0x1C */ f32 unk_1C;
    /* 0x20 */ f32 unk_20;
    /* 0x24 */ f32 unk_24;
} unk_D_843C2C00_0B0; // size = 0x28

typedef struct unk_D_843C2C00_064 {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A;
} unk_D_843C2C00_064; // size = 0xC

typedef struct unk_D_843C2C00_024 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ s32 unk_0C;
    /* 0x10 */ s32 unk_10;
    /* 0x14 */ s32 unk_14;
    /* 0x18 */ s32 unk_18;
    /* 0x1C */ s32 unk_1C;
} unk_D_843C2C00_024; // size = 0x20

typedef struct unk_D_84389CE0 {
    /* 0x00 */ Color_RGBA8 unk_00;
    /* 0x04 */ u8 unk_04;
} unk_D_84389CE0; // size = 0x8

typedef struct DoubleTextureTrail {
    /* 0x000 */ s16 unk_000;
    /* 0x002 */ s16 unk_002;
    /* 0x004 */ s16 unk_004;
    /* 0x008 */ f32 unk_008;
    /* 0x00C */ f32 unk_00C;
    /* 0x010 */ f32 unk_010;
    /* 0x014 */ f32 unk_014;
    /* 0x018 */ Vtx* unk_018;
    /* 0x01C */ s32 unk_01C;
    /* 0x020 */ s32 unk_020;
    /* 0x024 */ unk_D_843C2C00_024 unk_024;
    /* 0x044 */ unk_D_843C2C00_024 unk_044;
    /* 0x064 */ unk_D_843C2C00_064 unk_064;
    /* 0x070 */ unk_D_843C2C00_064 unk_070;
    /* 0x07C */ unk_D_84389CE0 unk_07C;
    /* 0x081 */ unk_D_84389CE0 unk_081;
    /* 0x086 */ Color_RGBA8 unk_086;
    /* 0x08C */ f32 unk_08C;
    /* 0x090 */ f32 unk_090;
    /* 0x094 */ s16 unk_094;
    /* 0x096 */ char unk096[0x1A];
    /* 0x0B0 */ unk_D_843C2C00_0B0 unk_0B0[10];
} DoubleTextureTrail; // size = 0x240

typedef struct AuxiliaryEffectNode {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ Vec3f unk_0C;
} AuxiliaryEffectNode; // size = 0x18

typedef struct AuxiliaryEffect {
    /* 0x000 */ s16 unk_000;
    /* 0x002 */ char pad002[2];
    /* 0x004 */ AuxiliaryEffectNode unk_004[20];
} AuxiliaryEffect; // size = 0x1E4

typedef struct unk_D_843C3508_0A4 {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ Vec3f unk_0C;
    /* 0x18 */ Vec3f unk_18;
} unk_D_843C3508_0A4; // size = 0x24

typedef struct unk_D_843C3508 {
    /* 0x000 */ s16 unk_000;
    /* 0x002 */ s16 unk_002;
    /* 0x004 */ s16 unk_004;
    /* 0x006 */ s16 unk_006;
    /* 0x008 */ unk_D_84389CE0 unk_008;
    /* 0x00D */ unk_D_84389CE0 unk_00D;
    /* 0x014 */ f32 unk_014;
    /* 0x018 */ f32 unk_018;
    /* 0x01C */ s16 unk_01C;
    /* 0x020 */ s32 unk_020;
    /* 0x024 */ s32 unk_024;
    /* 0x028 */ unk_D_843C2C00_024 unk_028;
    /* 0x048 */ unk_D_843C2C00_024 unk_048;
    /* 0x068 */ unk_D_843C2C00_064 unk_068;
    /* 0x074 */ unk_D_843C2C00_064 unk_074;
    /* 0x080 */ f32 unk_080;
    /* 0x084 */ f32 unk_084;
    /* 0x088 */ Vtx* unk_088;
    /* 0x08C */ char unk08C[0x18];
    /* 0x0A4 */ unk_D_843C3508_0A4 unk_0A4[20];
} unk_D_843C3508; // size = 0x374

typedef struct StatStageMultiplier {
    /* 0x00 */ u8 multiplier;
    /* 0x01 */ u8 devisor;
} StatStageMultiplier; // size = 0x2

typedef struct TypeEffectivenessEntry {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
} TypeEffectivenessEntry; // size = 0x3

typedef struct BattleAiMonState {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x03 */ u8 unk_03;
    /* 0x04 */ u8 unk_04;
    /* 0x05 */ u8 unk_05[1];
    /* 0x0C */ char unk06[0xC];
    /* 0x12 */ BattleMonRuntime unk_12;
} BattleAiMonState; // size = 0x76

typedef struct BattleAiTeamState {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01[2];
    /* 0x03 */ u8 unk_03;
    /* 0x04 */ u8 unk_04[2];
    /* 0x06 */ u8 unk_06;
    /* 0x07 */ u8 unk_07[1];
    /* 0x08 */ u8 unk_08;
    /* 0x09 */ u8 unk_09;
    /* 0x0A */ u8 unk_0A[1];
    /* 0x0B */ char unk0B[0x5];
    /* 0x10 */ u8 unk_10;
    /* 0x11 */ u8 unk_11;
    /* 0x12 */ u8 unk_12;
    /* 0x13 */ u8 unk_13;
    /* 0x14 */ BattleAiMonState unk_14[1];
} BattleAiTeamState; // size >= 0x8A

typedef struct unk_D_843C5568 {
    /* 0x000 */ u8 unk_000;
    /* 0x001 */ u8 unk_001;
    /* 0x002 */ u8 unk_002;
    /* 0x003 */ u8 unk_003;
    /* 0x004 */ u8 unk_004;
    /* 0x005 */ u8 unk_005;
    /* 0x006 */ u8 unk_006;
    /* 0x007 */ u8 unk_007;
    /* 0x008 */ u8 unk_008;
    /* 0x009 */ char unk009[0x3];
    /* 0x00C */ u8 unk_00C[1];
    /* 0x00D */ char unk00D[0x5];
    /* 0x012 */ u8 unk_012;
    /* 0x013 */ char unk013[0x3];
    /* 0x016 */ BattleAiMonState unk_016[1];
    /* 0x08C */ char unk08C[0x510];
} unk_D_843C5568; // size = 0x59C

typedef struct BattleAiScoredMove {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x03 */ u8 unk_03;
    /* 0x04 */ u8 unk_04;
    /* 0x05 */ u8 unk_05;
    /* 0x06 */ u8 unk_06;
    /* 0x07 */ u8 unk_07;
    /* 0x08 */ u8 unk_08;
    /* 0x0A */ s16 unk_0A;
    /* 0x0C */ s16 unk_0C;
    /* 0x0E */ s16 unk_0E;
    /* 0x10 */ s16 unk_10;
    /* 0x12 */ s16 unk_12;
    /* 0x14 */ s16 unk_14;
    /* 0x16 */ s16 unk_16;
    /* 0x18 */ s16 unk_18;
    /* 0x1A */ s16 unk_1A;
    /* 0x1C */ s16 unk_1C;
    /* 0x1E */ s16 unk_1E;
    /* 0x20 */ s16 unk_20;
    /* 0x24 */ s32 unk_24;
    /* 0x28 */ s32 unk_28;
} BattleAiScoredMove; // size = 0x2C

typedef struct SpeciesLevelUpLearnset {
    /* 0x00 */ u8 unk_00[1];
    /* 0x01 */ char unk01[9];
    /* 0x0A */ u8 unk_0A[1];
    /* 0x0B */ char unk0B[0x15];
} SpeciesLevelUpLearnset; // size = 0x20

typedef struct AIDecisionScratch {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x04 */ s32 unk_04[1];
    /* 0x08 */ char unk08[0xC];
} AIDecisionScratch; // size = 0x14

typedef struct AIDifficultyConfig {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ u8 unk_04;
    /* 0x05 */ u8 unk_05;
    /* 0x06 */ u8 unk_06;
    /* 0x07 */ u8 unk_07;
    /* 0x08 */ u8 unk_08;
    /* 0x09 */ u8 unk_09;
    /* 0x0A */ u8 unk_0A;
    /* 0x0B */ u8 unk_0B;
    /* 0x0C */ u8 unk_0C;
    /* 0x0D */ u8 unk_0D;
    /* 0x0E */ u8 unk_0E;
    /* 0x0F */ u8 unk_0F;
    /* 0x10 */ u8 unk_10;
    /* 0x11 */ char unk11[0x3];
} AIDifficultyConfig; // size = 0x14

typedef struct unk_D_843C60F0 {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x03 */ char unk03[0x9];
    /* 0x0C */ AIDecisionScratch unk_0C[3];
} unk_D_843C60F0; // size >= 0x48

typedef struct unk_D_843C6148 {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x03 */ u8 unk_03;
    /* 0x04 */ u8 unk_04;
    /* 0x05 */ u8 unk_05;
    /* 0x06 */ char unk06[0x2];
} unk_D_843C6148; // size <= 0x8

typedef struct unk_func_8438220C {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ char unk01[0x3];
    /* 0x04 */ BattleAiScoredMove unk_04[4];
    /* 0xB4 */ u8 unk_B4;
    /* 0xB8 */ s32 unk_B8;
} unk_func_8438220C; // size = 0xBC

typedef struct unk_func_8437F85C_arg3 {
    /* 0x00 */ u8 unk_00[3][3];
} unk_func_8437F85C_arg3; // size = 0x3

#define NINETY_DEGREES 0x4000

extern PokemonBattleCues* sSideBattleCues[];
extern SpeciesModelTransform* sSidePresentationTransform[];
extern SpeciesModelTransform* sSideCameraTransform[];
extern TagTeamPartners* sPlayerTagTeam[];
extern u8 sPokemonWithAlternateSleepIdle[];
extern u8 sAerodactyl;
extern u8 sFearow;
extern u8 sFarfetchD;
extern u8 sDoduoEvoLine[];
extern s16 sChargeMoveCues[];
extern ThumbnailSettings sSideThumbnailSettings[2];
extern DioramaAnimData D_843847EC[3];
extern DioramaAnimData D_84384888[2];
extern DioramaAnimData sSwitchCameraPreset;
extern DioramaAnimData D_84384924;
extern DioramaAnimData D_84384958;
extern DioramaAnimData D_8438498C;
extern DioramaAnimData D_843849C0[];
extern DioramaAnimData D_84384AF8;
extern DioramaAnimData D_84384B2C[];
extern Vec3f sModelCameraAdjustments[];
extern BattleSceneStateCallback sBattleSceneStateTickSentinel;
extern BattleSceneStateRow sBattleSceneStateRowTable[];
extern Vec3f s100UnitsUpFromCenter;
extern Vec3f s10UnitsUpFromCenter;
extern u32 sBattleMessagesTableFileNumbers[];
extern Vtx D_84385C80[];
extern Vtx D_84385CC0[];
extern Vtx D_84385D00[];
extern Vtx D_84385D40[];
extern Vtx D_84385DC0[];
extern Vtx D_84385E00[];
extern ParticleDescriptor gBattleAnimParticleDescriptors[86];
extern func_D_84390320 gMoveAnimEffectStartFuncs[];
extern func_D_84390320 gMoveAnimEffectEndFuncs[];
extern func_D_84390320 gMoveAnimEffectStartUpdateFuncs[];
extern func_D_84390320 gMoveAnimEffectEndUpdateFuncs[];
extern func_D_84390320 gMoveAnimEffectStartDrawFuncs[];
extern func_D_84390320 gMoveAnimEffectEndDrawFuncs[];
extern Gfx D_84389AE0[];
extern u32 D_8438A648[];
extern Gfx D_8438A980[];
extern u8 sBattleMechanicsSetting[];
extern StatStageMultiplier sStatStageMultipliers[];
extern u8 D_8438ACD4[];
extern u8 sTypeEffects[];
extern u8* sSide1PokemonThumbnailImage;
extern u8* sSide2PokemonThumbnailImage;

extern u8 sBattleMoveFailed;
extern u8 sBattleCritFlag;
extern u8 D_843C4DA9;
extern u8 D_843C4DAA;
extern s32 sTypeEffectiveness;
// extern unk_D_843C4E44 D_843C4E44;
extern u8 sConfused;
extern u8 sStatCanBeChangedFurther;
extern u16 sBattleDamage;
extern BattleMonRuntime* sBattleUser;
extern BattleMonRuntime* sBattleTarget;

extern ActivePokemon* sActivePokemon[2];
extern ActivePokemon* sSide1ActivePokemon;
extern ActivePokemon* sSide2ActivePokemon;
extern ModelDimensions sPokemonModelDimensions[2];
extern f32 sCurrentPokemonHp[];
extern char** sBattleMessagesTable;
extern char** sPokemonNameTable;
extern char** sMoveNameTable;
extern char** sStatusConditionNameTable;
extern char** sBattleControllerDisconnectedMessage;
extern f32 sOrderTimer;
extern f32 sBattleTimer;
extern u8 D_84390134;
extern s32 sAttractInputIndex;
extern unk_D_84390178 sAttractScreenTimer;
extern Vec3f sEndBattleBannerPos;
extern BattleMessageQueue* sBattleMessageQueues;
extern BattleCameraPreset gCameraPreset;
extern ActivePokemon* sNonActingPokemon;
extern ActivePokemon* sActingPokemon;
extern s32 sMultihitCount;
extern s32 sMultihitCountdown;
extern s32 sMultihitEffectCountdown;
extern BattleSceneContext sBattleScene;
extern s32 D_84390288;
extern s32 gBattleAnimFrameCounter;
extern s32 gBattleAnimSpawnDelay;
extern s16 gBattleAnimCurrentAnchor;
extern s16 gBattleAnimDefaultAnchor;
extern ActivePokemon* gBattleAnimCurrentOwner;
extern ActivePokemon* gBattleAnimSecondaryOwner;
extern ActivePokemon* gBattleAnimTertiaryOwner;
extern s32 D_843902F4;
extern s32 D_843902F8;
extern func_D_84390300 gBattleAnimEffectUpdateFuncs[8];
extern func_D_84390320 gBattleAnimEffectDrawFuncs[8];
extern s32 D_84390340;
extern Vec3f D_84390348;
extern s16 gSavedPrimColorR;
extern s16 gSavedPrimColorG;
extern s16 gSavedPrimColorB;
extern s16 gSavedPrimColorA;
extern s16 gPrimColorR;
extern s16 gPrimColorG;
extern s16 gPrimColorB;
extern s16 gPrimColorA;
extern s16 gEnvColorR;
extern s16 gEnvColorG;
extern s16 gEnvColorB;
extern s16 gEnvColorA;
extern s16 gFogColorR;
extern s16 gFogColorG;
extern s16 gFogColorB;
extern s16 gFogColorA;
extern s16 gBlendColorR;
extern s16 gBlendColorG;
extern s16 gBlendColorB;
extern s16 gBlendColorA;
extern f32 D_84392058[26];
extern u8* sBattleAnimAssetPointers[256];
extern s16 sBattleAnimResourceList[0x10];
extern arg1_func_80019420* sBattleAnimResourceTablePointers[40];

//@src/fragments/battle_engine/battle_engine_main.c
s32 Battle_StageGeoDispatchCallback(s32 event, GraphNode* node);
s32 Battle_StageLightAngleCallbackA(s32 event, unk_D_86002F34_alt18* light);
s32 Battle_StageLightAngleCallbackB(s32 event, unk_D_86002F34_alt18* light);
s32 Battle_StageMaterialTintCallback(s32 event, DisplayObject* object);
void Battle_SetStageTintColor(u8 r, u8 g, u8 b);
void Battle_SetStageScale(f32 scale);
u16 Battle_TintTexel16(u16 color);
void Battle_DrawStageBackdrop(void);
void Battle_RenderActivePokemonThumbnail(GraphNode* thumbnail_root, DepthBuffer* thumbnail);
void Battle_DrawClippedTextureRect(s32 x1, s32 y1, s32 x2, s32 y2);
void Battle_DrawTexturedRectSegment(s32 x1, s32 y1, s32 draw_width, s32 height, u8* texture, s32 load_width);
void Battle_FillActivePokemonThumbnail(GraphNode* thumbnail_root, DepthBuffer* thumbnail);
void Battle_ResetFaintBatches(void);
void Battle_UpdateFaintBatches(void);
void func_84300E78(void);
void func_84300E80(void);
s32 BattleScene_UpdateFrame(s32 fade_mode);
s32 BattleScene_UpdatePausedFrame(s32 fade_mode);
void Battle_UpdateCompletionGate(void);
s32 Battle_FrameCallback(s32 fade_mode);
void Battle_CompleteRosterOrder(u8* roster_order, s32 number_of_pokemon_picked);
void Battle_SelectComRoster(void);
void BattleScene_InitializeParticipantPresentation(BattleSessionTeams* side, unk_D_86002F30* model);
void BattleScene_Init(StageContext* stage_context);
void func_84301A24(void);
s32 Battle_Main(s32 arg0, SessionContext* arg1);

//@src/fragments/battle_engine/battle_engine_move_animation.c
void BattleScene_ClearStateTickCallbacks(void);
void BattleScene_QueueRowEffectList3(ActivePokemon* actor, s32 effect_entry);
void BattleAnim_PlayMoveEffectScript(ActivePokemon* user, s32 move_id);
void BattleAnim_PlayChargingMoveEffectScript(ActivePokemon* user, s32 move_id);
void BattleAnim_PlayMoveEffectEnd(ActivePokemon* user, s32 move_id);
void BattleScene_TickClearAnimRecordIfLoaded(ActivePokemon* active_pokemon);
void BattleScene_TickStartRowAnimRecord(ActivePokemon* active_pokemon);
void BattleScene_TickQueueRowEffectList3AtAnimEnd(ActivePokemon* active_pokemon);
void BattleScene_ResetStateFrameCounter(ActivePokemon* active_pokemon);
void BattleScene_ResetOpponentStateFrameCounter(ActivePokemon* active_pokemon);
void BattleAnim_SetFlagForRockThrowingMoves(ActivePokemon* active_pokemon);
void BattleAnim_ClearRockThrowingMoveFlag(ActivePokemon* active_pokemon);
s32 BattleAnim_IsAssetLoadBusy(ActivePokemon* active_pokemon);
s32 BattleAnim_PollAssetLoadBusyFlags(ActivePokemon* active_pokemon);
s32 BattleAnim_IsSessionBusy(ActivePokemon* active_pokemon);
s32 BattleAnim_TryRegisterAssetTables(ActivePokemon* active_pokemon);
void BattleScene_LoadMoveResourceList(ActivePokemon* active_pokemon);
void BattleScene_LoadRowResourceList(ActivePokemon* active_pokemon);
void BattleScene_ReloadMoveResourceList(ActivePokemon* active_pokemon);
void Battle_MakeSubstitute(ActivePokemon* active_pokemon);
void BattleScene_DestroySubstitute(ActivePokemon* active_pokemon);
s32 BattleScene_FindU16InByteArray(u16 lookup_value, u8* array, s32 array_length);
void Battle_MarkIllegalMoves(ActivePokemon* pokemon, u16 pokemon_id, SpeciesLearnsetBuffer* red_blue_learnset,
                             SpeciesLearnsetBuffer* yellow_learnset);
void Battle_InitializeMoveIllegalityMask(ActivePokemon* active_pokemon);
void Battle_DmaLoadLevelUpLearnset(u8 species, u32 destination);
void Battle_DmaLoadSpeciesLearnsets(s32 species_index, SpeciesLearnsetBuffer* red_blue_learnset,
                                    SpeciesLearnsetBuffer* yellow_learnset);
void Battle_LoadAnimationAndMarkMoveIllegality(ActivePokemon* pokemon, s32 mark_move_illegality);
void BattleScene_TickRow10CleanupAndEnterState18(ActivePokemon* active_pokemon);
void BattleScene_EnterRow3StartMoveAnim(ActivePokemon* active_pokemon);
void BattleScene_TickRow3PlayMoveEffectScript(ActivePokemon* active_pokemon);
void BattleScene_TickRow3PostEffectCamera(ActivePokemon* active_pokemon);
void Battle_HandleStatBoostingMoveAnimation(ActivePokemon* active_pokemon);
void BattleScene_MakeGoldenWhenHarden(ActivePokemon* active_pokemon);
void BattleScene_EnterRow21StartAnimRecord0(ActivePokemon* active_pokemon);
void BattleScene_TickRow21PlaySpeciesSound(ActivePokemon* active_pokemon);
void BattleScene_EnterRow19StartMoveAnimMidFrame(ActivePokemon* active_pokemon);
void BattleScene_TickRow19PlayMoveEffectScript(ActivePokemon* active_pokemon);
void BattleScene_TickRow19PostEffectCamera(ActivePokemon* active_pokemon);
void BattleScene_StartHitReactionAnim(ActivePokemon* active_pokemon);
void BattleScene_HandleLassoMoves(ActivePokemon* active_pokemon);
void BattleScene_TickRow4PostEndCamera(ActivePokemon* active_pokemon);
void BattleScene_EnterRow15StartAnimA80Seq30(ActivePokemon* active_pokemon);
s32 BattleScene_IsOpponentRowCheckpointReached(ActivePokemon* active_pokemon);
s32 BattleScene_IsRowZeroCheckpointReached(ActivePokemon* active_pokemon);
void BattleScene_StartHitCueAnimation(ActivePokemon* active_pokemon);
void BattleScene_EnterRowAnim166WithCry(ActivePokemon* active_pokemon);
void BattleScene_TickRow1PlayOwnerCry(ActivePokemon* active_pokemon);
void BattleScene_TickRow16PlayCryAndSpeciesSound(ActivePokemon* active_pokemon);
void BattleScene_TickRow18PlayCryAndSpeciesSound(ActivePokemon* active_pokemon);
void BattleScene_TickAssetLoadQueueRowEffectList3(ActivePokemon* active_pokemon);
void BattleScene_TickQueueRowEffectList3OnFirstFrame(ActivePokemon* active_pokemon);
void BattleScene_TickRow20CleanupAndEnterState18(ActivePokemon* active_pokemon);
void BattleScene_ResetOwnerModelIfLoaded(ActivePokemon* active_pokemon);
void BattleScene_EnterRow8StartMappedMoveAnim(ActivePokemon* active_pokemon);
void BattleScene_TickRow8PlayMoveEffectScriptB(ActivePokemon* active_pokemon);
void BattleScene_EnterRow9SetEventTrack(ActivePokemon* active_pokemon);
void BattleScene_EnterRow9QueueEffectList6(ActivePokemon* active_pokemon);
void BattleScene_Row12Slot1CameraCleanup(ActivePokemon* active_pokemon);
void BattleScene_Row13PlaySequences14And21(ActivePokemon* active_pokemon);
void BattleScene_TickPlaySpeciesSequence33(ActivePokemon* active_pokemon);
void BattleScene_TickQueueList3Selector2OrC(ActivePokemon* active_pokemon);
void BattleScene_TickQueueList3Selector2OrCFromB10(ActivePokemon* active_pokemon);
void BattleScene_EnterRow11StartHitReactionAnim(ActivePokemon* active_pokemon);
void BattleScene_TickRow14PlayMoveEffectEnd(ActivePokemon* active_pokemon);
s32 BattleScene_GetOpponentEffectCheckpoint(ActivePokemon* active_pokemon);
void BattleScene_TickRow11QueueEffectList6(ActivePokemon* active_pokemon);
void BattleScene_TickRow11PostEndCamera(ActivePokemon* active_pokemon);
void BattleScene_StateCallbackNop(ActivePokemon* active_pokemon);
void BattleScene_RegisterStateTickCallback(ActivePokemon* active_pokemon, BattleSceneStateCallback callback);
void BattleScene_SetOpponentEventTrackA6(ActivePokemon* active_pokemon);
void BattleScene_EnterStateRow(ActivePokemon* active_pokemon, s32 row_index);
void BattleScene_TickStateCallbacks(void);
void Battle_StopBeingGoldAfterHarden(ActivePokemon* active_pokemon);
void BattleAnim_SetModelEventTrackFromRow(ActivePokemon* pokemon, s32 cue_index);
s32 BattleAnim_TrySetModelEventTrackFromRow(ActivePokemon* active_pokemon, s32 cue_index, s32 event_state);
void BattleAnim_SetModelEventTrackAndState(ActivePokemon* active_pokemon, s32 cue_index, s32 event_state);
s32 BattleScene_IsParticipantIncapacitated(ActivePokemon* active_pokemon);
s32 BattleScene_IsRowWaitComplete(ActivePokemon* active_pokemon, s32 frame_limit);
s32 BattleScene_IsRowWaitCompleteWithEventCount(ActivePokemon* active_pokemon, s32 frame_limit);
s32 Battle_IsRowAnimationSettled(ActivePokemon* active_pokemon, s32 frame_limit);
s32 BattleScene_IsMoveKeyedRowWaitComplete(ActivePokemon* active_pokemon, s32 frame_limit);
s32 BattleScene_IsRowAnimWaitComplete(ActivePokemon* active_pokemon, s32 frame_limit);
void BattleScene_TryQueueIdleCueEffect(ActivePokemon* active_pokemon);
void Battle_SetRuntimeFlags(ActivePokemon* active_pokemon, u16 flag);
s32 Battle_ClearRuntimeFlags(ActivePokemon* active_pokemon, u16 flag);
void BattleScene_ResetParticipantAnimationFlags(ActivePokemon* active_pokemon);
void BattleScene_ResetBothParticipantsAnimationState(void);
void Battle_ClearOpponentHopFlagAndSyncMove(ActivePokemon* active_pokemon);
void BattleScene_CleanupParticipantHideFlag(ActivePokemon* active_pokemon);
void BattleScene_CleanupOpponentSpecialAnimFlag(ActivePokemon* active_pokemon);
void BattleScene_CleanupOpponentAnimationFlags(ActivePokemon* active_pokemon);
void BattleScene_CleanupParticipantEffectMode(ActivePokemon* active_pokemon);
void BattleAnim_TickParticipantStatusAnimation(ActivePokemon* active_pokemon);
void BattleAnim_TriggerStatusAfflictionCry(ActivePokemon* active_pokemon);
void BattleScene_AllocateParticipantRecord(BattleSessionTeams* session_teams, s32 side_index);
void BattleScene_AllocateParticipants(BattleSessionTeams* battle_teams);
void BattleScene_InitializeParticipantController(ActivePokemon* active_pokemon);
void BattleScene_InitializeParticipantModel(ActivePokemon* active_pokemon);
void BattleScene_InitializeParticipantOwnerModel(ActivePokemon* active_pokemon);
void BattleScene_InitializeParticipantSecondaryModels(ActivePokemon* active_pokemon, UNUSED s32* secondary_models);
void BattleScene_InitializeParticipantAuxiliaryModel(ActivePokemon* active_pokemon);
void BattleScene_InitializePresentation(UNUSED BattleSessionTeams* session_teams);
void BattleScene_ResetParticipantAuxTransform(UNUSED ActivePokemon* owner, ActivePokemon* active_pokemon,
                                              s32 facing_direction);
void BattleScene_InitializeRuntime(BattleSessionTeams* session_teams);
void BattleScene_UpdateParticipantsAndQueueEffects(UNUSED ActivePokemon* side1_pokemon,
                                                   UNUSED ActivePokemon* side2_pokemon);
s32 BattleScene_DispatchLifecycle(s32 lifecycle_stage, BattleSessionTeams* session_teams);
void func_84307414(ActivePokemon* active_pokemon);
void BattleScene_StateRow56EnterNop(ActivePokemon* active_pokemon);
void BattleScene_StateRow5Slot3Nop(ActivePokemon* active_pokemon);
void BattleScene_StateRow5Slot4Nop(ActivePokemon* active_pokemon);
void BattleScene_StateRow5Slot5Nop(ActivePokemon* active_pokemon);
void BattleScene_StateRow5Slot6Nop(ActivePokemon* active_pokemon);
void BattleScene_StateRow7Slot5Nop(ActivePokemon* active_pokemon);
void BattleScene_StateRow6Slot6Nop(ActivePokemon* active_pokemon);
void BattleScene_StateRow6Slot5Nop(ActivePokemon* active_pokemon);
void BattleScene_StateRow6Slot4Nop(ActivePokemon* active_pokemon);
void BattleScene_StateRow6Slot3Nop(ActivePokemon* active_pokemon);
void func_8430746C(ActivePokemon* active_pokemon);
void func_84307474(UNUSED ActivePokemon* active_pokemon, UNUSED s32 unused);
void func_84307480(UNUSED ActivePokemon* active_pokemon, UNUSED s32 unused);
void BattleScene_StateRow7Slot4Nop(ActivePokemon* active_pokemon);

//@src/fragments/battle_engine/battle_engine_thumbnail.c
void BattleThumbnail_TriggerOwnerModelAnimation(ActivePokemon* active_pokemon);
void BattleThumbnail_ClearParticipantOwnerIdleFlags(ActivePokemon* active_pokemon);
void BattleThumbnail_SetParticipantOwnerIdleFlags(ActivePokemon* active_pokemon);
void BattleThumbnail_ClearBothOwnerIdleFlags(void);
void BattleThumbnail_SetBothOwnerIdleFlags(void);
void BattleThumbnail_SetFlagToUpdatePokemonThumbnail(CameraNode* camera);
void BattleThumbnail_LoadOwnerCameraFramingConstants(ActivePokemon* active_pokemon);
void BattleThumbnail_SetupOwnerCameraFraming(ActivePokemon* side1_pokemon, ActivePokemon* side2_pokemon);
void BattleThumbnail_OwnerCameraLifecycleNop(void);
void BattleThumbnail_ApplyOwnerCameraFraming(ActivePokemon* active_pokemon);
void BattleThumbnail_TickOwnerCameraFraming(ActivePokemon* active_pokemon);
void BattleThumbnail_InitializeOwnerCameraFraming(CameraNode* side1_camera_node, ActivePokemon* side1_pokemon,
                                                  CameraNode* side2_camera_node, ActivePokemon* side2_pokemon);
void BattleThumbnail_TickOwnerSlideIn(ActivePokemon* active_pokemon, s32 delta);
void BattleThumbnail_TickOwnerSlideOut(ActivePokemon* active_pokemon, s32 delta);
void BattleThumbnail_TickOwnerSlideAnimation(ActivePokemon* active_pokemon);
void BattleThumbnail_TickBothOwnerSlideAnimations(ActivePokemon* side1_pokemon, ActivePokemon* side2_pokemon);
s32 BattleThumbnail_DispatchOwnerCameraLifecycle(s32 state, CameraNode* side1_camera_node,
                                                 CameraNode* side2_camera_node);

//@src/fragments/battle_engine/battle_engine_2F23A0.c
s32 BattleScene_ByteArrayContains(u8 lookup, u8* array, s32 array_length);
BattleMon* BattleScene_GetParticipantSessionMon(ActivePokemon* active_pokemon);
s32 BattleScene_IsPartnerPikachu(ActivePokemon* active_pokemon);
f32 BattleScene_GetParticipantModelRestingHeight(ActivePokemon* active_pokemon);
f32 BattleScene_GetParticipantModelWidth(ActivePokemon* active_pokemon);
f32 BattleScene_GetParticipantModelHeight(ActivePokemon* active_pokemon);
f32 BattleScene_GetParticipantCameraTargetOffsetY(ActivePokemon* active_pokemon);
void BattleScene_PositionParticipantModel(ActivePokemon* active_pokemon);
f32 BattleScene_GetParticipantModelXOffset(ActivePokemon* active_pokemon);
void BattleScene_SetParticipantModelScale(ActivePokemon* arg0, f32 arg1);
s32 BattleScene_GetTeamMemberTeamSize(ActivePokemon* active_pokemon);
s32 BattleScene_GetPartnerTeamSize(ActivePokemon* active_pokemon);
s32 BattleScene_GetParticipantSideIndex(ActivePokemon* active_pokemon);
u8 BattleScene_GetMoveCueDefaultAnchor(ActivePokemon* active_pokemon);
u8 BattleScene_GetMoveCueAlternateAnchor(ActivePokemon* active_pokemon);
s16 BattleScene_GetHitCueDefaultAnchor(ActivePokemon* active_pokemon);
s32 BattleScene_GetHitCueAlternateAnchor(ActivePokemon* active_pokemon);
f32 BattleScene_GetParticipantModelBoundsHeight(ActivePokemon* active_pokemon);
f32 BattleScene_GetParticipantUnscaledBoundsHeight(ActivePokemon* active_pokemon);
s16 BattleScene_GetParticipantUsedMove(ActivePokemon* active_pokemon);
void BattleScene_GetParticipantModelBoundsScaled(ActivePokemon* arg0, Vec3f* arg1);
void BattleScene_GetParticipantModelRotation(ActivePokemon* arg0, Vec3s* arg1);
u16 Battle_GetActiveMonHp(ActivePokemon* active_pokemon);
s16 BattleScene_GetParticipantFacingAngle(ActivePokemon* active_pokemon);
s32 BattleAnim_GetSpeciesBySideIndex(s32 arg0);
s32 BattleAnim_GetOwnerSpecies(ActivePokemon* active_pokemon);
u8 BattleScene_IsParticipantModelVisible(ActivePokemon* active_pokemon);
void BattleScene_GetParticipantModelPosition(ActivePokemon* arg0, Vec3f* arg1);
void BattleAnim_GetOwnerDefaultAnchorPosition(ActivePokemon* pokemon, Vec3f* anchor_position);
void BattleScene_GetParticipantRestingPosition(ActivePokemon* arg0, Vec3f* arg1);
void BattleScene_GetParticipantPokeBallPosition(ActivePokemon* arg0, Vec3f* arg1);
f32 BattleAnim_GetOwnerModelHeight(ActivePokemon* active_pokemon);
f32 BattleScene_GetParticipantModelRadiusScaled(ActivePokemon* active_pokemon);
ModelNodeView* Battle_GetLoadedModelNode(ActivePokemon* active_pokemon, s32 node_index);
s32 Battle_GetDisplayModelId(ActivePokemon* active_pokemon, UNUSED s32 unused);
char* Battle_GetActiveMoveName(ActivePokemon* active_pokemon);
u8* Battle_GetActiveMonNickname(ActivePokemon* active_pokemon);
void BattleScene_SetParticipantModelTransform(ActivePokemon* active_pokemon, s32 transform_id, s32 transform_timer);
void Battle_ReturnToIdleAnimIfFinished(ActivePokemon* active_pokemon, ActivePokemon* animated_pokemon);
void Battle_ResetModelToIdleAnim(ActivePokemon* active_pokemon);
void Battle_SetModelToHitAnim(ActivePokemon* active_pokemon);
void BattleScene_BindParticipantTeamRoster(ActivePokemon* active_pokemon, s32 team_member_index);
void BattleScene_SetParticipantTeamMember(ActivePokemon* active_pokemon, s32 team_member_index);
void BattleScene_SetParticipantController(ActivePokemon* active_pokemon, Controller* controller);
void BattleScene_RebindParticipantTeamMember(ActivePokemon* arg0, s32 arg1);
s32 Battle_FreezeClauseInEffect(ActivePokemon* active_pokemon);
s32 Battle_SleepClauseInEffect(ActivePokemon* active_pokemon);
s32 BattleScene_GetCombinedTeamSize(ActivePokemon* active_pokemon);
u8 BattleScene_GetRosterTeamSize(TeamRoster* arg0);
s32 BattleScene_GetParticipantDisplayPlayerNumber(ActivePokemon* active_pokemon);
void BattleScene_ReinitParticipantBodyModel(ActivePokemon* arg0, ActivePokemon* arg1, s32 arg2, s32 arg3);
void BattleScene_ReinitParticipantThumbnailModel(ActivePokemon* arg0, ThumbnailState* arg1, s16 arg2);
void BattleScene_ReinitParticipantAfterimages(ActivePokemon* arg0, s16 arg1);
void Battle_ResetEventTrack(ActivePokemon* active_pokemon);
void Battle_LoadOwnerModelForSpecies(ActivePokemon* active_pokemon);
void Battle_LoadOwnerModelForSwitchIn(ActivePokemon* active_pokemon);
void Battle_ResetOwnerModelToSubstituteDoll(ActivePokemon* active_pokemon);
void Battle_ReloadSubstituteDoll(ActivePokemon* active_pokemon);
void Battle_ReloadDisplayModel(ActivePokemon* active_pokemon);

//@src/fragments/battle_engine/battle_engine_2F3CB0.c
void BattleAnim_InitSwayOffset(ActivePokemon* pokemon, f32 y, s16 theta);
void BattleAnim_UpdateTransform(ActivePokemon* owner, ActivePokemon* animated_pokemon, f32 height, f32 delta_y,
                                s32 delta_theta);
void BattleAnim_UpdateSwayPhysicsToTarget(UNUSED ActivePokemon* owner, ActivePokemon* animated_object, f32 target,
                                          f32 delta, s32 theta0, s32 theta1);
void BattleScene_HideParticipantSecondaryModels(ActivePokemon* active_pokemon);
void BattleScene_ShowSecondaryModelsAtMainTransform(ActivePokemon* pokemon);
void BattleScene_UpdateSecondaryModelOrbit(ActivePokemon* pokemon);
void BattleScene_ShowSecondaryModelsAtMainPose(ActivePokemon* active_pokemon);
s32 BattleScene_ShrinkSecondaryModelsToScale(ActivePokemon* active_pokemon);
void BattleScene_ShowSecondaryModelsStaggeredFrame(ActivePokemon* active_pokemon);
void BattleScene_UpdateSecondaryModelTrailPosition(ActivePokemon* pokemon, s16 new_position);
void BattleScene_ResetParticipantModelFlags(ActivePokemon* active_pokemon);
void BattleAnim_PokeBallThrowStart(UNUSED ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_PokeBallThrowContinue(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_PokeBallThrow(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_PokeBallLandStart(UNUSED ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_PokeBallLandContinue(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_PokeBallLand(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_FlyStart(UNUSED ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_FlyContinue(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_Fly(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_WaterfallStart(UNUSED ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_WaterfallContinue(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_WaterfallEnd(UNUSED ActivePokemon* owner, UNUSED ActivePokemon* animated_pokemon);
void BattleAnim_Waterfall(ActivePokemon* owner, ActivePokemon* animated_pokemon);
s32 BattleAnim_ApproachAngleAndCheckThreshold(ActivePokemon* pokemon, s16 phase, s16 camera_offset_scale);
void BattleAnim_SubmissionStart(UNUSED ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_SubmissionContinue(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_SubmissionEnd(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_Submission(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_DigStart(UNUSED ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_DigContinue(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_DigEnd(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_Dig(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_AgilityStart(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_AgilityContinue(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_AgilityEnd(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_Agility(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_DoubleTeamStart(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_DoubleTeamContinue(UNUSED ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_DoubleTeam(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_ResetOwnerFacingAndExitStatusIdle(ActivePokemon* arg0);
void BattleAnim_SeismicTossStart(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_SeismicTossContinue(UNUSED ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_SeismicToss(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_MinimizeStart(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_MinimizeContinue(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_Minimize(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_WithdrawStart(UNUSED ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_WithdrawContinue(ActivePokemon* owner, UNUSED ActivePokemon* animated_pokemon);
void BattleAnim_Withdraw(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_MeditateStart(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_MeditateContinue(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_MeditateEnd(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_Meditate(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_SquishHitReactionStart(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_SquishHitReactionContinue(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_SquishHitReaction(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_LassoMoveHitReactionStart(UNUSED ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_LassoMoveHitReactionContinue(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_LassoMoveHitReaction(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_SurfStart(UNUSED ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_SurfContinue(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_Surf(ActivePokemon* owner, ActivePokemon* animated_pokemon);
void BattleAnim_TransformNop(UNUSED ActivePokemon* owner, UNUSED ActivePokemon* animated_pokemon);
void BattleAnim_PlayTransformInducingAnimation(s32 side_index);
void BattleAnim_SetOwnerSecondaryStatusFlags(ActivePokemon* arg0, u8 arg1, u8 arg2);
void BattleAnim_ClearOwnerSecondaryStatusFlags(ActivePokemon* arg0);
void BattleAnim_InitSecondarySwayOffset(ActivePokemon* arg0, f32 arg1, s16 arg2);
void BattleAnim_UpdateSecondarySwayPhysics(ActivePokemon* arg0, ActivePokemon* arg1, UNUSED f32 arg2, UNUSED f32 arg3,
                                           UNUSED s32 arg4);
void BattleAnim_SecondaryStatusCaseBSub0(UNUSED ActivePokemon* arg0, ActivePokemon* arg1);
void BattleAnim_SecondaryStatusCaseBSub1(ActivePokemon* arg0, ActivePokemon* arg1);
void BattleAnim_SecondaryStatusCaseB(ActivePokemon* arg0, ActivePokemon* arg1);
void BattleAnim_SecondaryStatusDefault(UNUSED ActivePokemon* arg0, UNUSED ActivePokemon* arg1);
void BattleAnim_UpdateOwnerSecondaryStatusAnimation(s32 arg0);

//@src/fragments/battle_engine/battle_engine_math.c
void BattleAnim_CalculateDistanceAngles(Vec3f* from, Vec3f* to, f32* euclidean_distance, s16* elevation, s16* azimuth);
Vec3f* BattleAnim_Vec3fSetZero(Vec3f* dest);
Vec3f* BattleAnim_Vec3fOffsetByPolarXZ(Vec3f* dest, Vec3f* origin, f32 radius, s16 theta);
void BattleAnim_Vec3fSetPolarXZ(Vec3f* dest, f32 magnitude, s16 theta);
Vec3f* BattleAnim_Vec3fCopy(Vec3f* dest, Vec3f* src);
Vec3s* BattleAnim_Vec3sCopy(Vec3s* dest, Vec3s* src);
Vec3f* BattleAnim_Vec3fScaleComponents(Vec3f* dest, f32 scale_x, f32 scale_y, f32 scale_z);
s16 BattleAnim_Atan2sFromPoints(f32 y0, f32 x0, f32 y1, f32 x1);
f32 BattleAnim_Vec3fDistance(Vec3f a, Vec3f b);
f32 BattleAnim_Distance2D(f32 x0, f32 y0, f32 x1, f32 y1);
s32 BattleAnim_RandomIntWithWarmup(f32 count);
u16 BattleAnim_RandomRange(s32 range);
s32 BattleAnim_StepToS16(s16* dest, s16 finish, s16 rate);
s32 BattleAnim_StepToS16VariableRate(s16* dest, s16 finish, f32 rate);
void BattleAnim_ApproachF(f32* dest, f32 value, f32 speed);
void BattleAnim_ApproachS16(s16* dest, s16 value, s16 speed);
void BattleAnim_ApproachU16(u16* dest, u16 value, s16 speed);
void BattleAnim_BSplineWeightsUniform(f32* dest, f32 t);
void BattleAnim_BSplineWeightsClampedFirst(f32* dest, f32 t);
void BattleAnim_BSplineWeightsClampedSecond(f32* dest, f32 t);
void BattleAnim_BSplineWeightsClampedPenultimate(f32* dest, f32 t);
void BattleAnim_BSplineWeightsClampedLast(f32* dest, f32 t);

//@src/fragments/battle_engine/battle_engine_action_selection.c
void BattleScene_InitializeBattleTimers(void);
void BattleScene_QuantizeBattleTimer(void);
s32 BattleScene_TickBattleTimer(void);
s32 BattleScene_IsBattleCountdownComplete(void);
s32 Battle_ClassifyHpTier(s32 current_hp, s32 max_hp);
s32 BattleScene_DetermineWinner(void);
s32 BattleScene_UpdateResultPresentation(void);
s32 BattleScene_IsActionSelectionTimerExpired(void);
void BattleScene_ResetActionSelectionTimerHelper(void);
s32 BattleScene_TickActionSelectionTimers(void);
void BattleScene_ResetActionSelectionTimer(void);
s32 BattleScene_ApplyForcedActionDelay(ActivePokemon* arg0);
void BattleScene_SyncActiveHpToPartyState(void);
s32 BattleScene_SelectFallbackWhenNoUsableMoves(ActivePokemon* pokemon);
s32 BattleScene_IsMoveUnavailable(ActivePokemon* pokemon, s32 move);
void BattleScene_AutoSelectAvailableMove(ActivePokemon* pokemon);
void BattleScene_AutoSelectReplacementMon(ActivePokemon* pokemon);
void BattleScene_SelectActiveController(ActivePokemon* pokemon);
void BattleScene_HandleActionSelectionCancel(s32 side);
void Battle_InitializeActiveRuntimeState(void);
s32 BattleScene_CpuHasNoUsableMoves(ActivePokemon* pokemon);
s32 BattleScene_WaitForCpuChoice(ActivePokemon* pokemon1, ActivePokemon* pokemon2);
void BattleScene_CompleteCpuForcedAction(ActivePokemon* pokemon, UNUSED ActivePokemon* arg1);
void BattleScene_CompleteCpuMoveAction(ActivePokemon* pokemon1, ActivePokemon* pokemon2);
void BattleScene_ApplyCpuChoice(ActivePokemon* pokemon1, ActivePokemon* pokemon2);
s32 BattleScene_ApplyCpuSwitchChoice(ActivePokemon* pokemon1, ActivePokemon* pokemon2);
s32 BattleScene_CollectCpuAction(ActivePokemon* pokemon1, ActivePokemon* pokemon2);
void BattleScene_ForfeitPrompt(ActivePokemon* pokemon);
void BattleScene_CommitPlayerSwitchAction(ActivePokemon* arg0);
s32 BattleScene_ValidateSelectedPartyMember(ActivePokemon* arg0);
void BattleScene_QueuePartySelectionConfirmMessage(ActivePokemon* arg0);
void BattleScene_AssignActiveCoopController(ActivePokemon* arg0, s32 arg1);
void BattleScene_HandlePartySlotCursorInput(ActivePokemon* pokemon, Controller* controller);
void BattleScene_HandlePlayerPartySelection(ActivePokemon* pokemon);
void BattleScene_CommitSelectedMoveAction(ActivePokemon* pokemon);
s32 BattleScene_HandleNoUsableMoveSelection(ActivePokemon* pokemon);
s32 BattleScene_ValidateSelectedMove(ActivePokemon* pokemon);
void BattleScene_SelectMoveFromButtons(ActivePokemon* pokemon);
void BattleScene_HandlePlayerMoveSelection(ActivePokemon* pokemon);
s32 BattleScene_HandlePlayerMoveSelectionLocked(ActivePokemon* pokemon, UNUSED Controller* arg1);
void BattleScene_ConfirmForcedMoveRepeat(ActivePokemon* arg0);
void BattleScene_PollActionMenuRequestButtons(ActivePokemon* pokemon);
s32 BattleScene_HandlePlayerActionInput(ActivePokemon* pokemon);
s32 BattleScene_HandlePlayerForcedActionState(ActivePokemon* pokemon1, ActivePokemon* pokemon2);
void BattleScene_ShowParticipantOwnerAndSecondaryModels(void);
void BattleScene_HideParticipantOwnerAndSecondaryModels(void);
void BattleScene_AdvanceAttractInputScript(void);
void BattleScene_CheckForcedActionCondition(ActivePokemon* pokemon1, ActivePokemon* pokemon2);
void BattleScene_TickAttractModeInput(void);
void BattleScene_CollectPlayerAction(ActivePokemon* pokemon1, ActivePokemon* pokemon2);
void BattleScene_CollectActions(void);
void BattleScene_UpdateActionCollection(void);
void BattleScene_UpdateActionSelectPhase(void);

//@src/fragments/battle_engine/battle_engine_hud.c
void func_8430FC10(void);
void BattleHud_DrawTranslucentShadowRect(s32 x, s32 y, s32 width, s32 height, s32 alpha);
void BattleHud_DrawGradientRect(s32 x, s32 y, s32 width, s32 height, Color_RGB8* start_color, Color_RGB8* end_color);
void Battle_DrawGradientRectWrapper(s32 x, s32 y, s32 width, s32 height, Color_RGB8* start_color,
                                    Color_RGB8* end_color);
void func_84310360(void);
void BattleHud_DrawHpBar(s32 x, s32 y, f32 current_hp, s32 max_hp);
void BattleHud_Noop(void);
void BattleHud_DrawLevelNumber(s32 x, s32 y, s32 level);
void BattleHud_DrawHpFractionText(s32 x, s32 y, s32 current_hp, s32 max_hp);
void BattleHud_DrawTrainerNameLabel(s16 x, s16 y, s32 port, char* name, s32 has_illegal_move);
void BattleHud_DrawTrainerNameLabelBlink(s16 x, s16 y, s32 port, u8* name, s32 blink_phase, s32 has_illegal_move);
void BattleHud_DrawTrainerNameLabelAuto(ActivePokemon* pokemon, TeamRoster* pokemon_team, s16 x, s16 y, s32 port,
                                        u16 blink_phase, s32 selected_move);
void BattleHud_DrawMonInfoPanel(ActivePokemon* active_pokemon, BattleMon* pokemon_info,
                                StatusConditionLabel* status_label, s32 x, s32 y, s32 width, s32 length, s32 r, s32 g,
                                s32 b, s32 alpha);
s32 BattleHud_DrawBorderTile(s32 x, s32 y, s32 width, s32 height, u8* texture);
void func_84311420(void);
void BattleHud_DrawBoxBorder(s16 x, s16 y, s16 width, s16 height);
void BattleHud_DrawParticipantTimer(s16 x, s16 y);
void BattleHud_DrawBattleTimer(s16 x, s16 y);
void BattleHud_DrawPartyBallRow(ActivePokemon* pokemon, s16 x, s16 y);
void BattleHud_DrawMonStatusBoxPlayer(ActivePokemon* active_pokemon, s16 x, s16 y, u16 blink_phase, s32 selected_move);
void BattleHud_DrawMonStatusBoxOpponent(ActivePokemon* active_pokemon, s16 x, s16 y, u16 blink_phase,
                                        s32 selected_move);
void BattleHud_DrawThreeOptionLabelBar(ActivePokemon* pokemon, s16 x, s16 y, s32 port);
void BattleHud_DrawTwoOptionLabelBar(ActivePokemon* pokemon, s16 x, s16 y, s32 port);
void BattleHud_DrawOneOptionLabelBar(ActivePokemon* pokemon, s16 x, s16 y, s32 port);
void Battle_DrawRightAlignedText(s32 x, s32 y, char* string);
s32 BattleHud_DrawMovePpTextLTR(s32 x, s32 y, s32 current_pp, s32 max_pp);
s32 BattleHud_DrawMovePpTextRTL(s32 x, s32 y, s32 current_pp, s32 max_pp);
void BattleHud_DrawMoveSelectMenu(ActivePokemon* active_pokemon, s16 x, s16 y, s32 port);
void BattleHud_DrawForfeitConfirmDialog(s16 x, s16 y, s16 port, s32 arg3);
void BattleHud_DrawPlainText(s16 x, s16 y, char* fmt);
void BattleHud_DrawDoublesRosterRow(ActivePokemon* active_pokemon, TeamRoster* roster, BattleMon* party_pokemon, s32 x,
                                    s32 y, s32 party_slot);
void BattleHud_DrawPartyRosterRow(ActivePokemon* active_pokemon, TeamRoster* roster, BattleMon* party_pokemon, s32 x,
                                  s32 y, s32 party_slot);
void BattleHud_DrawTeamStatusList(ActivePokemon* pokemon, TeamRoster* team_roster, s16 x, s16 y, s32 port);
void BattleHud_DrawDoublesTeamStatusList(ActivePokemon* pokemon, s16 x, s16 y, s32 port);
void BattleHud_DrawCompactMonStatusBox(ActivePokemon* active_pokemon, s16 x, s16 y, s32 port);
void BattleHud_DrawMessageBox(s32 port, s16 x, s16 y, char* text);
s32 Battle_DrawTextIfNonEmpty(char* text, s16 x, s16 y);
void BattleHud_DrawExpandingCaptionBox(BattleBox* battle_box, s16 center_x, s16 center_y, Vec3f corner);
void BattleHud_InitCaptionBoxPopup(BattleBox* battle_box, s16 center_x, s16 center_y);
s32 BattleHud_UpdateCaptionBoxPopup(BattleBox* battle_box, s16 center_x, s16 center_y);
void BattleHud_DrawExpandingCaptionBoxBlack(BattleBox* battle_box, s16 center_x, s16 center_y, Vec3f corner);
void BattleHud_InitCaptionBoxPopupBlack(BattleBox* battle_box, s16 center_x, s16 center_y);
s32 BattleHud_UpdateCaptionBoxPopupBlack(BattleBox* battle_box, s16 center_x, s16 center_y);
void BattleHud_DrawTextBox(s32 port, s16 x, s16 y, char* text);
void BattleHud_DrawTwoOptionCursorMenu(ActivePokemon* pokemon, s16 x, s16 y, s32 port);
void BattleScene_DrawStripWipeIfActive(void);
void Battle_DrawScrollingTextureRect(s16 x, s16 y, s16 width, s16 height, s16 texture_x, s16 texture_y,
                                     s16 texture_spread_x, s16 texture_spread_y);
void Battle_DrawTiledStripSegment(s32 x, s32 y, s32 width, s32 height, u8* texture, s32 load_width);
void Battle_DrawDemoStadiumLogo(void);
void Battle_DrawEndBattleBanner(void);
void Battle_DrawDemoControllerWarning(void);
void BattleHud_DrawOwnSideStatus(ActivePokemon* active_pokemon, ActivePokemonState* state);
void BattleHud_DrawOpponentSideStatus(ActivePokemon* active_pokemon, ActivePokemonState* state);

//@src/fragments/battle_engine/battle_engine_messages.c
void Battle_ClearAllMessageQueues(void);
void Battle_ClearMessageCursor(void);
void Battle_SimpleStringFormat(s8* buffer, s8* format, ...);
#if defined(VERSION_JP)
// The Japanese build formats the message: %s in fmt copies the next char* argument.
void Battle_QueueMessage(s8* arg0, s8 arg1, const char* fmt, ...);
#else
void Battle_QueueMessage(s8* text, s8 file_number_index);
#endif
void Battle_QueueStatusText(char* text, s32 side);
void func_84317B38(void);
void Battle_DrawCompactStatusBoxesIfVisible(void);
void Battle_QueueMoveCategoryUsedMessage(void);
void Battle_QueueMoveCategoryMessage(void);
void Battle_QueueMetronomeMoveMessage(void);
void BattleBox_Open(s32 opening_progress);
void BattleBox_Close(void);
void BattleBox_PlayTextBlip(s8* message);
void BattleBox_DrawMessage(BattleBox* box, s16 x, s16 y);
s32 BattleBox_SelectMessage(BattleBox* box);
void BattleBox_Update(s16 x, s16 y);
void BattleScene_ClearParticipantMessageBuffer(ActivePokemon* active_pokemon);
void BattleScene_BuildParticipantMessage(ActivePokemon* pokemon, s32 file_number);
void BattleScene_RevealParticipantMessageChars(ActivePokemon* pokemon);
void BattleScene_TickHudTimers(void);
void Battle_InitMessageQueues(void);
void BattleScene_ResetMessageQueue(void);
void Battle_RenderHud(void);

//@src/fragments/battle_engine/battle_engine_broll.c
s32 BattleScene_AreBothParticipantModelsCompact(void);
s32 BattleScene_SkipUnavailableBRollShot(UNUSED ActivePokemon* pokemon, CameraNode* camera);
s32 BattleScene_IsEitherActivePokemonUnderground(UNUSED ActivePokemon* pokemon, UNUSED CameraNode* camera);
void BattleScene_BRollCameraSetupArenaOrbit(CameraNode* camera);
void BattleScene_BRollCameraSetupRotatingArenaOverview(CameraNode* camera);
void BattleScene_BRollCameraSetupStaticZoomedOutShot(CameraNode* camera, s32 side);
void BattleScene_BRollCameraSetupZoomShot(CameraNode* camera, s32 side);
void BattleScene_BRollCameraSetupSendOutShot(CameraNode* camera, s32 side);
void BattleScene_BRollCameraSetupHeightShotFirstSide(CameraNode* camera);
void BattleScene_BRollCameraSetupHeightShotSecondSide(CameraNode* camera);
void BattleScene_BRollCameraSetupStaticArenaOverview(CameraNode* camera);
void BattleScene_BRollCameraSetupStaticCloseUp(CameraNode* camera, s32 side);
void BattleScene_SelectNextBRollCameraShot(CameraNode* camera);
void BattleScene_BRollCameraUpdateArenaOrbit(CameraNode* camera);
void BattleScene_BRollCameraUpdateRotatingArenaOverview(CameraNode* camera);
void BattleScene_BRollCameraUpdateStaticZoomedOutShot(CameraNode* camera);
void BattleScene_BRollCameraUpdateZoomShot(CameraNode* camera);
void BattleScene_BRollCameraUpdateSendOutShot(CameraNode* camera);
void BattleScene_BRollCameraUpdateHeightShotFirstSide(CameraNode* camera);
void BattleScene_BRollCameraUpdateHeightShotSecondSide(CameraNode* camera);
void BattleScene_BRollCameraUpdateStaticArenaOverview(CameraNode* camera);
void BattleScene_BRollCameraUpdateStaticCloseUp(CameraNode* camera);
void BattleScene_UpdateBRollCameraSequence(CameraNode* camera);

//@src/fragments/battle_engine/battle_engine_camera.c
f32 BattleScene_ScaleCameraDistance(ActivePokemon* pokemon, f32 scale, f32 max_allowed);
s32 BattleScene_AdvanceCameraSubstepCounter(s32 frame_limit);
s32 BattleScene_AdvanceCameraFrameCounter(s32 frame_limit);
s32 BattleScene_AdvanceCameraFrameCounterUntil(s32 frame_limit);
s32 BattleScene_IsCameraFrameCounterAtLeast(s32 frame_limit);
void BattleScene_SetActingSide(s32 side_index);
void BattleScene_ResetCameraFrameCounter(void);
void BattleScene_ResetCameraSubstepCounter(void);
void BattleScene_SetCameraAtAndEyeFromAngles(CameraNode* camera, f32 x, f32 y, f32 z, f32 distance, s16 rotation_x,
                                             s16 rotation_y);
void BattleScene_GetOwnerCameraAnchor(ActivePokemon* pokemon, Vec3f* dest);
void BattleScene_GetOwnerBoneOrDefaultAnchor(ActivePokemon* pokemon, Vec3f* dest, s32 lookup_id);
void BattleScene_SetCameraModeAndStep(s32 camera_mode, s32 camera_step);
void BattleScene_SetCameraClipPlanes(CameraNode* camera_node, f32 near_plane, f32 far_plane);
void BattleScene_SetCameraSubstate(s32 transition_step, u16 transition_style);
void BattleScene_ApplyCameraPhaseDefaults(void);
void BattleScene_SetDamageShakeAmplitude(ActivePokemon* pokemon);
void BattleScene_ApplyCameraShake(CameraNode* camera);
void BattleScene_StartCameraShake(UNUSED ActivePokemon* active_pokemon);
s32 BattleScene_IsAngleWithinFixedStep(s16 starting_angle, s16 target_angle);
s32 BattleScene_ApproachCameraRotationY(CameraNode* camera, s16 delta_theta, s16 final_angle_y);
s32 BattleScene_ApproachCameraRotationYNoEyeUpdate(UNUSED CameraNode* camera, s16 delta_theta, s16 final_angle_y);
void BattleScene_SetCameraAnglesAndDistance(s16 rotation_x, s16 rotation_y, f32 distance);
void BattleScene_SetCameraSendOutShot(CameraNode* camera, s32 opposite_side);
void BattleScene_ClampCameraEyeFloor(CameraNode* camera);
void BattleScene_UpdateCameraModelOffset(ActivePokemon* pokemon, CameraNode* camera);
void BattleScene_ClearCameraOffsetVectors(void);
void BattleScene_SetCameraAtFromAnchor(ActivePokemon* pokemon, CameraNode* camera, Vec3f starting_position,
                                       s16 rotation_x, s16 rotation_y, f32 distance, f32 y_offset);
void BattleScene_SetCameraAtFromPosition(CameraNode* camera, Vec3f starting_position, s16 rotation_x, s16 rotation_y,
                                         f32 distance, f32 y_offset);
void BattleScene_SetCameraEyeFromPosition(CameraNode* camera, Vec3f starting_position, s16 rotation_x, s16 rotation_y,
                                          f32 distance, f32 y_offset);
void BattleScene_SetCameraEyeFromAt(CameraNode* camera, s16 rotation_x, s16 rotation_y, f32 distance, f32 y_offset);
void BattleScene_SetCameraEyeAndStoreAngles(CameraNode* camera, Vec3f starting_postion, s16 rotation_x, s16 rotation_y,
                                            f32 distance, f32 y_offset);
void BattleScene_SetCameraEyeAndStoreAnglesFromAt(CameraNode* camera, s16 rotation_x, s16 rotation_y, f32 distance,
                                                  f32 y_offset);
void BattleScene_CameraHelperNop(UNUSED CameraNode* camera);
void BattleScene_SnapFloatPair(f32* a, f32* b);
void BattleScene_ApplyOwnerFramingAdjustments(CameraNode* camera, ActivePokemon* pokemon, UNUSED f32 arg2);
void BattleScene_RecomputeCameraDistance(CameraNode* camera, ActivePokemon* pokemon);
void BattleScene_ReframeAndRecomputeDistance(CameraNode* camera, ActivePokemon* pokemon);
void BattleScene_ReframeAndRecomputeDistanceForSwitch(CameraNode* camera, ActivePokemon* pokemon);
void BattleScene_ApplyOwnerFramingOnly(CameraNode* camera, ActivePokemon* pokemon);
void BattleScene_UpdateCameraTargetFromOwner(CameraNode* camera, ActivePokemon* pokemon);
s32 BattleScene_FrameCamera(CameraNode* camera, ActivePokemon* pokemon);
void BattleScene_ApproachCameraTargetFromOwner(CameraNode* camera, ActivePokemon* pokemon);
void BattleScene_UpdateCameraVerticalFraming(CameraNode* camera, ActivePokemon* pokemon);
void BattleScene_RecenterCameraOnOwner(CameraNode* camera, ActivePokemon* pokemon);
void BattleScene_ApproachCameraTarget(CameraNode* camera, ActivePokemon* pokemon, UNUSED s32 arg2);
void BattleScene_ApproachCameraZoom(CameraNode* camera, ActivePokemon* pokemon);
s32 BattleScene_ApproachCloseCameraShot(CameraNode* camera, ActivePokemon* pokemon);
s32 BattleScene_AnimateCameraPreset(CameraNode* camera, ActivePokemon* pokemon, s32 adjust_y_offset,
                                    s32 adjust_vertical_fov);
s32 BattleScene_AnimateCameraPresetToOwner(CameraNode* camera, ActivePokemon* pokemon);
void BattleScene_CameraPresetNop(UNUSED CameraNode* camera);
void BattleScene_ApplyCameraPreset(CameraNode* camera, ActivePokemon* pokemon, s32 update_offset);
void BattleScene_SetCameraPresetAtAngles(s16 starting_rotation_x, s16 starting_rotation_y, s16 rotation_x_offset,
                                         s16 rotation_y_offset);
void BattleScene_SetCameraPresetAtDistances(f32 starting_distance, f32 distance_offset);
void BattleScene_SetCameraPresetEyeAngles(s16 eye_rotation_x, s16 eye_rotation_y, s16 rotation_x, s16 rotation_y);
void BattleScene_SetCameraPresetEyeDistances(f32 eye_distance, f32 distance);
void BattleScene_SetCameraPresetSmoothing(f32 target_z, f32 speed);
void BattleScene_LoadCameraPreset(DioramaAnimData* diorama, CameraNode* camera, ActivePokemon* pokemon);
void BattleScene_LoadCameraPresetUnscaled(DioramaAnimData* diorama, CameraNode* camera);
s32 BattleScene_AnimateCameraIntro(CameraNode* camera, ActivePokemon* pokemon);
s32 BattleScene_AreActivePokemonHeightsSeparated(UNUSED CameraNode* camera);
void BattleScene_SetCameraActivePokemonHeightShot(CameraNode* camera, s32 side);
s32 BattleScene_ApproachCameraActivePokemonHeightShot(CameraNode* camera, s32 side);
void BattleScene_SetCameraOwnerShot(CameraNode* camera, ActivePokemon* pokemon);
void BattleScene_SetCameraOwnerWideShot(CameraNode* camera, ActivePokemon* pokemon);
void BattleScene_SetCameraPresetClose(CameraNode* camera, ActivePokemon* pokemon, s32 update_offset);
void BattleScene_SetCameraPresetAngledClose(CameraNode* camera, ActivePokemon* pokemon, s32 update_offset);
void BattleScene_SetCameraPresetOffsetClose(CameraNode* camera, ActivePokemon* pokemon, s32 update_offset);
void BattleScene_SetCameraPresetLowClose(CameraNode* camera, ActivePokemon* pokemon, s32 update_offset);
void BattleScene_SetCameraPresetWide(CameraNode* camera, ActivePokemon* pokemon, s32 update_offset);
void BattleScene_SetCameraPresetWideStatic(CameraNode* camera, ActivePokemon* pokemon, s32 update_offset);
void BattleScene_SetCameraPresetFromTable(CameraNode* camera, ActivePokemon* pokemon, s32 diorama_data_index,
                                          s32 update_offset);
void BattleScene_CameraShotTargetStandard(CameraNode* camera, ActivePokemon* pokemon);
void BattleScene_CameraShotTargetAngled(CameraNode* camera, ActivePokemon* pokemon);
void BattleScene_CameraShotTargetHigh(CameraNode* camera, ActivePokemon* pokemon);
void BattleScene_CameraShotTargetLow(CameraNode* camera, ActivePokemon* pokemon);
void BattleScene_CameraShotTargetClose(CameraNode* camera, ActivePokemon* pokemon);
void BattleScene_SetCameraFollowPresetNear(CameraNode* camera, ActivePokemon* pokemon, s32 update_offset);
void BattleScene_SetCameraOwnerStandardShot(CameraNode* camera, ActivePokemon* pokemon);
void BattleScene_SelectCameraPreset(ActivePokemon* pokemon, CameraNode* camera, s32 shot_type, s32 update_offset);
void BattleScene_SelectCameraFollowPreset(ActivePokemon* pokemon, CameraNode* camera, s32 shot_type);
void BattleScene_SelectCameraShot(ActivePokemon* pokemon, CameraNode* camera, s32 shot_type);

//@src/fragments/battle_engine/battle_engine_battle_phases.c
void BattleScene_RestoreUndergroundDiglett(ActivePokemon* pokemon);
void BattleScene_ResetModelReloadStep(void);
s32 BattleScene_RevealSubstituteUser(ActivePokemon* pokemon);
s32 BattleScene_RestoreSubstituteDoll(ActivePokemon* pokemon);
s32 BattleScene_BreakSubstitute(ActivePokemon* pokemon);
s32 BattleScene_RevealSubstituteForAction(ActivePokemon* pokemon);
s32 BattleScene_RestoreBothSubstitutes(void);
void Battle_ClearExpiredTrappingFlags(void);
void BattleScene_NoOp8431FCC4(void);
s32 BattleScene_AnimateDisplayedHpToRuntime(ActivePokemon* active_pokemon);
void BattleScene_ResetPhaseSubstep(void);
void BattleScene_BeginPhaseSubstep(void);
void BattleScene_SetPhase(UNUSED CameraNode* camera, s32 phase);
void BattleScene_SaveCameraFov(CameraNode* camera);
void BattleScene_ResetCameraFov(CameraNode* camera);
s32 BattleScene_GetInitialCameraCue(ActivePokemon* pokemon);
s32 BattleScene_GetChargeInitialCameraCue(ActivePokemon* pokemon);
s32 BattleScene_GetFollowupCameraCue(ActivePokemon* pokemon);
s32 BattleScene_GetChargeFollowupCameraCue(ActivePokemon* pokemon);
s32 BattleScene_GetCameraShotDuration(ActivePokemon* pokemon);
void Battle_StartTurn(void);
void BattleScene_InitializeSwitchActions(void);
s32 BattleScene_PrepareReplacementTransition(void);
void BattleScene_BeginBattlePhase(void);
s32 BattleScene_TransitionAndClearMoveEffect(s32 phase);
s32 BattleScene_TransitionPreservingMoveEffect(s32 phase);
s32 BattleScene_TransitionToRage(s32 phase);
void BattleScene_SwitchActiveSideAndSetPhase(CameraNode* camera);
void Battle_ActSecondAttacker(u8 next_phase);
void BattleScene_PhaseEnterQueuedPhase(void);
void BattleScene_CleanupActionAndAdvance(void);
s32 BattleScene_HandleFaintStatusCompletion(void);
void BattleScene_CompleteActionPhase(void);
void BattleScene_ResolveDisplayedHpCompletion(CameraNode* camera);
void BattleScene_UpdateDisplayedHpAndTransition(CameraNode* camera);
void BattleScene_WaitForDisplayedHpAnimations(CameraNode* camera);
s32 BattleScene_ActionPresentationTransitionCheck(void);
s32 BattleScene_ActionPresentationCleanupCheck(void);
s32 BattleScene_PrepareResidualStatusPresentation(void);
void BattleScene_UpdateActiveOwnerPointers(void);
void BattleScene_InitializeBattlePresentation(CameraNode* camera);
void BattleScene_PrepareWinnerPresentation(CameraNode* camera);
s32 BattleScene_ResetOwnerStatusPresentation(ActivePokemon* pokemon);
void BattleScene_BeginFaintPresentation(CameraNode* camera);
void BattleScene_UpdateFaintAnimation(CameraNode* camera);
void BattleScene_AdvanceFaintCameraClose(CameraNode* camera);
void BattleScene_FinishFaintCameraClose(CameraNode* camera);
void BattleScene_CompleteFaintPresentation(CameraNode* camera);
void BattleScene_FaintStepNoOp(UNUSED CameraNode* camera);
void BattleScene_SetupWinnerCamera(CameraNode* camera);
void BattleScene_CompleteWinnerCameraFade(UNUSED CameraNode* camera);
void BattleScene_CompleteWinnerExtendedFade(UNUSED CameraNode* camera);
void BattleScene_UpdateAlternateFaintAnimation(CameraNode* camera);
void BattleScene_PhaseFaint(CameraNode* camera);
void BattleScene_CompleteResidualFaintPresentation(UNUSED CameraNode* camera);
void BattleScene_PhaseFaintAfterResidualDamage(CameraNode* camera);
void BattleScene_ResetOwnerPresentationFlags(ActivePokemon* pokemon);
void BattleScene_InitializeIntroCamera(CameraNode* camera);
void BattleScene_AdvanceIntroCamera(CameraNode* camera);
void BattleScene_PhaseTurnIntroCamera(CameraNode* camera);
void BattleScene_SetMonMoveResourceFlag(ActivePokemon* pokemon);
void BattleScene_PrepareMoveResourcePresentation(UNUSED CameraNode* camera);
void BattleScene_SelectActionCameraPreset(CameraNode* camera);
s32 BattleScene_AdvanceActionPresentationSetup(CameraNode* camera);
void BattleScene_ActionSetupCameraStep(CameraNode* camera);
void BattleScene_RefreshActionCameraPreset(CameraNode* camera);
void BattleScene_SelectActionCameraPresetFromTiming(CameraNode* camera, s32 use_charge_cue);
s32 BattleScene_ApplyActionCameraMode(CameraNode* camera, s32 allow_reframe);
void BattleScene_CleanupActionPresentation(UNUSED CameraNode* camera);
s32 BattleScene_WaitForActionCameraDuration(UNUSED CameraNode* camera);
void BattleScene_ActionSetupAnimationDefault(CameraNode* camera);
void BattleScene_AnimateMultiHitAction(CameraNode* camera);
void BattleScene_AnimateConfusionAction(CameraNode* camera);
void BattleScene_AnimateDigReturnAction(CameraNode* camera);
void BattleScene_AnimateAirborneAction(CameraNode* camera);
void BattleScene_ActionSetupAnimationStep(CameraNode* camera);
void BattleScene_ResetSeismicTossFacing(ActivePokemon* pokemon);
s32 BattleScene_IsFireSpin(ActivePokemon* pokemon);
void BattleScene_CompleteHpUpdateTransition(CameraNode* camera, s32* side_hp_is_consistent,
                                            s32* other_side_hp_is_consistent);
void BattleScene_PresentTargetStatus(CameraNode* camera);
void BattleScene_PresentTargetStatDrop(CameraNode* camera);
void BattleScene_PresentDisable(CameraNode* camera);
void BattleScene_RunActionEffectHpTransition(CameraNode* camera);
void BattleScene_NoOp84324404(void);
void BattleScene_NoOp8432440C(void);
void BattleScene_RunRepeatedHitHpTransition(CameraNode* camera);
void BattleScene_ActionSetupFinalizeStep(CameraNode* camera);
void BattleScene_PhaseFirstAttackerAction(CameraNode* camera);
void BattleScene_PhaseSecondAttackerAction(CameraNode* camera);
s32 BattleScene_PhaseDispatchMoveEffect(void);
void BattleScene_PhaseSelfTargetMove(CameraNode* camera);
void BattleScene_PhaseWithdrawMove(CameraNode* camera);
void BattleScene_PhaseBide(CameraNode* camera);
void BattleScene_PhaseExplosion(CameraNode* camera);
void BattleScene_PhaseNoAction(CameraNode* camera);
void BattleScene_PhaseTransform(CameraNode* camera);
void BattleScene_PhaseSubstitute(CameraNode* camera);
s32 BattleScene_EndStatusAnimationsAndCommit(ActivePokemon* pokemon);
void BattleScene_PhaseHaze(CameraNode* camera);
void BattleScene_PhaseFly(CameraNode* camera);
void BattleScene_PhaseDig(CameraNode* camera);
void BattleScene_PhaseTwoTurnCharge(CameraNode* camera);
void BattleScene_PhaseSleepOrImmobilized(CameraNode* camera);
void BattleScene_PhaseConfusionSelfHit(CameraNode* camera);
void BattleScene_PhaseFullParalysis(CameraNode* camera);
void BattleScene_NoOp84326A70(void);
void BattleScene_PhaseEndTurn(UNUSED CameraNode* camera);
void BattleScene_PhaseJumpKickCrash(CameraNode* camera);
void BattleScene_PhaseMoveFailedMessage(CameraNode* camera);
void BattleScene_PhaseResidualDamageAfterMove(CameraNode* camera);
void BattleScene_PhaseRecoilDamage(CameraNode* camera);
void BattleScene_PhaseRage(CameraNode* camera);
void BattleScene_PhaseResidualDamageWithoutMove(CameraNode* camera);
void BattleScene_PhaseResidualDamageFaintCheck(CameraNode* camera);
void BattleScene_ClearMonStatusAnimations(ActivePokemon* pokemon);
void BattleScene_PhaseVoluntarySwitch(CameraNode* camera);
void BattleScene_PhaseSendOutAfterFaint(CameraNode* camera);
void BattleScene_UpdatePhase(CameraNode* camera);

//@src/fragments/battle_engine/battle_engine_intro.c
void BattleScene_IntroSetupCamera(CameraNode* camera);
void BattleScene_IntroSendOutFirstMon(UNUSED CameraNode* camera);
void BattleScene_IntroSendOutSecondMon(UNUSED CameraNode* camera);
void BattleScene_IntroNarrowFov(CameraNode* camera);
void BattleScene_IntroPanCamera(CameraNode* camera);
void BattleScene_IntroWaitFirstMonAnim(UNUSED CameraNode* camera);
void BattleScene_IntroRevealFirstMon(CameraNode* camera);
void BattleScene_IntroFirstMonSettle(CameraNode* camera);
void BattleScene_IntroRevealSecondMon(CameraNode* camera);
void BattleScene_IntroSecondMonSettle(CameraNode* camera);
void BattleScene_IntroStepNop(UNUSED CameraNode* camera);
void BattleScene_IntroInitNop(void);
void BattleScene_UpdateIntroSequence(CameraNode* camera);

//@src/fragments/battle_engine/battle_engine_scene.c
void BattleScene_FrameDefaultSide(CameraNode* camera);
void BattleScene_FrameSideZero(CameraNode* camera);
void BattleScene_InitializeMode(CameraNode* camera, UNUSED ActivePokemon* first_pokemon,
                                UNUSED ActivePokemon* second_pokemon);
void BattleScene_AllocateMemory(void);
void BattleScene_UpdateCameraDefaultModeNoop(UNUSED CameraNode* camera);
void BattleScene_UpdateCameraOwner0FramingModeNoop(UNUSED CameraNode* camera);
void BattleScene_AvoidDegenerateCameraLookAt(CameraNode* camera);
void BattleScene_TickCameraSubstateDefault(CameraNode* camera);
void BattleScene_TickCameraSubstateStripWipe(CameraNode* camera);
void BattleScene_TickCameraSubstate(CameraNode* camera);
u16 BattleScene_IsOwnerInViewWedge(ActivePokemon* owner, CameraNode* camera, ActivePokemon* tested_pokemon,
                                   s16 half_angle, f32 wedge_length);
u16 BattleScene_IsOwnerInViewWedgeAlt(ActivePokemon* owner, CameraNode* camera, ActivePokemon* tested_pokemon,
                                      s16 half_angle, f32 wedge_length);
void BattleScene_UpdateBothOwnersVisibilityCulling(CameraNode* camera);
void BattleScene_UpdateOpponentVisibilityCulling(CameraNode* camera);
void func_8432A414(void);
void BattleScene_ShowBothOwners(void);
void BattleScene_HideOpponentOwner(void);
void BattleScene_HideActiveOwner(void);
void BattleScene_HideAbsentParticipants(void);
void BattleScene_UpdateVisibilityBeforeMode(CameraNode* camera);
void BattleScene_UpdateVisibilityAfterMode(CameraNode* camera);
void BattleScene_Update(CameraNode* camera);
s32 BattleScene_Dispatch(s32 operation, CameraNode* camera);

//@src/fragments/battle_engine/battle_engine_status_animation.c
void BattleAnim_StartFreezeStatusAnimation(ActivePokemon* active_pokemon);
void BattleAnim_RestoreBaseModelAnimation(ActivePokemon* active_pokemon);
void BattleAnim_ClearFreezeStatusAnimation(ActivePokemon* active_pokemon);
void BattleAnim_ResetFreezeStatusAndRestoreAnimation(ActivePokemon* active_pokemon);
void BattleAnim_ClearFreezeStatusAnimationIfThawed(ActivePokemon* active_pokemon);
void BattleAnim_UpdateSleepStatusAnimation(ActivePokemon* active_pokemon);
void BattleAnim_EndSleepStatusAnimation(ActivePokemon* active_pokemon);
void BattleAnim_ClearSleepStatusAnimation(ActivePokemon* active_pokemon);

//@src/fragments/battle_engine/battle_engine_animation_system.c
void BattleAnim_PlayMenuScrollSoundEffect(void);
void BattleAnim_PlayMenuConfirmSoundEffect(void);
void BattleAnim_PlayMenuBackSoundEffect(void);
void BattleAnim_PlayMenuErrorSoundEffect(void);
void BattleAnim_PlayOwnerCry(ActivePokemon* active_pokemon, s16 species);
void BattleAnim_PlaySpeciesSequence33(ActivePokemon* active_pokemon);
void Battle_TriggerTrainerPresentationCue(ActivePokemon* active_pokemon);
void BattleAnim_PlaySpeciesReactionSound(ActivePokemon* active_pokemon, BattleMonRuntime* runtime, s32 reaction_kind);
void BattleScene_ConfigureAudioChannelGroupsByTeam(void);
void BattleScene_ConfigureAudioChannelGroupCounts(void);
void BattleScene_SelectBattleMusic(void);
void BattleScene_StartBattleMusic(void);
void BattleAnim_PlayBattleSequenceById(ActivePokemon* active_pokemon, s32 sequence_id);
void BattleAnim_RunEffectInitStart(s16 arg0);
void BattleAnim_RunEffectInitEnd(s16 arg0);
void BattleAnim_RegisterEffectSlot(func_D_84390300 arg0, func_D_84390320 arg1);
void BattleAnim_RunEffectList(s32 arg0, s32 arg1, s32 arg2);
void BattleAnim_ProcessQueuedEffects(void);
void BattleAnim_UpdateFrame(UNUSED CameraNode* camera);
void BattleAnim_SetQueuedEffectList(u8 effect_list_index);
u8 BattleAnim_GetCurrentEffectList(void);
void BattleAnim_SetQueuedResourceListId(u8 arg0);
u8 BattleAnim_GetQueuedResourceListId(void);
s16 BattleAnim_GetQueuedMoveId(void);
void BattleAnim_QueueMoveEffect(s32 move_id, ActivePokemon* user, ActivePokemon* user_fallback, s16 default_anchor,
                                s16 alternate_anchor);
void BattleAnim_QueueMoveEffectScript(s32 move_id, ActivePokemon* user, ActivePokemon* user_fallback,
                                      s16 default_anchor, s16 alternate_anchor);
void BattleAnim_QueueMoveEffectEnd(s32 move_id, ActivePokemon* user, ActivePokemon* user_fallback, s16 default_anchor,
                                   s16 alternate_anchor);
void BattleAnim_QueueChargingMoveEffectScript(s32 move_id, ActivePokemon* user, ActivePokemon* user_fallback,
                                              s16 default_anchor, s16 alternate_anchor);
void BattleAnim_QueueEffectList3(s32 move_id, ActivePokemon* user, ActivePokemon* user_fallback, s16 default_anchor,
                                 s16 alternative_anchor);
void BattleAnim_QueueEffectList6(s32 move_id, ActivePokemon* owner);
void BattleAnim_QueueEffectList5(s32 arg0, ActivePokemon* arg1);
void BattleAnim_QueueEffectList6Alt(s32 arg0, ActivePokemon* arg1);
void BattleAnim_QueueEffectList9(s32 arg0);
void BattleAnim_QueueEffectList(u8 resource_list_id, ActivePokemon* owner);
void BattleAnim_StopOwnerCategoryParticles(u8 category, ActivePokemon* owner);
void BattleAnim_RegisterAssetTable(arg1_func_80019420* arg0);
void BattleAnim_ResetAssetTable(void);
void BattleAnim_BuildResourceListForMove(s16 arg0);
void BattleAnim_LoadMoveResourceList(BattleSessionTeams* battle_session_teams, s16 move_id);
void BattleAnim_LoadModeResourceList(BattleSessionTeams* battle_session_teams, s16 mode);
void BattleAnim_RegisterAllAssetTables(void);
void BattleAnim_RegisterAssetTablesForSession(BattleSessionTeams* battle_session_teams);
void BattleAnim_ResetDisplayContexts(void);
Particle* BattleAnim_GetOwnerContextParticle(DisplayObject* arg0);
DisplayObject* BattleAnim_AllocOwnerDisplayContext(Particle* arg0);
void BattleAnim_FreeOwnerDisplayContext(DisplayObject* arg0);
DisplayObject* BattleAnim_AllocParticleDisplayContext(Particle* arg0);
void BattleAnim_FreeParticleDisplayContext(DisplayObject* arg0);
void BattleAnim_InitModelWithAnim(DisplayObject* arg0, s16 arg1, s16 arg2);
void BattleAnim_SetModelAnimFrame(DisplayObject* arg0, s16 arg1);
void BattleAnim_InitModel(DisplayObject* arg0, s16 arg1);
void BattleAnim_BindModelAnim(DisplayObject* arg0, s16 arg1);
void BattleAnim_BindModelAnimReversed(DisplayObject* arg0, s16 arg1);
s32 BattleAnim_IsModelAnimComplete(DisplayObject* arg0);
void BattleAnim_UpdateEffectCallbacks(UNUSED CameraNode* camera);
void BattleAnim_DrawEffectCallbacks(UNUSED CameraNode* camera);
void BattleAnim_UpdateCamera(CameraNode* camera);
s32 BattleAnim_DispatchPresentation(s32 arg0, CameraNode* arg1);
void BattleAnim_Initialize(void);
void BattleAnim_Reset(void);
void BattleAnim_GetPointAlongCameraRay(Vec3f* arg0, f32 arg1);
void BattleAnim_GetCameraEyeAngles(s16* arg0, s16* arg1);
void BattleAnim_GetCameraEyeYaw(s16* arg0);
void BattleAnim_GetCameraLookAngles(s16* arg0, s16* arg1);
void BattleAnim_GetCameraLookYaw(s16* arg0);
void BattleAnim_GetCameraStoredAngles76(s16* arg0, s16* arg1);
void BattleAnim_GetCameraStoredYaw78(s16* arg0);
void BattleAnim_GetCameraStoredAngles70(s16* arg0, s16* arg1);
void BattleAnim_GetCameraStoredYaw72(s16* arg0);

//@src/fragments/battle_engine/battle_engine_particle_system.c
void BattleAnim_InitParticleSystem(void);
BattleAnimEffectSlot* BattleAnim_AllocEffectSlot(void);
void BattleAnim_FreeEffectSlot(BattleAnimEffectSlot* arg0);
void BattleAnim_InitializeParticleChain(Particle* arg0, Vec3f arg1, Vec3s arg2, s32 arg3, s32 arg4);
void BattleAnim_CloneParticleState(Particle* arg0, Particle* arg1, ParticleUpdateCallback arg2,
                                   ParticleDescriptor* arg3, s32 arg4);
void BattleAnim_EmitParticleBurst(Particle* arg0, ParticleUpdateCallback arg1, ParticleDescriptor* arg2, s16 arg3);
void BattleAnim_EmitParticleBurstWithMetadata(Particle* arg0, ParticleUpdateCallback arg1, ParticleDescriptor* arg2,
                                              s16 arg3, s16 arg4, s16 arg5);
Particle* BattleAnim_CreateProceduralParticle(s16 arg0, s16 arg1, s32 arg2, f32 arg3);
void BattleAnim_ReleaseParticleModel(Particle* arg0);
void BattleAnim_DestroyParticle(Particle* arg0);
void BattleAnim_InitializeParticleAnchor(Particle* arg0);
void BattleAnim_UpdateParticleAnchor(Particle* arg0);
void BattleAnim_UpdateParticleCameraScale(Particle* arg0);
void BattleAnim_UpdateOrDestroyParticle(Particle* arg0);
void BattleAnim_UpdateParticlePool(void);
void BattleAnim_InitializeParticleFromEffect(Particle* arg0, ActivePokemon* arg1, BattleAnimEffectSlot* arg2, s16 arg3);
void BattleAnim_SpawnEffectParticles(BattleAnimEffectSlot* arg0, s32 arg1);
void BattleAnim_SpawnStandardParticles(BattleAnimEffectSlot* arg0);
void BattleAnim_SpawnBatchedParticles(BattleAnimEffectSlot* arg0);
void BattleAnim_SpawnSingleParticle(BattleAnimEffectSlot* arg0);
void BattleAnim_DispatchParticleEmission(BattleAnimEffectSlot* arg0);
void BattleAnim_TickEffectSlots(void);
void BattleAnim_UpdateEffects(void);
void BattleAnim_CleanupEffectsRetainingCategories47(void);
void BattleAnim_CleanupEffectsRetainingCategory4(void);
s32 BattleAnim_GetMoveFailedFlag(void);
void BattleAnim_UpdateMoveFailedFlag(void);
void BattleAnim_ClearEffectCallbacks(void);
s32 BattleAnim_IsEffectSpawningEnabled(void);
void BattleAnim_DisableEffectSpawning(void);
void BattleAnim_EnableEffectSpawning(void);
void BattleAnim_DisableEffectSpawningAlias(void);
void BattleAnim_CleanupEffects(s32 cleanup_mode);
void BattleAnim_StopParticlesForOwnerCategory(u8 arg0, ActivePokemon* arg1);
void BattleAnim_ResetSpawnDelay(void);
void BattleAnim_SetSpawnDelay(s32 arg0);
void BattleAnim_AddSpawnDelay(s32 arg0);
void BattleAnim_ResetSpawnDelayAlias(void);
BattleAnimEffectSlot* BattleAnim_CreateEffectSlot(s32 arg0, s32 arg1, s8 arg2, func_unk_D_8140E724 arg3,
                                                  ParticleDescriptor* arg4, ActivePokemon* arg5, s16 arg6, s16 arg7,
                                                  u8 arg8, u8 arg9, u8 argA, u8 argB);
BattleAnimEffectSlot* BattleAnim_CreateEffectSlotForCurrentOwner(s32 arg0, func_unk_D_8140E724 arg1,
                                                                 ParticleDescriptor* arg2, s16 arg3, s16 arg4, u8 arg5,
                                                                 u8 arg6, u8 arg7, u8 arg8);
BattleAnimEffectSlot* BattleAnim_CreateRepeatingEffectSlotForCurrentOwner(s32 arg0, s32 arg1, s8 arg2,
                                                                          func_unk_D_8140E724 arg3,
                                                                          ParticleDescriptor* arg4, s16 arg5, s16 arg6,
                                                                          u8 arg7, u8 arg8, u8 arg9, u8 argA);
BattleAnimEffectSlot* BattleAnim_SpawnGlobalTintColorFadeDetach(s32 arg0, s16 arg1, u8 arg2);
BattleAnimEffectSlot* BattleAnim_SpawnWhiteHueChange(s32 arg0, s16 arg1, s16 arg2, u8 arg3, u8 arg4);
BattleAnimEffectSlot* BattleAnim_SpawnDescriptor25AlphaHoldFadeOutFullAlpha(s32 arg0, s16 arg1, s16 arg2, u8 arg3);
BattleAnimEffectSlot* BattleAnim_SpawnDescriptor25AlphaHoldFadeOut(s32 arg0, s16 arg1, s16 arg2, u8 arg3, u8 arg4);
BattleAnimEffectSlot* BattleAnim_SpawnDescriptor25AlphaHoldFadeOutRepeating(s32 arg0, s32 arg1, s8 arg2, s16 arg3,
                                                                            s16 arg4, u8 arg5, u8 arg6);
BattleAnimEffectSlot* BattleAnim_SpawnDescriptor25AlphaFadeInHoldFullAlpha(s32 arg0, s16 arg1, s16 arg2, u8 arg3);
BattleAnimEffectSlot* BattleAnim_SpawnDescriptor25AlphaFadeInHold(s32 arg0, s16 arg1, s16 arg2, u8 arg3, u8 arg4);
BattleAnimEffectSlot* BattleAnim_SpawnDescriptor25AlphaFadeInHoldRepeating(s32 arg0, s32 arg1, s8 arg2, s16 arg3,
                                                                           s16 arg4, u8 arg5, u8 arg6);
BattleAnimEffectSlot* BattleAnim_SpawnDescriptor25AlphaHoldDetachFullAlpha(s32 arg0, s16 arg1, u8 arg2);
BattleAnimEffectSlot* BattleAnim_SpawnDescriptor25AlphaHoldDetach(s32 arg0, s16 arg1, u8 arg2, u8 arg3);
BattleAnimEffectSlot* BattleAnim_SpawnDescriptor25AlphaHoldDetachRepeating(s32 arg0, s32 arg1, s8 arg2, s16 arg3,
                                                                           u8 arg4, u8 arg5);
BattleAnimEffectSlot* BattleAnim_SpawnDescriptor25PaletteFadeOut(s32 arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4);
BattleAnimEffectSlot* BattleAnim_SpawnDescriptor25PaletteFadeIn(s32 arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4);
BattleAnimEffectSlot* BattleAnim_SpawnOwnerModelTintRampDetach(s32 arg0, u8 arg1, s16 arg2, s16 arg3, s16 arg4);
BattleAnimEffectSlot* BattleAnim_SpawnOwnerModelTintPulseDetach(s32 arg0, s16 arg1, u8 arg2, s16 arg3, s16 arg4,
                                                                s16 arg5);
BattleAnimEffectSlot* BattleAnim_SpawnOwnerModelTintHoldFadeDetach(s32 arg0, s16 arg1, u8 arg2, s16 arg3, s16 arg4);
BattleAnimEffectSlot* BattleAnim_SpawnBeamBoomWaterfallHueChange(s32 arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4);
BattleAnimEffectSlot* BattleAnim_SpawnOwnerModelTintPaletteApproachInactiveAlt(s32 arg0, s16 arg1, s16 arg2, s16 arg3,
                                                                               s16 arg4);
BattleAnimEffectSlot* BattleAnim_SpawnIceBeamBlizzardHitHueChange(s32 arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4,
                                                                  s16 arg5);
BattleAnimEffectSlot* BattleAnim_SpawnOwnerModelTintPaletteCycleVariant(s32 arg0, s16 arg1, s16 arg2, s16 arg3,
                                                                        s16 arg4, s16 arg5);
BattleAnimEffectSlot* BattleAnim_SpawnDrainFlameHueChange(s32 arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4, s16 arg5);
BattleAnimEffectSlot* BattleAnim_SpawnOwnerModelTintRandomPaletteCycle(s32 arg0, s16 arg1, u8 arg2);
BattleAnimEffectSlot* BattleAnim_SpawnRainbowHueChange(s32 arg0, s16 arg1, s16 arg2, u8 arg3);
BattleAnimEffectSlot* BattleAnim_SpawnInvisibility(s32 arg0, s16 arg1, s16 arg2, s16 arg3);
BattleAnimEffectSlot* BattleAnim_SpawnOwnerModelAlphaFadeFullPreset(s32 arg0);
BattleAnimEffectSlot* BattleAnim_SpawnOwnerModelAlphaFadeZeroStartPreset(s32 arg0);
BattleAnimEffectSlot* BattleAnim_SpawnCategory17MarkerSlot(s32 arg0, u8 arg1);
BattleAnimEffectSlot* BattleAnim_SpawnCategory19MarkerSlot(s32 arg0, u8 arg1);
void BattleAnim_EffectStartNop(void);
void BattleAnim_EffectEndNop(void);
void BattleAnim_SetEffectStateFlag(s32 arg0);
void BattleAnim_ClearEffectStateFlag(void);
s32 BattleAnim_GetEffectStateFlag(void);
s32 BattleAnim_IsCurrentOwnerSpecies(s32 species);
s32 BattleAnim_GetSecondaryOwnerSpecies(void);
s32 BattleAnim_GetTertiaryOwnerSpecies(void);
void BattleAnim_GetOwnerBonePosition(ActivePokemon* arg0, s16 arg1, Vec3f* arg2);
void BattleAnim_GetCurrentOwnerBonePosition(Vec3f* arg0);
void BattleAnim_GetSecondaryOwnerBonePosition(Vec3f* arg0);
void BattleAnim_GetTertiaryOwnerBonePosition(Vec3f* arg0);
void BattleAnim_GetSecondaryOwnerAnchorPosition(Vec3f* arg0);
void BattleAnim_GetTertiaryOwnerBone9Position(Vec3f* arg0);
void BattleAnim_GetOwnerModelBoundsScaled(ActivePokemon* arg0, Vec3f* arg1);
f32 BattleAnim_GetSpeciesBaseScale(ActivePokemon* active_pokemon);
f32 BattleAnim_GetOwnerModelScale(ActivePokemon* active_pokemon);
void BattleAnim_GetOwnerSpeciesBaseScaleAlias(ActivePokemon* active_pokemon);
f32 BattleAnim_GetSecondaryOwnerModelScale(void);
void BattleAnim_GetSecondaryOwnerSpeciesBaseScale(void);
f32 BattleAnim_GetTertiaryOwnerModelScale(void);
void BattleAnim_GetTertiaryOwnerSpeciesBaseScale(void);
CameraNode* BattleAnim_GetCameraContext(void);
Particle* BattleAnim_SpawnSecondaryOwnerParticle(Vec3f arg0, u8 arg1);
void BattleAnim_SetParticlePosition(Particle* arg0, Vec3f arg1);
void BattleAnim_MarkParticleInactive(Particle* arg0);
s16 BattleAnim_GetParticleAngleYToTertiaryOwner(Particle* arg0);
void BattleAnim_SetGlobalTintFromParticle(Particle* arg0);
void BattleAnim_LoadGlobalTintToParticle(Particle* arg0);

//@src/fragments/battle_engine/battle_engine_stinger_projectiles.c
void BattleAnim_InitStingerTrailColor(Particle* arg0);
void BattleAnim_UpdateStingerProjectileLifecycle(Particle* arg0);
void BattleAnim_InitStingerTrailColorDuplicate(Particle* arg0);
void BattleAnim_SpawnStingerProjectile(Particle* arg0);
void BattleAnim_InitStingerMoveParticle(Particle* arg0, u8 arg1);

//@src/fragments/battle_engine/battle_engine_particle_renderers.c
s32 BattleAnim_DrawAsset95Ia8Billboard(Particle* arg0, CameraNode* arg1);
void BattleAnim_DrawFlame(Particle* arg0, CameraNode* arg1);
void BattleAnim_DrawFlameHit(Particle* arg0, CameraNode* arg1);
void BattleAnim_DrawFlameUse(Particle* arg0, CameraNode* arg1);
void BattleAnim_DrawAsset1FAnimatedI4WithAsset20DisplayList(Particle* arg0, CameraNode* arg1);
s32 BattleAnim_DrawDigRocks(Particle* arg0, UNUSED CameraNode* arg1);
void BattleAnim_DrawSporePuffs(Particle* arg0, UNUSED CameraNode* arg1);
void BattleAnim_DrawSporePuffsFast(Particle* arg0, UNUSED CameraNode* arg1);
void BattleAnim_DrawMistLightest(Particle* arg0, UNUSED CameraNode* arg1);
void BattleAnim_DrawAsset87AnimatedI4Quad(Particle* arg0, UNUSED CameraNode* arg1);
void BattleAnim_DrawFullScreenSlash(Particle* arg0, u8* arg1);
void BattleAnim_DrawFullScreenTripleSlash(Particle* arg0, UNUSED CameraNode* arg1);
void BattleAnim_DrawFullScreenSingleSlash(Particle* arg0, UNUSED CameraNode* arg1);
void BattleAnim_DrawSlashingEffect(Particle* arg0, UNUSED CameraNode* arg1);
void BattleAnim_DrawVineWhipLeaves(Particle* arg0, UNUSED CameraNode* arg1);
void BattleAnim_DrawPetalDanceSakura(Particle* arg0, UNUSED CameraNode* arg1);
void BattleAnim_DrawWingAttackFeathers(Particle* arg0, UNUSED CameraNode* arg1);
void BattleAnim_DrawAsset1DRotatedI4Quad(Particle* arg0, UNUSED CameraNode* arg1);
void BattleAnim_DrawParticleNoop(UNUSED Particle* arg0, UNUSED CameraNode* arg1);
void BattleAnim_DrawHyperBeamImpactThinBolts(Particle* arg0, UNUSED CameraNode* arg1);
void BattleAnim_DrawAsset13FrameI4YScaledQuadB(Particle* arg0, UNUSED CameraNode* arg1);
void BattleAnim_DrawBoltOfLightning(Particle* arg0, UNUSED CameraNode* arg1);
void BattleAnim_DrawLongBoltOfLightning(Particle* arg0, CameraNode* arg1);
void BattleAnim_DrawHyperBeamImpactThickBolts(Particle* arg0, CameraNode* arg1);
void BattleAnim_DrawThunderShockUnknown(Particle* arg0, CameraNode* arg1);
void BattleAnim_DrawAsset1BI4TriangleA(Particle* arg0, UNUSED CameraNode* arg1);
void BattleAnim_DrawAsset1BI4TriangleB(Particle* arg0, UNUSED CameraNode* arg1);
void BattleAnim_DrawAsset13FrameDiv2I4Quad(Particle* arg0, UNUSED CameraNode* arg1);
void BattleAnim_DrawAsset18FrameI4ModelQuad(Particle* arg0, CameraNode* arg1);
void BattleAnim_DrawAssetA9I4XYScaledQuad(Particle* arg0, UNUSED CameraNode* arg1);
void BattleAnim_DrawFadingFullScreenColorOverlay(Particle* arg0, UNUSED CameraNode* arg1);

//@src/fragments/battle_engine/battle_engine_tint_and_alpha_effects.c
void BattleAnim_SetupBackgroundRecolor(Particle* arg0);
void BattleAnim_RunBackgroundRecolor(Particle* arg0);
void BattleAnim_SetupTargetHueChange(Particle* arg0);
void BattleAnim_RunTargetHueChange(Particle* arg0);
void BattleAnim_SetupUserHueChange(Particle* arg0);
void BattleAnim_RunUserHueChange(Particle* arg0);
void func_8433209C(void);
void BattleAnim_CallbackOwnerModelTintHoldFadeDetach(Particle* arg0);
void BattleAnim_SetupBeamBoomWaterfallHueChange(Particle* arg0);
void BattleAnim_RunBeamBoomWaterfallHueChange(Particle* arg0);
void BattleAnim_SetupOwnerModelTintPaletteApproach(Particle* arg0);
void BattleAnim_CallbackOwnerModelTintPaletteApproach(Particle* arg0);
void BattleAnim_SetupIceBeamBlizzardHitHueChange(Particle* arg0);
void BattleAnim_RunIceBeamBlizzardHitHueChange(Particle* arg0);
void BattleAnim_SetupOwnerModelTintPaletteApproachRampDown(Particle* arg0);
void BattleAnim_CallbackOwnerModelTintPaletteApproachRampDown(Particle* arg0);
void BattleAnim_SetupDrainFlameHueChange(Particle* arg0);
void BattleAnim_RunDrainFlameHueChange(Particle* arg0);
void BattleAnim_SetupOwnerModelTintRandomPaletteCycle(Particle* arg0);
void BattleAnim_CallbackOwnerModelTintRandomPaletteCycle(Particle* arg0);
void BattleAnim_SetupRainbowHueChange(Particle* arg0);
void BattleAnim_RunRainbowHueChange(Particle* arg0);
void BattleAnim_SetupInvisibility(Particle* arg0);
void BattleAnim_RunInvisibility(Particle* arg0);
void BattleAnim_SetupParticleRecolorTransparent(Particle* arg0);
void BattleAnim_CallbackStartEffect125OwnerColorAlphaFade(Particle* arg0);
void BattleAnim_SetupWhiteHueChange(Particle* arg0);
void BattleAnim_RunWhiteHueChange(Particle* arg0);
void BattleAnim_SetupFullScreenHueChange(Particle* arg0);
void BattleAnim_RunFullScreenHueChange(Particle* arg0);
void BattleAnim_SetupDescriptor25AlphaFadeInHold(Particle* arg0);
void BattleAnim_RunPokemonAndFullScreenHueChange(Particle* arg0);
void BattleAnim_SetupDescriptor25AlphaHoldDetach(Particle* arg0);
void BattleAnim_CallbackDescriptor25AlphaHoldDetach(Particle* arg0);
void BattleAnim_SetupDescriptor25PaletteFadeOut(Particle* arg0);
void BattleAnim_CallbackDescriptor25PaletteFadeOut(Particle* arg0);
void BattleAnim_SetupDescriptor25PaletteFadeIn(Particle* arg0);
void BattleAnim_CallbackDescriptor25PaletteFadeIn(Particle* arg0);

//@src/fragments/battle_engine/battle_engine_flame_and_explosion_effects.c
void BattleAnim_SetupFireBreathFlameFading(Particle* arg0, s16 arg1);
void BattleAnim_ContinueFireBreathFlameFading(Particle* arg0);
void BattleAnim_RunFireBreathFlameFading(Particle* arg0);
void BattleAnim_SetupUnusedDescriptor4FColorCycleScaleFadeVariant(Particle* arg0);
void BattleAnim_MotionUnusedDescriptor4FColorCycleScaleFadeVariant(Particle* arg0);
void BattleAnim_CallbackUnusedDescriptor4FColorCycleScaleFadeVariant(Particle* arg0);
void BattleAnim_SetupFireBreathFlameGeometry(Particle* arg0);
void BattleAnim_ContinueFireBreathFlameGeometry(Particle* arg0);
void BattleAnim_RunFireBreathFlameGeometry(Particle* arg0);
void BattleAnim_SetupFireBreathFlameColoration(Particle* arg0, s16 arg1);
void BattleAnim_ContinueFireBreathFlameColoration(Particle* arg0);
void BattleAnim_RunFireBreathFlameColoration(Particle* arg0);
void BattleAnim_SetupExpandingHitSpark(Particle* arg0);
void BattleAnim_RunExpandingHitSpark(Particle* arg0);
void BattleAnim_RunPsybeamHitSparkDrift(Particle* arg0);
void BattleAnim_CallbackUnusedGrowFadeVariant(Particle* arg0);
void BattleAnim_SetupOscillatingFlameUnderneathPokemon(Particle* arg0);
void BattleAnim_RunOscillatingFlameUnderneathPokemon(Particle* arg0);
void BattleAnim_SetupFireSpinFlame(Particle* arg0);
void BattleAnim_RunFireSpinFlame(Particle* arg0);
void BattleAnim_SetupUnusedVariant2(Particle* arg0);
void BattleAnim_CallbackUnusedVariant2(Particle* arg0);
void BattleAnim_SetupEmberProjectileAfterimages(Particle* arg0);
void BattleAnim_RunEmberProjectileAfterimages(Particle* arg0);
void BattleAnim_SetupEmberProjectile(Particle* arg0);
void BattleAnim_RunEmberProjectile(Particle* arg0);
void BattleAnim_SetupEmberBurstEmitter(Particle* arg0);
void BattleAnim_RunEmberBurstEmitter(Particle* arg0);
void BattleAnim_SetupEmberUseEffect(Particle* arg0);
void BattleAnim_RunEmberUseEffect(Particle* arg0);
void BattleAnim_GetExplosionParticleColor(Particle* arg0, u8 arg1);
void func_84334B04(void);
void BattleAnim_FirePunchEmberEggBombColorChange(Particle* arg0);
void BattleAnim_SetupBackgroundBombExplosion(Particle* arg0);
void BattleAnim_RunBackgroundBombExplosion(Particle* arg0);
void BattleAnim_SetupBoomSmallExplosion(Particle* arg0);
void BattleAnim_RunBoomSmallExplosion(Particle* arg0);
void BattleAnim_SetupEggBombHitEffect(Particle* arg0);
void BattleAnim_RunEggBombHitEffect(Particle* arg0);
void BattleAnim_SetupSecondaryBombExplosion(Particle* arg0);
void BattleAnim_RunSecondaryBombExplosion(Particle* arg0);
void BattleAnim_SetupEmberHitFireball(Particle* arg0);
void BattleAnim_RunEmberHitFireball(Particle* arg0);
void BattleAnim_SetupEmberHitSmoke(Particle* arg0);
void BattleAnim_RunEmberHitSmoke(Particle* arg0);
void BattleAnim_SetupFirePunchLingeringHitSpark(Particle* arg0);
void BattleAnim_RunFirePunchLingeringHitSpark(Particle* arg0);
void BattleAnim_SetupAfterImpactSmoke(Particle* arg0);
void BattleAnim_RunAfterImpactSmoke(Particle* arg0);
void BattleAnim_SetupFirePunchHitEffect(Particle* arg0);
void BattleAnim_RunFirePunchHitEffect(Particle* arg0);
void BattleAnim_SetupFlamethrowerHitRotation(Particle* arg0, s16 arg1, s16 arg2);
void BattleAnim_RunFlamethrowerHitRotation(Particle* arg0, s32 arg1);
void BattleAnim_RunFlamethrowerHitRotationWeak(Particle* arg0);
void BattleAnim_RunFlamethrowerHitRotationStrong(Particle* arg0);
void BattleAnim_SetupFireBlastHitFrontFlames(Particle* arg0);
void BattleAnim_ContinueFireBlastHitFrontFlames(Particle* arg0);
void BattleAnim_RunFireBlastHitFrontFlames(Particle* arg0);
void BattleAnim_SetupDaimonji(Particle* arg0);
void BattleAnim_ContinueDaimonji(Particle* arg0);
void BattleAnim_RunDaimonji(Particle* arg0);
void BattleAnim_SetupDaimonjiColoration(Particle* arg0);
void BattleAnim_RunDaimonjiColoration(Particle* arg0);
void BattleAnim_InitializeEmberUse(void);
void BattleAnim_InitializeEmberHit(void);
void BattleAnim_InitializeFirePunchHit(void);
void BattleAnim_InitializeFireBlastFlamethrowerUseSmoke(void);
void BattleAnim_InitializeFlamethrowerHit(void);
void BattleAnim_InitializeDragonRageUse(void);
void BattleAnim_InitializeDragonRageHit(void);
void BattleAnim_InitializeFireSpinUseSmoke(void);
void BattleAnim_InitializeFireSpinHitFlames(void);
void func_8433765C(void);
void ParticleEffect_InitNoop(void);
void BattleAnim_InitializeEggBombHit(void);
void func_843378CC(void);
void BattleAnim_InitializeFireBlastHit(void);

//@src/fragments/battle_engine/battle_engine_water_and_beam_effects.c
void BattleAnim_SetupSurfHitSpark(Particle* arg0);
void BattleAnim_RunSurfHitSpark(Particle* arg0);
void BattleAnim_SetupBubbleBubbles(Particle* arg0, s16 arg1);
void BattleAnim_RunBubbleBubbles(Particle* arg0);
void BattleAnim_SetupBubbleBeamBubblesSlow(Particle* arg0);
void BattleAnim_ContinueBubbleBeamBubblesSlow(Particle* arg0);
void BattleAnim_RunBubbleBeamBubblesSlow(Particle* arg0);
void BattleAnim_SetupBubbleBeamBubblesFast(Particle* arg0);
void BattleAnim_RunBubbleBeamBubblesFast(Particle* arg0);
void BattleAnim_SetupUnusedRotatingSparkVariant(Particle* arg0);
void BattleAnim_MotionUnusedRotatingSparkVariant(Particle* arg0);
void BattleAnim_CallbackUnusedRotatingSparkVariant(Particle* arg0);
void BattleAnim_SetupUnusedAimedSparkVariant(Particle* arg0);
void BattleAnim_CallbackUnusedAimedSparkVariant(Particle* arg0);
void BattleAnim_SetupWaterGunAndHydroPumpEmitterFade(Particle* arg0);
void BattleAnim_RunWaterGunAndHydroPumpEmitterFade(Particle* arg0);
void BattleAnim_SetupHydroPumpBurstChild(Particle* arg0);
void BattleAnim_RunHydroPumpBurstChild(Particle* arg0);
void BattleAnim_SetupHydroPumpMetadataBurstChild(Particle* arg0);
void BattleAnim_RunHydroPumpMetadataBurstChild(Particle* arg0);
void BattleAnim_SetupHydroPumpBurstEmitter(Particle* arg0);
void BattleAnim_RunHydroPumpBurstEmitter(Particle* arg0);
void BattleAnim_SetupWaterGunProjectileChild(Particle* arg0);
void BattleAnim_RunWaterGunProjectileChild(Particle* arg0);
void BattleAnim_SetupWaterGunProjectileMetadata(Particle* arg0);
void BattleAnim_RunWaterGunProjectileMetadata(Particle* arg0);
void BattleAnim_SetupWaterGunProjectile(Particle* arg0);
void BattleAnim_RunWaterGunProjectile(Particle* arg0);
void BattleAnim_SetupIceBeamColdBreath(Particle* arg0);
void BattleAnim_ContinueIceBeamColdBreath(Particle* arg0);
void BattleAnim_RunIceBeamColdBreath(Particle* arg0);
void BattleAnim_SetupMegaKickAndPunchProjectile(Particle* arg0);
void BattleAnim_RunMegaKickAndPunchProjectile(Particle* arg0);
void BattleAnim_SetupInitialBeamEmanator(Particle* arg0);
void BattleAnim_RunInitialBeamEmanator(Particle* arg0);
void BattleAnim_SetupPulsingBeamEmanator(Particle* arg0);
void BattleAnim_RunPulsingBeamEmanator(Particle* arg0);
void BattleAnim_SetupStrongBeamHitEffect(Particle* arg0);
void BattleAnim_RunStrongBeamHitEffect(Particle* arg0);
void BattleAnim_SetupPsybeamHitSparkFlicker(Particle* arg0);
void BattleAnim_RunPsybeamHitSparkFlicker(Particle* arg0);
void BattleAnim_SetupAuroraBeamHitSpark(Particle* arg0);
void BattleAnim_RunAuroraBeamHitSpark(Particle* arg0);
void BattleAnim_SetupStrongOutwardMovingLargeHitSpark(Particle* arg0);
void BattleAnim_RunStrongOutwardMovingLargeHitSpark(Particle* arg0);
void BattleAnim_SetupBlizzardHitUpwardColdAir(Particle* arg0);
void BattleAnim_RunBlizzardHitUpwardColdAir(Particle* arg0);
void BattleAnim_SetupWaterGunHitEffect(Particle* arg0);
void BattleAnim_RunWaterGunHitEffect(Particle* arg0);
void BattleAnim_SetupWeakBeamHitEffect(Particle* arg0);
void BattleAnim_RunWeakBeamHitEffect(Particle* arg0);
void BattleAnim_SetupTriAttackHitSpark(Particle* arg0);
void BattleAnim_RunTriAttackHitSpark(Particle* arg0);
void BattleAnim_SetupStrongOutwardMovingSmallHitSpark(Particle* arg0);
void BattleAnim_RunStrongOutwardMovingSmallHitSpark(Particle* arg0);
void BattleAnim_SetupUnusedIndexedFadeVariantA(Particle* arg0);
void BattleAnim_CallbackUnusedIndexedFadeVariantA(Particle* arg0);
void BattleAnim_SetupUnusedIndexedFadeVariantB(Particle* arg0);
void BattleAnim_CallbackUnusedIndexedFadeVariantB(Particle* arg0);
void BattleAnim_SetupBubbleBubbleBeamSharedEffect(Particle* arg0);
void BattleAnim_RunBubbleBubbleBeamSharedEffect(Particle* arg0);
void BattleAnim_SetupHydroPumpColorShiftFade(Particle* arg0);
void BattleAnim_RunHydroPumpColorShiftFade(Particle* arg0);
void BattleAnim_SetupWaterGunEmitterColoration(Particle* arg0);
void BattleAnim_RunWaterGunEmitterColoration(Particle* arg0);
void BattleAnim_SetupHydroPumpWaterGunProjectileHit(Particle* arg0);
void BattleAnim_RunHydroPumpWaterGunProjectileHit(Particle* arg0);
void BattleAnim_StartMegaPunchEffect(void);
void BattleAnim_DispatchNormalMegaKickAnimation(void);
void BattleAnim_InitializeBubbleUse(void);
void BattleAnim_InitializeBubbleHit(void);
void BattleAnim_InitializeWaterGunUse(void);
void BattleAnim_InitializeWaterGunHit(void);
void BattleAnim_InitializeHydroPumpUse(void);
void BattleAnim_InitializeHydroPumpHit(void);
void BattleAnim_InitializeIceBeamUse(void);
void BattleAnim_InitializePsybeamUse(void);
void BattleAnim_InitializeBubbleBeamUse(void);
void BattleAnim_InitializeAuroraBeamUse(void);
void BattleAnim_InitializeHyperBeamUse(void);
void BattleAnim_InitializeSolarBeamUse1(void);
void BattleAnim_InitializeTriAttackUse(void);
void InitParticleComboVariant(void);
void BattleAnim_InitializeIceBeamHit(void);
void BattleAnim_InitializePsybeamHit(void);
void BattleAnim_InitializeBubbleBeamHit(void);
void BattleAnim_InitializeAuroraBeamHit(void);
void BattleAnim_InitializeHyperBeamHit(void);
void BattleAnim_InitializeSolarBeamHit(void);
void BattleAnim_InitializeSkyAttackHitSequence(void);
void BattleAnim_InitializeTriAttackHit(void);

//@src/fragments/battle_engine/battle_engine_electric_burst_effects.c
void BattleAnim_SetupStrongElectricSpark(Particle* arg0);
void BattleAnim_RunStrongElectricSpark(Particle* arg0);
void BattleAnim_ApplyThunderThunderboltUseRadialOffset(Particle* arg0);
void BattleAnim_SetupThunderThunderboltUseEffect(Particle* arg0);
void BattleAnim_RunThunderThunderboltUseEffect(Particle* arg0);
void BattleAnim_SetupSharedDescriptor0FDelayedRisingColumnFade(Particle* arg0);
void BattleAnim_RunDeterministicRotatingBoltOfLightning(Particle* arg0);
void BattleAnim_SetupSharedDescriptor0FDelayedRisingColumnRandomScale(Particle* arg0);
void BattleAnim_RunRandomRotatingBoltOfLightning(Particle* arg0);
void BattleAnim_SetupSharedDirectionalSprayFade(Particle* arg0);
void BattleAnim_RunRandomStationaryBoltOfLightning(Particle* arg0);
void BattleAnim_SetupSharedDescriptor13DirectionalSprayScaledFade(Particle* arg0);
void BattleAnim_RunDeterministicStationaryBoltOfLightning(Particle* arg0);
void BattleAnim_SetupThunderShockOuterJolts(Particle* arg0);
void BattleAnim_RunThunderShockOuterJolts(Particle* arg0);
void BattleAnim_SetupUnregisteredStraightDirectionalStreamFade(Particle* arg0);
void BattleAnim_CallbackUnregisteredStraightDirectionalStreamFade(Particle* arg0);
void BattleAnim_SetupEndEffect28Descriptor10UpwardBurstFade(Particle* arg0);
void BattleAnim_CallbackEndEffect28Descriptor10UpwardBurstFade(Particle* arg0);
void BattleAnim_SetupEndEffect28Descriptor10WideScatterBurstFade(Particle* arg0);
void BattleAnim_CallbackEndEffect28Descriptor10WideScatterBurstFade(Particle* arg0);
void BattleAnim_InitializeThunderThunderboltUse(void);
void BattleAnim_InitializeThunderShockThunderboltHit(void);
void BattleAnim_InitializeThunderHit(void);
void BattleAnim_InitializeThunderShockUse(void);
void BattleAnim_CreateBurstStreamEffect(void);
void BattleAnim_InitializeThunderPunchUse(void);
void BattleAnim_InitializeHyperBeamHitSequence(void);

//@src/fragments/battle_engine/battle_engine_328F10.c
void BattleAnim_InitStartEffect135Descriptor1CRisingScale(Particle* arg0);
void BattleAnim_CallbackStartEffect135Descriptor1CRisingScaleFade(Particle* arg0);
void BattleAnim_InitStartEffect135ExpandingFlash(Particle* arg0);
void BattleAnim_CallbackStartEffect135ExpandingFlashFade(Particle* arg0);
void BattleAnim_InitStartEffect139Descriptor42FallingScale(Particle* arg0);
void BattleAnim_CallbackStartEffect139Descriptor42FallingScaleFade(Particle* arg0);
void BattleAnim_InitStartEffect139Descriptor22SpinFade(Particle* arg0);
void BattleAnim_CallbackStartEffect139Descriptor22SpinFadeSpark(Particle* arg0);
void BattleAnim_InitStartEffect141Descriptor1CGridRise(Particle* arg0);
void BattleAnim_CallbackStartEffect141Descriptor1CGridRiseFade(Particle* arg0);
void BattleAnim_InitStartEffects133And134And143Descriptor44ModeOrbit(Particle* arg0);
void BattleAnim_CallbackStartEffects133And134And143Descriptor44ModeOrbitSwirl(Particle* arg0);
void BattleAnim_InitStartEffects133And134And143Descriptor38MetadataBurst(Particle* arg0);
void BattleAnim_CallbackStartEffects133And134And143Descriptor38MetadataBurstEmitter(Particle* arg0);
s16 BattleAnim_GetSessionModeEffectParam(void);
void BattleAnim_StartEffect139DualRepeatingSparks(void);
void BattleAnim_StartEffect140StartEffect139Alias(void);
void BattleAnim_StartEffect141GridRiseAndFallingBurst(void);
void BattleAnim_StartEffect142StartEffect141Alias(void);
void BattleAnim_StartEffect135ExpandingFlashAndRisingScaleFade(void);
void BattleAnim_StartEffect134ModeOrbitAndMetadataBurst(void);
void BattleAnim_StartEffect143ModeOrbitAndMetadataBurst(void);
void BattleAnim_StartEffect133ModeOrbitBurstAndEffect135Followup(void);
void BattleAnim_StartEffect122GlobalAndOwnerTintFade(void);

//@src/fragments/battle_engine/battle_engine_lingering_fall_particles.c
void BattleAnim_UnusedResetAppearanceAndAdvance(Particle* arg0);
void BattleAnim_CallbackUnusedGenericFadeApproach(Particle* arg0);
void BattleAnim_SetupOrphanDescriptor2BSpinFade(Particle* arg0);
void BattleAnim_CallbackOrphanDescriptor2BSpinFade(Particle* arg0);
void BattleAnim_SetupOrphanDescriptor2EScatterFade(Particle* arg0);
void BattleAnim_CallbackOrphanDescriptor2EScatterFade(Particle* arg0);
void BattleAnim_SetupPetalDanceLingeringHitEffect(Particle* arg0);
void BattleAnim_RunPetalDanceLingeringHitEffect(Particle* arg0);
void BattleAnim_SetupSlowFallingHitEffect(Particle* arg0);
void BattleAnim_RunSlowFallingHitEffect(Particle* arg0);
void func_84340AC4(void);
void BattleAnim_OrphanDescriptor2BAnd2EEmitterSequence(void);
void BattleAnim_InitializePetalDanceHit(void);
void BattleAnim_InitializeWingAttackHit(void);

//@src/fragments/battle_engine/battle_engine_model_and_overlay_effects.c
void BattleAnim_InitializeThunderWaveUse(void);
void BattleAnim_InitializeThunderWaveHit(void);
void BattleAnim_StartEffect1Descriptor2FPhaseFadeOut(void);
void BattleAnim_InitializeAmnesiaUse(void);
void BattleAnim_ApplySpeciesMotionOffset(Particle*);
void BattleAnim_InitializeScreenMove(void);
void ParticleEffect_RegisterCallbacks(void);
void ParticleEffect_RegisterParticleNodes(void);
void BattleAnim_InitializeSmokeScreenHazePoisonGasUse(void);
void BattleAnim_InitializeSmogUse(void);
void BattleAnim_InitializeMistUse(void);
void BattleAnim_InitializeLeechSeedUse(void);
void BattleAnim_InitializeLeechSeedHit(void);
void BattleAnim_InitializePayDayHit(void);
void BattleAnim_InitializeBlizzardUse(void);
void BattleAnim_InitializeBlizzardHit(void);
void BattleAnim_InitializeDizzyPunchHit(void);
void ParticleEffect_RegisterColorAnim(void);
void ParticleEffect_RegisterModeSpecific(void);
void BattleAnim_InitializeFlyUse(void);
void BattleAnim_InitializeTeleportUse(void);
void ParticleEffect_RegisterHandlers_Default(void);
void BattleAnim_InitializeRockSlideRockThrowHit(void);
void BattleAnim_InitializeStrengthUse(void);
void BattleAnim_InitializeStrengthHit(void);
void BattleAnim_InitializeEggBombUse(void);
void BattleAnim_InitializeSoftBoiledUse(void);
void BattleAnim_InitializeStrongWind(void);
void BattleAnim_ApplySpeciesXZOffsetVariantA(Particle*);
void BattleAnim_InitializeRazorWindUse(void);

//@src/fragments/battle_engine/battle_engine_model_and_overlay_effects.c
void BattleAnim_SetupReflectiveCamera(void);
void BattleAnim_SelectParticleModelFrames(Particle* arg0, s32 arg1);
void BattleAnim_CallbackInitModelAnimWait(Particle* arg0);
void BattleAnim_InitializeParticleModel(Particle* arg0);
void BattleAnim_RunScaryEyes(Particle* arg0);
void BattleAnim_RunDreamEaterUseEffect(Particle* arg0);
void BattleAnim_CallbackInitModelAnimMode2(Particle* arg0);
void BattleAnim_RunFillScreenOverlay(Particle* arg0);
Gfx* BattleAnim_BuildAnimatedTileDisplayList(Gfx* arg0, u8* arg1, s16 arg2, s16 arg3, s16 arg4, s16 arg5, s16 arg6,
                                             s16 arg7, s16 arg8, s16 arg9, s16 argA, s16 argB);
Gfx* BattleAnim_BuildAnimatedTileDisplayListFromStruct(Gfx* arg0, arg1_func_8434E21C* arg1, s16 arg2, s16 arg3,
                                                       s16 arg4, s16 arg5, s16 arg6, s16 arg7, s16 arg8, s16 arg9,
                                                       s16 argA, s16 argB);
void Gfx_SetParticlePrimEnvColors(Gfx* arg0);
void ParticleEffect_AllocateGfxIfNeeded(s32 arg0, arg1_func_84344CE8* arg1);
void Gfx_DrawParticleTextureWithParams(Gfx* arg0, arg1_func_8434E21C* arg1, UNUSED u16 arg2);
void ParticleEffect_AllocateGfxAndDraw(s32 arg0, arg1_func_84344CE8* arg1);
void BattleAnim_SetupThunderWaveHitEffect(Particle* arg0);
void BattleAnim_RunThunderWaveHitEffect(Particle* arg0);
void BattleAnim_ApplyThunderWaveUseSpeciesOffset(Particle* arg0);
void BattleAnim_SetupThunderWaveUseEffect(Particle* arg0);
void BattleAnim_RunThunderWaveUseEffect(Particle* arg0);
void BattleAnim_InitializeThunderWaveUse(void);
void BattleAnim_InitializeThunderWaveHit(void);
void BattleAnim_ApplyEffect1SpeciesMotion(Particle* arg0);
void BattleAnim_SetupEffect1Descriptor2F(Particle* arg0);
void BattleAnim_CallbackStartEffect1Descriptor2FPhaseFadeOut(Particle* arg0);
void BattleAnim_StartEffect1Descriptor2FPhaseFadeOut(void);
void BattleAnim_ApplyModelSpeciesOffset(Particle* arg0);
void BattleAnim_InitModelAnimationSlot(Particle* arg0, s16 arg1);
void BattleAnim_CallbackStartEffects12And129DelayedModelAnimWait(Particle* arg0);
void BattleAnim_InitializeAmnesiaUse(void);
void BattleAnim_ApplySpeciesMotionOffset(Particle* arg0);
void BattleAnim_InitializeScreenMove(void);
void BattleAnim_BuildReflectiveTextureDisplayList(Gfx* arg0, s32 arg1, u16 arg2);
void BattleAnim_AllocateReflectiveTextureDisplayList(s32 arg0, arg1_func_84344CE8* arg1);
void BattleAnim_ApplyEffect125ModelScaleOffset(Particle* arg0);
void BattleAnim_InitEffect125Model(Particle* arg0, UNUSED s16 arg1);
void BattleAnim_CallbackStartEffect125ModelBurstEmitter(Particle* arg0);
void BattleAnim_BuildEffect125DisplayList(Gfx* arg0, u16 arg1);
void BattleAnim_AllocateEffect125DisplayList(s32 arg0, arg1_func_84344CE8* arg1);
void ParticleEffect_RegisterCallbacks(void);
void BattleAnim_InitEffect65Overlay(Particle* arg0);
void BattleAnim_RunSporePowderMoveOverlay(Particle* arg0);
void ParticleEffect_RegisterParticleNodes(void);
void BattleAnim_InitEffect28Overlay(Particle* arg0);
void BattleAnim_RunGasAttack(Particle* arg0);
void BattleAnim_InitializeSmokeScreenHazePoisonGasUse(void);
void BattleAnim_InitializeSmogUse(void);
void BattleAnim_InitializeMistUse(void);
void BattleAnim_SetupLeechSeedUseEffect1(Particle* arg0);
void BattleAnim_RunLeechSeedUseEffect1(Particle* arg0);
void BattleAnim_SetupLeechSeedUseEffect2(Particle* arg0);
void BattleAnim_RunLeechSeedUseEffect2(Particle* arg0);
void BattleAnim_ApplyLeechSeedHitSpeciesOffset(Particle* arg0);
void BattleAnim_SetupLeechSeedHitEffect(Particle* arg0);
void BattleAnim_RunLeechSeedHitEffect(Particle* arg0);
void BattleAnim_InitializeLeechSeedUse(void);
void BattleAnim_InitializeLeechSeedHit(void);
void BattleAnim_InitializePayDayHit(void);
void ParticleEffect_DrawSimple(Gfx* arg0, u8* arg1);
void ParticleEffect_CreateSimpleDL(s32 arg0, arg1_func_84344CE8* arg1);
void BattleAnim_SetupBlizzardHitSpark(Particle* arg0);
void BattleAnim_RunBlizzardHitSpark(Particle* arg0);
void BattleAnim_SetupBlizzard(Particle* arg0);
void BattleAnim_RunBlizzard(Particle* arg0);
void BattleAnim_InitializeBlizzardUse(void);
void BattleAnim_InitializeBlizzardHit(void);
void ParticleEffect_DrawNode(Gfx* arg0, s32 arg1, u16 arg2);
void ParticleEffect_CreateNodeDL(s32 arg0, arg1_func_84344CE8* arg1);
void ParticleEffect_DrawNodeVariant(Gfx* arg0, arg1_func_8434E21C* arg1, UNUSED u16 arg2);
void ParticleEffect_CreateNodeVariantDL(s32 arg0, arg1_func_84344CE8* arg1);
void ParticleEffect_DrawSimpleOffsetTileNoCamera(Gfx* arg0, u8* arg1, UNUSED u16 arg2);
void ParticleEffect_CreateSimpleOffsetTileNoCameraDL(s32 arg0, arg1_func_84344CE8* arg1);
void func_84345490(void);
void BattleAnim_ApplyDizzyPunchHitSpeciesOffset(Particle* arg0);
void BattleAnim_SetupDizzyPunchHitEffect(Particle* arg0);
void BattleAnim_RunDizzyPunchHitEffect(Particle* arg0);
void BattleAnim_InitializeDizzyPunchHit(void);
void BattleAnim_InitEffect49Model(Particle* arg0);
void BattleAnim_CallbackEndEffect49Model33Anim34WaitCleanup(Particle* arg0);
void BattleAnim_RunHealthAbsorptionParticles(Particle* arg0);
void BattleAnim_BuildEffect49DisplayList(Gfx* arg0, UNUSED s32 arg1, UNUSED u16 arg2);
void BattleAnim_AllocateEffect49DisplayList(s32 arg0, arg1_func_84344CE8* arg1);
void ParticleEffect_RegisterColorAnim(void);
void ParticleEffect_RegisterModeSpecific(void);
void BattleAnim_SetupFlyUpwardMovementEffect(Particle* arg0);
void BattleAnim_RunFlyUpwardMovementEffect(Particle* arg0);
void BattleAnim_InitializeFlyUse(void);
void BattleAnim_SetupTeleportUseEffect(Particle* arg0);
void BattleAnim_RunTeleportUseEffect(Particle* arg0);
void BattleAnim_InitializeTeleportUse(void);
void BattleAnim_BuildEffect63DisplayList(Gfx* arg0, s32 arg1, u16 arg2);
void BattleAnim_AllocateEffect63DisplayList(s32 arg0, arg1_func_84344CE8* arg1);
void BattleAnim_BuildEffect75DisplayList(Gfx* arg0, UNUSED s32 arg1, UNUSED u16 arg2);
void BattleAnim_AllocateEffect75DisplayList(s32 arg0, arg1_func_84344CE8* arg1);
void BattleAnim_InitEffects63And75Model(Particle* arg0);
void BattleAnim_RunRaisingRock(Particle* arg0);
void BattleAnim_InitEffects10And50Model(Particle* arg0);
void BattleAnim_RunFallingRock(Particle* arg0);
void BattleAnim_ApplyEffects63And75SpeciesScaleOffset(Particle* arg0);
void BattleAnim_InitEffects63And75Model38(Particle* arg0);
void BattleAnim_RunRockLiftingMotionEffect(Particle* arg0);
void Gfx_DrawParticleAlpha(Gfx* arg0, s32 arg1, u16 arg2);
void Particle_CreateDisplayList_Alpha(s32 arg0, arg1_func_84344CE8* arg1);
void Gfx_DrawParticleTexture4b(Gfx* arg0, u8* arg1, UNUSED u16 arg2);
void Particle_CreateDisplayList_Texture4b(s32 arg0, arg1_func_84344CE8* arg1);
void Gfx_DrawParticleGreenPrim(Gfx* arg0, u8* arg1, UNUSED u16 arg2);
void Particle_CreateDisplayList_GreenPrim(s32 arg0, arg1_func_84344CE8* arg1);
void ParticleEffect_RegisterHandlers_Default(void);
void BattleAnim_InitializeRockSlideRockThrowHit(void);
void BattleAnim_InitializeStrengthUse(void);
void BattleAnim_InitializeStrengthHit(void);
void BattleAnim_InitializeEggBombUse(void);
void BattleAnim_InitializeSoftBoiledUse(void);
void BattleAnim_SetupStrongWinds(Particle* arg0);
void BattleAnim_RunStrongWinds(Particle* arg0);
void Gfx_DrawParticleCyanEnv(Gfx* arg0, arg1_func_8434E21C* arg1, UNUSED u16 arg2);
void Particle_CreateDisplayList_CyanEnv(s32 arg0, arg1_func_84344CE8* arg1);
void Gfx_DrawParticleBluePrim(Gfx* arg0, arg1_func_8434E21C* arg1, UNUSED u16 arg2);
void Particle_CreateDisplayList_BluePrim(s32 arg0, arg1_func_84344CE8* arg1);
void BattleAnim_InitializeStrongWind(void);
void BattleAnim_InitializeRazorWindUse(void);
void BattleAnim_ApplySpeciesXZOffsetVariantA(Particle* arg0);
void BattleAnim_RunBoomUseEffect1(Particle* arg0);
void BattleAnim_SetupBoomUseEffect2(Particle* arg0);
void BattleAnim_RunBoomUseEffect2(Particle* arg0);
void BattleAnim_SetupBoomUseEffect3(Particle* arg0);
void BattleAnim_RunBoomUseEffect3(Particle* arg0);
void BattleAnim_SetupBoomUseEffect4(Particle* arg0);
void BattleAnim_RunBoomUseEffect4(Particle* arg0);
void Gfx_DrawParticleCustomColors(Gfx* arg0, arg1_func_8434E21C* arg1, UNUSED u16 arg2);
void Particle_CreateDisplayList_CustomColors(s32 arg0, arg1_func_84344CE8* arg1);
void BattleAnim_RunBoomUseEffect5(Particle* arg0);
void BattleAnim_RunBoomUseEffect6(Particle* arg0);
void BattleAnim_RunBoomHitEffect1(Particle* arg0);
void BattleAnim_SetupBoomHitEffect2(Particle* arg0);
void BattleAnim_RunBoomHitEffect2(Particle* arg0);
void BattleAnim_SetupBoomHitEffect3(Particle* arg0);
void BattleAnim_RunBoomHitEffect3(Particle* arg0);
void BattleAnim_InitializeBoomUse(void);
void BattleAnim_InitializeBoomHit(void);
void func_84348C6C(UNUSED s32 arg0, UNUSED s32 arg1);
void ParticleEffect_Noop(UNUSED s32 arg0, UNUSED s32 arg1);
void BattleAnim_SetupBitingEffect(Particle* arg0);
void BattleAnim_RunBitingEffect(Particle* arg0);
void Gfx_DrawParticleFromTempColors(Gfx* arg0, s32 arg1, u16 arg2);
void Particle_CreateDisplayList_TempColors(s32 arg0, arg1_func_84344CE8* arg1);
void BattleAnim_InitializeClampViceGripUse(void);
void BattleAnim_InitializeGuillotineUse(void);
void BattleAnim_InitializeBitingMoveUse(void);
void BattleAnim_InitializeBitingMoveHit(void);
void BattleAnim_InitializeGuillotineHit(void);
void Gfx_DrawParticleDualTileDiagonal4(Gfx* arg0, u8* arg1, UNUSED u16 arg2);
void Particle_CreateDisplayList_DualTileDiagonal4(s32 arg0, arg1_func_84344CE8* arg1);
void Gfx_DrawParticleDualTileAntiDiagonal1(Gfx* arg0, u8* arg1, UNUSED u16 arg2);
void Particle_CreateDisplayList_DualTileAntiDiagonal1(s32 arg0, arg1_func_84344CE8* arg1);
void BattleAnim_InitializeSwordsDanceUse(void);
f32 BattleAnim_GetSpeciesScaleAdjust(Particle* arg0);
void BattleAnim_CallbackStartEffect69ModelPhaseWait(Particle* arg0);
void BattleAnim_StartEffect69ModelPhaseWaitAndFallingParticles(void);
void BattleAnim_ApplySpeciesXZOffsetVariantB(Particle* arg0);
void BattleAnim_InitializeDigUse(void);
void BattleAnim_InitializeDigHit(void);
void BattleAnim_ApplySpeciesYOffsetVariantA(Particle* arg0);
void BattleAnim_InitializeKinesisUse(void);
void BattleAnim_InitializeBoneClubUse(void);
void BattleAnim_ApplySpeciesYOffsetVariantB(Particle* arg0);
void BattleAnim_InitializeBonemerangUse(void);
void BattleAnim_CallbackStartEffect17ModelAnimFinish_InitParticle(Particle* arg0);
void BattleAnim_CallbackStartEffect17ModelAnimFinish(Particle* arg0);
void BattleAnim_SetupSlimePuddle(Particle* arg0);
void BattleAnim_RunSlimePuddle(Particle* arg0);
void Gfx_DrawParticlePaletteBased(Gfx* arg0, u8* arg1, UNUSED u16 arg2);
void Gfx_ParticlePaletteCallback(s32 arg0, arg1_func_84344CE8* arg1);
void Gfx_ParticlePaletteDL(Gfx* arg0, UNUSED arg1_func_8434E21C* arg1, UNUSED u16 arg2);
void Gfx_ParticlePaletteColorOnlyCallback(s32 arg0, arg1_func_84344CE8* arg1);
s16 BattleAnim_GetSlimeVariantIndex(void);
s16 BattleAnim_GetMoveKeyedTintRampParam(void);
s16 BattleAnim_GetMoveKeyedTrailVariantParam(void);
void BattleAnim_InitializeSlimeMove(void);
void ParticleEffect_PaletteMultiBurst(void);
f32 BattleAnim_GetSpeciesScaleAndApplyYOffset(Particle* arg0);
void Gfx_ParticleTextureTileCallback(Gfx* arg0, arg1_func_8434E21C* arg1, UNUSED u16 arg2);
void Gfx_ParticleTextureTileAllocCallback(s32 arg0, arg1_func_84344CE8* arg1);
void Gfx_ParticleRedPrimCallback(Gfx* arg0, UNUSED arg1_func_8434E21C* arg1, UNUSED u16 arg2);
void Gfx_ParticleRedPrimAllocCallback(s32 arg0, arg1_func_84344CE8* arg1);
void BattleAnim_InitializeGlareLeerUse(void);
void BattleAnim_InitializeHyperBeamUseSequence(void);
void BattleAnim_InitializeEarthquakeFissureUse(void);
void BattleAnim_InitializeEarthquakeFissureHit(void);
void BattleAnim_InitializeEarthquakeHit(void);
void BattleAnim_InitializeDefenseCurlUse(void);
void BattleAnim_InitializeSharpenUse(void);
void Gfx_PokemonPortraitCallback(Gfx* arg0, u8 arg1, UNUSED u16 arg2);
void Gfx_PokemonPortraitAllocCallback(s32 arg0, arg1_func_84344CE8* arg1);
void BattleAnim_InitializeSpeedLines(void);
void BattleAnim_InitializeSkullBashUse(void);
void BattleAnim_InitializeWithdrawUse(void);
void BattleAnim_InitializeLickUse(void);
f32 BattleAnim_GetSpeciesScaleAndOffset(Particle* arg0);
void BattleAnim_InitializeLovelyKissUse(void);
void BattleAnim_InitializeLovelyKissHit(void);
void BattleAnim_ApplySpeciesYOffsetVariantC(Particle* arg0);
void BattleAnim_SetupPsychedelicOverlay(Particle* arg0);
void BattleAnim_RunPsychedelicOverlay(Particle* arg0);
void Gfx_PaletteParticleDL(Gfx* arg0, u8* arg1, UNUSED u16 arg2);
void Gfx_PaletteParticleDLCallback(s32 arg0, arg1_func_84344CE8* arg1);
void BattleAnim_InitializeDreamEaterUse(void);
void BattleAnim_InitializeDreamEaterHit(void);
void BattleAnim_CreateDrainingMoveOverlay(void);
void BattleAnim_InitializeMeditateUse(void);
void BattleAnim_InitializeNightShadeUse(void);
void BattleAnim_InitializeNightShadeHit(void);
void BattleAnim_SetupConfuseRayUseSpotlight(Particle* arg0);
void BattleAnim_RunConfuseRayUseSpotlight(Particle* arg0);
void BattleAnim_SetupConfuseRayHitSpotlight(Particle* arg0);
void BattleAnim_RunConfuseRayHitSpotlight(Particle* arg0);
void Gfx_WaterAnimPrimEnvDL(Gfx* arg0, u8 arg1, UNUSED u16 arg2);
void Gfx_WaterAnimPrimEnvDLCallback(s32 arg0, arg1_func_84344CE8* arg1);
void ParticleEffect_WaterAnimBurst(void);
void ParticleEffect_WaterAnimBurstTintFadeOnly(void);
void BattleAnim_InitializeConfuseRayUse(void);
void BattleAnim_InitializeConfuseRayHit(void);
void Gfx_WaterAnimDL(Gfx* arg0, u8* arg1, UNUSED u16 arg2);
void Gfx_WaterAnimDLCallback(s32 arg0, arg1_func_84344CE8* arg1);
f32 BattleAnim_GetSpeciesScaleAdjustSmall(Particle* arg0);
void BattleAnim_InitializeWaterfallUse(void);
void BattleAnim_InitializeWaterfallHit(void);
void BattleAnim_InitializeSurfUse(void);
void BattleAnim_InitializeSurfHit(void);
void BattleAnim_InitializeCrabhammerUse(void);
void BattleAnim_SetupBarrageEffect(Particle* arg0);
void BattleAnim_RunBarrageEffect(Particle* arg0);
void BattleAnim_SetupBarrageUseEffect1(Particle* arg0);
void BattleAnim_RunBarrageUseEffect1(Particle* arg0);
void BattleAnim_InitializeBarrageUse(void);
void BattleAnim_SetupBarrageHitEffect(Particle* arg0);
void BattleAnim_RunBarrageHitEffect(Particle* arg0);
void BattleAnim_InitializeBarrageHit(void);
void BattleAnim_SetupPsywaveEffect(Particle* arg0);
void BattleAnim_RunPsywaveEffect(Particle* arg0);
void Gfx_SimpleNodeDL(Gfx* arg0, arg1_func_8434E21C* arg1, UNUSED u16 arg2);
void Gfx_SimpleNodeDLCallback(s32 arg0, arg1_func_84344CE8* arg1);
void BattleAnim_InitializePsywaveUse(void);
void BattleAnim_InitializePsywaveHit(void);
void BattleAnim_RunConfusionPsychicHitEffect(Particle* arg0);
void BattleAnim_InitializePsychicUse(void);
void BattleAnim_InitializeConfusionUse(void);
void BattleAnim_InitializePsychicHit(void);
void BattleAnim_InitializeConfusionHit(void);
f32 BattleAnim_GetSpeciesScaleAdjustTiny(Particle* arg0);
void BattleAnim_InitializeFocusEnergyUse(void);
void BattleAnim_InitializeBideRageUseSequence(void);
void BattleAnim_InitializeSkyAttackUse1(void);
void BattleAnim_InitializeSkyAttackUse2(void);
void BattleAnim_InitializeSkyAttackHit(void);
void ParticleEffect_DrawQuad(Gfx* arg0, arg1_func_8434E21C* arg1, UNUSED u16 arg2);
void ParticleEffect_AllocateDisplayList(s32 arg0, arg1_func_84344CE8* arg1);
void BattleAnim_InitializeThunderHitSequence(void);
void BattleAnim_InitializeSolarBeamUse2(void);
void BattleAnim_CallbackEndEffect76DefaultDescriptorModelFade(Particle* arg0);
void BattleAnim_InitializeLargePhysicalHitSpark(void);
void BattleAnim_InitializeDrillPeckUse(void);
void BattleAnim_InitializeDrillPeckHit(void);
void BattleAnim_InitializeHornDrillUse(void);
void BattleAnim_InitializeHornDrillHit(void);
void BattleAnim_RunWeakSpear(Particle* arg0);
void BattleAnim_InitializeFuryAttackPeckUse(void);
void BattleAnim_InitializeHornAttackUse(void);
void BattleAnim_RunVerticalLandingHitSpark(Particle* arg0);
void BattleAnim_InitializeVerticalLandingHitSpark(void);
void BattleAnim_InitializeSonicBoomUse(void);
void BattleAnim_CallbackStartEffects127And128Descriptor39ScaleFade_InitParticle(Particle* arg0);
void BattleAnim_CallbackStartEffects127And128Descriptor39ScaleFade(Particle* arg0);
void BattleAnim_StartEffect127Descriptor39ScaleFade(void);
void BattleAnim_StartEffect128Descriptor39ScaleFade(void);
void ParticleEffect_CreateTypeX(void);
void ParticleEffect_CreateSmoke(void);
void ParticleEffect_CreateSmokeHighPriority(void);
void BattleAnim_CallbackStartEffect132Descriptor39DelayedRiseFade_InitParticle(Particle* arg0);
void BattleAnim_CallbackStartEffect132Descriptor39DelayedRiseFade(Particle* arg0);
void ParticleEffect_CreateDual(void);
//@src/fragments/battle_engine/battle_engine_shared_move_effects.c
void BattleAnim_SetupSlashingMoveUse(Particle* arg0);
void BattleAnim_RunSlashingMoveUse(Particle* arg0);
void BattleAnim_SetupRazorWindHitEffect(Particle* arg0);
void BattleAnim_RunRazorWindHitEffect(Particle* arg0);
void BattleAnim_SetupPetalDanceRazorLeafHitEffect(Particle* arg0);
void BattleAnim_RunPetalDanceRazorLeafHitEffect(Particle* arg0);
void BattleAnim_SetupSlashingMoveFullScreenFlash(Particle* arg0);
void BattleAnim_RunSlashingMoveFullScreenFlash(Particle* arg0);
void BattleAnim_SetupSlashMoveWhiteFlashOverlay(Particle* arg0);
void BattleAnim_RunSlashMoveWhiteFlashOverlay(Particle* arg0);
void BattleAnim_SetupSlashingMoveHitSpark(Particle* arg0);
void BattleAnim_RunSlashingMoveHitSpark(Particle* arg0);
void BattleAnim_SetupFurySwipesHitEffect(Particle* arg0);
void BattleAnim_RunFurySwipesHitEffect(Particle* arg0);
void BattleAnim_CallbackEndEffects39And41And45And46And83Descriptor3DMetadataBurstChild_InitParticle(Particle* arg0);
void BattleAnim_CallbackEndEffects39And41And45And46And83Descriptor3DMetadataBurstChild(Particle* arg0);
void BattleAnim_SetupSmallSand(Particle* arg0);
void BattleAnim_RunSmallSand(Particle* arg0);
void BattleAnim_SetupLargeSand(Particle* arg0);
void BattleAnim_RunLargeSand(Particle* arg0);
void BattleAnim_SetupSandAttackUseEffect(Particle* arg0);
void BattleAnim_RunSandAttackUseEffect(Particle* arg0);
void BattleAnim_SetupUnusedRiseImpulseVariant(Particle* arg0);
void BattleAnim_CallbackUnusedRiseImpulseVariant(Particle* arg0);
void BattleAnim_SetupSandAttackHitEffect(Particle* arg0);
void BattleAnim_RunSandAttackHitEffect(Particle* arg0);
void BattleAnim_CallbackSharedDescriptors3EAnd3FAnimatedRiseFade_InitParticle(Particle* arg0);
void BattleAnim_CallbackSharedDescriptors3EAnd3FAnimatedRiseFade(Particle* arg0);
void BattleAnim_SetupIceBeamIcePunchEffect(Particle* arg0);
void BattleAnim_RunIceBeamIcePunchEffect(Particle* arg0);
void BattleAnim_CallbackOrphanDescriptor34ColorFadeDetach_InitParticle(Particle* arg0);
void BattleAnim_CallbackOrphanDescriptor34ColorFadeDetach(Particle* arg0);
void BattleAnim_SetupGroundedCircularHitSpark(Particle* arg0);
void BattleAnim_RunGroundedCircularHitSpark(Particle* arg0);
void BattleAnim_SetupUnusedGlitterVariant(Particle* arg0);
void BattleAnim_CallbackUnusedGlitterVariant(Particle* arg0);
void BattleAnim_SetupPointyMoveHitSparkChild(Particle* arg0);
void BattleAnim_RunPointyMoveHitSparkChild(Particle* arg0);
void BattleAnim_SetupPhysicalHitPurpleSmoke(Particle* arg0);
void BattleAnim_RunPhysicalHitPurpleSmoke(Particle* arg0);
void BattleAnim_SetupSwiftHitEffect1(Particle* arg0);
void BattleAnim_RunSwiftHitEffect1(Particle* arg0);
void BattleAnim_SetupSwiftHitEffect2(Particle* arg0);
void BattleAnim_RunSwiftHitEffect2(Particle* arg0);
void BattleAnim_SetupSparkleUse(Particle* arg0);
void BattleAnim_RunSparkleHit(Particle* arg0);
void BattleAnim_SetupBlizzardHitEffect(Particle* arg0);
void BattleAnim_RunBlizzardHitEffect(Particle* arg0);
void BattleAnim_RunIceBeamHitEffect(Particle* arg0);
void BattleAnim_SetupThunderPunchHitEffect(Particle* arg0);
void BattleAnim_RunThunderPunchHitEffect(Particle* arg0);
void BattleAnim_SetupDefaultHitSpark(Particle* arg0);
void BattleAnim_RunDefaultHitSpark(Particle* arg0);
void BattleAnim_SetupStrongPurpleHitSmoke(Particle* arg0);
void BattleAnim_RunStrongPurpleHitSmoke(Particle* arg0);
void BattleAnim_SetupPointyMoveHitSpark(Particle* arg0);
s32 BattleAnim_RunPointyMoveHitSpark(Particle* arg0);
void BattleAnim_SetupFirePunchUseEffect1(Particle* arg0);
void BattleAnim_RunFirePunchUseEffect1(Particle* arg0);
void BattleAnim_SetupFirePunchUseEffect2(Particle* arg0);
void BattleAnim_RunFirePunchUseEffect2(Particle* arg0);
void BattleAnim_SetupThunderPunchUseEffect(Particle* arg0);
void BattleAnim_RunThunderPunchUseEffect(Particle* arg0);
void BattleAnim_RecolorParticleByQueuedMoveId(Particle* arg0);
void BattleAnim_CallbackStartEffects28And53And65And76Descriptor47AnimatedScaleMotion_InitParticle(Particle* arg0);
void BattleAnim_CallbackStartEffects28And53And65And76Descriptor47AnimatedScaleMotion(Particle* arg0);
void BattleAnim_SetupAcidArmorUseEffect1(Particle* arg0);
void BattleAnim_RunAcidArmorUseEffect1(Particle* arg0);
void BattleAnim_SetupAcidArmorUseEffect2(Particle* arg0);
void BattleAnim_RunAcidArmorUseEffect2(Particle* arg0);
void BattleAnim_InitSharedVerticalFadeParticle_ApplySpeciesOffset(Particle* arg0);
void BattleAnim_InitSharedVerticalFadeParticle(Particle* arg0);
void BattleAnim_RunSharedAnimatedVerticalFade(Particle* arg0);
void BattleAnim_ApplySpeciesKeyedSpawnOffset(Particle* arg0);
void BattleAnim_RunSplashOfWater(Particle* arg0);
void BattleAnim_RunDigHitDust(Particle* arg0);
void BattleAnim_RunDigHitRocks(Particle* arg0);
void BattleAnim_SetupBarrageUseEffect2(Particle* arg0);
void BattleAnim_RunBarrageUseEffect2(Particle* arg0);
void BattleAnim_CallbackDescriptor50BurstChild_InitParticle(Particle* arg0);
void BattleAnim_CallbackDescriptor50BurstChild(Particle* arg0);
void BattleAnim_CallbackDescriptor50BurstEmitter_InitParticle(Particle* arg0);
void BattleAnim_CallbackDescriptor50BurstEmitter(Particle* arg0);
void func_843553B4(void);
void BattleAnim_CallbackUnusedFallingTrailVariant(Particle* arg0);
void BattleAnim_SetupSolarBeamUseEffect(Particle* arg0);
void BattleAnim_RunSolarBeamUseEffect(Particle* arg0);
void BattleAnim_SetupQiChargeChild(Particle* arg0);
void BattleAnim_RunQiChargeChild(Particle* arg0);
void BattleAnim_SetupQiCharge(Particle* arg0);
void BattleAnim_RunQiCharge(Particle* arg0);
void BattleAnim_CallbackDescriptor26BurstChild_InitParticle(Particle* arg0);
void BattleAnim_CallbackDescriptor26BurstChild(Particle* arg0);
void BattleAnim_CallbackDescriptor26BurstEmitter_InitParticle(Particle* arg0);
void BattleAnim_CallbackDescriptor26BurstEmitter(Particle* arg0);
void BattleAnim_SetupUnusedGrowingSparkVariant(Particle* arg0);
void BattleAnim_CallbackUnusedGrowingSparkVariant(Particle* arg0);
void BattleAnim_RunStrongContactMoveHitSpark(void);
void BattleAnim_InitializeUniqueSharedHitSpark(void);
void BattleAnim_InitializeLowKickHit(void);
void BattleAnim_InitializeDefaultLimbForMove(void);
void BattleAnim_InitializeSpecialLimbForMove(void);
void BattleAnim_InitializePointyMoveHitSparks(void);
void BattleAnim_InitializePoisonStingTwineedleHit(void);
void BattleAnim_InitializeFuryAttackGustHit(void);
void BattleAnim_InitializeIcePunchUse(void);
void BattleAnim_InitializeIcePunchHit(void);
void BattleAnim_InitializeThunderPunchHit(void);
void BattleAnim_OrphanDescriptor34Emitter(void);
void BattleAnim_InitializeSubmissionUse(void);
void BattleAnim_InitializeStompHit(void);
void BattleAnim_InitializeGroundedCircularHitSpark(void);
void BattleAnim_InitializeSubmissionHit(void);
void BattleAnim_InitializeBitingMoveEndEffect(void);
void BattleAnim_InitializeSwiftUse(void);
void BattleAnim_InitializeSwiftHit(void);
void BattleAnim_StartEffect137Descriptor3FRisingFade(void);
void ParticleEffect_TextureNodeSetup(void);
void BattleAnim_InitializeCutSlashScratchEffect(void);
void BattleAnim_InitializeSlashMoveWhiteFlashOverlay(void);
void BattleAnim_InitializeScratchSlashUse(void);
void BattleAnim_InitializeScratchSlashHit(void);
void BattleAnim_InitializeFurySwipesUse1(void);
void BattleAnim_InitializeFurySwipesUse2(void);
void BattleAnim_InitializeFurySwipesHit(void);
void BattleAnim_InitializeVineWhipUse(void);
void BattleAnim_InitializeVineWhipHit(void);
void BattleAnim_InitializeFirePunchUse(void);
void BattleAnim_InitializeRazorWindHit(void);
void BattleAnim_InitializeRazorLeafUse(void);
void BattleAnim_InitializeRazorLeafHit(void);
void BattleAnim_InitializePetalDanceUse(void);
void BattleAnim_InitializePetalDanceHitSequence(void);
void BattleAnim_InitializePowderSporeGasMoveHit(void);
void BattleAnim_InitializeSandAttackUse(void);
void BattleAnim_InitializeSandAttackHit(void);
void BattleAnim_InitializeAcidArmorUse(void);
void BattleAnim_InitializeFlashUse(void);
void BattleAnim_InitializeFlashHit(void);
void BattleAnim_InitializeBideRageUse(void);
void BattleAnim_InitializeStruggleThrashUse(void);
void BattleAnim_InitializeDoubleEdgeTakeDownUse(void);
void BattleAnim_InitializeBodySlamUse(void);
void BattleAnim_InitializeEarthquakeFissureHitSequence(void);
void BattleAnim_EndEffectUnusedVerticalFadeAndDelayedFrameSequence(void);
void BattleAnim_InitializeDefenseCurlSharpenUseSequence(void);
void BattleAnim_InitializeSubstituteTransformUse(void);
void BattleAnim_InitializeQiCharge(s32 arg0);
void BattleAnim_InitializeMimicConversionUse(void);
void BattleAnim_InitializeSolarBeamUseSequence(void);
void BattleAnim_InitializeGrowthUse(void);
void BattleAnim_InitializeRecoverUse(void);
void BattleAnim_InitializeHardenUse(void);

// 3438B0
//@src/fragments/battle_engine/battle_engine_effect_render_pipeline.c
void BattleAnim_RenderNop1(void);
void BattleAnim_RenderNop2(void);
void BattleAnim_DrawFullScreenColorOverlay(u8, u8, u8, u8);
void BattleAnim_RenderEffectsFrame(CameraNode*);

//@src/fragments/battle_engine/battle_engine_mesh_effect_callbacks.c
void BattleAnim_NoopUpdateHook(void);
void func_843592E8(void);
void func_843592F0(void);
void func_843592F8(void);
void func_84359300(void);
void func_84359308(void);
void func_84359310(void);
void func_84359318(void);
void func_84359320(void);
void func_84359328(void);
void func_84359330(UNUSED s32 arg0);
void func_84359338(UNUSED s32 arg0);
void func_84359340(void);
void BattleAnim_DrawStoredColorOverlay(void);
void BattleAnim_ReinitAuxiliaryEffects(void);
void BattleAnim_DefaultStartEffect_Init(void);
void BattleAnim_DefaultEndEffect_Init(void);
s32 BattleAnim_DefaultStartEffect_CompleteImmediately(void);
s32 BattleAnim_DefaultEndEffect_CompleteImmediately(void);
void BattleAnim_DefaultStartEffect_Draw(void);
void BattleAnim_DefaultEndEffect_Draw(void);
void Battle_GetTertiaryOwnerBonePositionUnused(Vec3f* arg0);
void BattleAnim_EffectSecondaryOwnerAnchorPosition(Vec3f* arg0);
void BattleAnim_EffectTertiaryOwnerBonePosition(Vec3f* arg0);
void BattleAnim_EffectSecondaryOwnerBonePosition(Vec3f* arg0);
f32 BattleAnim_EffectTertiaryOwnerModelScale(void);
f32 BattleAnim_EffectSecondaryOwnerModelScale(void);
void BattleAnim_CalcMoveAimingVectors(f32* arg0, f32* arg1, f32* arg2, f32* arg3, f32* arg4, f32* arg5);
void BattleAnim_CalcSwiftAimVectors(f32* arg0, f32* arg1, f32* arg2, f32* arg3, f32* arg4, f32* arg5);
void BattleAnim_ResetSharedEffectMatrix(void);
void BattleAnim_SetupMistUseRibbonMesh(void);
s32 BattleAnim_RunMistUseRibbonMesh(void);
void BattleAnim_DrawMistUseRibbonMesh(void);
void BattleAnim_EndEffect1RibbonMesh_Init(void);
s32 BattleAnim_EndEffect1RibbonMesh_Update(void);
void BattleAnim_EndEffect1RibbonMesh_Draw(void);
void BattleAnim_EndEffect19RibbonMesh_Init(void);
s32 BattleAnim_EndEffect19RibbonMesh_Update(void);
void BattleAnim_EndEffect19RibbonMesh_Draw(void);
void BattleAnim_SetupHazeUseRibbonMesh(void);
s32 BattleAnim_RunHazeUseRibbonMesh(void);
void BattleAnim_DrawHazeUseRibbonMesh(void);
void BattleAnim_SetupSurfUseTerrainGrid(void);
s32 BattleAnim_RunSurfUseTerrainGrid(void);
void BattleAnim_DrawSurfUseTerrainGrid(void);
void BattleAnim_EndEffect16_NoOpInit(void);
s32 BattleAnim_SurfCompleteImmediately(void);
void BattleAnim_EndEffect16_NoOpDraw(void);
void BattleAnim_StartTrappingLassoMove_EmitVariantD(void);
void BattleAnim_StartTrappingLassoMove_Init(void);
s32 BattleAnim_StartTrappingLassoMove_Update(void);
void BattleAnim_StartTrappingLassoMove_Draw(void);
void BattleAnim_EndEffect9Radial20_Init(void);
s32 BattleAnim_EndEffect9Radial20_Update(void);
void BattleAnim_EndEffect9Radial20_Draw(void);
void BattleAnim_EmitSwiftUseRibbonSheet(void);
void BattleAnim_SetupSwiftUseRibbonSheet(void);
s32 BattleAnim_RunSwiftUseRibbonSheet(void);
void BattleAnim_DrawSwiftUseRibbonSheet(void);
void BattleAnim_EmitStringShotUseRadial(void);
void BattleAnim_SetupStringShotUseRadial(void);
s32 BattleAnim_RunStringShotUseRadial(void);
void BattleAnim_DrawStringShotUseRadial(void);
void BattleAnim_SetupStringShotHitRadial(void);
s32 BattleAnim_RunStringShotHitRadial(void);
void BattleAnim_DrawStringShotHitRadial(void);
void BattleAnim_SetupDisableHitRadial(void);
s32 BattleAnim_RunDisableHitRadial(void);
void BattleAnim_DrawDisableHitRadial(void);
void BattleAnim_EmitDisableUseRadial(void);
void BattleAnim_SetupDisableUseRadial(void);
s32 BattleAnim_RunDisableUseRadial(void);
void BattleAnim_DrawDisableUseRadial(void);
void BattleAnim_SetupRazorLeafUseTrail(void);
s32 BattleAnim_RunRazorLeafUseTrail(void);
void BattleAnim_DrawRazorLeafUseTrail(void);
void BattleAnim_SetupPetalDanceUseTrail(void);
s32 BattleAnim_RunPetalDanceUseTrail(void);
void BattleAnim_DrawPetalDanceUseTrail(void);
void BattleAnim_SetupHypnosisScreechUseTrail(void);
s32 BattleAnim_RunHypnosisScreechUseTrail(void);
void BattleAnim_DrawHypnosisScreechUseTrail(void);
void BattleAnim_SetupSingUseTrail(void);
s32 BattleAnim_RunSingUseTrail(void);
void BattleAnim_DrawSingUseTrail(void);
void BattleAnim_SetupKinesisSupersonicUseTrail(void);
s32 BattleAnim_RunKinesisSupersonicUseTrail(void);
void BattleAnim_DrawKinesisSupersonicUseTrail(void);
void BattleAnim_SetupSonicBoomUseFourStream(void);
s32 BattleAnim_RunSonicBoomUseFourStream(void);
void BattleAnim_DrawSonicBoomUseFourStream(void);
void BattleAnim_SetupSpikeCannonUseFourStream(void);
s32 BattleAnim_RunSpikeCannonUseFourStream(void);
void BattleAnim_DrawSpikeCannonUseFourStream(void);
void BattleAnim_StingerMoves_Init(void);
s32 BattleAnim_StingerMoves_Update(void);
void BattleAnim_StingerMoves_Draw(void);
void BattleAnim_EmitTriAttackUseRadial(void);
void BattleAnim_SetupTriAttackUseRadial(void);
s32 BattleAnim_RunTriAttackUseRadial(void);
void BattleAnim_DrawTriAttackUseRadial(void);
void BattleAnim_SeedAuroraBeamUseDoubleTextureTrail(void);
void BattleAnim_SetupAuroraBeamUseDoubleTextureTrail(void);
s32 BattleAnim_RunAuroraBeamUseDoubleTextureTrail(void);
void BattleAnim_DrawAuroraBeamUseDoubleTextureTrail(void);
void BattleAnim_SeedIceBeamUseSixSlotTrail(void);
void BattleAnim_SetupIceBeamUseSixSlotTrail(void);
s32 BattleAnim_RunIceBeamUseSixSlotTrail(void);
void BattleAnim_DrawIceBeamUseSixSlotTrail(void);
void BattleAnim_SeedPsybeamUseDoubleTextureTrail(void);
void BattleAnim_SetupPsybeamUseDoubleTextureTrail(void);
s32 BattleAnim_RunPsybeamUseDoubleTextureTrail(void);
void BattleAnim_DrawPsybeamUseDoubleTextureTrail(void);
void BattleAnim_StartEffect77DoubleTextureTrail_Seed(void);
void BattleAnim_StartEffect77DoubleTextureTrail_Init(void);
s32 BattleAnim_StartEffect77DoubleTextureTrail_Update(void);
void BattleAnim_StartEffect77DoubleTextureTrail_Draw(void);
void BattleAnim_SeedSolarBeamUseDoubleTextureTrail(void);
void BattleAnim_SetupSolarBeamUseDoubleTextureTrail(void);
s32 BattleAnim_RunSolarBeamUseDoubleTextureTrail(void);
void BattleAnim_DrawSolarBeamUseDoubleTextureTrail(void);
void BattleAnim_SeedHyperBeamUseDoubleTextureTrail(void);
void BattleAnim_SeedHyperBeamUseSixSlotTrail(void);
void BattleAnim_SetupHyperBeamUseCombinedTrail(void);
s32 BattleAnim_RunHyperBeamUseCombinedTrail(void);
void BattleAnim_DrawHyperBeamUseCombinedTrail(void);

//@src/fragments/battle_engine/battle_engine_ribbon_mesh_and_surf_grid.c
void MistHaze_Init(void);
void MistHaze_Configure(RibbonMesh* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7,
                        s32 arg8, s32 arg9);
void MistHaze_Spawn(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, s32 arg5);
void MistHaze_UpdateEnvAlpha(RibbonMesh* arg0);
s32 MistHaze_Update(RibbonMesh* arg0);
s32 MistHaze_UpdatePool(void);
Gfx* MistHaze_Draw(Gfx* arg0, RibbonMesh* arg1);
Gfx* MistHaze_DrawPool(Gfx* arg0);
void SurfTerrainGrid_BuildCameraBounds(unk_D_843975E8* arg0, f32 arg1);
f32 SurfTerrainGrid_SampleHeightAndSlope(f32 arg0, f32 arg1, f32* arg2);
s32 SurfTerrainGrid_UpdateWaves(void);
void SurfTerrainGrid_Init(s32 arg0, s16 arg1, s16 arg2);
s32 SurfTerrainGrid_Update(void);
Gfx* SurfTerrainGrid_Draw(Gfx* arg0);

//@src/fragments/battle_engine/battle_engine_lasso.c
void LassoHit_Init(f32 arg0, f32 arg1, f32 arg2, f32 arg3, u8 arg4, u8 arg5, u8 arg6, u8 arg7, u8 arg8, u8 arg9);
f32 Strip400_GetTailFadeRatio(void);
s32 LassoHit_Update(void);
Gfx* LassoHit_Draw(Gfx* arg0);
void Radial20_ResetPool(void);
void TriAttackRadial_EmitVariantA(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, s16 arg6, u8 arg7,
                                  u8 arg8, u8 arg9, u8 argA, f32 argB, f32 argC, f32 argD, s16 argE);
void Radial20_VariantB(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, s16 arg6, u8 arg7, u8 arg8, u8 arg9,
                       u8 argA, f32 argB, f32 argC, f32 argD, s16 argE);
void StringShotRadial_EmitVariantC(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, s16 arg6, u8 arg7,
                                   u8 arg8, u8 arg9, u8 argA, u8 argB, u8 argC, u8 argD, f32 argE, f32 argF, f32 arg10,
                                   s16 arg11);
void LassoMoveDefaultRadial20(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, s16 arg6, u8 arg7, u8 arg8,
                              u8 arg9, u8 argA, u8 argB, u8 argC, u8 argD, f32 argE, f32 argF, f32 arg10, s16 arg11);
void Radial20_VariantE(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, s16 arg6, u8 arg7, u8 arg8, u8 arg9,
                       u8 argA, f32 argB, f32 argC, f32 argD, s16 argE);
void Radial20_RotateVertexOffset(f32 arg0, UNUSED Vec3f* arg1, Vec3f* arg2, f32 arg3, UNUSED f32 arg4, UNUSED f32 arg5,
                                 UNUSED f32 arg6);
void Radial20_UpdateNode(Radial20* arg0);
s32 Radial20_IsComplete(void);
Gfx* Radial20_Draw(Gfx* arg0);

//@src/fragments/battle_engine/battle_engine_hypnosis_screech.c
void HypnosisScreechTrail_Init(s16 arg0);
void HypnosisScreechTrail_Start(s32 arg0, s32 arg1);
void HypnosisScreechTrailNode_FillVertex(Trail40Node* arg0, Vtx* arg1);
s32 HypnosisScreechTrail_IsComplete(void);
void HypnosisScreechTrail_Reset(void);
Gfx* HypnosisScreechTrail_Draw(Gfx* arg0);

//@src/fragments/battle_engine/battle_engine_sing_ripple.c
void SingTrail_ApplyRippleAtPoint(s16 arg0, s16 arg1);
void SingTrail_InitPool(s16 arg0);
void SingTrail_UpdateNodes(s32 arg0, s32 arg1);
void SingTrail_WriteVertex(Trail40VariantNode* arg0, Vtx* arg1);
s32 SingTrail_AdvanceTimer(void);
void SingTrail_SetupCameraTransform(void);
Gfx* SingTrail_BuildGfx(Gfx* arg0);

//@src/fragments/battle_engine/battle_engine_3502D0.c
void Trail40GridAlt_Init(s16 arg0);
void Trail40GridAlt_Start(s32 arg0, s32 arg1);
void Trail40GridAlt_FillVertex(Trail40Node* arg0, Vtx* arg1);
s32 Trail40GridAlt_IsComplete(void);
void Trail40GridAlt_Reset(void);
Gfx* Trail40GridAlt_Draw(Gfx* arg0);

//@src/fragments/battle_engine/battle_engine_swift.c
void SwiftRibbonSheet_Init(void);
void SwiftRibbonSheet_Start(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, u8 arg6, u8 arg7, u8 arg8,
                            u8 arg9, f32 argA, s16 argB);
void SwiftRibbonSheet_Update(f32 arg0, Vec3f* arg1, f32 arg2);
void SwiftRibbonSheet_UpdateRecord(TexturedRibbonSheet* arg0);
s32 SwiftRibbonSheet_IsComplete(void);
Gfx* SwiftRibbonSheet_Draw(Gfx* arg0);

//@src/fragments/battle_engine/battle_engine_beam_renderer.c
Gfx* Gfx_DrawThickLine(Gfx* arg0, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3, f32 arg4, u8 arg5, u8 arg6, u8 arg7, u8 arg8);

//@src/fragments/battle_engine/battle_engine_352570.c
void FourStreamTrail_Init(void);
void FourStreamTrail_Start(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, u8 arg6, u8 arg7, u8 arg8,
                           u8 arg9, f32 argA, f32 argB, f32 argC, s16 argD);
void FourStreamTrail_Update(f32 arg0, Vec3f* arg1, f32 arg2, f32 arg3);
void FourStreamTrail_UpdateNodeChain(FourStreamTrail* arg0);
s32 FourStreamTrail_IsComplete(void);
Gfx* FourStreamTrail_Draw(Gfx* arg0);

//@src/fragments/battle_engine/battle_engine_spike_cannon.c
void SpikeCannonFourStream_Init(void);
void SpikeCannonFourStream_Start(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, u8 arg6, u8 arg7, u8 arg8,
                                 u8 arg9, f32 argA, f32 argB, f32 argC, s16 argD);
void SpikeCannonFourStream_Update(f32 arg0, Vec3f* arg1, f32 arg2);
void SpikeCannonFourStream_UpdateNodeChain(OwnerLinkedFourStream* arg0);
s32 SpikeCannonFourStream_IsComplete(void);
Gfx* SpikeCannonFourStream_Draw(Gfx* arg0);

//@src/fragments/battle_engine/battle_engine_3540B0.c
void OwnerAnchoredFourStream_Init(void);
void OwnerAnchoredFourStream_Start(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, u8 arg6, u8 arg7,
                                   u8 arg8, u8 arg9, f32 argA, f32 argB, f32 argC, s16 argD);
void OwnerAnchoredFourStream_Update(f32 arg0, Vec3f* arg1, f32 arg2);
void OwnerAnchoredFourStream_UpdateNodeChain(OwnerAnchoredFourStream* arg0);
s32 OwnerAnchoredFourStream_IsComplete(void);
Gfx* OwnerAnchoredFourStream_Draw(Gfx* arg0);

//@src/fragments/battle_engine/battle_engine_petal_dance.c
void PetalDanceTrail_Init(void);
void PetalDanceTrail_Start(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, u8 arg6, u8 arg7, u8 arg8,
                           u8 arg9, f32 argA, f32 argB, f32 argC, s16 argD);
void PetalDanceTrail_Update(f32 arg0, Vec3f* arg1, f32 arg2);
void PetalDanceTrail_AdvanceNodes(Trail50Effect* arg0);
s32 PetalDanceTrail_IsComplete(void);
Gfx* PetalDanceTrail_Draw(Gfx* arg0);

//@src/fragments/battle_engine/battle_engine_razor_leaf.c
void RazorLeafTrail_Init(void);
void RazorLeafTrail_Start(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, u8 arg6, u8 arg7, u8 arg8,
                          u8 arg9, f32 argA, f32 argB, f32 argC, s16 argD);
void RazorLeafTrail_Update(f32 arg0, Vec3f* arg1, f32 arg2);
void RazorLeafTrail_AdvanceNodes(Trail50Effect* arg0);
s32 RazorLeafTrail_IsComplete(void);
Gfx* RazorLeafTrail_Draw(Gfx* arg0);

//@src/fragments/battle_engine/battle_engine_356730.c
void DoubleTextureTrail_InitPool(void);
void DoubleTextureTrail_Spawn(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, f32 arg7, f32 arg8,
                              f32 arg9, f32 argA, s16 argB, s32 argC, s32 argD, s16 argE, s16 argF, s16 arg10,
                              s16 arg11, s16 arg12, s16 arg13, s16 arg14, s16 arg15, unk_D_843C2C00_024* arg16,
                              unk_D_843C2C00_024* arg17, u8 arg18, u8 arg19, u8 arg1A, u8 arg1B, u8 arg1C, u8 arg1D,
                              u8 arg1E, u8 arg1F, u8 arg20, u8 arg21, u8 arg22, u8 arg23, u8 arg24);
void DoubleTextureTrail_RotateVertexOffset(f32 arg0, Vec3f* arg1, f32 arg2);
void DoubleTextureTrail_Update(DoubleTextureTrail* arg0);
s32 DoubleTextureTrail_UpdateAll(void);
Gfx* WaterParticle_DrawPass(Gfx* arg0, DoubleTextureTrail* arg1, s16 arg2, s16 arg3);
Gfx* DoubleTextureTrail_LoadTextures(Gfx* arg0, DoubleTextureTrail* arg1);
Gfx* DoubleTextureTrail_DrawAll(Gfx* arg0);

//@src/fragments/battle_engine/battle_engine_hyper_beam_ice_beam_trails.c
void HyperBeamIceBeamTrail_ResetPool(void);
void HyperBeamIceBeamTrail_Spawn(s16 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, u8 arg7, u8 arg8,
                                 u8 arg9, u8 argA, u8 argB, u8 argC, u8 argD, u8 argE, u8 argF, f32 arg10, f32 arg11,
                                 f32 arg12, s16 arg13, s32 arg14, s32 arg15, s16 arg16, s16 arg17, s16 arg18, s16 arg19,
                                 s16 arg1A, s16 arg1B, s16 arg1C, s16 arg1D, unk_D_843C2C00_024* arg1E,
                                 unk_D_843C2C00_024* arg1F);
void HyperBeamIceBeamTrail_ComputeCameraRelativeOffset(s32 arg0, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3, f32 arg4,
                                                       f32 arg5, f32 arg6, f32 arg7);
void HyperBeamIceBeamTrail_ComputeRotatedOffset(f32 arg0, Vec3f* arg1, Vec3f* arg2, f32 arg3, f32 arg4, f32 arg5,
                                                f32 arg6);
void HyperBeamIceBeamTrail_UpdateSlot(unk_D_843C3508* arg0);
s32 HyperBeamIceBeamTrail_UpdateAll(void);
Gfx* HyperBeamIceBeamTrail_DrawSegments(Gfx* arg0, unk_D_843C3508* arg1);
Gfx* HyperBeamIceBeamTrail_LoadTextures(Gfx* arg0, unk_D_843C3508* arg1);
Gfx* HyperBeamIceBeamTrail_DrawAll(Gfx* arg0);
//@src/fragments/battle_engine/battle_engine_3591E0.c
void AuxiliaryEffect_Init(void);
s16 AuxiliaryEffect_Alloc(void);
void AuxiliaryEffect_Spawn(s16 arg0, f32 arg1, f32 arg2, f32 arg3, s16 arg4, f32 arg5);
void AuxiliaryEffect_Update(s16 arg0);
Gfx* AuxiliaryEffect_DrawOne(Gfx* arg0, AuxiliaryEffectNode* arg1, MtxF* arg2);
Gfx* AuxiliaryEffect_Draw(s16 arg0, Gfx* arg1);
void func_8436EED4(void);
void func_8436EEDC(void);
s32 AuxEffect_GetId(s32 arg0);

//@src/fragments/battle_engine/battle_engine_3597B0.c
void BattleScene_SetStripWipeState(s32 arg0, s32 arg1);
s32 BattleScene_GetStripWipeState(void);
void BattleScene_UpdateStripWipe(void);
Gfx* BattleScene_BuildStripWipeDisplayList(Gfx* arg0, s32 arg1);
void BattleScene_DrawStripWipe(void);

//@src/fragments/battle_engine/battle_engine_debug_strings.c
void func_8436F6C0(void);

//@src/fragments/battle_engine/battle_engine_mechanics.c
void func_8436F6D0(void);
u8 Battle_Random(void);
u8 Battle_RandomAccuracy(void);
void ShowDamageMessage(s32 arg0, s32 arg1);
void Battle_ResetTransientRuntimeState(s32 arg0, s32 arg1);
void Battle_ResetEntityState(ActivePokemon* active_pokemon);
void Battle_ApplyDamageRandomFactor(void);
u16 Battle_DetermineMoveOrder(ActivePokemon* arg0, ActivePokemon* arg1);
s32 IsByteInArray(u8 arg0, u8* arg1, s32 arg2);
void Battle_SetActiveMoveFromSlotIndex(void);
s32 Battle_CheckMoveSuccess(void);
void ComputePokemonStat(s32 arg0, BattleMon* arg1);
void ComputeAllPokemonStats(BattleMon* arg0);
void InitBattleParameters(void);
s32 Battle_CalcDamage(void);
void Battle_ApplyDamage(void);
void Battle_DecrementPP(s32 arg0);
void ApplyAccuracyStageMultipliers(void);
void MarkAttackFailed(void);
void HandleMoveEffectFlags(void);
void Battle_ApplyAnimStateEffects(ActivePokemon* active_pokemon);
void ApplyParalysisSpeedCut(ActivePokemon* active_pokemon);
void ApplyBurnAttackCut(ActivePokemon* active_pokemon);
void Battle_InitPokemonRuntimeState(ActivePokemon* active_pokemon);
s32 Battle_IsPartyMemberAlive(ActivePokemon* active_pokemon);
void func_84370E70(void);
void func_84370E78(void);
void Battle_ExecuteMoveAnimation(void);
void Battle_ApplyRageContinuation(void);
void Battle_UpdateBattleLog(void);
void Battle_PlayMoveSound(void);
void Battle_ApplyDamageAnimation(void);
void Battle_QueueMoveFailureAndApplyRecoil(void);
void Battle_IncrementMoveUsage(void);
s32 Battle_Effect_Metronome(void);
void Battle_ApplyPoisonOrBurnDamage(void);
void Battle_ApplyLeechSeedDamage(void);
s32 Battle_ApplyResidualEffects(void);
void func_843718DC(void);
s32 Battle_TryLoadFollowupMoveFromTarget(void);
void Battle_ApplyTypeEffectAndSTAB(void);
s32 Battle_Effect_Counter(void);
s32 Battle_CheckCriticalHit(void);
s32 Battle_CheckMonCanAct(void);
void ProcessBattleTurn(void);
void Battle_HandleFaint(ActivePokemon* active_pokemon);
void Battle_CheckAllBroughtFainted_HP(ActivePokemon* active_pokemon);
void Battle_CheckAllBroughtFainted_Status(ActivePokemon* active_pokemon);
void Battle_CheckAllBroughtFainted_PP(ActivePokemon* active_pokemon);
void Battle_CheckAllBroughtFainted_Level(ActivePokemon* active_pokemon);
s32 Battle_CheckSideDefeatAfterResidualStatus(ActivePokemon* active_pokemon);
s32 Battle_CheckFaintAfterMove(void);
void Battle_ResolveMonAction(void);
void BeginBattleForEntity(ActivePokemon* active_pokemon);

//@src/fragments/battle_engine/battle_engine_move_mechanics.c
u16 BattleEffect_ApplyStatStageMultiplier(u16* stat_ptr, u16 stat_stage_delta, u8 stat_index) ;
void BattleEffect_Sleep(void);
void BattleEffect_Poison(void);
void BattleEffect_DrainHP(void);
void BattleEffect_Explosion(void);
void BattleEffect_SecondaryStatusChance(void);
void BattleEffect_ApplyStatBoost(void);
void BattleEffect_RaiseOwnStat(void);
void BattleEffect_LowerStat(void);
void BattleEffect_Bide(void);
void BattleEffect_Thrash(void);
void BattleEffect_WildBattleOnlyMoves(void);
void BattleEffect_MultiHit(void);
void BattleEffect_Flinch(void);
void BattleEffect_OHKO(void);
void BattleEffect_TwoTurnMove(void);
void BattleEffect_Trap(void);
void BattleEffect_Mist(void);
void BattleEffect_FocusEnergy(void);
void BattleEffect_Recoil(void);
void BattleEffect_ConfuseChance(void);
void BattleEffect_Confuse(void);
void BattleEffect_Paralyze(void);
void BattleEffect_Substitute(void);
void BattleEffect_Recharge(void);
void BattleEffect_Rage(void);
void BattleEffect_Mimic(void);
void BattleEffect_LeechSeed(void);
void BattleEffect_Splash(void);
void BattleEffect_Disable(void);
void BattleEffect_PayDay(void);
void BattleEffect_Conversion(void);
void BattleEffect_Haze(void);
void BattleEffect_Heal(void);
void BattleEffect_Transform(void);
void BattleEffect_ReflectLightScreen(void);
void BattleEffect_RunMoveEffect(void);
void BattleEffect_ExecuteMoveEffect(void);

//@src/fragments/battle_engine/battle_engine_com_decision_logic.c
void BattleAI_LoadConfiguration(s32 arg0, u32 arg1);
void func_843767DC(void);
s32 BattleAI_RandomBelowInclusive(s32 arg0);
void BattleAI_SeedRandomState(u8 arg0);
u8 BattleAI_EstimateCriticalHitChance(BattleMonRuntime* arg0, BattleMonRuntime* arg1, u8 arg2);
u16 BattleAI_ApplyMoveEffectDamageOverride(BattleMonRuntime* arg0, BattleMonRuntime* arg1);
void BattleAI_ClampDamageRatio(void);
void BattleAI_ApplyTypeEffectAndStab(BattleMonRuntime* arg0, BattleMonRuntime* arg1);
void BattleAI_PrepareDamageCalcState(BattleMonRuntime* arg0, BattleMonRuntime* arg1, u8 arg2);
void BattleAI_CalcDamage(BattleMonRuntime* arg0, BattleMonRuntime* arg1);
u16 BattleAI_PredictDamage(BattleMonRuntime* arg0, BattleMonRuntime* arg1, u8 arg2, u8 arg3);
void BattleAI_ResetTeamState(BattleAiMonState* arg0);
s32 BattleAI_ScaleSignedRatio(s32 arg0, s32 arg1);
void BattleAI_AppendTrainerTeamSpecies(TeamRoster* arg0);
s32 BattleAI_FindSpeciesIndex(s32 arg0);
s32 BattleAI_IsThresholdMet(u8 arg0, u8 arg1);
u16 BattleAI_ScaleStatHigh(BattleMonRuntime* arg0);
u16 BattleAI_ScaleStatLow(BattleMonRuntime* arg0);
f32 Battle_GetTypeEffectiveness(u8 arg0, BattleMonRuntime* arg1);
s32 BattleAI_AllCandidatesResistType(u8 arg0, BattleAiTeamState* arg1);
s32 BattleAI_IsTypeMatchupAllowed(u8 arg0, u8 arg1);
s32 BattleAI_HasCandidateStatusMask(BattleAiTeamState* arg0, u8 arg1);
u8 BattleAI_ClassifyMoveAvailability(BattleAiMonState* arg0, BattleAiMonState* arg1, u8 arg2);
void func_843779C0(void);
u8 BattleAI_FindHeuristicMove(BattleAiMonState* arg0, BattleAiMonState* arg1, u8 arg2, u8* arg3);
u8 BattleAI_SelectBestHeuristicCandidate(BattleAiMonState* arg0, BattleAiTeamState* arg1, u8* arg2, u8 arg3, u8* arg4);
s32 BattleAI_IsPriorityTableMove(u8 arg0, u8 arg1);
u8 BattleAI_ScoreCategorySetLowPP(BattleAiMonState* arg0, BattleAiMonState* arg1);
u8 BattleAI_ScoreCategorySetReady(BattleAiMonState* arg0, BattleAiMonState* arg1);
u8 BattleAI_HasExactlyOneUsableCandidate(BattleAiTeamState* arg0);
void BattleAI_InitializeOrderCandidates(TeamRoster* arg0, TeamRoster* arg1, TeamRoster* arg2, unk_D_843C5568* arg3,
                                        s32 arg4);
void BattleAI_UpdateRememberedMoveCandidate(BattleAiMonState* arg0, u8 arg1);
u16 BattleAI_ScoreMove(BattleMonRuntime* arg0, BattleMonRuntime* arg1, u8 arg2);
void BattleAI_PickBestMove(BattleMonRuntime* arg0, BattleMonRuntime* arg1, u16* arg2, u8* arg3, u8 arg4, u8 arg5);
s32 BattleAI_ScoreAllCandidateMoves(BattleAiMonState* arg0, BattleAiMonState* arg1, u8* arg2, u8 arg3);
s32 BattleAI_ScoreAllCandidateMovesRatioToMaxHp(BattleAiMonState* arg0, BattleAiMonState* arg1, u8* arg2, u8 arg3);
s32 BattleAI_ScoreAllCandidateMovesRatioToCurrentHp(BattleAiMonState* arg0, BattleAiMonState* arg1, u8* arg2, u8 arg3);
void func_8437921C(void);
u8 BattleAI_CalcStagedMovePower(BattleMonRuntime* arg0, BattleMonRuntime* arg1, u8 arg2, u8 arg3);
void BattleAI_ApplyInverseWeightScale(u8 arg0, u8* arg1);
void BattleAI_BuildMoveCandidateScores(BattleAiMonState* arg0, BattleAiMonState* arg1, s32 arg2, u8 arg3, u8 arg4,
                                       BattleAiScoredMove* arg5);
s32 BattleAI_HasScoredMoveAtValue(BattleAiScoredMove* arg0, u8 arg1, u8 arg2);
s32 BattleAI_IsCandidateScoreBelowPeers(BattleAiScoredMove* arg0, u8 arg1, u8 arg2, u8 arg3);
u8 BattleAI_HasOtherCandidateAtLeastScore(BattleAiScoredMove* arg0, u8 arg1, u8 arg2, u8 arg3);
void BattleAI_ApplyTypeResistance(BattleAiTeamState* arg0, BattleAiTeamState* arg1, u8 arg2, BattleAiScoredMove* arg3,
                                  u8 arg4);
s32 BattleAI_IsScoreBetter(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32 BattleAI_SelectBestScoredMove(BattleAiMonState* arg0, UNUSED BattleAiMonState* arg1, u8 arg2,
                                  BattleAiScoredMove* arg3);
s32 BattleAI_ChooseMoveForCandidate(void);
u16 BattleAI_ComputeDifficultyThreshold(u8 arg0, u8 arg1);
u8 BattleAI_GetPpWeight(u8 arg0);
void BattleAI_InitTeamContext(TeamRoster* arg0, TeamRoster* arg1, BattleAiTeamState* arg2, u8 arg3);
s32 BattleAI_IsSpeciesUnselected(BattleAiTeamState* arg0, u8* arg1, u8 arg2);
s32 BattleAI_IsSpeciesInList(BattleAiTeamState* arg0, u8* arg1, u8 arg2, u8 arg3);
s32 BattleAI_MeetsHpBudgetConstraint(BattleAiTeamState* arg0, u8* arg1, u8 arg2, s32 arg3);
s32 BattleAI_IsCandidateEligible(BattleAiTeamState* arg0, u8* arg1, u8 arg2, u8 arg3);
s32 BattleAI_GroupSharesTypeWeakness(BattleAiTeamState* arg0, u8* arg1, u8 arg2);
s32 BattleAI_ScoreAllTeamCandidates(BattleAiTeamState* arg0, BattleAiTeamState* arg1, u8* arg2, s32* arg3);
s32 BattleAI_InsertRankedCandidateGroup(BattleAiTeamState* arg0, s32* arg1, s32 arg2, unk_func_8437F85C_arg3* arg3,
                                        u8* arg4, u8 arg5, u8 arg6);
void BattleAI_SelectRandomFallbackOrder(BattleAiTeamState* arg0, u8* arg1, s32 arg2);
s32 BattleAI_SelectRandomUsableCandidate(BattleAiTeamState* arg0);
s32 BattleAI_SelectRandomUsableMoveSlot(BattleMonRuntime* arg0);
s32 BattleAI_SelectTeamOrder(BattleAiTeamState* arg0, u8* arg1, s32* arg2, u8 arg3);
s32 BattleAI_SelectLeadFromScoredGroup(BattleAiTeamState* arg0, BattleAiTeamState* arg1, u8* arg2, u8* arg3, s32* arg4);
s32 BattleAI_TryBuildScoredOrder(BattleAiTeamState* arg0, BattleAiTeamState* arg1, u8* arg2, u8* arg3, s32 arg4);
void BattleAI_UpdateTeamAdvantageBias(void);
u8 BattleAI_SelectBestSwitchCandidateIndex(BattleAiTeamState* arg0, BattleAiTeamState* arg1);
u8 BattleAI_SelectBestAttackTargetIndex(BattleAiMonState* arg0, s32* arg1, s32* arg2, BattleAiTeamState* arg3, u8 arg4,
                                        s32 arg5);
s32 BattleAI_ScoreSwitchUrgencyFromThreatMove(BattleAiMonState* arg0, BattleAiMonState* arg1, u8 arg2);
s32 BattleAI_ComputeAttackTargetScore(unk_func_8438220C* arg0);
s32 BattleAI_ScoreAttackOption(BattleAiMonState* arg0, BattleAiMonState* arg1, BattleAiTeamState* arg2, u8* arg3,
                               s32* arg4, s32* arg5, s32* arg6, s32* arg7, s32* arg8);
s32 BattleAI_ScoreAttackOptionDoubles(BattleAiMonState* arg0, BattleAiMonState* arg1, BattleAiTeamState* arg2, u8* arg3,
                                      unk_func_8438220C* arg4);
u8 BattleAI_ScoreAttackVsSwitch(BattleAiMonState* arg0, BattleAiMonState* arg1, BattleAiTeamState* arg2,
                                BattleAiTeamState* arg3, u8* arg4, unk_func_8438220C* arg5);
u16 BattleAI_ApplyStatStageModifier(u16* arg0, u16 arg1, u8 arg2);
void BattleAI_SimulateStatBoostGuess(BattleMonRuntime* arg0, BattleMonRuntime* arg1, u8 arg2, u8 arg3);
void BattleAI_BuildAllCandidateMoveScores(unk_func_8438220C* arg0);
void BattleAI_RefineAllCandidateMoveScores(unk_func_8438220C* arg0);
u8 BattleAI_DecideFinalAction(unk_func_8438220C* arg0);
s32 BattleAI_DecideAction(void);
s32 BattleAI_ResolveTiedActionChoice(s32 arg0);
s32 BattleAI_MatchesLastMoveCategory(u8 arg0, u8 arg1);
s32 BattleAI_IsDiscouragedRepeatCategory(u8 arg0, u8 arg1);
s32 BattleAI_IsBannedRandomMove(u8 arg0);
s32 BattleAI_IsRestrictedRandomMove(u8 arg0);
s32 BattleAI_IsMoveExcludedForSpecies(BattleMonRuntime* arg0, s32 arg1, u8 arg2, u8 arg3);
s32 BattleAI_IsMoveIneffective(BattleMonRuntime* arg0, BattleMonRuntime* arg1, u8 arg2);
void BattleAI_SyncTeamStateFromRuntime(void);
s32 func_843831A0(TeamRoster* arg0, TeamRoster* arg1, TeamRoster* arg2, s32 ai_difficulty, u8* arg4, s32 rule_category,
                  s32 arg6);
void BattleAI_RecordUsedMove(UNUSED s32 arg0, UNUSED s32 arg1, s32 arg2);
s32 BattleAI_ChooseAction(UNUSED s32 arg0);
void BattleAI_InitTrainerRecords(void);
s32 BattleAI_ResolveRandomMove(s32 arg0, s32 arg1);

#endif // _FRAGMENT62_H_
