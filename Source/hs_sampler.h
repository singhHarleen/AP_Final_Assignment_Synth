/*
  ==============================================================================

    hs_sampler.h
    Created: 1 May 2022 2:06:27pm
    Author:  Harleen

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"


class hs_sampler : public juce::Synthesiser
{
public: 

    void samplerInit()
    {
        formatManager.registerBasicFormats();
       
       /* juce::File* file = new juce::File("D:\Sample Packs\Own Sounds\Designed Sounds\03_Textures\18_HS_VA_RainTexture_PS18.wav");
        std::unique_ptr<juce::AudioFormatReader> reader; 
        reader.reset(formatManager.createReaderFor(*file));

        juce::BigInteger samplrAllNotes; 
        samplrAllNotes.setRange(0, 128, true);

        addSound(new juce::SamplerSound("hsSamplr", *reader, samplrAllNotes, 60, 0, 0.1, 20.0));*/
    }

    void linkParametersSampler(std::atomic<float>* ptrToSamplerAttackParam)
    {
        samplerAttackParam = ptrToSamplerAttackParam;
    }

    
    void setSample(const void* sourceData, size_t sourceDataSize)
    {
        formatManager.registerBasicFormats();

        auto* reader = formatManager.createReaderFor(std::make_unique<juce::MemoryInputStream>(sourceData,
            sourceDataSize, false));

        juce::BigInteger samplrAllNotes;

        samplrAllNotes.setRange(0, 128, true);

        addSound(new juce::SamplerSound("hsSamplr", *reader, samplrAllNotes, 60, 1.5, 0.1, 20.0));

    }

  /*  void setSamplerGain (float _samplerOutputGain)
    {
        samplerOutputGain = _samplerOutputGain;
    }*/

private: 
    juce::AudioFormatManager formatManager; 
    
    std::atomic<float>* samplerOutputLevelParam;
    float samplerAttack;

    std::atomic<float>* samplerAttackParam;
   // float samplerOutputGain; 
};



