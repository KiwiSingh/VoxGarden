#pragma once
#include "Processor.h"
#include "I18n.h"
#include <map>
struct GardenPalette {
    juce::Colour paper,ink,forest,sage,muted,line,gold,panel,background,knob;
    explicit GardenPalette(bool dark):paper(dark?0xff151e20:0xfff3f0e8),ink(dark?0xffe6eee7:0xff223d38),forest(dark?0xff99d4b1:0xff294f46),sage(dark?0xff7caa95:0xff8fa99a),muted(dark?0xffa2b6ad:0xff728078),line(dark?0xff3b4c48:0xffdfdfd4),gold(dark?0xffe1bb7d:0xffcfaa6d),panel(dark?0xff202d2d:0xfffafaf3),background(dark?0xff10191b:0xffe9ecdf),knob(dark?0xff30423f:0xffffffff){}
};
class GardenLook final:public juce::LookAndFeel_V4 {
public:
    GardenPalette palette{true};
    int language=0;
    void drawToggleButton(juce::Graphics&,juce::ToggleButton&,bool,bool)override;
    void drawRotarySlider(juce::Graphics&,int,int,int,int,float,float,float,juce::Slider&)override;
    void drawLinearSlider(juce::Graphics&,int,int,int,int,float,float,float,juce::Slider::SliderStyle,juce::Slider&)override;
    void drawButtonBackground(juce::Graphics&,juce::Button&,const juce::Colour&,bool,bool)override;
    void drawComboBox(juce::Graphics&,int,int,bool,int,int,int,int,juce::ComboBox&)override;
    juce::Font getComboBoxFont(juce::ComboBox&)override{return juce::FontOptions(language==1?16:14);}
};
class Stage final:public juce::Component {
    VoxGardenProcessor& p;int selected=-1;
public:
    explicit Stage(VoxGardenProcessor& proc):p(proc){}
    void paint(juce::Graphics&)override;
    void mouseDown(const juce::MouseEvent&)override;
    void mouseDrag(const juce::MouseEvent&)override;
    void mouseUp(const juce::MouseEvent&)override;
};
class VoxGardenEditor final:public juce::AudioProcessorEditor,public juce::FileDragAndDropTarget,private juce::Timer,private juce::Thread {
public:
    explicit VoxGardenEditor(VoxGardenProcessor&);
    ~VoxGardenEditor() override;
    void paint(juce::Graphics&)override;
    void resized()override;
    bool isInterestedInFileDrag(const juce::StringArray&)override;
    void filesDropped(const juce::StringArray&,int,int)override;
private:
    VoxGardenProcessor& p;
    GardenLook look;
    Stage stage;
    juce::Label title,subtitle,pitch,status,ragaNotes;
    juce::ComboBox root,scale,mode,presets,voiceCount,aiCast,raga,languageChoice;
    juce::ToggleButton theme{"Dark mode"};
    juce::ToggleButton formants{"Preserve formants"},aiPrint{"AI singers on print"};
    juce::Slider aiBlend;
    std::array<juce::TextButton,vg::voices> muteButtons,soloButtons;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> auditionAttachments;
    juce::TextButton render{"Render vocal + stems..."},help{"Ableton setup"},reveal{"Show renders"},importFile{"Render a file"};
    struct Control {juce::Label label;juce::Slider slider;std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;};
    std::vector<std::unique_ptr<Control>> controls;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>> choices;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> formantAttachment,aiAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> aiBlendAttachment;
    std::unique_ptr<juce::FileChooser> chooser;
    juce::File inputFile,outputFolder;
    vg::Settings renderSettings;
    bool capturePending=false;
    int lastLanguage=-1,lastRaga=-1;
    std::map<juce::ComboBox*,juce::PopupMenu> englishMenus;
    juce::String t(const juce::String& text)const{return vg::tr(p.language.load(),text);}
    void applyLanguage();
    int lastCount=-1;
    std::atomic<float> progress{0};std::atomic<bool> rendering{false};
    juce::String result;
    void applyTheme();
    void addControl(const juce::String&,const juce::String&);
    void beginRender();
    void renderFile(const juce::File&,bool trackTake=false);
    void chooseFile();
    void run()override;
    void timerCallback()override;
};
