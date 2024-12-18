#pragma once

#include <JuceHeader.h>
#include <vector>
#include <array>
#include <memory>

class AudioRouting
{
public:
    // Constants for maximum supported I/O
    static constexpr int maxInputChannels = 32;
    static constexpr int maxOutputChannels = 32;
    static constexpr int maxBusses = 8;

    struct RoutingPoint
    {
        enum class Type
        {
            HardwareInput,
            HardwareOutput,
            Bus,
            ChannelInput,
            ChannelOutput
        };

        Type type;
        int index;
        juce::String name;
    };

    struct ChannelRouting
    {
        std::vector<int> inputSources;     // Indices of input routing points
        std::vector<int> outputTargets;    // Indices of output routing points
        bool inputEnabled = true;
        bool outputEnabled = true;
        float inputGain = 1.0f;
        float outputGain = 1.0f;
    };

    AudioRouting();

    // Setup methods
    void updateAvailableInputs(const juce::StringArray& inputNames);
    void updateAvailableOutputs(const juce::StringArray& outputNames);
    void createBus(const juce::String& busName);
    void removeBus(int busIndex);

    // Routing methods
    bool connectInput(int channelIndex, const RoutingPoint& source);
    bool connectOutput(int channelIndex, const RoutingPoint& target);
    bool disconnectInput(int channelIndex, const RoutingPoint& source);
    bool disconnectOutput(int channelIndex, const RoutingPoint& target);

    // Query methods
    juce::StringArray getAvailableInputs() const;
    juce::StringArray getAvailableOutputs() const;
    juce::StringArray getAvailableBusses() const;
    std::vector<RoutingPoint> getChannelInputs(int channelIndex) const;
    std::vector<RoutingPoint> getChannelOutputs(int channelIndex) const;

    // Channel routing configuration
    ChannelRouting& getChannelRouting(int channelIndex);
    void setChannelInputEnabled(int channelIndex, bool enabled);
    void setChannelOutputEnabled(int channelIndex, bool enabled);
    void setChannelInputGain(int channelIndex, float gain);
    void setChannelOutputGain(int channelIndex, float gain);

private:
    std::vector<RoutingPoint> availableInputs;
    std::vector<RoutingPoint> availableOutputs;
    std::vector<RoutingPoint> busses;
    std::vector<ChannelRouting> channelRouting;
    
    bool isValidRoutingPoint(const RoutingPoint& point) const;
    void updateRoutingPoints();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioRouting)
};