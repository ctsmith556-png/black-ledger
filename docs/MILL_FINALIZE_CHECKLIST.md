# The Mill — Finalize: PIE Verification Checklist

> Systems re-verify pass after the June 22 square-loop rehaul. Run in PIE on `L_Mill`.
> Static code review found **no rehaul regressions** — every system is wired
> layout-independently. This checklist confirms that in-engine and gathers the
> observations the §15 combat-tuning pass will act on.
>
> **Report back per block:** a screenshot + one line (pass / what felt off). Logs only
> where noted. Don't fix anything mid-test — just record.

## Console cheats (backtick to open the console in PIE)

| Command | Effect |
|---|---|
| `bl.SpawnBoss` | Spawns the Foundryman ~40 m ahead of you, facing you |
| `bl.SetDifficulty 0\|1\|2` | Easy / Medium / Hard (scales pour cadence, AI, boss frequency) |
| `bl.NumAI <n>` | Override AI count for the brawl |
| `bl.AIDebug 1` | AI state overlay (ROAM green / DETOUR white / ENGAGE orange) |
| `show Navigation` | Draw the navmesh — use it to watch tiles re-knit on a collapse |
| `slomo 0.3` / `slomo 1` | Slow time to read a telegraph, then restore |

Landmarks (world cm): furnace row at **y≈2000** — **FA (−9000) fan**, **FB (0)
river + ladle + boss-rise**, **FC (+9000) circular + valve**. Trench runs E–W at
**y≈−7000** (end ramps at x≈±14300). Catwalk loop is a rectangle ±16000 (X) ×
±13000 (Y); **W/E entry ramps at x≈±12684**; the **collapsible catwalk is the south
mid-span at (0, −13000)**. Sealed bay is the **E-wall pocket near (+24950, +5600)**.

---

## A. Slag trench in/out (your first test)

1. Drive into the trench from the **west** end ramp, cross the channel floor, drive up
   and out the **east** ramp — then repeat the reverse.
2. Do it again **at speed** (the thin ramps used to tunnel; these are 1.5 m thick).

**Expect:** smooth roll-down in, clean climb out both ends, no wall-catch, no tunneling
through the ramp, no getting stuck on the retaining walls.
**Report:** pass, or which end/direction felt like a wall or a snag.

## B. Catwalk loop drive + seams

1. Drive up the **W entry ramp** onto the loop, lap the full square clockwise, come down
   the **E entry ramp**. Then the reverse.
2. Pay attention to the **four corners** and the **seams where the south flank roads meet
   the catwalk gantries** (x≈±6500) — that's the spot most likely to have a lip.

**Expect:** flush transitions everywhere; no lip that catches the nose, no gap you fall
through, deck reads as one continuous road.
**Report:** screenshot any seam that jolts you; note the corner if a corner is rough.

## C. Catwalk-collapse (destructible #2)

1. Drive the loop to the **south mid-span**. `show Navigation` on.
2. Shoot the **two support pillars** under the deck (MG works; ~350 HP each) until it
   collapses.

**Expect:** mid-span deck drops out → **wreck-ramps** appear from the floor up to the two
surviving end gantries → you can drive **down** the wreck onto the floor at y≈−13000 →
navmesh **re-knits** green over the new ramps within a second or two.
**Report:** does the wreck make a drivable ramp down? Does nav re-knit? Screenshot the
collapsed state with `show Navigation`.

## D. Tap-the-Ladle (destructible #1)

1. Face the **centre furnace (FB, x≈0)**. The ladle hangs above it.
2. Shoot the ladle / its release pins (~500 HP total).

**Expect:** ladle "gives" → a **recurring circular molten pour** begins in the centre
(re-pours ~every 22 s, weaponising the middle) → the emptied ladle **drops to a low cover
blob** you can use as cover.
**Report:** does the centre pour start and recur? Is the dropped cover where you'd want
cover, or in the way? Screenshot the pour active.

## E. Blow-the-Furnace (destructible #3)

1. Go to the **right furnace (FC, x≈+9000)** — the valves are the glowing cluster on it.
2. Shoot the valves (~400 HP).

**Expect:** **one-shot blast** (lethal point-blank, ~1400-radius) → the **E-wall sealed
bay opens** (its wall vanishes) → the **Power Missile reward inside (near +24000, +5600)**
becomes reachable.
**Report:** did the sealed bay open? Could you drive in and grab the reward? Note: the
valve (centre-right) and the bay it opens are ~160 m apart by design — does that read as
connected, or confusing? Screenshot the opened bay.

## F. Furnace pours — three distinct shapes + cadence

1. Just drive the central floor for ~2 minutes and watch the furnaces cycle (no boss).
2. Note each shape and whether the **5 s telegraph** (steam + orange warning blink) gives
   you enough time to clear.

**Expect:** **FA = fan** spraying south into the open floor · **FB = straight river**, a
~90 m molten line running E–W across the centre · **FC = circular** overflow on the right.
Staggered so something pours roughly every ~30 s.
**Report (this feeds the tuning pass):**
- Does the **River-B** line wall off N↔S movement too hard, or is it fair?
- Is a pour-every-~30 s too busy, too sparse, or right?
- Is the telegraph readable, or does a pour catch you with no warning?

## G. The Foundryman fight

1. Clear a little space, `bl.SpawnBoss`. (Try once on `bl.SetDifficulty 1`, once on `2`.)
2. Fight him through all three phases.

**Expect:**
- **P1:** mortar shells with a ground ring telegraph; under-leads you.
- **P2 (~66% HP):** molten walls cut across your lane **and** the furnaces start pouring
  **twice as often (~45 s)**; boss music escalates.
- **P3 (~33% HP):** he **overheats** (faster, leaves a fire trail), all furnaces go
  **relentless (~18 s)**, and he winds up a **slam** — cage ramps **white-hot** + ground
  ring → shockwave. Survive it and the **core is exposed ~3 s** ("STRIKE THE CORE"). Bait
  slam → punish core.
- **Death:** core goes dark, final detonation, Result/Unlock screen (Collector claimed).

**Report:** do the phase transitions fire at 66/33%? Does the furnace cadence audibly
ramp at P2/P3? Is the slam→core-window loop readable and punishable? Anything that didn't
trigger.
**Known/expected:** the boss spawns **in front of you** (cheat), not rising from the
centre furnace — the `BLBossRise` point is tagged but not yet wired (a separate TODO, not
a rehaul break).

## H. Pickup flow & spawn safety (feeds the tuning pass)

1. Run a normal brawl: `bl.NumAI 13`, `bl.AIDebug 1`.
2. Lap the arena once focusing on the **pickups**, then once on the **spawns**.

**Observe & report:**
- **Health** (by the N / W / E bays): does grabbing it cost you position / require risk, or
  is it a free safe top-up?
- **Offensive** (homing in front of each furnace + the trench + sides; power on the N loop
  deck, south floor, and sealed bay): do they pull cars **through** the conflict lanes?
- **Travel time** between pickups — dead zones with nothing, or well spaced?
- **Spawns:** any spawn that drops you into a pour, into cover, or with a spawn-kill
  sightline? (14 nodes ring the outer edge facing centre.)

---

## After the run

Send the screenshots + the per-block notes. With your **Build Plan docx (Section 15)**
in the repo, I'll turn blocks **F / G / H** into concrete value changes — furnace cadence
(the §23 Arena Manager vs the current self-cycle), pour footprints/DPS, and pickup
placement — delivered as an **additive script** (load → edit → save, so your hand-placed
rubble survives), never a `make_mill_map` rebuild.
