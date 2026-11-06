# PR #357 annotation completion and evidence audit

Prepared November 5, 2026 for [PR #357](https://github.com/team-hippodrome/pokestadium/pull/357). This supplement is intended for a separate stacked PR targeting `battle_mechanics_annotation`, not master. It restores the intended retail behavior rather than introducing a gameplay change.

## Scope and revision

- Repository: `team-hippodrome/pokestadium`.
- PR head inspected: `2f9d4f18fef696bd82da0afd0cfccf67c6164f5d`, branch `battle_mechanics_annotation`.
- Clean master baseline: `b93d599f41b1d82af5b02b62990a0ebea103798b`.
- Primary units: `src/fragments/battle_engine/battle_engine_mechanics.c` and `battle_engine_move_mechanics.c`.
- Shared changes are confined to declarations, fields, masks and consumers used by those units. Function names are carried through all ten regional `linker_scripts/<version>/symbol_addrs_code.txt` inputs without altering addresses.
- No debugger, emulator, visual observation or listening test was performed. This audit establishes static code meaning and ROM preservation, not independent human confirmation of presentation.

## Matching regression

Observation: the unchanged PR compiled successfully but failed retail verification. The original PR ROM had MD5 `057b9a9d76287ecde0abe118844ff4db`; a direct comparison against the US retail ROM found exactly two differing bytes:

| Zero-based ROM offset | PR byte | Retail byte |
| --- | --- | --- |
| `0x360F5B` | `02` | `04` |
| `0x360F63` | `02` | `04` |

The source cause is `UNIQUE_STATUS_REFLECT` being defined as `2`, the same value as Light Screen. The pre-annotation Reflect branch tests and sets `4`. Evidence: the PR's `BattleEffect_ReflectLightScreen` diff and the independent `4`/`2` defensive-stat checks in `InitBattleParameters`, both in the primary units named above.

Changing only Reflect's definition to `4` restored the normal US retail verifier and direct byte comparison. No instruction replacement, post-compile target-byte copying or behavioral workaround was added. The ordinary repository build pipeline, including its existing checksum/boot-library processing, was retained.

## Annotation evidence

### Separate runtime bytes

| Storage | Meaning and evidence | Confidence |
| --- | --- | --- |
| `BattleMon.statusCondition` and `BattleMonRuntime.pendingStatusCondition` | Major status. `BattleEffect_Sleep` writes a duration and `Battle_CheckMonCanAct` decrements bits 0-2; `Battle_StoreMajorStatus` copies the runtime byte to the selected roster Pokemon. | Direct producer/consumer evidence |
| `BattleMonRuntime.volatileStatusFlags`, offset `0x4D` | Separate flags. Substitute sets `0x10`, recharge sets `0x20`, Rage sets `0x40`, and Leech Seed sets `0x80`. Mist and Focus Energy set `0x02` and `0x04`. `HandleMoveEffectFlags` skips accuracy processing for `0x01`. | Direct producer/consumer evidence; no stronger claim about the origin of `0x01` |
| `BattleMonRuntime.uniqueStatusFlags`, offset `0x4E` | Toxic `0x01`, Light Screen `0x02`, Reflect `0x04`, Transform `0x08`. Their effect setters and damage/residual consumers agree. | Direct producer/consumer evidence |
| `BattleMonRuntime.immobilizedReason`, offset `0x4C` | Independent turn-continuation flags. Trapping and semi-invulnerability must not use the unrelated freeze/paralysis names even when the numeric bits coincide. | Direct setters and `Battle_CheckMonCanAct` consumers |

The old runtime `statusCondition` name was changed only for `BattleMonRuntime`; the roster Pokemon's actual `statusCondition` was retained. The sleep local formerly called `is_frozen` is now `was_recharging`. Haze's old major-status labels were replaced by the flags for the byte it actually masks. The unresolved `0x08` volatile bit remains numeric and explicitly unresolved.

Sources: the primary units; `src/3D140.h`; and the directly affected consumers in `battle_engine_com_decision_logic.c`, `battle_engine_battle_phases.c` and `battle_engine_action_selection.c`.

### Shared counters and layout

| Offset | Old field | Final field | Concrete evidence |
| --- | --- | --- | --- |
| `0x4D` | `statusCondition` | `volatileStatusFlags` | Independent setters described above. |
| `0x4F` | `isTrapped` | `effectTurnsLeft` | Initialized by Bide, Thrash, trapping and multi-hit handlers; decremented by their continuation paths. It is not a boolean. |
| `0x50` | `confusedCounter` | `confusionTurnsLeft` | Confusion handlers initialize it; the action gate decrements it. |
| `0x51` | `unk_51` | `toxicCounter` | Toxic initializes it; poison/burn and Leech Seed residual routines increment and multiply by it when Toxic is active. |
| `0x52` | `movesDisabled` | `disabledMoveState` | Disable packs `(slot + 1)` into the high nibble and duration into the low nibble; the action gate extracts/decrements them. |
| `0x54` | `multihitCount` | `effectCounter` | Used both as total multi-hit count and as Bide's accumulated damage; the continuation doubles accumulated Bide damage. |
| `0x5C` | `statChanges[6]` plus two padding bytes | `statChanges[8]` | Runtime initialization and Transform both access eight entries. The six identified stage roles do not justify inventing meanings for the other two. |

`BattleMonRuntime` remains `0x64` bytes. A 32-bit compile-time assertion fixture checked every listed offset, the eight-byte stage span, the total size and the separate screen masks. The multiplier field spelling `devisor` was corrected to `divisor`; the stat helper arguments now describe the actual base-stat and stage inputs.

### Misleading function names

All changes below preserve signatures and implementations. Their declarations, call sites and ten regional code-symbol maps are updated together.

| Previous name | Final name | Body-derived evidence |
| --- | --- | --- |
| `Battle_ExecuteMoveAnimation` | `Battle_LoadMoveCache` | Copies the six bytes of `gMoveData[usedMove - 1]`; invokes no animation. |
| `Battle_IncrementMoveUsage` | `Battle_RefundSelectedMovePP` | Increments selected PP, and stored PP outside Transform. |
| `Battle_PlayMoveSound` | `Battle_QueueCriticalHitMessage` | Selects critical/OHKO queue responses, records the result and clears the shared flag; invokes no sound routine. |
| `Battle_UpdateBattleLog` | `Battle_QueueEffectivenessMessage` | Queues a response based on `sBattleTypeEffectiveness`. |
| `Battle_ApplyAnimStateEffects` | `Battle_StoreMajorStatus` | Assigns the runtime major-status byte to the selected roster entry. |
| `Battle_ResetEntityState` | `Battle_QueueDamageSummary` | Queues an HP-loss band from the pre-action HP snapshot; does not reset an entity. |
| `Battle_ApplyDamageAnimation` | `Battle_ResolveMoveDamage` | Selects normal/half-HP/fixed damage and calls `Battle_ApplyDamage`. |
| `Battle_CheckAllBroughtFainted_HP` | `Battle_CheckDefeatAtStatusHp` | Substitutes runtime `unk_0E`, produced by the poison/burn damage routine, into the roster HP sum. |
| `Battle_CheckAllBroughtFainted_Status` | `Battle_CheckDefeatAtSeedHp` | Uses runtime `unk_10`, produced by Leech Seed HP staging. |
| `Battle_CheckAllBroughtFainted_PP` | `Battle_CheckDefeatAtCurrentHp` | Sums HP using runtime `currentHp`, not PP. |
| `Battle_CheckAllBroughtFainted_Level` | `Battle_CheckDefeatAtDisplayHp` | Sums HP using runtime `displayHp`, not level. |

Source: function bodies and direct callers in `battle_engine_mechanics.c`, plus staged-HP consumption in `battle_engine_battle_phases.c`. The HP-stage names describe code checkpoints, not a newly observed rendering sequence.

### Comments, dispatch slots and preserved behavior

- Function comments describe each primary unit's implemented responsibilities, including failure paths, stage limits, turn continuations and residual staging. Empty address-named entry points remain empty and address-named.
- `gMoveData[165]` in `src/gb_data.c` was inspected independently of the dispatch table. Live sleep moves use effect `0x20` (move IDs 47, 79, 95, 142 and 147). Slot `0x01` also dispatches to Sleep, but no move uses it; its name is now `EFFECT_SLEEP_UNUSED_01`.
- Slots `0x48` through `0x4B` dispatch to `BattleEffect_LowerStat`, despite previously being described as skipped. None of the 165 move rows uses those slots.
- The `BattleEffect_ApplyStatBoost` cap path writes `statChanges[increase_amount]` rather than `statChanges[move_effect_index]`. This observable indexing discrepancy is documented, not repaired. Reachability or a gameplay consequence was not asserted.
- Unsupported profile identities were removed from the mechanics-setting comment. Byte 0's default is `1`; the source also contains branches for other values. Complete alternative-profile identities remain unresolved.
- `include/types.h` now has an include guard. No type ID was changed.

## Verification

The clean master US baseline passed the normal production-toolchain ROM build and direct `cmp` before source integration. The Reflect-only control also passed before the annotation supplement.

Toolchain: the repository's Linux IDO 7.1 at `tools/ido/linux/7.1/cc`, SHA-256 `3864adf8b9f66dc5691bef3ed9083c5210217dc003f7460df5bd9386f78b4a5c`. Production builds use the repository Makefile's target flags, including `-O2 -mips2 -G 0 -non_shared -Xcpluscomm -nostdinc -Wab,-r4300_mul` and big-endian output.

| Target | Final retail verifier and direct byte comparison | Retail MD5 |
| --- | --- | --- |
| `us` | PASS | `ed1378bc12115f71209a77844965ba50` |
| `us_rev1` | PASS | `40e03eda831c01e0a12294287fd240e2` |
| `us_rev2` | PASS | `6dc6820cef755fc1253d06df45c9bd2a` |
| `jp` | PASS | `2449bb712a64e3363a6cbd56f5adeda5` |
| `eu` | PASS | `2859090d78581e0925a3af8045e81e4b` |
| `eu_rev1` | PASS | `31ee2de8e65e30f5934c450dbaa924f0` |
| `fr` | PASS | `0e85a098d0f0e8a7eb572a69612a6873` |
| `de` | PASS | `24be2ccb0dea0755908c02bf67e22fe5` |
| `es` | PASS | `d14a499bc4e324974eae3e42dec58625` |
| `it` | PASS | `a81321759af38beb30a40fdaca2ea22a` |

Additional checks:

- All 12 tests from `python -m unittest discover -s tools -p 'test_clangd*.py'` passed with Clang and clangd installed, including unopened-file rename and actual game-TU parsing tests; no skips.
- `python tools/clangd_config.py --check` passed after regeneration.
- The 32-bit layout/mask assertion fixture passed.
- `git diff --check` passed.
- All 110 function renames (11 names across 10 maps) preserve their original addresses and each map's symbol count.
- No stale function names, replaced counter names, `devisor` spelling or obsolete mixed-domain status macros remain in source/config inputs.

All final targets were extracted after the last function-name changes, then rebuilt and compared directly. Extraction emitted the repository-documented kind of missing-asset messages, including `assets/jp/rom_parts/52.bin` and `assets/es/snap_select_ui/0.bin`; these did not prevent the subsequent production builds or either retail verification gate. These are observed extraction-script diagnostics, not unexplained ROM mismatches.

Reproduction from the repository root in PowerShell, after documented prerequisites and legally supplied target ROMs:

```powershell
make setup
make extract VERSION=us
make -j4 VERSION=us
python tools/clangd_config.py --check
python -m unittest discover -s tools -p "test_clangd*.py"
git diff --check

# Repeat extraction/build with the appropriate VERSION for every affected target.
# On Windows, direct byte comparison of a built target:
cmd /c fc /b build\pokestadium-us.z64 baseroms\us\baserom.z64
Get-FileHash build\pokestadium-us.z64 -Algorithm MD5
```

The ordinary retail verifier runs as part of `make`; direct byte comparison is an additional gate. Regional preservation does not imply that every region compiles these functions from C, since some regions retain assembly-backed ownership.

## Sanity check and limits

The strongest new evidence is the paired producer/consumer source inspection and full-ROM byte comparison. No new visual, audio or gameplay meaning was inferred from an AI's perception; a human remains the authority for those observations.

Remaining opaque fields and globals were not assigned invented meanings. In particular, volatile bit `0x08`, the last two stage entries, some runtime fields and alternative mechanics-profile identities remain unresolved. Comments and source-based names are annotations, not a substitute for runtime validation.

The user explicitly authorized creating a new branch and publishing a stacked PR into #357's `battle_mechanics_annotation` branch. Direct updates to that base branch and merging either PR are outside the authorized scope.
