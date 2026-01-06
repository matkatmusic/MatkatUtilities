/*
  ==============================================================================

    Utilities.h
    Created: 5 Jan 2026 4:07:50pm
    Author:  Matkat Music LLC

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Concepts.h"
#include "../HelperUtilities/BackgroundMultiuserLogger.h"

template<typename Task, typename ... Args>
requires(IsTaskWithBoolResult<Task, Args...>)
bool repeatTaskUntilSuccess(Task task,
                            int numFailsUntilForcedExit,
                            Args&& ... args)
{
    int numFails = 0;
    auto result = false;
    do
    {
        result = task(std::forward<Args>(args)...);
        if( result == false )
        {
            ++numFails;
            
            if(numFailsUntilForcedExit > 0 && numFails >= numFailsUntilForcedExit )
            {
                BML::writeToLog("repeatTaskUntilSuccess: task failed " + juce::String(numFails) + " times, forcing exit!");
                break;
            }
        }
    }
    while( result == false );
    
    return result;
}
