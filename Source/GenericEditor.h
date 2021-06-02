
#pragma once

//==============================================================================
class GenericEditor : public juce::AudioProcessorEditor
{
public:
    enum
    {
        paramControlHeight = 40,
        paramLabelWidth    = 80,
        paramSliderWidth   = 300,
        paramToggleWidth = 75
    };

    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    GenericEditor (juce::AudioProcessor& parent, juce::AudioProcessorValueTreeState& vts)
        : AudioProcessorEditor (parent),
          valueTreeState (vts)
    {
        valueLabel.setText ("Value", juce::dontSendNotification);
        addAndMakeVisible (valueLabel);

        addAndMakeVisible (valueSlider);
        gainAttachment.reset (new SliderAttachment (valueTreeState, "value", valueSlider));

        activeButton.setButtonText ("Active");
        addAndMakeVisible (activeButton);
        invertAttachment.reset (new ButtonAttachment (valueTreeState, "active", activeButton));
        
        int parametersCount = 1;
        setSize (paramSliderWidth + paramLabelWidth + paramToggleWidth, juce::jmax (100, paramControlHeight * parametersCount));
    }

    void resized() override
    {
        auto r = getLocalBounds();

        auto gainRect = r.removeFromTop (paramControlHeight);
        valueLabel .setBounds (gainRect.removeFromLeft (paramLabelWidth));
        activeButton.setBounds (gainRect.removeFromRight(paramToggleWidth));
        valueSlider.setBounds (gainRect);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    }

private:
    juce::AudioProcessorValueTreeState& valueTreeState;

    juce::Label valueLabel;
    juce::Slider valueSlider;
    std::unique_ptr<SliderAttachment> gainAttachment;

    juce::ToggleButton activeButton;
    std::unique_ptr<ButtonAttachment> invertAttachment;
};
