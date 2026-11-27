#include "pokemon_lab.h"

// JP only. Linked after pokemon_lab.c, so these come after its .data and .rodata.

// Unreferenced Pokédex description; the leading spaces look like a placeholder for a name.
UNUSED static char sLabUnusedPokedexDescription[] =
    "　　　　　たちがつかってるずかん\nよりせいのうのいい　ずかんじゃ。\nポケモンのすみかもわかるぞ。";

// Professor's description of each lab machine, indexed by the selected menu item.
char* sLabMachineDescriptions[] = {
    NULL,
    NULL,
    NULL,
    // "Lucky you, getting a Pokémon as a present! Go on, take it."
    "ポケモンをプレゼントしてもらえる\nとは　うらやましいのう。\nさあ　もっていってくれ。",
    // "A high-performance PC for Pokémon research, items and sorting boxes."
    "ポケモンのけんきゅうやどうぐ\nボックスのせいりなどができる\nこうせいのうパソコンじゃ。",
    // "The Pokémon trade machine. Trade directly with a friend's GB cartridge."
    "ポケモンこうかんのきかいじゃ。\nともだちのＧＢカートリッジと\nちょくせつこうかんできるぞ。",
    // "Switches between GB cartridges."
    "ＧＢカートリッジをきりかえる\nことができるぞ。",
    // "A machine for looking through your Pokédex in detail. It even shows where Pokémon live."
    "キミのずかんのなかを　くわしく\nみるきかいじゃ。\nポケモンの　すみかもわかるぞ。",
};
