#ifndef __INCONTROL_H
#define __INCONTROL_H

void IRremote_Init(void);      // 初始化红外接收模块
/*
 * 轮询读取一帧红外数据。
 * 返回 1 表示收到并校验通过一帧完整数据，返回 0 表示当前没有完整数据。
 * 这里选择轮询方案，避免“外部中断”和“主循环轮询”同时解析同一条信号。
 */
uint8_t IRremote_Scan(void);

// 红外接收状态和帧数据
extern uint32_t IR_Receivecode;
extern uint8_t  IR_Receiveflag;

#endif

