/**
 * @file ntc_heat_pid.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-06-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef NTC_HEAT_PID_H
#define NTC_HEAT_PID_H
// 模糊PID控制器结构体定义
typedef struct
{
	// PID参数
	float kp;
	float ki;
	float kd;

	// 模糊PID参数调整范围
	float kp_base;
	float ki_base;
	float kd_base;
	float kp_range;
	float ki_range;
	float kd_range;

	// 误差和误差变化率
	float error;
	float last_error;
	float prev_error;

	// 积分项
	float integral;
	float integral_max;

	// 输出限幅
	int output_min;
	int output_max;

	// 目标温度
	int target_temp;

	// 模糊控制相关参数
	float error_membership[7];	// 误差隶属度
	float derror_membership[7]; // 误差变化率隶属度
	float delta_kp[49];			// 模糊规则输出
	float delta_ki[49];
	float delta_kd[49];
} FuzzyPIDController;

// 模糊PID控制器初始化
void FuzzyPID_Init(FuzzyPIDController *controller,
				   float kp_base, float ki_base, float kd_base,
				   float kp_range, float ki_range, float kd_range,
				   float integral_max, int output_min, int output_max);

// 设置目标温度
void FuzzyPID_SetTarget(FuzzyPIDController *controller, int target_temp);

// 计算模糊PID控制输出
int FuzzyPID_Calculate(FuzzyPIDController *controller, int current_temp);

// 计算误差隶属度
void CalculateErrorMembership(FuzzyPIDController *controller, float error);

// 计算误差变化率隶属度
void CalculateDErrorMembership(FuzzyPIDController *controller, float derror);

// 模糊规则推理
void FuzzyInference(FuzzyPIDController *controller);

// 解模糊化
void Defuzzification(FuzzyPIDController *controller);
#endif // NTC_HEAT_PID_H