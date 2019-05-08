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
                        treeState(*this, nullptr, "PARAMETERS", createParameterLayout())

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
    
    auto delayParams = std::make_unique<AudioParameterFloat> (DELAY_ID, DELAY_NAME, NormalisableRange<float> (0.0f, 320.0f), 100.0f, DELAY_NAME,  AudioProcessorParameter::genericParameter, [](float value, int){return String (value, 0);}, nullptr);
    auto freqParams = std::make_unique<AudioParameterFloat> (FREQ_ID, FREQ_NAME, NormalisableRange<float> (400.0f, 1000.0f), 700.0f, FREQ_NAME, AudioProcessorParameter::genericParameter, [](float value, int){return String (value, 2);}, nullptr);
    auto qParams = std::make_unique<AudioParameterFloat> (Q_ID, Q_NAME, NormalisableRange<float> (0.1f, 1.0f), 0.5f, Q_NAME, AudioProcessorParameter::genericParameter, [](float value, int){return String (value, 2);}, nullptr);
    auto sepParams = std::make_unique<AudioParameterFloat> (SEP_ID, SEP_NAME, NormalisableRange<float> (-6.0f, 0.0f), -4.0f, SEP_NAME,
        AudioProcessorParameter::genericParameter, [](float value, int){return String (value, 1);}, nullptr);
    auto dGainParams = std::make_unique<AudioParameterFloat> (DGAIN_ID, DGAIN_NAME, NormalisableRange<float> (-10.0f, 10.0f), 0.0f, DGAIN_NAME, AudioProcessorParameter::genericParameter, [](float value, int){return String (value, 1);}, nullptr);
    auto xGainParams = std::make_unique<AudioParameterFloat> (XGAIN_ID, XGAIN_NAME, NormalisableRange<float> (-10.0f, 10.0f), 0.0f, XGAIN_NAME, AudioProcessorParameter::genericParameter, [](float value, int){return String (value, 1);}, nullptr);
    auto activeParams = std::make_unique<AudioParameterBool> (ACTIVE_ID, ACTIVE_NAME, true);
    auto spectrumParams = std::make_unique<AudioParameterBool> (SPECTR_ID, SPECT_NAME, true);
    auto typeParams = std::make_unique<AudioParameterChoice> (TYPE_ID, TYPE_NAME, StringArray {"Shelf", "Pass", "None"}, 0);
    
    params.push_back(std::move(delayParams));
    params.push_back(std::move(freqParams));
    params.push_back(std::move(qParams));
    params.push_back(std::move(sepParams));
    params.push_back(std::move(dGainParams));
    params.push_back(std::move(xGainParams));
    params.push_back(std::move(activeParams));
    params.push_back(std::move(spectrumParams));
    params.push_back(std::move(typeParams));
    
    
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
    mSamplesPerBlock = samplesPerBlock;
    
    // Initialisation of the spec for the process duplicators of the filters
    dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    // Initialisation of the process duplicators
    iirLowPassFilterDuplicator.reset();
    iirHighPassFilterDuplicator.reset();
    
    updateFilterParameters();
    
    iirLowPassFilterDuplicator.prepare(spec);
    iirHighPassFilterDuplicator.prepare(spec);
    
    // Debug
    /*
    DBG("delayBufferSize: " << delayBufferSize); //44228
    DBG("samplesPerBlock: " << samplesPerBlock); //128
    DBG("sampleRate: " << sampleRate); //44100
     */
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
    auto* delayValue = treeState.getRawParameterValue(DELAY_ID);
    auto* sliderFreqValue = treeState.getRawParameterValue(FREQ_ID);
    auto* sliderqValue = treeState.getRawParameterValue(Q_ID);
    auto* sliderSepValue = treeState.getRawParameterValue(SEP_ID);
    auto* xGainValue = treeState.getRawParameterValue(XGAIN_ID);
    auto* dGainValue = treeState.getRawParameterValue(DGAIN_ID);
    auto* filterType = treeState.getRawParameterValue(TYPE_ID);
    
    if (*filterType == 0)
    {
        //DBG("Frequency value: " << *sliderFreqValue);
        iirCoefficientsXfeed = *dsp::IIR::Coefficients<float>::makeLowShelf(mSampleRate, *sliderFreqValue, *sliderqValue, Decibels::decibelsToGain(-1.0f * *sliderSepValue));
        iirCoefficientsDirect = *dsp::IIR::Coefficients<float>::makeLowShelf(mSampleRate, *sliderFreqValue, *sliderqValue, Decibels::decibelsToGain(*sliderSepValue));
    }
    else if (*filterType == 1)
    {
        iirCoefficientsXfeed = *dsp::IIR::Coefficients<float>::makeLowPass(mSampleRate, *sliderFreqValue * 2.0f, *sliderqValue);
        iirCoefficientsDirect = *dsp::IIR::Coefficients<float>::makeHighPass(mSampleRate, *sliderFreqValue / 2.0f, *sliderqValue);
    }
    else if (*filterType == 2)
    {
        // No filtering
    }
    iirCoefficientsXfeed.getMagnitudeForFrequencyArray(filterGraphics.scopeFreq, filterGraphics.scopeGain[0], filterGraphics.scopeSize, mSampleRate);
    iirCoefficientsXfeed.getPhaseForFrequencyArray(filterGraphics.scopeFreq, filterGraphics.scopePhase[0], filterGraphics.scopeSize, mSampleRate);
    iirCoefficientsDirect.getMagnitudeForFrequencyArray(filterGraphics.scopeFreq, filterGraphics.scopeGain[1], filterGraphics.scopeSize, mSampleRate);
    iirCoefficientsDirect.getPhaseForFrequencyArray(filterGraphics.scopeFreq, filterGraphics.scopePhase[1], filterGraphics.scopeSize, mSampleRate);
    *iirLowPassFilterDuplicator.state = iirCoefficientsXfeed;
    *iirHighPassFilterDuplicator.state = iirCoefficientsDirect;
    
    filterGraphics.gains[0] = *xGainValue;
    filterGraphics.gains[1] = *dGainValue;
    
    filterGraphics.phases[0] = *delayValue;

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
    }
    
    
    auto* activeState = treeState.getRawParameterValue(ACTIVE_ID);
    updateFilterParameters();
    if (*activeState == true)
    {
        const int bufferLength = buffer.getNumSamples();
        const int delayBufferLength = mDelayBuffer.getNumSamples();

        // We copy the buffer into the filter buffer
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            const float* bufferChannelData = buffer.getReadPointer(channel);
            mFilterBuffer.copyFrom(channel, 0, bufferChannelData, bufferLength);
        }

        // Do the filtering on the filter buffer for the Crossfeed signal
        //updateFilterParameters();
        dsp::AudioBlock<float> block (mFilterBuffer);
        iirLowPassFilterDuplicator.process(dsp::ProcessContextReplacing<float> (block));

        // Adjust gain on the Filter buffer for separation (and for debug too)
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* filteredChannelData = mFilterBuffer.getWritePointer (channel);
            auto* sepGainValue = treeState.getRawParameterValue(SEP_ID);
            auto* xGainValue = treeState.getRawParameterValue(XGAIN_ID);
            for (int sample = 0; sample < mFilterBuffer.getNumSamples(); ++sample)
            {
                filteredChannelData[sample] = mFilterBuffer.getSample(channel, sample) * Decibels::decibelsToGain(2.0 * *sepGainValue) * Decibels::decibelsToGain(*xGainValue);
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
        //updateFilterParameters();
        dsp::AudioBlock<float> block2 (buffer);
        iirHighPassFilterDuplicator.process(dsp::ProcessContextReplacing<float> (block2));

        // Gain adjustment to the main signal (To be removed in the end? Useful to debug)
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* DirectChannelData = buffer.getWritePointer (channel);
            auto* dGainValue = treeState.getRawParameterValue(DGAIN_ID);
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                DirectChannelData[sample] = buffer.getSample(channel, sample) * Decibels::decibelsToGain(*dGainValue);
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
    auto* spectrumState = treeState.getRawParameterValue(SPECTR_ID);
    if (*spectrumState == true)
    {
        spectrumAnalyser.getNextAudioBlock(buffer);
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
    /*
    MemoryOutputStream stream(destData, false);
    treeState.state.writeToStream (stream);
    */
}

void GainSliderAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    /*
    ValueTree tree = ValueTree::readFromData (data, size_t (sizeInBytes));
    if (tree.isValid())
    {
        treeState.state = tree;
    }
    */
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GainSliderAudioProcessor();
}
