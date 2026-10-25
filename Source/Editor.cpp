#include "Editor.h"
#include "Offline.h"
#include "AI.h"
#include "Composer.h"
#include "GenrePresets.h"
namespace {
#define VG_PALETTE const auto& palette=static_cast<GardenLook&>(getLookAndFeel()).palette; const auto paper=palette.paper,ink=palette.ink,forest=palette.forest,sage=palette.sage,muted=palette.muted,line=palette.line,gold=palette.gold;
void caption(juce::Graphics& g,const juce::String& s,int x,int y,int w,juce::Colour colour){g.setColour(colour);g.setFont(juce::FontOptions(vg::usesDevanagari(s)?12.f:10.f,juce::Font::bold));g.drawText(s,x,y,w,16,juce::Justification::centredLeft);}
}
void GardenLook::drawToggleButton(juce::Graphics& g,juce::ToggleButton& button,bool over,bool){ const auto paper=palette.paper,ink=palette.ink,forest=palette.forest,sage=palette.sage,muted=palette.muted,line=palette.line,gold=palette.gold;
    auto box=juce::Rectangle<float>(3,(float)button.getHeight()*.5f-8,16,16);g.setColour(button.getToggleState()?forest:paper);g.fillRoundedRectangle(box,4);g.setColour(forest.withAlpha(over?.8f:.45f));g.drawRoundedRectangle(box,4,1);
    if(button.getToggleState()){juce::Path mark;mark.startNewSubPath(box.getX()+4,box.getY()+8);mark.lineTo(box.getX()+7,box.getY()+11);mark.lineTo(box.getX()+12,box.getY()+5);g.setColour(paper);g.strokePath(mark,juce::PathStrokeType(1.6f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));}
    g.setColour(ink.withMultipliedAlpha(button.isEnabled()?1.f:.4f));g.setFont(juce::FontOptions(language==1?15:13));g.drawText(button.getButtonText(),26,0,button.getWidth()-26,button.getHeight(),juce::Justification::centredLeft);
}
void GardenLook::drawRotarySlider(juce::Graphics& g,int x,int y,int w,int h,float pos,float start,float end,juce::Slider& slider){ const auto paper=palette.paper,ink=palette.ink,forest=palette.forest,sage=palette.sage,muted=palette.muted,line=palette.line,gold=palette.gold;
    auto a=juce::Rectangle<float>((float)x,(float)y,(float)w,(float)h).reduced(13);auto size=std::min(a.getWidth(),a.getHeight());auto c=a.getCentre();float r=size*.5f;
    g.setColour(ink.withAlpha(.04f));g.fillEllipse(c.x-r+1,c.y-r+3,size,size);
    g.setColour(palette.line);juce::Path track;track.addCentredArc(c.x,c.y,r,r,0,start,end,true);g.strokePath(track,juce::PathStrokeType(4,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
    float angle=start+pos*(end-start);juce::Path fill;fill.addCentredArc(c.x,c.y,r,r,0,start,angle,true);g.setColour(slider.getComponentID()=="weird"?gold:forest);g.strokePath(fill,juce::PathStrokeType(4,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
    g.setGradientFill(juce::ColourGradient(palette.knob,c.x,c.y-r,paper,c.x,c.y+r,false));g.fillEllipse(c.x-r+6,c.y-r+6,size-12,size-12);
    g.setColour(forest.withAlpha(.25f));g.drawEllipse(c.x-r+6,c.y-r+6,size-12,size-12,.6f);
    juce::Point<float> tip{c.x+std::sin(angle)*(r-10),c.y-std::cos(angle)*(r-10)};g.setColour(ink);g.fillEllipse(tip.x-2,tip.y-2,4,4);
}
void GardenLook::drawLinearSlider(juce::Graphics& g,int x,int y,int w,int h,float pos,float,float,juce::Slider::SliderStyle,juce::Slider& slider){ const auto paper=palette.paper,ink=palette.ink,forest=palette.forest,sage=palette.sage,muted=palette.muted,line=palette.line,gold=palette.gold;
    float mid=(float)y+(float)h*.5f;auto colour=slider.isEnabled()?forest:sage.withAlpha(.4f);
    g.setColour(line);g.fillRoundedRectangle((float)x,mid-1.5f,(float)w,3,1.5f);g.setColour(colour.withAlpha(.6f));g.fillRoundedRectangle((float)x,mid-1.5f,std::max(0.f,pos-x),3,1.5f);
    g.setColour(paper);g.fillEllipse(pos-5,mid-5,10,10);g.setColour(colour);g.drawEllipse(pos-5,mid-5,10,10,1.7f);
}
void GardenLook::drawButtonBackground(juce::Graphics& g,juce::Button& b,const juce::Colour& base,bool over,bool down){ const auto paper=palette.paper,ink=palette.ink,forest=palette.forest,sage=palette.sage,muted=palette.muted,line=palette.line,gold=palette.gold;
    auto a=b.getLocalBounds().toFloat().reduced(.5f);g.setColour(base.brighter(over?.06f:0).darker(down?.08f:0).withMultipliedAlpha(b.isEnabled()?1.f:.45f));g.fillRoundedRectangle(a,9);g.setColour(ink.withAlpha(.14f));g.drawRoundedRectangle(a,9,.8f);
}
void GardenLook::drawComboBox(juce::Graphics& g,int w,int h,bool,int,int,int,int,juce::ComboBox& b){ const auto paper=palette.paper,ink=palette.ink,forest=palette.forest,sage=palette.sage,muted=palette.muted,line=palette.line,gold=palette.gold;
    auto a=juce::Rectangle<float>(0,0,(float)w,(float)h).reduced(.5f);g.setColour(b.findColour(juce::ComboBox::backgroundColourId));g.fillRoundedRectangle(a,8);g.setColour(line);g.drawRoundedRectangle(a,8,1);
    juce::Path arrow;arrow.startNewSubPath((float)w-20,(float)h*.43f);arrow.lineTo((float)w-15,(float)h*.58f);arrow.lineTo((float)w-10,(float)h*.43f);g.setColour(muted);g.strokePath(arrow,juce::PathStrokeType(1.4f));
}
Stage::Stage(VoxGardenProcessor& proc):p(proc){
    addAndMakeVisible(view);addAndMakeVisible(mute);addAndMakeVisible(solo);addAndMakeVisible(selection);
    view.setComponentID("stageView");view.addItem(vg::tr(p.language.load(),"Low view"),1);view.addItem(vg::tr(p.language.load(),"High view"),2);view.setSelectedId(2,juce::dontSendNotification);view.onChange=[this]{repaint();};
    mute.setClickingTogglesState(true);solo.setClickingTogglesState(true);mute.setComponentID("stageMute");solo.setComponentID("stageSolo");
    mute.setTooltip(vg::tr(p.language.load(),"Mute the selected singer"));solo.setTooltip(vg::tr(p.language.load(),"Solo the selected singer"));
    mute.onClick=[this]{if(selected>=0)p.setValue("v"+juce::String(selected+1)+"_mute",mute.getToggleState()?1.f:0.f);refresh();};
    solo.onClick=[this]{if(selected>=0)p.setValue("v"+juce::String(selected+1)+"_solo",solo.getToggleState()?1.f:0.f);refresh();};refresh();
}
void Stage::lookAndFeelChanged(){if(auto* look=dynamic_cast<GardenLook*>(&getLookAndFeel())){auto c=look->palette;selection.setColour(juce::Label::textColourId,c.muted);selection.setFont(juce::FontOptions(11));for(auto* button:{&mute,&solo}){button->setColour(juce::TextButton::buttonColourId,c.panel);button->setColour(juce::TextButton::buttonOnColourId,button==&mute?c.gold:c.forest);button->setColour(juce::TextButton::textColourOnId,c.background);}}}
void Stage::setLanguage(){int id=view.getSelectedId();view.clear(juce::dontSendNotification);view.addItem(vg::tr(p.language.load(),"Low view"),1);view.addItem(vg::tr(p.language.load(),"High view"),2);view.setSelectedId(std::max(1,id),juce::dontSendNotification);mute.setTooltip(vg::tr(p.language.load(),"Mute the selected singer"));solo.setTooltip(vg::tr(p.language.load(),"Solo the selected singer"));refresh();}
Stage::~Stage(){finishDrag();}
void Stage::finishDrag(){if(dragging&&selected>=0)for(auto suffix:{"pan","delay"})p.state.getParameter("v"+juce::String(selected+1)+"_"+suffix)->endChangeGesture();dragging=false;}
float Stage::depthSpan()const{return std::max(20.f,(float)getHeight()-(view.getSelectedId()==1?138.f:112.f));}
juce::Point<float> Stage::project(float pan,float depth)const{float z=juce::jlimit(0.f,1.f,depth/80);return {getWidth()*.5f+pan*(getWidth()*.5f-30)*(1-.28f*z),(float)getHeight()-49-z*depthSpan()+pan*pan*5};}
std::array<juce::Point<float>,vg::voices> Stage::singerPositions()const{
    auto sound=p.settings();std::array<juce::Point<float>,vg::voices> result{};
    // Display-only separation keeps coincident singers selectable. Audio positions
    // are unchanged; a selected displaced marker shows its true-position tether.
    for(int v=0;v<sound.count;++v){auto target=project(sound.voice[(size_t)v].pan,sound.voice[(size_t)v].delay);auto candidate=target;
        auto free=[&](juce::Point<float> point){for(int i=0;i<v;++i)if(point.getDistanceFrom(result[(size_t)i])<25)return false;return true;};
        for(int step=0;step<600&&!free(candidate);++step){float radius=7*std::sqrt((float)(step+1)),angle=step*2.399963f;candidate={juce::jlimit(22.f,(float)getWidth()-22,target.x+radius*std::cos(angle)),juce::jlimit(66.f,(float)getHeight()-48,target.y+radius*.7f*std::sin(angle))};}
        if(!free(candidate)){ // Guaranteed seats for pathological all-coincident input.
            std::vector<int> seats;for(int i=0;i<sound.count;++i)seats.push_back(i);std::stable_sort(seats.begin(),seats.end(),[&](int x,int y){auto a=sound.voice[(size_t)x],b=sound.voice[(size_t)y];return a.delay==b.delay?a.pan<b.pan:a.delay>b.delay;});
            int columns=std::min(6,sound.count),rows=(sound.count+columns-1)/columns;float front=(float)getHeight()-48,back=std::max(66.f,front-depthSpan());
            for(int seat=0;seat<sound.count;++seat)result[(size_t)seats[(size_t)seat]]={columns==1?getWidth()*.5f:35+(getWidth()-70.f)*(seat%columns)/(columns-1),rows==1?front:back+(front-back)*(seat/columns)/(rows-1)};
            return result;
        }
        result[(size_t)v]=candidate;
    }return result;
}
void Stage::refresh(){auto sound=p.settings();if(selected>=sound.count)selected=std::max(0,sound.count-1);mute.setEnabled(selected>=0);solo.setEnabled(selected>=0);if(selected>=0){auto voice=sound.voice[(size_t)selected];mute.setToggleState(voice.mute,juce::dontSendNotification);solo.setToggleState(voice.solo,juce::dontSendNotification);selection.setText(vg::tr(p.language.load(),"Singer")+" "+juce::String(selected+1)+"  /  "+juce::String(voice.pan,2)+"  /  "+juce::String(voice.delay,0)+" ms",juce::dontSendNotification);}repaint();}
void Stage::resized(){view.setBounds(getWidth()-122,8,110,23);selection.setBounds(16,getHeight()-30,getWidth()-118,24);mute.setBounds(getWidth()-94,getHeight()-30,34,24);solo.setBounds(getWidth()-54,getHeight()-30,34,24);}
void Stage::paint(juce::Graphics& g){VG_PALETTE
    auto t=[this](const char* text){return vg::tr(p.language.load(),text);};auto a=getLocalBounds().toFloat();
    g.setGradientFill(juce::ColourGradient(palette.panel.brighter(.08f),0,0,palette.background,0,a.getBottom(),false));g.fillRoundedRectangle(a,16);
    g.setColour(gold);g.setFont(juce::FontOptions(10,juce::Font::bold));g.drawText(t("AMPHITHEATRE"),16,10,getWidth()-150,17,juce::Justification::left);
    g.setColour(ink.withAlpha(.8f));g.setFont(juce::FontOptions(11));g.drawText(t("Drag to position. Click to select."),16,33,getWidth()-25,18,juce::Justification::left);
    // Tiered, perspective floor with a front fascia: entirely native geometry.
    for(int tier=0;tier<4;++tier){float front=tier*20.f,back=(tier+1)*20.f;auto fl=project(-1,front),fr=project(1,front),bl=project(-1,back),br=project(1,back);juce::Path floor;floor.startNewSubPath(fl);floor.quadraticTo(getWidth()*.5f,fl.y-10,fr.x,fr.y);floor.lineTo(br);floor.quadraticTo(getWidth()*.5f,br.y-10,bl.x,bl.y);floor.closeSubPath();g.setColour(palette.forest.withAlpha(.07f+.018f*tier));g.fillPath(floor);g.setColour(sage.withAlpha(.3f));g.strokePath(floor,juce::PathStrokeType(.7f));}
    for(float x:{-.75f,-.5f,-.25f,0.f,.25f,.5f,.75f}){g.setColour(sage.withAlpha(.11f));g.drawLine({project(x,0),project(x,80)},.7f);}
    auto front=project(-1,0),right=project(1,0);juce::Path fascia;fascia.startNewSubPath(front);fascia.lineTo(right);fascia.lineTo(right.translated(0,8));fascia.lineTo(front.translated(0,8));fascia.closeSubPath();g.setColour(palette.forest.withAlpha(.22f));g.fillPath(fascia);
    auto sound=p.settings();auto positions=singerPositions();std::vector<int> order;for(int v=0;v<sound.count;++v)order.push_back(v);std::stable_sort(order.begin(),order.end(),[&](int x,int y){return positions[(size_t)x].y<positions[(size_t)y].y;});
    if(selected>=0&&selected<sound.count){auto actual=project(sound.voice[(size_t)selected].pan,sound.voice[(size_t)selected].delay);if(actual.getDistanceFrom(positions[(size_t)selected])>6){g.setColour(gold.withAlpha(.65f));g.drawLine({actual,positions[(size_t)selected]},1);g.fillEllipse(actual.x-2,actual.y-2,4,4);}}
    for(int v:order){auto point=positions[(size_t)v];float z=sound.voice[(size_t)v].delay/80,r=8.4f-1.6f*z;bool audible=vg::voiceAudible(sound,v);auto colour=sound.voice[(size_t)v].mute?palette.muted:audible?palette.forest:palette.sage;float alpha=audible?1.f:.48f;
        g.setColour(juce::Colours::black.withAlpha(.25f));g.fillEllipse(point.x-r-2,point.y+8,r*2+4,6);
        if(v==selected){g.setColour(gold);g.drawEllipse(point.x-14,point.y-13,28,29,1.5f);}
        g.setGradientFill(juce::ColourGradient(colour.brighter(.4f).withAlpha(alpha),point.x-r,point.y,colour.darker(.45f).withAlpha(alpha),point.x+r,point.y,false));g.fillRoundedRectangle(point.x-r,point.y-1,r*2,13,4);g.fillEllipse(point.x-4,point.y-10,8,8);
        g.setColour(palette.background.withAlpha(.95f));g.fillRoundedRectangle(point.x-10,point.y+2,20,12,3);g.setColour(sound.voice[(size_t)v].solo?gold:ink.withAlpha(alpha));g.setFont(juce::FontOptions(10,juce::Font::bold));g.drawText(juce::String(v+1),(int)point.x-10,(int)point.y+2,20,12,juce::Justification::centred);
        if(sound.voice[(size_t)v].mute){g.setColour(palette.muted);g.drawLine(point.x-7,point.y-7,point.x+7,point.y+7,1.4f);}
    }
}
void Stage::mouseDown(const juce::MouseEvent& e){finishDrag();armed=false;auto sound=p.settings();auto positions=singerPositions();int hit=-1;float best=19;for(int v=0;v<sound.count;++v){float distance=positions[(size_t)v].getDistanceFrom(e.position);if(distance<best){best=distance;hit=v;}}if(hit<0)return;armed=true;selected=hit;origin=e.position;panStart=sound.voice[(size_t)selected].pan;depthStart=sound.voice[(size_t)selected].delay;if(onSelected)onSelected(selected);refresh();}
void Stage::mouseDrag(const juce::MouseEvent& e){if(!armed||selected<0||e.getDistanceFromDragStart()<3)return;if(!dragging){dragging=true;for(auto suffix:{"pan","delay"})p.state.getParameter("v"+juce::String(selected+1)+"_"+suffix)->beginChangeGesture();}auto delta=e.position-origin;float scale=(getWidth()*.5f-30)*(1-.28f*depthStart/80);auto set=[&](const char* suffix,float value){auto* parameter=p.state.getParameter("v"+juce::String(selected+1)+"_"+suffix);parameter->setValueNotifyingHost(parameter->convertTo0to1(value));};set("pan",juce::jlimit(-1.f,1.f,panStart+delta.x/scale));set("delay",juce::jlimit(0.f,80.f,depthStart-delta.y/depthSpan()*80));refresh();}
void Stage::mouseUp(const juce::MouseEvent&){finishDrag();armed=false;}
VoxGardenEditor::VoxGardenEditor(VoxGardenProcessor& proc):AudioProcessorEditor(proc),Thread("VoxGarden file renderer"),p(proc),stage(proc){
    look.palette=GardenPalette(p.darkMode.load(),p.themeAccent.load());const auto paper=look.palette.paper,ink=look.palette.ink,forest=look.palette.forest,sage=look.palette.sage,muted=look.palette.muted,line=look.palette.line;
    look.setColour(juce::Slider::textBoxTextColourId,ink);look.setColour(juce::Slider::textBoxOutlineColourId,juce::Colours::transparentBlack);look.setColour(juce::Slider::textBoxBackgroundColourId,juce::Colours::transparentBlack);look.setColour(juce::Slider::textBoxHighlightColourId,sage.withAlpha(.4f));look.setColour(juce::ComboBox::backgroundColourId,juce::Colours::white.withAlpha(.7f));look.setColour(juce::ComboBox::textColourId,ink);look.setColour(juce::ComboBox::outlineColourId,line);look.setColour(juce::TextButton::buttonColourId,juce::Colour(0xffe9e9df));look.setColour(juce::TextButton::textColourOffId,ink);look.setColour(juce::ToggleButton::textColourId,ink);look.setColour(juce::ToggleButton::tickColourId,forest);look.setColour(juce::PopupMenu::backgroundColourId,paper);look.setColour(juce::PopupMenu::textColourId,ink);look.setColour(juce::PopupMenu::highlightedBackgroundColourId,forest);look.setColour(juce::PopupMenu::highlightedTextColourId,paper);setLookAndFeel(&look);
    title.setText("VoxGarden",juce::dontSendNotification);title.setFont(juce::FontOptions("Georgia",35,juce::Font::plain));title.setColour(juce::Label::textColourId,ink);
    subtitle.setText("One voice. A room full of possibility.  /  Kiwi Singh",juce::dontSendNotification);subtitle.setFont(juce::FontOptions(13));subtitle.setColour(juce::Label::textColourId,muted);
    for(auto* c:std::initializer_list<juce::Component*>{&title,&subtitle,&pitch,&status,&root,&scale,&mode,&presets,&voiceCount,&stage,&formants,&render,&help,&reveal,&importFile,&aiPrint,&aiBlend,&theme,&aiCast,&raga,&ragaNotes,&languageChoice,&themeChoice,&compose,&voiceBank})addAndMakeVisible(c);
    for(int i=0;i<12;++i)root.addItem(vg::roots[(size_t)i],i+1);for(int i=0;i<9;++i)scale.addItem(vg::scales[(size_t)i],i+1);for(int i=1;i<=vg::voices;++i)voiceCount.addItem(juce::String(i)+(i==1?" voice":" voices"),i);
    mode.addItemList({"Scale harmony","MIDI chord","Chromatic intervals"},1);
    presets.addItem("Natural backing",1);presets.addItem("Unison ensemble",2);presets.addItem("Chapel",3);
    presets.addSectionHeading("Natural voices");presets.addItem("Intimate duet",6);presets.addItem("Studio quartet",7);presets.addItem("Warm chamber",8);presets.addItem("Soft alto choir",9);presets.addItem("Gospel backing",10);presets.addItem("Open fifths",11);presets.addItem("Airy unison",12);
    presets.addSectionHeading("Beyond the choir");presets.addItem("Glass garden",4);presets.addItem("Strange bloom",5);presets.addSectionHeading("Studio & harmony");
    const juce::StringArray newNames{"Close double","Wide double","Octave foundation","Soul sevenths","Suspended trio","Cathedral octaves","Low velvet","High silk","Cinematic ensemble","Extended harmony","Fifths in the mist","Drifting unison","Octave constellation","Electric quartet","Cluster bloom","Fractured choir"};
    for(int i=0;i<newNames.size();++i){if(i==8)presets.addSectionHeading("Cinematic & experimental");presets.addItem(newNames[i],13+i);}
    presets.addSectionHeading("Voice transformations");
    const juce::StringArray characters{"Female choir / male lead","Male choir / female lead","Velvet altos","Soprano halo","Baritone club","Bass cathedral","Mixed choir split","Her & him duet","Pixie chorus","Gentle giants"};
    for(int i=0;i<characters.size();++i)presets.addItem(characters[i],29+i);
    presets.addSectionHeading("Raga-inspired choirs");
    raga.addItem("Off",1);
    for(int i=0;i<8;++i){raga.addItem(vg::ragaNames[(size_t)i],i+2);
        juce::PopupMenu sounds;auto name=juce::String(vg::ragaNames[(size_t)i]);
        sounds.addItem(39+i,name+" / "+vg::ragaWestern[(size_t)i]);
        sounds.addItem(47+i,name+" / Female choir / male lead");
        sounds.addItem(55+i,name+" / Male choir / female lead");
        presets.getRootMenu()->addSubMenu(name+" / "+vg::ragaWestern[(size_t)i],sounds);
    }
    presets.addSectionHeading("Large ensembles");presets.addItem("Abbey Ensemble 24",63);presets.addItem("Chamber Ensemble 16",64);presets.addItem("Velvet Abbey 24",65);
    for(int family=0;family<4;++family){juce::PopupMenu group;for(int item=0;item<3;++item){int index=family*3+item;group.addItem(66+index,vg::genrePresets[(size_t)index].name);}presets.getRootMenu()->addSubMenu(vg::genrePresets[(size_t)(family*3)].family,group);}
    voiceBank.setComponentID("voiceBank");voiceBank.addItem("Voices 1-8",1);voiceBank.addItem("Voices 9-16",2);voiceBank.addItem("Voices 17-24",3);voiceBank.setSelectedId(1,juce::dontSendNotification);voiceBank.onChange=[this]{resized();repaint();};
    stage.setComponentID("soundstage");stage.onSelected=[this](int singer){voiceBank.setSelectedId(singer/8+1,juce::sendNotificationSync);repaint();};
    voiceCount.setComponentID("voiceCount");voiceCount.onChange=[this]{p.setChoirSize(voiceCount.getSelectedId());};
    raga.onChange=[this]{if(raga.getSelectedId()>1)p.setValue("mode",0);};
    for(int i=0;i<5;++i)languageChoice.addItem(juce::String::fromUTF8(vg::languageNames[(size_t)i]),i+1);
    languageChoice.setComponentID("languageChoice");raga.setComponentID("ragaChoice");
    languageChoice.setSelectedId(p.language.load()+1,juce::dontSendNotification);
    languageChoice.onChange=[this]{p.language.store(languageChoice.getSelectedId()-1);applyLanguage();p.updateHostDisplay(juce::AudioProcessor::ChangeDetails{}.withNonParameterStateChanged(true));};
    aiCast.addItemList({"Original mixed cast","Female voices","Male voices","Alternating female / male"},1);
    aiCast.setTooltip("Reference voices for AI printing. DSP preview uses voicing and formant colour. New character presets set both. No input voice classification is performed.");
    presets.setComponentID("presets");presets.setText("Explore the choir...",juce::dontSendNotification);presets.onChange=[this]{p.applyPreset(presets.getSelectedId()-1);};
    for(auto pair:std::initializer_list<std::pair<const char*,juce::ComboBox*>>{{"root",&root},{"scale",&scale},{"mode",&mode},{"aiCast",&aiCast},{"raga",&raga}})choices.push_back(std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.state,pair.first,*pair.second));
    aiAttachment=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.state,"aiPrint",aiPrint);
    aiBlend.setLookAndFeel(&look);aiBlend.setSliderStyle(juce::Slider::LinearHorizontal);aiBlend.setTextBoxStyle(juce::Slider::TextBoxRight,false,42,20);aiBlend.setTooltip("AI singer blend for printed audio. Live playback remains the DSP preview.");aiBlendAttachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.state,"aiBlend",aiBlend);
    formantAttachment=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.state,"formantLock",formants);
    addControl("mix","CHOIR MIX");addControl("gain","OUTPUT");addControl("correction","SCALE LOCK");addControl("glide","GLIDE / MS");addControl("detune","DETUNE / CT");addControl("vibrato","VIBRATO / CT");addControl("human","TIMING / MS");addControl("width","WIDTH");addControl("space","ROOM");addControl("weird","MUTATION");
    for(int i=1;i<=vg::voices;++i){auto id="v"+juce::String(i)+"_";addControl(id+"degree","HARMONY");addControl(id+"level","LEVEL");addControl(id+"pan","POSITION");addControl(id+"formant","COLOUR / ST");addControl(id+"delay","DEPTH / MS");}
    for(int v=0;v<vg::voices;++v){auto prefix="v"+juce::String(v+1)+"_";
        for(int k=0;k<2;++k){auto& button=k==0?muteButtons[(size_t)v]:soloButtons[(size_t)v];
            button.setButtonText(k==0?"M":"S");button.setClickingTogglesState(true);button.setComponentID(prefix+(k==0?"mute":"solo"));
            button.setTitle("Voice "+juce::String(v+1)+(k==0?" mute":" solo"));button.setTooltip(k==0?"Mute this choir voice. Level is preserved; mute overrides solo.":"Solo this choir voice. Multiple solos combine. Dry lead is controlled by Choir Mix.");
            addAndMakeVisible(button);auditionAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.state,prefix+(k==0?"mute":"solo"),button));
        }
    }
    render.setButtonText("Render vocal + stems");render.setColour(juce::TextButton::buttonColourId,forest);render.setColour(juce::TextButton::textColourOffId,paper);render.onClick=[this]{beginRender();};
    importFile.onClick=[this]{chooseFile();};reveal.onClick=[this]{if(outputFolder.exists())outputFolder.revealToUser();};reveal.setEnabled(false);
    help.onClick=[this]{juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::InfoIcon,t("Your choir in Ableton Live"),vg::helpText(p.language.load()));};
    status.setColour(juce::Label::textColourId,muted);status.setFont(juce::FontOptions(12));pitch.setColour(juce::Label::textColourId,forest);pitch.setFont(juce::FontOptions(12));status.setText(vg::aiReady()?"AI singers are ready. Enable AI singers on print for neural voice conversion.":"Capture the vocal from this track, then print your choir and individual parts.",juce::dontSendNotification);
    capturePending=p.capture.getPhase()==TrackCapture::starting||p.capture.getPhase()==TrackCapture::recording||p.capture.getPhase()==TrackCapture::stopping||p.capture.getPhase()==TrackCapture::complete;
    themeChoice.addItemList({"Garden","Rose","Ocean","Sunset"},1);themeChoice.setComponentID("themeChoice");
    themeChoice.onChange=[this]{p.setAppearance(themeChoice.getSelectedId()-1,p.darkMode.load());applyTheme();p.updateHostDisplay(juce::AudioProcessor::ChangeDetails{}.withNonParameterStateChanged(true));};
    compose.onClick=[this]{if(composer){composer->setVisible(true);composer->toFront(true);return;}composer=showComposer(p);};
    theme.onClick=[this]{p.setAppearance(p.themeAccent.load(),theme.getToggleState());applyTheme();p.updateHostDisplay(juce::AudioProcessor::ChangeDetails{}.withNonParameterStateChanged(true));};applyTheme();
    for(auto* combo:{&root,&scale,&mode,&presets,&voiceCount,&aiCast,&raga,&themeChoice,&voiceBank})englishMenus.emplace(combo,*combo->getRootMenu());
    ragaNotes.setFont(juce::FontOptions(12));ragaNotes.setColour(juce::Label::textColourId,muted);
    applyLanguage();setSize(1120,900);timerCallback();startTimerHz(12);
}
VoxGardenEditor::~VoxGardenEditor(){composer.reset();signalThreadShouldExit();stopThread(-1);p.printInProgress=false;setLookAndFeel(nullptr);}
void VoxGardenEditor::addControl(const juce::String& id,const juce::String& name){ VG_PALETTE auto c=std::make_unique<Control>();c->label.setText(name,juce::dontSendNotification);c->label.setColour(juce::Label::textColourId,muted);c->label.setFont(juce::FontOptions(9.5f,juce::Font::bold));c->label.setJustificationType(juce::Justification::centred);bool global=controls.size()<10;c->slider.setLookAndFeel(&look);c->slider.setComponentID(id);c->slider.setSliderStyle(global?juce::Slider::RotaryHorizontalVerticalDrag:juce::Slider::LinearHorizontal);c->slider.setTextBoxStyle(juce::Slider::TextBoxBelow,false,76,19);c->slider.setDoubleClickReturnValue(true,p.state.getParameter(id)->convertFrom0to1(p.state.getParameter(id)->getDefaultValue()));addAndMakeVisible(c->label);addAndMakeVisible(c->slider);c->attachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.state,id,c->slider);controls.push_back(std::move(c));}
void VoxGardenEditor::paint(juce::Graphics& g){ VG_PALETTE
    g.setGradientFill(juce::ColourGradient(paper,0,0,palette.background,1100,900,false));g.fillAll();
    // Small botanical mark; drawn natively so it stays crisp on Retina displays.
    g.setColour(juce::Colour(0xff294f46));g.fillRoundedRectangle(24,23,47,47,14);g.setColour(juce::Colour(0xffd4dfbe));juce::Path stem;stem.startNewSubPath(47,61);stem.quadraticTo(42,42,57,31);g.strokePath(stem,juce::PathStrokeType(1.5f));g.fillEllipse(34,35,11,19);g.fillEllipse(49,31,10,16);
    caption(g,t("KIWI SINGH  /  VOXGARDEN 2.1 PREVIEW"),83,76,300,muted);caption(g,t("START WITH A SOUND"),768,17,320,muted);caption(g,t("AI VOICE CAST"),768,78,100,muted);
    g.setColour(palette.panel);g.fillRoundedRectangle(20,103,1080,126,13);caption(g,t("ROOT"),36,111,105,muted);caption(g,t("SCALE"),147,111,170,muted);caption(g,t("HARMONY MODE"),345,111,190,muted);caption(g,t("CHOIR SIZE"),565,111,125,muted);
    caption(g,t("RAGA / NOTE SET"),36,166,300,muted);
    g.setColour(palette.panel);g.fillRoundedRectangle(505,242,595,215,16);
    caption(g,t("YOUR ENSEMBLE"),25,481,300,muted);g.setColour(muted);g.setFont(12);g.drawText(t("0 = unison   /   2 = third   /   4 = fifth in seven-note scales"),440,478,650,22,juce::Justification::right);
    g.setColour(palette.panel);g.fillRoundedRectangle(20,512,1080,294,14);
    auto ensemble=p.settings();
    for(int row=0;row<8;++row){int v=(voiceBank.getSelectedId()-1)*8+row;int y=540+row*32;bool active=vg::voiceAudible(ensemble,v);g.setColour(v%2==0?forest.withAlpha(.025f):juce::Colours::transparentBlack);g.fillRoundedRectangle(27,(float)y,1066,31,5);g.setColour(active?forest.withAlpha(.09f):line.withAlpha(.4f));g.fillEllipse(36,(float)y+7,22,22);g.setColour(active?forest:muted.withAlpha(.4f));g.setFont(juce::FontOptions(11,juce::Font::bold));g.drawText(juce::String(v+1),36,y+7,22,22,juce::Justification::centred);if(v==stage.selectedSinger()){g.setColour(gold.withAlpha(.7f));g.drawRoundedRectangle(juce::Rectangle<float>(28,(float)y,1062,31),5,1);}}
    g.setColour(line);g.drawHorizontalLine(835,24,1096);
}
void VoxGardenEditor::resized(){
    theme.setBounds(610,9,135,25);themeChoice.setBounds(610,37,135,27);compose.setBounds(345,464,145,25);title.setBounds(82,17,420,46);subtitle.setBounds(84,58,510,22);presets.setBounds(768,39,328,33);aiCast.setBounds(870,76,226,23);
    languageChoice.setBounds(610,70,135,26);raga.setBounds(36,185,300,30);ragaNotes.setBounds(352,174,730,48);
    root.setBounds(36,130,99,30);scale.setBounds(147,130,185,30);mode.setBounds(345,130,205,30);voiceCount.setBounds(565,130,130,30);formants.setBounds(712,130,190,30);pitch.setBounds(900,130,194,30);
    voiceBank.setBounds(178,478,158,27);stage.setBounds(20,242,470,215);
    for(int i=0;i<10;++i){int x=511+(i%5)*117,y=253+(i/5)*99;controls[(size_t)i]->label.setBounds(x,y,110,16);controls[(size_t)i]->slider.setBounds(x,y+17,110,78);}
    int bank=std::max(0,voiceBank.getSelectedId()-1);
    for(int v=0;v<vg::voices;++v){int row=v%8;bool visible=v/8==bank;for(int k=0;k<5;++k){auto& c=*controls[(size_t)(10+v*5+k)];int x=137+k*191;c.label.setVisible(visible&&row==0);c.label.setBounds(x,516,183,20);c.slider.setVisible(visible);c.slider.setBounds(x,540+row*32,183,30);}
        muteButtons[(size_t)v].setVisible(visible);soloButtons[(size_t)v].setVisible(visible);muteButtons[(size_t)v].setBounds(66,543+row*32,27,24);soloButtons[(size_t)v].setBounds(99,543+row*32,27,24);}
    aiPrint.setBounds(588,851,173,33);aiBlend.setBounds(770,851,150,33);
    status.setBounds(25,809,1070,24);render.setBounds(24,851,244,33);importFile.setBounds(280,851,142,33);reveal.setBounds(434,851,142,33);help.setBounds(942,851,154,33);
}
void VoxGardenEditor::timerCallback(){
    if(lastLanguage!=p.language.load())applyLanguage();
    if(theme.getToggleState()!=p.darkMode.load()||lastAccent!=p.themeAccent.load())applyTheme();
    VG_PALETTE
    auto note=p.engine.detectedNote.load();auto conf=p.engine.pitchConfidence.load();int n=(int)std::round(note);pitch.setText(conf>.8f?"Input  "+juce::String(vg::roots[(size_t)((n%12+12)%12)])+juce::String(n/12-1)+" / "+juce::String((note-n)*100,0)+" ct":"Listening for your vocal",juce::dontSendNotification);
    aiBlend.setEnabled(p.settings().aiPrint);
    int count=p.settings().count;voiceCount.setSelectedId(count,juce::dontSendNotification);if(lastCount!=count){lastCount=count;for(int v=0;v<vg::voices;++v)for(int k=0;k<5;++k)controls[(size_t)(10+v*5+k)]->slider.setEnabled(v<count);repaint();}
    for(int v=0;v<vg::voices;++v){muteButtons[(size_t)v].setEnabled(v<count);soloButtons[(size_t)v].setEnabled(v<count);}
    repaint();stage.refresh();
    compose.setEnabled(!rendering.load()&&!capturePending);
    auto phase=p.capture.getPhase();
    if(capturePending){
        if(phase==TrackCapture::starting){render.setEnabled(false);status.setText("Preparing channel capture...",juce::dontSendNotification);}
        else if(phase==TrackCapture::recording){render.setEnabled(true);render.setButtonText("Stop + render");render.setColour(juce::TextButton::buttonColourId,juce::Colour(0xff9b513b));status.setText("CAPTURING DRY INPUT   /   "+juce::String(p.capture.seconds(),1)+" sec   /   Play the vocal passage, then click Stop + render.",juce::dontSendNotification);}
        else if(phase==TrackCapture::stopping){render.setEnabled(false);status.setText("Finishing your captured take...",juce::dontSendNotification);}
        else if(phase==TrackCapture::complete){capturePending=false;auto file=p.capture.file();p.capture.acknowledge();renderSettings=p.captureRenderSettings;renderFile(file,true);}
        else if(phase==TrackCapture::idle){capturePending=false;render.setEnabled(true);importFile.setEnabled(true);render.setButtonText("Render vocal + stems");render.setColour(juce::TextButton::buttonColourId,forest);status.setText("Audio setup changed. Any captured take was retained on disk.",juce::dontSendNotification);}
        else if(phase==TrackCapture::failed){capturePending=false;render.setEnabled(true);importFile.setEnabled(true);render.setButtonText("Render vocal + stems");render.setColour(juce::TextButton::buttonColourId,forest);reveal.setEnabled(outputFolder.exists());status.setText(p.capture.errorMessage(),juce::dontSendNotification);}
    }
    if(rendering.load()){
        juce::String detail="Mixing choir and stems";
        if(renderSettings.aiPrint && renderSettings.aiBlend>0 && progress.load()<.75f){auto report=juce::JSON::parse(outputFolder.getChildFile("AI Sources/AI Status.json"));detail=report.getProperty("message","Starting local AI renderer").toString();}
        status.setText(detail+"  /  "+juce::String(progress.load()*100,0)+"%  /  Dry monitoring",juce::dontSendNotification);
    }
    else if(!isThreadRunning()&&!result.isEmpty()){status.setText(result,juce::dontSendNotification);result.clear();render.setEnabled(true);importFile.setEnabled(true);render.setButtonText("Render vocal + stems");render.setColour(juce::TextButton::buttonColourId,forest);reveal.setEnabled(outputFolder.exists());}
    auto musical=p.settings();
    scale.setEnabled(musical.raga==0);
    if(musical.raga>0){int index=musical.raga-1;auto western=t(vg::ragaWestern[(size_t)index]);scale.setText(western,juce::dontSendNotification);
        juce::StringArray notes;for(const auto& name:vg::ragaNoteNames(musical.root,musical.raga))notes.add(name);
        constexpr int parentOffsets[8]={7,0,10,8,-1,8,0,0};int parent=parentOffsets[index];
        auto related=parent<0?t("No ordinary major/minor equivalent"):t("Related major notes: ")+juce::String(vg::roots[(size_t)((musical.root+parent)%12)])+(index==1||index>=5?t(" (subset)"):juce::String{});
        ragaNotes.setText(t("Sa = ")+juce::String(vg::roots[(size_t)musical.root])+" / "+western+" / "+t("Notes: ")+notes.joinIntoString(" ")+"\n"+related+" · "+t("Note-set equivalent; phrasing and ornaments are not generated."),juce::dontSendNotification);
    }else{scale.setSelectedId((int)p.state.getRawParameterValue("scale")->load()+1,juce::dontSendNotification);ragaNotes.setText(t("Choose a raga or keep your Western scale."),juce::dontSendNotification);}
    vg::localiseComponent(*this,p.language.load());
}
void VoxGardenEditor::beginRender(){
    if(rendering.load()){signalThreadShouldExit();render.setEnabled(false);status.setText("Cancelling print; your source take will be kept.",juce::dontSendNotification);return;}
    if(p.capture.getPhase()==TrackCapture::recording){renderSettings=p.settings();p.captureRenderSettings=renderSettings;p.capture.requestStop();return;}
    if(rendering.load()||isThreadRunning()||capturePending)return;
    if(p.settings().aiPrint && p.settings().aiBlend>0 && !vg::aiReady()){status.setText("AI singers need the free local runtime. Run Setup AI.command from the package first.",juce::dontSendNotification);return;}
    if(p.settings().mode==1){status.setText("Track rendering uses Scale harmony or Chromatic mode. Print MIDI-chord performances with Live's audio recorder.",juce::dontSendNotification);return;}
    auto base=juce::File::getSpecialLocation(juce::File::userMusicDirectory).getChildFile("VoxGarden Renders");
    auto take=base.getNonexistentChildFile("Choir Take "+juce::Time::getCurrentTime().formatted("%Y-%m-%d %H-%M-%S"),"");
    if(!take.createDirectory()){status.setText("Could not create a take folder in Music/VoxGarden Renders.",juce::dontSendNotification);return;}
    if(p.capture.begin(take.getChildFile("Input Vocal.wav"))){outputFolder=take;capturePending=true;renderSettings=p.settings();p.captureRenderSettings=renderSettings;importFile.setEnabled(false);reveal.setEnabled(false);}
    else status.setText("Start audio playback in your host before capturing.",juce::dontSendNotification);
}
void VoxGardenEditor::chooseFile(){chooser=std::make_unique<juce::FileChooser>(t("Render a vocal file"),juce::File{},"*.wav;*.aif;*.aiff;*.flac");chooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[safe=juce::Component::SafePointer<VoxGardenEditor>(this)](const juce::FileChooser& c){if(safe&&c.getResult().existsAsFile())safe->renderFile(c.getResult());});}
bool VoxGardenEditor::isInterestedInFileDrag(const juce::StringArray& files){return files.size()==1&&juce::File(files[0]).hasFileExtension("wav;aif;aiff;flac;mid;midi")&&!rendering.load()&&!capturePending;}
void VoxGardenEditor::filesDropped(const juce::StringArray& files,int,int){if(isInterestedInFileDrag(files)){auto file=juce::File(files[0]);if(file.hasFileExtension("mid;midi")){if(composer){composer->setVisible(true);composer->toFront(true);if(auto* content=dynamic_cast<ScoreComposer*>(composer->getContentComponent()))content->filesDropped(files,0,0);return;}composer=showComposer(p,file);}else renderFile(file);}}
void VoxGardenEditor::renderFile(const juce::File& f,bool trackTake){ VG_PALETTE
    if(capturePending&&!trackTake){status.setText(t("Stop the channel capture before rendering another file."),juce::dontSendNotification);return;}
    if(isThreadRunning()||rendering.load())return;inputFile=f;
    if(!trackTake)renderSettings=p.settings();
    if(renderSettings.aiPrint && renderSettings.aiBlend>0 && !vg::aiReady()){status.setText("AI runtime is not ready. Turn off AI singers for a DSP print, or run Setup AI.command.",juce::dontSendNotification);render.setEnabled(true);importFile.setEnabled(true);render.setButtonText("Render vocal + stems");return;}
    if(renderSettings.mode==1){status.setText("Choose Scale harmony or Chromatic mode to render a vocal.",juce::dontSendNotification);render.setEnabled(true);importFile.setEnabled(true);render.setButtonText("Render vocal + stems");render.setColour(juce::TextButton::buttonColourId,forest);return;}
    outputFolder=trackTake?f.getParentDirectory():f.getParentDirectory().getNonexistentChildFile(f.getFileNameWithoutExtension()+" - VoxGarden","");progress=0;rendering=true;p.printInProgress=true;render.setEnabled(false);importFile.setEnabled(false);reveal.setEnabled(false);render.setButtonText("Cancel print");render.setEnabled(true);startThread();
}
void VoxGardenEditor::run(){
    std::vector<juce::File> aiFiles;
    if(renderSettings.aiPrint && renderSettings.aiBlend>0){result=vg::createAISingers(inputFile,outputFolder,renderSettings,progress,[this]{return threadShouldExit();},aiFiles);if(result.isNotEmpty()){p.printInProgress=false;rendering=false;return;}}
    result=renderVocal(inputFile,outputFolder,renderSettings,progress,[this]{return threadShouldExit();},aiFiles);p.printInProgress=false;rendering=false;
}

void VoxGardenEditor::applyTheme(){
    look.palette=GardenPalette(p.darkMode.load(),p.themeAccent.load());auto c=look.palette;
    theme.setToggleState(p.darkMode.load(),juce::dontSendNotification);lastAccent=p.themeAccent.load();themeChoice.setSelectedId(lastAccent+1,juce::dontSendNotification);
    look.setColour(juce::Slider::textBoxTextColourId,c.ink);look.setColour(juce::Slider::textBoxHighlightColourId,c.sage.withAlpha(.4f));
    look.setColour(juce::TooltipWindow::backgroundColourId,c.panel);look.setColour(juce::TooltipWindow::textColourId,c.ink);look.setColour(juce::TooltipWindow::outlineColourId,c.line);
    look.setColour(juce::ComboBox::backgroundColourId,c.panel);look.setColour(juce::ComboBox::textColourId,c.ink);
    look.setColour(juce::TextButton::buttonColourId,c.panel);look.setColour(juce::TextButton::textColourOffId,c.ink);
    look.setColour(juce::PopupMenu::backgroundColourId,c.paper);look.setColour(juce::PopupMenu::textColourId,c.ink);
    look.setColour(juce::PopupMenu::highlightedBackgroundColourId,c.forest);look.setColour(juce::PopupMenu::highlightedTextColourId,c.paper);
    title.setColour(juce::Label::textColourId,c.ink);subtitle.setColour(juce::Label::textColourId,c.muted);
    ragaNotes.setColour(juce::Label::textColourId,c.muted);status.setColour(juce::Label::textColourId,c.muted);pitch.setColour(juce::Label::textColourId,c.forest);
    for(auto& control:controls)control->label.setColour(juce::Label::textColourId,c.muted);
    render.setColour(juce::TextButton::buttonColourId,c.forest);render.setColour(juce::TextButton::textColourOffId,c.paper);
    for(int v=0;v<vg::voices;++v)for(int k=0;k<2;++k){auto& button=k==0?muteButtons[(size_t)v]:soloButtons[(size_t)v];
        button.setColour(juce::TextButton::buttonColourId,c.paper);button.setColour(juce::TextButton::buttonOnColourId,k==0?c.gold:c.forest);
        button.setColour(juce::TextButton::textColourOffId,c.muted);button.setColour(juce::TextButton::textColourOnId,(k==0 || p.darkMode.load())?juce::Colour(0xff151e20):juce::Colours::white);
    }
    sendLookAndFeelChange();repaint();
}

void VoxGardenEditor::applyLanguage(){
    stage.setLanguage();
    lastLanguage=p.language.load();look.language=lastLanguage;languageChoice.setSelectedId(lastLanguage+1,juce::dontSendNotification);
    for(auto& pair:englishMenus){auto* combo=pair.first;int selected=combo->getSelectedId();*combo->getRootMenu()=pair.second;
        juce::PopupMenu::MenuItemIterator iterator(*combo->getRootMenu(),true);while(iterator.next()){auto& item=iterator.getItem();item.text=t(item.text);}
        if(selected>0)combo->setText(combo->getItemText(combo->indexOfItemId(selected)),juce::dontSendNotification);
    }
    for(int count=1;count<=vg::voices;++count)voiceCount.changeItemText(count,vg::voiceCountText(lastLanguage,count));
    if(presets.getSelectedId()==0)presets.setText(t("Explore the choir..."),juce::dontSendNotification);
    for(auto& control:controls)control->label.setFont(juce::FontOptions(lastLanguage==1?12.f:9.5f,juce::Font::bold));
    subtitle.setFont(juce::FontOptions(lastLanguage==1?15:13));status.setFont(juce::FontOptions(lastLanguage==1?14:12));ragaNotes.setFont(juce::FontOptions(lastLanguage==1?14:12));
    vg::localiseComponent(*this,lastLanguage);sendLookAndFeelChange();repaint();
}
