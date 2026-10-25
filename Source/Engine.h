#pragma once
#include "Harmony.h"
#include <juce_dsp/juce_dsp.h>
#include <signalsmith-stretch/signalsmith-stretch.h>

namespace vg {
// Decimated YIN. Fixed storage; no allocation, locks or I/O during processing.
class PitchTracker {
    std::array<float,1024> ring{};
    std::array<float,1024> window{};
    std::array<float,256> diff{};
    int write=0,dec=0,tick=0,filled=0,decimation=4;
    float low=0,low2=0,alpha=.28f; double rate=12000;
public:
    float note=60, confidence=0; bool voiced=false;
    void prepare(double sr) { decimation=std::max(1,(int)std::round(sr/12000));rate=sr/decimation;alpha=1-std::exp(-(float)(juce::MathConstants<double>::twoPi*2500/sr));ring.fill(0);write=dec=tick=filled=0;low=low2=0;note=60;confidence=0;voiced=false; }
    void push(float x) {
        low += alpha*(x-low);low2+=alpha*(low-low2);
        if(++dec<decimation) return; dec=0;
        ring[(size_t)write]=low2;write=(write+1)%1024;filled=std::min(1024,filled+1);
        if(++tick<128 || filled<1024) return;tick=0;
        for(int i=0;i<1024;++i) window[(size_t)i]=ring[(size_t)((write+i)%1024)];
        float energy=0;for(float v:window)energy+=v*v;
        if(energy/1024<1e-7f) {voiced=false;confidence=0;return;}
        int lo=std::max(2,(int)(rate/1100)), hi=std::min(254,(int)(rate/65));
        float sum=0;diff[0]=1;
        for(int lag=1;lag<=hi+1;++lag) {
            float d=0;for(int i=0;i<512;++i) {float e=window[(size_t)i]-window[(size_t)(i+lag)];d+=e*e;}
            sum+=d;diff[(size_t)lag]=sum>0?d*lag/sum:1;
        }
        int best=-1;
        for(int lag=lo;lag<hi;++lag) if(diff[(size_t)lag]<.16f) {
            while(lag+1<hi && diff[(size_t)(lag+1)]<diff[(size_t)lag])++lag;
            best=lag;break;
        }
        if(best<0) {voiced=false;confidence=0;return;}
        confidence=1-diff[(size_t)best];voiced=confidence>.8f;
        float a=diff[(size_t)(best-1)], b=diff[(size_t)best], c=diff[(size_t)(best+1)];
        float denom=a-2*b+c;
        float offset=std::abs(denom)>1e-8f?.5f*(a-c)/denom:0;
        float hz=(float)rate/(best+std::clamp(offset,-.5f,.5f));
        if(voiced)note=69+12*std::log2(hz/440);
    }
};
struct VoiceSettings {float level=.5f, pan=0, formant=0, delay=0; int degree=0; bool mute=false,solo=false;};
struct Settings {
    int raga=0;
    int aiCast=0; // mixed / female / male / alternating female-male references
    int root=0, scale=0, mode=0, count=8; // scale harmony / MIDI chord / fixed chromatic intervals
    float mix=1, gain=.7f, detune=5, vibrato=2, human=14, width=.8f, correction=.85f, glide=50, space=.12f, weird=0;
    bool formantLock=true, aiPrint=false;
    float aiBlend=1;
    std::array<VoiceSettings,voices> voice{};
    std::array<int,128> midi{}; int midiCount=0;
};
inline bool voiceAudible(const Settings& s,int index){
    if(index<0 || index>=s.count || index>=voices || s.voice[(size_t)index].mute)return false;
    bool anySolo=false;for(int v=0;v<std::min(voices,s.count);++v)anySolo|=s.voice[(size_t)v].solo;
    return !anySolo || s.voice[(size_t)index].solo;
}
class Engine {
    std::array<signalsmith::stretch::SignalsmithStretch<float>,voices> shifters;
    std::array<std::array<float,128>,voices> shifted{};
    std::array<std::vector<float>,voices> delay;
    std::array<int,voices> pos{};
    std::array<bool,voices> running{};
    std::array<float,voices> smoothShift{}, phase{},smoothLevel{},smoothPan{},smoothDelay{};
    std::array<float,128> mono{};
    std::array<std::vector<float>,2> dryDelay;
    int dryPos=0,lat=0;
    double sr=48000;
    juce::Reverb reverb;
    float mixSmooth=1,gainSmooth=.7f;
public:
    PitchTracker pitch;
    std::atomic<float> detectedNote{60}, pitchConfidence{0};
    int latency()const {return lat;}
    void prepare(double sampleRate) {
        sr=sampleRate>0?sampleRate:48000;pitch.prepare(sr);
        for(int v=0;v<voices;++v) {
            // Fixed quality/latency across presets and snapshot morphs.
            int window=juce::nextPowerOfTwo((int)(sr*.16));
            shifters[(size_t)v].configure(1,window,window/8,true);
            shifters[(size_t)v].reset();
            lat=shifters[(size_t)v].inputLatency()+shifters[(size_t)v].outputLatency();
            delay[(size_t)v].assign((size_t)(sr*.16)+256,0);pos[(size_t)v]=0;running[(size_t)v]=false;
            smoothShift[(size_t)v]=0;phase[(size_t)v]=v*.618f;smoothLevel[(size_t)v]=0;smoothPan[(size_t)v]=0;smoothDelay[(size_t)v]=0;
        }
        for(auto& d:dryDelay)d.assign((size_t)lat+1,0);
        dryPos=0;mixSmooth=1;gainSmooth=.7f;reverb.setSampleRate(sr);reverb.reset();
    }
    // Optional stems: 48 channels, stereo per voice. Caller preallocates and clears.
    void process(float* left,float* right,int count,const Settings& s,float* const* stems=nullptr,const float* const* voiceInputs=nullptr,bool dryMonitor=false) {
        std::array<bool,voices> audible{};for(int v=0;v<voices;++v)audible[(size_t)v]=voiceAudible(s,v);
        juce::Reverb::Parameters rp;rp.roomSize=.72f;rp.damping=.6f;rp.wetLevel=s.space*.32f;rp.dryLevel=1;rp.width=1;reverb.setParameters(rp);
        for(int start=0;start<count;start+=128) {
            int n=std::min(128,count-start);
            for(int i=0;i<n;++i) {mono[(size_t)i]=.5f*(left[start+i]+right[start+i]);pitch.push(mono[(size_t)i]);}
            detectedNote.store(pitch.note,std::memory_order_relaxed);pitchConfidence.store(pitch.confidence,std::memory_order_relaxed);
            for(int v=0;v<voices;++v) {
                bool needed=v<s.count||smoothLevel[(size_t)v]>1e-5f;
                if(!needed){shifted[(size_t)v].fill(0);if(running[(size_t)v]){shifters[(size_t)v].reset();std::fill(delay[(size_t)v].begin(),delay[(size_t)v].end(),0.f);running[(size_t)v]=false;}continue;}
                running[(size_t)v]=true;
                const auto& vs=s.voice[(size_t)v];
                float target=pitch.note;
                if(s.mode==1) {
                    if(s.midiCount>0) target=(float)s.midi[(size_t)(v%s.midiCount)];
                } else if(s.mode==2) target=pitch.note+vs.degree;
                else {
                    float scaleTarget=(float)degreeNote(pitch.note,vs.degree,s.root,s.scale);
                    // Preserve expression at low correction; 100% lands on the selected scale.
                    float sourceSnap=(float)nearest(pitch.note,s.root,s.scale);
                    target=scaleTarget+(1-s.correction)*(pitch.note-sourceSnap);
                }
                phase[(size_t)v]+=(float)(n/sr)*(4.7f+.13f*v);
                phase[(size_t)v]-=std::floor(phase[(size_t)v]);
                float wave=std::sin(phase[(size_t)v]*juce::MathConstants<float>::twoPi);
                float cents=s.detune*((v%2?1.f:-1.f)*(.35f+.65f*(v%8)/7.f))+s.vibrato*wave;
                float desired=voiceInputs?0.f:std::clamp(target-pitch.note+cents*.01f,-24.f,24.f);
                if(!pitch.voiced) desired=smoothShift[(size_t)v];
                float coeff=1-std::exp(-(float)n/((float)sr*std::max(.005f,s.glide*.001f)));
                smoothShift[(size_t)v]+=coeff*(desired-smoothShift[(size_t)v]);
                shifters[(size_t)v].setTransposeSemitones(smoothShift[(size_t)v]);
                shifters[(size_t)v].setFormantSemitones(vs.formant+s.weird*wave*9,s.formantLock);
                shifters[(size_t)v].setFormantBase(440*std::pow(2.f,(pitch.note-69)/12)/(float)sr);
                const float* in[1]{voiceInputs?voiceInputs[v]+start:mono.data()};float* out[1]{shifted[(size_t)v].data()};
                shifters[(size_t)v].process(in,n,out,n);
            }
            for(int i=0;i<n;++i) {
                float dl=dryDelay[0][(size_t)dryPos],dr=dryDelay[1][(size_t)dryPos];
                dryDelay[0][(size_t)dryPos]=left[start+i];dryDelay[1][(size_t)dryPos]=right[start+i];dryPos=(dryPos+1)%lat;
                float wl=0,wr=0, norm=0;
                for(int v=0;v<voices;++v)norm+=smoothLevel[(size_t)v]*smoothLevel[(size_t)v];
                norm=1/std::sqrt(std::max(1.f,norm));
                for(int v=0;v<voices;++v) {
                    const auto& vs=s.voice[(size_t)v];auto& d=delay[(size_t)v];int& p=pos[(size_t)v];
                    d[(size_t)p]=shifted[(size_t)v][(size_t)i];
                    float ms=vs.delay+s.human*(.1f+.9f*(v%8)/7.f);
                    smoothDelay[(size_t)v]+=.001f*(ms-smoothDelay[(size_t)v]);
                    float offset=std::clamp(smoothDelay[(size_t)v]*(float)sr*.001f,0.f,(float)d.size()-2);
                    float read=(float)p-offset;while(read<0)read+=(float)d.size();
                    int a=(int)read;float frac=read-a;
                    float val=d[(size_t)a]+frac*(d[(size_t)((a+1)%(int)d.size())]-d[(size_t)a]);p=(p+1)%(int)d.size();
                    const float targetLevel=audible[(size_t)v]?vs.level:0;
                    smoothLevel[(size_t)v]+=.002f*(targetLevel-smoothLevel[(size_t)v]);
                    smoothPan[(size_t)v]+=.002f*(vs.pan*s.width-smoothPan[(size_t)v]);
                    val*=smoothLevel[(size_t)v]*norm;
                    // Bounded nonlinear texture; wet-only, no feedback loop.
                    val=(1-s.weird)*val+s.weird*std::tanh(val*(1+8*s.weird));
                    float angle=(smoothPan[(size_t)v]+1)*juce::MathConstants<float>::pi*.25f;
                    float l=val*std::cos(angle),r=val*std::sin(angle);wl+=l;wr+=r;
                    if(stems) {stems[v*2][start+i]=l;stems[v*2+1][start+i]=r;}
                }
                reverb.processStereo(&wl,&wr,1);
                mixSmooth+=.002f*(s.mix-mixSmooth);gainSmooth+=.002f*(s.gain-gainSmooth);
                float l=dryMonitor?dl:(dl*(1-mixSmooth)+wl*mixSmooth)*gainSmooth;
                float r=dryMonitor?dr:(dr*(1-mixSmooth)+wr*mixSmooth)*gainSmooth;
                left[start+i]=std::isfinite(l)?(dryMonitor?l:std::clamp(l,-1.f,1.f)):0;
                right[start+i]=std::isfinite(r)?(dryMonitor?r:std::clamp(r,-1.f,1.f)):0;
            }
        }
    }
};
}
