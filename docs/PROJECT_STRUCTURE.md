# Black Ledger — Project Structure

## Repository root

    BlackLedger/
    |- BlackLedger.uproject     # created on first Unreal project setup
    |- .gitignore
    |- .gitattributes
    |- README.md
    |- Config/                  # DefaultEngine.ini, DefaultGame.ini, DefaultInput.ini
    |- Source/                  # C++ gameplay code (see below)
    |- Content/                 # Unreal assets (Git LFS)
    |- Build/                   # packaging / CI scripts
    \- docs/                    # design + planning (this folder)

## `Source/BlackLedger/` (C++)

    Core/         GameMode, GameState, save system, the "Ledger" services
    Vehicles/     ABLCombatVehicle, movement, health/damage components
    Weapons/      primary fire, pickups, projectile classes
    Specials/     per-character signature special abilities
    AI/           AI controller, behavior trees, boss logic
    Arenas/       hazards, destructible / interactive feature actors
    UI/           HUD, menus, roster screen
    Cinematics/   sequence triggers, intro / ending playback

## `Content/BlackLedger/` (feature-first)

    Characters/   one folder per character (Surgeon, Hollow, ...)
      Surgeon/    BP_Vehicle_Surgeon, meshes, materials, special FX, audio
    Bosses/       one folder per Collector
    Arenas/       one folder per map (TheMill, Downtown, ...)
    Weapons/      shared 14-pickup pool
    Core/         base classes, data assets, shared systems
    UI/           widgets, fonts, icons
    FX/           shared Niagara, decals, post-process
    Audio/        music, shared SFX, VO

Rule of thumb: assets used by one feature live in that feature's folder; shared assets live in `Core/`, `FX/`, or `Audio/`.
