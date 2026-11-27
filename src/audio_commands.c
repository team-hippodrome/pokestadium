#include "audio_commands.h"
#include "src/audio_commands_category2.h"
#include "src/audio_commands.h"
#include "src/3D140.h"
#include "src/libnumus/player.h"
#include "src/rom_device.h"
#include "src/audio_channel.h"
#include "src/audio_cache.h"
#include "src/cry.h"
#include "src/audio_fade.h"

typedef struct unk_D_80077970 {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x03 */ u8 unk_03;
    /* 0x04 */ u8 unk_04;
} unk_D_80077970; // size = 0x5

static unk_D_80077970 D_80077970[151] = {
    { 1, 1, 0, 0x52, 4 },    { 1, 1, 0, 0x58, 0x36 }, { 2, 2, 0, 0x1A, 0x3C }, { 1, 1, 0, 0, 0 },
    { 1, 1, 0, 0x54, 0x3C }, { 2, 2, 1, 0x2E, 0xA },  { 1, 1, 0, 0x5C, 0 },    { 1, 1, 0, 0x44, 0x52 },
    { 2, 2, 0, 0x22, 0x36 }, { 1, 1, 0, 0x98, 0 },    { 1, 1, 0, 0x32, 0xA4 }, { 1, 1, 0, 0, 0 },
    { 1, 1, 0, 0x32, 0x9E }, { 1, 1, 0, 0xC, 0 },     { 1, 1, 0, 0x12, 0 },    { 1, 1, 0, 0x8C, 0x24 },
    { 1, 1, 0, 0x8C, 0x24 }, { 2, 1, 0, 0x8C, 0x24 }, { 1, 1, 0, 0x54, 0 },    { 1, 1, 0, 0x50, 0x34 },
    { 0, 1, 0, 0x3C, 0 },    { 9, 1, 0, 0x28, 0x10 }, { 2, 1, 0, 0x76, 0x94 }, { 2, 2, 0, 0x72, 0x3C },
    { 1, 0, 0, 0x4E, 0x3C }, { 1, 1, 0, 0x7E, 0x72 }, { 1, 1, 0, 0, 0x7E },    { 1, 1, 0, 0x3C, 0 },
    { 1, 1, 0, 0x34, 0x16 }, { 1, 1, 0, 0x3C, 0x5A }, { 2, 2, 0, 0, 0 },       { 1, 1, 0, 8, 0xA },
    { 1, 1, 0, 0x6E, 0x64 }, { 2, 2, 0, 0x40, 0 },    { 1, 0, 0, 0x54, 0x7E }, { 1, 0, 0, 0x2C, 0x66 },
    { 1, 0, 0, 0x72, 0x98 }, { 1, 1, 0, 0x2A, 0x78 }, { 9, 0, 0, 0x92, 0x2C }, { 1, 1, 0, 0x52, 0x3C },
    { 1, 1, 0, 0x46, 0 },    { 2, 2, 0, 0x54, 0x4A }, { 0, 0, 0, 0x38, 0x1E }, { 1, 1, 0, 0, 0x38 },
    { 9, 1, 0, 0x50, 0x7E }, { 1, 1, 0, 0x6E, 0 },    { 1, 1, 0, 0x8E, 0 },    { 1, 1, 0, 0x38, 0xBA },
    { 2, 1, 0, 0x3A, 0x36 }, { 0, 0, 0, 0x3C, 0x5E }, { 0, 1, 0, 0x3C, 0x54 }, { 1, 1, 0, 0x24, 0x88 },
    { 2, 1, 0, 0x5A, 8 },    { 1, 1, 0, 0x5A, 0 },    { 2, 1, 0, 0x38, 0x64 }, { 1, 1, 0, 0x64, 0x54 },
    { 1, 1, 0, 0, 0 },       { 1, 1, 0, 0x42, 0x18 }, { 2, 1, 0, 0x6A, 0x16 }, { 1, 0, 0, 0x32, 0x60 },
    { 1, 1, 0, 0xA4, 0x42 }, { 2, 1, 0, 0x38, 0x86 }, { 1, 1, 0, -0x34, 0 },   { 1, 1, 0, 0x5A, 0x6C },
    { 2, 1, 0, 0x46, 0 },    { 1, 1, 0, 0x14, 0xAE }, { 2, 1, 0, 0x64, 0x32 }, { 2, 2, 0, 0x6C, 0xB0 },
    { 0, 1, 0, 0x28, 0x92 }, { 1, 1, 0, 0x24, 0x34 }, { 2, 1, 0, 0x16, 0 },    { 1, 1, 0, 0x30, 0x9E },
    { 2, 2, 0, 0x20, 0 },    { 2, 1, 0, 0x7A, 0x7C }, { 2, 2, 0, 0, 0xC },     { 2, 2, 0, 0x3C, 0x2E },
    { 1, 0, 0, 0, 0 },       { 2, 1, 0, 0x28, 0x10 }, { 2, 1, 0, 0, 0x98 },    { 2, 1, 0, 0, 0x80 },
    { 1, 1, 0, 0x6A, 0x1A }, { 1, 1, 0, 0, 0x1A },    { 1, 1, 0, 0x64, 0xC },  { 1, 1, 0, 0, 0 },
    { 2, 1, 0, 0x28, 0 },    { 1, 1, 0, 0x54, 0x72 }, { 2, 1, 0, 0x6A, 0 },    { 9, 2, 0, 0x44, 0 },
    { 9, 2, 0, 0x40, 0x2A }, { 1, 1, 0, 0x7E, 0 },    { 2, 2, 0, 0, 0x7C },    { 0, 1, 0, 0x3E, 0x2E },
    { 0, 2, 0, 0x32, 0x33 }, { 0, 2, 0, 0x38, 0x32 }, { 2, 2, 0, 0x30, 0x3A }, { 1, 1, 0, 0x34, 0 },
    { 2, 1, 0, 0, 0 },       { 1, 1, 0, 0x58, 0 },    { 1, 1, 0, 0x46, 0 },    { 9, 1, 0, 0, 0 },
    { 9, 1, 0, 0x28, 0x36 }, { 1, 1, 0, 0x44, 0x2A }, { 2, 1, 0, 0x60, 0 },    { 1, 1, 0, 0xB4, 0x36 },
    { 1, 1, 0, 0x14, 0 },    { 2, 1, 0, 0x10, 0x72 }, { 2, 1, 0, 0x40, 0xB6 }, { 2, 1, 0, 0x3E, 0x2C },
    { 1, 1, 0, 0x50, 0x78 }, { 2, 2, 0, 0x20, 0x3C }, { 2, 1, 0, 0x4C, 0x3C }, { 2, 2, 0, 0x36, 0xC },
    { 1, 0, 0, 0x1E, 0 },    { 1, 1, 0, 0, 0x51 },    { 2, 1, 0, 0x8C, 0x22 }, { 0, 0, 0, 0x5C, 0x6E },
    { 1, 1, 0, 0x5A, 0x6E }, { 1, 0, 0, 0x7C, 0 },    { 1, 1, 0, 0, 0 },       { 1, 1, 0, 0x28, 0 },
    { 1, 1, 0, 0x1A, 0x64 }, { 1, 1, 0, 0x62, 0 },    { 1, 1, 0, 0x14, 0x1A }, { 9, 1, 0, 0x4A, 0 },
    { 2, 1, 0, 0xA, 0x5E },  { 2, 2, 0, 0x2E, 0 },    { 1, 1, 0, 0x14, 0 },    { 2, 2, 0, 0x8C, 0x62 },
    { 0, 1, 0, 0, 0x64 },    { 2, 2, 0, 0x8C, 0x1E }, { 2, 1, 0, 0x28, 0x82 }, { 9, 0, 0, 0, 0 },
    { 1, 0, 0, 0, 0x3C },    { 1, 0, 0, 0, 6 },       { 1, 0, 0, 0x40, 0xC },  { 1, 0, 0, 0x18, 0x8E },
    { 1, 1, 0, 0x3C, 0xE },  { 1, 1, 0, 0, 0 },       { 1, 1, 0, 0, 0x34 },    { 1, 1, 0, 0x32, 0x12 },
    { 1, 1, 0, 0x50, 0x42 }, { 9, 2, 0, 0x28, 0 },    { 2, 1, 0, 0x3C, 0x82 }, { 2, 1, 0, 0x32, 0x2A },
    { 2, 1, 0, 0x38, 0x2E }, { 2, 1, 0, 0x1E, 0x2A }, { 2, 0, 0, 0x23, 0x18 }, { 2, 1, 0, 0x28, 0 },
    { 2, 1, 0, 0x6C, 0 },    { 2, 1, 0, 0x64, 0xA },  { 0, 0, 0, 0x82, 0 },
};

static s32 D_80077C64 = 0;
static s32 D_80077C68 = 0;
static s32 D_80077C6C = 0;
static s32 D_80077C70 = 0;
static s32 D_80077C74 = 0;
static s32 D_80077C78 = 0;

#if defined(VERSION_FR) || defined(VERSION_DE) || defined(VERSION_ES) || defined(VERSION_IT)
extern s32 D_80078388;
// The translated releases play these sounds 0x10 quieter while the battle music stream plays.
#define SFX_STREAM_PLAYING() ((D_80078388 != 0) && (D_80078E78 == 0) && (sLoopPointTable[0] == 0))
#define SFX_SET_VOLUME(volume)                     \
    if (SFX_STREAM_PLAYING()) {                    \
        effect_priority = (volume) - 0x10;         \
    }
#define SFX_VOLUME(volume) effect_priority
#else
#define SFX_SET_VOLUME(volume)
#define SFX_VOLUME(volume) (volume)
#endif

s32 Audio_PlayCommand(u32 command, s32 parameter, UNUSED s32 unused) {
    static s32 D_80077C7C = 0;

    UNUSED s32 pad;
    u32 bank_offset;
    u32 sweep_amount;
    s32 effect_priority;
    s32 effect_id;
#if defined(VERSION_PAL)
    u32 command_bank;
    volatile s32 sound_handle;
#else
    UNUSED s32 pad2;
    volatile s32 sound_handle;
    u32 command_bank;
#endif

    command_bank = command & 0xFFFF0000;
    effect_priority = 0x80;

    if (command_bank != D_80079014) {
        bank_offset = D_800FC6F4->seqArray[0].offset;
        Rom_DmaRead(bank_offset, D_800FC798, 0xC);
        Audio_RelocateOffsets(D_800FC798, bank_offset, 3);
        Rom_DmaRead(D_800FC798[0], D_800FC6DC, D_800FC798[1] - D_800FC798[0]);
        Audio_CopyDataWithCacheSync(D_800FC6DC, D_800FC6A8, 0xBB8);
        Audio_RelocateSoundBankTable(D_800FC6A8);
        Rom_DmaRead(D_800FC798[1], D_800FC6DC, D_800FC798[2] - D_800FC798[1]);
        Audio_CopyDataWithCacheSync(D_800FC6DC, D_800FC6A4, 0x1388);
        Audio_RelocateSoundBankWrapper(D_800FC6A4, D_800FC798[2]);
    }

    D_80079014 = command_bank;

    switch (command) {
        case 0x0:
            if (Audio_IsSoundPlaying(D_80077C64) != 0) {
                return 0;
            }

            switch (parameter) {
                case 1:
                    effect_id = 1;
                    break;

                case 2:
                    effect_id = 2;
                    break;

                default:
                    return 0;
            }

#if defined(VERSION_FR) || defined(VERSION_DE) || defined(VERSION_ES) || defined(VERSION_IT)
            if (SFX_STREAM_PLAYING()) {
                effect_priority = 0x60;
            } else {
                effect_priority = 0x70;
            }
#endif
            D_80077C64 = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, effect_id, SFX_VOLUME(0x70), 0x80, -1);
            sound_handle = D_80077C64;
            break;

        case 0x1:
            if (parameter < -0x5A) {
                parameter = -0x5A;
            } else if (parameter >= 0x5B) {
                parameter = 0x5A;
            }

            do {
                sweep_amount = (parameter + 0x5A) * 1.4111111f;
            } while (0);

            if (sweep_amount > 0xFE) {
                sweep_amount = 0xFE;
            }
            SFX_SET_VOLUME(0x80);
            sound_handle = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, 3, SFX_VOLUME(0x80), sweep_amount, -1);
            break;

        case 0x2:
            if (Audio_IsSoundPlaying(D_80077C6C) != 0) {
                Audio_QueueFadeSoundCommand(D_80077C6C, 0x14);
                D_80077C6C = 0;
            }
            SFX_SET_VOLUME(0x80);
            sound_handle = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, 4, SFX_VOLUME(0x80), 0x80, -1);
            break;

        case 0x3:
            SFX_SET_VOLUME(0x80);
            D_80077C6C = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, 5, SFX_VOLUME(0x80), 0x80, -1);
            sound_handle = D_80077C6C;
            break;

        case 0x4:
            switch (parameter) {
                case 1:
                    effect_id = 1;
                    break;
                case 2:
                    effect_id = 2;
                    break;

                default:
                    return 0;
            }
            SFX_SET_VOLUME(0x80);
            D_80077C68 = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, effect_id, SFX_VOLUME(0x80), 0x80, -1);
            sound_handle = D_80077C68;
            break;

        case 0x6:
            SFX_SET_VOLUME(0x80);
            sound_handle = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, 6, SFX_VOLUME(0x80), 0x80, -1);
            break;

        case 0x7:
            SFX_SET_VOLUME(0x80);
            sound_handle = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, 7, SFX_VOLUME(0x80), 0x80, -1);
            break;

        case 0x8:
            return 0;

        case 0x33:
            if (parameter >= 0x98) {
                parameter = 3;
            }

            if ((parameter > 0) && (parameter < 0x98)) {
                parameter--;
            } else {
                parameter = 0;
            }

            switch (D_80077970[parameter].unk_00) {
                case 0:
                    return 0;

                case 1:
                    SFX_SET_VOLUME(0x80);
                    sound_handle = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, 7, SFX_VOLUME(0x80), 0x80, -1);
                    break;

                case 2:
                    SFX_SET_VOLUME(0x80);
                    sound_handle = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, 6, SFX_VOLUME(0x80), 0x80, -1);
                    break;

                default:
                    return 0;
            }
            break;

        case 0x32:
            if (parameter == 0xC8) {
                SFX_SET_VOLUME(0x80);
                return Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, 0x20, SFX_VOLUME(0x80), 0x80, -1);
            }

            if (parameter >= 0x98) {
                parameter = 2;
            }

            if ((parameter > 0) && (parameter < 0x98)) {
                parameter--;
            } else {
                parameter = 0;
            }

            switch (D_80077970[parameter].unk_01) {
                case 0:
                    SFX_SET_VOLUME(0x80);
                    sound_handle = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, 0xB, SFX_VOLUME(0x80), 0x80, -1);
                    break;

                case 1:
                    SFX_SET_VOLUME(0x80);
                    sound_handle = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, 0xA, SFX_VOLUME(0x80), 0x80, -1);
                    break;

                case 2:
                    SFX_SET_VOLUME(0x80);
                    sound_handle = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, 9, SFX_VOLUME(0x80), 0x80, -1);
                    break;
            }
            break;

        case 0x12:
            SFX_SET_VOLUME(0x80);
            sound_handle = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, 0xC, SFX_VOLUME(0x80), 0x80, -1);
            break;

        case 0x13:
            SFX_SET_VOLUME(0x80);
            sound_handle = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, 0xD, SFX_VOLUME(0x80), 0x80, -1);
            break;

        case 0x14:
            SFX_SET_VOLUME(0x80);
            sound_handle = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, 0xE, SFX_VOLUME(0x80), 0x80, -1);
            break;

        case 0x28:
#if defined(VERSION_PAL)
            SFX_SET_VOLUME(0x80);
            Audio_QueueVoiceVolume(sound_handle = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, 0xF, SFX_VOLUME(0x80), 0x80, -1), 0x6A);
#else
            SFX_SET_VOLUME(0x80);
            sound_handle = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, 0xF, SFX_VOLUME(0x80), 0x80, -1);
#endif
            break;

        case 0x5A:
            switch (parameter) {
                case 1:
                    effect_id = 0x12;
                    break;

                case 2:
                    effect_id = 0x13;
                    break;

                default:
                    return 0;
            }

            SFX_SET_VOLUME(0x80);
            D_80077C74 = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, effect_id, SFX_VOLUME(0x80), 0x80, -1);
            break;

        case 0x5B:
            switch (parameter) {
                case 1:
                    break;

                case 2:
                    return 0;

                default:
                    return 0;
            }
            SFX_SET_VOLUME(0x80);
            D_80077C78 = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, 0x14, SFX_VOLUME(0x80), 0x80, -1);
            break;

        case 0x15:
            switch (parameter) {
                case 1:
                    effect_id = 0x10;
                    break;

                case 2:
                    effect_id = 0x11;
                    break;

                default:
                    return 0;
            }
            SFX_SET_VOLUME(0x80);
            sound_handle = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, effect_id, SFX_VOLUME(0x80), 0x80, 0x80);
            break;

        case 0x16:
            switch (parameter) {
                case 1:
                    effect_id = 0x16;
                    break;

                case 2:
                    effect_id = 0x17;
                    break;

                default:
                    return 0;
            }

            if (Audio_IsSoundPlaying(D_80077C7C) != 0) {
                D_80077C7C = 0;
                sound_handle = D_80077C7C;
            } else {
                SFX_SET_VOLUME(0x80);
                D_80077C7C = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, effect_id, SFX_VOLUME(0x80), 0x80, 0x80);
                sound_handle = D_80077C7C;
            }
            break;

        case 0x3C:
            sound_handle = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, 0x18, 0x80, 0x80, -1);
            break;

        case 0x3D:
            switch (parameter) {
                case 3:
                    effect_id = 0x19;
                    break;

                case 2:
                    effect_id = 0x1A;
                    break;

                case 1:
                    effect_id = 0x1B;
                    break;

                case 0:
                    effect_id = 0x1C;
                    effect_priority = 0xFF;
                    break;

                default:
                    return 0;
            }
            sound_handle = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, effect_id, effect_priority, 0x80, -1);
            break;

        case 0x3E:
            if (parameter < 11) {
                if (parameter == 1 || parameter == 2) {
                    effect_id = 0x1D;
                } else {
                    effect_id = 0x1E;
                }
            } else {
                return 0;
            }
            sound_handle = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, effect_id, 0xFF, 0x80, -1);
            break;

        case 0x3F:
            sound_handle = Audio_PlaySoundEffect(D_800FC6A4, D_800FC6A8, 0x1F, 0x80, 0x80, -1);
            break;

        case 0x1E:
            sound_handle = Effect_TriggerMoveVisual(0xA6, 1, 0);
            break;

        case 0x1F:
            sound_handle = Effect_TriggerMoveVisual(0xA6, 1, 1);
            break;

        case 0x20:
            sound_handle = Effect_TriggerMoveVisual(0xA6, 1, 2);
            break;

        case 0x21:
            sound_handle = Effect_TriggerMoveVisual(0xA6, 1, 3);
            break;

        case 0x22:
            sound_handle = Effect_TriggerMoveVisual(0xA6, 1, 4);
            break;

        case 0x23:
            sound_handle = Effect_TriggerMoveVisual(0xA6, 1, 5);
            break;

        default:
            return 0;
    }

    return sound_handle;
}

s32 Audio_FadeCommand(s32 arg0, u32 arg1) {
    switch (arg0) {
        case 0x0:
            if (arg1 == 0) {
                arg1 = 1;
            }
            Audio_QueueFadeSoundCommand(D_80077C64, arg1);
            D_80077C64 = 0;
            break;

        case 0x4:
            if (arg1 == 0) {
                arg1 = 1;
            }
            Audio_QueueFadeSoundCommand(D_80077C68, arg1);
            break;

        case 0x3:
            Audio_QueueFadeSoundCommand(D_80077C6C, 0x3C);
            D_80077C6C = 0;
            break;

        case 0xA1:
            Audio_QueueFadeSoundCommand(D_80077C70, 0x78);
            D_80077C70 = 0;
            break;

        case 0x5A:
            Audio_QueueFadeSoundCommand(D_80077C74, 0x5A);
            D_80077C74 = 0;
            break;

        case 0x5B:
            Audio_QueueFadeSoundCommand(D_80077C78, 0x5A);
            D_80077C78 = 0;
            break;

        default:
            return 0;
    }

    return 1;
}

void func_8003733C(void) {
}
