#include <stdio.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/spi_common.h"
#include "driver/sdspi_host.h"
#include "driver/spi_master.h"  // ✅ DODANE

static const char *TAG = "SD_CARD";

void app_main(void)
{
    ESP_LOGI(TAG, " ESP32 uruchomione!");

    esp_err_t ret;

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = SPI3_HOST;

    spi_bus_config_t bus_cfg = {
        .mosi_io_num = 23,
        .miso_io_num = 19,
        .sclk_io_num = 18,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };

    // ✅ Zmienione dla ESP-IDF 5.x
    ret = spi_bus_initialize(host.slot, &bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Błąd inicjalizacji SPI: %s", esp_err_to_name(ret));
        return;
    }

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = 5;
    slot_config.host_id = host.slot;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    sdmmc_card_t *card;

    ESP_LOGI(TAG, "Montowanie karty SD...");

    ret = esp_vfs_fat_sdspi_mount("/sdcard", &host, &slot_config, &mount_config, &card);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Nie udało się zamontować karty SD: %s", esp_err_to_name(ret));
        spi_bus_free(host.slot);
        return;
    }

    ESP_LOGI(TAG, "Karta SD zamontowana pomyślnie!");
    sdmmc_card_print_info(stdout, card);

    FILE *f = fopen("/sdcard/dane.txt", "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Nie udało się otworzyć pliku do zapisu");
    } else {
        fprintf(f, "To jest testowy zapis z ESP32!\n");
        fprintf(f, "Kolejna linia danych.\n");
        fclose(f);
        ESP_LOGI(TAG, "Zapis zakończony!");
    }

    esp_vfs_fat_sdcard_unmount("/sdcard", card);
    ESP_LOGI(TAG, "Karta SD odmontowana.");

    spi_bus_free(host.slot);
}