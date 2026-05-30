# Black Ledger — Audio Event List (Wwise / FMOD)

Implementation-ready translation of `AUDIO_DESIGN_BRIEF.md`. Names assume Wwise; FMOD equivalents in parentheses where it differs. Convention: `Verb_Category_Subject_State`.

## Naming
- Events: `Play_<Category>_<Subject>[_<State>]` (e.g., `Play_Weapon_MG_Hit`).
- RTPCs (FMOD: parameters): PascalCase nouns.
- Switches (FMOD: labeled params): group + value.
- States (FMOD: global params for music): `MusicState`, `ArenaState`.

## Weapon / impact events (each is a 3-layer blend container — approach + crunch + tail)
- `Play_Weapon_MG_Fire`, `Play_Weapon_MG_Hit`
- `Play_Missile_Homing_Launch`, `_Track`, `_Impact`
- `Play_Missile_Power_Launch`, `_Impact` (drives a hit-stop audio gap)
- `Play_Pickup_Ricochet_Bounce`, `_Impact`
- `Play_Pickup_RemoteBomb_Set`, `_Detonate`
- `Play_Pickup_Freeze_Hit`, `Play_Pickup_EMP_Hit`, `Play_Pickup_Napalm_Ignite`
- `Play_Pickup_SpikeStrip_Deploy`, `Play_Pickup_Concussion_Blast`
- `Play_Pickup_ArcLance_Loop` (start/stop), `Play_Pickup_ProcessServer_Deploy`/`_Fire`
- `Play_Pickup_Effigy_Spawn`, `Play_Pickup_FinalNotice_Launch`/`_Seek_Loop`/`_Impact`
- `Play_Impact_Collision` (Switch: SurfaceType), `Play_Vehicle_Death`

## Special events (one per character; 3-layer + themed tail)
`Play_Special_Surgeon` (DiagnosticField), `_Hollow` (Conflagration), `_Antoinette` (Compact), `_Warden` (Solitary), `_Pup` (Storytime), `_Crucible` (Pour), `_Cartographer`, `_Lien`, `_Hemlock`, `_Shepherd`, `_Specter` (DangerClose), `_Refuse`, `_Bride`, `_Hunter`, `_Photographer`, `_Vault` (Redaction). Boss specials: `Play_BossSpecial_<Collector>`.

## Vehicle / engine (per chassis, with damage layer)
- `Play_Vehicle_Engine_Start` / `_Loop` / `_Stop` (Switch: VehicleType).
- RTPC `EngineRPM` (from speed), RTPC `DamageState` 0–4 (clean→fire) drives engine grit + a music low-pass.
- Diegetic loops: `Play_Diegetic_Surgeon_HeartMonitor` (RTPC `LowHealth` → flatline), `Play_Diegetic_Pup_Jingle`, `Play_Diegetic_Hollow_Bell`, `Play_Diegetic_Shepherd_Hymn`.

## RTPCs (parameters)
- `EngineRPM`, `Speed`, `DamageState` (0–4), `LowHealth` (0–1), `Distance` (3D), `BossPhase` (0–3), `ArenaWetness` (Dymphna's/Drowned).

## Switch groups
- `VehicleType` { Ambulance, FireTruck, Cadillac, IceCreamTruck, SlagHauler, ... }
- `SurfaceType` { Asphalt, Metal, Water, Gravel, Mud, Tile }
- `WeaponClass` { Rapid, Burst, Heavy }

## Music states (`MusicState`)
`Explore`, `Combat`, `BossP1`, `BossP2`, `BossP3`, `Death`, `Victory`, `Menu`.
- Boss states cross-fade at the §4.6 thresholds (66% / 33%).
- `ArenaState` selects the per-arena ambience bed (Mill, Downtown, Tenements, Highway6, Cemetery=NONE, Office, Drowned, Switchyard, Pinewood, Dymphna). **Cemetery routes to a silent bed (wind SFX only).**

## Mix / buses
- Buses: SFX_Player, SFX_World, Music, VO, Ambience.
- Ducking: SFX_Player (own hits, special-ready, low-HP) ducks World + Music; **VO_Auditor side-chain-ducks the entire mix** when he speaks.
- Low-HP state: raise heartbeat, low-pass Music + World.
- Master: Mature/headphone target; preserve dynamic range (no brickwall).

## Build order
Stub `Play_Weapon_MG_Hit`, `Play_Missile_*`, `Play_Vehicle_Death`, and `EngineRPM`/`DamageState` first (Phase 2). Add specials + boss music states in Phase 3. Everything else post-slice.
