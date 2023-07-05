#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FDN_Reverb::FDN_Reverb()
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                         ),
      treeState(*this, nullptr, "Parameters", createParameterLayout())
{
}

FDN_Reverb::~FDN_Reverb()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout FDN_Reverb::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID{"mix", 1}, "Mix", 0.f, 1.f, 0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID{"n_allPassGain", 1}, "Density", 0.f, 1.f, 0.85f));
    layout.add(std::make_unique<juce::AudioParameterInt>(ParameterID{"allPassDelay", 1}, "Diffusion", 1, 100, 2));
    layout.add(std::make_unique<juce::AudioParameterInt>(ParameterID{"n_allPassDelay", 1}, "Size", 1, 100, 2));

    layout.add(std::make_unique<juce::AudioParameterInt>(ParameterID{"delayLineFilterCutoff", 1}, "FilterCutoff", 0.f, 20000.f, 9500.f));

    return layout;
}

//==============================================================================
const juce::String FDN_Reverb::getName() const
{
    return JucePlugin_Name;
}

bool FDN_Reverb::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool FDN_Reverb::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool FDN_Reverb::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double FDN_Reverb::getTailLengthSeconds() const
{
    return 0.0;
}

int FDN_Reverb::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int FDN_Reverb::getCurrentProgram()
{
    return 0;
}

void FDN_Reverb::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String FDN_Reverb::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void FDN_Reverb::changeProgramName(int index, const juce::String &newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void FDN_Reverb::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    // juce::ignoreUnused(sampleRate, samplesPerBlock);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumInputChannels();

    allPassChain.c_delayLinePrepare(spec);
    allPassChain.c_setMaxDelayInSamples(10000);
    allPassChain.c_prepareFilterArray(spec);

    auto filterType = juce::dsp::StateVariableTPTFilterType::lowpass;
    allPassChain.c_typeFilterArray(filterType);

    allPassChain.c_resetFilterArray();

    mixer.prepare(spec);
    mixer.reset();
}

void FDN_Reverb::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool FDN_Reverb::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

void FDN_Reverb::processBlock(juce::AudioBuffer<float> &buffer,
                              juce::MidiBuffer &midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    auto mix = treeState.getRawParameterValue("mix");
    auto allPassGain = treeState.getRawParameterValue("allPassGain");
    auto n_allPassGain = treeState.getRawParameterValue("n_allPassGain");

    auto allPassDelay = treeState.getRawParameterValue("allPassDelay");
    auto allPassDelay_n = juce::jmap<int>(*allPassDelay, 1, 100, 500, 10000);

    auto n_allPassDelay = treeState.getRawParameterValue("n_allPassDelay");
    auto n_allPassDelay_n = juce::jmap<int>(*n_allPassDelay, 1, 100, 500, 10000);

    auto filterFreqCutoff = treeState.getRawParameterValue("delayLineFilterCutoff");

    juce::dsp::AudioBlock<float> block(buffer);
    mixer.pushDrySamples(block);
    mixer.setWetMixProportion(*mix);

    allPassChain.c_setAllPassGain(0.7f, *n_allPassGain);
    allPassChain.c_setDelayInSamples(allPassDelay_n, n_allPassDelay_n);
    allPassChain.c_setFilterArrayCutoffFrequency(*filterFreqCutoff);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto *channelData = buffer.getWritePointer(channel);
        allPassChain.c_process(channelData, buffer.getNumSamples(), channel);
    }
    mixer.mixWetSamples(buffer);
}
//==============================================================================
bool FDN_Reverb::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *FDN_Reverb::createEditor()
{
    // return new FDN_ReverbEditor(*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void FDN_Reverb::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused(destData);
}

void FDN_Reverb::setStateInformation(const void *data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused(data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin...
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new FDN_Reverb();
}
