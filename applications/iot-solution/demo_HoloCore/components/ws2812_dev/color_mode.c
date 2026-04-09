/**
 * @file color_mode.c
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-07-10
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "color_mode.h"
#include <math.h>
#include <stdint.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/**
 * @brief 数码管颜渐变 紫——>蓝
 *
 */
color_t seg_color_FF00FF_to_0000FF[4][7][2] = {
	// 第一个数码管 - 淡紫色（降低红色）
	{
		{{180, 50, 220}, {160, 40, 200}}, // A
		{{190, 60, 230}, {170, 50, 210}}, // B
		{{200, 70, 240}, {180, 60, 220}}, // C
		{{190, 60, 230}, {170, 50, 210}}, // D
		{{180, 50, 220}, {160, 40, 200}}, // E
		{{200, 70, 240}, {180, 60, 220}}, // F
		{{190, 60, 230}, {170, 50, 210}}, // G
	},
	// 第二个数码管 - 蓝紫色（开始明显向蓝色过渡）
	{
		{{130, 60, 230}, {110, 50, 210}}, // A
		{{140, 70, 240}, {120, 60, 220}}, // B
		{{150, 80, 250}, {130, 70, 230}}, // C
		{{140, 70, 240}, {120, 60, 220}}, // D
		{{130, 60, 230}, {110, 50, 210}}, // E
		{{150, 80, 250}, {130, 70, 230}}, // F
		{{140, 70, 240}, {120, 60, 220}}, // G
	},
	// 第三个数码管 - 深蓝色调
	{
		{{80, 70, 240}, {60, 60, 220}},	 // A
		{{90, 80, 250}, {70, 70, 230}},	 // B
		{{100, 90, 255}, {80, 80, 240}}, // C
		{{90, 80, 250}, {70, 70, 230}},	 // D
		{{80, 70, 240}, {60, 60, 220}},	 // E
		{{100, 90, 255}, {80, 80, 240}}, // F
		{{90, 80, 250}, {70, 70, 230}},	 // G
	},
	// 第四个数码管 - 纯蓝色调
	{
		{{30, 80, 250}, {20, 70, 230}},	 // A
		{{40, 90, 255}, {30, 80, 240}},	 // B
		{{50, 100, 255}, {40, 90, 250}}, // C
		{{40, 90, 255}, {30, 80, 240}},	 // D
		{{30, 80, 250}, {20, 70, 230}},	 // E
		{{50, 100, 255}, {40, 90, 250}}, // F
		{{40, 90, 255}, {30, 80, 240}},	 // G
	},
};
/**
 * @brief 数码管颜色 深蓝——>浅绿
 *
 */
color_t seg_color_0000FF_to_00FF00[4][7][2] = {
	// 第一个数码管 - 深蓝色
	{
		{{0, 50, 200}, {0, 40, 180}}, // A
		{{0, 60, 210}, {0, 50, 190}}, // B
		{{0, 70, 220}, {0, 60, 200}}, // C
		{{0, 60, 210}, {0, 50, 190}}, // D
		{{0, 50, 200}, {0, 40, 180}}, // E
		{{0, 70, 220}, {0, 60, 200}}, // F
		{{0, 60, 210}, {0, 50, 190}}, // G
	},
	// 第二个数码管 - 蓝绿色
	{
		{{0, 100, 180}, {0, 90, 160}},	// A
		{{0, 110, 190}, {0, 100, 170}}, // B
		{{0, 120, 200}, {0, 110, 180}}, // C
		{{0, 110, 190}, {0, 100, 170}}, // D
		{{0, 100, 180}, {0, 90, 160}},	// E
		{{0, 120, 200}, {0, 110, 180}}, // F
		{{0, 110, 190}, {0, 100, 170}}, // G
	},
	// 第三个数码管 - 青绿色
	{
		{{0, 150, 150}, {0, 140, 140}}, // A
		{{0, 160, 160}, {0, 150, 150}}, // B
		{{0, 170, 170}, {0, 160, 160}}, // C
		{{0, 160, 160}, {0, 150, 150}}, // D
		{{0, 150, 150}, {0, 140, 140}}, // E
		{{0, 170, 170}, {0, 160, 160}}, // F
		{{0, 160, 160}, {0, 150, 150}}, // G
	},
	// 第四个数码管 - 浅绿色
	{
		{{0, 200, 100}, {0, 190, 90}},	// A
		{{0, 210, 110}, {0, 200, 100}}, // B
		{{0, 220, 120}, {0, 210, 110}}, // C
		{{0, 210, 110}, {0, 200, 100}}, // D
		{{0, 200, 100}, {0, 190, 90}},	// E
		{{0, 220, 120}, {0, 210, 110}}, // F
		{{0, 210, 110}, {0, 200, 100}}, // G
	},
};
/**
 * @brief 数码管颜色 渐变 红——>橙——>黄——>淡黄
 *
 */
color_t seg_color_FF0000_to_FF00FF[4][7][2] = {
	// 第一个数码管 - 深红色
	{
		{{200, 30, 30}, {180, 20, 20}}, // A
		{{210, 40, 40}, {190, 30, 30}}, // B
		{{220, 50, 50}, {200, 40, 40}}, // C
		{{210, 40, 40}, {190, 30, 30}}, // D
		{{200, 30, 30}, {180, 20, 20}}, // E
		{{220, 50, 50}, {200, 40, 40}}, // F
		{{210, 40, 40}, {190, 30, 30}}, // G
	},
	// 第二个数码管 - 橙色
	{
		{{220, 100, 30}, {200, 90, 20}},  // A
		{{230, 110, 40}, {210, 100, 30}}, // B
		{{240, 120, 50}, {220, 110, 40}}, // C
		{{230, 110, 40}, {210, 100, 30}}, // D
		{{220, 100, 30}, {200, 90, 20}},  // E
		{{240, 120, 50}, {220, 110, 40}}, // F
		{{230, 110, 40}, {210, 100, 30}}, // G
	},
	// 第三个数码管 - 金黄色
	{
		{{230, 150, 50}, {210, 140, 40}}, // A
		{{240, 160, 60}, {220, 150, 50}}, // B
		{{250, 170, 70}, {230, 160, 60}}, // C
		{{240, 160, 60}, {220, 150, 50}}, // D
		{{230, 150, 50}, {210, 140, 40}}, // E
		{{250, 170, 70}, {230, 160, 60}}, // F
		{{240, 160, 60}, {220, 150, 50}}, // G
	},
	// 第四个数码管 - 淡黄色
	{
		{{240, 200, 100}, {220, 190, 90}},	// A
		{{250, 210, 110}, {230, 200, 100}}, // B
		{{255, 220, 120}, {240, 210, 110}}, // C
		{{250, 210, 110}, {230, 200, 100}}, // D
		{{240, 200, 100}, {220, 190, 90}},	// E
		{{255, 220, 120}, {240, 210, 110}}, // F
		{{250, 210, 110}, {230, 200, 100}}, // G
	},
};
/**
 * @brief 数码管颜色 紫色系
 *
 */
color_t seg_color_FF00FF_to_F06E82[4][7][2] = {
	// 第一个数码管 - 深紫色
	{
		{{120, 30, 200}, {100, 20, 180}}, // A
		{{130, 40, 210}, {110, 30, 190}}, // B
		{{140, 50, 220}, {120, 40, 200}}, // C
		{{130, 40, 210}, {110, 30, 190}}, // D
		{{120, 30, 200}, {100, 20, 180}}, // E
		{{140, 50, 220}, {120, 40, 200}}, // F
		{{130, 40, 210}, {110, 30, 190}}, // G
	},
	// 第二个数码管 - 粉紫色
	{
		{{160, 50, 180}, {140, 40, 160}}, // A
		{{170, 60, 190}, {150, 50, 170}}, // B
		{{180, 70, 200}, {160, 60, 180}}, // C
		{{170, 60, 190}, {150, 50, 170}}, // D
		{{160, 50, 180}, {140, 40, 160}}, // E
		{{180, 70, 200}, {160, 60, 180}}, // F
		{{170, 60, 190}, {150, 50, 170}}, // G
	},
	// 第三个数码管 - 粉红色
	{
		{{200, 70, 150}, {180, 60, 130}}, // A
		{{210, 80, 160}, {190, 70, 140}}, // B
		{{220, 90, 170}, {200, 80, 150}}, // C
		{{210, 80, 160}, {190, 70, 140}}, // D
		{{200, 70, 150}, {180, 60, 130}}, // E
		{{220, 90, 170}, {200, 80, 150}}, // F
		{{210, 80, 160}, {190, 70, 140}}, // G
	},
	// 第四个数码管 - 淡粉色
	{
		{{230, 100, 120}, {210, 90, 100}},	// A
		{{240, 110, 130}, {220, 100, 110}}, // B
		{{250, 120, 140}, {230, 110, 120}}, // C
		{{240, 110, 130}, {220, 100, 110}}, // D
		{{230, 100, 120}, {210, 90, 100}},	// E
		{{250, 120, 140}, {230, 110, 120}}, // F
		{{240, 110, 130}, {220, 100, 110}}, // G
	},
};
/**
 * @brief 将HSV颜色转换为RGB颜色
 *
 * @param hsv
 * @return color_t_t
 */
color_t hsv_to_rgb(hsv_color_t hsv)
{
	float h = hsv.h;
	float s = hsv.s;
	float v = hsv.v;

	int hi = (int)(h / 60) % 6;
	float f = h / 60.0 - hi;

	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1 - (1 - f) * s);

	color_t rgb;
	switch (hi)
	{
	case 0:
		rgb.r = v * 255;
		rgb.g = t * 255;
		rgb.b = p * 255;
		break;
	case 1:
		rgb.r = q * 255;
		rgb.g = v * 255;
		rgb.b = p * 255;
		break;
	case 2:
		rgb.r = p * 255;
		rgb.g = v * 255;
		rgb.b = t * 255;
		break;
	case 3:
		rgb.r = p * 255;
		rgb.g = q * 255;
		rgb.b = v * 255;
		break;
	case 4:
		rgb.r = t * 255;
		rgb.g = p * 255;
		rgb.b = v * 255;
		break;
	case 5:
		rgb.r = v * 255;
		rgb.g = p * 255;
		rgb.b = q * 255;
		break;
	}

	// 添加舍入处理，减少精度损失
	rgb.r = (uint8_t)(rgb.r + 0.5f);
	rgb.g = (uint8_t)(rgb.g + 0.5f);
	rgb.b = (uint8_t)(rgb.b + 0.5f);

	return rgb;
}
/**
 * @brief 将RGB颜色转换为HSV颜色
 *
 * @param rgb
 * @param hsv
 */
hsv_color_t rgb_to_hsv(color_t rgb)
{
	float r = rgb.r / 255.0f;
	float g = rgb.g / 255.0f;
	float b = rgb.b / 255.0f;

	float max = fmax(r, fmax(g, b));
	float min = fmin(r, fmin(g, b));
	float delta = max - min;
	hsv_color_t hsv;
	// 避免除零错误（虽然已经检查过delta）
	float divisor = delta > 0 ? delta : 1e-6f;

	if (delta == 0)
	{
		hsv.h = 0;
	}
	else if (max == r)
	{
		// 使用更稳定的公式，减少边界跳变
		hsv.h = 60 * (fmod((g - b) / divisor, 6));
		if (hsv.h < 0)
			hsv.h += 360;
	}
	else if (max == g)
	{
		hsv.h = 60 * ((b - r) / divisor + 2);
	}
	else
	{
		hsv.h = 60 * ((r - g) / divisor + 4);
	}

	hsv.s = (max == 0) ? 0 : (delta / max);
	hsv.v = max;

	return hsv;
}

// 计算两个颜色之间的插值（不使用 A 通道）
color_t interpolatecolor_t(color_t start, color_t end, float t)
{
	color_t result;
	// 确保 t 在 0 到 1 的范围内
	if (t < 0.0f)
		t = 0.0f;
	if (t > 1.0f)
		t = 1.0f;

	// 使用线性插值计算每个通道的值
	result.r = (unsigned char)(start.r + (end.r - start.r) * t);
	result.g = (unsigned char)(start.g + (end.g - start.g) * t);
	result.b = (unsigned char)(start.b + (end.b - start.b) * t);

	return result;
}

// 1. 线性缓动（无加速）
static float easeLinear(float t)
{
	return t;
}

// 2. 二次方缓动（先慢后快）
static float easeInQuad(float t)
{
	return t * t;
}

// 3. 二次方缓动（先快后慢）
static float easeOutQuad(float t)
{
	return -t * (t - 2);
}

// 4. 二次方缓动（先慢中间快后慢）
static float easeInOutQuad(float t)
{
	if (t < 0.5f)
		return 2 * t * t;
	return -1 + (4 - 2 * t) * t;
}

// 5. 三次方缓动（快速加速）
static float easeInCubic(float t)
{
	return t * t * t;
}

// 6. 三次方缓动（快速减速）
static float easeOutCubic(float t)
{
	t--;
	return t * t * t + 1;
}

// 7. 弹性缓动（结束时有回弹效果）
static float easeOutElastic(float t)
{
	if (t == 0 || t == 1)
		return t;
	return pow(2, -10 * t) * sin((t * 10 - 0.75) * (2 * M_PI) / 3) + 1;
}

// 8. 弹跳缓动（结束时有弹跳效果）
float easeOutBounce(float t)
{
	if (t < 1 / 2.75)
	{
		return 7.5625 * t * t;
	}
	else if (t < 2 / 2.75)
	{
		t -= 1.5 / 2.75;
		return 7.5625 * t * t + 0.75;
	}
	else if (t < 2.5 / 2.75)
	{
		t -= 2.25 / 2.75;
		return 7.5625 * t * t + 0.9375;
	}
	else
	{
		t -= 2.625 / 2.75;
		return 7.5625 * t * t + 0.984375;
	}
}

// 实现平滑的颜色过渡
void smoothcolorTransition(color_t start, color_t end, int steps, void (*updateCallback)(color_t, void *), void *userData)
{
	if (updateCallback == NULL)
	{
		return;
	}
	for (int i = 0; i <= steps; i++)
	{
		// 计算当前的插值比例
		float t = (float)i / steps;

		// 应用缓动函数使过渡更平滑
		float easedT = easeInOutQuad(t); // 线性缓动，可替换为其他缓动函数

		// 计算当前颜色
		color_t current = interpolatecolor_t(start, end, easedT);
		// 调用回调函数更新颜色
		updateCallback(current, userData);
	}
}

/**
 * @brief 生成360度渐变
 *
 * @param steps
 * @param brightness 亮度，0.00~1.0
 * @param callback
 * @param data
 */
void generate360Gradient(int steps, float brightness, void (*callback)(color_t, int, void *), void *data)
{
	for (int i = 0; i < steps; i++)
	{
		// 计算当前角度 (0-360度)
		float hue = (float)i / steps * 360.0;
		hsv_color_t hsv = {hue, 1.0, 1.0};
		// 转换为RGB (饱和度和亮度设为100%)
		color_t color = hsv_to_rgb(hsv);

		// 调用回调处理颜色
		callback(color, i, data);
	}
}
/**
 * @brief 根据给定的位置计算颜色
 *
 * @param easing
 * @param speed
 */
void animateColorWheel(uint8_t speed, void (*updateColorOutput)(color_t, void *), void *userData)
{
	static uint8_t position = 0;
	position = (position + speed) % 256;

	// 应用缓动函数
	uint8_t easedPos = easeInOutQuad(position);
	hsv_color_t hsv = {easedPos, 1.0, 0.2};
	// 计算当前颜色
	color_t currentColor = hsv_to_rgb(hsv);

	// 更新输出 (需根据硬件实现)
	updateColorOutput(currentColor, userData);
}