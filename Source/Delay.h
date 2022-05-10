/**
  ==============================================================================
  * A basic delay that takes dry signal and outputs delayed signal built following the lecture  
  * 
  * 

  ==============================================================================
*/

#pragma once 
#include <cmath>

class Delay
{
public: 

    /// input a sample and return the delayed signal 
    float process(float inputSample)
    {
        float output = readVal();
        writeVal(inputSample + (output * feedback)); 
        return output; 
    }

    /// get read value and increment read position
    float readVal()
    {
        float outVal = linearInterpolation();
        readPos++; 
        if (readPos >= size)
        {
            readPos = 0; 
        }
        return outVal;
    }

    float linearInterpolation()
    {
        int indexA = floor(readPos);
        int indexB = indexA + 1; 
        
        // Wrap 
        if (indexB >= size)
            indexB -= size;

        float valA = buffer[indexA]; 
        float valB = buffer[indexB];
        float remainder = readPos - indexA;
        float interpolatedValue = (1 - remainder) * valA + remainder * valB;

        return interpolatedValue;
    }

    /// store current value at write position 
    float writeVal(float inputSample)
    {
        buffer[WritePos] = inputSample; 
        WritePos++;

        if (WritePos >= size)
            WritePos = 0;
        
        return buffer[WritePos];
    }

    /// Delay size in samples 
    void setSize(int sizeInSamples)
    {
        size = sizeInSamples;
        buffer = new float[size];
       
            for (int i = 0; i<size; i++)
            {
                buffer[i] = 0.0;
            }
    }

    /// delay time in samples 
    void setDelayTime(int _delayTimeInSamples)
    {
        delayTimeInSamples = _delayTimeInSamples;

        if (delayTimeInSamples > size)
            delayTimeInSamples = size; 

        if (delayTimeInSamples < 1)
            delayTimeInSamples = 1;

        readPos = WritePos - delayTimeInSamples;

        if (readPos < 0)
            readPos += size; 
    }

    /// feedback for the delay
    void setFeedback(float _fb)
    {
        feedback - _fb; 

        if (feedback > 1.0)
            feedback = 1.0; 

        if (feedback < 0.0)
            feedback = 0.00; 
    }
private: 

    float* buffer;          // store samples
    int size;               // size of buffer in samples
    int delayTimeInSamples; // delay time in samples
    float readPos = 0;      // read position
    int WritePos = 0;       // write position
    float feedback;   // range in 0-1 
};
