#ifndef _E1C0_H_
#define _E1C0_H_

#include "global.h"

typedef struct unk_D_800AA664 {
    /* 0x0000 */ SchedClient client;
    /* 0x01E0 */ char stack[0x2000];
} unk_D_800AA664; // size = 0x21E0

extern unk_D_800AA660* gGbTowerEmuThread;

void GbTower_Start(GbTowerSessionParams* params);
OSMesg* GbTower_RecvExitMesg(void);
void GbTower_Stop(GbTowerSessionParams* params);

#endif // _E1C0_H_
