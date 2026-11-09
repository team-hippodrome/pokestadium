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
static s16 sHoveredSetting;
static u16 sSoundVoiceSettingsMask;

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

void Options_DrawPanelBox(s16 left, s16 bottom, s16 right, s16 top, u8 r, u8 g, u8 b, u8 alpha) {
    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetEnvColor(gDisplayListHead++, r, g, b, alpha);

    Gfx_DrawTextureIa8(left, bottom, 8, 8, D_2000B40, 8, 0);
    Gfx_DrawTextureIa8((left + right) - 8, bottom, 8, 8, D_2000B80, 8, 0);
    Gfx_DrawTexturedRectClipped(left + 8, bottom, right - 16, 8, 0, 0, 0, 0x400, 0);
    Gfx_DrawTextureIa8(left, (bottom + top) - 8, 8, 8, D_2000BC0, 8, 0);
    Gfx_DrawTexturedRectClipped(left, bottom + 8, 8, top - 16, 0, 0, 0x400, 0, 0);
    Gfx_DrawTextureIa8((left + right) - 8, (bottom + top) - 8, 8, 8, D_2000C00, 8, 0);
    Gfx_DrawTexturedRectClipped(left + 8, (bottom + top) - 8, right - 16, 8, 0, 0, 0, 0x400, 0);
    Gfx_DrawTexturedRectClipped((left + right) - 8, bottom + 8, 8, top - 16, 0, 0, 0x400, 0, 0);

    gSPDisplayList(gDisplayListHead++, gUiTranslucentFillDL);

    Gfx_FillRectRgba(left + 8, bottom + 8, right - 16, top - 16, r, g, b, alpha);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void Options_DrawValueCapsule(s16 x, s16 y) {
    gDPSetEnvColor(gDisplayListHead++, 0x3C, 0x3C, 0xA0, 0xFF); // #3c3ca0

    Gfx_DrawTextureI4(x, y, 0x10, 28, D_30242C0, 0x10, 0);
    Gfx_DrawTextureI4(x + 104, y, 0x10, 28, D_30243A0, 0x10, 0);
    Gfx_DrawTexturedRectClipped(x + 0x10, y, 88, 28, 0, 0, 0, 0x400, 0);
}

void Options_DrawMainPanel(s16 opening_progress, s32 fill) {
    UNUSED s32 pad;
    s32 option_text_blue_channel;
    s16 x = (opening_progress * 22) + 10;
    s16 y = ((186 - x) / 2) + 229;

    if (opening_progress > 0) {
        Ui_DrawBorderedPanelNoFill(137, y, 366, x);

        if (opening_progress == 8) {
            char* text;

            Options_DrawPanelBox(144, y + 7, 352, 0x20, 0x3C, 0x3C, 0xA0, 0xFF);
            Options_DrawPanelBox(144, y + 39, 352, 140, 0x1E, 0x1E, 0x64, 0xFF);

            if (fill != 0) {
                Ui_DrawAnimatedTextureMarker(149, y + (sHoveredSetting * 0x20) + 49);
            }

            gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

            Options_DrawValueCapsule(360, y + 48);
            Options_DrawValueCapsule(360, y + 80);

            gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

            Font_BeginTranslucentTextRendering();
            Font_SetActive(0x10, 0);
            text = Text_GetString(NULL, 0, sOptionsText, 4);
            Font_Printf(320 - (Font_MeasureTextExtent(0x10, 0, text) / 2), y + 10, text);

            if (sHoveredSetting == 0) {
                option_text_blue_channel = 0;
            } else {
                option_text_blue_channel = 0xFF;
            }

            Gfx_SetEnvColor(0xFF, 0xFF, option_text_blue_channel, 0xFF);
            Font_Printf(187, y + 49, Text_GetString(NULL, 0, sOptionsText, 5));
            text = Text_GetString(NULL, 0, sOptionsText, (sSoundVoiceSettingsMask & 0x1) != 0);
            Font_Printf(420 - (Font_MeasureTextExtent(0x10, 0, text) / 2), y + 49, text);

            if (sHoveredSetting == 1) {
                option_text_blue_channel = 0;
            } else {
                option_text_blue_channel = 0xFF;
            }

            Gfx_SetEnvColor(0xFF, 0xFF, option_text_blue_channel, 0xFF);
            Font_Printf(187, y + 81, Text_GetString(NULL, 0, sOptionsText, 6));
            text = Text_GetString(NULL, 0, sOptionsText, ((sSoundVoiceSettingsMask & 0x2) != 0) + 2);
            Font_Printf(420 - (Font_MeasureTextExtent(0x10, 0, text) / 2), y + 81, text);

            if (sHoveredSetting == 2) {
                option_text_blue_channel = 0;
            } else {
                option_text_blue_channel = 0xFF;
            }

            Gfx_SetEnvColor(0xFF, 0xFF, option_text_blue_channel, 0xFF);
            Font_Printf(187, y + 113, Text_GetString(NULL, 0, sOptionsText, 7));

            if (sHoveredSetting == 3) {
                option_text_blue_channel = 0;
            } else {
                option_text_blue_channel = 0xFF;
            }

            Gfx_SetEnvColor(0xFF, 0xFF, option_text_blue_channel, 0xFF);
            Font_Printf(187, y + 145, Text_GetString(NULL, 0, sOptionsText, 8));
            Font_EndTexturedTextRendering();
        }
    }
}

void Options_DrawConfirmPrompt(s32 number_of_yeses, s32 hovering_yes, s32 opening_progress) {
    UNUSED s32 pad;
    s16 height = (opening_progress * 17) + 2;
    s16 y_start = ((138 - height) / 2) + 84;

    if (opening_progress > 0) {
        Ui_DrawBorderedPanel(81, y_start, 478, height, 0x6109);

        if (opening_progress == 8) {
            Ui_DrawAnimatedTextureMarker(281, y_start + (hovering_yes * 28) + 72);
            Font_BeginTranslucentTextRendering();
            Font_SetActive(0x10, 0);
            Gfx_SetEnvColor(0xFF, 0xFF, 0x64, 0xFF);

            if (number_of_yeses == 0) {
                Font_SetLineHeight(28);
                // The N64 Game Pak data will be deleted. OK?
                Font_Printf(99, y_start + 12, Text_GetString(NULL, 0, sOptionsText, 9));
            } else {
                // "All GB items & Pokémon transferred to the\nN64 Game Pak will also be lost. Are you sure?"
                Font_SetLineHeight(28);
                Font_Printf(99, y_start + 12, Text_GetString(NULL, 0, sOptionsText, 10));
            }

            Gfx_SetEnvColor(0xFF, 0xFF, (hovering_yes == 0) ? 0 : 0xFF, 0xFF);
            Font_Printf(321, y_start + 72, Text_GetString(NULL, 0, sOptionsText, 11));
            Gfx_SetEnvColor(0xFF, 0xFF, (hovering_yes == 1) ? 0 : 0xFF, 0xFF);
            Font_Printf(321, y_start + 100, Text_GetString(NULL, 0, sOptionsText, 12));
            Font_EndTexturedTextRendering();
        }
    }
}

void Options_DrawEraseCompleteMessage(s32 raw_height) {
    UNUSED s32 pad;
    s16 height = raw_height * 9;
    s16 y = ((56 - height) / 2) + 130;

    if (raw_height >= 2) {
        Ui_DrawBorderedPanel(135, y, 370, height, 0x6109);
        if (raw_height == 8) {
            Font_BeginTranslucentTextRendering();
            Font_SetActive(0x10, 0);
            Gfx_SetEnvColor(0xFF, 0xFF, 0x64, 0xFF); // #ffffff
            Font_Printf(155, y + 24, Text_GetString(NULL, 0, sOptionsText, 13));
            Font_EndTexturedTextRendering();
        }
    }
}

void Options_Draw(s32 main_open_prog, s32 delete_open_prog, s32 yeses, s32 hovering_yes_for_delete) {
    BgStage_DrawFrame();
    Options_DrawBackground();
    Options_DrawMainPanel(main_open_prog, delete_open_prog == 0);

    if (yeses < 2) {
        Options_DrawConfirmPrompt(yeses, hovering_yes_for_delete, delete_open_prog);
    } else {
        Options_DrawEraseCompleteMessage(delete_open_prog);
    }

    BgStage_AdvanceFrame();
}

// Better name: Options_ConfirmEraseSaveFilePrompt
s32 Options_ConfirmPrompt(s32 yeses) {
    s16 i;
    s32 temp_s2;
    s32 close_window = 1;
    s16 exited_deletion = 1;

    if (yeses == 0) {
        for (i = 1; i < 8; i++) {
            Controller_PollInputs();
            Options_Draw(8, i, yeses, 1);
        }
    }

    while (close_window != 0) {
        Controller_PollInputs();
        if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DUP | BTN_DDOWN)) {
            Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
            exited_deletion ^= 1;
        } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
            close_window = 0;
        } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_B)) {
            exited_deletion = 1;
            close_window = 0;
        }
        Options_Draw(8, 8, yeses, exited_deletion);
    }

    temp_s2 = exited_deletion == 0;
    if (temp_s2 != 0) {
        if (yeses == 0) {
            Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
        } else {
            Audio_PlaySoundEffectById(31);
        }
    } else {
        Audio_PlaySoundEffectById(SFX_MENU_BACK);
    }

    if ((yeses == 1) || (exited_deletion == 1)) {
        for (i = 7; i > 0; i--) {
            Controller_PollInputs();
            Options_Draw(8, i, yeses, exited_deletion);
        }
    }
    return temp_s2;
}

void Options_ShowEraseCompleteScreen(void) {
    s16 i;

    for (i = 1; i < 8; i++) {
        Controller_PollInputs();
        Options_Draw(8, i, 2, 0);
    }

    do {
        Controller_PollInputs();
        Options_Draw(8, 8, 2, 0);
    } while (!BTN_IS_PRESSED(gPlayer1Controller, BTN_A));

    Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);

    for (i = 7; i > 0; i--) {
        Controller_PollInputs();
        Options_Draw(8, i, 2, 0);
    }
}

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
    sSoundVoiceSettingsMask = 0;
}

void Options_ApplySoundSettings(u16 pending_settings) {
    u16 current_settings;
    u16 new_settings;

    Save_GetOptionsField02(&current_settings);
    new_settings = pending_settings ^ current_settings;

    if (new_settings != 0) {
        if (new_settings & 0x1) {
            if (pending_settings & 0x1) {
                Audio_SetMonoOutput(1);
            } else {
                Audio_SetMonoOutput(0);
            }
        }

        if (new_settings & 0x2) {
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

s16 Options_ApplySelection(void) {
    s16 exit_out = 0;

    switch (sHoveredSetting) {
        case 0:
            Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
            sSoundVoiceSettingsMask ^= 1;
            break;

        case 1:
            Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
            sSoundVoiceSettingsMask ^= 2;
            break;

        case 2:
            Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
            if ((Options_ConfirmPrompt(0) != 0) && (Options_ConfirmPrompt(1) != 0)) {
                Options_EraseSaveData();
                Options_ShowEraseCompleteScreen();
            }
            break;

        case 3:
            Audio_PlaySoundEffectById(29);
            Options_ApplySoundSettings(sSoundVoiceSettingsMask);
            exit_out = 1;
            break;
    }

    return exit_out;
}

void Options_Loop(void) {
    s16 i;
    s16 state = 0;

    StageFade_StartFromOpaque(8);
    while (StageContext_GetFadeMode() != 0) {
        Controller_PollInputs();
        Options_Draw(0, 0, 0, 0);
    }

    for (i = 1; i < 8; i++) {
        Controller_PollInputs();
        Options_Draw(i, 0, 0, 0);
    }

    while (state == 0) {
        Controller_PollInputs();
        if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DUP)) {
            sHoveredSetting = (sHoveredSetting + 3) % 4;
            Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
        } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DDOWN)) {
            sHoveredSetting = (sHoveredSetting + 1) % 4;
            Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
        } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
            state = Options_ApplySelection();
        } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_B)) {
            state = 2;
            Audio_PlaySoundEffectById(SFX_MENU_BACK);
        }
        Options_Draw(8, 0, 0, 0);
    }

    for (i = 7; i > 0; i--) {
        Controller_PollInputs();
        Options_Draw(i, 0, 0, 0);
    }

    StageContext_SetClearColor(0xFFFF);
    StageFade_StartFromTransparent(8);

    while (StageContext_GetFadeMode() != 1) {
        Controller_PollInputs();
        Options_Draw(0, 0, 0, 0);
    }
}

s32 Options_Main(UNUSED s32 arg0, UNUSED s32 arg1) {
    StageContext* sp24;

    main_pool_push_state('PREF');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    sp24 = StageContext_Allocate(1, 0, 2, 0, 2, 1);
    Font_Init(28, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_3000000, menu_select_ui, 0);
    ASSET_LOAD(D_2000000, common_menu2_ui, 0);

    Text_InitStringTables();
    sOptionsText = Text_GetStringTable(21);
    Save_EnsureAllBanksLoaded();
    Save_GetOptionsField02(&sSoundVoiceSettingsMask);
    sHoveredSetting = 0;
    StageContext_Activate(sp24);
    Options_Loop();
    StageContext_Deactivate();
    Save_FlushDirtyBanks();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('PREF');

    return 4;
}
