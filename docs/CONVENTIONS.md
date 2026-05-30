# Black Ledger — Conventions

Conventions keep a solo / small-team project navigable. When in doubt, match the existing pattern.

## Asset naming (`Content/`)

Format: `Prefix_AssetName_Variant`

| Type | Prefix | Example |
|------|--------|---------|
| Blueprint class | `BP_` | `BP_Vehicle_Surgeon` |
| Blueprint base/abstract | `BP_..._Base` | `BP_Vehicle_Base` |
| Widget Blueprint | `WBP_` | `WBP_RosterScreen` |
| Material | `M_` | `M_Ambulance_Body` |
| Material Instance | `MI_` | `MI_Ambulance_Body_Rusted` |
| Texture | `T_` | `T_Ambulance_BaseColor` |
| Static Mesh | `SM_` | `SM_Mill_Catwalk` |
| Skeletal Mesh | `SK_` | `SK_Surgeon_Driver` |
| Anim Blueprint | `ABP_` | `ABP_Vehicle_Surgeon` |
| Niagara System | `NS_` | `NS_FurnacePour` |
| Sound Cue / Wave | `SC_` / `S_` | `SC_Impact_Heavy` |
| Data Asset | `DA_` | `DA_Character_Surgeon` |
| Level / Map | `L_` | `L_TheMill` |

Texture suffixes: `_BC` base color, `_N` normal, `_ORM` packed occlusion/rough/metal, `_M` mask.

## C++ naming (Unreal standard, `BL` project prefix)

| Kind | Prefix | Example |
|------|--------|---------|
| Actor | `A` | `ABLCombatVehicle` |
| UObject (non-actor) | `U` | `UBLWeaponComponent` |
| Struct | `F` | `FBLDamageEvent` |
| Enum | `E` | `EBLWeaponType` |
| Interface | `I` | `IBLDamageable` |
| Boolean member | `b` | `bIsBoosting` |

## Blueprints vs C++

- **C++** for systems: health, damage, weapons, AI, save/load, the "Ledger" services.
- **Blueprints** for tuning, wiring, FX/animation hookup, and one-off behavior.
- Avoid hard references across features; prefer interfaces and Data Assets.

## Git

- **Branches:** `main` (always buildable) · `dev` · `feature/<short-name>` (e.g. `feature/vehicle-physics`).
- **Commits:** imperative present tense — "Add diagnostic-field special to Surgeon".
- **LFS:** never commit `.uasset` / `.umap` / binaries without LFS. Run `git lfs install` once per machine.
- **Never commit:** `Saved/`, `Intermediate/`, `Binaries/`, `DerivedDataCache/`.
- Binary assets don't merge — one person edits a given `.umap`/`.uasset` at a time. Use **File Locking** if the team grows.
