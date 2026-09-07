# DIRECTOR BRIEF: CHRONICLES OF DOMINION (BRONZE TO STEAM)
**Living Source of Truth — All Agents Must Read Before Execution**

---

## 1. Project Identity & Vision
* **Title**: *Chronicles of Dominion: Bronze to Steam*
* **Core Genre**: AAA Grand Strategy & Real-Time Tactical Civilization Simulation (*Age of Empires II* combat + *Manor Lords* physical logistics/organic city building + *Victoria 3* demographics and geopolitical estate leverage).
* **Scale**: Colossal fantasy-realism (*Elden Ring / Dark Souls / Game of Thrones* magnitude: 80m–100m tall fortifications, 100-man mass legions, panoramic 160,000 unit orbital camera).
* **Architectural Evolution (4 Eras)**:
  1. Primitive Earth & Mud / Megalithic
  2. Angkor Wat Hewn Sandstone (Lotus-bud Prang Spires)
  3. Dark Evil Gothic (Monolithic Obsidian, Needle Spires, Spiky Buttresses)
  4. Victorian Industrial Grimdark (Smokestacks, Cast-Iron Plating, Steam Valves)

---

## 2. Core Director Principles & Rules of Engagement

1. **Short, Verifiable Vertical Slices**:
   - Never attempt monolithic system overhauls. Build focused, functional vertical slices with clear acceptance criteria.
   - Example: *"Implement physical ox-cart baggage supply lines on one test map with visual debug and starvation attrition, then verify with automated playtest."*

2. **Required Artifacts on Every Task**:
   - Clean C++ code diffs complying with Unreal Engine 5.8 standards.
   - Build log confirming compilation with zero errors (`Result: Succeeded`).
   - In-game verification report (what works, controls tested, performance FPS).
   - "What Still Fails / Next Steps" itemized punch list.

3. **The Director Plays the Game**:
   - The user's direct gameplay experience is the primary quality gate. Controls must be immediate, snappy, and bug-free (WASD 8-way pan, Q/E rotate, mouse wheel zoom, marquee select, right-click move/attack).

4. **Performance Targets & Non-Negotiables**:
   - **Target FPS**: Stable 60+ FPS with 500–2,000+ active units on screen.
   - **No Game-Breaking Regressions**: Existing working features (camera zoom, unit movement, HUD rendering) must remain operational across all commits.
   - **"Do Not Touch" Core Layer**: Keep core simulation logic modular in UWorldSubsystems (`DominionFormationSystem`, `DominionLogisticsSubsystem`, `DominionSupplyLineSubsystem`, `DominionDemographicsSubsystem`, `DominionGeopoliticalAISubsystem`).

---

## 3. Active Phase & Priority Focus

### Current Phase: **Phase 0 → Phase 1 (Personal Playable Core Loop)**
* **Current Top Priorities**:
  1. **100-Man Cohort Cohesion**: Keep tight 10x10 formation fidelity during dynamic rotation and path navigation.
  2. **Tactical Combat Feedback**: Impact visual traces, spear thrust combat cycles, and shield-wall bracing reflection against charges.
  3. **Physical Supply Line Baggage Train**: Connect marching cohorts to granary buffers with visual ox-carts and active attrition triggers.
  4. **4-Era Visual Morphing**: Polish dynamic material and silhouette transitions for buildings and fortifications.
