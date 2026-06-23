# Black Ledger - ADDITIVE tweak: load L_Mill, fix the two slag-trench ramps in place, save.
# Does NOT rebuild the level, so hand-placed rubble + all other edits are preserved.
# Run with the EDITOR CLOSED (no -nullrhi):
#   & "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" `
#     "C:\Users\csmit\black-ledger\BlackLedger.uproject" `
#     -ExecCmds="py C:/Users/csmit/black-ledger/tools/unreal/fix_trench_ramps.py" `
#     -stdout -unattended -nosplash -nosound -nopause

import unreal, traceback

MAP = "/Game/BlackLedger/Maps/L_Mill"
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors_sub = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

# label -> (location, scale, rotation(roll,pitch,yaw)) - thick (anti-tunnel), gentle 6.65 deg,
# top crests ~18 cm above the floor, bottom overlaps the trench floor (z=-400).
RAMPS = {
    "Floor_TrenchRamp_W": ((-14300.0, -7000.0, -265.0), (36.0, 20.0, 1.5), (0.0, -6.65, 0.0)),
    "Floor_TrenchRamp_E": ((14300.0, -7000.0, -265.0), (36.0, 20.0, 1.5), (0.0, 6.65, 0.0)),
}


def main():
    if not les.load_level(MAP):
        raise RuntimeError(f"could not load {MAP}")
    found = 0
    for a in actors_sub.get_all_level_actors():
        lbl = a.get_actor_label()
        if lbl in RAMPS:
            loc, scl, rot = RAMPS[lbl]
            a.set_actor_location(unreal.Vector(*loc), False, False)
            a.set_actor_rotation(unreal.Rotator(rot[0], rot[1], rot[2]), False)
            a.set_actor_scale3d(unreal.Vector(*scl))
            found += 1
            unreal.log_warning(f"updated {lbl}")
    if found < 2:
        unreal.log_warning(f"WARNING: only found {found}/2 trench ramps")
    if not les.save_current_level():
        raise RuntimeError("save_current_level failed")
    unreal.log_warning(f"TRENCH RAMPS FIXED ({found}) - rubble + level preserved")


try:
    main()
except Exception:
    unreal.log_error("FIX TRENCH RAMPS FAILED")
    unreal.log_error(traceback.format_exc())
finally:
    unreal.SystemLibrary.quit_editor()
