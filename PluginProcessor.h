/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

//this is an edit made on my desktop

#pragma once

#include <JuceHeader.h>

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

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SlowGear_JUCEv1AudioProcessor)
    
    int samplesPerBlock;
    double sampleRate;
    
    std::vector<double> gainRamp;
    double gainRampDurationSeconds;
    double gainRampDurationSecondsMax = 5.0;
    int gainRampNumSamples;
    std::vector<double> prepareMasterGainRamp(double sampleRate, double gainRampDurationSecondsMax);
    
    double envelopeAttackTimeMS = 0.1, envelopeDecayTimeMS = 10;
    double envelopeAttackTime;
    double envelopeDecayTime;
    std::vector<double> signalEnvelope;
    void calculateRCEnvelope(auto channelDataReadPtr);
    
    double impulseThreshold;
    int impulseIndex;
    int detectImpulseFromEnvelope(double f_threshold);
    
    void applyGainRamp();
};
