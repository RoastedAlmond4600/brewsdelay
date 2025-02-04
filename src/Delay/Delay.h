#pragma once
#include <JuceHeader.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include "DelayLine.h"

template <typename Type, size_t maxNumChannels=2>
class Delay {
    public:
        Delay() {
            setMaxDelayTime(2.f); 
            std::printf("Delay Lines: %ld\n", delayLines.size());
            std::printf("Delay Time Samples: %ld\n", delayTimesSample.size());
            std::printf("Delay Time: %ld\n", delayTimes.size());
        }
        ~Delay() {}
        void reset() {
            for (auto& filter : filters) {
                filter.reset();
            }
            for (auto& delayLine : delayLines) {
                delayLine.clear();
            }
        }
        void prepare(const juce::dsp::ProcessSpec& spec) {
            jassert(spec.numChannels < maxNumChannels);
            sampleRate = (Type)spec.sampleRate;
            updateDelayLineSize();
            updateDelayTime();
            filterCoefs = juce::dsp::IIR::Coefficients<Type>::makeFirstOrderHighPass (sampleRate, Type(1e3));
            for (auto& filter : filters) {
                filter.prepare(spec);
                filter.coefficients = filterCoefs; 
            }

            std::printf("Delay Lines: %ld\n", delayLines.size());
            std::printf("Delay Time Samples: %ld\n", delayTimesSample.size());
            std::printf("Delay Time: %ld\n", delayTimes.size());
        }
        template <typename ProcessContext>
        void process(const ProcessContext& context) noexcept {
            auto& inputBlock = context.getInputBlock();
            auto& outputBlock = context.getOutputBlock();
            auto numChannels = outputBlock.getNumChannels();
            auto numSamples = outputBlock.getNumSamples();

            jassert(inputBlock.getNumSamples() == numSamples);
            jassert(inputBlock.getNumChannels() == numChannels);

            //run through the channels
            for (size_t ch = 0; ch < numChannels; ++ch) {
                auto* input = inputBlock.getChannelPointer(ch);
                auto* output = outputBlock.getChannelPointer(ch);
                auto& dline = delayLines[ch];
                auto delayTime = delayTimesSample[ch];
                auto& filter = filters[ch];
                //run through the buffer
                for (size_t sample = 0; sample < numSamples; ++sample) {
                   //process magic. 
                   auto delayedSample = filter.processSample(dline.get(delayTime));
                   auto inputSample = input[sample];
                   auto dlineInputSample = std::atan(inputSample + feedbackLevel * delayedSample);
                   dline.push(dlineInputSample);
                   auto outputSample = std::atan(inputSample + wetLevel * delayedSample);
                   output[sample] = outputSample;
                }
            }
        }
        //Setters
        void setDelayTime(size_t channel, Type newValue) {
            delayTimes[channel] = newValue; 
            updateDelayTime();
        }
        void setMaxDelayTime(Type maxDelayTime_) {
            maxDelayTime = maxDelayTime_;
            updateDelayLineSize();
        }

        void setWetLevel(Type wetLevel_) {
            wetLevel = wetLevel_;
        }

        void setFeedbackLevel(Type feedbackLevel_) {
            feedbackLevel = feedbackLevel_;
        }

        void setSampleRate(Type sampleRate_) {
            sampleRate = sampleRate_;
        }

        //Ancillary Functions
        void updateDelayLineSize() {
            auto delayLineSamples = (size_t)std::ceil(maxDelayTime * sampleRate);
            for (auto& delayLine : delayLines) {
                delayLine.resize(delayLineSamples);
            }
        }

        void updateDelayTime() noexcept {
            for (size_t ch = 0; ch < maxNumChannels; ++ch) {
                delayTimesSample[ch] = (size_t) juce::roundToInt(delayTimes[ch] * sampleRate); 
            }
        }
    private:
        //Variables
        Type maxDelayTime {Type(2)};
        Type wetLevel {Type(0)};
        Type feedbackLevel {Type(0)};
        Type sampleRate {Type(44.1e3)};
        //Containers
        std::array<DelayLine<Type>, maxNumChannels> delayLines;
        std::array<size_t, maxNumChannels> delayTimesSample;
        //For each channel, what are the corresponding delays.
        std::array<Type, maxNumChannels> delayTimes;
        //Effects
        std::array<juce::dsp::IIR::Filter<Type>, maxNumChannels> filters;
        typename juce::dsp::IIR::Coefficients<Type>::Ptr filterCoefs;
};

template class Delay<float>;
template class Delay<double>;
