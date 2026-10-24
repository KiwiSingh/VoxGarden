from pathlib import Path
import argparse,json,os,time

def prepare(root):
    root=Path(root).resolve();root.mkdir(parents=True,exist_ok=True)
    from download_voices import prepare as voices
    voices(root/'voices')
    import soundfile as sf
    from choir_worker import render
    source=root/'diagnostics/Reference input.wav';source.parent.mkdir(parents=True,exist_ok=True)
    audio,sr=sf.read(root/'voices/p226.wav',dtype='float32');sf.write(source,audio[:int(sr*2.5)],sr)
    settings={'root':0,'scale':0,'mode':2,'count':1,'correction':1,'glide':35,'detune':0,'vibrato':0,'voices':[{'degree':0,'level':.6} for _ in range(8)]}
    os.environ['VOXGARDEN_SETUP_ONLINE']='1'
    print('Loading model assets and testing one local singing-voice conversion. This may take several minutes.',flush=True)
    render({'source':str(source),'output':str(root/'diagnostics/result'),'settings':settings,'steps':4},root)
    ready={'schema':1,'voice_cast_version':1,'raga_version':1,'engine':'Seed-VC SVC 44.1kHz','seed_vc_commit':'51383efd921027683c89e5348211d93ff12ac2a8','reference_pack':'VCTK CC-BY-4.0','tested_at':time.time()}
    (root/'READY.json').write_text(json.dumps(ready,indent=2))
    print('AI singers are ready. Enable AI singers on print in VoxGarden. Live playback remains the DSP preview.',flush=True)

if __name__=='__main__':
    ap=argparse.ArgumentParser();ap.add_argument('--root',required=True)
    prepare(ap.parse_args().root)
