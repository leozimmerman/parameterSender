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
#include "OscManager.h"

class ValueProcessorComponent {
public:
    
    ValueProcessorComponent(int identifier, AudioProcessorValueTreeState* valueTreeState) {
        idx = identifier;
        sendParameter = valueTreeState->getRawParameterValue (IDs::StringWithIdx(IDs::send, idx));
        valueParameter  = valueTreeState->getRawParameterValue (IDs::StringWithIdx(IDs::value, idx));
    }
    
    juce::String getName() {
        return IDs::StringWithIdx(IDs::value, idx);
    }
    
    bool getSendActive() {
        return *sendParameter >= 0.5;
    }
    
    float getParameter() {
        return *valueParameter;
    }
    
private:
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
    layout.add(std::make_unique<juce::AudioParameterInt> (IDs::oscPort,            // parameterID
                                                          IDs::oscPortName,            // parameter name
                                                          MIN_OSC_PORT,              // minimum value
                                                          MAX_OSC_PORT,              // maximum value
                                                          DEFAULT_OSC_PORT));
    
   
    return layout;
}


class ParameterSenderProcessor  : public juce::AudioProcessor, public OscHostListener, private juce::AudioProcessorValueTreeState::Listener
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
        parameters.addParameterListener(IDs::oscPort, this);
        parameters.addParameterListener(IDs::mainId, this);
    }

    //==============================================================================
    void prepareToPlay (double, int) override {}
    void releaseResources() override {}
    
    void parameterChanged (const juce::String& param, float value) override {
        if (param == IDs::oscPort) {
            oscPortHasChanged(value);
        }
    }
    
    void oscMainIDHasChanged (juce::String newOscMainID) override {
        oscManager.setMaindId(newOscMainID);
    }

    void oscHostHasChanged (juce::String newOscHostAdress) override {
        oscManager.setOscHost(newOscHostAdress);
    }

    void oscPortHasChanged(int newOscPort) {
        oscManager.setOscPort(newOscPort);
    }

    void processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override {
        for (auto comp: components) {
            if (comp.getSendActive()) {
                oscManager.sendValue(comp.getParameter(), comp.getName());
            }
        }
    }

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override          {
        editor = new GenericEditor (*this, parameters);
        editor->addOscListener(this);
        return editor;
    }
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

        if (xmlState.get() != nullptr) {
            auto type = parameters.state.getType();
            if (xmlState->hasTagName (type)) {
                auto newValueTree = juce::ValueTree::fromXml (*xmlState);
                parameters.replaceState (newValueTree);
                editor->updateOscLabelsTexts(true);
            }
        }
            
    }

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState parameters;
    std::vector<ValueProcessorComponent> components;
    
    OscManager oscManager;
    GenericEditor* editor;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterSenderProcessor)
};
