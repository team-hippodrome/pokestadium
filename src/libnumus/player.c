#include "player.h"
#include "src/3D140.h"
#include "src/libnaudio/n_synsetpriority.h"

typedef u8* (*command_func_t)(channel_t*, u8*);
typedef u8* (*api_func_t)(u8*);

static s32 mus_song_stop_countdown = -1;
static u8 mus_voice_start_immediate_mode = 0;
static command_func_t command_func_jumptable[] = {
    Audio_ClearVoicePlaybackState,
    Audio_SetVoiceSampleIndex,
    Audio_EnableVoicePitchSlide,
    Audio_DisableVoicePitchSlide,
    Audio_SetVoiceVolumeEnvelope,
    Audio_SetVoiceSequenceTempo,
    Audio_SetVoiceFixedDuration,
    Audio_SetVoiceReleaseDuration,
    Audio_EnableVoiceVibrato,
    Audio_EnableVoiceVibratoDown,
    Audio_DisableVoiceVibrato,
    Audio_SetVoiceDurationOverride,
    Audio_EnableVoiceVariableDuration,
    Audio_SetVoiceDetune,
    Audio_ApplyVoiceDetuneOnce,
    Audio_SetVoiceFinePitch,
    Audio_SetVoiceEnvelopeFromTable,
    Audio_SetVoiceFlagED,
    Audio_ClearVoiceFlagED,
    Audio_SetVoiceFlagEE,
    Audio_ClearVoiceFlagEE,
    Audio_PushVoiceLoopState,
    Audio_PopVoiceLoopState,
    Audio_SetVoicePeriodicPitchModulation,
    Audio_DisableVoicePeriodicPitchModulation,
    Audio_EnableVoiceVelocityFromStream,
    Audio_DisableVoiceVelocityFromStream,
    Audio_SetVoiceDefaultVelocity,
    Audio_SetVoicePan,
    Audio_SkipVoiceCommandWord,
    Audio_SetVoiceSubEnvelopePointer,
    Audio_ClearVoiceSubEnvelopePointer,
    Audio_SkipVoiceCommandByte,
    Audio_SetVoiceEnvelopePointers,
    Audio_SetVoiceFxMix,
    Audio_SetRandomVoiceDetune,
    Audio_SetRandomVoiceVolume,
    Audio_SetRandomVoicePan,
    Audio_SetVoiceVolume,
    Audio_AllocateSubVoiceFromCommand,
    Audio_SetVoiceVolumeScale,
    Audio_SetVoicePanModulation,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Audio_AllocateVoiceFromCommand,
};
static api_func_t api_func_jumptable[] = {
    Audio_PlayMusicSequenceHandler,
    Audio_PlaySoundEffectHandler,
    Audio_ApplyVoiceDetuneCommand,
    Audio_ApplyVoiceVolumeCommand,
    Audio_ApplyVoiceVolumeScaleCommand,
    Audio_ApplyVoicePanCommand,
    Audio_ApplyVoiceFxMixCommand,
    Audio_FadeSoundHandler,
    Audio_FadeCategoryHandler,
    Audio_SetChannelVolumeHandler,
    Audio_SetMonoOutputHandler,
    Audio_PlayMusicSequenceLoopedHandler,
    NULL,
};

static ALPlayer plr_player;
static ALHeap heap;
static s32 pad_D_800FC7C8;
static u32 max_channels;
static channel_t* mus_channels;
static u32 mus_vsyncs_per_second;
static u32 mus_next_frame_time;
static u16 mus_master_volume_effects;
static u16 mus_master_volume_songs;
static u32 mus_current_handle;
static s32 mus_random_seed;
static u8* mus_command_queue;
static s32 mus_command_queue_size;
static u32 mus_command_queue_max_size;
static u8 mus_use_fixed_pan_mode;
static u32 mus_current_frame_counter;
static audio_voice_t* mus_voices;
static audio_voice_t* mus_free_voice_1;
static audio_voice_t* mus_free_voice_2;
static audio_voice_t* mus_releasing_voice_3;
static audio_voice_t* mus_releasing_voice_4;
ALHeap* audio_heap;
u32 mus_num_vvoices;
s32 mus_free_voices_count;
s32 mus_releasing_voices_count;

u8* Audio_ClearVoicePlaybackState(channel_t* cp, UNUSED u8* ptr) {
    cp->pvolumebase = 0;
    cp->ppitchbendbase = 0;
    cp->song_addr = 0;
    cp->IsFX = 0;
    cp->handle = 0;
    return NULL;
}

u8* Audio_SetVoiceSampleIndex(channel_t* cp, u8* ptr) {
    u16 wave = *ptr++;

    if (wave & 0x80) {
        wave = (wave & 0x7F) << 8;
        wave |= *ptr++;
    }

    cp->wave = wave;
    return ptr;
}

u8* Audio_EnableVoicePitchSlide(channel_t* cp, u8* ptr) {
    cp->port = *ptr++;
    if (cp->port) {
        cp->port_base = cp->base_note;
    }
    return ptr;
}

u8* Audio_DisableVoicePitchSlide(channel_t* cp, u8* ptr) {
    cp->port = 0;
    return ptr;
}

u8* Audio_SetVoiceVolumeEnvelope(channel_t* cp, u8* ptr) {
    u8 value;

    value = *ptr++;
    if (value == 0) {
        value = 1;
    }
    cp->env_speed = value;
    cp->env_speed_calc = 0x400 / value;
    cp->env_init_vol = 0;

    ptr++;

    value = *ptr++;
    if (value == 0) {
        value = 1;
    }
    cp->env_attack_speed = value;
    cp->env_max_vol = *ptr++;
    cp->env_attack_calc = (1.0 / (f32)value) * (f32)(cp->env_max_vol - cp->env_init_vol);

    value = *ptr++;
    if (value == 0) {
        value = 1;
    }
    cp->env_decay_speed = value;
    cp->env_sustain_vol = *ptr++;
    cp->env_decay_calc = (1.0f / value) * ((f32)cp->env_sustain_vol - cp->env_max_vol);

    value = *ptr++;
    if (value == 0) {
        value = 1;
    }
    cp->env_release_speed = value;

    return ptr;
}

u8* Audio_SetVoiceSequenceTempo(channel_t* cp, u8* ptr) {
    s32 i;
    s32 temp_lo;
    s32 temp2;
    channel_t* sp;

    temp_lo = ((*ptr++ * 0x6000) / 120) / mus_vsyncs_per_second;
    temp2 = (cp->temscale * temp_lo) >> 7;

    if (cp->IsFX != 0) {
        cp->channel_tempo = temp_lo;
    } else {
        for (i = 0, sp = mus_channels; i < max_channels; i++, sp++) {
            if (sp->song_addr == cp->song_addr) {
                sp->channel_tempo_save = temp_lo;
                sp->channel_tempo = temp2;
            }
        }
    }

    return ptr;
}

u8* Audio_SetVoiceReleaseDuration(channel_t* cp, u8* ptr) {
    cp->endit = *ptr++;
    cp->cutoff = 0;
    return ptr;
}

u8* Audio_SetVoiceFixedDuration(channel_t* cp, u8* ptr) {
    s16 tmp = *ptr++ << 8;

    tmp |= *ptr++;

    cp->cutoff = tmp;
    cp->endit = 0;

    return ptr;
}

u8* Audio_EnableVoiceVibrato(channel_t* cp, u8* ptr) {
    cp->vib_delay = *ptr++;
    cp->vib_speed = *ptr++;
    cp->vib_amount = (f32)*ptr++ / 50.0;
    return ptr;
}

u8* Audio_EnableVoiceVibratoDown(channel_t* cp, u8* ptr) {
    cp->vib_delay = *ptr++;
    cp->vib_speed = *ptr++;
    cp->vib_amount = -(f32)*ptr++ / 50.0;

    return ptr;
}

u8* Audio_DisableVoiceVibrato(channel_t* cp, u8* ptr) {
    cp->vib_speed = 0;
    cp->vibrato = 0.0f;
    return ptr;
}

u8* Audio_SetVoiceDurationOverride(channel_t* cp, u8* ptr) {
    u8 value = *ptr++;

    if (value < 0x80) {
        cp->fixed_length = value;
    } else {
        cp->fixed_length = (value & 0x7F) << 8;
        cp->fixed_length += *ptr++;
    }
    return ptr;
}

u8* Audio_EnableVoiceVariableDuration(channel_t* cp, u8* ptr) {
    cp->ignore = 1;
    return ptr;
}

u8* Audio_SetVoiceDetune(channel_t* cp, u8* ptr) {
    cp->transpose = *ptr++;
    return ptr;
}

u8* Audio_ApplyVoiceDetuneOnce(channel_t* cp, u8* ptr) {
    cp->ignore_transpose = 1;
    return ptr;
}

u8* Audio_SetVoiceFinePitch(channel_t* cp, u8* ptr) {
    s32 value = *ptr++;

    if (value & 0x80) {
        value |= ~0xFF;
    }
    cp->freqoffset -= cp->distort;
    cp->distort = (f32)value / 100.0;
    cp->freqoffset += cp->distort;
    return ptr;
}

u8* Audio_SetVoiceEnvelopeFromTable(channel_t* cp, u8* ptr) {
    s32 value = *ptr++;

    if (value & 0x80) {
        value = (value & 0x7F) << 8;
        value |= *ptr++;
    }

    Audio_SetVoiceVolumeEnvelope(cp, (u8*)&cp->song_addr->env_table[value]);
    return ptr;
}

u8* Audio_SetVoiceFlagED(channel_t* cp, u8* ptr) {
    cp->env_trigger_off = 1;
    return ptr;
}

u8* Audio_ClearVoiceFlagED(channel_t* cp, u8* ptr) {
    cp->env_trigger_off = 0;
    return ptr;
}

u8* Audio_SetVoiceFlagEE(channel_t* cp, u8* ptr) {
    cp->trigger_off = 1;
    return ptr;
}

u8* Audio_ClearVoiceFlagEE(channel_t* cp, u8* ptr) {
    cp->trigger_off = 0;
    return ptr;
}

u8* Audio_PushVoiceLoopState(channel_t* cp, u8* ptr) {
    u8 index = cp->for_stack_count;

    cp->for_count[index] = *ptr++;

    cp->for_stack[index] = ptr;
    cp->for_stackvol[index] = cp->pvolumebase;
    cp->for_stackpb[index] = cp->ppitchbendbase;

    cp->for_volume[index] = cp->volume;
    cp->for_pitchbend[index] = cp->pitchbend;

    cp->for_vol_count[index] = cp->cont_vol_repeat_count;
    cp->for_pb_count[index] = cp->cont_pb_repeat_count;

    cp->for_stack_count++;
    return ptr;
}

u8* Audio_PopVoiceLoopState(channel_t* cp, u8* ptr) {
    s32 index = cp->for_stack_count - 1;

    if (cp->for_count[index] != 0xFF) {
        cp->for_count[index]--;
        if (!cp->for_count[index]) {
            cp->for_stack_count = index;
            index = -1;
        }
    }

    if (index >= 0) {
        ptr = cp->for_stack[index];

        cp->pvolumebase = cp->for_stackvol[index];
        cp->ppitchbendbase = cp->for_stackpb[index];
        cp->volume = cp->for_volume[index];

        cp->pitchbend = cp->for_pitchbend[index];
        cp->cont_vol_repeat_count = cp->for_vol_count[index];
        cp->cont_pb_repeat_count = cp->for_pb_count[index];
        cp->bendrange = 0.03125f;
        cp->pitchbend_calc = cp->pitchbend * cp->bendrange;
    }

    return ptr;
}

u8* Audio_SetVoicePeriodicPitchModulation(channel_t* cp, u8* ptr) {
    cp->wobble_amount = *ptr++;
    cp->wobble_on_speed = *ptr++;
    cp->wobble_off_speed = *ptr++;
    return ptr;
}

u8* Audio_DisableVoicePeriodicPitchModulation(channel_t* cp, u8* ptr) {
    cp->wobble_on_speed = 0;
    return ptr;
}

u8* Audio_EnableVoiceVelocityFromStream(channel_t* cp, u8* ptr) {
    cp->velocity_on = 1;
    return ptr;
}

u8* Audio_DisableVoiceVelocityFromStream(channel_t* cp, u8* ptr) {
    cp->velocity_on = 0;
    return ptr;
}

u8* Audio_SetVoiceDefaultVelocity(channel_t* cp, u8* ptr) {
    cp->default_velocity = *ptr++;
    cp->velocity_on = 0;
    return ptr;
}

u8* Audio_SetVoicePan(channel_t* cp, u8* ptr) {
    cp->pan = *ptr++ / 2;
    return ptr;
}

u8* Audio_SkipVoiceCommandWord(UNUSED channel_t* cp, u8* ptr) {
    ptr++;
    ptr++;
    return ptr;
}

u8* Audio_SetVoiceSubEnvelopePointer(channel_t* cp, u8* ptr) {
    u8* addr = (u8*)&cp->song_addr->num_channels + cp->song_addr->drum_table[*ptr++];

    cp->pdrums = addr;
    return ptr;
}

u8* Audio_ClearVoiceSubEnvelopePointer(channel_t* cp, u8* ptr) {
    cp->pdrums = NULL;
    return ptr;
}

u8* Audio_SkipVoiceCommandByte(UNUSED channel_t* cp, u8* ptr) {
#ifdef _AUDIODEBUG
    osSyncPrintf("PLAYER_COMMANDS.C: Audio_SkipVoiceCommandByte -  %d (channel frame=%d)\n", *ptr++, cp->channel_frame);
    return ptr;
#else
    ptr++;
    return ptr;
#endif
}

u8* Audio_SetVoiceEnvelopePointers(channel_t* cp, u8* ptr) {
    s32 tmp0;
    s32 tmp2;

    tmp0 = *ptr++ << 8;
    tmp0 += *ptr++;

    tmp2 = *ptr++ << 8;
    tmp2 += *ptr++;

    cp->pvolumebase = &cp->pvolumebase_offset[tmp2];
    cp->cont_vol_repeat_count = 1;

    tmp2 = *ptr++ << 8;
    tmp2 += *ptr++;

    cp->ppitchbendbase = &cp->ppitchbendbase_offset[tmp2];
    cp->cont_pb_repeat_count = 1;

    return &cp->pbase[tmp0];
}

u8* Audio_SetVoiceFxMix(channel_t* cp, u8* ptr) {
    cp->reverb = *ptr++;
    return ptr;
}

u8* Audio_SetRandomVoiceDetune(channel_t* cp, u8* ptr) {
    cp->transpose = Audio_RandomScale(*ptr++);
    cp->transpose += *ptr++;
    return ptr;
}

u8* Audio_SetRandomVoiceVolume(channel_t* cp, u8* ptr) {
    cp->volume = Audio_RandomScale(*ptr++);
    cp->volume += *ptr++;
    return ptr;
}

u8* Audio_SetRandomVoicePan(channel_t* cp, u8* ptr) {
    cp->pan = Audio_RandomScale(*ptr++);
    cp->pan += *ptr++;
    return ptr;
}

u8* Audio_SetVoiceVolume(channel_t* cp, u8* ptr) {
    cp->volume = *ptr++;
    return ptr;
}

u8* Audio_AllocateSubVoiceFromCommand(channel_t* cp, u8* ptr) {
    s32 i;
    s32 num;
    channel_t* sp;
    u32* fx_data;

    num = *ptr++;
    if (num >= 0x80) {
        num = *ptr++ + ((num & 0x7F) << 8);
    }

    fx_data = cp->fx_bank->fx_data;
    for (i = 0, sp = mus_channels; i < max_channels; i++, sp++) {
        if (sp->pdata == NULL) {
            Audio_InitializeVoice(sp, num, cp->volume_scale, cp->sweep_amount, cp->priority);
            sp->handle = cp->handle;
            sp->sample_bank = cp->sample_bank;
            sp->fx_bank = cp->fx_bank;
            sp->duration = cp->duration;
            sp->duration_scale = cp->duration_scale;
            sp->pdata = sp->pbase = fx_data[num];
            break;
        }
    }

    return ptr;
}

u8* Audio_SetVoiceVolumeScale(channel_t* cp, u8* ptr) {
    cp->bendrange = (f32)*ptr++ * 0.015625;
    cp->pitchbend_calc = SQ(cp->pitchbend);
    return ptr;
}

u8* Audio_SetVoicePanModulation(channel_t* cp, u8* ptr) {
    cp->sweep_speed = *ptr++;
    if (!cp->sweep_speed) {
        cp->sweep_amount = 0x80;
    }
    return ptr;
}

// Same as Audio_AllocateSubVoiceFromCommand
u8* Audio_AllocateVoiceFromCommand(channel_t* cp, u8* ptr) {
    s32 i;
    s32 num;
    channel_t* sp;
    u32* fx_data;

    num = *ptr++;
    if (num >= 0x80) {
        num = *ptr++ + ((num & 0x7F) << 8);
    }

    fx_data = cp->fx_bank->fx_data;
    for (i = 0, sp = mus_channels; i < max_channels; i++, sp++) {
        if (sp->pdata == NULL) {
            Audio_InitializeVoice(sp, num, cp->volume_scale, cp->sweep_amount, cp->priority);
            sp->handle = cp->handle;
            sp->sample_bank = cp->sample_bank;
            sp->fx_bank = cp->fx_bank;
            sp->duration = cp->duration;
            sp->duration_scale = cp->duration_scale;
            sp->pdata = sp->pbase = fx_data[num];
            break;
        }
    }

    return ptr;
}

/**
 * Write a big endian 32-bit value
 */
void Audio_WriteU32BE(u8* dst, u32 value) {
    *dst++ = (value >> 0x18) & 0xFF;
    *dst++ = (value >> 0x10) & 0xFF;
    *dst++ = (value >> 8) & 0xFF;
    *dst++ = (value)&0xFF;
}

/**
 * Write a big endian 16-bit value
 */
void Audio_WriteU16BE(u8* dst, u32 value) {
    *dst++ = (value >> 8u) & 0xFF;
    *dst++ = value & 0xFF;
}

s32 Audio_InitializeVoiceManager(musConfig* config) {
    UNUSED s32 pad[1];
    s32 i;
    ALSynConfig sp4C;
    amConfig sp40;

    sp4C.outputRate = osAiSetFrequency(config->syn_output_rate);
    max_channels = config->channels;
    if (osTvType == OS_TV_PAL) {
        mus_vsyncs_per_second = 50;
    } else {
        mus_vsyncs_per_second = 60;
    }
    mus_next_frame_time = 1000000 / mus_vsyncs_per_second;

    Audio_FillBytes(config->heap, 0, config->heap_length);
    alHeapInit(&heap, config->heap, config->heap_length);
    audio_heap = &heap;
    mus_channels = alHeapDBAlloc(NULL, 0, &heap, 1, max_channels * sizeof(channel_t));
    Audio_FillBytes((u8*)mus_channels, 0, max_channels * sizeof(channel_t));

    mus_command_queue = alHeapDBAlloc(NULL, 0, &heap, 1, config->command_queue_max_size);
    mus_command_queue_size = 0;
    mus_command_queue_max_size = config->command_queue_max_size;

    mus_voices = alHeapDBAlloc(NULL, 0, &heap, config->vvoices, sizeof(audio_voice_t));
    mus_free_voice_1 = mus_free_voice_2 = NULL;
    mus_releasing_voice_3 = mus_releasing_voice_4 = NULL;
    mus_free_voices_count = 0;
    mus_releasing_voices_count = 0;

    for (i = 0; i < config->vvoices; i++) {
        Audio_QueueFreeVoice(mus_voices++);
    }

    sp4C.maxVVoices = config->vvoices;
    sp4C.maxPVoices = config->pvoices;
    sp4C.maxUpdates = config->syn_updates;
    sp4C.dmaproc = NULL;
    sp4C.fxType = AL_FX_BIGROOM;
    sp4C.heap = &heap;

    sp40.outputRate = config->syn_output_rate;
    sp40.framesPerField = config->syn_retraceCount;
    sp40.maxACMDSize = config->syn_rsp_cmds;

    amCreateAudioMgr(&sp4C, &sp40, config->syn_num_dma_bufs, config->syn_dma_buf_size, mus_vsyncs_per_second);

    mus_num_vvoices = config->vvoices;
    mus_current_handle = 1;
    mus_current_frame_counter = 0;
    mus_master_volume_songs = 0x7FFF;
    mus_master_volume_effects = 0x7FFF;

    mus_song_stop_countdown = -1;
    mus_sound_effect_blocked = 0;
    mus_voice_start_immediate_mode = 1;

    plr_player.next = NULL;
    plr_player.handler = Audio_PlayerFrameHandler;
    plr_player.clientData = &plr_player;

    n_alSynAddPlayer(&plr_player);

    for (i = 0; i < max_channels; i++) {
        mus_channels[i].playing = 0;
        mus_channels[i].pdata = 0;
        Audio_ResetVoiceState(&mus_channels[i]);
    }

    return heap.cur - heap.base;
}

/**
 * Queue set master volume command
 */
void Audio_QueueSetVolumeCommand(s32 volume_type_flags, u32 master_volume_level) {
    u8* queue;
    u32 mask = osSetIntMask(1);

    queue = mus_command_queue;
    queue += mus_command_queue_size;

    *queue++ = 0x9;
    *queue++ = volume_type_flags;
    Audio_WriteU16BE(queue, master_volume_level);
    queue += 2;

    mus_command_queue_size += 4;
    if (mus_command_queue_size >= mus_command_queue_max_size) {
        mus_command_queue_size -= 4;
    }

    osSetIntMask(mask);
}

/**
 * Queue load song command (returns handle)
 */
s32 Audio_PlayMusicSequence(SoundBank* sample_bank_ptr, song_t* song_data) {
    u8* queue;
    u32 song_handle;
    UNUSED s32 pad;
    u32 mask;

    if (sample_bank_ptr == 0) {
        return 0;
    }

    if (!(sample_bank_ptr->flags & 1)) {
        return 0;
    }

    mask = osSetIntMask(1);

    queue = mus_command_queue;
    queue += mus_command_queue_size;

    song_handle = mus_current_handle;
    mus_current_handle++;

    *queue++ = 0x0;
    Audio_WriteU32BE(queue, sample_bank_ptr);
    queue += 4;
    Audio_WriteU32BE(queue, song_data);
    queue += 4;
    Audio_WriteU32BE(queue, song_handle);
    queue += 4;

    mus_command_queue_size += 0xD;
    if (mus_command_queue_size >= mus_command_queue_max_size) {
        mus_command_queue_size -= 0xD;
    }

    osSetIntMask(mask);
    return song_handle;
}

/**
 * Queue start FX command with priority (returns handle)
 */
s32 Audio_PlaySoundEffect(SoundBank* sample_bank_ptr, fx_t* fx_bank_ptr, s32 effect_id, s32 effect_priority,
                          u32 effect_sweep_amount, s32 effect_volume_scale) {
    u32 effect_handle;
    UNUSED s32 pad[2];
    u32 mask;
    u8* queue;

    if (mus_sound_effect_blocked != 0) {
        return 0;
    }

    if (sample_bank_ptr == NULL) {
        return 0;
    }

    if (!(sample_bank_ptr->flags & 1)) {
        return 0;
    }

    mask = osSetIntMask(1);

    queue = mus_command_queue;
    queue += mus_command_queue_size;

    effect_handle = mus_current_handle;
    mus_current_handle++;

    *queue++ = 0x1;
    Audio_WriteU32BE(queue, sample_bank_ptr);
    queue += 4;
    Audio_WriteU32BE(queue, fx_bank_ptr);
    queue += 4;
    Audio_WriteU32BE(queue, effect_handle);
    queue += 4;
    Audio_WriteU32BE(queue, effect_id);
    queue += 4;

    *queue++ = effect_priority;
    *queue++ = effect_sweep_amount;
    *queue++ = effect_volume_scale;

    mus_command_queue_size += 0x14;
    if (mus_command_queue_size >= mus_command_queue_max_size) {
        mus_command_queue_size -= 0x14;
    }

    osSetIntMask(mask);
    return effect_handle;
}

/**
 * Queue set duration for all channels by type flag
 */
void Audio_QueueFadeCategoryCommand(s32 channel_type_flags, u32 duration_frames) {
    u8* queue;
    u32 mask = osSetIntMask(1);

    queue = mus_command_queue;
    queue += mus_command_queue_size;

    *queue++ = 0x8;
    *queue++ = channel_type_flags;
    Audio_WriteU16BE(queue, duration_frames);
    queue += 2;

    mus_command_queue_size += 4;
    if (mus_command_queue_size >= mus_command_queue_max_size) {
        mus_command_queue_size -= 4;
    }

    osSetIntMask(mask);
}

s32 Audio_CountActiveSoundsByCategory(s32 flags) {
    s32 i;
    s32 count;
    channel_t* cp;
    u32 mask;

    mask = osSetIntMask(1);

    for (i = 0, count = 0, cp = mus_channels; i < max_channels; i++, cp++) {
        if (cp->pdata != NULL) {
            if (((cp->IsFX != 0) && (flags & MUSFLAG_EFFECTS)) || ((cp->IsFX == 0) && (flags & MUSFLAG_SONGS))) {
                count++;
            }
        }
    }

    osSetIntMask(mask);
    return count;
}

void Audio_QueueFadeSoundCommand(s32 channel_handle, u32 duration_frames) {
    UNUSED s32 pad;
    u8* queue;
    u32 mask;

    if (channel_handle != 0) {
        mask = osSetIntMask(1);

        queue = mus_command_queue;
        queue += mus_command_queue_size;

        *queue++ = 0x7;
        Audio_WriteU32BE(queue, channel_handle);
        queue += 4;
        Audio_WriteU16BE(queue, (u16)duration_frames);

        mus_command_queue_size += 7;
        if (mus_command_queue_size >= mus_command_queue_max_size) {
            mus_command_queue_size -= 7;
        }

        osSetIntMask(mask);
    }
}

s32 Audio_IsSoundPlaying(s32 handle) {
    channel_t* cp;
    s32 i;
    s32 count;
    u32 mask;

    if (handle == 0) {
        return 0;
    }

    mask = osSetIntMask(1);

    for (i = 0, count = 0, cp = mus_channels; i < max_channels; i++, cp++) {
        if (handle == cp->handle) {
            count++;
        }
    }

    osSetIntMask(mask);

    return count;
}

/**
 * Queue set volume scale for handle
 */
void Audio_QueueVoiceVolumeScale(s32 channel_handle, u32 volume_scale_factor) {
    u8* queue;
    UNUSED s32 pad[2];
    u32 mask;

    if (channel_handle != 0) {
        mask = osSetIntMask(1);

        queue = mus_command_queue;
        queue += mus_command_queue_size;

        *queue++ = 0x4;
        Audio_WriteU32BE(queue, channel_handle);
        queue += 4;
        Audio_WriteU16BE(queue, (u16)volume_scale_factor);

        mus_command_queue_size += 7;
        if (mus_command_queue_size >= mus_command_queue_max_size) {
            mus_command_queue_size -= 7;
        }

        osSetIntMask(mask);
    }
}

/**
 * Queue set sweep/modulation for handle
 */
void Audio_QueueVoicePan(s32 channel_handle, u16 sweep_modulation_value) {
    u8* queue;
    UNUSED s32 pad[2];
    u32 mask;

    if (channel_handle != 0) {
        mask = osSetIntMask(1);

        queue = mus_command_queue;
        queue += mus_command_queue_size;

        *queue++ = 0x5;
        Audio_WriteU32BE(queue, channel_handle);
        queue += 4;
        Audio_WriteU16BE(queue, sweep_modulation_value);
        queue += 2;

        mus_command_queue_size += 7;
        if (mus_command_queue_size >= mus_command_queue_max_size) {
            mus_command_queue_size -= 7;
        }
        osSetIntMask(mask);
    }
}

/**
 * Queue set frequency offset/distortion for handle
 */
void Audio_QueueVoiceDetune(s32 channel_handle, f32 frequency_offset_value) {
    u8* queue;
    UNUSED s32 pad[3];
    u32 mask;
    UNUSED s32 pad2;

    if (channel_handle != 0) {
        mask = osSetIntMask(1);

        queue = mus_command_queue;
        queue += mus_command_queue_size;

        *queue++ = 0x2;
        Audio_WriteU32BE(queue, channel_handle);
        queue += 4;
        Audio_WriteU16BE(queue, (s32)(255.0f * frequency_offset_value) & 0xFFFF);

        mus_command_queue_size += 7;
        if (mus_command_queue_size >= mus_command_queue_max_size) {
            mus_command_queue_size -= 7;
        }

        osSetIntMask(mask);
    }
}

/**
 * Queue set tempo scale (clamped 1-256)
 */
void Audio_QueueVoiceVolume(s32 channel_handle, s32 tempo_scale_factor) {
    u8* queue;
    UNUSED s32 pad;
    u32 mask;

    if (channel_handle != 0) {
        if (tempo_scale_factor <= 0) {
            tempo_scale_factor = 1;
        } else if (tempo_scale_factor > 256) {
            tempo_scale_factor = 256;
        }
        tempo_scale_factor--;

        mask = osSetIntMask(1);
        queue = mus_command_queue;
        queue += mus_command_queue_size;

        *queue++ = 0x3;
        Audio_WriteU32BE(queue, channel_handle);
        queue += 4;
        Audio_WriteU16BE(queue, tempo_scale_factor & 0xFFFF);

        mus_command_queue_size += 7;
        if (mus_command_queue_size >= mus_command_queue_max_size) {
            mus_command_queue_size -= 7;
        }

        osSetIntMask(mask);
    }
}

/**
 * Queue set tempo scale (u8 variant, clamped 0-127)
 */
void Audio_QueueVoiceVolumeByte(u32 channel_handle, s32 tempo_scale_factor) {
    u8* queue;
    UNUSED s32 pad;
    u32 mask;

    if (channel_handle != 0) {
        if (tempo_scale_factor < 0) {
            tempo_scale_factor = 0;
        } else if (tempo_scale_factor >= 0x80) {
            tempo_scale_factor = 0x7F;
        }

        mask = osSetIntMask(1);

        queue = mus_command_queue;
        queue += mus_command_queue_size;

        *queue++ = 0x3;
        Audio_WriteU32BE(queue, channel_handle);
        queue += 4;

        *queue++ = tempo_scale_factor;

        mus_command_queue_size += 6;
        if (mus_command_queue_size >= mus_command_queue_max_size) {
            mus_command_queue_size -= 6;
        }

        osSetIntMask(mask);
    }
}

void Audio_RelocateSoundBankWrapper(s32 arg0, s32 arg1) {
    Audio_RelocateSoundBank(arg0, arg1);
}

/**
 * Remap relative pointers in sound bank/fx bank to absolute addresses
 */
void Audio_RelocateSoundBankTable(bank_remap_descriptor_t* descriptor) {
    u32 i;
    u32* ptr;
    u32 num_entries;
    u32 val;
    u32 a0 = (u32)descriptor;

    if (descriptor->flags & 2) {
        return;
    }

    num_entries = descriptor->num_entries;

    val = descriptor->metadata_offset;
    descriptor->metadata_offset = a0 + val;

    val = descriptor->entries_offset;
    descriptor->entries_offset = a0 + val;

    ptr = descriptor->entries_offset;
    // FAKE
    if (0) {}

    for (i = 0; i < num_entries; i++, ptr++) {
        *ptr += a0;
    }

    descriptor->flags |= 2;
}

/**
 * Queue set pan computation mode flag
 */
void Audio_QueueSetMonoOutputCommand(s32 mode) {
    u32 mask = osSetIntMask(1);
    u8* queue = mus_command_queue;

    queue += mus_command_queue_size;

    *queue++ = 0xA;
    *queue++ = mode;

    mus_command_queue_size += 2;
    if (mus_command_queue_size >= mus_command_queue_max_size) {
        mus_command_queue_size -= 2;
    }

    osSetIntMask(mask);
}

/**
 * Set voice priority flag (mus_voice_start_immediate_mode) for voice allocation
 */
void Audio_SetVoiceStartTimingMode(s32 priority) {
    u32 mask = osSetIntMask(1);

    if (priority != 0) {
        mus_voice_start_immediate_mode = 1;
    } else {
        mus_voice_start_immediate_mode = 0;
    }

    osSetIntMask(mask);
}

/**
 * Stop all active songs (not FX), fade them out
 */
void Audio_StopAllSoundEffects(void) {
    UNUSED s32 pad[4];
    u32 mask = osSetIntMask(1);
    u32 i;
    channel_t* ptr = mus_channels;

    for (i = 0; i < max_channels; i++, ptr++) {
        if ((ptr->IsFX != 0) || (ptr->pdata == 0)) {
            continue;
        }

        ptr->pdata = Audio_ClearVoicePlaybackState(ptr, 0);

        if (ptr->playing == 0) {
            ptr->sample = 0;
            continue;
        }

        ptr->voice->env_phase = 4;
        ptr->voice->release_end_frame = mus_current_frame_counter + 2;

        n_alSynSetVol(&ptr->voice->voice, 0, mus_next_frame_time * 2);

        n_alSynSetPriority(&ptr->voice->voice, 0);
        Audio_QueueActiveVoice(ptr->voice);

        ptr->voice = NULL;
        ptr->pending = 1;
        ptr->playing = 0;
        ptr->sample = 0;
    }

    osSetIntMask(mask);
}

/**
 * Queue load song with fade/loop parameter (returns handle)
 */
s32 Audio_PlayMusicSequenceLooped(SoundBank* bank, song_t* song_data, u32 fade_out_delay_frames) {
    u8* queue;
    s32 song_handle;
    UNUSED s32 pad;
    u32 mask;

    if (bank == NULL) {
        return 0;
    }

    if (!(bank->flags & 1)) {
        return 0;
    }

    mask = osSetIntMask(1);

    queue = mus_command_queue;
    queue += mus_command_queue_size;

    song_handle = mus_current_handle++;

    *queue++ = 0xB;
    Audio_WriteU32BE(queue, bank->header_name);
    queue += sizeof(u32);
    Audio_WriteU32BE(queue, song_data);
    queue += sizeof(u32);
    Audio_WriteU32BE(queue, song_handle);
    queue += sizeof(u32);
    Audio_WriteU32BE(queue, fade_out_delay_frames);

    mus_command_queue_size += 17;
    if (mus_command_queue_size >= mus_command_queue_max_size) {
        mus_command_queue_size -= 17;
    }

    osSetIntMask(mask);

    return song_handle;
}

/**
 * Setup volume fade state (validates from < to, calculates step rate)
 */
s32 Audio_StartVolumeTransition(s32 fade_handle, u8 from_volume, u8 to_volume, u32 duration_frames) {
    u32 mask;

    if (from_volume == to_volume) {
        return 0;
    }

    if (mus_active_fade_handle != 0) {
        return 0;
    }

    mask = osSetIntMask(1);

    mus_active_fade_handle = fade_handle;
    mus_fade_step_rate = ((f32)to_volume - from_volume) / duration_frames;
    mus_fade_start_volume = from_volume;
    mus_fade_target_volume = to_volume;

    osSetIntMask(mask);

    return 1;
}

/**
 * Read a big endian 32-bit value
 */
u32 Audio_ReadU32BE(u8* buf) {
    u32 val = *buf++ << 0x18;
    val += *buf++ << 0x10;
    val += *buf++ << 8;
    val += *buf++;
    return val;
}

/**
 * Read a big endian 16-bit value
 */
u16 Audio_ReadU16BE(u8* buf) {
    u32 val = *buf++ << 8;
    val += *buf++;
    return val;
}

/**
 * Main command dispatcher - reads queue, calls api_func_jumptable entries
 */
void Audio_ProcessCommandQueue(void) {
    u8* var_s0 = mus_command_queue;
    s32 idx;

    while (mus_command_queue_size) {
        idx = *var_s0++;
        mus_command_queue_size--;
        var_s0 = api_func_jumptable[idx](var_s0);
    }
}

/**
 * Load Song (Command 0x00)
 * Initialize and load a song for playback with associated sample bank.
 */
u8* Audio_PlayMusicSequenceHandler(u8* buf) {
    song_t* song_handle;
    song_t* song_data;
    song_t* sample_bank_ptr;
    channel_t* cp;
    u32 i;
    u32 count;
    u32 index;

    sample_bank_ptr = Audio_ReadU32BE(buf);
    buf += 4;
    song_data = Audio_ReadU32BE(buf);
    buf += 4;
    song_handle = Audio_ReadU32BE(buf);
    buf += 4;

    mus_command_queue_size -= 0xC;

    count = song_data->num_channels;
    if ((u32)song_data->channel_data < 0x400) {
        Audio_RelocatePointerArray(&song_data->channel_data, song_data, 5);
        Audio_RelocatePointerArray(song_data->channel_data, song_data, count);
        Audio_RelocatePointerArray(song_data->volume_data, song_data, count);
        Audio_RelocatePointerArray(song_data->pitchbend_data, song_data, count);
    }

    for (i = 0; i < count; i++) {
        if (song_data->channel_data[i] == 0) {
            continue;
        }

        index = Audio_FindFreeVoice(song_data, i);
        if (index == -1) {
            continue;
        }

        cp = &mus_channels[index];
        Audio_ResetVoiceState(cp);
        cp->song_addr = song_data;
        cp->sample_bank = sample_bank_ptr;
        cp->pvolumebase = cp->pvolumebase_offset = cp->song_addr->volume_data[i];
        cp->ppitchbendbase = cp->ppitchbendbase_offset = cp->song_addr->pitchbend_data[i];
        cp->pdata = cp->pbase = cp->song_addr->channel_data[i];
        cp->handle = song_handle;
        cp->priority = 0x3C;
    }

    mus_song_stop_countdown = -1;
    return buf;
}

/**
 * Load Song with Fade/Loop Parameter (Command 0x0B)
 * Similar to Audio_PlayMusicSequenceHandler, but includes an additional fade/loop parameter.
 */
u8* Audio_PlayMusicSequenceLoopedHandler(u8* buf) {
    u32 song_handle;
    u32 i;
    u32 sample_bank_ptr;
    s32 channel_index;
    u32 num_channels;
    song_t* song_data;
    channel_t* cp;
    u32 fade_out_delay_frames;

    sample_bank_ptr = Audio_ReadU32BE(buf);
    buf += 4;
    song_data = Audio_ReadU32BE(buf);
    buf += 4;
    song_handle = Audio_ReadU32BE(buf);
    buf += 4;
    fade_out_delay_frames = Audio_ReadU32BE(buf);
    buf += 4;

    mus_command_queue_size -= 0x10;

    num_channels = song_data->num_channels;
    if ((u32)song_data->channel_data < 0x400) {
        Audio_RelocatePointerArray(&song_data->channel_data, song_data, 5);
        Audio_RelocatePointerArray(song_data->channel_data, song_data, num_channels);
        Audio_RelocatePointerArray(song_data->volume_data, song_data, num_channels);
        Audio_RelocatePointerArray(song_data->pitchbend_data, song_data, num_channels);
    }

    for (i = 0; i < num_channels; i++) {
        if (song_data->channel_data[i] == 0) {
            continue;
        }

        channel_index = Audio_FindFreeVoice(song_data, i);
        if (channel_index == -1) {
            continue;
        }

        cp = &mus_channels[channel_index];
        Audio_ResetVoiceState(cp);
        cp->song_addr = song_data;
        cp->sample_bank = sample_bank_ptr;
        cp->pvolumebase = cp->pvolumebase_offset = song_data->volume_data[i];
        cp->ppitchbendbase = cp->ppitchbendbase_offset = song_data->pitchbend_data[i];
        cp->pdata = cp->pbase = song_data->channel_data[i];
        cp->handle = song_handle;
        cp->priority = 0x3C;
    }

    mus_song_stop_countdown = fade_out_delay_frames;
    return buf;
}

/**
 * Start Sound Effect (Command 0x01)
 * Start a sound effect/FX using a specified FX bank and sample bank.
 */
u8* Audio_PlaySoundEffectHandler(u8* buf) {
    u32 sample_bank_ptr;
    fx_t* fx_bank_ptr;
    u32* fx_data;
    u32 effect_handle;
    u32 effect_id;
    u32 i;
    u8 effect_priority;
    u8 effect_sweep_amount;
    u8 effect_volume_scale;
    channel_t* cp;
    fx_metadata_t* metadata;

    sample_bank_ptr = Audio_ReadU32BE(buf);
    buf += 4;
    fx_bank_ptr = Audio_ReadU32BE(buf);
    buf += 4;
    effect_handle = Audio_ReadU32BE(buf);
    buf += 4;
    effect_id = Audio_ReadU32BE(buf);
    buf += 4;

    effect_volume_scale = *buf++;
    effect_sweep_amount = *buf++;
    effect_priority = *buf++;

    mus_command_queue_size -= 0x13;
    fx_data = fx_bank_ptr->fx_data;
    metadata = fx_bank_ptr->fx_metadata;

    /* Use default priority from FX bank metadata if specified as 0xFF */
    if (effect_priority == 0xFF) {
        effect_priority = metadata[effect_id].params[FX_PRIORITY];
    }

    for (i = 0, cp = mus_channels; i < max_channels; i++, cp++) {
        if (cp->pdata == NULL) {
            Audio_InitializeVoice(cp, effect_id, effect_volume_scale, effect_sweep_amount, effect_priority);
            cp->handle = effect_handle;
            cp->sample_bank = sample_bank_ptr;
            cp->fx_bank = fx_bank_ptr;
            cp->pdata = cp->pbase = fx_data[effect_id];
            break;
        }
    }

    return buf;
}

/**
 * Set Frequency Offset (Command 0x02)
 * Adjust the frequency offset (distortion/detune) for a song/effect by handle.
 */
u8* Audio_ApplyVoiceDetuneCommand(u8* buf) {
    u32 channel_handle;
    s32 i;
    s16 frequency_offset_value;
    channel_t* cp;

    channel_handle = Audio_ReadU32BE(buf);
    buf += 4;
    frequency_offset_value = Audio_ReadU16BE(buf);
    buf += 2;

    mus_command_queue_size -= 6;

    for (i = 0, cp = mus_channels; i < max_channels; i++, cp++) {
        if (channel_handle == cp->handle) {
            cp->freqoffset = cp->distort + (frequency_offset_value / 255.0f);
        }
    }

    return buf;
}

/**
 * Set Tempo Scale (Command 0x03)
 * Adjust playback speed/tempo for a song/effect by handle.
 */
u8* Audio_ApplyVoiceVolumeCommand(u8* buf) {
    u32 channel_handle;
    u16 tempo_scale_factor;
    channel_t* cp;
    u32 i;
    u8 val;

    channel_handle = Audio_ReadU32BE(buf);
    buf += 4;
    tempo_scale_factor = Audio_ReadU16BE(buf);
    buf += 2;

    mus_command_queue_size -= 6;

    for (i = 0, cp = mus_channels; i < max_channels; i++, cp++) {
        if (channel_handle == cp->handle) {
            val = tempo_scale_factor;
            cp->temscale = val;
            cp->channel_tempo = (cp->channel_tempo_save * val) >> 7;
        }
    }

    return buf;
}

/**
 * Set Volume Scale (Command 0x04)
 * Adjust output volume for a song/effect by handle.
 */
u8* Audio_ApplyVoiceVolumeScaleCommand(u8* buf) {
    u32 channel_handle;
    u32 i;
    channel_t* cp;
    u16 volume_scale_factor;
    u8 val;

    channel_handle = Audio_ReadU32BE(buf);
    buf += 4;
    volume_scale_factor = Audio_ReadU16BE(buf);
    buf += 2;

    mus_command_queue_size -= 6;

    for (i = 0, cp = mus_channels; i < max_channels; i++, cp++) {
        if (channel_handle == cp->handle) {
            val = volume_scale_factor;
            cp->volume_scale = val;
        }
    }

    return buf;
}

/**
 * Set Sweep Amount (Command 0x05)
 * Set sweep/modulation amount for a sound and reset pan state.
 */
u8* Audio_ApplyVoicePanCommand(u8* buf) {
    u32 channel_handle;
    u32 i;
    u16 sweep_modulation_value;
    channel_t* cp;
    u8 val;

    channel_handle = Audio_ReadU32BE(buf);
    buf += 4;
    sweep_modulation_value = Audio_ReadU16BE(buf);
    buf += 2;

    mus_command_queue_size -= 6;

    for (i = 0, cp = mus_channels; i < max_channels; i++, cp++) {
        if (channel_handle == cp->handle) {
            val = sweep_modulation_value;
            cp->sweep_amount = val;
            cp->pan_current = 0xFF;
        }
    }

    return buf;
}

/**
 * Set Reverb Intensity (Command 0x06)
 * Set reverb/effects send level for a sound by handle.
 */
u8* Audio_ApplyVoiceFxMixCommand(u8* buf) {
    u32 i;
    u32 channel_handle;
    channel_t* cp;
    u8 reverb_send_level;

    channel_handle = Audio_ReadU32BE(buf);
    buf += 4;
    reverb_send_level = *buf++;

    mus_command_queue_size -= 5;

    for (i = 0, cp = mus_channels; i < max_channels; i++, cp++) {
        if (channel_handle == cp->handle) {
            cp->reverb_intensity = reverb_send_level;
            cp->reverb_mix = 0xFF;
        }
    }

    return buf;
}

/**
 * Set Duration with Scale (Handle-Specific) (Command 0x07)
 * Set time limit and scale factor for a specific sound handle.
 */
u8* Audio_FadeSoundHandler(u8* buf) {
    u32 channel_handle;
    u32 i;
    u32 duration_scale_factor;
    u16 duration_frames;
    channel_t* cp;

    channel_handle = Audio_ReadU32BE(buf);
    buf += 4;
    duration_frames = Audio_ReadU16BE(buf);
    buf += 2;

    mus_command_queue_size -= 6;

    if (channel_handle != 0) {
        if (duration_frames != 0) {
            duration_scale_factor = duration_frames & 0xFFFF;
        } else {
            duration_scale_factor = 1;
        }

        for (i = 0, cp = mus_channels; i < max_channels; i++, cp++) {
            if (channel_handle == cp->handle) {
                cp->duration = duration_frames;
                cp->duration_scale = duration_scale_factor;
            }
        }
    }

    return buf;
}

/**
 * Set Duration for All Channels (Command 0x08)
 * Set duration for all active channels (either effects or songs based on flag).
 */
u8* Audio_FadeCategoryHandler(u8* buf) {
    u8 channel_type_flags;
    u32 i;
    u32 duration_scale_factor;
    u32 duration_frames;
    channel_t* cp;

    channel_type_flags = *buf++;
    duration_frames = Audio_ReadU16BE(buf);
    buf += 2;

    mus_command_queue_size -= 3;

    if (duration_frames != 0) {
        duration_scale_factor = duration_frames & 0xFFFF;
    } else {
        duration_scale_factor = 1;
    }

    for (i = 0, cp = mus_channels; i < max_channels; i++, cp++) {
        if (((cp->IsFX != 0) && (channel_type_flags & 1)) || ((cp->IsFX == 0) && (channel_type_flags & 2))) {
            cp->duration = duration_frames;
            cp->duration_scale = duration_scale_factor;
        }
    }

    return buf;
}

/**
 * Set Master Volume (Command 0x09)
 * Set global/master volume for either effects or songs.
 */
u8* Audio_SetChannelVolumeHandler(u8* buf) {
    u8 volume_type_flags;
    u16 master_volume_level;

    volume_type_flags = *buf++;
    master_volume_level = Audio_ReadU16BE(buf);
    buf += 2;

    mus_command_queue_size -= 3;

    if (volume_type_flags & 1) {
        mus_master_volume_effects = master_volume_level;
    }

    if (volume_type_flags & 2) {
        mus_master_volume_songs = master_volume_level;
    }

    return buf;
}

/**
 * Set Pan Computation Mode (Command 0x0A)
 * Enable/disable pan computation mode. When enabled, uses fixed pan value (0x40) instead of computed pan.
 */
u8* Audio_SetMonoOutputHandler(u8* buf) {
    u8 pan_mode_flag = *buf++;

    mus_command_queue_size--;

    if (pan_mode_flag == 1) {
        mus_use_fixed_pan_mode = 1;
    } else {
        mus_use_fixed_pan_mode = 0;
    }
    return buf;
}

s32 Audio_PlayerFrameHandler(UNUSED void* node) {
    f64 pad64;
    f32 spA4;
    s32 pad[4];
    ALVoiceConfig sp8C;
    s32 pad2[3];
    s32 pad3[1];
    s32 pad4[1];
    f32 var_fs0;
    u32 var_s4;
    u32 temp_s1;
    s32 temp_s3;
    s32 var_s2;
    s32 i;
    channel_t* ptr;

    if (mus_song_stop_countdown > 0) {
        mus_song_stop_countdown--;
    }

    if (mus_song_stop_countdown == 0) {
        Audio_StopAllSoundEffects();
        mus_song_stop_countdown = -1;
    }

    Audio_ProcessCommandQueue();

    ptr = mus_channels;

    for (i = 0; i < max_channels; i++, ptr++) {
        if (ptr->pdata == 0) {
            continue;
        }

        ptr->channel_frame += ptr->channel_tempo;

        if (ptr->note_duration != 0x7FFF) {
            while ((ptr->channel_frame > ptr->note_end_frame) && (ptr->pdata != 0)) {
                Audio_DecodeVoiceCommandStream(ptr);
            }
        }

        if (ptr->pvolumebase != 0) {
            if (ptr->volume_frame < ptr->channel_frame) {
                Audio_AdvancePitchEnvelope(ptr);
            }
        }

        if (ptr->ppitchbendbase != 0) {
            if (ptr->pitchbend_frame < ptr->channel_frame) {
                Audio_AdvanceVolumeEnvelope(ptr);
            }
        }

        if (ptr->duration != -1) {
            ptr->duration--;
            if (ptr->duration == -1) {
                ptr->pdata = Audio_ClearVoicePlaybackState(ptr, 0);
                if (ptr->playing != 0) {
                    ptr->playing = 0;
                    n_alSynStopVoice(&ptr->voice->voice);
                    n_alSynFreeVoice(&ptr->voice->voice);
                    ptr->voice->env_phase = 5;
                    Audio_QueueFreeVoice(ptr->voice);
                    ptr->voice = NULL;
                    ptr->IsFX = 0;
                }
                ptr->sample = 0;
            }
        }

        if (ptr->sample == 0) {
            continue;
        }

        Audio_ReleaseVoice(ptr);

        if (ptr->sample == 0) {
            continue;
        }

        spA4 = ptr->freqoffset;
        if (ptr->vib_speed != 0) {
            spA4 += Audio_ComputeSinePitchModulation(ptr);
        }

        if (ptr->wobble_on_speed != 0) {
            spA4 += Audio_AdvancePeriodicPitchModulation(ptr);
        }

        var_fs0 = Audio_AdvanceVoicePitch(ptr, spA4);
        temp_s3 = Audio_ComputeVoiceVolume(ptr);

        if (ptr->sweep_speed != 0) {
            if (ptr->sweep_frame < ptr->channel_frame) {
                Audio_AdvancePanModulation(ptr);
            }
        }

        if (mus_use_fixed_pan_mode == 1) {
            var_s2 = 0x40;
        } else {
            var_s2 = ((ptr->pan * ptr->sweep_amount) >> 7) & 0x7F;
        }

        temp_s1 = ptr->reverb_intensity;
        temp_s1 += (((128 - temp_s1) * ptr->reverb) / 128);

        ptr->wobble_frame = (u32)(ptr->channel_frame - ptr->note_start_frame) / 256;

        if (ptr->playing != 0) {
            if (var_fs0 != 0.0f) {
                n_alSynSetPitch(&ptr->voice->voice, var_fs0);
            }

            if (temp_s3 != ptr->volume_current) {
                ptr->volume_current = temp_s3;
                n_alSynSetVol(&ptr->voice->voice, temp_s3, mus_next_frame_time);
            }

            if (var_s2 != ptr->pan_current) {
                ptr->pan_current = var_s2;
                n_alSynSetPan(&ptr->voice->voice, var_s2);
            }

            if (ptr->reverb != ptr->reverb_mix) {
                ptr->reverb_mix = temp_s1;
                n_alSynSetFXMix(&ptr->voice->voice, temp_s1);
            }
        } else {
            ptr->voice = Audio_DequeueFreeVoice();
            if (ptr->voice == NULL) {
                ptr->sample = 0;
                continue;
            }

            ptr->voice->env_phase = ptr->env_state;
            ptr->voice->env_release_speed = ptr->env_release_speed;

            if (ptr->priority >= 128) {
                sp8C.priority = 0x7F;
            } else {
                sp8C.priority = ptr->priority;
            }

            sp8C.unityPitch = 0;
            sp8C.fxBus = 0;
            n_alSynAllocVoice(&ptr->voice->voice, &sp8C);

            ptr->volume_current = temp_s3;
            ptr->pan_current = var_s2;
            ptr->reverb_mix = temp_s1;

            if (var_fs0 == 0.0f) {
                var_fs0 = Audio_ExpApproximation(ptr->pitch_current * (5.0 / 60.0));
                if (var_fs0 > 2.0) {
                    var_fs0 = 2.0f;
                }
            }

            if (mus_voice_start_immediate_mode == 0) {
                n_alSynStartVoiceParams(&ptr->voice->voice, ptr->sample, var_fs0, temp_s3, var_s2, temp_s1,
                                        mus_next_frame_time);
            } else {
                n_alSynStartVoiceParams(&ptr->voice->voice, ptr->sample, var_fs0, temp_s3, var_s2, temp_s1, 1);
            }

            ptr->pending = 0;
            ptr->playing = 1;
            ptr->voice->env_phase = 1;
        }
    }

    Audio_ReclaimExpiredVoices();
    mus_current_frame_counter++;

    return mus_next_frame_time;
}

void Audio_DecodeVoiceCommandStream(channel_t* arg0) {
    s32 pad[2];
    song_t* bank;
    u8* var_s0;
    u8 temp_a0;
    u8 tmp;
    u8 tmp1;
    u16 wave;
    u32 sample;

    var_s0 = arg0->pdata;
    while (var_s0 != NULL && (tmp1 = *var_s0) >= 0x80) {
        var_s0 = command_func_jumptable[tmp1 & 0x7F](arg0, var_s0 + 1);
    }
    arg0->pdata = var_s0;

    if (arg0->pdata != NULL) {
        arg0->last_note = arg0->port_base;
        tmp = *arg0->pdata++;

        if (arg0->velocity_on != 0) {
            arg0->velocity = *arg0->pdata++;
        } else {
            arg0->velocity = arg0->default_velocity;
        }

        if (arg0->fixed_length != 0) {
            if (arg0->ignore == 0) {
                arg0->note_duration = arg0->fixed_length;
            } else {
                arg0->ignore = 0;
                temp_a0 = *arg0->pdata++;
                if (temp_a0 < 0x80) {
                    arg0->note_duration = temp_a0;
                } else {
                    arg0->note_duration = *arg0->pdata++ + ((temp_a0 & 0x7F) << 8);
                }
            }
        } else {
            temp_a0 = *arg0->pdata++;
            if (temp_a0 < 0x80) {
                arg0->note_duration = temp_a0 ^ 0;
            } else {
                arg0->note_duration = *arg0->pdata++ + ((temp_a0 & 0x7F) << 8);
            }
        }

        arg0->note_start_frame = arg0->note_end_frame;
        arg0->note_end_frame += arg0->note_duration << 8;

        arg0->wobble_frame = 0;
        arg0->wobble_value = 0;
        arg0->wobble_counter = arg0->wobble_off_speed;

        if (tmp != 0x60) {
            bank = arg0->sample_bank;
            Audio_UpdateVoicePlaybackEnd(arg0);

            if (arg0->sweep_speed != 0) {
                Audio_ResetPanModulation(arg0);
            }

            if (arg0->playing != 0) {
                arg0->sample = NULL;
                Audio_ReleaseVoice(arg0);
            }

            wave = arg0->wave;
            sample = bank->samples[wave];
            arg0->sample = sample;
            arg0->base_note = bank->detune[wave] + tmp;
        } else {
            arg0->sample = NULL;
            if (arg0->playing != 0) {
                arg0->envelope_end = arg0->channel_frame;
                Audio_ReleaseVoice(arg0);
            }
        }
    } else {
        arg0->sample = NULL;
        if (arg0->playing != 0) {
            arg0->envelope_end = arg0->channel_frame;
            Audio_ReleaseVoice(arg0);
        }
    }
}

s32 Audio_ComputeVoiceVolume(channel_t* cp) {
    u32 volume = cp->volume * cp->env_current_vol * cp->velocity * cp->volume_scale;

    volume >>= 0xD;
    if (cp->IsFX == 0) {
        volume *= mus_master_volume_songs;
    } else {
        volume *= mus_master_volume_effects;
    }

    volume >>= 0xF;
    if (volume >= 0x8000) {
        volume = 0x7FFF;
    }

    if (cp->duration != -1) {
        volume = (cp->duration * volume) / cp->duration_scale;
    }

    return volume;
}

f32 Audio_AdvanceVoicePitch(channel_t* arg0, f32 offset) {
    f32 temp_fv1;
    f32 var_fv1;
    f32 var_fv1_2;
    s32 temp_lo;

    temp_fv1 = arg0->base_note;
    if (arg0->port != 0) {
        if (arg0->port >= arg0->wobble_frame) {
            var_fv1_2 = ((temp_fv1 - arg0->last_note) / arg0->port);
            var_fv1_2 *= arg0->wobble_frame;
            temp_fv1 = arg0->last_note + var_fv1_2;
        }
        arg0->port_base = temp_fv1;
    }

    temp_lo = arg0->transpose * (1 - arg0->ignore_transpose);

    arg0->ignore_transpose = 0;

    temp_fv1 += (offset + arg0->pitchbend_calc + temp_lo);
    if (temp_fv1 == arg0->pitch_current) {
        return 0.0f;
    }
    arg0->pitch_current = temp_fv1;

    temp_fv1 = Audio_ExpApproximation(temp_fv1 * 0.0833333333333333287);
    if (temp_fv1 > 2.0) {
        temp_fv1 = 2.0f;
        arg0->velocity = 0;
    }

    return temp_fv1;
}

void Audio_UpdateVoicePlaybackEnd(channel_t* arg0) {
    s32 temp_t7;
    s32 temp_t7_2;
    u16 temp_v0;
    u32 temp_t3;
    u32 temp_t5;

    if (arg0->note_duration != 0x7FFF) {
        if (arg0->cutoff != 0) {
            arg0->envelope_end = arg0->note_start_frame + (arg0->cutoff << 8);
        } else {
            arg0->envelope_end = arg0->note_end_frame - (arg0->endit << 8);
        }
    } else {
        arg0->envelope_end = 0x7FFFFFFF;
    }

    arg0->env_current_vol = arg0->env_init_vol;
    arg0->env_decay_begin = arg0->env_speed;
    arg0->env_state = 1;
}

void Audio_ReleaseVoice(channel_t* arg0) {
    u32 temp_ft4;
    s32 var_v1;
    u32 temp_a0;

    if (arg0->voice != NULL) {
        if (((arg0->channel_frame >= arg0->envelope_end) && (arg0->voice->env_phase < 4)) || (arg0->sample == 0)) {
            arg0->voice->env_phase = 4;
            temp_ft4 = (f32)arg0->voice->env_release_speed / ((f32)arg0->channel_tempo * 0.00390625);
            if (temp_ft4 == 0) {
                temp_ft4++;
            }
            arg0->voice->release_end_frame = mus_current_frame_counter + temp_ft4 + 1;

            n_alSynSetVol(&arg0->voice->voice, 0, mus_next_frame_time * temp_ft4);

            if (arg0->priority < 0x80) {
                if (arg0->IsFX != 0) {
                    n_alSynSetPriority(&arg0->voice->voice, 1);
                } else {
                    n_alSynSetPriority(&arg0->voice->voice, 0);
                }
            }

            Audio_QueueActiveVoice(arg0->voice);

            arg0->voice = NULL;
            arg0->pending = 1;
            arg0->playing = 0;
            arg0->sample = 0;
        } else {
            switch (arg0->voice->env_phase) {
                case 1:
                    temp_a0 = (u32)(arg0->channel_frame - arg0->note_start_frame) >> 8;
                    if (temp_a0 < arg0->env_attack_speed) {
                        arg0->env_current_vol = arg0->env_attack_calc * temp_a0;
                    } else {
                        arg0->voice->env_phase++;
                        arg0->env_current_vol = arg0->env_max_vol;
                    }
                    break;

                case 2:
                    temp_a0 = ((arg0->channel_frame - arg0->note_start_frame) >> 8) - arg0->env_attack_speed;
                    if (temp_a0 < arg0->env_decay_speed) {
                        arg0->env_current_vol = arg0->env_max_vol + (s32)(arg0->env_decay_calc * temp_a0);
                    } else {
                        arg0->voice->env_phase++;
                        arg0->env_current_vol = arg0->env_sustain_vol;
                    }
                    break;

                case 3:
                case 4:
                    break;
            }
        }
    } else {
        temp_a0 = (u32)(arg0->channel_frame - arg0->note_start_frame) >> 8;
        if ((temp_a0 < arg0->env_attack_speed) && (arg0->env_attack_speed >= 2)) {
            arg0->env_state = 1;
            arg0->env_current_vol = arg0->env_attack_calc * temp_a0;
        } else {
            arg0->env_state = 2;
            arg0->env_current_vol = arg0->env_max_vol;
        }
    }
}

void Audio_ResetPanModulation(channel_t* cp) {
    cp->sweep_frame = cp->note_start_frame;
    cp->sweep_counter = 0;
    cp->sweep_direction = cp->sweep_amount & 0x80;
}

void Audio_AdvancePanModulation(channel_t* arg0) {
    u32 temp_v1;
    u32 var_v0;

    do {
        arg0->sweep_frame += 0x100;

        temp_v1 = arg0->sweep_counter + (arg0->sweep_speed * 2);
        if (temp_v1 < 0x80) {
            arg0->sweep_counter = temp_v1;
            continue;
        }

        arg0->sweep_counter = temp_v1 & 0x7F;
        temp_v1 >>= 7;
        if (arg0->sweep_direction == 0) {
            var_v0 = arg0->sweep_amount;
            var_v0 += temp_v1;
            if (var_v0 >= 0xFF) {
                var_v0 = 0xFE;
                arg0->sweep_direction = 1;
            }
            arg0->sweep_amount = var_v0;
        } else {
            var_v0 = arg0->sweep_amount;
            var_v0 -= temp_v1;
            if ((var_v0 >= 0xFF) || (var_v0 == 0)) {
                var_v0 = 1;
                arg0->sweep_direction = 0;
            }
            arg0->sweep_amount = var_v0;
        }
    } while (arg0->sweep_frame < arg0->channel_frame);

    arg0->pan_current = 0xFF;
}

f32 Audio_AdvancePeriodicPitchModulation(channel_t* arg0) {
    arg0->wobble_counter--;
    if (!arg0->wobble_counter) {
        if (arg0->wobble_value == 0) {
            arg0->wobble_value = arg0->wobble_amount;
            arg0->wobble_counter = arg0->wobble_on_speed;
        } else {
            arg0->wobble_value = 0;
            arg0->wobble_counter = arg0->wobble_off_speed;
        }
    }

    return arg0->wobble_value;
}

f32 Audio_ComputeSinePitchModulation(channel_t* arg0) {
    u32 temp_v0 = arg0->wobble_frame - arg0->vib_delay;
    f32 tmp;

    if (temp_v0 != 0) {
        tmp = __sinf(((temp_v0 / (f32)arg0->vib_speed) * 2) * 3.14159260000000007) * arg0->vib_amount;
        arg0->vibrato = tmp;
    }
    return arg0->vibrato;
}

void Audio_AdvancePitchEnvelope(channel_t* arg0) {
    s32 tmp;

    do {
        arg0->volume_frame += 0x100;

        arg0->cont_vol_repeat_count--;
        if (arg0->cont_vol_repeat_count) {
            continue;
        }

        tmp = *arg0->pvolumebase++;
        if (tmp >= 0x80u) {
            arg0->volume = tmp & 0x7F;

            tmp = *arg0->pvolumebase++;
            if (tmp >= 0x80u) {
                arg0->cont_vol_repeat_count = (tmp & 0x7F) << 8;
                arg0->cont_vol_repeat_count += *arg0->pvolumebase++ + 2;
            } else {
                arg0->cont_vol_repeat_count = tmp + 2;
            }
        } else {
            arg0->volume = tmp;
            arg0->cont_vol_repeat_count = 1;
        }
    } while (arg0->volume_frame < arg0->channel_frame);
}

void Audio_AdvanceVolumeEnvelope(channel_t* arg0) {
    u8 tmp;

    do {
        arg0->pitchbend_frame += 0x100;

        arg0->cont_pb_repeat_count--;
        if (arg0->cont_pb_repeat_count) {
            continue;
        }

        tmp = *arg0->ppitchbendbase++;
        if (tmp >= 0x80) {
            arg0->pitchbend = (f32)(tmp & 0x7F) - 64.0;
            arg0->pitchbend_calc = arg0->pitchbend * arg0->bendrange;
            tmp = *arg0->ppitchbendbase++;
            if (tmp >= 0x80) {
                arg0->cont_pb_repeat_count = (tmp & 0x7F) << 8;
                arg0->cont_pb_repeat_count += *arg0->ppitchbendbase++ + 2;
            } else {
                arg0->cont_pb_repeat_count = tmp + 2;
            }
        } else {
            arg0->pitchbend = (f32)(tmp)-64.0;
            arg0->pitchbend_calc = arg0->pitchbend * arg0->bendrange;
            arg0->cont_pb_repeat_count = 1;
        }
    } while (arg0->pitchbend_frame < arg0->channel_frame);
}

f32 Audio_ExpApproximation(f32 arg0) {
    f32 temp_fa1;
    f32 temp_fv1;

    if (arg0 == 0.0f) {
        return 1.0f;
    }

    if (arg0 > 0.0f) {
        temp_fv1 = SQ(arg0);
        temp_fa1 = SQ(temp_fv1);
        return (arg0 * 0.693147180559944953) + 1.0 + (temp_fv1 * 0.240226506959100999) +
               ((temp_fv1 * arg0) * 0.055504108664821597) + (temp_fa1 * 0.00961812910762848035) +
               ((temp_fa1 * arg0) * 0.00133335581464283999) + ((temp_fa1 * temp_fv1) * 0.000154035303933816006);
    } else {
        arg0 = -arg0;
        temp_fv1 = SQ(arg0);
        temp_fa1 = SQ(temp_fv1);
        return 1.0 / ((arg0 * 0.693147180559944953) + 1.0 + (temp_fv1 * 0.240226506959100999) +
                      ((temp_fv1 * arg0) * 0.055504108664821597) + (temp_fa1 * 0.00961812910762848035) +
                      ((temp_fa1 * arg0) * 0.00133335581464283999) + ((temp_fa1 * temp_fv1) * 0.000154035303933816006));
    }
}

void Audio_RelocateSoundBank(u8* arg0, s32 arg1) {
    UNUSED s32 pad;
    u32 i;
    f32 var_fv0;
    SoundBank* soundbank = arg0;
    u8 temp_v1;

    if (soundbank->flags & 1) {
        return;
    }

    soundbank->flags |= 1;

    Audio_RelocatePointerArray(&soundbank->basenote, arg0, 3);
    Audio_RelocatePointerArray(soundbank->wave_list, arg0, soundbank->count);

    for (i = 0; i < soundbank->count; i++) {
        f32* ptr = &soundbank->detune[i];

        temp_v1 = *(u8*)ptr;
        if (temp_v1 & 0x80) {
            var_fv0 = -(0x100 - temp_v1);
        } else {
            var_fv0 = (s32)temp_v1;
        }
        *ptr = var_fv0 / 100.0;

        temp_v1 = soundbank->basenote[i] - 0x30;
        if (temp_v1 & 0x80) {
            var_fv0 = -(0x100 - temp_v1);
        } else {
            var_fv0 = (s32)temp_v1;
        }
        *ptr += var_fv0;

        if (soundbank->wave_list[i]->flags == 0) {
            u32 tmp = soundbank->wave_list[i]->base;

            if ((tmp & 0xFF000000) != 0xFF000000) {
                tmp += arg1;
                soundbank->wave_list[i]->base = tmp;
            }

            soundbank->wave_list[i]->flags = 1;

            if (soundbank->wave_list[i]->waveInfo.rawWave.loop != NULL) {
                soundbank->wave_list[i]->waveInfo.rawWave.loop =
                    (u32)soundbank + (u32)soundbank->wave_list[i]->waveInfo.rawWave.loop;
            }

            if (soundbank->wave_list[i]->type == AL_ADPCM_WAVE) {
                soundbank->wave_list[i]->waveInfo.adpcmWave.book =
                    (u32)soundbank + (u32)soundbank->wave_list[i]->waveInfo.adpcmWave.book;
            }
        }
    }

    osWritebackDCacheAll();
}

s32 Audio_RandomScale(s32 arg0) {
    s32 i;
    s32 tmp;
    f32 tmpf;

    for (i = 0; i < 8; i++) {
        tmp = mus_random_seed & 0x48000000;
        mus_random_seed <<= 1;
        if ((tmp == 0x48000000) || (tmp == 0x08000000)) {
            mus_random_seed |= 1;
        }
    }

    tmpf = mus_random_seed / 65536.0f;
    tmpf /= 65536.0f;
    return arg0 * tmpf;
}

void Audio_ResetVoiceState(channel_t* arg0) {
    u8* ptr;
    s32 i;

    arg0->pdata = 0;
    arg0->sample_bank = 0;
    arg0->sample = 0;
    arg0->voice = NULL;

    arg0->channel_frame = 0;
    arg0->volume_frame = 0;
    arg0->pitchbend_frame = 0;
    arg0->note_end_frame = 0;
    arg0->note_start_frame = 0;
    arg0->envelope_end = 0;

    ptr = (u8*)&arg0->envelope_end + 4;
    for (i = 0x34; i < 0x150; i++) {
        *ptr++ = 0;
    }

    arg0->volume_current = 0xFFFF;
    arg0->reverb_mix = -1;
    arg0->pan_current = -1;
    arg0->pitch_current = 99.90000153f;

    arg0->channel_tempo = arg0->channel_tempo_save = 0x6000 / mus_vsyncs_per_second;
    arg0->note_duration = 1;

    arg0->velocity_on = 0;
    arg0->default_velocity = 0x7F;

    // FAKE
    if (1) {}

    arg0->fixed_length = 0;
    arg0->ignore = 0;
    arg0->volume = 0x7F;
    arg0->pan = 0x40;
    arg0->cont_vol_repeat_count = 1;
    arg0->cont_pb_repeat_count = 1;
    arg0->duration = -1;
    arg0->sweep_speed = 0;
    arg0->volume_scale = 0x80;
    arg0->sweep_amount = 0x80;
    arg0->temscale = 0x80;
    arg0->env_speed = 1;
    arg0->env_attack_speed = 1;
    arg0->env_max_vol = 0x7F;
    arg0->env_decay_speed = -1;
    arg0->bendrange = 0.03125f;

    arg0->env_attack_calc = 1.0f;
    arg0->env_decay_calc = 0.003921568859f;
    arg0->pitchbend_calc = 0.0f;
    arg0->freqoffset = 0.0f;
    arg0->port_base = 0.0f;
    arg0->distort = 0.0f;

    arg0->env_sustain_vol = 0x7F;
    arg0->env_release_speed = 1;
    arg0->pending = 1;
    arg0->vib_speed = 0;
    arg0->wobble_on_speed = 0;
    arg0->IsFX = 0;
    arg0->playing = 0;
}

s32 Audio_FindFreeVoice(UNUSED song_t* arg0, UNUSED s32 arg1) {
    s32 i;
    channel_t* var_v0 = mus_channels;

    for (i = 0; i < max_channels; i++, var_v0++) {
        if (var_v0->pdata == NULL) {
            return i;
        }
    }

    return -1;
}

void Audio_FillBytes(u8* arg0, u8 arg1, u32 arg2) {
    u8* a0 = arg0;

    while (arg2--) {
        *a0++ = arg1;
    }
}

void Audio_RelocatePointerArray(u8* arg0, u32 arg1, u32 arg2) {
    u32* ptr = arg0;
    u32 i;

    for (i = 0; i < arg2; i++) {
        if (ptr[i] != 0) {
            ptr[i] += arg1;
        }
    }
}

s32 Audio_InitializeVoice(channel_t* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    Audio_ResetVoiceState(arg0);
    arg0->IsFX = arg1;
    arg0->volume_scale = arg2;
    arg0->sweep_amount = arg3;
    arg0->priority = arg4;
    return arg0->handle;
}

/**
 * Add voice to free list (mus_voice1/mus_voice2) when done
 */
void Audio_QueueFreeVoice(audio_voice_t* voice) {
    if (mus_free_voice_1 == NULL) {
        mus_free_voice_1 = mus_free_voice_2 = voice;
        voice->node.next = NULL;
    } else {
        mus_free_voice_2->node.next = voice;
        voice->node.next = NULL;
        mus_free_voice_2 = voice;
    }

    voice->env_phase = 5;
    mus_free_voices_count++;
}

/**
 * Get voice from free list, update in-use counter
 */
audio_voice_t* Audio_DequeueFreeVoice(void) {
    audio_voice_t* voice;

    if (mus_free_voice_1 == NULL) {
        return NULL;
    }

    voice = mus_free_voice_1;
    mus_free_voice_1 = mus_free_voice_1->node.next;

    mus_free_voices_count--;
    return voice;
}

/**
 * Add voice to release/cleanup list (mus_voice3/mus_voice4)
 */
void Audio_QueueActiveVoice(audio_voice_t* voice) {
    if (mus_releasing_voice_3 == NULL) {
        mus_releasing_voice_3 = mus_releasing_voice_4 = voice;
        voice->node.next = NULL;
        voice->node.prev = NULL;
    } else {
        mus_releasing_voice_4->node.next = voice;
        voice->node.next = NULL;
        voice->node.prev = mus_releasing_voice_4;
        mus_releasing_voice_4 = voice;
    }

    mus_releasing_voices_count++;
}

/**
 * Garbage collect released voices when release_end_frame expires
 */
void Audio_ReclaimExpiredVoices(void) {
    audio_voice_t* temp_s1;
    audio_voice_t* var_s0;

    if (mus_releasing_voice_3 != NULL) {
        var_s0 = mus_releasing_voice_3;

        do {
            if (var_s0->release_end_frame < mus_current_frame_counter) {
                n_alSynStopVoice(&var_s0->voice);
                n_alSynFreeVoice(&var_s0->voice);

                var_s0->env_phase = 5;
                if (var_s0->node.prev != NULL) {
                    if (var_s0->node.next != NULL) {
                        var_s0->node.prev->next = var_s0->node.next;
                        var_s0->node.next->prev = var_s0->node.prev;
                    } else {
                        var_s0->node.prev->next = NULL;
                        mus_releasing_voice_4 = var_s0->node.prev;
                    }
                } else if (var_s0->node.next != NULL) {
                    mus_releasing_voice_3 = var_s0->node.next;
                    // FAKE
                    if (!var_s0) {}
                    mus_releasing_voice_3->node.prev = NULL;
                } else {
                    mus_releasing_voice_3 = mus_releasing_voice_4 = NULL;
                }
                temp_s1 = var_s0->node.next;
                mus_releasing_voices_count -= 1;
                Audio_QueueFreeVoice(var_s0);
                var_s0 = temp_s1;
            } else {
                var_s0 = var_s0->node.next;
            }
        } while (var_s0 != NULL);
    }
}
