# Black Ledger — Day-One / First-Sprint Checklist

The exact order to work the moment the laptop is online. Roughly the first 1–3 weeks. Checkboxes paste into a GitHub Issue. Ties to `docs/SETUP.md`, `TECHNICAL_DESIGN.md`, and `VERTICAL_SLICE_SCOPE.md`.

## Day 0 — Environment (a few hours)
- [ ] Install per `docs/SETUP.md` (UE5.x, VS 2022 workloads, Git + LFS).
- [ ] `git lfs install`; clone the repo; confirm `.uasset` round-trips through LFS.
- [ ] Create `BlackLedger.uproject` (C++ blank); generate VS project files; build Development Editor; launch.
- [ ] Pass all Phase 0 exit checks in `SETUP.md` (level loads, C++ class compiles, Blueprint calls it).
- [ ] Commit the empty project. Tag `phase0-foundation`.

## Week 1 — Vehicle prototype (Phase 1)
- [ ] Create `ABLCombatVehicle` (C++), placeholder box/cylinder mesh + 4 wheels.
- [ ] Implement movement — **start arcade** (force-based accel/turn/handbrake); expose accel, top speed, turn rate, grip as UPROPERTYs.
- [ ] Build a flat "gym" level with ramps, walls, jumps.
- [ ] Tune until it's fun to just drive (no objectives). **Lock the arcade-vs-Chaos decision.**
- [ ] Add `UBLHealthComponent` (HP + armor multiplier from a hard-coded test stat); debug-print on damage.
- [ ] Hook the control scheme (`docs/vertical-slice/CONTROL_SCHEME.md`) for gamepad + keyboard.
- [ ] Exit criterion met → tag `phase1-vehicle`.

## Week 2 — Combat core (Phase 2)
- [ ] `ABLProjectile` base + a straight machine-gun primary; fire from `UBLWeaponComponent`.
- [ ] Add one Homing Missile (pickup) + `ABLPickupActor` granting ammo.
- [ ] `UBLImpactFXSubsystem`: hit-stop (50–100 ms), camera shake, placeholder impact SFX/particles.
- [ ] Damage feels weighty (test against a dummy vehicle); damage-state visuals stub.
- [ ] One AI opponent: `ABLAIController` + a minimal Behavior Tree (DriveTo + FirePrimary + GrabPickup).
- [ ] A 2-minute 1v1 against the AI is fun → tag `phase2-combat`.

## Week 3+ — Vertical slice (Phase 3, ongoing)
- [ ] Greybox **The Mill** to the blockout (`Arena_5_1` / `TheMill_Blockout`).
- [ ] Furnace-pour `ABLHazardActor` (telegraph → pour → cooldown).
- [ ] **Catwalk-collapse `ABLDestructibleFeature` with runtime navmesh rebuild** — the make-or-break tech; prove it early.
- [ ] The Surgeon: stats from data + `UBLSpecial_DiagnosticField`.
- [ ] `ABLBossPawn` + The Foundryman (3 phases, tells, windows) from the Bosses tuning data.
- [ ] Unlock-on-defeat → Foundryman selectable.
- [ ] HUD (per wireframe) + minimal Main Menu + Roster + Pause + Win/Lose (per `SCREEN_FLOW`).
- [ ] Surgeon intro (~30s) + ending (~60–90s); motion-comic is fine.
- [ ] Playtest with a stranger; iterate. Tag `vertical-slice`.

## Standing habits
- [ ] Commit small and often; keep `main` buildable; branch `feature/*`.
- [ ] Update the GitHub board (PREPRODUCTION → a production board) as you go.
- [ ] Devlog entry + a screenshot/gif each week.
