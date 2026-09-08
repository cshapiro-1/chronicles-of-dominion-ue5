import unreal
import time

print("=== CAPTURING HIGH RESOLUTION TACTICAL SCREENSHOT ===")

world_path = "/Game/TopDown/Lvl_TopDown"
unreal.EditorLoadingAndSavingUtils.load_map(world_path)

editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
world = editor_subsystem.get_editor_world()

# Set camera location/rotation in editor if possible, or execute console commands
unreal.SystemLibrary.execute_console_command(world, "r.Lumen.DiffuseIndirect.Allow 1")
unreal.SystemLibrary.execute_console_command(world, "r.Shadow.Virtual.Enable 1")
unreal.SystemLibrary.execute_console_command(world, "r.ScreenPercentage 150")

# Request HighResShot
screenshot_path = "C:/Users/Collin/Documents/Unreal Projects/ChroniclesOfDominion/Saved/Screenshots/WindowsEditor/Dominion_TopDown_Tactical_Render.png"
unreal.AutomationLibrary.take_high_res_screenshot(1920, 1080, "Dominion_TopDown_Tactical_Render.png")
print("HighResShot command queued.")
