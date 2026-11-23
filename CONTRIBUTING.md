# Code of Conduct

## General

Don't act a fool.

## Making a PR
Please read the style guide in full before making a PR.


Human and AI-made PRs are evaluated equally, but you must specify in your PR if it is:
1. Majority Human-produced (0%-20% written by AI)
2. AI-assisted (20%-80% written by AI)
3. AI-generated (>80% written by AI)

Do **not** merge your PR until the CI-romgate check passes and confirms all versions still match. The only exception is if your PR touches only documentation.

### Matching PR
Please see the sections on formatting unknown tokens and fakes in the style guide and follow their guidance. Beyond that, if it matches, it matches.

### Annotation PR
Please briefly describe the evidence for your annotations: gameplay evidence, prior knowledge, or clues from the rest of the decomp.

## Documentation
Please do not include excessively long AI-generated `.md` files in your PRs. They will go stale quickly and give other agents undo confidence.

## Respect for the ones who came before
Never delete `oldnotes`. It stands as a testament to manual decomping that this project stands on.




# Editor setup and cross-file renaming

For VS Code, install the recommended **clangd** extension and run:

```sh
python3 tools/clangd_config.py
```

Then restart the clangd language server and let background indexing finish before using F2 on shared symbols. The workspace task can keep the database current automatically, including source-file additions and moves; ordinary identifier renames do not require regenerating it. No ROM, IDO compiler, or custom VS Code extension is needed for this editor setup.

See [docs/clangd.md](docs/clangd.md) for automatic-task permissions, regional analysis views, verification, and the limits of C-aware rename.

# Team Hippodrome Decomp Style Guide

## Formatting
IDO is not whitespace-neutral: whitespace materially affects code matching. Therefore, assume that any change to code spacing can cause a function to stop matching.

## Naming
### N64 Type Aliases
|Type|Alias|
|---|---|
|`unsigned char`|`u8`|
|`signed char`|`s8`|
|`short unsigned int`|`u16`|
|`short signed int`|`s16`|
|`long unsigned int`|`u32`|
|`long signed int`|`s32`|
|`long long unsigned int`|`u64`|
|`long long signed int`|`s64`|
|`float`|`f32`|
|`double`|`f64`|

### Cases

|Object|Style|Examples|
|---|---|---|
|Global-to-Game "Global" Variables|`gCamelCase`|`gPlayer1Controller`, `gSessionContext`,`gDisplayListHead`|
|Global-to-Fragment "Global" Variables|`sCamelCase` |`sCasteMapNodes`, `sSide2HpBar`,`sRuleDescWindow`|
|Local Function Variables (including arguments)|`snake_case`|`selected_menu_item`,`cpu_pokemon`,`type_effectiveness`|
|Functions|`{Domain}_{Purpose}`|`RentalHub_MenuLoop`,`Title_FadeOut`,`GraphNode_AppendChild`|
|Macro Variables|`CAPITAL_CASE`|`PIKACHU_SPECIES`, `CONFIRM_SFX`, `GIOVANNI`|
|External Library Functions (e.g., libultra)|Leave as is|`gSPDisplayList`,`gSPSprite2DBase`
|Structs|`TitleCase`|`QualifyingPokemonDisplay`,`HallOfFameSlot`,`GymLeader`|

### Naming Known Tokens
Because a decomp should be readable to a wide variety of people (recomp developers, romhackers, speedrunners, video essayists, and AI assistants helping confused players), our decomp repositories tolerate more specific naming than most modern "good practice." Function names can be up to 50 characters, so names like `RentalRules_QualifyingSpeciesWindowDraw` are acceptable.

### Naming Unknown Tokens
|Object|Style|Examples|
|---|---|---|
|Global Variables|`D_{Address}`|`D_8421187C`|
|Functions|`func_{address}` |`func_8420DBA0`|
|Structs|`unk_func_{address of func}` or `unk_D_{address of variable}`|`unk_func_80031270`,`unk_D_84102450`|
|Struct attributes|`unk_{offset}`|`unk_0A`,`unk_C0`|
|Function Arguments|`arg{index}`|`arg0`,`arg4`|
|Local variables|`var_{register}`|`var_v0`,`var_s1`|

Keep functions without a body named in the style of `func_{address}`, since we cannot make claims about their original purpose.

### Spelling
Use American English spelling (color, mom, etc.).

### Japanese Text
Japanese literal strings should have a English Translation in a comment alongside them regardless of if the text is in kana, kanji, or romaji.

### Terminology
#### Technical
Do not use yaw, pitch, or roll to describe rotations about an axis. Those terms are body-relative, while most rotations in Pokemon Stadium are world-relative. Use `rotationX`, `rotationY`, and `rotationZ`.

Prefer technically correct obscure language over ambiguous common language. For example: `alpha` instead of `fadeLevel`.

Do not include non-alphanumeric or non-ASCII symbols in variable names, even if they are accurate to the lore (Nidoran♂ should be `NIDORAN_MALE`).

Therefore, do not include é in Pokémon names used as variable names, because IDO does not accept non-ASCII characters in identifiers. The é is optional in documentation and comments.

Per [Bulbapedia](https://bulbapedia.bulbagarden.net/wiki/Pok%C3%A9_Ball): Poké Ball is two words; both when referring to the standard Poké Ball and when referring to the class of items called Poké Balls.

#### Mechanics
The format used in Stadium is commonly called "bring 6, pick 3." To distinguish Pokemon in the party from Pokemon chosen for a particular battle, use "brought Pokemon" and "picked Pokemon."

As AI is now a term to refer to a technology, use the term "COM" to refer to the non-player trainers since that is the label in Free Battle. Battle AI procedures may still be called `BattleAi` as a function domain.

##### Valid vs Legal vs Eligible Pokemon

A Pokemon is *valid* if it has valid summary data (move and species index within bounds, nickname uses valid characters for version, data structure matches). Invalid pokemon cannot even be read by the game

A pokemon is *legal* if it has moves it can learn in-game. Illegal pokemon are still allowed to be used in Stadium, but have their trainer's name marked in purple to make it clear the moveset is illegal.

A Pokemon is *eligible* if it meets level and species requirement of the cup in question.

A Level 70 Mewtwo that only nows Lick is:
- Valid (has the correct data structure)
- Illegal (Mewtwo cannot learn like)
- Only Eligible in prime cup and the gym leader castle.


#### Game

Use English names for Pokemon and Characters.

Never use the name Poke Cup. All versions of the game contain duplicate internal strings for Poke Cup. In the Japanese version, the three different versions were called Nintendo Cup.

The adopted term for the creature in Kangaskhan's pouch is Kangaskid.

Explosion and self-destruct are collectively referred to as "boom" (following competitive pokemon terminology). This is to allow for using the term explosion descriptively elsewhere.


##### Area Names 

Because the areas in Stadium are rather poorly named (the names in the game's dialogue can contradict the selection labels), this repository uses the more common names regardless of the selection label:

|Area Label|Repo/Common Name|
|---|---|
|Victory Palace|Hall of Fame|
|Pokemon Lab|Professor Oak's Lab|
|Kid's Club|Minigames|


#### Sensitive Language
Using the terms "blackface" and "purpleface" to describe Jynx's coloration in the JP and US 1.0 versions is discouraged. Use "original color" and "purple color" or "new color."

Nidoqueen's animation changed between JP and US 1.0, so documenting this change may require referring to Nidoqueen's breasts. Use the word "breasts," not slang such as "boobs" or the technical term for the breasts of the animal Nidoqueen represents.

That said, use of these sensitive terms outside code is none of our business.

## Comments 
### Documentation
All functions must have a `/**/` comment above them that describes their purpose.

### Fakes: Decomp Slop and Karodo 
"Fakes" are code anti-patterns that enable a function to match through register coloring but serve no other purpose. There are two types of fakes:
- **Decomp Slop** consists of fakes that no programmer would write, such as:
	- Type casting doom loops: `*((void **) (((u8 *) (*((void **) (((u8 *) arg0) + 0)))) + 4))`
	- Arithmetic nops: `arg0 + 1 - 1`, `var++; var--;`
- **Karodo** (from 過労 + code, "overworked code") refers to debugging idioms left in the ROM after their debugging functionality has been omitted:
	- Empty if statements `if (1) {}`
	- Empty enumeration: `for (i=0; i < n; i++) {}`
	- Empty pause: `while (0) {}`

Our philosophy is that even the worst, most disgusting match is better than the most beautiful non-match. Therefore, a new match should not be rejected simply because it contains fakes.

That said, fakes should always be marked with a `// FAKE` comment when used or encountered, so other maintainers can try to create matching code without them. If you encounter fakes while annotating, mark them as such even if the original committer did not; reviewers can miss details in generated changes.

Karodo may be left in with a `// karodo` comment if there appears to be no other way to remove it. Decomp slop should never have its `// FAKE` label changed. Never disguise decomp slop as karodo.

### Bugs
Bugs in the original game code should be labeled `//!bug`.

### Colors
Next to any color, add a comment in the format `// #{hex}` above or right next to the color to allow the color to be shown in vs code.

### Cross References
Reference other files with `//@{path}`

## Numbers

Hex is for bit structure. Decimal is for quantities.

### Must be hex
- Addresses, offsets, segment values
- Bitmasks and bit flags
- Packed color/channel values (RGBA words, RGB5A1, etc.)
- Binary angles (s16/u16 angle units where 0x10000 = one full turn)

### Must be decimal
- Loop bounds, array lengths, element counts
- Timers, frame counts, durations
- Game-logic quantities (HP, damage, speed, coordinates in world units)
- Indices

### Either is acceptable
- One-off powers of 2 and values one less than a power of 2 (`16` / `0x10`, `255` / `0xFF`, `4096` / `0x1000`). Pick whichever matches the role of the number: a count reads better in decimal, a width or alignment reads better in hex.

Do not change an existing literal from one acceptable form to the other (for example `15` to `0xF`) unless you are already editing that line for a functional reason. Matching the surrounding code beats matching this guide. A diff that only flips such a literal will be rejected.

### Everything else
A literal that falls under "Must be hex" or "Must be decimal" must use that base. Correct it even when the line is otherwise unchanged. For example, `buttonPressed & 512` becomes `0x200`, and `i < 0x10` as a loop bound becomes `16`.