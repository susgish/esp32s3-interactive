#pragma once

#include "esp_err.h"

/**
 * @file led.h
 * @brief 单色 LED 反馈：GPIO 直接开关（按键声光反馈用）。
 *        三色灯呼吸效果见 rgb_led.h / tasks/rgb_task.c（选做项，已实现）。
 */

esp_err_t led_init(void);
void led_on(void);
void led_off(void);
