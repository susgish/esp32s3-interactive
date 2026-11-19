/**
 * @file main.c
 * @brief 应用入口：初始化外设、创建通信对象与 FreeRTOS 任务。
 *
 * 任务划分（详见 docs/design.md）：
 *   - input_task    按键输入：消抖 + 长短按识别
 *   - ui_task       LVGL 界面：消费按键事件切换页面
 *   - feedback_task 声光反馈：消费事件组触发 LED/蜂鸣器
 *   - rgb_task      三色灯呼吸：周期刷新 3 路 LEDC PWM（选做项）
 *
 * 通信方式：
 *   - 队列 g_button_queue：ISR -> input_task（原始按键事件）
 *   - 队列 g_ui_queue：    input_task -> ui_task（语义化按键事件）
 *   - 事件组 g_feedback_eg：input_task -> feedback_task（反馈触发位）
 *   - 互斥量（i2c_bus 模块内部）：保护共享 I2C 总线
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_check.h"

#include "app_config.h"
#include "app_events.h"
#include "tasks.h"

#include "i2c_bus.h"
#include "ssd1306.h"
#include "button.h"
#include "led.h"
#include "buzzer.h"
#include "rgb_led.h"
#include "lvgl_port.h"
#include "ui.h"

static const char *TAG = "main";

/* 通信对象句柄定义（声明见 app_events.h） */
QueueHandle_t g_button_queue = NULL;
QueueHandle_t g_ui_queue = NULL;
EventGroupHandle_t g_feedback_eg = NULL;

void app_main(void)
{
    ESP_LOGI(TAG, "=== ESP32-S3 Interactive v%s ===", APP_VERSION);

    /* 1. 外设初始化（顺序敏感：I2C 总线 -> OLED -> LVGL -> UI） */
    ESP_ERROR_CHECK(i2c_bus_init());
    ESP_ERROR_CHECK(ssd1306_init());
    lvgl_port_init();
    ui_init();

    /* 2. 创建通信对象。必须在 button_init() 之前，
     *    否则按键 ISR 可能向未创建的队列发送数据。 */
    g_button_queue = xQueueCreate(BUTTON_QUEUE_LEN, sizeof(raw_btn_event_t));
    g_ui_queue = xQueueCreate(UI_QUEUE_LEN, sizeof(btn_event_t));
    g_feedback_eg = xEventGroupCreate();
    configASSERT(g_button_queue != NULL);
    configASSERT(g_ui_queue != NULL);
    configASSERT(g_feedback_eg != NULL);

    /* 3. 初始化会触发中断/产生输出的外设 */
    ESP_ERROR_CHECK(button_init());
    ESP_ERROR_CHECK(led_init());
    ESP_ERROR_CHECK(buzzer_init());
    ESP_ERROR_CHECK(rgb_led_init());

    /* 4. 创建任务 */
    xTaskCreate(input_task, "input", INPUT_TASK_STACK, NULL, INPUT_TASK_PRIO, NULL);
    xTaskCreate(ui_task, "ui", UI_TASK_STACK, NULL, UI_TASK_PRIO, NULL);
    xTaskCreate(feedback_task, "feedback", FEEDBACK_TASK_STACK, NULL, FEEDBACK_TASK_PRIO, NULL);
    xTaskCreate(rgb_task, "rgb", RGB_TASK_STACK, NULL, RGB_TASK_PRIO, NULL);

    ESP_LOGI(TAG, "All tasks started. Short press = next page, long press = home.");
}
