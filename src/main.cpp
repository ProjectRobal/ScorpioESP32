#include <Arduino.h>

#include <soc/sens_reg.h>
#include <soc/sens_struct.h>

#include "driver/adc.h"
#include "esp_adc_cal.h"

#include <sys/param.h>
#include "sdkconfig.h"
#include "hal/assert.h"
#include "soc/lldesc.h"
#include "soc/soc_caps.h"
#include "hal/adc_ll.h"
#include "hal/adc_types.h"

#define CH0 1 // ADC1 CH0
#define CH1 4 // ADC1 CH3
#define CH2 5 // ADC1 CH6

#define DATA_SIZE 1024

int IRAM_ATTR local1_adc1_read(int channel)
{
    adc_ll_rtc_enable_channel(ADC_NUM_1, channel);
    adc_ll_rtc_start_convert(ADC_NUM_1, channel);
    while (adc_ll_rtc_convert_is_done(ADC_NUM_1) != true);
    return  adc_ll_rtc_get_convert_value(ADC_NUM_1);

}

int IRAM_ATTR local_adc1_read(int channel) {
    uint16_t adc_value;
    SENS.sar_meas_start1.sar1_en_pad = (1 << channel); // only one channel is selected
    while (SENS.sar_slave_addr1.meas_status != 0);
    SENS.sar_meas_start1.meas1_start_sar = 0;
    SENS.sar_meas_start1.meas1_start_sar = 1;
    while (SENS.sar_meas_start1.meas1_done_sar == 0);
    adc_value = SENS.sar_meas_start1.meas1_data_sar;
    return adc_value;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);


  adc1_config_width(ADC_WIDTH_BIT_10);

  adc1_config_channel_atten((adc1_channel_t)CH0,ADC_ATTEN_DB_2_5);

}

volatile uint64_t start=0;

volatile uint64_t end=0;

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Starting sampling: ");
  start=micros();
  Serial.println(adc1_get_raw((adc1_channel_t)CH0));
  end=micros();

  Serial.print(end-start);
  Serial.println(" us");

}