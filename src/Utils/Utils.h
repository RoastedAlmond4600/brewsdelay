#pragma once
#include <JuceHeader.h>
template <typename T>
void castParameter(juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& id, T& dest) {
    dest = dynamic_cast<T>(apvts.getParameter(id.getParamID()));
    jassert(dest);
}
