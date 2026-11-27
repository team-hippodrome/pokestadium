#include "parade_debug.h"
#include "src/fragments/minigame_hud/minigame_hud.h"
#include "src/display_object_textures.h"
#include "src/geo_node.h"
#include "src/matrix.h"
#include "src/model_renderer.h"

static ParadeSpeciesInfo sParadeSpeciesInfo[7] = {
    { 0, 0, 0.0f, 0.0f },    { 1, 0x65, 1.0f, 0.0f }, { 2, 0x64, 1.0f, 0.0f }, { 3, 0x9A, 10.0f, 25.0f },
    { 4, 0x27, 1.0f, 0.0f }, { 5, 0x28, 1.0f, 0.0f }, { 6, 0x23, 1.0f, 0.0f },
};
static f32 sParadeStartX[2] = { -600.0f, 600.0f };
static s16 sParadeStartYaw[2] = { 0x4000, 0xC000 };
static s32 sParadeModelIds[6] = { 1, 2, 3, 4, 5, 6 };

static ParadeActor sParadeActors[10];
static ParadeModel sParadeModels[3];
static s32 sParadeSpawnTimer;

ParadeSpeciesInfo* Parade_FindSpeciesInfo(s32 id) {
    ParadeSpeciesInfo* info = sParadeSpeciesInfo;
    s32 i;

    for (i = 0; i < 7; i++, info++) {
        if (id == info->id) {
            break;
        }
    }

    if (i == 7) {
        info = NULL;
    }
    return info;
}

// Mode 0 clears the actor, 1 spawns a random model at a random side, 2 removes it.
void Parade_SetActorMode(s32 mode, ParadeActor* actor) {
    s32 side;
    DisplayObject* obj;
    ParadeModel* model;

    actor->state = 0;

    switch (mode) {
        case 0:
            actor->modelIndex = 0;
            actor->id = 0;
            actor->phase = 0;
            actor->scale = 0.0f;
            actor->speed = 0.0f;
            actor->position = D_8006F050;
            actor->rotation = D_8006F05C;
            ModelRenderer_AttachDisplayObject(&actor->displayObject);
            break;

        case 1:
            side = Rand_Range(0x100) & 1;
            actor->modelIndex = Rand_Range(3);
            model = &sParadeModels[actor->modelIndex];
            actor->id = model->id;
            actor->phase = 0;
            actor->scale = model->info->scale;
            actor->speed = Rand_Range(11) + 5.0f;
            actor->position.x = sParadeStartX[side];
            actor->position.y = model->info->height;
            actor->position.z = Rand_Range(500) + -600.0f;
            actor->rotation.x = 0;
            actor->rotation.y = sParadeStartYaw[side];
            actor->rotation.z = 0;
            Model_InitDisplayObject(&actor->displayObject, 0, model->info->archiveIndex, model->model->nodes[0]);
            ModelAnim_SetAnimation(&actor->displayObject, 1);
            break;

        case 2:
            model = &sParadeModels[actor->modelIndex];
            actor->id = 0;
            obj = &actor->displayObject;
            GraphNode_RemoveChild((GraphNode*)obj, (GraphNode*)model->model->nodes[0]);
            obj->node.flags &= ~1;
            break;
    }
}

ParadeActor* Parade_FindFreeActor(void) {
    ParadeActor* actor = sParadeActors;
    s32 i;

    for (i = 0; i < 10; i++, actor++) {
        if (actor->id == 0) {
            break;
        }
    }

    if (i == 10) {
        actor = NULL;
    }
    return actor;
}

void Parade_SyncDisplayObject(ParadeActor* actor) {
    actor->displayObject.position = actor->position;
    actor->displayObject.rotation = actor->rotation;
    actor->displayObject.scale.x = actor->displayObject.scale.y = actor->displayObject.scale.z = actor->scale;
}

void Parade_UpdateActor(ParadeActor* actor) {
    f32 sin;
    f32 cos;

    switch (actor->phase) {
        case 0:
            actor->phase = 1;
            break;

        case 1:
            sin = SINS(actor->rotation.y);
            cos = COSS(actor->rotation.y);
            if (actor->id == 3) {
                actor->rotation.x += 0x400;
            }
            actor->position.x += sin * actor->speed;
            actor->position.z += cos * actor->speed;
            if ((actor->position.x < -600.0f) || (600.0f < actor->position.x)) {
                Parade_SetActorMode(2, actor);
            }
            break;
    }
}

void Parade_InitActors(void) {
    ParadeActor* actor;
    s32 i;

    sParadeSpawnTimer = 1;

    for (i = 0; i < 3; i++) {
        sParadeModels[i].id = sParadeModelIds[i];
        sParadeModels[i].info = Parade_FindSpeciesInfo(sParadeModelIds[i]);
        sParadeModels[i].model = Model_LoadByArchiveIndex(sParadeModels[i].info->archiveIndex);
    }

    for (i = 0, actor = sParadeActors; i < 10; i++, actor++) {
        Parade_SetActorMode(0, actor);
    }
}

void Parade_UpdateActors(void) {
    ParadeActor* actor;
    s32 i;

    if (sParadeSpawnTimer > 0) {
        sParadeSpawnTimer--;
    }

    if (sParadeSpawnTimer == 0) {
        actor = Parade_FindFreeActor();
        if (actor != NULL) {
            sParadeSpawnTimer = 1;
            Parade_SetActorMode(1, actor);
        }
    }

    for (i = 0, actor = sParadeActors; i < 10; i++, actor++) {
        if (actor->id != 0) {
            Parade_UpdateActor(actor);
            if (actor->id != 0) {
                Parade_SyncDisplayObject(actor);
            }
        }
    }
}
