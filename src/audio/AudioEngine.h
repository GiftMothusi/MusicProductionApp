#pragma once

#include <JuceHeader.h>
#include "ChannelAudioProcessor.h"
#include "AudioRouting.h"

class AudioEngine : public juce::AudioIODeviceCallback,
                    public juce::ChangeBroadcaster,
                    public juce::ChangeListener  
{
public:
    AudioEngine();
    ~AudioEngine() override;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    // Device Management
    void initialiseAudio();
    void shutdownAudio();
    bool setAudioDevice(const juce::String& deviceName, double sampleRate, int bufferSize);

    // AudioIODeviceCallback methods
    void audioDeviceIOCallback(const float** inputChannelData,
                               int numInputChannels,
                               float** outputChannelData,
                               int numOutputChannels,
                               int numSamples);

    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

    // Channel Management
    void addChannel();
    void removeChannel(int index);
    int getNumChannels() const { return channelProcessors.size(); }
    ChannelAudioProcessor* getChannelProcessor(int index);

    // Getters for audio state
    bool isAudioInitialised() const { return audioInitialised; }
    double getCurrentSampleRate() const { return currentSampleRate; }
    int getCurrentBufferSize() const { return currentBlockSize; }
    const juce::AudioDeviceManager& getDeviceManager() const { return deviceManager; }

    // Channel Management
    void setMainVolume(float newVolume);
    float getMainVolume() const { return mainVolume.load(); }
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    // New methods to get left and right channel levels
    float getLeftLevel() const;
    float getRightLevel() const;

private:
    AudioRouting router; 
    juce::AudioDeviceManager deviceManager;
    std::vector<std::unique_ptr<ChannelAudioProcessor>> channelProcessors;
    juce::AudioBuffer<float> mixBuffer;

    std::atomic<double> currentSampleRate{0.0};
    std::atomic<int> currentBlockSize{0};
    std::atomic<bool> audioInitialised{false};
    std::atomic<float> mainVolume{1.0f};

    juce::CriticalSection processAudioLock;
   

    // New members to track left and right channel levels
    std::atomic<float> leftLevel{0.0f};
    std::atomic<float> rightLevel{0.0f};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};
