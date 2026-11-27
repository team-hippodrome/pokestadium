#include "audio_loop_point.h"
#include "src/libnumus/player.h"
#include "src/audio_channel.h"
#include "src/audio_commands_category2.h"
#include "src/rom_device.h"
#include "src/audio_stream.h"
#include "src/audio_cache.h"
#include "src/3D140.h"
#include "src/audio_commands.h"

static s32 D_800FF9B0;
static s32 D_800FF9B4;

// Interior aliases of sLoopPointTable and D_80078EBC, which IDO addresses separately.
extern u8 D_80078E83;
extern u8 D_80078E8B;
extern u8 D_80078E93;
extern u8 D_80078E9B;
extern u8 D_80078EA3;
extern u8 D_80078EAB;
extern u8 D_80078EB3;
extern u8 D_80078EBF;

// JP still defines these in gb_audio_render.
#if !defined(VERSION_JP)
s32 D_80078E70 = 0;
s32 D_80078E74 = 112;
u8 D_80078E78 = 0x00;
u8 D_80078E7C = 0x00;

u8 sLoopPointTable[0x3C] = {
    0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x04, 0x05, 0x23, 0x23, 0x23,
    0x25, 0x25, 0x25, 0x25, 0x0E, 0x23, 0x23, 0x23, 0x25, 0x25, 0x25, 0x0E, 0x0D, 0x06, 0x06,
    0x06, 0x37, 0x37, 0x37, 0x3B, 0x39, 0x34, 0x34, 0x34, 0x2F, 0x2F, 0x2F, 0x2E, 0x35, 0x0B,
    0x0B, 0x0B, 0x0C, 0x0C, 0x0C, 0x0C, 0x12, 0x0B, 0x0B, 0x0B, 0x0C, 0x0C, 0x0C, 0x12, 0x36,
};
u8 D_80078EBC[0xC] = {
    0x22, 0x22, 0x22, 0x24, 0x24, 0x24, 0x24, 0x24, 0x26, 0x00, 0x00, 0x00,
};
s32 D_80078EC8 = -1;
s32 D_80078ECC = 0;
u8 D_80078ED0 = 0x00;
s32 D_80078ED4 = 0;
s32 D_80078ED8 = 0;
s32 D_80078EDC = 0;
#else
extern u8 D_80078EBC[0xC];
#endif

void Audio_SetMusicLoopPoint(s32 arg0) {
    D_80078EC8 = arg0;
}

// The translated releases play these tracks 8 quieter.
#if defined(VERSION_FR) || defined(VERSION_DE) || defined(VERSION_ES) || defined(VERSION_IT)
#define TRACK_VOLUME(volume) ((volume) - 8)
#else
#define TRACK_VOLUME(volume) (volume)
#endif

s32 Audio_StartMusicTrack(s32 track_id) {
    u32 file_index;
    s32 file_size;
    UNUSED s32 pad;
    s32 volume_scale;
    s32 voice_volume;

    volume_scale = track_id;
    voice_volume = 0x80;
    AudioStream_SetEnabled(1);
    Audio_SetVoiceStartTimingMode(1);
    D_800FF9B4 = track_id;
    if (track_id == 0x50) {
        D_80078E78 = 0;
        D_80078E7C = 0;
        if (D_80078ED0 == 1) {
            Audio_StartVolumeTransition(D_80078ECC, 0, 0x80, 0x50);
            D_80078ED0 = 2;
            return 0;
        }
        Audio_StopAllSoundEffects();
        D_80078E70 = 0;
        Audio_QueueFadeSoundCommand(D_80078ECC, 1);
        D_80078ED0 = 0;
        D_80078ECC = Audio_PlayDirectSoundCommand(0x01000003, 0, 0);
        track_id = 0x28;
        D_80078ED0 = 2;
    } else if (track_id == 0x28) {
        D_80078E78 = 0;
        if (D_80078ED0 == 2) {
            Audio_StartVolumeTransition(D_80078ECC, 0x80, 0, 0x50);
            D_80078ED0 = 1;
            return 0;
        }
        Audio_StopAllSoundEffects();
        D_80078E70 = 0;
        Audio_QueueFadeSoundCommand(D_80078ECC, 1);
        D_80078ED0 = 0;
        D_80078ECC = Audio_PlayDirectSoundCommand(0x01000003, 0, 0);
        Audio_QueueVoiceVolumeScale(D_80078ECC, 0);
        D_80078ED0 = 1;
    } else {
        Audio_StopAllSoundEffects();
        D_80078E70 = 0;
        Audio_QueueFadeSoundCommand(D_80078ECC, 1);
        D_80078ED0 = 0;
    }

    if (track_id == 0x32) {
        if ((D_80078404 < 0xC) && (D_80078390[0] > 0) && (D_80078390[1] == 0) && (D_80078408 == 8)) {
            track_id = 0x3C;
        } else if ((D_80078404 == 0xD) && (D_80078390[0] > 0) && (D_80078390[1] == 0) && (D_80078408 == 5)) {
            track_id = 0x3E;
            D_800FF9B4 = 0x3E;
        } else if ((D_80078404 == 0xF) && (D_80078390[0] > 0) && (D_80078390[1] == 0)) {
            track_id = 0x40;
        }
    }

    if ((track_id == 0x3F) && (D_80078404 == 0xD) && (D_80078408 == 5)) {
        track_id = 0x41;
    }

    if ((track_id == 0x31) && (D_80078E78 != 0)) {
        track_id = 0x46;
    }

    if ((track_id == 0x3A) && (D_80078E7C != 0)) {
        track_id = 0x48;
    }

    file_index = track_id;
    switch (track_id) {
        case 0x0:
            D_80078E78 = 0;
            D_80078E7C = 0;
            sLoopPointTable[0] = 1;
            volume_scale = 0x98;
            break;

        case 0x1:
            volume_scale = 0x68;
            sLoopPointTable[0] = 1;
            Audio_SelectModeMusic(0, 0xF);
            break;

        case 0x9:
            volume_scale = 0x50;
            break;

        case 0x7:
            volume_scale = 0x78;
            break;

        case 0x8:
            volume_scale = 0xA0;
            break;

        case 0xF:
            volume_scale = 0x60;
            break;

        case 0xA:
            volume_scale = 0x60;
            break;

        case 0x11:
            volume_scale = 0x60;
            break;

        case 0x17:
            volume_scale = 0x60;
            break;

        case 0x18:
            volume_scale = 0x60;
            break;

        case 0x19:
            volume_scale = 0x58;
            break;

        case 0x1C:
            volume_scale = 0x60;
            break;

        case 0x1D:
            volume_scale = D_80078E74;
            break;

        case 0x1E:
            volume_scale = 0x65;
            break;

        case 0x1F:
            volume_scale = 0x70;
            break;

        case 0x20:
            volume_scale = 0x70;
            break;

        case 0x21:
            volume_scale = 0x50;
            break;

        case 0x1A:
            volume_scale = 0x60;
            Audio_ResetVolumeTransition();
            break;

        case 0x1B:
            volume_scale = 0x58;
            Audio_ResetVolumeTransition();
            break;

        case 0x2C:
            volume_scale = 0x70;
            break;

        case 0x13:
            volume_scale = 0x68;
            break;

        case 0x38:
            volume_scale = 0x60;
            break;

        case 0x15:
            volume_scale = 0x60;
            break;

        case 0x2D:
            volume_scale = 0x60;
            break;

        case 0x16:
            volume_scale = 0x60;
            break;

        case 0x10:
            volume_scale = 0x68;
            break;

        case 0x2A:
            volume_scale = 0x60;
            break;

        case 0x27:
            volume_scale = 0x58;
            break;

        case 0x2B:
            volume_scale = 0x60;
            break;

        case 0x30:
            D_80078E78 = 1;
            volume_scale = 0x64;
            break;

        case 0x28:
            D_80078E78 = 0;
            D_80078E7C = 0;
            volume_scale = 0x48;
            break;

        case 0x29:
            D_80078E78 = 0;
            D_80078E7C = 0;
            volume_scale = 0x78;
#if !defined(VERSION_PAL)
            voice_volume = 0x87;
#endif
            break;

        case 0x31:
            volume_scale = 0x60;
            break;

        case 0x33:
            volume_scale = 0x60;
            break;

        case 0x46:
            volume_scale = 0x60;
            break;

        case 0x32:
            volume_scale = 0x60;
#if defined(VERSION_PAL)
            voice_volume = 0x7C;
#endif
            break;

        case 0x3C:
            volume_scale = 0x60;
#if defined(VERSION_PAL)
            voice_volume = 0x75;
#endif
            break;

        case 0x3D:
            volume_scale = 0x88;
            break;

        case 0x3E:
            volume_scale = 0x60;
#if defined(VERSION_PAL)
            voice_volume = 0x75;
#endif
            break;

        case 0x3F:
            volume_scale = 0x60;
            break;

        case 0x40:
            volume_scale = 0x50;
#if defined(VERSION_PAL)
            voice_volume = 0x78;
#endif
            break;

        case 0x41:
            volume_scale = 0x60;
            break;

        case 0x45:
            volume_scale = 0x60;
#if defined(VERSION_PAL)
            voice_volume = 0x7C;
#endif
            break;

        case 0x44:
            volume_scale = 0x90;
            break;

        case 0x47:
            volume_scale = 0x88;
            D_80078E7C = 1;
            break;

        case 0x5B:
            volume_scale = 0x78;
            file_index = 1;
            break;

        case 0x3A:
            volume_scale = 0x68;
            Audio_SelectModeMusic(0, 0xF);
            Audio_PlayCategory11SoundCommand(0x01100016, 0, 0);
            break;

        case 0x48:
            Audio_SelectModeMusic(0, 0xF);
            Audio_PlayCategory11SoundCommand(0x01100016, 0, 0);
            volume_scale = 0x70;
            break;

        case 0x49:
            volume_scale = 0x68;
            break;

        case 0x4A:
            volume_scale = 0xA8;
            break;

        case 0x4B:
            volume_scale = 0x60;
            break;

        case 0x42:
            D_80078E78 = 0;
            D_80078E7C = 0;
            volume_scale = 0x80;
            break;

        case 0x43:
            D_80078E78 = 0;
            D_80078E7C = 0;
            volume_scale = 0x80;
            break;

        case 0x22:
            volume_scale = TRACK_VOLUME(0x48);
            break;

        case 0x24:
            volume_scale = TRACK_VOLUME(0x48);
            break;

        case 0x26:
            volume_scale = TRACK_VOLUME(0x40);
            break;

        case 0x23:
            volume_scale = TRACK_VOLUME(0x50);
            break;

        case 0x25:
            volume_scale = TRACK_VOLUME(0x50);
            break;

        case 0xE:
            volume_scale = TRACK_VOLUME(0x64);
            break;

        case 0xD:
            volume_scale = TRACK_VOLUME(0x48);
            break;

        case 0xB:
            volume_scale = TRACK_VOLUME(0x38);
            break;

        case 0xC:
            volume_scale = TRACK_VOLUME(0x38);
            break;

        case 0x12:
            volume_scale = TRACK_VOLUME(0x2E);
            break;

        case 0x36:
            volume_scale = TRACK_VOLUME(0x38);
            break;

        case 0x6:
            volume_scale = TRACK_VOLUME(0x38);
            break;

        case 0x37:
            volume_scale = TRACK_VOLUME(0x38);
            break;

        case 0x3B:
            volume_scale = TRACK_VOLUME(0x3C);
            break;

        case 0x39:
            volume_scale = TRACK_VOLUME(0x38);
            break;

        case 0x34:
            volume_scale = TRACK_VOLUME(0x60);
            break;

        case 0x2F:
            volume_scale = TRACK_VOLUME(0x58);
            break;

        case 0x2E:
            volume_scale = TRACK_VOLUME(0x54);
            break;

        case 0x35:
            volume_scale = TRACK_VOLUME(0x54);
            break;

        case 0x14:
            volume_scale = TRACK_VOLUME(0x58);
            break;

        case 0x2:
            volume_scale = TRACK_VOLUME(0x34);
            break;

        case 0x3:
            volume_scale = TRACK_VOLUME(0x44);
            break;

        case 0x4:
            volume_scale = TRACK_VOLUME(0x38);
            break;

        case 0x5:
            volume_scale = TRACK_VOLUME(0x34);
            break;

        case 0x5A:
            volume_scale = TRACK_VOLUME(0x50);
            sLoopPointTable[0] = 0;
            file_index = 2;
            break;

        default:
            return 0;
    }

    if (file_index < D_800FC6E4->num_files - 1) {
        file_size = D_800FC6E4->files[file_index + 1] - D_800FC6E4->files[file_index];
    } else if (file_index == D_800FC6E4->num_files - 1) {
        file_size = gAudioRegions[2] - D_800FC6E4->files[file_index];
    } else {
        return 0;
    }

    Rom_DmaRead(D_800FC6E4->files[file_index], D_800FC6DC, file_size);
    Audio_CopyDataWithCacheSync(D_800FC6DC, D_800FC680, 0x98D8);

    if (D_80078EC8 <= 0) {
        D_80078E70 = Audio_PlayMusicSequence(D_800FC684, D_800FC680);
    } else {
        D_80078E70 = Audio_PlayMusicSequenceLooped(D_800FC684, D_800FC680, D_80078EC8 ^ 0);
    }

    D_80078EC8 = -1;
    Audio_QueueVoiceVolumeScale(D_80078E70, volume_scale);

    if (voice_volume != 0x80) {
        Audio_QueueVoiceVolume(D_80078E70, voice_volume);
    }

    if (track_id == 0x1D) {
        D_800FF9B0 = D_80078E70;
    }

    return D_80078E70;
}

void Audio_FadeOutAll(s32 arg0) {
    s32 i;

    if (arg0 == 0) {
        arg0 = 1;
    }

    Audio_QueueFadeSoundCommand(D_80078E70, arg0);
    D_80078EC8 = -1;
    Audio_QueueFadeSoundCommand(D_80078ECC, arg0);
    D_80078ED0 = 0;
    D_8007840C = 0;
    Audio_QueueFadeSoundCommand(D_80078FA8, arg0);
    AudioStream_SetEnabled(0);
    AudioFrameUpdate(0);

    if (D_800FF9B4 != 0x50) {
        for (i = 0; i < 2; i++) {
            AudioStream_Stop(i, arg0);
        }
    }
}

s32 Audio_StartTrackForCategory(s32 arg0, s32 arg1) {
    s32 ret;
    sLoopPointTable[0] = 0;
    if ((arg0 == 0) || (arg0 == 0xFF)) {
        arg0 = 1;
        if ((arg1 == 0xE)) {
            return Audio_StartMusicTrack(2U);
        }
    }
    switch (arg1) {
        case 10:
        case 11:
            if (arg0 >= 9U) {
                return 0;
            }
            ret = Audio_StartMusicTrack(*(&D_80078E83 + arg0));
            break;
        case 6:
        case 7:
        case 8:
            if (arg0 >= 9U) {
                return 0;
            }
            ret = Audio_StartMusicTrack(*(&D_80078E8B + arg0));
            break;
        case 9:
            if (arg0 >= 9U) {
                return 0;
            }
            ret = Audio_StartMusicTrack(*(&D_80078E93 + arg0));
            break;
        case 5:
            if (arg0 >= 9U) {
                return 0;
            }
            ret = Audio_StartMusicTrack(*(&D_80078E9B + arg0));
            break;
        case 4:
            if (arg0 >= 9U) {
                return 0;
            }
            ret = Audio_StartMusicTrack(*(&D_80078EA3 + arg0));
            break;
        case 0:
        case 1:
        case 2:
            if (arg0 >= 9U) {
                return 0;
            }
            ret = Audio_StartMusicTrack(*(&D_80078EAB + arg0));
            break;
        case 3:
            if (arg0 >= 9U) {
                return 0;
            }
            ret = Audio_StartMusicTrack(*(&D_80078EB3 + arg0));
            break;
        case 12:
            if (arg0 >= 0x21U) {
                return 0;
            }
            ret = Audio_StartMusicTrack(D_80078EBC[(arg0 - 1) & 3]);
            break;
        case 13:
            if (arg0 >= 6U) {
                return 0;
            }
            ret = Audio_StartMusicTrack(*(&D_80078EBF + arg0));
            break;
        case 15:
            ret = Audio_StartMusicTrack(0x14U);
            break;
        default:
            ret = Audio_StartMusicTrack(2U);
    }
    return ret;
}

// PAL loops the special sequence's ending earlier and steps through it in smaller increments.
#if defined(VERSION_PAL)
#define SPECIAL_SEQ_LOOP_POINT 0x3A
#define SPECIAL_SEQ_BASE 0xB2
#define SPECIAL_SEQ_STEP 0x3C
#else
#define SPECIAL_SEQ_LOOP_POINT 0x46
#define SPECIAL_SEQ_BASE 0xD6
#define SPECIAL_SEQ_STEP 0x48
#endif

s32 Audio_UpdateSpecialSequence(s32 arg0, u32 arg1) {
    s32 var_a3;

    switch (arg0) {
        case 30:
            switch (D_80078ED4) {
                case 0:
                    D_80078ED4 = 1;
                    return Audio_StartMusicTrack(0x1E);

                case 1:
                    return 0;

                case 2:
                    return 0;

                case 3:
                    if (Audio_IsSoundPlaying(D_80078EDC) == 0) {
                        D_80078ED4 = 1;
                        if (arg1 == 0) {
                            return Audio_StartMusicTrack(0x1E);
                        }
                        return 0;
                    }
                    return 0;
            }
            return 0;

        case 31:
            D_80078ED4 = 2;
            switch (arg1) {
                case 0:
                    var_a3 = (arg1 * SPECIAL_SEQ_STEP) + SPECIAL_SEQ_BASE;
                    break;

                case 1:
                    var_a3 = (arg1 * SPECIAL_SEQ_STEP) + SPECIAL_SEQ_BASE;
                    break;

                case 2:
                    var_a3 = (arg1 * SPECIAL_SEQ_STEP) + SPECIAL_SEQ_BASE;
                    break;

                case 3:
                    var_a3 = (arg1 * SPECIAL_SEQ_STEP) + SPECIAL_SEQ_BASE;
                    break;

                case 4:
                    var_a3 = (arg1 * SPECIAL_SEQ_STEP) + SPECIAL_SEQ_BASE;
                    break;

                default:
                    var_a3 = SPECIAL_SEQ_BASE;
                    break;
            }

            switch (arg1) {
                case 0:
                    Audio_DispatchSoundBankCommand(0x9000D, 0, 0);
                    break;

                case 1:
                    Audio_DispatchSoundBankCommand(0x9000E, 0, 0);
                    break;

                case 2:
                    Audio_DispatchSoundBankCommand(0x9000F, 0, 0);
                    break;

                case 3:
                    Audio_DispatchSoundBankCommand(0x90010, 0, 0);
                    break;

                case 4:
                    Audio_DispatchSoundBankCommand(0x90011, 0, 0);
                    break;
            }

            Audio_SetMusicLoopPoint(var_a3);
            D_80078ED8 = Audio_StartMusicTrack(0x1F);
            return D_80078ED8;

        case 32:
            switch (D_80078ED4) {
                case 0:
                    return 0;

                case 1:
                    return 0;

                case 2:
                    if (Audio_IsSoundPlaying(D_80078ED8) == 0) {
                        if (arg1 == 0) {
                            D_80078ED4 = 3;
                            D_80078EDC = 0;
                        } else {
                            Audio_SetMusicLoopPoint(SPECIAL_SEQ_LOOP_POINT);
                            D_80078ED4 = 3;
                            D_80078EDC = Audio_StartMusicTrack(0x20);
                            return D_80078EDC;
                        }
                    }
                    return 0;

                case 3:
                    return 0;
            }
            return 0;
    }
}

u32 Audio_GetSpecialSequenceState(void) {
    return D_80078ED4;
}
