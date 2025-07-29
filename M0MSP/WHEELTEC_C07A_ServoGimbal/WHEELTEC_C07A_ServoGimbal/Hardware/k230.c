#include "k230.h"

Kmessage k230_mes;
uint8_t laser_recv_buf[LASER_RECV_LEN];
uint8_t laser_recv_cnt = 0;
uint8_t laser_data_ready = 0;

void Process_laser_recv_byte(uint8_t byte)
{
	if(laser_recv_cnt == 0 && byte != 0xFF)return;
	
	laser_recv_buf[laser_recv_cnt++] = byte;
	
	if(laser_recv_cnt == LASER_RECV_LEN)
	{
		laser_recv_cnt = 0;
		
		if(laser_recv_buf[5] != 0xFE)return;
		
		// 解包
		k230_mes.head = laser_recv_buf[0];
		k230_mes.x   = (laser_recv_buf[1] << 8) | laser_recv_buf[2];
		k230_mes.y   = (laser_recv_buf[3] << 8) | laser_recv_buf[4];
		k230_mes.end = laser_recv_buf[5];

		laser_data_ready = 1; //数据标志位置
	}
	
}


void UART_2_INST_IRQHandler(void)
{
	switch(DL_UART_getPendingInterrupt(UART_K230_INST ))
	{
		case DL_UART_IIDX_RX:
		{
			uint8_t recv = DL_UART_Main_receiveData(UART_K230_INST);
			Process_laser_recv_byte(recv);
			break;
		}
		default:
			break;
	}
	
}