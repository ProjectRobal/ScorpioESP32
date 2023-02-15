#pragma once

#include "Signal.hpp"


template<uint16_t Size>
class Wrapper
{
    private:

    Signal *signals;
    
    float outputs[Size];

    uint32_t the_signal_with_biggest_value()
    {
        uint32_t index=0;

        for(uint16_t i=1;i<Size;++i)
        {
           if(signals[i].GetValue()>signals[index].GetValue())
           {
                index=i;
           }
        }

        return index;
    }

    public:

    Wrapper(Signal *_signals)
    {
        signals=_signals;
    }

    void loop()
    {
        for(uint16_t i=0;i<Size;++i)
        {
            if(!signals[i])
            {
                return;
            }
        }

        uint32_t index=the_signal_with_biggest_value();

        for(uint16_t i=0;i<Size;++i)
        {

            outputs[i]=signals[i].GetValue(index);

        }
    }

    const float& GetOutput(uint16_t index)
    {
        return outputs[index];
    }

    const float& operator[](uint16_t index)
    {
        return GetOutput(index);
    }

    uint16_t size()
    {
        return Size;
    }
};