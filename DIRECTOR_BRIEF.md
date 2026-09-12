# DIRECTOR BRIEF: CHRONICLES OF DOMINION (BRONZE TO STEAM)
**Living Source of Truth — All Agents Must Read Before Execution**

---

## 0. GRAPHICS POLICY – MANDATORY AND PERMANENT

Procedural generated graphical assets are permanently forbidden.
This includes:
* Spawning or combining Engine BasicShapes (Cube, Cylinder, Sphere, Cone, etc.) for any final or semi-final visuals
* Generating textures with noise, math, or pixel arrays in code
* Any runtime or editor-time procedural mesh/texture generation for units, buildings, terrain detail, or environment

From this point forward the only allowed visual assets are:
1. Assets that already exist in the project’s Content folder, or
2. External assets that you (Antigravity) have found and that the Director has explicitly approved

### Required process for any new graphics work:
1. Search for suitable existing free or affordable assets (Fab / Unreal Marketplace, Sketchfab, Kenney, Quaternius, OpenGameArt, etc.) that fit the Mythic Grimdark Bronze Mesopotamian style.
2. Present a clear list to the Director with:
   * Asset name and source link
   * Short description
   * Why it fits
   * License notes
3. **Stop and wait for explicit Director approval.**
4. Only after approval may you provide import instructions or write code that references and places those assets.
5. Never import, download, or integrate any asset without prior approval.
6. Never fall back to procedural primitives or procedural textures under any circumstances.

> [!CAUTION]
> If no suitable assets are found, report that fact and wait for Director guidance.
> Do not invent temporary procedural replacements.
> This policy has no exceptions and overrides any previous approach.

---

## 1. Project Identity & Vision
* **Title**: *Chronicles of Dominion: Bronze to Steam*
* **Core Genre**: AAA Grand Strategy & Real-Time Tactical RTS Civilization Simulation (*AoE2* mass combat + *Manor Lords* physical logistics + *Frostpunk* Hope/Discontent & Moral Edicts + 3-Estate Power Triad).
* **Current Focus**: Locked down, responsive, high-readability Tactical RTS Vertical Slice in Unreal Engine 5.8.

---

## 2. Honest Reality Check: What is Real vs. Scaffolding

| System / Feature | Real Status | What is Actually Working vs. Incomplete |
| :--- | :--- | :--- |
| **Camera Navigation** | **Working** | WASD / Arrow panning, zoom, middle-mouse drag, dynamic pitch tilt. |
| **Unit Selection & Feedback** | **Target Slice 1** | Single-click & drag marquee exist; polishing high-contrast brackets, ground rings, and hover feedback. |
| **Movement & Formations** | **Target Slice 2** | Slot math exists; needs physical slot adherence verification in PIE across `[1-4]`. |
| **Basic Combat Loop** | **Target Slice 3** | Raider wave `[E]` and targeting exist; needs verified engagement, damage, and death flow in PIE. |
| **Recruitment Hub** | **Target Slice 4** | Hotkeys `[Z/X/C/R]` exist; needs clean resource checks and courtyard muster points. |
| **Visual & UI Readability** | **Target Slice 5** | Monolithic kit ingested; needs final lighting calibration and HUD contrast polish. |
| **Physical Ox-Cart Supply** | **Scaffolding Only** | Code files and variables exist; gameplay loop is NOT active yet. |
| **3-Estate Politics & Edicts** | **Scaffolding Only** | Enums and subsystems exist; societal consequences are NOT verified. |
| **Imperial Ledger (`[L]`)** | **UI Mockup Only** | Renders static drawer; live economy integration is deferred. |
| **Epoch Morphing & Geopolitics** | **Scaffolding Only** | Architectural morphing and diplomacy are future roadmap items. |

---

## 3. Current Priority
> **Core Tactical Loop Polish only. No supply, politics, edicts, ledger, or epoch work until the tactical loop is solid.**

---

## 4. Strict Slice-by-Slice Execution Roadmap

1. **Slice 1: Crisp Selection & Feedback (ACTIVE)**
   - Single left-click selects one unit.
   - Drag marquee selects multiple units.
   - High-contrast visual feedback (bright golden ground decal ring, in-world screen brackets `[ ]`, overhead name/HP tag on hover/selection).
   - Selected units stay clearly marked and responsive.

2. **Slice 2: Reliable Movement + Formations**
   - Right-click ground move must be 100% reliable with immediate green waypoint ring feedback.
   - Units physically march into and hold geometric ranks when pressing <kbd>1</kbd> (Phalanx), <kbd>2</kbd> (Wedge), <kbd>3</kbd> (Skirmish), or <kbd>4</kbd> (Line).

3. **Slice 3: Basic Combat Loop**
   - Pressing <kbd>E</kbd> spawns raiders outside the southern gate.
   - Raiders path toward the ziggurat and engage friendly units.
   - Right-click attack orders work reliably; units deal/take damage with floating feedback and die cleanly at 0 HP.

4. **Slice 4: Clean Recruitment**
   - <kbd>Z</kbd> (Spearman), <kbd>X</kbd> (Slinger), <kbd>C</kbd> (Chariot), and <kbd>R</kbd> (5x Cohort) spawn correct archetypes at the courtyard muster area.
   - Resources (Grain, Bronze, Clay) are deducted properly with visual denial feedback on insufficient funds.

5. **Slice 5: Visual Readability Pass**
   - Eliminate remaining lighting glare, prototype polygons, or 45° tilted roofs.
   - High-contrast HUD text and Astrolabe radar blips readable at any camera elevation.

---

## 5. Verification Protocol (Mandatory for Every Claim of "Done")

Every slice completion report MUST provide:
1. **Exact files modified** (with direct file/line links).
2. **Clean compilation log** from `unreal_build_project` (`Result: Succeeded`, 0 errors).
3. **Specific, repeatable PIE test steps** for the developer to personally verify.
4. **Screenshot / log evidence** demonstrating the feature working in-engine.

**Ongoing Rule**: Never start the next slice until the current one has been personally tested and confirmed in PIE. Reject any report lacking the four verification items.
