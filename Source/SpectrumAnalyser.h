//
//  SpectrumAnalyser.h
//  PAF
//
//  Created by Grégoire Vandenschrick on 02/05/2019.
//  Copyright © 2019 Greg. All rights reserved.
//


#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

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
    }
    
    ~SpectrumAnalyser()
    {
    }
    
/* In the AnalyserComponent class, start by declaring an enum as a public member to define useful constants for the FFT implementation:
    [1]: The FFT order designates the size of the FFT window and the number of points
         on which it will operate corresponds to 2 to the power of the order.
         In this case, let's use an order of 11 which will produce an FFT with 2 ^ 11 = 2048 points.
    [2]: To calculate the corresponding FFT size, we use the left bit shift operator which produces 2048 as binary number 100000000000.
    [3]: We also set the number of points in the visual representation of the spectrum as a scope size of 512.
*/
    enum
    {
        fftOrder  = 11,            // [1]
        fftSize   = 1 << fftOrder, // [2]
        scopeSize = 512            // [3]
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
        auto mindB = -100.0f;
        auto maxdB =    0.0f;
        for (int i = 0; i < scopeSize; ++i)                        // [3]
        {
            auto skewedProportionX = 1.0f - std::exp (std::log (1.0f - i / (float) scopeSize) * 0.2f);
            auto fftDataIndex = jlimit (0, fftSize / 2, (int) (skewedProportionX * fftSize / 2));
            auto level = jmap (jlimit (mindB, maxdB, Decibels::gainToDecibels (fftData[fftDataIndex])
                                       - Decibels::gainToDecibels ((float) fftSize)),
                               mindB, maxdB, 0.0f, 1.0f);
            scopeData[i] = level;                                  // [4]
        }
    }
/*
 As a final step, the paint() callback will call our helper function drawFrame() whenever a repaint() request has been initiated and the frame can be drawn as follows:
*/
    void drawFrame (Graphics& g)
    {
        for (int i = 1; i < scopeSize; ++i)
        {
            auto width  = getLocalBounds().getWidth();
            auto height = getLocalBounds().getHeight();
            g.drawLine ({ (float) jmap (i - 1, 0, scopeSize - 1, 0, width),
                jmap (scopeData[i - 1], 0.0f, 1.0f, (float) height, 0.0f),
                (float) jmap (i,     0, scopeSize - 1, 0, width),
                jmap (scopeData[i],     0.0f, 1.0f, (float) height, 0.0f) });
        }
    }
/*
 declare private member variables required for the FFT implementation as shown below:
 [4]: Declare a dsp::FFT object to perform the forward FFT on.
 [5]: Also declare a dsp::WindowingFunction object to apply the windowing function on the signal.
 [6]: The fifo float array of size 2048 will contain our incoming audio data in samples.
 [7]: The fftData float array of size 4096 will contain the results of our FFT calculations.
 [8]: This temporary index keeps count of the amount of samples in the fifo.
 [9]: This temporary boolean tells us whether the next FFT block is ready to be rendered.
 [10]: The scopeData float array of size 512 will contain the points to display on the screen.
*/
private:
    
    dsp::FFT forwardFFT;                  // [4]
    dsp::WindowingFunction<float> window; // [5]
    float fifo [fftSize];                 // [6]
    float fftData [2 * fftSize];          // [7]
    int fifoIndex = 0;                    // [8]
    bool nextFFTBlockReady = false;       // [9]
    float scopeData [scopeSize];          // [10]

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumAnalyser)
};
