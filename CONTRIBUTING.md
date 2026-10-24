# Contributing

Open an issue with a reproducible example, host/version, sample rate and relevant settings. Do not attach private vocals without permission.

Build with the pinned dependencies in prepare-deps.sh and run VoxGardenCheck before submitting DSP changes. Keep existing parameter IDs, indices and normalized ranges stable. Update tests when changing raga masks, voice routing or state migration.

Interface translations live in Source/I18n.h, mirrored in Translations/catalog.json. Update both; keep technical identifiers, parameter IDs and filenames unchanged. Help translations are in Source/I18n.h. README translations are README.hi.md, README.de.md, README.nl.md and README.ru.md. Preserve the language selector and relative screenshot/document links.

Include screenshots for visible changes and test both themes, especially Hindi shaping and text fit.
