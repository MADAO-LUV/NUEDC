#include "Went.h"
float yaw_drift = 0.0f; //进行零漂补偿
Yaw_PID Angle_Yaw = {56.0f,0.0f,0.6f};
/*******
这里存放我要使用的走直线啥的

目标:角度 -- 角度环 -- 目标速度 --- 速度环 --- 最终速度

*******/
float Yaw_Compute(float target,float current)
{
	const float MAX_DELTA = 2.0f; // 度/控制周期（每次计算允许的最大变化值）
	static float last_bais,last_value;
	float bais;
	static float D,I; //微分量
	/*引入角度变化率限制*/
	float delta = current - last_value;
	
	if(fabs(delta) > MAX_DELTA)
	{
		delta = (delta > 0) ? MAX_DELTA : -MAX_DELTA;
	}
	last_value = current;
	current = last_value + delta;
	/**************/
	bais = target - current;
	I += bais;
	/*可能要做积分限幅度*/
	if(I >500) I = 500;
	else if(I < -500) I = -500; 
	D = bais - last_bais;
	last_bais = bais;

	return Angle_Yaw.Kp * bais + Angle_Yaw.Ki * I + Angle_Yaw.Kd * D;
	
}

void Calibrate_MPU6050(void)
{
	for(int i = 0; i < 100 ;i ++)
	{
		Read_DMP();
		yaw_drift += Yaw;
		delay_1ms(10);
	}
	yaw_drift /= 100;
}