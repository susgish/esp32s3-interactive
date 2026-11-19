#include "ssd1306.h"
#include "i2c_bus.h"

#include "driver/i2c_master.h"
#include "esp_log.h"
#include <string.h>

#define TAG "ssd1306"
#define SSD1306_I2C_ADDR 0x3C   /* 7-bit 地址，常见为 0x3C（部分模块 0x3D） */

static i2c_master_dev_handle_t s_dev = NULL;

/* 发送单条命令：首字节控制位 0x00 表示命令流 */
static esp_err_t ssd1306_write_cmd(uint8_t cmd)
{
    uint8_t buf[2] = {0x00, cmd};
    return i2c_bus_write(s_dev, buf, sizeof(buf));
}

/* 发送数据块：首字节控制位 0x40 表示数据流。
 * 注意：s_tx 为静态缓冲，当前仅由 LVGL flush 单线程调用，无并发问题。 */
static uint8_t s_tx[SSD1306_WIDTH * SSD1306_PAGES + 1];

static esp_err_t ssd1306_write_data(const uint8_t *data, size_t len)
{
    if (len > sizeof(s_tx) - 1) {
        return ESP_ERR_INVALID_SIZE;
    }
    s_tx[0] = 0x40;
    memcpy(&s_tx[1], data, len);
    return i2c_bus_write(s_dev, s_tx, len + 1);
}

esp_err_t ssd1306_init(void)
{
    esp_err_t err = i2c_bus_add_device(SSD1306_I2C_ADDR, 400000, &s_dev);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "add device failed: %s", esp_err_to_name(err));
        return err;
    }

    /* SSD1306 128x64 标准初始化序列 */
    static const uint8_t init_seq[] = {
        0xAE,        /* display off */
        0xD5, 0x80,  /* 时钟分频 */
        0xA8, 0x3F,  /* multiplex = 63（64 行） */
        0xD3, 0x00,  /* 显示偏移 */
        0x40,        /* 起始行 */
        0x8D, 0x14,  /* 电荷泵使能 */
        0x20, 0x00,  /* 内存寻址模式：水平 */
        0xA1,        /* 段重映射 */
        0xC8,        /* COM 扫描方向 */
        0xDA, 0x12,  /* COM 引脚配置 */
        0x81, 0xCF,  /* 对比度 */
        0xD9, 0xF1,  /* 预充电周期 */
        0xDB, 0x40,  /* VCOMH */
        0xA4,        /* 显示 RAM 内容 */
        0xA6,        /* 正常显示（非反显） */
        0xAF,        /* display on */
    };

    for (size_t i = 0; i < sizeof(init_seq); i++) {
        err = ssd1306_write_cmd(init_seq[i]);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "init cmd[%d] failed: %s", i, esp_err_to_name(err));
            return err;
        }
    }

    ESP_LOGI(TAG, "SSD1306 init OK (128x64 @0x%02X)", SSD1306_I2C_ADDR);
    return ESP_OK;
}

esp_err_t ssd1306_write_full_screen(const uint8_t *buf)
{
    /* 设置寻址窗口：列 0~127，页 0~7 */
    static const uint8_t window_cmds[] = {
        0x20, 0x00,       /* 水平寻址模式 */
        0x21, 0x00, 0x7F, /* 列 0~127 */
        0x22, 0x00, 0x07, /* 页 0~7 */
    };

    for (size_t i = 0; i < sizeof(window_cmds); i++) {
        esp_err_t err = ssd1306_write_cmd(window_cmds[i]);
        if (err != ESP_OK) {
            return err;
        }
    }
    return ssd1306_write_data(buf, SSD1306_WIDTH * SSD1306_PAGES);
}
