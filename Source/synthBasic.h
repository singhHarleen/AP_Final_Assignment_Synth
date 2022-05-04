

#pragma once
#include "JuceHeader.h"
#include "hs_oscilators.h"


// ===========================
// ===========================
// SOUND
class MySynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote      (int) override      { return true; }
    //--------------------------------------------------------------------------
    bool appliesToChannel   (int) override      { return true; }
};




// =================================
// =================================
// Synthesiser Voice - your synth code goes in here

/*!
 @class YourSynthVoice
 @abstract struct defining the DSP associated with a specific voice.
 @discussion multiple YourSynthVoice objects will be created by the Synthesiser so that it can be played polyphicially
 
 @namespace none
 @updated 2019-06-18
 */
class MySynthVoice : public juce::SynthesiserVoice
{
public:
    MySynthVoice() {}
    //--------------------------------------------------------------------------
    /**
     What should be done when a note starts

     @param midiNoteNumber
     @param velocity
     @param SynthesiserSound unused variable
     @param / unused variable
     */


    // initlaises the sample rate for the oscilators 
    void sampRateInitialise(float sampleRate)
    {
        sineOsc.setSampleRate(sampleRate);
        sawOsc.setSampleRate(sampleRate);
        squareOsc.setSampleRate(sampleRate);
        triOsc.setSampleRate(sampleRate);
        detunedSineOsc.setSampleRate(sampleRate);

        synthAmpEnv.setSampleRate(sampleRate); 

        // assigns values to the attack and decay parameters 
        juce::ADSR::Parameters synthEnvParams; 
        synthEnvParams.attack = 1.20f;   
        synthEnvParams.decay = 0.5f;
        synthEnvParams.sustain = 0.0f;
        synthEnvParams.release = 0.01f; 
        
        ending = false;

        synthAmpEnv.setParameters(synthEnvParams);
        
    }

    void setParamPointer(std::atomic<float>* detuneIn)
    {
        detuneAmount = detuneIn;
    }

    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        playing = true;
        oscFreq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        sineOsc.setFrequency(oscFreq);
        
        
        synthAmpEnv.noteOn();
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
        synthAmpEnv.noteOff();
        clearCurrentNote();
        playing = false;
        ending = true;
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
            detunedSineOsc.setFrequency(oscFreq - *detuneAmount);
            // iterate through the necessary number of samples (from startSample up to startSample + numSamples)
            for (int sampleIndex = startSample;   sampleIndex < (startSample+numSamples);   sampleIndex++)
            {
              
                // your sample-by-sample DSP code here!
                // An example white noise generater as a placeholder - replace with your own code



                float envVal = synthAmpEnv.getNextSample(); 

                float outputVol = 0.5;

                float currentSample = ((sineOsc.process() + detunedSineOsc.process()) * outputVol) * envVal;
                
                // for each channel, write the currentSample float to the output
                for (int chan = 0; chan<outputBuffer.getNumChannels(); chan++)
                {
                    // The output sample is scaled by 0.2 so that it is not too loud by default
                    outputBuffer.addSample (chan, sampleIndex, currentSample * 0.25);
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
    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<MySynthSound*> (sound) != nullptr;
    }
    //--------------------------------------------------------------------------
private:
    //--------------------------------------------------------------------------
    // Set up any necessary variables here
    /// Should the voice be playing?
    bool playing = false;
    bool ending = false; 

    /// a random object for use in our test noise function
    juce::Random random;

    SinOsc sineOsc, detunedSineOsc;     // sine oscilator 
    TriOsc triOsc;      // triangle oscilator 
    SquareOsc squareOsc;// square oscilator  
    Sawtooth sawOsc;    // saw oscilator  

    float oscFreq;      // sets the frequency of the oscilators


    std::atomic<float>* detuneAmount; // sets the amount of detune for the synth oscilators 
    juce::ADSR synthAmpEnv;     // instance of the juce ADSR class
};