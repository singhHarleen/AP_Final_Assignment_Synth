/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "synthBasic.h"
#include "hs_sampler.h"

//==============================================================================
/**
*/
class AP_Final_Assignment_SynthBitAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    AP_Final_Assignment_SynthBitAudioProcessor();
    ~AP_Final_Assignment_SynthBitAudioProcessor() override;

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

    juce::AudioProcessorValueTreeState parameters; 
    std::atomic<float>* detuneParam;
    std::atomic<float>* filterCutoffParam;
    std::atomic<float>* filterQParam;
    std::atomic<float>* sineLfoParam;
    std::atomic<float>* mainOutputGainParam;

    // AD params 
    std::atomic<float>* ampAttackParam;
    std::atomic<float>* ampDecayParam;

    // amplitude params for sound generators 
    std::atomic<float>* osc1VolParam;
    std::atomic<float>* osc2VolParam;
    std::atomic<float>* noiseOscVolParam;
    std::atomic<float>* subOscVolParam;

    std::atomic<float>* delayTimeParam;     // param for delay time 
    std::atomic<float>* delayFeedbackParam;     // param for delay feedback

    std::atomic<float>* samplerOutputLevelParam;

    // smooth parameter objects
    //==============================================
   
    std::atomic<float>* osc1ChoiceParam;        // choice beteen different waveforms for oscilator 1
    std::atomic<float>* osc2ChoiceParam;        // choice beteen different waveforms for oscilator 2
    std::atomic<float>* subOscChoiceParam;      // choice beteen sine and triangle waveform
    std::atomic<float>* noiseOscChoiceParam;    // choice between different types of noise 
    std::atomic<float>* filterChoiceParam;      // choice between hp, lp, and bp filters

    juce::Synthesiser mySynth; // synth object 
    hs_sampler mySampler; // sampler
    int voiceCount = 16;
    int voiceCountSampler = 16;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AP_Final_Assignment_SynthBitAudioProcessor)
};
