# Black Ledger - export the cleaned mesh straight to a vehicle's source FBX,
# bypassing the GUI export dialog. Run in Blender's Scripting tab (press Run).
# A dropdown pops up: pick the vehicle you have open, click OK, it exports.
# (Defaults to the vehicle named in your saved .blend, if any.) Works for any vehicle.

import bpy, os

ROOT = r"C:\Users\csmit\black-ledger\art\source\vehicles"
FOLDER = {
    "Surgeon": "01_Surgeon", "Antoinette": "02_Antoinette", "Hollow": "03_Hollow",
    "Warden": "04_Warden", "Pup": "05_Pup", "Crucible": "06_Crucible",
    "Cartographer": "07_Cartographer", "Lien": "08_Lien", "Hemlock": "09_Hemlock",
    "Shepherd": "10_Shepherd", "Specter": "11_Specter", "Refuse": "12_Refuse",
    "Bride": "13_Bride", "Hunter": "14_Hunter", "Photographer": "15_Photographer",
    "Vault": "16_Vault",
}
VEHICLES = sorted(FOLDER)
# default the dropdown to a vehicle whose name appears in the saved .blend filename
_blend = os.path.basename(bpy.data.filepath).lower()
_guess = next((v for v in VEHICLES if v.lower() in _blend), VEHICLES[0])


class BL_OT_export_vehicle(bpy.types.Operator):
    bl_idname = "bl.export_vehicle"
    bl_label = "Export cleaned mesh to vehicle source FBX"
    vehicle: bpy.props.EnumProperty(
        name="Vehicle", items=[(v, v, FOLDER[v]) for v in VEHICLES], default=_guess)

    def execute(self, context):
        dest = os.path.join(ROOT, FOLDER[self.vehicle], self.vehicle + ".fbx")
        meshes = [o for o in context.scene.objects if o.type == 'MESH']
        if not meshes:
            self.report({'ERROR'}, "No mesh in the scene to export.")
            return {'CANCELLED'}
        ob = max(meshes, key=lambda o: len(o.data.vertices))
        context.view_layer.objects.active = ob
        if context.object and context.object.mode != 'OBJECT':
            bpy.ops.object.mode_set(mode='OBJECT')
        bpy.ops.object.select_all(action='DESELECT')
        ob.select_set(True)
        bpy.ops.export_scene.fbx(filepath=dest, use_selection=True, object_types={'MESH'},
                                 mesh_smooth_type='FACE', add_leaf_bones=False)
        mb = os.path.getsize(dest) / 1e6
        msg = "EXPORTED -> %s   (%d verts, %.1f MB)" % (
            os.path.basename(dest), len(ob.data.vertices), mb)
        print("=" * 60); print(msg); print("=" * 60)
        self.report({'INFO'}, msg)
        context.window_manager.popup_menu(
            lambda s, c: s.layout.label(text=msg), title="FBX export", icon='INFO')
        return {'FINISHED'}

    def invoke(self, context, event):
        return context.window_manager.invoke_props_dialog(self, width=320)


try:
    bpy.utils.unregister_class(BL_OT_export_vehicle)
except Exception:
    pass
bpy.utils.register_class(BL_OT_export_vehicle)
bpy.ops.bl.export_vehicle('INVOKE_DEFAULT')
