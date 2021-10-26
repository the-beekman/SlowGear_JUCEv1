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
        attackTimeSlider(*audioProcessor.apvts.getParameter("Envelope Attack Time"), "Follower Attack", "ms"),
        decayTimeSlider(*audioProcessor.apvts.getParameter("Envelope Decay Time"), "Follower Reset","ms"),
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
    getLookAndFeel().setDefaultSansSerifTypefaceName("Arial");
    
    setToolTips();
    
    setSize (windowWidth, windowHeight);
}

SlowGear_JUCEv1AudioProcessorEditor::~SlowGear_JUCEv1AudioProcessorEditor()
{
}

//==============================================================================
void SlowGear_JUCEv1AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.fillAll( juce::Colour {62,62,62});
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

    topSliderBounds = paddedBounds.removeFromTop(0.5*windowHeight);
    juce::Rectangle<int> topSliderBoundsHolder = topSliderBounds;
    thresholdSliderBounds = topSliderBoundsHolder.removeFromLeft(0.5*topSliderBounds.getWidth());
    //thresholdSliderBounds = thresholdSliderBounds.removeFromTop(windowHeight*windowPadding);
    thresholdSlider.setBounds(thresholdSliderBounds);
    
    timeSliderBounds = topSliderBoundsHolder;
    swellTimeSlider.setBounds(timeSliderBounds);
        
    buttonBounds = paddedBounds.removeFromLeft(0.3*windowWidth);
    //The buttons and header should be aligned with the sliders, so we initialize them after the sliders

    //The time sliders get half of the remaining height
    attackTimeSliderBounds = paddedBounds.removeFromTop(0.5*paddedBounds.getHeight());
    attackTimeSlider.setBounds(attackTimeSliderBounds);

    decayTimeSliderBounds = paddedBounds;
    decayTimeSlider.setBounds(decayTimeSliderBounds);
    
    initializeButtonHeader(buttonBounds);
    initializeModeButtons(buttonBounds);

}

void SlowGear_JUCEv1AudioProcessorEditor::initializeButtonHeader(const juce::Rectangle<int> buttonArea)
{
    buttonHeader.setText("Envelope Follower", juce::NotificationType::dontSendNotification);
    buttonHeader.setFont(14);
    //buttonHeader.setBounds(buttonArea.getX(), buttonArea.getY()-3, buttonArea.getWidth(), buttonHeader.getFont().getHeight()+buttonHeader.getFont().getDescent());
    buttonHeader.setBounds(buttonArea.getX(), attackTimeSlider.getY()-1, buttonArea.getWidth(), buttonHeader.getFont().getHeight()+buttonHeader.getFont().getDescent());
    buttonHeader.setJustificationType(juce::Justification::centredTop);
}

//==============================================================================
void SlowGear_JUCEv1AudioProcessorEditor::initializeModeButtons(juce::Rectangle<int> buttonArea)
{
    
    defaultEnvelopeButton.setButtonText(std::string("Default"));
    customEnvelopeButton.setButtonText(std::string("Custom"));
    
    defaultButtonArea = buttonArea.removeFromTop(buttonArea.getHeight()*0.5);
    customButtonArea = buttonArea;
   
    int modeButtonHeight = 8+defaultEnvelopeButton.getLookAndFeel().getLabelFont(buttonHeader).getHeight(); //It looks good with 8
    int modeButtonWidth = defaultEnvelopeButton.getBestWidthForHeight(modeButtonHeight);
    
    //int buttonYOffset = defaultButtonArea.getHeight()/6-modeButtonHeight/2;
        
//    defaultEnvelopeButton.setBounds(defaultButtonArea.getCentreX()-modeButtonWidth/2, defaultButtonArea.getCentreY()+buttonYOffset, modeButtonWidth, modeButtonHeight);
//    customEnvelopeButton.setBounds(customButtonArea.getCentreX()-modeButtonWidth/2, customButtonArea.getCentreY()+buttonYOffset, modeButtonWidth, modeButtonHeight);
    
    int buttonFontSize = defaultEnvelopeButton.getLookAndFeel().getTextButtonFont(defaultEnvelopeButton, modeButtonHeight).getHeight();
    int buttonFontAscent = defaultEnvelopeButton.getLookAndFeel().getTextButtonFont(defaultEnvelopeButton, modeButtonHeight).getAscent();
    
    defaultEnvelopeButton.setBounds(defaultButtonArea.getCentreX()-modeButtonWidth/2,
                                    defaultButtonArea.getY()+attackTimeSlider.getHeight()/2-1, //it looks nicer with -1
                                    modeButtonWidth,
                                    modeButtonHeight);
    
    customEnvelopeButton.setBounds(customButtonArea.getCentreX()-modeButtonWidth/2,
                                   decayTimeSlider.getY()+decayTimeSlider.getHeight()/2-1, //it looks nicer with -1
                                   modeButtonWidth,
                                   modeButtonHeight);
    

    //We don't hard-code it because initialize gets called on resize and we want to keep the state
    defaultEnvelopeButton.setToggleState(envelopeMode == EnvelopeMode::Default, juce::NotificationType::dontSendNotification);
    customEnvelopeButton.setToggleState(envelopeMode == EnvelopeMode::Custom, juce::NotificationType::dontSendNotification);
    
    defaultEnvelopeButton.onClick = [&]() {
        if (envelopeMode == EnvelopeMode::Custom)
        {
            attackTimeSavedValue = attackTimeSlider.getValue();
            decayTimeSavedValue = decayTimeSlider.getValue();
            envelopeMode = EnvelopeMode::Default;
            defaultEnvelopeButton.setToggleState(true, juce::NotificationType::dontSendNotification);
            customEnvelopeButton.setToggleState(false, juce::NotificationType::dontSendNotification);
            attackTimeSlider.setValue(0.1);
            decayTimeSlider.setValue(10.0);
            attackTimeSlider.setEnabled(false);
            decayTimeSlider.setEnabled(false);
        }
    };
    
    customEnvelopeButton.onClick = [&]() {
        if (envelopeMode == EnvelopeMode::Default)
        {
            envelopeMode = EnvelopeMode::Custom;
            defaultEnvelopeButton.setToggleState(false, juce::NotificationType::dontSendNotification);
            customEnvelopeButton.setToggleState(true, juce::NotificationType::dontSendNotification);
            attackTimeSlider.setEnabled(true);
            decayTimeSlider.setEnabled(true);
            attackTimeSlider.setValue(attackTimeSavedValue);
            decayTimeSlider.setValue(decayTimeSavedValue);
        }
    };
    
}

void SlowGear_JUCEv1AudioProcessorEditor::setToolTips()
{
    thresholdSlider.setTooltip("Signal envelope level to trigger swell.");
    swellTimeSlider.setTooltip("0-99% rise time of signal.");
    attackTimeSlider.setTooltip("Envelope follower attack time. Longer values allow more of the initial signal transient.");
    decayTimeSlider.setTooltip("Envelope follower decay time. Longer values are less sensitive to repeated notes. Shorter values may cause false swells.");
    defaultEnvelopeButton.setTooltip("Use default envelope follower settings.");
    customEnvelopeButton.setTooltip("Use custom envelope follower settings.");
}
