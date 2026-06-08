# Black Ledger - build the Phase-1 driving gym (flat floor, ramps, obstacles, lights)
# and save it as /Game/BlackLedger/Maps/L_Gym.   UE 5.7 API (EditorActorSubsystem).
#
# Run with the EDITOR CLOSED:
#   & "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" `
#     "C:\Users\csmit\black-ledger\BlackLedger.uproject" `
#     -ExecCmds="py C:/Users/csmit/black-ledger/tools/unreal/make_gym_map.py" `
#     -stdout -unattended -nosplash -nosound -nullrhi -nopause

import unreal, traceback

MAP_PATH = "/Game/BlackLedger/Maps/L_Gym"

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
eal = unreal.EditorAssetLibrary

def spawn(cls, loc, rot=(0, 0, 0), label=None):
    a = actors.spawn_actor_from_class(cls, unreal.Vector(*loc), unreal.Rotator(*rot))
    if not a:
        raise RuntimeError(f"spawn failed: {cls.get_name()} at {loc}")
    if label:
        a.set_actor_label(label)
    return a

def spawn_cube(loc, scale, rot=(0, 0, 0), label=None):
    a = spawn(unreal.StaticMeshActor, loc, rot, label)
    smc = a.static_mesh_component
    smc.set_editor_property("static_mesh", eal.load_asset("/Engine/BasicShapes/Cube"))
    smc.set_editor_property("mobility", unreal.ComponentMobility.STATIC)
    a.set_actor_scale3d(unreal.Vector(*scale))
    return a

SCRATCH = "/Game/BlackLedger/Maps/L_Scratch"

def main():
    # the editor boots INTO L_Gym (startup map) - we cannot delete the loaded level.
    # step onto a scratch level first, then clear and recreate L_Gym.
    if eal.does_asset_exist(SCRATCH):
        eal.delete_asset(SCRATCH)
    if not les.new_level(SCRATCH):
        raise RuntimeError("scratch new_level failed")
    if eal.does_asset_exist(MAP_PATH):
        if not eal.delete_asset(MAP_PATH):
            raise RuntimeError("could not delete stale L_Gym")
        unreal.log_warning(f"deleted stale {MAP_PATH}")
    if not les.new_level(MAP_PATH):
        raise RuntimeError("new_level failed")

    # floor: 240 m x 240 m, top surface at z=0
    spawn_cube((0, 0, -50), (240, 240, 1), label="Floor")

    # perimeter walls (4 m high)
    spawn_cube((0,  12050, 150), (240, 1, 4), label="Wall_N")
    spawn_cube((0, -12050, 150), (240, 1, 4), label="Wall_S")
    spawn_cube(( 12050, 0, 150), (1, 240, 4), label="Wall_E")
    spawn_cube((-12050, 0, 150), (1, 240, 4), label="Wall_W")

    # ramps
    spawn_cube((3000, 0, 90),      (14, 10, 0.6), rot=(0, 12, 0),   label="Ramp_A")
    spawn_cube((-3500, 2500, 120), (16, 12, 0.6), rot=(0, 16, 180), label="Ramp_B")
    spawn_cube((1500, -4000, 70),  (10, 8, 0.6),  rot=(0, 9, 90),   label="Ramp_C")

    # slalom blocks
    for i, (x, y) in enumerate([(800, 1200), (-1500, -900), (2500, -2200),
                                (-2800, 1800), (4500, 3000), (-4200, -3400)]):
        spawn_cube((x, y, 150), (3, 3, 3), label=f"Block_{i}")

    # lighting: grim overcast
    sun = spawn(unreal.DirectionalLight, (0, 0, 1000), (0, -38, 35), "Sun")
    sun.light_component.set_editor_property("intensity", 6.0)
    sky = spawn(unreal.SkyLight, (0, 0, 1000), label="SkyLight")
    sky.light_component.set_editor_property("intensity", 1.2)
    spawn(unreal.SkyAtmosphere, (0, 0, 0), label="Atmosphere")
    spawn(unreal.ExponentialHeightFog, (0, 0, 0), label="Fog")

    # player start, facing +X
    spawn(unreal.PlayerStart, (0, 0, 250), (0, 0, 0), "PlayerStart")

    n = len(actors.get_all_level_actors())
    if n < 18:
        raise RuntimeError(f"only {n} actors in level - something failed")
    if not les.save_current_level():
        raise RuntimeError("save_current_level failed")
    if eal.does_asset_exist(SCRATCH):
        eal.delete_asset(SCRATCH)  # we are on L_Gym now; scratch is safe to remove
    unreal.log_warning(f"GYM MAP OK: {MAP_PATH} ({n} actors)")

try:
    main()
except Exception:
    unreal.log_error("GYM MAP FAILED")
    unreal.log_error(traceback.format_exc())
finally:
    unreal.SystemLibrary.quit_editor()
