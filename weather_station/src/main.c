#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "freertos/task.h"
#include "esp_err.h"
#include <math.h>

#define ADC_CHANNEL ADC1_CHANNEL_6 //GPIO34

#define ADC_ATTEN ADC_ATTEN_DB_11 //0-3,3V
#define ADC_WIDTH   ADC_WIDTH_BIT_12 // 12-bit resolution

void get_temp(void *pvParameters){
    int reading = adc1_get_raw(ADC_CHANNEL);
    float temp = ((reading*3.3)/4096)*100;

    vTaskDelay(pdMS_TO_TICKS(4000));
}

void app_main() {
    // Configure ADC
    adc1_config_width(ADC_WIDTH);
    adc1_config_channel_atten(ADC_CHANNEL,ADC_ATTEN); 

    while(1){
        vTaskDelay(pdMS_TO_TICKS(5000));

        xTaskCreate(get_temp,"get_temp",2048,NULL,5,NULL);
    }
}