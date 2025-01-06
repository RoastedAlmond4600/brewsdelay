#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <JuceHeader.h>
#include "Delay/Delay.h"

namespace ParameterID {
    #define PARAMETER_ID(str) const juce::ParameterID str(#str, 1);
    PARAMETER_ID(lDelayTime);
    PARAMETER_ID(rDelayTime);
    PARAMETER_ID(wetLevel);
    PARAMETER_ID(feedbackLevel);
    PARAMETER_ID(syncToggle);
}
//==============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor,
                                        private juce::ValueTree::Listener
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    juce::AudioProcessorValueTreeState apvts {*this, NULL, "apvts", createParameterLayout()};

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
    Delay<float> delayModule;

    //Parameter Pointers
    juce::AudioParameterFloat* lDelayTimeParam;
    juce::AudioParameterFloat* rDelayTimeParam;
    juce::AudioParameterInt* wetLevelParam;
    juce::AudioParameterInt* feedbackLevelParam;
    juce::AudioParameterBool* syncToggleParam;
    
    //Parameter Tree Setup
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    std::atomic<bool> parameterChanged {true};
    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) { parameterChanged.store(true); }
    void update();

    //Tempo-Synced Variables.
    juce::AudioPlayHead* playHead;
    static constexpr float bpmDividend = 60000.f;
    inline void setupSync(juce::AudioPlayHead* _playHead) {
         
    }
};
