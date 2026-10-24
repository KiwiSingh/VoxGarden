import unittest
import numpy as np
from choir_worker import harmony_contour,nearest,SCALES,select_reference
class Contours(unittest.TestCase):
    def settings(self,degree=0):
        return {'root':0,'scale':0,'mode':0,'glide':5,'correction':1,'detune':0,'vibrato':0,'voices':[{'degree':degree,'level':.6} for _ in range(8)]}
    def test_all_scales(self):
        for root in range(12):
            for scale in range(len(SCALES)):
                for degree in [-4,0,2,4]:
                    s=self.settings(degree);s.update(root=root,scale=scale)
                    f=harmony_contour(np.full(40,220.),s,0,1)
                    note=round(69+12*np.log2(f[-1]/440))
                    self.assertIn((note-root)%12,SCALES[scale])
    def test_major_minor(self):
        s=self.settings(2);s['root']=9;s['scale']=1
        f=harmony_contour(np.full(40,220.),s,0,1)
        self.assertAlmostEqual(float(f[-1]),261.625565,places=3)
    def test_raga_notes(self):
        s=self.settings(3);s['scale']=9
        c=261.625565
        f=harmony_contour(np.full(40,c),s,0,1)
        self.assertAlmostEqual(float(f[-1]),369.994423,places=3)
        for scale in (10,11,12,13,14):
            s=self.settings(0);s['scale']=scale
            f=harmony_contour(np.full(40,280.),s,0,1)
            self.assertIn(round(69+12*np.log2(f[-1]/440))%12,SCALES[scale])
    def test_unvoiced(self):
        f=harmony_contour(np.array([0.,220.,0.,220.]),self.settings(),0,1)
        self.assertEqual(f[0],0);self.assertEqual(f[2],0)
    def test_chromatic(self):
        s=self.settings(12);s['mode']=2
        f=harmony_contour(np.full(40,220.),s,0,1)
        self.assertAlmostEqual(float(f[-1]),440.,places=3)
class Casts(unittest.TestCase):
    def setUp(self):
        import json
        from pathlib import Path
        self.manifest=json.loads((Path(__file__).parent/'Voices/manifest.json').read_text())
    def test_original(self):
        for i,voice in enumerate(self.manifest['voices']):
            self.assertEqual(select_reference(self.manifest,0,i)['id'],voice['id'])
    def test_single_cast(self):
        for cast,gender,unique in [(1,'F',5),(2,'M',3)]:
            selected=[select_reference(self.manifest,cast,i) for i in range(8)]
            self.assertTrue(all(v['corpus_gender']==gender for v in selected))
            self.assertEqual(len(set(v['id'] for v in selected)),unique)
    def test_split(self):
        for i in range(8):self.assertEqual(select_reference(self.manifest,3,i)['corpus_gender'],'F' if i%2==0 else 'M')
    def test_missing_metadata(self):
        with self.assertRaises(ValueError):select_reference({'voices':[{'id':'old'}]},1,0)

if __name__=='__main__':unittest.main()
