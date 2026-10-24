#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "Engine.h"
#include "Capture.h"
class VoxGardenProcessor final : public juce::AudioProcessor {
public:
    VoxGardenProcessor();
    juce::AudioProcessorValueTreeState state;
    vg::Engine engine;
    TrackCapture capture;
    std::atomic<bool> printInProgress{false}, darkMode{true};
    std::atomic<int> language{0}, themeAccent{0};
    juce::CriticalSection scoreLock;
    juce::String scoreDraft{"0\t0.5\t62\tHello\n0.5\t0.5\t60\tmy\n1\t0.5\t62\tnew\n1.5\t1\t69\tchoir"};
    int scoreLanguage=0,scoreVoice=0;
    vg::Settings captureRenderSettings;
    static juce::AudioProcessorValueTreeState::ParameterLayout layout();
    vg::Settings settings() const;
    void setValue(const juce::String&,float);
    void applyPreset(int);
    void prepareToPlay(double,int) override;
    void releaseResources() override {capture.shutdown();}
    void reset() override;
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override {return true;}
    const juce::String getName() const override {return "VoxGarden";}
    bool acceptsMidi() const override {return true;}
    bool producesMidi() const override {return false;}
    double getTailLengthSeconds() const override {return 4;}
    int getNumPrograms() override {return 1;}
    int getCurrentProgram() override {return 0;}
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override {return {};}
    void changeProgramName(int,const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*,int) override;
private:
    std::array<bool,128> held{};
    // Cache parameter pointers once. Never look up strings or allocate in processBlock.
    struct CachedVoice {std::atomic<float> *level,*pan,*formant,*delay,*degree,*mute,*solo;};
    std::array<CachedVoice,vg::voices> cachedVoice;
    std::array<std::atomic<float>*,19> cached{};
};
