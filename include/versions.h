#ifndef VERSIONS_H
#define VERSIONS_H

// ROM offsets that the code passes as plain numbers instead of through a segment symbol,
// so they differ per version.
#if defined(VERSION_JP)
#define TRAINER_DATA_ROM_START 0x890000
#elif defined(VERSION_US_REV2)
#define TRAINER_DATA_ROM_START 0x895000
#elif defined(VERSION_PAL_LATE)
#define TRAINER_DATA_ROM_START 0x876000
#else
#define TRAINER_DATA_ROM_START 0x898000
#endif

// Values the European releases (50 Hz) change from the 60 Hz releases: frame counts and
// per-frame speeds. Gives `pal` in every VERSION_PAL build and `ntsc` otherwise.
#if defined(VERSION_PAL)
#define NTSC_PAL(ntsc, pal) (pal)
#else
#define NTSC_PAL(ntsc, pal) (ntsc)
#endif

// Values the Japanese release changes from the international ones. Gives `jp` in the
// VERSION_JP build and `intl` otherwise.
#if defined(VERSION_JP)
#define JP_INTL(jp, intl) (jp)
#else
#define JP_INTL(jp, intl) (intl)
#endif

#endif
