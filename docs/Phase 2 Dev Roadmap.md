## **Milestone 0 - Repo + Engine Skeleton

Goal: project compiles and runs an empty window.

### Files

- `src/include/engine.hpp`
    
- `src/app/main.cpp`
    
- `src/utils/Logger.hpp/.cpp`
    

### Features

- Engine::Init(), Engine::Run(), Engine::Shutdown()
    
- Logger with Info/Warn/Error
    
- Window creation stub (GLFW)
    
- Game loop stub
    

---

## **Milestone 1 - ECS Core**

Goal: Entities, components, and systems fully functional.

### Files

- EntityId.hpp
    
- Registry.hpp /.cpp
    
- EcsComponents.hpp
    
- System.hpp
    
- SystemManager.hpp /.cpp
    
- World.hpp
    

### Requirements

- Entity = index + generation
    
- Free list reuse
    
- Component type IDs
    
- Sparse/packed component storage
    
- Query API (simple, not archetype-based)
    
- System registration + update order
    
- Basic unit tests in `tests/ecs_tests.cpp`

---

## **Milestone 2 - Transform System**

Goal: world matrices update correctly.

### Files

- Transform.hpp/.cpp
    
- TransformSystem.hpp/.cpp
    

### Features

- Local -> World propagation
    
- Parent/child relations
    
- Dirty flag optimization
    
- (Optional) world AABB compute
    

---

## **Milestone 3 - OpenGL Context + Minimal Rendering**

Goal: draw a cube.

### Files

- GLContext.hpp/.cpp (GLFW)
    
- Shader.hpp/.cpp
    
- Mesh.hpp/.cpp
    
- MeshLoader.hpp/.cpp (tiny cube or OBJ)
    
- RenderSystem.hpp/.cpp
    

### Minimal RenderSystem

- Gather entities with Transform + MeshRenderer
    
- For each:
    
    - Bind shader
        
    - Set MVP
        
    - Mesh::Draw()
        

### Demo

- Create camera entity
    
- Load cube mesh
    
- Run engine and render it
    
---

## **Milestone 4 - Scripting (MonoBehaviour)**

Goal: user scripts can manipulate entities.

### Files

- MonoBehaviour.hpp (header only)
    
- ScriptComponent.hpp
    
- ScriptSystem.hpp/.cpp
    

### Features

- Awake()
    
- Start()
    
- Update(dt)
    
- OnDestroy()
    

Scripts stored in dense arrays, updated sequentially.

---

## **Milestone 5 - AssetManager + Materials + Textures**

Goal: basic materials, textures, reuse of assets.

### Files

- AssetManager.hpp/.cpp
    
- TextureLoader.hpp/.cpp
    
- MeshRenderer.hpp
    

### Features

- Load shader once
    
- Load mesh once
    
- Load texture once
    
- Material with shader + uniforms
    

RenderSystem is improved to:

- bind material
    
- then draw

---

## **Milestone 6 - Input, Camera, and Basic Scene Loading**

Goal: engine is usable for demos.

### Files

- Input.hpp (GLFW wrapper)
    
- Camera.hpp/.cpp
    
- SceneLoader (JSON)
    

### Features

- WASD movement
    
- Simple camera system
    
- Load basic scenes from JSON
    

---

## **Milestone 7 - Optional Physics**

Goal: simple movement + collision.

### Components

- Rigidbody
    
- Collider
    

### Systems

- PhysicsSystem (Euler integration + AABB)
    

This is optional - add only if needed. (probably not)

---


## **Milestone 9 - Tests & Polish**

Goal: stability.

---

# Recommended Libraries

	GLFW
	OpenAL
	glad
	glm
	json
	reactphysics3d
	stb
	tiny_obj_loader
	spdlog
	fmt
