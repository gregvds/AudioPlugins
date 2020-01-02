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
#include "ISInterface.h"

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

class MyColourSelector : public ColourSelector
{
public:
    MyColourSelector()
    {
        
    }
    
    ~MyColourSelector()
    {
        
    }
    
    virtual int getNumSwatches() const override
    {
        return colourSwatches.size();
    }
    
    virtual Colour getSwatchColour(int index) const override
    {
        return colourSwatches.getUnchecked(index);
    }
    
    void setSwatchColour(int index, const Colour &newColour) override
    {
        colourSwatches.set(index, newColour);
    }
private:
    Array<Colour> colourSwatches;
};


class MyPreferencesPanel : public PreferencesPanel
{
public:
    MyPreferencesPanel()
    {
        colourSelector.setSwatchColour(0, delayColour);
        colourSelector.setSwatchColour(1, freqColour);
        colourSelector.setSwatchColour(2, bandwidthColour);
        colourSelector.setSwatchColour(3, separationColour);
        colourSelector.setSwatchColour(4, xfeedColour);
        colourSelector.setSwatchColour(5, directColour);
        colourSelector.setSwatchColour(6, outColour);
        //DBG("number of swatches: " << colourSelector.getNumSwatches());
    }
    ~MyPreferencesPanel()
    {
        
    }

    virtual Component* createComponentForPage (const String &pageName) override
    {
        if (pageName == "Colours")
        {
            return &colourSelector;
        }
        //DBG("Nothing recognized");
        return nullptr;
    }
    MyColourSelector colourSelector;
private:
    
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
    GainSliderAudioProcessorEditor (GainSliderAudioProcessor&, AudioProcessorValueTreeState& vts);
    ~GainSliderAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void sliderValueChanged(Slider *slider) override;
    void comboBoxChanged(ComboBox *comboBox) override;
    void buttonClicked(Button *toggleButton) override;
    
private:
    
#ifdef JUCE_OPENGL
    OpenGLContext           openGLContext;
#endif
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GainSliderAudioProcessor& processor;
    AudioProcessorValueTreeState& valueTreeState;
    
    // These must be declared before the components using them
    RotaryLookAndFeel rotaryLookAndFeel;
    RotaryLookAndFeel rotaryLookAndFeel1;
    RotaryLookAndFeel rotaryLookAndFeel2;
    RotaryLookAndFeel rotaryLookAndFeel3;
    RotaryLookAndFeel rotaryLookAndFeel4;
    RotaryLookAndFeel rotaryLookAndFeel5;
    VerticalLookAndFeel verticalLookAndFeel;
    VerticalLookAndFeel verticalLookAndFeel2;
    XfeedLookAndFeel xfeedLookAndFeel;
    DirectLookAndFeel directLookAndFeel;
    LabelLookAndFeel labelLookAndFeel;
    
    ComboBox crossFeedMenu;
    ComboBox guiLayoutMenu;
    
    ToggleButton activeStateToggleButton { "Active" };
    TextButton preferencesButton { "preferencesButton", "Preferences" };
    
    MyPreferencesPanel prefsPanel;
    
    SharedResourcePointer<TooltipWindow> tooltipWindow;
    
    Rectangle<int> bounds;
    //Rectangle<int> topPanel;
    Rectangle<int> menu;
    Rectangle<int> dials;
    Rectangle<int> directSliders;
    Rectangle<int> xfeedSliders;
    Rectangle<int> slider1;
    Rectangle<int> slider2;
    Rectangle<int> slider3;
    Rectangle<int> slider4;
    Rectangle<int> slider5;
    Rectangle<int> slider6;
    
    Rectangle<int> basePanel;
    Rectangle<int> iSIPanel;
    Rectangle<int> displayPanel1;
    Rectangle<int> displayPanel2;

    //Rectangle<int> leftSpectrumPart;
    //Rectangle<int> spectrumFrame1;
    //Rectangle<int> spectrumFrame2;
    
    bool firstRun = true;
    
    int guiSizes [4] [2] = {{4* DIALSIZE,                 3*(DIALSIZE + TEXTBOXHEIGT + LABELHEIGHT) + TEXTBOXHEIGT},
                            {4* DIALSIZE + SPECTRUMWIDTH, 3*(DIALSIZE + TEXTBOXHEIGT + LABELHEIGHT) + TEXTBOXHEIGT},
                            {4* DIALSIZE + SPECTRUMWIDTH, 3*(DIALSIZE + TEXTBOXHEIGT + LABELHEIGHT) + TEXTBOXHEIGT},
                            {4* DIALSIZE + SPECTRUMWIDTH, 3*(DIALSIZE + TEXTBOXHEIGT + LABELHEIGHT) + TEXTBOXHEIGT + SPECTRUMHEIGHT}};
    
    std::map < String, std::map < String, float > > settingsDictionnary;
    
public:
 
    Slider delaySlider;
    Slider frequencySlider;
    Slider xfeedFrequencySlider;
    Slider qSlider;
    Slider xfeedQSlider;
    Slider separationSlider;
    Slider xfeedSeparationSlider;
    Slider directGainSlider;
    Slider xfeedGainSlider;
    
    Label delayLabel;
    Label frequencyLabel;
    Label xfeedFrequencyLabel;
    Label qLabel;
    Label xfeedQLabel;
    Label separationLabel;
    Label xfeedSeparationLabel;
    Label directGainLabel;
    Label xfeedGainLabel;
    
    FilterGraphics filterGraphics;
    SpectrumAnalyser spectrumAnalyser;
    ISInterface iSInterface;
    
    // Things public to be destroyed before private stuff.
    // Destruction operates on a bottom to top of the code order.
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> delaySliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> freqSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> xfeedFreqSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> qSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> xfeedQSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> sepSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> xfeedSepSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> directGainSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> xfeedGainSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::ButtonAttachment> activeStateToggleButtonAttach;
    std::unique_ptr <AudioProcessorValueTreeState::ComboBoxAttachment> guiLayoutMenuAttach;
    std::unique_ptr <AudioProcessorValueTreeState::ComboBoxAttachment> settingsMenuAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainSliderAudioProcessorEditor)
};
