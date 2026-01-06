/*
  ==============================================================================

    SineOscillator.cpp
    Created: 21 Jul 2025 4:35:48pm
    Author:  Charles Schiermeyer

  ==============================================================================
*/

#include "SineOscillator.h"
#include <JuceHeader.h>
#include <cmath>

SineOscillator::SineOscillator(double sampleRate)
: sampleRate(sampleRate)
{
    updatePhaseIncrement();
}

void SineOscillator::setFrequency(double newFrequency)
{
    frequency = newFrequency;
    updatePhaseIncrement();
}

float SineOscillator::getNextSample()
{
    float sample = std::sin(currentPhase);
    currentPhase += phaseIncrement;
    
    // Wrap phase to [0, 2π)
    if (currentPhase >= juce::MathConstants<float>::twoPi)
        currentPhase -= juce::MathConstants<float>::twoPi;
    
    return sample;
}

void SineOscillator::updatePhaseIncrement()
{
    const auto twoPi = juce::MathConstants<float>::twoPi;
    phaseIncrement = twoPi * frequency / sampleRate;
}
