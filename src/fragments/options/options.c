#include "options.h"
#include "src/graphics_textures.h"
#include "src/ui_graphics.h"
#include "src/save_data.h"
#include "src/game_state.h"
#include "src/text_system.h"
#include "src/3D140.h"
#include "src/jpeg_stream.h"
#include "src/audio_sfx.h"
#include "src/audio_channel.h"
#include "src/gfx_buffer.h"
#include "src/memory.h"
#include "src/stage_loader.h"

static char** sOptionsText;
static s16 sSelectedOption;
static u16 sPendingSoundSettings;

/* Tiles the options background with the menu's intensity texture and colors. */
void Options_DrawBackground(void) {
    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetCombineLERP(gDisplayListHead++, ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE, TEXEL0, 0, ENVIRONMENT, 0,
                      ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE, TEXEL0, 0, ENVIRONMENT, 0);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetEnvColor(gDisplayListHead++, 0x32, 0x96, 0x50, 0xFF); // #329650
    gDPSetPrimColor(gDisplayListHead++, 0, 0, 0x14, 0x64, 0x32, 0xFF); // #146432
    gSPClearGeometryMode(gDisplayListHead++, G_ZBUFFER | G_LIGHTING);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_300F780, G_IM_FMT_I, 128, 64, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 7, 6, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(0, 0, 640, 480, 0, 0, 0x400, 0x400, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

/* Draws a textured rectangular border and a solid interior using the supplied color. */
void Options_DrawFilledPanel(s16 x, s16 y, s16 width, s16 height, u8 red, u8 green, u8 blue, u8 alpha) {
    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetEnvColor(gDisplayListHead++, red, green, blue, alpha);

    Gfx_DrawTextureIa8(x, y, 8, 8, D_2000B40, 8, 0);
    Gfx_DrawTextureIa8((x + width) - 8, y, 8, 8, D_2000B80, 8, 0);
    Gfx_DrawTexturedRectClipped(x + 8, y, width - 16, 8, 0, 0, 0, 0x400, 0);
    Gfx_DrawTextureIa8(x, (y + height) - 8, 8, 8, D_2000BC0, 8, 0);
    Gfx_DrawTexturedRectClipped(x, y + 8, 8, height - 16, 0, 0, 0x400, 0, 0);
    Gfx_DrawTextureIa8((x + width) - 8, (y + height) - 8, 8, 8, D_2000C00, 8, 0);
    Gfx_DrawTexturedRectClipped(x + 8, (y + height) - 8, width - 16, 8, 0, 0, 0, 0x400, 0);
    Gfx_DrawTexturedRectClipped((x + width) - 8, y + 8, 8, height - 16, 0, 0, 0x400, 0, 0);

    gSPDisplayList(gDisplayListHead++, gUiTranslucentFillDL);

    Gfx_FillRectRgba(x + 8, y + 8, width - 16, height - 16, red, green, blue, alpha);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

/* Draws the tinted capsule behind a sound-setting value. */
void Options_DrawValueCapsule(s16 x, s16 y) {
    gDPSetEnvColor(gDisplayListHead++, 0x3C, 0x3C, 0xA0, 0xFF); // #3c3ca0

    Gfx_DrawTextureI4(x, y, 0x10, 28, D_30242C0, 0x10, 0);
    Gfx_DrawTextureI4(x + 104, y, 0x10, 28, D_30243A0, 0x10, 0);
    Gfx_DrawTexturedRectClipped(x + 0x10, y, 88, 28, 0, 0, 0, 0x400, 0);
}

/* Draws the expanding options panel, showing its rows and optional cursor when fully open. */
void Options_DrawMainPanel(s16 opening_progress, s32 show_cursor) {
    // FAKE: unused stack slot retained for matching.
    UNUSED s32 pad;
    s32 text_blue;
    s16 height = (opening_progress * 22) + 10;
    s16 y = ((186 - height) / 2) + 229;

    if (opening_progress > 0) {
        Ui_DrawBorderedPanelNoFill(137, y, 366, height);

        if (opening_progress == 8) {
            char* text;

            Options_DrawFilledPanel(144, y + 7, 352, 0x20, 0x3C, 0x3C, 0xA0, 0xFF); // #3C3CA0
            Options_DrawFilledPanel(144, y + 39, 352, 140, 0x1E, 0x1E, 0x64, 0xFF); // #1E1E64

            if (show_cursor != 0) {
                Ui_DrawAnimatedTextureMarker(149, y + (sSelectedOption * 32) + 49);
            }

            gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

            Options_DrawValueCapsule(360, y + 48);
            Options_DrawValueCapsule(360, y + 80);

            gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

            Font_BeginTranslucentTextRendering();
            Font_SetActive(0x10, 0);
            text = Text_GetString(NULL, 0, sOptionsText, 4);
            Font_Printf(320 - (Font_MeasureTextExtent(0x10, 0, text) / 2), y + 10, text);

            if (sSelectedOption == 0) {
                text_blue = 0;
            } else {
                text_blue = 0xFF;
            }

            Gfx_SetEnvColor(0xFF, 0xFF, text_blue, 0xFF);
            Font_Printf(187, y + 49, Text_GetString(NULL, 0, sOptionsText, 5));
            text = Text_GetString(NULL, 0, sOptionsText, (sPendingSoundSettings & 0x1) != 0);
            Font_Printf(420 - (Font_MeasureTextExtent(0x10, 0, text) / 2), y + 49, text);

            if (sSelectedOption == 1) {
                text_blue = 0;
            } else {
                text_blue = 0xFF;
            }

            Gfx_SetEnvColor(0xFF, 0xFF, text_blue, 0xFF);
            Font_Printf(187, y + 81, Text_GetString(NULL, 0, sOptionsText, 6));
            text = Text_GetString(NULL, 0, sOptionsText, ((sPendingSoundSettings & 0x2) != 0) + 2);
            Font_Printf(420 - (Font_MeasureTextExtent(0x10, 0, text) / 2), y + 81, text);

            if (sSelectedOption == 2) {
                text_blue = 0;
            } else {
                text_blue = 0xFF;
            }

            Gfx_SetEnvColor(0xFF, 0xFF, text_blue, 0xFF);
            Font_Printf(187, y + 113, Text_GetString(NULL, 0, sOptionsText, 7));

            if (sSelectedOption == 3) {
                text_blue = 0;
            } else {
                text_blue = 0xFF;
            }

            Gfx_SetEnvColor(0xFF, 0xFF, text_blue, 0xFF);
            Font_Printf(187, y + 145, Text_GetString(NULL, 0, sOptionsText, 8));
            Font_EndTexturedTextRendering();
        }
    }
}

/* Draws one of the two save-erasure warnings and its selected yes/no choice. */
void Options_DrawErasePrompt(s32 confirmation_step, s32 selected_choice, s32 opening_progress) {
    // FAKE: unused stack slot retained for matching.
    UNUSED s32 pad;
    s16 height = (opening_progress * 17) + 2;
    s16 y_start = ((138 - height) / 2) + 84;

    if (opening_progress > 0) {
        Ui_DrawBorderedPanel(81, y_start, 478, height, 0x6109); // #632121

        if (opening_progress == 8) {
            Ui_DrawAnimatedTextureMarker(281, y_start + (selected_choice * 28) + 72);
            Font_BeginTranslucentTextRendering();
            Font_SetActive(0x10, 0);
            Gfx_SetEnvColor(0xFF, 0xFF, 0x64, 0xFF); // #FFFF64

            if (confirmation_step == 0) {
                Font_SetLineHeight(28);
                // The N64 Game Pak data will be deleted. OK?
                Font_Printf(99, y_start + 12, Text_GetString(NULL, 0, sOptionsText, 9));
            } else {
                // "All GB items & Pokémon transferred to the\nN64 Game Pak will also be lost. Are you sure?"
                Font_SetLineHeight(28);
                Font_Printf(99, y_start + 12, Text_GetString(NULL, 0, sOptionsText, 10));
            }

            Gfx_SetEnvColor(0xFF, 0xFF, (selected_choice == 0) ? 0 : 0xFF, 0xFF);
            Font_Printf(321, y_start + 72, Text_GetString(NULL, 0, sOptionsText, 11));
            Gfx_SetEnvColor(0xFF, 0xFF, (selected_choice == 1) ? 0 : 0xFF, 0xFF);
            Font_Printf(321, y_start + 100, Text_GetString(NULL, 0, sOptionsText, 12));
            Font_EndTexturedTextRendering();
        }
    }
}

/* Draws the expanding completion message after save erasure. */
void Options_DrawEraseCompleteMessage(s32 opening_progress) {
    // FAKE: unused stack slot retained for matching.
    UNUSED s32 pad;
    s16 height = opening_progress * 9;
    s16 y = ((56 - height) / 2) + 130;

    if (opening_progress >= 2) {
        Ui_DrawBorderedPanel(135, y, 370, height, 0x6109); // #632121
        if (opening_progress == 8) {
            Font_BeginTranslucentTextRendering();
            Font_SetActive(0x10, 0);
            Gfx_SetEnvColor(0xFF, 0xFF, 0x64, 0xFF); // #FFFF64
            Font_Printf(155, y + 24, Text_GetString(NULL, 0, sOptionsText, 13));
            Font_EndTexturedTextRendering();
        }
    }
}

/* Renders one frame of the options panel and the active erasure dialog. */
void Options_DrawFrame(s32 panel_step, s32 dialog_step, s32 confirmation_step, s32 selected_choice) {
    BgStage_DrawFrame();
    Options_DrawBackground();
    Options_DrawMainPanel(panel_step, dialog_step == 0);

    if (confirmation_step < 2) {
        Options_DrawErasePrompt(confirmation_step, selected_choice, dialog_step);
    } else {
        Options_DrawEraseCompleteMessage(dialog_step);
    }

    BgStage_AdvanceFrame();
}

/* Runs one erasure confirmation, returning true for yes and false for no or cancellation. */
s32 Options_ConfirmSaveErasure(s32 confirmation_step) {
    s16 i;
    s32 confirmed;
    s32 awaiting_choice = 1;
    s16 selected_choice = 1;

    if (confirmation_step == 0) {
        for (i = 1; i < 8; i++) {
            Controller_PollInputs();
            Options_DrawFrame(8, i, confirmation_step, 1);
        }
    }

    while (awaiting_choice != 0) {
        Controller_PollInputs();
        if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DUP | BTN_DDOWN)) {
            Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
            selected_choice ^= 0x1;
        } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
            awaiting_choice = 0;
        } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_B)) {
            selected_choice = 1;
            awaiting_choice = 0;
        }
        Options_DrawFrame(8, 8, confirmation_step, selected_choice);
    }

    confirmed = selected_choice == 0;
    if (confirmed != 0) {
        if (confirmation_step == 0) {
            Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
        } else {
            Audio_PlaySoundEffectById(31);
        }
    } else {
        Audio_PlaySoundEffectById(SFX_MENU_BACK);
    }

    if ((confirmation_step == 1) || (selected_choice == 1)) {
        for (i = 7; i > 0; i--) {
            Controller_PollInputs();
            Options_DrawFrame(8, i, confirmation_step, selected_choice);
        }
    }
    return confirmed;
}

/* Opens the erasure-complete dialog, waits for A, and closes it. */
void Options_ShowEraseCompleteScreen(void) {
    s16 i;

    for (i = 1; i < 8; i++) {
        Controller_PollInputs();
        Options_DrawFrame(8, i, 2, 0);
    }

    do {
        Controller_PollInputs();
        Options_DrawFrame(8, 8, 2, 0);
    } while (!BTN_IS_PRESSED(gPlayer1Controller, BTN_A));

    Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);

    for (i = 7; i > 0; i--) {
        Controller_PollInputs();
        Options_DrawFrame(8, i, 2, 0);
    }
}

/* Resets saved records and round-two state, then restores the default audio settings. */
void Options_EraseSaveData(void) {
    s32 i;
    s32 j;

    for (i = 0; i < 9; i++) {
        Save_SetActiveRecordBank(i);

        for (j = 0; j < 12; j++) {
            Save_ResetAndCommitTypedRecord(16, j);
        }
    }

    for (j = 0; j < 12; j++) {
        Save_ResetAndCommitTypedRecord(17, j);
    }

    for (j = 0; j < 4; j++) {
        Save_ResetAndCommitTypedRecord(18, j);
    }

    Save_ResetAndCommitTypedRecord(19, 0);
    Save_ResetAndCommitTypedRecord(20, 0);
    Save_ResetAndCommitTypedRecord(21, 0);
    Save_ResetAndCommitTypedRecord(22, 0);
    Save_ResetAndCommitTypedRecord(23, 0);
    Save_FlushDirtyBanks();
    gSessionContext.inRound2 = 0;
    Audio_SetMonoOutput(0);
    Audio_SetVoiceEnabled(1);
    sPendingSoundSettings = 0;
}

/* Applies changed sound flags and commits them to the saved options. */
void Options_ApplySoundSettings(u16 pending_settings) {
    u16 current_settings;
    u16 changed_settings;

    Save_GetOptionsField02(&current_settings);
    changed_settings = pending_settings ^ current_settings;

    if (changed_settings != 0) {
        if (changed_settings & 0x1) {
            if (pending_settings & 0x1) {
                Audio_SetMonoOutput(1);
            } else {
                Audio_SetMonoOutput(0);
            }
        }

        if (changed_settings & 0x2) {
            if (pending_settings & 0x2) {
                Audio_SetVoiceEnabled(0);
            } else {
                Audio_SetVoiceEnabled(1);
            }
        }

        Save_SetOptionsField02(&pending_settings);
        Save_CommitTypedRecord(20, 0);
    }
}

/* Toggles a sound option, runs confirmed save erasure, or applies settings and requests exit. */
s16 Options_HandleSelection(void) {
    s16 should_exit = 0;

    switch (sSelectedOption) {
        case 0:
            Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
            sPendingSoundSettings ^= 0x1;
            break;

        case 1:
            Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
            sPendingSoundSettings ^= 0x2;
            break;

        case 2:
            Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
            if ((Options_ConfirmSaveErasure(0) != 0) && (Options_ConfirmSaveErasure(1) != 0)) {
                Options_EraseSaveData();
                Options_ShowEraseCompleteScreen();
            }
            break;

        case 3:
            Audio_PlaySoundEffectById(29);
            Options_ApplySoundSettings(sPendingSoundSettings);
            should_exit = 1;
            break;
    }

    return should_exit;
}

/* Runs the menu's fades, panel animation, navigation, and apply/cancel input. */
void Options_RunMenu(void) {
    s16 i;
    s16 exit_reason = 0;

    StageFade_StartFromOpaque(8);
    while (StageContext_GetFadeMode() != 0) {
        Controller_PollInputs();
        Options_DrawFrame(0, 0, 0, 0);
    }

    for (i = 1; i < 8; i++) {
        Controller_PollInputs();
        Options_DrawFrame(i, 0, 0, 0);
    }

    while (exit_reason == 0) {
        Controller_PollInputs();
        if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DUP)) {
            sSelectedOption = (sSelectedOption + 3) % 4;
            Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
        } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DDOWN)) {
            sSelectedOption = (sSelectedOption + 1) % 4;
            Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
        } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
            exit_reason = Options_HandleSelection();
        } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_B)) {
            exit_reason = 2;
            Audio_PlaySoundEffectById(SFX_MENU_BACK);
        }
        Options_DrawFrame(8, 0, 0, 0);
    }

    for (i = 7; i > 0; i--) {
        Controller_PollInputs();
        Options_DrawFrame(i, 0, 0, 0);
    }

    StageContext_SetClearColor(0xFFFF); // #FFFFFF
    StageFade_StartFromTransparent(8);

    while (StageContext_GetFadeMode() != 1) {
        Controller_PollInputs();
        Options_DrawFrame(0, 0, 0, 0);
    }
}

/* Loads the options screen, runs its menu, and releases its resources before returning. */
s32 Options_Main(UNUSED s32 arg0, UNUSED s32 arg1) {
    StageContext* stage;

    main_pool_push_state('PREF');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    stage = StageContext_Allocate(1, 0, 2, 0, 2, 1);
    Font_Init(0x1C, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_3000000, menu_select_ui, 0);
    ASSET_LOAD(D_2000000, common_menu2_ui, 0);

    Text_InitStringTables();
    sOptionsText = Text_GetStringTable(21);
    Save_EnsureAllBanksLoaded();
    Save_GetOptionsField02(&sPendingSoundSettings);
    sSelectedOption = 0;
    StageContext_Activate(stage);
    Options_RunMenu();
    StageContext_Deactivate();
    Save_FlushDirtyBanks();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('PREF');

    return 4;
}
