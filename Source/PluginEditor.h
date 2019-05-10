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

//static Colour xfeedColour = Colour(0xFFe5d865);
////Colour directColour = Colour(0xFFe5e27e);
//static Colour directColour = Colour(0xFFfbb040);

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

class XfeedLookAndFeel : public LookAndFeel_V4
{
public:
    XfeedLookAndFeel()
    {
        setColour(Slider::ColourIds::thumbColourId, xfeedColour);
    }
    
};

class DirectLookAndFeel : public LookAndFeel_V4
{
public:
    DirectLookAndFeel()
    {
        setColour(Slider::ColourIds::thumbColourId, directColour);
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
    RotaryLookAndFeel rotaryLookAndFeel1;
    RotaryLookAndFeel rotaryLookAndFeel2;
    RotaryLookAndFeel rotaryLookAndFeel3;
    VerticalLookAndFeel verticalLookAndFeel;
    XfeedLookAndFeel xfeedLookAndFeel;
    DirectLookAndFeel directLookAndFeel;
    LabelLookAndFeel labelLookAndFeel;
    
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
    
    ToggleButton activeStateToggleButton { "Active" };
    ToggleButton spectrumAnalyserToggleButton { "Spectrum" };
    
    SharedResourcePointer<TooltipWindow> tooltipWindow;
    
    Rectangle<int> leftSpectrumPart;
    Rectangle<int> spectrumFrame1;
    Rectangle<int> spectrumFrame2;
    
    bool firstRun = true;
    
    // filter settings, first index is filter type, second is intensity
    // See the filterTypeMenu and crossfeedMenu options
    // [0][0] is shelf filter, Full settings and so on
    float settings [3] [3] [6] = {  {
                                        {0.0f , 400.0f , -5.0f, 0.57f, 5.0f, -5.0f},
                                        {75.0f, 1000.0f, -1.0f, 0.66f, 2.0f, 2.0f},
                                        {}
                                    }, {
                                        {94.0f, 700.0f , -4.0f, 0.45f, 4.0f, -4.0f},
                                        {},
                                        {}
                                    }, {
                                        {67.0f, 400.0f , -3.0f, 0.6f, 3.1f, -3.1f},
                                        {},
                                        {}
                                    }};
    
public:
    // Things public to be destroyed before private stuff.
    // Destruction operates on a bottom to top of the code order.
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> delaySliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> freqSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> qSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> sepSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> directGainSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> xfeedGainSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::ButtonAttachment> activeStateToggleButtonAttach;
    std::unique_ptr <AudioProcessorValueTreeState::ButtonAttachment> spectrumToggleButtonAttach;
    std::unique_ptr <AudioProcessorValueTreeState::ComboBoxAttachment > filterTypeMenuAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainSliderAudioProcessorEditor)
};
