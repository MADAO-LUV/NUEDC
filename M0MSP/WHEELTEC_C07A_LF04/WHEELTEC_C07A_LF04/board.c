#include "board.h"
#include "stdio.h"

#define RE_0_BUFF_LEN_MAX	128

volatile uint8_t  recv0_buff[RE_0_BUFF_LEN_MAX] = {0};
volatile uint16_t recv0_length = 0;
volatile uint8_t  recv0_flag = 0;



//����SysTick����ֵ
uint32_t Systick_getTick(void)
{
	return (SysTick->VAL);
}


//ms�����ӳ�
void delay_ms(uint32_t ms)
{
	//���������������ӳ�
	//if( ms > SysTickMAX_COUNT/(SysTickFre/1000) ) ms = SysTickMAX_COUNT/(SysTickFre/1000);
	for(int i=0;i<1000;i++)
	{
		delay_us(ms);
	}
}


void delay_us(uint32_t us)
{
	if( us > SysTickMAX_COUNT/(SysTickFre/1000000) ) us = SysTickMAX_COUNT/(SysTickFre/1000000);
	
	us = us*(SysTickFre/1000000); //��λת��
	
	//���ڱ������߹���ʱ��
	uint32_t runningtime = 0;
	
	//��õ�ǰʱ�̵ļ���ֵ
	uint32_t InserTick = Systick_getTick();
	
	//����ˢ��ʵʱʱ��
	uint32_t tick = 0;
	
	uint8_t countflag = 0;
	//�ȴ��ӳ�
	while(1)
	{
		tick = Systick_getTick();//ˢ�µ�ǰʱ�̼���ֵ
		
		if( tick > InserTick ) countflag = 1;//���������ѯ,���л���ʱ�ļ��㷽ʽ
		
		if( countflag ) runningtime = InserTick + SysTickMAX_COUNT - tick;
		else runningtime = InserTick - tick;
		
		if( runningtime>=us ) break;
	}

}

void delay_1us(unsigned long __us){ delay_us(__us); }
void delay_1ms(unsigned long ms){ delay_ms(ms); }

//���ڷ��͵����ַ�
void uart0_send_char(char ch)
{
	//������0æ��ʱ��ȴ�����æ��ʱ���ٷ��ʹ��������ַ�
	while( DL_UART_isBusy(UART_0_INST) == true );
	//���͵����ַ�
	DL_UART_Main_transmitData(UART_0_INST, ch);

}
//���ڷ����ַ���
void uart0_send_string(char* str)
{
	//��ǰ�ַ�����ַ���ڽ�β ���� �ַ����׵�ַ��Ϊ��
	while(*str!=0&&str!=0)
	{
		//�����ַ����׵�ַ�е��ַ��������ڷ������֮���׵�ַ����
		uart0_send_char(*str++);
	}
}


#if !defined(__MICROLIB)
//��ʹ��΢��Ļ�����Ҫ�������ĺ���
#if (__ARMCLIB_VERSION <= 6000000)
//�����������AC5  �Ͷ�����������ṹ��
struct __FILE
{
	int handle;
};
#endif

FILE __stdout;

//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
	x = x;
}
#endif


//printf�����ض���
int fputc(int ch, FILE *stream)
{
	//������0æ��ʱ��ȴ�����æ��ʱ���ٷ��ʹ��������ַ�
	while( DL_UART_isBusy(UART_0_INST) == true );
	
	DL_UART_Main_transmitData(UART_0_INST, ch);
	
	return ch;
}

//���ڵ��жϷ�����
void UART_0_INST_IRQHandler(void)
{
	uint8_t receivedData = 0;
	
	//��������˴����ж�
	switch( DL_UART_getPendingInterrupt(UART_0_INST) )
	{
		case DL_UART_IIDX_RX://����ǽ����ж�
			
			// ���շ��͹��������ݱ���
			receivedData = DL_UART_Main_receiveData(UART_0_INST);

			// ��黺�����Ƿ�����
			if (recv0_length < RE_0_BUFF_LEN_MAX - 1)
			{
				recv0_buff[recv0_length++] = receivedData;

				// ������������ٷ��ͳ�ȥ������ش�����ע�͵�
				uart0_send_char(receivedData);
			}
			else
			{
				recv0_length = 0;
			}

			// ��ǽ��ձ�־
			recv0_flag = 1;
		
			break;
		
		default://�����Ĵ����ж�
			break;
	}
}
