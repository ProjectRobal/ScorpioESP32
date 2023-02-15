//#define DEBUG

#include <Arduino.h>

#include "Sampler.hpp"

#include "FFTOutput.hpp"



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

  if(outputs[0].FFT())
  {
    Serial.print("");
    Serial.println(outputs[0].GetFFT()[10]);
  } 

  delay(250);
}