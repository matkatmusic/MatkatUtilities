/*
  ==============================================================================

    ThreadRunner.h
    Created: 19 Aug 2025 10:04:57am
    Author:  Matkat Music LLC

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


/**
 A simple class that runs a public member function of a given owner class on a separate thread.
    This is useful for running background tasks without blocking the main thread.
 Be sure to use techniques that ensure thread safety when accessing shared resources, and also occasionally wait
 usage:
 @code
 struct MyClass
 {
    bool canRun() const { return true; } // or some condition to check if the task should run
 
    void myBackgroundTask(juce::Thread& thread)
    {
        // Do some background work here
 
        if( thread.threadShouldExit() )
        {
            return;
        }
        // Do more work, or sleep for a while
        thread.wait(100); // Sleep for 100 milliseconds
    }
 
    ThreadRunner<MyClass> backgroundThread { *this, "MyBackgroundThread", &MyClass::myBackgroundTask, &MyClass::canRun, ThreadLaunchType::Immediately };
 };
 @endcode
 */

enum class ThreadLaunchType
{
    Immediately,
    WaitForSignal
};

template<typename OwnerClass>
struct ThreadRunner : juce::Thread
{
    using MemberFn = void (OwnerClass::*)(Thread& threadRunner);
    
    using CanRun = bool (OwnerClass::*)();
    
    ThreadRunner(OwnerClass& owner_, 
                 const juce::String& threadName,
                 MemberFn memberFn,
                 CanRun canRunFn,
                 ThreadLaunchType launchType) :
    juce::Thread(threadName),
    owner(owner_),
    memberFunc(memberFn),
    canRunFunc(canRunFn)
    {
        if( launchType == ThreadLaunchType::Immediately )
            startThread();
    }
    
    ~ThreadRunner() override
    {
        stopThread(4000); // Wait for 4 seconds before forcefully stopping
    }
    
    void run() override
    {
        if( (owner.*canRunFunc)() == false )
            return;
        
        while (!threadShouldExit())
        {
            (owner.*memberFunc)(*this);
        }
    }
private:
    OwnerClass& owner;
    MemberFn memberFunc;
    CanRun canRunFunc;
};
