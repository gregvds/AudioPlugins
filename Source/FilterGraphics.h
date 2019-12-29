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
static Colour bandwidthColour  = Colour(0xFFa88854);

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

class FilterGraphics :  public Component

{
public:
    FilterGraphics()
    {
        setOpaque (true);
        tooltipWindow->setMillisecondsBeforeTipAppears (500);
        
        fillArrays(stepsPerDecuple);
        
        // editable fields
        minDBEditor.setRange (-20.0f, -0.01f);
        minDBEditor.setValue(mindB);
        minDBEditor.setNumDecimalPlacesToDisplay(1);
        minDBEditor.setTextValueSuffix(" dB");
        minDBEditor.setTooltip("Min dB displayed");
        minDBEditor.setLookAndFeel(&linearBarLookAndFeel);
        minDBEditor.onValueChange = [this] {mindB = minDBEditor.getValue(); repaint(); };
        addAndMakeVisible(minDBEditor);

        maxDBEditor.setRange (0.01f, 20.0f);
        maxDBEditor.setValue(maxdB);
        maxDBEditor.setNumDecimalPlacesToDisplay(1);
        maxDBEditor.setTextValueSuffix(" dB");
        maxDBEditor.setTooltip("Max dB displayed");
        maxDBEditor.setLookAndFeel(&linearBarLookAndFeel);
        maxDBEditor.onValueChange = [this] {maxdB = maxDBEditor.getValue(); repaint(); };
        addAndMakeVisible(maxDBEditor);

        minPhaseEditor.setRange (-500.0f, -0.01f);
        minPhaseEditor.setValue(minPhase);
        minPhaseEditor.setNumDecimalPlacesToDisplay(0);
        minPhaseEditor.setTextValueSuffix(microSec);
        minPhaseEditor.setTooltip("Min phase displayed");
        minPhaseEditor.setLookAndFeel(&linearBarLookAndFeel);
        minPhaseEditor.onValueChange = [this] {minPhase = minPhaseEditor.getValue(); repaint(); };
        addAndMakeVisible(minPhaseEditor);

        maxPhaseEditor.setRange (0.01f, 500.0f);
        maxPhaseEditor.setValue(maxPhase);
        maxPhaseEditor.setNumDecimalPlacesToDisplay(0);
        maxPhaseEditor.setTextValueSuffix(microSec);
        maxPhaseEditor.setTooltip("Max phase displayed");
        maxPhaseEditor.setLookAndFeel(&linearBarLookAndFeel);
        maxPhaseEditor.onValueChange = [this] {maxPhase = maxPhaseEditor.getValue(); repaint(); };
        addAndMakeVisible(maxPhaseEditor);
        
        // Combobox for frequency scale choice
        frequencyScaleTypeMenu.addItem("type 1", 3);
        frequencyScaleTypeMenu.addItem("type 2", 1);
        frequencyScaleTypeMenu.addItem("type 3", 2);
        frequencyScaleTypeMenu.addItem("type 4", 4);
        frequencyScaleTypeMenu.setSelectedId(4);
        frequencyScaleTypeMenu.setJustificationType(Justification::centred);
        //frequencyScaleTypeMenu.addListener(this);
        frequencyScaleTypeMenu.onChange = [this] {repaint(); };
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

        float xQ     = getSlider("xfeedQSlider")->getValue();
        float xCF    = getSlider("xfeedFrequencySlider")->getValue();
        lowFreqForXQ  = xCF - ((xCF)/(2.0 * xQ)) * (1.0 - qGraphicalAdjustmentFactor);
        highFreqForXQ = xCF + ((xCF)/(2.0 * xQ)) * (1.0 + qGraphicalAdjustmentFactor);
        float xLFPos  = jmap(getPositionForFrequency(jlimit(minFreq, maxFreq, lowFreqForXQ)), 0.0f, 1.0f, 3.0f, (float) width1 + 6.0f);
        float xHFPos  = jmap(getPositionForFrequency(jlimit(minFreq, maxFreq, highFreqForXQ)), 0.0f, 1.0f, 3.0f, (float) width1 + 6.0f);

        float separationTransparency = - getSlider("separationSlider")->getValue()/(getSlider("separationSlider")->getMaximum() - getSlider("separationSlider")->getMinimum());
        float xSeparationTransparency = - getSlider("xfeedSeparationSlider")->getValue()/(getSlider("xfeedSeparationSlider")->getMaximum() - getSlider("xfeedSeparationSlider")->getMinimum());
        float delayTransparency = getSlider("delaySlider")->getValue()/(getSlider("delaySlider")->getMaximum() - getSlider("delaySlider")->getMinimum());
        
        std::pair<double*, double*> minMaxDirectGain = std::minmax_element(std::begin(scopeGain[1]), std::end(scopeGain[1]));
        std::pair<double*, double*> minMaxXfeedGain = std::minmax_element(std::begin(scopeGain[0]), std::end(scopeGain[0]));

        float scopeMaxDirectGain  = jmap (jlimit(mindB, maxdB, gains[1] + Decibels::gainToDecibels((float)*(minMaxDirectGain.second))), mindB, maxdB, (float) y1 + height1, (float) y1);
        float scopeMinDirectGain  = jmap (jlimit(mindB, maxdB, gains[1] + Decibels::gainToDecibels((float)*(minMaxDirectGain.first))), mindB, maxdB, (float) y1 + height1, (float) y1);
        float scopeMaxXfeedGain   = jmap (jlimit(mindB, maxdB, gains[0] + Decibels::gainToDecibels((float)*(minMaxXfeedGain.second))), mindB, maxdB, (float) y1 + height1, (float) y1);
        float scopeMinXfeedGain   = jmap (jlimit(mindB, maxdB, gains[0] + Decibels::gainToDecibels((float)*(minMaxXfeedGain.first))), mindB, maxdB, (float) y1 + height1, (float) y1);

        float scopeMaxDirectPhase = jmap (jlimit(minPhase, maxPhase, getMaxTime(1)), minPhase, maxPhase, (float) y2 + height2, (float) y2);
        float scopeMinDirectPhase = jmap (jlimit(minPhase, maxPhase, getMinTime(1)), minPhase, maxPhase, (float) y2 + height2, (float) y2);
        float scopeMaxXfeedPhase  = jmap (jlimit(minPhase, maxPhase, phases[0] + getMaxTime(0)), minPhase, maxPhase, (float) y2 + height2, (float) y2);
        float scopeMinXfeedPhase  = jmap (jlimit(minPhase, maxPhase, phases[0] + getMinTime(0)), minPhase, maxPhase, (float) y2 + height2, (float) y2);
         
        g.setGradientFill(ColourGradient(separationColour.withAlpha(separationTransparency), 0.0f, y1, Colours::transparentWhite, hFPos, y1, false));
        // Draw Area on which separation is active for Direct Curve in Gain diagram
        g.fillRoundedRectangle(3.0f, scopeMaxDirectGain, hFPos, std::abs(scopeMaxDirectGain - scopeMinDirectGain), 5);
        // Draw Area on which separation is active for Direct Curve in Phase diagram
        g.fillRoundedRectangle(3.0f, scopeMaxDirectPhase, hFPos, std::abs(scopeMaxDirectPhase - scopeMinDirectPhase), 5);

        g.setGradientFill(ColourGradient(separationColour.withAlpha(xSeparationTransparency), 0.0f, y1, Colours::transparentWhite, xHFPos, y1, false));
        // Draw Area on which separation is active for Xfeed Curve in Gain diagram
        g.fillRoundedRectangle(3.0f, scopeMaxXfeedGain, xHFPos, std::abs(scopeMaxXfeedGain - scopeMinXfeedGain), 5);
        // Draw Area on which separation is active for Xfeed Curve in Phase diagram
        
        
        g.fillRoundedRectangle(3.0f, scopeMaxXfeedPhase, xHFPos, std::abs(scopeMaxXfeedPhase - scopeMinXfeedPhase), 5);
        
        g.setColour (bandwidthColour.withAlpha(0.25f));
        // Draw bandwidth based on Q values and frequency of filter(s) for Direct Curve in Gain Diagram.
        g.fillRoundedRectangle(lFPos, scopeMaxDirectGain, hFPos - lFPos, std::abs(scopeMaxDirectGain - scopeMinDirectGain), 5);
        // Draw bandwidth based on Q values and frequency of filter(s) for Direct Curve in Phase Diagram.
        g.fillRoundedRectangle(lFPos, scopeMaxDirectPhase, hFPos - lFPos, std::abs(scopeMaxDirectPhase - scopeMinDirectPhase), 5);

        // Draw bandwidth based on Q values and frequency of filter(s) for Xfeed Curve in Gain Diagram.
        g.fillRoundedRectangle(xLFPos, scopeMaxXfeedGain, xHFPos - xLFPos, std::abs(scopeMaxXfeedGain - scopeMinXfeedGain), 5);
        // Draw bandwidth based on Q values and frequency of filter(s) for Xfeed Curve in Phase Diagram.
        
        
        g.fillRoundedRectangle(xLFPos, scopeMaxXfeedPhase, xHFPos - xLFPos, std::abs(scopeMaxXfeedPhase - scopeMinXfeedPhase), 5);

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
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
        
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
            auto textPosition = roundToInt(y + 2);
            if (graph1.getY() - textPosition < 2)
            {
                textPosition = roundToInt(y - 16);
            }
            if ((getPositionForDB(mindB, graph1.getY(), graph1.getBottom()) - 2 > y) and (y > getPositionForDB(maxdB, graph1.getY(), graph1.getBottom())))
            {
                g.drawHorizontalLine (roundToInt (y), graph1.getX(), graph1.getRight());
                g.setColour (Colours::silver);
                g.drawFittedText (String (roundToInt (mindB + (i * (maxdB - mindB) / dBScaleTickNumber))) + " dB", graph1.getX() + 3, textPosition, 50, 14, Justification::left, 1);
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
        //DBG("Mouse down...");
        mouseDownX = e.position.getX();
        mouseDownY = e.position.getY();
        graph1DBRange = maxdB - mindB;
        graph2PhaseRange = maxPhase - minPhase;
    }
    
    void mouseMove (const MouseEvent& e) override
    {
        // Hereunder we only change the dragging booleans just to identify what's
        // currently attempted to be modified by the mousemove
        if (!graph1.contains (e.x, e.y) and !graph2.contains (e.x, e.y))
        {
            return;
        }
        else
        {
            auto posX                 = graph1.getX() + getPositionForFrequency (float (freqs[0])) * graph1.getWidth();
            auto posXfeedX            = graph1.getX() + getPositionForFrequency (float (freqs[1])) * graph1.getWidth();
            auto posLowFreqForQ       = graph1.getX() + getPositionForFrequency (float (lowFreqForQ)) * graph1.getWidth();
            auto posHighFreqForQ      = graph1.getX() + getPositionForFrequency (float (highFreqForQ)) * graph1.getWidth();
            auto posLowXfeedFreqForQ  = graph1.getX() + getPositionForFrequency (float (lowFreqForXQ)) * graph1.getWidth();
            auto posHighXfeedFreqForQ = graph1.getX() + getPositionForFrequency (float (highFreqForXQ)) * graph1.getWidth();
            
            // If we touch the direct frequency vertical line
            if (std::abs (posX - e.position.getX()) < clickRadius)
            {
                draggingFreq  = true;
                draggingXfeedFreq = false;
                draggingQ     = false;
                draggingXfeedQ = false;
                draggingCurve = -1;
                draggingGain  = false;
                draggingPhase = false;
                
                draggingMinDB = false;
                draggingMaxDb = false;
                draggingMinPhase = false;
                draggingMaxPhase = false;

                setMouseCursor (MouseCursor (MouseCursor::LeftRightResizeCursor));
                repaint (graph1);
                repaint (graph2);
                return;
            }
            // If we touch the xfeed frequency vertical line
            if (std::abs (posXfeedX - e.position.getX()) < clickRadius)
            {
                draggingFreq  = false;
                draggingXfeedFreq = true;
                draggingQ     = false;
                draggingXfeedQ = false;
                draggingCurve = -1;
                draggingGain  = false;
                draggingPhase = false;
                
                draggingMinDB = false;
                draggingMaxDb = false;
                draggingMinPhase = false;
                draggingMaxPhase = false;

                setMouseCursor (MouseCursor (MouseCursor::LeftRightResizeCursor));
                repaint (graph1);
                repaint (graph2);
                return;
            }
            // If we touch the Direct Q window limit
            if (std::abs (posLowFreqForQ - e.position.getX()) < clickRadius or std::abs (posHighFreqForQ - e.position.getX()) < clickRadius)
            {
                draggingFreq  = false;
                draggingXfeedFreq = false;
                draggingQ     = true;
                draggingXfeedQ = false;
                draggingCurve = -1;
                draggingGain  = false;
                draggingPhase = false;
                
                draggingMinDB = false;
                draggingMaxDb = false;
                draggingMinPhase = false;
                draggingMaxPhase = false;

                setMouseCursor (MouseCursor (MouseCursor::LeftRightResizeCursor));
                repaint (graph1);
                repaint (graph2);
                return;
            }
            // If we touch the Xfeed Q window limit
            if (std::abs (posLowXfeedFreqForQ - e.position.getX()) < clickRadius or std::abs (posHighXfeedFreqForQ - e.position.getX()) < clickRadius)
            {
                draggingFreq  = false;
                draggingXfeedFreq = false;
                draggingQ     = false;
                draggingXfeedQ = true;
                draggingCurve = -1;
                draggingGain  = false;
                draggingPhase = false;
                
                draggingMinDB = false;
                draggingMaxDb = false;
                draggingMinPhase = false;
                draggingMaxPhase = false;

                setMouseCursor (MouseCursor (MouseCursor::LeftRightResizeCursor));
                repaint (graph1);
                repaint (graph2);
                return;
            }
            if (graph1.contains(e.x, e.y))
            {
                draggingCurve = isOnCurve(e, clickRadius);
                // If we touch a curve in graph 1
                if (isPositiveAndBelow(draggingCurve, 2))
                {
                    draggingFreq  = false;
                    draggingXfeedFreq = false;
                    draggingQ     = false;
                    draggingXfeedQ = false;
                    draggingGain  = true;
                    draggingPhase = false;
                    
                    draggingMinDB = false;
                    draggingMaxDb = false;
                    draggingMinPhase = false;
                    draggingMaxPhase = false;

                    setMouseCursor (MouseCursor (MouseCursor::UpDownResizeCursor));
                    repaint (graph1);
                    return;
                }
                else
                {
                    //DBG("Dragging nothing in graph1");
                    if (isOnPositiveRange(e))
                    {
                        draggingFreq  = false;
                        draggingXfeedFreq = false;
                        draggingQ     = false;
                        draggingXfeedQ = false;
                        draggingGain  = false;
                        draggingPhase = false;

                        draggingMinDB = false;
                        draggingMaxDb = true;
                        draggingMinPhase = false;
                        draggingMaxPhase = false;

                        setMouseCursor (MouseCursor (MouseCursor::UpDownResizeCursor));
                        repaint (graph1);
                        return;
                    }
                    if (isOnNegativeRange(e))
                    {
                        draggingFreq  = false;
                        draggingXfeedFreq = false;
                        draggingQ     = false;
                        draggingXfeedQ = false;
                        draggingGain  = false;
                        draggingPhase = false;

                        draggingMinDB = true;
                        draggingMaxDb = false;
                        draggingMinPhase = false;
                        draggingMaxPhase = false;

                        setMouseCursor (MouseCursor (MouseCursor::UpDownResizeCursor));
                        repaint (graph1);
                        return;
                    }
                }
            }
            if (graph2.contains(e.x, e.y))
            {
                draggingCurve = isOnCurve(e, clickRadius);
                // if we touch a curve in graph 2
                if (draggingCurve == 2)
                {
                    draggingFreq  = false;
                    draggingXfeedFreq = false;
                    draggingQ     = false;
                    draggingXfeedQ = false;
                    draggingGain  = false;
                    draggingPhase = true;
                    
                    draggingMinDB = false;
                    draggingMaxDb = false;
                    draggingMinPhase = false;
                    draggingMaxPhase = false;

                    setMouseCursor (MouseCursor (MouseCursor::UpDownResizeCursor));
                    repaint (graph2);
                    return;
                }
                else
                {
                    //DBG("Dragging nothing in graph2");
                    if (isOnPositiveRange(e))
                    {
                        draggingFreq  = false;
                        draggingXfeedFreq = false;
                        draggingQ     = false;
                        draggingXfeedQ = false;
                        draggingGain  = false;
                        draggingPhase = false;

                        draggingMinDB = false;
                        draggingMaxDb = false;
                        draggingMinPhase = false;
                        draggingMaxPhase = true;

                        setMouseCursor (MouseCursor (MouseCursor::UpDownResizeCursor));
                        repaint (graph2);
                        return;
                    }
                    if (isOnNegativeRange(e))
                    {
                        draggingFreq  = false;
                        draggingXfeedFreq = false;
                        draggingQ     = false;
                        draggingXfeedQ = false;
                        draggingGain  = false;
                        draggingPhase = false;

                        draggingMinDB = false;
                        draggingMaxDb = false;
                        draggingMinPhase = true;
                        draggingMaxPhase = false;

                        setMouseCursor (MouseCursor (MouseCursor::UpDownResizeCursor));
                        repaint (graph2);
                        return;
                    }
                }
            }
        }
        draggingFreq  = false;
        draggingXfeedFreq = false;
        draggingQ     = false;
        draggingXfeedQ = false;
        draggingCurve = -1;
        draggingGain  = false;
        draggingPhase = false;
        
        draggingMinDB = false;
        draggingMaxDb = false;
        draggingMinPhase = false;
        draggingMaxPhase = false;
        
        setMouseCursor (MouseCursor (MouseCursor::NormalCursor));
        repaint (graph1);
        repaint (graph2);
    }
    
    void mouseDrag (const MouseEvent& e) override
    {
        //DBG("mouse drag...");
        auto posX = (e.position.getX() - graph1.getX()) / graph1.getWidth();
        auto posY = e.position.getY();
        
        Slider* delaySlider           = getSlider("delaySlider");
        Slider* qSlider               = getSlider("qSlider");
        Slider* xfeedQSlider          = getSlider("xfeedQSlider");
        Slider* frequencySlider       = getSlider("frequencySlider");
        Slider* xfeedFrequencySlider  = getSlider("xfeedFrequencySlider");
        Slider* separationSlider      = getSlider("separationSlider");
        Slider* xfeedSeparationSlider = getSlider("xfeedSeparationSlider");
        Slider* directGainSlider      = getSlider("directGainSlider");
        Slider* xfeedGainSlider       = getSlider("xfeedGainSlider");

        if (draggingFreq)
        {
            if (frequencySlider != nullptr)
            {
                frequencySlider->setValue(getFrequencyForPosition(posX));
            }
        }
        if (draggingXfeedFreq)
        {
            if (xfeedFrequencySlider != nullptr)
            {
                xfeedFrequencySlider->setValue(getFrequencyForPosition(posX));
            }
        }
        if (draggingQ)
        {
            if (qSlider != nullptr)
            {
                auto posFreq     = getPositionForFrequency (float (freqs[0]));
                // If adjusting direct Q from its low limit
                if (posX < posFreq)
                {
                    qSlider->setValue((1.0 - qGraphicalAdjustmentFactor) * (0.5) * (freqs[0] / (freqs[0] - getFrequencyForPosition(posX))));
                }
                // if adjusting direct Q from its high limit
                else if (posFreq < posX)
                {
                    qSlider->setValue((1.0 + qGraphicalAdjustmentFactor) * (0.5) * (freqs[0] / (getFrequencyForPosition(posX) - freqs[0])));
                }
            }
        }
        if (draggingXfeedQ)
        {
            if (xfeedQSlider != nullptr)
            {
                auto posXfeedFreq     = getPositionForFrequency (float (freqs[0]));
                // If adjusting xfeed Q from its low limit
                if (posX < posXfeedFreq)
                {
                    xfeedQSlider->setValue((1.0 - qGraphicalAdjustmentFactor) * (0.5) * (freqs[1] / (freqs[1] - getFrequencyForPosition(posX))));
                }
                // if adjusting xfeed Q from its high limit
                else if (posXfeedFreq < posX)
                {
                    xfeedQSlider->setValue((1.0 + qGraphicalAdjustmentFactor) * (0.5) * (freqs[1] / (getFrequencyForPosition(posX) - freqs[1])));
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
                    xfeedSeparationSlider->setValue( - Decibels::gainToDecibels(getGainForPosition(posY, graph1.getY(), graph1.getBottom()) / Decibels::decibelsToGain(xfeedGainSlider->getValue())));
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
        if (draggingMinDB)
        {
            auto newMinDB = maxdB - ((graph1DBRange) / ((graph1.getY() - posY)/(graph1.getY() - mouseDownY)));
            if (newMinDB >= minDBEditor.getRange().getStart())
                minDBEditor.setValue(newMinDB);
        }
        if (draggingMaxDb)
        {
            auto newMaxDB = mindB + ((graph1DBRange) / ((posY - graph1.getBottom())/(mouseDownY - graph1.getBottom())));
            if (newMaxDB <= maxDBEditor.getRange().getEnd())
                maxDBEditor.setValue(newMaxDB);
        }
        if (draggingMinPhase)
        {
            auto newMinPhase = maxPhase - ((graph2PhaseRange) / ((graph2.getY() - posY)/(graph2.getY() - mouseDownY)));
            if (newMinPhase >= minPhaseEditor.getRange().getStart())
                minPhaseEditor.setValue(newMinPhase);
        }
        if (draggingMaxPhase)
        {
            auto newMaxPhase = minPhase + ((graph2PhaseRange) / ((posY - graph2.getBottom())/(mouseDownY -graph2.getBottom())));
            if (newMaxPhase <= maxPhaseEditor.getRange().getEnd())
                maxPhaseEditor.setValue(newMaxPhase);
        }
    }
    
    void mouseDoubleClick (const MouseEvent& e) override
    {
        //DBG("Mouse double clic...");
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
    
    bool isOnPositiveRange(const MouseEvent& e)
    {
        if (graph1.contains(e.x, e.y))
        {
            float positionSearched = e.position.getY();
            float zeroPosition = getPositionForGain(Decibels::decibelsToGain(0.0f), graph1.getY(), graph1.getBottom());
            if (zeroPosition - positionSearched > clickRadius)
                return true;
            else
                return false;
        }
        else if (graph2.contains(e.x, e.y))
        {
            float positionSearched = e.position.getY();
            float zeroPosition = getPositionForPhase(0.0f, graph2.getY(), graph2.getBottom());
            if (zeroPosition - positionSearched > clickRadius)
                return true;
            else
                return false;
        }
        return false;
    }

    bool isOnNegativeRange(const MouseEvent& e)
    {
        if (graph1.contains(e.x, e.y))
        {
            float positionSearched = e.position.getY();
            float zeroPosition = getPositionForGain(Decibels::decibelsToGain(0.0f), graph1.getY(), graph1.getBottom());
            if (positionSearched - zeroPosition > clickRadius)
                return true;
            else
                return false;
        }
        else if (graph2.contains(e.x, e.y))
        {
            float positionSearched = e.position.getY();
            float zeroPosition = getPositionForPhase(0.0f, graph2.getY(), graph2.getBottom());
            if (positionSearched - zeroPosition > clickRadius)
                return true;
            else
                return false;
        }
        return false;
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
        return 20.0f * std::pow (10.0f, 3.0f * pos);
    }
    
    float getPositionForFrequency (float freq)
    {
        return (std::log10 (freq / 20.0f) / 3.0f);
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
        if (scopeIndex > 0)
        {
            return jmap(freq, (float)scopeFreq[scopeIndex], (float)scopeFreq[scopeIndex + 1], (float)scopeGain[filterIndex][scopeIndex], (float)scopeGain[filterIndex][scopeIndex + 1]);
        }
        return 0.0f;
    }
    
    float getFilterPhaseForFrequency(float freq, int filterIndex)
    {
        int scopeIndex = getScopeIndexForFrequency(freq);
        if (scopeIndex >0)
        {
            return jmap(freq, (float)scopeFreq[scopeIndex], (float)scopeFreq[scopeIndex + 1], getTimeForPhase((float)scopePhase[filterIndex][scopeIndex], (float)scopeFreq[scopeIndex]), getTimeForPhase((float)scopePhase[filterIndex][scopeIndex + 1], (float)scopeFreq[scopeIndex + 1]));
        }
        return 0.0f;
    }
    
    float getFrequencyForPhase(float phase, int filterIndex)
    {
        //Not the best methodology as more than one
        DBG("Searched phase: " << phase);
        for (int i = 0; i < scopeSize; i++)
        {
            if (String(scopePhase[filterIndex][i]) == String(phase))
            {
                return scopeFreq[i];
            }
            //else
            //    DBG("scopePhase for freq: " << scopePhase[filterIndex][i] << " : " << scopeFreq[i]);
        }
        return 20000.0f;
    }
    
    float getMinTime(int filterIndex)
    {
        float timeCandidate = 10000.0f;
        for (int i = 0; i < scopeSize; ++i)
        {
            float time = getTimeForPhase(scopePhase[filterIndex][i], scopeFreq[i]);
            if (time < timeCandidate)
            {
                timeCandidate = time;
            }
        }
        return timeCandidate;
        
    }
    
    float getMaxTime(int filterIndex)
    {
        float timeCandidate = -10000.0f;
        for (int i = 0; i < scopeSize; ++i)
        {
            float time = getTimeForPhase(scopePhase[filterIndex][i], scopeFreq[i]);
            if (time > timeCandidate)
            {
                timeCandidate = time;
            }
        }
        return timeCandidate;
        
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
        return -1;
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
    float frequencies [4] [26] = {{20.0f, 40.0f, 80.0f, 160.0f, 320.0f, 640.0f, 1280.0f, 2560.0f, 5120.0f, 10240.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f,                                                     22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f},
                                  {25.0f, 50.0f, 100.0f, 250.0f, 500.0f, 1000.0f, 2500.0f, 5000.0f, 10000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f},
                                  {27.5f, 55.0f, 110.0f, 220.0f, 440.0f, 880.0f, 1760.0f, 3520.0f, 7040.0f, 14080.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f},
                                  {30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f, 90.0f, 100.0f, 200.0f, 300.0f, 400.0f, 500.0f, 600.0f, 700.0f, 800.0f, 900.0f, 1000.0f, 2000.0f, 3000.0f, 4000.0f, 5000.0f, 6000.0f, 7000.0f, 8000.0f, 9000.0f, 10000.0f}};

    float mindB        = -10.0f;
    float maxdB        = 10.0f;
    float graph1DBRange = maxdB - mindB;
    int   dBScaleTickNumber = 5;
    
    float minPhase     = -500.0f;
    float maxPhase     = 500.0f;
    float graph2PhaseRange = maxPhase - minPhase;
    int   phaseScaleTickNumber = 9;
    
    float lowFreqForQ  = 20.0f;
    float highFreqForQ = 20000.0f;
    float lowFreqForXQ  = 20.0f;
    float highFreqForXQ = 20000.0f;
    float qGraphicalAdjustmentFactor = 0.4;
    
    float mouseDownX;
    float mouseDownY;

    // Dragging elements in diagrams
    int  clickRadius       = 4;
    bool draggingFreq      = false;
    bool draggingXfeedFreq = false;
    bool draggingQ         = false;
    bool draggingXfeedQ    = false;
    int  draggingCurve     = -1;
    bool draggingGain      = false;
    bool draggingPhase     = false;
    
    // Dragging in diagrams themselves to modify their min and max in y (dB range diagram 1, phase range diagram 2) first
    // TBD: later dragging could modify also the min and max in x (frequency range in both diagram)
    bool draggingMinDB = false;
    bool draggingMaxDb = false;
    bool draggingMinPhase = false;
    bool draggingMaxPhase = false;
    
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
