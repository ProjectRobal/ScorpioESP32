#pragma once

#include "FFTOutput.hpp"

class Signal
{
    private:
    FFTOutput* fft;
    uint32_t index;
    bool isReady;

    /** @brief a function that return an index of the biggets number in a input array 
    * the index 0 holds dc component so we start with 1 
    *  @param input - an input array
    *  @param size - a size of the input array
    * 
    * @return an index with the biggest value
    */
    uint32_t max(const float* input,const uint32_t& size)
    {
    uint32_t output=1;

    for(uint32_t i=2;i<size;++i)
    {
        if(input[i]>input[output])
        {
            output=i;
        }
    }

    return output;
    }

    public:
    Signal(FFTOutput *_fft)
    : fft(_fft)
    {

    }

    void loop()
    {
        if(isReady)
        {
            return;
        }

        if(fft->FFT())
        {
            index=max(fft->GetFFT(),fft->Size()/2);

            isReady=true;
        }
    }

    FFTOutput* FFT()
    {
        return fft;
    }

    const float& GetValue(const uint32_t& index)
    {
        return fft->GetFFT()[index];
    }

    const float& GetValue()
    {
        return fft->GetFFT()[index];
    }

    operator bool()
    {
        return isReady;
    }

    const uint32_t& GetIndex()
    {
        return index;
    }

};