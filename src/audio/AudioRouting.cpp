#include "AudioRouting.h"

AudioRouting::AudioRouting()
{
    // Initialize with some reasonable default size
    channelRouting.resize(16);
}

void AudioRouting::updateAvailableInputs(const juce::StringArray& inputNames)
{
    availableInputs.clear();
    for (int i = 0; i < inputNames.size(); ++i)
    {
        availableInputs.push_back({
            RoutingPoint::Type::HardwareInput,
            i,
            inputNames[i]
        });
    }
    updateRoutingPoints();
}

void AudioRouting::updateAvailableOutputs(const juce::StringArray& outputNames)
{
    availableOutputs.clear();
    for (int i = 0; i < outputNames.size(); ++i)
    {
        availableOutputs.push_back({
            RoutingPoint::Type::HardwareOutput,
            i,
            outputNames[i]
        });
    }
    updateRoutingPoints();
}

void AudioRouting::createBus(const juce::String& busName)
{
    if (busses.size() < maxBusses)
    {
        busses.push_back({
            RoutingPoint::Type::Bus,
            static_cast<int>(busses.size()),
            busName
        });
        updateRoutingPoints();
    }
}

void AudioRouting::removeBus(int busIndex)
{
    if (busIndex >= 0 && busIndex < busses.size())
    {
        busses.erase(busses.begin() + busIndex);
        updateRoutingPoints();
    }
}

bool AudioRouting::connectInput(int channelIndex, const RoutingPoint& source)
{
    if (channelIndex >= 0 && channelIndex < channelRouting.size() && isValidRoutingPoint(source))
    {
        auto& routing = channelRouting[channelIndex];
        auto it = std::find(routing.inputSources.begin(), routing.inputSources.end(), source.index);
        if (it == routing.inputSources.end())
        {
            routing.inputSources.push_back(source.index);
            return true;
        }
    }
    return false;
}

bool AudioRouting::connectOutput(int channelIndex, const RoutingPoint& target)
{
    if (channelIndex >= 0 && channelIndex < channelRouting.size() && isValidRoutingPoint(target))
    {
        auto& routing = channelRouting[channelIndex];
        auto it = std::find(routing.outputTargets.begin(), routing.outputTargets.end(), target.index);
        if (it == routing.outputTargets.end())
        {
            routing.outputTargets.push_back(target.index);
            return true;
        }
    }
    return false;
}

bool AudioRouting::disconnectInput(int channelIndex, const RoutingPoint& source)
{
    if (channelIndex >= 0 && channelIndex < channelRouting.size())
    {
        auto& routing = channelRouting[channelIndex];
        auto it = std::find(routing.inputSources.begin(), routing.inputSources.end(), source.index);
        if (it != routing.inputSources.end())
        {
            routing.inputSources.erase(it);
            return true;
        }
    }
    return false;
}

bool AudioRouting::disconnectOutput(int channelIndex, const RoutingPoint& target)
{
    if (channelIndex >= 0 && channelIndex < channelRouting.size())
    {
        auto& routing = channelRouting[channelIndex];
        auto it = std::find(routing.outputTargets.begin(), routing.outputTargets.end(), target.index);
        if (it != routing.outputTargets.end())
        {
            routing.outputTargets.erase(it);
            return true;
        }
    }
    return false;
}

juce::StringArray AudioRouting::getAvailableInputs() const
{
    juce::StringArray names;
    for (const auto& input : availableInputs)
        names.add(input.name);
    return names;
}

juce::StringArray AudioRouting::getAvailableOutputs() const
{
    juce::StringArray names;
    for (const auto& output : availableOutputs)
        names.add(output.name);
    return names;
}

juce::StringArray AudioRouting::getAvailableBusses() const
{
    juce::StringArray names;
    for (const auto& bus : busses)
        names.add(bus.name);
    return names;
}

std::vector<AudioRouting::RoutingPoint> AudioRouting::getChannelInputs(int channelIndex) const
{
    std::vector<RoutingPoint> inputs;
    if (channelIndex >= 0 && channelIndex < channelRouting.size())
    {
        const auto& routing = channelRouting[channelIndex];
        for (int index : routing.inputSources)
        {
            // Find the corresponding routing point
            for (const auto& input : availableInputs)
                if (input.index == index)
                    inputs.push_back(input);
            for (const auto& bus : busses)
                if (bus.index == index)
                    inputs.push_back(bus);
        }
    }
    return inputs;
}

std::vector<AudioRouting::RoutingPoint> AudioRouting::getChannelOutputs(int channelIndex) const
{
    std::vector<RoutingPoint> outputs;
    if (channelIndex >= 0 && channelIndex < channelRouting.size())
    {
        const auto& routing = channelRouting[channelIndex];
        for (int index : routing.outputTargets)
        {
            // Find the corresponding routing point
            for (const auto& output : availableOutputs)
                if (output.index == index)
                    outputs.push_back(output);
            for (const auto& bus : busses)
                if (bus.index == index)
                    outputs.push_back(bus);
        }
    }
    return outputs;
}

AudioRouting::ChannelRouting& AudioRouting::getChannelRouting(int channelIndex)
{
    if (channelIndex >= channelRouting.size())
        channelRouting.resize(channelIndex + 1);
    return channelRouting[channelIndex];
}

void AudioRouting::setChannelInputEnabled(int channelIndex, bool enabled)
{
    if (channelIndex >= 0 && channelIndex < channelRouting.size())
        channelRouting[channelIndex].inputEnabled = enabled;
}

void AudioRouting::setChannelOutputEnabled(int channelIndex, bool enabled)
{
    if (channelIndex >= 0 && channelIndex < channelRouting.size())
        channelRouting[channelIndex].outputEnabled = enabled;
}

void AudioRouting::setChannelInputGain(int channelIndex, float gain)
{
    if (channelIndex >= 0 && channelIndex < channelRouting.size())
        channelRouting[channelIndex].inputGain = gain;
}

void AudioRouting::setChannelOutputGain(int channelIndex, float gain)
{
    if (channelIndex >= 0 && channelIndex < channelRouting.size())
        channelRouting[channelIndex].outputGain = gain;
}

bool AudioRouting::isValidRoutingPoint(const RoutingPoint& point) const
{
    switch (point.type)
    {
        case RoutingPoint::Type::HardwareInput:
            return point.index >= 0 && point.index < availableInputs.size();
        case RoutingPoint::Type::HardwareOutput:
            return point.index >= 0 && point.index < availableOutputs.size();
        case RoutingPoint::Type::Bus:
            return point.index >= 0 && point.index < busses.size();
        default:
            return false;
    }
}

void AudioRouting::updateRoutingPoints()
{
    // Update all channel routing to remove any invalid routing points
    for (auto& routing : channelRouting)
    {
        routing.inputSources.erase(
            std::remove_if(routing.inputSources.begin(), routing.inputSources.end(),
                [this](int index) {
                    return !isValidRoutingPoint({RoutingPoint::Type::HardwareInput, index, {}}) &&
                           !isValidRoutingPoint({RoutingPoint::Type::Bus, index, {}});
                }),
            routing.inputSources.end()
        );

        routing.outputTargets.erase(
            std::remove_if(routing.outputTargets.begin(), routing.outputTargets.end(),
                [this](int index) {
                    return !isValidRoutingPoint({RoutingPoint::Type::HardwareOutput, index, {}}) &&
                           !isValidRoutingPoint({RoutingPoint::Type::Bus, index, {}});
                }),
            routing.outputTargets.end()
        );
    }
}