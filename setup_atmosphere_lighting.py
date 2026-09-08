import unreal

print("=== INSPECTING AND FIXING LIGHTING & ATMOSPHERE RIG ===")

editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
world = editor_subsystem.get_editor_world()

actors = unreal.EditorLevelLibrary.get_all_level_actors()
print(f"Total actors in world: {len(actors)}")

directional_light = None
sky_light = None
sky_atmosphere = None
fog = None
volumetric_cloud = None
post_process = None

for a in actors:
    cname = a.get_class().get_name()
    label = a.get_actor_label()
    if isinstance(a, unreal.DirectionalLight):
        directional_light = a
    elif isinstance(a, unreal.SkyLight):
        sky_light = a
    elif isinstance(a, unreal.SkyAtmosphere):
        sky_atmosphere = a
    elif isinstance(a, unreal.ExponentialHeightFog):
        fog = a
    elif isinstance(a, unreal.VolumetricCloud):
        volumetric_cloud = a
    elif isinstance(a, unreal.PostProcessVolume):
        post_process = a

print(f"DirectionalLight: {directional_light}")
print(f"SkyLight: {sky_light}")
print(f"SkyAtmosphere: {sky_atmosphere}")
print(f"ExponentialHeightFog: {fog}")
print(f"VolumetricCloud: {volumetric_cloud}")
print(f"PostProcessVolume: {post_process}")

# 1. Spawn or configure Directional Light (Sun)
if not directional_light:
    print("Spawning DirectionalLight...")
    directional_light = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(0, 0, 1000), unreal.Rotator(-45, 45, 0))
    directional_light.set_actor_label("Sun_DirectionalLight")

dl_comp = directional_light.light_component
dl_comp.set_editor_property("intensity", 6.0) # Lux
dl_comp.set_editor_property("light_color", unreal.Color(255, 240, 215, 255)) # Warm sunlight
dl_comp.set_editor_property("temperature", 5800.0)
dl_comp.set_editor_property("use_temperature", True)
dl_comp.set_editor_property("atmosphere_sun_light", True)
dl_comp.set_editor_property("cast_shadows", True)
dl_comp.set_editor_property("cast_volumetric_shadow", True)
directional_light.set_actor_rotation(unreal.Rotator(-38.0, 52.0, 0.0), False)

# 2. Spawn or configure SkyAtmosphere
if not sky_atmosphere:
    print("Spawning SkyAtmosphere...")
    sky_atmosphere = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.SkyAtmosphere, unreal.Vector(0, 0, 0))
    sky_atmosphere.set_actor_label("SkyAtmosphere_Dominion")

# 3. Spawn or configure SkyLight
if not sky_light:
    print("Spawning SkyLight...")
    sky_light = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(0, 0, 500))
    sky_light.set_actor_label("SkyLight_Dominion")

sl_comp = sky_light.light_component
sl_comp.set_editor_property("intensity", 2.5) # Soft ambient fill
sl_comp.set_editor_property("light_color", unreal.Color(210, 230, 255, 255)) # Soft atmospheric skylight fill
sl_comp.set_editor_property("real_time_capture", True)
sl_comp.set_editor_property("cast_shadows", True)

# 4. Spawn or configure ExponentialHeightFog (Volumetric Mesopotamian Dust)
if not fog:
    print("Spawning ExponentialHeightFog...")
    fog = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.ExponentialHeightFog, unreal.Vector(0, 0, 0))
    fog.set_actor_label("Fog_MesopotamianDust")

fog_comp = fog.component
fog_comp.set_editor_property("fog_density", 0.02)
fog_comp.set_editor_property("fog_height_falloff", 0.05)
fog_comp.set_editor_property("fog_inscattering_luminance", unreal.LinearColor(0.85, 0.65, 0.45, 1.0)) # Warm dust
fog_comp.set_editor_property("volumetric_fog", True)
fog_comp.set_editor_property("volumetric_fog_scattering_distribution", 0.7) # Forward scattering sun rays
fog_comp.set_editor_property("volumetric_fog_albedo", unreal.Color(230, 200, 160, 255))
fog_comp.set_editor_property("volumetric_fog_extinction_scale", 1.5)

# 5. Spawn VolumetricCloud if none
if not volumetric_cloud:
    try:
        volumetric_cloud = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.VolumetricCloud, unreal.Vector(0, 0, 0))
        volumetric_cloud.set_actor_label("VolumetricCloud_Dominion")
        print("Spawned VolumetricCloud.")
    except Exception as e:
        print("VolumetricCloud spawn:", e)

# 6. Delete or fix SM_SkySphere if it's blocking the SkyAtmosphere
for a in actors:
    if "skysphere" in a.get_actor_label().lower():
        print(f"Removing old blocking skysphere: {a.get_actor_label()}")
        unreal.EditorLevelLibrary.destroy_actor(a)

print("SUCCESS: Lighting & Atmosphere Rig Fully Calibrated!")
