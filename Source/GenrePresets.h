#pragma once
#include "Engine.h"
namespace vg {
struct GenrePreset {const char* name;const char* family;int count;std::array<int,4> degrees;float room,detune,timing,vibrato,colour;};
inline constexpr std::array<GenrePreset,12> genrePresets{{
    {"Bollywood Intimate Backing","Bollywood",6,{0,0,2,-7},.18f,2,10,1.5f,.3f},
    {"Bollywood Cinematic Chorus","Bollywood",24,{-7,0,2,4},.65f,4,22,3,.4f},
    {"Bollywood Raga Backing","Bollywood",12,{0,0,7,-7},.35f,2,18,2,.2f},
    {"Italodisco Octave Lights","Italodisco",8,{0,0,7,-7},.18f,6,7,1.5f,1.2f},
    {"Italodisco Silver Choir","Italodisco",12,{0,2,4,7},.32f,8,10,2.5f,1.8f},
    {"Italodisco Midnight Fifths","Italodisco",16,{0,4,7,-7},.42f,7,12,3,1.f},
    {"Disco Polo Tight Doubles","Disco polo",6,{0,0,0,0},.12f,3,5,1,.5f},
    {"Disco Polo Chorus Lift","Disco polo",12,{0,2,0,7},.22f,4,8,1.8f,.7f},
    {"Disco Polo Festival Stack","Disco polo",16,{-7,0,2,4},.3f,5,10,2,.8f},
    {"Classical Chamber Voices","Classical",8,{-7,-3,0,2},.35f,1.5f,18,2,0},
    {"Classical SATB Ensemble","Classical",16,{-7,-3,0,2},.52f,2,22,2.5f,0},
    {"Classical Sacred Hall","Classical",24,{-7,0,2,7},.82f,2.5f,26,3,.1f}
}};
}
