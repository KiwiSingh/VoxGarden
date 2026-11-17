# Windows x64 preview validation

A Windows package is built on GitHub's windows-2022 runner with Visual Studio 2022/MSVC. The automated gate includes:

- Native VST3 and standalone compilation with UTF-8 source encoding; 115 native DSP, state, MIDI/score, UI, stage and stem checks against the Windows VST3.
- Tracktion pluginval 1.0.4 at strictness 10, downloaded from its official release with a pinned SHA-256.
- The embedded private-runtime bootstrap through the same native process launcher used by the plugin. The runtime manager ZIP and all 18 neural model files are checksum-pinned.
- Real Supertonic setup/profile and six-language score renders on CPU; voice-file lengths and silent rests are checked. Two offline Python regressions cover overlapping chord pitches, length, output peaks and gain ratios.
- Installer copy and repeat-install backup, with the installed DLL hash compared to the packaged DLL.
- A separate macOS native build/check job for the preserved Mac path.

Actual run output is included in the package's Validation folder. No singing models are bundled, and no downloaded runtime/READY marker is copied into an end user's application data. Models download on explicit request inside Notes + lyrics.

The CI runner is forced to CPU for neural checks. DirectML is preferred on a suitable Windows PC, but physical-GPU inference has not been performance-verified. CPU setup is labelled CPU rather than advertised as GPU. Core ML/Metal remains required on macOS. The Windows profile-file cleanup tolerates ONNX's Windows file locks and uses unique session filenames.

Windows process cancellation uses a kill-on-close Job Object to include downloader children. Parameter IDs/ranges and plugin identity are unchanged. Preferences/model storage uses Windows application data paths; score workers run with UTF-8 enabled for multilingual text. The modern Windows score renderer is included; older optional Seed-VC voice conversion is not.

A real Ableton Live/SnappySnap session, physical-GPU speed and user listening review remain required before treating this as a production Windows release. Mac-only historical validation reports in Source are not Windows test results. The binaries are unsigned. Supported binary architecture is x64; no native Windows ARM64 build is provided.
