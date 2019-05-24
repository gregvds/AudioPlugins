//
//  FilterGraphics.h
//  PAF
//
//  Created by Grégoire Vandenschrick on 06/05/2019.
//  Copyright © 2019 Greg. All rights reserved.
//
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

static Colour delayColour      = Colour(0xFF726658);
static Colour xfeedColour      = Colour(0xFFe5d865);
static Colour directColour     = Colour(0xFFfbb040);
static Colour separationColour = Colour(0xff6b8e23);
static Colour freqColour       = Colour(0xFFf6f3ed);
static Colour bandwidthColour  = Colour(0x40a88854);

static String microSec = CharPointer_UTF8 (" \xc2\xb5s");

//==============================================================================
/**
 Classes to contain look and feel customizations for different types of sliders
 */

class LinearBarLookAndFeel : public LookAndFeel_V4,
                             public Component
{
public:
    LinearBarLookAndFeel()
    {
        LookAndFeel_V4::setColour(Slider::trackColourId, getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
        LookAndFeel_V4::setColour(Slider::textBoxTextColourId, Colours::silver);
    }
};

//==============================================================================
/**
 */

class FilterGraphics :  public Component,
                        public Slider::Listener,
                        public ComboBox::Listener

{
public:
    FilterGraphics()
    {
        tooltipWindow->setMillisecondsBeforeTipAppears (500);
        
        fillArrays(stepsPerDecuple);
        
        // editable fields
        minDBEditor.setRange (-20.0f, -0.01f);
        minDBEditor.setValue(mindB);
        minDBEditor.setNumDecimalPlacesToDisplay(1);
        minDBEditor.setTextValueSuffix(" dB");
        minDBEditor.setTooltip("Min dB displayed");
        minDBEditor.setLookAndFeel(&linearBarLookAndFeel);
        minDBEditor.addListener(this);
        addAndMakeVisible(minDBEditor);

        maxDBEditor.setRange (0.01f, 20.0f);
        maxDBEditor.setValue(maxdB);
        maxDBEditor.setNumDecimalPlacesToDisplay(1);
        maxDBEditor.setTextValueSuffix(" dB");
        maxDBEditor.setTooltip("Max dB displayed");
        maxDBEditor.setLookAndFeel(&linearBarLookAndFeel);
        maxDBEditor.addListener(this);
        addAndMakeVisible(maxDBEditor);

        minPhaseEditor.setRange (-500.0f, -0.01f);
        minPhaseEditor.setValue(minPhase);
        minPhaseEditor.setNumDecimalPlacesToDisplay(0);
        minPhaseEditor.setTextValueSuffix(microSec);
        minPhaseEditor.setTooltip("Min phase displayed");
        minPhaseEditor.setLookAndFeel(&linearBarLookAndFeel);
        minPhaseEditor.addListener(this);
        addAndMakeVisible(minPhaseEditor);

        maxPhaseEditor.setRange (0.01f, 500.0f);
        maxPhaseEditor.setValue(maxPhase);
        maxPhaseEditor.setNumDecimalPlacesToDisplay(0);
        maxPhaseEditor.setTextValueSuffix(microSec);
        maxPhaseEditor.setTooltip("Max phase displayed");
        maxPhaseEditor.setLookAndFeel(&linearBarLookAndFeel);
        maxPhaseEditor.addListener(this);
        addAndMakeVisible(maxPhaseEditor);
        
        // Combobox for frequency scale choice
        frequencyScaleTypeMenu.addItem("type 1", 1);
        frequencyScaleTypeMenu.addItem("type 2", 2);
        frequencyScaleTypeMenu.addItem("type 3", 3);
        frequencyScaleTypeMenu.setSelectedId(1);
        frequencyScaleTypeMenu.setJustificationType(Justification::centred);
        frequencyScaleTypeMenu.addListener(this);
        frequencyScaleTypeMenu.setTooltip(TRANS ("Frequency scale choice"));
        addAndMakeVisible(frequencyScaleTypeMenu);
        
        // Display fields
        maxPhaseDisplay.setRange (-1000.0f, 1000.0f);
        maxPhaseDisplay.setValue(0.0f);
        maxPhaseDisplay.setNumDecimalPlacesToDisplay(0);
        maxPhaseDisplay.setTextValueSuffix (microSec);
        maxPhaseDisplay.setTooltip("Max phase");
        maxPhaseDisplay.setLookAndFeel(&linearBarLookAndFeel);
        maxPhaseDisplay.setTextBoxIsEditable(false);
        addAndMakeVisible(maxPhaseDisplay);
 
        minPhaseDisplay.setRange (-1000.0f, 1000.0f);
        minPhaseDisplay.setValue(1000.0f);
        minPhaseDisplay.setNumDecimalPlacesToDisplay(0);
        minPhaseDisplay.setTextValueSuffix (microSec);
        minPhaseDisplay.setTooltip("Min phase");
        minPhaseDisplay.setLookAndFeel(&linearBarLookAndFeel);
        minPhaseDisplay.setTextBoxIsEditable(false);
        addAndMakeVisible(minPhaseDisplay);

    }
    ~FilterGraphics()
    {
        childrenOfGUI = {};
    }
    
    enum
    {
        stepsPerDecuple = 50,
        scopeSize = (3 * stepsPerDecuple),
        TEXTBOXHEIGHT = 20,
        TEXTBOXWIDTH = 100
    };

//==============================================================================
    void drawFrame (Graphics& g)
    {
        //auto x       = graph1.getX();
        auto y1      = graph1.getY();
        auto width1  = graph1.getWidth();
        auto height1 = graph1.getHeight();
        auto y2      = graph2.getY();
        auto width2  = graph2.getWidth();
        auto height2 = graph2.getHeight();
        
        float q      = getSlider("qSlider")->getValue();
        float cF     = getSlider("frequencySlider")->getValue();
        lowFreqForQ  = cF - ((cF)/(2.0 * q)) * (1.0 - qGraphicalAdjustmentFactor);
        highFreqForQ = cF + ((cF)/(2.0 * q)) * (1.0 + qGraphicalAdjustmentFactor);
        float lFPos  = jmap(getPositionForFrequency(jlimit(minFreq, maxFreq, lowFreqForQ)), 0.0f, 1.0f, 3.0f, (float) width1 + 6.0f);
        float hFPos  = jmap(getPositionForFrequency(jlimit(minFreq, maxFreq, highFreqForQ)), 0.0f, 1.0f, 3.0f, (float) width1 + 6.0f);
        
        float separationTransparency = - getSlider("separationSlider")->getValue()/(getSlider("separationSlider")->getMaximum() - getSlider("separationSlider")->getMinimum());
        float delayTransparency = getSlider("delaySlider")->getValue()/(getSlider("delaySlider")->getMaximum() - getSlider("delaySlider")->getMinimum());
        
        // Draw Area on which separation is active
        g.setGradientFill(ColourGradient(separationColour.withAlpha(separationTransparency), 0.0f, y1, Colours::transparentWhite, hFPos, y1, false));
        g.fillRoundedRectangle(3.0f, y1, hFPos, height1, 5);
        g.fillRoundedRectangle(3.0f, y2, hFPos, height2, 5);
        
        // Draw bandwidth based on Q values and frequency of filter(s).
        g.setColour (colours[3]);
        g.fillRoundedRectangle(lFPos, y1, hFPos - lFPos, height1, 5);
        g.fillRoundedRectangle(lFPos, y2, hFPos - lFPos, height2, 5);
        
        // Draw xfeed Delay horizontal line
        float delayPosition = getPositionForTime(getSlider("delaySlider")->getValue(), y2, graph2.getBottom());
        Line<float> delayLine = Line<float>(3.0f, delayPosition, (float) width1, delayPosition);
        float dashes [2] = {5,5};
        g.setColour (delayColour.withAlpha(delayTransparency));
        g.drawDashedLine(delayLine, dashes, 2);
        g.drawHorizontalLine (roundToInt (y2), graph2.getX(), graph2.getRight());

        // We have two filters to represent, for crossfeed and direct signals
        for (int j = 0; j < 2; j++)
        {
            for (int i = 1; i < scopeSize; ++i)
            {
                g.setColour (colours[j]);
                // Draw Amp vs freq of filter in graph1
                g.drawLine ({ (float)
                    jmap (getPositionForFrequency((float)scopeFreq[i - 1]), 0.0f, 1.0f, 3.0f, (float) width1 + 9.0f),
                    jmap (jlimit(mindB, maxdB, gains[j] + Decibels::gainToDecibels((float)scopeGain[j][i - 1])), mindB, maxdB, (float) y1 + height1, (float) y1),
                    (float)
                    jmap (getPositionForFrequency((float)scopeFreq[i]), 0.0f, 1.0f, 3.0f, (float) width1 + 9.0f),
                    jmap (jlimit(mindB, maxdB, gains[j] + Decibels::gainToDecibels((float)scopeGain[j][i])), mindB, maxdB, (float) y1 + height1, (float) y1) });
                // Draw Phase vs freq of filter in graph2
                g.drawLine ({ (float)
                    jmap (getPositionForFrequency((float)scopeFreq[i - 1]), 0.0f, 1.0f, 3.0f, (float) width2 + 9.0f),
                    jmap (jlimit(minPhase, maxPhase, phases[j] + getTimeForPhase((float)scopePhase[j][i - 1], (float)scopeFreq[i - 1])), minPhase, maxPhase, (float) y2 + height2, (float) y2),
                    (float)
                    jmap (getPositionForFrequency((float)scopeFreq[i]), 0.0f, 1.0f, 3.0f, (float) width2 + 9.0f),
                    jmap (jlimit(minPhase, maxPhase, phases[j] + getTimeForPhase((float)scopePhase[j][i], (float)scopeFreq[i])), minPhase, maxPhase, (float) y2 + height2, (float) y2) });
            }
            // Draw filter frequency vertical line for each signal.
            // Depending on the filtering type, it can be different
            g.setColour (colours[2]);
            auto x1 = graph1.getX() + graph1.getWidth() * getPositionForFrequency(freqs[j]);
            g.drawVerticalLine (roundToInt (x1), graph1.getY(), graph1.getBottom());
            auto x2 = graph2.getX() + graph2.getWidth() * getPositionForFrequency(freqs[j]);
            g.drawVerticalLine (roundToInt (x2), graph2.getY(), graph2.getBottom());
        }
    }

    void paint(Graphics& g) override
    {
        bounds = getLocalBounds();
        fieldBar = bounds.removeFromTop(TEXTBOXHEIGHT);
        drawingArea = bounds.reduced (3, 0);
        graph1 = drawingArea.removeFromTop(drawingArea.getHeight()/2).reduced(0,3);
        graph2 = drawingArea.removeFromTop(drawingArea.getHeight()).reduced(0,3);
        
        minDBEditor.setBounds(fieldBar.removeFromLeft(TEXTBOXWIDTH).reduced(3,0));
        maxDBEditor.setBounds(fieldBar.removeFromLeft(TEXTBOXWIDTH).reduced(3,0));
        minPhaseEditor.setBounds(fieldBar.removeFromLeft(TEXTBOXWIDTH).reduced(3,0));
        maxPhaseEditor.setBounds(fieldBar.removeFromLeft(TEXTBOXWIDTH).reduced(3,0));
        
        frequencyScaleTypeMenu.setBounds(fieldBar.removeFromLeft(TEXTBOXWIDTH).reduced(3,0));
        
        maxPhaseDisplay.setBounds(fieldBar.removeFromRight(TEXTBOXWIDTH).reduced(3,0));
        minPhaseDisplay.setBounds(fieldBar.removeFromRight(TEXTBOXWIDTH).reduced(3,0));
        
        // draw the curves
        drawFrame(g);

        // Silver frame
        g.setFont (12.0f);
        g.setColour (Colours::silver);
        g.drawRoundedRectangle (graph1.toFloat(), 5, 2);
        g.drawRoundedRectangle (graph2.toFloat(), 5, 2);
        
        // Vertical lines for frequency reference
        for (int i=0; i < 26 ; ++i) {
            auto freq = frequencies[frequencyScaleTypeMenu.getSelectedId() - 1][i];
            if (minFreq < freq and freq < maxFreq)
            {
                g.setColour (Colours::silver.withAlpha (0.3f));
                auto x1 = graph1.getX() + graph1.getWidth() * getPositionForFrequency(freq);
                auto x2 = graph2.getX() + graph2.getWidth() * getPositionForFrequency(freq);
                auto y1 = graph1.getY();
                auto y2 = graph2.getY();
                g.drawVerticalLine (roundToInt (x1), graph1.getY(), graph1.getBottom());
                g.drawVerticalLine (roundToInt (x2), graph2.getY(), graph2.getBottom());
                
                g.setColour (Colours::silver);
                g.addTransform(AffineTransform::rotation(-MathConstants<float>::halfPi, roundToInt (x1 + 25), y1 + 28));
                g.drawFittedText ((freq < 1000) ? String (freq) + " Hz" : String (freq / 1000, 1) + " kHz",
                                  roundToInt (x1), graph1.getY() + 3, 50, 15, Justification::right, 1);
                g.addTransform(AffineTransform::rotation(MathConstants<float>::halfPi, roundToInt (x1 + 25), y1 + 28));
                g.addTransform(AffineTransform::rotation(-MathConstants<float>::halfPi, roundToInt (x2 + 25), y2 + 28));
                g.drawFittedText ((freq < 1000) ? String (freq) + " Hz" : String (freq / 1000, 1) + " kHz",
                                  roundToInt (x2), graph2.getY() + 3, 50, 15, Justification::right, 1);
                g.addTransform(AffineTransform::rotation(MathConstants<float>::halfPi, roundToInt (x2 + 25), y2 + 28));
            }
        }
        
        // Horizontal lines for dB reference
        for (int i=0; i < dBScaleTickNumber; i++)
        {
            g.setColour (Colours::silver.withAlpha (0.3f));
            auto y = getPositionForDB(roundToInt (mindB + (i * (maxdB - mindB) / dBScaleTickNumber)), graph1.getY(), graph1.getBottom());
            if ((getPositionForDB(mindB, graph1.getY(), graph1.getBottom()) > y) and (y > getPositionForDB(maxdB, graph1.getY(), graph1.getBottom())))
            {
                g.drawHorizontalLine (roundToInt (y), graph1.getX(), graph1.getRight());
                g.setColour (Colours::silver);
                g.drawFittedText (String (roundToInt (mindB + (i * (maxdB - mindB) / dBScaleTickNumber))) + " dB", graph1.getX() + 3, roundToInt(y + 2), 50, 14, Justification::left, 1);
            }
        }
        g.setColour (Colours::silver.withAlpha (0.6f));
        auto y = getPositionForDB(0.0f, graph1.getY(), graph1.getBottom());
        g.drawHorizontalLine (roundToInt (y), graph1.getX(), graph1.getRight());
        g.setColour (Colours::silver);
        g.drawFittedText ("0 dB", graph1.getX() + 3, roundToInt(y + 2), 50, 14, Justification::left, 1);
        
        // Horizontal lines for phase reference
        for (int i=0; i < phaseScaleTickNumber; i++)
        {
            g.setColour (Colours::silver.withAlpha (0.3f));
            auto y = getPositionForPhase(roundToInt (minPhase + (i * (maxPhase - minPhase) / phaseScaleTickNumber)), graph2.getY(), graph2.getBottom());
            if ((getPositionForPhase(minPhase, graph2.getY(), graph2.getBottom()) > y) and (y > getPositionForPhase(maxPhase, graph2.getY(), graph2.getBottom())))
            {
                g.drawHorizontalLine (roundToInt (y), graph2.getX(), graph2.getRight());
                g.setColour (Colours::silver);
                g.drawFittedText (String (roundToInt (minPhase + (i * (maxPhase - minPhase) / phaseScaleTickNumber))) + " µs", graph2.getX() + 3, roundToInt(y + 2), 50, 14, Justification::left, 1);
            }
        }
        g.setColour (Colours::silver.withAlpha (0.6f));
        auto y2 = getPositionForPhase(0.0f, graph2.getY(), graph2.getBottom());
        g.drawHorizontalLine (roundToInt (y2), graph2.getX(), graph2.getRight());
        g.setColour (Colours::silver);
        g.drawFittedText (String (0.0f) + " µs", graph2.getX() + 3, roundToInt(y2 + 2), 50, 14, Justification::left, 1);
    }
    
    void updatePhasesRange()
    {
        // Reinitialisation of vars
        maxPhaseDisplay.setValue(0.0f);
        minPhaseDisplay.setValue(1000.0f);
        float totalPhase = 0.0f;
        
        // for both filters, crossfeed being [0], direct being [1]
        // for all phases data accross the frequency range
        for (int i = 0; i < scopeSize; ++i)
        {
            totalPhase = (phases[0] + getTimeForPhase((float)scopePhase[0][i], (float)scopeFreq[i])) - (phases[1] + getTimeForPhase((float)scopePhase[1][i], (float)scopeFreq[i]));
            if ( (double)totalPhase > maxPhaseDisplay.getValue())
            {
                maxPhaseDisplay.setValue(totalPhase);
            }
            if ( (double)totalPhase < minPhaseDisplay.getValue())
            {
                minPhaseDisplay.setValue(totalPhase);
            }
        }
    }


//==============================================================================
private:
    
    void mouseDown (const MouseEvent& e) override
    {
        DBG("Mouse down...");
    }
    
    void mouseMove (const MouseEvent& e) override
    {
        if (!graph1.contains (e.x, e.y) and !graph2.contains (e.x, e.y))
        {
            return;
        }
        else
        {
            auto posX            = graph1.getX() + getPositionForFrequency (float (freqs[0])) * graph1.getWidth();
            auto posLowFreqForQ  = graph1.getX() + getPositionForFrequency (float (lowFreqForQ)) * graph1.getWidth();
            auto posHighFreqForQ = graph1.getX() + getPositionForFrequency (float (highFreqForQ)) * graph1.getWidth();
            // If we touch the frequency vertical line
            if (std::abs (posX - e.position.getX()) < clickRadius)
            {
                draggingFreq  = true;
                draggingQ     = false;
                draggingCurve = -1;
                draggingGain  = false;
                draggingPhase = false;

                setMouseCursor (MouseCursor (MouseCursor::LeftRightResizeCursor));
                repaint (graph1);
                repaint (graph2);
                return;
            }
            // If we touch the Q window limit
            else if (std::abs (posLowFreqForQ - e.position.getX()) < clickRadius or std::abs (posHighFreqForQ - e.position.getX()) < clickRadius)
            {
                draggingFreq  = false;
                draggingQ     = true;
                draggingCurve = -1;
                draggingGain  = false;
                draggingPhase = false;
                
                setMouseCursor (MouseCursor (MouseCursor::LeftRightResizeCursor));
                repaint (graph1);
                repaint (graph2);
                return;

            }
            else if (graph1.contains(e.x, e.y))
            {
                draggingCurve = isOnCurve(e, clickRadius);
                if (isPositiveAndBelow(draggingCurve, 2))
                {
                    draggingFreq  = false;
                    draggingQ     = false;
                    draggingGain  = true;
                    draggingPhase = false;
                    
                    setMouseCursor (MouseCursor (MouseCursor::UpDownResizeCursor));
                    repaint (graph1);
                    return;
                }
            }
            else if (graph2.contains(e.x, e.y))
            {
                draggingCurve = isOnCurve(e, clickRadius);
                if (draggingCurve == 2)
                {
                    draggingFreq  = false;
                    draggingQ     = false;
                    draggingGain  = false;
                    draggingPhase = true;
                    
                    setMouseCursor (MouseCursor (MouseCursor::UpDownResizeCursor));
                    repaint (graph2);
                    return;
                }
            }
        }
        draggingFreq  = false;
        draggingQ     = false;
        draggingCurve = -1;
        draggingGain  = false;
        draggingPhase = false;
        setMouseCursor (MouseCursor (MouseCursor::NormalCursor));
        repaint (graph1);
        repaint (graph2);
    }
    
    void mouseDrag (const MouseEvent& e) override
    {
        auto posX = (e.position.getX() - graph1.getX()) / graph1.getWidth();
        auto posY = e.position.getY();
        
        Slider* delaySlider      = getSlider("delaySlider");
        Slider* qSlider          = getSlider("qSlider");
        Slider* frequencySlider  = getSlider("frequencySlider");
        Slider* separationSlider = getSlider("separationSlider");
        Slider* directGainSlider = getSlider("directGainSlider");
        Slider* xfeedGainSlider  = getSlider("xfeedGainSlider");

        if (draggingFreq)
        {
            if (frequencySlider != nullptr)
            {
                frequencySlider->setValue(getFrequencyForPosition(posX));
            }
        }
        if (draggingQ)
        {
            if (qSlider != nullptr)
            {
                auto posFreq     = getPositionForFrequency (float (freqs[0]));
                
                DBG("posX: " << posX);
                DBG("posFreq: " << posFreq);
                DBG("Q: " << 0.5*(freqs[0] / (freqs[0] - getFrequencyForPosition(posX))));
                
                // If adjusting Q from its low limit
                if (posX < posFreq)
                {
                    qSlider->setValue((1.0 - qGraphicalAdjustmentFactor) * (0.5) * (freqs[0] / (freqs[0] - getFrequencyForPosition(posX))));
                }
                // if adjusting Q from its high limit
                else if (posFreq < posX)
                {
                    qSlider->setValue((1.0 + qGraphicalAdjustmentFactor) * (0.5) * (freqs[0] / (getFrequencyForPosition(posX) - freqs[0])));
                }
            }
        }
        if (draggingGain)
        {
            // On the right or left of frequency line, updating gain sliders
            if (posX > getPositionForFrequency(freqs[0]) and xfeedGainSlider != nullptr and draggingCurve == 0)
            {
                xfeedGainSlider->setValue(Decibels::gainToDecibels(getGainForPosition(posY, graph1.getY(), graph1.getBottom()) / getFilterGainForFrequency(getFrequencyForPosition(posX), draggingCurve)));
            }
            if (posX > getPositionForFrequency(freqs[0]) and directGainSlider != nullptr and draggingCurve == 1)
            {
                directGainSlider->setValue(Decibels::gainToDecibels(getGainForPosition(posY, graph1.getY(), graph1.getBottom()) / getFilterGainForFrequency(getFrequencyForPosition(posX), draggingCurve)));
            }
            if (posX < getPositionForFrequency(freqs[0]) and separationSlider != nullptr)
            {
                if (draggingCurve == 0)
                {
                    separationSlider->setValue( - Decibels::gainToDecibels(getGainForPosition(posY, graph1.getY(), graph1.getBottom()) / Decibels::decibelsToGain(xfeedGainSlider->getValue())));
                }
                else if (draggingCurve == 1)
                {
                    separationSlider->setValue(Decibels::gainToDecibels(getGainForPosition(posY, graph1.getY(), graph1.getBottom()) / Decibels::decibelsToGain(directGainSlider->getValue())));
                }
            }
        }
        if (draggingPhase)
        {
            // On the right of frequency line, updating delay slider
            if (draggingCurve == 2 and delaySlider != nullptr)
            {
                delaySlider->setValue(getPhaseForPosition(posY, graph2.getY(), graph2.getBottom()));
            }
        }
    }
    
    void mouseDoubleClick (const MouseEvent& e) override
    {
        DBG("Mouse double clic...");
    }
    
    int isOnCurve(const MouseEvent& e, int clickRadius)
    {
        float delaySearched = getPhaseForPosition(e.position.getY(), graph2.getY(), graph2.getBottom());
        if (std::abs(delaySearched - getSlider("delaySlider")->getValue()) < clickRadius)
        {
            return 2;
        }
        
        float freqSearched = getFrequencyForPosition((e.position.getX() - graph1.getX()) / graph1.getWidth());
        // Test to exclude out of range freqs
        if (freqSearched < minFreq or maxFreq < freqSearched)
        {
            return -1;
        }
        for (int j = 0; j < 2; j++)
        {
            float positionSearched = e.position.getY();
            float gainPosition = getPositionForGain(getFilterGainForFrequency(freqSearched, j) * Decibels::decibelsToGain(gains[j]), graph1.getY(), graph1.getBottom());
            float phasePosition = getPositionForPhase(getFilterPhaseForFrequency(freqSearched, j) + phases [j], graph2.getY(), graph2.getBottom());
            if((std::abs (gainPosition - positionSearched) < clickRadius) or
               (std::abs (phasePosition - positionSearched) < clickRadius))
                return j;
        }
        return -1;
    }
    
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

    void sliderValueChanged(Slider *slider) override
    {
        if (slider == &minDBEditor)
        {
            mindB = slider->getValue();
        }
        else if (slider == &maxDBEditor)
        {
            maxdB = slider->getValue();
        }
        else if (slider == &minPhaseEditor)
        {
            minPhase = slider->getValue();
        }
        else if (slider == &maxPhaseEditor)
        {
            maxPhase = slider->getValue();
        }
    }

    void comboBoxChanged(ComboBox *comboBox) override
    {
        auto frequencyScaleTypeIndex = frequencyScaleTypeMenu.getSelectedId() - 1;
    }

    
//==============================================================================

    float getTimeForPhase(float phase, float freq)
    {
        return (-phase / MathConstants<float>::twoPi) * (1000000.0f / freq);
    }
    
    float getPositionForPhase(float phase, float top, float bottom)
    {
        return jmap(phase, minPhase, maxPhase, bottom, top);
    }
    
    float getPositionForTime(float time, float top, float bottom)
    {
        return jmap(time, minPhase, maxPhase, bottom, top);
    }
    
    float getPhaseForPosition(float pos, float top, float bottom)
    {
        return jmap(pos, bottom, top, minPhase, maxPhase);
    }
    
    float getFrequencyForPosition (float pos)
    {
        return 20.0f * std::pow (2.0f, pos * 10.0f);
    }
    
    float getPositionForFrequency (float freq)
    {
        return (std::log (freq / 20.0f) / std::log (2.0f)) / 10.0f;
    }
    
    float getPositionForGain (float gain, float top, float bottom)
    {
        return jmap (Decibels::gainToDecibels (gain, mindB), mindB, maxdB, bottom, top);
    }
    
    float getGainForPosition (float pos, float top, float bottom)
    {
        return Decibels::decibelsToGain (jmap (pos, bottom, top, mindB, maxdB), mindB);
    }
    
    float getDBForPosition (float pos, float top, float bottom)
    {
        return jmap (pos, bottom, top, mindB, maxdB);
    }
    
    float getPositionForDB (float dB, float top, float bottom)
    {
        return jmap (dB, mindB, maxdB, bottom, top);
    }
    
    float getFilterGainForFrequency(float freq, int filterIndex)
    {
        int scopeIndex = getScopeIndexForFrequency(freq);
        return jmap(freq, (float)scopeFreq[scopeIndex], (float)scopeFreq[scopeIndex + 1], (float)scopeGain[filterIndex][scopeIndex], (float)scopeGain[filterIndex][scopeIndex + 1]);
    }
    
    float getFilterPhaseForFrequency(float freq, int filterIndex)
    {
        int scopeIndex = getScopeIndexForFrequency(freq);
        return jmap(freq, (float)scopeFreq[scopeIndex], (float)scopeFreq[scopeIndex + 1], getTimeForPhase((float)scopePhase[filterIndex][scopeIndex], (float)scopeFreq[scopeIndex]), getTimeForPhase((float)scopePhase[filterIndex][scopeIndex + 1], (float)scopeFreq[scopeIndex + 1]));
    }
    
    float getScopeIndexForFrequency(float freq)
    {
        // return the scopeIndex of the frequency interval in which the frequency lies
        for(int i = 1; i < scopeSize; i++)
        {
            if (scopeFreq[i-1] <= freq and freq <= scopeFreq[i])
            {
                // we found the interval for the frequency
                return i-1;
            }
        }
        //return nanf(<#const char *#>);
    }
    
//==============================================================================

    void fillArrays(int stepsPerDecuple)
    {
        float computedFreq = minFreq;
        while (computedFreq <= maxFreq)
        {
            scopeFreq[scopeIndex] = computedFreq;
            computedFreq *= pow(10.0f, 1.0 / stepsPerDecuple);
            scopeGain[0][scopeIndex] = 0.0f;
            scopePhase[0][scopeIndex] = 0.0f;
            scopeGain[1][scopeIndex] = 0.0f;
            scopePhase[1][scopeIndex] = 0.0f;
            scopeIndex++;
        }
        if (scopeFreq[scopeIndex--] < maxFreq)
        {
            scopeFreq[scopeIndex] = maxFreq;
            scopeGain[0][scopeIndex] = 0.0f;
            scopePhase[0][scopeIndex] = 0.0f;
            scopeGain[1][scopeIndex] = 0.0f;
            scopePhase[1][scopeIndex] = 0.0f;
        }
        scopeIndex = 0;
    }

    
//==============================================================================
    
    Array<Component *> childrenOfGUI;
    
    SharedResourcePointer<TooltipWindow> tooltipWindow;
    
    int  scopeIndex = 0;
    
    float minFreq      = 20.0f;
    float maxFreq      = 20000.0f;
    float frequencies [3] [26] = {{27.5f, 55.0f, 110.0f, 220.0f, 440.0f, 880.0f, 1760.0f, 3320.0f, 6640.0f, 13280.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f,                                   22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f},
                                  {25.0f, 50.0f, 100.0f, 250.0f, 500.0f, 1000.0f, 2500.0f, 5000.0f, 10000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f},
                                  {30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f, 90.0f, 100.0f, 200.0f, 300.0f, 400.0f, 500.0f, 600.0f, 700.0f, 800.0f, 900.0f, 1000.0f, 2000.0f, 3000.0f, 4000.0f, 5000.0f, 6000.0f, 7000.0f, 8000.0f, 9000.0f, 10000.0f}};

    float mindB        = -10.0f;
    float maxdB        = 10.0f;
    int   dBScaleTickNumber = 5;
    
    float minPhase     = -500.0f;
    float maxPhase     = 500.0f;
    int   phaseScaleTickNumber = 9;
    
    float lowFreqForQ  = 20.0f;
    float highFreqForQ = 20000.0f;
    float qGraphicalAdjustmentFactor = 0.4;

    int  clickRadius   = 4;
    bool draggingFreq  = false;
    bool draggingQ     = false;
    int  draggingCurve = -1;
    bool draggingGain  = false;
    bool draggingPhase = false;
    
    Rectangle<int> bounds;
    Rectangle<int> fieldBar;
    Rectangle<int> drawingArea;
    Rectangle<int> graph1;
    Rectangle<int> graph2;
    
    LinearBarLookAndFeel linearBarLookAndFeel;
    
    Slider minDBEditor { Slider::LinearBar, Slider::TextBoxAbove };
    Slider maxDBEditor { Slider::LinearBar, Slider::TextBoxAbove };
    Slider minPhaseEditor { Slider::LinearBar, Slider::TextBoxAbove };
    Slider maxPhaseEditor { Slider::LinearBar, Slider::TextBoxAbove };
    
    ComboBox frequencyScaleTypeMenu;
    
    Slider maxPhaseDisplay { Slider::LinearBar, Slider::TextBoxAbove };
    Slider minPhaseDisplay { Slider::LinearBar, Slider::TextBoxAbove };
                                                                                                                 
    Array<Colour> colours = {xfeedColour, directColour, freqColour, bandwidthColour};
    
//==============================================================================
public:

    double scopeFreq [scopeSize];
    double scopeGain [2][scopeSize];
    double scopePhase [2][scopeSize];
    
    float gains [2] = {0.0f, 0.0f};
    float phases [2] = {0.0f, 0.0f};
    float freqs [2] = {0.0f, 0.0f};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterGraphics)
};
