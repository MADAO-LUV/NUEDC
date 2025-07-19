#include "key.h"

Key_V Now = {0,0};

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
                return 2;  // 双击
            }
            if (++timer > time_ms) {
                state = IDLE;
                return 1;  // 单击
            }
            break;
    }

    return 0;
}

/**************************************************************************
Function: USER_KEY SCAN --- 因为是上拉
Input   : Double click the waiting time
Output  : 0：No action；1：click；2：Double click
函数功能：按键扫描
入口参数：双击等待时间
返回  值：按键状态 0：无动作 1：单击 2：双击
**************************************************************************/
u8 click_N_Double_USER(u16 wait_time_ms)
{
    static enum {IDLE, FIRST_DOWN, WAIT_SECOND} state = IDLE;
    static u16 timer = 0;
    static u8 click_flag = 0;

    u8 key_down = (DL_GPIO_readPins(USER_PORT, USER_PIN_USER_PIN) == 0);  // 上拉，按下为0

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
                return 2; // 双击
            }

            timer++;
            if (timer > wait_time_ms)
            {
                state = IDLE;
                if (click_flag)
                {
                    click_flag = 0;
                    return 1; // 单击
                }
            }
            break;
    }

    return 0;
}



/**************************************************************************
Function: Long press detection
Input   : none
Output  : 0：No action；1：Long press for 2 seconds；
函数功能：长按检测
入口参数：无
返回  值：按键状态 0：无动作 1：长按2s
**************************************************************************/
u8 Long_Press(void)
{
        static u16 Long_Press_count,Long_Press;
      if(Long_Press==0&&KEY==0)  Long_Press_count++;   //长按标志位未置1
    else                       Long_Press_count=0;
        if(Long_Press_count>200)        //10ms扫描一次
      {
            Long_Press=1;
            Long_Press_count=0;
            return 1;
        }
        if(Long_Press==1)     //长按标志位置1
        {
            Long_Press=0;
        }
        return 0;
}

/********
UI Control
*********/


uint8_t tuning_direction = 1;   // 1 为 + 则 0 为 -
TuneTarget current_param = TUNE_KP; //当前调参数
/*******************************/

void Key(void)
{
	uint8_t key = click_N_Double(10);
	uint8_t user = click_N_Double_USER(10);
	
	// KEY 单击: 启停小车
	if(key == 1)
	{
		if(Now.Per == PAGE_SENSOR)
		{
			Flag_Stop = !Flag_Stop;
		}else if(Now.Per == PAGE_IR_PD || Now.Per == PAGE_CAR_PID || Now.Per == PAGE_ANGLE)
		{
			tuning_direction = !tuning_direction;
		}
	}else if(key == 2)  // 页面切换
	{
		Now.Per++;
		if(Now.Per > 3)
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
		}else if(Now.Per == PAGE_SENSOR)
		{
			baes_speed += (tuning_direction ? 1 : -1);
		}else if(Now.Per == PAGE_ANGLE)
		{
			if (current_param == TUNE_KP)
				Angle_Yaw.Kp += (tuning_direction ? 1.5f : -1.5f);
			else if (current_param == TUNE_KI)
				Angle_Yaw.Ki += (tuning_direction ? 0.01f : -0.01f);
			else if (current_param == TUNE_KD)
				Angle_Yaw.Kd += (tuning_direction ? 0.01f : -0.01f);

			if (Angle_Yaw.Kp < 0.0f) Angle_Yaw.Kp = 0.0f;
			if (Angle_Yaw.Ki < 0.0f) Angle_Yaw.Ki = 0.0f;
			if (Angle_Yaw.Kd < 0.0f) Angle_Yaw.Kd = 0.0f;			
			
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
		}else if(Now.Per == PAGE_SENSOR)
		{
			tuning_direction = !tuning_direction;
		}
		else if (Now.Per == PAGE_ANGLE)
		{
			current_param++;
			if (current_param > TUNE_KD)
				current_param = TUNE_KP;
		}
	}
	
}


