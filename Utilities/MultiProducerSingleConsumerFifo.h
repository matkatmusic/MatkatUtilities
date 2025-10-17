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

template<
    typename T,
    IsSorterType<T> SortFunc,
    size_t Capacity = 1'000,
    size_t ConsumerCapacity = Capacity * 4
>
struct MultiProducerSingleConsumerFifo
{
    ~MultiProducerSingleConsumerFifo()
    {
        timerRunner.halt();
        producers.clear();
        producerFifos.clear();
    }
    
    using ItemType = T;
    
    using ProducerFifoType = SimpleMBComp::Fifo<ItemType, Capacity>;
    using ConsumerFifoType = SimpleMBComp::Fifo<ItemType, Capacity * 4>;
    
    struct Producer
    {
        Producer(ProducerFifoType& fifo_) : fifo(fifo_) {}
        bool push(const ItemType& item)
        {
            return fifo.push(item);
        }
    private:
        ProducerFifoType& fifo;
    };
    
    Producer* addProducer()
    {
        juce::ScopedLock sl(creationLock);
        producerFifos.emplace_back(std::make_unique<ProducerFifoType>());
        producers.emplace_back(std::make_unique<Producer>(*producerFifos.back()));
        auto* producer = producers.back().get();
        return producer;
    }
    
    Producer* getProducer(size_t index)
    {
        juce::ScopedLock sl(creationLock);
        if (index < producers.size())
        {
            return producers[index].get();
        }
        
        return nullptr;
    }
    
    int getProducerIndex(Producer* producer)
    {
        juce::ScopedLock sl(creationLock);
        auto it = std::find_if(producers.begin(),
                               producers.end(),
                               [producer](const std::unique_ptr<Producer>& p)
                               {
            return p.get() == producer;
        });
        
        if (it != producers.end())
        {
            return static_cast<int>(std::distance(producers.begin(), it));
        }
        
        return -1; //not found
    }
    
    void removeProducer(Producer* producer)
    {
        juce::ScopedLock sl(creationLock);
        auto it = std::find_if(producers.begin(),
                               producers.end(),
                               [producer](const std::unique_ptr<Producer>& p)
                               {
            return p.get() == producer;
        });

        if (it != producers.end())
        {
            auto fifoIt = producerFifos.begin();
            std::advance(fifoIt, std::distance(producers.begin(), it));

            producerFifos.erase(fifoIt);
            producers.erase(it);
        }
    }
    
    void flushAllWithOptionalSort()
    {
        juce::ScopedLock sl(creationLock);
        std::vector<ItemType> itemsToPush = gatherLatestFromAllProducers();
        
        if( !itemsToPush.empty() )
        {
            std::sort(itemsToPush.begin(),
                      itemsToPush.end(),
                      SortFunc::compare);
            
            flushAll(itemsToPush);
        }
    }

    void flushAllToConsumerFifo()
    {
        juce::ScopedLock sl(creationLock);
        std::vector<ItemType> itemsToPush = gatherLatestFromAllProducers();
        
        if( !itemsToPush.empty() )
        {
            flushAll(itemsToPush);
        }
    }
    
    bool pull(ItemType& item)
    {
        return consumerFifo.pull(item);
    }
private:
    std::vector<std::unique_ptr<Producer>> producers;
    std::vector< std::unique_ptr<ProducerFifoType> > producerFifos;
    
    ConsumerFifoType consumerFifo;
    juce::CriticalSection creationLock;
    
    using ThisClass = MultiProducerSingleConsumerFifo;
    
    TimerRunner<ThisClass, 20> timerRunner
    {
        *this,
        &ThisClass::flushAllToConsumerFifo,
        TimerLaunchType::StartImmediately
    };
    
    std::vector<ItemType> gatherLatestFromAllProducers()
    {
        juce::ScopedLock sl(creationLock);
        
        std::vector<ItemType> latestItems;
        for( auto& fifo : producerFifos )
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
    
    void flushAll(std::vector<ItemType> itemsToFlush)
    {
        jassert(itemsToFlush.size() < consumerFifo.getFreeSpace() );
        
        for( const auto& item : itemsToFlush )
        {
            //continually try to push this element into the consumer fifo.
            //if this fails, the consumer fifo isn't being queried often enough
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

//template<typename T>
//struct TimedItemNoSort
//{
//    static bool compare(const TimedItem<T>& ,
//                        const TimedItem<T>& )
//    {
//        return true;
//    }
//};

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
