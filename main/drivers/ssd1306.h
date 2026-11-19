#pragma once

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"

/**
 * @file ssd1306.h
 * @brief SSD1306 128x64 单色 OLED 驱动（I2C）。
 */

#define SSD1306_WIDTH   128
#define SSD1306_HEIGHT  64
#define SSD1306_PAGES   8   /* 64 行 / 每页 8 行 */

esp_err_t ssd1306_init(void);

/* 写入整屏显存：buf 为 1bpp 数据，共 1024 字节，按页组织（每页 128 字节） */
esp_err_t ssd1306_write_full_screen(const uint8_t *buf_1bpp);
