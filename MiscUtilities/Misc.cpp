/*
  ==============================================================================

    Misc.cpp
    Created: 5 Jan 2026 4:45:16pm
    Author:  Matkat Music LLC

  ==============================================================================
*/

#include "Misc.h"

juce::File getAppDirectory()
{
    auto userAppDataDir = juce::File::getSpecialLocation(juce::File::SpecialLocationType::userApplicationDataDirectory);
    
    auto appDirPath = userAppDataDir.getChildFile(ProjectInfo::projectName);
    if( appDirPath.exists() == false  )
    {
        appDirPath.createDirectory();
    }
    
    return appDirPath;
}
