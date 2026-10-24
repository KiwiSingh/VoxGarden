#include <juce_audio_utils/juce_audio_utils.h>
#include <iostream>
class SnappyTestApp final:public juce::JUCEApplication,private juce::Timer {
    juce::AudioPluginFormatManager manager;
    std::unique_ptr<juce::AudioPluginInstance> plugin;
    struct Window:juce::DocumentWindow {Window():DocumentWindow("VoxGarden - isolated SnappySnap test",juce::Colours::black,allButtons){setUsingNativeTitleBar(true);}void closeButtonPressed()override{juce::JUCEApplication::getInstance()->systemRequestedQuit();}};
    std::unique_ptr<Window> window;
    juce::AudioBuffer<float> audio{2,256};juce::MidiBuffer midi;int frame=0;double energy=0;bool finite=true;
public:
    const juce::String getApplicationName()override{return "VoxGarden Snap Test";}
    const juce::String getApplicationVersion()override{return "0.1";}
    void initialise(const juce::String&)override {
        manager.addDefaultFormats();juce::OwnedArray<juce::PluginDescription> found;
        for(auto* format:manager.getFormats())format->findAllTypesForFile(found,"/Library/Audio/Plug-Ins/VST3/SnappySnapFX.vst3");
        if(found.isEmpty()){quit();return;}juce::String error;plugin=manager.createPluginInstance(*found[0],48000,256,error);
        if(!plugin){std::cerr<<error<<std::endl;quit();return;}plugin->prepareToPlay(48000,256);
        window=std::make_unique<Window>();window->setContentOwned(plugin->createEditor(),true);window->centreWithSize(window->getWidth(),window->getHeight());window->setVisible(true);startTimer(16);
    }
    void timerCallback()override {for(int b=0;b<3;++b){for(int i=0;i<256;++i){float x=.1f*std::sin(juce::MathConstants<double>::twoPi*220*frame++/48000);audio.setSample(0,i,x);audio.setSample(1,i,x);}plugin->processBlock(audio,midi);for(int c=0;c<2;++c)for(int i=0;i<256;++i){float x=audio.getSample(c,i);finite&=std::isfinite(x);energy+=x*x;}}}
    void shutdown()override {stopTimer();window.reset();if(plugin)plugin->releaseResources();plugin.reset();std::cout<<"Snappy hosted audio finite: "<<finite<<", energy "<<energy<<std::endl;}
};
START_JUCE_APPLICATION(SnappyTestApp)
