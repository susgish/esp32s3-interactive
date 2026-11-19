#pragma once

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"
#include "driver/i2c_master.h"

/**
 * @file i2c_bus.h
 * @brief I2C 主设备总线封装，内部用互斥量保护共享总线。
 */

esp_err_t i2c_bus_init(void);

/* 在总线上注册一个设备，返回设备句柄 */
esp_err_t i2c_bus_add_device(uint8_t addr, uint32_t speed_hz,
                             i2c_master_dev_handle_t *out_dev);

/* 向设备写数据（自动加互斥锁） */
esp_err_t i2c_bus_write(i2c_master_dev_handle_t dev,
                        const uint8_t *data, size_t len);
