#pragma once

#include "JuceHeader.h"

class MixerPanel : public juce::Component
{
public:
    MixerPanel();
    ~MixerPanel() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerPanel)
};