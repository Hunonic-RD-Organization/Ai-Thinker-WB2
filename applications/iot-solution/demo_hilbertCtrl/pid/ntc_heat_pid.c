/**
 * @file ntc_heat_pid.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-06-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "stdlib.h"
#include "ntc_heat_pid.h"
#include "math.h"
#include "stdio.h"

// 模糊集合：负大、负中、负小、零、正小、正中、正大
#define NB -3
#define NM -2
#define NS -1
#define ZO 0
#define PS 1
#define PM 2
#define PB 3

// 模糊规则表
static const int kp_rules[7][7] = {
	{PB, PB, PM, PM, PS, ZO, ZO},
	{PB, PB, PM, PS, PS, ZO, NS},
	{PM, PM, PM, PS, ZO, NS, NS},
	{PM, PM, PS, ZO, NS, NM, NM},
	{PS, PS, ZO, NS, NS, NM, NM},
	{PS, ZO, NS, NM, NM, NM, NB},
	{ZO, ZO, NM, NM, NM, NB, NB}};

static const int ki_rules[7][7] = {
	{NB, NB, NM, NM, NS, ZO, ZO},
	{NB, NB, NM, NS, NS, ZO, ZO},
	{NB, NM, NS, NS, ZO, PS, PS},
	{NM, NM, NS, ZO, PS, PM, PM},
	{NM, NS, ZO, PS, PS, PM, PB},
	{ZO, ZO, PS, PS, PM, PB, PB},
	{ZO, ZO, PS, PM, PM, PB, PB}};

static const int kd_rules[7][7] = {
	{PS, NS, NB, NB, NB, NM, PS},
	{PS, NS, NB, NM, NM, NS, ZO},
	{ZO, NS, NM, NM, NS, NS, ZO},
	{ZO, NS, NS, NS, NS, NS, ZO},
	{ZO, ZO, ZO, ZO, ZO, ZO, ZO},
	{PB, NS, PS, PS, PS, PS, PB},
	{PB, PM, PM, PM, PS, PS, PB}};

// 初始化模糊PID控制器
void FuzzyPID_Init(FuzzyPIDController *controller,
				   float kp_base, float ki_base, float kd_base,
				   float kp_range, float ki_range, float kd_range,
				   float integral_max, int output_min, int output_max)
{
	// 初始化PID参数
	controller->kp = kp_base;
	controller->ki = ki_base;
	controller->kd = kd_base;

	// 设置PID参数调整范围
	controller->kp_base = kp_base;
	controller->ki_base = ki_base;
	controller->kd_base = kd_base;
	controller->kp_range = kp_range;
	controller->ki_range = ki_range;
	controller->kd_range = kd_range;

	// 初始化误差和积分项
	controller->error = 0.0f;
	controller->last_error = 0.0f;
	controller->prev_error = 0.0f;
	controller->integral = 0.0f;
	controller->integral_max = integral_max;

	// 设置输出限幅
	controller->output_min = output_min;
	controller->output_max = output_max;

	// 初始化目标温度
	controller->target_temp = 0.0f;

	// 初始化模糊控制相关参数
	for (int i = 0; i < 7; i++)
	{
		controller->error_membership[i] = 0.0f;
		controller->derror_membership[i] = 0.0f;
	}

	for (int i = 0; i < 49; i++)
	{
		controller->delta_kp[i] = 0.0f;
		controller->delta_ki[i] = 0.0f;
		controller->delta_kd[i] = 0.0f;
	}
}

// 设置目标温度
void FuzzyPID_SetTarget(FuzzyPIDController *controller, int target_temp)
{
	controller->target_temp = target_temp;

	// 重置误差和积分项
	controller->error = 0.0f;
	controller->last_error = 0.0f;
	controller->prev_error = 0.0f;
	controller->integral = 0.0f;
}

// 计算模糊PID控制输出
int FuzzyPID_Calculate(FuzzyPIDController *controller, int current_temp)
{
	float error, derror;
	float p_term, i_term, d_term;
	int output;

	// 计算当前误差
	error = controller->target_temp - current_temp;
	derror = error - controller->last_error;

	// 保存当前误差
	controller->prev_error = controller->last_error;
	controller->last_error = error;
	controller->error = error;

	// 模糊化处理
	CalculateErrorMembership(controller, error);
	CalculateDErrorMembership(controller, derror);

	// 模糊推理
	FuzzyInference(controller);

	// 解模糊化
	Defuzzification(controller);

	// 计算PID控制项
	p_term = controller->kp * error;

	controller->integral += error;

	// 积分限幅
	if (controller->integral > controller->integral_max)
		controller->integral = controller->integral_max;
	else if (controller->integral < -controller->integral_max)
		controller->integral = -controller->integral_max;

	i_term = controller->ki * controller->integral;
	d_term = controller->kd * (error - controller->last_error);

	// 计算总输出
	output = (int)(p_term + i_term + d_term);

	// 输出限幅
	if (output > controller->output_max)
		output = controller->output_max;
	else if (output < controller->output_min)
		output = controller->output_min;

	return output;
}

// 计算误差隶属度
void CalculateErrorMembership(FuzzyPIDController *controller, float error)
{
	// 输入变量范围映射和隶属度计算
	// 这里使用三角形隶属度函数
	float e = error / 10.0f; // 假设误差范围为±10度

	// NB
	if (e <= -2.5f)
		controller->error_membership[0] = 1.0f;
	else if (e < -1.5f)
		controller->error_membership[0] = (-e - 1.5f) / 1.0f;
	else
		controller->error_membership[0] = 0.0f;

	// NM
	if (e <= -2.5f || e >= -0.5f)
		controller->error_membership[1] = 0.0f;
	else if (e < -1.5f)
		controller->error_membership[1] = (e + 2.5f) / 1.0f;
	else
		controller->error_membership[1] = (-e - 0.5f) / 1.0f;

	// NS
	if (e <= -1.5f || e >= 0.5f)
		controller->error_membership[2] = 0.0f;
	else if (e < -0.5f)
		controller->error_membership[2] = (e + 1.5f) / 1.0f;
	else
		controller->error_membership[2] = (-e + 0.5f) / 1.0f;

	// ZO
	if (e <= -0.5f || e >= 0.5f)
		controller->error_membership[3] = 0.0f;
	else if (e < 0.0f)
		controller->error_membership[3] = (e + 0.5f) / 0.5f;
	else
		controller->error_membership[3] = (-e + 0.5f) / 0.5f;

	// PS
	if (e <= -0.5f || e >= 1.5f)
		controller->error_membership[4] = 0.0f;
	else if (e < 0.5f)
		controller->error_membership[4] = (e + 0.5f) / 1.0f;
	else
		controller->error_membership[4] = (-e + 1.5f) / 1.0f;

	// PM
	if (e <= 0.5f || e >= 2.5f)
		controller->error_membership[5] = 0.0f;
	else if (e < 1.5f)
		controller->error_membership[5] = (e - 0.5f) / 1.0f;
	else
		controller->error_membership[5] = (-e + 2.5f) / 1.0f;

	// PB
	if (e >= 2.5f)
		controller->error_membership[6] = 1.0f;
	else if (e > 1.5f)
		controller->error_membership[6] = (e - 1.5f) / 1.0f;
	else
		controller->error_membership[6] = 0.0f;
}

// 计算误差变化率隶属度
void CalculateDErrorMembership(FuzzyPIDController *controller, float derror)
{
	// 输入变量范围映射和隶属度计算
	// 这里使用三角形隶属度函数
	float de = derror / 2.0f; // 假设误差变化率范围为±2度/周期

	// NB
	if (de <= -2.5f)
		controller->derror_membership[0] = 1.0f;
	else if (de < -1.5f)
		controller->derror_membership[0] = (-de - 1.5f) / 1.0f;
	else
		controller->derror_membership[0] = 0.0f;

	// NM
	if (de <= -2.5f || de >= -0.5f)
		controller->derror_membership[1] = 0.0f;
	else if (de < -1.5f)
		controller->derror_membership[1] = (de + 2.5f) / 1.0f;
	else
		controller->derror_membership[1] = (-de - 0.5f) / 1.0f;

	// NS
	if (de <= -1.5f || de >= 0.5f)
		controller->derror_membership[2] = 0.0f;
	else if (de < -0.5f)
		controller->derror_membership[2] = (de + 1.5f) / 1.0f;
	else
		controller->derror_membership[2] = (-de + 0.5f) / 1.0f;

	// ZO
	if (de <= -0.5f || de >= 0.5f)
		controller->derror_membership[3] = 0.0f;
	else if (de < 0.0f)
		controller->derror_membership[3] = (de + 0.5f) / 0.5f;
	else
		controller->derror_membership[3] = (-de + 0.5f) / 0.5f;

	// PS
	if (de <= -0.5f || de >= 1.5f)
		controller->derror_membership[4] = 0.0f;
	else if (de < 0.5f)
		controller->derror_membership[4] = (de + 0.5f) / 1.0f;
	else
		controller->derror_membership[4] = (-de + 1.5f) / 1.0f;

	// PM
	if (de <= 0.5f || de >= 2.5f)
		controller->derror_membership[5] = 0.0f;
	else if (de < 1.5f)
		controller->derror_membership[5] = (de - 0.5f) / 1.0f;
	else
		controller->derror_membership[5] = (-de + 2.5f) / 1.0f;

	// PB
	if (de >= 2.5f)
		controller->derror_membership[6] = 1.0f;
	else if (de > 1.5f)
		controller->derror_membership[6] = (de - 1.5f) / 1.0f;
	else
		controller->derror_membership[6] = 0.0f;
}

// 模糊规则推理
void FuzzyInference(FuzzyPIDController *controller)
{
	int rule_index = 0;

	// 49条模糊规则
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			// 计算规则的激活强度
			float activation = controller->error_membership[i] * controller->derror_membership[j];

			// 根据规则表计算参数调整量
			controller->delta_kp[rule_index] = activation * kp_rules[i][j];
			controller->delta_ki[rule_index] = activation * ki_rules[i][j];
			controller->delta_kd[rule_index] = activation * kd_rules[i][j];

			rule_index++;
		}
	}
}

// 解模糊化
void Defuzzification(FuzzyPIDController *controller)
{
	float sum_kp = 0.0f, sum_ki = 0.0f, sum_kd = 0.0f;
	float weight_sum = 0.0f;

	// 计算加权平均
	for (int i = 0; i < 49; i++)
	{
		float weight = fabs(controller->delta_kp[i]) + fabs(controller->delta_ki[i]) + fabs(controller->delta_kd[i]);

		sum_kp += controller->delta_kp[i] * weight;
		sum_ki += controller->delta_ki[i] * weight;
		sum_kd += controller->delta_kd[i] * weight;

		weight_sum += weight;
	}

	// 防止除零错误
	if (weight_sum > 0.0f)
	{
		// 归一化并映射到参数调整范围
		controller->kp = controller->kp_base + (sum_kp / weight_sum) * controller->kp_range / 3.0f;
		controller->ki = controller->ki_base + (sum_ki / weight_sum) * controller->ki_range / 3.0f;
		controller->kd = controller->kd_base + (sum_kd / weight_sum) * controller->kd_range / 3.0f;
	}

	// 确保参数在合理范围内
	if (controller->kp < 0.0f)
		controller->kp = 0.0f;
	if (controller->ki < 0.0f)
		controller->ki = 0.0f;
	if (controller->kd < 0.0f)
		controller->kd = 0.0f;
}
