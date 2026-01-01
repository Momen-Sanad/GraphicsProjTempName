Goals of project:

    1. Use shaders to program the GPU render pipeline.
    2. Use buffers to store mesh data and use vertex array objects to render meshes.
    3. Change an object's position, rotation and scale via matrices.
    4. Use depth testing, face culling, blending and color/depth masks.
    5. Use textures to draw images.
    6. Use samplers to configure the texture sampling process.
    7. Combine shaders, pipeline states and uniform data into a Material object.
    8. Describe and process a scene in a data-oriented manner.
    9. Implement a forward renderer with support for transparent objects.
    10. Implement sky sphere rendering.
    11. Use framebuffers to apply post-processing effects.
    12. Implement a shading model and support multiple lights of different types.
    13. Use the engine to implement a simple game.

### Phase 1 Delivery  
Week 10 Saturday 22 Nov 2025 23:59  
**Countdown:** `= date("2025-11-22T23:59") - date(today)`

### Phase 2 Delivery  
Week 11 Monday 1 Dec 2025 23:59  
**Countdown:** `= date("2025-12-01T23:59") - date(today)`

### Phase 2 Discussion  
Week 11 Tuesday 2 Dec 2025 13:00  
**Countdown:** `= date("2025-12-02T13:00") - date(today)`

### Phase 3 Delivery  
Week 14 Monday 22 Dec 2025 23:59  
**Countdown:** `= date("2025-12-22T23:59") - date(today)`

### Phase 3 Discussion & Presentation  
Week 14 Tuesday 23 Dec 2025 13:00  
**Countdown:** `= date("2025-12-23T13:00") - date(today)`


Deadline this weekend
Phase 1 tasks:
    Submit a game proposal containing the following:
        - Game name
        - Image demonstrating the game idea
        - Paragraph describing how the game will be played and the goal of the game
        - Team member names and ID's and team number

For the next 5 weeks after phase 1 is finished, do phase 2

Phase 2 tasks:
Build a simple game engine with the following requirements:

    Engine Design
        Design the engine to support any type of 3D game.
        Use an ECS (Entity-Component-System) architecture.
    
    Scene Management
        Load the scene from a data file.
        The scene file should define:
            Paths to assets (shaders, textures, models).
            Entities and their components.
            System configurations.
        Ensure the scene can be changed without recompiling the engine.
    
    Rendering Configuration via Scene File
        Configure shaders, meshes, textures, samplers, and pipeline state (back face culling, depth testing, blending, color/depth masks) for each entity.
        Configure post-processing shaders and their settings.
        Configure sky texture and shader.
    
    Entities and Components
        Allow full scene graph definition (entities with parent-child relationships and relative transformations).
        Implement at least the following components:
            Mesh Renderer: defines which material and mesh to use.
            Camera: defines camera attributes (type, FOV, near/far planes, orthographic height if applicable).
    
    Engine Architecture Implementation
        Implement a World class to hold entities.
        Each entity should contain:
            A list of components.
            Name, transformation, and pointer to parent.
        Implement an abstract Component class with:
            Virtual functions (e.g., deserialization).
            Reference to owning entity.
        Extend Component to create MeshRenderer and Camera components.
    
    Asset Management
        Implement singleton asset loader classes for shaders, textures, meshes, and materials.
        Assets must be loaded from the scene file and shared between components or systems.
        Materials should include pipeline state and other relevant properties (e.g., transparency).
        Implement at least:
            Material (abstract) -> TintedMaterial, TexturedMaterial.
     
    Deserialization
        Implement deserialization functions for:
            Components.
            Assets.
            Scene file.
    
    Rendering System
        Implement a forward renderer system that:
            Loops over entities.
            Finds cameras and drawable entities.
            Sorts entities: opaque -> sky -> transparent (far to near).
            Applies post-processing effects if configured.
    
    Flexibility
        You may modify the architecture, but must satisfy all project requirements.



### Phase 2 Checklist

#### Engine Design
- [ ] Design the engine to support any type of 3D game.
- [x] Use ECS (Entity-Component-System) architecture.

#### World and Entities
- [ ] Implement World class to hold all entities.
- [ ] Implement Entity class with:
  - [ ] List of components
  - [ ] Name
  - [ ] Transformation relative to parent
  - [ ] Pointer to parent

#### Components
- [ ] Implement abstract Component class with:
  - [ ] Virtual deserialization function
  - [ ] Reference to owning entity
- [ ] Implement MeshRenderer component:
  - [ ] Define which mesh and material to use
- [ ] Implement Camera component:
  - [ ] Define camera type (perspective/orthographic)
  - [ ] FOV, near/far planes, orthographic height if applicable

#### Asset Management
- [ ] Implement singleton asset loader classes for:
  - [ ] Shaders
  - [ ] Textures
  - [ ] Meshes
  - [ ] Materials
- [ ] Load assets from the scene file and allow sharing between components/systems

#### Materials
- [ ] Implement abstract Material class with:
  - [ ] Pipeline state (back face culling, depth testing, blending, color/depth mask)
  - [ ] Material properties (e.g., transparency)
- [ ] Implement TintedMaterial class for untextured objects
- [ ] Implement TexturedMaterial class for textured objects

#### Scene Deserialization
- [ ] Parse scene file to get:
  - [ ] Asset paths
  - [ ] Entities and components
  - [ ] System configurations
- [ ] Support full scene graph (parent-child transformations)
- [ ] Implement deserialization functions for:
  - [ ] Components
  - [ ] Assets
  - [ ] Entire scene
- [ ] Ensure scenes can be changed without recompiling the engine

#### Rendering System
- [ ] Implement forward renderer system:
  - [ ] Loop over all entities
  - [ ] Identify cameras and drawable entities
  - [ ] Sort entities: opaque → sky → transparent (far to near)
  - [ ] Apply post-processing effects if configured
- [ ] Render entities according to pipeline state and materials
- [ ] Configure shaders, meshes, textures, samplers, and post-processing via scene file

#### Camera & Navigation
- [ ] Implement camera controls to navigate the scene

#### Flexibility & Architecture
- [ ] Ensure engine design is flexible and not limited to a specific type of game
- [ ] Architecture can be modified as long as project requirements are met



phase 3 given 4 weeks to finish:

### Phase 3 – Full Checklist

#### Lighting System
- [ ] Implement a Light component:
  - [ ] Store light data (color, type, cone angles, etc.)
  - [ ] Position and direction derived from entity transformation
- [ ] Update shaders to support lighting
- [ ] Implement LitMaterial class:
  - [ ] Include texture maps: albedo, specular, roughness, ambient occlusion, emission
- [ ] Update forward renderer to support multiple lights affecting the same object

#### Game Implementation
- [ ] Use 3D models (created or downloaded)
- [ ] Apply lighting with multiple lights in the scene
- [ ] Apply texture mapping using LitMaterial textures
- [ ] Add a sky to the scene
- [ ] Implement at least one post-processing effect
- [ ] Implement 3D motion for player or objects
- [ ] Implement collision detection (obstacles, ray-picking, etc.)
- [ ] Implement game logic (user interaction with goals or objectives)
- [ ] Use scene deserialization for all scenes (no hard-coded scene data)
- [ ] Optional: integrate external libraries for non-rendering features (e.g., physics engine, audio)

#### Delivery Requirements
Submit a PDF report containing:
- [ ] Team member names and IDs
- [ ] Contribution of each member
- [ ] Three screenshots of the game
