/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

//this is an edit made on my desktop

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SlowGear_JUCEv1AudioProcessorEditor::SlowGear_JUCEv1AudioProcessorEditor (SlowGear_JUCEv1AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

SlowGear_JUCEv1AudioProcessorEditor::~SlowGear_JUCEv1AudioProcessorEditor()
{
}

//==============================================================================
void SlowGear_JUCEv1AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Welcome!", getLocalBounds(), juce::Justification::centred, 1);
}

void SlowGear_JUCEv1AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
