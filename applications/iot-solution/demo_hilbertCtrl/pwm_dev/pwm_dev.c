/**
 * @file pwm_dev.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-06-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "pwm_dev.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <hosal_pwm.h>
#include <bl_pwm.h>
#include "blog.h"
static hosal_pwm_dev_t pwm;

/**
 * @brief 初始化PWM设备
 *
 * @param pin
 * @param freq
 * @param duty
 */
void pwm_dev_init(int pin, int freq, int duty)
{

	pwm.port = pin % PWM_CH_MAX;
	pwm.config.pin = pin;
	pwm.config.freq = freq;
	pwm.config.duty_cycle = duty * 10;
	int ret = hosal_pwm_init(&pwm);
	if (ret != 0)
	{
		blog_error("hosal_pwm_init failed");
		return;
	}
	blog_info("hosal_pwm_init success, pin:%d freq:%d duty:%d", pwm.config.pin, pwm.config.freq, pwm.config.duty_cycle);

	ret = hosal_pwm_start(&pwm);
	if (ret != 0)
	{
		blog_error("hosal_pwm_init failed");
		return;
	}
	blog_info("hosal_pwm_start success");
}
/**
 * @brief 设置占空比
 *
 * @param duty
 */
void pwm_dev_set_duty(int duty)
{
	hosal_pwm_config_t para;
	para.duty_cycle = duty * 10;
	para.freq = pwm.config.freq;
	int ret = hosal_pwm_para_chg(&pwm, para);
	if (ret != 0)
	{
		blog_error("hosal_pwm_para_chg failed");
		return;
	}
	blog_info("hosal_pwm_para_chg success");
	blog_info("pwm parameter: duty_cycle:%d freq:%d", para.duty_cycle, para.freq);
}
/**
 * @brief 关闭PWM设备
 *
 */
void pwm_dev_close(void)
{
	hosal_pwm_stop(&pwm);
	hosal_pwm_finalize(&pwm);
}
