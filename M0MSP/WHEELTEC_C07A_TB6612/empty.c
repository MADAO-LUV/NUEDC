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
int baes_speed = 10; // 基础速度
float Kp = 2,Kd = 0.02;
void Car_Control(void);

void Menu(void); // 用于菜单选择
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
    delay_1ms(15000);
    while (1) 
    {
		//串口1打印编码器数据
		Read_DMP();
		Menu();
    }
}


void Car_IR_PID(void)
{
	 ir_dh4_state = IR_DH4_Read();
     ir_dh3_state = IR_DH3_Read();
     ir_dh2_state = IR_DH2_Read();
     ir_dh1_state = IR_DH1_Read();
	
	static int numerator,denominator;
	
	 int IR_State = (ir_dh1_state << 3) | (ir_dh2_state << 2) | (ir_dh3_state << 1) | ir_dh4_state;
	 numerator = ir_dh1_state * -3 + ir_dh2_state * -1 + ir_dh3_state * 1 + ir_dh4_state * 3;
	 denominator = ir_dh1_state + ir_dh2_state + ir_dh3_state + ir_dh4_state;
	 int temp = numerator / denominator;
	int temp_speed = temp * Kp;
	switch(IR_State)
	{
		case 0b0000:
			Motor_Find(baes_speed,encoderA_cnt);
//		Motor_Stop();
			break;
		
		case 0b0100:
			Motor_adjust(baes_speed+temp_speed,baes_speed,encoderA_cnt,encoderB_cnt);
			break;
		
		case 0b1000:
			Motor_adjust(baes_speed+temp_speed,baes_speed,encoderA_cnt,encoderB_cnt);
			break;
		
		case 0b0010:
			Motor_adjust(baes_speed+temp_speed,baes_speed,encoderA_cnt,encoderB_cnt);
		break;
		
		case 0b0001:
			Motor_adjust(baes_speed+temp_speed,baes_speed,encoderA_cnt,encoderB_cnt);
		break;
		
		case 0b0110:
			Motor_Gofarward(baes_speed,encoderA_cnt,encoderB_cnt);
		break;

		
		case 0b1100:
			Motor_adjust(baes_speed+temp_speed,baes_speed,encoderA_cnt,encoderB_cnt);
		break;
		
		case 0b0011:
			Motor_adjust(baes_speed+temp_speed,baes_speed,encoderA_cnt,encoderB_cnt);
		break;
	}
	
}


void Car_Control(void)
{
	 float diff_speed;
	
	diff_speed = Yaw_Compute(0.0f,Yaw);
	
	int pwml = baes_speed * 100 - diff_speed;
	int pwmr = baes_speed * 100 + diff_speed;

//	int speedl = Velocity_A(pwml,encoderA_cnt);
//	int speedr = Velocity_B(pwmr,encoderB_cnt);
	Set_PWM(pwml,pwmr);
}	




//10ms定时中断   
void TIMER_0_INST_IRQHandler(void)
{
    if(DL_TimerA_getPendingInterrupt(TIMER_0_INST))
    {
        if(DL_TIMER_IIDX_ZERO)
        {
			LED_Flash(100);//led闪烁
			Key();//获取当前BLS按键状态

			encoderA_cnt = -Get_Encoder_countA;//两个电机安装相反，其中一个编码器值需要相反
			encoderB_cnt =  Get_Encoder_countB;
			Get_Encoder_countA=Get_Encoder_countB=0;
		if(!Flag_Stop)//单击BLS开启或关闭电机
		{
//				Car_IR_PID();
				Car_Control();
		}else Set_PWM(0,0);//关闭电机
		}
    }
}






extern uint8_t tuning_direction;   // 1 为 + 则 0 为 -
extern TuneTarget current_param;

void Menu(void)
{
	OLED_Clear();
	switch(Now.Per)
	{
		case 0:
			sprintf(testS,"L:%d,R:%d",encoderA_cnt,encoderB_cnt);
			OLED_ShowString(0,0,(uint8_t *)testS);
			sprintf(testS,"Y:%.1f",Yaw);
			OLED_ShowString(70,0,(uint8_t *)testS);
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
		
		case 3:
			OLED_ShowString(0, 0, (uint8_t *)"Angle PID");
			
			sprintf(testS, "Kp:%.2f%s", Angle_Yaw.Kp, (current_param == TUNE_KP ? "<" : " "));
			OLED_ShowString(0, 16, (uint8_t *)testS);

			sprintf(testS, "Ki:%.2f%s", Angle_Yaw.Ki, (current_param == TUNE_KI ? "<" : " "));
			OLED_ShowString(0, 32, (uint8_t *)testS);

			sprintf(testS, "Kd:%.2f%s", Angle_Yaw.Kd, (current_param == TUNE_KD ? "<" : " "));
			OLED_ShowString(0, 48, (uint8_t *)testS);
		
			sprintf(testS, "D:%s", tuning_direction ? "+" : "-");
			OLED_ShowString(100, 0, (uint8_t *)testS);
		break;
	}
		OLED_Refresh_Gram();


}	

