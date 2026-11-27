/* JP-only TU, ported from pokestadium0 (Stadium Zero, fragment000_80C00). */
/* Audio driver, matching OoT src/audio/internal/thread.c in function order. The driver is
 * Nintendo's "Nas" library (original names Nas_*); this revision sits between SM64 Shindou and
 * OoT. pokestadium0's docs/annotation/audio-driver.md has the per-function mapping. */
/* C ported from the Stadium 1 decomp: src/audio_sound_state.c, src/fragments/battle_engine/battle_engine_2FA4D0.c,
 * src/fragments/battle_engine/battle_engine_317E70.c. */

#include "gb_tower_emulator_jp.h"

#include "libnumus/player.h"


#include "m2c_macros.h"

void AudioThread_Unused1(void);
void AudioThread_Unused2(void);
void AudioThread_Unused3(void);
void AudioThread_Unused4(void);
s32 AudioThread_GetFontsForSequence(s32 arg0);
void AudioThread_ResetExternalPool(void);
void AudioThread_Unused5(void);
void AudioThread_InitMesgQueues(void);

void AudioThread_InitMesgQueuesImpl(void);

/**
 * Empty.
 * No OoT counterpart. Zero's audio thread file starts with three empty stubs; OoT's thread.c starts
 * with AudioThread_Update.
 * No caller (2026-11-14): nothing in the built ROM jumps to it or holds its address, so play
 * cannot reach it.
 * @unverified Name from the code; it never runs.
 */
void AudioThread_Unused1(void) {
}

/**
 * Empty.
 * See AudioThread_Unused1.
 * No caller (2026-11-14): nothing in the built ROM jumps to it or holds its address, so play
 * cannot reach it.
 * @unverified Name from the code; it never runs.
 */
void AudioThread_Unused2(void) {
}

/**
 * Empty.
 * See AudioThread_Unused1.
 * No caller (2026-11-14): nothing in the built ROM jumps to it or holds its address, so play
 * cannot reach it.
 * @unverified Name from the code; it never runs.
 */
void AudioThread_Unused3(void) {
}

extern u8 D_8128967A;
extern u8 D_8128967B;
extern s8 D_8128967C;
extern M2C_UNK D_81289680;
extern M2C_UNK D_81289688;
extern M2C_UNK *D_812896A0;
extern M2C_UNK D_812896A8;
extern M2C_UNK D_812896B8;
extern OSMesgQueue* D_812896D0;
extern M2C_UNK D_812896D4;
extern M2C_UNK D_812896D8;
extern M2C_UNK *D_812896F0;

/**
 * Corresponds to OoT AudioThread_InitMesgQueuesImpl (original name Nap_AudioPortInit). Same three
 * queues of 1, 4 and 1 messages. Stadium 1's name BattleAnim_DisableEffectSpawning did not fit.
 * Seen in play in pokestadium0 (2026-11-13) ran when the GB Player started (main menu GB, Yes; second entry after
 * quitting).
 * Seen in play in pokestadium0 (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in
 * the Transfer Pak; called from AudioThread_InitMesgQueues.
 */
void AudioThread_InitMesgQueuesImpl(void) {
    D_8128967A = 0;
    D_8128967B = 0;
    D_8128967C = 0;
    D_812896A0 = &D_81289688;
    D_812896D0 = &D_812896B8;
    D_812896F0 = &D_812896D8;
    osCreateMesgQueue(D_812896A0, &D_81289680, 1);
    osCreateMesgQueue(D_812896D0, &D_812896A8, 4);
    osCreateMesgQueue(D_812896F0, &D_812896D4, 1);
}

extern s32 D_81288E78[][2];

/**
 * Appends a command to the 256-entry audio command ring; drops it when the ring is full.
 * Corresponds to OoT AudioThread_QueueCmd (original name Nap_PortSet).
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioThread_QueueCmd(DisplayObject* arg0, s32* arg1) {
    s32* entry = D_81288E78[D_8128967A & 0xFF]; // FAKE: no-op mask on a u8

    entry[0] = (s32)arg0;
    entry[1] = *arg1;
    if (D_8128967B == ++D_8128967A) {
        D_8128967A--;
    }
}

void AudioThread_QueueCmd(DisplayObject* arg0, s32* arg1);
/**
 * Queues a command with an f32 argument, passed by pointer from a stack copy.
 * Corresponds to OoT AudioThread_QueueCmdF32 (original name Nap_SetF32). Compiles to the same
 * machine code as AudioThread_QueueCmdS32; the argument is declared s32 here.
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioThread_QueueCmdF32(s32 arg0, s32 arg1) {
    AudioThread_QueueCmd((DisplayObject*)arg0, &arg1);
}

void AudioThread_QueueCmd(DisplayObject* arg0, s32* arg1);
/**
 * Queues a command with an s32 argument, passed by pointer from a stack copy. Callers pass a 32-bit
 * command word such as 0xF9000000 or 0xF1000000 as arg0.
 * Corresponds to OoT AudioThread_QueueCmdS32 (original name Nap_SetS32).
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioThread_QueueCmdS32(s32 arg0, s32 arg1) {
    AudioThread_QueueCmd((DisplayObject*)arg0, &arg1);
}

void AudioThread_QueueCmd(DisplayObject* arg0, s32* arg1);
/**
 * Queues a command with an s8 argument in the top byte.
 * Corresponds to OoT AudioThread_QueueCmdS8 (original name Nap_SetS8).
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioThread_QueueCmdS8(DisplayObject* arg0, s8 arg1) {
    s32 sp1C = arg1 << 24;

    AudioThread_QueueCmd(arg0, &sp1C);
}

/**
 * Queues a command with a u16 argument in the top half.
 * Corresponds to OoT AudioThread_QueueCmdU16 (original name Nap_SetU16).
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioThread_QueueCmdU16(DisplayObject* arg0, s16 arg1) {
    s32 sp1C = arg1 << 16;

    AudioThread_QueueCmd(arg0, &sp1C);
}

extern s32 D_8123B648;

/**
 * Sends the queued command range to the audio thread and tracks the largest batch seen.
 * Corresponds to OoT AudioThread_ScheduleProcessCmds (original name Nap_SendStart).
 * No return statement: retail leaves the send result in v0 and AudioThread_ResetAudioHeap returns it.
 * @unverified Name follows the counterpart; not checked in play.
 */
s32 AudioThread_ScheduleProcessCmds(void) {
    s32 head;
    s32 tail;
    s32 count = ((head = D_8128967A) - (tail = D_8128967B) + 0x100) & 0xFF;

    if (D_8123B648 < count) {
        D_8123B648 = count;
    }
    head = ((u8)tail << 8) | (u8)head; // FAKE: reuses head for the message
    if (osSendMesg(D_812896D0, head, 0) != -1) {
        D_8128967B = D_8128967A;
    }
}

/**
 * Drops the commands queued since the last send.
 * Corresponds to OoT AudioThread_ResetCmdQueue (original name Nap_FlushPort).
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioThread_ResetCmdQueue(void) {
    (*(u8 *)&D_8128967B) = (*(u8 *)&D_8128967A);
    (*(s8 *)&D_8128967C) = 0;
}

/**
 * Empty.
 * No OoT counterpart. It sits where OoT has AudioThread_ProcessCmd (Nap_Process1Command) and
 * AudioThread_ProcessCmds (Nap_AudioPortProcess).
 * No caller (2026-11-14): nothing in the built ROM jumps to it or holds its address, so play
 * cannot reach it.
 * @unverified Name from the code; it never runs.
 */
void AudioThread_Unused4(void) {
}

extern M2C_UNK D_81288A78;

/**
 * Polls the external load queue: returns the message's top byte and stores its low 24 bits in
 * `arg0`, or returns 0.
 * Corresponds to OoT func_800E5E20. OoT leaves it unnamed; the name is MM's.
 * @unverified Name follows the counterpart; not checked in play.
 */
u32 AudioThread_GetExternalLoadQueueMsg(s32 *arg0) {
    u32 sp1C;

    if (osRecvMesg(&D_81288A78, &sp1C, 0) == -1) {
        *arg0 = 0;
        return 0U;
    }
    *arg0 = sp1C & 0xFFFFFF;
    return sp1C >> 0x18;
}

/**
 * Returns the font list of sequence arg0 from AudioLoad_GetFontsForSequence; the font count
 * pointer that function takes is passed through untouched in the second argument register.
 * Corresponds to OoT AudioThread_GetFontsForSequence. Stadium 1's name Audio_CountCategorySounds
 * did not fit.
 */
s32 AudioThread_GetFontsForSequence(s32 arg0) {
    return AudioLoad_GetFontsForSequence(arg0);
}

extern u8 D_81287AF8;

/**
 * Polls the audio reset queue D_812896F0 once without blocking (osRecvMesg is osRecvMesg): returns 1 when the audio thread has reported the reset to the
 * current spec D_81287AF8 as done, otherwise 0.
 * Matching note: the unused leading `pad` local moves `sp18` to the ROM's
 * stack offset (sp+0x18); the two returns are written as separate
 * `return 0` / `return 1` paths because that is the form that compiles to the
 * ROM's branch layout.
 * Corresponds to OoT func_800E5EDC (original name Nap_CheckSpecChange). OoT leaves it unnamed; the
 * name follows the original.
 */
s32 AudioThread_CheckSpecChange(void) {
    UNUSED s32 pad; // FAKE: stack padding
    u32 sp18;

    if (osRecvMesg(D_812896F0, &sp18, 0) == -1) {
        return 0;
    }
    if (sp18 != D_81287AF8) {
        return 0;
    }
    return 1;
}

/**
 * Empties the audio reset queue D_812896F0: receives from it without blocking (osRecvMesg
 * with flag 0) until it reports that it is empty (-1), discarding the messages.
 * Matching note: the -1 is kept in a local assigned inside the loop body.
 * With a literal -1 in the condition the compiler hands out the saved
 * registers in a different order. Declaring `neg` before `sp28` puts `sp28`
 * at the ROM's stack offset (sp+0x28).
 * Corresponds to OoT func_800E5F34 (original name __ClearSpecChangeQ). OoT leaves it unnamed; the
 * name follows the original (MM: AudioThread_WaitForAudioResetQueueP).
 */
void AudioThread_ClearSpecChangeQueue(void) {
    s32 neg;
    u32 sp28;

    do {
        neg = -1;
    } while (osRecvMesg(D_812896F0, &sp28, 0) != neg);
}

void AudioThread_ClearSpecChangeQueue(void);
void AudioThread_QueueCmdS32(s32 arg0, s32 arg1);
s32 AudioThread_ScheduleProcessCmds(void);
extern volatile u8 D_81287AF7;
extern u8 D_81287AF8;
/**
 * Queues an audio heap reset to spec `arg0` (command 0xF9) and sends it.
 * Corresponds to OoT AudioThread_ResetAudioHeap (original name Nap_StartSpecChange).
 * @unverified Name follows the counterpart; not checked in play.
 */
s32 AudioThread_ResetAudioHeap(s32 arg0) {
    s32 v;
    u32 sp18;

    AudioThread_ClearSpecChangeQueue();
    v = D_81287AF7;
    if (v != 0) {
        AudioThread_ResetCmdQueue();
        if (arg0 == D_81287AF8) {
            return 0;
        }
        if (v >= 3) {
            return 0;
        }
        osRecvMesg(D_812896F0, &sp18, 1);
    }
    AudioThread_ClearSpecChangeQueue();
    AudioThread_QueueCmdS32(0xF9000000, arg0);
    return AudioThread_ScheduleProcessCmds();
}

extern volatile s32 D_812346FC;

/**
 * Prepares the audio driver for a reset (pre-NMI): starts the reset timer D_812346FC at 1, queues an
 * audio heap reset to spec 0 and clears the reset status D_81287AF7.
 * Corresponds to OoT AudioThread_PreNMIInternal (original name Nap_StartReset).
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioThread_PreNMIInternal(void) {
    D_812346FC = 1;
    AudioThread_ResetAudioHeap(0);
    D_81287AF7 = 0;
}

extern SequencePlayer D_81287B08[];

/**
 * Returns IO port `arg2` of channel `arg1` of sequence player `arg0`, or -1 when the player is off.
 * Corresponds to OoT AudioThread_GetChannelIO (original name Nap_ReadSubPort).
 * @unverified Name follows the counterpart; not checked in play.
 */
s8 AudioThread_GetChannelIO(s32 arg0, s32 arg1, s32 arg2) {
    SequencePlayer* ch = &D_81287B08[arg0];
    s8* p;

    if (ch->enabled) {
        p = ch->channels[arg1];
        return (p + arg2)[0xC4];
    }
    return -1;
}

extern s8 D_81287C5C;
/**
 * Returns IO port `arg1` of sequence player `arg0`.
 * Corresponds to OoT AudioThread_GetSeqPlayerIO (original name Nap_ReadGrpPort).
 * @unverified Name follows the counterpart; not checked in play.
 */
s8 AudioThread_GetSeqPlayerIO(s32 arg0, s32 arg1) {
    return *(&D_81287C5C + ((arg0 * 0x15C) + arg1));
}

M2C_UNK AudioHeap_InitPool(M2C_UNK *, s32, M2C_UNK);     /* extern */
extern M2C_UNK D_81287740;

/**
 * Sets up the external allocation pool.
 * Corresponds to OoT AudioThread_InitExternalPool.
 * @unverified Name follows the counterpart; not checked in play.
 */
void AudioThread_InitExternalPool(s32 arg0, M2C_UNK arg1) {
    AudioHeap_InitPool(&D_81287740, arg0, arg1);
}

/**
 * Disables the external allocation pool by clearing its start address (D_81287740).
 * Corresponds to OoT AudioThread_ResetExternalPool. Stadium 1's name
 * BattleAnim_ClearEffectStateFlag did not fit.
 */
void AudioThread_ResetExternalPool(void) {
    D_81287740 = 0;
}

/**
 * Empty.
 * No OoT counterpart; OoT's no-op command handlers are elsewhere in thread.c.
 * No caller (2026-11-14): nothing in the built ROM jumps to it or holds its address, so play
 * cannot reach it.
 * @unverified Name from the code; it never runs.
 */
void AudioThread_Unused5(void) {
}

/**
 * Sets up the audio thread message queues.
 * Corresponds to OoT AudioThread_InitMesgQueues (original name Nas_InitGAudio). Stadium 1's name
 * BattleAnim_DisableEffectSpawningAlias did not fit.
 * Seen in play in pokestadium0 (2026-11-14) ran when the GB Player started (main menu GB, はい "Yes") with JP Green in
 * the Transfer Pak; called from AudioLoad_Init.
 */
void AudioThread_InitMesgQueues(void) {
    AudioThread_InitMesgQueuesImpl();
}
