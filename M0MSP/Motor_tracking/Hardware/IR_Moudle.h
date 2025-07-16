#ifndef _IR_MODULE_H
#define _IR_MODULE_H
#include "ti_msp_dl_config.h"
#include "board.h"
extern uint32_t ir_dh1_state, ir_dh2_state, ir_dh3_state, ir_dh4_state;
// 红外引脚读取宏定义
#define IR_DH1_Read()   (DL_GPIO_readPins(ID_DH1_PORT, ID_DH1_PIN_27_PIN) ? 0 : 1)
#define IR_DH2_Read()   (DL_GPIO_readPins(ID_DH2_PORT, ID_DH2_PIN_12_PIN) ? 0 : 1)
#define IR_DH3_Read()   (DL_GPIO_readPins(ID_DH3_PORT, ID_DH3_PIN_16_PIN) ? 0 : 1)
#define IR_DH4_Read()   (DL_GPIO_readPins(ID_DH4_PORT, ID_DH4_PIN_17_PIN) ? 0 : 1)

// 红外之间的距离
#define SENSOR_DH1_POS    40.15f
#define SENSOR_DH2_POS    7.25f
#define SENSOR_DH3_POS    -7.25f
#define SENSOR_DH4_POS    -40.15f

void IR_Module_Read(void);
#endif
