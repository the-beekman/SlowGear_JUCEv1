/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/


#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "EditorCustomVisuals.h"



//==============================================================================
class SlowGear_JUCEv1AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SlowGear_JUCEv1AudioProcessorEditor (SlowGear_JUCEv1AudioProcessor&);
    ~SlowGear_JUCEv1AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SlowGear_JUCEv1AudioProcessor& audioProcessor;
    
    int windowHeight = 300, windowWidth = 400;
    float windowPadding = 0.05;
    
    juce::Rectangle<int> defaultButtonArea, customButtonArea;
    
    CustomRotarySlider thresholdSlider, swellTimeSlider;
    
    CustomHorizontalSlider attackTimeSlider, decayTimeSlider;
    
    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;
    
    Attachment  thresholdSliderAttachment,
                swellTimeSliderAttachment,
                attackTimeSliderAttachment,
                decayTimeSliderAttachment;
    
    int envelopeMode = EnvelopeMode::Custom;
    juce::TextButton defaultEnvelopeButton, customEnvelopeButton;
    juce::Label buttonHeader;
    void initializeButtonHeader(const juce::Rectangle<int> buttonArea);
    void initializeModeButtons(juce::Rectangle<int> buttonArea);
    
    juce::Rectangle<int> paddedBounds, topSliderBounds, thresholdSliderBounds, timeSliderBounds, buttonBounds, attackTimeSliderBounds, decayTimeSliderBounds;
    
    juce::TooltipWindow tooltipWindow {this, 500}; //2nd arg is ms before it appears
    
    void setToolTips();
    
    double attackTimeSavedValue = 0.1, decayTimeSavedValue = 10.0; //defaults
    
    std::vector<juce::Component*> getAllComponents()
    {
        return
        {
            &thresholdSlider,
            &swellTimeSlider,
            &attackTimeSlider,
            &decayTimeSlider,
            &buttonHeader,
            &defaultEnvelopeButton,
            &customEnvelopeButton,
            &tooltipWindow
        };
    }
    
    enum EnvelopeMode
    {
        Default,
        Custom,
        NumOptions
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SlowGear_JUCEv1AudioProcessorEditor)
};
