#!/usr/bin/env python3
"""
Black Ledger — Meshy batch driver.
Drives Meshy Text-to-3D (preview -> refine) over meshy_assets.json and downloads
FBX/GLB + PBR maps per asset. Stdlib only (no pip installs).

Setup: see MESHY_API_SETUP.md. Set your key:  export MESHY_API_KEY=msy-xxxxx
Verify request params against https://docs.meshy.ai/en/api/text-to-3d before a big run.

Examples:
  python meshy_batch.py                 # dry/safe: first 1 asset (test key if MESHY_API_KEY unset)
  python meshy_batch.py --filter weapon # nothing — names don't contain 'weapon'; use a real substring
  python meshy_batch.py --filter missile --all
  python meshy_batch.py --all           # everything (40 assets — watch your credits!)
"""
import os, sys, time, json, csv, argparse, urllib.request, urllib.error

API_KEY = os.environ.get("MESHY_API_KEY", "msy_dummy_api_key_for_test_mode_12345678")
BASE    = "https://api.meshy.ai/openapi"
HEAD    = {"Authorization": f"Bearer {API_KEY}", "Content-Type": "application/json"}

def _req(method, url, payload=None):
    data = json.dumps(payload).encode() if payload is not None else None
    req = urllib.request.Request(url, data=data, headers=HEAD, method=method)
    try:
        with urllib.request.urlopen(req, timeout=60) as r:
            return json.loads(r.read())
    except urllib.error.HTTPError as e:
        raise SystemExit(f"HTTP {e.code} on {url}: {e.read().decode()[:300]}")

def create_preview(prompt, polycount, topology, ai_model, formats):
    body = {"mode": "preview", "prompt": prompt[:600], "ai_model": ai_model,
            "topology": topology, "target_polycount": polycount,
            "should_remesh": True, "target_formats": formats}
    return _req("POST", f"{BASE}/v2/text-to-3d", body)["result"]

def create_refine(preview_id, ai_model, formats, hd):
    body = {"mode": "refine", "preview_task_id": preview_id, "ai_model": ai_model,
            "enable_pbr": True, "hd_texture": hd, "target_formats": formats}
    return _req("POST", f"{BASE}/v2/text-to-3d", body)["result"]

def get_task(tid):
    return _req("GET", f"{BASE}/v2/text-to-3d/{tid}")

def wait(tid, label, poll=8, timeout=1800):
    t0 = time.time()
    while True:
        t = get_task(tid); s = t.get("status"); p = t.get("progress", 0)
        sys.stdout.write(f"   [{label}] {s} {p}%        \r"); sys.stdout.flush()
        if s == "SUCCEEDED": print(); return t
        if s in ("FAILED", "CANCELED"): raise SystemExit(f"\n   {label} {s}: {t.get('task_error')}")
        if time.time() - t0 > timeout: raise SystemExit(f"\n   {label} timed out")
        time.sleep(poll)

def run_asset(a, outdir, topology, ai_model, formats, hd):
    name = a["name"]; d = os.path.join(outdir, name); os.makedirs(d, exist_ok=True)
    print(f"\n=== {name} (target {a.get('polycount',30000)} polys) ===")
    pid = create_preview(a["prompt"], a.get("polycount", 30000), topology, ai_model, formats)
    wait(pid, "preview")
    rid = create_refine(pid, ai_model, formats, hd)
    t = wait(rid, "refine")
    n = 0
    for fmt, url in (t.get("model_urls") or {}).items():
        if url and (fmt in formats or fmt == "mtl"):
            urllib.request.urlretrieve(url, os.path.join(d, f"{name}.{fmt}")); n += 1
    for mp, url in (t.get("texture_urls") or [{}])[0].items():
        if url:
            urllib.request.urlretrieve(url, os.path.join(d, f"{name}_{mp}.png")); n += 1
    print(f"   downloaded {n} files -> {d}/")
    return {"name": name, "preview_id": pid, "refine_id": rid,
            "status": t.get("status"), "credits": t.get("consumed_credits"), "files": n}

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--assets", default="meshy_assets.json")
    ap.add_argument("--out", default="meshy_out")
    ap.add_argument("--only", type=int, default=1, help="run first N assets (default 1)")
    ap.add_argument("--all", action="store_true", help="run every asset")
    ap.add_argument("--filter", default="", help="only assets whose name contains this substring")
    ap.add_argument("--topology", default="quad")
    ap.add_argument("--ai-model", dest="ai_model", default="meshy-6")
    ap.add_argument("--formats", default="fbx,glb")
    ap.add_argument("--no-hd", action="store_true", help="disable 4K texture (cheaper/faster)")
    a = ap.parse_args()

    assets = json.load(open(a.assets))
    if a.filter: assets = [x for x in assets if a.filter.lower() in x["name"].lower()]
    if not a.all: assets = assets[:a.only]
    formats = [f.strip() for f in a.formats.split(",")]
    os.makedirs(a.out, exist_ok=True)

    if API_KEY.startswith("msy_dummy"):
        print("WARNING: using Meshy TEST key — no real models, no credits consumed.")
        print("         Set MESHY_API_KEY to your msy-... Pro key for real generation.\n")
    print(f"Running {len(assets)} asset(s) -> {a.out}/  (model={a.ai_model}, topo={a.topology}, formats={formats})")

    rows = []
    for x in assets:
        try:
            rows.append(run_asset(x, a.out, a.topology, a.ai_model, formats, not a.no_hd))
        except SystemExit as e:
            print(e); rows.append({"name": x["name"], "status": "ERROR"})
        time.sleep(2)  # gentle on rate limits

    with open(os.path.join(a.out, "manifest.csv"), "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=["name","preview_id","refine_id","status","credits","files"])
        w.writeheader()
        for r in rows: w.writerow({k: r.get(k, "") for k in w.fieldnames})
    total = sum((r.get("credits") or 0) for r in rows if isinstance(r.get("credits"), int))
    print(f"\nDone. {len(rows)} asset(s). ~{total} credits. Manifest: {a.out}/manifest.csv")

if __name__ == "__main__":
    main()
