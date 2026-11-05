#include "particle_math.h"
#include "src/geo_render.h"

/*
 * Pool allocation, effect state, motion terms, and draw dispatch for particles.
 * A slot can remain allocated with lifecycle zero. Freeing is a separate action.
 * Helpers returning completion do not all use the same timer boundary.
 */
ParticleDescriptor sDefaultParticleDescriptor = { 3, Particle_NoopDraw };

/* Initialize lifecycle, motion parameters, rotation, timers, and white colors without clearing the entire allocation. */
void Particle_ResetDefaults(Particle* particle) {
    Particle_InitLifecycle(particle);
    Particle_SetPosition(particle, 0.0f, 0.0f, 0.0f);
    Particle_SetOrigin(particle, 0.0f, 0.0f, 0.0f);
    Particle_SetMotionOffset(particle, 0.0f, 0.0f, 0.0f);
    Particle_SetMotionOffsetTarget(particle, 0.0f, 0.0f, 0.0f);
    Particle_SetMotionStep(particle, 0.0f, 0.0f, 0.0f);
    Particle_StepMagnitude_SetScaled(particle, 0.0f);
    Particle_MagnitudeStep_SetScaled(particle, 0.0f);
    Particle_SetRotation(particle, 0, 0, 0);
    Particle_SetRotationSpeed(particle, 0, 0, 0);
    Particle_SetTextureFrame(particle, 0);
    Particle_FieldA6_SetS16_0BCA8(particle, 0);
    Particle_FieldA8_SetS16_0BE14(particle, 0);
    Particle_FieldCD_SetS16_0BF38(particle, 0);
    Particle_FieldCE_SetS16_0BF24(particle, 0);
    Particle_FieldAA_SetS16_0BF10(particle, 0);
    Particle_FieldAC_SetS16_0BED8(particle, 0);
    Particle_FieldCF_SetU8_0BEEC(particle, 0);
    Particle_SetPrimColor(particle, 0xFF, 0xFF, 0xFF);
    Particle_SetEnvColor(particle, 0xFF, 0xFF, 0xFF);
    Particle_SetPrimAlpha(particle, 0xFF);

    particle->scale = 1.0f;
    particle->unk_28 = 1.0f;
}

/* Search the 300-slot pool from the rolling index, initialize a free slot, or return (Particle*)-1. */
Particle* Particle_Alloc(void) {
    s32 i;
    s32 var_v0 = sParticlePoolIndex;
    Particle* var_a0 = &sParticlePool[var_v0];

    for (i = 0; i < 0x12C; i++) {
        if (var_a0->allocated == 0) {
            break;
        }

        var_v0++;
        if (var_v0 >= 0x12C) {
            var_v0 = 0;
            var_a0 = &sParticlePool[0];
        } else {
            var_a0++;
        }
    }

    if (i >= 0x12C) {
        var_a0 = (Particle*)-1;
    } else {
        var_a0->allocated = 1;

        Particle_ResetDefaults(var_a0);

        sParticlePoolIndex = var_v0 + 1;
        if (sParticlePoolIndex >= 0x12C) {
            sParticlePoolIndex = 0;
        }
    }

    return var_a0;
}

/* Unlink the particle from its neighbors, clear its links, and release its pool allocation. */
void Particle_Free(Particle* particle) {
    if (particle->prev != NULL) {
        particle->prev->next = particle->next;
    }

    if (particle->next != NULL) {
        particle->next->prev = particle->prev;
    }

    particle->prev = NULL;
    particle->next = NULL;

    particle->allocated = 0;
}

/* Allocate one particle using the shared pool allocator. */
Particle* Particle_New(void) {
    return Particle_Alloc();
}

/* Allocate a linked chain, returning its first particle even if a later allocation fails. */
Particle* Particle_AllocChain(u8 count) {
    s32 i;
    Particle* sp28;
    Particle* temp_v0;
    Particle* var_s0;

    sp28 = Particle_Alloc();

    if ((u32)sp28 != -1) {
        var_s0 = sp28;
        count--;

        for (i = 0; i < count; i++) {
            temp_v0 = Particle_Alloc();
            if ((u32)temp_v0 == -1) {
                break;
            }
            var_s0->next = temp_v0;
            temp_v0->prev = var_s0;
            var_s0 = temp_v0;
        }
    }

    return sp28;
}

/* Allocate a particle and copy its origin, rotation, callbacks, and per-emission metadata. */
Particle* Particle_CreateAtTransform(f32 unk_28_value, Vec3f origin, Vec3s rotation, ParticleUpdateCallback update_callback, ParticleDescriptor* descriptor,
                              s16 index, s16 anchor_id, s16 metadata) {
    Particle* sp1C = Particle_New();

    if ((u32)sp1C != -1) {
        sp1C->unk_28 = unk_28_value;

        ParticleMath_CopyVec3f(&sp1C->origin, &origin);
        ParticleMath_CopyVec3s(&sp1C->rotation, &rotation);

        sp1C->updateCallback = update_callback;
        sp1C->descriptor = descriptor;
        sp1C->pokemon = 0;
        sp1C->displayObject = 0;
        sp1C->unk_AC = index;
        sp1C->unk_CE = metadata;
        sp1C->unk_CD = anchor_id;
    }
    return sp1C;
}

/* Allocate at an object's position or selected anchor, copying its rotation and emission metadata. */
Particle* Particle_CreateFromObjectTransform(DisplayObject* object, ParticleUpdateCallback update_callback, ParticleDescriptor* descriptor,
                              s16 index, s16 anchor_id, s16 metadata) {
    Particle* sp1C = Particle_New();

    if ((u32)sp1C != -1) {
        if (anchor_id == 0xFF) {
            sp1C->positionOffset.x = object->position.x;
            sp1C->positionOffset.y = object->position.y;
            sp1C->positionOffset.z = object->position.z;
        } else {
            GeoRender_FindAnchorPosition(object, anchor_id, &sp1C->positionOffset);
        }

        sp1C->rotation.x = object->rotation.x;
        sp1C->rotation.y = object->rotation.y;
        sp1C->rotation.z = object->rotation.z;

        sp1C->updateCallback = update_callback;
        sp1C->descriptor = descriptor;

        sp1C->pokemon = 0;
        sp1C->displayObject = 0;

        sp1C->unk_AC = index;
        sp1C->unk_CE = metadata;
        sp1C->unk_CD = anchor_id;
    }
    return sp1C;
}

/* Create indexed particles at one transform, stopping when the pool is exhausted. */
void Particle_EmitBurstAtTransform(f32 unk_28_value, Vec3f origin, Vec3s rotation, ParticleUpdateCallback update_callback, ParticleDescriptor* descriptor, s16 count) {
    s32 i;

    for (i = 0; i < count; i++) {
        if ((u32)Particle_CreateAtTransform(unk_28_value, origin, rotation, update_callback, descriptor, i, 0xFF, NULL) == -1) {
            break;
        }
    }
}

/* Create indexed particles at one transform with shared extra metadata, stopping on allocation failure. */
void Particle_EmitBurstAtTransformWithMetadata(f32 unk_28_value, Vec3f origin, Vec3s rotation, ParticleUpdateCallback update_callback, ParticleDescriptor* descriptor, s16 count,
                   s16 metadata) {
    s32 i;

    for (i = 0; i < count; i++) {
        if ((u32)Particle_CreateAtTransform(unk_28_value, origin, rotation, update_callback, descriptor, i, 0xFF, metadata) == -1) {
            break;
        }
    }
}

/* Create indexed particles at an object or its selected anchor until the count or pool limit is reached. */
void Particle_EmitBurstFromObject(DisplayObject* object, ParticleUpdateCallback update_callback, ParticleDescriptor* descriptor, s16 count,
                   s16 anchor_id) {
    s32 i;

    for (i = 0; i < count; i++) {
        if ((u32)Particle_CreateFromObjectTransform(object, update_callback, descriptor, i, anchor_id, 0) == -1) {
            break;
        }
    }
}

/* Create indexed object/anchor particles with shared extra metadata until allocation fails or the count is reached. */
void Particle_EmitBurstFromObjectWithMetadata(DisplayObject* object, ParticleUpdateCallback update_callback, ParticleDescriptor* descriptor, s16 count,
                   s16 anchor_id, s16 metadata) {
    s32 i;

    for (i = 0; i < count; i++) {
        if ((u32)Particle_CreateFromObjectTransform(object, update_callback, descriptor, i, anchor_id, metadata) == -1) {
            break;
        }
    }
}

/* Count allocated pool slots, regardless of their lifecycle or rendering flags. */
s32 Particle_CountActive(void) {
    s32 i;
    s32 var_v1 = 0;
    Particle* ptr = sParticlePool;

    for (i = 0; i < 0x12C; i++, ptr++) {
        if (ptr->allocated != 0) {
            var_v1++;
        }
    }

    return var_v1;
}

/* Return the allocated-slot count through the shared counter. */
s32 Particle_CountActiveAlias(void) {
    return Particle_CountActive();
}

/* Start lifecycle 1 with a no-op descriptor, null associations, phase zero, and rendering enabled. */
void Particle_InitLifecycle(Particle* particle) {
    particle->lifecycle = 1;
    particle->descriptor = &sDefaultParticleDescriptor;
    particle->updateCallback = NULL;
    particle->pokemon = 0;
    particle->displayObject = 0;
    particle->phase = 0;
    Particle_SetFlags(particle, 4);
}

/* Set lifecycle to zero without releasing the pool slot. */
void Particle_MarkInactive(Particle* particle) {
    particle->lifecycle = 0;
}

/* Stop the lifecycle, clear the callback and descriptor, and reset the phase. */
void Particle_DisableAndDetach(Particle* particle) {
    particle->lifecycle = 0;

    particle->updateCallback = NULL;
    particle->descriptor = NULL;

    Particle_ResetPhase(particle);
}

/* Return whether lifecycle is zero; allocation is tracked separately. */
s32 Particle_IsInactive(Particle* particle) {
    s32 var_v1 = 0;

    if (particle->lifecycle == 0) {
        var_v1 = 1;
    }
    return var_v1;
}

/* Return whether lifecycle has reached 2 or later. */
s32 Particle_IsRunning(Particle* particle) {
    s32 var_v1 = 0;

    if (particle->lifecycle >= 2) {
        var_v1 = 1;
    }
    return var_v1;
}

/* Clear the frame-animation initialization bit and advance the lifecycle. */
void Particle_AdvanceLifecycle(Particle* particle) {
    Particle_ClearFlags(particle, 1);
    particle->lifecycle++;
}

/* Initialize once under flag 0x80, decrement immediately, and finish at zero while clearing that flag. */
s32 Particle_TickDelayTimer(Particle* particle, s16 duration) {
    s32 sp1C = 0;

    if (Particle_LacksFlags(particle, 0x80) != 0) {
        Particle_SetFlags(particle, 0x80);
        particle->delayTimer = duration;
    }

    particle->delayTimer--;
    if (particle->delayTimer <= 0) {
        particle->delayTimer = 0;
        Particle_ClearFlags(particle, 0x80);
        sp1C = 1;
    }

    return sp1C;
}

/* Tick the delay timer and advance lifecycle when it expires. */
s32 Particle_TickDelayAndAdvanceLifecycle(Particle* particle, s16 duration) {
    s32 sp1C = Particle_TickDelayTimer(particle, duration);

    if (sp1C != 0) {
        Particle_AdvanceLifecycle(particle);
    }
    return sp1C;
}

/* Set the selected particle flag bits. */
void Particle_SetFlags(Particle* particle, s32 mask) {
    particle->flags |= mask;
}

/* Clear the selected particle flag bits. */
void Particle_ClearFlags(Particle* particle, s32 mask) {
    particle->flags &= ~mask;
}

/* Toggle the selected particle flag bits. */
void Particle_ToggleFlags(Particle* particle, s32 mask) {
    particle->flags ^= mask;
}

/* Return whether any selected flag bit is set. */
s32 Particle_HasFlags(Particle* particle, s32 mask) {
    s32 var_v1 = 0;

    if (particle->flags & mask) {
        var_v1 = 1;
    }
    return var_v1;
}

/* Return whether all selected flag bits are clear. */
s32 Particle_LacksFlags(Particle* particle, s32 mask) {
    s32 var_v1 = 0;

    if (!(particle->flags & mask)) {
        var_v1 = 1;
    }
    return var_v1;
}

/* Set flag 4, which gates drawing in both particle render passes. */
void Particle_EnableRendering(Particle* particle) {
    Particle_SetFlags(particle, 4);
}

/* Clear flag 4 to suppress drawing without freeing the particle. */
void Particle_DisableRendering(Particle* particle) {
    Particle_ClearFlags(particle, 4);
}

/* Test flag 4, the per-particle drawing gate. */
s32 Particle_IsRenderingEnabled(Particle* particle) {
    return Particle_HasFlags(particle, 4);
}

/* Set flag 0x40; its effect-specific interpretation is unresolved. */
void Particle_EnableFlag40(Particle* particle) {
    Particle_SetFlags(particle, 0x40);
}

/* Clear flag 0x40 without changing the other flags. */
void Particle_DisableFlag40(Particle* particle) {
    Particle_ClearFlags(particle, 0x40);
}

/* Evaluate flag 0x40 and discard the result; the original void interface returns no test value. */
void Particle_HasFlag40(Particle* particle) {
    Particle_HasFlags(particle, 0x40);
}

/* Store the frame index used by the particle-indexed texture callbacks. */
void Particle_SetTextureFrame(Particle* particle, s16 frame) {
    particle->textureFrame = frame;
}

/* Run forward texture-frame animation with a frame timer and loop counter; return 1 on completion. */
s32 Particle_AnimateFramesForward(Particle* particle, s16 first_frame, s16 last_frame, s16 frame_step, s16 frame_delay, s16 loop_count) {
    s32 sp1C = 0;

    if (Particle_LacksFlags(particle, 1) != 0) {
        particle->frameTimer = frame_delay;
        particle->textureFrame = first_frame;
        particle->loopsRemaining = loop_count;
        Particle_SetFlags(particle, 1);
    } else {
        particle->frameTimer--;
        if (particle->frameTimer <= 0) {
            particle->frameTimer = frame_delay;

            particle->textureFrame += frame_step;
            if (last_frame < particle->textureFrame) {
                if (particle->loopsRemaining > 0) {
                    particle->loopsRemaining--;
                }

                if (particle->loopsRemaining == 0) {
                    particle->textureFrame = last_frame;
                    sp1C = 1;
                } else {
                    particle->textureFrame = first_frame;
                }
            }
        }
    }
    return sp1C;
}

/* Run the corresponding frame animation and advance lifecycle when it reports completion. */
s32 Particle_AnimateFramesForwardAndAdvanceLifecycle(Particle* particle, s16 first_frame, s16 last_frame, s16 frame_step, s16 frame_delay, s16 loop_count) {
    s32 sp24 = Particle_AnimateFramesForward(particle, first_frame, last_frame, frame_step, frame_delay, loop_count);

    if (sp24 != 0) {
        Particle_AdvanceLifecycle(particle);
    }

    return sp24;
}

/* Run reverse texture-frame animation with a frame timer and loop counter; return 1 on completion. */
s32 Particle_AnimateFramesReverse(Particle* particle, s16 first_frame, s16 last_frame, s16 frame_step, s16 frame_delay, s16 loop_count) {
    s32 sp1C = 0;

    if (Particle_LacksFlags(particle, 1) != 0) {
        particle->frameTimer = frame_delay;
        particle->textureFrame = first_frame;
        particle->loopsRemaining = loop_count;
        Particle_SetFlags(particle, 1);
    } else {
        particle->frameTimer--;
        if (particle->frameTimer <= 0) {
            particle->frameTimer = frame_delay;

            particle->textureFrame -= frame_step;
            if (particle->textureFrame < last_frame) {
                if (particle->loopsRemaining > 0) {
                    particle->loopsRemaining--;
                }

                if (particle->loopsRemaining == 0) {
                    particle->textureFrame = last_frame;
                    sp1C = 1;
                } else {
                    particle->textureFrame = first_frame;
                }
            }
        }
    }

    return sp1C;
}

/* Run the corresponding frame animation and advance lifecycle when it reports completion. */
s32 Particle_AnimateFramesReverseAndAdvanceLifecycle(Particle* particle, s16 first_frame, s16 last_frame, s16 frame_step, s16 frame_delay, s16 loop_count) {
    s32 sp24 = Particle_AnimateFramesReverse(particle, first_frame, last_frame, frame_step, frame_delay, loop_count);

    if (sp24 != 0) {
        Particle_AdvanceLifecycle(particle);
    }
    return sp24;
}

/* Run ping-pong texture-frame animation with a frame timer and loop counter; return 1 on completion. */
s32 Particle_AnimateFramesPingPong(Particle* particle, s16 first_frame, s16 last_frame, s16 frame_step, s16 frame_delay, s16 loop_count) {
    s32 sp24 = 0;

    if (Particle_LacksFlags(particle, 1) != 0) {
        particle->frameTimer = frame_delay;
        particle->textureFrame = first_frame;
        particle->loopsRemaining = loop_count;
        Particle_SetFlags(particle, 1);
        Particle_ClearFlags(particle, 0x10);
    } else {
        particle->frameTimer--;
        if (particle->frameTimer <= 0) {
            particle->frameTimer = frame_delay;

            if (Particle_LacksFlags(particle, 0x10) != 0) {
                particle->textureFrame += frame_step;
                if (particle->textureFrame >= last_frame) {
                    particle->textureFrame = last_frame;
                    Particle_SetFlags(particle, 0x10);
                }
            } else {
                particle->textureFrame -= frame_step;
                if (first_frame >= particle->textureFrame) {
                    particle->textureFrame = first_frame;
                    if (particle->loopsRemaining > 0) {
                        particle->loopsRemaining--;
                    }

                    if (particle->loopsRemaining == 0) {
                        Particle_ClearFlags(particle, 0x20);
                        Particle_ClearFlags(particle, 1);
                        sp24 = 1;
                    } else {
                        Particle_ClearFlags(particle, 0x10);
                        Particle_SetFlags(particle, 0x20);
                    }
                }
            }
        }
    }

    return sp24;
}

/* Run the corresponding frame animation and advance lifecycle when it reports completion. */
s32 Particle_AnimateFramesPingPongAndAdvanceLifecycle(Particle* particle, s16 first_frame, s16 last_frame, s16 frame_step, s16 frame_delay, s16 loop_count) {
    s32 sp24 = Particle_AnimateFramesPingPong(particle, first_frame, last_frame, frame_step, frame_delay, loop_count);

    if (sp24 != 0) {
        Particle_AdvanceLifecycle(particle);
    }
    return sp24;
}

/* Run reverse-first ping-pong texture-frame animation with a frame timer and loop counter; return 1 on completion. */
s32 Particle_AnimateFramesReverseFirstPingPong(Particle* particle, s16 first_frame, s16 last_frame, s16 frame_step, s16 frame_delay, s16 loop_count) {
    s32 sp24 = 0;

    if (Particle_LacksFlags(particle, 1) != 0) {
        particle->frameTimer = frame_delay;
        particle->textureFrame = first_frame;
        particle->loopsRemaining = loop_count;
        Particle_SetFlags(particle, 1);
        Particle_ClearFlags(particle, 0x10);
    } else {
        particle->frameTimer--;
        if (particle->frameTimer <= 0) {
            particle->frameTimer = frame_delay;
            if (Particle_LacksFlags(particle, 0x10) != 0) {
                particle->textureFrame -= frame_step;
                if (last_frame >= particle->textureFrame) {
                    particle->textureFrame = last_frame;
                    Particle_SetFlags(particle, 0x10);
                }
            } else {
                particle->textureFrame += frame_step;
                if (particle->textureFrame >= first_frame) {
                    particle->textureFrame = first_frame;
                    if (particle->loopsRemaining > 0) {
                        particle->loopsRemaining--;
                    }

                    if (particle->loopsRemaining == 0) {
                        Particle_ClearFlags(particle, 1);
                        sp24 = 1;
                    } else {
                        Particle_ClearFlags(particle, 0x10);
                    }
                }
            }
        }
    }
    return sp24;
}

/* Run the corresponding frame animation and advance lifecycle when it reports completion. */
s32 Particle_AnimateFramesReverseFirstPingPongAndAdvanceLifecycle(Particle* particle, s16 first_frame, s16 last_frame, s16 frame_step, s16 frame_delay, s16 loop_count) {
    s32 sp24 = Particle_AnimateFramesReverseFirstPingPong(particle, first_frame, last_frame, frame_step, frame_delay, loop_count);

    if (sp24 != 0) {
        Particle_AdvanceLifecycle(particle);
    }
    return sp24;
}

/* Set phase to one and clear the phase-timer initialization bit. */
void Particle_BeginPhase(Particle* particle) {
    particle->phase = 1;
    Particle_ClearFlags(particle, 2);
}

/* Set phase to zero and clear the phase-timer initialization bit. */
void Particle_ResetPhase(Particle* particle) {
    particle->phase = 0;
    Particle_ClearFlags(particle, 2);
}

/* Increment the effect-local phase without changing lifecycle. */
void Particle_AdvancePhase(Particle* particle) {
    particle->phase++;
}

/* Initialize under flag 2, decrement immediately, and finish only after the timer becomes negative. */
s32 Particle_TickPhaseTimer(Particle* particle, s16 duration) {
    s32 sp1C = 0;

    if (Particle_LacksFlags(particle, 2) != 0) {
        Particle_SetFlags(particle, 2);
        particle->phaseTimer = duration;
    }

    particle->phaseTimer--;
    if (particle->phaseTimer < 0) {
        Particle_ClearFlags(particle, 2);
        sp1C = 1;
    }
    return sp1C;
}

/* Sum origin and both offset terms; copy the resulting transform and alpha to an associated DisplayObject. */
void Particle_UpdateWorldTransform(Particle* particle) {
    particle->worldPosition.x = particle->origin.x + particle->positionOffset.x + particle->motionOffset.x;
    particle->worldPosition.y = particle->origin.y + particle->positionOffset.y + particle->motionOffset.y;
    particle->worldPosition.z = particle->origin.z + particle->positionOffset.z + particle->motionOffset.z;

    if (particle->displayObject != NULL) {
        particle->displayObject->position.x = particle->worldPosition.x;
        particle->displayObject->position.y = particle->worldPosition.y;
        particle->displayObject->position.z = particle->worldPosition.z;

        particle->displayObject->scale.x = particle->scale;
        particle->displayObject->scale.y = particle->scale;
        particle->displayObject->scale.z = particle->scale;

        if (Particle_LacksFlags(particle, 0x200) != 0) {
            particle->displayObject->rotation.x = particle->rotation.x;
            particle->displayObject->rotation.y = particle->rotation.y;
            particle->displayObject->rotation.z = particle->rotation.z;
        }
        particle->displayObject->lodFraction = particle->primAlpha;
    }
}

/* Set the base position offset after multiplying each input by the render context's Y scale. */
void Particle_SetPosition(Particle* particle, f32 x, f32 y, f32 z) {
    particle->positionOffset.x = x * sParticleRenderContext.scale.y;
    particle->positionOffset.y = y * sParticleRenderContext.scale.y;
    particle->positionOffset.z = z * sParticleRenderContext.scale.y;
}

/* Set positionOffset.x, positionOffset.y, positionOffset.z with render-context Y scaling. */
void Particle_PositionOffset_SetVecFromVec3fScaled(Particle* particle, Vec3f position_offset) {
    particle->positionOffset.x = position_offset.x * sParticleRenderContext.scale.y;
    particle->positionOffset.y = position_offset.y * sParticleRenderContext.scale.y;
    particle->positionOffset.z = position_offset.z * sParticleRenderContext.scale.y;
}

/* Set positionOffset.x, positionOffset.y, positionOffset.z. */
void Particle_PositionOffset_SetVec(Particle* particle, f32 x, f32 y, f32 z) {
    particle->positionOffset.x = x;
    particle->positionOffset.y = y;
    particle->positionOffset.z = z;
}

/* Set positionOffset.x with render-context Y scaling. */
void Particle_PositionOffset_SetXScaled(Particle* particle, f32 x) {
    particle->positionOffset.x = x * sParticleRenderContext.scale.y;
}

/* Set positionOffset.y with render-context Y scaling. */
void Particle_PositionOffset_SetYScaled(Particle* particle, f32 y) {
    particle->positionOffset.y = y * sParticleRenderContext.scale.y;
}

/* Set positionOffset.z with render-context Y scaling. */
void Particle_PositionOffset_SetZScaled(Particle* particle, f32 z) {
    particle->positionOffset.z = z * sParticleRenderContext.scale.y;
}

/* Set the base position offset using Particle_ComputeVectorFromRotationXYZAlt and context scaling. */
void Particle_SetPositionFromRotationTemplate1(Particle* particle, f32 magnitude) {
    Vec3f sp1C;

    Particle_ComputeVectorFromRotationXYZAlt(particle, magnitude, &sp1C);
    Particle_PositionOffset_SetVecFromVec3fScaled(particle, sp1C);
}

/* Set the base position offset using Particle_ComputeVectorFromRotationYZVariant and context scaling. */
void Particle_SetPositionFromRotationTemplate2(Particle* particle, f32 magnitude) {
    Vec3f sp1C;

    Particle_ComputeVectorFromRotationYZVariant(particle, magnitude, &sp1C);
    Particle_PositionOffset_SetVecFromVec3fScaled(particle, sp1C);
}

/* Add to positionOffset.x, positionOffset.y, positionOffset.z with render-context Y scaling. */
void Particle_PositionOffset_AddVecScaledFromVec3f(Particle* particle, Vec3f delta) {
    particle->positionOffset.x += delta.x * sParticleRenderContext.scale.y;
    particle->positionOffset.y += delta.y * sParticleRenderContext.scale.y;
    particle->positionOffset.z += delta.z * sParticleRenderContext.scale.y;
}

/* Add to positionOffset.x, positionOffset.y, positionOffset.z. */
void Particle_PositionOffset_AddVecFromVec3f(Particle* particle, Vec3f delta) {
    particle->positionOffset.x += delta.x;
    particle->positionOffset.y += delta.y;
    particle->positionOffset.z += delta.z;
}

/* Add to positionOffset.x, positionOffset.y, positionOffset.z with render-context Y scaling. */
void Particle_PositionOffset_AddVecScaled(Particle* particle, f32 delta_x, f32 delta_y, f32 delta_z) {
    particle->positionOffset.x += delta_x * sParticleRenderContext.scale.y;
    particle->positionOffset.y += delta_y * sParticleRenderContext.scale.y;
    particle->positionOffset.z += delta_z * sParticleRenderContext.scale.y;
}

/* Add to positionOffset.x with render-context Y scaling. */
void Particle_PositionOffset_AddXScaled(Particle* particle, f32 delta_x) {
    particle->positionOffset.x += delta_x * sParticleRenderContext.scale.y;
}

/* Add to positionOffset.y with render-context Y scaling. */
void Particle_PositionOffset_AddYScaled(Particle* particle, f32 delta_y) {
    particle->positionOffset.y += delta_y * sParticleRenderContext.scale.y;
}

/* Add to positionOffset.z with render-context Y scaling. */
void Particle_PositionOffset_AddZScaled(Particle* particle, f32 delta_z) {
    particle->positionOffset.z += delta_z * sParticleRenderContext.scale.y;
}

/* Add to positionOffset.x, positionOffset.z with render-context Y scaling. */
void Particle_PositionOffset_AddXZFromRotationY(Particle* particle, f32 magnitude) {
    particle->positionOffset.x += magnitude * SINS(particle->rotation.y) * sParticleRenderContext.scale.y;
    particle->positionOffset.z += magnitude * COSS(particle->rotation.y) * sParticleRenderContext.scale.y;
}

/* Add to positionOffset.x, positionOffset.z with render-context Y scaling. */
void Particle_Field38_AddXZFromField94YOffset180_08F38(Particle* particle, f32 magnitude) {
    particle->positionOffset.x += magnitude * SINS(particle->rotation.y + 0x8000) * sParticleRenderContext.scale.y;
    particle->positionOffset.z += magnitude * COSS(particle->rotation.y + 0x8000) * sParticleRenderContext.scale.y;
}

/* Add to positionOffset.x, positionOffset.z with render-context Y scaling. */
void Particle_PositionOffset_AddXZFromRotationYOffset90(Particle* particle, f32 magnitude) {
    particle->positionOffset.x += magnitude * SINS(particle->rotation.y + 0x4000) * sParticleRenderContext.scale.y;
    particle->positionOffset.z += magnitude * COSS(particle->rotation.y + 0x4000) * sParticleRenderContext.scale.y;
}

/* Add to positionOffset.x, positionOffset.z with render-context Y scaling. */
void Particle_Field38_AddXZFromField94YOffsetMinus90_0901C(Particle* particle, f32 magnitude) {
    particle->positionOffset.x += magnitude * SINS(particle->rotation.y - 0x4000) * sParticleRenderContext.scale.y;
    particle->positionOffset.z += magnitude * COSS(particle->rotation.y - 0x4000) * sParticleRenderContext.scale.y;
}

/* Add Particle_ComputeVectorFromRotationY's result to the base position offset with context scaling. */
void Particle_AddPositionOffsetFromRotationDirection(Particle* particle, f32 magnitude) {
    Vec3f sp1C;

    Particle_ComputeVectorFromRotationY(particle, magnitude, &sp1C);
    particle->positionOffset.x += sp1C.x * sParticleRenderContext.scale.y;
    particle->positionOffset.y += sp1C.y * sParticleRenderContext.scale.y;
    particle->positionOffset.z += sp1C.z * sParticleRenderContext.scale.y;
}

/* Add Particle_ComputeVectorFromRotationXYZAlt's result to the base position offset with context scaling. */
void Particle_AddPositionFromRotationTemplate1(Particle* particle, f32 magnitude) {
    Vec3f sp1C;

    Particle_ComputeVectorFromRotationXYZAlt(particle, magnitude, &sp1C);
    Particle_PositionOffset_AddVecScaledFromVec3f(particle, sp1C);
}

/* Add Particle_ComputeVectorFromRotationYZVariant's result to the base position offset with context scaling. */
void Particle_AddPositionFromRotationTemplate2(Particle* particle, f32 magnitude) {
    Vec3f sp1C;

    Particle_ComputeVectorFromRotationYZVariant(particle, magnitude, &sp1C);
    Particle_PositionOffset_AddVecScaledFromVec3f(particle, sp1C);
}

/* Rotate the input vector around Y and add it to the base position offset with context scaling. */
void Particle_AddPositionOffsetRotatedY(Particle* particle, Vec3f offset) {
    Vec3f sp24;

    ParticleMath_RotateVec3fY(&sp24, offset, particle->rotation);
    Particle_PositionOffset_AddVecScaledFromVec3f(particle, sp24);
}

/* Set the base position offset's X component(s) from the random sampler with context scaling. */
void Particle_SetRandomPositionOffsetX(Particle* particle, s32 range) {
    particle->positionOffset.x = ParticleMath_RandomSigned(range) * sParticleRenderContext.scale.y;
}

/* Set the base position offset's Y component(s) from the random sampler with context scaling. */
void Particle_SetRandomPositionOffsetY(Particle* particle, s32 range) {
    particle->positionOffset.y = ParticleMath_RandomSigned(range) * sParticleRenderContext.scale.y;
}

/* Set the base position offset's Z component(s) from the random sampler with context scaling. */
void Particle_SetRandomPositionOffsetZ(Particle* particle, s32 range) {
    particle->positionOffset.z = ParticleMath_RandomSigned(range) * sParticleRenderContext.scale.y;
}

/* Set the base position offset's X component(s) from the random sampler with context scaling and an unscaled additive offset. */
void Particle_SetRandomPositionOffsetXWithOffset(Particle* particle, s32 range, s32 offset) {
    particle->positionOffset.x = (ParticleMath_RandomSigned(range) * sParticleRenderContext.scale.y) + offset;
}

/* Set the base position offset's Y component(s) from the random sampler with context scaling and an unscaled additive offset. */
void Particle_SetRandomPositionOffsetYOffset(Particle* particle, s32 range, s32 offset) {
    particle->positionOffset.y = (ParticleMath_RandomSigned(range) * sParticleRenderContext.scale.y) + offset;
}

/* Set the base position offset's Z component(s) from the random sampler with context scaling and an unscaled additive offset. */
void Particle_SetRandomPositionOffsetZWithOffset(Particle* particle, s32 range, s32 offset) {
    particle->positionOffset.z = (ParticleMath_RandomSigned(range) * sParticleRenderContext.scale.y) + offset;
}

/* Set the base position offset's X component(s) from the random sampler with context scaling and a sign-dependent offset. */
void Particle_SetRandomPositionOffsetXSymmetric(Particle* particle, s32 range, s32 offset) {
    f32 temp_fv0 = ParticleMath_RandomSigned(range);

    if (temp_fv0 >= 0.0f) {
        particle->positionOffset.x = (temp_fv0 + offset) * sParticleRenderContext.scale.y;
    } else {
        particle->positionOffset.x = (temp_fv0 - offset) * sParticleRenderContext.scale.y;
    }
}

/* Set the base position offset's Y component(s) from the random sampler with context scaling and a sign-dependent offset. */
void Particle_SetRandomPositionOffsetYSignedOffset(Particle* particle, s32 range, s32 offset) {
    f32 temp_fv0 = ParticleMath_RandomSigned(range);

    if (temp_fv0 >= 0.0f) {
        particle->positionOffset.y = (temp_fv0 + offset) * sParticleRenderContext.scale.y;
    } else {
        particle->positionOffset.y = (temp_fv0 - offset) * sParticleRenderContext.scale.y;
    }
}

/* Set the base position offset's Z component(s) from the random sampler with context scaling and a sign-dependent offset. */
void Particle_SetRandomPositionOffsetZSymmetric(Particle* particle, s32 range, s32 offset) {
    f32 temp_fv0 = ParticleMath_RandomSigned(range);

    if (temp_fv0 >= 0.0f) {
        particle->positionOffset.z = (temp_fv0 + offset) * sParticleRenderContext.scale.y;
    } else {
        particle->positionOffset.z = (temp_fv0 - offset) * sParticleRenderContext.scale.y;
    }
}

/* Set the base position offset's XYZ component(s) from the random sampler with context scaling. */
void Particle_SetRandomPositionOffsetXYZ(Particle* particle, s32 range_x, s32 range_y, s32 range_z) {
    Vec3f sp18;

    sp18.x = ParticleMath_RandomSigned(range_x);
    sp18.y = ParticleMath_RandomSigned(range_y);
    sp18.z = ParticleMath_RandomSigned(range_z);

    particle->positionOffset.x = sp18.x * sParticleRenderContext.scale.y;
    particle->positionOffset.y = sp18.y * sParticleRenderContext.scale.y;
    particle->positionOffset.z = sp18.z * sParticleRenderContext.scale.y;
}

/* Set the base position offset's XYZ component(s) from the random sampler with context scaling. */
void Particle_SetRandomPositionOffsetXYZPositive(Particle* particle, s32 range_x, s32 range_y, s32 range_z) {
    Vec3f sp1C;

    sp1C.x = ParticleMath_RandomRange(range_x);
    sp1C.y = ParticleMath_RandomRange(range_y);
    sp1C.z = ParticleMath_RandomRange(range_z);

    particle->positionOffset.x = sp1C.x * sParticleRenderContext.scale.y;
    particle->positionOffset.y = sp1C.y * sParticleRenderContext.scale.y;
    particle->positionOffset.z = sp1C.z * sParticleRenderContext.scale.y;
}

/* Sample a signed XY vector, rotate it around Y, and store the context-scaled base position offset. */
void Particle_SetRandomRotatedPositionOffsetXY(Particle* particle, s32 range_x, s32 range_y) {
    Vec3f sp2C;
    Vec3f sp20;

    sp2C.x = ParticleMath_RandomSigned(range_x);
    sp2C.y = ParticleMath_RandomSigned(range_y);
    sp2C.z = 0.0f;

    ParticleMath_RotateVec3fY(&sp20, sp2C, particle->rotation);

    particle->positionOffset.x = sp20.x * sParticleRenderContext.scale.y;
    particle->positionOffset.y = sp20.y * sParticleRenderContext.scale.y;
    particle->positionOffset.z = sp20.z * sParticleRenderContext.scale.y;
}

/* Add independent signed random components to the base position offset with render-context scaling. */
void Particle_AddRandomPositionOffsetXYZ(Particle* particle, s32 range_x, s32 range_y, s32 range_z) {
    Vec3f sp24;

    sp24.x = ParticleMath_RandomSigned(range_x);
    sp24.y = ParticleMath_RandomSigned(range_y);
    sp24.z = ParticleMath_RandomSigned(range_z);

    particle->positionOffset.x += sp24.x * sParticleRenderContext.scale.y;
    particle->positionOffset.y += sp24.y * sParticleRenderContext.scale.y;
    particle->positionOffset.z += sp24.z * sParticleRenderContext.scale.y;
}

/* Set the unscaled origin term used in world-position composition. */
void Particle_SetOrigin(Particle* particle, f32 x, f32 y, f32 z) {
    particle->origin.x = x;
    particle->origin.y = y;
    particle->origin.z = z;
}

/* Set the secondary motion offset with render-context scaling; this term is summed directly into world position. */
void Particle_SetMotionOffset(Particle* particle, f32 x, f32 y, f32 z) {
    particle->motionOffset.x = x * sParticleRenderContext.scale.y;
    particle->motionOffset.y = y * sParticleRenderContext.scale.y;
    particle->motionOffset.z = z * sParticleRenderContext.scale.y;
}

/* Set motionOffset.x with render-context Y scaling. */
void Particle_MotionOffset_SetXScaled(Particle* particle, f32 x) {
    particle->motionOffset.x = x * sParticleRenderContext.scale.y;
}

/* Set motionOffset.y with render-context Y scaling. */
void Particle_MotionOffset_SetYScaled(Particle* particle, f32 y) {
    particle->motionOffset.y = y * sParticleRenderContext.scale.y;
}

/* Set motionOffset.z with render-context Y scaling. */
void Particle_MotionOffset_SetZScaled(Particle* particle, f32 z) {
    particle->motionOffset.z = z * sParticleRenderContext.scale.y;
}

/* Fill the secondary motion offset with a random-angle vector scaled by the render context. */
void Particle_SetMotionOffsetRandomDirection(Particle* particle, f32 magnitude) {
    ParticleMath_RandomDirection(&particle->motionOffset, magnitude * sParticleRenderContext.scale.y);
}

/* Add to motionOffset.x, motionOffset.y, motionOffset.z with render-context Y scaling. */
void Particle_MotionOffset_AddVecScaled(Particle* particle, f32 delta_x, f32 delta_y, f32 delta_z) {
    particle->motionOffset.x += delta_x * sParticleRenderContext.scale.y;
    particle->motionOffset.y += delta_y * sParticleRenderContext.scale.y;
    particle->motionOffset.z += delta_z * sParticleRenderContext.scale.y;
}

/* Add to motionOffset.x with render-context Y scaling. */
void Particle_MotionOffset_AddXScaled(Particle* particle, f32 delta_x) {
    particle->motionOffset.x += delta_x * sParticleRenderContext.scale.y;
}

/* Add to motionOffset.y with render-context Y scaling. */
void Particle_MotionOffset_AddYScaled(Particle* particle, f32 delta_y) {
    particle->motionOffset.y += delta_y * sParticleRenderContext.scale.y;
}

/* Add to motionOffset.z with render-context Y scaling. */
void Particle_MotionOffset_AddZScaled(Particle* particle, f32 delta_z) {
    particle->motionOffset.z += delta_z * sParticleRenderContext.scale.y;
}

/* Subtract from motionOffset.x, motionOffset.y, motionOffset.z with render-context Y scaling. */
void Particle_MotionOffset_SubVecScaled(Particle* particle, f32 delta_x, f32 delta_y, f32 delta_z) {
    particle->motionOffset.x -= delta_x * sParticleRenderContext.scale.y;
    particle->motionOffset.y -= delta_y * sParticleRenderContext.scale.y;
    particle->motionOffset.z -= delta_z * sParticleRenderContext.scale.y;
}

/* Subtract from motionOffset.x with render-context Y scaling. */
void Particle_MotionOffset_SubXScaled(Particle* particle, f32 delta_x) {
    particle->motionOffset.x -= delta_x * sParticleRenderContext.scale.y;
}

/* Subtract from motionOffset.y with render-context Y scaling. */
void Particle_MotionOffset_SubYScaled(Particle* particle, f32 delta_y) {
    particle->motionOffset.y -= delta_y * sParticleRenderContext.scale.y;
}

/* Subtract from motionOffset.z with render-context Y scaling. */
void Particle_MotionOffset_SubZScaled(Particle* particle, f32 delta_z) {
    particle->motionOffset.z -= delta_z * sParticleRenderContext.scale.y;
}

/* Approach motionOffset.x, motionTarget.x using the stored or supplied target and step. */
s32 Particle_MotionOffset_ApproachXToMotionTarget(Particle* particle, f32 target) {
    return ParticleMath_ApproachFloat(&particle->motionOffset.x, target * sParticleRenderContext.scale.y, particle->motionTarget.x);
}

/* Approach motionOffset.y, motionTarget.y using the stored or supplied target and step. */
s32 Particle_MotionOffset_ApproachYToMotionTarget(Particle* particle, f32 target) {
    return ParticleMath_ApproachFloat(&particle->motionOffset.y, target * sParticleRenderContext.scale.y, particle->motionTarget.y);
}

/* Approach motionOffset.z, motionTarget.z using the stored or supplied target and step. */
s32 Particle_MotionOffset_ApproachZToMotionTarget(Particle* particle, f32 target) {
    return ParticleMath_ApproachFloat(&particle->motionOffset.z, target * sParticleRenderContext.scale.y, particle->motionTarget.z);
}

/* Set motionOffset.x, motionOffset.y, motionOffset.z. */
void Particle_MotionOffset_SetVecFromMotionStep(Particle* particle) {
    particle->motionOffset.x = particle->motionStep.x;
    particle->motionOffset.y = particle->motionStep.y;
    particle->motionOffset.z = particle->motionStep.z;
}

/* Set motionOffset.x, motionOffset.z. */
void Particle_MotionOffset_SetXZFromMotionStep(Particle* particle) {
    particle->motionOffset.x = particle->motionStep.x;
    particle->motionOffset.z = particle->motionStep.z;
}

/* Set motionOffset.x, motionOffset.y. */
void Particle_MotionOffset_SetXYFromMotionStep(Particle* particle) {
    particle->motionOffset.x = particle->motionStep.x;
    particle->motionOffset.y = particle->motionStep.y;
}

/* Add to motionOffset.x, motionOffset.y, motionOffset.z. */
void Particle_MotionOffset_AddVecFromMotionStep(Particle* particle) {
    particle->motionOffset.x += particle->motionStep.x;
    particle->motionOffset.y += particle->motionStep.y;
    particle->motionOffset.z += particle->motionStep.z;
}

/* Add to motionOffset.x, motionOffset.z. */
void Particle_MotionOffset_AddXZFromMotionStep(Particle* particle) {
    particle->motionOffset.x += particle->motionStep.x;
    particle->motionOffset.z += particle->motionStep.z;
}

/* Add to motionOffset.x, motionOffset.y. */
void Particle_MotionOffset_AddXYFromMotionStep(Particle* particle) {
    particle->motionOffset.x += particle->motionStep.x;
    particle->motionOffset.y += particle->motionStep.y;
}

/* Add to motionOffset.x. */
void Particle_MotionOffset_AddXFromMotionStep(Particle* particle) {
    particle->motionOffset.x += particle->motionStep.x;
}

/* Add to motionOffset.y. */
void Particle_MotionOffset_AddYFromMotionStep(Particle* particle) {
    particle->motionOffset.y += particle->motionStep.y;
}

/* Add to motionOffset.z. */
void Particle_MotionOffset_AddZFromMotionStep(Particle* particle) {
    particle->motionOffset.z += particle->motionStep.z;
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_SetMotionOffsetXZFromRotationY(Particle* particle) {
    Particle_MotionStep_SetXZFromField74AngleY(particle);
    Particle_MotionOffset_SetXZFromMotionStep(particle);
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_AddVelocityXZFromRotationYViaImpulse(Particle* particle) {
    Particle_MotionStep_SetXZFromField74AngleY(particle);
    Particle_MotionOffset_AddXZFromMotionStep(particle);
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_AddVelocityXYFromRotationZViaImpulse(Particle* particle) {
    Particle_MotionStep_SetXYFromField74AngleZ(particle);
    Particle_MotionOffset_AddXYFromMotionStep(particle);
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_AddMotionOffsetFromRotationTemplate1(Particle* particle) {
    Particle_MotionStep_SetVecFromField74AnglesXY(particle);
    Particle_MotionOffset_AddVecFromMotionStep(particle);
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_AddMotionOffsetFromRotationTemplate2(Particle* particle) {
    Particle_MotionStep_SetVecFromField74AnglesXZ(particle);
    Particle_MotionOffset_AddVecFromMotionStep(particle);
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_AddMotionOffsetFromRotationTemplate3(Particle* particle) {
    Particle_MotionStep_SetVecFromField74AnglesXYAlt(particle);
    Particle_MotionOffset_AddVecFromMotionStep(particle);
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_AddMotionOffsetFromRotationTemplate4(Particle* particle) {
    Particle_MotionStep_SetVecFromField74AnglesYZ(particle);
    Particle_MotionOffset_AddVecFromMotionStep(particle);
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_AddMotionOffsetFromRotationTemplate5(Particle* particle) {
    Particle_MotionStep_SetVecFromField74AnglesXYZAlt(particle);
    Particle_MotionOffset_AddVecFromMotionStep(particle);
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_AddMotionOffsetFromRotationTemplate6(Particle* particle) {
    Particle_MotionStep_SetVecFromField74AnglesXYZ(particle);
    Particle_MotionOffset_AddVecFromMotionStep(particle);
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_AddMotionOffsetFromRotationTemplate7A(Particle* particle) {
    Particle_MotionStep_SetVecFromField74AnglesYZAlt(particle);
    Particle_MotionOffset_AddVecFromMotionStep(particle);
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_AddMotionOffsetFromRotationTemplate7B(Particle* particle) {
    Particle_MotionStep_SetVecFromField74AnglesYZAlt(particle);
    Particle_MotionOffset_AddVecFromMotionStep(particle);
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_AddMotionOffsetXZFromRotationY(Particle* particle) {
    particle->motionStep.x = particle->stepMagnitude * SINS(particle->rotation.y);
    particle->motionStep.z = particle->stepMagnitude * COSS(particle->rotation.y);

    Particle_MotionOffset_AddXZFromMotionStep(particle);
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_AddVelocityXZFromRotationYPlusHalfTurn(Particle* particle) {
    particle->motionStep.x = particle->stepMagnitude * SINS(particle->rotation.y + 0x8000);
    particle->motionStep.z = particle->stepMagnitude * COSS(particle->rotation.y + 0x8000);

    Particle_MotionOffset_AddXZFromMotionStep(particle);
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_AddVelocityXZFromRotationYPlusHalfTurnFromArgument(Particle* particle, f32 magnitude) {
    particle->motionStep.x = SINS(particle->rotation.y + 0x8000) * magnitude;
    particle->motionStep.z = COSS(particle->rotation.y + 0x8000) * magnitude;

    Particle_MotionOffset_AddXZFromMotionStep(particle);
}

/* Set the vertical motion step to stepMagnitude, then add that step to the motion offset. */
void Particle_AddVerticalMotionScalar(Particle* particle) {
    particle->motionStep.y = particle->stepMagnitude;

    Particle_MotionOffset_AddYFromMotionStep(particle);
}

/* Apply the downward motion term and return whether the stored world-position Y is nonpositive. */
s32 Particle_Field50_AddNegField74Y_CheckField2CYNonPositive_09F84(Particle* particle) {
    s32 sp1C = 0;

    particle->motionStep.y = -particle->stepMagnitude;
    Particle_MotionOffset_AddYFromMotionStep(particle);

    if (particle->worldPosition.y <= 0.0f) {
        sp1C = 1;
    }
    return sp1C;
}

/* Apply the downward motion term and return whether the stored world-position Y is nonpositive. */
s32 Particle_Field74_AdvanceAndAddNegY_CheckField2CYNonPositive_09FD8(Particle* particle) {
    s32 sp1C = 0;

    particle->stepMagnitude += particle->magnitudeStep;
    particle->motionStep.y = -particle->stepMagnitude;

    Particle_MotionOffset_AddYFromMotionStep(particle);

    if (particle->worldPosition.y <= 0.0f) {
        sp1C = 1;
    }
    return sp1C;
}

/* Set the component targets used when approaching the secondary motion offset. */
void Particle_SetMotionOffsetTarget(Particle* particle, f32 x, f32 y, f32 z) {
    particle->motionTarget.x = x * sParticleRenderContext.scale.y;
    particle->motionTarget.y = y * sParticleRenderContext.scale.y;
    particle->motionTarget.z = z * sParticleRenderContext.scale.y;
}

/* Set motionTarget.x with render-context Y scaling. */
void Particle_MotionTarget_SetXScaled(Particle* particle, f32 x) {
    particle->motionTarget.x = x * sParticleRenderContext.scale.y;
}

/* Set motionTarget.y with render-context Y scaling. */
void Particle_MotionTarget_SetYScaled(Particle* particle, f32 y) {
    particle->motionTarget.y = y * sParticleRenderContext.scale.y;
}

/* Set motionTarget.z with render-context Y scaling. */
void Particle_MotionTarget_SetZScaled(Particle* particle, f32 z) {
    particle->motionTarget.z = z * sParticleRenderContext.scale.y;
}

/* Approach motionTarget.y using the stored or supplied target and step. */
s32 Particle_MotionTarget_ApproachY(Particle* particle, f32 target, f32 step) {
    return ParticleMath_ApproachFloat(&particle->motionTarget.y, target * sParticleRenderContext.scale.y, step * sParticleRenderContext.scale.y);
}

/* Return 40 percent of the supplied difference for proportional position approach. */
f32 ParticleMath_ScaleApproachStep(f32 delta) {
    return delta * 0.4f;
}

/* Approach each base position-offset component by 40 percent of its difference when outside a +/-0.01 tolerance. */
void Particle_ApproachPosition(Particle* particle, Vec3f target_position) {
    f32 sp24;
    f32 sp20;
    f32 temp_fa0;

    temp_fa0 = target_position.x - particle->positionOffset.x;
    sp20 = target_position.y - particle->positionOffset.y;
    sp24 = target_position.z - particle->positionOffset.z;

    if (!(temp_fa0 <= 0.01f) || !(temp_fa0 >= -0.01f)) {
        ParticleMath_ApproachFloat(&particle->positionOffset.x, target_position.x, ParticleMath_ScaleApproachStep(temp_fa0));
    }

    if (!(sp20 <= 0.01f) || !(sp20 >= -0.01f)) {
        ParticleMath_ApproachFloat(&particle->positionOffset.y, target_position.y, ParticleMath_ScaleApproachStep(sp20));
    }

    if (!(sp24 <= 0.01f) || !(sp24 >= -0.01f)) {
        ParticleMath_ApproachFloat(&particle->positionOffset.z, target_position.z, ParticleMath_ScaleApproachStep(sp24));
    }
}

/* Set rotation.x, rotation.y, rotation.z. */
void Particle_Rotation_SetVecFromVec3s(Particle* particle, Vec3s rotation) {
    particle->rotation.x = rotation.x;
    particle->rotation.y = rotation.y;
    particle->rotation.z = rotation.z;
}

/* Set the three binary-angle rotation components. */
void Particle_SetRotation(Particle* particle, s16 rotation_x, s16 rotation_y, s16 rotation_z) {
    particle->rotation.x = rotation_x;
    particle->rotation.y = rotation_y;
    particle->rotation.z = rotation_z;
}

/* Set rotation.x. */
void Particle_Rotation_SetX(Particle* particle, s16 rotation_x) {
    particle->rotation.x = rotation_x;
}

/* Set rotation.y. */
void Particle_Rotation_SetY(Particle* particle, s16 rotation_y) {
    particle->rotation.y = rotation_y;
}

/* Set rotation.z. */
void Particle_Rotation_SetZ(Particle* particle, s16 rotation_z) {
    particle->rotation.z = rotation_z;
}

/* Add to rotation.x, rotation.y, rotation.z. */
void Particle_Rotation_AddConstantAll(Particle* particle) {
    particle->rotation.x += 0x8000;
    particle->rotation.y += 0x8000;
    particle->rotation.z += 0x8000;
}

/* Add to rotation.x. */
void Particle_Rotation_AddXConstant(Particle* particle) {
    particle->rotation.x += 0x8000;
}

/* Add to rotation.y. */
void Particle_Rotation_AddYConstant(Particle* particle) {
    particle->rotation.y += 0x8000;
}

/* Add to rotation.z. */
void Particle_Rotation_AddZConstant(Particle* particle) {
    particle->rotation.z += 0x8000;
}

/* Set the auxiliary angle vector used as rotation steps by the add/approach helpers. */
void Particle_SetRotationSpeed(Particle* particle, s16 rotation_step_x, s16 rotation_step_y, s16 rotation_step_z) {
    particle->rotationStep.x = rotation_step_x;
    particle->rotationStep.y = rotation_step_y;
    particle->rotationStep.z = rotation_step_z;
}

/* Set rotationStep.x, rotationStep.y, rotationStep.z. */
void Particle_RotationStep_SetVecFromVec3s(Particle* particle, Vec3s rotation_step) {
    particle->rotationStep.x = rotation_step.x;
    particle->rotationStep.y = rotation_step.y;
    particle->rotationStep.z = rotation_step.z;
}

/* Set rotationStep.x, rotationStep.y. */
void Particle_RotationStep_SetXY(Particle* particle, s16 rotation_step_x, s16 rotation_step_y) {
    particle->rotationStep.x = rotation_step_x;
    particle->rotationStep.y = rotation_step_y;
}

/* Set rotationStep.x. */
void Particle_RotationStep_SetX(Particle* particle, s16 rotation_step_x) {
    particle->rotationStep.x = rotation_step_x;
}

/* Set rotationStep.y. */
void Particle_RotationStep_SetY(Particle* particle, s16 rotation_step_y) {
    particle->rotationStep.y = rotation_step_y;
}

/* Set rotationStep.z. */
void Particle_RotationStep_SetZ(Particle* particle, s16 rotation_step_z) {
    particle->rotationStep.z = rotation_step_z;
}

/* Set rotationTarget.x. */
void Particle_RotationTarget_SetX(Particle* particle, s16 rotation_target_x) {
    particle->rotationTarget.x = rotation_target_x;
}

/* Set rotationTarget.y. */
void Particle_RotationTarget_SetY(Particle* particle, s16 rotation_target_y) {
    particle->rotationTarget.y = rotation_target_y;
}

/* Set the X/Y rotation targets, leaving Z unchanged. */
void Particle_SetRotationTargetXY(Particle* particle, s16 rotation_target_x, s16 rotation_target_y) {
    Particle_RotationTarget_SetX(particle, rotation_target_x);
    Particle_RotationTarget_SetY(particle, rotation_target_y);
}

/* Copy current X/Y rotation into their targets, leaving the Z target unchanged. */
void Particle_SetRotationTargetFromCurrent(Particle* particle) {
    particle->rotationTarget.x = particle->rotation.x;
    particle->rotationTarget.y = particle->rotation.y;
    particle->rotationTarget.z = particle->rotation.z;
}

/* Add to rotation.x, rotation.y, rotation.z. */
void Particle_Rotation_AddVecFromRotationStep(Particle* particle) {
    particle->rotation.x += particle->rotationStep.x;
    particle->rotation.y += particle->rotationStep.y;
    particle->rotation.z += particle->rotationStep.z;
}

/* Add to rotation.x. */
void Particle_Rotation_AddXFromRotationStep(Particle* particle) {
    particle->rotation.x += particle->rotationStep.x;
}

/* Add to rotation.y. */
void Particle_Rotation_AddYFromRotationStep(Particle* particle) {
    particle->rotation.y += particle->rotationStep.y;
}

/* Add to rotation.z. */
void Particle_Rotation_AddZFromRotationStep(Particle* particle) {
    particle->rotation.z += particle->rotationStep.z;
}

/* Add to rotation.x, rotation.y, rotation.z. */
void Particle_Rotation_AddVecFromVec3s(Particle* particle, Vec3s delta_rotation) {
    particle->rotation.x += delta_rotation.x;
    particle->rotation.y += delta_rotation.y;
    particle->rotation.z += delta_rotation.z;
}

/* Add to rotation.x, rotation.y, rotation.z. */
void Particle_Rotation_AddVec(Particle* particle, s16 delta_rotation_x, s16 delta_rotation_y, s16 delta_rotation_z) {
    particle->rotation.x += delta_rotation_x;
    particle->rotation.y += delta_rotation_y;
    particle->rotation.z += delta_rotation_z;
}

/* Add to rotation.x. */
void Particle_Rotation_AddX(Particle* particle, s16 delta_rotation_x) {
    particle->rotation.x += delta_rotation_x;
}

/* Add to rotation.y. */
void Particle_Rotation_AddY(Particle* particle, s16 delta_rotation_y) {
    particle->rotation.y += delta_rotation_y;
}

/* Add to rotation.z. */
void Particle_Rotation_AddZ(Particle* particle, s16 delta_rotation_z) {
    particle->rotation.z += delta_rotation_z;
}

/* Add to the stored X/Y rotation targets. */
void Particle_AddRotationTargetXY(Particle* particle, s16 delta_rotation_target_x, s16 delta_rotation_target_y) {
    particle->rotationTarget.x += delta_rotation_target_x;
    particle->rotationTarget.y += delta_rotation_target_y;
}

/* Subtract from rotation.x, rotation.y, rotation.z. */
void Particle_Rotation_SubVecFromVec3s(Particle* particle, Vec3s delta_rotation) {
    particle->rotation.x -= delta_rotation.x;
    particle->rotation.y -= delta_rotation.y;
    particle->rotation.z -= delta_rotation.z;
}

/* Subtract from rotation.x. */
void Particle_Rotation_SubX(Particle* particle, s16 delta_rotation_x) {
    particle->rotation.x -= delta_rotation_x;
}

/* Subtract from rotation.y. */
void Particle_Rotation_SubY(Particle* particle, s16 delta_rotation_y) {
    particle->rotation.y -= delta_rotation_y;
}

/* Subtract from rotation.z. */
void Particle_Rotation_SubZ(Particle* particle, s16 delta_rotation_z) {
    particle->rotation.z -= delta_rotation_z;
}

/* Approach rotation.x, rotationTarget.x, rotationStep.x using the stored or supplied target and step. */
void Particle_ApproachRotationTargetX(Particle* particle) {
    ParticleMath_ApproachAngle(&particle->rotation.x, particle->rotationTarget.x, particle->rotationStep.x);
}

/* Approach rotation.y, rotationTarget.y, rotationStep.y using the stored or supplied target and step. */
void Particle_ApproachRotationTargetY(Particle* particle) {
    ParticleMath_ApproachAngle(&particle->rotation.y, particle->rotationTarget.y, particle->rotationStep.y);
}

/* Approach rotation.z, rotationTarget.z, rotationStep.z using the stored or supplied target and step. */
void Particle_ApproachRotationTargetZ(Particle* particle) {
    ParticleMath_ApproachAngle(&particle->rotation.z, particle->rotationTarget.z, particle->rotationStep.z);
}

/* Approach the selected rotation components using the stored or supplied target and step. */
void Particle_ApproachRotationTargetXY(Particle* particle) {
    Particle_ApproachRotationTargetX(particle);
    Particle_ApproachRotationTargetY(particle);
}

/* Approach the selected rotation components using the stored or supplied target and step. */
void Particle_ApproachRotationTargetXYZ(Particle* particle) {
    Particle_ApproachRotationTargetX(particle);
    Particle_ApproachRotationTargetY(particle);
    Particle_ApproachRotationTargetZ(particle);
}

/* Approach rotation.x using the stored or supplied target and step. */
void Particle_ApproachRotationXTo(Particle* particle, s16 target, s16 step) {
    ParticleMath_ApproachAngle(&particle->rotation.x, target, step);
}

/* Approach rotation.y using the stored or supplied target and step. */
void Particle_ApproachRotationYTo(Particle* particle, s16 target, s16 step) {
    ParticleMath_ApproachAngle(&particle->rotation.y, target, step);
}

/* Decrease the selected field toward its threshold and return the completion result. */
s32 Particle_Rotation_DecrementXTo(Particle* particle, s32 target, s32 step) {
    s32 var_v1 = 0;
    s32 var_v0 = particle->rotation.x;

    if (target < var_v0) {
        var_v0 -= step;
        if (target >= var_v0) {
            var_v0 = target;
            var_v1 = 1;
        }
        particle->rotation.x = var_v0;
    }

    return var_v1;
}

/* Decrease the selected field toward its threshold and return the completion result. */
s32 Particle_Rotation_DecrementYTo(Particle* particle, s32 target, s32 step) {
    s32 var_v1 = 0;
    s32 var_v0 = particle->rotation.y;

    if (target < var_v0) {
        var_v0 -= step;
        if (target >= var_v0) {
            var_v0 = target;
            var_v1 = 1;
        }
        particle->rotation.y = var_v0;
    }

    return var_v1;
}

/* Decrease the selected field toward its threshold and return the completion result. */
s32 Particle_Rotation_DecrementZTo(Particle* particle, s32 target, s32 step) {
    s32 var_v0 = particle->rotation.z;
    s32 var_v1 = 0;

    if (target < var_v0) {
        var_v0 -= step;
        if (target >= var_v0) {
            var_v0 = target;
            var_v1 = 1;
        }
        particle->rotation.z = var_v0;
    }
    return var_v1;
}

/* Add the indicated random binary-angle offsets, including any fixed half/quarter turn. */
void Particle_AddRotationRandomSignedXY(Particle* particle, s32 range_x, s32 range_y) {
    Particle_Rotation_AddX(particle, ParticleMath_RandomSigned(range_x));
    Particle_Rotation_AddY(particle, ParticleMath_RandomSigned(range_y));
}

/* Add the indicated random binary-angle offsets, including any fixed half/quarter turn. */
void Particle_AddRotationRandomSignedXYWithXHalfTurn(Particle* particle, s32 range_x, s32 range_y) {
    Particle_Rotation_AddX(particle, ParticleMath_RandomSignedOffset(range_x, 0x8000));
    Particle_Rotation_AddY(particle, ParticleMath_RandomSigned(range_y));
}

/* Add the indicated random binary-angle offsets, including any fixed half/quarter turn. */
void Particle_AddRotationRandomSignedY(Particle* particle, s32 range) {
    Particle_Rotation_AddY(particle, ParticleMath_RandomSigned(range));
}

/* Add the indicated random binary-angle offsets, including any fixed half/quarter turn. */
void Particle_AddRotationRandomSignedYWithHalfTurn(Particle* particle, s32 range) {
    Particle_Rotation_AddY(particle, ParticleMath_RandomSignedOffset(range, 0x8000));
}

/* Add the indicated random binary-angle offsets, including any fixed half/quarter turn. */
void Particle_AddRotationRandomSignedXWithFixedYQuarterTurn(Particle* particle) {
    Particle_Rotation_AddX(particle, ParticleMath_RandomSigned(0x10000));
    Particle_Rotation_AddY(particle, 0x4000);
}

/* Write a direction vector using the indicated rotation components and the supplied magnitude. */
void Particle_ComputeVectorFromRotationY(Particle* particle, f32 magnitude, Vec3f* destination) {
    destination->x = SINS(particle->rotation.y) * magnitude;
    destination->y = 0.0f;
    destination->z = COSS(particle->rotation.y) * magnitude;
}

/* Write a direction vector using the indicated rotation components and the supplied magnitude. */
void Particle_ComputeVectorFromRotationXY(Particle* particle, f32 magnitude, Vec3f* destination) {
    destination->x = magnitude * COSS(particle->rotation.x) * SINS(particle->rotation.y);
    destination->y = -magnitude * SINS(particle->rotation.x);
    destination->z = magnitude * COSS(particle->rotation.x) * COSS(particle->rotation.y);
}

/* Write a direction vector using the indicated rotation components and the supplied magnitude. */
void Particle_ComputeVectorFromRotationXZ(Particle* particle, f32 magnitude, Vec3f* destination) {
    destination->x = magnitude * SINS(particle->rotation.x) * SINS(particle->rotation.z);
    destination->y = -magnitude * SINS(particle->rotation.x) * COSS(particle->rotation.z);
    destination->z = magnitude * COSS(particle->rotation.x);
}

/* Write a direction vector using the indicated rotation components and the supplied magnitude. */
void Particle_ComputeVectorFromRotationXYZAlt(Particle* particle, f32 magnitude, Vec3f* destination) {
    destination->x = magnitude * (SINS(particle->rotation.x) * COSS(particle->rotation.y) * SINS(particle->rotation.z) +
                      COSS(particle->rotation.x) * SINS(particle->rotation.y));
    destination->y = -magnitude * SINS(particle->rotation.x) * COSS(particle->rotation.z);
    destination->z = magnitude * (COSS(particle->rotation.x) * COSS(particle->rotation.y) -
                      SINS(particle->rotation.x) * SINS(particle->rotation.y) * SINS(particle->rotation.z));
}

/* Write a direction vector using the indicated rotation components and the supplied magnitude. */
void Particle_ComputeVectorFromRotationYZVariant(Particle* particle, f32 magnitude, Vec3f* destination) {
    destination->x = magnitude * (SINS(particle->rotation.y) * COSS(particle->rotation.z) +
                      SINS(particle->rotation.x) * COSS(particle->rotation.y) * SINS(particle->rotation.z));
    destination->y = magnitude * (SINS(particle->rotation.y) * SINS(particle->rotation.z) -
                      SINS(particle->rotation.x) * COSS(particle->rotation.y) * COSS(particle->rotation.z));
    destination->z = magnitude * (COSS(particle->rotation.x) * COSS(particle->rotation.y));
}

/* Set stepMagnitude with render-context Y scaling. */
void Particle_StepMagnitude_SetScaled(Particle* particle, f32 step_magnitude) {
    particle->stepMagnitude = step_magnitude * sParticleRenderContext.scale.y;
}

/* Set magnitudeStep with render-context Y scaling. */
void Particle_MagnitudeStep_SetScaled(Particle* particle, f32 magnitude_step) {
    particle->magnitudeStep = magnitude_step * sParticleRenderContext.scale.y;
}

/* Approach stepMagnitude using the stored or supplied target and step. */
s32 Particle_StepMagnitude_Approach(Particle* particle, f32 target, f32 step) {
    return ParticleMath_ApproachFloat(&particle->stepMagnitude, target * sParticleRenderContext.scale.y, step * sParticleRenderContext.scale.y);
}

/* Approach stepMagnitude, magnitudeStep using the stored or supplied target and step. */
s32 Particle_StepMagnitude_ApproachUsingMagnitudeStep(Particle* particle, f32 target) {
    return ParticleMath_ApproachFloat(&particle->stepMagnitude, target * sParticleRenderContext.scale.y, particle->magnitudeStep);
}

/* Approach magnitudeStep using the stored or supplied target and step. */
s32 Particle_MagnitudeStep_Approach(Particle* particle, f32 target, f32 step) {
    return ParticleMath_ApproachFloat(&particle->magnitudeStep, target * sParticleRenderContext.scale.y, step * sParticleRenderContext.scale.y);
}

/* Set motionStep.x, motionStep.y, motionStep.z with render-context Y scaling. */
void Particle_MotionStep_SetVecScaled(Particle* particle, Vec3f motion_step) {
    particle->motionStep.x = motion_step.x * sParticleRenderContext.scale.y;
    particle->motionStep.y = motion_step.y * sParticleRenderContext.scale.y;
    particle->motionStep.z = motion_step.z * sParticleRenderContext.scale.y;
}

/* Set the motion-step vector with render-context scaling; integration helpers add it to the motion offset. */
void Particle_SetMotionStep(Particle* particle, f32 x, f32 y, f32 z) {
    particle->motionStep.x = x * sParticleRenderContext.scale.y;
    particle->motionStep.y = y * sParticleRenderContext.scale.y;
    particle->motionStep.z = z * sParticleRenderContext.scale.y;
}

/* Set motionStep.x with render-context Y scaling. */
void Particle_MotionStep_SetXScaled(Particle* particle, f32 x) {
    particle->motionStep.x = x * sParticleRenderContext.scale.y;
}

/* Set motionStep.y with render-context Y scaling. */
void Particle_MotionStep_SetYScaled(Particle* particle, f32 y) {
    particle->motionStep.y = y * sParticleRenderContext.scale.y;
}

/* Set motionStep.z with render-context Y scaling. */
void Particle_MotionStep_SetZScaled(Particle* particle, f32 z) {
    particle->motionStep.z = z * sParticleRenderContext.scale.y;
}

/* Add to motionStep.x with render-context Y scaling. */
s32 Particle_MotionStep_IncrementXUntilAtLeast(Particle* particle, f32 limit, f32 increment) {
    f32 temp_fv0;
    s32 var_v1 = 0;

    particle->motionStep.x += increment * sParticleRenderContext.scale.y;

    temp_fv0 = limit * sParticleRenderContext.scale.y;
    if (temp_fv0 <= particle->motionStep.x) {
        particle->motionStep.x = temp_fv0;
        var_v1 = 1;
    }
    return var_v1;
}

/* Add to motionStep.y with render-context Y scaling. */
s32 Particle_MotionStep_IncrementYUntilAtLeast(Particle* particle, f32 limit, f32 increment) {
    f32 temp_fv0;
    s32 var_v1 = 0;

    particle->motionStep.y += increment * sParticleRenderContext.scale.y;

    temp_fv0 = limit * sParticleRenderContext.scale.y;
    if (temp_fv0 <= particle->motionStep.y) {
        particle->motionStep.y = temp_fv0;
        var_v1 = 1;
    }
    return var_v1;
}

/* Add to motionStep.z with render-context Y scaling. */
s32 Particle_MotionStep_IncrementZUntilAtLeast(Particle* particle, f32 limit, f32 increment) {
    f32 temp_fv0;
    s32 var_v1 = 0;

    particle->motionStep.z += increment * sParticleRenderContext.scale.y;

    temp_fv0 = limit * sParticleRenderContext.scale.y;
    if (temp_fv0 <= particle->motionStep.z) {
        particle->motionStep.z = temp_fv0;
        var_v1 = 1;
    }
    return var_v1;
}

/* Decrease the selected field toward its threshold and return the completion result. */
s32 Particle_MotionStep_DecrementXUntilAtMost(Particle* particle, f32 limit, f32 decrement) {
    f32 temp_fv0;
    s32 var_v1 = 0;

    particle->motionStep.x -= decrement * sParticleRenderContext.scale.y;

    temp_fv0 = limit * sParticleRenderContext.scale.y;
    if (particle->motionStep.x <= temp_fv0) {
        particle->motionStep.x = temp_fv0;
        var_v1 = 1;
    }
    return var_v1;
}

/* Decrease the selected field toward its threshold and return the completion result. */
s32 Particle_MotionStep_DecrementYUntilAtMost(Particle* particle, f32 limit, f32 decrement) {
    s32 var_v1 = 0;
    f32 temp_fv0;

    particle->motionStep.y -= decrement * sParticleRenderContext.scale.y;

    temp_fv0 = limit * sParticleRenderContext.scale.y;
    if (particle->motionStep.y <= temp_fv0) {
        particle->motionStep.y = temp_fv0;
        var_v1 = 1;
    }
    return var_v1;
}

/* Decrease the selected field toward its threshold and return the completion result. */
s32 Particle_MotionStep_DecrementZUntilAtMost(Particle* particle, f32 limit, f32 decrement) {
    f32 temp_fv0;
    s32 var_v1 = 0;

    particle->motionStep.z -= decrement * sParticleRenderContext.scale.y;

    temp_fv0 = limit * sParticleRenderContext.scale.y;
    if (particle->motionStep.z <= temp_fv0) {
        particle->motionStep.z = temp_fv0;
        var_v1 = 1;
    }
    return var_v1;
}

/* Scale motionStep.x, motionStep.y, motionStep.z. */
void Particle_MotionStep_ScaleVec(Particle* particle, f32 scale) {
    particle->motionStep.x *= scale;
    particle->motionStep.y *= scale;
    particle->motionStep.z *= scale;
}

/* Approach motionStep.y using the stored or supplied target and step. */
s32 Particle_MotionStep_ApproachY(Particle* particle, f32 target, f32 step) {
    return ParticleMath_ApproachFloat(&particle->motionStep.y, target * sParticleRenderContext.scale.y, step * sParticleRenderContext.scale.y);
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_MotionStep_SetYFromField74AngleX(Particle* particle) {
    particle->motionStep.y = COSS(particle->rotation.x) * particle->stepMagnitude;
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_MotionStep_SetXFromField74AngleY(Particle* particle) {
    particle->motionStep.x = SINS(particle->rotation.y) * particle->stepMagnitude;
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_MotionStep_SetZFromField74AngleY(Particle* particle) {
    particle->motionStep.z = COSS(particle->rotation.y) * particle->stepMagnitude;
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_MotionStep_SetXYFromField74AngleZ(Particle* particle) {
    particle->motionStep.x = COSS(particle->rotation.z) * particle->stepMagnitude;
    particle->motionStep.y = SINS(particle->rotation.z) * particle->stepMagnitude;
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_MotionStep_SetXZFromField74AngleY(Particle* particle) {
    particle->motionStep.x = SINS(particle->rotation.y) * particle->stepMagnitude;
    particle->motionStep.z = COSS(particle->rotation.y) * particle->stepMagnitude;
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_MotionStep_SetVecFromField74AnglesXY(Particle* particle) {
    particle->motionStep.x = particle->stepMagnitude * COSS(particle->rotation.x) * SINS(particle->rotation.y);
    particle->motionStep.y = -particle->stepMagnitude * SINS(particle->rotation.x);
    particle->motionStep.z = particle->stepMagnitude * COSS(particle->rotation.x) * COSS(particle->rotation.y);
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_MotionStep_SetVecFromField74AnglesXZ(Particle* particle) {
    particle->motionStep.x = particle->stepMagnitude * SINS(particle->rotation.x) * SINS(particle->rotation.z);
    particle->motionStep.y = -particle->stepMagnitude * SINS(particle->rotation.x) * COSS(particle->rotation.z);
    particle->motionStep.z = particle->stepMagnitude * COSS(particle->rotation.x);
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_MotionStep_SetVecFromField74AnglesXYAlt(Particle* particle) {
    particle->motionStep.x = particle->stepMagnitude * SINS(particle->rotation.y);
    particle->motionStep.y = -particle->stepMagnitude * SINS(particle->rotation.x) * COSS(particle->rotation.y);
    particle->motionStep.z = particle->stepMagnitude * COSS(particle->rotation.x) * COSS(particle->rotation.y);
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_MotionStep_SetVecFromField74AnglesYZ(Particle* particle) {
    particle->motionStep.x = particle->stepMagnitude * SINS(particle->rotation.y) * COSS(particle->rotation.z);
    particle->motionStep.y = particle->stepMagnitude * SINS(particle->rotation.y) * SINS(particle->rotation.z);
    particle->motionStep.z = COSS(particle->rotation.y);
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_MotionStep_SetVecFromField74AnglesXYZ(Particle* particle) {
    particle->motionStep.x = particle->stepMagnitude * (COSS(particle->rotation.x) * SINS(particle->rotation.y) * COSS(particle->rotation.z) +
                                     SINS(particle->rotation.x) * SINS(particle->rotation.z));
    particle->motionStep.y = -particle->stepMagnitude * (COSS(particle->rotation.x) * SINS(particle->rotation.y) * SINS(particle->rotation.z) -
                                      SINS(particle->rotation.x) * COSS(particle->rotation.z));
    particle->motionStep.z = particle->stepMagnitude * (COSS(particle->rotation.x) * COSS(particle->rotation.y));
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_MotionStep_SetVecFromField74AnglesXYZAlt(Particle* particle) {
    particle->motionStep.x = particle->stepMagnitude * (SINS(particle->rotation.x) * COSS(particle->rotation.y) * SINS(particle->rotation.z) +
                                     COSS(particle->rotation.x) * SINS(particle->rotation.y));
    particle->motionStep.y = -particle->stepMagnitude * SINS(particle->rotation.x) * COSS(particle->rotation.z);

    particle->motionStep.z = particle->stepMagnitude * (-SINS(particle->rotation.x) * SINS(particle->rotation.y) * SINS(particle->rotation.z) +
                                     COSS(particle->rotation.x) * COSS(particle->rotation.y));
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_MotionStep_SetVecFromField74AnglesYZAlt(Particle* particle) {
    particle->motionStep.x = particle->stepMagnitude * SINS(particle->rotation.y) * COSS(particle->rotation.z);
    particle->motionStep.y = particle->stepMagnitude * (COSS(particle->rotation.x) * SINS(particle->rotation.y) * SINS(particle->rotation.z) -
                                     SINS(particle->rotation.x) * COSS(particle->rotation.y));
    particle->motionStep.z = particle->stepMagnitude * (SINS(particle->rotation.x) * SINS(particle->rotation.y) * SINS(particle->rotation.z) +
                                     COSS(particle->rotation.x) * COSS(particle->rotation.y));
}

/* Evaluate the explicit trigonometric component formula and store/add the resulting motion term. */
void Particle_Field7C_SetVecFromField74AnglesYZVariant_0B700(Particle* particle) {
    particle->motionStep.x = particle->stepMagnitude * (SINS(particle->rotation.y) * COSS(particle->rotation.z) +
                                     SINS(particle->rotation.x) * COSS(particle->rotation.y) * SINS(particle->rotation.z));
    particle->motionStep.y = particle->stepMagnitude * (SINS(particle->rotation.y) * SINS(particle->rotation.z) -
                                     SINS(particle->rotation.x) * COSS(particle->rotation.y) * COSS(particle->rotation.z));
    particle->motionStep.z = particle->stepMagnitude * (COSS(particle->rotation.x) * COSS(particle->rotation.y));
}

/* Advance the auxiliary Y angle, replace the XZ motion offset, and store its change in motionStep. */
void Particle_UpdateRotatingMotionOffsetXZ(Particle* particle, f32 radius, s16 angle_step) {
    f32 tmp1 = particle->motionOffset.x;
    f32 tmp2 = particle->motionOffset.z;

    particle->rotationStep.y += angle_step;

    particle->motionOffset.x = radius * sParticleRenderContext.scale.y * SINS(particle->rotationStep.y);
    particle->motionOffset.z = radius * sParticleRenderContext.scale.y * COSS(particle->rotationStep.y);

    particle->motionStep.x = particle->motionOffset.x - tmp1;
    particle->motionStep.z = particle->motionOffset.z - tmp2;
}

/* Initialize ballistic motion under flag 2, subtract vertical acceleration, and integrate until phaseTimer is negative. */
s32 Particle_MotionOffset_AdvanceMotionStepWithTimer(Particle* particle, f32 initial_motion_step_y, f32 motion_step_y_decrement, f32 step_magnitude, s16 rotation_y, s16 duration) {
    s32 sp24 = 0;

    if (Particle_LacksFlags(particle, 2) != 0) {
        Particle_SetFlags(particle, 2);
        particle->motionOffset.x = particle->motionOffset.y = particle->motionOffset.z = 0.0f;
        particle->stepMagnitude = step_magnitude * sParticleRenderContext.scale.y;
        particle->motionStep.y = initial_motion_step_y * sParticleRenderContext.scale.y;
        particle->rotation.y = rotation_y;
        Particle_MotionStep_SetXZFromField74AngleY(particle);
        particle->phaseTimer = duration;
    }

    particle->phaseTimer--;
    if (particle->phaseTimer < 0) {
        sp24 = 1;
    } else {
        particle->motionStep.y -= motion_step_y_decrement * sParticleRenderContext.scale.y;
        Particle_MotionOffset_AddVecFromMotionStep(particle);
    }

    return sp24;
}

/* Set particle scale after multiplying by the render context's Y scale. */
void Particle_Scale_SetScaled(Particle* particle, f32 scale) {
    particle->scale = scale * sParticleRenderContext.scale.y;
}

/* Set particle scale directly, without render-context scaling. */
void Particle_SetScale(Particle* particle, f32 scale) {
    particle->scale = scale;
}

/* Write scaled field 0x20; the stored-scale approach helper uses this field as its target. */
void Particle_SetScaleParameter20(Particle* particle, f32 value) {
    particle->unk_20 = value * sParticleRenderContext.scale.y;
}

/* Write scaled field 0x24; the stored-scale approach helper uses this field as its step. */
void Particle_SetScaleParameter24(Particle* particle, f32 value) {
    particle->unk_24 = value * sParticleRenderContext.scale.y;
}

/* Approach scale using the stored or supplied target and step. */
s32 Particle_Scale_Approach(Particle* particle, f32 target, f32 step) {
    return ParticleMath_ApproachFloat(&particle->scale, target * sParticleRenderContext.scale.y, step * sParticleRenderContext.scale.y);
}

/* Run the field-approach helper and advance lifecycle when it reports completion. */
s32 Particle_Scale_ApproachAndStepState(Particle* particle, f32 target, f32 step) {
    s32 sp1C = Particle_Scale_Approach(particle, target, step);

    if (sp1C != 0) {
        Particle_AdvanceLifecycle(particle);
    }
    return sp1C;
}

/* Approach scale using field 0x20 as target and field 0x24 as step, preserving the actual argument order. */
s32 Particle_ApproachScaleUsingFields20And24(Particle* particle) {
    return ParticleMath_ApproachFloat(&particle->scale, particle->unk_20, particle->unk_24);
}

/* Run the field-approach helper and advance lifecycle when it reports completion. */
s32 Particle_ApproachStoredScaleAndAdvanceLifecycle(Particle* particle) {
    s32 sp1C = Particle_ApproachScaleUsingFields20And24(particle);

    if (sp1C != 0) {
        Particle_AdvanceLifecycle(particle);
    }
    return sp1C;
}

/* Approach unk_20 using the stored or supplied target and step. */
s32 Particle_Field20_Approach_0BA84(Particle* particle, f32 target, f32 step) {
    return ParticleMath_ApproachFloat(&particle->unk_20, target * sParticleRenderContext.scale.y, step * sParticleRenderContext.scale.y);
}

/* Set primRed, primGreen, primBlue. */
void Particle_SetPrimColor(Particle* particle, u8 red, u8 green, u8 blue) {
    particle->primRed = red;
    particle->primGreen = green;
    particle->primBlue = blue;
}

/* Approach primRed using the stored or supplied target and step. */
s32 Particle_PrimR_Approach_0BAE4(Particle* particle, u8 target, u8 step) {
    return ParticleMath_ApproachU8(&particle->primRed, target, step);
}

/* Approach primGreen using the stored or supplied target and step. */
s32 Particle_PrimG_Approach_0BB14(Particle* particle, u8 target, u8 step) {
    return ParticleMath_ApproachU8(&particle->primGreen, target, step);
}

/* Approach primBlue using the stored or supplied target and step. */
s32 Particle_PrimB_Approach_0BB44(Particle* particle, u8 target, u8 step) {
    return ParticleMath_ApproachU8(&particle->primBlue, target, step);
}

/* Set envRed, envGreen, envBlue. */
void Particle_SetEnvColor(Particle* particle, u8 red, u8 green, u8 blue) {
    particle->envRed = red;
    particle->envGreen = green;
    particle->envBlue = blue;
}

/* Approach envRed using the stored or supplied target and step. */
s32 Particle_EnvR_Approach_0BB90(Particle* particle, u8 target, u8 step) {
    return ParticleMath_ApproachU8(&particle->envRed, target, step);
}

/* Approach envBlue using the stored or supplied target and step. */
s32 Particle_ApproachEnvBlue(Particle* particle, u8 target, u8 step) {
    return ParticleMath_ApproachU8(&particle->envBlue, target, step);
}

/* Approach envGreen using the stored or supplied target and step. */
s32 Particle_ApproachEnvGreen(Particle* particle, u8 target, u8 step) {
    return ParticleMath_ApproachU8(&particle->envGreen, target, step);
}

/* Set primAlpha. */
void Particle_SetPrimAlpha(Particle* particle, u8 alpha) {
    particle->primAlpha = alpha;
}

/* Approach primAlpha using the stored or supplied target and step. */
s32 Particle_PrimAlpha_Approach_0BC2C(Particle* particle, u8 target_alpha, u8 alpha_step) {
    return ParticleMath_ApproachU8(&particle->primAlpha, target_alpha, alpha_step);
}

/* Run the field-approach helper and advance lifecycle when it reports completion. */
s32 Particle_PrimAlpha_ApproachAndStepState_0BC5C(Particle* particle, u8 target_alpha, u8 alpha_step) {
    s32 sp1C = Particle_PrimAlpha_Approach_0BC2C(particle, target_alpha, alpha_step);

    if (sp1C != 0) {
        Particle_AdvanceLifecycle(particle);
    }
    return sp1C;
}

/* Set unk_A6. */
void Particle_FieldA6_SetS16_0BCA8(Particle* particle, s16 value) {
    particle->unk_A6 = value;
}

/* Return the particle's unk_A6 field. */
s16 Particle_FieldA6_GetS16_0BCBC(Particle* particle) {
    return particle->unk_A6;
}

/* Decrease the selected field toward its threshold and return the completion result. */
s32 Particle_FieldA6_DecrementTo_0BCC4(Particle* particle, s16 target, s16 step) {
    s32 var_v1 = 0;

    particle->unk_A6 -= step;
    if (target >= particle->unk_A6) {
        particle->unk_A6 = target;
        var_v1 = 1;
    }
    return var_v1;
}

/* Decrease the selected field toward its threshold and return the completion result. */
s32 Particle_FieldA6_DecrementToZero_0BD08(Particle* particle, s16 step) {
    return Particle_FieldA6_DecrementTo_0BCC4(particle, 0, step);
}

/* Run the countdown helper and advance lifecycle when it reports completion. */
s32 Particle_FieldA6_DecrementToZeroAndStepState_0BD34(Particle* particle, s16 step) {
    s32 sp1C = Particle_FieldA6_DecrementTo_0BCC4(particle, 0, step);

    if (sp1C != 0) {
        Particle_AdvanceLifecycle(particle);
    }
    return sp1C;
}

/* Decrease the selected field toward its threshold and return the completion result. */
s32 Particle_FieldA6_DecrementToStep1_0BD80(Particle* particle, s16 target) {
    return Particle_FieldA6_DecrementTo_0BCC4(particle, target, 1);
}

/* Decrease the selected field toward its threshold and return the completion result. */
s32 Particle_FieldA6_DecrementToZeroStep1_0BDAC(Particle* particle) {
    return Particle_FieldA6_DecrementTo_0BCC4(particle, 0, 1);
}

/* Run the countdown helper and advance lifecycle when it reports completion. */
s32 Particle_FieldA6_DecrementToZeroStep1AndStepState_0BDD0(Particle* particle) {
    s32 sp1C = Particle_FieldA6_DecrementTo_0BCC4(particle, 0, 1);

    if (sp1C != 0) {
        Particle_AdvanceLifecycle(particle);
    }
    return sp1C;
}

/* Set unk_A8. */
void Particle_FieldA8_SetS16_0BE14(Particle* particle, s16 value) {
    particle->unk_A8 = value;
}

/* Return the particle's unk_A8 field. */
s16 Particle_FieldA8_GetS16_0BE28(Particle* particle) {
    return particle->unk_A8;
}

/* Decrease the selected field toward its threshold and return the completion result. */
s32 Particle_FieldA8_DecrementTo_0BE30(Particle* particle, s16 target) {
    s32 var_v1 = 0;

    particle->unk_A8--;
    if (target >= particle->unk_A8) {
        particle->unk_A8 = target;
        var_v1 = 1;
    }
    return var_v1;
}

/* Decrease the selected field toward its threshold and return the completion result. */
s32 Particle_FieldA8_DecrementToZeroStep1_0BE6C(Particle* particle) {
    return Particle_FieldA8_DecrementTo_0BE30(particle, 0);
}

/* Run the countdown helper and advance lifecycle when it reports completion. */
s32 Particle_FieldA8_DecrementToZeroStep1AndStepState_0BE8C(Particle* particle) {
    s32 sp1C = Particle_FieldA8_DecrementTo_0BE30(particle, 0);

    if (sp1C != 0) {
        Particle_AdvanceLifecycle(particle);
    }
    return sp1C;
}

/* Set unk_CA. */
void Particle_FieldCA_SetU8_0BECC(Particle* particle, u8 value) {
    particle->unk_CA = value;
}

/* Set unk_AC. */
void Particle_FieldAC_SetS16_0BED8(Particle* particle, s16 value) {
    particle->unk_AC = value;
}

/* Set unk_CF. */
void Particle_FieldCF_SetU8_0BEEC(Particle* particle, u8 value) {
    particle->unk_CF = value;
}

/* Set unk_CF. */
void Particle_FieldCF_SetU8Alt_0BEF8(Particle* particle, u8 value) {
    particle->unk_CF = value;
}

/* Set unk_CC. */
void Particle_FieldCC_SetU8_0BF04(Particle* particle, u8 value) {
    particle->unk_CC = value;
}

/* Set unk_AA. */
void Particle_FieldAA_SetS16_0BF10(Particle* particle, s16 value) {
    particle->unk_AA = value;
}

/* Set unk_CE. */
void Particle_FieldCE_SetS16_0BF24(Particle* particle, s16 value) {
    particle->unk_CE = value;
}

/* Set unk_CD. */
void Particle_FieldCD_SetS16_0BF38(Particle* particle, s16 value) {
    particle->unk_CD = value;
}

/* Set unk_AE. */
void Particle_FieldAE_SetS16_0BF4C(Particle* particle, s16 value) {
    particle->unk_AE = value;
}

/* Approach unk_AE using the stored or supplied target and step. */
s32 Particle_FieldAE_Approach_0BF60(Particle* particle, s16 target, s16 step) {
    return ParticleMath_ApproachS16(&particle->unk_AE, target, step);
}

/* Run the field-approach helper and advance lifecycle when it reports completion. */
s32 Particle_FieldAE_ApproachAndStepState_0BF98(Particle* particle, s16 target, s16 step) {
    s32 sp1C = Particle_FieldAE_Approach_0BF60(particle, target, step);

    if (sp1C != 0) {
        Particle_AdvanceLifecycle(particle);
    }
    return sp1C;
}

/* Set unk_B0. */
void Particle_FieldB0_SetS16_0BFEC(Particle* particle, s16 value) {
    particle->unk_B0 = value;
}

/* Approach unk_B0 using the stored or supplied target and step. */
s32 Particle_FieldB0_Approach_0C000(Particle* particle, s16 target, s16 step) {
    return ParticleMath_ApproachS16(&particle->unk_B0, target, step);
}

/* Return the particle's unk_AC field. */
s16 Particle_FieldAC_GetS16_0C038(Particle* particle) {
    return particle->unk_AC;
}

/* Return the particle's unk_CE field. */
s32 Particle_FieldCE_GetS32_0C040(Particle* particle) {
    return particle->unk_CE;
}

/* Return the particle's unk_CD field. */
u8 Particle_FieldCD_GetU8_0C048(Particle* particle) {
    return particle->unk_CD;
}

/* Return the particle's unk_AE field. */
s16 Particle_FieldAE_GetS16_0C050(Particle* particle) {
    return particle->unk_AE;
}

/* Return field 0xCF through the original s8 cast, sign-extending its byte to s16. */
s16 Particle_FieldCF_GetS16_0C058(Particle* particle) {
    s32 tmp = particle->unk_CF;
    return tmp;
}

/* Return the particle's unk_AA field. */
s16 Particle_FieldAA_GetS16_0C068(Particle* particle) {
    return particle->unk_AA;
}

/* Store the parameter only when it differs from the field-specific sentinel. */
void Particle_SetFieldOffsetACIfNotSentinel(Particle* particle, s16 value) {
    if ((u32)particle != -1) {
        Particle_FieldAC_SetS16_0BED8(particle, value);
    }
}

/* Store the parameter only when it differs from the field-specific sentinel. */
void Particle_SetFieldOffsetCFIfNotSentinel(Particle* particle, u8 value) {
    if ((u32)particle != -1) {
        Particle_FieldCF_SetU8_0BEEC(particle, value);
    }
}

/* Store the parameter only when it differs from the field-specific sentinel. */
void Particle_SetFieldOffsetA6IfNotSentinel(Particle* particle, s16 value) {
    if ((u32)particle != -1) {
        Particle_FieldA6_SetS16_0BCA8(particle, value);
    }
}

/* Store the parameter only when it differs from the field-specific sentinel. */
void Particle_SetFieldOffsetA8IfNotSentinel(Particle* particle, s16 value) {
    if ((u32)particle != -1) {
        Particle_FieldA8_SetS16_0BE14(particle, value);
    }
}

/* Store the parameter only when it differs from the field-specific sentinel. */
void Particle_SetFieldOffsetCEIfNotSentinel(Particle* particle, u8 value) {
    if ((u32)particle != -1) {
        particle->unk_CE = value;
    }
}

/* Store the parameter only when it differs from the field-specific sentinel. */
void Particle_SetFieldOffsetCDIfNotSentinel(Particle* particle, u8 value) {
    if ((u32)particle != -1) {
        particle->unk_CD = value;
    }
}

/* Store the parameter only when it differs from the field-specific sentinel. */
void Particle_SetFieldOffsetAAIfNotSentinel(Particle* particle, s16 value) {
    if ((u32)particle != -1) {
        particle->unk_AA = value;
    }
}

/* Store the parameter only when it differs from the field-specific sentinel. */
void Particle_SetPrimAlphaIfNotSentinel(Particle* particle, u8 alpha) {
    if ((u32)particle != -1) {
        Particle_SetPrimAlpha(particle, alpha);
    }
}

/* Store the parameter only when it differs from the field-specific sentinel. */
void Particle_SetFieldOffsetAEIfNotSentinel(Particle* particle, s16 value) {
    if ((u32)particle != -1) {
        particle->unk_AE = value;
    }
}

/* Store the parameter only when it differs from the field-specific sentinel. */
void Particle_SetFieldOffsetB0IfNotSentinel(Particle* particle, s16 value) {
    if ((u32)particle != -1) {
        particle->unk_B0 = value;
    }
}

/* Prepare each allocated non-ortho chain once, then draw its enabled members whose lifecycle is at least 2. */
void Particle_RenderWorldChains(CameraNode* camera) {
    s32 i;
    Particle* var_s0;
    Particle* var_s2 = sParticlePool;

    for (i = 0; i < 0x12C; i++, var_s2++) {
        if ((var_s2->allocated != 0) && (var_s2->prev == NULL) && (Particle_LacksFlags(var_s2, 8) != 0)) {
            Particle_PrepareRender(var_s2);
            var_s0 = var_s2;
            while (var_s0 != NULL) {
                if ((Particle_HasFlags(var_s0, 4) != 0) && (var_s0->lifecycle >= 2)) {
                    Particle_DispatchDraw(var_s0, camera);
                }
                var_s0 = var_s0->next;
            }
        }
    }
}

/* Prepare and draw allocated flag-8 particles individually when enabled and lifecycle is at least 2. */
void Particle_RenderOrthoParticles(CameraNode* camera) {
    s32 i;
    Particle* var_s0 = sParticlePool;

    for (i = 0; i < 0x12C; i++, var_s0++) {
        if ((var_s0->allocated != 0) && (Particle_HasFlags(var_s0, 8) != 0)) {
            Particle_PrepareRender(var_s0);
            if ((Particle_HasFlags(var_s0, 4) != 0) && (var_s0->lifecycle >= 2)) {
                Particle_DispatchDraw(var_s0, camera);
            }
        }
    }
}

/* Select one of four display-list presets; unknown selectors emit no commands. */
void Particle_SetRenderMode(s16 render_mode) {
    switch (render_mode) {
        case 0:
            gSPDisplayList(gDisplayListHead++, D_8140DD58);
            break;

        case 2:
            gSPDisplayList(gDisplayListHead++, D_8140DC78);
            break;

        case 1:
            gSPDisplayList(gDisplayListHead++, D_8140DCD8);
            break;

        case 3:
            gSPDisplayList(gDisplayListHead++, D_8140DC00);
            break;
    }
}

/* Invoke the descriptor setup callback with an inline argument or the word referenced by its argument pointer. */
void Particle_InvokeRenderSetup(Particle* particle, ParticleDescriptorChild* setup) {
    switch (setup->argumentSource) {
        case 0:
            setup->setupCallback(particle, setup->setupArgument.arg, setup);
            break;

        case 1:
            setup->setupCallback(particle, setup->setupArgument.s->arg, setup);
            break;
    }
}

/* For descriptor kind 1, select render mode and run setup once per chain when setup scope is 1. */
void Particle_PrepareRender(Particle* particle) {
    UNUSED s32 pad;
    ParticleDescriptorChild* sp18 = particle->descriptor->payload.a;

    if (particle->descriptor->kind == 1) {
        Particle_SetRenderMode(sp18->renderMode);
        if (sp18->setupScope == 1) {
            Particle_InvokeRenderSetup(particle, sp18);
        }
    }
}

/* For kind 1, optionally run per-particle setup then draw; kind 3 calls the direct render callback. */
void Particle_DispatchDraw(Particle* particle, CameraNode* camera) {
    UNUSED s32 pad;
    ParticleDescriptorChild* sp18 = particle->descriptor->payload.a;

    switch (particle->descriptor->kind) {
        case 1:
            if (sp18->setupScope == 0) {
                Particle_InvokeRenderSetup(particle, sp18);
            }
            sp18->drawCallback(particle, camera, sp18);
            break;

        case 3:
            particle->descriptor->payload.func(particle, camera);
            break;
    }
}

/* Empty callback retained at its original entry point. */
void Particle_NoopDraw(UNUSED Particle* particle, UNUSED CameraNode* camera) {
}
