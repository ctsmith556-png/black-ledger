# Black Ledger — Environment Setup & Day-One Checklist

Everything to install and verify the moment the development machine is online. Most can be pre-downloaded now.

## Toolchain

- [ ] **Epic Games Launcher** -> **Unreal Engine 5.x** (latest stable). Include *Editor symbols for debugging* if you'll debug C++.
- [ ] **Visual Studio 2022** (Community is fine) with workloads:
  - [ ] Game development with C++
  - [ ] Desktop development with C++
  - [ ] .NET desktop development (tooling)
  - [ ] Components: Unreal Engine installer, latest Windows SDK, C++ profiling tools
- [ ] **Git** + **Git LFS** (`git lfs install`)
- [ ] A Git client (GitHub Desktop, or CLI)
- [ ] *(Optional)* **Rider for Unreal Engine** — many prefer it to Visual Studio

## Accounts / licenses

- [x] GitHub repository created
- [ ] Epic Games account linked to GitHub (for engine source access if ever needed)
- [ ] Steamworks partner account — one-time Steam Direct fee applies (verify current amount); can wait until you have a build
- [ ] Choose and add a LICENSE before any public / source release

## First clone

    git lfs install
    git clone <repo-url>
    cd black-ledger

Create or open `BlackLedger.uproject` -> right-click -> **Generate Visual Studio project files** -> open the `.sln` -> build (Development Editor | Win64) -> launch.

## Phase 0 exit criteria (verify, then you're ready for real work)

- [ ] Editor launches and loads an empty level
- [ ] You can create a level, place actors, and save
- [ ] A trivial new C++ class (`ABLActorTest`) compiles
- [ ] A Blueprint can call a C++ function you exposed
- [ ] First commit pushes and a `.uasset` round-trips through Git LFS correctly

## Hardware sanity check

- [ ] SSD/NVMe with 100+ GB free (engine + project + DDC grow fast)
- [ ] 32 GB RAM recommended (UE5 + VS + browser)
- [ ] Discrete GPU with current drivers
