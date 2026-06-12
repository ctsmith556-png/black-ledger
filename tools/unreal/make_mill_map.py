# Black Ledger - build The Mill greybox at ARENA BUILD BRIEF scale and save as
# /Game/BlackLedger/Maps/L_Mill.
#
# Sources of truth (docs/arenas/ARENA_INDEX.md):
#   - 01_The_Mill_Arena_Build_Brief.docx : SCALE + systems. 260 m x 220 m playable
#     (26,000 x 22,000 UU), catwalk lane 12 m, 12 weapon + 3 health pickups,
#     14 FFA spawn nodes in the start zones, 30-50 m non-driveable backdrop apron.
#   - TheMill_Blockout.png : landmark placement (furnaces N pair + S boss pit,
#     catwalk mid-span, ladle center, loading bays SW/SE, sealed bay E).
#
# REQUIRES the C++ module built first. Run with the EDITOR CLOSED - and WITHOUT
# -nullrhi (UE 5.7.4 crashes on scripted actor spawns under nullrhi):
#   & "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" `
#     "C:\Users\csmit\black-ledger\BlackLedger.uproject" `
#     -ExecCmds="py C:/Users/csmit/black-ledger/tools/unreal/make_mill_map.py" `
#     -stdout -unattended -nosplash -nosound -nopause

import math
import unreal, traceback

MAP_PATH = "/Game/BlackLedger/Maps/L_Mill"
SCRATCH = "/Game/BlackLedger/Maps/L_Scratch"

# ---- scale knob: 1.0 = the brief's 260 m x 220 m. Scale-ladder testing toward
# larger arenas (Chris, June 12): 2.0 = 520x440. Drive each rung before climbing. ----
SCALE = 2.0
HW = 13000 * SCALE   # half-width  (X, cm)
HH = 11000 * SCALE   # half-height (Y, cm)

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
    # raised pit lip; modest ember glow (must NOT compete with the pour warning)
    spawn_shape("Cylinder", (x, y, 75), (18, 18, 1.5), label=f"Furnace_{name}")
    glow = spawn(unreal.PointLight, (x, y, 900), label=f"FurnaceGlow_{name}")
    lc = glow.light_component
    lc.set_editor_property("intensity", 8000.0)
    lc.set_editor_property("attenuation_radius", 3000.0)
    lc.set_editor_property("light_color", unreal.Color(255, 96, 18, 255))
    lc.set_editor_property("cast_shadows", False)


def spawn_node(x, y, label):
    # FFA spawn node (brief: start zones, no spawn-kill sightlines), facing arena center
    yaw = math.degrees(math.atan2(-y, -x))
    n = spawn(unreal.TargetPoint, (x, y, 150), (0, 0, yaw), label)
    n.set_editor_property("tags", ["BLSpawn"])
    return n


def main():
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

    # ---- shell: 260 x 220 m playable, 8 m walls, 40 m backdrop apron beyond ----
    unreal.log_warning("MILL: shell")
    spawn_cube((0, 0, -50), (340 * SCALE, 300 * SCALE, 1), label="Floor")  # apron included
    spawn_cube((0, HH + 50, 400), (260 * SCALE, 1, 8), label="Wall_N")
    spawn_cube((0, -HH - 50, 400), (260 * SCALE, 1, 8), label="Wall_S")
    spawn_cube((HW + 50, 0, 400), (1, 222 * SCALE, 8), label="Wall_E")
    spawn_cube((-HW - 50, 0, 400), (1, 222 * SCALE, 8), label="Wall_W")

    # ---- furnaces (blockout: F1 NW / F2 NE / F3 S-center = boss rise) ----
    unreal.log_warning("MILL: furnaces")
    F1 = (-0.49 * HW, 0.51 * HH)
    F2 = (0.49 * HW, 0.51 * HH)
    F3 = (0.0, -0.51 * HH)
    furnace(F1[0], F1[1], "F1")
    furnace(F2[0], F2[1], "F2")
    furnace(F3[0], F3[1], "F3")

    # ---- loading bays (SW + SE corners; brief: bay doors 18-22 m) ----
    unreal.log_warning("MILL: bays")
    spawn_cube((-0.76 * HW, -0.80 * HH, 25), (34, 24, 0.5), label="LoadingBay_SW")
    spawn_cube((0.76 * HW, -0.80 * HH, 25), (34, 24, 0.5), label="LoadingBay_SE")

    # ---- sealed bay (E wall pocket near F2; opened by "Blow the Furnace" later) ----
    spawn_cube((HW - 1050, 5600, 200), (1, 20, 4), label="SealedBay_Wall")
    spawn_cube((HW - 525, 6600, 200), (10.5, 1, 4), label="SealedBay_North")
    spawn_cube((HW - 525, 4600, 200), (10.5, 1, 4), label="SealedBay_South")

    # ---- furnace-pour hazards on all three furnaces, staggered
    # (F1 ~20s, F2 ~65s, F3 ~90s). The Foundryman commandeers F3 when he lands. ----
    unreal.log_warning("MILL: hazards")
    for ((x, y), offset, name) in [(F1, 70.0, "F1"), (F2, 25.0, "F2"), (F3, 0.0, "F3")]:
        h = spawn(unreal.BLHazard_FurnacePour, (x, y, 150), label=f"Pour_{name}")
        h.set_editor_property("start_offset_seconds", offset)
        h.set_editor_property("zone_radius", 1800.0)

    # ---- the catwalk (E-W mid-span shortcut; collapse = destructible #2) ----
    unreal.log_warning("MILL: catwalk")
    spawn(unreal.BLDestructible_Catwalk, (0, 1000, 0), label="Catwalk")
    # suspended slag ladle landmark over the mid-span (destructible #1 later);
    # deck top is 8.5 m, ladle hangs clear of deck traffic
    spawn_cube((0, 1000, 1500), (3.5, 3.5, 3), label="Ladle_Placeholder")

    # ---- navigation bounds ----
    unreal.log_warning("MILL: navbounds")
    nav = spawn(unreal.BLNavBounds, (0, 0, 500), label="NavBounds")
    nav.set_editor_property("bounds_extent", unreal.Vector(HW + 500, HH + 500, 1600))

    # ---- pickups: 12 weapon + 3 health (brief: High(12); health requires risk;
    # offensive pickups pull vehicles through conflict lanes) ----
    unreal.log_warning("MILL: pickups")
    homing_pts = [(-6400, 2600), (6400, 2600), (-2800, -2600), (2800, -2600),
                  (0, 7600), (0, -8600), (-9800, 0), (9800, -4000)]
    for i, (x, y) in enumerate(homing_pts):
        spawn(unreal.BLPickupActor, (x, y, 120), label=f"Pickup_Homing_{i}")
    power_cls = unreal.BLProjectile_Power.static_class()
    power_pts = [(0, 1000, 970),       # ON the catwalk deck (top z=850) - speed-skill reward
                 (-9800, -4000, 120), (9800, 1000, 120), (0, -2600, 120)]
    for i, (x, y, z) in enumerate(power_pts):
        p = spawn(unreal.BLPickupActor, (x, y, z), label=f"Pickup_Power_{i}")
        p.set_editor_property("projectile_class", power_cls)
        p.set_editor_property("ammo", 2)
        p.set_editor_property("weapon_name", "Power Missile")
    # health near the pour rings / boss pit approach = risk to collect
    for i, (x, y) in enumerate([(F1[0], F1[1] + 2600), (F2[0], 2400), (0, -0.51 * HH - 2600)]):
        spawn(unreal.BLPickup_Health, (x, y, 120), label=f"Pickup_Health_{i}")

    # ---- 14 FFA spawn nodes in the four start zones (brief) ----
    unreal.log_warning("MILL: spawns")
    sw = [(-10800, -9400), (-9000, -9400), (-9900, -7400), (-11400, -7400)]
    se = [(9000, -9400), (10800, -9400), (9900, -7400)]
    nw = [(-10500, 8800), (-8700, 9300), (-11300, 7000), (-9300, 7200)]
    ne = [(10500, 8800), (8700, 9300), (9500, 7000)]
    for i, (x, y) in enumerate(sw + se + nw + ne):
        spawn_node(x, y, f"Spawn_{i:02d}")

    # ---- lighting: dim warm interior; furnace glow does the storytelling ----
    unreal.log_warning("MILL: lights")
    sun = spawn(unreal.DirectionalLight, (0, 0, 1200), (0, -55, 20), "Sun")
    sun.light_component.set_editor_property("intensity", 2.0)
    sun.light_component.set_editor_property("light_color", unreal.Color(255, 234, 204, 255))
    sky = spawn(unreal.SkyLight, (0, 0, 1200), label="SkyLight")
    sky.light_component.set_editor_property("intensity", 0.45)
    spawn(unreal.SkyAtmosphere, (0, 0, 0), label="Atmosphere")
    fog = spawn(unreal.ExponentialHeightFog, (0, 0, 0), label="Fog")
    fog.component.set_editor_property("fog_density", 0.06)

    # ---- player start: SW loading bay (a blockout start zone), facing center ----
    unreal.log_warning("MILL: playerstart")
    px, py = -0.76 * HW, -0.80 * HH
    spawn(unreal.PlayerStart, (px, py, 250), (0, 0, math.degrees(math.atan2(-py, -px))), "PlayerStart")

    n = len(actors.get_all_level_actors())
    if n < 50:
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
