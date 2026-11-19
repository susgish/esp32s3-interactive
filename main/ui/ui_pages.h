#pragma once

/**
 * @file ui_pages.h
 * @brief 页面创建与切换接口。
 */

void ui_pages_create(void);
void ui_show_page(int idx);
void ui_next_page(void);
int ui_get_current_page(void);
