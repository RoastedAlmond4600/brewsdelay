#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utils/Utils.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
    castParameter(apvts, ParameterID::lDelayTime, lDelayTimeParam);
    castParameter(apvts, ParameterID::rDelayTime, rDelayTimeParam);
    castParameter(apvts, ParameterID::wetLevel, wetLevelParam);
    castParameter(apvts, ParameterID::feedbackLevel, feedbackLevelParam);
    apvts.state.addListener(this);
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
    apvts.state.removeListener(this);
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (sampleRate, samplesPerBlock);
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    delayModule.prepare(spec);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());


    //Only want to call update when a parameter is actually changed.
    bool expected = true;
    if (parameterChanged.compare_exchange_strong(expected, false)) {
       update(); 
    }

    juce::dsp::AudioBlock<float> block {buffer};
    delayModule.process(juce::dsp::ProcessContextReplacing<float>{block});
}

juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    // Initialises and creates a pointer to an AudioParameterFloat type.
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                   ParameterID::lDelayTime,
                   "Left Delay Time",
                  juce::NormalisableRange(0.01f,5.f,0.1f),
                  0.5f,
                 juce::AudioParameterFloatAttributes().withLabel("ms") 
                ));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                   ParameterID::rDelayTime,
                   "RIght Delay Time",
                  juce::NormalisableRange(0.01f,5.f,0.1f),
                  0.5f,
                 juce::AudioParameterFloatAttributes().withLabel("ms") 
                ));
    layout.add(std::make_unique<juce::AudioParameterInt>(
                    ParameterID::feedbackLevel,
                    "Feedback Level",
                    0,
                    100,
                    50,
                    juce::AudioParameterIntAttributes().withLabel("%")
                ));
    layout.add(std::make_unique<juce::AudioParameterInt>(
                    ParameterID::wetLevel,
                    "Wet",
                    0,
                    100,
                    80,
                    juce::AudioParameterIntAttributes().withLabel("%")
                ));
    return layout;
}

void AudioPluginAudioProcessor::update() {
//    delayModule.setMaxDelayTime(maxDelayTimeParam->get());    
    delayModule.setDelayTime(0, lDelayTimeParam->get());
    delayModule.setDelayTime(1, rDelayTimeParam->get());
    delayModule.setWetLevel((float)wetLevelParam->get() * 0.01f);    
    delayModule.setFeedbackLevel((float)feedbackLevelParam->get() * 0.01);    
    std::printf("Changing some values\n");
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
  //  return new AudioPluginAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
