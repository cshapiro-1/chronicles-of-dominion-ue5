import unreal

print("=== CLEANING PROCEDURAL BASIC SHAPES FROM LVL_TOPDOWN ===")
world_path = "/Game/TopDown/Lvl_TopDown"
unreal.EditorLoadingAndSavingUtils.load_map(world_path)

editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
world = editor_subsystem.get_editor_world()

actors = unreal.EditorLevelLibrary.get_all_level_actors()
removed_count = 0

for a in actors:
    label = a.get_actor_label()
    # Remove any procedural basic shapes (Cube, Cylinder, Cone, etc.) or procedural blockouts
    if any(k in label for k in ["Ziggurat_", "Rampart_", "Gatehouse_", "Bastion_", "Courtyard_", "Colonnade_", "Granary_", "Freight_", "Palace_", "Citadel_Terrain", "SM_Cube", "SM_Cone", "SM_Ramp", "SM_Cylinder", "SM_SkySphere", "Playground"]):
        print(f"Removing procedural actor: {label}")
        unreal.EditorLevelLibrary.destroy_actor(a)
        removed_count += 1

print(f"Cleaned {removed_count} procedural actors.")
unreal.EditorLoadingAndSavingUtils.save_map(world, world_path)
print("=== LEVEL CLEANED AND SAVED ACCORDING TO GRAPHICS POLICY ===")
