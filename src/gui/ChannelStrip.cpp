#include "ChannelStrip.h"

ChannelStrip::ChannelStrip()
{
    processor = nullptr;
    meterLevel = 0.0f;
    wasJustDragged = false;

    createAndSetupSliders();
    createAndSetupButtons();
    createAndSetupLabels();
    createAndSetupMeters();

    startTimerHz(30);
}

ChannelStrip::~ChannelStrip()
{
    // Nothing specific needed here, but we must define it
}

void ChannelStrip::createAndSetupSliders()
{
    // Fader setup
    fader.setSliderStyle(juce::Slider::LinearVertical);
    fader.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    fader.setRange(-60.0, 6.0, 0.1);  // dB range
    fader.setValue(0.0);
    fader.setSkewFactorFromMidPoint(-12.0);  // Natural feeling volume control
    fader.addListener(this);
    addAndMakeVisible(fader);

    // Pan knob setup
    panKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    panKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    panKnob.setRange(-1.0, 1.0);
    panKnob.setValue(0.0);
    panKnob.addListener(this);
    addAndMakeVisible(panKnob);

    // Gain knob setup
    gainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    gainKnob.setRange(-12.0, 12.0);
    gainKnob.setValue(0.0);
    gainKnob.addListener(this);
    addAndMakeVisible(gainKnob);

    // EQ knobs setup
    for (auto& knob : eqKnobs)
    {
        knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        knob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        knob.setRange(-12.0, 12.0);
        knob.setValue(0.0);
        knob.addListener(this);
        addAndMakeVisible(knob);
    }
}

void ChannelStrip::createAndSetupButtons()
{
    // Mute button
    muteButton.setButtonText("M");
    muteButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::red);
    muteButton.setClickingTogglesState(true);
    muteButton.addListener(this);
    addAndMakeVisible(muteButton);

    // Solo button
    soloButton.setButtonText("S");
    soloButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::yellow);
    soloButton.setClickingTogglesState(true);
    soloButton.addListener(this);
    addAndMakeVisible(soloButton);

    // Record button
    recButton.setButtonText("R");
    recButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::red);
    recButton.setClickingTogglesState(true);
    recButton.addListener(this);
    addAndMakeVisible(recButton);
}

void ChannelStrip::createAndSetupLabels()
{
    channelLabel.setText("Channel 1", juce::dontSendNotification);
    channelLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(channelLabel);

    volumeLabel.setText("Volume", juce::dontSendNotification);
    volumeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(volumeLabel);

    panLabel.setText("Pan", juce::dontSendNotification);
    panLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(panLabel);

    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(gainLabel);

    const char* eqNames[] = { "Low", "Mid", "High" };
    for (int i = 0; i < 3; ++i)
    {
        eqLabels[i].setText(eqNames[i], juce::dontSendNotification);
        eqLabels[i].setJustificationType(juce::Justification::centred);
        addAndMakeVisible(eqLabels[i]);
    }
}

void ChannelStrip::createAndSetupMeters()
{
    // Initially implemented as an empty method since meter visualization
    // is currently handled in the paint method
    // This method can be expanded later to create more sophisticated meters
}

void ChannelStrip::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Background
    g.setColour(juce::Colours::darkgrey);
    g.fillAll();

    // Border
    g.setColour(juce::Colours::grey);
    g.drawRect(bounds, 1);

    // Level meter background
    auto meterBounds = bounds.removeFromRight(20).reduced(2);
    g.setColour(juce::Colours::black);
    g.fillRect(meterBounds);

    // Draw meter level
    if (meterLevel > 0.0f)
    {
        auto levelBounds = meterBounds.removeFromBottom(
            (int)(meterBounds.getHeight() * meterLevel));

        // Create gradient based on level
        if (meterLevel > 0.8f)
        {
            g.setColour(juce::Colours::red);
        }
        else if (meterLevel > 0.6f)
        {
            g.setColour(juce::Colours::yellow);
        }
        else
        {
            g.setColour(juce::Colours::green);
        }
        
        g.fillRect(levelBounds);
    }
}


void ChannelStrip::setAudioProcessor(ChannelAudioProcessor* newProcessor)
{
    if (!newProcessor) return;
    
    processor = newProcessor;
    updateProcessorFromUI();
}

void ChannelStrip::resized()
{
    auto bounds = getLocalBounds().reduced(4);
    const int buttonHeight = 20;
    const int knobSize = 60;
    const int labelHeight = 20;
    const int spacing = 4;

    // Channel label at top
    channelLabel.setBounds(bounds.removeFromTop(labelHeight));

    // Top row buttons
    auto buttonRow = bounds.removeFromTop(buttonHeight);
    const int buttonWidth = (buttonRow.getWidth() - spacing * 2) / 3;
    muteButton.setBounds(buttonRow.removeFromLeft(buttonWidth));
    buttonRow.removeFromLeft(spacing);
    soloButton.setBounds(buttonRow.removeFromLeft(buttonWidth));
    buttonRow.removeFromLeft(spacing);
    recButton.setBounds(buttonRow);

    bounds.removeFromTop(spacing * 2);

    // Gain and Pan controls
    auto topControls = bounds.removeFromTop(knobSize + labelHeight);
    auto gainArea = topControls.removeFromLeft(topControls.getWidth() / 2);
    gainLabel.setBounds(gainArea.removeFromTop(labelHeight));
    gainKnob.setBounds(gainArea);

    panLabel.setBounds(topControls.removeFromTop(labelHeight));
    panKnob.setBounds(topControls);

    bounds.removeFromTop(spacing * 2);

    // EQ controls
    auto eqRow = bounds.removeFromTop(knobSize + labelHeight);
    const int eqWidth = (eqRow.getWidth() - spacing * 2) / 3;
    
    for (int i = 0; i < 3; ++i)
    {
        auto eqArea = eqRow.removeFromLeft(eqWidth);
        if (i < 2) eqRow.removeFromLeft(spacing);
        
        eqLabels[i].setBounds(eqArea.removeFromTop(labelHeight));
        eqKnobs[i].setBounds(eqArea);
    }

    bounds.removeFromTop(spacing * 4);

    // Main fader
    volumeLabel.setBounds(bounds.removeFromTop(labelHeight));
    fader.setBounds(bounds);
}

void ChannelStrip::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &fader)
    {
        // Convert dB to linear gain
        float gainLinear = juce::Decibels::decibelsToGain(static_cast<float>(fader.getValue()));
        // Handle volume change
        DBG("Volume (dB): " << fader.getValue() << ", Linear: " << gainLinear);
    }
    else if (slider == &panKnob)
    {
        DBG("Pan: " << panKnob.getValue());
    }
    else if (slider == &gainKnob)
    {
        DBG("Input Gain: " << gainKnob.getValue() << " dB");
    }
    // Handle EQ knobs...
    updateProcessorFromUI();
}

void ChannelStrip::buttonClicked(juce::Button* button)
{
    juce::String debugMessage;
    
    if (button == &muteButton)
    {
        debugMessage << "Mute: " << (muteButton.getToggleState() ? "On" : "Off");
        DBG(debugMessage);
    }
    else if (button == &soloButton)
    {
        debugMessage << "Solo: " << (soloButton.getToggleState() ? "On" : "Off");
        DBG(debugMessage);
    }
    else if (button == &recButton)
    {
        debugMessage << "Record: " << (recButton.getToggleState() ? "On" : "Off");
        DBG(debugMessage);
    }
    updateProcessorFromUI();
}

void ChannelStrip::comboBoxChanged(juce::ComboBox* box)
{
    if (box == &inputSelector)
    {
        DBG("Input changed to: " << box->getSelectedId());
    }
    else if (box == &outputSelector)
    {
        DBG("Output changed to: " << box->getSelectedId());
    }
}

void ChannelStrip::updateMeterLevel(float level)
{
   // Convert level to dB for better visualization
    float dbLevel = juce::Decibels::gainToDecibels(level, -60.0f);
    meterLevel = juce::jmap(dbLevel, -60.0f, 6.0f, 0.0f, 1.0f);
    
    // Ensure the level is within bounds
    meterLevel = juce::jlimit(0.0f, 1.0f, meterLevel);
}

void ChannelStrip::setInputSource(int sourceIndex)
{
    inputSelector.setSelectedId(sourceIndex);
}

void ChannelStrip::setOutputDestination(int destinationIndex)
{
    outputSelector.setSelectedId(destinationIndex);
}

float ChannelStrip::getVolume() const
{
    return fader.getValue();
}

float ChannelStrip::getPan() const
{
    return panKnob.getValue();
}

bool ChannelStrip::isMuted() const
{
    return muteButton.getToggleState();
}

bool ChannelStrip::isSoloed() const
{
    return soloButton.getToggleState();
}

void ChannelStrip::updateProcessorFromUI()
{
    if (processor)
    {
        processor->setGain(static_cast<float>(gainKnob.getValue()));
        processor->setPan(static_cast<float>(panKnob.getValue()));
        processor->setVolume(static_cast<float>(fader.getValue()));
        processor->setMute(muteButton.getToggleState());
        processor->setSolo(soloButton.getToggleState());
    }
}

void ChannelStrip::updateMetersFromProcessor()
{
    if (processor)
    {
        updateMeterLevel(processor->getCurrentLevel());
        
        // Add visual feedback for clipping if needed
        if (processor->isClipping())
        {
            // Change the meter color or add an indicator here
        }
    }
}


void ChannelStrip::timerCallback()
{
    if (processor)
    {
        // Get the current level from the processor
        float level = processor->getCurrentLevel();
        
        // Update meter level
        updateMeterLevel(level);
        
        // Handle peak indicators if needed
        if (processor->isClipping())
        {
            // You could add visual feedback for clipping here
            // For example, change the meter color temporarily
        }
        
        repaint();
    }
}