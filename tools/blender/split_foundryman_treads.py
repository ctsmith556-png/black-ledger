# Black Ledger - split the Foundryman's lower track band into a 2nd material slot so a
# scrolling tread material can animate the REAL geometry (the body is single-material).
#
# Selects faces whose center Z <= --tread-z, assigns them material index 1, renders a
# side PREVIEW (tracks = magenta) so we can verify the band, then re-exports
# Foundryman_UE.fbx with the prep's exact FBX settings (so it re-imports aligned).
#
# Run headless (Blender only):
#   & "C:\Program Files\Blender Foundation\Blender 4.x\blender.exe" --background `
#     --python tools\blender\split_foundryman_treads.py -- --tread-z 1.25
#
# Inspect the preview PNG; if the magenta covers the tracks (and NOT the boiler), proceed
# to re-import. If it grabs too much/little, re-run with a different --tread-z.

import bpy, os, sys, math, argparse
from mathutils import Vector

argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
ap = argparse.ArgumentParser()
ap.add_argument("--tread-z", type=float, default=1.25, help="faces with center Z <= this go to the tread slot")
ap.add_argument("--keep-x-min", type=float, default=-1e9, help="exclude faces with center X below this (drops the cab/rear end)")
ap.add_argument("--keep-x-max", type=float, default=1e9, help="exclude faces with center X above this")
ap.add_argument("--no-export", action="store_true", help="render the preview only, don't overwrite the FBX")
args = ap.parse_args(argv)

BASE = os.path.abspath(os.path.join(
    os.path.dirname(__file__), "..", "..", "art", "source", "vehicles", "B1_Foundryman"))
FBX = os.path.join(BASE, "Foundryman_UE.fbx")
PREVIEW = os.path.join(BASE, "Foundryman_tread_split_preview.png")


def log(m):
    print("SPLIT: " + m)


def main():
    bpy.ops.wm.read_factory_settings(use_empty=True)
    if not os.path.exists(FBX):
        log("FBX NOT FOUND: " + FBX)
        return
    bpy.ops.import_scene.fbx(filepath=FBX)
    obj = [o for o in bpy.context.scene.objects if o.type == "MESH"][0]
    me = obj.data

    # RESET to exactly two material slots every run (the imported FBX may already carry
    # leftover TreadMat slots from a previous split - appending would accumulate them).
    obj.data.materials.clear()
    m0 = bpy.data.materials.new("BodyMat")
    m0.diffuse_color = (0.55, 0.55, 0.55, 1.0)
    m1 = bpy.data.materials.new("TreadMat")
    m1.diffuse_color = (1.0, 0.0, 1.0, 1.0)
    obj.data.materials.append(m0)   # slot 0 = body
    obj.data.materials.append(m1)   # slot 1 = tread
    tread_idx = 1
    for p in me.polygons:           # everything starts on the body slot
        p.material_index = 0

    mw = obj.matrix_world
    n = 0
    sel_x = []
    for p in me.polygons:
        c = mw @ p.center
        if c.z <= args.tread_z and args.keep_x_min <= c.x <= args.keep_x_max:
            p.material_index = tread_idx
            n += 1
            sel_x.append(c.x)
    me.update()
    xr = "X[%.2f,%.2f]" % (min(sel_x), max(sel_x)) if sel_x else "X[none]"
    log("tread-z=%.3f keep-x[%.2f,%.2f] -> %d / %d faces to tread slot %d  %s"
        % (args.tread_z, args.keep_x_min, args.keep_x_max, n, len(me.polygons), tread_idx, xr))

    # ---- side preview render (workbench, material colours, headless-safe) ----
    scene = bpy.context.scene
    scene.render.engine = "BLENDER_WORKBENCH"
    scene.display.shading.light = "FLAT"
    scene.display.shading.color_type = "MATERIAL"
    scene.render.resolution_x = 1100
    scene.render.resolution_y = 460
    scene.render.film_transparent = False
    scene.render.filepath = PREVIEW

    cos = [mw @ v.co for v in me.vertices]
    mn = Vector((min(c.x for c in cos), min(c.y for c in cos), min(c.z for c in cos)))
    mx = Vector((max(c.x for c in cos), max(c.y for c in cos), max(c.z for c in cos)))
    ctr = (mn + mx) / 2
    size = max((mx - mn).length, 1.0)

    cam_data = bpy.data.cameras.new("Cam")
    cam = bpy.data.objects.new("Cam", cam_data)
    scene.collection.objects.link(cam)
    scene.camera = cam
    cam.location = (ctr.x, mn.y - size, ctr.z)
    d = Vector(ctr) - cam.location
    cam.rotation_euler = d.to_track_quat("-Z", "Y").to_euler()
    cam_data.type = "ORTHO"
    cam_data.ortho_scale = (mx.x - mn.x) * 1.08
    bpy.ops.render.render(write_still=True)
    log("preview -> " + PREVIEW)

    if args.no_export:
        log("no-export: FBX left unchanged")
        return

    bpy.ops.object.select_all(action="DESELECT")
    obj.select_set(True)
    bpy.context.view_layer.objects.active = obj
    bpy.ops.export_scene.fbx(
        filepath=FBX, use_selection=True, object_types={"MESH"},
        mesh_smooth_type="FACE", apply_scale_options="FBX_SCALE_NONE",
        bake_space_transform=True, add_leaf_bones=False)
    log("exported (2 material slots): " + FBX)


main()
