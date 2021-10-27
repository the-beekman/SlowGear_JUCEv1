
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
    CustomRotarySlider(juce::RangedAudioParameter& constructorRAP, juce::String sliderTitle, juce::String suffix) : juce::Slider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox), parameterPtr(&constructorRAP)
    {
        setLookAndFeel(&lookAndFeel);
        this->sliderTitle = sliderTitle;
        this->suffix = juce::String{suffix};
        getLookAndFeel().setDefaultSansSerifTypefaceName("Arial");
    }
    ~CustomRotarySlider()
    {
        setLookAndFeel(nullptr); //Since set it in the constructor, we have to unset it in the destructor
    }
    
    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const {return textHeight;}
    int getTitleTextHeight() const {return titleTextHeight;}
    juce::String getDisplayString() {return juce::String(getValue(), 1)+juce::String(" ")+suffix;};
    juce::String getSliderTitle() {return sliderTitle;}
    void drawSliderTitle(juce::Graphics& g);
    
private:
    CustomRotarySliderLookAndFeel lookAndFeel;
    const int textHeight = 14;
    const int titleTextHeight = 14;
    juce::String sliderTitle;
    juce::String suffix;
    
    juce::RangedAudioParameter* parameterPtr; //We need to provide a pointer to a parameter so that the plugin knows which parameter the slider is controlling
    

};

//==============================================================================

class CustomHorizontalSliderLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const juce::Slider::SliderStyle, juce::Slider& slider) override;
    
};

class CustomHorizontalSlider : public juce::Slider
{
public:
    CustomHorizontalSlider(juce::RangedAudioParameter& constructorRAP, juce::String sliderTitle, juce::String suffix) : juce::Slider(juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::TextEntryBoxPosition::NoTextBox), parameterPtr(&constructorRAP)
    {
        setLookAndFeel(&lookAndFeel);
        this->sliderTitle = sliderTitle;
        this->suffix = juce::String{suffix};
        getLookAndFeel().setDefaultSansSerifTypefaceName("Arial");
    }
    ~CustomHorizontalSlider()
    {
        setLookAndFeel(nullptr); //Since set it in the constructor, we have to unset it in the destructor
    }
    
    void drawSliderInformation(juce::Graphics& g);
    
    juce::String getDisplayString() {return juce::String(getValue(), 2)+juce::String(" ")+suffix;}
    juce::String getSliderTitle() {return sliderTitle;}
    int getTextHeight() {return textHeight;}
    int getTitleTextHeight() {return titleTextHeight;}
    
    //void paint(juce::Graphics& g) override;
private:
    CustomHorizontalSliderLookAndFeel lookAndFeel;
    const int textHeight = 14;
    const int titleTextHeight = 14;
    juce::String suffix;
    
    juce::String sliderTitle;
    
    juce::RangedAudioParameter* parameterPtr; //We need to provide a pointer to a parameter so that the plugin knows which parameter the slider is controlling
    

};
