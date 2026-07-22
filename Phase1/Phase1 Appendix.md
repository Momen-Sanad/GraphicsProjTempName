## 2.5D Roguelike Dungeon Crawler with Meta-Currency

---

### **Theme**

- **Time Era**: Medieval
    
- **Weapons**:
    
    - **Ranged (Two-Handed)**:
        
        - Bow
            
        - Throwing Knives (Maybe as a side weapon)
            
        - Crossbow
            
        - Slingshot
            
    - **Melee**:
        
        - **Medium Melee (One-Handed)**:
            
            - Sword (Shorter range)
                
            - Spear (Longer range, slower attack speed)
                
            - Shield (Parry + Block)
                
        - **Short Melee (Two-Handed, Must Have)**:
            
            - Daggers
                
            - Double Axe
                
        - **Heavy Melee**:
            
            - Mace
                
            - Chained Mace
                
            - Two-Handed Axe
                
            - Broadsword
                
            - Two-Handed Spear
                
            - Great Shield + Spiked (Block + Attack, No Parry, Lowers Damage Taken)
                
- **Mobs**:
    
    - **Mini-Bosses**:
        
        - Dragon
            
        - Great Mage
            
            - Arch Wizard
                
            - Fancy Skeleton Wizard
                
        - Giant Skeleton (Summons Light Skeletons)
            
    - **Brutes (High HP, Shielded)**:
        
        - Armored Crusader with Shield and One-Handed Mace
            
        - Armored Crusader with Shield and One-Handed Sword
            
    - **Heavy Enemies (High HP, High Damage, Two-Handed Weapons)**:
        
        - Crusader with Two-Handed Spear
            
        - Crusader with Broadsword
            
        - Crusader with Two-Handed Axe
            
    - **Light Enemies (Low HP, Low Damage)**:
        
        - Hounds
            
        - Skele Hounds (Summoned)
            
        - Light Crusaders (Daggers, Sword, Axe, Mace)
            
    - **Ranged Enemies**:
        
        - Skele Archer (Summoned)
            
        - Skele Mage (Only Summoned by Arch Mage)
            
        - Human Mage
            
        - Human Archer
            
        - Turrets
            
    - **Traps**:
        
        - Poison Trap
            
        - Fire Trap
            
        - Freeze Trap
            
        - Root Trap
            
        - Damage Traps
            

---

### **Goal**

- Traverse as much as you can.
    

---

### **Controls**:

- **Move**: Arrow Keys
    
- **Attack**: Light Attack (Z), Heavy Attack (X)
    
- **Evade**: Dash (Space)
    
- **Block**: Block (D), Optional Timed Parry
    
- **Throwables**: Press G to throw a dagger or other throwable item
    
- **Magic**: Optional (Details TBD)
    
- **Interact**: Press E to interact
    

---

### **Progression**:

- Each 2 cleared levels unlocks 1 boon.
    
- Three doors per room:
    
    - **Mini-Boss Room**: Unavoidable (No options for Blacksmith or Healing)
        
    - **Options per Door**:
        
        - Level (Mobs)
            
        - Blacksmith + Traveling Merchant
            
        - Heal (Medic)
            
- Design:
    
    - The rogue character seeks revenge on Crusader leader **Reynald de Châtillon**.
        
    - Travel to his castle (100 levels).
        
    - Each level has three stairs:
        
        - Mystery -> Could be a mini-boss, fight, merchant, or heal.
            
        - Fight -> Could be a regular fight or mini-boss.
            
        - A random combination of fight, mini-boss, merchant, or heal.
            
    - Every tenth level (level % 10 == 0) is always a mini-boss.
        

---

### **More Lore**

#### **The Ashen Cross**

- The **Ashen Cross** is an unofficial, unholy, irrationally deadly knight:
    
    - Fights **alone**.
        
    - Breaks the standard knightly code.
        
    - Uses **ambushes, night raids, trap setting, and psychological warfare**.
        
    - Wears a slightly altered version of classic Crusader armor:
        
        - Blackened or ashen mail
            
        - Black surcoat (stained with dirt/blood)
            
        - **Red cross symbol stylized like a bleeding slash**
            
- **Enemies say**: “Comes with the moon, kills without warning, and vanishes like smoke.”
    
- **Allies say**: “Is the sword of God no army can see.”
    

#### **Symbolic Marks**:

- Leaves a **blood-red cross**, painted or carved, on:
    
    - Shields
        
    - Walls
        
    - Dead enemies
        
- This marks him as a **mythological terror**.
    

#### **How the Crusader Kingdom Views Him**:

- **The Nobles**: Call him “dishonorable,” “heretical,” and “a danger.”
    
- **The People**: Call him “a guardian angel,” “God’s shadow,” and “the unseen knight.”
    
- **The Enemy**: Believe he is a demon sent by the Franks.
    

---

### **Motivation**:

- **Reynald** murdered the Ghost’s family and village during one of his brutal raids.
    
- However, Reynald’s cruelty wasn’t just human viciousness—it was **already influenced by corruption**.
    
- The Ghost’s objective: **“End Reynald… whatever he has become.”**
    

---

### **Reynald's Corruption Lore - Why He's Sub-Human Now**:

- Reynald’s brutality attracted a dark force—a demonic or forgotten crusader relic—that:
    
    - Twisted his flesh.
        
    - Enhanced his strength.
        
    - Gave him necromantic abilities.
        
    - Whispered to him through ancient, “holy” relics turned evil.
        
    - Allowed him to feed on souls (of soldiers, prisoners, and past comrades).
        
- He’s no longer human; he is **Reynald the Unbound**—a corrupted Crusader warlord.
    

---

### **The Map**: **The Fortress of 100 Levels - “Châtillon's Descent”**

- Reynald’s castle becomes a **vertical cathedral-fortress**, each floor more corrupted than the last.
    

#### **Floor Progression**:

1–10: Abandoned barracks, undead Crusaders  
11–20: War beasts, haunted armories  
21–40: Traps, assassin ghosts, twisted Templars  
41–60: Alchemical horrors, plague chambers  
61–80: Necromancy halls, bone forges, soul prisons  
81–99: Dimensional corruption, spectral knights, eldritch light  
100: **Throne of the Red Abyss - Reynald’s Chamber**

---

### **Boss Design** - **Throne of the Red Abyss**:

1. **Summon Bone Mages**:
    
    - Hooded skeletal clerics that:
        
        - Cast fireballs.
            
        - Heal Reynald slowly.
            
        - **Priority Mechanic**: Destroy Bone Mages before they finish charging a shield rune circle to prevent healing.
            
2. **Swarm of Spiders**:
    
    - Corrupted desert spiders that:
        
        - Poison the Ghost.
            
        - Burst on death.
            
        - Spawn in waves, telegraphed by floor patches that become spawn zones. Player can light them on fire to deny spawns.
            
3. **Absorb Reinforcements**:
    
    - When living Crusader knights or soldiers appear:
        
        - Reynald grabs them.
            
        - Their bodies dissolve into red mist.
            
        - He inhales the mist, **massively recovering health**.
            
        - **Interrupt Rule**: Interrupt Reynald’s absorption channel (2-3s) with skills like Purify, heavy stun, or environmental traps (e.g., dropping a chandelier).
            
4. **Berserk Phase**:
    
    - At 30% HP:
        
        - Reynald’s armor cracks.
            
        - His jaw distends.
            
        - Eyes glow white.
            
        - His sword ignites in corrupted “holy fire”.
            
    - He becomes:
        
        - Faster.
            
        - Insane.
            
        - Screaming twisted biblical lines.
            

---

### **Player Boons**:

1. **Sanctified Execution**: Instantly kills summoned units if struck from behind or while stunned.
    
2. **Purifying Flames**: Destroys Reynald’s healing mist, preventing recoveries.
    
3. **Ashen Step**: Teleport-dash:
    
    - Leaves ash trails.
        
    - Silent kills.
        
    - Immune to projectiles for 0.5 seconds.
        
4. **Cross of Silence Finisher**: Final blow animation:
    
    - Drives a burning cross-shaped dagger into Reynald’s heart.
        
    - Releases all absorbed souls.
        
    - The fortress begins collapsing.
        
    - The player remains, kneeling in the ruins, absorbing some corruption.
        

---

### **UX**:

- **Audio**: Give Reynald an unsettling voice and build-up when absorbing souls.
    
- **Visuals**: Show red mist swirling into Reynald; add a progress bar for the absorption channel.
    
- **HUD**: Display the number of summons and a “heal-threat” meter for Reynald.
    
- **Accessibility**: Remappable keys, colorblind-friendly cues, and toggles for camera shake.
    

---

### **Logo**:

#### **Core Icon** (Main Symbol):

- A **crusader helmet** representing “cursed ghost knight”:
    
    - **Hollow, glowing white eye sockets** (no longer human).
        
    - **Helmet dissolving into spectral smoke** (ghostly, cursed, ethereal).
        
    - **A jagged, blood-red cross carved on the forehead** (symbol of his broken holy mission).
        
    - **Fine cracks running down the faceplate** (represents corruption and undead condition).
        
    - **A faint shadow of the knight’s armor** (fades into ash).
        

#### **Shape Language**:

- Ghostly smoke trails from the helmet edges.
    
- Small ash particles drifting downward.
    
- Cross glows: _faint red -> darker at the edges_.
    
- **Asymmetric cross**, as if painted in anger.
    

#### **Typography**:

- Gothic Serif.
    
- Slight erosion on edges.
    
- Tall, slightly narrow letters.
    
- Distinct letter features:
    
    - **R** in “CURSED” has a sharp downward hook.
        
    - **D** in “CURSED” is cracked.
        
    - **C** in “CURSADE” slightly split.
        

#### **Color Palette**:

- **Ash grey (#B0B0B0)** - Ghostly armor.
    
- **Blood red (#7A0A0A or #8B0000)** - Cursed cross.
    
- **Bone white (#F7F7F7)** - Ghostly glow in the eyes.
    
- **Black / Charcoal (#0A0A0A)** - Background.
    

#### **Effect Use**:

- Light inner glow on the cross.
    
- Subtle red reflection under the letters.
    
- Faint fog/mist behind the helmet.
    

#### **Compositional Layout**:

- **Text Overlay on Icon**: For dramatic effect.
    
    ```
    [Helmet ghost fade behind]
    CURSED
    CURSADE
    ```
    
    The icon is large, and the text is centered on top with a glowing outline.
    

---

### **Visual Motifs**:

1. **Spectral Cracks**: Glowing cracks on the helmet, representing the curse.
    
2. **Red Ash Drift**: Particles drifting upward, symbolizing the curse consuming him.
    

---

### **Warrior Description - The Ashen Cross** (Rogue Crusader)

- **Overview**: Once a sworn knight, now a broken, vengeful warrior seeking to end Reynald for his brutality.
    
- **Appearance & Iconography**:
    
    - Wears battered, blackened plate and chain with a jagged red sigil marking his chest and forehead.
        
    - Scars on his face; one eye narrowed, the other ringed with ash.
        
    - Moves with a soldier’s economy, leaving traces of ash and dried blood.
        
- **Combat Style & Tactics**: Pragmatic and ruthless; blends disciplined strikes with guerrilla techniques like quick attacks, surgical parries, and sudden ambushes.
    
- **Personality & Moral Code**: Cynical, terse, and single-minded; his moral compass is brittle, and he fights for personal vengeance rather than glory or faith.
    
- **Role in the Story & Arc**: Acts as both protagonist and mirror, embodying the danger of vengeance, with his arc reflecting the costs of pursuing revenge. His actions shape NPC reactions and fuel the game's moral tension.