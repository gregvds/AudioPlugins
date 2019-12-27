/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GainSliderAudioProcessorEditor::GainSliderAudioProcessorEditor (GainSliderAudioProcessor& p, AudioProcessorValueTreeState& vts)
: AudioProcessorEditor (&p), processor (p), valueTreeState (vts)
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
    

    activeStateToggleButton.setToggleState (true, NotificationType::dontSendNotification);
    activeStateToggleButton.setClickingTogglesState(true);
    addAndMakeVisible(activeStateToggleButton);
        
    guiLayoutMenu.addItem("Plugin only", 1);
    guiLayoutMenu.addItem("+ Diagrams", 2);
    guiLayoutMenu.addItem("+ Spectrum", 3);
    guiLayoutMenu.addItem("Full plugin", 4);
    guiLayoutMenu.setSelectedId(2);
    guiLayoutMenu.setJustificationType(Justification::centred);
    guiLayoutMenu.addListener(this);
    guiLayoutMenu.setTooltip(TRANS ("Gui layout choice"));
    addAndMakeVisible(guiLayoutMenu);

    crossFeedMenu.setEditableText(true);
    crossFeedMenu.addItem("Full", 1);
    crossFeedMenu.addItem("Medium", 2);
    crossFeedMenu.addItem("Light", 3);
    crossFeedMenu.addItem("Pure Haas", 4);
    crossFeedMenu.setSelectedId(2);
    crossFeedMenu.setJustificationType(Justification::centred);
    crossFeedMenu.addListener(this);
    crossFeedMenu.setTooltip(TRANS ("Global intensity settings"));
    addAndMakeVisible(crossFeedMenu);
    
    delaySlider.setName("delaySlider");
    delaySlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    delaySlider.setTextBoxStyle(Slider::TextBoxBelow, false, TEXTBOXWIDTH, TEXTBOXHEIGT);
    delaySlider.setRange(0.0f, 320.0f);
    delaySlider.setNumDecimalPlacesToDisplay(0);
    delaySlider.setTextValueSuffix(CharPointer_UTF8 (" \xc2\xb5s"));
    delaySlider.setSkewFactorFromMidPoint(50.0f);
    delaySlider.addListener(this);
    delaySlider.onValueChange = [this] {filterGraphics.phases[0] = delaySlider.getValue();
                                        filterGraphics.updatePhasesRange();
                                        repaint(); };
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
    frequencySlider.setRange(300.0f, 1000.0f);
    frequencySlider.setSkewFactorFromMidPoint(600.0f);
    frequencySlider.onValueChange = [this] {filterGraphics.freqs[0] = frequencySlider.getValue();
                                            filterGraphics.freqs[1] = frequencySlider.getValue();
                                            filterGraphics.updatePhasesRange();
                                            repaint(); };
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
    qSlider.setSkewFactorFromMidPoint(0.3f);
    qSlider.onValueChange = [this] {filterGraphics.updatePhasesRange();
                                    repaint(); };
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
    separationSlider.setSkewFactorFromMidPoint(-4.0f);
    separationSlider.onValueChange = [this] {filterGraphics.updatePhasesRange();
                                             repaint(); };
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
    directGainSlider.onValueChange = [this] {filterGraphics.gains[1] = directGainSlider.getValue(); repaint(); };
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
    xfeedGainSlider.onValueChange = [this] {filterGraphics.gains[0] = xfeedGainSlider.getValue(); repaint(); };
    xfeedGainSlider.setTooltip(TRANS ("Xfeed signal attenuation"));
    addAndMakeVisible(xfeedGainSlider);
    xfeedGainLabel.setText("Xfeed signal", NotificationType::dontSendNotification);
    xfeedGainLabel.setLookAndFeel(&labelLookAndFeel);
    xfeedGainLabel.attachToComponent(&xfeedGainSlider, false);
    xfeedGainLabel.setJustificationType(Justification::centredBottom);
    addAndMakeVisible(xfeedGainLabel);
    
    rotaryLookAndFeel1.setColour(Slider::ColourIds::thumbColourId, delayColour);
    rotaryLookAndFeel2.setColour(Slider::ColourIds::thumbColourId, freqColour);
    rotaryLookAndFeel3.setColour(Slider::ColourIds::thumbColourId, bandwidthColour);
    
    delaySlider.setLookAndFeel(&rotaryLookAndFeel1);
    frequencySlider.setLookAndFeel(&rotaryLookAndFeel2);
    qSlider.setLookAndFeel(&rotaryLookAndFeel3);
    separationSlider.setLookAndFeel(&rotaryLookAndFeel);
    directGainSlider.setLookAndFeel(&directLookAndFeel);
    xfeedGainSlider.setLookAndFeel(&xfeedLookAndFeel);
    
    delaySliderAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (valueTreeState, DELAY_ID, delaySlider));
    freqSliderAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (valueTreeState, FREQ_ID, frequencySlider));
    qSliderAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (valueTreeState, Q_ID, qSlider));
    sepSliderAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (valueTreeState, SEP_ID, separationSlider));
    directGainSliderAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (valueTreeState, DGAIN_ID, directGainSlider));
    xfeedGainSliderAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (valueTreeState, XGAIN_ID, xfeedGainSlider));
    activeStateToggleButtonAttach.reset (new AudioProcessorValueTreeState::ButtonAttachment (valueTreeState, ACTIVE_ID, activeStateToggleButton));
    guiLayoutMenuAttach.reset (new AudioProcessorValueTreeState::ComboBoxAttachment (valueTreeState, SPECTR_ID, guiLayoutMenu));
    settingsMenuAttach.reset (new AudioProcessorValueTreeState::ComboBoxAttachment (valueTreeState, SETTINGS_ID, crossFeedMenu));


    filterGraphics.freqs[0] = frequencySlider.getValue();
    filterGraphics.freqs[1] = frequencySlider.getValue();
    filterGraphics.gains[0] = xfeedGainSlider.getValue();
    filterGraphics.gains[1] = directGainSlider.getValue();
    filterGraphics.phases[0] = delaySlider.getValue();
    
    addAndMakeVisible(filterGraphics);
    addAndMakeVisible(spectrumAnalyser);
    
    settingsDictionnary[String("Full")] = {0.0f , 400.0f , -5.0f, 0.57f, 5.0f, -5.0f};
    settingsDictionnary[String("Medium")] = {94.0f, 700.0f , -4.0f, 0.45f, 4.0f, -4.0f};
    settingsDictionnary[String("Light")] = {67.0f, 400.0f , -3.0f, 0.6f, 3.1f, -3.1f};
    settingsDictionnary[String("Pure Haas")] = {270.0f, 700.0f , 0.0f, 1.0f, 1.0f, -3.0f};
    
    /*
    preferencesButton.setButtonText("Preferences");
    preferencesButton.onClick = [this] {prefsPanel.showInDialogBox("Preferences", 400, 400, getLookAndFeel().findColour (ResizableWindow::backgroundColourId)); };
    addAndMakeVisible(preferencesButton);
    
    // Here we add the different pages of preferences in the panel.
    prefsPanel.addSettingsPage("Colours", PAF_Data::colour_svg, PAF_Data::colour_svgSize);
    prefsPanel.colourSelector.setSwatchColour(0, delayColour);
    prefsPanel.colourSelector.setSwatchColour(1, freqColour);
    prefsPanel.colourSelector.setSwatchColour(2, bandwidthColour);
    prefsPanel.colourSelector.setSwatchColour(3, separationColour);
    prefsPanel.colourSelector.setSwatchColour(4, xfeedColour);
    prefsPanel.colourSelector.setSwatchColour(5, directColour);
    prefsPanel.colourSelector.setSwatchColour(6, outColour);
    DBG("Number of swatches" << prefsPanel.colourSelector.getNumSwatches());
    */
}

GainSliderAudioProcessorEditor::~GainSliderAudioProcessorEditor()
{
#ifdef JUCE_OPENGL
    openGLContext.detach();
#endif
}

//==============================================================================

//==============================================================================
void GainSliderAudioProcessorEditor::paint (Graphics& g)
{
    Graphics::ScopedSaveState state (g);
    
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    bounds = getLocalBounds();
    topPanel = bounds.removeFromLeft(4*DIALSIZE);
    
    if (guiLayoutMenu.getSelectedId() == 4)
    {
        leftSpectrumPart = topPanel.removeFromBottom(SPECTRUMHEIGHT);
    }
    
    menu = topPanel.removeFromTop(TEXTBOXHEIGT).reduced(3,0);
    
    crossFeedMenu.setBounds(menu.removeFromLeft(DIALSIZE));
    //preferencesButton.setBounds(menu.removeFromLeft(DIALSIZE));
    guiLayoutMenu.setBounds(menu.removeFromRight(DIALSIZE));
    activeStateToggleButton.setBounds(menu.removeFromRight(DIALSIZE));
    
    g.setColour (Colours::silver);
    topPanel = topPanel.reduced(3,3);
    g.drawRoundedRectangle (topPanel.toFloat(), 5, 2);
    topPanel = topPanel.reduced(0,3);
    
    int sliderSize = topPanel.getWidth()/4;
    
    dials = topPanel.removeFromLeft(sliderSize);
    slider1 = topPanel.removeFromLeft(sliderSize);
    slider2 = topPanel.removeFromLeft(sliderSize);
    slider3 = topPanel.removeFromLeft(sliderSize);
    
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
        spectrumAnalyser.setVisible(false);
        filterGraphics.setVisible(false);
    }
    if (guiLayoutMenu.getSelectedId() == 2)
    {
        spectrumFrame1 = bounds.removeFromRight(SPECTRUMWIDTH);
        filterGraphics.setBounds(spectrumFrame1);
        spectrumAnalyser.setVisible(false);
        filterGraphics.setVisible(true);
    }
    else if (guiLayoutMenu.getSelectedId() == 3)
    {
        spectrumFrame1 = bounds.removeFromRight(SPECTRUMWIDTH);
        spectrumAnalyser.setBounds(spectrumFrame1);
        filterGraphics.setVisible(false);
        spectrumAnalyser.setVisible(true);
    }
    else if (guiLayoutMenu.getSelectedId() == 4)
    {
        spectrumFrame2 = bounds.removeFromBottom(SPECTRUMHEIGHT);
        spectrumFrame1 = bounds.removeFromRight(SPECTRUMWIDTH);
        spectrumFrame2.enlargeIfAdjacent(leftSpectrumPart);
        filterGraphics.setBounds(spectrumFrame1);
        spectrumAnalyser.setBounds(spectrumFrame2);
        spectrumAnalyser.setVisible(true);
        filterGraphics.setVisible(true);
    }
}

void GainSliderAudioProcessorEditor::resized()
{
}


//==============================================================================
// Unavoidable methods that must be instanciated, even if not used
void GainSliderAudioProcessorEditor::sliderValueChanged(Slider *slider)
{
    //DBG("In sliderValueChanged");
}

void GainSliderAudioProcessorEditor::buttonClicked(Button *toggleButton)
{
}

void GainSliderAudioProcessorEditor::comboBoxChanged(ComboBox *comboBox)
{
    if (comboBox == &crossFeedMenu)
    {
        // This code intends to save a newly named preset.
        // Currently, it works only when the edition is based from first combobox setting (Full)
        // If a new setting is attemptedly recorded when the combobox is on alother element,
        // The new preset is lot and Full is choosen again. This is not known why...
        auto comboboxText = crossFeedMenu.getText();
        auto itemsNumber = crossFeedMenu.getNumItems();
        //DBG("comboboxText: " << comboboxText);
        //DBG("number of items, selected ID as value: " << itemsNumber << ", " << crossFeedMenu.getSelectedIdAsValue().toString());

        // If text content is unknown, it's a New setting that has to be saved
        if (comboboxText != "")
        {
            if (settingsDictionnary.find(comboboxText) == settingsDictionnary.end())
            {
                /*
                DBG("====== A new unknown text =======");
                for (int i = 0; i < crossFeedMenu.getNumItems(); ++i)
                {
                    DBG("Label, Index, i: " << crossFeedMenu.getItemText(i) << ", " << crossFeedMenu.indexOfItemId(i) << ", " << i);
                }
                */
                crossFeedMenu.addItem(comboboxText, itemsNumber + 1);
                settingsDictionnary[comboboxText] = {(float) delaySlider.getValue() ,
                                                     (float) frequencySlider.getValue() ,
                                                     (float) separationSlider.getValue() ,
                                                     (float) qSlider.getValue() ,
                                                     (float) directGainSlider.getValue() ,
                                                     (float) xfeedGainSlider.getValue()};
                crossFeedMenu.setSelectedId(crossFeedMenu.getNumItems());
                /*
                for (int i = 0; i < crossFeedMenu.getNumItems(); ++i)
                {
                    DBG("Label, Index, i: " << crossFeedMenu.getItemText(i) << ", " << crossFeedMenu.indexOfItemId(i) << ", " << i);
                }
                */
            }
            else
            {
                delaySlider.setValue(settingsDictionnary[comboboxText][0]);
                frequencySlider.setValue(settingsDictionnary[comboboxText][1]);
                separationSlider.setValue(settingsDictionnary[comboboxText][2]);
                qSlider.setValue(settingsDictionnary[comboboxText][3]);
                directGainSlider.setValue(settingsDictionnary[comboboxText][4]);
                xfeedGainSlider.setValue(settingsDictionnary[comboboxText][5]);
                if (comboboxText == "Pure Haas")
                {
                    frequencySlider.setEnabled(false);
                    qSlider.setEnabled(false);
                }
                else
                {
                    frequencySlider.setEnabled(true);
                    qSlider.setEnabled(true);
                }
            }
        } else
        {
            crossFeedMenu.setSelectedId(0);
            
        }
    }
    if (comboBox == &guiLayoutMenu)
    {
        auto guiLayoutIndex = guiLayoutMenu.getSelectedId() - 1;
        setSize(guiSizes[guiLayoutIndex][0], guiSizes[guiLayoutIndex][1]);
    }
}
