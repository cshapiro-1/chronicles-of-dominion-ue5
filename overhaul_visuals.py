import unreal

print("=== STARTING VISUAL RENDERING OVERHAUL FOR LVL_TOPDOWN ===")

# 1. Load Level
world_path = "/Game/TopDown/Lvl_TopDown"
unreal.EditorLoadingAndSavingUtils.load_map(world_path)

editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
world = editor_subsystem.get_editor_world()
asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
material_lib = unreal.MaterialEditingLibrary

# 2. Setup PostProcessVolume
actors = unreal.EditorLevelLibrary.get_all_level_actors()
pp_volume = None
for actor in actors:
    if isinstance(actor, unreal.PostProcessVolume):
        pp_volume = actor
        break

if not pp_volume:
    print("Spawning new PostProcessVolume...")
    pp_volume = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.PostProcessVolume, unreal.Vector(0, 0, 0))
    pp_volume.set_actor_label("PP_DominionTactical")

# Set unbound
try:
    pp_volume.set_editor_property("b_unbound", True)
except Exception:
    try:
        pp_volume.set_editor_property("unbound", True)
    except Exception as e:
        print("Warning setting unbound property:", e)

# Retrieve PostProcessSettings struct
settings = pp_volume.get_editor_property("settings")

# Print available settings keys/attributes for inspection
props = [p for p in dir(settings) if not p.startswith("_")]
print(f"PostProcessSettings has {len(props)} properties.")

# Set properties directly on struct or via set_editor_property
def try_set(target, prop_name, val):
    try:
        target.set_editor_property(prop_name, val)
        return True
    except Exception:
        try:
            setattr(target, prop_name, val)
            return True
        except Exception as e:
            # print(f"Could not set {prop_name}: {e}")
            return False

# Bloom (Intensity = 0.65)
try_set(settings, "override_bloom_intensity", True)
try_set(settings, "bloom_intensity", 0.65)

# Color Grading (6200K, Saturation 1.05, Contrast 1.15)
try_set(settings, "override_white_temp", True)
try_set(settings, "white_temp", 6200.0)

try_set(settings, "override_color_saturation", True)
try_set(settings, "color_saturation", unreal.Vector4(1.05, 1.05, 1.05, 1.0))

try_set(settings, "override_color_contrast", True)
try_set(settings, "color_contrast", unreal.Vector4(1.15, 1.15, 1.15, 1.0))

# Auto-Exposure (Min EV100 = 7.0, Max EV100 = 9.0)
try_set(settings, "override_auto_exposure_min_brightness", True)
try_set(settings, "auto_exposure_min_brightness", 7.0)
try_set(settings, "override_auto_exposure_max_brightness", True)
try_set(settings, "auto_exposure_max_brightness", 9.0)

# Ambient Occlusion (Intensity = 1.0, Radius = 100.0)
try_set(settings, "override_ambient_occlusion_intensity", True)
try_set(settings, "ambient_occlusion_intensity", 1.0)
try_set(settings, "override_ambient_occlusion_radius", True)
try_set(settings, "ambient_occlusion_radius", 100.0)

pp_volume.set_editor_property("settings", settings)
print("SUCCESS: PostProcessVolume configured.")

# 3. Create / Configure M_DesertTerrain_PBR Material
mat_path = "/Game/M_DesertTerrain_PBR"
mat_asset = unreal.EditorAssetLibrary.load_asset(mat_path)
if not mat_asset:
    mat_asset = asset_tools.create_asset("M_DesertTerrain_PBR", "/Game", unreal.Material, unreal.MaterialFactoryNew())

if mat_asset:
    material_lib.delete_all_material_expressions(mat_asset)
    
    # Base Color: Warm Mesopotamian Terracotta Silt
    base_color_node = material_lib.create_material_expression(mat_asset, unreal.MaterialExpressionVectorParameter, -400, -100)
    base_color_node.set_editor_property("parameter_name", "TerrainBaseColor")
    base_color_node.set_editor_property("default_value", unreal.LinearColor(0.72, 0.52, 0.32, 1.0)) # Warm baked clay/silt
    material_lib.connect_material_property(base_color_node, "", unreal.MaterialProperty.MP_BASE_COLOR)
    
    # Roughness: Dry baked ground (0.85)
    roughness_node = material_lib.create_material_expression(mat_asset, unreal.MaterialExpressionScalarParameter, -400, 100)
    roughness_node.set_editor_property("parameter_name", "TerrainRoughness")
    roughness_node.set_editor_property("default_value", 0.85)
    material_lib.connect_material_property(roughness_node, "", unreal.MaterialProperty.MP_ROUGHNESS)
    
    # Specular: 0.35
    spec_node = material_lib.create_material_expression(mat_asset, unreal.MaterialExpressionScalarParameter, -400, 250)
    spec_node.set_editor_property("parameter_name", "TerrainSpecular")
    spec_node.set_editor_property("default_value", 0.35)
    material_lib.connect_material_property(spec_node, "", unreal.MaterialProperty.MP_SPECULAR)
    
    material_lib.recompile_material(mat_asset)
    unreal.EditorAssetLibrary.save_asset(mat_path)
    print("SUCCESS: M_DesertTerrain_PBR compiled and saved.")

# 4. Create / Configure M_HewnSandstone Material
sandstone_path = "/Game/M_HewnSandstone"
sandstone_asset = unreal.EditorAssetLibrary.load_asset(sandstone_path)
if not sandstone_asset:
    sandstone_asset = asset_tools.create_asset("M_HewnSandstone", "/Game", unreal.Material, unreal.MaterialFactoryNew())

if sandstone_asset:
    material_lib.delete_all_material_expressions(sandstone_asset)
    
    # Base Color: Weathered Sandstone with Warm Ochre Tone
    stone_color_node = material_lib.create_material_expression(sandstone_asset, unreal.MaterialExpressionVectorParameter, -400, -100)
    stone_color_node.set_editor_property("parameter_name", "StoneBaseColor")
    stone_color_node.set_editor_property("default_value", unreal.LinearColor(0.62, 0.46, 0.32, 1.0))
    material_lib.connect_material_property(stone_color_node, "", unreal.MaterialProperty.MP_BASE_COLOR)
    
    # Roughness: 0.88 for rough hewn rock
    stone_rough_node = material_lib.create_material_expression(sandstone_asset, unreal.MaterialExpressionScalarParameter, -400, 100)
    stone_rough_node.set_editor_property("parameter_name", "StoneRoughness")
    stone_rough_node.set_editor_property("default_value", 0.88)
    material_lib.connect_material_property(stone_rough_node, "", unreal.MaterialProperty.MP_ROUGHNESS)
    
    material_lib.recompile_material(sandstone_asset)
    unreal.EditorAssetLibrary.save_asset(sandstone_path)
    print("SUCCESS: M_HewnSandstone compiled and saved.")

# 5. Assign Materials to Level Actors
mat_terrain = unreal.EditorAssetLibrary.load_asset(mat_path)
mat_stone = unreal.EditorAssetLibrary.load_asset(sandstone_path)

for actor in actors:
    if isinstance(actor, unreal.StaticMeshActor):
        label = actor.get_actor_label().lower()
        smc = actor.static_mesh_component
        if not smc:
            continue
        
        # Ground floor mesh
        if "floor" in label or "ground" in label or "plane" in label or "terrain" in label:
            smc.set_material(0, mat_terrain)
            print(f"Assigned M_DesertTerrain_PBR to ground actor: {actor.get_actor_label()}")
        else:
            # Architecture / props / cubes / walls / ramps
            num_mats = smc.get_num_materials()
            for slot_idx in range(num_mats):
                smc.set_material(slot_idx, mat_stone)
            print(f"Assigned M_HewnSandstone to architectural actor: {actor.get_actor_label()}")

# 6. Apply High-Fidelity Rendering CVars
unreal.SystemLibrary.execute_console_command(world, "r.Lumen.DiffuseIndirect.Allow 1")
unreal.SystemLibrary.execute_console_command(world, "r.Shadow.Virtual.Enable 1")
unreal.SystemLibrary.execute_console_command(world, "r.ScreenPercentage 150")

# 7. Save Map
unreal.EditorLoadingAndSavingUtils.save_map(world, world_path)
print("SUCCESS: Lvl_TopDown saved with full visual overhaul!")
