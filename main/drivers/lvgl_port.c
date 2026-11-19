#include "lvgl_port.h"

#include "lvgl.h"
#include "ssd1306.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_check.h"

#define TAG "lvgl_port"

static lv_display_t *s_disp = NULL;

/* L8 灰度全屏缓冲：128 * 64 * 1 字节 = 8KB。
 * 采用 L8 而非 16bit 色深，是为适配 SSD1306 单色屏、节省内存，
 * 并在 flush 回调中做阈值转换（见下方说明）。 */
static uint8_t s_buf[SSD1306_WIDTH * SSD1306_HEIGHT];

static esp_timer_handle_t s_tick_timer = NULL;

/* LVGL 时间源：每 2ms 递增一次内部 tick */
static void tick_cb(void *arg)
{
    (void)arg;
    lv_tick_inc(2);
}

/**
 * 显示 flush 回调（LVGL 渲染完成后调用）。
 *
 * LVGL 输出的是 L8 灰度数据（每像素 1 字节，0~255），
 * 这里按阈值 127 二值化：>127 视为“亮”，否则“灭”，
 * 再打包成 SSD1306 的 1bpp GDDRAM（每字节 8 像素，bit0 = 页内最顶行）。
 */
static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    (void)area; /* FULL 渲染模式下 px_map 即整屏缓冲 */

    static uint8_t gddram[SSD1306_WIDTH * SSD1306_PAGES];

    for (int page = 0; page < SSD1306_PAGES; page++) {
        for (int col = 0; col < SSD1306_WIDTH; col++) {
            uint8_t byte = 0;
            for (int bit = 0; bit < 8; bit++) {
                int y = page * 8 + bit;
                if (px_map[y * SSD1306_WIDTH + col] > 127) {
                    byte |= (1 << bit);
                }
            }
            gddram[page * SSD1306_WIDTH + col] = byte;
        }
    }

    ssd1306_write_full_screen(gddram);
    lv_display_flush_ready(disp);
}

void lvgl_port_init(void)
{
    lv_init();

    s_disp = lv_display_create(SSD1306_WIDTH, SSD1306_HEIGHT);
    lv_display_set_color_format(s_disp, LV_COLOR_FORMAT_L8);
    lv_display_set_buffers(s_disp, s_buf, NULL, sizeof(s_buf),
                           LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(s_disp, flush_cb);

    esp_timer_create_args_t args = {
        .callback = tick_cb,
        .name = "lvgl_tick",
    };
    ESP_ERROR_CHECK(esp_timer_create(&args, &s_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(s_tick_timer, 2000)); /* 2000us = 2ms */

    ESP_LOGI(TAG, "LVGL port ready (L8 full-buffer -> 1bpp SSD1306)");
}
