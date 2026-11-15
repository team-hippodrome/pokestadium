#ifndef _COMMON_H
#define _COMMON_H

#include <ultra64.h>
#include <PR/sched.h>

typedef unsigned int uintptr_t;
typedef int intptr_t;

#include "macros.h"
#include "versions.h"
#include "attributes.h"
#include "functions.h"
#include "sections.h"
#include "gfx.h"
#include "color.h"
#include "sound.h"
#include "string.h"
#include "math.h"

struct ModelAssetNode;

typedef union arg1_func_80010CA8 {
    struct {
        /* 0x00 */ s16 unk_00;
        /* 0x02 */ s8 unk_02;
        /* 0x03 */ s8 unk_03;
    };
    u32 raw;
} arg1_func_80010CA8; // size = 0x4

typedef struct ModelAnimationBank {
    /* 0x00 */ u16 modelId;
    /* 0x02 */ u8 unk_02;
    /* 0x03 */ u8 modelCount;
    /* 0x04 */ u8 transformCount;
    /* 0x05 */ u8 eventTrackCount;
    /* 0x06 */ char pad6[2];
    /* 0x08 */ struct ModelNodeView** nodes;
    /* 0x0C */ s32* transformTracks;
    /* 0x10 */ s32* eventTracks;
    /* 0x14 */ struct ModelAnimationBank* (*entryPoint)(s32, s32);
} ModelAnimationBank; // size = 0x18

typedef ModelAnimationBank* (*ModelBankCallback)(s32, s32);

typedef ModelAnimationBank ModelLoadResult;

typedef struct ModelLoadContext {
    /* 0x00 */ u8 flags;
    /* 0x01 */ u8 pendingLoads;
    /* 0x02 */ u8 bufferIndex;
    /* 0x04 */ s32 modelBuffers[5];
    /* 0x18 */ s32 backgroundBuffer;
    /* 0x1C */ s32 modelSize;
    /* 0x20 */ s32 backgroundSize;
    /* 0x24 */ ModelLoadResult* loadedModel;
    /* 0x28 */ s32 loadedBackground;
    /* 0x2C */ arg1_func_80010CA8 colorVariant;
    /* 0x30 */ OSMesgQueue modelQueue;
    /* 0x48 */ OSMesg modelMessage;
    /* 0x4C */ char pad4C[0x4];
    /* 0x50 */ OSMesgQueue backgroundQueue;
    /* 0x68 */ OSMesg backgroundMessage;
    /* 0x6C */ char pad6C[0x4];
} ModelLoadContext; // size = 0x70

typedef struct ModelNodeView {
    // Legacy graph-header view: preserve pointer declarations for matching.
    // The four-slot overlay is also indexed by asset callers; its full role is unresolved.
    /* 0x00 */ struct ModelNodeView* unk_00[4];
    /* 0x10 */ struct ModelAssetNode* callback;
    /* 0x14 */ ModelLoadContext* callbackArg;
} ModelNodeView; // size >= 0x18

typedef struct ModelAssetNode {
    /* 0x00 */ ModelNodeView node;
    /* 0x18 */ char unk_18[0x10];
    /* 0x28 */ ModelBankCallback getAnimationBank;
} ModelAssetNode; // size >= 0x2C

typedef struct DisplayNodeHeader {
    // Layout-compatible with GraphNode; retain this view's legacy member types.
    /* 0x00 */ u8 type;
    /* 0x01 */ u8 flags;
    /* 0x02 */ u8 renderFlags;
    /* 0x03 */ u8 renderLayer;
    /* 0x04 */ struct DisplayNodeHeader* prevSibling;
    /* 0x08 */ struct DisplayNodeHeader* nextSibling;
    /* 0x0C */ ModelAssetNode* children;
    /* 0x10 */ ModelLoadContext* callback;
    /* 0x14 */ intptr_t callbackArg; // Callback data/display-list address, or an integer selector.
} DisplayNodeHeader; // size = 0x18

typedef struct TransformAnimData {
    /* 0x00 */ s16 flags; // bit 0x2 clamps at the boundary; otherwise playback wraps
    /* 0x02 */ char unk02[0x2];
    /* 0x04 */ s16 startFrame;
    /* 0x06 */ s16 loopStart;
    /* 0x08 */ u16 channelCount;
    /* 0x0A */ u16 endFrame;
    /* 0x0C */ u32 channelTable;
    /* 0x10 */ u32 translationData;
    /* 0x14 */ u32 rotationData;
    /* 0x18 */ u32 scaleData;
} TransformAnimData; // size >= 0x1C

typedef struct TransformAnimState {
    /* 0x00 */ s16 animationId;
    /* 0x04 */ TransformAnimData* data;
    /* 0x08 */ s32 frameFixed; // signed 16.16 playback position
    /* 0x0C */ u32 speedFixed; // 16.16 step; preserve the legacy unsigned declaration
    /* 0x10 */ char unk10[0x2];
    /* 0x12 */ u16 lastRenderFrame;
} TransformAnimState; // size >= 0x14

typedef struct EventTrackData {
    /* 0x00 */ s16 flags; // bit 0x2 clamps at the boundary; otherwise playback wraps
    /* 0x02 */ char unk_02[0x2];
    /* 0x04 */ s16 startFrame;
    /* 0x06 */ s16 loopStart;
    /* 0x08 */ u16 trackCount;
    /* 0x0A */ u16 endFrame;
    /* 0x0C */ void* trackTable;
    /* 0x10 */ void* frameValues;
} EventTrackData; // size >= 0x14

typedef struct EventTrackState {
    /* 0x00 */ s16 trackId;
    /* 0x04 */ EventTrackData* data;
    /* 0x08 */ s16 frame;
    /* 0x0A */ u16 lastRenderFrame;
} EventTrackState; // size = 0xC

typedef struct ModelAnchor {
    /* 0x00 */ u16 id;
    /* 0x02 */ u8 unk_02;
    /* 0x04 */ Vec3f position;
} ModelAnchor; // size = 0x10

// Animated model instance processed by Geo_NodeModelPart (node type 0x16).
typedef struct DisplayObject {
    /* 0x000 */ DisplayNodeHeader node;
    /* 0x018 */ s16 rootId;
    /* 0x01A */ s16 objectType;
    /* 0x01C */ u8 textureMode;
    /* 0x01D */ u8 lodFraction; // gDPSetPrimColor's primitive LOD-fraction argument
    /* 0x01E */ Vec3s rotation;
    /* 0x024 */ Vec3f position;
    /* 0x030 */ Vec3f scale;
    /* 0x03C */ Color_RGBA8_u32 textureTint;
    /* 0x040 */ TransformAnimState transformAnim;
    /* 0x054 */ EventTrackState eventTrack;
    /* 0x060 */ MtxF matrix;
    /* 0x0A0 */ Color_RGBA8_u32 fogColor; // submitted through GeoRender_SetFogColor
    /* 0x0A4 */ char unk0A4[2];
    /* 0x0A6 */ u8 effectSlot; // 0/1 select effect state; 0xFF disables slot-specific work
    /* 0x0A7 */ u8 anchorCount;
    /* 0x0A8 */ ModelAnchor anchors[12];
} DisplayObject; // size = 0x168

typedef struct unk_D_86002F34_000_014_004 {
    /* 0x00 */ u8 unk_00[5];
} unk_D_86002F34_000_014_004; // size = 0x5

typedef struct unk_D_86002F34_000_014_010 {
    /* 0x00 */ u8 unk_00[4];
} unk_D_86002F34_000_014_010; // size = 0x4

typedef struct unk_D_86002F34_000_014 {
    /* 0x00 */ u8* unk_00;
    /* 0x04 */ u8* unk_04;
} unk_D_86002F34_000_014; // size >= 0x8

struct GraphNode;
typedef s32 (*GraphNodeCallback)(s32 event, struct GraphNode* node);

typedef struct GraphNode {
    /* 0x00 */ u8 type;        // Index into the geometry-node renderer dispatch table.
    /* 0x01 */ u8 flags;       // Bit 0 enables rendering; initialized to 0x11.
    /* 0x02 */ u8 renderFlags; // Type-dependent rendering options, not universal flags.
    /* 0x03 */ u8 renderLayer; // Material/render-slot selector; may be remapped by material state.
    /* 0x04 */ struct GraphNode* prevSibling;
    /* 0x08 */ struct GraphNode* nextSibling;
    /* 0x0C */ struct CameraNode* children; // Circular list of graph nodes through a legacy camera view.
    /* 0x10 */ GraphNodeCallback callback;  // Events: 0 on installation, 2 on traversal, 5 on DL submission.
    /* 0x14 */ void* callbackArg;           // Callback-owned data or selector, accessed through the node.
} GraphNode;                                // size = 0x18

// Geo_NodeCamera converts this pixel rectangle into an RSP viewport and RDP scissor.
//@src/geo_render.c
typedef struct CameraViewport {
    /* 0x00 */ Vp* rspViewport;
    /* 0x04 */ s16 x;
    /* 0x06 */ s16 y;
    /* 0x08 */ s16 width;
    /* 0x0A */ s16 height;
} CameraViewport; // size = 0xC

// Geo_NodeCamera passes these projection parameters to guPerspective.
//@src/geo_render.c
typedef struct CameraPerspective {
    /* 0x00 */ Mtx* projectionMatrix;
    /* 0x04 */ u16 unk_04;
    /* 0x08 */ f32 verticalFieldOfView; // Degrees; guPerspective converts to radians.
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 nearPlane;
    /* 0x14 */ f32 farPlane;
    /* 0x18 */ f32 unk_18;
} CameraPerspective; // size = 0x1C

// Orthographic projection block consumed by Geo_NodeCamera and GeoOverlay_Draw.
//@src/geo_render.c
typedef struct CameraOrthographic {
    /* 0x00 */ Mtx* unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ f32 unk_1C;
} CameraOrthographic; // size = 0x20

// guLookAtReflectF builds floatMatrix from these eye, target and up vectors.
// MtxF_ToFixed writes packed 16.16 words through fixedMatrix's legacy MtxF* type.
//@src/geo_render.c
//@src/matrix.c
typedef struct CameraView {
    /* 0x00 */ MtxF* fixedMatrix;
    /* 0x04 */ MtxF floatMatrix;
    /* 0x44 */ LookAt* unk_44;
    /* 0x48 */ Vec3f eyePosition;
    /* 0x54 */ Vec3f targetPosition;
    /* 0x60 */ Vec3f upDirection;
} CameraView; // size >= 0x6C

// GeoOverlay_Draw renders solid/fading color quads or textured transition quads.
// Transition progress advances once per overlay draw, rather than from a clock.
//@src/geo_render.c
typedef struct CameraOverlay {
    /* 0x00 */ u16 mode; // 0 = disabled, 1 = solid, 2/3 = fade, 4/5 = textured transition.
    /* 0x02 */ u16 unk_02;
    /* 0x04 */ Color_RGBA8_u32 color;
    /* 0x08 */ u16 stepCount;
    /* 0x0A */ u16 stepIndex; // Starts at 1; reaching stepCount selects the terminal mode.
    /* 0x0C */ s16 unk_0C;
    /* 0x0E */ s16 unk_0E;
    /* 0x10 */ s16 unk_10;
    /* 0x12 */ s16 unk_12;
    /* 0x14 */ s16 unk_14;
    /* 0x16 */ s16 unk_16;
    /* 0x18 */ s16 unk_18;
    /* 0x1A */ s16 unk_1A;
    /* 0x1C */ u8* unk_1C;
} CameraOverlay; // size >= 0x20

// Camera scene-graph node (type 2), initialized by GeoNode_CreateCamera.
// Geo_NodeCamera prepares projection/view state, traverses children and draws the overlay.
//@src/geo_node.c
//@src/geo_render.c
typedef struct CameraNode {
    /* 0x00 */ GraphNode node;
    /* 0x18 */ CameraViewport viewport;
    /* 0x24 */ CameraPerspective perspective;
    /* 0x40 */ CameraOrthographic orthographic;
    /* 0x60 */ CameraView view;
    /* 0xCC */ CameraOverlay overlay;
    /* 0xEC */ char unk_EC[0x4];
} CameraNode; // size = 0xF0

typedef struct unk_D_86002F34_alt1 {
    /* 0x00 */ GraphNode node;
    /* 0x18 */ s16 unk_18;
    /* 0x1A */ u16 unk_1A;
    /* 0x1C */ s8 unk_1C;
    /* 0x1D */ s8 unk_1D;
    /* 0x20 */ s32 unk_20;
    /* 0x24 */ Lights7* lights;
} unk_D_86002F34_alt1; // size >= 0x28

typedef struct unk_D_86002F34_alt2 {
    /* 0x00 */ GraphNode node;
    /* 0x18 */ GraphNode* unk_18;
} unk_D_86002F34_alt2; // size >= 0x1C

typedef struct unk_D_86002F34_alt3 {
    /* 0x00 */ GraphNode node;
    /* 0x18 */ s16 unk_18;
    /* 0x1A */ s16 unk_1A;
    /* 0x1C */ s32 unk_1C;
} unk_D_86002F34_alt3; // size >= 0x20

typedef struct unk_D_86002F34_alt4 {
    /* 0x00 */ GraphNode node;
    /* 0x18 */ Color_RGBA8_u32 unk_18;
    /* 0x1C */ u16 unk_1C;
    /* 0x1E */ u16 unk_1E;
} unk_D_86002F34_alt4; // size >= 0x20

typedef struct unk_D_86002F34_alt5 {
    /* 0x00 */ GraphNode node;
    /* 0x18 */ Vec3f unk_18;
    /* 0x24 */ Vec3s unk_24;
} unk_D_86002F34_alt5; // size = 0x2C

typedef struct unk_D_86002F34_alt6 {
    /* 0x00 */ GraphNode node;
    /* 0x18 */ Vec3s unk_18;
    /* 0x1E */ Vec3s unk_1E;
    /* 0x24 */ Vec3f unk_24;
    /* 0x30 */ u8 unk_30;
    /* 0x31 */ u8 unk_31;
    /* 0x32 */ s16 unk_32;
} unk_D_86002F34_alt6; // size = 0x34

typedef struct unk_D_86002F34_alt7 {
    /* 0x00 */ GraphNode node;
    /* 0x18 */ Gfx* unk_18;
    /* 0x1C */ s16 unk_1C;
} unk_D_86002F34_alt7; // size = 0x20

typedef struct unk_D_86002F34_alt8 {
    /* 0x00 */ GraphNode node;
    /* 0x18 */ Gfx* unk_18;
    /* 0x1C */ MtxF unk_1C;
} unk_D_86002F34_alt8; // size = 0x5C

typedef struct unk_D_86002F34_alt9 {
    /* 0x00 */ GraphNode node;
    /* 0x18 */ Gfx* unk_18;
    /* 0x1C */ Vec3f unk_1C;
    /* 0x28 */ f32 unk_28;
} unk_D_86002F34_alt9; // size = 0x2C

typedef struct unk_D_86002F34_alt10 {
    /* 0x00 */ GraphNode node;
    /* 0x18 */ Gfx* unk_18;
    /* 0x1C */ s16 unk_1C;
    /* 0x1E */ s16 unk_1E;
    /* 0x20 */ s16 unk_20;
    /* 0x22 */ u8 unk_22;
    /* 0x24 */ Color_RGBA8_u32 unk_24;
} unk_D_86002F34_alt10; // size >= 0x28

typedef struct unk_D_86002F34_alt11_018 {
    /* 0x00 */ u8 fmt;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ char unk04[0x4];
    /* 0x08 */ u8* texture;
} unk_D_86002F34_alt11_018; // size >= 0xC

typedef struct unk_D_86002F34_alt11 {
    /* 0x00 */ GraphNode node;
    /* 0x18 */ unk_D_86002F34_alt11_018* unk_18;
    /* 0x1C */ unk_D_86002F34_alt11_018* unk_1C;
} unk_D_86002F34_alt11; // size >= 0x20

typedef struct unk_D_86002F34_alt12 {
    /* 0x00 */ GraphNode node;
    /* 0x18 */ s16 unk_18;
    /* 0x1A */ s16 unk_1A;
    /* 0x1C */ s16 unk_1C;
    /* 0x1E */ s16 unk_1E;
} unk_D_86002F34_alt12; // size = 0x20

typedef struct unk_D_86002F34_alt13 {
    /* 0x00 */ GraphNode node;
    /* 0x18 */ s16 unk_18;
} unk_D_86002F34_alt13; // size = 0x1C

typedef struct unk_D_86002F34_alt14 {
    /* 0x00 */ GraphNode node;
} unk_D_86002F34_alt14; // size = 0x18

typedef struct unk_D_86002F34_alt15 {
    /* 0x00 */ GraphNode node;
} unk_D_86002F34_alt15; // size = 0x18

typedef struct unk_D_86002F34_alt16 {
    /* 0x00 */ GraphNode node;
    /* 0x18 */ u32 unk_18;
} unk_D_86002F34_alt16; // size = 0x1C

typedef struct unk_D_86002F34_alt17 {
    /* 0x00 */ GraphNode node;
} unk_D_86002F34_alt17; // size = 0x18

typedef struct unk_D_86002F34_alt18 {
    /* 0x00 */ GraphNode node;
    /* 0x18 */ u8 r;
    /* 0x19 */ u8 g;
    /* 0x1A */ u8 b;
    /* 0x1B */ u8 a;
    /* 0x1C */ s16 unk_1C;
    /* 0x1E */ s16 unk_1E;
} unk_D_86002F34_alt18; // size = 0x20

typedef struct unk_D_86002F34_alt19 {
    /* 0x00 */ GraphNode node;
    /* 0x18 */ u8 r;
    /* 0x19 */ u8 g;
    /* 0x1A */ u8 b;
} unk_D_86002F34_alt19; // size = 0x1C

typedef struct unk_D_86002F34_alt20 {
    /* 0x00 */ GraphNode node;
    /* 0x18 */ s16 unk_18;
    /* 0x1A */ s16 unk_1A;
} unk_D_86002F34_alt20; // size = 0x1C

typedef struct unk_D_86002F34_alt21 {
    /* 0x00 */ GraphNode node;
    /* 0x18 */ s16 unk_18;
    /* 0x1A */ s16 unk_1A;
} unk_D_86002F34_alt21; // size = 0x1C

typedef struct unk_D_86002F34_alt22 {
    /* 0x00 */ GraphNode node;
    /* 0x18 */ Vec3f unk_18;
} unk_D_86002F34_alt22; // size = 0x24

typedef struct unk_D_86002F34_alt23 {
    /* 0x00 */ GraphNode node;
    /* 0x18 */ s16 unk_18;
} unk_D_86002F34_alt23; // size = 0x1C

typedef struct unk_D_86002F34_alt24 {
    /* 0x00 */ GraphNode node;
    /* 0x18 */ s16 unk_18;
    /* 0x1A */ s16 unk_1A;
    /* 0x1C */ s16 unk_1C;
    /* 0x1E */ s16 unk_1E;
    /* 0x20 */ s16 unk_20;
    /* 0x22 */ s16 unk_22;
    /* 0x24 */ s32 unk_24;
    /* 0x28 */ Color_RGBA8_u32 unk_28;
} unk_D_86002F34_alt24; // size = 0x2C

typedef void* (*func_unk_D_800ABE10)(s32, s32);

#define unk_D_86002F34_018_GFX_TYPE_1 1 // 8-bit per texel
#define unk_D_86002F34_018_GFX_TYPE_2 2 // 16-bit per texel
#define unk_D_86002F34_018_GFX_TYPE_3 3 // 32-bit per texel

typedef struct unk_D_86002F34_018 {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 type;
    /* 0x02 */ s16 width;
    /* 0x04 */ s16 height;
    /* 0x06 */ s16 num_texels;
    /* 0x08 */ u32* texture;
} unk_D_86002F34_018; // size = 0xC

typedef struct unk_D_86002F34_01C {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s16* unk_04;
    /* 0x08 */ char unk08[0x4];
} unk_D_86002F34_01C; // size = 0xC

typedef struct unk_D_86002F34 {
    /* 0x00 */ GraphNode node;
    /* 0x18 */ unk_D_86002F34_018* unk_18;
    /* 0x1C */ unk_D_86002F34_01C* unk_1C;
    /* 0x20 */ Vtx* unk_20;
    /* 0x24 */ u8 unk_24;
    /* 0x25 */ u8 unk_25;
    /* 0x26 */ s16 unk_26;
    /* 0x28 */ func_unk_D_800ABE10 unk_28;
} unk_D_86002F34; // size = 0x2C

typedef ModelAnimationBank unk_D_86002F30;

typedef struct GbTowerSessionParams {
    /* 0x00 */ s16 buttonBindings;
    /* 0x02 */ u8 unlockTier;
    /* 0x03 */ u8 partyCount;
    /* 0x04 */ u16 (*partyIcons)[6][0x640];
} GbTowerSessionParams; // size = 0x8

typedef struct unk_D_800AA660 {
    /* 0x0000 */ OSThread thread;
    /* 0x01B0 */ char unk01B0[0x10];
    /* 0x01C0 */ OSMesgQueue queue1;
    /* 0x01D8 */ struct unk_D_800AA660* nextClient;
    /* 0x01DC */ s32 unk_01DC;
    /* 0x01E0 */ char stack[0x2000];
    /* 0x21E0 */ OSMesg mesg;
    /* 0x21E4 */ OSMesgQueue queue2;
    /* 0x21FC */ u8* font1;
    /* 0x2200 */ u8* font2;
    /* 0x2204 */ GbTowerSessionParams session;
    /* 0x220C */ char unk220C[4];
} unk_D_800AA660; // size = 0x2210

typedef struct unk_D_864027C0 {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ s16 unk_18;
    /* 0x1A */ s16 unk_1A;
    /* 0x1C */ s16 unk_1C;
} unk_D_864027C0; // size = 0x20

typedef struct ParticleCombineMux {
    /* 0x00 */ u8 colorA0;
    /* 0x01 */ u8 colorB0;
    /* 0x02 */ u8 colorC0;
    /* 0x03 */ u8 colorD0;
    /* 0x04 */ u8 alphaA0;
    /* 0x05 */ u8 alphaB0;
    /* 0x06 */ u8 alphaC0;
    /* 0x07 */ u8 alphaD0;
    /* 0x08 */ u8 colorA1;
    /* 0x09 */ u8 colorB1;
    /* 0x0A */ u8 colorC1;
    /* 0x0B */ u8 colorD1;
    /* 0x0C */ u8 alphaA1;
    /* 0x0D */ u8 alphaB1;
    /* 0x0E */ u8 alphaC1;
    /* 0x0F */ u8 alphaD1;
} ParticleCombineMux; // size >= 0x10

typedef struct func_88002628_sp38 {
    /* 0x000 */ OSThread thread;
    /* 0x1B0 */ char unk1B0[0x10];
} func_88002628_sp38; // size = 0x1C0

typedef struct unk_func_88500020 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ u8* unk_0C;
} unk_func_88500020; // size = 0x10

// offset 0x898000 in the rom
typedef struct TrainerData {
    /* 0x000 */ char unk00[0x4];
    /* 0x004 */ char name1[12];
    /* 0x010 */ char name2[40];
    /* 0x038 */ u16 gfx_file_index; // Big-endian ROM halfword; high byte indexes battle_portraits, low byte unknown.
    /* 0x03A */ char unk3A[0x1F6];
} TrainerData; // size = 0x230

// GB Emulator State
typedef struct unk_D_8122B2C0 {
    /* 0x00000 */ u8 oamIoHram[0x208]; // GB 0xFE00-0xFFFF: OAM 0x00-0x9F, IO 0x100-0x17F, HRAM 0x180-0x1FE, IE 0x1FF
    /* 0x00208 */ u8 cgbBgPaletteRam[0x40]; // written by the FF69 (BCPD) handler at BCPS & 0x3F
    /* 0x00248 */ u8 cgbObjPaletteRam[0x40]; // written by the FF6B (OCPD) handler at OCPS & 0x3F
    /* 0x00288 */ u8 unk_288[0x100]; // SGB palettes at 0x308 (u16[4][4]) and SGB attribute map at 0x328 (90 bytes)
    /* 0x00388 */ u8 vramTileCached[0x400]; // per 16-byte VRAM tile: VRAM writes clear it, the tile converter sets 0xFF
    /* 0x00788 */ u8 vram[0x4000]; // 0x8000-0x9FFF, two CGB banks
    /* 0x04788 */ u8 opcodeTables[0xC00]; // CB-prefix handler table (256 pointers), then 256 x {cycles, length, rst, pad, handler}
    /* 0x05388 */ s32 rspTaskDone; // 0x400 (SP_STATUS_SIG3) once the RSP has copied the frame, 0 on submit
    /* 0x0538C */ s32 frameCounter;
    /* 0x05390 */ u32 cartridgeFeatures; // sCartridgeTypeFeatures[header 0x147]
    /* 0x05394 */ u32 romSize; // bytes, from header 0x148
    /* 0x05398 */ u32 ramSize; // bytes, from header 0x149
    /* 0x0539C */ u32 unk_539C; // 0x25800 (320 * 240 * 2) at reset; sent to the RSP task
    /* 0x053A0 */ char unk053A0[0x4];
    /* 0x053A4 */ u32 unk_53A4;
    /* 0x053A8 */ void* unk_53A8;
    /* 0x053AC */ void* unk_53AC;
    /* 0x053B0 */ void* unk_53B0;
    /* 0x053B4 */ void* tileCache; // 0x6000 bytes of converted tiles, DMA'd by the RSP renderer
    /* 0x053B8 */ void* sgbPacketState; // continuation of the SGB joypad packet receiver
    /* 0x053BC */ void* romImage; // 1 MB cartridge ROM copy; also the 0x0000-0x3FFF page base
    /* 0x053C0 */ s32 unk_53C0; // APU timestamp base (cpu agent, Mild)
    /* 0x053C4 */ char unk053C4[0x12];
    /* 0x053D6 */ s16 cyclesPerLine;
    /* 0x053D8 */ char unk053D8[0x4];
    /* 0x053DC */ u16 lineLogOffset;
    /* 0x053DE */ u16 linesRemaining;
    /* 0x053E0 */ u16 savedStoreAddr;
    /* 0x053E2 */ u16 savedSp;
    /* 0x053E4 */ u16 regF; // lazy flag format
    /* 0x053E6 */ u16 regHL;
    /* 0x053E8 */ u16 regPC;
    /* 0x053EA */ u16 regSP;
    /* 0x053EC */ char unk053EC[0x2];
    /* 0x053EE */ u16 unk_53EE;
    /* 0x053F0 */ u16 unk_53F0;
    /* 0x053F2 */ u16 buttonsHeld; // N64 layout, with START -> 0x1000 and SELECT -> 0x2000 remapped
    /* 0x053F4 */ u16 joypadMatrix;
    /* 0x053F6 */ u16 buttonsPressed; // raw N64 buttons pressed this frame
    /* 0x053F8 */ char unk053F8[0x4];
    /* 0x053FC */ s8 borderFadeLevel; // |sBorderFade|, sent to the RSP task
    /* 0x053FD */ s8 gameScreenFadeLevel; // 0x40 while playing, fades to 0 when the pause menu opens; sent to the RSP task
    /* 0x053FE */ u8 unk_53FE;
    /* 0x053FF */ u8 sgbPlayerIndex;
    /* 0x05400 */ u8 sgbMultiplayerMode; // MLT_REQ
    /* 0x05401 */ char unk05401[0x1];
    /* 0x05402 */ u8 sgbMaskMode; // MASK_EN
    /* 0x05403 */ u8 sgbTransferPending;
    /* 0x05404 */ u8 unk_5404; // nonzero triggers GbSgb_RenderBorder
    /* 0x05405 */ u8 sgbPacketBuffer[0x80];
    /* 0x05485 */ u8 dmgMode; // 1 unless header 0x143 bit 7 (CGB) is set; selects the DMG RSP renderer
    /* 0x05486 */ char unk05486[0x1];
    /* 0x05487 */ u8 regA;
    /* 0x05488 */ u8 regB;
    /* 0x05489 */ u8 regC;
    /* 0x0548A */ u8 regD;
    /* 0x0548B */ u8 regE;
    /* 0x0548C */ char unk0548C[0x1];
    /* 0x0548D */ u8 invalidOpcodeHit; // set by GbCpu_HandleReservedOpcode; ends the session with code 3
    /* 0x0548E */ u8 speedMultiplier; // 1, 2 or 4; the CPU runs speedMultiplier * 154 scanlines per frame
    /* 0x0548F */ char unk0548F[0x1];
    /* 0x05490 */ u8 apuTimeDivisor; // copy of speedMultiplier each frame; APU write timestamps are divided by it
    /* 0x05491 */ u8 mbcRomBank;
    /* 0x05492 */ u8 mbcRamBank;
    /* 0x05493 */ u8 mbcBankingMode;
    /* 0x05494 */ u8 romBankMask;
    /* 0x05495 */ u8 ramBankMask;
    /* 0x05496 */ char unk05496[0x3];
    /* 0x05499 */ u8 timerShift; // from TAC
    /* 0x0549A */ u16 paletteFadeState;
    /* 0x0549C */ volatile u8 romBankStatus[0x100]; // per 16 KB ROM bank, 0xFF once loaded
    /* 0x0559C */ volatile u8 ramBankStatus[0x10]; // per 8 KB RAM bank, 0xFF once loaded
    /* 0x055AC */ u8 readPageTable[0x40]; // 16 host base pointers, one per 4 KB GB page
    /* 0x055EC */ u8 writePageHandlers[0x40]; // 16 write handlers, one per 4 KB GB page
    /* 0x0562C */ u8 ioWriteHandlers[0x200]; // 0x80 handlers for 0xFF00-0xFF7F
    /* 0x0582C */ u8 sramPageSkipSync[0x200]; // per cart RAM page: 0 = written back to the pak, 0xFE = never written back (Pokemon carts: pages 0-4)
    /* 0x05A2C */ u8 sramPageSyncState[0x200]; // per 256-byte cart RAM page: 0xFF unmodified since load, emulated writes clear to 0, 1 write queued, 2 written back
    /* 0x05C2C */ u32 savedHostRegs[0xB]; // a0-a3, v0, v1, resume pc, ra, t7, t8, t9
    /* 0x05C58 */ void* unk_5C58;
    /* 0x05C5C */ OSGbpakId cartridgeId; // header read at startup; later reads (gbpakId) must match
    /* 0x05CAC */ OSGbpakId gbpakId;
    /* 0x05CFC */ OSPfs* pfs;
    /* 0x05D00 */ char unk05D00[0x64];
    /* 0x05D64 */ u8* transferBuffer;
    /* 0x05D68 */ u32 gbAddress;
    /* 0x05D6C */ s32 transferSize;
    /* 0x05D70 */ u32 unk_5D70[4]; // [0] ROM checksum sum, [1] pak bank list, [2] stored bank list, [3] stored image offset
    /* 0x05D80 */ u32 probePattern[4]; // 32-byte RAM probe block with unk_5D90
    /* 0x05D90 */ u32 unk_5D90[4];
    /* 0x05DA0 */ u16 probeAddress;
    /* 0x05DA2 */ u16 pakRomBank;
    /* 0x05DA4 */ u8 romSignature[0x20]; // 32 bytes read from ROM 0x9C000, checked before fast boot
    /* 0x05DC4 */ u8 probeValid;
    /* 0x05DC5 */ u8 cartridgeVersion; // GbPak_DetectSupportedCartridgeVariant: 1 Red, 2 Green, 3 Blue, 4 Yellow
    /* 0x05DC6 */ u8 unk_5DC6; // 1 for Red, 0 for Blue/Yellow; gates delta images in GbEmu_LoadStoredRomBanks
    /* 0x05DC7 */ u8 pakRequest; // command for GbPak_ServiceThread
    /* 0x05DC8 */ volatile u8 pakBusy;
    /* 0x05DC9 */ volatile u8 pakState; // 2 ready, 8-11 error states
    /* 0x05DCA */ volatile u8 pakConnected;
    /* 0x05DCB */ u8 status;
    /* 0x05DCC */ u8 pakPort;
    /* 0x05DCD */ s8 pakRetryBudget;
    /* 0x05DCE */ u8 pakRamBank;
    /* 0x05DCF */ u8 pakMbcMode;
    /* 0x05DD0 */ u8 pakIdleCountdown; // set to 8 after a RAM page write; the pak is powered off at 0
    /* 0x05DD1 */ char unk05DD1[0x3];
    /* 0x05DD4 */ u16 hdmaLength;
    /* 0x05DD6 */ u16 hdmaDst;
    /* 0x05DD8 */ u16 hdmaSrc;
    /* 0x05DDA */ char unk05DDA[0x16];
    /* 0x05DF0 */ u8 wram[0x8000];
    /* 0x0DDF0 */ u8 sgbBorderChr[0x2000]; // CHR_TRN
    /* 0x0FDF0 */ u8 sgbBorderMap[0x800]; // PCT_TRN
    /* 0x105F0 */ u8 sgbBorderPalettes[0x80];
    /* 0x10670 */ char unk10670[0x780];
    /* 0x10DF0 */ u8 sgbSystemPalettes[0x1000]; // PAL_TRN
    /* 0x11DF0 */ u8 sgbAttrFiles[0x1000]; // ATTR_TRN
    /* 0x12DF0 */ u8 cartRam[0x8000]; // accessed as (u8*)emu + 0x12DF0
    /* 0x1ADF0 */ u8 sgbAttrMapUnpacked[360];
    /* 0x1AF58 */ char unk1AF58[0x100];
} unk_D_8122B2C0; // size = 0x1B058

#define GB_HEADER(emu, offset) (((u8*)(emu)->romImage)[offset])

extern s16 gCamDistances[];

#include "variables.h"

#endif
