"""Fetch a tiny attributed reference pack using range reads, not the 11GB corpus."""
from pathlib import Path
import argparse, io, json, hashlib
import numpy as np
import soundfile as sf
from remotezip import RemoteZip

CORPUS_URL = "https://zenodo.org/records/10691876/files/VCTK-Corpus-0.92.zip?download=1"
CORPUS_GENDERS = {'p225': 'F', 'p226': 'M', 'p227': 'M', 'p228': 'F', 'p229': 'F', 'p230': 'F', 'p231': 'F', 'p232': 'M'}  # Original speaker-info.txt labels
SPEAKERS = ["p225", "p228", "p230", "p231", "p229", "p232", "p226", "p227"]

def prepare(destination):
    destination = Path(destination)
    destination.mkdir(parents=True, exist_ok=True)
    existing = destination / "manifest.json"
    if existing.exists():
        d = json.loads(existing.read_text())
        if all((destination / x["file"]).is_file() for x in d["voices"]):
            return d
    voices = []
    with RemoteZip(CORPUS_URL, initial_buffer_size=131072, timeout=40) as archive:
        names = archive.namelist()
        print(f"Indexed {len(names)} corpus entries using byte-range requests", flush=True)
        for speaker in SPEAKERS:
            out = destination / f"{speaker}.wav"
            segments, provenance, samples = [], [], 0
            candidates = sorted(n for n in names if f"/{speaker}/" in n and n.endswith("_mic2.flac"))
            if not candidates:
                raise RuntimeError(f"No audio found for {speaker}")
            for name in candidates[:8]:
                wave, sr = sf.read(io.BytesIO(archive.read(name)), dtype="float32")
                if wave.ndim > 1:
                    wave = wave.mean(axis=1)
                segments.extend([wave, np.zeros(int(sr * .12), dtype=np.float32)])
                samples += len(wave) + int(sr * .12)
                provenance.append(name)
                if samples >= sr * 8:
                    break
            combined = np.concatenate(segments)[:int(sr * 8)]
            peak = np.max(np.abs(combined))
            if peak > 0:
                combined = combined * min(1., .85 / peak)
            sf.write(out, combined, sr, subtype="PCM_16")
            voices.append({"id": speaker, "name": f"VCTK {speaker[1:]}", "file": out.name,
                           "sha256": hashlib.sha256(out.read_bytes()).hexdigest(), "source_clips": provenance, "corpus_gender": CORPUS_GENDERS[speaker]})
            print(f"Prepared {speaker}: {len(combined)/sr:.1f} seconds", flush=True)
    d = {"schema": 1, "name": "VoxGarden VCTK Reference Pack", "license": "CC-BY-4.0",
         "attribution": "Yamagishi, Junichi; Veaux, Christophe; MacDonald, Kirsten (2019). CSTR VCTK Corpus version 0.92. University of Edinburgh, CSTR. DOI: 10.7488/ds/2645.",
         "source": "https://doi.org/10.7488/ds/2645", "mirror": CORPUS_URL,
         "modifications": "Selected mic2 recordings concatenated with short silent gaps, cropped to at most 8 seconds, and peak reduced where necessary. No speaker names inferred.",
         "voices": voices}
    existing.write_text(json.dumps(d, indent=2))
    (destination / "ATTRIBUTION.txt").write_text(d["attribution"] + "\nCC BY 4.0: https://creativecommons.org/licenses/by/4.0/\n" + d["modifications"] + "\nSource: " + d["source"] + "\n")
    return d

if __name__ == "__main__":
    ap = argparse.ArgumentParser(); ap.add_argument("destination")
    prepare(ap.parse_args().destination)
