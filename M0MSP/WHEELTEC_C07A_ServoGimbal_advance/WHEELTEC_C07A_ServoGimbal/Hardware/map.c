#include "map.h"

/*********
- **舵机规格**：  
  - PWM周期：20ms (50Hz)  
  - 角度范围：0°~180°  
  - 脉宽范围：0.5ms(0°) ~ 2.5ms(180°)  
**********/

void Set_Servo_Angle(uint8_t servo_id,float angle)
{
	uint16_t pulse = MIN_PULSE + (uint16_t)((MAX_PULSE - MIN_PULSE) * angle / ANGLE_RANGE);
	
	switch(servo_id)
	{
		case 0:
			// pwm 设置
			DL_Timer_setCaptureCompareValue(PWM_0_INST,pulse,GPIO_PWM_0_C0_IDX);
			break;
		
		case 1:
			DL_Timer_setCaptureCompareValue(PWM_1_INST,pulse,GPIO_PWM_1_C1_IDX);
			break;
	}
}


//void Draw_Circle(void) {
//    for(int i = 0; i < POINT_COUNT; i++) {
//        float theta = 2 * 3.14 * i / POINT_COUNT;
//        
//        // 计算圆上点的坐标
//        float x_angle = CENTER_X + CIRCLE_RADIUS * cos(theta);
//        float y_angle = CENTER_Y + CIRCLE_RADIUS * sin(theta);
//        
//        // 限制角度范围
//        x_angle = (x_angle < 0) ? 0 : (x_angle > 180) ? 180 : x_angle;
//        y_angle = (y_angle < 0) ? 0 : (y_angle > 180) ? 180 : y_angle;
//        
//        // 更新舵机位置
//        Set_Servo_Angle(0, x_angle);  // 水平
//        Set_Servo_Angle(1, y_angle);  // 垂直
//        
//        // 控制移动速度 (约100ms/点)
//        delay_1ms(100);
//    }
//}


// 实体圆形绘制（考虑空间投影）
//void Draw_Circle_Physical(void)
//{
//    for (int i = 0; i < POINT_COUNT; i++)
//    {
//        float theta = 2.0f * PI * i / POINT_COUNT;

//        // 圆上点在纸面坐标系的位置 (单位：cm)
//        float x = CENTER_X_CM + CIRCLE_RADIUS_CM * cosf(theta);
//        float y = CENTER_Y_CM + CIRCLE_RADIUS_CM * sinf(theta);
//        float z = DISTANCE_CM;

//        // 计算舵机角度（单位：度）
//        float yaw   = atan2f(x, z) * 180.0f / PI;
//        float pitch = atan2f(y, sqrtf(x * x + z * z)) * 180.0f / PI;

//        // 映射到舵机角度范围：中心是 90°
//        float servo_x = 90.0f + yaw;
//        float servo_y = 90.0f + pitch;

//        // 限制到舵机合法角度范围
//        if (servo_x < 0) servo_x = 0;
//        if (servo_x > 180) servo_x = 180;
//        if (servo_y < 0) servo_y = 0;
//        if (servo_y > 180) servo_y = 180;

//        // 设置舵机角度
//        Set_Servo_Angle(0, servo_x);  // 水平
//        Set_Servo_Angle(1, servo_y);  // 垂直

//        delay_1ms(30);  // 可调整速度
//    }
//}

void Draw_Circle_Physical(void)
{
    // 圆心到纸的距离
    const float z = 14.0f;  // 激光到纸距离，单位：cm

    // 轨迹圆参数（纸面上的圆）
    const float r = 1.5f;           // 圆半径，单位cm（建议1~2）
    const float center_x = 0.0f;    // 圆心X位置，单位cm
    const float center_y = 0.0f;    // 圆心Y位置，单位cm

    for (int i = 0; i < POINT_COUNT; i++)
    {
        float theta = 2.0f * PI * i / POINT_COUNT;

        // 圆在纸面上的位置
        float x = center_x + r * cosf(theta);
        float y = center_y + r * sinf(theta);

        // 空间中角度换算（单位：度）
        float yaw = atan2f(x, z) * 180.0f / PI;
        float pitch = atan2f(y, sqrtf(x * x + z * z)) * 180.0f / PI;

        // 舵机角度偏移（中心角度为90°）
        float servo_x = 90.0f + yaw;
        float servo_y = 90.0f + pitch;

        // 限制范围
        if (servo_x < 0) servo_x = 0;
        if (servo_x > 180) servo_x = 180;
        if (servo_y < 0) servo_y = 0;
        if (servo_y > 180) servo_y = 180;

        // 控制舵机
        Set_Servo_Angle(0, servo_x);
        Set_Servo_Angle(1, servo_y);

        delay_1ms(30);  // 可调节运动速度
    }
}

float clamp_angle(float angle)
{
    if (angle < SERVO_MIN) return SERVO_MIN;
    if (angle > SERVO_MAX) return SERVO_MAX;
    return angle;
}


void draw_sine_wave(void)
{
	    for (int i = 0; i < STEP_COUNT; ++i)
    {
        float x = ((float)i / STEP_COUNT) * 100.0f; // 0 ~ 100mm 扫描距离
        float y = AMPLITUDE * sinf(2.0f * 3.14159f * FREQ * x);

        float theta_x = x / D; // 弧度
        float theta_y = y / D; // 弧度

        // 转换为舵机角度（单位度）
        float servo_x_angle = SERVO_CENTER + theta_x * 180.0f / 3.14159f;
        float servo_y_angle = SERVO_CENTER - theta_y * 180.0f / 3.14159f;

        // 限幅处理
        servo_x_angle = clamp_angle(servo_x_angle);
        servo_y_angle = clamp_angle(servo_y_angle);

        Set_Servo_Angle(1, servo_x_angle); // 水平舵机
        Set_Servo_Angle(0, servo_y_angle); // 垂直舵机

        delay_ms(20); // 控制绘制速度
    }
	
}

void draw_shape(Point* shape, int point_count, float distance)
{
    for (int i = 0; i < point_count - 1; i++)
    {
        Point p1 = shape[i];
        Point p2 = shape[i+1];

        // 线段插值绘制（线性过渡）
        const int segments = 30;
        for (int j = 0; j <= segments; j++)
        {
            float t = (float)j / segments;
            float x = p1.x + t * (p2.x - p1.x);
            float y = p1.y + t * (p2.y - p1.y);

            float theta_x = x / distance;
            float theta_y = y / distance;

            float servo_x = SERVO_CENTER + theta_x * 180.0f / 3.14159f;
            float servo_y = SERVO_CENTER - theta_y * 180.0f / 3.14159f;

            Set_Servo_Angle(0, clamp_angle(servo_x));
            Set_Servo_Angle(1, clamp_angle(servo_y));

            delay_ms(20); // 控制画笔移动速度
        }
    }
}



void draw_square()
{
    float x0 = 20.0f, y0 = 20.0f, L = 40.0f;

    Point square[5] = {
        {x0, y0},
        {x0 + L, y0},
        {x0 + L, y0 + L},
        {x0, y0 + L},
        {x0, y0}
    };

    draw_shape(square, 5, D);
}


// 画三角形
void draw_triangle()
{
    float x0 = 20.0f, y0 = 20.0f, L = 40.0f;
    float h = 0.866f * L; // √3/2 * L

    Point triangle[4] = {
        {x0, y0},
        {x0 + L, y0},
        {x0 + L/2.0f, y0 + h},
        {x0, y0}
    };

    draw_shape(triangle, 4, D);
}






