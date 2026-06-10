# Black Ledger — Vehicle QA Review (all 16 launch models)

**Date:** 2026-06-08 · **Reviewer:** Cowork pass · **Scope:** the 16 prepped/imported launch vehicles (Ref_01–16)
**Sources inspected:** `art/source/vehicles/<NN_Name>/` — prepped `*_UE.fbx`, the 8 preview renders per vehicle, `*_prep_report.json` (hard geometry), the 6 PBR maps; plus the in-engine `Content/BlackLedger/Characters/<Name>/`.
**Contact sheets** (what this review looked at) live in `docs/qa/sheets/` — one `cs_<Name>.png` per vehicle (4 hero views + 4 wheel-well close-ups) plus `grid_front/side/three_quarter/basecolor.png`.

> **Method note / caveat:** preview renders are untextured **clay**, so geometry, scale, proportion, wheel fit and facing were judged in 3D; **material/texture** was judged from the source PBR atlases (`grid_basecolor.png`), not yet in-engine under lighting. A final pass with materials applied, in the editor, is still worth doing — see Action 6.

---

## Verdict

**14 of 16 are in good shape** — coherent forms, on-theme silhouettes, sane scale, clean wheel wells, complete texture sets, and all correctly facing **+X**. There is **one true defect that should be fixed before the model is usable (Bride)**, one **scale outlier to confirm (Shepherd)**, and a short list of cosmetic / known-deferred items.

| Severity | Count | Vehicles |
|---|---|---|
| **High — fix before use** | 1 | Bride (front axle detached, no front wheel wells) |
| **Medium — decide / schedule** | 2 | Shepherd (oversized + bare middle axle), the 6-wheeler middle axles (Crucible, Shepherd, Bride) |
| **Low — cosmetic / known** | 3 | Pup (lopsided wheel-well carve), Surgeon (front/rear wheel-radius mismatch), wide-body arena-fit check |
| **Clean** | 13 | Surgeon, Antoinette, Hollow, Warden, Cartographer, Lien, Hemlock, Specter, Refuse, Hunter, Photographer, Vault, Pup* |

\* Pup is clean to the eye; its only issue is a cosmetic carve asymmetry that doesn't show in render.

> **Fix status — updated 2026-06-08.** The two priority items are resolved in the prep code (`tools/blender/prep_vehicle.py` + new `tools/blender/wheel_solve.py`) and staged for re-prep on the workstation:
> - **Bride (HIGH)** — the prep now auto-repairs a weak axle by mirroring the reliable one, so the orphaned front wheels inherit the rear's radius/width/track and a mirrored overhang. No floating front axle. Logic is unit-tested in `tools/blender/test_wheel_solve.py` (15 checks pass).
> - **Shepherd (MEDIUM)** — new `--width`/`--height` non-uniform scale; roster set to **11.0 × 2.55 × 3.2 m** (true bus proportions) with a pinned 0.52 m wheel radius.
> - **Middle axle on the three six-wheelers (Crucible, Shepherd, Bride)** — new opt-in `--mid-axle-x` installs the 3rd axle's wheels (carved + lined like the corners, which also clears the fused original Meshy wheel, then merged into the body as cosmetic/static geometry — no pawn change). Starting X estimates are Crucible −0.6 / Shepherd −2.0 / Bride −2.3 and **need an in-editor eyeball + nudge**, since exact positions weren't verifiable headless.
> Run with the editor closed: `batch_prep.ps1 -Only Bride,Shepherd,Crucible` → `batch_import.ps1 -Only Bride,Shepherd,Crucible`. Changes are backward-compatible — the axle repair only fires on a starved axle (every healthy axle has ≥680 zone verts vs the 250 threshold; Bride's front had 136/0), and non-uniform scale + extra axles only trigger when their flags are passed, so the other 13 vehicles are untouched.

---

## Summary table

Dimensions are the **final prepped extents** (metres, L×W×H) from each `_prep_report.json`. Body tris exclude the 4 combat wheels (+~4.5k). Real-world references: a US car ≈ 4.5–5.3 m long / 1.9 m wide; a legal truck ≈ 2.6 m wide / 4.1 m tall.

| # | Vehicle | Reads as | L × W × H (m) | Wheelbase | Body tris | 6-wh | Status |
|---|---|---|---|---|---|---|---|
| 01 | Surgeon | Armored ambulance | 6.50 × 2.54 × 2.68 | 3.74 | 19.2k | — | ✅ Clean (reference) |
| 02 | Antoinette | '50s convertible, fins | 5.60 × 2.15 × 1.38 | 3.08 | 22.9k | — | ✅ Clean · pink palette to confirm |
| 03 | Hollow | Fire engine | 9.00 × 3.43 × 3.31 | 4.95 | 28.7k | — | ✅ Clean · wide (3.43) |
| 04 | Warden | Armored riot van | 6.50 × 2.97 × 2.88 | 4.08 | 26.8k | — | ✅ Clean |
| 05 | Pup | Delivery/mail van | 5.80 × 2.79 × 2.67 | 3.73 | 27.9k | — | ✅ Clean · L/R carve asymmetry (cosmetic) |
| 06 | Crucible | Foundry hauler | 8.50 × 2.88 × 3.10 | 3.82 | 28.5k | **6** | ⚠️ Bare middle axle |
| 07 | Cartographer | Station wagon + roof rack | 5.60 × 2.07 × 1.69 | 3.10 | 24.4k | — | ✅ Clean |
| 08 | Lien | Tow truck / wrecker | 7.50 × 2.83 × 3.02 | 3.95 | 27.1k | — | ✅ Clean |
| 09 | Hemlock | Chemical tanker | 8.48 × 3.09 × 2.91 | 4.10 | 29.0k | — | ✅ Clean |
| 10 | Shepherd | Armored bus | 11.00 × 3.87 × 4.13 | 7.30 | 29.3k | **6** | ⚠️ Oversized + bare middle axle |
| 11 | Specter | Comms/equipment truck | 7.00 × 2.66 × 3.09 | 4.28 | 24.3k | — | ✅ Clean |
| 12 | Refuse | Garbage truck | 9.00 × 3.16 × 3.59 | 5.17 | 28.1k | — | ✅ Clean |
| 13 | **Bride** | Stretched limo / hearse | 8.88 × 3.82 × 1.99 | 6.90 | 28.3k | **6** | 🛑 **Front axle detached — fix** |
| 14 | Hunter | Engine-blown pickup | 5.80 × 2.53 × 2.18 | 3.29 | 23.5k | — | ✅ Clean |
| 15 | Photographer | Camera-laden lowrider | 5.40 × 1.95 × 1.47 | 3.17 | 26.7k | — | ✅ Clean · thin camera props |
| 16 | Vault | Armored ram truck | 6.80 × 3.27 × 3.12 | 3.25 | 26.9k | — | ✅ Clean · long ram hood (intentional) |

---

## Findings by severity

### 🛑 HIGH — Bride: front axle detached, no front wheel wells

Bride is the one model that is currently **broken**. Both the numbers and the renders agree:

- In `Bride_prep_report.json` the **front carve removed 0 faces** on both sides (`"FL: removed 0 faces"`, `"FR: removed 0 faces"`) and the well-fit found essentially no body geometry where the front wheels were placed (`zone_pts`: **FR = 0**, FL = 136, vs ~1,200–1,650 at the rear). The front wheels were also installed at the **wrong width** (0.22 m vs 0.40 m at the rear).
- In `cs_Bride.png` the front wheels visibly **float ahead of and below the chassis** with no fender, no well, and a large air-gap to the body — see the `arch_FL` / `arch_FR` close-ups (wheel pressed against a flat, un-carved panel).

Root cause: the auto wheelbase/axle detection put the **front axle outside the body envelope** (Bride's front section is the narrow limo nose, well behind where the axle landed), so there was nothing to carve a well into and the wheels ended up orphaned. This needs a corrected front-axle X (and width) and a re-prep + re-import. **Until fixed, Bride should not go on the selectable roster.**

### ⚠️ MEDIUM — Shepherd: scale outlier

Shepherd is the largest by a wide margin at **11.0 × 3.87 × 4.13 m**. Length is bus-appropriate, but it is **wider and taller than a real bus** (≈3.2 m tall / 2.5 m wide) and is the only model that exceeds the legal-truck envelope on both axes. The geometry itself is clean (coherent armored-bus form, no melts). This is a **design decision, not a defect** — but it's worth a conscious call: if it's meant to feel like a rolling fortress, keep it; if not, `prep_vehicle.py` scales uniformly to a target length, so dropping `--length` brings width/height down in proportion. Either way, confirm it fits the arenas (see below).

### ⚠️ MEDIUM — Six-wheelers only have 4 wheels (Crucible, Shepherd, Bride)

Known/deferred per `CLAUDE.md`: prep installs only front+rear pairs, so the **middle axle is bare** on all three six-wheelers. Visible in the side/three-quarter renders as an empty hub mid-span. Arcade movement only raycasts 4 corners, so this is **cosmetic**, but it reads as missing hardware up close. Schedule a middle-wheel pass for these three (Bride's belongs with its HIGH fix).

### 🔧 LOW — cosmetic & consistency

- **Pup — lopsided wheel-well carve.** The right-side wells carved far less than the left (`zone_pts` FR = 330 / RR = 459 vs FL = 3,149 / RL = 2,641). It does **not** show in render — Pup looks clean — but a re-prep would even it out if you're being thorough.
- **Surgeon — front/rear wheel-radius mismatch** (0.408 vs 0.374 m). The auto-fitter measured slightly different radii front vs rear; visually fine, but if you want uniform tyres, pin a single `--wheel-radius`.
- **`fit_ok = false` on nearly every wheel** across all 16 (only Surgeon FL came back true). The well-fit heuristic is clearly **over-strict** — the renders look correct — but because it's flagging everything it's no longer telling you anything. Worth a quick spot-check of **front-wheel ground contact in-engine** for Bride and Vault specifically, then either trust it or relax the threshold.
- **Wide bodies & arena fit.** Seven vehicles are ≥3.0 m wide (Hollow 3.43, Shepherd 3.87, Bride 3.82, Vault 3.27, Refuse 3.16, Hemlock 3.09, Warden ~2.97). Not a model flaw, but a **level-design constraint**: make sure doorways, lane gaps and the destructible chokepoints in the 10 arenas clear ~3.9 m at the widest. Cheaper to bake the minimum gap into the blockouts now than to re-tune later.

---

## Scale & proportion

Scale is **internally consistent and broadly sane**. The roster tiers cleanly: small cars 5.4–5.8 m (Antoinette, Cartographer, Photographer, Hunter), vans/ambulances 5.8–7.0 m (Pup, Surgeon, Warden, Specter), heavies 7.5–9.0 m (Lien, Crucible, Hemlock, Hollow, Refuse, Bride), and the Shepherd super-heavy at 11 m. The cars run a touch long and wide for their class (American land-yacht proportions), which suits the tone. The only genuine scale outlier is **Shepherd**; the only proportion defect is **Bride's** axle, covered above.

## Facing — all correct (carried-over check ✅)

The carried-over "verify front = +X" task **passes for all 16**. Each vehicle's `front` render shows a true front — grille, windshield, cab, or (Vault) the ram — never a tailgate or rear doors, which appear correctly in the separate `low_rear` renders. The auto-flip took on every model.

## Triangle budget — healthy

Every body is **19.2k–29.0k tris**, comfortably under the ~60k hero target (with wheels, totals land ~24k–34k). There's headroom; nothing needs decimating. Hemlock (29.0k) and Shepherd (29.3k) are the heaviest.

## Texture / material — complete and on-tone

All 16 carry the full PBR set (basecolor, normal, metallic, roughness, packed metallic_roughness, emissive) in source, and each imported character folder is uniform: **5 static meshes** (body + 4 wheels), **4 materials**, **5 textures**. No missing or corrupt maps. The base-color palette (`grid_basecolor.png`) is overwhelmingly **dark, rusted and grimy — squarely on the Silent Hill / Se7en dossier tone**. Two pop saturated and should be confirmed against their dossiers as intentional identity, not stray Meshy color: **Antoinette** (faded pageant pink) and **Crucible** (molten foundry orange — that one is the emissive doing its job). Meshy's busy baked-in panel detail is expected; real text/badges still come in as decals per the playbook.

---

## Recommended actions (prioritized)

1. **Fix Bride (HIGH).** Determine the correct front-axle X and width, re-prep and re-import:
   `powershell -ExecutionPolicy Bypass -File tools/blender/batch_prep.ps1 -Only Bride` (with corrected front-axle params), then with the **editor closed**, `tools/unreal/batch_import.ps1 -Only Bride`. Verify the front wheels sit in carved wells with no gap.
2. **Decide Shepherd's scale (MEDIUM).** Keep as a deliberate super-heavy, or re-prep with a smaller `--length` to pull W/H back toward bus-real.
3. **Middle-axle pass for the three six-wheelers (MEDIUM)** — Crucible, Shepherd, Bride — when you next touch the prep script.
4. **Arena-fit check (MEDIUM, level design).** Confirm the 10 blockouts clear ~3.9 m at the narrowest combat-relevant gap.
5. **Optional cosmetic re-preps (LOW):** Pup (even out L/R carve), Surgeon (uniform wheel radius).
6. **In-engine material pass (LOW but real):** drop all 16 into a lit scene, confirm textures/normals/emissive read correctly under the game's lighting, and spot-check Bride/Vault front-wheel ground contact. This closes the one gap in this review (clay renders couldn't show materials).
7. **Relax or retire the `fit_ok` heuristic** so it stops false-flagging every wheel.

## What was NOT in scope

Collision primitives, LODs, pivot/origin correctness under physics, in-engine material wiring under lighting, and the deferred Data-Asset stat wiring (the pawn still hardcodes Surgeon meshes). Those are best validated live in the editor.
