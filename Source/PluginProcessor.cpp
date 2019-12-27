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
    thisEditor = static_cast<GainSliderAudioProcessorEditor*>(createEditorIfNeeded());
    treeState.addParameterListener(DELAY_ID, this);
    treeState.addParameterListener(FREQ_ID, this);
    treeState.addParameterListener(Q_ID, this);
    treeState.addParameterListener(SEP_ID, this);
    treeState.addParameterListener(DGAIN_ID, this);
    treeState.addParameterListener(XGAIN_ID, this);
    
    treeState.state = ValueTree (JucePlugin_Name);
}

GainSliderAudioProcessor::~GainSliderAudioProcessor()
{
    treeState.removeParameterListener(DELAY_ID, this);
    treeState.removeParameterListener(FREQ_ID, this);
    treeState.removeParameterListener(Q_ID, this);
    treeState.removeParameterListener(SEP_ID, this);
    treeState.removeParameterListener(DGAIN_ID, this);
    treeState.removeParameterListener(XGAIN_ID, this);
}

//==============================================================================

AudioProcessorValueTreeState::ParameterLayout GainSliderAudioProcessor::createParameterLayout()
{
    // method to stuff parameters into the treeState
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    
    auto delayParams = std::make_unique<AudioParameterFloat> (DELAY_ID, DELAY_NAME, NormalisableRange<float> (0.0f, 320.0f), 100.0f, DELAY_NAME,  AudioProcessorParameter::genericParameter, [](float value, int){return String (value, 0);}, nullptr);
    auto freqParams = std::make_unique<AudioParameterFloat> (FREQ_ID, FREQ_NAME, NormalisableRange<float> (300.0f, 1000.0f), 700.0f, FREQ_NAME, AudioProcessorParameter::genericParameter, [](float value, int){return String (value, 2);}, nullptr);
    auto qParams = std::make_unique<AudioParameterFloat> (Q_ID, Q_NAME, NormalisableRange<float> (0.1f, 1.0f), 0.5f, Q_NAME, AudioProcessorParameter::genericParameter, [](float value, int){return String (value, 2);}, nullptr);
    auto sepParams = std::make_unique<AudioParameterFloat> (SEP_ID, SEP_NAME, NormalisableRange<float> (-6.0f, 0.0f), -4.0f, SEP_NAME,
        AudioProcessorParameter::genericParameter, [](float value, int){return String (value, 1);}, nullptr);
    auto dGainParams = std::make_unique<AudioParameterFloat> (DGAIN_ID, DGAIN_NAME, NormalisableRange<float> (-10.0f, 10.0f), 0.0f, DGAIN_NAME, AudioProcessorParameter::genericParameter, [](float value, int){return String (value, 1);}, nullptr);
    auto xGainParams = std::make_unique<AudioParameterFloat> (XGAIN_ID, XGAIN_NAME, NormalisableRange<float> (-10.0f, 10.0f), 0.0f, XGAIN_NAME, AudioProcessorParameter::genericParameter, [](float value, int){return String (value, 1);}, nullptr);
    auto activeParams = std::make_unique<AudioParameterBool> (ACTIVE_ID, ACTIVE_NAME, true);
    auto guiParams = std::make_unique<AudioParameterChoice> (SPECTR_ID, SPECT_NAME, StringArray {"Plugin only", "+ Diagrams", "+ Spectrum", "Full plugin"}, 1);
    auto settingsParams = std::make_unique<AudioParameterChoice> (SETTINGS_ID, SETTINGS_NAME, StringArray {"Full", "Medium", "Light", "Pure Haas", "user"}, 1);
    
    params.push_back(std::move(delayParams));
    params.push_back(std::move(freqParams));
    params.push_back(std::move(qParams));
    params.push_back(std::move(sepParams));
    params.push_back(std::move(dGainParams));
    params.push_back(std::move(xGainParams));
    params.push_back(std::move(activeParams));
    params.push_back(std::move(guiParams));
    params.push_back(std::move(settingsParams));
    
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
    
    iirLowPassFilterDuplicator.reset();
    iirHighPassFilterDuplicator.reset();
    
    updateFilterParameters();
    
    iirLowPassFilterDuplicator.prepare(spec);
    iirHighPassFilterDuplicator.prepare(spec);

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

void GainSliderAudioProcessor::parameterChanged (const String &parameterID, float newValue)
{
    updateFilterParameters();
}

void GainSliderAudioProcessor::updateFilterParameters ()
{
    auto* sliderFreqValue = treeState.getRawParameterValue(FREQ_ID);
    auto* sliderqValue = treeState.getRawParameterValue(Q_ID);
    auto* sliderSepValue = treeState.getRawParameterValue(SEP_ID);

    // Update Crossfeed and Direct IIR filter parameters with new values from GUI
    iirCoefficientsXfeed = *dsp::IIR::Coefficients<float>::makeLowShelf(mSampleRate, *sliderFreqValue, *sliderqValue, Decibels::decibelsToGain(-1.0f * *sliderSepValue));
    iirCoefficientsDirect = *dsp::IIR::Coefficients<float>::makeLowShelf(mSampleRate, *sliderFreqValue, *sliderqValue, Decibels::decibelsToGain(*sliderSepValue));
    
    iirCoefficientsXfeed.getMagnitudeForFrequencyArray(static_cast<GainSliderAudioProcessorEditor*>(thisEditor)->filterGraphics.scopeFreq,                                       static_cast<GainSliderAudioProcessorEditor*>(thisEditor)->filterGraphics.scopeGain[0], static_cast<GainSliderAudioProcessorEditor*>(thisEditor)->filterGraphics.scopeSize, mSampleRate);
    iirCoefficientsXfeed.getPhaseForFrequencyArray(static_cast<GainSliderAudioProcessorEditor*>(thisEditor)->filterGraphics.scopeFreq, static_cast<GainSliderAudioProcessorEditor*>(thisEditor)->filterGraphics.scopePhase[0], static_cast<GainSliderAudioProcessorEditor*>(thisEditor)->filterGraphics.scopeSize, mSampleRate);
    iirCoefficientsDirect.getMagnitudeForFrequencyArray(static_cast<GainSliderAudioProcessorEditor*>(thisEditor)->filterGraphics.scopeFreq, static_cast<GainSliderAudioProcessorEditor*>(thisEditor)->filterGraphics.scopeGain[1], static_cast<GainSliderAudioProcessorEditor*>(thisEditor)->filterGraphics.scopeSize, mSampleRate);
    iirCoefficientsDirect.getPhaseForFrequencyArray(static_cast<GainSliderAudioProcessorEditor*>(thisEditor)->filterGraphics.scopeFreq, static_cast<GainSliderAudioProcessorEditor*>(thisEditor)->filterGraphics.scopePhase[1], static_cast<GainSliderAudioProcessorEditor*>(thisEditor)->filterGraphics.scopeSize, mSampleRate);
    
    *iirLowPassFilterDuplicator.state = iirCoefficientsXfeed;
    *iirHighPassFilterDuplicator.state = iirCoefficientsDirect;
    
    /*
        DBG("Update Reverb parameters");
        // Update Reverb parameters with new values from GUI
        reverbParameters.damping = 0.99f;
        reverbParameters.width = 0.05f;
        reverbParameters.wetLevel = 0.02f;
        reverbParameters.freezeMode = 0.0f;
        reverbParameters.dryLevel = 0.5f;
    reverbFilter.setParameters(reverbParameters);
    */
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
    
    // If filter is activated
    auto* activeState = treeState.getRawParameterValue(ACTIVE_ID);
    if (*activeState == true)
    {
        const int bufferLength = buffer.getNumSamples();
        const int delayBufferLength = mDelayBuffer.getNumSamples();

        // We copy the buffer into the filter buffer
        mFilterBuffer.makeCopyOf(buffer);
        
        // Do the filtering on the filter buffer for the Crossfeed signal
        //updateFilterParameters();
        dsp::AudioBlock<float> block (mFilterBuffer);
        iirLowPassFilterDuplicator.process(dsp::ProcessContextReplacing<float> (block));

        // Adjust gain on the Filter buffer for separation (and for debug too)
        auto* sepGainValue = treeState.getRawParameterValue(SEP_ID);
        auto* xGainValue = treeState.getRawParameterValue(XGAIN_ID);
        mFilterBuffer.applyGain(Decibels::decibelsToGain(2.0 * *sepGainValue) * Decibels::decibelsToGain(*xGainValue));
        
        // We copy the filter buffer into the delay Buffer
        fillDelayBuffer2(bufferLength, delayBufferLength, mFilterBuffer);

        // Do the filtering on the main buffer for the direct signal
        //updateFilterParameters();
        dsp::AudioBlock<float> block2 (buffer);
        iirHighPassFilterDuplicator.process(dsp::ProcessContextReplacing<float> (block2));

        // Gain adjustment to the main signal
        auto* dGainValue = treeState.getRawParameterValue(DGAIN_ID);
        buffer.applyGain(Decibels::decibelsToGain(*dGainValue));

        // We add the delayBuffer to the main buffer
        auto sliderDelayValue = treeState.getRawParameterValue(DELAY_ID);
        addFromDelayBuffer(*sliderDelayValue, buffer, bufferLength, delayBufferLength);

        //Update write position
        mWritePosition += buffer.getNumSamples();
        mWritePosition %= mDelayBuffer.getNumSamples();
    }
        
    
    // If spectrum is displayed, we push the output buffer into its fifo
    auto* spectrumState = treeState.getRawParameterValue(SPECTR_ID);
    if (*spectrumState == 2 or *spectrumState == 3)
    {
        static_cast<GainSliderAudioProcessorEditor*>(thisEditor)->spectrumAnalyser.getNextAudioBlock(buffer);
    }
}

//==============================================================================

//==============================================================================
void GainSliderAudioProcessor::fillDelayBuffer2(const int bufferLength, const int delayBufferLength, AudioBuffer<float>& buffer)
{
    const int bufferRemaining = delayBufferLength - mWritePosition;
    
    //Copy data from main buffer to delay buffer, and cross the channels
    if (bufferLength <= bufferRemaining)
    {
        mDelayBuffer.copyFrom(0, mWritePosition, buffer, 1, 0, bufferLength);
        mDelayBuffer.copyFrom(1, mWritePosition, buffer, 0, 0, bufferLength);
    }
    else
    {
        mDelayBuffer.copyFrom(0, mWritePosition, buffer, 1, 0, bufferRemaining);
        mDelayBuffer.copyFrom(0, 0, buffer, 1, bufferRemaining, bufferLength - bufferRemaining);
        mDelayBuffer.copyFrom(1, mWritePosition, buffer, 0, 0, bufferRemaining);
        mDelayBuffer.copyFrom(1, 0, buffer, 0, bufferRemaining, bufferLength - bufferRemaining);
    }
}

//==============================================================================
void GainSliderAudioProcessor::addFromDelayBuffer(float delayTimeValue, AudioBuffer<float>& buffer, const int bufferLength, const int delayBufferLength)
{
    // delay time is in microseconds!
    int delaySamples = static_cast<int>(round(mSampleRate * delayTimeValue/1000000.0f));
    const int readPosition = static_cast<int>(( delayBufferLength + mWritePosition - delaySamples )) % delayBufferLength;
    const int bufferRemaining = delayBufferLength - readPosition;
    
    //Add data from delay Buffer to main buffer
    if (bufferLength <= bufferRemaining)
    {
        buffer.addFrom(0, 0, mDelayBuffer, 0, readPosition, bufferLength);
        buffer.addFrom(1, 0, mDelayBuffer, 1, readPosition, bufferLength);
    }
    else
    {
        buffer.addFrom(0, 0, mDelayBuffer, 0, readPosition, bufferRemaining);
        buffer.addFrom(1, 0, mDelayBuffer, 1, readPosition, bufferRemaining);
        buffer.addFrom(0, bufferRemaining, mDelayBuffer, 0, 0, bufferLength - bufferRemaining);
        buffer.addFrom(1, bufferRemaining, mDelayBuffer, 1, 0, bufferLength - bufferRemaining);
    }
}


//==============================================================================
bool GainSliderAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* GainSliderAudioProcessor::createEditor()
{
    return new GainSliderAudioProcessorEditor (*this, treeState);
}

//==============================================================================
void GainSliderAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = treeState.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void GainSliderAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (treeState.state.getType()))
            treeState.replaceState (ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GainSliderAudioProcessor();
}
