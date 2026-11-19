#include "buzzer.h"
#include "pinmap.h"

#include "driver/gpio.h"
#include "esp_log.h"

#define TAG "buzzer"

esp_err_t buzzer_init(void)
{
    gpio_config_t cfg = {
        .pin_bit_mask = 1ULL << PIN_BUZZER,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    esp_err_t err = gpio_config(&cfg);
    if (err != ESP_OK) {
        return err;
    }
    buzzer_off();
    ESP_LOGI(TAG, "buzzer ready (GPIO%d)", PIN_BUZZER);
    return ESP_OK;
}

void buzzer_on(void)
{
    gpio_set_level(PIN_BUZZER, 1);
}

void buzzer_off(void)
{
    gpio_set_level(PIN_BUZZER, 0);
}
