#include "AudioEngine.h"

AudioEngine::AudioEngine()
{
    initialiseAudio();
}

AudioEngine::~AudioEngine()
{
    shutdownAudio();
}

void AudioEngine::initialiseAudio()
{
    auto errorMessage = deviceManager.initialiseWithDefaultDevices(2, 2);
    
    if (errorMessage.isEmpty())
    {
        deviceManager.addAudioCallback(this);
        audioInitialised = true;
    }
    else
    {
        juce::Logger::writeToLog("Audio initialization failed: " + errorMessage);
    }
}

void AudioEngine::shutdownAudio()
{
    deviceManager.removeAudioCallback(this);
    deviceManager.closeAudioDevice();
    audioInitialised = false;
}

bool AudioEngine::setAudioDevice(const juce::String& deviceName, double sampleRate, int bufferSize)
{
    auto currentDevice = deviceManager.getCurrentAudioDevice();
    if (currentDevice != nullptr && currentDevice->getName() == deviceName)
        return true;

    juce::AudioDeviceManager::AudioDeviceSetup setup;
    deviceManager.getAudioDeviceSetup(setup);
    
    setup.outputDeviceName = deviceName;
    setup.sampleRate = sampleRate;
    setup.bufferSize = bufferSize;

    auto error = deviceManager.setAudioDeviceSetup(setup, true);
    return error.isEmpty();
}

void AudioEngine::audioDeviceIOCallback(const float** inputChannelData,
                                        int numInputChannels,
                                        float** outputChannelData,
                                        int numOutputChannels,
                                        int numSamples)
{
    const juce::ScopedLock sl(processAudioLock);

    // Clear outputs first
    for (int channel = 0; channel < numOutputChannels; ++channel)
        if (outputChannelData[channel] != nullptr)
            juce::FloatVectorOperations::clear(outputChannelData[channel], numSamples);

    // Process audio and apply main volume
    if (numOutputChannels > 0)
    {
        const float currentMainVolume = mainVolume.load();

        for (int channel = 0; channel < numOutputChannels; ++channel)
        {
            if (outputChannelData[channel] != nullptr)
            {
                if (channel < numInputChannels && inputChannelData[channel] != nullptr)
                {
                    juce::FloatVectorOperations::multiply(outputChannelData[channel],
                                                          inputChannelData[channel],
                                                          currentMainVolume,
                                                          numSamples);
                    
                    // Calculate RMS levels for left (channel 0) and right (channel 1)
                    float rmsLevel = juce::FloatVectorOperations::findMinAndMax(inputChannelData[channel], numSamples).getEnd();
                    if (channel == 0)
                        leftLevel.store(rmsLevel);
                    if (channel == 1)
                        rightLevel.store(rmsLevel);
                }
            }
        }
    }
}

void AudioEngine::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    currentSampleRate = device->getCurrentSampleRate();
    currentBlockSize = device->getCurrentBufferSizeSamples();

    mixBuffer.setSize(2, currentBlockSize);
    mixBuffer.clear();
}

void AudioEngine::audioDeviceStopped()
{
    currentSampleRate = 0.0;
    currentBlockSize = 0;
    mixBuffer.setSize(0, 0);
}

void AudioEngine::setMainVolume(float newVolume)
{
    mainVolume.store(juce::jlimit(0.0f, 1.0f, newVolume));
}

void AudioEngine::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // Implement the prepareToPlay logic here
}

void AudioEngine::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Implement the getNextAudioBlock logic here
}

void AudioEngine::releaseResources()
{
    // Implement the releaseResources logic here
}

float AudioEngine::getLeftLevel() const
{
    return leftLevel.load();
}

float AudioEngine::getRightLevel() const
{
    return rightLevel.load();
}
