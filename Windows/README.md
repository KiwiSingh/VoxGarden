# VoxGarden — Windows x64 preview

**English** · [हिन्दी](../README.hi.md) · [Deutsch](../README.de.md) · [Nederlands](../README.nl.md) · [Русский](../README.ru.md)

VST3 + standalone for Windows 10/11 x64. Same 24-voice Amphi stage, 77 sounds, individual mute/solo, themes and MIDI/lyrics workspace. This is a Windows preview, not a claim of manual Ableton/SnappySnap testing on every PC.

Close your audio host. Run Install.cmd and accept the administrator prompt to install into C:\Program Files\Common Files\VST3. The previous plugin is backed up first. Alternatively copy the whole Plugins/VoxGarden.vst3 bundle there manually. Reopen/rescan Live; load VST3 → Kiwi Singh → VoxGarden. Standalone/VoxGarden.exe can run from the extracted folder. The binaries are unsigned.

Download singing voices inside Notes + lyrics (~401 MB models plus a private Python runtime; allow 2 GB free). Setup downloads a checksum-pinned runtime manager and model assets. Windows uses ONNX Runtime DirectML where available and CPU otherwise; the setup profile reports the backend actually used. GPU rendering on a physical Windows PC still needs testing. Mac's Core ML/Metal path is unchanged. No global Python installation is modified. Private data lives in AppData/Roaming/VoxGarden.

Export Live MIDI clips to .mid before import. Render dry, then drag the dry WAV onto the audio track containing VoxGarden to audition. The older optional Seed-VC voice-conversion workflow and its macOS setup script are not included in this Windows preview; DSP choir printing and modern score singing are included.

The installer requires administrator rights only for the common VST3 folder. Singing setup does not. Keep the complete VST3 directory structure. If Windows requests the Microsoft Visual C++ runtime, install Microsoft's current x64 redistributable.

Source, licences and automated validation logs are included. Source: https://github.com/KiwiSingh/VoxGarden

## ☕️ Support the Project

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/kiwisingh)
