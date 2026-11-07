#ifndef _E1C0_H_
#define _E1C0_H_

#include "global.h"

typedef struct unk_D_800AA664 {
    /* 0x0000 */ OSThread thread;
    /* 0x01B0 */ char unk01B0[0x2030];
} unk_D_800AA664; // size = 0x21E0

extern unk_D_800AA660* gGbTowerEmuThread;

void GbTower_Start(GbTowerSessionParams* arg0);
OSMesg* GbTower_RecvExitMesg(void);
void GbTower_Stop(GbTowerSessionParams* arg0);

#endif // _E1C0_H_
