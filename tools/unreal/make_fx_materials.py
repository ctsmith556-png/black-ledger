# Black Ledger - create the procedural-FX materials the C++ effects load at runtime:
#   /Game/BlackLedger/FX/M_BL_Emissive  - unlit, Emissive = Color * Strength (fireballs,
#                                          sparks, tracers, molten glow)
#   /Game/BlackLedger/FX/M_BL_Smoke     - unlit translucent, tinted, Opacity param (smoke)
# Both expose parameters so the C++ drives them through dynamic material instances.
# Niagara can't be scripted headlessly, but materials can - this keeps the FX pass
# inside the same script-driven pipeline as everything else.
#
# Run with the EDITOR CLOSED (no -nullrhi: it compiles shaders):
#   & "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" `
#     "C:\Users\csmit\black-ledger\BlackLedger.uproject" `
#     -ExecCmds="py C:/Users/csmit/black-ledger/tools/unreal/make_fx_materials.py" `
#     -stdout -unattended -nosplash -nosound -nopause

import unreal, traceback

FX_DIR = "/Game/BlackLedger/FX"
at = unreal.AssetToolsHelpers.get_asset_tools()
mel = unreal.MaterialEditingLibrary
eal = unreal.EditorAssetLibrary


def fresh(name):
    path = f"{FX_DIR}/{name}"
    if eal.does_asset_exist(path):
        eal.delete_asset(path)
    return at.create_asset(name, FX_DIR, unreal.Material, unreal.MaterialFactoryNew())


def vec_param(mat, name, color, x, y):
    e = mel.create_material_expression(mat, unreal.MaterialExpressionVectorParameter, x, y)
    e.set_editor_property("parameter_name", name)
    e.set_editor_property("default_value", color)
    return e


def scalar_param(mat, name, val, x, y):
    e = mel.create_material_expression(mat, unreal.MaterialExpressionScalarParameter, x, y)
    e.set_editor_property("parameter_name", name)
    e.set_editor_property("default_value", val)
    return e


def make_emissive():
    mat = fresh("M_BL_Emissive")
    mat.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_UNLIT)
    color = vec_param(mat, "Color", unreal.LinearColor(1.0, 0.45, 0.10, 1.0), -520, -40)
    strength = scalar_param(mat, "Strength", 6.0, -520, 180)
    mul = mel.create_material_expression(mat, unreal.MaterialExpressionMultiply, -220, 0)
    mel.connect_material_expressions(color, "", mul, "A")
    mel.connect_material_expressions(strength, "", mul, "B")
    mel.connect_material_property(mul, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    mel.recompile_material(mat)
    eal.save_asset(mat.get_path_name())
    unreal.log_warning("made M_BL_Emissive")


def make_smoke():
    mat = fresh("M_BL_Smoke")
    mat.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_UNLIT)
    mat.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)
    color = vec_param(mat, "Color", unreal.LinearColor(0.045, 0.045, 0.05, 1.0), -520, -40)
    opacity = scalar_param(mat, "Opacity", 0.55, -520, 180)
    mel.connect_material_property(color, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    mel.connect_material_property(opacity, "", unreal.MaterialProperty.MP_OPACITY)
    mel.recompile_material(mat)
    eal.save_asset(mat.get_path_name())
    unreal.log_warning("made M_BL_Smoke")


def make_molten():
    # Animated molten slag for the furnace pours: a scrolling procedural-noise field lerps
    # a dark crust -> hot core, unlit emissive, tinted by "Color" and scaled by "Strength"
    # (so the C++ pour-pool MID still drives it). Replaces the flat orange disc with flowing
    # lava. All knobs are params: Tiling, Color, Strength (+ Panner speeds in-graph).
    mat = fresh("M_BL_MoltenSlag")
    mat.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_UNLIT)

    def E(cls, x, y):
        return mel.create_material_expression(mat, cls, x, y)

    tc = E(unreal.MaterialExpressionTextureCoordinate, -1300, -120)
    tiling = scalar_param(mat, "Tiling", 4.0, -1300, 120)
    uv = E(unreal.MaterialExpressionMultiply, -1100, -60)
    mel.connect_material_expressions(tc, "", uv, "A")
    mel.connect_material_expressions(tiling, "", uv, "B")

    # scroll the field so the lava flows
    pan = E(unreal.MaterialExpressionPanner, -900, -60)
    pan.set_editor_property("speed_x", 0.03)
    pan.set_editor_property("speed_y", 0.05)
    mel.connect_material_expressions(uv, "", pan, "Coordinate")

    # drift a 3rd (time) axis so the noise slice evolves instead of just sliding
    tm = E(unreal.MaterialExpressionTime, -1100, 170)
    tz = E(unreal.MaterialExpressionMultiply, -900, 170)
    tz.set_editor_property("const_b", 0.12)
    mel.connect_material_expressions(tm, "", tz, "A")

    pos = E(unreal.MaterialExpressionAppendVector, -700, 20)
    mel.connect_material_expressions(pan, "", pos, "A")
    mel.connect_material_expressions(tz, "", pos, "B")

    noise = E(unreal.MaterialExpressionNoise, -520, 20)
    noise.set_editor_property("scale", 0.5)
    mel.connect_material_expressions(pos, "", noise, "Position")

    # noise (~-1..1) -> 0..1
    half = E(unreal.MaterialExpressionMultiply, -340, 20)
    half.set_editor_property("const_b", 0.5)
    mel.connect_material_expressions(noise, "", half, "A")
    t = E(unreal.MaterialExpressionAdd, -200, 20)
    t.set_editor_property("const_b", 0.5)
    mel.connect_material_expressions(half, "", t, "A")

    crust = E(unreal.MaterialExpressionConstant3Vector, -340, -190)
    crust.set_editor_property("constant", unreal.LinearColor(0.35, 0.04, 0.01, 1.0))   # cooling skin
    hot = E(unreal.MaterialExpressionConstant3Vector, -340, -95)
    hot.set_editor_property("constant", unreal.LinearColor(1.7, 0.75, 0.12, 1.0))      # molten core
    lava = E(unreal.MaterialExpressionLinearInterpolate, 20, -60)
    mel.connect_material_expressions(crust, "", lava, "A")
    mel.connect_material_expressions(hot, "", lava, "B")
    mel.connect_material_expressions(t, "", lava, "Alpha")

    color = vec_param(mat, "Color", unreal.LinearColor(1.0, 0.5, 0.15, 1.0), 20, 210)
    tint = E(unreal.MaterialExpressionMultiply, 220, 20)
    mel.connect_material_expressions(lava, "", tint, "A")
    mel.connect_material_expressions(color, "", tint, "B")

    strength = scalar_param(mat, "Strength", 5.0, 220, 210)
    em = E(unreal.MaterialExpressionMultiply, 400, 70)
    mel.connect_material_expressions(tint, "", em, "A")
    mel.connect_material_expressions(strength, "", em, "B")
    mel.connect_material_property(em, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)

    mel.recompile_material(mat)
    eal.save_asset(mat.get_path_name())
    unreal.log_warning("made M_BL_MoltenSlag")


def make_tread():
    # A scrolling-cleat tread belt for the Foundryman. The C++ drives a "Scroll"
    # scalar from forward speed; the cleats march so the treads read as moving.
    # Pattern: tri = 1 - abs(2*frac(U*Cleats + Scroll) - 1), sharpened, lerped
    # between dark and mid metal, with a faint heat emissive in the cleat gaps.
    mat = fresh("M_BL_Tread")

    def E(cls, x, y):
        return mel.create_material_expression(mat, cls, x, y)

    tc = E(unreal.MaterialExpressionTextureCoordinate, -1000, 0)
    maskU = E(unreal.MaterialExpressionComponentMask, -820, 0)
    maskU.set_editor_property("r", True)
    maskU.set_editor_property("g", False)
    maskU.set_editor_property("b", False)
    maskU.set_editor_property("a", False)
    mel.connect_material_expressions(tc, "", maskU, "")

    cleats = scalar_param(mat, "Cleats", 24.0, -1000, 200)
    mulU = E(unreal.MaterialExpressionMultiply, -640, 40)
    mel.connect_material_expressions(maskU, "", mulU, "A")
    mel.connect_material_expressions(cleats, "", mulU, "B")

    scroll = scalar_param(mat, "Scroll", 0.0, -1000, 360)
    addS = E(unreal.MaterialExpressionAdd, -480, 60)
    mel.connect_material_expressions(mulU, "", addS, "A")
    mel.connect_material_expressions(scroll, "", addS, "B")

    fr = E(unreal.MaterialExpressionFrac, -340, 60)
    mel.connect_material_expressions(addS, "", fr, "")

    # triangle wave: 1 - abs(2*fr - 1)
    m2 = E(unreal.MaterialExpressionMultiply, -200, 60)
    m2.set_editor_property("const_b", 2.0)
    mel.connect_material_expressions(fr, "", m2, "A")
    sub1 = E(unreal.MaterialExpressionSubtract, -60, 60)
    sub1.set_editor_property("const_b", 1.0)
    mel.connect_material_expressions(m2, "", sub1, "A")
    ab = E(unreal.MaterialExpressionAbs, 80, 60)
    mel.connect_material_expressions(sub1, "", ab, "")
    tri = E(unreal.MaterialExpressionOneMinus, 220, 60)
    mel.connect_material_expressions(ab, "", tri, "")
    bar = E(unreal.MaterialExpressionPower, 360, 60)
    bar.set_editor_property("const_exponent", 2.2)   # softer, wider cleats (less "slashy")
    mel.connect_material_expressions(tri, "", bar, "Base")

    # base color: dark metal -> mid metal by the cleat mask
    dark = E(unreal.MaterialExpressionConstant3Vector, 360, -180)
    dark.set_editor_property("constant", unreal.LinearColor(0.02, 0.02, 0.025, 1.0))
    light = E(unreal.MaterialExpressionConstant3Vector, 360, -80)
    light.set_editor_property("constant", unreal.LinearColor(0.17, 0.17, 0.185, 1.0))  # brighter so cleats catch furnace light
    basecol = E(unreal.MaterialExpressionLinearInterpolate, 560, -40)
    mel.connect_material_expressions(dark, "", basecol, "A")
    mel.connect_material_expressions(light, "", basecol, "B")
    mel.connect_material_expressions(bar, "", basecol, "Alpha")
    mel.connect_material_property(basecol, "", unreal.MaterialProperty.MP_BASE_COLOR)

    # VERY faint ember in the cleat gaps (he runs hot) - kept low so the treads read as
    # dark metal rungs, not glowing molten slashes (the old "inverted" look)
    heat = E(unreal.MaterialExpressionConstant3Vector, 360, 260)
    heat.set_editor_property("constant", unreal.LinearColor(0.5, 0.14, 0.02, 1.0))
    barq = E(unreal.MaterialExpressionMultiply, 560, 200)
    barq.set_editor_property("const_b", 0.06)
    mel.connect_material_expressions(bar, "", barq, "A")
    em = E(unreal.MaterialExpressionMultiply, 720, 240)
    mel.connect_material_expressions(heat, "", em, "A")
    mel.connect_material_expressions(barq, "", em, "B")
    mel.connect_material_property(em, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)

    mel.recompile_material(mat)
    eal.save_asset(mat.get_path_name())
    unreal.log_warning("made M_BL_Tread")


def make_foundryman_tread():
    # The Foundryman's REAL tread band (split into material slot 1 in Blender) gets this.
    # Cleats are driven by LOCAL POSITION X (not UV), so they march cleanly along the track
    # length on the actual geometry regardless of the mesh's messy auto-UVs. C++ drives
    # "Scroll" from his forward speed; "Cleats" tunes spacing.
    mat = fresh("M_BL_FoundrymanTread")

    def E(cls, x, y):
        return mel.create_material_expression(mat, cls, x, y)

    lp = E(unreal.MaterialExpressionLocalPosition, -1120, 0)
    maskX = E(unreal.MaterialExpressionComponentMask, -900, 0)
    maskX.set_editor_property("r", True)
    maskX.set_editor_property("g", False)
    maskX.set_editor_property("b", False)
    maskX.set_editor_property("a", False)
    mel.connect_material_expressions(lp, "", maskX, "")

    cleats = scalar_param(mat, "Cleats", 0.06, -1120, 220)   # ~cleat every 1/0.06 local units
    mulU = E(unreal.MaterialExpressionMultiply, -700, 40)
    mel.connect_material_expressions(maskX, "", mulU, "A")
    mel.connect_material_expressions(cleats, "", mulU, "B")

    scroll = scalar_param(mat, "Scroll", 0.0, -1120, 340)
    addS = E(unreal.MaterialExpressionAdd, -540, 60)
    mel.connect_material_expressions(mulU, "", addS, "A")
    mel.connect_material_expressions(scroll, "", addS, "B")

    fr = E(unreal.MaterialExpressionFrac, -400, 60)
    mel.connect_material_expressions(addS, "", fr, "")

    # triangle wave: 1 - abs(2*fr - 1)
    m2 = E(unreal.MaterialExpressionMultiply, -260, 60)
    m2.set_editor_property("const_b", 2.0)
    mel.connect_material_expressions(fr, "", m2, "A")
    sub1 = E(unreal.MaterialExpressionSubtract, -120, 60)
    sub1.set_editor_property("const_b", 1.0)
    mel.connect_material_expressions(m2, "", sub1, "A")
    ab = E(unreal.MaterialExpressionAbs, 20, 60)
    mel.connect_material_expressions(sub1, "", ab, "")
    tri = E(unreal.MaterialExpressionOneMinus, 160, 60)
    mel.connect_material_expressions(ab, "", tri, "")
    bar = E(unreal.MaterialExpressionPower, 300, 60)
    bar.set_editor_property("const_exponent", 2.2)
    mel.connect_material_expressions(tri, "", bar, "Base")

    # base color: dark metal -> mid metal by the cleat ridge
    dark = E(unreal.MaterialExpressionConstant3Vector, 300, -210)
    dark.set_editor_property("constant", unreal.LinearColor(0.02, 0.02, 0.024, 1.0))
    light = E(unreal.MaterialExpressionConstant3Vector, 300, -110)
    light.set_editor_property("constant", unreal.LinearColor(0.22, 0.22, 0.235, 1.0))  # brighter so cleats read
    basecol = E(unreal.MaterialExpressionLinearInterpolate, 520, -60)
    mel.connect_material_expressions(dark, "", basecol, "A")
    mel.connect_material_expressions(light, "", basecol, "B")
    mel.connect_material_expressions(bar, "", basecol, "Alpha")
    mel.connect_material_property(basecol, "", unreal.MaterialProperty.MP_BASE_COLOR)

    # oily track metal
    metal = E(unreal.MaterialExpressionConstant, 520, 110)
    metal.set_editor_property("r", 0.85)
    mel.connect_material_property(metal, "", unreal.MaterialProperty.MP_METALLIC)
    rough = E(unreal.MaterialExpressionConstant, 520, 200)
    rough.set_editor_property("r", 0.55)
    mel.connect_material_property(rough, "", unreal.MaterialProperty.MP_ROUGHNESS)

    # hot ember glowing in the cleat ridges (he runs hot) - bright enough that the
    # marching cleats are clearly visible even in the dark mill; tune down later if needed
    heat = E(unreal.MaterialExpressionConstant3Vector, 300, 330)
    heat.set_editor_property("constant", unreal.LinearColor(1.0, 0.32, 0.06, 1.0))
    barq = E(unreal.MaterialExpressionMultiply, 520, 330)
    barq.set_editor_property("const_b", 0.9)
    mel.connect_material_expressions(bar, "", barq, "A")
    em = E(unreal.MaterialExpressionMultiply, 690, 350)
    mel.connect_material_expressions(heat, "", em, "A")
    mel.connect_material_expressions(barq, "", em, "B")
    mel.connect_material_property(em, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)

    mel.recompile_material(mat)
    eal.save_asset(mat.get_path_name())
    unreal.log_warning("made M_BL_FoundrymanTread")


def make_mill_materials():
    # Dark, rough industrial surfaces so the greybox reads as a grim mill even
    # before real Megascans textures (those swap in later). Lit, flat-color PBR.
    def simple(name, color, rough, metal):
        mat = fresh(name)
        c = mel.create_material_expression(mat, unreal.MaterialExpressionConstant3Vector, -400, 0)
        c.set_editor_property("constant", color)
        mel.connect_material_property(c, "", unreal.MaterialProperty.MP_BASE_COLOR)
        r = mel.create_material_expression(mat, unreal.MaterialExpressionConstant, -400, 160)
        r.set_editor_property("r", rough)
        mel.connect_material_property(r, "", unreal.MaterialProperty.MP_ROUGHNESS)
        if metal > 0:
            m = mel.create_material_expression(mat, unreal.MaterialExpressionConstant, -400, 300)
            m.set_editor_property("r", metal)
            mel.connect_material_property(m, "", unreal.MaterialProperty.MP_METALLIC)
        mel.recompile_material(mat)
        eal.save_asset(mat.get_path_name())
        unreal.log_warning(f"made {name}")

    simple("M_BL_MillFloor", unreal.LinearColor(0.050, 0.050, 0.055, 1.0), 0.92, 0.0)   # dark concrete
    simple("M_BL_MillWall",  unreal.LinearColor(0.040, 0.034, 0.030, 1.0), 0.55, 0.65)  # rusted dark metal


def main():
    if not eal.does_directory_exist(FX_DIR):
        eal.make_directory(FX_DIR)
    make_emissive()
    make_smoke()
    make_molten()
    make_tread()
    make_foundryman_tread()
    make_mill_materials()
    unreal.log_warning("FX MATERIALS OK")


try:
    main()
except Exception:
    unreal.log_error("FX MATERIALS FAILED")
    unreal.log_error(traceback.format_exc())
finally:
    unreal.SystemLibrary.quit_editor()
