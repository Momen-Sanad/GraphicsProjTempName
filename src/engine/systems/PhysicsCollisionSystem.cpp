/*
 * 1) gather active colliders into vector
 * 2) broadphase: naive O(n^2) for small scenes, or sweep-sorted by minX for larger
 * 3) narrowphase: AABB check above
 * 4) on overlap -> dispatch CollisionEvent(entityA, entityB);
 * 5) optionally compute penetration vector and pushback
 */