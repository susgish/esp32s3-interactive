# GitHub Issues 记录





## Issue #1：按键时间戳使用 FreeRTOS tick，长时间运行后长按判定会出错

**标题**：长按识别的时间戳在系统运行约 49.7 天后溢出，导致长按误判

**标签**：`bug` `freertos` `input`

**状态**：已修复（Fixed）

### 描述

`input_task` 依赖 `raw_btn_event_t.timestamp_ms` 计算按键持续时间，
而该时间戳来自 `xTaskGetTickCountFromISR() * portTICK_PERIOD_MS`，为 32 位毫秒计数。
FreeRTOS 时钟节拍在 1000Hz 下约 49.7 天回绕一次，回绕后
`duration = timestamp_now - press_start_ms` 的减法结果会变成巨大值，
导致一次正常短按被误判为长按（或反之）。

### 复现步骤

1. 让系统连续运行超过 `2^32` 毫秒（约 49.7 天），不重启；
2. 在回绕临界点附近短按 BOOT 键；
3. 观察串口日志：按键时长被打印为异常大的数值。

### 预期 vs 实际

- **预期**：短按始终触发 `BTN_EVT_SHORT_PRESS`，页面切换一次。
- **实际**：回绕后短按可能触发 `BTN_EVT_LONG_PRESS`，直接回到首页。

### 初步原因

`main/app_events.h` 中 `timestamp_ms` 为 `uint32_t`；
`main/tasks/input_task.c` 中 `duration = raw.timestamp_ms - press_start_ms`
未处理无符号回绕。

### 建议修复

改用 64 位时间源（`esp_timer_get_time()` 返回微秒），或在减法时
对回绕做补偿（`(int32_t)(now - start)` 且限制按键时长上界）。

### 修复说明

已按上述建议修复：`raw_btn_event_t.timestamp_us` 改用 `esp_timer_get_time()` 的
64 位微秒计数（见 `main/app_events.h`、`main/drivers/button.c`、`main/tasks/input_task.c`），
回绕周期提升到约 29 万年，问题消除。本 issue 关闭。

---

## Issue #2：SSD1306 显示方向未在真实硬件上验证

**标题**：无硬件环境下 SSD1306 翻转配置（0xA1/0xC8）可能与实际屏相反

**标签**：`hardware-verify` `oled`

**状态**：Open

### 描述

`ssd1306.c` 初始化序列使用 `0xA1`（段重映射）与 `0xC8`（COM 扫描方向）的
常见组合。本工程在无硬件环境下编写，未烧录验证。不同厂商 SSD1306 模块的
装配方向可能不同，导致显示内容上下/左右镜像。

### 复现步骤

1. 烧录到实际硬件并上电；
2. 观察首页文字方向。

### 预期 vs 实际

- **预期**：文字正向显示。
- **实际**：可能出现上下或左右翻转。

### 初步原因

`main/drivers/ssd1306.c` 中初始化序列的翻转位与实际屏装配方向不匹配。

### 建议修复

烧录后若方向异常，按需翻转 `0xA1` ↔ `0xA0`、`0xC8` ↔ `0xC0`；
最终确定的值回写 `pinmap.h` 或 `ssd1306.c` 并附硬件实测说明。
