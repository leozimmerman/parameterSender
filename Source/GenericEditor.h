
#pragma once

#define VALUES_NUMBER 4
#include "OscManager.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

enum
{
    paramControlHeight = 50,
    paramLabelWidth    = 80,
    paramSliderWidth   = 300,
    paramToggleWidth = 75,
    oscSectionHeight = 35,
    portSliderWidth = 100,
    maindIdLabelWidth = 100,
    hostLabelWidth = 200
};

namespace IDs
{
    static juce::String value  { "value" };
    static juce::String valueName  { "Value" };

    static juce::String send  { "send" };
    static juce::String sendName  { "Send" };

    static juce::String oscPort  { "oscPort" };
    static juce::String oscPortName  { "Osc Port" };


    static juce::String StringWithIdx(juce::String ID, int idx) {
        return ID +":" + juce::String(idx);
    }
}

//==============================================================================
class ValueEditorComponent {
public:
    
    ValueEditorComponent(int identifier, juce::AudioProcessorEditor* editor, AudioProcessorValueTreeState* valueTreeState) {
        idx = identifier;
        valueLabel = new juce::Label();
        valueSlider = new juce::Slider();
        sendButton = new juce::ToggleButton();
        
        valueLabel->setText (IDs::StringWithIdx(IDs::valueName, idx), juce::dontSendNotification);
        editor->addAndMakeVisible (valueLabel);

        editor->addAndMakeVisible (valueSlider);
        valueAttachment = new SliderAttachment (*valueTreeState, IDs::StringWithIdx(IDs::value, idx), *valueSlider);

        sendButton->setButtonText (IDs::StringWithIdx(IDs::sendName, idx));
        editor->addAndMakeVisible (sendButton);
        sendAttachment = new ButtonAttachment (*valueTreeState, IDs::StringWithIdx(IDs::send, idx), *sendButton);
    }
    
    void resized(Rectangle<int> bounds) {
        auto rect = bounds;
        rect.setHeight(paramControlHeight);
        rect.setY(paramControlHeight * (idx));
        valueLabel ->setBounds (rect.removeFromLeft (paramLabelWidth));
        sendButton->setBounds (rect.removeFromRight(paramToggleWidth));
        valueSlider->setBounds (rect);
    }

//private:
    int idx;
    
    juce::Label* valueLabel;
    juce::Slider* valueSlider;
    SliderAttachment* valueAttachment;

    juce::ToggleButton* sendButton;
    ButtonAttachment* sendAttachment;
};
//==============================================================================
class GenericEditor : public juce::AudioProcessorEditor, public juce::Label::Listener
{
public:

    GenericEditor (juce::AudioProcessor& parent, juce::AudioProcessorValueTreeState& vts)
        : AudioProcessorEditor (parent),
          valueTreeState (vts)
    {
       
        for (int i=0; i<VALUES_NUMBER; ++i) {
            auto comp = ValueEditorComponent(i, this, &valueTreeState);
            components.push_back(comp);
        }
        
        
        addAndMakeVisible (hostLabel);
        hostLabel.setFont (juce::Font (20.0, juce::Font::bold));
        hostLabel.setComponentID("hostLabel");
        hostLabel.setEditable(true);
        
        hostLabel.setColour (juce::Label::textColourId, juce::Colours::lightgreen);
        hostLabel.setJustificationType (juce::Justification::centredRight);
        hostLabel.addListener(this);
        
        addAndMakeVisible (mainIDLabel);
        mainIDLabel.setComponentID("mainIDLabel");
        mainIDLabel.setFont (juce::Font (20.0, juce::Font::bold));
        mainIDLabel.setEditable(true);
        
        mainIDLabel.setColour (juce::Label::textColourId, juce::Colours::lightblue);
        mainIDLabel.setJustificationType (juce::Justification::centredRight);
        mainIDLabel.addListener(this);
        
        addAndMakeVisible (portSlider);
        portSlider.setSliderStyle(juce::Slider::IncDecButtons);
        portAttachment.reset (new SliderAttachment (valueTreeState, IDs::oscPort, portSlider));
        
        juce::String hostAddress = DEFAULT_OSC_HOST;
        ///processorState.getLastHostAddress(hostAddress);
        
        juce::String mainId = DEFAULT_OSC_MAIN_ID;
        ///processorState.getLastMaindId(mainId);

        mainIDLabel.setText (mainId, juce::dontSendNotification);
        hostLabel.setText (hostAddress, juce::dontSendNotification);
        
        
        setSize (paramSliderWidth + paramLabelWidth + paramToggleWidth, (paramControlHeight * VALUES_NUMBER) + oscSectionHeight);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        for (auto comp: components) {
            comp.resized(bounds);
        }
        
        ///***processorState.setLastEditorSize (getWidth(), getHeight());
        
        int spacing = 10;
        int yPos = getHeight() - oscSectionHeight;
        mainIDLabel.setBounds (spacing,
                               yPos,
                               maindIdLabelWidth,
                               oscSectionHeight);
        portSlider.setBounds(getWidth() - portSliderWidth - spacing,
                             yPos,
                             portSliderWidth,
                             oscSectionHeight);
        hostLabel.setBounds (getWidth() - portSliderWidth - hostLabelWidth - spacing*2,
                             yPos,
                             hostLabelWidth,
                             oscSectionHeight);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    }
    
    void labelTextChanged (juce::Label* labelThatHasChanged) override {
        if (labelThatHasChanged->getComponentID() == "hostLabel") {
            //processorState.setOscIPAdress(labelThatHasChanged->getText());
            std::cout << "Hola" << std::endl;
        } else if (labelThatHasChanged->getComponentID() == "mainIDLabel") {
            //processorState.setOscMainID(labelThatHasChanged->getText());
            std::cout << "Chau" << std::endl;
        }
    }

private:
    juce::AudioProcessorValueTreeState& valueTreeState;
    std::vector<ValueEditorComponent> components;
    
    juce::Label hostLabel;
    juce::Label mainIDLabel;
    
    juce::Slider portSlider;
    std::unique_ptr<SliderAttachment> portAttachment;
};
