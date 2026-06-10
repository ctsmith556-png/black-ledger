# Black Ledger - pure-Python wheel axle consolidation (no bpy; unit-testable).
# Used by prep_vehicle.py after per-corner wheel detection. Kept bpy-free so the
# logic can be tested headless:  python tools/blender/test_wheel_solve.py
#
# Input `wheels` is {tag: {x,y,r,w,cz,fit_ok,zone_pts}} with plain floats, where
# tag in FL/FR/RL/RR, x = longitudinal (front +), y = lateral, r = radius,
# w = width, cz = axle height. `ext_x` is the body length (m) at detection time.

# An axle whose *better* corner gathered fewer than this many zone verts is
# considered unreliable (e.g. a stretch-limo nose narrower than its widest point,
# where the outer-band detector finds almost nothing). Bride's front came in at
# FL=136 / FR=0; every healthy axle in the roster is >=600.
import math

WEAK_ZONE_PTS = 250


def axle_strength(wheels, a, b):
    """How trustworthy an axle's detection was = verts found at its stronger corner."""
    return max(wheels[a]["zone_pts"], wheels[b]["zone_pts"])


def consolidate_axles(wheels, ext_x,
                      front_axle_x=0.0, rear_axle_x=0.0, wheel_y=0.0,
                      wheel_radius=0.0, wheel_width=0.0):
    """Mutate + return `wheels`:
      1. enforce left/right symmetry per axle (average each pair),
      2. repair a weak axle by mirroring the reliable axle (radius/width/track,
         and a mirrored overhang for X) so no wheel is ever orphaned,
      3. apply explicit overrides (deterministic escape hatch).
    Pure float math; safe to import without Blender."""
    # 1) left/right symmetry per axle
    for a, b in (("FL", "FR"), ("RL", "RR")):
        for key in ("x", "r", "w", "cz"):
            avg = (wheels[a][key] + wheels[b][key]) / 2.0
            wheels[a][key] = wheels[b][key] = avg

    # 2) repair a weak axle from the reliable one
    front_s = axle_strength(wheels, "FL", "FR")
    rear_s = axle_strength(wheels, "RL", "RR")
    half = ext_x / 2.0

    def mirror(dst_pair, src_pair, dst_sign):
        src = src_pair[0]
        overhang = half - abs(wheels[src]["x"])      # reliable axle's overhang from its end
        x = dst_sign * (half - overhang)             # same overhang at the opposite end
        for d in dst_pair:
            wheels[d]["x"] = x
            wheels[d]["r"] = wheels[src]["r"]
            wheels[d]["w"] = wheels[src]["w"]
            wheels[d]["cz"] = wheels[src]["cz"]
            wheels[d]["y"] = (1.0 if wheels[d]["y"] > 0 else -1.0) * abs(wheels[src]["y"])
            wheels[d]["repaired"] = True

    if front_s < WEAK_ZONE_PTS <= rear_s:
        mirror(("FL", "FR"), ("RL", "RR"), +1)
    elif rear_s < WEAK_ZONE_PTS <= front_s:
        mirror(("RL", "RR"), ("FL", "FR"), -1)

    # 3) explicit overrides (front is +X, rear is -X by convention)
    if front_axle_x:
        for f in ("FL", "FR"):
            wheels[f]["x"] = abs(front_axle_x)
    if rear_axle_x:
        for f in ("RL", "RR"):
            wheels[f]["x"] = -abs(rear_axle_x)
    if wheel_y > 0:
        for f in wheels:
            wheels[f]["y"] = (1.0 if wheels[f]["y"] > 0 else -1.0) * wheel_y
    if wheel_radius > 0:
        for f in wheels:
            wheels[f]["r"] = wheel_radius
    if wheel_width > 0:
        for f in wheels:
            wheels[f]["w"] = wheel_width
    return wheels


def build_extra_axles(mid_xs, template):
    """Cosmetic extra axles for 3+ axle vehicles (the six-wheelers).

    `mid_xs` is a list of longitudinal X positions (m, same frame as the detected
    wheels). Each one becomes a left+right wheel that inherits the `template`
    axle's radius / width / track (|y|) / height (cz) -- extra axles match the
    rear. Returns {tag: {x,y,r,w,cz}} with tags M0L/M0R, M1L/M1R, ...  These are
    carved + lined like the corners, then merged into the body (static/cosmetic;
    the 4 raycast corners stay separate)."""
    extra = {}
    for i, x in enumerate(mid_xs):
        for sgn, lr in ((1.0, "L"), (-1.0, "R")):
            extra[f"M{i}{lr}"] = {
                "x": float(x),
                "y": sgn * abs(template["y"]),
                "r": template["r"],
                "w": template["w"],
                "cz": template["cz"],
            }
    return extra


def cluster_axles_1d(xs, k, iters=60):
    """1D k-means on wheel-vert X coordinates -> k axle centers, sorted front (+X)
    to rear. Used by `--axles N` mode to locate 2/3+ axles by clustering the
    outer-lower wheel geometry, instead of the fragile front/rear circle-fit that
    collapses on tandem/3-axle trucks. Pure; unit-tested headless."""
    xs = sorted(float(x) for x in xs)
    n = len(xs)
    if k < 1:
        return []
    if n == 0:
        return [0.0] * k
    if n <= k:
        return sorted(xs + [xs[-1]] * (k - n), reverse=True)
    # quantile init keeps the centers separated so k-means converges to the axles
    centers = [xs[min(n - 1, int((i + 0.5) * n / k))] for i in range(k)]
    for _ in range(iters):
        groups = [[] for _ in range(k)]
        for x in xs:
            ci = min(range(k), key=lambda c: abs(x - centers[c]))
            groups[ci].append(x)
        moved = 0.0
        for i in range(k):
            if groups[i]:
                nc = sum(groups[i]) / len(groups[i])
                moved = max(moved, abs(nc - centers[i]))
                centers[i] = nc
        if moved < 1e-5:
            break
    return sorted(centers, reverse=True)


def _circumcircle(p):
    """Circle through 3 (x,z) points -> (cx, cz, r) or None if collinear."""
    (ax, ay), (bx, by), (cx, cy) = p
    d = 2 * (ax * (by - cy) + bx * (cy - ay) + cx * (ay - by))
    if abs(d) < 1e-9:
        return None
    ux = ((ax * ax + ay * ay) * (by - cy) + (bx * bx + by * by) * (cy - ay) + (cx * cx + cy * cy) * (ay - by)) / d
    uy = ((ax * ax + ay * ay) * (cx - bx) + (bx * bx + by * by) * (ax - cx) + (cx * cx + cy * cy) * (bx - ax)) / d
    return ux, uy, math.hypot(ux - ax, uy - ay)


def _lsq_circle(pts):
    """Least-squares circle through (x,z) points -> (cx, cz, r) or None."""
    n = len(pts)
    if n < 3:
        return None
    ma = sum(p[0] for p in pts) / n
    mb = sum(p[1] for p in pts) / n
    suu = svv = suv = suuu = svvv = suvv = svuu = 0.0
    for a, b in pts:
        u, v = a - ma, b - mb
        suu += u * u; svv += v * v; suv += u * v
        suuu += u ** 3; svvv += v ** 3; suvv += u * v * v; svuu += v * u * u
    det = suu * svv - suv * suv
    if abs(det) < 1e-12:
        return None
    uc = (svv * (suuu + suvv) - suv * (svvv + svuu)) / (2 * det)
    vc = (suu * (svvv + svuu) - suv * (suuu + suvv)) / (2 * det)
    return ma + uc, mb + vc, math.sqrt(uc * uc + vc * vc + (suu + svv) / n)


def ransac_circle(pts, iters=300, tol=0.04, rmin=0.2, rmax=0.7, seed=1):
    """Robust circle fit amid outliers (RANSAC). Finds the circle the most points
    agree on -> the round tire, rejecting pipes/booms/chassis clutter that a plain
    least-squares fit gets dragged toward. `pts` are (x, z) tuples. Deterministic
    (fixed seed). Returns (cx, cz, r) or None. Unit-tested headless."""
    import random as _random
    rng = _random.Random(seed)
    pts = [(float(a), float(b)) for a, b in pts]
    if len(pts) < 10:
        return _lsq_circle(pts)
    best = None
    best_in = []
    for _ in range(iters):
        c = _circumcircle(rng.sample(pts, 3))
        if not c or not (rmin < c[2] < rmax):
            continue
        cx, cz, r = c
        inl = [p for p in pts if abs(math.hypot(p[0] - cx, p[1] - cz) - r) < tol]
        if len(inl) > len(best_in):
            best_in, best = inl, c
    if len(best_in) >= 20:
        return _lsq_circle(best_in)
    return best or _lsq_circle(pts)
