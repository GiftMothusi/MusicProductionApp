#include "MixerPanel.h"

MixerPanel::MixerPanel()
{
    //Create Default channels
    for (int i = 0; i < 4; ++i)
    {
       addChannel();
    }
}

MixerPanel::~MixerPanel()
{
}

void MixerPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
   
}

void MixerPanel::resized()
{
    // Layout mixer controls here
    int padding = 10;
    int channelWidth = (getWidth() - (channelStrips.size() + 1) * padding) / channelStrips.size();
    int channelHeight = getHeight() - 2 * padding;

    for (size_t i = 0; i < channelStrips.size(); ++i)
    {
        channelStrips[i]->setBounds(padding + i * (channelWidth + padding), padding, channelWidth, channelHeight);
    }
}

void MixerPanel::setAudioEngine(AudioEngine* engine)
{
    if (!engine) return;
    
    audioEngine = engine;
    audioEngine->addChangeListener(this);
    
    // Connect existing channels
    for (size_t i = 0; i < channelStrips.size(); ++i)
    {
        if (audioEngine)
        {
            audioEngine->addChannel();
            auto processor = audioEngine->getChannelProcessor(i);
            if (processor && channelStrips[i])
            {
                // Connect channel strip to processor
                channelStrips[i]->setAudioProcessor(processor);
            }
        }
    }
    updateRoutingOptions();
}


void MixerPanel::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == audioEngine)
    {
        updateRoutingOptions();
    }
}

void MixerPanel::addChannel()
{
    auto channelStrip = std::make_unique<ChannelStrip>();
    addAndMakeVisible(*channelStrip);
    
    if (audioEngine)
    {
        audioEngine->addChannel();
        if (auto processor = audioEngine->getChannelProcessor(channelStrips.size()))
        {
            channelStrip->setAudioProcessor(processor);
        }
    }
    
    channelStrips.push_back(std::move(channelStrip));
    resized();
}

void MixerPanel::removeChannel(int index)
{
    if (index >= 0 && index < channelStrips.size())
    {
        channelStrips.erase(channelStrips.begin() + index);
        if (audioEngine)
            audioEngine->removeChannel(index);
        resized();
    }
}

void MixerPanel::updateRoutingOptions()
{
    if (audioEngine)
    {
        auto inputs = audioEngine->getAvailableInputs();
        auto outputs = audioEngine->getAvailableOutputs();
        
        for (auto& strip : channelStrips)
        {
            if (strip)
            {
                strip->updateRoutingOptions(inputs, outputs);
            }
        }
    }
}

void MixerPanel::refreshChannelRoutingOptions()
{
    updateRoutingOptions();
}