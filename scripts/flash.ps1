Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

Set-Location $PSScriptRoot

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot '..')
$uf2File  = Join-Path $repoRoot 'build\monster_book.uf2'

Write-Host "UF2 file expected at: $uf2File"
if (-not (Test-Path $uf2File)) {
    throw "UF2 not found. Build first: .\build.bat"
}

$picoDrive = (Get-CimInstance Win32_LogicalDisk |
    Where-Object { $_.VolumeName -in @('RPI-RP2','RP2350') } |
    Select-Object -First 1 -ExpandProperty DeviceID)

if (-not $picoDrive) {
    throw "Pico not found. Hold BOOTSEL and plug in USB; it should mount as RPI-RP2."
}

Write-Host "Copying to $picoDrive..."
$dest = Join-Path "$picoDrive\" (Split-Path $uf2File -Leaf)
Copy-Item -LiteralPath $uf2File -Destination $dest -Force

Write-Host "Done! Pico will reboot automatically."
