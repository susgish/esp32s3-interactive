#include "button.h"
#include "pinmap.h"
#include "app_events.h"

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_timer.h"
#include "esp_log.h"

#define TAG "button"

/* 按键中断服务例程。
 * 原则：ISR 内只做最小工作——读电平、记时间戳、发队列。
 * 消抖与长短按识别交给 input_task，绝不在 ISR 里延时或打印。 */
static void IRAM_ATTR btn_isr(void *arg)
{
    (void)arg;
    raw_btn_event_t evt;
    evt.level = gpio_get_level(PIN_BTN);
    evt.timestamp_us = esp_timer_get_time();

    /* 使用 FromISR 版本；队列已满时丢弃事件（高优先级唤醒已发出） */
    xQueueSendFromISR(g_button_queue, &evt, NULL);
}

esp_err_t button_init(void)
{
    gpio_config_t cfg = {
        .pin_bit_mask = 1ULL << PIN_BTN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,     /* BOOT 键按下接地，需上拉 */
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE,       /* 捕获按下与释放 */
    };

    esp_err_t err = gpio_config(&cfg);
    if (err != ESP_OK) {
        return err;
    }

    /* 安装 ISR 服务；若已安装则返回 ESP_ERR_INVALID_STATE，可忽略 */
    err = gpio_install_isr_service(0);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        return err;
    }

    err = gpio_isr_handler_add(PIN_BTN, btn_isr, NULL);
    if (err != ESP_OK) {
        return err;
    }

    ESP_LOGI(TAG, "button ready (GPIO%d, any edge)", PIN_BTN);
    return ESP_OK;
}
