#include "AudioEngine.h"

AudioEngine::AudioEngine()
{
    initialiseAudio();
}

AudioEngine::~AudioEngine()
{
    shutdownAudio();
    deviceManager.removeChangeListener(this);  
}

void AudioEngine::initialiseAudio()
{
     // First, get the current default device settings
    auto* currentDevice = deviceManager.getCurrentAudioDevice();
    juce::String defaultDeviceName;
    if (currentDevice != nullptr)
        defaultDeviceName = currentDevice->getName();

    // Setup options for the device manager
    juce::AudioDeviceManager::AudioDeviceSetup setup;
    deviceManager.getAudioDeviceSetup(setup);

    // Try to keep the current default device if possible
    if (!defaultDeviceName.isEmpty())
        setup.outputDeviceName = defaultDeviceName;

    setup.bufferSize = 512;  // or whatever default you prefer
    setup.sampleRate = 44100.0;  // or whatever default you prefer
    setup.useDefaultOutputChannels = true;

    // Initialize with these settings
    auto error = deviceManager.initialise(
        0,     // numInputChannels - 0 to start with no inputs
        2,     // numOutputChannels - stereo output
        nullptr,  // no XML settings
        true,   // selectDefaultDeviceOnFailure
        defaultDeviceName.isEmpty() ? "" : defaultDeviceName,  // preferredDefaultDeviceName
        &setup  // preferredSetupOptions
    );

    if (error.isEmpty())
    {
        deviceManager.addAudioCallback(this);
        audioInitialised = true;

        // Add a callback for device type change
        deviceManager.addChangeListener(this);
        updateRoutingOptions();
    }
    else
    {
        juce::Logger::writeToLog("Audio initialization failed: " + error);
    }
}

void AudioEngine::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &deviceManager)
    {
        // Device settings have changed - update our internal state
        auto* device = deviceManager.getCurrentAudioDevice();
        if (device != nullptr)
        {
            currentSampleRate = device->getCurrentSampleRate();
            currentBlockSize = device->getCurrentBufferSizeSamples();
            updateRoutingOptions();

            // Notify channel processors of the change
            for (auto& processor : channelProcessors)
            {
                if (processor)
                    processor->prepareToPlay(currentSampleRate, currentBlockSize);
            }
        }
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

    if (!audioInitialised)
    {
        // Clear output if not initialized
        for (int i = 0; i < numOutputChannels; ++i)
        {
            if (outputChannelData[i])
                juce::FloatVectorOperations::clear(outputChannelData[i], numSamples);
        }
        return;
    }

    // Clear mix buffer and resize if needed
    if (mixBuffer.getNumSamples() != numSamples || mixBuffer.getNumChannels() != numOutputChannels)
        mixBuffer.setSize(numOutputChannels, numSamples, false, false, true);
    mixBuffer.clear();

    // Process each channel
    for (size_t i = 0; i < channelProcessors.size(); ++i)
    {
        if (auto* processor = channelProcessors[i].get())
        {
            // Create working buffer for this channel
            juce::AudioBuffer<float> channelBuffer(2, numSamples);
            channelBuffer.clear();
            
            // Get the input routing for this channel
            int inputIndex = processor->getInput();
            
            // Copy input if available and routed
            if (inputIndex >= 0 && inputIndex < numInputChannels)
            {
                // Copy the routed input to both channels for now (can be enhanced for stereo routing)
                for (int chan = 0; chan < channelBuffer.getNumChannels(); ++chan)
                {
                    if (inputChannelData[inputIndex] != nullptr)
                    {
                        channelBuffer.copyFrom(chan, 0, 
                                             inputChannelData[inputIndex], 
                                             numSamples);
                    }
                }
            }

            // Process the channel
            processor->processBlock(channelBuffer);

            // Get the output routing for this channel
            int outputIndex = processor->getOutput();
            
            // Add to appropriate outputs in mix buffer
            if (outputIndex >= 0 && outputIndex < numOutputChannels)
            {
                // For now, route to the specified output and the next channel (for stereo)
                for (int chan = 0; chan < juce::jmin(2, numOutputChannels - outputIndex); ++chan)
                {
                    mixBuffer.addFrom(outputIndex + chan, 0,
                                    channelBuffer, chan, 0,
                                    numSamples);
                }
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
                juce::FloatVectorOperations::clear(outputChannelData[chan], numSamples);
            }
        }
    }

    // Update meters
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
    if (currentSampleRate > 0.0){
        processor->prepareToPlay(currentSampleRate, currentBlockSize);
    }
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
     const juce::ScopedLock sl(processAudioLock);
    if (index >= 0 && index < channelProcessors.size())
    {
        return channelProcessors[index].get();
    }
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

void AudioEngine::updateRoutingOptions()
{
    auto* device = deviceManager.getCurrentAudioDevice();
    if (device)
    {
        // Get input names from current device
        juce::StringArray inputNames;
        for (int i = 0; i < device->getInputChannelNames().size(); ++i)
        {
            inputNames.add("Input " + juce::String(i + 1) + ": " + 
                          device->getInputChannelNames()[i]);
        }
        // router.updateAvailableInputs(inputNames);

        // Get output names from current device
        juce::StringArray outputNames;
        auto outputChannelNames = device->getOutputChannelNames();
        for (int i = 0; i < device->getOutputChannelNames().size(); ++i)
        {
            outputNames.add("Output " + juce::String(i + 1) + ": " + 
                          outputChannelNames[i]);
        }
        router.updateAvailableInputs(inputNames);
        router.updateAvailableOutputs(outputNames);

         sendChangeMessage();
    }
}

juce::StringArray AudioEngine::getAvailableInputs() const
{
    return router.getAvailableInputs();
}

juce::StringArray AudioEngine::getAvailableOutputs() const
{
    return router.getAvailableOutputs();
}

bool AudioEngine::setChannelInput(int channelIndex, int inputIndex)
{
    AudioRouting::RoutingPoint point{AudioRouting::RoutingPoint::Type::HardwareInput, 
                                   inputIndex, ""};
    return router.connectInput(channelIndex, point);
}

bool AudioEngine::setChannelOutput(int channelIndex, int outputIndex)
{
    AudioRouting::RoutingPoint point{AudioRouting::RoutingPoint::Type::HardwareOutput, 
                                   outputIndex, ""};
    return router.connectOutput(channelIndex, point);
}