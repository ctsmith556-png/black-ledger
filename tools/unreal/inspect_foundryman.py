# Black Ledger - inspect the Foundryman body mesh's material slots + bounds, so we can
# decide how to animate the modeled treads (pan a tread material slot vs. overlay strips).
#
# Run with the EDITOR CLOSED (read-only, -nullrhi is fine):
#   & "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" `
#     "C:\Users\csmit\black-ledger\BlackLedger.uproject" `
#     -ExecCmds="py C:/Users/csmit/black-ledger/tools/unreal/inspect_foundryman.py" `
#     -stdout -unattended -nosplash -nosound -nullrhi -nopause

import unreal, traceback

PATH = "/Game/BlackLedger/Characters/Foundryman/SM_Foundryman_Body"


def main():
    mesh = unreal.load_asset(PATH)
    if not mesh:
        unreal.log_error(f"FOUNDRYMAN INSPECT: mesh not found at {PATH}")
        return

    mats = mesh.get_editor_property("static_materials")
    unreal.log_warning(f"FOUNDRYMAN INSPECT: {len(mats)} material slot(s)")
    for i, m in enumerate(mats):
        slot = m.get_editor_property("material_slot_name")
        mi = m.get_editor_property("material_interface")
        name = mi.get_name() if mi else "None"
        unreal.log_warning(f"  slot {i}: name='{slot}'  material='{name}'")

    try:
        b = mesh.get_bounds()
        unreal.log_warning(f"  bounds box_extent={b.box_extent}  origin={b.origin}")
    except Exception as e:
        unreal.log_warning(f"  bounds read failed: {e}")

    # UV channel count drives whether a panning material is feasible
    try:
        n_uv = mesh.get_num_uv_channels(0)
        unreal.log_warning(f"  LOD0 UV channels: {n_uv}")
    except Exception as e:
        unreal.log_warning(f"  uv channel read failed: {e}")


try:
    main()
except Exception:
    unreal.log_error("FOUNDRYMAN INSPECT FAILED")
    unreal.log_error(traceback.format_exc())
finally:
    unreal.SystemLibrary.quit_editor()
