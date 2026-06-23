# Black Ledger - build The Mill graybox (M1) and save as /Game/BlackLedger/Maps/L_Mill.
#
# Layout follows Black_Ledger_The_Mill_Unreal_Map_Build_Plan.docx: THREE combat loops -
# a central kill floor with cover islands, an outer loading-bay recovery ring, and the
# elevated catwalk route - at 520 x 440 m (2x doctrine; route widths fit the vehicles and
# do NOT scale). Three furnace pits with DISTINCT pour shapes (A fan / B river / C circular,
# plan section 8). 14 outer-ring spawns, 12 pickups (center offensive + ring health + sealed
# bay reward). Reuses the C++ gameplay actors (furnace-pour hazard, catwalk collapse,
# pickups, nav bounds). Graybox first: DRESS=False skips the Fab mesh art pass (M5).
#
# Sources of truth: the Build Plan above + docs/arenas/ARENA_INDEX.md.
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
HW = 13000 * SCALE   # half-width  (X, cm) -> 520 m full (2x doctrine)
HH = 11000 * SCALE   # half-height (Y, cm) -> 440 m full

# Graybox-first (Mill Build Plan M1): build the playable layout + lighting WITHOUT the
# Fab/Megascans mesh dressing. Flip to True for the art pass (M5) once the layout drives
# well - the dressing code (dress_with_meshes) re-fits to the furnace/catwalk positions.
DRESS = True

# ---- STRUCTURAL MESHES (real Fab / Megascans / AbandonedPowerPlant kits) ----
# Decorative overlays (NO collision) on top of the gameplay greybox - the floor /
# walls / hazards / catwalk collision are untouched. Placement SELF-CALIBRATES:
# each mesh's bounds are measured at runtime, so a role either fits to a target
# real-world size (`target_h`, cm) or uses an explicit `scale`, and seated props
# rest on the floor automatically. Every measured size is logged - read the log
# after the first run, then nudge the knobs below. Empty path = skip that role.
P_CAT = "/Game/LN3D_Modular_CatWalk/LN3D_Modular_CatWalk_Free/Models/"
P_FAB = "/Game/Fab/"
P_APP = "/Game/AbandonedPowerPlant/Assets/"

MESHES = {
    # --- mid-span catwalk: tiled to match the C++ BLDestructible_Catwalk (90 m deck
    #     along X, 12 m lane, deck top z=850, centred at world y=1000). ---
    "catwalk": {
        "path": "",   # DISABLED - was the old straight-catwalk grate dressing (orphaned by the
                      # square-loop rehaul; it tiled at y=1000 next to the furnaces). The loop
                      # roads are the catwalk now.
        "scale": 1.0, "yaw": 90.0,        # piece length on its local Y -> yaw 90 runs E-W
        "deck_z": 858.0,                  # grate sits ~8 cm above the 850 collision deck (Chris's in-editor value)
        "z_adjust": 0.0,                  # nudge up/down after seeing it
        "center_y": 1000.0,               # the catwalk actor lives at world y=1000
        "span": 13000.0,                  # cover deck + gantries (x -6500..+6500)
        "lane_width": 1200.0,             # = the C++ 12 m drivable deck; each tile is
                                          #   stretched across it so railings sit only on
                                          #   the two OUTER edges (clear vehicle lane).
        "overlap": 0.98,                  # tiles butt with a hair of overlap (no gaps)
    },
    "stairs":  {"path": "",   # REMOVED - vehicles can't drive stairs; the C++ entry ramps are the way up
                "yaw": 0.0, "end_x": 6600.0, "side_off": 1500.0},
    "support": {"path": "",   # DISABLED with the old straight catwalk (orphaned legs at y=1000)
                "yaw": 0.0, "xs": [-5500.0, -2200.0, 2200.0, 5500.0],
                "edge": 0.0},
    # --- furnace structures: DISABLED - the Sketchfab boilers (placed in main()) now fill
    #     this role centred on the pits. Re-enable (restore the path) for extra machinery. ---
    "furnace": {"path": "",
                "target_h": 900.0, "yaw": 0.0, "outward": 2800.0},
    # --- scatter props (each: path + target_h or scale, seat=rest on floor, pts) ---
    "scrap":  {"path": P_FAB + "Rusty_steel_scrap_pile/rusty_steel_scrap_pile/StaticMeshes/rusty_steel_scrap_pile",
               "target_h": 420.0, "seat": True,   # fewer, bigger, placed deliberately near the bays/walls
               "pts": [(-18000, -10000), (18000, -10000), (0, 9600)]},
    "barrel": {"path": P_FAB + "Industrial_Steel_Barrel/industrialsteeldrum/StaticMeshes/industrialsteeldrum",
               "target_h": 90.0, "seat": True,   # ~55-gal drum clusters by the loading bays
               "pts": [(-19000, -16800), (-19700, -16400), (-18500, -16100),
                       (19000, -16800), (19700, -16400), (18500, -16100),
                       (-700, 3200), (900, 3500)]},
    "tank":   {"path": P_FAB + "Megascans/3D/Industrial_Junkyard_Propane_Tank_Metal_xf3pbjy/Raw/xf3pbjy_tier_0/StaticMeshes/xf3pbjy_tier_0",
               "scale": 1.0, "seat": True,        # Megascans = real-world cm scale
               "pts": [(HW - 2400, 7000), (HW - 2400, 5000), (-HW + 2400, -3000), (-HW + 2400, -1000)]},
    # girders: DISABLED - overhead beams floated with no ceiling truss to hold them (read as
    # junk in mid-air; Chris deleted the worst). Re-enable only with a real roof structure.
    "girder": {"path": "",
               "scale": 1.0, "seat": False, "yaw": 90.0,
               "pts": [(-8000, 0, 2100), (0, 0, 2100), (8000, 0, 2100),
                       (-4000, 6000, 2100), (4000, -6000, 2100)]},
    # rubble: DISABLED - placed/sized by hand in the editor (the auto-bounds kept floating
    # it). Empty path = the script skips it, so it won't fight your manual placement.
    "rubble": {"path": "",
               "target_h": 1100.0, "seat": True, "sink_frac": 0.5,
               "pts": [(-19000, -14500), (19000, -14500), (-21500, 3000), (21500, -3000)]},
    # --- caged industrial wall lamps on the perimeter (the 'vent pipe' slot, repurposed).
    #     Each gets a dim warm point light so the fixtures actually read. ---
    "walllight": {"path": P_FAB + "Megascans/3D/Rusty_Bulkhead_Light_vgyidfpaw/Raw/vgyidfpaw_tier_0/StaticMeshes/vgyidfpaw_tier_0",
                  "scale": 1.0, "seat": False, "z": 600.0, "inset": 350.0, "per_wall": 4,
                  "lamp_light": False},   # fixtures only - the ABLIndustrialFlickerLight lamps already light the walls
}

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
    # raised pit lip; FIERY glow that spreads across the floor - this (not the sun) is
    # the dominant light, so the dark foundry stays playable + orange-lit. A bright core
    # at the pit + a wide soft wash above it.
    spawn_shape("Cylinder", (x, y, 75), (18, 18, 1.5), label=f"Furnace_{name}")
    core = spawn(unreal.PointLight, (x, y, 300), label=f"FurnaceCore_{name}")
    cc = core.light_component
    cc.set_editor_property("intensity", 95000.0)
    cc.set_editor_property("attenuation_radius", 11000.0)        # spills onto the lanes
    cc.set_editor_property("light_color", unreal.Color(255, 105, 24, 255))
    cc.set_editor_property("cast_shadows", False)
    wash = spawn(unreal.PointLight, (x, y, 1400), label=f"FurnaceWash_{name}")
    wc = wash.light_component
    wc.set_editor_property("intensity", 55000.0)
    wc.set_editor_property("attenuation_radius", 16000.0)        # broad ambient fill
    wc.set_editor_property("light_color", unreal.Color(255, 126, 46, 255))
    wc.set_editor_property("cast_shadows", False)


def spawn_node(x, y, label):
    # FFA spawn node (brief: start zones, no spawn-kill sightlines), facing arena center
    yaw = math.degrees(math.atan2(-y, -x))
    n = spawn(unreal.TargetPoint, (x, y, 150), (0, 0, yaw), label)
    n.set_editor_property("tags", ["BLSpawn"])
    return n


def load_mesh(pkg):
    # accepts a package path ("/Game/.../Name"); tries the dotted ObjectPath too.
    if not pkg:
        return None
    for p in (pkg, f"{pkg}.{pkg.rsplit('/', 1)[-1]}"):
        try:
            a = eal.load_asset(p)
        except Exception:
            a = None
        if a:
            return a
    unreal.log_warning(f"MILL: mesh NOT FOUND: {pkg}")
    return None


def mesh_size(mesh):
    # returns (min, max, size) Vectors in the mesh's local space (cm), robust across
    # UE API shapes (get_bounding_box -> Box, else get_bounds -> BoxSphereBounds).
    mn = mx = None
    try:
        box = mesh.get_bounding_box()
        mn, mx = box.min, box.max
    except Exception:
        try:
            b = mesh.get_bounds()
            o, e = b.origin, b.box_extent
            mn = unreal.Vector(o.x - e.x, o.y - e.y, o.z - e.z)
            mx = unreal.Vector(o.x + e.x, o.y + e.y, o.z + e.z)
        except Exception as e:
            unreal.log_warning(f"MILL: bounds read failed ({e}); assuming 100cm cube")
            mn, mx = unreal.Vector(-50, -50, -50), unreal.Vector(50, 50, 50)
    return mn, mx, unreal.Vector(mx.x - mn.x, mx.y - mn.y, mx.z - mn.z)


def place_static(mesh, x, y, yaw, label, *, z=0.0, ground_z=None, target_h=None,
                 scale=None, scale3d=None, z_adjust=0.0, collide=False, sink_frac=0.0):
    # Drops a mesh. If `ground_z` is given the mesh is SEATED (its measured bottom rests
    # on that floor height); otherwise it's placed at `z`. Scale resolves from
    # scale3d > scale > fit-to-target_h > 1.0. collide=True keeps the mesh's collision
    # (big landmark structures); default is a no-collision decorative overlay.
    # sink_frac pushes the mesh DOWN by that fraction of its rendered height - a
    # scale-independent way to ground meshes whose bounds don't match their visual bottom
    # (so they don't float when scaled up).
    mn, mx, sz = mesh_size(mesh)
    if scale3d is not None:
        sv = unreal.Vector(*scale3d)
    else:
        s = scale if scale is not None else (target_h / sz.z if (target_h and sz.z) else 1.0)
        sv = unreal.Vector(s, s, s)
    if ground_z is not None:
        z = ground_z - mn.z * sv.z + z_adjust
    z -= sink_frac * sz.z * sv.z
    a = spawn(unreal.StaticMeshActor, (x, y, z), (0, 0, yaw), label)
    smc = a.static_mesh_component
    smc.set_editor_property("static_mesh", mesh)
    smc.set_editor_property("mobility", unreal.ComponentMobility.STATIC)
    smc.set_collision_enabled(unreal.CollisionEnabled.QUERY_AND_PHYSICS
                              if collide else unreal.CollisionEnabled.NO_COLLISION)
    a.set_actor_scale3d(sv)
    return a


def _scatter(role, default_yaw=0.0):
    # generic floor/overhead scatter for a MESHES role with a `pts` list.
    c = MESHES.get(role, {})
    if not c.get("path"):
        return 0
    base = load_mesh(c["path"])
    if not base:
        return 0
    pieces = [load_mesh(c["path"].rsplit("/", 1)[0] + "/" + p) for p in c["pieces"]] \
        if c.get("pieces") else [base]
    pieces = [m for m in pieces if m] or [base]
    n = 0
    for i, pt in enumerate(c["pts"]):
        x, y = pt[0], pt[1]
        m = pieces[i % len(pieces)]
        yaw = (i * 67) % 360 + c.get("yaw", default_yaw)
        if c.get("seat"):
            place_static(m, x, y, yaw, f"Mesh_{role}_{i}", ground_z=0.0,
                         target_h=c.get("target_h"), scale=c.get("scale"),
                         sink_frac=c.get("sink_frac", 0.0))
        else:
            z = pt[2] if len(pt) > 2 else c.get("z", 0.0)
            place_static(m, x, y, yaw, f"Mesh_{role}_{i}", z=z,
                         target_h=c.get("target_h"), scale=c.get("scale"))
        n += 1
    return n


def dress_with_meshes(F1, F2, F3):
    # places every MESHES role whose mesh loads; skips the rest. Decorative only -
    # the gameplay greybox (floor/walls/hazards/catwalk-collision) is untouched.
    placed = 0
    # log measured sizes up front so the knobs can be tuned from the first run's log
    for role, c in MESHES.items():
        m = load_mesh(c.get("path", ""))
        if m:
            mn, mx, sz = mesh_size(m)
            unreal.log_warning(f"MILL mesh[{role}]: {sz.x:.0f} x {sz.y:.0f} x {sz.z:.0f} cm  (min.z {mn.z:.0f})")

    # --- furnace machines: behind each pit, toward the nearest wall ---
    fm = load_mesh(MESHES["furnace"]["path"])
    if fm:
        c = MESHES["furnace"]
        for (x, y), name in [(F1, "F1"), (F2, "F2"), (F3, "F3")]:
            oy = c["outward"] if y > 0 else -c["outward"]            # push toward the near wall
            yaw = math.degrees(math.atan2(-y, -x)) + c["yaw"]        # face arena center
            place_static(fm, x, y + oy, yaw, f"Mesh_Furnace_{name}",
                         ground_z=0.0, target_h=c["target_h"]); placed += 1

    # --- catwalk: tile the grate along world X at the gameplay deck height ---
    cm = load_mesh(MESHES["catwalk"]["path"])
    if cm:
        c = MESHES["catwalk"]
        mn, mx, sz = mesh_size(cm)
        s = c["scale"]
        # length axis = the longer horizontal dim (tile along world X); width axis = the
        # shorter one, STRETCHED to the 12 m lane so railings end up on the outer edges only.
        length_dim = max(sz.x, sz.y)
        width_dim = min(sz.x, sz.y)
        seg = length_dim * s * c.get("overlap", 1.0)               # step = piece length
        wfac = (c["lane_width"] / width_dim) if width_dim else 1.0  # widen to fill the lane
        # build local scale3d so the WIDTH axis gets wfac, others get base s
        if sz.x <= sz.y:                                            # local X is the width axis
            cat_s3d = (wfac, s, s)
        else:                                                      # local Y is the width axis
            cat_s3d = (s, wfac, s)
        cols = max(1, int(math.ceil(c["span"] / seg)))
        x0 = -(cols - 1) * seg / 2.0
        zc = c["deck_z"] - mn.z * s + c.get("z_adjust", 0.0)        # seat grate at deck top
        for i in range(cols):
            place_static(cm, x0 + i * seg, c["center_y"], c["yaw"],
                         f"Mesh_Catwalk_{i}", z=zc, scale3d=cat_s3d); placed += 1
        unreal.log_warning(
            f"MILL: catwalk {cols} segs (seg {seg:.0f} cm, lane {c['lane_width']:.0f} cm "
            f"= {wfac:.1f}x width, deck z {zc:.0f})")

        # supports (legs) under the deck - stretched on Z to reach the deck, at both edges
        pm = load_mesh(MESHES["support"]["path"])
        if pm:
            sc = MESHES["support"]
            _, _, psz = mesh_size(pm)
            zfac = c["deck_z"] / psz.z if psz.z else 1.0
            edge = sc.get("edge", 0.0)
            ys = [c["center_y"] - edge, c["center_y"] + edge] if edge else [c["center_y"]]
            k = 0
            for sx in sc["xs"]:
                for sy in ys:
                    place_static(pm, sx, sy, sc["yaw"], f"Mesh_Support_{k}",
                                 ground_z=0.0, scale3d=(1.0, 1.0, zfac)); placed += 1; k += 1

        # stairs at each end - uniform-scaled to roughly reach the deck (decorative)
        sm = load_mesh(MESHES["stairs"]["path"])
        if sm:
            sc = MESHES["stairs"]
            _, _, ssz = mesh_size(sm)
            sscale = c["deck_z"] / ssz.z if ssz.z else 1.0
            for sx, syaw, tag in [(-sc["end_x"], 90.0, "W"), (sc["end_x"], -90.0, "E")]:
                place_static(sm, sx, c["center_y"] - sc["side_off"], syaw + sc["yaw"],
                             f"Mesh_Stairs_{tag}", ground_z=0.0, scale=sscale); placed += 1

    # --- scatter props ---
    for role in ("scrap", "barrel", "tank", "girder", "rubble"):
        placed += _scatter(role)

    # --- caged wall lamps around the perimeter (+ a dim warm light each) ---
    wl = MESHES["walllight"]
    wm = load_mesh(wl["path"])
    if wm:
        z = wl["z"]; inset = wl["inset"]; per = int(wl.get("per_wall", 4))
        def span_pts(a, b, k):
            return [a + (b - a) * (j + 0.5) / k for j in range(k)]
        walls = []
        for fy in (HH - inset, -HH + inset):     # N / S walls -> vary X, face +/-Y
            for x in span_pts(-HW + 4000, HW - 4000, per):
                walls.append((x, fy, 0.0 if fy < 0 else 180.0))
        for fx in (HW - inset, -HW + inset):      # E / W walls -> vary Y, face +/-X
            for y in span_pts(-HH + 4000, HH - 4000, per):
                walls.append((fx, y, 180.0 if fx > 0 else 0.0))
        for i, (x, y, yaw) in enumerate(walls):
            place_static(wm, x, y, yaw, f"Mesh_WallLight_{i}", z=z, scale=wl["scale"]); placed += 1
            if wl.get("lamp_light"):
                lt = spawn(unreal.PointLight, (x, y, z), label=f"WallLamp_{i}")
                lc = lt.light_component
                lc.set_editor_property("intensity", 2600.0)
                lc.set_editor_property("attenuation_radius", 2600.0)
                lc.set_editor_property("light_color", unreal.Color(255, 170, 90, 255))
                lc.set_editor_property("cast_shadows", False)
                placed += 1

    unreal.log_warning(f"MILL: placed {placed} structural meshes/lamps")


# ---- gameplay cover (graybox, WITH collision) - builds the readable combat loops:
# columns + rail-car blockers break line-of-sight and create lanes without walling
# off space (Mill plan section 3 / table 4). Sizes from the plan's mesh kit. ----
def cover_column(x, y, name):
    # SM_Mill_Column 4x4x18 m - tall structural cover (cube = 100 cm)
    return spawn_cube((x, y, 900), (4, 4, 18), label=f"Cover_Column_{name}")


def cover_railcar(x, y, yaw, name):
    # SM_Mill_RailCar_Blocker 20x4x5 m - outer-ring LOS blocker / cover island
    return spawn_cube((x, y, 250), (20, 4, 5), (0, 0, yaw), label=f"Cover_RailCar_{name}")


def cover_scrap(x, y, name):
    # SM_Mill_ScrapPile - low cover, doesn't block sightlines fully
    return spawn_cube((x, y, 140), (7, 6, 2.8), (0, 0, (hash(name) % 90)), label=f"Cover_Scrap_{name}")


# ---- DRIVABLE SQUARE CATWALK LOOP (elevated ring road on columns, per the reference image:
# a square frame around the central pit). Greybox WITH collision; the deck top is z=850. ----
def loop_road(cx, cy, sx, sy, name):
    # a raised drivable road segment (top at z=850)
    return spawn_cube((cx, cy, 825), (sx, sy, 0.5), label=f"Loop_{name}")


def loop_support(x, y, name):
    # support column from the floor up to just under the road (~8.2 m)
    return spawn_cube((x, y, 410), (3, 3, 8.2), label=f"LoopCol_{name}")


def loop_ramp(cx, cz, pitch, name):
    # gentle 9 deg drivable ramp at y=0, flush-top onto an E/W road edge (proven catwalk geometry)
    return spawn_cube((cx, 0, cz), (55, 12, 0.5), (0, pitch, 0), label=f"LoopRamp_{name}")


def place_decals(FA, FB, FC):
    # M6 atmosphere: Megascans decals (Fab Standard License) projected onto the floor + walls.
    # Floor decals project DOWN (pitch -90); wall decals project along +X, rotated by yaw to
    # face the wall (E=0, N=90, W=180, S=270). DecalSize = (projection depth, halfY, halfZ).
    P = "/Game/Fab/Megascans/Decals/"

    def M(folder, mid):
        return f"{P}{folder}/Medium/{mid}_tier_2/Materials/MI_{mid}"

    RUST_PILE = M("Metal_Rust_Pile_slxobqd", "slxobqd")
    RUST_PLATE = M("Rusted_Metal_Plate_smsqo0n", "smsqo0n")
    RUST_PANEL = M("Rusty_Metal_Panel_vhqnfe0", "vhqnfe0")
    BLOOD = M("Blood_Stain_sgfjdepc", "sgfjdepc")
    SPATTER = M("High_Velocity_Blood_Spatter_sgeoahup", "sgeoahup")
    GRAFF1 = M("Graffiti_vlrkdiyc", "vlrkdiyc")
    GRAFF2 = M("Graffiti_Tag_vcqhadvl", "vcqhadvl")
    DEBRIS = M("Road_Debris_Pile_uceheb2ka", "uceheb2ka")

    # (material, kind, x, y, z, yaw, half-size)
    decals = [
        # rust/slag staining on the floor around each furnace pit
        (RUST_PILE, "floor", FA[0] + 2600, FA[1] - 1800, 12, 20, 360),
        (RUST_PLATE, "floor", FB[0] - 2600, FB[1] - 1800, 12, 70, 360),
        (RUST_PILE, "floor", FC[0] + 2600, FC[1] + 1700, 12, 130, 360),
        (RUST_PLATE, "floor", FC[0] - 2400, FC[1] + 1400, 12, 200, 320),
        # road / scrap debris scatter on the central floor
        (DEBRIS, "floor", -5000, 4000, 12, 15, 420),
        (DEBRIS, "floor", 5500, 5200, 12, 95, 420),
        (DEBRIS, "floor", 1500, -6000, 12, 220, 400),
        # blood (the horror tone) through the kill-floor lanes
        (BLOOD, "floor", 3000, 1500, 12, 40, 280),
        (BLOOD, "floor", -3500, -2500, 12, 160, 260),
        (SPATTER, "floor", 7000, -1000, 12, 300, 300),
        (SPATTER, "floor", -7500, 2000, 12, 250, 300),
        # rust panels on the perimeter walls (E=0 / W=180 / N=90 / S=270)
        (RUST_PANEL, "wall", HW - 150, 6000, 430, 0, 300),
        (RUST_PLATE, "wall", HW - 150, -7000, 430, 0, 300),
        (RUST_PLATE, "wall", -HW + 150, 5000, 430, 180, 300),
        (RUST_PANEL, "wall", -HW + 150, -6000, 430, 180, 300),
        (RUST_PLATE, "wall", -4000, HH - 150, 430, 90, 300),
        (RUST_PANEL, "wall", 9000, HH - 150, 430, 90, 300),
        (RUST_PANEL, "wall", -9000, -HH + 150, 430, 270, 300),
        (RUST_PLATE, "wall", 4000, -HH + 150, 430, 270, 300),
        # graffiti by the loading bays
        (GRAFF1, "wall", -HW + 150, 1200, 430, 180, 260),
        (GRAFF2, "wall", HW - 150, 1600, 430, 0, 260),
    ]

    n = 0
    for i, (mat, kind, x, y, z, yaw, half) in enumerate(decals):
        mi = eal.load_asset(mat)
        if not mi:
            unreal.log_warning(f"MILL: decal material missing: {mat}")
            continue
        rot = (0.0, -90.0, float(yaw)) if kind == "floor" else (0.0, 0.0, float(yaw))
        a = spawn(unreal.DecalActor, (x, y, z), rot, label=f"Decal_{kind}_{i}")
        dc = a.get_editor_property("decal")
        dc.set_editor_property("decal_material", mi)
        # X = projection DEPTH (half). Floor decals get a THIN slab so they only hit the
        # floor, not the vehicle driving over them; walls can project deep into the wall.
        depth = 45.0 if kind == "floor" else 300.0
        dc.set_editor_property("decal_size", unreal.Vector(depth, float(half), float(half)))
        dc.set_editor_property("sort_order", 1)
        n += 1
    unreal.log_warning(f"MILL: placed {n} decals")


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

    # ============================================================================
    # THE MILL - reference-image layout (June 2026): a square drivable CATWALK LOOP
    # framing the central pit, THREE furnaces in a row across the centre with the ladle
    # over the middle one, an outer loading-bay ring, and a collapsible cross-bridge.
    # 520x440 m. Reuses the C++ gameplay actors.
    # ============================================================================

    # ---- shell: floor FRAME around a sunken SLAG TRENCH (E-W escape/ambush channel, plan
    # section 3) + 8 m perimeter walls. The floor is built as 4 plates around the trench
    # opening so vehicles can drop into the channel via the end ramps. ----
    unreal.log_warning("MILL: shell + slag trench")
    TY, THW, TDEEP = -7000.0, 1000.0, 400.0   # trench centre-y, half-width (20 m), depth (4 m)
    TX = 16000.0                               # channel half-length in x (ramps at the ends)
    TWY = THW * 2 / 100.0                       # channel width as a cube scale
    # floor frame (leaves the channel open). Everything labelled Floor* gets the floor material.
    spawn_cube((0, (TY + THW + 30000) / 2.0, -50), (680, (30000 - (TY + THW)) / 100.0, 1), label="Floor_N")
    spawn_cube((0, (TY - THW - 30000) / 2.0, -50), (680, (30000 + (TY - THW)) / 100.0, 1), label="Floor_S")
    spawn_cube((-(TX + 34000) / 2.0, TY, -50), ((34000 - TX) / 100.0, TWY, 1), label="Floor_W")
    spawn_cube(((TX + 34000) / 2.0, TY, -50), ((34000 - TX) / 100.0, TWY, 1), label="Floor_E")
    # trench: flat bottom (x -13000..13000, top at z=-400), N/S retaining walls, end ramps
    spawn_cube((0, TY, -TDEEP - 50), (260, TWY, 1), label="Floor_Trench")
    spawn_cube((0, TY + THW, -TDEEP / 2.0), (TX * 2 / 100.0 + 6, 0.2, TDEEP / 100.0), label="TrenchWall_N")
    spawn_cube((0, TY - THW, -TDEEP / 2.0), (TX * 2 / 100.0 + 6, 0.2, TDEEP / 100.0), label="TrenchWall_S")
    # THICK (1.5 m, so a fast car can't tunnel through), gentle ~6.6 deg ramps: top crests
    # ~18 cm ABOVE the floor (a roll-down onto it, never an up-step wall) and the bottom
    # overlaps the trench floor (z=-400) - flush both ends.
    spawn_cube((-14300, TY, -265), (36, TWY, 1.5), (0, -6.65, 0), label="Floor_TrenchRamp_W")
    spawn_cube((14300, TY, -265), (36, TWY, 1.5), (0, 6.65, 0), label="Floor_TrenchRamp_E")
    spawn_cube((0, HH + 50, 400), (270 * SCALE, 1, 8), label="Wall_N")
    spawn_cube((0, -HH - 50, 400), (270 * SCALE, 1, 8), label="Wall_S")
    spawn_cube((HW + 50, 0, 400), (1, 222 * SCALE, 8), label="Wall_E")
    spawn_cube((-HW - 50, 0, 400), (1, 222 * SCALE, 8), label="Wall_W")

    # ---- THREE furnaces in a ROW across the centre (reference image): A left / B centre
    # (ladle above + boss rise) / C right. Distinct pour shapes (plan section 8). ----
    unreal.log_warning("MILL: furnaces")
    FA = (-9000.0, 2000.0)   # left  - fan (spills south into the open floor)
    FB = (0.0, 2000.0)       # centre - circular; ladle hangs above; Foundryman rises here
    FC = (9000.0, 2000.0)    # right - river (runs across the floor)
    furnace(FA[0], FA[1], "A")
    furnace(FB[0], FB[1], "B")
    furnace(FC[0], FC[1], "C")

    unreal.log_warning("MILL: hazards")
    ha = spawn(unreal.BLHazard_FurnacePour, (FA[0], FA[1], 150), label="Pour_A_Fan")
    ha.set_editor_property("pour_shape", unreal.BLPourShape.FAN)
    ha.set_editor_property("pour_yaw_deg", 270.0)        # fan spills south into the open floor
    ha.set_editor_property("fan_half_angle_deg", 45.0)
    ha.set_editor_property("zone_radius", 2300.0)
    ha.set_editor_property("start_offset_seconds", 60.0)
    # B (centre) = straight river across the floor (per the blockout diagram)
    hb = spawn(unreal.BLHazard_FurnacePour, (FB[0], FB[1], 150), label="Pour_B_River")
    hb.set_editor_property("pour_shape", unreal.BLPourShape.RIVER)
    hb.set_editor_property("pour_yaw_deg", 180.0)        # straight molten line running E-W
    hb.set_editor_property("river_half_width", 850.0)
    hb.set_editor_property("river_length", 4500.0)
    hb.set_editor_property("zone_radius", 4800.0)
    hb.set_editor_property("start_offset_seconds", 0.0)
    # C (right) = circular overflow (per the blockout diagram)
    hc = spawn(unreal.BLHazard_FurnacePour, (FC[0], FC[1], 150), label="Pour_C_Circ")
    hc.set_editor_property("pour_shape", unreal.BLPourShape.CIRCULAR)
    hc.set_editor_property("zone_radius", 2300.0)
    hc.set_editor_property("start_offset_seconds", 30.0)

    # ---- boilers centred on each furnace (Sketchfab CC0); FB non-colliding for boss rise ----
    unreal.log_warning("MILL: boilers")
    boiler = load_mesh("/Game/BlackLedger/Meshes/industrial_boiler_01/SM_Mill_Boiler")
    if boiler:
        for (fx, fy), nm, is_boss in [(FA, "A", False), (FB, "B", True), (FC, "C", False)]:
            place_static(boiler, fx, fy, 180.0, f"Boiler_{nm}",
                         ground_z=0.0, target_h=2000.0, collide=not is_boss)
    else:
        unreal.log_warning("MILL: boiler mesh missing - run import_boiler.py first")

    # ---- DRIVABLE SQUARE CATWALK LOOP: an elevated ring road on columns around the pit
    # (the reference's square frame). Drive up the W/E ramps, loop around, drop down. ----
    unreal.log_warning("MILL: catwalk loop")
    LX, LY = 16000.0, 13000.0          # loop centreline rectangle (half-extents)
    RW = 12.0                          # 12 m road width
    nlen = (2 * LX + 1200) / 100.0     # N/S roads span full width (cover the corners)
    elen = (2 * LY - 1200) / 100.0     # E/W roads fit between the N/S roads
    loop_road(0, LY, nlen, RW, "N")
    # the SOUTH side's MIDDLE is the collapsible catwalk (x -6500..6500); greybox road flanks
    # it out to the corners (per the blockout: the south edge = "collapsible catwalk -> debris ramp")
    loop_road(-11550, -LY, 101, RW, "Sw")
    loop_road(11550, -LY, 101, RW, "Se")
    loop_road(LX, 0, RW, elen, "E")
    loop_road(-LX, 0, RW, elen, "W")
    for i, x in enumerate((-12000, -4000, 4000, 12000)):
        loop_support(x, LY, f"N{i}")
    for i, x in enumerate((-14000, -9000, 9000, 14000)):   # south: clear of the collapsible middle (+/-6500)
        loop_support(x, -LY, f"S{i}")
    for i, y in enumerate((-8000, 0, 8000)):
        loop_support(LX, y, f"E{i}")
        loop_support(-LX, y, f"W{i}")
    # two entry ramps (W + E), flush-top onto the road edge at y=0 (proven 9 deg geometry)
    loop_ramp(-12684, 401, -9.0, "W")  # high (-X) end at the W road inner edge (x=-15400)
    loop_ramp(12684, 401, 9.0, "E")    # high (+X) end at the E road inner edge (x=+15400)

    # ---- collapsible catwalk = the MIDDLE of the south loop side (destructible #2). It's a
    # flush deck section (entry ramps suppressed - you reach it by driving the loop); shooting
    # its supports collapses the mid-span into a debris ramp down to the floor. ----
    unreal.log_warning("MILL: collapsible south catwalk")
    cw = spawn(unreal.BLDestructible_Catwalk, (0, -LY, 0), label="Catwalk")
    # bool UPROPERTY bEntryRamps -> Python drops the leading 'b' = "entry_ramps". Guarded so a
    # property hiccup (e.g. C++ not rebuilt) never aborts the whole map build.
    for _pname in ("entry_ramps", "b_entry_ramps"):
        try:
            cw.set_editor_property(_pname, False)
            break
        except Exception as _e:
            unreal.log_warning(f"catwalk {_pname} set failed: {_e}")

    # ---- "Tap the Ladle" (destructible #1): hanging ladle over the CENTRE furnace ----
    spawn(unreal.BLDestructible_Ladle, (FB[0], FB[1], 0), label="Ladle")

    # ---- some south-half cover (open area, clear of the furnace row / ramps / bridge) ----
    unreal.log_warning("MILL: cover")
    for cx, cy, nm in [(-6000, -3500, "SW"), (6000, -3500, "SE"), (0, -11000, "S")]:  # clear of the trench (y -8000..-6000)
        cover_column(cx, cy, nm)
    for sx, sy, nm in [(-3500, -4000, "c0"), (3500, -4500, "c1")]:
        cover_scrap(sx, sy, nm)

    # ---- outer loading-bay ring + LOS-break rail cars (outside the loop) ----
    unreal.log_warning("MILL: ring + bays")
    bays = [(0.0, 0.80 * HH, "N"), (-0.80 * HW, 0.0, "W"), (0.80 * HW, 0.0, "E"),
            (-0.76 * HW, -0.80 * HH, "SW"), (0.76 * HW, -0.80 * HH, "SE")]
    for bx, by, nm in bays:
        spawn_cube((bx, by, 25), (32, 22, 0.5), label=f"LoadingBay_{nm}")
    for rx, ry, ryaw, nm in [(-19500, 17000, 28, "r0"), (19500, 17000, -28, "r1"),
                             (-19500, -17000, -28, "r2"), (19500, -17000, 28, "r3"),
                             (21000, 4500, 90, "r4"), (-21000, -4500, 90, "r5")]:
        cover_railcar(rx, ry, ryaw, nm)

    # ---- sealed bay (E wall pocket; inner wall tagged BLSealedDoor -> opened by the FC valve) ----
    door = spawn_cube((HW - 1050, 5600, 200), (1, 20, 4), label="SealedBay_Wall")
    door.set_editor_property("tags", ["BLSealedDoor"])
    spawn_cube((HW - 525, 6600, 200), (10.5, 1, 4), label="SealedBay_North")
    spawn_cube((HW - 525, 4600, 200), (10.5, 1, 4), label="SealedBay_South")

    # ---- "Blow the Furnace" (destructible #3): valves on Furnace C (right, nearest the
    # E-wall sealed bay) -> blast + drops the sealed door, opening the reward room ----
    spawn(unreal.BLDestructible_FurnaceValve, (FC[0], FC[1], 0), label="FurnaceValve_C")

    # ---- navigation bounds ----
    unreal.log_warning("MILL: navbounds")
    nav = spawn(unreal.BLNavBounds, (0, 0, 500), label="NavBounds")
    nav.set_editor_property("bounds_extent", unreal.Vector(HW + 500, HH + 500, 1600))

    # ---- 12 pickups: center offensive + loop-deck reward + outer-ring health + sealed bay ----
    unreal.log_warning("MILL: pickups")
    power_cls = unreal.BLProjectile_Power.static_class()

    def homing(x, y, i, z=120):
        spawn(unreal.BLPickupActor, (x, y, z), label=f"Pickup_Homing_{i}")

    def power(x, y, i, z=120):
        p = spawn(unreal.BLPickupActor, (x, y, z), label=f"Pickup_Power_{i}")
        p.set_editor_property("projectile_class", power_cls)
        p.set_editor_property("ammo", 2)
        p.set_editor_property("weapon_name", "Power Missile")

    def health(x, y, i):
        spawn(unreal.BLPickup_Health, (x, y, 120), label=f"Pickup_Health_{i}")

    power(0, LY, 0, z=970)            # ON the N loop deck (top z=850) - speed-skill reward
    homing(FA[0], FA[1] - 2500, 0)    # just in front of each furnace (draws fights to the row)
    homing(FB[0], FB[1] - 2500, 1)
    homing(FC[0], FC[1] - 2500, 2)
    homing(0, -7000, 3)
    homing(-13000, 4000, 4)
    homing(13000, 4000, 5)
    power(-12000, -6000, 1)           # south-floor mid
    health(0, 0.80 * HH - 1600, 0)    # by the N bay
    health(-0.80 * HW + 1600, 0, 1)   # by the W bay
    health(0.80 * HW - 1600, 0, 2)    # by the E bay
    power(HW - 2000, 5600, 2)         # inside the sealed bay = rare reward

    # ---- boss rise: the Foundryman surfaces from the CENTRE furnace ----
    rise = spawn(unreal.TargetPoint, (FB[0], FB[1], 200), (0, 0, 90), "BossRise")
    rise.set_editor_property("tags", ["BLBossRise"])

    # ---- 14 FFA spawn nodes around the OUTER RING (outside the loop, >=2 exits each) ----
    unreal.log_warning("MILL: spawns")
    ring = [(-13000, 19500), (-4000, 20500), (5000, 20500), (13000, 19500),   # N
            (23000, 9000), (24000, 0), (23000, -9000),                        # E
            (13000, -19500), (4000, -20500), (-5000, -20500), (-13000, -19500),  # S
            (-23000, -9000), (-24000, 0), (-23000, 9000)]                     # W
    for i, (x, y) in enumerate(ring):
        spawn_node(x, y, f"Spawn_{i:02d}")

    # ---- lighting: dark, moody foundry lit by FIRE, not a sky. A dim cold key gives
    # base form; the furnaces + scattered molten fill lights wash the floor orange so the
    # arena reads clearly for combat. NO SkyAtmosphere (a daytime dome kills the tone). ----
    unreal.log_warning("MILL: lights")
    sun = spawn(unreal.DirectionalLight, (0, 0, 1200), (0, -50, 25), "Sun")
    sun.light_component.set_editor_property("intensity", 1.1)         # faint cold roof key (form only)
    sun.light_component.set_editor_property("light_color", unreal.Color(170, 178, 200, 255))
    sky = spawn(unreal.SkyLight, (0, 0, 1200), label="SkyLight")
    skc = sky.light_component
    skc.set_editor_property("intensity", 0.45)                        # dim near-neutral fill (was a blue wash)
    skc.set_editor_property("light_color", unreal.Color(98, 94, 96, 255))
    # scattered warm "molten" fill lights so the fiery glow reaches the whole floor,
    # not just the three furnaces (floor pools, vents, slag runoff).
    for i, (fx, fy) in enumerate([(-6000, -3000), (6000, -3000), (-6000, 4000),
                                  (6000, 4000), (0, 0), (0, -7000)]):
        fl = spawn(unreal.PointLight, (fx, fy, 350), label=f"MoltenFill_{i}")
        flc = fl.light_component
        flc.set_editor_property("intensity", 24000.0)
        flc.set_editor_property("attenuation_radius", 9000.0)
        flc.set_editor_property("light_color", unreal.Color(255, 118, 38, 255))
        flc.set_editor_property("cast_shadows", False)

    # ---- flickering industrial lamps (ABLIndustrialFlickerLight, plan section 24):
    # sickly bay sodium lamps, catwalk work bulbs, red warning lamps by the furnaces.
    # Atmosphere + readability tells. Guarded so the map still builds if the C++ class
    # isn't compiled yet (build C++ first, then re-run for the lamps). ----
    unreal.log_warning("MILL: flicker lamps")
    try:
        FlickerCls = unreal.BLIndustrialFlickerLight

        def flicker(x, y, z, base, color, radius, amount, name):
            f = spawn(FlickerCls, (x, y, z), label=f"Flicker_{name}")
            f.set_editor_property("base_intensity", base)
            f.set_editor_property("flicker_amount", amount)
            f.set_editor_property("light_color", unreal.Color(*color))
            f.set_editor_property("attenuation_radius", radius)
            return f

        for nm, bx, by in [("N", 0, 0.80 * HH), ("W", -0.80 * HW, 0), ("E", 0.80 * HW, 0),
                           ("SW", -0.76 * HW, -0.80 * HH), ("SE", 0.76 * HW, -0.80 * HH)]:
            flicker(bx, by, 800, 5000.0, (255, 205, 120, 255), 4000.0, 2200.0, f"Bay_{nm}")
        # sickly lamps down in the slag trench (it's below grade + dark)
        flicker(-9000, -7000, 250, 3200.0, (255, 200, 120, 255), 2600.0, 1500.0, "Trench_W")
        flicker(9000, -7000, 250, 3200.0, (255, 200, 120, 255), 2600.0, 1500.0, "Trench_E")
        flicker(-4200, 8000, 1050, 3000.0, (210, 200, 180, 255), 2600.0, 1400.0, "Cat_W")  # on the cross-bridge
        flicker(4200, 8000, 1050, 3000.0, (210, 200, 180, 255), 2600.0, 1400.0, "Cat_E")
        for nm, fx, fy in [("A", FA[0] + 1600, FA[1]), ("B", FB[0] - 1600, FB[1]),
                           ("C", FC[0], FC[1] + 1900)]:
            flicker(fx, fy, 700, 2600.0, (255, 45, 25, 255), 3000.0, 1200.0, f"Warn_{nm}")
    except Exception as _e:
        unreal.log_warning(f"MILL: flicker lamps skipped ({_e}) - build C++ then re-run")

    fog = spawn(unreal.ExponentialHeightFog, (0, 0, 0), label="Fog")
    fog.component.set_editor_property("fog_density", 0.08)            # haze that catches the glow
    # warm ember tint so the haze glows orange near the fires - UE5 renamed the UE4
    # 'FogInscatteringColor'; try both, never crash
    for _prop in ("fog_inscattering_luminance", "fog_inscattering_color"):
        try:
            fog.component.set_editor_property(_prop, unreal.LinearColor(0.16, 0.10, 0.07, 1.0))
            break
        except Exception as _e:
            unreal.log_warning(f"fog color via {_prop}: {_e}")

    # ---- player start: SW loading bay (a blockout start zone), facing center ----
    unreal.log_warning("MILL: playerstart")
    px, py = -0.76 * HW, -0.80 * HH
    spawn(unreal.PlayerStart, (px, py, 250), (0, 0, math.degrees(math.atan2(-py, -px))), "PlayerStart")

    # ---- DRESSING PASS: grim materials + cinematic grade (no external assets;
    # Megascans meshes swap in later). All guarded so the map still builds if the
    # FX materials or a post-process property name aren't present. ----
    unreal.log_warning("MILL: dressing")
    if DRESS:
        dress_with_meshes(FA, FB, FC)   # Fab/Megascans mesh art pass (M5); off for the M1 graybox
    else:
        unreal.log_warning("MILL: DRESS=False (graybox M1) - skipping Fab mesh pass")
    # prefer the real PBR materials (import_textures.py) if present; else the flat fallback
    floor_mat = (eal.load_asset("/Game/BlackLedger/FX/M_BL_MillFloorPBR")
                 or eal.load_asset("/Game/BlackLedger/FX/M_BL_MillFloor"))
    wall_mat = (eal.load_asset("/Game/BlackLedger/FX/M_BL_MillWallPBR")
                or eal.load_asset("/Game/BlackLedger/FX/M_BL_MillWall"))
    if floor_mat and wall_mat:
        for a in actors.get_all_level_actors():
            if not isinstance(a, unreal.StaticMeshActor):
                continue
            lbl = a.get_actor_label()
            smc = a.static_mesh_component
            if lbl.startswith("Floor") or lbl.startswith("LoadingBay"):
                smc.set_material(0, floor_mat)        # floor frame, trench bottom + ramps = concrete
            else:
                smc.set_material(0, wall_mat)         # walls / furnaces / bays / trench walls = dark metal
    else:
        unreal.log_warning("MILL: mill materials missing - run make_fx_materials.py first")

    # cinematic grade: desaturated, cold, contrasty, bloomed + vignetted (Se7en / Silent Hill)
    def setp(s, name, val):
        try:
            s.set_editor_property(name, val)
        except Exception as e:
            unreal.log_warning(f"PPV {name}: {e}")

    ppv = spawn(unreal.PostProcessVolume, (0, 0, 800), label="GrimGrade")
    ppv.set_editor_property("unbound", True)
    s = ppv.get_editor_property("settings")
    setp(s, "override_color_saturation", True)
    setp(s, "color_saturation", unreal.Vector4(0.72, 0.74, 0.78, 1.0))   # drained, slightly cold
    setp(s, "override_color_contrast", True)
    setp(s, "color_contrast", unreal.Vector4(1.10, 1.10, 1.08, 1.0))
    setp(s, "override_white_temp", True)
    setp(s, "white_temp", 7200.0)                                        # warm balance = fire-lit, not blue
    setp(s, "override_bloom_intensity", True)
    setp(s, "bloom_intensity", 1.0)                                      # furnace glow blooms (pulled back so pours read orange, not white)
    setp(s, "override_vignette_intensity", True)
    setp(s, "vignette_intensity", 0.55)
    setp(s, "override_film_grain_intensity", True)
    setp(s, "film_grain_intensity", 0.18)
    # lock auto-exposure so the scene stays as DARK as it's actually lit (no eye-adaptation
    # boosting the dim interior back to "normal"). min==max disables adaptation; the bias
    # pulls the locked exposure down a touch for the oppressive read.
    setp(s, "override_auto_exposure_min_brightness", True)
    setp(s, "auto_exposure_min_brightness", 1.0)
    setp(s, "override_auto_exposure_max_brightness", True)
    setp(s, "auto_exposure_max_brightness", 1.0)
    setp(s, "override_auto_exposure_bias", True)
    setp(s, "auto_exposure_bias", 0.3)        # playable brightness (raise toward +1 if still dim)
    ppv.set_editor_property("settings", s)

    # ---- M6 atmosphere: Megascans decals (rust/blood/graffiti/debris) ----
    unreal.log_warning("MILL: decals")
    place_decals(FA, FB, FC)

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
