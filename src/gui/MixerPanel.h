#pragma once

#include "JuceHeader.h"
#include "ChannelStrip.h"


class MixerPanel : public juce::Component
{
public:
    MixerPanel();
    ~MixerPanel() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    std::vector<std::unique_ptr<ChannelStrip>> channelStrips;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerPanel)
};