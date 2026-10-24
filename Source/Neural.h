#pragma once
#include "Score.h"
#include <juce_cryptography/juce_cryptography.h>
#include "SingingResources.h"
#include <spawn.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <cerrno>
#include <unistd.h>
extern char** environ;
namespace vg {
inline juce::File singingRoot(){return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("Application Support/VoxGarden/Singing");}
inline bool singingReady(){auto root=singingRoot();auto ready=juce::JSON::parse(root.getChildFile("READY.json"));return (int)ready.getProperty("schema",0)==1 && ready.getProperty("model_revision",{}).toString()=="724fb5abbf5502583fb520898d45929e62f02c0b" && root.getChildFile("python/bin/python").existsAsFile();}
inline bool installSingingHelpers(){auto root=singingRoot();if(!root.createDirectory())return false;
    auto write=[&](const char* name,const char* data,int size){auto file=root.getChildFile(name);auto hash=juce::SHA256(data,(size_t)size).toHexString();if(file.existsAsFile()&&juce::SHA256(file).toHexString()==hash)return true;return file.replaceWithData(data,(size_t)size);};
    return write("runtime.py",SingingData::runtime_py,SingingData::runtime_pySize)&&write("bootstrap.sh",SingingData::bootstrap_sh,SingingData::bootstrap_shSize)&&write("model-manifest.json",SingingData::modelmanifest_json,SingingData::modelmanifest_jsonSize);
}
// A separate process group makes Cancel stop downloads and their children as well.
inline int runSingingProcess(const juce::StringArray& args,const juce::File& log,const std::function<bool()>& cancelled,const std::function<void()>& poll){
    std::vector<std::string> storage;for(auto& a:args)storage.push_back(a.toStdString());std::vector<char*> argv;for(auto& a:storage)argv.push_back(a.data());argv.push_back(nullptr);
    posix_spawnattr_t attributes;posix_spawnattr_init(&attributes);posix_spawnattr_setflags(&attributes,POSIX_SPAWN_SETPGROUP);posix_spawnattr_setpgroup(&attributes,0);
    posix_spawn_file_actions_t actions;posix_spawn_file_actions_init(&actions);posix_spawn_file_actions_addopen(&actions,STDOUT_FILENO,log.getFullPathName().toRawUTF8(),O_CREAT|O_WRONLY|O_TRUNC,0600);posix_spawn_file_actions_adddup2(&actions,STDOUT_FILENO,STDERR_FILENO);
    pid_t pid=0;int error=posix_spawn(&pid,argv[0],&actions,&attributes,argv.data(),environ);posix_spawnattr_destroy(&attributes);posix_spawn_file_actions_destroy(&actions);if(error)return -1;
    int result=0;for(;;){auto wait=waitpid(pid,&result,WNOHANG);if(wait==pid)break;if(wait<0){if(errno==EINTR)continue;kill(-pid,SIGTERM);return -1;}
        if(cancelled()){kill(-pid,SIGTERM);for(int i=0;i<20;++i){if(waitpid(pid,&result,WNOHANG)==pid){kill(-pid,SIGKILL);return -2;}juce::Thread::sleep(25);}kill(-pid,SIGKILL);waitpid(pid,&result,0);return -2;}
        poll();juce::Thread::sleep(100);
    }poll();return WIFEXITED(result)?WEXITSTATUS(result):1;
}
inline juce::String renderNeuralScore(const Score& notes,int language,const juce::String& voice,const juce::File& folder,std::atomic<float>& progress,const std::function<bool()>& cancelled,juce::File& result){
    if(!singingReady())return "Download the singing models in this window first.";
    if(!installSingingHelpers()||!folder.createDirectory())return "Could not prepare the singing runtime.";
    auto job=new juce::DynamicObject;juce::Array<juce::var> values;for(auto& n:notes){auto note=new juce::DynamicObject;note->setProperty("start",n.start);note->setProperty("duration",n.duration);note->setProperty("midi",n.midi);note->setProperty("syllable",n.syllable);note->setProperty("velocity",n.velocity);note->setProperty("gain",n.gain);values.add(juce::var(note));}
    constexpr const char* languages[]{"en","hi","de","ja","ru","nl"};job->setProperty("notes",values);job->setProperty("language",languages[juce::jlimit(0,5,language)]);job->setProperty("voice",voice);job->setProperty("output",folder.getFullPathName());
    auto file=folder.getChildFile("Singing Job.json");if(!file.replaceWithText(juce::JSON::toString(juce::var(job))))return "Could not save singing job.";
    auto report=folder.getChildFile("Singing Status.json");auto root=singingRoot();auto code=runSingingProcess({root.getChildFile("python/bin/python").getFullPathName(),root.getChildFile("runtime.py").getFullPathName(),"--job",file.getFullPathName(),"--status",report.getFullPathName()},folder.getChildFile("Singing Render.log"),cancelled,[&]{auto state=juce::JSON::parse(report);progress=(float)(double)state.getProperty("progress",0);});
    if(code==-2)return "Cancelled.";auto state=juce::JSON::parse(report);if(code!=0||state.getProperty("phase",{}).toString()!="complete")return "Singing render failed: "+state.getProperty("message","See Singing Render.log").toString();
    result=folder.getChildFile("Dry Singing.wav");if(!result.existsAsFile())return "Singing renderer did not return an audio file.";folder.getChildFile("Score.tsv").replaceWithText(scoreText(notes));return {};
}
}
