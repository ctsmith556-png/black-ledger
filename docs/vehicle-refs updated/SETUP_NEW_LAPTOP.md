# Black Ledger — New Laptop Setup & Continuation Guide

Versions current as of **June 2026**: Unreal Engine **5.7** (stable), Visual Studio **2022** (the safe toolchain), GitHub free **Git LFS = 10 GB** storage + 10 GB/mo bandwidth.

Work top to bottom. Phases 0–1 are the ones not to skip.

---

## Phase 0 — Protect your work FIRST (on the computer that has your files)

Your reference sheets, multi-view kit, and updated docs are **uncommitted** and live only on the machine you've been working on with me. Do this there **before** anything else, so the laptop can simply pull a complete repo. (If that machine *is* the new laptop, do this right after Phase 5 clone instead.)

Open a terminal (Git Bash or PowerShell) at your repo root:

```bash
cd C:/Users/User/black-ledger        # your repo root (where .git lives)
git status                           # review what's new/changed
```

Make sure Git LFS is handling the big binaries (so you don't blow past GitHub's 100 MB/file limit):

```bash
git lfs install
git lfs track "*.png" "*.jpg" "*.pdf" "*.zip" "*.docx" "*.xlsx" "*.pptx" "*.fbx" "*.glb" "*.gltf" "*.tga" "*.psd" "*.wav" "*.mp4" "*.uasset" "*.umap"
git add .gitattributes
```

Commit and push:

```bash
git add docs/
git commit -m "Add vehicle + weapon reference sheets, multi-view kit, updated design docs"
git push
```

Notes:
- **Skip committing the convenience .zip bundles** if you also commit the loose files they contain — it doubles your LFS storage. Commit the source files; keep zips for sharing.
- If you previously committed large files *without* LFS (the earlier issue), they're still raw in history. Optional cleanup later: `git lfs migrate import --include="*.png,*.zip,*.pdf,*.docx,*.xlsx" --everything` then force-push. Do this only when you understand it rewrites history.
- Confirm it worked: open the repo on github.com and check the new files show a small **"Stored with Git LFS"** note.

---

## Phase 1 — Laptop first boot

1. **Windows OOBE:** language, Wi-Fi, sign in with a Microsoft account.
2. **Windows Update — fully.** Run it, reboot, run again until "You're up to date." Don't skip; UE/VS assume a current OS.
3. **GPU drivers (critical for UE5):**
   - NVIDIA: install the **NVIDIA App** → get the latest **Studio (or Game Ready)** driver.
   - AMD: install **AMD Adrenalin** → latest driver.
4. **Performance setup:** keep it **plugged in**; Settings → System → Power → **Best performance**. In the NVIDIA/AMD panel set the laptop to use the **dedicated GPU** (not integrated) for UE and Blender.
5. **Disk:** make sure you have **~250 GB free** on the fast SSD (UE ~50 GB, VS ~30 GB, project + assets grow fast).
6. **Tip:** add your dev folder (e.g., `C:\Dev`) to **Windows Defender exclusions** (Settings → Privacy & security → Virus & threat protection → Exclusions). Real-time scanning slows UE compiles noticeably.

---

## Phase 2 — Developer tools

Install in this order:

1. **Git for Windows** — https://git-scm.com → includes Git Bash + **Git Credential Manager** (handles GitHub login via browser popup).
2. **Git LFS** — after Git, run once in a terminal: `git lfs install`.
3. **GitHub sign-in** — easiest is GitHub CLI: install from https://cli.github.com, then `gh auth login` → GitHub.com → HTTPS → login with browser. (Or just let the credential-manager popup handle it on first clone.)
4. **Visual Studio 2022 Community** (free) — https://visualstudio.microsoft.com. In the installer, check the **"Game development with C++"** workload. Confirm these are included: **MSVC v143** build tools, **Windows SDK (latest)**, and **"Visual Studio Tools for Unreal Engine."** This is the #1 thing people miss — UE won't compile C++ without it.
   - VS **2026** also works, but UE 5.7 targets the **v143** toolset bundled with VS 2022 — stick with 2022 to avoid toolchain surprises.
5. **VS Code** (optional, nice for editing docs/scripts) — https://code.visualstudio.com.
6. **Claude desktop app** — install it here too so we can keep using this Cowork workflow on the laptop, pointed at your local repo.
7. **One-time Git config** (UE has very long paths):
   ```bash
   git config --global core.longpaths true
   git config --global user.name "Your Name"
   git config --global user.email "ctsmith556@gmail.com"
   ```
   Also enable Windows long paths: run in an **admin** PowerShell:
   ```powershell
   Set-ItemProperty "HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem" -Name LongPathsEnabled -Value 1
   ```

---

## Phase 3 — Unreal Engine 5.7

1. Install the **Epic Games Launcher** — https://www.unrealengine.com/download.
2. Sign in (free Epic account). Connect your GitHub to Epic later if you want UE source access; not required for the launcher build.
3. Launcher → **Unreal Engine → Library → +** → install **5.7** (latest stable). ~50+ GB.
4. Launch it once to confirm it opens and detects your GPU.

---

## Phase 4 — 3D content tools

1. **Blender** (free) — https://www.blender.org → latest stable/LTS. This is your cleanup/retopo/wheel-separation tool (see our earlier discussion).
2. **Meshy export** (you'll use this in Phase 6): in Meshy, each model → **Download** → **FBX** (and/or **GLB**) **with textures (PBR)**. Each downloads as a folder/zip with the mesh + base color, normal, metallic-roughness maps.

---

## Phase 5 — Clone the project to the laptop

```bash
mkdir C:/Dev && cd C:/Dev
git clone https://github.com/<your-username>/<your-repo>.git black-ledger
cd black-ledger
git lfs pull                         # pulls the actual big files
```

First clone will pop a browser GitHub login (credential manager). After this, the laptop has every doc, sheet, and the multi-view kit.

---

## Phase 6 — Bring in your 16 Meshy models

1. In Meshy, for each of your 16 models: **Remesh** if you haven't (Fixed, ~60k tris; bosses 80–120k) → **Download FBX + textures**.
2. In the repo, create a home for raw source art:
   ```
   black-ledger/
     art/
       source/
         vehicles/
           01_Surgeon/   (Surgeon.fbx + texture maps)
           02_Antoinette/
           ...
   ```
3. Commit via LFS (already tracked from Phase 0's `.gitattributes`):
   ```bash
   git add art/
   git commit -m "Add 16 Meshy vehicle source models (FBX + PBR)"
   git push
   ```
4. **LFS budget:** free tier is **10 GB**. 16 textured FBX should fit, but 4K maps add up. If you approach the cap, either drop textures to 2K, keep raw source in cloud storage (Drive/Dropbox) and commit only the imported UE assets, or add a GitHub **data pack** ($5/mo per 50 GB). Check usage at github.com → Settings → Billing.

---

## Phase 7 — Create the Unreal project + first import

1. Epic Launcher → UE **5.7** → **Launch** → **Games** → **Blank** (or **Vehicle** template) → **C++** → name it **BlackLedger** → set the location **inside your repo** (e.g., `C:/Dev/black-ledger/`). Follow `docs/PROJECT_STRUCTURE.md` if it specifies a path.
2. Add the UE **.gitignore** and **.gitattributes** at the project root (see Appendix) so you don't commit gigabytes of build cache.
3. Let it generate + compile. If VS opens, build once (`Development Editor`). It should launch the editor.
4. **Import your first vehicle:** Content Browser → Import → `art/source/vehicles/01_Surgeon/Surgeon.fbx` → into `Content/Vehicles/Surgeon/`. Create a Material, plug in base color / normal / metallic-roughness.
5. Drop it in a test level, hit Play, orbit it. That's your round-trip working.
6. Commit the project:
   ```bash
   git add .gitignore .gitattributes BlackLedger.uproject Config/ Source/ Content/
   git commit -m "Initialize BlackLedger UE5.7 C++ project + first vehicle import"
   git push
   ```

---

## Phase 8 — Verify, then build

Checklist:
- [ ] Windows + GPU drivers current
- [ ] VS 2022 with C++ game-dev workload (UE compiles)
- [ ] UE 5.7 opens
- [ ] Repo cloned, `git lfs pull` done, `git push` works
- [ ] 16 Meshy models in `art/source/`, committed
- [ ] BlackLedger project created, compiles, opens
- [ ] First vehicle imported and visible in a level

Then continue with the docs you already have:
- `docs/DAY_ONE_CHECKLIST.md` — first build tasks
- `docs/TECHNICAL_DESIGN.md` — architecture (ABLCombatVehicle, components, data assets)
- `docs/PREPRODUCTION.md` + the vertical-slice packet — what the slice must contain
- Blender prep (separate wheels, fix pivot/scale) per our earlier walkthrough before rigging the drivable pawn

---

## Appendix — UE5 .gitignore & .gitattributes

**.gitignore** (project root):
```
Binaries/
DerivedDataCache/
Intermediate/
Saved/
.vs/
*.VC.db
*.sln
!*.uproject
```

**.gitattributes** (LFS — extends what you set in Phase 0):
```
*.uasset filter=lfs diff=lfs merge=lfs -text
*.umap   filter=lfs diff=lfs merge=lfs -text
*.fbx    filter=lfs diff=lfs merge=lfs -text
*.glb    filter=lfs diff=lfs merge=lfs -text
*.png    filter=lfs diff=lfs merge=lfs -text
*.tga    filter=lfs diff=lfs merge=lfs -text
*.wav    filter=lfs diff=lfs merge=lfs -text
*.pdf    filter=lfs diff=lfs merge=lfs -text
*.zip    filter=lfs diff=lfs merge=lfs -text
*.docx   filter=lfs diff=lfs merge=lfs -text
*.xlsx   filter=lfs diff=lfs merge=lfs -text
```

## Sources
- Unreal Engine current version / UE6 status — [unrealengine.com](https://www.unrealengine.com/en-US/unreal-engine-5), [Unreal Engine 6 status](https://unrealengine6.org/)
- Visual Studio for UE C++ — [Epic: Setting Up Visual Studio for UE](https://dev.epicgames.com/documentation/en-us/unreal-engine/setting-up-visual-studio-development-environment-for-cplusplus-projects-in-unreal-engine), [Microsoft Learn: VS Tools for Unreal](https://learn.microsoft.com/en-us/visualstudio/gamedev/unreal/get-started/vs-tools-unreal-install)
- Git LFS billing/quota — [GitHub Docs: About billing for Git LFS](https://docs.github.com/billing/managing-billing-for-git-large-file-storage/about-billing-for-git-large-file-storage)
