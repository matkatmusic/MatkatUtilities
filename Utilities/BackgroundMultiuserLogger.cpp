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
    messagePurger = std::make_unique<TimerRunner<BackgroundMultiuserLogger, 25>>(*this, &BackgroundMultiuserLogger::flushMessagesFromFifo, TimerLaunchType::StartWhenSignaled);
    messagePurger->launch();
};

BackgroundMultiuserLogger::~BackgroundMultiuserLogger()
{
    flushMessagesFromFifo();
    if( messagePurger )
        messagePurger->halt();
    
    messagePurger.reset();
    
    mpscFifo.reset();
    
    if( fileLogger && revealOnExit == RevealOptions::RevealOnExit )
        fileLogger->getLogFile().revealToUser();
    
    fileLogger.reset();
    clearSingletonInstance();
}

void BackgroundMultiuserLogger::configure(LoggerWithOptionalCout::LogOptions alsoLogToCout,
                                          RevealOptions revealLogFileOnExit,
                                          MessageTimestampOptions withTimestamp)
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
    
    isConfigured = true;
}

void BackgroundMultiuserLogger::writeToLog(const juce::String& message)
{
    auto* logger = BackgroundMultiuserLogger::getInstance();
    logger->writeToLogInternal(message);
}

//double BackgroundMultiuserLogger::getMessageTimestamp()
//{
//    auto ts = juce::Time::getMillisecondCounterHiRes();
//    return updateLastTimestamp(ts);
//}

//double BackgroundMultiuserLogger::updateLastTimestamp(double ts)
//{
//    auto lastTS = lastMessageTimestamp.get();
//    jassert( ts > lastTS ); //should never log two messages in the same millisecond
//    if( ts <= lastTS )
//    {
//        jassertfalse;
//        ts = lastTS + 0.0000001;
//    }
//    
//    lastMessageTimestamp = ts;
//    return ts;
//}

void BackgroundMultiuserLogger::writeToLogInternal(const juce::String& message)
{
    /*
     you must call BML::getInstance()->configure(...) before you can start using the logger!!
     */
    jassert(isConfigured);
    if( isConfigured == false )
        return;
    
//    auto timestamp = getMessageTimestamp();
    auto timestamp = juce::Time::getMillisecondCounterHiRes() - startTime;
    auto producerIterator = getOrCreateProducer();
    
    jassert(producerIterator != producerIndexes.end() );
    jassert(producerIterator->second != nullptr);
    
    auto str = createMessageWithThreadName(message, producerIterator);
    
    log(producerIterator->second->getIndex(), timestamp, str);
}

BackgroundMultiuserLogger::Map::iterator BackgroundMultiuserLogger::getOrCreateProducer()
{
    const juce::ScopedLock lock(indexesLock);
    
    if( isThisAJuceThread() )
    {
        //do we have a producer for this thread yet?
        auto it = getEntryInMapForCurrentThread();
        if( it != producerIndexes.end() )
        {
            return it;
        }
        
        /*
         from juce::Thread::getCurrentThread() docs:
         Note that the main UI thread (or other non-JUCE threads) don't have a Thread
         object associated with them, so this will return nullptr.
         
         it's ok if we pass in nullptr to createProducerIndexForCurrentThread though.
         */
        auto currentThread = juce::Thread::getCurrentThread();
        return createProducerForCurrentThread(currentThread);
    }
    
    /*
     it's some other thread type
     use a fallback with an invalid juce::ThreadID.
     if no producer exists for this other thread, make one.
     */
    auto fallbackID = juce::Thread::ThreadID { nullptr };
    
    //do we have a producer yet?
    auto fallbackIT = producerIndexes.find(fallbackID);
    if( fallbackIT == producerIndexes.end() )
    {
        auto newProducerIndex = mpscFifo->createProducer();
        
        return addProducerIndexEntry(fallbackID, newProducerIndex, nullptr);
    }
    
    return fallbackIT;
}

bool BackgroundMultiuserLogger::isThisAJuceThread()
{
    if( juce::MessageManager::existsAndIsCurrentThread() )
        return true;
    
    if( auto currentThread = juce::Thread::getCurrentThread() )
        return true;
    
    return false;
}
    
BackgroundMultiuserLogger::Map::iterator BackgroundMultiuserLogger::getEntryInMapForCurrentThread()
{
    auto currentThreadID = juce::Thread::getCurrentThreadId();
    return producerIndexes.find(currentThreadID);
}

BackgroundMultiuserLogger::Map::iterator BackgroundMultiuserLogger::addProducerIndexEntry(juce::Thread::ThreadID id,
                                                      size_t producerIndex,
                                                      juce::Thread *thread)
{
    auto [it, result] = producerIndexes.emplace(id, std::make_unique<ProducingThreadTracker>(producerIndex, thread));
    jassert( result != false );
    juce::ignoreUnused(result);
    return it;
}

BackgroundMultiuserLogger::Map::iterator BackgroundMultiuserLogger::createProducerForCurrentThread(juce::Thread* thread)
{
    return addProducerIndexEntry(juce::Thread::getCurrentThreadId(),
                                 mpscFifo->createProducer(),
                                 thread);
}

juce::String BackgroundMultiuserLogger::createMessageWithThreadName(juce::String message, iterator it)
{
    juce::String str;
    str << "[";
    
    if( it != producerIndexes.end() && it->second != nullptr )
    {
        str << it->second->getName();
    }
    else
    {
        str << "unknown threadName";
    }
    
    str << "]: ";
    
    str << message;
    
    return str;
}

void BackgroundMultiuserLogger::log(size_t producerIndex,
                                    double timestamp,
                                    juce::String str)
{
    jassert(mpscFifo != nullptr && isConfigured );
    
    auto logResult = mpscFifo->add({timestamp, str}, producerIndex);
    jassert(logResult == true); //if this fails, the ProducerCapacity parameter of the MPSCFifo is too small.
    juce::ignoreUnused(logResult);
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
        mpscFifo->flushAllToConsumerFifo();
        
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
}

JUCE_IMPLEMENT_SINGLETON (BackgroundMultiuserLogger)
