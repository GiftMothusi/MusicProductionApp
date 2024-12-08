#pragma once

#include <JuceHeader.h>

class ChannelStrip : public juce::Component, public juce::Slider::Listener, public juce::ComboBox::Listener
{
public:
    ChannelStrip();
    ~ChannelStrip() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // Slider and ComboBox callbacks
    void sliderValueChanged(juce::Slider* slider) override;
    void comboBoxChanged(juce::ComboBox* box) override;

    // Set audio input and output routing
    void setInputSource(int sourceIndex);
    void setOutputDestination(int destinationIndex);

    // Get volume and pan values
    float getVolume() const;
    float getPan() const;

private:
    // GUI Elements
    juce::Slider volumeSlider;
    juce::Slider panSlider;
    juce::ComboBox inputSelector;
    juce::ComboBox outputSelector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelStrip)
};
