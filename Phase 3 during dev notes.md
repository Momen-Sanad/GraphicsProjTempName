

current situation, we have:

- `Entity`
    
    - Already working
        
    - Owns transform, hierarchy, maybe render data
        
    - **Does NOT know what a Component is**
        
- Gameplay code (Player, Enemy, etc.)
    
    - Currently **logic objects**, not ECS components
        
    - Drive `Entity` directly
        
- Future plan
    
    - CameraFollowComponent
        
    - Modular behaviors
        
    - ECS-ish direction

---

To add `Component` support to `Entity` **now**, we must:

- refactor Health, Hitbox, Hurtbox
    
- create Systems
    
- change update order
    
- re-test everything

---

When should 'Entity' gain 'Component' support?

Only when **all three are true**:

we have multiple reusable behaviors  
we want data-driven loading (JSON)  
we introduce Systems (RenderSystem, CameraSystem, CombatSystem, etc.,)

---
