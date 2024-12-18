#include "MainComponent.h"

MainComponent::MainComponent()
    : audioDeviceSelector("Audio Device"),
      volumeSlider("Volume")
{
    audioEngine = std::make_unique<AudioEngine>();
    
    // Setup audio device selector with available devices
    addAndMakeVisible(audioDeviceSelector);
    audioDeviceSelector.addListener(this);
    initialiseAudioDeviceSelector();
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


void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (!audioEngine)
        return;

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

    // Update the main level meter
    if (numOutputChannels >= 2)
    {
        float leftLevel = bufferToFill.buffer->getRMSLevel(0, 0, numSamples);
        float rightLevel = bufferToFill.buffer->getRMSLevel(1, 0, numSamples);
        levelMeter.setLevel(leftLevel, rightLevel);
    }
}

void MainComponent::releaseResources() {
    audioEngine->releaseResources();
}



void MainComponent::initialiseAudioDeviceSelector()
{
    audioDeviceSelector.clear();
    audioDeviceSelector.addItem("System Default", 1);
    
    if (audioEngine)
    {
        auto& deviceManager = const_cast<juce::AudioDeviceManager&>(audioEngine->getDeviceManager());
        auto& deviceTypes = deviceManager.getAvailableDeviceTypes();
        
        if (!deviceTypes.isEmpty())
        {
            auto names = deviceTypes[0]->getDeviceNames();
            int itemId = 2;
            for (const auto& name : names)
            {
                audioDeviceSelector.addItem(name, itemId++);
            }
        }
        
        // Select current device
        auto* currentDevice = deviceManager.getCurrentAudioDevice();
        if (currentDevice != nullptr)
        {
            const juce::String currentName = currentDevice->getName();
            for (int i = 0; i < audioDeviceSelector.getNumItems(); ++i)
            {
                if (audioDeviceSelector.getItemText(i) == currentName)
                {
                    audioDeviceSelector.setSelectedItemIndex(i);
                    break;
                }
            }
        }
    }
}

void MainComponent::comboBoxChanged(juce::ComboBox* box) {
    if (box == &audioDeviceSelector && audioEngine)
    {
        if (audioDeviceSelector.getSelectedId() == 1)
        {
            // System Default selected
            audioEngine->initialiseAudio();
        }
        else
        {
            juce::String deviceName = audioDeviceSelector.getText();
            double sampleRate = audioEngine->getCurrentSampleRate();
            int bufferSize = audioEngine->getCurrentBufferSize();
            
            // Change device with current settings
            audioEngine->setAudioDevice(deviceName, sampleRate, bufferSize);
        }
    }
}

void MainComponent::sliderValueChanged(juce::Slider* slider) {
    if (slider == &volumeSlider) {
        audioEngine->setMainVolume(volumeSlider.getValue());
    }
}

void MainComponent::timerCallback() {
    if (audioEngine)
    {
        float leftLevel = audioEngine->getLeftLevel();
        float rightLevel = audioEngine->getRightLevel();
        levelMeter.setLevel(leftLevel, rightLevel);
    }
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

