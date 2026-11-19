#include "ui_pages.h"
#include "app_config.h"

#include "lvgl.h"
#include "esp_idf_version.h"
#include <stdio.h>

#define PAGE_COUNT 3

static lv_obj_t *s_pages[PAGE_COUNT];
static int s_cur_page = 0;

/* 创建铺满全屏的页面容器（去边框/圆角，适配 128x64 小屏） */
static lv_obj_t *create_page(void)
{
    lv_obj_t *page = lv_obj_create(lv_screen_active());
    lv_obj_set_size(page, 128, 64);
    lv_obj_set_pos(page, 0, 0);
    lv_obj_set_style_pad_all(page, 0, 0);
    lv_obj_set_style_border_width(page, 0, 0);
    lv_obj_set_style_radius(page, 0, 0);
    return page;
}

static lv_obj_t *add_label(lv_obj_t *parent, const char *text, int x, int y)
{
    lv_obj_t *l = lv_label_create(parent);
    lv_label_set_text(l, text);
    lv_obj_set_pos(l, x, y);
    return l;
}

/* 页 0：首页 */
static void create_page0(void)
{
    lv_obj_t *p = create_page();
    s_pages[0] = p;
    add_label(p, "ESP32-S3", 4, 2);
    add_label(p, "Interactive", 4, 18);
    add_label(p, "Short: next", 4, 40);
    add_label(p, "Long: home", 4, 52);
}

/* 页 1：系统信息 */
static void create_page1(void)
{
    lv_obj_t *p = create_page();
    s_pages[1] = p;
    char buf[48];
    add_label(p, "System Info", 4, 2);
    snprintf(buf, sizeof(buf), "IDF  %s", IDF_VER);
    add_label(p, buf, 4, 18);
    snprintf(buf, sizeof(buf), "LVGL v%d.%d", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR);
    add_label(p, buf, 4, 30);
    add_label(p, "Chip ESP32-S3", 4, 42);
}

/* 页 2：关于 */
static void create_page2(void)
{
    lv_obj_t *p = create_page();
    s_pages[2] = p;
    char buf[48];
    add_label(p, "About", 4, 2);
    snprintf(buf, sizeof(buf), "v%s", APP_VERSION);
    add_label(p, buf, 4, 18);
    add_label(p, "Interview Demo", 4, 30);
    add_label(p, "SSD1306 + LVGL", 4, 42);
}

void ui_pages_create(void)
{
    create_page0();
    create_page1();
    create_page2();
}

void ui_show_page(int idx)
{
    if (idx < 0 || idx >= PAGE_COUNT) {
        idx = 0;
    }
    for (int i = 0; i < PAGE_COUNT; i++) {
        if (s_pages[i]) {
            lv_obj_add_flag(s_pages[i], LV_OBJ_FLAG_HIDDEN);
        }
    }
    if (s_pages[idx]) {
        lv_obj_clear_flag(s_pages[idx], LV_OBJ_FLAG_HIDDEN);
        lv_obj_move_foreground(s_pages[idx]);
    }
    s_cur_page = idx;
}

void ui_next_page(void)
{
    ui_show_page((s_cur_page + 1) % PAGE_COUNT);
}

int ui_get_current_page(void)
{
    return s_cur_page;
}
