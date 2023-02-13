#include <Arduino.h>

#include "Sampler.hpp"

#include "BluetoothSerial.h"

//#define DEBUG

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

#ifdef DEBUG

BluetoothSerial BSerial;

#endif

Print *debug;

void setup()
{
Serial.begin(115200);

#ifdef DEBUG

BSerial.begin("Scorpio");

debug=&BSerial;

#else

debug=&Serial;

#endif

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

  #ifdef DEBUG

  if(!BSerial.connected())
  {
    return;
  }

  #endif

  for(uint16_t c=0;c<3;c++)
  {
    if(outputs[c])
    {
  
  for(uint16_t i=0;i<DATA_SIZE;++i)
  {
  debug->print(c);
  debug->print(":");
  debug->println(outputs[c].Data()[i]);
  }
    }
  }

}