#include "MainComponent.h"

MainComponent::MainComponent()
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

    // Initialize audio engine
    audioEngine = std::make_unique<AudioEngine>();

    // Add the mixer panel and connect it to the audio engine
    mixerPanel = std::make_unique<MixerPanel>();
    mixerPanel->setAudioEngine(audioEngine.get());
    addAndMakeVisible(*mixerPanel);

    // Start timer for meter updates
    startTimer(50);
}


MainComponent::~MainComponent() {
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    if (audioEngine)
    {
        juce::String device = audioEngine->getDeviceManager().getCurrentAudioDevice()->getName();
        audioEngine->setAudioDevice(device, sampleRate, samplesPerBlockExpected);
    }
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    // Get audio input and handle const-correctness
    const float** inputChannelData = const_cast<const float**>(bufferToFill.buffer->getArrayOfReadPointers());
    float** outputChannelData = const_cast<float**>(bufferToFill.buffer->getArrayOfWritePointers());
    const int numInputChannels = bufferToFill.buffer->getNumChannels();
    const int numOutputChannels = bufferToFill.buffer->getNumChannels();
    const int numSamples = bufferToFill.numSamples;

    // Process audio through engine
    audioEngine->audioDeviceIOCallback(
        inputChannelData,
        numInputChannels,
        outputChannelData,
        numOutputChannels,
        numSamples
    );

    // Calculate RMS levels for the main meter
    float leftLevel = 0.0f;
    float rightLevel = 0.0f;

    if (numOutputChannels >= 1)
        leftLevel = bufferToFill.buffer->getRMSLevel(0, 0, numSamples);
    if (numOutputChannels >= 2)
        rightLevel = bufferToFill.buffer->getRMSLevel(1, 0, numSamples);

    // Update the main level meter
    levelMeter.setLevel(leftLevel, rightLevel);
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

