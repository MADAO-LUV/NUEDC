#ifndef _KEY_H
#define _KEY_H
#include "ti_msp_dl_config.h"
#include "board.h"
#define KEY  DL_GPIO_readPins(KEY_PORT,KEY_key_PIN)
uint8_t click_N_Double (uint8_t time);  //��������ɨ���˫������ɨ��
uint8_t click(void);               //��������ɨ��
uint8_t Long_Press(void);           //����ɨ��
void Key(void);
#endif
