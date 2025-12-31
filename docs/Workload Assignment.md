# How to use this roadmap

1. Each person self-assigns a **Track** (1 -> 4).
    
2. Each person implements milestones in the track in any order; milestones are intentionally orthogonal.
    
3. Use the provided **stubs** during early integration: e.g., implement `IAssetManager` interface returning fake assets so renderers/tests can run without real loaders.
    
4. Create a branch per milestone. Test each milestone.
    

---

## TRACK 1 - Core & Platform (Engine bootstrap, window, input, scheduler, GL context)

**Owner:** --

### T1.1 - Application skeleton & CLI

- **Goal:** `Application` class with CLI entry (`src/app/main.cpp`) that parses args and calls `Application::run()`.
    
- **Deliverables:** `main.cpp`, `Application` class, small README on how to run.
    
- **Acceptance:** `./app --help` prints usage; `Application::run()` starts and exits cleanly.
        
- **Related classes / seq:** `Application`, `Engine`. (startup)
    

### T1.2 - Window wrapper (GLFW) 

- **Goal:** `Window` class that creates a GLFW window
    
- **Deliverables:** `Window.hpp/cpp`
    
- **Acceptance:** On desktop, window opens. implements `create()`/`destroy()` and returns success.
    
- **Deps / stubs:** GLFW optional.
    
- **Related classes / seq:** `Window`, `Application`. (startup)
    

### T1.3 - GLContext (glad) init + capability query

- **Goal:** `GLContext` that loads OpenGL functions (using glad) and exposes version/extension queries.
    
- **Deliverables:** `GLContext.hpp/cpp`
    
- **Acceptance:** On real run, GL functions load.
    
- **Deps / stubs:** Needs Window; can use `HeadlessWindow`.
    
- **Related classes / seq:** `GLContext`. (startup)
    

### T1.4 - FrameScheduler & main loop driver

- **Goal:** `FrameScheduler` that provides `deltaTime()` and tick control hooks; `Application` drives loop calling `Engine::update()` with 'dt' (deltatime).
    
- **Deliverables:** `FrameScheduler.hpp/cpp`.
    
- **Acceptance:** Loop runs and invokes update callbacks; can toggle fixed-step vs variable-step.
    
- **Related classes / seq:** `FrameScheduler`, `Application`. (game loop)
    

### T1.5 - Input polling abstraction

- **Goal:** `Input` class that polls GLFW or a stub and exposes keyboard/mouse API.
    
- **Deliverables:** `Input.hpp/cpp`, unit tests with stub input events.
    
- **Acceptance:** Tests simulate key presses and ensure `Input` reports state.
    
- **Deps / stubs:** Window for real input.
    
- **Related classes / seq:** `Input`, `CameraControlSystem`. (game loop / camera control)
    

---

## TRACK 2 - Rendering & Assets (Shaders, Mesh, Texture, Materials, simple renderer)

**Owner:** --

### T2.1 - Rendering primitives & GL resource management

- **Goal:** Implement `Shader`, `Texture`, `Mesh`, `Framebuffer` wrappers (create/destroy).
    
- **Deliverables:** `Shader.hpp/cpp`, `Texture.hpp/cpp`, `Mesh.hpp/cpp`.
    
- **Acceptance:** Create/destroy functions call underlying GL objects can be bound/unbound without errors.
    
- **Deps / stubs:** Depends on `GLContext`.
    
- **Related classes / seq:** `Shader`, `Mesh`, `Texture`, `Framebuffer`. (render loop)
    

### T2.2 - Material abstraction & pipeline state

- **Goal:** `Material` base + `TintedMaterial` and `TexturedMaterial` implementations; `PipelineState` object.
    
- **Deliverables:** `MaterialManager` interface stub, `Material` classes implementing `bind()` behavior.
    
- **Acceptance:** `Material.bind()` sets pipeline state.
    
- **Deps / stubs:** No real assets required.
    
- **Related classes / seq:** `Material`, `PipelineState`. (render loop)
    

### T2.3 - Mesh loader interface & tiny_obj_loader integration

- **Goal:** `MeshLoader` that loads OBJ via `tiny_obj_loader`. Provide an **interface** `IMeshLoader` so others can stub.
    
- **Deliverables:** `MeshLoader.hpp/cpp`, sample OBJ in `assets/models/`, unit test to load it.
    
- **Acceptance:** Loader returns a `Mesh` resource that can be drawn or validated via vertex counts.
    
- **Deps / stubs:** can use stubbed `Mesh` for other work.
    
- **Related classes / seq:** `MeshLoader`, `AssetManager`. (scene loading)
    

### T2.4 - Shader manager (file-based) & simple shader examples

- **Goal:** `ShaderManager` loads vertex/fragment sources and compiles via `Shader` wrapper. Include `main.vert`/`main.frag`.
    
- **Deliverables:** `ShaderManager.hpp/cpp`, sample shaders.
    
- **Acceptance:** Shader compiles or logs meaningful errors in mock environment; `ShaderManager.get("basic")` returns program object.
    
- **Deps / stubs:** GLContext
    
- **Related classes / seq:** `ShaderManager`, `Shader`. (scene loading & render)
    

### T2.5 - Simple ForwardRenderer & demo scene

- **Goal:** `RenderSystem` that draws a single `MeshRenderer` using the `Material`.
    
- **Deliverables:** `RenderSystem.hpp/cpp`, `examples/hello_scene` using a stubbed `World` and a test mesh.
    
- **Acceptance:** On real run, displays textured/tinted cube
    
- **Deps / stubs:** AssetManager can be stubbed to return test assets.
    
- **Related classes / seq:** `RenderSystem`, `MeshRenderer`, `Camera`, `PostProcessingPass`. (game loop)

### T2.6 - SkySphere / Skybox Support  

- **Acceptance:** Scene format supports `sky` config (texture + shader); renderer draws sky sphere/box behind geometry correctly (depth test/state) and it is included in sorting (opaque > sky > transparent).

### T2.7 - Lighting & LitMaterial (Phase 3)  

- **Acceptance:** `Light` component exists (directional/point/spot), `LitMaterial` exists and loads albedo/specular/roughness/AO/emission textures, forward renderer collects lights per-frame and lighting shader(s) combine contributions so that multiple lights affect objects; sample scene demonstrates 2+ lights on one mesh producing expected shading differences.

### T2.8 - Sampler abstraction & material sampler config  
**Acceptance:** `Sampler` or sampler settings are loadable from scene (filtering, wrap), textures/materials respect sampler settings at bind time.

---

## TRACK 3 - ECS & Scene (World, Entity, Component, SceneDeserializer)

**Owner:** --

> This track enables other tracks to plug in assets/systems via interfaces. Make managers return simple objects so render/physics can proceed.

### T3.1 - Basic ECS skeleton (Entity, World, Managers)

- **Goal:** `Entity`, `EntityManager`, `ComponentManager`, `World` minimal implementations supporting create/destroy and assign IDs.
    
- **Deliverables:** `Entity.hpp/cpp`, `EntityManager.hpp/cpp`, `ComponentManager.hpp/cpp`, `World.hpp/cpp`.
    
- **Acceptance:**  create entities, add/remove components (use `Component` stubs), and validate counts.
    
- **Related classes / seq:** `World`, `Entity`, `EntityManager`, `ComponentManager`. (scene loading, physics)
    

### T3.2 - Abstract Component & core components (Transform, MeshRenderer, Camera, Light)

- **Goal:** `Component` base class and implementations for `Transform`, `MeshRenderer`, `Camera`, `Light` with simple data containers and serialization hooks.
    
- **Deliverables:** header/cpp for each component test for serialization/deserialization from JSON.
    
- **Acceptance:** Components can be attached to an `Entity` and serialized/deserialized to/from JSON snippets.
    
- **Deps / stubs:** none (uses JSON lib).
    
- **Related classes / seq:** `Component`, `Transform`, `MeshRenderer`, `Camera`, `Light`. (scene loading & render)
    

### T3.3 - SceneDeserializer + scene format spec

- **Goal:** `SceneDeserializer` that parses external JSON scene, creates entities, attaches components, and requests assets from `IAssetManager`.
    
- **Deliverables:** `SceneDeserializer.hpp/cpp`, `assets/scenes/sample_scene.json`, schema doc.
    
- **Acceptance:** Given sample JSON, `SceneDeserializer` creates entities in `World` and attaches components. parsing materials (including pipeline state: culling, depth test, blending, color/depth masks), linking materials to entities, and ensuring `MaterialManager` creates Material objects accordingly.`AssetManager` (singleton) exists, shares assets (same mesh/texture pointer) across entities.
    
- **Related classes / seq:** `SceneDeserializer`, `ComponentManager`, `AssetManager`. (scene loading)
    

### T3.4 - Coordinator / SystemManager integration stub

- **Goal:** `Coordinator` facade that exposes `getEntityManager()`/`getComponentManager()` and a `SystemManager`.
    
- **Deliverables:** `Coordinator.hpp/cpp`, `SystemManager` minimal implementation.
    
- **Acceptance:** system calls `updateSystems()` calls system `update().
    
- **Related classes / seq:** `Coordinator`, `SystemManager`. (startup & loop)
    

---

## TRACK 4 - Systems & Tools (Physics, Camera control, Scripting, Post-processing, Tests & CI)

**Owner:** --

### T4.1 - CameraControlSystem (input-driven)

- **Goal:** `CameraControlSystem` that reads `Input` and updates `Transform`/`Camera.
    
- **Deliverables:** `CameraControlSystem.hpp/cpp`,tested with`Input`.
    
- **Acceptance:** Input modifies camera transform values.
    
- **Deps / stubs:** Uses `Input` stub if Track1 not yet ready.
    
- **Related classes / seq:** `CameraControlSystem`, `Input`, `Camera`, `Transform`. (game loop)
    

### T4.2 - PhysicsSystem integration (reactphysics3d)

- **Goal:** `PhysicsSystem` API that integrates with reactphysics3d; provide `IPhysicsBackend` interface.
    
- **Deliverables:** `PhysicsSystem.hpp/cpp`, `IPhysicsBackend`.
    
- **Acceptance:** physics step updates transforms; with reactphysics3d available, real physics runs.
    
- **Deps / stubs:** reactphysics3d.
    
- **Related classes / seq:** `PhysicsSystem`, `Transform`, `EntityManager`. (game loop / physics)
    

### T4.3 - ScriptSystem + ScriptHost

- **Goal:** `ScriptSystem` that calls scripting components; `ScriptHost` exposes API to register `MonoBehaviour` classes. Provide simple C++ `MonoBehaviour`to run.
    
- **Deliverables:** `ScriptSystem.hpp/cpp`, `ScriptHost.hpp/cpp`, sample script class in C++.
    
- **Acceptance:** Scripts receive `start()` and `update()` calls during loop when attached.
    
- **Related classes / seq:** `ScriptSystem`, `ScriptingComponent`, `MonoBehaviour`. (game loop / scripting)
    

### T4.4 - PostProcessingPass manager

- **Goal:** `PostProcessingPass` representation and manager; implement a simple fullscreen pass (tone mapping) that reads a texture and writes to screen.
    
- **Deliverables:** `PostProcessingPass.hpp/cpp`, sample shader, example config.
    
- **Acceptance:** RenderSystem can call `apply()` and the pass executes. RenderSystem renders to an FBO, then applies configured post-processing passes (chain) and outputs to screen.
    
- **Deps / stubs:** Use `Framebuffer`/`Texture` from Track2.
    
- **Related classes / seq:** `PostProcessingPass`, `Framebuffer`. (render loop)
    

---

## Mapping to sequence & class diagrams

- **Startup & bootstrap**: Track1 (Application, Window, GLContext, FrameScheduler, Engine init)
    
- **Scene Loading**: Track3 (SceneDeserializer, EntityManager, ComponentManager) + Track2 (AssetManager / MeshLoader)
    
- **Rendering**: Track2 (RenderSystem, Shader, Mesh, Material, Framebuffer)
    
- **Physics**: Track4 (PhysicsSystem, reactphysics3d/mock)
    
- **Camera & Input**: Track1 + Track4 (Input, CameraControlSystem)
    
- **Scripting**: Track4 (ScriptSystem, ScriptHost, MonoBehaviour)
    

---