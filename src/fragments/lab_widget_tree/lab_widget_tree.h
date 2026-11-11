#ifndef _FRAGMENT26_H_
#define _FRAGMENT26_H_

#include "global.h"
#include "src/memory.h"
#include "src/memmap.h"
#include "src/controller.h"
#include "src/game_state.h"
#include "src/poke_icon.h"
#include "src/table_view.h"
#include "src/fragments/pokemon_lab_support/pokemon_lab_support.h"

struct WidgetMainMenu;
struct WidgetNode;
struct WidgetLinkHeader;
struct WidgetGridMenu;
struct unk_func_88203ED8;
struct unk_func_88203ED8_064;
struct unk_func_882025E0;
struct unk_func_8850CD44;
struct unk_func_8830867C_02C_048_000;
struct unk_func_8850BD40;
struct unk_func_88200FA0_030_030;
struct unk_func_8821421C_038_034;

typedef s32 (*WidgetDrawCallback)(struct WidgetNode*, s32, s32);
typedef s32 (*WidgetUpdateCallback)(struct WidgetNode*);
typedef s32 (*WidgetInputCallback)(struct WidgetNode*, Controller*);
typedef void (*WidgetSetStateCallback)(struct WidgetNode*, s32);
typedef void (*WidgetDestructorCallback)(struct WidgetLinkHeader*, MemoryPool*);

typedef struct WidgetLinkHeader {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ struct WidgetLinkHeader* unk_04;
    /* 0x08 */ struct WidgetLinkHeader* unk_08;
    /* 0x0C */ WidgetDestructorCallback unk_0C;
} WidgetLinkHeader; // size = 0x10

typedef struct WidgetPoint {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
} WidgetPoint; // size = 0x4

typedef struct WidgetNode {
    /* 0x00 */ WidgetLinkHeader unk_00;
    /* 0x10 */ WidgetPoint unk_10;
    /* 0x14 */ WidgetPoint unk_14;
    /* 0x18 */ WidgetDrawCallback unk_18;
    /* 0x1C */ WidgetUpdateCallback unk_1C;
    /* 0x20 */ WidgetInputCallback unk_20;
    /* 0x24 */ WidgetSetStateCallback unk_24;
    /* 0x28 */ u16 unk_28;
    /* 0x2A */ u16 unk_2A;
} WidgetNode; // size = 0x2C

typedef struct WidgetDelayedNode {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ s32 unk_2C;
} WidgetDelayedNode; // size = 0x30

typedef struct WidgetAnimatedPanelVariantB {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ Color_RGBA8 unk_2C;
    /* 0x30 */ s32 unk_30;
    /* 0x34 */ s32 unk_34;
    /* 0x38 */ char unk38[0x4];
} WidgetAnimatedPanelVariantB; // size = 0x3C

typedef struct WidgetAnimatedPanel {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ s32 unk_30;
} WidgetAnimatedPanel; // size = 0x34

typedef struct WidgetTextList {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ WidgetAnimatedPanel* unk_2C;
    /* 0x30 */ s32 unk_30;
    /* 0x34 */ u8 unk_34;
    /* 0x35 */ s8 unk_35;
    /* 0x36 */ u8 unk_36;
    /* 0x37 */ Color_RGBA8 unk_38;
    /* 0x3C */ FontContext* unk_3C;
    /* 0x40 */ s32 unk_40;
} WidgetTextList; // size = 0x44

typedef struct WidgetAnimatedFrame {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ Color_RGBA8 unk_2C;
    /* 0x30 */ s32 unk_30;
} WidgetAnimatedFrame; // size = 0x34

typedef s32 (*WidgetGridMenuInputCallback)(struct WidgetGridMenu*, Controller*);
typedef void (*WidgetGridMenuSetStateCallback)(struct WidgetGridMenu*, s32);

typedef struct WidgetGridMenu {
    /* 0x00 */ char unk00[0xC];
    /* 0x0C */ WidgetDestructorCallback unk_0C;
    /* 0x10 */ WidgetGridMenuInputCallback unk_10;
    /* 0x14 */ WidgetGridMenuSetStateCallback unk_14;
    /* 0x18 */ WidgetNode** unk_18;
    /* 0x1C */ WidgetAnimatedFrame* unk_1C;
    /* 0x20 */ s32 unk_20;
    /* 0x24 */ s32 unk_24;
    /* 0x28 */ s32 unk_28;
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ s32 unk_30;
    /* 0x34 */ s16 unk_34;
    /* 0x36 */ s16 unk_36;
    /* 0x38 */ s16 unk_38;
    /* 0x3A */ s16 unk_3A;
    /* 0x3C */ s16 unk_3C;
    /* 0x3E */ s16 unk_3E;
} WidgetGridMenu; // size = 0x40

typedef struct WidgetMainMenu {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ WidgetDelayedNode* unk_2C;
    /* 0x30 */ WidgetAnimatedPanelVariantB* unk_30;
    /* 0x34 */ WidgetTextList* unk_34;
    /* 0x38 */ WidgetAnimatedPanel* unk_38;
    /* 0x3C */ WidgetGridMenu* unk_3C;
    /* 0x40 */ s32 unk_40;
    /* 0x44 */ char unk44[0xC];
    /* 0x50 */ s32 unk_50;
    /* 0x54 */ s32 unk_54;
} WidgetMainMenu; // size = 0x58

typedef struct unk_func_88500E34 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ u8* unk_2C;
} unk_func_88500E34; // size = 0x30

typedef struct unk_func_88509A2C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ s32 unk_30;
    /* 0x34 */ s32 unk_34;
    /* 0x38 */ s32 unk_38;
    /* 0x3C */ char* unk_3C;
    /* 0x40 */ s8 unk_40[12];
} unk_func_88509A2C; // size = 0x4C

typedef struct unk_func_88500EE4 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ u8* unk_2C;
    /* 0x30 */ Color_RGBA8 unk_30;
    /* 0x34 */ Color_RGBA8 unk_34;
} unk_func_88500EE4; // size = 0x38

typedef struct unk_func_885088F4 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ s32 unk_30;
    /* 0x34 */ s32 unk_34;
    /* 0x38 */ s32 unk_38;
} unk_func_885088F4; // size = 0x3C

typedef struct unk_func_88509F0C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ char* unk_2C;
} unk_func_88509F0C; // size = 0x30

typedef struct unk_func_88509E34 {
    /* 0x00 */ unk_func_88509F0C unk_00;
    /* 0x30 */ char unk30[0x4];
} unk_func_88509E34; // size = 0x34

typedef struct unk_func_8850B254 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ Color_RGBA8 unk_2C;
    /* 0x30 */ Color_RGBA8 unk_30;
} unk_func_8850B254; // size = 0x34

typedef struct unk_func_8850878C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ Color_RGBA8 unk_30;
    /* 0x34 */ Color_RGBA8 unk_34;
    /* 0x38 */ s32 unk_38;
    /* 0x3C */ s32 unk_3C;
    /* 0x40 */ char* unk_40;
    /* 0x44 */ s32 unk_44;
} unk_func_8850878C; // size = 0x48

typedef struct unk_func_885012A4 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ Color_RGBA8 unk_2C;
} unk_func_885012A4; // size = 0x30

typedef struct unk_func_88503298 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ WidgetAnimatedPanel* unk_2C;
    /* 0x30 */ unk_func_8850878C* unk_30;
} unk_func_88503298; // size = 0x34

typedef void (*unk_func_88200FA0_030_030_040)(s32, s32, char*, s32, s32, struct unk_func_88200FA0_030_030*);

typedef struct unk_func_88200FA0_030_030 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ unk_func_8820BE14_06C* unk_2C;
    /* 0x30 */ s32 unk_30;
    /* 0x34 */ u16* unk_34;
    /* 0x38 */ s32 unk_38;
    /* 0x3C */ s16 unk_3C;
    /* 0x3E */ s16 unk_3E;
    /* 0x40 */ unk_func_88200FA0_030_030_040 unk_40;
    /* 0x44 */ WidgetAnimatedFrame* unk_44;
    /* 0x48 */ s16 unk_48;
    /* 0x4A */ s16 unk_4A;
    /* 0x4C */ s16 unk_4C;
    /* 0x4E */ s16 unk_4E;
    /* 0x50 */ s16 unk_50;
    /* 0x52 */ s16 unk_52;
} unk_func_88200FA0_030_030; // size = 0x54

// Copy of above with unk_2C changed just for fragment23_1AE680
typedef struct unk_func_88200FA0_030_030_1AE680 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ unk_func_88205880_A030* unk_2C;
    /* 0x30 */ s32 unk_30;
    /* 0x34 */ u16* unk_34;
    /* 0x38 */ s32 unk_38;
    /* 0x3C */ s16 unk_3C;
    /* 0x3E */ s16 unk_3E;
    /* 0x40 */ unk_func_88200FA0_030_030_040 unk_40;
    /* 0x44 */ WidgetAnimatedFrame* unk_44;
    /* 0x48 */ s16 unk_48;
    /* 0x4A */ s16 unk_4A;
    /* 0x4C */ s16 unk_4C;
    /* 0x4E */ s16 unk_4E;
    /* 0x50 */ s16 unk_50;
    /* 0x52 */ s16 unk_52;
} unk_func_88200FA0_030_030_1AE680; // size = 0x54

// Copy of above with unk_2C changed just for fragment24_1CEA00
typedef struct unk_func_88200FA0_030_030_1CEA00 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ unk_func_8830867C_02C_0CC* unk_2C;
    /* 0x30 */ s32 unk_30;
    /* 0x34 */ u16* unk_34;
    /* 0x38 */ s32 unk_38;
    /* 0x3C */ s16 unk_3C;
    /* 0x3E */ s16 unk_3E;
    /* 0x40 */ unk_func_88200FA0_030_030_040 unk_40;
    /* 0x44 */ WidgetAnimatedFrame* unk_44;
    /* 0x48 */ s16 unk_48;
    /* 0x4A */ s16 unk_4A;
    /* 0x4C */ s16 unk_4C;
    /* 0x4E */ s16 unk_4E;
    /* 0x50 */ s16 unk_50;
    /* 0x52 */ s16 unk_52;
} unk_func_88200FA0_030_030_1CEA00; // size = 0x54

typedef struct unk_func_8850182C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ Color_RGBA8 unk_2C;
} unk_func_8850182C; // size = 0x30

typedef struct unk_func_889000C4_02C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ s32 unk_30;
} unk_func_889000C4_02C; // size = 0x34

typedef struct unk_func_889000C4_044_02C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ char unk30[0x4];
} unk_func_889000C4_044_02C; // size = 0x34

typedef struct unk_func_88500994 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ s32 unk_30;
} unk_func_88500994; // size = 0x34

typedef struct unk_func_889000C4_044 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ unk_func_889000C4_044_02C* unk_2C;
    /* 0x30 */ unk_func_88500994* unk_30;
    /* 0x34 */ unk_func_8850878C* unk_34;
    /* 0x38 */ unk_func_8850878C* unk_38;
} unk_func_889000C4_044; // size = 0x3C

typedef struct unk_func_889000C4 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ unk_func_889000C4_02C* unk_2C;
    /* 0x30 */ unk_func_88200FA0_030_030* unk_30;
    /* 0x34 */ unk_func_88200FA0_030_038 unk_34;
    /* 0x40 */ unk_func_88500994* unk_40;
    /* 0x44 */ unk_func_889000C4_044* unk_44;
} unk_func_889000C4; // size = 0x48

typedef struct unk_func_8850143C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ Color_RGBA8 unk_2C;
    /* 0x30 */ Color_RGBA8 unk_30;
} unk_func_8850143C; // size = 0x34

typedef struct unk_func_8820399C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ unk_func_8850143C* unk_30;
} unk_func_8820399C; // size = 0x34

typedef struct unk_func_88509F48 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ WidgetAnimatedPanel* unk_2C;
    /* 0x30 */ unk_func_885012A4* unk_30;
    /* 0x34 */ unk_func_8850878C* unk_34;
    /* 0x38 */ FontContext* unk_38;
} unk_func_88509F48; // size = 0x3C

typedef s32 (*unk_func_8850CD44_010)(struct unk_func_8850CD44* arg0, Controller* arg1);
typedef void (*unk_func_8850CD44_014)(struct unk_func_8850CD44* arg0, s32 arg1);

typedef struct unk_func_882173EC {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ WidgetPoint unk_30;
    /* 0x34 */ WidgetPoint unk_34;
} unk_func_882173EC; // size = 0x38

typedef struct unk_func_8850CD44 {
    /* 0x00 */ char unk00[0x10];
    /* 0x10 */ unk_func_8850CD44_010 unk_10;
    /* 0x14 */ unk_func_8850CD44_014 unk_14;
    /* 0x18 */ s32 unk_18;
    /* 0x1C */ unk_func_882173EC** unk_1C;
    /* 0x20 */ s32 unk_20;
} unk_func_8850CD44; // size = 0x24

typedef struct unk_func_882025E0_03C {
    /* 0x00 */ unk_func_8850878C* unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
} unk_func_882025E0_03C; // size = 0xC

typedef s32 (*unk_func_882025E0_010)(struct unk_func_882025E0* arg0, Controller* arg1);
typedef void (*unk_func_882025E0_014)(struct unk_func_882025E0* arg0, s32 arg1);

typedef struct unk_func_882025E0_024_02C {
    /* 0x00 */ unk_func_88205880_00D0* unk_00[2];
    /* 0x08 */ s32 unk_08;
} unk_func_882025E0_024_02C; // size >= 0xC

typedef struct unk_func_882025E0_024_02C_wrap {
    /* 0x00 */ unk_func_882025E0_024_02C* unk_00;
} unk_func_882025E0_024_02C_wrap; // size >= 0xC

typedef struct unk_func_882025E0_024 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ unk_func_882025E0_024_02C_wrap* unk_2C;
    /* 0x30 */ s32 unk_30;
    /* 0x34 */ u16* unk_34;
    /* 0x38 */ s32 unk_38;
    /* 0x3C */ s16 unk_3C;
    /* 0x3E */ s16 unk_3E;
    /* 0x40 */ unk_func_88200FA0_030_030_040 unk_40;
    /* 0x44 */ WidgetAnimatedFrame* unk_44;
    /* 0x48 */ s16 unk_48;
    /* 0x4A */ s16 unk_4A;
    /* 0x4C */ s16 unk_4C;
    /* 0x4E */ s16 unk_4E;
    /* 0x50 */ s16 unk_50;
    /* 0x52 */ s16 unk_52;
} unk_func_882025E0_024; // size >= 0x54

typedef struct unk_func_882025E0_000 {
    /* 0x00 */ WidgetLinkHeader unk_00;
    /* 0x10 */ unk_func_882025E0_010 unk_10;
    /* 0x14 */ unk_func_882025E0_014 unk_14;
    /* 0x18 */ s32 unk_18;
    /* 0x1C */ s32 unk_1C;
    /* 0x20 */ struct unk_func_88203ED8* unk_20;
} unk_func_882025E0_000; // size = 0x24

typedef struct unk_func_882025E0 {
    /* 0x00 */ unk_func_882025E0_000 unk_00;
    /* 0x24 */ unk_func_88200FA0_030_030* unk_24[2]; // unk_func_882025E0_024
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ s32 unk_30;
    /* 0x34 */ s32 unk_34;
    /* 0x38 */ s32 unk_38;
    /* 0x3C */ unk_func_882025E0_03C unk_3C[5];
} unk_func_882025E0; // size = 0x78

typedef struct unk_func_882025E0_1A9780 {
    /* 0x00 */ unk_func_882025E0_000 unk_00;
    /* 0x24 */ unk_func_88200FA0_030_030_1AE680* unk_24[2];
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ s32 unk_30;
    /* 0x34 */ s32 unk_34;
    /* 0x38 */ s32 unk_38;
    /* 0x3C */ unk_func_882025E0_03C unk_3C[5];
} unk_func_882025E0_1A9780; // size = 0x78

typedef struct unk_func_88205880_0098_008_040_02C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ s32 unk_2C;
} unk_func_88205880_0098_008_040_02C; // size = 0x30

typedef struct unk_func_88205880_0098_008_040_03C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ s32 unk_2C;
} unk_func_88205880_0098_008_040_03C; // size = 0x30

typedef struct unk_func_88205880_0098_008_040_05C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ s32 unk_30;
} unk_func_88205880_0098_008_040_05C; // size = 0x34

typedef struct unk_func_88205880_0098_008_040_06C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ WidgetPoint unk_30;
    /* 0x34 */ WidgetPoint unk_34;
} unk_func_88205880_0098_008_040_06C; // size = 0x38

typedef struct unk_func_88205880_0098_008_040 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ unk_func_88205880_0098_008_040_02C* unk_2C[4];
    /* 0x3C */ unk_func_88205880_0098_008_040_03C* unk_3C[4];
    /* 0x4C */ unk_func_88205880_0098_008_040_06C* unk_4C[4];
    /* 0x5C */ unk_func_88205880_0098_008_040_05C* unk_5C[4];
    /* 0x6C */ unk_func_88205880_0098_008_040_06C* unk_6C[4];
    /* 0x7C */ unk_func_885012A4* unk_7C[4];
} unk_func_88205880_0098_008_040; // size = 0x8C

typedef struct unk_func_88205880_0098_008_044 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ BattleMon* unk_2C;
} unk_func_88205880_0098_008_044; // size = 0x30

typedef struct unk_func_88205880_0098_008_048 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ BattleMon* unk_2C;
} unk_func_88205880_0098_008_048; // size = 0x30

typedef struct unk_func_88205880_0098_008_04C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ BattleMon* unk_2C;
} unk_func_88205880_0098_008_04C; // size = 0x30

typedef struct unk_func_88205880_0098_008_050 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ BattleMon* unk_30;
    /* 0x34 */ unk_func_8001B1FC* unk_34;
} unk_func_88205880_0098_008_050; // size = 0x38

typedef struct unk_func_88205880_0098_008_054 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ BattleMon* unk_2C;
} unk_func_88205880_0098_008_054; // size = 0x30

typedef struct unk_func_88205880_0098_008_058 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ s32 unk_2C;
} unk_func_88205880_0098_008_058; // size = 0x30

typedef struct unk_func_88205880_0098_008_070 {
    /* 0x00 */ WidgetGridMenu unk_00;
    /* 0x40 */ s32 unk_40;
} unk_func_88205880_0098_008_070; // size = 0x44

typedef struct unk_func_88205880_0098_008 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ WidgetAnimatedPanel* unk_2C;
    /* 0x30 */ WidgetDelayedNode* unk_30;
    /* 0x34 */ WidgetDelayedNode* unk_34;
    /* 0x38 */ WidgetDelayedNode* unk_38;
    /* 0x3C */ WidgetDelayedNode* unk_3C;
    /* 0x40 */ unk_func_88205880_0098_008_040* unk_40;
    /* 0x44 */ unk_func_88205880_0098_008_044* unk_44;
    /* 0x48 */ unk_func_88205880_0098_008_048* unk_48;
    /* 0x4C */ unk_func_88205880_0098_008_04C* unk_4C;
    /* 0x50 */ unk_func_88205880_0098_008_050* unk_50;
    /* 0x54 */ unk_func_88205880_0098_008_054* unk_54;
    /* 0x58 */ unk_func_88205880_0098_008_058* unk_58;
    /* 0x5C */ unk_func_88500994* unk_5C;
    /* 0x60 */ unk_func_88500994* unk_60;
    /* 0x64 */ unk_func_88500994* unk_64;
    /* 0x68 */ unk_func_8850182C* unk_68;
    /* 0x6C */ WidgetGridMenu* unk_6C;
    /* 0x70 */ unk_func_88205880_0098_008_070* unk_70;
    /* 0x74 */ unk_func_8850CD44* unk_74;
    /* 0x78 */ BattleMon* unk_78;
    /* 0x7C */ s32 unk_7C;
    /* 0x80 */ s32 unk_80;
    /* 0x84 */ s32 unk_84;
    /* 0x88 */ s32 unk_88;
    /* 0x8C */ s32 unk_8C;
} unk_func_88205880_0098_008; // size = 0x90

typedef struct unk_func_88205880_0098 {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ MemoryPool* unk_04;
    /* 0x08 */ unk_func_88205880_0098_008* unk_08;
    /* 0x0C */ BattleMon* unk_0C;
    /* 0x10 */ s32 unk_10;
    /* 0x14 */ WidgetNode* unk_14;
    /* 0x18 */ s32 unk_18;
    /* 0x1C */ char** unk_1C;
    /* 0x20 */ char** unk_20;
    /* 0x24 */ char** unk_24;
    /* 0x28 */ char** unk_28;
    /* 0x2C */ char** unk_2C;
    /* 0x30 */ char** unk_30;
    /* 0x34 */ char** unk_34;
} unk_func_88205880_0098; // size = 0x38

typedef struct unk_func_88201DA0 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ WidgetAnimatedPanel* unk_2C;
    /* 0x30 */ unk_func_88200FA0_030_030* unk_30;
    /* 0x34 */ unk_func_88201DA0_034* unk_34;
    /* 0x38 */ unk_func_88201DA0_038 unk_38;
    /* 0x48 */ FragmentEntry unk_48;
    /* 0x4C */ unk_func_88205880_0098* unk_4C;
    /* 0x50 */ unk_func_8850878C* unk_50;
    /* 0x54 */ char* unk_54;
    /* 0x58 */ unk_func_8850182C* unk_58;
    /* 0x5C */ unk_func_88205880_00D0* unk_5C;
    /* 0x60 */ u32 unk_60;
} unk_func_88201DA0; // size = 0x64

typedef struct unk_func_8820253C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ unk_func_88205880_00D0* unk_2C;
} unk_func_8820253C; // size = 0x30

typedef struct unk_func_88217740 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ s32 unk_30;
} unk_func_88217740; // size = 0x34

typedef struct unk_func_882149A0 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ u8* unk_2C;
    /* 0x30 */ s16 unk_30;
    /* 0x32 */ Color_RGBA8 unk_32;
    /* 0x36 */ Color_RGBA8 unk_36;
    /* 0x3A */ Color_RGBA8 unk_3A;
    /* 0x40 */ s32 unk_40;
} unk_func_882149A0; // size = 0x44

typedef s32 (*unk_func_8830867C_02C_048_000_010)(struct unk_func_8830867C_02C_048_000*, Controller*);
typedef void (*unk_func_8830867C_02C_048_000_014)(struct unk_func_8830867C_02C_048_000*, s32);

typedef struct unk_func_8830867C_02C_048_000 {
    /* 0x00 */ WidgetLinkHeader unk_00;
    /* 0x10 */ unk_func_8830867C_02C_048_000_010 unk_10;
    /* 0x14 */ unk_func_8830867C_02C_048_000_014 unk_14;
    /* 0x18 */ s32 unk_18;
    /* 0x1C */ s32 unk_1C;
    /* 0x20 */ s32 unk_20;
} unk_func_8830867C_02C_048_000; // size = 0x24

typedef struct unk_func_88203ED8_064 {
    /* 0x00 */ unk_func_8830867C_02C_048_000 unk_00;
    /* 0x24 */ struct unk_func_88203ED8* unk_24;
} unk_func_88203ED8_064; // size = 0x28

typedef struct unk_func_88203ED8 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ WidgetAnimatedPanel* unk_2C;
    /* 0x30 */ unk_func_8820399C* unk_30;
    /* 0x34 */ unk_func_8820399C* unk_34;
    /* 0x38 */ unk_func_88200FA0_030_030* unk_38;
    /* 0x3C */ unk_func_88200FA0_030_030* unk_3C;
    /* 0x40 */ unk_func_88217740* unk_40;
    /* 0x44 */ unk_func_882149A0* unk_44[5];
    /* 0x58 */ unk_func_88500994* unk_58;
    /* 0x5C */ unk_func_88201DA0* unk_5C;
    /* 0x60 */ unk_func_88509F48* unk_60;
    /* 0x64 */ unk_func_88203ED8_064* unk_64;
    /* 0x68 */ WidgetGridMenu* unk_68;
    /* 0x6C */ unk_func_882025E0* unk_6C;
    /* 0x70 */ unk_func_8850CD44* unk_70;
    /* 0x74 */ unk_func_882173EC* unk_74[2];
    /* 0x7C */ unk_func_8820253C* unk_7C[2];
    /* 0x84 */ unk_func_889000C4* unk_84;
} unk_func_88203ED8; // size = 0x88

typedef struct unk_func_8820BE14_02C_038_084 {
    /* 0x00 */ WidgetNode unk_00;
} unk_func_8820BE14_02C_038_084; // size >= 0x18

typedef struct unk_func_8820BE14_02C_038 {
    /* 0x00 */ unk_func_88200FA0_030_030 unk_00; // unk_func_882025E0_024
    /* 0x54 */ u16 unk_54;
    /* 0x56 */ u16 unk_56;
    /* 0x58 */ s32 unk_58;
    /* 0x5C */ s32 unk_5C;
    /* 0x60 */ s32 unk_60;
    /* 0x64 */ s32 unk_64;
    /* 0x68 */ s32 unk_68;
    /* 0x6C */ s32 unk_6C;
    /* 0x70 */ s32 unk_70;
    /* 0x74 */ WidgetUpdateCallback unk_74;
    /* 0x78 */ WidgetDrawCallback unk_78;
    /* 0x7C */ WidgetInputCallback unk_7C;
    /* 0x80 */ WidgetDrawCallback unk_80;
    /* 0x84 */ unk_func_8820BE14_02C_038_084* unk_84;
    /* 0x88 */ s32* unk_88;
    /* 0x8C */ u32 unk_8C;
} unk_func_8820BE14_02C_038; // size = 0x90

typedef struct unk_func_88507D4C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ struct unk_func_8821421C_038_034* unk_2C;
    /* 0x30 */ s32 unk_30;
} unk_func_88507D4C; // size = 0x34

typedef struct unk_func_8821421C_038_034 {
    /* 0x00 */ unk_func_88200FA0_030_030 unk_00;
    /* 0x54 */ u16 unk_54;
    /* 0x56 */ u16 unk_56;
    /* 0x58 */ s32 unk_58;
    /* 0x5C */ s32 unk_5C;
    /* 0x60 */ s32 unk_60;
    /* 0x64 */ s32 unk_64;
    /* 0x68 */ s32 unk_68;
    /* 0x6C */ s32 unk_6C;
} unk_func_8821421C_038_034; // size = 0x70

typedef s32 (*unk_func_8850BD40_010)(struct unk_func_8850BD40*);
typedef void (*unk_func_8850BD40_014)(struct unk_func_8850BD40*);

typedef struct unk_func_8850BD40 {
    /* 0x00 */ WidgetLinkHeader unk_00;
    /* 0x10 */ unk_func_8850BD40_010 unk_10;
    /* 0x14 */ unk_func_8850BD40_014 unk_14;
    /* 0x18 */ char unk18[0x4];
    /* 0x1C */ unk_func_8821421C_038_034* unk_1C;
} unk_func_8850BD40; // size = 0x20

typedef struct unk_func_8821421C_038_050 {
    /* 0x00 */ char unk00[0x8];
    /* 0x08 */ char unk_08[4];
    /* 0x0C */ char unk0C[0x8];
} unk_func_8821421C_038_050; // size = 0x14

typedef struct unk_func_8820E99C_030_044 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ char unk2C[0x4];
    /* 0x30 */ s32 unk_30;
} unk_func_8820E99C_030_044; // size = 0x34

struct unk_func_8830867C_02C;

typedef struct unk_func_8830867C_02C_034 {
    /* 0x00 */ unk_func_88200FA0_030_030_1CEA00 unk_00;
    /* 0x54 */ s16 unk_54;
    /* 0x58 */ s32 unk_58;
    /* 0x5C */ s32 unk_5C;
    /* 0x60 */ char unk60[0xC];
    /* 0x6C */ s32 unk_6C;
    /* 0x70 */ struct unk_func_8830867C_02C* unk_70;
} unk_func_8830867C_02C_034; // size = 0x74

typedef struct unk_func_8830867C_02C_048 {
    /* 0x00 */ unk_func_8830867C_02C_048_000 unk_00;
    /* 0x24 */ struct unk_func_8830867C_02C* unk_24;
} unk_func_8830867C_02C_048; // size = 0x28

typedef struct unk_func_8830867C_034_038 {
    /* 0x00 */ unk_func_8830867C_02C_048_000 unk_00;
    /* 0x24 */ struct unk_func_8830867C_034* unk_24;
} unk_func_8830867C_034_038; // size = 0x28

typedef struct unk_func_8830867C_044_040 {
    /* 0x00 */ unk_func_8830867C_02C_048_000 unk_00;
    /* 0x24 */ struct unk_func_8830867C_044* unk_24;
} unk_func_8830867C_044_040; // size = 0x28

typedef struct unk_func_8830867C_03C_058 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ unk_D_88400138* unk_04;
} unk_func_8830867C_03C_058; // size = 0x8

typedef struct unk_func_8830867C_03C_0D0 {
    /* 0x00 */ unk_func_8830867C_03C_058** unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ s32 unk_0C;
} unk_func_8830867C_03C_0D0; // size = 0x10

typedef struct unk_func_8830867C_03C_04C {
    /* 0x00 */ unk_func_8830867C_02C_048_000 unk_00;
    /* 0x24 */ struct unk_func_8830867C_03C* unk_24;
} unk_func_8830867C_03C_04C; // size = 0x28

typedef struct unk_func_8830867C_040_05C {
    /* 0x00 */ unk_func_8830867C_02C_048_000 unk_00;
    /* 0x24 */ struct unk_func_8830867C_040* unk_24;
} unk_func_8830867C_040_05C; // size = 0x28

typedef struct unk_func_8830867C_04C_080 {
    /* 0x00 */ unk_func_8830867C_02C_048_000 unk_00;
    /* 0x24 */ struct unk_func_8830867C_04C* unk_24;
} unk_func_8830867C_04C_080; // size = 0x28

typedef struct unk_func_8830867C_048_048 {
    /* 0x00 */ unk_func_8830867C_02C_048_000 unk_00;
    /* 0x24 */ struct unk_func_8830867C_048* unk_24;
} unk_func_8830867C_048_048; // size = 0x28

typedef void (*func88500020)(unk_func_88500020*, s32, s32);
typedef void (*func88500338)(unk_func_88500020* arg0, s32 arg1, s32 arg2, s32 arg3);
typedef void (*func88500668)(unk_func_882025E0*, s32);
typedef void (*func8850068C)(WidgetLinkHeader*, WidgetLinkHeader*);
typedef void (*func885007CC)(WidgetNode*, s32);
typedef void (*func88500828)(WidgetNode*, s32, s32);
typedef void (*func885008C4)(WidgetNode*);
typedef void (*func8850093C)(s32, s32);
typedef void (*func88500994)(unk_func_88500994*, s32, s32);
typedef void (*func88500A3C)(unk_func_88500994*, WidgetNode*);
typedef void (*func88500A6C)(unk_func_88500994*, s32);
typedef void (*func88500E34)(unk_func_88500E34*, s32, s32, s32, s32, u8*);
typedef void (*func88500EE4)(unk_func_88500EE4*, s32, s32, s32, s32, u8*, Color_RGBA8, Color_RGBA8);
typedef void (*func885012A4)(unk_func_885012A4*, s32, s32, s32, s32, Color_RGBA8);
typedef void (*func8850143C)(unk_func_8850143C*, s32, s32, s32, s32, Color_RGBA8, Color_RGBA8);
typedef void (*func8850182C)(unk_func_8850182C*, s32, s32, s32, s32, Color_RGBA8);
typedef void (*func88502274)(WidgetAnimatedPanel*, s32, s32, s32, s32);
typedef void (*func88502C98)(WidgetAnimatedPanel*);
typedef void (*func88503118)(WidgetDelayedNode*, s32, s32, s32, s32);
typedef void (*func885031E8)(WidgetNode*, s32, s32, s32, s32);
typedef void (*func88503298)(unk_func_88503298*, WidgetAnimatedPanel*, WidgetNode*);
typedef void (*func88503340)(WidgetAnimatedFrame*, s32, s32, s32, s32, Color_RGBA8);
typedef void (*func88503BAC)(WidgetAnimatedFrame*, s32, s32, s32, s32, Color_RGBA8);
typedef void (*func88504570)(WidgetAnimatedFrame*, s32, s32, s32, s32, Color_RGBA8);
typedef void (*func88504F98)(unk_func_8820E99C_030_044*, s32, s32, s32, s32);
typedef void (*func885054D8)(WidgetAnimatedPanelVariantB*, s32, s32, s32, s32, Color_RGBA8);
typedef void (*func88505E2C)(WidgetTextList*, s32, s32, s32, s32, s32, Color_RGBA8, FontContext*);
typedef void (*func88506074)(WidgetTextList*, char*);
typedef void (*func885060BC)(unk_func_88200FA0_030_030*, s32, s32, unk_func_88200FA0_030_030_040, s32, s32, s32, s32,
                             MemoryPool*);
typedef void (*func88506238)(unk_func_88200FA0_030_030*, s32, MemoryPool*);
typedef void (*func8850628C)(unk_func_88200FA0_030_030*);
typedef void (*func8850633C)(unk_func_88200FA0_030_030_1CEA00*, unk_func_8830867C_02C_0CC*);
typedef void (*func88506384)(unk_func_88200FA0_030_030*, unk_func_88200FA0_030_038*, MemoryPool*);
typedef void (*func885063B8)(unk_func_88200FA0_030_030*, unk_func_88201DA0_038*);
typedef void (*func88506404)(unk_func_88200FA0_030_030*, u8*, MemoryPool*);
typedef void (*func88506BFC)(unk_func_88200FA0_030_030*, s32);
typedef void (*func88506CE4)(unk_func_88200FA0_030_030*);
typedef void (*func88506DCC)(unk_func_8821421C_038_034*, s32, s32, void (*)(), s32, s32, s32, MemoryPool*);
typedef void (*func88506EE4)(unk_func_8820BE14_02C_038*, unk_func_88200FA0_030_038*);
typedef void (*func88506F1C)(unk_func_8830867C_02C_034*, unk_func_88200FA0_030_038*, MemoryPool*);
typedef void (*func88506F50)(unk_func_8821421C_038_034*, unk_func_8821421C_038_050**);
typedef void (*func88506F8C)(unk_func_8821421C_038_034*, unk_func_8830867C_03C_0D0*, MemoryPool*);
typedef void (*func88506FC0)(unk_func_8820BE14_02C_038*);
typedef void (*func88506FE0)(unk_func_8820BE14_02C_038*);
typedef void (*func88507AE4)(unk_func_8820BE14_02C_038*, s32);
typedef void (*func88507B80)(unk_func_8820BE14_02C_038*, s32);
typedef void (*func88507C0C)(unk_func_8830867C_02C_034*, s32, s32);
typedef void (*func88507CC4)(unk_func_8820BE14_02C_038*, unk_func_88205880_00D0*);
typedef void (*func88507CEC)(unk_func_8820BE14_02C_038*);
typedef void (*func88507D4C)(unk_func_88507D4C*, s32, s32, s32, s32, unk_func_8820BE14_02C_038*);
typedef void (*func8850878C)(unk_func_8850878C*, s32, s32, char*, s32);
typedef void (*func885088F4)(unk_func_885088F4*, s32, s32, s32);
typedef void (*func88509A2C)(unk_func_88509A2C*, s32, s32, s32, char*);
typedef void (*func88509E34)(unk_func_88509F0C*, s32, s32, char*);
typedef void (*func88509F48)(unk_func_88509F48*, s32, s32, s32, s32, MemoryPool*, s32);
typedef void (*func8850A10C)(unk_func_88509F48*, unk_func_8850878C*, s32, s32);
typedef void (*func8850A3CC)(unk_func_88509F48*, Controller*);
typedef void (*func8850A3EC)(unk_func_88509F48*, Controller*);
typedef void (*func8850A40C)(WidgetNode*, s32, s32, s32, s32);
typedef void (*func8850AB48)(WidgetNode*, s32, s32, s32, s32);
typedef void (*func8850B254)(unk_func_8850B254*, s32, s32, s32, s32, Color_RGBA8, Color_RGBA8);
typedef void (*func8850BC94)(s32);
typedef void (*func8850BD40)(unk_func_8850BD40*, unk_func_8821421C_038_034*);
typedef void (*func8850BDF0)(unk_func_88203ED8_064* arg0);
typedef void (*func8850BF60)(WidgetNode*, s32);
typedef void (*func8850BF80)(unk_func_8830867C_02C_048_000*, WidgetLinkHeader*);
typedef void (*func8850BFB0)(unk_func_88203ED8_064*);
typedef void (*func8850C064)(unk_func_88203ED8_064*, s32);
typedef void (*func8850C284)(WidgetGridMenu*, s32, s32, MemoryPool*);
typedef void (*func8850CAB4)(WidgetGridMenu*);
typedef void (*func8850CB48)(WidgetGridMenu*, s32);
typedef void (*func8850CBA8)(unk_func_8830867C_02C_048*);
typedef void (*func8850CC74)(unk_func_8830867C_02C_048_000*, WidgetLinkHeader*);
typedef void (*func8850CD24)(unk_func_8830867C_02C_048*, s32);
typedef void (*func8850CD44)(unk_func_8850CD44*, s32, MemoryPool*);
typedef void (*func8850CF00)(unk_func_8850CD44*, Controller*);

void WidgetTree_DrawTiledTexture(unk_func_88500020*, s32, s32);
void WidgetTree_DrawTiledTextureFmt(unk_func_88500020* arg0, s32 arg1, s32 arg2, s32 arg3);
void WidgetTree_InitNode(WidgetLinkHeader*, s32);
void WidgetTree_AppendChild(WidgetLinkHeader*, WidgetLinkHeader*);
void WidgetTree_InitWidget(WidgetNode*, s32);
void WidgetTree_Draw(WidgetNode*, s32, s32);
void WidgetTree_Update(WidgetNode*);
void WidgetTree_SetStateDefault(WidgetNode*, s32);
void WidgetTree_InitPagedContainer(unk_func_88500994*, s32, s32);
s32 WidgetTree_DrawCurrentPage(unk_func_88500994*, s32, s32);
void WidgetTree_AddPage(unk_func_88500994*, WidgetNode*);
void WidgetTree_SelectPage(unk_func_88500994*, s32);
void WidgetTree_InitTiledTextureRegion(unk_func_88500E34*, s32, s32, s32, s32, u8*);
s32 WidgetTree_DrawTiledTextureRegion(unk_func_88500E34*, s32, s32);
void WidgetTree_InitTintedTexture(unk_func_88500EE4*, s32, s32, s32, s32, u8*, Color_RGBA8, Color_RGBA8);
s32 WidgetTree_DrawTintedTexture(unk_func_88500EE4*, s32, s32);
void WidgetTree_InitSolidColor(unk_func_885012A4*, s32, s32, s32, s32, Color_RGBA8);
s32 WidgetTree_DrawSolidColor(unk_func_885012A4*, s32, s32);
void WidgetTree_InitPatternTexture(unk_func_8850143C*, s32, s32, s32, s32, Color_RGBA8, Color_RGBA8);
s32 WidgetTree_DrawPatternTexture(unk_func_8850143C*, s32, s32);
void WidgetTree_InitCornerFrame(unk_func_8850182C*, s32, s32, s32, s32, Color_RGBA8);
s32 WidgetTree_DrawCornerFrame(unk_func_8850182C*, s32, s32);
void WidgetTree_InitAnimatedPanel(WidgetAnimatedPanel*, s32, s32, s32, s32);
s32 WidgetTree_UpdateAnimatedPanel(WidgetAnimatedPanel*);
s32 WidgetTree_DrawAnimatedPanel(WidgetAnimatedPanel*, s32, s32);
s32 WidgetTree_GetAnimatedPanelInputState(WidgetAnimatedPanel*, Controller*);
void WidgetTree_OpenAnimatedPanel(WidgetAnimatedPanel*);
s32 WidgetTree_DrawHorizontalFrame(WidgetNode*, s32, s32);
void WidgetTree_InitDelayedWidget(WidgetDelayedNode*, s32, s32, s32, s32);
s32 WidgetTree_DrawDelayedWidget(WidgetDelayedNode*, s32, s32);
s32 WidgetTree_UpdateDelayedWidget(WidgetDelayedNode*);
void WidgetTree_InitVisibilityGate(WidgetNode*, s32, s32, s32, s32);
s32 WidgetTree_ClearVisibilityGateFlags(WidgetNode*);
s32 WidgetTree_DrawVisibilityGate(WidgetNode*, s32, s32);
void WidgetTree_InitVisibilityGateBridge(unk_func_88503298*, WidgetAnimatedPanel*, WidgetNode*);
s32 WidgetTree_UpdateVisibilityGateBridge(unk_func_88503298*);
void WidgetTree_InitAnimatedFrameVariantA(WidgetAnimatedFrame*, s32, s32, s32, s32, Color_RGBA8);
s32 WidgetTree_UpdateAnimatedFrameVariantA(WidgetAnimatedFrame*);
s32 WidgetTree_DrawAnimatedFrameVariantA(WidgetAnimatedFrame*, s32, s32);
void WidgetTree_SetAnimatedFrameVariantAState(WidgetAnimatedFrame*, s32);
void WidgetTree_InitAnimatedFrameVariantB(WidgetAnimatedFrame*, s32, s32, s32, s32, Color_RGBA8);
s32 WidgetTree_UpdateAnimatedFrameVariantB(WidgetAnimatedFrame*);
s32 WidgetTree_DrawAnimatedFrameVariantB(WidgetAnimatedFrame*, s32, s32);
void WidgetTree_SetAnimatedFrameVariantBState(WidgetAnimatedFrame*, s32);
void WidgetTree_InitAnimatedFrameVariantC(WidgetAnimatedFrame*, s32, s32, s32, s32, Color_RGBA8);
s32 WidgetTree_UpdateAnimatedFrameVariantC(WidgetAnimatedFrame*);
s32 WidgetTree_DrawAnimatedFrameVariantC(WidgetAnimatedFrame*, s32, s32);
void WidgetTree_SetAnimatedFrameVariantCState(WidgetAnimatedFrame*, s32);
void WidgetTree_InitDirectionalIndicator(unk_func_8820E99C_030_044*, s32, s32, s32, s32);
s32 WidgetTree_UpdateDirectionalIndicator(unk_func_8820E99C_030_044*);
s32 WidgetTree_DrawDirectionalIndicator(unk_func_8820E99C_030_044*, s32, s32);
void WidgetTree_SetDirectionalIndicatorState(unk_func_8820E99C_030_044*, s32);
void WidgetTree_InitAnimatedPanelVariantB(WidgetAnimatedPanelVariantB*, s32, s32, s32, s32, Color_RGBA8);
s32 WidgetTree_UpdateAnimatedPanelVariantB(WidgetAnimatedPanelVariantB*);
s32 WidgetTree_DrawAnimatedPanelVariantB(WidgetAnimatedPanelVariantB*, s32, s32);
void WidgetTree_InitTextList(WidgetTextList*, s32, s32, s32, s32, s32, Color_RGBA8, FontContext*);
s32 WidgetTree_UpdateTextList(WidgetTextList*);
s32 WidgetTree_DrawTextList(WidgetTextList*, s32, s32);
void WidgetTree_SetTextList(WidgetTextList* arg0, u8* arg1);
void WidgetTree_InitPagedGrid(unk_func_88200FA0_030_030*, s32, s32, unk_func_88200FA0_030_030_040, s32, s32, s32, s32,
                              MemoryPool*);
void WidgetTree_AllocateEntryFlags(unk_func_88200FA0_030_030*, s32, MemoryPool*);
void WidgetTree_ClearEntryFlags(unk_func_88200FA0_030_030*);
void WidgetTree_SetPagedGridState(unk_func_88200FA0_030_030*, s32);
void WidgetTree_BindPagedGridData(unk_func_88200FA0_030_030_1CEA00*, unk_func_8830867C_02C_0CC*);
void WidgetTree_BindPagedGridPage(unk_func_88200FA0_030_030*, unk_func_88200FA0_030_038*, MemoryPool*);
void WidgetTree_BindPagedGridStridedData(unk_func_88200FA0_030_030*, unk_func_88201DA0_038*);
void WidgetTree_BindPagedGridStridedPage(unk_func_88200FA0_030_030*, unk_func_8820BE14_06C*, MemoryPool*);
s32 WidgetTree_DrawPagedGrid(unk_func_88200FA0_030_030*, s32, s32);
s32 func_885065E0(unk_func_88200FA0_030_030*, Controller*);
void WidgetTree_SetPagedGridSelection(unk_func_88200FA0_030_030*, s32);
s32 WidgetTree_FindSelectableGridEntry(unk_func_88200FA0_030_030*);
void WidgetTree_InitScrollableGrid(unk_func_8821421C_038_034*, s32, s32, unk_func_88200FA0_030_030_040, s32, s32, s32,
                                   MemoryPool*);
void WidgetTree_BindScrollableGridData(unk_func_8821421C_038_034*, unk_func_88200FA0_030_038*);
void WidgetTree_BindScrollableGridPage(unk_func_8821421C_038_034*, unk_func_88200FA0_030_038*, MemoryPool*);
void WidgetTree_BindScrollableGridStridedData(unk_func_8821421C_038_034*, unk_func_8821421C_038_050**);
void WidgetTree_BindScrollableGridStridedPage(unk_func_8821421C_038_034*, unk_func_8830867C_03C_0D0*, MemoryPool*);
void WidgetTree_ClampScrollableGridTop(unk_func_8821421C_038_034*);
void WidgetTree_ClampScrollableGridBottom(unk_func_8821421C_038_034*);
s32 WidgetTree_UpdateScrollableGridScroll(unk_func_8821421C_038_034*);
s32 func_8850734C(unk_func_8821421C_038_034*, s32, s32);
s32 WidgetTree_HandleScrollableGridInput(unk_func_8821421C_038_034*, Controller*);
void WidgetTree_SetScrollableGridIndex(unk_func_8821421C_038_034*, s32);
void WidgetTree_SetScrollableGridIndexPreserveScroll(unk_func_8821421C_038_034*, s32);
void WidgetTree_SetPagedGridSelectionWithOffset(unk_func_8821421C_038_034*, s32, s32);
void WidgetTree_SetScrollableGridEntryText(unk_func_8821421C_038_034*, unk_func_88205880_00D0*);
void WidgetTree_RemoveScrollableGridEntry(unk_func_8821421C_038_034*);
void WidgetTree_InitScrollableGridScrollbar(unk_func_88507D4C*, s32, s32, s32, s32, unk_func_8821421C_038_034*);
s32 WidgetTree_DrawScrollableGridScrollbar(unk_func_88507D4C*, s32, s32);
void WidgetTree_InitTextLabel(unk_func_8850878C*, s32, s32, char*, s32);
s32 WidgetTree_DrawTextLabel(unk_func_8850878C*, s32, s32);
void WidgetTree_InitSlideTransition(unk_func_885088F4*, s32, s32, s32);
s32 WidgetTree_UpdateSlideTransition(unk_func_885088F4*);
s32 WidgetTree_DrawSlideTransition(unk_func_885088F4*, s32, s32);
void WidgetTree_InitSaveSlotLabel(unk_func_88509A2C*, s32, s32, s32, char*);
s32 WidgetTree_DrawSaveSlotLabel(unk_func_88509A2C*, s32, s32);
void WidgetTree_InitTextMessage(unk_func_88509E34*, s32, s32, char*);
s32 WidgetTree_DrawTextMessage(unk_func_88509F0C*, s32, s32);
void WidgetTree_InitMessagePanel(unk_func_88509F48*, s32, s32, s32, s32, MemoryPool*, FontContext*);
void WidgetTree_SetMessagePanelText(unk_func_88509F48*, unk_func_8850878C*, s32, s32);
s32 WidgetTree_HandleMessagePanelInput(unk_func_88509F48*, Controller*);
void WidgetTree_RunMessagePanelWithSound(unk_func_88509F48*, Controller*);
void WidgetTree_RunMessagePanelSilent(unk_func_88509F48*, Controller*);
void WidgetTree_InitBorderFrame(WidgetNode*, s32, s32, s32, s32);
s32 WidgetTree_DrawBorderFrame(WidgetNode*, s32, s32);
void WidgetTree_InitInsetBorderFrame(WidgetNode*, s32, s32, s32, s32);
s32 WidgetTree_DrawInsetBorderFrame(WidgetNode*, s32, s32);
void WidgetTree_InitDashedBorderFrame(unk_func_8850B254*, s32, s32, s32, s32, Color_RGBA8, Color_RGBA8);
s32 func_8850B2D4(unk_func_8850B254*, s32, s32);
void Ui_PlayInputActionSound(s32);
void WidgetTree_InitScrollableGridInputProxy(unk_func_8850BD40*, unk_func_8821421C_038_034*);
s32 WidgetTree_HandleScrollableGridInputProxy(unk_func_8850BD40* arg0, Controller* arg1);
void WidgetTree_SetScrollableGridInputProxyState(unk_func_8850BD40* arg0, s32 arg1);
void WidgetTree_InitVerticalMenu(unk_func_8830867C_02C_048_000* arg0);
s32 WidgetTree_HandleVerticalMenuInput(unk_func_8830867C_02C_048_000*, Controller*);
void WidgetTree_SetVerticalMenuState(unk_func_8830867C_02C_048_000*, s32);
void WidgetTree_AppendVerticalMenuItem(unk_func_8830867C_02C_048_000*, WidgetLinkHeader*);
void WidgetTree_UpdateVerticalMenuItems(unk_func_8830867C_02C_048_000*);
void WidgetTree_SetVerticalMenuSelection(unk_func_8830867C_02C_048_000*, s32);
s32 WidgetTree_HandleHorizontalMenuInput(unk_func_8830867C_02C_048_000* arg0, Controller* arg1);
void WidgetTree_SetHorizontalMenuState(unk_func_8830867C_02C_048_000* arg0, s32 arg1);
void WidgetTree_UpdateHorizontalMenuItems(unk_func_8830867C_02C_048_000*);
void WidgetTree_InitGridMenu(WidgetGridMenu*, s32, s32, MemoryPool*);
s32 WidgetTree_HandleGridMenuInput(WidgetGridMenu*, Controller*);
void WidgetTree_SetGridMenuState(WidgetGridMenu*, s32);
void WidgetTree_UpdateGridMenuItems(WidgetGridMenu*);
void WidgetTree_SelectFirstEnabledGridItem(WidgetGridMenu*);
void WidgetTree_SetGridMenuSelection(WidgetGridMenu*, s32);
void WidgetTree_InitChildSelectionProxy(unk_func_8830867C_02C_048_000*);
s32 WidgetTree_HandleChildSelectionProxyInput(unk_func_8830867C_02C_048_000*, Controller*);
void WidgetTree_SetChildSelectionProxyState(unk_func_8830867C_02C_048_000*, s32);
void WidgetTree_UpdateChildSelectionProxyItems(unk_func_8830867C_02C_048_000* arg0);
void WidgetTree_AppendChildSelectionProxyItem(unk_func_8830867C_02C_048_000*, WidgetLinkHeader*);
void WidgetTree_SetChildSelectionProxySelection(unk_func_8830867C_02C_048*, s32);
void WidgetTree_InitChildWidgetGroup(unk_func_8850CD44*, s32, MemoryPool*);
s32 WidgetTree_HandleChildWidgetGroupInput(unk_func_8850CD44* arg0, Controller* arg1);
void WidgetTree_SetChildWidgetGroupState(unk_func_8850CD44* arg0, s32 arg1);
void WidgetTree_RunChildWidgetGroup(unk_func_8850CD44*, Controller* arg1);

#endif // _FRAGMENT26_H_
