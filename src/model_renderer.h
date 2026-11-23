#ifndef _1C720_H_
#define _1C720_H_

#include "global.h"
#include "src/game_state.h"

typedef struct unk_func_8001C014 {
    /* 0x00 */ char unk_00[0x2];
    /* 0x02 */ u16 unk_02;
    /* 0x04 */ s8 unk_04[0x10];
    /* 0x14 */ s8 unk_14[0x10];
} unk_func_8001C014; // size >= 0x24

void ModelRenderer_InitDisplayRoots(void);
void ModelRenderer_AttachDisplayObject(DisplayObject* display_object);
void ModelRenderer_AttachSecondaryDisplayObject(DisplayObject* arg0);
void Model_InitDisplayObject(DisplayObject* display_object, s16 root_id, s16 object_type, ModelNodeView* child);
void ModelRenderer_ClearDisplayObject(DisplayObject* arg0);
s32 ModelAnim_SetAnimation(DisplayObject* display_object, s32 id);
s32 ModelAnim_SetEventTrack(DisplayObject* display_object, s32 track_id);
void Model_SetMaterialColor(DisplayObject* display_object, u8 r, u8 g, u8 b, u8 alpha);
void Model_SetMaterialAlpha(DisplayObject* arg0, u8 arg1);
void Model_SetMaterialTextureMode(DisplayObject* display_object, u8 texture_mode);
ModelAnimationBank* ModelAnim_GetAnimationRecord(DisplayObject* arg0, s32 arg1);
arg1_func_80010CA8* Model_ComputeNicknameColorVariant(arg1_func_80010CA8* arg0, BattleMon* arg1);
void Model_ComputeNicknameColorVariantFromRecord(arg1_func_80010CA8* arg0, unk_func_8001C014* arg1, u16 arg2);
void Trainer_RequestPokeIcon(BattleSessionTeams* arg0);
void PokeIcon_PollFrameLoadForObject(BattleSessionTeams* arg0);
void PokeIcon_WaitFrameLoadForObject(BattleSessionTeams* arg0);
void PokeIcon_RequestBackgroundForObject(BattleSessionTeams* arg0);
void PokeIcon_PollBackgroundForObject(BattleSessionTeams* arg0);

#endif // _1C720_H_
