/* resolve hits -> DamageEvent

When a Hitbox intersects a Hurtbox, create a DamageEvent with damage amount, source entity.

CombatSystem listens to those events -> applies status effect (e.g., stun), pushes force, sets invulnerability.

CombatSystem: When doAttack true -> spawn a transient Hitbox child on weapon with lifetime 0.2s; set damage and owner id.

Blocking: defensively reduce incoming damage in HealthSystem while isBlocking and facing attacker (dot product check).

Hit detection: CollisionSystem finds Hitbox vs Hurtbox overlap and issues DamageEvent with source and damage.
*/

//pseudocode for an attack
/*
// spawn attack hitbox as child of weapon transform

auto attack = world.createEntity();
attack->addComponent<TransformComponent>(); // position in front of weapon
auto &hb = attack->addComponent<HitboxComponent>();
hb.halfExtents = {0.3f,0.7f,0.6f};
hb.layer = CollisionLayer::Player;
hb.mask = static_cast<uint32_t>(CollisionLayer::Enemy);
// set owner, damage in CombatComponent
attack->addComponent<TransientComponent>().life = 0.18f;

*/