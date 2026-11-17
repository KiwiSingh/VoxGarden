param([string]$BuildDir='build',[string]$OutputDir='dist')
$ErrorActionPreference='Stop'
$Repo=(Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$Root=Join-Path $OutputDir 'VoxGarden-3.0.1-Windows-x64-preview'
New-Item -ItemType Directory -Force "$Root/Plugins","$Root/Standalone","$Root/Source","$Root/Validation" | Out-Null
Copy-Item "$BuildDir/VoxGarden_artefacts/Release/VST3/VoxGarden.vst3" "$Root/Plugins" -Recurse
Copy-Item "$BuildDir/VoxGarden_artefacts/Release/Standalone/VoxGarden.exe" "$Root/Standalone"
Copy-Item "$Repo/Windows/Install.ps1","$Repo/Windows/Install.cmd","$Repo/Windows/README.md","$Repo/LICENSE" $Root
Copy-Item "$Repo/Licenses" $Root -Recurse
if (Test-Path validation) { Copy-Item validation/* "$Root/Validation" -Recurse }
git archive --format=zip --output="$Root/Source/VoxGarden-source.zip" HEAD
if ($LASTEXITCODE) { throw 'Source archive failed' }
git rev-parse HEAD | Set-Content "$Root/SOURCE_COMMIT.txt"
$Zip="$Root.zip"
Compress-Archive -Path $Root -DestinationPath $Zip -Force
$Hash=(Get-FileHash $Zip -Algorithm SHA256).Hash.ToLower()
"$Hash  $([IO.Path]::GetFileName($Zip))" | Set-Content (Join-Path $OutputDir 'SHA256SUMS.txt')
