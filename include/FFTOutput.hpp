#pragma once

extern "C"
{
#include "fft.h"
}

#include "Arduino.h"

#include "Sampler.hpp"

class FFTOutput : public ADCOutput
{
    private:
    fft_config_t *fft;

    public:
    FFTOutput(uint8_t _channel,uint32_t size,ADC_DEVICE _unit=ADC_DEVICE::ADC1)
    :ADCOutput(_channel,size,_unit)
    {
        fft=fft_init(this->Size(),FFT_REAL,FFT_FORWARD,NULL,NULL);        
    }

    const float* FFT()
    {
        if(this->operator bool())
        {
            const uint16_t * raw=this->Data();

            for(uint32_t i=0;i<this->Size();++i)
            {
                this->fft->input[i]=static_cast<float>(raw[i]);
            }

            fft_execute(this->fft);

            return this->fft->output;
        }

    }

    ~FFTOutput()
    {
        fft_destroy(this->fft);      
    }
};