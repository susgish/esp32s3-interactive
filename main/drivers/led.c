#include "led.h"
#include "pinmap.h"

#include "driver/gpio.h"
#include "esp_log.h"

#define TAG "led"

esp_err_t led_init(void)
{
    gpio_config_t cfg = {
        .pin_bit_mask = 1ULL << PIN_LED,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    esp_err_t err = gpio_config(&cfg);
    if (err != ESP_OK) {
        return err;
    }
    led_off();
    ESP_LOGI(TAG, "led ready (GPIO%d)", PIN_LED);
    return ESP_OK;
}

void led_on(void)
{
    gpio_set_level(PIN_LED, 1);
}

void led_off(void)
{
    gpio_set_level(PIN_LED, 0);
}
