# textures_raw — drop zone for downloaded PBR texture sets (Poly Haven etc.)

Extract each texture's **ZIP** into its own subfolder here, then tell Claude the folder
name. Claude imports the maps into UE and builds a real material (extending
`make_fx_materials.py`), reassigns it to the Mill floor/walls, and logs the CC0 source
in `CREDITS/ASSET_LICENSES.md`.

Layout:
```
tools/textures_raw/
  concrete_floor_damaged_01/   <- extract the Poly Haven ZIP here
    concrete_floor_damaged_01_diff_2k.jpg   (base color)
    concrete_floor_damaged_01_nor_gl_2k.*   (normal, OpenGL)
    concrete_floor_damaged_01_rough_2k.jpg  (roughness)
    concrete_floor_damaged_01_ao_2k.jpg     (ambient occlusion)
    concrete_floor_damaged_01_disp_2k.*     (displacement, optional)
```

Rules: download the **ZIP** (not Blend/glTF), **2K** is plenty for tiling surfaces,
keep Poly Haven's original filenames. All Poly Haven assets are CC0.
