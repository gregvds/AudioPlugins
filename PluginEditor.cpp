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
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    // Size is dynamic regarding the number of objects
    setSize (3* DIALSIZE , 3*(DIALSIZE + TEXTBOXHEIGT + LABELHEIGHT) + TEXTBOXHEIGT);
    
    delaySliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, DELAY_ID, delaySlider);
    freqSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, FREQ_ID, frequencySlider);
    qSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, Q_ID, qSlider);
    sepSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, SEP_ID, separationSlider);
    gainSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, GAIN_ID, gainSlider);
    ActiveStateToggleButtonAttach = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(processor.treeState, ACTIVE_ID, ActiveStateToggleButton);

    crossFeedMenu.addItem("Full", 1);
    crossFeedMenu.addItem("Medium", 2);
    crossFeedMenu.addItem("Light", 3);
    crossFeedMenu.setSelectedId(2);
    crossFeedMenu.setJustificationType(Justification::centred);
    crossFeedMenu.addListener(this);
    crossFeedMenu.setTooltip("Global intensity settings");
    addAndMakeVisible(&crossFeedMenu);
    
    delaySlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    delaySlider.setTextBoxStyle(Slider::TextBoxBelow, false, TEXTBOXWIDTH, TEXTBOXHEIGT);
    delaySlider.setTextValueSuffix(" microsec");
    delaySlider.setRange(200.0f, 320.0f);
    delaySlider.setValue(270.0f);
    delaySlider.setSkewFactorFromMidPoint(270.0f);
    delaySlider.addListener(this);
    delaySlider.setTooltip("Delay time");
    addAndMakeVisible(&delaySlider);
    delayLabel.setText("Crossfeed delay", NotificationType::dontSendNotification);
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
    frequencySlider.setTooltip("Crossfeed cutoff frequency");
    addAndMakeVisible(&frequencySlider);
    frequencyLabel.setText("Cutoff frequency", NotificationType::dontSendNotification);
    frequencyLabel.attachToComponent(&frequencySlider, false);
    frequencyLabel.setJustificationType(Justification::centredBottom);
    addAndMakeVisible(frequencyLabel);
    
    qSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    qSlider.setTextBoxStyle(Slider::TextBoxBelow, false, TEXTBOXWIDTH, TEXTBOXHEIGT);
    qSlider.setTextValueSuffix(" Q");
    qSlider.setRange(0.1f, 0.6f);
    qSlider.setValue(0.4f);
    qSlider.setSkewFactorFromMidPoint(0.4f);
    qSlider.addListener(this);
    qSlider.setTooltip("Filter Q value");
    addAndMakeVisible(&qSlider);
    qLabel.setText("Q factor", NotificationType::dontSendNotification);
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
    separationSlider.setTooltip("Separation between direct and crossfeed signals");
    addAndMakeVisible(&separationSlider);
    separationLabel.setText("Separation", NotificationType::dontSendNotification);
    separationLabel.attachToComponent(&separationSlider, false);
    separationLabel.setJustificationType(Justification::centredBottom);
    addAndMakeVisible(separationLabel);
    
    gainSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    gainSlider.setTextBoxStyle(Slider::TextBoxBelow, false, TEXTBOXWIDTH, TEXTBOXHEIGT);
    gainSlider.setTextValueSuffix(" dB");
    gainSlider.setRange(-10.0f, 0.0f);
    gainSlider.setValue(0.0f);
    gainSlider.addListener(this);
    gainSlider.setTooltip("Direct signal attenuation");
    addAndMakeVisible(&gainSlider);
    gainLabel.setText("Direct signal", NotificationType::dontSendNotification);
    gainLabel.attachToComponent(&gainSlider, false);
    gainLabel.setJustificationType(Justification::centredBottom);
    addAndMakeVisible(gainLabel);
    
    ActiveStateToggleButton.setToggleState (true, NotificationType::dontSendNotification);
    addAndMakeVisible(&ActiveStateToggleButton);
    
    delaySlider.setLookAndFeel(&rotaryLookAndFeel);
    frequencySlider.setLookAndFeel(&rotaryLookAndFeel);
    qSlider.setLookAndFeel(&rotaryLookAndFeel);
    separationSlider.setLookAndFeel(&verticalLookAndFeel);
    gainSlider.setLookAndFeel(&verticalLookAndFeel);
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
    Rectangle<int> dials = bounds.removeFromLeft(DIALSIZE);
    Rectangle<int> slider1 = bounds.removeFromLeft(DIALSIZE);
    Rectangle<int> slider2 = bounds.removeFromLeft(DIALSIZE);
    
    crossFeedMenu.setBounds(menu.removeFromLeft(2*DIALSIZE));
    ActiveStateToggleButton.setBounds(menu);
    
    delayLabel.setBounds(dials.removeFromTop(LABELHEIGHT));
    delaySlider.setBounds(dials.removeFromTop(DIALSIZE + TEXTBOXHEIGT));
    frequencyLabel.setBounds(dials.removeFromTop(LABELHEIGHT));
    frequencySlider.setBounds(dials.removeFromTop(DIALSIZE + TEXTBOXHEIGT));
    qLabel.setBounds(dials.removeFromTop(LABELHEIGHT));
    qSlider.setBounds(dials.removeFromTop(DIALSIZE + TEXTBOXHEIGT));
    
    separationLabel.setBounds(slider1.removeFromTop(LABELHEIGHT));
    separationSlider.setBounds(slider1);

    gainLabel.setBounds(slider2.removeFromTop(LABELHEIGHT));
    gainSlider.setBounds(slider2);
    
}

//==============================================================================
// Unavoidable methods that must be instanciated, even if not used
void GainSliderAudioProcessorEditor::comboBoxChanged(ComboBox *comboBox)
{
    if (comboBox == &crossFeedMenu)
    {
        if (comboBox->getSelectedIdAsValue() == 1)
        {
            delaySlider.setValue(320.0f);
            frequencySlider.setValue(600.0f);
            qSlider.setValue(0.5f);
            separationSlider.setValue(-5.0f);
            //gainSlider.setValue(0.0f);
        }
        else if (comboBox->getSelectedIdAsValue() == 2)
        {
            delaySlider.setValue(270.0f);
            frequencySlider.setValue(700.0f);
            qSlider.setValue(0.4f);
            separationSlider.setValue(-4.0f);
            //gainSlider.setValue(0.0f);
        }
        else if (comboBox->getSelectedIdAsValue() == 3)
        {
            delaySlider.setValue(230.0f);
            frequencySlider.setValue(800.0f);
            qSlider.setValue(0.3f);
            separationSlider.setValue(-2.5f);
            //gainSlider.setValue(0.0f);
        }
    }
}

void GainSliderAudioProcessorEditor::sliderValueChanged (Slider *slider)
{

}
