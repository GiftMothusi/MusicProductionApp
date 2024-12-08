// In AudioEngine.h
#pragma once

#include "JuceHeader.h"

class AudioEngine : public juce::AudioIODeviceCallback  // Changed from AudioSource
{
public:
    AudioEngine();
    ~AudioEngine() override;

    // AudioIODeviceCallback methods
    void audioDeviceIOCallback(const float** inputChannelData,
                             int numInputChannels,
                             float** outputChannelData,
                             int numOutputChannels,
                             int numSamples);
    
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

private:
    juce::AudioDeviceManager deviceManager;
    double currentSampleRate;
    int currentBlockSize;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};