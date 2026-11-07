#ifndef _FRAGMENT64_H_
#define _FRAGMENT64_H_

#include "global.h"
#include "src/game_state.h"

typedef struct unk_D_84803760 {
    /* 0x00 */ s8 numberPicked;
    /* 0x01 */ s8 unk_01;
    /* 0x02 */ s8 pokemonPicked[6];
    /* 0x08 */ s8 pokemonPendingPick[6];
    /* 0x0E */ s8 firstTagTeamPartnerPort;
    /* 0x0F */ s8 secondTagTeamPartnerPort;
} TeamOrderState; // size = 0x10

typedef struct unk_D_848037A0 {
    /* 0x0000 */ s8 unk_0000;
    /* 0x0001 */ s8 port;
    /* 0x0002 */ s8 tagTeamPartnerPort;
    /* 0x0003 */ s8 isHuman;
    /* 0x0004 */ char unk0004[1];
    /* 0x0005 */ s8 doneSelecting;
    /* 0x0006 */ s8 confirmCursorMoving;
    /* 0x0007 */ s8 summaryDisplayProgress;
    /* 0x0008 */ s8 loadProgress;
    /* 0x0009 */ s8 rButtonHeld;
    /* 0x000A */ s8 confirmCursorSelection;
    /* 0x000B */ s8 numberOfPokemonPicked;
    /* 0x000C */ s8 pickedSlots[6];
    /* 0x0012 */ s16 x;
    /* 0x0014 */ s16 y;
    /* 0x0016 */ s8 summaryDisplaySlot;
    /* 0x0017 */ s8 rosterSize;
    /* 0x0018 */ BattleMon* pokemon;
    /* 0x001C */ TeamRoster* roster;
    /* 0x0020 */ u16 firstPokemonIcon[1][1600];
    /* 0x0CA0 */ char otherPokemonIcon[16000];
} BattleOrderInputState; // size = 0x4B20

typedef struct unk_D_848035FC {
    /* 0x00 */ Color_RGB8 color;
    /* 0x04 */ u8* string;
} PokemonTypeString; // size = 0x8

typedef struct unk_D_84803548 {
    /* 0x00 */ u16 delay;
    /* 0x02 */ u16 button;
} ComSelectionButtonPresses; // size = 0x4

void BattleMenu_DrawBottomActionButtons(void);
void BattleMenu_DrawScrollingRulesText(void);
void BattleMenu_InitTeamOrderState(TeamOrderState* arg0, BattleSessionTeams* arg1, s32 arg2);
void BattleMenu_DrawSidebarLeadIcon(s32 arg0, s32 arg1, TeamOrderState* arg2);
s32 BattleMenu_DrawSidebarRowTypeIcon(s32 arg0, s32 arg1, s8 arg2, s8 arg3);
void BattleMenu_DrawTeamOrderSidebarTeam0(void);
void BattleMenu_DrawTeamOrderSidebarTeam1(void);
void BattleMenu_AppendOrderSlotIcon(s32 arg0, s32 arg1);
void BattleMenu_ResetOrderSlotIcons(s32 arg0);
void BattleMenu_DrawBorderedPanel(s16 arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4);
void BattleMenu_DrawOrderSelectPanel_PartyGrid(BattleOrderInputState* arg0);
void BattleMenu_DrawCursorArrow(s16 arg0, s16 arg1, s16 arg2);
void BattleMenu_DrawOrderSelectPanel_ConfirmPrompt(BattleOrderInputState* arg0);
void BattleMenu_DrawOrderSelectPanel_LevelCapNotice(BattleOrderInputState* arg0);
void BattleMenu_DrawOrderSelectPanel_Ready(BattleOrderInputState* arg0);
void BattleMenu_DrawOrderSelectPanel_MonDetail(BattleOrderInputState* arg0);
void BattleMenu_DrawConfirmButtonLabel(BattleOrderInputState* arg0);
void BattleMenu_DrawOrderSelectPanel(BattleOrderInputState* arg0);
void BattleMenu_UpdateComPanelAutoInput(void);
s32 BattleMenu_IsSlotAlreadySelected(BattleOrderInputState* arg0, s8 arg1);
s32 BattleMenu_PicksExceedLevelCap(BattleOrderInputState* arg0);
s32 BattleMenu_IsOrderSelectionComplete(BattleOrderInputState* arg0);
void BattleMenu_SelectOrderSlot(BattleOrderInputState* arg0, s8 arg1, s32 arg2);
void BattleMenu_UndoLastOrderSelection(BattleOrderInputState* arg0);
void BattleMenu_HandleOrderSelectPanel_PartyInput(Controller* arg0, BattleOrderInputState* arg1);
void BattleMenu_HandleOrderSelectPanel_ConfirmInput(Controller* arg0, BattleOrderInputState* arg1);
void BattleMenu_HandleOrderSelectPanel_LevelCapNoticeInput(Controller* arg0, BattleOrderInputState* arg1);
void BattleMenu_HandleOrderSelectPanel_MonDetailInput(Controller* arg0, BattleOrderInputState* arg1);
s32 BattleMenu_UpdateOrderSelectPanel(s32 arg0, BattleOrderInputState* arg1);
s32 BattleMenu_UpdateOrderSelect(s32 arg0);
void BattleMenu_RearrangeLeadPokemon(BattleSessionTeams* arg0, TeamOrderState* arg1);
void BattleMenu_FillDefaultOrder(s8* arg0, s32 arg1);
void BattleMenu_CommitOrderSelection(void);
void BattleMenu_AddOrderSelectPanel(s16 arg0, s16 arg1, TeamRoster* arg2, s32 arg3, s32 arg4);
void BattleMenu_InitOrderSelectPanels(SessionContext* arg0);
s32 BattleMenu_RunOrderSelectScreen(s32 arg0, s32 arg1);

#endif // _FRAGMENT64_H_
