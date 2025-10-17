/*
  ==============================================================================

    LoggerWithOptionalCout.h
    Created: 16 Jul 2025 2:05:20pm
    Author:  Matkat Music LLC

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct LoggerWithOptionalCout
{
    LoggerWithOptionalCout(bool includeWritingToCout, std::unique_ptr<juce::FileLogger> logger);
    ~LoggerWithOptionalCout();
    void logMessage(const juce::String&);
private:
    bool writeToCout;
    std::unique_ptr<juce::FileLogger> fileLogger;
};
