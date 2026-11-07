#ifndef _FRAGMENT64_H_
#define _FRAGMENT64_H_

#include "global.h"
#include "src/game_state.h"

typedef struct TeamOrderState {
    /* 0x00 */ s8 numberPicked;
    /* 0x01 */ s8 isCom;
    // High nibble: controller port + 1; low nibble: animation frame. 0 = empty, -1 = disabled.
    /* 0x02 */ s8 slotIconStates[6];
    /* 0x08 */ s8 slotIconDirections[6];
    /* 0x0E */ s8 firstTagTeamPartnerPort;
    /* 0x0F */ s8 secondTagTeamPartnerPort;
} TeamOrderState; // size = 0x10

typedef struct BattleOrderInputState {
    /* 0x0000 */ s8 panelIndex;
    /* 0x0001 */ s8 controllerPort;
    /* 0x0002 */ s8 partnerPanelIndex;
    /* 0x0003 */ s8 sideIndex;
    /* 0x0004 */ char unk0004[1];
    /* 0x0005 */ s8 panelMode; // 0 grid, 1 confirm, 2 level cap, 3 ready, 4 details.
    /* 0x0006 */ s8 redrawFrames;
    /* 0x0007 */ s8 overlayClearFrames;
    /* 0x0008 */ s8 readyPulseFrame;
    /* 0x0009 */ s8 rButtonHeld;
    /* 0x000A */ s8 confirmChoice;
    /* 0x000B */ s8 numberOfPokemonPicked;
    /* 0x000C */ s8 pickedSlots[6];
    /* 0x0012 */ s16 x;
    /* 0x0014 */ s16 y;
    /* 0x0016 */ s8 summaryDisplaySlot;
    /* 0x0017 */ s8 rosterSize;
    /* 0x0018 */ BattleMon* pokemon;
    /* 0x001C */ TeamRoster* roster;
    // FAKE: loading and drawing index this split storage as six consecutive 40x40 RGBA16 icons.
    /* 0x0020 */ u16 firstPokemonIcon[1][1600];
    /* 0x0CA0 */ char otherPokemonIcon[16000];
} BattleOrderInputState; // size = 0x4B20

typedef struct PokemonTypeStyle {
    /* 0x00 */ Color_RGB8 color;
    /* 0x04 */ u8* iconTexture;
} PokemonTypeStyle; // size = 0x8

typedef struct ComSelectionButtonPresses {
    /* 0x00 */ u16 delay;
    /* 0x02 */ u16 button;
} ComSelectionButtonPresses; // size = 0x4

void BattleMenu_DrawBottomActionButtons(void);
void BattleMenu_DrawScrollingHelpText(void);
void BattleMenu_InitTeamOrderState(TeamOrderState* team_order_state, BattleSessionTeams* battle_session_teams,
                                   s32 rule_category);
void BattleMenu_DrawPlayerPortIcon(s32 x, s32 y, TeamOrderState* state);
s32 BattleMenu_DrawOrderSlotIcon(s32 x, s32 y, s8 icon_state, s8 animation_direction);
void BattleMenu_DrawTeamOrderSidebarTeam0(void);
void BattleMenu_DrawTeamOrderSidebarTeam1(void);
void BattleMenu_AppendOrderSlotIcon(s32 side, s32 controller_port);
void BattleMenu_ResetOrderSlotIcons(s32 side);
void BattleMenu_DrawPanelBorder(s16 x, s16 y, s16 width, s16 height, s16 port);
void BattleMenu_DrawOrderSelectPanel_PartyGrid(BattleOrderInputState* input_state);
void BattleMenu_DrawCursorArrow(s16 x, s16 y, s16 oscillation_index);
void BattleMenu_DrawOrderSelectPanel_ConfirmPrompt(BattleOrderInputState* input_state);
void BattleMenu_DrawOrderSelectPanel_LevelCapNotice(BattleOrderInputState* input_state);
void BattleMenu_DrawOrderSelectPanel_Ready(BattleOrderInputState* input_state);
void BattleMenu_DrawOrderSelectPanel_MonDetail(BattleOrderInputState* input_state);
void BattleMenu_DrawTrainerLabel(BattleOrderInputState* input_state);
void BattleMenu_DrawOrderSelectPanel(BattleOrderInputState* input_state);
void BattleMenu_UpdateComPanelAutoInput(void);
s32 BattleMenu_IsSlotAlreadySelected(BattleOrderInputState* input_state, s8 slot);
s32 BattleMenu_PicksExceedLevelCap(BattleOrderInputState* input_state);
s32 BattleMenu_IsOrderSelectionComplete(BattleOrderInputState* input_state);
void BattleMenu_SelectSlotOrShowSummary(BattleOrderInputState* input_state, s8 selected_slot, s32 show_summary);
void BattleMenu_UndoSelections(BattleOrderInputState* order_state);
void BattleMenu_HandleOrderSelectPanel_PartyInput(Controller* controller, BattleOrderInputState* input_state);
void BattleMenu_HandleOrderSelectPanel_ConfirmInput(Controller* controller, BattleOrderInputState* input_state);
void BattleMenu_HandleOrderSelectPanel_LevelCapNoticeInput(Controller* controller, BattleOrderInputState* input_state);
void BattleMenu_HandleOrderSelectPanel_MonDetailInput(Controller* controller, BattleOrderInputState* input_state);
s32 BattleMenu_UpdateOrderSelectPanel(s32 fade_mode, BattleOrderInputState* input_state);
s32 BattleMenu_UpdateOrderSelect(s32 fade_mode);
void BattleMenu_SetLeadTagTeamMember(BattleSessionTeams* battle_session_teams, TeamOrderState* order_state);
void BattleMenu_AppendUnpickedSlots(s8* pokemon_slots, s32 number_of_slots_filled);
void BattleMenu_CommitOrderSelection(void);
void BattleMenu_AddOrderSelectPanel(s16 x, s16 y, TeamRoster* roster, s32 side_index, s32 partner_panel_index);
void BattleMenu_InitOrderSelectPanels(SessionContext* session_context);
s32 BattleMenu_RunOrderSelectScreen(s32 arg0, s32 arg1);

#endif // _FRAGMENT64_H_
