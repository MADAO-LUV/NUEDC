#include "IR_Module.h"
// 引脚状态变量
uint32_t ir_dh1_state, ir_dh2_state, ir_dh3_state, ir_dh4_state;



void IR_Module_Read(void)
{
	 // 读取四个引脚的状态
     ir_dh4_state = DL_GPIO_readPins(IR_DH4_PORT, IR_DH4_PIN_17_PIN);
     ir_dh3_state = DL_GPIO_readPins(IR_DH3_PORT, IR_DH3_PIN_16_PIN);
     ir_dh2_state = DL_GPIO_readPins(IR_DH2_PORT, IR_DH2_PIN_12_PIN);
     ir_dh1_state = DL_GPIO_readPins(IR_DH1_PORT, IR_DH1_PIN_27_PIN);
     
     // 打印引脚状态 (转换为0或1)
     printf("IR_DH1: %d, IR_DH2: %d, IR_DH3: %d, IR_DH4: %d\r\n", 
            ir_dh1_state ? 1 : 0,
            ir_dh2_state ? 1 : 0, 
            ir_dh3_state ? 1 : 0,
            ir_dh4_state ? 1 : 0);
}






