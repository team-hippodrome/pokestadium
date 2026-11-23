#ifndef _1AB70_H_
#define _1AB70_H_

#include "global.h"
#include "gfx_rect.h"
#include "matrix.h"
#include "game_state.h"

typedef struct unk_func_8001B1FC {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ u16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A;
    /* 0x0C */ u8* unk_0C;
    /* 0x10 */ s16 unk_10;
    /* 0x12 */ s16 unk_12;
    /* 0x14 */ intptr_t unk_14; // BattleMon address or species id, selected by unk_00.
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ ModelLoadContext* unk_1C;
    /* 0x20 */ GraphNode* unk_20;
    /* 0x24 */ DisplayObject* unk_24;
    /* 0x28 */ DepthBuffer* unk_28;
    /* 0x2C */ DepthBuffer* unk_2C;
} unk_func_8001B1FC; // size = 0x30

typedef union unk_func_8001A024_008_014 {
    struct {
        /* 0x00 */ s16 unk_00;
        /* 0x02 */ s16 unk_02;
    };
    u8* ptr;
    u32 raw;
} unk_func_8001A024_008_014; // size = 0x4

typedef struct unk_func_8001A024_008 {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ u16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ DepthBuffer* unk_0C;
    /* 0x10 */ arg1_func_80010CA8 unk_10;
    /* 0x14 */ unk_func_8001A024_008_014 unk_14;
} unk_func_8001A024_008; // size = 0x18

typedef struct unk_func_8001A024 {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ s8 unk_02;
    /* 0x03 */ s8 unk_03;
    /* 0x04 */ ModelLoadContext* unk_04;
    /* 0x08 */ unk_func_8001A024_008* unk_08;
    /* 0x0C */ GraphNode* unk_0C;
    /* 0x10 */ DisplayObject* unk_10;
    /* 0x14 */ DepthBuffer* unk_14;
    /* 0x18 */ unk_D_8006FF00* unk_18;
} unk_func_8001A024; // size = 0x1C

/**
 * Pre-battle versus screen in battle_prep: each side's trainer card and icon columns flip in, then the
 * 3D V/S logo flies into the screen and the camera shakes. Side 0 is the top row, side 1 the bottom row.
 * Used only by src/fragments/battle_prep/.
 */
typedef struct BattlePrepRoster {
    /* 0x00 */ u8 state; // 0 idle, 1 loading icons, 2 icons ready, 3 intro animation running.
    /* 0x01 */ u8 timer; // Frame counter for the loading wait and the intro animation.
    /* 0x02 */ u8 isDoubleLayout; // 1 when a side has a tag team: each side shows two members' icons.
    /* 0x04 */ DisplayObject* displayObjects; // 0/9 trainer cards, 1-4/5-8 icon columns, 10 V/S logo.
    /* 0x08 */ CameraNode* camera;
    /* 0x0C */ GraphNode* side0IconGeo; // Icon columns on the blue D_300C000 / D_300F000 panels.
    /* 0x10 */ GraphNode* side1IconGeo; // Icon columns on the green D_300D800 / D_300FC00 panels.
    /* 0x14 */ GraphNode* side0CardGeo;
    /* 0x18 */ GraphNode* side1CardGeo;
    /* 0x1C */ GraphNode* versusLogoGeo; // 3D "V" and "S" meshes with the scrolling shine texture D_84B11C20.
    /* 0x20 */ unk_func_8001A024* iconRenderList; // PokeIcon render list holding each brought Pokémon's icon image.
    /* 0x24 */ DepthBuffer* side0CardImage; // 64x96 trainer card drawn by BattlePrep_DrawTrainerStatusIcon.
    /* 0x28 */ DepthBuffer* side1CardImage;
    /* 0x2C */ char unk2C[0x4];
} BattlePrepRoster; // size = 0x30

unk_func_8001A024* PokeIcon_CreateRenderList(ModelLoadContext*, s16, s16, s16);
void PokeIcon_AttachDepthBuffer(unk_func_8001A024*, ModelLoadContext*, DepthBuffer*);
void PokeIcon_SetNameTable(unk_func_8001A024*, unk_D_8006FF00*);
void PokeIcon_ResetRenderList(unk_func_8001A024*);
void func_8001A324(unk_func_8001A024*, s32, s32, u16);
void func_8001A46C(unk_func_8001A024*, s32, BattleMon*, u16);
s32 PokeIcon_ProcessNextSlot(unk_func_8001A024*, s32);
s32 PokeIcon_ProcessAllSlots(unk_func_8001A024*, s32);
s32 PokeIcon_RenderNextSlotBlocking(unk_func_8001A024*, s32);
u8* PokeIcon_GetImage(unk_func_8001A024*, s32);
s32 PokeIcon_AreSlotsIdle(unk_func_8001A024*);
void PokeIcon_OpenModelArchive(void);
void PokeIcon_LoadModelTextureForMon(u16*, s32, BattleMon*);
unk_func_8001B1FC* PokeIcon_CreateModelPreview(ModelLoadContext*, s16, s16, s16, s16, s16, s16, u8*);
void PokeIcon_SetPreviewMon(unk_func_8001B1FC* arg0, BattleMon* arg1, u16 arg2);
void PokeIcon_SetPreviewSpecies(unk_func_8001B1FC*, s32, u16);
u8* PokeIcon_RenderPreview(unk_func_8001B1FC*);

#endif // _1AB70_H_
