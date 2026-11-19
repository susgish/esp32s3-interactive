#include "i2c_bus.h"
#include "pinmap.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#define TAG "i2c_bus"
#define I2C_TIMEOUT_MS 1000

static i2c_master_bus_handle_t s_bus = NULL;

/* 互斥量：保护共享 I2C 总线。
 * 本工程当前只有 SSD1306 一个 I2C 设备，但预留互斥量，
 * 后续接入其他 I2C 外设时不会出现总线竞争。 */
static SemaphoreHandle_t s_mutex = NULL;

esp_err_t i2c_bus_init(void)
{
    i2c_master_bus_config_t cfg = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = PIN_I2C_SCL,
        .sda_io_num = PIN_I2C_SDA,
    };

    esp_err_t err = i2c_new_master_bus(&cfg, &s_bus);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "i2c_new_master_bus failed: %s", esp_err_to_name(err));
        return err;
    }

    s_mutex = xSemaphoreCreateMutex();
    if (s_mutex == NULL) {
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG, "I2C bus ready (SCL=GPIO%d, SDA=GPIO%d)",
             PIN_I2C_SCL, PIN_I2C_SDA);
    return ESP_OK;
}

esp_err_t i2c_bus_add_device(uint8_t addr, uint32_t speed_hz,
                             i2c_master_dev_handle_t *out_dev)
{
    i2c_device_config_t cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = addr,
        .scl_speed_hz = speed_hz,
    };
    return i2c_master_bus_add_device(s_bus, &cfg, out_dev);
}

esp_err_t i2c_bus_write(i2c_master_dev_handle_t dev,
                        const uint8_t *data, size_t len)
{
    esp_err_t err;

    if (s_mutex == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    if (xSemaphoreTake(s_mutex, pdMS_TO_TICKS(I2C_TIMEOUT_MS)) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    err = i2c_master_transmit(dev, data, len, I2C_TIMEOUT_MS);

    xSemaphoreGive(s_mutex);
    return err;
}
