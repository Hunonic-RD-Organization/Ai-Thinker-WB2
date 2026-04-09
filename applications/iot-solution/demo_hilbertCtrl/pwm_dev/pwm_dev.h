/**
 * @file pwm_dev.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-06-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef PWM_DEV_H
#define PWM_DEV_H
void pwm_dev_init(int pin, int freq, int duty);
void pwm_dev_set_duty(int duty);
void pwm_dev_close(void);
#endif