/**
 * @file main.c
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-07-23
 *
 * @copyright Ai-Thinker co.,ltd (c) 2025
 *
 */

#include <bl_ir.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <bl_irq.h>
#include <bl_sys.h>

#include "ws2812.h"
#include "color_mode.h"
#include "blog.h"
color_t RED = {0xff, 0x00, 0x00};
color_t GREEN = {0x00, 0xff, 0x00};
color_t BLUE = {0x00, 0x00, 0xff};
/**
 * @brief 定义WS2812灯带 只能使用 GPIO11 并且外部需要提供 1K~2K 的上拉电阻
 *
 */
static ws2812_strip_t ws2812_strip = {
    .led_count = 46,
    .brightness = 0.05,
    .pin = 12,
};

static void smoothcolorTransition_callbark(color_t color, void *arg)
{
    ws2812_set_all_pixels_color(color.r, color.g, color.b, ws2812_strip.brightness);
    vTaskDelay(pdMS_TO_TICKS(5));
}

void main(void)
{
    bl_sys_init(); // 初始化系统
    ws2812_init(&ws2812_strip);
    ws2812_set_pixel_color(0, 0xff, 0x00, 0x00);
    ws2812_set_pixel_brightness(0, 0.5);
    ws2812_show_leds();
    dns_init();
    // blog_info("ws2812 demo start");
    // ws2812_set_all_pixels_color(0xFF, 0x00, 0x00, 0.5);
    // vTaskDelay(pdMS_TO_TICKS(1000));
    // ws2812_set_all_pixels_color(0x00, 0xFF, 0x00, 0.5);
    // vTaskDelay(pdMS_TO_TICKS(1000));
    // ws2812_set_all_pixels_color(0x00, 0x00, 0xFF, 0.5);
    vTaskDelay(pdMS_TO_TICKS(1000));

    while (1)
    {
        // 颜色渐变模式
        smoothcolorTransition(RED, BLUE, 500, smoothcolorTransition_callbark, NULL);
        smoothcolorTransition(BLUE, RED, 500, smoothcolorTransition_callbark, NULL);
    }
}