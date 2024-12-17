#pragma once

#include <JuceHeader.h>

class ChannelAudioProcessor
{
public:
    ChannelAudioProcessor();
    
    void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock);
    void processBlock(juce::AudioBuffer<float>& buffer);
    void releaseResources();

    // Parameter setters
    void setGain(float newGain);
    void setPan(float newPan);        // -1.0 = left, 0.0 = center, 1.0 = right
    void setMute(bool shouldMute);
    void setSolo(bool shouldSolo);
    void setVolume(float newVolume);   // in dB
    
    void updateMeters(const juce::AudioBuffer<float>& buffer);

    // Parameter getters
    float getCurrentLevel() const { return currentLevel.load(); }
    float getPeakLevel() const { return peakLevel.load(); }
    bool isClipping() const { return clipping.load(); }

    

private:
    std::atomic<float> inputGain{1.0f};
    std::atomic<float> volume{1.0f};
    std::atomic<float> pan{0.0f};
    std::atomic<bool> mute{false};
    std::atomic<bool> solo{false};

    // Metering
    std::atomic<float> currentLevel{0.0f};
    std::atomic<float> peakLevel{0.0f};
    std::atomic<bool> clipping{false};

    // Processing helpers
    void applyGain(juce::AudioBuffer<float>& buffer);
    void applyPanning(juce::AudioBuffer<float>& buffer);
    

    double currentSampleRate{44100.0};
    float lastRmsLevel{0.0f};
    const float meterSmoothingCoeff{0.8f};
};