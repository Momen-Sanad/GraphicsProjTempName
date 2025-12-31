# Engine Boundaries

This project separates a reusable engine layer from game-specific logic. Treat this as the contract for new code and refactors.

## Core rules

- `engine/` is generic and reusable; it must not include headers from `Game/`.
- `Game/` is the only game-specific layer; it registers extra components/systems and builds gameplay entities, prefabs, and levels.
- `engine/core` is the runtime shell. `Application` owns `Window` and drives the loop; `Engine` owns subsystems and exposes `Init/Tick/Shutdown`.
- `engine/ecs` is infrastructure only: IDs, signatures, registries, `Entity` handle, `World/Coordinator`.
- `engine/components` are pure data (no system references or behavior): `Transform`, `Camera`, `MeshRenderer`, `Light`, `Collider`, `Script`.
- `engine/systems` are deterministic processors over ECS data; they may depend on ECS + assets + platform APIs, but never `Game/`.
- `engine/assets`, `engine/gl`, `engine/scene` are distinct: assets cache typed handles, GL wraps GPU objects, scene loads/serializes into `World`.

## Dependency flow

- `Game/` -> `engine/` (core, ecs, components, systems, assets, scene, platform, gl)
- `engine/core` -> `engine/ecs`, `engine/components`, `engine/systems`, `engine/assets`, `engine/scene`, `engine/platform`
- `engine/systems` -> `engine/ecs`, `engine/components`, `engine/assets`, `engine/platform`
- `engine/assets` -> `engine/gl`
- `engine/ecs` -> no rendering or gameplay types

## Runtime flow

`Game/main` -> `Application` -> `Engine::Init` -> `FrameScheduler::Tick`
  -> Input -> FixedUpdate/Physics -> Update -> Render -> `Window::Swap`
