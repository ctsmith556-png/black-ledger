# Arena Index — build briefs, scale targets, quick numbers

One **Arena Build Brief** (.docx) per arena lives in this folder — each contains the
environment rendering, arena description + tone, the recommended 14-vehicle footprint,
Unreal scale notes (1 m = 100 UU), lane/room widths, hazard + destructible specs, and
greybox priorities. **Open the brief before building or resizing any arena map.**
The blockout layouts (`png/`, `svg/`) remain the source for landmark placement; the
briefs are the source for SCALE, TONE, and SYSTEM behavior.

Per the package README: sizes are first-pass greybox targets — tune after drive-feel
passes, never before reading the brief.

## Scale doctrine (Chris, June 12 — drive-tested on The Mill)

**All arenas build at 2.0× the brief footprint (linear).** Drive-tested at 1.0× and
2.0×: the doubled scale keeps fights frequent while giving room to free-roam and
disengage from the chaos — that's the intended feel for every map. The briefs' numbers
stay authoritative for PROPORTIONS, lane widths, landmark layout, density tiers, and
tone; multiply footprints by 2 when generating. Each `make_<arena>_map.py` carries a
`SCALE` constant (2.0 default) as the per-map tuning knob after drive passes.

Coupled systems to revisit as more 2× arenas come online: AI dispersion
(`EngagementRange` + ROAM) so the field uses the space; weapon/AI range constants if
the largest maps (Highway 6, Switchyard at ~1 km long-axis) feel under-gunned; navmesh
invoker-based generation if whole-bounds dynamic tiles get expensive.

## Scale rulers (apply to every arena)

- 1 m = 100 UU. Top speed 95 kph ≈ 26 m/s → traversal seconds ≈ longest-axis-m / 26.
- MG effective range 90 m · homing lock 200 m · typical TTK ~13 s ≈ 350 m of driving.
- Lane minimums come from each brief's "Lane / Room Widths" row — do not eyeball them.
  Lane widths do NOT scale with the 2× doctrine; they're sized to the vehicles.

## Quick reference (from the briefs)

| § | Arena | Brief | Brief (m) | **Build 2× (m)** | Traverse | Pickups | Signature hazard | Boss |
|---|-------|-------|-----------|------------------|----------|---------|------------------|------|
| 5.1 | The Mill | `01_The_Mill_Arena_Build_Brief.docx` | 260 × 220 | **520 × 440** | ~20 s | High (12) | Furnace pours ~90 s (all 3 furnaces) | Foundryman "Tap" |
| 5.2 | Ashen Hollow Downtown | `02_Ashen_Hollow_Downtown_Arena_Build_Brief.docx` | 360 × 360 | **720 × 720** | ~28 s | Med (8) | Sinkholes (one per match) | Curfew |
| 5.3 | The Tenements | `03_The_Tenements_Arena_Build_Brief.docx` | 300 × 280 | **600 × 560** | ~23 s | Low (6) | Falling debris ~30 s | Condemned |
| 5.4 | Highway 6 Overpass | `04_Highway_6_Overpass_Arena_Build_Brief.docx` | 520 × 170 /deck | **1040 × 340** | ~40 s | Med (8) | Wind gusts 45–60 s | Jackknife |
| 5.5 | The Cemetery | `05_The_Cemetery_Arena_Build_Brief.docx` | 340 × 300 | **680 × 600** | ~26 s | Med (10) | Fog 20 s / ~90 s | The Sexton |
| 5.6 | The Auditor's Office | `06_The_Auditor_s_Office_Arena_Build_Brief.docx` | 260 × 220 | **520 × 440** (shifting) | ~20 s | High (12, shifts) | Reality glitches ~60 s | THE AUDITOR |
| 5.7 | The Drowned Quarter | `07_The_Drowned_Quarter_Arena_Build_Brief.docx` | 380 × 320 | **760 × 640** | ~29 s | Med (8) | Tidal surge ~75 s | Undertow |
| 5.8 | The Switchyard | `08_The_Switchyard_Arena_Build_Brief.docx` | 480 × 260 | **960 × 520** | ~37 s | Med-High (10) | Ghost Train ~60 s | Lodestone |
| 5.9 | Pinewood Drive-In | `09_Pinewood_Drive_In_Fairground_Arena_Build_Brief.docx` | 420 × 340 | **840 × 680** | ~32 s | High (12) | Rides wake up ~80 s | The Headliner |
| 5.10 | Saint Dymphna's | `10_Saint_Dymphna_s_Arena_Build_Brief.docx` | 290 × 250 | **580 × 500** | ~22 s | Low (6) | Arc & collapse ~50 s | The Alienist |

(Traverse = 2× longest axis at top speed; destructible lists are in
`ARENA_REFERENCE.pdf` and each brief.)

## Build loop per arena

1. Read the brief (tone, footprint, lanes, hazard timing) + the blockout (landmarks).
2. Greybox via a `tools/unreal/make_<arena>_map.py` script (pattern: `make_mill_map.py`),
   parameterized on one scale constant; spawn 14 FFA nodes per the brief (no spawn-kill
   sightlines), pickups per the density tier.
3. Drive-time pass: lap it, time the pockets, tune the constant. Brief priorities:
   oversized hazard tells first, simple collision until driving feel is locked.
4. Combat pass: hazard (`ABLHazardActor` subclass), 3 destructibles
   (`ABLDestructibleFeature` subclasses), pickups, nav bounds (`ABLNavBounds` per zone).
5. Boss, then art pass — only after the layout has survived play.

## Status (June 2026)

`L_Mill` generates at **520 × 440 m** (`SCALE = 2.0`, drive-approved June 12): 12 m
catwalk lane, 12+3 pickups in conflict lanes, 14 `BLSpawn` FFA nodes in the four start
zones, ladle landmark, 40 m backdrop apron, **pour hazards on all three furnaces**
(staggered ~20 s / ~65 s / ~90 s; the Foundryman takes over F3's pour when he lands).
