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
//		printf("{B%f:%f:%f}$",Pitch,Roll,Yaw);
//		printf("P:%.2f,R:%.2f,Y:%.2f\r\n",Pitch,Roll,Yaw);
		printf("%d  %d\n\r",encoderA_cnt,encoderB_cnt);
		OLED_ShowString(2,2,(uint8_t *)"fuck you");
		OLED_Refresh_Gram();

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
				PWMA = -Velocity_A(7000,encoderA_cnt);//PID����   0 --- 60 pwm����
				PWMB = -Velocity_B(10,encoderB_cnt);//PID����
				Set_PWM(PWMA,PWMB);//PWM���������
			}else Set_PWM(0,0);//�رյ��
			
		}
    }
}

