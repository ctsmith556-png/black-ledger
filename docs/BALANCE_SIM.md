# Combat Balance — Time-to-Kill Simulation

Quick model against `BlackLedger_CombatTuning.xlsx`. TTK = Effective HP / sustained DPS. Three blended DPS profiles bracket real play (primary fire only → focused fire with pickups + special). Bible target for a standard kill: **10–15s** (§4.5).

## Characters (TTK seconds)

| Character | Armor | Eff HP | Light ~60 | Typical ~95 | Heavy ~150 | In 10-15s band? (Typical) |
|---|---|---|---|---|---|---|
| The Surgeon | 7 | 1233 | 20.6 | 13.0 | 8.2 | yes |
| Hollow | 9 | 1411 | 23.5 | 14.9 | 9.4 | yes |
| Miss Antoinette | 3 | 878 | 14.6 | 9.2 | 5.9 | FAST (squishy) |
| The Warden | 9 | 1411 | 23.5 | 14.9 | 9.4 | yes |
| Pup | 4 | 967 | 16.1 | 10.2 | 6.4 | yes |
| Crucible | 10 | 1500 | 25.0 | 15.8 | 10.0 | SLOW (tanky) |
| The Cartographer | 6 | 1144 | 19.1 | 12.0 | 7.6 | yes |
| The Lien | 8 | 1322 | 22.0 | 13.9 | 8.8 | yes |
| Hemlock | 6 | 1144 | 19.1 | 12.0 | 7.6 | yes |
| The Shepherd | 8 | 1322 | 22.0 | 13.9 | 8.8 | yes |
| Specter | 7 | 1233 | 20.6 | 13.0 | 8.2 | yes |
| Refuse | 9 | 1411 | 23.5 | 14.9 | 9.4 | yes |
| The Bride | 4 | 967 | 16.1 | 10.2 | 6.4 | yes |
| The Hunter | 5 | 1056 | 17.6 | 11.1 | 7.0 | yes |
| The Photographer | 4 | 967 | 16.1 | 10.2 | 6.4 | yes |
| Vault (unlock) | 6 | 1144 | 19.1 | 12.0 | 7.6 | yes |

### Read
- The spread is **intentional** — it tracks the Armor stat (glass cannons die fast, juggernauts grind). At Typical DPS the roster runs ~9s to ~16s.
- Outside the 10–15s band at Typical DPS: Miss Antoinette (9.2s, FAST (squishy)), Crucible (15.8s, SLOW (tanky)).
- These are *expected* extremes, not bugs: Antoinette/Bride/Photographer (low armor) are meant to pop quickly; Crucible/Foundryman-tier armor is meant to grind. If any feels bad in play, nudge that character's Armor stat or the global DPS in the sheet — do **not** special-case code.

## Bosses (TTK seconds, and per-phase pacing)

| Boss | Eff HP | HP mult | Typical ~95 | Heavy ~150 | Per-phase @Heavy (÷3) |
|---|---|---|---|---|---|
| The Foundryman "Tap" | 11133 | x6 | 117s | 74s | 25s |
| Curfew | 7500 | x5 | 79s | 50s | 17s |
| Condemned | 7944 | x5 | 84s | 53s | 18s |
| Jackknife | 8389 | x5 | 88s | 56s | 19s |
| The Sexton | 7500 | x5 | 79s | 50s | 17s |
| Undertow | 5644 | x4 | 59s | 38s | 13s |
| Lodestone | 10600 | x6 | 112s | 71s | 24s |
| The Headliner "Mr. Marrow" | 5289 | x4 | 56s | 35s | 12s |
| The Alienist | 7500 | x5 | 79s | 50s | 17s |
| The Auditor | 10067 | x6 | 106s | 67s | 22s |

### Read
- Boss fights are long by design (3 phases). At **Heavy** DPS (focused fire + pickups + special — how bosses are actually fought), most land around **35–75s total**, i.e., **~12–25s per phase** — a good, tense range.
- The Foundryman and Lodestone (armor 14/13 + high HP mult) are the longest; if a phase drags past ~30s in play, drop their **HP Mult** in the Bosses tab (a single tunable) rather than touching HP directly.
- Undertow and the Headliner (lower mult) are the snappiest — appropriate for their mobile/chaotic styles.

## Recommendation
Numbers are healthy as starting values. Lock the *actual* sustained DPS once the slice exists (it depends on real fire rates and how often pickups drop), then re-run this against the sheet. Everything here is one spreadsheet edit away from re-tuning.