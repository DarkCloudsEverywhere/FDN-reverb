#pragma once

#include <JuceHeader.h>
#include "NestedAllPass.h"

class NestedChain
{

    std::array<NestedAllPassStruct, 6> nestedAllPassArray;
    std::array<juce::dsp::StateVariableTPTFilter<float>, 6> filterArray;

public:
    NestedChain();

    void c_setMaxDelayInSamples(int maximumDelayInSamples);
    void c_delayLinePrepare(juce::dsp::ProcessSpec spec);
    void c_setDelayInSamples(int delayInSamples, int n_delayInSamples);
    void c_setAllPassGain(float allPassG, float n_allPassG);
    void c_prepareFilterArray(juce::dsp::ProcessSpec spec);
    void c_typeFilterArray(juce::dsp::StateVariableTPTFilterType type);
    void c_resetFilterArray();
    void c_setFilterArrayCutoffFrequency(float freq);
    void c_process(float *sample, int bufferSize, int channel);
};