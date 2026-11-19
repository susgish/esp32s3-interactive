#include "rgb_led.h"
#include "pinmap.h"

#include "driver/ledc.h"
#include "esp_log.h"

#define TAG "rgb_led"

/* 3 路 PWM 共用 1 个 LEDC 定时器，13 位分辨率 */
#define LEDC_MODE       LEDC_LOW_SPEED_MODE
#define LEDC_TIMER      LEDC_TIMER_0
#define LEDC_FREQ_HZ    5000
#define LEDC_RES_BITS   LEDC_TIMER_13_BIT

/* 13 位分辨率的占空比上限 */
#define LEDC_DUTY_MAX   ((1u << 13) - 1)

static void channel_init(ledc_channel_t ch, int gpio)
{
    ledc_channel_config_t cfg = {
        .gpio_num   = gpio,
        .speed_mode = LEDC_MODE,
        .channel    = ch,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = LEDC_TIMER,
        .duty       = 0,
        .hpoint     = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&cfg));
}

esp_err_t rgb_led_init(void)
{
    ledc_timer_config_t tcfg = {
        .speed_mode      = LEDC_MODE,
        .duty_resolution = LEDC_RES_BITS,
        .timer_num       = LEDC_TIMER,
        .freq_hz         = LEDC_FREQ_HZ,
        .clk_cfg         = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&tcfg));

    channel_init(LEDC_CHANNEL_0, PIN_RGB_R);
    channel_init(LEDC_CHANNEL_1, PIN_RGB_G);
    channel_init(LEDC_CHANNEL_2, PIN_RGB_B);

    /* 初始全灭 */
    rgb_led_set(0, 0, 0);

    ESP_LOGI(TAG, "RGB LED ready (R=GPIO%d, G=GPIO%d, B=GPIO%d, LEDC 13-bit)",
             PIN_RGB_R, PIN_RGB_G, PIN_RGB_B);
    return ESP_OK;
}

void rgb_led_set(uint32_t r, uint32_t g, uint32_t b)
{
    /* 越界钳位，避免写入非法占空比 */
    if (r > LEDC_DUTY_MAX) r = LEDC_DUTY_MAX;
    if (g > LEDC_DUTY_MAX) g = LEDC_DUTY_MAX;
    if (b > LEDC_DUTY_MAX) b = LEDC_DUTY_MAX;

    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_0, r);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, g);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_2, b);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_0);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2);
}
