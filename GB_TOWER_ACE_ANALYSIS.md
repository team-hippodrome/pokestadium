# GB Tower Emulator — ACE Feasibility Analysis

Can the known trade-machine ACE technique be adapted to the GB Tower emulator path to trigger Mewtwo?

---

## The Known ACE (Trade Machine)

The [publicly documented exploit](https://www.youtube.com/watch?v=Bb0v-VDsBkQ) targets the **trade machine** in Pokémon Stadium's Pokémon Lab:

1. Craft a GB save where **Box 12 has 23 Pokémon** (the buffer holds 20). The extra 3 entries (each `0x54` bytes) overflow into adjacent memory.
2. The overflow overwrites a **function pointer** at `801356B0`. When nonzero, the game jumps to whatever address it holds during the trade screen.
3. Point the function pointer into the **raw GB save data** copied to N64 RAM (Cart 2 boxes at `801E5E70`–`801E9330`), where a MIPS payload waits.
4. The payload sets `800BF732 = 0x02` (Round 2 flag → Mewtwo always appears), patches the main loop at `800077A8` to redirect through `80110000`, and installs a per-frame hook that zeros Mewtwo's HP at `80282D38`.

The Mewtwo trigger in the decomp is `STATE_VS_MEWTWO = 0x22` (index 3 in `gAreaSelectStateIds[]`), which dispatches to `State_VsMewtwo()` in `src/game_state.c:1008`. The Round 2 flag gates its appearance on the area-select map.

---

## GB Tower Emulator Architecture

The emulator is a **dynamic recompiler** (dynarec): it converts GB opcodes into native MIPS code at runtime. The entire emulator state lives in a single `0x1B058`-byte struct (`unk_D_8122B2C0`) allocated from a 2 MB pool.

### Emulator Context Memory Map

| Offset | Content | Size | Notes |
|--------|---------|------|-------|
| `0x00000` | GB CPU registers / scratchpad | `0x208` | |
| `0x00208` | Tile row caches | `0x180` | |
| `0x00388` | OAM + HRAM mirror | `0x400` | |
| `0x00788` | **⚠ VRAM buffer (16 KB — 2 CGB banks)** | `0x4000` | |
| `0x04788` | **🔴 CB-prefix opcode dispatch (fn ptrs)** | `0xC00` | **0 byte gap from VRAM!** |
| `0x05388` | Emulator control fields | `0x24` | |
| `0x053A8` | **🔴 Buffer pointers (scanline, VRAM cache, ROM)** | `0x18` | |
| `0x055AC` | **🔴 Memory map base pointers (8 read + 8 write)** | `0x80` | |
| `0x0562C` | **🔴 IO register write dispatch (fn ptrs)** | `0x200` | |
| `0x0582C` | ROM/SRAM bank validation bitmaps | `0x400` | |
| `0x05C2C` | **🔴 Saved CPU state ($ra at 0x5C48)** | `0x2C` | |
| `0x05C58` | GBPak ID, Transfer Pak state | `0xD98` | |
| `0x12DF0` | **✓ GB SRAM (save data, up to 32 KB)** | `0x8000` | |
| `0x1ADF0` | Trailing padding | `0x268` | |

🔴 marks **function pointer tables** — the high-value targets for any overflow. Note that the VRAM buffer ends *exactly* where the CB opcode dispatch table begins: **zero bytes of gap**.

---

## Comparison: Trade Machine vs. GB Tower

| Dimension | Trade Machine (Known ACE) | GB Tower Emulator |
|-----------|--------------------------|-------------------|
| **Untrusted data source** | GB save → box Pokémon count | GB cartridge ROM + SRAM |
| **Buffer overflow?** | **YES** — Box count > 20 overflows `0x54` bytes per extra entry | **NO** — ROM bank mask clamps to header-declared size; SRAM bank is 2-bit masked |
| **Function pointer nearby?** | Code pointer at `801356B0`, 0x268 bytes after Box 12 | CB dispatch at `0x4788` — 0 bytes after VRAM. But VRAM writes are bounded by memory map pointers. |
| **Bounds checking** | None on Pokémon count | GB addresses 16-bit (`& 0xFFFF`); memory map base pointers constrain all R/W |
| **ROM integrity** | N/A (save data only) | Full ROM checksum verified against header → `goto shutdown` on mismatch |
| **Cartridge validation** | None (any save accepted) | `GbPak_IsPokemonCartridge()` string-matches title; `DetectSupportedCartridgeVariant()` checks header fields |
| **SRAM placement** | Raw save at `801E4EA0`+, near code ptr | SRAM inside emu struct at offset `0x12DF0` — `0xD5C8` bytes *after* the last function pointer table |
| **Code execution path** | Game checks pointer, jumps if nonzero | Dynarec dispatch through opcode tables → would need corrupted fn ptr entry |

---

## Attack Surfaces Evaluated

### 1. VRAM → CB Opcode Table Adjacency — BLOCKED

The VRAM buffer (`0x4000` bytes at offset `0x788`) sits directly adjacent to the CB-prefix opcode dispatch table. If a VRAM write could exceed its bounds, it would overwrite function pointers used for every `CB xx` instruction.

However, VRAM is mapped to GB addresses `0x8000`–`0x9FFF` through the memory map base pointer at `emu->unk_55CC`. The write path computes `base_ptr + GB_addr`, and since GB addresses are 16-bit, the maximum offset into the VRAM buffer is `0x1FFF` (DMG) or `0x3FFF` (CGB dual-bank) — both within the `0x4000` allocation. Pokémon R/B/Y are DMG-only, so only `0x2000` of the `0x4000` buffer is reachable.

### 2. MBC Bank Switching — BLOCKED

ROM bank selection masks the bank number against `emu->unk_5494` (derived as `(romSize >> 14) - 1`). For a 1 MB ROM (Pokémon maximum): mask = `0x3F`, maximum offset = `0x3F × 0x4000 = 0xFC000` — within the `0x100000` ROM buffer. SRAM bank selection uses a 2-bit mask (`& 0x3`), capping at bank 3 → offset `0x6000` within the SRAM region.

### 3. ROM Checksum Bypass — BLOCKED

The emulator computes a running sum of every ROM byte during transfer, then verifies it against the header checksum field. A checksum mismatch triggers `D_8122B2FC = 1; goto shutdown`. You cannot modify any ROM byte without adjusting the checksum header, and the header itself is part of the summed region — the validation is circular and sound.

```c
if (emu->unk_5C5C.sum != (emu->unk_5D70[0] & 0xFFFF)) {
    D_8122B2FC = 1;
    goto shutdown;
}
```

### 4. SRAM → Function Pointer Overwrite — BLOCKED

Unlike the trade machine (where save data lands near a code pointer), the emulator's SRAM lives at offset `0x12DF0` inside the struct — **0xD5C8 bytes after** the last function pointer table at `0x582C`. Even a full 32 KB SRAM stops at offset `0x1ADF0`, within the struct's `0x1B058` boundary. Nothing exploitable is downstream.

### 5. Dynarec Code Cache Injection — BLOCKED

The dynarec templates (`D_812297C0`, `D_8122A260`) are in **.rodata** — read-only memory. They're loaded onto the RSP by pointer, and the pointers themselves are set from compile-time constants, not from cartridge data. There's no path from GB-controlled data into the code generation pipeline.

### 6. IO Write Dispatch Table Corruption — THEORETICAL

The IO write dispatch table at offset `0x562C` (128 function pointers) is indexed by `(addr - 0xFF00) * 4` and jumped to via `jr $a2`. If the VRAM base pointer at `emu->unk_55CC` could somehow be corrupted to point earlier in the struct, a VRAM "write" could land on this table. However, the base pointer is set during `GbEmu_InitContext` from the struct's own address and is only modified by the CGB VRAM bank switch handler — which is gated by the `unk_5485` flag set from the cartridge header's CGB bit. Pokémon R/B/Y have this bit clear.

### 7. Coroutine Saved $ra — THEORETICAL

At offset `0x5C48`, the emulator saves `$ra` (return address) when yielding for Transfer Pak DMA. Overwriting this would redirect execution on resume. But the saved state region is between the IO dispatch table and the GBPak ID fields — no GB-writable buffer is adjacent or overlapping.

---

## Verdict

> **The GB Tower emulator does not have a viable ACE path analogous to the trade machine.** The original researcher's conclusion — "extensive error checking is done on the GB code, and no such exploit is possible" — is confirmed by the decomp. Every data path from the cartridge into N64 RAM is properly bounded.

---

## Other Lab Machines — Single-Cartridge ACE Search

Goal: find a vulnerability requiring only **1 Transfer Pak + 1 GB cartridge** (the trade machine needs 2 of each).

### Root Cause

The Deck API in `src/pokemon_stats.c` reads entry counts directly from the GB save via `Deck_GetCount()` (line 1151). Counts are raw u8 values from the save — party count at `unk_9AC.unk_000`, box counts via `GbSave_GetBoxEntryCount()`. A crafted save can set these to 0–255. The capacity check exists only in the **write** path (`Deck_WriteEntry`), never in the **read** path. This means every caller of `Deck_ReadEntries` is responsible for its own bounds checking.

### Vulnerable Patterns Found

**1. Lab PC List — `LabPCList_LoadAllBoxes` (THEORETICAL ACE, 1 cart + Controller Pak)**

- **File**: `src/fragments/lab_pc_list/lab_pc_list_1E46B0.c:774-784`
- **Pattern**: Unbounded `while(true)` loop reading full BattleMon data (0x54 bytes/entry, save-controlled):
  ```c
  while (true) {
      if (Deck_ReadEntries(&var_s0->unk_00->unk_08, 1, temp_v0) == 0) break;
      var_s0->unk_00->unk_00 = var_s3;
      var_s0->unk_00->unk_04 = var_s1;
      var_s0++;
      var_s1++;
  }
  ```
- **Buffer**: `unk_10.unk_00` pointer array, 20 entries per box (allocated at line 743 via `PointerList_Initialize` with capacity from `LabPCList_AllocateBoxSlots` arg1=0x14)
- **Overflow**: If save box count > 20, `var_s0` walks past the pointer array into adjacent memory. `var_s0` is reset per box (line 772: `var_s0 = var_s3->unk_10.unk_00`), so overflow is per-box. Only the last GB box (box 11, index 220-239 in the pointer array) overflows past the entire allocation into adjacent pool memory.
- **Verdict**: THEORETICAL ACE — a complete exploit chain to a code pointer exists via triple-indirection and a two-stage data-flow attack. See detailed analysis below.

**2. Gallery Rental Viewer (NOT EXPLOITABLE, 1 cart)**

- **File**: `src/fragments/gallery_rental_viewer/gallery_rental_viewer_2AFDB0.c:60-62`
- **Pattern**: `while (Deck_ReadEntries(sp48, 1, var_v0) == 1) { ptr->unk_02[ptr->unk_00++] = i++; }`
- **Buffer**: `unk_02[]` in `unk_D_838067F0_003C`, 20 bytes (struct size 0x16)
- **Overflow writes sequential indices (0, 1, 2...)** — values are NOT attacker-controlled
- Past entry 22: corrupts `unk_015C` pointer in `unk_D_838067F0`, but with small integer values (0x14, 0x15, 0x16), making the pointer invalid rather than redirectable

**3. Fragment 61 — Rental Team Selection (NOT EXPLOITABLE, 1 cart)**

- **File**: `src/fragments/61/fragment61_2DFB60.c:808-853`
- Same index-only pattern as Gallery Rental Viewer, with category filter reducing overflow entries
- Values still uncontrolled sequential integers

### Safe Callers (properly bounded)

| Machine | File | Limit | Mechanism |
|---------|------|-------|-----------|
| Lab PC Box Mgmt | `lab_pc_1A9780.c:65` | 20 | Hardcoded `arg4=0x14` |
| Lab PC Swap Screen | `lab_pc_1AE680.c:195` | 6/20 | Hardcoded per deck type |
| Lab PC Items | `lab_pc_1B4EA0.c:40` | Capacity | `arg3->unk_04` from `ByteMatrix_Allocate` |
| Lab PC List Category | `lab_pc_list_1E46B0.c:983` | Capacity | `arg0->unk_10` from initialization |
| GB Tower Team Prep | `gb_tower_team_prep.c:25` | 6 | `Deck_ReadEntries(buf, 6, ...)` matches `BattleMon[6]` |
| Pikachu Reward | `pikachu_reward.c:101` | 1 | Single slot overwritten each iteration |
| Transfer Pak Select | `transfer_pak_select.c` | N/A | No Deck_ReadEntries calls |
| Pokedex | `pokedex_214920.c` | N/A | No Deck_ReadEntries calls |

### Other Observations

- **GbSave_MarkSeenBits** (`pokemon_stats.c:1433`): Out-of-bounds **read** (not write) when party/box count is inflated. Writes to a 19-byte seen-bitfield that is provably bounded by species index validation. Not exploitable.
- **Save_LoadContinueRecord** (`save_data.c:986`): N64 save data count drives a loop — requires crafted Controller Pak, not GB cartridge. Different attack surface.

---

## Lab PC List — Full Exploit Chain Analysis

### Pool Layout

`LabPCList_BuildListScreen` (`lab_pc_list_1CEA00.c:892`) calls `LabPCList_InitDeckSlots` (line 927) which allocates from a sequential bump pool (`mem_pool_alloc`). Allocations are contiguous and deterministic:

```
Alloc  Contents                     Size (bytes)  Zeroed?
-----  --------                     ------------  -------
[1]    Party ByteMatrix descriptors  1 × 0x1C      No (ByteMatrix_Allocate)
[2]    Party data structs            6 × 0x5C      Yes (bzero at line 730)
[3]    Party pointer array           6 × 4          No (initialized by loop)
[4]    GB ByteMatrix descriptors    12 × 0x1C      No (ByteMatrix_Allocate)
[5]    GB data structs             240 × 0x5C      Yes (bzero)
[6]    GB pointer array            240 × 4          No (initialized by loop)
[7]    N64 ByteMatrix descriptors  12 × 0x1C      No (ByteMatrix_Allocate)
[8]    N64 data structs           240 × 0x5C      Yes (bzero)
[9]    N64 pointer array          240 × 4          No (initialized by loop)
[10]   PointerList unk_03C        486 × 4          No (PointerList_Allocate)
[11]   PointerList unk_0CC        486 × 4          No (PointerList_Allocate)
```

GB box 11's 20-entry pointer subarray spans entries 220–239 of allocation [6]. Entry 240 (the first overflow byte) is the start of allocation [7] — the N64 ByteMatrix descriptors.

### Stage 1: Overflow → Struct Corruption

Each N64 ByteMatrix descriptor is 0x1C bytes, initialized by `LabPCList_AllocateBoxSlots`:

```
+0x00  unk_00 = &unk_0E4[2]     ← pointer to N64 category in main struct (VALID)
+0x04  unk_04 = box_index        ← 0 for first box
+0x08  unk_08 = 0                ← flag
+0x0C  unk_0C = 0                ← count
+0x10  unk_10.unk_00 = ptr       ← pointer into N64 pointer array [9] (VALID)
+0x14  unk_10.unk_04 = 20        ← capacity
+0x18  unk_10.unk_08 = 0         ← entry count
```

With GB box 11 count = **21** in the crafted save:
- Entries 0–19: normal writes into box 11's data structs
- Entry 20: `var_s0` reads the first 4 bytes of allocation [7] → N64 descriptor's `unk_00` = pointer to `unk_0E4[2]` at `parent + 0x124`
- The code dereferences this as `var_s0->unk_00->unk_08` → writes BattleMon (0x54 bytes) to `parent + 0x12C`
- After 21 reads, `Deck_ReadEntries` returns 0 → loop breaks cleanly (no crash)

Entry 21 would read N64 descriptor's `unk_04` = 0 → NULL dereference → crash. But with count exactly 21, the loop never reaches entry 21.

### Stage 1 Corruption Map

BattleMon (0x54 bytes) written to `parent + 0x12C` through `parent + 0x17F`:

```
BattleMon     Main struct
offset  field              offset   corrupted field
------  -----              ------   ---------------
+0x00   unk_00 (species)   +0x12C   unk_0E4[2].unk_08 (source index)
+0x02   unk_02 (item)      +0x12E
+0x04   unk_04 (u8)        +0x130   unk_0E4[2].unk_0C.unk_00 MSB    ─┐
+0x05   unk_05 (u8)        +0x131                                     │ ByteMatrix DATA
+0x06   unk_06 (u8)        +0x132                                     │ POINTER (4 bytes,
+0x07   unk_07 (u8)        +0x133   unk_0E4[2].unk_0C.unk_00 LSB    ─┘ fully controlled)
+0x08   unk_08 (u8)        +0x134   unk_0E4[2].unk_0C.unk_04 MSB    ── ByteMatrix capacity
+0x0C   unk_09[3] (u8)     +0x138   unk_0E4[2].unk_0C.unk_08 MSB    ─┐
+0x0D   pad0D (u8)         +0x139                                     │ ByteMatrix COUNT
+0x0E   unk_0E (u16)       +0x13A                                    ─┘ (= inner loop bound)
+0x10   unk_10 (u32)       +0x13C   unk_0E4[2].unk_0C.unk_0C        ── ByteMatrix row_size
+0x14   unk_14 (u16)       +0x140   unk_0E4[2].unk_1C               ── max slot count
+0x18   unk_18 (u16)       +0x144   unk_144.unk_00 HIGH             ─┐ TableView category
+0x1A   unk_1A (u16)       +0x146   unk_144.unk_00 LOW              ─┘ pointer (big-endian)
+0x1C   unk_1C (u16)       +0x148   unk_144.unk_04 HIGH             ─┐
+0x1E   unk_1E (u16)       +0x14A   unk_144.unk_04 LOW              ─┘
+0x20   unk_20[4] (u8×4)   +0x14C   unk_144.unk_08                  ── outer loop bound
+0x24   unk_24 (u8)        +0x150   unk_144.unk_0C (element_size)
+0x28   unk_28 (u16)       +0x154   unk_154[0].unk_00/01
+0x2C   unk_2C (u16)       +0x158   unk_154[0].unk_04 (valid flag)
+0x30   unk_30[0..3]       +0x15C   unk_154[0].unk_08 (dirty flag)
+0x34   unk_30[4..7]       +0x160   unk_154[0].unk_0C               ── item buffer pointer
+0x38   unk_30[8..10]      +0x164   unk_154[0].unk_10               ── item capacity
+0x3B   unk_3B[0..4]       +0x167   unk_154[0].unk_14 / unk_18
+0x44   unk_3B[9..10]      +0x170   unk_154[1].unk_00/01
+0x46   unk_46[0..10]      +0x172   unk_154[1] fields
+0x52   unk_52 (u8)        +0x17E
+0x53   unk_53 (u8)        +0x17F
```

Key observations:
- `unk_0E4[2].unk_00` (deck type, +0x124) and `unk_0E4[2].unk_04` (port, +0x128) are **NOT** corrupted — the BattleMon starts at +0x12C, so `Deck_Open` still uses the correct deck type `0x11` and port `0`
- `unk_0E4[2].unk_0C.unk_00` (ByteMatrix data pointer) is fully controlled by BM bytes 0x04–0x07
- `unk_144.unk_08` (outer loop bound) is controlled by BM.unk_20[0..3]
- `unk_154[0..1]` are corrupted but get **overwritten** by `LabPCList_InitDeckCategories` at line 935 (called after `LoadAllBoxes` returns)

### Stage 2: Controlled Re-entry via N64 Box Processing

The outer loop at line 764 (`for (i = 0; i < arg0->unk_08; ...)`) reads `arg0->unk_08` from memory each iteration. After the corruption:

**Required BattleMon values for exploitation:**

| BM field | Value | Effect |
|----------|-------|--------|
| `unk_20[0..3]` | `{0x00, 0x00, 0x00, 0x03}` | `arg0->unk_08 = 3` → outer loop continues to category 2 (N64 boxes) |
| `unk_04..07` | `addr_A` (big-endian) | `var_s5->unk_0C` = corrupted ByteMatrix pointer → redirects N64 box iteration to `addr_A` |
| `unk_09[3], pad0D, unk_0E` | `{0, 0, 0x0001}` | `var_s5->unk_14 = 1` → inner loop processes exactly 1 "N64 box" |
| `unk_18, unk_1A` | Encodes `&unk_0E4[0]` | `unk_144.unk_00` points back to original category array → `FlattenEntries` doesn't crash |

When the outer loop reaches i=2 (N64 category):
1. `Deck_Open(0x11, 0, 0, 0)` opens N64 box 0 from Controller Pak → succeeds if Controller Pak save exists
2. `var_s3 = var_s5->unk_0C` = `addr_A` (our crafted pointer into GB box data, allocation [5])
3. `var_s0 = var_s3->unk_10.unk_00` = `*(addr_A + 0x10)` — reads from within a GB BattleMon struct where we placed `addr_B`
4. `var_s0->unk_00` = `*(addr_B)` — reads from another GB BattleMon field where we placed `addr_C`
5. `Deck_ReadEntries(&var_s0->unk_00->unk_08, ...)` writes Controller Pak BattleMon (0x54 bytes) to **`addr_C + 0x08`**

### Triple-Indirection Pointer Chain

```
addr_A  ──→  GB box data (allocation [5])
             BattleMon bytes at addr_A+0x10 encode addr_B
                    │
addr_B  ──→  Another location in GB box data
             First 4 bytes at addr_B encode addr_C
                    │
addr_C  ──→  TARGET ADDRESS
             Deck_ReadEntries writes 0x54 bytes to addr_C + 0x08
             Code also writes addr_A to addr_C + 0x00 and 0 to addr_C + 0x04
             Total: 0x5C bytes at addr_C (first 8 fixed, last 0x54 from Controller Pak save)
```

The GB box data at allocation [5] is filled from the GB save before the overflow runs. Each 0x5C-byte data struct has 0x54 bytes of BattleMon at offset 0x08. We control every BattleMon byte. The first 8 bytes of each struct (back-pointer and index) are set by the code, but the BattleMon data is fully attacker-controlled.

### Stage 3: Code Pointer Overwrite

**Target: Saved `$ra` on the stack frame of `LabPCList_BuildListScreen`.**

The N64 box 0 BattleMon from the Controller Pak save writes 0x54 bytes starting at `addr_C + 0x08`. By setting `addr_C` such that this window overlaps the saved `$ra` register in `LabPCList_BuildListScreen`'s stack frame:

- The Controller Pak BattleMon byte at the corresponding offset encodes the **payload address** — a pointer to MIPS code embedded in the GB save data (at a known address in allocation [5])
- When `LabPCList_BuildListScreen` executes its epilogue (`lw $ra, offset($sp); jr $ra`), it returns to the payload

The stack address is predictable on N64 (no ASLR, deterministic call chain). The `LabPCList_BuildListScreen` function has a stack frame containing saved registers ($s0–$s7, $ra, $fp) plus locals (sp50–sp70, tmp, pad), with $ra at a fixed offset from $sp.

**Alternative target: Global code address (e.g., main loop at `800077A8`).**

Instead of the stack, `addr_C + 0x08` could target a known code or data address in RDRAM. Since N64 code runs from cached RAM (KSEG0, 0x80xxxxxx), overwriting instructions is viable. The trade machine ACE already demonstrates this by patching `800077A8`.

### Execution Order Summary

```
LabPCList_BuildListScreen:
  927: InitDeckSlots          ← allocations [1]–[9]
  928: PointerList_Allocate   ← allocation [10]
  929: PointerList_Allocate   ← allocation [11]
  930: LoadAllBoxes           ← ⚡ OVERFLOW + STAGE 2 WRITE HAPPEN HERE
  934: FlattenEntries         ← uses preserved unk_144.unk_00 (safe)
  935: LoadDecks              ← overwrites unk_154 corruption (irrelevant)
  937+: Widget setup          ← all from constants/args, not corrupted data
  958: WidgetTree_InitWidget
  960: unk_20 = LabPCList_RowMenuHandleInput
  961: unk_24 = LabPCList_SetRowMenuState
  965+: Child widget allocs
  ...
  return                      ← ⚡ JUMPS TO PAYLOAD via corrupted $ra
```

### Requirements

| Component | Content | Purpose |
|-----------|---------|---------|
| **GB cartridge save** | Box 11 count = 21; entry 20 = crafted BattleMon with pointer chain setup; several box entries encode MIPS payload + fake pointer structs | Stage 1 overflow + pointer chain data + payload code |
| **Controller Pak save** | N64 box 0 count = 1; entry 0 = BattleMon with payload address at stack-$ra-aligned offset | Stage 3 code pointer overwrite data |
| **Address knowledge** | Pool base address (deterministic), stack frame address (deterministic call chain), allocation [5] base | Required for all three pointer chain stages |

**Hardware: 1 Transfer Pak + 1 GB cartridge + 1 Controller Pak (built into controller).** No second Transfer Pak or second cartridge needed.

### Comparison with Trade Machine ACE

| Dimension | Trade Machine (Known) | Lab PC List (Theoretical) |
|-----------|----------------------|--------------------------|
| **Hardware** | 2 Transfer Paks + 2 GB carts | 1 Transfer Pak + 1 GB cart + Controller Pak |
| **Overflow type** | Flat buffer, linear | Pointer-indirect, per-box |
| **Pointer chain** | Direct (3 extra BattleMon entries land on function pointer) | Triple indirection (3 dereferences through crafted GB data) |
| **Code pointer** | BSS function pointer at `801356B0` | Stack saved `$ra` or global code address |
| **Payload location** | Cart 2 boxes at `801E5E70` | GB box data in pool allocation [5] |
| **Complexity** | Low (linear overflow + known address) | High (multi-stage pointer chain + 2 crafted saves) |
| **Status** | Proven, demonstrated | Theoretical, untested |

---

## Assessment: Single-Cartridge ACE

> **A theoretical ACE path exists through the Lab PC List `LabPCList_LoadAllBoxes` overflow (line 774).** It requires a crafted GB save + crafted Controller Pak save and eliminates the need for a second Transfer Pak and second GB cartridge. The exploit uses a three-stage attack: (1) overflow corrupts category metadata, (2) corrupted ByteMatrix pointer redirects N64 box loading through a triple-indirection pointer chain in the GB save data, (3) Controller Pak Pokemon data writes to the stack, overwriting `$ra` with a payload address.

> **The trade machine remains the only _confirmed_ ACE path** and is far simpler. This Lab PC List path is theoretically sound but untested and requires precise address knowledge + careful BattleMon byte engineering in two separate saves.

---

## Recommended Path: Use the Trade Machine

> **The known trade-machine ACE works with this decomp.** The trade machine fragment (`src/fragments/trade_machine/`) is the proven path.

The decomp already contains the complete trade machine code. The vulnerability is in `Trade_ReadDeckEntries()` at `src/fragments/trade_machine/trade_machine_289240.c:20-40` — it loops `while (Deck_ReadEntries(...) == 1)` into a 20-entry buffer with **zero bounds checking** on the Pokémon count. To trigger Mewtwo:

1. Use the existing `generate_save.py` from the reference project to produce `Blue.sav` with the Box 12 overflow and MIPS payload.
2. The payload sets the Round 2 flag (`800BF732 = 0x02`), which unlocks `STATE_VS_MEWTWO` on the area-select screen.
3. The per-frame hook zeros Mewtwo's HP at `80282D38`, guaranteeing an instant win.
4. Two Transfer Paks + two GB cartridges required: one clean Red save, one glitched Blue save.

The payload can also be entered through the GB Tower (by first achieving GB-side ACE to manipulate the save file), then returning to the White City to enter the lab. As the reference notes: "since it is possible to play first-generation Pokémon games within Stadium at the GB Tower, it is still possible to do everything from within Stadium proper."

---

## Key Addresses

| Address | Purpose | Source |
|---------|---------|--------|
| `801356B0` | Code pointer checked on trade screen (jump if nonzero) | Trade machine BSS |
| `801E5E70` | Cart 2 box data copied to N64 RAM (payload storage) | Trade save loader |
| `800BF732` | Round 2 flag — set to `0x02` for Mewtwo | Game state BSS |
| `80282D38` | Mewtwo's HP (zeroed by per-frame hook) | Battle state |
| `800077A8` | Main loop — patched to redirect through payload | Main loop |
| `80110000` | Secondary payload: per-frame HP zero + flag set | Written by payload |
| `STATE_VS_MEWTWO` | `= 0x22`, index 3 in `gAreaSelectStateIds[]` | `src/game_state.c` |
| Pool alloc [5] | GB box data — 240×0x5C bytes, payload + pointer chain | Lab PC List heap |
| `$sp + frame_offset` | `LabPCList_BuildListScreen` saved `$ra` | Stack (deterministic) |
