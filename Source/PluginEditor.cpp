#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FDN_ReverbEditor::FDN_ReverbEditor(FDN_Reverb &p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    juce::ignoreUnused(processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    for (auto comp : getComps())
    {
        addAndMakeVisible(comp);
    }

    setSize(600, 400);
}

FDN_ReverbEditor::~FDN_ReverbEditor()
{
}

//==============================================================================
void FDN_ReverbEditor::paint(juce::Graphics &g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawFittedText("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void FDN_ReverbEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto bounds = getLocalBounds();

    Mix.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.2));
    Density.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.4));
    Diffusion.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.6));
    Size.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.8));
    FilterCutoff.setBounds(bounds.removeFromTop(bounds.getHeight()));
}

std::vector<juce::Component *> FDN_ReverbEditor::getComps()
{
    return {
        &Mix,
        &Density,
        &Diffusion,
        &Size,
        &FilterCutoff};
}