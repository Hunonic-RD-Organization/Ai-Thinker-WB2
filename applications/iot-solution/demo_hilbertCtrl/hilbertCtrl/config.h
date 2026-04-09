/**
 * @file config.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-06-11
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef CONFIG_H
#define CONFIG_H

#include "pwm_dev.h"
#include "ntc_adc.h"
#include "ntc_heat_pid.h"
/**
 * @brief PWM配置
 *
 */
#define PWM_PIN 1
#define PWM_DEF_DUTY 50
#define PWM_DEF_FREQ 5000

#define NTC_PIN 5
#define NTC_ADC_CHANNEL 4
/**
 * @brief PID配置
 *
 */
#define CAR_TEMPTRATURE_TARGET 65
#define CAR_TEMPTRATURE_KP 10.0f
#define CAR_TEMPTRATURE_KD 25.0f
#define CAR_TEMPTRATURE_KI 10.0f

#endif // CONFIG_H