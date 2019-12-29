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
    crossFeedMenu.addItem("Mode 1", 1);
    crossFeedMenu.addItem("Mode 2", 2);
    crossFeedMenu.addItem("Mode 3", 3);
    crossFeedMenu.addItem("Mode 4", 4);
    crossFeedMenu.addItem("Mode 5", 5);
    crossFeedMenu.addItem("Mode 6", 6);
    crossFeedMenu.addItem("Pure Haas", 7);
    crossFeedMenu.addItem("Nice", 8);
    crossFeedMenu.setSelectedId(8);
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
    frequencySlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    frequencySlider.setTextBoxStyle(Slider::TextBoxBelow, false, TEXTBOXWIDTH, TEXTBOXHEIGT);
    frequencySlider.setTextValueSuffix(" Hz");
    frequencySlider.setRange(300.0f, 1000.0f);
    frequencySlider.setSkewFactorFromMidPoint(600.0f);
    frequencySlider.onValueChange = [this] {filterGraphics.freqs[0] = frequencySlider.getValue();
                                            filterGraphics.updatePhasesRange();
                                            repaint(); };
    frequencySlider.setTooltip(TRANS ("Direct filter frequency"));
    addAndMakeVisible(frequencySlider);
    frequencyLabel.setText("Direct frequency", NotificationType::dontSendNotification);
    frequencyLabel.setLookAndFeel(&labelLookAndFeel);
    frequencyLabel.attachToComponent(&frequencySlider, false);
    frequencyLabel.setJustificationType(Justification::centredBottom);
    addAndMakeVisible(frequencyLabel);
    
    xfeedFrequencySlider.setName("xfeedFrequencySlider");
    xfeedFrequencySlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    xfeedFrequencySlider.setTextBoxStyle(Slider::TextBoxBelow, false, TEXTBOXWIDTH, TEXTBOXHEIGT);
    xfeedFrequencySlider.setTextValueSuffix(" Hz");
    xfeedFrequencySlider.setRange(300.0f, 1000.0f);
    xfeedFrequencySlider.setSkewFactorFromMidPoint(600.0f);
    xfeedFrequencySlider.onValueChange = [this] {filterGraphics.freqs[1] = xfeedFrequencySlider.getValue();
                                            filterGraphics.updatePhasesRange();
                                            repaint(); };
    xfeedFrequencySlider.setTooltip(TRANS ("Crossfeed filter frequency"));
    addAndMakeVisible(xfeedFrequencySlider);
    xfeedFrequencyLabel.setText("Xfeed frequency", NotificationType::dontSendNotification);
    xfeedFrequencyLabel.setLookAndFeel(&labelLookAndFeel);
    xfeedFrequencyLabel.attachToComponent(&xfeedFrequencySlider, false);
    xfeedFrequencyLabel.setJustificationType(Justification::centredBottom);
    addAndMakeVisible(xfeedFrequencyLabel);

    qSlider.setName("qSlider");
    qSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    qSlider.setTextBoxStyle(Slider::TextBoxBelow, false, TEXTBOXWIDTH, TEXTBOXHEIGT);
    qSlider.setTextValueSuffix(" Q");
    qSlider.setRange(0.1f, 1.0f);
    qSlider.setSkewFactorFromMidPoint(0.3f);
    qSlider.onValueChange = [this] {filterGraphics.updatePhasesRange();
                                    repaint(); };
    qSlider.setTooltip(TRANS ("Direct Filter quality"));
    addAndMakeVisible(qSlider);
    qLabel.setText("Direct Q", NotificationType::dontSendNotification);
    qLabel.setLookAndFeel(&labelLookAndFeel);
    qLabel.attachToComponent(&qSlider, false);
    qLabel.setJustificationType(Justification::centredBottom);
    addAndMakeVisible(qLabel);
    
    xfeedQSlider.setName("xfeedQSlider");
    xfeedQSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    xfeedQSlider.setTextBoxStyle(Slider::TextBoxBelow, false, TEXTBOXWIDTH, TEXTBOXHEIGT);
    xfeedQSlider.setTextValueSuffix(" Q");
    xfeedQSlider.setRange(0.1f, 1.0f);
    xfeedQSlider.setSkewFactorFromMidPoint(0.3f);
    xfeedQSlider.onValueChange = [this] {filterGraphics.updatePhasesRange();
                                    repaint(); };
    xfeedQSlider.setTooltip(TRANS ("Xfeed Filter quality"));
    addAndMakeVisible(xfeedQSlider);
    xfeedQLabel.setText("Xfeed Q", NotificationType::dontSendNotification);
    xfeedQLabel.setLookAndFeel(&labelLookAndFeel);
    xfeedQLabel.attachToComponent(&xfeedQSlider, false);
    xfeedQLabel.setJustificationType(Justification::centredBottom);
    addAndMakeVisible(xfeedQLabel);
    
    separationSlider.setName("separationSlider");
    separationSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    separationSlider.setTextBoxStyle(Slider::TextBoxBelow, false, TEXTBOXWIDTH, TEXTBOXHEIGT);
    separationSlider.setTextValueSuffix(" dB");
    separationSlider.setRange(-6.0f, 0.0f);
    separationSlider.setSkewFactorFromMidPoint(-4.0f);
    separationSlider.onValueChange = [this] {filterGraphics.updatePhasesRange();
                                             if (separationSlider.getValue() == 0.0f)
                                             {
                                                 qSlider.setEnabled(false);
                                                 frequencySlider.setEnabled(false);
                                             }
                                             else
                                             {
                                                 qSlider.setEnabled(true);
                                                 frequencySlider.setEnabled(true);
                                             }
                                             repaint(); };
    separationSlider.setTooltip(TRANS ("Direct filter intensity"));
    addAndMakeVisible(separationSlider);
    separationLabel.setText("Direct filter Intensity", NotificationType::dontSendNotification);
    separationLabel.setLookAndFeel(&labelLookAndFeel);
    separationLabel.attachToComponent(&separationSlider, false);
    separationLabel.setJustificationType(Justification::centredBottom);
    addAndMakeVisible(separationLabel);

    xfeedSeparationSlider.setName("xfeedSeparationSlider");
    xfeedSeparationSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    xfeedSeparationSlider.setTextBoxStyle(Slider::TextBoxBelow, false, TEXTBOXWIDTH, TEXTBOXHEIGT);
    xfeedSeparationSlider.setTextValueSuffix(" dB");
    xfeedSeparationSlider.setRange(-6.0f, 0.0f);
    xfeedSeparationSlider.setSkewFactorFromMidPoint(-4.0f);
    xfeedSeparationSlider.onValueChange = [this] {filterGraphics.updatePhasesRange();
                                                  if (xfeedSeparationSlider.getValue() == 0.0f)
                                                  {
                                                      xfeedQSlider.setEnabled(false);
                                                      xfeedFrequencySlider.setEnabled(false);
                                                  }
                                                  else
                                                  {
                                                      xfeedQSlider.setEnabled(true);
                                                      xfeedFrequencySlider.setEnabled(true);
                                                  }
                                             repaint(); };
    xfeedSeparationSlider.setTooltip(TRANS ("Crossfeed filter intensity"));
    addAndMakeVisible(xfeedSeparationSlider);
    xfeedSeparationLabel.setText("Xfeed filter intensity", NotificationType::dontSendNotification);
    xfeedSeparationLabel.setLookAndFeel(&labelLookAndFeel);
    xfeedSeparationLabel.attachToComponent(&xfeedSeparationSlider, false);
    xfeedSeparationLabel.setJustificationType(Justification::centredBottom);
    addAndMakeVisible(xfeedSeparationLabel);

    
    directGainSlider.setName("directGainSlider");
    directGainSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
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
    xfeedGainSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
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
    rotaryLookAndFeel4.setColour(Slider::ColourIds::thumbColourId, freqColour);
    rotaryLookAndFeel3.setColour(Slider::ColourIds::thumbColourId, bandwidthColour);
    rotaryLookAndFeel5.setColour(Slider::ColourIds::thumbColourId, bandwidthColour);
    
    delaySlider.setLookAndFeel(&rotaryLookAndFeel1);
    frequencySlider.setLookAndFeel(&rotaryLookAndFeel2);
    xfeedFrequencySlider.setLookAndFeel(&rotaryLookAndFeel4);
    qSlider.setLookAndFeel(&rotaryLookAndFeel3);
    xfeedQSlider.setLookAndFeel(&rotaryLookAndFeel5);
    separationSlider.setLookAndFeel(&rotaryLookAndFeel);
    xfeedSeparationSlider.setLookAndFeel(&rotaryLookAndFeel);
    directGainSlider.setLookAndFeel(&directLookAndFeel);
    xfeedGainSlider.setLookAndFeel(&xfeedLookAndFeel);
    
    delaySliderAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (valueTreeState, DELAY_ID, delaySlider));
    freqSliderAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (valueTreeState, FREQ_ID, frequencySlider));
    xfeedFreqSliderAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (valueTreeState, XFREQ_ID, xfeedFrequencySlider));
    qSliderAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (valueTreeState, Q_ID, qSlider));
    xfeedQSliderAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (valueTreeState, XQ_ID, xfeedQSlider));
    sepSliderAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (valueTreeState, SEP_ID, separationSlider));
    xfeedSepSliderAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (valueTreeState, XSEP_ID, xfeedSeparationSlider));
    directGainSliderAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (valueTreeState, DGAIN_ID, directGainSlider));
    xfeedGainSliderAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (valueTreeState, XGAIN_ID, xfeedGainSlider));
    activeStateToggleButtonAttach.reset (new AudioProcessorValueTreeState::ButtonAttachment (valueTreeState, ACTIVE_ID, activeStateToggleButton));
    guiLayoutMenuAttach.reset (new AudioProcessorValueTreeState::ComboBoxAttachment (valueTreeState, SPECTR_ID, guiLayoutMenu));
    settingsMenuAttach.reset (new AudioProcessorValueTreeState::ComboBoxAttachment (valueTreeState, SETTINGS_ID, crossFeedMenu));


    filterGraphics.freqs[0] = frequencySlider.getValue();
    filterGraphics.freqs[1] = xfeedFrequencySlider.getValue();
    filterGraphics.gains[0] = xfeedGainSlider.getValue();
    filterGraphics.gains[1] = directGainSlider.getValue();
    filterGraphics.phases[0] = delaySlider.getValue();
    
    addAndMakeVisible(filterGraphics);
    addAndMakeVisible(spectrumAnalyser);
    
    /*
    Define settings for filters...
     */
    settingsDictionnary[String("Mode 1")]["delay"] = 160.0f;
    settingsDictionnary[String("Mode 1")]["directFreq"] = 647.0f;
    settingsDictionnary[String("Mode 1")]["directSep"] = 0.0f;
    settingsDictionnary[String("Mode 1")]["directGain"] = 0.0f;
    settingsDictionnary[String("Mode 1")]["directQ"] = 0.48f;
    settingsDictionnary[String("Mode 1")]["xfeedFreq"] = 355.0f;
    settingsDictionnary[String("Mode 1")]["xfeedSep"] = -3.2f;
    settingsDictionnary[String("Mode 1")]["xfeedGain"] = -3.2f;
    settingsDictionnary[String("Mode 1")]["xfeedQ"] = 0.58f;

    settingsDictionnary[String("Mode 2")]["delay"] = 44.0f;
    settingsDictionnary[String("Mode 2")]["directFreq"] = 609.0f;
    settingsDictionnary[String("Mode 2")]["directSep"] = -2.0f;
    settingsDictionnary[String("Mode 2")]["directGain"] = 1.8f;
    settingsDictionnary[String("Mode 2")]["directQ"] = 0.48f;
    settingsDictionnary[String("Mode 2")]["xfeedFreq"] = 355.0f;
    settingsDictionnary[String("Mode 2")]["xfeedSep"] = -3.2f;
    settingsDictionnary[String("Mode 2")]["xfeedGain"] = -3.2f;
    settingsDictionnary[String("Mode 2")]["xfeedQ"] = 0.42f;

    settingsDictionnary[String("Mode 3")]["delay"] = 8.0f;
    settingsDictionnary[String("Mode 3")]["directFreq"] = 434.8f;
    settingsDictionnary[String("Mode 3")]["directSep"] = -2.0f;
    settingsDictionnary[String("Mode 3")]["directGain"] = 2.1f;
    settingsDictionnary[String("Mode 3")]["directQ"] = 0.30f;
    settingsDictionnary[String("Mode 3")]["xfeedFreq"] = 300.0f;
    settingsDictionnary[String("Mode 3")]["xfeedSep"] = -3.2f;
    settingsDictionnary[String("Mode 3")]["xfeedGain"] = -2.1f;
    settingsDictionnary[String("Mode 3")]["xfeedQ"] = 0.63f;

    settingsDictionnary[String("Mode 4")]["delay"] = 178.71f;
    settingsDictionnary[String("Mode 4")]["directFreq"] = 647.0f;
    settingsDictionnary[String("Mode 4")]["directSep"] = -1.3f;
    settingsDictionnary[String("Mode 4")]["directGain"] = 0.2f;
    settingsDictionnary[String("Mode 4")]["directQ"] = 0.30f;
    settingsDictionnary[String("Mode 4")]["xfeedFreq"] = 1000.0f;
    settingsDictionnary[String("Mode 4")]["xfeedSep"] = -5.6f;
    settingsDictionnary[String("Mode 4")]["xfeedGain"] = -3.1f;
    settingsDictionnary[String("Mode 4")]["xfeedQ"] = 0.85f;

    settingsDictionnary[String("Mode 5")]["delay"] = 57.22f;
    settingsDictionnary[String("Mode 5")]["directFreq"] = 300.0f;
    settingsDictionnary[String("Mode 5")]["directSep"] = -3.0f;
    settingsDictionnary[String("Mode 5")]["directGain"] = 0.0f;
    settingsDictionnary[String("Mode 5")]["directQ"] = 0.48f;
    settingsDictionnary[String("Mode 5")]["xfeedFreq"] = 1000.0f;
    settingsDictionnary[String("Mode 5")]["xfeedSep"] = -3.0f;
    settingsDictionnary[String("Mode 5")]["xfeedGain"] = -3.0f;
    settingsDictionnary[String("Mode 5")]["xfeedQ"] = 0.48f;

    settingsDictionnary[String("Mode 6")]["delay"] = 100.81f;
    settingsDictionnary[String("Mode 6")]["directFreq"] = 406.80f;
    settingsDictionnary[String("Mode 6")]["directSep"] = -3.0f;
    settingsDictionnary[String("Mode 6")]["directGain"] = 0.0f;
    settingsDictionnary[String("Mode 6")]["directQ"] = 0.66f;
    settingsDictionnary[String("Mode 6")]["xfeedFreq"] = 595.37f;
    settingsDictionnary[String("Mode 6")]["xfeedSep"] = -3.0f;
    settingsDictionnary[String("Mode 6")]["xfeedGain"] = -3.0f;
    settingsDictionnary[String("Mode 6")]["xfeedQ"] = 0.48f;

    settingsDictionnary[String("Nice")]["delay"] = 122.38f;
    settingsDictionnary[String("Nice")]["directFreq"] = 647.0f;
    settingsDictionnary[String("Nice")]["directSep"] = -3.0f;
    settingsDictionnary[String("Nice")]["directGain"] = 3.7f;
    settingsDictionnary[String("Nice")]["directQ"] = 0.48f;
    settingsDictionnary[String("Nice")]["xfeedFreq"] = 647.0f;
    settingsDictionnary[String("Nice")]["xfeedSep"] = -3.0f;
    settingsDictionnary[String("Nice")]["xfeedGain"] = -3.2f;
    settingsDictionnary[String("Nice")]["xfeedQ"] = 0.48f;
    
    settingsDictionnary[String("Pure Haas")]["delay"] = 300.0f;
    settingsDictionnary[String("Pure Haas")]["directFreq"] = 300.0f;
    settingsDictionnary[String("Pure Haas")]["directSep"] = 0.0f;
    settingsDictionnary[String("Pure Haas")]["directGain"] = 0.0f;
    settingsDictionnary[String("Pure Haas")]["directQ"] = 0.0f;
    settingsDictionnary[String("Pure Haas")]["xfeedFreq"] = 300.0f;
    settingsDictionnary[String("Pure Haas")]["xfeedSep"] = 0.0f;
    settingsDictionnary[String("Pure Haas")]["xfeedGain"] = -4.0f;
    settingsDictionnary[String("Pure Haas")]["xfeedQ"] = 0.0f;

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
    directSliders = topPanel.removeFromTop(topPanel.getHeight() / 2);
    xfeedSliders  = topPanel.removeFromTop(topPanel.getHeight());
    slider1 = directSliders.removeFromLeft(sliderSize);
    slider2 = directSliders.removeFromLeft(sliderSize);
    slider3 = directSliders.removeFromLeft(sliderSize);
    slider4 = xfeedSliders.removeFromLeft(sliderSize);
    slider5 = xfeedSliders.removeFromLeft(sliderSize);
    slider6 = xfeedSliders.removeFromLeft(sliderSize);

    int dialSize = dials.getHeight() - 3 * (LABELHEIGHT + TEXTBOXHEIGT);
    
    directGainLabel.setBounds(dials.removeFromTop(LABELHEIGHT));
    directGainSlider.setBounds(dials.removeFromTop(dialSize / 3 + TEXTBOXHEIGT));
    delayLabel.setBounds(dials.removeFromTop(LABELHEIGHT));
    delaySlider.setBounds(dials.removeFromTop(dialSize / 3 + TEXTBOXHEIGT));
    xfeedGainLabel.setBounds(dials.removeFromTop(LABELHEIGHT));
    xfeedGainSlider.setBounds(dials.removeFromTop(dialSize / 3 + TEXTBOXHEIGT));
    
    frequencyLabel.setBounds(slider1.removeFromTop(LABELHEIGHT));
    frequencySlider.setBounds(slider1);
    separationLabel.setBounds(slider2.removeFromTop(LABELHEIGHT));
    separationSlider.setBounds(slider2);
    qLabel.setBounds(slider3.removeFromTop(LABELHEIGHT));
    qSlider.setBounds(slider3);

    xfeedFrequencyLabel.setBounds(slider4.removeFromTop(LABELHEIGHT));
    xfeedFrequencySlider.setBounds(slider4);
    xfeedSeparationLabel.setBounds(slider5.removeFromTop(LABELHEIGHT));
    xfeedSeparationSlider.setBounds(slider5);
    xfeedQLabel.setBounds(slider6.removeFromTop(LABELHEIGHT));
    xfeedQSlider.setBounds(slider6);
    
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
        //auto itemsNumber = crossFeedMenu.getNumItems();
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
                
                crossFeedMenu.addItem(comboboxText, itemsNumber + 1);
                settingsDictionnary[comboboxText] = {(float) delaySlider.getValue() ,
                                                     (float) frequencySlider.getValue() ,
                                                     (float) separationSlider.getValue() ,
                                                     (float) qSlider.getValue() ,
                                                     (float) directGainSlider.getValue() ,
                                                     (float) xfeedGainSlider.getValue()};
                crossFeedMenu.setSelectedId(crossFeedMenu.getNumItems());
                
                for (int i = 0; i < crossFeedMenu.getNumItems(); ++i)
                {
                    DBG("Label, Index, i: " << crossFeedMenu.getItemText(i) << ", " << crossFeedMenu.indexOfItemId(i) << ", " << i);
                }
                */
            }
            else
            {
                /*
                settingsDictionnary[String("Nice")]["delay"] = 122.38f;
                settingsDictionnary[String("Nice")]["directFreq"] = 647.0f;
                settingsDictionnary[String("Nice")]["directSep"] = -3.0f;
                settingsDictionnary[String("Nice")]["directGain"] = 3.7f;
                settingsDictionnary[String("Nice")]["directQ"] = 0.48f;
                settingsDictionnary[String("Nice")]["xfeedFreq"] = 647.0f;
                settingsDictionnary[String("Nice")]["xfeedSep"] = -3.0f;
                settingsDictionnary[String("Nice")]["xfeedGain"] = -3.2f;
                settingsDictionnary[String("Nice")]["xfeedQ"] = 0.48f;
                */
                
                delaySlider.setValue(settingsDictionnary[comboboxText]["delay"]);
                frequencySlider.setValue(settingsDictionnary[comboboxText]["directFreq"]);
                xfeedFrequencySlider.setValue(settingsDictionnary[comboboxText]["xfeedFreq"]);
                separationSlider.setValue(settingsDictionnary[comboboxText]["directSep"]);
                xfeedSeparationSlider.setValue(settingsDictionnary[comboboxText]["xfeedSep"]);
                qSlider.setValue(settingsDictionnary[comboboxText]["directQ"]);
                xfeedQSlider.setValue(settingsDictionnary[comboboxText]["xfeedQ"]);
                directGainSlider.setValue(settingsDictionnary[comboboxText]["directGain"]);
                xfeedGainSlider.setValue(settingsDictionnary[comboboxText]["xfeedGain"]);
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
