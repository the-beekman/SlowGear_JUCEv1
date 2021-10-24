/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

//this is an edit made on my desktop

#pragma once

#include <JuceHeader.h>

struct Settings
{
    float threshold { 0 }, swellTime {0}, envelopeDecayTime {0};
};



//==============================================================================
/**
*/
class SlowGear_JUCEv1AudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SlowGear_JUCEv1AudioProcessor();
    ~SlowGear_JUCEv1AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    const float gainRampDurationSecondsMax = 5.0;

    
    //==============================================================================
    //How Matkat Music's example parameter layout was made
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts
    {
        *this, //AudioProcessor &processorToConnectTo,
        nullptr, //UndoManager *undoManagerToUse,
        "Parameters", //const Identifier &valueTreeType,
        createParameterLayout() //ParameterLayout parameterLayout
            //This function is called first, before we get to the private section. All variables used in createParameterLayout MUST be initialized BEFORE this line
    };
    
    Settings getAllSettings(juce::AudioProcessorValueTreeState& apValueTreeState);
    float getThresholdFromAPVTS(juce::AudioProcessorValueTreeState& apValueTreeState);
    float getSwellTimeFromAPVTS(juce::AudioProcessorValueTreeState& apValueTreeState);
    float getEnvelopeDecayTimeFromAPVTS(juce::AudioProcessorValueTreeState& apValueTreeState);
    
    float getEnvelopeAttackTimeFromAPVTS(juce::AudioProcessorValueTreeState& apValueTreeState);
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SlowGear_JUCEv1AudioProcessor)
    
    int samplesPerBlock;
    double sampleRate;
    
    void updateSettings(juce::AudioProcessorValueTreeState& apValueTreeState);
    
    std::vector<double> gainRamp;
    float gainRampSwellTimeSeconds = 1; //was 1
    
    int gainRampNumSamples;
    std::vector<double> prepareMasterGainRamp(double sampleRate, double gainRampDurationSecondsMax);
    
    double envelopeAttackTimeMS = 0.1, envelopeDecayTimeMS = 10;
    double envelopeAttackTime;
    double envelopeDecayTime;
    std::vector<double> signalEnvelope;
    template<typename dataType>
    void calculateRCEnvelope(dataType* channelDataReadPtr);
    
    double impulseThreshold = 0.02; //was 0.02
    int impulseIndex;
    int detectImpulseFromEnvelope(double f_threshold);
    
    template<typename dataType>
    void applyGainRamp(dataType* bufferWritePointer, int f_impulseIndex, double f_gainRampDurationSeconds);
};
