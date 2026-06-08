# Black Ledger - batch Blender prep over the launch roster (vehicles 02-16).
# The Surgeon (01) is already done. Lengths are real-world-ish per vehicle type.
#
# Usage (editor may stay open; this is Blender only):
#   .\tools\blender\batch_prep.ps1                 # run all
#   .\tools\blender\batch_prep.ps1 -Only Hollow,Bride   # run a subset (e.g. flip re-runs)
#
# 'Flip' rotates 180 deg when Meshy modeled the vehicle facing -X (front=+X for UE).

param([string[]]$Only = @())

$repo = "C:\Users\csmit\black-ledger"
$blender = (Get-ChildItem "C:\Program Files\Blender Foundation" -Recurse -Filter blender.exe -ErrorAction SilentlyContinue | Select-Object -First 1).FullName
if (-not $blender) { Write-Error "blender.exe not found under C:\Program Files\Blender Foundation"; exit 1 }

# Name | Folder | Length(m) | Flip
# Flip=$true: Meshy modeled the whole roster facing -X (same as the Surgeon); UE wants front=+X.
$vehicles = @(
  @{ Name="Antoinette";   Folder="02_Antoinette";   Length=5.6;  Flip=$true },  # '57 Cadillac
  @{ Name="Hollow";       Folder="03_Hollow";        Length=9.0;  Flip=$true },  # pumper fire engine
  @{ Name="Warden";       Folder="04_Warden";        Length=6.5;  Flip=$true },  # prison van
  @{ Name="Pup";          Folder="05_Pup";           Length=5.8;  Flip=$true },  # ice-cream step van
  @{ Name="Crucible";     Folder="06_Crucible";      Length=8.5;  Flip=$true },  # 6-wheel slag hauler
  @{ Name="Cartographer"; Folder="07_Cartographer";  Length=5.6;  Flip=$true },  # Plymouth wagon
  @{ Name="Lien";         Folder="08_Lien";          Length=7.5;  Flip=$true },  # repo tow truck
  @{ Name="Hemlock";      Folder="09_Hemlock";       Length=8.5;  Flip=$true },  # tanker truck
  @{ Name="Shepherd";     Folder="10_Shepherd";      Length=11.0; Flip=$true },  # congregation bus
  @{ Name="Specter";      Folder="11_Specter";       Length=7.0;  Flip=$true },  # surveillance box truck
  @{ Name="Refuse";       Folder="12_Refuse";        Length=9.0;  Flip=$true },  # garbage truck
  @{ Name="Bride";        Folder="13_Bride";         Length=9.0;  Flip=$true },  # 3-axle stretch limo
  @{ Name="Hunter";       Folder="14_Hunter";        Length=5.8;  Flip=$true },  # square-body pickup
  @{ Name="Photographer"; Folder="15_Photographer";  Length=5.4;  Flip=$true },  # '60s press sedan
  @{ Name="Vault";        Folder="16_Vault";         Length=6.8;  Flip=$true }   # armored cash truck
)

$ran = 0
foreach ($v in $vehicles) {
  if ($Only.Count -gt 0 -and ($Only -notcontains $v.Name)) { continue }
  $src = Join-Path $repo ("art\source\vehicles\{0}\{1}.fbx" -f $v.Folder, $v.Name)
  if (-not (Test-Path $src)) { Write-Warning "MISSING $src - skipping"; continue }
  $args = @("--background","--python","$repo\tools\blender\prep_vehicle.py","--",
            "--src",$src,"--name",$v.Name,"--length",$v.Length)
  if ($v.Flip) { $args += "--flip" }
  Write-Host "`n=== PREP $($v.Name)  length=$($v.Length)m  flip=$($v.Flip) ===" -ForegroundColor Cyan
  & $blender @args
  if ($LASTEXITCODE -ne 0) { Write-Warning "$($v.Name) prep returned $LASTEXITCODE" }
  $ran++
}
Write-Host "`nBatch prep complete: $ran vehicle(s) processed." -ForegroundColor Green
