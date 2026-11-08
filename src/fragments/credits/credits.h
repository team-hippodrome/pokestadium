#ifndef _FRAGMENT16_H_
#define _FRAGMENT16_H_

#include "global.h"

/**
 * One entry of sBackgroundSegments: how long one randomly picked arena stays on screen
 * behind the staff roll before Credits_MainLoop fades to the next one.
 */
typedef struct CreditsBackgroundSegment {
    /* 0x00 */ s8 endMarker; // 0 for a timed segment; -1 ends the arena sequence
    /* 0x01 */ s8 unk_01;    // 0-7 in order; no reader in this fragment
    /* 0x02 */ u16 duration; // frames; rewritten at runtime to absorb frame-delta overshoot
} CreditsBackgroundSegment; // size = 0x4

/**
 * One centered line of the staff roll. Credits_InitRoleList fills text[0] from textdata
 * table 45; text[1] keeps its static per-line string (empty in US builds).
 * sRoleTextColumn selects which column Credits_DrawRoleList prints.
 */
typedef struct CreditsRoleLine {
    /* 0x00 */ s32 style;   // 0 = role heading, 1 = staff name, 2 = 10-pixel blank spacer
    /* 0x04 */ char* text[2];
} CreditsRoleLine; // size = 0xC

/**
 * One page of the staff roll: lineCount consecutive CreditsRoleLine entries that fade in,
 * hold and fade out together. Pages are consumed in order by Credits_DrawRoleList; a page
 * with duration 0 ends the roll and switches to the Pokemon Stadium logo.
 */
typedef struct CreditsRolePage {
    /* 0x00 */ u16 timer;       // frames elapsed on this page, advanced by sFrameDelta
    /* 0x02 */ u16 duration;    // frames before the next page starts
    /* 0x04 */ u8 blankFrames;  // trailing frames of the page with no text drawn
    /* 0x05 */ u8 lineCount;
    /* 0x08 */ CreditsRoleLine* lines; // only page 0 is static; later pages are chained at runtime
} CreditsRolePage; // size = 0xC

/**
 * Orbit camera move used by Credits_InitCameraOrbitBlend. The camera keeps a fixed
 * rotationX and distance while rotationY is stepped from rotationYStart to rotationYEnd
 * over the segment. Angles are binary angles (0x10000 = one turn).
 */
typedef struct CreditsOrbitShot {
    /* 0x00 */ s16 rotationYStart;
    /* 0x02 */ s16 rotationYEnd;
    /* 0x04 */ s8 rotationYDirection; // 1 = increasing rotationY, -1 = decreasing (wraps through 0x10000)
    /* 0x06 */ s16 rotationX;
    /* 0x08 */ s16 distance;          // eye distance from the orbit center
    /* 0x0C */ f32 fieldOfView;       // vertical field of view, degrees
    /* 0x10 */ f32 targetHeight;      // y of the look-at point (x and z are 0 for shots 0-5)
    /* 0x14 */ f32 nearPlane;
    /* 0x18 */ f32 farPlane;
} CreditsOrbitShot; // size = 0x1C

/**
 * Keyframed camera move used by Credits_InitCameraKeyframeBlend: the eye, look direction and
 * field of view are linearly interpolated from the start to the end values over the segment.
 * Look directions are in degrees and converted to binary angles; the target is placed 720
 * units from the eye along that direction.
 */
typedef struct CreditsKeyframeShot {
    /* 0x00 */ Vec3f eyeStart;
    /* 0x0C */ f32 rotationXStart;
    /* 0x10 */ f32 rotationYStart;
    /* 0x14 */ Vec3f eyeEnd;
    /* 0x20 */ f32 rotationXEnd;
    /* 0x24 */ f32 rotationYEnd;
    /* 0x28 */ f32 fieldOfViewStart;
    /* 0x2C */ f32 fieldOfViewEnd;
    /* 0x30 */ f32 nearPlane;
    /* 0x34 */ f32 farPlane;
} CreditsKeyframeShot; // size = 0x38

// Orbit shots 0-5 for arenas 0-16; arena 17's row is sOrbitShotsArena17 (see credits_15A2B0.c).
extern CreditsOrbitShot sOrbitShots[17][6];

// Frames the end logo has been fully opaque; Credits_MainLoop ends at 721, or at 361+ on A/Start.
extern u16 sLogoHoldTimer;

// Initial text[0]/text[1] pointers for the 187 sRoleLines entries, in pairs. Every one is an
// empty string in US builds. Credits_InitRoleList replaces each text[0] with a string from
// textdata table 45; text[1] keeps its placeholder. Other regions place these at other addresses.
//@src/fragments/credits/credits_15AD80.c
extern char D_86A05B60;
extern char D_86A05B64;
extern char D_86A05B68;
extern char D_86A05B6C;
extern char D_86A05B70;
extern char D_86A05B74;
extern char D_86A05B78;
extern char D_86A05B7C;
extern char D_86A05B80;
extern char D_86A05B84;
extern char D_86A05B88;
extern char D_86A05B8C;
extern char D_86A05B90;
extern char D_86A05B94;
extern char D_86A05B98;
extern char D_86A05B9C;
extern char D_86A05BA0;
extern char D_86A05BA4;
extern char D_86A05BA8;
extern char D_86A05BAC;
extern char D_86A05BB0;
extern char D_86A05BB4;
extern char D_86A05BB8;
extern char D_86A05BBC;
extern char D_86A05BC0;
extern char D_86A05BC4;
extern char D_86A05BC8;
extern char D_86A05BCC;
extern char D_86A05BD0;
extern char D_86A05BD4;
extern char D_86A05BD8;
extern char D_86A05BDC;
extern char D_86A05BE0;
extern char D_86A05BE4;
extern char D_86A05BE8;
extern char D_86A05BEC;
extern char D_86A05BF0;
extern char D_86A05BF4;
extern char D_86A05BF8;
extern char D_86A05BFC;
extern char D_86A05C00;
extern char D_86A05C04;
extern char D_86A05C08;
extern char D_86A05C0C;
extern char D_86A05C10;
extern char D_86A05C14;
extern char D_86A05C18;
extern char D_86A05C1C;
extern char D_86A05C20;
extern char D_86A05C24;
extern char D_86A05C28;
extern char D_86A05C2C;
extern char D_86A05C30;
extern char D_86A05C34;
extern char D_86A05C38;
extern char D_86A05C3C;
extern char D_86A05C40;
extern char D_86A05C44;
extern char D_86A05C48;
extern char D_86A05C4C;
extern char D_86A05C50;
extern char D_86A05C54;
extern char D_86A05C58;
extern char D_86A05C5C;
extern char D_86A05C60;
extern char D_86A05C64;
extern char D_86A05C68;
extern char D_86A05C6C;
extern char D_86A05C70;
extern char D_86A05C74;
extern char D_86A05C78;
extern char D_86A05C7C;
extern char D_86A05C80;
extern char D_86A05C84;
extern char D_86A05C88;
extern char D_86A05C8C;
extern char D_86A05C90;
extern char D_86A05C94;
extern char D_86A05C98;
extern char D_86A05C9C;
extern char D_86A05CA0;
extern char D_86A05CA4;
extern char D_86A05CA8;
extern char D_86A05CAC;
extern char D_86A05CB0;
extern char D_86A05CB4;
extern char D_86A05CB8;
extern char D_86A05CBC;
extern char D_86A05CC0;
extern char D_86A05CC4;
extern char D_86A05CC8;
extern char D_86A05CCC;
extern char D_86A05CD0;
extern char D_86A05CD4;
extern char D_86A05CD8;
extern char D_86A05CDC;
extern char D_86A05CE0;
extern char D_86A05CE4;
extern char D_86A05CE8;
extern char D_86A05CEC;
extern char D_86A05CF0;
extern char D_86A05CF4;
extern char D_86A05CF8;
extern char D_86A05CFC;
extern char D_86A05D00;
extern char D_86A05D04;
extern char D_86A05D08;
extern char D_86A05D0C;
extern char D_86A05D10;
extern char D_86A05D14;
extern char D_86A05D18;
extern char D_86A05D1C;
extern char D_86A05D20;
extern char D_86A05D24;
extern char D_86A05D28;
extern char D_86A05D2C;
extern char D_86A05D30;
extern char D_86A05D34;
extern char D_86A05D38;
extern char D_86A05D3C;
extern char D_86A05D40;
extern char D_86A05D44;
extern char D_86A05D48;
extern char D_86A05D4C;
extern char D_86A05D50;
extern char D_86A05D54;
extern char D_86A05D58;
extern char D_86A05D5C;
extern char D_86A05D60;
extern char D_86A05D64;
extern char D_86A05D68;
extern char D_86A05D6C;
extern char D_86A05D70;
extern char D_86A05D74;
extern char D_86A05D78;
extern char D_86A05D7C;
extern char D_86A05D80;
extern char D_86A05D84;
extern char D_86A05D88;
extern char D_86A05D8C;
extern char D_86A05D90;
extern char D_86A05D94;
extern char D_86A05D98;
extern char D_86A05D9C;
extern char D_86A05DA0;
extern char D_86A05DA4;
extern char D_86A05DA8;
extern char D_86A05DAC;
extern char D_86A05DB0;
extern char D_86A05DB4;
extern char D_86A05DB8;
extern char D_86A05DBC;
extern char D_86A05DC0;
extern char D_86A05DC4;
extern char D_86A05DC8;
extern char D_86A05DCC;
extern char D_86A05DD0;
extern char D_86A05DD4;
extern char D_86A05DD8;
extern char D_86A05DDC;
extern char D_86A05DE0;
extern char D_86A05DE4;
extern char D_86A05DE8;
extern char D_86A05DEC;
extern char D_86A05DF0;
extern char D_86A05DF4;
extern char D_86A05DF8;
extern char D_86A05DFC;
extern char D_86A05E00;
extern char D_86A05E04;
extern char D_86A05E08;
extern char D_86A05E0C;
extern char D_86A05E10;
extern char D_86A05E14;
extern char D_86A05E18;
extern char D_86A05E1C;
extern char D_86A05E20;
extern char D_86A05E24;
extern char D_86A05E28;
extern char D_86A05E2C;
extern char D_86A05E30;
extern char D_86A05E34;
extern char D_86A05E38;
extern char D_86A05E3C;
extern char D_86A05E40;
extern char D_86A05E44;
extern char D_86A05E48;
extern char D_86A05E4C;
extern char D_86A05E50;
extern char D_86A05E54;
extern char D_86A05E58;
extern char D_86A05E5C;
extern char D_86A05E60;
extern char D_86A05E64;
extern char D_86A05E68;
extern char D_86A05E6C;
extern char D_86A05E70;
extern char D_86A05E74;
extern char D_86A05E78;
extern char D_86A05E7C;
extern char D_86A05E80;
extern char D_86A05E84;
extern char D_86A05E88;
extern char D_86A05E8C;
extern char D_86A05E90;
extern char D_86A05E94;
extern char D_86A05E98;
extern char D_86A05E9C;
extern char D_86A05EA0;
extern char D_86A05EA4;
extern char D_86A05EA8;
extern char D_86A05EAC;
extern char D_86A05EB0;
extern char D_86A05EB4;
extern char D_86A05EB8;
extern char D_86A05EBC;
extern char D_86A05EC0;
extern char D_86A05EC4;
extern char D_86A05EC8;
extern char D_86A05ECC;
extern char D_86A05ED0;
extern char D_86A05ED4;
extern char D_86A05ED8;
extern char D_86A05EDC;
extern char D_86A05EE0;
extern char D_86A05EE4;
extern char D_86A05EE8;
extern char D_86A05EEC;
extern char D_86A05EF0;
extern char D_86A05EF4;
extern char D_86A05EF8;
extern char D_86A05EFC;
extern char D_86A05F00;
extern char D_86A05F04;
extern char D_86A05F08;
extern char D_86A05F0C;
extern char D_86A05F10;
extern char D_86A05F14;
extern char D_86A05F18;
extern char D_86A05F1C;
extern char D_86A05F20;
extern char D_86A05F24;
extern char D_86A05F28;
extern char D_86A05F2C;
extern char D_86A05F30;
extern char D_86A05F34;
extern char D_86A05F38;
extern char D_86A05F3C;
extern char D_86A05F40;
extern char D_86A05F44;
extern char D_86A05F48;
extern char D_86A05F4C;
extern char D_86A05F50;
extern char D_86A05F54;
extern char D_86A05F58;
extern char D_86A05F5C;
extern char D_86A05F60;
extern char D_86A05F64;
extern char D_86A05F68;
extern char D_86A05F6C;
extern char D_86A05F70;
extern char D_86A05F74;
extern char D_86A05F78;
extern char D_86A05F7C;
extern char D_86A05F80;
extern char D_86A05F84;
extern char D_86A05F88;
extern char D_86A05F8C;
extern char D_86A05F90;
extern char D_86A05F94;
extern char D_86A05F98;
extern char D_86A05F9C;
extern char D_86A05FA0;
extern char D_86A05FA4;
extern char D_86A05FA8;
extern char D_86A05FAC;
extern char D_86A05FB0;
extern char D_86A05FB4;
extern char D_86A05FB8;
extern char D_86A05FBC;
extern char D_86A05FC0;
extern char D_86A05FC4;
extern char D_86A05FC8;
extern char D_86A05FCC;
extern char D_86A05FD0;
extern char D_86A05FD4;
extern char D_86A05FD8;
extern char D_86A05FDC;
extern char D_86A05FE0;
extern char D_86A05FE4;
extern char D_86A05FE8;
extern char D_86A05FEC;
extern char D_86A05FF0;
extern char D_86A05FF4;
extern char D_86A05FF8;
extern char D_86A05FFC;
extern char D_86A06000;
extern char D_86A06004;
extern char D_86A06008;
extern char D_86A0600C;
extern char D_86A06010;
extern char D_86A06014;
extern char D_86A06018;
extern char D_86A0601C;
extern char D_86A06020;
extern char D_86A06024;
extern char D_86A06028;
extern char D_86A0602C;
extern char D_86A06030;
extern char D_86A06034;
extern char D_86A06038;
extern char D_86A0603C;
extern char D_86A06040;
extern char D_86A06044;
extern char D_86A06048;
extern char D_86A0604C;
extern char D_86A06050;
extern char D_86A06054;
extern char D_86A06058;
extern char D_86A0605C;
extern char D_86A06060;
extern char D_86A06064;
extern char D_86A06068;
extern char D_86A0606C;
extern char D_86A06070;
extern char D_86A06074;
extern char D_86A06078;
extern char D_86A0607C;
extern char D_86A06080;
extern char D_86A06084;
extern char D_86A06088;
extern char D_86A0608C;
extern char D_86A06090;
extern char D_86A06094;
extern char D_86A06098;
extern char D_86A0609C;
extern char D_86A060A0;
extern char D_86A060A4;
extern char D_86A060A8;
extern char D_86A060AC;
extern char D_86A060B0;
extern char D_86A060B4;
extern char D_86A060B8;
extern char D_86A060BC;
extern char D_86A060C0;
extern char D_86A060C4;
extern char D_86A060C8;
extern char D_86A060CC;
extern char D_86A060D0;
extern char D_86A060D4;
extern char D_86A060D8;
extern char D_86A060DC;
extern char D_86A060E0;
extern char D_86A060E4;
extern char D_86A060E8;
extern char D_86A060EC;
extern char D_86A060F0;
extern char D_86A060F4;
extern char D_86A060F8;
extern char D_86A060FC;
extern char D_86A06100;
extern char D_86A06104;
extern char D_86A06108;
extern char D_86A0610C;
extern char D_86A06110;
extern char D_86A06114;
extern char D_86A06118;
extern char D_86A0611C;
extern char D_86A06120;
extern char D_86A06124;
extern char D_86A06128;
extern char D_86A0612C;
extern char D_86A06130;
extern char D_86A06134;

// Retrace (video frame) count since the previous Credits_UpdateFrameDelta; every timer steps by it.
extern u8 sFrameDelta;
// Selects CreditsRoleLine.text[0] or text[1]; cleared by Credits_InitRoleList and not written
// elsewhere in this fragment. While 0, Credits_SelectNextScene forces pick 2 to arena 6.
extern u8 sRoleTextColumn;

// Center for orbit shots 6-8: (0, targetHeight, 0) from sPivotOrbitShots.
extern Vec3f sOrbitPivot;

s32 Credits_BackgroundNodePostCallback(s32 event, GraphNode* node);
void Credits_InitRoleList(u8 initial_frame_delta, u8 is_round_1, CreditsRolePage* pages);
void Credits_UpdateFrameDelta(void);
void Credits_DrawRoleList(void);

u16 Credits_GetRoleLineCount(void);

void Credits_InitCameraKeyframeBlend(CameraNode* camera, u16 duration, s8 arena_index, u8 shot);
void Credits_StepCameraKeyframeBlend(void);
void Credits_InitCameraOrbitBlend(s16* rotation_x, s16* rotation_y, s16* distance, CameraNode* camera, u16 duration,
                                  s8 arena_index, u8 shot);
void Credits_StepCameraOrbitBlend(void);
void Credits_SelectNextScene(s8* arena_index, u8* shot);

s32 Credits_RandomRange(s32 range);
void Credits_DrawStadiumLogo(u8 fade_in);

#endif // _FRAGMENT16_H_
