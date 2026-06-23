# Black Ledger — Audio Acquisition Plan (real SFX + VO)

How to get the *real* sound for a grim TM:B-meets-Silent-Hill game, efficiently, as a solo dev.
The engineering is done: every event is wired through `UBLAudioSubsystem` + the data banks
(`DA_BLAudioBank`, `DA_BLVOBank`), and acquiring assets = **drop files in a raw folder and run an
import script** (`tools/unreal/import_audio.py`, `tools/unreal/import_vo.py`). No code changes.

Guiding rule: **source the 80% from libraries, design the 20% that is the game's soul.** The soul
here = the 3-layer impacts, the per-chassis engines, and the "one sound = one character" diegetic
signatures (Bible §4.4 / Audio Brief §4). Spend money/time there; buy the rest.

---

## Phase 0 — Lock the pipeline first (cheap, do once)

You already have a C++ audio director + data banks, so you do **not** need to buy middleware to ship.
Three valid routes, in order of "least new tooling":

1. **UE-native (recommended for the slice/launch):** keep your subsystem; back the bank slots with
   `USoundWave` + **SoundCues / MetaSounds** (both free, built into UE 5.7). MetaSounds covers the
   3-layer impacts and the speed-driven engine without middleware. **No license, no revenue cut.**
2. **FMOD** — only if a sound designer wants to author adaptive content without touching C++.
   Free while project budget < **$200k**, and FMOD takes **no cut of revenue** (indie favorite).
3. **Wwise** — most powerful interactive-music/states, but a more restrictive/complex free indie tier
   (also < ~$200k). Overkill for a solo dev whose audio is already wired in code. **Skip unless you
   hire a Wwise-fluent designer.**

> Decision for now: **stay UE-native.** The bank is the single integration point either way.

---

## Phase 1 — Library SFX (covers ~80%, start at $0)

Most of Black Ledger's palette — metal impacts, gunfire, explosions, machinery, fire, debris,
industrial ambience — is library work. Start free, buy only the heavy hitters.

**Free, royalty-free, commercial-OK:**
- **Sonniss GameAudioGDC archive** (`gdc.sonniss.com`) — the 2026 bundle plus a **200GB+** 9-year
  archive, royalty-free, commercially usable, **no attribution**. This alone can source most of your
  ballistics/explosions/metal/machinery/engine beds. *License note: media-production use only — do
  NOT feed these into AI/ML training.*
- **Freesound.org** (filter to **CC0**) — one-off oddments (alarms, switches, specific clanks).
- **GameSounds / itch.io CC0 packs** — fillers.

**Targeted paid, where free falls short (designed/weighty material):**
- **BOOM Library** — pro packs ~**$99–199** each (e.g. Cinematic Impacts, Destruction, Weaponry,
  Vehicles). Buy **2–3** for the punchy missiles/explosions and the boss's molten/metal hits.
- **A Sound Effect** — marketplace of curated single packs from indie sound designers; royalty-free;
  great for one specific need (e.g. a "heavy diesel + ladle car" set for the Foundryman).
- **Soundly** (~**$15/mo**, free tier exists) — cloud search/library to *work* in while designing.

**Budget:** $0 to start → **$200–500** buys 2–4 pro packs that cover the demo's heavy hits.

---

## Free-asset sourcing — Fab, Megascans & the license that confuses everyone

The **Unreal Engine Marketplace is now Fab** (`fab.com`, Epic's unified store since Oct 2024; your
Epic account carries over). It's a real free-asset source — most useful here for the **Mill's art**,
not audio, but worth grabbing:

- **Quixel Megascans** — large free library of scanned materials/props (industrial metal, rust,
  concrete, debris, girders) under Fab's Standard License. This is the best free dressing for the Mill.
- **Epic first-party content** — Paragon, animation/effects/environment kits, permanently free.
- **Rotating free content** — Fab posts biweekly freebies (successor to "free for the month");
  **claim them while free and they're yours to keep.**
- **General free library** — on fab.com set the price filter to **Free** and filter by category
  (3D, Audio, VFX). Some free **audio/music** packs live here too — a supplement to Sonniss, not a
  replacement.

**The license gotcha (read this):** free Fab assets are licensed as either **Creative Commons** or
Fab's **Standard License**. The Standard License has two price tiers — **Personal** (you've made
< $100k gross in 12 mo) and **Professional** (> $100k) — but **both grant identical rights and BOTH
allow commercial use.** "Personal" is *not* non-commercial; it's just the cheaper price band. So free
Standard-License assets are fine to ship. For Creative Commons assets, check the specific license per
asset (**CC0** = no strings; **CC-BY** = attribution owed). Bring assets in via the **Fab** plugin in
UE 5.7 (Megascans also flow through Bridge), and log each asset's source + license in
`CREDITS/ASSET_LICENSES.md`.

Sourcing priority for Black Ledger: **Fab/Megascans → Mill environment & props**;
**Sonniss/Freesound → most SFX**; **ElevenLabs → VO + designed/signature sounds**;
**Fab/Kenney → placeholder vehicles**.

---

## Phase 2 — Design the soul (the 20% a human must shape)

These don't come pre-made; they're assembled (library + foley + processing):

- **3-layer impacts** (Bible §4.4): approach + crunch + tail per weapon. Build these as MetaSounds/
  Cues from library layers.
- **Per-chassis engine beds + the 5 damage states** (clean → rattle → shriek → smoke → fire).
- **Diegetic signatures — one sound = one character** (Audio Brief §4): Surgeon heart-monitor
  (steady → flatline), Pup's *wrong* ice-cream jingle, Hollow's chrome bell, Shepherd's 40-voice
  hymn (then 39…), the Auditor's radio-static presence. These are the identity of the game.

**How to get them:**
- **DIY foley** — the grim/real tone *wants* you to record metal scrapes, fire, mechanical clanks
  with a decent shotgun/dynamic mic. Cheapest and most characteristic.
- **Hire one freelance game sound designer** for the signatures + the boss set (per-asset or a few
  days). Find them at **A Sound Effect** (sound-designer profiles), **r/gameDevClassifieds**,
  the game-audio community on X/Bluesky, or **Soundlister**. A focused contract (impacts + 16
  signatures + Foundryman + UI) is a well-bounded, affordable brief.

**Music** (industrial dread; per-arena beds; boss 3-stage cues; Auditor = near-silence): commission
**one** composer for menu + combat + the 3 boss-phase beds + victory/death first; license a
dark-ambient pack as *temp only* (don't ship temp). Composers: GameDevMarket, the same community
boards, or referrals.

---

## Phase 3 — Voice over (last; after the script is locked)

Your lines already exist (intro/ending storyboards in `BLVehicleBios.h` + the bible), and the bark
hooks are wired. Lock the script, then cast.

**Casting platforms:**
- **Casting Call Club** — indie/passion-project friendly; emerging + pro talent; post a role with
  your **tone references** (the Bible §5 brief already lists them: Sam-Neill-quiet Surgeon,
  J.K.-Simmons Warden, etc.). Best value for an indie.
- **Voice Acting Club** — indie **rate guide** to budget against; community of indie-rate talent.
- **Voices.com** — higher polish + reliability, higher cost; use if budget allows for the leads.

**Money:** pay a **session-fee buyout** (standard for games — actors don't take royalties on sales).
**Generative-AI rights are separate** and require explicit, paid, informed consent — don't bundle
them into a normal buyout.

**AI VO (ElevenLabs) — use it as *scratch*, not the ship:**
- Commercial license from the **Starter ($5/mo)** tier; paid plans waive attribution and grant
  **perpetual** rights to already-generated audio. Great for **timing barks/cinematics during dev**
  and for placeholder/announcer/crowd filler.
- For a character-driven horror game, **cast real humans for the leads** (Surgeon, Auditor, the
  Collectors) — the performance *is* the horror. Ethics: only clone voices you own or have written
  consent for; disclose AI VO if you ship any.

---

## AI voice generation — tools, the "perform-once" trick, and the rules

Two kinds of AI voice, and the difference matters for a horror game where *delivery* is everything:

- **Text-to-speech (TTS):** type a line, get audio. Fast; great for the many short **barks** and
  secondary/AI-opponent lines, and for **scratch** timing. Emotional range is improving but still the
  weak spot for raw, broken, grief-stricken leads.
- **Speech-to-speech (S2S / voice conversion):** *you perform the line* with full emotion, and the AI
  re-skins your performance as a different character voice. **This is the solo-dev superpower** — one
  performer can voice all 16 contestants + bosses while keeping real human acting, and because the
  source performance is *yours*, it's the most ethically/legally clean path.

**Tools worth a look (June 2026):**

| Tool | Type | Why for Black Ledger | Notes |
|---|---|---|---|
| **ElevenLabs** (v3, Voice Design, Voice Changer) | TTS + S2S | Pragmatic primary: indie pricing, commercial license, real emotion, **Voice Design** invents *original* voices from a text description, and Voice Changer does S2S | One tool from scratch → barks → many character voices |
| **Respeecher** | S2S | Emmy-winning, film/AAA-grade conversion (Cyberpunk, etc.); best for the **leads** if budget allows | Premium/enterprise pricing |
| **Altered Studio** | S2S | Indie-accessible performance-to-voice; good middle ground | Per-seat plans |
| **Murf / Noiz AI** | TTS | Fast, emotion controls + game-engine-friendly APIs; ideal for the **bark** volume | Quick secondary lines |
| ~~Replica Studios~~ | — | Was the game-focused UE-plugin option | **Reportedly shut down in 2025 — verify before relying** |

**Recommended hybrid for this game:**
1. **Leads (Surgeon, the Auditor, the Collectors):** record *yourself* (or one actor) performing the
   lines, convert per-character via **S2S** (ElevenLabs Voice Changer or Respeecher). Real
   performance, distinct voices, clean rights.
2. **Boss "voices" (Foundryman):** not casting at all — **design** the furnace-roar by processing
   growls; AI can generate the growl base.
3. **Barks + AI opponents:** **TTS** (ElevenLabs / Murf / Noiz) — fast for dozens of short lines.
4. **Scratch/timing during dev:** any TTS, already in your loop.

**The rules (don't skip these):**
- **Design original voices; don't clone real people.** Your Bible §5 references (Sam Neill, J.K.
  Simmons…) are **tone targets** — feed them as *descriptions* to Voice Design, do **not** clone the
  actual actors' voices (likeness/IP risk).
- **Only clone a voice you own or have written, paid, informed consent for** (your own voice is fine).
- Use **commercial-tier** plans (e.g. ElevenLabs Starter+) so you own perpetual rights to generated
  audio with no attribution.
- **Disclose AI-generated voices** in your Steam submission — Valve requires an AI disclosure and
  surfaces it on the store page (verify the current policy at submission time).
- If you hire **union** talent, AI rights are a separate, consented, paid item (SAG-AFTRA terms).

Everything still drops into `tools/vo_raw/{characters,bosses,auditor}/<event>_N.wav` → `import_vo.py`.

---

## Slice-first shopping list (what to get NOW for the demo)

You only need three voices and a handful of SFX families for the vertical slice:

| Need | Source (fastest) |
|---|---|
| MG / missile / explosion / impacts | Sonniss (free) + 1 BOOM pack |
| Engine bed + damage layers (Surgeon's ambulance) | Sonniss diesel + your foley |
| Furnace pour / molten / boss metal | Sonniss + A Sound Effect "industrial" pack |
| Surgeon special, pickups, UI | Sonniss + Freesound CC0 |
| **VO: Surgeon** (lead) | Cast on Casting Call Club |
| **VO: The Auditor** (lead) | Cast on Casting Call Club |
| **VO: Foundryman** | *Design* a processed furnace-roar — no casting needed |
| Music: menu / combat / 3 boss beds / victory / death | One composer commission |

---

## Budget tiers

- **$0 (today):** Sonniss archive + Freesound CC0 + ElevenLabs scratch VO + your own foley. Enough to
  replace every placeholder in the slice with real-enough audio.
- **~$300:** add 2 BOOM/A-Sound-Effect packs (weighty weapons + industrial) + a Soundly month.
- **~$1,500–3,000:** + cast 2–3 lead VO roles (buyout) + commission the boss/menu music. Ships the
  slice's audio at a real bar.

---

## Integration (already built — no code work)

1. Put finished SFX wavs in `tools/audio_raw/` (names match the bank slots), run
   `tools/unreal/import_audio.py` → rebuilds `DA_BLAudioBank`.
2. Put VO wavs in `tools/vo_raw/{characters,bosses,auditor}/<event>_N.wav`, run
   `tools/unreal/import_vo.py` → rebuilds `DA_BLVOBank` (random variation per event).
3. Music wavs → bank's music slots via the same SFX import.
4. **Track every asset's source + license in `CREDITS/ASSET_LICENSES.md` from day one** (Audio Brief
   rule). Note Sonniss = no-AI-training; note any CC-BY attribution you owe.

The call sites never change — swapping placeholder tones for real audio is a content drop, not code.
