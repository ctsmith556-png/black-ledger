# Black Ledger — Pre-Production Plan

A milestone/sprint plan for everything that can move forward **before and during** engine setup. Tasks are written as checkboxes so they paste straight into GitHub Issues or a GitHub Project board.

**Legend:** 💻 = no dev machine needed (do it now) · 🖥️ = needs the development laptop · ✅ = done.

**Suggested labels:** `setup` · `design` · `art` · `audio` · `code` · `business` · `vertical-slice`

---

## Milestone 0 — Foundation & Repo  💻
*Goal: anyone can clone the repo and know how the project is organized.*

- [x] Create GitHub repository
- [x] Add `.gitignore`, `.gitattributes` (Git LFS), `README.md`
- [x] Add `docs/`: conventions, project structure, setup, this plan
- [ ] Run `git lfs install` locally and commit the scaffolding
- [ ] Drop `DesignBible_v0.3.docx` into `docs/`
- [ ] Create a GitHub Project board with columns: Backlog · This Sprint · In Progress · Review · Done
- [ ] Convert each milestone below into an Issue; convert each checkbox into a task or sub-issue
- [ ] Pre-download the toolchain installers (see `docs/SETUP.md`)

*Acceptance: repo is cloneable, documented, and the board mirrors this plan.*

---

## Milestone 1 — Design Complete on Paper  💻
*Goal: every system is specified before a line of engine code. Cheapest work, highest leverage.*

- [x] Combat-tuning spreadsheet (`docs/BlackLedger_CombatTuning.xlsx`)
- [x] Control scheme (`docs/vertical-slice/CONTROL_SCHEME.md`)
- [x] Vertical-slice script — The Surgeon (`docs/vertical-slice/`)
- [x] The Mill blockout (`docs/vertical-slice/TheMill_Blockout.svg`)
- [ ] Write barks/VO scripts for the remaining 5 vertical-slice opponents
- [ ] HUD wireframe (health, special charge, pickup slot, minimap, boss health bar)
- [ ] Menu/UX wireframes — main menu + the "filing cabinet" roster screen
- [ ] Blockouts for the other 9 arenas (top-down: layout, pickups, hazards, the 2–3 destructible features)
- [ ] Audio design doc — 3-layer impact list, music direction, VO casting brief
- [ ] One-page "vertical slice scope" lock (exactly what the demo contains)

*Acceptance: a new collaborator could build any system from the docs without asking you questions.*

---

## Milestone 2 — Art & Audio Direction  💻
*Goal: lock the look and sound so production isn't guessing.*

- [ ] Reference boards (PureRef/Pinterest) per character, vehicle, and arena
- [ ] One-page visual style guide from the tone refs (Twisted Metal Black, Silent Hill, Se7en)
- [ ] Color script / lighting key per arena
- [ ] Concept thumbnails for the 15 vehicles and the 10 arena centerpieces (Krita is fine)
- [ ] Asset shopping list: free placeholder vehicles, props, SFX (Fab/Quixel, Sketchfab, Mixamo, Freesound)
- [ ] Music brief (or first sketch if you compose)

*Acceptance: a style guide + reference set good enough to hand to a contractor.*

---

## Milestone 3 — Skill Up for the Heavy Lifting  💻
*Goal: arrive at engine work already dangerous.*

- [ ] C++ refresher (pointers, references, classes, memory) — any machine
- [ ] Unreal course: Chaos Vehicles / vehicle physics
- [ ] Unreal course: AI Controllers + Behavior Trees
- [ ] Unreal course: damage/health systems & gameplay abilities
- [ ] Dissect the reference games — log what makes weight, hit-stop, and boss fights feel good
- [ ] Notes filed against the relevant code Issues so learning maps to the slice

*Acceptance: you can describe, in your own words, how you'll build `ABLCombatVehicle` and one special.*

---

## Milestone 4 — Business & Community  💻
*Goal: protect the name and start the slow-burn marketing now (it compounds).*

- [ ] Verify the title: search Steam + a trademark database for "Black Ledger"; pick a fallback if taken
- [ ] Grab domain + social handles if the name is clear
- [ ] Draft the Steam page plan (capsule art needs, short/long description, tags)
- [ ] Wishlist + Steam Next Fest timeline (target a date relative to the vertical slice)
- [ ] Start a private devlog (screenshots, notes, decisions) from day one
- [ ] Rough budget: Steam fee, asset packs, any contract art/audio/VO

*Acceptance: the name is cleared, a Steam plan exists, and the devlog has its first entry.*

---

## Milestone 5 — Engine: Vertical Slice  🖥️
*Goal: the playable proof — one character, one arena, one boss, one map transformation. (Design Bible §6.1–6.4 roadmap.)*

- [ ] **Phase 0 — Foundation:** install toolchain; pass all `docs/SETUP.md` exit criteria
- [ ] **Phase 1 — Vehicle prototype:** `ABLCombatVehicle`, driving feel fun in a gym level
- [ ] **Phase 2 — Combat core:** primary fire, one missile, health/damage, pickups, one AI opponent; hit-stop + screen shake
- [ ] **Phase 3 — Vertical slice:**
  - [ ] The Mill arena (greybox → art pass)
  - [ ] The Surgeon, fully playable (special: diagnostic field)
  - [ ] The Foundryman boss with 3 phases, tells, and vulnerability windows
  - [ ] One destructible feature (the catwalk collapse) with runtime navmesh rebuild
  - [ ] Unlock-on-defeat flow (beat the boss → it becomes playable)
  - [ ] The Surgeon intro + ending cinematic (motion-comic if budget is tight)
  - [ ] Menus, HUD, victory/defeat states

*Acceptance: a stranger plays the slice — drive, fight, boss, unlock, ending — and has fun. This is your Steam Next Fest demo and pitch.*

---

### Importing to GitHub quickly
- **Manual:** make one Issue per milestone; paste its checkbox list into the Issue body (GitHub renders task lists with progress bars).
- **CLI (optional):** with the `gh` tool — `gh issue create --title "M1 — Design Complete on Paper" --body-file -` and paste the section.
- Add this file to the repo so the plan is versioned alongside the code.
