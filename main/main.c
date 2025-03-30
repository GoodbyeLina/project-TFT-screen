#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// 引脚定义
#define TFT_D0    GPIO_NUM_4
#define TFT_D1    GPIO_NUM_5
#define TFT_D2    GPIO_NUM_6
#define TFT_D3    GPIO_NUM_7
#define TFT_D4    GPIO_NUM_8
#define TFT_D5    GPIO_NUM_9
#define TFT_D6    GPIO_NUM_10
#define TFT_D7    GPIO_NUM_11
#define TFT_RST   GPIO_NUM_12
#define TFT_CS    GPIO_NUM_13
#define TFT_RS    GPIO_NUM_14
#define TFT_WR    GPIO_NUM_15
#define TFT_RD    GPIO_NUM_16

void init_gpio() {
    // 配置数据线为输出
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << TFT_D0) | (1ULL << TFT_D1) | (1ULL << TFT_D2) |
                       (1ULL << TFT_D3) | (1ULL << TFT_D4) | (1ULL << TFT_D5) |
                       (1ULL << TFT_D6) | (1ULL << TFT_D7),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    // 配置控制线
    io_conf.pin_bit_mask = (1ULL << TFT_RST) | (1ULL << TFT_CS) |
                          (1ULL << TFT_RS) | (1ULL << TFT_WR) |
                          (1ULL << TFT_RD);
    gpio_config(&io_conf);
}

// 写命令到TFT
void tft_write_cmd(uint8_t cmd) {
    gpio_set_level(TFT_RS, 0); // 命令模式
    gpio_set_level(TFT_CS, 0); // 选中显示屏
    
    // 设置数据线
    gpio_set_level(TFT_D0, (cmd >> 0) & 1);
    gpio_set_level(TFT_D1, (cmd >> 1) & 1);
    gpio_set_level(TFT_D2, (cmd >> 2) & 1);
    gpio_set_level(TFT_D3, (cmd >> 3) & 1);
    gpio_set_level(TFT_D4, (cmd >> 4) & 1);
    gpio_set_level(TFT_D5, (cmd >> 5) & 1);
    gpio_set_level(TFT_D6, (cmd >> 6) & 1);
    gpio_set_level(TFT_D7, (cmd >> 7) & 1);
    
    // 产生写脉冲
    gpio_set_level(TFT_WR, 0);
    gpio_set_level(TFT_WR, 1);
    
    gpio_set_level(TFT_CS, 1); // 取消选中
}

// 写数据到TFT
void tft_write_data(uint8_t data) {
    gpio_set_level(TFT_RS, 1); // 数据模式
    gpio_set_level(TFT_CS, 0); // 选中显示屏
    
    // 设置数据线
    gpio_set_level(TFT_D0, (data >> 0) & 1);
    gpio_set_level(TFT_D1, (data >> 1) & 1);
    gpio_set_level(TFT_D2, (data >> 2) & 1);
    gpio_set_level(TFT_D3, (data >> 3) & 1);
    gpio_set_level(TFT_D4, (data >> 4) & 1);
    gpio_set_level(TFT_D5, (data >> 5) & 1);
    gpio_set_level(TFT_D6, (data >> 6) & 1);
    gpio_set_level(TFT_D7, (data >> 7) & 1);
    
    // 产生写脉冲
    gpio_set_level(TFT_WR, 0);
    gpio_set_level(TFT_WR, 1);
    
    gpio_set_level(TFT_CS, 1); // 取消选中
}

void init_tft() {
    // 初始化GPIO
    init_gpio();

    // 复位显示屏
    gpio_set_level(TFT_RST, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(TFT_RST, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // ILI9341初始化序列
    uint8_t init_cmds[] = {
        0xEF, 3, 0x03, 0x80, 0x02,
        0xCF, 3, 0x00, 0xC1, 0x30,
        0xED, 4, 0x64, 0x03, 0x12, 0x81,
        0xE8, 3, 0x85, 0x00, 0x78,
        0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
        0xF7, 1, 0x20,
        0xEA, 2, 0x00, 0x00,
        0xC0, 1, 0x23,
        0xC1, 1, 0x10,
        0xC5, 2, 0x3e, 0x28,
        0xC7, 1, 0x86,
        0x36, 1, 0x48,
        0x3A, 1, 0x55,
        0xB1, 2, 0x00, 0x18,
        0xB6, 3, 0x08, 0x82, 0x27,
        0xF2, 1, 0x00,
        0x26, 1, 0x01,
        0xE0, 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
        0xE1, 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
        0x11, 0,
        0x29, 0
    };

    // 发送初始化命令
    for (int i = 0; i < sizeof(init_cmds); ) {
        uint8_t cmd = init_cmds[i++];
        uint8_t len = init_cmds[i++];
        
        tft_write_cmd(cmd);
        
        if (len > 0) {
            for (int j = 0; j < len; j++) {
                tft_write_data(init_cmds[i++]);
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    // 初始化TFT显示屏
    init_tft();

    // TODO: 初始化LVGL

    while (1) {
        // TODO: LVGL任务处理
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}