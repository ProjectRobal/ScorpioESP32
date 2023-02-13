#include <Arduino.h>

#include "Sampler.hpp"

#define CH0 0 // ADC1 CH0
#define CH1 3 // ADC1 CH3
#define CH2 6 // ADC1 CH6

#define DATA_SIZE 4096

#define BUFFER_SIZE 1024

ADCOutput outputs[3]={
  ADCOutput(CH0,DATA_SIZE),
  ADCOutput(CH1,DATA_SIZE),
  ADCOutput(CH2,DATA_SIZE)
};

Sampler sampler;

void setup()
{
Serial.begin(115200);

dacWrite(25,127);
dacWrite(26,50);

  for(const ADCOutput& out : outputs)
  {

  sampler.setADCChannel(out);

  }

  sampler.setSampleFreq(60*1000);

  sampler.begin(BUFFER_SIZE,BUFFER_SIZE*10);

  sampler.start();
}

void loop()
{
  sampler.parse_data(outputs,3);

  for(uint16_t c=0;c<3;c++)
  {
    if(outputs[c])
    {
  Serial.print("c");
  Serial.print(c);
  Serial.println();
  for(uint16_t i=0;i<DATA_SIZE;++i)
  {
  Serial.println(outputs[c].Data()[i]);
  }
  Serial.println();
    }
  }

}