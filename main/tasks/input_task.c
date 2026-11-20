#include "tasks.h"
#include "app_events.h"
#include "app_config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include <stdbool.h>
#include <stdint.h>

#define TAG "input"

/* 由 app_config.h 的毫秒参数换算成微秒（时间戳单位为 esp_timer 微秒） */
#define BTN_DEBOUNCE_US    ((int64_t)BTN_DEBOUNCE_MS * 1000)
#define BTN_LONG_PRESS_US  ((int64_t)BTN_LONG_PRESS_MS * 1000)

/**
 * 按键输入任务：
 *  - 从 g_button_queue 读取 ISR 发出的原始事件；
 *  - 边沿消抖：距上一次有效边沿 < BTN_DEBOUNCE_MS 的边沿视为抖动丢弃，
 *    同时过滤按下沿与释放沿的接触抖动，保证声光反馈只触发一次；
 *  - 长短按识别：>= BTN_LONG_PRESS_MS 为长按，否则为短按
 *  - 语义化事件通过 g_ui_queue 交给 ui_task；
 *  - 声光反馈通过 g_feedback_eg 事件组触发。
 */
void input_task(void *arg)
{
    (void)arg;
    raw_btn_event_t raw;
    bool pressed = false;
    int64_t press_start_us = 0;
    int64_t last_edge_us = 0;

    while (1) {
        if (xQueueReceive(g_button_queue, &raw, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        int64_t now_us = raw.timestamp_us;

        /* 边沿消抖：忽略抖动边沿 */
        if (now_us - last_edge_us < BTN_DEBOUNCE_US) {
            continue;
        }
        last_edge_us = now_us;

        if (raw.level == 0) {
            /* 按下：记录起始时刻，触发 LED 反馈 */
            if (!pressed) {
                pressed = true;
                press_start_us = now_us;
                xEventGroupSetBits(g_feedback_eg, FEEDBACK_BIT_LED);
            }
        } else {
            /* 释放：根据持续时间判断长短按，触发蜂鸣反馈 */
            if (pressed) {
                uint32_t duration_ms = (uint32_t)((now_us - press_start_us) / 1000);
                if (duration_ms >= BTN_LONG_PRESS_MS) {
                    btn_event_t e = BTN_EVT_LONG_PRESS;
                    xQueueSend(g_ui_queue, &e, 0);
                    ESP_LOGI(TAG, "long press (%lu ms)", (unsigned long)duration_ms);
                } else {
                    /* 已通过边沿消抖，说明持续时间 >= BTN_DEBOUNCE_MS，为有效短按 */
                    btn_event_t e = BTN_EVT_SHORT_PRESS;
                    xQueueSend(g_ui_queue, &e, 0);
                    ESP_LOGI(TAG, "short press (%lu ms)", (unsigned long)duration_ms);
                }
                xEventGroupSetBits(g_feedback_eg, FEEDBACK_BIT_BEEP);
                pressed = false;
            }
        }
    }
}
