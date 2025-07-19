#include "Went.h"
float yaw_drift = 0.0f; //������Ư����
Yaw_PID Angle_Yaw = {56.0f,0.0f,0.6f};
/*******
��������Ҫʹ�õ���ֱ��ɶ��

Ŀ��:�Ƕ� -- �ǶȻ� -- Ŀ���ٶ� --- �ٶȻ� --- �����ٶ�

*******/
float Yaw_Compute(float target,float current)
{
	const float MAX_DELTA = 2.0f; // ��/�������ڣ�ÿ�μ�����������仯ֵ��
	static float last_bais,last_value;
	float bais;
	static float D,I; //΢����
	/*����Ƕȱ仯������*/
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
	/*����Ҫ�������޷���*/
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