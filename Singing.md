# Notes + lyrics — experimental source preview

This workspace prioritises **specified MIDI pitches and editable syllable timing**. The backend uses **Supertonic 3** for neural pronunciation, then **PSOLA** to set the score pitch and duration while preserving speech articulation. It is not a trained singing model: this first version still needs listening review, especially for long held notes and large pitch shifts. Automatic linguistic syllabification is not claimed.

## Workflow in Ableton Live

1. Open **Notes + lyrics**. Import a standard `.mid`/`.midi` file and select a melody track, or drop the file onto the plugin. Live's **Export MIDI Clip** provides a standard MIDI file.
2. Alternatively, use **Audio to notes** or drop an audio file into this workspace. Start with a clean solo melody. The detector produces a pitch sketch, not polyphonic transcription. Correct octave errors, note boundaries and repeated notes before continuing.
3. Double-click cells in the table to edit **start in seconds**, **length in seconds**, **note name** and **syllable/word**. Use Add note or Delete selected note to change the melody. C4 is MIDI 60; note names and MIDI numbers are accepted. Tempo changes in the MIDI file are converted to seconds; rests remain gaps. The plugin does not follow subsequent Live tempo changes.
4. Enter syllables separated by `|`, then click **Assign syllables**. For D4 C4 D4 A4, four syllables map to notes 62, 60, 62, 69. Split multi-syllable words yourself. Japanese morae and Hindi syllables require the same explicit alignment; automatic spacing cannot reliably infer them. For a melisma, duplicate a vowel across notes and audition the articulation.
5. Choose a pronunciation language: **English, Hindi (Devanagari), German, Japanese, Russian or Dutch**. Click **Download voices (~401 MB)** in this window first. Setup installs a private Python runtime and downloads checksum-verified, pinned models. It reports the current stage and downloaded MB, can be cancelled, and reuses completed model files on retry. No separate installer script or global Python changes are required. Allow at least 2 GB free for models, runtime and compiled caches. The optional neural runtime currently requires Apple Silicon **macOS 14+**; the existing DSP plugin remains usable on its original supported systems. Choose F1–F5 or M1–M5 for five female and five male voice styles.
6. **Render dry singing** writes `Dry Singing.wav` plus an editable `Score.tsv` into a new `Music/VoxGarden Renders/Singing …` folder. **Show dry vocal** reveals the file for auditioning/import into Live.
7. Use the dry vocal as-is, or **Send dry to choir** to run it through VoxGarden's current harmony, voice-count, mute/solo and mutation settings. Choose Scale harmony or Chromatic mode for the choir print. This shortcut uses the fast DSP choir and does not invoke Seed-VC, even if legacy AI printing was enabled. Choir harmony and correction can deliberately change the score's pitches; the original dry file remains available.

## Limits of this preview

- One melody at a time. Overlapping notes are rejected before singing; select the intended MIDI track or edit overlaps. Drum channel 10 is excluded. MIDI type 2 is rejected because tracks have independent timelines. Velocity, expression, sustain pedals and MIDI lyric meta-events are not imported yet.
- Notes must be MIDI 36–96, at least 60 ms long, with a total timeline of at most five minutes and at most 2,048 notes. Rendering is offline and cancellable. Each syllable can contain up to 160 Unicode characters.
- PSOLA sets voiced portions to the requested MIDI fundamental; consonants remain unpitched. It stretches the generated syllable to the selected duration. Large pitch shifts and long extensions can sound artificial or change pronunciation. A short consonant-only syllable may contain no stable sung pitch.
- Timing and pitch edits use an editable table in this first workspace. A piano roll, dedicated phoneme editor, breath controls and trained singing voices are future work.
- Drag/drop works when the host supplies a supported **file path**. Ableton's internal clips can be `.alc` objects with referenced audio, warp markers, loop/trim settings and devices. Direct internal-clip transfer and `.alc` decoding have not been validated or implemented. Export MIDI or consolidate/export audio first; imported audio does not reconstruct Live's warp/clip settings. [Ableton's Live Clip explanation](https://help.ableton.com/hc/en-us/articles/209071569-Using-Live-Clips-alc-files).
- Score text, pronunciation language, voice style, theme family and light/dark preference are stored per plugin instance. Generated files are not embedded in a Live Set; collect the resulting audio with the project.

## Speed and GPU behaviour

Neural sessions explicitly request **Core ML / CPUAndGPU**, with Core ML ahead of CPU in the provider list. This allows Metal GPU execution while retaining necessary CPU graph operations. Setup profiles a synthesis and requires actual Core ML node execution before marking the model ready. It does not silently switch to CPU-only neural inference. Core ML controls scheduling inside its subgraphs; the UI says **GPU preferred**, not “every operation runs on GPU”. PSOLA, file handling and score editing run on CPU, off the audio callback.

Initial measurements on the development Mac: short speech phrases took approximately 0.3–0.9 seconds warm, with several seconds for first synthesis and model loading. A four-word, 3.8-second English D4/C4/D4/A4 score rendered in **6.85 seconds** on its first run; a repeat with cached syllables took **0.064 seconds**. Median voiced pitch errors in that example were within **1.31 cents**. These are small local tests, not a guarantee for every machine, language or score. Download and first Core ML compilation are separate from inference time.

The model loads once per render job, and original neural syllables are cached so note/timing edits can reuse them. A shared process lock serializes jobs across plugin instances to avoid loading several GPU models at once. Per-render timing is written to `Render Stats.json`. Render cancellation stops the worker process group; it retains already cached syllables and completed output files.

## Models and licences

[Supertonic 3](https://huggingface.co/Supertone/supertonic-3) is a roughly 99M-parameter multilingual speech model. Its weights use [OpenRAIL-M](Licenses/Supertonic-3-OpenRAIL-M.txt), separate from its MIT SDK. The pinned model revision is recorded in `Singing/model-manifest.json`, with a SHA-256 checksum for each downloaded asset. No model weights are stored in this Git repository. ONNX Runtime is MIT; Praat/Parselmouth is GPL-compatible with VoxGarden's AGPL distribution. The dependency versions are pinned in the embedded bootstrap.

Natural trained singing voices, a dedicated phoneme editor and legato/melisma synthesis remain future improvements. The existing Seed-VC worker remains available for the older optional voice-conversion workflow; the new score renderer does not use it.
