#pragma once

#include "JuceHeader.h"
#include "ChannelStrip.h"
#include "AudioEngine.h"


class MixerPanel : public juce::Component
{
public:
    MixerPanel();
    ~MixerPanel() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void setAudioEngine(AudioEngine* engine);
    void addChannel();
    void removeChannel(int index);

private:
    AudioEngine* audioEngine{nullptr};
    std::vector<std::unique_ptr<ChannelStrip>> channelStrips;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerPanel)
};