---

---
---
## 1. Branching & Project Hygiene

- [ ] Merge the abstraction layer from `main` into `basic`
    
- [ ] Migrate development back to `main` afterward to reduce branch confusion
    
- [ ] Review and clean unused dependencies
    
    -  Remove `assimp` from `vendor` if confirmed unused (confirm with JTA first)
        

---

## 2. Scene & Level Architecture

- [ ] Implement **scene serialization / deserialization**
    
- [ ] Introduce `PrefabLoader.cpp` as a **factory for hard-coded prefabs**
    
    - Prefabs are defined once and reused
        
    - Called from `LevelBuilder.cpp`
        
    - Keeps level creation clean and declarative
        

---

## 3. ECS & System Architecture Decisions

- [ ] Decide the future of the following ECS files:
    
    - `System.hpp`
	        
    - `SystemManager.cpp / .hpp`
		    remove the system manager completely 
    - `Component` **DONE**
	        
- [ ] Decide how to handle:
    
    - `TransformSystem` (given `TransformComponent` already exists)
        
    - `RenderSystem` (replace current forward renderer)
        
- [ ] Design and implement a **frame scheduler**
    
- [ ] Introduce `PipelineState.hpp` (likely required for scheduler + coordinator)
    

---

## 4. Physics Integration

- [ ] Identify required features from **ReactPhysics3D**
    
- [ ] Implement a minimal physics abstraction layer
    
    - Only what the engine actually needs (no over-engineering)
        

---

## 5. Input & Control Layer

- [ ] Review `src/engine/input/Input.hpp`
    
    - Likely remove or replace with a cleaner input abstraction
        

---

## 6. Entity Design (Gameplay-Oriented)

### Player Entity

- [x] Create `Player` class extending `Entity` ✅ 2025-12-29
    
-  Player has:
    
    - Optional `Camera` (default = `nullptr`)
        
    - `PlayerController` component/class
        
        - Handles keyboard input
            
        - Keeps input logic separate from movement & rendering
            
-  Main player owns the main camera
    
    - Camera follows player movement
        

### Enemy Entity

-  Create `Enemy` class extending `Entity`
    
-  Enemy has:
    
    - No `PlayerController`
        
    - `EnemyAI` script instead
        
-  `EnemyAI` scripts attached via `ScriptSystem`
    
-  Enemy variation should only require:
    
    - Different model
        
    - Different texture
        
    - Different AI script
        

---

## 7. Scripting System Redesign

- [ ] Redesign scripting architecture
    
- [ ] Cleanly define responsibilities of:
    
    - `MonoBehaviour.hpp`
        
    - `ScriptingComponent.hpp`
        
    - `ScriptSystem.cpp / .hpp`
        
- [ ] Ensure scripts integrate cleanly with ECS and systems
    

---

## 8. Systems Implementation

Populate and implement the following systems in  
`src/engine/systems/`:

- [ ] `CameraSystem`
    
- [ ] `MovementSystem`
    
    - Handles all entity movement
        
    - Delegates player movement logic to `PlayerController`
        
- [ ] `CombatSystem`
    
- [ ] `HealthSystem`
    
- [ ] `LightSystem`
    
- [ ] `SpawnSystem`
    
- [ ] `RenderSystem`
    
    - Replace current forward renderer
        
    - Collect light components
        
    - Upload light data to shader uniforms
        
    - Render using `LitMaterial` where available (shaders already implemented)
        

---

## 9. Components Implementation

Populate and cleanly implement the following in  
`src/engine/components/`:

- [ ] `CameraFollowComponent`
    
    - Moves camera based on exposed data from `PlayerController`
        
- [ ] `MovementComponent`
    
- [ ] `CombatComponent`
    
    - Likely event-based (observer / notifier pattern)
        
- [ ] `HealthComponent`
    
- [ ] `HitBoxComponent`
    
- [ ] `HurtBoxComponent`
    
- [ ] `LightComponent`
    

---

## 10. Tooling & Debugging (Optional)

- [ ] Introduce a lightweight **logger**
    
    - Compile-time configurable (debug vs release)
        

---

-  Engine is now **lighting-aware, gameplay-oriented, and production-ready**
    
-  Begin actual game development (this is probably not really required if we show the other stuff to Dr. Yahia)



- Composition (fastest): Player wraps an Entity* (use the existing stub in Player.hpp).
- Implement Player core state + API: move, block, attack, dodge flags/timers; update moves the root entity’s transform or velocity.
- Camera follow: in Player::update, update world.get_camera() relative to player; disable or bypass CameraController in main.cpp.
- Add Crusader as a prefab function (or derived class) that sets stats/meshes/materials after CreatePlayer.
- Implement CreatePlayer(World&) -> std::unique_ptr<Player> that builds the hierarchy: root entity (parent), child body mesh, child weapon mesh.
