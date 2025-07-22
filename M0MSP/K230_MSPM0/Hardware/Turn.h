#ifndef __TURN_H__
#define __TURN_H__

#include "ti_msp_dl_config.h"
#include "board.h"

typedef struct 
{
	float Kp;
	float Ki;
	float Kd;
	float integral;   // 积分项
	float prev_error; //上一次误差
	float target;  //目标角度
}Turn_PID;

#define turn    1

extern Turn_PID Turn_Controller;
void Turn_Init(void);
void Set_Target_Angle(float target);
float Turn_Compute(float current_angle);
#endif