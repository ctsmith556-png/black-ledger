# Black Ledger - measure wheel positions for --axle-fracs (run inside Blender).
#
# HOW TO USE:
#   1. File > Import > FBX  ->  the vehicle SOURCE fbx
#        e.g.  art\source\vehicles\09_Hemlock\Hemlock.fbx
#      (the source still has the original wheels sitting in the correct wells).
#   2. Press Numpad 1 or Numpad 3 for a straight orthographic side view.
#   3. For EACH wheel:  Shift+A > Empty > Plain Axes, then press G and slide the
#      empty onto the CENTER of that wheel (in side view; left/right depth doesn't
#      matter). One empty per axle. For a rear tandem, drop one on each of the two
#      close wheels. (Delete any other stray empties first.)
#   4. Switch to the Scripting tab, open this file, press the Run (triangle) button.
#   5. It prints two lines. Pick the one whose CAB/front end matches your model and
#      send that "--axle-fracs ..." string to Claude (or paste it into batch_prep.ps1).
#
# Frame-independent: it auto-detects the length axis and reports fractions both ways,
# so orientation / flip never matters.

import bpy

meshes = [o for o in bpy.context.scene.objects if o.type == 'MESH']
if not meshes:
    print("No mesh found - import the vehicle FBX first.")
else:
    ob = max(meshes, key=lambda o: len(o.data.vertices))
    cos = [ob.matrix_world @ v.co for v in ob.data.vertices]
    ext = [max(c[i] for c in cos) - min(c[i] for c in cos) for i in range(3)]
    ax = ext.index(max(ext))                                   # longest axis = length
    lo = min(c[ax] for c in cos)
    hi = max(c[ax] for c in cos)
    L = (hi - lo) or 1.0
    vals = [o.matrix_world.translation[ax]
            for o in bpy.context.scene.objects if o.type == 'EMPTY']
    if not vals:
        print("No empties found - drop one Empty at each wheel center, then run again.")
    else:
        f_lo = sorted(((v - lo) / L for v in vals), reverse=True)        # front = HIGH end
        f_hi = sorted((1.0 - (v - lo) / L for v in vals), reverse=True)  # front = LOW end
        join = lambda fs: ",".join("%.3f" % f for f in fs)
        line_hi = "cab at HIGH-%s end:  --axle-fracs %s" % ("XYZ"[ax], join(f_lo))
        line_lo = "cab at LOW-%s  end:  --axle-fracs %s" % ("XYZ"[ax], join(f_hi))
        msg = "%d wheels, length axis %s\n%s\n%s" % (len(vals), "XYZ"[ax], line_hi, line_lo)
        print("=" * 60); print(msg); print("=" * 60)
        # copy to clipboard + show a popup so you don't need the system console
        bpy.context.window_manager.clipboard = msg
        def _draw(self, ctx):
            for ln in msg.split("\n"):
                self.layout.label(text=ln)
            self.layout.label(text="(copied to clipboard - paste to Claude)")
        bpy.context.window_manager.popup_menu(_draw, title="Axle fractions", icon='INFO')
