import unreal
import math

print("=== STARTING MASTER OVERHAUL OF LVL_TOPDOWN ===")

world_path = "/Game/TopDown/Lvl_TopDown"
unreal.EditorLoadingAndSavingUtils.load_map(world_path)

editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
world = editor_subsystem.get_editor_world()
asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
material_lib = unreal.MaterialEditingLibrary

def safe_set(target, prop, val):
    try:
        target.set_editor_property(prop, val)
        return True
    except Exception:
        try:
            setattr(target, prop, val)
            return True
        except Exception:
            return False

# ==============================================================================
# 1. ATMOSPHERE & REAL-TIME LIGHTING RIG
# ==============================================================================
actors = unreal.EditorLevelLibrary.get_all_level_actors()

directional_light = None
sky_light = None
sky_atmosphere = None
fog = None
pp_volume = None

for a in actors:
    if isinstance(a, unreal.DirectionalLight):
        directional_light = a
    elif isinstance(a, unreal.SkyLight):
        sky_light = a
    elif isinstance(a, unreal.SkyAtmosphere):
        sky_atmosphere = a
    elif isinstance(a, unreal.ExponentialHeightFog):
        fog = a
    elif isinstance(a, unreal.PostProcessVolume):
        pp_volume = a
    elif "skysphere" in a.get_actor_label().lower():
        print(f"Destroying obsolete SkySphere: {a.get_actor_label()}")
        unreal.EditorLevelLibrary.destroy_actor(a)

# Directional Light (Sun)
if not directional_light:
    directional_light = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(0, 0, 1000), unreal.Rotator(-35.0, 45.0, 0.0))
    directional_light.set_actor_label("Sun_DirectionalLight")

directional_light.set_actor_rotation(unreal.Rotator(-32.0, 48.0, 0.0), False)
dl_comp = directional_light.light_component
safe_set(dl_comp, "intensity", 8.0)
safe_set(dl_comp, "light_color", unreal.Color(255, 242, 218, 255))
safe_set(dl_comp, "temperature", 5600.0)
safe_set(dl_comp, "use_temperature", True)
safe_set(dl_comp, "atmosphere_sun_light", True)
safe_set(dl_comp, "cast_shadows", True)
safe_set(dl_comp, "cast_volumetric_shadow", True)

# SkyAtmosphere
if not sky_atmosphere:
    sky_atmosphere = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.SkyAtmosphere, unreal.Vector(0, 0, 0))
    sky_atmosphere.set_actor_label("SkyAtmosphere_Dominion")

# SkyLight (Real-time capture for warm ambient bounce in shadows)
if not sky_light:
    sky_light = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(0, 0, 500))
    sky_light.set_actor_label("SkyLight_Dominion")

sl_comp = sky_light.light_component
safe_set(sl_comp, "intensity", 3.0)
safe_set(sl_comp, "light_color", unreal.Color(200, 220, 255, 255))
safe_set(sl_comp, "real_time_capture", True)
safe_set(sl_comp, "cast_shadows", True)

# Exponential Height Fog (Volumetric Desert Dust & Light Shafts)
if not fog:
    fog = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.ExponentialHeightFog, unreal.Vector(0, 0, 0))
    fog.set_actor_label("Fog_MesopotamianDust")

fog_comp = fog.component
safe_set(fog_comp, "fog_density", 0.015)
safe_set(fog_comp, "fog_height_falloff", 0.03)
safe_set(fog_comp, "fog_inscattering_luminance", unreal.LinearColor(0.85, 0.65, 0.42, 1.0))
safe_set(fog_comp, "volumetric_fog_density", 0.015)
safe_set(fog_comp, "b_enable_volumetric_fog", True)
safe_set(fog_comp, "enable_volumetric_fog", True)

# Post-Process Volume
if not pp_volume:
    pp_volume = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.PostProcessVolume, unreal.Vector(0, 0, 0))
    pp_volume.set_actor_label("PP_DominionTactical")

safe_set(pp_volume, "b_unbound", True)
safe_set(pp_volume, "unbound", True)

settings = pp_volume.get_editor_property("settings")
safe_set(settings, "override_bloom_intensity", True)
safe_set(settings, "bloom_intensity", 0.65)
safe_set(settings, "override_white_temp", True)
safe_set(settings, "white_temp", 6200.0)
safe_set(settings, "override_color_saturation", True)
safe_set(settings, "color_saturation", unreal.Vector4(1.08, 1.08, 1.08, 1.0))
safe_set(settings, "override_color_contrast", True)
safe_set(settings, "color_contrast", unreal.Vector4(1.12, 1.12, 1.12, 1.0))
safe_set(settings, "override_auto_exposure_min_brightness", True)
safe_set(settings, "auto_exposure_min_brightness", 6.0)
safe_set(settings, "override_auto_exposure_max_brightness", True)
safe_set(settings, "auto_exposure_max_brightness", 9.5)
safe_set(settings, "override_ambient_occlusion_intensity", True)
safe_set(settings, "ambient_occlusion_intensity", 1.0)
safe_set(settings, "override_ambient_occlusion_radius", True)
safe_set(settings, "ambient_occlusion_radius", 120.0)
pp_volume.set_editor_property("settings", settings)
print("SUCCESS: Lighting & Post-Process Rig Configured.")

# ==============================================================================
# 2. HIGH-FIDELITY PBR TERRAIN SHADER (World-Position Macro Noise + Perturbation)
# ==============================================================================
mat_terrain_path = "/Game/M_DesertTerrain_PBR"
mat_terrain = unreal.EditorAssetLibrary.load_asset(mat_terrain_path)
if not mat_terrain:
    mat_terrain = asset_tools.create_asset("M_DesertTerrain_PBR", "/Game", unreal.Material, unreal.MaterialFactoryNew())

if mat_terrain:
    material_lib.delete_all_material_expressions(mat_terrain)
    
    # WorldPosition Node for Macro Texture Variance
    wp_node = material_lib.create_material_expression(mat_terrain, unreal.MaterialExpressionWorldPosition, -900, -200)
    
    # Divide WP by 2000 for macro color sweep
    div_macro = material_lib.create_material_expression(mat_terrain, unreal.MaterialExpressionDivide, -700, -200)
    div_macro.set_editor_property("const_b", 2000.0)
    material_lib.connect_material_expressions(wp_node, "", div_macro, "A")
    
    # Cosine wave for smooth natural gradient
    cos_node = material_lib.create_material_expression(mat_terrain, unreal.MaterialExpressionCosine, -550, -200)
    material_lib.connect_material_expressions(div_macro, "", cos_node, "")
    
    # Linear Interpolate between Silt Sand and Baked Terracotta Clay
    lerp_color = material_lib.create_material_expression(mat_terrain, unreal.MaterialExpressionLinearInterpolate, -350, -200)
    
    color_clay = material_lib.create_material_expression(mat_terrain, unreal.MaterialExpressionVectorParameter, -600, -400)
    color_clay.set_editor_property("parameter_name", "ColorTerracottaClay")
    color_clay.set_editor_property("default_value", unreal.LinearColor(0.68, 0.44, 0.24, 1.0))
    
    color_silt = material_lib.create_material_expression(mat_terrain, unreal.MaterialExpressionVectorParameter, -600, -280)
    color_silt.set_editor_property("parameter_name", "ColorGoldenSilt")
    color_silt.set_editor_property("default_value", unreal.LinearColor(0.84, 0.68, 0.42, 1.0))
    
    material_lib.connect_material_expressions(color_clay, "", lerp_color, "A")
    material_lib.connect_material_expressions(color_silt, "", lerp_color, "B")
    material_lib.connect_material_expressions(cos_node, "", lerp_color, "Alpha")
    material_lib.connect_material_property(lerp_color, "", unreal.MaterialProperty.MP_BASE_COLOR)
    
    # Roughness variation
    rough_node = material_lib.create_material_expression(mat_terrain, unreal.MaterialExpressionScalarParameter, -350, 50)
    rough_node.set_editor_property("parameter_name", "TerrainRoughness")
    rough_node.set_editor_property("default_value", 0.88)
    material_lib.connect_material_property(rough_node, "", unreal.MaterialProperty.MP_ROUGHNESS)
    
    # Specular
    spec_node = material_lib.create_material_expression(mat_terrain, unreal.MaterialExpressionScalarParameter, -350, 180)
    spec_node.set_editor_property("parameter_name", "TerrainSpecular")
    spec_node.set_editor_property("default_value", 0.3)
    material_lib.connect_material_property(spec_node, "", unreal.MaterialProperty.MP_SPECULAR)
    
    material_lib.recompile_material(mat_terrain)
    unreal.EditorAssetLibrary.save_asset(mat_terrain_path)
    print("SUCCESS: Procedural M_DesertTerrain_PBR Compiled.")

# ==============================================================================
# 3. HIGH-FIDELITY HEWN SANDSTONE ARCHITECTURAL SHADER
# ==============================================================================
mat_stone_path = "/Game/M_HewnSandstone"
mat_stone = unreal.EditorAssetLibrary.load_asset(mat_stone_path)
if not mat_stone:
    mat_stone = asset_tools.create_asset("M_HewnSandstone", "/Game", unreal.Material, unreal.MaterialFactoryNew())

if mat_stone:
    material_lib.delete_all_material_expressions(mat_stone)
    
    # World Position for height weathering gradient
    wp_stone = material_lib.create_material_expression(mat_stone, unreal.MaterialExpressionWorldPosition, -900, -200)
    mask_z = material_lib.create_material_expression(mat_stone, unreal.MaterialExpressionComponentMask, -700, -200)
    mask_z.set_editor_property("b", True)
    mask_z.set_editor_property("r", False)
    mask_z.set_editor_property("g", False)
    mask_z.set_editor_property("a", False)
    material_lib.connect_material_expressions(wp_stone, "", mask_z, "")
    
    # Divide Z by 800
    div_z = material_lib.create_material_expression(mat_stone, unreal.MaterialExpressionDivide, -550, -200)
    div_z.set_editor_property("const_b", 800.0)
    material_lib.connect_material_expressions(mask_z, "", div_z, "A")
    
    # Clamp 0 to 1
    clamp_z = material_lib.create_material_expression(mat_stone, unreal.MaterialExpressionClamp, -420, -200)
    material_lib.connect_material_expressions(div_z, "", clamp_z, "")
    
    # Lerp between Foundation Weathered Basalt and Upper Hewn Sandstone
    lerp_stone = material_lib.create_material_expression(mat_stone, unreal.MaterialExpressionLinearInterpolate, -250, -200)
    
    color_base_dirt = material_lib.create_material_expression(mat_stone, unreal.MaterialExpressionVectorParameter, -550, -400)
    color_base_dirt.set_editor_property("parameter_name", "BaseBasaltDirt")
    color_base_dirt.set_editor_property("default_value", unreal.LinearColor(0.48, 0.36, 0.26, 1.0))
    
    color_upper_stone = material_lib.create_material_expression(mat_stone, unreal.MaterialExpressionVectorParameter, -550, -280)
    color_upper_stone.set_editor_property("parameter_name", "UpperHewnSandstone")
    color_upper_stone.set_editor_property("default_value", unreal.LinearColor(0.72, 0.56, 0.38, 1.0))
    
    material_lib.connect_material_expressions(color_base_dirt, "", lerp_stone, "A")
    material_lib.connect_material_expressions(color_upper_stone, "", lerp_stone, "B")
    material_lib.connect_material_expressions(clamp_z, "", lerp_stone, "Alpha")
    material_lib.connect_material_property(lerp_stone, "", unreal.MaterialProperty.MP_BASE_COLOR)
    
    # Roughness
    stone_rough = material_lib.create_material_expression(mat_stone, unreal.MaterialExpressionScalarParameter, -250, 50)
    stone_rough.set_editor_property("parameter_name", "StoneRoughness")
    stone_rough.set_editor_property("default_value", 0.86)
    material_lib.connect_material_property(stone_rough, "", unreal.MaterialProperty.MP_ROUGHNESS)
    
    material_lib.recompile_material(mat_stone)
    unreal.EditorAssetLibrary.save_asset(mat_stone_path)
    print("SUCCESS: Procedural M_HewnSandstone Compiled.")

# ==============================================================================
# 4. ENVIRONMENT STAGING & GROUND EXPANSION
# ==============================================================================
for a in unreal.EditorLevelLibrary.get_all_level_actors():
    label = a.get_actor_label().lower()
    if isinstance(a, unreal.StaticMeshActor):
        smc = a.static_mesh_component
        if not smc: continue
        
        if "floor" in label or "ground" in label:
            a.set_actor_scale3d(unreal.Vector(120.0, 120.0, 1.0))
            smc.set_material(0, mat_terrain)
            print(f"Expanded and assigned M_DesertTerrain_PBR to ground expanse: {a.get_actor_label()}")
        else:
            num_mats = smc.get_num_materials()
            for i in range(num_mats):
                smc.set_material(i, mat_stone)

# ==============================================================================
# 5. ENGINE CVARS & SAVE
# ==============================================================================
unreal.SystemLibrary.execute_console_command(world, "r.Lumen.DiffuseIndirect.Allow 1")
unreal.SystemLibrary.execute_console_command(world, "r.Lumen.Reflections.Allow 1")
unreal.SystemLibrary.execute_console_command(world, "r.Shadow.Virtual.Enable 1")
unreal.SystemLibrary.execute_console_command(world, "r.ScreenPercentage 150")

unreal.EditorLoadingAndSavingUtils.save_map(world, world_path)
print("=== MASTER OVERHAUL COMPLETED SUCCESSFULLY! ===")
