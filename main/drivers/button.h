#pragma once

#include "esp_err.h"

/**
 * @file button.h
 * @brief 按键驱动：GPIO 中断 -> 队列
 */

esp_err_t button_init(void);
