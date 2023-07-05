#include "NestedAllPassChain.h"

NestedChain::NestedChain()
{
}

void NestedChain::c_setMaxDelayInSamples(int maximumDelayInSamples)
{
    for (auto &allPassChain : nestedAllPassArray)
    {
        allPassChain.n_setMaxDelayInSamples(maximumDelayInSamples);
    }
}

void NestedChain::c_delayLinePrepare(juce::dsp::ProcessSpec spec)
{
    for (auto &allPassChain : nestedAllPassArray)
    {
        allPassChain.n_delayLinePrepare(spec);
    }
}

void NestedChain::c_setDelayInSamples(int delayInSamples, int n_delayInSamples)
{

    for (int i{0}; i < 6; ++i)
    {
        delayInSamples *= 0.7f;
        n_delayInSamples *= 0.85f;
        nestedAllPassArray[i].n_setDelayInSamples(delayInSamples, n_delayInSamples);
    }
}

void NestedChain::c_setAllPassGain(float allPassG, float n_allPassG)
{

    for (int i{0}; i < 6; ++i)
    {
        allPassG *= 0.5f;
        nestedAllPassArray[i].n_setAllPassGain(allPassG, n_allPassG);
    }
}

void NestedChain::c_prepareFilterArray(juce::dsp::ProcessSpec spec)
{
    for (auto &filter : filterArray)
    {
        filter.prepare(spec);
    }
}

void NestedChain::c_setFilterArrayCutoffFrequency(float freq)
{
    for (int i{0}; i < 6; ++i)
    {
        freq *= 0.85f;
        filterArray[i].setCutoffFrequency(freq);
        filterArray[i].setResonance(0.7f);
    }
}

void NestedChain::c_typeFilterArray(juce::dsp::StateVariableTPTFilterType type)
{
    for (auto &filter : filterArray)
    {
        filter.setType(type);
    }
}

void NestedChain::c_resetFilterArray()
{
    for (auto &filter : filterArray)
    {
        filter.reset();
    }
}

void NestedChain::c_process(float *sample, int bufferSize, int channel)
{
    for (int i{0}; i < bufferSize; ++i)
    {
        auto currentSample = sample[i];
        for (int j{0}; j < 6; ++j)
        {
            currentSample = nestedAllPassArray[j].n_processSingleSample(currentSample, channel);
            currentSample = filterArray[j].processSample(channel, currentSample);
        }
        sample[i] = currentSample;
    }
}