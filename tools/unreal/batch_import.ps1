# Black Ledger - batch headless UE import over the launch roster (vehicles 02-16).
# RUN WITH THE EDITOR CLOSED. Each vehicle boots a headless editor, imports, quits.
# The post-save teardown "Fatal error" under -nullrhi is known/harmless; the script
# checks the Content folder on disk afterward for the real result.
#
# Usage:
#   .\tools\unreal\batch_import.ps1                  # all
#   .\tools\unreal\batch_import.ps1 -Only Bride      # subset

param([string[]]$Only = @())

# Robust -Only: `powershell -File ... -Only A,B,C` can arrive as ONE comma-joined
# string (never matching a single name); split it back into individual names.
if ($Only.Count -eq 1) { $Only = $Only[0] -split ',' | ForEach-Object { $_.Trim() } }

$repo = "C:\Users\csmit\black-ledger"
$editor = "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
$uproject = "$repo\BlackLedger.uproject"
$script = "C:/Users/csmit/black-ledger/tools/unreal/import_vehicle.py"

$vehicles = @(
  @{ Name="Surgeon";      Folder="01_Surgeon" },
  @{ Name="Antoinette";   Folder="02_Antoinette" },
  @{ Name="Hollow";       Folder="03_Hollow" },
  @{ Name="Warden";       Folder="04_Warden" },
  @{ Name="Pup";          Folder="05_Pup" },
  @{ Name="Crucible";     Folder="06_Crucible" },
  @{ Name="Cartographer"; Folder="07_Cartographer" },
  @{ Name="Lien";         Folder="08_Lien" },
  @{ Name="Hemlock";      Folder="09_Hemlock" },
  @{ Name="Shepherd";     Folder="10_Shepherd" },
  @{ Name="Specter";      Folder="11_Specter" },
  @{ Name="Refuse";       Folder="12_Refuse" },
  @{ Name="Bride";        Folder="13_Bride" },
  @{ Name="Hunter";       Folder="14_Hunter" },
  @{ Name="Photographer"; Folder="15_Photographer" },
  @{ Name="Vault";        Folder="16_Vault" }
)

foreach ($v in $vehicles) {
  if ($Only.Count -gt 0 -and ($Only -notcontains $v.Name)) { continue }
  $fbx = Join-Path $repo ("art\source\vehicles\{0}\{1}_UE.fbx" -f $v.Folder, $v.Name)
  if (-not (Test-Path $fbx)) { Write-Warning "MISSING $fbx - run prep first; skipping"; continue }
  Write-Host "`n=== IMPORT $($v.Name) ===" -ForegroundColor Cyan
  $env:BL_VEHICLE_NAME = $v.Name
  $env:BL_VEHICLE_FOLDER = $v.Folder
  & $editor $uproject "-ExecCmds=py $script" -stdout -unattended -nosplash -nosound -nullrhi -nopause | Out-Null
  $dest = Join-Path $repo ("Content\BlackLedger\Characters\{0}\SM_{0}_Body.uasset" -f $v.Name)
  if (Test-Path $dest) { Write-Host "  OK  $($v.Name) body asset on disk" -ForegroundColor Green }
  else { Write-Warning "  $($v.Name): SM_$($v.Name)_Body.uasset NOT found - check log" }
}
Write-Host "`nBatch import complete." -ForegroundColor Green
