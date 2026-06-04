# Meshy — Connecting It to Black Ledger

Meshy isn't a one-click Claude connector, so there's no "Connect" button to press here. Two ways to make the 3D pipeline seamless — pick one (or use both):

## Option A — The Unreal/Blender bridge (most seamless for this project)
Meshy ships official plugins that push generated models straight into your tools — including a **Bridge to Unreal** plugin. This is the smoothest path for a UE5 game: generate in Meshy, bridge into your project, done.
- Unreal plugin: https://docs.meshy.ai/en/unreal-plugin/introduction
- Blender plugin (for the cleanup/retopo pass): https://docs.meshy.ai/en/blender-plugin/introduction
Install the plugin, sign in with your Pro account, and you can send models from Meshy into Unreal/Blender directly. Use `MESHY_PROMPTS.md` and the reference sheets as your inputs.

## Option B — The batch API driver (automate all 40 assets)
`meshy_batch.py` drives Meshy's Text-to-3D API over every prompt in `meshy_assets.json` (14 weapons + 16 launch vehicles + 10 bosses), runs the preview→refine→texture workflow, and downloads the FBX/GLB + PBR maps per asset. Stdlib only — no installs beyond Python.

### 1. Get your API key
- Sign in to Meshy → **Settings → API** → create a key. It looks like `msy-xxxxxxxx`.
- API generation uses credits on your **Pro** plan (separate from web-app usage — check your balance).

### 2. Set the key (don't hard-code it)
```
# macOS/Linux
export MESHY_API_KEY=msy-your-real-key
# Windows PowerShell
setx MESHY_API_KEY "msy-your-real-key"
```
Tip: run once *without* a key first — it uses Meshy's free test key (`msy_dummy...`) so you can confirm the plumbing without spending credits.

### 3. Run it (start tiny, then scale)
```
python meshy_batch.py                    # first 1 asset only (safe default)
python meshy_batch.py --filter health    # just the Health Pack — your 5-min smoke test
python meshy_batch.py --filter missile --all
python meshy_batch.py --all              # everything (40 assets — watch credits!)
```
Options: `--topology quad|triangle` · `--formats fbx,glb` · `--no-hd` (skip 4K texture) · `--out <dir>` · `--ai-model meshy-6`.

Output: `meshy_out/<asset>/` with the model files + texture maps, plus `meshy_out/manifest.csv` (task IDs, status, credits, file counts).

### 4. Then finish in-engine
Take the FBX into Blender for the cleanup pass (separate wheels, scale, retopo if needed), then into Unreal and wire it up — all per `MESHY_PLAYBOOK.md` and `TECHNICAL_DESIGN.md`.

## Notes & caveats
- The script defaults to **Text-to-3D** (clean, fully automated). For **Image-to-3D**, the reference sheets in `vehicle-refs/` are best used in the Meshy web app (crop to the side view), since the API's image-to-3d wants a single clean object image or a hosted/data-URI image.
- Meshy's parameters, credit costs, and model versions change — verify against https://docs.meshy.ai/en/api/text-to-3d before a large run.
- Credits are refunded on `FAILED` tasks. Start with one asset, confirm quality, then batch.
