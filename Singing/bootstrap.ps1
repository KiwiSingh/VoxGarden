param([Parameter(Mandatory=$true)][string]$Root)
$ErrorActionPreference = 'Stop'
function Stage([string]$Message) { [IO.File]::WriteAllText((Join-Path $Root 'Setup Stage.txt'), $Message, [Text.UTF8Encoding]::new($false)) }
New-Item -ItemType Directory -Force (Join-Path $Root 'tools') | Out-Null
$Uv = Join-Path $Root 'tools/uv.exe'
if (!(Test-Path $Uv)) {
    Stage 'Downloading the private Windows runtime manager...'
    $Archive = Join-Path $Root 'tools/uv.zip.download'
    Invoke-WebRequest -UseBasicParsing -Uri 'https://github.com/astral-sh/uv/releases/download/0.8.22/uv-x86_64-pc-windows-msvc.zip' -OutFile $Archive -TimeoutSec 300
    if ((Get-FileHash $Archive -Algorithm SHA256).Hash.ToLower() -ne '5049375aa2a5162f132b2c1cb992e25d42d47d934cab8c174dbe6f60973dcc12') { throw 'Runtime download checksum failed.' }
    Add-Type -AssemblyName System.IO.Compression.FileSystem
    [IO.Compression.ZipFile]::ExtractToDirectory($Archive, (Join-Path $Root 'tools'))
    Remove-Item $Archive
}
$env:UV_CACHE_DIR = Join-Path $Root 'package-cache'
$env:UV_PYTHON_INSTALL_DIR = Join-Path $Root 'tools/python'
$env:UV_NATIVE_TLS = 'true'
$env:PYTHONUTF8 = '1'
$Python = Join-Path $Root 'python/Scripts/python.exe'
Stage 'Preparing private Python (no global Python changes)...'
if (!(Test-Path $Python)) { & $Uv venv --python 3.11.13 (Join-Path $Root 'python'); if ($LASTEXITCODE) { throw 'Private Python setup failed.' } }
Stage 'Installing DirectML / CPU singing runtime...'
& $Uv pip install --python $Python 'onnxruntime-directml==1.24.4' 'numpy==2.4.6' 'praat-parselmouth==0.4.7' 'soundfile==0.14.0' 'truststore==0.10.5' 'huggingface-hub==0.36.0'
if ($LASTEXITCODE) { throw 'Runtime dependency installation failed.' }
# DirectML replaces the onnxruntime distribution; do not install both over one namespace.
& $Uv pip install --python $Python --no-deps 'supertonic==1.3.1'
if ($LASTEXITCODE) { throw 'Voice engine installation failed.' }
Stage 'Downloading and checking the voice models...'
& $Python -X utf8 (Join-Path $Root 'runtime.py') --setup --status (Join-Path $Root 'Setup Status.json')
exit $LASTEXITCODE
