# Merges App0 (Bootloader) and App1 (PainDrain) Intel HEX files into a single
# hex that can be programmed in one pass, avoiding the chip-erase-between-programs issue.
#
# Usage: .\merge_hex.ps1
# Output: combined.hex in the firmware root — program this via PSoC Programmer.

$App0Hex = "$PSScriptRoot\Bootloader\Bootloader_BLE_App101.cydsn\CortexM0p\ARM_GCC_541\Debug\Bootloader_BLE_App101.hex"
$App1Hex = "$PSScriptRoot\PainDrain\PainDrain.cydsn\CortexM0p\ARM_GCC_541\Debug\PainDrain.hex"
$OutHex  = "$PSScriptRoot\combined.hex"

if (-not (Test-Path $App0Hex)) { Write-Error "App0 hex not found: $App0Hex"; exit 1 }
if (-not (Test-Path $App1Hex)) { Write-Error "App1 hex not found: $App1Hex"; exit 1 }

# Strip EOF record from App0, append full App1 (EOF comes from App1)
$app0Lines = Get-Content $App0Hex | Where-Object { $_ -notmatch "^:00000001FF" }
$app1Lines = Get-Content $App1Hex

($app0Lines + $app1Lines) | Set-Content -Encoding ASCII $OutHex

Write-Host "Combined hex written to: $OutHex"
Write-Host "Program this file via PSoC Programmer."
