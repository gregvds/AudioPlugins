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

class FilterGraphics :  public Component,
                        public Timer,
                        public TextEditor::Listener

{
public:
    FilterGraphics()
    {
        startTimerHz (30);
        fillArrays(stepsPerDecuple);
        
        minDBEditor.setInputRestrictions(7,"+-.0123456789");
        maxDBEditor.setInputRestrictions(7,"+-.0123456789");
        minPhaseEditor.setInputRestrictions(7,"+-.0123456789");
        maxPhaseEditor.setInputRestrictions(7,"+-.0123456789");
        
        minDBEditor.setText(static_cast<String>(mindB));
        minDBEditor.addListener(this);
        addAndMakeVisible(minDBEditor);
        
        maxDBEditor.setText(static_cast<String>(maxdB));
        maxDBEditor.addListener(this);
        addAndMakeVisible(maxDBEditor);
        
        minPhaseEditor.setText(static_cast<String>(minPhase));
        minPhaseEditor.addListener(this);
        addAndMakeVisible(minPhaseEditor);
        
        maxPhaseEditor.setText(static_cast<String>(maxPhase));
        maxPhaseEditor.addListener(this);
        addAndMakeVisible(maxPhaseEditor);

    }
    ~FilterGraphics()
    {
        
    }
    
    enum
    {
        stepsPerDecuple = 50,
        scopeSize = (3 * stepsPerDecuple),
        TEXTBOXHEIGT = 20,
        TEXTBOXWIDTH = 100
    };
//==============================================================================
    void timerCallback() override
    {
        repaint();
    }

    
    void textEditorFocusLost(TextEditor &textEditor) override
    {
        float changedValue = textEditor.getText().getFloatValue();
        
        DBG("Change into a text editor: " << changedValue);
        String textEditorName = textEditor.getName();
        if (textEditorName == "mindB")
        {
            mindB = changedValue;
        }
        else if (textEditorName == "maxdB")
        {
            maxdB = changedValue;
        }
        else if (textEditorName == "minPhase")
        {
            minPhase = changedValue;
        }
        else if (textEditorName == "maxPhase")
        {
            maxPhase = changedValue;
        }
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
        fieldBar = bounds.removeFromTop(TEXTBOXHEIGT);
        drawingArea = bounds.reduced (3, 0);
        
        graph1 = drawingArea.removeFromTop(drawingArea.getHeight()/2).reduced(0,3);
        graph2 = drawingArea.removeFromTop(drawingArea.getHeight()).reduced(0,3);
        
        minDBEditor.setBounds(fieldBar.removeFromLeft(TEXTBOXWIDTH).reduced(3,0));
        maxDBEditor.setBounds(fieldBar.removeFromLeft(TEXTBOXWIDTH).reduced(3,0));
        minPhaseEditor.setBounds(fieldBar.removeFromLeft(TEXTBOXWIDTH).reduced(3,0));
        maxPhaseEditor.setBounds(fieldBar.removeFromLeft(TEXTBOXWIDTH).reduced(3,0));

        // Silver frame around RT spectrum analysis
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
                              roundToInt (x1 + 3), graph1.getBottom() - 18, 50, 15, Justification::left, 1);
            g.drawFittedText ((freq < 1000) ? String (freq) + " Hz" : String (freq / 1000, 1) + " kHz",
                              roundToInt (x2 + 3), graph2.getBottom() - 18, 50, 15, Justification::left, 1);
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
        g.drawFittedText ("0.0 dB", graph1.getX() + 3, roundToInt(y + 2), 50, 14, Justification::left, 1);
        
        // Horizontal lines for phase reference
        for (int i=0; i < phaseScale.size(); i++)
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
       
//==============================================================================
    }
    
    float getTimeForPhase(float phase, float freq)
    {
        return (-phase / MathConstants<float>::twoPi) * (1000000.0f / freq);
    }
    
    float getPositionForPhase(float phase, float top, float bottom)
    {
        return jmap(phase, minPhase, maxPhase, bottom, top);
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


//==============================================================================
private:
    void fillArrays(int stepsPerDecuple)
    {
        float computedFreq = minFreq;
        //int i = 0;
        while (computedFreq <= maxFreq)
        {
            scopeFreq[scopeIndex] = computedFreq;
            computedFreq *= pow(10.0f, 1.0 / stepsPerDecuple);
            scopeGain[0][scopeIndex] = 0.0f;
            scopePhase[0][scopeIndex] = 0.0f;
            scopeGain[1][scopeIndex] = 0.0f;
            scopePhase[1][scopeIndex] = 0.0f;
            //DBG("freq added: " << freq[scopeIndex]);
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

    float minFreq = 20.0f;
    float maxFreq = 20000.0f;
    Array<float> frequencies = {25.0f, 50.0f, 100.0f, 250.0f, 500.0f, 1000.0f, 2500.0f, 5000.0f, 10000.0f};

    float mindB = -10.0f;
    float maxdB = 10.0f;
    Array<float> dBScale = {-6.0f, -3.0f, 0.0f, 3.0f, 6.0f};
    int dBScaleTickNumber = 5;
    
    float minPhase = -500.0f;
    float maxPhase = 500.0f;
    Array<float> phaseScale = {minPhase/5, minPhase*2/5, minPhase*3/5, minPhase*4/5, 0.0f, maxPhase/5, maxPhase*2/5, maxPhase*3/5, maxPhase*4/5};
    int phaseScaleTickNumber = 9;

    
    Rectangle<int> bounds;
    Rectangle<int> fieldBar;
    Rectangle<int> drawingArea;
    Rectangle<int> graph1;
    Rectangle<int> graph2;
    
    TextEditor minDBEditor {"mindB"};
    TextEditor maxDBEditor {"maxdB"};
    TextEditor minPhaseEditor {"minPhase"};
    TextEditor maxPhaseEditor {"maxPhase"};
    
    Array<Colour> colours = {xfeedColour, directColour, freqColour};
    
//==============================================================================
public:

    double scopeFreq [scopeSize];
    double scopeGain [2][scopeSize];
    double scopePhase [2][scopeSize];
    
    float gains [2] = {0.0f, 0.0f};
    float phases [2] = {0.0f, 0.0f};
    float freqs [2] = {0.0f, 0.0f};
    
    int scopeIndex = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterGraphics)
};
