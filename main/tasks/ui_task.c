#include "tasks.h"
#include "app_events.h"
#include "ui_pages.h"

#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"

#define TAG "ui"

/**
 * UI 任务：
 *  - 非阻塞消费 g_ui_queue 中的按键事件，切换/回到页面；
 *  - 周期调用 lv_timer_handler() 驱动 LVGL 渲染；
 *  - 按键事件通过队列、而非直接在 ISR 里操作 LVGL，
 *    保证所有 LVGL API 只在单一任务（本任务）中调用（LVGL 非线程安全）。
 */
void ui_task(void *arg)
{
    (void)arg;
    btn_event_t evt;

    while (1) {
        /* 处理所有待处理的按键事件（非阻塞） */
        while (xQueueReceive(g_ui_queue, &evt, 0) == pdTRUE) {
            if (evt == BTN_EVT_SHORT_PRESS) {
                ui_next_page();
                ESP_LOGI(TAG, "switch to page %d", ui_get_current_page());
            } else if (evt == BTN_EVT_LONG_PRESS) {
                ui_show_page(0);
                ESP_LOGI(TAG, "back to home");
            }
        }

        /* 驱动 LVGL 事件/渲染循环，按空闲时间休眠避免忙等 */
        uint32_t idle_ms = lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(idle_ms > 0 ? idle_ms : 5));
    }
}
