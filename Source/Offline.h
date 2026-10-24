#pragma once
#include "Engine.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <functional>
inline juce::String renderVocal(const juce::File& inputFile,const juce::File& outputFolder,const vg::Settings& renderSettings,std::atomic<float>& progress,const std::function<bool()>& cancelled,const std::vector<juce::File>& aiFiles={}) {
    juce::AudioFormatManager formats;formats.registerBasicFormats();std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(inputFile));
    if(!reader){return "Could not read that audio file.";}
    if(!outputFolder.createDirectory()){return "Could not create output folder.";}
    const int voiceCount=juce::jlimit(1,vg::voices,renderSettings.count);
    double sr=reader->sampleRate;vg::Engine offline;offline.prepare(sr);
    const bool withAI=!aiFiles.empty();
    std::array<std::unique_ptr<juce::AudioFormatReader>,vg::voices> aiReaders;
    std::unique_ptr<vg::Engine> aiEngine;
    if(withAI){
        if((int)aiFiles.size()!=voiceCount)return "AI source count does not match the selected choir size.";
        aiEngine=std::make_unique<vg::Engine>();aiEngine->prepare(sr);
        for(int v=0;v<voiceCount;++v){auto& r=aiReaders[(size_t)v];r.reset(formats.createReaderFor(aiFiles[(size_t)v]));if(!r || r->sampleRate!=sr || r->lengthInSamples!=reader->lengthInSamples)return "AI singer files do not align with the input take.";}
    }
    juce::WavAudioFormat wav;std::array<std::unique_ptr<juce::AudioFormatWriter>,9> writers;
    for(int i=0;i<=voiceCount;++i){auto f=outputFolder.getChildFile(i==0?"Choir Mix.wav":"Voice "+juce::String(i)+".wav");auto stream=f.createOutputStream();if(!stream){return "Could not write outputs.";}stream->setPosition(0);stream->truncate();writers[(size_t)i].reset(wav.createWriterFor(stream.release(),sr,2,24,{},0));if(!writers[(size_t)i]){return "Could not create WAV writer.";}}
    constexpr int block=512;juce::AudioBuffer<float> audio(2,block),stems(16,block),aiMix(2,block),aiStems(16,block),aiSources(8,block),readMono(1,block);
    int64_t total=reader->lengthInSamples+offline.latency()+(int64_t)(sr*4);
    for(int64_t at=0;at<total && !cancelled();at+=block){int n=(int)std::min<int64_t>(block,total-at);audio.clear();stems.clear();if(at<reader->lengthInSamples)reader->read(&audio,0,(int)std::min<int64_t>(n,reader->lengthInSamples-at),at,true,true);
        if(reader->numChannels==1)audio.copyFrom(1,0,audio,0,0,n);
        if(withAI){aiSources.clear();aiStems.clear();aiMix.copyFrom(0,0,audio,0,0,n);aiMix.copyFrom(1,0,audio,1,0,n);
            for(int v=0;v<voiceCount;++v){readMono.clear();if(at<reader->lengthInSamples)aiReaders[(size_t)v]->read(&readMono,0,(int)std::min<int64_t>(n,reader->lengthInSamples-at),at,true,false);aiSources.copyFrom(v,0,readMono,0,0,n);}
            aiEngine->process(aiMix.getWritePointer(0),aiMix.getWritePointer(1),n,renderSettings,aiStems.getArrayOfWritePointers(),aiSources.getArrayOfReadPointers());
        }
        offline.process(audio.getWritePointer(0),audio.getWritePointer(1),n,renderSettings,stems.getArrayOfWritePointers());
        if(withAI){float blend=juce::jlimit(0.f,1.f,renderSettings.aiBlend);for(int c=0;c<2;++c)for(int i=0;i<n;++i)audio.setSample(c,i,audio.getSample(c,i)*(1-blend)+aiMix.getSample(c,i)*blend);for(int c=0;c<16;++c)for(int i=0;i<n;++i)stems.setSample(c,i,stems.getSample(c,i)*(1-blend)+aiStems.getSample(c,i)*blend);}
        int skip=(int)std::clamp<int64_t>(offline.latency()-at,0,n);int take=n-skip;
        if(take>0){if(!writers[0]->writeFromAudioSampleBuffer(audio,skip,take)){return "Disk write failed.";}
            for(int v=0;v<voiceCount;++v){const float* data[2]{stems.getReadPointer(v*2)+skip,stems.getReadPointer(v*2+1)+skip};if(!writers[(size_t)v+1]->writeFromFloatArrays(data,2,take)){return "Stem write failed.";}}}
        progress=(withAI?.75f:0.f)+(withAI?.25f:1.f)*(float)((double)(at+n)/(double)total);
    }
    if(cancelled()){return "Render cancelled. Partial files remain in "+outputFolder.getFileName();}
    return "Saved mix + "+juce::String(voiceCount)+" stems: "+outputFolder.getFileName();
}
