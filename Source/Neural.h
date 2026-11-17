#pragma once
#include "Score.h"
#include <juce_cryptography/juce_cryptography.h>
#include "SingingResources.h"
#include "Platform.h"
#if JUCE_WINDOWS
#include <windows.h>
#else
#include <spawn.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <cerrno>
#include <unistd.h>
extern char** environ;
#endif
namespace vg {
inline juce::File singingRoot(){return dataRoot().getChildFile("Singing");}
inline bool singingReady(){auto root=singingRoot();auto ready=juce::JSON::parse(root.getChildFile("READY.json"));return (int)ready.getProperty("schema",0)==1 && ready.getProperty("model_revision",{}).toString()=="724fb5abbf5502583fb520898d45929e62f02c0b" && runtimePython(root).existsAsFile();}
inline bool installSingingHelpers(){auto root=singingRoot();if(!root.createDirectory())return false;
    auto write=[&](const char* name,const char* data,int size){auto file=root.getChildFile(name);auto hash=juce::SHA256(data,(size_t)size).toHexString();if(file.existsAsFile()&&juce::SHA256(file).toHexString()==hash)return true;return file.replaceWithData(data,(size_t)size);};
    return write("runtime.py",SingingData::runtime_py,SingingData::runtime_pySize)&&write("bootstrap.sh",SingingData::bootstrap_sh,SingingData::bootstrap_shSize)&&write("bootstrap.ps1",SingingData::bootstrap_ps1,SingingData::bootstrap_ps1Size)&&write("model-manifest.json",SingingData::modelmanifest_json,SingingData::modelmanifest_jsonSize);
}
// A separate process group makes Cancel stop downloads and their children as well.
inline int runSingingProcess(const juce::StringArray& args,const juce::File& log,const std::function<bool()>& cancelled,const std::function<void()>& poll){
#if JUCE_WINDOWS
    auto quote=[](const juce::String& arg){std::wstring out=L"\"";size_t slashes=0;for(auto ch:std::wstring(arg.toWideCharPointer())){if(ch==L'\\'){++slashes;continue;}if(ch==L'\"')out.append(slashes*2+1,L'\\');else out.append(slashes,L'\\');slashes=0;out+=ch;}out.append(slashes*2,L'\\');out+=L'\"';return out;};
    std::wstring command;for(auto& arg:args){if(!command.empty())command+=L' ';command+=quote(arg);}
    SECURITY_ATTRIBUTES security{sizeof(SECURITY_ATTRIBUTES),nullptr,TRUE};auto output=CreateFileW(log.getFullPathName().toWideCharPointer(),GENERIC_WRITE,FILE_SHARE_READ,&security,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);if(output==INVALID_HANDLE_VALUE)return -1;
    auto job=CreateJobObjectW(nullptr,nullptr);JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits{};limits.BasicLimitInformation.LimitFlags=JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if(!job||!SetInformationJobObject(job,JobObjectExtendedLimitInformation,&limits,sizeof(limits))){if(job)CloseHandle(job);CloseHandle(output);return -1;}
    STARTUPINFOW startup{};startup.cb=sizeof(startup);startup.dwFlags=STARTF_USESTDHANDLES;startup.hStdOutput=output;startup.hStdError=output;startup.hStdInput=nullptr;PROCESS_INFORMATION process{};
    bool started=CreateProcessW(args[0].toWideCharPointer(),command.data(),nullptr,nullptr,TRUE,CREATE_NO_WINDOW|CREATE_SUSPENDED,nullptr,nullptr,&startup,&process)!=FALSE;CloseHandle(output);
    if(!started){CloseHandle(job);return -1;}if(!AssignProcessToJobObject(job,process.hProcess)){TerminateProcess(process.hProcess,1);CloseHandle(process.hThread);CloseHandle(process.hProcess);CloseHandle(job);return -1;}ResumeThread(process.hThread);CloseHandle(process.hThread);
    int result=1;while(WaitForSingleObject(process.hProcess,100)==WAIT_TIMEOUT){if(cancelled()){TerminateJobObject(job,2);WaitForSingleObject(process.hProcess,2000);result=-2;break;}poll();}
    if(result!=-2){DWORD code=1;GetExitCodeProcess(process.hProcess,&code);result=(int)code;}CloseHandle(process.hProcess);CloseHandle(job);poll();return result;
#else
    std::vector<std::string> storage;for(auto& a:args)storage.push_back(a.toStdString());std::vector<char*> argv;for(auto& a:storage)argv.push_back(a.data());argv.push_back(nullptr);
    posix_spawnattr_t attributes;posix_spawnattr_init(&attributes);posix_spawnattr_setflags(&attributes,POSIX_SPAWN_SETPGROUP);posix_spawnattr_setpgroup(&attributes,0);
    posix_spawn_file_actions_t actions;posix_spawn_file_actions_init(&actions);posix_spawn_file_actions_addopen(&actions,STDOUT_FILENO,log.getFullPathName().toRawUTF8(),O_CREAT|O_WRONLY|O_TRUNC,0600);posix_spawn_file_actions_adddup2(&actions,STDOUT_FILENO,STDERR_FILENO);
    pid_t pid=0;int error=posix_spawn(&pid,argv[0],&actions,&attributes,argv.data(),environ);posix_spawnattr_destroy(&attributes);posix_spawn_file_actions_destroy(&actions);if(error)return -1;
    int result=0;for(;;){auto wait=waitpid(pid,&result,WNOHANG);if(wait==pid)break;if(wait<0){if(errno==EINTR)continue;kill(-pid,SIGTERM);return -1;}
        if(cancelled()){kill(-pid,SIGTERM);for(int i=0;i<20;++i){if(waitpid(pid,&result,WNOHANG)==pid){kill(-pid,SIGKILL);return -2;}juce::Thread::sleep(25);}kill(-pid,SIGKILL);waitpid(pid,&result,0);return -2;}
        poll();juce::Thread::sleep(100);
    }poll();return WIFEXITED(result)?WEXITSTATUS(result):1;
#endif
}
inline juce::StringArray singingSetupCommand(){auto root=singingRoot();if(singingReady())return {runtimePython(root).getFullPathName(),"-X","utf8",root.getChildFile("runtime.py").getFullPathName(),"--setup","--status",root.getChildFile("Setup Status.json").getFullPathName()};
#if JUCE_WINDOWS
return {juce::File::getSpecialLocation(juce::File::windowsSystemDirectory).getChildFile("WindowsPowerShell/v1.0/powershell.exe").getFullPathName(),"-NoProfile","-ExecutionPolicy","Bypass","-File",root.getChildFile("bootstrap.ps1").getFullPathName(),"-Root",root.getFullPathName()};
#else
return {"/bin/sh",root.getChildFile("bootstrap.sh").getFullPathName(),root.getFullPathName()};
#endif
}
inline juce::String singingBackendLabel(){
#if JUCE_WINDOWS
return juce::JSON::parse(singingRoot().getChildFile("READY.json")).getProperty("backend","DirectML / CPU").toString();
#else
return "Core ML GPU preferred";
#endif
}
inline juce::String renderNeuralScore(const Score& notes,int language,const juce::String& voice,const juce::File& folder,std::atomic<float>& progress,const std::function<bool()>& cancelled,juce::File& result){
    if(!singingReady())return "Download the singing models in this window first.";
    if(!installSingingHelpers()||!folder.createDirectory())return "Could not prepare the singing runtime.";
    auto job=new juce::DynamicObject;juce::Array<juce::var> values;for(auto& n:notes){auto note=new juce::DynamicObject;note->setProperty("start",n.start);note->setProperty("duration",n.duration);note->setProperty("midi",n.midi);note->setProperty("syllable",n.syllable);note->setProperty("velocity",n.velocity);note->setProperty("gain",n.gain);values.add(juce::var(note));}
    constexpr const char* languages[]{"en","hi","de","ja","ru","nl"};job->setProperty("notes",values);job->setProperty("language",languages[juce::jlimit(0,5,language)]);job->setProperty("voice",voice);job->setProperty("output",folder.getFullPathName());
    auto file=folder.getChildFile("Singing Job.json");if(!file.replaceWithText(juce::JSON::toString(juce::var(job))))return "Could not save singing job.";
    auto report=folder.getChildFile("Singing Status.json");auto root=singingRoot();auto code=runSingingProcess({runtimePython(root).getFullPathName(),"-X","utf8",root.getChildFile("runtime.py").getFullPathName(),"--job",file.getFullPathName(),"--status",report.getFullPathName()},folder.getChildFile("Singing Render.log"),cancelled,[&]{auto state=juce::JSON::parse(report);progress=(float)(double)state.getProperty("progress",0);});
    if(code==-2)return "Cancelled.";auto state=juce::JSON::parse(report);if(code!=0||state.getProperty("phase",{}).toString()!="complete")return "Singing render failed: "+state.getProperty("message","See Singing Render.log").toString();
    result=folder.getChildFile("Dry Singing.wav");if(!result.existsAsFile())return "Singing renderer did not return an audio file.";folder.getChildFile("Score.tsv").replaceWithText(scoreText(notes));return {};
}
}
