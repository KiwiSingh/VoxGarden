#include "Processor.h"
#include "Editor.h"
namespace {
const std::array<const char*,19> ids {"root","scale","mode","mix","gain","detune","vibrato","human","width","correction","glide","space","weird","formantLock","voiceCount","aiPrint","aiBlend","aiCast","raga"};
}
juce::AudioProcessorValueTreeState::ParameterLayout VoxGardenProcessor::layout() {
    juce::AudioProcessorValueTreeState::ParameterLayout l;
    auto f=[&](const juce::String& id,const juce::String& name,float lo,float hi,float val,float step=.001f){l.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{id,1},name,juce::NormalisableRange<float>(lo,hi,step),val));};
    juce::StringArray r,s;for(auto n:vg::roots)r.add(n);for(auto n:vg::scales)s.add(n);
    l.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"root",1},"Key Root",r,0));
    l.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"scale",1},"Scale",s,0));
    l.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"mode",1},"Harmony Mode",juce::StringArray{"Scale harmony","MIDI chord","Chromatic intervals"},0));
    f("mix","Choir Mix",0,1,1);f("gain","Output Gain",0,1,.7f);
    f("detune","Detune cents",0,30,4,.1f);f("vibrato","Vibrato cents",0,35,2,.1f);
    f("human","Human timing ms",0,45,12,.1f);f("width","Stage Width",0,1,1);
    f("correction","Scale Correction",0,1,1);f("glide","Note Glide ms",5,200,35,.1f);
    f("space","Room",0,1,.12f);f("weird","Mutation",0,1,0);
    l.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"formantLock",1},"Preserve Formants",true));
    constexpr std::array<int,8> degrees {0,0,2,2,4,4,-7,-7};
    for(int i=0;i<vg::voices;++i) {
        juce::String prefix="v"+juce::String(i+1)+"_",name="Voice "+juce::String(i+1)+" ";
        f(prefix+"level",name+"Level",0,1,.6f);
        f(prefix+"pan",name+"Pan",-1,1,(i%2?1.f:-1.f)*(.25f+.09f*i));
        f(prefix+"formant",name+"Formant",-12,12,(i%2?1.f:-1.f)*.5f,.01f);
        f(prefix+"delay",name+"Delay ms",0,80,0,.1f);
        l.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{prefix+"degree",1},name+"Degree / Semitones",-14,14,degrees[(size_t)i]));
    }
    // Append new parameters so existing host parameter indices remain stable.
    l.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{"voiceCount",1},"Choir Voices",1,8,6));
    l.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"aiPrint",1},"AI Singers on Render",false));
    f("aiBlend","AI Singer Blend",0,1,1);
    for(int v=1;v<=vg::voices;++v){auto prefix="v"+juce::String(v)+"_",name="Voice "+juce::String(v)+" ";
        l.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{prefix+"mute",1},name+"Mute",false));
        l.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{prefix+"solo",1},name+"Solo",false));
    }
    l.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"aiCast",1},"AI Voice Cast",juce::StringArray{"Original mixed cast","Female voices","Male voices","Alternating female / male"},0));
    juce::StringArray ragas{"Off"};for(auto name:vg::ragaNames)ragas.add(name);l.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"raga",1},"Raga Note Set",ragas,0));
    return l;
}
VoxGardenProcessor::VoxGardenProcessor():AudioProcessor(BusesProperties().withInput("Vocal",juce::AudioChannelSet::stereo(),true).withOutput("Choir",juce::AudioChannelSet::stereo(),true)),state(*this,nullptr,"VoxGardenState",layout()) {
    for(size_t i=0;i<19;++i)cached[i]=state.getRawParameterValue(ids[i]);
    for(int v=0;v<vg::voices;++v) {auto p="v"+juce::String(v+1)+"_";cachedVoice[(size_t)v]={state.getRawParameterValue(p+"level"),state.getRawParameterValue(p+"pan"),state.getRawParameterValue(p+"formant"),state.getRawParameterValue(p+"delay"),state.getRawParameterValue(p+"degree"),state.getRawParameterValue(p+"mute"),state.getRawParameterValue(p+"solo")};}
}
vg::Settings VoxGardenProcessor::settings() const {
    vg::Settings s;auto val=[&](int i){return cached[(size_t)i]->load(std::memory_order_relaxed);};
    s.root=(int)val(0);s.scale=(int)val(1);s.mode=(int)val(2);s.mix=val(3);s.gain=val(4);s.detune=val(5);s.vibrato=val(6);s.human=val(7);s.width=val(8);s.correction=val(9);s.glide=val(10);s.space=val(11);s.weird=val(12);s.formantLock=val(13)>.5f;s.count=(int)val(14);s.aiPrint=val(15)>.5f;s.aiBlend=val(16);s.aiCast=(int)val(17);s.raga=(int)val(18);if(s.raga>0)s.scale=vg::ragaScales[(size_t)(s.raga-1)];
    for(int v=0;v<vg::voices;++v) {const auto& c=cachedVoice[(size_t)v];s.voice[(size_t)v]={c.level->load(),c.pan->load(),c.formant->load(),c.delay->load(),(int)c.degree->load(),c.mute->load()>.5f,c.solo->load()>.5f};}
    return s;
}
void VoxGardenProcessor::setValue(const juce::String& id,float v) {if(auto* p=state.getParameter(id)){p->beginChangeGesture();p->setValueNotifyingHost(p->convertTo0to1(v));p->endChangeGesture();}}
void VoxGardenProcessor::applyPreset(int preset) {
    // Root and scale belong to the song, so starting sounds retain them.
    for(int v=1;v<=vg::voices;++v){auto prefix="v"+juce::String(v)+"_";setValue(prefix+"mute",0);setValue(prefix+"solo",0);}
    preset=juce::jlimit(0,61,preset);
    const int ragaCast=preset>=54?2:preset>=46?1:0;
    const int ragaPreset=preset>=46?(preset-46)%8:preset>=38?preset-38:-1;
    if(ragaPreset>=0)preset=6;
    const int character=preset>=28?preset:-1;
    if(character>=0)preset=6;
    setValue("aiCast",0);
    const int extended=preset>=12?preset:-1;
    if(extended>=0)preset=6;
    setValue("mode",0);setValue("mix",1);setValue("gain",.6f);setValue("correction",1);setValue("formantLock",1);setValue("weird",0);setValue("space",.12f);setValue("human",12);setValue("detune",4);setValue("vibrato",2);setValue("glide",35);setValue("width",1);
    constexpr int degrees[12][8]={
        {0,0,2,2,4,4,-7,-7},{0,0,0,0,0,0,0,0},{-7,-7,0,0,2,4,7,7},
        {-7,0,2,4,7,9,11,14},{-7,0,2,4,7,9,11,14},
        {0,2,0,2,0,2,0,2},{0,2,4,-7,0,2,4,-7},{0,2,4,-7,0,2,4,7},
        {0,0,2,2,4,4,7,7},{-7,0,2,4,0,2,4,7},{0,4,0,4,-7,-3,0,4},{0,0,0,0,0,0,0,0}};
    constexpr int counts[12]={6,8,8,8,8,2,4,6,8,8,6,8};
    setValue("voiceCount",(float)counts[preset]);
    for(int v=0;v<8;++v){auto id="v"+juce::String(v+1)+"_";
        setValue(id+"degree",(float)degrees[preset][v]);setValue(id+"level",.6f);
        setValue(id+"pan",(v%2?1.f:-1.f)*(.25f+.09f*v));setValue(id+"delay",0);
        setValue(id+"formant",(v%2?1.f:-1.f)*(.4f+.12f*v));}
    if(preset==1){setValue("mix",.65f);setValue("human",20);setValue("correction",.4f);}
    if(preset==2){setValue("space",.7f);setValue("human",24);setValue("glide",70);}
    if(preset==3){setValue("weird",.45f);setValue("detune",18);setValue("human",40);setValue("space",.5f);}
    if(preset==4){setValue("weird",.9f);setValue("space",.8f);setValue("vibrato",28);setValue("glide",150);}
    if(preset>=5){
        constexpr float rooms[7]={.04f,.10f,.25f,.32f,.18f,.2f,.35f};
        constexpr float timing[7]={7,10,18,22,15,14,25};
        setValue("space",rooms[preset-5]);setValue("human",timing[preset-5]);
        setValue("detune",preset==5?2.f:3.5f);setValue("vibrato",preset==11?3.f:1.5f);
        setValue("width",preset==5?.55f:(preset==6?.7f:.95f));setValue("glide",preset==9?25.f:50.f);
        for(int v=0;v<8;++v){auto id="v"+juce::String(v+1)+"_";
            float shade=(v%2?1.f:-1.f)*(.22f+.075f*v);
            if(preset==7)shade-=.5f;if(preset==8)shade+=.65f;if(preset==11)shade+=.25f;
            setValue(id+"formant",shade);setValue(id+"delay",(float)((v*3)%8));
            setValue(id+"level",degrees[preset][v]<0?.4f:.6f);}
    }
    if(extended>=0){
        // Musical voicings, not additional samples: all follow the selected key.
        constexpr int notes[16][8]={
            {0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{-7,-7,0,0,0,0,0,0},
            {0,2,4,6,0,2,4,6},{0,1,4,0,1,4,0,0},{-7,0,4,7,-7,0,4,7},
            {-7,-5,-3,0,-7,-5,-3,0},{0,2,4,7,0,2,4,7},{0,0,2,2,4,4,7,7},
            {0,2,4,6,7,9,11,13},{0,4,7,11,0,4,7,11},{0,0,0,0,0,0,0,0},
            {-7,0,7,14,-7,0,7,14},{0,2,4,6,0,2,4,6},{0,1,2,3,4,5,6,7},{0,4,-7,7,0,4,-7,7}};
        constexpr int sizes[16]={2,4,4,4,3,8,4,4,8,8,4,8,8,4,8,8};
        constexpr float rooms[16]={.02f,.08f,.2f,.15f,.12f,.65f,.18f,.22f,.45f,.55f,.65f,.45f,.7f,.05f,.65f,.35f};
        int i=extended-12;setValue("voiceCount",(float)sizes[i]);setValue("space",rooms[i]);
        setValue("human",i<2?5.f:18.f);setValue("detune",i<2?2.f:4.f);setValue("glide",i>=9?100.f:45.f);
        if(i<2)setValue("mix",.45f);if(i==11){setValue("correction",.25f);setValue("detune",12);setValue("vibrato",7);}
        if(i>=12){setValue("weird",i==13?.7f:.35f);setValue("vibrato",i==14?25.f:8.f);}
        for(int v=0;v<8;++v){auto id="v"+juce::String(v+1)+"_";setValue(id+"degree",(float)notes[i][v]);
            setValue(id+"delay",(float)((v*7)%23));setValue(id+"level",notes[i][v]<0?.4f:.6f);
            setValue(id+"formant",i==6?-1.2f:(i==7?1.1f:(i>=12?(v%2?3.f:-3.f):.25f*(v%2?1.f:-1.f))));}
    }
    if(character>=0){
        constexpr int notes[10][8]={
            {0,2,4,0,2,4,7,7},{-7,-5,-3,-7,-5,-3,0,0},
            {0,2,4,-3,0,2,4,-3},{7,9,11,7,9,11,7,11},
            {-7,-5,-3,0,-7,-5,-3,0},{-14,-7,-10,-7,-14,-7,-10,-7},
            {0,-7,2,-5,4,-3,7,0},{0,-7,0,-7,0,-7,0,-7},
            {7,9,11,14,7,9,11,14},{-14,-10,-7,-14,-10,-7,-14,-7}};
        constexpr int casts[10]={1,2,1,1,2,2,3,3,1,2};
        constexpr int sizes[10]={6,6,4,8,4,6,8,2,6,8};
        constexpr float shades[10]={2.2f,-2.2f,.9f,2.8f,-1.7f,-3.f,0,0,5.f,-5.f};
        constexpr float rooms[10]={.18f,.18f,.14f,.55f,.12f,.65f,.32f,.05f,.25f,.5f};
        int i=character-28;setValue("aiCast",(float)casts[i]);setValue("voiceCount",(float)sizes[i]);setValue("space",rooms[i]);
        setValue("human",i==7?8.f:17.f);setValue("glide",i>=8?30.f:55.f);setValue("detune",i>=8?9.f:3.f);setValue("vibrato",i>=8?7.f:2.f);
        if(i>=8)setValue("weird",.15f);
        for(int v=0;v<8;++v){auto id="v"+juce::String(v+1)+"_";
            setValue(id+"degree",(float)notes[i][v]);setValue(id+"formant",i==6||i==7?(v%2?-1.7f:1.7f):shades[i]+(v%2?.18f:-.18f));
            setValue(id+"delay",(float)((v*5)%17));setValue(id+"level",.52f);}
    }
    if(ragaPreset>=0){
        setValue("raga",(float)(ragaPreset+1));
        const auto& notes=vg::intervals[(size_t)vg::ragaScales[(size_t)ragaPreset]];int octave=(int)notes.size();
        auto pa=std::find(notes.begin(),notes.end(),7);int fifth=pa==notes.end()?0:(int)(pa-notes.begin());
        const int degrees[8]={0,0,octave,-octave,0,fifth,0,octave};
        setValue("voiceCount",ragaPreset==4||ragaPreset==5?6.f:4.f);setValue("correction",1);setValue("detune",1.5f);setValue("vibrato",1);setValue("human",16);setValue("glide",85);setValue("space",ragaPreset==4||ragaPreset==5?.4f:.22f);setValue("weird",0);
        for(int v=0;v<8;++v){auto id="v"+juce::String(v+1)+"_";setValue(id+"degree",(float)degrees[v]);setValue(id+"formant",(v%2?.2f:-.2f));setValue(id+"level",v==0||v==1?.6f:.32f);setValue(id+"delay",(float)(v*2));}
        if(ragaCast!=0){
            setValue("aiCast",(float)ragaCast);setValue("voiceCount",6);setValue("width",.95f);
            // Octaves use this raga's own note count, including five-note ragas.
            const int upper[8]={0,0,octave,octave,0,fifth,octave,0};
            const int lower[8]={-octave,-octave,0,0,-octave,fifth-octave,0,-octave};
            for(int v=0;v<8;++v){auto id="v"+juce::String(v+1)+"_";
                setValue(id+"degree",(float)(ragaCast==1?upper[v]:lower[v]));
                setValue(id+"formant",(ragaCast==1?1.8f:-1.8f)+(v%2?.15f:-.15f));
                setValue(id+"level",v<2?.55f:.38f);}
        }
    }
}
void VoxGardenProcessor::prepareToPlay(double sr,int block) {capture.prepare(sr,block);engine.prepare(sr);held.fill(false);setLatencySamples(engine.latency());}
void VoxGardenProcessor::reset() {if(getSampleRate()>0) {engine.prepare(getSampleRate());held.fill(false);}}
bool VoxGardenProcessor::isBusesLayoutSupported(const BusesLayout& l)const {return (l.getMainInputChannelSet()==juce::AudioChannelSet::stereo()||l.getMainInputChannelSet()==juce::AudioChannelSet::mono()) && l.getMainOutputChannelSet()==juce::AudioChannelSet::stereo();}
void VoxGardenProcessor::processBlock(juce::AudioBuffer<float>& b,juce::MidiBuffer& midi) {
    juce::ScopedNoDenormals noDenormals;
    if(b.getNumChannels()<2)return;
    if(getTotalNumInputChannels()==1)b.copyFrom(1,0,b,0,0,b.getNumSamples());
    capture.push(b.getReadPointer(0),b.getReadPointer(1),b.getNumSamples());
    auto s=settings();
    auto phase=capture.getPhase();
    const bool dryMonitor=printInProgress.load() || phase==TrackCapture::starting || phase==TrackCapture::recording || phase==TrackCapture::stopping || phase==TrackCapture::complete;
    auto processRange=[&](int from,int n){if(n<=0)return;s.midiCount=0;for(int i=0;i<128 && s.midiCount<128;++i)if(held[(size_t)i])s.midi[(size_t)s.midiCount++]=i;
        // In chord mode, silence wet voices until a chord is held.
        auto active=s;if(s.mode==1 && s.midiCount==0)for(auto& v:active.voice)v.level=0;
        engine.process(b.getWritePointer(0)+from,b.getWritePointer(1)+from,n,active,nullptr,nullptr,dryMonitor);
    };
    int cursor=0;
    for(const auto metadata:midi) {int at=juce::jlimit(cursor,b.getNumSamples(),metadata.samplePosition);processRange(cursor,at-cursor);cursor=at;auto m=metadata.getMessage();if(m.isNoteOn())held[(size_t)m.getNoteNumber()]=true;else if(m.isNoteOff())held[(size_t)m.getNoteNumber()]=false;else if(m.isAllNotesOff()||m.isAllSoundOff())held.fill(false);}
    processRange(cursor,b.getNumSamples()-cursor);midi.clear();
}
void VoxGardenProcessor::getStateInformation(juce::MemoryBlock& out) {auto tree=state.copyState();tree.setProperty("schemaVersion",7,nullptr);tree.setProperty("darkMode",darkMode.load(),nullptr);tree.setProperty("language",language.load(),nullptr);if(auto xml=tree.createXml())copyXmlToBinary(*xml,out);}
void VoxGardenProcessor::setStateInformation(const void* data,int size) {if(auto xml=getXmlFromBinary(data,size))if(xml->hasTagName(state.state.getType())){auto tree=juce::ValueTree::fromXml(*xml);darkMode.store((bool)tree.getProperty("darkMode",true));language.store(juce::jlimit(0,4,(int)tree.getProperty("language",0)));bool hasCount=false,hasAI=false,hasCast=false,hasRaga=false;for(int i=0;i<tree.getNumChildren();++i){auto id=tree.getChild(i).getProperty("id").toString();hasCount|=id=="voiceCount";hasAI|=id=="aiPrint";hasCast|=id=="aiCast";hasRaga|=id=="raga";}std::array<bool,16> auditionPresent{};
    for(int v=1;v<=vg::voices;++v)for(int k=0;k<2;++k){auto id="v"+juce::String(v)+(k==0?"_mute":"_solo");for(int i=0;i<tree.getNumChildren();++i)if(tree.getChild(i).getProperty("id").toString()==id)auditionPresent[(size_t)((v-1)*2+k)]=true;}
    state.replaceState(tree);
    for(int v=1;v<=vg::voices;++v)for(int k=0;k<2;++k)if(!auditionPresent[(size_t)((v-1)*2+k)])state.getParameter("v"+juce::String(v)+(k==0?"_mute":"_solo"))->setValueNotifyingHost(0);if(!hasRaga)state.getParameter("raga")->setValueNotifyingHost(0);if(!hasCast)state.getParameter("aiCast")->setValueNotifyingHost(0);if(!hasAI){state.getParameter("aiPrint")->setValueNotifyingHost(0);state.getParameter("aiBlend")->setValueNotifyingHost(1);}if(!hasCount)if(auto* param=state.getParameter("voiceCount"))param->setValueNotifyingHost(param->convertTo0to1(8));}}
juce::AudioProcessorEditor* VoxGardenProcessor::createEditor(){return new VoxGardenEditor(*this);}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new VoxGardenProcessor();}
