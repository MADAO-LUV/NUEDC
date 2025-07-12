#ifndef __OLED_H
#define __OLED_H		

//ti_msp_dl_config.h����������
#include "ti_msp_dl_config.h"
#include "board.h"

/*
����board.h --- ��������һ��delay_ms()Ҳ����˵��ֲ��ʱ��
������Ҫȥ����һ��delay_ms() �� board.h����Ҫ����
*/

//#include "ti_msp_dl_config.h"
//#include "board.h"


//Oled port macro definition
//OLED�˿ں궨��
//#define OLED_RST_Clr() 	DL_GPIO_clearPins(OLED_RST_PORT,OLED_RST_PIN_25_PIN)// PDout(12)=0
//		  //RST
//#define OLED_RST_Set()  DL_GPIO_setPins(OLED_RST_PORT,OLED_RST_PIN_25_PIN) // PDout(12)=1 

//#define OLED_RS_Clr()  DL_GPIO_clearPins(OLED_DC_PORT,OLED_DC_PIN_26_PIN) //PDout(11)=0   //DC
//#define OLED_RS_Set()  DL_GPIO_setPins(OLED_DC_PORT,OLED_DC_PIN_26_PIN) //PDout(11)=1   //DC

//#define OLED_SCLK_Clr()  DL_GPIO_clearPins(OLED_SCL_PORT,OLED_SCL_PIN_28_PIN) //PDout(14)=0   //SCL
//#define OLED_SCLK_Set()  DL_GPIO_setPins(OLED_SCL_PORT,OLED_SCL_PIN_28_PIN) //PDout(14)=1   //SCL

//#define OLED_SDIN_Clr()  DL_GPIO_clearPins(OLED_SDA_PORT,OLED_SDA_PIN_31_PIN) //PDout(13)=0   //SDA
//#define OLED_SDIN_Set()  DL_GPIO_setPins(OLED_SDA_PORT,OLED_SDA_PIN_31_PIN) //PDout(13)=1   //SDA
#define OLED_CMD  0	//Command //д����
#define OLED_DATA 1	//Data //д����

//Oled control function
//OLED�����ú���
void OLED_WR_Byte(uint8_t dat,uint8_t cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);		   				   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode);
void OLED_ShowNumber(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
void OLED_ShowString(uint8_t x,uint8_t y,const uint8_t *p);
void OLED_RST_Clr(void);	  //RST
void OLED_RST_Set(void);
void OLED_RS_Clr(void) ; 
void OLED_RS_Set(void) ;
void OLED_SCLK_Clr(void) ;
void OLED_SCLK_Set(void);
void OLED_SDIN_Clr(void);
void OLED_SDIN_Set(void) ;
#define CNSizeWidth  16
#define CNSizeHeight 16
//extern char Hzk16[][16];
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no,uint8_t font_width,uint8_t font_height);	
void OLED_Set_Pos(unsigned char x, unsigned char y);
#endif  
	 
