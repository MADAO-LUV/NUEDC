/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
//#include "board.h"
#include "board.h"
#include "oled.h"
int32_t encoderA_cnt,PWMA,encoderB_cnt,PWMB;
int Flag_Stop=1;
char testS[40];
int baes_speed = 20; // �����ٶ�
int compute_error(void);
void Car_PD_Control(void);
void Menu(void); // ���ڲ˵�ѡ��
int main(void)
{
	int i=0;
    SYSCFG_DL_init();
	DL_Timer_startCounter(PWM_0_INST);
	NVIC_ClearPendingIRQ(ENCODERA_INT_IRQN);
    NVIC_ClearPendingIRQ(ENCODERB_INT_IRQN);
	NVIC_EnableIRQ(ENCODERA_INT_IRQN);
    NVIC_EnableIRQ(ENCODERB_INT_IRQN);
	NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
	NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
	OLED_Init();
	MPU6050_initialize();
	DMP_Init();
//	Yaw = Yaw+18;
//	delay_ms(15000);
    while (1) 
    {
		//����1��ӡ����������
		Read_DMP();
		IR_Module_Read();
		Menu();


    }
}

//10ms��ʱ�ж�   
void TIMER_0_INST_IRQHandler(void)
{
    if(DL_TimerA_getPendingInterrupt(TIMER_0_INST))
    {
        if(DL_TIMER_IIDX_ZERO)
        {
			LED_Flash(100);//led��˸
			Key();//��ȡ��ǰBLS����״̬
			encoderA_cnt = -Get_Encoder_countA;//���������װ�෴������һ��������ֵ��Ҫ�෴
			encoderB_cnt =  Get_Encoder_countB;
			Get_Encoder_countA=Get_Encoder_countB=0;
		if(!Flag_Stop)//����BLS������رյ��
		{
				Car_PD_Control();
		
		}else Set_PWM(0,0);//�رյ��
		}
    }
}

int compute_error(void)
{
	
	
	    // ��ת������ֵ������Ϊ1������Ϊ0
    int s1 = !ir_dh1_state;  // ����=1������=0
    int s2 = !ir_dh2_state;
    int s3 = !ir_dh3_state;
    int s4 = !ir_dh4_state;
	
	int numerator = s1 * (3)+ s2 * (1) + s3 * (-1) + s4 * (-3);
	int denominator = s1 + s2 + s3 + s4;
	if(denominator == 0)return 666; //��û�ж��� ֹͣ 
	return numerator / denominator;
}

float Kp = 2,Kd = 0.02;
void Car_PD_Control(void) {
    int error = compute_error();
	static int last_error;

    if (error == 666) {
        // ���ߴ����������С��ԭ����ת�����һ�
        Motor_Stop();
        return;
    }

    int derivative = error - last_error;
    last_error = error;

    int correction = (int)(Kp * error + Kd * derivative);

    // ���������� PWM
    int leftSpeed = baes_speed - correction;
    int rightSpeed = baes_speed + correction;

    // ������ 0~100 ��Χ
    if (leftSpeed > 35) leftSpeed = 35;
    if (leftSpeed < 0) leftSpeed = 0;
    if (rightSpeed > 35) rightSpeed = 35;
    if (rightSpeed < 0) rightSpeed = 0;

	PWMA = Velocity_A(leftSpeed,encoderA_cnt);//PID����   0 --- 60 pwm����
	PWMB = Velocity_B(rightSpeed,encoderB_cnt);//PID����
	
	Set_PWM(PWMA,PWMB);
}


extern uint8_t tuning_direction;   // 1 Ϊ + �� 0 Ϊ -
extern TuneTarget current_param;

void Menu(void)
{
	OLED_Clear();
	switch(Now.Per)
	{
		case 0:
			sprintf(testS,"L:%d,R:%d",encoderA_cnt,encoderB_cnt);
			OLED_ShowString(0,0,(uint8_t *)testS);
			sprintf(testS,"IR1:%d,IR2:%d",ir_dh1_state,ir_dh2_state);
			OLED_ShowString(0,16,(uint8_t *)testS);
			sprintf(testS,"IR3:%d,IR4:%d",ir_dh3_state,ir_dh4_state);
			OLED_ShowString(0,32,(uint8_t *)testS);
			sprintf(testS,"BaseSpeed:%d",baes_speed);
			OLED_ShowString(0,48,(uint8_t *)testS);
			sprintf(testS, "D:%s", tuning_direction ? "+" : "-");
			OLED_ShowString(100, 48, (uint8_t *)testS);
		break;
		
		case 1:
			OLED_ShowString(0,0,(uint8_t *)"IR PD");
			sprintf(testS,"KP:%.1f  %s",Kp,(current_param == TUNE_KP ? "<" : " "));
			OLED_ShowString(0,16,(uint8_t *)testS);
			sprintf(testS,"KD:%.2f  %s",Kd,(current_param == TUNE_KD ? "<" : " "));
			OLED_ShowString(0,32,(uint8_t *)testS);
			sprintf(testS, "D:%s", tuning_direction ? "+" : "-");
			OLED_ShowString(100, 0, (uint8_t *)testS);
		break;
		
		case 2:
			OLED_ShowString(0, 0, (uint8_t *)"Car PID");

			sprintf(testS, "Kp:%.2f%s", Velcity_Kp, (current_param == TUNE_KP ? "<" : " "));
			OLED_ShowString(0, 16, (uint8_t *)testS);

			sprintf(testS, "Ki:%.2f%s", Velcity_Ki, (current_param == TUNE_KI ? "<" : " "));
			OLED_ShowString(0, 32, (uint8_t *)testS);

			sprintf(testS, "Kd:%.2f%s", Velcity_Kd, (current_param == TUNE_KD ? "<" : " "));
			OLED_ShowString(0, 48, (uint8_t *)testS);
		
			sprintf(testS, "D:%s", tuning_direction ? "+" : "-");
			OLED_ShowString(100, 0, (uint8_t *)testS);
		break;
	}
		OLED_Refresh_Gram();


}	

