# Black Ledger - import downloaded PBR texture sets (Poly Haven etc.) and build real
# UE materials from them. Data-driven: add an entry to SETS, drop the maps in
# tools/textures_raw/<folder>/, run this, then run make_mill_map.py (the floor/walls
# prefer the PBR materials and fall back to the flat M_BL_Mill* if absent).
#
# Materials use WORLD-ALIGNED UVs (world XY / TileCm) so a single huge floor/wall cube
# tiles at a real-world size instead of stretching one texture across the whole plate.
#
# Run with the EDITOR CLOSED (no -nullrhi: it imports + compiles shaders):
#   & "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" `
#     "C:\Users\csmit\black-ledger\BlackLedger.uproject" `
#     -ExecCmds="py C:/Users/csmit/black-ledger/tools/unreal/import_textures.py" `
#     -stdout -unattended -nosplash -nosound -nopause

import unreal, traceback

RAW = "C:/Users/csmit/black-ledger/tools/textures_raw"
TEX_DIR = "/Game/BlackLedger/Textures"
FX_DIR = "/Game/BlackLedger/FX"

# Each material: source folder under RAW, world-tile size (cm), and the three maps.
SETS = {
    "M_BL_MillFloorPBR": {
        "folder": "concrete_floor_damaged_01",
        "tile_cm": 600.0,                  # concrete repeats every ~6 m across the floor
        "maps": {
            "basecolor": "concrete_floor_damaged_01_diff_2k.jpg",
            "roughness": "concrete_floor_damaged_01_rough_2k.exr",
            "normal":    "concrete_floor_damaged_01_nor_gl_2k.exr",
        },
    },
    "M_BL_MillWallPBR": {
        "folder": "rusty_metal_04",
        "projection": "triplanar",         # project on all 3 axes -> no vertical smear on walls
        "tile_cm": 300.0,                  # ~3 m rust panels
        "rough_const": 0.85,               # matte rust, never chrome
        "metallic_const": 0.08,            # rust is dielectric
        "maps": {
            "basecolor": "rusty_metal_04_diff_2k.jpg",
            "roughness": "rusty_metal_04_rough_2k.exr",
            "normal":    "rusty_metal_04_nor_gl_2k.exr",
            "metallic":  "rusty_metal_04_metal_2k.exr",
        },
    },
}

at = unreal.AssetToolsHelpers.get_asset_tools()
mel = unreal.MaterialEditingLibrary
eal = unreal.EditorAssetLibrary
TCS = unreal.TextureCompressionSettings
ST = unreal.MaterialSamplerType


def import_texture(src, dest_dir, name):
    task = unreal.AssetImportTask()
    task.set_editor_property("filename", src)
    task.set_editor_property("destination_path", dest_dir)
    task.set_editor_property("destination_name", name)
    task.set_editor_property("automated", True)
    task.set_editor_property("replace_existing", True)
    task.set_editor_property("save", True)
    at.import_asset_tasks([task])
    paths = task.get_editor_property("imported_object_paths")
    path = paths[0] if paths else f"{dest_dir}/{name}"
    tex = eal.load_asset(path)
    if not tex:
        raise RuntimeError(f"texture import failed: {src}")
    return tex


def config_texture(tex, srgb, comp, flip_green=False):
    tex.set_editor_property("srgb", srgb)
    tex.set_editor_property("compression_settings", comp)
    if flip_green:
        tex.set_editor_property("flip_green_channel", True)   # OpenGL nor_gl -> DirectX
    eal.save_asset(tex.get_path_name())


def build_set(mat_name, cfg):
    folder = cfg["folder"]
    src_dir = f"{RAW}/{folder}"
    dest_dir = f"{TEX_DIR}/{folder}"
    if not eal.does_directory_exist(dest_dir):
        eal.make_directory(dest_dir)

    m = cfg["maps"]
    proj = cfg.get("projection", "world_xy")   # "world_xy" (flat floor) | "triplanar" (walls)
    base = import_texture(f"{src_dir}/{m['basecolor']}", dest_dir, f"T_{folder}_BaseColor")
    config_texture(base, True, TCS.TC_DEFAULT)

    path = f"{FX_DIR}/{mat_name}"
    if eal.does_asset_exist(path):
        eal.delete_asset(path)
    mat = at.create_asset(mat_name, FX_DIR, unreal.Material, unreal.MaterialFactoryNew())

    def E(cls, x, y):
        return mel.create_material_expression(mat, cls, x, y)

    def mask(src, r, g, b, x, y):
        cm = E(unreal.MaterialExpressionComponentMask, x, y)
        cm.set_editor_property("r", r)
        cm.set_editor_property("g", g)
        cm.set_editor_property("b", b)
        cm.set_editor_property("a", False)
        mel.connect_material_expressions(src, "", cm, "")
        return cm

    def smp(tex, stype, uv, x, y):
        ts = E(unreal.MaterialExpressionTextureSample, x, y)
        ts.set_editor_property("texture", tex)
        ts.set_editor_property("sampler_type", stype)
        if uv is not None:
            mel.connect_material_expressions(uv, "", ts, "UVs")
        return ts

    tile = E(unreal.MaterialExpressionScalarParameter, -1600, 700)
    tile.set_editor_property("parameter_name", "TileCm")
    tile.set_editor_property("default_value", cfg["tile_cm"])

    if proj == "triplanar":
        # Project the texture on all THREE world planes and blend by |world normal|, so it
        # reads correctly on floors, vertical walls AND the round furnace platform with no
        # vertical smearing (the "shiny stripes"). Roughness/metallic are matte constants.
        wp = E(unreal.MaterialExpressionWorldPosition, -1600, 0)

        def plane_uv(r, g, b, y):
            mk = mask(wp, r, g, b, -1400, y)
            dv = E(unreal.MaterialExpressionDivide, -1200, y)
            mel.connect_material_expressions(mk, "", dv, "A")
            mel.connect_material_expressions(tile, "", dv, "B")
            return dv

        uv_xy = plane_uv(True, True, False, -160)
        uv_xz = plane_uv(True, False, True, 40)
        uv_yz = plane_uv(False, True, True, 240)

        vn = E(unreal.MaterialExpressionVertexNormalWS, -1600, 440)
        an = E(unreal.MaterialExpressionAbs, -1400, 440)
        mel.connect_material_expressions(vn, "", an, "")
        nx = mask(an, True, False, False, -1200, 400)
        ny = mask(an, False, True, False, -1200, 480)
        nz = mask(an, False, False, True, -1200, 560)

        sxy = smp(base, ST.SAMPLERTYPE_COLOR, uv_xy, -900, -260)
        sxz = smp(base, ST.SAMPLERTYPE_COLOR, uv_xz, -900, -100)
        syz = smp(base, ST.SAMPLERTYPE_COLOR, uv_yz, -900, 60)

        def weighted(sample_node, weight_node, y):
            mul = E(unreal.MaterialExpressionMultiply, -640, y)
            mel.connect_material_expressions(sample_node, "RGB", mul, "A")
            mel.connect_material_expressions(weight_node, "", mul, "B")
            return mul

        mxy = weighted(sxy, nz, -260)   # XY plane faces up/down -> weight by |n.z|
        mxz = weighted(sxz, ny, -100)   # XZ plane -> |n.y|
        myz = weighted(syz, nx, 60)     # YZ plane -> |n.x|
        a1 = E(unreal.MaterialExpressionAdd, -420, -180)
        mel.connect_material_expressions(mxy, "", a1, "A")
        mel.connect_material_expressions(mxz, "", a1, "B")
        a2 = E(unreal.MaterialExpressionAdd, -240, -100)
        mel.connect_material_expressions(a1, "", a2, "A")
        mel.connect_material_expressions(myz, "", a2, "B")
        mel.connect_material_property(a2, "", unreal.MaterialProperty.MP_BASE_COLOR)

        rc = E(unreal.MaterialExpressionConstant, -240, 200)
        rc.set_editor_property("r", float(cfg.get("rough_const", 0.85)))
        mel.connect_material_property(rc, "", unreal.MaterialProperty.MP_ROUGHNESS)
        mc = E(unreal.MaterialExpressionConstant, -240, 320)
        mc.set_editor_property("r", float(cfg.get("metallic_const", 0.05)))
        mel.connect_material_property(mc, "", unreal.MaterialProperty.MP_METALLIC)
    else:
        # world-aligned XY: one flat projection - perfect for the big flat ground plate.
        rough = import_texture(f"{src_dir}/{m['roughness']}", dest_dir, f"T_{folder}_Rough")
        config_texture(rough, False, TCS.TC_MASKS)
        norm = import_texture(f"{src_dir}/{m['normal']}", dest_dir, f"T_{folder}_Normal")
        config_texture(norm, False, TCS.TC_NORMALMAP, flip_green=True)
        div = E(unreal.MaterialExpressionDivide, -960, 0)
        mel.connect_material_expressions(mask(E(unreal.MaterialExpressionWorldPosition, -1320, 0),
                                              True, True, False, -1140, 0), "", div, "A")
        mel.connect_material_expressions(tile, "", div, "B")
        mel.connect_material_property(smp(base, ST.SAMPLERTYPE_COLOR, div, -700, -260),
                                      "RGB", unreal.MaterialProperty.MP_BASE_COLOR)
        ts_rough = smp(rough, ST.SAMPLERTYPE_MASKS, div, -700, 20)
        rmin = cfg.get("rough_min")
        if rmin is not None:
            mx = E(unreal.MaterialExpressionMax, -440, 20)
            mx.set_editor_property("const_b", float(rmin))
            mel.connect_material_expressions(ts_rough, "R", mx, "A")
            mel.connect_material_property(mx, "", unreal.MaterialProperty.MP_ROUGHNESS)
        else:
            mel.connect_material_property(ts_rough, "R", unreal.MaterialProperty.MP_ROUGHNESS)
        mel.connect_material_property(smp(norm, ST.SAMPLERTYPE_NORMAL, div, -700, 300),
                                      "RGB", unreal.MaterialProperty.MP_NORMAL)
        mconst = cfg.get("metallic_const")
        if mconst is not None:
            mc = E(unreal.MaterialExpressionConstant, -700, 560)
            mc.set_editor_property("r", float(mconst))
            mel.connect_material_property(mc, "", unreal.MaterialProperty.MP_METALLIC)
        elif m.get("metallic"):
            metal = import_texture(f"{src_dir}/{m['metallic']}", dest_dir, f"T_{folder}_Metal")
            config_texture(metal, False, TCS.TC_MASKS)
            mel.connect_material_property(smp(metal, ST.SAMPLERTYPE_MASKS, div, -700, 560),
                                          "R", unreal.MaterialProperty.MP_METALLIC)

    mel.recompile_material(mat)
    eal.save_asset(mat.get_path_name())
    unreal.log_warning(f"TEXTURES: built {mat_name} ({proj}) from {folder}")


def main():
    if not eal.does_directory_exist(TEX_DIR):
        eal.make_directory(TEX_DIR)
    for mat_name, cfg in SETS.items():
        build_set(mat_name, cfg)
    unreal.log_warning("IMPORT TEXTURES OK")


try:
    main()
except Exception:
    unreal.log_error("IMPORT TEXTURES FAILED")
    unreal.log_error(traceback.format_exc())
finally:
    unreal.SystemLibrary.quit_editor()
