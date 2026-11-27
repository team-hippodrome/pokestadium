#include "battle_engine.h"
#include "src/matrix.h"
#include "src/math_util.h"

// JP-only unit: the other versions build these functions at the end of
// //@src/fragments/battle_engine/battle_engine_animation_system.c. The JP ROM pads
// BattleAnim_DispatchPresentation with nops to a 16-byte boundary before
// BattleAnim_Initialize, which marks the start of a separate object file.
#if defined(VERSION_JP)
extern s32 sD_843902BC;
extern s8 sBattleAnimEffectCallbackIndex;
extern f32 sD_84390344;
extern Vec3s sD_84390354;
extern Vec3f sD_84390360;
extern Vec3s sD_8439036C;
extern Vec3s sD_84390374;
extern s32 sD_843903B0;
extern s32 sD_843903B4;
extern unk_D_84392040 sD_84392040;

#include "battle_engine_animation_system_camera.inc.c"
#endif
