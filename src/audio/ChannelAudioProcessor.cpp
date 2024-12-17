#include "ChannelAudioProcessor.h"

ChannelAudioProcessor::ChannelAudioProcessor()
{
}

void ChannelAudioProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    currentSampleRate = sampleRate;
}

void ChannelAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer)
{
    if (mute.load() || buffer.getNumSamples() == 0)
    {
        buffer.clear();
        currentLevel.store(0.0f);
        return;
    }

    // Apply input gain
    applyGain(buffer);

    // Apply panning
    applyPanning(buffer);

    // Update meters
    updateMeters(buffer);
}

void ChannelAudioProcessor::applyGain(juce::AudioBuffer<float>& buffer)
{
    const float gainLinear = inputGain.load() * volume.load();
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        buffer.applyGain(channel, 0, buffer.getNumSamples(), gainLinear);
    }
}

void ChannelAudioProcessor::applyPanning(juce::AudioBuffer<float>& buffer)
{
    if (buffer.getNumChannels() < 2)
        return;

    const float panValue = pan.load();
    const float leftGain = std::cos((panValue + 1.0f) * juce::MathConstants<float>::pi / 4.0f);
    const float rightGain = std::sin((panValue + 1.0f) * juce::MathConstants<float>::pi / 4.0f);

    buffer.applyGain(0, 0, buffer.getNumSamples(), leftGain);
    buffer.applyGain(1, 0, buffer.getNumSamples(), rightGain);
}

void ChannelAudioProcessor::updateMeters(const juce::AudioBuffer<float>& buffer)
{
    float rmsLevel = 0.0f;
    float peak = 0.0f;

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        rmsLevel += buffer.getRMSLevel(channel, 0, buffer.getNumSamples());
        peak = std::max(peak, buffer.getMagnitude(channel, 0, buffer.getNumSamples()));
    }

    rmsLevel /= buffer.getNumChannels();

    // Smooth the RMS level
    lastRmsLevel = lastRmsLevel * meterSmoothingCoeff + 
                  rmsLevel * (1.0f - meterSmoothingCoeff);

    currentLevel.store(lastRmsLevel);
    peakLevel.store(peak);
    clipping.store(peak > 1.0f);
}

void ChannelAudioProcessor::releaseResources()
{
    // Nothing to release currently
}

void ChannelAudioProcessor::setGain(float newGain)
{
    // Convert dB to linear gain
    inputGain.store(juce::Decibels::decibelsToGain(newGain));
}

void ChannelAudioProcessor::setPan(float newPan)
{
    pan.store(juce::jlimit(-1.0f, 1.0f, newPan));
}

void ChannelAudioProcessor::setMute(bool shouldMute)
{
    mute.store(shouldMute);
}

void ChannelAudioProcessor::setSolo(bool shouldSolo)
{
    solo.store(shouldSolo);
}

void ChannelAudioProcessor::setVolume(float newVolume)
{
    // Convert dB to linear gain
    volume.store(juce::Decibels::decibelsToGain(newVolume));
}