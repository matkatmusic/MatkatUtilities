/*
  ==============================================================================

    Indenter.h
    Created: 18 Sep 2025 4:55:45pm
    Author:  Matkat Music LLC

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct Indenter
{
    static int count;
    Indenter();
    operator juce::String();
    ~Indenter();
private:
    juce::String str;
};

#define USE_INDENTER true

#if USE_INDENTER
    #define IND_CAT_(a,b) a##b //performs concatentation of 'a' and 'b'
    #define IND_CAT(a,b)  IND_CAT_(a,b) //forces macro-expansion of the arguments passed in.  remember that 'b' is __LINE__ most of the time and needs to be expanded into its numeric value first before concatinating 'b' with 'a'.
    // keep an object alive for the whole statement/scope
    //this creates a variable name from the word "_indent_guard_" and the line number the macro is used on
    #define INDENT [[maybe_unused]] ::Indenter IND_CAT(_indent_guard_, __LINE__);
#else
    #define INDENT
#endif

