#pragma once

/**
 * @file tasks.h
 * @brief 三个 FreeRTOS 任务入口声明。
 */

void input_task(void *arg);     /* 按键输入：消抖 + 长短按识别 */
void ui_task(void *arg);        /* LVGL 界面：消费按键事件切换页面 */
void feedback_task(void *arg);  /* 声光反馈：消费事件组触发 LED/蜂鸣器 */
void rgb_task(void *arg);       /* 三色灯呼吸：周期刷新 3 路 LEDC PWM（选做项） */
