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
#include "board.h"
#include "stdio.h"
#include "key.h"
#include "motor.h"
int32_t PWMA,PWMB;
uint8_t flag;
float Position1=1500,Position2=1500,Target1=1500,Target2=1500; 
int main(void)
{
	SYSCFG_DL_init();
    DL_Timer_startCounter(PWM_0_INST);//开启PWM输出
	DL_Timer_startCounter(PWM_1_INST);//开启PWM输出
	NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);//清楚中断标志位
	NVIC_ClearPendingIRQ(UART_K230_INST_INT_IRQN);
	NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
//	//使能串口中断
	NVIC_EnableIRQ(UART_0_INST_INT_IRQN);//开启中断
	NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
	NVIC_EnableIRQ(UART_K230_INST_INT_IRQN);
	
			PWMA = Position_PID_1(Position1,Target1);
			PWMB = Position_PID_2(Position2,Target2);
	Set_PWM(PWMA,PWMB);
    while (1) 
    {
//		if(flag)
//		{
//					Draw_Circle();
//		}

//		printf("%f %f\n\r",Target1,Target2);
//		printf("%d %d\n\r",PWMA,PWMB);
					
    }
}


void TIMER_0_INST_IRQHandler(void)
{
	if(DL_TimerA_getPendingInterrupt(TIMER_0_INST))
	{
		if(DL_TIMER_IIDX_ZERO)
		{
			if(click())
			{
				flag != 1;

			}
//			Draw_Circle_Physical();

				draw_sine_wave();

		}
		       
	}
}











