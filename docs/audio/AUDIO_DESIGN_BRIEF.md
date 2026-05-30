# Black Ledger — Audio Design Brief

Audio carries Pillar 4 (Atmosphere Over Spectacle) and Pillar 1 (Weight & Consequence). The §4.4 rule stands: every impact is at least three layers, and silence is a tool.

## 1. The 3-layer impact model
Every hit = (A) approach/projectile + (B) impact crunch + (C) debris/tail. Mix all three — never ship a one-shot "pew."

| Event | A — approach / projectile | B — impact crunch | C — debris / tail |
|---|---|---|---|
| Machine-gun hit | dry supersonic snap | metal-on-metal tick | spent-casing tinkle |
| Homing missile | rising whine + tracking warble | deep concussive thud | shrapnel patter + alarm |
| Power missile | slow low whoosh | huge bass crump (+ hit-stop) | sustained metal groan |
| Character special | the signature tell | over-scaled themed impact | themed tail (fire / ice / slag) |
| Vehicle collision | tire scrub + engine strain | crumple + glass | dragging-metal scrape |
| Death / explosion | ignition swell | massive low boom (30% time-slow) | raining debris + fire crackle |

Hit-stop (50–100 ms) needs a matching micro-gap in the audio — the tiny silence is what sells the weight.

## 2. Vehicle & engine voices
Each chassis gets a distinct engine bed + a damage layer synced to the §4.4 HP states (clean → rattles → metal shriek → smoke hiss → fire roar):
- **Ambulance (Surgeon):** diesel idle + a heart-monitor beep that flatlines at low HP.
- **Fire engine (Hollow):** heavy throaty rumble + the chrome bell on his special.
- **Cadillac (Antoinette):** smooth, too-perfect purr + a compact-mirror chime.
- **Ice-cream truck (Pup):** a warped, half-speed "Pop Goes the Weasel" that skips and reverses.
- **Slag hauler (Crucible):** massive low-end lug + molten sizzle.
- *(remaining chassis follow the same engine + damage-layer pattern.)*

## 3. Music direction
Overall: industrial dread — sparse, textural, mostly drones, distant clanging, and detuned strings. Music yields to ambience and surges only for specials, boss phases, and deaths.

- **The Mill** — industrial gothic; furnace breath, clanging on a slow pulse.
- **Downtown** — sodium-vapor hum, distant sirens, rain.
- **The Tenements** — almost no music; creaks, falling masonry, a TV through a wall.
- **Highway 6** — wind, tire drone, a parallel highway that never gets closer.
- **The Cemetery** — **no diegetic music** (per §5.5). Wind only. The absence is the cue.
- **The Auditor's Office** — paper rustle, distant typewriters, a clock; the reality-glitch = audio stutter/reverse.
- **The Drowned Quarter** — underwater bell, dripping, a low tidal rumble before the surge klaxon.
- **The Switchyard** — iron groans, a signal clicking, the Ghost Train horn from nowhere.
- **Pinewood** — a calliope a half-step flat, looping; applause from an empty grandstand.
- **Saint Dymphna's** — fluorescent buzz, a PA paging dead doctors, electrical-arc whine.

Boss music: each Collector gets a 3-stage cue that escalates at the §4.6 phase thresholds (66% / 33%). The Auditor's "theme" is near-silence plus his voice.

## 4. Diegetic signatures (one sound = one character)
- Surgeon: the heart monitor (steady → flatline).
- Pup: the ice-cream jingle, wrong.
- The Shepherd: forty voices singing a hymn (then thirty-nine...).
- Hollow: the chrome bell.
- Specter: grayscale comms chatter + a missile's slow fall.
- The Auditor: a soft voice on every radio between rounds; static when he's near.

## 5. VO casting brief
Cast for texture, not celebrity — these are tone targets from the bible.

| Character | Voice target / reference | Note |
|---|---|---|
| The Surgeon | Sam Neill (Event Horizon), quieter | clinical, never shouts |
| Hollow | Doug Bradley (Pinhead), dialed down | halting, damaged throat |
| Miss Antoinette | Jessica Lange (AHS) | honeyed Southern, cruel |
| The Warden | J.K. Simmons (Whiplash), older | gravel, terse |
| Pup | child VO + pitch/reverse processing | third person, eerie |
| Crucible | Brian Cox-type gravel | working-class, fierce |
| The Cartographer | Tilda Swinton, quiet | precise → dreamy |
| The Lien | Paul Giamatti-type | dry, courteous menace |
| Hemlock | Carrie Coon, lower / tired | exhausted, scientific |
| The Shepherd | softer Sam Elliott + a tremor | preacher cadence |
| Specter | hollowed-out Lauren Cohan | flat comms; cracks once |
| Refuse | quieter Michael Chiklis | plain, working-tired |
| The Bride | dreamy Cate Blanchett | dissociated, joyful |
| The Hunter | grief-worn Sam Elliott, stone | spare, Western |
| The Photographer | brittle Robin Wright | wry, deflecting |
| Vault | modulated mystery-man, sad | speaks rarely; only truth |
| The Auditor | soft, unfailingly polite, never raised | the calm is the horror |

Collectors (bosses) each get a short, distinctly processed voice: furnace roar (Foundryman), dispatcher PA (Curfew), demolition drawl (Condemned), CB rasp (Jackknife), kindly gravedigger (The Sexton), wet gargle (Undertow), indifferent industrial (Lodestone), curdling showman (The Headliner), clinical reassurance (The Alienist).

## 6. Mix priorities
- Player feedback (own hits, special-ready cue, low-HP flatline) ducks everything else.
- Low HP: heartbeat/flatline rises, music low-passes, the world goes muffled.
- The Auditor's voice always cuts through — side-chain the mix under him.
- Mature, headphone-friendly mix; preserve dynamic range (no brickwall limiting).

## 7. Tooling (suggested)
Wwise or FMOD for adaptive layers and boss-phase music states. Author the 3-layer impacts as blend containers; drive both the engine bed and a music low-pass from one "damage state" RTPC.
