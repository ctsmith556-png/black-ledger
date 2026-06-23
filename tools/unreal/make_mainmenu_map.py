# Black Ledger - build the front-end map /Game/BlackLedger/Maps/L_MainMenu and
# point its World Settings at ABLMenuGameMode (which raises the main menu).
#
# A deliberately bare, grim backdrop: a dark floor + heavy fog + one dim key
# light. The Slate menu (UBLUISubsystem) draws on top. The dossier-art title
# scene replaces this backdrop in the art pass.
#
# Run with the EDITOR CLOSED (NOT -nullrhi: scripted spawns crash under nullrhi
# as of UE 5.7.4):
#   & "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" `
#     "C:\Users\csmit\black-ledger\BlackLedger.uproject" `
#     -ExecCmds="py C:/Users/csmit/black-ledger/tools/unreal/make_mainmenu_map.py" `
#     -stdout -unattended -nosplash -nosound -nopause

import unreal, traceback

MAP_PATH = "/Game/BlackLedger/Maps/L_MainMenu"
SCRATCH  = "/Game/BlackLedger/Maps/L_Scratch"
MENU_GM  = "/Script/BlackLedger.BLMenuGameMode"

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
eal = unreal.EditorAssetLibrary


def spawn(cls, loc, rot=(0, 0, 0), label=None):
    a = actors.spawn_actor_from_class(cls, unreal.Vector(*loc), unreal.Rotator(*rot))
    if not a:
        raise RuntimeError(f"spawn failed: {cls.get_name()} at {loc}")
    if label:
        a.set_actor_label(label)
    return a


def spawn_cube(loc, scale, rot=(0, 0, 0), label=None):
    a = spawn(unreal.StaticMeshActor, loc, rot, label)
    smc = a.static_mesh_component
    smc.set_editor_property("static_mesh", eal.load_asset("/Engine/BasicShapes/Cube"))
    smc.set_editor_property("mobility", unreal.ComponentMobility.STATIC)
    a.set_actor_scale3d(unreal.Vector(*scale))
    return a


def main():
    # step onto a scratch level first so we can recreate L_MainMenu cleanly
    if eal.does_asset_exist(SCRATCH):
        eal.delete_asset(SCRATCH)
    if not les.new_level(SCRATCH):
        raise RuntimeError("scratch new_level failed")
    if eal.does_asset_exist(MAP_PATH):
        if not eal.delete_asset(MAP_PATH):
            raise RuntimeError("could not delete stale L_MainMenu")
        unreal.log_warning(f"deleted stale {MAP_PATH}")
    if not les.new_level(MAP_PATH):
        raise RuntimeError("new_level failed")

    # dark backdrop: a wide floor the camera looks across
    spawn_cube((0, 0, -50), (120, 120, 1), label="Floor")

    # grim lighting - dim key + faint sky, heavy fog so the void reads as dread
    sun = spawn(unreal.DirectionalLight, (0, 0, 1000), (0, -28, 40), "KeyLight")
    sun.light_component.set_editor_property("intensity", 2.0)
    sky = spawn(unreal.SkyLight, (0, 0, 1000), label="SkyLight")
    sky.light_component.set_editor_property("intensity", 0.5)
    fog = spawn(unreal.ExponentialHeightFog, (0, 0, 0), label="Fog")
    fog.get_component_by_class(unreal.ExponentialHeightFogComponent).set_editor_property(
        "fog_density", 0.18)

    # a PlayerController needs a place to spawn; the menu pawn just stands here
    spawn(unreal.PlayerStart, (0, 0, 250), (0, 0, 0), "PlayerStart")

    # point this level's World Settings at the menu game mode
    ws = None
    for a in actors.get_all_level_actors():
        if isinstance(a, unreal.WorldSettings):
            ws = a
            break
    if not ws:
        raise RuntimeError("no WorldSettings actor found")
    gm = unreal.load_class(None, MENU_GM)
    if not gm:
        raise RuntimeError(f"could not load {MENU_GM} - is the C++ built?")
    ws.set_editor_property("default_game_mode", gm)

    n = len(actors.get_all_level_actors())
    if not les.save_current_level():
        raise RuntimeError("save_current_level failed")
    if eal.does_asset_exist(SCRATCH):
        eal.delete_asset(SCRATCH)
    unreal.log_warning(f"MAINMENU MAP OK: {MAP_PATH} ({n} actors, GameMode=BLMenuGameMode)")


try:
    main()
except Exception:
    unreal.log_error("MAINMENU MAP FAILED")
    unreal.log_error(traceback.format_exc())
finally:
    unreal.SystemLibrary.quit_editor()
