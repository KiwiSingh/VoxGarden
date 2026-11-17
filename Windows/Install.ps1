$ErrorActionPreference = 'Stop'
Set-Location $PSScriptRoot
if (Get-Process -Name 'Ableton Live*','VoxGarden' -ErrorAction SilentlyContinue) { throw 'Close Ableton Live and VoxGarden before installing.' }
$Source = Join-Path $PSScriptRoot 'Plugins/VoxGarden.vst3'
$Parent = Join-Path $env:CommonProgramFiles 'VST3'
$Target = Join-Path $Parent 'VoxGarden.vst3'
New-Item -ItemType Directory -Force $Parent | Out-Null
if (Test-Path $Target) {
    $Backup = Join-Path $env:LOCALAPPDATA ('VoxGarden/Backups/' + [DateTime]::Now.ToString('yyyyMMdd-HHmmss-fff'))
    New-Item -ItemType Directory -Force $Backup | Out-Null
    Copy-Item $Target $Backup -Recurse
}
$Stage = Join-Path $Parent ('.VoxGarden-' + [Guid]::NewGuid().ToString())
Copy-Item $Source $Stage -Recurse
try {
    if (Test-Path $Target) { Remove-Item $Target -Recurse -Force }
    Move-Item $Stage $Target
} finally { if (Test-Path $Stage) { Remove-Item $Stage -Recurse -Force } }
Write-Host 'VoxGarden installed. Reopen/rescan your host. Previous plugin is backed up under LOCALAPPDATA/VoxGarden/Backups.'
