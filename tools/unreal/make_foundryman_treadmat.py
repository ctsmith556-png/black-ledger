# Black Ledger - rebuild M_Foundryman_Body so the SINGLE-slot body shows scrolling tank-
# tread cleats on its lower track band, without needing a 2nd material slot or re-import.
#
# The body keeps its normal textured look everywhere EXCEPT a masked region (local Z below
# the deck, local X ahead of the cab), where it blends to dark metal cleats whose pattern
# is driven by LOCAL POSITION X + a "Scroll" param (the boss C++ drives Scroll by speed).
#
# Run with the EDITOR CLOSED (no -nullrhi: compiles shaders):
#   & "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" `
#     "C:\Users\csmit\black-ledger\BlackLedger.uproject" `
#     -ExecCmds="py C:/Users/csmit/black-ledger/tools/unreal/make_foundryman_treadmat.py" `
#     -stdout -unattended -nosplash -nosound -nopause

import unreal, traceback

DEST = "/Game/BlackLedger/Characters/Foundryman"
MAT = f"{DEST}/M_Foundryman_Body"
at = unreal.AssetToolsHelpers.get_asset_tools()
mel = unreal.MaterialEditingLibrary
eal = unreal.EditorAssetLibrary


def main():
    if eal.does_asset_exist(MAT):
        eal.delete_asset(MAT)
    mat = at.create_asset("M_Foundryman_Body", DEST, unreal.Material, unreal.MaterialFactoryNew())
    if not mat:
        raise RuntimeError("create_asset failed - CLOSE the Unreal editor and re-run.")

    def E(cls, x, y):
        return mel.create_material_expression(mat, cls, x, y)

    def tex(code, sampler=None):
        path = f"{DEST}/T_Foundryman_{code}"
        t = eal.load_asset(path)
        if not t:
            unreal.log_warning(f"texture missing: {path}")
            return None
        n = E(unreal.MaterialExpressionTextureSample, -1500, 0)
        n.texture = t
        if sampler:
            n.sampler_type = sampler
        return n

    bc = tex("BC")
    mtl = tex("M", unreal.MaterialSamplerType.SAMPLERTYPE_LINEAR_COLOR)
    rgh = tex("R", unreal.MaterialSamplerType.SAMPLERTYPE_LINEAR_COLOR)
    emi = tex("E")
    nrm = tex("N", unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL)

    # ---- cleat pattern from LOCAL POSITION X (marches along the track length) ----
    lp = E(unreal.MaterialExpressionLocalPosition, -1300, 500)
    maskX = E(unreal.MaterialExpressionComponentMask, -1120, 460)
    maskX.set_editor_property("r", True); maskX.set_editor_property("g", False)
    maskX.set_editor_property("b", False); maskX.set_editor_property("a", False)
    mel.connect_material_expressions(lp, "", maskX, "")
    maskZ = E(unreal.MaterialExpressionComponentMask, -1120, 560)
    maskZ.set_editor_property("r", False); maskZ.set_editor_property("g", False)
    maskZ.set_editor_property("b", True); maskZ.set_editor_property("a", False)
    mel.connect_material_expressions(lp, "", maskZ, "")

    cleats = E(unreal.MaterialExpressionScalarParameter, -1120, 360)
    cleats.set_editor_property("parameter_name", "Cleats")
    cleats.set_editor_property("default_value", 0.05)
    scroll = E(unreal.MaterialExpressionScalarParameter, -1120, 660)
    scroll.set_editor_property("parameter_name", "Scroll")
    scroll.set_editor_property("default_value", 0.0)

    xc = E(unreal.MaterialExpressionMultiply, -940, 400)
    mel.connect_material_expressions(maskX, "", xc, "A")
    mel.connect_material_expressions(cleats, "", xc, "B")
    xs = E(unreal.MaterialExpressionAdd, -800, 420)
    mel.connect_material_expressions(xc, "", xs, "A")
    mel.connect_material_expressions(scroll, "", xs, "B")
    fr = E(unreal.MaterialExpressionFrac, -660, 420)
    mel.connect_material_expressions(xs, "", fr, "")
    m2 = E(unreal.MaterialExpressionMultiply, -520, 420)
    m2.set_editor_property("const_b", 2.0)
    mel.connect_material_expressions(fr, "", m2, "A")
    s1 = E(unreal.MaterialExpressionSubtract, -400, 420)
    s1.set_editor_property("const_b", 1.0)
    mel.connect_material_expressions(m2, "", s1, "A")
    ab = E(unreal.MaterialExpressionAbs, -280, 420)
    mel.connect_material_expressions(s1, "", ab, "")
    tri = E(unreal.MaterialExpressionOneMinus, -160, 420)
    mel.connect_material_expressions(ab, "", tri, "")
    cleat = E(unreal.MaterialExpressionPower, -40, 420)
    cleat.set_editor_property("const_exponent", 2.2)
    mel.connect_material_expressions(tri, "", cleat, "Base")

    # dark<->mid metal by the cleat ridge
    dark = E(unreal.MaterialExpressionConstant3Vector, -40, 240)
    dark.set_editor_property("constant", unreal.LinearColor(0.02, 0.02, 0.024, 1.0))
    mid = E(unreal.MaterialExpressionConstant3Vector, -40, 320)
    mid.set_editor_property("constant", unreal.LinearColor(0.17, 0.17, 0.18, 1.0))
    treadcol = E(unreal.MaterialExpressionLinearInterpolate, 160, 300)
    mel.connect_material_expressions(dark, "", treadcol, "A")
    mel.connect_material_expressions(mid, "", treadcol, "B")
    mel.connect_material_expressions(cleat, "", treadcol, "Alpha")

    # ---- region mask: Z below the deck AND X ahead of the cab (smooth edges) ----
    treadz = E(unreal.MaterialExpressionScalarParameter, -940, 600)
    treadz.set_editor_property("parameter_name", "TreadZ")
    treadz.set_editor_property("default_value", 120.0)
    zdiff = E(unreal.MaterialExpressionSubtract, -800, 600)   # TreadZ - Z
    mel.connect_material_expressions(treadz, "", zdiff, "A")
    mel.connect_material_expressions(maskZ, "", zdiff, "B")
    zsc = E(unreal.MaterialExpressionMultiply, -660, 600)
    zsc.set_editor_property("const_b", 0.04)                  # ~25-unit soft edge
    mel.connect_material_expressions(zdiff, "", zsc, "A")
    zmask = E(unreal.MaterialExpressionClamp, -520, 600)
    mel.connect_material_expressions(zsc, "", zmask, "")

    cabx = E(unreal.MaterialExpressionScalarParameter, -940, 760)
    cabx.set_editor_property("parameter_name", "CabX")
    cabx.set_editor_property("default_value", -338.0)
    xdiff = E(unreal.MaterialExpressionSubtract, -800, 760)   # X - CabX
    mel.connect_material_expressions(maskX, "", xdiff, "A")
    mel.connect_material_expressions(cabx, "", xdiff, "B")
    xsc = E(unreal.MaterialExpressionMultiply, -660, 760)
    xsc.set_editor_property("const_b", 0.03)
    mel.connect_material_expressions(xdiff, "", xsc, "A")
    xmask = E(unreal.MaterialExpressionClamp, -520, 760)
    mel.connect_material_expressions(xsc, "", xmask, "")

    region = E(unreal.MaterialExpressionMultiply, -360, 680)
    mel.connect_material_expressions(zmask, "", region, "A")
    mel.connect_material_expressions(xmask, "", region, "B")

    # ---- base color: body texture blended to tread metal in the region ----
    basecol = E(unreal.MaterialExpressionLinearInterpolate, 420, 0)
    if bc:
        mel.connect_material_expressions(bc, "RGB", basecol, "A")
    else:
        fallback = E(unreal.MaterialExpressionConstant3Vector, 200, -80)
        fallback.set_editor_property("constant", unreal.LinearColor(0.08, 0.08, 0.085, 1.0))
        mel.connect_material_expressions(fallback, "", basecol, "A")
    mel.connect_material_expressions(treadcol, "", basecol, "B")
    mel.connect_material_expressions(region, "", basecol, "Alpha")
    mel.connect_material_property(basecol, "", unreal.MaterialProperty.MP_BASE_COLOR)

    # metallic / roughness / normal straight from textures
    if mtl:
        mel.connect_material_property(mtl, "RGB", unreal.MaterialProperty.MP_METALLIC)
    if rgh:
        mel.connect_material_property(rgh, "RGB", unreal.MaterialProperty.MP_ROUGHNESS)
    if nrm:
        mel.connect_material_property(nrm, "RGB", unreal.MaterialProperty.MP_NORMAL)

    # ---- emissive: body emit + hot ember glowing in the moving cleats ----
    ember = E(unreal.MaterialExpressionConstant3Vector, 200, 460)
    ember.set_editor_property("constant", unreal.LinearColor(1.0, 0.32, 0.06, 1.0))
    cq = E(unreal.MaterialExpressionMultiply, 200, 560)
    cq.set_editor_property("const_b", 0.5)
    mel.connect_material_expressions(cleat, "", cq, "A")
    rq = E(unreal.MaterialExpressionMultiply, 360, 560)       # region * cleat * 0.5
    mel.connect_material_expressions(region, "", rq, "A")
    mel.connect_material_expressions(cq, "", rq, "B")
    embcol = E(unreal.MaterialExpressionMultiply, 520, 500)
    mel.connect_material_expressions(ember, "", embcol, "A")
    mel.connect_material_expressions(rq, "", embcol, "B")
    if emi:
        emadd = E(unreal.MaterialExpressionAdd, 700, 440)
        mel.connect_material_expressions(emi, "RGB", emadd, "A")
        mel.connect_material_expressions(embcol, "", emadd, "B")
        mel.connect_material_property(emadd, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    else:
        mel.connect_material_property(embcol, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)

    mel.recompile_material(mat)
    eal.save_asset(MAT)
    unreal.log_warning("MADE M_Foundryman_Body with masked scrolling treads")


try:
    main()
except Exception:
    unreal.log_error("FOUNDRYMAN TREADMAT FAILED")
    unreal.log_error(traceback.format_exc())
finally:
    unreal.SystemLibrary.quit_editor()
