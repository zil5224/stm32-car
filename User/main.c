#include "stm32f10x.h"                  // Device header
#include "LEDSEG.h"
#include "Delay.h"
#include "robot.h"
#include "Incontrol.h"
#include "Irtracking.h"
#include "Irobstacle.h"
#include "Key.h"
#include "Serial.h"

/* 模式编号与数码管显示一致，1 为上电默认的循迹模式。 */
unsigned char current_mode = 1;    
void Robot_Traction()                    
{
	/* 循迹传感器返回 1 表示检测到线，返回 0 表示未检测到线。 */
	if(Left_Irtracking_Get() == 0 && Right_Irtracking_Get() == 0)
	{
		makerobo_run(70,0);   
	}
	else if(Left_Irtracking_Get() == 1 && Right_Irtracking_Get() == 0) // 左侧传感器检测到线
	{
		makerobo_Left(70,0);   
	}
	else if(Left_Irtracking_Get() == 0 && Right_Irtracking_Get() == 1) // 右侧传感器检测到线
	{
		makerobo_Right(70,0); 
	}			
	else if(Left_Irtracking_Get() == 1 && Right_Irtracking_Get() == 1) // 两侧传感器均检测到线，停车
	{
	 makerobo_brake(0); 
	}
}

void Robot_Avoidance()     // 避障模式控制函数
{
	/* 避障传感器低电平有效。转向延时结束后，下一轮主循环会重新检测障碍。 */
	if(Left_Irobstacle_Get() == 1 && Right_Irobstacle_Get() == 1)
	{
		makerobo_run(70,0);   // 左右均无障碍，前进
	}
	else if(Left_Irobstacle_Get() == 1 && Right_Irobstacle_Get() == 0)
	{
		makerobo_Left(70,400);   // 右侧检测到障碍，向左避让
	}
	else if(Left_Irobstacle_Get() == 0 && Right_Irobstacle_Get() == 1)
	{
		makerobo_Right(70,400);  // 左侧检测到障碍，向右避让
	}
	else if(Left_Irobstacle_Get() == 0 && Right_Irobstacle_Get() == 0)
	{
		makerobo_brake(400);          // 两侧均有障碍，先停止
		makerobo_Spin_Right(70,900);  // 再原地右转
	}
}

/*
 * 以下按键值对应驱动层按接收顺序左移组装后的 NEC 命令字节。
 * 红外驱动层已经完成反码校验，这里只负责“命令字节 -> 动作函数”。
 */
#define IR_KEY_FORWARD   0x62
#define IR_KEY_BACK      0xA8
#define IR_KEY_LEFT      0x22
#define IR_KEY_RIGHT     0xC2
#define IR_KEY_STOP      0x02

void ControlCar_Ircontrol(void)
{
	uint8_t command_byte;

	/* 没有新帧时不更新 PWM，小车保持上一条命令的运动状态。 */
	if(IR_Receiveflag == 0)
	{
		return;
	}

	/* 消费本帧。低 8 位是命令反码，右移 8 位后取实际命令。 */
	IR_Receiveflag = 0;
	command_byte = (uint8_t)(IR_Receivecode >> 8);

	printf("IR code: %08lX, command: %02X\r\n",
	       (unsigned long)IR_Receivecode, command_byte);
	IR_Receivecode = 0;

	/*
	 * 遥控动作使用 time=0：动作函数只更新 PWM，马上返回主循环，
	 * 这样下一次循环才能及时接收“停止”或另一个方向命令。
	 */
	if(command_byte == IR_KEY_FORWARD)
	{
		makerobo_run(70, 0);
	}
	else if(command_byte == IR_KEY_BACK)
	{
		makerobo_back(70, 0);
	}
	else if(command_byte == IR_KEY_LEFT)
	{
		makerobo_Spin_Left(50, 0);
	}
	else if(command_byte == IR_KEY_RIGHT)
	{
		makerobo_Spin_Right(50, 0);
	}
	else if(command_byte == IR_KEY_STOP)
	{
		makerobo_brake(0);
	}
	else
	{
		/* 未配置的按键采取安全策略：停止电机。 */
		makerobo_brake(0);
	}
}

void Irscan(void)
{
	/* 一次循环只尝试接收一帧，不再在这里永久 while(1) 阻塞主程序。 */
	if(IRremote_Scan() == 1)
	{
		ControlCar_Ircontrol();
	}
}

int main(void)
{

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  
	Key_Init();
	IRremote_Init();           
	Irtracking_Init();         
	Irobstacle_Init();         
	LEDSEG_Init();
	Serial_Init();             
	robot_Init();              
	while (1)
	{
		/* 按键事件让模式按 1、2、3 的顺序循环。 */
		if(Key_GetNum() == 1)
		{
			current_mode++;
		}
		if(current_mode > 3)
		{
			current_mode = 1;
		}
		switch(current_mode)
		{
			case 1: Digital_Display(1);Robot_Traction();break;
			case 2: Digital_Display(2);Robot_Avoidance();break;
			case 3: Digital_Display(3);Irscan();break;
		}
	}
}
