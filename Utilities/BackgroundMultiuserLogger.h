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
#include "LoggerWithOptionalCout.h"


/**
 The `BackgroundMultiuserLogger` is a wrapper class around an instance of the `juce::FileLogger` class.
 The wrapper adds a `juce::Singleton` interface for multiple threads to write to the `juce::FileLogger`simultaneously, without data races, and with timestamps per message.
 The wrapper class achieves this by using a `MultiProducerSingleConsumerFifo<T>` for collecting messages, that owns `Producer` instances and their associated `Fifo<T>` instances.
 A `Key-Value` map is used to coordinate collection of messages and sending them to the correct Producer.
 The `Key` is the calling thread's `threadID`.
 The `Value` in the map is the index of the `Producer` in the `MPSCFifo`'s list of `Producer`s
 If the `Key` (`threadID`) doesn't exist in the map, a `Producer` object and `Fifo<T>` object are created in the `MPSCFifo` instance.
 The `message` is timestamped and added to that producer's fifo.
 A `TimerRunner` object periodically tells the `MPSCFifo` to retrieve all messages from each `Producer`'s fifo, sort them by their timestamp, and then pass then to the `MPSCFifo`'s `SingleConsumer` `Fifo<T>`.
 Then, all messages in the SingleConsumer fifo are passed to the `juce::FileLogger` instance and written to the log file.
 
 Helper functions include:
 - `printAllRemainingMessages()` which flushes the MSPCFifo to the FileLogger
 */

struct BackgroundMultiuserLogger
{
    BackgroundMultiuserLogger();
    
    ~BackgroundMultiuserLogger();
    
    enum class RevealOptions
    {
        RevealOnExit,
        DontRevealOnExit
    };
    
    enum class MessageTimestampOptions
    {
        Show,
        Hide
    };
    
    enum class MessageSortingOptions
    {
        SortedByTimestamp,
        Unsorted
    };
    
    void configure(LoggerWithOptionalCout::LogOptions alsoLogToCout, 
                   RevealOptions revealLogFileOnExit,
                   MessageTimestampOptions withTimestamp,
                   MessageSortingOptions sortedOrNot);
    
    static void writeToLog(const juce::String& message);
    
    static void printAllRemainingMessages();
    
    JUCE_DECLARE_SINGLETON(BackgroundMultiuserLogger, false)
private:
    RevealOptions revealOnExit = RevealOptions::DontRevealOnExit;
    MessageTimestampOptions withTS = MessageTimestampOptions::Hide;
    MessageSortingOptions sortedOrNot = MessageSortingOptions::SortedByTimestamp;
    bool isConfigured = false;
    std::unique_ptr<LoggerWithOptionalCout> fileLogger;
    std::unordered_map<juce::Thread::ThreadID, size_t> producerIndexes;
    std::unique_ptr<TimedItemMultiProducerSingleConsumerFifoDefaultSort<juce::String>> mpscFifo;
    
    juce::Atomic<double> lastMessageTimestamp = 0.0;
    
    void writeToLogInternal(const juce::String& message);
    
    void flushMessagesFromFifo();
    
    std::unique_ptr<TimerRunner<BackgroundMultiuserLogger, 25>> timerRunner;
    
    juce::String createMessageWithThreadName(juce::String str);
    void log(size_t producerIndex,
             double timestamp,
             juce::String str);
    
    double getMessageTimestamp();
    double updateLastTimestamp(double ts);
};

using BML = BackgroundMultiuserLogger;
