This collection implements a manager-style ECS (EntityManager, ComponentManager,
SystemManager) with a Coordinator facade. The World class provides a lightweight
name-indexed wrapper for convenience.
Usage summary:
  ecs::World world;
  auto id = world.CreateEntity("Player");
  world.GetCoordinator().RegisterComponent<TransformComponent>();
  world.GetCoordinator().AddComponent<TransformComponent>(id, { ... });
  auto renderSys = world.GetCoordinator().RegisterSystem<RenderSystem>();
  world.GetCoordinator().SetSystemSignature<RenderSystem>( /* bitmask */ );

Notes & choices made:
- We use a 64-bit Signature type; this limits component types to 64 by default
  but keeps the code simple. If you need more, replace Signature with std::bitset<N>.
- ComponentTypeRegistry maps a C++ type to a compact ComponentTypeId (0..N-1).
- The World class in this variant intentionally avoids strong parent/child
  ownership to keep lifecycle clear; if you want hierarchical transforms, add a
  TransformHierarchy component or specialized Parent/Child manager that stores
  entity relationships explicitly.