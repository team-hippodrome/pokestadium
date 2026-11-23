#ifndef _EVENT_BATTLE_H_
#define _EVENT_BATTLE_H_

#include "global.h"
#include "src/game_state.h"

typedef struct EventBattleMenuState {
    /* 0x00 */ u8 state;
    /* 0x01 */ u8 mayProceedToBattle; // Both parties pass the six per-slot checks; excludes the picked-level total.
    /* 0x02 */ s16 cursorPosition;
    /* 0x04 */ s16 actionCursor; // 0 = battle, 1 = rental rules, 2 = back
    /* 0x06 */ s16 nextGameState; // Returned to the game-state dispatcher.
} EventBattleMenuState; // size = 0x8

typedef struct EventBattleSelectionOption {
    /* 0x00 */ u8 slideState; // 0 = hidden, 1 = shown, 2 = entering, 3 = exiting
    /* 0x01 */ u8 bothPartiesEligible;
    /* 0x02 */ s16 slideOffsetSteps; // Horizontal distance from rest, in 32-pixel steps.
    /* 0x04 */ s16 restingX;
    /* 0x06 */ s16 restingY;
    /* 0x08 */ s16 currentX;
    /* 0x0A */ s16 currentY;
    /* 0x0C */ u8* icon;
    /* 0x10 */ char* text;
    /* 0x14 */ s16 minLevel;
    /* 0x16 */ s16 maxLevel;
    /* 0x18 */ s16 descriptionTextId;
} EventBattleSelectionOption; // size = 0x1C

typedef struct EventBattleDescriptionPanel {
    /* 0x00 */ u8 animationState; // 0 = idle, 1 = opening, 2 = closing
    /* 0x01 */ u8 bothPartiesEligible; // Used only by the party-eligibility banner.
    /* 0x02 */ s16 height;
    /* 0x04 */ s16 x;
    /* 0x06 */ s16 y;
} EventBattleDescriptionPanel; // size = 0x8

typedef struct EventBattleSettingsPanel {
    /* 0x00 */ u8 state; // 0 = hidden, 1 = interactive, 2 = opening, 3 = closing
    /* 0x01 */ u8 timeLimitsEnabled;
    /* 0x02 */ s16 battleLimitMinutes;
    /* 0x04 */ s16 orderLimitSeconds;
    /* 0x06 */ s16 height;
    /* 0x08 */ s16 x;
    /* 0x0A */ s16 y;
    /* 0x0C */ s16 cursorPosition;
} EventBattleSettingsPanel; // size = 0x0E

typedef struct EventBattleTrainerParty {
    /* 0x0000 */ u8 slideState; // 0 = hidden, 1 = shown, 2 = entering, 3 = exiting
    /* 0x0001 */ u8 controllerPort;
    /* 0x0002 */ u8 allPokemonEligible; // All six per-slot checks pass; excludes the picked-level total.
    /* 0x0003 */ u8 levelCapCompliancePossible; // Forced true when a per-slot check fails, suppressing the total-level warning.
    /* 0x0004 */ s16 ruleCategory;
    /* 0x0006 */ s16 slideOffsetSteps; // Horizontal distance from rest, in 64-pixel steps.
    /* 0x0008 */ s16 restingX;
    /* 0x000A */ s16 restingY;
    /* 0x000C */ s16 currentX;
    /* 0x000E */ s16 currentY;
    /* 0x0010 */ s16 levelCapWarningPhase;
    /* 0x0012 */ u16 trainerId;
    /* 0x0014 */ char trainerName[12];
    /* 0x0020 */ BattleMon pokemon[6];
    /* 0x0218 */ u8 pokemonIcons[6][3200];
} EventBattleTrainerParty; // size = 0x4D18

typedef struct EventBattleActionLabel {
    /* 0x00 */ u8 animationState; // 0 = idle, 1 = growing, 2 = shrinking
    /* 0x02 */ s16 height; // Grows/shrinks by six pixels per update between 6 and 48.
    /* 0x04 */ s16 x;
    /* 0x06 */ s16 y; // Top of the fully expanded 48-pixel-tall label.
    /* 0x08 */ char* text;
} EventBattleActionLabel; // size = 0x0C

typedef struct EventBattleSelectionEntry {
    /* 0x00 */ s16 x;
    /* 0x02 */ s16 y;
    /* 0x04 */ s16 width;
    /* 0x06 */ s16 height;
    /* 0x08 */ s16 ruleCategory; // Session rule category for cup entries; unused for action bounds.
} EventBattleSelectionEntry; // size = 0xA

typedef struct EventBattleCupRules {
    /* 0x00 */ s16 minLevel;
    /* 0x02 */ s16 maxLevel;
    /* 0x04 */ s16 levelCap;
    /* 0x06 */ s16 speciesEligibilityMask;
} EventBattleCupRules; // size = 0x8

u8 EventBattle_CheckPartyEligibility(EventBattleTrainerParty* trainer_card, s16 rule_category);

#endif // _EVENT_BATTLE_H_
