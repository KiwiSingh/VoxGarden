# VoxGarden 1.0 / Kiwi Singh — audit build

This package contains the native Apple Silicon VST3, standalone app, source,
translation catalog, licensed voice-pack attribution, validation records and
screenshots. The public repository includes these materials.

## New sounds

Under Raga-inspired choirs, each of Yaman, Bhupali, Kafi, Bhairavi, Bhairav,
Malkauns, Durga and Hamsadhwani has three choices: original, Female choir /
male lead, and Male choir / female lead. Total: 62 sounds. Earlier preset IDs
and all sound parameter IDs are retained. New variants use six slots.

Live DSP applies voicing, octave placement and formant colour. AI printing
uses the chosen female/male reference pool as well. The original licensed
pack still has five female and three male references; larger casts cycle
references with different settings/seeds. No new reference recordings or
model downloads were added in 1.0. AI enable/blend remain your choice.

## Suggested listening and workflow review

- Set Sa/root to the song's tonic, then compare all three sounds for a raga.
  Verify the displayed Western notes against your song; this is not key detection.
- Compare the female and male variants on the same clean solo vocal. Adjust
  register and formant colour to the source range; audition AI at full blend
  and against DSP. Listen for pronunciation, timing and pitch artifacts.
- Confirm that muted/soloed parts behave as expected in live preview and in
  the mix/stems. During capture and printing, monitoring remains dry.
- Try Bhupali and Malkauns in particular: octave placement must reflect a
  five-note set, and Malkauns must retain its missing Pa.
- Save/reopen a project with a variant, language and theme selected. Check
  both light/dark themes and Hindi, German, Dutch and Russian menus/help.
- Check your Ableton routing and SnappySnap workflow. Keep tonic/raga and
  gain protected if you want to compare sound changes without changing key.

Automated validation covers routing/state/notes and native rendering; it does
not replace listening on your singing. The raga masks do not generate pakad,
meend, gamakas or direction-specific phrases. Native file dialogs use the OS
language; arbitrary external model diagnostics can remain untranslated.

## Repository materials

In this repository, source is in Source/. Translations/catalog.json and
Source/I18n.h contain the interface translations; English/Hindi/German/Dutch/
Russian screenshots are in Media. Ragas.md links the musical sources.
Validation, LICENSE.txt and Licenses retain the dependency/model/voice-pack
information. Version 1.0.0 is the initial public release.
