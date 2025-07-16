#ifndef _MOTOR_H
#define _MOTOR_H
#include "ti_msp_dl_config.h"
#include "board.h"
extern float Velcity_Kp,Velcity_Ki,Velcity_Kd;
int Velocity_A(int TargetVelocity, int CurrentVelocity);
int Velocity_B(int TargetVelocity, int CurrentVelocity);
void Set_PWM(int pwma,int pwmb);
void Motor_Left(int pwmL);
void Motor_Right(int pwmR);
void Motor_Stop(void);
void Motor_Gofarward(int speed,int currentL,int currentR);
void Motor_Turn_Left(int speed,int currentL,int currentR);
void Motor_Turn_Right(int speed,int currentL,int currentR);
void Motor_Left_Sharp(int speed,int currentL,int currentR);
void Motor_Right_Sharp(int speed,int currentL,int currentR);
void Motor_Find(int speed,int currentL);
void Motor_adjust(int speedl,int speedr,int currentL,int currentR); 

#endif