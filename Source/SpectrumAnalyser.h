//
//  SpectrumAnalyser.h
//  PAF
//
//  Created by Grégoire Vandenschrick on 02/05/2019.
//  Copyright © 2019 Greg. All rights reserved.
//


#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


static Colour outColour       = Colour(0xFFe5e27e);
static Colour leftSpectrumColour      = Colour(0x881E90FF);
static Colour rightSpectrumColour     = Colour(0x88FF8C00);


//==============================================================================
/**
 Classes to contain look and feel customizations for different types of sliders
 */

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
          //leftForwardFFT (fftOrder),
          //rightForwardFFT (fftOrder),
          window (fftSize, dsp::WindowingFunction<float>::rectangular ),
          leftWindow (fftSize, dsp::WindowingFunction<float>::rectangular ),
          rightWindow (fftSize, dsp::WindowingFunction<float>::rectangular )
    {

        startTimerHz (30);
        setOpaque (true);
        drawingArea = getLocalBounds();
        
        // stuff once and for all the scopeDataFreqs references for spectrum plot
        for (int i = 0; i < scopeSize; ++i)
        {
            scopeDataFreqs[i] = fftFrequencyStep * (float) i/var2;
        }
        
        displayDifferenceToggleButton.setName("displayDifferenceToggleButton");
        displayDifferenceToggleButton.setToggleState (false, NotificationType::dontSendNotification);
        displayDifferenceToggleButton.setClickingTogglesState(true);
        addAndMakeVisible(displayDifferenceToggleButton);

        
        // editable fields
        minDBEditor.setRange (-100.0f, -60.0f);
        minDBEditor.setValue(mindB);
        minDBEditor.setNumDecimalPlacesToDisplay(1);
        minDBEditor.setTextValueSuffix(" dB");
        minDBEditor.setTooltip("Min dB displayed");
        minDBEditor.setLookAndFeel(&linearBarLookAndFeel2);
        minDBEditor.onValueChange = [this] {mindB = minDBEditor.getValue(); repaint(); };
        addAndMakeVisible(minDBEditor);

        maxDBEditor.setRange (-20.0f, 0.0f);
        maxDBEditor.setValue(maxdB);
        maxDBEditor.setNumDecimalPlacesToDisplay(1);
        maxDBEditor.setTextValueSuffix(" dB");
        maxDBEditor.setTooltip("Max dB displayed");
        maxDBEditor.setLookAndFeel(&linearBarLookAndFeel2);
        maxDBEditor.onValueChange = [this] {maxdB = maxDBEditor.getValue(); repaint(); };
        addAndMakeVisible(maxDBEditor);

        
        // Combobox for FFT Window type choice
        fftWindowTypeMenu.addItem("hann", 1);
        fftWindowTypeMenu.addItem("hamming", 2);
        fftWindowTypeMenu.addItem("blackman", 3);
        fftWindowTypeMenu.addItem("blackmanHarris", 4);
        fftWindowTypeMenu.addItem("triangular", 5);
        fftWindowTypeMenu.setSelectedId(1);
        fftWindowTypeMenu.setJustificationType(Justification::centred);
        fftWindowTypeMenu.onChange = [this] {repaint(); };
        fftWindowTypeMenu.setTooltip(TRANS ("FFT Window function choice"));
        //addAndMakeVisible(fftWindowTypeMenu);
        
        // Combobox for frequency scale choice
        frequencyScaleTypeMenu.addItem("type 1", 3);
        frequencyScaleTypeMenu.addItem("type 2", 1);
        frequencyScaleTypeMenu.addItem("type 3", 2);
        frequencyScaleTypeMenu.addItem("type 4", 4);
        frequencyScaleTypeMenu.setSelectedId(3);
        frequencyScaleTypeMenu.setJustificationType(Justification::centred);
        frequencyScaleTypeMenu.onChange = [this] {repaint(); };
        frequencyScaleTypeMenu.setTooltip(TRANS ("Frequency scale choice"));
        addAndMakeVisible(frequencyScaleTypeMenu);

        colourModeMenu.addItem("fill", 1);
        colourModeMenu.addItem("gradient", 2);
        colourModeMenu.setSelectedId(2);
        colourModeMenu.setJustificationType(Justification::centred);
        colourModeMenu.onChange = [this] {repaint(); };
        colourModeMenu.setTooltip(TRANS ("Colour mode choice"));
        addAndMakeVisible(colourModeMenu);
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
            //DBG("Number of Channels: " << buffer.getNumChannels());
            auto* leftChannelData  = buffer.getReadPointer(0);
            auto* rightChannelData = buffer.getReadPointer(1);
            for (auto i = 0; i < buffer.getNumSamples(); ++i)
                pushNextSampleIntoFifo (leftChannelData[i],  rightChannelData[i]);
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
    void pushNextSampleIntoFifo (float leftSample, float rightSample) noexcept
    {
        // if the fifo contains enough data, set a flag to say
        // that the next frame should now be rendered..
        if (fifoIndex == fftSize)    // [11]
        {
            if (! nextFFTBlockReady) // [12]
            {
                zeromem (fftData[0], sizeof (fftData[0]));
                zeromem (fftData[1], sizeof (fftData[1]));
                memcpy (fftData[0], fifo[0], sizeof (fifo[0]));
                memcpy (fftData[1], fifo[1], sizeof (fifo[1]));
                nextFFTBlockReady = true;
            }
            fifoIndex = 0;
        }
        int newFifoIndex = fifoIndex++;
        fifo[0][newFifoIndex] = leftSample;  // [12]
        fifo[1][newFifoIndex] = rightSample;  // [12]
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
        // Why on Earth here do I have to apply FIRST Window to fftData[1] and THEN
        // to fftData[0] in order to avoid fftData[1] not to be emptied, I have
        // NO IDEA?!?!
        window.multiplyWithWindowingTable (fftData[0], fftSize);      // [1]
        window.multiplyWithWindowingTable (fftData[1], fftSize);      // [1]
        forwardFFT.performFrequencyOnlyForwardTransform (fftData[0]); // [2]
        forwardFFT.performFrequencyOnlyForwardTransform (fftData[1]); // [2]
        for (int i = 0; i < scopeSize; ++i)                        // [3] from 0 to 1023
        {
            scopeData[0][i] = getLevelFromFFT(getFFTDataIndex(fftSize, i), fftSize, 0);
            scopeData[1][i] = getLevelFromFFT(getFFTDataIndex(fftSize, i), fftSize, 1);
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

        Path leftSpectrum;
        Path rightSpectrum;
        Path diffSpectrum;

        leftSpectrum.startNewSubPath(3.0f, y + height);
        rightSpectrum.startNewSubPath(3.0f, y + height);
        diffSpectrum.startNewSubPath(3.0f, y + height / 2.0f);

        float leftX1;
        float leftY1;
        float rightX1;
        float rightY1;
        float diffX1;
        float diffY1;
        for (int i = 0; i < scopeSize; i++)
        {
            leftX1 = (float) jmap (getPositionForFrequency(scopeDataFreqs[i]), 0.0f,   1.0f,            3.0f, (float) width + 3.0f) + 1.0f;
            leftY1 = jmap (jlimit(0.0f, 1.0f, scopeData[0][i]),     0.0f,   1.0f,                   (float) y + height, y + 0.0f);
            leftSpectrum.lineTo(leftX1, leftY1);
            rightX1 = (float) jmap (getPositionForFrequency(scopeDataFreqs[i]), 0.0f,   1.0f,            3.0f, (float) width + 3.0f) + 1.0f;
            rightY1 = jmap (jlimit(0.0f, 1.0f, scopeData[1][i]),     0.0f,   1.0f,                   (float) y + height, y + 0.0f);
            rightSpectrum.lineTo(rightX1, rightY1);
            diffX1 = (float) jmap (getPositionForFrequency(scopeDataFreqs[i]), 0.0f,   1.0f,            3.0f, (float) width + 3.0f) + 1.0f;
            diffY1 = jmap (jlimit(-1.0f, 1.0f, scopeData[0][i] - scopeData[1][i]),     -1.0f,   1.0f,                   (float) y + height, y + 0.0f);
            diffSpectrum.lineTo(diffX1, diffY1);
        }

        leftSpectrum.lineTo(width + 3.0f, y + height);
        leftSpectrum.closeSubPath();
        rightSpectrum.lineTo(width + 3.0f, y + height);
        rightSpectrum.closeSubPath();
        diffSpectrum.lineTo(width + 3.0f, y + height / 2.0f);
        diffSpectrum.closeSubPath();

        int colourMode = colourModeMenu.getSelectedId();
        int diffActive = displayDifferenceToggleButton.getToggleState();
        if (colourMode == 1)
            g.setColour(leftSpectrumColour);
        else
            g.setGradientFill(ColourGradient(leftSpectrumColour, x, y, Colours::transparentWhite, x, y + height, false));
        g.fillPath (leftSpectrum);

        if (colourMode == 1)
            g.setColour(rightSpectrumColour);
        else
            g.setGradientFill(ColourGradient(rightSpectrumColour, x, y, Colours::transparentWhite, x, y + height, false));
        g.fillPath (rightSpectrum);
        
        if (diffActive == true)
        {
            g.setColour(Colours::silver);
            g.fillPath (diffSpectrum);
        }
    }

    void paint(Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
        
        bounds = getLocalBounds();
        fieldBar = bounds.removeFromBottom(TEXTBOXHEIGHT);
        drawingArea = bounds.reduced (3, 3);
        
        minDBEditor.setBounds(fieldBar.removeFromLeft(TEXTBOXWIDTH).reduced(3,0));
        maxDBEditor.setBounds(fieldBar.removeFromLeft(TEXTBOXWIDTH).reduced(3,0));
        
        //fftWindowTypeMenu.setBounds(fieldBar.removeFromLeft(TEXTBOXWIDTH).reduced(3,0));
        frequencyScaleTypeMenu.setBounds(fieldBar.removeFromLeft(TEXTBOXWIDTH).reduced(3,0));
        colourModeMenu.setBounds(fieldBar.removeFromLeft(TEXTBOXWIDTH).reduced(3,0));
        displayDifferenceToggleButton.setBounds(fieldBar.removeFromRight(TEXTBOXWIDTH));

        drawFrame(g);

        // Silver frame around RT spectrum analysis
        g.setFont (12.0f);
        g.setColour (Colours::silver);
        g.drawRoundedRectangle (drawingArea.toFloat(), 5, 2);
                 
        // Vertical lines for frequency reference
        for (int i=0; i < 26 ; ++i) {
            auto freq = frequencies[frequencyScaleTypeMenu.getSelectedId() - 1][i];
            if (minFreq < freq and freq < maxFreq)
            {
                g.setColour (Colours::silver.withAlpha (0.3f));
                auto x1 = drawingArea.getX() + drawingArea.getWidth() * getPositionForFrequency(freq);
                auto y1 = drawingArea.getY();
                g.drawVerticalLine (roundToInt (x1), drawingArea.getY(), drawingArea.getBottom());
                
                g.setColour (Colours::silver);
                g.addTransform(AffineTransform::rotation(-MathConstants<float>::halfPi, roundToInt (x1 + 25), y1 + 28));
                g.drawFittedText ((freq < 1000) ? String (freq) + " Hz" : String (freq / 1000, 1) + " kHz",
                                  roundToInt (x1), drawingArea.getY() + 3, 50, 15, Justification::right, 1);
                g.addTransform(AffineTransform::rotation(MathConstants<float>::halfPi, roundToInt (x1 + 25), y1 + 28));
            }
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
        
        g.setColour (Colours::silver.withAlpha (0.6f));
        auto y2 = getPositionForDB(-96.0f, drawingArea.getY(), drawingArea.getBottom());
        auto textPosition = roundToInt(y2 + 2);
        if (drawingArea.getY() - textPosition < 2)
        {
            textPosition = roundToInt(y2 - 16);
        }
        if (y2 <= drawingArea.getBottom() - 2)
        {
            g.drawHorizontalLine (roundToInt (y2), drawingArea.getX(), drawingArea.getRight());
            g.setColour (Colours::silver);
            g.drawFittedText (String (-96.0f) + " dB", drawingArea.getX() + 3, textPosition, 50, 14, Justification::left, 1);
        }
    }

    
    int getFFTDataIndex(int fftSize, int i)
    {
        return jlimit (0, fftSize / 2, (int) ((float)i/var3));
    }
    
    float getLevelFromFFT(int fftDataIndex, int fftSize, int channelIndex)
    {
        return jmap (jlimit (mindB,
                             maxdB,
                             Decibels::gainToDecibels (fftData[channelIndex][fftDataIndex])
                             - Decibels::gainToDecibels ((float) fftSize)
                             ),
                     mindB,
                     maxdB,
                     0.0f,
                     1.0f);
    }
    
    float getFrequencyForPosition (float pos)
    {
        return 20.0f * std::pow (10.0f, 3.0f * jlimit(0.0f, 1.0f, pos));
    }
    
    float getPositionForFrequency (float freq)
    {
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
    
    float getPositionForDB (float dB, float top, float bottom)
    {
        return jmap (dB, mindB, maxdB, bottom, top);
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
    //dsp::FFT leftForwardFFT;                  // [4]
    //dsp::FFT rightForwardFFT;                  // [4]
    dsp::WindowingFunction<float> window; // [5]
    dsp::WindowingFunction<float> leftWindow; // [5]
    dsp::WindowingFunction<float> rightWindow; // [5]
    float fifo [2][fftSize];                 // [6]
    float fftData [2][2 * fftSize];          // [7]
    int fifoIndex = 0;                    // [8]
    bool nextFFTBlockReady = false;       // [9]
    float scopeData [2][scopeSize];          // [10]
    float scopeDiffData [scopeSize];
    float scopeDataFreqs [scopeSize];
    float fftFrequencyStep = (float) mSampleRate / ((float) fftSize / 2.0f);
    
    float mindB = -100.0f;
    float maxdB =    0.0f;
    float graph1DBRange = maxdB - mindB;
    
    float diffMinDB = -10.0f;
    float diffMaxDB = 10.0f;
    float graph2DBRange = diffMaxDB - diffMinDB;

    
    float minFreq      = 20.0f;
    float maxFreq      = 20000.0f;
    float frequencies [4] [26] = {{20.0f, 40.0f, 80.0f, 160.0f, 320.0f, 640.0f, 1280.0f, 2560.0f, 5120.0f, 10240.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f,                     22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f},
                                  {25.0f, 50.0f, 100.0f, 250.0f, 500.0f, 1000.0f, 2500.0f, 5000.0f, 10000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f},
                                  {27.5f, 55.0f, 110.0f, 220.0f, 440.0f, 880.0f, 1760.0f, 3520.0f, 7040.0f, 14080.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f, 22000.0f},
                                  {30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f, 90.0f, 100.0f, 200.0f, 300.0f, 400.0f, 500.0f, 600.0f, 700.0f, 800.0f, 900.0f, 1000.0f, 2000.0f, 3000.0f, 4000.0f, 5000.0f, 6000.0f, 7000.0f, 8000.0f, 9000.0f, 10000.0f}};

    Rectangle<int> bounds;
    Rectangle<int> fieldBar;
    Rectangle<int> drawingArea;
    
    LinearBarLookAndFeel2 linearBarLookAndFeel2;
    
    float var1        = 1.0f;
    float var2        = 1.0f;
    float var3        = 1.0f;
    
    ComboBox fftWindowTypeMenu;
    ComboBox frequencyScaleTypeMenu;
    ComboBox colourModeMenu;
    ToggleButton displayDifferenceToggleButton { "L-R diff" };
    
    Slider minDBEditor { Slider::LinearBar, Slider::TextBoxAbove };
    Slider maxDBEditor { Slider::LinearBar, Slider::TextBoxAbove };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumAnalyser)
};
