# Black Ledger - vehicle prep: Meshy FBX -> UE-ready FBX
# Carves fused wheels out of the body, installs clean cylinder combat wheels,
# fixes scale/pivots/axes, optional decimate, exports FBX + report + previews.
#
# Run headless:
#   blender.exe --background --python tools\blender\prep_vehicle.py -- ^
#     --src art\source\vehicles\01_Surgeon\Surgeon.fbx --name Surgeon --length 6.5
#
# Outputs (next to --src):
#   <Name>_UE.fbx, <Name>_prep.blend, <Name>_prep_report.json, <Name>_preview_*.png

import bpy, bmesh, json, math, os, sys, argparse
from mathutils import Vector

# ---------- args ----------
argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
ap = argparse.ArgumentParser()
ap.add_argument("--src", required=True)
ap.add_argument("--name", required=True)
ap.add_argument("--length", type=float, default=6.5, help="target length in meters")
ap.add_argument("--flip", action="store_true", help="rotate 180 if front faces -X")
ap.add_argument("--wheel-radius", type=float, default=0.0, help="override fitted radius (m)")
ap.add_argument("--wheel-width", type=float, default=0.0, help="override fitted width (m)")
ap.add_argument("--decimate", type=float, default=0.5, help="body decimate ratio; 1.0 = off")
args = ap.parse_args(argv)

SRC = os.path.abspath(args.src)
OUTDIR = os.path.dirname(SRC)
NAME = args.name
report = {"src": SRC, "name": NAME}

def out(p): return os.path.join(OUTDIR, p)

# ---------- clean scene ----------
bpy.ops.wm.read_factory_settings(use_empty=True)

# ---------- import ----------
bpy.ops.import_scene.fbx(filepath=SRC)
meshes = [o for o in bpy.context.scene.objects if o.type == "MESH"]
if not meshes:
    raise SystemExit("no meshes imported")
for o in bpy.context.scene.objects:
    o.select_set(False)
for o in meshes:
    o.select_set(True)
bpy.context.view_layer.objects.active = meshes[0]
if len(meshes) > 1:
    bpy.ops.object.join()
body = bpy.context.view_layer.objects.active
body.name = f"SM_{NAME}_Body"
# clear parenting (Meshy FBX often has empties) and apply transforms
bpy.ops.object.parent_clear(type="CLEAR_KEEP_TRANSFORM")
for o in list(bpy.context.scene.objects):
    if o.type != "MESH":
        bpy.data.objects.remove(o, do_unlink=True)
body.select_set(True)
bpy.context.view_layer.objects.active = body
bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)

def bbox(o):
    pts = [o.matrix_world @ Vector(c) for c in o.bound_box]
    mn = Vector((min(p.x for p in pts), min(p.y for p in pts), min(p.z for p in pts)))
    mx = Vector((max(p.x for p in pts), max(p.y for p in pts), max(p.z for p in pts)))
    return mn, mx

# ---------- orient: length along X ----------
mn, mx = bbox(body)
ext = mx - mn
if ext.y > ext.x:  # length came in along Y -> rotate -90 Z
    body.rotation_euler = (0, 0, math.radians(-90))
    bpy.ops.object.transform_apply(rotation=True)
    mn, mx = bbox(body); ext = mx - mn
if args.flip:
    body.rotation_euler = (0, 0, math.radians(180))
    bpy.ops.object.transform_apply(rotation=True)
    mn, mx = bbox(body); ext = mx - mn

# ---------- scale to real length ----------
s = args.length / ext.x
body.scale = (s, s, s)
bpy.ops.object.transform_apply(scale=True)
mn, mx = bbox(body); ext = mx - mn
report["scaled_extents_m"] = [round(v, 3) for v in ext]

# ---------- ground + center (provisional) ----------
body.location.x -= (mn.x + mx.x) / 2
body.location.y -= (mn.y + mx.y) / 2
body.location.z -= mn.z
bpy.ops.object.transform_apply(location=True)
mn, mx = bbox(body); ext = mx - mn

# ---------- wheel zone detection ----------
def fit_circle(pts):  # least squares in (a,b)
    n = len(pts)
    ma = sum(p[0] for p in pts) / n; mb = sum(p[1] for p in pts) / n
    suu = svv = suv = suuu = svvv = suvv = svuu = 0.0
    for a, b in pts:
        u, v = a - ma, b - mb
        suu += u*u; svv += v*v; suv += u*v
        suuu += u**3; svvv += v**3; suvv += u*v*v; svuu += v*u*u
    det = suu*svv - suv*suv
    if abs(det) < 1e-12: return None
    uc = (svv*(suuu+suvv) - suv*(svvv+svuu)) / (2*det)
    vc = (suu*(svvv+svuu) - suv*(suuu+suvv)) / (2*det)
    r = math.sqrt(uc*uc + vc*vc + (suu+svv)/n)
    return ma+uc, mb+vc, r

verts = [body.matrix_world @ v.co for v in body.data.vertices]
half_w = ext.y / 2
zband = mn.z + 0.40 * ext.z
wheels = {}
fallback_r = 0.115 * args.length / 2  # sane tire radius ~ length-proportional
for xs, ys, tag in ((1, 1, "FL"), (1, -1, "FR"), (-1, 1, "RL"), (-1, -1, "RR")):
    zone = [v for v in verts
            if v.z < zband and ys * v.y > 0.62 * half_w and xs * v.x > 0.04 * ext.x]
    fit = fit_circle([(v.x, v.z) for v in zone]) if len(zone) > 40 else None
    ok = False
    if fit:
        cx, cz, r = fit
        inliers = [v for v in zone if abs(math.hypot(v.x - cx, v.z - cz) - r) < 0.3 * r]
        if 0.05 * args.length / 6.5 < r < 0.55 and len(inliers) > 0.4 * len(zone) and cz < 0.30 * ext.z:
            fit2 = fit_circle([(v.x, v.z) for v in inliers]) or (cx, cz, r)
            cx, cz, r = fit2
            ok = True
    if not ok:
        # fallback: cluster mean x, default radius
        xsel = [v.x for v in zone] or [xs * 0.32 * ext.x]
        cx, cz, r = sum(xsel) / len(xsel), fallback_r, fallback_r
    ys_sel = [abs(v.y) for v in zone] or [half_w * 0.85]
    w = max(0.18, min(0.40, (max(ys_sel) - min(ys_sel)) * 1.15)) if len(ys_sel) > 5 else 0.26
    wheels[tag] = {"x": cx, "y": ys * (half_w * 0.86), "r": r, "w": w, "cz": max(cz, 0.02),
                   "fit_ok": ok, "zone_pts": len(zone)}

# enforce left/right symmetry per axle (average the pair)
for fa, fb in (("FL", "FR"), ("RL", "RR")):
    ax = (wheels[fa]["x"] + wheels[fb]["x"]) / 2
    ar = (wheels[fa]["r"] + wheels[fb]["r"]) / 2
    aw = (wheels[fa]["w"] + wheels[fb]["w"]) / 2
    acz = (wheels[fa]["cz"] + wheels[fb]["cz"]) / 2
    for f in (fa, fb):
        wheels[f]["x"], wheels[f]["r"], wheels[f]["w"], wheels[f]["cz"] = ax, ar, aw, acz
if args.wheel_radius > 0:
    for f in wheels: wheels[f]["r"] = args.wheel_radius
if args.wheel_width > 0:
    for f in wheels: wheels[f]["w"] = args.wheel_width
report["wheels"] = {k: {kk: (round(vv, 3) if isinstance(vv, float) else vv)
                        for kk, vv in d.items()} for k, d in wheels.items()}

# ---------- carve wheel openings (no boolean: delete faces fully inside each cylinder) ----------
carve_log = []
bm = bmesh.new()
bm.from_mesh(body.data)
for tag, wd in wheels.items():
    r_cut = wd["r"] * 1.05
    cz = wd["cz"]
    side = 1 if wd["y"] > 0 else -1
    y_in = max(abs(wd["y"]) - wd["w"] * 1.6, 0.12)  # deep enough to take the inner barrel
    doomed = [f for f in bm.faces if all(
        side * v.co.y > y_in and math.hypot(v.co.x - wd["x"], v.co.z - cz) < r_cut
        for v in f.verts)]
    bmesh.ops.delete(bm, geom=doomed, context="FACES")
    carve_log.append(f"{tag}: removed {len(doomed)} faces")
loose = [v for v in bm.verts if not v.link_faces]
if loose:
    bmesh.ops.delete(bm, geom=loose, context="VERTS")
bm.to_mesh(body.data)
bm.free()

# clean stray shards: drop disconnected islands left inside the wells
bm = bmesh.new()
bm.from_mesh(body.data)
bm.verts.ensure_lookup_table()
parent = list(range(len(bm.verts)))
def _find(a):
    while parent[a] != a:
        parent[a] = parent[parent[a]]; a = parent[a]
    return a
for e in bm.edges:
    ra, rb = _find(e.verts[0].index), _find(e.verts[1].index)
    if ra != rb:
        parent[ra] = rb
groups = {}
for v in bm.verts:
    groups.setdefault(_find(v.index), []).append(v)
main_size = max(len(g) for g in groups.values())
def in_well(centroid):
    for wd in wheels.values():
        if (math.hypot(centroid.x - wd["x"], centroid.z - wd["cz"]) < wd["r"] * 1.35 and
                abs(centroid.y) > max(abs(wd["y"]) - wd["w"] * 1.8, 0.1)):
            return True
    return False
shard_verts = []
for g in groups.values():
    if len(g) == main_size:
        continue
    centroid = sum((v.co for v in g), Vector()) / len(g)
    if len(g) < max(40, main_size * 0.002) or in_well(centroid):
        shard_verts.extend(g)
if shard_verts:
    bmesh.ops.delete(bm, geom=shard_verts, context="VERTS")
bm.to_mesh(body.data)
bm.free()
report["carve"] = carve_log
report["shard_verts_removed"] = len(shard_verts)

# ---------- decimate body (before liners join, so liners stay clean) ----------
if 0 < args.decimate < 1.0:
    mod = body.modifiers.new("Decimate", "DECIMATE")
    mod.ratio = args.decimate
    bpy.context.view_layer.objects.active = body
    bpy.ops.object.modifier_apply(modifier=mod.name)

# ---------- wheel-well liners (dark tubs that hide the cut edges) ----------
well_mat = bpy.data.materials.new("M_Well")
well_mat.use_nodes = True
well_mat.node_tree.nodes["Principled BSDF"].inputs["Base Color"].default_value = (0.015, 0.015, 0.016, 1)
well_mat.node_tree.nodes["Principled BSDF"].inputs["Roughness"].default_value = 0.95

body_verts = [v.co.copy() for v in body.data.vertices]
liners = []
for tag, wd in wheels.items():
    r_l = wd["r"] * 1.06          # just behind the carve edge (cut radius is r*1.05)
    side = 1 if wd["y"] > 0 else -1
    d = wd["w"] * 1.6
    y_outer = abs(wd["y"]) + wd["w"] * 0.30   # tucked INSIDE the arch panel, never proud of it
    # the tire occupies the opening below axle height - the liner only needs the upper arch
    local_floor = wd["cz"] - wd["r"] * 0.25
    bpy.ops.mesh.primitive_cylinder_add(vertices=32, radius=r_l, depth=d,
                                        location=(wd["x"], side * (y_outer - d / 2), wd["cz"]))
    ln = bpy.context.view_layer.objects.active
    ln.rotation_euler = (math.radians(90), 0, 0)  # axis along Y
    bpy.ops.object.transform_apply(rotation=True)
    lbm = bmesh.new()
    lbm.from_mesh(ln.data)
    outer_caps = [f for f in lbm.faces if f.normal.y * side > 0.9]  # open the outboard end
    bmesh.ops.delete(lbm, geom=outer_caps, context="FACES")
    # trim everything hanging below the local bodywork (front valance sits high!)
    low = [f for f in lbm.faces
           if sum(v.co.z for v in f.verts) / len(f.verts) < local_floor + 0.015]
    if low:
        bmesh.ops.delete(lbm, geom=low, context="FACES")
    stray = [v for v in lbm.verts if not v.link_faces]
    if stray:
        bmesh.ops.delete(lbm, geom=stray, context="VERTS")
    bmesh.ops.reverse_faces(lbm, faces=list(lbm.faces))  # visible from outside, through the arch
    lbm.to_mesh(ln.data)
    lbm.free()
    ln.data.materials.append(well_mat)
    liners.append(ln)
    carve_log.append(f"{tag}: liner trimmed at z<{local_floor + 0.015:.3f}")
for o in bpy.context.scene.objects:
    o.select_set(False)
for ln in liners:
    ln.select_set(True)
body.select_set(True)
bpy.context.view_layer.objects.active = body
bpy.ops.object.join()  # liners become body slot 1 (M_Well)
report["liners"] = len(liners)
report["body_tris_final"] = sum(len(p.vertices) - 2 for p in body.data.polygons)

# ---------- build replacement wheels ----------
tire = bpy.data.materials.new("M_Tire")
tire.use_nodes = True
tire.node_tree.nodes["Principled BSDF"].inputs["Base Color"].default_value = (0.018, 0.018, 0.02, 1)
tire.node_tree.nodes["Principled BSDF"].inputs["Roughness"].default_value = 0.92
hub = bpy.data.materials.new("M_Hub")
hub.use_nodes = True
hub.node_tree.nodes["Principled BSDF"].inputs["Base Color"].default_value = (0.08, 0.08, 0.09, 1)
hub.node_tree.nodes["Principled BSDF"].inputs["Metallic"].default_value = 1.0
hub.node_tree.nodes["Principled BSDF"].inputs["Roughness"].default_value = 0.45

wheel_objs = []
for tag, wd in wheels.items():
    r, w = wd["r"], wd["w"]
    bpy.ops.mesh.primitive_cylinder_add(vertices=24, radius=r, depth=w,
                                        location=(wd["x"], wd["y"], r))
    wo = bpy.context.view_layer.objects.active
    wo.rotation_euler = (math.radians(90), 0, 0)  # axis along Y
    bpy.ops.object.transform_apply(rotation=True)
    wo.name = f"SM_{NAME}_Wheel_{tag}"
    # bevel tire shoulders
    bv = wo.modifiers.new("Bevel", "BEVEL")
    bv.width = min(0.025, r * 0.12); bv.segments = 2
    bpy.ops.object.modifier_apply(modifier=bv.name)
    wo.data.materials.append(tire)
    wo.data.materials.append(hub)
    for p in wo.data.polygons:  # caps face +-Y after rotation
        if abs(p.normal.y) > 0.7:
            p.material_index = 1
    bpy.ops.object.shade_smooth()
    wheel_objs.append(wo)
report["wheel_tris"] = sum(sum(len(p.vertices) - 2 for p in w.data.polygons) for w in wheel_objs)

# ---------- final centering on wheelbase ----------
cx = sum(w["x"] for w in wheels.values()) / 4
for o in [body] + wheel_objs:
    o.location.x -= cx
    bpy.context.view_layer.objects.active = o
    o.select_set(True)
    bpy.ops.object.transform_apply(location=True)
    o.select_set(False)
# body origin -> world origin (ground, wheelbase center); wheel origins -> centers
bpy.context.scene.cursor.location = (0, 0, 0)
bpy.context.view_layer.objects.active = body
body.select_set(True)
bpy.ops.object.origin_set(type="ORIGIN_CURSOR")
body.select_set(False)
for wo in wheel_objs:
    bpy.context.view_layer.objects.active = wo
    wo.select_set(True)
    bpy.ops.object.origin_set(type="ORIGIN_GEOMETRY", center="BOUNDS")
    wo.select_set(False)
mn, mx = bbox(body)
report["final_extents_m"] = [round(v, 3) for v in (mx - mn)]
report["wheelbase_m"] = round(abs(wheels["FL"]["x"] - wheels["RL"]["x"]), 3)

# ---------- export ----------
for o in [body] + wheel_objs:
    o.select_set(True)
fbx_path = out(f"{NAME}_UE.fbx")
bpy.ops.export_scene.fbx(filepath=fbx_path, use_selection=True,
                         object_types={"MESH"}, mesh_smooth_type="FACE",
                         apply_scale_options="FBX_SCALE_NONE", bake_space_transform=True,
                         add_leaf_bones=False)
report["fbx"] = fbx_path

# ---------- save core outputs before attempting renders ----------
bpy.ops.wm.save_as_mainfile(filepath=out(f"{NAME}_prep.blend"))
with open(out(f"{NAME}_prep_report.json"), "w") as f:
    json.dump(report, f, indent=1)

# ---------- previews (workbench, headless-safe) ----------
scene = bpy.context.scene
scene.render.engine = "BLENDER_WORKBENCH"
scene.display.shading.light = "STUDIO"
scene.display.shading.color_type = "MATERIAL"
scene.render.resolution_x = 960; scene.render.resolution_y = 640
scene.render.film_transparent = False
cam_data = bpy.data.cameras.new("Cam"); cam = bpy.data.objects.new("Cam", cam_data)
scene.collection.objects.link(cam); scene.camera = cam
mn, mx = bbox(body)
ctr = (mn + mx) / 2
size = max((mx - mn).length, 1)
views = {"front": ((size*1.6, 0, ctr.z + size*0.15), ctr),
         "side": ((0, -size*1.6, ctr.z + size*0.15), ctr),
         "three_quarter": ((size*1.2, -size*1.2, size*0.55), ctr),
         "low_rear": ((-size*1.2, -size*0.9, size*0.25), ctr)}
for wo in wheel_objs:
    tag = wo.name.split("_")[-1]
    wl = wo.location
    side = 1 if wl.y > 0 else -1
    views[f"arch_{tag}"] = ((wl.x + (0.9 if wl.x >= 0 else -0.9), wl.y + side * 1.7, wl.z + 0.55),
                            Vector((wl.x, wl.y, wl.z)))
for vname, (pos, target) in views.items():
    cam.location = pos
    d = Vector(target) - cam.location
    cam.rotation_euler = d.to_track_quat("-Z", "Y").to_euler()
    scene.render.filepath = out(f"{NAME}_preview_{vname}.png")
    bpy.ops.render.render(write_still=True)
report["previews"] = list(views.keys())

# ---------- final report ----------
with open(out(f"{NAME}_prep_report.json"), "w") as f:
    json.dump(report, f, indent=1)
print("PREP OK:", json.dumps(report, indent=1))
