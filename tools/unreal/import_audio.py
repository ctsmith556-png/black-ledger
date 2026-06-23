# Black Ledger - import the placeholder WAVs (tools/audio_raw/, made by
# tools/gen_placeholder_audio.py) as USoundWaves into /Game/BlackLedger/Audio/,
# flag the loops, then build DA_BLAudioBank (UBLAudioBank) with every slot wired.
# The UBLAudioSubsystem loads that bank at runtime.
#
# Run with the EDITOR CLOSED (asset import is fine under -nullrhi):
#   & "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" `
#     "C:\Users\csmit\black-ledger\BlackLedger.uproject" `
#     -ExecCmds="py C:/Users/csmit/black-ledger/tools/unreal/import_audio.py" `
#     -stdout -unattended -nosplash -nosound -nullrhi -nopause

import unreal, os, traceback

RAW_DIR = "C:/Users/csmit/black-ledger/tools/audio_raw"
DEST = "/Game/BlackLedger/Audio"

at = unreal.AssetToolsHelpers.get_asset_tools()
eal = unreal.EditorAssetLibrary

# wav basename -> (bank property [exact UPROPERTY name], is_looping)
TABLE = [
    ("mg_fire",        "MGFire",          False),
    ("mg_hit",         "MGHit",           False),
    ("missile_launch", "MissileLaunch",   False),
    ("missile_impact", "MissileImpact",   False),
    ("power_impact",   "PowerImpact",     False),
    ("explosion",      "Explosion",       False),
    ("vehicle_death",  "VehicleDeath",    False),
    ("special",        "SpecialActivate", False),
    ("pickup",         "Pickup",          False),
    ("ui_select",      "UISelect",        False),
    ("engine_loop",    "EngineLoop",      True),
    ("engine_rev",     "EngineRev",       True),
    ("heartbeat",      "Heartbeat",       True),
    ("music_menu",     "MusicMenu",       True),
    ("music_combat",   "MusicCombat",     True),
    ("music_boss1",    "MusicBossP1",     True),
    ("music_boss2",    "MusicBossP2",     True),
    ("music_boss3",    "MusicBossP3",     True),
    ("music_victory",  "MusicVictory",    True),
    ("music_death",    "MusicDeath",      True),
]


def import_wav(basename, looping):
    asset_name = "SW_" + basename
    asset_path = f"{DEST}/{asset_name}"
    src = f"{RAW_DIR}/{basename}.wav"
    if not os.path.exists(src):
        unreal.log_error(f"missing wav: {src}")
        return None
    task = unreal.AssetImportTask()
    task.filename = src
    task.destination_path = DEST
    task.destination_name = asset_name
    task.automated = True
    task.replace_existing = True
    task.save = True
    at.import_asset_tasks([task])
    sound = unreal.load_asset(asset_path)
    if sound and looping:
        try:
            sound.set_editor_property("looping", True)
            eal.save_asset(asset_path)
        except Exception as e:
            unreal.log_warning(f"could not set looping on {asset_name}: {e}")
    return sound


def main():
    if not eal.does_directory_exist(DEST):
        eal.make_directory(DEST)

    sounds = {}
    for basename, prop, looping in TABLE:
        sounds[prop] = import_wav(basename, looping)

    # (re)build the bank data asset
    bank_path = f"{DEST}/DA_BLAudioBank"
    if eal.does_asset_exist(bank_path):
        eal.delete_asset(bank_path)
    bank = at.create_asset("DA_BLAudioBank", DEST, unreal.BLAudioBank, None)
    if not bank:
        raise RuntimeError("could not create DA_BLAudioBank (is the C++ built?)")

    assigned = 0
    for _, prop, _ in TABLE:
        snd = sounds.get(prop)
        if not snd:
            continue
        try:
            bank.set_editor_property(prop, snd)
            assigned += 1
        except Exception as e:
            unreal.log_warning(f"could not assign {prop}: {e}")
    eal.save_asset(bank.get_path_name())
    unreal.log_warning(f"AUDIO BANK OK: {bank_path} ({assigned}/{len(TABLE)} slots)")


try:
    main()
except Exception:
    unreal.log_error("AUDIO IMPORT FAILED")
    unreal.log_error(traceback.format_exc())
finally:
    unreal.SystemLibrary.quit_editor()
