# Main Struct Analysis: `unk_func_8830867C_02C`

## Overview

The central data structure throughout the entire codebase is `unk_func_8830867C_02C` — a **0xCE0-byte monolithic context struct** that represents the entire runtime state of the Lab PC List screen. Its name comes from the decompiler's automated naming convention: the function at address `0x8830867C`, accessed at offset `0x02C`.

---

## What It Is

It's the **Lab PC List fragment's "god object"** — every function in `lab_pc_list_1CEA00.c`, `lab_pc_list_1D7B40.c`, and `lab_pc_list_1E46B0.c` takes a pointer to it as `arg0`. It owns the entire screen: UI widgets, deck slot arrays, filter/sort state, Pokémon data for all three storage categories (GB, N64, Party), and the TableView that drives the display.

---

## Full Layout

| Offset | Field | Type | Purpose |
|--------|-------|------|---------|
| `0x000` | `unk_000` | `WidgetNode` | Base widget node — this struct IS a widget in the tree |
| `0x02C` | `unk_02C` | `WidgetAnimatedPanel*` | Animated panel (background/frame) |
| `0x030` | `unk_030` | ptr | Unknown sub-widget |
| `0x034` | `unk_034` | ptr | Column page sub-struct |
| `0x038–0x0CC` | widget ptrs | Various | ~20 `WidgetNode*` pointers + sub-widgets for every panel element |
| `0x0D8` | `unk_0D8` | `s32` | State/mode flag |
| `0x0DC` | `unk_0DC` | ptr | Filter state backup struct |
| `0x0E0` | `unk_0E0` | `s32` | Category count / loop bound |
| **`0x0E4`** | **`unk_0E4[3]`** | **`unk_func_8830867C_02C_0E4[3]`** | ⚠ The three deck categories: Party (0), GB boxes (1), N64 boxes (2) |
| **`0x144`** | **`unk_144`** | **`unk_func_8830867C_02C_144`** | ⚠ TableView controller — drives the visible list |
| `0x150` | padding | `char[4]` | |
| **`0x154`** | **`unk_154[3]`** | **`unk_func_8830867C_02C_154[3]`** | Item buffer slots for each category |
| `0x1A8` | `unk_1A8[3]` | array | Additional per-category data |
| `0x1D8` | padding/data | `char[0xA70]` | Large internal data block |
| `0xC48` | `unk_C48` | struct | Additional widget data |
| `0xC54` | `unk_C54` | `unk_func_8830867C_02C_C54` | Filter/sort config block A |
| `0xC84` | `unk_C84` | same | Filter/sort config block B |
| `0xCB4–0xCC8` | `unk_CB4`…`unk_CC8` | `s32` × 6 | State/mode integers (sort mode, dirty flags, etc.) |
| `0xCCC` | `unk_CCC` | ptr | Callback function pointer |
| `0xCD0` | `unk_CD0` | `s32` | |
| `0xCD4` | `unk_CD4` | `s32*` | |
| `0xCD8` | `unk_CD8` | `FragmentEntry` | Fragment lifecycle entry |
| `0xCDC` | `unk_CDC` | ptr | Sub-widget for the row menu |

---

## The Three Critical Sub-Structs

### `unk_0E4[3]` — Deck Category Descriptors

Size: `0x20` bytes each, total `0x60` bytes.

```c
typedef struct unk_func_8830867C_02C_0E4 {
    /* 0x00 */ s32 unk_00;               // deck type (0x11 = N64, etc.)
    /* 0x04 */ s32 unk_04;               // port / controller index
    /* 0x08 */ s32 unk_08;               // source/category index
    /* 0x0C */ unk_func_8820BE14_06C unk_0C;  // ByteMatrix descriptor:
                                              //   .unk_00 = data pointer  ← EXPLOIT TARGET
                                              //   .unk_04 = capacity
                                              //   .unk_08 = entry count
                                              //   .unk_0C = row_size
    /* 0x1C */ s32 unk_1C;               // max slot count
} // size = 0x20
```

Index 0 = Party, 1 = GB boxes, 2 = N64 boxes. The exploit's Stage 1 overflow writes a crafted `BattleMon` struct into `unk_0E4[2]` (the N64 category), specifically corrupting `unk_0C.unk_00` (the ByteMatrix data pointer) to redirect where Stage 2 reads from.

---

### `unk_144` — TableView Controller

Size: `0x0C` bytes minimum.

```c
typedef struct unk_func_8830867C_02C_144 {
    /* 0x00 */ unk_func_8830867C_02C_144_000* unk_00;  // category pointer  ← EXPLOIT TARGET
    /* 0x04 */ char[4];
    /* 0x08 */ s32 unk_08;                             // outer loop iteration count  ← EXPLOIT TARGET
} // size >= 0x0C
```

This is what the exploit's crafted `BattleMon` bytes at `unk_18` and `unk_20` corrupt:

- `unk_00` is set back to `main_struct + 0x0E4` (the real category array) to keep the loop valid.
- `unk_08` is set to `2` to skip the N64 category and prevent a second-stage crash.

---

### `unk_154[3]` — Item Buffer Slots

Size: `0x1C` bytes each.

```c
typedef struct unk_func_8830867C_02C_154 {
    /* 0x00 */ u8  unk_00, unk_01, unk_02;
    /* 0x04 */ s32 unk_04;    // valid flag
    /* 0x08 */ s32 unk_08;    // dirty flag
    /* 0x0C */ u8* unk_0C;    // item data buffer pointer
    /* 0x10 */ s32 unk_10;    // buffer capacity
    /* 0x14 */ s32 unk_14;
    /* 0x18 */ char[4];
} // size = 0x1C
```

These get corrupted by the Stage 1 overflow's tail bytes, but `LabPCList_LoadDecks` overwrites them immediately after, so the corruption is harmless there.

---

## Why Every File Points Back to It

| File | Role |
|------|------|
| `lab_pc_list.h` | Declares the struct and all ~30 functions that take `unk_func_8830867C_02C*` as `arg0` |
| `lab_pc_list_1CEA00.c` | `LabPCList_BuildListScreen`, `LabPCList_InitDeckSlots`, `LabPCList_LoadAllBoxes` — creates and initializes the struct; **where the overflow lives** |
| `lab_pc_list_1D7B40.c` | Input handling, sort/filter logic, row menu — all consume the struct |
| `lab_pc_list_1E46B0.c` | `LabPCList_LoadAllBoxes` inner loop (line 774), `LabPCList_AllocateBoxSlots` — the **specific overflow site** |
| `gen_lab_ace_save.py` | Crafts `unk_0E4[2]`'s data pointer and `unk_144`'s loop count by embedding exact bytes at the right offsets in the GB save's OT name fields |

---

## Exploit Relevance Summary

Every file in the project is ultimately about reading from or writing to `unk_func_8830867C_02C`. The entire exploit chain is a story about what happens when two of its fields get controlled by attacker data:

| Field | Exploit Role |
|-------|-------------|
| `unk_0E4[2].unk_0C.unk_00` | ByteMatrix data pointer — corrupted in Stage 1 to redirect Stage 2 reads into crafted GB box data |
| `unk_144.unk_08` | Outer loop count — set to `2` by the crafted BattleMon to control how many categories are processed |
| `unk_144.unk_00` | Category array pointer — restored to `main_struct + 0x0E4` to keep `FlattenEntries` from crashing |

The crafted OT names in box 12 of the GB save encode these exact values, routed through `Pokemon_FromGbRecord → BattleMon` field mapping, landing precisely at `main_struct + 0x12C` via the overflow write.
