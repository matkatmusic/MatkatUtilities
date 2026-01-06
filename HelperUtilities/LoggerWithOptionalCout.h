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
    enum class LogOptions
    {
        LogToCout,
        DontLogToCout
    };
    
    LoggerWithOptionalCout(LogOptions includeWritingToCout, std::unique_ptr<juce::FileLogger> logger);
    ~LoggerWithOptionalCout();
    void logMessage(const juce::String&);
    const juce::File& getLogFile() const;
private:
    LogOptions writeToCout;
    std::unique_ptr<juce::FileLogger> fileLogger;
};
