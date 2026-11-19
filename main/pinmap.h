#pragma once

#include "driver/gpio.h"

/**
 * @file pinmap.h
 * @brief 引脚集中定义（ESP32-S3-DevKitC-1）
 *
 * 注意：本工程在“无硬件”环境下编写，引脚依据 Espressif 官方
 * ESP32-S3-DevKitC-1 原理图 / 数据手册定义。烧录前请核对实际接线。
 * 若使用自有板子，仅需修改本文件即可，其余代码不受影响。
 */

/* ---- I2C（SSD1306 OLED）----
 * ESP32-S3 任意空闲 GPIO 均可作 I2C，此处选用 GPIO8(SDA)/GPIO9(SCL)。
 * SSD1306 模块 I2C 地址默认 0x3C（见 ssd1306.c）。
 * 需在 SDA/SCL 上外接 4.7kΩ 上拉电阻（模块通常自带）。 */
#define PIN_I2C_SDA   GPIO_NUM_8
#define PIN_I2C_SCL   GPIO_NUM_9

/* ---- 按键 ----
 * DevKitC-1 板载 BOOT 键（GPIO0），按下为低电平，配置内部上拉 + 边沿中断。 */
#define PIN_BTN       GPIO_NUM_0

/* ---- 声光反馈（外部扩展，需自行接线）----
 * LED：高电平点亮，需串联限流电阻（约 330Ω）。 */
#define PIN_LED       GPIO_NUM_4
/* 蜂鸣器：主动式，高电平鸣响，建议经三极管（如 S8050）驱动。 */
#define PIN_BUZZER    GPIO_NUM_5

/* ---- 三色灯 RGB（呼吸灯，选做项）----
 * 外接共阴极 RGB LED，3 路 LEDC PWM 调光实现呼吸渐变。
 * 需各串联限流电阻（约 330Ω）；若用共阳极，请把 rgb_led.c 中
 * 占空比做反相（duty -> LEDC_DUTY_MAX - duty）。 */
#define PIN_RGB_R     GPIO_NUM_6
#define PIN_RGB_G     GPIO_NUM_7
#define PIN_RGB_B     GPIO_NUM_10
