#pragma once

#include <JuceHeader.h>
#include "AllPass.h"

class NestedAllPassStruct
{
    float n_delayLineOutput;
    float n_feedBack;
    float n_feedForward;
    float n_allPassGain;
    juce::dsp::DelayLine<float> n_delayLine;
    std::array<AllPassFilter, 15> allPassArray;

public:
    NestedAllPassStruct();

    void n_setMaxDelayInSamples(int maximumDelayInSamples);
    void n_delayLinePrepare(juce::dsp::ProcessSpec spec);
    void n_setDelayInSamples(int delayInSamples, int n_delayInSamples);
    void n_setAllPassGain(float allPassG, float n_allPassG);
    void n_process(float *sample, int bufferSize, int channel);
    float n_processSingleSample(float sample, int channel);
    float n_allPassFilterChain(float sample, int channel);
};