#pragma once
#include <juce_audio_utils/juce_audio_utils.h>

// Single audio producer / disk consumer. The audio callback only copies into
// preallocated storage; file operations and finalization happen on this thread.
class TrackCapture final : private juce::Thread {
public:
    enum Phase { idle, starting, recording, stopping, complete, failed };
    TrackCapture():Thread("VoxGarden track capture"),fifo(1){}
    ~TrackCapture() override { shutdown(); }
    void prepare(double sampleRate,int maxBlock) {
        shutdown();sr=sampleRate>0?sampleRate:48000;
        const int capacity=std::max((int)(sr*2),std::max(16384,maxBlock*4));
        fifo.setTotalSize(capacity);buffer.setSize(2,capacity);scratch.setSize(2,4096);
        phase=idle;startThread();
    }
    void shutdown() {
        if(!isThreadRunning())return;
        requestStop();signalThreadShouldExit();notify();stopThread(-1);
    }
    bool begin(const juce::File& file) {
        const auto current=phase.load();if(current==starting||current==recording||current==stopping||!isThreadRunning()||file.exists())return false;
        {const juce::ScopedLock lock(messageLock);destination=file;error.clear();}
        frames=0;overflow=false;phase=starting;notify();return true;
    }
    void requestStop(){int expected=recording;phase.compare_exchange_strong(expected,stopping);notify();}
    Phase getPhase()const{return (Phase)phase.load();}
    double seconds()const{return frames.load()/sr;}
    juce::File file()const{const juce::ScopedLock lock(messageLock);return destination;}
    juce::String errorMessage()const{const juce::ScopedLock lock(messageLock);return error;}
    void acknowledge(){int expected=complete;phase.compare_exchange_strong(expected,idle);}
    void push(const float* l,const float* r,int count) noexcept {
        readers.fetch_add(1);
        if(phase.load()==recording && count>0) {
            int a=0,an=0,b=0,bn=0;fifo.prepareToWrite(count,a,an,b,bn);
            if(an+bn!=count){overflow=true;phase=stopping;}
            else {
                buffer.copyFrom(0,a,l,an);buffer.copyFrom(1,a,r,an);
                if(bn){buffer.copyFrom(0,b,l+an,bn);buffer.copyFrom(1,b,r+an,bn);}
                fifo.finishedWrite(count);frames.fetch_add(count);
            }
        }
        readers.fetch_sub(1);
    }
private:
    juce::AbstractFifo fifo;juce::AudioBuffer<float> buffer,scratch;
    std::atomic<int> phase{idle},readers{0};std::atomic<int64_t> frames{0};std::atomic<bool> overflow{false};
    std::atomic<double> sr{48000};mutable juce::CriticalSection messageLock;juce::File destination;juce::String error;
    void fail(juce::String message){const juce::ScopedLock lock(messageLock);error=std::move(message);phase=failed;}
    void run() override {
        std::unique_ptr<juce::AudioFormatWriter> writer;
        while(!threadShouldExit() || writer) {
            if(phase.load()==starting){
                if(threadShouldExit()){phase=idle;break;}
                fifo.reset();auto target=file();auto stream=target.createOutputStream();
                if(!stream){fail("Could not create a track capture file.");continue;}
                juce::WavAudioFormat format;writer.reset(format.createWriterFor(stream.release(),sr,2,24,{},0));
                if(!writer){fail("Could not open the capture writer.");continue;}phase=recording;
            }
            if(threadShouldExit() && phase.load()==recording)phase=stopping;
            if(writer){
                const bool finishing=phase.load()==stopping;
                if(finishing)while(readers.load()!=0)juce::Thread::yield();
                int a=0,an=0,b=0,bn=0;fifo.prepareToRead(4096,a,an,b,bn);int n=an+bn;
                if(n){for(int ch=0;ch<2;++ch){scratch.copyFrom(ch,0,buffer,ch,a,an);if(bn)scratch.copyFrom(ch,an,buffer,ch,b,bn);}
                    bool ok=writer->writeFromAudioSampleBuffer(scratch,0,n);fifo.finishedRead(n);
                    if(!ok){phase=stopping;while(readers.load()!=0)juce::Thread::yield();writer.reset();fail("Capture stopped: disk write failed. The partial take was retained.");continue;}
                } else if(finishing){
                    writer.reset();
                    if(overflow.load())fail("Capture buffer overflowed. The partial take was retained; try a quieter CPU load.");
                    else if(frames.load()==0)fail("No channel audio arrived. Play the vocal in Live while capturing.");
                    else phase=complete;
                }
                wait(2);
            } else wait(20);
        }
    }
};
