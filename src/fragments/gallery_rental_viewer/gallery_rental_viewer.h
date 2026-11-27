#ifndef _FRAGMENT47_H_
#define _FRAGMENT47_H_

#include "global.h"
#include "src/gallery.h"
#include "src/save_data.h"
#include "src/geo_layout.h"
#include "src/fragments/gallery_backgrounds/gallery_backgrounds.h"

typedef struct unk_D_8380565C {
    /* 0x000 */ u32 unk_000;
    /* 0x004 */ BattleMon unk_004[3];
    /* 0x100 */ char unk100[0x6E4];
    /* 0x7E4 */ BattleMon unk_7E4;
} unk_D_8380565C; // size >= 0x838

typedef struct unk_D_838067F0_0168_0000 {
    union {
        struct {
            /* 0x000 */ u8 unk_000;
            /* 0x001 */ u8 unk_001;
            /* 0x002 */ u8 unk_002;
            /* 0x003 */ u8 unk_003;
        };
        s32 raw;
    };
    /* 0x004 */ BattleMon unk_004;
#if defined(VERSION_JP)
    /* 0x054 */ char unk054[0x4];
#endif
    /* 0x058 */ u16 unk_058[0x640];
} unk_D_838067F0_0168_0000; // size = 0xCD8

typedef struct unk_D_838067F0_0168 {
    /* 0x0000 */ unk_D_838067F0_0168_0000 unk_0000[3];
    /* 0x2688 */ struct unk_D_838067F0_0168* unk_2688;
    /* 0x268C */ struct unk_D_838067F0_0168* unk_268C;
    /* 0x2690 */ s16 unk_2690;
    /* 0x2692 */ s16 unk_2692;
    /* 0x2694 */ char unk2694[0x4];
} unk_D_838067F0_0168; // size = 0x2698

// One GB box (or the party): how many of its Pokémon are usable, and their slots.
typedef struct unk_D_838067F0_003C {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ u8 unk_02[SAVE_BOX_CAPACITY];
} unk_D_838067F0_003C; // size = 0x16, 0x20 in JP

typedef struct unk_D_838067F0 {
    /* 0x0000 */ s32 unk_0000;
    /* 0x0004 */ s32 unk_0004;
    /* 0x0008 */ s32 unk_0008;
    /* 0x000C */ s32 unk_000C;
    /* 0x0010 */ s32 unk_0010;
    /* 0x0014 */ s32 unk_0014;
    /* 0x0018 */ s32 unk_0018;
    /* 0x001C */ s32 unk_001C;
    /* 0x0020 */ s32 unk_0020;
    /* 0x0024 */ s32 unk_0024;
    /* 0x0028 */ s32 unk_0028;
    /* 0x002C */ s32 unk_002C;
    /* 0x0030 */ s32 unk_0030;
    /* 0x0034 */ s32 unk_0034;
    /* 0x0038 */ s32 unk_0038;
    /* 0x003C */ unk_D_838067F0_003C unk_003C[13];
    /* 0x015C */ unk_D_8380565C* unk_015C;
    /* 0x0160 */ unk_D_838067F0_0168* unk_0160;
    /* 0x0164 */ char unk0164[0x4];
    /* 0x0168 */ unk_D_838067F0_0168 unk_0168[12];
} unk_D_838067F0; // size = 0x1D088

typedef struct unk_D_8380548C {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A;
    /* 0x0C */ s16 unk_0C;
} unk_D_8380548C; // size = 0xE

void Gallery_RentalViewerInit(s32 arg0);
void Gallery_RentalViewerSetFadeCounter(s32 arg0);
void Gallery_RentalViewerDrawIcon(s32 arg0, s32 arg1);
void Gallery_RentalViewerDrawTitleBar(void);
void Gallery_RentalViewerDrawFrame(void);
void Gallery_RentalViewerLoadPort(s32 arg0);
void Gallery_RentalViewerUpdateLoadConfirm(void);
s32 Gallery_RentalViewerHandleInput(void);
void Gallery_RentalViewerPollInput(void);
void Gallery_RentalViewerLoop(void);
s32 Gallery_RentalViewerGetResult(void);
s32 Gallery_RentalViewerMain(s32 arg0, s32 arg1);

s32 Gallery_RentalViewerNodePostCallback(s32 arg0, GraphNode* arg1);
void Gallery_RentalViewerAnimateSlotScale(s32 arg0, f32 arg1);
void Gallery_RentalViewerAnimateSlotBob(s32 arg0, f32 arg1);
void Gallery_RentalViewerRefreshPortStatus(void);
void Gallery_RentalViewerUpdatePortIcons(void);
void Gallery_RentalViewerInitScene(void);
void Gallery_RentalViewerInitSlotModels(void);
s32 Gallery_RentalViewerBeginZoomIntro(s32 arg0);
void Gallery_RentalViewerUpdateZoomIntro(void);
void Gallery_RentalViewerMoveCursorUp(void);
void Gallery_RentalViewerMoveCursorDown(void);
void Gallery_RentalViewerMoveCursorLeft(void);
void Gallery_RentalViewerMoveCursorRight(void);
void Gallery_RentalViewerUpdateSlotSelect(void);
void Gallery_RentalViewerBeginConfirmZoom(void);
void Gallery_RentalViewerFinishConfirmZoom(void);
void Gallery_RentalViewerCancelSelection(void);
void Gallery_RentalViewerUpdateBusyConfirm(void);
void Gallery_RentalViewerUpdate(void);
void Gallery_RentalViewerDrawCursorMarker(void);
void Gallery_RentalViewerDrawSlotFrame(s32 arg0, s16 arg1, s32 arg2);
void Gallery_RentalViewerDrawSlotLabel(s32 arg0);
void Gallery_RentalViewerDrawLabels(void);
void Gallery_RentalViewerDrawTeamList(s32 arg0);
s32 Gallery_RentalViewerGetSelectedSlot(void);

void Gallery_RentalViewerSaveBoxCursor(unk_D_83407B18_008* arg0);
s32 Gallery_RentalViewerGetLoadState(void);
void Gallery_RentalViewerInitBoxPicker(s32 arg0, s32 arg1, unk_D_8380565C* arg2);
s32 Gallery_RentalViewerLoadDeckEntries(unk_D_838067F0* arg0, s32 arg1);
void Gallery_RentalViewerMarkMatchingPikachu(unk_D_838067F0* arg0, BattleMon* arg1);
s32 Gallery_RentalViewerFillPage(unk_D_838067F0* arg0, unk_D_838067F0_0168* arg1, s32 arg2, unk_D_8380565C* arg3);
s32 Gallery_RentalViewerOpenBoxPicker(s32 arg0, unk_D_83407B18_008* arg1);
void Gallery_RentalViewerAdvanceOpenPanel(unk_D_838067F0* arg0);
unk_D_838067F0_0168* Gallery_RentalViewerGetPage(unk_D_838067F0_0168* arg0, s32 arg1);
void Gallery_RentalViewerConfirmMonSelect(unk_D_838067F0* arg0);
void Gallery_RentalViewerCloseBoxPicker(unk_D_838067F0* arg0);
void Gallery_RentalViewerNextBox(unk_D_838067F0* arg0);
void Gallery_RentalViewerPrevBox(unk_D_838067F0* arg0);
void Gallery_RentalViewerBoxPickerMoveLeft(unk_D_838067F0* arg0);
void Gallery_RentalViewerBoxPickerMoveRight(unk_D_838067F0* arg0);
void Gallery_RentalViewerBoxPickerMoveUp(unk_D_838067F0* arg0);
void Gallery_RentalViewerBoxPickerMoveDown(unk_D_838067F0* arg0);
void Gallery_RentalViewerUpdateBoxPickerInput(unk_D_838067F0* arg0);
void Gallery_RentalViewerUpdateBoxPickerSlide(unk_D_838067F0* arg0);
void Gallery_RentalViewerAdvanceClosePanel(unk_D_838067F0* arg0);
void Gallery_RentalViewerUpdateBoxPicker(void);
void Gallery_RentalViewerDrawTitleBarStrip(s16 arg0, s16 arg1, s16 arg2);
void Gallery_RentalViewerDrawBoxTabIcon(s16 arg0, s16 arg1, u8* arg2, s16 arg3);
void Gallery_RentalViewerDrawBoxTabBar(unk_D_838067F0* arg0);
void Gallery_RentalViewerDrawScrollbar(s32 arg0, s32 arg1, unk_D_838067F0* arg2);
s32 Gallery_RentalViewerCountBoxes(unk_D_838067F0* arg0);
s32 Gallery_RentalViewerGetPageFillState(unk_D_838067F0_0168* arg0);
void Gallery_RentalViewerDrawPageHeader(s32 arg0, s32 arg1, unk_D_838067F0_0168* arg2);
void Gallery_RentalViewerDrawMonRow(s32 arg0, s32 arg1, unk_D_838067F0_0168* arg2);
void func_838043F8(s32 arg0);

#endif // _FRAGMENT47_H_
