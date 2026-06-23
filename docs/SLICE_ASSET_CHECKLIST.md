# Black Ledger — Vertical Slice Asset Checklist

The single tracker for turning the **systems-complete greybox slice** into a **Steam-Next-Fest-quality
vertical slice**. Scope = ONLY what the slice needs (The Mill + The Surgeon + The Foundryman + the
weapon subset). The full 10-arena / 26-vehicle build is Phase 4 — do **not** asset-up the whole game
here.

**Gate before any of this:** the slice must already be *fun* with placeholders (driving feel + the
Foundryman fight). Decorate a game that's already good.

**How assets get in (already built — no code changes):**
- SFX/music → drop wavs in `tools/audio_raw/` (names match the slot), run `tools/unreal/import_audio.py`.
- VO → drop wavs in `tools/vo_raw/{characters,bosses,auditor}/<event>_N.wav`, run `tools/unreal/import_vo.py`.
- Materials/FX → `tools/unreal/make_fx_materials.py`. Map/props → `tools/unreal/make_mill_map.py`.
- **Log every asset's source + license in `CREDITS/ASSET_LICENSES.md` as you add it.**

**Status key:** ☐ todo · ◐ in progress · ☑ done (placeholder ✎ = procedural/temp already in)
**Priority:** **P0** = needed for the demo · **P1** = polish if time

---

## 1. SFX — combat & vehicle (slot → `DA_BLAudioBank`)

All 19 slots already have procedural placeholders (✎). Replace with real recordings via `import_audio.py`.

| Slot (bank) | Event | Source (recommended) | Lic | Pri | Status |
|---|---|---|---|---|---|
| `MGFire` | machine-gun fire | Sonniss "weapons" / BOOM Weaponry | RF | P0 | ✎ |
| `MGHit` | round-on-metal tick | Sonniss metal impacts | RF | P0 | ✎ |
| `MissileLaunch` | homing missile whoosh | Sonniss / Freesound CC0 | RF/CC0 | P0 | ✎ |
| `MissileImpact` | missile detonation | BOOM Cinematic Impacts | RF | P0 | ✎ |
| `PowerImpact` | power-missile crump (+hit-stop) | BOOM Destruction | RF | P0 | ✎ |
| `Explosion` | generic blast | Sonniss / BOOM | RF | P0 | ✎ |
| `VehicleDeath` | car detonation + metal groan | BOOM Destruction + your foley | RF | P0 | ✎ |
| `SpecialActivate` | Operating Table deploy (teal field) | designed (ElevenLabs SFX / synth) | own | P0 | ✎ |
| `Pickup` | weapon-crate grab | Sonniss mechanical | RF | P0 | ✎ |
| `UISelect` | menu click *(hook exists; not yet called — wire `PostUISelect`)* | Freesound CC0 | CC0 | P1 | ✎ |
| `EngineLoop` | diesel idle (pitch-driven) | Sonniss "diesel/engine" loop | RF | P0 | ✎ |
| `Heartbeat` | Surgeon low-HP heart monitor *(his diegetic signature, Bible §4)* | designed (real monitor + your foley) | own | P0 | ✎ |

---

## 2. Music — adaptive beds (slot → `DA_BLAudioBank`)

Industrial dread; music yields to ambience, surges for boss/death (Audio Brief §3). Commission **one**
composer for the set, or license a dark-ambient pack as temp (don't ship temp).

| Slot | State | Source | Lic | Pri | Status |
|---|---|---|---|---|---|
| `MusicMenu` | title screen | composer / dark-ambient pack | comm | P0 | ✎ |
| `MusicCombat` | the brawl | composer | comm | P0 | ✎ |
| `MusicBossP1` | Foundryman phase 1 | composer | comm | P0 | ✎ |
| `MusicBossP2` | phase 2 (66%) | composer | comm | P0 | ✎ |
| `MusicBossP3` | phase 3 (33%) | composer | comm | P0 | ✎ |
| `MusicVictory` | account settled | composer | comm | P1 | ✎ |
| `MusicDeath` | account closed | composer | comm | P1 | ✎ |
| Mill ambience bed | furnace breath, slow clang (Audio Brief §3) | Sonniss/Fab ambience loop | RF | P0 | ☐ |

---

## 3. VO (slot → `DA_BLVOBank`; pipeline `import_vo.py`)

Slice needs only **3 voices**. Lead lines = real cast or your own **speech-to-speech** performance;
the Foundryman is **designed**, not cast. Scripts already exist in `BLVehicleBios.h` + the bible.

| Who | Events (slots) | Source | Lic | Pri | Status |
|---|---|---|---|---|---|
| **The Surgeon** | `Taunt` (wired), `Special`, `LowHealth`, `Death` (wired); `Hurt`/`Kill` slots exist, not yet wired | cast (Casting Call Club) **or** ElevenLabs S2S of your perf | comm/own | P0 | ☐ |
| **The Auditor** | `AuditorLines` (round start; add at boss death later) | cast — the calm-is-the-horror lead | comm | P0 | ☐ |
| **The Foundryman** | `Rise`, `Phase2`, `Phase3`, `Attack`, `Death` | **designed** furnace-roar (process growls; not casting) | own | P0 | ☐ |

> Tone targets are in **Design Bible §5** — use them as *descriptions* for ElevenLabs Voice Design;
> do **not** clone the named real actors.

---

## 4. Environment / props — The Mill (map: `make_mill_map.py`; brief: `docs/arenas/01_The_Mill_*`)

Replace greybox cubes with industrial dressing. Authoritative scale/landmarks = the Arena Build Brief.
Best free source = **Fab + Megascans** (Standard License, commercial-OK). *I can script placement +
lighting in Python like the greybox.*

| Element | Greybox now | Source | Lic | Pri | Status |
|---|---|---|---|---|---|
| Ground / mill plate + walls | cubes + **dark concrete/metal mats** (`M_BL_MillFloor/Wall`) | Megascans concrete/metal meshes swap in later | Fab Std | P0 | ◐ |
| Furnaces F1–F3 (structures + glow) | cube + light | Fab industrial furnace/boiler meshes + `M_BL_Emissive` glow | Fab Std | P0 | ✎ |
| Catwalk (8.5 m deck + ramps) + collapsed "after" geo | cubes | Fab catwalk/gantry/railing kit (authored wreck stays C++) | Fab Std | P0 | ✎ |
| Loading bays (2) + sealed bay | cubes | Fab industrial doors / bays | Fab Std | P1 | ☐ |
| Ladle / torpedo-car landmark | cube | Fab/Sketchfab industrial vessel | Fab Std | P1 | ☐ |
| Pipes, girders, gantries, railings | — | Megascans/Kenney modular industrial | Fab Std | P0 | ☐ |
| Slag, debris, barrels, containers | — | Megascans debris/props | Fab Std | P1 | ☐ |
| Pour-pit rings / hazard decals | disc | decals (warning paint, grime) | Fab Std | P1 | ✎ |
| Lighting pass (furnace glow, fog) + **cinematic grade** (PostProcess: desat/cold/bloom/vignette/grain) | grim grade + thick cold fog **done** in `make_mill_map.py`; sodium lamps later | UE lights/PP + your tuning | — | P0 | ◐ |
| Signage / "CUPOLA" plate decals | — | custom decals | own | P1 | ☐ |

---

## 5. Vehicles (Content/BlackLedger/Characters)

Slice needs the Surgeon + Foundryman looking final-ish; AI opponents may stay Surgeon-placeholders.

| Vehicle | State | Work needed | Pri | Status |
|---|---|---|---|---|
| The Surgeon (ambulance) | Meshy model imported | final materials/decals (red cross, rust, case-file plate) | P0 | ◐ |
| The Foundryman | imported @18 m, treads, glowing core | material polish (molten cracks emissive) | P0 | ◐ |
| AI opponents (2–3) | Surgeon clones | OK as placeholder for the slice | P1 | ☑ |

---

## 6. VFX, UI, fonts, cinematics

| Item | State | Source / work | Pri | Status |
|---|---|---|---|---|
| Impacts/explosions/tracers/muzzle/smoke/fire | procedural emissive (✎) | optional Niagara upgrade | P1 | ✎ |
| HUD / menus | hand-rolled Slate (works) | dossier UMG art pass (flow stays in `UBLUISubsystem`) | P1 | ◐ |
| Display + body font | engine default | Google Fonts (industrial display + clean body) | P0 | ☐ |
| Weapon/pickup icons | — | game-icons.net (CC-BY) | P1 | ☐ |
| Intro/ending cinematics | text motion-comic (works) | panel art for the Surgeon's ~30s + ending | P1 | ✎ |

---

## Suggested execution order (work down by priority)

1. **Mill environment + lighting (P0)** — biggest visual lift; sets the tone + informs ambience.
2. **SFX (P0)** in parallel — swap the 12 combat/vehicle slots; quick wins.
3. **Music (P0)** — commission early (long lead), drop beds in as they arrive; add the Mill ambience loop.
4. **Vehicle material polish (P0)** — Surgeon + Foundryman.
5. **VO (P0)** last — lock script (done), cast/generate the 3 voices.
6. **P1 polish** — UMG dossier UI, Niagara VFX, cinematic panel art, icons, sealed-bay/ladle dressing.

Tackle one row at a time; ping me for the one you want and I'll do the scripting/import/processing.
