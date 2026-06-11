# Black Ledger - minimal spawn repro: new level + ONE cube + save.
# Isolates whether the editor environment crashes on any scripted spawn
# (vs something specific to make_mill_map.py).

import unreal, traceback

MAP_PATH = "/Game/BlackLedger/Maps/L_TestSpawn"

les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
eal = unreal.EditorAssetLibrary

try:
    if eal.does_asset_exist(MAP_PATH):
        eal.delete_asset(MAP_PATH)
    if not les.new_level(MAP_PATH):
        raise RuntimeError("new_level failed")
    unreal.log_warning("TEST: spawning bare StaticMeshActor")
    a = actors.spawn_actor_from_class(
        unreal.StaticMeshActor, unreal.Vector(0, 0, 0), unreal.Rotator(0, 0, 0))
    unreal.log_warning("TEST: bare spawn OK, assigning mesh")
    a.static_mesh_component.set_editor_property(
        "static_mesh", eal.load_asset("/Engine/BasicShapes/Cube"))
    unreal.log_warning("TEST: mesh assigned, setting mobility")
    a.static_mesh_component.set_editor_property(
        "mobility", unreal.ComponentMobility.STATIC)
    unreal.log_warning("TEST: mobility set, scaling")
    a.set_actor_scale3d(unreal.Vector(10, 10, 1))
    unreal.log_warning("TEST: scaled, saving")
    if not les.save_current_level():
        raise RuntimeError("save failed")
    unreal.log_warning("TEST SPAWN OK")
except Exception:
    unreal.log_error("TEST SPAWN FAILED")
    unreal.log_error(traceback.format_exc())
finally:
    unreal.SystemLibrary.quit_editor()
