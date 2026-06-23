# Black Ledger - inspect the Foundryman prepped mesh so we can pick the tread Z-band
# before splitting the tread faces into their own material slot.
#
# Run headless (Blender only; UE editor can stay open):
#   & "C:\Program Files\Blender Foundation\Blender 4.x\blender.exe" --background `
#     --python tools\blender\inspect_foundryman_treads.py
#
# Prints overall bounds, a Z histogram of face centers (20 bins), and for the lowest
# ~18% of height: the Y/X extent + face count (the candidate "tread" band).

import bpy, os

FBX = os.path.abspath(os.path.join(
    os.path.dirname(__file__), "..", "..",
    "art", "source", "vehicles", "B1_Foundryman", "Foundryman_UE.fbx"))


def log(msg):
    print("INSPECT: " + msg)


def main():
    bpy.ops.wm.read_factory_settings(use_empty=True)
    if not os.path.exists(FBX):
        log("FBX NOT FOUND: " + FBX)
        return
    bpy.ops.import_scene.fbx(filepath=FBX)

    objs = [o for o in bpy.context.scene.objects if o.type == "MESH"]
    log("mesh objects: %d -> %s" % (len(objs), [o.name for o in objs]))
    if not objs:
        return
    obj = objs[0]
    me = obj.data
    log("object '%s': %d verts, %d polys, %d material slot(s)"
        % (obj.name, len(me.vertices), len(me.polygons), len(obj.material_slots)))
    for i, s in enumerate(obj.material_slots):
        log("  slot %d: %s" % (i, s.material.name if s.material else "None"))

    # world-space bounds
    mw = obj.matrix_world
    cos = [mw @ v.co for v in me.vertices]
    xs = [c.x for c in cos]; ys = [c.y for c in cos]; zs = [c.z for c in cos]
    minz, maxz = min(zs), max(zs)
    log("bounds X[%.3f,%.3f] Y[%.3f,%.3f] Z[%.3f,%.3f]  (units; ~meters)"
        % (min(xs), max(xs), min(ys), max(ys), minz, maxz))
    H = maxz - minz

    # Z histogram of face centers
    NB = 20
    bins = [0] * NB
    for p in me.polygons:
        cz = (mw @ p.center).z
        b = min(NB - 1, int((cz - minz) / max(H, 1e-6) * NB))
        bins[b] += 1
    log("Z histogram of face centers (low->high, %d bins over %.3f tall):" % (NB, H))
    for i, c in enumerate(bins):
        z0 = minz + H * i / NB
        log("  bin %2d  z>=%.3f : %s (%d)" % (i, z0, "#" * (c // 40), c))

    # candidate tread band = lowest 18% of height
    cut = minz + H * 0.18
    tx = []; ty = []; n = 0
    for p in me.polygons:
        if (mw @ p.center).z <= cut:
            c = mw @ p.center
            tx.append(c.x); ty.append(c.y); n += 1
    if n:
        log("LOW BAND (z<=%.3f, %.0f%% of height): %d faces, X[%.2f,%.2f] Y[%.2f,%.2f]"
            % (cut, 18, n, min(tx), max(tx), min(ty), max(ty)))
        log("  -> Y extent of low band tells us if tracks are at outer |Y| or full-width belly")


main()
