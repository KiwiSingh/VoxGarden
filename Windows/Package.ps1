param([string]$BuildDir='build',[string]$OutputDir='dist')
$ErrorActionPreference='Stop'
$Repo=(Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$Root=Join-Path $OutputDir 'VoxGarden-3.0.0-Windows-x64-preview'
New-Item -ItemType Directory -Force "$Root/Plugins","$Root/Standalone","$Root/Source","$Root/Validation" | Out-Null
Copy-Item "$BuildDir/VoxGarden_artefacts/Release/VST3/VoxGarden.vst3" "$Root/Plugins" -Recurse
Copy-Item "$BuildDir/VoxGarden_artefacts/Release/Standalone/VoxGarden.exe" "$Root/Standalone"
Copy-Item "$Repo/Windows/Install.ps1","$Repo/Windows/Install.cmd","$Repo/Windows/README.md","$Repo/LICENSE" $Root
Copy-Item "$Repo/Licenses" $Root -Recurse
if (Test-Path validation) {
  Get-ChildItem validation -File | Where-Object { $_.Extension -in '.log','.json' } | Copy-Item -Destination "$Root/Validation"
  if (Test-Path validation/screenshots) { Copy-Item validation/screenshots "$Root/Validation" -Recurse }
  if (Test-Path validation/neural) { Copy-Item validation/neural "$Root/Validation" -Recurse }
}
git archive --format=zip --output="$Root/Source/VoxGarden-source.zip" HEAD
if ($LASTEXITCODE) { throw 'Source archive failed' }
Expand-Archive -Path "$Root/Source/VoxGarden-source.zip" -DestinationPath "$Root/Source/VoxGarden"
Remove-Item "$Root/Source/VoxGarden-source.zip"
$Readme=[IO.File]::ReadAllText("$Root/README.md").Replace('](../README.', '](Source/VoxGarden/README.')
[IO.File]::WriteAllText("$Root/README.md",$Readme,[Text.UTF8Encoding]::new($false))
git rev-parse HEAD | Set-Content "$Root/SOURCE_COMMIT.txt"
$Zip="$Root.zip"
Compress-Archive -Path $Root -DestinationPath $Zip -Force
$Hash=(Get-FileHash $Zip -Algorithm SHA256).Hash.ToLower()
"$Hash  $([IO.Path]::GetFileName($Zip))" | Set-Content (Join-Path $OutputDir 'SHA256SUMS.txt')
