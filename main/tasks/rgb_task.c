#include "tasks.h"
#include "rgb_led.h"
#include "app_config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <math.h>

#define TAG "rgb"

static const float PI = 3.14159265f;

/**
 * 三色灯呼吸任务（选做项）：
 *  - 三路正弦波相位各差 120°，叠加成平滑的 RGB 呼吸循环；
 *  - 每 20ms 刷新一次占空比，不与其他任务共享状态，无需 IPC。
 */
void rgb_task(void *arg)
{
    (void)arg;

    const float period_ms = 3000.0f;   /* 一个完整呼吸周期 */
    const uint32_t step_ms = 20;       /* 刷新间隔 */
    const uint32_t duty_max = (1u << 13) - 1;
    uint32_t t = 0;

    ESP_LOGI(TAG, "breathing start (period %.0f ms)", period_ms);

    while (1) {
        float phase = 2.0f * PI * (float)t / period_ms;
        uint32_t r = (uint32_t)((sinf(phase) + 1.0f) * 0.5f * (float)duty_max);
        uint32_t g = (uint32_t)((sinf(phase + 2.0f * PI / 3.0f) + 1.0f) * 0.5f * (float)duty_max);
        uint32_t b = (uint32_t)((sinf(phase + 4.0f * PI / 3.0f) + 1.0f) * 0.5f * (float)duty_max);
        rgb_led_set(r, g, b);

        t = (t + step_ms) % (uint32_t)period_ms;
        vTaskDelay(pdMS_TO_TICKS(step_ms));
    }
}
