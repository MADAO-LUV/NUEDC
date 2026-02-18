#ifndef __K230_H__
#define __K230_H__

#include "ti_msp_dl_config.h"
#include "board.h"

typedef struct
{
	uint8_t head; // 0xFF
	uint16_t x;
	uint16_t y;
	uint8_t end; // 0xFE
}__attribute__((packed)) Kmessage;

#define  LASER_RECV_LEN 6



#endif