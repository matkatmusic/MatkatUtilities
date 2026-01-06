/*
  ==============================================================================

    BufferHelpers.cpp
    Created: 5 Jan 2026 4:46:38pm
    Author:  Matkat Music LLC

  ==============================================================================
*/

#include "BufferHelpers.h"

namespace AudioUtilities
{
std::vector<juce::AudioBuffer<float>> splitChannelInBufferIntoSmallerBuffers(int channelToSplit,
                                                                             const juce::AudioBuffer<float>& bufferToSplit,
                                                                             int numSamplesPerChunk)
{
    std::vector<juce::AudioBuffer<float>> buffers;
    
    jassert( juce::isPositiveAndBelow(channelToSplit, bufferToSplit.getNumChannels()));
    int numSamplesRemaining = bufferToSplit.getNumSamples();
    
    int sampleToStartAt = 0;
    while( numSamplesRemaining > 0 )
    {
        auto numSamplesThisRound = juce::jmin(numSamplesPerChunk, numSamplesRemaining);
        
        auto audioBuffer = juce::AudioBuffer<float>(1, numSamplesThisRound);
        
        audioBuffer.copyFrom(0,
                             0,
                             bufferToSplit,
                             channelToSplit,
                             sampleToStartAt,
                             numSamplesThisRound);
        
        buffers.push_back(audioBuffer);
        
        numSamplesRemaining -= numSamplesThisRound;
        sampleToStartAt += numSamplesThisRound;
    }
    
    return buffers;
}

}//end namespace UDPHelpers
