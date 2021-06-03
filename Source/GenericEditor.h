
#pragma once

#define VALUES_NUMBER 4

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

enum
{
    paramControlHeight = 50,
    paramLabelWidth    = 80,
    paramSliderWidth   = 300,
    paramToggleWidth = 75
};

namespace IDs
{
    static juce::String value  { "value" };
    static juce::String valueName  { "Value" };

    static juce::String send  { "send" };
    static juce::String sendName  { "Send" };


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
        
        valueSlider->setColour(01, juce::Colour(255, 0, 0)); // = juce::Colour(255, 0, 0);
        
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
class GenericEditor : public juce::AudioProcessorEditor
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
        setSize (paramSliderWidth + paramLabelWidth + paramToggleWidth, paramControlHeight * VALUES_NUMBER);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        for (auto comp: components) {
            comp.resized(bounds);
        }
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    }

private:
    juce::AudioProcessorValueTreeState& valueTreeState;
    std::vector<ValueEditorComponent> components;
};
