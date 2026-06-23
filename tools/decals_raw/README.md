# decals_raw — drop zone for CC0 decals (ambientCG etc.) for the Mill atmosphere pass (M6)

Extract each decal into its own subfolder here, then tell Claude the folder name + source.
Claude imports the maps, builds a deferred decal material, and places DecalActors
(scorch around the furnace pits/pours, rust streaks down the walls, oil stains on the
floor), then logs the source in CREDITS/ASSET_LICENSES.md.

Decals need an **Opacity / alpha** map (that's what makes them stamp with transparent
edges) — on ambientCG, download the ones in the **Decals** category, not tileable textures.

Layout:
```
tools/decals_raw/
  scorch_01/        <- one folder per decal
    *_Color.* / _BaseColor.*
    *_Opacity.* / _Mask.*     (REQUIRED - the alpha)
    *_Normal.*  *_Roughness.* (optional)
  rust_streak_01/
  oil_stain_01/
```
Good first set: 1 scorch/burn, 1 rust streak, 1 oil/grime. All CC0.
