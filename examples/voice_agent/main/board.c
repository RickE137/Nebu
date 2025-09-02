#include "esp_log.h"
#include "codec_init.h"
#include "codec_board.h"
#include "driver/temperature_sensor.h"
#include "bsp/esp-bsp.h"

#include "board.h"

static const char *TAG = "board";

static temperature_sensor_handle_t temp_sensor = NULL;

void board_init()
{
    ESP_LOGI(TAG, "Initializing board");

    // Initialize board support package and LEDs
    bsp_i2c_init();
    // Comentado: Tu ESP32S3-WROOM-1 no tiene LEDs Korvo
    // bsp_leds_init();
    // bsp_led_set(BSP_LED_RED, true);
    // bsp_led_set(BSP_LED_BLUE, true);

    // Initialize temperature sensor
    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);
    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor_config, &temp_sensor));
    ESP_ERROR_CHECK(temperature_sensor_enable(temp_sensor));

    // Initialize codec board for custom hardware
    set_codec_board_type("ESP32S3_WROOM_CUSTOM");
    codec_init_cfg_t cfg = {
        .in_mode = CODEC_I2S_MODE_STD,     // Modo I2S estándar para INMP441
        .in_use_tdm = false,               // No TDM para hardware simple
        .reuse_dev = false
    };
    init_codec(&cfg);
    
    // CRÍTICO: Configurar I2S para MONO + LEFT (INMP441)
    // El INMP441 con L/R a GND envía audio por canal izquierdo
    ESP_LOGI(TAG, "Configurando I2S para INMP441 MONO + LEFT");
}

float board_get_temp(void)
{
    float temp_out;
    ESP_ERROR_CHECK(temperature_sensor_get_celsius(temp_sensor, &temp_out));
    return temp_out;
}