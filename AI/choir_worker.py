"""Local Seed-VC singing renderer. Invoked out-of-process, never on the audio thread."""
from pathlib import Path
import argparse, contextlib, hashlib, json, math, os, sys, time, traceback

SCALES = [(0,2,4,5,7,9,11),(0,2,3,5,7,8,10),(0,2,3,5,7,8,11),
          (0,2,3,5,7,9,11),(0,2,3,5,7,9,10),(0,2,4,5,7,9,10),
          (0,2,4,7,9),(0,3,5,7,10),tuple(range(12)),(0,2,4,6,7,9,11),(0,1,3,5,7,8,10),
          (0,1,4,5,7,8,11),(0,3,5,8,10),(0,2,5,7,9),(0,2,4,7,11)]

def nearest(note, root, scale):
    candidates = [n for n in range(math.floor(note)-12, math.ceil(note)+13)
                  if (n-root)%12 in SCALES[scale]]
    return min(candidates, key=lambda n:abs(n-note))

def select_reference(manifest, cast, index):
    """Select attributed corpus references using corpus metadata, not vocal inference."""
    voices=manifest['voices']
    if cast==0:
        return voices[index%len(voices)]
    if cast not in (1,2,3):
        raise ValueError('Unknown AI voice cast')
    gender=('F' if index%2==0 else 'M') if cast==3 else ('F' if cast==1 else 'M')
    pool=[v for v in voices if v.get('corpus_gender')==gender]
    if not pool:
        raise ValueError('Voice pack lacks cast metadata. Run Setup AI.command from the new package.')
    return pool[(index//2 if cast==3 else index)%len(pool)]

def harmony_contour(f0, settings, index, seconds):
    """Constrain the model's F0 condition itself, before neural synthesis."""
    import numpy as np
    values = np.asarray(f0, dtype=np.float64)
    output = np.zeros_like(values)
    voice = settings['voices'][index]
    degree = int(voice['degree'])
    root, scale = int(settings['root']), int(settings['scale'])
    previous = None
    hop = seconds / max(1, len(values))
    coefficient = 1-math.exp(-hop/max(.005, float(settings['glide'])*.001))
    for i, hz in enumerate(values):
        if hz <= 1 or not np.isfinite(hz):
            continue
        source = 69+12*math.log2(hz/440)
        if int(settings['mode']) == 2:
            desired = source+degree
        else:
            anchor = nearest(source, root, scale)
            target = anchor
            step = 1 if degree >= 0 else -1
            for _ in range(abs(degree)):
                target += step
                while (target-root)%12 not in SCALES[scale]:
                    target += step
            desired = target+(1-float(settings['correction']))*(source-anchor)
        desired = source+max(-24,min(24,desired-source))
        cents = float(settings['detune'])*(1 if index%2 else -1)*(.35+.65*index/7)
        cents += float(settings['vibrato'])*math.sin(2*math.pi*((4.7+.13*index)*i*hop+index*.618))
        desired += cents/100
        previous = desired if previous is None else previous+coefficient*(desired-previous)
        output[i] = 440*2**((previous-69)/12)
    return output.astype('float32')

def status(path, phase, progress, message=''):
    target = Path(path);temporary=target.with_suffix('.tmp')
    temporary.write_text(json.dumps({'phase':phase,'progress':float(progress),'message':message,'updated_at':time.time()}))
    temporary.replace(target)

def load_runtime(root, online=False):
    os.environ.setdefault('PYTORCH_ENABLE_MPS_FALLBACK','1')
    os.environ.setdefault('HF_HUB_DISABLE_XET','1')
    os.environ.setdefault('HF_HOME',str(root/'hf'))
    if not online:
        os.environ['HF_HUB_OFFLINE']='1';os.environ['TRANSFORMERS_OFFLINE']='1'
    source = root/'seed-vc-local'
    os.chdir(source);sys.path.insert(0,str(source))
    import torch
    torch.set_num_threads(2)
    import inference
    from types import SimpleNamespace
    args = SimpleNamespace(fp16=False,f0_condition=True,checkpoint=None,config=None)
    return inference, inference.load_models(args)

def render(job, root):
    import fcntl
    import numpy as np
    import soundfile as sf
    import librosa
    from types import SimpleNamespace
    folder=Path(job['output']);folder.mkdir(parents=True,exist_ok=True)
    report=folder/'AI Status.json'
    status(report,'waiting',.01,'Waiting for the local AI renderer')
    # Serialize model loads across instances to protect a 16GB Mac's memory.
    with (root/'renderer.lock').open('w') as lock:
        fcntl.flock(lock,fcntl.LOCK_EX)
        status(report,'loading',.03,'Loading the local singing model')
        started=time.perf_counter()
        inference, loaded = load_runtime(root,online=os.environ.get("VOXGARDEN_SETUP_ONLINE")=="1")
        manifest=json.loads((root/'voices/manifest.json').read_text())
        count=int(job['settings']['count'])
        source_path=Path(job['source'])
        audio,sample_rate=sf.read(source_path,always_2d=True,dtype='float32')
        original_length=len(audio);seconds=original_length/sample_rate
        if seconds < .25:
            raise RuntimeError('Capture at least a quarter second of vocal before using AI print.')
        # Silence must remain silence: avoid neural hallucination in empty takes.
        silent=float(np.max(np.abs(audio)))<1e-5
        extractor=loaded[2]
        def native_f0(wave,thred=.03):
            # RMVPE's full-song activation memory is large on a 16GB Mac.
            # Keep its analysis bounded while retaining 0.2s of boundary context.
            if wave.numel()<=16000*25:
                return np.asarray(extractor(wave,thred=thred),dtype=np.float32)
            total=wave.numel()//160+1;chunks=[];step=16000*25
            for at in range(0,wave.numel(),step):
                begin=max(0,at-3200);end=min(wave.numel(),at+step+3200)
                values=np.asarray(extractor(wave[begin:end],thred=thred),dtype=np.float32)
                offset=(at-begin)//160;take=min(step//160,total-sum(len(x) for x in chunks))
                chunks.append(values[offset:offset+take])
            values=np.concatenate(chunks)
            return np.pad(values,(0,max(0,total-len(values))))[:total]
        # Seed-VC exposes its diffusion loop through this module's tqdm binding.
        # Report real completed steps; never simulate progress during inference.
        import modules.flow_matching as flow_matching
        active_index=0;part=0
        def report_stage(message):
            status(report,'converting',.08+.87*active_index/count,
                   f'Singer {active_index+1}/{count} - {message}')
        original_tqdm=flow_matching.tqdm
        def tracked_steps(iterable,*args,**kwargs):
            nonlocal part
            part+=1;total=len(iterable)
            for completed,item in enumerate(original_tqdm(iterable,*args,**kwargs)):
                report_stage(f'synthesis part {part}, step {completed}/{total}')
                yield item
                report_stage(f'synthesis part {part}, step {completed+1}/{total}')
        flow_matching.tqdm=tracked_steps
        semantic=loaded[1]
        semantic_cache={}
        source_f0=None
        def cached_semantic(wave):
            report_stage('analysing vocal content')
            key=hashlib.sha256(wave.detach().cpu().numpy().tobytes()).digest()
            if key not in semantic_cache:
                semantic_cache[key]=semantic(wave)
            return semantic_cache[key]
        results=[]
        for index in range(count):
            active_index=index;part=0
            status(report,'converting',.08+.87*index/count,f'Synthesizing singer {index+1} of {count}')
            selected=select_reference(manifest,int(job['settings'].get('aiCast',0)),index)
            reference=root/'voices'/selected['file']
            voice_dir=folder/f'voice-{index+1:02d}';voice_dir.mkdir(exist_ok=True)
            if silent or float(job['settings']['voices'][index]['level'])<.0001:
                result=np.zeros(original_length,dtype='float32')
            else:
                calls=0
                def conditioned_f0(wave, thred=.03):
                    nonlocal calls,source_f0
                    calls+=1
                    report_stage('analysing reference pitch' if calls==1 else 'mapping vocal pitch to your scale')
                    if calls==1:
                        return np.asarray(native_f0(wave,thred=thred),dtype=np.float32)
                    if source_f0 is None:
                        source_f0=native_f0(wave,thred=thred)
                    return harmony_contour(source_f0,job['settings'],index,seconds)
                modified=list(loaded);modified[1]=cached_semantic;modified[2]=conditioned_f0
                inference.load_models=lambda args:tuple(modified)
                import torch
                torch.manual_seed(7001+index)
                args=SimpleNamespace(fp16=False,f0_condition=True,checkpoint=None,config=None,
                    auto_f0_adjust=False,semi_tone_shift=0,source=str(source_path),target=str(reference),
                    diffusion_steps=int(job.get('steps',20)),length_adjust=1.0,
                    inference_cfg_rate=.7,output=str(voice_dir))
                inference.main(args)
                files=list(voice_dir.glob('vc_*.wav'))
                if len(files)!=1:
                    raise RuntimeError('The singing model did not return one vocal file.')
                report_stage('aligning converted voice')
                result,model_sr=sf.read(files[0],dtype='float32',always_2d=True)
                result=result.mean(axis=1)
                if model_sr!=sample_rate:
                    result=librosa.resample(result,orig_sr=model_sr,target_sr=sample_rate)
                if not np.isfinite(result).all():
                    raise RuntimeError('The AI model produced non-finite audio.')
                # Fix rounding at model frame boundaries without time stretching.
                result=np.pad(result,(0,max(0,original_length-len(result))))[:original_length]
                # Match source RMS, preserving the plugin's level control semantics.
                input_mono=audio.mean(axis=1)
                source_rms=float(np.sqrt(np.mean(input_mono**2)))
                output_rms=float(np.sqrt(np.mean(result**2)))
                if output_rms>1e-7:
                    result*=min(4.,source_rms/output_rms)
                from scipy.ndimage import uniform_filter1d
                activity=np.sqrt(np.maximum(0,uniform_filter1d(input_mono**2,size=max(1,int(sample_rate*.03)))))
                result*=np.clip(activity/.001,0,1)
                peak=float(np.max(np.abs(result)))
                if peak>.97:
                    result*=.97/peak
            target=folder/f'AI Singer {index+1}.wav'
            sf.write(target,result,sample_rate,subtype='PCM_24')
            status(report,'converting',.08+.87*(index+1)/count,f'Singer {index+1}/{count} saved')
            results.append({'slot':index+1,'reference':selected['id'],'file':str(target)})
        (folder/'AI Provenance.json').write_text(json.dumps({'engine':'Seed-VC singing 44.1kHz',
            'source':str(source_path),'source_rate':sample_rate,'source_samples':original_length,
            'elapsed_seconds':time.perf_counter()-started,'device':str(inference.device),
            'references':results,'settings':job['settings'],'reference_license':manifest['license'],
            'attribution':manifest['attribution'],'timing':'End padded or cropped for model frame rounding; no global time stretching.'},indent=2))
        status(report,'complete',1,'AI singers ready for the final choir mix')

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--job',required=True);parser.add_argument('--root',required=True)
    args=parser.parse_args();root=Path(args.root).resolve();job=json.loads(Path(args.job).read_text())
    output=Path(job['output']);output.mkdir(parents=True,exist_ok=True)
    with (output/'AI Render.log').open('a',buffering=1) as log,contextlib.redirect_stdout(log),contextlib.redirect_stderr(log):
        try:
            render(job,root)
        except Exception as exc:
            traceback.print_exc();status(output/'AI Status.json','failed',0,str(exc));return 1
    return 0

if __name__=='__main__':
    raise SystemExit(main())
