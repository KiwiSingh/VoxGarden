#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

// Transfer the unprocessed WAV to the host. This component deliberately has no
// connection to the offline renderer or the plugin's sound settings.
class DryVocalButton final:public juce::TextButton {
    juce::File file;
    bool dragged=false;
public:
    using BeginDrag=std::function<bool(const juce::StringArray&,bool,juce::Component*)>;
    BeginDrag beginDrag=[](const auto& files,bool move,auto* source){return juce::DragAndDropContainer::performExternalDragDropOfFiles(files,move,source);};
    std::function<void(bool)> onTransfer;
    DryVocalButton():juce::TextButton("Drag dry vocal to Live"){}
    void setFile(const juce::File& value){file=value;setEnabled(file.existsAsFile());}
    void mouseDown(const juce::MouseEvent& e)override{dragged=false;juce::TextButton::mouseDown(e);}
    void mouseDrag(const juce::MouseEvent& e)override{
        if(!dragged&&isEnabled()&&file.existsAsFile()&&e.getDistanceFromDragStart()>=6){dragged=true;setState(juce::Button::buttonNormal);bool started=beginDrag({file.getFullPathName()},false,this);if(onTransfer)onTransfer(started);return;}
        if(!dragged)juce::TextButton::mouseDrag(e);
    }
    void mouseUp(const juce::MouseEvent& e)override{if(dragged){setState(juce::Button::buttonNormal);return;}juce::TextButton::mouseUp(e);}
};
