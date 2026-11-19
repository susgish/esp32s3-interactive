#pragma once

#include <stdint.h>
#include "esp_err.h"

/**
 * @file rgb_led.h
 * @brief 三色灯（RGB）呼吸灯驱动：3 路 LEDC PWM 调光（选做项）。
 */

esp_err_t rgb_led_init(void);

/* 设置三路占空比，范围 [0, 8191]，立即生效 */
void rgb_led_set(uint32_t r, uint32_t g, uint32_t b);
