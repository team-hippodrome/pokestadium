#ifndef _FRAGMENT24_H_
#define _FRAGMENT24_H_

#include "global.h"
#include "src/fragments/pokemon_lab_support/pokemon_lab_support.h"
#include "src/fragments/widget_tree/widget_tree.h"
#include "src/fragments/lab_ui_support/lab_ui_support.h"
#include "src/gb_save.h"

typedef struct unk_func_8830867C_02C_04C_018 {
    /* 0x00 */ char unk00[0x8];
    /* 0x08 */ WidgetNode* unk_08;
    /* 0x0C */ WidgetNode* unk_0C;
    /* 0x10 */ WidgetNode* unk_10;
    /* 0x14 */ WidgetNode* unk_14;
    /* 0x18 */ WidgetNode* unk_18;
    /* 0x1C */ WidgetNode* unk_1C;
} unk_func_8830867C_02C_04C_018; // size >= 0x20

typedef struct unk_func_8830867C_02C_04C {
    /* 0x00 */ char unk00[0xC];
    /* 0x00 */ WidgetGridMenuItem* unk_0C;
    /* 0x10 */ WidgetGridMenuInputCallback unk_10;
    /* 0x14 */ WidgetGridMenuSetStateCallback unk_14;
    /* 0x18 */ unk_func_8830867C_02C_04C_018* unk_18;
    /* 0x1C */ WidgetAnimatedFrame* unk_1C;
    /* 0x20 */ s32 unk_20;
    /* 0x24 */ s32 unk_24;
    /* 0x28 */ s32 unk_28;
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ char unk30[0x8];
    /* 0x38 */ s16 unk_38;
    /* 0x3A */ s16 unk_3A;
    /* 0x3C */ s16 unk_3C;
    /* 0x3E */ s16 unk_3E;
} unk_func_8830867C_02C_04C; // size = 0x40

typedef struct unk_func_8830867C_02C_050 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ struct unk_func_8830867C_02C* unk_2C;
} unk_func_8830867C_02C_050; // size = 0x30

typedef struct unk_func_8830867C_02C_0B4_030 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ char unk2C[0xC];
    /* 0x38 */ s32 unk_38;
    /* 0x3C */ char unk3C[0x18];
} unk_func_8830867C_02C_0B4_030; // size = 0x54

typedef struct unk_func_8830867C_02C_0B4 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ WidgetAnimatedPanel* unk_2C;
    /* 0x30 */ unk_func_8830867C_02C_0B4_030* unk_30;
    /* 0x34 */ unk_func_88200FA0_030_038 unk_34;
} unk_func_8830867C_02C_0B4; // size = 0x40

typedef struct unk_func_8830867C_02C_0C0 {
    /* 0x00 */ unk_func_882173EC unk_00;
    /* 0x38 */ s32 unk_38;
} unk_func_8830867C_02C_0C0; // size = 0x3C

typedef struct unk_func_8830867C_034_03C_038_02C_000 {
    /* 0x00 */ char unk00[0x8];
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ char unk0C[0x10];
} unk_func_8830867C_034_03C_038_02C_000; // size = 0x1C

typedef struct unk_func_8830867C_034_03C_038_02C {
    /* 0x00 */ unk_func_8830867C_034_03C_038_02C_000* unk_00;
    /* 0x04 */ char unk04[0x4];
    /* 0x08 */ s32 unk_08;
} unk_func_8830867C_034_03C_038_02C; // size >= 0xC

// Seems to be >= 0x58 in other places?
typedef struct unk_func_8830867C_02C_144_000 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ char unk08[0x4];
    /* 0x0C */ unk_func_8830867C_02C_0CC_000_000* unk_0C;
    /* 0x10 */ char unk10[0x4];
    /* 0x14 */ s32 unk_14;
    /* 0x18 */ char unk18[0x8];
} unk_func_8830867C_02C_144_000; // size = 0x20

typedef struct unk_func_8830867C_02C_144_000_alt {
    /* 0x00 */ char unk00[0x2C];
    /* 0x2C */ unk_func_8830867C_034_03C_038_02C unk_2C;
    /* 0x38 */ char unk_38[0x14];
    /* 0x4C */ unk_func_8830867C_034_03C_038_02C unk_4C;
} unk_func_8830867C_02C_144_000_alt; // size >= 0x58

typedef struct unk_func_8830867C_02C_144 {
    /* 0x00 */ unk_func_8830867C_02C_144_000* unk_00;
    /* 0x04 */ char unk04[0x4];
    /* 0x08 */ s32 unk_08;
} unk_func_8830867C_02C_144; // size >= 0xC

typedef struct unk_func_8830867C_02C_154 {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ u8* unk_0C;
    /* 0x10 */ s32 unk_10;
    /* 0x14 */ s32 unk_14;
    /* 0x18 */ char unk18[0x4];
} unk_func_8830867C_02C_154; // size = 0x1C

typedef struct unk_func_8830867C_02C_1A8 {
    /* 0x00 */ u8* unk_00;
    /* 0x04 */ unk_func_8830867C_02C_154* unk_04;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ s32 unk_0C;
} unk_func_8830867C_02C_1A8; // size = 0x10

typedef struct unk_func_8830867C_030_03C_034 {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ u16 unk_02;
    /* 0x04 */ char unk04[0x2];
    /* 0x06 */ u16 unk_06;
    /* 0x08 */ char unk08[0x2];
    /* 0x0A */ u16 unk_0A;
} unk_func_8830867C_030_03C_034; // size >= 0xC

typedef struct unk_func_8830867C_030 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ WidgetAnimatedPanel* unk_2C;
    /* 0x30 */ WidgetPoint unk_30;
    /* 0x34 */ WidgetPoint unk_34;
    /* 0x38 */ unk_func_885012A4* unk_38;
    /* 0x3C */ unk_func_88200FA0_030_030_1CEA00* unk_3C;
    /* 0x40 */ unk_func_8830867C_02C_0CC unk_40;
    /* 0x4C */ s32 unk_4C;
    /* 0x50 */ s32 unk_50;
} unk_func_8830867C_030; // size = 0x54

typedef struct unk_func_8830867C_034_034 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ char unk2C[0x4];
    /* 0x30 */ s32 unk_30;
} unk_func_8830867C_034_034; // size = 0x34

typedef struct unk_func_8830867C_034_03C_038 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ unk_func_8830867C_034_03C_038_02C* unk_2C;
    /* 0x30 */ char unk30[0x4];
    /* 0x34 */ u16* unk_34;
    /* 0x38 */ s32 unk_38;
    /* 0x3C */ char unk3C[0x8];
    /* 0x44 */ WidgetNode* unk_44;
    /* 0x48 */ char unk48[0xC];
} unk_func_8830867C_034_03C_038; // size = 0x54

typedef struct unk_func_8830867C_034_03C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ WidgetAnimatedPanel* unk_2C;
    /* 0x30 */ unk_func_88500994* unk_30;
    /* 0x34 */ unk_func_88500994* unk_34;
    /* 0x38 */ unk_func_8830867C_034_03C_038* unk_38[2];
    /* 0x40 */ char unk40[0x4];
} unk_func_8830867C_034_03C; // size = 0x44

typedef struct unk_func_8830867C_034 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ WidgetAnimatedPanel* unk_2C;
    /* 0x30 */ unk_func_88200FA0_030_030_1CEA00* unk_30;
    /* 0x34 */ unk_func_8830867C_034_034* unk_34;
    /* 0x38 */ unk_func_8830867C_034_038* unk_38;
    /* 0x3C */ unk_func_8830867C_034_03C* unk_3C;
    /* 0x40 */ unk_func_8850878C* unk_40;
} unk_func_8830867C_034; // size = 0x44

typedef struct unk_func_8830867C_038_034 {
    /* 0x00 */ char** unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
} unk_func_8830867C_038_034; // size = 0xC

typedef struct unk_func_8830867C_038 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ WidgetAnimatedPanel* unk_2C;
    /* 0x30 */ unk_func_88200FA0_030_030_1CEA00* unk_30;
    /* 0x34 */ unk_func_8830867C_038_034 unk_34;
    /* 0x40 */ WidgetAnimatedPanel* unk_40;
} unk_func_8830867C_038; // size = 0x44

typedef struct unk_func_8830867C_03C_030 {
    /* 0x00 */ unk_func_8850878C* unk_00;
    /* 0x00 */ s32 unk_04;
} unk_func_8830867C_03C_030; // size = 0x8

typedef struct unk_func_8830867C_03C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ WidgetAnimatedPanel* unk_2C;
    /* 0x30 */ unk_func_8830867C_03C_030 unk_30[2];
    /* 0x40 */ unk_func_8850878C* unk_40;
    /* 0x44 */ unk_func_8850878C* unk_44;
    /* 0x48 */ WidgetNode* unk_48;
    /* 0x4C */ unk_func_8830867C_03C_04C* unk_4C;
    /* 0x50 */ WidgetGridMenu* unk_50;
    /* 0x54 */ unk_func_8821421C_038_034* unk_54;
    /* 0x58 */ unk_func_8830867C_03C_058 unk_58[15];
    /* 0xD0 */ unk_func_8830867C_03C_0D0 unk_D0;
    /* 0xE0 */ u16 unk_E0;
} unk_func_8830867C_03C; // size = 0xE4

typedef struct unk_func_8830867C_040_030 {
    /* 0x00 */ unk_func_8850878C* unk_00;
    /* 0x04 */ u8* unk_04;
} unk_func_8830867C_040_030; // size = 0x8

typedef struct unk_func_8830867C_040 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ WidgetAnimatedPanel* unk_2C;
    /* 0x30 */ unk_func_8830867C_040_030 unk_30[4];
    /* 0x50 */ unk_func_8850878C* unk_50;
    /* 0x54 */ unk_func_8850878C* unk_54;
    /* 0x58 */ WidgetNode* unk_58;
    /* 0x5C */ unk_func_8830867C_040_05C* unk_5C;
    /* 0x60 */ WidgetGridMenu* unk_60;
    /* 0x64 */ unk_func_8821421C_038_034* unk_64;
    /* 0x68 */ unk_func_88205880_A030 unk_68;
    /* 0x74 */ u16 unk_74;
} unk_func_8830867C_040; // size = 0x78

typedef struct unk_func_8830867C_044_038 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ char unk2C[0x4];
    /* 0x30 */ unk_func_8830867C_044_038_030 unk_30;
    /* 0x44 */ unk_func_8820E99C_030_044* unk_44;
} unk_func_8830867C_044_038; // size = 0x48

typedef struct unk_func_8830867C_044 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ WidgetAnimatedPanel* unk_2C;
    /* 0x30 */ unk_func_8850878C* unk_30;
    /* 0x34 */ WidgetNode* unk_34;
    /* 0x38 */ unk_func_8830867C_044_038* unk_38;
    /* 0x3C */ unk_func_8830867C_044_038* unk_3C;
    /* 0x40 */ unk_func_8830867C_044_040* unk_40;
    /* 0x44 */ WidgetGridMenu* unk_44;
} unk_func_8830867C_044; // size = 0x48

typedef struct unk_func_8830867C_048_04C {
    /* 0x00 */ char unk00[0x1C];
    /* 0x1C */ s32 unk_1C;
    /* 0x20 */ char unk20[0x4];
} unk_func_8830867C_048_04C; // size = 0x24

typedef struct unk_func_8830867C_048 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ WidgetAnimatedPanel* unk_2C;
    /* 0x30 */ WidgetAnimatedPanel* unk_30;
    /* 0x34 */ unk_func_88200FA0_030_030_1CEA00* unk_34;
    /* 0x38 */ unk_func_88200FA0_030_030_1CEA00* unk_38[2];
    /* 0x40 */ unk_func_88500994* unk_40;
    /* 0x44 */ unk_func_88500994* unk_44;
    /* 0x48 */ unk_func_8830867C_048_048* unk_48;
    /* 0x4C */ unk_func_8830867C_048_04C* unk_4C;
    /* 0x50 */ char unk_50[0x80];
} unk_func_8830867C_048; // size = 0xD0

typedef struct unk_func_8830867C_04C_030_02C_000_000 {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ char unk02[0x6];
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ unk_func_8830867C_04C_030_02C_000_000_00C unk_0C;
} unk_func_8830867C_04C_030_02C_000_000; // size >= 0x18

typedef struct unk_func_8830867C_04C_030_02C_000 {
    /* 0x00 */ unk_func_8830867C_04C_030_02C_000_000* unk_00[1];
} unk_func_8830867C_04C_030_02C_000; // size >= 0x8

typedef struct unk_func_8830867C_04C_030_02C {
    /* 0x00 */ unk_func_8830867C_04C_030_02C_000* unk_00;
    /* 0x04 */ char unk04[0x4];
    /* 0x08 */ s32 unk_08;
} unk_func_8830867C_04C_030_02C; // size >= 0xC

typedef struct unk_func_8830867C_04C_030 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ unk_func_8830867C_04C_030_02C* unk_2C;
    /* 0x30 */ char unk30[0x4];
    /* 0x34 */ u16* unk_34;
    /* 0x38 */ s32 unk_38;
    /* 0x3C */ char unk3C[0x2];
    /* 0x3E */ s16 unk_3E;
    /* 0x40 */ char unk40[0x18];
    /* 0x58 */ s32 unk_58;
    /* 0x5C */ char unk5C[0x14];
} unk_func_8830867C_04C_030; // size = 0x70

typedef void (*unk_func_8830867C_04C_078_030_040)();

typedef struct unk_func_8830867C_04C_078_030 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ void* unk_2C;
    /* 0x30 */ s32 unk_30;
    /* 0x34 */ u16* unk_34;
    /* 0x38 */ s32 unk_38;
    /* 0x3C */ s16 unk_3C;
    /* 0x3E */ s16 unk_3E;
    /* 0x40 */ unk_func_8830867C_04C_078_030_040 unk_40;
    /* 0x44 */ WidgetAnimatedFrame* unk_44;
    /* 0x48 */ s16 unk_48;
    /* 0x4A */ s16 unk_4A;
    /* 0x4C */ s16 unk_4C;
    /* 0x4E */ s16 unk_4E;
    /* 0x50 */ s16 unk_50;
    /* 0x52 */ s16 unk_52;
} unk_func_8830867C_04C_078_030; // size = 0x54

typedef struct unk_func_8830867C_04C_078_044 {
    /* 0x00 */ unk_func_88201DA0_034* unk_00;
    /* 0x04 */ s32 unk_04;
} unk_func_8830867C_04C_078_044; // size = 0x8

typedef struct unk_func_8830867C_04C_078 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ WidgetAnimatedPanel* unk_2C;
    /* 0x30 */ unk_func_8830867C_04C_078_030* unk_30;
    /* 0x34 */ unk_func_88201DA0_038 unk_34;
    /* 0x44 */ unk_func_8830867C_04C_078_044 unk_44[1];
    /* 0x4C */ char unk4C[0x18];
} unk_func_8830867C_04C_078; // size = 0x64

typedef struct unk_func_8830867C_04C_02C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ s32 unk_30;
} unk_func_8830867C_04C_02C; // size = 0x34

typedef struct unk_func_8830867C_04C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ unk_func_8830867C_04C_02C* unk_2C;
    /* 0x30 */ unk_func_8830867C_04C_030* unk_30;
    /* 0x34 */ unk_func_88500994* unk_34;
    /* 0x38 */ char unk_38[4];
    /* 0x3C */ char unk3C[0x3C];
    /* 0x78 */ unk_func_8830867C_04C_078* unk_78;
    /* 0x7C */ unk_func_8830867C_030* unk_7C;
    /* 0x80 */ unk_func_8830867C_04C_080* unk_80;
    /* 0x84 */ s32 unk_84;
    /* 0x88 */ BattleMon* unk_88; // maybe unk_func_8830867C_02C_0CC_000_008
    /* 0x8C */ s32 unk_8C;
    /* 0x90 */ unk_func_88503298* unk_90;
} unk_func_8830867C_04C; // size = 0x94

typedef struct unk_func_8830867C_02C_C54_014 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ u8 unk_04[2];
} unk_func_8830867C_02C_C54_014; // size >= 0x8

typedef struct unk_func_8830867C_02C_C54_01C {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ u8 unk_04[2];
} unk_func_8830867C_02C_C54_01C; // size >= 0x8

typedef struct unk_func_8830867C_02C_C54_024 {
    /* 0x00 */ u8 unk_00[4];
    /* 0x04 */ s32 unk_04;
} unk_func_8830867C_02C_C54_024; // size >= 0x8

typedef struct unk_func_8830867C_02C_C54 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ u8 unk_08;
    /* 0x09 */ u8 unk_09;
    /* 0x0C */ s32 unk_0C;
    /* 0x10 */ s32 unk_10;
    /* 0x14 */ unk_func_8830867C_02C_C54_014 unk_14;
    /* 0x1C */ unk_func_8830867C_02C_C54_01C unk_1C;
    /* 0x24 */ unk_func_8830867C_02C_C54_024 unk_24;
    /* 0x2C */ u8 unk_2C;
} unk_func_8830867C_02C_C54; // size = 0x30

typedef struct unk_func_8830867C_054 {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ MemoryPool* unk_04;
    /* 0x08 */ WidgetNode* unk_08;
    /* 0x0C */ s32 unk_0C;
    /* 0x10 */ s32 unk_10;
    /* 0x14 */ WidgetNode* unk_14;
} unk_func_8830867C_054; // size = 0x18

typedef struct unk_func_8830867C_02C_0E4 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ unk_func_8820BE14_06C unk_0C;
    /* 0x1C */ s32 unk_1C;
} unk_func_8830867C_02C_0E4; // size = 0x20

typedef struct unk_func_8830867C_02C_0DC {
    /* 0x00 */ s32* unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ char unk0C[0xC];
    /* 0x18 */ s32 unk_18;
    /* 0x1C */ char unk1C[0xC];
    /* 0x28 */ s32 unk_28;
} unk_func_8830867C_02C_0DC; // size >= 0x2C

typedef struct unk_func_8830867C_02C {
    /* 0x000 */ WidgetNode unk_000;
    /* 0x02C */ WidgetAnimatedPanel* unk_02C;
    /* 0x030 */ unk_func_8850B254* unk_030;
    /* 0x034 */ unk_func_8830867C_02C_034* unk_034;
    /* 0x038 */ unk_func_88500994* unk_038;
    /* 0x03C */ unk_func_88200FA0_030_038 unk_03C;
    /* 0x048 */ unk_func_8830867C_02C_048* unk_048;
    /* 0x04C */ unk_func_8830867C_02C_04C* unk_04C;
    /* 0x050 */ unk_func_8830867C_02C_050* unk_050;
    /* 0x054 */ WidgetNode* unk_054;
    /* 0x058 */ WidgetNode* unk_058;
    /* 0x05C */ WidgetNode* unk_05C;
    /* 0x060 */ WidgetNode* unk_060;
    /* 0x064 */ WidgetNode* unk_064;
    /* 0x068 */ WidgetNode* unk_068;
    /* 0x06C */ WidgetNode* unk_06C;
    /* 0x070 */ WidgetNode* unk_070;
    /* 0x074 */ WidgetNode* unk_074;
    /* 0x078 */ WidgetNode* unk_078;
    /* 0x07C */ WidgetNode* unk_07C;
    /* 0x080 */ WidgetNode* unk_080;
    /* 0x084 */ WidgetNode* unk_084;
    /* 0x088 */ WidgetNode* unk_088;
    /* 0x08C */ unk_func_8830867C_030* unk_08C;
    /* 0x090 */ unk_func_88509F48* unk_090;
    /* 0x094 */ unk_func_8830867C_034* unk_094;
    /* 0x098 */ unk_func_8830867C_038* unk_098;
    /* 0x09C */ unk_func_8830867C_03C* unk_09C;
    /* 0x0A0 */ unk_func_8830867C_040* unk_0A0;
    /* 0x0A4 */ unk_func_8830867C_044* unk_0A4;
    /* 0x0A8 */ unk_func_8830867C_048* unk_0A8;
    /* 0x0AC */ unk_func_8830867C_04C* unk_0AC;
    /* 0x0B0 */ unk_func_889000C4* unk_0B0;
    /* 0x0B4 */ unk_func_8830867C_02C_0B4* unk_0B4;
    /* 0x0B8 */ unk_func_8850CD44* unk_0B8;
    /* 0x0BC */ unk_func_882173EC* unk_0BC;
    /* 0x0C0 */ unk_func_8830867C_02C_0C0* unk_0C0;
    /* 0x0C4 */ unk_func_882173EC* unk_0C4;
    /* 0x0C8 */ unk_func_882173EC* unk_0C8;
    /* 0x0CC */ unk_func_88200FA0_030_038 unk_0CC;
    /* 0x0D8 */ s32 unk_0D8;
    /* 0x0DC */ unk_func_8830867C_02C_0DC* unk_0DC;
    /* 0x0E0 */ s32 unk_0E0;
    /* 0x0E4 */ unk_func_8830867C_02C_0E4 unk_0E4[3];
    /* 0x144 */ unk_func_8830867C_02C_144 unk_144;
    /* 0x150 */ char unk150[0x4];
    /* 0x154 */ unk_func_8830867C_02C_154 unk_154[3];
    /* 0x1A8 */ unk_func_8830867C_02C_1A8 unk_1A8[3];
    /* 0x1D8 */ char unk1D8[0xA70];
    /* 0xC48 */ unk_func_88200FA0_030_038 unk_C48;
    /* 0xC54 */ unk_func_8830867C_02C_C54 unk_C54;
    /* 0xC84 */ unk_func_8830867C_02C_C54 unk_C84;
    /* 0xCB4 */ s32 unk_CB4;
    /* 0xCB8 */ s32 unk_CB8;
    /* 0xCBC */ s32 unk_CBC;
    /* 0xCC0 */ s32 unk_CC0;
    /* 0xCC4 */ s32 unk_CC4;
    /* 0xCC8 */ s32 unk_CC8;
    /* 0xCCC */ unk_func_8850878C* unk_CCC;
    /* 0xCD0 */ s32 unk_CD0;
    /* 0xCD4 */ s32* unk_CD4;
    /* 0xCD8 */ FragmentEntry unk_CD8;
    /* 0xCDC */ unk_func_8830867C_054* unk_CDC;
} unk_func_8830867C_02C; // size = 0xCE0

typedef struct unk_func_8830867C {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ unk_func_8830867C_02C* unk_2C;
    /* 0x30 */ unk_func_8830867C_030* unk_30;
    /* 0x34 */ unk_func_8830867C_034* unk_34;
    /* 0x38 */ unk_func_8830867C_038* unk_38;
    /* 0x3C */ unk_func_8830867C_03C* unk_3C;
    /* 0x40 */ unk_func_8830867C_040* unk_40;
    /* 0x44 */ unk_func_8830867C_044* unk_44;
    /* 0x48 */ unk_func_8830867C_048* unk_48;
    /* 0x4C */ unk_func_8830867C_04C* unk_4C;
    /* 0x50 */ unk_func_889000C4* unk_50;
    /* 0x54 */ unk_func_8830867C_054 unk_54;
    /* 0x6C */ s32 unk_6C;
    /* 0x70 */ char** unk_70;
    /* 0x74 */ char** unk_74;
    /* 0x78 */ char** unk_78;
    /* 0x7C */ char** unk_7C;
    /* 0x80 */ char** unk_80;
    /* 0x84 */ char** unk_84;
    /* 0x88 */ char** unk_88;
} unk_func_8830867C; // size = 0x8C

typedef struct unk_func_88302D98 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ unk_func_8830867C_02C* unk_2C;
} unk_func_88302D98; // size = 0x30

typedef struct unk_func_88304850_arg2 {
    /* 0x00 */ s32* unk_00;
    /* 0x04 */ char unk04[4];
    /* 0x08 */ u8 unk_08;
    /* 0x09 */ char unk09[0xD];
    /* 0x16 */ s16 unk_16;
    /* 0x18 */ char unk18[0x14];
    /* 0x2C */ u8 unk_2C;
    /* 0x2E */ s16 unk_2E;
    /* 0x30 */ s16 unk_30;
    /* 0x32 */ s16 unk_32;
    /* 0x34 */ s16 unk_34;
    /* 0x36 */ s16 unk_36;
    /* 0x38 */ char unk_38[4];
    /* 0x3C */ char unk3C[0x4];
    /* 0x40 */ char unk_40[3];
    /* 0x43 */ char unk_43[1];
} unk_func_88304850_arg2; // size >= 0x44

typedef struct unk_func_88309C38_arg2_004 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ Color_RGBA8 unk_04;
} unk_func_88309C38_arg2_004; // size >= 0x8

typedef struct unk_func_88309C38_arg2_00C {
    /* 0x00 */ char unk00[0x14];
    /* 0x14 */ s32 unk_14;
    /* 0x18 */ s32 unk_18;
} unk_func_88309C38_arg2_00C; // size >= 0x1C

typedef struct unk_func_88309C38_arg2 {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x04 */ unk_func_88309C38_arg2_004* unk_04;
    /* 0x08 */ char unk08[0x4];
    /* 0x0C */ unk_func_88309C38_arg2_00C* unk_0C;
    /* 0x10 */ char unk10[0x4];
    /* 0x14 */ s32 unk_14;
    /* 0x18 */ s32 unk_18;
} unk_func_88309C38_arg2; // size >= 0x1C

typedef struct unk_func_88309160_arg2 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ char unk08[0x4];
    /* 0x0C */ unk_func_88309C38_arg2_00C* unk_0C;
    /* 0x10 */ char unk10[0x4];
    /* 0x14 */ s32 unk_14;
    /* 0x18 */ s32 unk_18;
} unk_func_88309160_arg2; // size >= 0x1C

typedef struct unk_func_88310CA4_arg2 {
    /* 0x00 */ u8* unk_00;
    /* 0x04 */ u8* unk_04;
    /* 0x08 */ char unk08[0x4];
    /* 0x0C */ unk_func_88309C38_arg2_00C* unk_0C;
    /* 0x10 */ char unk10[0x4];
    /* 0x14 */ s32 unk_14;
    /* 0x18 */ s32 unk_18;
} unk_func_88310CA4_arg2; // size >= 0x1C

typedef struct unk_func_88310B70 {
    /* 0x00 */ WidgetNode unk_00;
    /* 0x2C */ unk_func_8830867C_04C_030* unk_2C;
    /* 0x30 */ unk_func_8850878C* unk_30;
} unk_func_88310B70; // size = 0x34

typedef struct unk_func_88313894_020 {
    /* 0x00 */ char unk00[0xC];
    /* 0x0C */ unk_func_8830867C_02C_0CC_000_000* unk_0C;
    /* 0x10 */ char unk10[0x4];
    /* 0x14 */ s32 unk_14;
    /* 0x18 */ char unk18[0x4];
    /* 0x1C */ s32 unk_1C;
} unk_func_88313894_020; // size = 0x20

typedef struct unk_func_88316128 {
    /* 0x00 */ char unk00[0x8];
    /* 0x08 */ u8 unk_08;
    /* 0x09 */ char unk09[0xD];
    /* 0x16 */ u16 unk_16;
    /* 0x18 */ char unk18[0x14];
    /* 0x2C */ u8 unk_2C;
    /* 0x2E */ u16 unk_2E;
    /* 0x30 */ u16 unk_30;
    /* 0x32 */ u16 unk_32;
    /* 0x34 */ u16 unk_34;
    /* 0x36 */ u16 unk_36;
    /* 0x38 */ u8 unk_38;
    /* 0x39 */ char unk39[0xA];
    /* 0x43 */ u8 unk_43;
} unk_func_88316128; // size >= 0x44

extern char** sIdLabelText;
extern char** sPcListText;
extern char** sItemDescriptionText;
extern char** sPcListMoveNames;
extern char** sPcListPokemonNames;
extern char** sPcListTypeNames;
extern char** sItemNameTable;
extern Color_RGBA8 D_8831727C;
extern Color_RGBA8 D_88317280;
extern Color_RGBA8 D_88317284;
extern u32 D_883176D8[];
extern u32 D_88317920[];
extern u32 D_88317F28[];
extern u32 D_88318530[];
extern u32 D_883186E8[];
extern u32 D_88318898[];
extern u32 D_883198A0[];
extern unk_D_8831A1F8 D_8831A1F8[];

void LabPCList_InitHeaderSelectIcon(WidgetNode* arg0, s32 arg1, s32 arg2);
s32 LabPCList_HeaderSelectIcon_Draw(WidgetNode* arg0, s32 arg1, s32 arg2);
void LabPCList_DrawHeaderPokemon(WidgetNode* arg0, s32 arg1, s32 arg2);
s32 LabPCList_HeaderPokemon_Draw(WidgetNode* arg0, s32 arg1, s32 arg2);
void LabPCList_DrawHeaderNickname(WidgetNode* arg0, s32 arg1, s32 arg2);
s32 LabPCList_HeaderNickname_Draw(WidgetNode* arg0, s32 arg1, s32 arg2);
void LabPCList_DrawHeaderID(WidgetNode* arg0, s32 arg1, s32 arg2);
s32 LabPCList_HeaderID_Draw(WidgetNode* arg0, s32 arg1, s32 arg2);
void LabPCList_DrawHeaderOT(WidgetNode* arg0, s32 arg1, s32 arg2);
s32 LabPCList_HeaderOT_Draw(WidgetNode* arg0, s32 arg1, s32 arg2);
void LabPCList_DrawHeaderArea(WidgetNode* arg0, s32 arg1, s32 arg2);
s32 LabPCList_HeaderArea_Draw(WidgetNode* arg0, s32 arg1, s32 arg2);
void LabPCList_DrawHeaderTypes(WidgetNode* arg0, s32 arg1, s32 arg2);
s32 LabPCList_HeaderTypes_Draw(WidgetNode* arg0, s32 arg1, s32 arg2);
void LabPCList_DrawHeaderMoves(WidgetNode* arg0, s32 arg1, s32 arg2);
s32 LabPCList_HeaderMoves_Draw(WidgetNode* arg0, s32 arg1, s32 arg2);
void LabPCList_DrawHeaderLevel(WidgetNode* arg0, s32 arg1, s32 arg2);
s32 LabPCList_HeaderLevel_Draw(WidgetNode* arg0, s32 arg1, s32 arg2);
void LabPCList_DrawHeaderHp(WidgetNode* arg0, s32 arg1, s32 arg2);
s32 LabPCList_HeaderHp_Draw(WidgetNode* arg0, s32 arg1, s32 arg2);
void LabPCList_DrawHeaderAttack(WidgetNode* arg0, s32 arg1, s32 arg2);
s32 LabPCList_HeaderAttack_Draw(WidgetNode* arg0, s32 arg1, s32 arg2);
void LabPCList_DrawHeaderDefense(WidgetNode* arg0, s32 arg1, s32 arg2);
s32 LabPCList_HeaderDefense_Draw(WidgetNode* arg0, s32 arg1, s32 arg2);
void LabPCList_DrawHeaderSpeed(WidgetNode* arg0, s32 arg1, s32 arg2);
s32 LabPCList_HeaderSpeed_Draw(WidgetNode* arg0, s32 arg1, s32 arg2);
void LabPCList_DrawHeaderSpecial(WidgetNode* arg0, s32 arg1, s32 arg2);
s32 LabPCList_HeaderSpecial_Draw(WidgetNode* arg0, s32 arg1, s32 arg2);
s32 LabPCList_DrawColumnDivider(unk_func_8830867C_02C_050* arg0, s32 arg1, s32 arg2);
void LabPCList_InitCornerIcon(WidgetNode* arg0, s32 arg1, s32 arg2);
s32 LabPCList_DrawCornerIcon(WidgetNode* arg0, s32 arg1, s32 arg2);
void LabPCList_LoadDecks(unk_func_8830867C_02C* arg0, MemoryPool* arg1);
void LabPCList_SaveDirtyDecks(unk_func_8830867C_02C* arg0);
void LabPCList_InitDeckSlots(unk_func_8830867C_02C* arg0, MemoryPool* arg1);
void LabPCList_InitPageHeader(unk_func_88302D98* arg0, s32 arg1, s32 arg2, unk_func_8830867C_02C* arg3);
s32 LabPCList_DrawPageHeader(unk_func_88302D98* arg0, s32 arg1, s32 arg2);
void LabPCList_SwitchColumnPage(unk_func_8830867C_02C* arg0);
void LabPCList_DrawTypeName(s32 arg0, s32 arg1, s32 arg2, u8* arg3, s32 arg4);
void LabPCList_DrawMoveNameCell(s32 arg0, s32 arg1, s32 arg2, u8* arg3, s32 arg4);
void LabPCList_DrawAreaCell(s32 arg0, s32 arg1, s32 arg2, unk_func_88304850_arg2* arg3);
void LabPCList_DrawPlainText(s32 arg0, s32 arg1, s8* arg2, s32 arg3);
void LabPCList_DrawFormattedNumber(s32 arg0, s32 arg1, char* arg2, s32 arg3);
void LabPCList_DrawStatNumber(s32 arg0, s32 arg1, char* arg2, s32 arg3, u8* arg4);
s32 LabPCList_CheckStatFilterRange(unk_func_8830867C_02C* arg0, unk_func_88304850_arg2* arg1);
void func_88304850(s32 arg0, s32 arg1, unk_func_88304850_arg2* arg2, s32 arg3, s32 arg4,
                   unk_func_8830867C_02C_034* arg5);
void LabPCList_SaveFilterState(unk_func_8830867C_02C* arg0);
void LabPCList_RestoreFilterState(unk_func_8830867C_02C* arg0);
void LabPCList_DiscardFilterBackup(unk_func_8830867C_02C* arg0);
void LabPCList_ResetFilterState(unk_func_8830867C_02C* arg0);
s32 LabPCList_HasFilterFlag(unk_func_8830867C_02C* arg0, s32 arg1);
void LabPCList_SetFilterFlag(unk_func_8830867C_02C* arg0, s32 arg1);
void LabPCList_ClearFilterFlag(unk_func_8830867C_02C* arg0, s32 arg1);
s32 LabPCList_ToggleFilterFlag(unk_func_8830867C_02C* arg0, s32 arg1);
void LabPCList_SetSortMode(unk_func_8830867C_02C* arg0, s32 arg1);
void LabPCList_ApplyFiltersAndSort(unk_func_8830867C_02C* arg0);
s32 LabPCList_CheckAnyDirty(unk_func_8830867C_02C* arg0);
s32 LabPCList_FindEntryIndex(unk_func_8830867C_02C_0CC* arg0, unk_func_8830867C_02C_0CC_000* arg1);
void LabPCList_PerformMoveMon(unk_func_8830867C_02C* arg0, unk_func_8830867C_02C_0CC_000* arg1, Controller* arg2);
void LabPCList_ReindexBoxSlots(unk_func_8830867C_02C_0CC_000_000* arg0);
void LabPCList_MoveEntryToBox(unk_func_8830867C_02C* arg0, unk_func_8830867C_02C_0CC_000* arg1, s32 arg2, s32 arg3);
void LabPCList_ClearMovesFrom(unk_func_8830867C_02C_0CC_000_008* arg0, s32 arg1);
void LabPCList_TeachTmMove(unk_func_8830867C_02C_0CC_000* arg0, unk_func_8830867C_04C_030_02C_000* arg1, s32 arg2);
void LabPCList_SetRowMenuState(unk_func_8830867C_02C* arg0, s32 arg1);
s32 LabPCList_RowMenuHandleInput(unk_func_8830867C_02C* arg0, Controller* arg1);
void LabPCList_RunPartySelectionModal(unk_func_8830867C_02C* arg0, Controller* arg1);
void func_88305F28(unk_func_8830867C_02C* arg0);
s32 LabPCList_OpenListMenu(unk_func_8830867C_02C* arg0, Controller* arg1);
s32 LabPCList_OpenRowMenu(unk_func_8830867C_02C* arg0, Controller* arg1);
s32 LabPCList_HandleColumnHeaderClick(unk_func_8830867C_02C* arg0, WidgetNode* arg1, s32 arg2, Controller* arg3);
u32 LabPCList_GetColumnMenuIndex(unk_func_8830867C_02C* arg0);
void func_88306FE4(void);
void LabPCList_InitColumnSelectionProxy(unk_func_8830867C_02C_048* arg0, unk_func_8830867C_02C* arg1);
void LabPCList_SaveAll(unk_func_8830867C_02C* arg0, s32 arg1);
void LabPCList_MarkIneligibleEntries(unk_func_8830867C_02C* arg0);
s32 LabPCList_IsGamePakBoxEmpty(unk_func_8830867C_02C* arg0);
s32 LabPCList_ConfirmExit(unk_func_8830867C_02C* arg0, Controller* arg1, s32 arg2);
void LabPCList_SetGridInteractive(unk_func_8830867C_02C* arg0, s32 arg1);
void LabPCList_ResetGridSelection(unk_func_8830867C_02C_048* arg0);
void LabPCList_HandleGridSelectionEntry(unk_func_8830867C_02C_048* arg0, Controller* arg1);
s32 LabPCList_HandleBackButton(unk_func_8830867C_02C_048* arg0, Controller* arg1);
s32 LabPCList_HandlePageSwitchInput(unk_func_8830867C_02C_048* arg0, Controller* arg1);
s32 LabPCList_HandleGridInput(unk_func_8830867C_02C_048* arg0, Controller* arg1, s32 arg2);
s32 LabPCList_HandleColumnMenuInput(unk_func_8830867C_02C_048* arg0, Controller* arg1, s32 arg2);
s32 LabPCList_HandleTopLevelInput(unk_func_8830867C_02C_048* arg0, Controller* arg1);
void LabPCList_RunWidgetLoop(unk_func_8830867C* arg0);
void LabPCList_ScreenThreadEntry(unk_func_8830867C* arg0);
s32 LabPCList_ShowListScreen(UNUSED s32 arg0, UNUSED s32 arg1);

void BattleMoveListWidget_Initialize(unk_func_8830867C_04C_078* arg0, s32 arg1, s32 arg2, MemoryPool* arg3);
void BattleMoveListWidget_SetMoves(unk_func_8830867C_04C_078* arg0, BattleMon* arg1);
void BattleMoveListWidget_DisableUnavailableMoves(unk_func_8830867C_04C_078* arg0);
void BattleMoveListWidget_DisableRestrictedMoves(unk_func_8830867C_04C_078* arg0);
s32 BattleMoveListWidget_HandleInput(unk_func_8830867C_04C_078* arg0, Controller* arg1);
void BattleMoveListWidget_Destroy(unk_func_8830867C_04C_078* arg0);
void BattleMoveListWidget_SetCloseState(unk_func_8830867C_04C_078* arg0);
void func_883097E4(void);
s32 BattleInfoWidget_GetEntryDisabledFlags(unk_func_8830867C_034_03C_038_02C_000* arg0);
void func_88309808(void);
void BattleInfoWidget_ApplyEntryDisabledFlags(unk_func_8830867C_034_03C* arg0);
void BattleInfoWidget_SetActivePageFlags(unk_func_8830867C_034_03C* arg0, s32 arg1);
void BattleInfoWidget_SyncEntrySelection(unk_func_8830867C_034_03C* arg0);
void BattleInfoWidget_DrawIndicator(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32 BattleInfoWidget_GetEntryWidth(void);
void BattleInfoWidget_Initialize(unk_func_8830867C_034_03C* arg0, s32 arg1, s32 arg2, s32 arg3, MemoryPool* arg4);
s32 BattleInfoWidget_HandleInput(unk_func_8830867C_034_03C* arg0, Controller* arg1);
void BattleInfoWidget_SelectPage(unk_func_8830867C_034_03C* arg0, s32 arg1);
void BattleInfoWidget_InitializeEntryMarker(unk_func_8830867C_034_034* arg0, s32 arg1, s32 arg2, s32 arg3);
s32 BattleInfoWidget_DrawEntryMarker(unk_func_8830867C_034_034* arg0, s32 arg1, s32 arg2);
s32 BattleInfoWidget_GetPageSelectionFlags(unk_func_8830867C_02C_144_000* arg0);
void BattleInfoWidget_SyncParentSelection(unk_func_8830867C_034* arg0, unk_func_8830867C_034_03C* arg1);
void BattleInfoWidget_UpdateEntryFlags(unk_func_8830867C_034* arg0);
void BattleInfoWidget_UpdateConfirmState(unk_func_8830867C_034* arg0);
void BattleInfoWidget_ApplyEntrySelection(unk_func_8830867C_034* arg0, s32 arg1);
s32 BattleInfoWidget_GetStatusEntryWidth(void);
void BattleInfoWidget_Build(unk_func_8830867C_034* arg0, s32 arg1, s32 arg2, WidgetNode* arg3, MemoryPool* arg4);
void BattleInfoWidget_BindData(unk_func_8830867C_034* arg0, unk_func_8830867C_02C_144* arg1);
void BattleInfoWidget_SetWidgetState(unk_func_8830867C_034* arg0, s32 arg1);
s32 BattleInfoWidget_HandleContainerInput(unk_func_8830867C_034* arg0, Controller* arg1);
void BattleInfoWidget_InitSelectionProxy(unk_func_8830867C_034_038* arg0, unk_func_8830867C_034* arg1);
s32 BattleInfoWidget_HandleSelectionProxyInput(unk_func_8830867C_034_038* arg0, Controller* arg1);
void BattleInfoWidget_OpenPanels(unk_func_8830867C_034* arg0);
void BattleInfoWidget_SetCloseState(unk_func_8830867C_034* arg0);
s32 BattleInfoWidget_RunSelectionModal(unk_func_8830867C_034* arg0, Controller* arg1);
void BattleInfoWidget_DrawLabel(s32 arg0, s32 arg1, char* arg2, s32 arg3, s32 arg4);
void BattleInfoWidget_InitDropdownMenu(unk_func_8830867C_030* arg0, WidgetNode* arg1, MemoryPool* arg2);
void BattleInfoWidget_SetLabels(unk_func_8830867C_030* arg0, char** arg1, s32 arg2);
void BattleInfoWidget_UpdateViewport(unk_func_8830867C_030* arg0);
void BattleInfoWidget_SetSelectionState(unk_func_8830867C_030* arg0, s32 arg1);
s32 BattleInfoWidget_HandleSelectionInput(unk_func_8830867C_030* arg0, Controller* arg1);
void BattleInfoWidget_OpenSelectionAt(unk_func_8830867C_030* arg0, s32 arg1);
void BattleInfoWidget_CloseSelection(unk_func_8830867C_030* arg0);
u32 BattleInfoWidget_RunEntryModal(unk_func_8830867C_030* arg0, Controller* arg1, s32 arg2);
void BattleInfoWidget_DrawStatusMenuLabel(s32 arg0, s32 arg1, char* arg2, s32 arg3, s32 arg4);
void BattleInfoWidget_BuildStatusMenu(unk_func_8830867C_038* arg0, s32 arg1, s32 arg2, WidgetNode* arg3,
                                      MemoryPool* arg4);
void BattleInfoWidget_SetStatusSelection(unk_func_8830867C_038* arg0, s32 arg1);
s32 BattleInfoWidget_HandleStatusInput(unk_func_8830867C_038* arg0, Controller* arg1);
s32 BattleInfoWidget_RunStatusModal(unk_func_8830867C_038* arg0, Controller* arg1);
void BattleInfoWidget_UpdateNumberIndicator(unk_func_8830867C_044_038* arg0);
void BattleInfoWidget_InitNumberSelectorDigits(unk_func_8830867C_044_038* arg0, s32 arg1, s32 arg2, MemoryPool* arg3);
void BattleInfoWidget_SetNumberSelectorState(unk_func_8830867C_044_038* arg0, s32 arg1);
s32 BattleInfoWidget_DrawNumberSelector(unk_func_8830867C_044_038* arg0, s32 arg1, s32 arg2);
s32 BattleInfoWidget_HandleNumberSelectorInput(unk_func_8830867C_044_038* arg0, Controller* arg1);
void BattleInfoWidget_BuildLevelRangeMenu(unk_func_8830867C_044* arg0, s32 arg1, s32 arg2, WidgetNode* arg3,
                                          MemoryPool* arg4);
void BattleInfoWidget_SetLevelRange(unk_func_8830867C_044* arg0, s32 arg1, s32 arg2, s32 arg3);
void BattleInfoWidget_SetLevelRangeState(unk_func_8830867C_044* arg0, s32 arg1);
s32 BattleInfoWidget_HandleLevelRangeInput(unk_func_8830867C_044* arg0, Controller* arg1);
s32 BattleInfoWidget_RunLevelRangeModal(unk_func_8830867C_044* arg0, Controller* arg1);
void BattleInfoWidget_InitLevelRangeSelectionProxy(unk_func_8830867C_044_040* arg0, unk_func_8830867C_044* arg1);
s32 BattleInfoWidget_LevelRangeSelectionInput(unk_func_8830867C_044_040* arg0, Controller* arg1);
void BattleInfoWidget_CenterLabel(unk_func_8830867C_03C* arg0, unk_func_8850878C* arg1, char* arg2);
void BattleInfoWidget_BuildTypePanel(unk_func_8830867C_03C* arg0, s32 arg1, s32 arg2, WidgetNode* arg3,
                                     MemoryPool* arg4);
void BattleInfoWidget_SetTypeEntries(unk_func_8830867C_03C* arg0, unk_func_8830867C_02C_C54_01C* arg1);
void BattleInfoWidget_GetTypeEntries(unk_func_8830867C_03C* arg0, unk_func_8830867C_02C_C54_01C* arg1);
void BattleInfoWidget_SetTypePanelState(unk_func_8830867C_03C* arg0, s32 arg1);
s32 BattleInfoWidget_HandleTypePanelInput(unk_func_8830867C_03C* arg0, Controller* arg1);
s32 BattleInfoWidget_RunTypeSelectionModal(unk_func_8830867C_03C* arg0, Controller* arg1);
void BattleInfoWidget_InitTypeSelectionProxy(unk_func_8830867C_03C_04C* arg0, unk_func_8830867C_03C* arg1);
s32 BattleInfoWidget_HandleTypeSelectionProxyInput(unk_func_8830867C_03C_04C* arg0, Controller* arg1);
void func_8830FA10(void);
void func_8830FA18(void);
void BattleInfoWidget_CenterTypeLabel(unk_func_8830867C_040* arg0, unk_func_8850878C* arg1, char* arg2);
void BattleInfoWidget_BuildMovePanel(unk_func_8830867C_040* arg0, s32 arg1, s32 arg2, WidgetNode* arg3,
                                     MemoryPool* arg4);
void BattleInfoWidget_SetMoveSlots(unk_func_8830867C_040* arg0, unk_func_8830867C_02C_C54_024* arg1);
void BattleInfoWidget_GetMoveSlots(unk_func_8830867C_040* arg0, unk_func_8830867C_02C_C54_024* arg1);
void BattleInfoWidget_SetMovePanelState(unk_func_8830867C_040* arg0, s32 arg1);
s32 BattleInfoWidget_HandleMovePanelInput(unk_func_8830867C_040* arg0, Controller* arg1);
s32 BattleInfoWidget_RunMovePanelModal(unk_func_8830867C_040* arg0, Controller* arg1);
void BattleInfoWidget_InitMoveSelectionProxy(unk_func_8830867C_040_05C* arg0, unk_func_8830867C_040* arg1);
s32 func_88310854(unk_func_8830867C_040_05C* arg0, Controller* arg1);
void BattleInfoWidget_InitGamePakSlotLabel(unk_func_88310B70* arg0, s32 arg1, s32 arg2, unk_func_8830867C_04C_030* arg3,
                                           MemoryPool* arg4);
s32 BattleInfoWidget_DrawGamePakSlotLabel(unk_func_88310B70* arg0);
void BattleInfoWidget_BuildItemMenu(unk_func_8830867C_04C* arg0, s32 arg1, s32 arg2, WidgetNode* arg3,
                                    MemoryPool* arg4);
s32 LabPCList_CanLearnMoveByLevelUp(s32 arg0, s32 arg1);
s32 LabPCList_CheckTmHmIneligibility(s32 arg0, unk_func_8830867C_02C_0CC_000_008* arg1);
s32 LabPCList_CheckPpUpIneligibility(unk_func_8830867C_02C_0CC_000_008* arg0);
s32 LabPCList_CheckVitaminIneligibility(s32 arg0, unk_func_8830867C_02C_0CC_000_008* arg1);
void BattleInfoWidget_RebindItemGrid(unk_func_8830867C_04C* arg0, unk_func_88200FA0_030_038* arg1);
void BattleInfoWidget_SetItemMenuMon(unk_func_8830867C_04C* arg0, BattleMon* arg1);
void BattleInfoWidget_MarkIneligibleItems(unk_func_8830867C_04C* arg0);
void BattleInfoWidget_UpdateItemMenuTitle(unk_func_8830867C_04C* arg0);
void BattleInfoWidget_SetItemMenuMode(unk_func_8830867C_04C* arg0, s32 arg1);
void BattleInfoWidget_SetItemMenuState(unk_func_8830867C_04C* arg0, s32 arg1);
s32 BattleInfoWidget_GetItemMenuActionType(unk_func_8830867C_04C* arg0, s32* arg1);
void BattleInfoWidget_InitItemMenuSelectionProxy(unk_func_8830867C_04C_080* arg0, unk_func_8830867C_04C* arg1);
s32 BattleInfoWidget_ItemMenuBackToBrowse(unk_func_8830867C_04C_080* arg0);
s32 BattleInfoWidget_ItemMenuBackFromMoveSelect(unk_func_8830867C_04C_080* arg0);
void BattleInfoWidget_ItemMenuConfirmSelection(unk_func_8830867C_04C_080* arg0);
s32 BattleInfoWidget_ItemMenuHandleInput(unk_func_8830867C_04C_080* arg0, Controller* arg1);
s32 BattleInfoWidget_ItemMenuHandleContainerInput(unk_func_8830867C_04C* arg0, Controller* arg1);
void BattleInfoWidget_OpenItemMenu(unk_func_8830867C_04C* arg0);
void BattleInfoWidget_CloseItemMenu(unk_func_8830867C_04C* arg0);
s32 BattleInfoWidget_RunItemMenuModal(unk_func_8830867C_04C* arg0, Controller* arg1);
void BattleInfoWidget_DrawSlotEntry(s32 arg0, s32 arg1, unk_func_88309160_arg2* arg2, s32 arg3, s32 arg4,
                                    unk_func_88200FA0_030_030_1CEA00* arg5);
void BattleInfoWidget_DrawLocationEntry(s32 arg0, s32 arg1, unk_func_88309160_arg2* arg2, s32 arg3, s32 arg4,
                                        unk_func_88200FA0_030_030_1CEA00* arg5);
void BattleInfoWidget_BuildMoveDestinationMenu(unk_func_8830867C_048* arg0, s32 arg1, s32 arg2, WidgetNode* arg3,
                                               MemoryPool* arg4);
void BattleInfoWidget_BindMoveDestinationData(unk_func_8830867C_048* arg0, unk_func_8830867C_02C_144* arg1);
void BattleInfoWidget_MarkFullBoxSlots(unk_func_88200FA0_030_030_1CEA00* arg0, unk_func_8830867C_02C_0CC_000* arg1,
                                       unk_func_88313894_020* arg2);
void BattleInfoWidget_MarkIneligibleDestinations(unk_func_8830867C_048* arg0, unk_func_8830867C_02C_0CC_000* arg1);
void BattleInfoWidget_FormatLocationName(char* arg0, s32 arg1, s32 arg2);
void BattleInfoWidget_SetMoveDestinationTitle(unk_func_8830867C_048* arg0, s32 arg1, s32 arg2);
void BattleInfoWidget_SyncDestinationPage(unk_func_8830867C_048* arg0);
void BattleInfoWidget_OpenMoveDestinationMenu(unk_func_8830867C_048* arg0);
void BattleInfoWidget_CloseMoveDestinationMenu(unk_func_8830867C_048* arg0);
void BattleInfoWidget_SetMoveDestinationState(unk_func_8830867C_048* arg0, s32 arg1);
s32 BattleInfoWidget_HandleMoveDestinationContainerInput(unk_func_8830867C_048* arg0, Controller* arg1);
void BattleInfoWidget_InitMoveDestinationSelectionProxy(unk_func_8830867C_048_048* arg0, unk_func_8830867C_048* arg1);
s32 BattleInfoWidget_MoveDestinationSelectionInput(unk_func_8830867C_048_048* arg0, Controller* arg1);
s32 BattleInfoWidget_RunMoveDestinationModal(unk_func_8830867C_048* arg0, Controller* arg1);
void BattleInfoWidget_DrawSaveConfirmLabel(s32 arg0, s32 arg1, s8* arg2, s32 arg3);
void BattleInfoWidget_BuildSaveConfirmDialog(unk_func_8830867C_02C_0B4* arg0, s32 arg1, s32 arg2, MemoryPool* arg3);
s32 BattleInfoWidget_HandleSaveConfirmInput(unk_func_8830867C_02C_0B4* arg0, Controller* arg1);
s32 BattleInfoWidget_RunSaveConfirmModal(unk_func_8830867C_02C_0B4* arg0, Controller* arg1);
void LabPCList_InitTextFadeWidget(unk_func_882173EC* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);
s32 LabPCList_TextFadeWidget_Update(unk_func_882173EC* arg0);
s32 LabPCList_DrawTextFadeWidget(unk_func_882173EC* arg0, s32 arg1, s32 arg2);
void LabPCList_TextFadeWidget_Start(unk_func_882173EC* arg0, s32 arg1, s32 arg2, WidgetPoint arg3);
void LabPCList_InitPpUpAnimWidget(unk_func_8830867C_02C_0C0* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);
s32 LabPCList_PpUpAnimWidget_Update(unk_func_8830867C_02C_0C0* arg0);
s32 LabPCList_DrawPpUpAnimWidget(unk_func_8830867C_02C_0C0* arg0, s32 arg1, s32 arg2);
void LabPCList_PpUpAnimWidget_Start(unk_func_8830867C_02C_0C0* arg0, s32 arg1, s32 arg2, s32 arg3, WidgetPoint arg4);
void LabPCList_InitVitaminAnimWidget(unk_func_882173EC* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);
s32 LabPCList_VitaminAnimWidget_Update(unk_func_882173EC* arg0);
s32 LabPCList_DrawVitaminAnimWidget(unk_func_882173EC* arg0, s32 arg1, s32 arg2);
void LabPCList_TextCrossfadeWidget_Start(unk_func_882173EC* arg0, char* arg1, char* arg2, WidgetPoint arg3);
void LabPCList_InitLocationCrossfadeWidget(unk_func_882173EC* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);
s32 LabPCList_LocationCrossfadeWidget_Update(unk_func_882173EC* arg0);
void LabPCList_DrawLocationCaption(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6);
s32 LabPCList_DrawLocationCrossfadeWidget(unk_func_882173EC* arg0, s32 arg1, s32 arg2);
void LabPCList_LocationCrossfadeWidget_Start(unk_func_882173EC* arg0, s32 arg1, s32 arg2, WidgetPoint arg3);

s32 LabPCList_CompareByPointer(unk_func_88316128*, unk_func_88316128*);
s32 LabPCList_CompareByLevelAsc(unk_func_88316128*, unk_func_88316128*);
s32 LabPCList_CompareByLevelDesc(unk_func_88316128*, unk_func_88316128*);
s32 LabPCList_CompareByHpAsc(unk_func_88316128*, unk_func_88316128*);
s32 LabPCList_CompareByHpDesc(unk_func_88316128*, unk_func_88316128*);
s32 LabPCList_CompareByAttackAsc(unk_func_88316128*, unk_func_88316128*);
s32 LabPCList_CompareByAttackDesc(unk_func_88316128*, unk_func_88316128*);
s32 LabPCList_CompareByDefenseAsc(unk_func_88316128*, unk_func_88316128*);
s32 LabPCList_CompareByDefenseDesc(unk_func_88316128*, unk_func_88316128*);
s32 LabPCList_CompareBySpeedAsc(unk_func_88316128*, unk_func_88316128*);
s32 LabPCList_CompareBySpeedDesc(unk_func_88316128*, unk_func_88316128*);
s32 LabPCList_CompareBySpecialAsc(unk_func_88316128*, unk_func_88316128*);
s32 LabPCList_CompareBySpecialDesc(unk_func_88316128*, unk_func_88316128*);
s32 LabPCList_CompareById(unk_func_88316128*, unk_func_88316128*);
s32 LabPCList_CompareByPokemonColumn(unk_func_8830867C_02C_0CC*, unk_func_8830867C_02C_0CC*);
s32 LabPCList_CompareByOtName(unk_func_88316128* arg0, unk_func_88316128* arg1);
s32 LabPCList_CompareByNickname(unk_func_88316128*, unk_func_88316128*);
s32 LabPCList_CompareBySpeciesDexOrder(unk_func_8830867C_02C_0CC*, unk_func_8830867C_02C_0CC*);
s32 LabPCList_CompareBySpeciesRawId(unk_func_8830867C_02C_0CC*, unk_func_8830867C_02C_0CC*);
int LabPCList_FilterByLevelRange(unk_func_8830867C_02C_0CC* arg0, u8* arg1);
s32 LabPCList_FilterByAnyType(unk_func_8830867C_02C_0CC* arg0, unk_func_8830867C_02C_C54_01C* arg1);
s32 LabPCList_FilterByAnyMove(unk_func_8830867C_02C_0CC* arg0, unk_func_8830867C_02C_C54_024* arg1);
s32 LabPCList_FilterBySpeciesId(unk_func_8830867C_02C_0CC* arg0, s32* arg1);
s32 LabPCList_FilterByExactTypePair(unk_func_8830867C_02C_0CC* arg0, unk_func_8830867C_02C_C54_014* arg1);
s32 LabPCList_FilterByCategoryBit(unk_func_88316128* arg0, u8* arg1);
void LabPCList_AllocateBoxSlots(unk_func_8830867C_02C_0E4*, s32, s32, MemoryPool*);
void LabPCList_LoadAllBoxes(unk_func_8830867C_02C_144*);
void LabPCList_FlattenEntries(unk_func_88200FA0_030_038*, unk_func_8830867C_02C_144*);
s32 LabPCList_CheckDirtyBoxes(unk_func_8830867C_02C_144*);
void LabPCList_SaveDirtyBoxes(unk_func_8830867C_02C_144*, s32);
void LabPCList_InitDeckCategories(unk_func_8830867C_02C_154*, MemoryPool* arg1);
void LabPCList_ReadDeckCategory(unk_func_8830867C_02C_154*);
void LabPCList_WriteDeckCategory(unk_func_8830867C_02C_154*);
void LabPCList_BuildTmHmList(unk_func_88200FA0_030_038*, unk_func_8830867C_02C_1A8*, unk_func_8830867C_02C_154*, s32,
                             s32);
void LabPCList_ConsumeUsedTmItem(unk_func_8830867C_04C_030_02C_000*);

#endif // _FRAGMENT24_H_
