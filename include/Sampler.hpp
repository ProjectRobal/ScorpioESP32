#pragma once
/**
 * @brief A class for continoues adc read from DMA

*/
#include <Arduino.h>

#include "driver/adc.h"
#include "esp_adc_cal.h"

#if CONFIG_IDF_TARGET_ESP32
#define ADC_RESULT_BYTE     2
#define ADC_CONV_LIMIT_EN   1                       //For ESP32, this should always be set to 1
#define ADC_CONV_MODE       ADC_CONV_SINGLE_UNIT_1  //ESP32 only supports ADC1 DMA mode
#define ADC_OUTPUT_TYPE     ADC_DIGI_OUTPUT_FORMAT_TYPE1
#elif CONFIG_IDF_TARGET_ESP32S2
#define ADC_RESULT_BYTE     2
#define ADC_CONV_LIMIT_EN   0
#define ADC_CONV_MODE       ADC_CONV_BOTH_UNIT
#define ADC_OUTPUT_TYPE     ADC_DIGI_OUTPUT_FORMAT_TYPE2
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
#define ADC_RESULT_BYTE     4
#define ADC_CONV_LIMIT_EN   0
#define ADC_CONV_MODE       ADC_CONV_ALTER_UNIT     //ESP32C3 only supports alter mode
#define ADC_OUTPUT_TYPE     ADC_DIGI_OUTPUT_FORMAT_TYPE2
#elif CONFIG_IDF_TARGET_ESP32S3
#define ADC_RESULT_BYTE     4
#define ADC_CONV_LIMIT_EN   0
#define ADC_CONV_MODE       ADC_CONV_BOTH_UNIT
#define ADC_OUTPUT_TYPE     ADC_DIGI_OUTPUT_FORMAT_TYPE2
#endif

#define ADC_BUFFER_DEF 1024
#define TIMEOUT 10 // in ms

class Sampler;

enum ADC_DEVICE
{
    ADC1=0,
    ADC2=1
};

/** @brief A class that represents each adc channel 
 * 
 * 
*/
class ADCOutput
{
    private:

    uint8_t channel;

    uint16_t *data;

    uint32_t ptr;

    uint32_t _size;

    ADC_DEVICE unit;

    public:

    ADCOutput(uint8_t _channel,uint32_t size,ADC_DEVICE _unit=ADC_DEVICE::ADC1)
    {
        channel=_channel;
        _size=size;
        unit=_unit;
        ptr=0;

        #ifdef BOARD_HAS_PSRAM

        data=(uint16_t*)ps_malloc(size*sizeof(uint16_t));

        #else

        data=new uint16_t[size];

        #endif
    }

    const uint32_t& Size()
    {
        return _size;
    }

    void AppendData(const uint16_t& sample,const uint8_t& _channel,const uint8_t& _unit)
    {
        if((_channel!=channel)||(unit!=_unit))
        {
            return;
        }

        if(ptr<_size)
        {
            data[ptr]=sample;
            ++ptr;
        }
    }

    const uint16_t* Data()
    {
        ptr=0;
        return data;
    }

    operator bool()
    {
        return ptr>=_size;
    }

    virtual ~ADCOutput()
    {
        delete [] data;
    }

    friend class Sampler;
};


class Sampler
{
    private:

    adc_digi_convert_mode_t adc_mode;

    adc_digi_init_config_s config;

    uint32_t sample_freq;

    uint8_t *buffer_out;

    uint32_t data_ptr;

    ADCOutput *output;
    //number of adcs
    uint8_t n_adc;


    uint16_t adc1_channels; // adc1 mask
    uint16_t adc2_channels; // adc2 mask
    uint8_t channels_number; // a adc channel number
    uint32_t buffer_size;

    void _init_adc_dma(const uint32_t& buffer_size,const uint32_t& max_buffer_size);

    void _init_adc_channel(const ADCOutput& adc);

    void _init_adc_controller_cfg();

    void _init_buffer(const uint32_t& buffer_size);

    uint16_t count_channels(uint16_t mask);

    bool check_valid_data(const adc_digi_output_data_t *data);

    public:

    Sampler();
    
    void begin(const uint32_t& buffer_size,const uint32_t& max_buffer_size=ADC_BUFFER_DEF);

    void parse_data(ADCOutput* _output,const uint32_t& _adcs);

    void start();

    void stop();

    void end();

    void clearADCChannel(ADC_DEVICE dev);

    void setADCChannel(ADC_DEVICE dev,uint16_t channel);

    void setADCChannel(const ADCOutput& out);

    const uint32_t& BufferSize() const
    {
        return buffer_size;
    }

    const uint8_t* Buffer() const
    {
        uint32_t readed=0;
        adc_digi_read_bytes(buffer_out,buffer_size,&readed,TIMEOUT);

        return buffer_out;
    }

    void setSampleFreq(const uint32_t& samp)
    {
        sample_freq=samp;
    }

    const uint32_t& SampleFrequency() const
    {
        return sample_freq;
    } 

    void setADCMode(const adc_digi_convert_mode_t& mode)
    {
        adc_mode=mode;
    }

    const adc_digi_convert_mode_t& ADCMode()
    {
        return adc_mode;
    }

    ~Sampler();

};