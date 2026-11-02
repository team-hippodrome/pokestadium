#ifndef _FRAGMENT34_H_
#define _FRAGMENT34_H_

#include "global.h"

/*
 * Fragment 34: particle math, pool state, frame animation and graphics callbacks.
 * Names describe source dataflow, not recovered originals or observed visuals.
 * See //@docs/particle_math.md for the producer/consumer evidence and unknowns.
 */
struct Particle;
struct ParticleDescriptorChild;
struct ActivePokemon;
struct BattleAnimEffectSlot;

typedef void (*ParticleUpdateCallback)(struct Particle*);
typedef void (*ParticleRenderCallback)(struct Particle*, CameraNode*);
typedef void (*ParticleSetupCallback)(struct Particle*, s32, struct ParticleDescriptorChild*);
typedef void (*ParticleDrawCallback)(struct Particle*, s32, struct ParticleDescriptorChild*);

typedef struct ParticleSetupArgumentPointer {
    /* 0x00 */ s32 arg;
} ParticleSetupArgumentPointer;

typedef union ParticleSetupArgument {
    s32 arg;
    ParticleSetupArgumentPointer* s;
} ParticleSetupArgument;

typedef struct ParticleDescriptorChild {
    /* 0x00 */ u8 setupScope; // 0: draw stage; 1: prepare stage (once per world chain)
    /* 0x01 */ u8 argumentSource; // 0: inline argument; 1: dereference setupArgument.s
    /* 0x02 */ s16 renderMode; // Particle_SetRenderMode selector
    /* 0x04 */ ParticleSetupCallback setupCallback;
    /* 0x08 */ ParticleDrawCallback drawCallback;
    /* 0x0C */ ParticleSetupArgument setupArgument;
} ParticleDescriptorChild; // size = 0x10

typedef union ParticleDescriptorPayload {
    ParticleDescriptorChild* a;
    ParticleRenderCallback func;
} ParticleDescriptorPayload;

typedef struct ParticleDescriptor {
    /* 0x00 */ s32 kind; // payload: 1 = setup/draw descriptor; 3 = direct render callback
    /* 0x04 */ ParticleDescriptorPayload payload;
} ParticleDescriptor; // size = 0x08

typedef struct ParticleEmitter {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ Vec3f origin;
    /* 0x10 */ Vec3s rotation;
    /* 0x18 */ ParticleUpdateCallback updateCallback;
    /* 0x1C */ ParticleDescriptor* descriptor;
    /* 0x20 */ s32 unk_20;
    /* 0x24 */ u16 unk_24;
    /* 0x26 */ s16 unk_26;
    /* 0x28 */ s16 unk_28;
    /* 0x2A */ s8 unk_2A;
    /* 0x2B */ s8 unk_2B;
    /* 0x2C */ u8 unk_2C;
    /* 0x2D */ u8 unk_2D;
    /* 0x2E */ u8 unk_2E;
    /* 0x2F */ u8 unk_2F;
} ParticleEmitter; // size = 0x30

/*
 * World position is origin + positionOffset + motionOffset.
 * Allocated slots, lifecycle, phase, and texture animation are separate state.
 * Particle_UpdateWorldTransform copies the composed transform to displayObject.
 */
typedef struct Particle {
    /* 0x00 */ struct Particle* next;
    /* 0x04 */ struct Particle* prev;
    /* 0x08 */ ParticleUpdateCallback updateCallback;
    /* 0x0C */ ParticleDescriptor* descriptor;
    /* 0x10 */ struct ActivePokemon* pokemon;
    /* 0x14 */ DisplayObject* displayObject;
    /* 0x18 */ u32 flags;
    /* 0x1C */ f32 scale;
    /* 0x20 */ f32 unk_20; // target in Particle_ApproachScaleUsingFields20And24
    /* 0x24 */ f32 unk_24; // step in that helper; other effect-specific use is not renamed
    /* 0x28 */ f32 unk_28; // copied from creation/emitter parameter, not the current scale
    /* 0x2C */ Vec3f worldPosition;
    /* 0x38 */ Vec3f positionOffset;
    /* 0x44 */ char unk44[0xC];
    /* 0x50 */ Vec3f motionOffset;
    /* 0x5C */ Vec3f motionTarget;
    /* 0x68 */ Vec3f origin;
    /* 0x74 */ f32 stepMagnitude; // signed motion scalar; also used by downward-motion helpers
    /* 0x78 */ f32 magnitudeStep; // step for approaching stepMagnitude
    /* 0x7C */ Vec3f motionStep;
    /* 0x88 */ char unk88[0xC];
    /* 0x94 */ Vec3s rotation;
    /* 0x9A */ Vec3s rotationStep; // also reused as an evolving orbital angle
    /* 0xA0 */ Vec3s rotationTarget;
    /* 0xA6 */ s16 unk_A6;
    /* 0xA8 */ s16 unk_A8;
    /* 0xAA */ s16 unk_AA;
    /* 0xAC */ s16 unk_AC;
    /* 0xAE */ s16 unk_AE;
    /* 0xB0 */ s16 unk_B0;
    /* 0xB2 */ s16 lifecycle; // 0: stopped; 1: initialized; >=2: eligible for drawing
    /* 0xB4 */ char unkB4[0x2];
    /* 0xB6 */ s16 frameTimer;
    /* 0xB8 */ s16 phase;
    /* 0xBA */ s16 phaseTimer;
    /* 0xBC */ s16 delayTimer;
    /* 0xBE */ u8 primRed;
    /* 0xBF */ u8 primGreen;
    /* 0xC0 */ u8 primBlue;
    /* 0xC1 */ u8 envRed;
    /* 0xC2 */ u8 envGreen;
    /* 0xC3 */ u8 envBlue;
    /* 0xC4 */ u8 primAlpha;
    /* 0xC5 */ char unkC5[0x1];
    /* 0xC6 */ s8 loopsRemaining; // negative counts are not decremented by frame animation
    /* 0xC7 */ s8 textureFrame;
    /* 0xC8 */ char unkC8[0x2];
    /* 0xCA */ u8 unk_CA;
    /* 0xCB */ u8 unk_CB;
    /* 0xCC */ u8 unk_CC;
    /* 0xCD */ u8 unk_CD;
    /* 0xCE */ u8 unk_CE;
    /* 0xCF */ u8 unk_CF;
    /* 0xD0 */ s8 allocated; // independent of lifecycle; Particle_Free releases the slot
} Particle; // size = 0xD4

typedef void (*func_unk_D_8140E724)(struct Particle*);

typedef struct BattleAnimEffectSlot {
    /* 0x00 */ func_unk_D_8140E724 unk_00;
    /* 0x04 */ ParticleDescriptor* unk_04;
    /* 0x08 */ struct ActivePokemon* unk_08;
    /* 0x0C */ s16 unk_0C;
    /* 0x0E */ s16 unk_0E;
    /* 0x10 */ s16 unk_10;
    /* 0x12 */ s16 unk_12;
    /* 0x14 */ s8 unk_14;
    /* 0x15 */ s8 unk_15;
    /* 0x16 */ u8 unk_16;
    /* 0x17 */ u8 unk_17;
    /* 0x18 */ u8 unk_18;
    /* 0x19 */ u8 unk_19;
    /* 0x1A */ u8 unk_1A;
    /* 0x1B */ u8 unk_1B;
} BattleAnimEffectSlot; // size = 0x1C

typedef struct arg1_func_87903D64_014 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ unk_D_86002F34_000_014_004* unk_04;
    /* 0x08 */ unk_D_86002F34_000_014_004* unk_08;
    /* 0x0C */ unk_D_86002F34_000_014_004* unk_0C;
    /* 0x10 */ unk_D_86002F34_000_014_010* unk_10;
    /* 0x14 */ unk_D_86002F34_000_014_010* unk_14;
    /* 0x18 */ unk_D_86002F34_000_014_010* unk_18;
} arg1_func_87903D64_014; // size = 0x1C

// Possibly unk_D_86002F34_alt7, unk_D_86002F34_alt8,
// unk_D_86002F34_alt9 or unk_D_86002F34_alt2
typedef struct arg1_func_87903D64 {
    /* 0x00 */ char unk00[0x14];
    /* 0x14 */ arg1_func_87903D64_014* unk_14;
    /* 0x18 */ Gfx* unk_18;
} arg1_func_87903D64; // size >= 0x1C

typedef struct ParticlePrimColorFrame {
    /* 0x00 */ Color_RGBA8 color;
    /* 0x04 */ u8 l;
} ParticlePrimColorFrame; // size = 0x5

typedef struct arg1_func_81407874_014_000_018_000_000 {
    /* 0x00 */ Color_RGBA8 color;
} arg1_func_81407874_014_000_018_000_000; // size = 0x4

typedef union ParticleMaterialValue {
    ParticlePrimColorFrame* ptr5;
    arg1_func_81407874_014_000_018_000_000* ptr4;
    Color_RGBA8_u32 color;
    u8** texture;
} ParticleMaterialValue; // size = 0x4

typedef struct ParticleValueSequence {
    /* 0x00 */ ParticleMaterialValue unk_00; // constant color or frame-indexed value storage
    /* 0x04 */ s16 unk_04; // zero selects constant color; otherwise the sequence length
    /* 0x06 */ s16 unk_06; // clock selector consumed by ParticleGfx_GetFrameDivisor
} ParticleValueSequence; // size >= 0x08

typedef struct arg1_func_81407874_014_000_01C {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ u16 unk_02;
} arg1_func_81407874_014_000_01C; // size >= 0x04

typedef struct ParticleMaterialState {
    /* 0x00 */ struct ParticleMaterialState* unk_00;
    /* 0x04 */ s32 unk_04; // texture generation: 0 off, 1 generated, 2 generated + linear
    /* 0x08 */ char unk08[0x8];
    /* 0x10 */ ParticleCombineMux* unk_10; // optional two-cycle combiner mux
    /* 0x14 */ ParticleValueSequence* unk_14; // optional primitive color/LOD sequence
    /* 0x18 */ ParticleValueSequence* unk_18; // optional environment color sequence
    /* 0x1C */ arg1_func_81407874_014_000_01C* unk_1C;
} ParticleMaterialState; // size >= 0x20

typedef struct ParticleTileScroll {
    /* 0x00 */ s16 startS;
    /* 0x02 */ s16 startT;
    /* 0x04 */ s16 stepS;
    /* 0x06 */ s16 stepT;
    /* 0x08 */ s16 clockDivisor;
} ParticleTileScroll; // size >= 0xA

typedef struct ParticleTextureFormat {
    /* 0x00 */ s16 format;
    /* 0x02 */ s16 texelSize;
    /* 0x04 */ s16 width;
    /* 0x06 */ s16 height;
    /* 0x08 */ s16 addressModeS;
    /* 0x0A */ s16 addressModeT;
    /* 0x0C */ s16 maskS;
    /* 0x0E */ s16 maskT;
    /* 0x10 */ s16 shiftS;
    /* 0x12 */ s16 shiftT;
} ParticleTextureFormat; // size >= 0x14

/* The loaders interpret the first member as image data; legacy pointer types are retained. */
typedef struct ParticleTexture {
    /* 0x00 */ ParticleMaterialState* unk_00; // static image pointer through the legacy type
    /* 0x04 */ ParticleTextureFormat* unk_04; // texture upload format and addressing
    /* 0x08 */ ParticleTileScroll* unk_08; // optional tile scrolling
    /* 0x0C */ ParticleValueSequence* unk_0C; // optional divided-clock image sequence
} ParticleTexture; // size = 0x10

typedef struct ParticleMaterial {
    /* 0x00 */ ParticleMaterialState* unk_00; // also passed to the primary texture reader via the legacy layout
    /* 0x04 */ ParticleTexture* unk_04; // optional secondary texture
} ParticleMaterial; // size >= 0x8

typedef struct arg1_func_81407874 {
    /* 0x00 */ char unk00[0x14];
    /* 0x00 */ ParticleMaterial* unk_14;
} arg1_func_81407874; // size >= 0x18

typedef struct ParticleCameraState {
    /* 0x00 */ MtxF viewMatrix;
    /* 0x40 */ Vec3f eyePosition;
    /* 0x4C */ Vec3f targetPosition;
    /* 0x58 */ Vec3f lookDirection; // normalized eye-to-target direction
    /* 0x64 */ Vec3f eyeDirection; // normalized target-to-eye direction
    /* 0x70 */ s16 unk_70;
    /* 0x72 */ s16 unk_72;
    /* 0x74 */ char unk74[0x2];
    /* 0x76 */ s16 unk_76;
    /* 0x78 */ s16 unk_78;
} ParticleCameraState; // size = 0x7C

typedef struct ParticleRenderContext {
    /* 0x00 */ Vec3f scale; // offset and motion helpers use Y for all three axes
    /* 0x0C */ CameraNode* camera;
    /* 0x10 */ ParticleCameraState cameraState;
} ParticleRenderContext; // size = 0x8C

typedef struct ParticleMatrixPool {
    /* 0x0000 */ MtxF unk_0000[364]; // fixed-point storage despite the legacy MtxF declaration
    /* 0x5B00 */ char unk5B00[0x80];
    /* 0x5B80 */ Mtx unk_5B80;
    /* 0x5BC0 */ char unk5BC0[0x20];
} ParticleMatrixPool; // size = 0x5BE0

extern Gfx D_8140DAE0[];
extern Gfx D_8140DB30[];
extern Gfx gDL_ItemIcon[];
extern Gfx D_8140DC00[];
extern Gfx D_8140DC58[];
extern Gfx D_8140DC78[];
extern Gfx D_8140DC98[];
extern Gfx D_8140DCB8[];
extern Gfx D_8140DCD8[];
extern Gfx D_8140DCF8[];
extern Gfx D_8140DD18[];
extern Gfx D_8140DD38[];
extern Gfx D_8140DD58[];
extern Gfx gDL_BeginUIOverlay[];
extern Gfx gDL_EndColorFill[];
extern Gfx gDL_BeginColorFill[];
extern Vtx D_8140DE78[];
extern Vtx D_8140DEF8[];
extern Vtx gVtx_UI_TriangleSmall[];
extern Vtx gVtx_UI_TriangleLarge[];
extern Vtx D_8140DFD8[];
extern Vtx D_8140E018[];
extern Vtx D_8140E058[];
extern Vtx gVtx_UI_RectVertical[];
extern Vtx D_8140E0D8[];
extern Vtx D_8140E108[];
extern Vtx D_8140E148[];
extern Vtx D_8140E178[];
extern Vtx D_8140E1B8[];
extern Vtx D_8140E1F8[];
extern Vtx gVtx_UI_ComplexQuad[];
extern Vtx D_8140E2B8[];
extern Vtx D_8140E378[];
extern Vtx gFullscreenQuadVtx[];
extern Gfx gDL_PanelBackground[];
extern Gfx D_8140E430[];
extern Gfx D_8140E448[];
extern ParticleDescriptor sDefaultParticleDescriptor;

extern u8 sParticlePaletteSetA[];

extern ParticleMatrixPool* sParticleMatrixPool;
extern s32 sParticleMatrixPoolIndex;
extern s32 sParticleMatrixPoolIndex;
extern ParticleRenderContext sParticleRenderContext;
extern ParticleCameraState* sParticleCameraState;
extern u32 sParticleFrameCounter;
extern s32 sParticleFrameMod64;
extern s32 sParticleFrameMod32;
extern s32 sParticleFrameMod16;
extern s32 sParticleFrameMod8;
extern s32 sParticleFrameMod4;
extern u32 sParticleFrameMod3;
extern s32 sParticleFrameMod2;
extern u32 sParticleFrameDiv64;
extern u32 sParticleFrameDiv32;
extern u32 sParticleFrameDiv16;
extern u32 sParticleFrameDiv8;
extern u32 sParticleFrameDiv4;
extern u32 sParticleFrameDiv3;
extern u32 sParticleFrameDiv2;
extern s32 sParticleFrameDiv64Parity;
extern s32 sParticleFrameDiv32Parity;
extern s32 sParticleFrameDiv16Parity;
extern s32 sParticleFrameDiv8Parity;
extern s32 sParticleFrameDiv4Parity;
extern s32 sParticleFrameDiv3Parity;
extern s32 sParticleFrameDiv2Parity;
extern s32 sParticleRandomOffset;
extern Particle* sParticlePool;
extern BattleAnimEffectSlot* gBattleAnimEffectSlots;
extern ParticleEmitter* gMiniFxEmitters;
extern s32 sParticlePoolIndex;
extern s32 gBattleAnimEffectSlotIndex;
extern s32 gMiniFxEmitterIndex;

void ParticleMath_BitwiseOr(s32* value, s32 mask);
void ParticleMath_BitwiseAndNot(s32* value, s32 mask);
void ParticleMath_BitwiseXor(s32* value, s32 mask);
s32 ParticleMath_HasBits(s32* value, s32 mask);
s32 ParticleMath_LacksBits(s32* value, s32 mask);
s32 ParticleMath_ApproachFloatUp(f32* value, f32 target, f32 step);
s32 ParticleMath_ApproachFloatDown(f32* value, f32 target, f32 step);
s32 ParticleMath_ApproachS32Up(s32* value, s32 target, s32 step);
s32 ParticleMath_ApproachS32Down(s32* value, s32 target, s32 step);
s32 ParticleMath_ApproachS16Up(s16* value, s16 target, s16 step);
s32 ParticleMath_ApproachU16Up(u16* value, u16 target, u16 step);
s32 ParticleMath_ApproachS16Down(s16* value, s16 target, s16 step);
s32 ParticleMath_ApproachU16Down(u16* value, s32 target, s32 step);
s32 ParticleMath_ApproachFloatUpByPointer(f32* value, f32 target, f32* step);
s32 ParticleMath_ApproachFloatDownByPointer(f32* value, f32 target, f32* step);
s32 ParticleMath_ApproachS32UpByPointer(s32* value, s32 target, s32* step);
s32 ParticleMath_ApproachS32DownByPointer(s32* value, s32 target, s32* step);
s32 ParticleMath_ApproachS16UpByPointer(s16* value, s16 target, s16* step);
s32 ParticleMath_ApproachS16DownByPointer(s16* value, s16 target, s16* step);
s32 ParticleMath_ApproachS8(s8* value, s8 target, s8 step);
s32 ParticleMath_ApproachU8(u8* value, u8 target, u8 step);
s32 ParticleMath_ApproachS16(s16* value, s16 target, s16 step);
s32 ParticleMath_ApproachAngle(s16* value, s16 target, s16 step);
s32 ParticleMath_ApproachS32(s32* value, s32 target, s32 step);
s32 ParticleMath_ApproachFloat(f32* value, f32 target, f32 step);
void ParticleMath_BounceFloat(f32* value, f32 minimum, f32 maximum, f32* step);
void ParticleMath_BounceS16(s16* value, s16 minimum, s16 maximum, s16* step);
void ParticleMath_CopyVec3f(Vec3f* destination, Vec3f* source);
void ParticleMath_CopyVec3s(Vec3s* destination, Vec3s* source);
void ParticleMath_AddVec3f(Vec3f* destination, Vec3f* source);
void ParticleMath_AddVec3s(Vec3s* destination, Vec3s* source);
f32 ParticleMath_DotVec3f(Vec3f lhs, Vec3f rhs);
s32 ParticleMath_IsWithinRadius2D(f32 first_x, f32 first_y, f32 second_x, f32 second_y, f32 radius);
void ParticleMath_AdvanceRandomOffset(void);
u32 ParticleMath_RandomRange(s32 range);
s32 ParticleMath_RandomRangeOffset(s32 range, s32 offset);
s32 ParticleMath_RandomSigned(s32 range);
s32 ParticleMath_RandomSignedOffset(s32 range, s32 offset);
s32 ParticleMath_RandomSignedOffsetBySign(s32 range, s32 offset);
void ParticleMath_NoopStub(UNUSED s32 unused_value);
s16 ParticleMath_Atan2YZ(Vec3f vector);
s16 ParticleMath_Atan2ZX(Vec3f vector);
s16 ParticleMath_Atan2XY(Vec3f vector);
void ParticleMath_Vec3ToAngles(Vec3s* destination, Vec3f vector);
f32 ParticleMath_Rotate2DX(f32 point_x, f32 point_y, s16 angle);
f32 ParticleMath_Rotate2DY(f32 point_x, f32 point_y, s16 angle);
Vec3f* ParticleMath_RotateVec3fY(Vec3f* destination, Vec3f vector, Vec3s rotation);
Vec3f* ParticleMath_MakeXZVectorFromAngle(Vec3f* destination, f32 length, s16 angle);
Vec3f* ParticleMath_RotateVec3fXY(Vec3f* destination, Vec3f vector, Vec3s rotation);
f32 ParticleMath_Distance3D(Vec3f start, Vec3f end);
f32 ParticleMath_DistanceXZ(Vec3f start, Vec3f end);
Vec3f* ParticleMath_NormalizedDirection(Vec3f* destination, Vec3f start, Vec3f end);
s32 ParticleMath_DistanceScaled(Vec3f start, Vec3f end, f32 divisor);
s32 ParticleMath_ScaleDirectionToLength(Vec3f* destination, Vec3f start, Vec3f end, f32 length);
Vec3f* ParticleMath_RandomDirection(Vec3f* destination, f32 length);
Vec3f* ParticleMath_ScaleVec3f(Vec3f* vector, f32 scale);
s16 ParticleMath_AngleXBetweenPoints(Vec3f start, Vec3f end);
s16 ParticleMath_AngleYBetweenPoints(Vec3f start, Vec3f end);
s16 ParticleMath_AngleZBetweenPoints(Vec3f start, Vec3f end);
s16 ParticleMath_ApproachAngleCoarse(s16 angle, s16 target_angle);
void ParticleGfx_NoopStub1(void);
void ParticleGfx_PushMatrix(MtxF* matrix);
void ParticleGfx_Build2DTransformMatrix(MtxF* matrix, s16 translation_x, s16 translation_y, s16 rotation_z, f32 scale);
void ParticleGfx_Build2DTransformMatrixYScaled(MtxF* matrix, s16 translation_x, s16 translation_y, s16 rotation_z, f32 scale_y);
void ParticleGfx_Push2DTransform(s16 translation_x, s16 translation_y, s16 rotation_z, f32 scale);
void ParticleGfx_Push2DTransformYScaled(s16 translation_x, s16 translation_y, s16 rotation_z, f32 scale_y);
void ParticleGfx_BuildTransformMatrix(MtxF* matrix, Vec3f position, Vec3s rotation, f32 scale);
void ParticleGfx_BuildTransformMatrixYScaled(MtxF* matrix, Vec3f position, Vec3s rotation, f32 scale_y);
void ParticleGfx_BuildTransformMatrixXYScaled(MtxF* matrix, Vec3f position, Vec3s rotation, f32 scale_x, f32 scale_y);
void ParticleGfx_BuildBillboardMatrix(MtxF* matrix, MtxF* view_matrix, Vec3f position, f32 scale);
void ParticleGfx_BuildBillboardMatrixYScaled(MtxF* matrix, MtxF* view_matrix, Vec3f position, f32 scale_y);
void ParticleGfx_BuildBillboardRotatedMatrix(MtxF* matrix, MtxF* view_matrix, Vec3f position, f32 scale, s16 rotation_z);
void ParticleGfx_BuildBillboardRotatedMatrixYScaled(MtxF* matrix, MtxF* view_matrix, Vec3f position, f32 scale_y, s16 rotation_z);
void ParticleGfx_BuildDirectionMatrix(MtxF* matrix, Vec3f direction, Vec3f position, s16 reference_angle, f32 scale);
void ParticleGfx_PushTransformMatrix(Vec3f position, Vec3s rotation, f32 scale);
void ParticleGfx_PushTransformMatrixYScaled(Vec3f position, Vec3s rotation, f32 scale_y);
void ParticleGfx_PushTransformMatrixXYScaled(Vec3f position, Vec3s rotation, f32 scale_x, f32 scale_y);
void ParticleGfx_PushBillboardMatrix(CameraNode* camera, Vec3f position, f32 scale);
void ParticleGfx_PushBillboardRotatedMatrix(CameraNode* camera, Vec3f position, f32 scale, s16 rotation_z);
void ParticleGfx_PushBillboardMatrixYScaled(CameraNode* camera, Vec3f position, f32 scale_y);
void ParticleGfx_PushBillboardRotatedMatrixYScaled(CameraNode* camera, Vec3f position, f32 scale_y, s16 rotation_z);
void ParticleGfx_PushTransformMatrixWithHalfTurnXY(Vec3f position, Vec3s rotation, f32 scale);
void ParticleGfx_PushDirectionMatrix(Vec3f position, Vec3f direction, f32 scale);


void ParticleGfx_PipeSync(void);
void ParticleGfx_SetTextureState(void);
s32 ParticleGfx_TextureSizeLog2(s32 size);
void ParticleGfx_NoopStub2(void);
void ParticleGfx_NoopStub3(void);
void ParticleGfx_NoopStub4(void);
void ParticleGfx_NoopStub5(void);
void ParticleGfx_NoopStub6(void);
void ParticleGfx_NoopStub7(void);
void ParticleGfx_NoopStub8(void);
void ParticleGfx_NoopStub9(void);
void ParticleGfx_LoadTextureBlock32(u8* texture, s32 fmt, s32 width, s32 height, s32 cms, s32 cmt, s32 masks, s32 maskt, s32 shifts, s32 shiftt);
void ParticleGfx_LoadTextureTile32(u8* texture, s32 fmt, s32 width, s32 height, s32 cms, s32 cmt, s32 masks, s32 maskt, s32 shifts, s32 shiftt);
void ParticleGfx_LoadTextureBlock16(u8* texture, s32 fmt, s32 width, s32 height, s32 cms, s32 cmt, s32 masks, s32 maskt, s32 shifts, s32 shiftt);
void ParticleGfx_LoadTextureTile16(u8* texture, s32 fmt, s32 width, s32 height, s32 cms, s32 cmt, s32 masks, s32 maskt, s32 shifts, s32 shiftt);
void ParticleGfx_LoadTextureBlock8(u8* texture, s32 fmt, s32 width, s32 height, s32 cms, s32 cmt, s32 masks, s32 maskt, s32 shifts, s32 shiftt);
void ParticleGfx_LoadTextureTile8(u8* texture, s32 fmt, s32 width, s32 height, s32 cms, s32 cmt, s32 masks, s32 maskt, s32 shifts, s32 shiftt);
void ParticleGfx_LoadTextureBlock4(u8* texture, s32 fmt, s32 width, s32 height, s32 cms, s32 cmt, s32 masks, s32 maskt, s32 shifts, s32 shiftt);
void ParticleGfx_LoadTextureTile4(u8* texture, s32 fmt, s32 width, s32 height, s32 cms, s32 cmt, s32 masks, s32 maskt, s32 shifts, s32 shiftt);
void ParticleGfx_LoadTextureMultiBlock4(u8* texture, s32 tmem, s32 rtile, s32 fmt, s32 width, s32 height, s32 cms, s32 cmt, s32 masks, s32 maskt, s32 shifts, s32 shiftt);
void ParticleGfx_LoadTextureRgba32(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureTileRgba32(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureRgba16(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureRgba16Dual(u8* texture1, u8* texture2, s32 width, s32 height);
void ParticleGfx_LoadTextureRgba16WrapS(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureRgba16WrapT(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureRgba16WrapST(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureRgba16WrapSWithMask(u8* texture, s32 width, s32 height, s32 masks);
void ParticleGfx_LoadTextureTileRgba16(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureIa16(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureTileIa16(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureIa8(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureTileIa8(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureIa8WrapST(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureI8(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureI8WrapST(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureTileI8(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureI8Alias(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureTileI8Alias(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureTileIa4(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureTileI4(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureIa4(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureI4(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureI4WrapST(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureI4Alias(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureTileI4Alias(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureI4WrapS(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureI4WrapSTNoMirror(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureI4MultiBlockWrapST(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureI4MultiBlockWrapSTWithShift(u8* texture, s32 width, s32 height, s32 shifts, s32 shiftt);
void ParticleGfx_LoadTextureCi8(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureCi8WrapSWithMask(u8* texture, s32 width, s32 height, s32 masks);
void ParticleGfx_LoadTextureTileCi8(u8* texture, s32 width, s32 height);
void ParticleGfx_LoadTextureDualRgba16I8(u8* texture1, u8* texture2, s32 width, s32 height);
void ParticleGfx_DrawQuadVertices(Vtx* vtx);
void ParticleGfx_NoopStub10(void);
void ParticleGfx_DrawTransformedQuad(Vec3f position, Vec3s rotation, f32 scale, Vtx* vtx);
void ParticleGfx_DrawTransformedQuadYScaled(Vec3f position, Vec3s rotation, f32 scale_y, Vtx* vtx);
void ParticleGfx_DrawTransformedQuadXYScaled(Vec3f position, Vec3s rotation, f32 scale_x, f32 scale_y, Vtx* vtx);
void ParticleGfx_DrawTransformedTriangle(Vec3f position, Vec3s rotation, f32 scale, Vtx* vtx);
void ParticleGfx_DrawModelQuad(CameraNode* camera, Vec3f position, f32 scale, Vtx* vtx);
void ParticleGfx_DrawModelQuadRotated(CameraNode* camera, Vec3f position, f32 scale, Vtx* vtx, s16 rotation_z);
void ParticleGfx_DrawModelQuadYScaled(CameraNode* camera, Vec3f position, f32 scale_y, Vtx* vtx);
void ParticleGfx_DrawModelQuadRotatedYScaled(CameraNode* camera, Vec3f position, f32 scale_y, Vtx* vtx, s16 rotation_z);
void ParticleGfx_DrawModelTriangle(CameraNode* camera, Vec3f position, f32 scale, Vtx* vtx);
void ParticleGfx_DrawModelTriangleRotated(CameraNode* camera, Vec3f position, f32 scale, Vtx* vtx, s16 rotation_z);
void ParticleGfx_DrawI8TextureAtlas(u8* texture, Vtx* vtx, s32 atlas_width, s32 atlas_height, s32 width, s32 height);
void ParticleGfx_DrawIa16TextureAtlas(u8* texture, Vtx* vtx, s32 atlas_width, s32 atlas_height, s32 width, s32 height);
void ParticleGfx_DrawIa8TextureAtlas(u8* texture, Vtx* vtx, s32 atlas_width, s32 atlas_height, s32 width, s32 height);
void ParticleGfx_DrawI4TextureAtlas(u8* texture, Vtx* vtx, s32 atlas_width, s32 atlas_height, s32 width, s32 height);
void ParticleGfx_DrawI4TextureAtlasTiled(u8* texture, Vtx* vtx, s32 atlas_width, s32 atlas_height, s32 width, s32 height);
void ParticleGfx_SetPrimColor(u8 r, u8 g, u8 b, u8 a);
void ParticleGfx_SetEnvColor(u8 r, u8 g, u8 b, u8 a);
void ParticleGfx_SetFogColor(u8 r, u8 g, u8 b, u8 a);
void ParticleGfx_SetBlendColor(u8 r, u8 g, u8 b, u8 a);
void ParticleGfx_SetPrimColorTextureCombine(u8 r, u8 g, u8 b, u8 a);
void ParticleGfx_SetTextureAlphaCombine(u8 alpha);
void ParticleGfx_SetPrimEnvColorCombine(u8 prim_r, u8 prim_g, u8 prim_b, u8 env_r, u8 env_g, u8 env_b, u8 prim_a);
void ParticleGfx_SetPrimEnvNoiseCombine(u8 prim_r, u8 prim_g, u8 prim_b, u8 env_r, u8 env_g, u8 env_b, u8 prim_a);
void ParticleGfx_SetPrimEnvColorShadeCombine(u8 prim_r, u8 prim_g, u8 prim_b, u8 prim_a, u8 env_r, u8 env_g, u8 env_b, u8 env_a);
void ParticleGfx_SetPrimEnvLodCombine(u8 lod_frac, u8 prim_r, u8 prim_g, u8 prim_b, u8 prim_a, u8 env_r, u8 env_g, u8 env_b);
void ParticleGfx_SetPrimEnvLodTextureCombine(u8 lod_frac, u8 prim_r, u8 prim_g, u8 prim_b, u8 prim_a, u8 env_r, u8 env_g, u8 env_b);
u32 ParticleGfx_GetFrameDivisor(s32 divisor);
void ParticleGfx_SetAnimatedTileSize(s32 scroll_s, s32 scroll_t, s32 width, s32 height);
void ParticleGfx_SetAnimatedAuxTileSize(s32 scroll_s, s32 scroll_t, s32 width, s32 height);
void ParticleGfx_SetAnimatedTileSizeOnList(Gfx* gfx, s32 scroll_s, s32 scroll_t, s32 width, s32 height);
void ParticleGfx_SetAnimatedAuxTileSizeOnList(Gfx* gfx, s32 scroll_s, s32 scroll_t, s32 width, s32 height);
void ParticleGfx_SetPartiallyAnimatedTileSize(Gfx* gfx, s32 start_s, s32 scroll_t, s32 width, s32 height);
void ParticleGfx_SetPartiallyAnimatedAuxTileSize(Gfx* gfx, s32 start_s, s32 scroll_t, s32 width, s32 height);
void ParticleGfx_SetFrameIndexedTileSize(Gfx* gfx, s32 start_s, s32 start_t, s32 step_s, s32 step_t, s32 width, s32 height, s32 clock_divisor);
void ParticleGfx_SetFrameIndexedAuxTileSize(Gfx* gfx, s32 start_s, s32 start_t, s32 step_s, s32 step_t, s32 width, s32 height, s32 clock_divisor);
void ParticleGfx_LoadTextureI4ToList(Gfx* gfx, u8* texture, s32 width, s32 height);
Gfx* ParticleGfx_LoadDescriptorTexture(Gfx* gfx, u8* texture, ParticleTexture* descriptor, s32 tmem, s32 rtile);
Gfx* ParticleGfx_LoadDescriptorTextureAndTile(Gfx* gfx, ParticleTexture* descriptor, s32 tmem, s32 rtile);
Gfx* ParticleGfx_LoadPrimaryDescriptorTexture(Gfx* gfx, ParticleMaterial* material);
Gfx* ParticleGfx_LoadSecondaryDescriptorTexture(Gfx* gfx, ParticleMaterial* material);
void ParticleGfx_SetCombineModeFromDescriptor(Gfx* gfx, ParticleCombineMux* combine_mux);
Gfx* ParticleGfx_AppendDescriptorMaterial(Gfx* gfx, ParticleMaterial* material);
void ParticleGfx_GraphNodeBuildDescriptorMaterialList(s32 callback_phase, arg1_func_87903D64* node);


void Particle_ResetDefaults(Particle* particle);
Particle* Particle_Alloc(void);
void Particle_Free(Particle* particle);
Particle* Particle_New(void);
Particle* Particle_AllocChain(u8 count);
void Particle_EmitBurstAtTransform(f32 unk_28_value, Vec3f origin, Vec3s rotation, ParticleUpdateCallback update_callback, ParticleDescriptor* descriptor, s16 count);
void Particle_EmitBurstAtTransformWithMetadata(f32 unk_28_value, Vec3f origin, Vec3s rotation, ParticleUpdateCallback update_callback, ParticleDescriptor* descriptor, s16 count, s16 metadata);
void Particle_EmitBurstFromObject(DisplayObject* object, ParticleUpdateCallback update_callback, ParticleDescriptor* descriptor, s16 count, s16 anchor_id);
void Particle_EmitBurstFromObjectWithMetadata(DisplayObject* object, ParticleUpdateCallback update_callback, ParticleDescriptor* descriptor, s16 count, s16 anchor_id, s16 metadata);
s32 Particle_CountActive(void);
s32 Particle_CountActiveAlias(void);
void Particle_InitLifecycle(Particle* particle);
void Particle_MarkInactive(Particle* particle);
void Particle_DisableAndDetach(Particle* particle);
s32 Particle_IsInactive(Particle* particle);
s32 Particle_IsRunning(Particle* particle);
void Particle_AdvanceLifecycle(Particle* particle);
s32 Particle_TickDelayTimer(Particle* particle, s16 duration);
s32 Particle_TickDelayAndAdvanceLifecycle(Particle* particle, s16 duration);
void Particle_SetFlags(Particle* particle, s32 mask);
void Particle_ClearFlags(Particle* particle, s32 mask);
void Particle_ToggleFlags(Particle* particle, s32 mask);
s32 Particle_HasFlags(Particle* particle, s32 mask);
s32 Particle_LacksFlags(Particle* particle, s32 mask);
void Particle_EnableRendering(Particle* particle);
void Particle_DisableRendering(Particle* particle);
s32 Particle_IsRenderingEnabled(Particle* particle);
void Particle_EnableFlag40(Particle* particle);
void Particle_DisableFlag40(Particle* particle);
void Particle_HasFlag40(Particle* particle);
void Particle_SetTextureFrame(Particle* particle, s16 frame);
s32 Particle_AnimateFramesForward(Particle* particle, s16 first_frame, s16 last_frame, s16 frame_step, s16 frame_delay, s16 loop_count);
s32 Particle_AnimateFramesForwardAndAdvanceLifecycle(Particle* particle, s16 first_frame, s16 last_frame, s16 frame_step, s16 frame_delay, s16 loop_count);
s32 Particle_AnimateFramesReverse(Particle* particle, s16 first_frame, s16 last_frame, s16 frame_step, s16 frame_delay, s16 loop_count);
s32 Particle_AnimateFramesReverseAndAdvanceLifecycle(Particle* particle, s16 first_frame, s16 last_frame, s16 frame_step, s16 frame_delay, s16 loop_count);
s32 Particle_AnimateFramesPingPong(Particle* particle, s16 first_frame, s16 last_frame, s16 frame_step, s16 frame_delay, s16 loop_count);
s32 Particle_AnimateFramesPingPongAndAdvanceLifecycle(Particle* particle, s16 first_frame, s16 last_frame, s16 frame_step, s16 frame_delay, s16 loop_count);
s32 Particle_AnimateFramesReverseFirstPingPong(Particle* particle, s16 first_frame, s16 last_frame, s16 frame_step, s16 frame_delay, s16 loop_count);
s32 Particle_AnimateFramesReverseFirstPingPongAndAdvanceLifecycle(Particle* particle, s16 first_frame, s16 last_frame, s16 frame_step, s16 frame_delay, s16 loop_count);
void Particle_BeginPhase(Particle* particle);
void Particle_ResetPhase(Particle* particle);
void Particle_AdvancePhase(Particle* particle);
s32 Particle_TickPhaseTimer(Particle* particle, s16 duration);
void Particle_UpdateWorldTransform(Particle* particle);
void Particle_SetPosition(Particle* particle, f32 x, f32 y, f32 z);
void Particle_PositionOffset_SetVecFromVec3fScaled(Particle* particle, Vec3f position_offset);
void Particle_PositionOffset_SetVec(Particle* particle, f32 x, f32 y, f32 z);
void Particle_PositionOffset_SetXScaled(Particle* particle, f32 x);
void Particle_PositionOffset_SetYScaled(Particle* particle, f32 y);
void Particle_PositionOffset_SetZScaled(Particle* particle, f32 z);
void Particle_SetPositionFromRotationTemplate1(Particle* particle, f32 magnitude);
void Particle_SetPositionFromRotationTemplate2(Particle* particle, f32 magnitude);
void Particle_PositionOffset_AddVecScaledFromVec3f(Particle* particle, Vec3f delta);
void Particle_PositionOffset_AddVecFromVec3f(Particle* particle, Vec3f delta);
void Particle_PositionOffset_AddVecScaled(Particle* particle, f32 delta_x, f32 delta_y, f32 delta_z);
void Particle_PositionOffset_AddXScaled(Particle* particle, f32 delta_x);
void Particle_PositionOffset_AddYScaled(Particle* particle, f32 delta_y);
void Particle_PositionOffset_AddZScaled(Particle* particle, f32 delta_z);
void Particle_PositionOffset_AddXZFromRotationY(Particle* particle, f32 magnitude);
void Particle_Field38_AddXZFromField94YOffset180_08F38(Particle* particle, f32 magnitude);
void Particle_PositionOffset_AddXZFromRotationYOffset90(Particle* particle, f32 magnitude);
void Particle_Field38_AddXZFromField94YOffsetMinus90_0901C(Particle* particle, f32 magnitude);
void Particle_AddPositionOffsetFromRotationDirection(Particle* particle, f32 magnitude);
void Particle_AddPositionFromRotationTemplate1(Particle* particle, f32 magnitude);
void Particle_AddPositionFromRotationTemplate2(Particle* particle, f32 magnitude);
void Particle_AddPositionOffsetRotatedY(Particle* particle, Vec3f offset);
void Particle_SetRandomPositionOffsetX(Particle* particle, s32 range);
void Particle_SetRandomPositionOffsetY(Particle* particle, s32 range);
void Particle_SetRandomPositionOffsetZ(Particle* particle, s32 range);
void Particle_SetRandomPositionOffsetXWithOffset(Particle* particle, s32 range, s32 offset);
void Particle_SetRandomPositionOffsetYOffset(Particle* particle, s32 range, s32 offset);
void Particle_SetRandomPositionOffsetZWithOffset(Particle* particle, s32 range, s32 offset);
void Particle_SetRandomPositionOffsetXSymmetric(Particle* particle, s32 range, s32 offset);
void Particle_SetRandomPositionOffsetYSignedOffset(Particle* particle, s32 range, s32 offset);
void Particle_SetRandomPositionOffsetZSymmetric(Particle* particle, s32 range, s32 offset);
void Particle_SetRandomPositionOffsetXYZ(Particle* particle, s32 range_x, s32 range_y, s32 range_z);
void Particle_SetRandomPositionOffsetXYZPositive(Particle* particle, s32 range_x, s32 range_y, s32 range_z);
void Particle_SetRandomRotatedPositionOffsetXY(Particle* particle, s32 range_x, s32 range_y);
void Particle_AddRandomPositionOffsetXYZ(Particle* particle, s32 range_x, s32 range_y, s32 range_z);
void Particle_SetOrigin(Particle* particle, f32 x, f32 y, f32 z);
void Particle_SetMotionOffset(Particle* particle, f32 x, f32 y, f32 z);
void Particle_MotionOffset_SetXScaled(Particle* particle, f32 x);
void Particle_MotionOffset_SetYScaled(Particle* particle, f32 y);
void Particle_MotionOffset_SetZScaled(Particle* particle, f32 z);
void Particle_SetMotionOffsetRandomDirection(Particle* particle, f32 magnitude);
void Particle_MotionOffset_AddVecScaled(Particle* particle, f32 delta_x, f32 delta_y, f32 delta_z);
void Particle_MotionOffset_AddXScaled(Particle* particle, f32 delta_x);
void Particle_MotionOffset_AddYScaled(Particle* particle, f32 delta_y);
void Particle_MotionOffset_AddZScaled(Particle* particle, f32 delta_z);
void Particle_MotionOffset_SubVecScaled(Particle* particle, f32 delta_x, f32 delta_y, f32 delta_z);
void Particle_MotionOffset_SubXScaled(Particle* particle, f32 delta_x);
void Particle_MotionOffset_SubYScaled(Particle* particle, f32 delta_y);
void Particle_MotionOffset_SubZScaled(Particle* particle, f32 delta_z);
s32 Particle_MotionOffset_ApproachXToMotionTarget(Particle* particle, f32 target);
s32 Particle_MotionOffset_ApproachYToMotionTarget(Particle* particle, f32 target);
s32 Particle_MotionOffset_ApproachZToMotionTarget(Particle* particle, f32 target);
void Particle_MotionOffset_SetVecFromMotionStep(Particle* particle);
void Particle_MotionOffset_SetXZFromMotionStep(Particle* particle);
void Particle_MotionOffset_SetXYFromMotionStep(Particle* particle);
void Particle_MotionOffset_AddVecFromMotionStep(Particle* particle);
void Particle_MotionOffset_AddXZFromMotionStep(Particle* particle);
void Particle_MotionOffset_AddXYFromMotionStep(Particle* particle);
void Particle_MotionOffset_AddXFromMotionStep(Particle* particle);
void Particle_MotionOffset_AddYFromMotionStep(Particle* particle);
void Particle_MotionOffset_AddZFromMotionStep(Particle* particle);
void Particle_SetMotionOffsetXZFromRotationY(Particle* particle);
void Particle_AddVelocityXZFromRotationYViaImpulse(Particle* particle);
void Particle_AddVelocityXYFromRotationZViaImpulse(Particle* particle);
void Particle_AddMotionOffsetFromRotationTemplate1(Particle* particle);
void Particle_AddMotionOffsetFromRotationTemplate2(Particle* particle);
void Particle_AddMotionOffsetFromRotationTemplate3(Particle* particle);
void Particle_AddMotionOffsetFromRotationTemplate4(Particle* particle);
void Particle_AddMotionOffsetFromRotationTemplate5(Particle* particle);
void Particle_AddMotionOffsetFromRotationTemplate6(Particle* particle);
void Particle_AddMotionOffsetFromRotationTemplate7A(Particle* particle);
void Particle_AddMotionOffsetFromRotationTemplate7B(Particle* particle);
void Particle_AddMotionOffsetXZFromRotationY(Particle* particle);
void Particle_AddVelocityXZFromRotationYPlusHalfTurn(Particle* particle);
void Particle_AddVelocityXZFromRotationYPlusHalfTurnFromArgument(Particle* particle, f32 magnitude);
void Particle_AddVerticalMotionScalar(Particle* particle);
s32 Particle_Field50_AddNegField74Y_CheckField2CYNonPositive_09F84(Particle* particle);
s32 Particle_Field74_AdvanceAndAddNegY_CheckField2CYNonPositive_09FD8(Particle* particle);
void Particle_SetMotionOffsetTarget(Particle* particle, f32 x, f32 y, f32 z);
void Particle_MotionTarget_SetXScaled(Particle* particle, f32 x);
void Particle_MotionTarget_SetYScaled(Particle* particle, f32 y);
void Particle_MotionTarget_SetZScaled(Particle* particle, f32 z);
s32 Particle_MotionTarget_ApproachY(Particle* particle, f32 target, f32 step);
f32 ParticleMath_ScaleApproachStep(f32 delta);
void Particle_ApproachPosition(Particle* particle, Vec3f target_position);
void Particle_Rotation_SetVecFromVec3s(Particle* particle, Vec3s rotation);
void Particle_SetRotation(Particle* particle, s16 rotation_x, s16 rotation_y, s16 rotation_z);
void Particle_Rotation_SetX(Particle* particle, s16 rotation_x);
void Particle_Rotation_SetY(Particle* particle, s16 rotation_y);
void Particle_Rotation_SetZ(Particle* particle, s16 rotation_z);
void Particle_Rotation_AddConstantAll(Particle* particle);
void Particle_Rotation_AddXConstant(Particle* particle);
void Particle_Rotation_AddYConstant(Particle* particle);
void Particle_Rotation_AddZConstant(Particle* particle);
void Particle_SetRotationSpeed(Particle* particle, s16 rotation_step_x, s16 rotation_step_y, s16 rotation_step_z);
void Particle_RotationStep_SetVecFromVec3s(Particle* particle, Vec3s rotation_step);
void Particle_RotationStep_SetXY(Particle* particle, s16 rotation_step_x, s16 rotation_step_y);
void Particle_RotationStep_SetX(Particle* particle, s16 rotation_step_x);
void Particle_RotationStep_SetY(Particle* particle, s16 rotation_step_y);
void Particle_RotationStep_SetZ(Particle* particle, s16 rotation_step_z);
void Particle_RotationTarget_SetX(Particle* particle, s16 rotation_target_x);
void Particle_RotationTarget_SetY(Particle* particle, s16 rotation_target_y);
void Particle_SetRotationTargetXY(Particle* particle, s16 rotation_target_x, s16 rotation_target_y);
void Particle_SetRotationTargetFromCurrent(Particle* particle);
void Particle_Rotation_AddVecFromRotationStep(Particle* particle);
void Particle_Rotation_AddXFromRotationStep(Particle* particle);
void Particle_Rotation_AddYFromRotationStep(Particle* particle);
void Particle_Rotation_AddZFromRotationStep(Particle* particle);
void Particle_Rotation_AddVecFromVec3s(Particle* particle, Vec3s delta_rotation);
void Particle_Rotation_AddVec(Particle* particle, s16 delta_rotation_x, s16 delta_rotation_y, s16 delta_rotation_z);
void Particle_Rotation_AddX(Particle* particle, s16 delta_rotation_x);
void Particle_Rotation_AddY(Particle* particle, s16 delta_rotation_y);
void Particle_Rotation_AddZ(Particle* particle, s16 delta_rotation_z);
void Particle_AddRotationTargetXY(Particle* particle, s16 delta_rotation_target_x, s16 delta_rotation_target_y);
void Particle_Rotation_SubVecFromVec3s(Particle* particle, Vec3s delta_rotation);
void Particle_Rotation_SubX(Particle* particle, s16 delta_rotation_x);
void Particle_Rotation_SubY(Particle* particle, s16 delta_rotation_y);
void Particle_Rotation_SubZ(Particle* particle, s16 delta_rotation_z);
void Particle_ApproachRotationTargetX(Particle* particle);
void Particle_ApproachRotationTargetY(Particle* particle);
void Particle_ApproachRotationTargetZ(Particle* particle);
void Particle_ApproachRotationTargetXY(Particle* particle);
void Particle_ApproachRotationTargetXYZ(Particle* particle);
void Particle_ApproachRotationXTo(Particle* particle, s16 target, s16 step);
void Particle_ApproachRotationYTo(Particle* particle, s16 target, s16 step);
s32 Particle_Rotation_DecrementXTo(Particle* particle, s32 target, s32 step);
s32 Particle_Rotation_DecrementYTo(Particle* particle, s32 target, s32 step);
s32 Particle_Rotation_DecrementZTo(Particle* particle, s32 target, s32 step);
void Particle_AddRotationRandomSignedXY(Particle* particle, s32 range_x, s32 range_y);
void Particle_AddRotationRandomSignedXYWithXHalfTurn(Particle* particle, s32 range_x, s32 range_y);
void Particle_AddRotationRandomSignedY(Particle* particle, s32 range);
void Particle_AddRotationRandomSignedYWithHalfTurn(Particle* particle, s32 range);
void Particle_AddRotationRandomSignedXWithFixedYQuarterTurn(Particle* particle);
void Particle_ComputeVectorFromRotationY(Particle* particle, f32 magnitude, Vec3f* destination);
void Particle_ComputeVectorFromRotationXY(Particle* particle, f32 magnitude, Vec3f* destination);
void Particle_ComputeVectorFromRotationXZ(Particle* particle, f32 magnitude, Vec3f* destination);
void Particle_ComputeVectorFromRotationXYZAlt(Particle* particle, f32 magnitude, Vec3f* destination);
void Particle_ComputeVectorFromRotationYZVariant(Particle* particle, f32 magnitude, Vec3f* destination);
void Particle_StepMagnitude_SetScaled(Particle* particle, f32 step_magnitude);
void Particle_MagnitudeStep_SetScaled(Particle* particle, f32 magnitude_step);
s32 Particle_StepMagnitude_Approach(Particle* particle, f32 target, f32 step);
s32 Particle_StepMagnitude_ApproachUsingMagnitudeStep(Particle* particle, f32 target);
s32 Particle_MagnitudeStep_Approach(Particle* particle, f32 target, f32 step);
void Particle_MotionStep_SetVecScaled(Particle* particle, Vec3f motion_step);
void Particle_SetMotionStep(Particle* particle, f32 x, f32 y, f32 z);
void Particle_MotionStep_SetXScaled(Particle* particle, f32 x);
void Particle_MotionStep_SetYScaled(Particle* particle, f32 y);
void Particle_MotionStep_SetZScaled(Particle* particle, f32 z);
s32 Particle_MotionStep_IncrementXUntilAtLeast(Particle* particle, f32 limit, f32 increment);
s32 Particle_MotionStep_IncrementYUntilAtLeast(Particle* particle, f32 limit, f32 increment);
s32 Particle_MotionStep_IncrementZUntilAtLeast(Particle* particle, f32 limit, f32 increment);
s32 Particle_MotionStep_DecrementXUntilAtMost(Particle* particle, f32 limit, f32 decrement);
s32 Particle_MotionStep_DecrementYUntilAtMost(Particle* particle, f32 limit, f32 decrement);
s32 Particle_MotionStep_DecrementZUntilAtMost(Particle* particle, f32 limit, f32 decrement);
void Particle_MotionStep_ScaleVec(Particle* particle, f32 scale);
s32 Particle_MotionStep_ApproachY(Particle* particle, f32 target, f32 step);
void Particle_MotionStep_SetYFromField74AngleX(Particle* particle);
void Particle_MotionStep_SetXFromField74AngleY(Particle* particle);
void Particle_MotionStep_SetZFromField74AngleY(Particle* particle);
void Particle_MotionStep_SetXYFromField74AngleZ(Particle* particle);
void Particle_MotionStep_SetXZFromField74AngleY(Particle* particle);
void Particle_MotionStep_SetVecFromField74AnglesXY(Particle* particle);
void Particle_MotionStep_SetVecFromField74AnglesXZ(Particle* particle);
void Particle_MotionStep_SetVecFromField74AnglesXYAlt(Particle* particle);
void Particle_MotionStep_SetVecFromField74AnglesYZ(Particle* particle);
void Particle_MotionStep_SetVecFromField74AnglesXYZ(Particle* particle);
void Particle_MotionStep_SetVecFromField74AnglesXYZAlt(Particle* particle);
void Particle_MotionStep_SetVecFromField74AnglesYZAlt(Particle* particle);
void Particle_Field7C_SetVecFromField74AnglesYZVariant_0B700(Particle* particle);
void Particle_UpdateRotatingMotionOffsetXZ(Particle* particle, f32 radius, s16 angle_step);
s32 Particle_MotionOffset_AdvanceMotionStepWithTimer(Particle* particle, f32 initial_motion_step_y, f32 motion_step_y_decrement, f32 step_magnitude, s16 rotation_y, s16 duration);
void Particle_Scale_SetScaled(Particle* particle, f32 scale);
void Particle_SetScale(Particle* particle, f32 scale);
void Particle_SetScaleParameter20(Particle* particle, f32 value);
void Particle_SetScaleParameter24(Particle* particle, f32 value);
s32 Particle_Scale_Approach(Particle* particle, f32 target, f32 step);
s32 Particle_Scale_ApproachAndStepState(Particle* particle, f32 target, f32 step);
s32 Particle_ApproachScaleUsingFields20And24(Particle* particle);
s32 Particle_ApproachStoredScaleAndAdvanceLifecycle(Particle* particle);
s32 Particle_Field20_Approach_0BA84(Particle* particle, f32 target, f32 step);
void Particle_SetPrimColor(Particle* particle, u8 red, u8 green, u8 blue);
s32 Particle_PrimR_Approach_0BAE4(Particle* particle, u8 target, u8 step);
s32 Particle_PrimG_Approach_0BB14(Particle* particle, u8 target, u8 step);
s32 Particle_PrimB_Approach_0BB44(Particle* particle, u8 target, u8 step);
void Particle_SetEnvColor(Particle* particle, u8 red, u8 green, u8 blue);
s32 Particle_EnvR_Approach_0BB90(Particle* particle, u8 target, u8 step);
s32 Particle_ApproachEnvBlue(Particle* particle, u8 target, u8 step);
s32 Particle_ApproachEnvGreen(Particle* particle, u8 target, u8 step);
void Particle_SetPrimAlpha(Particle* particle, u8 alpha);
s32 Particle_PrimAlpha_Approach_0BC2C(Particle* particle, u8 target_alpha, u8 alpha_step);
s32 Particle_PrimAlpha_ApproachAndStepState_0BC5C(Particle* particle, u8 target_alpha, u8 alpha_step);
void Particle_FieldA6_SetS16_0BCA8(Particle* particle, s16 value);
s16 Particle_FieldA6_GetS16_0BCBC(Particle* particle);
s32 Particle_FieldA6_DecrementTo_0BCC4(Particle* particle, s16 target, s16 step);
s32 Particle_FieldA6_DecrementToZero_0BD08(Particle* particle, s16 step);
s32 Particle_FieldA6_DecrementToZeroAndStepState_0BD34(Particle* particle, s16 step);
s32 Particle_FieldA6_DecrementToStep1_0BD80(Particle* particle, s16 target);
s32 Particle_FieldA6_DecrementToZeroStep1_0BDAC(Particle* particle);
s32 Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(Particle* particle);
void Particle_FieldA8_SetS16_0BE14(Particle* particle, s16 value);
s16 Particle_FieldA8_GetS16_0BE28(Particle* particle);
s32 Particle_FieldA8_DecrementTo_0BE30(Particle* particle, s16 target);
s32 Particle_FieldA8_DecrementToZeroStep1_0BE6C(Particle* particle);
s32 Particle_FieldA8_DecrementToZeroStep1AndStepState_0BE8C(Particle* particle);
void Particle_FieldCA_SetU8_0BECC(Particle* particle, u8 value);
void Particle_FieldAC_SetS16_0BED8(Particle* particle, s16 value);
void Particle_FieldCF_SetU8_0BEEC(Particle* particle, u8 value);
void Particle_FieldCF_SetU8Alt_0BEF8(Particle* particle, u8 value);
void Particle_FieldCC_SetU8_0BF04(Particle* particle, u8 value);
void Particle_FieldAA_SetS16_0BF10(Particle* particle, s16 value);
void Particle_FieldCE_SetS16_0BF24(Particle* particle, s16 value);
void Particle_FieldCD_SetS16_0BF38(Particle* particle, s16 value);
void Particle_FieldAE_SetS16_0BF4C(Particle* particle, s16 value);
s32 Particle_FieldAE_Approach_0BF60(Particle* particle, s16 target, s16 step);
s32 Particle_FieldAE_ApproachAndStepState_0BF98(Particle* particle, s16 target, s16 step);
void Particle_FieldB0_SetS16_0BFEC(Particle* particle, s16 value);
s32 Particle_FieldB0_Approach_0C000(Particle* particle, s16 target, s16 step);
s16 Particle_FieldAC_GetS16_0C038(Particle* particle);
s32 Particle_FieldCE_GetS32_0C040(Particle* particle);
u8 Particle_FieldCD_GetU8_0C048(Particle* particle);
s16 Particle_FieldAE_GetS16_0C050(Particle* particle);
s16 Particle_FieldCF_GetS16_0C058(Particle* particle);
s16 Particle_FieldAA_GetS16_0C068(Particle* particle);
void Particle_SetFieldOffsetACIfNotSentinel(Particle* particle, s16 value);
void Particle_SetFieldOffsetCFIfNotSentinel(Particle* particle, u8 value);
void Particle_SetFieldOffsetA6IfNotSentinel(Particle* particle, s16 value);
void Particle_SetFieldOffsetA8IfNotSentinel(Particle* particle, s16 value);
void Particle_SetFieldOffsetCEIfNotSentinel(Particle* particle, u8 value);
void Particle_SetFieldOffsetCDIfNotSentinel(Particle* particle, u8 value);
void Particle_SetFieldOffsetAAIfNotSentinel(Particle* particle, s16 value);
void Particle_SetPrimAlphaIfNotSentinel(Particle* particle, u8 alpha);
void Particle_SetFieldOffsetAEIfNotSentinel(Particle* particle, s16 value);
void Particle_SetFieldOffsetB0IfNotSentinel(Particle* particle, s16 value);
void Particle_RenderWorldChains(CameraNode* camera);
void Particle_RenderOrthoParticles(CameraNode* camera);
void Particle_SetRenderMode(s16 render_mode);
void Particle_InvokeRenderSetup(Particle* particle, ParticleDescriptorChild* setup);
void Particle_PrepareRender(Particle* particle);
void Particle_DispatchDraw(Particle* particle, CameraNode* camera);
void Particle_NoopDraw(UNUSED Particle* particle, UNUSED CameraNode* camera);


void Particle_UpdateFrameCounters(void);
void Particle_UpdateFrameCountersAlias(void);


void ParticleGfx_LoadTextureCallbackRgba16_32x32(UNUSED Particle* particle, u8* texture);
void ParticleGfx_LoadTextureCallbackIa8Frame_32x32(Particle* particle, u8* texture);
void ParticleGfx_LoadTextureCallbackIa8_32x32(UNUSED Particle* particle, u8* texture);
void ParticleGfx_LoadTextureCallbackIa8Frame_32x64(Particle* particle, u8* texture);
void ParticleGfx_LoadTextureCallbackI4_32x32(UNUSED Particle* particle, u8* texture);
void ParticleGfx_LoadTextureCallbackI4ParticleFrame_32x32(Particle* particle, u8* texture);
void ParticleGfx_LoadTextureCallbackI4FrameMod4_32x32(UNUSED Particle* particle, u8* texture);
void ParticleGfx_LoadTextureCallbackI4FrameDiv2Mod4_32x32(UNUSED Particle* particle, u8* texture);
void ParticleGfx_LoadTextureCallbackI4FrameDiv4Mod4_32x32(UNUSED Particle* particle, u8* texture);
void ParticleGfx_LoadTextureCallbackI4FrameDiv2Mod8_32x32(UNUSED Particle* particle, u8* texture);
void ParticleGfx_LoadTextureCallbackI4TileFrameDiv2Mod8_24x24(UNUSED Particle* particle, u8* texture);
void ParticleGfx_LoadTextureCallbackI4TileParticleFrame_24x24(Particle* particle, u8* texture);
void ParticleGfx_LoadTextureCallbackI4FrameMod10_32x64(UNUSED Particle* particle, u8* texture);
void ParticleGfx_LoadTextureCallbackI4ParticleFrame_32x64(Particle* particle, u8* texture);
void ParticleGfx_LoadTextureCallbackI4_64x64(UNUSED Particle* particle, u8* texture);
void ParticleGfx_LoadTextureCallbackI4FrameMod8_64x64(UNUSED Particle* particle, u8* texture);
void ParticleGfx_LoadTextureCallbackI4FrameDiv2Mod8_64x64(UNUSED Particle* particle, u8* texture);
void ParticleGfx_LoadTextureCallbackI4ParticleFrame_64x64(Particle* particle, u8* texture);
void ParticleGfx_LoadTextureCallbackI4WrapST_64x64(UNUSED Particle* particle, u8* texture);
void ParticleGfx_LoadTextureCallbackIa8WrapST_64x64(UNUSED Particle* particle, u8* texture);
void ParticleGfx_DrawBillboardPrim(Particle* particle, CameraNode* camera);
void ParticleGfx_DrawBillboardPrimEnv(Particle* particle, CameraNode* camera);
s32 ParticleGfx_DrawBillboardDoubleSided(Particle* particle, UNUSED CameraNode* camera);
void ParticleGfx_DrawBillboardDefaultQuad(Particle* particle, CameraNode* camera);
void ParticleGfx_DrawBillboardPrimEnvQuad(Particle* particle, CameraNode* camera);
void ParticleGfx_DrawBillboardTextureAlphaRotatedQuad(Particle* particle, CameraNode* camera);
void ParticleGfx_DrawBillboardOffsetQuad(Particle* particle, CameraNode* camera);
void ParticleGfx_DrawBillboardPrimEnvNoiseQuad(Particle* particle, CameraNode* camera);
void ParticleGfx_DrawBillboardPrimEnvRotatedQuad(Particle* particle, CameraNode* camera);
void ParticleGfx_DrawBillboardPrimEnvRotatedTriangle(Particle* particle, CameraNode* camera);
void ParticleGfx_DrawBillboardPrimEnvQuadVariantA(Particle* particle, CameraNode* camera);
void ParticleGfx_DrawBillboardPrimEnvQuadVariantB(Particle* particle, CameraNode* camera);
void ParticleGfx_DrawBillboardPrimEnvQuadVariantC(Particle* particle, CameraNode* camera);
void ParticleGfx_DrawBillboardPrimEnvComposite(Particle* particle, CameraNode* camera);
s32 ParticleGfx_DrawBillboardPrimEnvTriangleDoubleSided(Particle* particle, UNUSED CameraNode* camera);
void ParticleGfx_DrawBillboardPrimEnvTransformedQuad(Particle* particle, CameraNode* camera);
void ParticleGfx_CopyPaletteRgb(u8* destination, u16 palette_index);
void ParticleGfx_CopyPalettePairRgb(u8* prim_color, u8* env_color, u16 pair_index);
void ParticleGfx_RecolorPrim(Particle* particle, u16 palette_index);
void ParticleGfx_RecolorEnv(Particle* particle, u16 palette_index);
void ParticleGfx_RecolorPrimEnvFromPalettePair(Particle* particle, u16 palette_index);
void ParticleGfx_RecolorPrimFromPaletteSetA(Particle* particle, s16 palette_index);
void ParticleGfx_RecolorEnvFromPaletteSetA(Particle* particle, s16 palette_index);
void ParticleGfx_RecolorPrimFromPaletteSetB(Particle* particle, s16 palette_index);
void ParticleGfx_RecolorEnvFromPaletteSetB(Particle* particle, s16 palette_index);
void ParticleGfx_RecolorPrimEnvFromPaletteSetA(Particle* particle, s16 palette_index);
void ParticleGfx_RecolorPrimEnvFromPaletteSetB(Particle* particle, s16 palette_index);
void ParticleGfx_RecolorPrimRandomPalette(Particle* particle);
void ParticleGfx_RecolorEnvRandomPalette(Particle* particle);
s32 ParticleGfx_ApproachPrimPalette(Particle* particle, u16 palette_index, u8 step);
s32 ParticleGfx_ApproachEnvPalette(Particle* particle, u16 palette_index, u8 step);
s32 ParticleGfx_ApproachPrimEnvPalettePair(Particle* particle, u16 palette_index, u8 step);

#endif // _FRAGMENT34_H_
