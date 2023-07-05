#pragma once

#include <JuceHeader.h>
#include <array>
class AllPassFilter
{
    float delayLineOutput;
    float feedBack;
    float feedForward;
    float allPassGain;
    juce::dsp::DelayLine<float> delayLine;

public:
    AllPassFilter();

    void setMaxDelayInSamples(int maximumDelayInSamples);
    void delayLinePrepare(juce::dsp::ProcessSpec spec);
    void setDelayInSamples(int delayInSamles);
    void setAllPassGain(float allPassG);
    float processSingleSample(float sample, int channel);
};