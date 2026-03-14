# Ozz-animation sample: Animations browser

## Description

Loads a skeleton, a list of animations and a skinned mesh and lets you browse through and preview all animations.

## Concept

This sample reads a list of animation archive files from a text file (one animation per line) and groups them by name prefix. It then allows selecting any animation, plays it back using the built-in playback controller and displays the result by rendering a skeleton and/or a skinned mesh.

The animation sampling pipeline is:

1. Sample the animation using `ozz::animation::SamplingJob`.
2. Convert sampled local-space joint transforms to model-space matrices using `ozz::animation::LocalToModelJob`.
3. Optionally skin meshes using the computed model-space matrices.

## Sample usage

The sample UI exposes:

- A list of animations grouped by name prefix, with buttons to select the next animation or pick any animation from a group.
- Playback controls (play/pause, scrub time, speed) via the built-in playback controller.
- Rendering toggles:
  - Draw skeleton.
  - Draw mesh.
  - Show triangles, texture, vertices, normals, tangents, binormals, colors.
  - Wireframe.
  - Skip skinning.

Command line options (with defaults):

- `--skeleton=<path>` (default: `media/skeleton.ozz`)
- `--animations=<path>` (default: `media/animations.txt`)
- `--mesh=<path>` (default: `media/mesh.ozz`)

## Implementation

1. Load the skeleton archive.
2. Read the animations list file and build a sorted list of animation archive filenames.
3. Randomly pick an animation at startup and load it from `media/<animation_name>`.
4. Allocate runtime buffers:
   - Local-space transforms (`ozz::math::SoaTransform`) for sampling.
   - Model-space matrices (`ozz::math::Float4x4`) for rendering.
   - Skinning matrices computed from model matrices and the mesh inverse bind poses.
5. Every frame:
   1. Update animation time using the playback controller.
   2. Sample the current animation with `ozz::animation::SamplingJob`.
   3. Convert local-space transforms to model-space using `ozz::animation::LocalToModelJob`.
   4. Update skinning matrices (if rendering a mesh) and render the skeleton/mesh.
