# Vertical Slice — Control Scheme

Twisted-Metal-style: easy to drive, deep to master. Designed for gamepad first, full keyboard/mouse parity. All bindings remappable in the final build.

## Gamepad (Xbox layout)

| Action | Button |
|--------|--------|
| Accelerate | Right Trigger (RT) |
| Brake / Reverse | Left Trigger (LT) |
| Steer | Left Stick (X) |
| Aim / free-look | Right Stick |
| Fire primary (unlimited) | RB |
| Fire pickup weapon | X |
| Cycle pickup | D-pad Left/Right |
| Signature Special | LB |
| Handbrake / drift | A |
| Look behind | Right Stick (click) |
| Horn (flavor) | D-pad Up |
| Pause | Menu / Start |

## Keyboard & Mouse

| Action | Key |
|--------|-----|
| Accelerate | W |
| Brake / Reverse | S |
| Steer | A / D |
| Aim / free-look | Mouse |
| Fire primary | Left Mouse |
| Fire pickup weapon | Right Mouse |
| Cycle pickup | Mouse Wheel (or Q / E) |
| Signature Special | Space (or F) |
| Handbrake / drift | Left Shift |
| Look behind | C |
| Horn | H |
| Pause | Esc |

## Design notes
- **Special** is its own dedicated button — it's the character's identity (Bible §3); it should never share an input with pickups.
- **Aim assist** on by default for gamepad; tunable, off-able.
- **Handbrake/drift** matters for heavy vehicles taking tight mill corners — tune per-vehicle.
- Reserve a "self-right" auto-recover (no input) if a vehicle flips, rather than a button.
- Accessibility: full remap, toggle-vs-hold for accelerate, adjustable aim assist, and (per §4.6) longer boss tells without trivializing fights.
