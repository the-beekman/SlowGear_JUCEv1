/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

//this is an edit made on my desktop

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "EditorCustomVisuals.h"

//==============================================================================

//The slider attachments need to be explicitly initialized here
SlowGear_JUCEv1AudioProcessorEditor::SlowGear_JUCEv1AudioProcessorEditor (SlowGear_JUCEv1AudioProcessor& p)
    :   AudioProcessorEditor (&p),
        audioProcessor (p),
        thresholdSlider(*audioProcessor.apvts.getParameter("Threshold dB"), "Threshold", "dB"), //This must be a pointer or it doesn't compile
        swellTimeSlider(*audioProcessor.apvts.getParameter("Swell Time"), "Swell Time", "s"),
        attackTimeSlider(*audioProcessor.apvts.getParameter("Envelope Attack Time"), "Attack", "s"),
        decayTimeSlider(*audioProcessor.apvts.getParameter("Envelope Decay Time"), "Decay","s"),
        thresholdSliderAttachment(audioProcessor.apvts, "Threshold dB", thresholdSlider),
        swellTimeSliderAttachment(audioProcessor.apvts, "Swell Time", swellTimeSlider),
        attackTimeSliderAttachment(audioProcessor.apvts, "Envelope Attack Time", attackTimeSlider),
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
    

    
    //juce::Rectangle<int> paddedBounds, thresholdSliderBounds, timeSliderBounds, buttonBounds, attackTimeSliderBounds, decayTimeSliderBounds;
    
//    g.setColour(juce::Colours::black);
//    g.fillRect(paddedBounds);
//
//    g.setColour(juce::Colours::blue);
//    g.fillRect(thresholdSliderBounds);
//
//    g.setColour(juce::Colours::orange);
//    g.fillRect(timeSliderBounds);
//
//    g.setColour(juce::Colours::lightgoldenrodyellow);
//    g.fillRect(buttonBounds);
//
//    g.setColour(juce::Colours::purple);
//    g.fillRect(attackTimeSliderBounds);
//
//    g.setColour(juce::Colours::green);
//    g.fillRect(decayTimeSliderBounds);
//
//    g.setColour(juce::Colours::seagreen);
//    g.fillRect( defaultEnvelopeButton.getBounds() );
//    g.setColour(juce::Colours::purple);
//    g.fillRect(customEnvelopeButton.getBounds());
//
//    g.setColour(juce::Colours::pink);
//    g.fillRect(buttonHeader.getBounds());
    

}

void SlowGear_JUCEv1AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    juce::Rectangle<int> windowBounds = getLocalBounds(); //the entire window
    windowWidth =  windowBounds.getWidth();
    windowHeight = windowBounds.getHeight();

     paddedBounds = windowBounds.reduced(windowWidth*windowPadding, windowHeight*windowPadding); //take a percentage off each side

    //rectangle.removeFrom___() returns what was removed, not what remains

    thresholdSliderBounds = paddedBounds.removeFromTop(0.5*windowHeight);
    //thresholdSliderBounds = thresholdSliderBounds.removeFromTop(windowHeight*windowPadding);
    thresholdSlider.setBounds(thresholdSliderBounds);

    timeSliderBounds = paddedBounds.removeFromTop(0.125*windowHeight);
    swellTimeSlider.setBounds(timeSliderBounds);
        
    buttonBounds = paddedBounds.removeFromLeft(0.3*windowWidth);
    initializeButtonHeader(buttonBounds);
    initializeModeButtons(buttonBounds);

    //The time sliders get half of the remaining height
     attackTimeSliderBounds = paddedBounds.removeFromTop(0.5*paddedBounds.getHeight());
    attackTimeSlider.setBounds(attackTimeSliderBounds);

    decayTimeSliderBounds = paddedBounds;
    decayTimeSlider.setBounds(decayTimeSliderBounds);

}

void SlowGear_JUCEv1AudioProcessorEditor::initializeButtonHeader(const juce::Rectangle<int> buttonArea)
{
    buttonHeader.setText("Envelope Follower", juce::NotificationType::dontSendNotification);
    buttonHeader.setBounds(buttonArea.getX(), buttonArea.getY()-3, buttonArea.getWidth(), buttonHeader.getFont().getHeight()+buttonHeader.getFont().getDescent());
    buttonHeader.setJustificationType(juce::Justification::centredTop);
}

//==============================================================================
void SlowGear_JUCEv1AudioProcessorEditor::initializeModeButtons(juce::Rectangle<int> buttonArea)
{
    
    defaultEnvelopeButton.setButtonText(std::string("Default"));
    customEnvelopeButton.setButtonText(std::string("Custom"));
    
    defaultButtonArea = buttonArea.removeFromTop(buttonArea.getHeight()*0.5);
    customButtonArea = buttonArea; //This is a redundant line
   
    int modeButtonHeight = 20;
    int modeButtonWidth = 50;//customEnvelopeButton.getBestWidthForHeight(modeButtonHeight);
    
    int buttonYOffset = defaultButtonArea.getHeight()/6-modeButtonHeight/2;
        
    defaultEnvelopeButton.setBounds(defaultButtonArea.getCentreX()-modeButtonWidth/2, defaultButtonArea.getCentreY()+buttonYOffset, modeButtonWidth, modeButtonHeight);
    customEnvelopeButton.setBounds(customButtonArea.getCentreX()-modeButtonWidth/2, customButtonArea.getCentreY()+buttonYOffset, modeButtonWidth, modeButtonHeight);
    
    defaultEnvelopeButton.setTooltip("Use default envelope follower.");
    customEnvelopeButton.setTooltip("Use custom envelope follower.");

    //We don't hard-code it because initialize gets called on resize and we want to keep the state
    defaultEnvelopeButton.setToggleState(envelopeMode == EnvelopeMode::Default, juce::NotificationType::dontSendNotification);
    customEnvelopeButton.setToggleState(envelopeMode == EnvelopeMode::Custom, juce::NotificationType::dontSendNotification);
    
    
    defaultEnvelopeButton.onClick = [&]() {
        envelopeMode = EnvelopeMode::Default;
        defaultEnvelopeButton.setToggleState(true, juce::NotificationType::dontSendNotification);
        customEnvelopeButton.setToggleState(false, juce::NotificationType::dontSendNotification);
    };
    
    customEnvelopeButton.onClick = [&]() {
        envelopeMode = EnvelopeMode::Custom;
        defaultEnvelopeButton.setToggleState(false, juce::NotificationType::dontSendNotification);
        customEnvelopeButton.setToggleState(true, juce::NotificationType::dontSendNotification);
    };
    
}
