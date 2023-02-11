#include <Arduino.h>

#include "Sampler.hpp"

#define CH0 6 // ADC1 CH0
#define CH1 4 // ADC1 CH3
#define CH2 5 // ADC1 CH6

#define DATA_SIZE 256

Sampler sampler;

ADCOutput output[3]={
  ADCOutput(CH0,DATA_SIZE),
  ADCOutput(CH1,DATA_SIZE),
  ADCOutput(CH2,DATA_SIZE)
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  sampler.setADCChannel(output[0]);
  sampler.setADCChannel(output[1]);
  sampler.setADCChannel(output[2]);

  sampler.setSampleFreq(44000);

  sampler.begin(256);

  sampler.start(); 
}

void loop() {
  // put your main code here, to run repeatedly:

  sampler.parse_data(output,3);

  
    if(output[2])
    {
      
      const uint16_t *out=output[2].Data();
      Serial.print("Channel ");
      Serial.print(2);
      
      for(uint16_t o=0;o<DATA_SIZE;++o)
      {
               
        Serial.print(" ");
        Serial.print(out[o]);
        
      }
    
  }

  delay(250);

}