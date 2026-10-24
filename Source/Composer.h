#pragma once
#include "Editor.h"
#include "Neural.h"
#include "ScoreGrid.h"
class ScoreComposer final:public juce::Component,public juce::FileDragAndDropTarget,private juce::Thread,private juce::Timer {
    VoxGardenProcessor& processor;
    GardenLook look;
    std::function<void(const juce::File&)> sendToChoir;
    ScoreGrid grid;
    juce::TextEditor lyrics;
    juce::Label intro,columns,message;
    juce::ComboBox language,tracks,singingVoice;
    juce::TextButton importMidi{"Import MIDI"},importAudio{"Audio to notes"},assign{"Assign syllables"},generate{"Render dry singing"},choir{"Send dry to choir"},reveal{"Show dry vocal"},download{"Download voices (~401 MB)"},licence{"Model licence"},addNote{"Add note"},deleteNote{"Delete selected notes"},modelFolder{"Open model folder"};
    juce::Label modelStatus;
    std::unique_ptr<juce::FileChooser> chooser;
    std::vector<vg::ScoreTrack> midiTracks;
    vg::Score jobScore;juce::File jobInput,folder,dry;
    enum Job{none,transcribe,sing,setup};Job job=none;
    int jobLanguage=0;juce::String jobVoice{"F1"};
    std::atomic<float> progress{0};juce::String error;vg::Score transcribed;bool pending=false;
    juce::String tr(const char* s)const{return vg::tr(processor.language.load(),juce::String::fromUTF8(s));}
    juce::String tr(const juce::String& s)const{return vg::tr(processor.language.load(),s);}
    void report(const char* s){report(juce::String::fromUTF8(s));}
    void report(const juce::String& s){message.setText(tr(s),juce::dontSendNotification);}
    void saveDraft(){{const juce::ScopedLock lock(processor.scoreLock);processor.scoreDraft=grid.getText();processor.scoreLanguage=language.getSelectedId()-1;processor.scoreVoice=juce::jlimit(0,9,singingVoice.getSelectedId()-1);}dry=juce::File{};choir.setEnabled(false);reveal.setEnabled(false);processor.updateHostDisplay(juce::AudioProcessor::ChangeDetails{}.withNonParameterStateChanged(true));}
    void busy(bool on){for(auto* b:{&importMidi,&importAudio,&assign,&addNote,&deleteNote})b->setEnabled(!on);grid.setReadOnly(on);lyrics.setReadOnly(on);language.setEnabled(!on);singingVoice.setEnabled(!on);tracks.setEnabled(!on);deleteNote.setEnabled(!on&&grid.hasSelection());download.setEnabled(!on);download.setButtonText(tr(vg::singingReady()?"Check / repair models":"Download voices (~401 MB)"));generate.setButtonText(tr(on?"Cancel":"Render dry singing"));choir.setEnabled(!on&&dry.existsAsFile());reveal.setEnabled(!on&&dry.existsAsFile());}
    void import(const juce::File& f){if(pending)return;
        if(f.hasFileExtension("mid;midi")){auto result=vg::readMidiScore(f,midiTracks);if(result.isNotEmpty()){report(result);return;}tracks.clear();for(int i=0;i<(int)midiTracks.size();++i)tracks.addItem(midiTracks[(size_t)i].name,i+1);tracks.setSelectedId(1,juce::sendNotificationSync);report("Choose a MIDI track. Overlapping notes are rendered together.");}
        else{jobInput=f;job=transcribe;pending=true;progress=0;busy(true);report("Extracting a monophonic melody…");startThread();}
    }
    void choose(bool midi){chooser=std::make_unique<juce::FileChooser>(tr(midi?"Import MIDI":"Audio to notes"),juce::File{},midi?"*.mid;*.midi":"*.wav;*.aif;*.aiff;*.flac");chooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[safe=juce::Component::SafePointer<ScoreComposer>(this)](const auto& c){if(safe&&c.getResult().existsAsFile())safe->import(c.getResult());});}
    void run()override{error.clear();if(job==setup){juce::InterProcessLock setupLock("VoxGarden singing setup");if(!setupLock.enter(0)){error="Model setup is already running in another plugin window.";return;}
        struct Unlock {juce::InterProcessLock& lock;~Unlock(){lock.exit();}} unlock{setupLock};
        if(!vg::installSingingHelpers()){error="Could not prepare the singing runtime.";return;}
        auto root=vg::singingRoot();root.getChildFile("Setup Status.json").deleteFile();root.getChildFile("Setup Stage.txt").replaceWithText("Preparing model setup…");
        juce::StringArray command=vg::singingReady()?juce::StringArray{root.getChildFile("python/bin/python").getFullPathName(),root.getChildFile("runtime.py").getFullPathName(),"--setup","--status",root.getChildFile("Setup Status.json").getFullPathName()}:juce::StringArray{"/bin/sh",root.getChildFile("bootstrap.sh").getFullPathName(),root.getFullPathName()};
        int code=vg::runSingingProcess(command,root.getChildFile("Setup.log"),[this]{return threadShouldExit();},[]{});
        if(code==-2)error="Cancelled.";else if(code!=0||!vg::singingReady())error=juce::JSON::parse(root.getChildFile("Setup Status.json")).getProperty("message","Setup failed. Open the model folder to inspect Setup.log and retry.").toString();return;}
        if(job==transcribe)error=vg::transcribeScore(jobInput,transcribed,[this]{return threadShouldExit();});else if(job==sing)error=vg::renderNeuralScore(jobScore,jobLanguage,jobVoice,folder,progress,[this]{return threadShouldExit();},dry);}
    void timerCallback()override{
        if(!pending)return;
        if(isThreadRunning()){if(job==setup){auto root=vg::singingRoot();auto state=juce::JSON::parse(root.getChildFile("Setup Status.json"));modelStatus.setText(tr(state.getProperty("message",root.getChildFile("Setup Stage.txt").loadFileAsString()).toString()),juce::dontSendNotification);}else if(job==sing){auto state=juce::JSON::parse(folder.getChildFile("Singing Status.json"));auto message=state.getProperty("message","Loading the GPU speech model…").toString();report(message+(progress.load()>=0?" "+juce::String(progress.load()*100,0)+"%":""));}return;}
        pending=false;if(error.isEmpty()){if(job==transcribe){tracks.clear();midiTracks.clear();grid.setText(vg::scoreText(transcribed));report("Pitch sketch ready. Check notes and timing before assigning lyrics.");}else if(job==sing)report("Dry singing saved. Use it directly or send it to the choir.");else modelStatus.setText(tr("Ready • Core ML GPU preferred"),juce::dontSendNotification);}else{report(error);if(job==setup)modelStatus.setText(tr(error),juce::dontSendNotification);}busy(false);generate.setEnabled(true);job=none;
    }
public:
    ScoreComposer(VoxGardenProcessor& p,std::function<void(const juce::File&)> callback,const juce::File& initial):Thread("VoxGarden score renderer"),processor(p),sendToChoir(std::move(callback)){
        look.palette=GardenPalette(p.darkMode.load(),p.themeAccent.load());look.language=p.language.load();auto colours=look.palette;
        look.setColour(juce::ResizableWindow::backgroundColourId,colours.paper);look.setColour(juce::Label::textColourId,colours.ink);
        look.setColour(juce::TextEditor::backgroundColourId,colours.panel);look.setColour(juce::TextEditor::textColourId,colours.ink);look.setColour(juce::TextEditor::highlightColourId,colours.forest.withAlpha(.4f));look.setColour(juce::TextEditor::outlineColourId,colours.line);look.setColour(juce::TextEditor::focusedOutlineColourId,colours.forest);
        look.setColour(juce::TableHeaderComponent::backgroundColourId,colours.panel);look.setColour(juce::TableHeaderComponent::textColourId,colours.ink);look.setColour(juce::TableHeaderComponent::outlineColourId,colours.line);look.setColour(juce::TableHeaderComponent::highlightColourId,colours.forest.withAlpha(.2f));
        look.setColour(juce::ComboBox::backgroundColourId,colours.panel);look.setColour(juce::ComboBox::textColourId,colours.ink);
        look.setColour(juce::TextButton::buttonColourId,colours.panel);look.setColour(juce::TextButton::textColourOffId,colours.ink);
        look.setColour(juce::PopupMenu::backgroundColourId,colours.panel);look.setColour(juce::PopupMenu::textColourId,colours.ink);look.setColour(juce::PopupMenu::highlightedBackgroundColourId,colours.forest);look.setColour(juce::PopupMenu::highlightedTextColourId,colours.paper);setLookAndFeel(&look);
        for(auto* c:std::initializer_list<juce::Component*>{&grid,&lyrics,&intro,&columns,&message,&language,&tracks,&importMidi,&importAudio,&assign,&generate,&choir,&reveal,&download,&modelStatus,&licence,&singingVoice,&addNote,&deleteNote,&modelFolder})addAndMakeVisible(c);
        grid.refreshColours();
        importAudio.setButtonText(tr("Audio to notes"));
        intro.setText(tr("Neural pronunciation • MIDI pitch + syllable timing • Core ML GPU preferred"),juce::dontSendNotification);
        columns.setText(tr("Click to select. Cmd/Ctrl-click for more. Double-click to edit."),juce::dontSendNotification);
        grid.setLanguage(p.language.load());grid.onSelectionChange=[this]{deleteNote.setEnabled(!pending&&grid.hasSelection());};addNote.onClick=[this]{grid.addNote();};deleteNote.onClick=[this]{grid.deleteNote();};
        {const juce::ScopedLock lock(p.scoreLock);grid.setText(p.scoreDraft,false);for(int i=0;i<6;++i)language.addItem(juce::String::fromUTF8(vg::singingLanguages[(size_t)i]),i+1);language.setSelectedId(p.scoreLanguage+1,juce::dontSendNotification);jobVoice=juce::String(p.scoreVoice+1);}
        lyrics.setMultiLine(true);lyrics.setReturnKeyStartsNewLine(true);lyrics.setTextToShowWhenEmpty(tr("Separate lyrics with |: one entry per note or simultaneous chord."),juce::Colours::grey);
        grid.onTextChange=[this]{saveDraft();};language.onChange=[this]{saveDraft();};tracks.onChange=[this]{int i=tracks.getSelectedId()-1;if(i>=0&&i<(int)midiTracks.size())grid.setText(vg::scoreText(midiTracks[(size_t)i].notes));};
        singingVoice.addItemList({"F1","F2","F3","F4","F5","M1","M2","M3","M4","M5"},1);singingVoice.setSelectedId(jobVoice.getIntValue(),juce::dontSendNotification);singingVoice.onChange=[this]{saveDraft();};
        modelFolder.onClick=[]{auto root=vg::singingRoot();if(root.exists())root.revealToUser();};
        licence.onClick=[]{juce::URL("https://huggingface.co/Supertone/supertonic-3/blob/724fb5abbf5502583fb520898d45929e62f02c0b/LICENSE").launchInDefaultBrowser();};
        modelStatus.setText(tr(vg::singingReady()?"Ready • Core ML GPU preferred":"401 MB of models + private runtime/cache. Allow 2 GB free. Model licence: OpenRAIL-M."),juce::dontSendNotification);
        download.onClick=[this]{if(pending)return;if(!vg::singingReady()&&juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getBytesFreeOnVolume()<2LL*1024*1024*1024){report("Model setup needs at least 2 GB of free disk space.");return;}job=setup;pending=true;busy(true);startThread();};
        tracks.setTextWhenNothingSelected(tr("MIDI track"));importMidi.onClick=[this]{choose(true);};importAudio.onClick=[this]{choose(false);};
        assign.onClick=[this]{vg::Score notes;auto result=vg::parseScore(grid.getText(),notes);if(result.isNotEmpty()){report(result);return;}auto syllables=juce::StringArray::fromTokens(lyrics.getText(),"|","");for(auto& s:syllables)s=s.trim();if(!vg::assignScoreSyllables(notes,syllables)){report("Supply one syllable per note or simultaneous chord, separated by |.");return;}grid.setText(vg::scoreText(notes));report("Syllables assigned. Adjust note start and duration in seconds.");};
        generate.onClick=[this]{if(pending){signalThreadShouldExit();generate.setEnabled(false);report("Cancelling…");return;}if(!vg::singingReady()){report("Download the singing models in this window first.");return;}error=vg::parseScore(grid.getText(),jobScore);if(error.isNotEmpty()){report(error);return;}folder=juce::File::getSpecialLocation(juce::File::userMusicDirectory).getChildFile("VoxGarden Renders").getNonexistentChildFile("Singing "+juce::Time::getCurrentTime().formatted("%Y-%m-%d %H-%M-%S"),"");jobLanguage=language.getSelectedId()-1;jobVoice=singingVoice.getText();job=sing;pending=true;progress=0;busy(true);startThread();};
        choir.onClick=[this]{if(dry.existsAsFile())sendToChoir(dry);};reveal.onClick=[this]{if(dry.existsAsFile())dry.revealToUser();};
        busy(false);report("Import a melody, align syllables, then render. Neural speech is retimed to your score; this is an experimental singing voice.");
        vg::localiseComponent(*this,p.language.load());setSize(920,750);startTimerHz(8);if(initial.existsAsFile())import(initial);
    }
    ~ScoreComposer()override{stopTimer();signalThreadShouldExit();stopThread(-1);setLookAndFeel(nullptr);}
    void paint(juce::Graphics& g)override{g.fillAll(findColour(juce::ResizableWindow::backgroundColourId));}
    void resized()override{intro.setBounds(16,10,888,30);importMidi.setBounds(16,48,135,30);importAudio.setBounds(160,48,145,30);tracks.setBounds(318,48,250,30);language.setBounds(580,48,200,30);singingVoice.setBounds(792,48,112,30);columns.setBounds(16,88,888,26);grid.setBounds(16,118,888,304);addNote.setBounds(16,430,135,26);deleteNote.setBounds(163,430,225,26);lyrics.setBounds(16,462,710,70);assign.setBounds(738,480,166,32);message.setBounds(16,540,888,56);generate.setBounds(16,612,235,32);choir.setBounds(267,612,235,32);reveal.setBounds(518,612,235,32);download.setBounds(16,666,235,32);modelStatus.setBounds(267,657,637,65);licence.setBounds(16,710,235,26);modelFolder.setBounds(267,710,235,26);}
    bool isInterestedInFileDrag(const juce::StringArray& f)override{return !pending&&f.size()==1&&juce::File(f[0]).hasFileExtension("mid;midi;wav;aif;aiff;flac");}
    void filesDropped(const juce::StringArray& f,int,int)override{if(isInterestedInFileDrag(f))import(juce::File(f[0]));}
};
class ComposerWindow final:public juce::DialogWindow {
public:
    ComposerWindow(VoxGardenProcessor& p,std::function<void(const juce::File&)> callback,const juce::File& initial):DialogWindow(vg::tr(p.language.load(),"Notes + lyrics"),juce::Colour(0xff20292d),true){
        // Keep the drop target above the host when the user clicks a source clip.
        // Do not grab focus while a drag is in progress.
        setAlwaysOnTop(true);setUsingNativeTitleBar(true);auto* content=new ScoreComposer(p,std::move(callback),initial);setContentOwned(content,true);centreWithSize(920,750);setVisible(true);
    }
    void closeButtonPressed()override{setVisible(false);}
};
inline std::unique_ptr<juce::DialogWindow> showComposer(VoxGardenProcessor& p,std::function<void(const juce::File&)> callback,const juce::File& initial={}){return std::make_unique<ComposerWindow>(p,std::move(callback),initial);}
