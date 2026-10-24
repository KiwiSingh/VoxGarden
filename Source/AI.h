#pragma once
#include "Engine.h"
#include <juce_audio_utils/juce_audio_utils.h>
#include <functional>
namespace vg {
inline juce::File aiRoot(){return juce::File::getSpecialLocation(juce::File::userHomeDirectory).getChildFile("Library/Application Support/VoxGarden/AI");}
inline bool aiReady(){auto r=aiRoot();return r.getChildFile("READY.json").existsAsFile() && r.getChildFile("python/bin/python").existsAsFile() && r.getChildFile("choir_worker.py").existsAsFile() && r.getChildFile("voices/manifest.json").existsAsFile();}
inline juce::var aiSettings(const Settings& s){
    auto obj=new juce::DynamicObject;juce::var result(obj);
    obj->setProperty("root",s.root);obj->setProperty("scale",s.scale);obj->setProperty("mode",s.mode);obj->setProperty("count",s.count);obj->setProperty("aiCast",s.aiCast);obj->setProperty("raga",s.raga);
    obj->setProperty("correction",s.correction);obj->setProperty("glide",s.glide);obj->setProperty("detune",s.detune);obj->setProperty("vibrato",s.vibrato);
    juce::Array<juce::var> voices;
    for(int index=0;index<vg::voices;++index){const auto& voice=s.voice[(size_t)index];auto v=new juce::DynamicObject;v->setProperty("degree",voice.degree);v->setProperty("stored_level",voice.level);v->setProperty("mute",voice.mute);v->setProperty("solo",voice.solo);v->setProperty("level",voiceAudible(s,index)?voice.level:0.f);voices.add(juce::var(v));}
    obj->setProperty("voices",voices);return result;
}
inline juce::String createAISingers(const juce::File& source,const juce::File& output,const Settings& s,std::atomic<float>& progress,const std::function<bool()>& cancelled,std::vector<juce::File>& files){
    if(!aiReady())return "AI singers need the free local runtime. Run Setup AI.command in the package first.";
    if(s.aiCast!=0 && (int)juce::JSON::parse(aiRoot().getChildFile("READY.json")).getProperty("voice_cast_version",0)<1)return "Update AI cast support: run Setup AI.command from the new VoxGarden package. Existing models can be reused.";
    if(s.raga>0 && (int)juce::JSON::parse(aiRoot().getChildFile("READY.json")).getProperty("raga_version",0)<1)return "Update AI raga support: run Setup AI.command from the new VoxGarden package. Existing models can be reused.";
    auto folder=output.getChildFile("AI Sources");if(!folder.createDirectory())return "Could not create the AI render folder.";
    auto job=new juce::DynamicObject;job->setProperty("source",source.getFullPathName());job->setProperty("output",folder.getFullPathName());job->setProperty("settings",aiSettings(s));job->setProperty("steps",20);
    auto jobFile=folder.getChildFile("AI Job.json");if(!jobFile.replaceWithText(juce::JSON::toString(juce::var(job))))return "Could not save the AI render job.";
    auto root=aiRoot();juce::ChildProcess worker;
    juce::StringArray command{root.getChildFile("python/bin/python").getFullPathName(),root.getChildFile("choir_worker.py").getFullPathName(),"--root",root.getFullPathName(),"--job",jobFile.getFullPathName()};
    if(!worker.start(command))return "Could not start the local AI renderer.";
    auto statusFile=folder.getChildFile("AI Status.json");
    while(worker.isRunning()){
        if(cancelled()){worker.kill();return "AI render cancelled. Your source take was retained.";}
        auto status=juce::JSON::parse(statusFile);
        if(status.isObject())progress.store((float)(double)status.getProperty("progress",0)*.75f);
        juce::Thread::sleep(120);
    }
    auto status=juce::JSON::parse(statusFile);
    if(worker.getExitCode()!=0 || status.getProperty("phase",{}).toString()!="complete")return "AI render failed: "+status.getProperty("message","See AI Sources/AI Render.log").toString();
    files.clear();for(int v=1;v<=s.count;++v){auto file=folder.getChildFile("AI Singer "+juce::String(v)+".wav");if(!file.existsAsFile())return "The AI renderer did not return every selected singer.";files.push_back(file);}
    return {};
}
}
