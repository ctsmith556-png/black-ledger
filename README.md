# Black Ledger

A single-player vehicular combat game where fifteen broken people enter a tournament hosted by a cosmic accountant who promises to erase their greatest regret. They fight through a dying city held by his enforcers — the Collections — and the price of winning is always higher than they bargained for.

> "Every debt comes due. The Ledger always balances."

Dark, atmospheric arena combat in the spirit of *Twisted Metal Black*, with the doomed-character framing of *Silent Hill* and the moral weight of *Se7en*.

## Status

**Pre-production.** Engine work begins once the development machine is online. See `docs/` for the full design bible and the pre-production plan.

## At a glance

- **Genre:** Single-player vehicular combat (arena)
- **Engine:** Unreal Engine 5 (C++ core + Blueprints)
- **Platform:** PC (Steam) at launch
- **Rating target:** ESRB Mature 17+ / PEGI 18
- **Scope:** 15 playable characters · 10 arenas · 10 unlockable Collector bosses · 14 pickup weapons

## Design pillars

1. **Weight & Consequence** — vehicles feel heavy; every shot and every story carries weight.
2. **Personal Damnation** — every character is the protagonist of their own tragedy.
3. **Combat as Catharsis** — the fights are brutal and empowering; the horror lives in the cutscenes.
4. **Atmosphere Over Spectacle** — one flickering streetlight beats ten explosions.

## Getting started

> Requires Unreal Engine 5.x, Visual Studio 2022, Git, and Git LFS. Full setup in `docs/SETUP.md`.

    git lfs install
    git clone <this-repo-url>
    cd black-ledger
    # Right-click BlackLedger.uproject -> Generate Visual Studio project files
    # Open BlackLedger.sln, build (Development Editor | Win64), launch the editor.

## Documentation (`docs/`)

- `DesignBible_v0.3.docx` — the full design bible (add it here)
- `PREPRODUCTION.md` — milestone / sprint plan (paste into GitHub Issues or Projects)
- `CONVENTIONS.md` — naming and code conventions
- `PROJECT_STRUCTURE.md` — repository and content folder layout
- `SETUP.md` — environment setup and day-one checklist
- `vertical-slice/` — The Surgeon script, The Mill blockout, control scheme

## License

Proprietary — all rights reserved (placeholder; choose a license before any public or source release).
