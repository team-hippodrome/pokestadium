#ifndef _FRAGMENT66_H_
#define _FRAGMENT66_H_

#include "global.h"
#include "src/game_state.h"

// Blink state for the "press A" prompt glyph (PikachuReward_DrawBlinkingStarIcon).
// Only the first word is accessed in this fragment; the purpose of the remaining
// 0x1FC bytes of the 0x200-byte BSS block is unknown.
typedef struct unk_D_832025C8 {
    /* 0x000 */ s32 blinkTimer; // 0..60 frame counter driving the glyph alpha
    /* 0x004 */ char unk04[0x1FC];
} unk_D_832025C8; // size = 0x200

// Location on a Game Boy cartridge of the Pikachu that may learn Surf, plus a
// working copy of its data. PikachuReward_FindPikachuOnCart fills the location from
// the picked Pikachu's BattleMon.unk_52/unk_53; PikachuReward_SaveToCartridge writes
// `pokemon` back to the same slot.
typedef struct RewardPikachuSource {
    /* 0x00 */ u8 controllerPort; // Transfer Pak port 0..3, passed to GbSave_LoadPort
    /* 0x01 */ u8 boxIndex;       // 0 = party (deck 0x20), 1..12 = PC box boxIndex - 1 (deck 0x21)
    /* 0x02 */ u8 deckSlot;       // raw entry index within that party/box
    /* 0x04 */ BattleMon pokemon; // copy read from the cartridge; Surf is written here
} RewardPikachuSource; // size = 0x58

s32 PikachuReward_SpinSunburstRays(s32 arg0, DisplayObject* arg1);
s32 PikachuReward_HasOnlyValidMoves(BattleMon* arg0);
s32 PikachuReward_MeetsCupRules(BattleMon* arg0, s16 arg1, s16 arg2, s16 arg3);
s32 PikachuReward_SearchDeckForPikachu(s16 arg0, s16 arg1, s16 arg2);
s32 PikachuReward_FindPikachuOnCart(void);
s32 PikachuReward_CountKnownMoves(void);
void PikachuReward_UpdateLeadPikachuAnimation(void);
void PikachuReward_UpdateFollowerVisibility(void);
s32 PikachuReward_RenderFrame(u8 arg0);
void PikachuReward_DrawBlinkingStarIcon(s16 arg0, s16 arg1);
void PikachuReward_DrawTextBoxPanel(s16 arg0, s16 arg1, s16 arg2, s16 arg3);
void PikachuReward_DrawSelectionCursor(s16 arg0, s16 arg1);
void PikachuReward_ShowTextForFrames(char* arg0, s32 arg1);
void PikachuReward_ShowTextWaitForA(char* arg0, s32 arg1);
void PikachuReward_ScrollTextIn(s8* arg0);
s32 PikachuReward_ConfirmYesNoPrompt(char* arg0);
s32 PikachuReward_SelectMoveToForget(char* arg0);
void PikachuReward_ShowDeclinedEnding(void);
void PikachuReward_ShowSuccessEnding(void);
void PikachuReward_PlayIntroCamera(void);
s32 PikachuReward_ShowIntroAndConfirm(void);
s32 PikachuReward_ResolveMoveSlot(void);
void PikachuReward_ShowSaveStatusScreen(s32 arg0);
void PikachuReward_SaveToCartridge(void);
void PikachuReward_TeachSurfToPikachu(s32 arg0);
void PikachuReward_RunSequence(void);
void PikachuReward_SetupFollowerModel(DisplayObject* arg0, s16 arg1, s16 arg2, s16 arg3);
void PikachuReward_SetupScene(void);
s32 Stadium_BonusPikachuReward(UNUSED s32 arg0, UNUSED s32 arg1);

#endif // _FRAGMENT66_H_
