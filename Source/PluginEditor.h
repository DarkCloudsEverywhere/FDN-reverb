#pragma once

#include "PluginProcessor.h"

//==============================================================================
class FDN_ReverbEditor  : public juce::AudioProcessorEditor
{
public:
    explicit FDN_ReverbEditor (FDN_Reverb&);
    ~FDN_ReverbEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FDN_Reverb& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FDN_ReverbEditor)
};
