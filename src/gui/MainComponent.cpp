#include "MainComponent.h"

MainComponent::MainComponent()
{
    audioEngine = std::make_unique<AudioEngine>();
    midiHandler = std::make_unique<MidiHandler>();
    mixerPanel = std::make_unique<MixerPanel>();
    
    addAndMakeVisible(mixerPanel.get());
    setSize(800, 600);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void MainComponent::resized()
{
    // Make the mixer panel fill the window
    mixerPanel->setBounds(getLocalBounds());
}