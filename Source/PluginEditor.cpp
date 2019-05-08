/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
GainSliderAudioProcessorEditor::GainSliderAudioProcessorEditor (GainSliderAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    tooltipWindow->setMillisecondsBeforeTipAppears (500);
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    // Size is dynamic regarding the number of objects
    // setResizable (true, true);
    setSize (4* DIALSIZE + SPECTRUMWIDTH, 3*(DIALSIZE + TEXTBOXHEIGT + LABELHEIGHT) + TEXTBOXHEIGT + SPECTRUMHEIGHT);
    
    delaySliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, DELAY_ID, delaySlider);
    freqSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, FREQ_ID, frequencySlider);
    qSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, Q_ID, qSlider);
    sepSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, SEP_ID, separationSlider);
    directGainSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, DGAIN_ID, directGainSlider);
    xfeedGainSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, XGAIN_ID, xfeedGainSlider);
    activeStateToggleButtonAttach = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(processor.treeState, ACTIVE_ID, activeStateToggleButton);
    spectrumToggleButtonAttach = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(processor.treeState, SPECTR_ID, spectrumAnalyserToggleButton);
    filterTypeMenuAttach = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(processor.treeState, TYPE_ID, filterTypeMenu);

    crossFeedMenu.addItem("Full", 1);
    crossFeedMenu.addItem("Medium", 2);
    crossFeedMenu.addItem("Light", 3);
    crossFeedMenu.setSelectedId(2);
    crossFeedMenu.setJustificationType(Justification::centred);
    crossFeedMenu.addListener(this);
    crossFeedMenu.setTooltip(TRANS ("Global intensity settings"));
    addAndMakeVisible(crossFeedMenu);
    
    filterTypeMenu.addItem ("Shelf", 1);
    filterTypeMenu.addItem("Pass", 2);
    filterTypeMenu.addItem("None", 3);
    filterTypeMenu.setSelectedId(1);
    filterTypeMenu.setJustificationType(Justification::centred);
    filterTypeMenu.addListener(this);
    filterTypeMenu.setTooltip(TRANS ("Filter types"));
    addAndMakeVisible(filterTypeMenu);
    
    delaySlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    delaySlider.setTextBoxStyle(Slider::TextBoxBelow, false, TEXTBOXWIDTH, TEXTBOXHEIGT);
    delaySlider.setTextValueSuffix(" microsec");
    delaySlider.setRange(0.0f, 320.0f);
    delaySlider.setValue(100.0f);
    delaySlider.setSkewFactorFromMidPoint(50.0f);
    delaySlider.addListener(this);
    delaySlider.setTooltip(TRANS ("Delay time"));
    addAndMakeVisible(delaySlider);
    delayLabel.setText("Crossfeed delay", NotificationType::dontSendNotification);
    delayLabel.setLookAndFeel(&labelLookAndFeel);
    delayLabel.attachToComponent(&delaySlider, false);
    delayLabel.setJustificationType(Justification::centredBottom);
    addAndMakeVisible(delayLabel);
    
    frequencySlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    frequencySlider.setTextBoxStyle(Slider::TextBoxBelow, false, TEXTBOXWIDTH, TEXTBOXHEIGT);
    frequencySlider.setTextValueSuffix(" Hz");
    frequencySlider.setRange(400.0f, 1000.0f);
    frequencySlider.setValue(700.0f);
    frequencySlider.setSkewFactorFromMidPoint(600.0f);
    frequencySlider.addListener(this);
    frequencySlider.setTooltip(TRANS ("Crossfeed cutoff frequency"));
    addAndMakeVisible(frequencySlider);
    frequencyLabel.setText("Cutoff frequency", NotificationType::dontSendNotification);
    frequencyLabel.setLookAndFeel(&labelLookAndFeel);
    frequencyLabel.attachToComponent(&frequencySlider, false);
    frequencyLabel.setJustificationType(Justification::centredBottom);
    addAndMakeVisible(frequencyLabel);
    
    qSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    qSlider.setTextBoxStyle(Slider::TextBoxBelow, false, TEXTBOXWIDTH, TEXTBOXHEIGT);
    qSlider.setTextValueSuffix(" Q");
    qSlider.setRange(0.1f, 1.0f);
    qSlider.setValue(0.5f);
    qSlider.setSkewFactorFromMidPoint(0.3f);
    qSlider.addListener(this);
    qSlider.setTooltip(TRANS ("Filter quality"));
    addAndMakeVisible(qSlider);
    qLabel.setText("Quality", NotificationType::dontSendNotification);
    qLabel.setLookAndFeel(&labelLookAndFeel);
    qLabel.attachToComponent(&qSlider, false);
    qLabel.setJustificationType(Justification::centredBottom);
    addAndMakeVisible(qLabel);

    separationSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    separationSlider.setTextBoxStyle(Slider::TextBoxBelow, false, TEXTBOXWIDTH, TEXTBOXHEIGT);
    separationSlider.setTextValueSuffix(" dB");
    separationSlider.setRange(-6.0f, 0.0f);
    separationSlider.setValue(-4.0f);
    separationSlider.setSkewFactorFromMidPoint(-4.0f);
    separationSlider.addListener(this);
    separationSlider.setTooltip(TRANS ("Separation between direct and crossfeed signals"));
    addAndMakeVisible(separationSlider);
    separationLabel.setText("Separation", NotificationType::dontSendNotification);
    separationLabel.setLookAndFeel(&labelLookAndFeel);
    separationLabel.attachToComponent(&separationSlider, false);
    separationLabel.setJustificationType(Justification::centredBottom);
    addAndMakeVisible(separationLabel);
    
    directGainSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    directGainSlider.setTextBoxStyle(Slider::TextBoxBelow, false, TEXTBOXWIDTH, TEXTBOXHEIGT);
    directGainSlider.setTextValueSuffix(" dB");
    directGainSlider.setRange(-10.0f, 10.0f);
    directGainSlider.setValue(0.0f);
    directGainSlider.addListener(this);
    directGainSlider.setTooltip(TRANS ("Direct signal attenuation"));
    addAndMakeVisible(directGainSlider);
    directGainLabel.setText("Direct signal", NotificationType::dontSendNotification);
    directGainLabel.setLookAndFeel(&labelLookAndFeel);
    directGainLabel.attachToComponent(&directGainSlider, false);
    directGainLabel.setJustificationType(Justification::centredBottom);
    addAndMakeVisible(directGainLabel);
    
    xfeedGainSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    xfeedGainSlider.setTextBoxStyle(Slider::TextBoxBelow, false, TEXTBOXWIDTH, TEXTBOXHEIGT);
    xfeedGainSlider.setTextValueSuffix(" dB");
    xfeedGainSlider.setRange(-10.0f, 10.0f);
    xfeedGainSlider.setValue(0.0f);
    xfeedGainSlider.addListener(this);
    xfeedGainSlider.setTooltip(TRANS ("Xfeed signal attenuation"));
    addAndMakeVisible(xfeedGainSlider);
    xfeedGainLabel.setText("Xfeed signal", NotificationType::dontSendNotification);
    xfeedGainLabel.setLookAndFeel(&labelLookAndFeel);
    xfeedGainLabel.attachToComponent(&xfeedGainSlider, false);
    xfeedGainLabel.setJustificationType(Justification::centredBottom);
    addAndMakeVisible(xfeedGainLabel);

    activeStateToggleButton.setToggleState (true, NotificationType::dontSendNotification);
    activeStateToggleButton.setClickingTogglesState(true);
    addAndMakeVisible(activeStateToggleButton);
    
    spectrumAnalyserToggleButton.setToggleState(true, NotificationType::dontSendNotification);
    spectrumAnalyserToggleButton.setClickingTogglesState(true);
    spectrumAnalyserToggleButton.addListener(this);
    addAndMakeVisible(spectrumAnalyserToggleButton);
    
    delaySlider.setLookAndFeel(&rotaryLookAndFeel);
    frequencySlider.setLookAndFeel(&rotaryLookAndFeel);
    qSlider.setLookAndFeel(&rotaryLookAndFeel);
    separationSlider.setLookAndFeel(&rotaryLookAndFeel);
    directGainSlider.setLookAndFeel(&directLookAndFeel);
    xfeedGainSlider.setLookAndFeel(&xfeedLookAndFeel);
    
    addAndMakeVisible(processor.filterGraphics);
    addAndMakeVisible(processor.spectrumAnalyser);
    
}

GainSliderAudioProcessorEditor::~GainSliderAudioProcessorEditor()
{
}

//==============================================================================
void GainSliderAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void GainSliderAudioProcessorEditor::resized()
{
    Rectangle<int> bounds = getLocalBounds();
    Rectangle<int> menu = bounds.removeFromTop(TEXTBOXHEIGT);
    Rectangle<int> topPanel = bounds.removeFromTop(3*(DIALSIZE + TEXTBOXHEIGT + LABELHEIGHT)); //.reduced(3,3);
    Rectangle<int> dials = topPanel.removeFromLeft(DIALSIZE);
    Rectangle<int> slider1 = topPanel.removeFromLeft(DIALSIZE);
    Rectangle<int> slider2 = topPanel.removeFromLeft(DIALSIZE);
    Rectangle<int> slider3 = topPanel.removeFromLeft(DIALSIZE);
    spectrumFrame1 = topPanel.removeFromLeft(SPECTRUMWIDTH);
    spectrumFrame2 = bounds.removeFromTop(SPECTRUMHEIGHT);
    
    filterTypeMenu.setBounds(menu.removeFromLeft(DIALSIZE));
    crossFeedMenu.setBounds(menu.removeFromLeft(DIALSIZE));
    activeStateToggleButton.setBounds(menu.removeFromLeft(DIALSIZE));
    spectrumAnalyserToggleButton.setBounds(menu.removeFromLeft(DIALSIZE));
    
    delayLabel.setBounds(dials.removeFromTop(LABELHEIGHT));
    delaySlider.setBounds(dials.removeFromTop(DIALSIZE + TEXTBOXHEIGT));
    frequencyLabel.setBounds(dials.removeFromTop(LABELHEIGHT));
    frequencySlider.setBounds(dials.removeFromTop(DIALSIZE + TEXTBOXHEIGT));
    qLabel.setBounds(dials.removeFromTop(LABELHEIGHT));
    qSlider.setBounds(dials.removeFromTop(DIALSIZE + TEXTBOXHEIGT));
    
    separationLabel.setBounds(slider1.removeFromTop(LABELHEIGHT));
    separationSlider.setBounds(slider1);

    directGainLabel.setBounds(slider2.removeFromTop(LABELHEIGHT));
    directGainSlider.setBounds(slider2);
    
    xfeedGainLabel.setBounds(slider3.removeFromTop(LABELHEIGHT));
    xfeedGainSlider.setBounds(slider3);
    
    processor.filterGraphics.setBounds(spectrumFrame1);
    processor.spectrumAnalyser.setBounds(spectrumFrame2);

}

//==============================================================================
// Unavoidable methods that must be instanciated, even if not used
void GainSliderAudioProcessorEditor::buttonClicked(Button *toggleButton)
{
    if (toggleButton == &spectrumAnalyserToggleButton)
    {
        if (toggleButton->getToggleState())
        {
            setSize (4* DIALSIZE + SPECTRUMWIDTH, 3*(DIALSIZE + TEXTBOXHEIGT + LABELHEIGHT) + TEXTBOXHEIGT + SPECTRUMHEIGHT);
        }
        else
        {
            setSize (4* DIALSIZE, 3*(DIALSIZE + TEXTBOXHEIGT + LABELHEIGHT) + TEXTBOXHEIGT);
        }
    }
}

void GainSliderAudioProcessorEditor::comboBoxChanged(ComboBox *comboBox)
{
    if (comboBox == &crossFeedMenu)
    {
        if (comboBox->getSelectedIdAsValue() == 1)
        {
            delaySlider.setValue(320.0f);
            frequencySlider.setValue(600.0f);
            separationSlider.setValue(-5.0f);
        }
        else if (comboBox->getSelectedIdAsValue() == 2)
        {
            delaySlider.setValue(270.0f);
            frequencySlider.setValue(700.0f);
            separationSlider.setValue(-4.0f);
        }
        else if (comboBox->getSelectedIdAsValue() == 3)
        {
            delaySlider.setValue(230.0f);
            frequencySlider.setValue(800.0f);
            separationSlider.setValue(-2.5f);
        }
    }
}

void GainSliderAudioProcessorEditor::sliderValueChanged (Slider *slider)
{

}
