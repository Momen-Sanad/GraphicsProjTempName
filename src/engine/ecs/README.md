The engine ECS uses generation-safe `EntityId` handles, a typed `Registry`,
plain data components, and systems that query component sets directly.

`World` is the owning integration point for registry, assets, render, lights,
physics, animation, and scheduling services. Game and demo code should create
entities through `World::createEntity`, `World::createRenderable`, and
`World::createSkinnedRenderable`, then access component data through
`World::registry()`.

Lifecycle is handle-based:

- `World::setParent(child, parent)` stores hierarchy in ECS data.
- `World::destroyEntity(id, DestroyMode::Recursive)` removes an entity and its
  children while invalidating stale handles through generation bumps.
- Render components store asset handles/shared ownership for GPU resources and
  materials; gameplay code does not own raw engine resources.

Behavior belongs in systems. Components should stay plain data unless a small
method is intrinsic to that data type.
