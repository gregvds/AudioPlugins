//
//  SpectrumAnalyser.h
//  PAF
//
//  Created by Grégoire Vandenschrick on 02/05/2019.
//  Copyright © 2019 Greg. All rights reserved.
//


#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class SpectrumAnalyser : public AudioAppComponent,
                         private Timer
{
public:
    SpectrumAnalyser()
        : forwardFFT (fftOrder),
          window (fftSize, dsp::WindowingFunction<float>::blackman )
    {
        setOpaque (true);
        setAudioChannels (2, 0);  // we want a couple of input channels but no outputs
        startTimerHz (30);
        setSize (700, 500);
    }
    
    ~SpectrumAnalyser()
    {
        shutdownAudio();
    }

    enum
    {
        fftOrder  = 11,            // [1]
        fftSize   = 1 << fftOrder, // [2]
        scopeSize = 512            // [3]
    };
    
    //==============================================================================
    void prepareToPlay (int, double) override {}
    void releaseResources() override          {}
    
    
    void getNextAudioBlock (const AudioBuffer<float>& buffer)
    {
        if (buffer.getNumChannels() > 0)
        {
            auto* channelData = buffer.getReadPointer (0);
            for (auto i = 0; i < buffer.getNumSamples(); ++i)
                pushNextSampleIntoFifo (channelData[i]);
        }
    }
    
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
   {
//        if (bufferToFill.buffer->getNumChannels() > 0)
//        {
//            auto* channelData = bufferToFill.buffer->getReadPointer (0, bufferToFill.startSample);
//            for (auto i = 0; i < bufferToFill.numSamples; ++i)
//                pushNextSampleIntoFifo (channelData[i]);
//        }
    }
    
    //==============================================================================
    
    void timerCallback() override
    {
        if (nextFFTBlockReady)
        {
            drawNextFrameOfSpectrum();
            nextFFTBlockReady = false;
            repaint();
        }
    }
    
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
