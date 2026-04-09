/**
 * @file ntc_adc.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-06-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef NTC_ADC_H
#define NTC_ADC_H

typedef struct
{
	int temperature;
	float res_min;
	float res_mid;
	float res_max;
} ntc_heat_res_t;

void ntc_adc_init(int ntc_pin, int adc_channel);
int ntc_adc_get_temperature(void);
#endif // NTC_ADC_H