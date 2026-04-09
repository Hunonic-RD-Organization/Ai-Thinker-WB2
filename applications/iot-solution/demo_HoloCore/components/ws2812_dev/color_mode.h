/**
 * @file color_mode.h
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-07-10
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef COLOR_MODE_H
#define COLOR_MODE_H
#include <stdint.h>

typedef struct
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} color_t;

typedef struct
{
	float h; // 色相 (0-360)
	float s; // 饱和度 (0-1)
	float v; // 亮度 (0-1)
} hsv_color_t;


color_t hsv_to_rgb(hsv_color_t hsv);
hsv_color_t rgb_to_hsv(color_t rgb);
void smoothcolorTransition(color_t start, color_t end, int steps, void (*updateCallback)(color_t, void *), void *userData);
void generate360Gradient(int steps, float brightness, void (*callback)(color_t, int, void *), void *data);
void animateColorWheel(uint8_t speed, void (*updateColorOutput)(color_t, void *), void *userData);
#endif // COLOR_MODE_H