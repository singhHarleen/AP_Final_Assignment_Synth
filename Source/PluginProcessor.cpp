/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include"synthBasic.h"
#include"hs_sampler.h"

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
        
        // osc 1
        std::make_unique<juce::AudioParameterChoice>("osc1Choices", "Oscilator 1", juce::StringArray({"Saw", "Square", "Triangle", "Sine"}),0),
        std::make_unique<juce::AudioParameterFloat>("osc1Voulume", "Osc 1 Level", 0.0, 1.0, 1.0),

        // osc 2
        std::make_unique<juce::AudioParameterChoice>("osc2Choices", "Oscilator 2", juce::StringArray({"Saw", "Square", "Triangle", "Sine"}),0),
        std::make_unique<juce::AudioParameterFloat>("osc2Voulume", "Osc 2 Level", 0.0, 1.0, 1.0),

        // sub osc
        std::make_unique<juce::AudioParameterChoice>("subOscChoices", "Sub Oscilator", juce::StringArray({"Triangle", "Sine"}),0),
        std::make_unique<juce::AudioParameterFloat>("subOscVolume", "Sub Osc Level", 0.0, 1.0, 1.0),
        
        //noise osc 
        std::make_unique<juce::AudioParameterFloat>("noiseOScVolume", "Noise Osc Level", 0.0, 1.0, 0.0),
        std::make_unique<juce::AudioParameterChoice>("noiseChoices", "Noise", juce::StringArray({"White", "Filtered"}),0),

        // IIRF 
        std::make_unique<juce::AudioParameterChoice>("filterChoices", "Filter", juce::StringArray({"Lowpass", "Highpass", "Notch"}),0),
        std::make_unique<juce::AudioParameterFloat>("filterCutoff", "Filter Cutoff" , 20.0, 20000.0, 18000.0),
        std::make_unique<juce::AudioParameterFloat>("filterQ", "Q Factor", 0.1, 20.0, 1.0),
       
        // modulators 
        std::make_unique<juce::AudioParameterFloat>("detune", "Detune (Hz)" , 0.0f, 20.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("lfo", "LFO" , 0.0, 20.0, 0.15),

        // fx 
        std::make_unique<juce::AudioParameterFloat>("delayTime", "Delay Time", 0.0, 2.0, 0.0),
        std::make_unique<juce::AudioParameterFloat>("delayFeedback", "Delay Feedback", 0.0, 1.0, 0.0),

        // Amp Env 
        std::make_unique<juce::AudioParameterFloat>("ampAttack", "Amp Attack", 0.0, 5.0, 0.5),
        std::make_unique<juce::AudioParameterFloat>("ampDecay", "Amp Decay", 0.0, 5.0, 2.0),
        std::make_unique<juce::AudioParameterFloat>("ampSustain", "Amp Sustain", 0.0, 1.0, 1.0),
        std::make_unique<juce::AudioParameterFloat>("ampRelease", "Amp Release", 0.0, 5.0, 2.0),
        
        // Output Gain 
        std::make_unique<juce::AudioParameterFloat>("outputGain", "Main Output", 0.0, 1.0, 0.3),

        std::make_unique<juce::AudioParameterFloat>("samplerAttack", "Sampler Attack (ms)", 0.0, 5.0, 1.0),

        std::make_unique<juce::AudioParameterChoice>("audioSampleChoices", "Audio Samples", juce::StringArray({"Sample 1", "Sample 2"}),0),

        
 })  
{
    // volume params 
    osc1VolParam = parameters.getRawParameterValue("osc1Voulume");
    osc2VolParam = parameters.getRawParameterValue("osc2Voulume");
    noiseOscVolParam = parameters.getRawParameterValue("noiseOScVolume");
    subOscVolParam = parameters.getRawParameterValue("subOscVolume");
    mainOutputGainParam = parameters.getRawParameterValue("outputGain");

    osc1ChoiceParam = parameters.getRawParameterValue("osc1Choices");
    osc2ChoiceParam = parameters.getRawParameterValue("osc2Choices");

    subOscChoiceParam = parameters.getRawParameterValue("subOscChoices");
    filterChoiceParam = parameters.getRawParameterValue("filterChoices");
    noiseOscChoiceParam = parameters.getRawParameterValue("noiseChoices");

    // modulator params 
    lfoParam = parameters.getRawParameterValue("lfo");
    detuneParam = parameters.getRawParameterValue("detune"); 

    // filter params 
    filterCutoffParam = parameters.getRawParameterValue("filterCutoff");
    filterQParam = parameters.getRawParameterValue("filterQ");
    
    // Amp env params
    ampAttackParam = parameters.getRawParameterValue("ampAttack");
    ampDecayParam = parameters.getRawParameterValue("ampDecay");
    ampSustainParam = parameters.getRawParameterValue("ampSustain"); 
    ampReleaseParam = parameters.getRawParameterValue("ampRelease");

    samplerAttackParam = parameters.getRawParameterValue("samplerAttack");

    // fx params     
    delayTimeParam = parameters.getRawParameterValue("delayTime");
    delayFeedbackParam = parameters.getRawParameterValue("delayFeedback");

    audioSampleChoiceParam = parameters.getRawParameterValue("audioSampleChoices");

    //osc1ChoiceParam = parameters.getRawParameterValue("osc1Choices");

    
  /*  osc1ChoiceParam = parameters.getRawParameterValue("osc1Choices");
    osc2ChoiceParam = parameters.getRawParameterValue("osc2Choices");
    subOscChoiceParam = parameters.getRawParameterValue("subOscChoices");
    filterChoiceParam = parameters.getRawParameterValue("filterChoices");
    noiseOscChoiceParam = parameters.getRawParameterValue("noiseChoices");
    */
 
    
    // constructor for the synth 
    for (int i = 0; i<voiceCount; i++)
    {
        mySynth.addVoice(new HsSynthVoice());
    }

    mySynth.addSound(new YourSynthSound);

    // constructor for the sampler 
    for (int i = 0; i < voiceCountSampler; i++)
    {
        mySampler.addVoice(new juce::SamplerVoice());
    }

    // selects the audio file for the sampler 
    mySampler.setSample(BinaryData::_17_HS_ACD_AmbientDrone_PS17_wav, BinaryData::_17_HS_ACD_AmbientDrone_PS17_wavSize);

    if (*audioSampleChoiceParam == 1)
    {
        mySampler.setSample(BinaryData::_16_HS_HXString_OmniousDrone_PS16_wav, BinaryData::_16_HS_HXString_OmniousDrone_PS16_wavSize);
    }
    
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
    mySampler.setCurrentPlaybackSampleRate(sampleRate);
    

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
        v->setLfoFreq(*lfoParam);
        v->setOutputGain(*mainOutputGainParam);
        v->setAmpAttack(*ampAttackParam); 
        v->setAmpDecay(*ampDecayParam);
        v->setAmpSustain(*ampSustainParam);
        v->setAmpRelease(*ampReleaseParam);
        v->setOsc1Vol(*osc1VolParam); 
        v->setOsc2Vol(*osc2VolParam); 
        v->setOscNoiseVol(*noiseOscVolParam); 
        v->setOscSubVol(*subOscVolParam);
        v->setFilterQ(*filterQParam);
        v->setDelayTime(*delayTimeParam);
        v->setDelayFeedback(*delayFeedbackParam);
        v->linkParameters(osc1ChoiceParam, osc2ChoiceParam, subOscChoiceParam, filterChoiceParam, noiseOscChoiceParam, lfoParam);
    }

   

    
    /*for (int i = 0; i < voiceCountSampler; i++)
    {
    mySampler* voice = dynamic_cast<mySampler*>(mySampler.getVoice(i));
    }*/
    /*for (int i = 0; i < voiceCountSampler; i++)
    {
        mySampler* voice = 

    }*/

    mySynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    //mySampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

}



//==============================================================================
bool AP_Final_Assignment_SynthBitAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AP_Final_Assignment_SynthBitAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void AP_Final_Assignment_SynthBitAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AP_Final_Assignment_SynthBitAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(parameters.state.getType()))
        {
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AP_Final_Assignment_SynthBitAudioProcessor();
}
