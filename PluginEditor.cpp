/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

//this is an edit made on my desktop

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

//The slider attachments need to be explicitly initialized here
SlowGear_JUCEv1AudioProcessorEditor::SlowGear_JUCEv1AudioProcessorEditor (SlowGear_JUCEv1AudioProcessor& p)
    :   AudioProcessorEditor (&p),
        audioProcessor (p),
        thresholdSliderAttachment(audioProcessor.apvts, "Threshold dB", thresholdSlider),
        swellTimeSliderAttachment(audioProcessor.apvts, "Swell Time", swellTimeSlider),
        //attackTimeSliderAttachment(audioProcessor.apvts, "Envelope Attack Time", attackTimeSlider),
        decayTimeSliderAttachment(audioProcessor.apvts, "Envelope Decay Time", decayTimeSlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    for (juce::Component* comp : getAllComponents())
    {
        addAndMakeVisible(comp);
    }
    
    setSize (windowWidth, windowHeight);
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
    
    auto windowBounds = getLocalBounds(); //the entire window
    
    auto paddedBounds = windowBounds.reduced(windowWidth*windowPadding, windowHeight*windowPadding); //take a percentage off each side
    
    //rectangle.removeFrom___() returns what was removed, not what remains
    
    auto thresholdSliderBounds = paddedBounds.removeFromTop(0.5*windowHeight);
    //thresholdSliderBounds = thresholdSliderBounds.removeFromTop(windowHeight*windowPadding);
    thresholdSlider.setBounds(thresholdSliderBounds);
    

    auto timeSliderBounds = paddedBounds.removeFromTop(0.15*windowHeight);
    swellTimeSlider.setBounds(timeSliderBounds);
        
    auto buttonBounds = paddedBounds.removeFromLeft(0.3*windowWidth);
    
    //The time sliders get half of the remaining height
    auto attackTimeSliderBounds = paddedBounds.removeFromTop(0.5*paddedBounds.getHeight());
    attackTimeSlider.setBounds(attackTimeSliderBounds);
    
    auto decayTimeSliderBounds = paddedBounds;
    decayTimeSlider.setBounds(decayTimeSliderBounds);
    
}
