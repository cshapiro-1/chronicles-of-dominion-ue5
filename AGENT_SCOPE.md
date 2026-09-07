# AGENT SCOPE & DIRECTED TASK PROMPTS
**Operational Execution Roadmap for Chronicles of Dominion: Bronze to Steam**

---

## Task Prompt Catalog (Phase 0 → Phase 1)

### Task 1: 100-Man Cohort Physics & Formation Bracing
* **Objective**: Refine the 10x10 mass legion movement so soldiers maintain tight regiment cohesion during rotation, charges, and terrain navigation without overlapping.
* **Acceptance Criteria**:
  1. Selected 100-man cohort maintains 10x10 grid alignment during 180° rotation.
  2. Front rank soldiers lock shields when `[1] Phalanx` is toggled.
  3. Cavalry units (Chariots) charging into the front of a braced Phalanx suffer 70% charge reflection damage and stop momentum.
* **Required Artifacts**: Code diff in `DominionFormationSystem.cpp` & `DominionUnitActor.cpp`, compile log, and in-game playtest notes.

---

### Task 2: Physical Baggage Train & Granary Connection
* **Objective**: Connect field armies to the nearest provincial granary via an active, visual ox-cart baggage line.
* **Acceptance Criteria**:
  1. Ox-cart supply units spawn at the Granary and physically travel along the road network to the 100-man cohort.
  2. Army displays real-time "Supply Days Remaining" in the HUD.
  3. If supply carts are intercepted or destroyed by enemy raiders, the army enters starvation attrition after 30 seconds (-5 HP/sec and morale cascade).
* **Required Artifacts**: Code diff in `DominionSupplyLineSubsystem.cpp`, build log, and verification screenshot/log of starvation trigger.

---

### Task 3: 4-Era Real-Time Architectural Morphing Animation
* **Objective**: Implement a visual transition when pressing `[X]` (Era Advance) that smoothly scales and updates the citadel, walls, and lighting from Primitive Mud to Angkor Wat, Evil Gothic, and Victorian Industrial.
* **Acceptance Criteria**:
  1. Pressing `[X]` transitions through all 4 architectural styles without crashing or hitching.
  2. Spire meshes and flying buttresses dynamically adjust scale and materials.
  3. Brazier lights update intensity and color (Warm Wood Fire $\rightarrow$ Golden Incense $\rightarrow$ Blood Crimson $\rightarrow$ White-Hot Furnace).
* **Required Artifacts**: Code diff in `DominionBuildingActor.cpp`, compile log, and verification checklist.

---

### Task 4: Flanking & Psychological Morale Cascade Engine
* **Objective**: Implement directional flank detection and dynamic morale cascades during mass clashes.
* **Acceptance Criteria**:
  1. Units attacked from behind (>120° rear arc) suffer 2x morale damage.
  2. If a cohort's morale drops below 20%, soldiers break formation, drop shields, and route away from the attacker.
  3. Friendly units witnessing a nearby route lose 15% morale.
* **Required Artifacts**: Code diff in `DominionMassCombatProcessor.cpp` and unit status display.

---

### Task 5: High-Performance MassEntity Batching (2,000+ Units)
* **Objective**: Optimize unit rendering using UE5 MassEntity / Hierarchical Instanced Static Meshes (HISM) for background cohorts.
* **Acceptance Criteria**:
  1. Spawn 20 cohorts (2,000 active soldiers) on the Euphrates test map.
  2. Maintain a solid 60+ FPS at 1080p/1440p during full army engagement.
  3. No visual pop-in or animation desync.
* **Required Artifacts**: `stat fps` and `stat unit` performance profiling log.

---

### Task 6: Geopolitical AI Ruler Personalities & Skirmish AI
* **Objective**: Implement basic autonomous behavior for the Nomadic Warlord and Merchant Prince AI.
* **Acceptance Criteria**:
  1. AI opponent assesses player army strength and chooses between phalanx defense, flanking chariot charges, or granary raids.
  2. AI retreats wounded regiments back to city walls for archer tower protection.
* **Required Artifacts**: Code diff in `DominionGeopoliticalAISubsystem.cpp`.

---

### Task 7: Minimap Radar & Strategic Viewport Overlay
* **Objective**: Complete the bottom-left minimap radar on the AoE2 HUD console.
* **Acceptance Criteria**:
  1. Live radar blips for player units (Blue), enemy raiders (Red), and buildings (Gold).
  2. Left-clicking or dragging on the minimap immediately pans the camera to those world coordinates.
* **Required Artifacts**: Code diff in `DominionRTSHUD.cpp`.

---

### Task 8: Save / Load Game State Subsystem
* **Objective**: Serialize active armies, building eras, granary stockpiles, and tech progress.
* **Acceptance Criteria**:
  1. Pressing `F5` saves game state to JSON / UE binary format.
  2. Pressing `F9` restores exact army positions, health, morale, and active era.
* **Required Artifacts**: Code diff in `DominionEpochManager.cpp` and sample save file.
