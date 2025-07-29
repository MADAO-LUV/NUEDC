#ifndef __MAP_H__
#define __MAP_H__

#include "ti_msp_dl_config.h"
#include "board.h"

#define MIN_PULSE    500     // 0.5ms脉冲宽度
#define MAX_PULSE    2500    // 2.5ms脉冲宽度
#define ANGLE_RANGE  180.0f  // 舵机角度范围

// 云台参数
#define CENTER_X       45.0f     // 舵机中心角度：水平方向
#define CENTER_Y        45.0f     // 舵机中心角度：垂直方向
#define CIRCLE_RADIUS   5.0f     // 角度半径：控制实际偏转范围

#define POINT_COUNT     360     // 圆形离散点数
#define PI              3.1415926f

// 画圆参数（单位：cm）
#define CIRCLE_RADIUS_CM    0.5f     // 纸面上的圆半径
#define DISTANCE_CM         5.0f    // 激光到纸面的垂直距离
#define CENTER_X_CM         0.0f
#define CENTER_Y_CM         0.0f


#define D 50.0f         // 云台到平面距离，单位mm
#define AMPLITUDE 10.0f  // 正弦波振幅，单位mm
#define FREQ 0.05f       // 正弦波频率
#define STEP_COUNT 200   // 步进数（X方向采样数）

// 假设舵机角度范围为 0°~180°，中心为90°
#define SERVO_MIN 0
#define SERVO_MAX 180
#define SERVO_CENTER 90

typedef struct
{
	float x;
	float y;
}Point;


void Set_Servo_Angle(uint8_t servo_id,float angle);
void Draw_Circle_Physical(void);
void draw_sine_wave(void);
void draw_shape(Point* shape, int point_count, float distance);

void draw_square(void);

// 画三角形
void draw_triangle(void);



#endif