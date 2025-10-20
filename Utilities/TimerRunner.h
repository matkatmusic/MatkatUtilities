/*
  ==============================================================================

    TimerRunner.h
    Created: 29 Aug 2025 2:46:04pm
    Author:  Matkat Music LLC

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Concepts.h"

enum class TimerLaunchType
{
    StartImmediately,
    StartWhenSignaled
};

template<TimerHandler Owner, int intervalInMS>
struct TimerRunner : private juce::Timer
{
    using ServiceFunc = void(Owner::*)();
    
    TimerRunner(Owner& o,
                ServiceFunc serviceFn,
                TimerLaunchType tlt = TimerLaunchType::StartImmediately) :
    owner(o),
    serviceFunc(serviceFn)
    {
        static_assert(intervalInMS > 0, "intervalInMS must be greater than 0");
        
        if( tlt == TimerLaunchType::StartImmediately )
        {
            startTimer(intervalInMS);
        }
    }
    
    ~TimerRunner() override
    {
        halt();
    }
    
    void launch()
    {
        startTimer(intervalInMS);
    }
    
    void halt()
    {
        stopTimer();
    }
private:
    Owner& owner;
    ServiceFunc serviceFunc;
    
    void timerCallback() override
    {
        (owner.*serviceFunc)();
    }
};


