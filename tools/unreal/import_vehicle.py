# Black Ledger - headless UE import: prepped FBX + textures -> Content/BlackLedger/Characters/<Name>
# Imports the 5 static meshes, imports/renames textures, builds + wires the body
# material, creates tire/hub materials, assigns all slots, saves, quits the editor.
#
# Run with the EDITOR CLOSED (headless editor session; -run=pythonscript crashes on
# Slate-dependent FBX import, so we use -ExecCmds and the script quits the editor itself):
#   $env:BL_VEHICLE_NAME = "Surgeon"; $env:BL_VEHICLE_FOLDER = "01_Surgeon"
#   & "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" `
#     "C:\Users\csmit\black-ledger\BlackLedger.uproject" `
#     -ExecCmds="py C:/Users/csmit/black-ledger/tools/unreal/import_vehicle.py" `
#     -stdout -unattended -nosplash -nosound -nullrhi -nopause

import unreal, os, sys, traceback

NAME = os.environ.get("BL_VEHICLE_NAME") or (sys.argv[1] if len(sys.argv) > 1 else "Surgeon")
SRC_FOLDER = os.environ.get("BL_VEHICLE_FOLDER") or (sys.argv[2] if len(sys.argv) > 2 else "01_Surgeon")

proj_dir = unreal.SystemLibrary.get_project_directory()
src_dir = os.path.join(proj_dir, "art", "source", "vehicles", SRC_FOLDER)
dest = f"/Game/BlackLedger/Characters/{NAME}"
tools = unreal.AssetToolsHelpers.get_asset_tools()
eal = unreal.EditorAssetLibrary
mel = unreal.MaterialEditingLibrary
log = []


def import_fbx():
    ui = unreal.FbxImportUI()
    ui.import_mesh = True
    ui.import_as_skeletal = False
    ui.import_animations = False
    ui.import_materials = False
    ui.import_textures = False
    ui.mesh_type_to_import = unreal.FBXImportType.FBXIT_STATIC_MESH
    smd = ui.static_mesh_import_data
    smd.combine_meshes = False
    smd.auto_generate_collision = True
    smd.normal_import_method = unreal.FBXNormalImportMethod.FBXNIM_IMPORT_NORMALS

    task = unreal.AssetImportTask()
    task.filename = os.path.join(src_dir, f"{NAME}_UE.fbx")
    task.destination_path = dest
    task.automated = True
    task.save = True
    task.replace_existing = True
    task.options = ui
    task.factory = unreal.FbxFactory()  # pin the classic importer explicitly
    tools.import_asset_tasks([task])
    log.append(f"FBX imported from {task.filename}")
    normalize_mesh_names()


def normalize_mesh_names():
    """Interchange prefixes assets with the source filename (Surgeon_UE_SM_...).
    Strip that prefix so downstream steps find clean SM_<Name>_* names."""
    prefix = f"{NAME}_UE_"
    for a in eal.list_assets(dest, recursive=False, include_folder=False):
        pkg = a.split(".")[0]
        base = pkg.rsplit("/", 1)[-1]
        if base.startswith(prefix):
            clean = base[len(prefix):]
            if eal.does_asset_exist(f"{dest}/{clean}"):
                eal.delete_asset(f"{dest}/{clean}")
            eal.rename_asset(pkg, f"{dest}/{clean}")
            log.append(f"renamed {base} -> {clean}")


def import_textures():
    tex_codes = {"basecolor": "BC", "normal": "N", "metallic": "M", "roughness": "R", "emit": "E"}
    tex_tasks = []
    for suffix in tex_codes:
        f = os.path.join(src_dir, f"{NAME}_{suffix}.png")
        if os.path.exists(f):
            t = unreal.AssetImportTask()
            t.filename = f; t.destination_path = dest
            t.automated = True; t.save = True; t.replace_existing = True
            tex_tasks.append((t, suffix))
    tools.import_asset_tasks([t for t, _ in tex_tasks])

    textures = {}
    for t, suffix in tex_tasks:
        code = tex_codes[suffix]
        old = f"{dest}/{NAME}_{suffix}"
        new = f"{dest}/T_{NAME}_{code}"
        if eal.does_asset_exist(old):
            eal.rename_asset(old, new)
        tex = eal.load_asset(new)
        if not tex:
            log.append(f"WARN: texture missing {new}"); continue
        if code == "N":
            tex.set_editor_property("compression_settings", unreal.TextureCompressionSettings.TC_NORMALMAP)
            tex.set_editor_property("srgb", False)
        elif code in ("M", "R"):
            tex.set_editor_property("srgb", False)
        eal.save_asset(new)
        textures[code] = tex
        log.append(f"texture {new}")
    return textures


def build_body_material(textures):
    mat_path = f"{dest}/M_{NAME}_Body"
    if eal.does_asset_exist(mat_path):
        eal.delete_asset(mat_path)
    mat = tools.create_asset(f"M_{NAME}_Body", dest, unreal.Material, unreal.MaterialFactoryNew())
    if not mat:
        raise RuntimeError("create_asset failed - assets are locked. CLOSE the Unreal editor and re-run.")

    def wire(code, prop, y, sampler=None):
        if code not in textures:
            return
        node = mel.create_material_expression(mat, unreal.MaterialExpressionTextureSample, -420, y)
        node.texture = textures[code]
        if sampler:
            node.sampler_type = sampler
        mel.connect_material_property(node, "RGB", prop)

    wire("BC", unreal.MaterialProperty.MP_BASE_COLOR, -360)
    wire("M", unreal.MaterialProperty.MP_METALLIC, -180, unreal.MaterialSamplerType.SAMPLERTYPE_LINEAR_COLOR)
    wire("R", unreal.MaterialProperty.MP_ROUGHNESS, 0, unreal.MaterialSamplerType.SAMPLERTYPE_LINEAR_COLOR)
    wire("E", unreal.MaterialProperty.MP_EMISSIVE_COLOR, 180)
    wire("N", unreal.MaterialProperty.MP_NORMAL, 360, unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL)
    mel.recompile_material(mat)
    eal.save_asset(mat_path)
    log.append(f"material {mat_path}")
    return mat


def const_material(name, base, metallic, roughness, specular=None):
    p = f"{dest}/{name}"
    if eal.does_asset_exist(p):
        eal.delete_asset(p)
    m = tools.create_asset(name, dest, unreal.Material, unreal.MaterialFactoryNew())
    if not m:
        raise RuntimeError(f"create_asset failed for {name} - CLOSE the Unreal editor and re-run.")
    c = mel.create_material_expression(m, unreal.MaterialExpressionConstant3Vector, -380, -200)
    c.constant = unreal.LinearColor(*base, 1.0)
    mel.connect_material_property(c, "", unreal.MaterialProperty.MP_BASE_COLOR)
    mtl = mel.create_material_expression(m, unreal.MaterialExpressionConstant, -380, 0)
    mtl.r = metallic
    mel.connect_material_property(mtl, "", unreal.MaterialProperty.MP_METALLIC)
    r = mel.create_material_expression(m, unreal.MaterialExpressionConstant, -380, 120)
    r.r = roughness
    mel.connect_material_property(r, "", unreal.MaterialProperty.MP_ROUGHNESS)
    if specular is not None:
        s = mel.create_material_expression(m, unreal.MaterialExpressionConstant, -380, 240)
        s.r = specular
        mel.connect_material_property(s, "", unreal.MaterialProperty.MP_SPECULAR)
    mel.recompile_material(m)
    eal.save_asset(p)
    log.append(f"material {p}")
    return m


def assign(mesh_path, mats):
    mesh = eal.load_asset(mesh_path)
    if not mesh:
        log.append(f"WARN: mesh missing {mesh_path}"); return
    slots = list(mesh.get_editor_property("static_materials"))
    for i, m in enumerate(mats):
        if i < len(slots):
            slots[i].material_interface = m
    mesh.set_editor_property("static_materials", slots)
    eal.save_asset(mesh_path)
    log.append(f"assigned {mesh_path} ({len(slots)} slot(s))")


def main():
    # force the classic FBX importer (Interchange reimports are async + ignore FbxImportUI)
    unreal.SystemLibrary.execute_console_command(None, "Interchange.FeatureFlags.Import.FBX 0")
    # idempotent: wipe the destination so every run is a clean first-time import
    if eal.does_directory_exist(dest):
        eal.delete_directory(dest)
        log.append(f"cleared {dest}")
    import_fbx()
    textures = import_textures()
    body_mat = build_body_material(textures)
    tire = const_material(f"M_{NAME}_Tire", (0.02, 0.02, 0.022), 0.0, 0.92, specular=0.1)
    hub = const_material(f"M_{NAME}_Hub", (0.08, 0.08, 0.09), 1.0, 0.45)
    well = const_material(f"M_{NAME}_Well", (0.008, 0.008, 0.009), 0.0, 0.97, specular=0.0)
    # slot 0 = painted body, slot 1 = wheel-well liner (reads as pure shadow)
    assign(f"{dest}/SM_{NAME}_Body", [body_mat, well])
    for tag in ("FL", "FR", "RL", "RR"):
        assign(f"{dest}/SM_{NAME}_Wheel_{tag}", [tire, hub])
    eal.save_directory(dest)


try:
    main()
    print("IMPORT OK")
    for line in log:
        print("  " + line)
except Exception:
    print("IMPORT FAILED")
    traceback.print_exc()
    for line in log:
        print("  " + line)
finally:
    unreal.SystemLibrary.quit_editor()
