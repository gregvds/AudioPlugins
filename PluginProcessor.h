/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#define DELAY_ID "delay"
#define DELAY_NAME "Delay"
#define FREQ_ID "freq"
#define FREQ_NAME "Freq"
#define Q_ID "q"
#define Q_NAME "Q"
#define SEP_ID "sep"
#define SEP_NAME "Sep"
#define GAIN_ID "gain"
#define GAIN_NAME "Gain"
#define ACTIVE_ID "active"
#define ACTIVE_NAME "Active"


//==============================================================================
/**
*/
class GainSliderAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    GainSliderAudioProcessor();
    ~GainSliderAudioProcessor();

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
    double mSampleRate {44100};
    
    // Processor duplicators for the filters
    dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>> lowPassFilterDuplicator;
    dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>> highPassFilterDuplicator;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainSliderAudioProcessor)
};
