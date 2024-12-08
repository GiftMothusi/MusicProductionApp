#include "MixerPanel.h"

MixerPanel::MixerPanel()
{
    setOpaque(true);
}

MixerPanel::~MixerPanel()
{
}

void MixerPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.drawText("Mixer Panel", getLocalBounds(), juce::Justification::centred);
}

void MixerPanel::resized()
{
    // Layout mixer controls here
}