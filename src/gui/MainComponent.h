#pragma once

#include "JuceHeader.h"
#include "MixerPanel.h"
#include "../audio/AudioEngine.h"
#include "../audio/MidiHandler.h"

class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override = default;  // Changed to default implementation

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    std::unique_ptr<AudioEngine> audioEngine;
    std::unique_ptr<MidiHandler> midiHandler;
    std::unique_ptr<MixerPanel> mixerPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};