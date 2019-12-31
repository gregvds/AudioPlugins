/*
  ==============================================================================

    ISInterface.h
    Created: 30 Dec 2019 11:41:21am
    Author:  Grégoire Vandenschrick

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//static Colour leftColour      = Colour(0xFF00BFFF);
static Colour leftColour      = Colour(0xFF1E90FF);
static Colour rightColour     = Colour(0xFFFF8C00);
static Colour transparent     = Colour(0x00FFFFFF);


class HdphnzLookAndFeel : public LookAndFeel_V4,
                        public Component
{
public:
    HdphnzLookAndFeel()
    {
        LookAndFeel_V4::setColour(Slider::ColourIds::thumbColourId, leftColour);
        LookAndFeel_V4::setColour(Slider::ColourIds::trackColourId, transparent);
        LookAndFeel_V4::setColour(Slider::ColourIds::rotarySliderFillColourId, transparent);
    }
    
    void drawRotarySlider (Graphics &g, int x, int y, int width, int height, float sliderPositionProportional, float rotaryStartAngle, float rotaryEndAngle, Slider &slider) override
    {
        auto outline = slider.findColour (Slider::rotarySliderOutlineColourId);
        auto bounds = Rectangle<int> (x, y, width, height).toFloat().reduced (10);
        auto radius = jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto lineW = jmin (8.0f, radius * 0.5f);
        auto arcRadius = radius - lineW * 0.5f;

        float centerX = x + width / 2.0f;
        float centerY = y + height / 2.0f;
        float angle = MathConstants<float>::halfPi + sliderPositionProportional * MathConstants<float>::pi;
        float rotationAngle = sliderPositionProportional * (rotaryEndAngle - rotaryStartAngle);
        
        Path backgroundArc;
        backgroundArc.addCentredArc (bounds.getCentreX(),
                                     bounds.getCentreY(),
                                     arcRadius,
                                     arcRadius,
                                     0.0f,
                                     rotaryStartAngle,
                                     rotaryEndAngle,
                                     true);

        g.setColour (outline);
        g.strokePath (backgroundArc, PathStrokeType (lineW, PathStrokeType::curved, PathStrokeType::rounded));

        Path headphoneTransducer;
        headphoneTransducer.addRectangle(-10, 0, 20, 10);
        headphoneTransducer.addTriangle(-20, -6, 20, -6, 0, 10);
        headphoneTransducer.addRectangle(-20, -9, 40, 3);
        headphoneTransducer.addEllipse(-15, -11, 30, 12);
        
        g.setColour(leftColour);
        g.fillPath(headphoneTransducer, AffineTransform::rotation(angle).translated(centerX + radius * sin(rotaryStartAngle + rotationAngle), centerY - radius * cos(rotaryStartAngle + rotationAngle)));

        g.setColour(rightColour);
        g.fillPath(headphoneTransducer, AffineTransform::rotation(-angle).translated(centerX + radius * sin(rotaryEndAngle - rotationAngle), centerY - radius * cos(rotaryEndAngle - rotationAngle)));

    }
};

class ISInterface :  public Component

{
public:
    ISInterface()
    {
        setOpaque (true);
        tooltipWindow->setMillisecondsBeforeTipAppears (500);
        
        hdphnzSlider.setName("hdphnzSlider");
        hdphnzSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
        hdphnzSlider.setRange(-100.0f, 100.0f);
        hdphnzSlider.setValue(-100.0f);
        hdphnzSlider.setMouseDragSensitivity(500.0f);
        hdphnzSlider.onValueChange = [this] { if (hdphnzSlider.getValue() > -2.7f)
                                            {
                                               hdphnzSlider.setValue(-2.7f);
                                            }
                                            updateEditorValues(hdphnzSlider.getValue());
                                          };
        addAndMakeVisible(hdphnzSlider);
        hdphnzSlider.setLookAndFeel(&hdphnzLookAndFeel);
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
        
        hdphnzSlider.setBounds(bounds);

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
        
        float normalizedPosition = (position + 97.3f) / 97.3f;
        
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
            directGainSlider->setValue( 0.0f - (3.0f
                                   * (sin((-90.0f + (180.0f * directGainNormalizedPosition)) * MathConstants<float>::pi / 180.0f) + 1.0f) / 2.0f));
        }
        
        // crossfeed gain should go from -10.0dB to 0.0dB
        xfeedGainSlider->setValue(      -10.0f + 7.2f * normalizedPosition);
        
        // Total delay should go from around 500 to 0 microseconds
        if (position == -100.0f)
            delaySlider->setValue(      0.0f);
        else if (position > -100.0f and position < -47.78f)
        {
            float delayNormalizedPosition = (position + 47.78f) / 52.22f;
            delaySlider->setValue(      delaySlider->getMaximum() - (delaySlider->getMaximum() - delaySlider->getMinimum())
                                   * cos(180.0f * delayNormalizedPosition * MathConstants<float>::pi / 180.0f));
        }
        
        // Direct filter Intensity should go from 0 to a maximum to a low value again at the end
        separationSlider->setValue(     separationSlider->getMaximum() - (separationSlider->getMaximum() - separationSlider->getMinimum() / 3.0f)
                                   * sin(170.0f * normalizedPosition * MathConstants<float>::pi / 180.0f));
        // Crossfeed filter Intensity should go from high to minimal
        xfeedSeparationSlider->setValue(xfeedSeparationSlider->getMaximum() - (xfeedSeparationSlider->getMaximum() - xfeedSeparationSlider->getMinimum())
                                   * sin(170.0f * normalizedPosition * MathConstants<float>::pi / 180.0f));
        // Direct filter freq is irrelevant at first, and could stay very low (300?)
        frequencySlider->setValue(      frequencySlider->getMinimum() + (frequencySlider->getMaximum() - frequencySlider->getMinimum()) / 4.0f
                                   * sin(180.0f * normalizedPosition * MathConstants<float>::pi / 180.0f));
        // Crossfeed filter freq could begin quite high and could go back to very low at the end (300)
        xfeedFrequencySlider->setValue(xfeedFrequencySlider->getMaximum() - (xfeedFrequencySlider->getMaximum() - xfeedFrequencySlider->getMinimum())
                                   * (  normalizedPosition * normalizedPosition));
        // Direct Q is irrelevant at the beginning, could stay low for all position
        qSlider->setValue(              qSlider->getMinimum() + (qSlider->getMaximum() - qSlider->getMinimum()) / 4.0f
                                   * sin(180.0f * normalizedPosition * MathConstants<float>::pi / 180.0f));
        
        // Crossfeed Q filter should be hard at first and smooth out at the end
        xfeedQSlider->setValue(         xfeedQSlider->getMaximum() * 2.0f / 3.0f - (xfeedQSlider->getMaximum() - xfeedQSlider->getMinimum()) / 3.0f
                                   * sin(180.0f * normalizedPosition * MathConstants<float>::pi / 180.0f));
    }

//==============================================================================

    Array<Component *> childrenOfGUI;
    
    SharedResourcePointer<TooltipWindow> tooltipWindow;
    
    Rectangle<int> bounds;
    Rectangle<int> dials;
    // This must be declared before the component using it
    HdphnzLookAndFeel hdphnzLookAndFeel;

    Slider hdphnzSlider { Slider::RotaryVerticalDrag, Slider::NoTextBox };
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ISInterface)
};

