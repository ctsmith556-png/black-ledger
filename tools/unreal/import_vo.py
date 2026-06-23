# Black Ledger - import voice-over recordings and build DA_BLVOBank (UBLVOBank),
# the data the UBLAudioSubsystem reads for barks / boss roars / the Auditor.
#
# DROP-IN LAYOUT - put wavs under tools/vo_raw/ like this, then run this script:
#   tools/vo_raw/characters/<CharacterKey>/<event>_<n>.wav
#       <CharacterKey> = Surgeon, Hollow, Antoinette, ...  (matches BLVehicleBios)
#       <event>        = taunt | hurt | kill | lowhealth | special | death
#   tools/vo_raw/bosses/<BossKey>/<event>_<n>.wav
#       <BossKey>      = Foundryman, ...
#       <event>        = rise | phase2 | phase3 | attack | death
#   tools/vo_raw/auditor/<n>.wav
# Multiple files per event = random variation in game. No files yet? This still
# builds an empty DA_BLVOBank so the slots are editable in the editor.
#
# Run with the EDITOR CLOSED (-nullrhi OK for audio import):
#   & "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" `
#     "C:\Users\csmit\black-ledger\BlackLedger.uproject" `
#     -ExecCmds="py C:/Users/csmit/black-ledger/tools/unreal/import_vo.py" `
#     -stdout -unattended -nosplash -nosound -nullrhi -nopause

import unreal, os, traceback

VO_RAW = "C:/Users/csmit/black-ledger/tools/vo_raw"
DEST = "/Game/BlackLedger/Audio/VO"

at = unreal.AssetToolsHelpers.get_asset_tools()
eal = unreal.EditorAssetLibrary

CHAR_EVENTS = ["taunt", "hurt", "kill", "lowhealth", "special", "death"]
BOSS_EVENTS = ["rise", "phase2", "phase3", "attack", "death"]
# bank property name per character event (UBLVOBank FBLCharacterVO snake_case)
CHAR_PROP = {"taunt": "taunt", "hurt": "hurt", "kill": "kill",
             "lowhealth": "low_health", "special": "special", "death": "death"}
BOSS_PROP = {"rise": "rise", "phase2": "phase2", "phase3": "phase3",
             "attack": "attack", "death": "death"}


def import_wav(src, dest_path, name):
    task = unreal.AssetImportTask()
    task.filename = src
    task.destination_path = dest_path
    task.destination_name = name
    task.automated = True
    task.replace_existing = True
    task.save = True
    at.import_asset_tasks([task])
    return unreal.load_asset(f"{dest_path}/{name}")


def import_event_dir(folder, dest_sub, prefix):
    # import every wav in `folder`, return the list of USoundWave assets
    out = []
    if not os.path.isdir(folder):
        return out
    for fn in sorted(os.listdir(folder)):
        if not fn.lower().endswith(".wav"):
            continue
        name = "VO_" + prefix + "_" + os.path.splitext(fn)[0]
        snd = import_wav(os.path.join(folder, fn), f"{DEST}/{dest_sub}", name)
        if snd:
            out.append(snd)
    return out


def main():
    if not eal.does_directory_exist(DEST):
        eal.make_directory(DEST)

    # ---- characters ----
    characters = {}
    char_root = os.path.join(VO_RAW, "characters")
    if os.path.isdir(char_root):
        for key in sorted(os.listdir(char_root)):
            cdir = os.path.join(char_root, key)
            if not os.path.isdir(cdir):
                continue
            cv = unreal.BLCharacterVO()
            any_line = False
            for ev in CHAR_EVENTS:
                lines = import_event_dir(os.path.join(cdir, ev), f"characters/{key}", f"{key}_{ev}")
                if lines:
                    cv.set_editor_property(CHAR_PROP[ev], lines)
                    any_line = True
            if any_line:
                characters[unreal.Name(key)] = cv

    # ---- bosses ----
    bosses = {}
    boss_root = os.path.join(VO_RAW, "bosses")
    if os.path.isdir(boss_root):
        for key in sorted(os.listdir(boss_root)):
            bdir = os.path.join(boss_root, key)
            if not os.path.isdir(bdir):
                continue
            bv = unreal.BLBossVO()
            any_line = False
            for ev in BOSS_EVENTS:
                lines = import_event_dir(os.path.join(bdir, ev), f"bosses/{key}", f"{key}_{ev}")
                if lines:
                    bv.set_editor_property(BOSS_PROP[ev], lines)
                    any_line = True
            if any_line:
                bosses[unreal.Name(key)] = bv

    # ---- auditor ----
    auditor = import_event_dir(os.path.join(VO_RAW, "auditor"), "auditor", "Auditor")

    # ---- build the bank ----
    bank_path = f"/Game/BlackLedger/Audio/DA_BLVOBank"
    if eal.does_asset_exist(bank_path):
        eal.delete_asset(bank_path)
    bank = at.create_asset("DA_BLVOBank", "/Game/BlackLedger/Audio", unreal.BLVOBank, None)
    if not bank:
        raise RuntimeError("could not create DA_BLVOBank (is the C++ built?)")
    if characters:
        bank.set_editor_property("characters", characters)
    if bosses:
        bank.set_editor_property("bosses", bosses)
    if auditor:
        bank.set_editor_property("auditor_lines", auditor)
    eal.save_asset(bank.get_path_name())
    unreal.log_warning(
        f"VO BANK OK: {bank_path} (chars={len(characters)} bosses={len(bosses)} auditor={len(auditor)})")


try:
    main()
except Exception:
    unreal.log_error("VO IMPORT FAILED")
    unreal.log_error(traceback.format_exc())
finally:
    unreal.SystemLibrary.quit_editor()
