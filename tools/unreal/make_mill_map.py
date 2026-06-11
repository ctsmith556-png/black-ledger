# Black Ledger - build The Mill greybox (vertical slice arena) per
# docs/vertical-slice/TheMill_Blockout.png and save as /Game/BlackLedger/Maps/L_Mill.
# Floor 170 m x 130 m, 3 furnace pits w/ orange glow, overhead catwalk
# (ABLDestructible_Catwalk - the collapse prototype), sealed bay, loading bays,
# pickups, nav bounds. UE 5.7 API (EditorActorSubsystem).
#
# REQUIRES the C++ module built first (spawns BLDestructible_Catwalk etc.).
# Run with the EDITOR CLOSED - and WITHOUT -nullrhi: as of UE 5.7.4, scripted
# actor spawns under -nullrhi die with EXCEPTION_INT_DIVIDE_BY_ZERO (asset
# imports are fine headless; anything that spawns actors needs a real RHI):
#   & "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" `
#     "C:\Users\csmit\black-ledger\BlackLedger.uproject" `
#     -ExecCmds="py C:/Users/csmit/black-ledger/tools/unreal/make_mill_map.py" `
#     -stdout -unattended -nosplash -nosound -nopause

import unreal, traceback

MAP_PATH = "/Game/BlackLedger/Maps/L_Mill"
SCRATCH = "/Game/BlackLedger/Maps/L_Scratch"

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


def spawn_shape(shape, loc, scale, rot=(0, 0, 0), label=None):
    a = spawn(unreal.StaticMeshActor, loc, rot, label)
    smc = a.static_mesh_component
    smc.set_editor_property("static_mesh", eal.load_asset(f"/Engine/BasicShapes/{shape}"))
    smc.set_editor_property("mobility", unreal.ComponentMobility.STATIC)
    a.set_actor_scale3d(unreal.Vector(*scale))
    return a


def spawn_cube(loc, scale, rot=(0, 0, 0), label=None):
    return spawn_shape("Cube", loc, scale, rot, label)


def furnace(x, y, name):
    # raised pit lip (cylinder), orange glow above it; pour hazard actor comes next
    spawn_shape("Cylinder", (x, y, 75), (18, 18, 1.5), label=f"Furnace_{name}")
    # modest ambient ember - must NOT compete with the pour hazard's warning light
    glow = spawn(unreal.PointLight, (x, y, 900), label=f"FurnaceGlow_{name}")
    lc = glow.light_component
    lc.set_editor_property("intensity", 8000.0)
    lc.set_editor_property("attenuation_radius", 3000.0)
    lc.set_editor_property("light_color", unreal.Color(255, 96, 18, 255))
    lc.set_editor_property("cast_shadows", False)


def main():
    # editor boots into the startup map - step onto scratch, then rebuild L_Mill
    if eal.does_asset_exist(SCRATCH):
        eal.delete_asset(SCRATCH)
    if not les.new_level(SCRATCH):
        raise RuntimeError("scratch new_level failed")
    if eal.does_asset_exist(MAP_PATH):
        if not eal.delete_asset(MAP_PATH):
            raise RuntimeError("could not delete stale L_Mill")
        unreal.log_warning(f"deleted stale {MAP_PATH}")
    if not les.new_level(MAP_PATH):
        raise RuntimeError("new_level failed")

    # ---- shell: floor 170 m x 130 m (top z=0), 8 m walls ----
    unreal.log_warning("MILL: shell")
    spawn_cube((0, 0, -50), (170, 130, 1), label="Floor")
    spawn_cube((0, 6550, 400), (170, 1, 8), label="Wall_N")
    spawn_cube((0, -6550, 400), (170, 1, 8), label="Wall_S")
    spawn_cube((8550, 0, 400), (1, 132, 8), label="Wall_E")
    spawn_cube((-8550, 0, 400), (1, 132, 8), label="Wall_W")

    # ---- furnaces F1/F2/F3 (blockout positions; F3 = boss rise point) ----
    unreal.log_warning("MILL: furnaces")
    furnace(-4200, 3300, "F1")
    furnace(4200, 3300, "F2")
    furnace(0, -3300, "F3")

    # ---- loading bays (raised slabs, SW + SE corners) ----
    unreal.log_warning("MILL: bays")
    spawn_cube((-6500, -5200, 25), (24, 18, 0.5), label="LoadingBay_SW")
    spawn_cube((6500, -5200, 25), (24, 18, 0.5), label="LoadingBay_SE")

    # ---- sealed bay (east wall pocket near F2; breach via furnace blast later) ----
    spawn_cube((6900, 1200, 200), (1, 14, 4), label="SealedBay_Wall")
    spawn_cube((7700, 2000, 200), (15, 1, 4), label="SealedBay_North")
    spawn_cube((7700, 400, 200), (15, 1, 4), label="SealedBay_South")

    # ---- furnace-pour hazards on F1/F2 (F3 belongs to the boss), staggered:
    # F1 telegraphs ~20s into a session, F2 ~65s ----
    unreal.log_warning("MILL: hazards")
    for (x, y, offset, name) in [(-4200, 3300, 70.0, "F1"), (4200, 3300, 25.0, "F2")]:
        h = spawn(unreal.BLHazard_FurnacePour, (x, y, 150), label=f"Pour_{name}")
        h.set_editor_property("start_offset_seconds", offset)

    # ---- the catwalk (destructible #2 - the collapse prototype) ----
    unreal.log_warning("MILL: catwalk")
    catwalk = spawn(unreal.BLDestructible_Catwalk, (0, 800, 0), label="Catwalk")

    # ---- navigation bounds (runtime navmesh covers the whole hall) ----
    unreal.log_warning("MILL: navbounds")
    nav = spawn(unreal.BLNavBounds, (0, 0, 500), label="NavBounds")
    nav.set_editor_property("bounds_extent", unreal.Vector(9000, 7000, 1600))

    # ---- pickups (blockout P points, subset) ----
    unreal.log_warning("MILL: pickups")
    for i, (x, y) in enumerate([(-4200, 1500), (4200, 1500), (-1800, -1600),
                                (1800, -1600), (-6200, 0), (6200, -2600),
                                (0, 4800), (0, -5400)]):
        spawn(unreal.BLPickupActor, (x, y, 120), label=f"Pickup_{i}")

    # ---- lighting: dim interior, furnace glow does the storytelling ----
    unreal.log_warning("MILL: lights")
    sun = spawn(unreal.DirectionalLight, (0, 0, 1200), (0, -55, 20), "Sun")
    sun.light_component.set_editor_property("intensity", 2.0)
    sun.light_component.set_editor_property("light_color", unreal.Color(255, 234, 204, 255))
    sky = spawn(unreal.SkyLight, (0, 0, 1200), label="SkyLight")
    sky.light_component.set_editor_property("intensity", 0.45)
    spawn(unreal.SkyAtmosphere, (0, 0, 0), label="Atmosphere")
    fog = spawn(unreal.ExponentialHeightFog, (0, 0, 0), label="Fog")
    fog.component.set_editor_property("fog_density", 0.06)

    # ---- player start: open floor south of the catwalk (GameMode rings AI here) ----
    unreal.log_warning("MILL: playerstart")
    spawn(unreal.PlayerStart, (0, -1500, 250), (0, 0, 90), "PlayerStart")

    n = len(actors.get_all_level_actors())
    if n < 25:
        raise RuntimeError(f"only {n} actors in level - something failed")
    if not les.save_current_level():
        raise RuntimeError("save_current_level failed")
    if eal.does_asset_exist(SCRATCH):
        eal.delete_asset(SCRATCH)
    unreal.log_warning(f"MILL MAP OK: {MAP_PATH} ({n} actors)")


try:
    main()
except Exception:
    unreal.log_error("MILL MAP FAILED")
    unreal.log_error(traceback.format_exc())
finally:
    unreal.SystemLibrary.quit_editor()
