/*
  ==============================================================================

    BackgroundMultiuserLogger.cpp
    Created: 17 Aug 2025 9:00:43pm
    Author:  Matkat Music LLC

  ==============================================================================
*/

#include "BackgroundMultiuserLogger.h"


BackgroundMultiuserLogger::BackgroundMultiuserLogger()
{
    mpscFifo = std::make_unique<TimedItemMultiProducerSingleConsumerFifoDefaultSort<juce::String>>();
    timerRunner = std::make_unique<TimerRunner<BackgroundMultiuserLogger, 25>>(*this, &BackgroundMultiuserLogger::flushMessagesFromFifo, TimerLaunchType::StartWhenSignaled);
    timerRunner->launch();
};

BackgroundMultiuserLogger::~BackgroundMultiuserLogger()
{
    flushMessagesFromFifo();
    if( timerRunner )
        timerRunner->halt();
    
    timerRunner.reset();
    
    mpscFifo.reset();
    
    if( fileLogger && revealOnExit == RevealOptions::RevealOnExit )
        fileLogger->getLogFile().revealToUser();
    
    fileLogger.reset();
    clearSingletonInstance();
}

void BackgroundMultiuserLogger::configure(LoggerWithOptionalCout::LogOptions alsoLogToCout,
                                          RevealOptions revealLogFileOnExit,
                                          MessageTimestampOptions withTimestamp,
                                          MessageSortingOptions sortedOrNot_)
{
    auto welcomeMessage = juce::String("Welcome to ") + ProjectInfo::projectName;
    welcomeMessage << " ";
    welcomeMessage << ProjectInfo::versionString;
    welcomeMessage << " spawned at ";
    welcomeMessage << juce::Time::getCurrentTime().toISO8601(true);
    
    auto logger = std::unique_ptr<juce::FileLogger>(juce::FileLogger::createDateStampedLogger(ProjectInfo::projectName, "session", ".log", welcomeMessage));
    fileLogger = std::make_unique<LoggerWithOptionalCout>(alsoLogToCout, std::move(logger));
    
    revealOnExit = revealLogFileOnExit;
    withTS = withTimestamp;
    sortedOrNot = sortedOrNot_;
    
    isConfigured = true;
}

void BackgroundMultiuserLogger::writeToLog(const juce::String& message)
{
    auto* logger = BackgroundMultiuserLogger::getInstance();
    logger->writeToLogInternal(message);
}

double BackgroundMultiuserLogger::getMessageTimestamp()
{
    auto ts = juce::Time::getMillisecondCounterHiRes();
    return updateLastTimestamp(ts);
}

double BackgroundMultiuserLogger::updateLastTimestamp(double ts)
{
    auto lastTS = lastMessageTimestamp.get();
    jassert( ts > lastTS ); //should never log two messages in the same millisecond
    if( ts <= lastTS )
    {
        jassertfalse;
        ts = lastTS + 0.0000001;
    }
    
    lastMessageTimestamp = ts;
    return ts;
}

void BackgroundMultiuserLogger::writeToLogInternal(const juce::String& message)
{
    auto timestamp = getMessageTimestamp();
    
    /*
     you must call BML::getInstance()->configure(...) before you can start using the logger.
     */
    jassert(isConfigured);
    if( isConfigured == false )
        return;
    
    auto str = createMessageWithThreadName(message);
    
    auto threadID = juce::Thread::getCurrentThreadId();
    auto it = producerIndexes.find(threadID);
    if (it == producerIndexes.end())
    {
        auto newProducer = mpscFifo->addProducer();
        auto producerIndex = mpscFifo->getProducerIndex(newProducer);
        producerIndexes[threadID] = producerIndex;
//        writeToLogInternal(message); //whoops: this was adding the timestamp to the message twice!
        log(producerIndex, timestamp, str);
    }
    else
    {
        log(it->second, timestamp, str);
    }
}

juce::String BackgroundMultiuserLogger::createMessageWithThreadName(juce::String message)
{
    const auto threadName = []() -> juce::String
    {
        if( juce::MessageManager::existsAndIsCurrentThread() )
        {
            return "juce::MessageThread";
        }
        
        if( auto currentThread = juce::Thread::getCurrentThread() )
            return currentThread->getThreadName();
        
        return "Anonymous Thread";
    }();
    
    juce::String str;
    str << "[" << threadName << "]: ";
    str << message;
    
    return str;
}

void BackgroundMultiuserLogger::log(size_t producerIndex,
                                    double timestamp,
                                    juce::String str)
{
    if( auto p = mpscFifo->getProducer(producerIndex) )
    {
        p->push({timestamp, str});
    }
    else
    {
        jassertfalse; //should never happen
    }
}

void BackgroundMultiuserLogger::printAllRemainingMessages()
{
    if( auto instance = getInstance() )
        instance->flushMessagesFromFifo();
}

void BackgroundMultiuserLogger::flushMessagesFromFifo()
{
    if( mpscFifo)
    {
        if( sortedOrNot == MessageSortingOptions::SortedByTimestamp )
        {
            mpscFifo->flushAllWithOptionalSort();
        }
        else
        {
            mpscFifo->flushAllToConsumerFifo();
        }
        
        decltype(mpscFifo)::element_type::ItemType message;
        while (mpscFifo->pull(message))
        {
            juce::String str;
            
            if( withTS == MessageTimestampOptions::Show )
            {
                str << juce::String::formatted("%f", message.timeOfCreation) << ": ";
                //            str << message.timeOfCreation << ": ";
            }
            
            str << message.item;
            juce::Logger::writeToLog(str);
        }
    }
    else
    {
        jassertfalse; //should never happen
    }
    
    //ideally check if the thread is still running here, and if not, remove it from the producerIndexes and mpscFifo
}

JUCE_IMPLEMENT_SINGLETON (BackgroundMultiuserLogger)
