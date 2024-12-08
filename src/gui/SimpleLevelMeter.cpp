#include "SimpleLevelMeter.h"

SimpleLevelMeter::SimpleLevelMeter()
{
    startTimer(30); // Update the meter 30 times per second
}

SimpleLevelMeter::~SimpleLevelMeter()
{
    stopTimer();
}

void SimpleLevelMeter::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    const int width = getWidth();
    const int height = getHeight();

    // Draw left channel meter
    g.setColour(juce::Colour::fromRGB(0, 255, 0).interpolatedWith(juce::Colour::fromRGB(255, 0, 0), leftLevel));
    g.fillRect(0, static_cast<int>(height * (1.0f - leftLevel)), width / 2, static_cast<int>(height * leftLevel));

    // Draw right channel meter
    g.setColour(juce::Colour::fromRGB(0, 255, 0).interpolatedWith(juce::Colours::red, rightLevel));
    g.fillRect(width / 2, static_cast<int>(height * (1.0f - rightLevel)), width / 2, static_cast<int>(height * rightLevel));
}

void SimpleLevelMeter::timerCallback()
{
    repaint(); // Trigger a redraw of the component
}

void SimpleLevelMeter::setLevel(float left, float right)
{
    leftLevel = left;
    rightLevel = right;
}