/* JP-only TU, ported from pokestadium0 (Stadium Zero, fragment000_80C00). */
/* Audio driver, matching OoT src/audio/internal/seqplayer.c in function order. The driver is
 * Nintendo's "Nas" library (original names Nas_*); this revision sits between SM64 Shindou and
 * OoT. pokestadium0's docs/annotation/audio-driver.md has the per-function mapping. */
/* Fixed-layout split; original translation-unit boundaries remain unconfirmed. */
#include "gb_tower_emulator_jp.h"

typedef struct SeqScriptState811126B8 {
    /* 0x00 */ u8* pc;
    /* 0x04 */ u8* stack[4];
    /* 0x14 */ u8 remLoopIters[4];
    /* 0x18 */ u8 depth;
    /* 0x19 */ s8 value;
} SeqScriptState811126B8;

typedef struct SampleStep5 {
    /* 0x00 */ u32 codec : 4;
    /* 0x00 */ u32 medium : 2;
    /* 0x00 */ u32 rest : 26;
} SampleStep5;

typedef struct SoundStep5 {
    /* 0x00 */ SampleStep5* sample;
} SoundStep5;

typedef struct SequenceLayerFlagsU32 {
    /* 0x00 */ u32 enabled : 1;
    /* 0x00 */ u32 finished : 1;
    /* 0x00 */ u32 stopSomething : 1;
    /* 0x00 */ u32 continuousNotes : 1;
    /* 0x00 */ u32 bit3 : 1;
    /* 0x00 */ u32 notePropertiesNeedInit : 1;
    /* 0x00 */ u32 ignoreDrumPan : 1;
    /* 0x00 */ u32 bit1 : 1;
    /* 0x01 */ u32 pad01 : 16;
    /* 0x03 */ u8 status;
    /* 0x04 */ u8 pad04[0x48 - 0x04];
    /* 0x48 */ Note* note;
    /* 0x4C */ u8 pad4C[4];
    /* 0x50 */ struct SoundStep5* sound;
} SequenceLayerFlagsU32;


// Declared here instead of in a header: the callers of these disagree on their types.
extern SequencePlayer D_81287B08[];
extern AudioBufferParameters sGbAudioOutputConfig;
extern volatile u32 D_812346FC;
void AudioPlayback_NotePoolClear(s8*);
void AudioPlayback_SeqLayerNoteDecay(SequenceLayer*);
void AudioSeq_AudioListPushBack(s8 *arg0, s8 *arg1);

extern u8 D_8123BB00[];
void AudioSeq_SequencePlayerDisableChannels(u8* seq_player, s32 channel_bits);
s32 AudioLoad_IsFontLoadComplete(s32);
s32 AudioLoad_IsSeqLoadComplete(s32);
void AudioLoad_SetFontLoadStatus(s32, s32);
void AudioLoad_SetSeqLoadStatus(s32, s32);
extern M2C_UNK D_81287820;
u8* AudioPlayback_GetInstrumentInner(s32, s32);
extern u16 D_81138A96;
extern u32 D_81138A90;
extern s16 D_81180700;
extern s16 D_81180702;
extern u8* D_81180704;
extern GbApuWrite D_81180848[GB_APU_WRITE_QUEUE_LEN];
extern u8 D_81182468[0x40];
extern s32 D_811824A8;
extern GbApuStatusOverride D_811824B8;
extern OSMesgQueue D_811824C0;
extern OSMesg D_811824D8[GB_APU_WRITE_QUEUE_LEN + 1];
extern AudioBufferParameters sGbAudioOutputConfig;
extern volatile s32 sGbAudioFrameCount;
extern s32 D_812346D0;
extern s32 sGbAudioBufferIndex;
extern s16* sGbAudioBuffers[3];
extern s16 sGbAudioBufferLengths[3];
extern u32 D_812346F4;
extern volatile u32 D_812346FC;
extern SequencePlayer D_81287B08_jp[];
extern OSMesgQueue* D_812896A0_jp;
extern s32 D_81138A70;
extern u8 D_81138A80[0x10];
extern s32 D_81138AD0;
extern s32 D_81138AD4;
extern s32 D_81138AD8;
extern u16 D_81138ADC[32];
extern u8 D_81138B1C[0x10];
extern u8 D_81138B2C[0x4];
extern u8 D_81138B30[0x10];
extern u8 D_81138B40[0x10];
extern u32 D_81138B50[9];
extern s32 D_81138B80;
extern GbAudioStream D_81138B84;
extern struct AudioListItem D_81287DC8;
extern AudioListItem D_81287DC8;
extern SequenceLayer D_81287C68[1];
extern s16 D_81288D6C;
extern SequencePlayer D_81287B08[];

/**
 * Reads the argument of a control-flow command of kind `kind` from the script stream.
 * Corresponds to OoT AudioSeq_GetScriptControlFlowArgument (original name Convert_Com).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
u16 AudioSeq_GetScriptControlFlowArgument(u8** stream, u8 kind) {
    u8 flags = D_8123BB00[kind];
    u16 value = 0;

    if (((flags & 0xFF) & 3) == 1) {
        if (!(flags & 0x80)) {
            value = AudioSeq_ScriptReadU8(stream);
        } else {
            value = AudioSeq_ScriptReadS16(stream);
        }
    }
    return value;
}


u16 AudioSeq_ScriptReadCompressedU16(u8** arg0);

/**
 * Runs a script control-flow command (end, call, loop, jump, branch).
 * Corresponds to OoT AudioSeq_HandleScriptFlowControl (original name Common_Com).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
s32 AudioSeq_HandleScriptFlowControl(SequencePlayer* seq_player, SeqScriptState811126B8* state, s32 cmd, s32 cmd_arg) {
    u8 depth;

    switch (cmd) {
        case 0xFF:
            if (state->depth == 0) {
                return -1;
            }
            state->pc = state->stack[--state->depth];
            break;
        case 0xFD:
            return AudioSeq_ScriptReadCompressedU16(&state->pc);
        case 0xFE:
            return 1;
        case 0xFC:
            depth = state->depth;
            state->stack[depth] = state->pc;
            state->depth++;
            state->pc = seq_player->seqData + (u16)cmd_arg;
            break;
        case 0xF8:
            depth = state->depth;
            state->remLoopIters[depth] = cmd_arg;
            state->stack[state->depth++] = state->pc;
            break;
        case 0xF7:
            state->remLoopIters[state->depth - 1]--;
            if (state->remLoopIters[state->depth - 1] != 0) {
                state->pc = state->stack[state->depth - 1];
            } else {
                state->depth--;
            }
            break;
        case 0xF6:
            state->depth--;
            break;
        case 0xF5:
        case 0xF9:
        case 0xFA:
        case 0xFB:
            if (cmd == 0xFA && state->value != 0) {
                break;
            }
            if (cmd == 0xF9 && state->value >= 0) {
                break;
            }
            if (cmd == 0xF5 && state->value < 0) {
                break;
            }
            state->pc = seq_player->seqData + (u16)cmd_arg;
            break;
        case 0xF2:
        case 0xF3:
        case 0xF4:
            if (cmd == 0xF3 && state->value != 0) {
                break;
            }
            if (cmd == 0xF2 && state->value >= 0) {
                break;
            }
            state->pc += (s8)(cmd_arg & 0xFF);
            break;
    }
    return 0;
}

extern SequenceChannel D_81287CF0;

void AudioPlayback_InitNoteLists(void*);
extern u8 D_81239E08[];
/**
 * Resets a sequence channel to its defaults.
 * Corresponds to OoT AudioSeq_InitSequenceChannel (original name Nas_InitSubTrack).
 * Seen in play in pokestadium0 (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play in pokestadium0 (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in
 * the Transfer Pak; called from AudioSeq_ResetSequencePlayer.
 */
void AudioSeq_InitSequenceChannel(void* obj) {
    SequenceChannel* channel = obj;
    s32 i;

    if (channel != &D_81287CF0) {
        channel->enabled = 0;
        channel->finished = 0;
        channel->stopScript = 0;
        channel->muted = 0;
        channel->hasInstrument = 0;
        channel->stereoHeadsetEffects = 0;
        channel->transposition = 0;
        channel->largeNotes = 0;
        channel->bookOffset = 0;
        channel->changes.asByte = 0xFF;
        channel->scriptState.depth = 0;
        channel->newPan = 0x40;
        channel->panChannelWeight = 0x80;
        channel->velocityRandomVariance = 0;
        channel->gateTimeRandomVariance = 0;
        channel->noteUnused = 0;
        channel->reverbIndex = 0;
        channel->reverb = 0;
        channel->gain = 0;
        channel->notePriority = 3;
        channel->someOtherPriority = 1;
        channel->delay = 0;
        channel->adsr.envelope = D_81239E08;
        channel->adsr.decayIndex = 0x20;
        channel->vibratoRateTarget = 0x800;
        channel->vibratoRateStart = 0x800;
        channel->adsr.sustain = 0;
        channel->vibratoExtentTarget = 0;
        channel->vibratoExtentStart = 0;
        channel->vibratoRateChangeDelay = 0;
        channel->vibratoExtentChangeDelay = 0;
        channel->vibratoDelay = 0;
        channel->filter = 0;
        channel->combFilterGain = 0;
        channel->combFilterSize = 0;
        channel->volume = 1.0f;
        channel->volumeScale = 1.0f;
        channel->freqScale = 1.0f;
        for (i = 0; i < 8; i++) {
            channel->soundScriptIO[i] = -1;
        }
        channel->unused = 0;
        AudioPlayback_InitNoteLists(&channel->notePool);
    }
}

extern struct AudioListItem D_81287DC8;
s32 AudioSeq_AudioListPopBack(AudioListItem* list);
void AudioPlayback_SeqLayerNoteDecay(SequenceLayer*);
/**
 * Gives channel `arg0` a layer in slot `arg1`, taking one from the free list when needed.
 * Corresponds to OoT AudioSeq_SeqChannelSetLayer (original name Nas_EntryNoteTrack).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
s32 AudioSeq_SeqChannelSetLayer(SequenceChannel* arg0, s32 arg1) {
    SequenceLayer* p;
    s32 pad; // FAKE: stack padding
    if (arg0->layers[arg1] == NULL) {
        arg0->layers[arg1] = (SequenceLayer*)AudioSeq_AudioListPopBack((s8*)&D_81287DC8);
        if (arg0->layers[arg1] == NULL) {
            arg0->layers[arg1] = NULL;
            return -1;
        }
    } else {
        AudioPlayback_SeqLayerNoteDecay(arg0->layers[arg1]);
    }
    p = arg0->layers[arg1];
    p->channel = arg0;
    p->adsr = arg0->adsr;
    p->enabled = 1;
    p->stopSomething = 0;
    p->continuousNotes = 0;
    p->finished = 0;
    p->ignoreDrumPan = 0;
    p->adsr.decayIndex = 0;
    p->bit1 = 0;
    p->stereo.asByte = 0x40;
    p->portamento.mode = 0;
    p->scriptState.depth = 0;
    p->status = 0;
    p->noteDuration = 0x80;
    p->pan = 0x40;
    p->transposition = 0;
    p->delay = 0;
    p->gateDelay = 0;
    p->duration = 0;
    p->note = 0;
    p->instrument = 0;
    p->instOrWave = 0xFF;
    p->freqScale = 1.0f;
    p->bend = 1.0f;
    p->velocitySquare2 = 0.0f;
    return 0;
}

/**
 * Decays the layer's note and turns the layer off.
 * Corresponds to OoT AudioSeq_SeqLayerDisable (original name Nas_ReleaseNoteTrack).
 * Not reached in play (2026-11-13): did not run in a GB Player session (boot, Continue, menus) or
 * in a tour of the menus and a COM battle.
 * @unverified Name and comment guessed from the code.
 */
void AudioSeq_SeqLayerDisable(SequenceLayer* arg0) {
    if (arg0 != NULL) {
        AudioPlayback_SeqLayerNoteDecay(arg0);
        arg0->enabled = 0;
        arg0->finished = 1;
    }
}

/**
 * Returns layer slot `arg1` of a channel to the free list.
 * Corresponds to OoT AudioSeq_SeqLayerFree (original name Nas_CloseNoteTrack).
 * Seen in play in pokestadium0 (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play in pokestadium0 (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in
 * the Transfer Pak; called from AudioSeq_SequenceChannelDisable.
 */
void AudioSeq_SeqLayerFree(s8** arg0, s32 arg1) {
    s8* temp = arg0[arg1 + 0x14];

    if (temp != NULL) {
        AudioSeq_AudioListPushBack((s8*)&D_81287DC8, temp + 0x74);
        AudioSeq_SeqLayerDisable((SequenceLayer*)temp);
        arg0[arg1 + 0x14] = NULL;
    }
}

/**
 * Turns a sequence channel off and frees its layers and notes.
 * Corresponds to OoT AudioSeq_SequenceChannelDisable (original name Nas_ReleaseSubTrack). Stadium
 * 1's name Particle31_ResetAllSlots did not fit.
 * Seen in play in pokestadium0 (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in
 * the Transfer Pak; called from AudioSeq_SequencePlayerDisableChannels.
 */
void AudioSeq_SequenceChannelDisable(SequenceChannel* channel) {
    s32 i;

    for (i = 0; i < 4; i++) {
        AudioSeq_SeqLayerFree(channel, i);
    }
    AudioPlayback_NotePoolClear((s8*)channel + 0x84);
    channel->enabled = 0;
    channel->finished = 1;
}

/**
 * Resets each channel selected in `mask`.
 * Corresponds to OoT AudioSeq_SequencePlayerSetupChannels (original name Nas_AllocSub).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void AudioSeq_SequencePlayerSetupChannels(SequencePlayer* seq_player, u16 mask) {
    s32 i;
    SequenceChannel* channel;

    for (i = 0; i < 16; i++) {
        if (mask & 1) {
            channel = seq_player->channels[i];
            channel->fontId = seq_player->defaultFont;
            channel->muteBehavior = seq_player->muteBehavior;
            channel->noteAllocPolicy = seq_player->noteAllocPolicy;
        }
        mask = (s32)mask >> 1;
    }
}

void AudioSeq_SequenceChannelDisable(SequenceChannel*);

/**
 * Disables each channel selected in `channel_bits`.
 * Corresponds to OoT AudioSeq_SequencePlayerDisableChannels (original name Nas_DeAllocSub).
 * Seen in play in pokestadium0 (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play in pokestadium0 (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in
 * the Transfer Pak; called from AudioSeq_SequencePlayerDisable.
 */
void AudioSeq_SequencePlayerDisableChannels(u8* seq_player, s32 channel_bits) {
    SequenceChannel* channel;
    s32 i;

    for (i = 0; i < 16; i++) {
        channel = M2C_FIELD(seq_player, SequenceChannel**, 0x34 + i * 4);
        if (((u32)channel != (u32)&D_81287CF0) == 1) {
            AudioSeq_SequenceChannelDisable(channel);
        }
    }
}

/**
 * Starts channel `arg1` at script address `arg2`.
 * Corresponds to OoT AudioSeq_SequenceChannelEnable (original name Nas_OpenSub).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void AudioSeq_SequenceChannelEnable(SequencePlayer* seq_player, u8 channel_index, void* script) {
    SequenceChannel* p = seq_player->channels[channel_index];
    s32 i;

    p->scriptState.depth = 0;
    p->scriptState.pc = script;
    p->enabled = 1;
    p->finished = 0;
    p->delay = 0;
    for (i = 0; i < 4; i++) {
        if (p->layers[i] != NULL) {
            AudioSeq_SeqLayerFree((s8**)p, i);
        }
    }
}

/**
 * Stops a sequence player: disables its channels, clears its note pool and marks its data discardable.
 * Corresponds to OoT AudioSeq_SequencePlayerDisable (original name Nas_ReleaseGroup). OoT also
 * releases the notes of the player's fonts (AudioHeap_ReleaseNotesForFont).
 * Seen in play in pokestadium0 (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play in pokestadium0 (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in
 * the Transfer Pak; called from AudioSeq_ResetSequencePlayer.
 */
void AudioSeq_SequencePlayerDisable(u8* arg0) {
    AudioSeq_SequencePlayerDisableChannels(arg0, 0xFFFF);
    AudioPlayback_NotePoolClear((s8*)arg0 + 0x98);
    ((SequencePlayer*)arg0)->finished = 1;
    ((SequencePlayer*)arg0)->enabled = 0;
    if (AudioLoad_IsSeqLoadComplete(arg0[4]) != 0) {
        AudioLoad_SetSeqLoadStatus(arg0[4], 3);
    }
    if (AudioLoad_IsFontLoadComplete(arg0[5]) != 0) {
        AudioLoad_SetFontLoadStatus(arg0[5], 4);
    }
    if (arg0[5] == M2C_FIELD(&D_81287820, s16*, 0x3E)) {
        M2C_FIELD(&D_81287820, s32*, 0x20) = 1;
    } else if (arg0[5] == M2C_FIELD(&D_81287820, s16*, 0x4A)) {
        M2C_FIELD(&D_81287820, s32*, 0x20) = 0;
    }
}

/**
 * Appends an item to the end of a list if it is on no list.
 * Corresponds to OoT AudioSeq_AudioListPushBack (original name Nas_AddList).
 * Seen in play in pokestadium0 (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play in pokestadium0 (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in
 * the Transfer Pak; called from AudioPlayback_InitNoteFreeList.
 */
void AudioSeq_AudioListPushBack(s8 *arg0, s8 *arg1)
{
  if ((*((s8 **) (((s8 *) arg1) + 0))) == 0) // FAKE: decomp slop, raw pointer casts stand in for struct fields
  {
    *((s8 **) (((s8 *) (*((s8 **) (((s8 *) arg0) + 0)))) + 4)) = arg1; // FAKE: decomp slop, raw pointer casts stand in for struct fields
    *((s8 **) (((s8 *) arg1) + 0)) = (s8 *) (*((s8 **) (((s8 *) arg0) + 0))); // FAKE: decomp slop, raw pointer casts stand in for struct fields
    *((s8 **) (((s8 *) arg1) + 4)) = arg0; // FAKE: decomp slop, raw pointer casts stand in for struct fields
    *((s8 **) (((s8 *) arg0) + 0)) = arg1; // FAKE: decomp slop, raw pointer casts stand in for struct fields
    *((s32 *) (((s8 *) arg0) + 8)) = ((s32) ((*((s32 *) (((s8 *) arg0) + 8))) + 1)); // FAKE: decomp slop, raw pointer casts stand in for struct fields
    *((s32 *) (((s8 *) arg1) + 0xC)) = (s32) (*((s32 *) (((s8 *) arg0) + 0xC))); // FAKE: decomp slop, raw pointer casts stand in for struct fields
  }
}

/**
 * Removes and returns the last item of a list, or 0 when it is empty.
 * Corresponds to OoT AudioSeq_AudioListPopBack (original name Nas_GetList).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
s32 AudioSeq_AudioListPopBack(AudioListItem* list) {
    AudioListItem* item;

    item = list->prev;
    if (item == list) {
        return 0;
    }
    item->prev->next = list;
    list->prev = item->prev;
    item->prev = NULL;
    list->u.count = (s32)(list->u.count - 1);
    return (s32)item->u.value;
}

/**
 * Puts every sequence layer on the free list.
 * Corresponds to OoT AudioSeq_InitLayerFreelist (original name Nas_InitNoteList).
 * Seen in play in pokestadium0 (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play in pokestadium0 (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in
 * the Transfer Pak; called from AudioSeq_InitSequencePlayers.
 */
void AudioSeq_InitLayerFreelist(void) {
    s32 i;

    D_81287DC8.prev = &D_81287DC8;
    D_81287DC8.next = &D_81287DC8;
    D_81287DC8.u.count = 0;
    D_81287DC8.pool = 0;
    for (i = 0; i < 1; i++) {
        D_81287C68[i].listItem.u.value = &D_81287C68[i];
        D_81287C68[i].listItem.prev = NULL;
        AudioSeq_AudioListPushBack((s8*)&D_81287DC8, (s8*)&D_81287C68[i].listItem);
    }
}

/**
 * Reads one byte from the script stream.
 * Corresponds to OoT AudioSeq_ScriptReadU8 (original name Nas_ReadByteData).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
u8 AudioSeq_ScriptReadU8(u8 **arg0)
{
  u8 new_var; // FAKE: permuter temp
  u8 *temp_a1;
  new_var = *(*arg0); // FAKE: permuter temp
  temp_a1 = *arg0;
  *arg0 = temp_a1 + 1;
  return new_var; // FAKE: permuter temp
}

/**
 * Reads a big-endian 16-bit value from the script stream.
 * Corresponds to OoT AudioSeq_ScriptReadS16 (original name Nas_ReadWordData).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
s16 AudioSeq_ScriptReadS16(u8** arg0) {
    s16 tmp = *(*arg0)++ << 8;

    tmp = *(*arg0)++ | tmp;
    return tmp;
}

/**
 * AudioSeq_ScriptReadCompressedU16
 *
 * Reads a one- or two-byte value from the byte stream whose current position
 * is stored at *arg0, advancing that position. If the first byte has bit 7
 * clear it is the result. Otherwise the result is ((first & 0x7F) << 8) |
 * second byte, and the position advances past both bytes.
 * Corresponds to OoT AudioSeq_ScriptReadCompressedU16 (original name Nas_ReadLengthData).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name guessed from the code; comment kept.
 */
u16 AudioSeq_ScriptReadCompressedU16(u8** arg0) {
    u16 v;

    v = *(*arg0)++;
    if (v & 0x80) {
        v = (v << 8) & 0x7F00;
        v = *(*arg0)++ | v;
    }
    return v;
}

void AudioSeq_SeqLayerProcessScriptStep1(SequenceLayer* arg0);
s32 AudioSeq_SeqLayerProcessScriptStep2(SequenceLayer* layer);
s32 AudioSeq_SeqLayerProcessScriptStep3(SequenceLayer* layer, s32 cmd);
s32 AudioSeq_SeqLayerProcessScriptStep4(SequenceLayer* layer, s32 cmd_semitone);
s32 AudioSeq_SeqLayerProcessScriptStep5(SequenceLayerFlagsU32* layer, u32 same_sound);
/**
 * Runs a layer's script for one tick.
 * Corresponds to OoT AudioSeq_SeqLayerProcessScript (original name Nas_NoteSeq).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void AudioSeq_SeqLayerProcessScript(SequenceLayer* arg0) {
    s32 ret;
    if (arg0->enabled) {
        if (arg0->delay >= 2) {
            arg0->delay--;
            if (!arg0->stopSomething && arg0->gateDelay >= arg0->delay) {
                AudioPlayback_SeqLayerNoteDecay((SequenceLayer*)arg0);
                arg0->stopSomething = 1;
            }
        } else {
            AudioSeq_SeqLayerProcessScriptStep1(arg0);
            ret = AudioSeq_SeqLayerProcessScriptStep2(arg0);
            if (ret != -1) {
                ret = AudioSeq_SeqLayerProcessScriptStep3(arg0, ret);
                if (ret != -1) {
                    ret = AudioSeq_SeqLayerProcessScriptStep4(arg0, ret);
                }
                if (ret != -1) {
                    AudioSeq_SeqLayerProcessScriptStep5(arg0, ret);
                }
                if (arg0->stopSomething == 1 && (arg0->note != 0 || arg0->continuousNotes)) {
                    AudioPlayback_SeqLayerNoteDecay((SequenceLayer*)arg0);
                }
            }
        }
    }
}

/**
 * Decays or releases the layer's note before the next script command.
 * Corresponds to OoT AudioSeq_SeqLayerProcessScriptStep1 (original name __Stop_Note).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
void AudioSeq_SeqLayerProcessScriptStep1(SequenceLayer* arg0) {
    if (!arg0->continuousNotes) {
        AudioPlayback_SeqLayerNoteDecay((SequenceLayer*)arg0);
    } else if (arg0->note != NULL && arg0 == arg0->note->playbackState.wantedParentLayer) {
        AudioPlayback_SeqLayerNoteDecay((SequenceLayer*)arg0);
    }
    switch (arg0->portamento.mode & ~0x80) {
        case 1:
        case 2:
            arg0->portamento.mode = 0;
            break;
    }
    arg0->notePropertiesNeedInit = 1;
}

s8* AudioPlayback_AllocNote(s8*);
void AudioEffects_InitVibrato(void*);

/* SequenceLayer and Sample as AudioSeq_SeqLayerProcessScriptStep5 reads them. The layouts are the same as
 * SequenceLayer/Sample, but the function only matches with the layer flags in a u32 container and the sample
 * word split as codec:4/medium:2/rest:26 (tested: SequenceLayer's u8 flag byte or Sample's finer split both
 * change the register allocation), so this view stays. */


/**
 * Gets a note for the layer, reusing the current one for the same sound.
 * Corresponds to OoT AudioSeq_SeqLayerProcessScriptStep5 (original name __SetChannel).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
s32 AudioSeq_SeqLayerProcessScriptStep5(SequenceLayerFlagsU32* layer, u32 same_sound) {
    Note* note;

    if (!layer->stopSomething && layer->sound != NULL && layer->sound->sample->codec == 2 &&
        layer->sound->sample->medium != 0) {
        layer->stopSomething = 1;
        return -1;
    }

    if (layer->continuousNotes == 1 && layer->bit1 == 1) {
        return 0;
    }

    if (layer->continuousNotes != 1 || layer->note == NULL || !layer->status || same_sound != 1 ||
        layer != (SequenceLayerFlagsU32*)layer->note->playbackState.parentLayer) {
        if (same_sound == 0) {
            AudioPlayback_SeqLayerNoteDecay((SequenceLayer*)layer);
        }
        layer->note = (Note*)AudioPlayback_AllocNote((s8*)layer);
    }
    if (layer->note != NULL && layer->note->playbackState.parentLayer == (SequenceLayer*)layer) {
        note = layer->note;
        AudioEffects_InitVibrato(note);
    }
    return 0;
}

extern f32 D_812397E8[];
u8 AudioSeq_GetInstrument(u8* arg0, u8 arg1, u8** arg2, u8* arg3);

/**
 * Runs layer script commands until a note command.
 * Corresponds to OoT AudioSeq_SeqLayerProcessScriptStep2 (original name __Command_Seq).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
s32 AudioSeq_SeqLayerProcessScriptStep2(SequenceLayer* layer) {
    SequenceChannel* channel = layer->channel;
    SeqScriptState* state = &layer->scriptState;
    SequencePlayer* seq_player = channel->seqPlayer;
    u8 cmd;
    u8 cmd_arg8;
    u16 cmd_arg16;
    u16 velocity;

    while (1) {
        cmd = AudioSeq_ScriptReadU8(&state->pc);
        if (cmd <= 0xC0) {
            return cmd;
        }
        if (cmd >= 0xF2) {
            cmd_arg16 = AudioSeq_GetScriptControlFlowArgument(&state->pc, cmd);
            if (AudioSeq_HandleScriptFlowControl(seq_player, state, cmd, cmd_arg16) == 0) {
                continue;
            }
            AudioSeq_SeqLayerDisable((SequenceLayer*)layer);
            return -1;
        }

        switch (cmd) {
            case 0xC1:
            case 0xCA:
                cmd_arg8 = *(state->pc++);
                if (cmd == 0xC1) {
                    layer->velocitySquare = (f32)(cmd_arg8 * cmd_arg8) / 16129.0f;
                } else {
                    layer->pan = cmd_arg8;
                }
                break;
            case 0xC9:
            case 0xC2:
                cmd_arg8 = *(state->pc++);
                if (cmd == 0xC9) {
                    layer->noteDuration = cmd_arg8;
                } else {
                    layer->transposition = cmd_arg8;
                }
                break;
            case 0xC4:
            case 0xC5:
                if (cmd == 0xC4) {
                    layer->continuousNotes = 1;
                } else {
                    layer->continuousNotes = 0;
                }
                layer->bit1 = 0;
                AudioPlayback_SeqLayerNoteDecay((SequenceLayer*)layer);
                break;
            case 0xC3:
                cmd_arg16 = AudioSeq_ScriptReadCompressedU16(&state->pc);
                layer->shortNoteDefaultDelay = cmd_arg16;
                break;
            case 0xC6:
                cmd = AudioSeq_ScriptReadU8(&state->pc);
                if (cmd >= 0x7F) {
                    if (cmd == 0x7F) {
                        layer->instOrWave = 0;
                    } else {
                        layer->instOrWave = cmd;
                        layer->instrument = NULL;
                    }
                    if (cmd == 0xFF) {
                        layer->adsr.decayIndex = 0;
                    }
                    break;
                }
                if ((layer->instOrWave = AudioSeq_GetInstrument((u8*)channel, cmd, &layer->instrument, &layer->adsr.decayIndex)) == 0) {
                    layer->instOrWave = 0xFF;
                }
                break;
            case 0xC7:
                layer->portamento.mode = AudioSeq_ScriptReadU8(&state->pc);
                cmd = AudioSeq_ScriptReadU8(&state->pc);
                cmd += channel->transposition;
                cmd += layer->transposition;
                cmd += seq_player->transposition;
                if (cmd >= 0x80) {
                    cmd = 0;
                }
                layer->portamentoTargetNote = cmd;
                if (layer->portamento.mode & 0x80) {
                    layer->portamentoTime = *(state->pc++);
                    break;
                }
                cmd_arg16 = AudioSeq_ScriptReadCompressedU16(&state->pc);
                layer->portamentoTime = cmd_arg16;
                break;
            case 0xC8:
                layer->portamento.mode = 0;
                break;
            case 0xCB:
                cmd_arg16 = AudioSeq_ScriptReadS16(&state->pc);
                layer->adsr.envelope = seq_player->seqData + cmd_arg16;
            case 0xCF:
                layer->adsr.decayIndex = AudioSeq_ScriptReadU8(&state->pc);
                break;
            case 0xCC:
                layer->ignoreDrumPan = 1;
                break;
            case 0xCD:
                layer->stereo.asByte = AudioSeq_ScriptReadU8(&state->pc);
                break;
            case 0xCE:
                layer->bend = D_812397E8[(u8)(AudioSeq_ScriptReadU8(&state->pc) + 0x80)];
                break;
            default:
                switch (cmd & 0xF0) {
                    case 0xD0:
                        velocity = seq_player->shortNoteVelocityTable[cmd & 0xF];
                        layer->velocitySquare = (f32)(velocity * velocity) / 16129.0f;
                        break;
                    case 0xE0:
                        layer->noteDuration = seq_player->shortNoteGateTimeTable[cmd & 0xF];
                        break;
                }
                break;
        }
    }
}

extern f32 D_81239BE8[];
extern s16 D_81288D8C;
extern SynthesisReverb D_812875B0[];
u8* AudioPlayback_GetInstrumentTunedSample(u8* arg0, s32 arg1);
u8* AudioPlayback_GetDrum(s32 arg0, s32 arg1);

/**
 * Sets up the note's instrument, pitch and velocity from a note command.
 * Corresponds to OoT AudioSeq_SeqLayerProcessScriptStep4 (original name __SetVoice).
 * No caller in play (2026-11-13): nothing that runs reaches it, not even through callbacks, data
 * tables or fragment headers.
 * @unverified Name and comment guessed from the code.
 */
s32 AudioSeq_SeqLayerProcessScriptStep4(SequenceLayer* layer, s32 cmd_semitone) {
    s32 same_sound = 1;
    s32 inst_or_wave;
    s32 vel;
    f32 temp_f14;
    f32 temp_f2;
    Portamento* portamento;
    f32 freq_scale;
    f32 freq_scale2;
    SoundFontSound* sound;
    u8* instrument;
    Drum* drum;
    f32 tuning;
    SequenceChannel* channel;
    SequencePlayer* seq_player;
    u8 semitone = cmd_semitone;

    inst_or_wave = layer->instOrWave;
    channel = layer->channel;
    seq_player = channel->seqPlayer;

    if (inst_or_wave == 0xFF) {
        if (!channel->hasInstrument) {
            return -1;
        }
        inst_or_wave = channel->instOrWave;
    }

    switch (inst_or_wave) {
        case 0:
            semitone += channel->transposition + layer->transposition;
            drum = (Drum*)AudioPlayback_GetDrum(channel->fontId, semitone);
            if (drum == NULL) {
                layer->stopSomething = 1;
                layer->duration = layer->delay;
                return -1;
            }
            sound = &drum->sound;
            layer->adsr.envelope = drum->envelope;
            layer->adsr.decayIndex = drum->adsrDecayIndex;
            if (!layer->ignoreDrumPan) {
                layer->pan = drum->pan;
            }
            layer->sound = sound;
            layer->freqScale = sound->tuning;
            break;

        default:
            semitone += seq_player->transposition + channel->transposition + layer->transposition;
            if (semitone >= 0x80) {
                layer->stopSomething = 1;
                return -1;
            }
            if (layer->instOrWave == 0xFF) {
                instrument = channel->instrument;
            } else {
                instrument = layer->instrument;
            }
            if (layer->portamento.mode != 0) {
                vel = (semitone > layer->portamentoTargetNote) ? semitone : layer->portamentoTargetNote;
                if (instrument != NULL) {
                    sound = (SoundFontSound*)AudioPlayback_GetInstrumentTunedSample(instrument, vel);
                    same_sound = (layer->sound == sound);
                    layer->sound = sound;
                    tuning = sound->tuning;
                } else {
                    layer->sound = NULL;
                    tuning = 1.0f;
                    if (inst_or_wave >= 0xC0) {
                        layer->sound = &D_812875B0[inst_or_wave - 0xC0].sound;
                    }
                }
                temp_f2 = D_81239BE8[(s32)semitone] * tuning; // FAKE: no-op cast; only changes the spill slot
                temp_f14 = D_81239BE8[layer->portamentoTargetNote] * tuning;
                portamento = &layer->portamento;
                switch (layer->portamento.mode & ~0x80) {
                    case 1:
                    case 3:
                    case 5:
                        freq_scale2 = temp_f2;
                        freq_scale = temp_f14;
                        break;
                    case 2:
                    case 4:
                        freq_scale = temp_f2;
                        freq_scale2 = temp_f14;
                        break;
                    default:
                        freq_scale = temp_f2;
                        freq_scale2 = temp_f2;
                        break;
                }
                portamento->extent = (freq_scale2 / freq_scale) - 1.0f;
                if (layer->portamento.mode & 0x80) {
                    portamento->speed = 32512.0f * (f32)(s32)seq_player->tempo /
                                        ((f32)layer->delay * (f32)D_81288D8C * (f32)(s32)layer->portamentoTime);
                } else {
                    portamento->speed = 127.0f / (f32)(s32)layer->portamentoTime;
                }
                portamento->cur = 0.0f;
                layer->freqScale = freq_scale;
                if ((layer->portamento.mode & ~0x80) == 5) {
                    layer->portamentoTargetNote = semitone;
                }
                break;
            }
            if (instrument != NULL) {
                sound = (SoundFontSound*)AudioPlayback_GetInstrumentTunedSample(instrument, semitone);
                same_sound = (sound == layer->sound);
                layer->sound = sound;
                layer->freqScale = D_81239BE8[(s32)semitone] * sound->tuning; // FAKE: no-op cast
            } else {
                layer->sound = NULL;
                layer->freqScale = D_81239BE8[(s32)semitone]; // FAKE: no-op cast
                if (inst_or_wave >= 0xC0) {
                    layer->sound = &D_812875B0[inst_or_wave - 0xC0].sound;
                }
            }
            break;
    }
    layer->duration = layer->delay;
    layer->freqScale *= layer->bend;
    return same_sound;
}

/* Audio driver, matching OoT src/audio/internal/seqplayer.c (continued) in function order. The
 * driver is Nintendo's "Nas" library (original names Nas_*); this revision sits between SM64
 * Shindou and OoT. pokestadium0's docs/annotation/audio-driver.md has the per-function mapping. */
/* Fixed-layout split; original translation-unit boundaries remain unconfirmed. */
#include "gb_tower_emulator_jp.h"

// Declared here instead of in a header: the callers of these disagree on their types.
void AudioPlayback_NotePoolClear(s8*);


extern u32 D_812346F4;

/**
 * Sets the layer's pitch, delay and velocity for a note command, handling portamento.
 * Corresponds to OoT AudioSeq_SeqLayerProcessScriptStep3 (original name __SetNote).
 * @unverified Name follows the counterpart; not checked in play.
 */
s32 AudioSeq_SeqLayerProcessScriptStep3(SequenceLayer* layer, s32 cmd) {
    SeqScriptState* state = &layer->scriptState;
    u16 delay;
    s32 velocity;
    SequenceChannel* channel = layer->channel;
    SequencePlayer* seq_player = channel->seqPlayer;
    s32 int_delta;
    f32 float_delta;

    if (cmd == 0xC0) {
        layer->delay = AudioSeq_ScriptReadCompressedU16(&state->pc);
        layer->stopSomething = 1;
        return -1;
    }

    layer->stopSomething = 0;

    if (channel->largeNotes == 1) {
        switch (cmd & 0xC0) {
            case 0x00:
                delay = AudioSeq_ScriptReadCompressedU16(&state->pc);
                velocity = *(state->pc++);
                layer->noteDuration = *(state->pc++);
                layer->lastDelay = delay;
                break;
            case 0x40:
                delay = AudioSeq_ScriptReadCompressedU16(&state->pc);
                velocity = *(state->pc++);
                layer->noteDuration = 0;
                layer->lastDelay = delay;
                break;
            case 0x80:
                delay = layer->lastDelay;
                velocity = *(state->pc++);
                layer->noteDuration = *(state->pc++);
                break;
        }
        if (velocity > 0x7F || velocity < 0) {
            velocity = 0x7F;
        }
        layer->velocitySquare = (f32)velocity * (f32)velocity / 16129.0f;
        cmd -= (cmd & 0xC0);
    } else {
        switch (cmd & 0xC0) {
            case 0x00:
                delay = AudioSeq_ScriptReadCompressedU16(&state->pc);
                layer->lastDelay = delay;
                break;
            case 0x40:
                delay = layer->shortNoteDefaultDelay;
                break;
            case 0x80:
                delay = layer->lastDelay;
                break;
        }
        cmd -= (cmd & 0xC0);
    }

    if (channel->velocityRandomVariance != 0) {
        float_delta = layer->velocitySquare * (f32)(D_812346F4 % channel->velocityRandomVariance) / 100.0f;
        if ((D_812346F4 & 0x8000) != 0) {
            float_delta = -float_delta;
        }
        layer->velocitySquare2 = layer->velocitySquare + float_delta;
        if (layer->velocitySquare2 < 0.0f) {
            layer->velocitySquare2 = 0.0f;
        } else if (layer->velocitySquare2 > 1.0f) {
            layer->velocitySquare2 = 1.0f;
        }
    } else {
        layer->velocitySquare2 = layer->velocitySquare;
    }

    layer->delay = delay;
    layer->gateDelay = (layer->noteDuration * delay) >> 8;
    if (channel->gateTimeRandomVariance != 0) {
        //!bug Takes the random gate time change modulo velocityRandomVariance instead of
        //! gateTimeRandomVariance, so a channel with only a gate time variance divides by zero.
        int_delta = (layer->gateDelay * (D_812346F4 % channel->velocityRandomVariance)) / 100;
        if ((D_812346F4 & 0x4000) != 0) {
            int_delta = -int_delta;
        }
        layer->gateDelay += int_delta;
        if (layer->gateDelay < 0) {
            layer->gateDelay = 0;
        } else if (layer->gateDelay > layer->delay) {
            layer->gateDelay = layer->delay;
        }
    }

    if ((seq_player->muted && (channel->muteBehavior & 0x50)) || channel->muted) {
        layer->stopSomething = 1;
        return -1;
    }
    if (seq_player->skipTicks != 0) {
        layer->stopSomething = 1;
        return -1;
    }
    return cmd;
}

/**
 * Sets channel arg0's priorities from the two nibbles of arg1: the low nibble goes to notePriority
 * (+0x05) and the high nibble to someOtherPriority (+0x06). A zero nibble leaves that priority
 * unchanged. (arg1 itself is reused for the
 * high nibble, which is what the retail register use needs.)
 * Corresponds to OoT AudioSeq_SetChannelPriorities (original name Nas_PriorityChanger).
 */
void AudioSeq_SetChannelPriorities(u8* arg0, u8 arg1) {
    s32 low;

    low = arg1 & 0xF;
    if (low != 0) {
        arg0[5] = low;
    }
    arg1 = arg1 >> 4;
    if (arg1 != 0) {
        arg0[6] = arg1;
    }
}

/**
 * Returns the channel's instrument `arg1` and copies its ADSR into `arg3`; returns 0 when it does
 * not exist.
 * Corresponds to OoT AudioSeq_GetInstrument (original name Nas_ProgramChanger).
 * @unverified Name follows the counterpart; not checked in play.
 */
u8 AudioSeq_GetInstrument(u8* arg0, u8 arg1, u8** arg2, u8* arg3) {
    u8* p = AudioPlayback_GetInstrumentInner(arg0[7], arg1);

    if (p == NULL) {
        *arg2 = NULL;
        return 0;
    }
    M2C_FIELD(arg3, s32*, 4) = M2C_FIELD(p, s32*, 4);
    arg3[0] = p[3];
    *arg2 = p;
    arg1++;
    return arg1;
}

/**
 * Sets the channel's instrument: 0x80 and up are raw instrument ids, 0x7F selects drums.
 * Corresponds to OoT AudioSeq_SetInstrument (original name Nas_SubVoiceSet).
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioSeq_SetInstrument(u8* arg0, u8 arg1) {
    s16 ret;

    if (arg1 >= 0x80) {
        M2C_FIELD(arg0, s16*, 0x22) = arg1;
        M2C_FIELD(arg0, s32*, 0x48) = 0;
    } else if (arg1 == 0x7F) {
        M2C_FIELD(arg0, s16*, 0x22) = 0;
        M2C_FIELD(arg0, s32*, 0x48) = 1;
    } else {
        ret = M2C_FIELD(arg0, s16*, 0x22) = AudioSeq_GetInstrument(arg0, arg1, (u8**)(arg0 + 0x48), arg0 + 0x7C);
        if (ret == 0) {
            ((SequenceChannel*)arg0)->hasInstrument = 0;
            return;
        }
    }
    ((SequenceChannel*)arg0)->hasInstrument = 1;
}

/**
 * Sets the channel's volume from 0-127.
 * Corresponds to OoT AudioSeq_SequenceChannelSetVolume (original name Nas_SubVolumeSet).
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioSeq_SequenceChannelSetVolume(u8* arg0, u8 arg1) {
    *(f32*)(arg0 + 0x2C) = (f32)(s32)arg1 / 127.0f;
}

extern u8* D_81288D48;
extern u8 D_8123BB00[];
extern u32 D_812346F4;
extern f32 D_812393E8[];
extern f32 D_812397E8[];
s32 AudioHeap_SearchCaches(s32, s32, s32);
void AudioHeap_LoadFilter(s16*, s32, s32);
s32 AudioLoad_SlowLoadSample(s32, u8, s8*);
void AudioPlayback_NotePoolFill(void*, s32);
void AudioSeq_SequenceChannelDisable(SequenceChannel*);
s32 AudioSeq_SeqChannelSetLayer(SequenceChannel* arg0, s32 arg1);
void AudioSeq_SeqLayerProcessScript(SequenceLayer* arg0);
s32 AudioSeq_HandleScriptFlowControl(SequencePlayer* seq_player, SeqScriptState811126B8* state, s32 cmd, s32 cmd_arg);

/**
 * Runs one tick of a channel's script, then its layers' scripts.
 * Corresponds to OoT AudioSeq_SequenceChannelProcessScript (original name Nas_SubSeq). OoT adds
 * random numbers (AudioThread_NextRandom).
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioSeq_SequenceChannelProcessScript(SequenceChannel* channel) {
    s32 i;
    u8* data;
    s8 signed_param;
    u8 command;
    u8 high_bits;
    u8 low_bits;
    u8* test;
    SequencePlayer* seq_player;

    if (channel->stopScript) {
        goto exit_loop;
    }
    seq_player = channel->seqPlayer;
    if (seq_player->muted && (channel->muteBehavior & 0x80)) {
        return;
    }

    if (channel->delay >= 2) {
        channel->delay--;
        goto exit_loop;
    }

    while (1) {
        SeqScriptState* script_state = &channel->scriptState;
        u16 offset;
        u32 parameters[3];
        s32 result;

        command = AudioSeq_ScriptReadU8(&script_state->pc);
        if (command >= 0xB0) {
            high_bits = (D_8123BB00 + 0xB0)[command - 0xB0]; // FAKE: the +0xB0/-0xB0 pair only changes register allocation
            low_bits = high_bits & 3;

            for (i = 0; i < low_bits; i++, high_bits <<= 1) {
                if (!(high_bits & 0x80)) {
                    parameters[i] = AudioSeq_ScriptReadU8(&script_state->pc);
                } else {
                    parameters[i] = AudioSeq_ScriptReadS16(&script_state->pc);
                }
            }
            if (command >= 0xF2) {
                result = AudioSeq_HandleScriptFlowControl(seq_player, script_state, command, parameters[0]);
                if (result != 0) {
                    if (result == -1) {
                        AudioSeq_SequenceChannelDisable(channel);
                    } else {
                        channel->delay = result;
                    }
                    break;
                }
            } else {
                switch (command) {
                    case 0xEA:
                        channel->stopScript = 1;
                        goto exit_loop;
                    case 0xF1:
                        AudioPlayback_NotePoolClear((s8*)&channel->notePool);
                        command = (u8)parameters[0];
                        AudioPlayback_NotePoolFill(&channel->notePool, command);
                        break;
                    case 0xF0:
                        AudioPlayback_NotePoolClear((s8*)&channel->notePool);
                        break;
                    case 0xC2:
                        offset = (u16)parameters[0];
                        channel->dynTable = (void*)&seq_player->seqData[offset];
                        break;
                    case 0xC5:
                        if (script_state->value != -1) {
                            data = (*channel->dynTable)[script_state->value];
                            offset = (u16)((data[0] << 8) + data[1]);
                            channel->dynTable = (void*)&seq_player->seqData[offset];
                        }
                        break;
                    case 0xEB:
                        command = (u8)parameters[0];
                        offset = ((u16*)D_81288D48)[seq_player->seqId];
                        low_bits = D_81288D48[offset];
                        command = D_81288D48[offset + low_bits - command];
                        if (AudioHeap_SearchCaches(1, 2, command)) {
                            channel->fontId = command;
                        }
                        parameters[0] = parameters[1];
                    case 0xC1:
                        command = (u8)parameters[0];
                        AudioSeq_SetInstrument((u8*)channel, command);
                        break;
                    case 0xC3:
                        channel->largeNotes = 0;
                        break;
                    case 0xC4:
                        channel->largeNotes = 1;
                        break;
                    case 0xDF:
                        command = (u8)parameters[0];
                        AudioSeq_SequenceChannelSetVolume((u8*)channel, command);
                        channel->changes.s.volume = 1;
                        break;
                    case 0xE0:
                        command = (u8)parameters[0];
                        channel->volumeScale = (f32)(s32)command / 128.0f;
                        channel->changes.s.volume = 1;
                        break;
                    case 0xDE:
                        offset = (u16)parameters[0];
                        channel->freqScale = (f32)(s32)offset / 32768.0f;
                        channel->changes.s.freqScale = 1;
                        break;
                    case 0xD3:
                        command = (u8)parameters[0];
                        command += 0x80;
                        channel->freqScale = D_812393E8[command];
                        channel->changes.s.freqScale = 1;
                        break;
                    case 0xEE:
                        command = (u8)parameters[0];
                        command += 0x80;
                        channel->freqScale = D_812397E8[command];
                        channel->changes.s.freqScale = 1;
                        break;
                    case 0xDD:
                        command = (u8)parameters[0];
                        channel->newPan = command;
                        channel->changes.s.pan = 1;
                        break;
                    case 0xDC:
                        command = (u8)parameters[0];
                        channel->panChannelWeight = command;
                        channel->changes.s.pan = 1;
                        break;
                    case 0xDB:
                        signed_param = (s8)parameters[0];
                        channel->transposition = signed_param;
                        break;
                    case 0xDA:
                        offset = (u16)parameters[0];
                        channel->adsr.envelope = &seq_player->seqData[offset];
                        break;
                    case 0xD9:
                        command = (u8)parameters[0];
                        channel->adsr.decayIndex = command;
                        break;
                    case 0xD8:
                        command = (u8)parameters[0];
                        channel->vibratoExtentTarget = command * 8;
                        channel->vibratoExtentStart = 0;
                        channel->vibratoExtentChangeDelay = 0;
                        break;
                    case 0xD7:
                        command = (u8)parameters[0];
                        channel->vibratoRateChangeDelay = 0;
                        channel->vibratoRateTarget = command * 32;
                        channel->vibratoRateStart = command * 32;
                        break;
                    case 0xE2:
                        command = (u8)parameters[0];
                        channel->vibratoExtentStart = command * 8;
                        command = (u8)parameters[1];
                        channel->vibratoExtentTarget = command * 8;
                        command = (u8)parameters[2];
                        channel->vibratoExtentChangeDelay = command * 16;
                        break;
                    case 0xE1:
                        command = (u8)parameters[0];
                        channel->vibratoRateStart = command * 32;
                        command = (u8)parameters[1];
                        channel->vibratoRateTarget = command * 32;
                        command = (u8)parameters[2];
                        channel->vibratoRateChangeDelay = command * 16;
                        break;
                    case 0xE3:
                        command = (u8)parameters[0];
                        channel->vibratoDelay = command * 16;
                        break;
                    case 0xD4:
                        command = (u8)parameters[0];
                        channel->reverb = command;
                        break;
                    case 0xC6:
                        command = (u8)parameters[0];
                        offset = ((u16*)D_81288D48)[seq_player->seqId];
                        low_bits = D_81288D48[offset];
                        command = D_81288D48[offset + low_bits - command];
                        if (AudioHeap_SearchCaches(1, 2, command)) {
                            channel->fontId = command;
                        }
                        break;
                    case 0xC7:
                        command = (u8)parameters[0];
                        offset = (u16)parameters[1];
                        test = &seq_player->seqData[offset];
                        test[0] = (u8)script_state->value + command;
                        break;
                    case 0xC8:
                    case 0xC9:
                    case 0xCC:
                        signed_param = (s8)parameters[0];
                        if (command == 0xC8) {
                            script_state->value -= signed_param;
                        } else if (command == 0xCC) {
                            script_state->value = signed_param;
                        } else {
                            script_state->value &= signed_param;
                        }
                        break;
                    case 0xCD:
                        command = (u8)parameters[0];
                        AudioSeq_SequenceChannelDisable(seq_player->channels[command]);
                        break;
                    case 0xCA:
                        command = (u8)parameters[0];
                        channel->muteBehavior = command;
                        channel->changes.s.volume = 1;
                        break;
                    case 0xCB:
                        offset = (u16)parameters[0];
                        script_state->value = *(seq_player->seqData + (u32)(offset + script_state->value));
                        break;
                    case 0xCE:
                        offset = (u16)parameters[0];
                        channel->unk_CC = offset;
                        break;
                    case 0xCF:
                        offset = (u16)parameters[0];
                        test = &seq_player->seqData[offset];
                        test[0] = (channel->unk_CC >> 8) & 0xFF;
                        test[1] = channel->unk_CC & 0xFF;
                        break;
                    case 0xD0:
                        command = (u8)parameters[0];
                        channel->stereoHeadsetEffects = command;
                        break;
                    case 0xD1:
                        command = (u8)parameters[0];
                        channel->noteAllocPolicy = command;
                        break;
                    case 0xD2:
                        command = (u8)parameters[0];
                        channel->adsr.sustain = command;
                        break;
                    case 0xE5:
                        command = (u8)parameters[0];
                        channel->reverbIndex = command;
                        break;
                    case 0xE4:
                        if (script_state->value != -1) {
                            data = (*channel->dynTable)[script_state->value];
                            high_bits = script_state->depth; // FAKE: reuses high_bits as a depth copy for register allocation
                            script_state->stack[high_bits] = script_state->pc;
                            script_state->depth++;
                            offset = (u16)((data[0] << 8) + data[1]);
                            script_state->pc = seq_player->seqData + offset;
                        }
                        break;
                    case 0xE6:
                        command = (u8)parameters[0];
                        channel->bookOffset = command;
                        break;
                    case 0xE7:
                        offset = (u16)parameters[0];
                        data = &seq_player->seqData[offset];
                        channel->muteBehavior = data[0];
                        data += 3;
                        channel->noteAllocPolicy = data[-2];
                        AudioSeq_SetChannelPriorities((u8*)channel, data[-1]);
                        channel->transposition = (s8)data[0];
                        data += 4;
                        channel->newPan = data[-3];
                        channel->panChannelWeight = data[-2];
                        channel->reverb = data[-1];
                        channel->reverbIndex = data[0];
                        channel->changes.s.pan = 1;
                        break;
                    case 0xE8:
                        channel->muteBehavior = parameters[0];
                        channel->noteAllocPolicy = parameters[1];
                        command = (u8)parameters[2];
                        AudioSeq_SetChannelPriorities((u8*)channel, command);
                        channel->transposition = (s8)AudioSeq_ScriptReadU8(&script_state->pc);
                        channel->newPan = AudioSeq_ScriptReadU8(&script_state->pc);
                        channel->panChannelWeight = AudioSeq_ScriptReadU8(&script_state->pc);
                        channel->reverb = AudioSeq_ScriptReadU8(&script_state->pc);
                        channel->reverbIndex = AudioSeq_ScriptReadU8(&script_state->pc);
                        channel->changes.s.pan = 1;
                        break;
                    case 0xEC:
                        channel->vibratoExtentTarget = 0;
                        channel->vibratoExtentStart = 0;
                        channel->vibratoExtentChangeDelay = 0;
                        channel->vibratoRateTarget = 0;
                        channel->vibratoRateStart = 0;
                        channel->vibratoRateChangeDelay = 0;
                        channel->filter = NULL;
                        channel->gain = 0;
                        channel->adsr.sustain = 0;
                        channel->velocityRandomVariance = 0;
                        channel->gateTimeRandomVariance = 0;
                        channel->combFilterSize = 0;
                        channel->combFilterGain = 0;
                        channel->bookOffset = 0;
                        channel->freqScale = 1.0f;
                        break;
                    case 0xE9:
                        AudioSeq_SetChannelPriorities((u8*)channel, (u8)parameters[0]);
                        break;
                    case 0xED:
                        command = (u8)parameters[0];
                        channel->gain = command;
                        break;
                    case 0xB0:
                        offset = (u16)parameters[0];
                        data = seq_player->seqData + offset;
                        channel->filter = (s16*)data;
                        break;
                    case 0xB1:
                        channel->filter = NULL;
                        break;
                    case 0xB3:
                        command = parameters[0];
                        if (channel->filter != NULL) {
                            low_bits = (command >> 4) & 0xF;
                            command &= 0xF;
                            AudioHeap_LoadFilter(channel->filter, low_bits, command);
                        }
                        break;
                    case 0xB2:
                        offset = (u16)parameters[0];
                        channel->unk_CC = *(u16*)(seq_player->seqData + (u32)(offset + script_state->value * 2));
                        break;
                    case 0xB4:
                        channel->dynTable = (void*)&seq_player->seqData[channel->unk_CC];
                        break;
                    case 0xB5:
                        channel->unk_CC = ((u16*)(channel->dynTable))[script_state->value];
                        break;
                    case 0xB6:
                        script_state->value = (*channel->dynTable)[0][script_state->value];
                        break;
                    case 0xB7:
                        channel->unk_CC = (parameters[0] == 0) ? D_812346F4 & 0xFFFF : D_812346F4 % parameters[0];
                        break;
                    case 0xB8:
                        script_state->value = (parameters[0] == 0) ? D_812346F4 & 0xFF : D_812346F4 % parameters[0];
                        break;
                    case 0xB9:
                        channel->velocityRandomVariance = parameters[0];
                        break;
                    case 0xBA:
                        channel->gateTimeRandomVariance = parameters[0];
                        break;
                    case 0xBB:
                        channel->combFilterSize = parameters[0];
                        channel->combFilterGain = parameters[1];
                        break;
                }
            }
        } else if (command >= 0x80) {
            low_bits = command & 7;
            switch (command & 0xF8) {
                case 0x80:
                    if (channel->layers[low_bits] != NULL) {
                        script_state->value = channel->layers[low_bits]->finished;
                    } else {
                        script_state->value = -1;
                    }
                    break;
                case 0x88:
                    offset = AudioSeq_ScriptReadS16(&script_state->pc);
                    if (!AudioSeq_SeqChannelSetLayer(channel, low_bits)) {
                        channel->layers[low_bits]->scriptState.pc = &seq_player->seqData[offset];
                    }
                    break;
                case 0x90:
                    AudioSeq_SeqLayerFree((s8**)channel, low_bits);
                    break;
                case 0x98:
                    if (script_state->value != -1 && AudioSeq_SeqChannelSetLayer(channel, low_bits) != -1) {
                        data = (*channel->dynTable)[script_state->value];
                        offset = (data[0] << 8) + data[1];
                        channel->layers[low_bits]->scriptState.pc = &seq_player->seqData[offset];
                    }
                    break;
            }
        } else {
            low_bits = command & 0xF;
            switch (command & 0xF0) {
                case 0x00:
                    channel->delay = low_bits;
                    goto exit_loop;
                case 0x10:
                    channel->soundScriptIO[low_bits] = -1;
                    if (AudioLoad_SlowLoadSample(channel->fontId, script_state->value, &channel->soundScriptIO[low_bits]) == -1) {} // karodo
                    break;
                case 0x70:
                    channel->soundScriptIO[low_bits] = script_state->value;
                    break;
                case 0x60:
                    script_state->value = channel->soundScriptIO[low_bits];
                    if (low_bits < 2) {
                        channel->soundScriptIO[low_bits] = -1;
                    }
                    break;
                case 0x50:
                    script_state->value -= channel->soundScriptIO[low_bits];
                    break;
                case 0x20:
                    offset = AudioSeq_ScriptReadS16(&script_state->pc);
                    AudioSeq_SequenceChannelEnable(seq_player, low_bits, &seq_player->seqData[offset]);
                    break;
                case 0x30:
                    command = AudioSeq_ScriptReadU8(&script_state->pc);
                    seq_player->channels[low_bits]->soundScriptIO[command] = script_state->value;
                    break;
                case 0x40:
                    command = AudioSeq_ScriptReadU8(&script_state->pc);
                    script_state->value = seq_player->channels[low_bits]->soundScriptIO[command];
                    break;
            }
        }
    }
exit_loop:
    for (i = 0; i < 4; i++) {
        if (channel->layers[i] != NULL) {
            AudioSeq_SeqLayerProcessScript(channel->layers[i]);
        }
    }
}

/* Audio driver, matching the end of OoT src/audio/internal/seqplayer.c in function order, then
 * (from Audio_ScheduleProcessCmds) Zero's own game-side wrappers that queue audio thread commands. The driver
 * is Nintendo's "Nas" library (original names Nas_*); this revision sits between SM64 Shindou and
 * OoT. pokestadium0's docs/annotation/audio-driver.md has the per-function mapping. */
/* Fixed-layout split; original translation-unit boundaries remain unconfirmed. */
#include "gb_tower_emulator_jp.h"

// Declared here instead of in a header: the callers of these disagree on their types.
M2C_UNK AudioEffects_SequencePlayerProcessSound(s8 *);
extern SequencePlayer D_81287B08[];
void AudioPlayback_NotePoolClear(s8*);
void AudioThread_QueueCmdS8(s32 cmd, s8 arg1);
void AudioThread_ScheduleProcessCmds(void);

s32 AudioLoad_IsFontLoadComplete(s32);
s32 AudioLoad_IsSeqLoadComplete(s32);
void AudioLoad_SetFontLoadStatus(s32, s32);
void AudioLoad_SetSeqLoadStatus(s32, s32);
u16 AudioSeq_GetScriptControlFlowArgument(u8** stream, u8 kind);
s32 AudioSeq_HandleScriptFlowControl(SequencePlayer* seq_player, SeqScriptState811126B8* state, s32 cmd, s32 cmd_arg);
void AudioSeq_SequencePlayerSetupChannels(SequencePlayer* seq_player, u16 mask);
void AudioSeq_SequencePlayerDisable(u8* arg0);
void AudioSeq_SequenceChannelDisable(SequenceChannel*);

extern s16 D_81288D8C;
void AudioPlayback_NotePoolFill(void*, s32);
void AudioSeq_SequenceChannelProcessScript(SequenceChannel* channel);

/**
 * Runs one tick of a sequence player's script, then its channels.
 * Corresponds to OoT AudioSeq_SequencePlayerProcessSequence (original name Nas_GroupSeq). OoT adds
 * script loads and slow sequence loads.
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioSeq_SequencePlayerProcessSequence(SequencePlayer* seq_player) {
    u8 cmd;
    u8 lo_bits;
    u16 temp;
    s32 value;
    s32 i;
    u8* data;
    SeqScriptState* state;
    UNUSED s32 pad[4]; // FAKE: stack pad of unknown origin; only reserves frame space

    if (!seq_player->enabled) {
        return;
    }
    if (!AudioLoad_IsSeqLoadComplete(seq_player->seqId) || !AudioLoad_IsFontLoadComplete(seq_player->defaultFont)) {
        AudioSeq_SequencePlayerDisable((u8*)seq_player);
        return;
    }
    AudioLoad_SetSeqLoadStatus(seq_player->seqId, 2);
    AudioLoad_SetFontLoadStatus(seq_player->defaultFont, 2);

    if (seq_player->muted && (seq_player->muteBehavior & 0x80)) {
        return;
    }

    seq_player->scriptCounter++;
    seq_player->tempoAcc += seq_player->tempo;
    seq_player->tempoAcc += seq_player->tempoChange;
    if (seq_player->tempoAcc < D_81288D8C) {
        return;
    }
    seq_player->tempoAcc -= (u16)D_81288D8C;

    if (seq_player->stopScript == 1) {
        return;
    }

    state = &seq_player->scriptState;
    if (seq_player->delay > 1) {
        seq_player->delay--;
    } else {
        seq_player->recalculateVolume = 1;
        while (1) {
            cmd = AudioSeq_ScriptReadU8(&state->pc);
            if (cmd >= 0xF2) {
                value = AudioSeq_HandleScriptFlowControl(seq_player, state, cmd, AudioSeq_GetScriptControlFlowArgument(&state->pc, cmd));
                if (value != 0) {
                    if (value == -1) {
                        AudioSeq_SequencePlayerDisable((u8*)seq_player);
                    } else {
                        seq_player->delay = value;
                    }
                    break;
                }
            } else if (cmd >= 0xC0) {
                switch (cmd) {
                    case 0xF1:
                        AudioPlayback_NotePoolClear((s8*)&seq_player->notePool);
                        AudioPlayback_NotePoolFill(&seq_player->notePool, AudioSeq_ScriptReadU8(&state->pc));
                        break;
                    case 0xF0:
                        AudioPlayback_NotePoolClear((s8*)&seq_player->notePool);
                        break;
                    case 0xDF:
                        seq_player->transposition = 0;
                    case 0xDE:
                        seq_player->transposition += (s8)AudioSeq_ScriptReadU8(&state->pc);
                        break;
                    case 0xDD:
                        seq_player->tempo = AudioSeq_ScriptReadU8(&state->pc) * 48;
                        if (seq_player->tempo > D_81288D8C) {
                            seq_player->tempo = (u16)D_81288D8C;
                        }
                        if ((s16)seq_player->tempo <= 0) {
                            seq_player->tempo = 1;
                        }
                        break;
                    case 0xDC:
                        seq_player->tempoChange = (s8)AudioSeq_ScriptReadU8(&state->pc) * 48;
                        break;
                    case 0xDA:
                        cmd = AudioSeq_ScriptReadU8(&state->pc);
                        temp = AudioSeq_ScriptReadS16(&state->pc);
                        switch (cmd) {
                            case 0:
                            case 1:
                                if (seq_player->state != 2) {
                                    seq_player->storedFadeTimer = temp;
                                    seq_player->state = cmd;
                                }
                                break;
                            case 2:
                                seq_player->fadeTimer = temp;
                                seq_player->state = cmd;
                                seq_player->fadeVelocity = (0.0f - seq_player->fadeVolume) / (s32)seq_player->fadeTimer;
                                break;
                        }
                        break;
                    case 0xDB:
                        value = AudioSeq_ScriptReadU8(&state->pc);
                        switch (seq_player->state) {
                            case 1:
                                seq_player->state = 0;
                                seq_player->fadeVolume = 0.0f;
                            case 0:
                                seq_player->fadeTimer = seq_player->storedFadeTimer;
                                if (seq_player->storedFadeTimer != 0) {
                                    seq_player->fadeVelocity = ((value / 127.0f) - seq_player->fadeVolume) / (s32)seq_player->fadeTimer;
                                } else {
                                    seq_player->fadeVolume = value / 127.0f;
                                }
                                break;
                            case 2:
                                break;
                        }
                        break;
                    case 0xD9:
                        seq_player->fadeVolumeScale = (s8)AudioSeq_ScriptReadU8(&state->pc) / 127.0f;
                        break;
                    case 0xD7:
                        temp = AudioSeq_ScriptReadS16(&state->pc);
                        AudioSeq_SequencePlayerSetupChannels(seq_player, temp);
                        break;
                    case 0xD6:
                        AudioSeq_ScriptReadS16(&state->pc);
                        break;
                    case 0xD5:
                        seq_player->muteVolumeScale = (s8)AudioSeq_ScriptReadU8(&state->pc) / 127.0f;
                        break;
                    case 0xD4:
                        seq_player->muted = 1;
                        break;
                    case 0xD3:
                        seq_player->muteBehavior = AudioSeq_ScriptReadU8(&state->pc);
                        break;
                    case 0xD1:
                    case 0xD2:
                        temp = AudioSeq_ScriptReadS16(&state->pc);
                        data = seq_player->seqData + temp;
                        if (cmd == 0xD2) {
                            seq_player->shortNoteVelocityTable = data;
                        } else {
                            seq_player->shortNoteGateTimeTable = data;
                        }
                        break;
                    case 0xD0:
                        seq_player->noteAllocPolicy = AudioSeq_ScriptReadU8(&state->pc);
                        break;
                    case 0xCC:
                        state->value = AudioSeq_ScriptReadU8(&state->pc);
                        break;
                    case 0xC9:
                        state->value &= AudioSeq_ScriptReadU8(&state->pc);
                        break;
                    case 0xC8:
                        state->value -= AudioSeq_ScriptReadU8(&state->pc);
                        break;
                    case 0xC7:
                        cmd = AudioSeq_ScriptReadU8(&state->pc);
                        temp = AudioSeq_ScriptReadS16(&state->pc);
                        data = seq_player->seqData + temp;
                        *data = (u8)state->value + cmd;
                        break;
                    case 0xC6:
                        seq_player->stopScript = 1;
                        return;
                    case 0xC5:
                        seq_player->scriptCounter = (u16)AudioSeq_ScriptReadS16(&state->pc);
                        break;
                }
            } else {
                lo_bits = cmd & 0xF;
                switch (cmd & 0xF0) {
                    case 0x00:
                        state->value = seq_player->channels[lo_bits]->finished;
                        break;
                    case 0x10:
                        break;
                    case 0x20:
                        break;
                    case 0x50:
                        state->value -= seq_player->soundScriptIO[lo_bits];
                        break;
                    case 0x70:
                        seq_player->soundScriptIO[lo_bits] = state->value;
                        break;
                    case 0x80:
                        state->value = seq_player->soundScriptIO[lo_bits];
                        if (lo_bits < 2) {
                            seq_player->soundScriptIO[lo_bits] = -1;
                        }
                        break;
                    case 0x40:
                        AudioSeq_SequenceChannelDisable((SequenceChannel*)seq_player->channels[lo_bits]);
                        break;
                    case 0x90:
                        temp = AudioSeq_ScriptReadS16(&state->pc);
                        AudioSeq_SequenceChannelEnable(seq_player, lo_bits, seq_player->seqData + temp);
                        break;
                    case 0x60:
                    case 0xA0:
                        break;
                }
            }
        }
    }

    for (i = 0; i < 16; i++) {
        if (seq_player->channels[i]->enabled) {
            AudioSeq_SequenceChannelProcessScript(seq_player->channels[i]);
        }
    }
}

/**
 * Runs every enabled sequence player for one tick, then updates the notes.
 * Corresponds to OoT AudioSeq_ProcessSequences (original name Nas_MySeqMain).
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioSeq_ProcessSequences(s32 arg0) {
    u32 i;

    for (i = 0; i < D_81288D6C; i++) {
        if (D_81287B08[i].enabled == 1) {
            AudioSeq_SequencePlayerProcessSequence((s8*)&D_81287B08[i]);
            AudioEffects_SequencePlayerProcessSound((s8*)&D_81287B08[i]);
        }
    }
    AudioPlayback_ProcessNotes();
}

/**
 * Runs a sequence player's skipTicks ticks at once.
 * Corresponds to OoT AudioSeq_SkipForwardSequence (original name Nas_SeqSkip).
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioSeq_SkipForwardSequence(s8* arg0) {
    if (M2C_FIELD(arg0, s32*, 0xD8) > 0) {
        do {
            AudioSeq_SequencePlayerProcessSequence(arg0);
            AudioEffects_SequencePlayerProcessSound(arg0);
            M2C_FIELD(arg0, s32*, 0xD8)--;
        } while (M2C_FIELD(arg0, s32*, 0xD8) > 0);
    }
}

extern u8 D_81239DE8[];
extern u8 D_81239DF8[];
void AudioSeq_InitSequenceChannel(void* obj);
/**
 * Stops sequence player `arg0` and restores its tempo, volumes and channels to their defaults.
 * Corresponds to OoT AudioSeq_ResetSequencePlayer (original name Nas_InitMySeq).
 * Seen in play in pokestadium0 (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in
 * the Transfer Pak; called from AudioSeq_InitSequencePlayers.
 */
void AudioSeq_ResetSequencePlayer(s32 arg0) {
    SequencePlayer* chan = (SequencePlayer*)&D_81287B08[arg0];
    s32 i;

    AudioSeq_SequencePlayerDisable((u8*)chan);
    chan->stopScript = 0;
    chan->delay = 0;
    chan->state = 1;
    chan->fadeTimer = 0;
    chan->storedFadeTimer = 0;
    chan->tempoAcc = 0;
    chan->tempo = 5760;
    chan->tempoChange = 0;
    chan->transposition = 0;
    chan->noteAllocPolicy = 0;
    chan->shortNoteVelocityTable = D_81239DE8;
    chan->shortNoteGateTimeTable = D_81239DF8;
    chan->scriptCounter = 0;
    chan->fadeVolume = 1.0f;
    chan->fadeVolumeScale = 1.0f;
    chan->fadeVelocity = 0.0f;
    chan->volume = 0.0f;
    chan->muteVolumeScale = 0.5f;
    for (i = 0; i < 16; i++) {
        AudioSeq_InitSequenceChannel(chan->channels[i]);
    }
}

extern u8 D_81287760[];
extern SequenceChannel D_81287CF0;
void* GbApu_AllocZeroed(void*, s32);
/**
 * Allocates sequence player `arg0`'s 16 channels, falling back to the shared dummy channel.
 * Corresponds to OoT AudioSeq_InitSequencePlayerChannels (original name Nas_AssignSubTrack).
 * Seen in play in pokestadium0 (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in
 * the Transfer Pak; called from AudioHeap_Init.
 */
void AudioSeq_InitSequencePlayerChannels(s32 arg0) {
    SequencePlayer* rec = (SequencePlayer*)&D_81287B08[arg0];
    s32 i;
    s32 j;
    SequenceChannel* item;

    for (i = 0; i < 16; i++) {
        rec->channels[i] = GbApu_AllocZeroed(D_81287760, 0xD4);
        if (rec->channels[i] == NULL) {
            rec->channels[i] = &D_81287CF0;
        } else {
            item = rec->channels[i];
            item->enabled = 0;
            item->seqPlayer = rec;
            for (j = 0; j < 1; j++) {
                item->layers[j] = NULL;
            }
        }
    }
}

void AudioPlayback_InitNoteLists(void*);
/**
 * Sets up the layer free list, the sequence layers and the one sequence player.
 * Corresponds to OoT AudioSeq_InitSequencePlayers (original name Nas_InitPlayer). Zero has OoT's
 * AudioSeq_InitSequencePlayer (__InitGroup) inlined for its single player.
 * Seen in play in pokestadium0 (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in
 * the Transfer Pak; called from AudioHeap_Init.
 */
void AudioSeq_InitSequencePlayers(void) {
    s32 i;
    s32 j;
    SequencePlayer* rec;

    AudioSeq_InitLayerFreelist();
    for (i = 0; i < 1; i++) {
        ((SequenceLayer*)D_81287C68)[i].channel = 0;
        ((SequenceLayer*)D_81287C68)[i].enabled = 0;
    }
    for (i = 0; i < 1; i++) {
        rec = (SequencePlayer*)&D_81287B08[i];
        for (j = 0; j < 16; j++) {
            rec->channels[j] = &D_81287CF0;
        }
        rec->soundScriptIO[0] = -1;
        rec->muteBehavior = 0x60;
        rec->enabled = 0;
        rec->muted = 0;
        rec->fontDmaInProgress = 0;
        rec->seqDmaInProgress = 0;
        AudioPlayback_InitNoteLists(&rec->notePool);
        AudioSeq_ResetSequencePlayer(i);
    }
}
