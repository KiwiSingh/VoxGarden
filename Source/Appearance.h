#pragma once
#include <juce_core/juce_core.h>
namespace vg {
struct Appearance { int accent=0;bool dark=true; };
inline juce::File appearancePreferencesFile(){return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("Application Support/VoxGarden/Appearance.json");}
inline Appearance loadAppearance(const juce::File& file){
    Appearance result;if(!file.existsAsFile()||file.getSize()>4096)return result;
    auto data=juce::JSON::parse(file);if(!data.isObject())return result;
    auto accent=data.getProperty("accent",0);if(accent.isInt())result.accent=juce::jlimit(0,3,(int)accent);
    auto dark=data.getProperty("dark",true);if(dark.isBool())result.dark=(bool)dark;return result;
}
inline bool saveAppearance(const juce::File& file,Appearance value){
    if(!file.getParentDirectory().createDirectory())return false;
    juce::DynamicObject::Ptr data=new juce::DynamicObject;data->setProperty("accent",juce::jlimit(0,3,value.accent));data->setProperty("dark",value.dark);
    juce::TemporaryFile temporary(file);return temporary.getFile().replaceWithText(juce::JSON::toString(juce::var(data.get())))&&temporary.overwriteTargetFileWithTemporary();
}
}
