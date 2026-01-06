/*
  ==============================================================================

    LoggerWithOptionalCout.cpp
    Created: 16 Jul 2025 2:05:20pm
    Author:  Matkat Music LLC

  ==============================================================================
*/

#include "LoggerWithOptionalCout.h"

LoggerWithOptionalCout::LoggerWithOptionalCout(LoggerWithOptionalCout::LogOptions b, std::unique_ptr<juce::FileLogger> logger ) : writeToCout(b)
{
    fileLogger = std::move(logger);
    juce::Logger::setCurrentLogger(fileLogger.get());
}

LoggerWithOptionalCout::~LoggerWithOptionalCout()
{
    juce::Logger::setCurrentLogger(nullptr);
}

const juce::File& LoggerWithOptionalCout::getLogFile() const
{
    return fileLogger->getLogFile();
}

void LoggerWithOptionalCout::logMessage(const juce::String& message)
{
    if (writeToCout == LogOptions::LogToCout)
    {
        std::cout << message << std::endl;
    }
    
    if (fileLogger)
    {
        fileLogger->logMessage(message);
    }
}
