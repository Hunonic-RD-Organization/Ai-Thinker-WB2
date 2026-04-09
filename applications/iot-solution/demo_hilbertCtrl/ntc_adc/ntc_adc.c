/**
 * @file ntc_adc.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-06-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "ntc_adc.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <math.h>
#include <hosal_adc.h>

#include <blog.h>

static hosal_adc_dev_t adc0;
static int NTC_ADC_CHANNEL = 0;

static ntc_heat_res_t ntc_data_table[] = {

	{20, 12.351, 12.504, 12.657},
	{21, 11.811, 11.951, 12.092},
	{22, 11.296, 11.426, 11.555},
	{23, 10.808, 10.926, 11.045},
	{24, 10.342, 10.452, 10.561},
	{25, 9.900, 10.000, 10.100},
	{26, 9.537, 9.660, 9.784},
	{27, 9.223, 9.328, 9.434},
	{28, 8.948, 9.055, 9.163},
	{29, 8.698, 8.797, 8.897},
	{30, 7.952, 8.049, 8.147},
	{31, 7.542, 7.624, 7.707},
	{32, 7.191, 7.268, 7.346},
	{33, 6.890, 6.967, 7.044},
	{34, 6.627, 6.703, 6.781},
	{35, 6.427, 6.520, 6.613},
	{36, 6.277, 6.419, 6.562},
	{37, 6.171, 6.314, 6.457},
	{38, 6.096, 6.241, 6.385},
	{39, 6.058, 6.194, 6.338},
	{40, 5.226, 5.313, 5.400},
	{41, 4.940, 5.021, 5.102},
	{42, 4.683, 4.761, 4.840},
	{43, 4.459, 4.536, 4.614},
	{44, 4.261, 4.337, 4.415},
	{45, 4.274, 4.354, 4.434},
	{46, 4.012, 4.087, 4.163},
	{47, 3.777, 3.847, 3.918},
	{48, 3.557, 3.624, 3.691},
	{49, 3.352, 3.416, 3.481},
	{50, 3.516, 3.588, 3.661},
	{51, 3.298, 3.367, 3.437},
	{52, 3.098, 3.167, 3.237},
	{53, 2.915, 2.980, 3.046},
	{54, 2.746, 2.810, 2.875},
	{55, 2.908, 2.973, 3.039},
	{56, 2.736, 2.797, 2.858},
	{57, 2.582, 2.639, 2.698},
	{58, 2.439, 2.495, 2.552},
	{59, 2.309, 2.364, 2.419},
	{60, 2.417, 2.476, 2.535},
	{61, 2.286, 2.343, 2.401},
	{62, 2.165, 2.222, 2.280},
	{63, 2.051, 2.108, 2.165},
	{64, 1.943, 1.998, 2.053},
	{65, 2.019, 2.072, 2.126},
	{66, 1.906, 1.957, 2.008},
	{67, 1.805, 1.853, 1.901},
	{68, 1.710, 1.756, 1.802},
	{69, 1.620, 1.664, 1.710},
	{70, 1.695, 1.742, 1.790},

};
/**
 * @brief initialize ntc adc
 *
 * @param ntc_pin
 */
void ntc_adc_init(int ntc_pin, int channel)
{
	adc0.cb = NULL;
	adc0.config.mode = HOSAL_ADC_ONE_SHOT;
	adc0.config.pin = ntc_pin;
	adc0.config.sampling_freq = 340;
	adc0.dma_chan = 0;
	adc0.p_arg = NULL;
	adc0.port = 0;
	NTC_ADC_CHANNEL = channel;
	int ret = hosal_adc_init(&adc0);
	if (ret != 0)
	{
		blog_error("ntc adc init failed, error code: %d", ret);
		return;
	}
	blog_info("ntc adc init success");
	ret = hosal_adc_add_channel(&adc0, NTC_ADC_CHANNEL);
	if (ret != 0)
	{
		blog_error("ntc adc add channel failed, error coed: %d", ret);
		return;
	}
	blog_info("ntc adc add channel success");
}

/**
 * @brief get adc value
 *
 * @return int
 */
static int ntc_adc_get_value(void)
{
	int ret = 0;
	uint32_t adc_value = 0;
	ret = hosal_adc_value_get(&adc0, NTC_ADC_CHANNEL, 10);
	if (ret < 0)
	{
		blog_error("ntc adc start failed, error code: %d", ret);
		return -1;
	}

	return ret;
}
/**
 * @brief get temperature
 *
 * @return float
 */
int ntc_adc_get_temperature(void)
{
	int adc_value = ntc_adc_get_value();
	if (adc_value == -1)
	{
		blog_error("ntc adc get value failed");
		return -1;
	}
	/**
	 * @brief 计算出NTC电阻值
	 *   adc_value = 3300*(Rt/(Rt+10k))
	 */
	// Rt/(Rt+10k) = adc_value/3300
	// float Rt_Rt_plus_10k = (float)(adc_value / 3300.0);
	// Rt = (Rt+10k) * Rt_Rt_plus_10k

	float resistance = (float)10000.0 * ((3300.0 - adc_value) / adc_value) / 1000;

	int closest_temp = ntc_data_table[0].temperature;
	float min_diff = fabs(resistance - ntc_data_table[0].res_min);

	for (int i = 0; i < sizeof(ntc_data_table) / sizeof(ntc_data_table[0]); i++)
	{
		if (resistance >= ntc_data_table[i].res_min && resistance <= ntc_data_table[i].res_max)
		{
			return ntc_data_table[i].temperature;
		}
		// 计算当前电阻与表项电阻的差值
		float diff_min = fabs(resistance - ntc_data_table[i].res_min);
		float diff_max = fabs(resistance - ntc_data_table[i].res_max);
		float current_diff = (diff_min < diff_max) ? diff_min : diff_max;

		// 更新最接近的值
		if (current_diff < min_diff)
		{
			min_diff = current_diff;
			closest_temp = ntc_data_table[i].temperature;
		}
	}
	// 如果没有找到完全匹配的范围，返回最接近的温度值
	blog_info("No exact match found, using closest value: %d °C", closest_temp);
	return closest_temp;
}