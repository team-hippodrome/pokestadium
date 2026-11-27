#include "src/fragments/minigame_hud/minigame_hud.h"

// JP-only unit with no code: the shared minigame labels. The other versions load
// this text from a text archive file through gMinigameText.
// gMinigameStrings is indexed by MinigameStringId.
#if defined(VERSION_JP)
char* gMinigameOwnerNames[2] = {
    "おいらのポケモン", // Our Pokémon
    "メカポケモン",     // Mecha Pokémon
};

char* gMinigameStrings[] = {
    "ゲームのせつめい",           // How to play
    "ゲームをはじめる",           // Start the game
    "３Ｄスティック",             // Control Stick
    "くびをふる",                 // Shake head
    "なげる",                     // Throw
    "Ａボタン",                   // A Button
    "かたくなる",                 // Harden
    "あなをほる",                 // Dig
    "じゅうじキー",               // Control Pad
    "・・・",                     // ...
    "Ｌボタン",                   // L Button
    "Ｒボタン",                   // R Button
    "１Ｐ",                       // 1P
    "２Ｐ",                       // 2P
    "３Ｐ",                       // 3P
    "４Ｐ",                       // 4P
    "はじめ",                     // Begin
    "おわり",                     // End
    "つぎ",                       // Next
    "１Ｐのかち",                 // 1P wins
    "２Ｐのかち",                 // 2P wins
    "３Ｐのかち",                 // 3P wins
    "４Ｐのかち",                 // 4P wins
    "ひきわけ",                   // Draw
    "のかち",                     // wins (after a name)
    "アーボでわなげ",             // Ring Toss with Ekans
    "かたくなるがっせん",         // Harden Battle
    "サンドのいどほりきょうそう", // Sandshrew's Well-Digging Race
};
#endif
