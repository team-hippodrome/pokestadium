#ifndef _FRAGMENT63_H_
#define _FRAGMENT63_H_

#include "global.h"
#include "src/jpeg_stream.h"
#include "src/poke_icon.h"
#include "src/geo_layout.h"

// One point of the background starfield (texture D_3024A00) that streams toward the camera behind every
// battle_prep phase: the versus and round intros, the result screen and the castle clear outro.
typedef struct StarfieldStar {
    // Star 0: set to 1 (as an s8) when BattlePrep_InitStarfield finishes. Other stars: a random 20-30 that
    // nothing reads. InitStarfield writes it one star ahead (i + 1), so its last write lands past sStarfield[399].
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ Vec3s position;
} StarfieldStar; // size = 0x8

// Scene-wide state shared by every battle_prep phase (intros, result screen, castle outro).
typedef struct BattlePrepState {
    /* 0x0000 */ char unk0000[0x1];
    /* 0x0001 */ u8 playerWon; // battleSides[0].wonBattle set and the sides differ
    /* 0x0002 */ u8 perfectWin; // BattlePrep_IsPerfectCupWin result
    /* 0x0003 */ u8 isDraw; // both sides' wonBattle flags are equal
    /* 0x0004 */ GraphNode* rootNode; // built from sSceneGeoLayout; its first child is the perspective camera
    /* 0x0008 */ DisplayObject displayObjects[128]; // pool lent to the roster, team icons, ladder, banners and labels
    /* 0xB408 */ BinArchive* trainerDataArchive; // TRAINER_DATA_ROM_START: per-cup TrainerData lists for the ladder
    /* 0xB40C */ BinArchive* portraitArchive; // battle_portraits: trainer portrait images
} BattlePrepState; // size >= 0xB410

typedef struct BattlePrepRuleWindow {
    /* 0x00 */ u8 state;
    /* 0x01 */ u8 kind;
    /* 0x02 */ s16 openingProgress;
    /* 0x04 */ s16 selectedOption;
    /* 0x06 */ char unk06[0x2];
} BattlePrepRuleWindow; // size >= 0x8

typedef struct BattlePrepBanner {
    /* 0x00 */ u8 state; // 0 idle, 1 round intro, 2 CLEAR plaque slide-in, 3 slide-out
    /* 0x01 */ u8 timer; // Frames since the current state started.
    /* 0x02 */ s16 roundLabelRotationZ[5]; // Per-copy rotationZ of round label objects 9-12; [4] is never used.
    /* 0x0C */ void* plateTexture; // 256x96 RGBA16 plaque background from battle_headers.
    /* 0x10 */ void* titleTexture; // 200x28 IA8 cup or gym name, e.g. "PEWTER GYM BATTLE".
    /* 0x14 */ void* divisionTexture; // 152x20 IA8 division name, e.g. "POKé BALL".
    /* 0x18 */ void* roundLabelTexture; // IA8 "BATTLE", "TRAINER", "LEADER", "SEMIFINAL", ...
    /* 0x1C */ void* roundNumberTexture; // 32x32 IA8 round digit "1"-"6" (segment 0x0E).
    /* 0x20 */ DisplayObject* displayObjects;
    /* 0x24 */ unk_D_86002F34* plateLayout;
    /* 0x28 */ unk_D_86002F34* titleTopLayout; // Uploads the title's top-row vertices only.
    /* 0x2C */ unk_D_86002F34* titleBottomLayout; // Uploads the bottom-row vertices and draws the title.
    /* 0x30 */ unk_D_86002F34* roundLabelLayout;
    /* 0x34 */ unk_D_86002F34* divisionLayout;
    /* 0x38 */ unk_D_86002F34* clearLabelLayout;
    /* 0x3C */ char unk3C[0x4];
} BattlePrepBanner; // size >= 0x40

typedef struct OpponentLadder {
    /* 0x00 */ u8 state; // 0 idle, 1 slide in, 2 slide out, 3 flip in and stamp (win), 4 flip in (loss)
    /* 0x01 */ u8 timer; // Frames since the current state started.
    /* 0x02 */ u8 highlightCurrent; // Pulse the frame around the current round's opponent.
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ DisplayObject* displayObjects;
    /* 0x0C */ GraphNode* slotLayout;
    /* 0x10 */ u8* portraits[4]; // Cups store 8 portraits here, running on into unk20.
    /* 0x20 */ char unk20[0x10];
    /* 0x30 */ DepthBuffer* castleCards[4]; // Rendered 64x96 castle slot cards (name, portrait).
} OpponentLadder; // size >= 0x40

/**
 * Post-battle team display: both sides' picked Pokémon as icon cards with HP bars. The cards flip in,
 * the HP bars drain from full to each Pokémon's final HP, a red X stamps each fainted Pokémon in faint
 * order, and fainted cards then flip over to a face showing the Pokémon's model texture and "FAINTED"
 * (D_3016F00). Side 0 is the top row (blue panels), side 1 the bottom row (green panels).
 */
typedef struct BattlePrepTeamIcons {
    /* 0x00 */ u8 state; // 0 idle, 1 loading icons, 2 icons ready, 3 result animation running.
    /* 0x01 */ u8 timer; // Frame counter for the loading wait and the result animation.
    /* 0x02 */ u8 isDoubleLayout; // 1 for 6 slots per side (tag team or 4+ brought): two half-height cards each.
    /* 0x04 */ DisplayObject* displayObjects; // 0/7 trainer cards, 1-6 cards, 9-14 fainted faces, 16-39 flip nodes.
    /* 0x08 */ unk_D_86002F34* iconGeo; // Card with icon and HP bar (upper half in the double layout).
    /* 0x0C */ unk_D_86002F34* lowerIconGeo; // Double layout only: lower-half card for a tag team's second member.
    /* 0x10 */ unk_D_86002F34* faintedGeo; // Fainted face: model texture + "FAINTED" (upper half in the double layout).
    /* 0x14 */ unk_D_86002F34* lowerFaintedGeo; // Double layout only: lower-half fainted face.
    /* 0x18 */ unk_D_86002F34* trainerCardGeo;
    /* 0x1C */ unk_func_8001A024* iconRenderList; // PokeIcon render list; HP bars are drawn into its slot images.
    /* 0x20 */ u16 (*modelTextures)[0x640]; // 40x40 RGBA16 model texture per slot, shown on the fainted face.
    /* 0x24 */ DepthBuffer* side0CardImage; // 64x96 trainer card drawn by BattlePrep_DrawTrainerStatusIcon.
    /* 0x28 */ DepthBuffer* side1CardImage;
    /* 0x2C */ u8 isFaintMarked[12]; // Per slot: set in faint order to show the red X (D_3017F00) over the card.
} BattlePrepTeamIcons; // size = 0x38

// Post-battle badge plate grid: eight 64x96 badge plates (BOULDER ... EARTH) flip face-up in a 4x2
// grid, then the medal of each cup round won appears on its plate with a smoke puff.
typedef struct BattlePrepBadgeAward {
    /* 0x00 */ u8 state; // 0 idle, 1 flip in, 2 reveal medals, 3 twinkle loop, 4 scatter exit
    /* 0x01 */ u8 timer;
    /* 0x04 */ DisplayObject* displayObjects; // 0-15 plates/medals, 16-23 smoke puffs, 24 shockwave, 16-39 twinkles
    /* 0x08 */ unk_D_86002F34* plateGeo;
    /* 0x0C */ unk_D_86002F34* medalGeo;
    /* 0x10 */ unk_D_86002F34* smokePuffGeo;
    /* 0x14 */ unk_D_86002F34* shockwaveGeo; // I4 ring D_2022640/D_2022A40, tinted blue
    /* 0x18 */ unk_D_86002F34* twinkleGeo; // IA8 four-point twinkle D_2022E40, tinted yellow
    /* 0x1C */ u8* plateTextures; // badge_ui D_2018000 pointer table
    /* 0x20 */ u8* medalTextures; // badge_ui D_2021620 pointer table
    /* 0x24 */ u8** smokePuffTextures; // badge_ui D_2023240: 8 I4 32x32 smoke cloud frames
    // Current smoke frame per medal, -1 = finished. Indexed 0-7, so it runs into the pad below.
    /* 0x28 */ s8 smokePuffFrames[4];
    /* 0x2C */ char unk2C[0x4];
} BattlePrepBadgeAward; // size = 0x30

// One of the 10 smoke puffs that burst around the LOSE banner (texture table D_2023240).
typedef struct ResultBannerSmokePuff {
    /* 0x00 */ DisplayObject* displayObject;
    /* 0x04 */ s16 timer; // 0-15 animating; texture frame = timer / 2
    /* 0x06 */ s16 angle; // direction it drifts outward
    /* 0x08 */ Color_RGB8 color; // env tint; blue fades from 0x82 to 0x28
    /* 0x0C */ u8* texture; // current smoke frame, bound to segment 0x0F
} ResultBannerSmokePuff; // size = 0x10

// A star particle thrown by the WIN banner (texture D_3024800).
typedef struct ResultBannerStar {
    /* 0x00 */ DisplayObject* displayObject;
    /* 0x04 */ s16 mode; // 0 fly out with color ramp, 1 trail, 2 flash, 3 scatter
    /* 0x06 */ s16 timer; // -1 = free slot
    /* 0x08 */ s16 angle; // flight direction
    /* 0x0A */ Color_RGBA8 color; // env tint
} ResultBannerStar; // size = 0x10

// The post-battle WIN, LOSE or DRAW banner for one side, with its star and smoke puff particles.
typedef struct ResultBanner {
    /* 0x000 */ ResultBannerSmokePuff smokePuffs[10];
    /* 0x0A0 */ ResultBannerStar stars[10][8]; // per star: [0] flier, [1-4] trail, [5-7] flash
    // 0 idle, 1 particle burst, 2 flight arc, 3 star throw exit, 4 fall exit, 5 DRAW slide in, 7 DRAW exit
    /* 0x5A0 */ u8 state;
    /* 0x5A1 */ s8 timer;
    /* 0x5A2 */ s8 outcome; // -1 DRAW (D_3020800), 0 LOSE (D_301C800), nonzero WIN (D_3018800)
    /* 0x5A3 */ s8 isPlayerSide; // 1 = the player's banner (top row), 0 = the opponent's (bottom row)
    /* 0x5A4 */ f32 velocityY;
    /* 0x5A8 */ DisplayObject* banner;
    /* 0x5AC */ DisplayObject* particleObjects; // 10 smoke puffs, then 80 star objects
    /* 0x5B0 */ unk_D_86002F34* bannerGeo;
    /* 0x5B4 */ unk_D_86002F34* smokePuffGeo;
    /* 0x5B8 */ unk_D_86002F34* starGeo;
    /* 0x5BC */ char unk5BC[0x4];
} ResultBanner; // size = 0x5C0

// A label that pops in after a perfect cup win: PERFECT (D_3028870) or EXTRA CONTINUE (D_30281B0).
typedef struct ResultLabel {
    /* 0x00 */ u8 state; // 0 idle, 1 hold, 2 grow, 3 shrink
    /* 0x01 */ s8 scaleStep; // 0-8; scale = scaleStep / 8
    /* 0x02 */ s8 kind; // 0 PERFECT, 1 EXTRA CONTINUE
    /* 0x04 */ DisplayObject* displayObject;
    /* 0x08 */ unk_D_86002F34* geo;
} ResultLabel; // size = 0xC

typedef struct BattlePrepResultUi {
    /* 0x00 */ ResultLabel perfectLabel;
    /* 0x0C */ char unk0C[0x4];
    /* 0x10 */ ResultLabel extraContinueLabel;
    /* 0x1C */ BattlePrepRuleWindow mainWindow; // prompt (kinds 1, 2, 10) or header (kinds 7, 8, 9)
    /* 0x24 */ BattlePrepRuleWindow menuWindow; // menu shown under a header (kinds 3, 4, 5, 6)
} BattlePrepResultUi; // size = 0x2C

typedef struct BattlePrepRuleWindowLayout {
    /* 0x00 */ s16 x;
    /* 0x02 */ s16 y;
    /* 0x04 */ s16 width;
    /* 0x06 */ s16 restingHeight; // ruleset?
    /* 0x08 */ s16 optionCount;
    /* 0x0A */ s16 color;
    /* 0x0C */ s16 isKind8;
    /* 0x0E */ s16 disabledRows;
    /* 0x10 */ s16 unk_10;
} BattlePrepRuleWindowLayout; // size = 0x12

extern unk_D_8006FF00 sRosterSpeciesIcons[];
extern unk_D_8006FF00 sTeamIconsSpeciesIcons[];

void BattlePrep_SaveVictoryPalaceRecords(void);
void BattlePrep_AdvanceGbTowerUnlock(void);
void BattlePrep_MarkModeCleared(void);
void BattlePrep_UpdateClearProgress(void);
char* BattlePrep_GetString(s32);
char* BattlePrep_FormatString(char* buffer, s32 buffer_size, u32 string_id);
s32 BattlePrep_StarfieldGeoCallback(s32 event, unk_D_86002F34_alt8* geo_node);
void BattlePrep_ResetDisplayObject(DisplayObject* display_object);
void BattlePrep_ResetAllDisplayObjects(void);
void BattlePrep_DrawBorderedRect(s16, s16, s16, s16, u8, u8, u8, u8);
void BattlePrep_RenderTrainerPortrait(DepthBuffer*, s16, u8*, u8*);
void BattlePrep_ScatterTeamPanelsStep(s16 frame);
void BattlePrep_FadeOutResultPanelsStep(s16 frame);
void BattlePrep_RenderFrame(void);
void BattlePrep_UpdateVersusIntroFrame(void);
s32 BattlePrep_ShowVersusIntro(void);
void BattlePrep_UpdateRoundIntroFrame(void);
void BattlePrep_InitRoundIntroScene(void);
s32 BattlePrep_ShowRoundIntro(void);
void BattlePrep_CheckPikachuNeedsSurf(void);
void BattlePrep_UpdateResultScene(void);
void BattlePrep_InitResultScene(void);
void BattlePrep_RunResultReveal(void);
void BattlePrep_RunPerfectResultReveal(void);
void BattlePrep_RunOpponentLadderResult(void);
void BattlePrep_ScatterTeamPanelsExit(void);
void BattlePrep_FadeOutResultPanels(void);
void BattlePrep_FadeOutScene(void);
s32 BattlePrep_PromptKeepBattling(void);
s32 BattlePrep_SpendContinueAndSave(void);
s32 BattlePrep_ShowFreeBattleResult(void);
s32 BattlePrep_ShowSimpleResult(void);
s32 BattlePrep_ShowCupResult(void);
s32 BattlePrep_ShowCastleResult(void);
s32 BattlePrep_ShowMewtwoResult(void);
u8 BattlePrep_IsPerfectCupWin(void);
s32 BattlePrep_ShowBattleResult(void);
s32 BattlePrep_ShowCastleClearOutro(void);
void BattlePrep_InitScene(void);
s32 BattlePrep_Main(s32 phase, UNUSED s32 unused_arg);

s16 BattlePrep_RandomStarOffset(void);
void BattlePrep_DrawStarfield(StarfieldStar* stars);
void BattlePrep_UpdateStarfield(StarfieldStar* stars);
void BattlePrep_InitStarfield(StarfieldStar* stars);

s32 BattlePrepBanner_DrawPlate(s32 event, unk_func_80011B94* display_node);
s32 BattlePrepBanner_DrawTitle(s32 event, unk_func_80011B94* display_node);
s32 BattlePrepBanner_DrawRoundLabel(s32 event, unk_func_80011B94* display_node);
s32 BattlePrepBanner_DrawDivision(s32 event, unk_func_80011B94* display_node);
s32 BattlePrepBanner_ApplyRoundLabelSpin(s32 event, unk_D_86002F34_alt5* rotate_node);
void BattlePrepBanner_SlidePlateIn(BattlePrepBanner* banner, s16 frame);
void BattlePrepBanner_SlideTitleInNTSC(BattlePrepBanner* banner, s16 frame);
void BattlePrepBanner_SlideTitleInPAL(BattlePrepBanner* banner, s16 frame);
void BattlePrepBanner_SpinRoundLabelInNTSC(BattlePrepBanner* banner, s16 frame);
void BattlePrepBanner_SpinRoundLabelInPAL(BattlePrepBanner* banner, s16 frame);
void BattlePrepBanner_UpdateRoundIntro(BattlePrepBanner* banner);
void BattlePrepBanner_UpdateSlideOut(BattlePrepBanner* banner);
void BattlePrepBanner_UpdateClearSlideIn(BattlePrepBanner* banner);
void BattlePrepBanner_Update(BattlePrepBanner* banner);
void BattlePrepBanner_InitPlate(BattlePrepBanner* banner, s16 x, s16 y);
void BattlePrepBanner_InitTitleCopy(BattlePrepBanner* banner, s16 copy, s16 x, s16 y);
void BattlePrepBanner_InitRoundLabelCopy(BattlePrepBanner* banner, s16 copy);
void BattlePrepBanner_InitClearLabelCopy(BattlePrepBanner* banner, s16 copy);
void BattlePrepBanner_InitDivisionCopy(BattlePrepBanner* banner, s16 copy);
void BattlePrepBanner_StartRoundIntro(BattlePrepBanner* banner, DisplayObject* display_objects);
void BattlePrepBanner_StartSlideOut(BattlePrepBanner* banner);
void BattlePrepBanner_StartClearPlaque(BattlePrepBanner* banner, DisplayObject* display_objects);
void BattlePrepBanner_LoadTextures(BattlePrepBanner* banner);
void BattlePrepBanner_FitRound2Icon(BattlePrepBanner* banner);
void BattlePrepBanner_LoadGeoLayouts(BattlePrepBanner* banner);
void BattlePrepBanner_SetVertexGradient(unk_D_86002F34* layout, Color_RGBA8* top_color, Color_RGBA8* bottom_color);
void BattlePrepBanner_ApplyColors(BattlePrepBanner* banner);
void BattlePrepBanner_Init(BattlePrepBanner* banner);
s32 OpponentLadder_DrawCupSlot(s32 event, unk_func_80011B94* display_node);
s32 OpponentLadder_DrawCupHighlight(s32 event, unk_func_80011B94* display_node);
s32 OpponentLadder_DrawCastleSlot(s32 event, unk_func_80011B94* display_node);
s32 OpponentLadder_DrawCastleHighlight(s32 event, unk_func_80011B94* display_node);
void OpponentLadder_StampDefeatedMark(s32 portrait);
void OpponentLadder_FinishFlip(OpponentLadder* ladder);
void OpponentLadder_UpdateSlideIn(OpponentLadder* ladder);
void OpponentLadder_UpdateSlideOut(OpponentLadder* ladder);
void OpponentLadder_UpdateFlipInAndStamp(OpponentLadder* ladder);
void OpponentLadder_UpdateFlipIn(OpponentLadder* ladder);
void OpponentLadder_Update(OpponentLadder* ladder);
void OpponentLadder_Start(OpponentLadder* ladder, DisplayObject* display_objects, s16 animation);
void OpponentLadder_RenderCastleCard(OpponentLadder* ladder, s16 slot, char* name);
s32 BattlePrep_Align64PoolBlock(s32* block);
void OpponentLadder_LoadPortraits(OpponentLadder* ladder, BinArchive* trainer_archive, BinArchive* portrait_archive);
void OpponentLadder_Init(OpponentLadder* ladder, BinArchive* trainer_archive, BinArchive* portrait_archive);

s32 BattlePrepRoster_DrawIconColumnSingle(s32 phase, unk_func_80011B94* display_node);
s32 BattlePrepRoster_DrawIconColumnDouble(s32 phase, unk_func_80011B94* display_node);
s32 BattlePrepRoster_DrawTrainerCard(s32 phase, unk_func_80011B94* display_node);
s32 BattlePrepRoster_ScrollVersusLogoShine(s32 phase, unk_func_80011B94* display_node);
s32 BattlePrepRoster_FlipInPanelsStep(BattlePrepRoster* roster, s16 frame);
void BattlePrepRoster_ShakeCamera(BattlePrepRoster* roster, s16 frame);
s32 BattlePrepRoster_VersusLogoStepNTSC(BattlePrepRoster* roster, s16 frame);
s32 BattlePrepRoster_VersusLogoStepPAL(BattlePrepRoster* roster, s16 frame);
void BattlePrepRoster_UpdateAnimationStep(BattlePrepRoster* roster);
void BattlePrepRoster_UpdateLoadingStep(BattlePrepRoster* roster);
void BattlePrepRoster_Update(BattlePrepRoster* roster);
void BattlePrepRoster_QueueIconsSingle(BattlePrepRoster* roster);
void BattlePrepRoster_QueueIconsDouble(BattlePrepRoster* roster);
void BattlePrepRoster_Load(BattlePrepRoster* roster, BinArchive* portrait_archive);
void BattlePrepRoster_PositionPanelsSingle(BattlePrepRoster* roster);
void BattlePrepRoster_PositionPanelsDouble(BattlePrepRoster* roster);
s32 BattlePrepRoster_InitDisplayObjects(BattlePrepRoster* roster, DisplayObject* display_objects);
void BattlePrepRoster_Init(BattlePrepRoster* roster, CameraNode* camera);

s32 BattlePrepTeamIcons_DrawTrainerCard(s32 phase, unk_func_80011B94* display_node);
s32 BattlePrepTeamIcons_DrawIconCardSingle(s32 phase, unk_func_80011B94* display_node);
s32 BattlePrepTeamIcons_DrawFaintedFaceSingle(s32 phase, unk_func_80011B94* display_node);
s32 BattlePrepTeamIcons_DrawIconCardDouble(s32 phase, unk_func_80011B94* display_node);
s32 BattlePrepTeamIcons_DrawFaintedFaceDouble(s32 phase, unk_func_80011B94* display_node);
void BattlePrepTeamIcons_DrawHpBarsSingle(BattlePrepTeamIcons* team_icons, s32 min_hp_percent);
void BattlePrepTeamIcons_DrawHpBarsDouble(BattlePrepTeamIcons* team_icons, s32 min_hp_percent);
void BattlePrepTeamIcons_MarkFaintedSingle(BattlePrepTeamIcons* team_icons, s32 faint_order);
void BattlePrepTeamIcons_MarkFaintedDouble(BattlePrepTeamIcons* team_icons, s32 faint_order);
void BattlePrepTeamIcons_InitIconCardsSingle(BattlePrepTeamIcons* team_icons);
void BattlePrepTeamIcons_InitFaintedFacesSingle(BattlePrepTeamIcons* team_icons);
void BattlePrepTeamIcons_InitIconCardsDouble(BattlePrepTeamIcons* team_icons);
void BattlePrepTeamIcons_InitFaintedFacesDouble(BattlePrepTeamIcons* team_icons);
void BattlePrepTeamIcons_FlipInStepSingle(BattlePrepTeamIcons* team_icons, s16 frame);
void BattlePrepTeamIcons_FlipInStepDouble(BattlePrepTeamIcons* team_icons, s16 frame);
void BattlePrepTeamIcons_HpDrainStepSingleNTSC(BattlePrepTeamIcons* team_icons, s16 frame);
void BattlePrepTeamIcons_HpDrainStepDoubleNTSC(BattlePrepTeamIcons* team_icons, s16 frame);
void BattlePrepTeamIcons_HpDrainStepSinglePAL(BattlePrepTeamIcons* team_icons, s16 frame);
void BattlePrepTeamIcons_HpDrainStepDoublePAL(BattlePrepTeamIcons* team_icons, s16 frame);
void BattlePrepTeamIcons_FlipFaintedStepSingle(BattlePrepTeamIcons* team_icons, s16 frame);
void BattlePrepTeamIcons_FlipFaintedStepDouble(BattlePrepTeamIcons* team_icons, s16 frame);
void BattlePrepTeamIcons_UpdateAnimationStep(BattlePrepTeamIcons* team_icons);
void BattlePrepTeamIcons_UpdateLoadingStep(BattlePrepTeamIcons* team_icons);
void BattlePrepTeamIcons_Update(BattlePrepTeamIcons* team_icons);
void BattlePrepTeamIcons_QueueIconsSingle(BattlePrepTeamIcons* team_icons);
void BattlePrepTeamIcons_QueueIconsDouble(BattlePrepTeamIcons* team_icons);
void BattlePrepTeamIcons_Load(BattlePrepTeamIcons* team_icons, BinArchive* portrait_archive);
s32 BattlePrepTeamIcons_InitDisplayObjects(BattlePrepTeamIcons* team_icons, DisplayObject* display_objects);
void BattlePrepTeamIcons_LoadModelTexturesSingle(BattlePrepTeamIcons* team_icons);
void BattlePrepTeamIcons_LoadModelTexturesDouble(BattlePrepTeamIcons* team_icons);
void BattlePrepTeamIcons_Init(BattlePrepTeamIcons* team_icons);

s32 BattlePrepBadgeAward_DrawPlate(s32 callback_phase, unk_func_80011B94* node);
s32 BattlePrepBadgeAward_DrawMedal(s32 callback_phase, unk_func_80011B94* node);
s32 BattlePrepBadgeAward_DrawSmokePuff(s32 callback_phase, unk_func_80011B94* node);
s32 BattlePrepBadgeAward_DrawShockwave(s32 callback_phase, unk_func_80011B94* node);
void func_84B0B054(void);
void BattlePrepBadgeAward_FlipInStep(BattlePrepBadgeAward* badge_award);
void BattlePrepBadgeAward_RevealStep(BattlePrepBadgeAward* badge_award);
void BattlePrepBadgeAward_SpawnTwinkle(BattlePrepBadgeAward* badge_award, DisplayObject* display_object,
                                       s16 medal_index);
void BattlePrepBadgeAward_TwinkleStep(BattlePrepBadgeAward* badge_award);
void BattlePrepBadgeAward_ScatterStep(BattlePrepBadgeAward* badge_award);
void BattlePrepBadgeAward_Update(BattlePrepBadgeAward* badge_award);
void BattlePrepBadgeAward_StartFlipIn(BattlePrepBadgeAward* badge_award, DisplayObject* display_objects);
void BattlePrepBadgeAward_StartScatter(BattlePrepBadgeAward* badge_award);
void BattlePrepBadgeAward_Init(BattlePrepBadgeAward* badge_award);

s32 BattlePrepResultBanner_DrawWinOrLoseLabel(s32 callback_phase, unk_func_80011B94* node);
s32 BattlePrepResultBanner_DrawTieLabel(s32 callback_phase, unk_func_80011B94* node);
s32 BattlePrepResultBanner_DrawSmokePuff(s32 callback_phase, unk_func_80011B94* node);
void BattlePrepResultBanner_InitSmokePuff(ResultBannerSmokePuff* smoke_puff, unk_D_86002F34* geo, s16 center_x,
                                          s16 center_y, s16 angle);
void BattlePrepResultBanner_UpdateSmokePuff(ResultBannerSmokePuff* smoke_puff);
s32 BattlePrepResultBanner_DrawStarParticle(s32 callback_phase, unk_func_80011B94* node);
void BattlePrepResultBanner_InitStarParticle(ResultBannerStar* star, unk_D_86002F34* geo, s16 center_x, s16 center_y,
                                             s16 angle);
void BattlePrepResultBanner_SpawnTrailCopy(ResultBannerStar* trail, ResultBannerStar* star);
void BattlePrepResultBanner_SpawnFlashCopy(ResultBannerStar* flash, ResultBannerStar* star, s16 angle_offset);
void BattlePrepResultBanner_SpawnScatterParticle(ResultBannerStar* star, unk_D_86002F34* geo, Vec3f* origin);
void BattlePrepResultBanner_UpdateStarFlyOutStep(ResultBannerStar* star);
void BattlePrepResultBanner_UpdateTrailFadeStep(ResultBannerStar* star);
void BattlePrepResultBanner_UpdateFlashFadeStep(ResultBannerStar* star);
void BattlePrepResultBanner_UpdateScatterFallStep(ResultBannerStar* star);
void BattlePrepResultBanner_UpdateStarParticle(ResultBannerStar* star);
void BattlePrepResultBanner_UpdateParticleBurst(ResultBanner* result_banner);
void BattlePrepResultBanner_UpdateFlightArc(ResultBanner* result_banner);
void BattlePrepResultBanner_UpdateStarThrowExit(ResultBanner* result_banner);
void BattlePrepResultBanner_UpdateFallExit(ResultBanner* result_banner);
void BattlePrepResultBanner_UpdateDrawSlideIn(ResultBanner* result_banner);
void BattlePrepResultBanner_UpdateDrawExit(ResultBanner* result_banner);
void BattlePrepResultBanner_Update(ResultBanner* result_banner);
void BattlePrepResultBanner_Launch(ResultBanner* result_banner, DisplayObject* banner_object,
                                   DisplayObject* particle_objects);
void BattlePrepResultBanner_StartExit(ResultBanner* result_banner);
void BattlePrepResultBanner_Init(ResultBanner* result_banner, s8 is_player_side, s8 outcome);
void BattlePrepResultLabel_HoldStep(ResultLabel* result_label);
void BattlePrepResultLabel_GrowStep(ResultLabel* result_label);
void BattlePrepResultLabel_ShrinkStep(ResultLabel* result_label);
void BattlePrepResultLabel_Update(ResultLabel* result_label);
void BattlePrepResultLabel_StartGrow(ResultLabel* result_label, DisplayObject* display_object);
void BattlePrepResultLabel_StartShrink(ResultLabel* result_label);
void BattlePrepResultLabel_Init(ResultLabel* result_label, u8 kind);
void BattlePrep_DrawCursorArrow(BattlePrepRuleWindow* text_window, s16 x, s16 y);
void BattlePrep_DrawRuleOption(BattlePrepRuleWindow* text_window, s16 x, s16 y, s16 option_index, s32 is_selectable,
                               s8* text);
void BattlePrep_DrawKeepBattlingPrompt(BattlePrepRuleWindow* text_window, BattlePrepRuleWindowLayout* window_layout);
void BattlePrep_DrawCupLossMenu(BattlePrepRuleWindow* text_window, BattlePrepRuleWindowLayout* window_layout);
void BattlePrep_DrawCastleLossMenu(BattlePrepRuleWindow* text_window, BattlePrepRuleWindowLayout* window_layout);
void BattlePrep_DrawFreeBattleMenu(BattlePrepRuleWindow* text_window, BattlePrepRuleWindowLayout* window_layout);
void BattlePrep_DrawMewtwoLossMenu(BattlePrepRuleWindow* text_window, BattlePrepRuleWindowLayout* window_layout);
void BattlePrep_DrawCupProgressHeader(BattlePrepRuleWindow* text_window, BattlePrepRuleWindowLayout* window_layout);
void BattlePrep_DrawCastleProgressHeader(BattlePrepRuleWindow* text_window, BattlePrepRuleWindowLayout* window_layout);
void BattlePrep_DrawPlayerTag(s16 x, s16 y, BattleSessionTeams* session_teams);
void BattlePrep_SetWinCountColor(BattleSessionTeams* session_teams);
void BattlePrep_DrawWinRecord(BattlePrepRuleWindow* text_window, BattlePrepRuleWindowLayout* window_layout);
void BattlePrep_DrawOverwriteSavePrompt(BattlePrepRuleWindow* text_window, BattlePrepRuleWindowLayout* window_layout);
void BattlePrep_DrawRuleWindow(BattlePrepRuleWindow* text_window);
void BattlePrep_RunQuitMenu(BattlePrepRuleWindow* text_window);
void BattlePrep_RuleWindowExpanding(BattlePrepRuleWindow* text_window);
void BattlePrep_RuleWindowCollapsing(BattlePrepRuleWindow* text_window);
void BattlePrep_UpdateRuleWindow(BattlePrepRuleWindow* text_window);
void BattlePrep_InitRuleWindow(BattlePrepRuleWindow* text_window, s16 layout_index);
void BattlePrep_CloseRuleWindow(BattlePrepRuleWindow* text_window);
s32 BattlePrep_PollRuleWindow(BattlePrepRuleWindow* text_window);

#endif // _FRAGMENT63_H_
