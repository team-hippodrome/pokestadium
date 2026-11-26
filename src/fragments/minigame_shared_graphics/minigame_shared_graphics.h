#ifndef _FRAGMENT4_H_
#define _FRAGMENT4_H_

#include "global.h"

extern u32 D_87A01110[];
extern u32 D_87A01178[];
extern u32 D_87A01180[];
extern u32 D_87A01190[];
extern u32 D_87A011A4[];

// EUC-JP labels packed into D_87A01110, used by the Japanese minigame tutorials.
#define MINIGAME_SHARED_TEXT_HOW_TO_PLAY ((char*)&D_87A01110[0])  // "ゲームのせつめい" "How to play"
#define MINIGAME_SHARED_TEXT_A_BUTTON ((char*)&D_87A01110[5])     // "Ａボタン" "A Button"
#define MINIGAME_SHARED_TEXT_CONTROL_PAD ((char*)&D_87A01110[8])  // "じゅうじキー" "Control Pad"
#define MINIGAME_SHARED_TEXT_START_GAME ((char*)&D_87A01110[12])  // "ゲームをはじめる" "Start the game"
#define MINIGAME_SHARED_TEXT_JUMP ((char*)&D_87A01110[17])        // "ジャンプ" "Jump"
#define MINIGAME_SHARED_TEXT_ELLIPSIS ((char*)&D_87A01110[20])    // "・・・"
#define MINIGAME_SHARED_TEXT_GOT_IT ((char*)&D_87A01110[22])      // "ゲットだぜ！" "Got it!"

void MiniCam_SetActive(CameraNode*);
Gfx* MiniCam_ApplyProjection(Gfx*);
Gfx* MiniGfx_DrawTexturedQuad(Gfx*, Mtx*, f32, f32, f32, f32, void*, s32, s32);
void func_87A00DB8(s32);
s32 MiniSound_DispatchCommand(s32, s32, s32);

#endif // _FRAGMENT4_H_
