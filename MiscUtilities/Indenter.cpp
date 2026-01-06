/*
  ==============================================================================

    Indenter.cpp
    Created: 18 Sep 2025 4:55:45pm
    Author:  Matkat Music LLC

  ==============================================================================
*/

#include "Indenter.h"

Indenter::Indenter()
{
    for( int i = 0; i < count; ++i )
    {
        str << "  ";
    }
    ++count;
}

Indenter::operator juce::String()
{
    return str;
}

Indenter::~Indenter()
{
    --count;
}

int Indenter::count = 0;
