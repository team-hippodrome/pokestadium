#ifndef _2C1C0_H_
#define _2C1C0_H_

#include "global.h"
#include "game_state.h"

extern u8 sEventBattleSettings[3];

void Team_ClearTrainerSlot(BattleSessionTeams* arg0);
void Session_SetMode(s32 arg0, s32 arg1, s32 arg2);
void Team_ResetOpponentHistory(void);
TeamRoster* Trainer_Create(s32 arg0, s16 arg1, char* arg2, char* arg3);
void Team_AddTrainer(s32 arg0, TeamRoster* arg1);
void Trainer_AddPokemon(TeamRoster* arg0, BattleMon* arg1);
BattleMon* Pokemon_CreateDefault(BattleMon* arg0, u8 arg1, s16 arg2);
void Session_SetupDemoBattle(s16 arg0, s16 arg1, s16 arg2, s16 arg3);
void Team_AddDeckTrainer(s32 arg0, s32 arg1, s32 arg2);
void Team_BuildDeckOpponents(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32 Team_IsOpponentIndexUsed(s32 arg0);
s32 Team_PickRandomOpponentIndex(s32 arg0);
s16 Team_GetHighestPartyLevel(void);
void Team_ScaleOpponentToLevel(s16 arg0);
void Team_LoadPresetTrainer(s16 arg0, s16 arg1, s16 arg2, s32 arg3);
void Team_LoadOpponentPreset(void);
void Team_BuildQuickBattleTeams(s16 arg0);
void Session_SaveContinueData(void);
s32 Session_LoadContinueData(void);

#endif // _2C1C0_H_
