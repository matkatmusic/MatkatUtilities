/*
  ==============================================================================

    BackgroundMultiuserLogger.h
    Created: 17 Aug 2025 9:00:43pm
    Author:  Matkat Music LLC

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MultiProducerSingleConsumerFifo.h"

#include "TimerRunner.h"

struct BackgroundMultiuserLogger
{
    BackgroundMultiuserLogger();
    
    ~BackgroundMultiuserLogger();
    
    static void writeToLog(const juce::String& message);
    
    static void printAllRemainingMessages();
    
    JUCE_DECLARE_SINGLETON(BackgroundMultiuserLogger, false)
private:
    std::map<juce::Thread::ThreadID, size_t> producerIndexes;
    std::unique_ptr<TimedItemMultiProducerSingleConsumerFifoDefaultSort<juce::String>> mpscFifo;
    
    //double lastMessageTimestamp = 0.0;
    juce::Atomic<double> lastMessageTimestamp = 0.0;
    
    void writeToLogInternal(const juce::String& message);
    
    void flushMessagesFromFifo();
    
    std::unique_ptr<TimerRunner<BackgroundMultiuserLogger, 25>> timerRunner; // { *this, 25, &BackgroundMultiuserLogger::flushMessagesFromFifo, TimerLaunchType::StartWhenSignaled };
    
};

using BML = BackgroundMultiuserLogger;
