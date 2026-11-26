#include "credits.h"
#include "src/graphics_textures.h"
#include "src/text_system.h"
#include "src/stage_loader.h"
#include "src/scheduler.h"

/**
 * Staff-roll color keyframe read by Credits_InterpolateRoleColor. colors[0]/[1] are the
 * env/prim colors for role headings and colors[2]/[3] those for staff names.
 */
typedef struct CreditsRoleColorKeyframe {
    /* 0x00 */ u16 frame; // page timer value at which this keyframe applies
    /* 0x02 */ u8 alpha;
    /* 0x03 */ Color_RGB8 colors[4];
} CreditsRoleColorKeyframe; // size = 0x10

u8 sFrameDelta;
u8 sRoleTextColumn;
u8 sFrameDeltaPrimed;              // 0 until Credits_UpdateFrameDelta has a previous retrace count
CreditsRolePage* sCurrentRolePage; // NULL after the last page; the end logo is drawn instead

// Page color/alpha keyframes: invisible at frame 0, opaque with lighter colors at frame 8,
// settled colors from frame 30, then a 20-frame fade out. Frames for keyframes 3 and 4 are
// rewritten per page by Credits_InterpolateRoleColor.
CreditsRoleColorKeyframe sRoleColorKeyframes[] = {
    {
        0,
        0,
        {
            { 0xFF, 0, 0xE6 },    // #FF00E6
            { 0xFF, 0xC8, 0xFF }, // #FFC8FF
            { 0, 0x46, 0xFF },    // #0046FF
            { 0x64, 0xFF, 0xFF }, // #64FFFF
        },
    },
    {
        8,
        0xFF,
        {
            { 0xFF, 0xC8, 0xFF }, // #FFC8FF
            { 0xFF, 0xFF, 0xFF }, // #FFFFFF
            { 0x64, 0xFF, 0xFF }, // #64FFFF
            { 0xFF, 0xFF, 0xFF }, // #FFFFFF
        },
    },
    {
        0x1E,
        0xFF,
        {
            { 0xFF, 0, 0xFF },    // #FF00FF
            { 0xFF, 0xC8, 0xFF }, // #FFC8FF
            { 0, 0x46, 0xFF },    // #0046FF
            { 0x64, 0xFF, 0xFF }, // #64FFFF
        },
    },
    {
        0x153,
        0xFF,
        {
            { 0xFF, 0, 0xFF },    // #FF00FF
            { 0xFF, 0xC8, 0xFF }, // #FFC8FF
            { 0, 0x46, 0xFF },    // #0046FF
            { 0x64, 0xFF, 0xFF }, // #64FFFF
        },
    },
    {
        0x167,
        0,
        {
            { 0xFF, 0, 0xE6 },    // #FF00E6
            { 0xFF, 0xC8, 0xFF }, // #FFC8FF
            { 0, 0x46, 0xFF },    // #0046FF
            { 0x64, 0xFF, 0xFF }, // #64FFFF
        },
    },
};

#ifdef VERSION_JP
// The 175 JP staff-roll lines (strings in credits_15A2A0.c). The second round shows the
// Japanese column.
static CreditsRoleLine sRoleLines[] = {
    { 0, { gCreditsLine000En, gCreditsLine000Jp } }, { 1, { gCreditsLine001En, gCreditsLine001Jp } },
    { 0, { gCreditsLine002En, gCreditsLine002Jp } }, { 1, { gCreditsLine003En, gCreditsLine003Jp } },
    { 0, { gCreditsLine004En, gCreditsLine004Jp } }, { 1, { gCreditsLine005En, gCreditsLine005Jp } },
    { 0, { gCreditsLine006En, gCreditsLine006Jp } }, { 1, { gCreditsLine007En, gCreditsLine007Jp } },
    { 0, { gCreditsLine008En, gCreditsLine008Jp } }, { 1, { gCreditsLine009En, gCreditsLine009Jp } },
    { 0, { gCreditsLine010En, gCreditsLine010Jp } }, { 1, { gCreditsLine011En, gCreditsLine011Jp } },
    { 0, { gCreditsLine012En, gCreditsLine012Jp } }, { 1, { gCreditsLine013En, gCreditsLine013Jp } },
    { 0, { gCreditsLine014En, gCreditsLine014Jp } }, { 1, { gCreditsLine015En, gCreditsLine015Jp } },
    { 0, { gCreditsLine016En, gCreditsLine016Jp } }, { 1, { gCreditsLine017En, gCreditsLine017Jp } },
    { 1, { gCreditsLine018En, gCreditsLine018Jp } }, { 1, { gCreditsLine019En, gCreditsLine019Jp } },
    { 0, { gCreditsLine020En, gCreditsLine020Jp } }, { 1, { gCreditsLine021En, gCreditsLine021Jp } },
    { 0, { gCreditsLine022En, gCreditsLine022Jp } }, { 1, { gCreditsLine023En, gCreditsLine023Jp } },
    { 0, { gCreditsLine024En, gCreditsLine024Jp } }, { 1, { gCreditsLine025En, gCreditsLine025Jp } },
    { 0, { gCreditsLine026En, gCreditsLine026Jp } }, { 1, { gCreditsLine027En, gCreditsLine027Jp } },
    { 1, { gCreditsLine028En, gCreditsLine028Jp } }, { 1, { gCreditsLine029En, gCreditsLine029Jp } },
    { 1, { gCreditsLine030En, gCreditsLine030Jp } }, { 1, { gCreditsLine031En, gCreditsLine031Jp } },
    { 0, { gCreditsLine032En, gCreditsLine032Jp } }, { 1, { gCreditsLine033En, gCreditsLine033Jp } },
    { 1, { gCreditsLine034En, gCreditsLine034Jp } }, { 1, { gCreditsLine035En, gCreditsLine035Jp } },
    { 1, { gCreditsLine036En, gCreditsLine036Jp } }, { 0, { gCreditsLine037En, gCreditsLine037Jp } },
    { 1, { gCreditsLine038En, gCreditsLine038Jp } }, { 1, { gCreditsLine039En, gCreditsLine039Jp } },
    { 0, { gCreditsLine040En, gCreditsLine040Jp } }, { 1, { gCreditsLine041En, gCreditsLine041Jp } },
    { 1, { gCreditsLine042En, gCreditsLine042Jp } }, { 1, { gCreditsLine043En, gCreditsLine043Jp } },
    { 1, { gCreditsLine044En, gCreditsLine044Jp } }, { 1, { gCreditsLine045En, gCreditsLine045Jp } },
    { 1, { gCreditsLine046En, gCreditsLine046Jp } }, { 1, { gCreditsLine047En, gCreditsLine047Jp } },
    { 1, { gCreditsLine048En, gCreditsLine048Jp } }, { 0, { gCreditsLine049En, gCreditsLine049Jp } },
    { 1, { gCreditsLine050En, gCreditsLine050Jp } }, { 1, { gCreditsLine051En, gCreditsLine051Jp } },
    { 1, { gCreditsLine052En, gCreditsLine052Jp } }, { 1, { gCreditsLine053En, gCreditsLine053Jp } },
    { 1, { gCreditsLine054En, gCreditsLine054Jp } }, { 1, { gCreditsLine055En, gCreditsLine055Jp } },
    { 1, { gCreditsLine056En, gCreditsLine056Jp } }, { 1, { gCreditsLine057En, gCreditsLine057Jp } },
    { 0, { gCreditsLine058En, gCreditsLine058Jp } }, { 1, { gCreditsLine059En, gCreditsLine059Jp } },
    { 1, { gCreditsLine060En, gCreditsLine060Jp } }, { 1, { gCreditsLine061En, gCreditsLine061Jp } },
    { 1, { gCreditsLine062En, gCreditsLine062Jp } }, { 1, { gCreditsLine063En, gCreditsLine063Jp } },
    { 1, { gCreditsLine064En, gCreditsLine064Jp } }, { 1, { gCreditsLine065En, gCreditsLine065Jp } },
    { 1, { gCreditsLine066En, gCreditsLine066Jp } }, { 0, { gCreditsLine067En, gCreditsLine067Jp } },
    { 1, { gCreditsLine068En, gCreditsLine068Jp } }, { 1, { gCreditsLine069En, gCreditsLine069Jp } },
    { 1, { gCreditsLine070En, gCreditsLine070Jp } }, { 1, { gCreditsLine071En, gCreditsLine071Jp } },
    { 1, { gCreditsLine072En, gCreditsLine072Jp } }, { 1, { gCreditsLine073En, gCreditsLine073Jp } },
    { 1, { gCreditsLine074En, gCreditsLine074Jp } }, { 1, { gCreditsLine075En, gCreditsLine075Jp } },
    { 0, { gCreditsLine076En, gCreditsLine076Jp } }, { 1, { gCreditsLine077En, gCreditsLine077Jp } },
    { 1, { gCreditsLine078En, gCreditsLine078Jp } }, { 1, { gCreditsLine079En, gCreditsLine079Jp } },
    { 1, { gCreditsLine080En, gCreditsLine080Jp } }, { 1, { gCreditsLine081En, gCreditsLine081Jp } },
    { 1, { gCreditsLine082En, gCreditsLine082Jp } }, { 1, { gCreditsLine083En, gCreditsLine083Jp } },
    { 1, { gCreditsLine084En, gCreditsLine084Jp } }, { 0, { gCreditsLine085En, gCreditsLine085Jp } },
    { 1, { gCreditsLine086En, gCreditsLine086Jp } }, { 1, { gCreditsLine087En, gCreditsLine087Jp } },
    { 1, { gCreditsLine088En, gCreditsLine088Jp } }, { 0, { gCreditsLine089En, gCreditsLine089Jp } },
    { 1, { gCreditsLine090En, gCreditsLine090Jp } }, { 0, { gCreditsLine091En, gCreditsLine091Jp } },
    { 1, { gCreditsLine092En, gCreditsLine092Jp } }, { 1, { gCreditsLine093En, gCreditsLine093Jp } },
    { 0, { gCreditsLine094En, gCreditsLine094Jp } }, { 1, { gCreditsLine095En, gCreditsLine095Jp } },
    { 0, { gCreditsLine096En, gCreditsLine096Jp } }, { 1, { gCreditsLine097En, gCreditsLine097Jp } },
    { 1, { gCreditsLine098En, gCreditsLine098Jp } }, { 0, { gCreditsLine099En, gCreditsLine099Jp } },
    { 1, { gCreditsLine100En, gCreditsLine100Jp } }, { 0, { gCreditsLine101En, gCreditsLine101Jp } },
    { 1, { gCreditsLine102En, gCreditsLine102Jp } }, { 1, { gCreditsLine103En, gCreditsLine103Jp } },
    { 0, { gCreditsLine104En, gCreditsLine104Jp } }, { 1, { gCreditsLine105En, gCreditsLine105Jp } },
    { 1, { gCreditsLine106En, gCreditsLine106Jp } }, { 1, { gCreditsLine107En, gCreditsLine107Jp } },
    { 0, { gCreditsLine108En, gCreditsLine108Jp } }, { 1, { gCreditsLine109En, gCreditsLine109Jp } },
    { 0, { gCreditsLine110En, gCreditsLine110Jp } }, { 1, { gCreditsLine111En, gCreditsLine111Jp } },
    { 0, { gCreditsLine112En, gCreditsLine112Jp } }, { 1, { gCreditsLine113En, gCreditsLine113Jp } },
    { 1, { gCreditsLine114En, gCreditsLine114Jp } }, { 1, { gCreditsLine115En, gCreditsLine115Jp } },
    { 1, { gCreditsLine116En, gCreditsLine116Jp } }, { 1, { gCreditsLine117En, gCreditsLine117Jp } },
    { 0, { gCreditsLine118En, gCreditsLine118Jp } }, { 1, { gCreditsLine119En, gCreditsLine119Jp } },
    { 1, { gCreditsLine120En, gCreditsLine120Jp } }, { 1, { gCreditsLine121En, gCreditsLine121Jp } },
    { 1, { gCreditsLine122En, gCreditsLine122Jp } }, { 2, { gCreditsLine123En, gCreditsLine123Jp } },
    { 1, { gCreditsLine124En, gCreditsLine124Jp } }, { 1, { gCreditsLine125En, gCreditsLine125Jp } },
    { 1, { gCreditsLine126En, gCreditsLine126Jp } }, { 0, { gCreditsLine127En, gCreditsLine127Jp } },
    { 1, { gCreditsLine128En, gCreditsLine128Jp } }, { 1, { gCreditsLine129En, gCreditsLine129Jp } },
    { 1, { gCreditsLine130En, gCreditsLine130Jp } }, { 1, { gCreditsLine131En, gCreditsLine131Jp } },
    { 1, { gCreditsLine132En, gCreditsLine132Jp } }, { 1, { gCreditsLine133En, gCreditsLine133Jp } },
    { 0, { gCreditsLine134En, gCreditsLine134Jp } }, { 1, { gCreditsLine135En, gCreditsLine135Jp } },
    { 0, { gCreditsLine136En, gCreditsLine136Jp } }, { 1, { gCreditsLine137En, gCreditsLine137Jp } },
    { 1, { gCreditsLine138En, gCreditsLine138Jp } }, { 1, { gCreditsLine139En, gCreditsLine139Jp } },
    { 1, { gCreditsLine140En, gCreditsLine140Jp } }, { 0, { gCreditsLine141En, gCreditsLine141Jp } },
    { 1, { gCreditsLine142En, gCreditsLine142Jp } }, { 1, { gCreditsLine143En, gCreditsLine143Jp } },
    { 1, { gCreditsLine144En, gCreditsLine144Jp } }, { 1, { gCreditsLine145En, gCreditsLine145Jp } },
    { 0, { gCreditsLine146En, gCreditsLine146Jp } }, { 1, { gCreditsLine147En, gCreditsLine147Jp } },
    { 0, { gCreditsLine148En, gCreditsLine148Jp } }, { 1, { gCreditsLine149En, gCreditsLine149Jp } },
    { 1, { gCreditsLine150En, gCreditsLine150Jp } }, { 1, { gCreditsLine151En, gCreditsLine151Jp } },
    { 2, { gCreditsLine152En, gCreditsLine152Jp } }, { 1, { gCreditsLine153En, gCreditsLine153Jp } },
    { 1, { gCreditsLine154En, gCreditsLine154Jp } }, { 0, { gCreditsLine155En, gCreditsLine155Jp } },
    { 1, { gCreditsLine156En, gCreditsLine156Jp } }, { 1, { gCreditsLine157En, gCreditsLine157Jp } },
    { 1, { gCreditsLine158En, gCreditsLine158Jp } }, { 1, { gCreditsLine159En, gCreditsLine159Jp } },
    { 1, { gCreditsLine160En, gCreditsLine160Jp } }, { 1, { gCreditsLine161En, gCreditsLine161Jp } },
    { 1, { gCreditsLine162En, gCreditsLine162Jp } }, { 0, { gCreditsLine163En, gCreditsLine163Jp } },
    { 1, { gCreditsLine164En, gCreditsLine164Jp } }, { 0, { gCreditsLine165En, gCreditsLine165Jp } },
    { 1, { gCreditsLine166En, gCreditsLine166Jp } }, { 1, { gCreditsLine167En, gCreditsLine167Jp } },
    { 0, { gCreditsLine168En, gCreditsLine168Jp } }, { 1, { gCreditsLine169En, gCreditsLine169Jp } },
    { 1, { gCreditsLine170En, gCreditsLine170Jp } }, { 1, { gCreditsLine171En, gCreditsLine171Jp } },
    { 1, { gCreditsLine172En, gCreditsLine172Jp } }, { 0, { gCreditsLine173En, gCreditsLine173Jp } },
    { 1, { gCreditsLine174En, gCreditsLine174Jp } },
};

CreditsRolePage sRolePages[28] = {
    { 0, 360, 40, 6, sRoleLines }, { 0, 360, 40, 6, NULL },  { 0, 368, 48, 8, NULL }, { 0, 352, 40, 6, NULL },
    { 0, 360, 40, 6, NULL },       { 0, 368, 48, 5, NULL },  { 0, 352, 40, 3, NULL }, { 0, 360, 40, 9, NULL },
    { 0, 368, 48, 9, NULL },       { 0, 352, 40, 9, NULL },  { 0, 360, 40, 9, NULL }, { 0, 368, 48, 9, NULL },
    { 0, 232, 40, 4, NULL },       { 0, 240, 40, 5, NULL },  { 0, 240, 40, 5, NULL }, { 0, 368, 48, 9, NULL },
    { 0, 352, 40, 4, NULL },       { 0, 360, 40, 6, NULL },  { 0, 368, 48, 9, NULL }, { 0, 352, 40, 7, NULL },
    { 0, 240, 40, 2, NULL },       { 0, 360, 40, 10, NULL }, { 0, 368, 48, 9, NULL }, { 0, 352, 40, 8, NULL },
    { 0, 360, 40, 5, NULL },       { 0, 360, 40, 5, NULL },  { 0, 240, 40, 2, NULL }, { 0, 0, 40, 0, NULL },
};
#else
// The 187 staff-roll lines in display order, one per string of textdata table 45 (US 1.0
// assets/us/textdata/45.bin). Entry 0 is "DIRECTOR" / "Takao Shimizu" ...; the last is
// "EXECUTIVE PRODUCER" / "Hiroshi Yamauchi". Style 2 entries are blank spacers whose table
// strings are empty (lines 126 and 155).
static CreditsRoleLine sRoleLines[] = {
    {
        0,
        { &D_86A05B60, &D_86A05B64 },
    },
    {
        1,
        { &D_86A05B68, &D_86A05B6C },
    },
    {
        0,
        { &D_86A05B70, &D_86A05B74 },
    },
    {
        1,
        { &D_86A05B78, &D_86A05B7C },
    },
    {
        0,
        { &D_86A05B80, &D_86A05B84 },
    },
    {
        1,
        { &D_86A05B88, &D_86A05B8C },
    },
    {
        0,
        { &D_86A05B90, &D_86A05B94 },
    },
    {
        1,
        { &D_86A05B98, &D_86A05B9C },
    },
    {
        0,
        { &D_86A05BA0, &D_86A05BA4 },
    },
    {
        1,
        { &D_86A05BA8, &D_86A05BAC },
    },
    {
        0,
        { &D_86A05BB0, &D_86A05BB4 },
    },
    {
        1,
        { &D_86A05BB8, &D_86A05BBC },
    },
    {
        0,
        { &D_86A05BC0, &D_86A05BC4 },
    },
    {
        1,
        { &D_86A05BC8, &D_86A05BCC },
    },
    {
        0,
        { &D_86A05BD0, &D_86A05BD4 },
    },
    {
        1,
        { &D_86A05BD8, &D_86A05BDC },
    },
    {
        0,
        { &D_86A05BE0, &D_86A05BE4 },
    },
    {
        1,
        { &D_86A05BE8, &D_86A05BEC },
    },
    {
        1,
        { &D_86A05BF0, &D_86A05BF4 },
    },
    {
        1,
        { &D_86A05BF8, &D_86A05BFC },
    },
    {
        0,
        { &D_86A05C00, &D_86A05C04 },
    },
    {
        1,
        { &D_86A05C08, &D_86A05C0C },
    },
    {
        0,
        { &D_86A05C10, &D_86A05C14 },
    },
    {
        1,
        { &D_86A05C18, &D_86A05C1C },
    },
    {
        0,
        { &D_86A05C20, &D_86A05C24 },
    },
    {
        1,
        { &D_86A05C28, &D_86A05C2C },
    },
    {
        0,
        { &D_86A05C30, &D_86A05C34 },
    },
    {
        1,
        { &D_86A05C38, &D_86A05C3C },
    },
    {
        1,
        { &D_86A05C40, &D_86A05C44 },
    },
    {
        1,
        { &D_86A05C48, &D_86A05C4C },
    },
    {
        1,
        { &D_86A05C50, &D_86A05C54 },
    },
    {
        1,
        { &D_86A05C58, &D_86A05C5C },
    },
    {
        0,
        { &D_86A05C60, &D_86A05C64 },
    },
    {
        1,
        { &D_86A05C68, &D_86A05C6C },
    },
    {
        1,
        { &D_86A05C70, &D_86A05C74 },
    },
    {
        1,
        { &D_86A05C78, &D_86A05C7C },
    },
    {
        1,
        { &D_86A05C80, &D_86A05C84 },
    },
    {
        0,
        { &D_86A05C88, &D_86A05C8C },
    },
    {
        1,
        { &D_86A05C90, &D_86A05C94 },
    },
    {
        1,
        { &D_86A05C98, &D_86A05C9C },
    },
    {
        0,
        { &D_86A05CA0, &D_86A05CA4 },
    },
    {
        1,
        { &D_86A05CA8, &D_86A05CAC },
    },
    {
        1,
        { &D_86A05CB0, &D_86A05CB4 },
    },
    {
        1,
        { &D_86A05CB8, &D_86A05CBC },
    },
    {
        1,
        { &D_86A05CC0, &D_86A05CC4 },
    },
    {
        1,
        { &D_86A05CC8, &D_86A05CCC },
    },
    {
        1,
        { &D_86A05CD0, &D_86A05CD4 },
    },
    {
        1,
        { &D_86A05CD8, &D_86A05CDC },
    },
    {
        1,
        { &D_86A05CE0, &D_86A05CE4 },
    },
    {
        0,
        { &D_86A05CE8, &D_86A05CEC },
    },
    {
        1,
        { &D_86A05CF0, &D_86A05CF4 },
    },
    {
        1,
        { &D_86A05CF8, &D_86A05CFC },
    },
    {
        1,
        { &D_86A05D00, &D_86A05D04 },
    },
    {
        1,
        { &D_86A05D08, &D_86A05D0C },
    },
    {
        1,
        { &D_86A05D10, &D_86A05D14 },
    },
    {
        1,
        { &D_86A05D18, &D_86A05D1C },
    },
    {
        1,
        { &D_86A05D20, &D_86A05D24 },
    },
    {
        1,
        { &D_86A05D28, &D_86A05D2C },
    },
    {
        0,
        { &D_86A05D30, &D_86A05D34 },
    },
    {
        1,
        { &D_86A05D38, &D_86A05D3C },
    },
    {
        1,
        { &D_86A05D40, &D_86A05D44 },
    },
    {
        1,
        { &D_86A05D48, &D_86A05D4C },
    },
    {
        1,
        { &D_86A05D50, &D_86A05D54 },
    },
    {
        1,
        { &D_86A05D58, &D_86A05D5C },
    },
    {
        1,
        { &D_86A05D60, &D_86A05D64 },
    },
    {
        1,
        { &D_86A05D68, &D_86A05D6C },
    },
    {
        1,
        { &D_86A05D70, &D_86A05D74 },
    },
    {
        0,
        { &D_86A05D78, &D_86A05D7C },
    },
    {
        1,
        { &D_86A05D80, &D_86A05D84 },
    },
    {
        1,
        { &D_86A05D88, &D_86A05D8C },
    },
    {
        1,
        { &D_86A05D90, &D_86A05D94 },
    },
    {
        1,
        { &D_86A05D98, &D_86A05D9C },
    },
    {
        1,
        { &D_86A05DA0, &D_86A05DA4 },
    },
    {
        1,
        { &D_86A05DA8, &D_86A05DAC },
    },
    {
        1,
        { &D_86A05DB0, &D_86A05DB4 },
    },
    {
        1,
        { &D_86A05DB8, &D_86A05DBC },
    },
    {
        0,
        { &D_86A05DC0, &D_86A05DC4 },
    },
    {
        1,
        { &D_86A05DC8, &D_86A05DCC },
    },
    {
        1,
        { &D_86A05DD0, &D_86A05DD4 },
    },
    {
        1,
        { &D_86A05DD8, &D_86A05DDC },
    },
    {
        1,
        { &D_86A05DE0, &D_86A05DE4 },
    },
    {
        1,
        { &D_86A05DE8, &D_86A05DEC },
    },
    {
        1,
        { &D_86A05DF0, &D_86A05DF4 },
    },
    {
        1,
        { &D_86A05DF8, &D_86A05DFC },
    },
    {
        1,
        { &D_86A05E00, &D_86A05E04 },
    },
    {
        0,
        { &D_86A05E08, &D_86A05E0C },
    },
    {
        1,
        { &D_86A05E10, &D_86A05E14 },
    },
    {
        1,
        { &D_86A05E18, &D_86A05E1C },
    },
    {
        1,
        { &D_86A05E20, &D_86A05E24 },
    },
    {
        0,
        { &D_86A05E28, &D_86A05E2C },
    },
    {
        1,
        { &D_86A05E30, &D_86A05E34 },
    },
    {
        0,
        { &D_86A05E38, &D_86A05E3C },
    },
    {
        1,
        { &D_86A05E40, &D_86A05E44 },
    },
    {
        1,
        { &D_86A05E48, &D_86A05E4C },
    },
    {
        0,
        { &D_86A05E50, &D_86A05E54 },
    },
    {
        1,
        { &D_86A05E58, &D_86A05E5C },
    },
    {
        0,
        { &D_86A05E60, &D_86A05E64 },
    },
    {
        1,
        { &D_86A05E68, &D_86A05E6C },
    },
    {
        1,
        { &D_86A05E70, &D_86A05E74 },
    },
    {
        0,
        { &D_86A05E78, &D_86A05E7C },
    },
    {
        1,
        { &D_86A05E80, &D_86A05E84 },
    },
    {
        0,
        { &D_86A05E88, &D_86A05E8C },
    },
    {
        1,
        { &D_86A05E90, &D_86A05E94 },
    },
    {
        1,
        { &D_86A05E98, &D_86A05E9C },
    },
    {
        0,
        { &D_86A05EA0, &D_86A05EA4 },
    },
    {
        1,
        { &D_86A05EA8, &D_86A05EAC },
    },
    {
        1,
        { &D_86A05EB0, &D_86A05EB4 },
    },
    {
        1,
        { &D_86A05EB8, &D_86A05EBC },
    },
    {
        0,
        { &D_86A05EC0, &D_86A05EC4 },
    },
    {
        1,
        { &D_86A05EC8, &D_86A05ECC },
    },
    {
        0,
        { &D_86A05ED0, &D_86A05ED4 },
    },
    {
        1,
        { &D_86A05ED8, &D_86A05EDC },
    },
    {
        0,
        { &D_86A05EE0, &D_86A05EE4 },
    },
    {
        1,
        { &D_86A05EE8, &D_86A05EEC },
    },
    {
        1,
        { &D_86A05EF0, &D_86A05EF4 },
    },
    {
        0,
        { &D_86A05EF8, &D_86A05EFC },
    },
    {
        1,
        { &D_86A05F00, &D_86A05F04 },
    },
    {
        1,
        { &D_86A05F08, &D_86A05F0C },
    },
    {
        1,
        { &D_86A05F10, &D_86A05F14 },
    },
    {
        1,
        { &D_86A05F18, &D_86A05F1C },
    },
    {
        1,
        { &D_86A05F20, &D_86A05F24 },
    },
    {
        0,
        { &D_86A05F28, &D_86A05F2C },
    },
    {
        1,
        { &D_86A05F30, &D_86A05F34 },
    },
    {
        1,
        { &D_86A05F38, &D_86A05F3C },
    },
    {
        1,
        { &D_86A05F40, &D_86A05F44 },
    },
    {
        1,
        { &D_86A05F48, &D_86A05F4C },
    },
    {
        2,
        { &D_86A05F50, &D_86A05F54 },
    },
    {
        1,
        { &D_86A05F58, &D_86A05F5C },
    },
    {
        1,
        { &D_86A05F60, &D_86A05F64 },
    },
    {
        1,
        { &D_86A05F68, &D_86A05F6C },
    },
    {
        0,
        { &D_86A05F70, &D_86A05F74 },
    },
    {
        1,
        { &D_86A05F78, &D_86A05F7C },
    },
    {
        1,
        { &D_86A05F80, &D_86A05F84 },
    },
    {
        1,
        { &D_86A05F88, &D_86A05F8C },
    },
    {
        1,
        { &D_86A05F90, &D_86A05F94 },
    },
    {
        1,
        { &D_86A05F98, &D_86A05F9C },
    },
    {
        1,
        { &D_86A05FA0, &D_86A05FA4 },
    },
    {
        0,
        { &D_86A05FA8, &D_86A05FAC },
    },
    {
        1,
        { &D_86A05FB0, &D_86A05FB4 },
    },
    {
        0,
        { &D_86A05FB8, &D_86A05FBC },
    },
    {
        1,
        { &D_86A05FC0, &D_86A05FC4 },
    },
    {
        0,
        { &D_86A05FC8, &D_86A05FCC },
    },
    {
        1,
        { &D_86A05FD0, &D_86A05FD4 },
    },
    {
        1,
        { &D_86A05FD8, &D_86A05FDC },
    },
    {
        1,
        { &D_86A05FE0, &D_86A05FE4 },
    },
    {
        1,
        { &D_86A05FE8, &D_86A05FEC },
    },
    {
        0,
        { &D_86A05FF0, &D_86A05FF4 },
    },
    {
        1,
        { &D_86A05FF8, &D_86A05FFC },
    },
    {
        1,
        { &D_86A06000, &D_86A06004 },
    },
    {
        1,
        { &D_86A06008, &D_86A0600C },
    },
    {
        1,
        { &D_86A06010, &D_86A06014 },
    },
    {
        0,
        { &D_86A06018, &D_86A0601C },
    },
    {
        1,
        { &D_86A06020, &D_86A06024 },
    },
    {
        1,
        { &D_86A06028, &D_86A0602C },
    },
    {
        1,
        { &D_86A06030, &D_86A06034 },
    },
    {
        2,
        { &D_86A06038, &D_86A0603C },
    },
    {
        1,
        { &D_86A06040, &D_86A06044 },
    },
    {
        1,
        { &D_86A06048, &D_86A0604C },
    },
    {
        1,
        { &D_86A06050, &D_86A06054 },
    },
    {
        1,
        { &D_86A06058, &D_86A0605C },
    },
    {
        1,
        { &D_86A06060, &D_86A06064 },
    },
    {
        1,
        { &D_86A06068, &D_86A0606C },
    },
    {
        0,
        { &D_86A06070, &D_86A06074 },
    },
    {
        1,
        { &D_86A06078, &D_86A0607C },
    },
    {
        1,
        { &D_86A06080, &D_86A06084 },
    },
    {
        1,
        { &D_86A06088, &D_86A0608C },
    },
    {
        1,
        { &D_86A06090, &D_86A06094 },
    },
    {
        1,
        { &D_86A06098, &D_86A0609C },
    },
    {
        0,
        { &D_86A060A0, &D_86A060A4 },
    },
    {
        1,
        { &D_86A060A8, &D_86A060AC },
    },
    {
        1,
        { &D_86A060B0, &D_86A060B4 },
    },
    {
        0,
        { &D_86A060B8, &D_86A060BC },
    },
    {
        1,
        { &D_86A060C0, &D_86A060C4 },
    },
    {
        0,
        { &D_86A060C8, &D_86A060CC },
    },
    {
        1,
        { &D_86A060D0, &D_86A060D4 },
    },
    {
        1,
        { &D_86A060D8, &D_86A060DC },
    },
    {
        1,
        { &D_86A060E0, &D_86A060E4 },
    },
    {
        0,
        { &D_86A060E8, &D_86A060EC },
    },
    {
        1,
        { &D_86A060F0, &D_86A060F4 },
    },
    {
        1,
        { &D_86A060F8, &D_86A060FC },
    },
    {
        0,
        { &D_86A06100, &D_86A06104 },
    },
    {
        1,
        { &D_86A06108, &D_86A0610C },
    },
    {
        1,
        { &D_86A06110, &D_86A06114 },
    },
    {
        1,
        { &D_86A06118, &D_86A0611C },
    },
    {
        1,
        { &D_86A06120, &D_86A06124 },
    },
    {
        0,
        { &D_86A06128, &D_86A0612C },
    },
    {
        1,
        { &D_86A06130, &D_86A06134 },
    },
};

// The staff roll in 27 pages followed by two terminators (duration 0). Page 0 points at
// sRoleLines; Credits_DrawRoleList chains each following page to the line after the previous
// page's last line. Durations sum to 9120 frames, the same total as sBackgroundSegments.
CreditsRolePage sRolePages[29] = {
    {
        0,
        360,
        40,
        6,
        sRoleLines,
    },
    {
        0,
        360,
        40,
        6,
        NULL,
    },
    {
        0,
        368,
        48,
        8,
        NULL,
    },
    {
        0,
        352,
        40,
        6,
        NULL,
    },
    {
        0,
        360,
        40,
        6,
        NULL,
    },
    {
        0,
        368,
        48,
        5,
        NULL,
    },
    {
        0,
        352,
        40,
        3,
        NULL,
    },
    {
        0,
        360,
        40,
        9,
        NULL,
    },
    {
        0,
        368,
        48,
        9,
        NULL,
    },
    {
        0,
        352,
        40,
        9,
        NULL,
    },
    {
        0,
        360,
        40,
        9,
        NULL,
    },
    {
        0,
        368,
        48,
        9,
        NULL,
    },
    {
        0,
        232,
        40,
        4,
        NULL,
    },
    {
        0,
        240,
        40,
        5,
        NULL,
    },
    {
        0,
        240,
        40,
        5,
        NULL,
    },
    {
        0,
        368,
        48,
        9,
        NULL,
    },
    {
        0,
        352,
        40,
        7,
        NULL,
    },
    {
        0,
        360,
        40,
        6,
        NULL,
    },
    {
        0,
        368,
        48,
        9,
        NULL,
    },
    {
        0,
        352,
        40,
        9,
        NULL,
    },
    {
        0,
        240,
        40,
        2,
        NULL,
    },
    {
        0,
        360,
        40,
        10,
        NULL,
    },
    {
        0,
        368,
        48,
        11,
        NULL,
    },
    {
        0,
        352,
        40,
        9,
        NULL,
    },
    {
        0,
        360,
        40,
        9,
        NULL,
    },
    {
        0,
        360,
        40,
        5,
        NULL,
    },
    {
        0,
        240,
        40,
        2,
        NULL,
    },
    {
        0,
        0,
        40,
        0,
        NULL,
    },
    {
        0,
        0,
        0,
        0,
        NULL,
    },
};
#endif

/**
 * Computes the four staff-roll colors for page time `timer` by linear interpolation between
 * sRoleColorKeyframes. Keyframes 3 and 4 are moved so the fade out ends 1 frame before the
 * page's trailing blank period. Returns 0 (draw nothing) once timer reaches
 * duration - blank_frames, otherwise 1.
 */
s32 Credits_InterpolateRoleColor(Color_RGBA8_u32* colors, u16 timer, u16 duration, u8 blank_frames) {
    u8 i;
    u8 j;
    u8 alpha;

    if (timer >= (duration - blank_frames)) {
        return 0;
    }

    sRoleColorKeyframes[3].frame = (duration - blank_frames) - 0x15;
    sRoleColorKeyframes[4].frame = (duration - blank_frames) - 1;

    for (i = 0; i < 5; i++) {
        if (timer == sRoleColorKeyframes[i].frame) {
            for (j = 0; j < 4; j++) {
                Color_SetRGBA(&colors[j], sRoleColorKeyframes[i].colors[j].r, sRoleColorKeyframes[i].colors[j].g,
                              sRoleColorKeyframes[i].colors[j].b, sRoleColorKeyframes[i].alpha);
            }
            break;
        }

        if ((sRoleColorKeyframes[i].frame < timer) && (timer < sRoleColorKeyframes[i + 1].frame)) {
            alpha = (s32)ROUND_MAX((((sRoleColorKeyframes[i + 1].alpha - sRoleColorKeyframes[i].alpha) *
                                     (timer - sRoleColorKeyframes[i].frame)) /
                                    (f32)(sRoleColorKeyframes[i + 1].frame - sRoleColorKeyframes[i].frame)) +
                                   sRoleColorKeyframes[i].alpha);

            for (j = 0; j < 4; j++) {
                colors[j].r =
                    (s32)ROUND_MAX((((sRoleColorKeyframes[i + 1].colors[j].r - sRoleColorKeyframes[i].colors[j].r) *
                                     (timer - sRoleColorKeyframes[i].frame)) /
                                    (f32)(sRoleColorKeyframes[i + 1].frame - sRoleColorKeyframes[i].frame)) +
                                   sRoleColorKeyframes[i].colors[j].r);
                colors[j].g =
                    (s32)ROUND_MAX((((sRoleColorKeyframes[i + 1].colors[j].g - sRoleColorKeyframes[i].colors[j].g) *
                                     (timer - sRoleColorKeyframes[i].frame)) /
                                    (f32)(sRoleColorKeyframes[i + 1].frame - sRoleColorKeyframes[i].frame)) +
                                   sRoleColorKeyframes[i].colors[j].g);
                colors[j].b =
                    (s32)ROUND_MAX((((sRoleColorKeyframes[i + 1].colors[j].b - sRoleColorKeyframes[i].colors[j].b) *
                                     (timer - sRoleColorKeyframes[i].frame)) /
                                    (f32)(sRoleColorKeyframes[i + 1].frame - sRoleColorKeyframes[i].frame)) +
                                   sRoleColorKeyframes[i].colors[j].b);
                colors[j].a = alpha;
            }

            break;
        }
    }
    return 1;
}

/**
 * Prepares the staff roll: loads every string of textdata table 45 (the US staff credits)
 * into sRoleLines[i].text[0], sets the starting frame delta, clears the text column and
 * frame-delta history, and makes `pages` the current page. `is_round_1` is unused.
 */
#ifdef VERSION_JP
// JP keeps its staff names in sRoleLines' data and picks text[0] or text[1] by round.
void Credits_InitRoleList(u8 initial_frame_delta, u8 is_round_1, CreditsRolePage* pages) {
    sFrameDelta = initial_frame_delta;
    sRoleTextColumn = is_round_1;
    sFrameDeltaPrimed = 0;
    sCurrentRolePage = pages;
}
#else
void Credits_InitRoleList(u8 initial_frame_delta, u8 is_round_1, CreditsRolePage* pages) {
    static char** sCreditsText;

    u16 i;
    u16 line_count;
    CreditsRoleLine* line;

    line_count = Credits_GetRoleLineCount();
    Text_InitStringTables();
    sCreditsText = Text_GetStringTable(0x2D);

    for (i = 0, line = sRoleLines; i < line_count; i++, line++) {
        line->text[0] = Text_GetString(NULL, 0, sCreditsText, i);
    }

    sFrameDelta = initial_frame_delta;
    sRoleTextColumn = 0;
    sFrameDeltaPrimed = 0;
    sCurrentRolePage = pages;
}
#endif

// D_800A62E0.unk_A34 at the previous Credits_UpdateFrameDelta. Defined after Credits_InitRoleList
// so that its block-scope sCreditsText comes first in .bss.
u32 sLastRetraceCount;

/**
 * Sets sFrameDelta to the number of retraces (D_800A62E0.unk_A34, incremented by
 * Sched_HandleRetrace) since the previous call, so the staff roll and camera keep real-time
 * speed when frames are dropped. The first call only records the count.
 * //@src/scheduler.c Sched_HandleRetrace
 *
 * FAKE: the wrap-around arm widens the counter to 64 bits before subtracting, which is what makes
 * IDO stage the value through the low half of a register pair (addiu t5, v0, 0) and only then
 * do the 32-bit subtract (subu t9, t5, v1). The two chained 0x100000000 subtractions cancel
 * out of the low word (the result is truncated to 8 bits anyway), but they are what puts the
 * pair at t4:t5 instead of t2:t3.
 */
void Credits_UpdateFrameDelta(void) {
    s32 retrace_count = D_800A62E0.unk_A34;

    if (sFrameDeltaPrimed == 0) {
        sFrameDeltaPrimed = 1;
    } else if ((u32)sLastRetraceCount < (u32)retrace_count) {
        sFrameDelta = retrace_count - sLastRetraceCount;
    } else {
        sFrameDelta = ((u64)(u32)retrace_count - 0x100000000ULL - 0x100000000ULL) - sLastRetraceCount;
    }
    sLastRetraceCount = retrace_count;
}

/**
 * Draws and advances the current staff-roll page. Lines are centered horizontally at x 160;
 * the block is centered vertically using 17 pixels per printed line, 12 extra pixels before a
 * heading that follows a name, 4 before a name that follows a heading and 10 per spacer.
 * Headings use colors 0/1 and names colors 2/3 from Credits_InterpolateRoleColor. When the
 * page timer reaches its duration the next page is chained to the following lines and its
 * duration absorbs the overshoot; after the last page the Pokemon Stadium logo fades in.
 */
void Credits_DrawRoleList(void) {
    u8 heading_gap;
    u8 heading_count;
    u8 spacer_count;
    u8 i;
    s32 current_style;
    Color_RGBA8_u32 colors[4];
    u16 y;
    CreditsRoleLine* line;

    if (sCurrentRolePage == NULL) {
        Credits_DrawStadiumLogo(1);
        return;
    }

    heading_count = 0;
    spacer_count = 0;

    if (Credits_InterpolateRoleColor(colors, sCurrentRolePage->timer, sCurrentRolePage->duration,
                                     sCurrentRolePage->blankFrames) != 0) {
        Font_BeginTranslucentTextRendering();
        Font_EnableTwoCycleTexturing();
#ifdef VERSION_JP
        Font_SetActive(4, -2);

        // The first round draws the roll as projected quads (see Credits_DrawGlyph).
        if (sRoleTextColumn == 0) {
            gSPDisplayList(gDisplayListHead++, sRoleTextSetupDL);
            gSPMatrix(gDisplayListHead++, (u32)&sRoleTextProjection & 0x1FFFFFFF,
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
            gSPMatrix(gDisplayListHead++, (u32)&gIdentityMtx & 0x1FFFFFFF, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        }
#else
        Font_SetActive(4, 0);
#endif

        current_style = 0;

        Gfx_SetEnvColor(colors[0].r, colors[0].g, colors[0].b, colors[0].a);
        Gfx_SetPrimColor(colors[1].r, colors[1].g, colors[1].b, colors[1].a);

        line = sCurrentRolePage->lines;
        for (i = 0; i < sCurrentRolePage->lineCount; i++, line++) {
            switch (line->style) {
                case 0:
                    heading_count++;
                    break;

                case 2:
                    spacer_count++;
                    break;
            }
        }

        if (heading_count >= 2) {
            heading_gap = ((heading_count * 4) + (heading_count * 0xC) + (spacer_count * 0xA)) - 0xC;
        } else {
            heading_gap = (heading_count * 4) + (spacer_count * 0xA);
        }

        y = (((-((sCurrentRolePage->lineCount - spacer_count) * 0x11) - heading_gap) + 0xF0) / 2) & 0xFFFF;

        for (i = 0, line = sCurrentRolePage->lines; i < sCurrentRolePage->lineCount; i++, line++) {
            if (line->style == 2) {
                y += 0xA;
                continue;
            }

            if (current_style != line->style) {
                switch (line->style) {
                    case 0:
                        Gfx_SetEnvColor(colors[0].r, colors[0].g, colors[0].b, colors[0].a);
                        Gfx_SetPrimColor(colors[1].r, colors[1].g, colors[1].b, colors[1].a);
                        current_style = 0;
                        y += 0xC;
                        break;

                    case 1:
                        Gfx_SetEnvColor(colors[2].r, colors[2].g, colors[2].b, colors[2].a);
                        Gfx_SetPrimColor(colors[3].r, colors[3].g, colors[3].b, colors[3].a);
                        current_style = 1;
                        y += 4;
                        break;
                }
            }

#ifdef VERSION_JP
            Credits_Printf(0x280 - (Credits_MeasureText(sFontContext, (u8*)line->text[sRoleTextColumn]) / 2), y * 4,
                           line->text[sRoleTextColumn]);
#else
            Font_Printf(0xA0 - (Font_MeasureTextExtent(0, 0, line->text[sRoleTextColumn]) / 2), (y * 4) >> 2,
                        line->text[sRoleTextColumn]);
#endif
            y += 0x11;
        }

        Font_DisableTwoCycleTexturing();
        Font_EndTexturedTextRendering();
    }

    sCurrentRolePage->timer = sCurrentRolePage->timer + sFrameDelta;
    if (sCurrentRolePage->timer >= sCurrentRolePage->duration) {
        CreditsRolePage finished_page;

        finished_page = *sCurrentRolePage;

        sCurrentRolePage++;

        sCurrentRolePage->lines = &finished_page.lines[finished_page.lineCount];
        if (sCurrentRolePage->duration == 0) {
            sCurrentRolePage = NULL;
        } else {
            sCurrentRolePage->duration = (sCurrentRolePage->duration - finished_page.timer) + finished_page.duration;
            sCurrentRolePage->timer = 0;
        }
    }
}
