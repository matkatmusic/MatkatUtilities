/*
  ==============================================================================

    FifoHelpers.h
    Created: 5 Jan 2026 4:40:49pm
    Author:  Matkat Music LLC

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "../MiscUtilities/Concepts.h"

template<typename FifoType>
requires( HasType<FifoType> )
static std::vector<typename FifoType::Type> retrieveLatestFrom(FifoType& fifo)
{
    auto numAvailable = fifo.getNumAvailableForReading();
    if( numAvailable == 0 )
        return {};
    
    using T = FifoType::Type;
    
    std::vector<T> elements;
    elements.reserve(numAvailable);
    T element;
    while( fifo.pull(element) )
    {
        elements.push_back(element);
    }
    return elements;
}

template<IsFifoType FifoType>
requires( HasGetID<typename FifoType::Type> )
static std::vector<typename FifoType::Type> retrieveLatestFrom(FifoType& fifo)
{
    auto numAvailable = fifo.getNumAvailableForReading();
    if( numAvailable == 0 )
        return {};
    
    using T = FifoType::Type;
    
    std::vector<T> elements;
    elements.reserve(numAvailable);
    T element;
    while( fifo.getNext(element) )
    {
        elements.push_back(element);
    }
    return elements;
}
