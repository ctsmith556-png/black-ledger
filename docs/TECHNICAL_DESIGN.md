# Black Ledger — Technical Design Document (v0.1)

Target: Unreal Engine 5, C++ core + Blueprints, solo / very small team. Built to match Design Bible v0.3 and `BlackLedger_CombatTuning.xlsx`. This is a practical build guide, not academic — every system maps to something you'll implement.

## 0. Guiding principles
- **Data-driven.** Characters, weapons, arenas, and bosses live in **Data Assets**, so tuning is not a code change. The combat-tuning spreadsheet is the source; it exports into these assets (see §11).
- **C++ for contracts and systems; Blueprints for tuning, wiring, FX/anim.** If it has rules, it's C++. If it's values or hookup, it's Blueprint.
- **One vehicle, many characters.** There is a single combat-vehicle class; a character is *data + one special ability*.
- **Feel is centralized.** Hit-stop, screen shake, and impact audio all route through one subsystem so every weapon feels consistent (Bible §4.4).
- **Prefix `BL`** on all game classes (see `docs/CONVENTIONS.md`).

## 1. High-level architecture
- `ABLGameMode` — match rules, spawn the roster + AI, win/lose conditions, boss-gate logic.
- `UBLGameInstance` — persists across levels: current character, unlocks, save handle.
- `ABLCombatVehicle` (Pawn) — the heart of the game (see §2).
- `ABLPlayerController` / `ABLAIController` — drive the same pawn; only the input source differs.
- `ABLHUD` + UMG widgets — bound to the local vehicle's components via delegates.
- `UBLImpactFXSubsystem` (World Subsystem) — centralized combat feel (§5).

## 2. ABLCombatVehicle + components
Movement: **start with a custom arcade movement** (force-based, tuned for the heavy-but-responsive TM feel) rather than full Chaos Vehicle physics — it's faster to make "fun" and easier to balance. Keep Chaos as a fallback if you want sim-grade suspension. *Decide and lock this in Phase 1.*

Components (each is a `UActorComponent`, reusable by player and AI):
- **`UBLHealthComponent`** — current/max HP; applies the armor multiplier from the character's stat (same formula as the tuning sheet: `HP = BaseHP * (0.7 + (Armor-1)*0.0889)`); applies `FBLDamageEvent`; owns the damage-state ladder (paint → dents → broken glass → smoke → fire) and broadcasts `OnDamaged` / `OnHealthState` / `OnDeath`.
- **`UBLWeaponComponent`** — primary fire (unlimited, type from data) + the pickup slot (ammo of a held pickup weapon); spawns `ABLProjectile`s; broadcasts ammo changes to the HUD.
- **`UBLSpecialComponent`** — cooldown timer + activates the character's `UBLSpecialAbility`; broadcasts charge state ("READY") to the HUD.
- **`UBLStatusComponent`** — holds active `UBLStatusEffect`s (burn, freeze, EMP-disable, armor-corrode, control-invert, mark) applied by weapons/specials.
Stats and visuals come from **`UBLCharacterDataAsset`** (Speed/Armor/Handling/SpecialPower, vehicle mesh, special class, VO bank).

## 3. Weapons & specials (data-driven)
- **`UBLWeaponDataAsset`** — category (Rapid / Burst / Heavy), damage, fire rate, ammo, projectile class, FX/SFX. The 14 pickups (Bible §4.3) are 14 of these.
- **`ABLPickupActor`** — sits at a spawn point; on touch, grants ammo of its weapon to the `WeaponComponent`. Health Pack is a pickup that calls Heal().
- **`ABLProjectile`** (base) → `Homing`, `Straight`, `Beam` (Arc Lance), `Seeker` (Final Notice), `Lobbed` (Napalm). On hit, builds an `FBLDamageEvent` and routes through the FX subsystem.
- **`UBLSpecialAbility`** (UObject base, one subclass per character) — `Activate()`, optional tick, `End()`. Example: `UBLSpecial_DiagnosticField` (Surgeon) spawns a radius volume that applies a "diagnosed" status (double-damage + self-heal). Designers set radius/duration via data, not code.

## 4. Damage & status model
- **`FBLDamageEvent`** `{ Instigator, Amount, EBLDamageType, HitZone, bSplash, SplashRadius }`.
- `HealthComponent::ApplyDamage` runs: armor multiplier → active status modifiers (e.g., corrode raises damage taken) → apply → broadcast.
- **`UBLStatusEffect`** (base): `OnApply/Tick/OnExpire`, stack rules. Shared by many kits — burn DoT (Hollow/Napalm), freeze (Freeze Missile), disable-special (EMP/Vault Redaction), corrode-armor (Hemlock/Rust), invert-controls (Pup/Alienist), mark (Bride/Photographer). Build this once; reuse everywhere.
- All numbers originate in the tuning sheet (§11).

## 5. Combat feel — centralized (Bible §4.4)
**`UBLImpactFXSubsystem`** is the single entry point for every impact:
- **Hit-stop:** brief global time-dilation dip (50–100 ms), scaled by weapon weight; matched by a micro audio gap.
- **Screen shake:** `UCameraShakeBase` scaled small (primary) → massive (special).
- **Audio:** triggers the 3-layer impact event (see `AUDIO_EVENTS.md`).
- **Particles + decals:** spawn by surface/weapon.
- **Death moment:** 30% time-slow for ~1s + kill-cam linger.
Route player hits, AI hits, and projectile impacts all through here so nothing feels inconsistent.

## 6. Hazards & destructible features (Bible §5)
- **`ABLHazardActor`** (base): timed environmental hazards — telegraph (fires a warning event/VFX/SFX) → active window (applies damage/effects) → cooldown. One subclass per hazard: furnace pour, sinkhole, wind-push, fog (toggles AI/LoS rules), tidal surge, ghost train (moves along a spline), ride-debris, arc-and-collapse. Bosses can drive these (§7).
- **`ABLDestructibleFeature`** (base): has health; `OnDestroyed` → (1) play the Chaos Geometry-Collection break for spectacle, (2) **swap in pre-authored "after" geometry** that is navigable, (3) **rebuild navigation** for that region. Subclasses: `Collapse→Ramp`, `Breach→Opening`, `Drop→Bridge`, `Flood→LevelChange`.
- **Critical tech:** dynamic navigation. Use **Runtime Navigation Mesh generation + Nav Invokers** so the AI can re-path after a map transformation. **Prototype this first** in the slice (the Mill catwalk collapse) — it's the riskiest system. Do **not** rely on simulated debris for navigation; the navigable result is an authored swap.

## 7. Bosses & phases (Bible §4.6, §6)
- **`ABLBossPawn`** (shares components with `ABLCombatVehicle`): a phase state machine (Phase 1/2/3 at the HP thresholds in the tuning sheet's Bosses tab), each phase enabling a different attack set, every heavy attack following **telegraph → window** so it's hard-but-fair. Add-spawning is capped. The boss can command its arena's `ABLHazardActor`.
- Boss = **`UBLBossDataAsset`** (over-budget stats, HP multiplier, phase thresholds) + a **Behavior Tree**.
- **Unlock-on-defeat:** first kill flags the Collector unlocked in the save → it appears on the roster as a playable, overpowered pick.

## 8. AI (driver + combatant)
- **`ABLAIController`** + **Behavior Tree** + **Blackboard** + **AIPerception**.
- Targeting: nearest/threat selection; pickup-seeking; **hazard avoidance** (query nearby `ABLHazardActor` danger zones); flee-when-low.
- Movement: NavMesh + Detour crowd avoidance; must respect runtime navmesh changes (§6).
- Tasks/Services: `DriveTo`, `StrafeTarget`, `FirePrimary`, `UseSpecialWhenReady`, `GrabPickup`, `AvoidHazard`, `FleeWhenLow`. Boss BTs are bespoke per Collector but share these building blocks.
- **Difficulty** scales HP / attack frequency / tell-window length — **never by removing tells** (Bible §4.6).

## 9. UI (Bible wireframes + screen flow)
- UMG widgets bound to component delegates: HUD (health, special charge, pickup+ammo, radar, boss bar), Roster ("filing cabinet"), Main Menu, Pause, Victory/Defeat, Unlock, Ending. See `HUD_Wireframe`, `RosterScreen_Wireframe`, `SCREEN_FLOW`.
- A lightweight UI manager (on GameInstance) drives transitions per `SCREEN_FLOW.md`.

## 10. Save & progression
- **`UBLSaveGame`**: unlocked characters (15 launch + 10 Collectors + Vault + Auditor), endings seen, per-character campaign progress, options.
- Unlock rules: Collector on first defeat; **Vault** after all 15 endings; **the Auditor** after all 10 Collectors claimed *and* the Auditor beaten (Bible §2.5, §6.10).

## 11. Data pipeline (spreadsheet → game)
1. Export each sheet of `BlackLedger_CombatTuning.xlsx` to CSV.
2. Import as Unreal **DataTables** (one row struct per type).
3. Author/refresh **Data Assets** from the tables (or read tables directly).
Designers tune in the sheet → re-import → values update without recompiling. Keep the sheet authoritative.

## 12. Build order (maps to Bible §7 Phases)
- **P0 — Foundation:** project + modules + LFS (done); a trivial `ABLActor` compiles; editor round-trips a `.uasset` (see `docs/SETUP.md`).
- **P1 — Vehicle:** `ABLCombatVehicle` + movement + `UBLHealthComponent`; a flat gym level; exit when *driving is fun with no objectives*. **Decide arcade vs Chaos here.**
- **P2 — Combat core:** `UBLWeaponComponent` (primary + one missile), `ABLProjectile`, `UBLImpactFXSubsystem` (hit-stop + shake + 3-layer audio), one AI opponent (BT), pickups. Exit when a 2-minute AI fight is fun.
- **P3 — Vertical slice:** `UBLSpecialComponent` + Surgeon's special; The Mill + one hazard (furnace pour) + **one destructible (catwalk collapse with runtime navmesh rebuild)**; `ABLBossPawn` + Foundryman 3-phase; unlock flow; HUD + minimal menus; Surgeon intro + ending. Exit = a stranger has fun (see `VERTICAL_SLICE_SCOPE.md`).

## 13. Tech risk watch-list
1. **Runtime navmesh on destructibles** — prototype in P3 first; it gates the whole "maps transform" pillar.
2. **Vehicle feel (arcade vs Chaos)** — resolve in P1; everything downstream depends on the size-to-speed ratio (see arena scale note).
3. **Scope** — build the slice's slice; don't generalize systems before one of each works end-to-end.
