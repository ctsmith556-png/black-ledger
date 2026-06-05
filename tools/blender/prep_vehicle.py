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
    wheels[tag] = {"x": cx, "y": ys * (half_w * 0.86), "r": r, "w": w, "fit_ok": ok,
                   "zone_pts": len(zone)}

# enforce left/right symmetry per axle (average the pair)
for fa, fb in (("FL", "FR"), ("RL", "RR")):
    ax = (wheels[fa]["x"] + wheels[fb]["x"]) / 2
    ar = (wheels[fa]["r"] + wheels[fb]["r"]) / 2
    aw = (wheels[fa]["w"] + wheels[fb]["w"]) / 2
    for f in (fa, fb):
        wheels[f]["x"], wheels[f]["r"], wheels[f]["w"] = ax, ar, aw
if args.wheel_radius > 0:
    for f in wheels: wheels[f]["r"] = args.wheel_radius
if args.wheel_width > 0:
    for f in wheels: wheels[f]["w"] = args.wheel_width
report["wheels"] = {k: {kk: (round(vv, 3) if isinstance(vv, float) else vv)
                        for kk, vv in d.items()} for k, d in wheels.items()}

# ---------- carve wheel zones out of body ----------
bm = bmesh.new()
bm.from_mesh(body.data)
bm.verts.ensure_lookup_table()
doomed = set()
for tag, wd in wheels.items():
    cx, cy, r, w = wd["x"], wd["y"], wd["r"], wd["w"]
    cz = r if not wd["fit_ok"] else max(r * 0.95, 0.01)
    side = 1 if cy > 0 else -1
    y_in = abs(cy) - w * 0.75
    for v in bm.verts:
        if side * v.co.y > y_in and math.hypot(v.co.x - cx, v.co.z - cz) < r * 1.10:
            doomed.add(v)
faces = set()
for v in doomed:
    faces.update(v.link_faces)
bmesh.ops.delete(bm, geom=list(faces), context="FACES")
# drop stranded verts/edges
loose_v = [v for v in bm.verts if not v.link_faces]
bmesh.ops.delete(bm, geom=loose_v, context="VERTS")
# cap the arch holes (fill boundary loops near each carve zone)
boundary = [e for e in bm.edges if e.is_boundary]
near = []
for e in boundary:
    mid = (e.verts[0].co + e.verts[1].co) / 2
    for wd in wheels.values():
        if (abs(mid.y) > abs(wd["y"]) - wd["w"] * 1.5 and
                math.hypot(mid.x - wd["x"], mid.z - max(wd["r"], 0.01)) < wd["r"] * 1.6):
            near.append(e); break
if near:
    bmesh.ops.holes_fill(bm, edges=near, sides=0)
bm.to_mesh(body.data)
bm.free()
report["carved_verts"] = len(doomed)

# ---------- decimate body ----------
if 0 < args.decimate < 1.0:
    mod = body.modifiers.new("Decimate", "DECIMATE")
    mod.ratio = args.decimate
    bpy.context.view_layer.objects.active = body
    bpy.ops.object.modifier_apply(modifier=mod.name)
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
views = {"front": (size*1.6, 0, ctr.z + size*0.15), "side": (0, -size*1.6, ctr.z + size*0.15),
         "three_quarter": (size*1.2, -size*1.2, size*0.55), "low_rear": (-size*1.2, -size*0.9, size*0.25)}
for vname, pos in views.items():
    cam.location = pos
    d = Vector(ctr) - cam.location
    cam.rotation_euler = d.to_track_quat("-Z", "Y").to_euler()
    scene.render.filepath = out(f"{NAME}_preview_{vname}.png")
    bpy.ops.render.render(write_still=True)
report["previews"] = list(views.keys())

# ---------- final report ----------
with open(out(f"{NAME}_prep_report.json"), "w") as f:
    json.dump(report, f, indent=1)
print("PREP OK:", json.dumps(report, indent=1))
