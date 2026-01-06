/*
  ==============================================================================

    AudioSystem.cpp
    Created: 5 Jan 2026 4:40:35pm
    Author:  Matkat Music LLC

  ==============================================================================
*/

#include "AudioSystem.h"

namespace AudioUtilities
{
void printAudioSystemDevices() //TODO: customizable logger (templated?)
{
    juce::AudioDeviceManager adm;
    
    juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                 [&adm] (bool granted)
                                 {
                                     int numInputChannels = granted ? 256 : 0;
        int numOutputChannels = 0;
        adm.initialise (numInputChannels, numOutputChannels, nullptr, true, {}, nullptr);
                                 });
    
    const auto& availableTypes = adm.getAvailableDeviceTypes();
    
    juce::String output;
    for( const auto& type : availableTypes )
    {
        output << "audio system type: " << type->getTypeName() << juce::NewLine();
        auto deviceNames = type->getDeviceNames();
        for( auto d : deviceNames )
        {
            output << juce::NewLine() << "  device: " << d << juce::NewLine();
        }
    }
    
    juce::Logger::writeToLog(output);
}

juce::String printDeviceInfo(juce::AudioIODevice& device) //TODO: customizable logger (templated?)
{
    juce::String output;
    output << juce::NewLine() << "  device: " << device.getName() << juce::NewLine();
    
    juce::String bufferSizes;
    bufferSizes << "    bufferSizes: ";
    for( auto b : device.getAvailableBufferSizes() )
    {
        bufferSizes << b << ", ";
    }
    
    output << bufferSizes << juce::NewLine();
    
    juce::String sampleRates;
    sampleRates << "    sampleRates: ";
    for(auto s : device.getAvailableSampleRates() )
    {
        sampleRates << s << ", ";
    }
    output << sampleRates << juce::NewLine();
    
    auto inputChannelNames = juce::String();
    inputChannelNames << "  input channel names: ";
    for( auto icn : device.getInputChannelNames() )
    {
        inputChannelNames << icn << ", ";
    }
    output << inputChannelNames << juce::NewLine();
    
    auto outputChannelNames = juce::String();
    outputChannelNames << "  output channel names: ";
    for( auto icn : device.getInputChannelNames() )
    {
        outputChannelNames << icn << ", ";
    }
    output << outputChannelNames << juce::NewLine();
    
    return output;
}

void printAudioSystemInfo() //TODO: customizable logger (templated?)
{
    juce::AudioDeviceManager adm;
    
    juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                 [&adm] (bool granted)
                                 {
                                     int numInputChannels = granted ? 256 : 0;
        int numOutputChannels = 0;
        adm.initialise (numInputChannels, numOutputChannels, nullptr, true, {}, nullptr);
                                 });
    /*
     is there a default audio device??
     */
    if( auto currentDevice = adm.getCurrentAudioDevice() )
    {
        //juce::Logger::writeToLog("current device: " + currentDevice->getName());
        //juce::Logger::writeToLog( "available channel count: " + juce::String(currentDevice->getInputChannelNames().size()));
        //jassertfalse;
    }
    else
    {
        juce::Logger::writeToLog("no current audio device!!");
        jassertfalse;
        return;
    }
    
    const auto& availableTypes = adm.getAvailableDeviceTypes();
    
    juce::String output;
    for( const auto& type : availableTypes )
    {
        output << "audio system type: " << type->getTypeName() << juce::NewLine();
        auto deviceNames = type->getDeviceNames();
        for( auto d : deviceNames )
        {
            std::unique_ptr<juce::AudioIODevice> device(type->createDevice(d, d));
            if( device != nullptr )
            {
                output << printDeviceInfo(*device);
            }
        }
    }
    
    juce::Logger::writeToLog(output);
}
} //end namespace AudioUtilities
