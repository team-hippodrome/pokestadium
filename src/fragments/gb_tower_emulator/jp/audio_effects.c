/* Stadium Zero fragment000_80C00. */
/* Audio driver, matching OoT src/audio/internal/effects.c in function order. The driver is
 * Nintendo's "Nas" library (original names Nas_*); this revision sits between SM64 Shindou and
 * OoT. See docs/annotation/audio-driver.md for the per-function mapping. */

#include "global.h"
#include "m2c_macros.h"
#include "gb_tower_emulator_jp.h"

/**
 * Recomputes a channel's volume and pan when dirty and passes pitch, volume and pan on to its
 * layers.
 * Corresponds to OoT Audio_SequenceChannelProcessSound (original name __Nas_CallWaveProcess_Sub).
 * MM: AudioScript_SequenceChannelProcessSound.
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioEffects_SequenceChannelProcessSound(SequenceChannel* arg0, u32 force) {
    f32 vol;
    s32 i;
    SequenceLayer* v;

    if (arg0->changes.s.volume || force) {
        vol = arg0->volume * arg0->volumeScale * arg0->seqPlayer->appliedFadeVolume;
        if (arg0->seqPlayer->muted && (M2C_FIELD(arg0, u8*, 3) & 0x20)) {
            vol = arg0->seqPlayer->muteVolumeScale * vol;
        }
        arg0->appliedVolume = vol * vol;
    }
    if (arg0->changes.s.pan) {
        arg0->pan = arg0->newPan * arg0->panChannelWeight;
    }
    for (i = 0; i < 4; i++) {
        v = arg0->layers[i];
        if (v != NULL && v->enabled && v->note != 0) {
            if (v->notePropertiesNeedInit) {
                v->noteFreqScale = v->freqScale * arg0->freqScale;
                v->noteVelocity = v->velocitySquare2 * arg0->appliedVolume;
                v->notePan = (arg0->pan + v->pan * (0x80 - arg0->panChannelWeight)) >> 7;
                v->notePropertiesNeedInit = 0;
            } else {
                if (arg0->changes.s.freqScale) {
                    v->noteFreqScale = v->freqScale * arg0->freqScale;
                }
                if (arg0->changes.s.volume || force) {
                    v->noteVelocity = v->velocitySquare2 * arg0->appliedVolume;
                }
                if (arg0->changes.s.pan) {
                    v->notePan = (arg0->pan + v->pan * (0x80 - arg0->panChannelWeight)) >> 7;
                }
            }
        }
    }
    M2C_FIELD(arg0, u8*, 1) = 0;
}

void AudioEffects_SequenceChannelProcessSound(SequenceChannel* arg0, u32 force);
void AudioSeq_SequencePlayerDisable(SequencePlayer *);
/**
 * Steps a sequence player's fade, stops it when a fade-out ends, and updates its channels' sound.
 * Corresponds to OoT Audio_SequencePlayerProcessSound (original name Nas_MainCtrl). MM:
 * AudioScript_SequencePlayerProcessSound.
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioEffects_SequencePlayerProcessSound(SequencePlayer *arg0) {
    s32 i;

    if (arg0->fadeTimer != 0) {
        arg0->fadeVolume += arg0->fadeVelocity;
        arg0->recalculateVolume = 1;
        if (arg0->fadeVolume > 1.0f) {
            arg0->fadeVolume = 1.0f;
        }
        if (arg0->fadeVolume < 0.0f) {
            arg0->fadeVolume = 0.0f;
        }
        if ((--arg0->fadeTimer == 0) && (arg0->state == 2)) {
            AudioSeq_SequencePlayerDisable(arg0);
            return;
        }
    }
    if (arg0->recalculateVolume) {
        arg0->appliedFadeVolume = arg0->fadeVolume * arg0->fadeVolumeScale;
    }
    for (i = 0; i < 16; i++) {
        if (arg0->channels[i]->enabled == 1) {
            AudioEffects_SequenceChannelProcessSound(arg0->channels[i], arg0->recalculateVolume);
        }
    }
    arg0->recalculateVolume = 0;
}

extern f32 D_812395E8[];

/**
 * Advances a portamento and returns its frequency scale.
 * Corresponds to OoT Audio_GetPortamentoFreqScale (original name Nas_SweepCalculator). OoT:
 * Audio_GetPortamentoFreqScale. The name is MM's.
 * @unverified Name follows the counterpart; not checked in play.
 */
f32 AudioEffects_UpdatePortamento(arg0)
    s8* arg0;
{
    u32 idx;
    f32 ret;

    M2C_FIELD(arg0, f32*, 4) += M2C_FIELD(arg0, f32*, 8);
    idx = M2C_FIELD(arg0, f32*, 4);
    if (idx >= 0x80) {
        idx = 0x7F;
    }
    ret = (M2C_FIELD(arg0, f32*, 0xC) * (D_812395E8[idx] - 1.0f)) + 1.0f;
    return ret;
}


/*
 * AudioEffects_GetVibratoPitchChange
 *
 * Advances the vibrato time by its rate (truncated to an integer) and
 * returns the curve entry selected by time bits 10-15 (a 64-entry table of
 * s16), shifted right by 8 (sign-extended).
 * Corresponds to OoT Audio_GetVibratoPitchChange (original name Nas_ModTableRead). The name is
 * MM's.
 */
s16 AudioEffects_GetVibratoPitchChange(VibratoState* arg0) {
    s32 index;

    arg0->time += (s32)arg0->rate;
    index = (arg0->time >> 10) & 0x3F;
    return (s16)(arg0->curve[index] >> 8);
}

/**
 * Returns the frequency scale for a note's vibrato and steps it: counts down
 * the delay, moves the extent and rate toward their channel targets over their
 * change timers, and scales the curve value at the current phase by the
 * extent. Same structure as get_vibrato_freq_scale in the Super Mario 64 audio
 * library.
 * Corresponds to OoT Audio_GetVibratoFreqScale (original name Nas_Modulator). OoT:
 * Audio_GetVibratoFreqScale. The name is MM's.
 */
f32 AudioEffects_UpdateVibrato(vib)
    VibratoState* vib;
{
    s32 pitch_change;
    f32 extent;
    f32 result;
    SequenceChannel* seq_channel = vib->channel;

    if (vib->delay != 0) {
        vib->delay--;
        return 1;
    }

    if (vib->extentChangeTimer) {
        if (vib->extentChangeTimer == 1) {
            vib->extent = (s32)seq_channel->vibratoExtentTarget;
        } else {
            vib->extent += ((s32)seq_channel->vibratoExtentTarget - vib->extent) / (s32)vib->extentChangeTimer;
        }

        vib->extentChangeTimer--;
    } else if (seq_channel->vibratoExtentTarget != (s32)vib->extent) {
        if ((vib->extentChangeTimer = seq_channel->vibratoExtentChangeDelay) == 0) {
            vib->extent = (s32)seq_channel->vibratoExtentTarget;
        }
    }

    if (vib->rateChangeTimer) {
        if (vib->rateChangeTimer == 1) {
            vib->rate = (s32)seq_channel->vibratoRateTarget;
        } else {
            vib->rate += ((s32)seq_channel->vibratoRateTarget - vib->rate) / (s32)vib->rateChangeTimer;
        }

        vib->rateChangeTimer--;
    } else if (seq_channel->vibratoRateTarget != (s32)vib->rate) {
        if ((vib->rateChangeTimer = seq_channel->vibratoRateChangeDelay) == 0) {
            vib->rate = (s32)seq_channel->vibratoRateTarget;
        }
    }

    if (vib->extent == 0) {
        return 1.0f;
    }

    pitch_change = AudioEffects_GetVibratoPitchChange(vib);
    extent = (f32)vib->extent / 4096.0f;

    result = 1.0f + extent * (D_812395E8[pitch_change] - 1.0f);
    return result;
}

f32 AudioEffects_UpdatePortamento(); // The caller passes a second argument the callee ignores.
f32 AudioEffects_UpdateVibrato(); // The caller passes a second argument the callee ignores.

/**
 * Updates a note's portamento and vibrato frequency scales.
 * Corresponds to OoT Audio_NoteVibratoUpdate (original name Nas_ChannelModulation). OoT:
 * Audio_NoteVibratoUpdate. The name is MM's.
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioEffects_UpdatePortamentoAndVibrato(s8 *arg0) {
    if (M2C_FIELD(arg0, u8 *, 0x80) != 0) {
        M2C_FIELD(arg0, f32 *, 0x38) = AudioEffects_UpdatePortamento(arg0 + 0x80, arg0);
    }
    if ((M2C_FIELD(arg0, u8 *, 0xA4) != 0) && (M2C_FIELD(arg0, s32 *, 0x44) != -1)) {
        M2C_FIELD(arg0, f32 *, 0x3C) = AudioEffects_UpdateVibrato(arg0 + 0x90, arg0);
    }
}

extern s16* D_812393D8;

/**
 * Starts a note's vibrato from its channel's vibrato settings (extent and rate
 * start at their start values when they have a change delay, otherwise at
 * their targets) and copies the layer's portamento into the note. Same
 * structure as note_vibrato_init in the Super Mario 64 audio library.
 * Corresponds to OoT Audio_NoteVibratoInit (original name Nas_ChannelModInit). The name is MM's.
 * Zero also copies the layer's portamento at the end, which OoT does in Audio_NotePortamentoInit
 * (Nas_SweepInit).
 */
void AudioEffects_InitVibrato(Note* note) {
    VibratoState* vib;
    SequenceChannel* seq_channel;
    NotePlaybackState* seq_player_state = &note->playbackState;

    note->playbackState.vibratoFreqScale = 1.0f;
    note->playbackState.portamentoFreqScale = 1.0f;
    vib = &note->playbackState.vibratoState;

    vib->active = 1;
    vib->time = 0;

    vib->curve = D_812393D8;
    vib->channel = note->playbackState.parentLayer->channel;
    seq_channel = vib->channel;

    if ((vib->extentChangeTimer = seq_channel->vibratoExtentChangeDelay) == 0) {
        vib->extent = (s32)seq_channel->vibratoExtentTarget;
    } else {
        vib->extent = (s32)seq_channel->vibratoExtentStart;
    }

    if ((vib->rateChangeTimer = seq_channel->vibratoRateChangeDelay) == 0) {
        vib->rate = (s32)seq_channel->vibratoRateTarget;
    } else {
        vib->rate = (s32)seq_channel->vibratoRateStart;
    }
    vib->delay = seq_channel->vibratoDelay;

    seq_player_state->portamento = seq_player_state->parentLayer->portamento;
}

/**
 * Resets an ADSR envelope to state 0 at envelope `arg1`.
 * Corresponds to OoT Audio_AdsrInit (original name Nas_EnvInit). OoT: Audio_AdsrInit. The name is
 * MM's.
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioEffects_InitAdsr(s8 *arg0, s32 arg1, M2C_UNK arg2) {
    M2C_FIELD(arg0, s8 *, 0) = 0;
    M2C_FIELD(arg0, s8 *, 1) = 0;
    M2C_FIELD(arg0, s16 *, 4) = 0;
    M2C_FIELD(arg0, s32 *, 0x20) = arg1;
    M2C_FIELD(arg0, f32 *, 8) = 0.0f;
    M2C_FIELD(arg0, f32 *, 0x14) = 0.0f;
}

extern f32 D_81288D7C;

/**
 * Steps an ADSR envelope by one update and returns its level clamped to
 * [0, 1]. Envelope points with a delay of 0, -1, -2 and -3 disable, hang,
 * jump and restart; other points fade toward their target over their delay.
 * Corresponds to OoT Audio_AdsrUpdate (original name Nas_EnvProcess; MM AudioEffects_UpdateAdsr):
 * the same states, the squared target level, the 128-update sustain countdown and the 0.00001f
 * cutoff.
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name guessed from the code; comment kept.
 */
f32 AudioEffects_UpdateAdsr(AdsrState* adsr) {
    u8 action = adsr->action;
    u8 state = adsr->state;

    switch (state) {
        case 0:
            return 0.0f;

        case 1:
            if (action & 0x40) {
                adsr->state = 5;
                break;
            }
        case 2:
            adsr->envIndex = 0;
            adsr->state = 3;
        retry:
        case 3:
            adsr->delay = adsr->envelope[adsr->envIndex].delay;
            switch (adsr->delay) {
                case 0:
                    adsr->state = 0;
                    break;
                case -1:
                    adsr->state = 5;
                    break;
                case -2:
                    adsr->envIndex = adsr->envelope[adsr->envIndex].arg;
                    goto retry;
                case -3:
                    adsr->state = 1;
                    break;
                default:
                    adsr->delay *= D_81288D7C;
                    if (adsr->delay == 0) {
                        adsr->delay = 1;
                    }
                    adsr->target = adsr->envelope[adsr->envIndex].arg / 32767.0f;
                    adsr->target = adsr->target * adsr->target;
                    adsr->velocity = (adsr->target - adsr->current) / adsr->delay;
                    adsr->state = 4;
                    adsr->envIndex++;
                    break;
            }
            if (adsr->state != 4) {
                break;
            }
        case 4:
            adsr->current += adsr->velocity;
            if (--adsr->delay <= 0) {
                adsr->state = 3;
            }
        case 5:
            break;

        case 6:
        case 7:
            adsr->current -= adsr->fadeOutVel;
            if (adsr->sustain != 0.0f && state == 6) {
                if (adsr->current < adsr->sustain) {
                    adsr->current = adsr->sustain;
                    adsr->delay = 128;
                    adsr->state = 8;
                }
                break;
            }

            if (adsr->current < 0.00001f) {
                adsr->current = 0.0f;
                adsr->state = 0;
            }
            break;

        case 8:
            adsr->delay--;
            if (adsr->delay == 0) {
                adsr->state = 7;
            }
            break;
    }

    if (action & 0x20) {
        adsr->state = 6;
        adsr->action = action & ~0x20;
    }

    if (action & 0x10) {
        adsr->state = 7;
        adsr->action = action & ~0x10;
    }

    if (adsr->current < 0.0f) {
        return 0.0f;
    }
    if (adsr->current > 1.0f) {
        return 1.0f;
    }
    return adsr->current;
}
