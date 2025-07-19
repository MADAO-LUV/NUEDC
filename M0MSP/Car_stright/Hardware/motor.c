#include "motor.h"
float Velcity_Kp=1.0,  Velcity_Ki=0.4,  Velcity_Kd; //����ٶ�PID����
/***********************************************
��˾����Ȥ�Ƽ�����ݸ�����޹�˾
Ʒ�ƣ�WHEELTEC
������wheeltec.net
�Ա����̣�shop114407458.taobao.com 
����ͨ: https://minibalance.aliexpress.com/store/4455017
�汾��V1.0
�޸�ʱ�䣺2024-07-019

Brand: WHEELTEC
Website: wheeltec.net
Taobao shop: shop114407458.taobao.com 
Aliexpress: https://minibalance.aliexpress.com/store/4455017
Version: V1.0
Update��2024-07-019

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
�������ܣ������PID�ջ�����
��ڲ��������ҵ���ı�����ֵ
����ֵ  �������PWM
***************************************************************************/
int Velocity_A(int TargetVelocity, int CurrentVelocity)
{  
    int Bias;  //������ر���
		static int ControlVelocityA, Last_biasA; //��̬�������������ý�������ֵ��Ȼ����
		
		Bias=TargetVelocity-CurrentVelocity; //���ٶ�ƫ��
		
		ControlVelocityA+=Velcity_Ki*(Bias-Last_biasA)+Velcity_Kp*Bias;  //����ʽPI������
                                                                   //Velcity_Kp*(Bias-Last_bias) ����Ϊ���Ƽ��ٶ�
	                                                                 //Velcity_Ki*Bias             �ٶȿ���ֵ��Bias���ϻ��ֵõ� ƫ��Խ����ٶ�Խ��
		Last_biasA=Bias;	
	    if(ControlVelocityA>7000) ControlVelocityA=7000;
	    else if(ControlVelocityA<-7000) ControlVelocityA=-7000;
		return ControlVelocityA; //�����ٶȿ���ֵ
}

/***************************************************************************
�������ܣ������PID�ջ�����
��ڲ��������ҵ���ı�����ֵ
����ֵ  �������PWM
***************************************************************************/
int Velocity_B(int TargetVelocity, int CurrentVelocity)
{  
    int Bias;  //������ر���
		static int ControlVelocityB, Last_biasB; //��̬�������������ý�������ֵ��Ȼ����
		
		Bias=TargetVelocity-CurrentVelocity; //���ٶ�ƫ��
		
		ControlVelocityB+=Velcity_Ki*(Bias-Last_biasB)+Velcity_Kp*Bias;  //����ʽPI������
                                                                   //Velcity_Kp*(Bias-Last_bias) ����Ϊ���Ƽ��ٶ�
	                                                                 //Velcity_Ki*Bias             �ٶȿ���ֵ��Bias���ϻ��ֵõ� ƫ��Խ����ٶ�Խ��
		Last_biasB=Bias;	
	    if(ControlVelocityB>7000) ControlVelocityB=7000;
	    else if(ControlVelocityB<-7000) ControlVelocityB=-7000;
		return ControlVelocityB; //�����ٶȿ���ֵ
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
�������ܣ�С��ֹͣ
***************************************************************************/
void Motor_Stop(void)
{
	Set_PWM(0,0);
}


/***************************************************************************
�������ܣ�С��ǰ��
***************************************************************************/
void Motor_Gofarward(int speed,int currentL,int currentR)
{
	int pwml = Velocity_A(speed,currentL);
	int pwmr = Velocity_B(speed,currentR);
	Set_PWM(pwml,pwmr);
}


/***************************************************************************
�������ܣ�С����ת
***************************************************************************/
void Motor_Turn_Left(int speed,int currentL,int currentR)
{
	int pwml = Velocity_A(speed / 2,currentL);
	int pwmr = Velocity_B(speed,currentR);
	Set_PWM(pwml,pwmr);
}

/***************************************************************************
�������ܣ�С����ת
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