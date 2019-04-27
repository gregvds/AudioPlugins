/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GainSliderAudioProcessor::GainSliderAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
                        treeState(*this, nullptr, "PARAMETERS", createParameterLayout()),
                        lowPassFilterDuplicator(dsp::IIR::Coefficients<float>::makeHighShelf(44100, 700.0f, 0.2f, -6.0f)),
                        highPassFilterDuplicator(dsp::IIR::Coefficients<float>::makeLowShelf(44100, 700.0f, 0.2f, -3.0f))

#endif
{
}

GainSliderAudioProcessor::~GainSliderAudioProcessor()
{
}

//==============================================================================

AudioProcessorValueTreeState::ParameterLayout GainSliderAudioProcessor::createParameterLayout()
{
    // method to stuff parameters into the treeState
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    
    auto delayParams = std::make_unique<AudioParameterFloat> (DELAY_ID, DELAY_NAME, NormalisableRange<float> (200.0f, 300.0f), 250.0f, DELAY_NAME,  AudioProcessorParameter::genericParameter, [](float value, int){return String (value, 0);}, nullptr);
    auto freqParams = std::make_unique<AudioParameterFloat> (FREQ_ID, FREQ_NAME, NormalisableRange<float> (400.0f, 1000.0f), 700.0f, FREQ_NAME, AudioProcessorParameter::genericParameter, [](float value, int){return String (value, 2);}, nullptr);
    auto qParams = std::make_unique<AudioParameterFloat> (Q_ID, Q_NAME, NormalisableRange<float> (0.1f, 0.6f), 0.2f, Q_NAME,
        AudioProcessorParameter::genericParameter, [](float value, int){return String (value, 2);}, nullptr);
    auto sepParams = std::make_unique<AudioParameterFloat> (SEP_ID, SEP_NAME, NormalisableRange<float> (-10.0f, 0.0f), -3.0f, SEP_NAME,
        AudioProcessorParameter::genericParameter, [](float value, int){return String (value, 1);}, nullptr);
    auto gainParams = std::make_unique<AudioParameterFloat> (GAIN_ID, GAIN_NAME, NormalisableRange<float> (-48.0f, 0.0f), 0.0f, GAIN_NAME, AudioProcessorParameter::genericParameter, [](float value, int){return String (value, 1);}, nullptr);
    auto activeParams = std::make_unique<AudioParameterBool> (ACTIVE_ID, ACTIVE_NAME, true);
    
    params.push_back(std::move(delayParams));
    params.push_back(std::move(freqParams));
    params.push_back(std::move(qParams));
    params.push_back(std::move(sepParams));
    params.push_back(std::move(gainParams));
    params.push_back(std::move(activeParams));
    
    return {params.begin(), params.end()};
}

//==============================================================================

const String GainSliderAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GainSliderAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GainSliderAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GainSliderAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GainSliderAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GainSliderAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GainSliderAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GainSliderAudioProcessor::setCurrentProgram (int index)
{
}

const String GainSliderAudioProcessor::getProgramName (int index)
{
    return {};
}

void GainSliderAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void GainSliderAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    const int numInputChannels = getTotalNumInputChannels();
    
    // Initialisation of the filter buffer.
    // It has the same size as the buffer itself.
    mFilterBuffer.setSize(numInputChannels, samplesPerBlock, false, true, true);
    mFilterBuffer.clear();
    
    // Initialisation of the delay buffer.
    // this is for 1 second of delay buffer size.
    // This is absolutely enough for our purpose, the psychoacoustic filter
    // requiring usually between 200 to 300 microseconds.
    const int delayBufferSize = sampleRate + samplesPerBlock;
    mDelayBuffer.setSize(numInputChannels, delayBufferSize, false, true, true);
    mDelayBuffer.clear();
    
    mSampleRate = sampleRate;
    
    // Initialisation of the spec for the process duplicator of the filters
    dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    // Initialisation of the process duplicators
    lowPassFilterDuplicator.reset();
    highPassFilterDuplicator.reset();
    updateFilterParameters();
    lowPassFilterDuplicator.prepare(spec);
    highPassFilterDuplicator.prepare(spec);
    
    // Debug
    DBG("delayBufferSize" << delayBufferSize); //44228
    DBG("samplesPerBlock" << samplesPerBlock); //128
    DBG("sampleRate" << sampleRate); //44100
}

void GainSliderAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GainSliderAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GainSliderAudioProcessor::updateFilterParameters ()
{
    auto sliderFreqValue = treeState.getRawParameterValue(FREQ_ID);
    auto sliderQValue = treeState.getRawParameterValue(Q_ID);
    *lowPassFilterDuplicator.state = *dsp::IIR::Coefficients<float>::makeLowPass(mSampleRate, *sliderFreqValue, *sliderQValue);
    *highPassFilterDuplicator.state = *dsp::IIR::Coefficients<float>::makeHighPass(mSampleRate, *sliderFreqValue, *sliderQValue);
}

void GainSliderAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
        //mDelayBuffer.clear(i, 0, mDelayBuffer.getNumSamples());
    }
    
    auto* activeState = treeState.getRawParameterValue(ACTIVE_ID);
    if (*activeState == true)
    {
        updateFilterParameters();
        const int bufferLength = buffer.getNumSamples();
        const int delayBufferLength = mDelayBuffer.getNumSamples();

        // We copy the buffer into the filter buffer
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            const float* bufferChannelData = buffer.getReadPointer(channel);
            mFilterBuffer.copyFrom(channel, 0, bufferChannelData, bufferLength);
        }
        
        // Do the filtering on the filter buffer for the Crossfeed signal
        dsp::AudioBlock<float> block (mFilterBuffer);
        lowPassFilterDuplicator.process(dsp::ProcessContextReplacing<float> (block));
        
        // Adjust gain on the Filter buffer for separation
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* filteredChannelData = mFilterBuffer.getWritePointer (channel);
            auto sepGainValue = treeState.getRawParameterValue(SEP_ID);
            for (int sample = 0; sample < mFilterBuffer.getNumSamples(); ++sample)
            {
                filteredChannelData[sample] = mFilterBuffer.getSample(channel, sample) * Decibels::decibelsToGain(*sepGainValue/2.0);
            }
        }

        // We copy the filter buffer into the delay Buffer
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            // Here we cross the channels with getTotalNumInputChannels()-1-channel 0 -> 1, 1 -> 0!
            const float* filterChannelData = mFilterBuffer.getReadPointer(getTotalNumInputChannels()-1-channel);
            fillDelayBuffer(channel, bufferLength, delayBufferLength, filterChannelData);
        }

        // Do the filtering on the main buffer for the direct signal
        dsp::AudioBlock<float> block2 (buffer);
        highPassFilterDuplicator.process(dsp::ProcessContextReplacing<float> (block2));
        
        // Gain adjustment to the main signal (To be removed in the end? Useful to debug)
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* DirectChannelData = buffer.getWritePointer (channel);
            auto sliderGainValue = treeState.getRawParameterValue(GAIN_ID);
            auto sepGainValue = treeState.getRawParameterValue(SEP_ID);
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                DirectChannelData[sample] = buffer.getSample(channel, sample) * Decibels::decibelsToGain(*sliderGainValue) * Decibels::decibelsToGain(*sepGainValue/-2.0);
         
            }
        }
        
        // We add the delayBuffer to the main buffer
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            const float* delayChannelData = mDelayBuffer.getReadPointer(channel);
            auto sliderDelayValue = treeState.getRawParameterValue(DELAY_ID);
            getFromDelayBuffer(*sliderDelayValue, buffer, channel, bufferLength, delayBufferLength, delayChannelData);
        }

        //Update write position
        mWritePosition += buffer.getNumSamples();
        mWritePosition %= mDelayBuffer.getNumSamples();
    }
}

//==============================================================================



//==============================================================================
void GainSliderAudioProcessor::fillDelayBuffer(int channel, const int bufferLength, const int delayBufferLength, const float* bufferData)
{
    const int bufferRemaining = delayBufferLength - mWritePosition;
    //Copy data from main buffer to delay buffer
    if (bufferLength <= bufferRemaining)
    {
        mDelayBuffer.copyFromWithRamp(channel, mWritePosition, bufferData, bufferLength, 1.0, 1.0);
    }
    else
    {
        mDelayBuffer.copyFromWithRamp(channel, mWritePosition, bufferData, bufferRemaining, 1.0, 1.0);
        mDelayBuffer.copyFromWithRamp(channel, 0, bufferData, bufferLength - bufferRemaining, 1.0, 1.0);
    }
}

//==============================================================================
void GainSliderAudioProcessor::getFromDelayBuffer(float delayTimeValue, AudioBuffer<float>& buffer, int channel, const int bufferLength, const int delayBufferLength, const float* delayBufferData)
{
    // delay time is in microseconds!
    int delaySamples = static_cast<int>(round(mSampleRate * delayTimeValue/1000000.0f));
    const int readPosition = static_cast<int>(( delayBufferLength + mWritePosition - delaySamples )) % delayBufferLength;
    const int bufferRemaining = delayBufferLength - readPosition;
    
    if (bufferLength <= bufferRemaining)
    {
        buffer.addFrom(channel, 0, delayBufferData + readPosition, bufferLength);
    }
    else
    {
        buffer.addFrom(channel, 0, delayBufferData + readPosition, bufferRemaining);
        buffer.addFrom(channel, bufferRemaining, delayBufferData, bufferLength - bufferRemaining);
    }
}

//==============================================================================
bool GainSliderAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* GainSliderAudioProcessor::createEditor()
{
    return new GainSliderAudioProcessorEditor (*this);
}

//==============================================================================
void GainSliderAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GainSliderAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GainSliderAudioProcessor();
}
