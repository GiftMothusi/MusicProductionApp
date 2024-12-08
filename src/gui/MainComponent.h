#pragma once

#include <JuceHeader.h>
#include "audio/AudioEngine.h"
#include "gui/SimpleLevelMeter.h"
#include "gui/MixerPanel.h"

class MainComponent : public juce::AudioAppComponent,
                      public juce::ComboBox::Listener,
                      public juce::Slider::Listener,
                      public juce::Timer {
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void comboBoxChanged(juce::ComboBox* box) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void timerCallback() override;

    void paint(juce::Graphics& g) override;

    void resized() override;



private:
    std::unique_ptr<AudioEngine> audioEngine = std::make_unique<AudioEngine>();
    std::unique_ptr<MixerPanel> mixerPanel = std::make_unique<MixerPanel>();
    SimpleLevelMeter levelMeter;

    juce::ComboBox audioDeviceSelector;
    juce::Slider volumeSlider;
};