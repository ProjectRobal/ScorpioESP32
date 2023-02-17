/** @brief A class that wraps all the inputs from Signal objects all together.
 *  
 * It gives outputs that represents amplitude reading from all sensors.
*/

#pragma once

#include "Signal.hpp"


template<uint16_t Size>
class Wrapper
{
    private:

    Signal *signals;
    
    float outputs[Size];

    bool isReady;

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
        isReady=false;
    }

    void loop()
    {
        for(uint16_t i=0;i<Size;++i)
        {
            if(!signals[i])
            {
                return;
            }
            //Serial.print("Signal ready ");
            //Serial.println(i);
        }

        uint32_t index=the_signal_with_biggest_value();

        index=signals[index].GetIndex();

        for(uint16_t i=0;i<Size;++i)
        {

            outputs[i]=signals[i].GetValue(index);

        }

        for(uint16_t i=0;i<Size;++i)
        {
            signals[i].reset();
        }

        isReady=true;

    }

    operator bool()
    {
        return isReady;
    }

    void reset()
    {
        isReady=false;
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