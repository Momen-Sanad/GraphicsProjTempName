first, do the most basic game loop then extend it according to the time constraint


player class that inherits from entity
player class extend new functionalities
player move
player block
player atk
player dodge

player : Entity
player extends a camera attribute <- camera component gets attached

enemies : Entity


cursader : player
cursader extends functionalities needed

void CreatePlayer(){
	player is a unique pointer
	player is also a parent
	player has children -> player mesh (body)
	player mesh has a child -> weapon (whatever weapon types)
}
### Basic:

	This satisfies project requirements
	
- Gameplay:
	-  Player movement (WASD + shift + space + F) -> apply horizontal force
	-  Camera movement relative to player movement (reads player coords and updates camera coords accordingly)
	-  Enemies spawn (handled automatically) -> apply horizontal force
	-  Collectibles
- Design:
	- load three enemies
	- load player
	- load terrain (walls or something)
	- collectibles
	- design initial level (visual)


if any hit box collides with the player's hurt box
terrain have a hurtbox or none but still has a collider
### extended:

- Gameplay:
	-  Enemies chase player (if the enemy sees the player)
	-  Enemy attacks player if player within range
	-  Player gets hurt if attack hitbox collides with player hurtbox
	-  load procedurally generated levels (walls, traps, enemies, collectibles, etc..)
	-  dodge
	-  block
	-  parry ?
	- 

- Design:
	- Do animations 
	- breakable terrain
	- design boss level and hard fight level and "rest" level
	- 