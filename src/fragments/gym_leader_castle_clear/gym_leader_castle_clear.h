#ifndef _FRAGMENT40_H_
#define _FRAGMENT40_H_

#include "global.h"
#include "src/geo_layout.h"

/**
 * A model on the Gym Leader Castle clear screen: either the "GYM LEADER CASTLE" banner or the "CLEAR" text.
 * The DisplayObject comes first so the geo callbacks can cast the current D_8006F09C back to this type.
 */
typedef struct CastleClearObject {
    /* 0x000 */ DisplayObject displayObject;
    /* 0x168 */ s16 kind;          // 1 = banner (slide-in), 2 = "CLEAR" text (keyframed pop-in)
    /* 0x16A */ s16 animState;     // 0 = waiting, 1 = animating, 2 = finished
    /* 0x16C */ s16 timer;         // Frames left in the current keyframe segment
    /* 0x16E */ s16 duration;      // Frame length of the current keyframe segment
    /* 0x170 */ s16 keyframeIndex; // Index into sClearTextKeyframes
    /* 0x172 */ s16 primR;         // Primitive color (bright texels) for sClearTextDL
    /* 0x174 */ s16 primG;
    /* 0x176 */ s16 primB;
    /* 0x178 */ s16 envR;          // Environment color (dark texels) for sClearTextDL
    /* 0x17A */ s16 envG;
    /* 0x17C */ s16 envB;
    /* 0x180 */ f32 slideOffset;   // Banner offset; x = -520 * slideOffset, settles at 0
    /* 0x184 */ f32 slideVelocity; // Per-frame change of slideOffset
} CastleClearObject; // size = 0x188

void GymLeaderCastleClear_UpdateBannerSlide(DisplayObject* display_object);
s32 GymLeaderCastleClear_GeoBannerCallback(s32 arg0, UNUSED GraphNode* arg1);
void GymLeaderCastleClear_UpdateClearTextKeyframes(CastleClearObject* object);
s32 GymLeaderCastleClear_GeoClearTextCallback(s32 arg0, GraphNode* arg1);
void GymLeaderCastleClear_InitStarWipe(void);
void GymLeaderCastleClear_UpdateStarWipe(void);
void GymLeaderCastleClear_DrawStarWipe(void);
void GymLeaderCastleClear_StartStarWipe(s16 duration);
void GymLeaderCastleClear_InitBackgroundFade(void);
void GymLeaderCastleClear_UpdateBackgroundFade(void);
void GymLeaderCastleClear_DrawTranslucentBackground(u8* texture, s16 alpha);
void GymLeaderCastleClear_DrawBackground(void);
void GymLeaderCastleClear_StartBackgroundFade(s16 mode);
void GymLeaderCastleClear_ReadInputs(void);
void GymLeaderCastleClear_StartObjectAnimation(CastleClearObject* object, s16 state);
void GymLeaderCastleClear_DrawScissoredTextureRect(s16 x1, s16 y1, s16 width, s16 height, s16 texture_start_x,
                                                 s16 texture_start_y, s16 texture_spread_x, s16 texture_spread_y);
void GymLeaderCastleClear_DrawRound2Badge(void);
void GymLeaderCastleClear_InitState(void);
void GymLeaderCastleClear_Draw(void);
s32 GymLeaderCastleClear_AdvanceState(void);
void GymLeaderCastleClear_RunLoop(void);
void GymLeaderCastleClear_InitGraphics(void);
s32 GymLeaderCastleClear_Main(UNUSED s32 arg0, UNUSED s32 arg1);

#endif // _FRAGMENT40_H_
