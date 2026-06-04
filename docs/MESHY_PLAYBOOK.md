# Meshy Playbook (Pro) — Black Ledger

How to turn the prompt pack (`MESHY_PROMPTS.md`) into game-ready assets. Tuned for a Meshy **Pro** plan. Meshy's UI, credit costs, poly caps, and texture resolutions change often — treat specific numbers here as guidance and confirm them in your Pro dashboard.

## What Pro gives you (use it)
- More monthly credits, higher polygon budgets, and up to 4K PBR textures.
- **Quad topology + Remesh** — essential for clean, animatable, game-ready meshes.
- **Image-to-3D** (including multi-image) — far better control than text alone.
- Export to **FBX / OBJ / GLB / USDZ** with textures; private generations; API access on higher tiers.

## Pick the mode per asset
- **Weapons, pickups, props → Text-to-3D.** Small, topology-forgiving; the text prompts in the pack are enough.
- **Hero vehicles → Image-to-3D.** Vehicles need controlled proportions and a readable silhouette; a reference image gets you a usable base instead of a blobby guess.

## References for Image-to-3D (the input that matters most)
Feed Meshy a clean, single-object reference on a neutral background — ideally a clear 3/4 view plus a side view. Three ways to get one, best first:
1. **Orthographic reference sheet** — I can generate a front/side/top blockout sheet per vehicle (SVG/PNG) for you to upload. Ask and I'll produce them.
2. **Concept render** — if you connect Adobe/Firefly, I'll generate painted concepts from the bible to feed Meshy.
3. **Quick sketch or kitbash** — even a rough side-profile photo/sketch beats text-only.
(No reference handy? Text-to-3D still works; just expect more retries.)

## The loop, step by step (in Meshy)
1. **New generation** → choose Text-to-3D or Image-to-3D.
2. Paste the **Prompt** and **Negative prompt** from the pack (Text mode), or upload the reference (Image mode) + paste the prompt as guidance.
3. Settings: **Art Style = Realistic**, **Symmetry** per the asset, **Topology = Quad**, **Target Polycount** per the asset, **PBR textures = on**.
4. **Generate Preview** — you get ~4 geometry candidates (cheap on credits). Pick the best silhouette.
5. **Refine** the chosen one (better geometry), then **generate Textures** (2K, or 4K for hero assets).
6. Review. If it's off, tweak one thing in the prompt or reroll the seed before spending more credits. Use **Retexture** to fix only the look.
7. **Remesh** to hit your quad + polycount target.
8. **Export FBX** (with textures). Note the scale and up-axis for the cleanup step.

## Credit-smart batching (do this, Pro credits aren't infinite)
- Prototype on **Previews** before you Refine/Texture — that's where credits go.
- Nail your settings on **one** weapon first, then batch the rest with the same recipe.
- **Order:** (1) the 14 pickup weapons + simple props — fast, cheap wins; (2) the 16 launch vehicles; (3) the 10 boss vehicles last (most complex). Save good prompts/seeds as you go.

## Cleanup pass (Blender) — checklist
- [ ] Import the FBX; set **real-world scale** (a sedan ≈ 4.5 m long; match your gym vehicle).
- [ ] **Separate the wheels** into their own objects and set pivots/origins (vehicles need spinning wheels).
- [ ] Split any moving/special part (crane boom, magnet, ladle, water cannon, rear doors) into its own object.
- [ ] Retopo/decimate to the polycount budget if Meshy overshot; check normals.
- [ ] Merge/reduce materials; confirm PBR maps (base color, normal, ORM) assign cleanly.
- [ ] Export to FBX for Unreal.

## Unreal import + hookup (per TECHNICAL_DESIGN.md)
- Import FBX (units = cm, Z-up), generate or author collision, assign the PBR material instances.
- Build the physics asset / wheel setup; slot the mesh into `ABLCombatVehicle`.
- Add **sockets**: primary-fire muzzle, special-weapon emitter, the diegetic detail (e.g., Surgeon's monitor).
- Author the `UBLCharacterDataAsset` (stats from the tuning sheet) and wire it up.
- Name per `CONVENTIONS.md` (`SM_`/`SK_`, `M_`, `T_`).

## What Meshy will NOT do (handle these in-engine/art)
- **Damage states** (dents → smoke → fire): generate the clean mesh; author the ladder in UE (material states + Chaos).
- **Glow, fire, water, electricity** (Hollow's heat, Jackknife's flames, Undertow's water, the Foundryman's molten core): those are **VFX/materials**, not modeled — prompts describe the object, not the effect.
- **Clutter-heavy rigs** (the Lien's hung trophies, the Cartographer's maps): generate the **base vehicle**, add the clutter as separate small props or decals so it stays clean and usable.
- **The Auditor's shifting car:** generate one sinister base; do the "shifting/redaction" via material/VFX, not multiple models.

## Per-asset tracker (copy into a sheet or GitHub issue)
| Asset | Prompt locked | Generated | Cleaned | In-engine |
|---|---|---|---|---|
| (one row per weapon/vehicle) | ☐ | ☐ | ☐ | ☐ |
