#include "ui.h"
#include "ui_pages.h"

#include "lvgl.h"
#include "themes/default/lv_theme_default.h"
#include "misc/lv_palette.h"
#include "esp_log.h"

#define TAG "ui"

void ui_init(void)
{
    lv_display_t *disp = lv_display_get_default();

    /* 深色主题：背景黑、文字白。
     * SSD1306 单色屏按灰度阈值二值化（亮=1），深色主题可保证
     * 背景“灭”、文字“亮”，显示正常不反相。 */
    lv_theme_t *theme = lv_theme_default_init(disp,
                                              lv_palette_main(LV_PALETTE_BLUE),
                                              lv_palette_main(LV_PALETTE_RED),
                                              true, LV_FONT_DEFAULT);
    lv_display_set_theme(disp, theme);

    ui_pages_create();
    ui_show_page(0);

    ESP_LOGI(TAG, "UI ready (dark theme, 3 pages)");
}
