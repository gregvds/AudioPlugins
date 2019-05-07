/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SpectrumAnalyser.h"
#include "FilterGraphics.h"

#define DELAY_ID "delay"
#define DELAY_NAME "Delay"
#define FREQ_ID "freq"
#define FREQ_NAME "Freq"
#define SEP_ID "sep"
#define SEP_NAME "Sep"
#define DGAIN_ID "dgain"
#define DGAIN_NAME "Dgain"
#define XGAIN_ID "xgain"
#define XGAIN_NAME "Xgain"
#define ACTIVE_ID "active"
#define ACTIVE_NAME "Active"
#define SPECTR_ID "spectrum"
#define SPECT_NAME "Spectrum"
#define TYPE_ID "type"
#define TYPE_NAME "Type"



//==============================================================================
/**
*/
class GainSliderAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    GainSliderAudioProcessor();
    ~GainSliderAudioProcessor();

    SpectrumAnalyser spectrumAnalyser;
    FilterGraphics filterGraphics;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    void fillDelayBuffer(int channel, const int bufferLength, const int delayBufferLength, const float* bufferData);
    void getFromDelayBuffer(float delayTimeValue, AudioBuffer<float>& buffer, int channel, const int bufferLength, const int delayBufferLength, const float* delayBufferData);
    void updateFilterParameters ();
    
    //==============================================================================
    AudioProcessorValueTreeState treeState;
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
private:
    // Audio buffer for filtering the crossfeed signal only
    AudioBuffer<float> mFilterBuffer;
    int mFilterBufferSize { 0 };
    
    // Audio buffer for the delay
    AudioBuffer<float> mDelayBuffer;
    
    // Variables useful for the delay buffer
    int mWritePosition { 0 };
    int mSamplesPerBlock { 512 };
    double mSampleRate { 44100 };
    
    // Useful to get magnitude and phase of the filters
    dsp::IIR::Coefficients<float> iirCoefficientsXfeed;
    dsp::IIR::Coefficients<float> iirCoefficientsDirect;
    
    // Processor duplicators for the filters
    dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>> iirLowPassFilterDuplicator;
    dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>> iirHighPassFilterDuplicator;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainSliderAudioProcessor)
};
