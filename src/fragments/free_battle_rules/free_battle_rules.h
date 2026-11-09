#ifndef _FRAGMENT58_H_
#define _FRAGMENT58_H_

#include "global.h"

void FreeBattle_DrawPokeBallIcon(s16 x, s16 y);
void FreeBattle_DrawVsIcon(s16 x, s16 y);
void FreeBattle_DrawTintedBar(s16 x, s16 y, s16 width, Color_RGBA8* env_color, Color_RGBA8* prim_color);
void FreeBattle_DrawTitleBar(void);
void FreeBattle_DrawSelectionCorners(s16 x, s16 y, s16 width, s16 height);
void FreeBattle_DrawPanelFrame(s16 x, s16 y, s16 width, s16 height);
void FreeBattle_DrawRoundedPanel(s16 x, s16 y, s16 raw_width, s16 raw_height, u8 r, u8 g, u8 b, u8 alpha);

// Tag-team selection window.
s32 FreeBattle_IsTagTeamOptionEnabled(s16 option_index);
void FreeBattle_DrawTeamAmpersand(s16 x, s16 y, s16 option_index);
void FreeBattle_DrawTagTeamWindow(void);
void FreeBattle_UpdateLeftTagTeam(void);
void FreeBattle_UpdateRightTagTeam(void);
void FreeBattle_ClearTagTeamSelection(void);
s32 FreeBattle_SelectionHasTagTeam(void);
// Ruleset selection window.
void FreeBattle_DrawRulesetOptionText(s16 box_x, s16 box_y, s16 option_index, char* text);
void func_84001AD8(void);
void FreeBattle_DrawRulesetWindow(void);
void FreeBattle_UpdateRulesetSelection(void);
void FreeBattle_ClearRulesetSelection(void);
// COM difficulty popup.
void FreeBattle_DrawComDifficultyText(s16 position, char* text);
void FreeBattle_DrawComDifficultyWindow(void);
void FreeBattle_InitComDifficultyWindow(void);
void FreeBattle_OpenComDifficultyWindow(void);
void FreeBattle_CloseComDifficultyWindow(void);
void FreeBattle_UpdateComDifficulty(void);
void FreeBattle_PollInputs(void);
void FreeBattle_DrawFrame(void);
void FreeBattle_FadeIn(void);
void FreeBattle_FadeOut(void);
void FreeBattle_SlideInPanel(void);
void FreeBattle_SlideOutPanel(void);
void FreeBattle_ApplySettings(void);
s32 FreeBattle_SettingsLoop(void);
s32 FreeBattle_SettingsMain(s32 arg0, s32 arg1);

#endif // _FRAGMENT58_H_
