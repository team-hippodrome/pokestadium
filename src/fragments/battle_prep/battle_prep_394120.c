#include "battle_prep.h"
#include "src/geo_render.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/ui_graphics.h"
#include "src/pokemon_stats.h"
#include "src/save_data.h"
#include "src/game_state.h"
#include "src/session.h"
#include "src/text_system.h"
#include "src/audio_sfx.h"
#include "src/audio_commands_category2.h"
#include "src/gfx_buffer.h"
#include "src/gfx_rect.h"
#include "src/DDC0.h"
#include "src/matrix.h"
#include "src/memory.h"
#include "src/stage_loader.h"

s32 BattlePrep_StarfieldGeoCallback(s32, unk_D_86002F34_alt8*);

unk_D_84B19910 D_84B19910[400];
s32 pad_D_84B1A590[2];
unk_D_84B1A598 sBattlePrepState;
unk_D_84B259A8 D_84B259A8;
unk_D_84B259E8 D_84B259E8;
unk_D_84B25A28 D_84B25A28;
unk_D_84B25A58 D_84B25A58;
unk_D_84B25A90 D_84B25A90;
unk_D_84B25AC0 D_84B25AC0;
unk_D_84B25AC0 D_84B26080;
unk_D_84B26640 D_84B26640;
ModeSettings D_84B26670;
char** sBattlePrepText;

static u8* D_84B0FB10[] = {
    D_3015100,
    D_3015700,
    D_3015D00,
    D_3016300,
};
static u32 D_84B0FB20[] = {
    0x0C000000,  0x05000000,  0x0B00002D, 0x00000000,  0x014000F0,
    0x00000000,  0xFEDF0000,  0x00000121, 0x05000000,  0x0D000000,
    0x05000000,  0x0F000000,  0x05000000, 0x20050000,  0x00000000,
    0x00000000,  0x00000000,  0x00000000, 0x08000000,  BattlePrep_StarfieldGeoCallback,
    D_84B19910,  0x06000000,  0x0F000002, 0x05000000,  0x0A000000,
    &D_800AC840, 0x06000000,  0x06000000, 0x0D000001,  0x05000000,
    0x0F000002,  0x05000000,  0x0A000000, &D_800AC840, 0x06000000,
    0x06000000,  0x0D000002,  0x05000000, 0x0F000000,  0x05000000,
    0x0A000000,  &D_800AC840, 0x06000000, 0x06000000,  0x0D000003,
    0x05000000,  0x14000000,  0x002B0012, 0xFFFFFF32,  0x16FFFFFF,
    0x0F000003,  0x05000000,  0x0A000000, &D_800AC840, 0x06000000,
    0x06000000,  0x06000000,  0x06000000, 0x01000000,
};

/**
 * Records each Pokémon on the player's roster with VictoryPalace_SaveSpeciesRecord (Hall of Fame),
 * then commits save record 0x13. For modes 3/6 only when modeSelection is 3, for mode 7 only when 9.
 */
void BattlePrep_SaveVictoryPalaceRecords(void) {
    s16 temp_s0;
    ExtendedRosterInfo* temp_v0;
    BattleMon* var_s1;

    temp_v0 = gSessionContext.battleSides[0].tagTeamMember[0]->rosterInfo;
    temp_s0 = temp_v0->numberOfPokemonCanBePicked;
    var_s1 = temp_v0->battleMon;

    if ((gSessionContext.mode == 3) || (gSessionContext.mode == 6)) {
        if (gSessionContext.modeSelection != 3) {
            return;
        }
    } else if ((gSessionContext.mode == 7) && (gSessionContext.modeSelection != 9)) {
        return;
    }

    while (temp_s0-- > 0) {
        VictoryPalace_SaveSpeciesRecord(var_s1++, gSessionContext.mode, gSessionContext.inRound2);
    }

    Save_CommitTypedRecord(0x13, 0);
}

/**
 * On the first clear of mode 3 or 6 at modeSelection 3 (clearedModes bit 0x8 / 0x40 not yet set),
 * raises GameOptions.gbTowerUnlockTier (max 2) and sets transitionFlags 0x8 (tier 1) or 0x10 (tier 2).
 */
void BattlePrep_MarkFirstClearBonus(void) {
    GameOptions sp20;
    s32 sp1C = 0;

    Save_GetOptions(&sp20);

    if ((gSessionContext.mode == 3) && (gSessionContext.modeSelection == 3) && !(D_84B26670.clearedModes & 8)) {
        sp1C = 1;
    }

    if ((gSessionContext.mode == 6) && (gSessionContext.modeSelection == 3) && !(D_84B26670.clearedModes & 0x40)) {
        sp1C = 1;
    }

    if ((sp1C != 0) && (sp20.gbTowerUnlockTier < 2)) {
        sp20.gbTowerUnlockTier++;
        Save_SetOptions(&sp20);
        if (sp20.gbTowerUnlockTier == 1) {
            gSessionContext.transitionFlags |= 8;
        } else {
            gSessionContext.transitionFlags |= 0x10;
        }
    }
}

/**
 * Sets clearedModes bit (1 << mode) when the final selection of mode 3/6/7 is cleared; when all of
 * bits 0xF8 are set, unlocks Mewtwo and sets SESSION_FLAG_REVEAL_MEWTWO. Saves mode settings.
 */
void BattlePrep_MarkCupCleared(void) {
    if (((gSessionContext.mode != 3) || (gSessionContext.modeSelection >= 3)) &&
        ((gSessionContext.mode != 6) || (gSessionContext.modeSelection >= 3)) &&
        ((gSessionContext.mode != 7) || (gSessionContext.modeSelection >= 9))) {
        D_84B26670.clearedModes |= (1 << gSessionContext.mode);
        if (D_84B26670.clearedModes == 0xF8) {
            D_84B26670.mewtwoUnlocked = 1;
            gSessionContext.sessionFlags |= SESSION_FLAG_REVEAL_MEWTWO;
        }
        Save_SetModeSettings(&D_84B26670, gSessionContext.inRound2);
    }
}

/**
 * Raises castleClears / nintendoCupClears / primeCupClears to modeSelection + 1 when the current
 * selection exceeds the saved count, setting transitionFlags 0x400. Saves mode settings.
 */
void BattlePrep_MarkNewRoundRecord(void) {
    if (gSessionContext.mode == 7) {
        if ((gSessionContext.modeSelection >= D_84B26670.castleClears) && (gSessionContext.modeSelection < 8)) {
            D_84B26670.castleClears = gSessionContext.modeSelection + 1;
            gSessionContext.transitionFlags |= 0x400;
        }
    } else if (gSessionContext.mode == 3) {
        if ((gSessionContext.modeSelection >= D_84B26670.nintendoCupClears) && (gSessionContext.modeSelection < 4)) {
            D_84B26670.nintendoCupClears = gSessionContext.modeSelection + 1;
            gSessionContext.transitionFlags |= 0x400;
        }
    } else if ((gSessionContext.mode == 6) && (gSessionContext.modeSelection >= D_84B26670.primeCupClears) &&
               (gSessionContext.modeSelection < 4)) {
        D_84B26670.primeCupClears = gSessionContext.modeSelection + 1;
        gSessionContext.transitionFlags |= 0x400;
    }
    Save_SetModeSettings(&D_84B26670, gSessionContext.inRound2);
}

/**
 * Returns string file_number from the battle_prep text table (table 0x1B).
 */
char* BattlePrep_GetString(s32 file_number) {
    return Text_GetString(NULL, 0, sBattlePrepText, file_number);
}

/**
 * Formats string arg2 from the battle_prep text table into buffer arg0 of size arg1.
 */
char* BattlePrep_FormatString(char* arg0, s32 arg1, u32 arg2) {
    return Text_GetString(arg0, arg1, sBattlePrepText, arg2);
}

/**
 * Geo layout callback: on draw (arg0 == 5) draws the starfield from the node's callbackArg.
 */
s32 BattlePrep_StarfieldGeoCallback(s32 arg0, unk_D_86002F34_alt8* arg1) {
    if (arg0 == 5) {
        BattlePrep_DrawStarfield(arg1->node.callbackArg);
        GeoRender_ApplyMaterialState();
    }
}

/**
 * Clears a display object and resets its position to 0, scale to 1 and rotation to 0.
 */
void BattlePrep_ResetDisplayObject(DisplayObject* arg0) {
    ModelRenderer_ClearDisplayObject(arg0);
    Vec3f_SetComponentsDuplicate(&arg0->position, 0.0f, 0.0f, 0.0f);
    Vec3f_SetComponentsDuplicate(&arg0->scale, 1.0f, 1.0f, 1.0f);
    Vec3s_SetComponents(&arg0->rotation, 0, 0, 0);
}

/**
 * Resets all 128 display objects in sBattlePrepState.
 */
void BattlePrep_ResetAllDisplayObjects(void) {
    s32 i;

    for (i = 0; i < 128; i++) {
        BattlePrep_ResetDisplayObject(&sBattlePrepState.unk_0008[i]);
    }
}

/**
 * Prints arg2 in font 2 horizontally centered on x = 32 at row arg1 (arg0 unused).
 */
void BattlePrep_DrawCenteredText(s16 arg0, s16 arg1, char* arg2) {
    s32 sp1C = Font_MeasureTextExtent(2, 0, arg2);

    Font_BeginTranslucentTextRendering();
    Font_SetLineHeight(0xC);
    Font_SetActive(2, 0);
    Font_Printf(0x20 - (sp1C / 2), arg1, arg2);
    Font_EndTexturedTextRendering();
}

/**
 * Draws a rounded rectangle at (arg0, arg1) of size arg2 x arg3 tinted RGBA (arg4..arg7): four
 * corner textures up to 8 px each, with fills for the edges and center.
 */
void BattlePrep_DrawBorderedRect(s16 arg0, s16 arg1, s16 arg2, s16 arg3, u8 arg4, u8 arg5, u8 arg6, u8 arg7) {
    s16 temp_a2;
    s16 temp_s0_32;
    s16 var_s2;
    s16 var_s3;

    if (arg2 >= 0x10) {
        var_s3 = 8;
    } else {
        var_s3 = arg2 / 2;
    }
    if (arg3 >= 0x10) {
        var_s2 = 8;
    } else {
        var_s2 = arg3 / 2;
    }
    temp_a2 = arg2 - (var_s3 * 2);
    temp_s0_32 = arg3 - (var_s2 * 2);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetEnvColor(gDisplayListHead++, arg4, arg5, arg6, arg7);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_3015080, G_IM_FMT_I, 16, 8, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(arg0, arg1, var_s3, var_s2, 0, 0, 0x2000 / var_s3, 0x2000 / var_s2, 0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_30150C0, G_IM_FMT_I, 16, 8, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped((arg0 + arg2) - var_s3, arg1, var_s3, var_s2, 0, 0, 0x2000 / var_s3, 0x2000 / var_s2,
                                0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_3015000, G_IM_FMT_I, 16, 8, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(arg0, (arg1 + arg3) - var_s2, var_s3, var_s2, 0, 0, 0x2000 / var_s3, 0x2000 / var_s2,
                                0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_3015040, G_IM_FMT_I, 16, 8, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped((arg0 + arg2) - var_s3, (arg1 + arg3) - var_s2, var_s3, var_s2, 0, 0, 0x2000 / var_s3,
                                0x2000 / var_s2, 0);

    gSPDisplayList(gDisplayListHead++, gUiTranslucentFillDL);

    if ((temp_a2 > 0) && (temp_s0_32 > 0)) {
        Gfx_FillRectRgba(arg0 + var_s3, arg1, temp_a2, var_s2, arg4, arg5, arg6, arg7);
        Gfx_FillRectRgba(arg0, arg1 + var_s3, arg2, temp_s0_32, arg4, arg5, arg6, arg7);
        Gfx_FillRectRgba(arg0 + var_s3, arg1 + var_s3 + temp_s0_32, temp_a2, var_s2, arg4, arg5, arg6, arg7);
    } else if (temp_a2 > 0) {
        Gfx_FillRectRgba(arg0 + var_s3, arg1, temp_a2, arg3, arg4, arg5, arg6, arg7);
    } else if (temp_s0_32 > 0) {
        Gfx_FillRectRgba(arg0, arg1 + var_s2, arg2, temp_s0_32, arg4, arg5, arg6, arg7);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

/**
 * Renders side arg1's 64x96 nameplate image into buffer arg0 from frame arg3 and portrait arg2:
 * controller-port labels (or the COM label) in 2-player modes 0/9, otherwise a port label plus
 * the trainer name, or only the opponent name for side 1.
 */
void BattlePrep_DrawTrainerStatusIcon(DepthBuffer* arg0, s16 arg1, u8* arg2, u8* arg3) {
    BattleSessionTeams* temp_v1 = &gSessionContext.battleSides[arg1];

    GfxImage_SetRenderTarget(&gDisplayListHead, arg0);

    gSPDisplayList(gDisplayListHead++, D_8006F4E0);

    Gfx_DrawTextureRgba16(0, 0, 0x40, 0x20, arg3, 0x40, 0x200000);
    Gfx_DrawTextureRgba16(0, 0x20, 0x40, 0x20, arg3 + 0x1000, 0x40, 0x200000);
    Gfx_DrawTextureRgba16(0, 0x40, 0x40, 0x20, arg3 + 0x2000, 0x40, 0x200000);
    Gfx_DrawTextureRgba16(4, 0x24, 0x38, 0x1C, arg2 + 0x208, 0x40, 0x200000);
    Gfx_DrawTextureRgba16(4, 0x40, 0x38, 0x1C, arg2 + 0x1008, 0x40, 0x200000);

    if (gSessionContext.mode == 0 || gSessionContext.mode == 9) {
        if (temp_v1->tagTeamMember[0]->isHuman & 2) {
            Gfx_DrawTextureRgba16(8, 4, 0x30, 0x10, &D_3016900, 0x30, 0x200000);
        } else if (temp_v1->tagTeamSize == 1) {
            Gfx_DrawTextureRgba16(8, 4, 0x30, 0x10, D_84B0FB10[temp_v1->tagTeamMember[0]->controllerPort], 0x30, 0x200000);
        } else {
            if (temp_v1->tagTeamMember[0]->controllerPort < temp_v1->tagTeamMember[1]->controllerPort) {
                Gfx_DrawTextureRgba16(8, 4, 0x30, 0x10, D_84B0FB10[temp_v1->tagTeamMember[0]->controllerPort], 0x30,
                                      0x200000);
                Gfx_DrawTextureRgba16(8, 0x14, 0x30, 0x10, D_84B0FB10[temp_v1->tagTeamMember[1]->controllerPort], 0x30,
                                      0x200000);
            } else {
                Gfx_DrawTextureRgba16(8, 4, 0x30, 0x10, D_84B0FB10[temp_v1->tagTeamMember[1]->controllerPort], 0x30,
                                      0x200000);
                Gfx_DrawTextureRgba16(8, 0x14, 0x30, 0x10, D_84B0FB10[temp_v1->tagTeamMember[0]->controllerPort], 0x30,
                                      0x200000);
            }
        }

        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
    } else if ((arg1 == 0) || (gSessionContext.mode == 0xA)) {
        Gfx_DrawTextureRgba16(8, 4, 0x30, 0x10, D_84B0FB10[temp_v1->tagTeamMember[0]->controllerPort], 0x30, 0x200000);

        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

        BattlePrep_DrawCenteredText(arg1, 0x14, temp_v1->tagTeamMember[0]->rosterInfo->unk_014);
    } else {
        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

        BattlePrep_DrawCenteredText(arg1, 6, temp_v1->tagTeamMember[0]->rosterInfo->unk_220);
    }
}

/**
 * Frame arg0 (< 16) of the burst: moves display objects 0-15 apart vertically and horizontally
 * while spinning them.
 */
void BattlePrep_AnimateBallBurstEffect(s16 arg0) {
    static s16 D_84B0FC0C[] = {
        0x0300, 0x0150, 0xFEB0, 0xFD00, 0xFD00, 0xFEB0, 0x0150, 0x0300,
    };
    static f32 D_84B0FC1C[] = {
        -3.0f, -2.0f, 2.0f, 3.0f, -3.0f, -2.0f, 2.0f, 3.0f,
    };

    s32 i;

    if (arg0 < 0x10) {
        for (i = 0; i < 4; i++) {
            sBattlePrepState.unk_0008[i + 0].position.y += 2.0f * arg0 * 0.5f;
            sBattlePrepState.unk_0008[i + 0].rotation.x += 0x300;
            sBattlePrepState.unk_0008[i + 8].position.y += 2.0f * arg0 * 0.5f;
            sBattlePrepState.unk_0008[i + 8].rotation.x += 0x300;
            sBattlePrepState.unk_0008[i + 4].rotation.x -= 0x300;
            // clang-format off
            sBattlePrepState.unk_0008[i + 4].position.y -= 2.0f * arg0 * 0.5f; sBattlePrepState.unk_0008[i + 12].rotation.x -= 0x300;
            // clang-format on
            sBattlePrepState.unk_0008[i + 12].position.y -= 2.0f * arg0 * 0.5f;
        }

        for (i = 0; i < 8; i++) {
            sBattlePrepState.unk_0008[i + 0].rotation.z += D_84B0FC0C[i];
            sBattlePrepState.unk_0008[i + 8].rotation.z += D_84B0FC0C[i];

            sBattlePrepState.unk_0008[i + 0].position.x += arg0 * D_84B0FC1C[i] * 0.5f;
            sBattlePrepState.unk_0008[i + 8].position.x += arg0 * D_84B0FC1C[i] * 0.5f;
        }
    }
}

/**
 * Frame arg0 (< 16): fades lodFraction of display objects 0-15 and 28-31 from 0xF0 toward 0.
 */
void BattlePrep_FadeInBallBurstObjects(s16 arg0) {
    s32 i;

    if (arg0 < 0x10) {
        for (i = 0; i < 16; i++) {
            sBattlePrepState.unk_0008[i].lodFraction = (-arg0 * 0x10) + 0xF0;
        }

        for (i = 28; i < 32; i++) {
            sBattlePrepState.unk_0008[i].lodFraction = (-arg0 * 0x10) + 0xF0;
        }
    }
}

/**
 * Draws one frame: background stage, geo root (starfield and display objects) and both rule windows.
 */
void BattlePrep_RenderFrame(void) {
    BgStage_DrawFrame();
    GfxImage_FillCurrent(&gDisplayListHead, 1);
    Geo_RenderRootNode(sBattlePrepState.unk_0004);
    BattlePrep_DrawRuleWindow(&D_84B26640.unk_1C);
    BattlePrep_DrawRuleWindow(&D_84B26640.unk_28);
    BgStage_AdvanceFrame();
}

/**
 * Advances one frame of the phase 0 versus intro.
 */
void BattlePrep_UpdateVersusIntroFrame(void) {
    Controller_PollInputs();
    BattlePrep_UpdateStarfield(D_84B19910);
    BattlePrepRoster_Update(&D_84B25A28);
    BattlePrep_RenderFrame();
}

/**
 * Phase 0: pre-battle VS roster for one-off battles (Free Battle, Event, Battle Now, Mewtwo).
 * Plays music 0x31.
 */
s32 BattlePrep_ShowVersusIntro(void) {
    s32 i;
    s32 var_s1 = 0;

    Team_LoadOpponentPreset();
    BattlePrepRoster_Init(&D_84B25A28, sBattlePrepState.unk_0004->children);
    BattlePrepRoster_Load(&D_84B25A28, sBattlePrepState.unk_B40C);
    Audio_PlayMusicIfChanged(0x31);

    for (i = 0; i < 16; i++) {
        BattlePrep_UpdateVersusIntroFrame();
    }

    StageFade_StartFromOpaque(8);

    while (BattlePrepRoster_InitDisplayObjects(&D_84B25A28, sBattlePrepState.unk_0008) == 0) {
        BattlePrep_UpdateVersusIntroFrame();
        var_s1++;
    }

    while (var_s1 < 0x1E) {
        BattlePrep_UpdateVersusIntroFrame();
        var_s1++;
    }

    for (i = 0; i < 90; i++) {
        BattlePrep_UpdateVersusIntroFrame();
    }

    Audio_StopMusic(0x20);
    StageContext_SetClearColor(1);
    StageFade_StartFromTransparent(0x10);

    for (i = 0; i < 18; i++) {
        BattlePrep_UpdateVersusIntroFrame();
    }

    if (gSessionContext.sessionFlags & 1) {
        gSessionContext.sessionFlags &= ~1;
        Save_ResetAndCommitTypedRecord(0x15, 0);
        Save_FlushBank(2);
    }

    return 4;
}

/**
 * Advances one frame of the phase 1 round intro (starfield, banner carousel, opponent ladder, roster).
 */
void BattlePrep_UpdateRoundIntroFrame(void) {
    Controller_PollInputs();
    BattlePrep_UpdateStarfield(D_84B19910);
    BattlePrep_UpdateCarouselState(&D_84B259A8);
    BattlePrep_UpdateOpponentLadder(&D_84B259E8);
    BattlePrepRoster_Update(&D_84B25A28);
    BattlePrep_RenderFrame();
}

/**
 * Initializes the banner, opponent ladder and roster for the phase 1 round intro.
 */
void BattlePrep_InitRoundIntroScene(void) {
    BattlePrep_InitBannerScene(&D_84B259A8);
    BattlePrep_InitOpponentLadderScene(&D_84B259E8, sBattlePrepState.unk_B408, sBattlePrepState.unk_B40C);
    BattlePrepRoster_Init(&D_84B25A28, sBattlePrepState.unk_0004->children);
}

/**
 * Phase 1: cup or Gym Leader Castle round intro. Shows the battle_headers banner (cup or gym name,
 * BATTLE N / SEMI-FINAL / FINAL BATTLE label), the opponent ladder, then the roster. Plays music 0x33.
 */
s32 BattlePrep_ShowRoundIntro(void) {
    s32 i;
    s32 var_s1;

    if (osTvType == OS_TV_PAL) {
        var_s1 = 0x4B;
    } else {
        var_s1 = 0x5A;
    }

    Team_LoadOpponentPreset();
    BattlePrep_InitRoundIntroScene();
    BattlePrepRoster_Load(&D_84B25A28, sBattlePrepState.unk_B40C);

    for (i = 0; i < 16; i++) {
        BattlePrep_UpdateRoundIntroFrame();
    }

    StageFade_StartFromOpaque(8);

    for (i = 0; i < 20; i++) {
        BattlePrep_UpdateRoundIntroFrame();
    }

    BattlePrep_InitCarouselEntry(&D_84B259A8, &sBattlePrepState.unk_0008[0x10]);
    BattlePrep_InitOpponentLadder(&D_84B259E8, sBattlePrepState.unk_0008, 0);
    Audio_PlayMusicIfChanged(0x33);

    for (i = 0; i < var_s1; i++) {
        BattlePrep_UpdateRoundIntroFrame();
    }

    BattlePrep_StartCarouselExit(&D_84B259A8);
    D_84B259E8.unk_00 = 2;

    for (i = 0; i < 16; i++) {
        BattlePrep_UpdateRoundIntroFrame();
    }

    while (BattlePrepRoster_InitDisplayObjects(&D_84B25A28, sBattlePrepState.unk_0008) == 0) {
        BattlePrep_UpdateRoundIntroFrame();
    }

    for (i = 0; i < var_s1; i++) {
        BattlePrep_UpdateRoundIntroFrame();
    }

    Audio_StopMusic(0x20);
    StageContext_SetClearColor(1);
    StageFade_StartFromTransparent(0x10);

    for (i = 0; i < 18; i++) {
        BattlePrep_UpdateRoundIntroFrame();
    }

    if (gSessionContext.sessionFlags & 1) {
        gSessionContext.sessionFlags &= ~1;
        Save_ResetAndCommitTypedRecord(0x15, 0);
        Save_FlushBank(2);
    }

    return 4;
}

/**
 * In round 2 mode 6 at modeSelection 3 (session flag 0x2 clear), if every roster Pokémon passes the
 * unk_52 limits and a brought Pikachu (species 25) lacks Surf (move 57), sets transitionFlags 0x1000.
 */
void BattlePrep_CheckPikachuNeedsSurf(void) {
    s32 i;
    s32 j;
    TeamRoster* var_a1;
    ExtendedRosterInfo* temp_v1;

    if (!(gSessionContext.sessionFlags & 2) && (gSessionContext.inRound2 == 1) && (gSessionContext.mode == 6) &&
        (gSessionContext.modeSelection == 3)) {
        var_a1 = gSessionContext.battleSides[0].tagTeamMember[0];
        temp_v1 = var_a1->rosterInfo;

        for (i = 0; i < temp_v1->numberOfPokemonCanBePicked; i++) {
            s32 tmp = (temp_v1->battleMon[i].unk_52 & 0x70) >> 4;
            s32 tmp2 = temp_v1->battleMon[i].unk_52 & 0xF;

            if ((tmp >= 4) || (tmp2 >= 13)) {
                return;
            }
        }

        for (i = 0; i < var_a1->numberOfPokemonBrought; i++) {
            BattleMon* ptr = &var_a1->pokemonPicked[i];

            if (ptr->species.id == 0x19) {
                for (j = 0; j < 4; j++) {
                    if (ptr->moves[j] == 0) {
                        break;
                    } else if (ptr->moves[j] == 0x39) {
                        return;
                    }
                }
                gSessionContext.transitionFlags |= 0x1000;
                break;
            }
        }
    }
}

/**
 * Advances one frame of the result scene objects and rule windows, then renders.
 */
void BattlePrep_UpdateResultScene(void) {
    Controller_PollInputs();
    BattlePrep_UpdateStarfield(D_84B19910);
    BattlePrepTeamIcons_Update(&D_84B25A58);
    BattlePrep_UpdateOpponentLadder(&D_84B259E8);
    BattlePrepBadgeAward_Update(&D_84B25A90);
    BattlePrepResultBanner_Update(&D_84B25AC0);
    BattlePrepResultBanner_Update(&D_84B26080);
    BattlePrep_UpdateResultLabel(&D_84B26640.unk_00);
    BattlePrep_UpdateResultLabel(&D_84B26640.unk_10);
    BattlePrep_UpdateRuleWindow(&D_84B26640.unk_1C);
    BattlePrep_UpdateRuleWindow(&D_84B26640.unk_28);
    BattlePrep_RenderFrame();
}

/**
 * Initializes the result banner, team icons, badge grid and result labels for phase 2.
 */
void BattlePrep_InitResultScene(void) {
    s32 tmp;

    BattlePrepTeamIcons_Init(&D_84B25A58);
    BattlePrep_InitOpponentLadderScene(&D_84B259E8, sBattlePrepState.unk_B408, sBattlePrepState.unk_B40C);
    BattlePrepBadgeAward_Init(&D_84B25A90);

    if (sBattlePrepState.isDraw != 0) {
        BattlePrepResultBanner_Init(&D_84B25AC0, 1, -1);
        BattlePrepResultBanner_Init(&D_84B26080, 0, -1);
    } else {
        BattlePrepResultBanner_Init(&D_84B25AC0, 1, sBattlePrepState.playerWon);
        tmp = sBattlePrepState.playerWon == 0;
        BattlePrepResultBanner_Init(&D_84B26080, 0, tmp);
    }

    BattlePrep_InitResultLabel(&D_84B26640.unk_00, 0);
    BattlePrep_InitResultLabel(&D_84B26640.unk_10, 1);
}

/**
 * Reveals both teams and the WIN/LOSE/DRAW banners after a battle. Plays music 0x32, or 0x45
 * when the player loses to a COM roster.
 */
void BattlePrep_RunResultReveal(void) {
    s32 i;

    BattlePrepTeamIcons_Load(&D_84B25A58, sBattlePrepState.unk_B40C);

    for (i = 0; i < 8; i++) {
        BattlePrep_UpdateResultScene();
    }

    if (gSessionContext.battleSides[1].tagTeamMember[0]->isHuman & 2) {
        if (gSessionContext.battleSides[0].wonBattle == 1) {
            Audio_PlayMusicIfChanged(0x32);
        } else {
            Audio_PlayMusicIfChanged(0x45);
        }
    } else {
        Audio_PlayMusicIfChanged(0x32);
    }

    StageFade_StartFromOpaque(8);

    while (BattlePrepTeamIcons_InitDisplayObjects(&D_84B25A58, sBattlePrepState.unk_0008) == 0) {
        BattlePrep_UpdateResultScene();
    }

    while (D_84B25A58.unk_00 != 0) {
        BattlePrep_UpdateResultScene();
    }

    BattlePrepResultBanner_Launch(&D_84B25AC0, &sBattlePrepState.unk_0008[0x1C], &sBattlePrepState.unk_0008[0x20]);
    BattlePrepResultBanner_Launch(&D_84B26080, &sBattlePrepState.unk_0008[0x1E], &sBattlePrepState.unk_0008[0x20]);

    while ((D_84B259E8.unk_00 != 0) || (D_84B26080.unk_5A0 != 0)) {
        BattlePrep_UpdateResultScene();
    }
}

/**
 * Result reveal for a perfect cup win: also grows the PERFECT label (D_3028870) and, before the
 * final, the EXTRA CONTINUE label (D_30281B0).
 */
void BattlePrep_RunPerfectReveal(void) {
    s32 i;

    BattlePrepTeamIcons_Load(&D_84B25A58, sBattlePrepState.unk_B40C);

    for (i = 0; i < 8; i++) {
        BattlePrep_UpdateResultScene();
    }

    Audio_PlayMusicIfChanged(0x32);
    StageFade_StartFromOpaque(8);

    while (BattlePrepTeamIcons_InitDisplayObjects(&D_84B25A58, sBattlePrepState.unk_0008) == 0) {
        BattlePrep_UpdateResultScene();
    }

    for (i = 0; i < 50; i++) {
        BattlePrep_UpdateResultScene();
    }

    BattlePrep_StartResultLabelGrow(&D_84B26640.unk_00, &sBattlePrepState.unk_0008[0x1A]);
    Audio_PlayCategory11SoundCommand(0x01100013, 0, 0);

    while (D_84B25A58.unk_00 != 0) {
        BattlePrep_UpdateResultScene();
    }

    BattlePrep_StartResultLabelShrink(&D_84B26640.unk_00);
    BattlePrepResultBanner_Launch(&D_84B25AC0, &sBattlePrepState.unk_0008[0x1C], &sBattlePrepState.unk_0008[0x20]);
    BattlePrepResultBanner_Launch(&D_84B26080, &sBattlePrepState.unk_0008[0x1E], &sBattlePrepState.unk_0008[0x20]);

    if (gSessionContext.roundOrDemoId < 8) {
        for (i = 0; i < 16; i++) {
            BattlePrep_UpdateResultScene();
        }
        BattlePrep_StartResultLabelGrow(&D_84B26640.unk_10, &sBattlePrepState.unk_0008[0x1B]);
        Audio_PlaySoundEffectById(SFX_TROPHY_APPEAR);
    }

    while ((D_84B259E8.unk_00 != 0) || (D_84B26080.unk_5A0 != 0)) {
        BattlePrep_UpdateResultScene();
    }
}

/**
 * Shows the opponent ladder after a cup or castle battle and, on a win, stamps the defeated
 * opponent's portrait. Stamps follow the round index (roundOrDemoId), not the opponents actually
 * fought: skipping rounds leaves stamped placeholder portraits in the skipped slots.
 */
void BattlePrep_RunOpponentLadderResult(void) {
    s32 i;
    s32 tmp;

    BattlePrepResultBanner_StartExit(&D_84B26080);
    tmp = sBattlePrepState.playerWon == 0;
    BattlePrep_InitOpponentLadder(&D_84B259E8, sBattlePrepState.unk_0008, tmp + 1);

    while ((D_84B259E8.unk_00 != 0) || (D_84B26080.unk_5A0 != 0)) {
        BattlePrep_UpdateResultScene();
    }

    for (i = 0; i < 16; i++) {
        BattlePrep_UpdateResultScene();
    }
}

/**
 * Waits for both result banners to finish, resets display objects 16+, plays sound 0x01100017,
 * runs 16 burst frames, then resets objects 0-15 and runs 16 idle starfield frames.
 */
void BattlePrep_RunBallBurstTransition(void) {
    s32 i;
    s32 j;

    while ((D_84B25AC0.unk_5A0 != 0) || (D_84B26080.unk_5A0 != 0)) {
        BattlePrep_UpdateResultScene();
    }

    for (i = 16; i < ARRAY_COUNT(sBattlePrepState.unk_0008); i++) {
        BattlePrep_ResetDisplayObject(&sBattlePrepState.unk_0008[i]);
    }

    Audio_PlayCategory11SoundCommand(0x01100017, 0, 0);

    for (j = 0; j < 16; j++) {
        Controller_PollInputs();
        BattlePrep_AnimateBallBurstEffect(j);
        BattlePrep_UpdateStarfield(D_84B19910);
        BattlePrep_RenderFrame();
    }

    for (i = 0; i < 16; i++) {
        BattlePrep_ResetDisplayObject(&sBattlePrepState.unk_0008[i]);
    }

    for (j = 0; j < 16; j++) {
        Controller_PollInputs();
        BattlePrep_UpdateStarfield(D_84B19910);
        BattlePrep_RenderFrame();
    }
}

/**
 * Stops music and runs 16 frames fading out display objects via BattlePrep_FadeInBallBurstObjects.
 */
void BattlePrep_FadeInBallBurstTransition(void) {
    s32 i;

    Audio_StopMusic(0x3C);

    for (i = 0; i < 16; i++) {
        Controller_PollInputs();
        BattlePrep_FadeInBallBurstObjects(i);
        BattlePrep_UpdateStarfield(D_84B19910);
        BattlePrep_RenderFrame();
    }
}

/**
 * Stops music, starts a 16-frame fade from transparent, and runs 34 starfield frames to finish it.
 */
void BattlePrep_FadeToTransparentEnding(void) {
    s32 i;

    Audio_StopMusic(0x3C);
    StageContext_SetClearColor(1);
    StageFade_StartFromTransparent(0x10);

    for (i = 0; i < 18; i++) {
        Controller_PollInputs();
        BattlePrep_UpdateStarfield(D_84B19910);
        BattlePrep_RenderFrame();
    }

    for (i = 0; i < 16; i++) {
        Controller_PollInputs();
        BattlePrep_UpdateStarfield(D_84B19910);
        BattlePrep_RenderFrame();
    }
}

/**
 * "Keep battling?" Continue / Save and Quit prompt after a cup or castle win. Save and Quit
 * advances roundOrDemoId and writes continue data (after the overwrite prompt if a save exists).
 */
s32 BattlePrep_PromptKeepBattling(void) {
    s32 var_s2;
    s32 var_v0_2;

    if (gSessionContext.mode == 7) {
        BattlePrep_InitRuleWindow(&D_84B26640.unk_1C, 2);
    } else {
        BattlePrep_InitRuleWindow(&D_84B26640.unk_1C, 1);
    }

    var_s2 = BattlePrep_PollRuleWindow(&D_84B26640.unk_1C);
    while (var_s2 == -1) {
        BattlePrep_UpdateResultScene();
        var_s2 = BattlePrep_PollRuleWindow(&D_84B26640.unk_1C);
    }

    if (var_s2 == 1) {
        if (Save_ContinueRecordExists() != 0) {
            BattlePrep_InitRuleWindow(&D_84B26640.unk_1C, 0xA);

            var_v0_2 = BattlePrep_PollRuleWindow(&D_84B26640.unk_1C);
            while (var_v0_2 == -1) {
                BattlePrep_UpdateResultScene();
                var_v0_2 = BattlePrep_PollRuleWindow(&D_84B26640.unk_1C);
            }

            if (var_v0_2 == 1) {
                var_s2 = 0;
            }
        }
        if (var_s2 == 1) {
            gSessionContext.roundOrDemoId = gSessionContext.roundOrDemoId + 1;
            Session_SaveContinueData();
            Save_FlushBank(2);
        }
    }
    return var_s2;
}

/**
 * Cup loss menu Save option: confirms overwriting existing continue data, then spends a
 * continue and writes the continue data. Returns 0 if the player declines the overwrite.
 * Only BattlePrep_ShowCupResult calls this. Its Continue option spends a continue
 * directly, without saving. Continues are only spent after a cup loss.
 */
s32 BattlePrep_SpendContinueAndSave(void) {
    s32 sp24 = 1;
    s32 var_v0;

    if (Save_ContinueRecordExists() != 0) {
        BattlePrep_InitRuleWindow(&D_84B26640.unk_1C, 0xA);

        var_v0 = BattlePrep_PollRuleWindow(&D_84B26640.unk_1C);
        while (var_v0 == -1) {
            BattlePrep_UpdateResultScene();
            var_v0 = BattlePrep_PollRuleWindow(&D_84B26640.unk_1C);
        }

        if (var_v0 == 1) {
            sp24 = 0;
        }
    }

    if (sp24 != 0) {
        gSessionContext.continueCount--;
        Session_SaveContinueData();
        Save_FlushBank(2);
    }
    return sp24;
}

/**
 * Free Battle (mode 0) result: adds wins to winCount, shows the win record and the
 * Try once more / Change team combination / Quit menu. Try once more (0x21) returns to
 * entry Pokemon selection; Change team combination (0x81) returns to the Free Battle rules
 * screen, where the controller ports assigned to each team can be changed; Quit (0x1)
 * returns to the area select. Once the winner's winCount reaches 99, Try once more is disabled
 * and the cursor starts on Change team combination.
 */
s32 BattlePrep_ShowFreeBattleResult(void) {
    s32 var_v0;

    if (gSessionContext.battleSides[0].wonBattle == 1) {
        gSessionContext.battleSides[0].winCount += 1;
    }

    if (gSessionContext.battleSides[1].wonBattle == 1) {
        gSessionContext.battleSides[1].winCount += 1;
    }

    BattlePrep_RunResultReveal();
    Audio_PlayCategory11SoundCommand(0x0110000F, 0, 0);
    BattlePrep_InitRuleWindow(&D_84B26640.unk_1C, 9);
    BattlePrep_InitRuleWindow(&D_84B26640.unk_28, 5);

    var_v0 = BattlePrep_PollRuleWindow(&D_84B26640.unk_28);
    while (var_v0 == -1) {
        BattlePrep_UpdateResultScene();
        var_v0 = BattlePrep_PollRuleWindow(&D_84B26640.unk_28);
    }

    switch (var_v0) {
        case 0:
            gSessionContext.transitionFlags |= 0x21;
            break;

        case 1:
            gSessionContext.transitionFlags |= 0x81;
            break;

        case 2:
            gSessionContext.transitionFlags |= 1;
            break;
    }

    BattlePrepResultBanner_StartExit(&D_84B26080);
    BattlePrepResultBanner_StartExit(&D_84B25AC0);
    Audio_PlayCategory11SoundCommand(0x0110000D, 0, 0);
    BattlePrep_RunBallBurstTransition();
    BattlePrep_FadeToTransparentEnding();

    return 4;
}

/**
 * Battle Now and Event (modes 9 and 10) result: shows the WIN/LOSE banner and waits for A.
 */
s32 BattlePrep_ShowSimpleResult(void) {
    BattlePrep_RunResultReveal();

    do {
        BattlePrep_UpdateResultScene();
    } while (!(BTN_IS_PRESSED(gPlayer1Controller, BTN_A)));

    BattlePrepResultBanner_StartExit(&D_84B26080);
    BattlePrepResultBanner_StartExit(&D_84B25AC0);
    Audio_PlayCategory11SoundCommand(0x0110000D, 0, 0);
    BattlePrep_RunBallBurstTransition();
    BattlePrep_FadeToTransparentEnding();

    return 4;
}

/**
 * Stadium cup (modes 1-6) result. On a win: ladder mark, badge grid, then Keep battling?
 * (or cup clear after the final, roundOrDemoId 8). On a loss: progress header plus the
 * Continue / Save / Re-enter / From the start / Quit menu. A cup final win hands off to the
 * cup clear ceremony (mode 17); observed transitionFlags 0x403 (Poke Ball) and 0x40B (Master
 * Ball, which also raises the GB Tower unlock tier).
 */
s32 BattlePrep_ShowCupResult(void) {
    UNUSED s32 pad[2];
    s32 sp2C = 4;
    s32 i;
    s32 var_s2;
    s32 var_v0;

    if (sBattlePrepState.perfectWin != 0) {
        BattlePrep_RunPerfectReveal();
    } else {
        BattlePrep_RunResultReveal();
    }

    BattlePrep_RunOpponentLadderResult();

    if (gSessionContext.battleSides[0].wonBattle == 1) {
        gSessionContext.continueCount += sBattlePrepState.perfectWin;
        if ((sBattlePrepState.perfectWin != 0) && (gSessionContext.roundOrDemoId < 8)) {
            BattlePrep_StartResultLabelShrink(&D_84B26640.unk_10);
        }

        BattlePrepResultBanner_StartExit(&D_84B25AC0);
        BattlePrepBadgeAward_InitDisplayObjects(&D_84B25A90, sBattlePrepState.unk_0008);

        while (D_84B25A90.unk_00 != 3) {
            BattlePrep_UpdateResultScene();
        }

        if (gSessionContext.roundOrDemoId == 8) {
            for (i = 0; i < 120; i++) {
                BattlePrep_UpdateResultScene();
                if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
                    break;
                }
            }

            BattlePrep_SaveVictoryPalaceRecords();
            BattlePrep_MarkFirstClearBonus();
            BattlePrep_MarkCupCleared();
            BattlePrep_MarkNewRoundRecord();
            Save_CommitTypedRecord(0x14, 0);
            Save_FlushBank(2);
            BattlePrep_RunBallBurstTransition();
            BattlePrep_FadeToTransparentEnding();
            BattlePrep_CheckPikachuNeedsSurf();
            gSessionContext.transitionFlags |= 3;
        } else if (BattlePrep_PromptKeepBattling() == 1) {
            BattlePrep_RunBallBurstTransition();
            BattlePrep_FadeToTransparentEnding();
            gSessionContext.transitionFlags |= 0x41;
        } else {
            BattlePrep_RunBallBurstTransition();
            Audio_StopMusic(0x50);
            gSessionContext.roundOrDemoId++;
            sp2C = 1;
        }
    } else {
        var_s2 = 1;
        while (var_s2 != 0) {
            BattlePrep_InitRuleWindow(&D_84B26640.unk_1C, 7);
            BattlePrep_InitRuleWindow(&D_84B26640.unk_28, 3);

            var_v0 = BattlePrep_PollRuleWindow(&D_84B26640.unk_28);
            while (var_v0 == -1) {
                BattlePrep_UpdateResultScene();
                var_v0 = BattlePrep_PollRuleWindow(&D_84B26640.unk_28);
            }

            switch (var_v0) {
                case 0:
                    sp2C = 1;
                    gSessionContext.continueCount--;
                    var_s2 = 0;
                    break;

                case 1:
                    if (BattlePrep_SpendContinueAndSave() != 0) {
                        var_s2 = 0;
                        gSessionContext.transitionFlags |= 0x41;
                    }
                    break;

                case 2:
                    var_s2 = 0;
                    gSessionContext.transitionFlags |= 0x21;
                    break;

                case 3:
                    var_s2 = 0;
                    gSessionContext.transitionFlags |= 0x801;
                    break;

                case 4:
                    var_s2 = 0;
                    gSessionContext.transitionFlags |= 1;
                    break;
            }
        }

        if (sp2C == 4) {
            BattlePrepResultBanner_StartExit(&D_84B25AC0);
            BattlePrep_RunBallBurstTransition();
            BattlePrep_FadeToTransparentEnding();
        } else {
            BattlePrep_FadeInBallBurstTransition();
            BattlePrep_ResetAllDisplayObjects();
        }
    }

    return sp2C;
}

/**
 * Gym Leader Castle (mode 7) result. A win shows Keep battling? or, after a gym leader or the
 * rival, returns phase 3. A loss shows Re-enter / Re-start at this GYM / Quit. Observed
 * transitionFlags: 0x501 after the Elite Four clear and 0x201 after the rival clear.
 */
s32 BattlePrep_ShowCastleResult(void) {
    s32 sp28;
    s32 i;
    s32 sp24 = 4;
    s32 var_v0;

    BattlePrep_RunResultReveal();
    BattlePrep_RunOpponentLadderResult();

    if (gSessionContext.battleSides[0].wonBattle == 1) {
        if ((gSessionContext.roundOrDemoId == 4) || (gSessionContext.modeSelection == 9)) {
            BattlePrep_MarkCupCleared();
            BattlePrep_MarkNewRoundRecord();
            BattlePrep_SaveVictoryPalaceRecords();
            Save_CommitTypedRecord(0x14, 0);
            Save_FlushBank(2);

            for (i = 0; i < 60; i++) {
                BattlePrep_UpdateResultScene();
                if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
                    break;
                }
            }

            BattlePrepResultBanner_StartExit(&D_84B25AC0);
            Audio_PlayCategory11SoundCommand(0x0110000D, 0, 0);
            BattlePrep_RunBallBurstTransition();
            Audio_StopMusic(0x10);

            gSessionContext.transitionFlags |= 1;
            if (gSessionContext.modeSelection == 8) {
                gSessionContext.transitionFlags |= 0x400;
            }

            if (gSessionContext.modeSelection == 9) {
                gSessionContext.transitionFlags |= 0x200;
            } else {
                gSessionContext.transitionFlags |= 0x100;
            }
            sp24 = 3;
        } else if (BattlePrep_PromptKeepBattling() == 1) {
            BattlePrepResultBanner_StartExit(&D_84B25AC0);
            Audio_PlayCategory11SoundCommand(0x0110000D, 0, 0);
            BattlePrep_RunBallBurstTransition();
            BattlePrep_FadeToTransparentEnding();
            gSessionContext.transitionFlags |= 0x41;
        } else {
            BattlePrepResultBanner_StartExit(&D_84B25AC0);
            Audio_PlayCategory11SoundCommand(0x0110000D, 0, 0);
            BattlePrep_RunBallBurstTransition();
            Audio_StopMusic(0x50);
            gSessionContext.roundOrDemoId++;
            sp24 = 1;
        }
    } else {
        BattlePrep_InitRuleWindow(&D_84B26640.unk_1C, 8);
        BattlePrep_InitRuleWindow(&D_84B26640.unk_28, 4);

        var_v0 = BattlePrep_PollRuleWindow(&D_84B26640.unk_28);
        while (var_v0 == -1) {
            BattlePrep_UpdateResultScene();
            var_v0 = BattlePrep_PollRuleWindow(&D_84B26640.unk_28);
        }

        switch (var_v0) {
            case 0:
                gSessionContext.transitionFlags |= 0x21;
                break;

            case 1:
                gSessionContext.transitionFlags |= 0x801;
                break;

            case 2:
                gSessionContext.transitionFlags |= 1;
                break;
        }

        BattlePrepResultBanner_StartExit(&D_84B25AC0);
        BattlePrep_RunBallBurstTransition();
        BattlePrep_FadeToTransparentEnding();

        if (gSessionContext.modeSelection == 9) {
            gSessionContext.modeSelection = 8;
        }
    }
    return sp24;
}

/**
 * Mewtwo (mode 8) result. A loss offers Battle again / Quit; a win sets transitionFlags 0x5 (0x4 plus 0x1).
 */
s32 BattlePrep_ShowMewtwoResult(void) {
    s32 var_v0;
    u16 sp2A;

    BattlePrep_RunResultReveal();
    if (gSessionContext.battleSides[0].wonBattle == 1) {
        s32 tmp1 = !!(D_84B26670.clearedModes & 0x100);

        if ((gSessionContext.inRound2 == 0) & !tmp1) {
            Save_GetOptionsField02(&sp2A);
            sp2A |= 4;
            Save_SetOptionsField02(&sp2A);
        }

        BattlePrep_MarkCupCleared();
        Save_CommitTypedRecord(0x14, 0);
        Save_FlushBank(2);
        BattlePrepResultBanner_StartExit(&D_84B26080);
        BattlePrepResultBanner_StartExit(&D_84B25AC0);
        Audio_PlayCategory11SoundCommand(0x0110000D, 0, 0);
        BattlePrep_RunBallBurstTransition();
        BattlePrep_FadeToTransparentEnding();
        gSessionContext.transitionFlags |= 5;
    } else {
        BattlePrep_InitRuleWindow(&D_84B26640.unk_28, 6);

        var_v0 = BattlePrep_PollRuleWindow(&D_84B26640.unk_28);
        while (var_v0 == -1) {
            BattlePrep_UpdateResultScene();
            var_v0 = BattlePrep_PollRuleWindow(&D_84B26640.unk_28);
        }

        switch (var_v0) {
            case 0:
                gSessionContext.transitionFlags |= 0x21;
                break;

            case 1:
                gSessionContext.transitionFlags |= 1;
                break;
        }

        BattlePrepResultBanner_StartExit(&D_84B26080);
        BattlePrepResultBanner_StartExit(&D_84B25AC0);
        Audio_PlayCategory11SoundCommand(0x0110000D, 0, 0);
        BattlePrep_RunBallBurstTransition();
        BattlePrep_FadeToTransparentEnding();
    }
    return 4;
}

/**
 * Returns 1 when a cup mode (1-6) battle was won with all three picked Pokemon at nonzero HP.
 * The result grants an extra continue and shows the PERFECT label.
 */
u8 BattlePrep_IsPerfectCupWin(void) {
    s32 i;
    u8 var_v1 = 0;

    if ((gSessionContext.mode > 0) && (gSessionContext.mode < 7)) {
        if (gSessionContext.battleSides[0].wonBattle == 1) {
            var_v1 = 1;
            for (i = 0; i < 3; i++) {
                if (gSessionContext.battleSides[0].tagTeamMember[0]->pokemonPicked[i].hp == 0) {
                    var_v1 = 0;
                }
            }
        }
    }

    return var_v1;
}

/**
 * Phase 2: post-battle result screen. Records draw/win state, then dispatches on
 * gSessionContext.mode to the per-mode result handler and returns the next phase.
 */
s32 BattlePrep_ShowBattleResult(void) {
    s32 var_v0;

    sBattlePrepState.perfectWin = BattlePrep_IsPerfectCupWin();
    BattlePrep_InitResultScene();

    switch (gSessionContext.mode) {
        case 0:
            var_v0 = BattlePrep_ShowFreeBattleResult();
            break;

        case 1:
            var_v0 = BattlePrep_ShowCupResult();
            break;

        case 2:
            var_v0 = BattlePrep_ShowCupResult();
            break;

        case 3:
            var_v0 = BattlePrep_ShowCupResult();
            break;

        case 4:
            var_v0 = BattlePrep_ShowCupResult();
            break;

        case 5:
            var_v0 = BattlePrep_ShowCupResult();
            break;

        case 6:
            var_v0 = BattlePrep_ShowCupResult();
            break;

        case 7:
            var_v0 = BattlePrep_ShowCastleResult();
            break;

        case 8:
            var_v0 = BattlePrep_ShowMewtwoResult();
            break;

        case 9:
            var_v0 = BattlePrep_ShowSimpleResult();
            break;

        case 10:
            var_v0 = BattlePrep_ShowSimpleResult();
            break;
    }
    return var_v0;
}

/**
 * Phase 3: castle gym-clear outro, reached when BattlePrep_ShowCastleResult returns 3.
 * Plays music 0x3F.
 */
s32 BattlePrep_ShowCastleClearOutro(void) {
    s32 i;
    s32 var_s1;

    if (gSessionContext.modeSelection == 9) {
        var_s1 = 0x3C;
    } else {
        var_s1 = 0x4B;
    }

    BattlePrep_InitBannerScene(&D_84B259A8);
    BattlePrep_InitCarouselExit(&D_84B259A8, &sBattlePrepState.unk_0008[0x10]);
    Audio_PlayMusicIfChanged(0x3F);

    for (i = 0; i < var_s1; i++) {
        Controller_PollInputs();
        BattlePrep_UpdateStarfield(D_84B19910);
        BattlePrep_UpdateCarouselState(&D_84B259A8);
        BattlePrep_RenderFrame();
    }

    StageContext_SetClearColor(1);
    StageFade_StartFromTransparent(0x10);

    for (i = 0; i < 18; i++) {
        Controller_PollInputs();
        BattlePrep_UpdateStarfield(D_84B19910);
        BattlePrep_UpdateCarouselState(&D_84B259A8);
        BattlePrep_RenderFrame();
    }

    return 4;
}

/**
 * Computes playerWon / isDraw from battleSides[].wonBattle, attaches the 128 display objects, builds
 * the geo layout D_84B0FB20 (near 10, far 6000), initializes the starfield and closes both rule windows.
 */
void BattlePrep_InitScene(void) {
    MemoryBlock* temp_v0;
    CameraNode* ptr;
    DisplayObject* ptr2;
    s32 i;

    sBattlePrepState.playerWon = 0;
    sBattlePrepState.isDraw = 0;
    if (gSessionContext.battleSides[0].wonBattle == gSessionContext.battleSides[1].wonBattle) {
        sBattlePrepState.isDraw = 1;
    } else if (gSessionContext.battleSides[0].wonBattle == 1) {
        sBattlePrepState.playerWon = 1;
    }

    ModelRenderer_InitDisplayRoots();

    for (i = 0, ptr2 = &sBattlePrepState.unk_0008[0]; i < 128; ptr2++, i++) {
        ModelRenderer_AttachDisplayObject(ptr2);
        sBattlePrepState.unk_0008[i].effectSlot = i;
    }

    temp_v0 = MainPool_AllocState(main_pool_get_available(), 0);
    sBattlePrepState.unk_0004 = process_geo_layout(temp_v0, &D_84B0FB20);
    MainPool_FinalizeAllocation(temp_v0);

    ptr = sBattlePrepState.unk_0004->children;
    ptr->perspective.nearPlane = 10.0f;
    ptr->perspective.farPlane = 6000.0f;

    BattlePrep_InitStarfield(&D_84B19910);
    BattlePrep_CloseRuleWindow(&D_84B26640.unk_1C);
    BattlePrep_CloseRuleWindow(&D_84B26640.unk_28);
}

/**
 * Fragment entry. arg0 selects the phase: 0 = versus intro, 1 = cup or castle round intro,
 * 2 = post-battle result, 3 = castle gym-clear outro. Loops while a phase returns another phase
 * (a cup Continue returns 1 to show the next round intro). Called from the mode states in
 * src/game_state.c around BATTLE_ENGINE.
 */
//@src/game_state.c
s32 BattlePrep_Main(s32 arg0, UNUSED s32 arg1) {
    StageContext* sp2C;

    main_pool_push_state('EYEC');
    Save_EnsureBankLoaded(2);
    Save_GetModeSettings(&D_84B26670, gSessionContext.inRound2);
    Gfx_InitDisplayListBuffers(0x18000, 0);
    sp2C = StageContext_Allocate(0, 1, 3, 1, 2, 1);
    Font_Init(0x17, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_2000000, badge_ui, 0);
    ASSET_LOAD(D_3000000, common_menu0_ui, 0);

    Text_InitStringTables();
    sBattlePrepText = Text_GetStringTable(0x1B);
    PokeIcon_OpenModelArchive();

    sBattlePrepState.unk_B408 = BinArchive_Open(TRAINER_DATA_ROM_START, NULL, 1, 1);
    sBattlePrepState.unk_B40C = ASSET_LOAD2(battle_portraits, 1, 1);

    StageContext_Activate(sp2C);
    BattlePrep_InitScene();

    while (arg0 < 4) {
        main_pool_push_state('eye0');

        switch (arg0) {
            case 0:
                arg0 = BattlePrep_ShowVersusIntro();
                break;

            case 1:
                arg0 = BattlePrep_ShowRoundIntro();
                break;

            case 2:
                arg0 = BattlePrep_ShowBattleResult();
                break;

            case 3:
                arg0 = BattlePrep_ShowCastleClearOutro();
                break;
        }

        main_pool_pop_state('eye0');
    }

    StageContext_Deactivate();
    Font_Free();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('EYEC');

    return 0;
}
