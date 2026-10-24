#!/bin/sh
set -eu
cd "$(dirname "$0")"
vox_ai_root="$HOME/Library/Application Support/VoxGarden/AI"
mkdir -p "$vox_ai_root"
cp choir_worker.py prepare_ai.py download_voices.py "$vox_ai_root/"
if [ -d Voices ]; then
  mkdir -p "$vox_ai_root/voices"
  ditto --noextattr --norsrc Voices "$vox_ai_root/voices"
fi
if command -v uv >/dev/null 2>&1; then
  if [ ! -x "$vox_ai_root/python/bin/python" ]; then uv venv --python 3.10 "$vox_ai_root/python"; fi
  uv pip install --python "$vox_ai_root/python/bin/python" -r requirements-ai.txt
else
  if ! command -v python3.10 >/dev/null 2>&1; then
    echo "Install Python 3.10 or uv, then run this setup again. No global packages have been changed."
    exit 1
  fi
  if [ ! -x "$vox_ai_root/python/bin/python" ]; then python3.10 -m venv "$vox_ai_root/python"; fi
  "$vox_ai_root/python/bin/python" -m pip install -r requirements-ai.txt
fi
if [ ! -d "$vox_ai_root/seed-vc-local/.git" ]; then
  git clone https://github.com/Plachtaa/seed-vc.git "$vox_ai_root/seed-vc-local"
fi
git -C "$vox_ai_root/seed-vc-local" fetch --depth 1 origin 51383efd921027683c89e5348211d93ff12ac2a8
git -C "$vox_ai_root/seed-vc-local" checkout --detach 51383efd921027683c89e5348211d93ff12ac2a8
"$vox_ai_root/python/bin/python" "$vox_ai_root/prepare_ai.py" --root "$vox_ai_root"
