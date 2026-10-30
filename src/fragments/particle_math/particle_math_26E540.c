#include "particle_math.h"

/* Derived counters share one clock tick; "frame" does not establish a wall-clock rate. */
/* Advance the particle clock and derive its remainder, quotient, and quotient-parity counters. */
void Particle_UpdateFrameCounters(void) {
    sParticleFrameCounter++;
    sParticleFrameMod64 = sParticleFrameCounter & 0x3F;
    sParticleFrameMod32 = sParticleFrameCounter & 0x1F;
    sParticleFrameMod16 = sParticleFrameCounter & 0xF;
    sParticleFrameMod8 = sParticleFrameCounter & 7;
    sParticleFrameMod4 = sParticleFrameCounter & 3;
    sParticleFrameMod3 = sParticleFrameCounter % 3;
    sParticleFrameMod2 = sParticleFrameCounter & 1;
    sParticleFrameDiv64 = sParticleFrameCounter >> 6;
    sParticleFrameDiv32 = sParticleFrameCounter >> 5;
    sParticleFrameDiv16 = sParticleFrameCounter >> 4;
    sParticleFrameDiv8 = sParticleFrameCounter >> 3;
    sParticleFrameDiv4 = sParticleFrameCounter >> 2;
    sParticleFrameDiv3 = sParticleFrameCounter / 3;
    sParticleFrameDiv2 = sParticleFrameCounter >> 1;
    sParticleFrameDiv64Parity = sParticleFrameDiv64 & 1;
    sParticleFrameDiv32Parity = sParticleFrameDiv32 & 1;
    sParticleFrameDiv16Parity = sParticleFrameDiv16 & 1;
    sParticleFrameDiv8Parity = sParticleFrameDiv8 & 1;
    sParticleFrameDiv4Parity = sParticleFrameDiv4 & 1;
    sParticleFrameDiv3Parity = sParticleFrameDiv3 & 1;
    sParticleFrameDiv2Parity = sParticleFrameDiv2 & 1;
}

/* Advance the particle clock through the shared frame-counter updater. */
void Particle_UpdateFrameCountersAlias(void) {
    Particle_UpdateFrameCounters();
}
