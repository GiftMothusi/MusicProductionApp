// MidiHandler.cpp
#include "MidiHandler.h"

MidiHandler::MidiHandler()
{
    // Get list of MIDI input devices
    auto devices = juce::MidiInput::getAvailableDevices();

    // Try to open the first available device
    if (!devices.isEmpty())
    {
        auto device = devices[0];
        // The openDevice function returns a uniquePtr, so we should move it
        midiInput = juce::MidiInput::openDevice(device.identifier, this);
        
        if (midiInput != nullptr)
            midiInput->start();
    }
}

MidiHandler::~MidiHandler()
{
    if (midiInput != nullptr)
        midiInput->stop();
}

void MidiHandler::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message)
{
    midiCollector.addMessageToQueue(message);
}