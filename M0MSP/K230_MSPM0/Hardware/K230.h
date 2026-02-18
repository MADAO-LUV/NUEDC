#ifndef __K230_H__
#define __K230_H__

#include "ti_msp_dl_config.h"
#include "board.h"

#define FRAME_HEAD		0XFF
#define FRAME_TAIL		0xFC

#pragma pack(1)
typedef struct
{
	uint8_t Head;         
	uint16_t follow_x;  // 2 bytes
	uint16_t follow_y;  // 2 bytes
	uint8_t BCCcheck;    
	uint8_t End;  	
}K230_Recvmsg;
#pragma pack()



extern K230_Recvmsg k230_recv;
extern uint16_t area;
#endif