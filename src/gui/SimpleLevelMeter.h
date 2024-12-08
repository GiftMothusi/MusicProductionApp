#pragma once

#include <JuceHeader.h>

class SimpleLevelMeter : public juce::Component, public::Timer {
public:
    SimpleLevelMeter();
    ~SimpleLevelMeter() override;

    void paint(juce::Graphics& g) override;
    void timerCallback() override;

    void setLevel(float leftLevel, float rightLevel);

private:
    float leftLevel = 0.0f;
    float rightLevel = 0.0f;
    
    juce::Image background;
};