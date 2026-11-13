#!/usr/bin/env python3 -I
"""
Pokemon Stadium Lab PC List ACE — Save Generator

Generates a Gen 1 (Pokemon Red) .sav that triggers the
LabPCList_LoadAllBoxes buffer overflow. Box 12 (current box)
has count=21; the 21st entry overflows the PointerList and
overwrites unk_144 (TableView) via BattleMon corruption.

EXPLOIT CHAIN:
  1. Box 12 count=21 → entry 20 overflows the 20-entry PointerList
  2. The 21st pointer slot reads from CtrPak ByteMatrix row 0
     → a back-pointer to unk_0E4[2]
  3. Deck_ReadEntries writes BattleMon at unk_0E4[2]+0x08 (main_struct+0x12C)
  4. BattleMon corruption zone: main_struct+0x12C..0x17F (0x54 bytes)
  5. LabPCList_FlattenEntries reads corrupted unk_144.unk_00 and unk_144.unk_08

Entry 20's GbPokemonRecord is built from OT names 0-2 of box 12:
  records[20] = unk_016 + 20*0x21 = +0x2AA = OT name area start
  OT[0][0..10] = GbRec[0x00..0x0A]
  OT[1][0..10] = GbRec[0x0B..0x15]
  OT[2][0..10] = GbRec[0x16..0x20]

Key controlled fields (via Pokemon_FromGbRecord → BattleMon):
  GbRec[0x15..0x16] → BM.unk_18 → unk_144.unk_00 high half (cat ptr)
  GbRec[0x17..0x18] → BM.unk_1A → unk_144.unk_00 low half  (cat ptr)
  GbRec[0x1D..0x20] → BM.unk_20 → unk_144.unk_08           (loop count)

Safe mode (default): category ptr → original unk_0E4 array,
loop count → 2 (process party + box, skip corrupted CtrPak).
The list loads normally minus Controller Pak entries.

Usage:
  python3 gen_lab_ace_save.py --addr 80200000 [-o lab_ace_exploit.sav]
"""
import argparse
import sys

# ═══════════════════════════════════════════════════════════════════
# ROM addresses (Pokemon Stadium US 1.0)
# ═══════════════════════════════════════════════════════════════════
STATE_RUN_CREDITS = 0x800291E0
G_SESSION_CONTEXT = 0x800AE540
SESSION_UNK_11F2  = G_SESSION_CONTEXT + 0x11F2  # 0x800AF732

# ═══════════════════════════════════════════════════════════════════
# MIPS payloads (stored in box bank 0 at file offset 0x4004)
# ═══════════════════════════════════════════════════════════════════

PAYLOAD_CREDITS = [
    0x3C088002,  # lui $t0, 0x8002
    0x350891E0,  # ori $t0, $t0, 0x91E0  → $t0 = 0x800291E0
    0x01000008,  # jr $t0
    0x00000000,  # nop
]

PAYLOAD_MEWTWO = [
    0x3C18800B,  # lui $t8, 0x800B
    0x24020002,  # li $v0, 2
    0xA302F732,  # sb $v0, -0x08CE($t8)  → *(0x800AF732) = 2
    0x03E00008,  # jr $ra
    0x00000000,  # nop
]

# ═══════════════════════════════════════════════════════════════════
# Gen 1 text encoding
# ═══════════════════════════════════════════════════════════════════

GEN1_CHARS = {chr(c): 0x80 + (c - ord('A')) for c in range(ord('A'), ord('Z') + 1)}
GEN1_CHARS.update({chr(c): 0xA0 + (c - ord('a')) for c in range(ord('a'), ord('z') + 1)})
GEN1_CHARS[' '] = 0x7F
GEN1_TERM = 0x50


def gen1_encode(name, length=11):
    buf = [GEN1_CHARS.get(c, GEN1_TERM) for c in name]
    buf.append(GEN1_TERM)
    while len(buf) < length:
        buf.append(0x00)
    return buf[:length]


def generate_save(main_struct_addr):
    sav = bytearray(0x8000)

    tableview_ptr = main_struct_addr + 0x0E4
    loop_count = 2

    # ═══════════════════════════════════════════════════════════════
    # Game state
    # ═══════════════════════════════════════════════════════════════
    sav[0x2598:0x259C] = [0x91, 0x84, 0x83, 0x50]  # player: "RED"
    sav[0x2605:0x2607] = [0x12, 0x34]               # trainer ID
    sav[0x260A] = 0x29                               # map: Poke Center
    sav[0x29F7] = 0x20                               # pokedex obtained

    # current box = 12 → box 12 data lives in unk_B40 (file 0x30C0)
    sav[0x284C] = 0x80 + 11

    # ═══════════════════════════════════════════════════════════════
    # Party: Bulbasaur (lv5) + Rattata (lv3)
    # Party struct at main+0x9AC (file 0x2F2C):
    #   +0x000 count, +0x001 species[7], +0x008 records[6]×0x2C,
    #   +0x110 OT names[6]×11, +0x152 nicknames[6]×11
    # ═══════════════════════════════════════════════════════════════
    sav[0x2F2C] = 2
    sav[0x2F2D:0x2F30] = [0x99, 0xA5, 0xFF]

    sav[0x2F34:0x2F34 + 0x2C] = [
        0x99,                    # species: Bulbasaur
        0x00, 0x14,              # current HP = 20
        0x05,                    # level
        0x00,                    # status
        0x16, 0x03,              # type: Grass/Poison
        0x2D,                    # catch rate = 45
        0x21, 0x2D, 0x00, 0x00,  # moves: Tackle, Growl
        0x12, 0x34,              # OT ID
        0x00, 0x00, 0x40,        # exp = 64
        0x00, 0x00,              # HP EV
        0x00, 0x00,              # Atk EV
        0x00, 0x00,              # Def EV
        0x00, 0x00,              # Spd EV
        0x00, 0x00,              # Spc EV
        0xFF, 0xFF,              # DVs
        0x23, 0x28, 0x00, 0x00,  # PP: 35, 40
        0x05,                    # actual level
        0x00, 0x14,              # max HP = 20
        0x00, 0x0B,              # attack
        0x00, 0x0B,              # defense
        0x00, 0x0B,              # speed
        0x00, 0x0E,              # special
    ]

    sav[0x2F60:0x2F60 + 0x2C] = [
        0xA5,                    # species: Rattata
        0x00, 0x0F,              # current HP = 15
        0x03,                    # level
        0x00,                    # status
        0x00, 0x00,              # type: Normal
        0xFF,                    # catch rate
        0x21, 0x27, 0x00, 0x00,  # moves: Tackle, Quick Attack
        0x12, 0x34,              # OT ID
        0x00, 0x00, 0x1B,        # exp = 27
        0x00, 0x00,              # HP EV
        0x00, 0x00,              # Atk EV
        0x00, 0x00,              # Def EV
        0x00, 0x00,              # Spd EV
        0x00, 0x00,              # Spc EV
        0x5B, 0xDE,              # DVs
        0x23, 0x1E, 0x00, 0x00,  # PP: 35, 30
        0x03,                    # actual level
        0x00, 0x0F,              # max HP
        0x00, 0x08,              # attack
        0x00, 0x07,              # defense
        0x00, 0x0A,              # speed
        0x00, 0x07,              # special
    ]

    # party OT names at file 0x303C (main+0xABC)
    sav[0x303C:0x303C + 11] = gen1_encode("RED")
    sav[0x3047:0x3047 + 11] = gen1_encode("RED")

    # party nicknames at file 0x307E (main+0xAFE)
    sav[0x307E:0x307E + 11] = gen1_encode("BULBASAUR")
    sav[0x3089:0x3089 + 11] = gen1_encode("RATTATA")

    # ═══════════════════════════════════════════════════════════════
    # All SRAM boxes empty
    # (box 12 is current box, its bank slot is stale)
    # ═══════════════════════════════════════════════════════════════
    sav[0x4000] = 0  # box 1
    sav[0x4462] = 0  # box 2
    sav[0x48C4] = 0  # box 3
    sav[0x4D26] = 0  # box 4
    sav[0x5188] = 0  # box 5
    sav[0x55EA] = 0  # box 6
    sav[0x6000] = 0  # box 7
    sav[0x6462] = 0  # box 8
    sav[0x68C4] = 0  # box 9
    sav[0x6D26] = 0  # box 10
    sav[0x7188] = 0  # box 11

    # ═══════════════════════════════════════════════════════════════
    # Current box (box 12) at file 0x30C0 — THE EXPLOIT
    #
    # GbSaveCurrentBox layout from the unk_B40 pointer:
    #   +0x000  count byte
    #   +0x001  species list (20 entries + 0xFF terminator = 0x15)
    #   +0x016  records[0..19] (20 × 0x21 = 0x294 bytes)
    #   +0x2AA  OT names[0..19] (20 × 11 = 0xDC bytes)
    #   +0x386  nicknames[0..19] (20 × 11 = 0xDC bytes)
    #
    # records[20] = +0x016 + 20*0x21 = +0x2AA = OT name area!
    # Entry 20's GbPokemonRecord (33 bytes) = OT[0..2]:
    #   OT[0][0..10] = GbRec[0x00..0x0A]
    #   OT[1][0..10] = GbRec[0x0B..0x15]
    #   OT[2][0..10] = GbRec[0x16..0x20]
    # ═══════════════════════════════════════════════════════════════
    sav[0x30C0] = 21

    for i in range(20):
        sav[0x30C1 + i] = 0x99  # species: Bulbasaur
    sav[0x30D5] = 0xFF           # terminator after 20 species

    # 20 box-format Bulbasaur records (0x21 bytes each) at 0x30D6
    bulba_rec = [
        0x99, 0x00, 0x14, 0x05, 0x00, 0x16, 0x03, 0x2D,
        0x21, 0x2D, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00,
        0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xFF, 0xFF, 0x23, 0x28, 0x00,
        0x00,
    ]
    for i in range(20):
        off = 0x30D6 + i * 0x21
        sav[off:off + 0x21] = bulba_rec

    # ── Exploit OT names: entry 20's GbPokemonRecord ─────────────
    #
    # GbRec byte  OT location    BM field       Corruption target
    # ─────────── ────────────── ────────────── ──────────────────
    # [0x00]      OT[0][0]       unk_00.01      unk_0E4[2].unk_08
    # [0x01-02]   OT[0][1-2]     unk_02         unk_0E4[2]+0x0A
    # [0x03-06]   OT[0][3-6]     unk_04-07      unk_0E4[2].unk_0C (data ptr)
    # [0x07]      OT[0][7]       unk_08         unk_0E4[2].unk_0C+4
    # [0x08-0A]   OT[0][8-10]    unk_09[0-2]    unk_0E4[2].unk_0C+5
    # [0x0B]      OT[1][0]       unk_09[3]      unk_0E4[2].unk_0C+8
    # [0x0C-0D]   OT[1][1-2]     unk_0E         unk_0E4[2].unk_0C+10
    # [0x0E-10]   OT[1][3-5]     unk_10         unk_0E4[2].unk_0C+12
    # [0x11-12]   OT[1][6-7]     unk_14         unk_0E4[2].unk_1C
    # [0x13-14]   OT[1][8-9]     unk_16         unk_0E4[2].unk_1C+2
    # [0x15]      OT[1][10]      unk_18 hi  ★   unk_144.unk_00 byte 0
    # [0x16]      OT[2][0]       unk_18 lo  ★   unk_144.unk_00 byte 1
    # [0x17]      OT[2][1]       unk_1A hi  ★   unk_144.unk_00 byte 2
    # [0x18]      OT[2][2]       unk_1A lo  ★   unk_144.unk_00 byte 3
    # [0x19-1A]   OT[2][3-4]     unk_1C         unk_144.unk_04
    # [0x1B-1C]   OT[2][5-6]     unk_1E         unk_144.unk_04+2
    # [0x1D]      OT[2][7]       unk_20[0]  ★   unk_144.unk_08 byte 0
    # [0x1E]      OT[2][8]       unk_20[1]  ★   unk_144.unk_08 byte 1
    # [0x1F]      OT[2][9]       unk_20[2]  ★   unk_144.unk_08 byte 2
    # [0x20]      OT[2][10]      unk_20[3]  ★   unk_144.unk_08 byte 3

    gbrec = bytearray(0x21)
    gbrec[0x00] = 0x15          # species: Mew
    gbrec[0x01] = 0x00          # HP hi
    gbrec[0x02] = 0x01          # HP lo = 1
    gbrec[0x03] = 0x05          # level = 5

    # ★ tableview category pointer (BM.unk_18 << 16 | BM.unk_1A)
    gbrec[0x15] = (tableview_ptr >> 24) & 0xFF
    gbrec[0x16] = (tableview_ptr >> 16) & 0xFF
    gbrec[0x17] = (tableview_ptr >>  8) & 0xFF
    gbrec[0x18] = (tableview_ptr      ) & 0xFF

    # ★ loop count (BM.unk_20[0..3], big-endian u32)
    gbrec[0x1D] = (loop_count >> 24) & 0xFF
    gbrec[0x1E] = (loop_count >> 16) & 0xFF
    gbrec[0x1F] = (loop_count >>  8) & 0xFF
    gbrec[0x20] = (loop_count      ) & 0xFF

    # write GbRec into OT names 0-2 at file offset 0x336A
    sav[0x336A:0x336A + 11] = gbrec[0x00:0x0B]  # OT[0]
    sav[0x3375:0x3375 + 11] = gbrec[0x0B:0x16]  # OT[1]
    sav[0x3380:0x3380 + 11] = gbrec[0x16:0x21]  # OT[2]

    # normal OT names for entries 3-19
    for i in range(3, 20):
        off = 0x336A + i * 11
        sav[off:off + 11] = gen1_encode("RED")

    # nicknames for entries 0-19
    for i in range(20):
        off = 0x3446 + i * 11
        sav[off:off + 11] = gen1_encode("BULBASAUR")

    # ═══════════════════════════════════════════════════════════════
    # MIPS payload in box bank 0 (0x4004+)
    # Box 1 count stays 0 at 0x4000; payload after the header.
    # ═══════════════════════════════════════════════════════════════
    payload = PAYLOAD_CREDITS + PAYLOAD_MEWTWO
    for i, word in enumerate(payload):
        off = 0x4004 + i * 4
        sav[off:off + 4] = word.to_bytes(4, 'big')

    # ═══════════════════════════════════════════════════════════════
    # Checksum: sum 0x2598..0x3522, complement at 0x3523
    # (same algorithm as GbSave_ComputeChecksum in pokemon_stats.c)
    # ═══════════════════════════════════════════════════════════════
    checksum = 255
    for x in sav[0x2598:0x3523]:
        checksum -= x
        checksum %= 256
    sav[0x3523] = checksum

    assert len(sav) == 0x8000
    return bytes(sav)


def main():
    parser = argparse.ArgumentParser(
        description="Generate a Gen 1 .sav triggering the Lab PC List overflow")
    parser.add_argument(
        "--addr", type=lambda x: int(x, 16), default=0x80200000,
        help="Main struct address in hex (default: 80200000 placeholder)")
    parser.add_argument(
        "-o", "--output", default="lab_ace_exploit.sav",
        help="Output filename (default: lab_ace_exploit.sav)")
    args = parser.parse_args()

    addr = args.addr
    sav_data = generate_save(addr)

    with open(args.output, "wb") as f:
        f.write(sav_data)

    # ── Verification output ───────────────────────────────────────
    tv = addr + 0x0E4
    print(f"Lab PC List ACE Save Generator")
    print(f"══════════════════════════════")
    if addr == 0x80200000:
        print(f"Main struct  : 0x{addr:08X}  (PLACEHOLDER — get real addr from emulator)")
    else:
        print(f"Main struct  : 0x{addr:08X}")
    print(f"unk_0E4 array: 0x{tv:08X}")
    print(f"unk_144 (TV) : 0x{addr + 0x144:08X}")
    print()
    print(f"Current box  : 12 (index 11, data at file 0x30C0)")
    print(f"Box count    : 21 (20 normal + 1 overflow)")
    print(f"Entry 20 rec : file 0x336A (= OT[0][0])")
    print()
    print(f"Corruption targets:")
    print(f"  unk_144.unk_00 (category ptr) → 0x{tv:08X}")
    print(f"  unk_144.unk_08 (loop count)   → 0x00000002")
    print()

    # dump the 3 exploit OT names
    print(f"Exploit OT names (entry 20's GbPokemonRecord):")
    for i in range(3):
        start = 0x336A - 0x2580 + i * 11  # struct offset within main data
        file_off = 0x336A + i * 11
        row = sav_data[file_off:file_off + 11]
        hex_str = " ".join(f"{b:02X}" for b in row)
        print(f"  OT[{i}] @ 0x{file_off:04X}: {hex_str}")
    print()

    print(f"Payload @ 0x4004: State_RunCredits (0x{STATE_RUN_CREDITS:08X})")
    print(f"                + Mewtwo flag     (0x{SESSION_UNK_11F2:08X})")
    print()
    print(f"Checksum byte  : 0x{sav_data[0x3523]:02X} @ file 0x3523")
    print(f"Wrote {len(sav_data)} bytes → {args.output}")


if __name__ == "__main__":
    main()
