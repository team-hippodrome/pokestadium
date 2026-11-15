#ifndef _FRAGMENT63_H_
#define _FRAGMENT63_H_

#include "global.h"
#include "src/jpeg_stream.h"
#include "src/poke_icon.h"
#include "src/geo_layout.h"

// One point of the background starfield behind the versus and round intros.
typedef struct StarfieldStar {
    /* 0x00 */ s16 unk_00; // star 0: initialized flag (written as s8); others: random 20-30, never read
    /* 0x02 */ Vec3s position;
} StarfieldStar; // size = 0x8

typedef struct BattlePrepState {
    /* 0x0000 */ char unk0000[0x1];
    /* 0x0001 */ u8 playerWon; // battleSides[0].wonBattle set and the sides differ
    /* 0x0002 */ u8 perfectWin; // BattlePrep_IsPerfectCupWin result
    /* 0x0003 */ u8 isDraw; // both sides' wonBattle flags are equal
    /* 0x0004 */ GraphNode* unk_0004;
    /* 0x0008 */ DisplayObject unk_0008[128];
    /* 0xB408 */ BinArchive* unk_B408;
    /* 0xB40C */ BinArchive* unk_B40C;
} BattlePrepState; // size >= 0xB410

typedef struct BattlePrepRuleWindow {
    /* 0x00 */ u8 state;
    /* 0x01 */ u8 kind;
    /* 0x02 */ s16 openingProgress;
    /* 0x04 */ s16 selectedOption;
    /* 0x06 */ char unk06[0x2];
} BattlePrepRuleWindow; // size >= 0x8

typedef struct BattlePrepBanner {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ s16 unk_02[5];
    /* 0x0C */ void* unk_0C;
    /* 0x10 */ void* unk_10;
    /* 0x14 */ void* unk_14;
    /* 0x18 */ void* unk_18;
    /* 0x1C */ void* unk_1C;
    /* 0x20 */ DisplayObject* unk_20;
    /* 0x24 */ unk_D_86002F34* unk_24;
    /* 0x28 */ unk_D_86002F34* unk_28;
    /* 0x2C */ unk_D_86002F34* unk_2C;
    /* 0x30 */ unk_D_86002F34* unk_30;
    /* 0x34 */ unk_D_86002F34* unk_34;
    /* 0x38 */ unk_D_86002F34* unk_38;
    /* 0x3C */ char unk3C[0x4];
} BattlePrepBanner; // size >= 0x40

typedef struct OpponentLadder {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ DisplayObject* unk_08;
    /* 0x0C */ GraphNode* unk_0C;
    /* 0x10 */ u8* unk_10[4];
    /* 0x20 */ char unk20[0x10];
    /* 0x30 */ DepthBuffer* unk_30[4];
} OpponentLadder; // size >= 0x40

typedef struct BattlePrepTeamIcons {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x04 */ DisplayObject* unk_04;
    /* 0x08 */ unk_D_86002F34* unk_08;
    /* 0x0C */ unk_D_86002F34* unk_0C;
    /* 0x10 */ unk_D_86002F34* unk_10;
    /* 0x14 */ unk_D_86002F34* unk_14;
    /* 0x18 */ unk_D_86002F34* unk_18;
    /* 0x1C */ unk_func_8001A024* unk_1C;
    /* 0x20 */ u16 (*unk_20)[0x640];
    /* 0x24 */ DepthBuffer* unk_24;
    /* 0x28 */ DepthBuffer* unk_28;
    /* 0x2C */ u8 unk_2C[12];
} BattlePrepTeamIcons; // size = 0x38

typedef struct BattlePrepBadgeAward {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x04 */ DisplayObject* unk_04;
    /* 0x08 */ unk_D_86002F34* unk_08;
    /* 0x0C */ unk_D_86002F34* unk_0C;
    /* 0x10 */ unk_D_86002F34* unk_10;
    /* 0x14 */ unk_D_86002F34* unk_14;
    /* 0x18 */ unk_D_86002F34* unk_18;
    /* 0x1C */ u8* unk_1C;
    /* 0x20 */ u8* unk_20;
    /* 0x24 */ u8** unk_24;
    /* 0x28 */ s8 unk_28[4];
    /* 0x2C */ char unk2C[0x4];
} BattlePrepBadgeAward; // size = 0x30

typedef struct ResultBannerSpark {
    /* 0x00 */ DisplayObject* unk_00;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ Color_RGB8 unk_08;
    /* 0x0C */ u8* unk_0C;
} ResultBannerSpark; // size = 0x10

typedef struct ResultBannerStar {
    /* 0x00 */ DisplayObject* unk_00;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ Color_RGBA8 unk_0A;
} ResultBannerStar; // size = 0x10

typedef struct ResultBanner {
    /* 0x000 */ ResultBannerSpark unk_000[10];
    /* 0x0A0 */ ResultBannerStar unk_0A0[10][8];
    /* 0x5A0 */ u8 unk_5A0;
    /* 0x5A1 */ s8 unk_5A1;
    /* 0x5A2 */ s8 unk_5A2;
    /* 0x5A3 */ s8 unk_5A3;
    /* 0x5A4 */ f32 unk_5A4;
    /* 0x5A8 */ DisplayObject* unk_5A8;
    /* 0x5AC */ DisplayObject* unk_5AC;
    /* 0x5B0 */ unk_D_86002F34* unk_5B0;
    /* 0x5B4 */ unk_D_86002F34* unk_5B4;
    /* 0x5B8 */ unk_D_86002F34* unk_5B8;
    /* 0x5BC */ char unk5BC[0x4];
} ResultBanner; // size = 0x5C0

typedef struct unk_D_84B26640_010_010 {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ s8 unk_01;
    /* 0x02 */ s8 unk_02;
    /* 0x04 */ DisplayObject* unk_04;
    /* 0x08 */ unk_D_86002F34* unk_08;
} ResultLabel; // size = 0xC

typedef struct BattlePrepResultUi {
    /* 0x00 */ ResultLabel unk_00;
    /* 0x0C */ char unk0C[0x4];
    /* 0x10 */ ResultLabel unk_10;
    /* 0x1C */ BattlePrepRuleWindow unk_1C;
    /* 0x24 */ BattlePrepRuleWindow unk_28;
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

extern unk_D_8006FF00 D_84B17620[];
extern unk_D_8006FF00 D_84B186C0[];

void BattlePrep_SaveVictoryPalaceRecords(void);
void BattlePrep_MarkFirstClearBonus(void);
void BattlePrep_MarkCupCleared(void);
void BattlePrep_MarkNewRoundRecord(void);
char* BattlePrep_GetString(s32);
char* BattlePrep_FormatString(char* arg0, s32 arg1, u32 arg2);
s32 BattlePrep_StarfieldGeoCallback(s32 arg0, unk_D_86002F34_alt8* arg1);
void BattlePrep_ResetDisplayObject(DisplayObject* arg0);
void BattlePrep_ResetAllDisplayObjects(void);
void BattlePrep_DrawBorderedRect(s16, s16, s16, s16, u8, u8, u8, u8);
void BattlePrep_DrawTrainerStatusIcon(DepthBuffer*, s16, u8*, u8*);
void BattlePrep_AnimateBallBurstEffect(s16 arg0);
void BattlePrep_FadeInBallBurstObjects(s16 arg0);
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
void BattlePrep_RunPerfectReveal(void);
void BattlePrep_RunOpponentLadderResult(void);
void BattlePrep_RunBallBurstTransition(void);
void BattlePrep_FadeInBallBurstTransition(void);
void BattlePrep_FadeToTransparentEnding(void);
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
s32 BattlePrep_Main(s32 arg0, UNUSED s32 arg1);

s16 BattlePrep_RandomStarOffset(void);
void BattlePrep_DrawStarfield(StarfieldStar* arg0);
void BattlePrep_UpdateStarfield(StarfieldStar* arg0);
void BattlePrep_InitStarfield(StarfieldStar* arg0);

s32 BattlePrep_BindCarouselSegment0(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrep_BindCarouselSegment1(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrep_BindCarouselSegment3(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrep_BindCarouselSegment2(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrep_CarouselDepthCallback(s32 arg0, unk_D_86002F34_alt5* arg1);
void BattlePrep_CarouselStepFirstItem(BattlePrepBanner* arg0, s16 arg1);
void BattlePrep_CarouselStepNTSC(BattlePrepBanner* arg0, s16 arg1);
void BattlePrep_CarouselStepPAL(BattlePrepBanner* arg0, s16 arg1);
void BattlePrep_CarouselStepFinalNTSC(BattlePrepBanner* arg0, s16 arg1);
void BattlePrep_CarouselStepFinalPAL(BattlePrepBanner* arg0, s16 arg1);
void BattlePrep_UpdateCarousel(BattlePrepBanner* arg0);
void BattlePrep_CarouselExitAnimation(BattlePrepBanner* arg0);
void BattlePrep_CarouselEntryAnimation(BattlePrepBanner* arg0);
void BattlePrep_UpdateCarouselState(BattlePrepBanner* arg0);
void BattlePrep_InitCarouselBanner(BattlePrepBanner* arg0, s16 arg1, s16 arg2);
void BattlePrep_InitCarouselGymBadgeSlot(BattlePrepBanner* arg0, s16 arg1, s16 arg2, s16 arg3);
void BattlePrep_InitCarouselLeftIcon(BattlePrepBanner* arg0, s16 arg1);
void BattlePrep_InitCarouselRightIcon(BattlePrepBanner* arg0, s16 arg1);
void BattlePrep_InitCarouselCupIcon(BattlePrepBanner* arg0, s16 arg1);
void BattlePrep_InitCarouselEntry(BattlePrepBanner* arg0, DisplayObject* arg1);
void BattlePrep_StartCarouselExit(BattlePrepBanner* arg0);
void BattlePrep_InitCarouselExit(BattlePrepBanner* arg0, DisplayObject* arg1);
void BattlePrep_LoadBannerAssets(BattlePrepBanner* arg0);
void BattlePrep_AdjustBannerTextureOffset(BattlePrepBanner* arg0);
void BattlePrep_LoadBannerGeoLayouts(BattlePrepBanner* arg0);
void BattlePrep_SetVertexGradientColors(unk_D_86002F34* arg0, Color_RGBA8* arg1, Color_RGBA8* arg2);
void BattlePrep_ApplyBannerColors(BattlePrepBanner* arg0);
void BattlePrep_InitBannerScene(BattlePrepBanner* arg0);
s32 BattlePrep_OpponentLadderPortraitCallback(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrep_OpponentLadderHighlightCallback(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrep_OpponentLadderTextureCallback(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrep_OpponentLadderHighlightCallback2(s32 arg0, unk_func_80011B94* arg1);
void BattlePrep_StampDefeatedMark(s32 arg0);
void BattlePrep_ResetLadderSlots(OpponentLadder* arg0);
void BattlePrep_LadderSlideInLeft(OpponentLadder* arg0);
void BattlePrep_LadderSlideInRight(OpponentLadder* arg0);
void BattlePrep_LadderRevealSpin(OpponentLadder* arg0);
void BattlePrep_LadderHideSpin(OpponentLadder* arg0);
void BattlePrep_UpdateOpponentLadder(OpponentLadder* arg0);
void BattlePrep_InitOpponentLadder(OpponentLadder* arg0, DisplayObject* arg1, s16 arg2);
void BattlePrep_RenderLadderBackground(OpponentLadder* arg0, s16 arg1, char* arg2);
s32 BattlePrep_Align64PoolBlock(s32* arg0);
void BattlePrep_LoadLadderTextures(OpponentLadder* arg0, BinArchive* arg1, BinArchive* arg2);
void BattlePrep_InitOpponentLadderScene(OpponentLadder* arg0, BinArchive* arg1, BinArchive* arg2);

s32 BattlePrepRoster_DrawIconVariantA(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepRoster_DrawIconVariantB(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepRoster_DrawTrainerBadge(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepRoster_ScrollBackgroundTexture(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepRoster_ConvergeIconsStep(BattlePrepRoster* arg0, s16 arg1);
void BattlePrepRoster_UpdateCameraSwoop(BattlePrepRoster* arg0, s16 arg1);
s32 BattlePrepRoster_ZoomStepNTSC(BattlePrepRoster* arg0, s16 arg1);
s32 BattlePrepRoster_ZoomStepPAL(BattlePrepRoster* arg0, s16 arg1);
void BattlePrepRoster_UpdateAssembleStep(BattlePrepRoster* arg0);
void BattlePrepRoster_UpdateLoadingStep(BattlePrepRoster* arg0);
void BattlePrepRoster_Update(BattlePrepRoster* arg0);
void BattlePrepRoster_LoadDexIdsSingle(BattlePrepRoster* arg0);
void BattlePrepRoster_LoadDexIdsDouble(BattlePrepRoster* arg0);
void BattlePrepRoster_Load(BattlePrepRoster* arg0, BinArchive* arg1);
void BattlePrepRoster_PositionIconsSingle(BattlePrepRoster* arg0);
void BattlePrepRoster_PositionIconsDouble(BattlePrepRoster* arg0);
s32 BattlePrepRoster_InitDisplayObjects(BattlePrepRoster* arg0, DisplayObject* arg1);
void BattlePrepRoster_Init(BattlePrepRoster* arg0, CameraNode* arg1);

s32 BattlePrepTeamIcons_DrawActiveTeamBadge(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepTeamIcons_DrawIconSingle(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepTeamIcons_DrawIconDouble(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepTeamIcons_DrawBadgeIconSingle(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepTeamIcons_DrawBadgeIconDouble(s32 arg0, unk_func_80011B94* arg1);
void BattlePrepTeamIcons_DrawHpBarsSingle(BattlePrepTeamIcons* arg0, s32 arg1);
void BattlePrepTeamIcons_DrawHpBarsDouble(BattlePrepTeamIcons* arg0, s32 arg1);
void BattlePrepTeamIcons_MarkFaintedSingle(BattlePrepTeamIcons* arg0, s32 arg1);
void BattlePrepTeamIcons_MarkFaintedDouble(BattlePrepTeamIcons* arg0, s32 arg1);
void BattlePrepTeamIcons_InitDisplayObjectsSingleVisible(BattlePrepTeamIcons* arg0);
void BattlePrepTeamIcons_InitDisplayObjectsSingleHidden(BattlePrepTeamIcons* arg0);
void BattlePrepTeamIcons_InitDisplayObjectsDoubleVisible(BattlePrepTeamIcons* arg0);
void BattlePrepTeamIcons_InitDisplayObjectsDoubleHidden(BattlePrepTeamIcons* arg0);
void BattlePrepTeamIcons_RevealStepSingle(BattlePrepTeamIcons* arg0, s16 arg1);
void BattlePrepTeamIcons_RevealStepDouble(BattlePrepTeamIcons* arg0, s16 arg1);
void BattlePrepTeamIcons_HpDrainStepSingleNTSC(BattlePrepTeamIcons* arg0, s16 arg1);
void BattlePrepTeamIcons_HpDrainStepDoubleNTSC(BattlePrepTeamIcons* arg0, s16 arg1);
void BattlePrepTeamIcons_HpDrainStepSinglePAL(BattlePrepTeamIcons* arg0, s16 arg1);
void BattlePrepTeamIcons_HpDrainStepDoublePAL(BattlePrepTeamIcons* arg0, s16 arg1);
void BattlePrepTeamIcons_FinalRevealSingle(BattlePrepTeamIcons* arg0, s16 arg1);
void BattlePrepTeamIcons_FinalRevealDouble(BattlePrepTeamIcons* arg0, s16 arg1);
void BattlePrepTeamIcons_UpdateStep(BattlePrepTeamIcons* arg0);
void BattlePrepTeamIcons_UpdateLoadingStep(BattlePrepTeamIcons* arg0);
void BattlePrepTeamIcons_Update(BattlePrepTeamIcons* arg0);
void BattlePrepTeamIcons_LoadDexIdsSingle(BattlePrepTeamIcons* arg0);
void BattlePrepTeamIcons_LoadDexIdsDouble(BattlePrepTeamIcons* arg0);
void BattlePrepTeamIcons_Load(BattlePrepTeamIcons* arg0, BinArchive* arg1);
s32 BattlePrepTeamIcons_InitDisplayObjects(BattlePrepTeamIcons* arg0, DisplayObject* arg1);
void BattlePrepTeamIcons_LoadIconTexturesSingle(BattlePrepTeamIcons* arg0);
void BattlePrepTeamIcons_LoadIconTexturesDouble(BattlePrepTeamIcons* arg0);
void BattlePrepTeamIcons_Init(BattlePrepTeamIcons* arg0);

s32 BattlePrepBadgeAward_DrawPlate(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepBadgeAward_DrawMedal(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepBadgeAward_DrawSparkle(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepBadgeAward_DrawGlowFade(s32 arg0, unk_func_80011B94* arg1);
void BattlePrepBadgeAward_UnusedStub(void);
void BattlePrepBadgeAward_SlideInStep(BattlePrepBadgeAward* arg0);
void BattlePrepBadgeAward_RevealStep(BattlePrepBadgeAward* arg0);
void BattlePrepBadgeAward_SpawnSparkleParticle(BattlePrepBadgeAward* arg0, DisplayObject* arg1, s16 arg2);
void BattlePrepBadgeAward_UpdateSparkleStep(BattlePrepBadgeAward* arg0);
void BattlePrepBadgeAward_ScatterStep(BattlePrepBadgeAward* arg0);
void BattlePrepBadgeAward_Update(BattlePrepBadgeAward* arg0);
void BattlePrepBadgeAward_InitDisplayObjects(BattlePrepBadgeAward* arg0, DisplayObject* arg1);
void BattlePrepBadgeAward_StartScatter(BattlePrepBadgeAward* arg0);
void BattlePrepBadgeAward_Init(BattlePrepBadgeAward* arg0);

s32 BattlePrepResultBanner_DrawWinOrLoseLabel(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepResultBanner_DrawTieLabel(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepResultBanner_DrawSparkParticle(s32 arg0, unk_func_80011B94* arg1);
void BattlePrepResultBanner_InitSparkSlot(ResultBannerSpark* arg0, unk_D_86002F34* arg1, s16 arg2, s16 arg3, s16 arg4);
void BattlePrepResultBanner_UpdateSparkSlot(ResultBannerSpark* arg0);
s32 BattlePrepResultBanner_DrawStarParticle(s32 arg0, unk_func_80011B94* arg1);
void BattlePrepResultBanner_InitStarParticle(ResultBannerStar* arg0, unk_D_86002F34* arg1, s16 arg2, s16 arg3,
                                             s16 arg4);
void BattlePrepResultBanner_SpawnTrailCopy(ResultBannerStar* arg0, ResultBannerStar* arg1);
void BattlePrepResultBanner_SpawnFlashCopy(ResultBannerStar* arg0, ResultBannerStar* arg1, s16 arg2);
void BattlePrepResultBanner_SpawnScatterParticle(ResultBannerStar* arg0, unk_D_86002F34* arg1, Vec3f* arg2);
void BattlePrepResultBanner_UpdateStarGradientStep(ResultBannerStar* arg0);
void BattlePrepResultBanner_UpdateTrailFadeStep(ResultBannerStar* arg0);
void BattlePrepResultBanner_UpdateFlashFadeStep(ResultBannerStar* arg0);
void BattlePrepResultBanner_UpdateScatterFallStep(ResultBannerStar* arg0);
void BattlePrepResultBanner_UpdateStarParticle(ResultBannerStar* arg0);
void BattlePrepResultBanner_UpdateBurstEffect(ResultBanner* arg0);
void BattlePrepResultBanner_UpdateFlightArc(ResultBanner* arg0);
void BattlePrepResultBanner_UpdateStarAftermath(ResultBanner* arg0);
void BattlePrepResultBanner_UpdateSparkAftermath(ResultBanner* arg0);
void BattlePrepResultBanner_UpdateExitSlide(ResultBanner* arg0);
void BattlePrepResultBanner_UpdateFirstClearExit(ResultBanner* arg0);
void BattlePrepResultBanner_Update(ResultBanner* arg0);
void BattlePrepResultBanner_Launch(ResultBanner* arg0, DisplayObject* arg1, DisplayObject* arg2);
void BattlePrepResultBanner_StartExit(ResultBanner* arg0);
void BattlePrepResultBanner_Init(ResultBanner* arg0, s8 arg1, s8 arg2);
void BattlePrep_ResultLabelIdle(ResultLabel* arg0);
void BattlePrep_ResultLabelGrow(ResultLabel* arg0);
void BattlePrep_ResultLabelShrink(ResultLabel* arg0);
void BattlePrep_UpdateResultLabel(ResultLabel* arg0);
void BattlePrep_StartResultLabelGrow(ResultLabel* arg0, DisplayObject* arg1);
void BattlePrep_StartResultLabelShrink(ResultLabel* arg0);
void BattlePrep_InitResultLabel(ResultLabel* arg0, u8 arg1);
void BattlePrep_DrawCursorArrow(BattlePrepRuleWindow* arg0, s16 arg1, s16 arg2);
void BattlePrep_DrawRuleOption(BattlePrepRuleWindow* arg0, s16 arg1, s16 arg2, s16 arg3, s32 arg4, s8* arg5);
void BattlePrep_DrawKeepBattlingPrompt(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1);
void BattlePrep_DrawCupLossMenu(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1);
void BattlePrep_DrawCastleLossMenu(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1);
void BattlePrep_DrawFreeBattleMenu(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1);
void BattlePrep_DrawMewtwoLossMenu(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1);
void BattlePrep_DrawCupProgressHeader(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1);
void BattlePrep_DrawCastleProgressHeader(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1);
void BattlePrep_DrawPlayerTag(s16 arg0, s16 arg1, BattleSessionTeams* arg2);
void BattlePrep_SetWinCountColor(BattleSessionTeams* arg0);
void BattlePrep_DrawWinRecord(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1);
void BattlePrep_DrawOverwriteSavePrompt(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1);
void BattlePrep_DrawRuleWindow(BattlePrepRuleWindow* arg0);
void BattlePrep_RunQuitMenu(BattlePrepRuleWindow* arg0);
void BattlePrep_RuleWindowExpanding(BattlePrepRuleWindow* arg0);
void BattlePrep_RuleWindowCollapsing(BattlePrepRuleWindow* arg0);
void BattlePrep_UpdateRuleWindow(BattlePrepRuleWindow* arg0);
void BattlePrep_InitRuleWindow(BattlePrepRuleWindow* arg0, s16 arg1);
void BattlePrep_CloseRuleWindow(BattlePrepRuleWindow* arg0);
s32 BattlePrep_PollRuleWindow(BattlePrepRuleWindow* arg0);

#endif // _FRAGMENT63_H_
