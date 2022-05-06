/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "synthBasic.h"

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
    std::atomic<float>* sineLfoParam;
    std::atomic<float>* mainOutputGainParam;

    // ADSR params 
    std::atomic<float>* ampAttackParam;
    std::atomic<float>* ampDecayParam;
    std::atomic<float>* ampSustainParam;
    std::atomic<float>* ampReleaseParam;


    std::atomic<float>* osc1ChoiceParam;        // choice beteen different waveforms for oscilator 1
    std::atomic<float>* osc2ChoiceParam;        // choice beteen different waveforms for oscilator 2
    std::atomic<float>* subOscChoiceParam;      // choice beteen sine and triangle waveform
    std::atomic<float>* noiseOscChoiceParam;    // choice between different types of noise 
    std::atomic<float>* filterChoiceParam;      // choice between hp, lp, and bp filters




    juce::Synthesiser mySynth; 
    int voiceCount = 16;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AP_Final_Assignment_SynthBitAudioProcessor)
};
