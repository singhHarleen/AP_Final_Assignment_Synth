/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include"synthBasic.h"

//==============================================================================
AP_Final_Assignment_SynthBitAudioProcessor::AP_Final_Assignment_SynthBitAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    parameters(*this, nullptr, "ParamTreeID", {
        std::make_unique<juce::AudioParameterFloat>("detune", "Detune (Hz)" , 0.0f, 20.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("filterCutoff", "Filter Cutoff" , 20.0, 18000.0, 200.0),
        std::make_unique<juce::AudioParameterFloat>("sineLfo", "LFO" , 0.0, 20.0, 0),
 })
{

    detuneParam = parameters.getRawParameterValue("detune"); 
    filterCutoffParam = parameters.getRawParameterValue("filterCutoff");
    sineLfoParam = parameters.getRawParameterValue("sineLfo");
        
    
    // constructor 
    for (int i = 0; i<voiceCount; i++)
    {
        mySynth.addVoice(new HsSynthVoice());
    }

    mySynth.addSound(new YourSynthSound);
   
}

AP_Final_Assignment_SynthBitAudioProcessor::~AP_Final_Assignment_SynthBitAudioProcessor()
{
}

//==============================================================================
const juce::String AP_Final_Assignment_SynthBitAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AP_Final_Assignment_SynthBitAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AP_Final_Assignment_SynthBitAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AP_Final_Assignment_SynthBitAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AP_Final_Assignment_SynthBitAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AP_Final_Assignment_SynthBitAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AP_Final_Assignment_SynthBitAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AP_Final_Assignment_SynthBitAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AP_Final_Assignment_SynthBitAudioProcessor::getProgramName (int index)
{
    return {};
}

void AP_Final_Assignment_SynthBitAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AP_Final_Assignment_SynthBitAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mySynth.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < voiceCount; i++)
    {
        HsSynthVoice* v = dynamic_cast<HsSynthVoice*>(mySynth.getVoice(i));
        v->hsSynthInitialise(sampleRate);
    }
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void AP_Final_Assignment_SynthBitAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AP_Final_Assignment_SynthBitAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void AP_Final_Assignment_SynthBitAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    for (int i = 0; i < voiceCount; i++)
    {
        HsSynthVoice* v = dynamic_cast<HsSynthVoice*>(mySynth.getVoice(i));
        v->setDetune(*detuneParam);
        v->setFilterCutoff(*filterCutoffParam);
        v->setLfoFreq(*sineLfoParam);
    }
  

    mySynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool AP_Final_Assignment_SynthBitAudioProcessor::hasEditor() const
{
    return false; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AP_Final_Assignment_SynthBitAudioProcessor::createEditor()
{
    return new AP_Final_Assignment_SynthBitAudioProcessorEditor (*this);
}

//==============================================================================
void AP_Final_Assignment_SynthBitAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AP_Final_Assignment_SynthBitAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AP_Final_Assignment_SynthBitAudioProcessor();
}
