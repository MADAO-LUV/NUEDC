#include "IR_Moudle.h"
// ����״̬����
uint32_t ir_dh1_state, ir_dh2_state, ir_dh3_state, ir_dh4_state;



void IR_Module_Read(void)
{
	 // ��ȡ�ĸ����ŵ�״̬
     ir_dh4_state = IR_DH4_Read();
     ir_dh3_state = IR_DH3_Read();
     ir_dh2_state = IR_DH2_Read();
     ir_dh1_state = IR_DH1_Read();
     
     // ��ӡ����״̬ (ת��Ϊ0��1)
//     printf("IR_DH1: %d, IR_DH2: %d, IR_DH3: %d, IR_DH4: %d\r\n", 
//            ir_dh1_state ? 1 : 0,
//            ir_dh2_state ? 1 : 0, 
//            ir_dh3_state ? 1 : 0,
//            ir_dh4_state ? 1 : 0);
}






