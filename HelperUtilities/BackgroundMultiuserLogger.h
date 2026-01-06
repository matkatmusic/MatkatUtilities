/*
  ==============================================================================

    BackgroundMultiuserLogger.h
    Created: 17 Aug 2025 9:00:43pm
    Author:  Matkat Music LLC

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../ThreadUtilities/MultiProducerSingleConsumerFifo.h"

#include "../ThreadUtilities/TimerRunner.h"
#include "LoggerWithOptionalCout.h"


/**
 The `BackgroundMultiuserLogger` is a wrapper class around an instance of the `juce::FileLogger` class.
 
 The wrapper adds a `juce::Singleton` interface for multiple threads to write to the `juce::FileLogger`simultaneously, without data races, and with timestamps per message.
 The wrapper class achieves this by using a `MultiProducerSingleConsumerFifo<T>` for collecting messages, that owns `Producer Fifo<T>` instances.
 
 A `Key-Value` `unordered_map` is used to coordinate collection of messages and sending them to the correct Producer fifo.
 The `Key` is the calling thread's `threadID`.
 The `Value` in the map is the index of the `Producer` in the `MPSCFifo`'s list of `Producer Fifo<T>`s
 
 If the `Key` (`threadID`) doesn't exist in the map, a `Producer` is automatically created.
 when you call `writeToLog(message)`, the `message` is timestamped and added to that Producer's fifo.
 
 A `TimerRunner` object periodically tells the `MPSCFifo` to retrieve all messages from each `Producer Fifo<T>`, sort them by their timestamp, and then pass then to the `MPSCFifo`'s `SingleConsumer` `Fifo<T>`.
 Then, all messages in the SingleConsumer fifo are passed to the `juce::FileLogger` instance and written to the log file.
 
 Be sure to call `configure()` before you start logging messages!
 
 Helper functions:
 - `printAllRemainingMessages()` which flushes the `MSPCFifo` to the FileLogger
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
    
    void configure(LoggerWithOptionalCout::LogOptions alsoLogToCout, 
                   RevealOptions revealLogFileOnExit,
                   MessageTimestampOptions withTimestamp);
    
    static void writeToLog(const juce::String& message);
    
    static void printAllRemainingMessages();
    
    JUCE_DECLARE_SINGLETON(BackgroundMultiuserLogger, false)
private:
    RevealOptions revealOnExit = RevealOptions::DontRevealOnExit;
    MessageTimestampOptions withTS = MessageTimestampOptions::Hide;
    bool isConfigured = false;
    std::unique_ptr<LoggerWithOptionalCout> fileLogger;
    
    juce::CriticalSection indexesLock;
    
    struct ProducingThreadDetails
    {
        ProducingThreadDetails(size_t index_, juce::Thread* thread) : index(index_)
        {
            if( thread )
            {
                threadName = thread->getThreadName();
            }
            else if( juce::MessageManager::existsAndIsCurrentThread() )
            {
                threadName = "juce::MessageThread";
            }
            else
            {
                threadName = "Anonymous Thread";
            }
        }
        
        size_t getIndex() const { return index; }
        juce::String getName() const { return threadName; }
    private:
        size_t index;
        juce::String threadName;
    };
    
    using Map = std::unordered_map<juce::Thread::ThreadID, std::unique_ptr<ProducingThreadDetails>>;
    Map producerIndexes;
    
    using iterator = Map::iterator;
    
    static constexpr int MessageQueueSize = 10'000;
    using TimedMPSCFifo = TimedItemMultiProducerSingleConsumerFifoDefaultSort<juce::String, MessageQueueSize>;
    std::unique_ptr<TimedMPSCFifo> mpscFifo;
    
    std::unique_ptr<TimerRunner<BackgroundMultiuserLogger, 25>> messagePurger;
    
    void writeToLogInternal(const juce::String& message);
    
    void flushMessagesFromFifo();
    
    juce::String createMessageWithThreadName(juce::String str, iterator producerIterator);
    void log(size_t producerIndex,
             double timestamp,
             juce::String str);
    
    const double startTime = juce::Time::getMillisecondCounterHiRes();
    
    iterator getOrCreateProducer();
    iterator createProducerForCurrentThread(juce::Thread* thread);
    iterator getEntryInMapForCurrentThread();
    
    static bool isThisAJuceThread();
    iterator addProducerIndexEntry(juce::Thread::ThreadID id,
                               size_t producerIndex,
                               juce::Thread* thread);
};

using BML = BackgroundMultiuserLogger;
