#include "MainComponent.h"

MainComponent::MainComponent()
    : audioDeviceSelector("Audio Device"),
      volumeSlider("Volume")
{
    // Setup audio device selector
    audioDeviceSelector.addListener(this);
    addAndMakeVisible(audioDeviceSelector);

    // Setup volume slider
    volumeSlider.setRange(0.0, 1.0);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    // Add the level meter
    addAndMakeVisible(levelMeter);

    // Add the mixer panel
    addAndMakeVisible(*mixerPanel);

    // Start timer for meter updates
    startTimer(50);
}

MainComponent::~MainComponent() {
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    audioEngine->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    audioEngine->getNextAudioBlock(bufferToFill);
    levelMeter.setLevel(audioEngine->getLeftLevel(), audioEngine->getRightLevel());
}

void MainComponent::releaseResources() {
    audioEngine->releaseResources();
}

void MainComponent::comboBoxChanged(juce::ComboBox* box) {
    if (box == &audioDeviceSelector) {
        int selectedIndex = audioDeviceSelector.getSelectedItemIndex();
        audioEngine->setAudioDevice(audioEngine->getDeviceManager().getAudioDeviceSetup().outputDeviceName, audioEngine->getCurrentSampleRate(), audioEngine->getCurrentBufferSize());
    }
}

void MainComponent::sliderValueChanged(juce::Slider* slider) {
    if (slider == &volumeSlider) {
        audioEngine->setMainVolume(volumeSlider.getValue());
    }
}

void MainComponent::timerCallback() {
    repaint();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey); // Fill background with a dark grey color
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawText("Music Production App", getLocalBounds(), juce::Justification::centred, true);
}

void MainComponent::resized()
{
    audioDeviceSelector.setBounds(10, 10, getWidth() - 20, 30);
    volumeSlider.setBounds(10, 50, getWidth() - 20, 30);
    levelMeter.setBounds(10, 90, getWidth() - 20, 100);
    mixerPanel->setBounds(10, 200, getWidth() - 20, getHeight() - 210);
}

