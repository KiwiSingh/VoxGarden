# VoxGarden 1.0 / Kiwi Singh

Reviewed official product information on 24 October 2026:

| Reference | Relevant behaviour | VoxGarden choice |
|---|---|---|
| [Antares Choir](https://www.antarestech.com/products/creative-vocal-effects/choir) | Unison ensembles; choir size and individual pitch/timing/vibrato variation | Distinguish doubling/unison sounds from harmonized ensembles; expose 1–8 voices |
| [Antares Harmony Engine](https://www.antarestech.com/products/creative-vocal-effects/harmony-engine) | Key/scale harmony and per-part human variation | Preserve the song's root/scale when browsing voicings |
| [Waves Harmony user guide](https://assets.wavescdn.com/pdf/plugins/waves-harmony.pdf) | Independent harmony parts, placement, snapshots and modulation | Give presets distinct voicings, register, colour and space; retain the existing stable sound parameters for Snaps |
| [Auburn Sounds Graillon](https://www.auburnsounds.com/products/Graillon.html) | Free and paid editions; natural/hard pitch correction, formant shifting and creative effects | Keep restrained studio sounds and experimental sounds clearly labelled |

This is a feature/workflow review of official documentation, not a hands-on audio comparison. No claim that VoxGarden matches their sound quality. No competitor code, preset data or assets were reused.

28 factory starting sounds: 12 existing, 16 additional. They are parameter voicings using the same DSP/optional neural engine, not a new set of recorded singers. The eight licensed reference identities remain unchanged.

Dark mode is the default for new instances. Light/dark is a per-instance host-state preference, outside the sound parameter list. Backgrounds, panels, controls, numbers, menus, labels and text editors receive theme colours. Theme selection is separate from sound presets.

Capture/printing monitor dry at unity gain using the existing compensated latency. Rendering uses the actual chosen choir settings. The engine keeps advancing so delays and pitch processing stay coherent. Ordinary live preview returns after printing finishes or is cancelled; neural results are auditioned by importing the generated WAVs into Live.

0.4.1 adds per-voice M/S controls. Multiple solos are additive, mute takes precedence, and out-of-count solos do not affect active voices. Factory presets clear audition states. Dry lead follows Choir Mix. Muting uses the engine's existing level smoothing. Printed stems preserve all active numbered slots, with excluded parts silent. Sixteen boolean sound parameters append after the existing 57; old states default these controls off.

0.5 adds ten voice-transformation presets, for 38 total, and a saved/automatable AI Voice Cast choice. DSP uses voicings/formants; neural printing selects actual corpus references by their documented F/M metadata. This is an explicit creative selection; input gender is not inferred. Five female and three male references are cycled in single-cast ensembles; split alternates the pools. The exact earlier order remains available as Original mixed cast. Original selected corpus rows and attribution are bundled in AI/Voices/Cast metadata.txt, fetched directly from speaker-info.txt in the licensed VCTK corpus archive. No new recordings or model assets were needed.

0.6 adds eight independently selected raga note sets and eight restrained choir presets. See Ragas.md for primary sources and Western pitch relationships. Raga selection is appended rather than extending the original scale parameter, to preserve normalized automation. The English/Hindi/German/Dutch/Russian interface uses per-instance translation, with no global language state. Hindi glyphs and locale layouts are rendered for visual QA; native file dialogs and technical external diagnostics are outside the catalog.

1.0 retains the 46 earlier preset IDs and adds 16 raga/cast sounds (indices 46–61). Each raga submenu groups its existing sound and both new cast variants. No sound parameters were added or reordered. Five-note raga octaves use five degrees; omitted Pa is not reintroduced. The existing translation keys cover the new compositional names in all four languages. Repository publication follows the v1.0 audit.
