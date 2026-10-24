#include "Processor.h"
#include "Offline.h"
#include "AI.h"
#include "I18n.h"
#include <iostream>
#include <chrono>
static int failures=0;
static void check(bool pass,const char* message){std::cout<<(pass?"PASS ":"FAIL ")<<message<<std::endl;if(!pass)++failures;}
int main(int argc,char** argv) {
    juce::ScopedJuceInitialiser_GUI gui;
    if(argc>3 && juce::String(argv[1])=="--ai-smoke"){
        VoxGardenProcessor p;p.applyPreset(5);p.setValue("root",9);p.setValue("scale",1);p.setValue("aiPrint",1);p.setValue("aiBlend",1);
        if(argc>4)p.setValue("aiCast",juce::String(argv[4]).getIntValue());
        if(argc>5)p.setValue("raga",juce::String(argv[5]).getIntValue());
        auto settings=p.settings();juce::File input{juce::String(argv[2])},output{juce::String(argv[3])};std::atomic<float> progress{0};std::vector<juce::File> singers;
        auto error=vg::createAISingers(input,output,settings,progress,[]{return false;},singers);check(error.isEmpty(),"Plugin launches local AI worker and receives two singers");if(error.isNotEmpty()){std::cout<<error<<std::endl;return 1;}
        auto result=renderVocal(input,output,settings,progress,[]{return false;},singers);std::cout<<result<<std::endl;check(progress.load()==1,"AI singers pass through native mixing and stem export");return failures?1:0;
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
        juce::AudioBuffer<float> io(2,24000),stems(16,24000);stems.clear();for(int i=0;i<24000;++i)for(int c=0;c<2;++c)io.setSample(c,i,.1f*std::sin(juce::MathConstants<double>::twoPi*220*i/48000));
        solo.process(io.getWritePointer(0),io.getWritePointer(1),24000,settings,stems.getArrayOfWritePointers());bool muted=true;for(int c=2;c<16;++c)muted&=stems.getMagnitude(c,0,24000)<1e-7f;
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
        if(found.size()){juce::String error;auto instance=manager.createPluginInstance(*found[0],48000,256,error);check(instance!=nullptr,"Built plugin instantiates through host API");if(!instance)std::cout<<"Host error: "<<error<<std::endl;if(instance){instance->prepareToPlay(48000,256);juce::AudioBuffer<float> io(2,256);io.clear();instance->processBlock(io,midi);std::cout<<"Host parameter count: "<<instance->getParameters().size()<<" (includes wrapper bypass)\n";check(instance->getParameters().size()==76,"Host exposes 75 sound parameters plus wrapper bypass");juce::MemoryBlock chunk;instance->getStateInformation(chunk);instance->setStateInformation(chunk.getData(),(int)chunk.getSize());check(chunk.getSize()>0,"Host state chunk round trip");instance->releaseResources();}}}
    if(argc>2) {
        processor.applyPreset(46);
        auto folder=juce::File(juce::String(argv[2]));folder.createDirectory();
        std::unique_ptr<juce::AudioProcessorEditor> editor(processor.createEditor());
        if(auto* menu=dynamic_cast<juce::ComboBox*>(editor->findChildWithID("presets")))menu->setSelectedId(47,juce::dontSendNotification);
        auto image=editor->createComponentSnapshot(editor->getLocalBounds(),true,1);
        juce::PNGImageFormat png;auto out=folder.getChildFile("VoxGarden-preview.png").createOutputStream();if(out){out->setPosition(0);out->truncate();png.writeImageToStream(image,*out);}
        check(image.isValid(),"Plugin editor renders to image");
        out.reset();editor.reset();processor.darkMode=false;editor.reset(processor.createEditor());
        if(auto* menu=dynamic_cast<juce::ComboBox*>(editor->findChildWithID("presets")))menu->setSelectedId(47,juce::dontSendNotification);
        auto light=editor->createComponentSnapshot(editor->getLocalBounds(),true,1);auto lightOut=folder.getChildFile("VoxGarden-light-preview.png").createOutputStream();if(lightOut){lightOut->setPosition(0);lightOut->truncate();png.writeImageToStream(light,*lightOut);}
        check(light.getPixelAt(0,0)!=image.getPixelAt(0,0),"Light and dark themes render independently");
        lightOut.reset();editor.reset();processor.language=0;processor.setValue("raga",1);editor.reset(processor.createEditor());
        auto* picker=dynamic_cast<juce::ComboBox*>(editor->findChildWithID("languageChoice"));auto* ragaPicker=dynamic_cast<juce::ComboBox*>(editor->findChildWithID("ragaChoice"));auto* sounds=dynamic_cast<juce::ComboBox*>(editor->findChildWithID("presets"));
        bool menu=sounds&&sounds->getNumItems()==62;
        if(menu){sounds->setSelectedId(60,juce::sendNotificationSync);menu=processor.settings().raga==6&&processor.settings().aiCast==2;sounds->setSelectedId(47,juce::sendNotificationSync);menu &= processor.settings().raga==1&&processor.settings().aiCast==1;}
        check(menu,"Nested raga preset menus expose all 62 sounds and select the correct voice casts");
        bool switched=picker&&ragaPicker;
        if(switched)for(int language:{1,2,3,4,0}){picker->setSelectedId(language+1,juce::sendNotificationSync);switched &= processor.language.load()==language&&ragaPicker->getText()==vg::tr(language,"Yaman");}
        check(switched,"Language picker switches every locale and back without changing or duplicating selected raga names");editor.reset();
        for(int language=1;language<5;++language)for(int dark=0;dark<2;++dark){processor.language=language;processor.darkMode=dark!=0;processor.setValue("raga",1);editor.reset(processor.createEditor());
            if(auto* menu=dynamic_cast<juce::ComboBox*>(editor->findChildWithID("presets")))menu->setSelectedId(47,juce::dontSendNotification);
            auto snapshot=editor->createComponentSnapshot(editor->getLocalBounds(),true,1);auto filename="VoxGarden-"+juce::String(language)+(dark?"-dark.png":"-light.png");auto stream=folder.getChildFile(filename).createOutputStream();if(stream){stream->setPosition(0);stream->truncate();png.writeImageToStream(snapshot,*stream);}editor.reset();}

    }
    return failures?1:0;
}
