#ifndef __WENT_H__
#define __WENT_H__

#include "ti_msp_dl_config.h"
#include "board.h"

typedef struct
{
	float Kp;
	float Ki;
	float Kd;
}Yaw_PID;

extern Yaw_PID Angle_Yaw;
extern float yaw_drift;
float Yaw_Compute(float target,float current);
void Calibrate_MPU6050(void);
#endif