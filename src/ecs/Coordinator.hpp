/* High-level interface that ties all managers together (used by Engine or Application). 

example usage:

Coordinator gCoordinator;

Entity player = gCoordinator.CreateEntity();
gCoordinator.AddComponent(player, Transform{...});
gCoordinator.AddComponent(player, MeshRenderer{...}); */