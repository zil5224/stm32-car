#include "stm32f10x.h"                  // Device header
#include "PWM.h"
#include "Delay.h"
// 初始化电机 PWM
void robot_Init(void)
{
	PWM_Init(); 
}

// 更新 TIM4 的四路 PWM 比较值
void robot_speed(uint8_t left1_speed,uint8_t left2_speed,uint8_t right1_speed,uint8_t right2_speed)
{	
	    TIM_SetCompare1(TIM4,left1_speed);
      TIM_SetCompare2(TIM4,left2_speed);
      TIM_SetCompare3(TIM4,right1_speed);
      TIM_SetCompare4(TIM4,right2_speed);
}

// 小车运动控制
// 左右电机正转，小车前进
void makerobo_run(int8_t speed,uint16_t time)  // 前进
{
      if(speed > 100)
			{
				speed = 100;
			}
			if(speed < 0)
			{
				speed = 0;
			}
	    robot_speed(speed,0,speed,0);
			Delay_ms(time);                 // 延时，单位为毫秒
			//robot_speed(0,0,0,0);           // 停止电机
 
}

void makerobo_brake(uint16_t time) // 刹车
{
		robot_speed(0,0,0,0);     // 停止电机
		Delay_ms(time);          // 延时，单位为毫秒
}

void makerobo_Left(int8_t speed,uint16_t time) // 左转
{
	    if(speed > 100)
			{
				speed = 100;
			}
			if(speed < 0)
			{
				speed = 0;
			}
		robot_speed(0,0,speed,0);
		Delay_ms(time);                 // 延时，单位为毫秒
	  //robot_speed(0,0,0,0);           // 停止电机

}

void makerobo_Spin_Left(int8_t speed,uint16_t time) // 原地左转
{
		  if(speed > 100)
			{
				speed = 100;
			}
			if(speed < 0)
			{
				speed = 0;
			}  
		robot_speed(0,speed,speed,0);
		Delay_ms(time);                    // 延时，单位为毫秒
    //robot_speed(0,0,0,0);           // 停止电机
}

void makerobo_Right(int8_t speed,uint16_t time) // 右转
{
	    if(speed > 100)
			{
				speed = 100;
			}
			if(speed < 0)
			{
				speed = 0;
			}
		robot_speed(speed,0,0,0);
		Delay_ms(time);                 // 延时，单位为毫秒
	  //robot_speed(0,0,0,0);           // 停止电机

}

void makerobo_Spin_Right(int8_t speed,uint16_t time) // 原地右转
{
		  if(speed > 100)
			{
				speed = 100;
			}
			if(speed < 0)
			{
				speed = 0;
			}  
		robot_speed(speed,0,0,speed);
		Delay_ms(time);                    // 延时，单位为毫秒
    //robot_speed(0,0,0,0);           // 停止电机
}

void makerobo_back(int8_t speed,uint16_t time)  // 后退
{
      if(speed > 100)
			{
				speed = 100;
			}
			if(speed < 0)
			{
				speed = 0;
			}
	    robot_speed(0,speed,0,speed);
			Delay_ms(time);                 // 延时，单位为毫秒
			//robot_speed(0,0,0,0);           // 停止电机
 
}
void makerobo_Left45(uint8_t left_speed,uint8_t right_speed,uint16_t time)
{
    robot_speed(left_speed,0,right_speed,0);

    Delay_ms(time);
}

void makerobo_Right45(uint8_t left_speed,uint8_t right_speed,uint16_t time)
{
    robot_speed(left_speed,0,right_speed,0);

    Delay_ms(time);
}
