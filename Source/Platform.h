#pragma once
#include <juce_core/juce_core.h>
namespace vg {
#if JUCE_WINDOWS
inline constexpr bool windowsBuild=true;
#else
inline constexpr bool windowsBuild=false;
#endif
inline juce::File dataRoot(){auto root=juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
#if JUCE_MAC
return root.getChildFile("Application Support/VoxGarden");
#else
return root.getChildFile("VoxGarden");
#endif
}
inline juce::File runtimePython(const juce::File& root){
#if JUCE_WINDOWS
return root.getChildFile("python/Scripts/python.exe");
#else
return root.getChildFile("python/bin/python");
#endif
}
}
