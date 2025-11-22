2.5D roguelite dungeon crawler with meta-currency

- theme
	- time era
		- medieval
	- weapons:
		- ranged (double handed)
			- bow
			- throwing knives ( maybe a side weapon )
			- crossbow
			- slingshot
		- melee
			- medium melee (1 handed)
				- sword shorter range
				- spear longer range  slower atk spd
				- shield (parry + block)
			- short melee (two handed (must))
				- daggers
				- double axe
			- heavy melee
				- mace
				- chained mace
				-  two handed axe
				- broadsword
				- two handed spear
				- great shield + spiked (block + attack NO PARRY)(lowers damage taken)
	- mobs
		- mini-boss
			- dragon
			- great mage
				- arch wizard
				- fancy skele wizard
			- giant skele (summons light skele)
		- brute (high hp has a shield)
			- armored cursader with a shield and 1 handed mace
			- armored cursader with a shield and 1 handed sword
		- heavy enemy (high hp high dmg 2 handed-weapon)
			- cursader 2 handed spear
			- cursader broadsword
			- cursader with 2 handed axe
		- light enemy (low dmg low hp)
			- hounds
			- skele hounds (summoned)
			- light cursaders (daggers, sword, axe, mace)
		- ranged enemy
			- skele archer (summoned)
			- skele mage (only summoned by arch mage)
			- human mage
			- human archer
			- turrets 
		- traps
			- poison trap
			- fire trap
			- freeze
			- root trap
			- just damage traps
- goal
	- traverse as much as u can
- controls
	- move
		- arrow keys
	- atk
		- light atk, heavy atk (Z, X)
	- evade
		- dash (space)
	- block
		- just block "D" (optional timed parry)
	- throwables
		- press g to throw dagger or smth
	- magic (optional)
	- interact
		- press E to interact
- progression
	- each 2 cleared levels is 1 boon
	- three doors per room
		- mini-boss room is unavoidable
			- no options for blacksmith or healing
		- each door is either
			- level (mobs)
			- blacksmith + travelling merchant
			- heal (medic)

	- design
		- a rogue guy like ghost of tsushima wishes to murder a cursader leader **Reynald de Châtillon**
		- go to his castle (100 levels)
		- each level has three stairs 
			- mystery -> either mini-boss, fight, merchant, heal 
			- fight -> either fight or mini-boss 
			- a random choice between a fight, mini-boss, merchant, heal
				- so a player could have 2 fights and a mystery, or a fight merchant and mystery, or a fight mystery and heal
		- game_level % 10 == 0 means a mini-boss always (1 flight of stairs)

---

# more lore

The **The Ashen Cross** is an _unofficial_, _unholy_, _irrationally deadly_ knight who:

- Fights **alone**
    
- Breaks the standard knightly code
    
- Uses **ambushes, night raids, trap setting, psychological warfare**
    
- Wears a slightly altered version of the classic Crusader armor:
    
    - Blackened or ashen mail
        
    - Black surcoat (stained with dirt/blood)
        
    - **Red cross symbol stylized like a bleeding slash**
        

Enemies say he:

> “Comes with the moon, kills without warning, and vanishes like smoke.”

Allies say he:

> “Is the sword of God no army can see.”

### **Symbolic marks**

Leaves a **blood-red cross**, painted or carved, on:

- Shields
    
- Walls
    
- Dead enemies
    

This turns him into a _mythological terror_.

### **How the Crusader Kingdom Views Him**

### **The Nobles**

Call him “dishonorable,” “heretical,” “a danger.”

### **The People**

Call him “a guardian angel,” “God's shadow,” “the unseen knight.”

### **The Enemy**

Believe he is a demon sent by the Franks.

---
## Motivation

Reynald murdered the Ghost's family and village during one of his brutal raids -  
BUT, Reynald's cruelty wasn't just human viciousness… It was **already influenced by the corruption**.

Our Ghost has one objective:

> **“End Reynald… whatever he has become.”**

---

# **REYNALD'S CORRUPTION LORE - WHY HE'S SUB-HUMAN NOW**

Reynald's brutality attracted a dark force - a demonic or forgotten crusader relic - that:

- Twisted his flesh
    
- Enhanced his strength
    
- Gave him necromantic abilities
    
- Whispered to him through ancient “holy” relics turned evil
    
- Let him feed on souls (his soldiers, prisoners, even past comrades)
    

Now he's not even a man.  
He's **Reynald the Unbound** - a corrupted Crusader warlord.

---

# The map

**THE FORTRESS OF 100 LEVELS - “Châtillon's Descent”**

Reynald's castle becomes a **vertical cathedral-fortress**, each floor more corrupted than the last.

### Floors progression:

1–10   -> Abandoned barracks, undead Crusaders
11–20 -> War beasts, haunted armories
21–40 -> Traps, assassin ghosts, twisted Templars
41–60 -> Alchemical horrors, plague chambers
61–80 -> Necromancy halls, bone forges, soul prisons
81–99 -> Dimensional corruption, spectral knights, eldritch light
100     -> **Throne of the Red Abyss - Reynald's chamber**

---

# Boss design (Throne of the Red Abyss)

## **1. Summon Bone Mages**

Hooded skeletal clerics that:

- Cast fireballs
    
- Heal Reynald slowly
	-  **Bone Mages**: prioritize them, if left alive they shield/heal Reynald. A priority mechanic: if a Bone Mage reaches a rune circle, it creates a shield; destroy the mage before it finishes.
	- Shield him with holy-looking corrupted symbols

## **2. Swarm of Spiders**

Horrific desert spiders infused with corruption:

- Poison the Ghost
    
- Burst on death

- Spawn in waves - telegraph floor patches that become spawn zones (player can light them on fire to deny spawns).

## **3. Absorb Reinforcements**

When living Crusader knights or soldiers appear:

- Reynald grabs them
    
- Their bodies dissolve into red mist
    
- He inhales it
    
- **Massive health recovery**
    

This makes the fight feel hopeless and terrifying.
**Absorb rule**: Makes it interruptible:
- He channels for 2–3s with visual red mist. Player can interrupt with a Purify-style skill, heavy stun, or environmental trap (e.g., drop chandelier).

## **4. Berserk Phase**

At 30% HP:

- Reynolds' armor cracks
    
- Jaw distends
    
- Eyes glow white
    
- His sword becomes a flame of corrupted “holy fire”
    

He becomes:

- Faster
    
- Insane
    
- Screaming biblical lines twisted into nightmares

---

# Player boons

### **1. Sanctified Execution**

Instantly kills summoned units if struck from behind or while stunned.

### **2. Purifying Flames**

Destroys Reynald’s healing mist, preventing recoveries.

### **3. Ashen Step**

Teleport-dash:

- Leaves ash trails
    
- Silent kills
    
- Immune to projectile for 0.5 sec
    

### **4. “Cross of Silence” Finisher**

Final blow animation:

- Drives a burning cross-shaped dagger into Reynald’s heart
    
- Releases all absorbed souls
    
- Fortress begins collapsing
	- The player is now fulfilled after killing Reynald and stays on his knees inside the fortress, content with his situation, _Corruption Echoes_, Ghost absorbs some corruption (bittersweet, leaving ambiguous legacy.)

---

### UX

- **Audio**: give Reynald an unsettling voice and build-up when human absorb starts.
    
- **Visual**: show red mist swirling into Reynald; add a progress bar for absorb channel.
    
- **HUD**: show number of summons and a “heal-threat” meter for the boss to help players prioritize.
    
- **Accessibility**: remappable keys, colorblind-friendly cues, toggles for camera shake.

---

# Logo

## **1. Core Icon (Main Symbol)**

A **crusader helmet** that visually communicates “cursed ghost knight”:

- **Hollow, glowing white eye sockets**  
    -> shows he’s no longer human
    
- **Helmet top dissolving into spectral smoke**  
    -> ghostly, cursed, ethereal
    
- **A jagged, blood-red cross carved on the forehead**  
    -> the symbol of his broken holy mission
    
- **Fine cracks running down the faceplate**  
    -> represents corruption + his undead condition
    
- **A faint shadow of the knight’s armor below the helmet**  
    -> fades into ash (no full body needed)
    

This icon becomes the **identity of the game**.

---

# **2. Shape Language**

**“Ghost of Tsushima mask + Crusader knight helm + Dark Souls curse aesthetic.”**

- Curved smoke trails from the helmet edges
    
- Small ash particles drifting downward
    
- Cross glow: _faint red -> darker toward edges_
    

The cross slightly **asymmetric**, like painted in anger.

---

# **3. Typography**

### **Font Style**

- Gothic Serif
    
- Slight erosion on edges
    
- Strong medieval identity
    
- Letters tall and slightly narrow
    
- Some letters extended:
    
    - The **R** in CURSED with a sharp downward hook
        
    - The **D** in CURSED cracked
        
    - The **C** in CURSADE slightly split
        
---

# **4. Color Palette**

### **Primary**

- **Ash grey (#B0B0B0)** — ghostly armor
    
- **Blood red (#7A0A0A or #8B0000)** — cursed cross
    
- **Bone white (#F7F7F7)** — ghost glow in eyes
    
- **Black / Charcoal (#0A0A0A)** — background
    

### **Effect Use**

- Light inner glow on the cross
    
- Very subtle red reflection under the letters
    
- Faint fog/mist behind the helmet
    

---

# **5. Compositional Layout**

###**Text Overlay on Icon**

For dramatic effect:

```
[Helmet ghost fade behind]
CURSED
CURSADE
```

Icon is large, text is centered on top with glowing outline.

---

# **6. Visual Motifs (Optional Enhancements)**

### **A. Spectral cracks**

Faint glowing cracks on the helmet -> ties to “cursed.”

### **B. Red Ash Drift**

Particles drifting upward -> symbolizes the curse consuming him.

---