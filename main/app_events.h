#pragma once

#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

/**
 * @file app_events.h
 * @brief 任务间通信“契约”：队列消息结构、事件组位定义、句柄声明。
 *
 * 数据流（详见 docs/design.md）：
 *   ISR --[g_button_queue]--> input_task --[g_ui_queue]--> ui_task
 *   input_task --[g_feedback_eg]--> feedback_task
 */

/* 原始按键事件：ISR 直接发出，仅记录电平与时刻。
 * 消抖与长短按识别交由 input_task 完成（ISR 内不做重活）。
 * 时间戳来自 esp_timer_get_time() 的 64 位微秒计数，长时间运行不回绕
 * （原 32 位毫秒方案约 49.7 天回绕，见 docs/ISSUE.md Issue #1）。 */
typedef struct {
    int64_t timestamp_us;   /* 事件时刻（esp_timer 微秒，自开机） */
    uint8_t  level;         /* 0 = 按下，1 = 释放 */
} raw_btn_event_t;

/* 语义化按键事件：input_task 消抖 + 长短按识别后的结果 */
typedef enum {
    BTN_EVT_SHORT_PRESS = 1,  /* 短按：切换页面 */
    BTN_EVT_LONG_PRESS  = 2,  /* 长按：回到首页 */
} btn_event_t;

/* 声光反馈事件组位 */
#define FEEDBACK_BIT_LED   (1 << 0)
#define FEEDBACK_BIT_BEEP  (1 << 1)

/* 通信对象句柄（定义见 main.c） */
extern QueueHandle_t g_button_queue;     /* 元素类型 raw_btn_event_t */
extern QueueHandle_t g_ui_queue;         /* 元素类型 btn_event_t */
extern EventGroupHandle_t g_feedback_eg; /* 反馈触发位 */
