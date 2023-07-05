#include "NestedAllPass.h"

NestedAllPassStruct::NestedAllPassStruct()
{
}

void NestedAllPassStruct::n_setMaxDelayInSamples(int maximumDelayInSamples)
{
    for (auto &allPass : allPassArray)
    {
        allPass.setMaxDelayInSamples(maximumDelayInSamples);
    }
    n_delayLine.setMaximumDelayInSamples(maximumDelayInSamples);
}

void NestedAllPassStruct::n_delayLinePrepare(juce::dsp::ProcessSpec spec)
{
    for (auto &allPass : allPassArray)
    {
        allPass.delayLinePrepare(spec);
    }
    n_delayLine.prepare(spec);
}

void NestedAllPassStruct::n_setDelayInSamples(int delayInSamples, int n_delayInSamples)
{

    for (int i{0}; i < 15; ++i)
    {
        delayInSamples *= 0.33f;
        allPassArray[i].setDelayInSamples(delayInSamples);
    }
    n_delayLine.setDelay(n_delayInSamples);
}

void NestedAllPassStruct::n_setAllPassGain(float allPassG, float n_allPassG)
{

    for (int i{0}; i < 15; ++i)
    {
        allPassG *= 0.7f;
        allPassArray[i].setAllPassGain(allPassG);
    }
    n_allPassGain = n_allPassG;
}

//--------------------------------------------------------------------------------------------------------------------
void NestedAllPassStruct::n_process(float *sample, int bufferSize, int channel)
{
    for (int i{0}; i < bufferSize; ++i)
    {

        sample[i] = n_processSingleSample(sample[i], channel);
    }
}

float NestedAllPassStruct::n_processSingleSample(float sample, int channel)
{

    n_delayLineOutput = n_delayLine.popSample(channel);
    for (int w{0}; w < 3; ++w)
    {
        n_delayLineOutput = allPassArray[w].processSingleSample(n_delayLineOutput, channel);
    }

    n_feedForward = sample * (-n_allPassGain);
    n_feedBack = n_delayLineOutput * n_allPassGain;
    n_delayLine.pushSample(channel, sample + n_feedBack);
    sample = (n_delayLineOutput * (1 - std::pow(n_allPassGain, 2))) + n_feedForward;

    return sample;
}

float NestedAllPassStruct::n_allPassFilterChain(float sample, int channel)
{
    for (int i{1}; i < 15; ++i)
    {
        sample = allPassArray[i].processSingleSample(sample, channel);
    }
    return sample;
}