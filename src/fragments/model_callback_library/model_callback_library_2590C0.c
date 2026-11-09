#include "model_callback_library.h"
#include "src/gfx_buffer.h"
#include "src/geo_render.h"

typedef struct AnimationFrameRange {
    /* 0x00 */ u16 animationId;
    /* 0x02 */ u16 firstFrame;
    /* 0x04 */ u16 lastFrame;
} AnimationFrameRange; // size = 0x6

typedef struct AnimationFlagTarget {
    /* 0x00 */ char unk00[0x1];
    /* 0x01 */ u8 flags;
} AnimationFlagTarget; // size = 0x2

typedef struct AnimationRangeFlagState {
    /* 0x00 */ char unk00[0x4];
    /* 0x04 */ AnimationFlagTarget* target;
    /* 0x08 */ char unk08[0xC];
    /* 0x14 */ AnimationFrameRange* range;
} AnimationRangeFlagState; // size = 0x18

/**
 * Geo callback (jump-table slot 0x1E8). Sets bit 0x1 of target->flags while the current display
 * object plays range->animationId between range->firstFrame and range->lastFrame, and clears it otherwise.
 * Referenced by the Sandshrew model.
 */
void DisplayList_SetFlagInRange(s32 arg0, AnimationRangeFlagState* arg1) {
    s32 temp_a2;
    s32 var_v1;
    AnimationFrameRange* temp_v0;
    AnimationFlagTarget* temp_v0_2;

    if (arg0 == 2) {
        temp_v0 = arg1->range;
        var_v1 = 0;
        if (D_8006F09C->transformAnim.animationId == temp_v0->animationId) {
            temp_a2 = ((s32)D_8006F09C->transformAnim.frameFixed >> 0x10) & 0xFFFF;
            if ((temp_a2 >= (s32)temp_v0->firstFrame) && ((s32)temp_v0->lastFrame >= temp_a2)) {
                var_v1 = 1;
            }
        }
        if (var_v1 == 1) {
            temp_v0_2 = arg1->target;
            temp_v0_2->flags |= 1;
            return;
        }
        temp_v0_2 = arg1->target;
        temp_v0_2->flags &= 0xFFFE;
    }
}

/**
 * Geo callback (jump-table slot 0x1F0). The inverse of DisplayList_SetFlagInRange: bit 0x1 is
 * set outside the frame range and cleared inside it. Referenced by the Sandshrew model.
 */
void DisplayList_SetFlagOutOfRange(s32 arg0, AnimationRangeFlagState* arg1) {
    s32 temp_a2;
    s32 var_v1;
    AnimationFrameRange* temp_v0;
    AnimationFlagTarget* temp_v0_2;

    if (arg0 == 2) {
        temp_v0 = arg1->range;
        var_v1 = 1;
        if (D_8006F09C->transformAnim.animationId == temp_v0->animationId) {
            temp_a2 = ((s32)D_8006F09C->transformAnim.frameFixed >> 0x10) & 0xFFFF;
            if ((temp_a2 >= (s32)temp_v0->firstFrame) && ((s32)temp_v0->lastFrame >= temp_a2)) {
                var_v1 = 0;
            }
        }
        if (var_v1 == 1) {
            temp_v0_2 = arg1->target;
            temp_v0_2->flags |= 1;
            return;
        }
        temp_v0_2 = arg1->target;
        temp_v0_2->flags &= 0xFFFE;
    }
}

/**
 * Geo callback (jump-table slot 0x1F8). Enables noise alpha dither and sets a dark primitive
 * color whose alpha is the current display object's effectSlot, plus a fixed environment color.
 */
void DisplayList_InitDitheredPrimColorSegment(s32 arg0, DisplayListState* state) {
    Gfx* gfx;

    if (arg0 == 2) {
        gfx = Gfx_AllocDisplayList(0xA0);
        state->gfx = gfx;
        gDPSetAlphaDither(gfx++, G_AD_NOISE);
        gDPSetPrimColor(gfx++, 0, 0, 10, 10, 10, D_8006F09C->effectSlot & 0xFF);
        gDPSetEnvColor(gfx++, 0x37, 0x14, 0x14, 0xB4);
        gDPSetCombineLERP(gfx++, 0, 0, 0, 0, TEXEL0, 1, PRIMITIVE, 1, 0, 0, 0, 0, COMBINED, 0, ENVIRONMENT, 0);
        gSPEndDisplayList(gfx++);
    }
}
