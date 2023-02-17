/** @brief A class that is child of ADCOutput class, it perform the same task plus fft transform. 
 * 
*/
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

    float filter(float mag)
    {
        if(isnan(mag))
        {
            return 0;
        }

        if(mag<2500.0)
        {
            return mag;
        }

        return 0;
    }


    /**
     * @brief FFT loop function
     * 
     * A function that make fft when data are ready.
     * 
     * @return true - when data is ready, otherwise return false
    */

    bool FFT()
    {
        if(this->operator bool())
        {
            uint32_t i;

            const uint16_t * raw=this->Data();

            for(i=0;i<this->Size();++i)
            {
                this->fft->input[i]=static_cast<float>(raw[i]);
            }

            fft_execute(this->fft);


            for(i=1;i<this->Size()/2;++i)
            {

                float mag=sqrt(pow(fft->output[2*i],2) + pow(fft->output[2*i+1],2))/(this->Size()/2);

                this->fft->output[i]=mag;//filter(mag);
                
            }

            this->flush();

            return true;
        }

        return false;
    }

    /** @brief Get fft output
     * 
     *  @return float array with fft's output
    */
    const float * GetFFT()
    {
        return this->fft->output;
    }

    /** @brief Interept index as frequency
     * 
     * @param i - index
     * @param sample_freq - a signal sample freq
     * 
     *  @return frequency corrensponding to input index
    */
    float IndexToFrequency(const uint32_t& i,const float& sample_freq)
    {
        return (static_cast<float>(i)*sample_freq)/this->Size();
    }

    ~FFTOutput()
    {
        fft_destroy(this->fft);      
    }
};