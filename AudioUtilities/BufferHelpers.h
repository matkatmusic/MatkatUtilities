/*
  ==============================================================================

    BufferHelpers.h
    Created: 5 Jan 2026 4:46:38pm
    Author:  Matkat Music LLC

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace AudioUtilities
{
std::vector<juce::AudioBuffer<float>> splitChannelInBufferIntoSmallerBuffers(int channelToSplit,
                                                                             const juce::AudioBuffer<float>& bufferToSplit,
                                                                             int numSamplesPerChunk);
} //end namespace AudioUtilities
