# UE Import Walkthrough — vehicle round-trip (manual)

Step-by-step for importing a prepped vehicle (`<Name>_UE.fbx` + textures) into Unreal, written for the Surgeon. Same flow for all 26 vehicles. The scripted version of this is `tools/unreal/import_vehicle.py` — use either.

## 1. Open the project + Content Drawer

1. Epic Launcher → UE 5.7 → open **BlackLedger** (Recent Projects).
2. Bottom-left of the editor: **Content Drawer** button (or `Ctrl+Space`). This is your asset browser — everything under `Content/` on disk shows as `/Game/` here.

## 2. Create the folder

1. In the Content Drawer, click **Content** in the left tree (the root).
2. Right-click in the empty area → **New Folder** → name it `BlackLedger`.
3. Inside it create `Characters`, inside that `Surgeon`.
   (Result: `Content/BlackLedger/Characters/Surgeon` — matches `PROJECT_STRUCTURE.md`.)

## 3. Import the FBX (5 static meshes)

1. Open the `Surgeon` folder → click **Import** (top of Content Drawer).
2. Pick `C:\Users\csmit\black-ledger\art\source\vehicles\01_Surgeon\Surgeon_UE.fbx`.
3. In the **FBX Import Options** dialog:
   - **Skeletal Mesh: OFF** — these are static meshes.
   - Open the **Mesh** section's advanced arrow → **Combine Meshes: OFF** ← *the critical one; you want body + 4 wheels as 5 assets.*
   - **Generate Missing Collision: ON**
   - **Normal Import Method: Import Normals**
   - **Material Import Method: Do Not Create Material**, **Import Textures: OFF** (we build the material ourselves in step 5).
4. Click **Import All**. You get `SM_Surgeon_Body` + `SM_Surgeon_Wheel_FL/FR/RL/RR`.

## 4. Import the textures

1. Same folder → **Import** → multi-select from `art\source\vehicles\01_Surgeon\`:
   `Surgeon_basecolor.png, Surgeon_normal.png, Surgeon_metallic.png, Surgeon_roughness.png, Surgeon_emit.png`
2. Rename them in UE (right-click → Rename, or F2) per conventions:
   `T_Surgeon_BC, T_Surgeon_N, T_Surgeon_M, T_Surgeon_R, T_Surgeon_E`
3. Texture settings (double-click each to open):
   - `T_Surgeon_N`: **Compression Settings = Normalmap**, **sRGB = OFF** (usually auto-detected — verify).
   - `T_Surgeon_M` and `T_Surgeon_R`: **sRGB = OFF** (they're data masks, not color). Save each.

## 5. Build the body material

1. Right-click in the folder → **Material** → name it `M_Surgeon_Body` → double-click to open the Material Editor.
2. Drag each texture from the Content Drawer into the graph (drops in as a Texture Sample node).
3. Connect (drag from the node's **RGB** pin to the main result node's input):
   - `T_Surgeon_BC` → **Base Color**
   - `T_Surgeon_M` → **Metallic**
   - `T_Surgeon_R` → **Roughness**
   - `T_Surgeon_E` → **Emissive Color**
   - `T_Surgeon_N` → **Normal**
4. If you get a sampler error on M/R: select the node → Details panel → **Sampler Type = Linear Color**.
5. **Apply** then **Save** (toolbar).

## 6. Wheel materials

1. Right-click → Material → `M_Surgeon_Tire`. Open it:
   - Hold **3** + click in the graph = a Constant3Vector (color) node. Double-click its swatch → near-black (0.02, 0.02, 0.022) → connect to **Base Color**.
   - Hold **1** + click = Constant. Set 0.92 → **Roughness**.
2. `M_Surgeon_Hub`: color (0.08, 0.08, 0.09) → Base Color; Constant 1.0 → **Metallic**; Constant 0.45 → **Roughness**.
3. Apply + Save both.

## 7. Assign materials

1. Double-click `SM_Surgeon_Body` → Details panel → **Material Slots** → set slot 0 to `M_Surgeon_Body` → **Save**.
2. Each wheel mesh: slot 0 = `M_Surgeon_Tire`, slot 1 = `M_Surgeon_Hub` → Save.

## 8. Look at it

1. Drag `SM_Surgeon_Body` from the Content Drawer into the viewport.
2. Navigation: **hold right-mouse + WASD** to fly, scroll to change speed. `F` frames the selected object.
3. Drag the 4 wheels in too if you want the full stance (they'll sit at world origin offsets — fine for eyeballing; the vehicle Blueprint assembles them properly later).
4. **Check the normal map:** if panel seams look *inverted* (dents where bumps should be), open `T_Surgeon_N` → check **Flip Green Channel** → Save. (Meshy exports OpenGL-style normals; UE expects DirectX.)

## 9. Save + commit

1. **Ctrl+Shift+S** (Save All) — new `.uasset` files land in `Content/BlackLedger/Characters/Surgeon/`.
2. Commit (Git Bash):
   ```bash
   git add Content/ art/source/vehicles/01_Surgeon/ tools/ docs/UE_IMPORT_WALKTHROUGH.md
   git commit -m "Import Surgeon: meshes, textures, materials"
   git push
   ```
