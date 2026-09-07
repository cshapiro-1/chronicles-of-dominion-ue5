# CHRONICLES OF DOMINION: BRONZE TO STEAM
### AAA Grand Strategy & Real-Time Tactical RTS Civilization Simulation in Unreal Engine 5.8 C++

*Combines the tactical mass combat of Age of Empires II, the physical logistics and organic city-building of Manor Lords, the deep demographics/estates of Victoria 3, and the colossal fantasy-realism scale of Elden Ring & Game of Thrones.*

---

## ⚔️ Key Features & Vertical Slices

### 1. Colossal Scale & Mass Legions
- **100-Soldier Tactical Cohorts**: Marching phalanxes with rotation-locked heading vectors and dynamic speed modulation to maintain military ranks.
- **Monumental Fortifications**: 80m–90m tall cyclopean stone curtain walls, corner citadels, and Angkor-Wat-style temple keeps.
- **Macro RTS Camera Rig**: 160,000-unit altitude panoramic orbit with exponential horizon pitch tilt, momentum damping, and 8-way WASD navigation.

### 2. Tactical Formations & Physics Combat (DominionFormationSystem)
- **Phalanx Shield Wall ([1])**: 10x10 rigid grid with 3x bracing bonus against cavalry charges.
- **Wedge Shock Charge ([2])**: Arrowhead apex with wing penetration against infantry blocks.
- **Skirmish Dispersion ([3])**: Staggered hexagonal lattice to minimize catapult and arrow vulnerability.
- **Square Perimeter Defense ([4])**: 4-sided hollow box perimeter providing 360° defensive coverage.

### 3. Physical Ox-Cart Supply Lines & Starvation Attrition (DominionSupplyLineSubsystem)
- **Physical Baggage Trains**: Ox-carts follow fighting cohorts to transport grain rations.
- **Proximity Logistics Engine**: Automatically calculates supply tether distance.
- **Starvation Cascade**: If baggage trains are severed, cohorts suffer 1.5% health loss/sec, morale decay, and mutinous panic routing.

### 4. 4-Era Dynamic Architectural Morphing (DominionBuildingActor)
- **Era I**: Primitive Megalithic Mud & Earthworks
- **Era II**: Angkor Wat Hewn Sandstone (100m Prang Lotus Spires & Corbelled Arches)
- **Era III**: Dark Gothic Monoliths (Obsidian keeps, needle spires, spiky flying buttresses)
- **Era IV**: Victorian Industrial Grimdark (Cast-iron armor plating, brass steam pipes, towering smokestacks)

---

## 🎮 In-Game Controls (Play In Editor - Alt + P)

| Input | Action |
| :--- | :--- |
| **W / A / S / D** / **Arrow Keys** | Smooth 8-Way Tactical RTS Camera Panning |
| **Q / E** | Orbital Camera Rotation around Focal Center |
| **Mouse Scroll Up / Down** | Dynamic Altitude Zoom with Exponential Horizon Tilt |
| **Middle Mouse Drag** / **Screen Edge** | Quick Viewport Navigation |
| **Left Click & Drag** | Box Marquee Select (Select 100-Man Legions / Buildings) |
| **Right Click** | Tactical Move & Attack Orders on Target |
| **1, 2, 3, 4** | Tactical Formations (Phalanx, Wedge, Skirmish, Square) |
| **X** | Cycle 4 Architectural Eras (Bronze $\rightarrow$ Angkor Wat $\rightarrow$ Dark Gothic $\rightarrow$ Victorian Steam) |
| **T** | Toggle Baggage Train Interdiction (Test Starvation Attrition & Panic Routing) |
| **Z, C, V** | Recruit Spearman / Heavy Chariot / Baggage Train Wagons |

---

## 🏗️ Technical Architecture (Native Unreal Engine 5.8 C++)
- **Source Modules**: DominionCore
- **Subsystems**: UDominionFormationSystem, UDominionSupplyLineSubsystem, UDominionLogisticsSubsystem, UDominionDemographicsSubsystem, UDominionGeopoliticalAISubsystem, UDominionTechTreeSubsystem
- **Rendering**: Nanite Virtual Geometry, Lumen Dynamic Global Illumination, Virtual Shadow Maps (VSM)
- **Automation / MCP**: Native Node.js & Python Remote Control API bridge on port 30010

---

## 📜 License
MIT License. Created by Collin Shapiro.
