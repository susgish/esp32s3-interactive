# ESP32-S3 Interactive

ESP32-S3 + SSD1306（128×64 I2C OLED）+ LVGL v9 + FreeRTOS 的交互小系统（面试实操题）。

按键（板载 BOOT 键）切换 OLED 显示内容，并带有声光反馈。

## 功能

- **必做项全部实现**，`idf.py build` 编译通过
- **选做项完成 3 项**：三色灯呼吸、测试用例清单、简版项目计划
- 短按 BOOT 键切换页面（3 页循环），长按回到首页
- 按键按下时 LED 闪一下，释放时蜂鸣器响一下（边沿消抖，保证只触发一次）
- 三色灯 RGB 呼吸渐变（3 路 LEDC PWM + 独立任务）
- 4 个 FreeRTOS 任务，通过**队列 + 事件组 + 互斥量**通信

## 环境

| 项 | 版本 |
|---|---|
| ESP-IDF | v5.2.x（目标 `esp32s3`） |
| LVGL | v9.2.x（组件管理器自动拉取） |
| 编译目标 | ESP32-S3-DevKitC-1 |

## 构建

```bash
idf.py set-target esp32s3
idf.py build
```

首次 build 会自动通过组件管理器拉取 LVGL 依赖。

## 目录结构

```
esp32s3-interactive/
├── CMakeLists.txt
├── sdkconfig.defaults
├── main/
│   ├── CMakeLists.txt
│   ├── idf_component.yml      # LVGL 依赖（锁 9.2.x）
│   ├── main.c                 # 入口
│   ├── pinmap.h               # 引脚集中定义
│   ├── app_config.h           # 全局配置
│   ├── app_events.h           # 通信契约
│   ├── tasks.h
│   ├── drivers/               # 外设驱动（含 rgb_led.c 三色灯）
│   ├── ui/                    # LVGL 界面
│   └── tasks/                 # FreeRTOS 任务（含 rgb_task.c 呼吸灯）
└── docs/
    ├── design.md              # 设计文档
    ├── ISSUE.md               # GitHub Issue 正文（贴到仓库）
    ├── test-cases.md          # 测试用例清单（选做项）
    └── project-plan.md        # 简版项目计划（选做项）
```

## 文档

- 设计文档（硬件接口 / 任务划分 / 通信方式）：[docs/design.md](docs/design.md)
- 已知问题跟踪示例：[docs/ISSUE.md](docs/ISSUE.md)
- 测试用例清单（选做项）：[docs/test-cases.md](docs/test-cases.md)
- 简版项目计划（选做项）：[docs/project-plan.md](docs/project-plan.md)
