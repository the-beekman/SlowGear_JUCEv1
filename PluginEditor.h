/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SlowGear_JUCEv1AudioProcessorEditor)
};
