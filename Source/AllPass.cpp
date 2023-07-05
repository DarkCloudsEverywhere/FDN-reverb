#include "AllPass.h"
#include <math.h>

AllPassFilter::AllPassFilter()
{
}

void AllPassFilter::setMaxDelayInSamples(int maximumDelayInSamples)
{
    delayLine.setMaximumDelayInSamples(maximumDelayInSamples);
}

void AllPassFilter::delayLinePrepare(juce::dsp::ProcessSpec spec)
{
    delayLine.prepare(spec);
}

void AllPassFilter::setDelayInSamples(int delayInSamples)
{
    delayLine.setDelay(delayInSamples);
}

void AllPassFilter::setAllPassGain(float allPassG)
{
    allPassGain = allPassG;
}

float AllPassFilter::processSingleSample(float sample, int channel)
{
    delayLineOutput = delayLine.popSample(channel);
    feedForward = sample * (-allPassGain);
    feedBack = delayLineOutput * allPassGain;
    delayLine.pushSample(channel, sample + feedBack);
    return ((delayLineOutput * (1 - std::pow(allPassGain, 2))) + feedForward);
}