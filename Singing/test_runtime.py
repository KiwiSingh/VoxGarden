"""Offline overlap regression: cached neural syllables exercise the real mixer/PSOLA path."""
import hashlib, importlib.util, json, tempfile, unittest
from pathlib import Path
import numpy as np
import soundfile as sf
spec=importlib.util.spec_from_file_location('singing_runtime',Path(__file__).with_name('runtime.py'))
runtime=importlib.util.module_from_spec(spec);spec.loader.exec_module(runtime)
class OverlapTests(unittest.TestCase):
    def test_chord_and_long_early_note_survive_mix(self):
        original=runtime.ROOT
        with tempfile.TemporaryDirectory() as temporary:
            runtime.ROOT=Path(temporary);cache=runtime.ROOT/'syllable-cache';cache.mkdir()
            key=hashlib.sha256(json.dumps([runtime.REVISION,'F1','en','la',5],ensure_ascii=False).encode()).hexdigest()
            sr=44100;t=np.arange(sr)/sr;sf.write(cache/f'{key}.wav',.3*np.sin(2*np.pi*180*t),sr)
            notes=[dict(start=0,duration=2,midi=n,syllable='la') for n in (60,64,67)]
            notes.append(dict(start=.5,duration=.2,midi=69,syllable='la'))
            out=runtime.ROOT/'result';runtime.render(dict(notes=notes,language='en',voice='F1',output=str(out)),runtime.ROOT/'status.json')
            audio,rate=sf.read(out/'Dry Singing.wav');self.assertEqual(len(audio),2*sr);self.assertTrue(np.isfinite(audio).all());self.assertLessEqual(np.max(np.abs(audio)),.951)
            window=audio[int(.9*sr):int(1.5*sr)];spectrum=np.abs(np.fft.rfft(window*np.hanning(len(window))));frequencies=np.fft.rfftfreq(len(window),1/rate)
            for midi in (60,64,67):
                hz=440*2**((midi-69)/12);band=np.abs(frequencies-hz)<4;self.assertGreater(spectrum[band].max(),spectrum.max()*.05,f'Chord note {midi} was lost')
            stack=[dict(start=0,duration=.5,midi=60,syllable='la') for _ in range(12)]
            runtime.render(dict(notes=stack,language='en',voice='F1',output=str(out)),runtime.ROOT/'status.json')
            clipped,_=sf.read(out/'Dry Singing.wav');self.assertLessEqual(np.max(np.abs(clipped)),.951)
            self.assertLess(json.loads((out/'Render Stats.json').read_text())['mix_gain'],1.)
        runtime.ROOT=original
    def test_velocity_and_leader_gain_reach_audio(self):
        original=runtime.ROOT
        with tempfile.TemporaryDirectory() as temporary:
            self.addCleanup(setattr,runtime,'ROOT',original)
            runtime.ROOT=Path(temporary);cache=runtime.ROOT/'syllable-cache';cache.mkdir()
            key=hashlib.sha256(json.dumps([runtime.REVISION,'F1','en','la',5],ensure_ascii=False).encode()).hexdigest()
            sr=44100;t=np.arange(sr)/sr;sf.write(cache/f'{key}.wav',.1*np.sin(2*np.pi*180*t),sr)
            out=runtime.ROOT/'result'
            def render(velocity,gain=1):
                runtime.render(dict(notes=[dict(start=0,duration=.5,midi=60,syllable='la',velocity=velocity,gain=gain)],language='en',voice='F1',output=str(out)),runtime.ROOT/'status.json')
                return sf.read(out/'Dry Singing.wav')[0]
            full=render(127);quiet=render(64);harmony=render(127,.55)
            self.assertAlmostEqual(np.linalg.norm(quiet)/np.linalg.norm(full),64/127,places=3)
            self.assertAlmostEqual(np.linalg.norm(harmony)/np.linalg.norm(full),.55,places=3)
            with self.assertRaises(ValueError):render(128)
if __name__=='__main__':unittest.main()
