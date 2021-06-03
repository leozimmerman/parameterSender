/*
  ==============================================================================

   This file is part of the JUCE tutorials.
   Copyright (c) 2020 - Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

#pragma once

#include "GenericEditor.h"

class ValueProcessorComponent {
public:
    
    ValueProcessorComponent(int identifier, AudioProcessorValueTreeState* valueTreeState) {
        idx = identifier;
        sendParameter = valueTreeState->getRawParameterValue (IDs::StringWithIdx(IDs::send, idx));
        valueParameter  = valueTreeState->getRawParameterValue (IDs::StringWithIdx(IDs::value, idx));
    }
    
    int idx;
    
    std::atomic<float>* sendParameter = nullptr;
    std::atomic<float>* valueParameter  = nullptr;
};

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout(int valuesCount)
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    for (int i=0; i<valuesCount; ++i) {
        layout.add(std::make_unique<juce::AudioParameterFloat> (IDs::StringWithIdx(IDs::value, i),            // parameterID
                                                                IDs::StringWithIdx(IDs::valueName, i),            // parameter name
                                                                0.0f,              // minimum value
                                                                1.0f,              // maximum value
                                                                0.5f));
        layout.add(std::make_unique<juce::AudioParameterBool> (IDs::StringWithIdx(IDs::send, i),      // parameterID
                                                               IDs::StringWithIdx(IDs::sendName, i),     // parameter name
                                                               false));
    }
   
    return layout;
}


class ParameterSenderProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ParameterSenderProcessor()
        : parameters (*this, nullptr, juce::Identifier ("ParameterSender"), createParameterLayout(VALUES_NUMBER))
    {
        for (int i=0; i<VALUES_NUMBER; ++i) {
            auto comp = ValueProcessorComponent(i, &parameters);
            components.push_back(comp);
        }
    }

    //==============================================================================
    void prepareToPlay (double, int) override
    {


    }

    void releaseResources() override {}

    void processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
    {
        for (auto comp: components) {
            ///send osc
        }
    }

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override          { return new GenericEditor (*this, parameters); }
    bool hasEditor() const override                              { return true; }

    //==============================================================================
    const juce::String getName() const override                  { return "Parameter Sender"; }
    bool acceptsMidi() const override                            { return false; }
    bool producesMidi() const override                           { return false; }
    double getTailLengthSeconds() const override                 { return 0; }

    //==============================================================================
    int getNumPrograms() override                                { return 1; }
    int getCurrentProgram() override                             { return 0; }
    void setCurrentProgram (int) override                        {}
    const juce::String getProgramName (int) override             { return {}; }
    void changeProgramName (int, const juce::String&) override   {}

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override
    {
        auto state = parameters.copyState();
        std::unique_ptr<juce::XmlElement> xml (state.createXml());
        copyXmlToBinary (*xml, destData);
    }

    void setStateInformation (const void* data, int sizeInBytes) override
    {
        std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

        if (xmlState.get() != nullptr)
            if (xmlState->hasTagName (parameters.state.getType()))
                parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
    }

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState parameters;
    std::vector<ValueProcessorComponent> components;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterSenderProcessor)
};
