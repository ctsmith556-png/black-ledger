# Black Ledger - batch Blender prep over the launch roster (vehicles 01-16).
# Lengths are real-world-ish per vehicle type.
#
# Usage (editor may stay open; this is Blender only):
#   .\tools\blender\batch_prep.ps1                 # run all
#   .\tools\blender\batch_prep.ps1 -Only Hollow,Bride   # run a subset (e.g. flip re-runs)
#
# 'Flip' rotates 180 deg when Meshy modeled the vehicle facing -X (front=+X for UE).

param([string[]]$Only = @())

# Robust -Only: `powershell -File ... -Only A,B,C` can arrive as ONE comma-joined
# string (never matching a single name); split it back into individual names.
if ($Only.Count -eq 1) { $Only = $Only[0] -split ',' | ForEach-Object { $_.Trim() } }

$repo = "C:\Users\csmit\black-ledger"
$blender = (Get-ChildItem "C:\Program Files\Blender Foundation" -Recurse -Filter blender.exe -ErrorAction SilentlyContinue | Select-Object -First 1).FullName
if (-not $blender) { Write-Error "blender.exe not found under C:\Program Files\Blender Foundation"; exit 1 }

# Name | Folder | Length(m) | Flip
# Flip=$true: Meshy modeled the whole roster facing -X (same as the Surgeon); UE wants front=+X.
$vehicles = @(
  @{ Name="Surgeon";      Folder="01_Surgeon";       Length=6.5;  Flip=$true; Extra=@("--axle-fracs","0.800,0.253") },  # armored ambulance: measured in Blender (cab=low-X)
  @{ Name="Antoinette";   Folder="02_Antoinette";   Length=5.6;  Flip=$true; Extra=@("--axle-fracs","0.841,0.258","--wheel-radius","0.322","--wheel-y","0.80") },  # '57 Cadillac: measured (cab=low-X); track pinned 0.80 (fins fooled the auto-fit)
  @{ Name="Hollow";       Folder="03_Hollow";        Length=9.0;  Flip=$true; Extra=@("--axle-fracs","0.819,0.247") },  # pumper fire engine: measured (cab=low-X)
  @{ Name="Warden";       Folder="04_Warden";        Length=6.5;  Flip=$true; Extra=@("--axle-fracs","0.834,0.253") },  # prison van: measured (cab=low-X)
  @{ Name="Pup";          Folder="05_Pup";           Length=5.8;  Flip=$true; Extra=@("--axle-fracs","0.784,0.221") },  # ice-cream step van: measured (cab=low-X)
  @{ Name="Crucible";     Folder="06_Crucible";      Length=8.5;  Flip=$true; Extra=@("--axle-fracs","0.699,0.433,0.196") },  # 6x6 hauler: measured in Blender (cab=low-X)
  @{ Name="Cartographer"; Folder="07_Cartographer";  Length=5.6;  Flip=$true; Extra=@("--axle-fracs","0.791,0.256","--wheel-radius","0.322") },  # Plymouth wagon: measured (cab=low-X)
  @{ Name="Lien";         Folder="08_Lien";          Length=7.5;  Flip=$true; Extra=@("--axle-fracs","0.818,0.229,0.084") },  # tow truck: measured in Blender (cab=low-X, rear tandem)
  @{ Name="Hemlock";      Folder="09_Hemlock";       Length=8.5;  Flip=$true; Extra=@("--axle-fracs","0.889,0.252") },  # tanker: measured in Blender (cab=low-X); source smoothed
  @{ Name="Shepherd";     Folder="10_Shepherd";      Length=11.0; Flip=$true; Extra=@("--width","2.55","--height","3.2","--wheel-radius","0.52","--wheel-width","0.45","--axle-fracs","0.919,0.279") },  # bus: measured in Blender (cab=low-X)
  @{ Name="Specter";      Folder="11_Specter";       Length=7.0;  Flip=$true; Extra=@("--axle-fracs","0.795,0.187","--wheel-radius","0.45") },  # surveillance box truck: measured (cab=low-X)
  @{ Name="Refuse";       Folder="12_Refuse";        Length=9.0;  Flip=$true; Extra=@("--axle-fracs","0.829,0.308") },  # garbage truck: measured in Blender (cab=low-X)
  @{ Name="Bride";        Folder="13_Bride";         Length=9.0;  Flip=$true; Extra=@("--axle-fracs","0.920,0.300,0.196","--wheel-radius","0.40") },  # stretch limo: DONE in-engine. Source reverted to Meshy original after a mix-up; re-clean (blob/spikes) + lengthen hood + re-measure fracs before any re-prep.
  @{ Name="Hunter";       Folder="14_Hunter";        Length=5.8;  Flip=$true; Extra=@("--axle-fracs","0.791,0.237") },  # square-body pickup: measured (cab=low-X)
  @{ Name="Photographer"; Folder="15_Photographer";  Length=5.4;  Flip=$true; Extra=@("--axle-fracs","0.815,0.281","--wheel-radius","0.311","--wheel-y","0.70") },  # '60s press sedan: measured (cab=low-X); track pinned 0.70 (narrow nose splayed the wheels)
  @{ Name="Vault";        Folder="16_Vault";         Length=6.8;  Flip=$true; Extra=@("--axle-fracs","0.631,0.202","--wheel-y","1.30") }   # armored cash truck: measured (cab=low-X, long ram-hood overhang); track pinned 1.30
)

$ran = 0
foreach ($v in $vehicles) {
  if ($Only.Count -gt 0 -and ($Only -notcontains $v.Name)) { continue }
  $src = Join-Path $repo ("art\source\vehicles\{0}\{1}.fbx" -f $v.Folder, $v.Name)
  if (-not (Test-Path $src)) { Write-Warning "MISSING $src - skipping"; continue }
  $args = @("--background","--python","$repo\tools\blender\prep_vehicle.py","--",
            "--src",$src,"--name",$v.Name,"--length",$v.Length)
  if ($v.Flip) { $args += "--flip" }
  if ($v.Extra) { $args += $v.Extra }   # per-vehicle extra flags (non-uniform scale, axle pins, etc.)
  Write-Host "`n=== PREP $($v.Name)  length=$($v.Length)m  flip=$($v.Flip) ===" -ForegroundColor Cyan
  & $blender @args
  if ($LASTEXITCODE -ne 0) { Write-Warning "$($v.Name) prep returned $LASTEXITCODE" }
  $ran++
}
Write-Host "`nBatch prep complete: $ran vehicle(s) processed." -ForegroundColor Green
