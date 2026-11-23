#ifndef _FRAGMENT44_H_
#define _FRAGMENT44_H_

#include "global.h"
#include "src/gallery.h"

s32 Gallery_AlbumHandleGridInput(unk_D_83407B38* arg0);
void Gallery_AlbumInit(s32 arg0, s32 arg1);
void Gallery_AlbumDrawTileColumn(s32 arg0, s32 arg1);
void Gallery_AlbumDrawFilledPanel(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void Gallery_AlbumDrawEdgeBand(s32 arg0, s32 arg1, s32 arg2);
void Gallery_AlbumDrawCornerMarkers(s32 arg0, s32 arg1, s32 arg2);
void func_83500FE8(unk_D_83407B38* arg0, s32 arg1, s32 arg2);
void Gallery_AlbumDrawThumbnailFrame(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void Gallery_AlbumDrawStickerBadge(s32 arg0, s32 arg1, u16 arg2);
void Gallery_AlbumComputeEnlargeMotion(s32 arg0, s32* arg1, s32* arg2, f32* arg3);
void func_83501718(u16* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, f32 arg5);
void Gallery_AlbumDrawThumbnails(s32 arg0, s32 arg1, unk_D_83407B38* arg2);
void Gallery_AlbumDrawTitleBar(unk_D_83407B38* arg0, s32 arg1, s32 arg2);
void Gallery_AlbumDrawEnlargeHint(s32 arg0);
void Gallery_AlbumDrawGridPanel(s32 arg0, s32 arg1, unk_D_83407B38* arg2);
void Gallery_AlbumDrawGradientBackground(void);
void Gallery_AlbumDrawAnimatedOverlay(s32 arg0, s32 arg1, s32 arg2);
void Gallery_AlbumDrawExitButton(int arg0, int arg1, s32 arg2, f32 arg3);
char* Gallery_AlbumGetStatusText(unk_D_83407B38* arg0);
void Gallery_AlbumDrawStatusText(unk_D_83407B38* arg0, s32 arg1, s32 arg2);
f32 Gallery_AlbumPulseAlpha(s32* arg0, u32* arg1, s32 arg2);
void Gallery_AlbumDrawCursor(unk_D_83407B38* arg0);
void Gallery_AlbumDraw(void);
void Gallery_AlbumAdvanceDeleteAnimation(unk_D_83407B38* arg0);
void Gallery_AlbumConfirmAndExit(unk_D_83407B38* arg0);
s32 Gallery_AlbumUpdate(void);
void Gallery_AlbumReadInput(void);
void Gallery_AlbumLoop(void);
s32 Gallery_AlbumGetResult(unk_D_83407B38* arg0);
s32 Gallery_AlbumMain(s32 arg0, s32 arg1);

#endif // _FRAGMENT44_H_
