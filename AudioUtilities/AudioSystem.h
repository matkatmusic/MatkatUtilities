/*
  ==============================================================================

    AudioSystem.h
    Created: 5 Jan 2026 4:40:35pm
    Author:  Matkat Music LLC

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace AudioUtilities
{

void printAudioSystemInfo();
void printAudioSystemDevices();
juce::String printDeviceInfo(juce::AudioIODevice& device);

} //end namespace AudioUtilities
