# Free local AI singer printing

VoxGarden uses Seed-VC's 44.1kHz singing-voice model to resynthesize each harmony as a different singer. It conditions the model on the original vocal and a scale-aware pitch contour, rather than asking a speech generator to invent lyrics. Eight reference voices come from CSTR VCTK 0.92 under CC BY 4.0; attribution and source-clip IDs are in Voices/manifest.json.

Run **Setup AI.command** once to install an isolated Python runtime, fetch model assets, and perform a real local conversion test. Allow several GB of free disk space and time for the downloads. Nothing is installed into global Python. An Apple Silicon Mac with 16GB or more memory is the intended platform. The runtime and weights live in Library/Application Support/VoxGarden/AI in your user folder, outside synced Documents.

After setup, enable **AI singers on print** in the plugin and use Render vocal + stems as usual. The adjacent slider blends AI singers with the DSP choir (rightmost is fully AI). Your chosen root, scale, harmony degrees, correction, detune, vibrato and glide condition the neural pitch trajectory. The native engine then applies voice colour, timing, levels, placement, room and output controls. MIDI-chord printing is still handled through Live's recorder rather than this offline flow.

AI conversion happens only when printing: live playback and SnappySnap remain the DSP preview. AI printing can take minutes, particularly for many singers and long vocals. Use Cancel print to stop a job; the captured input is retained. Multiple AI jobs queue rather than loading several models simultaneously. Once setup is complete, printing is offline and vocal audio is not uploaded.

The AI Sources subfolder retains raw singer files, model logs, settings, reference IDs and attribution for review. Final mix and stems retain the same duration and sample rate as the DSP print, including its four-second tail. Model frame rounding is cropped/padded at the end; no global stretching is applied. Small timing differences within a neural performance can remain.

This is an optional experimental quality path. We have not established that it sounds better for every singer or recording. Listen for consonant changes, phrase alignment, pitch deviations and artifacts. There is no newly trained professional choir dataset or claim that VCTK speech references are recorded choral performances.

Upstream code: https://github.com/Plachtaa/seed-vc (GPL-3.0, pinned to 51383efd921027683c89e5348211d93ff12ac2a8).
Seed-VC weights: https://huggingface.co/Plachta/Seed-VC (GPL-3.0).
Reference corpus: https://doi.org/10.7488/ds/2645 (CC BY 4.0).
Reference mirror: https://zenodo.org/records/10691876.
The setup retains upstream dependencies and their notices. The voice-pack recordings have been selected, concatenated and cropped to eight seconds; speaker IDs are retained without identifying individuals.

## Voice casting (0.5)

The new cast parameter routes references using the corpus's speaker-info.txt labels. See Voices/Cast metadata.txt for the eight original rows and attribution. Pools are female p225/p228/p230/p231/p229 and male p232/p226/p227, in pack order. Slots cycle the selected pool; split alternates female and male. The original cast preserves the exact previous eight-slot order. Every rendered slot records its actual selected reference ID in AI Provenance.json. Missing cast metadata fails explicitly. Existing jobs without aiCast use the original order. Setup records voice_cast_version=1 so an older runtime cannot silently ignore a new cast selection.
