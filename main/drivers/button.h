#pragma once

#include "esp_err.h"

/**
 * @file button.h
 * @brief 按键驱动：GPIO 中断 -> 队列（消抖在 input_task 完成）。
 */

esp_err_t button_init(void);
