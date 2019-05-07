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
//Colour directColour = Colour(0xFFe5e27e);
static Colour directColour = Colour(0xFFfbb040);

class FilterGraphics : public Component,
                       public Timer

{
public:
    FilterGraphics()
    {
        startTimerHz (30);
        fillArrays(stepsPerDecuple);
    }
    ~FilterGraphics()
    {
        
    }
    
    enum
    {
        stepsPerDecuple = 20,
        scopeSize = (3 * stepsPerDecuple)
    };
//==============================================================================
    void timerCallback() override
    {
        repaint();
    }

//==============================================================================
    void drawFrame (Graphics& g)
    {
        auto width1  = graph1.getWidth();
        auto height1 = graph1.getHeight();
        auto y2      = graph2.getY();
        auto width2  = graph2.getWidth();
        auto height2 = graph2.getHeight();
        
        for (int i = 1; i < scopeSize; ++i)
        {
            for (int j = 0; j < 2; j++)
            {
                g.setColour (colours[j]);
                // Draw Amp vs freq of filter
                g.drawLine ({ (float)
                    jmap ((float)freq[i - 1], minFreq, maxFreq, 3.0f, (float) width1 + 3.0f),
                    jmap (Decibels::gainToDecibels((float)gain[j][i - 1]), mindB, maxdB, (float) height1 + 6.0f, 6.0f),
                    (float)
                    jmap ((float)freq[i], minFreq, maxFreq, 3.0f, (float) width1 + 3.0f),
                    jmap (Decibels::gainToDecibels((float)gain[j][i]), mindB, maxdB, (float) height1 + 6.0f, 6.0f) });
                // Draw Phase vs freq of filter
                g.drawLine ({ (float)
                    jmap ((float)freq[i - 1], minFreq, maxFreq, 3.0f, (float) width2 + 3.0f),
                    jmap ((float)phase[j][i - 1], minPhase, maxPhase, (float) y2 + height2, y2 + 0.0f),
                    (float)
                    jmap ((float)freq[i], minFreq, maxFreq, 3.0f, (float) width2 + 3.0f),
                    jmap ((float)phase[j][i], minPhase, maxPhase, (float) y2 + height2, y2 + 0.0f) });
            }
        }
    }

    void paint(Graphics& g) override
    {
        drawingArea = getLocalBounds().reduced (3, 3);
        
        graph1 = drawingArea.removeFromTop(drawingArea.getHeight()/2).reduced(0,3);
        graph2 = drawingArea.removeFromTop(drawingArea.getHeight()).reduced(0,3);

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
        for (int i=0; i < dBScale.size(); i++)
        {
            g.setColour (Colours::silver.withAlpha (0.3f));
            auto y = getPositionForDB(dBScale[i], graph1.getY(), graph1.getBottom());
            g.drawHorizontalLine (roundToInt (y), graph1.getX(), graph1.getRight());
            g.setColour (Colours::silver);
            //auto dB = getDBForPosition(y, graph1.getY(), graph1.getBottom());
            g.drawFittedText (String (dBScale[i]) + " dB", graph1.getX() + 3, roundToInt(y + 2), 50, 14, Justification::left, 1);
            
        }
        
        // Horizontal lines for phase reference
        for (int i=0; i < phaseScale.size(); i++)
        {
            g.setColour (Colours::silver.withAlpha (0.3f));
            auto y = getPositionForPhase(phaseScale[i], graph2.getY(), graph2.getBottom());
            g.drawHorizontalLine (roundToInt (y), graph2.getX(), graph2.getRight());
            g.setColour (Colours::silver);
            //auto dB = getDBForPosition(y, graph1.getY(), graph1.getBottom());
            g.drawFittedText (String (phaseScale[i]) + " °", graph2.getX() + 3, roundToInt(y + 2), 50, 14, Justification::left, 1);
        }

        // draw the curves
        drawFrame(g);
       
//==============================================================================
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
            freq[scopeIndex] = computedFreq;
            computedFreq *= pow(10.0f, 1.0 / stepsPerDecuple);
            gain[0][scopeIndex] = 0.0f;
            phase[0][scopeIndex] = 0.0f;
            gain[1][scopeIndex] = 0.0f;
            phase[1][scopeIndex] = 0.0f;
            DBG("freq added: " << freq[scopeIndex]);
            scopeIndex++;
        }
        if (freq[scopeIndex--] < maxFreq)
        {
            freq[scopeIndex] = maxFreq;
            gain[0][scopeIndex] = 0.0f;
            phase[0][scopeIndex] = 0.0f;
            gain[1][scopeIndex] = 0.0f;
            phase[1][scopeIndex] = 0.0f;
            DBG("freq added: " << freq[scopeIndex]);
        }
        scopeIndex = 0;
        
//        DBG("freq size: " << sizeof(freq)/sizeof(double));
//        for (int i=0; i < scopeSize; i++)
//        {
//            DBG("" << freq[i]);
//            DBG("" << gain[i]);
//            DBG("" << phase[i]);
//        }
    }
    
//==============================================================================

    float minFreq = 20.0f;
    float maxFreq = 20000.0f;
    
    float mindB = -10.0f;
    float maxdB = 10.0f;
    
    float maxPhase = MathConstants<float>::twoPi;
    float minPhase = -1.0f * maxPhase;
    
    Array<float> frequencies = {25.0f, 50.0f, 100.0f, 250.0f, 500.0f, 1000.0f, 2500.0f, 5000.0f, 10000.0f};
    Array<float> dBScale = {-6.0f, -3.0f, 0.0f, 3.0f, 6.0f};
    Array<float> phaseScale = {-1.0f * MathConstants<float>::pi, 0.0f, MathConstants<float>::pi};

    Rectangle<int> drawingArea;
    Rectangle<int> graph1;
    Rectangle<int> graph2;
    
    Array<Colour> colours = {xfeedColour, directColour};
    
//==============================================================================
public:
    double freq [scopeSize];
    double gain [2][scopeSize];
    double phase [2][scopeSize];
    
    int scopeIndex = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterGraphics)
};
