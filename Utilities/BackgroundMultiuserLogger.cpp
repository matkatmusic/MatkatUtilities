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
    clearSingletonInstance();
}

void BackgroundMultiuserLogger::writeToLog(const juce::String& message)
{
    auto* logger = BackgroundMultiuserLogger::getInstance();
    logger->writeToLogInternal(message);
}

void BackgroundMultiuserLogger::writeToLogInternal(const juce::String& message)
{
    //Moved timestamp calculation to here so it's exactly when the function is called.
    auto timestamp = juce::Time::getMillisecondCounterHiRes();
    
    auto lastTS = lastMessageTimestamp.get();
    jassert( timestamp > lastTS ); //should never log two messages in the same millisecond
    if( timestamp <= lastTS )
    {
        jassertfalse;
        timestamp = lastTS + 0.0000001;
    }
    
    lastMessageTimestamp = timestamp;
    
    auto threadID = juce::Thread::getCurrentThreadId();
    auto it = producerIndexes.find(threadID);
    if (it == producerIndexes.end())
    {
        auto newProducer = mpscFifo->addProducer();
        auto producerIndex = mpscFifo->getProducerIndex(newProducer);
        producerIndexes[threadID] = producerIndex;
        writeToLogInternal(message);
    }
    else
    {
        const auto threadName = []() -> juce::String
        {
            if( juce::MessageManager::existsAndIsCurrentThread() )
            {
                return "Message Thread";
            }
            
            auto * currentThread = juce::Thread::getCurrentThread();
            if(currentThread != nullptr )
                return currentThread->getThreadName();
            
//            jassertfalse;
//            return "No Thread Name";
            return "Anonymous";
        }();
        
        juce::String str;
        str << "thread [" << threadName << "]: ";
        str << message;
//#if JUCE_DEBUG
#if false
        DBG( str );
        juce::ignoreUnused();
#else
        if( auto p = mpscFifo->getProducer(it->second) )
        {
            /*
            auto timestamp = juce::Time::getMillisecondCounterHiRes();
            
            jassert( timestamp > lastMessageTimestamp ); //should never log two messages in the same millisecond
            if( timestamp <= lastMessageTimestamp )
            {
                jassertfalse;
                timestamp = lastMessageTimestamp + 0.0000001;
            }
            
            lastMessageTimestamp = timestamp;
             */
            p->push({timestamp, str});
        }
        else
        {
            jassertfalse; //should never happen
        }
#endif
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
        mpscFifo->flushAllWithOptionalSort();
        decltype(mpscFifo)::element_type::ItemType message;
        while (mpscFifo->pull(message))
        {
            juce::String str;
            str << juce::String::formatted("%f", message.timeOfCreation) << ": ";
//            str << message.timeOfCreation << ": ";
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
