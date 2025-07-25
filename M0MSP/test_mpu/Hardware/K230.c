#include "K230.h"

uint16_t area;
K230_Recvmsg k230_recv; //定义接收数据的数据组
/*******
通信协议格式 0xFF ...  0xFE 
中间位 Cx， Cy 大小为 0 - 255
********/

/****
BCC校验
****/
static unsigned char calculateBCC(const unsigned char *data, uint16_t length) {
    unsigned char bcc = 0;
	uint16_t i =0;
    for (i = 0; i < length; i++) {
        bcc ^= data[i];
    }
    return bcc;
}



static uint8_t K230_data_callback(uint8_t recv)
{
	static uint8_t K230_FRAME_LEN = sizeof(K230_Recvmsg);
	static uint8_t recv_data[sizeof(K230_Recvmsg)];
	static uint8_t recv_counts = 0;
	uint8_t data_ready = 0;

	recv_data[recv_counts] = recv;

	if (recv == FRAME_HEAD || recv_counts > 0)
		recv_counts++;
	else
		recv_counts = 0;

	if (recv_counts == K230_FRAME_LEN)
	{
		recv_counts = 0;
		if (recv_data[K230_FRAME_LEN - 1] == FRAME_TAIL)
		{
			if (recv_data[K230_FRAME_LEN - 2] == calculateBCC(recv_data, K230_FRAME_LEN-2))  // 仅对 xH, xL, yH, yL 计算 BCC
			{
				k230_recv.Head = recv_data[0];
				k230_recv.follow_x = (recv_data[1] << 8) | recv_data[2];
				k230_recv.follow_y = (recv_data[3] << 8) | recv_data[4];
				k230_recv.BCCcheck = recv_data[5];
				k230_recv.End = recv_data[6];
				data_ready = 1;
			}
		}
	}

	return data_ready;
}




void UART_1_INST_IRQHandler(void)
{
	uint8_t ReadyLine = 0;
	uint8_t ReadyARcode = 0;
	uint8_t recv;
	
	switch(DL_UART_getPendingInterrupt(UART1))
	{
		case DL_UART_IIDX_RX:  //如果是接收中断
			// 接收发送过来的数据保存
			recv = DL_UART_Main_receiveData(UART1);
			
			//K230 巡线数据解包
			ReadyLine = K230_data_callback(recv);
		
			if(ReadyLine)
			{
				LED_Flash(10);
			}
			break;
		default:
			break;
	}
	
	
}

