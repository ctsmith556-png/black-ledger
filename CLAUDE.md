# CLAUDE.md — Black Ledger

> Project context for Claude / Cowork sessions. Read this first, then open the specific docs it points to. Keep it updated as the project moves.
> **Place this file at the REPO ROOT** (`black-ledger/CLAUDE.md`) so any session opened in the project folder picks it up automatically.

---

## What this is

**Black Ledger** is a **single-player vehicular combat game** — Twisted Metal Black-inspired gameplay with a grim **Silent Hill / Se7en horror tone** (oppressive, psychological, not campy). Built by a **solo developer (Chris)** in **Unreal Engine 5.7**.

Framing: contestants with dark pasts are pulled into a deadly competition run by **"The Auditor."** Each vehicle is documented as an Auditor **case-file dossier** — that grim dossier look is the project's signature art style.

**Status (June 2026):** Pre-production complete. Dev laptop is set up (Git+LFS, VS 2022 v143 toolset, UE 5.7, Blender); repo cloned to `C:\Users\csmit\black-ledger` and connected in Cowork. **16 vehicle models generated in Meshy** — not yet downloaded into the repo. No Unreal project exists yet.

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
- **Toolchain:** Visual Studio **Community** with the **v143 (VS 2022) build tools** — UE 5.7 does *not* support VS 2026's default v144 compiler yet. "Game development with C++" workload required.
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

1. **Bring in the 16 Meshy models** — download FBX + PBR textures from Meshy into `art/source/vehicles/<NN_Name>/` (folders + README ready) → commit via LFS.
2. **Create the UE 5.7 C++ project** "BlackLedger" inside the repo (root `.gitignore`/`.gitattributes` already in place); compile; **import the Surgeon** as the first round-trip.
3. **Build the vertical slice** per the vertical-slice packet + `DAY_ONE_CHECKLIST.md`.

*(Done June 4, 2026: docs pushed; laptop tools installed; repo cloned + Cowork connected; `vehicle-refs updated/` folded into the main tree.)*

---

## How to work with this project

- **Chris prefers concise, direct answers** — minimal preamble and verbosity.
- Produce **real files** saved to the connected folder (not just chat text); verify outputs (render/preview, contact sheets, etc.); track work with a task list.
- Keep the **grim dossier / horror tone** consistent in all art and copy.
- Naming: `Ref_NN_Name` (launch), `Ref_BN_Name` (boss), `Wref_NN_Name` (weapon).
- When resuming on a new machine: read this file, then `SETUP_NEW_LAPTOP.md`, then `DAY_ONE_CHECKLIST.md`.
