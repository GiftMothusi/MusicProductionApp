// MidiHandler.h
#pragma once

#include "JuceHeader.h"

class MidiHandler : public juce::MidiInputCallback
{
public:
    MidiHandler();
    ~MidiHandler() override;

    void handleIncomingMidiMessage(juce::MidiInput* source, 
                                 const juce::MidiMessage& message) override;

private:
    std::unique_ptr<juce::MidiInput> midiInput;
    juce::MidiMessageCollector midiCollector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiHandler)
};