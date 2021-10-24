/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

//this is an edit made on my desktop

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SlowGear_JUCEv1AudioProcessor::SlowGear_JUCEv1AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::mono(), true)
                     #endif
                       )
#endif
{
}

SlowGear_JUCEv1AudioProcessor::~SlowGear_JUCEv1AudioProcessor()
{
}

//==============================================================================
const juce::String SlowGear_JUCEv1AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SlowGear_JUCEv1AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SlowGear_JUCEv1AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SlowGear_JUCEv1AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SlowGear_JUCEv1AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SlowGear_JUCEv1AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SlowGear_JUCEv1AudioProcessor::getCurrentProgram()
{
    return 0;
}

void SlowGear_JUCEv1AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SlowGear_JUCEv1AudioProcessor::getProgramName (int index)
{
    return {};
}

void SlowGear_JUCEv1AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SlowGear_JUCEv1AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    this->sampleRate = sampleRate;
    this->samplesPerBlock = samplesPerBlock;
    
    gainRamp = prepareMasterGainRamp(sampleRate, this->gainRampDurationSecondsMax);

    //reset the signalEnvelope to have the same size as samplesPerBlock
    signalEnvelope.clear(); //clearing the envelope may be unnecessary
    signalEnvelope.resize(samplesPerBlock);

    updateSettings(apvts);
    
    //define the 0-63% attack/decay times for the envelope follower
    envelopeAttackTime = std::exp(-1.0 / (sampleRate*envelopeAttackTimeMS*0.001) );
    envelopeDecayTime = std::exp(-1.0 / (sampleRate*envelopeDecayTimeMS*0.001) );
}

void SlowGear_JUCEv1AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SlowGear_JUCEv1AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SlowGear_JUCEv1AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
    
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    updateSettings(apvts);
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelDataWrite = buffer.getWritePointer(channel);
        auto* channelDataRead = buffer.getReadPointer(channel);
        
        calculateRCEnvelope(channelDataRead);
        impulseIndex = detectImpulseFromEnvelope(impulseThreshold);
        applyGainRamp(channelDataWrite, impulseIndex, gainRampSwellTimeSeconds);
        
    }
}

//==============================================================================
bool SlowGear_JUCEv1AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SlowGear_JUCEv1AudioProcessor::createEditor()
{
    return new SlowGear_JUCEv1AudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void SlowGear_JUCEv1AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SlowGear_JUCEv1AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// Custom functions
//==============================================================================

juce::AudioProcessorValueTreeState::ParameterLayout SlowGear_JUCEv1AudioProcessor::createParameterLayout()
{
    /* PARAMETERS:
     * Threshold: The envelope threshold to activate the swell
     * Swell Time: The time it takes the swell to go from 0% to 99%
     * Envelope Decay Time: The 0-68% fall time of the envelope follower. This would be set by a trimpot that is not accessible from the outside in an actual Slow Gear
     */
    juce::AudioProcessorValueTreeState::ParameterLayout parameterLayout;
    
    //This wants std::unique_ptr<Items> items
    //There is no juce::AudioParameterDouble
    parameterLayout.add(std::make_unique<juce::AudioParameterFloat>(
        "Threshold dB",//const String &parameterID,
        "Threshold dB",//const String &parameterName,
        juce::NormalisableRange<float>(-60.f, 0.f, 0.5f, 1.f),//NormalisableRange<float> normalisableRange(rangeStart, rangeEnd, intervalValue, skewFactor)
            //The skew factor is log(0.5)/log( (midpoint-min)/(max-min) ). Since this is a dB slider already in logarithmic units, we don't need to skew
        -34.f//float defaultValue
        ) );
    parameterLayout.add(std::make_unique<juce::AudioParameterFloat>(
        "Swell Time",//const String &parameterID,
        "Swell Time",//const String &parameterName,
        juce::NormalisableRange<float>(0.1f, gainRampDurationSecondsMax, 0.1f, 1.f),
        1.f//float defaultValue
        ) );
    
    //This is a risky parameter. High values are less likely to react correctly to closely-timed notes. Low values are more likely to cause the envelope to go below-and-above the threshold in the same frame, causing low volume glitches.
    //10 is a good value from the testing
    parameterLayout.add(std::make_unique<juce::AudioParameterFloat>(
        "Envelope Decay Time",//const String &parameterID,
        "Envelope Decay Time",//const String &parameterName,
        juce::NormalisableRange<float>(1.f, 20.f, 0.1f, 2.2698f),//NormalisableRange<float> normalisableRange(rangeStart, rangeEnd, intervalValue, skewFactor)
            //This skew factor sets the midpoint to be 15ms
        10.f//float defaultValue
        ) );
    
    return parameterLayout;
}

void SlowGear_JUCEv1AudioProcessor::updateSettings(juce::AudioProcessorValueTreeState& apValueTreeState)
{
    envelopeDecayTimeMS = getEnvelopeDecayTimeFromAPVTS(apvts);
    impulseThreshold =  juce::Decibels::decibelsToGain( getThresholdFromAPVTS(apvts) );
    gainRampSwellTimeSeconds = getSwellTimeFromAPVTS(apvts);
}

Settings SlowGear_JUCEv1AudioProcessor::getAllSettings(juce::AudioProcessorValueTreeState& apValueTreeState)
{
    Settings settings;
    
    //Because the raw parameter values are atomic, we need to call their load funct to get them
    settings.threshold = apValueTreeState.getRawParameterValue("Threshold dB")->load();
    settings.swellTime = apValueTreeState.getRawParameterValue("Swell Time")->load();
    settings.envelopeDecayTime = apValueTreeState.getRawParameterValue("Envelope Decay Time")->load();
    
    return settings;
}

float SlowGear_JUCEv1AudioProcessor::getThresholdFromAPVTS(juce::AudioProcessorValueTreeState& apValueTreeState)
{
    return apValueTreeState.getRawParameterValue("Threshold dB")->load();
}

float SlowGear_JUCEv1AudioProcessor::getSwellTimeFromAPVTS(juce::AudioProcessorValueTreeState& apValueTreeState)
{
    return apValueTreeState.getRawParameterValue("Swell Time")->load();
}

float SlowGear_JUCEv1AudioProcessor::getEnvelopeDecayTimeFromAPVTS(juce::AudioProcessorValueTreeState& apValueTreeState)
{
    return apValueTreeState.getRawParameterValue("Envelope Decay Time")->load();
}

std::vector<double> SlowGear_JUCEv1AudioProcessor::prepareMasterGainRamp(double f_sampleRate, double f_gainRampDurationSecondsMax)
{
    //We interpolate between samples of the master gain ramp to get the gain value for faster swell times (compared to the maximum).
    //prefix f_ denotes the variable has local function scope
    
    
    //weird stuff here - gainRampNumSamples is the CLASS variable, while sampleRate and the duration is the FUNCTION ARGUMENTS. Justification at-the-time is that I want to guaruntee that INDEPENDENT VARIABLES sampleRate and gainRampDurationSecondsMax are set correctly, since gainRampNumSamples and vector gainRamp are DEPENDENT VARIABLES on them.
    gainRampNumSamples = static_cast<int>(f_sampleRate*f_gainRampDurationSecondsMax);
    
    std::vector<double> f_gainRamp;
    f_gainRamp.resize(gainRampNumSamples);
    
    for (int i = 0; i < gainRampNumSamples; ++i)
    {
        f_gainRamp.at(i) = 1.0-exp(-4.6*i/gainRampNumSamples);
    }
    
    return f_gainRamp;
}

template<typename dataType>
void SlowGear_JUCEv1AudioProcessor::calculateRCEnvelope(dataType* channelDataReadPtr)
{
    //Process (Adapted from Pirkle's book):
    // 1) Take ABS of the sample. We could also square it here to get the R/MS envelope, but we're not going to.
    // 2) Compare with previous envelope value. If the new sample is greater, apply attackTime. Else apply decayTime.
    // 3) Make sure the result is non-negative
    
    //First sample is handled differently because we're comparing to the end of the previous envelope (which was preserved because the signalEnvelope is a private class variable)
    float currentSample = std::abs( channelDataReadPtr[0] );
    if (currentSample > signalEnvelope.at(samplesPerBlock-1) )
    {
        signalEnvelope.at(0) = envelopeAttackTime * (signalEnvelope.at(samplesPerBlock-1) - currentSample ) + currentSample;
    }
    else
    {
        signalEnvelope.at(0) = envelopeDecayTime * (signalEnvelope.at(samplesPerBlock-1) - currentSample ) + currentSample;
    }
    
    //Now the rest of the samples
    for(int i = 1; i < samplesPerBlock; ++i)
    {
        //Are these taylor expansions?
        currentSample = std::abs( channelDataReadPtr[i] );
        if (currentSample > signalEnvelope.at(i-1) )
        {
            signalEnvelope.at(i) = envelopeAttackTime * (signalEnvelope.at(i-1) - currentSample ) + currentSample;
        }
        else
        {
            signalEnvelope.at(i) = envelopeDecayTime * (signalEnvelope.at(i-1) - currentSample ) + currentSample;
        }
        
        //make sure the envelope is non-negative
        if (signalEnvelope.at(i) < 0)
        {
            signalEnvelope.at(i) = 0;
        }
    } //end sample for loop
    
} //end calculateRCEnvelope

int SlowGear_JUCEv1AudioProcessor::detectImpulseFromEnvelope(double f_threshold)
{
    //f_threshold is used instead of the class variable to ensure it remains at the same value for the entire processing time, regardless of the user possibly changing the threshold in the middle of this function's runtime
    
    static bool swellInProgress = false; //static so that the state persists between frames
    //we cannot make swellInProgress a class variable that the applyGainRamp function also uses because we do not detect the impulse and apply the gain ramp within the same function. With the current function flow, we will only know swellInProgress state at the END of the buffer if it was a class variable
    
    for (int i = 0; i < samplesPerBlock; ++i)
    {
        if ( !swellInProgress && (signalEnvelope.at(i) >= f_threshold) )
        {
            swellInProgress = true;
            //return i; //The index where the envelope went over the threshold
            if (i) // >= 1, saves us a compare
            {
                return std::max( 0, static_cast<int>( i - signalEnvelope.at(i)/(signalEnvelope.at(i)-signalEnvelope.at(i-1)) )); //perform 1 iteration of the secant method. float->int cast acts as a floor. std::max makes sure that the index is non-negative
            }
            else
            {
                return i; //if the threshold-crossing is at the first index, the index we want to return is actually in the previous frame. We don't operate on previous frames, so just return i (0).
            }
        }
        if ( signalEnvelope.at(i) < 0.85*f_threshold )
        {
            //if it goes below the threshold, reset the swell state
            swellInProgress = false;
        }
    }//end for
    
    return -1; //if it gets here, the envelope is never above the threshold OR there is a swell in progress that started in a previous frame
}

template<typename dataType>
void SlowGear_JUCEv1AudioProcessor::applyGainRamp(dataType* bufferWritePointer, int f_impulseIndex, double f_gainRampDurationSeconds)
{
    //For now we will calculate the gain ramp sample-by-sample.
    static int rampIndex = 0;
    
    bool impulseInFrame = ( f_impulseIndex >= 0 );
    bool previousFramePartOfSwell = !( rampIndex <= 0 || rampIndex >= gainRampNumSamples ); //If rampIndex is zero or underflow, or if rampIndex overflows, then it's not being swelled. Mind the negation ! operator
    
    double gainValue;
    // CASES:
    // 1) Impulse in frame, previous frame not part of swell
    if (impulseInFrame && !previousFramePartOfSwell)
    {
       //We want all the samples up to impulseIndex to be unaffected, then start applying the ramp starting from the sample's impulseIndex. We also reset the gain ramp index.
        rampIndex = 0;
        for (int bufferIndex = f_impulseIndex; bufferIndex < samplesPerBlock; ++bufferIndex)
        {
            if (rampIndex < gainRampNumSamples)
            {
                gainValue = 1-std::exp( (-4.6*rampIndex) / (f_gainRampDurationSeconds*sampleRate) );
                bufferWritePointer[bufferIndex] = gainValue * bufferWritePointer[bufferIndex];
                ++rampIndex;
            }
//            else
//            {
//                //Multiply by 1
//                bufferWritePointer[bufferIndex] = bufferWritePointer[bufferIndex];
//            }
            
        } //end for loop
    } //end if (impulseInFrame && !previousFramePartOfSwell)
    
    // 2) Impulse in frame, previous frame is part of swell
    else if (impulseInFrame && previousFramePartOfSwell)
    {
        //We want to continue the previous frame's swell, then restart a new swell at impulseIndex
        for ( int bufferIndex = 0; bufferIndex < f_impulseIndex; ++bufferIndex)
        {
            if (rampIndex < gainRampNumSamples)
            {
                gainValue = 1-std::exp( (-4.6*rampIndex) / (f_gainRampDurationSeconds*sampleRate) );
                bufferWritePointer[bufferIndex] = gainValue * bufferWritePointer[bufferIndex];
                ++rampIndex;
            }
            //else multiply by 1
        } //end for loop (1st section)
        
        //Now we are at impulseIndex. The following is a copy-paste of case 1
        rampIndex = 0;
        for (int bufferIndex = f_impulseIndex; bufferIndex < samplesPerBlock; ++bufferIndex)
        {
            if (rampIndex < gainRampNumSamples)
            {
                gainValue = 1-std::exp( (-4.6*rampIndex) / (f_gainRampDurationSeconds*sampleRate) );
                bufferWritePointer[bufferIndex] = gainValue * bufferWritePointer[bufferIndex];
                ++rampIndex;
            } //end if
            //else multiply by 1
        } //end for loop (2nd section)
    } //end else if (impulseInFrame && previousFramePartOfSwell)
    
    // 3) Impulse not in frame, previous frame not part of swell
    else if (!impulseInFrame && !previousFramePartOfSwell)
    {
        //Do nothing to the audio. Ensure the rampIndex is 0.
        rampIndex = 0;
    }
    
    // 4) Impulse not in frame, previous frame is part of swell
    else if (!impulseInFrame && previousFramePartOfSwell)
    {
        // Continue the previous swell through the entire duration of the frame
        
        for ( int bufferIndex = 0; bufferIndex < samplesPerBlock; ++bufferIndex)
        {
            if (rampIndex < gainRampNumSamples)
            {
                gainValue = 1-std::exp( (-4.6*rampIndex) / (f_gainRampDurationSeconds*sampleRate) );
                bufferWritePointer[bufferIndex] = gainValue * bufferWritePointer[bufferIndex];
                ++rampIndex;
            }
            //else multiply by 1
        } //end for loop (1st section)
        
    }
    
    else
    {
        DBG("Error in applyGainRamp: Reached unexpected case of impulseInFrame and previousFramePartOfSwell");
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SlowGear_JUCEv1AudioProcessor();
}
