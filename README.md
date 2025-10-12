# Custom OpenGL Engine & 3D Game

> A two-phase graphics project: a custom OpenGL rendering engine (Phase 1) and a 3D game built on top of it (Phase 2).

---

## Table of Contents

- [Overview](#overview)
- [Goals](#goals)
  - [Phase 1 — Engine](#phase-1---engine)
  - [Phase 2 — Game](#phase-2---game)
- [Key Features](#key-features)
- [Architecture](#architecture)
  - [Entity-Component-System (ECS)](#entity-component-system-ecs)
  - [Asset Loaders and Asset Management](#asset-loaders-and-asset-management)
  - [Forward Renderer & Pipeline State](#forward-renderer--pipeline-state)
- [Scene File Format](#scene-file-format)
  - [What belongs in the scene file](#what-belongs-in-the-scene-file)
  - [Example (JSON)](#example-json)
- [Materials & Shaders](#materials--shaders)
- [Post-processing & Sky](#post-processing--sky)
- [Lighting Model](#lighting-model)
- [Collision & Physics](#collision--physics)
- [Build & Run](#build--run)
  - [Prerequisites](#prerequisites)
  - [Build instructions (CMake)](#build-instructions-cmake)
- [Project Structure (recommended)](#project-structure-recommended)
- [How to Add/Change Scenes and Assets](#how-to-addchange-scenes-and-assets)
- [Extending the Engine](#extending-the-engine)
- [Performance Tips](#performance-tips)
- [Testing & Debugging](#testing--debugging)
- [Known Limitations](#known-limitations)
- [License](#license)
- [Credits & References](#credits--references)
- [Contact](#contact)

---

## Overview

This repository contains a custom OpenGL-based rendering engine implemented in C++ with CMake, followed by a simple 3D game built on top of that engine. The engine is designed around an **ECS (Entity-Component-System)** architecture and supports scene deserialization from external data files, forward rendering, configurable pipeline state, post-processing, real-time lighting, textured materials, and basic collision handling.

The project is structured as a learning-focused engine that can be used as the basis for any simple 3D game and demonstrates low-level graphics concepts and engine patterns.

---

## Goals

### Phase 1 - Engine
- Implement an engine able to deserialize scenes from external files.
- Support assets (shaders, textures, meshes) managed by singleton asset loaders.
- Provide an ECS-based runtime with components (e.g., `MeshRenderer`, `Camera`) and systems (e.g., `ForwardRenderer`).
- Support configurable rendering pipeline state per material (culling, depth test, blending, masks).
- Implement post-processing and sky rendering hooks.

### Phase 2 - Game
- Build a 3D game using the engine.
- Use real 3D models, textures, multiple dynamic lights, and at least one postprocessing effect.
- Implement collision detection and basic game logic / objectives.
- Ensure all scenes are externally defined and loadable without recompilation.

---

## Key Features

- **ECS architecture** with easily-extendable components and systems
- **Scene deserialization**: assets, entities, components, and pipeline configured via data files
- **Asset management**: reusable assets (meshes, materials, shaders, textures, samplers)
- **Forward rendering pipeline** with correct opaque/transparent sorting
- **Per-material pipeline state** (back-face culling, depth testing, blending, color/depth mask)
- **Post-processing pipeline** support (framebuffer-based effects)
- **Lighting**: multiple light types supported and combined in shaders
- **Lit material** supporting standard texture maps (albedo, specular, roughness, AO, emission)
- **Camera component** with perspective/orthographic modes and scene graph transform hierarchy
- **Collision detection & basic physics integration** (external physics engine allowed)
- **Modular design**: add new components and systems with minimal coupling

---

## Architecture

### Entity-Component-System (ECS)

- **World**: container for entities (a linear list or other container)
- **Entity**: lightweight ID + name, parent pointer, local transform; holds a list of components
- **Component**: abstract base class with virtual methods (e.g., `deserialize()`, `update()`)
- **Systems**: process entities that have required components (e.g., `ForwardRenderer` looks for `Camera` and `MeshRenderer`)

Common components:
- `Transform` — local transform and parent-child relationship
- `MeshRenderer` — references to mesh and material
- `Camera` — perspective/orthographic parameters
- `Light` — color, type, cone angles (position and direction come from Transform)
- `Collider` — collider shape data for collision detection

Design principles:
- Components should be serializable/deserializable
- Assets referenced by components are loaded through asset loaders by name
- Systems should not own components; they query the world and operate on component sets

### Asset Loaders and Asset Management

- **Singleton-like loaders** (or centralized `AssetManager`) map string names to loaded objects
- Asset types: `ShaderProgram`, `Mesh`, `Texture`, `Sampler`, `Material`
- Loaders are responsible for file IO, GPU resource creation, and caching
- Asset deserialization: scene file lists assets by name and path; loaders construct the runtime objects

### Forward Renderer & Pipeline State

- The forward renderer loops over entities to find the main camera(s) and drawable entities
- Rendering order: **opaque** objects first (any order), then **sky**, then **transparent** objects sorted back-to-front
- Each `Material` includes a `PipelineState` struct describing culling, depth testing, blending, masks
- Rendering steps:
  1. Bind shader
  2. Set shader uniforms (camera matrices, material params, lights)
  3. Bind textures & samplers
  4. Configure pipeline state (glEnable/glDisable / glDepthFunc / glBlendFunc etc.)
  5. Draw mesh

---

## Scene File Format

All scene data lives in external files so scenes can be changed without recompilation. The example below uses JSON, but YAML or a custom format is also acceptable.

### What belongs in the scene file
- **Assets**: shaders, meshes, textures, samplers, materials (with pipeline state)
- **Entities**: list of entities with name, parent, transform, and a list of components
- **Systems**: optional, to configure system-level settings (e.g., post-processing chain)
- **Scene metadata**: sky settings, default camera name, exposure, etc.

### Example (JSON)

```json
{
  "assets": {
    "shaders": {
      "basic": "assets/shaders/basic.glsl",
      "pbr": "assets/shaders/pbr.glsl"
    },
    "meshes": {
      "cube": "assets/models/cube.obj",
      "environment": "assets/models/scene.obj"
    },
    "textures": {
      "sky": "assets/textures/sky.png"
    },
    "materials": {
      "wall": {
        "type": "lit",
        "shader": "pbr",
        "textures": {"albedo": "assets/textures/wall_albedo.png"},
        "pipeline": {"cull": "back", "depth_test": true}
      }
    }
  },
  "entities": [
    {
      "name": "MainCamera",
      "components": [
        {"type": "Transform", "position": [0,2,5]},
        {"type": "Camera", "fov": 60, "near": 0.1, "far": 100}
      ]
    },
    {
      "name": "Room",
      "components": [
        {"type": "Transform"},
        {"type": "MeshRenderer", "mesh": "environment", "material": "wall"}
      ]
    }
  ],
  "postprocessing": ["bloom"]
}
```

Keep scene files human-editable. Asset names referenced by components should match keys in the `assets` section.

---

## Materials & Shaders

- `Material` is an abstract class. Subclasses include:
  - `TintedMaterial` — uniform color, no textures
  - `TexturedMaterial` — base color texture
  - `LitMaterial` — supports PBR textures (albedo, normal, roughness, metalness, AO, emission)
- Each material contains a `PipelineState` and references to required textures and the shader program.
- Shader uniform conventions: standard names for matrices and material maps (e.g., `u_ViewProj`, `u_Model`, `u_Albedo`)

---

## Post-processing & Sky

- Post-processing uses a framebuffer object (FBO) pipeline: render scene into texture(s), then process via full-screen quad passes.
- Common effects to include: tone mapping, bloom, gamma correction, screen-space ambient occlusion (SSAO), color grading.
- Sky is implemented as a textured skybox or skydome with optional shader for atmospheric scattering.

---

## Lighting Model

- Light component stores light parameters (color, intensity, type-specific params) excluding position/direction which are derived from entity transform.
- Support multiple light types: directional, point, spot.
- Shaders accumulate multiple lights per-object; consider a reasonable upper limit or use light culling for many lights.
- Lit material shader should combine textures and lighting using a physically-based shading model or a simplified Blinn-Phong model depending on scope.

---

## Collision & Physics

- Collision detection (ray-picking, AABB/OBB, sphere) is required for the game phase. You may integrate an external physics library (e.g., Bullet) for rigid-body physics and collision resolution.
- `Collider` components hold collision shape data and optional physics parameters (mass, friction).
- Implement simple collision callbacks or query APIs for game logic (e.g., `onCollide`, `raycastFromCamera`).

---

## Build & Run

### Prerequisites

- C++17 or newer compiler
- CMake (>= 3.10)
- OpenGL development headers (GL 3.3+ ideally)
- Libraries (suggested):
  - GLFW (window & input)
  - GLAD or GLEW (OpenGL loader)
  - GLM (math)
  - stb_image (image loading) or equivalent
  - Assimp (model loading)
  - Optional: Bullet (physics), Dear ImGui (debug UI)

> You can choose alternative libraries if preferred; adjust `CMakeLists.txt` accordingly.

### Build (example)

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

On success, the engine executable(s) will be in `build/bin` (or as configured by `CMakeLists.txt`).

### Run

```bash
./bin/engine --scene ../scenes/example_scene.json
```

Provide an argument to point to a scene file. The engine should fallback to a default scene if the argument is omitted.

---

## Project Structure

```
/ (repo root)
├─ CMakeLists.txt
├─ README.md
├─ src/
│  ├─ core/          # ECS core, world, entity, component base
│  ├─ systems/       # ForwardRenderer, PhysicsSystem, etc.
│  ├─ components/    # MeshRenderer, Camera, Light, Collider
│  ├─ assets/        # Asset loader implementations
│  ├─ platform/      # Window and input (GLFW glue)
│  └─ app/           # main, application loop, CLI
├─ shaders/
├─ assets/
│  ├─ models/
│  ├─ textures/
│  └─ scenes/
├─ examples/        # example scenes and helper tools
└─ third_party/     # external libs (optional)
```

---

## How to Add / Change Scenes and Assets

1. Add files to `assets/models`, `assets/textures`, or `shaders/`.
2. Update or create a new scene JSON file in `assets/scenes/` referencing those assets.
3. Run the engine with `--scene path/to/scene.json`.

Make sure asset keys in the scene file match loader keys.

---

## Extending the Engine

- **New Component**: derive from `Component`, implement `deserialize()` and relevant update hooks; register so the deserializer can instantiate by type name.
- **New System**: add a system class that queries the world for component sets and performs updates. Register it in the application initialization.
- **New Material/Shader**: create a shader, add a material subclass if needed, and add an entry in the scene `assets.materials` section.
- **Renderer**: implement additional render passes (deferred, shadow maps, etc.) but ensure the scene format or a config option can enable them.

---

## Performance Tips

- Batch draw calls when possible by grouping by shader/material.
- Minimize state changes (textures, shader binds) during rendering.
- Use indexed VBOs/VAOs and static/dynamic usage hints appropriately.
- Use texture atlases where helpful and normal map compression for large textures.
- Profile with tools like RenderDoc and platform profilers.

---

## Testing & Debugging

- Add an in-engine debug UI (Dear ImGui) for live tweaking of cameras, lights, and material parameters.
- Validate scene deserialization with unit tests or small Python scripts.
- Provide verbose logging for asset loading failures and shader compile errors.
- Include debug draw modes (wireframe, normals visualization, bounding boxes).

---

## Known Limitations

- The engine is intended as an educational/simple engine; it is not optimized for AAA-scale performance.
- Light count is limited by the shader strategy; large numbers of dynamic lights will require light-culling or deferred approaches.
- Asset loaders assume common file formats (OBJ, glTF via Assimp); custom formats require additional parsers.

---

## License

This repository is released under the MIT License. See `LICENSE` for details.

---

## Credits & References

- Third-party libraries: GLFW, GLAD, GLM, stb_image, Assimp, Bullet, Dear ImGui
- Research & tutorials used for reference: OpenGL docs, ECS design patterns, PBR/lighting references

---