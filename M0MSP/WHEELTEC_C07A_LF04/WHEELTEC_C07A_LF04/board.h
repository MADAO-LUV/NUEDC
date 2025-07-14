#ifndef	__BOARD_H__
#define __BOARD_H__
#include "ti_msp_dl_config.h"
#include "stdio.h"



#define ABS(a)      (a>0 ? a:(-a))
extern int32_t Get_Encoder_countA,Get_Encoder_countB;


//Systick������ֵ,24λ
#define SysTickMAX_COUNT 0xFFFFFF

//Systick����Ƶ��
#define SysTickFre 80000000

//��systick�ļ���ֵת��Ϊ�����ʱ�䵥λ
#define SysTick_MS(x)  ((SysTickFre/1000U)*(uint32_t)(x))
#define SysTick_US(x)  ((SysTickFre/1000000U)*(uint32_t)(x))

uint32_t Systick_getTick(void);
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);
void delay_1us(unsigned long __us);
void delay_1ms(unsigned long ms);

void uart0_send_char(char ch);
void uart0_send_string(char* str);



#endif
