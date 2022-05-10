/*
  ==============================================================================

  ==============================================================================
*/

#pragma once
#include "hs_oscilators.h"
#include "PluginProcessor.h"
#include "Delay.h"
#include "hs_sampler.h"

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

        sr = sampleRate;

        sinelfo.setSampleRate(sampleRate);

        env.setSampleRate(sampleRate);      // sets sample rate for the ADSR object 
      


        delay.setSize(sampleRate);              // sets the sample rate of the delay object
        // delay time

        filter.reset(); 
        noiseFilter.reset();

        reverb.setSampleRate(sampleRate);

        //============================================= Setting smooth parameters 
        smoothCutOff.reset(sampleRate, 1.0);
        smoothCutOff.setCurrentAndTargetValue(0.0);

    }

    void linkParameters(std::atomic<float>* ptrToOsc1Choice, std::atomic<float>* ptrToOsc2Choice, std::atomic<float>* ptrToSubOscChoice,
        std::atomic<float>* ptrToFilterChoice, std::atomic<float>* ptrToNoiseChoiceParam, std::atomic<float>* ptrToLfoParam)
    {
        osc1ChoiceParam = ptrToOsc1Choice;
        osc2ChoiceParam = ptrToOsc2Choice;
        subOscChoiceParam = ptrToSubOscChoice;
        filterChoiceParam = ptrToFilterChoice;
        noiseOscChoiceParam = ptrToNoiseChoiceParam;
        lfoParam = ptrToLfoParam;
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

    void setFilterQ(float _filterQ)
    {
        filterQ = _filterQ;
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

    void setAmpSustain(float _ampSustain)
    {
        ampSustain = _ampSustain;
    }

    void setAmpRelease(float _ampRelease)
    {
        ampRelease = _ampRelease;
    }

    void setOsc1Vol(float _osc1Vol)
    {
        osc1Vol = _osc1Vol;
    }

    void setOsc2Vol(float _osc2Vol)
    {
        osc2Vol = _osc2Vol;
    }

    void setOscNoiseVol(float _oscNoiseVol)
    {
        oscNoiseVol = _oscNoiseVol;
    }

    void setOscSubVol(float _oscSubVol)
    {
        oscSubVol = _oscSubVol;
    }

    void setDelayTime(float _delayTime)
    {
        delayTime = _delayTime;
    }

    void setDelayFeedback(float _delayFeedback)
    {
        delayFeedback = _delayFeedback;
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
        triOsc2.setFrequency(freq - detuneAmount);

        squareOsc1.setFrequency(freq);
        squareOsc2.setFrequency(freq - detuneAmount);

        sawOsc1.setFrequency(freq); 
        sawOsc2.setFrequency(freq - detuneAmount); 


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
            env.setParameters(envParams);

            smoothCutOff.setTargetValue(filterCutOff);
            
            // iterate through the necessary number of samples (from startSample up to startSample + numSamples)
            for (int sampleIndex = startSample; sampleIndex < (startSample + numSamples); sampleIndex++)
            {

                float cutOffafterSmooth = smoothCutOff.getNextValue();

                // ADSR parameters for the amp envelope 
                envParams.attack = ampAttack;
                envParams.decay = ampDecay;
                envParams.sustain = ampSustain;
                envParams.release = ampDecay;

                delay.setDelayTime(sr * delayTime);
                delay.setFeedback(delayFeedback);

                float envVal = env.getNextSample();
               
               
              // float filterCutoff = sinelfo1.process();
                
               
                // your sample-by-sample DSP code here!
                // An example white noise generater as a placeholder - replace with your own code
                

                sinelfo.setFrequency(lfo1Freq);

                // oscilator 1 
                osc1 = sawOsc1.process() * osc1Vol;  

                if (*osc1ChoiceParam == 1)
                    osc1 = squareOsc1.process() * osc1Vol;

                if (*osc1ChoiceParam == 2)
                    osc1 = triOsc1.process() * osc1Vol;

                if (*osc1ChoiceParam == 3)
                    osc1 = sineOsc1.process() * osc1Vol;

                // oscilator 2 
                osc2 = sawOsc2.process() * osc2Vol;

                if (*osc2ChoiceParam == 1)
                    osc2 = squareOsc1.process() * osc2Vol;

                if (*osc2ChoiceParam == 2)
                    osc2 = triOsc1.process() * osc2Vol;

                if (*osc2ChoiceParam == 3)
                    osc2 = sineOsc1.process() * osc2Vol;

                // sub oscilator 
                subOsc = triSubOsc.process() * oscSubVol;
                if (*subOscChoiceParam == 1)
                    subOsc = subSinOsc.process() * oscSubVol;

                noiseOsc = (random.nextFloat() * 2 - 1.0) * oscNoiseVol;

                noiseFilter.setCoefficients(juce::IIRCoefficients::makeLowPass(getSampleRate(), 2000.0));
                 
                filter.setCoefficients(juce::IIRCoefficients::makeLowPass(getSampleRate(), cutOffafterSmooth, filterQ));
                if (*filterChoiceParam == 1)
                {
                    filter.setCoefficients(juce::IIRCoefficients::makeHighPass(getSampleRate(), cutOffafterSmooth, filterQ));
                }
                if (*filterChoiceParam == 2)
                {
                    filter.setCoefficients(juce::IIRCoefficients::makeNotchFilter(getSampleRate(), cutOffafterSmooth, filterQ));
                }

                if (*noiseOscChoiceParam == 1)
                {
                    float whitenoiseSignal = (random.nextFloat() * 2 - 1.0) * oscNoiseVol;
                    noiseOsc = noiseFilter.processSingleSampleRaw(whitenoiseSignal);
                }



                float individualSignals = osc1 + osc2 + noiseOsc + subOsc;

                float delayedSample = delay.process(individualSignals);

                float lfo = sinelfo.process();

                float rawSignal = ((delayedSample + individualSignals) * envVal * outputGain) * lfo;

                float currentSample = filter.processSingleSampleRaw(rawSignal);

                // for each channel, write the currentSample float to the output
                for (int chan = 0; chan < outputBuffer.getNumChannels(); chan++)
                {
                    // The output sample is scaled by 0.2 so that it is not too loud by default
                    outputBuffer.addSample(chan, sampleIndex, currentSample * 0.2) ;
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

    float delayTime;
    float delayFeedback; 

    float sr;
   

    float osc1; 
    float osc2; 
    float noiseOsc; 
    float subOsc; 

    float osc1Vol; 
    float osc2Vol; 
    float oscSubVol; 
    float oscNoiseVol; 
        
    float freq; 
    float lfo1Freq;

    TriOsc triSubOsc, triOsc1, triOsc2, triLfo; 
    SinOsc subSinOsc, sineOsc1, sineOsc2, sinelfo; 

    SquareOsc squareOsc1, squareOsc2; 

    Sawtooth sawOsc1, sawOsc2; 

    /// a random object for use in our test noise function
    juce::Random random;

    juce::ADSR env; 

    juce::ADSR::Parameters envParams;

    juce::SmoothedValue<float> smoothCutOff;
    juce::SmoothedValue<float> smoothGain;

    juce::IIRFilter filter;
    float filterCutOff;
    float filterQ;
    
    juce::IIRFilter noiseFilter; 

    Delay delay;


    juce::Reverb reverb;

    //juce::AudioProcessorValueTreeState parameters;
    std::atomic<float>* detuneParam;
    std::atomic<float>* filterCutoffParam;
    std::atomic<float>* filterQParam;
    std::atomic<float>* lfoParam;
    std::atomic<float>* mainOutputGainParam;

    // ADSR params 
    std::atomic<float>* ampAttackParam;
    std::atomic<float>* ampDecayParam;
    std::atomic<float>* ampSustainParam; 
    std::atomic<float>* ampReleaseParam;

    // amplitude params for sound generators 
    std::atomic<float>* osc1VolParam;
    std::atomic<float>* osc2VolParam;
    std::atomic<float>* noiseOscVolParam;
    std::atomic<float>* subOscVolParam;

    std::atomic<float>* delayTimeParam;     // param for delay time 
    std::atomic<float>* delayFeedbackParam;     // param for delay feedback

    std::atomic<float>* samplerOutputLevelParam;

    //// drop down choice paramters 
    std::atomic<float>* osc1ChoiceParam;        // choice beteen different waveforms for oscilator 1
    std::atomic<float>* osc2ChoiceParam;        // choice beteen different waveforms for oscilator 2
    std::atomic<float>* subOscChoiceParam;      // choice beteen sine and triangle waveform
    std::atomic<float>* noiseOscChoiceParam;    // choice between different types of noise 
    std::atomic<float>* filterChoiceParam;      // choice between hp, lp, and bp filters

};