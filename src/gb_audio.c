#include "gb_audio.h"
#include "gb_audio_render.h"

typedef struct unk_D_800FCD70 {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x03 */ u8 unk_03;
    /* 0x04 */ u8 unk_04;
} unk_D_800FCD70; // size = 0x5

typedef struct unk_D_800FCD98 {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ u8 unk_0C;
} unk_D_800FCD98; // size = 0x10

static u8 D_800FCD40[8];
static u8 D_800FCD48[8];
static u8 D_800FCD50[8];
static u8 D_800FCD58[8];
static u8 D_800FCD60[8];
static u8 D_800FCD68;
static unk_D_800FCD70 D_800FCD70[8];
static unk_D_800FCD98 D_800FCD98[8];
static u8 D_800FCE18[8];
static u8 D_800FCE20[8];
static u8 D_800FCE28[8];
static u8 D_800FCE30[8];
static u8 D_800FCE38[8];
static u8 D_800FCE40[8];
static u8* D_800FCE48[8];
static s32 D_800FCE68[8];
static s32 D_800FCE88[8];
static u8 D_800FCEA8[8];
static u16 D_800FCEB0;
static u16 D_800FCEB2;
static u8 D_800FCEB4;
static u8 D_800FCEB6[2];
static u8* D_800FCEB8[2];
static u8 D_800FCEC0[8];
static u32 D_800FCEC8;
static u8 D_800FCECC;
static u8 D_800FCECD;

static u8 D_80078590[] = {
    0x11, 0x22, 0x44, 0x88, 0x11, 0x22, 0x44, 0x88,
};
static u8 D_80078598[] = {
    0xEE, 0xDD, 0xBB, 0x77, 0xEE, 0xDD, 0xBB, 0x77,
};
static u8 D_800785A0 = 0;
static u8 D_800785A4 = 0xF0;
static u8 D_800785A8 = 0xF1;
static u8 D_800785AC = 0xFF;
static u8 D_800785B0 = 0;
static u8 D_800785B4 = 0;
static u8 D_800785B8 = 0xFF;

void GbMusic_UpdateChannels(void) {
    s32 i;

    for (i = 0; i < 8; i++) {
        if (D_800FCD48[i] != 0) {
            GbMusic_UpdateChannel(i);
        }
    }
}

void GbMusic_StartSequenceById(u8 arg0) {
    D_800FCEB4 = arg0;
    if (arg0 == 0xFF) {
        GbMusic_ResetApu();
    } else if ((D_800785A8 >= arg0) || (D_800785AC < arg0)) {
        GbMusic_StartSfx(arg0);
    } else {
        GbMusic_StartMusic(arg0);
    }
}

s32 GbMusic_StartTableSequence(u16 arg0) {
    s32 a;
    s32 offset;

    if (GbMusic_CountActiveHighChannels() != 0) {
        return 0;
    }

    func_80044A50(D_800FC6D4);

    if (D_800FC6D4->num_entries < arg0) {
        return 0;
    }

    offset = D_800FC6D4->offset;
    GbMusic_ResetApu();

    arg0--;

    a = D_800FC6D4->entries[arg0].unk_00;
    D_800FCEB4 = a;
    D_800FCECC = D_800FC6D4->entries[arg0].unk_02;
    D_800FCECD = D_800FC6D4->entries[arg0].unk_03;
    D_800FCEC8 = offset + a * 0x18;

    GbMusic_StartSfx(a);

    return 1;
}

void GbMusic_UpdateChannel(u32 arg0) {
    if (D_800FCD50[arg0] < 2) {
        D_800FCD70[arg0].unk_03 = D_800FCD70[arg0].unk_04;
        D_800FCD58[arg0] &= (u8)~0x30;
        GbMusic_DecodeChannelCommands(arg0);
        return;
    }

    D_800FCD50[arg0]--;

    if ((arg0 < 4) && (D_800FCD48[arg0 + 4] != 0)) {
        return;
    }

    if (D_800FCD58[arg0] & 0x40) {
        GbMusic_RotateChannelDuty(arg0);
    }

    if ((D_800FCD60[arg0] & 1) || (D_800FCD58[arg0] & 4)) {
        if (D_800FCD58[arg0] & 0x10) {
            GbMusic_AdvancePitchSlide(arg0);
        } else if (D_800FCD70[arg0].unk_03 != 0) {
            D_800FCD70[arg0].unk_03--;
        } else if (D_800FCD70[arg0].unk_00 != 0) {
            if (D_800FCD70[arg0].unk_01 & 0xF) {
                D_800FCD70[arg0].unk_01--;
            } else {
                u8 sp2F = D_800FCD70[arg0].unk_01;
                sp2F = (sp2F << 4) | ((sp2F >> 4) & 0xF);

                D_800FCD70[arg0].unk_01 |= sp2F;

                if ((D_800FCD58[arg0] & 8) != 0) {
                    sp2F = D_800FCD70[arg0].unk_00 & 0xF;
                    D_800FCD58[arg0] &= ~8;
                    if (D_800FCD70[arg0].unk_02 < sp2F) {
                        sp2F = D_800FCD58[arg0] * 0;
                    } else {
                        sp2F = D_800FCD70[arg0].unk_02 - sp2F;
                    }
                } else {
                    sp2F = (D_800FCD70[arg0].unk_00 & 0xF0) >> 4;
                    D_800FCD58[arg0] &= 8;
                    if ((0xFF - sp2F) < D_800FCD70[arg0].unk_02) {
                        sp2F = 0xFF;
                    } else {
                        sp2F += D_800FCD70[arg0].unk_02;
                    }
                }
                GbAudio_QueueRegisterWrite(GbMusic_GetChannelRegister(arg0, 3), sp2F, 0);
            }
        }
    }
}

void GbMusic_DecodeChannelCommands(u32 arg0) {
    u32 var_v0;
    u8 temp_v0;

    do {
        temp_v0 = GbMusic_ReadStreamByte(arg0);
        switch (temp_v0) {
            case 0xFF:
                var_v0 = GbMusic_CmdEndOrReturn(arg0);
                break;
            case 0xFD:
                var_v0 = GbMusic_CmdCall(arg0);
                break;
            case 0xFE:
                var_v0 = GbMusic_CmdLoop(arg0);
                break;
            case 0xE8:
                var_v0 = GbMusic_CmdToggleFinePitch(arg0);
                break;
            case 0xEA:
                var_v0 = GbMusic_CmdSetEnvelope(arg0);
                break;
            case 0xEB:
                var_v0 = GbMusic_CmdPitchSlide(arg0);
                break;
            case 0xEC:
                var_v0 = GbMusic_CmdSetDutyCycle(arg0);
                break;
            case 0xED:
                var_v0 = GbMusic_CmdSetTempo(arg0);
                break;
            case 0xEE:
                var_v0 = GbMusic_CmdSetPanMask(arg0);
                break;
            case 0xEF:
                var_v0 = GbMusic_CmdPlaySequence(arg0);
                break;
            case 0xFC:
                var_v0 = GbMusic_CmdSetDutyRotation(arg0);
                break;
            case 0xF0:
                var_v0 = GbMusic_CmdSetMasterVolume(arg0);
                break;
            case 0xF8:
                var_v0 = GbMusic_CmdEnableEnvelope(arg0);
                break;
            default:
                switch (temp_v0 & 0xF0) {
                    case 0xD0:
                        var_v0 = GbMusic_CmdSetNoteLength(arg0, temp_v0);
                        break;
                    case 0xE0:
                        var_v0 = GbMusic_CmdSetOctave(arg0, temp_v0);
                        break;
                    case 0x20:
                        var_v0 = GbMusic_CmdPlayNoteWithDuty(arg0, temp_v0);
                        break;
                    default:
                        var_v0 = GbMusic_CmdPlayNote(arg0, temp_v0);
                        break;
                }
                break;
        }
    } while (!var_v0 ^ 0);
}

u8 GbMusic_CmdEndOrReturn(u32 arg0) {
    if (D_800FCD58[arg0] & 2) {
        D_800FCD58[arg0] &= ~2;
        D_800FCE68[arg0] = D_800FCE48[arg0];
        return 0;
    }

    if (arg0 >= 3) {
        D_800FCD58[arg0] &= ~4;
        D_800FCD60[arg0] &= ~1;
        if (arg0 == 6) {
            GbAudio_QueueRegisterWrite(0xFF1A, 0, 0);
            GbAudio_QueueRegisterWrite(0xFF1A, 0x80, 0);
        }

        if (D_800FCD68 != 0) {
            D_800FCD68 = 0;
        }
        GbAudio_QueueRegisterWrite(0xFF25, D_80078598[arg0] & GbAudio_GetRegisterValue(0xFF25), 0);
    }

    D_800FCD48[arg0] = 0;

    if ((D_800FCD48[4] >= D_800785A0) && (D_800FCD48[4] < D_800785A4)) {
        if ((arg0 != 4) && (GbMusic_RewindIfSfxActive(arg0) != 0)) {
            return 1;
        } else {
            GbAudio_QueueRegisterWrite(0xFF24, D_800FCE40[0], 0);
            D_800FCE40[0] = 0;
            D_800FCD48[arg0] = 0;
        }
    }
    return 1;
}

u8 GbMusic_CmdCall(u32 arg0) {
    u8 sp1F;
    u8 temp_v0;
    s32 tmp;

    sp1F = GbMusic_ReadStreamByte(arg0);
    temp_v0 = GbMusic_ReadStreamByte(arg0);
    tmp = (temp_v0 << 8) + sp1F;

    D_800FCE68[arg0] = D_800FCE48[arg0];
    D_800FCE48[arg0] = tmp + D_800FCE88[arg0];
    D_800FCD58[arg0] |= 2;
    return 0;
}

u8 GbMusic_CmdLoop(u32 arg0) {
    u8 sp27;
    u8 sp26;
    u32 temp_v0 = GbMusic_ReadStreamByte(arg0);

    if (temp_v0) {
        if (D_800FCE30[arg0] == temp_v0) {
            D_800FCE30[arg0] = 1;
            GbMusic_ReadStreamByte(arg0);
            GbMusic_ReadStreamByte(arg0);
            return 0;
        }
        D_800FCE30[arg0]++;
    }

    sp27 = GbMusic_ReadStreamByte(arg0);
    sp26 = GbMusic_ReadStreamByte(arg0);

    D_800FCE48[arg0] = (u8*)D_800FCE88 + ((sp26 << 8) | sp27);
    return 0;
}

u8 GbMusic_CmdToggleFinePitch(u32 arg0) {
    D_800FCD58[arg0] ^= 1;
    return 0;
}

u8 GbMusic_CmdSetEnvelope(u32 arg0) {
    u8 temp_a0;
    u8 temp_a1;
    u8 temp_v0;
    u8 temp_t9;

    temp_v0 = GbMusic_ReadStreamByte(arg0);
    D_800FCD70[arg0].unk_03 = temp_v0;
    D_800FCD70[arg0].unk_04 = temp_v0;

    temp_v0 = GbMusic_ReadStreamByte(arg0);
    temp_a0 = temp_v0 & 0xF0;
    temp_a0 >>= 4;
    temp_a1 = temp_a0;
    temp_a1 >>= 1;
    temp_t9 = temp_a0 & 1;
    temp_a0 = temp_a1 + temp_t9;
    temp_a0 <<= 4;
    temp_t9 = temp_a0 | temp_a1;
    D_800FCD70[arg0].unk_00 = temp_t9;

    temp_t9 = temp_v0 & 0xF;
    temp_v0 = temp_t9 << 4;
    temp_v0 |= temp_t9;
    D_800FCD70[arg0].unk_01 = temp_v0;
    return 0;
}

u8 GbMusic_CmdPitchSlide(u32 arg0) {
    UNUSED s32 pad;
    u8 temp_v0;

    D_800FCD98[arg0].unk_0C = GbMusic_ReadStreamByte(arg0);
    temp_v0 = GbMusic_ReadStreamByte(arg0);
    D_800FCD98[arg0].unk_04 = GbMusic_GetNoteFrequency(temp_v0 & 0xF, (temp_v0 & 0xF0) >> 4);
    D_800FCD58[arg0] |= 0x10;
    GbMusic_ScheduleNote(arg0, GbMusic_ReadStreamByte(arg0));
    return 1;
}

u8 GbMusic_CmdSetDutyCycle(u32 arg0) {
    D_800FCE20[arg0] = GbMusic_ReadStreamByte(arg0) << 6;
    return 0;
}

u8 GbMusic_CmdSetTempo(u32 arg0) {
    u8 sp1F;
    u8 sp1E;

    if (arg0 < 4) {
        sp1F = GbMusic_ReadStreamByte(arg0);
        sp1E = GbMusic_ReadStreamByte(arg0);
        D_800FCEB0 = (sp1E << 8) + sp1F;
        D_800FCEC0[0] = 0;
        D_800FCEC0[1] = 0;
        D_800FCEC0[2] = 0;
        D_800FCEC0[3] = 0;
    } else {
        sp1F = GbMusic_ReadStreamByte(arg0);
        sp1E = GbMusic_ReadStreamByte(arg0);
        D_800FCEB2 = (sp1E << 8) + sp1F;
        D_800FCEC0[4] = 0;
        D_800FCEC0[5] = 0;
        D_800FCEC0[6] = 0;
        D_800FCEC0[7] = 0;
    }
    return 0;
}

u8 GbMusic_CmdSetPanMask(u32 arg0) {
    D_800FCE40[1] = GbMusic_ReadStreamByte(arg0);
    return 0;
}

u8 GbMusic_CmdPlaySequence(u32 arg0) {
    GbMusic_StartSequenceById(GbMusic_ReadStreamByte(arg0));
    if (D_800FCD68 == 0) {
        D_800FCD68 = D_800FCD48[7];
        D_800FCD48[7] = 0;
    }
    return 0;
}

u8 GbMusic_CmdSetDutyRotation(u32 arg0) {
    u8 temp_v0 = GbMusic_ReadStreamByte(arg0);

    D_800FCE18[arg0] = temp_v0;
    D_800FCE20[arg0] = temp_v0 & 0xC0;
    D_800FCD58[arg0] |= 0x40;
    return 0;
}

u8 GbMusic_CmdSetMasterVolume(u32 arg0) {
    GbAudio_QueueRegisterWrite(0xFF24, GbMusic_ReadStreamByte(arg0), 0);
    return 0;
}

u8 GbMusic_CmdEnableEnvelope(u32 arg0) {
    D_800FCD60[arg0] |= 1;
    return 0;
}

u8 GbMusic_CmdSetNoteLength(u32 arg0, u8 arg1) {
    u8 var_a1;

    D_800FCEA8[arg0] = arg1 & 0xF;
    if (arg0 != 3) {
        var_a1 = GbMusic_ReadStreamByte(arg0);
        if (arg0 == 2) {
            D_800FCEB6[0] = var_a1 & 0xF;
            var_a1 = (var_a1 & 0x30) * 2;
        } else if (arg0 == 6) {
            D_800FCEB6[1] = var_a1 & 0xF;
            var_a1 = (var_a1 & 0x30) * 2;
        }
        D_800FCE28[arg0] = var_a1;
    }
    return 0;
}

u8 GbMusic_CmdSetOctave(u32 arg0, u8 arg1) {
    D_800FCE38[arg0] = arg1 & 0xF;
    return 0;
}

s32 GbMusic_CmdPlayNoteWithDuty(u32 arg0, u8 arg1) {
    u8 sp27;
    u8 sp26;
    u8 sp25;
    u32 var_v1;

    if (((arg1 & 0xF0) != 0x20) || (arg0 < 3) || (D_800FCD60[arg0] & 1)) {
        var_v1 = GbMusic_CmdPlayNote(arg0, arg1);
    } else {
        sp25 = D_800FCE20[arg0] | GbMusic_ScheduleNote(arg0, arg1);
        GbAudio_QueueRegisterWrite(GbMusic_GetChannelRegister(arg0, 1), sp25, 0);
        sp27 = GbMusic_ReadStreamByte(arg0);
        GbAudio_QueueRegisterWrite(GbMusic_GetChannelRegister(arg0, 2), sp27, 0);
        sp26 = GbMusic_ReadStreamByte(arg0);
        sp27 = 0;
        if (arg0 != 7) {
            sp27 = GbMusic_ReadStreamByte(arg0);
        }
        GbMusic_WriteChannelDutyLength(arg0);
        GbMusic_UpdateChannelPanning(arg0);
        GbMusic_WriteChannelFrequency(arg0, (sp27 << 8) + sp26);
        var_v1 = 1;
    }
    return var_v1;
}

u8 GbMusic_CmdPlayNote(u32 arg0, u8 arg1) {
    if (arg0 < 4) {
        GbMusic_TriggerNoteFromByte(arg0, arg1);
    }

    if (arg1 != 0x10) {
        GbMusic_TriggerNoteFromByte(arg0, arg1);
        return 1;
    }

    if (D_800FCD60[arg0] & 1) {
        GbMusic_TriggerNoteFromByte(arg0, arg1);
        return 1;
    }

    GbAudio_QueueRegisterWrite(0xFF10, GbMusic_ReadStreamByte(arg0), 0);
    return 0;
}

u8 GbMusic_TriggerNoteFromByte(u32 arg0, u8 arg1) {
    u8 sp1F;
    s32 temp_v0 = arg1 & 0xF0;

    if ((arg0 != 3) || (temp_v0 > 0xB0)) {
        GbMusic_ScheduleNote(arg0, arg1);
    } else {
        if (temp_v0 == 0xB0) {
            sp1F = GbMusic_ReadStreamByte(arg0);
        } else {
            sp1F = temp_v0 >> 4;
        }

        if (D_800FCD68 == 0) {
            GbMusic_StartSequenceById(D_800FCEB4);
        }
    }

    GbMusic_ScheduleNote(arg0, sp1F);
    return 1;
}

s32 GbMusic_ScheduleNote(u32 arg0, u8 arg1) {
    u16 sp26;
    u16 tmp;
    u16 var_t0;
    u16 sp20;
    u8 sp1F;

    sp26 = (D_800FCEA8[arg0] * ((arg1 & 0xF) + 1));
    var_t0 = 0x100;
    if (arg0 >= 4) {
        if (arg0 != 7) {
            GbMusic_UpdateHighChannelTempo();
            var_t0 = D_800FCEB2;
        }
    } else {
        var_t0 = D_800FCEB0;
    }

    tmp = (D_800FCEC0[arg0] + (sp26 * var_t0));
    D_800FCEC0[arg0] = tmp & 0xFFFF;
    D_800FCD50[arg0] = (tmp & 0xFF00) >> 8;

    if (!(D_800FCD60[arg0] & 1) && (D_800FCD58[arg0] & 4)) {
        return D_800FCD50[arg0];
    } else if ((arg1 & 0xF0) != 0xC0) {
        return GbMusic_TriggerChannelNote(arg0, arg1, tmp);
    } else if ((arg0 < 4) && (D_800FCD48[4 + arg0] != 0)) {
        return D_800FCD48[4 + arg0];
    } else if ((arg0 == 2) || (arg0 == 6)) {
        sp1F = D_80078598[arg0] | GbAudio_GetRegisterValue(0xFF25);
        GbAudio_QueueRegisterWrite(0xFF25, sp1F, 0);
        return sp1F;
    } else {
        sp20 = GbMusic_GetChannelRegister(arg0, 2);
        GbAudio_QueueRegisterWrite(sp20, 8, 0);
        GbAudio_QueueRegisterWrite(sp20 + 2, 0x80, 0);
    }

    return 0x80;
}

s32 GbMusic_TriggerChannelNote(u32 arg0, u16 arg1, u16 arg2) {
    u16 var_s1;
    s32 tmp;
    u8 tmp1;

    arg1 = (arg1 & 0xF0) >> 8;

    tmp1 = D_800FCE38[arg0];
    var_s1 = GbMusic_GetNoteFrequency(arg1, tmp1);
    tmp = D_800FCD58[arg0] == 0;

    if (tmp & 0x10) {
        GbMusic_StartPitchSlide(arg0, var_s1);
    }

    if ((arg0 < 4) && (D_800FCD48[4 + arg0] != 0)) {
        return 1;
    }

    GbAudio_QueueRegisterWrite(GbMusic_GetChannelRegister(arg0, 2), D_800FCE28[arg0], 0);
    GbMusic_WriteChannelDutyLength(arg0);
    GbMusic_UpdateChannelPanning(arg0);
    if (D_800FCD58[arg0] & 1) {
        var_s1++;
    }
    D_800FCD70[arg0].unk_02 = var_s1 & 0xFF;
    GbMusic_WriteChannelFrequency(arg0, var_s1);
    return 1;
}

s32 GbMusic_UpdateChannelPanning(u32 arg0) {
    u8 tmp;
    u8 sp1E;
    u8 temp_v1 = D_80078590[arg0] | GbAudio_GetRegisterValue(0xFF25);

    if ((arg0 >= 4) && (arg0 != 7)) {
        GbAudio_QueueRegisterWrite(0xFF25, temp_v1, 0);
        return 1;
    }

    if ((arg0 != 7) && (D_800FCD48[4 + arg0] != 0)) {
        GbAudio_QueueRegisterWrite(0xFF25, temp_v1, 0);
        return 1;
    }

    sp1E = (D_800FCE40[1] & D_80078590[arg0]);
    tmp = D_80078598[arg0] & GbAudio_GetRegisterValue(0xFF25);
    tmp |= sp1E;
    GbAudio_QueueRegisterWrite(0xFF25, tmp, 0);
    return 1;
}

s32 GbMusic_WriteChannelDutyLength(u32 arg0) {
    u8 sp1F = D_800FCD50[arg0];
    u8 tmp;

    if ((arg0 != 2) && (arg0 != 6)) {
        tmp = D_800FCE20[arg0];

        sp1F &= 0x3F;
        sp1F |= tmp;
    }
    GbAudio_QueueRegisterWrite(GbMusic_GetChannelRegister(arg0, 1), sp1F, 0);
    return 1;
}

s32 GbMusic_WriteChannelFrequency(u32 arg0, u16 arg1) {
    u8 sp2E[4];
    s32 var_s2;
    u16 temp_v0;

    if ((arg0 == 2) || (arg0 == 6)) {
        s32 i;

        if (arg0 == 2) {
            var_s2 = 0;
        } else {
            var_s2 = 1;
        }
        GbAudio_QueueRegisterWrite(0xFF1A, 0, 0);

        for (i = 0; i < 16; i++) {
            GbAudio_QueueRegisterWrite(i + 0x20, D_800FCEB8[var_s2][i], 0);
        }

        GbAudio_QueueRegisterWrite(0xFF1A, 0x80, 0);
    }

    {
        u32 new_var2 = arg1 & 0xFF;
        u32 new_var = (((arg1 >> 8) & 0xFF) | 0x80) & 0xC7;

        sp2E[2] = new_var2;
        sp2E[3] = new_var;

        temp_v0 = GbMusic_GetChannelRegister(arg0, 3);
        GbAudio_QueueRegisterWrite(temp_v0, new_var2, 0);
        GbAudio_QueueRegisterWrite(temp_v0 + 1, new_var, 0);

        if (D_800FCD48[4] != 0) {
            GbMusic_WriteTransposedFrequency(sp2E[2] + (sp2E[3] << 8), temp_v0);
        }
    }
    return 1;
}

void GbMusic_UpdateHighChannelTempo(void) {
    s32 tmp;

    if (GbMusic_IsSfxActive() != 0) {
        tmp = D_800FCECD + 0x80;
        D_800FCEB2 = tmp;
    } else {
        D_800FCEB2 = 0x100;
    }
}

void GbMusic_WriteTransposedFrequency(u16 arg0, u16 arg1) {
    if (GbMusic_IsSfxActive() != 0) {
        u16 sp18 = D_800FCECC;

        arg0 += sp18;
        GbAudio_QueueRegisterWrite(arg1, arg0 & 0xFF, 0);
        GbAudio_QueueRegisterWrite(arg1 + 1, (arg0 >> 8) & 0xFF, 0);
    }
}

s32 GbMusic_RewindIfSfxActive(u32 arg0) {
    if (GbMusic_IsSfxActive() != 0) {
        D_800FCE48[arg0]--;
        return 1;
    }
    return 0;
}

s32 GbMusic_IsSfxActive(void) {
    if ((D_800FCD48[4] < D_800785A4) && (D_800FCD48[4] >= D_800785A0)) {
        return 1;
    }
    return 0;
}

void GbMusic_AdvancePitchSlide(u32 arg0) {
    u8 sp1F;
    u8 sp1E;
    u16 sp1C;

    D_800FCD98[arg0].unk_00 += D_800FCD98[arg0].unk_08;
    if (D_800FCD58[arg0] & 0x20) {
        if (D_800FCD98[arg0].unk_04 < D_800FCD98[arg0].unk_00) {
            D_800FCD58[arg0] &= ~0x30;
            return;
        }
    } else if (D_800FCD98[arg0].unk_00 < D_800FCD98[arg0].unk_04) {
        D_800FCD58[arg0] &= ~0x30;
        return;
    }

    sp1F = D_800FCD98[arg0].unk_00;
    sp1E = ((u16)D_800FCD98[arg0].unk_00 & 0xFF00) >> 8;
    sp1C = GbMusic_GetChannelRegister(arg0, 3);
    GbAudio_QueueRegisterWrite(sp1C, sp1F, 0);
    GbAudio_QueueRegisterWrite(sp1C + 1, sp1E, 0);
}

s32 GbMusic_StartPitchSlide(u32 arg0, u16 arg1) {
    D_800FCD98[arg0].unk_00 = arg1;
    if (arg1 < D_800FCD98[arg0].unk_04) {
        D_800FCD58[arg0] &= ~0x20;
    } else {
        D_800FCD58[arg0] |= 0x20;
    }
    D_800FCD98[arg0].unk_08 = (D_800FCD98[arg0].unk_04 - D_800FCD98[arg0].unk_00) / D_800FCD98[arg0].unk_0C;
    return 1;
}

void GbMusic_RotateChannelDuty(u32 arg0) {
    u8 sp1F;
    u16 sp1C;

    sp1F = D_800FCE18[arg0];
    sp1F &= 0xC0;
    D_800FCE18[arg0] *= 4;
    D_800FCE18[arg0] |= (sp1F >> 6);
    sp1F &= 0xC0;
    sp1C = GbMusic_GetChannelRegister(arg0, 1);
    GbAudio_QueueRegisterWrite(sp1C, (sp1F | (GbAudio_GetRegisterValue(sp1C) & 0x3F) & 0xFF), 0);
}

u8 GbMusic_ReadStreamByte(u32 arg0) {
    return *D_800FCE48[arg0]++;
}

u16 GbMusic_GetChannelRegister(u32 arg0, s32 arg1) {
    UNUSED s32 pad;
    u16 D_800785BC[] = {
        0xFF10, 0XFF15, 0xFF1A, 0XFF1F, 0xFF10, 0XFF15, 0xFF1A, 0XFF1F,
    };
    s32 ret = D_800785BC[arg0] + arg1;

    return ret;
}

u16 GbMusic_GetNoteFrequency(u8 arg0, u8 arg1) {
    s16 sp8[] = {
        0xF82C, 0xF89D, 0xF907, 0xF96B, 0xF9CA, 0xFA23, 0xFA77, 0xFAC7, 0xFB12, 0xFB58, 0xFB9B, 0xFBDA,
    };
    s16 ret = sp8[arg0];

    ret >>= 7 - arg1;
    ret += 0x800;
    return ret;
}

void GbMusic_StartMusic(u8 arg0) {
    s32 i;

    D_800FCD68 = 0;
    D_800FCEB0 = 0x100;
    D_800FCEB6[0] = 0;
    D_800FCEB6[1] = 0;

    for (i = 0; i < 8; i++) {
        D_800FCE68[i] = 0;
        D_800FCE48[i] = 0;
    }

    for (i = 0; i < 4; i++) {
        D_800FCD48[i] = 0;
        D_800FCD58[i] = 0;
        D_800FCE20[i] = 0;
        D_800FCE18[i] = 0;

        D_800FCD70[i].unk_03 = 0;
        D_800FCD70[i].unk_00 = 0;
        D_800FCD70[i].unk_01 = 0;
        D_800FCD70[i].unk_02 = 0;
        D_800FCD70[i].unk_04 = 0;

        D_800FCD60[i] = 0;

        D_800FCD98[i].unk_0C = 0;
        D_800FCD98[i].unk_00 = 0;
        D_800FCD98[i].unk_04 = 0;
        D_800FCD98[i].unk_08 = 0;

        D_800FCE30[i] = 1;
        D_800FCD50[i] = 1;
        D_800FCEA8[i] = 1;
    }

    D_800FCE40[1] = 0xFF;
    GbAudio_QueueRegisterWrite(0xFF24, 0, 0);
    GbAudio_QueueRegisterWrite(0xFF10, 8, 0);
    GbAudio_QueueRegisterWrite(0xFF25, 0, 0);
    GbAudio_QueueRegisterWrite(0xFF1A, 0, 0);
    GbAudio_QueueRegisterWrite(0xFF1A, 0x80, 0);
    GbAudio_QueueRegisterWrite(0xFF24, 0x77, 0);
    GbMusic_BindSequenceTracks(arg0);
}

void GbMusic_StartSfx(u8 arg0) {
    u8 i;

    D_800FCE40[1] = 0xFF;

    for (i = 4; i < 7; i++) {
        if (i == 6) {
            continue;
        }

        if (D_800FCD48[i]) {
            if (i == 7) {
                if (arg0 < D_800785B0) {
                    return;
                }
                if ((D_800FCD48[i] < D_800785B0) && (D_800FCD48[i] < arg0)) {
                    return;
                }
            } else if (D_800FCD48[i] < arg0) {
                return;
            }
        }

        D_800FCE68[i] = 0;
        D_800FCE48[i] = 0;
        D_800FCD48[i] = 0;

        D_800FCD58[i] = 0;
        D_800FCE20[i] = 0;
        D_800FCE18[i] = 0;

        D_800FCD70[i].unk_03 = 0;
        D_800FCD70[i].unk_00 = 0;
        D_800FCD70[i].unk_01 = 0;
        D_800FCD70[i].unk_02 = 0;
        D_800FCD70[i].unk_04 = 0;

        D_800FCD98[i].unk_0C = 0;
        D_800FCD98[i].unk_00 = 0.0f;
        D_800FCD98[i].unk_04 = 0.0f;
        D_800FCD98[i].unk_08 = 0.0f;

        D_800FCE30[i] = 1;
        D_800FCD50[i] = 1;
        D_800FCEA8[i] = 1;

        if (i == 4) {
            GbAudio_QueueRegisterWrite(0xFF10, 8, 0);
        }
    }

    GbMusic_BindSequenceTracks(arg0);
}

void GbMusic_ResetApu(void) {
    s32 i;

    GbAudio_QueueRegisterWrite(0xFF26, 0x80, 0);
    GbAudio_QueueRegisterWrite(0xFF1A, 0x80, 0);
    GbAudio_QueueRegisterWrite(0xFF25, 0, 0);
    GbAudio_QueueRegisterWrite(0xFF1C, 0, 0);
    GbAudio_QueueRegisterWrite(0xFF10, 8, 0);
    GbAudio_QueueRegisterWrite(0xFF12, 8, 0);
    GbAudio_QueueRegisterWrite(0xFF17, 8, 0);
    GbAudio_QueueRegisterWrite(0xFF21, 8, 0);
    GbAudio_QueueRegisterWrite(0xFF14, 0x40, 0);
    GbAudio_QueueRegisterWrite(0xFF19, 0x40, 0);
    GbAudio_QueueRegisterWrite(0xFF23, 0x40, 0);
    GbAudio_QueueRegisterWrite(0xFF24, 0x77, 0);

    D_800FCD68 = 0;
    D_800785B4 = 0;
    D_800FCEB0 = 0x100;
    D_800FCEB2 = 0x100;
    D_800FCEB6[0] = 0;
    D_800FCEB6[1] = 0;

    for (i = 0; i < 8; i++) {
        D_800FCD50[i] = 1;
        D_800FCE30[i] = 1;
        D_800FCEA8[i] = 1;
    }

    D_800FCE40[1] = 0xFF;
}

void GbMusic_BindSequenceTracks(u8 arg0) {
    u32 i;
    u8 x;
    u8 temp;
    u8 temp_v1;

    temp_v1 = ((unk_D_800FC6D4_008*)D_800FCEC8)->unk_04;
    temp = (temp_v1 >> 6) & 3;
    x = temp_v1 & 0xF;
    i = 0;
    while (1) {
        D_800FCD48[x] = arg0 + 1;
        if (x >= 3) {
            D_800FCD58[x] |= 4;
        }

        D_800FCE48[x] = ((unk_D_800FC6D4_008*)D_800FCEC8 + i)->unk_00;
        D_800FCE88[x] = D_800FCE48[x];
        i++;

        if (i > temp) {
            break;
        }
        x = ((unk_D_800FC6D4_008*)D_800FCEC8 + i)->unk_04 & 0xF;
    }

    if ((arg0 >= D_800785A0) && (arg0 < D_800785A4)) {
        D_800FCD48[4] = arg0 + 1;
        D_800FCD48[5] = arg0 + 1;
        D_800FCD48[6] = arg0 + 1;
        D_800FCD48[7] = arg0 + 1;
        D_800FCE48[6] = &D_800785B8;
        if (D_800FCE40[0] == 0) {
            D_800FCE40[0] = GbAudio_GetRegisterValue(0xFF24);
            GbAudio_QueueRegisterWrite(0xFF24, 0x77, 0);
        }
    }
}

s32 GbMusic_CountActiveHighChannels(void) {
    s32 ret = 0;
    s32 i;

    for (i = 0; i < 4; i++) {
        if (D_800FCD48[4 + i] != 0) {
            ret++;
        }
    }

    return ret;
}

typedef struct unk_80044A50_008 {
    u32 unk_00;
    u32 unk_04;
} unk_80044A50_008;

void func_80044A50(unk_D_800FC6D4* arg0) {
    s32 i, end;
    u32 base;
    u8* temp_v1;
    unk_80044A50_008* temp_v0;
    if (arg0->flags != 0)
        return;
    end = arg0->unk_04;
    base = (u32)arg0;
    temp_v1 = arg0->offset;
    temp_v1 += base;
    arg0->offset = (u32)temp_v1;
    for (i = 0; i != (end * 3); i++) {
        temp_v0 = (unk_80044A50_008*)((u32)i * 8 + (u32)temp_v1);
        temp_v0 += 0;
        temp_v0->unk_00 += base;
    }
    arg0->flags = 1;
}
