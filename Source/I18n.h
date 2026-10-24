#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <array>
namespace vg {
inline constexpr std::array<const char*,5> languageNames{"English","हिन्दी","Deutsch","Nederlands","Русский"};
inline const std::array<std::array<const char*,5>,254> translations{{
    {"Reference voices for AI printing. DSP preview uses voicing and formant colour. New character presets set both. No input voice classification is performed.","AI रेंडर के संदर्भ स्वर। DSP में स्वर-विन्यास व रंग बदलते हैं। इनपुट की पहचान नहीं की जाती।","Referenzstimmen für KI-Rendern. DSP nutzt Voicings und Formanten. Keine Klassifizierung der Eingangsstimme.","Referentiestemmen voor AI-renderen. DSP gebruikt stemvoering en formanten. De invoerstem wordt niet geclassificeerd.","Референсы для ИИ-рендера. DSP меняет ноты и форманты. Входной голос не классифицируется."},
    {"Import a melody, align syllables, then render. Neural speech is retimed to your score; this is an experimental singing voice.","धुन आयात करें, अक्षरांश मिलाएँ और रेंडर करें। न्यूरल बोली का समय व सुर बदले जाते हैं; यह प्रयोगात्मक गायन है।","Melodie importieren, Silben zuweisen, rendern. Neuronale Sprache wird an die Noten angepasst; experimentelle Gesangsstimme.","Importeer een melodie, lijn lettergrepen uit en render. Neurale spraak wordt aan de noten aangepast; experimentele zangstem.","Импортируйте мелодию, назначьте слоги и создайте вокал. Нейронная речь подгоняется под ноты; это экспериментальное пение."},
    {"Import a monophonic melody or edit the example. This first voice is a synthetic vocoder, not a natural AI singer.","एक स्वर वाली धुन आयात करें या उदाहरण बदलें। यह पहला स्वर कृत्रिम वोकोडर है, प्राकृतिक AI गायक नहीं।","Einstimmige Melodie importieren oder Beispiel bearbeiten. Die erste Stimme ist ein synthetischer Vocoder, kein natürlicher KI-Gesang.","Importeer een eenstemmige melodie of bewerk het voorbeeld. De eerste stem is een synthetische vocoder, geen natuurlijke AI-zanger.","Импортируйте одноголосную мелодию или измените пример. Первый голос — синтетический вокодер, а не естественный ИИ-певец."},
    {"Track rendering uses Scale harmony or Chromatic mode. Print MIDI-chord performances with Live's audio recorder.","ट्रैक रेंडर में स्वर-समूह या क्रोमैटिक मोड चुनें। MIDI प्रदर्शन Live में रिकॉर्ड करें।","Spurrendern nutzt Tonleiter oder Chromatik. MIDI-Akkorde in Live aufnehmen.","Gebruik toonladder- of chromatische modus. Neem MIDI-akkoorden in Live op.","Для рендера выберите лад или хроматику. MIDI-аккорды записывайте в Live."},
    {"Update AI cast support: run Setup AI.command from the new VoxGarden package. Existing models can be reused.","AI स्वर-समूह अपडेट करें: नए पैकेज से Setup AI.command चलाएँ। पुराने मॉडल काम करेंगे।","KI-Besetzung aktualisieren: Setup AI.command aus neuem Paket starten. Modelle bleiben nutzbar.","Werk AI-stembezetting bij met Setup AI.command uit het nieuwe pakket. Modellen blijven bruikbaar.","Обновите ИИ-состав: запустите Setup AI.command из нового пакета. Модели можно использовать повторно."},
    {"Update AI raga support: run Setup AI.command from the new VoxGarden package. Existing models can be reused.","AI राग समर्थन अपडेट करें: नए पैकेज से Setup AI.command चलाएँ। पुराने मॉडल काम करेंगे।","KI-Raga-Unterstützung aktualisieren: Setup AI.command aus neuem Paket starten. Modelle bleiben nutzbar.","Werk AI-ragaondersteuning bij met Setup AI.command uit het nieuwe pakket. Modellen blijven bruikbaar.","Обновите поддержку раг: запустите Setup AI.command из нового пакета. Модели можно использовать повторно."},
    {"No installed macOS voice for this language. Add one in System Settings > Accessibility > Spoken Content.","इस भाषा का macOS स्वर स्थापित नहीं है। सिस्टम सेटिंग्स > ऐक्सेसिबिलिटी > बोली गई सामग्री में जोड़ें।","Keine macOS-Stimme für diese Sprache installiert. In Systemeinstellungen > Bedienungshilfen > Gesprochene Inhalte hinzufügen.","Geen macOS-stem voor deze taal geïnstalleerd. Voeg er een toe via Systeeminstellingen > Toegankelijkheid > Gesproken materiaal.","Нет голоса macOS для этого языка. Добавьте его в Системных настройках > Универсальный доступ > Устный контент."},
    {"AI runtime is not ready. Turn off AI singers for a DSP print, or run Setup AI.command.","AI रनटाइम तैयार नहीं है। DSP रेंडर चुनें या Setup AI.command चलाएँ।","KI-Laufzeit nicht bereit. KI deaktivieren oder Setup AI.command ausführen.","AI-runtime is niet klaar. Schakel AI uit of voer Setup AI.command uit.","ИИ-среда не готова. Отключите ИИ или запустите Setup AI.command."},
    {"Start (seconds)    Duration (seconds)    MIDI note    Syllable — TAB-separated columns","शुरुआत (सेकंड)    अवधि (सेकंड)    MIDI स्वर    अक्षरांश — कॉलम TAB से अलग करें","Start (Sekunden)    Dauer (Sekunden)    MIDI-Note    Silbe — Spalten mit TAB trennen","Start (seconden)    Duur (seconden)    MIDI-noot    Lettergreep — kolommen met TAB scheiden","Начало (секунды)    Длина (секунды)    MIDI-нота    Слог — столбцы через TAB"},
    {"401 MB of models + private runtime/cache. Allow 2 GB free. Model licence: OpenRAIL-M.","401 MB मॉडल + निजी रनटाइम/कैश। 2 GB खाली रखें। मॉडल लाइसेंस: OpenRAIL-M।","401 MB Modelle + eigene Laufzeit/Cache. 2 GB frei halten. Modelllizenz: OpenRAIL-M.","401 MB modellen + eigen runtime/cache. Houd 2 GB vrij. Modellicentie: OpenRAIL-M.","401 МБ моделей + отдельная среда/кэш. Нужно 2 ГБ свободно. Лицензия: OpenRAIL-M."},
    {"AI singers need the free local runtime. Run Setup AI.command from the package first.","AI गायकों के लिए स्थानीय रनटाइम चाहिए। पैकेज से Setup AI.command चलाएँ।","Für KI-Stimmen zuerst Setup AI.command aus dem Paket ausführen.","Voer eerst Setup AI.command uit het pakket uit voor AI-stemmen.","Для ИИ-голосов сначала запустите Setup AI.command из пакета."},
    {"Solo this choir voice. Multiple solos combine. Dry lead is controlled by Choir Mix.","यह स्वर अकेला सुनें। कई सोलो साथ बजते हैं। मूल स्वर मंडली मिश्रण से नियंत्रित है।","Diese Stimme solo hören. Mehrere Solos kombinieren sich. Trockensignal über Chormix steuern.","Beluister deze stem solo. Meerdere solo's spelen samen. Droge zang regel je met Koormix.","Прослушать голос соло. Несколько соло звучат вместе. Сухой вокал регулируется миксом хора."},
    {"AI singers need the free local runtime. Run Setup AI.command in the package first.","AI गायकों के लिए स्थानीय रनटाइम चाहिए। पैकेज से Setup AI.command चलाएँ।","Für KI-Stimmen zuerst Setup AI.command aus dem Paket ausführen.","Voer eerst Setup AI.command uit het pakket uit voor AI-stemmen.","Для ИИ-голосов сначала запустите Setup AI.command из пакета."},
    {"Capture buffer overflowed. The partial take was retained; try a quieter CPU load.","रिकॉर्डिंग बफ़र भर गया। आंशिक रिकॉर्डिंग सुरक्षित है; CPU भार घटाएँ।","Aufnahmepuffer übergelaufen. Teilaufnahme bleibt erhalten; CPU entlasten.","Opnamebuffer overgelopen. Gedeeltelijke opname bewaard; verlaag de CPU-belasting.","Переполнение буфера. Частичная запись сохранена; снизьте нагрузку CPU."},
    {"Capture the vocal from this track, then print your choir and individual parts.","इस ट्रैक का स्वर रिकॉर्ड करें, फिर मंडली व अलग स्वर रेंडर करें।","Gesang dieser Spur aufnehmen, dann Chor und Einzelstimmen rendern.","Neem de zang van deze track op en render koor en losse stemmen.","Запишите вокал с дорожки, затем создайте хор и отдельные партии."},
    {"AI singers are ready. Enable AI singers on print for neural voice conversion.","AI गायक तैयार हैं। न्यूरल रेंडर के लिए AI विकल्प चालू करें।","KI-Stimmen bereit. KI-Stimmen zum Rendern aktivieren.","AI-stemmen zijn klaar. Schakel ze in voor neurale conversie.","ИИ-голоса готовы. Включите их для нейронного рендера."},
    {"Type syllables separated by |, one per note. Example: Hel | lo | my | choir","हर स्वर के लिए एक अक्षरांश, | से अलग करें। जैसे: सा | रे | गा | मा","Eine Silbe pro Note, mit | trennen. Beispiel: Hal | lo | mein | Chor","Eén lettergreep per noot, gescheiden door |. Voorbeeld: Hal | lo | mijn | koor","По слогу на ноту, разделяя |. Например: При | вет | мой | хор"},
    {"Neural pronunciation • MIDI pitch + syllable timing • Core ML GPU preferred","न्यूरल उच्चारण • MIDI स्वर + अक्षरांश समय • Core ML GPU प्राथमिक","Neuronale Aussprache • MIDI-Tonhöhe + Silbentiming • Core ML bevorzugt GPU","Neurale uitspraak • MIDI-toonhoogte + lettergreeptiming • Core ML met GPU-voorkeur","Нейронное произношение • MIDI-высота + время слогов • приоритет GPU Core ML"},
    {"AI singer blend for printed audio. Live playback remains the DSP preview.","रेंडर में AI गायकों का मिश्रण। लाइव प्लेबैक DSP पूर्वावलोकन है।","KI-Stimmenanteil beim Rendern. Live bleibt die DSP-Vorschau.","Aandeel AI-stemmen in de render. Live blijft de DSP-voorvertoning.","Доля ИИ-голосов в рендере. В реальном времени звучит DSP."},
    {"Synthetic singing • MIDI notes + your syllables • offline macOS voices","कृत्रिम गायन • MIDI स्वर + आपके अक्षरांश • ऑफ़लाइन macOS स्वर","Synthetischer Gesang • MIDI-Noten + Ihre Silben • lokale macOS-Stimmen","Synthetische zang • MIDI-noten + je lettergrepen • lokale macOS-stemmen","Синтетическое пение • MIDI-ноты + ваши слоги • локальные голоса macOS"},
    {"Overlapping notes: choose a monophonic melody track or edit the times.","स्वर साथ बज रहे हैं: एक स्वर वाली धुन चुनें या समय बदलें।","Noten überlappen: einstimmige Melodiespur wählen oder Zeiten bearbeiten.","Noten overlappen: kies een eenstemmige melodie of bewerk de tijden.","Ноты пересекаются: выберите одноголосную мелодию или измените время."},
    {"Pitch sketch ready. Check notes and timing before assigning lyrics.","स्वर रूपरेखा तैयार है। गीत जोड़ने से पहले स्वर व समय जाँचें।","Notenskizze bereit. Noten und Timing vor der Textzuweisung prüfen.","Notenschets klaar. Controleer noten en timing vóór het toewijzen van tekst.","Ноты распознаны приблизительно. Проверьте высоту и время перед назначением текста."},
    {"Double-click a cell to edit timing, note or syllable. C4 = MIDI 60.","समय, स्वर या अक्षरांश बदलने के लिए सेल पर डबल-क्लिक करें। C4 = MIDI 60।","Zelle doppelklicken, um Zeit, Note oder Silbe zu bearbeiten. C4 = MIDI 60.","Dubbelklik een cel om timing, noot of lettergreep te bewerken. C4 = MIDI 60.","Дважды щёлкните ячейку для правки времени, ноты или слога. C4 = MIDI 60."},
    {"Capture stopped: disk write failed. The partial take was retained.","रिकॉर्डिंग रुकी: डिस्क लेखन विफल। आंशिक रिकॉर्डिंग सुरक्षित है।","Aufnahme gestoppt: Schreibfehler. Teilaufnahme bleibt erhalten.","Opname gestopt: schrijffout. Gedeeltelijke opname blijft behouden.","Запись остановлена: ошибка диска. Частичная запись сохранена."},
    {"No stable monophonic notes detected. Try a clean, isolated melody.","स्थिर एक स्वर वाली धुन नहीं मिली। साफ़, अलग धुन आज़माएँ।","Keine stabilen einstimmigen Noten erkannt. Saubere, isolierte Melodie versuchen.","Geen stabiele eenstemmige noten herkend. Probeer een schone, geïsoleerde melodie.","Стабильные ноты не найдены. Попробуйте чистую изолированную мелодию."},
    {"No channel audio arrived. Play the vocal in Live while capturing.","ट्रैक से ऑडियो नहीं आया। रिकॉर्डिंग के समय Live में स्वर चलाएँ।","Kein Audiosignal. Gesang während der Aufnahme in Live abspielen.","Geen audio ontvangen. Speel de zang in Live af tijdens de opname.","Аудио не поступило. Проиграйте вокал в Live во время записи."},
    {"0 = unison   /   2 = third   /   4 = fifth in seven-note scales","सात-स्वर समूह: 0 = समस्वर / 2 = तृतीय / 4 = पंचम","Siebentönig: 0 = Unisono / 2 = Terz / 4 = Quinte","Zeventonig: 0 = unisono / 2 = terts / 4 = kwint","Семь нот: 0 = унисон / 2 = терция / 4 = квинта"},
    {"Mute this choir voice. Level is preserved; mute overrides solo.","इस स्वर को बंद करें। स्तर बना रहेगा; म्यूट सोलो पर प्राथमिक है।","Diese Stimme stummschalten. Pegel bleibt erhalten; Stumm hat Vorrang vor Solo.","Demp deze stem. Niveau blijft behouden; dempen gaat vóór solo.","Отключить голос. Уровень сохраняется; отключение важнее соло."},
    {"Choose a melody track. Edit overlapping notes before rendering.","धुन ट्रैक चुनें। रेंडर से पहले साथ बजते स्वर ठीक करें।","Melodiespur wählen. Überlappende Noten vor dem Rendern bearbeiten.","Kies een melodietrack. Bewerk overlappende noten vóór het renderen.","Выберите мелодию. Устраните пересечения нот перед рендером."},
    {"Note-set equivalent; phrasing and ornaments are not generated.","स्वर-समूह का समकक्ष; राग की चाल व अलंकार उत्पन्न नहीं होते।","Nur Tonvorrat; Phrasierung und Verzierungen werden nicht erzeugt.","Alleen de notenset; frasering en versieringen worden niet gemaakt.","Совпадает набор нот; фразировка и украшения не создаются."},
    {"Supply exactly one nonempty syllable per note, separated by |.","हर स्वर के लिए एक अक्षरांश लिखें और | से अलग करें।","Genau eine nicht leere Silbe pro Note, getrennt durch |.","Geef precies één niet-lege lettergreep per noot, gescheiden door |.","Введите ровно один непустой слог на ноту, разделяя их |."},
    {"Syllables assigned. Adjust note start and duration in seconds.","अक्षरांश जुड़ गए। स्वर की शुरुआत और अवधि सेकंड में बदलें।","Silben zugewiesen. Notenstart und Dauer in Sekunden bearbeiten.","Lettergrepen toegewezen. Pas start en duur in seconden aan.","Слоги назначены. Отредактируйте начало и длину нот в секундах."},
    {"Audio setup changed. Any captured take was retained on disk.","ऑडियो सेटअप बदला। रिकॉर्डिंग डिस्क पर सुरक्षित है।","Audioeinstellungen geändert. Aufnahme bleibt gespeichert.","Audio-instellingen gewijzigd. De opname blijft bewaard.","Настройки аудио изменились. Запись сохранена на диске."},
    {"Dry singing saved. Use it directly or send it to the choir.","मूल गायन सहेजा गया। सीधे इस्तेमाल करें या मंडली में भेजें।","Trockener Gesang gespeichert. Direkt nutzen oder an den Chor senden.","Droge zang opgeslagen. Gebruik deze direct of stuur hem naar het koor.","Сухой вокал сохранён. Используйте его напрямую или передайте в хор."},
    {"Could not create a take folder in Music/VoxGarden Renders.","Music/VoxGarden Renders में फ़ोल्डर नहीं बन सका।","Aufnahmeordner in Music/VoxGarden Renders konnte nicht erstellt werden.","Kan geen opnamemap maken in Music/VoxGarden Renders.","Не удалось создать папку в Music/VoxGarden Renders."},
    {"Choose Scale harmony or Chromatic mode to render a vocal.","स्वर रेंडर के लिए स्वर-समूह या क्रोमैटिक मोड चुनें।","Zum Rendern Tonleiterharmonie oder Chromatik wählen.","Kies toonladderharmonie of chromatische modus om te renderen.","Для рендера выберите гармонию по ладу или хроматику."},
    {"Installing the neural runtime (no global Python changes)…","न्यूरल रनटाइम स्थापित हो रहा है (वैश्विक Python बदलाव नहीं)…","Neuronale Laufzeit installieren (globales Python bleibt unverändert)…","Neurale runtime installeren (geen globale Python-wijzigingen)…","Установка нейронной среды (системный Python не меняется)…"},
    {"Model setup is already running in another plugin window.","मॉडल सेटअप दूसरी प्लगइन विंडो में चल रहा है।","Modelleinrichtung läuft bereits in einem anderen Pluginfenster.","Modelinstallatie loopt al in een ander pluginvenster.","Установка моделей уже выполняется в другом окне плагина."},
    {"AI source count does not match the selected choir size.","AI स्वरों की संख्या मंडली से मेल नहीं खाती।","KI-Stimmenanzahl stimmt nicht mit Chorgröße überein.","Aantal AI-stemmen komt niet overeen met de koorgrootte.","Число ИИ-голосов не совпадает с размером хора."},
    {"Stop the channel capture before rendering another file.","दूसरी फ़ाइल रेंडर करने से पहले ट्रैक रिकॉर्डिंग रोकें।","Vor dem Rendern einer anderen Datei die Spuraufnahme stoppen.","Stop de trackopname voordat je een ander bestand rendert.","Остановите запись дорожки перед рендером другого файла."},
    {"One voice. A room full of possibility.  /  Kiwi Singh","एक स्वर। अनगिनत संभावनाएँ। / Kiwi Singh","Eine Stimme. Ein Raum voller Möglichkeiten. / Kiwi Singh","Eén stem. Een ruimte vol mogelijkheden. / Kiwi Singh","Один голос. Простор возможностей. / Kiwi Singh"},
    {"The AI renderer did not return every selected singer.","AI रेंडर से सभी चुने स्वर नहीं मिले।","KI-Renderer hat nicht alle Stimmen geliefert.","AI-renderer heeft niet alle gekozen stemmen geleverd.","ИИ-рендер вернул не все выбранные голоса."},
    {"Start audio playback in your host before capturing.","रिकॉर्डिंग से पहले होस्ट में प्लेबैक शुरू करें।","Vor Aufnahme die Wiedergabe im Host starten.","Start de weergave in je host vóór de opname.","Перед записью запустите воспроизведение в хосте."},
    {"AI render cancelled. Your source take was retained.","AI रेंडर रद्द। मूल रिकॉर्डिंग सुरक्षित है।","KI-Rendern abgebrochen. Originalaufnahme bleibt erhalten.","AI-render geannuleerd. De bronopname blijft behouden.","ИИ-рендер отменён. Исходная запись сохранена."},
    {"Model setup needs at least 2 GB of free disk space.","मॉडल सेटअप के लिए कम से कम 2 GB खाली स्थान चाहिए।","Modelleinrichtung benötigt mindestens 2 GB freien Speicher.","Modelinstallatie vereist minimaal 2 GB vrije schijfruimte.","Для установки моделей нужно не менее 2 ГБ свободного места."},
    {"Each note needs a syllable (up to 160 characters).","हर स्वर को एक अक्षरांश चाहिए (अधिकतम 160 अक्षर)।","Jede Note braucht eine Silbe (bis zu 160 Zeichen).","Elke noot heeft een lettergreep nodig (maximaal 160 tekens).","Каждой ноте нужен слог (до 160 символов)."},
    {"Play the vocal passage, then click Stop + render.","स्वर चलाएँ, फिर रोकें व रेंडर करें दबाएँ।","Gesang abspielen, dann Stoppen und rendern klicken.","Speel de zang af en klik op Stoppen en renderen.","Проиграйте вокал и нажмите «Стоп и рендер»."},
    {"AI singer files do not align with the input take.","AI स्वर फ़ाइलों का समय मूल स्वर से मेल नहीं खाता।","KI-Stimmdateien passen zeitlich nicht zur Aufnahme.","AI-stembestanden lopen niet gelijk met de opname.","ИИ-файлы не совпадают по времени с исходной записью."},
    {"Download the singing models in this window first.","पहले इसी विंडो से गायन मॉडल डाउनलोड करें।","Zuerst die Gesangsmodelle in diesem Fenster laden.","Download eerst de zangmodellen in dit venster.","Сначала скачайте модели пения в этом окне."},
    {"Cancelling print; your source take will be kept.","रेंडर रद्द हो रहा है; मूल रिकॉर्डिंग सुरक्षित रहेगी।","Rendern abbrechen; Originalaufnahme bleibt erhalten.","Render annuleren; de bronopname blijft behouden.","Отмена рендера; исходная запись сохранится."},
    {"Compiling GPU models and checking a first voice…","GPU मॉडल संकलित हो रहे हैं और पहला स्वर जाँचा जा रहा है…","GPU-Modelle kompilieren und erste Stimme prüfen…","GPU-modellen compileren en eerste stem controleren…","Компиляция моделей GPU и проверка первого голоса…"},
    {"Downloading the runtime manager (Apple Silicon)…","रनटाइम प्रबंधक डाउनलोड हो रहा है (Apple Silicon)…","Laufzeitverwaltung laden (Apple Silicon)…","Runtimebeheer laden (Apple Silicon)…","Загрузка менеджера среды (Apple Silicon)…"},
    {"Use an audio excerpt of five minutes or less.","पाँच मिनट या कम का ऑडियो लें।","Audioausschnitt von höchstens fünf Minuten verwenden.","Gebruik een audiofragment van maximaal vijf minuten.","Используйте аудиофрагмент не длиннее пяти минут."},
    {"Downloading and verifying the voice models…","स्वर मॉडल डाउनलोड और जाँचे जा रहे हैं…","Sprachmodelle laden und prüfen…","Stemmodellen downloaden en controleren…","Загрузка и проверка голосовых моделей…"},
    {"Render cancelled. Partial files remain in ","रेंडर रद्द। आंशिक फ़ाइलें यहाँ हैं: ","Rendern abgebrochen. Teildateien bleiben in ","Render geannuleerd. Gedeeltelijke bestanden staan in ","Рендер отменён. Частичные файлы находятся в "},
    {"Choose a raga or keep your Western scale.","राग चुनें या अपना पश्चिमी स्वर-समूह रखें।","Raga wählen oder westliche Tonleiter beibehalten.","Kies een raga of behoud je westerse toonladder.","Выберите рагу или оставьте западный лад."},
    {"AI singers ready for the final choir mix","अंतिम मिश्रण के लिए AI स्वर तैयार हैं","KI-Stimmen bereit für den Chormix","AI-stemmen klaar voor de koormix","ИИ-голоса готовы к сведению хора"},
    {"Could not create the AI render folder.","AI रेंडर फ़ोल्डर नहीं बन सका।","KI-Renderordner konnte nicht erstellt werden.","Kan de AI-rendermap niet maken.","Не удалось создать папку ИИ-рендера."},
    {"Could not start the local AI renderer.","स्थानीय AI रेंडर शुरू नहीं हो सका।","Lokaler KI-Renderer konnte nicht gestartet werden.","Kan de lokale AI-renderer niet starten.","Не удалось запустить локальный ИИ-рендер."},
    {"Could not create a track capture file.","ट्रैक रिकॉर्डिंग फ़ाइल नहीं बन सकी।","Aufnahmedatei konnte nicht erstellt werden.","Kan het opnamebestand niet maken.","Не удалось создать файл записи дорожки."},
    {"Drag each singer to place their voice","हर गायक को खींचकर स्थान दें","Stimmen durch Ziehen platzieren","Sleep elke stem naar zijn plek","Перетащите голоса на сцене"},
    {"Waiting for the GPU singing renderer…","GPU गायन रेंडरर का इंतज़ार…","Warten auf GPU-Gesangsrenderer…","Wachten op de GPU-zangrenderer…","Ожидание GPU-рендера пения…"},
    {"Preparing the private Python runtime…","निजी Python रनटाइम तैयार हो रहा है…","Eigene Python-Laufzeit vorbereiten…","Eigen Python-runtime voorbereiden…","Подготовка отдельной среды Python…"},
    {"Could not read a standard MIDI file.","मानक MIDI फ़ाइल पढ़ी नहीं जा सकी।","Standard-MIDI-Datei konnte nicht gelesen werden.","Kan standaard MIDI-bestand niet lezen.","Не удалось прочитать стандартный MIDI-файл."},
    {"Could not open the capture writer.","रिकॉर्डिंग फ़ाइल नहीं खोली जा सकी।","Aufnahmedatei konnte nicht geöffnet werden.","Kan het opnamebestand niet openen.","Не удалось открыть файл записи."},
    {"No ordinary major/minor equivalent","सामान्य मेजर/माइनर समकक्ष नहीं","Keine gewöhnliche Dur-/Moll-Entsprechung","Geen gewone majeur-/mineurtegenhanger","Нет обычного мажорного/минорного аналога"},
    {"Waiting for the local AI renderer","स्थानीय AI रेंडर की प्रतीक्षा","Warte auf lokalen KI-Renderer","Wachten op lokale AI-renderer","Ожидание локального ИИ-рендера"},
    {"mapping vocal pitch to your scale","स्वर को चुने समूह में ढालना","Tonhöhe an Tonleiter anpassen","Toonhoogte aan toonladder koppelen","привязка высоты к ладу"},
    {"Could not save the AI render job.","AI रेंडर कार्य नहीं सहेजा जा सका।","KI-Renderauftrag konnte nicht gespeichert werden.","Kan de AI-renderopdracht niet opslaan.","Не удалось сохранить задание ИИ-рендера."},
    {"Finishing your captured take...","रिकॉर्डिंग पूरी की जा रही है…","Aufnahme abschließen…","Opname afronden…","Завершение записи…"},
    {"Loading the local singing model","स्थानीय गायन मॉडल लोड हो रहा है","Lokales Gesangsmodell laden","Lokaal zangmodel laden","Загрузка локальной вокальной модели"},
    {"Could not read that audio file.","ऑडियो फ़ाइल नहीं पढ़ी जा सकी।","Audiodatei konnte nicht gelesen werden.","Kan het audiobestand niet lezen.","Не удалось прочитать аудиофайл."},
    {"Could not create output folder.","आउटपुट फ़ोल्डर नहीं बन सका।","Ausgabeordner konnte nicht erstellt werden.","Kan de uitvoermap niet maken.","Не удалось создать выходную папку."},
    {"Extracting a monophonic melody…","एक स्वर वाली धुन निकाली जा रही है…","Einstimmige Melodie wird erkannt…","Eenstemmige melodie wordt herkend…","Распознаётся одноголосная мелодия…"},
    {"Add or import some notes first.","पहले स्वर जोड़ें या आयात करें।","Zuerst Noten hinzufügen oder importieren.","Voeg eerst noten toe of importeer ze.","Сначала добавьте или импортируйте ноты."},
    {"Ready • Core ML GPU preferred","तैयार • Core ML GPU प्राथमिक","Bereit • Core ML bevorzugt GPU","Klaar • Core ML met GPU-voorkeur","Готово • приоритет GPU Core ML"},
    {"Loading the GPU speech model…","GPU स्वर मॉडल लोड हो रहा है…","GPU-Sprachmodell wird geladen…","GPU-spraakmodel wordt geladen…","Загружается речевая модель GPU…"},
    {"Preparing channel capture...","ट्रैक रिकॉर्डिंग की तैयारी…","Spuraufnahme vorbereiten…","Trackopname voorbereiden…","Подготовка записи дорожки…"},
    {"Could not create WAV writer.","WAV लेखक नहीं बन सका।","WAV-Datei konnte nicht angelegt werden.","Kan geen WAV-bestand aanmaken.","Не удалось создать WAV-файл."},
    {"Rendering synthetic singing…","कृत्रिम गायन बन रहा है…","Synthetischer Gesang wird gerendert…","Synthetische zang wordt gerenderd…","Создаётся синтетическое пение…"},
    {"Your choir in Ableton Live","Ableton Live में आपकी मंडली","Dein Chor in Ableton Live","Je koor in Ableton Live","Ваш хор в Ableton Live"},
    {"Starting local AI renderer","स्थानीय AI रेंडर शुरू हो रहा है","Lokalen KI-Renderer starten","Lokale AI-renderer starten","Запуск локального ИИ-рендера"},
    {"Alternating female / male","बारी-बारी महिला / पुरुष","Abwechselnd Frauen / Männer","Afwisselend vrouwen / mannen","Чередование женских / мужских"},
    {"analysing reference pitch","संदर्भ स्वर का विश्लेषण","Referenztonhöhe analysieren","Referentietoonhoogte analyseren","анализ высоты референса"},
    {"Download voices (~401 MB)","स्वर डाउनलोड करें (~401 MB)","Stimmen laden (~401 MB)","Stemmen laden (~401 MB)","Скачать голоса (~401 МБ)"},
    {"Cinematic & experimental","सिनेमाई व प्रयोगात्मक","Filmisch und experimentell","Filmisch en experimenteel","Кино и эксперименты"},
    {"Female choir / male lead","महिला मंडली / पुरुष मुख्य स्वर","Frauenchor / Männerlead","Vrouwenkoor / mannenlead","Женский хор / мужской солист"},
    {"Male choir / female lead","पुरुष मंडली / महिला मुख्य स्वर","Männerchor / Frauenlead","Mannenkoor / vrouwenlead","Мужской хор / женская солистка"},
    {"Listening for your vocal","आपके स्वर की प्रतीक्षा","Warte auf Gesang","Wachten op je stem","Ожидание вокала"},
    {"aligning converted voice","बदले स्वर का समय मिलाना","Konvertierte Stimme ausrichten","Omgezette stem uitlijnen","выравнивание преобразованного голоса"},
    {"Could not write outputs.","आउटपुट नहीं लिखे जा सके।","Ausgaben konnten nicht geschrieben werden.","Kan de uitvoer niet schrijven.","Не удалось записать файлы."},
    {"analysing vocal content","स्वर सामग्री का विश्लेषण","Gesangsinhalt analysieren","Zanginhoud analyseren","анализ вокального материала"},
    {"Use at most 2048 notes.","अधिकतम 2048 स्वर इस्तेमाल करें।","Höchstens 2048 Noten verwenden.","Gebruik maximaal 2048 noten.","Используйте не более 2048 нот."},
    {"Mixing choir and stems","मंडली व अलग स्वर मिलाए जा रहे हैं","Chor und Einzelspuren mischen","Koor en losse stemmen mixen","Сведение хора и партий"},
    {"Double harmonic major","डबल हार्मोनिक मेजर","Doppelharmonisch Dur","Dubbelharmonisch majeur","Двойной гармонический мажор"},
    {"Voice transformations","स्वर रूपांतरण","Stimmverwandlungen","Stemtransformaties","Преображение голосов"},
    {"Related major notes: ","संबंधित मेजर स्वर: ","Verwandter Dur-Tonvorrat: ","Verwante majeurnoten: ","Родственный мажор: "},
    {"Check / repair models","मॉडल जाँचें / सुधारें","Modelle prüfen / reparieren","Modellen controleren / herstellen","Проверить / исправить модели"},
    {"Render vocal + stems","स्वर व अलग ट्रैक रेंडर करें","Chor und Einzelspuren rendern","Koor en losse stemmen renderen","Рендер хора и партий"},
    {"Explore the choir...","गायक-मंडली चुनें…","Chorklang wählen…","Koorklank kiezen…","Выберите звучание хора…"},
    {"Suspended pentatonic","सस्पेंडेड पेंटाटोनिक","Sus-Pentatonik","Sus-pentatoniek","Пентатоника sus"},
    {"Octave constellation","सप्तक नक्षत्र","Oktavkonstellation","Octaafsterrenbeeld","Созвездие октав"},
    {"Raga-inspired choirs","राग-प्रेरित मंडलियाँ","Raga-inspirierte Chöre","Door raga geïnspireerde koren","Хоры по мотивам раг"},
    {"Synthesizing singer ","गायक बनाया जा रहा है ","Stimme synthetisieren ","Stem synthetiseren ","Синтез голоса "},
    {"Delete selected note","चुना स्वर हटाएँ","Gewählte Note löschen","Geselecteerde noot wissen","Удалить выбранную ноту"},
    {"Downloading models: ","मॉडल डाउनलोड: ","Modelle laden: ","Modellen downloaden: ","Загрузка моделей: "},
    {"AI singers on print","AI गायक से रेंडर","KI-Stimmen beim Rendern","AI-stemmen bij renderen","ИИ-голоса при рендере"},
    {"Chromatic intervals","अर्धस्वर अंतराल","Chromatische Intervalle","Chromatische intervallen","Хроматические интервалы"},
    {"Minor pentatonic b6","माइनर पेंटाटोनिक ♭6","Moll-Pentatonik mit b6","Mineurpentatoniek met b6","Минорная пентатоника с ♭6"},
    {"Original mixed cast","मूल मिश्रित स्वर-समूह","Ursprüngliche Mischbesetzung","Oorspronkelijke gemengde bezetting","Исходный смешанный состав"},
    {"Render a vocal file","स्वर की फ़ाइल रेंडर करें","Gesangsdatei rendern","Zangbestand renderen","Рендер вокального файла"},
    {"CAPTURING DRY INPUT","मूल स्वर रिकॉर्ड हो रहा है","TROCKENSIGNAL WIRD AUFGENOMMEN","DROOG SIGNAAL OPNEMEN","ЗАПИСЬ СУХОГО ВХОДА"},
    {"START WITH A SOUND","ध्वनि से शुरू करें","MIT EINEM KLANG BEGINNEN","BEGIN MET EEN KLANK","ВЫБЕРИТЕ ЗВУЧАНИЕ"},
    {"Major pentatonic 7","मेजर पेंटाटोनिक 7","Dur-Pentatonik mit 7","Majeurpentatoniek met 7","Мажорная пентатоника с 7"},
    {"Cinematic ensemble","सिनेमाई मंडली","Filmensemble","Filmisch ensemble","Кинематографический ансамбль"},
    {"Fifths in the mist","धुंध में पंचम","Quinten im Nebel","Kwinten in de mist","Квинты в тумане"},
    {"Disk write failed.","डिस्क पर लिखना विफल।","Schreiben auf Datenträger fehlgeschlagen.","Schrijven naar schijf mislukt.","Ошибка записи на диск."},
    {"Stem write failed.","अलग स्वर लिखना विफल।","Einzelspur konnte nicht geschrieben werden.","Losse stem schrijven mislukt.","Ошибка записи партии."},
    {"AI render failed: ","AI रेंडर विफल: ","KI-Rendern fehlgeschlagen: ","AI-render mislukt: ","Ошибка ИИ-рендера: "},
    {"Render dry singing","मूल गायन रेंडर करें","Trockenen Gesang rendern","Droge zang renderen","Создать сухой вокал"},
    {"Preserve formants","स्वर-रंग बनाए रखें","Formanten erhalten","Formanten behouden","Сохранять форманты"},
    {"Octave foundation","सप्तक आधार","Oktavfundament","Octaaffundament","Октавная основа"},
    {"Cathedral octaves","गिरजाघर सप्तक","Kathedraloktaven","Kathedraaloctaven","Соборные октавы"},
    {"Mixed choir split","मिश्रित विभाजित मंडली","Geteilter gemischter Chor","Gesplitst gemengd koor","Разделённый смешанный хор"},
    {"Send dry to choir","मूल गायन मंडली में भेजें","Gesang an Chor senden","Zang naar koor sturen","Передать вокал в хор"},
    {"MIDI melody track","MIDI धुन ट्रैक","MIDI-Melodiespur","MIDI-melodietrack","Дорожка MIDI-мелодии"},
    {"Open model folder","मॉडल फ़ोल्डर खोलें","Modellordner öffnen","Modelmap openen","Открыть папку моделей"},
    {"Major pentatonic","मेजर पेंटाटोनिक","Dur-Pentatonik","Majeurpentatoniek","Мажорная пентатоника"},
    {"Minor pentatonic","माइनर पेंटाटोनिक","Moll-Pentatonik","Mineurpentatoniek","Минорная пентатоника"},
    {"Beyond the choir","मंडली से आगे","Jenseits des Chors","Voorbij het koor","За пределами хора"},
    {"Studio & harmony","स्टूडियो व संगति","Studio und Harmonie","Studio en harmonie","Студия и гармония"},
    {"Extended harmony","विस्तारित संगति","Erweiterte Harmonie","Uitgebreide harmonie","Расширенная гармония"},
    {"Electric quartet","इलेक्ट्रिक चौकड़ी","Elektrisches Quartett","Elektrisch kwartet","Электрический квартет"},
    {"Assign syllables","अक्षरांश जोड़ें","Silben zuweisen","Lettergrepen toewijzen","Назначить слоги"},
    {"Natural backing","स्वाभाविक सहगान","Natürlicher Hintergrundchor","Natuurlijke achtergrondzang","Естественный бэк-вокал"},
    {"Unison ensemble","समस्वर मंडली","Unisono-Ensemble","Unisono-ensemble","Унисонный ансамбль"},
    {"Soft alto choir","कोमल ऑल्टो मंडली","Sanfter Altchor","Zacht altkoor","Мягкий альтовый хор"},
    {"Drifting unison","बहता समस्वर","Schwebendes Unisono","Zwevend unisono","Парящий унисон"},
    {"Fractured choir","खंडित मंडली","Zersplitterter Chor","Gebroken koor","Расколотый хор"},
    {"RAGA / NOTE SET","राग / स्वर-समूह","RAGA / TONVORRAT","RAGA / NOTENSET","РАГА / НАБОР НОТ"},
    {"synthesis part ","रचना भाग ","Syntheseteil ","Synthesedeel ","часть синтеза "},
    {"Syllable / word","अक्षरांश / शब्द","Silbe / Wort","Lettergreep / woord","Слог / слово"},
    {"THE SOUNDSTAGE","स्वर-मंच","DIE KLANGBÜHNE","HET KLANKPODIUM","ЗВУКОВАЯ СЦЕНА"},
    {"Harmonic minor","हार्मोनिक माइनर","Harmonisch Moll","Harmonisch mineur","Гармонический минор"},
    {"Natural voices","स्वाभाविक स्वर","Natürliche Stimmen","Natuurlijke stemmen","Естественные голоса"},
    {"Studio quartet","स्टूडियो चौकड़ी","Studioquartett","Studiokwartet","Студийный квартет"},
    {"Gospel backing","गॉस्पेल सहगान","Gospel-Hintergrundchor","Gospelachtergrondkoor","Госпел-бэк-вокал"},
    {"Suspended trio","सस्पेंडेड तिकड़ी","Sus-Trio","Sus-trio","Трио sus"},
    {"Bass cathedral","बास गिरजाघर","Basskathedrale","Baskathedraal","Басовый собор"},
    {"Her & him duet","स्त्री-पुरुष युगल","Sie-und-er-Duett","Zij-en-hij-duet","Дуэт «Она и он»"},
    {"Dry monitoring","मूल स्वर सुनाई दे रहा है","Trockenes Monitoring","Droge monitoring","Сухой мониторинг"},
    {"Notes + lyrics","स्वर + गीत","Noten + Liedtext","Noten + tekst","Ноты + текст"},
    {"Show dry vocal","मूल गायन दिखाएँ","Gesangsdatei anzeigen","Droge zang tonen","Показать сухой вокал"},
    {"Audio to notes","ऑडियो से स्वर","Audio zu Noten","Audio naar noten","Аудио в ноты"},
    {"Render a file","फ़ाइल रेंडर करें","Datei rendern","Bestand renderen","Рендер файла"},
    {"Ableton setup","Ableton सेटअप","Ableton einrichten","Ableton instellen","Настройка Ableton"},
    {"Stop + render","रोकें व रेंडर करें","Stoppen und rendern","Stoppen en renderen","Стоп и рендер"},
    {"AI VOICE CAST","AI स्वर-समूह","KI-STIMMBESETZUNG","AI-STEMBEZETTING","ИИ-СОСТАВ"},
    {"YOUR ENSEMBLE","आपकी गायक-मंडली","DEIN ENSEMBLE","JE ENSEMBLE","ВАШ АНСАМБЛЬ"},
    {"Scale harmony","स्वर-समूह संगति","Tonleiterharmonie","Toonladderharmonie","Гармония по ладу"},
    {"Natural minor","नैचुरल माइनर","Natürlich Moll","Natuurlijk mineur","Натуральный минор"},
    {"Melodic minor","मेलोडिक माइनर","Melodisch Moll","Melodisch mineur","Мелодический минор"},
    {"Female voices","महिला स्वर","Frauenstimmen","Vrouwenstemmen","Женские голоса"},
    {"Intimate duet","अंतरंग युगल","Intimes Duett","Intiem duet","Камерный дуэт"},
    {"Strange bloom","अनोखा पुष्प","Seltsame Blüte","Vreemde bloei","Необычный цветок"},
    {"Soul sevenths","सोल सप्तम","Soul-Septimen","Soul-septiemen","Соул-септимы"},
    {"Cluster bloom","स्वर-गुच्छ पुष्प","Clusterblüte","Clusterbloei","Цветение кластера"},
    {"Baritone club","बैरिटोन सभा","Baritonclub","Baritonclub","Баритон-клуб"},
    {"Gentle giants","सौम्य दानव","Sanfte Riesen","Zachtaardige reuzen","Добрые великаны"},
    {"Audio → notes","ऑडियो → स्वर","Audio → Noten","Audio → noten","Аудио → ноты"},
    {"Model licence","मॉडल लाइसेंस","Modelllizenz","Modellicentie","Лицензия модели"},
    {"Show renders","रेंडर दिखाएँ","Renderdateien zeigen","Renders tonen","Показать файлы"},
    {"Cancel print","रेंडर रद्द करें","Rendern abbrechen","Renderen annuleren","Отменить рендер"},
    {"HARMONY MODE","संगति मोड","HARMONIEMODUS","HARMONIEMODUS","РЕЖИМ ГАРМОНИИ"},
    {"VIBRATO / CT","कंपन / ct","VIBRATO / CT","VIBRATO / CT","ВИБРАТО / ЦЕНТ"},
    {"Warm chamber","मधुर कक्ष","Warmer Kammerchor","Warm kamerkoor","Тёплый камерный хор"},
    {"Glass garden","काँच का बाग़","Glasgarten","Glazen tuin","Стеклянный сад"},
    {"Close double","निकट दोहराव","Nahes Double","Nauwe dubbeling","Близкий дубль"},
    {"Velvet altos","मखमली ऑल्टो","Samtige Altstimmen","Fluwelen alten","Бархатные альты"},
    {"Soprano halo","सोप्रानो प्रभामंडल","Sopranhalo","Sopraanhalo","Сопрановое сияние"},
    {"Pixie chorus","परी मंडली","Feenchor","Elfenkoor","Хор фей"},
    {"Saved mix + ","मिश्रण व सुरक्षित स्वर: ","Mix und Einzelspuren gespeichert: ","Mix en losse stemmen opgeslagen: ","Сохранён микс и партии: "},
    {"DETUNE / CT","सूक्ष्म अंतर / ct","VERSTIMMUNG / CT","ONTSTEMMING / CT","РАССТРОЙКА / ЦЕНТ"},
    {"TIMING / MS","समय-अंतर / ms","ZEITVERSATZ / MS","TIJDVARIATIE / MS","ВРЕМЯ / МС"},
    {"COLOUR / ST","स्वर-रंग / st","KLANGFARBE / HT","KLANKKLEUR / HT","ТЕМБР / ПОЛУТОН"},
    {"Male voices","पुरुष स्वर","Männerstimmen","Mannenstemmen","Мужские голоса"},
    {"Open fifths","खुले पंचम","Offene Quinten","Open kwinten","Открытые квинты"},
    {"Airy unison","हल्का समस्वर","Luftiges Unisono","Luchtig unisono","Воздушный унисон"},
    {"Wide double","विस्तृत दोहराव","Breites Double","Brede dubbeling","Широкий дубль"},
    {"Hamsadhwani","हंसध्वनि (Hamsadhwani)","Hamsadhwani","Hamsadhwani","Хамсадхвани (Hamsadhwani)"},
    {"Import MIDI","MIDI आयात करें","MIDI importieren","MIDI importeren","Импорт MIDI"},
    {"Cancelling…","रद्द हो रहा है…","Wird abgebrochen…","Annuleren…","Отмена…"},
    {"CHOIR SIZE","गायकों की संख्या","CHORGRÖSSE","KOORGROOTTE","РАЗМЕР ХОРА"},
    {"SCALE LOCK","स्वर बंधन","TONARTBINDUNG","TOONLADDERBINDING","ПРИВЯЗКА К ЛАДУ"},
    {"GLIDE / MS","स्वर-सरकाव / ms","GLEITZEIT / MS","GLIJTIJD / MS","СКОЛЬЖЕНИЕ / МС"},
    {"DEPTH / MS","गहराई / ms","TIEFE / MS","DIEPTE / MS","ГЛУБИНА / МС"},
    {"MIDI chord","MIDI कॉर्ड","MIDI-Akkord","MIDI-akkoord","MIDI-аккорд"},
    {"Mixolydian","मिक्सोलिडियन","Mixolydisch","Mixolydisch","Миксолидийский"},
    {"Low velvet","गहरा मखमल","Tiefer Samt","Laag fluweel","Низкий бархат"},
    {"Cancelled.","रद्द किया गया।","Abgebrochen.","Geannuleerd.","Отменено."},
    {"Length (s)","अवधि (से.)","Dauer (s)","Duur (s)","Длина (с)"},
    {"Dark mode","डार्क मोड","Dunkelmodus","Donkere modus","Тёмная тема"},
    {"CHOIR MIX","मंडली मिश्रण","CHORMIX","KOORMIX","МИКС ХОРА"},
    {"Chromatic","क्रोमैटिक","Chromatisch","Chromatisch","Хроматический"},
    {"High silk","ऊँचा रेशम","Hohe Seide","Hoge zijde","Высокий шёлк"},
    {" (subset)"," (आंशिक)"," (Teilmenge)"," (deelverzameling)"," (подмножество)"},
    {"Start (s)","शुरुआत (से.)","Start (s)","Start (s)","Начало (с)"},
    {"MUTATION","रूपांतरण","MUTATION","MUTATIE","МУТАЦИЯ"},
    {"POSITION","स्थान","POSITION","POSITIE","ПОЛОЖЕНИЕ"},
    {"Phrygian","फ़्रिजियन","Phrygisch","Frygisch","Фригийский"},
    {"Bhairavi","भैरवी (Bhairavi)","Bhairavi","Bhairavi","Бхайрави (Bhairavi)"},
    {"Malkauns","मालकौंस (Malkauns)","Malkauns","Malkauns","Малкаунс (Malkauns)"},
    {" stems: "," अलग ट्रैक: "," Einzelspuren: "," losse stemmen: "," партий: "},
    {"Add note","स्वर जोड़ें","Note hinzufügen","Noot toevoegen","Добавить ноту"},
    {"DISTANT","दूर","FERN","VER WEG","ДАЛЕКО"},
    {"HARMONY","स्वर-संगति","HARMONIE","HARMONIE","ГАРМОНИЯ"},
    {"Bhupali","भूपाली (Bhupali)","Bhupali","Bhupali","Бхупали (Bhupali)"},
    {"Bhairav","भैरव (Bhairav)","Bhairav","Bhairav","Бхайрав (Bhairav)"},
    {"Notes: ","स्वर: ","Töne: ","Noten: ","Ноты: "},
    {"Input  ","स्वर  ","Eingang  ","Ingang  ","Вход  "},
    {" voices"," स्वर"," Stimmen"," stemmen"," голосов"},
    {"Singer ","गायक ","Stimme ","Stem ","Голос "},
    {", step ",", चरण ",", Schritt ",", stap ",", шаг "},
    {"OUTPUT","आउटपुट","AUSGANG","UITGANG","ВЫХОД"},
    {"Dorian","डोरियन","Dorisch","Dorisch","Дорийский"},
    {"Lydian","लिडियन","Lydisch","Lydisch","Лидийский"},
    {"Chapel","प्रार्थना-कक्ष","Kapelle","Kapel","Часовня"},
    {" voice"," स्वर"," Stimme"," stem"," голос"},
    {"Voice ","स्वर ","Stimme ","Stem ","Голос "},
    {" saved"," सुरक्षित"," gespeichert"," opgeslagen"," сохранён"},
    {"Garden","बगीचा","Garten","Tuin","Сад"},
    {"Sunset","सूर्यास्त","Sonnenuntergang","Zonsondergang","Закат"},
    {"Cancel","रद्द करें","Abbrechen","Annuleren","Отмена"},
    {"SCALE","स्वर-समूह","TONLEITER","TOONLADDER","ЛАД"},
    {"CLOSE","पास","NAH","DICHTBIJ","БЛИЗКО"},
    {"WIDTH","विस्तार","BREITE","BREEDTE","ШИРИНА"},
    {"LEVEL","स्तर","PEGEL","NIVEAU","УРОВЕНЬ"},
    {"Major","मेजर","Dur","Majeur","Мажор"},
    {"Yaman","यमन (Yaman)","Yaman","Yaman","Яман (Yaman)"},
    {"Durga","दुर्गा (Durga)","Durga","Durga","Дурга (Durga)"},
    {"Sa = ","सा = ","Sa = ","Sa = ","Sa = "},
    {" mute"," म्यूट"," stummschalten"," dempen"," отключить"},
    {" solo"," सोलो"," solo"," solo"," соло"},
    {"Ocean","समुद्री","Ozean","Oceaan","Океан"},
    {"ROOT","मूल स्वर / सा","GRUNDTON / SA","GRONDTOON / SA","ТОНИКА / SA"},
    {"ROOM","कक्ष","RAUM","RUIMTE","ПОМЕЩЕНИЕ"},
    {"Kafi","काफ़ी (Kafi)","Kafi","Kafi","Кафи (Kafi)"},
    {" sec"," सेकंड"," Sek."," sec."," с"},
    {" of "," / "," von "," van "," из "},
    {"Rose","गुलाबी","Rosa","Roze","Розовая"},
    {"Note","स्वर","Note","Noot","Нота"},
    {"Off","बंद","Aus","Uit","Выкл."},
    {"L","बायाँ","L","L","Л"},
    {"R","दायाँ","R","R","П"},
}};
inline juce::String tr(int language,const juce::String& text){
    if(language<=0 || language>4 || text.isEmpty())return text;
    for(const auto& row:translations)if(text==juce::String::fromUTF8(row[0]))return juce::String::fromUTF8(row[(size_t)language]);
    auto result=text;
    for(const auto& row:translations){auto key=juce::String::fromUTF8(row[0]);if(key.length()>3)result=result.replace(key,juce::String::fromUTF8(row[(size_t)language]));}
    return result;
}
inline void localiseComponent(juce::Component& component,int language){
    auto translate=[&](const juce::String& current,const juce::String& field){
        auto& properties=component.getProperties();auto source=field+"Source",last=field+"Last",lang=field+"Language";
        if(!properties.contains(source)||current!=properties[last].toString())properties.set(source,current);
        if(current==properties[last].toString()&&(int)properties.getWithDefault(lang,-1)==language)return current;
        auto value=tr(language,properties[source].toString());properties.set(last,value);properties.set(lang,language);return value;
    };
    if(auto* label=dynamic_cast<juce::Label*>(&component);label && dynamic_cast<juce::ComboBox*>(component.getParentComponent())==nullptr)label->setText(translate(label->getText(),"text"),juce::dontSendNotification);
    if(auto* button=dynamic_cast<juce::Button*>(&component))button->setButtonText(translate(button->getButtonText(),"button"));
    if(auto* tooltip=dynamic_cast<juce::SettableTooltipClient*>(&component))tooltip->setTooltip(translate(tooltip->getTooltip(),"tip"));
    for(auto* child:component.getChildren())localiseComponent(*child,language);
}
inline bool usesDevanagari(const juce::String& text){for(auto c:text)if(c>=0x900&&c<=0x97f)return true;return false;}
inline juce::String voiceCountText(int language,int count){
    if(language==4)return juce::String(count)+juce::String::fromUTF8(count==1?" голос":count<5?" голоса":" голосов");
    return juce::String(count)+tr(language,count==1?" voice":" voices");
}
inline juce::String helpText(int language){
    static const std::array<const char*,5> text{
R"HELP(Insert VoxGarden on the vocal track in Ableton Live. Choose the tonic (Sa), scale or raga note set, and a starting sound. Raga presets change the note set but keep Sa. The Western display shows matching notes, not an automatic key detection or a complete raga performance.

Click Render vocal + stems, play the passage in Live, then Stop + render. Monitoring is dry during capture/printing. The saved choir uses settings at Stop; automation during capture is not recorded. Files are in Music/VoxGarden Renders. MIDI-chord performances should be recorded using Live.

Enable AI singers on print for local neural conversion. AI Voice Cast selects reference voices for printing only. M mutes; S solos; several solos combine. Set Choir Mix to 100% to isolate choir from the dry lead. Raga note sets do not generate pakad, meend, gamakas or direction-specific phrases.

Keep Delay Compensation on. DSP latency is about 192 ms at 48 kHz. In SnappySnapFX, save different settings as Snaps; keep tonic, scale/raga and output gain protected. Language and theme are saved per instance.)HELP",
R"HELP(Ableton Live में स्वर वाले ट्रैक पर VoxGarden लगाएँ। मूल स्वर (सा), पश्चिमी स्वर-समूह या राग चुनें, फिर ध्वनि चुनें। राग प्रीसेट सा को बदले बिना स्वर-समूह बदलता है। पश्चिमी नाम और स्वर केवल नोटों के समकक्ष हैं; यह स्वतः टोनल की पहचान या पूरा राग-प्रदर्शन नहीं है।

स्वर व अलग ट्रैक रेंडर करें दबाएँ, Live में अंश चलाएँ, फिर रोकें व रेंडर करें दबाएँ। रिकॉर्डिंग और रेंडर के दौरान मूल स्वर सुनाई देता है। रेंडर रुकने के समय की सेटिंग लेता है; रिकॉर्डिंग की ऑटोमेशन शामिल नहीं होती। फ़ाइलें Music/VoxGarden Renders में मिलेंगी। MIDI कॉर्ड का प्रदर्शन Live में रिकॉर्ड करें।

स्थानीय न्यूरल रूपांतरण के लिए AI गायक चालू करें। AI स्वर-समूह केवल रेंडर के संदर्भ स्वर चुनता है। M से स्वर बंद और S से सोलो होता है; कई सोलो साथ बजते हैं। मूल स्वर हटाकर केवल मंडली सुनने के लिए मिश्रण 100% रखें। राग सुविधा पकड़, मींड, गमक या आरोह-अवरोह की विशेष चाल नहीं बनाती।

Delay Compensation चालू रखें। 48 kHz पर DSP विलंब लगभग 192 ms है। SnappySnapFX में अलग सेटिंग Snap के रूप में रखें; सा, स्वर-समूह/राग और आउटपुट स्तर सुरक्षित रखें। भाषा व थीम हर इंस्टेंस के साथ सहेजी जाती हैं।)HELP",
R"HELP(VoxGarden auf der Gesangsspur in Ableton Live einsetzen. Grundton (Sa), Tonleiter oder Raga-Tonvorrat und einen Startklang wählen. Raga-Presets ändern den Tonvorrat, behalten Sa aber bei. Die westliche Anzeige zeigt passende Töne; sie ist keine automatische Tonarterkennung oder vollständige Raga-Darbietung.

Chor und Einzelspuren rendern anklicken, die Passage in Live abspielen, dann Stoppen und rendern. Während Aufnahme und Rendern hörst du das Trockensignal. Gerendert wird mit den Einstellungen beim Stoppen, ohne Aufnahme der Parameterautomation. Dateien liegen in Music/VoxGarden Renders. MIDI-Akkorde mit Live aufnehmen.

KI-Stimmen beim Rendern für lokale neuronale Konvertierung aktivieren. Die KI-Stimmbesetzung betrifft nur gerenderte Referenzstimmen. M schaltet stumm, S schaltet solo; mehrere Solos kombinieren sich. Chormix auf 100% setzen, um nur den Chor zu hören. Raga-Tonvorräte erzeugen keine charakteristischen Phrasen oder Verzierungen.

Delay Compensation eingeschaltet lassen. DSP-Latenz: etwa 192 ms bei 48 kHz. In SnappySnapFX Einstellungen als Snaps speichern; Grundton, Tonleiter/Raga und Ausgangspegel schützen. Sprache und Thema werden je Instanz gespeichert.)HELP",
R"HELP(Plaats VoxGarden op de zangtrack in Ableton Live. Kies grondtoon (Sa), toonladder of raganotenset en een beginklank. Ragapresets wijzigen de notenset maar behouden Sa. De westerse weergave toont overeenkomstige noten; dit is geen automatische toonsoortdetectie of volledige raga-uitvoering.

Klik op Koor en losse stemmen renderen, speel de passage in Live af en klik op Stoppen en renderen. Tijdens opname en renderen hoor je het droge signaal. De render gebruikt de instellingen bij Stop; parameterautomatisering tijdens de opname wordt niet vastgelegd. Bestanden staan in Music/VoxGarden Renders. Neem MIDI-akkoorden op met Live.

Schakel AI-stemmen bij renderen in voor lokale neurale conversie. AI-stembezetting kiest alleen referenties voor de render. M dempt, S zet solo; meerdere solo's spelen samen. Zet Koormix op 100% om alleen het koor te horen. Raganotensets maken geen karakteristieke frasen of versieringen.

Laat Delay Compensation aan. DSP-vertraging: ongeveer 192 ms bij 48 kHz. Sla instellingen in SnappySnapFX op als Snaps; bescherm grondtoon, toonladder/raga en uitgangsniveau. Taal en thema worden per instantie bewaard.)HELP",
R"HELP(Добавьте VoxGarden на вокальную дорожку в Ableton Live. Выберите тонику (Sa), западный лад или набор нот раги и начальное звучание. Пресеты раг меняют набор нот, сохраняя Sa. Западные названия и ноты не означают автоматическое определение тональности или полное исполнение раги.

Нажмите «Рендер хора и партий», проиграйте фрагмент в Live, затем нажмите «Стоп и рендер». Во время записи и рендера слышен сухой сигнал. Используются настройки в момент остановки; автоматизация во время записи не записывается. Файлы находятся в Music/VoxGarden Renders. MIDI-аккорды записывайте средствами Live.

Включите ИИ-голоса для локального нейронного преобразования. Состав ИИ-голосов выбирает референсы только для рендера. M отключает голос, S включает соло; несколько соло звучат вместе. Для прослушивания только хора установите микс хора на 100%. Набор нот раги не создаёт характерную фразировку, скольжения и украшения.

Оставьте Delay Compensation включённой. Задержка DSP — около 192 мс при 48 кГц. Сохраняйте настройки в SnappySnapFX как Snaps; защитите тонику, лад/рагу и выходной уровень. Язык и тема сохраняются для каждого экземпляра.)HELP"};
    return juce::String::fromUTF8(text[(size_t)juce::jlimit(0,4,language)]);
}
}
