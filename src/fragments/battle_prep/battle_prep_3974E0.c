#include "battle_prep.h"
#include "src/graphics_textures.h"
#include "src/gfx_buffer.h"
#include "src/math_util.h"

/**
 * Returns a random coordinate in [-2000, 2000).
 */
s16 BattlePrep_RandomStarOffset(void) {
    f32 temp_fv0 = MathUtil_Random_ZeroOne();

    if (temp_fv0 >= 0.5f) {
        temp_fv0 = (temp_fv0 - 0.5f) * 4000.0f;
    } else {
        temp_fv0 = (temp_fv0 - 0.5f) * 4000.0f;
    }
    return temp_fv0;
}

/**
 * Draws 400 16x16 textured quads (texture D_3024A00, vertex color 0xFFFF96FF) at each star position.
 */
void BattlePrep_DrawStarfield(StarfieldStar* arg0) {
    Vtx* temp_v0_12;
    s32 i;
    Vec3s* ptr2;

    gDPPipeSync(gDisplayListHead++);
    gSPClearGeometryMode(gDisplayListHead++, G_CULL_BACK);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGBA, G_CC_PASS2);
    gSPTexture(gDisplayListHead++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
    gDPLoadTextureBlock(gDisplayListHead++, D_3024A00, G_IM_FMT_IA, G_IM_SIZ_16b, 32, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    for (i = 0; i < 400; i++) {
        temp_v0_12 = Gfx_AllocDisplayList(sizeof(Vtx) * 4);
        if (temp_v0_12 != NULL) {
            ptr2 = &(i + ((void)0, arg0))->position;
            Gfx_SetVertexAttributes(&temp_v0_12[0], ptr2->x - 8, ptr2->y + 8, ptr2->z, 0, 0, 0xFF, 0xFF,
                                    0x96, 0xFF);
            Gfx_SetVertexAttributes(&temp_v0_12[1], ptr2->x - 8, ptr2->y - 8, ptr2->z, 0, 0x400, 0xFF,
                                    0xFF, 0x96, 0xFF);
            Gfx_SetVertexAttributes(&temp_v0_12[2], ptr2->x + 8, ptr2->y - 8, ptr2->z, 0x400, 0x400,
                                    0xFF, 0xFF, 0x96, 0xFF);
            Gfx_SetVertexAttributes(&temp_v0_12[3], ptr2->x + 8, ptr2->y + 8, ptr2->z, 0x400, 0, 0xFF,
                                    0xFF, 0x96, 0xFF);

            gSPVertex(gDisplayListHead++, temp_v0_12, 4, 0);
            gSP2Triangles(gDisplayListHead++, 0, 1, 2, 0, 0, 2, 3, 0);
        }
    }

    gSPTexture(gDisplayListHead++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF);
    gSPSetGeometryMode(gDisplayListHead++, G_CULL_BACK);
}

/**
 * Moves every star 100 units forward in z; stars past z = 0 respawn at a random x/y outside radius 160
 * with z in [-5500, -5000].
 */
void BattlePrep_UpdateStarfield(StarfieldStar* arg0) {
    s32 temp_s0;
    s32 temp_v0;
    s32 i;

    for (i = 0; i < 400; i++) {
        arg0[i].position.z += 0x64;
        if (arg0[i].position.z > 0) {
            do {
                temp_s0 = BattlePrep_RandomStarOffset();
                temp_v0 = BattlePrep_RandomStarOffset();
            } while ((SQ(temp_s0) + SQ(temp_v0)) < 0x6400);

            arg0[i].position.x = temp_s0;
            arg0[i].position.y = temp_v0;
            arg0[i].position.z = (-MathUtil_Random_ZeroOne() * 500.0f) - 5000.0f;
        }
    }
}

/**
 * Places 400 stars at random x/y outside radius 160 with z in [-5100, -100] and sets the init flag.
 */
void BattlePrep_InitStarfield(StarfieldStar* arg0) {
    s32 temp_s0;
    s32 temp_v0;
    s32 i;

    for (i = 0; i < 400; i++) {
        do {
            temp_s0 = BattlePrep_RandomStarOffset();
            temp_v0 = BattlePrep_RandomStarOffset();
        } while ((SQ(temp_s0) + SQ(temp_v0)) < 0x6400);

        arg0[i].position.x = temp_s0;
        arg0[i].position.y = temp_v0;
        arg0[i].position.z = (-MathUtil_Random_ZeroOne() * 5000.0f) - 100.0f;
        arg0[i + 1].unk_00 = (MathUtil_Random_ZeroOne() * 10.0f) + 20.0f;
    }
    *(s8*)&arg0->unk_00 = 1;
}
