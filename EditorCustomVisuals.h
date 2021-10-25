
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


//==============================================================================

class CustomRotarySliderLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;
    
};


//These can be classes if we specify PUBLIC parent class
class CustomRotarySlider : public juce::Slider
{
public:
    CustomRotarySlider(juce::RangedAudioParameter& constructorRAP) : juce::Slider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxAbove), parameterPtr(&constructorRAP)
    {
        setLookAndFeel(&lookAndFeel);
    }
    ~CustomRotarySlider()
    {
        setLookAndFeel(nullptr); //Since set it in the constructor, we have to unset it in the destructor
    }
    
    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const {return textHeight;}
    
private:
    CustomRotarySliderLookAndFeel lookAndFeel;
    const int textHeight = 14;
    
    juce::RangedAudioParameter* parameterPtr; //We need to provide a pointer to a parameter so that the plugin knows which parameter the slider is controlling
    

};

class CustomHorizontalSlider : public juce::Slider
{
public:
    CustomHorizontalSlider() : juce::Slider(juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::TextEntryBoxPosition::TextBoxAbove)
    {
        
    }

};
