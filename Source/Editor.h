#pragma once
#include "Processor.h"
#include "I18n.h"
#include <map>
struct GardenPalette {
    juce::Colour paper,ink,forest,sage,muted,line,gold,panel,background,knob;
    explicit GardenPalette(bool dark):paper(dark?0xff151e20:0xfff3f0e8),ink(dark?0xffe6eee7:0xff223d38),forest(dark?0xff99d4b1:0xff294f46),sage(dark?0xff7caa95:0xff8fa99a),muted(dark?0xffa2b6ad:0xff728078),line(dark?0xff3b4c48:0xffdfdfd4),gold(dark?0xffe1bb7d:0xffcfaa6d),panel(dark?0xff202d2d:0xfffafaf3),background(dark?0xff10191b:0xffe9ecdf),knob(dark?0xff30423f:0xffffffff){}
    GardenPalette(bool dark,int accent):GardenPalette(dark){
        if(accent==1){forest=juce::Colour(dark?0xffffacd5:0xff9b2869);sage=juce::Colour(dark?0xffc895b4:0xffb36994);gold=juce::Colour(dark?0xffffce9c:0xff94531e);paper=juce::Colour(dark?0xff271b29:0xffffedf5);panel=juce::Colour(dark?0xff352538:0xfffff8fc);background=juce::Colour(dark?0xff19121f:0xfff2d5e7);line=juce::Colour(dark?0xff64465e:0xffdcb6ce);ink=juce::Colour(dark?0xffffedf7:0xff49263e);muted=juce::Colour(dark?0xffd2aec5:0xff805b73);}
        if(accent==2){forest=juce::Colour(dark?0xff8edcff:0xff126187);sage=juce::Colour(dark?0xffa7b8ee:0xff667aa8);gold=juce::Colour(dark?0xffeab1ff:0xff884da4);paper=juce::Colour(dark?0xff152336:0xffedf5ff);panel=juce::Colour(dark?0xff20334b:0xfff9fcff);background=juce::Colour(dark?0xff101827:0xffdce6fa);line=juce::Colour(dark?0xff3f5677:0xffbfcee2);ink=juce::Colour(dark?0xffe4f5ff:0xff22364f);muted=juce::Colour(dark?0xffacc5e2:0xff5e738b);}
        if(accent==3){forest=juce::Colour(dark?0xffffbb85:0xff9d4519);sage=juce::Colour(dark?0xffd6a59a:0xffad7860);gold=juce::Colour(dark?0xffe8aaff:0xff8a419c);paper=juce::Colour(dark?0xff302024:0xfffff1e6);panel=juce::Colour(dark?0xff402d32:0xfffffaf4);background=juce::Colour(dark?0xff21171e:0xfff4dacc);line=juce::Colour(dark?0xff70504c:0xffdec1b2);ink=juce::Colour(dark?0xfffff0df:0xff513329);muted=juce::Colour(dark?0xffdbb7a9:0xff88665a);}
        if(accent>0)knob=panel.brighter(dark?.12f:.03f);
    }

};
class GardenLook final:public juce::LookAndFeel_V4 {
public:
    GardenPalette palette{true};
    int language=0;
    void drawTableHeaderBackground(juce::Graphics& g,juce::TableHeaderComponent&)override{g.fillAll(palette.panel);}
    void drawTableHeaderColumn(juce::Graphics& g,juce::TableHeaderComponent&,const juce::String& name,int,int width,int height,bool over,bool,int)override{if(over){g.setColour(palette.forest.withAlpha(.12f));g.fillAll();}g.setColour(palette.ink);g.setFont(juce::FontOptions(13,juce::Font::bold));g.drawFittedText(name,8,0,width-16,height,juce::Justification::centredLeft,1);g.setColour(palette.line);g.drawVerticalLine(width-1,0,(float)height);}

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
    std::unique_ptr<juce::DialogWindow> composer;
    juce::Label title,subtitle,pitch,status,ragaNotes;
    juce::ComboBox root,scale,mode,presets,voiceCount,aiCast,raga,languageChoice,themeChoice,voiceBank;
    juce::ToggleButton theme{"Dark mode"};
    juce::ToggleButton formants{"Preserve formants"},aiPrint{"AI singers on print"};
    juce::Slider aiBlend;
    std::array<juce::TextButton,vg::voices> muteButtons,soloButtons;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> auditionAttachments;
    juce::TextButton render{"Render vocal + stems..."},help{"Ableton setup"},reveal{"Show renders"},importFile{"Render a file"},compose{"Notes + lyrics"};
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
    int lastCount=-1,lastAccent=-1;
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
