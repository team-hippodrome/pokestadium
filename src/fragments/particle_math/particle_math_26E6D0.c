#include "particle_math.h"

/*
 * Fixed-format texture setup and draw callbacks, followed by indexed RGB palettes.
 * Palette operations change RGB only; primitive alpha has its own helpers.
 */
static u8 sParticlePalette[][3] = {
    { 0, 0, 0 }, /* [0] #000000 */
    { 0, 0x14, 0x1E }, /* [1] #00141E */
    { 0, 0x23, 0x2D }, /* [2] #00232D */
    { 0x40, 0x40, 0x40 }, /* [3] #404040 */
    { 0x46, 0x55, 0x55 }, /* [4] #465555 */
    { 0x6E, 0x6E, 0x6E }, /* [5] #6E6E6E */
    { 0x80, 0x80, 0x80 }, /* [6] #808080 */
    { 0xB4, 0xC8, 0xC8 }, /* [7] #B4C8C8 */
    { 0xC8, 0xFF, 0xFF }, /* [8] #C8FFFF */
    { 0xE6, 0xF0, 0xF0 }, /* [9] #E6F0F0 */
    { 0xFF, 0xFF, 0xFF }, /* [10] #FFFFFF */
    { 0xFF, 0, 0 }, /* [11] #FF0000 */
    { 0xF0, 0x28, 0x28 }, /* [12] #F02828 */
    { 0xFF, 0x80, 0x80 }, /* [13] #FF8080 */
    { 0xAA, 0x32, 0 }, /* [14] #AA3200 */
    { 0x96, 0, 0 }, /* [15] #960000 */
    { 0x64, 0, 0 }, /* [16] #640000 */
    { 0x46, 0x14, 0 }, /* [17] #461400 */
    { 0x32, 0xA, 0xA }, /* [18] #320A0A */
    { 0xFF, 0x80, 0 }, /* [19] #FF8000 */
    { 0xFF, 0xA0, 0 }, /* [20] #FFA000 */
    { 0xF0, 0xDC, 0xD8 }, /* [21] #F0DCD8 */
    { 0xFF, 0xC8, 0xC8 }, /* [22] #FFC8C8 */
    { 0xFF, 0x78, 0xC8 }, /* [23] #FF78C8 */
    { 0xFF, 0, 0x64 }, /* [24] #FF0064 */
    { 0xFF, 0xFF, 0 }, /* [25] #FFFF00 */
    { 0xFF, 0xFF, 0x64 }, /* [26] #FFFF64 */
    { 0xFF, 0xFF, 0x9B }, /* [27] #FFFF9B */
    { 0xFF, 0xFF, 0xC8 }, /* [28] #FFFFC8 */
    { 0xFF, 0xC8, 0 }, /* [29] #FFC800 */
    { 0xD7, 0xFF, 0x73 }, /* [30] #D7FF73 */
    { 0xC8, 0xC8, 0 }, /* [31] #C8C800 */
    { 0x96, 0x96, 0x32 }, /* [32] #969632 */
    { 0x64, 0x64, 0 }, /* [33] #646400 */
    { 0x80, 0x80, 0x50 }, /* [34] #808050 */
    { 0, 0xFF, 0 }, /* [35] #00FF00 */
    { 0x28, 0xFF, 0x28 }, /* [36] #28FF28 */
    { 0x64, 0xFF, 0x64 }, /* [37] #64FF64 */
    { 0x80, 0xFF, 0x80 }, /* [38] #80FF80 */
    { 0x9B, 0xFF, 0xC8 }, /* [39] #9BFFC8 */
    { 0x50, 0x64, 0x3C }, /* [40] #50643C */
    { 0xC8, 0xFF, 0 }, /* [41] #C8FF00 */
    { 0xC8, 0xC8, 0 }, /* [42] #C8C800 */
    { 0x96, 0xB4, 0 }, /* [43] #96B400 */
    { 0, 0x96, 0 }, /* [44] #009600 */
    { 0, 0x64, 0 }, /* [45] #006400 */
    { 0, 0, 0xFF }, /* [46] #0000FF */
    { 0x32, 0x32, 0xFF }, /* [47] #3232FF */
    { 0, 0x32, 0xFF }, /* [48] #0032FF */
    { 0, 0x64, 0xFF }, /* [49] #0064FF */
    { 0, 0x78, 0xFF }, /* [50] #0078FF */
    { 0x20, 0x7C, 0xFF }, /* [51] #207CFF */
    { 0x80, 0x80, 0xFF }, /* [52] #8080FF */
    { 0x64, 0xC8, 0xFF }, /* [53] #64C8FF */
    { 0, 0xFF, 0xFF }, /* [54] #00FFFF */
    { 0x64, 0xFF, 0xFF }, /* [55] #64FFFF */
    { 0xB4, 0xFF, 0xFF }, /* [56] #B4FFFF */
    { 0xC8, 0xFF, 0xFF }, /* [57] #C8FFFF */
    { 0x46, 0x64, 0x78 }, /* [58] #466478 */
    { 0xFF, 0, 0xFF }, /* [59] #FF00FF */
    { 0xFF, 0x32, 0xFF }, /* [60] #FF32FF */
    { 0xD7, 0, 0xFF }, /* [61] #D700FF */
    { 0x96, 0, 0xC8 }, /* [62] #9600C8 */
    { 0x64, 0, 0xFF }, /* [63] #6400FF */
    { 0xFF, 0x32, 0xFF }, /* [64] #FF32FF */
    { 0x78, 0x1E, 0x78 }, /* [65] #781E78 */
};

/* Six palette indices; pair-set selection uses this independent table. */
u8 sParticlePaletteSetA[] = {
    0x0C, 0x19, 0x23, 0x36, 0x2F, 0x3B,
};

/* Alternate six-entry palette-index table. */
static u8 sParticlePaletteSetB[] = {
    0x17, 0x19, 0x25, 0x23, 0x35, 0x14,
};

/* Five palette indices sampled by the primitive/environment random-palette helpers. */
static u8 sParticleRandomPaletteIndices[] = {
    0x0A, 0x19, 0x36, 0x24, 0x16,
};

/* Each row selects primitive RGB first, then environment RGB. */
static u8 sParticlePalettePairs[][2] = {
    { 0x19, 0xC },  { 0xA, 0x3B },  { 0, 0xC },     { 8, 0x31 },    { 0x2A, 0x23 }, { 0x19, 0x1F }, { 0x36, 0x32 },
    { 0x19, 0xB },  { 0x3B, 0x3C }, { 0xC, 0xC },   { 0xA, 0x1C },  { 0xA, 0x14 },  { 0xA, 0x35 },  { 0xA, 0x24 },
    { 0xA, 0xC },   { 0x19, 0x14 }, { 0x19, 0xC },  { 0x15, 0xA },  { 0xE, 0x11 },  { 8, 0x37 },    { 0x36, 0x30 },
    { 0xA, 0x3F },  { 0xB, 0 },     { 0xA, 0x19 },  { 0xA, 0x36 },  { 0x37, 0x31 }, { 0x19, 0xB },  { 0, 0x3B },
    { 0, 1 },       { 0xA, 0xA },   { 4, 2 },       { 0x29, 0x2C }, { 0xA, 0 },     { 0xC, 0x10 },  { 0x1B, 0x3B },
    { 0xA, 0x27 },  { 0x14, 0xB },  { 0xA, 0x1C },  { 0xA, 0x35 },  { 0xA, 8 },     { 0xA, 0x31 },  { 0x1C, 0x1A },
    { 0x1A, 0xB },  { 0x14, 0xF },  { 0x1A, 0x14 }, { 0x14, 0 },    { 0xB, 0 },     { 0x36, 0x2D }, { 0x23, 0x2C },
    { 0x1A, 0x20 }, { 0x21, 0x21 }, { 0xA, 0xA },   { 0x19, 0x2C }, { 0x3B, 0x3C }, { 0x2B, 0xA },  { 0x3E, 0xA },
    { 0x19, 0x23 }, { 0xA, 0x23 },  { 0xB, 0 },     { 0xA, 0x1A },  { 0xA, 0x35 },  { 0xA, 0x35 },  { 0xA, 0x17 },
    { 0xA, 0x3F },  { 0xA, 0x31 },  { 0xA, 0x17 },  { 0xA, 0x13 },  { 0x3C, 0x3E }, { 0x15, 0x15 }, { 0x3E, 0x3E },
    { 0x23, 0x23 }, { 0x13, 0xC },  { 0x19, 0x13 }, { 0x16, 0xC },  { 0x1C, 0x19 }, { 0x39, 0x2F }, { 0xA, 0x39 },
    { 0x38, 0x35 }, { 0x38, 0x39 }, { 0x1C, 0x1A }, { 9, 0x35 },
};

/* Load the supplied texture using ParticleGfx_LoadTextureRgba16. */
void ParticleGfx_LoadTextureCallbackRgba16_32x32(UNUSED Particle* particle, u8* texture) {
    ParticleGfx_LoadTextureRgba16(texture, 32, 32);
}

/* Load a texture frame selected by textureFrame using ParticleGfx_LoadTextureIa8. */
void ParticleGfx_LoadTextureCallbackIa8Frame_32x32(Particle* particle, u8* texture) {
    ParticleGfx_LoadTextureIa8(&texture[particle->textureFrame * 32 * 32], 32, 32);
}

/* Load the supplied texture using ParticleGfx_LoadTextureIa8. */
void ParticleGfx_LoadTextureCallbackIa8_32x32(UNUSED Particle* particle, u8* texture) {
    ParticleGfx_LoadTextureIa8(texture, 32, 32);
}

/* Load a texture frame selected by textureFrame using ParticleGfx_LoadTextureIa8. */
void ParticleGfx_LoadTextureCallbackIa8Frame_32x64(Particle* particle, u8* texture) {
    ParticleGfx_LoadTextureIa8(&texture[particle->textureFrame * 32 * 64], 32, 64);
}

/* Load the supplied texture using ParticleGfx_LoadTextureI4Alias. */
void ParticleGfx_LoadTextureCallbackI4_32x32(UNUSED Particle* particle, u8* texture) {
    ParticleGfx_LoadTextureI4Alias(texture, 32, 32);
}

/* Load a texture frame selected by textureFrame using ParticleGfx_LoadTextureI4Alias. */
void ParticleGfx_LoadTextureCallbackI4ParticleFrame_32x32(Particle* particle, u8* texture) {
    ParticleGfx_LoadTextureI4Alias(&texture[particle->textureFrame * (32 * 32 / 2)], 32, 32);
}

/* Load a texture frame selected by the shared particle clock using ParticleGfx_LoadTextureI4Alias. */
void ParticleGfx_LoadTextureCallbackI4FrameMod4_32x32(UNUSED Particle* particle, u8* texture) {
    ParticleGfx_LoadTextureI4Alias(&texture[sParticleFrameMod4 * (32 * 32 / 2)], 32, 32);
}

/* Load a texture frame selected by the shared particle clock using ParticleGfx_LoadTextureI4Alias. */
void ParticleGfx_LoadTextureCallbackI4FrameDiv2Mod4_32x32(UNUSED Particle* particle, u8* texture) {
    ParticleGfx_LoadTextureI4Alias(&texture[(sParticleFrameDiv2 & 3) * (32 * 32 / 2)], 32, 32);
}

/* Load a texture frame selected by the shared particle clock using ParticleGfx_LoadTextureI4Alias. */
void ParticleGfx_LoadTextureCallbackI4FrameDiv4Mod4_32x32(UNUSED Particle* particle, u8* texture) {
    ParticleGfx_LoadTextureI4Alias(&texture[(sParticleFrameDiv4 & 3) * (32 * 32 / 2)], 32, 32);
}

/* Load a texture frame selected by the shared particle clock using ParticleGfx_LoadTextureI4Alias. */
void ParticleGfx_LoadTextureCallbackI4FrameDiv2Mod8_32x32(UNUSED Particle* particle, u8* texture) {
    ParticleGfx_LoadTextureI4Alias(&texture[(sParticleFrameDiv2 & 7) * (32 * 32 / 2)], 32, 32);
}

/* Load a texture frame selected by the shared particle clock using ParticleGfx_LoadTextureTileI4Alias. */
void ParticleGfx_LoadTextureCallbackI4TileFrameDiv2Mod8_24x24(UNUSED Particle* particle, u8* texture) {
    ParticleGfx_LoadTextureTileI4Alias(&texture[(sParticleFrameDiv2 & 7) * (24 * 24 / 2)], 24, 24);
}

/* Load a texture frame selected by textureFrame using ParticleGfx_LoadTextureTileI4Alias. */
void ParticleGfx_LoadTextureCallbackI4TileParticleFrame_24x24(Particle* particle, u8* texture) {
    ParticleGfx_LoadTextureTileI4Alias(&texture[particle->textureFrame * (24 * 24 / 2)], 24, 24);
}

/* Load a texture frame selected by the shared particle clock using ParticleGfx_LoadTextureI4Alias. */
void ParticleGfx_LoadTextureCallbackI4FrameMod10_32x64(UNUSED Particle* particle, u8* texture) {
    ParticleGfx_LoadTextureI4Alias(&texture[(sParticleFrameCounter % 10) * (32 * 64 / 2)], 32, 64);
}

/* Load a texture frame selected by textureFrame using ParticleGfx_LoadTextureI4Alias. */
void ParticleGfx_LoadTextureCallbackI4ParticleFrame_32x64(Particle* particle, u8* texture) {
    ParticleGfx_LoadTextureI4Alias(&texture[particle->textureFrame * (32 * 64 / 2)], 32, 64);
}

/* Load the supplied texture using ParticleGfx_LoadTextureI4Alias. */
void ParticleGfx_LoadTextureCallbackI4_64x64(UNUSED Particle* particle, u8* texture) {
    ParticleGfx_LoadTextureI4Alias(texture, 64, 64);
}

/* Load a texture frame selected by the shared particle clock using ParticleGfx_LoadTextureI4Alias. */
void ParticleGfx_LoadTextureCallbackI4FrameMod8_64x64(UNUSED Particle* particle, u8* texture) {
    ParticleGfx_LoadTextureI4Alias(&texture[sParticleFrameMod8 * (64 * 64 / 2)], 64, 64);
}

/* Load a texture frame selected by the shared particle clock using ParticleGfx_LoadTextureI4Alias. */
void ParticleGfx_LoadTextureCallbackI4FrameDiv2Mod8_64x64(UNUSED Particle* particle, u8* texture) {
    ParticleGfx_LoadTextureI4Alias(&texture[(sParticleFrameDiv2 & 7) * (64 * 64 / 2)], 64, 64);
}

/* Load a texture frame selected by textureFrame using ParticleGfx_LoadTextureI4Alias. */
void ParticleGfx_LoadTextureCallbackI4ParticleFrame_64x64(Particle* particle, u8* texture) {
    ParticleGfx_LoadTextureI4Alias(&texture[particle->textureFrame * (64 * 64 / 2)], 64, 64);
}

/* Load the supplied texture using ParticleGfx_LoadTextureI4WrapST. */
void ParticleGfx_LoadTextureCallbackI4WrapST_64x64(UNUSED Particle* particle, u8* texture) {
    ParticleGfx_LoadTextureI4WrapST(texture, 64, 64);
}

/* Load the supplied texture using ParticleGfx_LoadTextureIa8WrapST. */
void ParticleGfx_LoadTextureCallbackIa8WrapST_64x64(UNUSED Particle* particle, u8* texture) {
    ParticleGfx_LoadTextureIa8WrapST(texture, 64, 64);
}

/* Draw the particle using its world position, scale, color/angle parameters, and the selected vertex template. */
void ParticleGfx_DrawBillboardPrim(Particle* particle, CameraNode* camera) {
    ParticleGfx_SetPrimColorTextureCombine(particle->primRed, particle->primGreen, particle->primBlue, particle->primAlpha);
    ParticleGfx_DrawModelQuad(camera, particle->worldPosition, particle->scale, D_8140E018);
}

/* Draw the particle using its world position, scale, color/angle parameters, and the selected vertex template. */
void ParticleGfx_DrawBillboardPrimEnv(Particle* particle, CameraNode* camera) {
    ParticleGfx_SetPrimEnvColorCombine(particle->primRed, particle->primGreen, particle->primBlue, particle->envRed, particle->envGreen, particle->envBlue, particle->primAlpha);
    ParticleGfx_DrawModelQuad(camera, particle->worldPosition, particle->scale, D_8140DFD8);
}

/* Disable back-face culling, draw the transformed primitive, then restore culling; the s32 function has no return statement. */
s32 ParticleGfx_DrawBillboardDoubleSided(Particle* particle, UNUSED CameraNode* camera) {
    gSPClearGeometryMode(gDisplayListHead++, G_CULL_BACK);

    ParticleGfx_SetPrimEnvColorCombine(particle->primRed, particle->primGreen, particle->primBlue, particle->envRed, particle->envGreen, particle->envBlue, particle->primAlpha);
    ParticleGfx_DrawTransformedQuad(particle->worldPosition, particle->rotation, particle->scale, D_8140DFD8);

    gSPSetGeometryMode(gDisplayListHead++, G_CULL_BACK);
}

/* Draw the particle using its world position, scale, color/angle parameters, and the selected vertex template. */
void ParticleGfx_DrawBillboardDefaultQuad(Particle* particle, CameraNode* camera) {
    ParticleGfx_DrawModelQuad(camera, particle->worldPosition, particle->scale, D_8140E018);
}

/* Draw the particle using its world position, scale, color/angle parameters, and the selected vertex template. */
void ParticleGfx_DrawBillboardPrimEnvQuad(Particle* particle, CameraNode* camera) {
    ParticleGfx_SetPrimEnvColorCombine(particle->primRed, particle->primGreen, particle->primBlue, particle->envRed, particle->envGreen, particle->envBlue, particle->primAlpha);
    ParticleGfx_DrawModelQuad(camera, particle->worldPosition, particle->scale, D_8140E018);
}

/* Draw the particle using its world position, scale, color/angle parameters, and the selected vertex template. */
void ParticleGfx_DrawBillboardTextureAlphaRotatedQuad(Particle* particle, CameraNode* camera) {
    ParticleGfx_SetTextureAlphaCombine(particle->primAlpha);
    ParticleGfx_DrawModelQuadRotated(camera, particle->worldPosition, particle->scale, D_8140E108, particle->rotation.z);
}

/* Draw the particle using its world position, scale, color/angle parameters, and the selected vertex template. */
void ParticleGfx_DrawBillboardOffsetQuad(Particle* particle, CameraNode* camera) {
    ParticleGfx_DrawModelQuad(camera, particle->worldPosition, particle->scale, D_8140E058);
}

/* Draw the particle using its world position, scale, color/angle parameters, and the selected vertex template. */
void ParticleGfx_DrawBillboardPrimEnvNoiseQuad(Particle* particle, CameraNode* camera) {
    ParticleGfx_SetPrimEnvNoiseCombine(particle->primRed, particle->primGreen, particle->primBlue, particle->envRed, particle->envGreen, particle->envBlue, particle->primAlpha);
    ParticleGfx_DrawModelQuad(camera, particle->worldPosition, particle->scale, D_8140E018);
}

/* Draw the particle using its world position, scale, color/angle parameters, and the selected vertex template. */
void ParticleGfx_DrawBillboardPrimEnvRotatedQuad(Particle* particle, CameraNode* camera) {
    ParticleGfx_SetPrimEnvColorCombine(particle->primRed, particle->primGreen, particle->primBlue, particle->envRed, particle->envGreen, particle->envBlue, particle->primAlpha);
    ParticleGfx_DrawModelQuadRotated(camera, particle->worldPosition, particle->scale, D_8140E018, particle->rotation.z);
}

/* Draw the particle using its world position, scale, color/angle parameters, and the selected vertex template. */
void ParticleGfx_DrawBillboardPrimEnvRotatedTriangle(Particle* particle, CameraNode* camera) {
    ParticleGfx_SetPrimEnvColorCombine(particle->primRed, particle->primGreen, particle->primBlue, particle->envRed, particle->envGreen, particle->envBlue, particle->primAlpha);
    ParticleGfx_DrawModelTriangleRotated(camera, particle->worldPosition, particle->scale, D_8140E0D8, particle->rotation.z);
}

/* Draw the particle using its world position, scale, color/angle parameters, and the selected vertex template. */
void ParticleGfx_DrawBillboardPrimEnvQuadVariantA(Particle* particle, CameraNode* camera) {
    ParticleGfx_SetPrimEnvColorCombine(particle->primRed, particle->primGreen, particle->primBlue, particle->envRed, particle->envGreen, particle->envBlue, particle->primAlpha);
    ParticleGfx_DrawModelQuad(camera, particle->worldPosition, particle->scale, D_8140E178);
}

/* Draw the particle using its world position, scale, color/angle parameters, and the selected vertex template. */
void ParticleGfx_DrawBillboardPrimEnvQuadVariantB(Particle* particle, CameraNode* camera) {
    ParticleGfx_SetPrimEnvColorCombine(particle->primRed, particle->primGreen, particle->primBlue, particle->envRed, particle->envGreen, particle->envBlue, particle->primAlpha);
    ParticleGfx_DrawModelQuad(camera, particle->worldPosition, particle->scale, D_8140E1F8);
}

/* Draw the particle using its world position, scale, color/angle parameters, and the selected vertex template. */
void ParticleGfx_DrawBillboardPrimEnvQuadVariantC(Particle* particle, CameraNode* camera) {
    ParticleGfx_SetPrimEnvColorCombine(particle->primRed, particle->primGreen, particle->primBlue, particle->envRed, particle->envGreen, particle->envBlue, particle->primAlpha);
    ParticleGfx_DrawModelQuad(camera, particle->worldPosition, particle->scale, D_8140E1B8);
}

/* Draw the particle using its world position, scale, color/angle parameters, and the selected vertex template. */
void ParticleGfx_DrawBillboardPrimEnvComposite(Particle* particle, CameraNode* camera) {
    ParticleGfx_SetPrimEnvColorCombine(particle->primRed, particle->primGreen, particle->primBlue, particle->envRed, particle->envGreen, particle->envBlue, particle->primAlpha);
    ParticleGfx_DrawModelQuad(camera, particle->worldPosition, particle->scale, D_8140E2B8);
}

/* Disable back-face culling, draw the transformed primitive, then restore culling; the s32 function has no return statement. */
s32 ParticleGfx_DrawBillboardPrimEnvTriangleDoubleSided(Particle* particle, UNUSED CameraNode* camera) {
    gSPClearGeometryMode(gDisplayListHead++, G_CULL_BACK);

    ParticleGfx_SetPrimEnvColorCombine(particle->primRed, particle->primGreen, particle->primBlue, particle->envRed, particle->envGreen, particle->envBlue, particle->primAlpha);
    ParticleGfx_DrawTransformedQuad(particle->worldPosition, particle->rotation, particle->scale, D_8140E2B8);

    gSPSetGeometryMode(gDisplayListHead++, G_CULL_BACK);
}

/* Draw the particle using its world position, scale, color/angle parameters, and the selected vertex template. */
void ParticleGfx_DrawBillboardPrimEnvTransformedQuad(Particle* particle, CameraNode* camera) {
    ParticleGfx_SetPrimEnvColorCombine(particle->primRed, particle->primGreen, particle->primBlue, particle->envRed, particle->envGreen, particle->envBlue, particle->primAlpha);
    ParticleGfx_DrawTransformedQuad(particle->worldPosition, particle->rotation, particle->scale, D_8140E378);
}

/* Copy the selected RGB palette entry or pair into the caller's color buffers. */
void ParticleGfx_CopyPaletteRgb(u8* destination, u16 palette_index) {
    destination[0] = sParticlePalette[palette_index][0];
    destination[1] = sParticlePalette[palette_index][1];
    destination[2] = sParticlePalette[palette_index][2];
}

/* Copy the selected RGB palette entry or pair into the caller's color buffers. */
void ParticleGfx_CopyPalettePairRgb(u8* prim_color, u8* env_color, u16 pair_index) {
    u8 sp1C = sParticlePalettePairs[pair_index][0];
    u8 sp1E = sParticlePalettePairs[pair_index][1];

    ParticleGfx_CopyPaletteRgb(prim_color, sp1C);
    ParticleGfx_CopyPaletteRgb(env_color, sp1E);
}

/* Set the particle's primitive RGB channels from the selected RGB palette entry, leaving alpha unchanged. */
void ParticleGfx_RecolorPrim(Particle* particle, u16 palette_index) {
    particle->primRed = sParticlePalette[palette_index][0];
    particle->primGreen = sParticlePalette[palette_index][1];
    particle->primBlue = sParticlePalette[palette_index][2];
}

/* Set the particle's environment RGB channels from the selected RGB palette entry, leaving alpha unchanged. */
void ParticleGfx_RecolorEnv(Particle* particle, u16 palette_index) {
    particle->envRed = sParticlePalette[palette_index][0];
    particle->envGreen = sParticlePalette[palette_index][1];
    particle->envBlue = sParticlePalette[palette_index][2];
}

/* Set the particle's primitive and environment RGB channels from the selected palette pair, leaving alpha unchanged. */
void ParticleGfx_RecolorPrimEnvFromPalettePair(Particle* particle, u16 palette_index) {
    u8 sp1C = sParticlePalettePairs[palette_index][0];
    u8 sp1E = sParticlePalettePairs[palette_index][1];

    ParticleGfx_RecolorPrim(particle, sp1C);
    ParticleGfx_RecolorEnv(particle, sp1E);
}

/* Set the particle's primitive RGB channels from the selected palette-set index, leaving alpha unchanged. */
void ParticleGfx_RecolorPrimFromPaletteSetA(Particle* particle, s16 palette_index) {
    ParticleGfx_RecolorPrim(particle, sParticlePaletteSetA[palette_index]);
}

/* Set the particle's environment RGB channels from the selected palette-set index, leaving alpha unchanged. */
void ParticleGfx_RecolorEnvFromPaletteSetA(Particle* particle, s16 palette_index) {
    ParticleGfx_RecolorEnv(particle, sParticlePaletteSetA[palette_index]);
}

/* Set the particle's primitive RGB channels from the selected palette-set index, leaving alpha unchanged. */
void ParticleGfx_RecolorPrimFromPaletteSetB(Particle* particle, s16 palette_index) {
    ParticleGfx_RecolorPrim(particle, sParticlePaletteSetB[palette_index]);
}

/* Set the particle's environment RGB channels from the selected palette-set index, leaving alpha unchanged. */
void ParticleGfx_RecolorEnvFromPaletteSetB(Particle* particle, s16 palette_index) {
    ParticleGfx_RecolorEnv(particle, sParticlePaletteSetB[palette_index]);
}

/* Set the particle's primitive and environment RGB channels from the selected palette-set index, leaving alpha unchanged. */
void ParticleGfx_RecolorPrimEnvFromPaletteSetA(Particle* particle, s16 palette_index) {
    u8 sp1F = sParticlePaletteSetA[palette_index];

    ParticleGfx_RecolorPrim(particle, sp1F);
    ParticleGfx_RecolorEnv(particle, sp1F);
}

/* Set the particle's primitive and environment RGB channels from the selected palette-set index, leaving alpha unchanged. */
void ParticleGfx_RecolorPrimEnvFromPaletteSetB(Particle* particle, s16 palette_index) {
    u8 sp1F = sParticlePaletteSetB[palette_index];

    ParticleGfx_RecolorPrim(particle, sp1F);
    ParticleGfx_RecolorEnv(particle, sp1F);
}

/* Set the particle's primitive RGB channels from a randomly selected palette index, leaving alpha unchanged. */
void ParticleGfx_RecolorPrimRandomPalette(Particle* particle) {
    ParticleGfx_RecolorPrim(particle, sParticleRandomPaletteIndices[(ParticleMath_RandomRange(5) & 0xFF)]);
}

/* Set the particle's environment RGB channels from a randomly selected palette index, leaving alpha unchanged. */
void ParticleGfx_RecolorEnvRandomPalette(Particle* particle) {
    ParticleGfx_RecolorEnv(particle, sParticleRandomPaletteIndices[ParticleMath_RandomRange(5) & 0xFF]);
}

/* Approach each selected RGB channel toward its palette value; return 1 only when all channel helpers report completion. */
s32 ParticleGfx_ApproachPrimPalette(Particle* particle, u16 palette_index, u8 step) {
    s32 sp2C = 0;
    s32 sp28;
    s32 sp24;
    s32 sp20;
    u8 sp1C[3];

    ParticleGfx_CopyPaletteRgb(sp1C, palette_index);

    sp28 = Particle_PrimR_Approach_0BAE4(particle, sp1C[0], step);
    sp24 = Particle_PrimG_Approach_0BB14(particle, sp1C[1], step);
    sp20 = Particle_PrimB_Approach_0BB44(particle, sp1C[2], step);

    if ((sp28 * sp24 * sp20) != 0) {
        sp2C = 1;
    }
    return sp2C;
}

/* Approach each selected RGB channel toward its palette value; return 1 only when all channel helpers report completion. */
s32 ParticleGfx_ApproachEnvPalette(Particle* particle, u16 palette_index, u8 step) {
    s32 sp2C = 0;
    s32 sp28;
    s32 sp24;
    s32 sp20;
    u8 sp1C[3];

    ParticleGfx_CopyPaletteRgb(&sp1C, palette_index);

    sp28 = Particle_EnvR_Approach_0BB90(particle, sp1C[0], step);
    sp24 = Particle_ApproachEnvGreen(particle, sp1C[1], step);
    sp20 = Particle_ApproachEnvBlue(particle, sp1C[2], step);

    if ((sp28 * sp24 * sp20) != 0) {
        sp2C = 1;
    }
    return sp2C;
}

/* Approach each selected RGB channel toward its palette value; return 1 only when all channel helpers report completion. */
s32 ParticleGfx_ApproachPrimEnvPalettePair(Particle* particle, u16 palette_index, u8 step) {
    s32 sp44 = 0;
    s32 sp40;
    s32 sp3C;
    s32 sp38;
    s32 sp34;
    s32 sp30;
    s32 sp2C;
    u8 sp28[3];
    u8 sp24[3];

    ParticleGfx_CopyPalettePairRgb(sp28, sp24, palette_index);

    sp40 = Particle_PrimR_Approach_0BAE4(particle, sp28[0], step);
    sp3C = Particle_PrimG_Approach_0BB14(particle, sp28[1], step);
    sp38 = Particle_PrimB_Approach_0BB44(particle, sp28[2], step);
    sp34 = Particle_EnvR_Approach_0BB90(particle, sp24[0], step);
    sp30 = Particle_ApproachEnvGreen(particle, sp24[1], step);
    sp2C = Particle_ApproachEnvBlue(particle, sp24[2], step);

    if ((sp40 * sp3C * sp38 * sp34 * sp30 * sp2C) != 0) {
        sp44 = 1;
    }
    return sp44;
}
