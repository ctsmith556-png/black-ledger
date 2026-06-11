# CLAUDE.md — Black Ledger

> Project context for Claude / Cowork sessions. Read this first, then open the specific docs it points to. Keep it updated as the project moves.
> **Place this file at the REPO ROOT** (`black-ledger/CLAUDE.md`) so any session opened in the project folder picks it up automatically.

---

## What this is

**Black Ledger** is a **single-player vehicular combat game** — Twisted Metal Black-inspired gameplay with a grim **Silent Hill / Se7en horror tone** (oppressive, psychological, not campy). Built by a **solo developer (Chris)** in **Unreal Engine 5.7**.

Framing: contestants with dark pasts are pulled into a deadly competition run by **"The Auditor."** Each vehicle is documented as an Auditor **case-file dossier** — that grim dossier look is the project's signature art style.

**Status (June 2026):** Pre-production complete; laptop set up; repo at `C:\Users\csmit\black-ledger` connected in Cowork. **All 16 Meshy vehicle models are committed** at `art/source/vehicles/` (FBX + PBR, ~1.7 GB LFS). **UE 5.7 project exists at the repo root** (`BlackLedger.uproject` + `Config/` + `Source/` + `Content/`, per `PROJECT_STRUCTURE.md`). The wizard produced a Blueprint project, so the C++ game module was added by hand. **Phase 1 complete (June 5, 2026): the Surgeon is drivable** — `ABLCombatVehicle` arcade pawn (raycast suspension, direct-yaw steering, handbrake drift, airborne auto-level), `UBLHealthComponent`, `ABLGameMode`, classic input bindings, and a scripted gym map `L_Gym`. **All 16 vehicles imported in-engine (June 8, 2026)** at `Content/BlackLedger/Characters/<Name>/` (body + 4 wheels + textures + materials) via the batch pipeline. **All 16 vehicles finalized + re-imported (June 10, 2026):** every wheel placed on **Blender-measured axle positions** (deterministic `--axle-fracs`), correct per-vehicle tire radii, and a **lateral wheel-track fix** so wheels seat in their wells instead of poking past the body. 15 of 16 are clean; **Bride is left on its older track by choice** (its cleaned source was reverted in a tooling mix-up, so re-prepping it would undo the manual blob/hood cleanup — its in-engine asset keeps the clean body, just slightly proud front wheels). The drivable pawn still spawns **only the Surgeon** (meshes + chassis numbers hard-coded); making the other 15 selectable is the next vehicle task (data-driven `UBLCharacterDataAsset`). Note: UBA is disabled on this laptop (`Saved/UnrealBuildTool/BuildConfiguration.xml`, machine-local) because its memory kill-threshold loops on a ~25 GB RAM machine.

---

## Core design facts

- **Roster:** 15 selectable launch characters + **Vault** (unlockable) = 16 launch vehicles. Plus **10 Collector bosses** that are overpowered and **become playable when defeated**. **The Auditor** is the final boss / ultimate unlock. **26 vehicles total.**
- **Each playable map has a boss fight** (challenging, Twisted-Metal style); beating a boss unlocks it.
- **10 arenas**, each with **2–3 destructible/interactive features** (Eiffel-Tower-collapse style).
- **14 pickup weapons.**
- **Stat budget:** Speed + Armor + Handling + Special Power = **24** for launch vehicles (bosses intentionally break this). Target time-to-kill ~**10–15s** standard.

**Launch vehicles (Ref_01–16):** 01 Surgeon, 02 Antoinette, 03 Hollow, 04 Warden, 05 Pup, 06 Crucible, 07 Cartographer, 08 Lien, 09 Hemlock *(eco-terrorism backstory)*, 10 Shepherd, 11 Specter, 12 Refuse, 13 Bride, 14 Hunter, 15 Photographer, 16 Vault *(unlockable)*.

**Bosses (Ref_B1–B10):** B1 Foundryman, B2 Curfew, B3 Condemned, B4 Jackknife, B5 Sexton, B6 Undertow, B7 Lodestone, B8 Headliner, B9 Alienist, B10 **Auditor** *(final)*.

**Arenas (10):** Ashen Hollow Downtown, The Tenements, Highway 6 Overpass, The Cemetery, The Auditor's Office, The Drowned Quarter, The Switchyard, Pinewood Drive-In, Saint Dymphna's (+ the opening arena). Details in the design bible.

**Weapons (14):** Homing Missile, Power Missile, Ricochet, Remote Bomb, Freeze Missile, EMP, Napalm, Spike Strip, Concussion Charge, Arc Lance, Process Server, Effigy, Final Notice, Health Pack.

---

## Tech & architecture

- **Engine:** Unreal Engine **5.7**. **C++ core + Blueprints.**
- **Toolchain:** Visual Studio **2026** (v14.44 toolset) is installed and **compiles UE 5.7 fine** — the earlier v143-only worry proved moot on this machine. "Game development with C++" workload required. VS 2026 also owns the `.uproject` double-click association (opens it as JSON); open the project via the UE Project Browser → Browse instead.
- **Vehicle pawn:** `ABLCombatVehicle` + components; data-driven via **Data Assets**.
- **Movement:** **arcade**, not sim. Preferred rig = body mesh + **4 wheel meshes as Blueprint components** (lighter than a full Chaos skeletal vehicle).
- **Destructibles:** runtime navmesh rebuild for collapsing geometry.
- **Source control:** **Git + Git LFS** (LFS for png/jpg/pdf/zip/docx/xlsx/fbx/glb/tga/wav/uasset/umap). GitHub free LFS = 10 GB.

---

## 3D art pipeline (Meshy → Blender → Unreal)

Tool: **Meshy Pro**. Per vehicle:
1. **Reference:** the dossier sheet + the multi-view prompt pack.
2. **Generate:** Meshy 6 **Image-to-3D / Multi-view** — 3 clean views (front/side/rear). Crops + prompts are in the multi-view kit.
3. **Remesh:** **Fixed** topology, **~60k tris** hero vehicles, **80–120k** bosses. (Faces are quads ≈ 2× tris in-engine.)
4. **Export:** FBX/GLB + PBR.
5. **Blender prep:** separate the 4 wheels, fix pivot (ground/center of wheelbase) and scale, clean hard-surface, cap the junk underside (don't detail undersides — chase cam never sees them).
6. **Unreal:** import, build materials; real text/badges as **decals**, not baked.
7. **VFX in Unreal, not baked into mesh:** Foundryman/Jackknife molten glow, Headliner lights, Auditor swirling papers.

**Done so far:** 16 vehicle models created in Meshy (the 16 launch vehicles).

---

## Where everything lives (docs/)

The detailed specs are the source of truth — open them as needed:

- **Design:** `DesignBible_v0.3.docx` (master), `BlackLedger_CombatTuning.xlsx`, `BALANCE_SIM.md`
- **Engineering:** `TECHNICAL_DESIGN.md`, `DAY_ONE_CHECKLIST.md`, `CONVENTIONS.md`, `PROJECT_STRUCTURE.md`, `SETUP.md`, `PREPRODUCTION.md`, `NAME_AND_TRADEMARK.md`, `INDEX.md`
- **Laptop setup:** `SETUP_NEW_LAPTOP.md` ← start here on a new machine
- **Art / 3D:** `art/MESHY_PLAYBOOK.md`, `MESHY_PROMPTS.md`, `MESHY_API_SETUP.md`, `meshy_batch.py`, `meshy_assets.json`
- **Reference sheets:** `vehicle-refs/` (26 dossiers — `png/` = latest art, `html/` = latest sheet format, `svg/` = legacy, + PDF), `weapon-refs/` (14)
- **Multi-view kit:** `MULTIVIEW_PROMPTS.md` + `multiview/<Vehicle>/` (front/side/top crops per vehicle)
- **Other:** `vertical-slice/`, `arenas/` (10 blockouts), `vo/` (opponent barks), `audio/` (brief + event list), `ui/` (6 wireframes), `business/` (pitch, Steam, titles)
- **Raw 3D source art:** `art/source/vehicles/<NN_Name>/` at repo root (Meshy FBX + PBR textures, committed via LFS)

---

## Immediate next steps

1. **Phase 2 — combat core: COMPLETE (June 11).** All Week-2 pieces in C++: `ABLProjectile` + MG primary (`UBLWeaponComponent`, twin muzzles, placeholder tracers + muzzle-flash light), Homing Missile pickup chain (`ABLPickupActor` → pickup slot → `ABLProjectile_Homing`, 80/40 splash + knockback impulses), `UBLImpactFXSubsystem` (weight-scaled hit-stop, perlin camera shake w/ falloff, placeholder `ABLImpactBurst` flash+fireball, player-gated death slow-mo; audio + real Niagara = TODO hooks), `ABLAIController` (C++ state machine: pursue/strafe/peel/pickup/unstuck — BT version arrives with bosses), difficulty scaling per Bible 4.6 (`UBLGameInstance` Easy/Med/Hard: HP, fire duty cycle, aim cone, missile rate; menu UI later, `bl.SetDifficulty 0|1|2` now), TM:B-style 14-car matches (`ABLGameMode` ring spawn, `bl.NumAI` override, `bl.AIDebug 1` overlay), and speed+mass-scaled ram damage. Exit test = the 2-min brawl is fun → tag `phase2-combat`.
2. **Vehicle selection (data-driven) — the one remaining vehicle task, deferred by choice.** The pawn (`ABLCombatVehicle`) hard-codes the Surgeon's body/wheels + chassis numbers. To make all 16 drivable: add a `UBLCharacterDataAsset` (body + 4 wheel meshes + `AxleFrontX/RearX`, `TrackHalfY`, front/rear wheel radii, `BodyMeshZOffset`, mass, and the Speed/Armor/Handling/Special stat budget), give the pawn a `Character` property that applies it on spawn, and generate all 16 assets — the chassis numbers are already in each `*_prep_report.json` (wheelbase/track/radius) and the meshes are imported. Foundation for the character-select screen (`ui/`).
3. **Vertical slice** (Phase 3) per the vertical-slice packet — The Mill + furnace hazard + catwalk-collapse destructible + Surgeon special + Foundryman boss.

**Known refinements deferred:** vehicle selection goes data-driven later (step 2) — the pawn is Surgeon-only for now, which is fine for combat work. **AI on large maps (decide at Phase 3 arenas):** (a) targeting is omniscient nearest-vehicle, genre-authentic but global — if encounter-style fights are wanted instead, add an `EngagementRange` + ROAM wander state to `ABLAIController` (~1 hr, design call); (b) steering is straight-line + unstuck only — real arenas need the TDD §8 NavMesh/Detour + AIPerception driving, which lands with the destructible runtime-navmesh work anyway. Explosion VFX are placeholder (orange burst) by choice — real Niagara explosions/tracers/impacts come in the vertical-slice FX pass. **Bride** carries the older wheel track (clean body, slightly proud front wheels); its cleaned source was reverted in a tooling mix-up, so its re-prep was skipped by choice — re-clean + re-prep if a perfect Bride is wanted before launch. The six-wheelers' middle wheels are cosmetic/static, merged into the body; split them out for spin when the pawn goes data-driven.

**Vehicle pipeline (proven, all 16 done):**
- `tools/blender/prep_vehicle.py` (host Blender, headless) — carves fused Meshy wheels, installs cylinder combat wheels + dark wheel-well liners, real-world scale, ground/wheelbase pivot, decimate, UE-ready FBX + preview renders. Per-vehicle flags: `--length`, `--flip`, `--wheel-radius`, `--wheel-width`, `--decimate`, plus **`--width` / `--height`** (non-uniform scale, e.g. Shepherd) and **`--front-axle-x` / `--rear-axle-x` / `--wheel-y`** (pin an axle when auto-detect is unreliable). Axle consolidation lives in `tools/blender/wheel_solve.py` (pure, bpy-free, unit-tested by `test_wheel_solve.py` — run `python tools/blender/test_wheel_solve.py`); **a weak axle now auto-repairs by mirroring the reliable one**, which fixes narrow-nosed bodies (Bride) where the detector found no front geometry. **`--mid-axle-x <X[,X]>`** installs extra cosmetic axles for the six-wheelers (`build_extra_axles` in `wheel_solve.py`); they're carved/lined like the corners (also clearing the fused original wheel) and merged into the body, so they import with no pawn change. **`--axles N`** (N≥2) swaps the front/rear circle-fit for k-means clustering of the wheel geometry into N axles — the robust path for tandem/3-axle trucks (Crucible, Hemlock, Lien, Refuse, Shepherd, Bride); outer two axles become the spinning corners, inner axles merge into the body. The full-res QA re-audit (`VEHICLE_QA_REVIEW.md`) found the original contact-sheet pass under-caught these six; the other 10 are genuine 2-axle bodies and stay on the classic path.
- **Wheel placement (final method, all 16 — supersedes the auto-detect paths above):** positions are **measured in Blender, not detected.** `tools/blender/measure_axles.py` — import the vehicle's *source* FBX (original wheels still in their wells), drop one Empty on each wheel center in side view, run it; it prints `--axle-fracs` as fractions of length (frame-independent, so flip never matters) and copies them to the clipboard. Each vehicle's measured fracs live in `batch_prep.ps1`. `build_axles_at` places an axle at each frac, RANSAC-fits the tire for radius, and **derives the lateral track from the tire's own lower arc** (where no fender reaches) so the wheel's outer face meets the tire edge instead of poking past the body — the **June 10 stick-out fix.** Per-vehicle overrides for bodies that fool the auto-fit: **`--wheel-radius`** (Antoinette/Cartographer 0.322, Photographer 0.311, Specter 0.45, Shepherd 0.52, Bride 0.40) and **`--wheel-y`** track pins (Antoinette 0.80, Photographer 0.70, Vault 1.30). `tools/blender/export_clean.py` re-exports a hand-cleaned mesh to a vehicle's source FBX via a dropdown picker — **save the .blend before exporting.**
- `tools/unreal/import_vehicle.py` (headless UE) — wipes the dest folder, imports meshes/textures (importer-agnostic: strips Interchange's `<Name>_UE_` prefix), builds + assigns materials, saves. Run via `-ExecCmds="py …"` with env vars `BL_VEHICLE_NAME`/`BL_VEHICLE_FOLDER` — **not** `-run=pythonscript` (Slate crash). **The post-save teardown "Fatal error" under `-nullrhi` is known/harmless — check the log for `IMPORT OK` and the Content folder on disk for truth.**
- **Batch runners:** `tools/blender/batch_prep.ps1` + `tools/unreal/batch_import.ps1` (data-driven roster table: name/folder/length/flip; `-Only <Name>,<Name>` runs a subset). Run with `powershell -ExecutionPolicy Bypass -File <script>`. **Editor must be CLOSED for any UE import** (asset locks); Blender prep is fine with it open.

*(Done June 4: docs pushed; tools installed; repo cloned + Cowork connected; `vehicle-refs updated/` folded in; 16 Meshy models committed via LFS; UE project at repo root with hand-added C++ module. Done June 5: Phase 1 drivable Surgeon. Done June 8: all 15 remaining vehicles batch-prepped + imported in-engine; full QA audit of all 16 (`docs/VEHICLE_QA_REVIEW.md`) — facing verified, 14 clean; prep pipeline hardened (axle auto-repair + manual axle pins + non-uniform scale, with `wheel_solve.py` unit tests); Bride front-axle fix, Shepherd bus-rescale, and opt-in 3rd-axle support (`--mid-axle-x`) for the six-wheelers staged for re-prep. Done June 10: **all 16 vehicles finalized** — every wheel moved onto Blender-measured axle positions (`measure_axles.py` → deterministic `--axle-fracs`, replacing auto-detect), per-vehicle tire radii corrected, and a **lateral wheel-track fix** added to `build_axles_at` (track read from the tire's lower arc) so wheels seat in their wells; all 15 non-Bride re-prepped + re-imported, Bride left on its older track by choice (reverted source). Pawn still Surgeon-only — data-driven selection deferred. Done June 11: **Phase 2 combat pieces 1–2** — `ABLProjectile` + machine-gun primary via `UBLWeaponComponent` (6 dmg × 10 rds/s = the 60-DPS primary-only profile), Homing Missile pickup (`ABLPickupActor` + `ABLProjectile_Homing`, 80 direct / 40 splash per the tuning sheet) built and play-tested in `L_Gym`; inputs `BL_Fire` (LMB/Space/RB) + `BL_FirePickup` (RMB/E/LB). Found + fixed a **wheel-pivot import bug**: UE's FBX import bakes node transforms into verts (`transform_vertex_to_absolute` — must stay default True; False also skips the Y-up→Z-up conversion and meshes import sideways), so wheels exported at their axle positions got ~2 m pivot offsets and orbited the vehicle when spun. Fix: `prep_vehicle.py` now parks wheel objects at the world origin before export (axle positions live in the prep report + pawn chassis numbers, not the FBX). Surgeon re-prepped + re-imported clean; pawn chassis numbers corrected to the prepped frame (axles **±177.78** — prep pivot is wheelbase center, so symmetric — track 100.78, radii 39.2). Remaining 14 non-Bride vehicles re-prepped + re-imported with the fixed pipeline (their June-10 assets carried the same baked wheel offsets). Bride still excluded (reverted source; its wheels also carry the orbit bug, which only matters once it becomes drivable — hand re-clean via `export_clean.py`, then re-prep + re-import, remains the gate).)*

---

## How to work with this project

- **Chris prefers concise, direct answers** — minimal preamble and verbosity.
- Produce **real files** saved to the connected folder (not just chat text); verify outputs (render/preview, contact sheets, etc.); track work with a task list.
- Keep the **grim dossier / horror tone** consistent in all art and copy.
- Naming: `Ref_NN_Name` (launch), `Ref_BN_Name` (boss), `Wref_NN_Name` (weapon).
- When resuming on a new machine: read this file, then `SETUP_NEW_LAPTOP.md`, then `DAY_ONE_CHECKLIST.md`.
