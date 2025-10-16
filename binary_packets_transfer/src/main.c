#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include <stdint.h>
#include <string.h>

#define UART_NUM UART_NUM_0
#define BUF_SIZE 1024

typedef struct {
    uint8_t header;
    uint8_t temperature;
    uint8_t humidity;
    uint32_t time;
} __attribute__((packed)) SensorData;

void uart_init(void) {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    
    uart_param_config(UART_NUM, &uart_config);
    uart_driver_install(UART_NUM, BUF_SIZE, BUF_SIZE, 0, NULL, 0);
}

void send_data(uint8_t temp) {
    SensorData data;
    data.header = 0xAA;
    data.temperature = temp;  // konwersja z 20.0°C na 20
    data.humidity = 10;
    data.time = (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
    
    
    // Konwersja time na big-endian
    uint32_t time_be = __builtin_bswap32(data.time);
    data.time = time_be;
    
    // Wyślij surowe bajty przez UART
    uart_write_bytes(UART_NUM, (const char*)&data, sizeof(SensorData));
}

void get_temp(void *pvParameters) {
    while(1) {
        uint8_t temp = 20 + rand()%12; // 20.0-31.0°C
        send_data(temp);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main() {
    uart_init();
    xTaskCreate(get_temp, "get_temp", 2048, NULL, 5, NULL);
}