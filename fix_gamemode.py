import unreal

print("=== FIXING DOMINION GAMEMODE IN LEVEL ===")
world_path = "/Game/TopDown/Lvl_TopDown"
unreal.EditorLoadingAndSavingUtils.load_map(world_path)

editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
world = editor_subsystem.get_editor_world()
world_settings = world.get_world_settings()

gm_class = unreal.load_class(None, "/Script/DominionCore.DominionGameModeBase")
print("DominionGameModeBase Class:", gm_class)

if gm_class:
    world_settings.set_editor_property("default_game_mode", gm_class)
    print("SUCCESS: Set default_game_mode to DominionGameModeBase!")

saved = unreal.EditorLoadingAndSavingUtils.save_map(world, world_path)
print("SUCCESS: Map saved status:", saved)
