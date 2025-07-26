#include "Turn.h"

Turn_PID Turn_Controller = {0};

void Turn_Init(void)
{
    Turn_Controller.Kp = 1.0f;
    Turn_Controller.Ki = 0.01f;
    Turn_Controller.Kd = 0.4f;
    Turn_Controller.integral = 0.0f;
    Turn_Controller.prev_error = 0.0f;
	Turn_Controller.target = 0.0f;  //角度
}


void Set_Target_Angle(float target)
{
	// 角度归一化
	while(target > 180.0f)target -= 360.0f;
	while(target < -180.0f)target += 360.0f;
	Turn_Controller.target = target;
}


float Turn_Compute(float current_angle)
{
	float error = Turn_Controller.target - current_angle;
	if (error > 180.0f) error -= 360.0f;
    else if (error < -180.0f) error += 360.0f;
	
	    // === 死区判断 ===
    float deadzone = 1.0f;  // 死区范围（比如±2度）
    if (fabs(error) < deadzone)
    {
        Turn_Controller.integral = 0;     // 可选：误差小就清积分，避免累积
        return 0.0f;                      // 返回 0，不再输出控制量
    }
	
	// PID计算
	float P = Turn_Controller.Kp * error;
	
	    Turn_Controller.integral += error;
    // 积分限幅防止饱和
    if (Turn_Controller.integral > 100.0f) Turn_Controller.integral = 100.0f;
    else if (Turn_Controller.integral < -100.0f) Turn_Controller.integral = -100.0f;
    float I = Turn_Controller.Ki * Turn_Controller.integral;
    
    float D = Turn_Controller.Kd * (error - Turn_Controller.prev_error);
    Turn_Controller.prev_error = error;
    
    return P + I + D;
}