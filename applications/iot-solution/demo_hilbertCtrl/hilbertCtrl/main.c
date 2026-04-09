/*
 * @Author: xuhongv@yeah.net xuhongv@yeah.net
 * @Date: 2022-10-03 15:02:19
 * @LastEditors: xuhongv@yeah.net xuhongv@yeah.net
 * @LastEditTime: 2022-10-08 14:55:16
 * @FilePath: \bl_iot_sdk_for_aithinker\applications\get-started\helloworld\helloworld\main.c
 * @Description: Hello world
 */
#include <stdio.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>

#include <blog.h>
#include "bl_sys.h"
#include "config.h"

static int tempture = 0;
static int pwm_output = 700;
/**
 * @brief NTC温度读取任务
 *
 * @param arg
 */
void ntc_adc_read_task(void *arg)
{
    ntc_adc_init(NTC_PIN, NTC_ADC_CHANNEL);
    vTaskDelay(pdMS_TO_TICKS(100));
    while (1)
    {
        tempture = ntc_adc_get_temperature();
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
/**
 * @brief PID控制任务
 *
 * @param arg
 */
void pid_ctrl_task(void *arg)
{
    FuzzyPIDController controller;

    FuzzyPID_Init(&controller,
                  CAR_TEMPTRATURE_KP, CAR_TEMPTRATURE_KI, CAR_TEMPTRATURE_KD, // 基础PID参数
                  10.0f, 0.3f, 5.0f,                                          // PID参数调整范围
                  100.0f,                                                     // 积分上限
                  0, 1000);                                                   // PWM输出范围
    FuzzyPID_SetTarget(&controller, CAR_TEMPTRATURE_TARGET);
    while (1)
    {
        pwm_output = FuzzyPID_Calculate(&controller, tempture);
        pwm_dev_set_duty(pwm_output);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
void main(void)
{
    pwm_dev_init(PWM_PIN, PWM_DEF_FREQ, PWM_DEF_DUTY);
    // pwm_dev_set_duty(5000);
    pwm_dev_set_duty(pwm_output);
    xTaskCreate(ntc_adc_read_task, "ntc_adc_read_task", 1024, NULL, 10, NULL);
    xTaskCreate(pid_ctrl_task, "pid_ctrl_task", 1024, NULL, 9, NULL);
    while (1)
    {
        printf("PWM:%d,%d\n", pwm_output, tempture);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
