# Vertical Slice — Scope Lock

**Status: LOCKED for the demo.** Anything not on the IN list is OUT until the slice ships. Re-open only with a deliberate decision — and a matching cut elsewhere.

## The slice in one sentence
One playable character (**The Surgeon**) fights through one arena (**The Mill**), triggers one map transformation (**the catwalk collapse**), and defeats one boss (**The Foundryman**) — who then becomes playable — bookended by a ~30s intro and a 60–90s ending. It must be *fun* and feel *heavy*.

## IN — build these
- **1 character:** The Surgeon — driving, primary fire, and his signature special (Operating Table / diagnostic field).
- **1 arena:** The Mill — greybox first; one art pass only if time allows.
- **1 destructible feature:** Collapse the Catwalks (proves dynamic geometry + runtime navmesh rebuild).
- **1 boss:** The Foundryman — 3 phases, clear tells, vulnerability windows (Bible §4.6).
- **Unlock-on-defeat flow:** beat the Foundryman → it becomes selectable.
- **Combat core:** health/damage, hit-stop, screen shake, the death moment (Bible §4.4).
- **Weapon subset — only these:** machine-gun primary, Homing Missile, Power Missile, Health Pack. (Spike Strip only if it's cheap.)
- **2–3 AI opponents** using placeholder vehicles for the pre-boss fight.
- **UI:** in-match HUD; minimal main menu; minimal roster screen (Surgeon selectable, the rest shown locked).
- **Cinematics:** Surgeon intro (~30s) + ending (~60–90s). Motion-comic is acceptable.
- **States:** pause, victory, defeat.

## OUT — explicitly deferred, do NOT build for the slice
- The other 14 characters and 9 bosses.
- The other 9 arenas and every other destructible feature.
- The full 14-weapon pool (the slice uses the subset above).
- Multiplayer, customization, full story/campaign wiring, the Vault and Auditor endgame, achievements / Steam integration.

## Done = success criteria
A stranger sits down cold and, in ~5–10 minutes: drives and it feels good → fights the AI and it's satisfying → triggers the catwalk collapse → beats the Foundryman → unlocks it → watches the ending — and *wants more*. That build is your Steam Next Fest demo and your pitch.

## Maps to the roadmap
Design Bible §7, Phases 0–3: setup → vehicle feel → combat core → this slice.

## Cut-line — if you fall behind, cut in this order
1. Art polish — stay greybox.
2. Ending cinematic → static motion-comic stills.
3. AI opponents 3 → 2.
4. Spike Strip (if it was added).

**Never cut:** driving feel, the boss fight, and the catwalk-collapse moment. Those three *are* the demo.
