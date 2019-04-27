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
    setSize (3* DIALSIZE , 3*(DIALSIZE + TEXTBOXHEIGT) + TEXTBOXHEIGT);
    //setSize(800, 600);
    
    crossFeedMenu.addItem("Full", 1);
    crossFeedMenu.addItem("Medium", 2);
    crossFeedMenu.addItem("Light", 3);
    crossFeedMenu.setSelectedId(2);
    
    crossFeedMenu.setJustificationType(Justification::centred);
    crossFeedMenu.addListener(this);
    addAndMakeVisible(&crossFeedMenu);
    
    delaySliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, DELAY_ID, delaySlider);
    freqSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, FREQ_ID, frequencySlider);
    qSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, Q_ID, qSlider);
    sepSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, SEP_ID, separationSlider);
    gainSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, GAIN_ID, gainSlider);
    ActiveStateToggleButtonAttach = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(processor.treeState, ACTIVE_ID, ActiveStateToggleButton);

    delaySlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    delaySlider.setTextBoxStyle(Slider::TextBoxBelow, false, TEXTBOXWIDTH, TEXTBOXHEIGT);
    delaySlider.setTextValueSuffix("microsecs");
    delaySlider.setRange(200.0f, 300.0f);
    delaySlider.setValue(250.0f);
    delaySlider.addListener(this);
    addAndMakeVisible(&delaySlider);
    
    frequencySlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    frequencySlider.setTextBoxStyle(Slider::TextBoxBelow, false, TEXTBOXWIDTH, TEXTBOXHEIGT);
    frequencySlider.setTextValueSuffix("Hz");
    frequencySlider.setRange(400.0f, 1000.0f);
    frequencySlider.setValue(700.0f);
    //frequencySlider.setSkewFactorFromMidPoint(<#double sliderValueToShowAtMidPoint#>)
    frequencySlider.addListener(this);
    addAndMakeVisible(&frequencySlider);
    
    qSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    qSlider.setTextBoxStyle(Slider::TextBoxBelow, false, TEXTBOXWIDTH, TEXTBOXHEIGT);
    qSlider.setTextValueSuffix("Q");
    qSlider.setRange(0.1f, 0.6f);
    qSlider.setValue(0.2f);
    qSlider.setSkewFactorFromMidPoint(0.2f);
    qSlider.addListener(this);
    addAndMakeVisible(&qSlider);
    
    separationSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    separationSlider.setTextBoxStyle(Slider::TextBoxBelow, false, TEXTBOXWIDTH, TEXTBOXHEIGT);
    separationSlider.setTextValueSuffix("dB");
    separationSlider.setRange(-10.0f, 0.0f);
    separationSlider.setValue(-3.0f);
    separationSlider.setSkewFactorFromMidPoint(-3.0f);
    separationSlider.addListener(this);
    addAndMakeVisible(&separationSlider);
    
    gainSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    gainSlider.setTextBoxStyle(Slider::TextBoxBelow, false, TEXTBOXWIDTH, TEXTBOXHEIGT);
    gainSlider.setTextValueSuffix("dB");
    gainSlider.setRange(-48.0, 0.0);
    gainSlider.setValue(-1.0);
    gainSlider.addListener(this);
    addAndMakeVisible(&gainSlider);
    
    ActiveStateToggleButton.setToggleState (true, NotificationType::dontSendNotification);
    addAndMakeVisible(&ActiveStateToggleButton);
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
    Rectangle<int> sliders = bounds.removeFromLeft(2*DIALSIZE);
    
    crossFeedMenu.setBounds(menu.removeFromLeft(2*DIALSIZE));
    ActiveStateToggleButton.setBounds(menu);
    delaySlider.setBounds(dials.removeFromTop(DIALSIZE + TEXTBOXHEIGT));
    frequencySlider.setBounds(dials.removeFromTop(DIALSIZE + TEXTBOXHEIGT));
    qSlider.setBounds(dials.removeFromTop(DIALSIZE + TEXTBOXHEIGT));
    separationSlider.setBounds(sliders.removeFromLeft(DIALSIZE));
    gainSlider.setBounds(sliders.removeFromLeft(DIALSIZE));
    
}

//==============================================================================
// Unavoidable methods that must be instanciated, even if not used
void GainSliderAudioProcessorEditor::comboBoxChanged(ComboBox *comboBox)
{
    if (comboBox == &crossFeedMenu)
    {
        if (comboBox->getSelectedIdAsValue() == 1)
        {
            delaySlider.setValue(300.0f);
            frequencySlider.setValue(900.0f);
            qSlider.setValue(0.4f);
            separationSlider.setValue(-6.0f);
            gainSlider.setValue(0.0f);
        }
        else if (comboBox->getSelectedIdAsValue() == 2)
        {
            delaySlider.setValue(250.0f);
            frequencySlider.setValue(800.0f);
            qSlider.setValue(0.4f);
            separationSlider.setValue(-4.0f);
            gainSlider.setValue(0.0f);
        }
        else if (comboBox->getSelectedIdAsValue() == 3)
        {
            delaySlider.setValue(200.0f);
            frequencySlider.setValue(700.0f);
            qSlider.setValue(0.3f);
            separationSlider.setValue(-3.0f);
            gainSlider.setValue(0.0f);
        }
    }
}

void GainSliderAudioProcessorEditor::sliderValueChanged (Slider *slider)
{

}
