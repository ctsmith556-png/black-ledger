# Headless tests for wheel_solve (no bpy).
#   python tools/blender/test_wheel_solve.py
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import math, random
from wheel_solve import consolidate_axles, build_extra_axles, cluster_axles_1d, ransac_circle

def W(x, y, r, w, cz, zone_pts, fit_ok=False):
    return {"x": x, "y": y, "r": r, "w": w, "cz": cz, "fit_ok": fit_ok, "zone_pts": zone_pts}

def approx(a, b, t=1e-6): return abs(a - b) <= t

passed = 0
def check(cond, msg):
    global passed
    assert cond, "FAIL: " + msg
    passed += 1
    print("  ok:", msg)

# ---- 1) Bride: weak/garbage front axle, reliable rear -> front repaired by mirroring rear
print("[1] Bride-like narrow-nose limo (front detection failed)")
bride = {
    "FL": W(3.0, 1.788, 0.30, 0.26, 0.30, zone_pts=136),   # barely any verts
    "FR": W(2.0, -1.788, 0.20, 0.22, 0.20, zone_pts=0),     # none at all (the bug)
    "RL": W(-3.76, 1.788, 0.518, 0.40, 0.518, zone_pts=1650),
    "RR": W(-3.75, -1.788, 0.518, 0.40, 0.518, zone_pts=1237),
}
consolidate_axles(bride, ext_x=8.881)
check(bride["FL"].get("repaired") and bride["FR"].get("repaired"), "front axle was repaired")
check(not bride["RL"].get("repaired"), "rear axle (reliable) left intact")
check(approx(bride["FL"]["r"], 0.518) and approx(bride["FL"]["w"], 0.40),
      "front wheel radius/width copied from rear (0.518 / 0.40, not 0.20/0.22)")
check(approx(bride["FL"]["x"], bride["FR"]["x"]) and bride["FL"]["x"] > 0,
      "front wheels symmetric and ahead of center (x>0)")
check(approx(bride["FL"]["x"], 3.755, 1e-3), "front X = mirrored rear overhang (+3.755)")
check(approx(bride["FL"]["y"], 1.788) and approx(bride["FR"]["y"], -1.788),
      "front track copied from rear (+/-1.788)")
wb = abs(bride["FL"]["x"] - bride["RL"]["x"])
check(approx(wb, 7.51, 1e-2), "resulting wheelbase sane (~7.51 m)")

# ---- 2) Normal vehicle (Surgeon-like): all axles reliable -> NO repair, just symmetry
print("[2] Surgeon-like (all axles detected) -> unchanged behaviour")
surg = {
    "FL": W(2.00, 1.091, 0.408, 0.40, 0.408, zone_pts=2841),
    "FR": W(2.02, -1.091, 0.408, 0.40, 0.408, zone_pts=3023),
    "RL": W(-1.733, 1.091, 0.374, 0.40, 0.374, zone_pts=3003),
    "RR": W(-1.733, -1.091, 0.374, 0.40, 0.374, zone_pts=2651),
}
consolidate_axles(surg, ext_x=6.502)
check(not any(surg[t].get("repaired") for t in surg), "no axle repaired (all reliable)")
check(approx(surg["FL"]["x"], surg["FR"]["x"]) and approx(surg["FL"]["x"], 2.01),
      "front L/R symmetry averaged (2.00,2.02 -> 2.01)")
check(approx(surg["RL"]["r"], 0.374) and approx(surg["FL"]["r"], 0.408),
      "front/rear radii preserved (no cross-axle bleed)")

# ---- 3) Explicit overrides win
print("[3] Explicit overrides")
ov = {
    "FL": W(2.00, 1.0, 0.40, 0.40, 0.40, zone_pts=2000),
    "FR": W(2.00, -1.0, 0.40, 0.40, 0.40, zone_pts=2000),
    "RL": W(-1.7, 1.0, 0.40, 0.40, 0.40, zone_pts=2000),
    "RR": W(-1.7, -1.0, 0.40, 0.40, 0.40, zone_pts=2000),
}
consolidate_axles(ov, ext_x=6.0, front_axle_x=1.9, rear_axle_x=1.6,
                  wheel_y=1.15, wheel_radius=0.42, wheel_width=0.38)
check(approx(ov["FL"]["x"], 1.9) and approx(ov["RL"]["x"], -1.6), "axle X overrides applied (+1.9 / -1.6)")
check(all(approx(abs(ov[t]["y"]), 1.15) for t in ov), "track override applied (|y|=1.15)")
check(all(approx(ov[t]["r"], 0.42) and approx(ov[t]["w"], 0.38) for t in ov), "radius/width overrides applied")

# ---- 4) Weak REAR axle also repairable (symmetry of the fix)
print("[4] Weak rear axle mirrors the front")
rr = {
    "FL": W(1.9, 1.0, 0.36, 0.34, 0.36, zone_pts=1800),
    "FR": W(1.9, -1.0, 0.36, 0.34, 0.36, zone_pts=1700),
    "RL": W(-1.0, 1.0, 0.20, 0.22, 0.20, zone_pts=20),
    "RR": W(-1.5, -1.0, 0.25, 0.24, 0.25, zone_pts=5),
}
consolidate_axles(rr, ext_x=5.8)
check(rr["RL"].get("repaired") and not rr["FL"].get("repaired"), "rear repaired from front")
check(approx(rr["RL"]["r"], 0.36) and rr["RL"]["x"] < 0, "rear took front radius and a negative X")

# ---- 5) extra cosmetic axles for six-wheelers
print("[5] build_extra_axles (six-wheeler middle axle)")
tmpl = {"x": -3.41, "y": 1.667, "r": 0.52, "w": 0.45, "cz": 0.52}   # Shepherd-like rear
ex = build_extra_axles([-2.0], tmpl)
check(set(ex) == {"M0L", "M0R"}, "one mid axle -> M0L + M0R")
check(approx(ex["M0L"]["x"], -2.0) and approx(ex["M0R"]["x"], -2.0), "mid wheels at requested X")
check(approx(ex["M0L"]["y"], 1.667) and approx(ex["M0R"]["y"], -1.667), "mid wheels mirror L/R at rear track")
check(approx(ex["M0L"]["r"], 0.52) and approx(ex["M0L"]["w"], 0.45), "mid wheels inherit rear radius/width")
check(build_extra_axles([], tmpl) == {}, "no mid axles when list empty (protects the 13 four-wheelers)")
check(set(build_extra_axles([-1.0, 0.5], tmpl)) == {"M0L", "M0R", "M1L", "M1R"}, "two mid axles -> 4 wheels")

# ---- 6) k-means axle clustering (--axles mode)
print("[6] cluster_axles_1d (multi-axle detection)")
def _blob(center, n, sd=0.13):
    return [center + random.gauss(0, sd) for _ in range(n)]
random.seed(7)
# front + rear tandem (Hemlock/Refuse/Lien/Shepherd shape)
tand = cluster_axles_1d(_blob(2.8, 300) + _blob(-1.8, 260) + _blob(-3.0, 240), 3)
check(abs(tand[0] - 2.8) < 0.2 and abs(tand[1] + 1.8) < 0.3 and abs(tand[2] + 3.0) < 0.3,
      "front + rear tandem -> ~[2.8, -1.8, -3.0] front-to-rear")
# evenly-spaced 3-axle (Crucible shape)
even = cluster_axles_1d(_blob(2.7, 200) + _blob(0.0, 200) + _blob(-2.7, 200), 3)
check(abs(even[0] - 2.7) < 0.25 and abs(even[1]) < 0.25 and abs(even[2] + 2.7) < 0.25,
      "even 3-axle -> ~[2.7, 0, -2.7]")
# plain 2-axle
two = cluster_axles_1d(_blob(2.0, 200) + _blob(-2.0, 200), 2)
check(abs(two[0] - 2.0) < 0.2 and abs(two[1] + 2.0) < 0.2, "2-axle -> ~[2.0, -2.0]")
check(cluster_axles_1d([], 3) == [0.0, 0.0, 0.0], "empty input is safe")
check(cluster_axles_1d(_blob(1.0, 50), 1) and len(cluster_axles_1d(_blob(1.0, 50), 2)) == 2,
      "k=1 and k=2 return k centers")

# ---- 7) RANSAC circle lock (centers a wheel on its tire despite pipe/chassis clutter)
print("[7] ransac_circle (well-centering through clutter)")
random.seed(3)
for tx, tr in [(2.0, 0.5), (-3.1, 0.45), (0.4, 0.55), (-1.85, 0.5)]:
    tire = [(tx + tr*math.cos(a := random.uniform(0, 2*math.pi)) + random.gauss(0, 0.01),
             tr + tr*math.sin(a) + random.gauss(0, 0.01)) for _ in range(220)]
    clutter = [(random.uniform(tx-1.0, tx+1.2), 0.95 + random.gauss(0, 0.05)) for _ in range(160)]
    clutter += [(tx-0.8 + random.gauss(0, 0.25), 0.3 + random.gauss(0, 0.12)) for _ in range(110)]
    cx, cz, r = ransac_circle(tire + clutter)
    check(abs(cx - tx) < 0.08 and abs(r - tr) < 0.08,
          f"axle@{tx:+.2f}: RANSAC center {cx:+.2f} r{r:.2f} (clutter {len(clutter)} > tire 220)")
check(ransac_circle([(0.0, 0.0)] * 3) is not None or True, "tiny input is safe (no crash)")

print("\nALL %d CHECKS PASSED" % passed)
