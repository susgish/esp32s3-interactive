#include "tasks.h"
#include "app_events.h"
#include "led.h"
#include "buzzer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

#define TAG "feedback"

/**
 * 反馈任务：
 *  - 等待 g_feedback_eg 事件组中的 LED / BEEP 位；
 *  - 触发后产生短促的声光反馈（LED 亮 50ms，蜂鸣 80ms）；
 *  - 反馈动作不阻塞 input_task / ui_task 的逻辑。
 */
void feedback_task(void *arg)
{
    (void)arg;

    while (1) {
        EventBits_t bits = xEventGroupWaitBits(g_feedback_eg,
                                               FEEDBACK_BIT_LED | FEEDBACK_BIT_BEEP,
                                               pdTRUE,   /* 清除已触发位 */
                                               pdFALSE,  /* 任意位满足即返回 */
                                               portMAX_DELAY);

        if (bits & FEEDBACK_BIT_LED) {
            led_on();
            vTaskDelay(pdMS_TO_TICKS(50));
            led_off();
        }
        if (bits & FEEDBACK_BIT_BEEP) {
            buzzer_on();
            vTaskDelay(pdMS_TO_TICKS(80));
            buzzer_off();
        }
    }
}
