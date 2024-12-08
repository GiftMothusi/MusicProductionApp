#include "AudioEngine.h"

AudioEngine::AudioEngine()
    : currentSampleRate(0.0)
    , currentBlockSize(0)
{
    // Initialize the device manager with default audio device
    auto error = deviceManager.initialise(2,     // input channels
                                        2,     // output channels
                                        nullptr, // no XML settings
                                        true);   // select default device

    // In debug builds, assert that initialization succeeded
    jassert(error.isEmpty());

    deviceManager.addAudioCallback(this);
}

AudioEngine::~AudioEngine()
{
    deviceManager.removeAudioCallback(this);
}

void AudioEngine::audioDeviceIOCallback(const float** inputChannelData,
                                      int numInputChannels,
                                      float** outputChannelData,
                                      int numOutputChannels,
                                      int numSamples)
{
    // For now, just clear the outputs (silence)
    for (int channel = 0; channel < numOutputChannels; ++channel)
    {
        if (outputChannelData[channel] != nullptr)
        {
            juce::FloatVectorOperations::clear(outputChannelData[channel], numSamples);
        }
    }
}

void AudioEngine::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    currentSampleRate = device->getCurrentSampleRate();
    currentBlockSize = device->getCurrentBufferSizeSamples();
}

void AudioEngine::audioDeviceStopped()
{
    currentSampleRate = 0.0;
    currentBlockSize = 0;
}