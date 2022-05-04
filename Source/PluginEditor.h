/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class AP_Final_Assignment_SynthBitAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    AP_Final_Assignment_SynthBitAudioProcessorEditor (AP_Final_Assignment_SynthBitAudioProcessor&);
    ~AP_Final_Assignment_SynthBitAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AP_Final_Assignment_SynthBitAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AP_Final_Assignment_SynthBitAudioProcessorEditor)
};
