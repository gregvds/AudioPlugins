/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

static Colour colour1 = Colour(0xFF726658);
static Colour colour2 = Colour(0xFFf6f3ed);
static Colour colour3 = Colour(0xFFa88854);


//==============================================================================
GainSliderAudioProcessorEditor::GainSliderAudioProcessorEditor (GainSliderAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setName("mainPluginEditorGUI");
    tooltipWindow->setMillisecondsBeforeTipAppears (500);
    
#ifdef JUCE_OPENGL
    openGLContext.attachTo (*getTopLevelComponent());
#endif
    
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

    activeStateToggleButton.setToggleState (true, NotificationType::dontSendNotification);
    activeStateToggleButton.setClickingTogglesState(true);
    addAndMakeVisible(activeStateToggleButton);
    
//    spectrumAnalyserToggleButton.setToggleState(true, NotificationType::dontSendNotification);
//    spectrumAnalyserToggleButton.setClickingTogglesState(true);
//    spectrumAnalyserToggleButton.addListener(this);
//    addAndMakeVisible(spectrumAnalyserToggleButton);
    
    guiLayoutMenu.addItem("Plugin only", 1);
    guiLayoutMenu.addItem("+ Diagrams", 2);
    guiLayoutMenu.addItem("+ Spectrum", 3);
    guiLayoutMenu.addItem("Full plugin", 4);
    guiLayoutMenu.setSelectedId(2);
    guiLayoutMenu.setJustificationType(Justification::centred);
    guiLayoutMenu.addListener(this);
    guiLayoutMenu.setTooltip(TRANS ("Gui layout choice"));
    addAndMakeVisible(guiLayoutMenu);

    crossFeedMenu.addItem("Full", 1);
    crossFeedMenu.addItem("Medium", 2);
    crossFeedMenu.addItem("Light", 3);
    crossFeedMenu.addItem("Pure Haas", 4);
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
    
    delaySlider.setName("delaySlider");
    delaySlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    delaySlider.setTextBoxStyle(Slider::TextBoxBelow, false, TEXTBOXWIDTH, TEXTBOXHEIGT);
    delaySlider.setRange(0.0f, 320.0f);
    delaySlider.setValue(100.0f);
    delaySlider.setNumDecimalPlacesToDisplay(0);
    delaySlider.setTextValueSuffix(CharPointer_UTF8 (" \xc2\xb5s"));
    delaySlider.setSkewFactorFromMidPoint(50.0f);
    delaySlider.addListener(this);
    delaySlider.setTooltip(TRANS ("Delay time"));
    addAndMakeVisible(delaySlider);
    delayLabel.setText("Crossfeed delay", NotificationType::dontSendNotification);
    delayLabel.setLookAndFeel(&labelLookAndFeel);
    delayLabel.attachToComponent(&delaySlider, false);
    delayLabel.setJustificationType(Justification::centredBottom);
    addAndMakeVisible(delayLabel);
    
    frequencySlider.setName("frequencySlider");
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
    
    qSlider.setName("qSlider");
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

    separationSlider.setName("separationSlider");
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
    
    directGainSlider.setName("directGainSlider");
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
    
    xfeedGainSlider.setName("xfeedGainSlider");
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
    
    rotaryLookAndFeel1.setColour(Slider::ColourIds::thumbColourId, colour1);
    rotaryLookAndFeel2.setColour(Slider::ColourIds::thumbColourId, colour2);
    rotaryLookAndFeel3.setColour(Slider::ColourIds::thumbColourId, colour3);
    
    delaySlider.setLookAndFeel(&rotaryLookAndFeel1);
    frequencySlider.setLookAndFeel(&rotaryLookAndFeel2);
    qSlider.setLookAndFeel(&rotaryLookAndFeel3);
    separationSlider.setLookAndFeel(&rotaryLookAndFeel);
    directGainSlider.setLookAndFeel(&directLookAndFeel);
    xfeedGainSlider.setLookAndFeel(&xfeedLookAndFeel);
    
    addAndMakeVisible(processor.filterGraphics);
    addAndMakeVisible(processor.spectrumAnalyser);
    
}

GainSliderAudioProcessorEditor::~GainSliderAudioProcessorEditor()
{
#ifdef JUCE_OPENGL
    openGLContext.detach();
#endif
}

//==============================================================================
void GainSliderAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    
    Rectangle<int> bounds = getLocalBounds();
    Rectangle<int> topPanel = bounds.removeFromLeft(4*DIALSIZE);
    
    if (guiLayoutMenu.getSelectedId() == 4)
    {
        leftSpectrumPart = topPanel.removeFromBottom(SPECTRUMHEIGHT);
    }
    
    Rectangle<int> menu = topPanel.removeFromTop(TEXTBOXHEIGT).reduced(3,0);
    
    filterTypeMenu.setBounds(menu.removeFromLeft(DIALSIZE));
    crossFeedMenu.setBounds(menu.removeFromLeft(DIALSIZE));
    activeStateToggleButton.setBounds(menu.removeFromLeft(DIALSIZE));
    guiLayoutMenu.setBounds(menu.removeFromLeft(DIALSIZE));
    
    g.setColour (Colours::silver);
    topPanel = topPanel.reduced(3,3);
    g.drawRoundedRectangle (topPanel.toFloat(), 5, 2);
    topPanel = topPanel.reduced(0,3);
    
    int sliderSize = topPanel.getWidth()/4;
    
    Rectangle<int> dials = topPanel.removeFromLeft(sliderSize);
    Rectangle<int> slider1 = topPanel.removeFromLeft(sliderSize);
    Rectangle<int> slider2 = topPanel.removeFromLeft(sliderSize);
    Rectangle<int> slider3 = topPanel.removeFromLeft(sliderSize);
    
    int dialSize = dials.getHeight() - 3 * (LABELHEIGHT + TEXTBOXHEIGT);
    
    delayLabel.setBounds(dials.removeFromTop(LABELHEIGHT));
    delaySlider.setBounds(dials.removeFromTop(dialSize / 3 + TEXTBOXHEIGT));
    frequencyLabel.setBounds(dials.removeFromTop(LABELHEIGHT));
    frequencySlider.setBounds(dials.removeFromTop(dialSize / 3 + TEXTBOXHEIGT));
    qLabel.setBounds(dials.removeFromTop(LABELHEIGHT));
    qSlider.setBounds(dials.removeFromTop(dialSize / 3 + TEXTBOXHEIGT));
    
    separationLabel.setBounds(slider1.removeFromTop(LABELHEIGHT));
    separationSlider.setBounds(slider1);
    directGainLabel.setBounds(slider2.removeFromTop(LABELHEIGHT));
    directGainSlider.setBounds(slider2);
    xfeedGainLabel.setBounds(slider3.removeFromTop(LABELHEIGHT));
    xfeedGainSlider.setBounds(slider3);
    
    if (guiLayoutMenu.getSelectedId() == 1)
    {
        processor.spectrumAnalyser.setVisible(false);
        processor.filterGraphics.setVisible(false);
    }
    if (guiLayoutMenu.getSelectedId() == 2)
    {
        spectrumFrame1 = bounds.removeFromRight(SPECTRUMWIDTH);
        processor.filterGraphics.setBounds(spectrumFrame1);
        processor.spectrumAnalyser.setVisible(false);
        processor.filterGraphics.setVisible(true);
    }
    else if (guiLayoutMenu.getSelectedId() == 3)
    {
        spectrumFrame1 = bounds.removeFromRight(SPECTRUMWIDTH);
        processor.spectrumAnalyser.setBounds(spectrumFrame1);
        processor.filterGraphics.setVisible(false);
        processor.spectrumAnalyser.setVisible(true);
    }
    else if (guiLayoutMenu.getSelectedId() == 4)
    {
        spectrumFrame2 = bounds.removeFromBottom(SPECTRUMHEIGHT);
        spectrumFrame1 = bounds.removeFromRight(SPECTRUMWIDTH);
        spectrumFrame2.enlargeIfAdjacent(leftSpectrumPart);
        processor.filterGraphics.setBounds(spectrumFrame1);
        processor.spectrumAnalyser.setBounds(spectrumFrame2);
        processor.spectrumAnalyser.setVisible(true);
        processor.filterGraphics.setVisible(true);


    }

}

void GainSliderAudioProcessorEditor::resized()
{
    
}


//==============================================================================
// Unavoidable methods that must be instanciated, even if not used
void GainSliderAudioProcessorEditor::buttonClicked(Button *toggleButton)
{
//    if (toggleButton == &spectrumAnalyserToggleButton)
//    {
//        if (toggleButton->getToggleState())
//        {
//            setSize (4* DIALSIZE + SPECTRUMWIDTH, 3*(DIALSIZE + TEXTBOXHEIGT + LABELHEIGHT) + TEXTBOXHEIGT + SPECTRUMHEIGHT);
//        }
//        else
//        {
//            setSize (4* DIALSIZE, 3*(DIALSIZE + TEXTBOXHEIGT + LABELHEIGHT) + TEXTBOXHEIGT);
//        }
//    }
}

void GainSliderAudioProcessorEditor::comboBoxChanged(ComboBox *comboBox)
{
    if (comboBox == &crossFeedMenu)
    {
        auto filterTypeIndex = filterTypeMenu.getSelectedId() - 1;
        auto filterIntensityIndex = crossFeedMenu.getSelectedId() - 1;
        
        delaySlider.setValue(settings[filterIntensityIndex][filterTypeIndex][0]);
        frequencySlider.setValue(settings[filterIntensityIndex][filterTypeIndex][1]);
        separationSlider.setValue(settings[filterIntensityIndex][filterTypeIndex][2]);
        qSlider.setValue(settings[filterIntensityIndex][filterTypeIndex][3]);
        directGainSlider.setValue(settings[filterIntensityIndex][filterTypeIndex][4]);
        xfeedGainSlider.setValue(settings[filterIntensityIndex][filterTypeIndex][5]);
    }
    if (comboBox == &guiLayoutMenu)
    {
        auto guiLayoutIndex = guiLayoutMenu.getSelectedId() - 1;

        setSize(guiSizes[guiLayoutIndex][0], guiSizes[guiLayoutIndex][1]);
    }
}

void GainSliderAudioProcessorEditor::sliderValueChanged (Slider *slider)
{

}
