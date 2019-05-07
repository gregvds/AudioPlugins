/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "SpectrumAnalyser.h"
#include "FilterGraphics.h"

#define DIALSIZE 100
#define SLIDERSIZE 200
#define TEXTBOXWIDTH 80
#define TEXTBOXHEIGT 20
#define LABELHEIGHT 20
#define SPECTRUMWIDTH 800
#define SPECTRUMHEIGHT 300

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

class LabelLookAndFeel : public LookAndFeel_V4
{
public:
    LabelLookAndFeel()
    {
        setColour(Label::ColourIds::textColourId, Colours::silver );
    }
};

//==============================================================================
/**
 */

class GainSliderAudioProcessorEditor  : public AudioProcessorEditor,
                                        public Slider::Listener,
                                        public ComboBox::Listener,
                                        public Button::Listener
{
public:
    GainSliderAudioProcessorEditor (GainSliderAudioProcessor&);
    ~GainSliderAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void sliderValueChanged(Slider *slider) override;
    void comboBoxChanged(ComboBox *comboBox) override;
    void buttonClicked(Button *toggleButton) override;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GainSliderAudioProcessor& processor;
    
    // These must be declared before the components using them
    RotaryLookAndFeel rotaryLookAndFeel;
    VerticalLookAndFeel verticalLookAndFeel;
    LabelLookAndFeel labelLookAndFeel;
    
    ComboBox crossFeedMenu;
    ComboBox filterTypeMenu;
    
    Slider delaySlider;
    Slider frequencySlider;
    Slider separationSlider;
    Slider directGainSlider;
    Slider xfeedGainSlider;
    
    Label delayLabel;
    Label frequencyLabel;
    Label separationLabel;
    Label directGainLabel;
    Label xfeedGainLabel;
    
    ToggleButton activeStateToggleButton { "Active" };
    ToggleButton spectrumAnalyserToggleButton { "Spectrum" };
    
    SharedResourcePointer<TooltipWindow> tooltipWindow;
    
    Rectangle<int> spectrumFrame1;
    Rectangle<int> spectrumFrame2;
    
public:
    // Things public to be destroyed before private stuff.
    // Destruction operates on a bottom to top of the code order.
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> delaySliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> freqSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> sepSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> directGainSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> xfeedGainSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::ButtonAttachment> activeStateToggleButtonAttach;
    std::unique_ptr <AudioProcessorValueTreeState::ButtonAttachment> spectrumToggleButtonAttach;
    std::unique_ptr <AudioProcessorValueTreeState::ComboBoxAttachment > filterTypeMenuAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainSliderAudioProcessorEditor)
};
