//#define DEBUG

/** @brief localization algorithm
 *  
 * 
 *  1.Read data from all channels and put them into buffers
 *  2.When buffer is full perform FFT on it
 *  3.From each channel's buffer get index with biggest value
 *  4.Take index that corrensponds with the biggest value
 *  5.Take magnitudes from all buffers at index position from step above
 * 
*/

#include <Arduino.h>

#include "Sampler.hpp"

#include "FFTOutput.hpp"

#include "Signal.hpp"

#include "Wrapper.hpp"

#define SAMPLE_FREQ 20*1000

#define CH0 0 // ADC1 CH0
#define CH1 3 // ADC1 CH3
#define CH2 6 // ADC1 CH6

#define DATA_SIZE 2048

#define BUFFER_SIZE 256


FFTOutput outputs[3]={
  FFTOutput(CH0,DATA_SIZE),
  FFTOutput(CH1,DATA_SIZE),
  FFTOutput(CH2,DATA_SIZE)
};

Signal signals[3]={
  Signal(&(outputs[0])),
  Signal(&(outputs[1])),
  Signal(&(outputs[2]))
};

Sampler sampler;


Wrapper<3> kebab(signals);

void setup()
{
Serial.begin(115200);

dacWrite(25,127);
dacWrite(26,50);

  for(const ADCOutput& out : outputs)
  {

  sampler.setADCChannel(out);

  }

  sampler.setSampleFreq(SAMPLE_FREQ*3); // cause we are using three channels

  sampler.begin(BUFFER_SIZE,BUFFER_SIZE*10);

  sampler.start();
}


void loop()
{
  sampler.parse_data(outputs,3);

  for(Signal& sig : signals)
  {
    sig.loop();
  }

  kebab.loop();

  for(uint16_t i=0;i<kebab.size();++i)
  {
    Serial.print(i);
    Serial.print(" : ");
    Serial.println(kebab[i]);
  }

  delay(250);
}