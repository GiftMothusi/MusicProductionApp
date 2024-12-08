#include "MixerPanel.h"

MixerPanel::MixerPanel()
{
    // setOpaque(true);
    for (int i = 0; i < 4; ++i)
    {
        auto channelStrip = std::make_unique<ChannelStrip>();
        addAndMakeVisible(*channelStrip);
        channelStrips.push_back(std::move(channelStrip));
    }
}

MixerPanel::~MixerPanel()
{
}

void MixerPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    // g.setColour(juce::Colours::white);
    // g.drawText("Mixer Panel", getLocalBounds(), juce::Justification::centred);
}

void MixerPanel::resized()
{
    // Layout mixer controls here
    int padding = 10;
    int channelWidth = (getWidth() - (channelStrips.size() + 1) * padding) / channelStrips.size();
    int channelHeight = getHeight() - 2 * padding;

    for (size_t i = 0; i < channelStrips.size(); ++i)
    {
        channelStrips[i]->setBounds(padding + i * (channelWidth + padding), padding, channelWidth, channelHeight);
    }
}