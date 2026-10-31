#include "battle_engine.h"

/*
moves covered by this file:
    - Fire Punch
    - Thunder punch
    - Scratch
    - razor wind
    - Cut
    - wing attack
    - vine whip
    - ember
    - flamethrower
    - Mist
    - ice beam
    - blizzard
    - psybeam
    - aurora beam
    - hyper beam
    - razor leaf
    - solarbeam
    - poison powder
    - stun spore
    - sleep powder
    - pedal dance (OH GOD THE LAG)
    - dragon rage
    - fire spin
    - thunder shock
    - thunderbolt
    - thunder
    - Dig
    - confusion
    - psychic
    - rage
    - night shade
    - smoke screen
    - confuse ray
    - Light screen
    - haze
    - reflect
    - self-destruct
    - egg bomb
    - smog
    - bone club
    - fire blast
    - poison gas
    - sky attack
    - spore
    - flash
    - crabhammer
    - explosion
    - fury swipes
    - tri attack
    - slash

    maybe bonemerang?

As well as:
    - The Zs that appear when a pokemon is sleeping
*/

s32 BattleAnim_DrawAsset95Ia8Billboard(Particle* arg0, CameraNode* arg1) {
    ParticleGfx_SetPrimEnvColorCombine(arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->envRed, arg0->envGreen, arg0->envBlue, arg0->primAlpha);
    ParticleGfx_PushBillboardMatrix(arg1, arg0->worldPosition, arg0->scale);
    
    gSPDisplayList(gDisplayListHead++, gDL_PanelBackground);
}

// better name: BattleAnim_DrawFlame
/*
    Draws a ball of fire
*/
void BattleAnim_DrawAsset76Ia8FrameWithAsset77I4Overlay(Particle* arg0, CameraNode* arg1) {
    ParticleGfx_LoadTextureIa8(sBattleAnimAssetPointers[0x76] + (arg0->textureFrame << 0xB), 0x20, 0x40);
    ParticleGfx_LoadTextureI4MultiBlockWrapSTWithShift(sBattleAnimAssetPointers[0x77], 0x40, 0x40, 2, 1);
    ParticleGfx_SetAnimatedAuxTileSize(0, 0xA, 0x40, 0x40);
    ParticleGfx_SetPrimEnvLodTextureCombine(0x80, arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->primAlpha, arg0->envRed, arg0->envGreen, arg0->envBlue);
    ParticleGfx_DrawModelQuad(arg1, arg0->worldPosition, arg0->scale, D_8140E1F8);
}


// Better name: BattleAnim_DrawFlameHit
/*
    Draw the flames when being *hit* by Flamethrower or dragon rage
*/
void BattleAnim_DrawAsset76Ia8FrameWithAsset77I4OverlayPipelineA(Particle* particle, CameraNode* camera) {
    gSPDisplayList(gDisplayListHead++, D_8140DCD8);

    BattleAnim_DrawAsset76Ia8FrameWithAsset77I4Overlay(particle, camera);
}

// Better name: BattleAnim_DrawFlameUse
/*
    Draw the flames when using Flamethrower, Dragon Rage, Fire Spin, Rage, Bide, Self-Destruct, Fire Blast, Sky Attack, or Explosion
*/
void BattleAnim_DrawAsset76Ia8FrameWithAsset77I4OverlayPipelineB(Particle* arg0, CameraNode* arg1) {
    gSPDisplayList(gDisplayListHead++, D_8140DD58);

    BattleAnim_DrawAsset76Ia8FrameWithAsset77I4Overlay(arg0, arg1);
}

void BattleAnim_DrawAsset1FAnimatedI4WithAsset20DisplayList(Particle* arg0, CameraNode* arg1) {
    ParticleGfx_PushBillboardMatrix(arg1, arg0->worldPosition, arg0->scale);

    gSPDisplayList(gDisplayListHead++, D_8140DD58);

    ParticleGfx_LoadTextureI4WrapSTNoMirror(sBattleAnimAssetPointers[0x1F], 0x20, 0x40);
    ParticleGfx_SetAnimatedTileSize(0, 8, 0x20, 0x40);
    ParticleGfx_SetPrimEnvColorShadeCombine(0xFF, 0xFF, 0xFF, arg0->primAlpha, 0xFF, 0xFF, 0xFF, 0xFF);

    gDPSetRenderMode(gDisplayListHead++, G_RM_PASS, G_RM_AA_ZB_XLU_SURF2);
    gSPDisplayList(gDisplayListHead++, sBattleAnimAssetPointers[0x20]);
}

// Better name: BattleAnim_DrawDigRocks
/*
    Raws the rocks that spawn out of the tunnel when pokemon other than diglett and dugtrio use dig
*/
s32 BattleAnim_DrawAssetBcRgba16WithAssetBdDisplayList(Particle* particle, UNUSED CameraNode* arg1) {
    ParticleGfx_PushTransformMatrix(particle->worldPosition, particle->rotation, particle->scale);

    gSPDisplayList(gDisplayListHead++, gDL_ItemIcon);

    ParticleGfx_LoadTextureRgba16(sBattleAnimAssetPointers[0xBC], 0x20, 0x20);

    gSPDisplayList(gDisplayListHead++, sBattleAnimAssetPointers[0xBD]);
}

// better name: BattleAnim_DrawSporePuffs
/*
    Draws spore puffs for powder moves 
*/
void BattleAnim_DrawAssets27And28LayeredI4QuadVariantA(Particle* arg0, UNUSED CameraNode* arg1) {
    ParticleGfx_Push2DTransform(arg0->worldPosition.x, arg0->worldPosition.y, arg0->rotation.z, arg0->scale);

    gSPDisplayList(gDisplayListHead++, D_8140DCD8);

    ParticleGfx_SetPrimEnvLodCombine(0x80, arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->primAlpha, arg0->envRed, arg0->envGreen, arg0->envBlue);
    ParticleGfx_LoadTextureI4WrapSTNoMirror(sBattleAnimAssetPointers[0x27], 0x40, 0x40);
    ParticleGfx_SetAnimatedTileSize(2, -2, 0x40, 0x40);
    ParticleGfx_LoadTextureI4MultiBlockWrapST(sBattleAnimAssetPointers[0x28], 0x40, 0x40);
    ParticleGfx_SetAnimatedAuxTileSize(0, -1, 0x40, 0x40);
    ParticleGfx_DrawQuadVertices(D_84385DC0);
}

// better name: BattleAnim_DrawSporePuffsFast
/*
    Draws faster spore puffs for powder moves 
*/
void BattleAnim_DrawAssets27And28LayeredI4QuadVariantB(Particle* arg0, UNUSED CameraNode* arg1) {
    ParticleGfx_Push2DTransform(arg0->worldPosition.x, arg0->worldPosition.y, arg0->rotation.z, arg0->scale);

    gSPDisplayList(gDisplayListHead++, D_8140DCD8);

    ParticleGfx_SetPrimEnvLodCombine(0x80, arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->primAlpha, arg0->envRed, arg0->envGreen, arg0->envBlue);
    ParticleGfx_LoadTextureI4WrapSTNoMirror(sBattleAnimAssetPointers[0x27], 0x40, 0x40);
    ParticleGfx_SetAnimatedTileSize(1, -3, 0x40, 0x40);
    ParticleGfx_LoadTextureI4MultiBlockWrapST(sBattleAnimAssetPointers[0x28], 0x40, 0x40);
    ParticleGfx_SetAnimatedAuxTileSize(-2, -2, 0x40, 0x40);
    ParticleGfx_DrawQuadVertices(D_84385E00);
}

// better name: BattleAnim_DrawMistLightest
/*
    Draw the lightest mist for mist/smoke moves (Mist, Smokescreen, Haze, Smog, Poison Gas)
*/
void BattleAnim_DrawAsset29DualI4LayeredQuad(Particle* arg0, UNUSED CameraNode* arg1) {
    ParticleGfx_Push2DTransform(arg0->worldPosition.x, arg0->worldPosition.y, arg0->rotation.z, arg0->scale);

    gSPDisplayList(gDisplayListHead++, D_8140DCD8);

    ParticleGfx_SetPrimEnvLodCombine(0x80, arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->primAlpha, arg0->envRed, arg0->envGreen, arg0->envBlue);
    ParticleGfx_LoadTextureMultiBlock4(sBattleAnimAssetPointers[0x29], 0, 0, 4, 0x20, 0x20, 1, 1, 5, 5, 2, 0);
    ParticleGfx_SetAnimatedTileSize(1, 1, 0x20, 0x20);
    ParticleGfx_LoadTextureMultiBlock4(sBattleAnimAssetPointers[0x29], 0x100, 1, 4, 0x20, 0x20, 1, 1, 5, 5, 1, 0xF);
    ParticleGfx_SetAnimatedAuxTileSize(-1, 1, 0x20, 0x20);
    ParticleGfx_DrawQuadVertices(gFullscreenQuadVtx);
}

void BattleAnim_DrawAsset87AnimatedI4Quad(Particle* arg0, UNUSED CameraNode* arg1) {
    ParticleGfx_Push2DTransform(arg0->worldPosition.x, arg0->worldPosition.y, arg0->rotation.z, arg0->scale);

    gSPDisplayList(gDisplayListHead++, D_8140DCD8);

    ParticleGfx_SetPrimEnvColorCombine(arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->envRed, arg0->envGreen, arg0->envBlue, arg0->primAlpha);
    ParticleGfx_LoadTextureI4WrapSTNoMirror(sBattleAnimAssetPointers[0x87], 0x20, 0x20);
    ParticleGfx_SetAnimatedTileSize(0, 2, 0x20, 0x20);
    ParticleGfx_DrawQuadVertices(gFullscreenQuadVtx);
}

// Better name: BattleAnim_DrawFullScreenSlash
/*
    Draws the triple effect for Scratch, Cut, Vine Whip, Crabhammer, Fury Swipes, and Slash. Called from 
    BattleAnim_DrawAsset19I4AtlasYScaled or BattleAnim_DrawAsset5EI4AtlasYScaled depending on the move
*/
void BattleAnim_DrawI4AtlasYScaled(Particle* arg0, u8* arg1) {
    ParticleGfx_Push2DTransformYScaled(arg0->worldPosition.x, arg0->worldPosition.y, arg0->rotation.z, arg0->scale);

    gSPDisplayList(gDisplayListHead++, D_8140DCD8);

    ParticleGfx_SetPrimEnvColorCombine(arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->envRed, arg0->envGreen, arg0->envBlue, arg0->primAlpha);
    ParticleGfx_DrawI4TextureAtlas(arg1, D_84385C80, 0x40, 0x40, 0x40, 0x40);
}

// Better name: BattleAnim_DrawFullScreenTripleSlash
/*
    Draws the triple slash effect for scratch, fury swipes, and slash. Goes \\\ on use, and /// on hit
*/
void BattleAnim_DrawAsset19I4AtlasYScaled(Particle* particle, UNUSED CameraNode* arg1) {
    BattleAnim_DrawI4AtlasYScaled(particle, sBattleAnimAssetPointers[0x19]);
}


// Better name: BattleAnim_DrawFullScreenSingleSlash
/*
    Draws the Single slash effect for Cut, Vine Whip, and Crabhammer. Goes \ on use, and / on hit
*/
void BattleAnim_DrawAsset5EI4AtlasYScaled(Particle* arg0, UNUSED CameraNode* arg1) {
    BattleAnim_DrawI4AtlasYScaled(arg0, sBattleAnimAssetPointers[0x5E]);
}


// Better name: BattleAnim_DrawSlashingEffect
/*
    Draws slashes effects used at the start of Scratch,  Cut, Vine Whip, Fury Swipes, Slash being used
    As well as the effects of Razor Wind,Razor Leaf, and Petal Dance upon hit
*/
void BattleAnim_DrawAsset1AFrameI4Quad(Particle* arg0, UNUSED CameraNode* arg1) {
    gSPDisplayList(gDisplayListHead++, D_8140DD58);
    gSPClearGeometryMode(gDisplayListHead++, G_CULL_BACK);

    ParticleGfx_LoadTextureI4Alias(sBattleAnimAssetPointers[0x1A] + (arg0->textureFrame << 9), 0x20, 0x20);
    ParticleGfx_SetPrimEnvColorCombine(arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->envRed, arg0->envGreen, arg0->envBlue, arg0->primAlpha);
    ParticleGfx_DrawTransformedQuad(arg0->worldPosition, arg0->rotation, arg0->scale, D_8140E018);

    gSPSetGeometryMode(gDisplayListHead++, G_CULL_BACK);
}

// better name: BattleAnim_DrawVineWhipLeaves
/*
    Draws the leaves that appear when hit by vine whip
*/
void BattleAnim_DrawAsset1CRgba16Quad(Particle* particle, UNUSED CameraNode* arg1) {
    gSPDisplayList(gDisplayListHead++, D_8140DC00);
    gSPClearGeometryMode(gDisplayListHead++, G_CULL_BACK);

    ParticleGfx_LoadTextureRgba16(sBattleAnimAssetPointers[0x1C], 0x20, 0x20);
    ParticleGfx_DrawTransformedQuad(particle->worldPosition, particle->rotation, particle->scale, D_8140E018);

    gSPSetGeometryMode(gDisplayListHead++, G_CULL_BACK);
}

// better name: BattleAnim_DrawPetalDanceSakura
/*
    Draws the sakura flower pedals that appear when hit by petal dance
*/
void BattleAnim_DrawAsset1ERgba16Quad(Particle* arg0, UNUSED CameraNode* arg1) {
    gSPDisplayList(gDisplayListHead++, D_8140DC00);
    gSPClearGeometryMode(gDisplayListHead++, G_CULL_BACK);

    ParticleGfx_LoadTextureRgba16(sBattleAnimAssetPointers[0x1E], 0x20, 0x20);
    ParticleGfx_DrawTransformedQuad(arg0->worldPosition, arg0->rotation, arg0->scale, D_8140E018);

    gSPSetGeometryMode(gDisplayListHead++, G_CULL_BACK);
}

// better name: BattleAnim_DrawAssetWingAttackFeathers
/*
    Draws the feathers that appear when hit by wing attack
*/
void BattleAnim_DrawAssetC3Rgba16Quad(Particle* particle, UNUSED CameraNode* arg1) {
    gSPDisplayList(gDisplayListHead++, D_8140DC00);
    gSPClearGeometryMode(gDisplayListHead++, G_CULL_BACK);

    ParticleGfx_LoadTextureRgba16(sBattleAnimAssetPointers[0xC3], 0x20, 0x20);
    ParticleGfx_DrawTransformedQuad(particle->worldPosition, particle->rotation, particle->scale, D_8140E018);

    gSPSetGeometryMode(gDisplayListHead++, G_CULL_BACK);
}

void BattleAnim_DrawAsset1DRotatedI4Quad(Particle* arg0, UNUSED CameraNode* arg1) {
    Vec3s sp40;

    gSPDisplayList(gDisplayListHead++, D_8140DCD8);
    gSPClearGeometryMode(gDisplayListHead++, G_CULL_BACK);

    ParticleGfx_LoadTextureI4Alias(sBattleAnimAssetPointers[0x1D], 0x20, 0x20);
    ParticleGfx_SetPrimEnvColorCombine(arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->envRed, arg0->envGreen, arg0->envBlue, arg0->primAlpha);
    sp40.x = arg0->rotation.x + 0x4000;
    sp40.y = arg0->rotation.y + 0x4000;
    sp40.z = arg0->rotation.z;
    ParticleGfx_DrawTransformedQuad(arg0->worldPosition, sp40, arg0->scale, D_8140E018);

    gSPSetGeometryMode(gDisplayListHead++, G_CULL_BACK);
}

void BattleAnim_DrawParticleNoop(UNUSED Particle* arg0, UNUSED CameraNode* arg1) {
}

// Better name: BattleAnim_DrawHyperBeamImpactThinBolts
/*
    Draws the thin impact bolts when being hit by Hyper beam
    Also draws particular bolts during thunder shock, thunderbolt, and thunder
*/ 
void BattleAnim_DrawAsset13FrameI4YScaledQuadA(Particle* arg0, UNUSED CameraNode* arg1) {
    gSPDisplayList(gDisplayListHead++, D_8140DD58);

    ParticleGfx_LoadTextureI4Alias(sBattleAnimAssetPointers[0x13] + (sParticleFrameMod8 * 0x600), 0x20, 0x60);
    ParticleGfx_SetPrimEnvColorCombine(arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->envRed, arg0->envGreen, arg0->envBlue, arg0->primAlpha);

    gSPClearGeometryMode(gDisplayListHead++, G_CULL_BACK);

    ParticleGfx_DrawTransformedQuadYScaled(arg0->worldPosition, arg0->rotation, arg0->scale, D_84385CC0);

    gSPSetGeometryMode(gDisplayListHead++, G_CULL_BACK);
}

void BattleAnim_DrawAsset13FrameI4YScaledQuadB(Particle* arg0, UNUSED CameraNode* arg1) {
    gSPDisplayList(gDisplayListHead++, D_8140DD58);
    gSPClearGeometryMode(gDisplayListHead++, G_CULL_BACK);

    ParticleGfx_LoadTextureI4Alias(sBattleAnimAssetPointers[0x13] + (sParticleFrameMod8 * 0x600), 0x20, 0x60);
    ParticleGfx_SetPrimEnvColorCombine(arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->envRed, arg0->envGreen, arg0->envBlue, arg0->primAlpha);
    ParticleGfx_DrawTransformedQuadYScaled(arg0->worldPosition, arg0->rotation, arg0->scale, D_84385D00);

    gSPSetGeometryMode(gDisplayListHead++, G_CULL_BACK);
}

// Better name: BattleAnim_DrawBoltOfLightening
/*
    Draws Bolts for thunder punch, thunder shock, thunder, and thunderbolt
*/
void BattleAnim_DrawAsset13FrameI4YScaledQuadC(Particle* particle, UNUSED CameraNode* arg1) {
    gSPDisplayList(gDisplayListHead++, D_8140DD58);
    gSPClearGeometryMode(gDisplayListHead++, G_CULL_BACK);

    ParticleGfx_LoadTextureI4Alias(sBattleAnimAssetPointers[0x13] + (sParticleFrameMod8 * 0x600), 0x20, 0x60);
    ParticleGfx_SetPrimEnvColorCombine(particle->primRed, particle->primGreen, particle->primBlue, particle->envRed, particle->envGreen, particle->envBlue, particle->primAlpha);
    ParticleGfx_DrawTransformedQuadYScaled(particle->worldPosition, particle->rotation, particle->scale, D_84385D40);

    gSPSetGeometryMode(gDisplayListHead++, G_CULL_BACK);
}

// better name: BattleAnim_DrawLongBoltOfLightening
/*
    Spawns the long bolts of lighting for Thunder shock, thunderbolt, and thunder
*/
void BattleAnim_DrawAsset13FrameI4ModelQuadA(Particle* arg0, CameraNode* arg1) {
    gSPDisplayList(gDisplayListHead++, D_8140DD58);
    gSPClearGeometryMode(gDisplayListHead++, G_CULL_BACK);

    ParticleGfx_LoadTextureI4Alias(sBattleAnimAssetPointers[0x13] + (sParticleFrameMod8 * 0x600), 0x20, 0x60);
    ParticleGfx_SetPrimEnvColorCombine(arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->envRed, arg0->envGreen, arg0->envBlue, arg0->primAlpha);
    ParticleGfx_DrawModelQuadRotatedYScaled(arg1, arg0->worldPosition, arg0->scale, D_84385CC0, arg0->rotation.z);

    gSPSetGeometryMode(gDisplayListHead++, G_CULL_BACK);
}
// Better name: BattleAnim_DrawHyperBeamImpactThinkBolts
/*
    Draws the think impact bolts when being hit by Hyper beam
    Also draws particular bolts during thunder shock, thunderbolt, and thunder
*/ 
void BattleAnim_DrawAsset13FrameI4ModelQuadB(Particle* arg0, CameraNode* arg1) {
    gSPDisplayList(gDisplayListHead++, D_8140DD58);
    gSPClearGeometryMode(gDisplayListHead++, G_CULL_BACK);

    ParticleGfx_LoadTextureI4Alias(sBattleAnimAssetPointers[0x13] + (sParticleFrameMod8 * 0x600), 0x20, 0x60);
    ParticleGfx_SetPrimEnvColorCombine(arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->envRed, arg0->envGreen, arg0->envBlue, arg0->primAlpha);
    ParticleGfx_DrawModelQuadRotatedYScaled(arg1, arg0->worldPosition, arg0->scale, D_84385D00, arg0->rotation.z);

    gSPSetGeometryMode(gDisplayListHead++, G_CULL_BACK);
}

// better name: BattleAnim_DrawThunderShockUnknown
/*
     draws... something from using thunder shock. it's not the jolts
*/
void BattleAnim_DrawAsset13FrameI4ModelQuadC(Particle* particle, CameraNode* camera) {
    gSPDisplayList(gDisplayListHead++, D_8140DD58);
    gSPClearGeometryMode(gDisplayListHead++, G_CULL_BACK);

    ParticleGfx_LoadTextureI4Alias(sBattleAnimAssetPointers[0x13] + (sParticleFrameMod8 * 0x600), 0x20, 0x60);
    ParticleGfx_SetPrimEnvColorCombine(particle->primRed, particle->primGreen, particle->primBlue, particle->envRed, particle->envGreen, particle->envBlue, particle->primAlpha);
    ParticleGfx_DrawModelQuadRotatedYScaled(camera, particle->worldPosition, particle->scale, D_84385D40, particle->rotation.z);

    gSPSetGeometryMode(gDisplayListHead++, G_CULL_BACK);
}

void BattleAnim_DrawAsset1BI4TriangleA(Particle* arg0, UNUSED CameraNode* arg1) {
    gSPDisplayList(gDisplayListHead++, D_8140DCD8);
    gSPClearGeometryMode(gDisplayListHead++, G_CULL_BACK);

    ParticleGfx_LoadTextureI4Alias(sBattleAnimAssetPointers[0x1B], 0x10, 0x100);
    ParticleGfx_SetPrimEnvColorCombine(0xFF, 0xFF, 0xFF, arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->primAlpha);
    ParticleGfx_DrawTransformedTriangle(arg0->worldPosition, arg0->rotation, arg0->scale, gVtx_UI_TriangleSmall);

    gSPSetGeometryMode(gDisplayListHead++, G_CULL_BACK);
}

void BattleAnim_DrawAsset1BI4TriangleB(Particle* arg0, UNUSED CameraNode* arg1) {
    gSPDisplayList(gDisplayListHead++, D_8140DCD8);
    gSPClearGeometryMode(gDisplayListHead++, G_CULL_BACK);

    ParticleGfx_LoadTextureI4Alias(sBattleAnimAssetPointers[0x1B], 0x10, 0x100);
    ParticleGfx_SetPrimEnvColorCombine(0xFF, 0xFF, 0xFF, arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->primAlpha);
    ParticleGfx_DrawTransformedTriangle(arg0->worldPosition, arg0->rotation, arg0->scale, gVtx_UI_TriangleLarge);

    gSPSetGeometryMode(gDisplayListHead++, G_CULL_BACK);
}

void BattleAnim_DrawAsset13FrameDiv2I4Quad(Particle* arg0, UNUSED CameraNode* arg1) {
    gSPDisplayList(gDisplayListHead++, D_8140DCD8);
    gSPClearGeometryMode(gDisplayListHead++, G_CULL_BACK);

    ParticleGfx_LoadTextureI4Alias(sBattleAnimAssetPointers[0x13] + ((sParticleFrameDiv2 & 7) * 0x600), 0x20, 0x60);
    ParticleGfx_SetPrimEnvColorCombine(0xFF, 0xFF, 0xFF, 0, 0x64, 0xC8, 0xFF);
    ParticleGfx_DrawTransformedQuad(arg0->worldPosition, arg0->rotation, arg0->scale, gVtx_UI_ComplexQuad);

    gSPSetGeometryMode(gDisplayListHead++, G_CULL_BACK);
}

void BattleAnim_DrawAsset18FrameI4ModelQuad(Particle* arg0, CameraNode* arg1) {
    gSPDisplayList(gDisplayListHead++, D_8140DD58);

    ParticleGfx_LoadTextureI4Alias(sBattleAnimAssetPointers[0x18] + (arg0->textureFrame << 9), 0x20, 0x20);
    ParticleGfx_SetPrimEnvColorCombine(arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->envRed, arg0->envGreen, arg0->envBlue, arg0->primAlpha);
    ParticleGfx_DrawModelQuadRotated(arg1, arg0->worldPosition, arg0->scale, D_8140E018, arg0->rotation.z);
}

void BattleAnim_DrawAssetA9I4XYScaledQuad(Particle* arg0, UNUSED CameraNode* arg1) {
    gSPDisplayList(gDisplayListHead++, D_8140DD58);
    gSPClearGeometryMode(gDisplayListHead++, G_CULL_BACK);

    ParticleGfx_LoadTextureI4Alias(sBattleAnimAssetPointers[0xA9], 0x20, 0x20);
    ParticleGfx_SetPrimEnvColorCombine(arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->envRed, arg0->envGreen, arg0->envBlue, arg0->primAlpha);
    ParticleGfx_DrawTransformedQuadXYScaled(arg0->worldPosition, arg0->rotation, arg0->scale, arg0->unk_20, gVtx_UI_RectVertical);

    gSPSetGeometryMode(gDisplayListHead++, G_CULL_BACK);
}

void BattleAnim_DrawParticleFullScreenColorOverlay(Particle* arg0, UNUSED CameraNode* arg1) {
    BattleAnim_DrawFullScreenColorOverlay(arg0->primRed, arg0->primGreen, arg0->primBlue, arg0->primAlpha);
}
