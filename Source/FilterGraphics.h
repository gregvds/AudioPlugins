//
//  FilterGraphics.h
//  PAF
//
//  Created by Grégoire Vandenschrick on 06/05/2019.
//  Copyright © 2019 Greg. All rights reserved.
//
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

static Colour xfeedColour = Colour(0xFFe5d865);
static Colour directColour = Colour(0xFFfbb040);
static Colour freqColour = Colour(0xFFf6f3ed);

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
                        public Timer,
                        public Slider::Listener

{
public:
    FilterGraphics()
    {
        tooltipWindow->setMillisecondsBeforeTipAppears (500);
        
        startTimerHz (30);
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
        
    }
    
    enum
    {
        stepsPerDecuple = 50,
        scopeSize = (3 * stepsPerDecuple),
        TEXTBOXHEIGHT = 20,
        TEXTBOXWIDTH = 100
    };
//==============================================================================
    void timerCallback() override
    {
        updatePhasesRange();
        repaint();
    }

//==============================================================================
    void drawFrame (Graphics& g)
    {
        auto y1      = graph1.getY();
        auto width1  = graph1.getWidth();
        auto height1 = graph1.getHeight();
        auto y2      = graph2.getY();
        auto width2  = graph2.getWidth();
        auto height2 = graph2.getHeight();
        
        // We have two filters to represent, for crossfeed and for direct signals
        for (int j = 0; j < 2; j++)
        {
            for (int i = 1; i < scopeSize; ++i)
            {
                g.setColour (colours[j]);
                // Draw Amp vs freq of filter
                g.drawLine ({ (float)
                    jmap (getPositionForFrequency((float)scopeFreq[i - 1]), 0.0f, 1.0f, 3.0f, (float) width1 + 9.0f),
                    jmap (jlimit(mindB, maxdB, gains[j] + Decibels::gainToDecibels((float)scopeGain[j][i - 1])), mindB, maxdB, (float) y1 + height1, (float) y1),
                    (float)
                    jmap (getPositionForFrequency((float)scopeFreq[i]), 0.0f, 1.0f, 3.0f, (float) width1 + 9.0f),
                    jmap (jlimit(mindB, maxdB, gains[j] + Decibels::gainToDecibels((float)scopeGain[j][i])), mindB, maxdB, (float) y1 + height1, (float) y1) });
                // Draw Phase vs freq of filter
                g.drawLine ({ (float)
                    jmap (getPositionForFrequency((float)scopeFreq[i - 1]), 0.0f, 1.0f, 3.0f, (float) width2 + 9.0f),
                    jmap (jlimit(minPhase, maxPhase, phases[j] + getTimeForPhase((float)scopePhase[j][i - 1], (float)scopeFreq[i - 1])), minPhase, maxPhase, (float) y2 + height2, (float) y2),
                    (float)
                    jmap (getPositionForFrequency((float)scopeFreq[i]), 0.0f, 1.0f, 3.0f, (float) width2 + 9.0f),
                    jmap (jlimit(minPhase, maxPhase, phases[j] + getTimeForPhase((float)scopePhase[j][i], (float)scopeFreq[i])), minPhase, maxPhase, (float) y2 + height2, (float) y2) });
            }
            // Draw filter frequency vertical line for each signal. Depending on the filtering type, it can be different
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
        
        maxPhaseDisplay.setBounds(fieldBar.removeFromRight(TEXTBOXWIDTH).reduced(3,0));
        minPhaseDisplay.setBounds(fieldBar.removeFromRight(TEXTBOXWIDTH).reduced(3,0));

        // Silver frame
        g.setFont (12.0f);
        g.setColour (Colours::silver);
        g.drawRoundedRectangle (graph1.toFloat(), 5, 2);
        g.drawRoundedRectangle (graph2.toFloat(), 5, 2);
        
        // Vertical lines for frequency reference
        for (int i=0; i < frequencies.size() ; ++i) {
            g.setColour (Colours::silver.withAlpha (0.3f));
            auto freq = frequencies[i];
            auto x1 = graph1.getX() + graph1.getWidth() * getPositionForFrequency(freq);
            g.drawVerticalLine (roundToInt (x1), graph1.getY(), graph1.getBottom());
            auto x2 = graph2.getX() + graph2.getWidth() * getPositionForFrequency(freq);
            g.drawVerticalLine (roundToInt (x2), graph2.getY(), graph2.getBottom());
            
            g.setColour (Colours::silver);
            g.drawFittedText ((freq < 1000) ? String (freq) + " Hz" : String (freq / 1000, 1) + " kHz",
                              roundToInt (x1 + 3), graph1.getY() + 3, 50, 15, Justification::left, 1);
            g.drawFittedText ((freq < 1000) ? String (freq) + " Hz" : String (freq / 1000, 1) + " kHz",
                              roundToInt (x2 + 3), graph2.getY() + 3, 50, 15, Justification::left, 1);
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

        // draw the curves
        drawFrame(g);
    }

//==============================================================================
private:
    
    void mouseDown (const MouseEvent& e) override
    {
        
    }
    
    void mouseMove (const MouseEvent& e) override
    {
        if (!graph1.contains (e.x, e.y) and !graph2.contains (e.x, e.y))
        {
            return;
        }
        else
        {
            auto posX = graph1.getX() + getPositionForFrequency (float (freqs[0])) * graph1.getWidth();
            // If we touch the frequency vertical line
            if (std::abs (posX - e.position.getX()) < clickRadius)
            {
                draggingFreq = true;
                draggingCurve = -1;
                draggingGain = false;
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
                    draggingFreq = false;
                    draggingGain = true;
                    draggingPhase = false;
                    setMouseCursor (MouseCursor (MouseCursor::UpDownResizeCursor));
                    repaint (graph1);
                    return;
                }
            }
            else if (graph2.contains(e.x, e.y))
            {
                draggingCurve = isOnCurve(e, clickRadius);
                if (isPositiveAndBelow(draggingCurve, 2))
                {
                    draggingFreq = false;
                    draggingGain = false;
                    draggingPhase = true;
                    setMouseCursor (MouseCursor (MouseCursor::UpDownResizeCursor));
                    repaint (graph2);
                    return;
                }
            }
        }
        draggingFreq = false;
        draggingCurve = -1;
        draggingGain = false;
        draggingPhase = false;
        setMouseCursor (MouseCursor (MouseCursor::NormalCursor));
        repaint (graph1);
        repaint (graph2);
    }
    
    void mouseDrag (const MouseEvent& e) override
    {
        const Array<Component *> childrenOfGUI= this->getParentComponent()->getChildren();
        if (draggingFreq)
        {
            auto pos = (e.position.getX() - graph1.getX()) / graph1.getWidth();
            for (int i = 0; i < childrenOfGUI.size(); i++)
            {
                if (childrenOfGUI[i]->getName() == "frequencySlider")
                {
                    static_cast<Slider*>(childrenOfGUI[i])->setValue(getFrequencyForPosition(pos));
                }
            }
        }
        if (draggingGain)
        {
            auto posX = (e.position.getX() - graph1.getX()) / graph1.getWidth();
            auto posY = e.position.getY();
            // On the right or left of frequency line, updating gain sliders
            for (int i = 0; i < childrenOfGUI.size(); i++)
            {
                if (draggingCurve == 0 and childrenOfGUI[i]->getName() == "xfeedGainSlider")
                {
                    static_cast<Slider*>(childrenOfGUI[i])->setValue(Decibels::gainToDecibels(getGainForPosition(posY, graph1.getY(), graph1.getBottom()) / getFilterGainForFrequency(getFrequencyForPosition(posX), draggingCurve)));
                }
                if (draggingCurve == 1 and childrenOfGUI[i]->getName() == "directGainSlider")
                {
                    static_cast<Slider*>(childrenOfGUI[i])->setValue(Decibels::gainToDecibels(getGainForPosition(posY, graph1.getY(), graph1.getBottom()) / getFilterGainForFrequency(getFrequencyForPosition(posX), draggingCurve)));
                }
            }
        }
        if (draggingPhase)
        {
            auto posX = (e.position.getX() - graph2.getX()) / graph2.getWidth();
            auto posY = e.position.getY();
            // On the right of frequency line, updating delay slider
            if (posX > getPositionForFrequency(freqs[0]))
            {
                for (int i = 0; i < childrenOfGUI.size(); i++)
                {
                    if (draggingCurve == 0 and childrenOfGUI[i]->getName() == "delaySlider")
                    {
                        //DBG(getFrequencyForPosition(posX));
                        //DBG(getPhaseForPosition(posY, graph2.getY(), graph2.getBottom()));
                        //DBG(getTimeForPhase(getFilterPhaseForFrequency(getFrequencyForPosition(posX), 0), getFrequencyForPosition(posX)));
                        static_cast<Slider*>(childrenOfGUI[i])->setValue(getPhaseForPosition(posY, graph2.getY(), graph2.getBottom()) - getTimeForPhase(getFilterPhaseForFrequency(getFrequencyForPosition(posX), 0), getFrequencyForPosition(posX)));
                    }
                }
            }
            // On the left of frequency line, updating separation sliders
            else
            {
                for (int i = 0; i < childrenOfGUI.size(); i++)
                {
                    if (childrenOfGUI[i]->getName() == "separationSlider")
                    {
                        //static_cast<Slider*>(childrenOfGUI[i])->setValue();
                    }
                }

            }

        }
    }
    
    void mouseDoubleClick (const MouseEvent& e) override
    {
        
    }
    
    int isOnCurve(const MouseEvent& e, int clickRadius)
    {
        
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

//==============================================================================

    float getTimeForPhase(float phase, float freq)
    {
        return (-phase / MathConstants<float>::twoPi) * (1000000.0f / freq);
    }
    
    float getPositionForPhase(float phase, float top, float bottom)
    {
        return jmap(phase, minPhase, maxPhase, bottom, top);
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

    }
    
//==============================================================================

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

    SharedResourcePointer<TooltipWindow> tooltipWindow;
    
    int clickRadius = 4;
    bool draggingFreq = false;
    int draggingCurve = -1;
    bool draggingGain = false;
    bool draggingPhase = false;

    int scopeIndex = 0;
    
    float minFreq = 20.0f;
    float maxFreq = 20000.0f;
    Array<float> frequencies = {25.0f, 50.0f, 100.0f, 250.0f, 500.0f, 1000.0f, 2500.0f, 5000.0f, 10000.0f};

    float mindB = -10.0f;
    float maxdB = 10.0f;
    int   dBScaleTickNumber = 5;
    
    float minPhase = -500.0f;
    float maxPhase = 500.0f;
    int   phaseScaleTickNumber = 9;

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
    
    Slider maxPhaseDisplay { Slider::LinearBar, Slider::TextBoxAbove };
    Slider minPhaseDisplay { Slider::LinearBar, Slider::TextBoxAbove };
                                                                                                                 
    Array<Colour> colours = {xfeedColour, directColour, freqColour};
    
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
