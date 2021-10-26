#include "EditorCustomVisuals.h"

#define ROTARY_SLIDER_TICK_THICKNESS 2.f
#define ROTARY_SLIDER_SETTING_THICKNESS 10.f

#define LINEAR_SLIDER_TICK_THICKNESS 3.f
#define LINEAR_SLIDER_SETTING_THICKNESS 10.f

#define SETTING_COLOR juce::Colours::cyan
#define UNFILLED_COLOR juce::Colours::black
#define TICK_COLOR juce::Colours::white
#define TITLE_COLOR juce::Colours::white
#define LABEL_COLOR juce::Colours::white


//CustomRotarySliderLookAndFeel's methods
//==============================================================================

void CustomRotarySliderLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    //This is called by paint. The function arguments are provided by paint.
    /*
     What I want:
        A circle in the center of the area
        A line extending from the center of the circle to the edge of the circle that rotates with the setting
        A thin arc slightly beyond the circle that tracks the possible area
        A thick arc outside the circle that tracks the setting
     */

    //The center circle
    auto circleBounds = juce::Rectangle<float>(x,y,width,height);
    g.setColour(juce::Colour(34u,46u,63u)); //was (55u,69u,79u)
    g.fillEllipse(circleBounds);
    //g.fillRect(circleBounds);
    
    auto circleCenter = circleBounds.getCentre();

    //The center line tick
    jassert(rotaryStartAngle < rotaryEndAngle);
    auto sliderAngleRadians = juce::jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);
    
    auto circleRadius = std::abs(circleCenter.getY()-circleBounds.getY());
    
    int tickHeight = circleRadius*0.333;
    
    juce::Rectangle<float> tickLine;
    tickLine.setLeft(circleCenter.getX() - ROTARY_SLIDER_TICK_THICKNESS/2);
    tickLine.setRight(circleCenter.getX() + ROTARY_SLIDER_TICK_THICKNESS/2);
    tickLine.setTop(circleBounds.getY());
    tickLine.setBottom(circleBounds.getY() + tickHeight);
    
    
    
    //Rotate it by assigning the rectangle to a path, and applying an affine transform rotation to the path.
    juce::Path tickPath;
    tickPath.addRectangle(tickLine);
    tickPath.applyTransform( juce::AffineTransform().rotated(sliderAngleRadians, circleCenter.getX(),circleCenter.getY() ) );
    
    g.setColour(TICK_COLOR);
    g.fillPath(tickPath);
    
    //The thin arc
    auto thinArcRadius = circleRadius+ROTARY_SLIDER_TICK_THICKNESS/2;
    
    juce::Path backgroundArc;
    backgroundArc.addCentredArc (circleCenter.getX(),
                                 circleCenter.getY(),
                                 thinArcRadius, //radiusX
                                 thinArcRadius, //radiusY
                                 0.0f, //rotationOfEllipse
                                 rotaryStartAngle,
                                 rotaryEndAngle,
                                 true //startAsNewSubPath
                                 );
    g.setColour(UNFILLED_COLOR);
    g.strokePath (backgroundArc, juce::PathStrokeType (ROTARY_SLIDER_TICK_THICKNESS, juce::PathStrokeType::curved, juce::PathStrokeType::square));
    
    //The thick arc
    
    auto settingArcRadius = circleRadius+ROTARY_SLIDER_SETTING_THICKNESS/2;
    
    juce::Path settingArc;
    settingArc.addCentredArc (circleCenter.getX(),
                                 circleCenter.getY(),
                                settingArcRadius, //radiusX
                               settingArcRadius, //radiusY
                                 0.0f, //rotationOfEllipse
                                 rotaryStartAngle,
                              sliderAngleRadians,
                                 true //startAsNewSubPath
                                 );
    g.setColour(SETTING_COLOR);
    g.strokePath (settingArc, juce::PathStrokeType (ROTARY_SLIDER_SETTING_THICKNESS, juce::PathStrokeType::curved, juce::PathStrokeType::butt));
    
    //We want to draw the text label from within this function. In order to do that, we need to make sure that it's actually a custom slider that called this draw function. If we can cast slider as a CustomRotarySlider, then we can use whatever class methods we want
    if (auto* customRotarySlider = dynamic_cast<CustomRotarySlider*>(&slider))
    {
        auto circleCenter = circleBounds.getCentre();
        
       
        auto text = customRotarySlider->getDisplayString();
        auto strWidth = g.getCurrentFont().getStringWidth(text);
        
        juce::Rectangle<float> textBoxRectangle;
        textBoxRectangle.setSize(strWidth+4, customRotarySlider->getTextHeight() + 2);
        textBoxRectangle.setCentre(circleCenter);
        
        g.setColour(LABEL_COLOR);
        g.drawFittedText(text, textBoxRectangle.toNearestInt(), juce::Justification::centred, 1);
        
        customRotarySlider->drawSliderTitle(g);
    }
    

}


//CustomRotarySlider's methods
//==============================================================================

void CustomRotarySlider::paint(juce::Graphics& g)
{
    float rotaryStartAngle = 1.2f*juce::MathConstants<float>::pi;
    float rotaryEndAngle = 2.8f*juce::MathConstants<float>::pi;
    
    auto sliderBounds = getSliderBounds();
    
    auto range = getRange();
    
    getLookAndFeel().drawRotarySlider(g,
                                      sliderBounds.getX(),
                                      sliderBounds.getY(),
                                      sliderBounds.getWidth(),
                                      sliderBounds.getHeight(),
                                      juce::jmap(getValue(),range.getStart(), range.getEnd(), 0.0, 1.0),//float sliderPosProportional, although jmap expects doubles,
                                      rotaryStartAngle,
                                      rotaryEndAngle,
                                      *this //Slider&
                                      );
    
}

juce::Rectangle<int> CustomRotarySlider::getSliderBounds() const
{
    //return getLocalBounds();
    
    auto localBounds = getLocalBounds();
    auto size = juce::jmin(localBounds.getWidth(), localBounds.getHeight());
    
    size -= getTitleTextHeight() * 2 + ROTARY_SLIDER_SETTING_THICKNESS;
    
    
    juce::Rectangle<int> sliderBounds;
    sliderBounds.setSize(size,size);
    sliderBounds.setCentre(localBounds.getCentreX(),localBounds.getCentreY());
    sliderBounds.setY(localBounds.getBottom()-size);
    size = (sliderBounds.getHeight() - getTitleTextHeight());
    sliderBounds.setSize(size,size);
    sliderBounds.setCentre(localBounds.getCentreX(), sliderBounds.getCentreY());
    // sliderBounds.setBottom(localBounds.getBottom()); //this line EXTENDS the bottom
    
    
    return sliderBounds;
}

void CustomRotarySlider::drawSliderTitle(juce::Graphics& g)
{
    auto areaBounds = getLocalBounds();
    
    areaBounds.setHeight(getTitleTextHeight()*2 - ROTARY_SLIDER_SETTING_THICKNESS);
    
    
    g.setColour(TITLE_COLOR);
    g.setFont(this->getTitleTextHeight());
    g.drawFittedText(this->getSliderTitle(), areaBounds, juce::Justification::centred, 1);
}


//CustomHorizontalSliderLookAndFeel's methods
//==============================================================================
//void CustomHorizontalSliderLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style, juce::Slider& slider)
void CustomHorizontalSliderLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style, juce::Slider& slider)
{

    if (auto* customHorizontalSlider = dynamic_cast<CustomHorizontalSlider*>(&slider))
    {
        customHorizontalSlider->drawSliderInformation(g);
    }
    
    y = y+height/6; //bias the height a bit down to make room for the title
    
    auto trackWidth = juce::jmin (6.0f, slider.isHorizontal() ? (float) height * 0.25f : (float) width * 0.25f);

    juce::Point<float> startPoint (slider.isHorizontal() ? (float) x : (float) x + (float) width * 0.5f,
                            slider.isHorizontal() ? (float) y + (float) height * 0.5f : (float) (height + y));

    juce::Point<float> endPoint (slider.isHorizontal() ? (float) (width + x) : startPoint.x,
                          slider.isHorizontal() ? startPoint.y : (float) y);

    //The background range
    juce::Path backgroundTrack;
    backgroundTrack.startNewSubPath (startPoint);
    backgroundTrack.lineTo (endPoint);
    g.setColour (UNFILLED_COLOR);
    g.strokePath (backgroundTrack, { trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::butt });
    

    //The setting track
    juce::Path valueTrack;
    juce::Point<float> minPoint, valuePoint;
    

    auto valueX = slider.isHorizontal() ? sliderPos : ((float) x + (float) width * 0.5f);
    auto valueY = slider.isHorizontal() ? ((float) y + (float) height * 0.5f) : sliderPos;

    minPoint = startPoint;
    valuePoint = { valueX, valueY };


    valueTrack.startNewSubPath (minPoint);
    valueTrack.lineTo (valuePoint);
    //g.setColour (slider.findColour (Slider::trackColourId));
    g.setColour(SETTING_COLOR);
    g.strokePath (valueTrack, { trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::butt });
    
    
    //The tick
    //auto thumbWidth = getSliderThumbRadius (slider);
    auto tickWidth = LINEAR_SLIDER_TICK_THICKNESS;
    auto tickHeight = 2.5*trackWidth;
    //g.setColour (slider.findColour (Slider::thumbColourId));
    g.setColour(TICK_COLOR);
    g.fillRect (juce::Rectangle<float> (static_cast<float> (tickWidth), static_cast<float> (tickHeight)).withCentre (valuePoint));
    
}

//CustomHorizontalSlider's methods
//==============================================================================

void CustomHorizontalSlider::drawSliderInformation(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    bounds = bounds.reduced(12, 0); //this magic number accounts for 12 pixels of padding on the sides of the slider.
    
    
    g.setColour(TITLE_COLOR);
    g.setFont(this->getTitleTextHeight());
    g.drawFittedText(getSliderTitle(), bounds, juce::Justification::topLeft, 1);
    
    g.setColour(LABEL_COLOR);
    g.setFont(this->getTextHeight());
    g.drawFittedText(getDisplayString(), bounds, juce::Justification::topRight, 1);
    
}

//void CustomHorizontalSlider::paint(juce::Graphics& g)
//{
//
//    setSliderSnapsToMousePosition (true);
//    auto bounds = getLocalBounds();
//    auto range = getRange();
//
//    //The drawLinearSlider function that juce provides in the LookAndFeel expects us to provide the slider position AS A POINT ON THE SCREEN. We have to
//    float sliderPostionProportional = juce::jmap(getValue(),range.getStart(), range.getEnd(), 0.0, 1.0);
//    auto sliderPosition = sliderPostionProportional*bounds.getWidth() + bounds.getX();
//
//    getLookAndFeel().drawLinearSlider (g,
//                      bounds.getX(),
//                      bounds.getY(),
//                      bounds.getWidth(),
//                      bounds.getHeight(),
//                      this.getLinearSliderPos (getValue()),
//                      NULL, //used in the 2- and 3- value versions
//                      NULL, //used in the 2- and 3- value versions
//                      juce::Slider::SliderStyle::LinearHorizontal,
//                      *this);
//}
