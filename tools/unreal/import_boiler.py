# Black Ledger - import the Sketchfab boiler (CC0) from tools/meshes_raw/industrial_boiler_01:
# one FBX with three material slots (Decals / Objects / Kuma_Rusted Metal), each with a full
# PBR set (base/AO/metal/normal/rough). Imports the mesh + 15 textures, builds three materials,
# assigns them to the matching slots by name, saves. make_mill_map.py then places 3 boilers
# on the furnaces.
#
# Run with the EDITOR CLOSED (no -nullrhi: it imports + compiles shaders):
#   & "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" `
#     "C:\Users\csmit\black-ledger\BlackLedger.uproject" `
#     -ExecCmds="py C:/Users/csmit/black-ledger/tools/unreal/import_boiler.py" `
#     -stdout -unattended -nosplash -nosound -nopause

import unreal, os, traceback

RAW = os.path.join(unreal.SystemLibrary.get_project_directory(), "tools", "meshes_raw", "industrial_boiler_01")
DEST = "/Game/BlackLedger/Meshes/industrial_boiler_01"
MESH_NAME = "SM_Mill_Boiler"

tools = unreal.AssetToolsHelpers.get_asset_tools()
eal = unreal.EditorAssetLibrary
mel = unreal.MaterialEditingLibrary
TCS = unreal.TextureCompressionSettings
ST = unreal.MaterialSamplerType
log = []

# matte-rust tuning: clamp roughness up + knock metallic down so the boiler reads as
# grim rusted iron, not polished chrome (the bundled maps run too glossy for our tone).
ROUGH_MIN = 0.62
METAL_SCALE = 0.30

# slot-name keyword -> material name + texture file prefix
TEXSETS = {
    "decal":  {"mat": "M_Boiler_Decals",  "prefix": "Boiler_Decals"},
    "object": {"mat": "M_Boiler_Objects", "prefix": "Boiler_Objects"},
    "rust":   {"mat": "M_Boiler_Rusted",  "prefix": "Kuma_Rusted Metal"},
}


def import_one(src, name):
    t = unreal.AssetImportTask()
    t.filename = src
    t.destination_path = DEST
    t.destination_name = name
    t.automated = True
    t.save = True
    t.replace_existing = True
    tools.import_asset_tasks([t])
    paths = t.get_editor_property("imported_object_paths")
    return paths[0] if paths else f"{DEST}/{name}"


def imp_tex(prefix, suffix, dest_name, srgb, comp):
    fname = f"{prefix}.png" if suffix == "" else f"{prefix}_{suffix}.png"
    src = os.path.join(RAW, fname)
    if not os.path.exists(src):
        log.append(f"WARN missing tex {fname}")
        return None
    tex = eal.load_asset(import_one(src, dest_name))
    if tex:
        tex.set_editor_property("srgb", srgb)
        tex.set_editor_property("compression_settings", comp)
        eal.save_asset(tex.get_path_name())
    return tex


def build_material(cfg):
    prefix, mat_name = cfg["prefix"], cfg["mat"]
    base = imp_tex(prefix, "", f"T_{mat_name}_BC", True, TCS.TC_DEFAULT)
    norm = imp_tex(prefix, "Normal", f"T_{mat_name}_N", False, TCS.TC_NORMALMAP)
    rough = imp_tex(prefix, "Roughness", f"T_{mat_name}_R", False, TCS.TC_MASKS)
    metal = imp_tex(prefix, "Metallic", f"T_{mat_name}_M", False, TCS.TC_MASKS)
    ao = imp_tex(prefix, "AO", f"T_{mat_name}_AO", False, TCS.TC_MASKS)

    p = f"{DEST}/{mat_name}"
    if eal.does_asset_exist(p):
        eal.delete_asset(p)
    mat = tools.create_asset(mat_name, DEST, unreal.Material, unreal.MaterialFactoryNew())

    def E(cls, x, y):
        return mel.create_material_expression(mat, cls, x, y)

    def sample(tex, st, y):
        n = E(unreal.MaterialExpressionTextureSample, -480, y)
        n.texture = tex
        n.sampler_type = st
        return n

    if base:
        mel.connect_material_property(sample(base, ST.SAMPLERTYPE_COLOR, -360), "RGB",
                                      unreal.MaterialProperty.MP_BASE_COLOR)
    if norm:
        mel.connect_material_property(sample(norm, ST.SAMPLERTYPE_NORMAL, 320), "RGB",
                                      unreal.MaterialProperty.MP_NORMAL)
    if ao:
        mel.connect_material_property(sample(ao, ST.SAMPLERTYPE_LINEAR_COLOR, 140), "R",
                                      unreal.MaterialProperty.MP_AMBIENT_OCCLUSION)
    # matte rust: clamp roughness UP (max with ROUGH_MIN) and scale metallic DOWN
    if rough:
        mx = E(unreal.MaterialExpressionMax, -160, -40)
        mx.set_editor_property("const_b", ROUGH_MIN)
        mel.connect_material_expressions(sample(rough, ST.SAMPLERTYPE_LINEAR_COLOR, -40), "R", mx, "A")
        mel.connect_material_property(mx, "", unreal.MaterialProperty.MP_ROUGHNESS)
    if metal:
        ml = E(unreal.MaterialExpressionMultiply, -160, -200)
        ml.set_editor_property("const_b", METAL_SCALE)
        mel.connect_material_expressions(sample(metal, ST.SAMPLERTYPE_LINEAR_COLOR, -200), "R", ml, "A")
        mel.connect_material_property(ml, "", unreal.MaterialProperty.MP_METALLIC)
    mel.recompile_material(mat)
    eal.save_asset(p)
    log.append(f"built {mat_name}")
    return mat


def import_mesh():
    ui = unreal.FbxImportUI()
    ui.import_mesh = True
    ui.import_as_skeletal = False
    ui.import_animations = False
    ui.import_materials = False
    ui.import_textures = False
    ui.mesh_type_to_import = unreal.FBXImportType.FBXIT_STATIC_MESH
    smd = ui.static_mesh_import_data
    smd.combine_meshes = True            # one boiler asset with its material slots
    smd.auto_generate_collision = True   # simple collision so vehicles can't drive through it
    smd.normal_import_method = unreal.FBXNormalImportMethod.FBXNIM_IMPORT_NORMALS

    t = unreal.AssetImportTask()
    t.filename = os.path.join(RAW, "Boiler.FBX")
    t.destination_path = DEST
    t.destination_name = MESH_NAME
    t.automated = True
    t.save = True
    t.replace_existing = True
    t.options = ui
    t.factory = unreal.FbxFactory()
    tools.import_asset_tasks([t])

    # locate the imported static mesh and ensure it's named MESH_NAME
    for a in eal.list_assets(DEST, recursive=False, include_folder=False):
        pkg = a.split(".")[0]
        obj = eal.load_asset(pkg)
        if isinstance(obj, unreal.StaticMesh):
            if pkg.rsplit("/", 1)[-1] != MESH_NAME:
                if eal.does_asset_exist(f"{DEST}/{MESH_NAME}"):
                    eal.delete_asset(f"{DEST}/{MESH_NAME}")
                eal.rename_asset(pkg, f"{DEST}/{MESH_NAME}")
            log.append(f"mesh -> {DEST}/{MESH_NAME}")
            return f"{DEST}/{MESH_NAME}"
    log.append("WARN: no static mesh found after import")
    return None


def assign(sm_path, mats_by_key):
    mesh = eal.load_asset(sm_path)
    slots = list(mesh.get_editor_property("static_materials"))
    for s in slots:
        sname = str(s.get_editor_property("material_slot_name")).lower()
        chosen = None
        for key, mat in mats_by_key.items():
            if key in sname:
                chosen = mat
                break
        if not chosen:
            chosen = mats_by_key.get("rust")  # default unmatched slots to the body metal
        s.set_editor_property("material_interface", chosen)
        log.append(f"slot '{sname}' -> {chosen.get_name() if chosen else 'none'}")
    mesh.set_editor_property("static_materials", slots)
    eal.save_asset(sm_path)


def main():
    unreal.SystemLibrary.execute_console_command(None, "Interchange.FeatureFlags.Import.FBX 0")
    if eal.does_directory_exist(DEST):
        eal.delete_directory(DEST)
    eal.make_directory(DEST)
    mats = {key: build_material(cfg) for key, cfg in TEXSETS.items()}
    sm = import_mesh()
    if sm:
        assign(sm, mats)
    eal.save_directory(DEST)
    unreal.log_warning("IMPORT BOILER OK")


try:
    main()
    print("IMPORT BOILER OK")
    for line in log:
        print("  " + line)
except Exception:
    print("IMPORT BOILER FAILED")
    traceback.print_exc()
    for line in log:
        print("  " + line)
finally:
    unreal.SystemLibrary.quit_editor()
