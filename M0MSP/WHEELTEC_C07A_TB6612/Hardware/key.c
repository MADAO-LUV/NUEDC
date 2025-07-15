#include "key.h"

Key_V Now = {0,0};
/**************************************************************************
Function: Key scan
Input   : Double click the waiting time
Output  : 0��No action��1��click��2��Double click
�������ܣ�����ɨ��
��ڲ�����˫���ȴ�ʱ��
����  ֵ������״̬ 0���޶��� 1������ 2��˫��
**************************************************************************/
//u8 click_N_Double (u8 time)
//{
//    static  u8 flag_key,count_key,double_key=0;
//    static  u16 count_single,Forever_count;
//    if(DL_GPIO_readPins(KEY_PORT,KEY_key_PIN)>0)  Forever_count++;   //������־λδ��1
//    else        Forever_count=0;
//    if((DL_GPIO_readPins(KEY_PORT,KEY_key_PIN)>0)&&0==flag_key)     flag_key=1; //��һ�ΰ���
//    if(0==count_key)
//    {
//            if(flag_key==1)
//            {
//                double_key++;
//                count_key=1;            //��ǰ���һ��
//            }
//            if(double_key==3)
//            {                                       //��������
//                double_key=0;
//                count_single=0;
//                return 2;                   //˫��ִ�е�ָ��
//            }
//    }
//    if(0==DL_GPIO_readPins(KEY_PORT,KEY_key_PIN))          flag_key=0,count_key=0;
//    if(1==double_key)
//    {
//        count_single++;
//        if(count_single>time&&Forever_count<time)
//        {
//            double_key=0;
//            count_single=0; //��ʱ�����Ϊ˫��
//			return 1;//����ִ�е�ָ��
//        }
//        if(Forever_count>time)
//        {
//            double_key=0;
//            count_single=0;
//        }
//    }
//    return 0;
//}
u8 click_N_Double(u8 time_ms) {
    static enum {IDLE, SINGLE_PRESS, WAIT_SECOND} state = IDLE;
    static u16 timer = 0;
    u8 key_pressed = (DL_GPIO_readPins(KEY_PORT, KEY_key_PIN) > 0);

    switch (state) {
        case IDLE:
            if (key_pressed) {
                state = SINGLE_PRESS;
                timer = 0;
            }
            break;

        case SINGLE_PRESS:
            if (!key_pressed) {
                state = WAIT_SECOND;
                timer = 0;
            }
            break;

        case WAIT_SECOND:
            if (key_pressed) {
                state = IDLE;
                return 2;  // ˫��
            }
            if (++timer > time_ms) {
                state = IDLE;
                return 1;  // ����
            }
            break;
    }

    return 0;
}

/**************************************************************************
Function: USER_KEY SCAN --- ��Ϊ������
Input   : Double click the waiting time
Output  : 0��No action��1��click��2��Double click
�������ܣ�����ɨ��
��ڲ�����˫���ȴ�ʱ��
����  ֵ������״̬ 0���޶��� 1������ 2��˫��
**************************************************************************/
u8 click_N_Double_USER(u16 wait_time_ms)
{
    static enum {IDLE, FIRST_DOWN, WAIT_SECOND} state = IDLE;
    static u16 timer = 0;
    static u8 click_flag = 0;

    u8 key_down = (DL_GPIO_readPins(USER_PORT, USER_PIN_USER_PIN) == 0);  // ����������Ϊ0

    switch (state)
    {
        case IDLE:
            if (key_down)
            {
                state = FIRST_DOWN;
                timer = 0;
                click_flag = 1;
            }
            break;

        case FIRST_DOWN:
            if (!key_down)
            {
                state = WAIT_SECOND;
                timer = 0;
            }
            break;

        case WAIT_SECOND:
            if (key_down)
            {
                state = IDLE;
                click_flag = 0;
                return 2; // ˫��
            }

            timer++;
            if (timer > wait_time_ms)
            {
                state = IDLE;
                if (click_flag)
                {
                    click_flag = 0;
                    return 1; // ����
                }
            }
            break;
    }

    return 0;
}



/**************************************************************************
Function: Long press detection
Input   : none
Output  : 0��No action��1��Long press for 2 seconds��
�������ܣ��������
��ڲ�������
����  ֵ������״̬ 0���޶��� 1������2s
**************************************************************************/
u8 Long_Press(void)
{
        static u16 Long_Press_count,Long_Press;
      if(Long_Press==0&&KEY==0)  Long_Press_count++;   //������־λδ��1
    else                       Long_Press_count=0;
        if(Long_Press_count>200)        //10msɨ��һ��
      {
            Long_Press=1;
            Long_Press_count=0;
            return 1;
        }
        if(Long_Press==1)     //������־λ��1
        {
            Long_Press=0;
        }
        return 0;
}

/********
UI Control
*********/


uint8_t tuning_direction = 1;   // 1 Ϊ + �� 0 Ϊ -
TuneTarget current_param = TUNE_KP; //��ǰ������
/*******************************/

void Key(void)
{
	uint8_t key = click_N_Double(10);
	uint8_t user = click_N_Double_USER(10);
	
	// KEY ����: ��ͣС��
	if(key == 1)
	{
		if(Now.Per == PAGE_SENSOR)
		{
			Flag_Stop = !Flag_Stop;
		}else if(Now.Per == PAGE_IR_PD || Now.Per == PAGE_CAR_PID)
		{
			tuning_direction = !tuning_direction;
		}
	}else if(key == 2)  // ҳ���л�
	{
		Now.Per++;
		if(Now.Per > 2)
		{
			Now.Per = 0;
		}
	}
	
	
	if(user == 1)
	{
		if(Now.Per == PAGE_IR_PD)
		{
			if(current_param == TUNE_KP)
			{
				Kp += (tuning_direction ? 0.1f : -0.1f);
			}else if(current_param == TUNE_KD)
			{
				Kd += (tuning_direction ? 0.01f : -0.01f);
			}
			
			if(Kp < 0.0f)Kp = 0.0f;
			if(Kd < 0.0f)Kd = 0.0f;
		}else if(Now.Per == PAGE_CAR_PID)
		{
			if (current_param == TUNE_KP)
				Velcity_Kp += (tuning_direction ? 0.1f : -0.1f);
			else if (current_param == TUNE_KI)
				Velcity_Ki += (tuning_direction ? 0.01f : -0.01f);
			else if (current_param == TUNE_KD)
				Velcity_Kd += (tuning_direction ? 0.01f : -0.01f);

			if (Velcity_Kp < 0.0f) Velcity_Kp = 0.0f;
			if (Velcity_Ki < 0.0f) Velcity_Ki = 0.0f;
			if (Velcity_Kd < 0.0f) Velcity_Kd = 0.0f;			
		}
	}else if(user == 2)
	{
		if (Now.Per == PAGE_IR_PD)
		{
			current_param = (current_param == TUNE_KP) ? TUNE_KD : TUNE_KP;
			
		}
		else if (Now.Per == PAGE_CAR_PID)
		{
			current_param++;
			if (current_param > TUNE_KD)
				current_param = TUNE_KP;
		}
		
	}
	
}


