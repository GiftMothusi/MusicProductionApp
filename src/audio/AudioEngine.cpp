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

    // Clear mix buffer and resize if needed
    if (mixBuffer.getNumSamples() != numSamples || mixBuffer.getNumChannels() != 2)
        mixBuffer.setSize(2, numSamples, false, false, true);
    mixBuffer.clear();

    // Process each channel
    for (size_t i = 0; i < channelProcessors.size(); ++i)
    {
        if (auto* processor = channelProcessors[i].get())
        {
            // Create working buffer for this channel
            juce::AudioBuffer<float> channelBuffer(2, numSamples);
            
            // Copy input if available
            if (numInputChannels > 0)
            {
                for (int chan = 0; chan < channelBuffer.getNumChannels(); ++chan)
                {
                    if (inputChannelData[chan % numInputChannels] != nullptr)
                    {
                        channelBuffer.copyFrom(chan, 0, 
                                             inputChannelData[chan % numInputChannels], 
                                             numSamples);
                    }
                }
            }

            // Process the channel
            processor->processBlock(channelBuffer);

            // Update channel meters
            float rmsLevel = (channelBuffer.getRMSLevel(0, 0, numSamples) + 
                            channelBuffer.getRMSLevel(1, 0, numSamples)) * 0.5f;
            processor->updateMeters(channelBuffer);

            // Add to mix buffer
            for (int chan = 0; chan < mixBuffer.getNumChannels(); ++chan)
            {
                mixBuffer.addFrom(chan, 0, channelBuffer, chan, 0, numSamples);
            }
        }
    }

    // Apply main volume to mix buffer
    mixBuffer.applyGain(mainVolume.load());

    // Copy mixed output to device output
    for (int chan = 0; chan < numOutputChannels; ++chan)
    {
        if (outputChannelData[chan] != nullptr)
        {
            if (chan < mixBuffer.getNumChannels())
            {
                juce::FloatVectorOperations::copy(outputChannelData[chan],
                                                mixBuffer.getReadPointer(chan),
                                                numSamples);
            }
            else
            {
                juce::FloatVectorOperations::clear(outputChannelData[chan], 
                                                 numSamples);
            }
        }
    }

    // Update main output meters
    leftLevel.store(mixBuffer.getRMSLevel(0, 0, numSamples));
    rightLevel.store(mixBuffer.getRMSLevel(1, 0, numSamples));
}
void AudioEngine::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    currentSampleRate = device->getCurrentSampleRate();
    currentBlockSize = device->getCurrentBufferSizeSamples();

    mixBuffer.setSize(2, currentBlockSize);
    mixBuffer.clear();

    // Prepare all channel processors
    for (auto& processor : channelProcessors)
    {
        if (processor)
            processor->prepareToPlay(currentSampleRate, currentBlockSize);
    }
}

void AudioEngine::audioDeviceStopped()
{
    currentSampleRate = 0.0;
    currentBlockSize = 0;
    mixBuffer.setSize(0, 0);

    // Release all channel processors
    for (auto& processor : channelProcessors)
    {
        if (processor)
            processor->releaseResources();
    }
}

void AudioEngine::addChannel()
{
    const juce::ScopedLock sl(processAudioLock);
    auto processor = std::make_unique<ChannelAudioProcessor>();
    if (currentSampleRate > 0.0)
        processor->prepareToPlay(currentSampleRate, currentBlockSize);
    channelProcessors.push_back(std::move(processor));
}


void AudioEngine::removeChannel(int index)
{
    const juce::ScopedLock sl(processAudioLock);
    if (index >= 0 && index < channelProcessors.size())
        channelProcessors.erase(channelProcessors.begin() + index);
}

ChannelAudioProcessor* AudioEngine::getChannelProcessor(int index)
{
    if (index >= 0 && index < channelProcessors.size())
        return channelProcessors[index].get();
    return nullptr;
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
