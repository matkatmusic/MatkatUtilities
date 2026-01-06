/*
  ==============================================================================

    SineOscillator.h
    Created: 21 Jul 2025 4:35:48pm
    Author:  Charles Schiermeyer

  ==============================================================================
*/

#pragma once

class SineOscillator
{
public:
    SineOscillator(double sampleRate);

    void setFrequency(double newFrequency);

    float getNextSample();

private:
    double frequency = 440.0; // Default: A4
    double sampleRate = 44100.0;
    double currentPhase = 0.0;
    double phaseIncrement = 0.0;

    void updatePhaseIncrement();
};
