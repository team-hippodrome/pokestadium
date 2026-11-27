#include "battle_prep.h"
#include "src/geo_render.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/ui_graphics.h"
#include "src/audio_commands_category2.h"
#include "src/gfx_rect.h"
#include "src/matrix.h"
#include "src/memory.h"
#include "src/util.h"

// The opponent ladder's geo layouts and display lists are defined with the banner's in battle_prep_397B10.c.
extern Gfx D_84B113C0[];
extern Gfx D_84B114F8[];
extern u32 D_84B11640[];
extern Vtx D_84B116E0[];
extern Gfx D_84B119A0[];
extern Gfx D_84B11A88[];

s32 OpponentLadder_DrawCupSlot(s32, unk_func_80011B94*);
s32 OpponentLadder_DrawCupHighlight(s32, unk_func_80011B94*);
s32 OpponentLadder_DrawCastleSlot(s32, unk_func_80011B94*);
s32 OpponentLadder_DrawCastleHighlight(s32, unk_func_80011B94*);

static u32 D_84B11BD0[] = {
    0x17000000, 0x00000000, 0x00000000, 0x00000000, D_84B116E0,
    0x05000000, 0x22050000, 0x00000000, 0x08000000, OpponentLadder_DrawCastleSlot,
    D_84B119A0, 0x22050000, 0x00000000, 0x08000000, OpponentLadder_DrawCastleHighlight,
    D_84B11A88, 0x06000000, 0x01000000,
};

/**
 * Geo draw callback (event 5) for one cup ladder slot. Each ladder display object is a column with
 * two slots; the node's callbackArg picks the row (0 top, 1 bottom), giving portrait index
 * row * 4 + column. Binds that portrait to segment 0x0F and draws the olive slot frame with it.
 */
s32 OpponentLadder_DrawCupSlot(s32 event, unk_func_80011B94* display_node) {
    OpponentLadder* ladder;
    s32 column;
    s32 slot;

    if (event == 5) {
        ladder = (OpponentLadder*)D_8006F09C->node.callbackArg;
        column = ((D_8006F09C->effectSlot - ladder->displayObjects->effectSlot) % 8) - 4;
        slot = (display_node->display_node_header.callbackArg * 4) + column;

        gDPPipeSync(gDisplayListHead++);
        gSPSegment(gDisplayListHead++, 0x0F, (u32)ladder->portraits[slot] & 0x1FFFFFFF);
        gSPDisplayList(gDisplayListHead++, D_84B113C0);

        GeoRender_ApplyMaterialState();
    }
    return 0;
}

/**
 * Geo draw callback (event 5): when highlightCurrent is set, draws a pulsing yellow and red frame
 * (8x8 corner and edge pieces) around the cup ladder slot for the current round.
 */
s32 OpponentLadder_DrawCupHighlight(s32 event, unk_func_80011B94* display_node) {
    static s16 D_84B11C18 = 0;

    OpponentLadder* ladder;
    s32 alpha;
    s32 column;
    s32 slot;

    if (event == 5) {
        ladder = (OpponentLadder*)D_8006F09C->node.callbackArg;
        column = ((D_8006F09C->effectSlot - ladder->displayObjects->effectSlot) % 8) - 4;
        slot = column + display_node->display_node_header.callbackArg * 4;

        if ((ladder->highlightCurrent != 0) && ((slot + 1) == gSessionContext.roundOrDemoId)) {
            // clang-format off
            if (D_84B11C18 >= 0) { alpha = (s32)(SINS((s32)(SINS(D_84B11C18) * 16384.0f)) * 127.0f) + 0x7F;
            } else {
                alpha = (s32)(SINS(D_84B11C18) * 127.0f) + 0x7F;
            }
            // clang-format on

            gDPPipeSync(gDisplayListHead++);
            gDPSetPrimColor(gDisplayListHead++, 0, D_8006F09C->lodFraction, 200, 200, 30, alpha); // #C8C81E
            gDPSetEnvColor(gDisplayListHead++, 200, 30, 30, alpha);                               // #C81E1E
            gSPDisplayList(gDisplayListHead++, D_84B114F8);

            GeoRender_ApplyMaterialState();
            D_84B11C18 += 0x800;
        }
    }
    return 0;
}

/**
 * Geo draw callback (event 5) for one Gym Leader Castle ladder slot: binds that slot's rendered
 * card (trainer name and portrait, see OpponentLadder_RenderCastleCard) to segment 0x0F and draws
 * it.
 */
s32 OpponentLadder_DrawCastleSlot(s32 event, unk_func_80011B94* display_node) {
    OpponentLadder* ladder;
    s32 slot;

    if (event == 5) {
        ladder = (OpponentLadder*)D_8006F09C->node.callbackArg;
        slot = ((D_8006F09C->effectSlot - ladder->displayObjects->effectSlot) % 8) - 4;

        gDPPipeSync(gDisplayListHead++);

        gSPSegment(gDisplayListHead++, 0x0F, (u32)ladder->castleCards[slot]->img_p & 0x1FFFFFFF);
        gSPDisplayList(gDisplayListHead++, display_node->display_node_header.callbackArg);

        GeoRender_ApplyMaterialState();
    }
    return 0;
}

/**
 * Geo draw callback (event 5): when highlightCurrent is set, draws the pulsing yellow and red frame
 * around the Gym Leader Castle ladder slot for the current round.
 */
s32 OpponentLadder_DrawCastleHighlight(s32 event, unk_func_80011B94* display_node) {
    static s16 D_84B11C1C = 0;

    OpponentLadder* ladder;
    s32 alpha;
    s32 slot;

    if (event == 5) {
        ladder = (OpponentLadder*)D_8006F09C->node.callbackArg;
        slot = ((D_8006F09C->effectSlot - ladder->displayObjects->effectSlot) % 8);
        slot -= 4;

        if ((ladder->highlightCurrent != 0) && ((slot + 1) == gSessionContext.roundOrDemoId)) {
            // clang-format off
            if (D_84B11C1C >= 0) {                alpha = (s32)(SINS((s32)(SINS(D_84B11C1C) * 16384.0f)) * 127.0f)+ 0x7F;
            } else {
                alpha = (s32)(SINS(D_84B11C1C) * 127.0f) + 0x7F;
            }
            // clang-format on
            gDPPipeSync(gDisplayListHead++);

            gDPSetPrimColor(gDisplayListHead++, 0, D_8006F09C->lodFraction, 200, 200, 30, alpha); // #C8C81E
            gDPSetEnvColor(gDisplayListHead++, 200, 30, 30, alpha);                               // #C81E1E
            gSPDisplayList(gDisplayListHead++, display_node->display_node_header.callbackArg);

            GeoRender_ApplyMaterialState();
            D_84B11C1C += 0x800;
        }
    }
    return 0;
}

/**
 * Draws the defeated mark into a 64x64 portrait texture: D_3017F00, a 64x64 I4 "X", tinted
 * red-orange with alpha 160.
 */
void OpponentLadder_StampDefeatedMark(s32 portrait) {
    DepthBuffer image;
    u32 mark_texture = Util_ConvertAddrToVirtAddr(&D_3017F00);

    GfxImage_Initialize(&image, 0, 2, 0x40, 0x40, portrait);
    GfxImage_SetRenderTarget(&gDisplayListHead, &image);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetEnvColor(gDisplayListHead++, 255, 50, 20, 160); // #FF3214A0

    Gfx_DrawTextureI4(0, 0, 0x40, 0x40, mark_texture, 0x40, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

/**
 * Ends a ladder flip: replaces the four flipping slot columns (objects 12-15) with upright front
 * columns (objects 4-7) using the ladder layout.
 */
void OpponentLadder_FinishFlip(OpponentLadder* ladder) {
    s32 i;

    for (i = 0; i < 4; i++) {
        ModelRenderer_ClearDisplayObject(&ladder->displayObjects[i + 4]);
        BattlePrep_ResetDisplayObject(&ladder->displayObjects[i + 12]);
        Model_InitDisplayObject(&ladder->displayObjects[i + 4], 1, 0, ladder->slotLayout);
        ladder->displayObjects[i + 4].rotation.x = 0;
        ladder->displayObjects[i + 4].node.callbackArg = ladder;
    }
}

/**
 * State 1 update: slides the four ladder columns in from the right at 32 units per frame for 10
 * frames, ending at x = -96, -32, 32 and 96, then goes idle.
 */
void OpponentLadder_UpdateSlideIn(OpponentLadder* ladder) {
    s32 i;

    ladder->timer++;

    for (i = 0; i < 4; i++) {
        ladder->displayObjects[4 + i].position.x = (0xE0 + i * 0x40) - (ladder->timer << 5);
    }

    if (ladder->timer == 0xA) {
        ladder->timer = 0;
        ladder->state = 0;
    }
}

/**
 * State 2 update: slides the four ladder columns off to the left at 32 units per frame for 10
 * frames, then hides them and goes idle.
 */
void OpponentLadder_UpdateSlideOut(OpponentLadder* ladder) {
    s32 i;
    s32 start_x;

    ladder->timer++;

    for (i = 0, start_x = -0x60; i < 4; i++, start_x += 0x40) {
        ladder->displayObjects[4 + i].position.x = (s16)start_x - (ladder->timer << 5);
    }

    if (ladder->timer == 0xA) {
        for (i = 0; i < 4; i++) {
            BattlePrep_ResetDisplayObject(&ladder->displayObjects[4 + i]);
        }
        ladder->timer = 0;
        ladder->state = 0;
    }
}

/**
 * State 3 update, after a win: the ladder flips over into view (half a turn about X over 16
 * frames). On frame 16 it plays sound 0x0110000C and settles; on frame 32 it plays sound 0x01100014
 * and stamps the defeated mark on the current round's opponent. In the castle the slot card is then
 * redrawn so the mark shows. Stamps follow the round index (roundOrDemoId), not the opponent
 * actually fought.
 */
void OpponentLadder_UpdateFlipInAndStamp(OpponentLadder* ladder) {
    s32 i;
    s32 slot_count;

    ladder->timer++;
    if (ladder->timer < 0x10) {
        for (i = 0; i < 4; i++) {
            ladder->displayObjects[4 + i].rotation.x += 0x800;
            ladder->displayObjects[12 + i].rotation.x += 0x800;
        }
    } else if (ladder->timer == 0x10) {
        Audio_PlayCategory11SoundCommand(0x0110000C, 0, 0);
        OpponentLadder_FinishFlip(ladder);
    } else if (ladder->timer == 0x20) {
        if (gSessionContext.mode == 7) {
            slot_count = 4;
        } else {
            slot_count = 8;
        }

        Audio_PlayCategory11SoundCommand(0x01100014, gSessionContext.roundOrDemoId, slot_count);
        OpponentLadder_StampDefeatedMark(ladder->portraits[gSessionContext.roundOrDemoId - 1]);

        if (gSessionContext.mode == 7) {
            GfxImage_SetRenderTarget(&gDisplayListHead, ladder->castleCards[gSessionContext.roundOrDemoId - 1]);

            gSPDisplayList(gDisplayListHead++, D_8006F4E0);

            Gfx_DrawTextureRgba16(4, 0x24, 0x38, 0x1C, &ladder->portraits[gSessionContext.roundOrDemoId - 1][0x208],
                                  0x40, 0x200000);
            Gfx_DrawTextureRgba16(4, 0x40, 0x38, 0x1C, &ladder->portraits[gSessionContext.roundOrDemoId - 1][0x1008],
                                  0x40, 0x200000);

            gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
        }

        ladder->timer = 0;
        ladder->state = 0;
    }
}

/**
 * State 4 update, after a loss: the ladder flips over into view (half a turn about X over 16
 * frames), then plays sound 0x0110000C and settles. No new mark is stamped.
 */
void OpponentLadder_UpdateFlipIn(OpponentLadder* ladder) {
    s32 i;

    for (i = 0; i < 4; i++) {
        ladder->displayObjects[4 + i].rotation.x -= 0x800;
        ladder->displayObjects[12 + i].rotation.x -= 0x800;
    }

    ladder->timer++;
    if (ladder->timer == 0x10) {
        Audio_PlayCategory11SoundCommand(0x0110000C, 0, 0);
        OpponentLadder_FinishFlip(ladder);
        ladder->timer = 0;
        ladder->state = 0;
    }
}

/**
 * Runs the per-frame ladder update for the current state: 1 slide in, 2 slide out, 3 flip in and
 * stamp (win), 4 flip in (loss); 0 is idle.
 */
void OpponentLadder_Update(OpponentLadder* ladder) {
    switch (ladder->state) {
        case 1:
            OpponentLadder_UpdateSlideIn(ladder);
            break;

        case 2:
            OpponentLadder_UpdateSlideOut(ladder);
            break;

        case 3:
            OpponentLadder_UpdateFlipInAndStamp(ladder);
            break;

        case 4:
            OpponentLadder_UpdateFlipIn(ladder);
            break;
    }
}

/**
 * Starts a ladder animation on the display objects starting at display_objects. animation 0 is
 * the round intro: columns 4-7 wait off-screen right and slide in (state 1). Otherwise columns
 * 12-15 are placed face-down at their final positions and flip in: animation 1 after a win stamps
 * the defeated opponent and stops the highlight (state 3); animation 2 after a loss keeps it
 * (state 4).
 */
void OpponentLadder_Start(OpponentLadder* ladder, DisplayObject* display_objects, s16 animation) {
    s32 i;
    s16 x;

    ladder->displayObjects = display_objects;
    ladder->timer = 0;

    if (animation == 0) {
        for (i = 4; i < 8; i++) {
            Model_InitDisplayObject(&ladder->displayObjects[i], 1, 0, ladder->slotLayout);
            Vec3f_SetComponentsDuplicate(&ladder->displayObjects[i].position, 320.0f, -48.0f, -289.0f);
            ladder->displayObjects[i].node.callbackArg = (s32)ladder;
        }
        ladder->state = 1;
    } else {
        for (i = 12, x = -0x60; i < 16; i++) {
            Model_InitDisplayObject(&ladder->displayObjects[i], 1, 0, ladder->slotLayout);
            Vec3f_SetComponentsDuplicate(&ladder->displayObjects[i].position, x, -48.0f, -289.0f);
            ladder->displayObjects[i].rotation.x = -0x8000;
            x += 0x40;
            ladder->displayObjects[i].node.callbackArg = ladder;
        }

        if (animation == 1) {
            ladder->highlightCurrent = 0;
            ladder->state = 3;
        } else {
            ladder->highlightCurrent = 1;
            ladder->state = 4;
        }
    }
}

/**
 * Renders Gym Leader Castle ladder card `slot` (64x96): the olive card background, the trainer's
 * portrait and the trainer's name centered at the top. For the rival (modeSelection 9) only slot 0
 * gets a portrait and name.
 */
void OpponentLadder_RenderCastleCard(OpponentLadder* ladder, s16 slot, char* name) {
    GfxImage_SetRenderTarget(&gDisplayListHead, ladder->castleCards[slot]);

    gSPDisplayList(gDisplayListHead++, D_8006F4E0);

    Gfx_DrawTextureRgba16(0, 0, 0x40, 0x20, D_3012000, 0x40, 0x200000);
    Gfx_DrawTextureRgba16(0, 0x20, 0x40, 0x20, D_3013000, 0x40, 0x200000);
    Gfx_DrawTextureRgba16(0, 0x40, 0x40, 0x20, D_3014000, 0x40, 0x200000);

    if ((gSessionContext.modeSelection < 9) || (slot == 0)) {
        Gfx_DrawTextureRgba16(4, 0x24, 0x38, 0x1C, ladder->portraits[slot] + 0x208, 0x40, 0x200000);
        Gfx_DrawTextureRgba16(4, 0x40, 0x38, 0x1C, ladder->portraits[slot] + 0x1008, 0x40, 0x200000);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    if ((gSessionContext.modeSelection < 9) || (slot == 0)) {
#if defined(VERSION_JP)
        Font_BeginTranslucentTextRendering();
        Font_SetLineHeight(0xC);
        Font_SetActive(2, -1);
        Font_Printf(4, 6, name);
        Font_EndTexturedTextRendering();
#else
        s32 name_width = Font_MeasureTextExtent(2, 0, name);

        Font_BeginTranslucentTextRendering();
        Font_SetLineHeight(0xC);
        Font_SetActive(2, 0);
        Font_Printf(0x20 - (name_width / 2), 6, name);
        Font_EndTexturedTextRendering();
#endif
    }
}

/**
 * Grows a main pool block and shifts its contents so the data starts on a 64-byte boundary;
 * returns the aligned address. Used for portrait textures.
 */
s32 BattlePrep_Align64PoolBlock(s32* block) {
    u32 size;
    s32 aligned;
    s32 i;
    u32 shift;

    aligned = (s32)block;
    shift = 0x40 - ((u32)block & 0x3F);
    if (shift != 0x40) {
        s32* src;
        s32* dst;

        size = main_pool_get_block_dist(block);
        src = (u32)block + size;
        dst = (u32)block + (size + shift);
        main_pool_realloc(block, (size + shift));
        for (; block < src;) {
            *--dst = *--src;
        }
        aligned += shift;
    }
    return aligned;
}

/**
 * Loads the ladder portraits. The trainer list for the current mode and division or location
 * (+31 in round 2) comes from trainer_archive; portraits come from portrait_archive. In the castle
 * the four location trainers are loaded and each slot card is rendered. In the cups, opponents up to
 * the current round get their portraits and later slots get the placeholder "?" portrait 0. Slots
 * before the current round are re-stamped with the defeated mark.
 */
void OpponentLadder_LoadPortraits(OpponentLadder* ladder, BinArchive* trainer_archive, BinArchive* portrait_archive) {
    s16 i;
    s16 roster_file;
    TrainerData* trainers;
    TrainerData* trainer;
    s16 portrait_file;

    switch (gSessionContext.mode) {
        case 1:
            roster_file = 6;
            break;

        case 2:
            roster_file = 7;
            break;

        case 3:
            roster_file = gSessionContext.modeSelection + 8;
            break;

        case 4:
            roster_file = 0;
            break;

        case 5:
            roster_file = 1;
            break;

        case 6:
            roster_file = gSessionContext.modeSelection + 2;
            break;

        case 7:
            roster_file = gSessionContext.modeSelection + 0xC;
            break;

        case 8:
            roster_file = 0x16;
            break;
    }

    if (gSessionContext.inRound2 != 0) {
        roster_file += 0x1F;
    }

    trainers = BinArchive_GetFile(trainer_archive, roster_file);
    if (gSessionContext.mode == 7) {
        for (i = 0; i < 4; i++) {
            trainer = i + ((void)0, trainers);

            portrait_file = (trainer->gfx_file_index >> 8) & 0xFF;
            ladder->portraits[i] = BattlePrep_Align64PoolBlock(BinArchive_GetFile(portrait_archive, portrait_file));
            if (i < (gSessionContext.roundOrDemoId - 1)) {
                OpponentLadder_StampDefeatedMark(ladder->portraits[i]);
            }
            ladder->castleCards[i] = GfxImage_Allocate(0, 2, 0x40, 0x60, 0);
            OpponentLadder_RenderCastleCard(ladder, i, trainer->name2);
        }
    } else {
        for (i = 0; i < 8; i++) {
            if (i < gSessionContext.roundOrDemoId) {
                portrait_file = (trainers[gSessionContext.opponentHistory[0][i]].gfx_file_index >> 8) & 0xFF;

                ladder->portraits[i] = BattlePrep_Align64PoolBlock(BinArchive_GetFile(portrait_archive, portrait_file));
                if (i < (gSessionContext.roundOrDemoId - 1)) {
                    OpponentLadder_StampDefeatedMark(ladder->portraits[i]);
                }
            } else {
                ladder->portraits[i] = BinArchive_GetFile(portrait_archive, 0);
            }
        }
    }
}

/**
 * Resets the ladder to idle with the highlight on, allocates the four castle slot cards, builds
 * the cup or castle ladder layout and loads the portraits.
 */
void OpponentLadder_Init(OpponentLadder* ladder, BinArchive* trainer_archive, BinArchive* portrait_archive) {
    s32 i;
    MemoryBlock* pool_block;

    ladder->state = 0;
    ladder->timer = 0;
    ladder->highlightCurrent = 1;
    ladder->unk_04 = 0;

    for (i = 0; i < 4; i++) {
        ladder->castleCards[i] = GfxImage_Allocate(0, 2, 0x40, 0x60, 0);
    }

    pool_block = MainPool_AllocState(main_pool_get_available(), 0);
    if (gSessionContext.mode == 7) {
        ladder->slotLayout = process_geo_layout(pool_block, D_84B11BD0);
    } else {
        ladder->slotLayout = process_geo_layout(pool_block, D_84B11640);
    }

    MainPool_FinalizeAllocation(pool_block);
    OpponentLadder_LoadPortraits(ladder, trainer_archive, portrait_archive);
}
