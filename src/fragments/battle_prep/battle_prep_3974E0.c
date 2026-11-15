#include "battle_prep.h"
#include "src/graphics_textures.h"
#include "src/gfx_buffer.h"
#include "src/math_util.h"

/**
 * Returns a random starfield x or y coordinate in [-2000, 2000).
 */
s16 BattlePrep_RandomStarOffset(void) {
    f32 offset = MathUtil_Random_ZeroOne();

    if (offset >= 0.5f) {
        offset = (offset - 0.5f) * 4000.0f;
    } else {
        offset = (offset - 0.5f) * 4000.0f;
    }
    return offset;
}

/**
 * Draws the background starfield: one 16x16 pale-yellow (vertex color 0xFFFF96FF) quad textured with the
 * star sprite D_3024A00 at each of the 400 star positions. Called from BattlePrep_StarfieldGeoCallback.
 */
void BattlePrep_DrawStarfield(StarfieldStar* stars) {
    Vtx* vertices;
    s32 i;
    Vec3s* star_position;

    gDPPipeSync(gDisplayListHead++);
    gSPClearGeometryMode(gDisplayListHead++, G_CULL_BACK);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGBA, G_CC_PASS2);
    gSPTexture(gDisplayListHead++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
    gDPLoadTextureBlock(gDisplayListHead++, D_3024A00, G_IM_FMT_IA, G_IM_SIZ_16b, 32, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    for (i = 0; i < 400; i++) {
        vertices = Gfx_AllocDisplayList(sizeof(Vtx) * 4);
        if (vertices != NULL) {
            star_position = &(i + ((void)0, stars))->position;
            Gfx_SetVertexAttributes(&vertices[0], star_position->x - 8, star_position->y + 8, star_position->z, 0, 0, 0xFF, 0xFF,
                                    0x96, 0xFF);
            Gfx_SetVertexAttributes(&vertices[1], star_position->x - 8, star_position->y - 8, star_position->z, 0, 0x400, 0xFF,
                                    0xFF, 0x96, 0xFF);
            Gfx_SetVertexAttributes(&vertices[2], star_position->x + 8, star_position->y - 8, star_position->z, 0x400, 0x400,
                                    0xFF, 0xFF, 0x96, 0xFF);
            Gfx_SetVertexAttributes(&vertices[3], star_position->x + 8, star_position->y + 8, star_position->z, 0x400, 0, 0xFF,
                                    0xFF, 0x96, 0xFF);

            gSPVertex(gDisplayListHead++, vertices, 4, 0);
            gSP2Triangles(gDisplayListHead++, 0, 1, 2, 0, 0, 2, 3, 0);
        }
    }

    gSPTexture(gDisplayListHead++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF);
    gSPSetGeometryMode(gDisplayListHead++, G_CULL_BACK);
}

/**
 * Streams the starfield toward the camera: moves every star 100 units forward in z each frame; a star
 * that passes z = 0 respawns far away (z in [-5500, -5000]) at a random x/y outside radius 160, so the
 * center of the screen stays clear.
 */
void BattlePrep_UpdateStarfield(StarfieldStar* stars) {
    s32 star_x;
    s32 star_y;
    s32 i;

    for (i = 0; i < 400; i++) {
        stars[i].position.z += 0x64;
        if (stars[i].position.z > 0) {
            do {
                star_x = BattlePrep_RandomStarOffset();
                star_y = BattlePrep_RandomStarOffset();
            } while ((SQ(star_x) + SQ(star_y)) < 0x6400);

            stars[i].position.x = star_x;
            stars[i].position.y = star_y;
            stars[i].position.z = (-MathUtil_Random_ZeroOne() * 500.0f) - 5000.0f;
        }
    }
}

/**
 * Scatters the 400 stars through the whole depth range (z in [-5100, -100]) at random x/y outside
 * radius 160, then sets star 0's unk_00 to 1. The random 20-30 value is stored one star ahead (i + 1),
 * so the last iteration writes past the array into pad_D_84B1A590.
 */
void BattlePrep_InitStarfield(StarfieldStar* stars) {
    s32 star_x;
    s32 star_y;
    s32 i;

    for (i = 0; i < 400; i++) {
        do {
            star_x = BattlePrep_RandomStarOffset();
            star_y = BattlePrep_RandomStarOffset();
        } while ((SQ(star_x) + SQ(star_y)) < 0x6400);

        stars[i].position.x = star_x;
        stars[i].position.y = star_y;
        stars[i].position.z = (-MathUtil_Random_ZeroOne() * 5000.0f) - 100.0f;
        stars[i + 1].unk_00 = (MathUtil_Random_ZeroOne() * 10.0f) + 20.0f;
    }
    *(s8*)&stars->unk_00 = 1;
}
