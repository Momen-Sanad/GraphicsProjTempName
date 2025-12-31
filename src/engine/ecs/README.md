This ECS is data-only and built around a lightweight World/Entity handle model.

Usage summary:
  World world;
  Entity* entity = world.createEntity();
  auto& transform = world.addComponent<Transform>(entity->id());
  transform.position = {0.0f, 1.0f, 0.0f};

Notes & choices made:
- Components are plain data; no polymorphic base class or lifecycle hooks.
- ComponentRegistry owns per-type storage and optional metadata.
- Entity is a thin handle {EntityId, World*} with convenience Add/Get/Remove helpers.
- Hierarchy is stored via Transform.parent; World can enumerate roots/children by scan.
