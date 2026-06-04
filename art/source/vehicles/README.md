# art/source/vehicles — raw Meshy exports

One folder per vehicle (`NN_Name`, matching `docs/multiview/`). Drop each Meshy download here **unzipped**.

## Per-vehicle contents

```
01_Surgeon/
  Surgeon.fbx          (or .glb — FBX preferred for UE)
  texture_basecolor.png
  texture_normal.png
  texture_metallic.png
  texture_roughness.png   (or combined metallic-roughness)
```

Keep Meshy's texture filenames if you like — just keep everything for one vehicle in its folder.

## Export settings (Meshy)

- **Remesh first:** Fixed topology, **~60k tris** (hero vehicles). Bosses later: 80–120k.
- **Download:** FBX **with textures (PBR)**.

## After dropping files in

```bash
git add art/
git commit -m "Add Meshy vehicle source models (FBX + PBR)"
git push
```

All formats here (fbx/glb/png) are LFS-tracked via the root `.gitattributes`.

**LFS budget note:** GitHub free tier = 10 GB storage / 10 GB-month bandwidth. 16 textured FBX should fit; if tight, drop textures to 2K. Check usage: github.com → Settings → Billing.

## Next stop

Blender prep per `docs/MESHY_PLAYBOOK.md`: separate 4 wheels, fix pivot + scale, cap underside → then UE import to `Content/Vehicles/<Name>/`.
