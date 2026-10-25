#include "Processor.h"
#include "Offline.h"
#include "AI.h"
#include "I18n.h"
#include "Composer.h"
#include <iostream>
#include <chrono>
static int failures=0;
static void check(bool pass,const char* message){std::cout<<(pass?"PASS ":"FAIL ")<<message<<std::endl;if(!pass)++failures;}
int main(int argc,char** argv) {
    juce::ScopedJuceInitialiser_GUI gui;
    if(argc>1 && juce::String(argv[1])=="--neural-setup"){
        check(vg::installSingingHelpers(),"Embedded model helpers install without a separate script package");auto root=vg::singingRoot();int code=vg::runSingingProcess({"/bin/sh",root.getChildFile("bootstrap.sh").getFullPathName(),root.getFullPathName()},root.getChildFile("Setup.log"),[]{return false;},[]{});check(code==0&&vg::singingReady(),"Integrated setup verifies models and confirms Core ML execution");return failures?1:0;
    }
    if(argc>2 && juce::String(argv[1])=="--neural-smoke"){
        auto folder=juce::File(juce::String(argv[2]));std::atomic<float> progress{0};constexpr const char* words[]{"la","सा","la","ら","ла","la"};
        for(int language=0;language<6;++language){auto word=juce::String::fromUTF8(words[language]);vg::Score score{{0,.8,62,word},{1,.8,60,word},{2,.8,69,word}};juce::File dry;auto error=vg::renderNeuralScore(score,language,"F1",folder.getChildFile(juce::String(vg::singingLocales[(size_t)language])),progress,[]{return false;},dry);std::cout<<vg::singingLocales[(size_t)language]<<" "<<error<<std::endl;check(error.isEmpty()&&dry.existsAsFile(),"GPU neural score render returns a dry vocal");if(error.isNotEmpty())continue;
            juce::AudioFormatManager fm;fm.registerBasicFormats();std::unique_ptr<juce::AudioFormatReader> r(fm.createReaderFor(dry));check(r&&std::abs(r->lengthInSamples/r->sampleRate-2.8)<.001,"Neural singing preserves score length");
            if(r){juce::AudioBuffer<float> audio(1,(int)r->lengthInSamples);r->read(&audio,0,audio.getNumSamples(),0,true,false);check(audio.getMagnitude(0,(int)(.8*r->sampleRate), (int)(.2*r->sampleRate))<.00001f,"Neural score rests remain silent");auto analysis=vg::singingRoot().getChildFile("python/bin/python");std::cout<<folder.getChildFile(juce::String(vg::singingLocales[(size_t)language])).getChildFile("Render Stats.json").loadFileAsString()<<std::endl;}
        }return failures?1:0;
    }
    if(argc>3 && juce::String(argv[1])=="--ai-smoke"){
        VoxGardenProcessor p;p.applyPreset(5);p.setValue("root",9);p.setValue("scale",1);p.setValue("aiPrint",1);p.setValue("aiBlend",1);
        if(argc>4)p.setValue("aiCast",juce::String(argv[4]).getIntValue());
        if(argc>5)p.setValue("raga",juce::String(argv[5]).getIntValue());
        auto settings=p.settings();juce::File input{juce::String(argv[2])},output{juce::String(argv[3])};std::atomic<float> progress{0};std::vector<juce::File> singers;
        auto error=vg::createAISingers(input,output,settings,progress,[]{return false;},singers);check(error.isEmpty(),"Plugin launches local AI worker and receives two singers");if(error.isNotEmpty()){std::cout<<error<<std::endl;return 1;}
        auto result=renderVocal(input,output,settings,progress,[]{return false;},singers);std::cout<<result<<std::endl;check(progress.load()==1,"AI singers pass through native mixing and stem export");return failures?1:0;
    }
    {
        vg::Score notes;check(vg::parseScore(juce::String::fromUTF8("0\t0.5\t62\tसा\n0.75\t0.4\t60\tら"),notes).isEmpty()&&notes.size()==2&&notes[0].syllable==juce::String::fromUTF8("सा"),"Score preserves Unicode syllables, exact pitches and rests");
        check(vg::parseScore("0\t2\t62\tla\n0.5\t1\t60\tla",notes).isEmpty()&&notes.size()==2&&notes[0].duration==2,"Overlapping score notes preserve both durations");
        {vg::Score chords{{1,.5,67,"la"},{0,2,60,"la"},{0,1,64,"la"}};check(vg::assignScoreSyllables(chords,{"Hello","choir"})&&chords[0].syllable=="choir"&&chords[1].syllable=="Hello"&&chords[2].syllable=="Hello","One lyric per chord maps by onset while preserving table order");}
        {
            vg::Score chord{{0,1,60,"bass",50},{.02,1,64,"lead",120},{.04,.8,67,"high",90},{1.1,.5,62,"next",80}};
            auto groups=vg::scoreChordGroups(chord);check(groups.size()==2&&groups[0].size()==3,"Staggered overlapping notes form a chord without merging the next phrase");
            auto velocity=chord;vg::prepareScoreChords(velocity,1);check(velocity[0].syllable=="lead"&&velocity[2].syllable=="lead"&&velocity[1].gain==1&&velocity[0].gain<velocity[1].gain&&velocity[3].syllable=="next","Velocity leader shares its word and stays louder than chord harmonies");
            vg::Score tied{{0,1,67,"high",120},{0,1,60,"low",40},{0,1,64,"middle",120}};check(vg::chordLeader(tied,vg::scoreChordGroups(tied)[0],1)==2,"Equal velocity leaders resolve to the lower pitch independently of row order");
            auto bass=chord;vg::prepareScoreChords(bass,2);check(bass[1].syllable=="bass"&&bass[0].gain==1&&bass[1].gain<1&&chord[0].velocity==50,"Chord root leads even when quiet, without modifying the source MIDI");
            vg::Score inversion{{0,1,64,"third",110},{0,1,67,"fifth",90},{0,1,72,"root",70}};vg::prepareScoreChords(inversion,2);check(inversion[0].syllable=="root"&&inversion[2].gain==1,"Root mode recognises an inverted major chord instead of choosing its bass");
            check(vg::assignScoreSyllables(chord,{"Hello","again"},1)&&chord[2].syllable=="Hello"&&chord[3].syllable=="again","Lyrics map to overlapping chord groups");
            vg::Score chain{{0,.5,60,"a"},{.4,.5,64,"b"},{.8,.5,67,"c"}};check(vg::scoreChordGroups(chain).size()==2,"Overlap grouping avoids transitive legato chains");
            vg::Score parsed;check(vg::parseScore(vg::scoreText(chord),parsed).isEmpty()&&parsed[0].velocity==50&&vg::parseScore("0\t1\t60\tla\t128",parsed).isNotEmpty(),"Velocity round trips and invalid velocity is rejected");
        }
        for(auto bad:{"abc\t1\t62\tla","0\tnan\t62\tla","0\t1\t62.5\tla","0\t301\t62\tla","0\t1\t62\t"})check(vg::parseScore(bad,notes).isNotEmpty(),"Malformed or unbounded score is rejected");
        auto path=juce::File::getSpecialLocation(juce::File::tempDirectory).getNonexistentChildFile("VoxGarden score test",".mid");juce::MidiFile midi;midi.setTicksPerQuarterNote(480);juce::MidiMessageSequence tempo,melody;
        auto event=juce::MidiMessage::tempoMetaEvent(500000);event.setTimeStamp(0);tempo.addEvent(event);event=juce::MidiMessage::tempoMetaEvent(1000000);event.setTimeStamp(480);tempo.addEvent(event);midi.addTrack(tempo);
        for(int i=0;i<2;++i){event=juce::MidiMessage::noteOn(1,62-i*2,(juce::uint8)100);event.setTimeStamp(i*480);melody.addEvent(event);event=juce::MidiMessage::noteOff(1,62-i*2);event.setTimeStamp((i+1)*480);melody.addEvent(event);}midi.addTrack(melody);
        {auto stream=path.createOutputStream();midi.writeTo(*stream);}std::vector<vg::ScoreTrack> tracks;auto error=vg::readMidiScore(path,tracks);check(error.isEmpty()&&tracks.size()==1&&tracks[0].notes.size()==2&&tracks[0].notes[0].velocity==100&&std::abs(tracks[0].notes[0].duration-.5)<1e-6&&std::abs(tracks[0].notes[1].duration-1)<1e-6,"MIDI import applies cross-track tempo changes and note lengths");path.deleteFile();
        {
            juce::MidiFile file;file.setTicksPerQuarterNote(480);juce::MidiMessageSequence events;
            for(auto pair:std::initializer_list<std::pair<int,bool>>{{0,true},{480,true},{720,false},{960,false}}){auto message=pair.second?juce::MidiMessage::noteOn(1,60,(juce::uint8)100):juce::MidiMessage::noteOff(1,60);message.setTimeStamp(pair.first);events.addEvent(message);}file.addTrack(events);
            auto temporary=juce::File::getSpecialLocation(juce::File::tempDirectory).getNonexistentChildFile("VoxGarden overlaps",".mid");{auto stream=temporary.createOutputStream();file.writeTo(*stream);}std::vector<vg::ScoreTrack> result;auto error=vg::readMidiScore(temporary,result);check(error.isEmpty()&&result.size()==1&&result[0].notes.size()==2&&std::abs(result[0].notes[0].duration-.75)<1e-6&&std::abs(result[0].notes[1].start-.5)<1e-6&&std::abs(result[0].notes[1].duration-.5)<1e-6,"MIDI repeated-pitch overlaps retain their original note-off times");temporary.deleteFile();
        }
        {ScoreGrid grid;grid.setText("0\t0.5\t62\tla\n0.75\t0.5\t60\tla",false);vg::Score roundTrip;check(vg::parseScore(grid.getText(),roundTrip).isEmpty()&&roundTrip.size()==2&&roundTrip[0].midi==62&&roundTrip[1].midi==60,"Editable note names retain the correct MIDI octave");}
        {
            auto file=juce::File::getSpecialLocation(juce::File::tempDirectory).getNonexistentChildFile("VoxGarden melody test",".wav");juce::AudioBuffer<float> audio(1,48000*2);audio.clear();for(int n=0;n<audio.getNumSamples();++n){double seconds=n/48000.;if(seconds<.7||(seconds>=1&&seconds<1.7)){double hz=seconds<.7?293.6647679:261.6255653;audio.setSample(0,n,.25f*std::sin(juce::MathConstants<double>::twoPi*hz*n/48000));}}
            {juce::WavAudioFormat wav;auto stream=file.createOutputStream();std::unique_ptr<juce::AudioFormatWriter> writer(wav.createWriterFor(stream.release(),48000,1,24,{},0));writer->writeFromAudioSampleBuffer(audio,0,audio.getNumSamples());}
            vg::Score sketch;auto result=vg::transcribeScore(file,sketch,[]{return false;});check(result.isEmpty()&&sketch.size()==2&&sketch[0].midi==62&&sketch[1].midi==60&&sketch[1].start>sketch[0].start+sketch[0].duration,"Audio pitch sketch detects isolated D4/C4 and retains a rest");file.deleteFile();
            auto log=file.withFileExtension(".log");auto start=juce::Time::getMillisecondCounterHiRes();auto code=vg::runSingingProcess({"/bin/sh","-c","sleep 30"},log,[&]{return juce::Time::getMillisecondCounterHiRes()-start>150;},[]{});check(code==-2&&juce::Time::getMillisecondCounterHiRes()-start<2000,"Cancelling setup/render stops the child process group promptly");log.deleteFile();
        }
        {
            ScoreGrid grid;grid.setSize(880,220);grid.setText("0\t1\t62\tfirst\n1\t1\t60\tmiddle\n2\t1\t69\tlast",false);grid.createComponentSnapshot(grid.getLocalBounds());
            auto click=[&](const juce::String& text,int modifiers){std::function<juce::Label*(juce::Component&)> find=[&](juce::Component& c)->juce::Label*{if(auto* label=dynamic_cast<juce::Label*>(&c);label&&label->getText()==text)return label;for(auto* child:c.getChildren())if(auto* label=find(*child))return label;return nullptr;};auto* cell=find(grid);if(!cell)return false;auto now=juce::Time::getCurrentTime();auto pos=cell->getLocalBounds().getCentre().toFloat();juce::MouseEvent event(juce::Desktop::getInstance().getMainMouseSource(),pos,juce::ModifierKeys(modifiers|juce::ModifierKeys::leftButtonModifier),1,0,0,0,0,cell,cell,now,pos,now,1,false);cell->mouseDown(event);return !cell->isBeingEdited();};
            bool selected=click("D4",0)&&click("A4",juce::ModifierKeys::commandModifier);grid.deleteNote();vg::Score remaining;check(selected&&vg::parseScore(grid.getText(),remaining).isEmpty()&&remaining.size()==1&&remaining[0].midi==60,"Cell clicks select rows; command-click and Delete remove precisely the selected notes");
        }
        {
            auto file=juce::File::getSpecialLocation(juce::File::tempDirectory).getNonexistentChildFile("VoxGarden dry transfer",".wav");file.replaceWithText("dry source unchanged");
            DryVocalButton button;button.setSize(235,32);button.setFile(file);int drags=0,clicks=0;bool copy=false,source=false;juce::String payload;
            button.beginDrag=[&](const auto& files,bool move,auto* component){++drags;copy=!move;source=component==&button;payload=files[0];return true;};button.onClick=[&]{++clicks;};
            auto event=[&](juce::Point<float> position){auto now=juce::Time::getCurrentTime();return juce::MouseEvent(juce::Desktop::getInstance().getMainMouseSource(),position,juce::ModifierKeys(juce::ModifierKeys::leftButtonModifier),1,0,0,0,0,&button,&button,now,{2,2},now,1,true);};
            button.mouseDown(event({2,2}));button.mouseDrag(event({4,2}));check(drags==0,"Dry transfer waits for a deliberate drag");button.mouseDrag(event({20,2}));button.mouseDrag(event({30,2}));button.mouseUp(event({30,2}));
            check(drags==1&&copy&&source&&payload==file.getFullPathName()&&clicks==0&&file.loadFileAsString()=="dry source unchanged","Dry transfer sends the original WAV as one native copy drag without a click or render");
            button.setFile({});button.mouseDown(event({2,2}));button.mouseDrag(event({20,2}));check(!button.isEnabled()&&drags==1,"Dry transfer is disabled without an available dry take");file.deleteFile();
        }
        VoxGardenProcessor p;p.themeAccent=3;{const juce::ScopedLock lock(p.scoreLock);p.scoreDraft=juce::String::fromUTF8("0\t1\t60\tनमस्ते");p.scoreLanguage=1;p.scoreChordMode=2;}juce::MemoryBlock state;p.getStateInformation(state);p.themeAccent=0;p.scoreDraft={};p.setStateInformation(state.getData(),(int)state.getSize());check(p.themeAccent==3&&p.scoreLanguage==1&&p.scoreChordMode==2&&p.scoreDraft.contains(juce::String::fromUTF8("नमस्ते")),"Host state restores accent, multilingual score and lyric language");
    }
    {
        auto folder=juce::File::getSpecialLocation(juce::File::tempDirectory).getNonexistentChildFile("VoxGarden appearance","",false);folder.createDirectory();auto file=folder.getChildFile("Appearance.json");
        VoxGardenProcessor first(file);check(first.setAppearance(1,false),"User theme preference saves atomically");VoxGardenProcessor second(file);check(second.themeAccent==1&&!second.darkMode,"Fresh instance inherits Rose and light mode from disk");
        second.themeAccent=2;second.darkMode=true;juce::MemoryBlock old;second.getStateInformation(old);first.setStateInformation(old.getData(),(int)old.getSize());VoxGardenProcessor third(file);check(first.themeAccent==2&&first.darkMode&&third.themeAccent==1&&!third.darkMode,"Project appearance restores without overwriting the global default");
        file.replaceWithText("broken json");auto fallback=vg::loadAppearance(file);check(fallback.accent==0&&fallback.dark,"Corrupt appearance preferences safely use defaults");folder.deleteRecursively();
        VoxGardenProcessor ensemble;ensemble.applyPreset(62);auto sound=ensemble.settings();check(sound.count==24&&sound.space>.7f&&sound.weird==0&&sound.voice[23].delay!=sound.voice[0].delay,"Abbey Ensemble 24 selects a varied, spacious 24-voice choir");
        ensemble.setValue("v24_mute",1);juce::MemoryBlock full;ensemble.getStateInformation(full);ensemble.setChoirSize(3);ensemble.setStateInformation(full.getData(),(int)full.getSize());check(ensemble.settings().count==24&&ensemble.settings().voice[23].mute,"All 24 voices and audition controls restore through host state");
        check(ensemble.state.getParameter("voiceCount")->getParameterIndex()==54&&ensemble.state.getParameter("raga")->getParameterIndex()==74&&std::abs(ensemble.state.getParameter("voiceCount")->convertTo0to1(3)-2.f/7)<1e-6,"Original parameter indices and 1..8 choir automation normalisation remain unchanged");
        auto legacy=ensemble.state.copyState();for(int i=legacy.getNumChildren();--i>=0;){auto id=legacy.getChild(i).getProperty("id").toString();if(id=="extraVoices"||(id.startsWith("v")&&id.substring(1).getIntValue()>8))legacy.removeChild(i,nullptr);}juce::MemoryBlock prior;juce::AudioProcessor::copyXmlToBinary(*legacy.createXml(),prior);ensemble.setStateInformation(prior.getData(),(int)prior.getSize());check(ensemble.settings().count==8&&!ensemble.settings().voice[23].mute,"Loading a 2.0 state clears additional voices and stale extra controls");
        ensemble.applyPreset(63);check(ensemble.settings().count==16,"Chamber Ensemble provides sixteen voices");ensemble.applyPreset(64);check(ensemble.settings().count==24&&ensemble.settings().aiCast==1,"Velvet Abbey provides a 24-voice female reference cast");ensemble.applyPreset(0);check(ensemble.settings().count<=8,"Original factory sounds return to their original choir sizes");
    }
    {
        auto folder=juce::File::getSpecialLocation(juce::File::tempDirectory).getNonexistentChildFile("VoxGarden 24 stems","",false);folder.createDirectory();auto input=folder.getChildFile("Input.wav");
        juce::WavAudioFormat wav;juce::AudioBuffer<float> tone(1,24000);for(int i=0;i<24000;++i)tone.setSample(0,i,.12f*std::sin(juce::MathConstants<double>::twoPi*220*i/48000));{auto stream=input.createOutputStream();std::unique_ptr<juce::AudioFormatWriter> writer(wav.createWriterFor(stream.release(),48000,1,24,{},0));writer->writeFromAudioSampleBuffer(tone,0,tone.getNumSamples());}
        VoxGardenProcessor choir;choir.applyPreset(62);choir.setValue("v24_solo",1);std::atomic<float> progress{0};auto output=folder.getChildFile("Printed");auto error=renderVocal(input,output,choir.settings(),progress,[]{return false;});bool correct=error.startsWith("Saved mix + 24 stems")&&output.getNumberOfChildFiles(juce::File::findFiles,"*.wav")==25;
        juce::AudioFormatManager formats;formats.registerBasicFormats();for(int v:{1,24}){std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(output.getChildFile("Voice "+juce::String(v)+".wav")));if(!reader){correct=false;continue;}juce::AudioBuffer<float> audio(2,(int)reader->lengthInSamples);reader->read(&audio,0,audio.getNumSamples(),0,true,true);correct &= reader->lengthInSamples==216000&&(v==24?audio.getMagnitude(0,audio.getNumSamples())>.001f:audio.getMagnitude(0,audio.getNumSamples())<1e-7f);}
        check(correct,"24-voice print writes 24 aligned stereo stems with cross-bank solo respected");folder.deleteRecursively();
    }
    for(int root=0;root<12;++root)for(int scale=0;scale<(int)vg::intervals.size();++scale)for(int note=36;note<85;++note)for(int degree=-14;degree<=14;++degree)
        if(!vg::inScale(vg::degreeNote((float)note+.31f,degree,root,scale),root,scale)){check(false,"Scale membership");return 1;}
    check(true,"Every harmony target stays in every supported key/scale");
    check(vg::degreeNote(60,2,0,0)==64 && vg::degreeNote(60,2,0,1)==63 && vg::degreeNote(60,-7,0,0)==48,"Major/minor thirds and negative degrees");
    vg::PitchTracker tracker;tracker.prepare(48000);for(int i=0;i<48000;++i)tracker.push(.2f*std::sin(juce::MathConstants<double>::twoPi*220*i/48000));
    check(tracker.voiced && std::abs(tracker.note-57)<.1f,"YIN detects A3 within 10 cents");
    VoxGardenProcessor processor;processor.setRateAndBufferSizeDetails(48000,256);processor.prepareToPlay(48000,256);processor.setValue("root",9);processor.setValue("scale",1);processor.setValue("v3_formant",3.4f);
    juce::MemoryBlock state;processor.getStateInformation(state);processor.setValue("root",0);processor.setStateInformation(state.getData(),(int)state.getSize());
    check(processor.settings().root==9 && processor.settings().scale==1 && std::abs(processor.settings().voice[2].formant-3.4f)<.01f,"Full state restores discrete and continuous controls");
    processor.applyPreset(2);check(processor.settings().root==9 && processor.settings().scale==1,"Starting sounds preserve the chosen key and scale");
    processor.applyPreset(0);processor.setValue("voiceCount",8);processor.setValue("space",0);processor.setValue("human",0);processor.setValue("detune",0);processor.setValue("vibrato",0);
    juce::AudioBuffer<float> b(2,256);juce::MidiBuffer midi;double energy=0,peak=0;bool finite=true;auto start=std::chrono::steady_clock::now();
    for(int block=0;block<600;++block){for(int i=0;i<256;++i){float x=.15f*std::sin(juce::MathConstants<double>::twoPi*220*(block*256+i)/48000);b.setSample(0,i,x);b.setSample(1,i,x);}processor.processBlock(b,midi);for(int ch=0;ch<2;++ch)for(int i=0;i<256;++i){float x=b.getSample(ch,i);finite&=std::isfinite(x);energy+=x*x;peak=std::max(peak,(double)std::abs(x));}}
    double seconds=std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();
    check(finite && energy>1 && peak<=1,"Eight-voice DSP produces finite, bounded audio");std::cout<<"DSP: "<<seconds<<" seconds for 3.2 audio seconds; "<<100*seconds/3.2<<"% of one core; latency "<<processor.getLatencySamples()<<" samples\n";
    {
        processor.applyPreset(62);processor.setValue("space",0);juce::AudioBuffer<float> stems(vg::voices*2,256);bool stable=true;double power=0;auto clock=std::chrono::steady_clock::now();
        for(int block=0;block<600;++block){for(int i=0;i<256;++i)for(int c=0;c<2;++c)b.setSample(c,i,.1f*std::sin(juce::MathConstants<double>::twoPi*220*(block*256+i)/48000));stems.clear();processor.engine.process(b.getWritePointer(0),b.getWritePointer(1),256,processor.settings(),stems.getArrayOfWritePointers());for(int c=0;c<2;++c)for(int i=0;i<256;++i)stable&=std::isfinite(b.getSample(c,i))&&std::abs(b.getSample(c,i))<=1;power+=stems.getMagnitude(46,0,256);}
        double elapsed=std::chrono::duration<double>(std::chrono::steady_clock::now()-clock).count();check(stable&&power>.1,"Twenty-four voices render bounded audio including the final singer");std::cout<<"24-voice DSP: "<<elapsed<<" seconds for 3.2 audio seconds; "<<100*elapsed/3.2<<"% of one core\n";
        processor.setValue("v24_solo",1);auto sound=processor.settings();check(vg::voiceAudible(sound,23)&&!vg::voiceAudible(sound,0),"Solo on the third bank isolates voice 24 across the whole choir");processor.applyPreset(0);
    }
    processor.reset();b.clear();processor.processBlock(b,midi);check(b.getMagnitude(0,256)<1e-6f,"Reset removes buffered audio");
    {
        vg::Engine isolated;isolated.prepare(48000);vg::Settings settings;for(auto& v:settings.voice)v.level=0;
        settings.voice[0].level=.5f;settings.voice[0].degree=2;settings.root=9;settings.scale=1;
        settings.correction=1;settings.detune=settings.vibrato=settings.human=settings.space=0;settings.glide=5;settings.gain=1;
        juce::AudioBuffer<float> tone(2,48000*2);for(int i=0;i<tone.getNumSamples();++i)for(int c=0;c<2;++c)tone.setSample(c,i,.2f*std::sin(juce::MathConstants<double>::twoPi*220*i/48000));
        isolated.process(tone.getWritePointer(0),tone.getWritePointer(1),tone.getNumSamples(),settings);
        double best=0,bestHz=0;
        for(double hz=255;hz<270;hz+=.1){double real=0,imag=0;for(int i=48000;i<96000;++i){double phase=juce::MathConstants<double>::twoPi*hz*i/48000;real+=tone.getSample(0,i)*std::cos(phase);imag+=tone.getSample(0,i)*std::sin(phase);}double mag=real*real+imag*imag;if(mag>best){best=mag;bestHz=hz;}}
        std::cout<<"Detected MIDI pitch: "<<isolated.pitch.note<<" confidence "<<isolated.pitch.confidence<<"\n";
        std::cout<<"A minor third output: "<<bestHz<<" Hz (C4 expected 261.626)\n";
        check(std::abs(1200*std::log2(bestHz/261.625565))<10,"Rendered A-minor harmony is C4 within 10 cents");
        auto temp=juce::File::getSpecialLocation(juce::File::tempDirectory).getNonexistentChildFile("VoxGarden-check","");temp.createDirectory();auto input=temp.getChildFile("input.wav");
        juce::WavAudioFormat wav;auto stream=input.createOutputStream();std::unique_ptr<juce::AudioFormatWriter> writer(wav.createWriterFor(stream.release(),48000,2,24,{},0));writer->writeFromAudioSampleBuffer(tone,0,48000);writer.reset();
        std::atomic<float> progress{0};auto output=temp.getChildFile("render");auto result=renderVocal(input,output,settings,progress,[]{return false;});
        juce::AudioFormatManager formats;formats.registerBasicFormats();int good=0;
        for(auto f:output.findChildFiles(juce::File::findFiles,false,"*.wav")){std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(f));if(reader && reader->numChannels==2 && reader->lengthInSamples==240000 && reader->sampleRate==48000)++good;}
        check(good==9 && progress.load()==1,"File renderer writes mix + eight aligned stereo WAVs with four-second tails");
        temp.deleteRecursively();
    }
    {
        VoxGardenProcessor chord;chord.setRateAndBufferSizeDetails(48000,256);chord.prepareToPlay(48000,256);chord.setValue("mode",1);chord.setValue("space",0);
        juce::AudioBuffer<float> io(2,256);juce::MidiBuffer events;double before=0,during=0,after=0;
        for(int k=0;k<320;++k){events.clear();if(k==50)events.addEvent(juce::MidiMessage::noteOn(1,64,(juce::uint8)100),73);if(k==200)events.addEvent(juce::MidiMessage::allNotesOff(1),127);
            for(int i=0;i<256;++i){float x=.15f*std::sin(juce::MathConstants<double>::twoPi*220*(k*256+i)/48000);io.setSample(0,i,x);io.setSample(1,i,x);}chord.processBlock(io,events);
            double e=io.getRMSLevel(0,0,256);if(k<45)before+=e;if(k>140&&k<190)during+=e;if(k>300)after+=e;
        }
        check(before<1e-6 && during>.1 && after<1e-6,"MIDI mode waits for notes, voices a chord and releases on All Notes Off");
    }
    {
        processor.setValue("voiceCount",3);juce::MemoryBlock saved;processor.getStateInformation(saved);processor.setValue("voiceCount",8);processor.setStateInformation(saved.getData(),(int)saved.getSize());
        check(processor.settings().count==3,"Choir size restores from plugin state");
        auto legacy=processor.state.copyState();for(int i=legacy.getNumChildren();--i>=0;)if(legacy.getChild(i).getProperty("id").toString()=="voiceCount" || legacy.getChild(i).getProperty("id").toString()=="aiPrint" || legacy.getChild(i).getProperty("id").toString()=="aiBlend")legacy.removeChild(i,nullptr);
        juce::MemoryBlock old;auto xml=legacy.createXml();juce::AudioProcessor::copyXmlToBinary(*xml,old);processor.setStateInformation(old.getData(),(int)old.getSize());
        check(processor.settings().count==8 && !processor.settings().aiPrint && processor.settings().aiBlend==1,"Legacy states retain all eight slots without unexpectedly enabling AI");
        bool natural=true;for(int preset=5;preset<12;++preset){processor.applyPreset(preset);auto s=processor.settings();natural&=s.weird==0 && s.root==9 && s.scale==1 && s.formantLock && s.count>=2 && s.count<=8;}
        check(natural,"Seven new natural presets preserve song key and avoid Mutation");
        vg::Engine solo;solo.prepare(48000);vg::Settings settings;settings.count=1;settings.space=settings.detune=settings.vibrato=settings.human=0;
        juce::AudioBuffer<float> io(2,24000),stems(vg::voices*2,24000);stems.clear();for(int i=0;i<24000;++i)for(int c=0;c<2;++c)io.setSample(c,i,.1f*std::sin(juce::MathConstants<double>::twoPi*220*i/48000));
        solo.process(io.getWritePointer(0),io.getWritePointer(1),24000,settings,stems.getArrayOfWritePointers());bool muted=true;for(int c=2;c<vg::voices*2;++c)muted&=stems.getMagnitude(c,0,24000)<1e-7f;
        check(muted && stems.getMagnitude(0,0,24000)>.001f,"Choir size one silences the other seven parts");
    }
    {
        auto folder=juce::File::getSpecialLocation(juce::File::tempDirectory).getNonexistentChildFile("VoxGarden-capture-check","");folder.createDirectory();auto source=folder.getChildFile("Input Vocal.wav");
        VoxGardenProcessor channel;channel.setRateAndBufferSizeDetails(48000,256);channel.prepareToPlay(48000,256);channel.setValue("mix",1);channel.setValue("voiceCount",3);channel.setValue("weird",.8f);
        auto waitFor=[&](TrackCapture::Phase phase){for(int i=0;i<1000;++i){if(channel.capture.getPhase()==phase)return true;juce::Thread::sleep(2);}return false;};
        check(channel.capture.begin(source)&&waitFor(TrackCapture::recording),"Channel capture arms its background disk writer");
        juce::AudioBuffer<float> audio(2,256);juce::MidiBuffer events;constexpr int blocks=80;
        for(int k=0;k<blocks;++k){for(int i=0;i<256;++i){int at=k*256+i;audio.setSample(0,i,.11f*std::sin(juce::MathConstants<double>::twoPi*220*at/48000));audio.setSample(1,i,.07f*std::cos(juce::MathConstants<double>::twoPi*330*at/48000));}channel.processBlock(audio,events);}
        channel.capture.requestStop();check(waitFor(TrackCapture::complete),"Channel capture drains and finalizes on Stop");
        juce::AudioFormatManager formats;formats.registerBasicFormats();std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(source));
        bool exact=reader && reader->lengthInSamples==blocks*256;
        if(exact){juce::AudioBuffer<float> captured(2,blocks*256);reader->read(&captured,0,blocks*256,0,true,true);float worst=0;for(int at=0;at<blocks*256;++at){float l=.11f*std::sin(juce::MathConstants<double>::twoPi*220*at/48000),r=.07f*std::cos(juce::MathConstants<double>::twoPi*330*at/48000);worst=std::max(worst,std::abs(captured.getSample(0,at)-l));worst=std::max(worst,std::abs(captured.getSample(1,at)-r));}exact=worst<1e-6f;}
        check(exact,"Captured stereo samples match the dry track input, not the processed choir");reader.reset();
        std::atomic<float> progress{0};auto destination=folder.getChildFile("printed");renderVocal(source,destination,channel.settings(),progress,[]{return false;});
        check(destination.findChildFiles(juce::File::findFiles,false,"*.wav").size()==4,"Captured vocal renders a mix and exactly the three requested voice stems");
        auto capture2=folder.getChildFile("second.wav");check(channel.capture.begin(capture2)&&waitFor(TrackCapture::recording),"Capture can start a second independent take");audio.clear();channel.processBlock(audio,events);channel.prepareToPlay(44100,512);
        std::unique_ptr<juce::AudioFormatReader> prior(formats.createReaderFor(capture2));check(prior && prior->lengthInSamples==256,"Audio reconfiguration safely finalizes the current captured take");prior.reset();channel.releaseResources();folder.deleteRecursively();
    }
    {
        processor.setValue("aiPrint",1);processor.setValue("aiBlend",.7f);juce::MemoryBlock data;processor.getStateInformation(data);processor.setValue("aiPrint",0);processor.setStateInformation(data.getData(),(int)data.getSize());
        check(processor.settings().aiPrint && std::abs(processor.settings().aiBlend-.7f)<.001f,"AI print and blend controls restore from state");processor.setValue("aiPrint",0);
        auto folder=juce::File::getSpecialLocation(juce::File::tempDirectory).getNonexistentChildFile("VoxGarden-AI-mix-check","");folder.createDirectory();
        juce::WavAudioFormat wav;juce::AudioBuffer<float> audio(1,48000);std::vector<juce::File> sources;
        for(int k=0;k<2;++k){for(int i=0;i<48000;++i)audio.setSample(0,i,.1f*std::sin(juce::MathConstants<double>::twoPi*(k==0?220:440)*i/48000));auto f=folder.getChildFile(k==0?"input.wav":"ai.wav");auto stream=f.createOutputStream();std::unique_ptr<juce::AudioFormatWriter> w(wav.createWriterFor(stream.release(),48000,1,24,{},0));w->writeFromAudioSampleBuffer(audio,0,48000);if(k==1)sources.push_back(f);}
        vg::Settings settings;settings.count=1;settings.aiBlend=1;settings.voice[0].degree=4;settings.detune=settings.vibrato=settings.human=settings.space=0;
        std::atomic<float> progress{0};auto destination=folder.getChildFile("printed");auto message=renderVocal(folder.getChildFile("input.wav"),destination,settings,progress,[]{return false;},sources);
        juce::AudioFormatManager formats;formats.registerBasicFormats();std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(destination.getChildFile("Voice 1.wav")));bool ok=reader && reader->lengthInSamples==240000;
        if(ok){juce::AudioBuffer<float> output(2,48000);reader->read(&output,0,48000,0,true,true);double at440=0,at660=0;for(int i=16000;i<48000;++i){at440+=output.getSample(0,i)*std::sin(juce::MathConstants<double>::twoPi*440*i/48000);at660+=output.getSample(0,i)*std::sin(juce::MathConstants<double>::twoPi*660*i/48000);}ok=std::abs(at440)>5*std::abs(at660)&&std::abs(at440)>1;}
        check(ok,"Native AI mixing preserves singer pitch without a second harmony transposition");reader.reset();folder.deleteRecursively();
    }
    {
        VoxGardenProcessor monitor;monitor.prepareToPlay(48000,256);monitor.setValue("mix",1);monitor.setValue("gain",.2f);monitor.printInProgress=true;
        juce::AudioBuffer<float> b(2,256);juce::MidiBuffer events;float error=0;
        for(int block=0;block<80;++block){for(int i=0;i<256;++i)for(int ch=0;ch<2;++ch)b.setSample(ch,i,.1f*std::sin((block*256+i)*.031+ch));monitor.processBlock(b,events);
            for(int i=0;i<256;++i)for(int ch=0;ch<2;++ch){int at=block*256+i-monitor.getLatencySamples();float expected=at<0?0:.1f*std::sin(at*.031+ch);error=std::max(error,std::abs(expected-b.getSample(ch,i)));}}
        check(error<1e-6f,"Printing monitors unity dry stereo at the declared latency, independent of wet mix/output gain");
        monitor.printInProgress=false;monitor.releaseResources();
        processor.setValue("root",9);processor.setValue("scale",1);bool presetsOk=true;
        for(int i=0;i<38;++i){processor.applyPreset(i);auto v=processor.settings();presetsOk &= v.root==9 && v.scale==1 && v.count>=1 && v.count<=8;}
        check(presetsOk,"All 38 presets preserve song key/scale and valid choir sizes");
        processor.darkMode=false;juce::MemoryBlock data;processor.getStateInformation(data);processor.darkMode=true;processor.setStateInformation(data.getData(),(int)data.getSize());
        check(!processor.darkMode.load(),"Theme preference restores from host state without adding sound parameters");processor.darkMode=true;
    }
    {
        VoxGardenProcessor audition;audition.applyPreset(6);audition.setValue("voiceCount",3);audition.setValue("v1_mute",1);audition.setValue("v2_solo",1);
        auto v=audition.settings();check(!vg::voiceAudible(v,0)&&vg::voiceAudible(v,1)&&!vg::voiceAudible(v,2),"Solo isolates an active choir voice and mute overrides it");
        audition.setValue("v3_solo",1);v=audition.settings();check(vg::voiceAudible(v,1)&&vg::voiceAudible(v,2),"Multiple solos combine");
        audition.setValue("v2_mute",1);v=audition.settings();check(!vg::voiceAudible(v,1)&&vg::voiceAudible(v,2),"Muted solo remains silent while another solo plays");
        audition.setValue("voiceCount",1);v=audition.settings();v.voice[0].mute=false;check(vg::voiceAudible(v,0),"Solos outside choir size do not silence active singers");
        audition.setValue("voiceCount",3);float level=audition.settings().voice[0].level;
        juce::MemoryBlock saved;audition.getStateInformation(saved);audition.applyPreset(0);audition.setStateInformation(saved.getData(),(int)saved.getSize());v=audition.settings();
        check(v.voice[0].mute&&v.voice[1].mute&&v.voice[1].solo&&v.voice[2].solo&&v.voice[0].level==level,"Mute/solo restore with project state without changing stored levels");
        auto legacy=audition.state.copyState();for(int i=legacy.getNumChildren();--i>=0;){auto id=legacy.getChild(i).getProperty("id").toString();if(id.endsWith("_mute")||id.endsWith("_solo"))legacy.removeChild(i,nullptr);}
        juce::MemoryBlock old;auto xml=legacy.createXml();juce::AudioProcessor::copyXmlToBinary(*xml,old);audition.setStateInformation(old.getData(),(int)old.getSize());bool cleared=true;for(auto voice:audition.settings().voice)cleared &= !voice.mute&&!voice.solo;
        check(cleared,"Older project states clear mute/solo even after an auditioned state");
        check(audition.state.getParameter("v1_mute")->getParameterIndex()==57&&audition.state.getParameter("aiBlend")->getParameterIndex()==56,"Mute/solo parameters append after existing sound controls");
        auto folder=juce::File::getSpecialLocation(juce::File::tempDirectory).getNonexistentChildFile("VoxGarden-audition-check","");folder.createDirectory();auto input=folder.getChildFile("input.wav");
        juce::WavAudioFormat wav;juce::AudioBuffer<float> tone(1,24000);for(int i=0;i<24000;++i)tone.setSample(0,i,.12f*std::sin(juce::MathConstants<double>::twoPi*220*i/48000));
        {auto stream=input.createOutputStream();std::unique_ptr<juce::AudioFormatWriter> w(wav.createWriterFor(stream.release(),48000,1,24,{},0));w->writeFromAudioSampleBuffer(tone,0,tone.getNumSamples());}
        vg::Settings settings;settings.count=3;settings.space=settings.human=settings.detune=settings.vibrato=0;settings.voice[0].mute=true;settings.voice[1].solo=true;
        auto ai=vg::aiSettings(settings);auto* levels=ai.getProperty("voices",{}).getArray();
        check(levels&&(*levels)[0].getProperty("level",1)==juce::var(0.f)&& (double)(*levels)[1].getProperty("level",0)>.1 && (double)(*levels)[2].getProperty("level",1)==0,"AI job skips muted and non-solo singers while preserving slot identities");
        juce::AudioFormatManager formats;formats.registerBasicFormats();bool isolated=true;
        for(int mode=0;mode<2;++mode){auto destination=folder.getChildFile(mode==0?"DSP":"AI");std::atomic<float> progress{0};std::vector<juce::File> aiFiles;if(mode==1)aiFiles={input,input,input};renderVocal(input,destination,settings,progress,[]{return false;},aiFiles);
            for(int slot=1;slot<=3;++slot){std::unique_ptr<juce::AudioFormatReader> r(formats.createReaderFor(destination.getChildFile("Voice "+juce::String(slot)+".wav")));if(!r){isolated=false;continue;}juce::AudioBuffer<float> out(2,(int)r->lengthInSamples);r->read(&out,0,out.getNumSamples(),0,true,true);auto peak=out.getMagnitude(0,out.getNumSamples());isolated &= (slot==2?peak>.001f:peak<1e-7f)&&r->lengthInSamples==216000;}}
        check(isolated,"DSP and AI printed stems isolate soloed voice and retain aligned silent excluded slots");folder.deleteRecursively();
    }
    {
        VoxGardenProcessor cast;cast.setValue("root",9);cast.setValue("scale",1);cast.applyPreset(28);auto female=cast.settings();
        cast.applyPreset(29);auto male=cast.settings();
        check(female.aiCast==1&&female.voice[0].formant>0&&male.aiCast==2&&male.voice[0].formant<0&&male.voice[0].degree<female.voice[0].degree,"Opposite-register presets change formant, harmony and neural reference cast");
        cast.applyPreset(34);check(cast.settings().aiCast==3,"Mixed choir preset selects alternating female/male references");
        juce::MemoryBlock state;cast.getStateInformation(state);cast.setValue("aiCast",1);cast.setStateInformation(state.getData(),(int)state.getSize());
        check(cast.settings().aiCast==3&&(int)vg::aiSettings(cast.settings()).getProperty("aiCast",0)==3,"AI cast survives project state and reaches the render job");
        auto legacy=cast.state.copyState();for(int i=legacy.getNumChildren();--i>=0;)if(legacy.getChild(i).getProperty("id").toString()=="aiCast")legacy.removeChild(i,nullptr);
        juce::MemoryBlock old;auto xml=legacy.createXml();juce::AudioProcessor::copyXmlToBinary(*xml,old);cast.setStateInformation(old.getData(),(int)old.getSize());
        check(cast.settings().aiCast==0&&cast.state.getParameter("aiCast")->getParameterIndex()==73,"Older projects restore original reference cast; existing parameter order remains stable");
        cast.applyPreset(28);cast.applyPreset(7);check(cast.settings().aiCast==0,"Existing factory sounds restore their original reference cast");
    }
    {
        VoxGardenProcessor raga;raga.setValue("root",0);raga.setValue("scale",1);
        bool valid=true;for(int i=0;i<8;++i){raga.applyPreset(38+i);auto v=raga.settings();valid &= v.root==0&&v.raga==i+1&&v.scale==vg::ragaScales[(size_t)i]&&v.weird==0;
            for(const auto& voice:v.voice)valid &= vg::inScale(vg::degreeNote(60,voice.degree,v.root,v.scale),v.root,v.scale);}
        check(valid,"Eight raga presets retain Sa and constrain every voice to the researched note set");
        check(vg::degreeNote(60,3,0,9)==66&&vg::degreeNote(60,1,0,11)==61&&vg::degreeNote(60,1,0,12)==63,"Yaman sharp fourth, Bhairav flat second and Malkauns minor third map correctly");
        check(vg::ragaNoteNames(9,1)==std::vector<std::string>{"A","B","C#","D#","E","F#","G#"}&&vg::ragaNoteNames(0,5)==std::vector<std::string>{"C","Db","E","F","G","Ab","B"},"Western raga display uses consistent degree-based accidental spellings");
        raga.setValue("raga",1);raga.language=4;juce::MemoryBlock data;raga.getStateInformation(data);raga.setValue("raga",0);raga.language=0;raga.setStateInformation(data.getData(),(int)data.getSize());
        check(raga.settings().raga==1&&raga.settings().scale==9&&raga.language.load()==4,"Raga selection and per-instance language restore from project state");
        check((int)vg::aiSettings(raga.settings()).getProperty("scale",0)==9,"AI render job receives effective raga note set");
        auto old=raga.state.copyState();old.removeProperty("language",nullptr);for(int i=old.getNumChildren();--i>=0;)if(old.getChild(i).getProperty("id").toString()=="raga")old.removeChild(i,nullptr);
        auto xml=old.createXml();juce::MemoryBlock legacy;juce::AudioProcessor::copyXmlToBinary(*xml,legacy);raga.setStateInformation(legacy.getData(),(int)legacy.getSize());
        check(raga.settings().raga==0&&raga.settings().scale==1&&raga.language.load()==0,"Older projects retain their original Western scale and default to English");
        check(raga.state.getParameter("raga")->getParameterIndex()==74&&raga.state.getParameter("scale")->getNormalisableRange().end==8,"Raga parameter is appended without changing the original Western scale automation range");
        juce::ComboBox translated;translated.addItem(vg::tr(1,"Yaman"),1);translated.setSelectedId(1,juce::dontSendNotification);auto before=translated.getText();vg::localiseComponent(translated,1);
        check(translated.getText()==before,"Already-localised menu labels do not translate proper names twice");
        check(vg::voiceCountText(4,2)==juce::String::fromUTF8("2 голоса")&&vg::voiceCountText(4,5)==juce::String::fromUTF8("5 голосов"),"Russian voice counts use the correct plural forms");
        bool complete=true;for(const auto& row:vg::translations)for(int language=1;language<5;++language)complete &= juce::String::fromUTF8(row[(size_t)language]).isNotEmpty();
        check(complete,"Every translation catalog entry has Hindi, German, Dutch and Russian text");
        for(int language=1;language<5;++language)check(vg::tr(language,"Render vocal + stems")!="Render vocal + stems"&&vg::tr(language,"Singer 1/2 - synthesis part 1, step 3/20").contains("3/20"),"Translated controls and dynamic synthesis status preserve numbers");
    }
    {
        VoxGardenProcessor voices;voices.setValue("root",9);voices.setValue("scale",1);voices.setValue("aiPrint",1);voices.setValue("aiBlend",.7f);bool valid=true;
        for(int i=0;i<8;++i)for(int cast=1;cast<=2;++cast){voices.applyPreset((cast==1?46:54)+i);auto sound=voices.settings();int octave=(int)vg::intervals[(size_t)sound.scale].size();
            valid &= sound.raga==i+1&&sound.root==9&&sound.aiCast==cast&&sound.count==6&&sound.weird==0&&sound.formantLock&&sound.aiPrint&&std::abs(sound.aiBlend-.7f)<.001f;
            valid &= cast==1?sound.voice[0].formant>0&&sound.voice[0].degree==0:sound.voice[0].formant<0&&sound.voice[0].degree==-octave;
            for(const auto& voice:sound.voice)valid &= vg::inScale(vg::degreeNote(57,voice.degree,sound.root,sound.scale),sound.root,sound.scale);
            auto job=vg::aiSettings(sound);valid &= (int)job.getProperty("aiCast",0)==cast&&(int)job.getProperty("raga",0)==i+1;
        }
        check(valid,"All sixteen raga voice-cast variants preserve Sa, AI blend, scale membership and true raga octaves");
        voices.applyPreset(47);auto sound=voices.settings();check(sound.voice[2].degree==5&&vg::degreeNote(57,sound.voice[2].degree,9,sound.scale)==69,"Bhupali female-choir octave uses five scale steps, not seven");
        voices.applyPreset(59);sound=voices.settings();bool noPa=true;for(const auto& voice:sound.voice)noPa &= ((vg::degreeNote(57,voice.degree,9,sound.scale)-9)%12+12)%12!=7;
        check(noPa,"Malkauns male-choir preset never introduces the omitted Pa");
        juce::MemoryBlock data;voices.getStateInformation(data);voices.applyPreset(0);voices.setStateInformation(data.getData(),(int)data.getSize());
        check(voices.settings().raga==6&&voices.settings().aiCast==2,"Raga voice-cast sound restores through project state");
    }
    if(argc>1){juce::AudioPluginFormatManager manager;manager.addDefaultFormats();juce::OwnedArray<juce::PluginDescription> found;auto path=juce::String(argv[1]);for(auto* format:manager.getFormats())format->findAllTypesForFile(found,path);
        check(found.size()>0,"Built plugin scans through host API");
        if(found.size()){juce::String error;auto instance=manager.createPluginInstance(*found[0],48000,256,error);check(instance!=nullptr,"Built plugin instantiates through host API");if(!instance)std::cout<<"Host error: "<<error<<std::endl;if(instance){instance->prepareToPlay(48000,256);juce::AudioBuffer<float> io(2,256);io.clear();instance->processBlock(io,midi);std::cout<<"Host parameter count: "<<instance->getParameters().size()<<" (includes wrapper bypass)\n";check(instance->getParameters().size()==189,"Host exposes 188 sound parameters plus wrapper bypass");juce::MemoryBlock chunk;instance->getStateInformation(chunk);instance->setStateInformation(chunk.getData(),(int)chunk.getSize());check(chunk.getSize()>0,"Host state chunk round trip");instance->releaseResources();}}}
    if(argc>2) {
        processor.applyPreset(46);
        auto folder=juce::File(juce::String(argv[2]));folder.createDirectory();
        std::unique_ptr<juce::AudioProcessorEditor> editor(processor.createEditor());
        if(auto* menu=dynamic_cast<juce::ComboBox*>(editor->findChildWithID("presets")))menu->setSelectedId(47,juce::dontSendNotification);
        auto image=editor->createComponentSnapshot(editor->getLocalBounds(),true,1);
        juce::PNGImageFormat png;auto out=folder.getChildFile("VoxGarden-preview.png").createOutputStream();if(out){out->setPosition(0);out->truncate();png.writeImageToStream(image,*out);}
        check(image.isValid(),"Plugin editor renders to image");
        out.reset();editor.reset();
        for(int accent=1;accent<=3;++accent){processor.themeAccent=accent;for(bool dark:{false,true}){processor.darkMode=dark;editor.reset(processor.createEditor());auto snapshot=editor->createComponentSnapshot(editor->getLocalBounds(),true,1);auto stream=folder.getChildFile("Theme-"+juce::String(accent)+(dark?"-dark.png":"-light.png")).createOutputStream();if(stream)png.writeImageToStream(snapshot,*stream);editor.reset();}}
        processor.themeAccent=1;processor.darkMode=true;{ScoreComposer composer(processor,{});auto snapshot=composer.createComponentSnapshot(composer.getLocalBounds(),true,1);auto stream=folder.getChildFile("Notes-and-lyrics.png").createOutputStream();if(stream)png.writeImageToStream(snapshot,*stream);}processor.themeAccent=0;
        for(int locale=0;locale<5;++locale){processor.language=locale;processor.themeAccent=1;processor.darkMode=true;editor.reset(processor.createEditor());auto shot=editor->createComponentSnapshot(editor->getLocalBounds(),true,1);auto stream=folder.getChildFile("Preview-"+juce::String(locale)+"-rose.png").createOutputStream();if(stream)png.writeImageToStream(shot,*stream);stream.reset();editor.reset();ScoreComposer composer(processor,{});shot=composer.createComponentSnapshot(composer.getLocalBounds(),true,1);stream=folder.getChildFile("Preview-"+juce::String(locale)+"-score.png").createOutputStream();if(stream)png.writeImageToStream(shot,*stream);}
        processor.language=0;processor.applyPreset(62);processor.themeAccent=1;processor.darkMode=true;editor.reset(processor.createEditor());if(auto* bank=dynamic_cast<juce::ComboBox*>(editor->findChildWithID("voiceBank")))bank->setSelectedId(3,juce::sendNotificationSync);if(auto* menu=dynamic_cast<juce::ComboBox*>(editor->findChildWithID("presets")))menu->setSelectedId(63,juce::dontSendNotification);auto large=editor->createComponentSnapshot(editor->getLocalBounds(),true,1);auto largeOut=folder.getChildFile("Abbey-24-bank3.png").createOutputStream();if(largeOut)png.writeImageToStream(large,*largeOut);largeOut.reset();editor.reset();
        processor.language=0;processor.themeAccent=0;processor.darkMode=false;editor.reset(processor.createEditor());
        if(auto* menu=dynamic_cast<juce::ComboBox*>(editor->findChildWithID("presets")))menu->setSelectedId(47,juce::dontSendNotification);
        auto light=editor->createComponentSnapshot(editor->getLocalBounds(),true,1);auto lightOut=folder.getChildFile("VoxGarden-light-preview.png").createOutputStream();if(lightOut){lightOut->setPosition(0);lightOut->truncate();png.writeImageToStream(light,*lightOut);}
        check(light.getPixelAt(0,0)!=image.getPixelAt(0,0),"Light and dark themes render independently");
        lightOut.reset();editor.reset();processor.language=0;processor.setValue("raga",1);editor.reset(processor.createEditor());
        auto* picker=dynamic_cast<juce::ComboBox*>(editor->findChildWithID("languageChoice"));auto* ragaPicker=dynamic_cast<juce::ComboBox*>(editor->findChildWithID("ragaChoice"));auto* sounds=dynamic_cast<juce::ComboBox*>(editor->findChildWithID("presets"));
        if(auto* bank=dynamic_cast<juce::ComboBox*>(editor->findChildWithID("voiceBank"))){bank->setSelectedId(3,juce::sendNotificationSync);auto* last=editor->findChildWithID("v24_mute");auto* first=editor->findChildWithID("v1_mute");check(last&&first&&last->isVisible()&&!first->isVisible(),"Third voice bank exposes voice 24 controls and hides the first bank");bank->setSelectedId(1,juce::sendNotificationSync);}
        bool menu=sounds&&sounds->getNumItems()==65;
        if(menu){sounds->setSelectedId(60,juce::sendNotificationSync);menu=processor.settings().raga==6&&processor.settings().aiCast==2;sounds->setSelectedId(47,juce::sendNotificationSync);menu &= processor.settings().raga==1&&processor.settings().aiCast==1;}
        check(menu,"Nested raga preset menus expose all 65 sounds and select the correct voice casts");
        bool switched=picker&&ragaPicker;
        if(switched)for(int language:{1,2,3,4,0}){picker->setSelectedId(language+1,juce::sendNotificationSync);switched &= processor.language.load()==language&&ragaPicker->getText()==vg::tr(language,"Yaman");}
        check(switched,"Language picker switches every locale and back without changing or duplicating selected raga names");editor.reset();
        for(int language=1;language<5;++language)for(int dark=0;dark<2;++dark){processor.language=language;processor.darkMode=dark!=0;processor.setValue("raga",1);editor.reset(processor.createEditor());
            if(auto* menu=dynamic_cast<juce::ComboBox*>(editor->findChildWithID("presets")))menu->setSelectedId(47,juce::dontSendNotification);
            auto snapshot=editor->createComponentSnapshot(editor->getLocalBounds(),true,1);auto filename="VoxGarden-"+juce::String(language)+(dark?"-dark.png":"-light.png");auto stream=folder.getChildFile(filename).createOutputStream();if(stream){stream->setPosition(0);stream->truncate();png.writeImageToStream(snapshot,*stream);}editor.reset();}

    }
    return failures?1:0;
}
