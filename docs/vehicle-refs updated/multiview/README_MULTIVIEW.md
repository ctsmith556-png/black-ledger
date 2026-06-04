# Black Ledger — Meshy Multi-View Kit

Two ways to feed Meshy 6's **Image to 3D → Multi-view** for all 26 vehicles.

## How Meshy multi-view works
- Upload **up to 3 images** of the **same** vehicle from different angles. Best trio: **front, side, rear**.
- **One clean object per image.** No text, no labels, no callouts, no multiple views in a single image.
- Keep the subject the **same size/framing** with **consistent lighting** across all three (mismatched zoom is the #1 cause of bad reconstructions).
- Multi-view is **Meshy 6 only**. Use Standard model, Image Enhancement on, License **Private**.

## What's in here

**1. `/multiview/<Vehicle>/` — quick-test crops (ready to upload now)**
Each folder has `1_front.png`, `2_side.png`, `3_top.png`, cut straight from your dossier sheets.
- ✅ Perfectly consistent (same render), upload immediately to test the pipeline.
- ⚠️ They carry faint callout dots/lines from the sheet, are modest resolution (panels of a 1448px sheet), and give **side/front/top** — no true rear.
- Good enough to learn multi-view and get a first model; expect minor baked marks.

**2. `../MULTIVIEW_PROMPTS.md` — the quality path**
3 prompts per vehicle (front / side / rear) for regenerating **pristine, text-free** views in the same image tool you used for the dossiers. Use your existing side render as an **img2img reference (~0.3–0.5 strength)** so the design stays identical across views. This gives the best 3D and a real rear view.

## Recommended workflow
1. Test now: upload a vehicle's 3 crops to Meshy multi-view, see how it reconstructs.
2. For keepers: generate the clean front/side/rear from the prompt pack and re-run — noticeably better geometry, no baked marks.
3. Remesh (Fixed, ~60k tris; bosses 80–120k), export FBX/GLB + PBR, into Unreal.

## Sources
- [How to use Multi-View — Meshy Help Center](https://help.meshy.ai/en/articles/12634481-how-to-use-multi-view)
- [Multi-Image to 3D API — Meshy Docs](https://docs.meshy.ai/en/api/multi-image-to-3d)
