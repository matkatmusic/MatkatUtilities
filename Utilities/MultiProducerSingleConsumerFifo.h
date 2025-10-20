/*
  ==============================================================================

    SourceCode.h
    Created: 17 Aug 2025 9:00:04pm
    Author:  Matkat Music LLC

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <Fifo.h>
#include "Concepts.h"
#include "TimerRunner.h"

/**
 A Multi-Producer Single-Consumer fifo.
 
 It manages the producer objects and consumes elements from each producer using a timed interval of 20ms.
 
 Usage:
 - First, from your calling thread, create a producer and store the index of that producer.
 - Then, Whenever you need to push from the calling thread, use the index given to you.
 
 ex:
 @code
    
 struct MyBackgroundThreadClass : juce::Thread
 {
     MyBackgroundThreadClass(MPSCFifo& mpsc) :
     juce::Thread("MyBackgroundThreadClass"),
     mpscfifo(mpsc)
     {
         index = mpscfifo.createProducer();
         startThread();
     }
     
     ~MyBackgroundThreadClass() override;
     {
         mpscfifo.removeProducer(index);
         stopThread(100);
     }
     
     void run() override
     {
         while( threadShouldExit() == false )
         {
             //add some data to the MPSC using your index from your background thread
             mpscfifo.add(data, index);
         }
     }
     
     MPSCFifo& mpscfifo;
     size_t index;
 };
 
 @endcode
 */



template<typename ItemType>
struct DefaultNonSorter
{
    static bool compare(const ItemType& a, const ItemType& b)
    {
        return true;
    }
};

template<
    typename T,
    IsSorterType<T> SortFunc = DefaultNonSorter<T>,
    size_t ProducerCapacity = 1'000,
    size_t ConsumerCapacity = ProducerCapacity * 8
>
struct MultiProducerSingleConsumerFifo
{
    using ItemType = T;
    
    using ProducerFifoType = SimpleMBComp::Fifo<ItemType, ProducerCapacity>;
    using ConsumerFifoType = SimpleMBComp::Fifo<ItemType, ConsumerCapacity>;
    
    ~MultiProducerSingleConsumerFifo()
    {
        timerRunner.halt();
        producers.clear();
    }
    
    size_t createProducer()
    {
        juce::ScopedLock sl(producersLock);
        
        producers.push_back( std::make_unique<ProducerFifoType>() );
        return producers.size() - 1;
    }
    
    bool removeProducer(size_t index)
    {
        juce::ScopedLock sl(producersLock);
        if( index < producers.size() )
        {
            if( producers[index] != nullptr)
            {
                flushAllToConsumerFifo(); //drain it before deleting
                
                auto it = producers.begin();
                std::advance(it, index);
                producers.erase(it);
                
                return true;
            }
        }
        
        return false;
    }
    
    bool add(const ItemType& element, size_t index)
    {
        juce::ScopedLock stl(producersLock);
        if( index < producers.size() )
        {
            if( auto p = producers[index].get() )
            {
                return p->push(element);
            }
        }
        
        //if this happens, the producer fifo doesn't exist!
        //call 'createProducer()' first, then add to it.
        return false;
    }
    
    bool pull(ItemType& item)
    {
        return consumerFifo.pull(item);
    }
    
    void flushAllToConsumerFifo()
    {
        auto itemsToPush = gatherLatestFromAllProducers();
        if( itemsToPush.empty() )
        {
            return;
        }
        
        //if sortFunc is not defaultNonSorter, skip calling std::sort()
        if constexpr( std::is_same_v<SortFunc, DefaultNonSorter<ItemType>> == false )
        {
            std::sort(itemsToPush.begin(),
                      itemsToPush.end(),
                      SortFunc::compare);
        }
        
        flushAll(itemsToPush);
    }
private:
    juce::CriticalSection producersLock;
    
    std::vector< std::unique_ptr<ProducerFifoType> > producers;
    ConsumerFifoType consumerFifo;
    
    using ThisClass = MultiProducerSingleConsumerFifo;
    TimerRunner<ThisClass, 20> timerRunner
    {
        *this,
        &ThisClass::flushAllToConsumerFifo,
        TimerLaunchType::StartImmediately
    };
    
    std::vector<ItemType> gatherLatestFromAllProducers()
    {
        juce::ScopedLock sl(producersLock);
        
        std::vector<ItemType> latestItems;
        for( auto& fifo : producers )
        {
            if( fifo != nullptr )
            {
                ItemType item;
                while( fifo->pull(item) )
                {
                    latestItems.push_back(item);
                }
            }
        }
        
        return latestItems;
    }
    
    void flushAll(const std::vector<ItemType>& itemsToFlush)
    {
        jassert(itemsToFlush.size() < consumerFifo.getFreeSpace() );
        
        for( const auto& item : itemsToFlush )
        {
            //continually try to push this element into the consumer fifo.
            //if this fails, the consumer fifo isn't being emptied often enough
            auto result = false;
            do
            {
                result = consumerFifo.push(item);
            }
            while( result == false );
        }
    }
};

template<typename T>
struct TimedItem
{
    double timeOfCreation;
    T item;
};

template<typename T>
struct TimedItemSort
{
    static bool compare(const TimedItem<T>& a,
                        const TimedItem<T>& b)
    {
        return a.timeOfCreation < b.timeOfCreation;
    }
};

template<
    typename T,
    size_t Capacity = 1'000,
    size_t ConsumerCapacity = Capacity * 4
>
using TimedItemMultiProducerSingleConsumerFifoDefaultSort = 
MultiProducerSingleConsumerFifo<
    TimedItem<T>,
    TimedItemSort<T>, 
    Capacity,
    ConsumerCapacity
>;

//template<
//    typename T,
//    size_t Capacity = 1'000,
//    size_t ConsumerCapacity = Capacity * 4
//>
//using TimedItemMultiProducerSingleConsumerFifoNoSort =
//MultiProducerSingleConsumerFifo<
//    TimedItem<T>,
//    TimedItemNoSort<T>,
//    Capacity,
//    ConsumerCapacity
//>;
