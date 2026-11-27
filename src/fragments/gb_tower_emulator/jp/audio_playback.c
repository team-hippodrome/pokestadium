/* Stadium Zero fragment000_80C00. */
/* Audio driver, matching OoT src/audio/internal/playback.c in function order. The driver is
 * Nintendo's "Nas" library (original names Nas_*); this revision sits between SM64 Shindou and
 * OoT. See docs/annotation/audio-driver.md for the per-function mapping. */

#include "global.h"
#include "gb_tower_emulator_jp.h"
#include "m2c_macros.h"

/* The attributes AudioPlayback_InitSampleState is given (built on the stack from a note's attributes). */
typedef struct NoteSubAttributes {
    /* 0x00 */ u8 reverbVol; // @unverified: same offset/use as OoT's NoteSubAttributes.reverbVol
    /* 0x01 */ u8 gain; // @unverified: same offset/use as OoT's NoteSampleStateAttributes.gain (from channel->gain)
    /* 0x02 */ u8 pan; // @unverified: same offset/use as OoT's NoteSubAttributes.pan
    /* 0x03 */ Stereo reverbBits; // @unverified: same offset/use as OoT's NoteSubAttributes.reverbBits
    /* 0x04 */ f32 freqScale; // @unverified: same offset/use as OoT's NoteSubAttributes.frequency
    /* 0x08 */ f32 velocity; // @unverified: same offset/use as OoT's NoteSubAttributes.velocity
    /* 0x0C */ s32 unk0C;
    /* 0x10 */ s32 filter; // @unverified: takes channel->filter / NoteAttributes.filter, like OoT's NoteSampleStateAttributes.filter
} NoteSubAttributes;
void AudioPlayback_SeqLayerDecayRelease(SequenceLayer* layer, s32 target);

void AudioPlayback_NoteSetResamplingRate(s8* arg0, f32 arg1);

extern s8 D_81288D8E;
extern u16 D_81239E48[];
extern f32 D_81239F48[];
extern f32 D_8123A148[];
extern f32 D_8123A348[];

/**
 * Sets a note's left and right volume, pan and reverb from its attributes.
 * Corresponds to OoT Audio_InitSampleState (original name Nas_smzSetParam).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void AudioPlayback_InitSampleState(s8* note, NoteSubAttributes* reverb_info) {
    NoteSampleState* sub;
    f32 vol_right;
    f32 vol_left;
    s32 small_pan_index;
    UNUSED u32 pad; // FAKE: stack padding
    UNUSED u32 pad1; // FAKE: stack padding
    u8 strong_right;
    u8 strong_left;
    f32 velocity;
    u8 pan;
    u8 reverb_vol;
    Stereo reverb_bits;

    AudioPlayback_NoteSetResamplingRate(note, reverb_info->freqScale);
    velocity = reverb_info->velocity;
    pan = reverb_info->pan;
    reverb_vol = reverb_info->reverbVol;
    reverb_bits = reverb_info->reverbBits;
    sub = (NoteSampleState*)(note + 0xB4);
    pan &= 0x7F;

    if (sub->stereoHeadsetEffects && D_81288D8E == 1) {
        small_pan_index = pan >> 1;
        if (small_pan_index > 0x3F) {
            small_pan_index = 0x3F;
        }
        sub->headsetPanLeft = D_81239E48[small_pan_index];
        sub->headsetPanRight = D_81239E48[0x3F - small_pan_index];
        sub->stereoStrongRight = 0;
        sub->stereoStrongLeft = 0;
        sub->usesHeadsetPanEffects = 1;
        vol_left = D_81239F48[pan];
        vol_right = D_81239F48[0x7F - pan];
    } else if (sub->stereoHeadsetEffects && D_81288D8E == 0) {
        strong_left = strong_right = 0;
        sub->headsetPanRight = 0;
        sub->headsetPanLeft = 0;
        sub->usesHeadsetPanEffects = 0;
        vol_left = D_8123A148[pan];
        vol_right = D_8123A148[0x7F - pan];
        if (pan < 0x20) {
            strong_left = 1;
        } else if (pan > 0x60) {
            strong_right = 1;
        }
        sub->stereoStrongRight = strong_right;
        sub->stereoStrongLeft = strong_left;
        switch (reverb_bits.s.stereoHeadsetEffects) {
            case 0:
                sub->stereoStrongRight = reverb_bits.s.strongRight;
                sub->stereoStrongLeft = reverb_bits.s.strongLeft;
                break;
            case 1:
                break;
            case 2:
                sub->stereoStrongRight = reverb_bits.s.strongRight | strong_right;
                sub->stereoStrongLeft = reverb_bits.s.strongLeft | strong_left;
                break;
            case 3:
                sub->stereoStrongRight = reverb_bits.s.strongRight ^ strong_right;
                sub->stereoStrongLeft = reverb_bits.s.strongLeft ^ strong_left;
                break;
        }
    } else if (D_81288D8E == 3) {
        vol_left = 0.707f;
        vol_right = 0.707f;
    } else {
        vol_left = D_8123A348[pan];
        vol_right = D_8123A348[0x7F - pan];
    }

    if (velocity < 0.0f) {
        velocity = 0.0f;
    }
    if (velocity > 1.0f) {
        velocity = 1.0f;
    }

    sub->targetVolLeft = (s32)(velocity * vol_left * 4095.999f);
    sub->targetVolRight = (s32)(velocity * vol_right * 4095.999f);
    sub->gain = reverb_info->gain;
    sub->filter = reverb_info->filter;

    if (sub->reverbVol != reverb_vol) {
        sub->reverbVol = reverb_vol;
        sub->envMixerNeedsInit = 1;
    } else if (sub->needsInit) {
        sub->envMixerNeedsInit = 1;
    } else {
        sub->envMixerNeedsInit = 0;
    }
}

/* Byte 1 of the note's sample state (OoT NoteSampleState.bitField1). */
typedef struct SampleStateFlags811105DC {
    /* 0x0 */ u8 unk0;
    /* 0x1 */ u8 pad : 7;
    /* 0x1 */ u8 hasTwoParts : 1;
} SampleStateFlags811105DC;

/**
 * Sets a note's fixed-point resampling rate. Rates of 2.0 or more are halved and flagged
 * hasTwoParts, so the synthesizer resamples in two halves.
 * Corresponds to OoT Audio_NoteSetResamplingRate (original name Nas_smzSetPitch). Same 2.0f split,
 * 1.99998f and 3.99996f clamps, halving and 32768.0f fixed-point scale.
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void AudioPlayback_NoteSetResamplingRate(s8* arg0, f32 arg1) {
    SampleStateFlags811105DC* flags = (SampleStateFlags811105DC*)(arg0 + 0xB4);
    f32 resampling_rate;

    if (arg1 < 2.0f) {
        flags->hasTwoParts = 0;
        if (1.99998f < arg1) {
            resampling_rate = 1.99998f;
        } else {
            resampling_rate = arg1;
        }
    } else {
        flags->hasTwoParts = 1;
        if (3.99996f < arg1) {
            resampling_rate = 1.99998f;
        } else {
            resampling_rate = arg1 * 0.5f;
        }
    }
    M2C_FIELD(arg0, s16*, 0xC0) = resampling_rate * 32768.0f;
}

extern NoteSampleState D_81239E30;
void AudioEffects_InitAdsr(u8*, s32, char*);

/**
 * Starts a note's envelope from its instrument or the layer's own envelope.
 * Corresponds to OoT Audio_NoteInit (original name Nas_StartVoice).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void AudioPlayback_NoteInit(Note* arg0) {
    SequenceLayer* def = arg0->playbackState.parentLayer;

    if (def->adsr.decayIndex == 0) {
        AudioEffects_InitAdsr(&arg0->playbackState.adsr.action, def->channel->adsr.envelope, &arg0->playbackState.adsrVolScale);
    } else {
        AudioEffects_InitAdsr(&arg0->playbackState.adsr.action, def->adsr.envelope, &arg0->playbackState.adsrVolScale);
    }
    arg0->playbackState.unk_04 = 0;
    arg0->playbackState.adsr.state = 1;
    arg0->sampleState = D_81239E30;
}

/**
 * Turns a note off and clears its playback state.
 * Corresponds to OoT Audio_NoteDisable (original name Nas_StopVoice).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void AudioPlayback_NoteDisable(Note* note) {
    if (note->sampleState.needsInit == 1) {
        note->sampleState.needsInit = 0;
    }
    note->playbackState.priority = 0;
    note->playbackState.unk_04 = 0;
    note->sampleState.enabled = 0;
    note->playbackState.parentLayer = (void*)-1;
    note->playbackState.prevParentLayer = (void*)-1;
    note->sampleState.finished = 0;
    note->playbackState.adsr.state = 0;
    note->playbackState.adsr.current = 0.0f;
}

extern struct Note* D_81287B04;
extern s32 D_81288D88;
extern f32 D_81288D70;
extern f32 D_81288D74;
void AudioPlayback_SeqLayerNoteRelease(Particle* arg0);
void AudioPlayback_AudioListRemove(s8* arg0);
void AudioPlayback_AudioListPushFront(M2C_UNK* arg0, M2C_UNK* arg1);
s32 AudioSeq_AudioListPushBack(s8*, struct Note*);
void AudioPlayback_NoteInitForLayer(s8* arg0, s8* arg1);
void AudioSeq_SequenceChannelDisable(SequenceChannel*);
void AudioEffects_InitVibrato(Note*);
f32 AudioEffects_UpdateAdsr(u8*);
void AudioEffects_UpdatePortamentoAndVibrato(Note*);

/**
 * Updates every note once per audio frame: envelopes, releases, pitch, volume and pan.
 * Corresponds to OoT Audio_ProcessNotes (original name Nas_UpdateChannel).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void AudioPlayback_ProcessNotes(void)
{
  Note *note;
  NotePlaybackState *playback_state;
  NoteSampleState *note_sub_eu;
  NoteAttributes *attrs;
  s32 pad[2]; // FAKE
  NoteSubAttributes sub_attrs;
  u8 book_offset;
  f32 scale;
  s32 i;
  for (i = 0; i < D_81288D88; i++)
  {
    note = i + (Note*)D_81287B04; // FAKE: index first, for retail's addu operand order
    playback_state = &note->playbackState;
    if (playback_state->parentLayer != ((SequenceLayer *) (-1)))
    {
      if (((u32) playback_state->parentLayer) < 0x7FFFFFFF)
      {
        continue;
      }
      if ((note != playback_state->parentLayer->note) && (playback_state->unk_04 == 0))
      {
        playback_state->adsr.action |= 0x10;
        playback_state->adsr.fadeOutVel = D_81288D74;
        playback_state->priority = 1;
        playback_state->unk_04 = 2;
        goto d;
      }
      else
        if (((!playback_state->parentLayer->enabled) && (playback_state->unk_04 == 0)) && (playback_state->priority >= 1)) // FAKE: no-op ^ 0
      {
      }
      else
        if (playback_state->parentLayer->channel->seqPlayer == 0)
      {
        AudioSeq_SequenceChannelDisable(playback_state->parentLayer->channel);
        playback_state->priority = 1;
        playback_state->unk_04 = 1;
        continue;
      }
      else
        if (playback_state->parentLayer->channel->seqPlayer->muted && (playback_state->parentLayer->channel->muteBehavior & 0x40))
      {
      }
      else
      {
        goto d;
      }
      AudioPlayback_SeqLayerNoteRelease((Particle *) playback_state->parentLayer);
      AudioPlayback_AudioListRemove((s8 *) note);
      AudioPlayback_AudioListPushFront((M2C_UNK*)&note->listItem.pool->decaying, (M2C_UNK *) note);
      playback_state->priority = 1;
      playback_state->unk_04 = 2;
    }
    else
      if ((playback_state->unk_04 == 0) && (playback_state->priority >= 1))
    {
      continue;
    }
    d:
    if (playback_state->priority != 0)
    {
      if (1) // FAKE
      {
        note_sub_eu = &note->sampleState;
      }
      if ((playback_state->unk_04 >= 1) || note_sub_eu->finished)
      {
        if ((playback_state->adsr.state == 0) || note_sub_eu->finished)
        {
          if (playback_state->wantedParentLayer != ((SequenceLayer *) (-1)))
          {
            AudioPlayback_NoteDisable((Note *) note);
            if (playback_state->wantedParentLayer->channel != 0)
            {
              AudioPlayback_NoteInitForLayer((s8 *) note, (s8 *) playback_state->wantedParentLayer);
              AudioEffects_InitVibrato(note);
              AudioPlayback_AudioListRemove((s8 *) note);
              AudioSeq_AudioListPushBack(&note->listItem.pool->active, (struct Note *) note);
              playback_state->wantedParentLayer = (SequenceLayer *) (-1);
              goto process;
            }
            AudioPlayback_NoteDisable((Note *) note);
            AudioPlayback_AudioListRemove((s8 *) note);
            AudioSeq_AudioListPushBack(&note->listItem.pool->disabled, (struct Note *) note);
            playback_state->wantedParentLayer = (SequenceLayer *) (-1);
          }
          else
          {
            if (playback_state->parentLayer != ((SequenceLayer *) (-1)))
            {
              playback_state->parentLayer->bit1 = 1;
            }
            AudioPlayback_NoteDisable((Note *) note);
            AudioPlayback_AudioListRemove((s8 *) note);
            AudioSeq_AudioListPushBack(&note->listItem.pool->disabled, (struct Note *) note);
          }
        }
        else
        {
          goto process;
        }
      }
      else
        if (playback_state->adsr.state == 0)
      {
        if (playback_state->parentLayer != ((SequenceLayer *) (-1)))
        {
          playback_state->parentLayer->bit1 = 1;
        }
        AudioPlayback_NoteDisable((Note *) note);
        AudioPlayback_AudioListRemove((s8 *) note);
        AudioSeq_AudioListPushBack(&note->listItem.pool->disabled, (struct Note *) note);
      }
      else
      {
        process:
        attrs = &playback_state->attributes;

        scale = AudioEffects_UpdateAdsr((u8 *) (&playback_state->adsr));
        AudioEffects_UpdatePortamentoAndVibrato(note);
        if ((playback_state->unk_04 == 1) || (playback_state->unk_04 == 2))
        {
          sub_attrs.freqScale = attrs->freqScale;
          sub_attrs.velocity = attrs->velocity;
          sub_attrs.pan = attrs->pan;
          sub_attrs.reverbVol = attrs->reverbVol;
          sub_attrs.reverbBits = attrs->stereo;
          sub_attrs.gain = attrs->gain;
          sub_attrs.filter = attrs->filter;
          book_offset = note_sub_eu->bookOffset;
        }
        else
        {
          SequenceChannel *channel = playback_state->parentLayer->channel;
          SequenceLayer *layer = playback_state->parentLayer;
          sub_attrs.freqScale = layer->noteFreqScale;
          sub_attrs.velocity = layer->noteVelocity;
          sub_attrs.pan = layer->notePan;
          sub_attrs.reverbBits = layer->stereo;
          sub_attrs.reverbVol = channel->reverb;
          sub_attrs.gain = channel->gain;
          sub_attrs.filter = channel->filter;
          book_offset = channel->bookOffset & 7;
          if (channel->seqPlayer->muted && (channel->muteBehavior & 8))
          {
            sub_attrs.freqScale = 0.0f;
            sub_attrs.velocity = 0.0f;
          }
        }
        sub_attrs.freqScale *= playback_state->vibratoFreqScale * playback_state->portamentoFreqScale;
        sub_attrs.freqScale *= D_81288D70;
        sub_attrs.velocity *= scale;
        AudioPlayback_InitSampleState((s8 *) note, &sub_attrs);
        note_sub_eu->bookOffset = book_offset;
      }
    }

  }

}

/**
 * Returns the low, normal or high sample of instrument `arg0` for semitone `arg1`.
 * Corresponds to OoT Audio_GetInstrumentTunedSample (original name NoteToVoice).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
u8* AudioPlayback_GetInstrumentTunedSample(u8* arg0, s32 arg1) {
    u8* ret;

    if (arg1 < arg0[1]) {
        ret = arg0 + 8;
    } else if (arg0[2] >= arg1) {
        ret = arg0 + 0x10;
    } else {
        ret = arg0 + 0x18;
    }
    return ret;
}


s32 AudioLoad_IsFontLoadComplete(s32 arg0);
extern SoundFont* D_81288D50;
extern s32 D_81288E70;

/**
 * Returns instrument `arg1` of font `arg0`, or NULL when the font is not loaded or the id is out of range.
 * Corresponds to OoT Audio_GetInstrumentInner (original name ProgToVp).
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment guessed from the code.
 */
u8* AudioPlayback_GetInstrumentInner(s32 arg0, s32 arg1) {
    SoundFont* bank;
    u8* ret;

    if (AudioLoad_IsFontLoadComplete(arg0) == 0) {
        D_81288E70 = arg0 + 0x10000000;
        return NULL;
    }
    bank = (SoundFont*)((u8*)D_81288D50 + arg0 * sizeof(SoundFont));
    if (arg1 >= bank->numInstruments) {
        D_81288E70 = (arg0 << 8) + arg1 + 0x03000000;
        return NULL;
    }
    ret = bank->instruments[arg1];
    if (ret == NULL) {
        D_81288E70 = (arg0 << 8) + arg1 + 0x01000000;
        return ret;
    }
    return ret;
}

/**
 * Returns drum `arg1` of font `arg0`, or NULL when the font is not loaded or the id is out of range.
 * Corresponds to OoT Audio_GetDrum (original name PercToPp).
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment guessed from the code.
 */
u8* AudioPlayback_GetDrum(s32 arg0, s32 arg1) {
    SoundFont* bank;
    u8* ret;

    if (AudioLoad_IsFontLoadComplete(arg0) == 0) {
        D_81288E70 = arg0 + 0x10000000;
        return NULL;
    }
    bank = (SoundFont*)((u8*)D_81288D50 + arg0 * sizeof(SoundFont));
    if (arg1 >= bank->numDrums) {
        D_81288E70 = (arg0 << 8) + arg1 + 0x04000000;
        return NULL;
    }
    if ((u32)bank->drums < 0x80000000) {
        return NULL;
    }
    ret = bank->drums[arg1];
    if (ret == NULL) {
        D_81288E70 = (arg0 << 8) + arg1 + 0x05000000;
    }
    return ret;
}

#define ADSR_ACTION_RELEASE_E3C 0x10
#define ADSR_ACTION_DECAY_E3C 0x20

extern f32 D_81288D74;
extern f32 D_81288D78;
void AudioPlayback_AudioListRemove(s8* arg0);
void AudioPlayback_AudioListPushFront(M2C_UNK* arg0, M2C_UNK* arg1);

/**
 * Starts the decay (6) or release (7) of the note a sequence layer is playing.
 * Corresponds to OoT Audio_SeqLayerDecayRelease (original name __Nas_Release_Channel_Main).
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment guessed from the code.
 */
void AudioPlayback_SeqLayerDecayRelease(SequenceLayer* layer, s32 target) {
    Note* note;
    NoteAttributes* attrs;

    if (layer == (SequenceLayer*)-1) {
        return;
    }

    layer->status = 0;

    if (layer->note == NULL) {
        return;
    }

    note = layer->note;
    attrs = &note->playbackState.attributes;

    if (note->playbackState.wantedParentLayer == layer) {
        note->playbackState.wantedParentLayer = (SequenceLayer*)-1;
    }

    if (note->playbackState.parentLayer != layer) {
        if (note->playbackState.parentLayer == (SequenceLayer*)-1 && note->playbackState.wantedParentLayer == (SequenceLayer*)-1 &&
            note->playbackState.prevParentLayer == layer && target != 6) {
            note->playbackState.adsr.fadeOutVel = D_81288D74;
            note->playbackState.adsr.action |= ADSR_ACTION_RELEASE_E3C;
        }
        return;
    }

    if (note->playbackState.adsr.state != 6) {
        attrs->freqScale = layer->noteFreqScale;
        attrs->velocity = layer->noteVelocity;
        attrs->pan = layer->notePan;
        attrs->stereo = layer->stereo;
        if (layer->channel != NULL) {
            attrs->reverbVol = layer->channel->reverb;
            attrs->gain = layer->channel->gain;
            attrs->filter = layer->channel->filter;
            if (layer->channel->seqPlayer->muted && (layer->channel->muteBehavior & 8)) {
                note->sampleState.finished = 1;
            }
            note->playbackState.priority = layer->channel->someOtherPriority;
        } else {
            note->playbackState.priority = 1;
        }

        note->playbackState.prevParentLayer = note->playbackState.parentLayer;
        note->playbackState.parentLayer = (SequenceLayer*)-1;
        if (target == 7) {
            note->playbackState.adsr.fadeOutVel = D_81288D74;
            note->playbackState.adsr.action |= ADSR_ACTION_RELEASE_E3C;
            note->playbackState.unk_04 = 2;
        } else {
            note->playbackState.unk_04 = 1;
            note->playbackState.adsr.action |= ADSR_ACTION_DECAY_E3C;
            if (layer->adsr.decayIndex == 0) {
                note->playbackState.adsr.fadeOutVel = layer->channel->adsr.decayIndex * D_81288D78;
            } else {
                note->playbackState.adsr.fadeOutVel = layer->adsr.decayIndex * D_81288D78;
            }
            note->playbackState.adsr.sustain = ((f32)(s32)layer->channel->adsr.sustain * note->playbackState.adsr.current) / 256.0f;
        }
    }

    if (target == 6) {
        AudioPlayback_AudioListRemove((s8*)note);
        AudioPlayback_AudioListPushFront((M2C_UNK*)&note->listItem.pool->decaying, (M2C_UNK*)note);
    }
}

/**
 * Starts the decay of the layer's note.
 * Corresponds to OoT Audio_SeqLayerNoteDecay (original name Nas_Release_Channel).
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment guessed from the code.
 */
void AudioPlayback_SeqLayerNoteDecay(Particle* arg0) {
    AudioPlayback_SeqLayerDecayRelease((struct SequenceLayer*)arg0, 6);
}

/**
 * Starts the release of the layer's note.
 * Corresponds to OoT Audio_SeqLayerNoteRelease (original name Nas_Release_Channel_Force).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void AudioPlayback_SeqLayerNoteRelease(Particle* arg0) {
    AudioPlayback_SeqLayerDecayRelease((struct SequenceLayer*)arg0, 7);
}

/**
 * Makes an empty circular note list.
 * Corresponds to OoT Audio_InitNoteList (original name __Nas_InitList).
 * Seen in play (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from AudioPlayback_InitNoteLists.
 */
void AudioPlayback_InitNoteList(s8 *arg0) {
    M2C_FIELD(arg0, s8 **, 0) = arg0;
    M2C_FIELD(arg0, s8 **, 4) = arg0;
    M2C_FIELD(arg0, s32 *, 8) = 0;
}

/**
 * Empties the four lists of a NotePool (disabled, decaying, releasing and active at arg0 + 0x00,
 * 0x10, 0x20 and 0x30) with AudioPlayback_InitNoteList and points each list head's pool field
 * (+0xC) at the pool.
 * Corresponds to OoT Audio_InitNoteLists (original name Nas_InitChNode).
 * Seen in play (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from AudioPlayback_InitNoteFreeList.
 */
void AudioPlayback_InitNoteLists(s8 *arg0) {
    AudioPlayback_InitNoteList(arg0);
    AudioPlayback_InitNoteList(arg0 + 0x10);
    AudioPlayback_InitNoteList(arg0 + 0x20);
    AudioPlayback_InitNoteList(arg0 + 0x30);
    M2C_FIELD(arg0, s8 **, 0xC) = arg0;
    M2C_FIELD(arg0, s8 **, 0x1C) = arg0;
    M2C_FIELD(arg0, s8 **, 0x2C) = arg0;
    M2C_FIELD(arg0, s8 **, 0x3C) = arg0;
}

s32 AudioSeq_AudioListPushBack(s8*, struct Note*);
extern Note* D_81287B04;
extern s8 D_81287DD8;
extern s32 D_81288D88;

/**
 * Sets up the global note pool D_81287DD8 and puts all D_81288D88 notes (D_81287B04) on its
 * disabled list: each note's listItem gets the note as its value and is marked as on no list
 * before it is pushed.
 * Corresponds to OoT Audio_InitNoteFreeList (original name Nas_InitChannelList).
 * Seen in play (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from AudioHeap_Init.
 */
void AudioPlayback_InitNoteFreeList(void) {
    s32 i;

    AudioPlayback_InitNoteLists(&D_81287DD8);
    for (i = 0; i < D_81288D88; i++) {
        D_81287B04[i].listItem.u.value = &D_81287B04[i];
        D_81287B04[i].listItem.prev = 0;
        AudioSeq_AudioListPushBack(&D_81287DD8, &D_81287B04[i]);
    }
}

extern s8 D_81287DE8;
extern s8 D_81287DF8;
extern s8 D_81287E08;
void AudioPlayback_AudioListRemove(s8* arg0);

/**
 * Moves every note in the four lists of a note pool back to the matching
 * global free list (D_81287DD8, D_81287DE8, D_81287DF8, D_81287E08). Same
 * structure as note_pool_clear in the Super Mario 64 audio library.
 * Corresponds to OoT Audio_NotePoolClear (original name Nas_DeAllocAllVoices).
 * Seen in play (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from AudioSeq_SequencePlayerDisable.
 */
void AudioPlayback_NotePoolClear(NotePool* pool) {
    s32 i;
    AudioListItem* source;
    AudioListItem* cur;
    s8* dest;

    for (i = 0; i < 4; i++) {
        switch (i) {
            case 0:
                source = &pool->disabled;
                dest = &D_81287DD8;
                break;

            case 1:
                source = &pool->decaying;
                dest = &D_81287DE8;
                break;

            case 2:
                source = &pool->releasing;
                dest = &D_81287DF8;
                break;

            case 3:
                source = &pool->active;
                dest = &D_81287E08;
                break;
        }

        for (;;) {
            cur = source->next;
            if (cur == source) {
                break;
            }
            if (cur == NULL) {
                break;
            }
            AudioPlayback_AudioListRemove((s8*)cur);
            AudioSeq_AudioListPushBack(dest, (Note*)cur);
        }
    }
}

s32 AudioSeq_AudioListPopBack(AudioListItem* list);
/**
 * Takes `arg1` notes from the global free lists into a pool.
 * Corresponds to OoT Audio_NotePoolFill (original name Nas_AllocVoices).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void AudioPlayback_NotePoolFill(s8* arg0, s32 arg1) {
    s32 pad[3]; // FAKE: stack padding; places src/dst at sp+0x48/0x44 and the frame at 0x58 as in the ROM
    s8* src;
    s8* dst;
    s32 n;
    s32 pass;
    s32 r;

    AudioPlayback_NotePoolClear((NotePool*)arg0);
    for (pass = 0, n = 0; n < arg1; pass++) {
        if (pass == 4) {
            return;
        }
        switch (pass) {
            case 0:
                src = &D_81287DD8;
                dst = arg0;
                break;
            case 1:
                src = &D_81287DE8;
                dst = arg0 + 0x10;
                break;
            case 2:
                src = &D_81287DF8;
                dst = arg0 + 0x20;
                break;
            case 3:
                src = &D_81287E08;
                dst = arg0 + 0x30;
                break;
        }
        while (n < arg1) {
            r = AudioSeq_AudioListPopBack(src);
            if (r == 0) {
                break;
            }
            AudioSeq_AudioListPushBack(dst, (Note*)r);
            n++;
        }
    }
}

/**
 * Pushes list item `arg1` onto the front of list `arg0` if it is on no list.
 * Corresponds to OoT Audio_AudioListPushFront (original name Nas_AddListHead).
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment guessed from the code.
 */
void AudioPlayback_AudioListPushFront(M2C_UNK *arg0, M2C_UNK *arg1) {
    if (M2C_FIELD(arg1, M2C_UNK **, 0) == NULL) {
        M2C_FIELD(arg1, M2C_UNK **, 0) = arg0;
        M2C_FIELD(arg1, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(arg0, M2C_UNK **, 4);
        *M2C_FIELD(arg0, M2C_UNK **, 4) = arg1;
        M2C_FIELD(arg0, M2C_UNK **, 4) = arg1;
        M2C_FIELD(arg0, s32 *, 8) = (s32) (M2C_FIELD(arg0, s32 *, 8) + 1);
        M2C_FIELD(arg1, s32 *, 0xC) = (s32) M2C_FIELD(arg0, s32 *, 0xC);
    }
}

/**
 * Unlinks a list item from its list.
 * Corresponds to OoT Audio_AudioListRemove (original name Nas_CutList).
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment guessed from the code.
 */
void AudioPlayback_AudioListRemove(s8 *arg0) {
    s8 *temp_v0;

    temp_v0 = M2C_FIELD(arg0, s8 **, 0);
    if (temp_v0 != NULL) {
        M2C_FIELD(temp_v0, s8 ***, 4) = (s8 **) M2C_FIELD(arg0, s8 ***, 4);
        *M2C_FIELD(arg0, s8 ***, 4) = M2C_FIELD(arg0, s8 **, 0);
        M2C_FIELD(arg0, s8 **, 0) = NULL;
    }
}

/**
 * Returns the note in list `arg0` with the lowest priority below `arg1`, or NULL.
 * Corresponds to OoT Audio_FindNodeWithPrioLessThan (original name __Nas_GetLowerPrio).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
s8 *AudioPlayback_FindNodeWithPrioLessThan(s8 *arg0, s32 arg1) {
    s8 *temp_a0;
    s8 *var_v0;
    s8 *var_v1;

    var_v0 = M2C_FIELD(arg0, s8 **, 4);
    if (var_v0 == arg0) {
        return NULL;
    }
    var_v1 = var_v0;
    if (var_v0 != arg0) {
        do {
            if ((s32) M2C_FIELD(M2C_FIELD(var_v1, s8 **, 8), u8 *, 0x30) >= (s32) M2C_FIELD(M2C_FIELD(var_v0, s8 **, 8), u8 *, 0x30)) {
                var_v1 = var_v0;
            }
            var_v0 = M2C_FIELD(var_v0, s8 **, 4);
        } while (var_v0 != arg0);
    }
    if (var_v1 == NULL) {
        return NULL;
    }
    temp_a0 = M2C_FIELD(var_v1, s8 **, 8);
    if ((s32) M2C_FIELD(temp_a0, u8 *, 0x30) >= arg1) {
        return NULL;
    }
    return temp_a0;
}

/**
 * Gives note `arg0` to layer `arg1` and sets it up from the layer's instrument.
 * Corresponds to OoT Audio_NoteInitForLayer (original name Nas_EntryTrack). OoT adds synthetic
 * waves.
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void AudioPlayback_NoteInitForLayer(s8* arg0, s8* arg1) {
    NoteSampleState* flags;
    s32 v;

    M2C_FIELD(arg0, s32*, 0x40) = -1;
    M2C_FIELD(arg0, s8**, 0x44) = arg1;
    M2C_FIELD(arg0, u8*, 0x30) = M2C_FIELD(M2C_FIELD(arg1, u8**, 0x54), u8*, 5);
    ((SequenceLayer*)arg1)->notePropertiesNeedInit = 1;
    M2C_FIELD(arg1, u8*, 3) = 3;
    M2C_FIELD(arg1, s8**, 0x48) = arg0;
    M2C_FIELD(M2C_FIELD(arg1, s8**, 0x54), s8**, 0x40) = arg0;
    M2C_FIELD(M2C_FIELD(arg1, s8**, 0x54), s8**, 0x44) = arg1;
    M2C_FIELD(arg1, f32*, 0x34) = 0.0f;
    AudioPlayback_NoteInit((Note*)arg0);
    v = M2C_FIELD(arg1, u8*, 2);
    flags = (NoteSampleState*)(arg0 + 0xB4);
    if (v == 0xFF) {
        v = M2C_FIELD(M2C_FIELD(arg1, s8**, 0x54), s16*, 0x22);
    }
    flags->tunedSample = M2C_FIELD(arg1, s32*, 0x50);
    if (v >= 0x80 && v < 0xC0) {
        flags->isSyntheticWave = 1;
    } else {
        flags->isSyntheticWave = 0;
    }
    M2C_FIELD(arg0, u8*, 0x33) = M2C_FIELD(M2C_FIELD(arg1, u8**, 0x54), u8*, 7);
    flags->stereoHeadsetEffects = M2C_FIELD(arg1, SequenceChannel**, 0x54)->stereoHeadsetEffects;
    flags->reverbIndex = M2C_FIELD(M2C_FIELD(arg1, u8**, 0x54), u8*, 8) & 3;
}


/**
 * Releases the note's current layer and queues layer `arg1` to take it over.
 * Corresponds to OoT func_800E82C0 (original name __Nas_InterTrack). OoT leaves it unnamed (MM:
 * func_801963E8).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void AudioPlayback_NoteReleaseOwnerAndQueueLayer(s8 *arg0, s32 arg1) {
    AudioPlayback_SeqLayerNoteRelease(M2C_FIELD(arg0, s32 *, 0x44));
    M2C_FIELD(arg0, s32 *, 0x48) = arg1;
}

extern f32 D_81288D74;
/**
 * Starts releasing the note and hands it to layer `arg1` with the layer's priority.
 * Corresponds to OoT Audio_NoteReleaseAndTakeOwnership (original name __Nas_InterReleaseTrack).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void AudioPlayback_NoteReleaseAndTakeOwnership(s8 *arg0, s8 *arg1)
{
  *((s8 **) (((s8 *) arg0) + 0x48)) = arg1; // FAKE: decomp slop, raw pointer casts stand in for struct fields
  *((u8 *) (((s8 *) arg0) + 0x30)) = (u8) (*((u8 *) (((s8 *) (*((s8 **) (((s8 *) arg1) + 0x54)))) + 5))); // FAKE: decomp slop, raw pointer casts stand in for struct fields
  *((f32 *) (((s8 *) arg0) + 0x6C)) = (f32) D_81288D74; // FAKE: decomp slop, raw pointer casts stand in for struct fields
  *((u8 *) (((s8 *) arg0) + 0x5C)) = (u8) ((*((u8 *) (((s8 *) arg0) + 0x5C))) | 0x10); // FAKE: decomp slop, raw pointer casts stand in for struct fields
}

s32 AudioSeq_AudioListPopBack(AudioListItem* list);
void AudioPlayback_NoteInitForLayer(s8* arg0, s8* arg1);

/**
 * Allocates a note for a layer from a pool's disabled list.
 * Corresponds to OoT Audio_AllocNoteFromDisabled.
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
s32 AudioPlayback_AllocNoteFromDisabled(s8 *arg0, s32 arg1) {
    s32 ret;

    ret = AudioSeq_AudioListPopBack(arg0);
    if (ret != 0) {
        AudioPlayback_NoteInitForLayer((s8*)ret, (s8*)arg1);
        AudioPlayback_AudioListPushFront((M2C_UNK *) (arg0 + 0x30), (M2C_UNK *) ret);
    }
    return ret;
}

s32 AudioSeq_AudioListPopBack(AudioListItem* list);

/**
 * Takes over a decaying note for a layer.
 * Corresponds to OoT Audio_AllocNoteFromDecaying.
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
s32 AudioPlayback_AllocNoteFromDecaying(s8 *arg0, s8 *arg1) {
    s32 ret;

    ret = AudioSeq_AudioListPopBack(arg0 + 0x10);
    if (ret != 0) {
        AudioPlayback_NoteReleaseAndTakeOwnership((s8 *) ret, arg1);
        AudioSeq_AudioListPushBack(arg0 + 0x20, (s8 *) ret);
    }
    return ret;
}

/**
 * Steals the lowest-priority active note for a layer.
 * Corresponds to OoT Audio_AllocNoteFromActive (original name __Nas_ChLookRelWait).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
s8* AudioPlayback_AllocNoteFromActive(s8* pool, s8* layer) {
    s8* r_note;
    s8* a_note;
    s32 r_priority;
    s32 a_priority;

    r_priority = a_priority = 0x10;
    r_note = AudioPlayback_FindNodeWithPrioLessThan(pool + 0x20, M2C_FIELD(M2C_FIELD(layer, u8**, 0x54), u8*, 5));
    if (r_note != NULL) {
        r_priority = M2C_FIELD(r_note, u8*, 0x30);
    }
    a_note = AudioPlayback_FindNodeWithPrioLessThan(pool + 0x30, M2C_FIELD(M2C_FIELD(layer, u8**, 0x54), u8*, 5));
    if (a_note != NULL) {
        a_priority = M2C_FIELD(a_note, u8*, 0x30);
    }
    if ((r_note == NULL) && (a_note == NULL)) {
        return NULL;
    }
    if (a_priority < r_priority) {
        AudioPlayback_AudioListRemove(a_note);
        AudioPlayback_NoteReleaseOwnerAndQueueLayer(a_note, (s32)layer);
        AudioSeq_AudioListPushBack(pool + 0x20, (Note*)a_note);
        M2C_FIELD(a_note, u8*, 0x30) = M2C_FIELD(M2C_FIELD(layer, u8**, 0x54), u8*, 5);
        return a_note;
    }
    M2C_FIELD(r_note, s8**, 0x48) = layer;
    M2C_FIELD(r_note, u8*, 0x30) = M2C_FIELD(M2C_FIELD(layer, u8**, 0x54), u8*, 5);
    return r_note;
}

/**
 * Allocates a note for a layer by its channel's note-stealing policy.
 * Corresponds to OoT Audio_AllocNote (original name Nas_AllocationOnRequest).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
s8* AudioPlayback_AllocNote(s8* arg0) {
    s8* voice;
    s32 flags = M2C_FIELD(arg0, u8**, 0x54)[2];

    if (flags & 1) {
        voice = M2C_FIELD(arg0, s8**, 0x48);
        if ((voice != NULL) && (arg0 == M2C_FIELD(voice, s8**, 0x40)) && (M2C_FIELD(voice, s32*, 0x48) == -1)) {
            AudioPlayback_NoteReleaseAndTakeOwnership(voice, arg0);
            AudioPlayback_AudioListRemove(voice);
            AudioSeq_AudioListPushBack(M2C_FIELD(voice, s8**, 0xC) + 0x20, (Note*)voice);
            return voice;
        }
    }
    if (flags & 2) {
        if (((voice = (s8*)AudioPlayback_AllocNoteFromDisabled(M2C_FIELD(arg0, s8**, 0x54) + 0x84, (s32)arg0)) != NULL) ||
            ((voice = (s8*)AudioPlayback_AllocNoteFromDecaying(M2C_FIELD(arg0, s8**, 0x54) + 0x84, arg0)) != NULL) ||
            ((voice = AudioPlayback_AllocNoteFromActive(M2C_FIELD(arg0, s8**, 0x54) + 0x84, arg0)) != NULL)) {
            return voice;
        }
    } else if (flags & 4) {
        if (((voice = (s8*)AudioPlayback_AllocNoteFromDisabled(M2C_FIELD(arg0, s8**, 0x54) + 0x84, (s32)arg0)) != NULL) ||
            ((voice = (s8*)AudioPlayback_AllocNoteFromDisabled(M2C_FIELD(M2C_FIELD(arg0, s8**, 0x54), s8**, 0x4C) + 0x98, (s32)arg0)) != NULL) ||
            ((voice = (s8*)AudioPlayback_AllocNoteFromDecaying(M2C_FIELD(arg0, s8**, 0x54) + 0x84, arg0)) != NULL) ||
            ((voice = (s8*)AudioPlayback_AllocNoteFromDecaying(M2C_FIELD(M2C_FIELD(arg0, s8**, 0x54), s8**, 0x4C) + 0x98, arg0)) != NULL) ||
            ((voice = AudioPlayback_AllocNoteFromActive(M2C_FIELD(arg0, s8**, 0x54) + 0x84, arg0)) != NULL) ||
            ((voice = AudioPlayback_AllocNoteFromActive(M2C_FIELD(M2C_FIELD(arg0, s8**, 0x54), s8**, 0x4C) + 0x98, arg0)) != NULL)) {
            return voice;
        }
    } else if (flags & 8) {
        if (((voice = (s8*)AudioPlayback_AllocNoteFromDisabled(&D_81287DD8, (s32)arg0)) != NULL) ||
            ((voice = (s8*)AudioPlayback_AllocNoteFromDecaying(&D_81287DD8, arg0)) != NULL) ||
            ((voice = AudioPlayback_AllocNoteFromActive(&D_81287DD8, arg0)) != NULL)) {
            return voice;
        }
    } else {
        if (((voice = (s8*)AudioPlayback_AllocNoteFromDisabled(M2C_FIELD(arg0, s8**, 0x54) + 0x84, (s32)arg0)) != NULL) ||
            ((voice = (s8*)AudioPlayback_AllocNoteFromDisabled(&D_81287DD8, (s32)arg0)) != NULL) ||
            ((voice = (s8*)AudioPlayback_AllocNoteFromDecaying(M2C_FIELD(arg0, s8**, 0x54) + 0x84, arg0)) != NULL) ||
            ((voice = (s8*)AudioPlayback_AllocNoteFromDecaying(&D_81287DD8, arg0)) != NULL) ||
            ((voice = AudioPlayback_AllocNoteFromActive(M2C_FIELD(arg0, s8**, 0x54) + 0x84, arg0)) != NULL) ||
            ((voice = AudioPlayback_AllocNoteFromActive(&D_81287DD8, arg0)) != NULL)) {
            return voice;
        }
    }
    arg0[3] = 0;
    return NULL;
}

extern NoteSampleState D_81239E18;
extern s8 D_81287760;
s32 GbApu_Alloc(s8 *, s32);
/**
 * Resets every note's state and sample buffers.
 * Corresponds to OoT Audio_NoteInitAll (original name Nas_ChannelInit). Zero allocates with
 * GbApu_Alloc; OoT uses AudioHeap_AllocDmaMemory.
 * Seen in play (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in the
 * Transfer Pak; called from AudioHeap_Init.
 */
void AudioPlayback_NoteInitAll(void) {
    s32 i;
    Note* rec;

    for (i = 0; i < D_81288D88; i++) {
        rec = (Note*)&D_81287B04[i];
        rec->sampleState = D_81239E18;
        rec->playbackState.priority = 0;
        rec->playbackState.unk_04 = 0;
        rec->playbackState.parentLayer = -1;
        rec->playbackState.wantedParentLayer = -1;
        rec->playbackState.prevParentLayer = -1;
        rec->playbackState.waveId = 0;
        rec->playbackState.attributes.velocity = 0.0f;
        rec->playbackState.adsrVolScale = 0;
        rec->playbackState.adsr.state = 0;
        rec->playbackState.adsr.action = 0;
        rec->playbackState.vibratoState.active = 0;
        rec->playbackState.portamento.cur = 0.0f;
        rec->playbackState.portamento.speed = 0.0f;
        rec->synthesisState.synthesisBuffers = GbApu_Alloc(&D_81287760, 0x80);
    }
}
