/*
  ==============================================================================

    YourSynthesiser.h
    Created: 7 Mar 2020 4:27:57pm
    Author:  Tom Mudd

  ==============================================================================
*/

#pragma once
#include "hs_oscilators.h"
#include "PluginProcessor.h"

// ===========================
// ===========================
// SOUND
class YourSynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    //--------------------------------------------------------------------------
    bool appliesToChannel(int) override { return true; }
};




// =================================
// =================================
// Synthesiser Voice - your synth code goes in here

/*!
 @class HsSynthVoice
 @abstract struct defining the DSP associated with a specific voice.
 @discussion multiple HsSynthVoice objects will be created by the Synthesiser so that it can be played polyphicially

 @namespace none
 @updated 2019-06-18
 */
class HsSynthVoice : public juce::SynthesiserVoice
{
public:
    HsSynthVoice() {}

    void hsSynthInitialise(float sampleRate)
    {   

        
        /// sets the sample rate for the sub oscilators 
        triSubOsc.setSampleRate(sampleRate); 
        subSinOsc.setSampleRate(sampleRate);

        triOsc1.setSampleRate(sampleRate);
        triOsc2.setSampleRate(sampleRate); 

        sineOsc1.setSampleRate(sampleRate); 
        sineOsc2.setSampleRate(sampleRate);
        
        squareOsc1.setSampleRate(sampleRate);
        squareOsc2.setSampleRate(sampleRate);

        sawOsc1.setSampleRate(sampleRate);
        sawOsc2.setSampleRate(sampleRate);

        

        sinelfo1.setSampleRate(sampleRate);

        env.setSampleRate(sampleRate);      // sets sample rate for the ADSR object 

        juce::ADSR::Parameters envParams; 


        // ADSR parameters for the amp envelope 
        envParams.attack = 0.5; 
        envParams.decay = 0.25; 
        envParams.sustain = 0.01; 
        envParams.release = 0.01; 

        env.setParameters(envParams);


        filter.reset(); 

    }

    void setDetune(float detuneIn)
    {
        detuneAmount = detuneIn;
    }

    void setFilterCutoff(float filterCutoffIn)
    {
        filterCutOff = filterCutoffIn;
    }

    void setLfoFreq(float _lfoFreq)
    {
        lfo1Freq = _lfoFreq;
    }

    void setOutputGain(float _outputGain)
    {
        outputGain = _outputGain;
    }

    void setAmpAttack(float _ampAttack)
    {
        ampAttack = _ampAttack; 
    }

    void setAmpDecay(float _ampDecay)
    {
        ampDecay = _ampDecay;
    }

    //--------------------------------------------------------------------------
    /**
     What should be done when a note starts

     @param midiNoteNumber
     @param velocity
     @param SynthesiserSound unused variable
     @param / unused variable
     */
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        playing = true;
        ending = false;


       
        freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);



        triSubOsc.setFrequency(freq);
        subSinOsc.setFrequency(freq);

        triOsc1.setFrequency(freq);
        triOsc2.setFrequency(freq);

        squareOsc1.setFrequency(freq);
        squareOsc2.setFrequency(freq);

        sawOsc1.setFrequency(freq); 
        sawOsc2.setFrequency(freq); 

        

        



        env.reset();    // restes the envelope 
        env.noteOn();   // turns on the ADSR 

    }
    //--------------------------------------------------------------------------
    /// Called when a MIDI noteOff message is received
    /**
     What should be done when a note stops

     @param / unused variable
     @param allowTailOff bool to decie if the should be any volume decay
     */
    void stopNote(float /*velocity*/, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            env.noteOff();
            ending = true;
        }
        else
        {
            clearCurrentNote();
            playing = false;
        }

      
    }

    //--------------------------------------------------------------------------
    /**
     The Main DSP Block: Put your DSP code in here

     If the sound that the voice is playing finishes during the course of this rendered block, it must call clearCurrentNote(), to tell the synthesiser that it has finished

     @param outputBuffer pointer to output
     @param startSample position of first sample in buffer
     @param numSamples number of smaples in output buffer
     */
    void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        if (playing) // check to see if this voice should be playing
        {
            sineOsc1.setFrequency(freq - detuneAmount);
            sineOsc2.setFrequency(freq - detuneAmount);

            // iterate through the necessary number of samples (from startSample up to startSample + numSamples)
            for (int sampleIndex = startSample; sampleIndex < (startSample + numSamples); sampleIndex++)
            {

                float envVal = env.getNextSample();
               
               
              // float filterCutoff = sinelfo1.process();
                
                filter.setCoefficients(juce::IIRCoefficients::makeLowPass(getSampleRate(), filterCutOff));
                // your sample-by-sample DSP code here!
                // An example white noise generater as a placeholder - replace with your own code
                

                sinelfo1.setFrequency(lfo1Freq);

                osc1 = squareOsc1.process(); 
                osc2 = sawOsc2.process();
                noiseOsc = random.nextFloat() * 2 - 1.0;
                subOsc = triSubOsc.process(); 


                float individualSignals = osc1 + osc2 + noiseOsc + subOsc;

                float rawSignal = (individualSignals * sinelfo1.process()) * outputGain * envVal;

                //if ()

                float cutoff = sinelfo1.process();
                
                float currentSample = filter.processSingleSampleRaw(rawSignal);

                // for each channel, write the currentSample float to the output
                for (int chan = 0; chan < outputBuffer.getNumChannels(); chan++)
                {
                    // The output sample is scaled by 0.2 so that it is not too loud by default
                    outputBuffer.addSample(chan, sampleIndex, currentSample * 0.2);
                }
                if (ending)
                {
                    if (envVal < 0.0001f)
                    {
                        clearCurrentNote();
                        playing = false;
                    }
                }
                
            }
        }
    }
    //--------------------------------------------------------------------------
    void pitchWheelMoved(int) override {}
    //--------------------------------------------------------------------------
    void controllerMoved(int, int) override {}
    //--------------------------------------------------------------------------
    /**
     Can this voice play a sound. I wouldn't worry about this for the time being

     @param sound a juce::SynthesiserSound* base class pointer
     @return sound cast as a pointer to an instance of YourSynthSound
     */
    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<YourSynthSound*> (sound) != nullptr;
    }
    //--------------------------------------------------------------------------
private:
    //--------------------------------------------------------------------------
    // Set up any necessary variables here
    /// Should the voice be playing?
    bool playing = false;
    bool ending = false;

    float detuneAmount; 

    float outputGain;

    float ampAttack; 
    float ampDecay; 
    float ampSustain;  
    float ampRelease;

    float osc1; 
    float osc2; 
    float noiseOsc; 
    float subOsc; 


        
    float freq; 
    float lfo1Freq;

    TriOsc triSubOsc, triOsc1, triOsc2; 
    SinOsc subSinOsc, sineOsc1, sineOsc2, sinelfo1; 

    SquareOsc squareOsc1, squareOsc2; 

    Sawtooth sawOsc1, sawOsc2; 

    /// a random object for use in our test noise function
    juce::Random random;

    juce::ADSR env; 

    juce::IIRFilter filter;
    float filterCutOff;
   
    
    // drop down choice paramters 
    std::atomic<float>* osc1ChoiceParam;        // choice beteen different waveforms for oscilator 1
    std::atomic<float>* osc2ChoiceParam;        // choice beteen different waveforms for oscilator 2
    std::atomic<float>* subOscChoiceParam;      // choice beteen sine and triangle waveform
    std::atomic<float>* noiseOscChoiceParam;    // choice between different types of noise 
    std::atomic<float>* filterChoiceParam;      // choice between hp, lp, and bp filters


   
};