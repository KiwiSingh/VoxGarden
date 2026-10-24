#include "Editor.h"
#include "Offline.h"
#include "AI.h"
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
void Stage::paint(juce::Graphics& g){ VG_PALETTE
    auto t=[this](const juce::String& text){return vg::tr(p.language.load(),text);};
    auto a=getLocalBounds().toFloat();g.setGradientFill(juce::ColourGradient(juce::Colour(0xff25463f),0,0,juce::Colour(0xff18382f),a.getRight(),a.getBottom(),false));g.fillRoundedRectangle(a,16);
    g.setColour(gold);g.setFont(juce::FontOptions(10,juce::Font::bold));g.drawText(t("THE SOUNDSTAGE"),18,12,200,18,juce::Justification::left);
    g.setColour(juce::Colour(0xffb7cabc));g.setFont(p.language.load()==1?13:11);g.drawText(t("Drag each singer to place their voice"),18,33,330,18,juce::Justification::left);
    for(int i=0;i<5;++i){juce::Path arc;float y=70.f+i*32;arc.startNewSubPath(14,y+25);arc.quadraticTo(a.getCentreX(),y-28,a.getRight()-14,y+25);g.setColour(sage.withAlpha(.15f));g.strokePath(arc,juce::PathStrokeType(.8f));}
    auto area=getLocalBounds().reduced(35,25).withTrimmedTop(40).withTrimmedBottom(10).toFloat();auto s=p.settings();
    for(int v=0;v<8;++v){auto vs=s.voice[(size_t)v];bool active=vg::voiceAudible(s,v) && vs.level>.001f;float x=area.getCentreX()+vs.pan*area.getWidth()*.5f,y=area.getY()+vs.delay/80*area.getHeight();
        if(active){g.setColour(gold.withAlpha(.07f));g.fillEllipse(x-21,y-21,42,42);g.setColour(ink.withAlpha(.25f));g.fillEllipse(x-12,y-9,26,26);}
        g.setColour(active?juce::Colour(0xffdce9d7):sage.withAlpha(.17f));g.fillEllipse(x-12,y-12,24,24);g.setColour(active?juce::Colour(0xff294f46):sage.withAlpha(.4f));g.setFont(juce::FontOptions(11,juce::Font::bold));g.drawText(juce::String(v+1),(int)x-12,(int)y-12,24,24,juce::Justification::centred);
    }
    g.setColour(sage);g.setFont(p.language.load()==1?11:9);g.drawText(t("L"),17,getHeight()-23,20,14,juce::Justification::left);g.drawText(t("R"),getWidth()-35,getHeight()-23,20,14,juce::Justification::right);g.drawText(t("CLOSE"),getWidth()-75,55,58,14,juce::Justification::right);g.drawText(t("DISTANT"),(getWidth()-80)/2,getHeight()-25,80,14,juce::Justification::centred);
}
void Stage::mouseDown(const juce::MouseEvent& e){
    auto s=p.settings();auto a=getLocalBounds().reduced(35,25).withTrimmedTop(40).withTrimmedBottom(10).toFloat();float best=10000;
    for(int i=0;i<s.count;++i){auto v=s.voice[(size_t)i];auto pt=juce::Point<float>{a.getCentreX()+v.pan*a.getWidth()*.5f,a.getY()+v.delay/80*a.getHeight()};float d=pt.getDistanceFrom(e.position);if(d<best){best=d;selected=i;}}
    if(best>30){selected=-1;return;}for(auto suffix:{"pan","delay"})p.state.getParameter("v"+juce::String(selected+1)+"_"+suffix)->beginChangeGesture();mouseDrag(e);
}
void Stage::mouseDrag(const juce::MouseEvent& e){if(selected<0)return;auto a=getLocalBounds().reduced(35,25).withTrimmedTop(40).withTrimmedBottom(10).toFloat();auto prefix="v"+juce::String(selected+1)+"_";
    auto set=[&](const char* id,float val){auto* param=p.state.getParameter(prefix+id);param->setValueNotifyingHost(param->convertTo0to1(val));};set("pan",juce::jlimit(-1.f,1.f,(e.position.x-a.getCentreX())/(a.getWidth()*.5f)));set("delay",juce::jlimit(0.f,80.f,(e.position.y-a.getY())/a.getHeight()*80));repaint();}
void Stage::mouseUp(const juce::MouseEvent&){if(selected>=0)for(auto suffix:{"pan","delay"})p.state.getParameter("v"+juce::String(selected+1)+"_"+suffix)->endChangeGesture();selected=-1;}
VoxGardenEditor::VoxGardenEditor(VoxGardenProcessor& proc):AudioProcessorEditor(proc),Thread("VoxGarden file renderer"),p(proc),stage(proc){
    look.palette=GardenPalette(p.darkMode.load());const auto paper=look.palette.paper,ink=look.palette.ink,forest=look.palette.forest,sage=look.palette.sage,muted=look.palette.muted,line=look.palette.line;
    look.setColour(juce::Slider::textBoxTextColourId,ink);look.setColour(juce::Slider::textBoxOutlineColourId,juce::Colours::transparentBlack);look.setColour(juce::Slider::textBoxBackgroundColourId,juce::Colours::transparentBlack);look.setColour(juce::Slider::textBoxHighlightColourId,sage.withAlpha(.4f));look.setColour(juce::ComboBox::backgroundColourId,juce::Colours::white.withAlpha(.7f));look.setColour(juce::ComboBox::textColourId,ink);look.setColour(juce::ComboBox::outlineColourId,line);look.setColour(juce::TextButton::buttonColourId,juce::Colour(0xffe9e9df));look.setColour(juce::TextButton::textColourOffId,ink);look.setColour(juce::ToggleButton::textColourId,ink);look.setColour(juce::ToggleButton::tickColourId,forest);look.setColour(juce::PopupMenu::backgroundColourId,paper);look.setColour(juce::PopupMenu::textColourId,ink);look.setColour(juce::PopupMenu::highlightedBackgroundColourId,forest);look.setColour(juce::PopupMenu::highlightedTextColourId,paper);setLookAndFeel(&look);
    title.setText("VoxGarden",juce::dontSendNotification);title.setFont(juce::FontOptions("Georgia",35,juce::Font::plain));title.setColour(juce::Label::textColourId,ink);
    subtitle.setText("One voice. A room full of possibility.  /  Kiwi Singh",juce::dontSendNotification);subtitle.setFont(juce::FontOptions(13));subtitle.setColour(juce::Label::textColourId,muted);
    for(auto* c:std::initializer_list<juce::Component*>{&title,&subtitle,&pitch,&status,&root,&scale,&mode,&presets,&voiceCount,&stage,&formants,&render,&help,&reveal,&importFile,&aiPrint,&aiBlend,&theme,&aiCast,&raga,&ragaNotes,&languageChoice})addAndMakeVisible(c);
    for(int i=0;i<12;++i)root.addItem(vg::roots[(size_t)i],i+1);for(int i=0;i<9;++i)scale.addItem(vg::scales[(size_t)i],i+1);for(int i=1;i<=8;++i)voiceCount.addItem(juce::String(i)+(i==1?" voice":" voices"),i);
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
    raga.onChange=[this]{if(raga.getSelectedId()>1)p.setValue("mode",0);};
    for(int i=0;i<5;++i)languageChoice.addItem(juce::String::fromUTF8(vg::languageNames[(size_t)i]),i+1);
    languageChoice.setComponentID("languageChoice");raga.setComponentID("ragaChoice");
    languageChoice.setSelectedId(p.language.load()+1,juce::dontSendNotification);
    languageChoice.onChange=[this]{p.language.store(languageChoice.getSelectedId()-1);applyLanguage();};
    aiCast.addItemList({"Original mixed cast","Female voices","Male voices","Alternating female / male"},1);
    aiCast.setTooltip("Reference voices for AI printing. DSP preview uses voicing and formant colour. New character presets set both. No input voice classification is performed.");
    presets.setComponentID("presets");presets.setText("Explore the choir...",juce::dontSendNotification);presets.onChange=[this]{p.applyPreset(presets.getSelectedId()-1);};
    for(auto pair:std::initializer_list<std::pair<const char*,juce::ComboBox*>>{{"root",&root},{"scale",&scale},{"mode",&mode},{"voiceCount",&voiceCount},{"aiCast",&aiCast},{"raga",&raga}})choices.push_back(std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.state,pair.first,*pair.second));
    aiAttachment=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.state,"aiPrint",aiPrint);
    aiBlend.setLookAndFeel(&look);aiBlend.setSliderStyle(juce::Slider::LinearHorizontal);aiBlend.setTextBoxStyle(juce::Slider::TextBoxRight,false,42,20);aiBlend.setTooltip("AI singer blend for printed audio. Live playback remains the DSP preview.");aiBlendAttachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.state,"aiBlend",aiBlend);
    formantAttachment=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.state,"formantLock",formants);
    addControl("mix","CHOIR MIX");addControl("gain","OUTPUT");addControl("correction","SCALE LOCK");addControl("glide","GLIDE / MS");addControl("detune","DETUNE / CT");addControl("vibrato","VIBRATO / CT");addControl("human","TIMING / MS");addControl("width","WIDTH");addControl("space","ROOM");addControl("weird","MUTATION");
    for(int i=1;i<=8;++i){auto id="v"+juce::String(i)+"_";addControl(id+"degree","HARMONY");addControl(id+"level","LEVEL");addControl(id+"pan","POSITION");addControl(id+"formant","COLOUR / ST");addControl(id+"delay","DEPTH / MS");}
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
    theme.onClick=[this]{p.darkMode.store(theme.getToggleState());applyTheme();};applyTheme();
    for(auto* combo:{&root,&scale,&mode,&presets,&voiceCount,&aiCast,&raga})englishMenus.emplace(combo,*combo->getRootMenu());
    ragaNotes.setFont(juce::FontOptions(12));ragaNotes.setColour(juce::Label::textColourId,muted);
    applyLanguage();setSize(1120,900);timerCallback();startTimerHz(12);
}
VoxGardenEditor::~VoxGardenEditor(){signalThreadShouldExit();stopThread(-1);p.printInProgress=false;setLookAndFeel(nullptr);}
void VoxGardenEditor::addControl(const juce::String& id,const juce::String& name){ VG_PALETTE auto c=std::make_unique<Control>();c->label.setText(name,juce::dontSendNotification);c->label.setColour(juce::Label::textColourId,muted);c->label.setFont(juce::FontOptions(9.5f,juce::Font::bold));c->label.setJustificationType(juce::Justification::centred);bool global=controls.size()<10;c->slider.setLookAndFeel(&look);c->slider.setComponentID(id);c->slider.setSliderStyle(global?juce::Slider::RotaryHorizontalVerticalDrag:juce::Slider::LinearHorizontal);c->slider.setTextBoxStyle(juce::Slider::TextBoxBelow,false,76,19);c->slider.setDoubleClickReturnValue(true,p.state.getParameter(id)->convertFrom0to1(p.state.getParameter(id)->getDefaultValue()));addAndMakeVisible(c->label);addAndMakeVisible(c->slider);c->attachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.state,id,c->slider);controls.push_back(std::move(c));}
void VoxGardenEditor::paint(juce::Graphics& g){ VG_PALETTE
    g.setGradientFill(juce::ColourGradient(paper,0,0,palette.background,1100,900,false));g.fillAll();
    // Small botanical mark; drawn natively so it stays crisp on Retina displays.
    g.setColour(juce::Colour(0xff294f46));g.fillRoundedRectangle(24,23,47,47,14);g.setColour(juce::Colour(0xffd4dfbe));juce::Path stem;stem.startNewSubPath(47,61);stem.quadraticTo(42,42,57,31);g.strokePath(stem,juce::PathStrokeType(1.5f));g.fillEllipse(34,35,11,19);g.fillEllipse(49,31,10,16);
    caption(g,t("KIWI SINGH  /  VOXGARDEN 1.0"),83,76,300,muted);caption(g,t("START WITH A SOUND"),768,17,320,muted);caption(g,t("AI VOICE CAST"),768,78,100,muted);
    g.setColour(palette.panel);g.fillRoundedRectangle(20,103,1080,126,13);caption(g,t("ROOT"),36,111,105,muted);caption(g,t("SCALE"),147,111,170,muted);caption(g,t("HARMONY MODE"),345,111,190,muted);caption(g,t("CHOIR SIZE"),565,111,125,muted);
    caption(g,t("RAGA / NOTE SET"),36,166,300,muted);
    g.setColour(palette.panel);g.fillRoundedRectangle(505,242,595,215,16);
    caption(g,t("YOUR ENSEMBLE"),25,481,300,muted);g.setColour(muted);g.setFont(12);g.drawText(t("0 = unison   /   2 = third   /   4 = fifth in seven-note scales"),440,478,650,22,juce::Justification::right);
    g.setColour(palette.panel);g.fillRoundedRectangle(20,512,1080,294,14);
    auto ensemble=p.settings();
    for(int v=0;v<8;++v){int y=540+v*32;bool active=vg::voiceAudible(ensemble,v);g.setColour(v%2==0?forest.withAlpha(.025f):juce::Colours::transparentBlack);g.fillRoundedRectangle(27,(float)y,1066,31,5);g.setColour(active?forest.withAlpha(.09f):line.withAlpha(.4f));g.fillEllipse(36,(float)y+7,22,22);g.setColour(active?forest:muted.withAlpha(.4f));g.setFont(juce::FontOptions(11,juce::Font::bold));g.drawText(juce::String(v+1),36,y+7,22,22,juce::Justification::centred);}
    g.setColour(line);g.drawHorizontalLine(835,24,1096);
}
void VoxGardenEditor::resized(){
    theme.setBounds(610,25,135,32);title.setBounds(82,17,420,46);subtitle.setBounds(84,58,510,22);presets.setBounds(768,39,328,33);aiCast.setBounds(870,76,226,23);
    languageChoice.setBounds(610,70,135,26);raga.setBounds(36,185,300,30);ragaNotes.setBounds(352,174,730,48);
    root.setBounds(36,130,99,30);scale.setBounds(147,130,185,30);mode.setBounds(345,130,205,30);voiceCount.setBounds(565,130,130,30);formants.setBounds(712,130,190,30);pitch.setBounds(900,130,194,30);
    stage.setBounds(20,242,470,215);
    for(int i=0;i<10;++i){int x=511+(i%5)*117,y=253+(i/5)*99;controls[(size_t)i]->label.setBounds(x,y,110,16);controls[(size_t)i]->slider.setBounds(x,y+17,110,78);}
    for(int v=0;v<8;++v)for(int k=0;k<5;++k){auto& c=*controls[(size_t)(10+v*5+k)];int x=137+k*191;if(v==0)c.label.setBounds(x,516,183,20);else c.label.setVisible(false);c.slider.setBounds(x,540+v*32,183,30);}
    for(int v=0;v<vg::voices;++v){muteButtons[(size_t)v].setBounds(66,543+v*32,27,24);soloButtons[(size_t)v].setBounds(99,543+v*32,27,24);}
    aiPrint.setBounds(588,851,173,33);aiBlend.setBounds(770,851,150,33);
    status.setBounds(25,809,1070,24);render.setBounds(24,851,244,33);importFile.setBounds(280,851,142,33);reveal.setBounds(434,851,142,33);help.setBounds(942,851,154,33);
}
void VoxGardenEditor::timerCallback(){
    if(lastLanguage!=p.language.load())applyLanguage();
    if(theme.getToggleState()!=p.darkMode.load())applyTheme();
    VG_PALETTE
    auto note=p.engine.detectedNote.load();auto conf=p.engine.pitchConfidence.load();int n=(int)std::round(note);pitch.setText(conf>.8f?"Input  "+juce::String(vg::roots[(size_t)((n%12+12)%12)])+juce::String(n/12-1)+" / "+juce::String((note-n)*100,0)+" ct":"Listening for your vocal",juce::dontSendNotification);
    aiBlend.setEnabled(p.settings().aiPrint);
    int count=p.settings().count;if(lastCount!=count){lastCount=count;for(int v=0;v<8;++v)for(int k=0;k<5;++k)controls[(size_t)(10+v*5+k)]->slider.setEnabled(v<count);repaint();}
    for(int v=0;v<vg::voices;++v){muteButtons[(size_t)v].setEnabled(v<count);soloButtons[(size_t)v].setEnabled(v<count);}
    repaint();stage.repaint();
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
bool VoxGardenEditor::isInterestedInFileDrag(const juce::StringArray& files){return files.size()==1&&juce::File(files[0]).hasFileExtension("wav;aif;aiff;flac")&&!rendering.load()&&!capturePending;}
void VoxGardenEditor::filesDropped(const juce::StringArray& files,int,int){if(isInterestedInFileDrag(files))renderFile(juce::File(files[0]));}
void VoxGardenEditor::renderFile(const juce::File& f,bool trackTake){ VG_PALETTE
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
    look.palette=GardenPalette(p.darkMode.load());auto c=look.palette;
    theme.setToggleState(p.darkMode.load(),juce::dontSendNotification);
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
    lastLanguage=p.language.load();look.language=lastLanguage;languageChoice.setSelectedId(lastLanguage+1,juce::dontSendNotification);
    for(auto& pair:englishMenus){auto* combo=pair.first;int selected=combo->getSelectedId();*combo->getRootMenu()=pair.second;
        juce::PopupMenu::MenuItemIterator iterator(*combo->getRootMenu(),true);while(iterator.next()){auto& item=iterator.getItem();item.text=t(item.text);}
        if(selected>0)combo->setText(combo->getItemText(combo->indexOfItemId(selected)),juce::dontSendNotification);
    }
    for(int count=1;count<=8;++count)voiceCount.changeItemText(count,vg::voiceCountText(lastLanguage,count));
    if(presets.getSelectedId()==0)presets.setText(t("Explore the choir..."),juce::dontSendNotification);
    for(auto& control:controls)control->label.setFont(juce::FontOptions(lastLanguage==1?12.f:9.5f,juce::Font::bold));
    subtitle.setFont(juce::FontOptions(lastLanguage==1?15:13));status.setFont(juce::FontOptions(lastLanguage==1?14:12));ragaNotes.setFont(juce::FontOptions(lastLanguage==1?14:12));
    vg::localiseComponent(*this,lastLanguage);sendLookAndFeelChange();repaint();
}
