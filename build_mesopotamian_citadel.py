import unreal

print("=== STARTING MESOPOTAMIAN CITADEL GENERATION ===")

world_path = "/Game/TopDown/Lvl_TopDown"
unreal.EditorLoadingAndSavingUtils.load_map(world_path)

editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
world = editor_subsystem.get_editor_world()

cube_mesh = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Cube.Cube")
cylinder_mesh = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Cylinder.Cylinder")
cone_mesh = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Cone.Cone")

mat_sandstone = unreal.EditorAssetLibrary.load_asset("/Game/M_HewnSandstone")
mat_desert = unreal.EditorAssetLibrary.load_asset("/Game/M_DesertTerrain_PBR")

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

# 1. Clean up existing starter cubes / playground props
actors = unreal.EditorLevelLibrary.get_all_level_actors()
for a in actors:
    label = a.get_actor_label()
    if any(k in label for k in ["SM_Cube", "SM_Cone", "SM_Ramp", "Template", "Playground", "Block_"]):
        print(f"Removing old starter prop: {label}")
        unreal.EditorLevelLibrary.destroy_actor(a)
    elif "skysphere" in label.lower():
        print(f"Removing SkySphere: {label}")
        unreal.EditorLevelLibrary.destroy_actor(a)

# 2. Lighting & Atmosphere Rig
directional_light = None
sky_light = None
sky_atmosphere = None
fog = None
pp_volume = None

for a in unreal.EditorLevelLibrary.get_all_level_actors():
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

if not directional_light:
    directional_light = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(0, 0, 2000), unreal.Rotator(-32.0, 48.0, 0.0))
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

if not sky_atmosphere:
    sky_atmosphere = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.SkyAtmosphere, unreal.Vector(0, 0, 0))
    sky_atmosphere.set_actor_label("SkyAtmosphere_Dominion")

if not sky_light:
    sky_light = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(0, 0, 500))
    sky_light.set_actor_label("SkyLight_Dominion")

sl_comp = sky_light.light_component
safe_set(sl_comp, "intensity", 3.0)
safe_set(sl_comp, "light_color", unreal.Color(205, 225, 255, 255))
safe_set(sl_comp, "real_time_capture", True)
safe_set(sl_comp, "cast_shadows", True)

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

if not pp_volume:
    pp_volume = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.PostProcessVolume, unreal.Vector(0, 0, 0))
    pp_volume.set_actor_label("PP_DominionTactical")

safe_set(pp_volume, "b_unbound", True)

def spawn_struct(label, mesh, mat, loc, rot, scale, folder="Citadel"):
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.StaticMeshActor, loc, rot)
    actor.set_actor_label(label)
    actor.set_folder_path(folder)
    smc = actor.static_mesh_component
    smc.set_mobility(unreal.ComponentMobility.STATIC)
    if mesh:
        smc.set_static_mesh(mesh)
    if mat:
        smc.set_material(0, mat)
    actor.set_actor_scale3d(scale)
    return actor

print("Building Mesopotamian Architecture...")

# 3. Ground Expanse (Desert bedrock & clay sands)
spawn_struct("Citadel_Terrain_Expanse", cube_mesh, mat_desert, unreal.Vector(0, 0, -50), unreal.Rotator(0, 0, 0), unreal.Vector(200.0, 200.0, 1.0), "Environment")

# 4. Central Sacred Ziggurat of Ur (North Acropolis)
ziggurat_center_x = 0
ziggurat_center_y = 1800

# Tier 1 (Base Platform): 48m x 48m, 5m high
spawn_struct("Ziggurat_Tier_1", cube_mesh, mat_sandstone, unreal.Vector(ziggurat_center_x, ziggurat_center_y, 250), unreal.Rotator(0, 0, 0), unreal.Vector(48.0, 48.0, 5.0), "Citadel/Ziggurat")

# Tier 2 (Middle Terrace): 34m x 34m, 4.5m high
spawn_struct("Ziggurat_Tier_2", cube_mesh, mat_sandstone, unreal.Vector(ziggurat_center_x, ziggurat_center_y, 725), unreal.Rotator(0, 0, 0), unreal.Vector(34.0, 34.0, 4.5), "Citadel/Ziggurat")

# Tier 3 (Upper Sanctum): 22m x 22m, 4m high
spawn_struct("Ziggurat_Tier_3", cube_mesh, mat_sandstone, unreal.Vector(ziggurat_center_x, ziggurat_center_y, 1150), unreal.Rotator(0, 0, 0), unreal.Vector(22.0, 22.0, 4.0), "Citadel/Ziggurat")

# Tier 4 (High Cella / Altar of Anu): 12m x 12m, 3.5m high
spawn_struct("Ziggurat_Altar_Cella", cube_mesh, mat_sandstone, unreal.Vector(ziggurat_center_x, ziggurat_center_y, 1525), unreal.Rotator(0, 0, 0), unreal.Vector(12.0, 12.0, 3.5), "Citadel/Ziggurat")

# Grand Axial Processional Ramp (Facing South into Courtyard)
spawn_struct("Ziggurat_Grand_Ramp_T1", cube_mesh, mat_sandstone, unreal.Vector(ziggurat_center_x, ziggurat_center_y - 2800, 125), unreal.Rotator(12.0, 0.0, 0.0), unreal.Vector(8.0, 16.0, 2.5), "Citadel/Ziggurat")
spawn_struct("Ziggurat_Grand_Ramp_T2", cube_mesh, mat_sandstone, unreal.Vector(ziggurat_center_x, ziggurat_center_y - 2000, 480), unreal.Rotator(16.0, 0.0, 0.0), unreal.Vector(6.0, 12.0, 2.0), "Citadel/Ziggurat")

# Flanking Braziers / Obelisks on Ziggurat Corners
for ox, oy in [(-2200, -2200), (2200, -2200), (-2200, 2200), (2200, 2200)]:
    spawn_struct(f"Ziggurat_Pylon_{ox}_{oy}", cylinder_mesh, mat_sandstone, unreal.Vector(ziggurat_center_x + ox, ziggurat_center_y + oy, 600), unreal.Rotator(0, 0, 0), unreal.Vector(1.5, 1.5, 7.0), "Citadel/Ziggurat")

# 5. Inner Fortress Perimeter Ramparts (Clay-Brick Bastions)
wall_h = 7.0  # 7m high
wall_thick = 2.5

# North Wall
spawn_struct("Rampart_North_Wall", cube_mesh, mat_sandstone, unreal.Vector(0, 4500, 350), unreal.Rotator(0, 0, 0), unreal.Vector(90.0, wall_thick, wall_h), "Citadel/Ramparts")

# East Wall
spawn_struct("Rampart_East_Wall", cube_mesh, mat_sandstone, unreal.Vector(4500, 1000, 350), unreal.Rotator(0, 90, 0), unreal.Vector(70.0, wall_thick, wall_h), "Citadel/Ramparts")

# West Wall
spawn_struct("Rampart_West_Wall", cube_mesh, mat_sandstone, unreal.Vector(-4500, 1000, 350), unreal.Rotator(0, 90, 0), unreal.Vector(70.0, wall_thick, wall_h), "Citadel/Ramparts")

# South Gatehouse Bastions (Flanking Grand Entrance)
spawn_struct("Rampart_South_Wall_Left", cube_mesh, mat_sandstone, unreal.Vector(-2800, -2500, 350), unreal.Rotator(0, 0, 0), unreal.Vector(35.0, wall_thick, wall_h), "Citadel/Ramparts")
spawn_struct("Rampart_South_Wall_Right", cube_mesh, mat_sandstone, unreal.Vector(2800, -2500, 350), unreal.Rotator(0, 0, 0), unreal.Vector(35.0, wall_thick, wall_h), "Citadel/Ramparts")

# Monumental South Gateway Towers (Pylons)
spawn_struct("Gatehouse_Tower_West", cube_mesh, mat_sandstone, unreal.Vector(-800, -2500, 550), unreal.Rotator(0, 0, 0), unreal.Vector(8.0, 8.0, 11.0), "Citadel/Ramparts")
spawn_struct("Gatehouse_Tower_East", cube_mesh, mat_sandstone, unreal.Vector(800, -2500, 550), unreal.Rotator(0, 0, 0), unreal.Vector(8.0, 8.0, 11.0), "Citadel/Ramparts")
spawn_struct("Gatehouse_Arch_Lintel", cube_mesh, mat_sandstone, unreal.Vector(0, -2500, 950), unreal.Rotator(0, 0, 0), unreal.Vector(16.0, 6.0, 2.5), "Citadel/Ramparts")

# 4 Heavy Corner Bastions
corner_coords = [
    ("NW", -4500, 4500),
    ("NE", 4500, 4500),
    ("SW", -4500, -2500),
    ("SE", 4500, -2500)
]
for cname, cx, cy in corner_coords:
    spawn_struct(f"Bastion_Tower_{cname}", cube_mesh, mat_sandstone, unreal.Vector(cx, cy, 550), unreal.Rotator(0, 0, 0), unreal.Vector(10.0, 10.0, 11.0), "Citadel/Ramparts")

# 6. Central Royal Parade Courtyard (Phalanx & Legion Staging Ground)
spawn_struct("Courtyard_Paved_Floor", cube_mesh, mat_desert, unreal.Vector(0, -500, 5), unreal.Rotator(0, 0, 0), unreal.Vector(60.0, 40.0, 0.2), "Citadel/Courtyard")

# Flanking Colonnade Pillars along Courtyard Wings
for y in range(-2000, 1000, 500):
    spawn_struct(f"Colonnade_Pillar_W_{y}", cylinder_mesh, mat_sandstone, unreal.Vector(-2800, y, 250), unreal.Rotator(0, 0, 0), unreal.Vector(1.2, 1.2, 5.0), "Citadel/Colonnade")
    spawn_struct(f"Colonnade_Pillar_E_{y}", cylinder_mesh, mat_sandstone, unreal.Vector(2800, y, 250), unreal.Rotator(0, 0, 0), unreal.Vector(1.2, 1.2, 5.0), "Citadel/Colonnade")

# 7. Logistics & Granary Depot Complex (North-West Sector)
granary_coords = [
    ("Vault_1", -3400, 2600),
    ("Vault_2", -3400, 3600),
    ("Vault_3", -2400, 2600),
    ("Vault_4", -2400, 3600),
]
for gname, gx, gy in granary_coords:
    spawn_struct(f"Granary_{gname}", cube_mesh, mat_sandstone, unreal.Vector(gx, gy, 250), unreal.Rotator(0, 0, 0), unreal.Vector(6.0, 6.0, 5.0), "Citadel/Logistics")
    spawn_struct(f"Granary_Dome_{gname}", cone_mesh, mat_sandstone, unreal.Vector(gx, gy, 550), unreal.Rotator(0, 0, 0), unreal.Vector(6.0, 6.0, 2.5), "Citadel/Logistics")

# Raised Freight Platform for Baggage Trains
spawn_struct("Freight_Loading_Dock", cube_mesh, mat_sandstone, unreal.Vector(-2900, 1900, 100), unreal.Rotator(0, 0, 0), unreal.Vector(14.0, 8.0, 2.0), "Citadel/Logistics")

# 8. Royal Treasury & Palace Wing (North-East Sector)
palace_coords = [
    ("Palace_Keep", 2900, 3200, unreal.Vector(16.0, 14.0, 7.0), 350),
    ("Palace_Annex", 2900, 2000, unreal.Vector(12.0, 10.0, 5.5), 275),
]
for pname, px, py, pscale, pz in palace_coords:
    spawn_struct(pname, cube_mesh, mat_sandstone, unreal.Vector(px, py, pz), unreal.Rotator(0, 0, 0), pscale, "Citadel/Palace")

# 9. Update NavMeshBoundsVolume to cover the entire citadel
nav_actors = [a for a in unreal.EditorLevelLibrary.get_all_level_actors() if isinstance(a, unreal.NavMeshBoundsVolume)]
if nav_actors:
    nav = nav_actors[0]
    nav.set_actor_location(unreal.Vector(0, 500, 500), False, False)
    nav.set_actor_scale3d(unreal.Vector(120.0, 120.0, 20.0))
    print("NavMeshBoundsVolume calibrated to 120m x 120m expanse.")

# 10. Position PlayerStart in the Courtyard facing the Ziggurat
ps_actors = [a for a in unreal.EditorLevelLibrary.get_all_level_actors() if isinstance(a, unreal.PlayerStart)]
if ps_actors:
    ps = ps_actors[0]
    ps.set_actor_location(unreal.Vector(0, -1800, 100), False, False)
    ps.set_actor_rotation(unreal.Rotator(0, 90, 0), False)
    print("PlayerStart set to Royal Courtyard (facing Ziggurat).")

# 11. Save Map
unreal.EditorLoadingAndSavingUtils.save_map(world, world_path)
print("=== MESOPOTAMIAN CITADEL LEVEL GENERATED AND SAVED SUCCESSFULLY! ===")
