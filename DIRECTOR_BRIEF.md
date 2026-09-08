# DIRECTOR BRIEF: CHRONICLES OF DOMINION (BRONZE TO STEAM)
**Living Source of Truth — All Agents Must Read Before Execution**

---

## 1. Project Identity & Vision
* **Title**: *Chronicles of Dominion: Bronze to Steam*
* **Core Genre**: AAA Grand Strategy & Real-Time Tactical RTS Civilization Simulation (*AoE2* mass combat + *Manor Lords* physical logistics + *Frostpunk* Hope/Discontent & Moral Edicts + 3-Estate Power Triad).
* **POV**: The Imperial Sovereign managing the volatile triad of **The High Priesthood**, **The Warlord Nobility**, and **The Common Masses** while waging real-time continental conquest.
* **Scale**: Colossal fantasy-realism (*Elden Ring / GoT* magnitude: 80m–100m tall fortifications, 100-man mass legions, panoramic 160,000 unit orbital camera).
* **4 Architectural Eras**: Primitive Megalithic Mud $\rightarrow$ Angkor Wat Sandstone $\rightarrow$ Dark Gothic Monoliths $\rightarrow$ Victorian Industrial Grimdark.

---

## 2. Standing Instructions for Autonomous Development
1. **Strict Closed-Loop Mode**: One feature/vertical slice at a time.
2. **Deterministic Quality Gate**:
   - Clean UBT build (`unreal_build_project` returns `Result: Succeeded` with 0 errors).
   - Passed automated verification via MCP (`unreal_call_function`, `unreal_get_property`, etc.).
   - Comprehensive completion report with evidence.
3. **Preserve All Working Systems**: Camera rig, WASD navigation, marquee selection, formation slots, baggage supply lines, and HUD must remain 100% operational across all commits.
4. **Hardware Reality**: RTX 5060 8 GB VRAM. Zero local LLM inference while editor is running. Native modular C++ WorldSubsystems.

---

## 3. Progressive Backlog & Priority Order
1. ✅ **100-Man Cohort Formation Marching Cohesion** (Phalanx, Wedge, Skirmish, Square)
2. ✅ **Physical Ox-Cart Supply Lines & Starvation Attrition** (Proximity tether, decay, routing panic, `[T]` toggle)
3. 🎯 **Basic Combat Feedback & Spear Bracing Physics** (NEXT VERTICAL SLICE):
   - Phalanx spear bracing reflection damage against cavalry/chariot shock charges.
   - Melee thrust attack cycles, hit-stop impact feedback, blood/sparks particle traces, and shield block reflections.
4. ⏳ **The 3-Estate Power Triad & Hope/Discontent System** (Priesthood, Nobility, Masses gauges, unrest triggers, and Edict Lawbook).
5. ⏳ **Conquest MVP Victory Loop** (Keep destruction & Hegemonic Tributary Subjugation).
