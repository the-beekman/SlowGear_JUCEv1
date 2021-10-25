/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

//this is an edit made on my desktop

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
    
    
    CustomRotarySlider thresholdSlider;
    
    CustomHorizontalSlider swellTimeSlider, attackTimeSlider, decayTimeSlider;
    
    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;
    
    Attachment  thresholdSliderAttachment,
                swellTimeSliderAttachment,
                attackTimeSliderAttachment,
                decayTimeSliderAttachment;
    
    std::vector<juce::Component*> getAllComponents()
    {
        return
        {
            &thresholdSlider,
            &swellTimeSlider,
            &attackTimeSlider,
            &decayTimeSlider
        };
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SlowGear_JUCEv1AudioProcessorEditor)
};
