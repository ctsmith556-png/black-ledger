# Black Ledger — Documentation Index

The complete pre-production package — design, art reference, and the Meshy 3D pipeline. Unzip at the repo root so this merges into `docs/`. The **Design Bible** is authoritative for content; the **tuning spreadsheet** for numbers.

## Core
- **DesignBible_v0.3.docx** — master design bible (15 characters, 10 arenas, 10 Collector bosses, 14 weapons, lore, roadmap).
- **BlackLedger_CombatTuning.xlsx** — live balance model. **BALANCE_SIM.md** — TTK simulation + tuning notes.
- **PREPRODUCTION.md** — milestone/sprint plan. **NAME_AND_TRADEMARK.md** — title availability scan.

## Engineering
- **TECHNICAL_DESIGN.md** — UE5 architecture + build order. **DAY_ONE_CHECKLIST.md** — first-sprint tasks (open first).
- **CONVENTIONS.md** · **PROJECT_STRUCTURE.md** · **SETUP.md** — naming, layout, environment.

## art/  (3D pipeline — Meshy)
- **MESHY_PLAYBOOK.md** — how to take prompts → game-ready assets (Pro-tuned).
- **MESHY_PROMPTS.md** — paste-ready prompts + settings for all 14 weapons, 16 vehicles, 10 bosses.
- **MESHY_API_SETUP.md** — connect Meshy (Unreal/Blender bridge + the API batch driver).
- **meshy_batch.py** + **meshy_assets.json** — batch driver that runs all 40 prompts through Meshy's API and downloads FBX + PBR maps.

## vehicle-refs/
- **VEHICLE_REFERENCE.pdf** — 26-page codex (all vehicle dossier sheets).
- **svg/** (editable sources) · **png/** (renders) — the 26 case-file reference sheets to feed Meshy image-to-3D.

## vertical-slice/
- **VERTICAL_SLICE_SCOPE.md** · **CONTROL_SCHEME.md** · **TheMill_Blockout** (svg/png).

## arenas/
- **ARENA_INDEX.md** — START HERE for map work: per-arena quick numbers, scale rulers, build loop.
- **Arena Build Briefs** (`01_…` – `10_…` .docx, one per arena) — rendering, tone, recommended 14-vehicle footprint, Unreal scale + lane widths, hazard/destructible specs, greybox priorities. Authoritative for arena SCALE and TONE.
- **ARENA_REFERENCE.pdf** + **svg/** + **png/** — the 10 arena blockouts (authoritative for landmark placement/flow).

## vo/
- **THE_SURGEON_SCRIPT.md** · **OPPONENT_BARKS.md** · **BARKS_REMAINING.md** · **CINEMATIC_SHOTLISTS.md**.

## audio/
- **AUDIO_DESIGN_BRIEF.md** · **AUDIO_EVENTS.md** (Wwise/FMOD).

## ui/
- **SCREEN_FLOW** (md/svg/png) · **HUD_Wireframe** · **RosterScreen_Wireframe** · **UI_MainMenu/Pause/CampaignHub/Unlock/Result/Ending**.

## business/
- **PITCH_ONE_PAGER.md** · **STEAM_PAGE_DRAFT.md** · **ASSET_SHOPPING_LIST.md**.

---
*Black Ledger pre-production package, 2026. SVGs are editable sources; PNGs are renders.*
