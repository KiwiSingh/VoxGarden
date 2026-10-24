#pragma once
#include "Score.h"
#include "I18n.h"
class ScoreGrid final:public juce::Component,private juce::TableListBoxModel {
    using Row=std::array<juce::String,4>;
    std::vector<Row> rows;
    juce::TableListBox table;
    bool readOnly=false;
    void changed(){if(onTextChange)onTextChange();}
    struct Cell final:juce::Label {
        int row=0,column=0;std::function<void(const juce::MouseEvent&)> select;
        void mouseDown(const juce::MouseEvent& event)override{if(select)select(event);juce::Label::mouseDown(event);}
    };
    int getNumRows()override{return (int)rows.size();}
    void paintRowBackground(juce::Graphics& g,int row,int width,int height,bool selected)override{auto colour=findColour(juce::TextEditor::backgroundColourId);g.setColour(selected?findColour(juce::TextEditor::highlightColourId):row%2?colour.darker(.06f):colour);g.fillRect(0,0,width,height);}
    void selectedRowsChanged(int)override{if(onSelectionChange)onSelectionChange();}
    void deleteKeyPressed(int)override{deleteNote();}
    void paintCell(juce::Graphics&,int,int,int,int,bool)override{}
    juce::Component* refreshComponentForCell(int row,int column,bool,juce::Component* existing)override{
        auto* cell=dynamic_cast<Cell*>(existing);if(!cell){cell=new Cell;cell->setFont(juce::FontOptions(16));cell->setJustificationType(juce::Justification::centredLeft);cell->onTextChange=[this,cell]{if(cell->column<4&&cell->row>=0&&cell->row<(int)rows.size()){rows[(size_t)cell->row][(size_t)cell->column]=cell->getText();changed();}};}
        cell->row=row;cell->column=column-1;cell->select=[this,cell](const auto& event){table.selectRowsBasedOnModifierKeys(cell->row,event.mods,false);table.grabKeyboardFocus();};cell->setEditable(false,!readOnly&&column!=5,false);cell->setColour(juce::Label::textColourId,findColour(juce::TextEditor::textColourId));cell->setText(column==5?juce::String(row+1):rows[(size_t)row][(size_t)column-1],juce::dontSendNotification);return cell;
    }
public:
    std::function<void()> onTextChange,onSelectionChange;
    bool hasSelection()const{return table.getNumSelectedRows()>0;}
    ScoreGrid(){addAndMakeVisible(table);table.setModel(this);table.setMultipleSelectionEnabled(true);table.setRowHeight(32);table.setHeaderHeight(30);table.getHeader().addColumn("#",5,48);table.getHeader().addColumn("Start (s)",1,125);table.getHeader().addColumn("Length (s)",2,125);table.getHeader().addColumn("Note",3,120);table.getHeader().addColumn("Syllable / word",4,415);table.setColour(juce::ListBox::outlineColourId,juce::Colours::transparentBlack);}
    ~ScoreGrid()override{table.setModel(nullptr);}
    void setLanguage(int language){const char* names[]{"Start (s)","Length (s)","Note","Syllable / word"};for(int i=0;i<4;++i)table.getHeader().setColumnName(i+1,vg::tr(language,names[i]));}
    void setText(const juce::String& text,bool notify=true){table.deselectAllRows();rows.clear();for(auto line:juce::StringArray::fromLines(text)){if(line.trim().isEmpty())continue;auto fields=juce::StringArray::fromTokens(line,"\t","");Row row{};for(int i=0;i<std::min(4,fields.size());++i)row[(size_t)i]=fields[i];if(fields.size()>=3&&fields[2].containsOnly("0123456789")){int midi=fields[2].getIntValue();if(midi>=0&&midi<=127)row[2]=juce::MidiMessage::getMidiNoteName(midi,true,true,4);}rows.push_back(row);}table.updateContent();table.repaint();if(notify)changed();}
    juce::String getText()const{juce::String text;for(auto& row:rows){for(int i=0;i<4;++i){if(i)text<<"\t";text<<row[(size_t)i];}text<<"\n";}return text;}
    void setReadOnly(bool value){readOnly=value;table.updateContent();}
    void addNote(){if(readOnly||rows.size()>=2048)return;double start=0;if(!rows.empty())start=rows.back()[0].getDoubleValue()+rows.back()[1].getDoubleValue();rows.push_back({juce::String(start,3),"0.5","C4","la"});table.updateContent();table.selectRow((int)rows.size()-1);changed();}
    void deleteNote(){if(readOnly)return;auto selected=table.getSelectedRows();if(selected.isEmpty())return;int next=selected[0];for(int i=selected.size()-1;i>=0;--i){int row=selected[i];if(row>=0&&row<(int)rows.size())rows.erase(rows.begin()+row);}table.deselectAllRows();table.updateContent();if(!rows.empty())table.selectRow(std::min(next,(int)rows.size()-1));changed();}
    void refreshColours(){table.setColour(juce::ListBox::backgroundColourId,findColour(juce::TextEditor::backgroundColourId));table.updateContent();table.repaint();}
    void lookAndFeelChanged()override{refreshColours();}
    void resized()override{table.setBounds(getLocalBounds());}
};
