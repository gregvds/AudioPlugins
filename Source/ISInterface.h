/*
  ==============================================================================

    ISInterface.h
    Created: 30 Dec 2019 11:41:21am
    Author:  Grégoire Vandenschrick

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

static Colour leftColour      = Colour(0xFF00BFFF);
static Colour rightColour     = Colour(0xFF1E90FF);
static Colour transparent     = Colour(0x00FFFFFF);

class RightLookAndFeel : public LookAndFeel_V4,
                         public Component
{
public:
    RightLookAndFeel()
    {
        LookAndFeel_V4::setColour(Slider::ColourIds::thumbColourId, rightColour);
        LookAndFeel_V4::setColour(Slider::ColourIds::rotarySliderFillColourId, transparent);
    }
};

class LeftLookAndFeel : public LookAndFeel_V4,
                        public Component
{
public:
    LeftLookAndFeel()
    {
        LookAndFeel_V4::setColour(Slider::ColourIds::thumbColourId, leftColour);
        LookAndFeel_V4::setColour(Slider::ColourIds::trackColourId, transparent);
        LookAndFeel_V4::setColour(Slider::ColourIds::rotarySliderFillColourId, transparent);
        LookAndFeel_V4::setColour(Slider::ColourIds::rotarySliderOutlineColourId, transparent);
    }
};

class ISInterface :  public Component

{
public:
    ISInterface()
    {
        setOpaque (true);
        tooltipWindow->setMillisecondsBeforeTipAppears (500);
        
        rightSlider.setName("rightSlider");
        rightSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
        rightSlider.setRange(-100.0f, 100.0f);
        rightSlider.setValue(100.0f);
        rightSlider.onValueChange = [this] {};
        //rightSlider.setTooltip(TRANS ("Direct signal attenuation"));
        addAndMakeVisible(rightSlider);
        
        leftSlider.setName("leftSlider");
        leftSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
        leftSlider.setRange(-100.0f, 100.0f);
        leftSlider.setValue(-100.0f);
        leftSlider.onValueChange = [this] { if (leftSlider.getValue() <= -2.7f)
                                            {
                                                rightSlider.setValue( -leftSlider.getValue());
                                            }
                                            else
                                            {
                                                leftSlider.setValue(-2.7f);
                                                rightSlider.setValue(-2.7f);
                                            }
                                            updateEditorValues(leftSlider.getValue());
                                          };
        //rightSlider.setTooltip(TRANS ("Direct signal attenuation"));
        addAndMakeVisible(leftSlider);
        
        rightSlider.setLookAndFeel(&rightLookAndFeel);
        leftSlider.setLookAndFeel(&leftLookAndFeel);
    }
    ~ISInterface()
    {
        childrenOfGUI = {};
    }
//==============================================================================

    void paint(Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
        
        bounds = getLocalBounds();
        
        g.setColour (Colours::silver);
        bounds = bounds.reduced(3,3);
        g.drawRoundedRectangle (bounds.toFloat(), 5, 2);
        dials = bounds.removeFromTop(20);
        
        rightSlider.setBounds(bounds);
        leftSlider.setBounds(bounds);

    }
//==============================================================================

//==============================================================================
private:

    Slider* getSlider(String name)
    {
        if (childrenOfGUI.size() == 0)
        {
            DBG("Refilling childrenOfGUI...");
            childrenOfGUI = this->getParentComponent()->getChildren();
        }
        for (int i = 0; i < childrenOfGUI.size(); i++)
        {
            if (childrenOfGUI[i]->getName() == name)
            {
                return static_cast<Slider*>(childrenOfGUI[i]);
            }
        }
        return nullptr;
        
    }
    
//==============================================================================
    void updateEditorValues(float position)
    {
        // position varie from -100.0f (uncrossfeeded) to 0.0f (fully crossfeeded, hence mono)
        DBG("Position: " << position);
        
        float normalizedPosition = (position + 97.3f) / 97.3f;
        // normalizedPosition goes from 0.0f to 1.0f
        
        Slider* delaySlider           = getSlider("delaySlider");
        Slider* qSlider               = getSlider("qSlider");
        Slider* xfeedQSlider          = getSlider("xfeedQSlider");
        Slider* frequencySlider       = getSlider("frequencySlider");
        Slider* xfeedFrequencySlider  = getSlider("xfeedFrequencySlider");
        Slider* separationSlider      = getSlider("separationSlider");
        Slider* xfeedSeparationSlider = getSlider("xfeedSeparationSlider");
        Slider* directGainSlider      = getSlider("directGainSlider");
        Slider* xfeedGainSlider       = getSlider("xfeedGainSlider");
        
        // direct gain could never move
        if (position < -45.0f)
            directGainSlider->setValue(0.0f);
        else
        {
            float directGainNormalizedPosition = (position + 45.0f)/42.3f;
            directGainSlider->setValue(0.0f - (3.0f
                                   * (sin((-90.0f + (180.0f * directGainNormalizedPosition)) * MathConstants<float>::pi / 180.0f) + 1.0f) / 2.0f));
        }
        // crossfeed gain should go from -10.0dB to 0.0dB
        xfeedGainSlider->setValue( -10.0f + 7.2f * normalizedPosition);
        
        // Total delay should go from around 500 to 0 microseconds
        if (position == -100.0f)
            delaySlider->setValue(0.0f);
        else if (position > -100.0f and position < -47.78f)
        {
            float delayNormalizedPosition = (position + 47.78f) / 52.22f;
            delaySlider->setValue(delaySlider->getMaximum() - (delaySlider->getMaximum() - delaySlider->getMinimum())
                                   * cos(180.0f * delayNormalizedPosition * MathConstants<float>::pi / 180.0f));
        }
        
        // Direct filter Intensity should go from 0 to a maximum to a low value again at the end
        separationSlider->setValue(separationSlider->getMaximum() - (separationSlider->getMaximum() - separationSlider->getMinimum() / 3.0f)
                                   * sin(170.0f * normalizedPosition * MathConstants<float>::pi / 180.0f));
        // Crossfeed filter Intensity should go from high to minimal
        xfeedSeparationSlider->setValue(xfeedSeparationSlider->getMaximum() - (xfeedSeparationSlider->getMaximum() - xfeedSeparationSlider->getMinimum())
                                   * sin(170.0f * normalizedPosition * MathConstants<float>::pi / 180.0f));
        // Direct filter freq is irrelevant at first, and could stay very low (300?)
        frequencySlider->setValue(frequencySlider->getMinimum() + (frequencySlider->getMaximum() - frequencySlider->getMinimum()) / 4.0f
                                   * sin(180.0f * normalizedPosition * MathConstants<float>::pi / 180.0f));
        // Crossfeed filter freq could begin quite high and could go back to very low at the end (300)
        xfeedFrequencySlider->setValue(xfeedFrequencySlider->getMaximum() - (xfeedFrequencySlider->getMaximum() - xfeedFrequencySlider->getMinimum())
                                   * (normalizedPosition * normalizedPosition));
        // Direct Q is irrelevant at the beginning, could stay low for all position
        qSlider->setValue(qSlider->getMinimum() + (qSlider->getMaximum() - qSlider->getMinimum()) / 4.0f
                                   * sin(180.0f * normalizedPosition * MathConstants<float>::pi / 180.0f));
        
        // Crossfeed Q filter should be hard at first and smooth out at the end
        xfeedQSlider->setValue(xfeedQSlider->getMaximum()*2.0f/3.0f - (xfeedQSlider->getMaximum() - xfeedQSlider->getMinimum()) / 3.0f
                                   * sin(180.0f * normalizedPosition * MathConstants<float>::pi / 180.0f));
    }

//==============================================================================

    Array<Component *> childrenOfGUI;
    
    SharedResourcePointer<TooltipWindow> tooltipWindow;
    
    Rectangle<int> bounds;
    Rectangle<int> dials;
    // These must be declared before the components using them
    RightLookAndFeel rightLookAndFeel;
    LeftLookAndFeel leftLookAndFeel;

    
    Slider rightSlider { Slider::RotaryVerticalDrag, Slider::NoTextBox };
    Slider leftSlider { Slider::RotaryVerticalDrag, Slider::NoTextBox };
    
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ISInterface)
};

