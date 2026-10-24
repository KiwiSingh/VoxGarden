#pragma once
#include "Engine.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <functional>
namespace vg {
struct ScoreNote { double start=0,duration=.5; int midi=60; juce::String syllable; };
using Score=std::vector<ScoreNote>;
inline juce::String scoreText(const Score& notes){juce::String text;for(auto& n:notes)text<<juce::String(n.start,4)<<"\t"<<juce::String(n.duration,4)<<"\t"<<n.midi<<"\t"<<n.syllable<<"\n";return text;}
inline juce::String parseScore(const juce::String& text,Score& notes){
    notes.clear();auto lines=juce::StringArray::fromLines(text);int row=0;
    for(auto line:lines){++row;if(line.trim().isEmpty())continue;auto fields=juce::StringArray::fromTokens(line,"\t","");
        if(fields.size()!=4)return "Row "+juce::String(row)+": use four TAB-separated columns.";
        auto number=[](juce::String value,double& result){auto v=value.trim();if(v.isEmpty()||v.containsOnly("+- ."))return false;auto bytes=v.toStdString();char* end=nullptr;result=std::strtod(bytes.c_str(),&end);return end==bytes.c_str()+bytes.size()&&std::isfinite(result);};
        double start,duration,pitch;
        auto pitchText=fields[2].trim();bool pitchValid=number(pitchText,pitch);
        if(!pitchValid&&pitchText.length()>=2){auto letter=pitchText.substring(0,1).toUpperCase();int base=letter=="C"?0:letter=="D"?2:letter=="E"?4:letter=="F"?5:letter=="G"?7:letter=="A"?9:letter=="B"?11:-100;int index=1;if(pitchText.substring(1,2)=="#"){++base;++index;}else if(pitchText.substring(1,2)=="b"){--base;++index;}double octave;if(base>-90&&number(pitchText.substring(index),octave)&&octave==std::floor(octave)){pitch=(octave+1)*12+base;pitchValid=true;}}
        if(!number(fields[0],start)||!number(fields[1],duration)||!pitchValid||start<0||duration<.06||start+duration>300||pitch<36||pitch>96||pitch!=std::floor(pitch))return "Row "+juce::String(row)+": start >= 0, duration >= 0.06, end <= 300 seconds, MIDI integer 36-96.";
        auto syllable=fields[3].trim();if(syllable.isEmpty()||syllable.length()>160)return "Each note needs a syllable (up to 160 characters).";
        notes.push_back({start,duration,(int)pitch,syllable});if(notes.size()>2048)return "Use at most 2048 notes.";
    }
    if(notes.empty())return "Add or import some notes first.";
    std::stable_sort(notes.begin(),notes.end(),[](auto& a,auto& b){return a.start<b.start;});
    for(size_t i=1;i<notes.size();++i)if(notes[i].start+1e-6<notes[i-1].start+notes[i-1].duration)return "Overlapping notes: choose a monophonic melody track or edit the times.";
    return {};
}
struct ScoreTrack {juce::String name;Score notes;};
inline juce::String readMidiScore(const juce::File& file,std::vector<ScoreTrack>& tracks){
    if(file.getSize()>8*1024*1024)return "Use a MIDI file smaller than 8 MB.";
    auto stream=file.createInputStream();juce::MidiFile midi;tracks.clear();int type=0;if(!stream||!midi.readFrom(*stream,true,&type))return "Could not read a standard MIDI file.";
    if(type==2)return "MIDI type 2 has independent track timelines. Export a type 0 or 1 melody.";
    midi.convertTimestampTicksToSeconds();
    for(int t=0;t<midi.getNumTracks();++t){auto sequence=*midi.getTrack(t);sequence.updateMatchedPairs();ScoreTrack track{"Track "+juce::String(t+1),{}};
        for(int i=0;i<sequence.getNumEvents();++i){auto* event=sequence.getEventPointer(i);auto& message=event->message;
            if(message.isTrackNameEvent())track.name=message.getTextFromTextMetaEvent();
            if(!message.isNoteOn()||message.getChannel()==10)continue;
            if(!event->noteOffObject)return "MIDI contains an unterminated note.";
            double start=message.getTimeStamp(),duration=event->noteOffObject->message.getTimeStamp()-start;
            if(duration>0)track.notes.push_back({start,duration,message.getNoteNumber(),"la"});
        }
        if(!track.notes.empty())tracks.push_back(std::move(track));
    }
    return tracks.empty()?"No pitched MIDI notes found (drum channel 10 is excluded).":juce::String{};
}
// Monophonic pitch sketch. Results are deliberately editable, never presented as polyphonic transcription.
inline juce::String transcribeScore(const juce::File& file,Score& notes,const std::function<bool()>& cancelled){
    juce::AudioFormatManager formats;formats.registerBasicFormats();std::unique_ptr<juce::AudioFormatReader> r(formats.createReaderFor(file));
    if(!r||r->lengthInSamples<=0)return "Could not read that audio file.";
    if(r->lengthInSamples/r->sampleRate>300)return "Use an audio excerpt of five minutes or less.";
    notes.clear();PitchTracker tracker;tracker.prepare(r->sampleRate);juce::AudioBuffer<float> block(2,512);
    int active=-1,candidate=-1,stable=0;double start=0,last=0;const double latency=2048.0/r->sampleRate;
    auto close=[&](double end){if(active>=0&&end-start>=.08)notes.push_back({start,end-start,active,"la"});active=-1;};
    for(int64_t at=0;at<r->lengthInSamples;at+=512){if(cancelled())return "Cancelled.";int n=(int)std::min<int64_t>(512,r->lengthInSamples-at);block.clear();r->read(&block,0,n,at,true,true);
        float rms=0;for(int i=0;i<n;++i){float x=r->numChannels>1?(block.getSample(0,i)+block.getSample(1,i))*.5f:block.getSample(0,i);tracker.push(x);rms+=x*x;}
        double time=std::max(0.0,(at+n)/r->sampleRate-latency);int detected=tracker.voiced&&rms/n>1e-6f?juce::roundToInt(tracker.note):-1;
        if(detected<36||detected>96)detected=-1;
        if(detected==candidate)++stable;else{candidate=detected;stable=1;}
        if(stable==3&&detected!=active){double boundary=std::max(0.0,time-1024.0/r->sampleRate);close(boundary);active=detected;start=boundary;}
        last=time;
    }
    close(std::max(last,r->lengthInSamples/r->sampleRate-latency));
    return notes.empty()?"No stable monophonic notes detected. Try a clean, isolated melody.":juce::String{};
}
inline constexpr std::array<const char*,6> singingLocales{"en_US","hi_IN","de_DE","ja_JP","ru_RU","nl_NL"};
inline constexpr std::array<const char*,6> singingLanguages{"English","हिन्दी (देवनागरी)","Deutsch","日本語","Русский","Nederlands"};

}
