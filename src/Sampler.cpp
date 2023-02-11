#include "Sampler.hpp"

bool Sampler::check_valid_data(const adc_digi_output_data_t *data)
    {
    const unsigned int unit = data->type2.unit;
    if (unit > 2) return false;
    if (data->type2.channel >= SOC_ADC_CHANNEL_NUM(unit)) return false;

    return true;
    }

uint16_t Sampler::count_channels(uint16_t mask)
{
    uint16_t output=0;

    while(mask)
    {
        if(mask & 1)
        {
            ++output;
        }

        mask=mask>>1;
    }

    return output;
}

void Sampler::_init_adc_dma(const uint32_t& buffer_size,const uint32_t& max_buffer_size)
    {
        adc_digi_init_config_s config;
        config.max_store_buf_size=max_buffer_size;
        config.conv_num_each_intr=buffer_size;
        config.adc1_chan_mask=adc1_channels;
        config.adc2_chan_mask=adc2_channels;

        adc_digi_initialize(&config);
        _init_buffer(buffer_size);
    }

void Sampler::_init_adc_controller_cfg()
{
    adc_digi_configuration_t config;

    config.conv_limit_num=ADC_CONV_LIMIT_EN;
    config.format=ADC_OUTPUT_TYPE;
    config.sample_freq_hz=sample_freq;
    config.conv_limit_num=250;

    uint16_t channels_number=0;

    #if ADC_CONV_MODE==ADC_CONV_SINGLE_UNIT_1

    config.conv_mode=ADC_CONV_SINGLE_UNIT_1;

    channels_number+=count_channels(adc1_channels);

    #elif ADC_CONV_MODE==ADC_CONV_SINGLE_UNIT_2

    config.conv_mode=ADC_CONV_SINGLE_UNIT_2;

    channels_number+=count_channels(adc2_channels);

    #else

    if((adc1_channels)&&(adc2_channels))
    {
        #if ADC_CONV_MODE==ADC_CONV_ALTER_UNIT

        config.conv_mode=ADC_CONV_ALTER_UNIT;

        #else

        config.conv_mode=ADC_CONV_BOTH_UNIT;

        #endif

        channels_number=count_channels(adc1_channels)+count_channels(adc2_channels);

    }
    else if(adc1_channels)
    {
        config.conv_mode=ADC_CONV_SINGLE_UNIT_1;
        channels_number+=count_channels(adc1_channels);
    }
    else if(adc2_channels)
    {
        config.conv_mode=ADC_CONV_SINGLE_UNIT_2;
        channels_number+=count_channels(adc2_channels);
    }

    #endif

    config.pattern_num=channels_number;

    adc_digi_pattern_config_t patterns[channels_number];

    uint8_t i=0;

    if(adc1_channels)
    {

    for(uint8_t channel=0;channel<8;++channel)
    {   
        if(adc1_channels & (1<<channel))
        {   
            patterns[i].unit=0;

            patterns[i].atten=ADC_ATTEN_DB_11;

            patterns[i].channel=channel;

            patterns[i].bit_width=SOC_ADC_DIGI_MAX_BITWIDTH;

            ++i;
        }
    }

    }

    i=0;

    if(adc2_channels)
    {

    for(uint8_t channel=0;channel<10;++channel)
    {   
        if(adc2_channels & (1<<channel))
        {   
            patterns[i].unit=1;

            patterns[i].atten=ADC_ATTEN_DB_11;

            patterns[i].channel=channel;

            patterns[i].bit_width=SOC_ADC_DIGI_MAX_BITWIDTH;

            ++i;
        }
    }

    }

    config.adc_pattern=patterns;

    //Here add adc channels inintializations

    adc_digi_controller_configure(&config);
}

void Sampler::clearADCChannel(ADC_DEVICE dev)
    {
        switch(dev)
        {
            case ADC_DEVICE::ADC1:

            adc1_channels=0;

            break;

            case ADC_DEVICE::ADC2:

            adc2_channels=0;

            break;
        }
    }

void Sampler::setADCChannel(ADC_DEVICE dev,uint16_t channel)
    {
        switch(dev)
        {
            case ADC_DEVICE::ADC1:

            adc1_channels|=(1<<channel);

            break;

            case ADC_DEVICE::ADC2:

            adc2_channels|=(1<<channel);

            break;
        }
    }

void Sampler::setADCChannel(ADCOutput out)
{
    setADCChannel(static_cast<ADC_DEVICE>(out.unit),out.channel);
}

void Sampler::_init_buffer(const uint32_t& buffer_size)
    {
        #ifdef BOARD_HAS_PSRAM

        buffer_out=ps_malloc(buffer_size);

        #else

        buffer_out=new uint8_t[buffer_size];

        #endif

    }

Sampler::Sampler()
    {
        adc1_channels=0;
        adc2_channels=0;
        data_ptr=0;
        buffer_out=NULL;
        adc_mode=ADC_CONV_SINGLE_UNIT_1;
    }


void Sampler::parse_data(ADCOutput* _output,const uint32_t& _adcs)
    {
    uint32_t readed=0;
    adc_digi_read_bytes(buffer_out,buffer_size,&readed,TIMEOUT);
    uint8_t a;

    for(uint32_t i=0;i<readed;i+=ADC_RESULT_BYTE)
    {
        adc_digi_output_data_t *p = (adc_digi_output_data_t*)&buffer_out[i];

        for(a=0;a<_adcs;++a)
        {
            #if ADC_CONV_MODE==ADC_DIGI_OUTPUT_FORMAT_TYPE1
            _output[a].AppendData(p->type1.data,p->type1.channel,0);
            #else
            _output[a].AppendData(p->type2.data,p->type2.channel,p->type2.unit);
            #endif
        }   
    }
    }

void Sampler::begin(const uint32_t& buffer_size,const uint32_t& max_buffer_size)
    {
        _init_adc_dma(buffer_size,max_buffer_size);

        this->buffer_size=buffer_size;

        _init_adc_controller_cfg();
    }

void Sampler::start()
    {
        adc_digi_start();
    }

void Sampler::stop()
    {
        adc_digi_stop();
    }

void Sampler::end()
    {
        stop();
        adc_digi_deinitialize();
        delete [] buffer_out;
    }

Sampler::~Sampler()
    {
        end();
    }