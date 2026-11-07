#ifndef _FRAGMENT50_H_
#define _FRAGMENT50_H_

#include "global.h"

extern char** gVictoryPokemonNames;

void VictoryPalace_CheckAllCaught(void);
s32 VictoryPalace_StringsEqual(s8* arg0, s8* arg1);
s32 VictoryPalace_IsCurrentSpeciesOwnedByPlayer(void);
Gfx* VictoryPalace_DrawFrameBorder(Gfx* arg0);
s32 VictoryPalace_SpeciesObtained(s32 arg0);
s32 VictoryPalace_UpdateLeftPillarGlow(s32 arg0, unk_D_86002F34_alt18* arg1);
s32 VictoryPalace_UpdateRightPillarGlow(s32 arg0, unk_D_86002F34_alt18* arg1);
void func_82600C00(void);
void func_82600C08(void);
void VictoryPalace_InitCamera(void);
void func_82600CFC(void);
void VictoryPalace_PollInput(void);
void VictoryPalace_DrawSpeciesIcon(s32 arg0, s32 arg1, f32 arg2, s32 arg3);
void VictoryPalace_DrawSpeciesGrid(void);
void VictoryPalace_DrawOverlay(void);
void VictoryPalace_DrawFrame(void);
void VictoryPalace_InitCarouselModel(s16 arg0);
void VictoryPalace_SpawnCarouselEntry(s16 arg0);
void func_82602570(void);
void VictoryPalace_ClearCarouselSlot(s16 arg0);
void VictoryPalace_RequestCarouselIconLoad(s16 arg0);
void VictoryPalace_LoadCurrentCarouselIcon(s16 arg0);
void VictoryPalace_ScrollCarousel(f32 arg0);
void VictoryPalace_SetBackgroundPositionX(f32 arg0);
void func_826028CC(void);
void VictoryPalace_UpdateCarouselModelRotation(void);
void VictoryPalace_ShowSpeciesInfo(void);
void VictoryPalace_ShowSpeciesGrid(void);
void VictoryPalace_HideSpeciesGrid(void);
void VictoryPalace_HideOverlays(void);
void VictoryPalace_ResetPillarGlow(s32 arg0);
s32 VictoryPalace_HandleInput(s16* arg0);
s32 HallOfFame_HandleGalleryInput(void);
s32 HallOfFame_ScrollRight(void);
s32 HallOfFame_ScrollLeft(void);
s32 HallOfFame_PageRight(void);
s32 HallOfFame_PageLeft(void);
s32 HallOfFame_WaitFramebuffer(void);
void HallOfFame_RunGallery(void);
void HallOfFame_SpawnPokeIcon(s32 arg0, f32 arg1);
void HallOfFame_InitGallery(void);
s32 HallOfFame_Main(s32 arg0, s32 arg1);

#endif // _FRAGMENT50_H_
