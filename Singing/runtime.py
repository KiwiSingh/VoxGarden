"""VoxGarden fast score renderer. Neural pronunciation via Supertonic/Core ML;
score pitch/duration via PSOLA. No Seed-VC and no user-audio network requests.
"""
from pathlib import Path
import argparse, hashlib, json, os, sys, time, traceback
ROOT=Path(__file__).resolve().parent
REVISION='724fb5abbf5502583fb520898d45929e62f02c0b'

def status(path,phase,progress,message,**extra):
    path=Path(path);path.parent.mkdir(parents=True,exist_ok=True)
    temporary=path.with_suffix('.tmp');temporary.write_text(json.dumps(dict(phase=phase,progress=progress,message=message,**extra)),encoding='utf-8');temporary.replace(path)

def download(report):
    import urllib.request,truststore
    truststore.inject_into_ssl()
    manifest=json.loads((ROOT/'model-manifest.json').read_text());total=sum(f['size'] for f in manifest['files']);done=0
    for entry in manifest['files']:
        path=ROOT/'model'/entry['path'];path.parent.mkdir(parents=True,exist_ok=True)
        if path.exists() and path.stat().st_size==entry['size'] and hashlib.sha256(path.read_bytes()).hexdigest()==entry['sha256']:
            done+=entry['size'];continue
        temporary=path.with_suffix(path.suffix+'.download')
        url=f"https://huggingface.co/{manifest['repo']}/resolve/{manifest['revision']}/{entry['path']}"
        size=temporary.stat().st_size if temporary.exists() else 0
        request=urllib.request.Request(url,headers={'Range':f'bytes={size}-'} if size else {})
        try:response=urllib.request.urlopen(request,timeout=60)
        except Exception:
            if size>=entry['size']:temporary.unlink();size=0;response=urllib.request.urlopen(url,timeout=60)
            else:raise
        if response.status!=206:size=0
        with response,temporary.open('ab' if size else 'wb') as output:
            while data:=response.read(1024*1024):
                output.write(data);size+=len(data)
                status(report,'downloading',(done+size)/total,f'Downloading models: {(done+size)/1e6:.1f} / {total/1e6:.1f} MB')
        if temporary.stat().st_size!=entry['size'] or hashlib.sha256(temporary.read_bytes()).hexdigest()!=entry['sha256']:
            temporary.unlink();raise RuntimeError('Model checksum mismatch; retry the download.')
        temporary.replace(path);done+=entry['size']

SESSIONS=[]
def engine(profile=False):
    import onnxruntime as ort
    if 'CoreMLExecutionProvider' not in ort.get_available_providers():
        raise RuntimeError('Core ML GPU backend is unavailable. CPU-only neural rendering is not enabled.')
    original=ort.InferenceSession
    class MetalPreferredSession(original):
        def __init__(self,path,sess_options=None,providers=None,**kwargs):
            opts=ort.SessionOptions();opts.intra_op_num_threads=2;opts.inter_op_num_threads=1;opts.log_severity_level=3
            opts.enable_profiling=profile;opts.profile_file_prefix=str(ROOT/'diagnostics/ort')
            actual=[('CoreMLExecutionProvider',{'ModelFormat':'NeuralNetwork','MLComputeUnits':'CPUAndGPU','RequireStaticInputShapes':'0','ModelCacheDirectory':str(ROOT/'coreml-cache')}),'CPUExecutionProvider']
            super().__init__(str(path),sess_options=opts,providers=actual)
            if 'CoreMLExecutionProvider' not in self.get_providers():raise RuntimeError('Core ML could not initialise; refusing a silent CPU-only render.')
            SESSIONS.append(self)
    ort.InferenceSession=MetalPreferredSession
    from supertonic import TTS
    (ROOT/'diagnostics').mkdir(exist_ok=True)
    return TTS(model_dir=ROOT/'model',auto_download=False,intra_op_num_threads=2,inter_op_num_threads=1)

def profile_report():
    totals={}
    for session in SESSIONS:
        filename=session.end_profiling()
        if not filename:continue
        for event in json.loads(Path(filename).read_text()):
            provider=event.get('args',{}).get('provider')
            if provider:totals[provider]=totals.get(provider,0)+event.get('dur',0)
        Path(filename).unlink(missing_ok=True)
    return totals

def _setup_locked(report):
    download(report);status(report,'testing',-1,'Compiling GPU models and checking a first voice…')
    import numpy as np
    tts=engine(profile=True);style=tts.get_voice_style('F1');measurements=[]
    for i in range(2):
        np.random.seed(42);start=time.perf_counter();audio,duration=tts.synthesize('Hello my new choir.',voice_style=style,lang='en',total_steps=5);elapsed=time.perf_counter()-start
        measurements.append(dict(seconds=elapsed,audio_seconds=float(np.asarray(duration).flat[0])))
    providers=profile_report()
    if providers.get('CoreMLExecutionProvider',0)<=0:raise RuntimeError('GPU check found no Core ML execution. Models were kept, but setup is not marked ready.')
    ready=dict(schema=1,engine='Supertonic 3 + PSOLA',model_revision=REVISION,backend='Core ML • GPU preferred',providers_microseconds=providers,benchmarks=measurements,tested_at=time.time())
    (ROOT/'READY.json').write_text(json.dumps(ready,indent=2));status(report,'ready',1,'Ready • Core ML GPU preferred',benchmark=ready)

def setup(report):
    import fcntl
    status(report,'waiting',-1,'Waiting for the GPU singing renderer…')
    with (ROOT/'renderer.lock').open('w') as lock:
        fcntl.flock(lock,fcntl.LOCK_EX)
        _setup_locked(report)


def retime(audio,sr,note,duration):
    import numpy as np,parselmouth
    from parselmouth.praat import call
    audio=np.asarray(audio,dtype=np.float64).reshape(-1)
    if not np.isfinite(audio).all():raise RuntimeError('Speech model returned invalid samples.')
    active=np.flatnonzero(np.abs(audio)>.002)
    if not len(active):raise RuntimeError('Speech model returned silence for a syllable.')
    first=max(0,int(active[0])-int(sr*.015));last=min(len(audio),int(active[-1])+int(sr*.025));audio=audio[first:last]
    sound=parselmouth.Sound(audio,sampling_frequency=sr)
    if sound.duration<.065:raise RuntimeError('Syllable was too short to retime. Try a complete pronounced syllable.')
    manipulation=call(sound,'To Manipulation',.005,60,1200)
    pitch=call(manipulation,'Extract pitch tier');call(pitch,'Remove points between',0,sound.duration)
    hz=440*2**((int(note)-69)/12)
    call(pitch,'Add point',.001,hz);call(pitch,'Add point',sound.duration-.001,hz);call([pitch,manipulation],'Replace pitch tier')
    timing=call(manipulation,'Extract duration tier');call(timing,'Add point',sound.duration/2,duration/sound.duration);call([timing,manipulation],'Replace duration tier')
    sung=call(manipulation,'Get resynthesis (overlap-add)').values.mean(axis=0)
    length=round(duration*sr);sung=np.pad(sung,(0,max(0,length-len(sung))))[:length]
    fade=min(round(sr*.006),length//2)
    if fade:sung[:fade]*=np.linspace(0,1,fade);sung[-fade:]*=np.linspace(1,0,fade)
    peak=np.max(np.abs(sung));return (sung*min(1,.95/max(peak,1e-8))).astype('float32')

def render(job,report):
    import numpy as np,soundfile as sf,fcntl
    score=job['notes'];language=job['language'];voice=job.get('voice','F1')
    if language not in ('en','hi','de','ja','ru','nl') or voice not in [f'{s}{i}' for s in ('F','M') for i in range(1,6)]:raise ValueError('Invalid pronunciation language or voice.')
    if not 1<=len(score)<=2048:raise ValueError('Use 1–2048 notes.')
    end=0
    for note in score:
        start,duration,pitch=note['start'],note['duration'],note['midi']
        if not all(np.isfinite(x) for x in (start,duration,pitch)) or start<0 or duration<.06 or start+duration>300 or not 36<=pitch<=96 or pitch!=int(pitch) or not note['syllable'].strip() or len(note['syllable'])>160:raise ValueError('Invalid score.')
        end=max(end,start+duration)
    started=time.perf_counter();status(report,'waiting',-1,'Waiting for the GPU singing renderer…')
    with (ROOT/'renderer.lock').open('w') as lock:
        fcntl.flock(lock,fcntl.LOCK_EX)
        status(report,'loading',0,'Loading the GPU speech model…');tts=None;sr=44100;cache=ROOT/'syllable-cache';cache.mkdir(exist_ok=True)
        output=np.zeros(round(end*sr),dtype='float32');neural_seconds=0;new_syllables=0
        for i,note in enumerate(score):
            key=hashlib.sha256(json.dumps([REVISION,voice,language,note['syllable'],5],ensure_ascii=False).encode()).hexdigest();path=cache/f'{key}.wav'
            status(report,'rendering',i/len(score),f'Syllable {i+1}/{len(score)} • Core ML GPU preferred')
            if not path.exists():
                if tts is None:tts=engine();sr=tts.sample_rate
                np.random.seed(int(key[:8],16));start=time.perf_counter();audio,duration=tts.synthesize(note['syllable'],voice_style=tts.get_voice_style(voice),lang=language,total_steps=5)
                neural_seconds+=time.perf_counter()-start;new_syllables+=1
                audio=np.asarray(audio).reshape(-1)[:round(float(np.asarray(duration).flat[0])*sr)]
                temporary=path.with_suffix('.tmp.wav');sf.write(temporary,audio,sr,subtype='PCM_24');temporary.replace(path)
            os.utime(path,None)
            audio,rate=sf.read(path,dtype='float32')
            if rate!=sr:raise RuntimeError('Syllable cache sample rate mismatch.')
            sung=retime(audio,sr,note['midi'],note['duration']);offset=round(note['start']*sr);count=min(len(sung),len(output)-offset);output[offset:offset+count]+=sung[:count]
        peak=float(np.max(np.abs(output)));mix_gain=min(1.,.95/max(peak,1e-8));output*=mix_gain
        folder=Path(job['output']);folder.mkdir(parents=True,exist_ok=True);target=folder/'Dry Singing.wav';temporary=folder/'Dry Singing.partial.wav';sf.write(temporary,output,sr,subtype='PCM_24');temporary.replace(target)
        entries=sorted(cache.glob('*.wav'),key=lambda p:p.stat().st_mtime);size=sum(p.stat().st_size for p in entries)
        for entry in entries:
            if size<=512*1024*1024:break
            size-=entry.stat().st_size;entry.unlink()
        stats=dict(mix_gain=mix_gain,wall_seconds=time.perf_counter()-started,neural_seconds=neural_seconds,new_syllables=new_syllables,cached_syllables=len(score)-new_syllables,audio_seconds=len(output)/sr,backend='Core ML GPU preferred; CPU PSOLA timing/pitch')
        (folder/'Render Stats.json').write_text(json.dumps(stats,indent=2));status(report,'complete',1,'Dry singing saved.',**stats)

if __name__=='__main__':
    ap=argparse.ArgumentParser();ap.add_argument('--setup',action='store_true');ap.add_argument('--job');ap.add_argument('--status',required=True);args=ap.parse_args()
    try:
        if args.setup:setup(args.status)
        else:render(json.loads(Path(args.job).read_text()),args.status)
    except Exception as error:
        traceback.print_exc();status(args.status,'failed',0,str(error));sys.exit(1)
