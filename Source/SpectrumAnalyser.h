//
//  SpectrumAnalyser.h
//  PAF
//
//  Created by Grégoire Vandenschrick on 02/05/2019.
//  Copyright © 2019 Greg. All rights reserved.
//


#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


static Colour outColour = Colour(0xFFe5e27e);

//==============================================================================
/**
 Classes to contain look and feel customizations for different types of sliders
 */
/*
class LinearBarLookAndFeel2 : public LookAndFeel_V4,
                             public Component
{
public:
    LinearBarLookAndFeel2()
    {
        LookAndFeel_V4::setColour(Slider::trackColourId, getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
        LookAndFeel_V4::setColour(Slider::textBoxTextColourId, Colours::silver);
    }
};
*/
//==============================================================================
/**
 */

class SpectrumAnalyser : public Component,
                         private Timer
{
public:
    /*
     initialise these variables in the member initialisation list of our constructor like so:
    */
    SpectrumAnalyser()
        : forwardFFT (fftOrder),
          window (fftSize, dsp::WindowingFunction<float>::blackman )
    {

        startTimerHz (30);
        setOpaque (true);
        drawingArea = getLocalBounds();
        
        // Combobox for frequency scale choice
        fftWindowTypeMenu.addItem("hann", 1);
        fftWindowTypeMenu.addItem("hamming", 2);
        fftWindowTypeMenu.addItem("blackman", 3);
        fftWindowTypeMenu.addItem("blackmanHarris", 4);
        fftWindowTypeMenu.addItem("triangular", 5);
        fftWindowTypeMenu.setSelectedId(1);
        fftWindowTypeMenu.setJustificationType(Justification::centred);
        //frequencyScaleTypeMenu.addListener(this);
        fftWindowTypeMenu.onChange = [this] {repaint(); };
        fftWindowTypeMenu.setTooltip(TRANS ("FFT Window function choice"));
        addAndMakeVisible(fftWindowTypeMenu);
        
/*
        // editable fields
        skewVar1.setRange (0.0f, 1.0f);
        skewVar1.setValue(var1);
        skewVar1.setNumDecimalPlacesToDisplay(3);
        skewVar1.setTextValueSuffix(" bool");
        skewVar1.setTooltip("Skewing variable 1");
        skewVar1.setLookAndFeel(&linearBarLookAndFeel2);
        skewVar1.onValueChange = [this] {var1 = skewVar1.getValue(); };
        addAndMakeVisible(skewVar1);

        // editable fields
        skewVar2.setRange (0.50f, 2.0f);
        skewVar2.setValue(var2);
        skewVar2.setNumDecimalPlacesToDisplay(3);
        skewVar2.setTextValueSuffix(" ");
        skewVar2.setTooltip("Skewing variable 2");
        skewVar2.setLookAndFeel(&linearBarLookAndFeel2);
        skewVar2.onValueChange = [this] {var2 = skewVar2.getValue(); };
        addAndMakeVisible(skewVar2);

        // editable fields
        skewVar3.setRange (0.5f, 2.0f);
        skewVar3.setValue(var3);
        skewVar3.setNumDecimalPlacesToDisplay(3);
        skewVar3.setTextValueSuffix(" ");
        skewVar3.setTooltip("Skewing variable 3");
        skewVar3.setLookAndFeel(&linearBarLookAndFeel2);
        skewVar3.onValueChange = [this] {var3 = skewVar3.getValue(); };
        addAndMakeVisible(skewVar3);
*/
        
        
        
    }
    
    ~SpectrumAnalyser()
    {
    }
    
/* In the AnalyserComponent class, start by declaring an enum as a public member to define useful constants for the FFT implementation:
    [1]: The FFT order designates the size of the FFT window and the number of points
         on which it will operate corresponds to 2 to the power of the order.
         In this case, let's use an order of 11 which will produce an FFT with 2 ^ 11 = 2048 points.
    [2]: To calculate the corresponding FFT size, we use the left bit shift operator which produces 2048 as binary number 100.000.000.000.
    [3]: We also set the number of points in the visual representation of the spectrum as a scope size of 1024.
*/
    enum
    {
        fftOrder  = 12,              // [1]
        fftSize   = 1 << fftOrder,   // [2]
        scopeSize = 2048,            // [3]
        TEXTBOXHEIGHT = 20,
        TEXTBOXWIDTH = 100
    };
    
    //==============================================================================
    //void prepareToPlay (int, double) override {}
    //void releaseResources() override          {}
    
    /*
     we simply push all the samples contained in our current audio buffer block to the fifo to be processed at a later time:
    */
    void getNextAudioBlock (const AudioBuffer<float>& buffer)
    {
        if (buffer.getNumChannels() > 0)
        {
            auto* channelData = buffer.getReadPointer (0);
            for (auto i = 0; i < buffer.getNumSamples(); ++i)
                pushNextSampleIntoFifo (channelData[i]);
        }
    }
    
    //==============================================================================
/*
 Update the analyser using the timer callback function by calling the drawNextFrameOfSpectrum()
 only when the next FFT block is ready, reset the flag and update the GUI using the repaint() function:
*/
    void timerCallback() override
    {
        if (nextFFTBlockReady)
        {
            drawNextFrameOfSpectrum();
            nextFFTBlockReady = false;
            repaint();
        }
    }
/*
 To push the sample into the fifo:
 [11]: If the fifo contains enough data in this case 2048 samples,
        we are ready to copy the data to the fftData array for it to be processed by the FFT.
        We also set a flag to say that the next frame should now be rendered and always reset the index to 0 to start filling the fifo again.
 [12]: Every time this function gets called, a sample is stored in the fifo and the index is incremented.
*/
    void pushNextSampleIntoFifo (float sample) noexcept
    {
        // if the fifo contains enough data, set a flag to say
        // that the next frame should now be rendered..
        if (fifoIndex == fftSize)    // [11]
        {
            if (! nextFFTBlockReady) // [12]
            {
                zeromem (fftData, sizeof (fftData));
                memcpy (fftData, fifo, sizeof (fifo));
                nextFFTBlockReady = true;
            }
            fifoIndex = 0;
        }
        fifo[fifoIndex++] = sample;  // [12]
   }
    
/*
 insert the frame drawing implementation as explained below:
 [1]: First, apply the windowing function to the incoming data by calling the multiplyWithWindowingTable() function on the window object and passing the data as an argument.
 [2]: Then, render the FFT data using the performFrequencyOnlyForwardTransform() function on the FFT object with the fftData array as an argument.
 [3]: Now in the for loop for every point in the scope width, calculate the level proportionally to the desired minimum and maximum decibels.
      To do this, we first need to skew the x-axis to use a logarithmic scale to better represent our frequencies.
      We can then feed this scaling factor to retrieve the correct array index and use the amplitude value to map it to a range between 0.0 .. 1.0.
 [4]: Finally set the appropriate point with the correct amplitude to prepare the drawing process.

*/
    void drawNextFrameOfSpectrum()
    {
        window.multiplyWithWindowingTable (fftData, fftSize);      // [1]
        forwardFFT.performFrequencyOnlyForwardTransform (fftData); // [2]
        //DBG("Frequency step: " << (float) mSampleRate / ((float) fftSize / 2.0f));
        for (int i = 0; i < scopeSize; ++i)                        // [3] from 0 to 1023
        {
            // for these 1024 steps, we need to go and pick data in a log10 way so as it renders correctly on the diagram in the end
            // But we don't know how the frequencies are laid out in the fftData in the first hand...
            auto fftDataIndex = i;
            if (var1 < 0.5f)
            {
                /* This original proposition does not do it properly...
                */
                auto skewedProportionX = 1.0f - std::exp (std::log (1.0f - i / (float) scopeSize) * 0.2f);
                fftDataIndex = jlimit (0,
                                            fftSize / 2,
                                            (int) (skewedProportionX * fftSize / 2));
            }
            else
            {
                /* And a simple geometric proportion does not do it either...
                */
                fftDataIndex = jlimit (0,
                                            fftSize / 2,
                                            (int) ((float)i/var3));
            
            }
            
            auto level = jmap (
                               jlimit (mindB,
                                       maxdB,
                                       Decibels::gainToDecibels (fftData[fftDataIndex])
                                       - Decibels::gainToDecibels ((float) fftSize)
                                       ),
                               mindB,
                               maxdB,
                               0.0f,
                               1.0f);
            scopeData[i] = level;                                  // [4]
            scopeDataFreqs[i] = ((float) mSampleRate / ((float) fftSize / 2.0f) ) * (float) i/var2;
            //DBG("i: " << i);
            //DBG("scopeDataFreqs[i]: " << scopeDataFreqs[i]);
            //DBG("scopeData[i]: " << scopeData[i]);
        }
    }
/*
 As a final step, the paint() callback will call our helper function drawFrame() whenever a repaint() request has been initiated and the frame can be drawn as follows:
*/
    void drawFrame (Graphics& g)
    {
        auto x      = drawingArea.getX();
        auto y      = drawingArea.getY();
        auto width  = drawingArea.getWidth();
        auto height = drawingArea.getHeight();

        g.setGradientFill(ColourGradient(outColour, x, y, Colours::transparentWhite, x, y + height, false));

        Path p;
        p.startNewSubPath(3.0f, y + height);
                
        for (int i = 0; i < scopeSize; i++)
        {
            float x1 = x;
            if (var1 < 0.5f)
            {
                x1 = (float) jmap ((float) i,                                  0.0f, (float) scopeSize, 3.0f, (float) width + 3.0f);
            }
            else
            {
                //DBG("Freq: " << scopeDataFreqs[i]);
                //DBG("Position: " << getPositionForFrequency(scopeDataFreqs[i]));
                x1 = (float) jmap (getPositionForFrequency(scopeDataFreqs[i]), 0.0f,   1.0f,            3.0f, (float) width + 3.0f) + 1.0f;
                //DBG("x1: " << x1);
            }
            //DBG("x1: " << x1);
            float y1 = jmap (jlimit(0.0f, 1.0f, scopeData[i]),     0.0f,   1.0f,                   (float) y + height, y + 0.0f);
            p.lineTo(x1, y1);
        }
        p.lineTo(width + 3.0f, y + height);
        p.closeSubPath();
        g.fillPath (p);
    }

    void paint(Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
        
        bounds = getLocalBounds();
        fieldBar = bounds.removeFromBottom(TEXTBOXHEIGHT);
        drawingArea = bounds.reduced (3, 3);
        
        fftWindowTypeMenu.setBounds(fieldBar.removeFromLeft(TEXTBOXWIDTH).reduced(3,0));
        
        /*
        skewVar1.setBounds(fieldBar.removeFromRight(TEXTBOXWIDTH).reduced(3,0));
        skewVar2.setBounds(fieldBar.removeFromRight(TEXTBOXWIDTH).reduced(3,0));
        skewVar3.setBounds(fieldBar.removeFromRight(TEXTBOXWIDTH).reduced(3,0));
        */
        drawFrame(g);

        // Silver frame around RT spectrum analysis
        g.setFont (12.0f);
        g.setColour (Colours::silver);
        g.drawRoundedRectangle (drawingArea.toFloat(), 5, 2);
        
        // Vertical lines for frequency reference
        //AffineTransform rotationAntiClockwize = AffineTransform::rotation(-MathConstants<float>::halfPi);
        //rotationAntiClockwize = rotationAntiClockwize.rotation(-MathConstants<float>::halfPi);
        //AffineTransform rotationClockwize = AffineTransform::rotation(MathConstants<float>::halfPi);
        //rotationClockwize = rotationClockwize.rotation(MathConstants<float>::halfPi);
        for (int i=0; i < frequencies.size() ; ++i) {
            g.setColour (Colours::silver.withAlpha (0.3f));
            auto freq = frequencies[i];
            auto x = drawingArea.getX() + drawingArea.getWidth() * getPositionForFrequency(freq);
            auto y = drawingArea.getY();
            g.drawVerticalLine (roundToInt (x), drawingArea.getY(), drawingArea.getBottom());
            g.setColour (Colours::silver);
            g.addTransform(AffineTransform::rotation(-MathConstants<float>::halfPi, roundToInt (x + 25), y + 28));
            g.drawFittedText ((freq < 1000) ? String (freq) + " Hz" : String (freq / 1000, 1) + " kHz", roundToInt (x), y + 3, 50, 15, Justification::right, 1);
            g.addTransform(AffineTransform::rotation(MathConstants<float>::halfPi, roundToInt (x + 25), y + 28));
        }
        // Horizontal lines for dB reference
        for (int i=0; i < 5; i++)
        {
            g.setColour (Colours::silver.withAlpha (0.3f));
            auto y = drawingArea.getY() + drawingArea.getHeight() * i * 0.2f;
            g.drawHorizontalLine (roundToInt (y), drawingArea.getX(), drawingArea.getRight());
            g.setColour (Colours::silver);
            auto dB = getDBForPosition(y, drawingArea.getY(), drawingArea.getBottom());
            g.drawFittedText (String (dB) + " dB", drawingArea.getX() + 3, roundToInt(y + 2), 50, 14, Justification::left, 1);
        }
        
    }

    
    float getFrequencyForPosition (float pos)
    {
        //return 20.0f * std::pow (2.0f, pos * 10.0f);
        return 20.0f * std::pow (10.0f, 3.0f * jlimit(0.0f, 1.0f, pos));
    }
    
    float getPositionForFrequency (float freq)
    {
        //return (std::log (freq / 20.0f) / std::log (2.0f)) / 10.0f;
        return (std::log10 (jlimit(20.0f, 20000.0f, freq) / 20.0f) / 3.0f);
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
    
/*
 declare private member variables required for the FFT implementation as shown below:
 [4]: Declare a dsp::FFT object to perform the forward FFT on.
 [5]: Also declare a dsp::WindowingFunction object to apply the windowing function on the signal.
 [6]: The fifo float array of size 2048 will contain our incoming audio data in samples.
 [7]: The fftData float array of size 4096 will contain the results of our FFT calculations.
 [8]: This temporary index keeps count of the amount of samples in the fifo.
 [9]: This temporary boolean tells us whether the next FFT block is ready to be rendered.
 [10]: The scopeData float array of size scopeSize will contain the points to display on the screen.
*/
private:
    
    double mSampleRate { 44100 };
    
    dsp::FFT forwardFFT;                  // [4]
    dsp::WindowingFunction<float> window; // [5]
    float fifo [fftSize];                 // [6]
    float fftData [2 * fftSize];          // [7]
    int fifoIndex = 0;                    // [8]
    bool nextFFTBlockReady = false;       // [9]
    float scopeData [scopeSize];          // [10]
    float scopeDataFreqs [scopeSize];
    
    float mindB = -100.0f;
    float maxdB =    0.0f;
    
    Array<float> frequencies = {30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f, 90.0f, 100.0f, 200.0f, 300.0f, 400.0f, 500.0f, 600.0f, 700.0f, 800.0f, 900.0f, 1000.0f, 2000.0f, 3000.0f, 4000.0f, 5000.0f, 6000.0f, 7000.0f, 8000.0f, 9000.0f, 10000.0f};
    Rectangle<int> bounds;
    Rectangle<int> fieldBar;
    Rectangle<int> drawingArea;
    
    float var1        = 1.0f;
    float var2        = 2.0f;
    float var3        = 1.0f;
    /*
    Array<dsp::WindowingFunction<float>> fftWindowTypeArray = {dsp::WindowingFunction<float>::hann,
                                                                         dsp::WindowingFunction<float>::hamming,
                                                                         dsp::WindowingFunction<float>::blackman,
                                                                         dsp::WindowingFunction<float>::blackmanHarris,
                                                                         dsp::WindowingFunction<float>::triangular};
     */
    //LinearBarLookAndFeel2 linearBarLookAndFeel2;
    
    ComboBox fftWindowTypeMenu;
/*
    Slider skewVar1 { Slider::LinearBar, Slider::TextBoxAbove };
    Slider skewVar2 { Slider::LinearBar, Slider::TextBoxAbove };
    Slider skewVar3 { Slider::LinearBar, Slider::TextBoxAbove };
*/
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumAnalyser)
};
