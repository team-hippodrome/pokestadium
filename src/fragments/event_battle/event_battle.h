#ifndef _FRAGMENT38_H_
#define _FRAGMENT38_H_

#include "global.h"
#include "src/game_state.h"

typedef struct unk_D_82305A28 {
    /* 0x00 */ u8 mode;
    /* 0x01 */ u8 mayProceedToBattle;
    /* 0x02 */ s16 cursorPosition;
    /* 0x04 */ s16 trainerCardDisplayState;
    /* 0x06 */ s16 battleReadyState;
} EventBattleMenuState; // size = 0x8

typedef struct unk_D_82305A50 {
    /* 0x00 */ u8 visible;
    /* 0x01 */ u8 displayEligibility;
    /* 0x02 */ s16 slideInProgress;
    /* 0x04 */ s16 restingX;
    /* 0x06 */ s16 restingY;
    /* 0x08 */ s16 currentX;
    /* 0x0A */ s16 currentY;
    /* 0x0C */ u8* icon;
    /* 0x10 */ char* text;
    /* 0x14 */ s16 minLevel;
    /* 0x16 */ s16 maxLevel;
    /* 0x18 */ s16 description;
} EventBattleSelectionOption; // size = 0x1C

typedef struct DescriptionPanel {
    /* 0x00 */ u8 expanding;
    /* 0x01 */ u8 partiesNotEligible;
    /* 0x02 */ s16 openingProgress;
    /* 0x04 */ s16 x;
    /* 0x06 */ s16 y;
} DescriptionPanel; // size = 0x8

typedef struct unk_D_82305A40 {
    /* 0x00 */ u8 state;
    /* 0x00 */ u8 timeLimitsOn;
    /* 0x02 */ s16 battleTime;
    /* 0x04 */ s16 moveSelectionTime;
    /* 0x06 */ s16 openingProgress;
    /* 0x08 */ s16 x;
    /* 0x0A */ s16 y;
    /* 0x0C */ s16 cursorPosition;
} SettingsWindow; // size = 0x10

typedef struct unk_D_82305AF8 {
    /* 0x0000 */ u8 state;
    /* 0x0001 */ u8 port;
    /* 0x0002 */ u8 unk_0002;
    /* 0x0003 */ u8 levelCapCompliancePossible;
    /* 0x0004 */ s16 ruleset;
    /* 0x0006 */ s16 slideSpeed;
    /* 0x0008 */ s16 restingX;
    /* 0x000A */ s16 restingY;
    /* 0x000C */ s16 currentX;
    /* 0x000E */ s16 currentY;
    /* 0x0010 */ s16 levelCapWarningPhase;
    /* 0x0012 */ u16 trainerId;
    /* 0x0014 */ char trainerName[0xC];
    /* 0x0020 */ BattleMon pokemon[6];
    /* 0x0218 */ u8 pokemonIcons[6][3200];
} EventBattleTrainerParty; // size = 0x4D18

typedef struct EventBattleOptionLabel {
    /* 0x00 */ u8 animState; // 0 = idle, 1 = growing, 2 = shrinking
    /* 0x02 */ s16 openingProgress;   // grows/shrinks by 6 per update between 6 and 48
    /* 0x04 */ s16 x;
    /* 0x06 */ s16 y;        // vertical center of the 48-pixel-tall label
    /* 0x08 */ char* text;
} EventBattleCupOption; // size = 0x0C

typedef struct EventBattleModeEntry {
    /* 0x00 */ s16 left;
    /* 0x02 */ s16 top;
    /* 0x04 */ s16 right;
    /* 0x06 */ s16 bottom;
    /* 0x08 */ s16 unk_08;
} EventBattleModeEntry; // size = 0xA

typedef struct unk_D_82305978 {
    /* 0x00 */ s16 minLevel;
    /* 0x02 */ s16 maxLevel;
    /* 0x04 */ s16 levelCap;
    /* 0x06 */ s16 speciesLegalityFlag;
} EventBattleCupRules; // size = 0x8

u8 EventBattle_CheckTeamLegality(EventBattleTrainerParty* arg0, s16 arg1);

#endif // _FRAGMENT38_H_
