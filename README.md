# CHRONICLES OF DOMINION: BRONZE TO STEAM
### AAA Grand Strategy & Real-Time Tactical RTS Simulation in Unreal Engine 5.8 C++

*Combines the mass tactical combat of Total War & Age of Empires II, the physical logistics and organic city-building of Manor Lords, the deep demographic estates of Victoria 3, and the dark fantasy-realism scale of Elden Ring & Game of Thrones.*

---

## ⚔️ Core Pillars & Strategic Mechanics

### 1. Macro-Pillar Economy & Multi-Tab Imperial Ledger (`[L]`)
- **Macro Economic Pillars (Top Header Bar)**:
  - **🌾 Sustenance**: Aggregates Raw Grain, Flour, Bread, Preserved Meats, and Food Buffers.
  - **⚒️ Industry**: Aggregates Timber, Quarried Stone, and Kiln-Baked Mudbricks.
  - **⚔️ Metallurgy**: Aggregates Copper/Tin Ore, Smelted Bronze Ingots, and Hoplite Armaments.
  - **🪙 Imperial Treasury**: Tracks Royal Coinage, Trade Tariffs, and Foreign Tribute.
  - **🐂 Logistics (`AST-05`)**: Real-time Ox-Cart Freight Network and Supply Security %.
- **Multi-Tab Imperial Management Ledger (`[L]`)**:
  - **Tab: Production Flow**: Visual input $\rightarrow$ processing $\rightarrow$ output supply pipelines.
  - **Tab: Necessities & Living**: Civilian survival pillars:
    - 💧 **Fresh Water & Canals**: Euphrates canal aqueduct network & alluvial silt fertilization.
    - 🏠 **Shelter & Housing**: Mudbrick tenement capacity and unhoused serf backlog.
    - 🪣 **Sanitation & Drainage**: City drainage ditches & corpse plague/miasma risk.
    - 🪵 **Winter Fuel & Firewood**: Hearth timber stockpile buffers.
    - ⚕️ **Public Health & Midwifery**: Infant mortality per 1,000 live births & apothecary tier.
  - **Tab: Demographics**: 4 Social Strata (Nobles, Artisans, Serfs, Captives) & 4 Age Cohorts.
  - **Tab: Trade & Logistics**: Ox-cart freight routes, Euphrates river barges, and granary buffers.

### 2. Soul of the Estate (3 Antique Barometer Dials)
- **📿 Altar (High Priesthood / First Estate)**: Manages religious fervor, temple tithes, and doomsday cults.
- **👑 Throne (Warlord Nobility / Second Estate)**: Manages military officer loyalty, fief conscription, and baronial coups.
- **🌾 Masses (Common Plebeians & Serfs / Third Estate)**: Manages labor strikes, bread riots, and unrest.
- **Central Pressure Needle**: Real-time *Hope* vs. *Discontent* gauge.

### 3. Mass Cohorts & Tactical Physics Formations (`DominionFormationSystem`)
- **100-Man Marching Cohorts**: Coordinated ranks with rotation-locked heading vectors and dynamic velocity modulation.
- **Tactical Formations (`[1-4]`)**:
  - **Line (`[1]`)**: Maximizes weapon frontage and flanking width.
  - **Square (`[2]`)**: 4-sided hollow box perimeter with 360° defensive coverage against encirclement.
  - **Phalanx (`[3]`)**: Rigid spear wall with $+40\%$ armor and 3x bracing bonus against cavalry shock charges.
  - **Skirmish (`[4]`)**: Staggered dispersion minimizing vulnerability to archers and siege artillery.

### 4. Physical Ox-Cart Logistics & Baggage Supply Lines (`DominionSupplyLineSubsystem`)
- **Physical Ox-Cart Baggage Trains (`AST-05`)**: Two-oxen freight wagons transport physical grain rations to field legions.
- **Distance & Spline Friction**: Supply routes calculate terrain friction, canal barge shortcuts, and traffic bottlenecks.
- **Starvation & Mutiny Cascades**: If a baggage line is severed or raided, legions suffer 1.5% health loss/sec, morale breakdown, and mutinous routing.

### 5. 4 Asymmetric Dark-Fantasy Starting Civilizations (`DominionTypes.h`)
- **🔥 The Ashen Theocracy of Ashur-Kish (`AshenHegemony`)**:
  - *Motto*: "Through Ash We Cleanse, In Blood We Endure"
  - *Strength*: $+50\%$ Tithe Income • Unbreakable Fanatical Morale (Immune to routing panic).
  - *Weakness*: $+25\%$ Discontent decay during peacetime • Zero Foreign Trade Diplomacy.
  - *Unique Unit*: **Blood-Sworn Zealot Hoplite** (Ignited bitumen-tipped spears, berserk death frenzy at $<30\%$ HP).
  - *Unique Edict*: **Edict of the Eternal Pyre** (Sacrifice grain for $+30\%$ combat attack).
- **⛓️ The Iron Slaver League of Karn-Drak (`IronDynasts`)**:
  - *Motto*: "Chains Forge the Realm, Iron Commands the Flesh"
  - *Strength*: $+40\%$ Strip-Mining Yield • Double construction speed with captive slave labor.
  - *Weakness*: Constant slave rebellion / mutiny risk • Priesthood loyalty permanently capped at $45\%$.
  - *Unique Unit*: **Dread-Juggernaut Chariot** (Spiked iron scythe wheels that trample infantry lines).
  - *Unique Edict*: **Blood Arena Games** (Sacrifice captive pops in gladiatorial circus for instant Hope surge).
- **💀 The Black-Salt Horde of the Dune Wastes (`MiasmaNomads`)**:
  - *Motto*: "The Wind Drinks the Dead, The Waste Swallows the Weak"
  - *Strength*: $+30\%$ Steppe Cavalry Speed • Baggage trains plunder $50\%$ of enemy granary stocks on contact.
  - *Weakness*: Cannot construct stone Ziggurats • Fragile wooden fortifications • $-40\%$ Naval strength.
  - *Unique Unit*: **Vulture Hornbow Outrider** (Horse archers firing poisoned black-glass arrows).
  - *Unique Edict*: **Salt the Earth & Poison Aquifers** (Poisons target province soil fertility for 3 years).
- **🌊 The River-Lich Syndicate of Marad-Nun (`SunkenSyndicate`)**:
  - *Motto*: "Gold Flows Like Silt, Death Holds No Debt"
  - *Strength*: $+60\%$ River Barge Freight Speed • $+50\%$ Gold Tariff Wealth • $-30\%$ Mercenary Upkeep.
  - *Weakness*: High bureaucratic corruption • Domestic levies suffer $-20\%$ armor.
  - *Unique Unit*: **Dread Galleass War-Barge** (Heavy rowing barge armed with Greek-fire pitch catapults).
  - *Unique Edict*: **Embalmed Debt Servitude** (Enslaves defaulted debtor pops into perpetual canal labor).

---

## 🏛️ Master 3D Asset & UI Catalog

| Asset ID | Category | Asset Name & Description |
| :--- | :--- | :--- |
| **`AST-01`** | **Architecture** | **Monumental Stepped Ziggurat Citadel**: 4 mudbrick terraces, grand staircase, verdigris copper temple roof, and 4 corner fire altar braziers. |
| **`AST-02`** | **Architecture / Props** | **Bronze Smelting Forge & Beehive Kilns**: Open-air smithy with molten bronze hearths, soot chimney flues, anvils, and casting molds. |
| **`AST-03`** | **Characters / Units** | **Mesopotamian Bronze Phalanx Hoplite**: Hammered bronze muscle cuirass, crested Corinthian helmet, lion Aspis shield, and $3.5\text{m}$ spear. |
| **`AST-04`** | **Vehicles / Units** | **Heavy War Chariot & Draught Steeds**: Armored battle chariot with spoked wheels, lion hubcaps, embossed bronze hull, and armored steeds. |
| **`AST-05`** | **Logistics / Props** | **Imperial Baggage Train Ox-Cart**: Two yoked horned oxen, iron-rimmed timber wagon, stacked burlap grain sacks, amphorae, and canvas tarp. |
| **`AST-06`** | **Environment / Props** | **Euphrates River Barge & Timber Docks**: Euphrates armed river barge, timber pier on stilts, stone bollards, cargo crates, and flaming braziers. |
| **`AST-07`** | **Fortifications** | **Fortress Curtain Wall & Bastion Gateway**: Weathered mudbrick ramparts with battlements, bronze-studded double timber gate, and twin bastion towers. |
| **`AST-08`** | **HUD & Interface** | **AoE2 / Total War Engraved PC UI (v8 Masterwork)**: Hewn basalt chassis, ox-cart logistics icon (`AST-05`), 3 Estate glass barometers, illuminated vellum necessities ledger tome `[L]`, and hand-drawn Mesopotamian cartographic map. |

---

## 🎮 In-Game Controls (Play In Editor - Alt + P)

| Input | Action |
| :--- | :--- |
| **W / A / S / D** / **Arrow Keys** | Smooth 8-Way Tactical RTS Camera Panning |
| **Q / E** | Orbital Camera Rotation around Focal Center |
| **Mouse Scroll Up / Down** | Dynamic Altitude Zoom with Exponential Horizon Pitch Tilt |
| **Middle Mouse Drag** | Quick Viewport Panning |
| **Left Click & Drag** | Box Marquee Selection (100-Man Legions & Buildings) |
| **Right Click** | Tactical Move & Attack Orders on Target |
| **1, 2, 3, 4** | Tactical Formations (Line, Square, Phalanx, Skirmish) |
| **F1, F2, F3, F4** | Enact Imperial Edicts (Sacred Tithe, Corvée, Bread Dole, Gladiator Games) |
| **L** | Toggle Multi-Tab Imperial Management Ledger (Production, Living, Demographics, Trade) |
| **T** | Toggle Baggage Train Interdiction (Test Starvation Attrition & Panic Routing) |
| **Z, X, C, V** | Direct Unit Commands (Halt, March, Attack-Move, Stance Brace) |

---

## 🏗️ Technical Architecture & Engine Stack

- **Engine**: Unreal Engine 5.8 (Native C++)
- **Primary Modules**: `DominionCore`
- **World Subsystems**:
  - `UDominionFormationSystem`: Formations, momentum, bracing, directional shield blocking, and panic cascades.
  - `UDominionSupplyLineSubsystem`: Physical ox-cart trains, granary reserves, forage decay, and starvation mutiny.
  - `UDominionLogisticsSubsystem`: Road spline friction, canal barge networks, and provincial warehouse quotas.
  - `UDominionPoliticalEstatesSystem`: 3 Estates (Altar, Throne, Masses), Hope/Discontent gauges, and imperial edicts.
  - `UDominionDemographicsSubsystem`: Age pyramid, infant mortality, housing density, and social strata cohorts.
  - `UDominionGeopoliticalAISubsystem`: 4 AI Ruler archetypes, anti-snowball coalitions, infamy index, and savagery blowback.
  - `UDominionTechTreeSubsystem`: 4-Epoch invention dependency graph (Bronze $\rightarrow$ Iron $\rightarrow$ Feudal $\rightarrow$ Steam).
- **Graphics Pipeline**: Nanite Virtualized Geometry, Lumen Dynamic Global Illumination, $4800\text{K}$ Chiaroscuro Atmospheric Volume with Volumetric Fog.
- **Automated Tooling / MCP**: Remote Control API on port 30010.

---

## 📜 License
MIT License. Developed for *Chronicles of Dominion: Bronze to Steam*.
