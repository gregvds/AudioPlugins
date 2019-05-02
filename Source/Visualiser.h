/*
  ==============================================================================

    Visualiser.h
    Created: 2 May 2019 2:34:14pm
    Author:  Joshua Hodge

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


class Visualiser  : public AudioVisualiserComponent
{
public:
    Visualiser() : AudioVisualiserComponent(2)
    {
        setBufferSize(64);
        setSamplesPerBlock(64);
        setColours(Colours::black , Colours::beige);
    }
};
