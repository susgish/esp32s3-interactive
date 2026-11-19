#pragma once

#include "esp_err.h"

/**
 * @file buzzer.h
 * @brief 蜂鸣器反馈：GPIO 直接开关（主动式蜂鸣器）。
 *        若使用无源蜂鸣器，需改用 LEDC PWM 输出音调（见 docs/design.md 后续思路）。
 */

esp_err_t buzzer_init(void);
void buzzer_on(void);
void buzzer_off(void);
