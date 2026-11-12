#ifndef __ROBOT_H
#define __ROBOT_H

void robot_Init(void);
void robot_speed(uint8_t left1_speed,uint8_t left2_speed,uint8_t right1_speed,uint8_t right2_speed);
// 小车运动控制
void makerobo_run(uint8_t speed,uint16_t time); // 前进
void makerobo_brake(uint16_t time);             // 刹车
void makerobo_Left(int8_t speed,uint16_t time); // 左转
void makerobo_Spin_Left(int8_t speed,uint16_t time);  // 原地左转
void makerobo_Right(int8_t speed,uint16_t time);      // 右转
void makerobo_Spin_Right(int8_t speed,uint16_t time); // 原地右转
void makerobo_back(int8_t speed,uint16_t time);      // 后退
void makerobo_Left45(uint8_t left_speed,uint8_t right_speed,uint16_t time);
void makerobo_Right45(uint8_t left_speed,uint8_t right_speed,uint16_t time);

#endif

