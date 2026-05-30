# Black Ledger — Screen Flow & Menu Structure

Connects the wireframes (HUD, roster) into the full front-to-back flow. See `SCREEN_FLOW.png` for the diagram.

## Screens
- **Cold Open / Boot** — the Auditor's letter + voiceover (first launch / New Game).
- **Main Menu** — New Game · Continue · Roster · Options · Quit.
- **Roster ("The Filing Cabinet")** — pick a contestant (see RosterScreen wireframe). Locked files: SEALED (Collectors) / REDACTED (Vault).
- **Campaign Hub** — the character's route through Ashen Hollow; pick the next arena (gated in order).
- **Intro Cinematic** — ~30s, plays on a character's first arena only.
- **Arena Load** — tip card: controls, the arena's hazard, its destructibles.
- **In-Match** — the HUD (see HUD wireframe). Pause overlay: Resume · Restart · Options · Quit to Menu.
- **Match End** — Victory or Defeat.
  - **Defeat** → Retry (reload arena) or Quit to Menu.
  - **Victory** → if the arena's Collector was just beaten for the first time: **Unlock** screen (Collector joins the roster). If it was the final arena: **Ending Cinematic** (~60–90s) → Roster. Otherwise: next arena.
- **Endgame unlocks** — all 15 endings → **Vault**; all 10 Collectors claimed + the Auditor beaten → **the Auditor** playable.

## Rules
- **Boss gates:** you cannot pass an arena until its Collector is defeated (§2.5, §6).
- **First-time-only:** intro cinematics and unlock screens play once; skippable on repeat.
- **Continue** resumes at the campaign hub for the last-played character.
