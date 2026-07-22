Class List:
---

### `src/engine/core/`

- **Engine**: coordinates subsystems, owns World and Asset managers, orchestrates init/shutdown.
    
- **Application**: platform-facing app bootstrap (creates window, runs main loop).
    
- **FrameScheduler**: manages fixed/update/render tick scheduling and delta/time step.
    

### `src/engine/platform/`

- **Window**: wraps GLFW window creation & lifecycle.
    
- **Input**: gathers input state and exposes queries for systems (keyboard/mouse).
    

### `src/engine/ecs/`

- **World**: high-level container holding the registry, assets, render/lights, systems, scheduling, and global state.
    
- **EntityId**: generation-safe handle used to reference an entity.
    
- **ECS components**: plain data structs such as `Transform`, `Hierarchy`, `Renderable`, `AnimatorData`, and `ColliderData`.
    
- **Registry**: creates/destroys entities, stores typed component pools, validates generations, and runs queries.
    
- **ComponentSignature**: tracks which component types an entity owns for query matching.
    
- **System (abstract)**: base class for systems that operate on entities/components.
    
- **SystemManager**: registers and updates systems and their execution order.
    
- **Coordinator**: optional facade over `Registry` and `SystemManager`.
    

### `src/engine/components/`

- **Transform**: local transform (position/rotation/scale) and parent-relative transform handling.
    
- **MeshRenderer**: component that references a mesh and a material (pipeline state comes from material).
    
- **Camera**: defines projection parameters and camera type (perspective/orthographic).
    
- **Light**: stores light type (directional/point/spot) and light parameters (color, range, cone angles).
    
- **Scripting data**: attaches scripts/behaviours to an entity handle (bridges to scripting system).
    

### `src/engine/assets/`

- **AssetManager**: top-level registry/accessor for loaded assets (delegates to specific loaders).
    
- **ShaderManager**: loads/compiles/link shaders and exposes shader programs.
    
- **MeshLoader**: loads mesh files (OBJ) and creates Mesh resources.
    
- **TextureLoader**: loads texture images and creates Texture resources / handles samplers.
    
- **MaterialManager**: constructs and stores Material instances referenced by entities/scene.
    
- **PipelineState**: small value object representing GL pipeline state (culling, depth, blending, masks).
    
- **Material (abstract)**: holds shader reference and pipeline state; base for concrete material types.
    
- **TintedMaterial**: untextured material with a tint/color.
    
- **TexturedMaterial**: material using textures and samplers.
    
- **LitMaterial**: PBR/lit material  (albedo/specular/roughness/AO/emission textures).
    

### `src/engine/gl/`

- **GLContext**: initializes OpenGL context, loads GL function pointers (glad) and extensions.
    
- **Shader**: wrapper around GPU shader program (bind/unbind, uniform helpers).
    
- **Mesh**: GPU-side mesh wrapper (VBO/IBO/VAO) and draw call abstraction.
    
- **Texture**: GPU texture wrapper (upload, bind, set sampler).
    
- **Framebuffer**: FBO wrapper for render targets / post-processing passes.
    
- **Sampler**: abstraction for texture sampling parameters (filter/wrap).
    

### `src/engine/scene/`

- **Scene**: in-memory representation of a scene (asset lists, entities and their component descriptors).
    
- **SceneDeserializer**: reads scene files (JSON/YAML) and instantiates assets, entities and components into World.
- SceneDeserializer:
	 ├── loads JSON
	 ├── loads assets via AssetManager
	 ├── for each entity:
	 │      create entity in World
	 │      add components
	 │      set transform
	 │      store parent id
	 └── second pass: resolve parent relationships

    

### `src/engine/systems/`

- **RenderSystem / ForwardRenderer**: the forward rendering system: collect cameras, sort opaque/sky/transparent, draw, and integrate post-processing.
    
- **PostProcessingPass**: represents a configured post-processing shader/pass (used by RenderSystem).
    
- **TransformSystem**: computes world transforms from parent-child relations.
    
- **PhysicsSystem**: integrates reactphysics3d, updates physics bodies, and provides collision queries (ray-picking).
    
- **CameraControlSystem**: implements camera navigation controls (responds to Input and updates Camera/Transform).
    
- **ScriptSystem**: updates scripting components and runs MonoBehaviour lifecycle callbacks or, a chosen scripting host.
    

### `src/engine/scripting/`

- **MonoBehaviour**: base class for user scripts/behaviours (lifecycle hooks).
    
- **ScriptHost**: hosts the scripting runtime and binds engine API to scripts (deserialization creates ScriptingComponents that refer to scripts).
    

### `src/engine/utils` 

- **ResourceHandle**: lightweight reference/owner for engine resources (used by AssetManager to share assets).
