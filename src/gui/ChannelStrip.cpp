#include "ChannelStrip.h"

ChannelStrip::ChannelStrip()
{
    // Volume Slider
    volumeSlider.setRange(0.0, 1.0);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    // Pan Slider
    panSlider.setRange(-1.0, 1.0); // -1.0 = Left, 1.0 = Right
    panSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    panSlider.addListener(this);
    addAndMakeVisible(panSlider);

    // Input Selector
    inputSelector.addItem("Input 1", 1);
    inputSelector.addItem("Input 2", 2);
    inputSelector.addListener(this);
    addAndMakeVisible(inputSelector);

    // Output Selector
    outputSelector.addItem("Output 1", 1);
    outputSelector.addItem("Output 2", 2);
    outputSelector.addListener(this);
    addAndMakeVisible(outputSelector);
}

ChannelStrip::~ChannelStrip() {}

void ChannelStrip::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::grey);
    g.setColour(juce::Colours::white);
    g.setFont(14.0f);
    g.drawText("Channel Strip", getLocalBounds(), juce::Justification::centredTop);
}

void ChannelStrip::resized()
{
    int padding = 10;
    int width = getWidth() - 2 * padding;
    int height = 40;

    inputSelector.setBounds(padding, padding, width, height);
    volumeSlider.setBounds(padding, padding + 50, width, height);
    panSlider.setBounds(padding, padding + 100, width, height);
    outputSelector.setBounds(padding, padding + 150, width, height);
}

void ChannelStrip::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        // Handle volume change
        DBG("Volume: " << volumeSlider.getValue());
    }
    else if (slider == &panSlider)
    {
        // Handle pan change
        DBG("Pan: " << panSlider.getValue());
    }
}

void ChannelStrip::comboBoxChanged(juce::ComboBox* box)
{
    if (box == &inputSelector)
    {
        DBG("Input Source: " << inputSelector.getSelectedId());
    }
    else if (box == &outputSelector)
    {
        DBG("Output Destination: " << outputSelector.getSelectedId());
    }
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
    return volumeSlider.getValue();
}

float ChannelStrip::getPan() const
{
    return panSlider.getValue();
}
