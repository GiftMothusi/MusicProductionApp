#pragma once

#include <JuceHeader.h>
#include "ChannelAudioProcessor.h" 

class ChannelStrip : public juce::Component,
                    public juce::Slider::Listener,
                    public juce::ComboBox::Listener,
                    public juce::Button::Listener,
                    public juce::Timer
{
public:
    ChannelStrip();
    ~ChannelStrip() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // Listeners
    void sliderValueChanged(juce::Slider* slider) override;
    void comboBoxChanged(juce::ComboBox* box) override;
    void buttonClicked(juce::Button* button) override;

    // Audio parameter setters/getters
    void setInputSource(int sourceIndex);
    void setOutputDestination(int destinationIndex);
    float getVolume() const;
    float getPan() const;
    bool isMuted() const;
    bool isSoloed() const;
    void updateMeterLevel(float level);

    void setAudioProcessor(ChannelAudioProcessor* newProcessor)
    {
       
        processor = newProcessor;  // Just store the pointer
        updateProcessorFromUI();
    }

    void timerCallback() override;

private:

    ChannelAudioProcessor* processor{nullptr};  // Non-owning pointer

    // Setup methods
    void createAndSetupSliders();
    void createAndSetupButtons();
    void createAndSetupLabels();
    void createAndSetupMeters();
    void updateProcessorFromUI();
    void updateMetersFromProcessor();

    // GUI Elements
    juce::Slider fader;           // Main volume fader
    juce::Slider panKnob;         // Pan control
    juce::Slider gainKnob;        // Input gain control
    std::array<juce::Slider, 3> eqKnobs;  // Basic 3-band EQ

    juce::TextButton muteButton;
    juce::TextButton soloButton;
    juce::TextButton recButton;   // Record arm button

    juce::ComboBox inputSelector;
    juce::ComboBox outputSelector;

    juce::Label channelLabel;
    juce::Label volumeLabel;
    juce::Label panLabel;
    juce::Label gainLabel;
    std::array<juce::Label, 3> eqLabels;

    float meterLevel{0.0f};       // Current meter level
    bool wasJustDragged{false};   // Helper for fader interaction

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelStrip)
};