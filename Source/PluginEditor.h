/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "Visualiser.h"
#include "SpectrumAnalyser.h"

#define DIALSIZE 100
#define SLIDERSIZE 200
#define TEXTBOXWIDTH 80
#define TEXTBOXHEIGT 20
#define LABELHEIGHT 20
#define SPECTRUMWIDTH 500

//==============================================================================
/**
 Classes to contain look and feel customizations for different types of sliders
*/

class RotaryLookAndFeel : public LookAndFeel_V4
{
public:
    RotaryLookAndFeel()
    {
        setColour(Slider::ColourIds::thumbColourId, Colours::olivedrab);
    }
    
};

class VerticalLookAndFeel : public LookAndFeel_V4
{
public:
    VerticalLookAndFeel()
    {
        setColour(Slider::ColourIds::thumbColourId, Colours::khaki);
    }
    
};

//==============================================================================
/**
 */

class GainSliderAudioProcessorEditor  : public AudioProcessorEditor,
                                        public Slider::Listener,
                                        public ComboBox::Listener
{
public:
    GainSliderAudioProcessorEditor (GainSliderAudioProcessor&);
    ~GainSliderAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void sliderValueChanged(Slider *slider) override;
    void comboBoxChanged(ComboBox *comboBox) override;

    
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GainSliderAudioProcessor& processor;
    
    // These must be declared before the components using them
    RotaryLookAndFeel rotaryLookAndFeel;
    VerticalLookAndFeel verticalLookAndFeel;

    ComboBox crossFeedMenu;
    ComboBox filterTypeMenu;
    
    Slider delaySlider;
    Slider frequencySlider;
    Slider qSlider;
    Slider separationSlider;
    Slider directGainSlider;
    Slider xfeedGainSlider;
    
    Label delayLabel;
    Label frequencyLabel;
    Label qLabel;
    Label separationLabel;
    Label directGainLabel;
    Label xfeedGainLabel;
    
    ToggleButton ActiveStateToggleButton { "Active" };
    
    SharedResourcePointer<TooltipWindow> tooltipWindow;
    
    //Visualiser visualiser;
    
    
public:
    // Things public to be destroyed before private stuff.
    // Destruction operates on a bottom to top of the code order.
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> delaySliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> freqSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> qSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> sepSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> directGainSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> xfeedGainSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::ButtonAttachment> ActiveStateToggleButtonAttach;
    std::unique_ptr <AudioProcessorValueTreeState::ComboBoxAttachment > filterTypeMenuAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainSliderAudioProcessorEditor)
};
