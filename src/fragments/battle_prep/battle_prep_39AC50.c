#include "battle_prep.h"
#include "src/geo_render.h"
#include "src/display_object_textures.h"
#include "src/model_renderer.h"
#include "src/audio_commands_category2.h"
#include "src/gfx_rect.h"
#include "src/matrix.h"
#include "src/memmap.h"
#include "src/memory.h"

static u32 D_84B11C20[] = {
    0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x39CF39CF, 0x39CF294B,
    0x18C70001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001,
    0x00010001, 0x00010001, 0x00010001, 0x10856B5B, 0xC631F7BD, 0xFFFFFFFF, 0xFFFFFFFF, 0xF7BDB5AD, 0x6319294B,
    0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001,
    0x1085A529, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFF7BD, 0xB5AD39CF, 0x00010001,
    0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x0001A527, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xA5212949, 0x00010001, 0x00010001,
    0x00010001, 0x00010001, 0x00010001, 0x00012947, 0xDEE9FFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFBDEE7, 0x42090001, 0x00010001, 0x00010001, 0x00010001,
    0x00010001, 0x1083DEDF, 0xFFF5FFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFD, 0xFFF5FFED, 0xDEDD2945, 0x00010001, 0x00010001, 0x00010001, 0x00011083, 0xDED5FFE1,
    0xFFEDFFFD, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFF3, 0xFFEFFFF3,
    0xFFF1FFE9, 0xFFDFDED7, 0x18C30001, 0x00010001, 0x00010001, 0x00016309, 0xFF95FFDB, 0xFFE5FFF9, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFF5FFEB, 0xFFE7FFE7, 0xFFEBFFE7, 0xFFDFFFDB,
    0xA50D0001, 0x00010001, 0x00010001, 0x1083F60F, 0xFED1FFDB, 0xFFE5FFEF, 0xFFF5FFF3, 0xFFF9FFFD, 0xFFF9FFFF,
    0xFFFFFFFF, 0xFFFFFFF9, 0xFFF7FFFB, 0xFFEFFFE7, 0xFFDFFFE1, 0xFFE5FFE1, 0xFFDFFFDB, 0xF79339C5, 0x00010001,
    0x00010001, 0xA389FD4D, 0xFE4FFFDB, 0xFFE1FFE5, 0xFFE9FFE9, 0xFFF1FFF3, 0xFFEDFFF9, 0xFFFFFFFF, 0xFFFFFFEF,
    0xFFEDFFEF, 0xFFE7FFE7, 0xFFDBFFDB, 0xFFDDFFDD, 0xFFDDFFD7, 0xFFD593C9, 0x00010001, 0x00011083, 0xFE0DFD4B,
    0xFF53FFDB, 0xFFDDFFDF, 0xFFE1FFE3, 0xFFE9FFE7, 0xFFE7FFF3, 0xFFFDFFF9, 0xFFFDFFE9, 0xFFE5FFE5, 0xFFE5FFE1,
    0xFFDFFFD5, 0xFFD9FFD9, 0xFFD9FFD7, 0xFFD1FD8D, 0x28830001, 0x00017B07, 0xFE0DFE4F, 0xFF93FFD7, 0xFFD7FFDB,
    0xFFDBFFE1, 0xFFDFFFDD, 0xFFDFFFEB, 0xFFEFFFEF, 0xFFEFFFE1, 0xFFDBFFDD, 0xFFDFFFD9, 0xFFDDFFD5, 0xFFD3FFD7,
    0xFFD7FFD3, 0xFFD1FCCB, 0x59430001, 0x0001B449, 0xFD4DFD8B, 0xFE91FF93, 0xFFD5FFD5, 0xFFD7FFD7, 0xFF95FFD5,
    0xFFD9FFE1, 0xFFE3FFE1, 0xFFE3FFDB, 0xFFD3FFD5, 0xFFD5FFD3, 0xFFD5FFD1, 0xFFD1FFD5, 0xFFD5FFD1, 0xFECFFD0B,
    0xAA450001, 0x0001DD0B, 0xFD0BFD0B, 0xFDCFFE0F, 0xFF91FE0F, 0xFF0FFD8D, 0xFE0DFF4F, 0xFF53FFD9, 0xFFD9FFD7,
    0xFFD9FFD7, 0xFE8FFECF, 0xFF0DFF0F, 0xFE0DFE8D, 0xFECFFE8F, 0xFF91FF91, 0xFDCDFD09, 0xF3090001, 0x0001FD4D,
    0xFC89FC49, 0xFD0DFCCB, 0xFD8DFD4B, 0xFD4BFC07, 0xFDCDFECF, 0xFE8FFFD3, 0xFECFFED1, 0xFFD3FF53, 0xFD4BFD49,
    0xFE0DFE8D, 0xFD8BFD0B, 0xFD49FD4B, 0xFECFFECF, 0xFD4BFCCB, 0xDAC70801, 0x1041FC8B, 0xFC49FC09, 0xFD4DFC0B,
    0xFBC9FCCB, 0xFD0BFB87, 0xFD4BFE4F, 0xFE0DFE0D, 0xFC4BFD8B, 0xFF91FE0D, 0xFC89FD4B, 0xFE8DFE0D, 0xFCC9FC4B,
    0xFC49FCC9, 0xFE8FFD4D, 0xFCC9FC89, 0xC2072081, 0x1841EB49, 0xFD0BFC09, 0xFCCDFC09, 0xFB49FBC9, 0xFC09F307,
    0xFBC9FD0B, 0xFC09FB89, 0xFB89FD4D, 0xFDCDFBC9, 0xFB89F389, 0xFD0DFCCB, 0xFB47FB4B, 0xFB89FC49, 0xFD8FFC8B,
    0xFC8BF347, 0xA1C51841, 0x0001CA87, 0xFC8BFB89, 0xFB89FB89, 0xEB09F347, 0xFBC9E2C9, 0xDAC7FB49, 0xAA05E2C7,
    0xD245EB8B, 0xEB89D247, 0xA1C7AA05, 0xFB8BFB89, 0xC247DA87, 0xF30BFC0B, 0xFBCBF349, 0xF349CA87, 0xA1C30001,
    0x000199C5, 0xEB09FB07, 0xDAC7F309, 0xDA87B207, 0x99C59185, 0x8145A1C5, 0x40839185, 0x9183D287, 0xCA479185,
    0x71059185, 0xA1C5C247, 0xA1C5A9C7, 0xDAC9F2C9, 0xB207AA07, 0xCA47A9C5, 0x91850001, 0x00016945, 0xCA47E2C7,
    0xC247D247, 0xB24768C3, 0x28412841, 0x508360C3, 0x58C37945, 0x8985A9C5, 0x71032841, 0x50C37905, 0x89837105,
    0x60C36903, 0xA9C799C7, 0x69038145, 0x91859185, 0x81830001, 0x00013083, 0x99C5DA89, 0xCA479985, 0xCA896903,
    0x28013843, 0x50C15083, 0x40836103, 0x68C36103, 0x50C32001, 0x404358C3, 0x58C34081, 0x488358C3, 0x79454041,
    0x488348C3, 0x5081A1C5, 0x48C30001, 0x00010801, 0x71458985, 0xC2078985, 0xA18791C5, 0x48832801, 0x38814883,
    0x30414043, 0x408150C3, 0x28012841, 0x20014083, 0x30413001, 0x40837943, 0x48833041, 0x60C33843, 0x7103C247,
    0x08010001, 0x00010001, 0x308358C3, 0x81458985, 0x60C37103, 0x59033001, 0x28413043, 0x20012001, 0x38413883,
    0x28413001, 0x18012841, 0x20012001, 0x58C358C3, 0x488358C3, 0x690360C3, 0x99C56905, 0x00010001, 0x00010001,
    0x100148C3, 0x58C36905, 0x48813043, 0x28412801, 0x28012041, 0x18011801, 0x30413043, 0x20012841, 0x20012001,
    0x18012841, 0x40433881, 0x384158C3, 0x50C34083, 0x48810801, 0x00010001, 0x00010001, 0x00012041, 0x48834083,
    0x40832801, 0x18012001, 0x18012001, 0x18011801, 0x20012041, 0x20011801, 0x18012001, 0x18012841, 0x38412001,
    0x20014083, 0x28012841, 0x18010001, 0x00010001, 0x00010001, 0x00010001, 0x20412001, 0x30433001, 0x18011801,
    0x18012001, 0x18011001, 0x18011801, 0x18011801, 0x18011801, 0x18012001, 0x28412001, 0x18013841, 0x20011801,
    0x08010001, 0x00010001, 0x00010001, 0x00010001, 0x00011801, 0x20012001, 0x18011801, 0x20011801, 0x18011801,
    0x10011801, 0x18011001, 0x18011801, 0x18011801, 0x18012801, 0x28412841, 0x18010801, 0x00010001, 0x00010001,
    0x00010001, 0x00010001, 0x00010801, 0x10012001, 0x18011801, 0x18011001, 0x18011801, 0x10011801, 0x18011801,
    0x10011801, 0x18011801, 0x18012001, 0x18011801, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001,
    0x00010001, 0x00011001, 0x18011801, 0x18011001, 0x18011801, 0x10011001, 0x10011801, 0x10011801, 0x18011001,
    0x18011801, 0x08010801, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001,
    0x00011001, 0x10011801, 0x10011801, 0x10011001, 0x10011001, 0x10011801, 0x10011801, 0x10010001, 0x00010001,
    0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010801,
    0x08011001, 0x10011001, 0x10011001, 0x10010801, 0x08010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001,
    0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001,
    0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001, 0x00010001,
};
static Gfx D_84B12420[] = {
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPSetCombineMode(G_CC_MODULATEI, G_CC_PASS2),
    gsSPSetGeometryMode(G_LIGHTING | G_TEXTURE_GEN),
    gsSPClearGeometryMode(G_TEXTURE_GEN_LINEAR),
    gsSPTexture(0x1194, 0x1194, 0, G_TX_RENDERTILE, G_ON),
    gsDPLoadTextureBlock(D_84B11C20, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_NOMIRROR | G_TX_WRAP,
                         G_TX_NOMIRROR | G_TX_CLAMP, 5, 5, 1, 1),
    gsSPEndDisplayList(),
};

/**
 * Geo callback (draw phase 5) for a versus-screen icon column in the single layout. Each column has two child icon
 * nodes (callbackArg 0 = upper row, 1 = lower row). Binds segment 0x0F to that Pokémon's PokeIcon image (side 0
 * slots 0-5 from nodes 2-4, side 1 slots 6-11 from nodes 5-7) and segment 0x0E to the blue side 0 panel D_300C000
 * or the green side 1 panel D_300D800.
 */
s32 BattlePrepRoster_DrawIconColumnSingle(s32 phase, unk_func_80011B94* display_node) {
    BattlePrepRoster* roster;
    u8* panel_texture;
    s32 node_index;
    s32 icon_index;

    if (phase == 5) {
        roster = (BattlePrepRoster*)D_8006F09C->node.callbackArg;

        node_index = D_8006F09C->effectSlot - roster->displayObjects->effectSlot;
        if (node_index < 5) {
            icon_index = ((display_node->display_node_header.callbackArg * 3) + node_index) - 2;
            panel_texture = D_300C000;
        } else {
            icon_index = (display_node->display_node_header.callbackArg * 3) + node_index + 1;
            panel_texture = D_300D800;
        }

        gDPPipeSync(gDisplayListHead++);

        gSPSegment(gDisplayListHead++, 0x0F, (u32)PokeIcon_GetImage(roster->iconRenderList, icon_index) & 0x1FFFFFFF);
        gSPSegment(gDisplayListHead++, 0x0E, (u32)Memmap_GetSegmentVaddr(panel_texture) & 0x1FFFFFFF);
    }
    return 0;
}

/**
 * Geo callback (draw phase 5) for a versus-screen icon column in the tag-team layout. Each column has three child
 * icon nodes (callbackArg 0-2 = rows); two columns hold one tag-team member's six icons. Slots: side 0 first member
 * 0-5 (nodes 1-2), second member 6-11 (nodes 3-4); side 1 12-17 (nodes 5-6) and 18-23 (nodes 7-8). Binds the slot's
 * PokeIcon image to segment 0x0F and the blue D_300F000 / green D_300FC00 panel to segment 0x0E.
 */
s32 BattlePrepRoster_DrawIconColumnDouble(s32 phase, unk_func_80011B94* display_node) {
    BattlePrepRoster* roster;
    u8* panel_texture;
    s32 node_index;
    s32 icon_index;

    if (phase == 5) {
        roster = (BattlePrepRoster*)D_8006F09C->node.callbackArg;

        node_index = D_8006F09C->effectSlot - roster->displayObjects->effectSlot;
        if (node_index < 5) {
            icon_index = (display_node->display_node_header.callbackArg * 2) + (((node_index - 1) / 2) * 6) + ((node_index - 1) % 2);
            panel_texture = D_300F000;
        } else {
            icon_index =
                (display_node->display_node_header.callbackArg * 2) + (((node_index - 5) / 2) * 6) + ((node_index - 5) % 2) + 0xC;
            panel_texture = D_300FC00;
        }

        gDPPipeSync(gDisplayListHead++);

        gSPSegment(gDisplayListHead++, 0x0F, (u32)PokeIcon_GetImage(roster->iconRenderList, icon_index) & 0x1FFFFFFF);
        gSPSegment(gDisplayListHead++, 0x0E, (u32)Memmap_GetSegmentVaddr(panel_texture) & 0x1FFFFFFF);
    }
    return 0;
}

/**
 * Geo callback (draw phase 5) for a trainer card. Binds segment 0x0F to side0CardImage for display object 0 (side
 * 0, top row) or side1CardImage otherwise, draws the display list in the node's callbackArg, and reapplies material
 * state. The 64x96 card images are composed by BattlePrep_DrawTrainerStatusIcon: card background, trainer portrait,
 * and the 1P/2P/COM tag or trainer name.
 */
s32 BattlePrepRoster_DrawTrainerCard(s32 phase, unk_func_80011B94* display_node) {
    BattlePrepRoster* roster;
    u8* card_image;

    if (phase == 5) {
        roster = (BattlePrepRoster*)D_8006F09C->node.callbackArg;
        if (roster->displayObjects->effectSlot == D_8006F09C->effectSlot) {
            card_image = roster->side0CardImage->img_p;
        } else {
            card_image = roster->side1CardImage->img_p;
        }

        gDPPipeSync(gDisplayListHead++);
        gSPSegment(gDisplayListHead++, 0x0F, (u32)card_image & 0x1FFFFFFF);
        gSPDisplayList(gDisplayListHead++, display_node->display_node_header.callbackArg);

        GeoRender_ApplyMaterialState();
    }
    return 0;
}

/**
 * Geo callback (draw phase 5) for the 3D V/S logo material: emits D_84B12420, which reflection-maps (G_TEXTURE_GEN)
 * the 32x32 orange-and-white texture D_84B11C20, then moves the tile's S offset back one texel per frame, wrapping
 * from below 0 to 0x80, so the shine slides across the logo. Falls off the end without a return value.
 */
s32 BattlePrepRoster_ScrollVersusLogoShine(s32 phase, unk_func_80011B94* display_node) {
    static s32 D_84B12490 = 0x80;

    s32 unused;

    if (phase == 5) {
        D_84B12490--;
        if (D_84B12490 < 0) {
            D_84B12490 = 0x80;
        }

        gSPDisplayList(gDisplayListHead++, D_84B12420);
        gDPSetTileSize(gDisplayListHead++, G_TX_RENDERTILE, D_84B12490, 0, (D_84B12490 + 0x1F) << 2, 0x007C);
    }
}

static Vtx D_84B12498[362] = {
    VTX(-1, -38, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF), VTX(-31, 28, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),
    VTX(-21, 35, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF), VTX(0, -11, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),
    VTX(23, 36, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),  VTX(34, 31, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),
    VTX(0, -38, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),  VTX(-4, -43, -19, 0, 0, 0xF9, 0xAC, 0xAC, 0xFF),
    VTX(-1, -38, -25, 0, 0, 0xF9, 0xAC, 0xAC, 0xFF), VTX(0, -38, -25, 0, 0, 0xF9, 0xAC, 0xAC, 0xFF),
    VTX(3, -43, -19, 0, 0, 0xF9, 0xAC, 0xAC, 0xFF),  VTX(3, -43, -19, 0, 0, 0x4C, 0xDB, 0xAC, 0xFF),
    VTX(0, -38, -25, 0, 0, 0x4C, 0xDB, 0xAC, 0xFF),  VTX(34, 31, -25, 0, 0, 0x4C, 0xDB, 0xAC, 0xFF),
    VTX(41, 33, -19, 0, 0, 0x4C, 0xDB, 0xAC, 0xFF),  VTX(41, 33, -19, 0, 0, 0x24, 0x4C, 0xAC, 0xFF),
    VTX(34, 31, -25, 0, 0, 0x24, 0x4C, 0xAC, 0xFF),  VTX(23, 36, -25, 0, 0, 0x24, 0x4C, 0xAC, 0xFF),
    VTX(20, 43, -19, 0, 0, 0x24, 0x4C, 0xAC, 0xFF),  VTX(20, 43, -19, 0, 0, 0xB4, 0x25, 0xAC, 0xFF),
    VTX(23, 36, -25, 0, 0, 0xB4, 0x25, 0xAC, 0xFF),  VTX(0, -11, -25, 0, 0, 0xB4, 0x25, 0xAC, 0xFF),
    VTX(0, 0, -19, 0, 0, 0xB4, 0x25, 0xAC, 0xFF),    VTX(0, 0, -19, 0, 0, 0x4D, 0x22, 0xAC, 0xFF),
    VTX(0, -11, -25, 0, 0, 0x4D, 0x22, 0xAC, 0xFF),  VTX(-21, 35, -25, 0, 0, 0x4D, 0x22, 0xAC, 0xFF),
    VTX(-19, 42, -19, 0, 0, 0x4D, 0x22, 0xAC, 0xFF), VTX(-19, 42, -19, 0, 0, 0xD1, 0x46, 0xAC, 0xFF),
    VTX(-21, 35, -25, 0, 0, 0xD1, 0x46, 0xAC, 0xFF), VTX(-31, 28, -25, 0, 0, 0xD1, 0x46, 0xAC, 0xFF),
    VTX(-37, 29, -19, 0, 0, 0xD1, 0x46, 0xAC, 0xFF),
    VTX(-37, 29, -19, 0, 0, 0xB3, 0xDD, 0xAC, 0xFF), VTX(-31, 28, -25, 0, 0, 0xB3, 0xDD, 0xAC, 0xFF),
    VTX(-1, -38, -25, 0, 0, 0xB3, 0xDD, 0xAC, 0xFF), VTX(-4, -43, -19, 0, 0, 0xB3, 0xDD, 0xAC, 0xFF),
    VTX(-4, -43, 19, 0, 0, 0xF7, 0x89, 0x00, 0xFF),  VTX(-4, -43, -19, 0, 0, 0xF7, 0x89, 0x00, 0xFF),
    VTX(3, -43, -19, 0, 0, 0xF7, 0x89, 0x00, 0xFF),  VTX(3, -43, 19, 0, 0, 0xF7, 0x89, 0x00, 0xFF),
    VTX(3, -43, 19, 0, 0, 0x6B, 0xCC, 0x00, 0xFF),   VTX(3, -43, -19, 0, 0, 0x6B, 0xCC, 0x00, 0xFF),
    VTX(41, 33, -19, 0, 0, 0x6B, 0xCC, 0x00, 0xFF),  VTX(41, 33, 19, 0, 0, 0x6B, 0xCC, 0x00, 0xFF),
    VTX(41, 33, 19, 0, 0, 0x33, 0x6C, 0x00, 0xFF),   VTX(41, 33, -19, 0, 0, 0x33, 0x6C, 0x00, 0xFF),
    VTX(20, 43, -19, 0, 0, 0x33, 0x6C, 0x00, 0xFF),  VTX(20, 43, 19, 0, 0, 0x33, 0x6C, 0x00, 0xFF),
    VTX(20, 43, 19, 0, 0, 0x95, 0x34, 0x00, 0xFF),   VTX(20, 43, -19, 0, 0, 0x95, 0x34, 0x00, 0xFF),
    VTX(0, 0, -19, 0, 0, 0x95, 0x34, 0x00, 0xFF),    VTX(0, 0, 19, 0, 0, 0x95, 0x34, 0x00, 0xFF),
    VTX(0, 0, 19, 0, 0, 0x6D, 0x31, 0x00, 0xFF),     VTX(0, 0, -19, 0, 0, 0x6D, 0x31, 0x00, 0xFF),
    VTX(-19, 42, -19, 0, 0, 0x6D, 0x31, 0x00, 0xFF), VTX(-19, 42, 19, 0, 0, 0x6D, 0x31, 0x00, 0xFF),
    VTX(-19, 42, 19, 0, 0, 0xBD, 0x63, 0x00, 0xFF),  VTX(-19, 42, -19, 0, 0, 0xBD, 0x63, 0x00, 0xFF),
    VTX(-37, 29, -19, 0, 0, 0xBD, 0x63, 0x00, 0xFF), VTX(-37, 29, 19, 0, 0, 0xBD, 0x63, 0x00, 0xFF),
    VTX(-37, 29, 19, 0, 0, 0x93, 0xCF, 0x00, 0xFF),  VTX(-37, 29, -19, 0, 0, 0x93, 0xCF, 0x00, 0xFF),
    VTX(-4, -43, -19, 0, 0, 0x93, 0xCF, 0x00, 0xFF), VTX(-4, -43, 19, 0, 0, 0x93, 0xCF, 0x00, 0xFF),
    VTX(-1, -38, 25, 0, 0, 0xF9, 0xAC, 0x54, 0xFF), VTX(-4, -43, 19, 0, 0, 0xF9, 0xAC, 0x54, 0xFF),
    VTX(3, -43, 19, 0, 0, 0xF9, 0xAC, 0x54, 0xFF),  VTX(0, -38, 25, 0, 0, 0xF9, 0xAC, 0x54, 0xFF),
    VTX(0, -38, 25, 0, 0, 0x4C, 0xDB, 0x54, 0xFF),  VTX(3, -43, 19, 0, 0, 0x4C, 0xDB, 0x54, 0xFF),
    VTX(41, 33, 19, 0, 0, 0x4C, 0xDB, 0x54, 0xFF),  VTX(34, 31, 25, 0, 0, 0x4C, 0xDB, 0x54, 0xFF),
    VTX(34, 31, 25, 0, 0, 0x24, 0x4C, 0x54, 0xFF),  VTX(41, 33, 19, 0, 0, 0x24, 0x4C, 0x54, 0xFF),
    VTX(20, 43, 19, 0, 0, 0x24, 0x4C, 0x54, 0xFF),  VTX(23, 36, 25, 0, 0, 0x24, 0x4C, 0x54, 0xFF),
    VTX(23, 36, 25, 0, 0, 0xB4, 0x25, 0x54, 0xFF),  VTX(20, 43, 19, 0, 0, 0xB4, 0x25, 0x54, 0xFF),
    VTX(0, 0, 19, 0, 0, 0xB4, 0x25, 0x54, 0xFF),    VTX(0, -11, 25, 0, 0, 0xB4, 0x25, 0x54, 0xFF),
    VTX(0, -11, 25, 0, 0, 0x4D, 0x22, 0x54, 0xFF),  VTX(0, 0, 19, 0, 0, 0x4D, 0x22, 0x54, 0xFF),
    VTX(-19, 42, 19, 0, 0, 0x4D, 0x22, 0x54, 0xFF), VTX(-21, 35, 25, 0, 0, 0x4D, 0x22, 0x54, 0xFF),
    VTX(-21, 35, 25, 0, 0, 0xD1, 0x46, 0x54, 0xFF), VTX(-19, 42, 19, 0, 0, 0xD1, 0x46, 0x54, 0xFF),
    VTX(-37, 29, 19, 0, 0, 0xD1, 0x46, 0x54, 0xFF), VTX(-31, 28, 25, 0, 0, 0xD1, 0x46, 0x54, 0xFF),
    VTX(-31, 28, 25, 0, 0, 0xB3, 0xDD, 0x54, 0xFF), VTX(-37, 29, 19, 0, 0, 0xB3, 0xDD, 0x54, 0xFF),
    VTX(-4, -43, 19, 0, 0, 0xB3, 0xDD, 0x54, 0xFF), VTX(-1, -38, 25, 0, 0, 0xB3, 0xDD, 0x54, 0xFF),
    VTX(0, -38, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),  VTX(34, 31, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),
    VTX(23, 36, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),  VTX(0, -11, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),
    VTX(0, -38, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),  VTX(0, -11, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),
    VTX(-21, 35, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF), VTX(-31, 28, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),
    VTX(-1, -38, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),
    VTX(0, 33, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),   VTX(8, 36, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),
    VTX(12, 30, -25, 0, 0, 0x0D, 0xD5, 0x92, 0xFF),  VTX(12, 30, -25, 0, 0, 0x13, 0xC7, 0x99, 0xFF),
    VTX(8, 28, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),   VTX(23, -12, -19, 0, 0, 0x51, 0x02, 0xA8, 0xFF),
    VTX(17, -12, -25, 0, 0, 0x51, 0x02, 0xA8, 0xFF), VTX(15, -6, -25, 0, 0, 0x45, 0x2F, 0xAB, 0xFF),
    VTX(19, -3, -19, 0, 0, 0x45, 0x2F, 0xAB, 0xFF),  VTX(7, 0, -25, 0, 0, 0x39, 0x3D, 0xAC, 0xFF),
    VTX(11, 4, -19, 0, 0, 0x39, 0x3D, 0xAC, 0xFF),   VTX(0, 8, -25, 0, 0, 0x3F, 0x37, 0xAB, 0xFF),
    VTX(2, 11, -19, 0, 0, 0x3F, 0x37, 0xAB, 0xFF),   VTX(-6, 17, -25, 0, 0, 0x49, 0xFA, 0xA2, 0xFF),
    VTX(0, 16, -19, 0, 0, 0x49, 0xFA, 0xA2, 0xFF),   VTX(0, 24, -25, 0, 0, 0x31, 0xBD, 0xAB, 0xFF),
    VTX(3, 20, -19, 0, 0, 0x31, 0xBD, 0xAB, 0xFF),   VTX(8, 28, -25, 0, 0, 0x20, 0xB3, 0xAB, 0xFF),
    VTX(10, 24, -19, 0, 0, 0x20, 0xB3, 0xAB, 0xFF),  VTX(17, 26, -19, 0, 0, 0x1A, 0xB0, 0xAB, 0xFF),
    VTX(20, 27, -19, 0, 0, 0x18, 0xB0, 0xAC, 0xFF),  VTX(20, 27, -19, 0, 0, 0x46, 0x2F, 0xAC, 0xFF),
    VTX(12, 30, -25, 0, 0, 0x46, 0x2F, 0xAC, 0xFF),  VTX(8, 36, -25, 0, 0, 0x15, 0x46, 0xA2, 0xFF),
    VTX(10, 42, -19, 0, 0, 0x15, 0x46, 0xA2, 0xFF),  VTX(0, 33, -25, 0, 0, 0xD9, 0x4A, 0xAB, 0xFF),
    VTX(-1, 37, -19, 0, 0, 0xD9, 0x4A, 0xAB, 0xFF),  VTX(-9, 26, -25, 0, 0, 0xCA, 0x40, 0xAB, 0xFF),
    VTX(-12, 30, -19, 0, 0, 0xCA, 0x40, 0xAB, 0xFF), VTX(-16, 19, -25, 0, 0, 0xBA, 0x2D, 0xAB, 0xFF),
    VTX(-20, 21, -19, 0, 0, 0xBA, 0x2D, 0xAB, 0xFF), VTX(-18, 14, -25, 0, 0, 0xB2, 0x1F, 0xAC, 0xFF),
    VTX(-20, 21, -19, 0, 0, 0xBA, 0x2D, 0xAB, 0xFF),  VTX(-18, 14, -25, 0, 0, 0xB2, 0x1F, 0xAC, 0xFF),
    VTX(-23, 14, -19, 0, 0, 0xB2, 0x1F, 0xAC, 0xFF),  VTX(-23, 14, -19, 0, 0, 0xB1, 0xE4, 0xAC, 0xFF),
    VTX(-18, 14, -25, 0, 0, 0xB1, 0xE4, 0xAC, 0xFF),  VTX(-16, 8, -25, 0, 0, 0xB8, 0xD6, 0xAB, 0xFF),
    VTX(-20, 5, -19, 0, 0, 0xB8, 0xD6, 0xAB, 0xFF),   VTX(-10, 1, -25, 0, 0, 0xC4, 0xC5, 0xAC, 0xFF),
    VTX(-13, -1, -19, 0, 0, 0xC4, 0xC5, 0xAC, 0xFF),  VTX(-2, -5, -25, 0, 0, 0xBF, 0xCB, 0xAB, 0xFF),
    VTX(-6, -8, -19, 0, 0, 0xBF, 0xCB, 0xAB, 0xFF),   VTX(1, -12, -25, 0, 0, 0xB2, 0xFE, 0xA6, 0xFF),
    VTX(-3, -13, -19, 0, 0, 0xB2, 0xFE, 0xA6, 0xFF),  VTX(-2, -21, -25, 0, 0, 0xBE, 0x33, 0xAB, 0xFF),
    VTX(-6, -18, -19, 0, 0, 0xBE, 0x33, 0xAB, 0xFF),  VTX(-8, -26, -25, 0, 0, 0xCD, 0x43, 0xAC, 0xFF),
    VTX(-11, -22, -19, 0, 0, 0xCD, 0x43, 0xAC, 0xFF), VTX(-14, -30, -25, 0, 0, 0xDF, 0x32, 0x99, 0xFF),
    VTX(-16, -26, -19, 0, 0, 0xD2, 0x46, 0xAB, 0xFF), VTX(-13, -30, -25, 0, 0, 0xE8, 0x25, 0x91, 0xFF),
    VTX(-19, -27, -19, 0, 0, 0xD3, 0x45, 0xAA, 0xFF), VTX(-19, -27, -19, 0, 0, 0xAE, 0xDE, 0xB1, 0xFF),
    VTX(-13, -30, -25, 0, 0, 0xAE, 0xDE, 0xB1, 0xFF), VTX(-9, -38, -25, 0, 0, 0xAE, 0xDE, 0xB1, 0xFF),
    VTX(-12, -45, -19, 0, 0, 0xAE, 0xDE, 0xB1, 0xFF), VTX(-12, -45, -19, 0, 0, 0x25, 0xB4, 0xAC, 0xFF),
    VTX(-9, -38, -25, 0, 0, 0x25, 0xB4, 0xAC, 0xFF),  VTX(-2, -34, -25, 0, 0, 0x2C, 0xB9, 0xAB, 0xFF),
    VTX(0, -38, -19, 0, 0, 0x2C, 0xB9, 0xAB, 0xFF),   VTX(7, -26, -25, 0, 0, 0x39, 0xC3, 0xAB, 0xFF),
    VTX(11, -30, -19, 0, 0, 0x39, 0xC3, 0xAB, 0xFF),  VTX(15, -17, -25, 0, 0, 0x47, 0xD5, 0xAB, 0xFF),
    VTX(11, -30, -19, 0, 0, 0x39, 0xC3, 0xAB, 0xFF), VTX(15, -17, -25, 0, 0, 0x47, 0xD5, 0xAB, 0xFF),
    VTX(19, -20, -19, 0, 0, 0x47, 0xD5, 0xAB, 0xFF), VTX(17, -12, -25, 0, 0, 0x51, 0x02, 0xA8, 0xFF),
    VTX(23, -12, -19, 0, 0, 0x51, 0x02, 0xA8, 0xFF), VTX(23, -12, 19, 0, 0, 0x77, 0x03, 0x00, 0xFF),
    VTX(23, -12, -19, 0, 0, 0x77, 0x03, 0x00, 0xFF), VTX(19, -3, -19, 0, 0, 0x62, 0x44, 0x00, 0xFF),
    VTX(19, -3, 19, 0, 0, 0x62, 0x44, 0x00, 0xFF),   VTX(11, 4, -19, 0, 0, 0x51, 0x57, 0x00, 0xFF),
    VTX(11, 4, 19, 0, 0, 0x51, 0x57, 0x00, 0xFF),    VTX(2, 11, -19, 0, 0, 0x5A, 0x4F, 0x00, 0xFF),
    VTX(2, 11, 19, 0, 0, 0x5A, 0x4F, 0x00, 0xFF),    VTX(0, 16, -19, 0, 0, 0x63, 0x42, 0x00, 0xFF),
    VTX(0, 16, 19, 0, 0, 0x63, 0x42, 0x00, 0xFF),    VTX(0, 16, 19, 0, 0, 0x57, 0xAE, 0x00, 0xFF),
    VTX(0, 16, -19, 0, 0, 0x57, 0xAE, 0x00, 0xFF),   VTX(3, 20, -19, 0, 0, 0x46, 0xA0, 0x00, 0xFF),
    VTX(3, 20, 19, 0, 0, 0x46, 0xA0, 0x00, 0xFF),    VTX(10, 24, -19, 0, 0, 0x2D, 0x92, 0x00, 0xFF),
    VTX(10, 24, 19, 0, 0, 0x2D, 0x92, 0x00, 0xFF),   VTX(17, 26, -19, 0, 0, 0x25, 0x8E, 0x00, 0xFF),
    VTX(17, 26, 19, 0, 0, 0x25, 0x8E, 0x00, 0xFF),   VTX(20, 27, -19, 0, 0, 0x22, 0x8E, 0x00, 0xFF),
    VTX(20, 27, 19, 0, 0, 0x22, 0x8E, 0x00, 0xFF),   VTX(20, 27, 19, 0, 0, 0x63, 0x42, 0x00, 0xFF),
    VTX(20, 27, -19, 0, 0, 0x63, 0x42, 0x00, 0xFF),  VTX(10, 42, -19, 0, 0, 0x63, 0x42, 0x00, 0xFF),
    VTX(10, 42, 19, 0, 0, 0x63, 0x42, 0x00, 0xFF),   VTX(10, 42, 19, 0, 0, 0xD4, 0x6F, 0x00, 0xFF),
    VTX(10, 42, -19, 0, 0, 0xD4, 0x6F, 0x00, 0xFF),  VTX(-1, 37, -19, 0, 0, 0xC8, 0x6A, 0x00, 0xFF),
    VTX(10, 42, 19, 0, 0, 0xD4, 0x6F, 0x00, 0xFF),    VTX(-1, 37, -19, 0, 0, 0xC8, 0x6A, 0x00, 0xFF),
    VTX(-1, 37, 19, 0, 0, 0xC8, 0x6A, 0x00, 0xFF),    VTX(-12, 30, -19, 0, 0, 0xB3, 0x5B, 0x00, 0xFF),
    VTX(-12, 30, 19, 0, 0, 0xB3, 0x5B, 0x00, 0xFF),   VTX(-20, 21, -19, 0, 0, 0x9C, 0x40, 0x00, 0xFF),
    VTX(-20, 21, 19, 0, 0, 0x9C, 0x40, 0x00, 0xFF),   VTX(-23, 14, -19, 0, 0, 0x89, 0x02, 0x00, 0xFF),
    VTX(-23, 14, 19, 0, 0, 0x89, 0x02, 0x00, 0xFF),   VTX(-20, 5, -19, 0, 0, 0x99, 0xC3, 0x00, 0xFF),
    VTX(-20, 5, 19, 0, 0, 0x99, 0xC3, 0x00, 0xFF),    VTX(-13, -1, -19, 0, 0, 0xAB, 0xAD, 0x00, 0xFF),
    VTX(-13, -1, 19, 0, 0, 0xAB, 0xAD, 0x00, 0xFF),   VTX(-6, -8, -19, 0, 0, 0xA3, 0xB5, 0x00, 0xFF),
    VTX(-6, -8, 19, 0, 0, 0xA3, 0xB5, 0x00, 0xFF),    VTX(-3, -13, -19, 0, 0, 0x89, 0xFC, 0x00, 0xFF),
    VTX(-3, -13, 19, 0, 0, 0x89, 0xFC, 0x00, 0xFF),   VTX(-6, -18, -19, 0, 0, 0xA2, 0x49, 0x00, 0xFF),
    VTX(-6, -18, 19, 0, 0, 0xA2, 0x49, 0x00, 0xFF),   VTX(-11, -22, -19, 0, 0, 0xB8, 0x5F, 0x00, 0xFF),
    VTX(-11, -22, 19, 0, 0, 0xB8, 0x5F, 0x00, 0xFF),  VTX(-16, -26, -19, 0, 0, 0xC1, 0x66, 0x00, 0xFF),
    VTX(-16, -26, 19, 0, 0, 0xC1, 0x66, 0x00, 0xFF),  VTX(-19, -27, -19, 0, 0, 0xC5, 0x68, 0x00, 0xFF),
    VTX(-19, -27, 19, 0, 0, 0xC5, 0x68, 0x00, 0xFF),  VTX(-19, -27, 19, 0, 0, 0x90, 0xD6, 0x00, 0xFF),
    VTX(-19, -27, -19, 0, 0, 0x90, 0xD6, 0x00, 0xFF), VTX(-12, -45, -19, 0, 0, 0x90, 0xD6, 0x00, 0xFF),
    VTX(-12, -45, 19, 0, 0, 0x90, 0xD6, 0x00, 0xFF),  VTX(-12, -45, 19, 0, 0, 0x35, 0x95, 0x00, 0xFF),
    VTX(-12, -45, -19, 0, 0, 0x35, 0x95, 0x00, 0xFF), VTX(0, -38, -19, 0, 0, 0x3F, 0x9B, 0x00, 0xFF),
    VTX(-12, -45, 19, 0, 0, 0x35, 0x95, 0x00, 0xFF), VTX(0, -38, -19, 0, 0, 0x3F, 0x9B, 0x00, 0xFF),
    VTX(0, -38, 19, 0, 0, 0x3F, 0x9B, 0x00, 0xFF),   VTX(11, -30, -19, 0, 0, 0x51, 0xA9, 0x00, 0xFF),
    VTX(11, -30, 19, 0, 0, 0x51, 0xA9, 0x00, 0xFF),  VTX(19, -20, -19, 0, 0, 0x66, 0xC2, 0x00, 0xFF),
    VTX(19, -20, 19, 0, 0, 0x66, 0xC2, 0x00, 0xFF),  VTX(23, -12, -19, 0, 0, 0x77, 0x03, 0x00, 0xFF),
    VTX(23, -12, 19, 0, 0, 0x77, 0x03, 0x00, 0xFF),  VTX(17, -12, 25, 0, 0, 0x51, 0x02, 0x58, 0xFF),
    VTX(23, -12, 19, 0, 0, 0x51, 0x02, 0x58, 0xFF),  VTX(19, -3, 19, 0, 0, 0x45, 0x2F, 0x55, 0xFF),
    VTX(15, -6, 25, 0, 0, 0x45, 0x2F, 0x55, 0xFF),   VTX(11, 4, 19, 0, 0, 0x39, 0x3D, 0x54, 0xFF),
    VTX(7, 0, 25, 0, 0, 0x39, 0x3D, 0x54, 0xFF),     VTX(2, 11, 19, 0, 0, 0x3F, 0x37, 0x55, 0xFF),
    VTX(0, 8, 25, 0, 0, 0x3F, 0x37, 0x55, 0xFF),     VTX(0, 16, 19, 0, 0, 0x49, 0xFA, 0x5E, 0xFF),
    VTX(-6, 17, 25, 0, 0, 0x49, 0xFA, 0x5E, 0xFF),   VTX(3, 20, 19, 0, 0, 0x31, 0xBD, 0x55, 0xFF),
    VTX(0, 24, 25, 0, 0, 0x31, 0xBD, 0x55, 0xFF),    VTX(10, 24, 19, 0, 0, 0x20, 0xB3, 0x55, 0xFF),
    VTX(8, 28, 25, 0, 0, 0x20, 0xB3, 0x55, 0xFF),    VTX(17, 26, 19, 0, 0, 0x1A, 0xB0, 0x55, 0xFF),
    VTX(12, 30, 25, 0, 0, 0x13, 0xC7, 0x67, 0xFF),   VTX(20, 27, 19, 0, 0, 0x18, 0xB0, 0x54, 0xFF),
    VTX(12, 30, 25, 0, 0, 0x0D, 0xD5, 0x6E, 0xFF),   VTX(12, 30, 25, 0, 0, 0x46, 0x2F, 0x54, 0xFF),
    VTX(20, 27, 19, 0, 0, 0x46, 0x2F, 0x54, 0xFF),   VTX(10, 42, 19, 0, 0, 0x15, 0x46, 0x5E, 0xFF),
    VTX(8, 36, 25, 0, 0, 0x15, 0x46, 0x5E, 0xFF),    VTX(-1, 37, 19, 0, 0, 0xD9, 0x4A, 0x55, 0xFF),
    VTX(8, 36, 25, 0, 0, 0x15, 0x46, 0x5E, 0xFF),    VTX(-1, 37, 19, 0, 0, 0xD9, 0x4A, 0x55, 0xFF),
    VTX(0, 33, 25, 0, 0, 0xD9, 0x4A, 0x55, 0xFF),    VTX(-12, 30, 19, 0, 0, 0xCA, 0x40, 0x55, 0xFF),
    VTX(-9, 26, 25, 0, 0, 0xCA, 0x40, 0x55, 0xFF),   VTX(-20, 21, 19, 0, 0, 0xBA, 0x2D, 0x55, 0xFF),
    VTX(-16, 19, 25, 0, 0, 0xBA, 0x2D, 0x55, 0xFF),  VTX(-23, 14, 19, 0, 0, 0xAF, 0x01, 0x57, 0xFF),
    VTX(-18, 14, 25, 0, 0, 0xAF, 0x01, 0x57, 0xFF),  VTX(-20, 5, 19, 0, 0, 0xB8, 0xD6, 0x55, 0xFF),
    VTX(-16, 8, 25, 0, 0, 0xB8, 0xD6, 0x55, 0xFF),   VTX(-13, -1, 19, 0, 0, 0xC4, 0xC5, 0x54, 0xFF),
    VTX(-10, 1, 25, 0, 0, 0xC4, 0xC5, 0x54, 0xFF),   VTX(-6, -8, 19, 0, 0, 0xBF, 0xCB, 0x55, 0xFF),
    VTX(-2, -5, 25, 0, 0, 0xBF, 0xCB, 0x55, 0xFF),   VTX(-3, -13, 19, 0, 0, 0xB2, 0xFE, 0x5A, 0xFF),
    VTX(1, -12, 25, 0, 0, 0xB2, 0xFE, 0x5A, 0xFF),   VTX(-6, -18, 19, 0, 0, 0xBE, 0x33, 0x55, 0xFF),
    VTX(-2, -21, 25, 0, 0, 0xBE, 0x33, 0x55, 0xFF),  VTX(-11, -22, 19, 0, 0, 0xCD, 0x43, 0x54, 0xFF),
    VTX(-8, -26, 25, 0, 0, 0xCD, 0x43, 0x54, 0xFF),  VTX(-16, -26, 19, 0, 0, 0xD2, 0x46, 0x55, 0xFF),
    VTX(-14, -30, 25, 0, 0, 0xDF, 0x32, 0x67, 0xFF), VTX(-19, -27, 19, 0, 0, 0xB8, 0x13, 0x5D, 0xFF),
    VTX(-13, -30, 25, 0, 0, 0xD0, 0x0D, 0x6C, 0xFF), VTX(-12, -45, 19, 0, 0, 0xAE, 0xDE, 0x4F, 0xFF),
    VTX(-9, -38, 25, 0, 0, 0xAE, 0xDE, 0x4F, 0xFF),  VTX(-9, -38, 25, 0, 0, 0x25, 0xB4, 0x54, 0xFF),
    VTX(-12, -45, 19, 0, 0, 0x25, 0xB4, 0x54, 0xFF), VTX(0, -38, 19, 0, 0, 0x2C, 0xB9, 0x55, 0xFF),
    VTX(-2, -34, 25, 0, 0, 0x2C, 0xB9, 0x55, 0xFF),  VTX(11, -30, 19, 0, 0, 0x39, 0xC3, 0x55, 0xFF),
    VTX(-2, -34, 25, 0, 0, 0x2C, 0xB9, 0x55, 0xFF),   VTX(11, -30, 19, 0, 0, 0x39, 0xC3, 0x55, 0xFF),
    VTX(7, -26, 25, 0, 0, 0x39, 0xC3, 0x55, 0xFF),    VTX(19, -20, 19, 0, 0, 0x47, 0xD5, 0x55, 0xFF),
    VTX(15, -17, 25, 0, 0, 0x47, 0xD5, 0x55, 0xFF),   VTX(23, -12, 19, 0, 0, 0x51, 0x02, 0x58, 0xFF),
    VTX(17, -12, 25, 0, 0, 0x51, 0x02, 0x58, 0xFF),   VTX(8, 28, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),
    VTX(12, 30, 25, 0, 0, 0x13, 0xC7, 0x67, 0xFF),    VTX(12, 30, 25, 0, 0, 0x0D, 0xD5, 0x6E, 0xFF),
    VTX(8, 36, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),     VTX(0, 33, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),
    VTX(-2, -34, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),  VTX(-9, -38, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),
    VTX(-13, -30, -25, 0, 0, 0xE8, 0x25, 0x91, 0xFF), VTX(-14, -30, -25, 0, 0, 0xDF, 0x32, 0x99, 0xFF),
    VTX(-8, -26, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),  VTX(-9, 26, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),
    VTX(0, 33, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),    VTX(8, 28, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),
    VTX(0, 24, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),    VTX(-16, 19, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),
    VTX(-6, 17, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),   VTX(-18, 14, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),
    VTX(-16, 8, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),   VTX(-10, 1, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),
    VTX(0, 8, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),     VTX(7, 0, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),
    VTX(1, -12, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),   VTX(15, -6, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),
    VTX(17, -12, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),  VTX(15, -17, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),
    VTX(-2, -21, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF), VTX(1, -12, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),
    VTX(15, -17, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF), VTX(7, -26, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),
    VTX(-8, -26, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF), VTX(-2, -34, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),
    VTX(-8, -26, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),  VTX(-14, -30, 25, 0, 0, 0xDF, 0x32, 0x67, 0xFF),
    VTX(-13, -30, 25, 0, 0, 0xD0, 0x0D, 0x6C, 0xFF), VTX(-9, -38, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),
    VTX(-2, -34, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),  VTX(0, 24, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),
    VTX(8, 28, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),    VTX(0, 33, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),
    VTX(-9, 26, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),   VTX(-6, 17, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),
    VTX(-16, 19, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),  VTX(-18, 14, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),
    VTX(-16, 8, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),   VTX(0, 8, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),
    VTX(7, 0, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),     VTX(-10, 1, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),
    VTX(7, 0, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),    VTX(15, -6, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),
    VTX(-2, -5, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),  VTX(15, -6, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),
    VTX(-2, -5, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),   VTX(1, -12, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),
    VTX(17, -12, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),  VTX(15, -17, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),
    VTX(7, -26, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),   VTX(-2, -21, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),
    VTX(7, 0, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),   VTX(-2, -5, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF),
    VTX(-10, 1, -25, 0, 0, 0x00, 0x00, 0x88, 0xFF), VTX(-16, 8, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),
    VTX(-10, 1, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),  VTX(0, 8, 25, 0, 0, 0x00, 0x00, 0x78, 0xFF),
};
static Gfx D_84B13B38[] = {
    gsSPVertex(D_84B12498, 32, 0),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(0, 3, 4, 0, 0, 4, 5, 0),
    gsSP2Triangles(0, 5, 6, 0, 7, 8, 9, 0),
    gsSP2Triangles(7, 9, 10, 0, 11, 12, 13, 0),
    gsSP2Triangles(11, 13, 14, 0, 15, 16, 17, 0),
    gsSP2Triangles(15, 17, 18, 0, 19, 20, 21, 0),
    gsSP2Triangles(19, 21, 22, 0, 23, 24, 25, 0),
    gsSP2Triangles(23, 25, 26, 0, 27, 28, 29, 0),
    gsSP1Triangle(27, 29, 30, 0),
    gsSPVertex(&D_84B12498[31], 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),
    gsSP2Triangles(16, 17, 18, 0, 16, 18, 19, 0),
    gsSP2Triangles(20, 21, 22, 0, 20, 22, 23, 0),
    gsSP2Triangles(24, 25, 26, 0, 24, 26, 27, 0),
    gsSP2Triangles(28, 29, 30, 0, 28, 30, 31, 0),
    gsSPVertex(&D_84B12498[63], 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),
    gsSP2Triangles(16, 17, 18, 0, 16, 18, 19, 0),
    gsSP2Triangles(20, 21, 22, 0, 20, 22, 23, 0),
    gsSP2Triangles(24, 25, 26, 0, 24, 26, 27, 0),
    gsSP2Triangles(28, 29, 30, 0, 28, 30, 31, 0),
    gsSPVertex(&D_84B12498[95], 5, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP1Triangle(0, 3, 4, 0),
    gsSPEndDisplayList(),
};
static Gfx D_84B13C40[] = {
    gsSPVertex(&D_84B12498[100], 32, 0),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(0, 3, 4, 0, 5, 6, 7, 0),
    gsSP2Triangles(5, 7, 8, 0, 8, 7, 9, 0),
    gsSP2Triangles(8, 9, 10, 0, 10, 9, 11, 0),
    gsSP2Triangles(10, 11, 12, 0, 12, 11, 13, 0),
    gsSP2Triangles(12, 13, 14, 0, 14, 13, 15, 0),
    gsSP2Triangles(14, 15, 16, 0, 16, 15, 17, 0),
    gsSP2Triangles(16, 17, 18, 0, 18, 17, 3, 0),
    gsSP2Triangles(18, 3, 19, 0, 19, 3, 2, 0),
    gsSP2Triangles(19, 2, 20, 0, 21, 22, 23, 0),
    gsSP2Triangles(21, 23, 24, 0, 24, 23, 25, 0),
    gsSP2Triangles(24, 25, 26, 0, 26, 25, 27, 0),
    gsSP2Triangles(26, 27, 28, 0, 28, 27, 29, 0),
    gsSP2Triangles(28, 29, 30, 0, 30, 29, 31, 0),
    gsSPVertex(&D_84B12498[132], 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(3, 5, 6, 0, 6, 5, 7, 0),
    gsSP2Triangles(6, 7, 8, 0, 8, 7, 9, 0),
    gsSP2Triangles(8, 9, 10, 0, 10, 9, 11, 0),
    gsSP2Triangles(10, 11, 12, 0, 12, 11, 13, 0),
    gsSP2Triangles(12, 13, 14, 0, 14, 13, 15, 0),
    gsSP2Triangles(14, 15, 16, 0, 16, 15, 17, 0),
    gsSP2Triangles(16, 17, 18, 0, 18, 17, 19, 0),
    gsSP2Triangles(18, 19, 20, 0, 21, 22, 23, 0),
    gsSP2Triangles(21, 23, 24, 0, 25, 26, 27, 0),
    gsSP2Triangles(25, 27, 28, 0, 28, 27, 29, 0),
    gsSP2Triangles(28, 29, 30, 0, 30, 29, 31, 0),
    gsSPVertex(&D_84B12498[164], 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 2, 1, 3, 0),
    gsSP2Triangles(2, 3, 4, 0, 5, 6, 7, 0),
    gsSP2Triangles(5, 7, 8, 0, 8, 7, 9, 0),
    gsSP2Triangles(8, 9, 10, 0, 10, 9, 11, 0),
    gsSP2Triangles(10, 11, 12, 0, 12, 11, 13, 0),
    gsSP2Triangles(12, 13, 14, 0, 15, 16, 17, 0),
    gsSP2Triangles(15, 17, 18, 0, 18, 17, 19, 0),
    gsSP2Triangles(18, 19, 20, 0, 20, 19, 21, 0),
    gsSP2Triangles(20, 21, 22, 0, 22, 21, 23, 0),
    gsSP2Triangles(22, 23, 24, 0, 25, 26, 27, 0),
    gsSP2Triangles(25, 27, 28, 0, 29, 30, 31, 0),
    gsSPVertex(&D_84B12498[196], 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 2, 1, 3, 0),
    gsSP2Triangles(2, 3, 4, 0, 4, 3, 5, 0),
    gsSP2Triangles(4, 5, 6, 0, 6, 5, 7, 0),
    gsSP2Triangles(6, 7, 8, 0, 8, 7, 9, 0),
    gsSP2Triangles(8, 9, 10, 0, 10, 9, 11, 0),
    gsSP2Triangles(10, 11, 12, 0, 12, 11, 13, 0),
    gsSP2Triangles(12, 13, 14, 0, 14, 13, 15, 0),
    gsSP2Triangles(14, 15, 16, 0, 16, 15, 17, 0),
    gsSP2Triangles(16, 17, 18, 0, 18, 17, 19, 0),
    gsSP2Triangles(18, 19, 20, 0, 20, 19, 21, 0),
    gsSP2Triangles(20, 21, 22, 0, 22, 21, 23, 0),
    gsSP2Triangles(22, 23, 24, 0, 25, 26, 27, 0),
    gsSP2Triangles(25, 27, 28, 0, 29, 30, 31, 0),
    gsSPVertex(&D_84B12498[228], 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 2, 1, 3, 0),
    gsSP2Triangles(2, 3, 4, 0, 4, 3, 5, 0),
    gsSP2Triangles(4, 5, 6, 0, 6, 5, 7, 0),
    gsSP2Triangles(6, 7, 8, 0, 9, 10, 11, 0),
    gsSP2Triangles(9, 11, 12, 0, 12, 11, 13, 0),
    gsSP2Triangles(12, 13, 14, 0, 14, 13, 15, 0),
    gsSP2Triangles(14, 15, 16, 0, 16, 15, 17, 0),
    gsSP2Triangles(16, 17, 18, 0, 18, 17, 19, 0),
    gsSP2Triangles(18, 19, 20, 0, 20, 19, 21, 0),
    gsSP2Triangles(20, 21, 22, 0, 22, 21, 23, 0),
    gsSP2Triangles(22, 23, 24, 0, 24, 23, 25, 0),
    gsSP2Triangles(24, 25, 26, 0, 27, 28, 29, 0),
    gsSP2Triangles(27, 29, 30, 0, 30, 29, 31, 0),
    gsSPVertex(&D_84B12498[260], 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 2, 1, 3, 0),
    gsSP2Triangles(2, 3, 4, 0, 4, 3, 5, 0),
    gsSP2Triangles(4, 5, 6, 0, 6, 5, 7, 0),
    gsSP2Triangles(6, 7, 8, 0, 8, 7, 9, 0),
    gsSP2Triangles(8, 9, 10, 0, 10, 9, 11, 0),
    gsSP2Triangles(10, 11, 12, 0, 12, 11, 13, 0),
    gsSP2Triangles(12, 13, 14, 0, 14, 13, 15, 0),
    gsSP2Triangles(14, 15, 16, 0, 16, 15, 17, 0),
    gsSP2Triangles(16, 17, 18, 0, 18, 17, 19, 0),
    gsSP2Triangles(18, 19, 20, 0, 20, 19, 21, 0),
    gsSP2Triangles(20, 21, 22, 0, 22, 21, 23, 0),
    gsSP2Triangles(22, 23, 24, 0, 24, 23, 25, 0),
    gsSP2Triangles(24, 25, 26, 0, 27, 28, 29, 0),
    gsSP2Triangles(27, 29, 30, 0, 30, 29, 31, 0),
    gsSPVertex(&D_84B12498[292], 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 2, 1, 3, 0),
    gsSP2Triangles(2, 3, 4, 0, 4, 3, 5, 0),
    gsSP2Triangles(4, 5, 6, 0, 7, 8, 9, 0),
    gsSP2Triangles(7, 9, 10, 0, 7, 10, 11, 0),
    gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),
    gsSP2Triangles(12, 15, 16, 0, 17, 18, 19, 0),
    gsSP2Triangles(17, 19, 20, 0, 21, 17, 20, 0),
    gsSP2Triangles(21, 20, 22, 0, 23, 21, 22, 0),
    gsSP2Triangles(24, 23, 22, 0, 25, 24, 22, 0),
    gsSP2Triangles(25, 22, 26, 0, 25, 26, 27, 0),
    gsSP2Triangles(28, 29, 30, 0, 28, 30, 31, 0),
    gsSPVertex(&D_84B12498[324], 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(4, 0, 3, 0, 4, 3, 5, 0),
    gsSP2Triangles(6, 7, 8, 0, 6, 8, 9, 0),
    gsSP2Triangles(6, 9, 10, 0, 11, 12, 13, 0),
    gsSP2Triangles(11, 13, 14, 0, 15, 11, 14, 0),
    gsSP2Triangles(15, 14, 16, 0, 15, 16, 17, 0),
    gsSP2Triangles(15, 17, 18, 0, 19, 15, 18, 0),
    gsSP2Triangles(20, 19, 21, 0, 22, 23, 1, 0),
    gsSP2Triangles(22, 1, 24, 0, 25, 20, 26, 0),
    gsSP2Triangles(25, 26, 27, 0, 28, 25, 27, 0),
    gsSP2Triangles(29, 28, 27, 0, 30, 29, 27, 0),
    gsSP2Triangles(30, 27, 31, 0, 10, 30, 31, 0),
    gsSP2Triangles(10, 31, 6, 0, 21, 26, 20, 0),
    gsSPVertex(&D_84B12498[356], 6, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B13FC8[] = {
    0x17000000, 0x0000016A, 0x00000000, 0x00000000, D_84B12498,
    0x05000000, 0x22010000, 0x00000000, 0x08000000, BattlePrepRoster_ScrollVersusLogoShine,
    0x00000000, 0x1B000000, 0x00000000, 0x0000001B, 0x00040000,
    0x05000000, 0x22010000, D_84B13C40, 0x06000000, 0x1B000000,
    0x00000000, 0x0000FFD2, 0x00010000, 0x05000000, 0x22010000,
    D_84B13B38, 0x06000000, 0x06000000, 0x01000000, 0x00000000,
};
static Vtx D_84B14040[] = {
    VTX(-32, 48, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-32, 16, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, 16, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(32, 48, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, 16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-32, -16, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -16, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(32, 16, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, -16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-32, -48, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -48, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(32, -16, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B14100[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineMode(G_CC_DECALRGB, G_CC_PASS2),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B14040, 12, 0),
    gsDPLoadTextureBlock(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureBlock(0x0F001000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsDPLoadTextureBlock(0x0F002000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B141E8[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B14040,
    0x05000000, 0x1B000000, 0x00000000, 0x00000000, 0xFFD00000,
    0x05000000, 0x22010000, 0x00000000, 0x08000000, BattlePrepRoster_DrawTrainerCard,
    D_84B14100, 0x06000000, 0x06000000, 0x01000000,
};
static u32 D_84B14234[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B14040,
    0x05000000, 0x1B000000, 0x00000000, 0x00000000, 0x00300000,
    0x05000000, 0x22010000, 0x00000000, 0x08000000, BattlePrepRoster_DrawTrainerCard,
    D_84B14100, 0x06000000, 0x06000000, 0x01000000,
};
static Vtx D_84B14280[] = {
    VTX(-32, 24, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-32, 0, 0, 0, 768, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, 0, 0, 2048, 768, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(32, 24, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, 0, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-32, -24, 0, 0, 768, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -24, 0, 2048, 768, 0xFF, 0xFF, 0xFF, 0xFF), VTX(32, 0, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Vtx D_84B14300[] = {
    VTX(-30, 22, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(-30, 7, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 7, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(0, 22, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 22, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),    VTX(0, 7, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(30, 7, 0, 4096, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(30, 22, 0, 4096, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Vtx D_84B14380[] = {
    VTX(-30, 7, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-30, -7, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, -7, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(0, 7, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 7, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(0, -7, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(30, -7, 0, 4096, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(30, 7, 0, 4096, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Vtx D_84B14400[] = {
    VTX(-30, -7, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-30, -22, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, -22, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(0, -7, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, -7, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(0, -22, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(30, -22, 0, 4096, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(30, -7, 0, 4096, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B14480[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_PASS2),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B14280, 8, 0),
    gsDPLoadTextureBlock(0x0E000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 24, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureBlock(0x0E000C00, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 24, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSPVertex(D_84B14300, 8, 0),
    gsDPLoadTextureTile(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 0, 0, 63, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureTile(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 64, 0, 127, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSPVertex(D_84B14380, 8, 0),
    gsDPLoadTextureTile(0x0F002000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 0, 0, 63, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureTile(0x0F002000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 64, 0, 127, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSPVertex(D_84B14400, 8, 0),
    gsDPLoadTextureTile(0x0F004000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 0, 0, 63, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureTile(0x0F004000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 0, 64, 0, 127, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B146C0[] = {
    0x17000000,
    0x00000000,
    0x00000000,
    0x00000000,
    D_84B14280,
    0x05000000,
    0x1B000000,
    0x00000000,
    0x00000000,
    0xFFE80000,
    0x05000000,
    0x22050000,
    0x00000000,
    0x08000000,
    BattlePrepRoster_DrawIconColumnSingle,
    0x00000000,
    0x22050000,
    D_84B14480,
    0x06000000,
    0x1B000000,
    0x00000000,
    0x00000000,
    0xFFB80000,
    0x05000000,
    0x22050000,
    0x00000000,
    0x08000000,
    BattlePrepRoster_DrawIconColumnSingle,
    0x00000001,
    0x22050000,
    D_84B14480,
    0x25000000,
    0x06000000,
    0x06000000,
    0x01000000,
};
static u32 D_84B1474C[] = {
    0x17000000,
    0x00000000,
    0x00000000,
    0x00000000,
    D_84B14280,
    0x05000000,
    0x1B000000,
    0x00000000,
    0x00000000,
    0x00480000,
    0x05000000,
    0x22050000,
    0x00000000,
    0x08000000,
    BattlePrepRoster_DrawIconColumnSingle,
    0x00000000,
    0x22050000,
    D_84B14480,
    0x06000000,
    0x1B000000,
    0x00000000,
    0x00000000,
    0x00180000,
    0x05000000,
    0x22050000,
    0x00000000,
    0x08000000,
    BattlePrepRoster_DrawIconColumnSingle,
    0x00000001,
    0x22050000,
    D_84B14480,
    0x25000000,
    0x06000000,
    0x06000000,
    0x01000000,
};
static Vtx D_84B147D8[] = {
    VTX(-24, 16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-24, -16, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(24, -16, 0, 1536, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(24, 16, 0, 1536, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Vtx D_84B14818[] = {
    VTX(-22, 14, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(-22, 0, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 0, 0, 1536, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(0, 14, 0, 1536, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 14, 0, 1536, 0, 0xFF, 0xFF, 0xFF, 0xFF),    VTX(0, 0, 0, 1536, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(22, 0, 0, 3072, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(22, 14, 0, 3072, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Vtx D_84B14898[] = {
    VTX(-22, 0, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),        VTX(-22, -14, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, -14, 0, 1536, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(0, 0, 0, 1536, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 0, 0, 1536, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(0, -14, 0, 1536, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(22, -14, 0, 3072, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(22, 0, 0, 3072, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx D_84B14918[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_PASS2),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_84B147D8, 4, 0),
    gsDPLoadTextureBlock(0x0E000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 48, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPVertex(D_84B14818, 8, 0),
    gsDPLoadTextureTile(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 96, 0, 0, 0, 47, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureTile(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 96, 0, 48, 0, 95, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSPVertex(D_84B14898, 8, 0),
    gsDPLoadTextureTile(0x0F001800, G_IM_FMT_RGBA, G_IM_SIZ_16b, 96, 0, 0, 0, 47, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureTile(0x0F001800, G_IM_FMT_RGBA, G_IM_SIZ_16b, 96, 0, 48, 0, 95, 31, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSPEndDisplayList(),
};
static u32 D_84B14A90[] = {
    0x17000000,
    0x00000000,
    0x00000000,
    0x00000000,
    D_84B147D8,
    0x05000000,
    0x1B000000,
    0x00000000,
    0x00000000,
    0xFFF00000,
    0x05000000,
    0x22050000,
    0x00000000,
    0x08000000,
    BattlePrepRoster_DrawIconColumnDouble,
    0x00000000,
    0x22050000,
    D_84B14918,
    0x06000000,
    0x1B000000,
    0x00000000,
    0x00000000,
    0xFFD00000,
    0x05000000,
    0x22050000,
    0x00000000,
    0x08000000,
    BattlePrepRoster_DrawIconColumnDouble,
    0x00000001,
    0x22050000,
    D_84B14918,
    0x06000000,
    0x1B000000,
    0x00000000,
    0x00000000,
    0xFFB00000,
    0x05000000,
    0x22050000,
    0x00000000,
    0x08000000,
    BattlePrepRoster_DrawIconColumnDouble,
    0x00000002,
    0x22050000,
    D_84B14918,
    0x25000000,
    0x06000000,
    0x06000000,
    0x01000000,
};
static u32 D_84B14B50[] = {
    0x17000000,
    0x00000000,
    0x00000000,
    0x00000000,
    D_84B147D8,
    0x05000000,
    0x1B000000,
    0x00000000,
    0x00000000,
    0x00500000,
    0x05000000,
    0x22050000,
    0x00000000,
    0x08000000,
    BattlePrepRoster_DrawIconColumnDouble,
    0x00000000,
    0x22050000,
    D_84B14918,
    0x06000000,
    0x1B000000,
    0x00000000,
    0x00000000,
    0x00300000,
    0x05000000,
    0x22050000,
    0x00000000,
    0x08000000,
    BattlePrepRoster_DrawIconColumnDouble,
    0x00000001,
    0x22050000,
    D_84B14918,
    0x06000000,
    0x1B000000,
    0x00000000,
    0x00000000,
    0x00100000,
    0x05000000,
    0x22050000,
    0x00000000,
    0x08000000,
    BattlePrepRoster_DrawIconColumnDouble,
    0x00000002,
    0x22050000,
    D_84B14918,
    0x25000000,
    0x06000000,
    0x06000000,
    0x01000000,
};

/**
 * Flips the versus panels into place about X: each frame through frame `duration` (20 NTSC / 16 PAL), the top row
 * (display objects 0-4, side 0) turns by -rotation_step and the bottom row (5-9, side 1) by +rotation_step, exactly
 * undoing the start tilt of 0x5400 (0x53F0 PAL) set in BattlePrepRoster_InitDisplayObjects. Plays sound 0x01100006
 * on frame 0. Returns 0 while flipping, 1 when done.
 */
s32 BattlePrepRoster_FlipInPanelsStep(BattlePrepRoster* roster, s16 frame) {
    s16 i;
    s16 duration;
    s16 rotation_step;

    if (osTvType == OS_TV_PAL) {
        duration = 0x10;
        rotation_step = 0x4F0;
    } else {
        duration = 0x14;
        rotation_step = 0x400;
    }

    if (frame == 0) {
        Audio_PlayCategory11SoundCommand(0x01100006, 0, 0);
    }

    if (duration >= frame) {
        for (i = 0; i < 5; i++) {
            roster->displayObjects[i].rotation.x -= rotation_step;
        }

        for (i = 5; i < 10; i++) {
            roster->displayObjects[i].rotation.x += rotation_step;
        }

        return 0;
    }

    return 1;
}

/**
 * Bounces the camera after the V/S logo lands: sets the camera target Y to sin(frame * 90 degrees) * ((16 - frame)
 * / 12)^2, a damped oscillation with a 4-frame period that dies out at frame 16, and recomputes the eye 289 units
 * from the target.
 */
void BattlePrepRoster_ShakeCamera(BattlePrepRoster* roster, s16 frame) {
    roster->camera->view.targetPosition.y = SINS(frame << 0xE) * SQ((0x10 - frame) / 12.0f);
    Camera_ComputeEyeFromAngles(&roster->camera->view.targetPosition, &roster->camera->view.eyePosition, 289.0f, 0, 0);
}

/**
 * NTSC V/S logo animation. Frames 0-19 move the logo (display object 10) from z = -80 to z = -650, so it flies away
 * from the camera into the screen; frames 20-35 shake the camera with BattlePrepRoster_ShakeCamera. Plays sound
 * 0x01100007 on frame 0 and 0x01100008 on frame 21. Returns 0 while running, 1 when done.
 */
s32 BattlePrepRoster_VersusLogoStepNTSC(BattlePrepRoster* roster, s16 frame) {
    if (frame == 0) {
        Audio_PlayCategory11SoundCommand(0x01100007, 0, 0);
    }

    if (frame == 0x15) {
        Audio_PlayCategory11SoundCommand(0x01100008, 0, 0);
    }

    if (frame < 0x14) {
        roster->displayObjects[10].position.z = -80.0f - (frame * 30.0f);
        return 0;
    }

    if (frame < 0x24) {
        BattlePrepRoster_ShakeCamera(roster, frame - 0x13);
        return 0;
    }
    return 1;
}

/**
 * PAL timing of BattlePrepRoster_VersusLogoStepNTSC: frames 0-15 move the logo from z = -80 to z = -650 (38 units
 * per frame), frames 16-31 shake the camera. Plays sound 0x01100007 on frame 0 and 0x01100008 on frame 17. Returns
 * 0 while running, 1 when done.
 */
s32 BattlePrepRoster_VersusLogoStepPAL(BattlePrepRoster* roster, s16 frame) {
    if (frame == 0) {
        Audio_PlayCategory11SoundCommand(0x01100007, 0, 0);
    }

    if (frame == 0x11) {
        Audio_PlayCategory11SoundCommand(0x01100008, 0, 0);
    }

    if (frame < 0x10) {
        roster->displayObjects[10].position.z = -80.0f - (frame * 38.0f);
        return 0;
    }

    if (frame < 0x20) {
        BattlePrepRoster_ShakeCamera(roster, frame - 0xF);
        return 0;
    }

    return 1;
}

/**
 * State 3 tick of the versus intro: runs BattlePrepRoster_FlipInPanelsStep from frame 0 and the TV-specific V/S
 * logo step from frame 16 (13 on PAL), so the two overlap. Returns to state 0 once both report done, and advances
 * timer on every call.
 */
void BattlePrepRoster_UpdateAnimationStep(BattlePrepRoster* roster) {
    s32 is_done = 1;

    if (osTvType == OS_TV_PAL) {
        if (roster->timer >= 0) {
            is_done = BattlePrepRoster_FlipInPanelsStep(roster, roster->timer) & 1;
        }

        if (roster->timer >= 0xD) {
            is_done &= BattlePrepRoster_VersusLogoStepPAL(roster, roster->timer - 0xD);
        }
    } else {
        if (roster->timer >= 0) {
            is_done = BattlePrepRoster_FlipInPanelsStep(roster, roster->timer) & 1;
        }

        if (roster->timer >= 0x10) {
            is_done &= BattlePrepRoster_VersusLogoStepNTSC(roster, roster->timer - 0x10);
        }
    }

    if (is_done != 0) {
        roster->state = 0;
    }
    roster->timer++;
}

/**
 * State 1 tick: renders one PokeIcon slot per frame. Once at least 30 frames have passed and every slot is idle,
 * resets timer and sets state 2, so BattlePrepRoster_InitDisplayObjects can build the scene.
 */
void BattlePrepRoster_UpdateLoadingStep(BattlePrepRoster* roster) {
    PokeIcon_ProcessNextSlot(roster->iconRenderList, 0);

    roster->timer++;
    if ((roster->timer >= 0x1E) && (PokeIcon_AreSlotsIdle(roster->iconRenderList) != 0)) {
        roster->timer = 0;
        roster->state = 2;
    }
}

/**
 * Per-frame update for the versus screen: state 1 loads the icons, state 3 runs the intro animation; states 0 and 2
 * do nothing.
 */
void BattlePrepRoster_Update(BattlePrepRoster* roster) {
    switch (roster->state) {
        case 1:
            BattlePrepRoster_UpdateLoadingStep(roster);
            break;

        case 3:
            BattlePrepRoster_UpdateAnimationStep(roster);
            break;
    }
}

/**
 * Attaches a 0x80x0x60 depth buffer to the icon render list and queues a species icon for each Pokémon in both
 * trainers' rosters (rosterInfo->numberOfPokemonCanBePicked entries): side 0 into slots 0-5, side 1 into slots
 * 6-11.
 */
void BattlePrepRoster_QueueIconsSingle(BattlePrepRoster* roster) {
    s32 i;
    s32 slot;
    ModelLoadContext* load_context;

    load_context = PokeIcon_AllocFramebuffers(5);
    PokeIcon_AttachDepthBuffer(roster->iconRenderList, load_context, GfxImage_Allocate(0, 2, 0x80, 0x60, 1));

    for (slot = 0, i = 0; i < gSessionContext.battleSides[0].tagTeamMember[0]->rosterInfo->numberOfPokemonCanBePicked;
         slot++, i++) {
        func_8001A324(roster->iconRenderList, slot,
                      gSessionContext.battleSides[0].tagTeamMember[0]->rosterInfo->battleMon[i].species.id, 0);
    }

    for (slot = 6, i = 0; i < gSessionContext.battleSides[1].tagTeamMember[0]->rosterInfo->numberOfPokemonCanBePicked;
         slot++, i++) {
        func_8001A324(roster->iconRenderList, slot,
                      gSessionContext.battleSides[1].tagTeamMember[0]->rosterInfo->battleMon[i].species.id, 0);
    }
}

/**
 * Tag-team counterpart of BattlePrepRoster_QueueIconsSingle with a 0x60x0x40 depth buffer: side 0's first and
 * second members go into slots 0-5 and 6-11, side 1's into 12-17 and 18-23. Absent second members are skipped.
 */
void BattlePrepRoster_QueueIconsDouble(BattlePrepRoster* roster) {
    s32 i;
    s32 slot;
    ModelLoadContext* load_context = PokeIcon_AllocFramebuffers(5);

    PokeIcon_AttachDepthBuffer(roster->iconRenderList, load_context, GfxImage_Allocate(0, 2, 0x60, 0x40, 1));

    for (slot = 0, i = 0; i < gSessionContext.battleSides[0].tagTeamMember[0]->rosterInfo->numberOfPokemonCanBePicked;
         slot++, i++) {
        func_8001A324(roster->iconRenderList, slot,
                      gSessionContext.battleSides[0].tagTeamMember[0]->rosterInfo->battleMon[i].species.id, 0);
    }

    if (gSessionContext.battleSides[0].tagTeamMember[1] != NULL) {
        for (i = 0, slot = 6; i < gSessionContext.battleSides[0].tagTeamMember[1]->rosterInfo->numberOfPokemonCanBePicked;
             slot++, i++) {
            func_8001A324(roster->iconRenderList, slot,
                          gSessionContext.battleSides[0].tagTeamMember[1]->rosterInfo->battleMon[i].species.id, 0);
        }
    }

    for (i = 0, slot = 12; i < gSessionContext.battleSides[1].tagTeamMember[0]->rosterInfo->numberOfPokemonCanBePicked;
         slot++, i++) {
        func_8001A324(roster->iconRenderList, slot,
                      gSessionContext.battleSides[1].tagTeamMember[0]->rosterInfo->battleMon[i].species.id, 0);
    }

    if (gSessionContext.battleSides[1].tagTeamMember[1] != NULL) {
        for (slot = 18, i = 0;
             i < gSessionContext.battleSides[1].tagTeamMember[1]->rosterInfo->numberOfPokemonCanBePicked; slot++, i++) {
            func_8001A324(roster->iconRenderList, slot,
                          gSessionContext.battleSides[1].tagTeamMember[1]->rosterInfo->battleMon[i].species.id, 0);
        }
    }
}

/**
 * Opens the PokeIcon model archives, loads the particle data library fragment, queues the roster icons for the
 * current layout, and draws both trainer cards (side 0 on the D_3000000 background, side 1 on D_3003000, each with
 * the trainer's portrait from portrait_archive). Sets state 1 (loading).
 */
void BattlePrepRoster_Load(BattlePrepRoster* roster, BinArchive* portrait_archive) {
    u8* side0_portrait = BinArchive_GetFile(portrait_archive, gSessionContext.battleSides[0].tagTeamMember[0]->rosterInfo->unk_003);
    u8* side1_portrait = BinArchive_GetFile(portrait_archive, gSessionContext.battleSides[1].tagTeamMember[0]->rosterInfo->unk_003);

    PokeIcon_OpenModelArchives();
    FRAGMENT_LOAD(FRAGMENT_MODEL_CALLBACK_LIBRARY);

    if (roster->isDoubleLayout == 1) {
        BattlePrepRoster_QueueIconsDouble(roster);
    } else {
        BattlePrepRoster_QueueIconsSingle(roster);
    }

    BattlePrep_RenderTrainerPortrait(roster->side0CardImage, 0, side0_portrait, D_3000000);
    BattlePrep_RenderTrainerPortrait(roster->side1CardImage, 1, side1_portrait, D_3003000);
    roster->state = 1;
}

/**
 * Single layout: places side 0's card (display object 0) and icon columns (2-4) along y = +96, and side 1's icon
 * columns (5-7) and card (9) along y = -96, 64 units apart at z = -289. Hides the unused column nodes 1 and 8.
 */
void BattlePrepRoster_PositionPanelsSingle(BattlePrepRoster* roster) {
    Vec3f_SetComponentsDuplicate(&roster->displayObjects[0].position, -96.0f, 96.0f, -289.0f);
    Vec3f_SetComponentsDuplicate(&roster->displayObjects[2].position, -32.0f, 96.0f, -289.0f);
    Vec3f_SetComponentsDuplicate(&roster->displayObjects[3].position, 32.0f, 96.0f, -289.0f);
    Vec3f_SetComponentsDuplicate(&roster->displayObjects[4].position, 96.0f, 96.0f, -289.0f);
    Vec3f_SetComponentsDuplicate(&roster->displayObjects[5].position, -96.0f, -96.0f, -289.0f);
    Vec3f_SetComponentsDuplicate(&roster->displayObjects[6].position, -32.0f, -96.0f, -289.0f);
    Vec3f_SetComponentsDuplicate(&roster->displayObjects[7].position, 32.0f, -96.0f, -289.0f);
    Vec3f_SetComponentsDuplicate(&roster->displayObjects[9].position, 96.0f, -96.0f, -289.0f);

    roster->displayObjects[1].node.flags &= ~1;
    roster->displayObjects[8].node.flags &= ~1;
}

/**
 * Tag-team layout: places side 0's card (display object 0, x = -96) and four 48-unit icon columns (1-4) along y =
 * +96, and side 1's columns (5-8) and card (9, x = 96) along y = -96, at z = -289.
 */
void BattlePrepRoster_PositionPanelsDouble(BattlePrepRoster* roster) {
    Vec3f_SetComponentsDuplicate(&roster->displayObjects[0].position, -96.0f, 96.0f, -289.0f);
    Vec3f_SetComponentsDuplicate(&roster->displayObjects[1].position, -40.0f, 96.0f, -289.0f);
    Vec3f_SetComponentsDuplicate(&roster->displayObjects[2].position, 8.0f, 96.0f, -289.0f);
    Vec3f_SetComponentsDuplicate(&roster->displayObjects[3].position, 56.0f, 96.0f, -289.0f);
    Vec3f_SetComponentsDuplicate(&roster->displayObjects[4].position, 104.0f, 96.0f, -289.0f);
    Vec3f_SetComponentsDuplicate(&roster->displayObjects[5].position, -104.0f, -96.0f, -289.0f);
    Vec3f_SetComponentsDuplicate(&roster->displayObjects[6].position, -56.0f, -96.0f, -289.0f);
    Vec3f_SetComponentsDuplicate(&roster->displayObjects[7].position, -8.0f, -96.0f, -289.0f);
    Vec3f_SetComponentsDuplicate(&roster->displayObjects[8].position, 40.0f, -96.0f, -289.0f);
    Vec3f_SetComponentsDuplicate(&roster->displayObjects[9].position, 96.0f, -96.0f, -289.0f);
}

/**
 * Once loading has reached state 2: stores the display objects and points their callbackArg at the roster, tilts
 * the top row to start_rotation and the bottom row to -start_rotation about X for
 * BattlePrepRoster_FlipInPanelsStep, binds objects 0-9 to the trainer card and icon column geo layouts and object
 * 10 to the V/S logo (parked at z = 1000 until the logo step moves it), positions the panels, and enters state 3.
 * Returns 1 once in state 3.
 */
s32 BattlePrepRoster_InitDisplayObjects(BattlePrepRoster* roster, DisplayObject* display_objects) {
    s32 i;
    s16 start_rotation;

    if (roster->state == 2) {
        roster->state = 3;
        roster->displayObjects = display_objects;
        roster->timer = 0;
        if (osTvType == OS_TV_PAL) {
            start_rotation = 0x53F0;
        } else {
            start_rotation = 0x5400;
        }

        for (i = 0; i < 11; i++) {
            roster->displayObjects[i].node.callbackArg = roster;
        }

        for (i = 0; i < 5; i++) {
            roster->displayObjects[i].rotation.x = start_rotation;
        }

        // clang-format off
        for (i = 5; i < 10; i++) { roster->displayObjects[i].rotation.x = -start_rotation; }
        // clang-format on

        Model_InitDisplayObject(&roster->displayObjects[0], 1, 0, roster->side0CardGeo);
        Model_InitDisplayObject(&roster->displayObjects[1], 1, 0, roster->side0IconGeo);
        Model_InitDisplayObject(&roster->displayObjects[2], 1, 0, roster->side0IconGeo);
        Model_InitDisplayObject(&roster->displayObjects[3], 1, 0, roster->side0IconGeo);
        Model_InitDisplayObject(&roster->displayObjects[4], 1, 0, roster->side0IconGeo);
        Model_InitDisplayObject(&roster->displayObjects[5], 1, 0, roster->side1IconGeo);
        Model_InitDisplayObject(&roster->displayObjects[6], 1, 0, roster->side1IconGeo);
        Model_InitDisplayObject(&roster->displayObjects[7], 1, 0, roster->side1IconGeo);
        Model_InitDisplayObject(&roster->displayObjects[8], 1, 0, roster->side1IconGeo);
        Model_InitDisplayObject(&roster->displayObjects[9], 1, 0, roster->side1CardGeo);
        Model_InitDisplayObject(&roster->displayObjects[10], 3, 0, roster->versusLogoGeo);
        Vec3f_SetComponentsDuplicate(&roster->displayObjects[10].position, 0.0f, 0.0f, 1000.0f);

        if (roster->isDoubleLayout == 1) {
            BattlePrepRoster_PositionPanelsDouble(roster);
        } else {
            BattlePrepRoster_PositionPanelsSingle(roster);
        }
    }

    return roster->state == 3;
}

/**
 * Initializes the versus screen: stores the camera, allocates the two 64x96 trainer card images, selects the
 * tag-team layout when rule category 0 has a 2-member tag team on either side, creates the PokeIcon render list (24
 * slots of 0x60x0x40, or 12 of 0x80x0x60), and builds the icon column, trainer card and V/S logo geo layouts.
 */
void BattlePrepRoster_Init(BattlePrepRoster* roster, CameraNode* camera) {
    MemoryBlock* geo_pool;

    roster->camera = camera;
    roster->state = 0;
    roster->timer = 0;
    roster->isDoubleLayout = 0;
    roster->side0CardImage = GfxImage_Allocate(0, 2, 0x40, 0x60, 0);
    roster->side1CardImage = GfxImage_Allocate(0, 2, 0x40, 0x60, 0);

    if ((gSessionContext.ruleCategory == 0) &&
        ((gSessionContext.battleSides[0].tagTeamSize == 2) || (gSessionContext.battleSides[1].tagTeamSize == 2))) {
        roster->isDoubleLayout = 1;
    }
    if (roster->isDoubleLayout == 1) {
        roster->iconRenderList = PokeIcon_CreateRenderList(NULL, 0x18, 0x60, 0x40);
    } else {
        roster->iconRenderList = PokeIcon_CreateRenderList(NULL, 0xC, 0x80, 0x60);
    }
    PokeIcon_SetNameTable(roster->iconRenderList, sRosterSpeciesIcons);

    geo_pool = MainPool_AllocState(main_pool_get_available(), 0);
    if (roster->isDoubleLayout == 1) {
        roster->side0IconGeo = process_geo_layout(geo_pool, D_84B14A90);
        roster->side1IconGeo = process_geo_layout(geo_pool, D_84B14B50);
    } else {
        roster->side0IconGeo = process_geo_layout(geo_pool, D_84B146C0);
        roster->side1IconGeo = process_geo_layout(geo_pool, D_84B1474C);
    }

    roster->side0CardGeo = process_geo_layout(geo_pool, D_84B141E8);
    roster->side1CardGeo = process_geo_layout(geo_pool, D_84B14234);
    roster->versusLogoGeo = process_geo_layout(geo_pool, D_84B13FC8);

    MainPool_FinalizeAllocation(geo_pool);
}
