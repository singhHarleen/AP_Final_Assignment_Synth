/**
  ==============================================================================
  * oscilator class with sawtooth, square, sine and triangle oscilators 
  * sawtooth is the parent class and all other classes inherit from it 
 

  ==============================================================================
*/


#ifndef hs_oscilators_h 
#define hs_oscilators_h 
#include <cmath>

class Sawtooth // Parent Class 
{
public:
    
    float process()
    {
        phase += phaseDelta; 

        if (phase > 1.0f) 
            phase -= 1.0f;  

        return output(phase);          
    }

    virtual float output(float p)
    {
        return p; 
    }

    void setSampleRate(float SR)
    {
        sampleRate = SR;
    }

    void setFrequency(float freq)
    {
        frequency = freq; 
        phaseDelta = frequency / sampleRate; 
    }
   
private:
    float frequency;
    float sampleRate;
    float phase = 0.0f;
    float phaseDelta;
};
//==========================================

/// child classes 
class TriOsc : public Sawtooth
    
{
public:
    float output(float p) override
    {
        return fabsf(p - 0.5f) - 0.5f; 
    }
};

class SinOsc : public Sawtooth
{
    float output(float p) override
    {
        return sin(p * 2.0 * 3.14159); 
    }
};

class SquareOsc :public Sawtooth
{
    float output(float p) override
    {
        float outVal = 0.5;
        if (p > pulseWidth)
            outVal = -0.5;
        return outVal;
    }
    void setPulseWidth(float pw)
    {
        pulseWidth = pw;
    }

private:
    float pulseWidth = 0.5f;
};

#endif /* hs_oscilators_h */