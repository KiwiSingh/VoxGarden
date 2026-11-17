$ErrorActionPreference = 'Stop'
Set-Location $PSScriptRoot
if (Get-Process -Name 'Ableton Live*','VoxGarden' -ErrorAction SilentlyContinue) { throw 'Close Ableton Live and VoxGarden before installing.' }
$Identity = [Security.Principal.WindowsIdentity]::GetCurrent()
$Principal = [Security.Principal.WindowsPrincipal]::new($Identity)
if (!$Principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    $Elevated = Start-Process powershell.exe -Verb RunAs -Wait -PassThru -ArgumentList @('-NoProfile','-ExecutionPolicy','Bypass','-File',('"' + $PSCommandPath + '"'))
    exit $Elevated.ExitCode
}
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
$Previous = Join-Path $Parent ('.VoxGarden-previous-' + [Guid]::NewGuid().ToString())
try {
    if (Test-Path $Target) { Move-Item $Target $Previous }
    try { Move-Item $Stage $Target } catch {
        if ((Test-Path $Previous) -and !(Test-Path $Target)) { Move-Item $Previous $Target }
        throw
    }
    if (Test-Path $Previous) { Remove-Item $Previous -Recurse -Force }
} finally { if (Test-Path $Stage) { Remove-Item $Stage -Recurse -Force } }
Write-Host 'VoxGarden installed. Reopen/rescan your host. Previous plugin is backed up under LOCALAPPDATA/VoxGarden/Backups.'
