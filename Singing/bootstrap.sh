#!/bin/sh
# Private, version-pinned runtime bootstrap invoked by VoxGarden's model panel.
set -eu
vox_root="$1"
mkdir -p "$vox_root/tools"
vox_stage() { printf '%s\n' "$1" > "$vox_root/Setup Stage.txt"; }
vox_stage 'Preparing the private Python runtime…'
vox_uv="$vox_root/tools/uv-aarch64-apple-darwin/uv"
if [ ! -x "$vox_uv" ]; then
    vox_stage 'Downloading the runtime manager (Apple Silicon)…'
    /usr/bin/curl --fail --location --retry 2 --connect-timeout 20 --max-time 300 --output "$vox_root/tools/uv.tar.gz.download" 'https://github.com/astral-sh/uv/releases/download/0.8.22/uv-aarch64-apple-darwin.tar.gz'
    vox_sum=$(/usr/bin/shasum -a 256 "$vox_root/tools/uv.tar.gz.download")
    case "$vox_sum" in '3f61099e261e449527141dbf125629fab33ad696468c8c90cebbac40185a306c '*) ;; *) echo 'Runtime download checksum failed.' >&2; exit 1;; esac
    /usr/bin/tar -xzf "$vox_root/tools/uv.tar.gz.download" -C "$vox_root/tools"
    /bin/rm "$vox_root/tools/uv.tar.gz.download"
fi
export UV_CACHE_DIR="$vox_root/package-cache"
export UV_PYTHON_INSTALL_DIR="$vox_root/tools/python"
export UV_NATIVE_TLS=true
if [ ! -x "$vox_root/python/bin/python" ]; then
    "$vox_uv" venv --python 3.11.13 "$vox_root/python"
fi
vox_stage 'Installing the neural runtime (no global Python changes)…'
"$vox_uv" pip install --python "$vox_root/python/bin/python" 'supertonic==1.3.1' 'onnxruntime==1.31.0' 'numpy==2.4.6' 'praat-parselmouth==0.4.7' 'soundfile==0.14.0' 'truststore==0.10.5'
vox_stage 'Downloading and verifying the voice models…'
exec "$vox_root/python/bin/python" "$vox_root/runtime.py" --setup --status "$vox_root/Setup Status.json"
