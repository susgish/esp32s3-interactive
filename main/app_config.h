#pragma once

/**
 * @file app_config.h
 * @brief 全局配置：版本、任务栈/优先级、队列深度、按键参数。
 */

#define APP_VERSION "0.1.0"

/* ---- FreeRTOS 任务 ---- */
#define INPUT_TASK_STACK      4096
#define UI_TASK_STACK         8192   /* LVGL 渲染需要较大栈 */
#define FEEDBACK_TASK_STACK   2048

#define INPUT_TASK_PRIO       5
#define UI_TASK_PRIO          5
#define FEEDBACK_TASK_PRIO    4
#define RGB_TASK_PRIO         3

#define RGB_TASK_STACK        2048

/* ---- 队列深度 ---- */
#define BUTTON_QUEUE_LEN      8
#define UI_QUEUE_LEN          8

/* ---- 按键参数（input_task 使用）---- */
#define BTN_DEBOUNCE_MS       20     /* 消抖时长：低于此值视为抖动 */
#define BTN_LONG_PRESS_MS     1000   /* 长按阈值 */
