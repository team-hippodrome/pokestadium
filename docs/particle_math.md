# Particle fragment annotation

This is a source-derived annotation of fragment 34 (`particle_math`), not recovered
original identifiers or a claim about how an effect looks in game. The six C
translation units and their shared header remain in their existing ROM order.

## Source map

| Source | Responsibility |
| --- | --- |
| `particle_math_261F90.c` | Shared state, random sampling, scalar/vector helpers, float transforms and matrix-pool submission |
| `particle_math_264380.c` | Render presets, geometry templates, texture uploads, tile scrolling and descriptor materials |
| `particle_math_269820.c` | Pool/chains, creation, lifecycle, frame animation, position/rotation/color helpers and render dispatch |
| `particle_math_26E540.c` | One particle clock and its divided/remainder/parity derivatives |
| `particle_math_26E6D0.c` | Fixed-format texture callbacks, draw callbacks and palette operations |
| `particle_math_26F9B0.c` | Constant-one entry point; its larger contract remains unknown |

## Position and state evidence

`Particle_UpdateWorldTransform` computes
`worldPosition = origin + positionOffset + motionOffset`, component by component.
It then copies that position, rotation, scale and primitive alpha to an associated
`DisplayObject`. Offset `0x50` is therefore named `motionOffset`, not universally
velocity: helpers add `motionStep` to it, but the world-transform helper adds the
stored value directly to position. The base-offset random helpers write `0x38`,
despite the old names calling it velocity.

`allocated` is the pool ownership marker used by `Particle_Alloc`, `Particle_Free`
and `Particle_CountActive`. It is distinct from `lifecycle`, which starts at one
and must be at least two for the normal draw passes. `phase` is a separate
effect-local state. `Particle_TickDelayTimer` finishes at zero, while
`Particle_TickPhaseTimer` finishes below zero; these are not interchangeable.
Frame animations separately use `frameTimer`, `textureFrame` and `loopsRemaining`.

The helpers operating on `rotationStep` also reuse that vector as an evolving
angle in orbital motion. Likewise, `stepMagnitude` and `magnitudeStep` are generic
motion scalars, not a universal physical speed/acceleration contract.

## Material evidence

`ParticleGfx_AppendDescriptorMaterial` consumes a `ParticleMaterial`: optional
members select combiner, primitive/environment color, texture generation,
primary texture and secondary texture commands. `ParticleGfx_LoadDescriptorTextureAndTile`
selects either a static texture or a divided-clock sequence and optionally
appends scrolling tile bounds. `ParticleGfx_SetCombineModeFromDescriptor` packs
the sixteen mux bytes; the format, sequence, scroll and mux type names describe
these readers rather than an inferred visual effect.

`Particle_PrepareRender`, `Particle_DispatchDraw` and
`Particle_InvokeRenderSetup` distinguish chain-level setup, particle-level setup
and direct-render descriptors. Their legacy callback signatures and casts are
preserved, including the existing s32 draw callbacks without return statements.

## Deliberate unknowns and preservation

- Fields `0x20` and `0x24` retain neutral parameter names. The stored-scale
  approach helper passes `0x20` as target and `0x24` as step, contrary to the old
  setter names. No argument order or computation was corrected.
- Effect-specific scratch values, descriptor fields without established roles,
  flag `0x40`, and address-named geometry templates remain explicit.
- Empty callbacks, duplicate declarations, truncation/sign-extension, sentinel
  values, random distribution, float literal types and operation order are not
  cleaned up.
- Shared-field edits are type-aware; required usage-site and regional symbol
  renames belong to this annotation, not a separate behavior refactor.

Validation records for the exact patch are maintained with the PR preparation
artifacts. ROM equality tests preservation only; no emulator or visual validation
is claimed.
