#ifndef _PARADE_DEBUG_H_
#define _PARADE_DEBUG_H_

#include "global.h"

// Japanese-only debug scene in fragment 15 (the slot the other releases use for the gallery
// camera). Pokemon walk across the floor from either side while the D-pad and C-buttons move the
// camera; START leaves.

typedef struct ParadeSpeciesInfo {
    /* 0x00 */ s32 id;
    /* 0x04 */ s16 archiveIndex;
    /* 0x08 */ f32 scale;
    /* 0x0C */ f32 height;
} ParadeSpeciesInfo; // size = 0x10

typedef struct ParadeModel {
    /* 0x00 */ s32 id;
    /* 0x04 */ ParadeSpeciesInfo* info;
    /* 0x08 */ ModelAnimationBank* model;
} ParadeModel; // size = 0xC

typedef struct ParadeActor {
    /* 0x00 */ s16 state;
    /* 0x02 */ s16 modelIndex;
    /* 0x04 */ s32 id; // 0 while the slot is free
    /* 0x08 */ s16 phase;
    /* 0x0C */ f32 scale;
    /* 0x10 */ f32 speed;
    /* 0x14 */ Vec3f position;
    /* 0x20 */ Vec3s rotation;
    /* 0x28 */ DisplayObject displayObject;
} ParadeActor; // size = 0x190

void Parade_InitActors(void);
void Parade_UpdateActors(void);

#endif // _PARADE_DEBUG_H_
