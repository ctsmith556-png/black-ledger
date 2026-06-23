# meshes_raw — drop zone for downloaded 3D models (FBX / glTF) for the Mill

Put each model in its own lowercase_underscore subfolder, together with any texture maps
that came with it. Then tell Claude the folder name + where you got it (for licensing).
Claude imports the mesh + textures into UE, builds/assigns the material, wires it into a
`MESHES` role in `make_mill_map.py`, and logs the source in `CREDITS/ASSET_LICENSES.md`.

Layout:
```
tools/meshes_raw/
  industrial_boiler_01/        <- one folder per model
    industrial_boiler_01.fbx   (the mesh; glTF/glb also fine)
    *_diff/_basecolor.*        (bundled textures, if any, kept alongside)
    *_normal.* *_rough.* ...
```

Notes:
- FBX or glTF/GLB both work with the Blender->UE flow.
- Real-world scale + a clean simple silhouette import best; Claude's placement auto-fits to
  measured bounds, so scale mismatches self-correct.
- Always note the **source + license** (CC0 / CC-BY / Fab Standard / etc.) so it can be
  logged before anything ships.
```
