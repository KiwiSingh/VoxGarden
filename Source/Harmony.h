#pragma once
#include <array>
#include <algorithm>
#include <cmath>
#include <vector>
#include <string>
namespace vg {
inline constexpr int voices = 24;
inline constexpr std::array<const char*, 12> roots {"C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B"};
inline constexpr std::array<const char*, 9> scales {"Major", "Natural minor", "Harmonic minor", "Melodic minor", "Dorian", "Mixolydian", "Major pentatonic", "Minor pentatonic", "Chromatic"};
inline const std::array<std::vector<int>, 15> intervals {{{0,2,4,5,7,9,11}, {0,2,3,5,7,8,10}, {0,2,3,5,7,8,11}, {0,2,3,5,7,9,11}, {0,2,3,5,7,9,10}, {0,2,4,5,7,9,10}, {0,2,4,7,9}, {0,3,5,7,10}, {0,1,2,3,4,5,6,7,8,9,10,11}, {0,2,4,6,7,9,11}, {0,1,3,5,7,8,10}, {0,1,4,5,7,8,11}, {0,3,5,8,10}, {0,2,5,7,9}, {0,2,4,7,11}}};
inline constexpr std::array<const char*,8> ragaNames{"Yaman","Bhupali","Kafi","Bhairavi","Bhairav","Malkauns","Durga","Hamsadhwani"};
inline constexpr std::array<const char*,8> ragaWestern{"Lydian","Major pentatonic","Dorian","Phrygian","Double harmonic major","Minor pentatonic b6","Suspended pentatonic","Major pentatonic 7"};
inline constexpr std::array<int,8> ragaScales{9,6,4,10,11,12,13,14};
inline bool inScale(int note, int root, int scale) {
    auto pc = ((note-root)%12+12)%12;
    const auto& s = intervals[(size_t)std::clamp(scale,0,(int)intervals.size()-1)];
    return std::find(s.begin(),s.end(),pc)!=s.end();
}
inline int nearest(float note,int root,int scale) {
    int best = (int)std::round(note); float d = 1000;
    for (int n=(int)std::floor(note)-12;n<=(int)std::ceil(note)+12;++n)
        if (inScale(n,root,scale) && std::abs(n-note)<d) {d=std::abs(n-note);best=n;}
    return best;
}
inline int degreeNote(float source,int degree,int root,int scale) {
    int n=nearest(source,root,scale), step=degree<0?-1:1;
    for(int k=0;k<std::abs(degree);++k) {do {n+=step;} while(!inScale(n,root,scale));}
    return n;
}
// Display only: spell notes by scale degree rather than mixing enharmonic aliases.
inline std::vector<std::string> ragaNoteNames(int root,int raga){
    root=std::clamp(root,0,11);raga=std::clamp(raga,1,8)-1;
    static const std::array<std::vector<int>,8> degrees{{{0,1,2,3,4,5,6},{0,1,2,4,5},{0,1,2,3,4,5,6},{0,1,2,3,4,5,6},{0,1,2,3,4,5,6},{0,2,3,5,6},{0,1,3,4,5},{0,1,2,4,6}}};
    constexpr char letters[]="CDEFGAB";constexpr int natural[7]={0,2,4,5,7,9,11};int first=0;while(letters[first]!=roots[(size_t)root][0])++first;
    const auto& notes=intervals[(size_t)ragaScales[(size_t)raga]];std::vector<std::string> result;
    for(size_t i=0;i<notes.size();++i){int letter=(first+degrees[(size_t)raga][i])%7;int difference=(root+notes[i]-natural[letter]+18)%12-6;std::string text(1,letters[letter]);
        while(difference>0){text+="#";--difference;}while(difference<0){text+="b";++difference;}result.push_back(text);}
    return result;
}

}
