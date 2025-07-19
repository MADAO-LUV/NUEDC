#include "motor.h"
float Velcity_Kp=1.0,  Velcity_Ki=0.4,  Velcity_Kd; //相关速度PID参数
/***********************************************
公司：轮趣科技（东莞）有限公司
品牌：WHEELTEC
官网：wheeltec.net
淘宝店铺：shop114407458.taobao.com 
速卖通: https://minibalance.aliexpress.com/store/4455017
版本：V1.0
修改时间：2024-07-019

Brand: WHEELTEC
Website: wheeltec.net
Taobao shop: shop114407458.taobao.com 
Aliexpress: https://minibalance.aliexpress.com/store/4455017
Version: V1.0
Update：2024-07-019

All rights reserved
***********************************************/
void Set_PWM(int pwmL,int pwmR)
{
	 if(pwmR>0)
    {
        DL_GPIO_setPins(AIN_PORT,AIN_AIN1_PIN);  // 1
        DL_GPIO_clearPins(AIN_PORT,AIN_AIN2_PIN); // 0
		DL_Timer_setCaptureCompareValue(PWM_0_INST,ABS(pwmR),GPIO_PWM_0_C0_IDX);
       
    }
    else
    {
         DL_GPIO_setPins(AIN_PORT,AIN_AIN2_PIN);
        DL_GPIO_clearPins(AIN_PORT,AIN_AIN1_PIN);
		DL_Timer_setCaptureCompareValue(PWM_0_INST,ABS(pwmR),GPIO_PWM_0_C0_IDX);
       
    }
    if(pwmL>0)
    {
		DL_GPIO_setPins(BIN_PORT,BIN_BIN1_PIN);
        DL_GPIO_clearPins(BIN_PORT,BIN_BIN2_PIN);
        DL_Timer_setCaptureCompareValue(PWM_0_INST,ABS(pwmL),GPIO_PWM_0_C1_IDX);
    }
    else
    {
		DL_GPIO_setPins(BIN_PORT,BIN_BIN2_PIN);
        DL_GPIO_clearPins(BIN_PORT,BIN_BIN1_PIN);
		
       
		 DL_Timer_setCaptureCompareValue(PWM_0_INST,ABS(pwmL),GPIO_PWM_0_C1_IDX);
    }
   

}

/***************************************************************************
函数功能：电机的PID闭环控制
入口参数：左右电机的编码器值
返回值  ：电机的PWM
***************************************************************************/
int Velocity_A(int TargetVelocity, int CurrentVelocity)
{  
    int Bias;  //定义相关变量
		static int ControlVelocityA, Last_biasA; //静态变量，函数调用结束后其值依然存在
		
		Bias=TargetVelocity-CurrentVelocity; //求速度偏差
		
		ControlVelocityA+=Velcity_Ki*(Bias-Last_biasA)+Velcity_Kp*Bias;  //增量式PI控制器
                                                                   //Velcity_Kp*(Bias-Last_bias) 作用为限制加速度
	                                                                 //Velcity_Ki*Bias             速度控制值由Bias不断积分得到 偏差越大加速度越大
		Last_biasA=Bias;	
	    if(ControlVelocityA>7000) ControlVelocityA=7000;
	    else if(ControlVelocityA<-7000) ControlVelocityA=-7000;
		return ControlVelocityA; //返回速度控制值
}

/***************************************************************************
函数功能：电机的PID闭环控制
入口参数：左右电机的编码器值
返回值  ：电机的PWM
***************************************************************************/
int Velocity_B(int TargetVelocity, int CurrentVelocity)
{  
    int Bias;  //定义相关变量
		static int ControlVelocityB, Last_biasB; //静态变量，函数调用结束后其值依然存在
		
		Bias=TargetVelocity-CurrentVelocity; //求速度偏差
		
		ControlVelocityB+=Velcity_Ki*(Bias-Last_biasB)+Velcity_Kp*Bias;  //增量式PI控制器
                                                                   //Velcity_Kp*(Bias-Last_bias) 作用为限制加速度
	                                                                 //Velcity_Ki*Bias             速度控制值由Bias不断积分得到 偏差越大加速度越大
		Last_biasB=Bias;	
	    if(ControlVelocityB>7000) ControlVelocityB=7000;
	    else if(ControlVelocityB<-7000) ControlVelocityB=-7000;
		return ControlVelocityB; //返回速度控制值
}



void Motor_Left(int pwmL)
{

    if(pwmL>0)
    {
		DL_GPIO_setPins(BIN_PORT,BIN_BIN1_PIN);
        DL_GPIO_clearPins(BIN_PORT,BIN_BIN2_PIN);
        DL_Timer_setCaptureCompareValue(PWM_0_INST,ABS(pwmL),GPIO_PWM_0_C1_IDX);
    }
    else
    {
		DL_GPIO_setPins(BIN_PORT,BIN_BIN2_PIN);
        DL_GPIO_clearPins(BIN_PORT,BIN_BIN1_PIN);
		
       
		 DL_Timer_setCaptureCompareValue(PWM_0_INST,ABS(pwmL),GPIO_PWM_0_C1_IDX);
    }

}



void Motor_Right(int pwmR)
{
	if(pwmR>0)
    {
        DL_GPIO_setPins(AIN_PORT,AIN_AIN1_PIN);  // 1
        DL_GPIO_clearPins(AIN_PORT,AIN_AIN2_PIN); // 0
		DL_Timer_setCaptureCompareValue(PWM_0_INST,ABS(pwmR),GPIO_PWM_0_C0_IDX);
       
    }
    else
    {
         DL_GPIO_setPins(AIN_PORT,AIN_AIN2_PIN);
        DL_GPIO_clearPins(AIN_PORT,AIN_AIN1_PIN);
		DL_Timer_setCaptureCompareValue(PWM_0_INST,ABS(pwmR),GPIO_PWM_0_C0_IDX);
       
    }
	
}
/***************************************************************************
函数功能：小车停止
***************************************************************************/
void Motor_Stop(void)
{
	Set_PWM(0,0);
}


/***************************************************************************
函数功能：小车前进
***************************************************************************/
void Motor_Gofarward(int speed,int currentL,int currentR)
{
	int pwml = Velocity_A(speed,currentL);
	int pwmr = Velocity_B(speed,currentR);
	Set_PWM(pwml,pwmr);
}


/***************************************************************************
函数功能：小车左转
***************************************************************************/
void Motor_Turn_Left(int speed,int currentL,int currentR)
{
	int pwml = Velocity_A(speed / 2,currentL);
	int pwmr = Velocity_B(speed,currentR);
	Set_PWM(pwml,pwmr);
}

/***************************************************************************
函数功能：小车右转
***************************************************************************/
void Motor_Turn_Right(int speed,int currentL,int currentR)
{
	int pwml = Velocity_A(speed,currentL);
	int pwmr = Velocity_B(speed / 2,currentR);
	Set_PWM(pwml,pwmr);
}

void Motor_Left_Sharp(int speed,int currentL,int currentR)
{
	int pwml = Velocity_A(speed,currentL);
	int pwmr = Velocity_B(speed + 5,currentR);
	Set_PWM(pwml,pwmr);
}


void Motor_Right_Sharp(int speed,int currentL,int currentR)
{
	int pwml = Velocity_A(speed + 5,currentL);
	int pwmr = Velocity_B(speed,currentR);
	Set_PWM(pwml,pwmr);
}

void Motor_Find(int speed,int currentL)
{
	int pwml = speed * 100;
	Set_PWM(pwml,0);
}

void Motor_adjust(int speedl,int speedr,int currentL,int currentR)
{
//	int pwml = Velocity_A(speedl,currentL);
//	int pwmr = Velocity_B(speedr,currentR);
	int pwml = speedl * 100;
	int pwmr = speedr * 100;
	Set_PWM(pwml,pwmr);
}